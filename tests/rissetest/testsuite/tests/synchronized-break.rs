{
	var test2 = function() {
		while(true)
		{
			synchronized(global)
			{
				synchronized(global)
				{
					break;
				}
			}

			return "not broken";
		}
		return "exited";
	};

	return test2(); //=> "exited"
}
