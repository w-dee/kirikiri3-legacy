#!/bin/sh -x

current=`dirname $0`
cd $current
current=`pwd`

cd $current
rm -f pv_*.nas

cd $current/default
rm -f all.s
make
echo "linker may fail, but it's ok ..."
cat all.s | perl ../conv.pl def_ > ../pv_def.nas

cd $current/sse
rm -f all.s
make
echo "linker may fail, but it's ok ..."
cat all.s | perl ../conv.pl sse_ > ../pv_sse.nas

