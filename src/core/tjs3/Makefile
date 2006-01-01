# TODO: 任意のprefixのディレクトリに install 出来るように

TOP_DIR=../../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=tjs3

# 出力バイナリ名
TJS3_LIB_OUTPUT_DIR := $(shell pwd)/lib
TJS3_BIN_OUTPUT_DIR := $(shell pwd)/bin
BIN=$(TJS3_LIB_OUTPUT_DIR)/libtjs3.a

# このバイナリが依存しているライブラリ
DEP_LIBS=WX

# 最終ターゲット (ふつう、出力バイナリや出力ライブラリ)
default: $(BIN)

#----------------------------#

CXXFLAGS = \
	-DTJS_NO_REGEXP \
	-DTJS_SUPPORT_WX

CPPFLAGS = $(CXXFLAGS)

#----------------------------#

CPPFILES = 	src/tjs.cpp                     \
			src/tjs.tab.cpp                 \
			src/tjsArray.cpp                \
			src/tjsCharUtils.cpp            \
			src/tjsCompileControl.cpp       \
			src/tjsConfig.cpp               \
			src/tjsDate.cpp                 \
			src/tjsDateParser.cpp           \
			src/tjsDebug.cpp                \
			src/tjsDictionary.cpp           \
			src/tjsDisassemble.cpp          \
			src/tjsError.cpp                \
			src/tjsException.cpp            \
			src/tjsGlobalStringMap.cpp      \
			src/tjsInterCodeExec.cpp        \
			src/tjsInterCodeGen.cpp         \
			src/tjsInterface.cpp            \
			src/tjsLex.cpp                  \
			src/tjsMT19937ar-cok.cpp        \
			src/tjsMath.cpp                 \
			src/tjsMessage.cpp              \
			src/tjsNamespace.cpp            \
			src/tjsNative.cpp               \
			src/tjsObject.cpp               \
			src/tjsRandomGenerator.cpp      \
			src/tjsScriptBlock.cpp          \
			src/tjsScriptCache.cpp          \
			src/tjsString.cpp               \
			src/tjsUtils.cpp                \
			src/tjsVariant.cpp              \
			src/tjsVariantString.cpp        \
			src/tjsdate.tab.cpp             \
			src/tjspp.tab.cpp               

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = include/tjsCommHead.h

INCLUDES = -Iinclude

#----------------------------#

# 設定ファイルを書き出す

TJS3_INCLUDE_DIR := $(shell pwd)/include

$(TJS3_BIN_OUTPUT_DIR)/tjs3-config: Makefile
	mkdir -p $(TJS3_BIN_OUTPUT_DIR)
	sed tjs3-config.in \
		-e 's|@LIBS@|"-L$(TJS3_LIB_OUTPUT_DIR) -ltjs3"|' \
		-e 's|@CXXFLAGS@|"-I$(TJS3_INCLUDE_DIR) $(CXXFLAGS)"|' \
		-e 's|@CPPFLAGS@|"-I$(TJS3_INCLUDE_DIR) $(CXXFLAGS)"|' \
		  > $(TJS3_BIN_OUTPUT_DIR)/tjs3-config
	chmod +x $(TJS3_BIN_OUTPUT_DIR)/tjs3-config

BIN_DEPS = $(TJS3_BIN_OUTPUT_DIR)/tjs3-config
#----------------------------#
include $(TOP_DIR)/Makefile_common
