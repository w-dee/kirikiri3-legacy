#!/bin/sh -e

prefix=`pwd`

case "`uname -s`" in
MINGW* | CYGWIN* )
	# mingw はすこし特殊。
	# 付属の makefile だとうまくビルドできないので、自前の Makefile で
	# ビルドする。
	cp Makefile_GC_msw src_temp/Makefile
	(cd src_temp && make)
	# この時点で gc.dll と libgc.a が できているはず
	# 必要なファイルをコピーする
	mkdir -p $prefix/lib
	cp -p src_temp/gc.dll src_temp/libgc.a $prefix/lib
	cp -pR src_temp/include $prefix
	# rename.h は gc_config_macros.h の前にくっつける
	cat src_temp/rename.h > src_temp/_tmp
	echo "" >> src_temp/_tmp
	cat $prefix/include/gc_config_macros.h >> src_temp/_tmp
	cp src_temp/_tmp $prefix/include/gc_config_macros.h
	rm src_temp/_tmp
	;;

*)
	common_configure_options="
		--prefix=$prefix
		--enable-cplusplus
		"

	. ../build_common.sh
	# この時点でカレントディレクトリはgcのソースを展開したディレクトリのはず

	# gcのヘッダ構造は、includeの下のgcに置いてあるファイルと
	# include 直下に置いてあるファイルが重複してあるので修正する
	mv $prefix/include/gc/* $prefix/include/
	rmdir $prefix/include/gc

	;;

esac



