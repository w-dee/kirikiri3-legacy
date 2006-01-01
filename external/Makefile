TOP_DIR=..
include $(TOP_DIR)/Makefile_filenames

# サブディレクトリの定義

SUBLIBS= zlib libpng libjpeg expat wxWidgets icu libtomcrypt freetype boost


# デフォルトターゲット
target: default compile_wx_win_res copy_mo copy_dll

# 各サブディレクトリに対して再帰

default:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  all      ); done

clean:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  clean    ); done

download:
	for dir in $(SUBLIBS); do ( cd $$dir && $(MAKE)  download ); done



#### copy_dll

# DLL や so などをターゲットの bin ディレクトリや lib ディレクトリに配置する

# TODO: win32 以外の動作

ifeq ($(PLATFORM),windows)
 copy_dll:
	mkdir -p $(BUILD_OUT_DIR)/bin
	for dir in $(SUBLIBS) ; do \
		for subdir in bin lib; do \
			if [ -d $$dir/$$subdir ]; then \
				files=`find $$dir/$$subdir -name '*.dll'` ;\
				if [ "x$$files" != "x" ]; then \
					cp -p $$files $(BUILD_OUT_DIR)/bin;\
				fi ;\
			fi ;\
		done ;\
	done
endif



#### copy_mo

# mo ファイルをターゲットの share/locale ディレクトリに配置する

copy_mo:
	mkdir -p $(BUILD_OUT_DIR)/share
	for dir in $(SUBLIBS) ; do \
		if [ -d $$dir/share/locale ]; then \
			cp -pR $$dir/share/locale $(BUILD_OUT_DIR)/share ;\
		fi ;\
	done


### wxWidgets の windows 用のデフォルトのリソースファイルをコンパイルする

WX_WIN_RES_OBJ  = wxWidgets/lib/wx_msw_rc.o
WX_INCLUDE_DIR := $(shell find wxWidgets/include -name msw -type d)/../..
WX_WIN_RES_SRC  = $(WX_INCLUDE_DIR)/wx/msw/wx.rc

compile_wx_win_res: $(WX_WIN_RES_OBJ)

$(WX_WIN_RES_OBJ): $(WX_WIN_RES_SRC)
	$(WINDRES) --include-dir $(WX_INCLUDE_DIR) $(WX_WIN_RES_SRC) -o $(WX_WIN_RES_OBJ)
