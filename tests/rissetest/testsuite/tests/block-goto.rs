var repeat_4 = function() block {
	for(var i = 4; i <= 7; i++) block(i);
};

var n = 0;

repeat_4() { |i|
	if(i == 5)
	{
		n = i;
		goto exit;
	}
};

exit:

return n; //=> 5

