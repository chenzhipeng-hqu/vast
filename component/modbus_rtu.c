/**
 ******************************************************************************
 * @file    demo.c
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

/***********************************************
                    include
***********************************************/
#include "math.h"
#include "modbus_rtu.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/***********************************************
                   function
***********************************************/

/**
  * @brief  writeModBus function.
  * @param  
  * @retval 
  */
int ModBus_Init(ModbusRtu_HandleTypeDef *pModbusRtu)
{	
	return 0;
}

//������תΪ����
uint16_t float2int(float value)
{
	uint16_t res = 0;
	float bufValue;
	int i = 0;
	
	if (value == 0)
	{
		return 0;
	}
	
	for (i = 0; i < 9; i++)
	{
		bufValue = value * pow(10,i);
		if (fmod(bufValue,1) == 0)
		{
			res = i * 10000 + bufValue;
			return res;
		}
	}
	
	return 0;
}

//����תΪ������
float int2float(uint16_t value)
{
	float res;
	uint16_t bufValue;
	
	bufValue = value / 10000;
	res = (value % 10000) / pow(10,bufValue);
	return res;
}

/**
  * @brief  crc16 function.
  * @param  
  * @retval 
  */
static uint16_t crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }
    return (crc_hi << 8 | crc_lo);
}

/**
  * @brief  writeModBus function.
  * @param  
  * @retval 
  */
static void writeModBus(ModbusRtu_HandleTypeDef *pModbusRtu, uint8_t cmd, uint8_t *pData, uint16_t dataLen)
{
	uint8_t TxCommand[MODBUS_RTU_BUFF_LEN] = {0};
	uint8_t *p = pData;
	uint16_t crc = 0;
	uint8_t crc_H = 0, crc_L = 0;
	
	pModbusRtu->opcode = cmd;

	TxCommand[0] = pModbusRtu->slave_addr;
	TxCommand[1] = cmd;
	for (int i = 0; i < dataLen; i++)
	{
		TxCommand[i + 2] = *p;
		p++;
	}
	crc = crc16((uint8_t *)TxCommand, dataLen + 2);
	crc_H = crc >> 8;
	crc_L = crc & 0x00ff;
	TxCommand[2 + dataLen] = crc_H;
	TxCommand[3 + dataLen] = crc_L;

//	uint8_t *TxData = TxCommand;
//	for (int i = 0; i < dataLen + 4; i++)
//	{
//		COMPutCharB(port, *TxData); 
//		TxData++;
//	}
	
	pModbusRtu->sendBuffFunc(TxCommand, dataLen+ 4);  // slave_addr, cmd, crc_L, crc_H
}

bool checkModBusData(ModbusRtu_HandleTypeDef *pModbusRtu, uint8_t *data, uint16_t len)
{
	uint16_t crc = 0;

	if ((data[0] == pModbusRtu->slave_addr) | (data[0] == 0)) {

	} else {
		return false;
	}

	if (data[1] != pModbusRtu->opcode) {
		return false;
	}

	crc = crc16((uint8_t *)data, len);

	return (crc == 0);
}

/**
  * @brief  //�����ּĴ���(���) function.
  * @param  
  * @retval 
  */
bool readHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr, uint8_t reg_count)
{
	uint8_t TxData[4] = {0};
//	uint8_t RegReadAckBytes[MODBUS_RTU_BUFF_LEN] = {0};
//	uint16_t value, size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	if (reg_count > MAX_HOLDRES)
	{
		return false;
	}
	
	TxData[0] = start_addr >> 8;
	TxData[1] = start_addr & 0x00ff;
	TxData[2] = reg_count >> 8;
	TxData[3] = reg_count & 0x00ff;
	
	writeModBus(pModbusRtu, READ_HOLD_RES, TxData, 4);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,(reg_count * 2 + 5),&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	
//	if (RegReadAckBytes[0] == pModbusRtu->slave_addr && RegReadAckBytes[1] == READ_HOLD_RES 
//		&& RegReadAckBytes[2] == ( reg_count * 2) && size == (reg_count * 2 + 5))
//	{
//		for(int i = 3; i < (reg_count * 2 + 3); i+=2)
//		{
//			value = (RegReadAckBytes[i] & 0x00ff);
//			value = value << 8;
//			value = value | (RegReadAckBytes[i+1] & 0x00ff);
//			*reg_value = value;
//			reg_value++;
//		}
//		
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

/**
  * @brief  //�����ּĴ���(����)
  * @param  
  * @retval 
  */
bool readSignalHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr)
{
	uint8_t TxData[4] = {0};
//	uint8_t RegReadAckBytes[READ_SINGLE_HOLD_RES_NUM] = {0};
//	uint16_t value,size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	
	TxData[0] = start_addr >> 8;
	TxData[1] = start_addr & 0x00ff;
	TxData[2] = 0x00;
	TxData[3] = 0x01;
	
	writeModBus(pModbusRtu, READ_HOLD_RES, TxData, 4);
	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,READ_SINGLE_HOLD_RES_NUM,&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	
//	if (RegReadAckBytes[0] == slave_addr && RegReadAckBytes[1] == READ_HOLD_RES 
//		&& RegReadAckBytes[2] == ( nRegCnt * 2) && size == READ_SINGLE_HOLD_RES_NUM)
//	{
//		value = (RegReadAckBytes[3] & 0x00ff);
//		value = value << 8;
//		value = value | (RegReadAckBytes[4] & 0x00ff);
//		*reg_value = value;
//		
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

/**
  * @brief  д�������ּĴ���
  * @param  
  * @retval 
  */
bool writeSingleHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr, uint16_t reg_value)
{
	uint8_t TxData[4] = {0};
//	uint8_t RegReadAckBytes[WRITE_SINGLE_HOLD_RES_NUM] = {0};
//	uint16_t size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	
	TxData[0] = reg_addr >> 8;
	TxData[1] = reg_addr & 0x00ff;
	TxData[2] = reg_value >> 8;
	TxData[3] = reg_value & 0x00ff;
	
	writeModBus(pModbusRtu, WRITE_SINGLE_HOLD_RES, TxData, 4);
	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,WRITE_SINGLE_HOLD_RES_NUM,&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);

//	if (RegReadAckBytes[0] == slave_addr && RegReadAckBytes[1] == WRITE_SINGLE_HOLD_RES 
//		&& RegReadAckBytes[2] == TxData[0] && RegReadAckBytes[3] == TxData[1] 
//		&& size == WRITE_SINGLE_HOLD_RES_NUM)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

/**
  * @brief  д������ּĴ���
  * @param  
  * @retval 
  */
bool writeMultiHoldReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t start_addr, uint16_t reg_count, uint16_t *pData)
{
	uint8_t TxData[MAX_HOLDRES * 2 + 5] = {0};
	uint8_t dataLen = (reg_count * 2) & 0x00ff;
//	uint8_t RegReadAckBytes[MODBUS_RTU_BUFF_LEN] = {0};
//	uint16_t size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	if (reg_count > MAX_HOLDRES)
	{
		return false;
	}
	
	TxData[0] = start_addr >> 8;
	TxData[1] = start_addr & 0x00ff;
	TxData[2] = reg_count >> 8;
	TxData[3] = reg_count & 0x00ff;
	TxData[4] = dataLen;
	
	uint16_t * p = pData;
	for (int i = 0; i < dataLen; i += 2)
	{
		TxData[i + 5] = (*p) >> 8;
		TxData[i + 6] = (*p) & 0x00ff;
		p++;
	}
	
	writeModBus(pModbusRtu, WRITE_MULTI_HOLD_RES, TxData, reg_count * 2 + 5);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,WRITE_MUTLI_HOLD_RES_NUM,&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	
//	if (RegReadAckBytes[0] == slave_addr && RegReadAckBytes[1] == WRITE_MULTI_HOLD_RES 
//		&& RegReadAckBytes[2] == TxData[0] && RegReadAckBytes[3] == TxData[1]
//		&& size == WRITE_MUTLI_HOLD_RES_NUM)
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

/**
  * @brief  ������Ĵ���(���)
  * @param  
  * @retval 
  */
bool readInputReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr, uint16_t reg_count)
{
	uint8_t TxData[4] = {0};
//	uint8_t RegReadAckBytes[MODBUS_RTU_BUFF_LEN] = {0};
//	uint16_t value,size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	if (reg_count > MAX_HOLDRES)
	{
		return false;
	}
	
	TxData[0] = reg_addr >> 8;
	TxData[1] = reg_addr & 0x00ff;
	TxData[2] = reg_count >> 8;
	TxData[3] = reg_count & 0x00ff;
	
	writeModBus(pModbusRtu, READ_INPUT_RES, TxData, 4);
	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,(reg_count * 2 + 5),&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	
//	if (RegReadAckBytes[0] == slave_addr && RegReadAckBytes[1] == READ_INPUT_RES 
//		&& RegReadAckBytes[2] == ( reg_count * 2) && size == (reg_count * 2 + 5))
//	{
//		for(int i = 3; i < (reg_count * 2 + 3); i+=2)
//		{
//			value = (RegReadAckBytes[i] & 0x00ff);
//			value = value << 8;
//			value = value | (RegReadAckBytes[i+1] & 0x00ff);
//			*reg_value = value;
//			reg_value++;
//		}
//		
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

/**
  * @brief  ������Ĵ���(����)
  * @param  
  * @retval 
  */
bool readSingleInputReg(ModbusRtu_HandleTypeDef *pModbusRtu, uint16_t reg_addr)
{
	uint8_t TxData[4] = {0};
//	uint8_t RegReadAckBytes[READ_SINGLE_INPUT_RES_NUM] = {0};
//	uint16_t value,size;
	
	if (pModbusRtu->slave_addr > MAX_SLAVE_ADDR)
	{
		return false;
	}
	
	TxData[0] = reg_addr >> 8;
	TxData[1] = reg_addr & 0x00ff;
	TxData[2] = 0x00;
	TxData[3] = 0x01;
	
	writeModBus(pModbusRtu, READ_INPUT_RES, TxData, 4);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	UART_GetBytes(port,RegReadAckBytes,READ_SINGLE_INPUT_RES_NUM,&size);
//	//delay_ms(100);
//	vTaskDelay(ELOAD_WRITE_READ_DELAY);
//	
//	if (RegReadAckBytes[0] == slave_addr && RegReadAckBytes[1] == READ_INPUT_RES 
//		&& RegReadAckBytes[2] == ( nRegCnt * 2) && size == READ_SINGLE_INPUT_RES_NUM)
//	{
//		value = (RegReadAckBytes[3] & 0x00ff);
//		value = value << 8;
//		value = value | (RegReadAckBytes[4] & 0x00ff);
//		*reg_value = value;
//		
//		return true;
//	}
//	else
//	{
//		return false;
//	}
	return true;
}

	
/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



