#!/bin/sh -e

prefix=`pwd`
common_configure_options="
	--shared --prefix=$prefix
	--exec_prefix=$prefix/bin
	--libdir=$prefix/lib
	--includedir=$prefix/include"


. ../build_common.sh

