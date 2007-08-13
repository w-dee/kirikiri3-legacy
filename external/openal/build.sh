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

