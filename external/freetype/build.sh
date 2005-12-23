#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--enable-shared --prefix=$prefix
	"

. ../build_common.sh

