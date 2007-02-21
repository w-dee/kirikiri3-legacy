class A
{
	function initialize()
	{
		this.v = 0;
	}

	function super_m(s)
	{
		v = v + s;
	}

}

class B : A
{
	function initialize()
	{
		super::initialize();
	}

	function sub_m(s)
	{
		super_m(s);
		super::super_m(s);
	}
}


var i1 = new B();


i1.sub_m(1);


return i1.v; //=> 2

