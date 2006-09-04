
var test2 = function() {
	var a = 0;
	for(var  n = 0; n <= 6; n++)
	{
		try
		{
			if(n&1) continue;
			a += n;
		}
		catch(e)
		{
			return "exception b : " + e;
		}
	}
	return a;
};

return test2(); //=> 12
