{
	var repeat_4 = function() block {
		var sum = 0;
		for(var i = 1; i <= 4; i++) sum += block(i);
		return sum;
	};

	return repeat_4() { |i| continue i*i; }; //=> 30
}

