# makefile のあるサブディレクトリの定義

SUBSYS_DIRS=\
	base/exception     \
	base/fs            \
	base/log           \
	base/main          \
	base/script        \
	base/utils         \
	sound

SUB_DIRS = risse  $(SUBSYS_DIRS)

export SUBSYS_DIRS


# デフォルトターゲット
target: subdirs link

# 各サブディレクトリに対してMakeを実行する

subdirs:
	(set -e; \
	pwd=$$(PWD); \
	for dir in $(SUB_DIRS); do \
		cd $$dir; \
		$(MAKE) || (echo "To compile only the failed directory:"; \
						echo "(cd $$dir && $(MAKE) )"; exit 1) ;\
		ret=$$?; \
		if [ "$$ret" != "0" ]; then \
			exit 1; \
		fi; \
		cd $$pwd; \
	done )

distclean:
	(set -e; for dir in $(SUB_DIRS); do (set -e; cd $$dir && $(MAKE)  distclean) || exit 1; done )

clean:
	(set -e; for dir in $(SUB_DIRS); do (set -e; cd $$dir && $(MAKE)  clean    ) || exit 1; done )


# リンク
link:
	$(MAKE) -f Makefile_core

