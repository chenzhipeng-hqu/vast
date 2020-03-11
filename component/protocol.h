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
//#include "ring_queue.h"
#include <stdint.h>
#include <stdbool.h>
#include <core/device.h>

/***********************************************
                    define
***********************************************/
#define		PROT_RX_LEN_MAX					0x7F

#define		PROT_DEBUG(fmt, ...)    do{ \
																			printf("[%s]: " fmt ,  __FUNCTION__, ##__VA_ARGS__);\
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
	CMDID_ATSX_STATUS 		= 0x1001,
	CMDID_TEST_RESULT,
	CMDID_HEART_BEAT,
	CMDID_SET_IP,
	CMDID_SET_MAC,
	CMDID_GET_VER,
	
	/* adc value */
	CMDID_V5_1_VALUE			= 0x2101,
	CMDID_V5_2_VALUE ,
	CMDID_V5_3_VALUE ,
	CMDID_V12_VALUE	,
	CMDID_AMP_VALUE	,
	CMDID_SOUND_P_VALUE	,
	CMDID_SOUND_N_VALUE	,
	
	/* PWM */
	CMDID_SET_PWM_FREQ		= 0x2201,
	CMDID_SET_PWM_DUTY ,
	CMDID_SET_LOGIC_PWM_FREQ ,
	CMDID_SET_LOGIC_PWM_DUTY ,
	CMDID_SET_LOGIC_PWM_INTERVAL ,
	
	/* communication */
	CMDID_CAN_COM					= 0x2301,
	CMDID_HPG_COM	, 				//high-pressure gauge
	CMDID_PM_COM	,					// power meter
	CMDID_ELOAD_COM	,   		// electronic load
	
	/* other */
	CMDID_IMP_TEST 				= 0x2401,	//impedance test
	CMDID_SHORT_V12 ,
	CMDID_SWITCH_V220 ,
	CMDID_CAP_DISCHARGE ,
	CMDID_CAP_CONNECT ,
	CMDID_BL_ON ,
	CMDID_PS_ON ,
	CMDID_ADJ ,
	CMDID_POWEROFF ,
	CMDID_V12_LOAD_SW ,
	CMDID_LED_RED_DET ,
	CMDID_GPIO_OUT		,
	CMDID_GPIO_IN			,
	
	/*  power meter */
	CMDID_POWER_FACTOR    	= 0x2601,
	
	/*  electronic load */
	CMDID_CHANNEL_NUM 			= 0x2710,
	CMDID_CH1_LOAD_MODE 		= 0x2711,
	CMDID_CH1_VO_VALUE,
	CMDID_CH1_IO_VALUE,
	CMDID_CH1_VON_VALUE,
	CMDID_CH1_RD_VALUE,
	CMDID_CH1_RC_VALUE,
	CMDID_CH1_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH1_VON_LATCH,
	CMDID_CH1_SET_V_MAX,
	CMDID_CH1_SET_V_MIN,
	CMDID_CH1_SET_I_MAX,
	CMDID_CH1_SET_I_MIN,
	CMDID_CH2_LOAD_MODE 		= 0x2721,
	CMDID_CH2_VO_VALUE,
	CMDID_CH2_IO_VALUE,
	CMDID_CH2_VON_VALUE,
	CMDID_CH2_RD_VALUE,
	CMDID_CH2_RC_VALUE,
	CMDID_CH2_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH2_VON_LATCH,
	CMDID_CH2_SET_V_MAX,
	CMDID_CH2_SET_V_MIN,
	CMDID_CH2_SET_I_MAX,
	CMDID_CH2_SET_I_MIN,
	CMDID_CH3_LOAD_MODE 		= 0x2731,
	CMDID_CH3_VO_VALUE,
	CMDID_CH3_IO_VALUE,
	CMDID_CH3_VON_VALUE,
	CMDID_CH3_RD_VALUE,
	CMDID_CH3_RC_VALUE,
	CMDID_CH3_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH3_VON_LATCH,
	CMDID_CH3_SET_V_MAX,
	CMDID_CH3_SET_V_MIN,
	CMDID_CH3_SET_I_MAX,
	CMDID_CH3_SET_I_MIN,
	CMDID_CH4_LOAD_MODE 		= 0x2741,
	CMDID_CH4_VO_VALUE,
	CMDID_CH4_IO_VALUE,
	CMDID_CH4_VON_VALUE,
	CMDID_CH4_RD_VALUE,
	CMDID_CH4_RC_VALUE,
	CMDID_CH4_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH4_VON_LATCH,
	CMDID_CH4_SET_V_MAX,
	CMDID_CH4_SET_V_MIN,
	CMDID_CH4_SET_I_MAX,
	CMDID_CH4_SET_I_MIN,
	CMDID_CH5_LOAD_MODE 		= 0x2751,
	CMDID_CH5_VO_VALUE,
	CMDID_CH5_IO_VALUE,
	CMDID_CH5_VON_VALUE,
	CMDID_CH5_RD_VALUE,
	CMDID_CH5_RC_VALUE,
	CMDID_CH5_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH5_VON_LATCH,
	CMDID_CH5_SET_V_MAX,
	CMDID_CH5_SET_V_MIN,
	CMDID_CH5_SET_I_MAX,
	CMDID_CH5_SET_I_MIN,
	CMDID_CH6_LOAD_MODE 		= 0x2761,
	CMDID_CH6_VO_VALUE,
	CMDID_CH6_IO_VALUE,
	CMDID_CH6_VON_VALUE,
	CMDID_CH6_RD_VALUE,
	CMDID_CH6_RC_VALUE,
	CMDID_CH6_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH6_VON_LATCH,
	CMDID_CH6_SET_V_MAX,
	CMDID_CH6_SET_V_MIN,
	CMDID_CH6_SET_I_MAX,
	CMDID_CH6_SET_I_MIN,
	CMDID_CH7_LOAD_MODE 		= 0x2771,
	CMDID_CH7_VO_VALUE,
	CMDID_CH7_IO_VALUE,
	CMDID_CH7_VON_VALUE,
	CMDID_CH7_RD_VALUE,
	CMDID_CH7_RC_VALUE,
	CMDID_CH7_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH7_VON_LATCH,
	CMDID_CH7_SET_V_MAX,
	CMDID_CH7_SET_V_MIN,
	CMDID_CH7_SET_I_MAX,
	CMDID_CH7_SET_I_MIN,
	CMDID_CH8_LOAD_MODE 		= 0x2781,
	CMDID_CH8_VO_VALUE,
	CMDID_CH8_IO_VALUE,
	CMDID_CH8_VON_VALUE,
	CMDID_CH8_RD_VALUE,
	CMDID_CH8_RC_VALUE,
	CMDID_CH8_INPUT_LINE_IMPEDANCE_VALUE,
	CMDID_CH8_VON_LATCH,
	CMDID_CH8_SET_V_MAX,
	CMDID_CH8_SET_V_MIN,
	CMDID_CH8_SET_I_MAX,
	CMDID_CH8_SET_I_MIN,
	CMDID_READ_V					 	= 0x27D0,
	CMDID_READ_I,
	
	/* led electronic load */
	CMDID_LED_CHANNEL_NUM		= 0x2810,
	CMDID_CH1_LED_NUM				= 0x2811,
	CMDID_CH1_LED_SET_V_MAX,
	CMDID_CH1_LED_SET_V_MIN,
	CMDID_CH1_LED_SET_I_MAX,
	CMDID_CH1_LED_SET_I_MIN,
	CMDID_CH2_LED_NUM				= 0x2821,
	CMDID_CH2_LED_SET_V_MAX,
	CMDID_CH2_LED_SET_V_MIN,
	CMDID_CH2_LED_SET_I_MAX,
	CMDID_CH2_LED_SET_I_MIN,
	CMDID_CH3_LED_NUM				= 0x2831,
	CMDID_CH3_LED_SET_V_MAX,
	CMDID_CH3_LED_SET_V_MIN,
	CMDID_CH3_LED_SET_I_MAX,
	CMDID_CH3_LED_SET_I_MIN,
	CMDID_CH4_LED_NUM				= 0x2841,
	CMDID_CH4_LED_SET_V_MAX,
	CMDID_CH4_LED_SET_V_MIN,
	CMDID_CH4_LED_SET_I_MAX,
	CMDID_CH4_LED_SET_I_MIN,
	CMDID_CH5_LED_NUM				= 0x2851,
	CMDID_CH5_LED_SET_V_MAX,
	CMDID_CH5_LED_SET_V_MIN,
	CMDID_CH5_LED_SET_I_MAX,
	CMDID_CH5_LED_SET_I_MIN,
	CMDID_CH6_LED_NUM				= 0x2861,
	CMDID_CH6_LED_SET_V_MAX,
	CMDID_CH6_LED_SET_V_MIN,
	CMDID_CH6_LED_SET_I_MAX,
	CMDID_CH6_LED_SET_I_MIN,
	CMDID_CH7_LED_NUM				= 0x2871,
	CMDID_CH7_LED_SET_V_MAX,
	CMDID_CH7_LED_SET_V_MIN,
	CMDID_CH7_LED_SET_I_MAX,
	CMDID_CH7_LED_SET_I_MIN,
	CMDID_CH8_LED_NUM				= 0x2881,
	CMDID_CH8_LED_SET_V_MAX,
	CMDID_CH8_LED_SET_V_MIN,
	CMDID_CH8_LED_SET_I_MAX,
	CMDID_CH8_LED_SET_I_MIN,
	CMDID_LED_READ_V				= 0x28D0,
	CMDID_LED_READ_I,
	
}E_CMD_ID;

#pragma pack(1)
typedef struct SmartFrame
{
	uint8_t stc;
    uint32_t from;
    uint32_t to;
    uint16_t seq; //最高位为1表示本地帧,
	uint8_t len;  // data 里面的长度
	uint8_t data[1];
} smart_frame_t;
#define SMART_FRAME_HEAD (offset_of(smart_frame_t, data))

typedef struct ListFrame
{
    list_t 	        entry;
	rx_indicate     rx_ind;
	tx_complete     tx_done;
	//uint8_t         len;        // data 里面的长度
	uint8_t         data[1];
} list_frame_t;
#define LIST_FRAME_HEAD (offset_of(list_frame_t, data))

#pragma pack()
/***********************************************
               function prototypes
***********************************************/
//extern int protocolParse				(RingQueueTypedef *rq, ProtMsg_TypeDef *pProtMsg);
extern int protocolMsgDispatch	(void * sender, ProtMsg_TypeDef *pProtMsg);
extern int protocolMsgUpload		(void * sender, ProtMsg_TypeDef *pProtMsg);
extern int protocolMsgUpload2		(void ** sender, unsigned char boardId, unsigned short int cmdId, const char *format, ...);
extern int protocolMsgUpload3(void ** sender, unsigned char boardId, unsigned short int cmdId, const char *format, ...);

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


