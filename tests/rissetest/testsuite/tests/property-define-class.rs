class C
{
	function initialize()
	{
		var this.v = 0;
	}

	property a
	{
		getter ()  { return -this.v; }
		setter (x) { this.v = -x; }
	}
}

var i = new C();
i.a = 6;
var s1 = i.v;
var s2 = i.a;

return "result: " + s1 + "," + s2; //=> "result: -6,6"

