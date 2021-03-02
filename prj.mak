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

# set default TARGET_MODULE as ats9
export TARGET_MODULE ?= vanca_test

TARGET := $(TARGET_MODULE).elf

LDSCRIPT := gcc.ld


DEFS += -DDEBUG

DEFS += -DGUN_SOURCE

DEFS += -DUSE_$(TARGET_MODULE)

################ application ################
LIBS += 

INCS += -I./

#SRCS_DIRS += Core/Src

################ bsp ################
INCS += -I./application/bsp

SRCS += application/bsp/bsp.c


################ port ################
INCS += -I./application/port/easylogger
SRCS += application/port/easylogger/elog_port.c

INCS += -I./application/port/core
SRCS += application/port/core/core_port.c

################ task ################
INCS += -I./application/task

SRCS += application/task/main.c


################ vanca ################
export VANCA_ROOT ?= ./vanca

include ./application/config.mk

include $(VANCA_ROOT)/prj.mak


################ all .c in SRCS_DIRS  ################
SRCS += $(foreach dir,$(SRCS_DIRS),$(wildcard $(dir)/*.c))


