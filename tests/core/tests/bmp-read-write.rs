import * in risa.graphic.image;
import risa.fs as fs;
import risa.stdio as stdio;

// /boot がマウントされているディレクトリを得る

var boot_script_source = fs.getFileSystemAt('/boot/').source;

// boot_script_source/../ を /root にマウントする

fs.mount('/root', new fs.OSFS("\{boot_script_source}/..", true));

function compareFile(a, b)
{
	return fs.open(a) { |st| break st.read() } == fs.open(b) { |st| break st.read() };
}

// ビットマップファイルのファイル名
var filenames = [
	"bmpR5G6R5.bmp",
	"bmpR8G8B8.bmp",
	"bmpX1R5G5B5.bmp",
	"bmpX8R8G8B8.bmp",
	"bmp2.bmp",
	"bmp16.bmp",
	"bmp256.bmp",
	"bmpA1R5G5B5.bmp",
	"bmpA8R8G8B8.bmp",
	];

// それぞれのビットマップに対して
for(var i = 0; i < filenames.length; i++)
{
	var filename = filenames[i];
	stdio.stderr.print("file \{filename}\n");

	var image = new Image();
	image.load("/root/media/\{filename}");
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // ビットマップサブタイプ
	image.save("/root/tmp/\{filename}", dic);

	assert(compareFile("/root/media/expected/\{filename}", "/root/tmp/\{filename}"));
}

stdio.stdout.print("ok"); //=> ok

