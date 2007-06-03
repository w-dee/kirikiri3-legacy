function test0()
{
	try
	{
		"a"
	}
	catch(e)
	{
	}
}

function test1()
{
	"-"
	try
	{
		"a"
		throw "a";
	}
	catch(e)
	{
	}
}

function test2()
{
	"-"
	try
	{
		"a"
	}
	catch(e)
	{
	}
	finally
	{
	}
}

function test3()
{
	"-"
	switch(-1) {
	default:
		try {
			"a";
			break;
		} catch(e) {
		}
	}
}

function pv(x) { x===void?"void":x }

pv(test0()) + ":" +  pv(test1()) + ":" + pv(test2()) + ":" + pv(test3()) //=> "a:void:void:a"
