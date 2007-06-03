このディレクトリは、Phase Vocoder を吉里吉里２にリンクするために必要な
ツールを含んでいます。

$ ./build.sh

のように build.sh を実行すると、pv_def.nas と pv_sse.nas が出力されます。
(この課程で出るリンカのエラーは無視できます)

これを 吉里吉里２の src/core/sound/cpu にコピーし、そこにある compile.bat 
を実行するとこれらをオブジェクトファイルにコンパイルすることができます。

