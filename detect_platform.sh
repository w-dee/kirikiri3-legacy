#!/bin/sh -e

# �R�}���h���C�������ɏ]���Č��݂̃v���b�g�t�H�[���Ɋւ���
# ����W���o�͂ɏo�͂���.
# uname �̃��b�p�[�B

case $1 in
-f | --family)
	# windows, unix �Ȃ�
	case `uname -s` in
	CYGWIN* | MINGW*)
		echo windows ;;
	Linux )
		echo unix ;;
	SunOS )
		echo unix ;;
	Darwin )
		echo unix ;;
	esac;;
-p | --platform)
	# win32, linux, solaris mac �Ȃ�
	case `uname -s` in
	CYGWIN* | MINGW*)
		# win64�ǂ����悤
		echo win32 ;;
	Linux )
		echo linux ;;
	SunOS )
		echo solaris ;;
	Darwin )
		echo mac ;;
	esac;;
*)
	echo "$0 [-fp]" ;;
esac

