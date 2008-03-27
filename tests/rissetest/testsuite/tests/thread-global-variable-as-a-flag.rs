import * in thread;

class T2 extends Thread
{
	function run()
	{
		global.f = "t2 run";
	}
}


for(var i = 0; i < 10; i++)
{
	var global.f = "initialized";

	var t1 = Thread.new()  function ()
	{
		global.f = "thread run";
		while(global.f != "t2 run") ;
		global.f = "end";
	};

	var t2 = T2.new();

	t1.start();

	while(global.f != "thread run") ;

	t2.start();


	while(global.f != "end") ;

	t1.join();

	t2.join();

}

global.f; //=> "end"

