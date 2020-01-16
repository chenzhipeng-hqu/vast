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
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
exp:

#include <stdarg.h>

CLI_HandleTypeDef	hcli;

int postToLog(const char *format, ... )
{
	va_list arg;
	int len;

	va_start(arg, format);
	len = vsnprintf(SendBuf, sizeof(SendBuf), format, arg);
	va_end(arg);

	if(huart1.gState == HAL_UART_STATE_READY)
	{
		memcpy(tx_buff, SendBuf, len);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buff, len);
	}
	else
	{
		pMsgPush = (MsgTypedef *)vast_malloc(sizeof(MsgTypedef)+len);

		if(pMsgPush == NULL)
		{
			printf("data=NULL!!!\r\n");
			return 1;
		}

		pMsgPush->len = len;
		memcpy(pMsgPush->data, SendBuf, pMsgPush->len);
		LinkListPush(&UsartSendLinkList, (void *)pMsgPush);
	}
	return 0;
}

uint8_t CliProcess_uart1 (CLI_HandleTypeDef *pCli, CLI_SEL_FUNCx selFunc)
{
	uint8_t ret = 0;

	switch(selFunc)
	{
		case CLI_FUNC_INIT :
		{
			vast_ring_queue_init(&RingQueue, 128, NULL);
			HAL_UART_Receive_DMA(&huart1, (uint8_t *)RingQueue.data, RingQueue.length);

			pCli->Init.Write("command-line interface initialize ok.\r\n");
			break;
		}
		case CLI_FUNC_TX_POP :
		{
		#if 0
			if(UsartSendLinkList->len > 0)
			{
				LinkListPop(&UsartSendLinkList, (void **)&pMsgPop);
				memcpy(tx_buff, pMsgPop->data, pMsgPop->len);
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buff, pMsgPop->len);
				vast_free(pMsgPop);
			}
		#endif
			break;
		}
		case CLI_FUNC_RX_PUSH :
		{
			break;
		}
		case CLI_FUNC_RX_POP :
		{
			vast_ring_queue_pop(&RingQueue, (char *)&ret, 1);
			break;
		}
		default:
		{
			//printf("%s %d, err cli func!\r\n", __FUNCTION__, __LINE__);
			break;
		}
	}

	return ret;
}

int main(int argc, char *argv[])
{
	hcli.Init.Write = postToDebugLog;
	hcli.pCLIProcess = CliProcess_uart1;
	CLI_Initialize(&hcli);

	while(1)
	{
		CLI_Handle(&hcli);
	}

	return 0;
}

  */
	
/** @addtogroup Template_Project
  * @{
  */ 
/*************************************
              include
*************************************/
#include <stdio.h>
#include <string.h>
#include "vast_cli.h"

/*************************************
              define
*************************************/
#define 	CLI_BUFFER_SIZE   	64
#define 	CLI_HISTORY_SIZE  	9
#define 	CLI_CMD_TREE_LEVEL 	3

#define 	ASCII_NULL  			0x00
#define 	ASCII_CANCEL 			0x03
#define 	ASCII_BELL  			0x07
#define 	ASCII_BS  				0x08	//backspace
#define 	ASCII_BS1 				0x7f
#define 	ASCII_TAB 				0x09	// '\t'
#define 	ASCII_LF  				0x0a  // '\n'
#define 	ASCII_CR  				0x0d  // '\r'
#define 	ASCII_ESC 				0x1b
#define 	ASCII_SPACE 			0x20
#define 	ASCII_ARROW 			0x5b
#define 	ASCII_UP  				0x41
#define 	ASCII_DOWN  			0x42
#define 	ASCII_RIGHT 			0x43
#define 	ASCII_LEFT  			0x44
#define 	ASCII_HAT 				0x5e  // '^'
#define 	ASCII_CMD 				'$'

#define		ASCII_LEFT_SUPPORT 		false

#define		CLI_CMD_LAST(cmd, help, func, child) 	\
			const struct _CLICmdTypedef __cli_last __attribute__((used)) 		\
			__attribute__((__section__("cliTableLast"))) = {	\
				cmd,								\
				help,								\
				(cli_func_t)func,					\
				(struct _CLICmdTypedef *)child		\
			}

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
int g_CLI_CurrentIdx = 0;
int g_CLI_HistoryIdx = 0;
static int g_CLI_PosIdx = 0;


CLI_CMD_LAST(0, 0, 0, 0);
CLI_CMD_EXPORT(cd		, "goto dir", CLICmd_GotoTree, 0);

extern CLICmdTypedef __cliTab_start, __cliTab_end; /*申明外部变量,在ld的脚本文件中定义*/
static const CLICmdTypedef *CLI_CmdPath[CLI_CMD_TREE_LEVEL] = {&__cliTab_start, 0};
static uint16_t cliCmdLevel = 0;

#if	ASCII_LEFT_SUPPORT	== true
	static int g_CLI_LeftPosIdx = 0;
#endif

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
int16_t CLI_Initialize(CLI_HandleTypeDef *pCli)
{
	if(pCli != NULL)
	{
		pCli->pCLIProcess(pCli, CLI_FUNC_INIT);
		return 0;
	}
	else
	{
		return 1;
	}
	
//	ring_queue_init(&RingQueue, 100);
//	HAL_UART_Receive_DMA(&huart1, (uint8_t *)RingQueue.data, RingQueue.length);
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
void CLICmd_List(CLI_HandleTypeDef *pCli)
{
  uint16_t idx = 0;
	
  pCli->Init.Write("Current command list:\r\n");
  while(CLI_CmdPath[cliCmdLevel][idx].cmd)
  {
    pCli->Init.Write("\t%s\t-> %s \r\n", CLI_CmdPath[cliCmdLevel][idx].cmd, CLI_CmdPath[cliCmdLevel][idx].help);
    idx ++;
  }
}

/**
  * @brief  CLICmd_GotoTree
  * @param  
  * @retval 
  */
void CLICmd_GotoTree(CLI_HandleTypeDef *pCli, int argc, char *argv[])
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
        CLICmd_List(pCli);
      }
      else
      {
        pCli->Init.Write("Dir not find!\r\n");
      }
    }
  }
}

/**
  * @brief  CLICmd_History
  * @param  
  * @retval 
  */
void CLICmd_History(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
	uint16_t idx = 0;

  pCli->Init.Write("history command list:\r\n");	
  while(idx < CLI_HISTORY_SIZE)
  {	
    pCli->Init.Write("\t%s \r\n", CLI_Buffer[idx]);
    idx ++;
  }
}

/**
  * @brief  protocolMsgDispatch.
  * @param  
  * @retval 
  */
int16_t findArgument(char *argvStr, char *argv[], char separationChar)
{
	int argc = 0, idx = 0, argvStrlen = 0;
	
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
//			CLI_PutString("BS: 0x%02x \r\n", argvStr[idx]);
		}
		else if( (argvStr[idx] == separationChar)
						| (argvStr[idx] == ASCII_CR)
						| (argvStr[idx] == ASCII_TAB)
		)
		{
			argvStr[idx] = 0;
			if( (argvStr[idx + 1] != separationChar)
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
	return argc;
}

/**
  * @brief  CLI_Execute
  * @param  
  * @retval 
  */
int CLI_Execute(CLI_HandleTypeDef *pCli, const char *cmd)
{
	CLICmdTypedef const *pCmd;
	int argc = 0, ret = 0;
	char argvStr[CLI_BUFFER_SIZE] = {0};
	char *argv[10] = {0};
	
	strcpy(argvStr, cmd);

	argc = findArgument(argvStr, argv, ASCII_SPACE);
	
	pCmd = CLI_FindCmd(argv[0], CLI_CMD_EXE);
	
	if(pCmd != NULL)
	{
		pCmd->pFun(pCli, argc, argv);
	}	
  else if((strcmp(argv[0], "ls") == 0) | (strcmp(argv[0], "ll") == 0))
  {// common command.
    CLICmd_List(pCli);
  }
  else if(strcmp(argv[0], "cd") == 0)
  {
    CLICmd_GotoTree(pCli, argc, argv);
  }
  else if(strcmp(argv[0], "history") == 0)
  {
    CLICmd_History(pCli, argc, argv);
  }
  else if (strcmp(argv[0], "") != 0)
  {
		pCli->Init.Write("\033[31m Command not find! argv=");
    
		for(uint8_t i=0; i<argc; i++)
		{
			pCli->Init.Write("%s ", argv[i]);  
		}	
		
		pCli->Init.Write("\033[0m \r\n");  
				
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
int CLI_Enter(CLI_HandleTypeDef *pCli)
{
	pCli->Init.Write("%c%c", ASCII_CR, ASCII_LF);
	
	CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ASCII_NULL;
	
	if(0 == CLI_Execute(pCli, (const char *)CLI_Buffer[g_CLI_CurrentIdx]))
	{
		g_CLI_HistoryIdx = g_CLI_CurrentIdx = (g_CLI_CurrentIdx+1)%CLI_HISTORY_SIZE;
	}				
	
	g_CLI_PosIdx = 0;
    memset(CLI_Buffer[g_CLI_CurrentIdx], ASCII_NULL, CLI_BUFFER_SIZE);

#if	ASCII_LEFT_SUPPORT	== true
	g_CLI_LeftPosIdx = 0;
#endif
	
	pCli->Init.Write("%c%c", ASCII_CMD, ASCII_SPACE);

	return 0;
}

/**
  * @brief  CLI_Cancel
  * @param  
  * @retval 
  */
int CLI_Cancel(CLI_HandleTypeDef *pCli)
{
	pCli->Init.Write("%c%c%c%c%c", g_CLI_PosIdx ? ASCII_HAT : ASCII_NULL, ASCII_CR, ASCII_LF, ASCII_CMD, ASCII_SPACE);
	g_CLI_PosIdx = 0;
    memset(CLI_Buffer[g_CLI_CurrentIdx], ASCII_NULL, CLI_BUFFER_SIZE);
	
#if	ASCII_LEFT_SUPPORT	== true
	g_CLI_LeftPosIdx = 0;
#endif

  g_CLI_HistoryIdx = g_CLI_CurrentIdx;
	
	return 0;
}

/**
  * @brief  cli backspace, delete previous input char.
  * @param  None.
  * @retval Backspace success(0) or not(1).
  */
uint8_t CLI_Backspace(CLI_HandleTypeDef *pCli)
{
  uint8_t backspaceFail;

  if(g_CLI_PosIdx > 0)
  {
//    CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
		pCli->Init.Write("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);

	#if	ASCII_LEFT_SUPPORT	== true
		if(g_CLI_LeftPosIdx)
		{
			CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-g_CLI_LeftPosIdx] = ASCII_SPACE;
			
			for(uint8_t pos=g_CLI_LeftPosIdx; pos > 0; pos--)
			{
				CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos+g_CLI_LeftPosIdx-1] = CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos+g_CLI_LeftPosIdx];
				CLI_PutString("%c", CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos-1]);
			}
			
			for(uint8_t i=0; i<g_CLI_LeftPosIdx; i++)
			{
				CLI_PutString("\x1b\x5b\x44");
			}			
		}
	#endif	
    g_CLI_PosIdx --;
    backspaceFail = false;
  }
  else
  {
//    CLI_PutString("%c", ASCII_BELL);
		pCli->Init.Write("%c", ASCII_BELL);
    backspaceFail = true;
  }
  return backspaceFail;
}

#define CLI_AUTO_COMPLETE

static int str_common(const char *str1, const char *str2)
{
    const char *str = str1;

    while ((*str != 0) && (*str2 != 0) && (*str == *str2))
    {
        str ++;
        str2 ++;
    }

    return (str - str1);
}

static void do_auto_complete(CLI_HandleTypeDef *pCli, char *prefix)
{
    int length, min_length;
    const char *name_ptr, *cmd_name;
    CLICmdTypedef *index;

    min_length = 0;
    name_ptr = NULL;

    if (*prefix == '\0')
    //if (g_CLI_PosIdx == 0)
    {
        //msh_help(0, NULL);
    //	CLI_PutString("%c%c", ASCII_CR, ASCII_LF);
        pCli->Init.Write("%c%c", ASCII_CR, ASCII_LF);
      //CLI_Execute(pCli, "ls");
      CLICmd_List(pCli);
        CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ASCII_NULL;
    //	CLI_PutString("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
        pCli->Init.Write("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
        return;
    }

    for (index = &__cliTab_start; index < &__cliTab_end; index++)
    {
        cmd_name = (const char *)index->cmd;
        if (!strncmp(prefix, cmd_name, strlen(prefix)))
        {
            if (min_length == 0)
            {
                name_ptr = cmd_name;
                min_length = strlen(name_ptr);
            }

            length = str_common(name_ptr, cmd_name);
            if (length < min_length)
                min_length = length;

            pCli->Init.Write("%s\r\n", cmd_name);
        }
    }

    if (name_ptr) 
    {
        //pCli->Init.Write("g_CLI_PosIdx=%d, min_length:%d, %s\r\n", g_CLI_PosIdx, min_length, prefix);
		g_CLI_PosIdx = min_length;
        strncpy(prefix, name_ptr, min_length);
        //strcpy(prefix, name_ptr);
        //pCli->Init.Write("g_CLI_PosIdx=%d, min_length:%d, %s\r\n", g_CLI_PosIdx, min_length, prefix);
    }
}

/**
  * @brief  Process tab ch, list current command dir.
  * @param  None.
  * @retval Always success(0).
  */
uint8_t CLI_Tab(CLI_HandleTypeDef *pCli)
{
#ifdef CLI_AUTO_COMPLETE
	pCli->Init.Write("\r\n");
    do_auto_complete(pCli, (char *)CLI_Buffer[g_CLI_CurrentIdx]);
	pCli->Init.Write("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
#else
//	CLI_PutString("%c%c", ASCII_CR, ASCII_LF);
	pCli->Init.Write("%c%c", ASCII_CR, ASCII_LF);
  //CLI_Execute(pCli, "ls");
  CLICmd_List(pCli);
	CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ASCII_NULL;
//	CLI_PutString("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
	pCli->Init.Write("%c%c%s", ASCII_CMD, ASCII_SPACE, CLI_Buffer[g_CLI_CurrentIdx]);
#endif
 
  return false;
}

/**
  * @brief  Push the rx data to command fifo.
  * @param  data byte.
  * @retval Push fifo success(0) or not(1). Fifo may overflow!
  */
uint8_t CLI_PushChar(CLI_HandleTypeDef *pCli, uint8_t ch)
{
  uint8_t pushFail;

  if(g_CLI_PosIdx < CLI_BUFFER_SIZE - 1)
  {
//    CLI_PutString("%c", ch);
		pCli->Init.Write("%c", ch);
	#if	ASCII_LEFT_SUPPORT	== true
		if(g_CLI_LeftPosIdx)
		{
			for(uint8_t pos=g_CLI_LeftPosIdx; pos > 0; pos--)
			{
				CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos+g_CLI_LeftPosIdx+1] = CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos+g_CLI_LeftPosIdx];
				CLI_PutString("%c", CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-pos+1]);
			}
			
			for(uint8_t i=0; i<g_CLI_LeftPosIdx; i++)
			{
				CLI_PutString("\x1b\x5b\x44");
			}
			
			CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx-g_CLI_LeftPosIdx] = ch;		
		}
		else
	#endif
		{
			CLI_Buffer[g_CLI_CurrentIdx][g_CLI_PosIdx] = ch;
		}
		g_CLI_PosIdx++;
    pushFail = false;
  }
  else
  {
//    CLI_PutString("%c", ASCII_BELL);
		pCli->Init.Write("%c", ASCII_BELL);
    pushFail = true;
  }
  return pushFail;
}

/**
  * @brief  Load cli history command.
  * @param  History up(1) or down(0).
  * @retval History load success(0) or not(1).
  */
uint8_t CLI_LoadHistory(CLI_HandleTypeDef *pCli, uint8_t historyUp)
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
//				CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
				pCli->Init.Write("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
      }
      g_CLI_HistoryIdx = next;
//      CLI_PutString("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			pCli->Init.Write("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			g_CLI_PosIdx = strlen((const char *)CLI_Buffer[g_CLI_HistoryIdx]);
			memcpy(CLI_Buffer[g_CLI_CurrentIdx], CLI_Buffer[g_CLI_HistoryIdx], g_CLI_PosIdx);	
    }
    else
    {
//			CLI_PutString("%c", ASCII_BELL);
			pCli->Init.Write("%c", ASCII_BELL);
      historyOk = false;
    }
  }
  else // history down.
  { // clean current line.
		for(tmp = 0; tmp < g_CLI_PosIdx; tmp++)
		{
//			CLI_PutString("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
			pCli->Init.Write("%c%c%c", ASCII_BS, ASCII_SPACE, ASCII_BS);
		}
    g_CLI_PosIdx = 0;
    next = (g_CLI_HistoryIdx + 1) % CLI_HISTORY_SIZE;
    if((next != g_CLI_CurrentIdx) && (g_CLI_HistoryIdx != g_CLI_CurrentIdx))
    {	// not null, switch to this history.
      g_CLI_HistoryIdx = next;	
//			CLI_PutString("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			pCli->Init.Write("%s", CLI_Buffer[g_CLI_HistoryIdx]);
			g_CLI_PosIdx = strlen((const char *)CLI_Buffer[g_CLI_HistoryIdx]);
			memcpy(CLI_Buffer[g_CLI_CurrentIdx], CLI_Buffer[g_CLI_HistoryIdx], g_CLI_PosIdx);	
    }
    else
    {		
      g_CLI_HistoryIdx = g_CLI_CurrentIdx;
//			CLI_PutString("%c", ASCII_BELL);
			pCli->Init.Write("%c", ASCII_BELL);
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
int16_t CLI_Handle(CLI_HandleTypeDef *pCli)
{	
	static CLI_State inputState = CLI_NORMAL;
	char pop_ch = 0;

	pop_ch = pCli->pCLIProcess(pCli, CLI_FUNC_TX_POP);
	
//	ring_queue_pop(&RingQueue, &pop_ch, 1);
	pop_ch = pCli->pCLIProcess(pCli, CLI_FUNC_RX_POP);	
	
	if(pop_ch != ASCII_NULL) {
//		CLI_PutString("0x%02x", pop_ch);
		switch(inputState) {
			case CLI_ESC: {	
				switch(pop_ch) {
					case ASCII_ARROW:
						inputState = CLI_ESC_ARROW;
						break;
					case ASCII_ESC:
						CLI_Cancel(pCli);
						inputState = CLI_NORMAL;
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
						CLI_LoadHistory(pCli, pop_ch == ASCII_UP);
						break;
					case ASCII_RIGHT:
						break;
					case ASCII_LEFT:
					#if	ASCII_LEFT_SUPPORT	== true	
						CLI_PutString("\x1b\x5b\x44");
						g_CLI_LeftPosIdx++;
					#else
						CLI_Backspace(pCli);
					#endif
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
						CLI_Cancel(pCli);
						break;
					}
					case ASCII_CR:
					case ASCII_LF: {
						CLI_Enter(pCli);		
						break;
					}
					case ASCII_BS:
					case ASCII_BS1: {
						CLI_Backspace(pCli);
						break;
					}
					case ASCII_ESC: {	// escape character, should translate to other char.
						inputState = CLI_ESC;
						break;
					}
					case ASCII_TAB: {
						CLI_Tab(pCli);
						break;
					}
					default: {
						CLI_PushChar(pCli, pop_ch);
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
	
#if 0
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
#else	
	if(strstr(pCh, "0x") != NULL || strstr(pCh, "0X") != NULL)
			sscanf(pCh, "%lx", &data);
	else
			sscanf(pCh, "%ld", &data);
#endif
	
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
#if 0
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
#else	
	float num = 0.0f;
	sscanf(pCh, "%f", &num);
	return num;
#endif
}

//FUNCTIONS
/*  ���Դ�Сд�ַ����Ա�  */


/**
  * @}
  */
/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/

