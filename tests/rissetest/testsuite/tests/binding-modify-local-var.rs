{
	var a = 1
	var b = 2
	var c = 3
	var binding = (@)
	binding['c'] = 5
	binding['a'].toString() + ':' + binding['b'].toString() + ':' + c.toString() //=> "1:2:5"
}
