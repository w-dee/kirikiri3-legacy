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
var stream = File::open('/tmp/Test.txt');

//=> ok
