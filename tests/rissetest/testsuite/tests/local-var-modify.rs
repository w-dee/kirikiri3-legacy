function t() block
{
	block();
}

{
	var v = 1;
	t() { v = 0; }
	if(v)
	{
		return "ng";
	}
	return "ok"; //=> "ok"
}
