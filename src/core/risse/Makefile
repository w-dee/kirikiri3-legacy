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
DEP_LIBS=BOOST WX

# 最終ターゲット (ふつう、出力バイナリや出力ライブラリ)
default: $(BIN)

#----------------------------#

CXXFLAGS = \
	-DRISSE_NO_REGEXP \
	-DRISSE_SUPPORT_WX

CPPFLAGS = $(CXXFLAGS)

#----------------------------#

CPPFILES = 	src/risse.cpp                     \
			src/risse.tab.cpp                 \
			src/risseArray.cpp                \
			src/risseCharUtils.cpp            \
			src/risseCompileControl.cpp       \
			src/risseConfig.cpp               \
			src/risseDate.cpp                 \
			src/risseDateParser.cpp           \
			src/risseDebug.cpp                \
			src/risseDictionary.cpp           \
			src/risseDisassemble.cpp          \
			src/risseError.cpp                \
			src/risseException.cpp            \
			src/risseGlobalStringMap.cpp      \
			src/risseInterCodeExec.cpp        \
			src/risseInterCodeGen.cpp         \
			src/risseInterface.cpp            \
			src/risseLex.cpp                  \
			src/risseMT19937ar-cok.cpp        \
			src/risseMath.cpp                 \
			src/risseMessage.cpp              \
			src/risseNamespace.cpp            \
			src/risseNative.cpp               \
			src/risseObject.cpp               \
			src/risseRandomGenerator.cpp      \
			src/risseScriptBlock.cpp          \
			src/risseScriptCache.cpp          \
			src/risseString.cpp               \
			src/risseUtils.cpp                \
			src/risseVariant.cpp              \
			src/risseVariantString.cpp        \
			src/rissedate.tab.cpp             \
			src/rissepp.tab.cpp               

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = include/risseCommHead.h

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
		threads=""; \
		if [ `grep -c "boost/thread.hpp" src/risseUtils.d` != 0 ]; then threads="$(BOOST_LIBS) -lboost_thread-gcc-mt"; fi; \
		sed risse-config.in \
			-e "s|@LIBS@|\"-L$(Risse_LIB_OUTPUT_DIR) -lrisse $$threads\"|" \
			-e "s|@CXXFLAGS@|\"-I$(Risse_INCLUDE_DIR) $(CXXFLAGS)\"|" \
			-e "s|@CPPFLAGS@|\"-I$(Risse_INCLUDE_DIR) $(CXXFLAGS)\"|" \
			-e "s|@LINKDEPS@|\"$(BIN)\"|" \
			  > $(Risse_BIN_OUTPUT_DIR)/risse-config \
	)
	chmod +x $(Risse_BIN_OUTPUT_DIR)/risse-config

BIN_DEPS = $(Risse_BIN_OUTPUT_DIR)/risse-config
#----------------------------#
include $(TOP_DIR)/Makefile_common
