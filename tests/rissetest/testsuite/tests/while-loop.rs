{
	var i = 0;
	var sum = 0;
	while( i < 10 )
	{
		i++;

		if(i == 2) continue;
		if(i == 8) break;
		sum += i;
	}
	return sum; //=> 26
}
