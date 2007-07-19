class C extends Test
{
	function initialize()
	{
		propString = "str";
		var s = propString;
		propString = s + "!";
	}
}
var obj = new C();

obj.propString; //=> "str!"

