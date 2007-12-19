#!/bin/sh -e

prefix=`pwd`


custom_build_func=custom_build


custom_build()
{
	# とりあえず、src/hamigaki を include にコピーする
	cd $prefix
	[ -d include ] || mkdir include
	cp -pR src_temp/hamigaki include
	# svn ディレクトリは一応削除しておく………
	rm -rf `find include -name .svn`
}

. ../build_common.sh




