var res = '';

class TestException extends RuntimeException {}

function make_coro(n)
{
	Coroutine.new() static function(co) {
		while(n--) co.yield();
		throw new TestException("throw!");
	}
}

var ret;

// exception on firsttime run()
var c = make_coro(0);
try
{
	c.run(); 
}
catch(e if e instanceof TestException)
{
	ret += "[ok]";
}

// exception on secondtime run()
var c = make_coro(1);
c.run();
try
{
	c.run(); 
}
catch(e if e instanceof TestException)
{
	ret += "[ok]";
}


ret; //=> "[ok][ok]"

