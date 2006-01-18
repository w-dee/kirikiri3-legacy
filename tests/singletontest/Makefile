TOP_DIR=../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=singletontest

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST COMMON

# リンカに渡すフラグ
LDFLAGS = -g $(CONSOLE_LDFLAGS)

#----------------------------#

CORE_DIR=$(TOP_DIR)/src/core

INCLUDES= \
		-I$(CORE_DIR)/utils  \
		-I.

CPPFILES = \
		$(CORE_DIR)/utils/Singleton.cpp  \
		main.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

