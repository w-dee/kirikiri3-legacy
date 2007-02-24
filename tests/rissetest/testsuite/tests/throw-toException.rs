function throw0()
{
	try
	{
		try
		{
			throw 4321;
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
}

function throw1()
{
	try
	{
		try
		{
			throw "!";
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
}

function throw2()
{
	try
	{
		try
		{
			throw RuntimeException;
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
}

function throw3()
{
	try
	{
		try
		{
			throw new RuntimeException("!");
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
}



return '' + throw0() + throw1() + throw2() + throw3();
	//=> "4321!!"

