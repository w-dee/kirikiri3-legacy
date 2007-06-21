{
	var a = 0;
	for(var  n = 0; n <= 6; n++)
	{
		synchronized(global)
		{
			if(n&1) goto cont;
			a += n;
		}

	cont:
	}
	return a; //=> 12
}

