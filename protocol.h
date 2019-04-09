//################################################################################
//# Copyright Statement: CVTE
//# Copyright (C) 2014 Guangzhou Shiyuan Electronics Co.,Ltd. All rights reserved.
//#      ____________        _______________  ___________
//#     / / ________ \      / / _____   ____|| |  _______|
//#    / / /      \ \ \    / / /   | | |     | | |
//#   | | |        \ \ \  / / /    | | |     | | |_______
//#   | | |         \ \ \/ / /     | | |     | |  _______|
//#   | | |          \ \ \/ /      | | |     | | |
//#    \ \ \______    \ \  /       | | |     | | |_______
//#     \_\_______|    \_\/        |_|_|     |_|_________|
//#
//################################################################################

/**
 ******************************************************************************
 * @file    demo.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/***********************************************
                    include
***********************************************/
#include "ring_queue.h"

/***********************************************
                    define
***********************************************/
#define		PROT_RX_LEN_MAX					64

#define		PROT_DEBUG(fmt, ...)    do{ \
																			postToDebugLog("[%s]: " fmt ,  __FUNCTION__, ##__VA_ARGS__);\
																	}while(0)
															
/***********************************************
                    typedef
***********************************************/
typedef struct
{
	unsigned char 		 	length;
	unsigned short int 	cmdId;
	unsigned char 		 	boardId;
	unsigned char 			data[PROT_RX_LEN_MAX];
}ProtMsg_TypeDef;	

typedef enum {
	CMDID_NONE = 0,
	/* status */
	CMDID_HEART_BEAT 		= 0x1001,
	
	/* test cmd */
	CMDID_LED_V12 			= 0x2101,
	CMDID_LED_SET_COLOR 				,
	CMDID_KEY_STATE 						,
	CMDID_LED_SIGNAL 						,
	CMDID_READ_SENSOR_DATA			,
	
	
}E_CMD_ID;
/***********************************************
               function prototypes
***********************************************/
extern int protocolParse				(RingQueueTypedef *rq, ProtMsg_TypeDef *pProtMsg);
extern int protocolMsgDispatch	(void * sender, ProtMsg_TypeDef *pProtMsg);
extern int protocolMsgUpload		(void ** sender, uint8_t boardBit, uint8_t boardId, uint16_t cmdId, const char *format, ...);

/***********************************************
									  inline
***********************************************/

/***********************************************
                   variable
***********************************************/

/**
  * @}
  */

#endif  /* __PROTOCOL_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


