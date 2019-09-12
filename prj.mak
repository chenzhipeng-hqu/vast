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

TARGET_MODULE ?= $(shell pwd |sed 's/^\(.*\)[/]//' )
#TARGET := $(TARGET_MODULE).a # the same name as dir.
TARGET := vast.a # the same name as dir.

################ vast lib ################

DEFS += 

LIBS += 

INCS += -I./vast
INCS += -I./vast/vast_core
INCS += -I./vast/vast_driver
INCS += -I./vast/vast_device

SRCS += vast/vast_core/device.c
SRCS += vast/vast_core/heap.c
SRCS += vast/vast_core/kfifo.c
SRCS += vast/vast_core/object.c
SRCS += vast/vast_core/softtimer.c
SRCS += vast/vast_core/utils.c
SRCS += vast/vast_device/vast_led.c


################ canopen lib ################

LIBS += 

