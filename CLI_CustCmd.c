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
#include "protocol.h"
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
static void CLICmd_DebugCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_IwdgCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_Print(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_AtsxCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_BaudCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
static void CLICmd_IpCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
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
  {"baud"	, "baud (485IN/485EX/232A/232B) baudrate", CLICmd_BaudCtrl, 0},
  {"print", "print (V5) (on/off)", CLICmd_Print, 0},
	{"ip"		, "ip set/get addr0 addr1 addr2 addr3", CLICmd_IpCtrl, 0},
  {"modbus", "cd modbus", 0, CLICmd_ModbusCtrl},
  {"atsx"	, "cd atsx", 0, CLICmd_ProtocolCtrl},

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
#if 1
	if(!strcmp(argv[1], "all"))
	{
		extern int dbgLevel;
		dbgLevel = str2u32(argv[2]);	
		
		DebugLevelEvt pe = {{DEBUG_LEVEL_SIG, 0U, 0U}, 0U};
		pe.dbg_level = dbgLevel;	
		
		QACTIVE_POST(AO_Board[0], &pe.super, me);
		QACTIVE_POST(AO_Board[1], &pe.super, me);	
		QACTIVE_POST(AO_Ethernet, &pe.super, me);	
		QACTIVE_POST(AO_Atsx		, &pe.super, me);
		QACTIVE_POST(AO_RS485[RS485_IN], &pe.super, me);	
		QACTIVE_POST(AO_RS485[RS485_EX], &pe.super, me);	
		QACTIVE_POST(AO_RS232[RS232_A], &pe.super, me);	
		QACTIVE_POST(AO_RS232[RS232_B], &pe.super, me);	
		
//		DebugLevelEvt * const pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
//		pe->dbg_level = dbgLevel;	
//		pe->super.refCtr_ = 5;
//		
//		QACTIVE_POST(AO_Board[0], &pe->super, me);
//		QACTIVE_POST(AO_Board[1], &pe->super, me);	
//		QACTIVE_POST(AO_Ethernet, &pe->super, me);	
//		QACTIVE_POST(AO_Atsx		, &pe->super, me);
//		QACTIVE_POST(AO_RS485		, &pe->super, me);		
	}
	else if(!strcmp(argv[1], "board"))
	{	
		DebugLevelEvt pe = {{DEBUG_LEVEL_SIG, 0U, 0U}, 0U};
		pe.dbg_level = str2u32(argv[2]);	
		QACTIVE_POST(AO_Board[BOARD_A], &pe.super, me);
		QACTIVE_POST(AO_Board[BOARD_B], &pe.super, me);	
		
//		DebugLevelEvt * pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);			
//		pe->dbg_level = str2u32(argv[2]);	
//		pe->super.refCtr_ = BOARD_MAX;
//		QACTIVE_POST(AO_Board[BOARD_A], &pe->super, me);	
//		QACTIVE_POST(AO_Board[BOARD_B], &pe->super, me);
	}
	else if(!strcmp(argv[1], "eth"))
	{
		DebugLevelEvt * const pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
		pe->dbg_level = str2u32(argv[2]);	
		QACTIVE_POST(AO_Ethernet, &pe->super, me);	
	}
	else if(!strcmp(argv[1], "atsx"))
	{
		DebugLevelEvt * const pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
		pe->dbg_level = str2u32(argv[2]);	
		QACTIVE_POST(AO_Atsx, &pe->super, me);
	}
	else if(!strcmp(argv[1], "232"))
	{
		DebugLevelEvt * const pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
		pe->dbg_level = str2u32(argv[2]);	
		pe->super.refCtr_ = RS232_MAX;
		QACTIVE_POST(AO_RS232[RS232_A], &pe->super, me);
		QACTIVE_POST(AO_RS232[RS232_B], &pe->super, me);
	}
	else if(!strcmp(argv[1], "485"))
	{
		DebugLevelEvt * const pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
		pe->dbg_level = str2u32(argv[2]);	
		QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
	}
	else
	{
		pCli->Init.Write("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
	}
#else
	
	DebugLevelEvt * pe = Q_NEW(DebugLevelEvt, DEBUG_LEVEL_SIG);		
	
	pe->dbg_level = str2u32(argv[2]);	
	
	if(!strcmp(argv[1], "board"))
	{	
//		DebugLevelEvt pe = {{DEBUG_LEVEL_SIG, 0U, 0U}, 0U};
//		pe.dbg_level = str2u32(argv[2]);	
//		QACTIVE_POST(AO_Board[0], &pe.super, me);
//		QACTIVE_POST(AO_Board[1], &pe.super, me);	
			
		CLI_PutString("1.sig=%d, poolId_=%d, refCtr_=%d \r\n", pe->super.sig, pe->super.poolId_, pe->super.refCtr_);
		QACTIVE_POST(AO_Board[BOARD_A], &pe->super, me);	
		CLI_PutString("2.sig=%d, poolId_=%d, refCtr_=%d \r\n", pe->super.sig, pe->super.poolId_, pe->super.refCtr_);
		QACTIVE_POST(AO_Board[BOARD_B], &pe->super, me);
		CLI_PutString("3.sig=%d, poolId_=%d, refCtr_=%d \r\n", pe->super.sig, pe->super.poolId_, pe->super.refCtr_);
		
	}
	else
	{
		pCli->Init.Write("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
	}
#endif
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

/**
  * @brief  CLICmd_Print
  * @param  
  * @retval 
  */
void CLICmd_AtsxCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	ProtMsg_TypeDef ProtMsg = {0};
	
  if(argc >= 3)
  {
		ProtMsg.boardId = argv[1][0] == 'B' ? 2 : 1;

		ProtMsg.cmdId = str2u32(argv[2]);
		
//		if(argv[3][1] == 'x' || argv[3][1] == 'X')
//		{
//			ProtMsg.data[0] = str2u32(argv[3]);
//		}
//		else
//		{				
//			strcpy((char *)ProtMsg.data, argv[3]);
//		}
		
		for(uint8_t i=3; i<argc; i++)
		{
			strcat((char *)ProtMsg.data, argv[i]);
			strcat((char *)ProtMsg.data, " ");
		}
		
		protocolMsgDispatch(AO_Debug, &ProtMsg);
  }
	else
	{
		pCli->Init.Write("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
	}
}

/**
  * @brief  CLICmd_Print
  * @param  
  * @retval 
  */
void CLICmd_Print(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  if(argc >= 2)
  {
		if(!strcmp(argv[0], "print") & !strcmp(argv[1], "V5"))
		{
			if(!strcmp(argv[2], "on"))
			{
				QACTIVE_POST(AO_Atsx, Q_NEW(QEvt, ATSX_POW_PRINT_ON_SIG), me);
			}
			else if(!strcmp(argv[2], "off") )
			{
				QACTIVE_POST(AO_Atsx, Q_NEW(QEvt, ATSX_POW_PRINT_OFF_SIG), me);
			}		
		}
		else
		{
			pCli->Init.Write("\033[31m Command not find! argv=%s %s %s \033[0m \r\n", argv[0], argv[1], argv[2]);
		}
  }
}

/**
  * @brief  CLICmd_Print
  * @param  
  * @retval 
  */
void CLICmd_IpCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  if(argc > 1)
  {
		if(!strcmp(argv[1], "set"))
		{
			IpEvt * const pe = Q_NEW(IpEvt, SET_IP_SIG);
			pe->addr[0] = str2u32(argv[2]);
			pe->addr[1] = str2u32(argv[3]);
			pe->addr[2] = str2u32(argv[4]);
			pe->addr[3] = str2u32(argv[5]);
			
			QACTIVE_POST(AO_Ethernet, &pe->super, me);
		}
		else if(!strcmp(argv[1], "get"))
		{
			AtsxEvt * const pe = Q_NEW(AtsxEvt, GET_IP_SIG);
			
			QACTIVE_POST(AO_Ethernet, &pe->super, me);	
		}
		else
		{
			pCli->Init.Write("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
		}
  }
}

/**
  * @brief  CLICmd_Print
  * @param  
  * @retval 
  */
void CLICmd_BaudCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
  if(argc > 2)
  {
		UartEvt * const pe = Q_NEW(UartEvt, SET_BAUDRATE_SIG);
		pe->baudrate = str2u32(argv[2]);
				
		if(!strcmp(argv[1], "485IN"))
		{			
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);	
		}
		else if(!strcmp(argv[1], "485EX"))
		{
			QACTIVE_POST(AO_RS485[RS485_EX], &pe->super, me);	
		}
		else if(!strcmp(argv[1], "232A"))
		{
			QACTIVE_POST(AO_RS232[RS232_A], &pe->super, me);	
		}
		else if(!strcmp(argv[1], "232B"))
		{
			QACTIVE_POST(AO_RS232[RS232_B], &pe->super, me);	
		}
			
  }
	else
	{
		pCli->Init.Write("\033[31m Command not find! argv=%s %s %s \033[0m \r\n", argv[0], argv[1], argv[2]);
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

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void cliCmdReadInputReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{	
	ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
	pe->slave_addr = str2u32(argv[1]);
	pe->start_addr = str2u32(argv[2]);
	pe->reg_count = str2u32(argv[3]);	
	pe->sender = NULL;
	QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
}

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void cliCmdReadHoldReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{	
	ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_HOLD_REG_SIG);
	pe->slave_addr = str2u32(argv[1]);
	pe->start_addr = str2u32(argv[2]);
	pe->reg_count = str2u32(argv[3]);
	pe->sender = NULL;
	QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
}

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void cliCmdWriteHoldReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{	
	ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
	pe->slave_addr = str2u32(argv[1]);
	pe->start_addr = str2u32(argv[2]);
	pe->reg_value = str2u32(argv[3]);
	pe->sender = NULL;
	QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
}

/**
  * @brief  CLICmd_IwdgCtrl
  * @param  
  * @retval 
  */
void cliCmdWriteHoldsReg(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{	
	ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_MULTI_HOLD_REG_SIG);
	pe->slave_addr = str2u32(argv[1]);
	pe->start_addr = str2u32(argv[2]);
	pe->reg_value = str2u32(argv[3]);
	pe->sender = NULL;
	QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
}

/**
  * @brief  CLICmd_GpioCtrl
  * @param  
  * @retval 
  */
const CLICmdTypedef CLICmd_ModbusCtrl[] =
{
  {"readinput", "readinput slave_addr reg_addr reg_count", cliCmdReadInputReg, 0},
  {"readhold" , "readhold slave_addr reg_addr reg_count", cliCmdReadHoldReg, 0},
  {"writehold", "writehold slave_addr reg_addr reg_value", cliCmdWriteHoldReg, 0},
  {"writeholds", "writeholds slave_addr reg_addr reg_value", cliCmdWriteHoldsReg, 0},
  // last command must be all 0s.
  {0, 0, 0, 0}
};

/**
  * @brief  CLICmd_GpioCtrl
  * @param  
  * @retval 
  */
const CLICmdTypedef CLICmd_ProtocolCtrl[] =
{	
  {" sort " , "test item      : cmdid  data"		, 0, 0},
  {"system ", "atsx status    : 0x1001 NULL"		, 0, 0},
  {" "			, "test result    : 0x1002 pass/ng/reset", 0, 0},
  {" "			, "heart heat     : 0x1003 NULL"		, 0, 0},
  {" "			, "set ip  (dec)  : 0x1004 x.x.x.x"	, 0, 0},
  {" "			, "set mac (hex)  : 0x1005 x:x:x:x:x:x", 0, 0},
  {" "			, "get version    : 0x1006 NULL"		, 0, 0},
  {"vol"		, "5V_1           : 0x2101 NULL"		, 0, 0},
  {" "			, "5V_2           : 0x2102 NULL"		, 0, 0},
  {" "			, "5V_3           : 0x2103 NULL"		, 0, 0},
  {" "			, "12V            : 0x2104 NULL"		, 0, 0},
  {" "			, "VAMP           : 0x2105 NULL"		, 0, 0},
  {" "			, "V1215_P        : 0x2106 NULL"		, 0, 0},
  {" "			, "V1215_N        : 0x2107 NULL"		, 0, 0},
  {"pwm"		, "pwm freq       : 0x2201 int(mV)"	, 0, 0},
  {" "			, "pwm duty       : 0x2202 int(%)"	, 0, 0},
  {" "			, "logic freq     : 0x2203 int(mV)"	, 0, 0},
  {" "			, "logic duty     : 0x2204 int(%)"	, 0, 0},
  {" "			, "logic interval : 0x2205 int(us)"	, 0, 0},
  {"others" , "impedance test : 0x2401 NULL"		, 0, 0},
  {" "			, "short 12V      : 0x2402 int(ms)"	, 0, 0},
  {" "			, "power off      : 0x2409 int(ms)"	, 0, 0},
  {" "			, "12V load sw    : 0x240a on/off"	, 0, 0},
  {" "			, "aging status   : 0x240b NULL"		, 0, 0},
  {"V220"		, "220V switch    : 0x2403 on/off/half", 0, 0},
  {" "			, "cap discharge  : 0x2404 on/off"	, 0, 0},
  {" "			, "cap status     : 0x2405 NULL"		, 0, 0},
  {" "			, "bl_on          : 0x2406 on/off"	, 0, 0},
  {" "			, "ps_on          : 0x2407 on/off"	, 0, 0},
  {" "			, "adj_on         : 0x2408 on/off"	, 0, 0},
  {"power"	, "power factor   : 0x2601 NULL"		, 0, 0},
  {"e load"	, "channel switch : 0x2710 hex(bit0~3)", 0, 0},
  {"  ch(n)", "load mode      : 0x27(n)1 hex(CC:0x01, CV:0x02, CR:0x03, CP:0x04, LED:0x05)", 0, 0},
  {" "			, "Vo value       : 0x27(n)2 int(mA/mV/mW/mR)", 0, 0},
  {" "			, "Io value       : 0x27(n)3 int(mA)"	, 0, 0},
  {" "			, "Von value      : 0x27(n)4 int(mV)"	, 0, 0},
  {" "			, "Rd coefficient : 0x27(n)5 folat"		, 0, 0},
  {" "			, "RC             : 0x27(n)6 on/off"	, 0, 0},  
  {" "			, "input resistor : 0x27(n)7 int"			, 0, 0}, 
  {" "			, "Von latch      : 0x27(n)8 on/off"	, 0, 0}, 
  {" "			, "set vol max    : 0x27(n)9 int(mV)"	, 0, 0}, 
  {" "			, "set vol min    : 0x27(n)A int(mV)"	, 0, 0}, 
  {" "			, "set I max      : 0x27(n)B int(mA)"	, 0, 0}, 
  {" "			, "set I min      : 0x27(n)C int(mA)"	, 0, 0}, 
  {" "			, "read Vol       : 0x27D0 hex(n)"	  , 0, 0}, 
  {" "			, "read I         : 0x27D1 hex(n)"	  , 0, 0},  
  {"ledLoad", "channel switch : 0x2810 hex(bit0~3)", 0, 0},
  {"  ch(n)", "led number     : 0x28(n)1 int"			, 0, 0},
  {" "			, "set vol max    : 0x28(n)2 int(mV)"	, 0, 0}, 
  {" "			, "set vol mix    : 0x28(n)3 int(mV)"	, 0, 0}, 
  {" "			, "set I max      : 0x28(n)4 int(mA)"	, 0, 0}, 
  {" "			, "set I max      : 0x28(n)5 int(mA)"	, 0, 0}, 
  {" "			, "read Vol       : 0x28D0 hex(n)"	  , 0, 0}, 
  {" "			, "read I         : 0x28D1 hex(n)"	  , 0, 0},  
  {"atsx", "atsx (A/B) cmdid data", CLICmd_AtsxCtrl, 0},
  // last command must be all 0s.
  {0, 0, 0, 0}
};

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

	
