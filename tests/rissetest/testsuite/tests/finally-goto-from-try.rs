{
	var n = 0;

	try
	{
		goto try_exit;
	}
	finally
	{
		n = -1;
	}

	try_exit:

	return n; //=> -1
}
