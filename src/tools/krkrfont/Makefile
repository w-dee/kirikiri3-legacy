TOP_DIR=../../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=krkrfont

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=WX FREETYPE COMMON

# リンカに渡すフラグ
LDFLAGS = -g $(GUI_LDFLAGS)

# 最終ターゲット (ふつう、出力バイナリや出力ライブラリ)
default: $(BIN)

#----------------------------#

XRCFILES = krkrfont_res.xrc

CPPFILES =  $(XRCFILES:.xrc=.cpp) \
			KrkrFontApp.cpp \
			msw/NativeFreeTypeFace.cpp msw/NativeFreeTypeDriver.cpp \
			MainDialog.cpp FreeType.cpp PreviewWindow.cpp \
			FontDriver.cpp WriteBFF.cpp uni_cp932.cpp cp932_uni.cpp

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#
include $(TOP_DIR)/Makefile_common
