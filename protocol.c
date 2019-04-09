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
#include <string.h>
#include <stdarg.h>

#include "CLI.h"
#include "qpc_common.h"
#include "czp_debug.h"
#include "protocol.h"

/***********************************************
                    define
***********************************************/

#define		PROT_RX_HEAD0			0xFF
#define		PROT_RX_HEAD1			0x22

#define		PROT_TX_HEAD0			0xFF
#define		PROT_TX_HEAD1			0x33

#define		PROT_LEN_MIN			0x03
#define		PROT_TAIL_1				0x0D
#define		PROT_TAIL_2				0x0A

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
  ERR_NOT_ALL		= -11,
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
	
	if((pSrcBuf[len+4] != PROT_TAIL_1) | (pSrcBuf[len+5] != PROT_TAIL_2)) //add head0, head1, len, check_xor
	{
		PROT_DEBUG("protocol tail is error: 0x%02x 0x%02x\r\n", pSrcBuf[len+4], pSrcBuf[len+5]);
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
	
	return ret;
}

/**
  * @brief  protoclParse.
  * @param  
  * @retval 
  */
int protocolParse(RingQueueTypedef *rq, ProtMsg_TypeDef *pProtMsg)
{
	int ret = ERR_OK, size_cnt;
	uint8_t pSrcBuf[128];
	
	ring_queue_pop(rq, (char *)&pSrcBuf[0], 1);
	
	if (ERR_OK != (ret = checkHead0(pSrcBuf)))
	{
		return ret;
	}
	
	size_cnt = ring_queue_find_block(rq, (char *)&pSrcBuf[1], rq->head, (rq->head+2)%rq->length);
	
	if (ERR_OK != (ret = checkHead1(pSrcBuf)))
	{
		return ret;
	}
			
	if (ERR_OK != (ret = getLength(pSrcBuf, &pProtMsg->length)))
	{
		return ret;
	}	
				
	rq->head = (rq->head+size_cnt)%rq->length;
	
	size_cnt = ring_queue_find_block(rq, (char *)&pSrcBuf[3], rq->head, (rq->head+pProtMsg->length+3)%rq->length);
			
	if (ERR_OK != (ret = checkTail(pSrcBuf, pProtMsg->length)))
	{
		return ret;
	}	
	
	if (ERR_OK != (ret = getCmdId(pSrcBuf, &pProtMsg->cmdId)))
	{
		return ret;
	}	
	
	if (ERR_OK != (ret = getBoardId(pSrcBuf, &pProtMsg->boardId)))
	{
		return ret;
	}	
	
	if (ERR_OK != (ret = checkXor(pSrcBuf)))
	{
		return ret;
	}
	
	if ( (pProtMsg->length > PROT_LEN_MIN) & (ERR_OK != (ret = getData(pSrcBuf, pProtMsg->data))))
	{
		return ret;
	}
	
	rq->head = (rq->head+size_cnt)%rq->length; 
	
	return ret;
}

/**
  * @brief  protocolMsgDispatch.
  * @param  
  * @retval 
  */
int protocolMsgDispatch(void * sender, ProtMsg_TypeDef *pProtMsg)
{
	int 			ret = ERR_OK;
	uint8_t 	board_id = pProtMsg->boardId;
	uint16_t  cmd_id = pProtMsg->cmdId;
	QActive 	*sender_ = (QActive *)sender;
	uint8_t 	boardx = 0;
	//QActive 	*sender_ = (QActive *)AO_Ethernet;
	
	ASSERT(board_id < (1<<LED_BOARD_MAX), 
					DBG_LOG(E_DBG_ERROR, "board_id=%d, out the range of (1<<LED_BOARD_MAX)\r\n", board_id); 
					return 0
				);
	
	switch(cmd_id)
	{	
		case CMDID_HEART_BEAT :
		{
			ProtocolEvt * const pe = Q_NEW(ProtocolEvt, HEART_BEAT_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
			QACTIVE_POST(AO_Protocol, &pe->super, me);	
			break;
		}
		case CMDID_LED_V12 :
		{
			ProtocolEvt * pe = Q_NEW(ProtocolEvt, LED_12VOFF_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
			
			if(strcmp((const char *)pProtMsg->data, "on") ? 0 : 1)
			{	
				pe->super.sig = LED_12VON_SIG;
			}
			
			while(board_id != 0)
			{
				if((board_id & 0x01) == 0x01)
				{
					QACTIVE_POST(AO_LedColor[boardx], &pe->super, me);						
				}
				boardx++;
				board_id = board_id >> 0x01;
			}
			
			break;
		}
		case CMDID_LED_SET_COLOR :
		{
			ProtocolEvt * pe = Q_NEW(ProtocolEvt, BLACK_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
			
			if(strcmp((const char *)pProtMsg->data, "red") ? 0 : 1)
			{	
				pe->super.sig = RED_SIG;
			}
			else if(strcmp((const char *)pProtMsg->data, "green") ? 0 : 1)
			{	
				pe->super.sig = GREEN_SIG;
			}
			else if(strcmp((const char *)pProtMsg->data, "blue") ? 0 : 1)
			{	
				pe->super.sig = BLUE_SIG;
			}
			else if(strcmp((const char *)pProtMsg->data, "white") ? 0 : 1)
			{	
				pe->super.sig = ALLON_SIG;
			}
			
			while(board_id != 0)
			{
				if((board_id & 0x01) == 0x01)
				{
					QACTIVE_POST(AO_LedColor[boardx], &pe->super, me);						
				}
				boardx++;
				board_id = board_id >> 0x01;
			}
			
			break;
		}
		case CMDID_KEY_STATE :
		{
			ProtocolEvt * pe = Q_NEW(ProtocolEvt, LED_KEY_DET_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
						
			while(board_id != 0)
			{
				if((board_id & 0x01) == 0x01)
				{
					QACTIVE_POST(AO_LedColor[boardx], &pe->super, me);						
				}
				boardx++;
				board_id = board_id >> 0x01;
			}
			break;
		}
		case CMDID_LED_SIGNAL :
		{
			LedSignalEvt * pe = Q_NEW(LedSignalEvt, LED_SIGNAL_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
			
			pe->pinState = strcmp((const char *)pProtMsg->data, "on") ? 0 : 1;
			
			while(board_id != 0)
			{
				if((board_id & 0x01) == 0x01)
				{
					QACTIVE_POST(AO_LedColor[boardx], &pe->super, me);						
				}
				boardx++;
				board_id = board_id >> 0x01;
			}
			break;
		}
		case CMDID_READ_SENSOR_DATA :
		{
			ProtocolEvt * pe = Q_NEW(ProtocolEvt, SENSOR_DATA_SIG);
			pe->sender = sender_;
			pe->cmd_id = cmd_id;
			pe->board_id = board_id;
						
			while(board_id != 0)
			{
				if((board_id & 0x01) == 0x01)
				{
					QACTIVE_POST(AO_LedColor[boardx], &pe->super, me);						
				}
				boardx++;
				board_id = board_id >> 0x01;
			}
			
			break;
		}
		default:
		{
			postToDebugLog("cmd is error, 0x%04x!!!!!!!!!!!!!!!!!!!\r\n", cmd_id);
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
int protocolMsgUpload(void ** sender, uint8_t boardBit, uint8_t boardId, uint16_t cmdId, const char *format, ...)
{
	int ret = ERR_OK;
	unsigned char check_xor=0, i;
	QActive * sender_ = (QActive *)(*(sender));
	static uint8_t board_bit = 0, send_len = 0, send_data[32] = {0};
		
	if(sender_ == NULL)
	{
		DBG_LOG(E_DBG_DEBUG, "sender_ is NULL! \r\n");
		return ERR_SENDER;
	}
	
	board_bit = board_bit | boardBit;
	
	va_list arg;
	va_start(arg, format);
	send_len += vsnprintf((char *)&send_data[send_len], BUFF_LEN_MAX, format, arg);
	va_end(arg);
	
	if(board_bit != boardId)
	{
		DBG_LOG(E_DBG_DEBUG, "ERR_NOT_ALL! boardBit=%d, boardId=%d\r\n", boardBit, boardId);
		return ERR_NOT_ALL;
	}
	
	board_bit = 0;
	
	ProtocolRebackEvt * pe = Q_NEW(ProtocolRebackEvt, PROTOCOL_TX_SIG);
		
//	va_list arg;
//	va_start(arg, format);
	pe->buff[2] = send_len + 3; // cmdidH cmdidL boardid
	strcpy(&pe->buff[6], (char *)send_data);
//	va_end(arg);

	pe->buff[0] = PROT_TX_HEAD0;
	pe->buff[1] = PROT_TX_HEAD1;
	pe->buff[3] = cmdId >> 8;
	pe->buff[4] = cmdId & 0xff;
	pe->buff[5] = boardId;
	
	DBG_INFO(E_DBG_INFO, "ProlTx: cmd: %x, board: %x, dat=%s\r\n", cmdId, pe->buff[5], &pe->buff[6]);
//	DBG_ARRAY(E_DBG_DEBUG, "->(", "\b)", (const char *)&pe->buff[6], pe->buff[2]-3);
//	DBG_INFO(E_DBG_INFO, "\r\n");
				
	DBG_INFO(E_DBG_DEBUG, "protocol tx: ");
	
	for(i=0; i<pe->buff[2] + 3; i++)  //head0, head1, len
	{
		check_xor ^= pe->buff[i];
		
		DBG_INFO(E_DBG_DEBUG, "%02x ", pe->buff[i]);
	}
	
	pe->buff[i++] = check_xor;  
	pe->buff[i++] = PROT_TAIL_1;  
	pe->buff[i++] = PROT_TAIL_2;  
	pe->buff[i] = 0;  
	
	DBG_INFO(E_DBG_DEBUG, "%02x %02x %02x\r\n", check_xor, PROT_TAIL_1, PROT_TAIL_2);
	
	pe->len = i;
	
	pe->board_id = boardId;
	
	QACTIVE_POST(sender_, &pe->super, me);
	
	*sender = (QActive *)NULL;
	send_len = 0;
	
	return ret;
}
/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



