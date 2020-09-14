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

export VAST_ROOT ?=

# include ../application/vast_config.mk

DEFS += -DVAST_MODULE_ENABLED

################ vast ################
INCS += -I./$(VAST_ROOT)
#INCS += -I./$(VAST_ROOT)/core
#INCS += -I./$(VAST_ROOT)/driver
#INCS += -I./$(VAST_ROOT)/device
#INCS += -I./$(VAST_ROOT)/component
ifeq (true, $(VAST_USING_OBSERIVER))
	DEFS += -DVAST_USING_OBSERIVER
	SRCS += $(VAST_ROOT)/component/observer.c
endif

ifeq (true, $(VAST_USING_SHELL))
	DEFS += -DVAST_USING_SHELL
	SRCS += $(VAST_ROOT)/component/vast_shell.c
endif
#SRCS += $(VAST_ROOT)/component/$(VAST_ROOT)_log.c
#SRCS += $(VAST_ROOT)/component/vast_store.c
#SRCS += $(VAST_ROOT)/component/vast_simulatite_eeprom.c
ifeq (true, $(VAST_USING_PROTOCOL))
	DEFS += -DVAST_USING_PROTOCOL
	SRCS += $(VAST_ROOT)/component/protocol.c
endif

ifeq (true, $(VAST_USING_MODBUS))
	DEFS += -DVAST_USING_MODBUS
	#SRCS += vast/component/modbus_rtu.c
	SRCS += $(VAST_ROOT)/component/modbus.c
	#SRCS += $(VAST_ROOT)/driver/modbus_rtu.c

endif

ifeq (true, $(VAST_USING_SM))
	DEFS += -DVAST_USING_SM
	SRCS += $(VAST_ROOT)/component/state_machine.c
endif

ifeq (true, $(VAST_USING_CORE))
	DEFS += -DVAST_USING_CORE
	SRCS += $(VAST_ROOT)/core/croutine.c
	SRCS += $(VAST_ROOT)/core/device.c
	#SRCS += $(VAST_ROOT)/core/heap.c
	SRCS += $(VAST_ROOT)/core/memheap.c
	SRCS += $(VAST_ROOT)/core/kfifo.c
	SRCS += $(VAST_ROOT)/core/object.c
	SRCS += $(VAST_ROOT)/core/softtimer.c
	SRCS += $(VAST_ROOT)/core/utils.c
endif
#SRCS += $(VAST_ROOT)/core/alloter.c

ifeq (true, $(VAST_USING_INA219))
	DEFS += -DVAST_USING_INA219
	SRCS += $(VAST_ROOT)/device/ina219.c
endif

ifeq (true, $(VAST_USING_KEY))
	DEFS += -DVAST_USING_KEY
	SRCS += $(VAST_ROOT)/device/vast_key.c
endif

ifeq (true, $(VAST_USING_IR))
	DEFS += -DVAST_USING_IR
	SRCS += $(VAST_ROOT)/device/vast_ir_JVC.c
	SRCS += $(VAST_ROOT)/device/vast_ir_MI.c
	SRCS += $(VAST_ROOT)/device/vast_ir_NEC.c
	SRCS += $(VAST_ROOT)/device/vast_ir_PANASONIC.c
	SRCS += $(VAST_ROOT)/device/vast_ir_RC5.c
	SRCS += $(VAST_ROOT)/device/vast_ir_RC6.c
	SRCS += $(VAST_ROOT)/device/vast_ir_RCA.c
	SRCS += $(VAST_ROOT)/device/vast_ir_SAMSUNG.c
	SRCS += $(VAST_ROOT)/device/vast_ir_SONY.c
	SRCS += $(VAST_ROOT)/device/vast_ir_TOSHIBA.c
	SRCS += $(VAST_ROOT)/device/vast_ir.c
endif

ifeq (true, $(VAST_USING_LED))
	DEFS += -DVAST_USING_LED
	SRCS += $(VAST_ROOT)/device/vast_led.c
endif

ifeq (true, $(VAST_USING_CP8102))
	DEFS += -DVAST_USING_CP8102
	SRCS += $(VAST_ROOT)/device/cp8102.c
endif
ifeq (true, $(VAST_USING_CP8113))
	DEFS += -DVAST_USING_CP8113
	SRCS += $(VAST_ROOT)/device/cp8113.c
endif
ifeq (true, $(VAST_USING_IM1227))
	DEFS += -DVAST_USING_IM1227
	SRCS += $(VAST_ROOT)/device/im1227.c
endif
#SRCS += $(VAST_ROOT)/device/ssr_v220.c

ifeq (true, $(VAST_USING_I2C))
	DEFS += -DVAST_USING_I2C
	SRCS += $(VAST_ROOT)/driver/i2c/i2c_core.c
	SRCS += $(VAST_ROOT)/driver/i2c/i2c_dev.c
	SRCS += $(VAST_ROOT)/driver/i2c/i2c-bit-ops.c
endif

ifeq (true, $(VAST_USING_SERIAL))
	DEFS += -DVAST_USING_SERIAL
	SRCS += $(VAST_ROOT)/driver/vast_serial.c
endif

################ third/json ################
ifeq (true, $(VAST_USING_JSON))
	DEFS += -DVAST_USING_JSON
	INCS += -I./$(VAST_ROOT)/third/cjson
	SRCS += $(VAST_ROOT)/third/cjson/cJSON.c
endif

################ third/qpc ################
ifeq (true, $(VAST_USING_QPC))
	DEFS += -DVAST_USING_QPC

	INCS += -I./$(VAST_ROOT)/third/qpc/include
	INCS += -I./$(VAST_ROOT)/third/qpc/src
	INCS += -I./$(VAST_ROOT)/third/qpc/ports/qv/gnu

	SRCS += $(VAST_ROOT)/third/qpc/src/
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qep_hsm.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qep_msm.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_act.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_actq.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_defer.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_dyn.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_mem.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_ps.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_qact.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_qeq.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_qmact.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qf/qf_time.c
	SRCS += $(VAST_ROOT)/third/qpc/src/qv/qv.c
	#SRCS += $(VAST_ROOT)/vast_third/qpc/ports/qv/gnu/qf_port.c
	SRCS += $(VAST_ROOT)/third/qpc/ports/qv/gnu/qv_port.c
endif

ifeq (true, $(VAST_USING_QPC_SPY))
	DEFS += -DVAST_USING_QPC_SPY
	SRCS += src/qs/qs.c
	SRCS += src/qs/qs_rx.c
	SRCS += src/qs/qs_fp.c
	SRCS += src/qs/qs_64bit.c
endif

ifeq (true, $(VAST_USING_CMBACKTRACE))
	DEFS += -DVAST_USING_CMBACKTRACE
	INCS += -I./$(VAST_ROOT)/third/CmBacktrace/cm_backtrace
	SRCS += $(VAST_ROOT)/third/CmBacktrace/cm_backtrace/cm_backtrace.c
	SRCS += $(VAST_ROOT)/third/CmBacktrace/cm_backtrace/fault_handler/gcc/cmb_fault.s
endif

ifeq (true, $(VAST_USING_EASYLOG))
	DEFS += -DVAST_USING_EASYLOG

	INCS += -I./$(VAST_ROOT)/third/EasyLogger/easylogger/inc

	SRCS += $(VAST_ROOT)/third/EasyLogger/easylogger/src/elog.c
	SRCS += $(VAST_ROOT)/third/EasyLogger/easylogger/src/elog_async.c
	SRCS += $(VAST_ROOT)/third/EasyLogger/easylogger/src/elog_buf.c
	SRCS += $(VAST_ROOT)/third/EasyLogger/easylogger/src/elog_utils.c
endif

ifeq (true, $(VAST_USING_EASYFLASH))
	DEFS += -DVAST_USING_EASYFLASH

	INCS += -I./$(VAST_ROOT)/third/EasyFlash/easyflash/inc

	SRCS_DIRS += $(VAST_ROOT)/third/EasyFlash/easyflash/src
endif


################ third/modbus ################
ifeq (true, $(VAST_USING_FREEMODBUS))
	DEFS += -DVAST_USING_FREEMODBUS

	INCS += -I./$(VAST_ROOT)/third/FreeModbus/modbus/include
	INCS += -I./$(VAST_ROOT)/third/FreeModbus/modbus/rtu
	INCS += -I./$(VAST_ROOT)/third/FreeModbus/modbus/ascii
	INCS += -I./$(VAST_ROOT)/third/FreeModbus/port

	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfuncdiag.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbutils.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfuncother.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/rtu/mbcrc.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfunccoils_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfuncdisc_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfuncholding_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/functions/mbfuncinput_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/rtu/mbrtu_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/modbus/mb_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/port/port.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/port/portevent_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/port/portserial_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/port/porttimer_m.c
	SRCS += $(VAST_ROOT)/third/FreeModbus/port/user_mb_app_m.c
endif

################ third/utest ################
ifeq (true, $(VAST_USING_UTEST))
	DEFS += -DVAST_USING_UTEST

	INCS += -I./$(VAST_ROOT)/third/utest

	SRCS += $(VAST_ROOT)/third/utest/utest.c
endif

################ third/lwip ################
ifeq (true, $(VAST_USING_LWIP))
	DEFS += -DVAST_USING_LWIP

	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/netif/ppp
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/posix
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/posix/sys
	INCS += -I./$(VAST_ROOT)/third/lwip/system/arch
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/lwip
	INCS += -I./$(VAST_ROOT)/third/lwip/system
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/lwip/priv
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/lwip/prot
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/lwip/apps
	INCS += -I./$(VAST_ROOT)/third/lwip/src/include/netif

	SRCS += $(VAST_ROOT)/third/lwip/src/api/api_lib.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/api_msg.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/err.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/netbuf.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/netdb.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/netifapi.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/sockets.c
	SRCS += $(VAST_ROOT)/third/lwip/src/api/tcpip.c
	SRCS += $(VAST_ROOT)/third/lwip/src/apps/mqtt/mqtt.c
	SRCS += $(VAST_ROOT)/third/lwip/src/apps/tftp/tftp_server.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/def.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/dns.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/inet_chksum.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/init.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ip.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/autoip.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/dhcp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/etharp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/icmp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/igmp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/ip4.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/ip4_addr.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv4/ip4_frag.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/dhcp6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/ethip6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/icmp6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/inet6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/ip6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/ip6_addr.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/ip6_frag.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/mld6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/ipv6/nd6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/mem.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/memp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/netif.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/pbuf.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/raw.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/stats.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/sys.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/tcp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/tcp_in.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/tcp_out.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/timeouts.c
	SRCS += $(VAST_ROOT)/third/lwip/src/core/udp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ethernet.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/lowpan6.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/auth.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/ccp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/chap-md5.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/chap-new.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/chap_ms.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/demand.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/eap.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/ecp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/eui64.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/fsm.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/ipcp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/ipv6cp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/lcp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/magic.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/mppe.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/multilink.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/ppp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/pppapi.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/pppcrypt.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/pppoe.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/pppol2tp.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/pppos.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/upap.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/utils.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/ppp/vj.c
	SRCS += $(VAST_ROOT)/third/lwip/src/netif/slipif.c

endif



