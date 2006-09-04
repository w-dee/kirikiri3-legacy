
var test2 = function() {
	while(true)
	{
		try
		{
			try
			{
				break;
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
		return "not broken";
	}
	return "exited";
};

return test2(); //=> "exited"
