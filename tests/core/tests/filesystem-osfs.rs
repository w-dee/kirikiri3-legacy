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

// もう一個ファイルを作ってみる(今度はブロック付きで)
File::open('/tmp/subdir/test3.txt', File::omWrite) { |stream| stream.print("Hello world!?\n") };



// サブディレクトリを作る
File::createDirectory('/tmp/subdir/subfolder');

// ディレクトリは作られたか？
assert(File::isDirectory('/tmp/subdir/subfolder'));
assert(File::isDirectory('/tmp/subdir//subfolder'));

// もう一個ファイルを作ってみる
File::open('/tmp/subdir/subfolder/test4.txt', File::omWrite) { |stream| stream.print("Hello world!!?\n") };

// もう一個サブディレクトリを作る
File::createDirectory('/tmp/subdir/subfolder/subtree');

// もう一個ファイルを作ってみる
File::open('/tmp/subdir/subfolder/subtree/test5.txt', File::omWrite) { |stream| stream.print("Hello world!!!?\n") };


// トラバースしてみる
var file_expected = 3;
var count = File::walkAt('/tmp/subdir') { |name, is_dir|
	if     (name == 'test2.txt' && !is_dir) file_expected--;
	else if(name == 'test3.txt' && !is_dir) file_expected--;
	else if(name == 'subfolder' &&  is_dir) file_expected--;
	else file_expected = -1;
}
assert(file_expected == 0);
assert(count == 3);


// 再帰的にトラバースしてみる
var file_expected = 6;
var count = File::walkAt('/tmp/subdir', true) { |name, is_dir|
	if     (name == 'test2.txt' && !is_dir) file_expected--;
	else if(name == 'test3.txt' && !is_dir) file_expected--;
	else if(name == 'subfolder' &&  is_dir) file_expected--;
	else if(name == 'subfolder/test4.txt' && !is_dir) file_expected--;
	else if(name == 'subfolder/subtree' && is_dir) file_expected--;
	else if(name == 'subfolder/subtree/test5.txt' && !is_dir) file_expected--;
	else file_expected = -1;
}
assert(file_expected == 0);
assert(count == 6);

// ファイルの削除
File::removeFile('/tmp/subdir/test2.txt');
File::removeFile('/tmp/subdir/subfolder/subtree/test5.txt');
assert(!File::isFile('/tmp/subdir/test2.txt'));
assert(!File::isFile('/tmp/subdir/subfolder/subtree/test5.txt'));

// ディレクトリの削除
File::removeDirectory('/tmp/subdir/subfolder/subtree');
assert(!File::isDirectory('/tmp/subdir/subfolder/subtree'));

// ファイルの削除とディレクトリの削除
File::removeFile('/tmp/subdir/subfolder/test4.txt');
File::removeDirectory('/tmp/subdir/subfolder/');
assert(!File::isDirectory('/tmp/subdir/subfolder/'));

// クリーンナップ。
// OSFS は安全のため、ファイルの再帰的な削除はサポートされていない。
File::removeFile('/tmp/subdir/test3.txt');
File::removeDirectory('/tmp/subdir');

System::stdout.print("ok"); //=> ok
