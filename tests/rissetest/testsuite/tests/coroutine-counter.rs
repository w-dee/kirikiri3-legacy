
var a = Coroutine.new() static function(co, arg) {
	var count = arg;
	while(true) {
		var res = "a:" + count + " ";
		count += co.yield(res);
	}
}
var b = Coroutine.new()        function(co, arg) {
	// this function's context will be the same as the instance of
	// class Coroutine, because this callback function is not
	// marked as 'static'.
	var count = arg;
	while(true) {
		var res = "b:" + count + " ";
		count +=    yield(res); // so we don't need to prefix 'co.' here.
	}
}

a.run(0)+
b.run(1)+
a.run(2)+
b.run(3)+
a.run(4)+
b.run(5)+
a.run(6)+
b.run(7);

  //=> "a:0 b:1 a:2 b:4 a:6 b:9 a:12 b:16 "
