#!/bin/sh -e

prefix=`pwd`

ogg_dir=$prefix/../libogg

case "`uname -s`" in
CYGWIN* | MINGW*)
	LDFLAGS="$LDFLAGS -L$ogg_dir/lib -logg.dll"
	;;
*)
	LDFLAGS="$LDFLAGS -L$ogg_dir/lib -logg"
	;;
esac

export LDFLAGS
CFLAGS="$CFLAGS -I$ogg_dir/include"
export CFLAGS

common_configure_options="
	--enable-shared --prefix=$prefix
	"

. ../build_common.sh

