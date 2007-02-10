function C() {
	var a = Array.new();
	for(var i = 0; i < 2; i ++)
	{
		a[i] =  function()
		{
			var r = 0;
			var obj = new Object();
			obj.set = function(x) { r = x; };
			obj.get = function() { return r; };
			return obj;
		};
		a[i] = a[i] ();
	}

	a[0].set(4);
	a[1].set(5);
	return a[0].get();
}


return C(); //=> 4
