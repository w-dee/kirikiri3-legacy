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

// PNGファイルのファイル名
var filenames = [
	"tlg5R8G8B8.tlg5",
	"tlg6R8G8B8.tlg6",
	"tlg5A8R8G8B8.tlg5",
	"tlg6A8R8G8B8.tlg6",
	];

// それぞれのPNGファイルに対して
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

	assert(compareFile("/root/media/expected/\{output_filename}", "/root/tmp/\{output_filename}"));

	var output_filename = fs.chopExtension(filename) + "_A8R8G8B8.tlg5";
	stdio.stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg5'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = fs.chopExtension(filename) + "_A8R8G8B8.tlg6";
	stdio.stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg6'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = fs.chopExtension(filename) + "_R8G8B8.tlg5";
	stdio.stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg5'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = fs.chopExtension(filename) + "_R8G8B8.tlg6";
	stdio.stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg6'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

}


stdio.stdout.print("ok"); //=> ok

