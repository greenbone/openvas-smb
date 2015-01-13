###################################################
# DCOM parser for Samba
# Basically the glue between COM and DCE/RPC with NDR
# Copyright jelmer@samba.org 2003-2005
# released under the GNU GPL

package Parse::Pidl::Samba4::COM::Proxy;

use Parse::Pidl::Samba4::COM::Header;
use Parse::Pidl::Util qw(has_property);
use Parse::Pidl::Typelist qw(mapType);

use vars qw($VERSION);
$VERSION = '0.01';

use strict;

my($res);

sub ParseVTable($$)
{
	my $interface = shift;
	my $name = shift;

	# Generate the vtable
	$res .="\tstruct $interface->{NAME}_vtable $name = {";

	if (defined($interface->{BASE})) {
		$res .= "\n\t\t{},";
	}

	my $data = $interface->{DATA};

	foreach my $d (@{$data}) {
		if ($d->{TYPE} eq "FUNCTION") {
		    $res .= "\n\t\tdcom_proxy_$interface->{NAME}_$d->{NAME}";
			$res .= ",";
		}
	}

	$res .= "\n\t};\n\n";
}

sub ParseRegFunc($)
{
	my $interface = shift;

	$res .= "\nNTSTATUS dcom_proxy_$interface->{NAME}_init(void)
{
	struct $interface->{NAME}_vtable *proxy_vtable = talloc(talloc_autofree_context(), struct $interface->{NAME}_vtable);
";

	if (defined($interface->{BASE})) {
		$res.= "
	struct GUID base_iid;
	const void *base_vtable;

	base_iid = dcerpc_table_$interface->{BASE}.syntax_id.uuid;

	base_vtable = dcom_proxy_vtable_by_iid(&base_iid);
	if (base_vtable == NULL) {
		DEBUG(0, (\"No proxy registered for base interface '$interface->{BASE}'\\n\"));
		return NT_STATUS_FOOBAR;
	}
	
	memcpy(proxy_vtable, base_vtable, sizeof(struct $interface->{BASE}_vtable));

";
	}
	foreach my $x (@{$interface->{DATA}}) {
		next unless ($x->{TYPE} eq "FUNCTION");

#		$res .= "\tproxy_vtable->$x->{NAME} = dcom_proxy_$interface->{NAME}_$x->{NAME};\n";
		$res .= "\tproxy_vtable->$x->{NAME}_send = dcom_proxy_$interface->{NAME}_$x->{NAME}_send;\n";
	}

	$res.= "
	proxy_vtable->iid = dcerpc_table_$interface->{NAME}.syntax_id.uuid;

	return dcom_register_proxy((struct IUnknown_vtable *)proxy_vtable);
}\n\n";
}

#####################################################################
# parse a function
sub ParseFunction($$)
{
	my $interface = shift;
	my $fn = shift;
	my $name = $fn->{NAME};
	my $short_name = ($fn->{NAME} =~ /^$interface->{NAME}_(.*)$/) ? $1 : $fn->{NAME};
	my $uname = uc $name;
	my $args_in = 0;
	my $args_out = 0;
	my $if_in = 0;
	my $if_out = 0;

	foreach my $a (@{$fn->{ELEMENTS}}) {
		$args_in++ if has_property($a, "in");
		$args_out++ if has_property($a, "out");
		$if_in++ if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE") && has_property($a, "in"));
		$if_out++ if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE") && has_property($a, "out"));
	}
	$args_out++ if $fn->{RETURN_TYPE} ne "void";

	#### declarations ############################################
	$res.="
static void dcom_proxy_$interface->{NAME}_${name}_recv_rpc(struct rpc_request *req);
";
	##### *_out struct ###########################################
	if ($args_out > 0) {
		$res.="
struct $interface->{NAME}_${name}_out {
";
		foreach my $a (@{$fn->{ELEMENTS}}) {
			next unless (has_property($a, "out"));
			my $decl = Parse::Pidl::Samba4::COM::Header::GetArgumentProto($a);
			$decl =~ s/ \*/ /;
			$res.= "\t" . $decl . ";\n";
		}
		if ($fn->{RETURN_TYPE} ne "void") {
			$res.= "\t" . mapType($fn->{RETURN_TYPE}) . " result;\n";
		}
		$res.="};\n";
	}

	#### *_send ##################################################
	$res.="
static struct composite_context *dcom_proxy_$interface->{NAME}_${name}_send(struct $interface->{NAME} *d, TALLOC_CTX *mem_ctx" . Parse::Pidl::Samba4::COM::Header::GetArgumentProtoList($fn, "in") . ")
{
        struct composite_context *c, *c_pipe;
        struct dcom_proxy_async_call_state *s;
        struct ${name} *r;
" . ($if_in > 0 ? "\tNTSTATUS status;\n" : "" ) . "
        c = composite_create(mem_ctx, d->ctx->event_ctx);
        if (c == NULL) return NULL;

        s = talloc_zero(c, struct dcom_proxy_async_call_state);
        if (composite_nomem(s, c)) return c;
        c->private_data = s;

        r = talloc_zero(s, struct ${name});
        if (composite_nomem(r, c)) return c;

        s->d = (struct IUnknown *)d;
        s->table = &dcerpc_table_$interface->{NAME};
        s->opnum = DCERPC_$uname;
        s->continuation = dcom_proxy_$interface->{NAME}_${name}_recv_rpc;
        s->mem_ctx = mem_ctx;
        s->r = r;

        r->in.ORPCthis.version.MajorVersion = COM_MAJOR_VERSION;
        r->in.ORPCthis.version.MinorVersion = COM_MINOR_VERSION;
        r->in.ORPCthis.cid = GUID_random();
";
	# Put arguments into r
	foreach my $a (@{$fn->{ELEMENTS}}) {
		next unless (has_property($a, "in"));
		if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE")) {
			my $varname;
			my $ctx = "mem_ctx";
			my $n;
			foreach $n (0..$a->{POINTERS}-1) {
				$res .=sprintf(("\t" x $n)."\tif (%s$a->{NAME}) {\n", ("*" x $n));
				$varname = ("*" x $n) . "r->in.$a->{NAME}";
				$res .=sprintf(("\t" x $n)."\t\t$varname = talloc_zero($ctx, struct MInterfacePointer%s);\n", ("*" x ($a->{POINTERS}-$n-1)));
				$ctx = $varname;
			}
			$n = $a->{POINTERS}-1;
			$res .=("\t" x $n)."\t\t(${varname})->size = sizeof(struct OBJREF);\n";
			$res .=sprintf(("\t" x $n)."\t\tstatus = dcom_OBJREF_from_IUnknown(&(*$varname).obj, (struct IUnknown *)%s$a->{NAME});\n", ("*" x ($a->{POINTERS}-1)));
			$res .= ("\t" x $n)."\t\tif (!NT_STATUS_IS_OK(status)) {\n"
                             .  ("\t" x $n)."\t\t\tcomposite_error(c, NT_STATUS_RPC_NT_CALL_FAILED);\n"
                             .  ("\t" x $n)."\t\t\treturn c;\n"
                             .  ("\t" x $n)."\t\t}\n";
			for ($n =$a->{POINTERS}; $n > 0; --$n) {
				$res .=("\t" x $n)."}\n";
			}
		} else {
			$res .= "\tr->in.$a->{NAME} = $a->{NAME};\n";
		}
	}

	$res .="
        if (DEBUGLVL(12)) {
                NDR_PRINT_IN_DEBUG(${name}, r);
        }

        c_pipe = dcom_get_pipe_send((struct IUnknown *)d, mem_ctx);
        composite_continue(c, c_pipe, dcom_proxy_async_call_recv_pipe_send_rpc, c);

        return c;
}
";

	#### *_recv_rpc ##############################################
        $res.="
static void dcom_proxy_$interface->{NAME}_${name}_recv_rpc(struct rpc_request *req)
{
        struct composite_context *c;
        struct dcom_proxy_async_call_state *s;
        struct $interface->{NAME}_${name}_out *out;
        struct ${name} *r;
        struct ORPCTHAT that;
        NTSTATUS status;
";
	foreach my $a (@{$fn->{ELEMENTS}}) {
		next unless (has_property($a, "out"));
		if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE")) {
			$res.="\tstruct MInterfacePointer *mip_$a->{NAME};\n";
		}
	}

	$res.="
        c = req->async.private;
        s = c->private_data;
        r = s->r;
        out = talloc_zero(c, struct $interface->{NAME}_${name}_out);
        if (composite_nomem(out, c)) return;
        c->private_data = out;

        r->out.ORPCthat = &that;
";
	foreach $a (@{$fn->{ELEMENTS}}) {
		next unless (has_property($a, "out"));

		if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE")) {
			$res.="\tr->out.$a->{NAME} = &mip_$a->{NAME};\n";
		} else {
			$res.="\tr->out.$a->{NAME} = &out->$a->{NAME};\n";
		}
	}
	
	$res.="
        status = dcerpc_ndr_request_recv(req);
        if (!NT_STATUS_IS_OK(status)) {
                composite_error(c, NT_STATUS_RPC_NT_CALL_FAILED);
                return;
        }

        if (DEBUGLVL(12)) {
                NDR_PRINT_OUT_DEBUG(${name}, r);
        }
";
	foreach $a (@{$fn->{ELEMENTS}}) {
		next unless (has_property($a, "out"));

		if (Parse::Pidl::Typelist::typeIs($a->{TYPE}, "INTERFACE")) {
			$res .=
"	if (r->out.$a->{NAME} && *r->out.$a->{NAME}) {
		status = dcom_IUnknown_from_OBJREF(s->d->ctx, (struct IUnknown **)&out->$a->{NAME}, &(**r->out.$a->{NAME}).obj);
		if (*r->out.$a->{NAME}) talloc_free(*r->out.$a->{NAME});
	} else {
		out->$a->{NAME} = NULL;
	}
";
		} elsif (defined($a->{POINTERS}) and $a->{POINTERS} > 1) {
			$res .=
"	talloc_steal(s->mem_ctx, out->$a->{NAME});
";
		}
	}

	if ($fn->{RETURN_TYPE} ne "void") {
		$res.="\tout->result = r->out.result;\n";
	}
	$res .="
        talloc_free(s);
        composite_done(c);
}
";

	#### *_recv ##################################################
	$res.="
" . mapType($fn->{RETURN_TYPE}) . " $interface->{NAME}_${name}_recv(struct composite_context *c" . Parse::Pidl::Samba4::COM::Header::GetArgumentProtoList($fn, "out") . ")
{
        struct $interface->{NAME}_${name}_out *out;
        NTSTATUS status;
" . (($fn->{RETURN_TYPE} ne "void") ? "\t" . mapType($fn->{RETURN_TYPE}) . " result;\n" : "") . "
        status = composite_wait(c);
        if (!NT_STATUS_IS_OK(status)) {
                talloc_free(c);
                return";
	if ($fn->{RETURN_TYPE} eq "WERROR") {
		$res.=" ntstatus_to_werror(NT_STATUS_RPC_NT_CALL_FAILED)";
	} elsif($fn->{RETURN_TYPE} eq "uint32") {
		$res.=" 0";
	}
        $res .= ";
	}
        out = c->private_data;

";
	foreach $a (@{$fn->{ELEMENTS}}) {
		next unless (has_property($a, "out"));
		$res.="\tif ($a->{NAME}) *$a->{NAME} = out->$a->{NAME};\n";
	}
	if ($fn->{RETURN_TYPE} ne "void") {
		$res.="\tresult = out->result;\n";
	}
	$res .= "
        talloc_free(c);
" . (($fn->{RETURN_TYPE} ne "void") ? "\treturn result;" : "") . "
}
";
	
	#### * sync ##################################################
	$res.="
" . mapType($fn->{RETURN_TYPE}) . " $interface->{NAME}_${short_name}(struct $interface->{NAME} *d, TALLOC_CTX *mem_ctx" . Parse::Pidl::Samba4::COM::Header::GetArgumentProtoList($fn) . ")
{
        struct composite_context *c;

	c = $interface->{NAME}_${short_name}_send(d, mem_ctx" . Parse::Pidl::Samba4::COM::Header::GetArgumentList($fn, "in") . ");
	if (c == NULL) return";
	if ($fn->{RETURN_TYPE} eq "WERROR") {
		$res.=" WERR_NOMEM";
	} elsif($fn->{RETURN_TYPE} eq "uint32") {
		$res.=" 0";
	}
	$res.=";

        " . (($fn->{RETURN_TYPE} ne "void") ? "return " : "") . "$interface->{NAME}_${name}_recv(c" . Parse::Pidl::Samba4::COM::Header::GetArgumentList($fn, "out") . ");
}
";

}

#####################################################################
# parse the interface definitions
sub ParseInterface($)
{
	my($interface) = shift;
	my($data) = $interface->{DATA};
	$res = "/* DCOM proxy for $interface->{NAME} generated by pidl */\n\n";
	foreach my $d (@{$data}) {
		($d->{TYPE} eq "FUNCTION") && 
		ParseFunction($interface, $d);
	}

	ParseRegFunc($interface);
}

sub RegistrationFunction($$)
{
	my $idl = shift;
	my $basename = shift;

	my $res = "\n\nNTSTATUS dcom_$basename\_init(void)\n";
	$res .= "{\n";
	$res .="\tNTSTATUS status = NT_STATUS_OK;\n";
	foreach my $interface (@{$idl}) {
		next if $interface->{TYPE} ne "INTERFACE";
		next if not has_property($interface, "object");

		my $data = $interface->{DATA};
		my $count = 0;
		foreach my $d (@{$data}) {
			if ($d->{TYPE} eq "FUNCTION") { $count++; }
		}

		next if ($count == 0);

		$res .= "\tstatus = dcom_$interface->{NAME}_init();\n";
		$res .= "\tif (NT_STATUS_IS_ERR(status)) {\n";
		$res .= "\t\treturn status;\n";
		$res .= "\t}\n\n";
	}
	$res .= "\treturn status;\n";
	$res .= "}\n\n";

	return $res;
}

sub Parse($$)
{
	my ($pidl,$comh_filename) = @_;
	my $res = "";

	$res .=	"#include \"includes.h\"\n" .
                        "#include \"librpc/rpc/dcerpc.h\"\n" .
			"#include \"lib/com/dcom/dcom.h\"\n" .
			"#include \"$comh_filename\"\n" .
			"#include \"libcli/composite/composite.h\"\n";


	foreach (@{$pidl}) {
		next if ($_->{TYPE} ne "INTERFACE");
		next if has_property($_, "local");
		next unless has_property($_, "object");

		$res .= ParseInterface($_);
	}

	return $res;
}

1;
