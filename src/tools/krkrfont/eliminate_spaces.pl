#! /usr/bin/perl


# 入力ファイル中の、先頭、背後の空白、および空行を削除し、
# 標準出力に表示する


open FH, $ARGV[0] or die;

while(<FH>)
{
	s/^\s*(.+)\s*$/$1/s;
	s/^\n$//s;
	print;
}

