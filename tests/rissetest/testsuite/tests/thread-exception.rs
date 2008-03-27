import * in thread;

var t = Thread.new() function() {
	throw "Exception!";
}

t.start();

try
{
	t.join();
}
catch(e)
{
	return e.toString(); //=> /Exception! at testsuite\/tests\/thread-exception.rs:\d+/
}

return "?"
