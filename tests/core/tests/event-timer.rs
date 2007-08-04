// イベントキューと、それを回すためのスレッドを作成する

var queue = new EventQueue();

var thread = Thread.new() function() {
	queue.loop();
}

// キュー用スレッドを開始する
thread.start();

// tick を格納するための配列を用意
var ticks = [];

// タイマーを一つ起動する
class MyTimer extends Timer
{
	function construct() { var this.count = 100; }

	function initialize()
	{
		super::initialize(...);
		interval = 20; // 20ms 周期
		queue = global.queue;
		enabled = true;
	}

	function onTimer(tick)
	{
		// タイマー周期が来た
		ticks.push(tick); // tick を格納
		if(count -- == 0)
		{
			enabled = false;
			queue.quit(); // count 回実行したらキュースレッドを終了させる
		}
	}
}

var timer = new MyTimer();

// キュー用スレッドが終了するまで待つ
thread.join();

// tick が正常っぽいかどうかを判断
var tick_ref = ticks[0];
var jitter = 0.0;
for(var i = 1; i < ticks.length; i++)
{
	var tick = ticks[i];
	var error = tick_ref + i*20 - tick;
	jitter += error * error;
}

if(jitter <= 4000)
{
	// たぶん 4000 以内だったら大丈夫
	System::stdout.print("ok");
}
else
{
	System::stdout.print("too large jitter : \{jitter}\n");
}

//=> ok

