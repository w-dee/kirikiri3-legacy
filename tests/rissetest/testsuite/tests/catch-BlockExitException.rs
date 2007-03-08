function callback() b { return b(); }

return callback() {
	try
	{
		break "this should not be caught";
	}
	catch(e if true)
	{
		return "caught";
	}
	catch(e)
	{
		return "caught";
	}
};

//=> "this should not be caught"
