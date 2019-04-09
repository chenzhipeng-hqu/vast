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
#ifndef __CLI_H_
#define __CLI_H_

/*************************************
              include
*************************************/
#include <stdint.h>
#include <stdbool.h>

/*************************************
              define
*************************************/
#define 	CLI_PutString(fmt, ...)			do{\
																				if(echoEnable) \
																					postToDebugLog(fmt, ##__VA_ARGS__);\
																			}while(0)

/**************************************
              typedef
**************************************/
typedef struct _CLICmdTypedef
{
  const char *cmd;
  const char *help;
  void (*pFun)(int argc, char *argv[]);
  const struct _CLICmdTypedef *child;
} CLICmdTypedef;

/*************************************
              variable
*************************************/
extern const CLICmdTypedef CLI_CmdTableMain[];
extern uint8_t echoEnable;
/*************************************
         function prototypes
*************************************/
int CLI_Initialize(void);
int CLI_Handle(void);
void CLICmd_GotoTree(int argc, char *argv[]);
uint32_t hex2u32(const char *str);
uint32_t str2u32(const char *str);
int32_t str2s32(const char *str);
float str2float(const char *str);

/**
  * @}
  */
#endif 

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/
