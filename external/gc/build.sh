#!/bin/sh -e

prefix=`pwd`

case "`uname -s`" in
MINGW* | CYGWIN* )
	# mingw はすこし特殊。
	# 付属の makefile だとうまくビルドできないので、自前の Makefile で
	# ビルドする。
	cp Makefile_GC_msw src_temp/Makefile
	(cd src_temp && make)
	# この時点で gc.dll と gc.a が できているはず
	# 必要なファイルをコピーする
	mkdir -p $prefix/lib
	cp -p src_temp/gc.dll src_temp/gc.a $prefix/lib
	cp -pR src_temp/include $prefix

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



