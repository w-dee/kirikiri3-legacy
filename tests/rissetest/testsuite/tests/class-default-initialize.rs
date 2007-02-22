class C
{
	function initialize(a, b, c)
	{
		this.v = a + ":" + b + ":" + c;
	}
}

class D extends C
{
	function getV() { return this.v; }
}

var i = new D('x', 'y', 'z');

return i.getV(); //=> "x:y:z"

