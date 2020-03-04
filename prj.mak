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

DEFS += -DVAST_MODULE_ENABLED

################ vast ################
INCS += -I./vast
#INCS += -I./vast/core
#INCS += -I./vast/driver
#INCS += -I./vast/device
#INCS += -I./vast/component
INCS += -I./vast/third/cjson
INCS += -I./vast/third/qpc/include
INCS += -I./vast/third/qpc/src
INCS += -I./vast/third/qpc/ports/qv/gnu

SRCS += vast/component/vast_cli.c
#SRCS += vast/component/vast_log.c
#SRCS += vast/component/vast_store.c
#SRCS += vast/component/vast_simulatite_eeprom.c
SRCS += vast/component/protocol.c
SRCS += vast/component/modbus_rtu.c
SRCS += vast/component/modbus.c
SRCS += vast/component/state_machine.c

SRCS += vast/core/croutine.c
SRCS += vast/core/device.c
SRCS += vast/core/heap.c
SRCS += vast/core/kfifo.c
SRCS += vast/core/object.c
SRCS += vast/core/softtimer.c
SRCS += vast/core/utils.c
#SRCS += vast/core/alloter.c

SRCS += vast/device/ina219.c
SRCS += vast/device/vast_key.c
SRCS += vast/device/vast_ir_JVC.c
SRCS += vast/device/vast_ir_MI.c
SRCS += vast/device/vast_ir_NEC.c
SRCS += vast/device/vast_ir_PANASONIC.c
SRCS += vast/device/vast_ir_RC5.c
SRCS += vast/device/vast_ir_RC6.c
SRCS += vast/device/vast_ir_RCA.c
SRCS += vast/device/vast_ir_SAMSUNG.c
SRCS += vast/device/vast_ir_SONY.c
SRCS += vast/device/vast_ir_TOSHIBA.c
SRCS += vast/device/vast_ir.c
SRCS += vast/device/vast_led.c
SRCS += vast/device/cp8102.c
SRCS += vast/device/cp8113.c
SRCS += vast/device/im1227.c
#SRCS += vast/device/ssr_v220.c

SRCS += vast/driver/i2c/i2c_core.c
SRCS += vast/driver/i2c/i2c_dev.c
SRCS += vast/driver/i2c/i2c-bit-ops.c
SRCS += vast/driver/modbus_rtu.c
SRCS += vast/driver/vast_serial.c

SRCS += vast/third/cjson/cJSON.c
SRCS += vast/third/qpc/src/
SRCS += vast/third/qpc/src/qf/qep_hsm.c
SRCS += vast/third/qpc/src/qf/qep_msm.c
SRCS += vast/third/qpc/src/qf/qf_act.c
SRCS += vast/third/qpc/src/qf/qf_actq.c
SRCS += vast/third/qpc/src/qf/qf_defer.c
SRCS += vast/third/qpc/src/qf/qf_dyn.c
SRCS += vast/third/qpc/src/qf/qf_mem.c
SRCS += vast/third/qpc/src/qf/qf_ps.c
SRCS += vast/third/qpc/src/qf/qf_qact.c
SRCS += vast/third/qpc/src/qf/qf_qeq.c
SRCS += vast/third/qpc/src/qf/qf_qmact.c
SRCS += vast/third/qpc/src/qf/qf_time.c
SRCS += vast/third/qpc/src/qv/qv.c

INCS += -I./vast/third/CmBacktrace/cm_backtrace
SRCS += vast/third/CmBacktrace/cm_backtrace/cm_backtrace.c
SRCS += vast/third/CmBacktrace/cm_backtrace/fault_handler/gcc/cmb_fault.s

INCS += -I./vast/third/EasyLogger/easylogger/inc
SRCS += vast/third/EasyLogger/easylogger/src/elog.c
SRCS += vast/third/EasyLogger/easylogger/src/elog_async.c
SRCS += vast/third/EasyLogger/easylogger/src/elog_buf.c
SRCS += vast/third/EasyLogger/easylogger/src/elog_utils.c

INCS += -I./vast/third/EasyFlash/easyflash/inc
SRCS_DIRS += vast/third/EasyFlash/easyflash/src

#SRCS += vast/vast_third/qpc/ports/qv/gnu/qf_port.c
SRCS += vast/third/qpc/ports/qv/gnu/qv_port.c

ifeq (spy, $(CONF))
	SRCS += src/qs/qs.c
	SRCS += src/qs/qs_rx.c
	SRCS += src/qs/qs_fp.c
	SRCS += src/qs/qs_64bit.c
endif


################ third/modbus ################
INCS += -I./vast/third/FreeModbus/modbus/include
INCS += -I./vast/third/FreeModbus/modbus/rtu
INCS += -I./vast/third/FreeModbus/modbus/ascii
INCS += -I./vast/third/FreeModbus/port

SRCS += vast/third/FreeModbus/modbus/functions/mbfuncdiag.c
SRCS += vast/third/FreeModbus/modbus/functions/mbutils.c
SRCS += vast/third/FreeModbus/modbus/functions/mbfuncother.c
SRCS += vast/third/FreeModbus/modbus/rtu/mbcrc.c
SRCS += vast/third/FreeModbus/modbus/functions/mbfunccoils_m.c
SRCS += vast/third/FreeModbus/modbus/functions/mbfuncdisc_m.c
SRCS += vast/third/FreeModbus/modbus/functions/mbfuncholding_m.c
SRCS += vast/third/FreeModbus/modbus/functions/mbfuncinput_m.c
SRCS += vast/third/FreeModbus/modbus/rtu/mbrtu_m.c
SRCS += vast/third/FreeModbus/modbus/mb_m.c
SRCS += vast/third/FreeModbus/port/port.c
SRCS += vast/third/FreeModbus/port/portevent_m.c
SRCS += vast/third/FreeModbus/port/portserial_m.c
SRCS += vast/third/FreeModbus/port/porttimer_m.c
SRCS += vast/third/FreeModbus/port/user_mb_app_m.c


