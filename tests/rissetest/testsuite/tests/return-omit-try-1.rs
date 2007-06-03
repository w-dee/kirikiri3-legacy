function test1()
{
	try
	{
		throw "!"
	}
	catch(e)
	{
		"a"
	}
}

test1() //=> "a"
