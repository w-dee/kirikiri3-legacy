#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--prefix=$prefix
	"

case "`uname -s`" in
MINGW* | CYGWIN* )
	custom_build_func=custom_build_windows
	;;
*)
	custom_build_func=custom_build
	;;
esac


custom_build()
{
	current=`pwd`

	cd OpenAL-Sample

	# configure
	./configure $common_configure_options

	# �r���h
	make && make install

	cd $current
}

custom_build_windows()
{
	current=`pwd`

	cd OpenAL-Windows

	# .svn �f�B���N�g�������݂���ꍇ�A�ُ�ȓ�������邱�Ƃ�����̂ō폜
	rm -rf `find . -name .svn`

	# �r���h
	make && make install

	cd $current
}


. ../build_common.sh

