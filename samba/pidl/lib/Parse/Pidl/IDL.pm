####################################################################
#
#    This file was generated using Parse::Yapp version 1.05.
#
#        Don't edit this file, use source file instead.
#
#             ANY CHANGE MADE HERE WILL BE LOST !
#
####################################################################
package Parse::Pidl::IDL;
use vars qw ( @ISA );
use strict;

@ISA= qw ( Parse::Yapp::Driver );
#Included Parse/Yapp/Driver.pm file----------------------------------------
{
#
# Module Parse::Yapp::Driver
#
# This module is part of the Parse::Yapp package available on your
# nearest CPAN
#
# Any use of this module in a standalone parser make the included
# text under the same copyright as the Parse::Yapp module itself.
#
# This notice should remain unchanged.
#
# (c) Copyright 1998-2001 Francois Desarmenien, all rights reserved.
# (see the pod text in Parse::Yapp module for use and distribution rights)
#

package Parse::Yapp::Driver;

require 5.004;

use strict;

use vars qw ( $VERSION $COMPATIBLE $FILENAME );

$VERSION = '1.05';
$COMPATIBLE = '0.07';
$FILENAME=__FILE__;

use Carp;

#Known parameters, all starting with YY (leading YY will be discarded)
my(%params)=(YYLEX => 'CODE', 'YYERROR' => 'CODE', YYVERSION => '',
			 YYRULES => 'ARRAY', YYSTATES => 'ARRAY', YYDEBUG => '');
#Mandatory parameters
my(@params)=('LEX','RULES','STATES');

sub new {
    my($class)=shift;
	my($errst,$nberr,$token,$value,$check,$dotpos);
    my($self)={ ERROR => \&_Error,
				ERRST => \$errst,
                NBERR => \$nberr,
				TOKEN => \$token,
				VALUE => \$value,
				DOTPOS => \$dotpos,
				STACK => [],
				DEBUG => 0,
				CHECK => \$check };

	_CheckParams( [], \%params, \@_, $self );

		exists($$self{VERSION})
	and	$$self{VERSION} < $COMPATIBLE
	and	croak "Yapp driver version $VERSION ".
			  "incompatible with version $$self{VERSION}:\n".
			  "Please recompile parser module.";

        ref($class)
    and $class=ref($class);

    bless($self,$class);
}

sub YYParse {
    my($self)=shift;
    my($retval);

	_CheckParams( \@params, \%params, \@_, $self );

	if($$self{DEBUG}) {
		_DBLoad();
		$retval = eval '$self->_DBParse()';#Do not create stab entry on compile
        $@ and die $@;
	}
	else {
		$retval = $self->_Parse();
	}
    $retval
}

sub YYData {
	my($self)=shift;

		exists($$self{USER})
	or	$$self{USER}={};

	$$self{USER};
	
}

sub YYErrok {
	my($self)=shift;

	${$$self{ERRST}}=0;
    undef;
}

sub YYNberr {
	my($self)=shift;

	${$$self{NBERR}};
}

sub YYRecovering {
	my($self)=shift;

	${$$self{ERRST}} != 0;
}

sub YYAbort {
	my($self)=shift;

	${$$self{CHECK}}='ABORT';
    undef;
}

sub YYAccept {
	my($self)=shift;

	${$$self{CHECK}}='ACCEPT';
    undef;
}

sub YYError {
	my($self)=shift;

	${$$self{CHECK}}='ERROR';
    undef;
}

sub YYSemval {
	my($self)=shift;
	my($index)= $_[0] - ${$$self{DOTPOS}} - 1;

		$index < 0
	and	-$index <= @{$$self{STACK}}
	and	return $$self{STACK}[$index][1];

	undef;	#Invalid index
}

sub YYCurtok {
	my($self)=shift;

        @_
    and ${$$self{TOKEN}}=$_[0];
    ${$$self{TOKEN}};
}

sub YYCurval {
	my($self)=shift;

        @_
    and ${$$self{VALUE}}=$_[0];
    ${$$self{VALUE}};
}

sub YYExpect {
    my($self)=shift;

    keys %{$self->{STATES}[$self->{STACK}[-1][0]]{ACTIONS}}
}

sub YYLexer {
    my($self)=shift;

	$$self{LEX};
}


#################
# Private stuff #
#################


sub _CheckParams {
	my($mandatory,$checklist,$inarray,$outhash)=@_;
	my($prm,$value);
	my($prmlst)={};

	while(($prm,$value)=splice(@$inarray,0,2)) {
        $prm=uc($prm);
			exists($$checklist{$prm})
		or	croak("Unknow parameter '$prm'");
			ref($value) eq $$checklist{$prm}
		or	croak("Invalid value for parameter '$prm'");
        $prm=unpack('@2A*',$prm);
		$$outhash{$prm}=$value;
	}
	for (@$mandatory) {
			exists($$outhash{$_})
		or	croak("Missing mandatory parameter '".lc($_)."'");
	}
}

sub _Error {
	print "Parse error.\n";
}

sub _DBLoad {
	{
		no strict 'refs';

			exists(${__PACKAGE__.'::'}{_DBParse})#Already loaded ?
		and	return;
	}
	my($fname)=__FILE__;
	my(@drv);
	open(DRV,"<$fname") or die "Report this as a BUG: Cannot open $fname";
	while(<DRV>) {
                	/^\s*sub\s+_Parse\s*{\s*$/ .. /^\s*}\s*#\s*_Parse\s*$/
        	and     do {
                	s/^#DBG>//;
                	push(@drv,$_);
        	}
	}
	close(DRV);

	$drv[0]=~s/_P/_DBP/;
	eval join('',@drv);
}

#Note that for loading debugging version of the driver,
#this file will be parsed from 'sub _Parse' up to '}#_Parse' inclusive.
#So, DO NOT remove comment at end of sub !!!
sub _Parse {
    my($self)=shift;

	my($rules,$states,$lex,$error)
     = @$self{ 'RULES', 'STATES', 'LEX', 'ERROR' };
	my($errstatus,$nberror,$token,$value,$stack,$check,$dotpos)
     = @$self{ 'ERRST', 'NBERR', 'TOKEN', 'VALUE', 'STACK', 'CHECK', 'DOTPOS' };

#DBG>	my($debug)=$$self{DEBUG};
#DBG>	my($dbgerror)=0;

#DBG>	my($ShowCurToken) = sub {
#DBG>		my($tok)='>';
#DBG>		for (split('',$$token)) {
#DBG>			$tok.=		(ord($_) < 32 or ord($_) > 126)
#DBG>					?	sprintf('<%02X>',ord($_))
#DBG>					:	$_;
#DBG>		}
#DBG>		$tok.='<';
#DBG>	};

	$$errstatus=0;
	$$nberror=0;
	($$token,$$value)=(undef,undef);
	@$stack=( [ 0, undef ] );
	$$check='';

    while(1) {
        my($actions,$act,$stateno);

        $stateno=$$stack[-1][0];
        $actions=$$states[$stateno];

#DBG>	print STDERR ('-' x 40),"\n";
#DBG>		$debug & 0x2
#DBG>	and	print STDERR "In state $stateno:\n";
#DBG>		$debug & 0x08
#DBG>	and	print STDERR "Stack:[".
#DBG>					 join(',',map { $$_[0] } @$stack).
#DBG>					 "]\n";


        if  (exists($$actions{ACTIONS})) {

				defined($$token)
            or	do {
				($$token,$$value)=&$lex($self);
#DBG>				$debug & 0x01
#DBG>			and	print STDERR "Need token. Got ".&$ShowCurToken."\n";
			};

            $act=   exists($$actions{ACTIONS}{$$token})
                    ?   $$actions{ACTIONS}{$$token}
                    :   exists($$actions{DEFAULT})
                        ?   $$actions{DEFAULT}
                        :   undef;
        }
        else {
            $act=$$actions{DEFAULT};
#DBG>			$debug & 0x01
#DBG>		and	print STDERR "Don't need token.\n";
        }

            defined($act)
        and do {

                $act > 0
            and do {        #shift

#DBG>				$debug & 0x04
#DBG>			and	print STDERR "Shift and go to state $act.\n";

					$$errstatus
				and	do {
					--$$errstatus;

#DBG>					$debug & 0x10
#DBG>				and	$dbgerror
#DBG>				and	$$errstatus == 0
#DBG>				and	do {
#DBG>					print STDERR "**End of Error recovery.\n";
#DBG>					$dbgerror=0;
#DBG>				};
				};


                push(@$stack,[ $act, $$value ]);

					$$token ne ''	#Don't eat the eof
				and	$$token=$$value=undef;
                next;
            };

            #reduce
            my($lhs,$len,$code,@sempar,$semval);
            ($lhs,$len,$code)=@{$$rules[-$act]};

#DBG>			$debug & 0x04
#DBG>		and	$act
#DBG>		and	print STDERR "Reduce using rule ".-$act." ($lhs,$len): ";

                $act
            or  $self->YYAccept();

            $$dotpos=$len;

                unpack('A1',$lhs) eq '@'    #In line rule
            and do {
                    $lhs =~ /^\@[0-9]+\-([0-9]+)$/
                or  die "In line rule name '$lhs' ill formed: ".
                        "report it as a BUG.\n";
                $$dotpos = $1;
            };

            @sempar =       $$dotpos
                        ?   map { $$_[1] } @$stack[ -$$dotpos .. -1 ]
                        :   ();

            $semval = $code ? &$code( $self, @sempar )
                            : @sempar ? $sempar[0] : undef;

            splice(@$stack,-$len,$len);

                $$check eq 'ACCEPT'
            and do {

#DBG>			$debug & 0x04
#DBG>		and	print STDERR "Accept.\n";

				return($semval);
			};

                $$check eq 'ABORT'
            and	do {

#DBG>			$debug & 0x04
#DBG>		and	print STDERR "Abort.\n";

				return(undef);

			};

#DBG>			$debug & 0x04
#DBG>		and	print STDERR "Back to state $$stack[-1][0], then ";

                $$check eq 'ERROR'
            or  do {
#DBG>				$debug & 0x04
#DBG>			and	print STDERR 
#DBG>				    "go to state $$states[$$stack[-1][0]]{GOTOS}{$lhs}.\n";

#DBG>				$debug & 0x10
#DBG>			and	$dbgerror
#DBG>			and	$$errstatus == 0
#DBG>			and	do {
#DBG>				print STDERR "**End of Error recovery.\n";
#DBG>				$dbgerror=0;
#DBG>			};

			    push(@$stack,
                     [ $$states[$$stack[-1][0]]{GOTOS}{$lhs}, $semval ]);
                $$check='';
                next;
            };

#DBG>			$debug & 0x04
#DBG>		and	print STDERR "Forced Error recovery.\n";

            $$check='';

        };

        #Error
            $$errstatus
        or   do {

            $$errstatus = 1;
            &$error($self);
                $$errstatus # if 0, then YYErrok has been called
            or  next;       # so continue parsing

#DBG>			$debug & 0x10
#DBG>		and	do {
#DBG>			print STDERR "**Entering Error recovery.\n";
#DBG>			++$dbgerror;
#DBG>		};

            ++$$nberror;

        };

			$$errstatus == 3	#The next token is not valid: discard it
		and	do {
				$$token eq ''	# End of input: no hope
			and	do {
#DBG>				$debug & 0x10
#DBG>			and	print STDERR "**At eof: aborting.\n";
				return(undef);
			};

#DBG>			$debug & 0x10
#DBG>		and	print STDERR "**Dicard invalid token ".&$ShowCurToken.".\n";

			$$token=$$value=undef;
		};

        $$errstatus=3;

		while(	  @$stack
			  and (		not exists($$states[$$stack[-1][0]]{ACTIONS})
			        or  not exists($$states[$$stack[-1][0]]{ACTIONS}{error})
					or	$$states[$$stack[-1][0]]{ACTIONS}{error} <= 0)) {

#DBG>			$debug & 0x10
#DBG>		and	print STDERR "**Pop state $$stack[-1][0].\n";

			pop(@$stack);
		}

			@$stack
		or	do {

#DBG>			$debug & 0x10
#DBG>		and	print STDERR "**No state left on stack: aborting.\n";

			return(undef);
		};

		#shift the error token

#DBG>			$debug & 0x10
#DBG>		and	print STDERR "**Shift \$error token and go to state ".
#DBG>						 $$states[$$stack[-1][0]]{ACTIONS}{error}.
#DBG>						 ".\n";

		push(@$stack, [ $$states[$$stack[-1][0]]{ACTIONS}{error}, undef ]);

    }

    #never reached
	croak("Error in driver logic. Please, report it as a BUG");

}#_Parse
#DO NOT remove comment

1;

}
#End of include--------------------------------------------------




sub new {
        my($class)=shift;
        ref($class)
    and $class=ref($class);

    my($self)=$class->SUPER::new( yyversion => '1.05',
                                  yystates =>
[
	{#State 0
		DEFAULT => -1,
		GOTOS => {
			'idl' => 1
		}
	},
	{#State 1
		ACTIONS => {
			'' => 2,
			"importlib" => 3,
			"import" => 6,
			"include" => 11
		},
		DEFAULT => -98,
		GOTOS => {
			'importlib' => 9,
			'interface' => 8,
			'include' => 4,
			'coclass' => 10,
			'import' => 7,
			'property_list' => 5
		}
	},
	{#State 2
		DEFAULT => 0
	},
	{#State 3
		ACTIONS => {
			'TEXT' => 13
		},
		GOTOS => {
			'commalist' => 12,
			'text' => 14
		}
	},
	{#State 4
		DEFAULT => -5
	},
	{#State 5
		ACTIONS => {
			"coclass" => 15,
			"[" => 17,
			"interface" => 16
		}
	},
	{#State 6
		ACTIONS => {
			'TEXT' => 13
		},
		GOTOS => {
			'commalist' => 18,
			'text' => 14
		}
	},
	{#State 7
		DEFAULT => -4
	},
	{#State 8
		DEFAULT => -2
	},
	{#State 9
		DEFAULT => -6
	},
	{#State 10
		DEFAULT => -3
	},
	{#State 11
		ACTIONS => {
			'TEXT' => 13
		},
		GOTOS => {
			'commalist' => 19,
			'text' => 14
		}
	},
	{#State 12
		ACTIONS => {
			";" => 20,
			"," => 21
		}
	},
	{#State 13
		DEFAULT => -129
	},
	{#State 14
		DEFAULT => -10
	},
	{#State 15
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 23
		}
	},
	{#State 16
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 24
		}
	},
	{#State 17
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 26,
			'property' => 27,
			'properties' => 25
		}
	},
	{#State 18
		ACTIONS => {
			";" => 28,
			"," => 21
		}
	},
	{#State 19
		ACTIONS => {
			";" => 29,
			"," => 21
		}
	},
	{#State 20
		DEFAULT => -9
	},
	{#State 21
		ACTIONS => {
			'TEXT' => 13
		},
		GOTOS => {
			'text' => 30
		}
	},
	{#State 22
		DEFAULT => -125
	},
	{#State 23
		ACTIONS => {
			"{" => 31
		}
	},
	{#State 24
		ACTIONS => {
			":" => 32
		},
		DEFAULT => -16,
		GOTOS => {
			'base_interface' => 33
		}
	},
	{#State 25
		ACTIONS => {
			"," => 34,
			"]" => 35
		}
	},
	{#State 26
		ACTIONS => {
			"(" => 36
		},
		DEFAULT => -102
	},
	{#State 27
		DEFAULT => -100
	},
	{#State 28
		DEFAULT => -7
	},
	{#State 29
		DEFAULT => -8
	},
	{#State 30
		DEFAULT => -11
	},
	{#State 31
		DEFAULT => -13,
		GOTOS => {
			'interface_names' => 37
		}
	},
	{#State 32
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 38
		}
	},
	{#State 33
		ACTIONS => {
			"{" => 39
		}
	},
	{#State 34
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 26,
			'property' => 40
		}
	},
	{#State 35
		DEFAULT => -99
	},
	{#State 36
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'text' => 46,
			'anytext' => 41,
			'constant' => 42,
			'commalisttext' => 44
		}
	},
	{#State 37
		ACTIONS => {
			"}" => 47,
			"interface" => 48
		}
	},
	{#State 38
		DEFAULT => -17
	},
	{#State 39
		ACTIONS => {
			"typedef" => 49,
			"union" => 50,
			"enum" => 63,
			"bitmap" => 64,
			"declare" => 56,
			"const" => 58,
			"struct" => 61
		},
		DEFAULT => -98,
		GOTOS => {
			'typedecl' => 62,
			'function' => 51,
			'bitmap' => 65,
			'definitions' => 52,
			'definition' => 55,
			'property_list' => 54,
			'usertype' => 53,
			'declare' => 67,
			'const' => 66,
			'struct' => 57,
			'enum' => 59,
			'typedef' => 60,
			'union' => 68
		}
	},
	{#State 40
		DEFAULT => -101
	},
	{#State 41
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -104
	},
	{#State 42
		DEFAULT => -108
	},
	{#State 43
		DEFAULT => -128
	},
	{#State 44
		ACTIONS => {
			"," => 84,
			")" => 85
		}
	},
	{#State 45
		DEFAULT => -107
	},
	{#State 46
		DEFAULT => -109
	},
	{#State 47
		ACTIONS => {
			";" => 86
		},
		DEFAULT => -130,
		GOTOS => {
			'optional_semicolon' => 87
		}
	},
	{#State 48
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 88
		}
	},
	{#State 49
		DEFAULT => -98,
		GOTOS => {
			'property_list' => 89
		}
	},
	{#State 50
		ACTIONS => {
			'IDENTIFIER' => 90
		},
		DEFAULT => -127,
		GOTOS => {
			'optional_identifier' => 91
		}
	},
	{#State 51
		DEFAULT => -20
	},
	{#State 52
		ACTIONS => {
			"}" => 92,
			"typedef" => 49,
			"union" => 50,
			"enum" => 63,
			"bitmap" => 64,
			"declare" => 56,
			"const" => 58,
			"struct" => 61
		},
		DEFAULT => -98,
		GOTOS => {
			'typedecl' => 62,
			'function' => 51,
			'bitmap' => 65,
			'definition' => 93,
			'property_list' => 54,
			'usertype' => 53,
			'const' => 66,
			'struct' => 57,
			'declare' => 67,
			'enum' => 59,
			'typedef' => 60,
			'union' => 68
		}
	},
	{#State 53
		ACTIONS => {
			";" => 94
		}
	},
	{#State 54
		ACTIONS => {
			'IDENTIFIER' => 22,
			"signed" => 100,
			"union" => 50,
			"enum" => 63,
			"bitmap" => 64,
			'void' => 95,
			"unsigned" => 101,
			"[" => 17,
			"struct" => 61
		},
		GOTOS => {
			'existingtype' => 99,
			'bitmap' => 65,
			'usertype' => 96,
			'identifier' => 97,
			'struct' => 57,
			'enum' => 59,
			'type' => 102,
			'union' => 68,
			'sign' => 98
		}
	},
	{#State 55
		DEFAULT => -18
	},
	{#State 56
		DEFAULT => -98,
		GOTOS => {
			'property_list' => 103
		}
	},
	{#State 57
		DEFAULT => -42
	},
	{#State 58
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 104
		}
	},
	{#State 59
		DEFAULT => -44
	},
	{#State 60
		DEFAULT => -22
	},
	{#State 61
		ACTIONS => {
			'IDENTIFIER' => 90
		},
		DEFAULT => -127,
		GOTOS => {
			'optional_identifier' => 105
		}
	},
	{#State 62
		DEFAULT => -24
	},
	{#State 63
		ACTIONS => {
			'IDENTIFIER' => 90
		},
		DEFAULT => -127,
		GOTOS => {
			'optional_identifier' => 106
		}
	},
	{#State 64
		ACTIONS => {
			'IDENTIFIER' => 90
		},
		DEFAULT => -127,
		GOTOS => {
			'optional_identifier' => 107
		}
	},
	{#State 65
		DEFAULT => -45
	},
	{#State 66
		DEFAULT => -21
	},
	{#State 67
		DEFAULT => -23
	},
	{#State 68
		DEFAULT => -43
	},
	{#State 69
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 108,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 70
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 109,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 71
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 110,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 72
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 111,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 73
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 112,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 74
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 113,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 75
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 114,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 76
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 41,
			'text' => 46,
			'constant' => 42,
			'commalisttext' => 115
		}
	},
	{#State 77
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 116,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 78
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 117,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 79
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 118,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 80
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 41,
			'text' => 46,
			'constant' => 42,
			'commalisttext' => 119
		}
	},
	{#State 81
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 120,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 82
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 121,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 83
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 122,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 84
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 123,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 85
		DEFAULT => -103
	},
	{#State 86
		DEFAULT => -131
	},
	{#State 87
		DEFAULT => -12
	},
	{#State 88
		ACTIONS => {
			";" => 124
		}
	},
	{#State 89
		ACTIONS => {
			'IDENTIFIER' => 22,
			"signed" => 100,
			"union" => 50,
			"enum" => 63,
			"bitmap" => 64,
			'void' => 95,
			"unsigned" => 101,
			"[" => 17,
			"struct" => 61
		},
		DEFAULT => -49,
		GOTOS => {
			'existingtype' => 125,
			'bitmap' => 65,
			'usertype' => 96,
			'identifier' => 97,
			'struct' => 57,
			'enum' => 59,
			'type' => 126,
			'union' => 68,
			'sign' => 98
		}
	},
	{#State 90
		DEFAULT => -126
	},
	{#State 91
		ACTIONS => {
			"{" => 128
		},
		DEFAULT => -83,
		GOTOS => {
			'union_body' => 129,
			'opt_union_body' => 127
		}
	},
	{#State 92
		ACTIONS => {
			";" => 86
		},
		DEFAULT => -130,
		GOTOS => {
			'optional_semicolon' => 130
		}
	},
	{#State 93
		DEFAULT => -19
	},
	{#State 94
		DEFAULT => -46
	},
	{#State 95
		DEFAULT => -54
	},
	{#State 96
		DEFAULT => -52
	},
	{#State 97
		DEFAULT => -51
	},
	{#State 98
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 131
		}
	},
	{#State 99
		DEFAULT => -53
	},
	{#State 100
		DEFAULT => -47
	},
	{#State 101
		DEFAULT => -48
	},
	{#State 102
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 132
		}
	},
	{#State 103
		ACTIONS => {
			"union" => 133,
			"enum" => 138,
			"bitmap" => 139,
			"[" => 17
		},
		GOTOS => {
			'decl_enum' => 134,
			'decl_bitmap' => 135,
			'decl_type' => 137,
			'decl_union' => 136
		}
	},
	{#State 104
		DEFAULT => -87,
		GOTOS => {
			'pointers' => 140
		}
	},
	{#State 105
		ACTIONS => {
			"{" => 142
		},
		DEFAULT => -73,
		GOTOS => {
			'struct_body' => 141,
			'opt_struct_body' => 143
		}
	},
	{#State 106
		ACTIONS => {
			"{" => 144
		},
		DEFAULT => -56,
		GOTOS => {
			'opt_enum_body' => 146,
			'enum_body' => 145
		}
	},
	{#State 107
		ACTIONS => {
			"{" => 148
		},
		DEFAULT => -64,
		GOTOS => {
			'bitmap_body' => 149,
			'opt_bitmap_body' => 147
		}
	},
	{#State 108
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -119
	},
	{#State 109
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -110
	},
	{#State 110
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -114
	},
	{#State 111
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -122
	},
	{#State 112
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -121
	},
	{#State 113
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -112
	},
	{#State 114
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -118
	},
	{#State 115
		ACTIONS => {
			"}" => 150,
			"," => 84
		}
	},
	{#State 116
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -116
	},
	{#State 117
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -117
	},
	{#State 118
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -120
	},
	{#State 119
		ACTIONS => {
			"," => 84,
			")" => 151
		}
	},
	{#State 120
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -115
	},
	{#State 121
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -111
	},
	{#State 122
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -113
	},
	{#State 123
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -105
	},
	{#State 124
		DEFAULT => -14
	},
	{#State 125
		ACTIONS => {
			"(" => 156,
			'IDENTIFIER' => 22,
			"*" => 154
		},
		GOTOS => {
			'direct_declarator' => 155,
			'identifier' => 153,
			'declarator' => 152
		}
	},
	{#State 126
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 157
		}
	},
	{#State 127
		DEFAULT => -85
	},
	{#State 128
		DEFAULT => -80,
		GOTOS => {
			'union_elements' => 158
		}
	},
	{#State 129
		DEFAULT => -84
	},
	{#State 130
		DEFAULT => -15
	},
	{#State 131
		DEFAULT => -50
	},
	{#State 132
		ACTIONS => {
			"(" => 159
		}
	},
	{#State 133
		DEFAULT => -34
	},
	{#State 134
		DEFAULT => -29
	},
	{#State 135
		DEFAULT => -30
	},
	{#State 136
		DEFAULT => -31
	},
	{#State 137
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 160
		}
	},
	{#State 138
		DEFAULT => -32
	},
	{#State 139
		DEFAULT => -33
	},
	{#State 140
		ACTIONS => {
			'IDENTIFIER' => 22,
			"*" => 162
		},
		GOTOS => {
			'identifier' => 161
		}
	},
	{#State 141
		DEFAULT => -74
	},
	{#State 142
		DEFAULT => -89,
		GOTOS => {
			'element_list1' => 163
		}
	},
	{#State 143
		DEFAULT => -75
	},
	{#State 144
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 164,
			'enum_element' => 165,
			'enum_elements' => 166
		}
	},
	{#State 145
		DEFAULT => -57
	},
	{#State 146
		DEFAULT => -58
	},
	{#State 147
		DEFAULT => -66
	},
	{#State 148
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		DEFAULT => -69,
		GOTOS => {
			'identifier' => 169,
			'bitmap_element' => 168,
			'bitmap_elements' => 167,
			'opt_bitmap_elements' => 170
		}
	},
	{#State 149
		DEFAULT => -65
	},
	{#State 150
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 171,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 151
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 172,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 152
		ACTIONS => {
			";" => 173
		}
	},
	{#State 153
		DEFAULT => -39
	},
	{#State 154
		ACTIONS => {
			"(" => 156,
			'IDENTIFIER' => 22,
			"*" => 154
		},
		GOTOS => {
			'direct_declarator' => 155,
			'identifier' => 153,
			'declarator' => 174
		}
	},
	{#State 155
		ACTIONS => {
			"[" => 175
		},
		DEFAULT => -37
	},
	{#State 156
		ACTIONS => {
			"(" => 156,
			'IDENTIFIER' => 22,
			"*" => 154
		},
		GOTOS => {
			'direct_declarator' => 155,
			'identifier' => 153,
			'declarator' => 176
		}
	},
	{#State 157
		ACTIONS => {
			"[" => 177
		},
		DEFAULT => -95,
		GOTOS => {
			'array_len' => 178
		}
	},
	{#State 158
		ACTIONS => {
			"}" => 179
		},
		DEFAULT => -98,
		GOTOS => {
			'optional_base_element' => 181,
			'property_list' => 180
		}
	},
	{#State 159
		ACTIONS => {
			"," => -91,
			"void" => 185,
			")" => -91
		},
		DEFAULT => -98,
		GOTOS => {
			'base_element' => 182,
			'element_list2' => 184,
			'property_list' => 183
		}
	},
	{#State 160
		ACTIONS => {
			";" => 186
		}
	},
	{#State 161
		ACTIONS => {
			"[" => 177,
			"=" => 188
		},
		GOTOS => {
			'array_len' => 187
		}
	},
	{#State 162
		DEFAULT => -88
	},
	{#State 163
		ACTIONS => {
			"}" => 189
		},
		DEFAULT => -98,
		GOTOS => {
			'base_element' => 190,
			'property_list' => 183
		}
	},
	{#State 164
		ACTIONS => {
			"=" => 191
		},
		DEFAULT => -61
	},
	{#State 165
		DEFAULT => -59
	},
	{#State 166
		ACTIONS => {
			"}" => 192,
			"," => 193
		}
	},
	{#State 167
		ACTIONS => {
			"," => 194
		},
		DEFAULT => -70
	},
	{#State 168
		DEFAULT => -67
	},
	{#State 169
		ACTIONS => {
			"=" => 195
		}
	},
	{#State 170
		ACTIONS => {
			"}" => 196
		}
	},
	{#State 171
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -124
	},
	{#State 172
		ACTIONS => {
			":" => 69,
			"<" => 71,
			"~" => 72,
			"?" => 75,
			"{" => 76,
			"=" => 79
		},
		DEFAULT => -123
	},
	{#State 173
		DEFAULT => -36
	},
	{#State 174
		DEFAULT => -38
	},
	{#State 175
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 197,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 176
		ACTIONS => {
			")" => 198
		}
	},
	{#State 177
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			"]" => 199,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 200,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 178
		ACTIONS => {
			";" => 201
		}
	},
	{#State 179
		DEFAULT => -82
	},
	{#State 180
		ACTIONS => {
			"[" => 17
		},
		DEFAULT => -98,
		GOTOS => {
			'base_or_empty' => 202,
			'base_element' => 203,
			'empty_element' => 204,
			'property_list' => 205
		}
	},
	{#State 181
		DEFAULT => -81
	},
	{#State 182
		DEFAULT => -93
	},
	{#State 183
		ACTIONS => {
			'IDENTIFIER' => 22,
			"signed" => 100,
			"union" => 50,
			"enum" => 63,
			"bitmap" => 64,
			'void' => 95,
			"unsigned" => 101,
			"[" => 17,
			"struct" => 61
		},
		DEFAULT => -49,
		GOTOS => {
			'existingtype' => 99,
			'bitmap' => 65,
			'usertype' => 96,
			'identifier' => 97,
			'struct' => 57,
			'enum' => 59,
			'type' => 206,
			'union' => 68,
			'sign' => 98
		}
	},
	{#State 184
		ACTIONS => {
			"," => 207,
			")" => 208
		}
	},
	{#State 185
		DEFAULT => -92
	},
	{#State 186
		DEFAULT => -28
	},
	{#State 187
		ACTIONS => {
			"=" => 209
		}
	},
	{#State 188
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 210,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 189
		DEFAULT => -72
	},
	{#State 190
		ACTIONS => {
			";" => 211
		}
	},
	{#State 191
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 212,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 192
		DEFAULT => -55
	},
	{#State 193
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 164,
			'enum_element' => 213
		}
	},
	{#State 194
		ACTIONS => {
			'IDENTIFIER' => 22
		},
		GOTOS => {
			'identifier' => 169,
			'bitmap_element' => 214
		}
	},
	{#State 195
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 215,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 196
		DEFAULT => -63
	},
	{#State 197
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"?" => 75,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"&" => 77,
			"{" => 76,
			"/" => 78,
			"=" => 79,
			"|" => 81,
			"(" => 80,
			"*" => 74,
			"." => 82,
			"]" => 216,
			">" => 83
		}
	},
	{#State 198
		DEFAULT => -40
	},
	{#State 199
		ACTIONS => {
			"[" => 177
		},
		DEFAULT => -95,
		GOTOS => {
			'array_len' => 217
		}
	},
	{#State 200
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"?" => 75,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"&" => 77,
			"{" => 76,
			"/" => 78,
			"=" => 79,
			"|" => 81,
			"(" => 80,
			"*" => 74,
			"." => 82,
			"]" => 218,
			">" => 83
		}
	},
	{#State 201
		DEFAULT => -35
	},
	{#State 202
		DEFAULT => -79
	},
	{#State 203
		ACTIONS => {
			";" => 219
		}
	},
	{#State 204
		DEFAULT => -78
	},
	{#State 205
		ACTIONS => {
			'IDENTIFIER' => 22,
			"signed" => 100,
			"union" => 50,
			";" => 220,
			"enum" => 63,
			"bitmap" => 64,
			'void' => 95,
			"unsigned" => 101,
			"[" => 17,
			"struct" => 61
		},
		DEFAULT => -49,
		GOTOS => {
			'existingtype' => 99,
			'bitmap' => 65,
			'usertype' => 96,
			'identifier' => 97,
			'struct' => 57,
			'enum' => 59,
			'type' => 206,
			'union' => 68,
			'sign' => 98
		}
	},
	{#State 206
		DEFAULT => -87,
		GOTOS => {
			'pointers' => 221
		}
	},
	{#State 207
		DEFAULT => -98,
		GOTOS => {
			'base_element' => 222,
			'property_list' => 183
		}
	},
	{#State 208
		ACTIONS => {
			";" => 223
		}
	},
	{#State 209
		ACTIONS => {
			'CONSTANT' => 43,
			'TEXT' => 13,
			'IDENTIFIER' => 22
		},
		DEFAULT => -106,
		GOTOS => {
			'identifier' => 45,
			'anytext' => 224,
			'text' => 46,
			'constant' => 42
		}
	},
	{#State 210
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"?" => 75,
			"<" => 71,
			";" => 225,
			"+" => 73,
			"~" => 72,
			"&" => 77,
			"{" => 76,
			"/" => 78,
			"=" => 79,
			"|" => 81,
			"(" => 80,
			"*" => 74,
			"." => 82,
			">" => 83
		}
	},
	{#State 211
		DEFAULT => -90
	},
	{#State 212
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -62
	},
	{#State 213
		DEFAULT => -60
	},
	{#State 214
		DEFAULT => -68
	},
	{#State 215
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"<" => 71,
			"+" => 73,
			"~" => 72,
			"*" => 74,
			"?" => 75,
			"{" => 76,
			"&" => 77,
			"/" => 78,
			"=" => 79,
			"(" => 80,
			"|" => 81,
			"." => 82,
			">" => 83
		},
		DEFAULT => -71
	},
	{#State 216
		DEFAULT => -41
	},
	{#State 217
		DEFAULT => -96
	},
	{#State 218
		ACTIONS => {
			"[" => 177
		},
		DEFAULT => -95,
		GOTOS => {
			'array_len' => 226
		}
	},
	{#State 219
		DEFAULT => -77
	},
	{#State 220
		DEFAULT => -76
	},
	{#State 221
		ACTIONS => {
			'IDENTIFIER' => 22,
			"*" => 162
		},
		GOTOS => {
			'identifier' => 227
		}
	},
	{#State 222
		DEFAULT => -94
	},
	{#State 223
		DEFAULT => -27
	},
	{#State 224
		ACTIONS => {
			"-" => 70,
			":" => 69,
			"?" => 75,
			"<" => 71,
			";" => 228,
			"+" => 73,
			"~" => 72,
			"&" => 77,
			"{" => 76,
			"/" => 78,
			"=" => 79,
			"|" => 81,
			"(" => 80,
			"*" => 74,
			"." => 82,
			">" => 83
		}
	},
	{#State 225
		DEFAULT => -25
	},
	{#State 226
		DEFAULT => -97
	},
	{#State 227
		ACTIONS => {
			"[" => 177
		},
		DEFAULT => -95,
		GOTOS => {
			'array_len' => 229
		}
	},
	{#State 228
		DEFAULT => -26
	},
	{#State 229
		DEFAULT => -86
	}
],
                                  yyrules  =>
[
	[#Rule 0
		 '$start', 2, undef
	],
	[#Rule 1
		 'idl', 0, undef
	],
	[#Rule 2
		 'idl', 2,
sub
#line 19 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 3
		 'idl', 2,
sub
#line 20 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 4
		 'idl', 2,
sub
#line 21 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 5
		 'idl', 2,
sub
#line 22 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 6
		 'idl', 2,
sub
#line 23 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 7
		 'import', 3,
sub
#line 26 "pidl/idl.yp"
{{
			"TYPE" => "IMPORT", 
			"PATHS" => $_[2],
		   "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		   "LINE" => $_[0]->YYData->{LINE}
		}}
	],
	[#Rule 8
		 'include', 3,
sub
#line 33 "pidl/idl.yp"
{{ 
			"TYPE" => "INCLUDE", 
			"PATHS" => $_[2],
		   "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		   "LINE" => $_[0]->YYData->{LINE}
		}}
	],
	[#Rule 9
		 'importlib', 3,
sub
#line 40 "pidl/idl.yp"
{{ 
			"TYPE" => "IMPORTLIB", 
			"PATHS" => $_[2],
		   "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		   "LINE" => $_[0]->YYData->{LINE}
		}}
	],
	[#Rule 10
		 'commalist', 1,
sub
#line 49 "pidl/idl.yp"
{ [ $_[1] ] }
	],
	[#Rule 11
		 'commalist', 3,
sub
#line 50 "pidl/idl.yp"
{ push(@{$_[1]}, $_[3]); $_[1] }
	],
	[#Rule 12
		 'coclass', 7,
sub
#line 54 "pidl/idl.yp"
{{
               "TYPE" => "COCLASS", 
	       "PROPERTIES" => $_[1],
	       "NAME" => $_[3],
	       "DATA" => $_[5],
		   "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		   "LINE" => $_[0]->YYData->{LINE},
          }}
	],
	[#Rule 13
		 'interface_names', 0, undef
	],
	[#Rule 14
		 'interface_names', 4,
sub
#line 66 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 15
		 'interface', 8,
sub
#line 70 "pidl/idl.yp"
{{
               "TYPE" => "INTERFACE", 
	       "PROPERTIES" => $_[1],
	       "NAME" => $_[3],
	       "BASE" => $_[4],
	       "DATA" => $_[6],
		   "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		   "LINE" => $_[0]->YYData->{LINE},
          }}
	],
	[#Rule 16
		 'base_interface', 0, undef
	],
	[#Rule 17
		 'base_interface', 2,
sub
#line 83 "pidl/idl.yp"
{ $_[2] }
	],
	[#Rule 18
		 'definitions', 1,
sub
#line 87 "pidl/idl.yp"
{ [ $_[1] ] }
	],
	[#Rule 19
		 'definitions', 2,
sub
#line 88 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 20
		 'definition', 1, undef
	],
	[#Rule 21
		 'definition', 1, undef
	],
	[#Rule 22
		 'definition', 1, undef
	],
	[#Rule 23
		 'definition', 1, undef
	],
	[#Rule 24
		 'definition', 1, undef
	],
	[#Rule 25
		 'const', 7,
sub
#line 96 "pidl/idl.yp"
{{
                     "TYPE"  => "CONST", 
		     "DTYPE"  => $_[2],
			 "POINTERS" => $_[3],
		     "NAME"  => $_[4],
		     "VALUE" => $_[6],
		     "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		     "LINE" => $_[0]->YYData->{LINE},
        }}
	],
	[#Rule 26
		 'const', 8,
sub
#line 106 "pidl/idl.yp"
{{
                     "TYPE"  => "CONST", 
		     "DTYPE"  => $_[2],
			 "POINTERS" => $_[3],
		     "NAME"  => $_[4],
		     "ARRAY_LEN" => $_[5],
		     "VALUE" => $_[7],
		     "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		     "LINE" => $_[0]->YYData->{LINE},
        }}
	],
	[#Rule 27
		 'function', 7,
sub
#line 120 "pidl/idl.yp"
{{
		"TYPE" => "FUNCTION",
		"NAME" => $_[3],
		"RETURN_TYPE" => $_[2],
		"PROPERTIES" => $_[1],
		"ELEMENTS" => $_[5],
		"FILE" => $_[0]->YYData->{INPUT_FILENAME},
		"LINE" => $_[0]->YYData->{LINE},
	  }}
	],
	[#Rule 28
		 'declare', 5,
sub
#line 132 "pidl/idl.yp"
{{
	             "TYPE" => "DECLARE", 
                     "PROPERTIES" => $_[2],
		     "NAME" => $_[4],
		     "DATA" => $_[3],
		     "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		     "LINE" => $_[0]->YYData->{LINE},
        }}
	],
	[#Rule 29
		 'decl_type', 1, undef
	],
	[#Rule 30
		 'decl_type', 1, undef
	],
	[#Rule 31
		 'decl_type', 1, undef
	],
	[#Rule 32
		 'decl_enum', 1,
sub
#line 146 "pidl/idl.yp"
{{
                     "TYPE" => "ENUM"
        }}
	],
	[#Rule 33
		 'decl_bitmap', 1,
sub
#line 152 "pidl/idl.yp"
{{
                     "TYPE" => "BITMAP"
        }}
	],
	[#Rule 34
		 'decl_union', 1,
sub
#line 158 "pidl/idl.yp"
{{
                     "TYPE" => "UNION"
        }}
	],
	[#Rule 35
		 'typedef', 6,
sub
#line 164 "pidl/idl.yp"
{{
	             "TYPE" => "TYPEDEF", 
                     "PROPERTIES" => $_[2],
		     "NAME" => $_[4],
		     "DATA" => $_[3],
		     "ARRAY_LEN" => $_[5],
		     "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		     "LINE" => $_[0]->YYData->{LINE},
        }}
	],
	[#Rule 36
		 'typedef', 5,
sub
#line 174 "pidl/idl.yp"
{
		    $_[4]->{TYPE} = "DECORATED";
		    $_[4]->{DATA_TYPE} = $_[3];
		    {
		    "TYPE" => "TYPEDEF",
		    "PROPERTIES" => $_[2],
		    "NAME" => $_[4]->{NAME},
		    "DATA" => $_[4],
		    "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		    "LINE" => $_[0]->YYData->{LINE}
		    }
	}
	],
	[#Rule 37
		 'declarator', 1, undef
	],
	[#Rule 38
		 'declarator', 2,
sub
#line 190 "pidl/idl.yp"
{
		    $_[2]->{LEVELS} = [] unless $_[2]->{LEVELS};
		    push (@{$_[2]->{LEVELS}}, { "TYPE" => "POINTER" });
		    $_[2]->{POINTERS}++;
		    $_[2]
	}
	],
	[#Rule 39
		 'direct_declarator', 1,
sub
#line 199 "pidl/idl.yp"
{{
		    "NAME" => $_[1],
		    "POINTERS" => 0
	}}
	],
	[#Rule 40
		 'direct_declarator', 3, undef
	],
	[#Rule 41
		 'direct_declarator', 4,
sub
#line 205 "pidl/idl.yp"
{
		    $_[1]->{LEVELS} = [] unless $_[1]->{LEVELS};
		    push (@{$_[1]->{LEVELS}}, { 'TYPE' => 'ARRAY', 'SIZE_IS' => $_[3] });
		    $_[1]->{ARRAY_LEN} = [] unless $_[1]->{ARRAY_LEN};
		    push @{$_[1]->{ARRAY_LEN}}, ($_[3] ne '') ? $_[3] : '*';
		    $_[1]
	}
	],
	[#Rule 42
		 'usertype', 1, undef
	],
	[#Rule 43
		 'usertype', 1, undef
	],
	[#Rule 44
		 'usertype', 1, undef
	],
	[#Rule 45
		 'usertype', 1, undef
	],
	[#Rule 46
		 'typedecl', 2,
sub
#line 218 "pidl/idl.yp"
{ $_[1] }
	],
	[#Rule 47
		 'sign', 1, undef
	],
	[#Rule 48
		 'sign', 1, undef
	],
	[#Rule 49
		 'existingtype', 0, undef
	],
	[#Rule 50
		 'existingtype', 2,
sub
#line 223 "pidl/idl.yp"
{ "$_[1] $_[2]" }
	],
	[#Rule 51
		 'existingtype', 1, undef
	],
	[#Rule 52
		 'type', 1, undef
	],
	[#Rule 53
		 'type', 1, undef
	],
	[#Rule 54
		 'type', 1,
sub
#line 227 "pidl/idl.yp"
{ "void" }
	],
	[#Rule 55
		 'enum_body', 3,
sub
#line 229 "pidl/idl.yp"
{ $_[2] }
	],
	[#Rule 56
		 'opt_enum_body', 0, undef
	],
	[#Rule 57
		 'opt_enum_body', 1, undef
	],
	[#Rule 58
		 'enum', 3,
sub
#line 232 "pidl/idl.yp"
{{
             "TYPE" => "ENUM", 
			 "NAME" => $_[2],
		     "ELEMENTS" => $_[3]
        }}
	],
	[#Rule 59
		 'enum_elements', 1,
sub
#line 240 "pidl/idl.yp"
{ [ $_[1] ] }
	],
	[#Rule 60
		 'enum_elements', 3,
sub
#line 241 "pidl/idl.yp"
{ push(@{$_[1]}, $_[3]); $_[1] }
	],
	[#Rule 61
		 'enum_element', 1, undef
	],
	[#Rule 62
		 'enum_element', 3,
sub
#line 245 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 63
		 'bitmap_body', 3,
sub
#line 248 "pidl/idl.yp"
{ $_[2] }
	],
	[#Rule 64
		 'opt_bitmap_body', 0, undef
	],
	[#Rule 65
		 'opt_bitmap_body', 1, undef
	],
	[#Rule 66
		 'bitmap', 3,
sub
#line 251 "pidl/idl.yp"
{{
             "TYPE" => "BITMAP", 
			 "NAME" => $_[2],
		     "ELEMENTS" => $_[3]
        }}
	],
	[#Rule 67
		 'bitmap_elements', 1,
sub
#line 259 "pidl/idl.yp"
{ [ $_[1] ] }
	],
	[#Rule 68
		 'bitmap_elements', 3,
sub
#line 260 "pidl/idl.yp"
{ push(@{$_[1]}, $_[3]); $_[1] }
	],
	[#Rule 69
		 'opt_bitmap_elements', 0, undef
	],
	[#Rule 70
		 'opt_bitmap_elements', 1, undef
	],
	[#Rule 71
		 'bitmap_element', 3,
sub
#line 265 "pidl/idl.yp"
{ "$_[1] ( $_[3] )" }
	],
	[#Rule 72
		 'struct_body', 3,
sub
#line 268 "pidl/idl.yp"
{ $_[2] }
	],
	[#Rule 73
		 'opt_struct_body', 0, undef
	],
	[#Rule 74
		 'opt_struct_body', 1, undef
	],
	[#Rule 75
		 'struct', 3,
sub
#line 272 "pidl/idl.yp"
{{
             "TYPE" => "STRUCT", 
			 "NAME" => $_[2],
		     "ELEMENTS" => $_[3]
        }}
	],
	[#Rule 76
		 'empty_element', 2,
sub
#line 280 "pidl/idl.yp"
{{
		 "NAME" => "",
		 "TYPE" => "EMPTY",
		 "PROPERTIES" => $_[1],
		 "POINTERS" => 0,
		 "ARRAY_LEN" => [],
		 "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		 "LINE" => $_[0]->YYData->{LINE},
	 }}
	],
	[#Rule 77
		 'base_or_empty', 2, undef
	],
	[#Rule 78
		 'base_or_empty', 1, undef
	],
	[#Rule 79
		 'optional_base_element', 2,
sub
#line 294 "pidl/idl.yp"
{ $_[2]->{PROPERTIES} = FlattenHash([$_[1],$_[2]->{PROPERTIES}]); $_[2] }
	],
	[#Rule 80
		 'union_elements', 0, undef
	],
	[#Rule 81
		 'union_elements', 2,
sub
#line 299 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 82
		 'union_body', 3,
sub
#line 302 "pidl/idl.yp"
{ $_[2] }
	],
	[#Rule 83
		 'opt_union_body', 0, undef
	],
	[#Rule 84
		 'opt_union_body', 1, undef
	],
	[#Rule 85
		 'union', 3,
sub
#line 306 "pidl/idl.yp"
{{
             "TYPE" => "UNION", 
		     "NAME" => $_[2],
		     "ELEMENTS" => $_[3]
        }}
	],
	[#Rule 86
		 'base_element', 5,
sub
#line 314 "pidl/idl.yp"
{{
			   "NAME" => $_[4],
			   "TYPE" => $_[2],
			   "PROPERTIES" => $_[1],
			   "POINTERS" => $_[3],
			   "ARRAY_LEN" => $_[5],
		       "FILE" => $_[0]->YYData->{INPUT_FILENAME},
		       "LINE" => $_[0]->YYData->{LINE},
              }}
	],
	[#Rule 87
		 'pointers', 0,
sub
#line 328 "pidl/idl.yp"
{ 0 }
	],
	[#Rule 88
		 'pointers', 2,
sub
#line 329 "pidl/idl.yp"
{ $_[1]+1 }
	],
	[#Rule 89
		 'element_list1', 0, undef
	],
	[#Rule 90
		 'element_list1', 3,
sub
#line 334 "pidl/idl.yp"
{ push(@{$_[1]}, $_[2]); $_[1] }
	],
	[#Rule 91
		 'element_list2', 0, undef
	],
	[#Rule 92
		 'element_list2', 1, undef
	],
	[#Rule 93
		 'element_list2', 1,
sub
#line 340 "pidl/idl.yp"
{ [ $_[1] ] }
	],
	[#Rule 94
		 'element_list2', 3,
sub
#line 341 "pidl/idl.yp"
{ push(@{$_[1]}, $_[3]); $_[1] }
	],
	[#Rule 95
		 'array_len', 0, undef
	],
	[#Rule 96
		 'array_len', 3,
sub
#line 346 "pidl/idl.yp"
{ push(@{$_[3]}, "*"); $_[3] }
	],
	[#Rule 97
		 'array_len', 4,
sub
#line 347 "pidl/idl.yp"
{ push(@{$_[4]}, "$_[2]"); $_[4] }
	],
	[#Rule 98
		 'property_list', 0, undef
	],
	[#Rule 99
		 'property_list', 4,
sub
#line 353 "pidl/idl.yp"
{ FlattenHash([$_[1],$_[3]]); }
	],
	[#Rule 100
		 'properties', 1,
sub
#line 356 "pidl/idl.yp"
{ $_[1] }
	],
	[#Rule 101
		 'properties', 3,
sub
#line 357 "pidl/idl.yp"
{ FlattenHash([$_[1], $_[3]]); }
	],
	[#Rule 102
		 'property', 1,
sub
#line 360 "pidl/idl.yp"
{{ "$_[1]" => "1"     }}
	],
	[#Rule 103
		 'property', 4,
sub
#line 361 "pidl/idl.yp"
{{ "$_[1]" => "$_[3]" }}
	],
	[#Rule 104
		 'commalisttext', 1, undef
	],
	[#Rule 105
		 'commalisttext', 3,
sub
#line 371 "pidl/idl.yp"
{ "$_[1],$_[3]" }
	],
	[#Rule 106
		 'anytext', 0,
sub
#line 375 "pidl/idl.yp"
{ "" }
	],
	[#Rule 107
		 'anytext', 1, undef
	],
	[#Rule 108
		 'anytext', 1, undef
	],
	[#Rule 109
		 'anytext', 1, undef
	],
	[#Rule 110
		 'anytext', 3,
sub
#line 377 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 111
		 'anytext', 3,
sub
#line 378 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 112
		 'anytext', 3,
sub
#line 379 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 113
		 'anytext', 3,
sub
#line 380 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 114
		 'anytext', 3,
sub
#line 381 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 115
		 'anytext', 3,
sub
#line 382 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 116
		 'anytext', 3,
sub
#line 383 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 117
		 'anytext', 3,
sub
#line 384 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 118
		 'anytext', 3,
sub
#line 385 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 119
		 'anytext', 3,
sub
#line 386 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 120
		 'anytext', 3,
sub
#line 387 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 121
		 'anytext', 3,
sub
#line 388 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 122
		 'anytext', 3,
sub
#line 389 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]" }
	],
	[#Rule 123
		 'anytext', 5,
sub
#line 390 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]$_[4]$_[5]" }
	],
	[#Rule 124
		 'anytext', 5,
sub
#line 391 "pidl/idl.yp"
{ "$_[1]$_[2]$_[3]$_[4]$_[5]" }
	],
	[#Rule 125
		 'identifier', 1, undef
	],
	[#Rule 126
		 'optional_identifier', 1, undef
	],
	[#Rule 127
		 'optional_identifier', 0, undef
	],
	[#Rule 128
		 'constant', 1, undef
	],
	[#Rule 129
		 'text', 1,
sub
#line 405 "pidl/idl.yp"
{ "\"$_[1]\"" }
	],
	[#Rule 130
		 'optional_semicolon', 0, undef
	],
	[#Rule 131
		 'optional_semicolon', 1, undef
	]
],
                                  @_);
    bless($self,$class);
}

#line 416 "pidl/idl.yp"


#####################################################################
# flatten an array of hashes into a single hash
sub FlattenHash($) 
{ 
    my $a = shift;
    my %b;
    for my $d (@{$a}) {
	for my $k (keys %{$d}) {
	    $b{$k} = $d->{$k};
	}
    }
    return \%b;
}



#####################################################################
# traverse a perl data structure removing any empty arrays or
# hashes and any hash elements that map to undef
sub CleanData($)
{
    sub CleanData($);
    my($v) = shift;
	return undef if (not defined($v));
    if (ref($v) eq "ARRAY") {
	foreach my $i (0 .. $#{$v}) {
	    CleanData($v->[$i]);
	    if (ref($v->[$i]) eq "ARRAY" && $#{$v->[$i]}==-1) { 
		    $v->[$i] = undef; 
		    next; 
	    }
	}
	# this removes any undefined elements from the array
	@{$v} = grep { defined $_ } @{$v};
    } elsif (ref($v) eq "HASH") {
	foreach my $x (keys %{$v}) {
	    CleanData($v->{$x});
	    if (!defined $v->{$x}) { delete($v->{$x}); next; }
	    if (ref($v->{$x}) eq "ARRAY" && $#{$v->{$x}}==-1) { delete($v->{$x}); next; }
	}
    }
	return $v;
}

sub _Error {
    if (exists $_[0]->YYData->{ERRMSG}) {
		print $_[0]->YYData->{ERRMSG};
		delete $_[0]->YYData->{ERRMSG};
		return;
	};
	my $line = $_[0]->YYData->{LINE};
	my $last_token = $_[0]->YYData->{LAST_TOKEN};
	my $file = $_[0]->YYData->{INPUT_FILENAME};
	
	print "$file:$line: Syntax error near '$last_token'\n";
}

sub _Lexer($)
{
	my($parser)=shift;

    $parser->YYData->{INPUT} or return('',undef);

again:
	$parser->YYData->{INPUT} =~ s/^[ \t]*//;

	for ($parser->YYData->{INPUT}) {
		if (/^\#/) {
			if (s/^\# (\d+) \"(.*?)\"( \d+|)//) {
				$parser->YYData->{LINE} = $1-1;
				$parser->YYData->{INPUT_FILENAME} = $2;
				goto again;
			}
			if (s/^\#line (\d+) \"(.*?)\"( \d+|)//) {
				$parser->YYData->{LINE} = $1-1;
				$parser->YYData->{INPUT_FILENAME} = $2;
				goto again;
			}
			if (s/^(\#.*)$//m) {
				goto again;
			}
		}
		if (s/^(\n)//) {
			$parser->YYData->{LINE}++;
			goto again;
		}
		if (s/^\"(.*?)\"//) {
			$parser->YYData->{LAST_TOKEN} = $1;
			return('TEXT',$1); 
		}
		if (s/^(\d+)(\W|$)/$2/) {
			$parser->YYData->{LAST_TOKEN} = $1;
			return('CONSTANT',$1); 
		}
		if (s/^([\w_]+)//) {
			$parser->YYData->{LAST_TOKEN} = $1;
			if ($1 =~ 
			    /^(coclass|interface|const|typedef|declare|union
			      |struct|enum|bitmap|void|unsigned|signed|import|include
				  |importlib)$/x) {
				return $1;
			}
			return('IDENTIFIER',$1);
		}
		if (s/^(.)//s) {
			$parser->YYData->{LAST_TOKEN} = $1;
			return($1,$1);
		}
	}
}

sub parse_string
{
	my ($data,$filename) = @_;

	my $self = new Parse::Pidl::IDL;

    $self->YYData->{INPUT_FILENAME} = $filename;
    $self->YYData->{INPUT} = $data;
    $self->YYData->{LINE} = 0;
    $self->YYData->{LAST_TOKEN} = "NONE";

	my $idl = $self->YYParse( yylex => \&_Lexer, yyerror => \&_Error );

	return CleanData($idl);
}

sub parse_file($$)
{
	my ($filename,$incdirs) = @_;

	my $saved_delim = $/;
	undef $/;
	my $cpp = $ENV{CPP};
	if (! defined $cpp) {
		$cpp = "cpp";
	}
	my $includes = join('',map { " -I$_" } @$incdirs);
	my $data = `$cpp -P -D__PIDL__$includes -xc $filename`;
	$/ = $saved_delim;

	return parse_string($data, $filename);
}

1;
