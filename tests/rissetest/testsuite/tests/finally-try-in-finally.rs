{
	var n = 0;

	try
	{
		n = 1;
	}
	finally
	{
		try
		{
			n = -1;
		}
		catch(e)
		{
		}
	}


	return n; //=> -1
}

