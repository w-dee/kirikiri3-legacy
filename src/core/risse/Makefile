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

CPPFILES =  src/risseCharUtils.cpp                   \
			src/risseConfig.cpp                      \
			src/risseCxxObject.cpp                   \
			src/risseCxxOctet.cpp                    \
			src/risseCxxString.cpp                   \
			src/risseException.cpp                   \
			src/risseLexerUtils.cpp                  \
			src/risseScriptBlockBase.cpp             \
			src/risseUtils.cpp                       \
			src/risseVariant.cpp                     \
			src/risse_parser/risseLexer.cpp          \
			src/risse_parser/risseScriptBlock.cpp    \
			src/risse_parser/risseParser.cpp         

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = src/prec.h

INCLUDES = -I.

# risseLexerMap.inc のコンパイル

src/risse_parser/risseLexerMap.inc: \
		src/risse_parser/words.txt \
		tools/create_word_map.rb Makefile
	ruby tools/create_word_map.rb src/risse_parser/words.txt RisseMapToken > \
		src/risse_parser/risseLexerMap.inc

# risseParser.cpp と risseParser.h のコンパイル

src/risse_parser/risseParser.cpp src/risse_parser/risseParser.h: \
		src/risse_parser/risse.y Makefile
	(set -e ;\
		cd src/risse_parser ;\
		bison -dv -p risse_ --name-prefix=risse risse.y ;\
		cat risse.tab.c | \
			sed -e "s/risse.tab.h/risseParser.h/" -e "s/risse.tab.c/risseParser.cpp/" \
				>  risseParser.cpp ;\
		echo "namespace Risse {" > risseParser.h ;\
		cat risse.tab.h | \
			sed -e "s/risse.tab.h/risseParser.h/" -e "s/risse.tab.c/risseParser.cpp/" \
				>  risseParser.h ;\
		echo "}" >> risseParser.h ;\
		rm risse.tab.c risse.tab.h ;\
	)

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
