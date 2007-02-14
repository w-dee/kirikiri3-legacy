class C
{
	function C_m()
	{
		@value = 4;
	}

	function C_get()
	{
		return @value;
	}
}

class D : C
{
	function D_m()
	{
		@value = 10;
	}

	function D_get()
	{
		return @value;
	}
}


var i = new D();
i.C_m();
i.D_m();
return i.C_get() + i.D_get(); //=> 14
