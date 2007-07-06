class C
{
	var value = "static-variable";

	static function m_static()
	{
		return value;
	}

	static property p_static
	{
		getter() { return value; }
	}

	function initialize()
	{
		var this.value = "instance-variable";
	}

	function m_instance()
	{
		return value;
	}

	property p_instance
	{
		getter() { return value; }
	}

}

var i = new C();

return C::m_static() + ":" + C::p_static + ":" + i.m_instance() + ":" + i.p_instance;
	//=> "static-variable:static-variable:instance-variable:instance-variable"
