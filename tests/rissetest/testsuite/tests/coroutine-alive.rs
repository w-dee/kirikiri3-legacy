function make_coro(n) {
	var c = Coroutine.new() static function(co) {
		while(n--) co.yield();
	}
	c.run(); // initial run
	c;
}


var ret = '';
var c = make_coro(0);
ret += (c.alive ? "t" : "f");
var c = make_coro(1);
ret += (c.alive ? "t" : "f");
c.run();
ret += (c.alive ? "t" : "f");

function make_coro() {
	Coroutine.new() static function(co) {
		throw new RuntimeException("hoge");
	}
}

var c = make_coro();
try
{
	c.run(); // will throw exception
}
catch(e)
{
}
ret += (c.alive ? "t" : "f"); // should be false



ret; //=> "ftff"
