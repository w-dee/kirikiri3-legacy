all_target: all_cpp prepare binary


PRESERVE_ASM=1
DEBUG=no
CXXFLAGS=-DRISA_RINGBUFFER_NO_GC

CORE_DIR=../../../core
include $(CORE_DIR)/Makefile_core_common


all_cpp:
	cat ../empty.cpp \
		$(CORE_DIR)/sound/opt_default/WaveFormatConverter_Default.cpp \
		$(CORE_DIR)/sound/filter/phasevocoder/opt_default/PhaseVocoderDSP_Default.cpp \
		$(CORE_DIR)/base/mathlib/opt_default/RealFFT_Default.cpp \
		$(CORE_DIR)/base/mathlib/opt_default/MathAlgorithms_Default.cpp \
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

