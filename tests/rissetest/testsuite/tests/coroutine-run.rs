class A extends Coroutine
{
	function run(arg)
	{
		var count = arg;
		while(true) {
			var res = "a:" + count.toString() + " ";
			count +=    yield(res);
		}
	}
}

class B extends Coroutine
{
	function run(arg)
	{
		var count = arg;
		while(true) {
			var res = "b:" + count.toString() + " ";
			count +=    yield(res);
		}
	}
}


var a = new A();
var b = new B();

a.resume(0)+
b.resume(1)+
a.resume(2)+
b.resume(3)+
a.resume(4)+
b.resume(5)+
a.resume(6)+
b.resume(7);

  //=> "a:0 b:1 a:2 b:4 a:6 b:9 a:12 b:16 "
