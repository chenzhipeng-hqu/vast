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
#include "smartframe.h"

#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "frame"
    #include "elog.h"
#else
#endif


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
			log_e("%02x ", pSrcBuf[i]);
		}
		
		log_e("\r\n");
		
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


#if (defined VAST_USING_QPC)
#include "qpc_common.h"
/**
  * @brief  protocolMsgDispatch.
  * @param  
  * @retval 
  */
int protocolMsgUpload2(void ** sender, unsigned char boardId, unsigned short int cmdId, const char *format, ...)
{
	int ret = ERR_OK;
	QActive * sender_ = (QActive *)(*(sender));
		
	if(sender_ == NULL)
	{
		log_d("sender_ is NULL!");
		return ERR_SENDER;
	}
	
	ProtocolSendEvt * pe = Q_NEW(ProtocolSendEvt, RESULT_SIG);
		
	if (pe != NULL)
	{
		va_list arg;
		va_start(arg, format);
		pe->len = vsnprintf(&pe->write_buff[0], BUFF_LEN_MAX, format, arg);
		va_end(arg);
		
		pe->socket_n = boardId;
		
		QACTIVE_POST(sender_, &pe->super, me);
	}
	else
	{
		log_w("pe == NULL");
	}
	
	*sender = (QActive *)NULL;
	
	return ret;
}
#endif
#if 1
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
/**
  * @brief  .
  * @param  
  * @retval 
  */
uint16_t check_crc16(uint8_t *buffer, uint16_t buffer_length)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }
    return (crc_hi << 8 | crc_lo);
}

#elif 0
unsigned short crc_table[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};
uint16_t check_crc16(uint8_t *buffer, uint16_t buffer_length)
{
    unsigned short crc = 0;
    int i = 0;

    for (i = 0; i < buffer_length; i++)
        crc = crc_table[((crc >> 8) ^ buffer[i]) & 0xFF] ^ (crc << 8);

    return crc;
}
#elif 0
unsigned short CRC_TABLE[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
};
uint16_t check_crc16(uint8_t *pBuffer, uint16_t ucLength)
{
    uint16_t uwCRC, uwCRCH, uwCRCL, ucTemp;

    uwCRC = 0;

    for (uint16_t c=0; c<ucLength; c++) {
        ucTemp = uwCRC >> 0x0C;
        uwCRC = (uwCRC << 4) & 0xFFFF;
        uwCRC = (uwCRC ^ (CRC_TABLE[(ucTemp ^ (pBuffer[c] >> 4))])) & 0xffff;
        ucTemp = (uwCRC >> 0x0C) & 0xFFFF;
        uwCRC = (uwCRC << 4) & 0xFFFF;
        uwCRC =( uwCRC ^ (CRC_TABLE[(ucTemp ^ (pBuffer[c] & 0x0F))])) & 0xffff;
        uwCRCH = (uwCRC >> 8) & 0xFF;
        uwCRCL = (uwCRC) & 0xFF;
    }

    return uwCRC;
}
#else
uint16_t check_crc16(uint8_t *datas, uint16_t len)
{
    unsigned short crc;
    unsigned char da;
    unsigned char *ptr = datas;

    unsigned short crc_table[16] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    };

    LOG_HEX("fsm.crc", 10, datas, len);

    crc = 0;
    while( len-- != 0 ) {
        da = ((unsigned char)(crc / 256)) / 16; /* temp crc high 4bits */
        crc <<= 4;
        crc ^= crc_table[da ^ ( *ptr / 16 )];

        da = ((unsigned char)(crc / 256)) / 16;
        crc <<= 4;
        crc ^= crc_table[da ^ (*ptr & 0x0F)];
        ptr++;
    }

    return (crc);
}
#endif

/**
  * @brief  .
  * @param  
  * @retval 
  */
//uint8_t checksum(const void *data, int len)
//{
    //uint8_t checksum = 0;

    //while (len-- > 0) {
        //checksum += *((uint8_t *)data + len);
    //}

    //return checksum;
//}

/**
  * @brief  .
  * @param  
  * @retval 
  */
remote_frame_t *get_remote_frame(const uint8_t *data, uint32_t len)
{
    int i = 0;

start_lbl:
    while (i < len) {
        if (REMOTE_STC == data[i]) {
            break;
        }
        i++;
    }

    if (len - i < REMOTE_FRAME_HEAD) {
        return NULL;
    }

    remote_frame_t *pframe = (remote_frame_t *)&data[i];

    int dlen = pframe->len - REMOTE_FRAME_HEAD - 2;

    //rt_kprintf("len:%#x, dlen:%#x\r\n", pframe->len, dlen);
    if (i + pframe->len > len) {
        i++;
        goto start_lbl;
    }

    uint16_t crc16 = check_crc16((uint8_t *)pframe, pframe->len - 2);

    if (((pframe->data[dlen+1] << 8) | pframe->data[dlen]) != crc16) {
        log_e("crc16:cal:%#x, get:%#x\r\n", crc16, (pframe->data[dlen+1] << 8) | pframe->data[dlen]);
        i++;
        goto start_lbl;
    }

    pframe = (remote_frame_t *)&data[i];

    return pframe;
}

/**
  * @brief  .
  * @param  
  * @retval 
  */
int code_remote_frame(const uint8_t from, const uint8_t to, int seq, uint8_t cmd, 
                                uint8_t *data, int len, void *out, int maxlen)
{
    //rt_enter_critical();

	remote_frame_t * pframe = (remote_frame_t *) out;
	pframe->stc = REMOTE_STC;
    pframe->from = from;
    pframe->to = to;
	pframe->seq = seq;
	pframe->cmd = cmd;

	pframe->len = REMOTE_FRAME_HEAD + len + 2;  //add crc16 len
    memmove(&pframe->data[0], data, len);

    uint16_t crc16 = check_crc16((uint8_t *)pframe, REMOTE_FRAME_HEAD + len);
    pframe->data[len] = (crc16) & 0xff;
    pframe->data[len+1] = (crc16>>8) & 0xff;

    //rt_exit_critical();

	return pframe->len;
}

/**
  * @brief  .
  * @param  
  * @retval 
  */
int code_local_frame(const uint32_t from, const uint32_t to, int seq, uint8_t cmd,
                                uint8_t *data, int len, void *out, int maxlen)
{
    //rt_enter_critical();

	local_frame_t * pframe = (local_frame_t *) out;
	//pframe->stc = LOCAL_STC;
    pframe->from = from;
    pframe->to = to;
	//pframe->seq = seq;
	pframe->cmd = cmd;

	pframe->len = LOCAL_FRAME_HEAD + len + 2;  //add crc16 len
    memmove(&pframe->data[0], data, len);

    uint16_t crc16 = check_crc16((uint8_t *)pframe, LOCAL_FRAME_HEAD + len);
    pframe->data[len] = (crc16) & 0xff;
    pframe->data[len+1] = (crc16>>8) & 0xff;

    //rt_exit_critical();

	return pframe->len;
}

/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



