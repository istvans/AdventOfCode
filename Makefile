MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))

ifdef YEAR
    YEAR_DIR = ${MAKEFILE_DIR}/${YEAR}
else
    $(error the YEAR is undefined)
endif

ifdef LEVEL
    LEVEL_DIR = ${YEAR_DIR}/Level${LEVEL}
else
    $(error the LEVEL is undefined)
endif

.PHONY: $(LEVEL_DIR)

$(LEVEL_DIR):
	$(MAKE) -C $@
