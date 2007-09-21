{
	var a = '';
	var b = 0;
	for(var i = 0; i < 4; i++)
	{
		var r = i & 1;
		if(r)
			a += "+",
			b ++;
		else
			a += '-';
	}

	"\{a}:\{b}"; //=> "-+-+:2"
}
