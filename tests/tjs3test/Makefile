TOP_DIR=../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=tjs3test

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=

# リンカに渡すフラグ
LDFLAGS = -g $(CONSOLE_LDFLAGS)

#----------------------------#

# tjs3 を使うための設定
include $(TOP_DIR)/src/core/tjs3/Makefile_for_user

#----------------------------#

CPPFILES = main.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

