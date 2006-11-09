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
	elsif($line =~ /\s*?([a-z]+ss)\s+?(.*?)DWORD(.*)/)
	{
		$out .= "\t$1\t$2 $3\n";
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

$head = '';

foreach $extern (keys %externs)
{
	$head .= "externdef $extern\n" unless defined($labels{$extern});
}

foreach $global (keys %globals)
{
	$head .= "globaldef $prefix$global\n";
}

# replace all globals with its prefix
foreach $global (keys %globals)
{
	$out =~ s/\b$global\b/$prefix$global/g;
	$globals{"$prefix$global"} = 1;
}


$out = $head . $out;


# replace all local label name according to lines around the label
# this is to minimize differences between revisions
sub make_hash
{
	my($ident) = @_;
	my($c, @n, $o, $ret, $i, $cn);
	@n = (0,0,0,0,0,0,0,0);

	for($i = 0; $i < length($ident); $i++)
	{
		$cn = ord(substr($ident, $i, 1));
		$n[$i % 8] += $cn;
	}

	$ret = '';
	foreach $c (@n)
	{
		$ret .= substr("qwertyuioplkjhgfdsazxcvbnmMNBVCXZASDFGHJKLPOIUYTREWQ", $c%(26*2), 1);
	}
	return $ret;
}

@lines = split(/\n/s, $out);
%known_labels = ();
for($i = 0; $i <= $#lines; $i++)
{
	$line = $lines[$i];
	if($line =~ /^(L\w+):/)
	{
		$label = $1;
		if(!defined($globals{"$label"}))
		{
			$ident = '';
			$ic = 0;
			$ii = $i - 1;
			while($ic < 5  && $ii >= 0)
			{
				if($lines[$ii] !~ /^(\w+):/)
				{
					$ident .= $lines[$ii];
					$ic++;
				}
				$ii--;
			}
			$ic = 0;
			$ii = $i + 1;
			while($ic < 18 && $ii <= $#lines)
			{
				if($lines[$ii] !~ /^(\w+):/)
				{
					$ident .= $lines[$ii];
					$ic++;
				}
				$ii++;
			}
			$hash = &make_hash($ident);
			if(defined($known_labels{$hash}))
			{
				print "hash conflict!!!\n";
				exit 1;
			}
			$known_labels{$hash} = $label;
		}
	}
}

foreach $to (keys %known_labels)
{
	$from = $known_labels{$to};
	$out =~ s/\b$from\b/$to/g;
}

print $out;
