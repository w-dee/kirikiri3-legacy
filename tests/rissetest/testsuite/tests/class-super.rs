global.result = "";

class A
{
	function m()
	{
		global.result += "A.m()";
	}
}

class B : A
{
	function m()
	{
		super.m();
		global.result += "B.m()";
	}
}

var i = new B();
i.m();

return global.result; //=> "A.m()B.m()"

