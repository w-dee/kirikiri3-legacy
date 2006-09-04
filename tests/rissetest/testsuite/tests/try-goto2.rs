var a = 0;
for(var  n = 0; n <= 6; n++)
{
	try
	{
		if(n&1) goto cont;
		a += n;
	}
	catch(e)
	{
		return "exception b : " + e;
	}
cont:
}
return a; //=> 12

