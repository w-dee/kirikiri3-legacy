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

build_release

