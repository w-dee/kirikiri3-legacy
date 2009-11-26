CORE_DIR=../..
include $(CORE_DIR)/Makefile_core_common

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=risa

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# リンカに渡すフラグ
LDFLAGS = $(GUI_LDFLAGS)

default: $(BIN)

#----------------------------#

CPPFILES=Main.cpp

OBJS =  $(CPPFILES:.cpp=.o) $(addsuffix /subsys.o,$(SUBSYS_DIRS))

POST_LINK_COMMAND=mkdir -p $(BUILD_OUT_DIR)/var;mkdir -p $(BUILD_OUT_DIR)/etc




#----------------------------#

include $(TOP_DIR)/Makefile_common

