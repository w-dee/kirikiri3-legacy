open(FH, $ARGV[0]);


@all = <FH>;


$all = join('', @all);


$all =~ s/(YYSTYPE;)/$1\nYYLEX_PROTO_DECL\n/i;
$all =~ s/NS_RISSE_START/namespace Risse \{/;
$all =~ s/\#line (\d+) \"risse.y\"/\#line $1 \"syntax\/risse.y\"/g;
$all =~ s/\#line (\d+) \"risse.yy.c\"/\#line $1 \"syntax\/risse.yy.cpp\"/g;
$all =~ s/\#line (\d+) \"rissepp.y\"/\#line $1 \"syntax\/rissepp.y\"/g;
$all =~ s/\#line (\d+) \"rissepp.yy.c\"/\#line $1 \"syntax\/rissepp.yy.cpp\"/g;
;# $all =~ s/\"\, expecting \`\" : \" or \`\"/\"\, expecting \" : \" or \"/g;
;# $all =~ s/strcat\(msg\, \"\'\"\)\;//g;

open(FH,">".$ARGV[0]);
print FH "#include \"risseCommHead.h\"\n";
print FH $all;
print FH "\n}\n";



