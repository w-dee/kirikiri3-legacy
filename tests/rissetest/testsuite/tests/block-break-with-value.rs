{
	var repeat_4 = function() block {
		for(var i = 4; i <= 7; i++) block(i);
	};

	return repeat_4() { |i|
		if(i == 5) break i;
	}; //=> 5
}

