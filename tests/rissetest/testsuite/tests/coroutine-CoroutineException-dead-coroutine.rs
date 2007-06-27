var c = Coroutine.new() static function(co) {
	// immediately return
}


c.resume();

try
{
	c.resume(); // coroutine has already exited
}
catch(e if e instanceof CoroutineException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"coroutine has already exited at .*?:10"$/
}

