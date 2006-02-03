TOP_DIR=../..
include $(TOP_DIR)/Makefile_filenames

#----------------------------#

# プログラムID (プログラムやライブラリのbasenameとなる)
PROGRAM_ID=openaltest

# 出力バイナリ名
BIN=$(BUILD_OUT_DIR)/bin/$(PROGRAM_ID)$(BIN_SUFFIX)

# このバイナリが依存しているライブラリ
DEP_LIBS=BOOST ZLIB WX LIBOGG LIBVORBIS OPENAL COMMON

# リンカに渡すフラグ
LDFLAGS = -g $(CONSOLE_LDFLAGS)

#----------------------------#

# risse を使うための設定
include $(TOP_DIR)/src/core/risse/Makefile_for_user

#----------------------------#

CORE_DIR=$(TOP_DIR)/src/core

INCLUDES= \
		-I$(CORE_DIR)/base/fs/common           \
		-I$(CORE_DIR)/base/fs/osfs             \
		-I$(CORE_DIR)/base/fs/pathfs           \
		-I$(CORE_DIR)/base/fs/tmpfs            \
		-I$(CORE_DIR)/base/fs/xp4fs            \
		-I$(CORE_DIR)/base/exception           \
		-I$(CORE_DIR)/base/script              \
		-I$(CORE_DIR)/base/sound               \
		-I$(CORE_DIR)/base/decoder             \
		-I$(CORE_DIR)/base/decoder/riffwave    \
		-I$(CORE_DIR)/base/decoder/vorbis      \
		-I$(CORE_DIR)/base/utils               \
		-I$(CORE_DIR)/base/log                 \
		-I$(CORE_DIR)/sound                    \
		-I$(CORE_DIR)/sound/decoder            \
		-I$(CORE_DIR)/sound/decoder/riffwave   \
		-I$(CORE_DIR)/sound/decoder/vorbis     \
		-I$(CORE_DIR)/sound/filter             \
		-I$(CORE_DIR)/sound/filter/phasevocoder\
		-I$(CORE_DIR)/sound/filter/reverb      \
		-I.

CPPFILES = \
		$(CORE_DIR)/base/fs/common/FSManager.cpp                     \
		$(CORE_DIR)/base/fs/common/FSManagerBind.cpp                 \
		$(CORE_DIR)/base/fs/osfs/OSFS.cpp                            \
		$(CORE_DIR)/base/fs/osfs/OSFSBind.cpp                        \
		$(CORE_DIR)/base/fs/pathfs/PathFS.cpp                        \
		$(CORE_DIR)/base/fs/pathfs/PathFSBind.cpp                    \
		$(CORE_DIR)/base/fs/tmpfs/MemoryStream.cpp                   \
		$(CORE_DIR)/base/fs/tmpfs/TmpFS.cpp                          \
		$(CORE_DIR)/base/fs/tmpfs/TmpFSBind.cpp                      \
		$(CORE_DIR)/base/fs/xp4fs/DecompressedHolder.cpp             \
		$(CORE_DIR)/base/fs/xp4fs/XP4Archive.cpp                     \
		$(CORE_DIR)/base/fs/xp4fs/XP4FS.cpp                          \
		$(CORE_DIR)/base/fs/xp4fs/XP4SegmentCache.cpp                \
		$(CORE_DIR)/base/fs/xp4fs/XP4Stream.cpp                      \
		$(CORE_DIR)/base/fs/xp4fs/XP4StreamCache.cpp                 \
		$(CORE_DIR)/base/fs/xp4fs/XP4FSBind.cpp                      \
		$(CORE_DIR)/base/exception/RisaException.cpp                 \
		$(CORE_DIR)/base/script/RisseEngine.cpp                      \
		$(CORE_DIR)/base/utils/fftsg.cpp                             \
		$(CORE_DIR)/base/utils/Singleton.cpp                         \
		$(CORE_DIR)/sound/WaveFormatConverter.cpp                    \
		$(CORE_DIR)/sound/WaveLoopManager.cpp                        \
		$(CORE_DIR)/sound/ALCommon.cpp                               \
		$(CORE_DIR)/sound/ALSource.cpp                               \
		$(CORE_DIR)/sound/ALBuffer.cpp                               \
		$(CORE_DIR)/sound/WaveSegmentQueue.cpp                       \
		$(CORE_DIR)/sound/decoder/riffwave/RIFFWaveDecoder.cpp       \
		$(CORE_DIR)/sound/decoder/vorbis/VorbisDecoder.cpp           \
		$(CORE_DIR)/sound/filter/BasicWaveFilter.cpp                 \
		$(CORE_DIR)/sound/filter/phasevocoder/PhaseVocoder.cpp       \
		$(CORE_DIR)/sound/filter/phasevocoder/PhaseVocoderDSP.cpp    \
		$(CORE_DIR)/sound/filter/reverb/fv_allpass.cpp               \
		$(CORE_DIR)/sound/filter/reverb/fv_comb.cpp                  \
		$(CORE_DIR)/sound/filter/reverb/fv_revmodel.cpp              \
		$(CORE_DIR)/sound/filter/reverb/Reverb.cpp                   \
		main.cpp

#		$(CORE_DIR)/base/log/Log.cpp                                 \

OBJS = $(CPPFILES:.cpp=.o)

#----------------------------#

include $(TOP_DIR)/Makefile_common

