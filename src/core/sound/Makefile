#-----------------------------------------
# トップディレクトリを相対ディレクトリで指すようにここは修正
#-----------------------------------------
CORE_DIR=..

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_pre

#-----------------------------------------
# このディレクトリにあるソースファイルを列挙
#-----------------------------------------
CPPFILES=\
		WaveFormatConverter.cpp                    \
		WaveLoopManager.cpp                        \
		ALCommon.cpp                               \
		ALSource.cpp                               \
		ALBuffer.cpp                               \
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

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_post

