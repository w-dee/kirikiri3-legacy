// ファイル名操作などいろいろ
assert(File::chopExtension('/a.txt') == '/a');
assert(File::chopExtension('/b/a.txt') == '/b/a');
assert(File::chopExtension('/b.c/a.txt') == '/b.c/a');
assert(File::chopExtension('/a.') == '/a');
assert(File::chopExtension('a.txt') == 'a');
assert(File::chopExtension('/a') == '/a');
assert(File::chopExtension('/.a') == '/');

assert(File::extractExtension('/a.b') == '.b');
assert(File::extractExtension('/a.') == '.');
assert(File::extractExtension('/a') == '');
assert(File::extractExtension('a.b') == '.b');
assert(File::extractExtension('/c.d/a.b') == '.b');
assert(File::extractExtension('/c.d/.b') == '.b');

assert(File::extractName('/a.b') == 'a.b');
assert(File::extractName('/a.') == 'a.');
assert(File::extractName('/a') == 'a');
assert(File::extractName('a.b') == 'a.b');
assert(File::extractName('/c.d/a.b') == 'a.b');
assert(File::extractName('/c.d/.b') == '.b');

assert(File::extractPath('/a.b') == '/');
assert(File::extractPath('/a.') == '/');
assert(File::extractPath('/a') == '/');
assert(File::extractPath('a.b') == '');
assert(File::extractPath('/c.d/a.b') == '/c.d/');
assert(File::extractPath('/c.d/.b') == '/c.d/');


File::cwd = '/';
File::cwd = 'test'; // これで cwd は /test/ のはず
assert(File::cwd == '/test/');

assert(File::normalize('a') == '/test/a');
assert(File::normalize('/../a') == '/a');
assert(File::normalize('/./a') == '/a');
assert(File::normalize('../a') == '/a');
assert(File::normalize('a/b') == '/test/a/b');
assert(File::normalize('/a/../b') == '/b');
assert(File::normalize('/a/../../b') == '/b');
assert(File::normalize('/a/c/../.././b') == '/b');
assert(File::normalize('/a/./b') == '/a/b');
assert(File::normalize('/a/../') == '/');
assert(File::normalize('/a//b') == '/a/b');
assert(File::normalize('/a//b//') == '/a/b/');

File::cwd = '/tmp';

// 再帰的にディレクトリを作ってみる
File::createDirectory('mp', true);

// マウント
assert(!File::isDirectory('mp/tmpfs'));
File::mount('mp/tmpfs', new FileSystem::TmpFS());

// マウントポイントにはディレクトリが作られているはず
assert(File::isDirectory('mp/tmpfs'));

// ディレクトリ作成
File::createDirectory('mp/tmpfs/dir');
assert(File::isDirectory('mp/tmpfs/dir'));

// マウントポイントを含むディレクトリを再帰的に削除しようとしてみる
var raised = false;
try {
	File::removeDirectory('mp', true);
} catch(e) {
	assert(e.class === FileSystemException);
	raised = true;
}
assert(raised);

// マウントポイントを削除しようとしてみる
var raised = false;
try {
	File::removeDirectory('mp/tmpfs');
} catch(e) {
	assert(e.class === FileSystemException);
	raised = true;
}
assert(raised);

var raised = false;
try {
	File::removeDirectory('mp/tmpfs/');
} catch(e) {
	assert(e.class === FileSystemException);
	raised = true;
}
assert(raised);

var raised = false;
try {
	File::removeDirectory('mp/tmpf'); // tmpfs じゃない
} catch(e) {
	assert(e.class === IOException); // この場合は IOException
	raised = true;
}
assert(raised);

// ファイルをいくつか作成
File::open('mp/file1', File::omWrite) {}
File::open('mp/tmpfs/file2', File::omWrite) {}
File::open('mp/tmpfs/dir/file3', File::omWrite) {}

// ファイルシステムをまたがった walk
var count = 6;
File::walkAt('/tmp', true) { |name, is_dir|
	var n = "\{name}|\{is_dir}";
	switch(n) {
	case 'mp|true':
	case 'mp/tmpfs|true':
	case 'mp/file1|false':
	case 'mp/tmpfs/dir|true':
	case 'mp/tmpfs/file2|false':
	case 'mp/tmpfs/dir/file3|false':
		count --;
		break;
	default:
		count = -1;
	}
}
assert(count == 0);


// ok を表示
System::stdout.print("ok"); //=> ok
