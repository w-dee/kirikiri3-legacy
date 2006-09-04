var a = 0;
try
{
	try
	{
		a += 1;
	}
	catch(e)
	{
	}
}
catch(e)
{
}
return a; //=> 1
