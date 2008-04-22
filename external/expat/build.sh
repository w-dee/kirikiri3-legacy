#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--prefix=$prefix --enable-shared
	"


. ../build_common.sh

