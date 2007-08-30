// /boot がマウントされているディレクトリを得る

var boot_script_source = File::getFileSystemAt('/boot/').source;

// boot_script_source/../media を /media にマウントする

File::mount('/media', new FileSystem::OSFS("\{boot_script_source}/../media", true));

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
		label_sum = label_sum * 10 + (+name);
	}
}


var c = new MySound();
c.open("/media/muted_conga_hi.ogg");

c.play();

// あー、まだなにかを待つって関数を用意してなかったな
var th = Thread.new() function()
{
	while(c.status != 1); // 再生が停止するまで無限ループ
}


th.start();
th.join();


// この時点で c.label_sum = 1234 のはず

assert(c.label_sum == 1234);

System::stdout.print("ok"); //=> "ok"
