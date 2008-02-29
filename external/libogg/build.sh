#!/bin/sh -e

prefix=`pwd`

# src_temp にある configure に手を加える
# --image-base=0x10000000 というオプションが指定されているが
# --enable-auto-image-base でないとうまくいかない(原因不明、binutilsのバグ？)
cd src_temp
cat configure | sed 's|image-base=0x10000000|enable-auto-image-base|' > configure.new
mv -f configure.new configure
cd $prefix


common_configure_options="
	--enable-shared --prefix=$prefix
	"

. ../build_common.sh

