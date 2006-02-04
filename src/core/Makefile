# makefile のあるサブディレクトリの定義

SUBDIRS= \
	risse              \
	base/exception     \
	base/fs            \
	base/script        \
	base/utils         \
	sound



# デフォルトターゲット
target: default

# 各サブディレクトリに対してMakeを実行する

default:
	(set -e; for dir in $(SUBDIRS); do (set -e; cd $$dir && $(MAKE)           ) || exit 1; done )

distclean:
	(set -e; for dir in $(SUBDIRS); do (set -e; cd $$dir && $(MAKE)  distclean) || exit 1; done )

clean:
	(set -e; for dir in $(SUBDIRS); do (set -e; cd $$dir && $(MAKE)  clean    ) || exit 1; done )


