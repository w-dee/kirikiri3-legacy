// /boot ���}�E���g����Ă���f�B���N�g���𓾂�

var boot_script_origin = File::getFileSystemAt('/boot/').origin;

// boot_script_origin/../tmp �� /tmp �Ƀ}�E���g����

File::createDirectory('/tmp');
File::mount('/tmp', new FileSystem::OSFS("\{boot_script_origin}/../tmp", true));


//=> ok
