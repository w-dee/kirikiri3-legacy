{
	var test2 = function() {
		try
		{
			try
			{
				throw "return by exception";
			}
			catch(e)
			{
				throw e;
			}
		}
		catch(e)
		{
			return e;
		}
		return "not caught";
	};

	return test2(); //=> "return by exception"
}
