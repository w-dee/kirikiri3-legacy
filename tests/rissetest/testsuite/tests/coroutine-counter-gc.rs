
function test_one()
{
	var a = Coroutine.new() static function(arg, co) {
		var count = arg;
		while(true) {
			var res = "a:" + count + " ";
			count += co.yield(res);
		}
	}
	var b = Coroutine.new()        function(arg, co) {
		// this function's context will be the same as the instance of
		// class Coroutine, because this callback function is not
		// marked as 'static'.
		var count = arg;
		while(true) {
			var res = "b:" + count + " ";
			count +=    yield(res); // so we don't need to prefix 'co.' here.
		}
	}

	a.resume(0)+
	b.resume(1)+
	a.resume(0)+
	b.resume(1)+
	a.resume(0)+
	b.resume(1)+
	a.resume(0)+
	b.resume(1)+
	a.resume(0)+
	b.resume(1);
}

for(var i = 0; i < 1000; i++)
{
	test_one();
}

"ok" //=> "ok"

/*
	This test ensures that the GC correctly releases these Coroutines.
*/

