TOP_DIR=../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=fstest

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST ZLIB COMMON

# リンカに渡すフラグ
LDFLAGS = -g $(CONSOLE_LDFLAGS)

#----------------------------#

# risse を使うための設定
include $(TOP_DIR)/src/core/risse/Makefile_for_user

#----------------------------#

CORE_DIR=$(TOP_DIR)/src/core
BASE_DIR=$(CORE_DIR)/base

INCLUDES= \
		-I$(BASE_DIR)/fs/common  \
		-I$(BASE_DIR)/fs/osfs    \
		-I$(BASE_DIR)/fs/pathfs  \
		-I$(BASE_DIR)/fs/tmpfs   \
		-I$(BASE_DIR)/fs/xp4fs   \
		-I$(BASE_DIR)/exception  \
		-I$(BASE_DIR)/script     \
		-I$(BASE_DIR)/script     \
		-I$(CORE_DIR)/utils      \
		-I.

CPPFILES = \
		$(BASE_DIR)/fs/common/FSManager.cpp         \
		$(BASE_DIR)/fs/common/FSManagerBind.cpp     \
		$(BASE_DIR)/fs/osfs/OSFS.cpp                \
		$(BASE_DIR)/fs/osfs/OSFSBind.cpp            \
		$(BASE_DIR)/fs/pathfs/PathFS.cpp            \
		$(BASE_DIR)/fs/pathfs/PathFSBind.cpp        \
		$(BASE_DIR)/fs/tmpfs/MemoryStream.cpp       \
		$(BASE_DIR)/fs/tmpfs/TmpFS.cpp              \
		$(BASE_DIR)/fs/tmpfs/TmpFSBind.cpp          \
		$(BASE_DIR)/fs/xp4fs/DecompressedHolder.cpp \
		$(BASE_DIR)/fs/xp4fs/XP4Archive.cpp         \
		$(BASE_DIR)/fs/xp4fs/XP4FS.cpp              \
		$(BASE_DIR)/fs/xp4fs/XP4SegmentCache.cpp    \
		$(BASE_DIR)/fs/xp4fs/XP4Stream.cpp          \
		$(BASE_DIR)/fs/xp4fs/XP4StreamCache.cpp     \
		$(BASE_DIR)/fs/xp4fs/XP4FSBind.cpp          \
		$(BASE_DIR)/exception/TVPException.cpp      \
		$(BASE_DIR)/script/RisseEngine.cpp            \
		main.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

