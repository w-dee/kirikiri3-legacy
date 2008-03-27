import * in thread;

var result = "ok";

class S extends Object
{
	function initialize()
	{
		var this.v = 0;
	}

	synchronized function flag()
	{
		v = 1;
		for(var j = 0; j < 100; j++) ;
		v = 0;
	}

	synchronized function check()
	{
		// in synchronization function, at this point
		// we should never see v == 1
		if(v == 1) global.result = "ng";
	}
}

var obj = new S();

class T1 extends Thread
{
	function initialize()
	{
		var this.terminated = false;
	}

	function run()
	{
		while(!terminated)
		{
			obj.flag();
		}
	}
}


var t1 = new T1();
t1.start();

{
	for(var i = 0; i < 10000; i++)
	{
		obj.check();
		for(var j = 0; j < 20; j++) ;
	}
}

t1.terminated = true;
t1.join();

result; //=> "ok"

