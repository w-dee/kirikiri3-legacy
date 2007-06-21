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


	synchronized(global)
	{
		goto d;

		e:
			ret += "e";
			goto f;

		d:
			ret += "d";
			goto e;


	}

	f:
		ret += "f";

	return ret; //=> "abcdef"
}
