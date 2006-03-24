#!/bin/sh -e

prefix=`pwd`

custom()
{
	cp -pR lib include bin $prefix
}

custom_build_func=custom


. ../build_common.sh
