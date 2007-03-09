function C() {
	return function()
	{
		var r = 0;
		var obj = new Object();
		var obj.set = function(x) { r = x; };
		var obj.get = function() { return r; };
		return obj;
	};
}
var i_obj = C()();
var j_obj = C()();
i_obj.set(4);
return j_obj.get(); //=> 0

