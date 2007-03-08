# yytokentype の定義を抜き出して書き出す

infile = ARGV[0]


input = IO.read(infile)

if input =~ /(#ifndef YYTOKENTYPE.*?#endif)/m
	puts "/* do not edit this file by hand ! */"
	puts "/* this file was generated from #{infile} by #{__FILE__} */"
	print $1
	print "\n"
else
	print "yytokentype not found in file #{infile}\n"
	exit 1
end



