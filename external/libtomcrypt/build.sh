#!/bin/sh -e

prefix=`pwd`


custom_build_func=custom_build


custom_build()
{
	pwd
	# makefile の修正
	sed \
		-e 's|-I\./testprof/|-I./testprof|' \
		-e 's|-I\./src/headers/|-I./src/headers|' \
		makefile > Makefile.new
	rm makefile
	mv Makefile.new Makefile

	# ビルド
	make

	# 対象ディレクトリを作成
	mkdir $prefix/include || true
	mkdir $prefix/lib || true

	# ヘッダファイルをコピー
	cp src/headers/* $prefix/include

	# ライブラリをコピー
	cp *.a $prefix/lib
}

. ../build_common.sh




