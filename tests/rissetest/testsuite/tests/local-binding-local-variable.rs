{
	var a = 1
	var b = 2
	var c = 0
	var binding = (@)
	binding['c'] = binding['a'] + binding['b']
	c //=> 3
}

