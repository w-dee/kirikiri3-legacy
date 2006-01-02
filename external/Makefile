TOP_DIR=..
include $(TOP_DIR)/Makefile_filenames

# サブディレクトリの定義

SUBLIBS= zlib libpng libjpeg expat wxWidgets icu libtomcrypt freetype boost


# デフォルトターゲット
target: default compile_wx_win_res copy_mo copy_dll print_all_done_mes

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


# Windows の場合、dll は実行可能ファイルと同じ場所か、
# パスの通った場所におかれていなければならない
# ここでは、生成された DLL をすべて探し、$(BUILD_OUT_DIR)/bin
# にコピーする
# また、マルチスレッドヘルパーである mingwm*.dll も bin ディレ
# クトリにコピーする
ifeq ($(PLATFORM),windows)
 copy_dll:
	mkdir -p $(BUILD_OUT_DIR)/bin
	@cp `which mingwm10.dll` $(BUILD_OUT_DIR)/bin || \
		(echo "error: mingwm10.dll missing." ; exit 1)
	@cp `which unicows.dll` $(BUILD_OUT_DIR)/bin || \
		(echo "error: unicows.dll missing. did you forget installing \
		MSLU into MinGW bin directory ?" ; exit 1)
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

ifeq ($(PLATFORM),windows)
 compile_wx_win_res:
	(set -e ;\
		WX_WIN_RES_OBJ=wxWidgets/lib/wx_msw_rc.o ;\
		WX_MSW_INCLUDE_DIR=`find wxWidgets/include -name msw -type d` ;\
		WX_INCLUDE_DIR=$$WX_MSW_INCLUDE_DIR/../.. ;\
		WX_WIN_RES_SRC=$$WX_INCLUDE_DIR/wx/msw/wx.rc ;\
		if [ "x$$WX_MSW_INCLUDE_DIR" = "x" ]; then \
			echo "error: wxWidgets was not properly built. stop"; exit 1; fi ;\
		$(WINDRES) --include-dir $$WX_INCLUDE_DIR $$WX_WIN_RES_SRC -o $$WX_WIN_RES_OBJ \
	)
else
 compile_wx_win_res:
	@true
endif

### 構築終了のメッセージを表示する
print_all_done_mes:
	@echo
	@echo All done.

