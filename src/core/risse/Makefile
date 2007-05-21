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

document:
	rm -rf doc
	doxygen > doxygen.log 2>&1
	find doc -name '*.html' -print | \
		while read n; do \
			mv $$n $$n.bak && \
			cat $$n.bak | \
			sed 's/content="text\/html;charset=iso-8859-1\"/content="text\/html;charset=utf-8\"/' >\
				$$n && \
			rm -f $$n.bak ; \
		done

#----------------------------#

CXXFLAGS += \
	-DRISSE_NO_REGEXP \
	-DRISSE_SUPPORT_WX

CPPFLAGS = $(CXXFLAGS)

#----------------------------#

CPPFILES =  \
			src/risseArrayClass.cpp                  \
			src/risseAssert.cpp                      \
			src/risseBindingClass.cpp                \
			src/risseCharUtils.cpp                   \
			src/risseClass.cpp                       \
			src/risseClassClass.cpp                  \
			src/risseCodeBlock.cpp                   \
			src/risseCodeExecutor.cpp                \
			src/risseConfig.cpp                      \
			src/risseCoroutine.cpp                   \
			src/risseCoroutineClass.cpp              \
			src/risseExceptionClass.cpp              \
			src/risseFunctionClass.cpp               \
			src/risseIntegerClass.cpp                \
			src/risseLexerUtils.cpp                  \
			src/risseMemberAttribute.cpp             \
			src/risseMethod.cpp                      \
			src/risseModuleClass.cpp                 \
			src/risseNativeFunction.cpp              \
			src/risseNativeProperty.cpp              \
			src/risseNumberClass.cpp                 \
			src/risseObject.cpp                      \
			src/risseObjectBase.cpp                  \
			src/risseObjectClass.cpp                 \
			src/risseOctet.cpp                       \
			src/risseOpCodes.cpp                     \
			src/risseOperateFlags.cpp                \
			src/risseOperateRetValue.cpp             \
			src/rissePrimitiveClass.cpp              \
			src/rissePropertyClass.cpp               \
			src/risseRealClass.cpp                   \
			src/risseScriptBlockBase.cpp             \
			src/risseScriptEngine.cpp                \
			src/risseStaticStrings.cpp               \
			src/risseString.cpp                      \
			src/risseStringClass.cpp                 \
			src/risseTypes.cpp                       \
			src/risseUtils.cpp                       \
			src/risseVariant.cpp                     \
			src/risse_parser/risseLexer.cpp          \
			src/risse_parser/risseScriptBlock.cpp    \
			src/risse_parser/risseParser.cpp         \
			src/compiler/risseAST.cpp                \
			src/compiler/risseCodeGen.cpp            \
			src/compiler/risseCompiler.cpp           \
			src/compiler/risseCompilerNS.cpp         \
			src/compiler/risseDeclAttribute.cpp      \
			src/compiler/risseSSABlock.cpp           \
			src/compiler/risseSSAForm.cpp            \
			src/compiler/risseSSAStatement.cpp       \
			src/compiler/risseSSAVariable.cpp        

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = src/prec.h

INCLUDES = -I. -Isrc/boost-coroutine

# risseLexerMap.inc のコンパイル
BIN_DEPS += src/risse_parser/risseLexerMap.inc

src/risse_parser/risseLexerMap.inc: \
		src/risse_parser/words.txt \
		tools/create_word_map.rb Makefile
	ruby tools/create_word_map.rb src/risse_parser/words.txt RisseMapToken > \
		src/risse_parser/risseLexerMap.inc

# risseOpCodesEnum.inc と risseOpCodesDefs.inc のコンパイル
BIN_DEPS += src/risseOpCodesEnum.inc src/risseOpCodesDefs.inc

src/risseOpCodesEnum.inc src/risseOpCodesDefs.inc: \
		src/risseOpCodes.txt \
		tools/opcodes.rb Makefile
	ruby tools/opcodes.rb src/risseOpCodes.txt \
		src/risseOpCodesEnum.inc src/risseOpCodesDefs.inc

# risseStaticStringsData.inc と risseStaticStringsIds.inc のコンパイル
BIN_DEPS += src/risseStaticStringsData.inc src/risseStaticStringsIds.inc

src/risseStaticStringsData.inc src/risseStaticStringsIds.inc: \
		src/risseStaticString.txt \
		src/risse_parser/words.txt \
		tools/static_strings.rb Makefile
	ruby tools/static_strings.rb src/risseStaticString.txt \
		src/risse_parser/words.txt \
		src/risseStaticStringsIds.inc src/risseStaticStringsData.inc

# risseParser.cpp と risseParser.inc のコンパイル
BIN_DEPS += src/risse_parser/risseParser.inc

src/risse_parser/risseParser.cpp src/risse_parser/risseParser.inc: \
		src/risse_parser/risse.y Makefile
	@echo Compiling src/risse_parser/risse.y
	@(set -e ;\
		cd src/risse_parser ;\
		bison -v --name-prefix=risse risse.y ;\
		cat risse.tab.c | \
			sed -e "s|risse.tab.h|src/risse_parser/risseParser.inc|" \
				-e "s|risse.tab.c|src/risse_parser/risseParser.cpp|" \
				-e "s|risse\.y|src/risse_parser/risse.y|" \
				>  risseParser.cpp ;\
		ruby ../../tools/extract_yytokentype.rb risse.tab.c > risseParser.inc; \
		rm risse.tab.c ;\
		(grep -i "As a special exception" risseParser.cpp > /dev/null) || (\
			echo "**********************************************************"; \
			echo "The bison output file seems not to contain a term which"; \
			echo "permits the parser is to be used in non-GPL software."; \
			echo "This isn't a problem when you use Risse script engine"; \
			echo "in GPLed software, otherwise you should update your"; \
			echo "bison 1.24 or above (which permits use in non-GPL software). "; \
			echo "**********************************************************"; ) \
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
