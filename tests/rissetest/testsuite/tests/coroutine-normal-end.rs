class A extends Coroutine
{
	function run(arg)
	{
		// immediate return
	}
}

class B extends Coroutine
{
	function run(arg)
	{
		yield(); // once yield,
		// then return
	}
}


var a = new A();
var b = new B();


a.resume(); // should not cause any exceptions
b.resume(); // should not cause any exceptions
b.resume(); // should not cause any exceptions

"ok" //=> "ok"

