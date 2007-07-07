var c1 = Coroutine.new() static function(arg, co) {
	while(true) co.yield();
}



try
{
	c1.yield();
}
catch(e if e instanceof CoroutineException)
{
	return e.message + " at " + e.trace[0].toString();
		//=> /^"coroutine has not started yet at .*?:9"$/
}

