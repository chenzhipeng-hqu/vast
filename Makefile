################################################################################
# Copyright (C) 2017 Maker-ray Co.,Ltd. All rights reserved.
#  __  __       _                                  
# |  \/  | __ _| | _____ _ __      _ __ __ _ _   _ 
# | |\/| |/ _` | |/ / _ \ '__|____| '__/ _` | | | |
# | |  | | (_| |   <  __/ | |_____| | | (_| | |_| |
# |_|  |_|\__,_|_|\_\___|_|       |_|  \__,_|\__, |
#                                            |___/ 
# 
# $FileName: Makefile
# $Date: 2017-08-31
# $Author: xwk
# $Note:
#   
################################################################################

export PRJ_ROOT ?= $(shell pwd)

export ARCH := $(shell uname -s)
export LINUX := Linux
ifeq ($(ARCH),$(LINUX))
export TOOL_PATH := 
#export ARM_PREFIX := $(TOOL_PATH)arm-none-eabi-
export ARM_PREFIX := $(TOOL_PATH)
else
export TOOL_PATH := 
export ARM_PREFIX := $(TOOL_PATH)arm-none-eabi-
endif
export CC := $(ARM_PREFIX)gcc
export CXX := $(ARM_PREFIX)g++
export LD := $(ARM_PREFIX)ld
export OBJCOPY := $(ARM_PREFIX)objcopy
export OBJDUMP := $(ARM_PREFIX)objdump
export STRIP := $(ARM_PREFIX)strip
export AR := $(ARM_PREFIX)ar
export RANLIB := $(ARM_PREFIX)ranlib
#export ARCH := $(uname -s)
#export LINUX := "Linux"
export GDB := $(ARM_PREFIX)gdb
export SIZE := $(ARM_PREFIX)size

export BUILD_ROOT ?= build/

include prj.mak

CUR_ROOT := $(shell pwd)
CURR_ROOT := $(subst $(PRJ_ROOT)/,,$(CUR_ROOT)/)


BUILD_PATH_UP := $(shell echo $(CURR_ROOT) | sed 's,[^/]\+,..,g')

BUILD_PATH := ./$(BUILD_PATH_UP)$(BUILD_ROOT)$(CURR_ROOT)

BUILD_GEN_PATH := $(BUILD_PATH)

TARGET_BIN := $(BUILD_PATH)/$(TARGET)
TARGET_ELF := $(TARGET_BIN:.bin=.elf)
TARGET_HEX := $(TARGET_ELF:.elf=.hex)
TARGET_MAP := $(TARGET_ELF:.elf=.map)
TARGET_LIST:= $(TARGET_ELF:.elf=.list)

TARGET_PATH = application/bin
TARGET_BASEBIN = $(TARGET_PATH)/atsxBase.bin
TARGET_BOOTBIN = $(TARGET_PATH)/F407_bl_eth.bin
TARGET_BOOT_MERGE = $(TARGET_PATH)/F407_bl_merge.bin
TARGET_MERGE = $(TARGET_PATH)/$(TARGET_MODULE)_merge.bin

SRCS_C := $(filter %.c,$(SRCS))
SRCS_ASM := $(filter %.s,$(SRCS))
SRCS_CXX := $(filter %.cpp,$(SRCS))
OBJS := $(addprefix $(BUILD_GEN_PATH),$(strip $(SRCS_C:.c=.obj) $(SRCS_ASM:.s=.obj) $(SRCS_CXX:.cpp=.obj)))
DEPS := $(OBJS:.obj=.dep)

AFLAGS += -u _printf_float -u _sscanf_float -lm
AFLAGS += -O0
AFLAGS +=  -g
AFLAGS += -m32
AFLAGS += -fPIC
AFLAGS += -Wall
CFLAGS += $(DEFS) $(INCS)
CFLAGS += $(AFLAGS)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wundef -Wall
#CFLAGS +=  -ffunction-sections -fdata-sections  -I/usr/include -w -D_REENTRANT -D_LINUX -DHAVE_SYS_SIGNALS
CFLAGS +=  -I/usr/include -w -D_REENTRANT -D_LINUX -DHAVE_SYS_SIGNALS
CFLAGS += -Werror
CFLAGS += -Wall
#LDFLAGS += -Wl,-gc-sections -Wl,-Map=$(TARGET_MAP)
#LDFLAGS += -ffunction-sections -fdata-sections -Wl,-Map=rtthread-linux.map -pthread -T gcc.ld
LDFLAGS += $(AFLAGS) -T $(LDSCRIPT)
LDFLAGS += -Wl,-Map=$(TARGET_MAP) -pthread
LDFLAGS += -Wl,--start-group -lm -lc -lgcc -Wl,--end-group

.PHONY: all clean


all: $(TARGET_ELF)

	
$(TARGET_ELF): $(OBJS) $(LIBS)
	@$(CXX) $(LDFLAGS) $^ -o $@
	@echo [build] $@ OK!

$(LIBS)::
	@$(MAKE) -C $(PRJ_ROOT)/$(basename $(@F))

clean:
	-rm -rf $(BUILD_PATH)

$(BUILD_GEN_PATH)%.obj: %.cpp
	@echo "cc $<"
	@if [ ! -d $(@D) ]; then mkdir -p "$(@D)"; fi
	@$(CXX) $(CFLAGS) -c $< -o $@  -MMD -MT "$@" -MF $(@:.obj=.dep)

$(BUILD_GEN_PATH)%.obj: %.s
	@echo "cc $<"
	@if [ ! -d $(@D) ]; then mkdir -p "$(@D)"; fi
	@$(CC) $(CFLAGS) -c $< -o $@  -MMD -MT "$@" -MF $(@:.obj=.dep)
	
$(BUILD_GEN_PATH)%.obj: %.c Makefile prj.mak
	@echo "cc $<"
	@if [ ! -d $(@D) ]; then mkdir -p "$(@D)"; fi
	@$(CC) $(CFLAGS) -c $< -o $@  -MMD -MT "$@" -MF $(@:.obj=.dep)
	
$(BUILD_GEN_PATH)%.dep:

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

before-build:
	-@echo 'this is before_build'
	-@echo ' '

#after-build: $(TARGET_BOOT_MERGE) $(TARGET_BIN)
after-build:
	-@echo 'this is after_build'
	-@echo ' '

secondary-outputs: $(TARGET_ELF)
	@$(SIZE)  $<
	@$(OBJDUMP) -h -S $< > "$(TARGET_LIST)"

#######################################
# Debug flags
#######################################
OOCD             := openocd
OOCD_INTERFACE   := /usr/share/openocd/scripts/interface/stlink-v2.cfg
OOCD_TARGET      := /usr/share/openocd/scripts/target/stm32f4x_stlink.cfg

debug: before-build $(BUILD_PATH)/$(TARGET_MODULE).debug
OOCDFLAGS := -f $(OOCD_INTERFACE) -f $(OOCD_TARGET)

%.debug: $(TARGET_ELF)
	@printf "  GDB DEBUG $<\n"
	@$(GDB) -iex 'target extended | $(OOCD) $(OOCDFLAGS) -c "gdb_port pipe"' \
	-iex 'monitor reset halt' -ex 'load' -ex '-' -ex 'break main' -ex 'c' $^


show:
	-@echo ARCH = $(ARCH)
	-@echo LINUX = $(LINUX)
	-@echo $(shell pwd |sed 's/^\(.*\)[/]//' )
	-@echo $(TARGET_MODULE)
	-@echo $(lastword  $(MAKEFILE_LIST))
	-@echo PRJ_ROOT = $(PRJ_ROOT)
	-@echo CUR_ROOT = $(CUR_ROOT)
	-@echo CURR_ROOT = $(CURR_ROOT)
	-@echo BUILD_PATH_UP = $(BUILD_PATH_UP)
	-@echo BUILD_PATH = $(BUILD_PATH)
	-@echo BUILD_ROOT = $(BUILD_ROOT)
	-@echo BUILD_GEN_PATH = $(BUILD_GEN_PATH)
	-@echo $(foreach n, $(LIBS),$(n)"\r\n") 
	-@echo $(foreach n, $(SRCS_ASM),$(n)"\r\n") 
	-@echo $(foreach n, $(SRCS_C),$(n)"\r\n") 
	-@echo $(foreach n, $(INCS),$(n)"\r\n") 

