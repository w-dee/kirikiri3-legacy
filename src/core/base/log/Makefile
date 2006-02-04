#-----------------------------------------
# 各トップディレクトリを指すようにここは修正
#-----------------------------------------
CORE_DIR=../..
TOP_DIR=../../../..

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(TOP_DIR)/Makefile_filenames
include $(CORE_DIR)/Makefile_core_common
PROGRAM_ID=subsys
BIN=subsys.a
PREC_H_NAME = $(CORE_DIR)/prec.h
default: $(BIN)

#-----------------------------------------
# このディレクトリにあるソースファイルを列挙
#-----------------------------------------
CPPFILES = \
		common/FSManager.cpp                     \
		common/FSManagerBind.cpp                 \
		osfs/OSFS.cpp                            \
		osfs/OSFSBind.cpp                        \
		pathfs/PathFS.cpp                        \
		pathfs/PathFSBind.cpp                    \
		tmpfs/MemoryStream.cpp                   \
		tmpfs/TmpFS.cpp                          \
		tmpfs/TmpFSBind.cpp                      \
		xp4fs/DecompressedHolder.cpp             \
		xp4fs/XP4Archive.cpp                     \
		xp4fs/XP4FS.cpp                          \
		xp4fs/XP4SegmentCache.cpp                \
		xp4fs/XP4Stream.cpp                      \
		xp4fs/XP4StreamCache.cpp                 \
		xp4fs/XP4FSBind.cpp                      


#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
OBJS = $(CPPFILES:.cpp=.o)
include $(TOP_DIR)/Makefile_common

