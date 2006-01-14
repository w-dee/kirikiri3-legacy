open(FH, $ARGV[0]);


@all = <FH>;


$all = join('', @all);


open(FH,">".$ARGV[0]);
print FH "namespace Risse {\n";
print FH $all;
print FH "\n}\n";



