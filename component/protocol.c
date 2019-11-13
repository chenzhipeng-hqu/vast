/**
 ******************************************************************************
 * @file    demo.c
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

/***********************************************
                    include
***********************************************/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "component/vast_cli.h"
//#include "cmsis_os.h"
#include "qpc_common.h"
#include "device/cp8102.h"
#include "device/cp8113.h"
#include "device/im1227.h"
#include "driver/modbus_rtu.h"
//#include "czp_debug.h"
#include "component/protocol.h"

/***********************************************
                    define
***********************************************/

#define		PROT_RX_HEAD0			0xFF
#define		PROT_RX_HEAD1			0x22

#define		PROT_TX_HEAD0			0xFF
#define		PROT_TX_HEAD1			0x33

#define		PROT_LEN_MIN			0x03
#define		PROT_TAIL					0xFE

/***********************************************
                    typedef
***********************************************/
/** Definitions for error constants. */
enum {
  ERR_OK        = 0,
  ERR_TIMEOUT  	= -1,
  ERR_HEAD0    	= -2,
  ERR_HEAD1    	= -3,
  ERR_LENGTH    = -4,
  ERR_CMD_ID    = -5,
  ERR_BOARD_ID  = -6,
  ERR_CHECK_XOR = -7,
  ERR_TAIL 			= -8,
  ERR_CMDID			= -9,
  ERR_SENDER		= -10,
};

/***********************************************
               function prototypes
***********************************************/


/***********************************************
                   variable
***********************************************/


/***********************************************
                   function
***********************************************/
/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static inline int checkHead0(unsigned char *pBuf)
{
	int ret = ERR_OK;
	
	if (PROT_RX_HEAD0 != pBuf[0])
	{
		ret = ERR_HEAD0;
	}
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static inline int checkHead1(unsigned char *pBuf)
{
	int ret = ERR_OK;
	
	if (PROT_RX_HEAD1 != pBuf[1])
	{
		ret = ERR_HEAD1;
	}
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline getLength(unsigned char *pSrcBuf, unsigned char *plen)
{
	int ret = ERR_OK;
	
	if ((pSrcBuf[2] < PROT_LEN_MIN) 
		| (pSrcBuf[2] > PROT_RX_LEN_MAX))  
	{
		PROT_DEBUG("protocol length is error: 0x%02x \r\n", pSrcBuf[pSrcBuf[2]]);
		ret = ERR_LENGTH;
	}
	else
	{		
		*plen = pSrcBuf[2];
	}
		
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline checkTail(unsigned char *pSrcBuf, unsigned char len)
{
	int ret = ERR_OK;
	uint8_t i;
	
	if(pSrcBuf[len+4] != PROT_TAIL) //add head0, head1, len, check_xor
	{
		PROT_DEBUG("protocol tail is error: 0x%02x \r\n", pSrcBuf[len+4]);
		
		for(i=0; i<len+5; i++)
		{
			DBG_INFO(DBG_ERROR, "%02x ", pSrcBuf[i]);
		}
		
		DBG_INFO(DBG_ERROR, "\r\n");
		
		ret = ERR_TAIL;
	}
		
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline getCmdId(unsigned char *pSrcBuf, unsigned short int *pCmdId)
{
	int ret = ERR_OK;
	
	if ((pSrcBuf[3] == 0x00) 
		| (pSrcBuf[4] == 0x00))
	{
		PROT_DEBUG("protocol cmd_id is error: 0x%04x\r\n", pSrcBuf[3]<<8 | pSrcBuf[4]);
		ret = ERR_CMD_ID;
	}
	else
	{		
		*pCmdId = pSrcBuf[3]<<8 | pSrcBuf[4];
	}
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline getBoardId(unsigned char *pSrcBuf, unsigned char *pBoardId)
{
	int ret = ERR_OK;
	
	if (pSrcBuf[5] == 0x00)
	{
		PROT_DEBUG("protocol board_id is error: 0x%02x\r\n", pSrcBuf[5]);
		ret = ERR_BOARD_ID;
	}
	else
	{		
		*pBoardId = pSrcBuf[5];
	}
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline checkXor(unsigned char *pSrcBuf)
{
	int ret = ERR_OK;
	unsigned char check_xor, i;
	unsigned short int length;
	
	check_xor = pSrcBuf[0];
	length = pSrcBuf[2] + 3;  //head0, head1, len
	
	for(i=1; i<length; i++)
	{
		check_xor ^= pSrcBuf[i];
	}
	
	if (check_xor != pSrcBuf[i])
	{
		PROT_DEBUG("protocol checkXor is error: calculate:0x%02x, get:0x%02x\r\n", check_xor, pSrcBuf[i]);
		ret = ERR_CHECK_XOR;
	}
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
static int inline getData(unsigned char *pSrcBuf, unsigned char *pDestBuf)
{
	int ret = ERR_OK;
	unsigned char i;
	unsigned short int length;
	
	length = pSrcBuf[2]-3;  //
	
	for(i=0; i<length; i++)
	{
		pDestBuf[i] = pSrcBuf[i+PROT_LEN_MIN+3];  //head0, head1, len, 
	}
	
	pDestBuf[i] = 0;
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
//int protocolParse(RingQueueTypedef *rq, ProtMsg_TypeDef *pProtMsg)
//{
	//int ret = ERR_OK, size_cnt;
	//uint8_t *pSrcBuf = pProtMsg->data;
	
	//ring_queue_pop(rq, (char *)&pSrcBuf[0], 1);
	
	//if (ERR_OK != (ret = checkHead0(pSrcBuf)))
	//{
		//return ret;
	//}
	
	//size_cnt = ring_queue_find_block(rq, (char *)&pSrcBuf[1], rq->head, (rq->head+2)%rq->length);
	
	//if (ERR_OK != (ret = checkHead1(pSrcBuf)))
	//{
		//return ret;
	//}
			
	//if (ERR_OK != (ret = getLength(pSrcBuf, &pProtMsg->length)))
	//{
		//return ret;
	//}	
				
	//rq->head = (rq->head+size_cnt)%rq->length;
	
	//size_cnt = ring_queue_find_block(rq, (char *)&pSrcBuf[3], rq->head, (rq->head+pProtMsg->length+2)%rq->length);
			
	//if (ERR_OK != (ret = checkTail(pSrcBuf, pProtMsg->length)))
	//{
		//return ret;
	//}	
	
	//if (ERR_OK != (ret = getCmdId(pSrcBuf, &pProtMsg->cmdId)))
	//{
		//return ret;
	//}	
	
	//if (ERR_OK != (ret = getBoardId(pSrcBuf, &pProtMsg->boardId)))
	//{
		//return ret;
	//}	
	
	//if (ERR_OK != (ret = checkXor(pSrcBuf)))
	//{
		//return ret;
	//}
	
	//if ( (pProtMsg->length > PROT_LEN_MIN) & (ERR_OK != (ret = getData(pSrcBuf, pProtMsg->data))))
	//{
		//return ret;
	//}
	
	//rq->head = (rq->head+size_cnt)%rq->length; 
	
	//return ret;
//}

/**
  * @brief  protocolMsgDispatch.
  * @param  
  * @retval 
  */
int protocolMsgDispatch(void * sender, ProtMsg_TypeDef *pProtMsg)
{
	int 			ret = ERR_OK;
	uint8_t 	board_id = pProtMsg->boardId-1;
	uint8_t 	socket_id = pProtMsg->boardId-1;
	uint16_t  cmd_id = pProtMsg->cmdId;
	QActive 	*sender_ = (QActive *)sender;
	//QActive 	*sender_ = (QActive *)AO_Ethernet;
	int argc = 0;
	char *argv[5] = {0};
	
	ASSERT(board_id < BOARD_MAX, 
					DBG_LOG(DBG_ERROR, "board_id=%d, out the range of BOARD_x\r\n", board_id); 
					return 0
				);
	
	argc = findArgument((char *)pProtMsg->data, argv, ' ');
	
	DBG_INFO(DBG_DEBUG, "argv: ");
	
	for(uint8_t argcCnt=0; argcCnt<argc; argcCnt++)
	{
		DBG_INFO(DBG_DEBUG, "%s ", argv[argcCnt]);
	}
	
	DBG_INFO(DBG_DEBUG, "\r\n");
					
	switch(cmd_id)
	{	
		case CMDID_ATSX_STATUS :
		{
			AtsxEvt * const pe = Q_NEW(AtsxEvt, GET_BOARD_STATUS_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);	
			break;
		}	
		case CMDID_TEST_RESULT :
		{
//			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_PASS_SIG);
//			pe->pinState = strcmp((const char *)pProtMsg->data, "pass");
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			QACTIVE_POST(AO_Board[board_id], &pe->super, me);	
			
			if(strcmp((const char *)pProtMsg->data, "pass") ? 0 : 1)
			{	
				AtsxEvt * const pe = Q_NEW(AtsxEvt, BOARD_PASS_SIG);
				pe->sender = sender_;
				pe->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe->super, me);			
			}
			else if(strcmp((const char *)pProtMsg->data, "ng") ? 0 : 1)
			{
				AtsxEvt * const pe2 = Q_NEW(AtsxEvt, BOARD_NG_SIG);
				pe2->sender = sender_;
				pe2->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe2->super, me);		
			}
			else
			{
				AtsxEvt * const pe2 = Q_NEW(AtsxEvt, BOARD_RESULT_RESET_SIG);
				pe2->sender = sender_;
				pe2->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe2->super, me);		
			}
			
			break;
		}	
		case CMDID_HEART_BEAT :
		{			
			AtsxEvt * const pe = Q_NEW(AtsxEvt, SOCK_HEART_HEAT_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			//QACTIVE_POST(AO_Ethernet, &pe->super, me);
			break;
		}	
		case CMDID_SET_IP :
		{			
			IpEvt * const pe = Q_NEW(IpEvt, SET_IP_SIG);					
			uint8_t idx=0, addrIndex=0;
			
			memset(pe->addr, 0, sizeof(uint8_t)*4);
			
			for(idx=0; idx<strlen((const char *)pProtMsg->data); idx++)
			{
				if(pProtMsg->data[idx] == '.')
				{
					addrIndex++;
				}
				else if(pProtMsg->data[idx] == 0)
				{
					break;
				}
				else
				{
					pe->addr[addrIndex] = pProtMsg->data[idx] - '0' + pe->addr[addrIndex] * 10;
				}	
			}
			
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			//QACTIVE_POST(AO_Ethernet, &pe->super, me);
			break;
		}	
		case CMDID_SET_MAC :
		{			
			MacEvt * const pe = Q_NEW(MacEvt, SET_MAC_SIG);					
			uint8_t idx=0, addrIndex=0;
			
			memset(pe->addr, 0, sizeof(uint8_t)*6);
			
			for(idx=0; idx<strlen((const char *)pProtMsg->data); idx++)
			{
				if(pProtMsg->data[idx] == ':')
				{
					
					addrIndex++;
				}
				else if(pProtMsg->data[idx] == 0)
				{
					break;
				}
				else
				{
					if(pProtMsg->data[idx] >= 'a' && pProtMsg->data[idx] <= 'f')
					{
						pe->addr[addrIndex] <<= 4;
						pe->addr[addrIndex] += pProtMsg->data[idx] - 'a' + 10;
					}
					else if(pProtMsg->data[idx] >= 'A' && pProtMsg->data[idx] <= 'F')
					{
						pe->addr[addrIndex] <<= 4;
						pe->addr[addrIndex] += pProtMsg->data[idx] - 'A' + 10;
					}
					else if(pProtMsg->data[idx] >= '0' && pProtMsg->data[idx] <= '9')
					{
						pe->addr[addrIndex] <<= 4;
						pe->addr[addrIndex] += pProtMsg->data[idx] - '0';
					}
					//pe->addr[addrIndex] = pProtMsg->data[idx] - '0' + pe->addr[addrIndex] * 10;
				}	
			}
			
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			//QACTIVE_POST(AO_Ethernet, &pe->super, me);
			break;
		}	
		case CMDID_GET_VER:
		{
			//protocolMsgUpload2((void *)&AO_Ethernet, board_id, cmd_id, "V%d.%d.%d:[%s %s]", VERSION[0], VERSION[1], VERSION[2], buildDate, buildTime);
			break;
		}	
		case CMDID_V5_1_VALUE:
		case CMDID_V5_2_VALUE:
		case CMDID_V5_3_VALUE:
		case CMDID_V12_VALUE:
		case CMDID_AMP_VALUE:
		case CMDID_SOUND_P_VALUE:
		case CMDID_SOUND_N_VALUE:
		{
			AdcEvt * const pe = Q_NEW(AdcEvt, GET_ADC_VALUE_SIG);
			pe->get_adc_chx = (BOARD_ADC_CH)(cmd_id - CMDID_V5_1_VALUE);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);	
			break;
		}
		case CMDID_SET_PWM_FREQ :
		{
			PwmEvt * const pe = Q_NEW(PwmEvt, SET_PWM_FREQ_SIG);
			pe->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_PWM_FREQ + BOARD_PWM);
			pe->freq = str2u32((const char *)pProtMsg->data);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
//			postToDebugLog("freq: %d, pwmNum: %x, board: %d.\r\n", pe->freq, pe->pwm_num, board_id);
					
			break;
		}
		case CMDID_SET_LOGIC_PWM_FREQ :
		{
			PwmEvt * const pe1 = Q_NEW(PwmEvt, SET_PWM_FREQ_SIG);
			pe1->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_FREQ + LOGIC_PWM1);
			pe1->freq = str2u32((const char *)pProtMsg->data);
			pe1->sender = NULL;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe1->super);
			
			PwmEvt * const pe2 = Q_NEW(PwmEvt, SET_PWM_FREQ_SIG);
			pe2->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_FREQ + LOGIC_PWM2);
			pe2->freq = str2u32((const char *)pProtMsg->data);
			pe2->sender = NULL;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe2->super);
			
			PwmEvt * const pe3 = Q_NEW(PwmEvt, SET_PWM_FREQ_SIG);
			pe3->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_FREQ + LOGIC_PWM3);
			pe3->freq = str2u32((const char *)pProtMsg->data);
			pe3->sender = NULL;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe3->super);
			
			PwmEvt * const pe4 = Q_NEW(PwmEvt, SET_PWM_FREQ_SIG);
			pe4->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_FREQ + LOGIC_PWM4);
			pe4->freq = str2u32((const char *)pProtMsg->data);
			pe4->sender = sender_;
			pe4->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe4->super, me);
			break;
		}
		case CMDID_SET_PWM_DUTY :
		{
			uint16_t duty = str2u32((const char *)pProtMsg->data) * 10;
			
			PwmEvt * const pe = Q_NEW(PwmEvt, SET_PWM_DUTY_SIG);
			pe->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_PWM_DUTY);
			pe->duty = duty;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			break;
		}
		case CMDID_SET_LOGIC_PWM_DUTY :
		{	
			uint16_t duty = str2u32((const char *)pProtMsg->data) * 10;
			
			PwmEvt * const pe1 = Q_NEW(PwmEvt, SET_PWM_DUTY_SIG);
			pe1->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_DUTY + LOGIC_PWM1);
			pe1->duty = duty;
			pe1->sender = NULL;
			pe1->cmd_id = cmd_id;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe1->super);
			
			PwmEvt * const pe2 = Q_NEW(PwmEvt, SET_PWM_DUTY_SIG);
			pe2->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_DUTY + LOGIC_PWM2);
			pe2->duty = duty;
			pe2->cmd_id = cmd_id;
			pe2->sender = NULL;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe2->super);
			
			PwmEvt * const pe3 = Q_NEW(PwmEvt, SET_PWM_DUTY_SIG);
			pe3->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_DUTY + LOGIC_PWM3);
			pe3->duty = duty;
			pe3->cmd_id = cmd_id;
			pe3->sender = NULL;
			QACTIVE_POST_LIFO(AO_Board[board_id], &pe3->super);
			
			PwmEvt * const pe4 = Q_NEW(PwmEvt, SET_PWM_DUTY_SIG);
			pe4->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_DUTY + LOGIC_PWM4);
			pe4->duty = duty;
			pe4->sender = sender_;
			pe4->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe4->super, me);
			
			break;
		}
		case CMDID_SET_LOGIC_PWM_INTERVAL :
		{	
			PwmEvt * const pe = Q_NEW(PwmEvt, SET_LOGIC_PWM_INTERVAL);
			pe->pwm_num = (BOARD_PWM_CH)(cmd_id - CMDID_SET_LOGIC_PWM_INTERVAL + LOGIC_PWM1);
			pe->logicInterval = str2u32((const char *)pProtMsg->data);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_SWITCH_V220 :
		{
			if(strcmp((const char *)pProtMsg->data, "on") ? 0 : 1)
			{	
				AtsxEvt * const pe = Q_NEW(AtsxEvt, PCT_220V_ON_SIG);
				pe->sender = sender_;
				pe->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe->super, me);			
			}
			else if(strcmp((const char *)pProtMsg->data, "half") ? 0 : 1)
			{
				AtsxEvt * const pe2 = Q_NEW(AtsxEvt, PCT_220V_P_ON_SIG);
				pe2->sender = sender_;
				pe2->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe2->super, me);		
			}
			else
			{
				AtsxEvt * const pe2 = Q_NEW(AtsxEvt, PCT_220V_OFF_SIG);
				pe2->sender = sender_;
				pe2->cmd_id = cmd_id;
				QACTIVE_POST(AO_Board[board_id], &pe2->super, me);		
			}
			
			break;
		}
		case CMDID_CAP_DISCHARGE :
		{
			AtsxEvt * const pe = Q_NEW(AtsxEvt, PCT_CAP_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_CAP_CONNECT :
		{
			AtsxEvt * const pe = Q_NEW(AtsxEvt, GET_CAP_CONNECT_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_IMP_TEST :
		{
			break;
		}
		case CMDID_SHORT_V12 :
		{
			ScpEvt * const pe = Q_NEW(ScpEvt, BOARD_SHORT_V12_SIG);
			pe->timeout = str2u32((const char *)pProtMsg->data);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
//			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_SHORT_V12_SIG);
//			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_BL_ON :
		{
			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_BLON_SIG);
			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_PS_ON :
		{
			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_PSON_SIG);
			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_ADJ :
		{
			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_ADJ_SIG);
			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_POWEROFF :
		{
			PowerOffEvt * const pe = Q_NEW(PowerOffEvt, BOARD_POWEROFF_SIG);
			pe->timeout = str2u32((const char *)pProtMsg->data);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_V12_LOAD_SW :
		{
			GpioEvt * const pe = Q_NEW(GpioEvt, BOARD_V12_SW_SIG);
			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_LED_RED_DET :
		{
			LedRedEvt * const pe = Q_NEW(LedRedEvt, BOARD_AGING_STA_SIG);
			pe->times = str2u32((const char *)pProtMsg->data);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);
			
			break;
		}
		case CMDID_GPIO_OUT :
		{
			GpioEvt * pe = Q_NEW(GpioEvt, GPIO_OUTPUT_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->pinId = str2u32(argv[0]);		
			pe->pinState = strcmp(argv[1], "on") ? 0 : 1;
			
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);						
			
			break;
		}
		case CMDID_GPIO_IN :
		{
			GpioEvt * pe = Q_NEW(GpioEvt, GPIO_INPUT_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			
			QACTIVE_POST(AO_Board[board_id], &pe->super, me);	
			
			break;
		}
		case CMDID_POWER_FACTOR :
		{
			ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_HOLD_REG_SIG);

			pe->slave_addr = PM_ADDR_A | (board_id << 4);
			pe->start_addr = IM1227_PF;
			pe->reg_count  = 1;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
            QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			break;
		}
		case CMDID_CHANNEL_NUM :
		{
//			uint8_t i, bit = 0x01, ch_nums = str2u32((const char *)pProtMsg->data);
//			uint16_t channels = 0;
//			
//			for (i=0; i<ch_nums; i++)
//			{
//				channels |= bit;
//				bit <<= 1;
//			}	
		#if 0
			uint16_t reg_value = pProtMsg->data[0];
			
			if((reg_value > 0x0f) | (reg_value == 0))
			{
				ModbusEvt * const pe2 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
				pe2->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
				pe2->start_addr = CP8102_CHANNEL_SWITCH;
				pe2->reg_value = (reg_value>>4) & 0x0f;
				pe2->sender = NULL;
				QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			}
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_SWITCH;
			pe->reg_value = reg_value & 0x0f;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
		#else
			
			uint16_t is8ch = pProtMsg->data[0];
			uint16_t reg_value = pProtMsg->data[2];
			
			if(is8ch == 8)
			{
				ModbusEvt * const pe2 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
				pe2->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
				pe2->start_addr = CP8102_CHANNEL_SWITCH;
				pe2->reg_value = (reg_value>>4) & 0x0f;
				pe2->sender = NULL;
				QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			}
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_SWITCH;
			pe->reg_value = reg_value & 0x0f;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
		#endif
			break;
		}
		case CMDID_CH1_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LOAD_MODE :
		{
			uint16_t reg_value = pProtMsg->data[0] - 1;
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_MODE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_VO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);;
			pe->start_addr = CP8102_CHANNEL_4_VO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_IO_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_IO;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_VON_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_VON;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_RD_VALUE :
		{
			uint16_t reg_value = float2int(str2float((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_RD;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_RC_VALUE :
		{
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_RC;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_INPUT_LINE_IMPEDANCE_VALUE :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_INPUT_LINE_IMPEDANCE;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_VON_LATCH :
		{			
			uint16_t reg_value = str2u32((const char *)pProtMsg->data);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_VonLatch;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_1_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_2_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_3_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_SET_I_MIN :
		{			
			
//			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
//			pe2->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
//			pe2->start_addr = CP8102_CHANNEL_SWITCH;
//			pe2->reg_value = 0x0f;
//			pe2->sender = NULL;
//			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8102_CHANNEL_4_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_1_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_2_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_3_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_SET_I_MIN :
		{						
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8102_CHANNEL_4_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_READ_V :
		{						
//			ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
//			
//			pe->start_addr = CP8102_CHANNEL_1_V_INPUT + (((pProtMsg->data[0]-1)&0x03) * 3); // one channel have 3 input registers 
//	
//			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
//			pe->reg_count = 1;
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			pe->socket_n = socket_id;
//			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
			
			if(pProtMsg->data[0] < 5)
			{
				pe2->start_addr = CP8102_CHANNEL_1_V_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			}
			else
			{
				pe2->start_addr = CP8102_CHANNEL_1_V_INPUT + ((pProtMsg->data[0]-4-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			}
			
			pe2->reg_count = 1;
			pe2->sender = sender_;
			pe2->cmd_id = cmd_id;
			pe2->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			break;
		}
		case CMDID_READ_I :
		{			
//			ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
//			
//			pe->start_addr = CP8102_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 

//			pe->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
//			pe->reg_count = 1;
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			pe->socket_n = socket_id;
//			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
			
			if(pProtMsg->data[0] < 5)
			{
				pe2->start_addr = CP8102_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = ELOAD_ADDR_A  | (board_id << 4);
			}
			else
			{
				pe2->start_addr = CP8102_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-4-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = ELOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			}
			
			pe2->reg_count = 1;
			pe2->sender = sender_;
			pe2->cmd_id = cmd_id;
			pe2->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			break;
		}
		case CMDID_LED_CHANNEL_NUM :
		{	
		#if 0     // set cp8113 judge output
			uint16_t reg_value = 0x03;
			
			ModbusEvt * const pe0 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe0->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe0->start_addr = CP8113_CHANNEL_1_JUDGE_OUTPUT;
			pe0->reg_value = reg_value;
			pe0->sender = NULL;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe0->super, me);
			
			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe2->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe2->start_addr = CP8113_CHANNEL_2_JUDGE_OUTPUT;
			pe2->reg_value = reg_value;
			pe2->sender = NULL;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			ModbusEvt * const pe3 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe3->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe3->start_addr = CP8113_CHANNEL_3_JUDGE_OUTPUT;
			pe3->reg_value = reg_value;
			pe3->sender = NULL;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe3->super, me);
			
			ModbusEvt * const pe4 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe4->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe4->start_addr = CP8113_CHANNEL_4_JUDGE_OUTPUT;
			pe4->reg_value = reg_value;
			pe4->sender = NULL;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe4->super, me);
		#endif
		
//			uint8_t i, bit = 0x01, ch_nums = str2u32((const char *)pProtMsg->data);
//			uint16_t channels = 0;
//			
//			for (i=0; i<ch_nums; i++)
//			{
//				channels |= bit;
//				bit <<= 1;
//			}
					
			uint16_t reg_value = pProtMsg->data[0];
			
			if((reg_value > 0x0f) | (reg_value == 0x00))
			{
				ModbusEvt * const pe2 = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
				pe2->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
				pe2->start_addr = CP8113_CHANNEL_SWITCH;
				pe2->reg_value = (reg_value>>4) & 0x0f;
				pe2->sender = NULL;
				pe2->cmd_id = cmd_id;
				pe2->socket_n = socket_id;
				QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			}			
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_SWITCH;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_1_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_2_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_3_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_4_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_1_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_2_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_3_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LED_NUM :
		{
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data));
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_4_LED_NUM;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_1_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_2_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_3_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_4_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_1_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_2_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_3_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LED_SET_V_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_4_V_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_1_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_2_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_3_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_4_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_1_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_2_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_3_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LED_SET_V_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_4_V_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_1_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_2_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_3_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_4_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_1_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_2_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_3_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LED_SET_I_MAX :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_4_I_MAX;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH1_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_1_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH2_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_2_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH3_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_3_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH4_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			pe->start_addr = CP8113_CHANNEL_4_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH5_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_1_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH6_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_2_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH7_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_3_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_CH8_LED_SET_I_MIN :
		{			
			uint16_t reg_value = float2int(str2u32((const char *)pProtMsg->data)/1000.0);
			
			ModbusEvt * const pe = Q_NEW(ModbusEvt, WRITE_SIGNAL_HOLD_REG_SIG);
			pe->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			pe->start_addr = CP8113_CHANNEL_4_I_MIN;
			pe->reg_value = reg_value;
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
			
			break;
		}
		case CMDID_LED_READ_V :
		{						
//			ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
//			
//			pe->start_addr = CP8113_CHANNEL_1_V_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel has 3 input registers 
//	
//			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
//			pe->reg_count = 1;
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			pe->socket_n = socket_id;
//			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
					
			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
			
			if(pProtMsg->data[0] < 5)
			{
				pe2->start_addr = CP8113_CHANNEL_1_V_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			}
			else
			{
				pe2->start_addr = CP8113_CHANNEL_1_V_INPUT + ((pProtMsg->data[0]-4-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			}
			
			pe2->reg_count = 1;
			pe2->sender = sender_;
			pe2->cmd_id = cmd_id;
			pe2->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			break;
		}
		case CMDID_LED_READ_I :
		{			
//			ModbusEvt * const pe = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
//			
//			pe->start_addr = CP8113_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 

//			pe->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
//			pe->reg_count = 1;
//			pe->sender = sender_;
//			pe->cmd_id = cmd_id;
//			pe->socket_n = socket_id;
//			QACTIVE_POST(AO_RS485[RS485_IN], &pe->super, me);
					
			ModbusEvt * const pe2 = Q_NEW(ModbusEvt, READ_INPUT_REG_SIG);
			
			if(pProtMsg->data[0] < 5)
			{
				pe2->start_addr = CP8113_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = LEDLOAD_ADDR_A  | (board_id << 4);
			}
			else
			{
				pe2->start_addr = CP8113_CHANNEL_1_I_INPUT + ((pProtMsg->data[0]-4-1) * 3); // one channel have 3 input registers 
				pe2->slave_addr = LEDLOAD_ADDR_A | ((board_id ? 0 : 1) << 4);
			}
			
			pe2->reg_count = 1;
			pe2->sender = sender_;
			pe2->cmd_id = cmd_id;
			pe2->socket_n = socket_id;
			QACTIVE_POST(AO_RS485[RS485_IN], &pe2->super, me);
			
			break;
		}
		default:
		{
			printf("cmd is error, 0x%04x!!!!!!!!!!!!!!!!!!!\r\n", cmd_id);
			ret = ERR_CMDID;
			break;
		}
	}
	
	return ret;
}

/**
  * @brief  protocolMsgDispatch.
  * @param  
  * @retval 
  */
int protocolMsgUpload2(void ** sender, unsigned char boardId, unsigned short int cmdId, const char *format, ...)
{
	int ret = ERR_OK;
	unsigned char check_xor=0, i;
	QActive * sender_ = (QActive *)(*(sender));
		
	if(sender_ == NULL)
	{
		DBG_LOG(DBG_DEBUG, "sender_ is NULL! \r\n");
		return ERR_SENDER;
	}
	
	ProtocolSendEvt * pe = Q_NEW(ProtocolSendEvt, SOCK_TX_SIG);
		
	if (pe != NULL)
	{
		va_list arg;
		va_start(arg, format);
		pe->write_buff[2] = vsnprintf(&pe->write_buff[6], BUFF_LEN_MAX-6, format, arg) + 3; // cmdid cmdid boardid
		va_end(arg);
		
		pe->write_buff[0] = PROT_TX_HEAD0;
		pe->write_buff[1] = PROT_TX_HEAD1;
		pe->write_buff[3] = cmdId >> 8;
		pe->write_buff[4] = cmdId & 0xff;
		pe->write_buff[5] = boardId + 1;
		
		DBG_LOG(DBG_INFO, "ProtocolTx: cmd: %x, board: %x, dat=%s", cmdId, pe->write_buff[5], &pe->write_buff[6]);
		DBG_ARRAY(DBG_DEBUG, "->(", "\b)", (const char *)&pe->write_buff[6], pe->write_buff[2]-3);
		DBG_INFO(DBG_INFO, "\r\n");
					
		DBG_INFO(DBG_DEBUG, "protocol tx: ");
		
		for(i=0; i<pe->write_buff[2] + 3; i++)  //head0, head1, len
		{
			check_xor ^= pe->write_buff[i];
			
			DBG_INFO(DBG_DEBUG, "%02x ", pe->write_buff[i]);
		}
		
		pe->write_buff[i++] = check_xor;  
		pe->write_buff[i++] = PROT_TAIL;  
		pe->write_buff[i] = 0;  
		
		DBG_INFO(DBG_DEBUG, "%02x %02x \r\n", check_xor, PROT_TAIL);
		
		pe->len = i;
		
		pe->socket_n = boardId;
		
		QACTIVE_POST(sender_, &pe->super, me);
	}
	else
	{
		DBG_LOG(DBG_WARNING, "pe == NULL\r\n");
	}
	
	*sender = (QActive *)NULL;
	
	return ret;
}

#ifdef configUSING_CLI
/**
 *   * @brief  CLICmd_Print
 *     * @param  
 *       * @retval 
 *         */
void CLICmd_AtsxCtrl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    ProtMsg_TypeDef ProtMsg = {0};

    if(argc >= 3) {
        ProtMsg.boardId = argv[1][0] == 'B' ? 2 : 1;

        ProtMsg.cmdId = str2u32(argv[2]);

        for(uint8_t i=3; i<argc; i++) {
            strcat((char *)ProtMsg.data, argv[i]);
            strcat((char *)ProtMsg.data, " ");
        }

        protocolMsgDispatch(NULL, &ProtMsg);
    } else {
        pCli->Init.Write("\033[31m Command not find! argv[0]=%s \033[0m \r\n", argv[0]);
    }
}

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
  {" "			, "gpio out       : 0x240c NULL"		, 0, 0},
  {" "			, "gpio in        : 0x240d NULL"		, 0, 0},
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
  {"s", "s (A/B) cmdid data", CLICmd_AtsxCtrl, 0},
  // last command must be all 0s.
  {0, 0, 0, 0}
};

CLI_CMD_EXPORT(atsx, "atsx dir", 0, CLICmd_ProtocolCtrl);

#endif
/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



