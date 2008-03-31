import risa.fs as fs;
import risa.fs.osfs as osfs;
import Sound in risa.sound;
import risa.stdio as stdio;
import Thread in thread;
import * in risa.event;

// /boot がマウントされているディレクトリを得る

var boot_script_source = fs.getFileSystemAt('/boot/').source;

// boot_script_source/../media を /media にマウントする

fs.mount('/media', new osfs.OSFS("\{boot_script_source}/../media", true));

// muted_conga_hi.ogg をならす。
// このサウンドは4つのラベルをもっていて、それぞれ 1～4 という名前である

class MySound extends Sound
{
	function construct()
	{
		var this.label_sum = 0;
	}

	function onLabel(name)
	{
		stdio.stderr.print("\{name}\n");
		label_sum = label_sum * 10 + (+name);
	}
}

var queue = new EventQueue();

var c = new MySound();
c.queue = queue;
c.open("/media/muted_conga_hi.ogg");

c.play();

// あー、まだなにかを待つって関数を用意してなかったな
var th = Thread.new() function()
{
	while(c.status != 1); // 再生が停止するまで無限ループ
	queue.quit();
}


th.start();
queue.loop();
th.join();


// この時点で c.label_sum = 1234 のはず

assert(c.label_sum == 1234);

stdio.stdout.print("ok"); //=> ok
