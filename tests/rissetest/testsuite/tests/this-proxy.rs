var a = "global-a";
var b = "global-b";
var c = "global-c";
var d = "global-d";

class C
{
	var b = "class-b";

	function initialize()
	{
		var this.c = "instance-c";
	}

	function test()
	{
		var d = "local-d";

		var s1 = a + ":" + b + ":" + c + ":" + d;

		a = "@";
		b = "@";
		c = "@";
		d = "@";

		var s2 = a + ":" + b + ":" + c + ":" + d;

		return s1 + ":" + s2;
	}
}

return C.new().test(); //=> "global-a:class-b:instance-c:local-d:@:@:@:@"
