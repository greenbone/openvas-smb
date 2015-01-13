# COM Header generation
# (C) 2005 Jelmer Vernooij <jelmer@samba.org>

package Parse::Pidl::Samba4::COM::Header;

use Parse::Pidl::Typelist qw(mapType);
use Parse::Pidl::Util qw(has_property is_constant);

use vars qw($VERSION);
$VERSION = '0.01';

use strict;

sub HeaderType($)
{
	my($e) = @_;
	if (has_property($e, "charset")) {
		return "const char";
	} else {
		return mapType($e->{TYPE});
	}
}

sub GetArgumentProto($)
{
	my $a = shift;
	my $res = HeaderType($a) . " ";

	my $l = $a->{POINTERS};
	$l-- if (Parse::Pidl::Typelist::scalar_is_reference($a->{TYPE}));
	foreach my $i (1..$l) {
		$res .= "*";
	}

	if (defined $a->{ARRAY_LEN}[0] && !is_constant($a->{ARRAY_LEN}[0]) &&
	!$a->{POINTERS}) {
		$res .= "*";
	}
	$res .= $a->{NAME};
	if (defined $a->{ARRAY_LEN}[0] && is_constant($a->{ARRAY_LEN}[0])) {
		$res .= "[$a->{ARRAY_LEN}[0]]";
	}
	return $res;
}

sub GetArgumentProtoList
{
	my ($f,$filter) = @_;
	my $res = "";

	foreach my $a (@{$f->{ELEMENTS}}) {
		next if defined($filter) && !has_property($a, $filter);
		$res .= ", " . GetArgumentProto($a);
	}

	return $res;
}

sub GetArgumentList
{
	my ($f,$filter) = @_;
	my $res = "";

	foreach (@{$f->{ELEMENTS}}) { 
		next if defined($filter) && !has_property($_, $filter);
		$res .= ", $_->{NAME}";
	}

	return $res;
}

#####################################################################
# generate vtable structure for COM interface
sub HeaderVTable($)
{
	my $interface = shift;
	my $res;
	$res .= "#define " . uc($interface->{NAME}) . "_METHODS \\\n";
	if (defined($interface->{BASE})) {
		$res .= "\t" . uc($interface->{BASE} . "_METHODS") . "\\\n";
	}

	my $data = $interface->{DATA};
	foreach my $d (@{$data}) {
		if ($d->{TYPE} eq "FUNCTION") {
#			$res .= "\t" . mapType($d->{RETURN_TYPE}) . " (*$d->{NAME}) (struct $interface->{NAME} *d, TALLOC_CTX *mem_ctx" . GetArgumentProtoList($d) . ");\\\n";
			$res .= "\tstruct composite_context *(*$d->{NAME}_send) (struct $interface->{NAME} *d, TALLOC_CTX *mem_ctx" . GetArgumentProtoList($d, "in") . ");\\\n";
		}
	}
	$res .= "\n";
	$res .= "struct $interface->{NAME}_vtable {\n";
	$res .= "\tstruct GUID iid;\n";
	$res .= "\t" . uc($interface->{NAME}) . "_METHODS\n";
	$res .= "};\n\n";

	return $res;
}

sub ParseInterface($)
{
	my $if = shift;
	my $res;

	$res .="\n\n/* $if->{NAME} */\n";

	$res .="#define COM_" . uc($if->{NAME}) . "_UUID $if->{PROPERTIES}->{uuid}\n\n";

	$res .="struct $if->{NAME}_vtable;\n\n";

	$res .="struct $if->{NAME} {
	struct OBJREF obj;
	struct com_context *ctx;
	struct $if->{NAME}_vtable *vtable;
	void *object_data;
};\n\n";

	$res.=HeaderVTable($if);

	foreach my $d (@{$if->{DATA}}) {
		next if ($d->{TYPE} ne "FUNCTION");

		my $defname = ($d->{NAME} =~ /^$if->{NAME}_(.*)$/) ? $1 : $d->{NAME};
#		$res .= "#define $if->{NAME}_${defname}(interface, mem_ctx" . GetArgumentList($d) . ") ";
#		$res .= "((interface)->vtable->$d->{NAME}(interface, mem_ctx" . GetArgumentList($d) . "))";
		$res .= mapType($d->{RETURN_TYPE}) . " $if->{NAME}_${defname}(struct $if->{NAME} *interface, TALLOC_CTX *mem_ctx" . GetArgumentProtoList($d) . ");\n";
		$res .= "#define $if->{NAME}_${defname}_send(interface, mem_ctx" . GetArgumentList($d, "in") . ") ";
		$res .= "((interface)->vtable->$d->{NAME}_send(interface, mem_ctx" . GetArgumentList($d, "in") . "))";
		$res .="\n";
#		$res .= "#define $if->{NAME}_${defname}_recv(c" . GetArgumentList($d, "out") . ") ";
#		$res .= "((interface)->vtable->$d->{NAME}_recv(c" . GetArgumentList($d, "out") . "))";
		$res .= mapType($d->{RETURN_TYPE}) . " $if->{NAME}_${defname}_recv(struct composite_context *c" . GetArgumentProtoList($d, "out") . ");\n";
		$res .="\n";
	}
	$res .= "\nNTSTATUS dcom_proxy_$if->{NAME}_init(void);\n";

	return $res;
}

sub ParseCoClass($)
{
	my $c = shift;
	my $res = "";
	$res .= "#define CLSID_" . uc($c->{NAME}) . " $c->{PROPERTIES}->{uuid}\n";
	if (has_property($c, "progid")) {
		$res .= "#define PROGID_" . uc($c->{NAME}) . " $c->{PROPERTIES}->{progid}\n";
	}
	$res .= "\n";
	return $res;
}

sub Parse($$)
{
	my ($idl,$ndr_header) = @_;
	my $res = "";

	$res .= "#include \"librpc/gen_ndr/orpc.h\"\n" . 
			"#include \"$ndr_header\"\n\n";

        foreach (@{$idl})
        {
                if ($_->{TYPE} eq "INTERFACE" && has_property($_, "object")) {
			$res.="struct $_->{NAME};\n";
        		foreach my $s (@{$_->{DATA}}) {
				next if ($s->{TYPE} ne "TYPEDEF" || defined($s->{DATA_TYPE}) || $s->{DATA}{TYPE} ne "STRUCT");
            			$res.="struct $s->{NAME};\n";
            		}
    		}
	}

	foreach (@{$idl})
	{
		if ($_->{TYPE} eq "INTERFACE" && has_property($_, "object")) {
			$res.=ParseInterface($_);
		} 

		if ($_->{TYPE} eq "COCLASS") {
			$res.=ParseCoClass($_);
		}
	}

	return $res;
}

1;
