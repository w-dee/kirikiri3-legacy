#!/bin/sh -e

prefix=`pwd`

jam_build_cmd()
{
	echo "
#include <stdlib.h>

int main(void)
{
	return system(\"build.bat mingw\");
}
" > launch.c
	gcc -o launch.exe launch.c
	./launch.exe
	mv bin.ntx86/* .
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
}


. ../build_common.sh

