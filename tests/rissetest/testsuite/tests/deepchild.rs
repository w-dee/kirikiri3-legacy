// スクリプト言語「りせ」テスト用スクリプト

{
	var n = 3;
	var func = function() {
		var a;
		try {
			var v = a;
			var func = function() { v = n; v += 10; };
			func();
			a = v;
		} catch(e) { }
		n = a;
	};
	func();
	func();
	return n; //=> 23
}
