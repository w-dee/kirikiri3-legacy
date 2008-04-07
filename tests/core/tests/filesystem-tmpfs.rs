import risa.fs as fs;
import stream;
import risa.stdio as stdio;

// /boot がマウントされているディレクトリを得る

var boot_script_source = fs.getFileSystemAt('/boot/').source;

// boot_script_source/../tmp を /data にマウントする

fs.mount('/data', new fs.OSFS("\{boot_script_source}/../tmp", true));

// /tmp に TmpFS をマウントする
var tmpfs = new fs.TmpFS();
fs.mount('/tmp', tmpfs);

// ファイルを作成して書き込んでみる
fs.open('/tmp/test.txt', fs.omWrite).print("Hello world!").dispose();

// そのファイルを読んでみる
var st = fs.open('/tmp/test.txt');
var data = st.read();
st.dispose();
assert(data == (octet)"Hello world!");

// ファイルの最後に追加してみる
fs.open('/tmp/test.txt', fs.omAppend) { |st|
	assert(st.position == 12);
	st.print("----");
}

// そのファイルを読んでみる
var st = fs.open('/tmp/test.txt');
var data = st.read();
st.dispose();
assert(data == (octet)"Hello world!----");

// ファイルをアップデートしてみる
fs.open('/tmp/test.txt', fs.omUpdate) { |st|
	st.position = 12;
	st.print("AB");
	st.seek(0, stream.soSet);
	st.print("'");
	st.seek(2, stream.soCur);
	st.print("i");
	st.seek(-2, stream.soEnd);
	st.print("CD");
}

// そのファイルを読んでみる
var st = fs.open('/tmp/test.txt');
var data = st.read();
st.dispose();
assert(data == (octet)"'elio world!ABCD");

// 大文字と小文字が違うファイルを開こうとしてみる
var raised = false;
try {
	st = fs.open('/tmp/Test.txt');
} catch(e) {
	assert(e.class === IOException);
	raised = true;
}
assert(raised);

// サブディレクトリを作る
fs.createDirectory('/tmp/subdir');

// ディレクトリは作られたか？
assert(fs.isDirectory('/tmp/subdir'));
assert(fs.isDirectory('/tmp/subdir/'));
assert(fs.isDirectory('/tmp/subdir//')); // 一応legal
assert(fs.isDirectory('/tmp//subdir//')); // 一応legal
assert(!fs.isFile('/tmp/subdir'));

// そこにファイルを作ってみる
fs.open('/tmp/subdir/test2.txt', fs.omWrite).print("Hello world?\n").dispose();

// ファイルは作られたか？
assert(fs.isFile('/tmp/subdir/test2.txt'));
assert(!fs.isDirectory('/tmp/subdir/test2.txt'));
assert(fs.isFile('/tmp/subdir//test2.txt')); // 一応 legal

// もう一個ファイルを作ってみる(今度はブロック付きで)
fs.open('/tmp/subdir/test3.txt', fs.omWrite) { |st| st.print("Hello world!?\n") };



// サブディレクトリを作る
fs.createDirectory('/tmp/subdir/subfolder');

// ディレクトリは作られたか？
assert(fs.isDirectory('/tmp/subdir/subfolder'));
assert(fs.isDirectory('/tmp/subdir//subfolder'));

// もう一個ファイルを作ってみる
fs.open('/tmp/subdir/subfolder/test4.txt', fs.omWrite) { |st| st.print("Hello world!!?\n") };

// もう一個サブディレクトリを作る
fs.createDirectory('/tmp/subdir/subfolder/subtree');

// もう一個ファイルを作ってみる
fs.open('/tmp/subdir/subfolder/subtree/test5.txt', fs.omWrite) { |st| st.print("Hello world!!!?-\n") };


// トラバースしてみる
var file_expected = 3;
var count = fs.walkAt('/tmp/subdir') { |name, is_dir|
	if     (name == 'test2.txt' && !is_dir) file_expected--;
	else if(name == 'test3.txt' && !is_dir) file_expected--;
	else if(name == 'subfolder' &&  is_dir) file_expected--;
	else file_expected = -1;
}
assert(file_expected == 0);
assert(count == 3);

// ファイルシステムを保存してみる
tmpfs.save('/data/content.arc');

// 保存したファイルシステムを読み込んでみる
tmpfs.load('/data/content.arc');

// 再帰的にトラバースしてみる
var file_expected = 6;
var count = fs.walkAt('/tmp/subdir', true) { |name, is_dir|
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
fs.removeFile('/tmp/subdir/test2.txt');
fs.removeFile('/tmp/subdir/subfolder/subtree/test5.txt');
assert(!fs.isFile('/tmp/subdir/test2.txt'));
assert(!fs.isFile('/tmp/subdir/subfolder/subtree/test5.txt'));

// ディレクトリの削除
fs.removeDirectory('/tmp/subdir/subfolder/subtree');
assert(!fs.isDirectory('/tmp/subdir/subfolder/subtree'));

// 空でないディレクトリを削除しようとしてみる
var raised = false;
try {
	fs.removeDirectory('/tmp/subdir/subfolder/subtree');
} catch(e) {
	assert(e.class === IOException);
	raised = true;
}
assert(raised);

// ファイルの削除とディレクトリの削除
fs.removeFile('/tmp/subdir/subfolder/test4.txt');
fs.removeDirectory('/tmp/subdir/subfolder/');
assert(!fs.isDirectory('/tmp/subdir/subfolder/'));

// クリーンナップ
fs.removeDirectory('/tmp/subdir', true); // 再帰削除
assert(!fs.isFile('/tmp/subdir/test3.txt')); // 削除されているはず

stdio.stdout.print("ok"); //=> ok
