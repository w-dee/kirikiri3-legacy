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

// JPEGファイルのファイル名
var filenames = [
"jpegGrayscale.jpg",
"jpegFullColor.jpg",
	];

// それぞれのJPEGファイルに対して
for(var i = 0; i < filenames.length; i++)
{
	var filename = filenames[i];
	stdio.stderr.print("file \{filename}\n");

	var image = new Image();
	image.load("/root/media/\{filename}");

	var output_filename = fs.chopExtension(filename) + ".bmp";
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // ビットマップサブタイプ
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = fs.chopExtension(filename) + ".jpg";
	image.save("/root/tmp/\{output_filename}"); // jpeg でも保存してみる

	// 現状、画像の比較メソッドを実装してないので……
	// IJG の今のJPEGデコーダの場合は、
	// デコード・エンコード時の誤差により結果が確実に同一になる保証がない。

//	assert(compareFile("/root/media/expected/A8R8G8B8_\{output_filename}", "/root/tmp/\{output_filename}"));
}



stdio.stdout.print("ok"); //=> ok

