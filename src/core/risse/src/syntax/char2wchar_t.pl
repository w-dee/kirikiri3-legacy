open(FH, $ARGV[0]);


@all = <FH>;


$all = join('', @all);


$all =~ s/\bchar\b/wchar_t/g;
$all =~ s/#include <unistd\.h>//g;
$all =~ s/\bunsigned wchar_t\b/wchar_t/g;
$all =~ s/\bisatty\b/std::isatty/g;
$all =~ s/yy_fatal_error\( msg \)/yy_fatal_error( L##msg )/g;
$all =~ s/\#line (\d+) \"risse.yy.c\"/\#line $1 \"risse.yy.cpp\"/g;
$all =~ s/\#line (\d+) \"risse.l\"/\#line $1 \"..\\bison-flex\\risse.l\"/g;


open(FH,">".$ARGV[0]);
print FH "#include \"risseCommHead.h\"\n";

print FH $all;



