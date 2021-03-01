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

export VANCA_ROOT ?=

# include ../application/vast_config.mk

DEFS += -DVANCA_MODULE_ENABLED

################### vast ###################
INCS += -I$(VANCA_ROOT)
#INCS += -I./$(VANCA_ROOT)/core
#INCS += -I./$(VANCA_ROOT)/driver
#INCS += -I./$(VANCA_ROOT)/device
#INCS += -I./$(VANCA_ROOT)/component

#$(error $(VANCA_SYS_CLOCK))
#ifeq ($(VANCA_SYS_CLOCK), )
ifndef $(VANCA_SYS_CLOCK)
	#$(error 2222222222222)
	DEFS += -DconfigHZ=$(VANCA_SYS_CLOCK)
endif
#$(error 3333333333333)

################### core ###################
ifeq (true, $(VANCA_USING_CORE))
	DEFS += -DVANCA_USING_CORE
	SRCS += $(VANCA_ROOT)/core/croutine.c
	SRCS += $(VANCA_ROOT)/core/device.c
	#SRCS += $(VANCA_ROOT)/core/heap.c
	SRCS += $(VANCA_ROOT)/core/memheap.c
	SRCS += $(VANCA_ROOT)/core/kfifo.c
	SRCS += $(VANCA_ROOT)/core/object.c
	SRCS += $(VANCA_ROOT)/core/softtimer.c
	SRCS += $(VANCA_ROOT)/core/utils.c
	#SRCS += $(VANCA_ROOT)/core/alloter.c
endif

################## driver ##################
ifeq (true, $(VANCA_USING_I2C))
	DEFS += -DVANCA_USING_I2C
	SRCS += $(VANCA_ROOT)/driver/i2c/i2c_core.c
	SRCS += $(VANCA_ROOT)/driver/i2c/i2c_dev.c
	SRCS += $(VANCA_ROOT)/driver/i2c/i2c-bit-ops.c
endif

ifeq (true, $(VANCA_USING_SERIAL_DMA))
	DEFS += -DVANCA_USING_SERIAL_DMA
endif

ifeq (true, $(VANCA_USING_SERIAL))
	DEFS += -DVANCA_USING_SERIAL
	SRCS += $(VANCA_ROOT)/driver/serial.c
endif

################## device ##################
ifeq (true, $(VANCA_USING_INA219))
	DEFS += -DVANCA_USING_INA219
	SRCS += $(VANCA_ROOT)/device/ina219.c
endif

ifeq (true, $(VANCA_USING_KEY))
	DEFS += -DVANCA_USING_KEY
	SRCS += $(VANCA_ROOT)/device/dev_key.c
endif

ifeq (true, $(VANCA_USING_IR))
	DEFS += -DVANCA_USING_IR
	SRCS += $(VANCA_ROOT)/device/dev_ir_KONKA.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_JVC.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_MI.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_NEC.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_PANASONIC.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_RC5.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_RC6.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_RCA.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_SAMSUNG.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_SONY.c
	SRCS += $(VANCA_ROOT)/device/dev_ir_TOSHIBA.c
	SRCS += $(VANCA_ROOT)/device/dev_ir.c
	SRCS += $(VANCA_ROOT)/driver/drv_infrared.c
endif

ifeq (true, $(VANCA_USING_LED))
	DEFS += -DVANCA_USING_LED
	SRCS += $(VANCA_ROOT)/driver/drv_led.c
endif

ifeq (true, $(VANCA_USING_ADC))
	DEFS += -DVANCA_USING_ADC
	SRCS += $(VANCA_ROOT)/driver/drv_adc.c
endif

ifeq (true, $(VANCA_USING_CP8102))
	DEFS += -DVANCA_USING_CP8102
	SRCS += $(VANCA_ROOT)/device/cp8102.c
endif
ifeq (true, $(VANCA_USING_CP8113))
	DEFS += -DVANCA_USING_CP8113
	SRCS += $(VANCA_ROOT)/device/cp8113.c
endif
ifeq (true, $(VANCA_USING_IM1227))
	DEFS += -DVANCA_USING_IM1227
	SRCS += $(VANCA_ROOT)/device/im1227.c
endif
ifeq (true, $(VANCA_USING_IM1275))
	DEFS += -DVANCA_USING_IM1275
	SRCS += $(VANCA_ROOT)/device/im1275.c
endif
#SRCS += $(VANCA_ROOT)/device/ssr_v220.c

################# component ################
ifeq (true, $(VANCA_USING_OBSERIVER))
	DEFS += -DVANCA_USING_OBSERIVER
	SRCS += $(VANCA_ROOT)/component/observer.c
endif

ifeq (true, $(VANCA_USING_SHELL))
	DEFS += -DVANCA_USING_SHELL
	SRCS += $(VANCA_ROOT)/component/shell.c
endif

ifeq (true, $(VANCA_USING_PROTOCOL))
	DEFS += -DVANCA_USING_PROTOCOL
	SRCS += $(VANCA_ROOT)/component/protocol.c
endif

ifeq (true, $(VANCA_USING_SMARTFRAME))
	DEFS += -DVANCA_USING_SMARTFRAME
	SRCS += $(VANCA_ROOT)/component/smartframe.c
endif

ifeq (true, $(VANCA_USING_MESSAGE))
	DEFS += -DVANCA_USING_MESSAGE
	SRCS += $(VANCA_ROOT)/component/message.c
endif

ifeq (true, $(VANCA_USING_EVENT))
	DEFS += -DVANCA_USING_EVENT
	SRCS += $(VANCA_ROOT)/component/event.c
endif

ifeq (true, $(VANCA_USING_MODBUS))
	DEFS += -DVANCA_USING_MODBUS
	#SRCS += vast/component/modbus_rtu.c
	SRCS += $(VANCA_ROOT)/component/modbus.c
	#SRCS += $(VANCA_ROOT)/driver/modbus_rtu.c

endif

ifeq (true, $(VANCA_USING_SM))
	DEFS += -DVANCA_USING_SM
	SRCS += $(VANCA_ROOT)/component/state_machine.c
endif

################### third ##################
################ third/json ################
ifeq (true, $(VANCA_USING_JSON))
	DEFS += -DVANCA_USING_JSON
	INCS += -I$(VANCA_ROOT)/third/cjson
	SRCS += $(VANCA_ROOT)/third/cjson/cJSON.c
endif

################ third/qpc ################
ifeq (true, $(VANCA_USING_QPC))
	DEFS += -DVANCA_USING_QPC

	INCS += -I$(VANCA_ROOT)/third/qpc/include
	INCS += -I$(VANCA_ROOT)/third/qpc/src
	INCS += -I$(VANCA_ROOT)/third/qpc/ports/qv/gnu

	SRCS += $(VANCA_ROOT)/third/qpc/src/
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qep_hsm.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qep_msm.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_act.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_actq.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_defer.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_dyn.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_mem.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_ps.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_qact.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_qeq.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_qmact.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qf/qf_time.c
	SRCS += $(VANCA_ROOT)/third/qpc/src/qv/qv.c
	#SRCS += $(VANCA_ROOT)/vast_third/qpc/ports/qv/gnu/qf_port.c
	SRCS += $(VANCA_ROOT)/third/qpc/ports/qv/gnu/qv_port.c
endif

ifeq (true, $(VANCA_USING_QPC_SPY))
	DEFS += -DVANCA_USING_QPC_SPY
	SRCS += src/qs/qs.c
	SRCS += src/qs/qs_rx.c
	SRCS += src/qs/qs_fp.c
	SRCS += src/qs/qs_64bit.c
endif

ifeq (true, $(VANCA_USING_QPN))
	DEFS += -DVANCA_USING_QPN
	INCS += -I$(VANCA_ROOT)/third/qpn/include
	SRCS += $(VANCA_ROOT)/third/qpn/src/qfn/qepn.c
	SRCS += $(VANCA_ROOT)/third/qpn/src/qfn/qfn.c
	SRCS += $(VANCA_ROOT)/third/qpn/src/qvn/qvn.c
endif

################ third/CmBacktrace ################
ifeq (true, $(VANCA_USING_CMBACKTRACE))
	DEFS += -DVANCA_USING_CMBACKTRACE
	INCS += -I$(VANCA_ROOT)/third/CmBacktrace/cm_backtrace
	SRCS += $(VANCA_ROOT)/third/CmBacktrace/cm_backtrace/cm_backtrace.c
	SRCS += $(VANCA_ROOT)/third/CmBacktrace/cm_backtrace/fault_handler/gcc/cmb_fault.s
endif

################ third/EasyLogger ################
ifeq (true, $(VANCA_USING_EASYLOG))
	DEFS += -DVANCA_USING_EASYLOG

	INCS += -I$(VANCA_ROOT)/third/EasyLogger/easylogger/inc

	SRCS += $(VANCA_ROOT)/third/EasyLogger/easylogger/src/elog.c
	SRCS += $(VANCA_ROOT)/third/EasyLogger/easylogger/src/elog_async.c
	SRCS += $(VANCA_ROOT)/third/EasyLogger/easylogger/src/elog_buf.c
	SRCS += $(VANCA_ROOT)/third/EasyLogger/easylogger/src/elog_utils.c
endif

################ third/EasyFlash ################
ifeq (true, $(VANCA_USING_EASYFLASH))
	DEFS += -DVANCA_USING_EASYFLASH

	INCS += -I$(VANCA_ROOT)/third/EasyFlash/easyflash/inc

	SRCS_DIRS += $(VANCA_ROOT)/third/EasyFlash/easyflash/src
endif

################ third/modbus ################
ifeq (true, $(VANCA_USING_FREEMODBUS))
	DEFS += -DVANCA_USING_FREEMODBUS

	INCS += -I$(VANCA_ROOT)/third/FreeModbus/modbus/include
	INCS += -I$(VANCA_ROOT)/third/FreeModbus/modbus/rtu
	INCS += -I$(VANCA_ROOT)/third/FreeModbus/modbus/ascii
	INCS += -I$(VANCA_ROOT)/third/FreeModbus/port

	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfuncdiag.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbutils.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfuncother.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/rtu/mbcrc.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfunccoils_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfuncdisc_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfuncholding_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/functions/mbfuncinput_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/rtu/mbrtu_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/modbus/mb_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/port/port.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/port/portevent_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/port/portserial_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/port/porttimer_m.c
	SRCS += $(VANCA_ROOT)/third/FreeModbus/port/user_mb_app_m.c
endif

################ third/utest ################
ifeq (true, $(VANCA_USING_UTEST))
	DEFS += -DVANCA_USING_UTEST

	INCS += -I$(VANCA_ROOT)/third/utest

	SRCS += $(VANCA_ROOT)/third/utest/utest.c
endif

################ third/lwip ################
ifeq (true, $(VANCA_USING_LWIP))
	DEFS += -DVANCA_USING_LWIP

	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/netif/ppp
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/posix
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/posix/sys
	INCS += -I$(VANCA_ROOT)/third/lwip/system/arch
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/lwip
	INCS += -I$(VANCA_ROOT)/third/lwip/system
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/lwip/priv
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/lwip/prot
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/lwip/apps
	INCS += -I$(VANCA_ROOT)/third/lwip/src/include/netif

	SRCS += $(VANCA_ROOT)/third/lwip/src/api/api_lib.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/api_msg.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/err.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/netbuf.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/netdb.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/netifapi.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/sockets.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/api/tcpip.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/apps/mqtt/mqtt.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/apps/tftp/tftp_server.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/def.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/dns.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/inet_chksum.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/init.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ip.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/autoip.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/dhcp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/etharp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/icmp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/igmp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/ip4.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/ip4_addr.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv4/ip4_frag.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/dhcp6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/ethip6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/icmp6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/inet6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/ip6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/ip6_addr.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/ip6_frag.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/mld6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/ipv6/nd6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/mem.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/memp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/netif.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/pbuf.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/raw.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/stats.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/sys.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/tcp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/tcp_in.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/tcp_out.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/timeouts.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/core/udp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ethernet.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/lowpan6.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/auth.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/ccp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/chap-md5.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/chap-new.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/chap_ms.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/demand.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/eap.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/ecp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/eui64.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/fsm.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/ipcp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/ipv6cp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/lcp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/magic.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/mppe.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/multilink.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/ppp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/pppapi.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/pppcrypt.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/pppoe.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/pppol2tp.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/pppos.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/upap.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/utils.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/ppp/vj.c
	SRCS += $(VANCA_ROOT)/third/lwip/src/netif/slipif.c

endif



