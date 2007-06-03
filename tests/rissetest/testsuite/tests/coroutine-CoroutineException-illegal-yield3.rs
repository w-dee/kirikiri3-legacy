var c1 = Coroutine.new() static function(co) {
	while(true) co.yield();
}

var c2 = Coroutine.new() static function(co) {
	while(true) co.yield();
}


c1.run();
c2.run();


try
{
	c1.yield();
}
catch(e if e instanceof CoroutineException)
{
	return e.message + " at " + e.trace[0];
		//=> /^"coroutine is not running at .*?:16"$/
}

