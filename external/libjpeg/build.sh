#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--enable-shared --prefix=$prefix
	"

mkdir  include  || true
mkdir  lib      || true
mkdir  bin      || true
mkdir  man      || true
mkdir  man/man1 || true


# configu.sub がちょっと古いので libpng のやつから拝借する
cp ../libpng/src_temp/config.sub src_temp

. ../build_common.sh

