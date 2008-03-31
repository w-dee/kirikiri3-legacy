import * in risa.graphic.image;
import risa.fs as fs;
import risa.fs.osfs as osfs;
import risa.stdio as stdio;

// /boot がマウントされているディレクトリを得る

var boot_script_source = fs.getFileSystemAt('/boot/').source;

// boot_script_source/../ を /root にマウントする

fs.mount('/root', new osfs.OSFS("\{boot_script_source}/..", true));

function compareFile(a, b)
{
	return fs.open(a) { |st| break st.read() } == fs.open(b) { |st| break st.read() };
}


var pixels = [
	[ 0xff000000, 0xff0000ff, 0xff00ff00 ],
	[ 0xffff0000, 0xffff00ff, 0xffffff00 ],
	[ 0xffffffff, 0xffffffff, 0xffffffff ],
	[ 0x80000000, 0x800000ff, 0x8000ff00 ],
	[ 0x80ff0000, 0x80ff00ff, 0x80ffff00 ],
	[ 0x80ffffff, 0x80ffffff, 0x80ffffff ],
];


// 画像を作成
var image = new Image();
image.allocate(3,6);
for(var y = 0; y < 6; y++)
{
	for(var x = 0; x < 3; x++)
	{
		image.setARGB32(x, y, pixels[y][x]);
	}
}

// ピクセルを読み出して同じ値が得られるかチェック
for(var y = 0; y < 6; y++)
{
	for(var x = 0; x < 3; x++)
	{
		assert(image.getARGB32(x, y) == pixels[y][x]);
	}
}

image.save("/root/tmp/pixelsR8G8B8.bmp");

var dic = new Dictionary();
dic['_type'] = 'A8R8G8B8'; // ビットマップサブタイプ
image.save("/root/tmp/pixelsA8R8G8B8.bmp", dic);

assert(compareFile("/root/tmp/pixelsR8G8B8.bmp", "/root/media/expected/pixelsR8G8B8.bmp"));
assert(compareFile("/root/tmp/pixelsA8R8G8B8.bmp", "/root/media/expected/pixelsA8R8G8B8.bmp"));

stdio.stdout.print("ok"); //=> ok

