import * in risa.graphic.image;

// /boot がマウントされているディレクトリを得る

var boot_script_source = File::getFileSystemAt('/boot/').source;

// boot_script_source/../ を /root にマウントする

File::mount('/root', new FileSystem::OSFS("\{boot_script_source}/..", true));

function compareFile(a, b)
{
	return File::open(a) { |st| break st.read() } == File::open(b) { |st| break st.read() };
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
	System::stderr.print("file \{filename}\n");

	var image = new Image();
	image.load("/root/media/\{filename}");

	var output_filename = File::chopExtension(filename) + ".bmp";
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // ビットマップサブタイプ
	image.save("/root/tmp/\{output_filename}", dic);

	assert(compareFile("/root/media/expected/\{output_filename}", "/root/tmp/\{output_filename}"));

	var output_filename = File::chopExtension(filename) + "_A8R8G8B8.tlg5";
	System::stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg5'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = File::chopExtension(filename) + "_A8R8G8B8.tlg6";
	System::stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'A8R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg6'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = File::chopExtension(filename) + "_R8G8B8.tlg5";
	System::stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg5'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

	var output_filename = File::chopExtension(filename) + "_R8G8B8.tlg6";
	System::stderr.print("writing file \{output_filename}\n");
	var dic = new Dictionary();
	dic['_type'] = 'R8G8B8'; // サブタイプ
	dic['_version'] = 'tlg6'; // バージョン
	image.save("/root/tmp/\{output_filename}", dic);

}


System::stdout.print("ok"); //=> ok

