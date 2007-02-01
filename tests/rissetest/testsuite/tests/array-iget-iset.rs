var a = new Array("a", "b", "c");
a[3] = "D"; // 新しく追加する
a[-1] = "d";  // 最後の値を上書き
return a[0] + a[1] + a[2] + a[3]; //=> "abcd"

