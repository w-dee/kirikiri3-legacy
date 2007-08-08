#-----------------------------------------
# トップディレクトリを相対ディレクトリで指すようにここは修正
#-----------------------------------------
CORE_DIR=..

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_subsys_pre

#-----------------------------------------
# このディレクトリにあるソースファイルを列挙
#-----------------------------------------
CPPFILES=\
		ALBuffer.cpp                               \
		ALCommon.cpp                               \
		ALSource.cpp                               \
		Sound.cpp                                  \
		WaveDecoder.cpp                            \
		WaveFormatConverter.cpp                    \
		WaveLoopManager.cpp                        \
		WaveSegmentQueue.cpp                       \
		decoder/riffwave/RIFFWaveDecoder.cpp       \
		decoder/vorbis/VorbisDecoder.cpp           \
		filter/BasicWaveFilter.cpp                 \
		filter/phasevocoder/PhaseVocoder.cpp       \
		filter/phasevocoder/PhaseVocoderDSP.cpp    \
		filter/reverb/fv_allpass.cpp               \
		filter/reverb/fv_comb.cpp                  \
		filter/reverb/fv_revmodel.cpp              \
		filter/reverb/Reverb.cpp                   

ifdef USE_SSE
 CPPFILES += \
		opt_sse/WaveFormatConverter_SSE.cpp \
		filter/phasevocoder/opt_sse/PhaseVocoderDSP_SSE.cpp
else
 CPPFILES += \
		opt_default/WaveFormatConverter_Default.cpp \
		filter/phasevocoder/opt_default/PhaseVocoderDSP_Default.cpp
endif


#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_subsys_post

