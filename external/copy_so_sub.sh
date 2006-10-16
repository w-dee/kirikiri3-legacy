#!/bin/sh
outdir=$1
while read obj ; do
    objdir=`dirname $obj`
    while test -L $obj ; do
	obj=$objdir/`readlink $obj`
    done;

    cp -p $obj $outdir
    # Ignore が出るのでエラー出力は捨てる
    /sbin/ldconfig -l $outdir/`basename $obj` 2>/dev/null
done

