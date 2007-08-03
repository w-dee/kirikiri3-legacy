var str = '';

for(var i = 0; i < 3; i ++)
{
	switch(i)
	{
	case 0:
		str += '*';
	case 1:
		str += '+';
	default:
		str += '-';
	}
}


str; //=> "*+-+--"
