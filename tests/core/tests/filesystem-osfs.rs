// /boot がマウントされているディレクトリを得る

var boot_script_source = File::getFileSystemAt('/boot/').source;

// boot_script_source/../tmp を /tmp にマウントする

File::createDirectory('/tmp');
File::mount('/tmp', new FileSystem::OSFS("\{boot_script_source}/../tmp", true));


//=> ok
