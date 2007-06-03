{
	var ret;

	goto a;


	b:
		ret += "b";
		goto c;

	a:
		ret += "a";
		goto b;

	c:
		ret += "c";


	try
	{
		goto d;

		e:
			ret += "e";
			goto f;

		d:
			ret += "d";
			goto e;


	}
	catch(e)
	{
	}

	f:
		ret += "f";

	return ret; //=> "abcdef"
}
