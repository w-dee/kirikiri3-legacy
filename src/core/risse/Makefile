# TODO: 任意のprefixのディレクトリに install 出来るように

TOP_DIR=../../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=risse

# 出力バイナリ名
Risse_LIB_OUTPUT_DIR := $(CURRENT_DIR)/lib
Risse_BIN_OUTPUT_DIR := $(CURRENT_DIR)/bin
BIN=$(Risse_LIB_OUTPUT_DIR)/librisse.a

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST WX GC

# 最終ターゲット (ふつう、出力バイナリや出力ライブラリ)
default: $(BIN)

#----------------------------#

CXXFLAGS += \
	-DRISSE_NO_REGEXP \
	-DRISSE_SUPPORT_WX

CPPFLAGS = $(CXXFLAGS)

#----------------------------#

CPPFILES =  src/risseCharUtils.cpp            \
			src/risseConfig.cpp               \
			src/risseCxxObject.cpp            \
			src/risseCxxOctet.cpp             \
			src/risseCxxString.cpp            \
			src/risseLexerUtils.cpp           \
			src/risseUtils.cpp                \
			src/risseVariant.cpp              

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = src/prec.h

INCLUDES = -Iinclude

#----------------------------#

# 設定ファイルを書き出す

Risse_INCLUDE_DIR := $(shell pwd)/include

# risse-config を作成する。
# boost ライブラリを使っているかどうかは、依存関係ファイルの中に
# 特定の文字列があるかどうかで判断する。

$(Risse_BIN_OUTPUT_DIR)/risse-config: Makefile risse-config.in src/risseUtils.d
	mkdir -p $(Risse_BIN_OUTPUT_DIR)
	(set -e ;\
		sed risse-config.in \
			-e "s|@LIBS@|\"-L$(Risse_LIB_OUTPUT_DIR) -lrisse\"|" \
			-e "s|@CXXFLAGS@|\"-I$(Risse_INCLUDE_DIR) $(CXXFLAGS)\"|" \
			-e "s|@CPPFLAGS@|\"-I$(Risse_INCLUDE_DIR) $(CXXFLAGS)\"|" \
			-e "s|@LINKDEPS@|\"$(BIN)\"|" \
			  > $(Risse_BIN_OUTPUT_DIR)/risse-config \
	)
	chmod +x $(Risse_BIN_OUTPUT_DIR)/risse-config

BIN_DEPS = $(Risse_BIN_OUTPUT_DIR)/risse-config
#----------------------------#
include $(TOP_DIR)/Makefile_common
