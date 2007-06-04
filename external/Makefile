TOP_DIR=..
include $(TOP_DIR)/Makefile_filenames

# サブディレクトリの定義

SUBLIBS= boost zlib libpng libjpeg expat wxWidgets libtomcrypt freetype libogg libvorbis openal gc


# デフォルトターゲット
target: default compile_wx_win_res copy_mo copy_dll print_all_done_mes

# 各サブディレクトリに対して再帰

default:
	(set -e; for dir in $(SUBLIBS); do (set -e; cd $$dir && $(MAKE)  all      ) || exit 1; done )

distclean:
	(set -e; for dir in $(SUBLIBS); do (set -e; cd $$dir && $(MAKE)  distclean) || exit 1; done )

clean:
	(set -e; for dir in $(SUBLIBS); do (set -e; cd $$dir && $(MAKE)  clean    ) || exit 1; done )

download:
	(set -e; for dir in $(SUBLIBS); do (set -e; cd $$dir && $(MAKE)  download ) || exit 1; done )



#### copy_dll

# DLL や so などをターゲットの bin ディレクトリや lib ディレクトリに配置する

# TODO: win32/unix 以外の動作


# Windows の場合、dll は実行可能ファイルと同じ場所か、
# パスの通った場所におかれていなければならない
# ここでは、生成された DLL をすべて探し、$(BUILD_OUT_DIR)/bin
# にコピーする
# また、マルチスレッドヘルパーである mingwm*.dll も bin ディレ
# クトリにコピーする
ifeq ($(PLATFORM),windows)
 copy_dll: copy_dll_w32 do_copy_dll
else
 copy_dll: copy_dll_so
endif

copy_dll_w32:
	mkdir -p $(BUILD_OUT_DIR)/bin
	@cp `which mingwm10.dll` $(BUILD_OUT_DIR)/bin || \
		(echo "error: mingwm10.dll missing." ; exit 1)
	@cp `which unicows.dll` $(BUILD_OUT_DIR)/bin || \
		(echo "error: unicows.dll missing. did you forget installing \
		MSLU into MinGW bin directory ?" ; exit 1)

do_copy_dll:
	@echo "copying dlls to the target directory ..."
	mkdir -p $(BUILD_OUT_DIR)/bin
	@for dir in $(SUBLIBS) ; do \
		for subdir in bin lib src_temp/bin.v2; do \
			if [ -d $$dir/$$subdir ]; then \
				files=`find $$dir/$$subdir -name '*$(DLL_SUFFIX)'` ;\
				if [ "x$$files" != "x" ]; then \
					cp -p $$files $(BUILD_OUT_DIR)/bin;\
				fi ;\
			fi ;\
		done ;\
	done

# Linuxの場合、.soは、実体ではなく、
# シンボリックリンクになってることがあるので、
# 必要なら、シンボリックを辿る
# (copy_dll_sub.sh参照)
copy_dll_so:
	@echo "copying dlls to the target directory ..."
	@mkdir -p $(BUILD_OUT_DIR)/bin
	@for dir in $(SUBLIBS) ; do \
		for subdir in bin lib; do \
			if [ -d $$dir/$$subdir ]; then \
				find $$dir/$$subdir -name '*$(DLL_SUFFIX)' | sh copy_so_sub.sh $(BUILD_OUT_DIR)/bin; \
			fi ;\
		done ;\
	done



#### copy_mo

# mo ファイルをターゲットの share/locale ディレクトリに配置する

copy_mo:
	mkdir -p $(BUILD_OUT_DIR)/share
	@echo "copying message objects to the target directory ..."
	@for dir in $(SUBLIBS) ; do \
		if [ -d $$dir/share/locale ]; then \
			cp -pR $$dir/share/locale $(BUILD_OUT_DIR)/share ;\
		fi ;\
	done


### wxWidgets の windows 用のデフォルトのリソースファイルをコンパイルする

ifeq ($(PLATFORM),windows)
 compile_wx_win_res:
	@echo "compiling wxWidgets resource ..."
	@(set -e ;\
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

