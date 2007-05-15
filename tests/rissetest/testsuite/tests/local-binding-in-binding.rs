{
	var a = 1
	var b = 2
	var binding = (@).eval("var c = 4; (@)")

	binding.eval("a + b + c") //=> 7
}
