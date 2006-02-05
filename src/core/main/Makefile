CORE_DIR=..
include $(CORE_DIR)/Makefile_core_common

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=k3_core

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# リンカに渡すフラグ
LDFLAGS = $(GUI_LDFLAGS)

default: $(BIN)

#----------------------------#

CPPFILES=Main.cpp

OBJS =  $(CPPFILES:.cpp=.o) $(addsuffix /subsys.a,$(SUBSYS_DIRS))

#----------------------------#

include $(TOP_DIR)/Makefile_common

