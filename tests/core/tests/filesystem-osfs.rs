// /boot がマウントされているディレクトリを得る

var boot_script_origin = File::getFileSystemAt('/boot/').origin;

// boot_script_origin/../tmp を /tmp にマウントする

File::createDirectory('/tmp');
File::mount('/tmp', new FileSystem::OSFS("\{boot_script_origin}/../tmp", true));


//=> ok
