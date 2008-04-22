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
	case "`uname -s`" in
	Linux* )
		CFLAGS=-DPTHREAD_MUTEX_RECURSIVE=PTHREAD_MUTEX_RECURSIVE_NP ./configure $common_configure_options
		;;
	*)
		./configure $common_configure_options
		;;
	esac
	

	# ビルド
	make && make install

	cd $current
}

custom_build_windows()
{
	current=`pwd`

	cd OpenAL-Windows

	# .svn ディレクトリが存在する場合、異常な動作をすることがあるので削除
	rm -rf `find . -name .svn`

	# ビルド
	make && make install

	cd $current
}


. ../build_common.sh

