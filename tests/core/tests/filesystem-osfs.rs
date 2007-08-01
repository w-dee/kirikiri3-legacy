// /boot がマウントされているディレクトリを得る

var boot_script_source = File::getFileSystemAt('/boot/').source;

// boot_script_source/../tmp を /tmp にマウントする

File::mount('/tmp', new FileSystem::OSFS("\{boot_script_source}/../tmp", true));

// ファイルを作成して書き込んでみる
File::open('/tmp/test.txt', File::omWrite).print("Hello world!\n").dispose();

// そのファイルを読んでみる
var stream = File::open('/tmp/test.txt');
var data = stream.read();
stream.dispose();
assert(data == (octet)"Hello world!\n");

// 大文字と小文字が違うファイルを開こうとしてみる
try {
	stream = File::open('/tmp/Test.txt');
} catch(e) {
	assert(e.class === IOException);
}

// サブディレクトリを作る
File::createDirectory('/tmp/subdir');

// ディレクトリは作られたか？
assert(File::isDirectory('/tmp/subdir'));
assert(File::isDirectory('/tmp/subdir/'));
assert(File::isDirectory('/tmp/subdir//')); // 一応legal
assert(File::isDirectory('/tmp//subdir//')); // 一応legal
assert(!File::isFile('/tmp/subdir'));

// そこにファイルを作ってみる
File::open('/tmp/subdir/test2.txt', File::omWrite).print("Hello world?\n").dispose();

// ファイルは作られたか？
assert(File::isFile('/tmp/subdir/test2.txt'));
assert(!File::isDirectory('/tmp/subdir/test2.txt'));
assert(File::isFile('/tmp/subdir//test2.txt')); // 一応 legal

// もう一個ファイルを作ってみる
File::open('/tmp/subdir/test3.txt', File::omWrite).print("Hello world!?\n").dispose();


// トラバースしてみる
var files = '';
File::walkAt('/tmp/subdir') { |name| files += ':' + name }
p(files);



//=> ok
