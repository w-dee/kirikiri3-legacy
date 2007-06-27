var c = Coroutine.new() static function(co) {
	// immediately return
}
c.resume(); // will finish coroutine


try
{
	c.yield();
}
catch(e if e instanceof CoroutineException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"coroutine has already exited at .*?:9"$/
}

