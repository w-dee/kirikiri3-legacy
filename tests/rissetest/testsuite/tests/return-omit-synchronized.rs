var global.locker = new Object();

function t()
{
	synchronized(global.locker)
	{
		"locking!"
	}
}


t(); //=> "locking!"

