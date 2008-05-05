risa:
	(cd src/core && $(MAKE))

rissetest:
	(cd src/core/risse && $(MAKE))
	(cd tests/rissetest && $(MAKE))
	