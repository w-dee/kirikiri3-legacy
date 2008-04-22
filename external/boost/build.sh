#!/bin/sh -e


# TODO: support platforms other than MinGW/Linux(x86)

prefix=`pwd`

jam_build_cmd()
{
	case "`uname -s`" in
	MINGW* | CYGWIN* )
		# bjam で使われている spawnvp はパスに " (ダブルクオーテーション) やカッコ
		# が含まれていると解釈に失敗する模様。
		# C:/Program Files/Microsoft DirectX SDK (June 2006)/Utilities/Bin/x86
		# などでハマった。
		# 一応ここでもチェックを行う。
		if ! ( echo $PATH | awk '/[\"\(\)]/ { exit 1; }' ); then
			echo "Your PATH contains invalid character! (one of '\"', '(', ')' )"
			echo "PATH is :"
			echo $PATH
			echo "see trouble shooting information at "
			echo "https://sv.kikyou.info/trac/kirikiri/wiki/documents/kirikiri3/development/build_win32"
			exit 1
		fi
		# MINGW の仕様かどうかはわからないが、シェルから
		# 直接バッチファイルを実行できないようなので
		# バッチファイルを引数にしてsystem関数を呼ぶ
		# プログラムをコンパイルしてそれを実行する
		# gcc 4.2 の場合はビルドしたバイナリがうごかない(^^;
		# こういう問題がある模様
		# http://d.hatena.ne.jp/y-hamigaki/20080305#1204718467
		# --debugオプションをつける
		echo "
#include <stdlib.h>

int main(void)
{
	return system(\"build.bat mingw --debug\");
}
	" > launch.c
		gcc -o launch.exe launch.c
		./launch.exe

		

		;;
	*)
		./build.sh --debug
		;;
	esac

	# bin.*.debug にマッチするディレクトリ下のファイルをカレントディレクトリにコピー
	# (必要ないかも?)
	for dir in bin.*.debug; do
		cp -P $dir/* .
	done
}





# boost のビルド

# ↓ ICU を使う場合
#	--with-icu=$prefix/../icu


common_configure_options="
	--prefix=$prefix
	--with-toolset=gcc
	"

case "`uname -s`" in
	MINGW*)
		# --with-toolset=gcc を書く。
		# どうも 1.34 から MSYS がサポートされなくなったらしいorz
		common_configure_options="$common_configure_options
			--with-toolset=gcc
			"
	;;
esac

custom_build_func=custom_build


custom_build()
{
	current=`pwd`

	# bjam のビルド
	cd tools/jam/src

	jam_build_cmd

	cd $current

	# configure
	./configure $common_configure_options --with-bjam=$current/tools/jam/src/bjam

	# ビルド
	make && make install
}


. ../build_common.sh


