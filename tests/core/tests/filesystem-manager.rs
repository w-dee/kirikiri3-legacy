import risa.fs as fs;
import risa.fs.tmpfs as tmpfs;
import risa.stdio as stdio;

// ファイル名操作などいろいろ
assert(fs.chopExtension('/a.txt') == '/a');
assert(fs.chopExtension('/b/a.txt') == '/b/a');
assert(fs.chopExtension('/b.c/a.txt') == '/b.c/a');
assert(fs.chopExtension('/a.') == '/a');
assert(fs.chopExtension('a.txt') == 'a');
assert(fs.chopExtension('/a') == '/a');
assert(fs.chopExtension('/.a') == '/');

assert(fs.extractExtension('/a.b') == '.b');
assert(fs.extractExtension('/a.') == '.');
assert(fs.extractExtension('/a') == '');
assert(fs.extractExtension('a.b') == '.b');
assert(fs.extractExtension('/c.d/a.b') == '.b');
assert(fs.extractExtension('/c.d/.b') == '.b');

assert(fs.extractName('/a.b') == 'a.b');
assert(fs.extractName('/a.') == 'a.');
assert(fs.extractName('/a') == 'a');
assert(fs.extractName('a.b') == 'a.b');
assert(fs.extractName('/c.d/a.b') == 'a.b');
assert(fs.extractName('/c.d/.b') == '.b');

assert(fs.extractPath('/a.b') == '/');
assert(fs.extractPath('/a.') == '/');
assert(fs.extractPath('/a') == '/');
assert(fs.extractPath('a.b') == '');
assert(fs.extractPath('/c.d/a.b') == '/c.d/');
assert(fs.extractPath('/c.d/.b') == '/c.d/');


fs.cwd = '/';
fs.cwd = 'test'; // これで cwd は /test/ のはず
assert(fs.cwd == '/test/');

assert(fs.normalize('a') == '/test/a');
assert(fs.normalize('/../a') == '/a');
assert(fs.normalize('/./a') == '/a');
assert(fs.normalize('../a') == '/a');
assert(fs.normalize('a/b') == '/test/a/b');
assert(fs.normalize('/a/../b') == '/b');
assert(fs.normalize('/a/../../b') == '/b');
assert(fs.normalize('/a/c/../.././b') == '/b');
assert(fs.normalize('/a/./b') == '/a/b');
assert(fs.normalize('/a/../') == '/');
assert(fs.normalize('/a//b') == '/a/b');
assert(fs.normalize('/a//b//') == '/a/b/');

fs.cwd = '/tmp';

// 再帰的にディレクトリを作ってみる
fs.createDirectory('mp', true);

// マウント
assert(!fs.isDirectory('mp/tmpfs'));
fs.mount('mp/tmpfs', new tmpfs.TmpFS());

// マウントポイントにはディレクトリが作られているはず
assert(fs.isDirectory('mp/tmpfs'));

// ディレクトリ作成
fs.createDirectory('mp/tmpfs/dir');
assert(fs.isDirectory('mp/tmpfs/dir'));

// マウントポイントを含むディレクトリを再帰的に削除しようとしてみる
var raised = false;
try {
	fs.removeDirectory('mp', true);
} catch(e) {
	assert(e.class === fs.FileSystemException);
	raised = true;
}
assert(raised);

// マウントポイントを削除しようとしてみる
var raised = false;
try {
	fs.removeDirectory('mp/tmpfs');
} catch(e) {
	assert(e.class === fs.FileSystemException);
	raised = true;
}
assert(raised);

var raised = false;
try {
	fs.removeDirectory('mp/tmpfs/');
} catch(e) {
	assert(e.class === fs.FileSystemException);
	raised = true;
}
assert(raised);

var raised = false;
try {
	fs.removeDirectory('mp/tmpf'); // tmpfs じゃない
} catch(e) {
	assert(e.class === IOException); // この場合は IOException
	raised = true;
}
assert(raised);

// ファイルをいくつか作成
fs.open('mp/file1', fs.omWrite) {}
fs.open('mp/tmpfs/file2', fs.omWrite) {}
fs.open('mp/tmpfs/dir/file3', fs.omWrite) {}

// ファイルシステムをまたがった walk
var count = 6;
fs.walkAt('/tmp', true) { |name, is_dir|
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
stdio.stdout.print("ok"); //=> ok
