#!/bin/sh -e

prefix=`pwd`

# TODO: GC のスレッドのサポートを wxWidgets でも有効にしなければならない
# (RisseのtRisseThreadはwxThreadをベースにしているため)

# TODO: 各プラットフォームに応じて GC に必要なオプションが変わるので
# 注意。


prefix_zlib=$prefix/../zlib
prefix_libpng=$prefix/../libpng
prefix_libjpeg=$prefix/../libjpeg
prefix_expat=$prefix/../expat
prefix_gc=$prefix/../gc

includes=" \
		-I$prefix_zlib/include     \
		-I$prefix_libpng/include   \
		-I$prefix_libjpeg/include  \
		-I$prefix_expat/include    \
	"

libs=" \
		-L$prefix_zlib/lib        \
		-L$prefix_libpng/lib      \
		-L$prefix_libjpeg/lib     \
		-L$prefix_expat/lib       \
	"


case "`uname -s`" in
MINGW* | CYGWIN* )
	;;

Linux*)
	# see details at README.linux of Boehm GC document.
	includes="$includes \
		-I$prefix_gc/include       \
		-DGC_LINUX_THREADS -D_REENTRANT \
		-include gc.h              \
		"
	# Ubuntu Intrepid 対策。これがないとvfwprintfの再定義でエラーになる
	includes="$includes -U_FORTIFY_SOURCE"
	# ライブラリ
	libs="$libs \
		-L$prefix_gc/lib     -lgc \
		"
	;;
*)
	echo "Your platform is not supported yet at this time."
	echo "Please contact W.Dee <dee@kikyou.info>"
	exit 1
	;;
esac


CFLAGS="$CFLAGS $includes"
export CFLAGS

CXXFLAGS="$CXXFLAGS $includes"
export CXXFLAGS

CPPFLAGS="$CPPFLAGS $includes"
export CPPFLAGS

LDFLAGS="$LDFLAGS $libs"
export LDFLAGS


common_configure_options="--with-opengl --enable-exceptions \
	--enable-catch_segvs --enable-mousewheel --enable-unicode \
	--enable-intl --enable-mslu \
	--disable-compat24 \
	--disable-compat26 \
	--prefix=$prefix \
	--with-zlib=sys \
	--with-expat=sys \
	--with-libpng=sys \
	--with-libjpeg=sys \
	--enable-vendor=k3 \
	
	"

build_debug()
{
	# build_debug ディレクトリを作成
	rm -rf build_debug
	mkdir build_debug
	cd build_debug
	# configure
	../configure \
		--disable-shared --enable-static \
		--enable-debug --enable-debug_gdb --enable-debug_cntxt \
		--disable-optimise \
		$common_configure_options
	make
	# mo ファイルだけはなぜか個別に作り直さなければならない
	(cd ../locale && make allmo)
	make install
	cd ..
}

build_release()
{
	# build_release ディレクトリを作成
	rm -rf build_release
	mkdir build_release
	cd build_release
	# configure
	../configure \
		$common_configure_options
	make
	# mo ファイルだけはなぜか個別に作り直さなければならない
	(cd ../locale && make allmo)
	make install
	cd ..
}

# ビルドを行う

cd src_temp

build_debug
build_release



