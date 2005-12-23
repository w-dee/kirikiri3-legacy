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


. ../build_common.sh

