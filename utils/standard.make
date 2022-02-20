ifndef MAKEFILE_DIR
    $(error the MAKEFILE_DIR is undefined)
endif

UTILS_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
UTILS_DIR := $(dir $(UTILS_PATH))
ROOT_DIR := ${UTILS_DIR}/..

.PHONY: prepare clean build run build_and_run clean_build_and_run
.DEFAULT_GOAL := build_and_run

# Conditional variable assignments:
COMPILER ?= clang
COMPILER_FLAGS ?=
INPUT ?= input.txt
RUN_ARGS ?= -i ${INPUT}
CPP_VERSION ?= c++20
SOURCE_NAME ?= main.cpp
NEEDS_BOOST ?= true
BINARY_NAME ?= solver
CONFIG ?= release

ifeq (${CONFIG}, release)
    ifeq (${COMPILER}, msvc)
        OPTIMISATION := /O2
    else
        OPTIMISATION := -O3
    endif
else ifeq (${CONFIG}, debug)
    ifeq (${COMPILER}, msvc)
        OPTIMISATION := /Od
    else
        OPTIMISATION := -O0
    endif
else
    $(error ${CONFIG} is not a supported configuration (choose from release or debug))
endif

SOURCE := ${MAKEFILE_DIR}/${SOURCE_NAME}

BINARY_DIR := ${MAKEFILE_DIR}/bin
BINARY := ${BINARY_DIR}/${BINARY_NAME}

BOOST_HEADER_DIR := /usr/include/boost

ifeq (${NEEDS_BOOST}, true)
    ifeq (${COMPILER}, msvc)
        INCLUDE_BOOST := /I:${BOOST_HEADER_DIR}
    else
        INCLUDE_BOOST := -I${BOOST_HEADER_DIR} -lboost_program_options
    endif
else
    INCLUDE_BOOST :=
endif

ifeq (${COMPILER}, clang)
    COMPILER_CMD := clang++ -std=${CPP_VERSION} -Weverything -Werror \
    -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-overlength-strings \
    -Wno-padded \
    -I${UTILS_DIR} ${OPTIMISATION} ${COMPILER_FLAGS}
    BUILD := ${COMPILER_CMD} ${INCLUDE_BOOST} ${SOURCE} -o ${BINARY}
else ifeq (${COMPILER}, gcc)
    COMPILER_CMD := g++ -std=${CPP_VERSION} -pedantic -Wall -Wextra -Wcast-align \
    -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
    -Wlogical-op -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
    -Wredundant-decls -Wshadow -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 \
    -Wswitch-default -Wundef -Werror -Wno-unused \
    ${OPTIMISATION} ${COMPILER_FLAGS}
    BUILD := ${COMPILER_CMD} ${INCLUDE_BOOST} ${SOURCE} -o ${BINARY}
else ifeq (${COMPILER}, msvc)
    BUILD := c:/windows/system32/cmd.exe /c "vcvars64.bat && cl.exe /std:${CPP_VERSION} /W4 /WX ${COMPILER_FLAGS} \
    /EHsc ${SOURCE} /Fe:${BINARY} ${INCLUDE_BOOST} ${OPTIMISATION}"
else
    $(error ${COMPILER} is not a supported compiler (choose from clang, gcc or msvc))
endif

${BINARY}: ${SOURCE}
	${BUILD}

${BINARY_DIR}:
	@echo "Folder ${BINARY_DIR} does not exist"
	@mkdir -p $@

clean:
	@rm -rf ${BINARY_DIR}

build: ${BINARY_DIR} ${BINARY}

run:
	${BINARY} ${RUN_ARGS}

build_and_run: build
	${MAKE} run

clean_build_and_run: clean build
	${MAKE} run
