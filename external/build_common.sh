#!/bin/sh

# ビルド 共通部分

build_release()
{
	if [ "x$custom_build_func" != "x" ]; then
		$custom_build_func
	else
		./configure --prefix=$prefix                $common_configure_options
		make && make install
	fi
}


# ビルドを行う

cd src

for dir in *; do
	if [ -d "$dir" ]; then
		cd $dir
		if [ -d "$subdir_in_src" ]; then
			cd $subdir_in_src
		fi
		build_release
	fi
done

