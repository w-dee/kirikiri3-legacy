var res = '';

class TestException extends RuntimeException {}

function make_coro(n)
{
	Coroutine.new() static function(arg, co) {
		while(n--) co.yield();
		throw new TestException("throw!");
	}
}

var ret;

// exception on firsttime resume()
var c = make_coro(0);
try
{
	c.resume(); 
}
catch(e if e instanceof TestException)
{
	ret += "[ok]";
}

// exception on secondtime resume()
var c = make_coro(1);
c.resume();
try
{
	c.resume(); 
}
catch(e if e instanceof TestException)
{
	ret += "[ok]";
}


ret; //=> "[ok][ok]"

