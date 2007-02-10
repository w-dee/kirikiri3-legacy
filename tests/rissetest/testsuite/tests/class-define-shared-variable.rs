class C {
	function m()
	{
		var r = 0;
		var obj = new Object();
		obj.set = function(x) { r = x; };
		obj.get = function() { return r; };
		return obj;
	}
}

var i = new C();
var i_obj = i.m();
var j = new C();
var j_obj = j.m();

i_obj.set(4);

/*
	同じクラスでも、インスタンスが異なれば共有変数領域は別になるはずである
*/

return j_obj.get(); //=> 0
