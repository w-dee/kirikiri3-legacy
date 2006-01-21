
cd `dirname $0`

(cd src/tools/uniqidgen && make)

check()
{
	file=$1
	echo -n $file ...
	(grep 'RISSE_DEFINE_SOURCE_ID([0-9]*)' $file > /dev/null) &&
		(
			awk '/RISSE_DEFINE_SOURCE_ID(\d*)/ {
				printf "%s", "RISSE_DEFINE_SOURCE_ID(";
				system("src/tools/uniqidgen/uniqidgen");
				printf "%s", ");\n";
				}
				! /RISSE_DEFINE_SOURCE_ID(\d*)/ { print }
				' $file > $file.tmp
			mv -f $file.tmp $file
			echo -n " updated"
		)
	echo ""
}

for dir in src tests; do
	find $dir -name '*.cpp' | while read n; do check $n; done
done
