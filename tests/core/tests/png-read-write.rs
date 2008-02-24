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
	"png256.png",
	"png256a.png",
	"pngA8R8G8B8.png",
	"pngR8G8B8.png",
	"png2.png",
	"png16.png",
	"png16a.png",
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

//	assert(compareFile("/root/media/expected/\{output_filename}", "/root/tmp/\{filename}"));
}

System::stdout.print("ok"); //=> ok

