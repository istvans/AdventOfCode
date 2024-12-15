MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

ifdef Y
    YEAR = $Y
    YEAR_DIR = ${MAKEFILE_DIR}/${YEAR}
else
    $(error Y=year is undefined)
endif

ifdef D
    DAY = $D
    DAY_DIR = ${YEAR_DIR}/${DAY}
else
    $(error D=day is undefined)
endif

.PHONY: $(DAY_DIR)
$(DAY_DIR):
	$(MAKE) -C ${YEAR_DIR}

.PHONY: test
test:
	$(MAKE) -C ${YEAR_DIR} test

