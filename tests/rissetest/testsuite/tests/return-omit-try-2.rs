function test1(t)
{
	try
	{
		if(t) throw "!"
	}
	catch(e)
	{
		"a"
	}
	finally
	{
		"b"
	}
}

test1(false) + test1(true) //=> "bb"
