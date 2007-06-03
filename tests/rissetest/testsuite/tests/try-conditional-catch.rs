class MyExceptionA extends Exception {}
class MyExceptionB extends Exception {}
class MyExceptionAA extends MyExceptionA {}
class MyExceptionBB extends MyExceptionB {}



{
	var test = function(v)
	{
		try
		{
			try
			{
				throw v;
			}
			catch(e if e instanceof MyExceptionA)
			{
				return "A";
			}
			catch(e if e instanceof MyExceptionB)
			{
				return "B";
			}
		}
		catch(e)
		{
			return "non caught exception";
		}
	};

	return test(MyExceptionA) + ":" + test(MyExceptionBB) + ":" + test(Exception);
	 //=> "A:B:non caught exception"
}

