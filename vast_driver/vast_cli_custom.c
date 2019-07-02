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
#include <vast_cli.h>
#include "gpio.h"


/*************************************
              define
*************************************/

/**************************************
              typedef
**************************************/

/*************************************
         function prototypes
*************************************/
static void CLICmd_DebugCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_IwdgCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void cliCmdReadRegByte(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void cliCmdWriteRegByte(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void cliCmdReadReg(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void cliCmdWriteReg(CLI_HandleTypeDef *pCli, int argc, char *argv[]);

/*************************************
              variable
*************************************/
extern const CLICmdTypedef CLICmd_GpioCtrl[];
extern const CLICmdTypedef CLICmd_ModbusCtrl[];
extern const CLICmdTypedef CLICmd_ProtocolCtrl[];

const CLICmdTypedef CLI_CmdTableMain[] =
{
  {"cd"		, "goto dir", CLICmd_GotoTree, 0},
  {"rb"		, "read reg:rb 0xAddr len(Byte)", cliCmdReadRegByte, 0},
  {"wb"		, "write reg:wb 0xAddr 0xVal(Byte)", cliCmdWriteRegByte, 0},
  {"r"		, "read reg:r 0xAddr len(Word)", cliCmdReadReg, 0},
  {"w"		, "write reg:w 0xAddr 0xVal(Word)", cliCmdWriteReg, 0},
  {"gpio"	, "gpio dir", 0, CLICmd_GpioCtrl},
  {"iwdg"	, "iwdg test", CLICmd_IwdgCtrl, 0},
  {"debug", "debug (all/board/eth/atsx/232/485) 0~5", CLICmd_DebugCtrl, 0},

  // last command must be all 0s.
  {0, 0, 0, 0}
};


/*************************************
              function
*************************************/
/**
  * @brief  CLICmd_DebugCtrl
  * @param  
  * @retval 
  */
void CLICmd_DebugCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
}
	

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void CLICmd_IwdgCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
        pCli->Init.Write("(%d) ", (unsigned int)(HAL_GetTick() - ts));
        cnt += 5;
      }while(1);
    }
  }
}

//------------------------------------------------------------------------------
//-   common command functions.
//------------------------------------------------------------------------------
void cliCmdReadRegByte(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
        pCli->Init.Write("\r\n[0x%08X]", addr + idx);
      }
      pCli->Init.Write(" 0x%02X", *(addr + idx));
    }
    pCli->Init.Write("\r\n");
  }
}

void cliCmdWriteRegByte(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  volatile uint8_t *addr;
  uint8_t value;
  if(argc > 2)
  {
    addr = (uint8_t *)hex2u32(argv[1]);
    value = hex2u32(argv[2]);

    *addr = value;
    pCli->Init.Write("[0x%08X] = 0x%02X\r\n", addr, *addr);
  }
}

void cliCmdReadReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
        pCli->Init.Write("\r\n[0x%08X]", addr + idx);
      }
      pCli->Init.Write(" 0x%08X", *(addr + idx));
    }
    pCli->Init.Write("\r\n");
  }
}

void cliCmdWriteReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  volatile uint32_t *addr;
  uint32_t value;
  if(argc > 2)
  {
    addr = (uint32_t *)(hex2u32(argv[1]) & 0xfffffffc);
    value = hex2u32(argv[2]);

    *addr = value;
    pCli->Init.Write("[0x%08X] = 0x%08X\r\n", addr, *addr);
  }
}

//------------------------------------------------------------------------------
//-   custom command functions. GPIO
//------------------------------------------------------------------------------
//pio (a-k) (0-f) (i/oo/op/fo/fp/a) (n/u/d) [0-16]
void cliCmdGpioFun(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
    pCli->Init.Write("GPIO-%c-%d set.\r\n", gpio+'A', (unsigned int)index);
  }
}

//p (a-k) (0-f) [1/0]
void cliCmdGpioPort(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
    pCli->Init.Write("GPIO-%c-%d = %d.\r\n", gpio+'A', index, value);
  }
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param  
  * @retval 
  */
const CLICmdTypedef CLICmd_GpioCtrl[] =
{
  {"pio", "pin set in/out{}/altfun{}<>/analog none/pullUp/pullDown funNum {openDrain/pushPull}:\r\n\tpio (a-k) (0-f) (i/oo/op/fo/fp/a) (n/u/d) [0-16]", cliCmdGpioFun, 0},
  {"p", "set/get port:p (a-k) (0-f) [1/0]", cliCmdGpioPort, 0},
  // last command must be all 0s.
  {0, 0, 0, 0}
};

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

	
