{
	var test2 = function() {
		synchronized(global)
		{
			synchronized(global)
			{
				return "return by exception";
			}
		}

		return "not caught";
	};

	return test2(); //=> "return by exception"
}
