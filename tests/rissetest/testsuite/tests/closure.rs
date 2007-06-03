// スクリプト言語「りせ」テスト用スクリプト
{

	var func = function() {
		var a = 0;
		return function () { return ++a; };
	};
	var la = func();
	var la_1 = la();  // la_1 => 1
	var la_2 = la();  // la_2 => 2
	var la_3 = la();  // la_3 => 3
	return la_1 + la_2 + la_3; //=> 6
}
