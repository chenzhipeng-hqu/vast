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
#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

/***********************************************
                    include
***********************************************/
#include <stdint.h>
#include <stdbool.h>

/***********************************************
                    define
***********************************************/
#define	MODBUS_RTU_BUFF_LEN				0x7F
#define MAX_SLAVE_ADDR						0xFF
#define	MAX_HOLDRES								64

#define READ_HOLD_RES							0x03
#define WRITE_MULTI_HOLD_RES			0x10
#define WRITE_SINGLE_HOLD_RES			0x06
#define READ_INPUT_RES						0x04

#define READ_SINGLE_HOLD_RES_NUM		7
#define READ_SINGLE_INPUT_RES_NUM		7
#define WRITE_SINGLE_HOLD_RES_NUM		8
#define WRITE_MUTLI_HOLD_RES_NUM		8

/***********************************************
                    typedef
***********************************************/
struct modbus_msg
{
	uint16_t slave_addr;
	uint16_t opcode;
	uint16_t start_addr;
	uint16_t reg_cnt;
	uint16_t  *buf;
};

typedef struct
{
	uint8_t slave_addr;
	uint8_t opcode;
	int (*sendBuffFunc)(uint8_t *pBuff, uint8_t len);
}ModbusRtu_HandleTypeDef;

/***********************************************
               function prototypes
***********************************************/
bool readSingleInputReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr);
bool readInputReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr, uint16_t reg_count);
bool writeMultiHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr, uint16_t reg_count, uint16_t *pData);
bool writeSingleHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr, uint16_t reg_value);
bool readSignalHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr);
bool readHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr, uint8_t reg_count);
bool checkModBusData(ModbusRtu_HandleTypeDef *pModbusRtu, uint8_t *data, uint16_t len);

uint16_t float2int(float value);
float int2float(uint16_t value);

/***********************************************
					inline
***********************************************/

/***********************************************
                   variable
***********************************************/

/**
  * @}
  */

#endif  /* __MODBUS_RTU_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


