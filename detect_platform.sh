#!/bin/sh -e

# コマンドライン引数に従って現在のプラットフォームに関する
# 情報を標準出力に出力する.
# uname のラッパー。

case $1 in
-f | --family)
	# windows, unix など
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
	# win32, linux, solaris mac など
	case `uname -s` in
	CYGWIN* | MINGW*)
		# win64どうしよう
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

