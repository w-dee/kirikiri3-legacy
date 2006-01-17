#!/bin/sh -e

prefix=`pwd`

prefix_zlib=$prefix/../zlib
prefix_libpng=$prefix/../libpng
prefix_libjpeg=$prefix/../libjpeg
prefix_expat=$prefix/../expat

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

CFLAGS="$CFLAGS $includes"
export CFLAGS

CXXFLAGS="$CXXFLAGS $includes"
export CXXFLAGS

CPPFLAGS="$CPPFLAGS $includes"
export CPPFLAGS

LDFLAGS="$LDFLAGS $libs"
export LDFLAGS


common_configure_options="--with-opengl --enable-exceptions
	--enable-catch_segvs --enable-mousewheel --enable-unicode
	--enable-intl --enable-mslu --disable-compat24 --prefix=$prefix
	--with-zlib=sys
	--with-expat=sys
	--with-libpng=sys
	--with-libjpeg=sys
	
	"

build_debug()
{
	# temp_build ディレクトリを作成
	rm -rf temp_build
	mkdir temp_build
	cd temp_build
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
	# temp_build ディレクトリを作成
	rm -rf temp_build
	mkdir temp_build
	cd temp_build
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

cd src

for dir in *; do
	if [ -d $dir ]; then
		cd $dir
		build_debug
		build_release
	fi
done



