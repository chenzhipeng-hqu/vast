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
#include "vast_common.h"

/*************************************
              define
*************************************/

/**************************************
              typedef
**************************************/
typedef enum
{
	CLI_FUNC_INIT,
	CLI_FUNC_TX,
	CLI_FUNC_RX_PUSH,
	CLI_FUNC_RX_POP,
}CLI_SEL_FUNCx;

typedef struct _EasyKey_InitTypeDef
{
	int 	(*Read)	(uint8_t *pData, uint8_t len);
	int 	(*Write)	(const char *format, ... );
}CLI_InitTypeDef;

typedef struct _CLI_HandleTypeDef
{
	CLI_InitTypeDef	 		Init;
	uint8_t							(*pCLIProcess)		(struct _CLI_HandleTypeDef *pKey, CLI_SEL_FUNCx selFunc);

}CLI_HandleTypeDef;

typedef struct _CLICmdTypedef
{
  const char *cmd;
  const char *help;
  void (*pFun)(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
  const struct _CLICmdTypedef *child;
} CLICmdTypedef;

/*************************************
              variable
*************************************/
extern const CLICmdTypedef CLI_CmdTableMain[];

/*************************************
         function prototypes
*************************************/
int CLI_Initialize(CLI_HandleTypeDef *pCli);
//int CLI_Handle(void);
int CLI_Handle(CLI_HandleTypeDef *pCli);
void CLICmd_GotoTree(CLI_HandleTypeDef *pCli, int argc, char *argv[]);
uint32_t hex2u32(const char *str);
uint32_t str2u32(const char *str);
int32_t str2s32(const char *str);
float str2float(const char *str);
int findArgument(char *argvStr, char *argv[], char separationChar);

/**
  * @}
  */
#endif 

/************************ (C) COPYRIGHT chenzhipeng *****END OF FILE****/
