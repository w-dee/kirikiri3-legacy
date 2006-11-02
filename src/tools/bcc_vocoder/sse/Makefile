all_target: all_cpp prepare binary


PRESERVE_ASM=1
DEBUG=no
USE_SSE=1

CORE_DIR=../../../core
include $(CORE_DIR)/Makefile_core_common


all_cpp:
	cat \
		$(CORE_DIR)/sound/filter/phasevocoder/opt_sse/PhaseVocoderDSP_SSE.cpp \
		$(CORE_DIR)/base/mathlib/opt_sse/RealFFT_SSE.cpp \
		$(CORE_DIR)/base/cpu/opt_sse/xmmlib.cpp \
		$(CORE_DIR)/base/mathlib/opt_sse/MathAlgorithms_SSE.cpp \
		> all.cpp
#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=dummy

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# リンカに渡すフラグ
LDFLAGS = -g $(CUI_LDFLAGS)

#----------------------------#

CPPFILES = all.cpp

OBJS =  $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

