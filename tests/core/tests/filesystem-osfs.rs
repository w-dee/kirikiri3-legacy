// /boot ���}�E���g����Ă���f�B���N�g���𓾂�

var boot_script_source = File::getFileSystemAt('/boot/').source;

// boot_script_source/../tmp �� /tmp �Ƀ}�E���g����

File::createDirectory('/tmp');
File::mount('/tmp', new FileSystem::OSFS("\{boot_script_source}/../tmp", true));


//=> ok
