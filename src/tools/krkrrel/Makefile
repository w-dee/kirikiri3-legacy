TOP_DIR=../../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=krkrrel

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=WX ZLIB LIBTOMCRYPT COMMON GC

# リンカに渡すフラグ
LDFLAGS = -g $(CONSOLE_LDFLAGS)

#----------------------------#

INCLUDES =

CPPFILES =  ConsoleMain.cpp \
			FileList.cpp  \
			WriteXP4.cpp \
			ReadXP4Meta.cpp \
			wxFileWrapper.cpp \
			XP4Hash.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

