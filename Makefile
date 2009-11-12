risa:
	(cd src/core && $(MAKE))

clean:
	(cd src/core && $(MAKE) clean)

rissetest:
	(cd src/core/risse && $(MAKE))
	(cd tests/rissetest && $(MAKE))
	