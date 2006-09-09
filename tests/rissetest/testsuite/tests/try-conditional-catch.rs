var test = function(v)
{
	try
	{
		try
		{
			throw v;
		}
		catch(e if e < 0)
		{
			return "negative";
		}
		catch(e if e > 0)
		{
			return "positive";
		}
		catch(e if e === 0)
		{
			return "zero";
		}
	}
	catch(e)
	{
		return "non caught exception";
	}
};

return test(-1) + ":" + test(1) + ":" + test(0) + ":" + test("hoge");
 //=> "negative:positive:zero:non caught exception"

