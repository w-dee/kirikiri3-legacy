#!/bin/sh
cd ../../src/core && make ADDITONAL_SUBSYS_DIRS='$(CORE_DIR)/../../tests/core/rina'
