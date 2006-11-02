#!/usr/bin/perl

print '%include		"nasm.nah"'. "\n";

$prefix=$ARGV[0];

%externs = ();
%labels = ();
%globals = ();
$out = '';

while($line = <STDIN>)
{
	$line =~ s/PTR//g;  # iranai
	$line =~ s/\b_(\w+?)\b/$1/g;

	if($line =~ /\w+WORD\s+?\w+/)
	{
		$line =~ s/(\w+WORD)\s+?(\w+)/$1 \[$2\]/;
		$externs{$2} = 1; 
	}

	$line =~ s/[()]//g; # iranai
	$line =~ s/XMMWORD//g; # iranai
	$line =~ s/\bst\b/st0/g;
	if($line =~ /\s*\.file/)
	{
	}
	elsif($line =~ /\s*\.intel_syntax/)
	{
	}
	elsif($line =~ /\s*\.text/)
	{
		$out .= "align 16\n";
		$out .= "segment_code\n";
	}
	elsif($line =~ /\s*\.p2align\s*(\d+),,(\d+)/)
	{
		$out .= "align " . ($2+1) . "\n";
	}
	elsif($line =~ /\s*\.def/)
	{
	}
	elsif($line =~ /\s*\.lcomm/)
	{
	}
	elsif($line =~ /\s*\.linkonce/)
	{
	}
	elsif($line =~ /\s*\.section\s*\.rdata\s*,\s*"dr"/)
	{
		$out .= "segment_data_aligned\n";
		$out .= "align 16\n";
	}
	elsif($line =~ /\s*\.section\s*\.text/)
	{
		$out .= "align 16\n";
		$out .= "segment_code\n";
	}
	elsif($line =~ /\s*\.align\s*(\d+)/)
	{
		$out .= "align " . ($1) . "\n";
	}
	elsif($line =~ /\s*\.long\s*([-\w]+)/)
	{
		$out .= "\tDD " . ($1) . "\n";
	}
	elsif($line =~ /\s*\.globl\s*(\w+)/)
	{
		$globals{$1} = 1;
	}
	elsif($line =~ /^(\w+):/)
	{
		$labels{$1} = 1;
		$out .= "$1:\n";
	}
	elsif($line =~ /\s*?movss\s+?(\w+)\s*?,\s*?DWORD\s*?(\[.*?\])/)
	{
		$out .= "\tmovss\t$1,$2\n";
	}
	elsif($line =~ /\s*?movss\s+?\DWORD\s*?(\[.*?\])\s*?,\s*?(\w+)\s*?/)
	{
		$out .= "\tmovss\t$1,$2\n";
	}
	elsif($line =~ /\s*?fld\s+?DWORD\s+?(\w+)/)
	{
		$out .= "\tfld\tDWORD [$1]\n";
	}
	elsif($line =~ /jmp\s*\[\s*DWORD\s*\[(\w+)\]\[0\+(\w+?)\*4\]\]/)
	{
		$out .= "\tjmp\tDWORD [$1+$2*4]\n";
	}
	elsif($line =~ /\s*?(s[hal][lr])\s+?([^\n,]+?)\n/)
	{
		$out .= "\t$1\t$2, 1\n";
	}
	elsif($line =~ /^\//)
	{
	}
	elsif($line =~ /\tandl \$-16, %esp/)
	{
		$out .= "\tand esp, -16\n";
	}
	elsif($line =~ /\s*?call\s+?(\w+)/)
	{
		$externs{$1} = 1; 
		$out .= $line;
	}
	else
	{
		$out .= $line;
	}
}

$out .= "align 16\n";


foreach $extern (keys %externs)
{
	print "externdef $extern\n" unless defined($labels{$extern});
}

foreach $global (keys %globals)
{
	print "globaldef $prefix$global\n";
}

# replace all globals with its prefix
foreach $global (keys %globals)
{
	$out =~ s/\b$global\b/$prefix$global/g;
}


print $out;
