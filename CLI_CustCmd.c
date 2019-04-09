/**
  ******************************************************************************
  * @file    CLI_Command.c
  * @author  chenzhipeng
  * @version V1.0.0
  * @date    23-July-2018
  * @brief   UsartTask
  ******************************************************************************
  * @attention
	*  CLI: command-line interface
  ******************************************************************************
  */
	
/** @addtogroup Template_Project
  * @{
  */ 
/*************************************
              include
*************************************/
#include <string.h>
#include <stdint.h>

#include "gpio.h"
#include "qpc_common.h"

#include "CLI.h"

/*************************************
              define
*************************************/

/**************************************
              typedef
**************************************/

/*************************************
         function prototypes
*************************************/
static void CLICmd_DebugCtrl(int argc, char *argv[]);
static void CLICmd_IwdgCtrl(int argc, char *argv[]);
static void cliCmdReadRegByte(int argc, char *argv[]);
static void cliCmdWriteRegByte(int argc, char *argv[]);
static void cliCmdReadReg(int argc, char *argv[]);
static void cliCmdWriteReg(int argc, char *argv[]);
static void cliCmdEcho(int argc, char *argv[]);

/*************************************
              variable
*************************************/
extern const CLICmdTypedef CLICmd_GpioCtrl[];
extern const CLICmdTypedef CLICmd_TableI2c[];

const CLICmdTypedef CLI_CmdTableMain[] =
{
	{"cd"		, "goto dir", CLICmd_GotoTree, 0},
  {"rb"		, "read reg:rb 0xAddr len(Byte)", cliCmdReadRegByte, 0},
  {"wb"		, "write reg:wb 0xAddr 0xVal(Byte)", cliCmdWriteRegByte, 0},
  {"r"		, "read reg:r 0xAddr len(Word)", cliCmdReadReg, 0},
  {"w"		, "write reg:w 0xAddr 0xVal(Word)", cliCmdWriteReg, 0},
  {"iwdg"	, "iwdg test", CLICmd_IwdgCtrl, 0},
	{"echo"	, "echo enable/disable", cliCmdEcho, 0}, 
  {"gpio"	, "gpio dir", 0, CLICmd_GpioCtrl},
  {"i2c"	, "i2c dir", 0, CLICmd_TableI2c},
  {"debug", "debug {all/led} 0~5", CLICmd_DebugCtrl, 0},

  // last command must be all 0s.
  {0, 0, 0, 0}
};


/*************************************
              function
*************************************/
//------------------------------------------------------------------------------
//-   common command functions.
//------------------------------------------------------------------------------
/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void CLICmd_IwdgCtrl(int argc, char *argv[])
{
  uint32_t ts;
  uint32_t cnt;
  if(argc == 2)
  {
    if(!strcmp(argv[1], "test"))
    {
      cnt = 0;
      ts = HAL_GetTick();
      do
      {
        while(HAL_GetTick() - ts < cnt);
        CLI_PutString("(%d) ", (unsigned int)(HAL_GetTick() - ts));
        cnt += 5;
      }while(1);
    }
  }
}

void cliCmdReadRegByte(int argc, char *argv[])
{
  volatile uint8_t *addr;
  uint32_t len = 1;
  uint32_t idx;
  if(argc > 1)
  {
    addr = (uint8_t *)hex2u32(argv[1]);
    if(argc > 2)
    {
      len = str2u32(argv[2]);
      if(len < 1)
      {
        len = 1;
      }
    }

    for(idx = 0; idx < len; idx++)
    {
      if((idx & 0x07) == 0)
      {
        CLI_PutString("\r\n[0x%08X]", addr + idx);
      }
      CLI_PutString(" 0x%02X", *(addr + idx));
    }
    CLI_PutString("\r\n");
  }
}

void cliCmdWriteRegByte(int argc, char *argv[])
{
  volatile uint8_t *addr;
  uint8_t value;
  if(argc > 2)
  {
    addr = (uint8_t *)hex2u32(argv[1]);
    value = hex2u32(argv[2]);

    *addr = value;
    CLI_PutString("[0x%08X] = 0x%02X\r\n", addr, *addr);
  }
}

void cliCmdReadReg(int argc, char *argv[])
{
  volatile uint32_t *addr;
  uint32_t len = 1;
  uint32_t idx;
  if(argc > 1)
  {
    addr = (uint32_t *)(hex2u32(argv[1]) & 0xfffffffc);
    if(argc > 2)
    {
      len = str2u32(argv[2]);
      if(len < 1)
      {
        len = 1;
      }
    }

    for(idx = 0; idx < len; idx++)
    {
      if((idx & 0x03) == 0)
      {
        CLI_PutString("\r\n[0x%08X]", addr + idx);
      }
      CLI_PutString(" 0x%08X", *(addr + idx));
    }
    CLI_PutString("\r\n");
  }
}

void cliCmdWriteReg(int argc, char *argv[])
{
  volatile uint32_t *addr;
  uint32_t value;
  if(argc > 2)
  {
    addr = (uint32_t *)(hex2u32(argv[1]) & 0xfffffffc);
    value = hex2u32(argv[2]);

    *addr = value;
    CLI_PutString("[0x%08X] = 0x%08X\r\n", addr, *addr);
  }
}
//------------------------------------------------------------------------------
//-   end common command functions.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//-   custom command functions. GPIO
//------------------------------------------------------------------------------
//pio (a-k) (0-f) (i/oo/op/fo/fp/a) (n/u/d) [0-16]
void cliCmdGpioFun(int argc, char *argv[])
{
  uint32_t index;
  GPIO_InitTypeDef GPIO_InitStruct;
  uint8_t altFun = 0;
  char gpio;
  char mode;
  char pullType;
  char outType;
  
  if(argc > 4)
  {
    gpio = argv[1][0];
    index = hex2u32(argv[2]);
    mode = argv[3][0];
    outType = argv[3][1];
    pullType = argv[4][0];
    if(argc > 5)
    {
      altFun = str2u32(argv[5]);
    }

    if(gpio >= 'a')
    {
      gpio = gpio - 'a';
    }
    else
    {
      gpio = gpio - 'A';
    }

    //GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Pin = 1<<index;
    switch(mode)
    {
    case 'i':
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      break;
    case 'o':
      if(outType == 'p')
      {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
      }
      else
      {
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
      }
      break;
    case 'f':
      if(outType == 'p')
      {
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      }
      else
      {
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      }
      break;
    case 'a':
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      break;
    }
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    switch(pullType)
    {
    case 'n':
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      break;
    case 'u':
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      break;
    case 'd':
      GPIO_InitStruct.Pull = GPIO_PULLDOWN;
      break;
    }
    GPIO_InitStruct.Alternate = altFun;
    
    HAL_GPIO_Init((GPIO_TypeDef *)(GPIOA_BASE+(0x0400*gpio)), &GPIO_InitStruct);
    CLI_PutString("GPIO-%c-%d set.\r\n", gpio+'A', (unsigned int)index);
  }
}

//p (a-k) (0-f) [1/0]
void cliCmdGpioPort(int argc, char *argv[])
{
  uint16_t index;
  uint8_t value;
  char gpio;
  
  if(argc > 2)
  {
    gpio = argv[1][0];
    index = hex2u32(argv[2]);
    if(argc > 3)
    {
      value = argv[3][0] - '0';
    }
    else
    {
      value = 0xff;
    }

    if(gpio >= 'a')
    {
      gpio = gpio - 'a';
    }
    else
    {
      gpio = gpio - 'A';
    }

    if(index > 15)
    {
      index = 15;
    }
    
    if(argc > 3)
    {
      HAL_GPIO_WritePin((GPIO_TypeDef *)(GPIOA_BASE+(0x0400*gpio)), 1<<index, (GPIO_PinState)(value));
    }
    else
    {
      value = HAL_GPIO_ReadPin((GPIO_TypeDef *)(GPIOA_BASE+(0x0400*gpio)), 1<<index);
    }
    CLI_PutString("GPIO-%c-%d = %d.\r\n", gpio+'A', index, value);
  }
}

void cliCmdEcho(int argc, char *argv[])
{
	if(argc > 1)
	{
		if(!strcmp(argv[1], "enable"))
		{
			echoEnable = 1;
		}
		else if(!strcmp(argv[1], "disable"))
		{
			echoEnable = 0;
		}
	}
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param  
  * @retval 
  */
const CLICmdTypedef CLICmd_GpioCtrl[] =
{
  {"pio", "pio (a-k) (0-f) (i/oo/op/fo/fp/a) (n/u/d) [0-16]", cliCmdGpioFun, 0},
  {"p", "set/get port:p (a-k) (0-f) [1/0]", cliCmdGpioPort, 0},
  // last command must be all 0s.
  {0, 0, 0, 0}
};


//------------------------------------------------------------------------------
//-   end custom command functions. GPIO
//------------------------------------------------------------------------------

/**
  * @brief  CLICmd_DebugCtrl
  * @param  
  * @retval 
  */
void CLICmd_DebugCtrl(int argc, char *argv[])
{
	if(!strcmp(argv[1], "all"))
	{
		extern int dbgLevel;
		dbgLevel = str2u32(argv[2]);			
	}
	else
	{
		CLI_PutString("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
	}
}

/**
  * @brief  cliCmdI2c
  * @param  
  * @retval 
  */
void cliCmdI2cReadByte(int argc, char *argv[])
{
	uint8_t addr;
	uint8_t value[16] = {0}, reg = 0, len = 0;
	
  if(argc > 1)
  {
		addr = str2u32(argv[1]);
		
		reg = str2u32(argv[2]);
		
		len = str2u32(argv[3]);
		
		extern I2C_HandleTypeDef hi2c3;
		
		printf("\r\n");
		
		HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c3, addr, reg, 1, value, len, 100);
		if(HAL_OK != ret)
		{
			printf("err: val=0x%02x, ret=%d\r\n", value[0], ret);
		}
		printf("read: addr:0x%02x\r\n", addr);
		
		for(uint8_t i=0; i<len; i++)
		{
			printf("\treg:0x%02x, value:0x%02x\r\n", reg+i, value[i]);
		}
  }
}

void cliCmdI2cWriteByte(int argc, char *argv[])
{
	uint8_t addr;
	uint8_t value = 0, reg = 0;
	
  if(argc > 1)
  {
    addr = str2u32(argv[1]);
		
		reg = str2u32(argv[2]);
		
		value = str2u32(argv[3]);
		
		extern I2C_HandleTypeDef hi2c3;
		
		printf("\r\n");
		
		HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c3, addr, reg, 1, &value, 1, 100);
		if(HAL_OK != ret)
		{
			printf("err: val=0x%02x, ret=%d\r\n", value, ret);
		}
				
		printf("\twrite: addr:0x%02x, reg:0x%02x, value:0x%02x\r\n", addr, reg, value);
  }
}

const CLICmdTypedef CLICmd_TableI2c[] = 
{
  {"r", "read addr reg [cnt]", cliCmdI2cReadByte, 0},
  {"w", "write addr reg d0 [d1...]", cliCmdI2cWriteByte, 0},
  {0, 0, 0, 0}
};

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

	
