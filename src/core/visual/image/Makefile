#-----------------------------------------
# トップディレクトリを相対ディレクトリで指すようにここは修正
#-----------------------------------------
CORE_DIR=../..

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_subsys_pre

#-----------------------------------------
# このディレクトリにあるソースファイルを列挙
#-----------------------------------------
CPPFILES=\
	Image.cpp                          \
	ImageBuffer.cpp                    \
	ImageCodec.cpp                     \
	PixelType.cpp                      \
	codec/BMPCodec.cpp                 \
	codec/PNGCodec.cpp                 \
	codec/JPEGCodec.cpp                \
	codec/TLGCodec.cpp                 

ifdef USE_YASM
 ASMFILES = \
      codec/opt_x86/tlg_core.yas       \
      codec/opt_x86/tlg6_golomb.yas
else
 CPPFILES += codec/opt_default/tlg_core.cpp
endif

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_subsys_post

