#!/bin/sh -e

prefix=`pwd`



common_configure_options="
	--prefix=$prefix
	"

subdir_in_src=source

. ../build_common.sh



cd $prefix

# ポストプロセッシング


case "`uname -s`" in
CYGWIN* | MINGW*)
	# まず、DLL の名前がboostが期待する物と違うので、これを変える
	cd lib
	cp icuin.dll icui18n.dll
	# なぜかインポートライブラリが全く作成されていないので作成する
	for file in *.dll; do
		pexports $file > $file.def
		dlltool --dllname $file --input-def $file.def \
			--output-lib lib`echo $file | sed 's/\.dll//'`.a
		rm $file.def
	done;;
esac


