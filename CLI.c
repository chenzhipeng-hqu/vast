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
#include <stdbool.h>
#include "usart.h"
#include "ring_queue.h"
#include "qpc_common.h"

#include "CLI.h"

/*************************************
              define
*************************************/
#define 	CLI_BUFFER_SIZE   	32
#define 	CLI_HISTORY_SIZE  	9
#define 	CLI_CMD_TREE_LEVEL 	4

#define 	ASCII_NULL  			0x00
#define 	ASCII_CANCEL 			0x03
#define 	ASCII_BELL  			0x07
#define 	ASCII_BS  				0x08	//backspace
#define 	ASCII_BS1 				0x7f
#define 	ASCII_TAB 				0x09	// '\t'
#define 	ASCII_LF  				0x0a  // '\r'
#define 	ASCII_CR  				0x0d  // '\n'
#define 	ASCII_ESC 				0x1b
#define 	ASCII_SPACE 			0x20
#define 	ASCII_ARROW 			0x5b
#define 	ASCII_UP  				0x41
#define 	ASCII_DOWN  			0x42
#define 	ASCII_RIGHT 			0x43
#define 	ASCII_LEFT  			0x44
#define 	ASCII_HAT 				0x5e  // '^'
#define 	ASCII_CMD 				'$'

/**************************************
              typedef
**************************************/
typedef enum
{
  CLI_CMD_EXE,
  CLI_CMD_DIR,
} CLI_CMD_Type;


typedef enum
{
  CLI_NORMAL,
  // process multi bytes char.
  CLI_ESC,
  CLI_ESC_ARROW,
} CLI_State;

/*************************************
              variable
*************************************/
static uint8_t CLI_Buffer[CLI_HISTORY_SIZE][CLI_BUFFER_SIZE] = {0};
RingQueueTypedef RingQueue;
int g_CLI_CurrentIdx = 0;
int g_CLI_HistoryIdx = 0;
static int g_CLI_PosIdx = 0;

static const CLICmdTypedef *CLI_CmdPath[CLI_CMD_TREE_LEVEL] = {CLI_CmdTableMain, 0};
static uint16_t cliCmdLevel = 0;
uint8_t echoEnable = 1;

/*************************************
         function prototypes
*************************************/

/*************************************
              function
*************************************/
/**
  * @brief  CLI_Initialize
  * @param  
  * @retval 
  */
int CLI_Initialize(void)
{
	ring_queue_init(&RingQueue, 100);
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)RingQueue.data, RingQueue.length);
	
	return 0;
}

/**
  * @brief  CLI_FindCmd
  * @param  
  * @retval 
  */
uint16_t CLI_GotoPath(const char *path, uint16_t *pPos)
{
  uint16_t idx = 0;
  uint16_t tmpLevel = cliCmdLevel;
  uint16_t pos = 0, len = 0;

  while(tmpLevel < CLI_CMD_TREE_LEVEL-1)
  {
    if(path[pos] == 0)
    {
      break;
    }
    while(path[pos+len])
    {
      if(path[pos+len] == '/')
      {
        break;
      }
      len++;
    }
    while(CLI_CmdPath[tmpLevel][idx].cmd)
    {
      if((strncmp(path+pos, CLI_CmdPath[tmpLevel][idx].cmd, len) == 0)
        &&(strlen(CLI_CmdPath[tmpLevel][idx].cmd) == len))
      {
        if(CLI_CmdPath[tmpLevel][idx].child)
        {
          CLI_CmdPath[tmpLevel+1] = CLI_CmdPath[tmpLevel][idx].child;
          tmpLevel ++;
          idx = 0;
          break;
        }
      }
      idx++;
    }
    if(CLI_CmdPath[tmpLevel][idx].cmd == 0)
    {
      break;
    }
    pos += len+1;
    len = 0;
  }
  if(pPos)
  {
    *pPos = pos;
  }
  return tmpLevel;
}

/**
  * @brief  CLI_FindCmd
  * @param  
  * @retval 
  */
CLICmdTypedef const *CLI_FindCmd(const char *cmd, CLI_CMD_Type mask)
{
	CLICmdTypedef const *pCmd = NULL;
	uint16_t idx = 0, tmpLevel, pos;

  tmpLevel = CLI_GotoPath(cmd, &pos);
  while(CLI_CmdPath[tmpLevel][idx].cmd)
  {
    if(strcmp(cmd+pos, CLI_CmdPath[tmpLevel][idx].cmd) == 0)
    {
      if(((mask == CLI_CMD_DIR) && (CLI_CmdPath[tmpLevel][idx].child))
          || ((mask == CLI_CMD_EXE) && (CLI_CmdPath[tmpLevel][idx].pFun))
        )
      {
        pCmd = &(CLI_CmdPath[tmpLevel][idx]);
        break;
      }
    }
    idx ++;
  }
	
	return pCmd;
}

/**
  * @brief  CLICmd_List
  * @param  
  * @retval 
  */
void CLICmd_List(void)
{
  uint16_t idx = 0;
	uint8_t old_echoEbale = 0;
	
	old_echoEbale = echoEnable;
	echoEnable = 1;
  CLI_PutString("Current command list:\r\n");
  while(CLI_CmdPath[cliCmdLevel][idx].cmd)
  {
    CLI_PutString("\t%s\t-> %s \r\n", CLI_CmdPath[cliCmdLevel][idx].cmd, CLI_CmdPath[cliCmdLevel][idx].help);
    idx ++;
  }
	echoEnable = old_echoEbale;
}

/**
  * @brief  CLICmd_GotoTree
  * @param  
  * @retval 
  */
void CLICmd_GotoTree(int argc, char *argv[])
{
  uint16_t pathLevel;
  char *p;
	
  if(argc > 1)
  {
    p = argv[1];
    if(strcmp(p, "..") == 0)
    {
      if(cliCmdLevel > 0)
      {
        cliCmdLevel --;
      }
    }
    else if(cliCmdLevel < CLI_CMD_TREE_LEVEL - 1)
    {
      pathLevel = CLI_GotoPath(p, 0);
      if(pathLevel != cliCmdLevel)
      {
        cliCmdLevel = pathLevel;
        CLICmd_List();
      }
      else
      {
        CLI_PutString("Dir not find!\r\n");
      }
    }
  }
}

/**
  * @brief  CLICmd_History
  * @param  
  * @retval 
  */
void CLICmd_History(int argc, char *argv[])
{
	uint16_t idx = 0;

  CLI_PutString("history command list:\r\n");	
  while(idx < CLI_HISTORY_SIZE)
  {	
    CLI_PutString("\t%s \r\n", CLI_Buffer[idx]);
    idx ++;
  }
}

/**
  * @brief  CLI_Execute
  * @param  
  * @retval 
  */
int CLI_Execute(const char *cmd)
{
	CLICmdTypedef const *pCmd;
	int argc = 0, idx, argvStrlen, ret = 0;
	char argvStr[CLI_BUFFER_SIZE];
	char *argv[10];
	
	strcpy(argvStr, cmd);
	argv[argc++] = argvStr;
	argvStrlen = strlen(argvStr);

	for(idx=0; idx<argvStrlen; idx++)
	{
		if(argvStr[idx] == ASCII_NULL)
		{
			argvStr[idx] = 0;
			break;
		}
		else if (argvStr[idx] == ASCII_BS)
		{
			CLI_PutString("BS: 0x%02x \r\n", argvStr[idx]);
		}
		else if( (argvStr[idx] == ASCII_SPACE)
						| (argvStr[idx] == ASCII_CR)
						| (argvStr[idx] == ASCII_TAB)
		)
		{
			argvStr[idx] = 0;
			if( (argvStr[idx + 1] != ASCII_SPACE)
				& (argvStr[idx + 1] != ASCII_CR)
				& (argvStr[idx + 1] != ASCII_TAB)								
			)
			{
				argv[argc++] = argvStr + idx + 1;
			}
		}
		else
		{	// query ASCII table
			//CLI_PutString("0x%02x \r\n", argvStr[idx]);
		}
	}
	
	pCmd = CLI_FindCmd(argv[0], CLI_CMD_EXE);
	
	if(pCmd != NULL)
	{
		pCmd->pFun(argc, argv);
	}	
  else if((strcmp(argv[0], "ls") == 0) | (strcmp(argv[0], "ll") == 0))
  {// common command.
    CLICmd_List();
  }
  else if(strcmp(argv[0], "cd") == 0)
  {
    CLICmd_GotoTree(argc, argv);
  }
  else if(strcmp(argv[0], "history") == 0)
  {
    CLICmd_History(argc, argv);
  }
  else if (strcmp(argv[0], "") != 0)
  {
    CLI_PutString("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
    ret = 1;
  }
	else
	{ // enter command
		ret = 2;
	}
		
	return ret;
}

/**
  * @brief  CLI_Enter
  * @param  
  * @retval 
  */
int CLI_Enter(void)
{
	CLI_PutString("%c%c", ASCII_CR, ASCII_LF);
	
	CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ASCII_NULL;
	
	if(0 == CLI_Execute((const char *)CLI_Buffer[g_CLI_CurrentIdx]))
	{
		g_CLI_HistoryIdx = g_CLI_CurrentIdx = (g_CLI_CurrentIdx+1)%CLI_HISTORY_SIZE;
	}				
	
	g_CLI_PosIdx = 0;
	
	CLI_PutString("%c%c", ASCII_CMD, ASCII_SPACE);

	return 0;
}

/**
  * @brief  CLI_Cancel
  * @param  
  * @retval 
  */
int CLI_Cancel(void)
{
	CLI_PutString("%c%c%c%c%c", g_CLI_PosIdx ? ASCII_HAT : ASCII_NULL, ASCII_CR, ASCII_LF, ASCII_CMD, ASCII_SPACE);
	g_CLI_PosIdx = 0;
  g_CLI_HistoryIdx = g_CLI_CurrentIdx;
	
	return 0;
}

/**
  * @brief  cli backspace, delete previous input char.
  * @param  None.
  * @retval Backspace success(0) or not(1).
  */
uint8_t CLI_Backspace(void)
{
  uint8_t backspaceFail;

  if(g_CLI_PosIdx > 0)
  {
    CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
    g_CLI_PosIdx --;
    backspaceFail = false;
  }
  else
  {
    CLI_PutString("%c", ASCII_BELL);
    backspaceFail = true;
  }
  return backspaceFail;
}

/**
  * @brief  Process tab ch, list current command dir.
  * @param  None.
  * @retval Always success(0).
  */
uint8_t CLI_Tab(void)
{
	CLI_PutString("%c%c", ASCII_CR, ASCII_LF);
  CLI_Execute("ls");
	CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ASCII_NULL;
	CLI_PutString("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
 
  return false;
}

/**
  * @brief  Push the rx data to command fifo.
  * @param  data byte.
  * @retval Push fifo success(0) or not(1). Fifo may overflow!
  */
uint8_t CLI_PushChar(uint8_t ch)
{
  uint8_t pushFail;

  if(g_CLI_PosIdx < CLI_BUFFER_SIZE - 1)
  {
    CLI_PutString("%c", ch);
    CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx++] = ch;
    pushFail = false;
  }
  else
  {
    CLI_PutString("%c", ASCII_BELL);
    pushFail = true;
  }
  return pushFail;
}

/**
  * @brief  Load cli history command.
  * @param  History up(1) or down(0).
  * @retval History load success(0) or not(1).
  */
uint8_t CLI_LoadHistory(uint8_t historyUp)
{
  int next, tmp;
  uint8_t historyOk = true;

  if(historyUp)
  {
    next = (g_CLI_HistoryIdx + CLI_HISTORY_SIZE - 1) % CLI_HISTORY_SIZE;
    if((next != g_CLI_CurrentIdx) && (CLI_Buffer[next][0] != ASCII_NULL))
    {// not null, switch to this history.
      for(tmp = 0; tmp < g_CLI_PosIdx; tmp++)
      {
				CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
      }
      g_CLI_HistoryIdx = next;
      CLI_PutString("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			g_CLI_PosIdx = strlen((const char *)CLI_Buffer[g_CLI_HistoryIdx]);
			memcpy(CLI_Buffer[g_CLI_CurrentIdx], CLI_Buffer[g_CLI_HistoryIdx], g_CLI_PosIdx);	
    }
    else
    {
			CLI_PutString("%c", ASCII_BELL);
      historyOk = false;
    }
  }
  else // history down.
  { // clean current line.
		for(tmp = 0; tmp < g_CLI_PosIdx; tmp++)
		{
			CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
		}
    g_CLI_PosIdx = 0;
    next = (g_CLI_HistoryIdx + 1) % CLI_HISTORY_SIZE;
    if((next != g_CLI_CurrentIdx) && (g_CLI_HistoryIdx != g_CLI_CurrentIdx))
    {	// not null, switch to this history.
      g_CLI_HistoryIdx = next;	
			CLI_PutString("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			g_CLI_PosIdx = strlen((const char *)CLI_Buffer[g_CLI_HistoryIdx]);
			memcpy(CLI_Buffer[g_CLI_CurrentIdx], CLI_Buffer[g_CLI_HistoryIdx], g_CLI_PosIdx);	
    }
    else
    {		
      g_CLI_HistoryIdx = g_CLI_CurrentIdx;
			CLI_PutString("%c", ASCII_BELL);
      historyOk = false;
    }
  }
  return historyOk;
}


/**
  * @brief  CLI_Handle
  * @param  
  * @retval 
  */
int CLI_Handle(void)
{	
	static CLI_State inputState = CLI_NORMAL;
	char pop_ch = 0;
	
	ring_queue_pop(&RingQueue, &pop_ch, 1);
	
	if(pop_ch != ASCII_NULL) {
		switch(inputState) {
			case CLI_ESC: {	
				switch(pop_ch) {
					case ASCII_ARROW:
						inputState = CLI_ESC_ARROW;
						break;
					default:// ch
						inputState = CLI_NORMAL;
						break;
				}
				break;
			}
			case CLI_ESC_ARROW: {
				switch(pop_ch) {
					case ASCII_UP:
					case ASCII_DOWN:
						CLI_LoadHistory(pop_ch == ASCII_UP);
						break;
					case ASCII_RIGHT:
						break;
					case ASCII_LEFT:
						CLI_Backspace();
						break;
					default:// ch
						break;
				}
				inputState = CLI_NORMAL;
				break;
			}
			default: {
				switch(pop_ch) {
					case ASCII_CANCEL: {
						CLI_Cancel();
						break;
					}
					case ASCII_CR:
					case ASCII_LF: {
						CLI_Enter();		
						break;
					}
					case ASCII_BS:
					case ASCII_BS1: {
						CLI_Backspace();
						break;
					}
					case ASCII_ESC: {	// escape character, should translate to other char.
						inputState = CLI_ESC;
						break;
					}
					case ASCII_TAB: {
						CLI_Tab();
						break;
					}
					default: {
						CLI_PushChar(pop_ch);
						break;
					}	
				}
			}
		}
	}
	
	return 0;
}

// "0x123"
uint32_t hex2u32(const char *str)
{
  const char *pCh = str;
  uint32_t data = 0;
  if(pCh[1] == 'x' || pCh[1] == 'X')
  {
    //skip 0x
    pCh += 2;
  }
  while(*pCh)
  {
    if(data & 0xf0000000)
    {
      // support max 32 bit data.
      break;
    }
    if(*pCh >= 'a' && *pCh <= 'f')
    {
      data <<= 4;
      data += *pCh - 'a' + 10;
    }
    else if(*pCh >= 'A' && *pCh <= 'F')
    {
      data <<= 4;
      data += *pCh - 'A' + 10;
    }
    else if(*pCh >= '0' && *pCh <= '9')
    {
      data <<= 4;
      data += *pCh - '0';
    }
    else
    {
      break;
    }
    pCh ++;
  }
  return data;
}

uint32_t str2u32(const char *str)
{
  const char *pCh = str;
  uint32_t data = 0;
  if(pCh[1] == 'x' || pCh[1] == 'X')
  {
    return hex2u32(str);
  }
  while(*pCh)
  {
    if(data > 0xffffffff / 10)
    {
      // support max 32 bit data.
      break;
    }
    if(*pCh >= '0' && *pCh <= '9')
    {
      data *= 10;
      data += *pCh - '0';
    }
    else
    {
      break;
    }
    pCh ++;
  }
  return data;
}

int32_t str2s32(const char *str)
{
  const char *pCh = str;
  int32_t data = 1;
  if(*pCh == '-')
  {
    data = -1;
    pCh ++;
  }
  else if(*pCh == '+')
  {
    pCh ++;
  }
  data = (int32_t)str2u32(pCh) * data;
  return data;
}

float str2float(const char *str)
{
  const char *pCh = str;
  int32_t data = 0;
  int32_t fractional = 0;
  uint32_t flag = 0;
  int sign = 1;
  if(*pCh == '-')
  {
    sign = -1;
    pCh ++;
  }
  else if(*pCh == '+')
  {
    pCh ++;
  }
  while(*pCh)
  {
    if(*pCh == '.')
    {
      flag = 1;
    }
    else if(*pCh >= '0' && *pCh <= '9')
    {
      if(flag)
      {
        fractional *= 10;
        fractional += *pCh - '0';
        flag *= 10;
      }
      else
      {
        data *= 10;
        data += *pCh - '0';
      }
    }
    else
    {
      break;
    }
    pCh ++;
  }
  data *= sign;
  fractional *= sign;
  return flag?((float)fractional/flag + data):(float)data;
}


/**
  * @}
  */
/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

