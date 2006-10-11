#!/bin/sh -e


# TODO: support platforms other than MinGW/Linux(x86)

prefix=`pwd`

jam_build_cmd()
{
	case "`uname -s`" in
	MINGW* | CYGWIN* )
		# MINGW の仕様かどうかはわからないが、シェルから
		# 直接バッチファイルを実行できないようなので
		# バッチファイルを引数にしてsystem関数を呼ぶ
		# プログラムをコンパイルしてそれを実行する
		echo "
#include <stdlib.h>

int main(void)
{
	return system(\"build.bat mingw\");
}
	" > launch.c
		gcc -o launch.exe launch.c
		./launch.exe
		;;
	*)
		./build.sh
		;;
	esac

	# bin. で始まるディレクトリ下のファイルをカレントディレクトリにコピー
	for dir in bin.*; do
		cp $dir/* .
	done
}





# boost のビルド

common_configure_options="
	--prefix=$prefix
	--with-icu=$prefix/../icu
	"


custom_build_func=custom_build


custom_build()
{
	current=`pwd`

	# bjam のビルド
	cd tools/build/jam_src

	jam_build_cmd

	cd $current

	# configure
	./configure $common_configure_options

	# ビルド
	make && make install

	case "`uname -s`" in
	    CYGWIN* | MINGW*)
	    # lib 中の *.lib を *.a に
		cd $prefix/lib
		for fname in *.lib; do mv $fname ${fname%.lib}.a; done
		cd $current
		;;
	esac
}


. ../build_common.sh


