#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--prefix=$prefix
	"

. ../build_common.sh

