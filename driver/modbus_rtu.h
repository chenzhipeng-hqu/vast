/**
 ******************************************************************************
 * @file    xxx.h
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/** @addtogroup Project
  * @{
  */

/* Define to prevent recursive inclusion */
#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "core/croutine.h"
#include <core/device.h>

/***********************************************
                    define
***********************************************/
#ifdef configUSING_MODBUS_RTU

#ifdef MODBUS_DEBUG
#define modbus_dbg(fmt, ...)   printk(fmt, ##__VA_ARGS__)
#else
#define modbus_dbg(fmt, ...)
#endif

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
	int (*sendBuffFunc)(device_t *dev, off_t off, uint8_t *pBuff, uint8_t len);
	device_t *dev;
}ModbusRtu_HandleTypeDef;

struct modbus_device;

struct modbus_device_ops
{
 size_t (*master_xfer)(struct modbus_device *bus,
					   struct modbus_msg msgs[],
					   uint32_t num);
 size_t (*slave_xfer)(struct modbus_device *bus,
					  struct modbus_msg msgs[],
					  uint32_t num);
 error_t(*modbus_control)(struct modbus_device *bus,
						 uint32_t,
						 uint32_t);
};

struct modbus_device
{
	struct device parent;
	const struct modbus_device_ops *ops;
	uint16_t  flags;
	uint16_t  addr;
	uint32_t  timeout;
	uint32_t  retries;
	void *priv;
	ModbusRtu_HandleTypeDef modbus;
	tcb_t tcb;
};

/***********************************************
               function prototypes
***********************************************/
error_t  modbus_device_register(struct modbus_device *bus, const char *bus_name, void *data);
struct modbus_device *modbus_device_find(const char *bus_name);
int    modbus_core_init(void);
int modbus_Transmit_Start_Callback(device_t *bus);
int modbus_Transmit_Complete_Callback(device_t *bus);
int modbus_Receive_Callback(struct modbus_device *dev, uint8_t *data, uint16_t len);
uint16_t float2int(float value);
float int2float(uint16_t value);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#endif /*configUSING_xxx*/

#ifdef __cplusplus
}
#endif

#endif /* __MODBUS_RTU_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


