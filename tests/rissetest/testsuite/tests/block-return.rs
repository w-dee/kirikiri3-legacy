var repeat_4 = function() block {
	for(var i = 4; i <= 7; i++) block(i);
};

repeat_4() { |i|
	if(i == 5) return i; //=> 5
};
