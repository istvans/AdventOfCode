MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

ifdef YEAR
    YEAR_DIR = ${MAKEFILE_DIR}/${YEAR}
else
    $(error the YEAR is undefined)
endif

ifdef DAY
    DAY_DIR = ${YEAR_DIR}/Day${DAY}
else
    $(error the DAY is undefined)
endif

.PHONY: $(DAY_DIR)

$(DAY_DIR):
	$(MAKE) -C $@
