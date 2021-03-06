import * in coroutine;

function make_coro(n) {
	var c = Coroutine.new() static function(arg, co) {
		while(n--) co.yield();
	}
	c.resume(); // initial run
	c;
}


var ret = '';
var c = make_coro(0);
ret += (c.alive ? "t" : "f");
var c = make_coro(1);
ret += (c.alive ? "t" : "f");
c.resume();
ret += (c.alive ? "t" : "f");

function make_coro() {
	Coroutine.new() static function(arg, co) {
		throw new RuntimeException("hoge");
	}
}

var c = make_coro();
try
{
	c.resume(); // will throw exception
}
catch(e)
{
}
ret += (c.alive ? "t" : "f"); // should be false



ret; //=> "ftff"
