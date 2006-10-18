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
CPPFILES =

ifdef USE_SSE
 CPPFILES += \
 		opt_sse/RealFFT_SSE.cpp           \
 		opt_sse/MathAlgorithms_SSE.cpp    
else
 CPPFILES += \
 		opt_default/RealFFT_Default.cpp           \
 		opt_default/MathAlgorithms_Default.cpp    
endif

#-----------------------------------------
# ここのセクションは通常変更する必要なし
#-----------------------------------------
include $(CORE_DIR)/Makefile_core_subsys_post

