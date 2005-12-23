#!/bin/sh -e

prefix=`pwd`
zlib_dir=$prefix/../zlib
common_configure_options="
	--enable-shared --prefix=$prefix
	"

LDFLAGS="$LDFLAGS -L$zlib_dir/lib"
export LDFLAGS

CFLAGS="$CFLAGS -I$zlib_dir/include"
export CFLAGS


. ../build_common.sh

