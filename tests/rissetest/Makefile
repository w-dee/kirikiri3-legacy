TOP_DIR=../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=rissetest

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST WX GC COMMON

# リンカに渡すフラグ
LDFLAGS += -g $(CONSOLE_LDFLAGS)

#----------------------------#

# risse を使うための設定
include $(TOP_DIR)/src/core/risse/Makefile_for_user

#----------------------------#

CORE_DIR=$(TOP_DIR)/src/core

INCLUDES= \
		-I../../src/core/risse/src       \
		-I.

CPPFILES = \
		main.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

