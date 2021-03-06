# TODO: 任意のprefixのディレクトリに install 出来るように

TOP_DIR=../../..
include $(TOP_DIR)/Makefile_filenames


# この変数を定義すると、risse ディレクトリの下に include ディレクトリを作らない。
# risse-config の出力するインストールディレクトリは src ディレクトリそのままになる。
# include ディレクトリを作りたい場合はこの定義をコメントアウトすること。
RISSE_NO_COPY_INCLUDES = 1

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=risse

# 出力バイナリ名
Risse_LIB_OUTPUT_DIR := $(CURRENT_DIR)/lib
Risse_BIN_OUTPUT_DIR := $(CURRENT_DIR)/bin
BIN=$(Risse_LIB_OUTPUT_DIR)/librisse.a

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST WX GC HAMIGAKI

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
	-DRISSE_SUPPORT_WX \
	-DRISSE_SUPPORT_THREADS \
	-DBOOST_ENABLE_ASSERT_HANDLER


CPPFLAGS = $(CXXFLAGS)

INCLUDES =

#----------------------------#

CPPFILES =  \
			src/risseArrayClass.cpp                            \
			src/risseAssert.cpp                                \
			src/risseBindingClass.cpp                          \
			src/risseBooleanClass.cpp                          \
			src/risseCharUtils.cpp                             \
			src/risseClass.cpp                                 \
			src/risseClassClass.cpp                            \
			src/risseCodeBlock.cpp                             \
			src/risseCodeExecutor.cpp                          \
			src/risseConfig.cpp                                \
			src/risseDataClass.cpp                             \
			src/risseDictionaryClass.cpp                       \
			src/risseExceptionClass.cpp                        \
			src/risseFunctionClass.cpp                         \
			src/risseGC.cpp                                    \
			src/risseIntegerClass.cpp                          \
			src/risseLexerUtils.cpp                            \
			src/risseMemberAttribute.cpp                       \
			src/risseMethod.cpp                                \
			src/risseModule.cpp                                \
			src/risseModuleClass.cpp                           \
			src/risseNativeBinder.cpp                          \
			src/risseNullClass.cpp                             \
			src/risseNumberClass.cpp                           \
			src/risseObject.cpp                                \
			src/risseObjectBase.cpp                            \
			src/risseObjectClass.cpp                           \
			src/risseOctet.cpp                                 \
			src/risseOctetClass.cpp                            \
			src/risseOpCodes.cpp                               \
			src/risseOperateFlags.cpp                          \
			src/risseOperateRetValue.cpp                       \
			src/rissePackage.cpp                               \
			src/rissePrimitiveClass.cpp                        \
			src/rissePropertyClass.cpp                         \
			src/risseRealClass.cpp                             \
			src/risseScriptBlockClass.cpp                      \
			src/risseScriptEngine.cpp                          \
			src/risseStaticStrings.cpp                         \
			src/risseStream.cpp                                \
			src/risseString.cpp                                \
			src/risseStringClass.cpp                           \
			src/risseThread.cpp                                \
			src/risseTypes.cpp                                 \
			src/risseVariant.cpp                               \
			src/risseVoidClass.cpp                             \
			src/risseWCString.cpp                              \
			src/risse_parser/risseLexer.cpp                    \
			src/risse_parser/risseRisseScriptBlockClass.cpp    \
			src/risse_parser/risseParser.cpp                   \
			src/compiler/risseAST.cpp                          \
			src/compiler/risseCodeGen.cpp                      \
			src/compiler/risseCompiler.cpp                     \
			src/compiler/risseCompilerNS.cpp                   \
			src/compiler/risseDeclAttribute.cpp                \
			src/compiler/risseSSABlock.cpp                     \
			src/compiler/risseSSAForm.cpp                      \
			src/compiler/risseSSAStatement.cpp                 \
			src/compiler/risseSSAVariable.cpp                  \
			src/builtin/date/risseDateClass.cpp                \
			src/builtin/date/risseDateParser.cpp               \
			src/builtin/thread/risseThreadClass.cpp            \
			src/builtin/coroutine/risseCoroutine.cpp           \
			src/builtin/coroutine/risseCoroutineClass.cpp      \
			src/builtin/stream/risseStreamClass.cpp            

OBJS = $(CPPFILES:.cpp=.o)

PREC_H_NAME = src/prec.h

# risseLexerMap.def のコンパイル
BIN_DEPS += src/risse_parser/risseLexerMap.def

src/risse_parser/risseLexerMap.def: \
		src/risse_parser/words.txt \
		tools/create_word_map.rb Makefile
	ruby tools/create_word_map.rb src/risse_parser/words.txt MapToken tVariant ::Risse::iswordcha_nc > \
		src/risse_parser/risseLexerMap.def

# risseDateLexerMap.def のコンパイル
BIN_DEPS += src/builtin/date/risseDateLexerMap.def

src/builtin/date/risseDateLexerMap.def: \
		src/builtin/date/risseDateLexerWords.txt \
		tools/create_word_map.rb Makefile
	ruby tools/create_word_map.rb src/builtin/date/risseDateLexerWords.txt \
		MapToken long ::Risse::iswalpha_nc > \
		src/builtin/date/risseDateLexerMap.def

# risseOpCodesEnum.inc と risseOpCodesDefs.def のコンパイル
BIN_DEPS += src/risseOpCodesEnum.inc src/risseOpCodesDefs.def

src/risseOpCodesEnum.inc src/risseOpCodesDefs.def: \
		src/risseOpCodes.txt \
		tools/opcodes.rb Makefile
	ruby tools/opcodes.rb src/risseOpCodes.txt \
		src/risseOpCodesEnum.inc src/risseOpCodesDefs.def

# risseStaticStringsData.def と risseStaticStringsIds.inc のコンパイル
BIN_DEPS += src/risseStaticStringsData.def src/risseStaticStringsIds.inc

src/risseStaticStringsData.def src/risseStaticStringsIds.inc: \
		src/risseStaticString.txt \
		src/risse_parser/words.txt \
		tools/static_strings.rb Makefile
	ruby tools/static_strings.rb src/risseStaticString.txt \
		src/risse_parser/words.txt \
		src/risseStaticStringsIds.inc src/risseStaticStringsData.def

# risseNativeBinderTemplates.inc のコンパイル
BIN_DEPS += src/risseNativeBinderTemplates.inc

src/risseNativeBinderTemplates.inc: \
		tools/create_binder.rb Makefile
	ruby tools/create_binder.rb > src/risseNativeBinderTemplates.inc

# risseStringTemplates.inc のコンパイル
BIN_DEPS += src/risseStringTemplates.inc

src/risseStringTemplates.inc: \
		tools/create_ss_templates.rb Makefile
	ruby tools/create_ss_templates.rb > src/risseStringTemplates.inc



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

# risseDateParser.cpp と risseDateParser.inc のコンパイル
BIN_DEPS += src/builtin/date/risseDateParser.inc

src/builtin/date/risseDateParser.cpp src/builtin/date/risseDateParser.inc: \
		src/builtin/date/risseDateParser.y Makefile
	@echo Compiling src/builtin/date/risseDateParser.y
	@(set -e ;\
		cd src/builtin/date ;\
		bison -v --name-prefix=rissedp risseDateParser.y ;\
		cat risseDateParser.tab.c | \
			sed -e "s|risseDateParser.tab.h|src/builtin/date/risseDateParser.inc|" \
				-e "s|risseDateParser.tab.c|src/builtin/date/risseDateParser.cpp|" \
				-e "s|risseDateParser\.y|src/builtin/date/risseDateParser.y|" \
				>  risseDateParser.cpp ;\
		ruby ../../../tools/extract_yytokentype.rb risseDateParser.tab.c > risseDateParser.inc; \
		rm risseDateParser.tab.c ;\
		(grep -i "As a special exception" risseDateParser.cpp > /dev/null) || (\
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

ifdef RISSE_NO_COPY_INCLUDES
Risse_INCLUDE_DIR := $(shell pwd)/src
else
Risse_INCLUDE_DIR := $(shell pwd)/include
endif

# risse-config を作成する。

$(Risse_BIN_OUTPUT_DIR)/risse-config: Makefile risse-config.in
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

BIN_DEPS += $(Risse_BIN_OUTPUT_DIR)/risse-config


#----------------------------#

# ヘッダファイルを集めてコピーする
ifndef RISSE_NO_COPY_INCLUDES

RISSE_HEADER_FILES := $(filter-out ./prec.h,$(shell sh -c "cd src; find . -name '*.h'") $(shell sh -c "cd src; find . -name '*.inc'"))
RISSE_HEADER_DESTS := $(subst ./,include/,$(RISSE_HEADER_FILES))

include/% :: src/%
	mkdir -p $(dir $@)
	cp $< $@

BIN_DEPS += $(RISSE_HEADER_DESTS)

endif

#----------------------------#
include $(TOP_DIR)/Makefile_common
