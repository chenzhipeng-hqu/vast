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
#include "vast_cli.h"
#include "vast_mem.h"
#include "gpio.h"
#include "vast_simulatite_eeprom.h"
#include "vast_ir.h"
#include "vast_ring_flash.h"
#include <string.h>

/*************************************
              define
*************************************/

/**************************************
              typedef
**************************************/

/*************************************
         function prototypes
*************************************/
static void CLICmd_ResetCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_DebugCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_MemCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);

/*************************************
              variable
*************************************/
extern const CLICmdTypedef CLICmd_GpioCtrl[];
extern const CLICmdTypedef cliCmdTableE2prom[];
extern const CLICmdTypedef cliCmdTableReg[];
extern const CLICmdTypedef CLICmd_IRCtrl[];
extern const CLICmdTypedef CLICmd_StoreCtrl[];

const CLICmdTypedef CLI_CmdTableMain[] =
{
  {"cd"		, "goto dir", CLICmd_GotoTree, 0},
  {"reg"	, "reg dir", 0, cliCmdTableReg},
  {"gpio"	, "gpio dir", 0, CLICmd_GpioCtrl},
  {"reset"	, "reset {iwdg/soft}", CLICmd_ResetCtrl, 0},
  {"debug"	, "debug (all/board/eth/atsx/232/485) 0~5", CLICmd_DebugCtrl, 0},
  {"mem"	, "mem {print/malloc/free} [size]", CLICmd_MemCtrl, 0},
  {"eep"	, "e2prom dir", 0, cliCmdTableE2prom},
  {"ir"		, "ir dir", 0, CLICmd_IRCtrl},
  {"store"	, "store dir", 0, CLICmd_StoreCtrl},

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

uint8_t *p[10] = {NULL};
/**
  * @brief  CLICmd_DebugCtrl
  * @param
  * @retval
  */
void CLICmd_MemCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	if(!stricmp(argv[1], "print"))
	{
		vast_getmemtablestatus();
	}
	else if(!stricmp(argv[1], "malloc"))
	{
		uint8_t i = str2u32(argv[2]);
		uint32_t size = str2u32(argv[3]);

		if(p[i] == NULL)
		{
			p[i] = vast_malloc(size);
		}
		else
		{
			printf("p[%d]:%p, p[%d]:%d\r\n", i, p[i], i, *p[i]);
		}
	}
	else if(!stricmp(argv[1], "free"))
	{
		uint8_t i = str2u32(argv[2]);

		if(p[i] == NULL)
		{
			printf("p[%d]:%p, p[%d]:%d\r\n", i, p[i], i, *p[i]);
		}
		else
		{
			vast_free(p[i]);
			p[i] = NULL;
		}

	}
}

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void CLICmd_ResetCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  uint32_t ts;
  uint32_t cnt;
  if(argc == 2)
  {
		if(!stricmp(argv[1], "iwdg"))
		{
			cnt = 0;
			ts = HAL_GetTick();
			LL_IWDG_ReloadCounter(IWDG);
			do
			{
				while(HAL_GetTick() - ts < cnt);
				printf("(%d) ", (unsigned int)(HAL_GetTick() - ts));
				cnt += 10;
			}while(1);
		}
		else if(!stricmp(argv[1], "soft"))
		{
			NVIC_SystemReset();
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


/**
  * @brief  CLICmd_GpioCtrl
  * @param
  * @retval
  */
const CLICmdTypedef cliCmdTableReg[] =
{
	{"rb"		, "read reg:rb 0xAddr len(Byte)", cliCmdReadRegByte, 0},
	{"wb"		, "write reg:wb 0xAddr 0xVal(Byte)", cliCmdWriteRegByte, 0},
	{"r"		, "read reg:r 0xAddr len(Word)", cliCmdReadReg, 0},
	{"w"		, "write reg:w 0xAddr 0xVal(Word)", cliCmdWriteReg, 0},
	{0, 0, 0, 0}
};
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

/**
  * @brief  CLICmd_GpioCtrl
  * @param
  * @retval
  */
void cliCmdE2promDispVirt(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint16_t VarData = 0;

	for(uint16_t i=0; i<NB_OF_VAR; i++)
	{
		VarData = 0;

		if((EE_ReadVariable(VirtTab[i].addr,  &VarData)) != HAL_OK)
		{
//			printf("EE_ReadVariable (%s) error\r\n", VirtTab[i].help);
		}

		pCli->Init.Write("VirtTab[%d]: data:0x%04x, %s\r\n", i, VarData, VirtTab[i].help);

	}
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param
  * @retval
  */
void cliCmdE2promReadByte(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint16_t VarData = 0;
  uint16_t virtId = str2u32(argv[1]);

	if((EE_ReadVariable(VirtTab[virtId].addr,  &VarData)) != HAL_OK)
	{
		pCli->Init.Write("EE_ReadVariable (%s) error\r\n", VirtTab[virtId].help);
	}
	else
	{
		pCli->Init.Write("R: data:0x%04x, %s\r\n", VarData, VirtTab[virtId].help);
	}
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param
  * @retval
  */
void cliCmdE2promWriteByte(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint16_t VarData = 0;
  uint16_t virtId = str2u32(argv[1]);
  uint16_t writeData = str2u32(argv[2]);

  if((EE_ReadVariable(VirtTab[virtId].addr,  &VarData)) != HAL_OK)
	{
	  pCli->Init.Write("EE_ReadVariable (%s) error\r\n", VirtTab[virtId].help);
	}

	if(VarData != writeData)
	{
		if(EE_WriteVariable(VirtTab[virtId].addr, writeData) != HAL_OK)
		{
			pCli->Init.Write("EE_WriteVariable (%s) error\r\n", VirtTab[virtId].help);
		}
		else
		{
			pCli->Init.Write("W: %s: 0x%x\r\n", VirtTab[virtId].help, writeData);
		}
	}
	else
	{
		pCli->Init.Write("W: 0x%x: %s\r\n", VarData, VirtTab[virtId].help);
	}
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param
  * @retval
  */
const CLICmdTypedef cliCmdTableE2prom[] =
{
  {"?", "display virtAddr", cliCmdE2promDispVirt, 0},
  {"r", "read virtId", cliCmdE2promReadByte, 0},
  {"w", "write virtId data", cliCmdE2promWriteByte, 0},
  {0, 0, 0, 0}
};

//------------------------------------------------------------------------------
//-   custom command functions. ir
//------------------------------------------------------------------------------
void cliCmdIrSetup(CLI_HandleTypeDef *pCli,int argc, char *argv[])
{
  uint8_t protocal;

  if(argc > 0)
  {
    protocal = str2u32(argv[1]);
    InfraRed_RX_ChangeProtocol(protocal);
    pCli->Init.Write("set ir %d\n", protocal);
  }
}

void cliCmdIrSend(CLI_HandleTypeDef *pCli,int argc, char *argv[])
{
  uint16_t head;
  uint16_t code;
  uint16_t check;

  if(argc > 3)
  {
    head = hex2u32(argv[1]);
    code = hex2u32(argv[2]);
    check = hex2u32(argv[3]);
   // io_irSend(head, code, check);
    pCli->Init.Write("send ir 0x%04X, 0x%X, 0x%X\n", head, code, check);
  }
}

/**
  * @brief  CLICmd_IRCtrl
  * @param
  * @retval
  */
const CLICmdTypedef CLICmd_IRCtrl[] =
{
  {"s", "s protocal(1:NEC/2:RC5/3:RC6/4:RCA/7:SONY/11:MI/14:SAMSUNG/15:PANASONIC) Exp:s 1", cliCmdIrSetup, 0},
//  {"t", "t head code check", cliCmdIrSend, 0},
	// last command must be all 0s.
  {0, 0, 0, 0}
};


//------------------------------------------------------------------------------
//-   custom command functions. store
//------------------------------------------------------------------------------
/**
  * @brief  cliCmdStoreWrite
  * @param
  * @retval
  */
void cliCmdStoreWrite(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint8_t ret = VAST_OK;
	uint32_t len = 0;
	uint8_t *p = NULL;

	printf("t1:%ld\r\n", HAL_GetTick());
	if(argc > 2)
	{
		len = str2u32(argv[2]);
		p = (uint8_t *)vast_malloc(len);

		if(p != NULL)
		{
			memset((void *)p, str2u32(argv[1]), len);

			ret = vast_ring_flash_store((uint8_t *)p, len);

			if(ret != VAST_OK)
			{
				pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
			}

			vast_free(p);
		}
		else
		{
			printf("p == NULL\r\n");
		}

	}
	else if(argc > 1)
	{
		ret = vast_ring_flash_store((uint8_t *)argv[1], strlen(argv[1]));

		if(ret != VAST_OK)
		{
			pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
		}
	}
	printf("t2:%ld\r\n", HAL_GetTick());
}

/**
  * @brief  cliCmdStoreRead
  * @param
  * @retval
  */

void cliCmdStoreRead(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint8_t ret = VAST_OK;
	uint16_t read_len = 0;
	static uint8_t buff_array2[10240] = {0};

	read_len = vast_ring_flash_read(buff_array2, sizeof(buff_array2));
	buff_array2[read_len] = 0;
	DBG_LOG(DBG_INFO, "read from flash [%d][%s]", read_len, (char *)buff_array2);
	DBG_ARRAY(DBG_DEBUG, "->(", "\b)\r\n", (char *)buff_array2, read_len);

	if(ret != VAST_OK)
	{
		pCli->Init.Write("vast_ring_flash_store ret=%d\r\n", ret);
	}
}

/**
  * @brief  cliCmdStoreRead
  * @param
  * @retval
  */

void cliCmdStoreInfo(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint32_t lastAddr = vast_ring_flash_get_last_addr();
	uint32_t nextAddr = vast_ring_flash_get_next_addr();
	pCli->Init.Write("lastAddr=%#lx, nextAddr=%#lx, len=%d\r\n", lastAddr, nextAddr, nextAddr-lastAddr);
}

/**
  * @brief  cliCmdStoreRead
  * @param
  * @retval
  */

void cliCmdStorePrint(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	vast_ring_flash_print();
}

/**
  * @brief  CLICmd_StoreCtrl
  * @param
  * @retval
  */
const CLICmdTypedef CLICmd_StoreCtrl[] =
{
	{"i", "i:info", cliCmdStoreInfo, 0},
	{"p", "p:print", cliCmdStorePrint, 0},
	{"w", "w data [size]", cliCmdStoreWrite, 0},
	{"r", "r", cliCmdStoreRead, 0},
	// last command must be all 0s.
	{0, 0, 0, 0}
};

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

	
