var global.value = "global";

var global.m1 = function () { return this.value; } incontextof dynamic;
var global.m2 = function () { return this.value; } incontextof global;


class C
{
	function initialize()
	{
		var this.value = "instance";
	}

	function test1()
	{
		return global::m1();
	}

	function test2()
	{
		return global::m2();
	}

	function test3()
	{
		return global.m1();
	}

	function test4()
	{
		return global.m2();
	}
}


return	C.new().test1() + "-" + C.new().test2() + "-" +
		C.new().test3() + "-" + C.new().test4(); //=> "instance-global-global-global"



