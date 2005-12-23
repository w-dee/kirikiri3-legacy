
このディレクトリで make を打つと必要なライブラリがダウンロードされ、ビルドされます。
ビルドには長い時間がかかります。気長に待ってください。

展開されたライブラリのソースを手動で修正して、そのライブラリだけを再構築し
たい場合は、各ライブラリのトップディレクトリにある build.sh を実行してください。
( あるいは、 build_completed ファイルを削除して make )

make clean と打つと、ディレクトリをクリーンにすることが出来ます。
make download_lib と打つと、ファイルのダウンロードだけを行うことが出来ます。

ライブラリのダウンロード元はすべて http://kikyou.info/tvp/build_libs です。
これは Makefile_common の中で定義されています。ミラーサイト(もしあれば)を
使う場合は手動で修正してください。

ダウンロードには wget を使用します。proxy の設定が必要な場合は

$ http_proxy=gw1
$ export http_proxy

などとして設定してください。


各ライブラリの(ビルドできることの確認が採れている)バージョンについては、
各サブディレクトリ中の Makefile を参照してください。


以下は、自分でダウンロードするとき等に参考にしてください。

・zlib

  入手先: http://www.zlib.net/


・libpng

  入手先: http://www.libpng.org/pub/png/libpng.html

  必ず -config というファイル名のついたアーカイブを使用してください。

・libjpeg

  入手先: http://www.ijg.org/

・expat

  入手先: http://expat.sourceforge.net/

・wxWidgets

  入手先: http://www.wxwidgets.org/

・icu

  入手先: http://www-306.ibm.com/software/globalization/icu/index.jsp
  (アドレスが時々変わるようです。ICU UNICODE などの検索キーワードで探してみてください)

・libtomcrypt

  入手先: http://libtomcrypt.org/

・freetype

  入手先: http://libtomcrypt.org/

・boost

  入手先: http://www.boost.org/

  ビルド後に Not all Boost libraries built properly. と出るがキニシナイ方向で。

  ちなみに執筆時点では

  ...failed updating 120 targets...
  ...skipped 16 targets...
  Not all Boost libraries built properly.

  と出ました。これよりも failed のターゲットが極端に多い場合は ICU が正しく
  コンパイルできていなかったりする可能性が高いです。

--

gzip: stdin: not in gzip format

などと出たときは、ダウンロードしたファイルが壊れてます。
とくに sourforge からダウンロードした場合、実はダウンロードしたのはファイ
ルそのものではなくてファイルをダウンロードするためのページだったというこ
とがあります ( この場合、ファイルサイズは20Kb 未満の場合が多い)。よく確かめ
てみてください。
