
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
a.resume(2)+
b.resume(3)+
a.resume(4)+
b.resume(5)+
a.resume(6)+
b.resume(7);

  //=> "a:0 b:1 a:2 b:4 a:6 b:9 a:12 b:16 "
