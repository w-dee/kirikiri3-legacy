#!/bin/sh -e

prefix=`pwd`

ogg_dir=$prefix/../libogg

LDFLAGS="$LDFLAGS -L$ogg_dir/lib -logg.dll"
export LDFLAGS
CFLAGS="$CFLAGS -I$ogg_dir/include"
export CFLAGS

common_configure_options="
	--enable-shared --prefix=$prefix
	"

. ../build_common.sh

