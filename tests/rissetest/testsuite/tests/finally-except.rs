{
	var n;

	try
	{
		try
		{
			throw "exception!";
		}
		finally
		{
			n = -1;
		}
	}
	catch(e)
	{
		return "exception thrown, but n = " + n.toString();
			//=> "exception thrown, but n = -1"
	}
}

