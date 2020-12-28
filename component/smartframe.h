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
#ifndef __SMARTFRAME_H__
#define __SMARTFRAME_H__

/***********************************************
                    include
***********************************************/
#include <stdint.h>
#include <stdbool.h>
#include <core/device.h>

/***********************************************
                    define
***********************************************/
#define		REMOTE_STC					    0xFF
#define		LOCAL_STC					    0xFE
#define		PROT_RX_LEN_MAX					0x7F

//#define		PROT_DEBUG(fmt, ...)            
															
/***********************************************
                    typedef
***********************************************/
#pragma pack(1)
typedef struct RemoteFrame
{
	uint8_t     stc;
    uint8_t     from;
    uint8_t     to;
    uint8_t     seq;
	uint16_t    len;  // 全长
    uint8_t     cmd;
	uint8_t     data[1];
} remote_frame_t;
#define REMOTE_FRAME_HEAD (offsetof(remote_frame_t, data))

typedef struct LocalFrame
{
	//uint8_t     stc;
    uint32_t    from;
    uint32_t    to;
    //uint8_t     seq; 
	uint8_t    len;  //全长
    uint8_t     cmd;
	uint8_t     data[1];
} local_frame_t;
#define LOCAL_FRAME_HEAD (offsetof(local_frame_t, data))

typedef struct ListFrame
{
    list_t 	        entry;
    rx_indicate     rx_ind;
    tx_complete     tx_done;
    uint8_t         len;        // data 里面的长度
	uint8_t         data[1];
} list_frame_t;
#define LIST_FRAME_HEAD (offsetof(list_frame_t, data))

#pragma pack()

/***********************************************
               function prototypes
***********************************************/
extern remote_frame_t *get_remote_frame(const uint8_t *data, uint32_t len);
extern int code_remote_frame(const uint8_t from, const uint8_t to, int seq, uint8_t cmd,
                                uint8_t *data, int len, void *out, int maxlen);

extern int code_local_frame(const uint32_t from, const uint32_t to, int seq, uint8_t cmd,
                                uint8_t *data, int len, void *out, int maxlen);

/***********************************************
                    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

/**
  * @}
  */

#endif  /* __SMARTFRAME_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


