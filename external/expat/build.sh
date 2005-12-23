#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--prefix=$prefix --disable-shared --enable-static
	"


. ../build_common.sh

