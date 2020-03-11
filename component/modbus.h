//################################################################################
//# Copyright Statement: CVTE
//# Copyright (C) 2014 Guangzhou Shiyuan Electronics Co.,Ltd. All rights reserved.
//#      ____________        _______________  ___________
//#     / / ________ \      / / _____   ____|| |  _______|
//#    / / /      \ \ \    / / /   | | |     | | |
//#   | | |        \ \ \  / / /    | | |     | | |_______
//#   | | |         \ \ \/ / /     | | |     | |  _______|
//#   | | |          \ \ \/ /      | | |     | | |
//#    \ \ \______    \ \  /       | | |     | | |_______
//#     \_\_______|    \_\/        |_|_|     |_|_________|
//#
//################################################################################

/**
 ******************************************************************************
 * @file    demo.h
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-sep-2018
 * @brief   
 ******************************************************************************
 * @attention
 * 
 ******************************************************************************
 */
    
/** @addtogroup Project
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODBUS_H__
#define __MODBUS_H__

/***********************************************
                    include
***********************************************/
#include <stdint.h>
#include <stdbool.h>
#include <core/device.h>
#include "core/croutine.h"
#include "component/state_machine.h"

/***********************************************
                    define
***********************************************/
#ifdef configUSING_MODBUS

#define	MODBUS_RTU_BUFF_LEN				0x7F
#define MAX_SLAVE_ADDR					0xFF
#define	MAX_HOLDRES						64

#define READ_HOLD_RES					0x03
#define WRITE_MULTI_HOLD_RES			0x10
#define WRITE_SINGLE_HOLD_RES			0x06
#define READ_INPUT_RES					0x04

#define	 MODBUS_RX_MAX					64
#define	 MODBUS_TX_MAX					512

/***********************************************
                    typedef
***********************************************/
//struct modbus_msg
//{
	//uint16_t slave_addr;
	//uint16_t opcode;
	//uint16_t start_addr;
	//uint16_t reg_cnt;
	//uint16_t  *buf;
//};

#pragma pack(1)
typedef struct ModbusFrame
{
	uint8_t slave_addr;
	uint8_t opcode;
	union {
		uint16_t reg_addr;
		uint16_t start_addr;
	};
	union {
		uint16_t reg_cnt;
		uint16_t reg_val;
	};
	uint16_t data[1];
} modbus_frame_t;
#define MODBUS_FRAME_HEAD (offset_of(modbus_frame_t, data))

typedef struct SmartFrame
{
	uint8_t stc;
	rx_indicate rx_ind;
	tx_complete tx_done;
    uint32_t from;
    uint32_t to;
    list_t 	entry;
	uint8_t len;
	uint8_t data[1];
} smart_frame_t;
#define SMART_FRAME_HEAD (offset_of(smart_frame_t, data))

#pragma pack()

typedef struct modbus_device modbus_t;

typedef struct _modbus_ops_t
{
    error_t (*init)             (struct modbus_device *modbus);
	error_t	(*tx_prepare)		(struct modbus_device *modbus, void *buffer);
	error_t	(*tx_done)		    (struct modbus_device *modbus, void *buffer);
	error_t	(*rx_indicate)		(struct modbus_device *modbus, size_t size);
    error_t(*ctrl)              (struct modbus_device *modbus, uint8_t cmd, void *args);
}modbus_ops_t;

struct modbus_device
{
	device_t parent;
	//uint16_t slave_addr;
	uint8_t rx_data[MODBUS_RX_MAX];
//	uint8_t tx_data[MODBUS_TX_MAX];
//    DECLARE_KFIFO(rx_kfifo, unsigned char, MODBUS_RX_MAX);
//    DECLARE_KFIFO(tx_kfifo, unsigned char, MODBUS_TX_MAX);
    list_t 	tx_list;
    state_machine_t sm;
 	const modbus_ops_t 	*ops;
};

/***********************************************
               function prototypes
***********************************************/
error_t modbus_device_register(modbus_t *modbus, const char *name, uint32_t flag, void *data);
int readInputReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t start_addr, uint16_t reg_cnt, rx_indicate rx_ind);
int writeMultiHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data, rx_indicate rx_ind);
int writeSingleHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_val, rx_indicate rx_ind);
//int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt);
int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t start_addr, uint16_t reg_cnt, rx_indicate rx_ind);

extern error_t read_input_reg_rx_ind(device_t *dev, size_t size);
extern error_t read_hold_reg_rx_ind(device_t *dev, size_t size);
extern error_t write_hold_reg_rx_ind(device_t *dev, size_t size);
extern error_t write_holds_reg_rx_ind(device_t *dev, size_t size);

/***********************************************
					inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#endif
/**
  * @}
  */

#endif  /* __MODBUS_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



