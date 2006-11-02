#!/bin/sh

current=`pwd`

cd $current/default
make
echo "linker may fail, but it's ok ..."
cat all.s | perl ../conv.pl def_ > ../pv_def.nas

cd $current/sse
make
echo "linker may fail, but it's ok ..."
cat all.s | perl ../conv.pl sse_ > ../pv_sse.nas

