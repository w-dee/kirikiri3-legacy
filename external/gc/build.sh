#!/bin/sh -e

prefix=`pwd`

common_configure_options="
	--prefix=$prefix
	--enable-cplusplus
	"

. ../build_common.sh

# この時点でカレントディレクトリはgcのソースを展開したディレクトリのはず

# gcのヘッダ構造は、includeの下のgcに置いてあるファイルと
# include 直下に置いてあるファイルが重複してあるので修正する
mv $prefix/include/gc/* $prefix/include/
rmdir $prefix/include/gc

# gcの動作をカスタマイズする必要があるので private ヘッダファイルもコピーする
cp -pR include/private $prefix/include


