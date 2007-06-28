var result = "ok";

var obj = new Object();
var obj.v = 0;

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
			synchronized(obj)
			{
				obj.v = 1;
				for(var j = 0; j < 100; j++) ;
				obj.v = 0;
			}
		}
	}
}


var t1 = new T1();
t1.start();

{
	for(var i = 0; i < 10000; i++)
	{
		synchronized(obj)
		{
			// in synchronization block, at this point
			// we should never see obj.v == 1
			if(obj.v == 1) result = "ng";
		}
		for(var j = 0; j < 20; j++) ;
	}
}

t1.terminated = true;
t1.join();

result; //=> "ok"

