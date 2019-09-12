/**
 ******************************************************************************
 * @file    xxx.c
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
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
#include <stdarg.h>
#include "vast_log.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
Log_HandleTypeDef 	hlog;

const char buildDate[] = __DATE__;
const char buildTime[] = __TIME__;

/***********************************************
                   function
***********************************************/
/**
  * @brief  vast_log_getLevel.
  * @param
  * @retval
  */

uint8_t vast_log_getLevel(void)
{
	return hlog.dbgLevel;
}

/**
  * @brief  vast_log_setLevel.
  * @param
  * @retval
  */

int16_t vast_log_setLevel(uint8_t dbg_level)
{
	hlog.dbgLevel = dbg_level;
	return 0;
}

/**
  * @brief  vast_log_setDate.
  * @param
  * @retval
  */

int16_t vast_log_setDate(Log_HandleTypeDef *pLog, uint16_t year, uint8_t month, uint8_t day,
													uint8_t hour, uint8_t min, uint8_t sec)
{
	pLog->Init.year 	= year 	== 0 ? pLog->Init.year 	: year;
	pLog->Init.month 	= month == 0 ? pLog->Init.month : month;
	pLog->Init.day 		= day 	== 0 ? pLog->Init.day 	: day;
	pLog->Init.hour 	= hour;
	pLog->Init.min 		= min;
	pLog->Init.sec 		= sec;
	pLog->WriteLog("set: %d-%02d-%02d %02d:%02d:%02d\r\n", pLog->Init.year, pLog->Init.month, pLog->Init.day, pLog->Init.hour, pLog->Init.min, pLog->Init.sec);
	return 0;
}

/**
  * @brief  vast_log_setDate.
  * @param
  * @retval
  */
/*
year : month : day  : hour : minute : second
6bit : 4bit  : 5bit : 5bit : 6bit   : 6bit
year from2000-2064
*/
int16_t vast_log_printDate(void)
{
	hlog.WriteLog("now: %d-%02d-%02d %02d:%02d:%02d\r\n", hlog.Init.year, hlog.Init.month, hlog.Init.day, hlog.Init.hour, hlog.Init.min, hlog.Init.sec);
	return 0;
}

static char write_buff[128] = {0};
/**
  * @brief  vast_log_array.
  * @param
  * @retval
  */
int16_t vast_log_array(const char *head_buff, const char *tail_buff, const char *pArray, uint16_t size)
{
	uint16_t i = 0, len_tmp = 0, len = 0;

	len = snprintf(&write_buff[0], BUFF_LEN_MAX, "%s", head_buff);

	for(i=0; i< size; i++)
	{
		len_tmp = len;
		len += snprintf(&write_buff[len], BUFF_LEN_MAX-len_tmp, "%02x ", pArray[i]);

		if(len >= BUFF_LEN_MAX-1)
		{
			hlog.WriteLog("%s", write_buff);
			len = len_tmp = 0;
			//break;
		}
	}

	len += snprintf(&write_buff[len], BUFF_LEN_MAX, "%s", tail_buff);

	hlog.WriteLog("%s", write_buff);

	return len;
}

/**
  * @brief  vast_log_array.
  * @param
  * @retval
  */
size_t vast_log_info(int level, const char *fmt, ...)
{
	size_t len = 0;

	return len;
}

/**
  * @brief  vast_log_initialize.
  * @param
  * @retval
  */
int16_t vast_log_initialize(WriteLogFunc writeLogFunc, LogTickFunc tickFunc)
{
	if(writeLogFunc != NULL)
	{
		hlog.WriteLog = writeLogFunc;
		hlog.Tick = tickFunc;

		if(hlog.Init.year == 0)
		{
			char month[][3] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",

							   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
			uint16_t u16Year = 0;
			uint8_t u8Month = 0, u8Day = 0, u8Hour = 0, u8Min = 0, u8Sec = 0;

			u8Day = ((buildDate[4]-'0')*10) + buildDate[5] - '0';
			u16Year = ((buildDate[7]-'0')*1000) + ((buildDate[8]-'0')*100) + ((buildDate[9]-'0')*10) + buildDate[10] - '0';

			for(u8Month=0; u8Month<12; u8Month++)
			{
				if(!strncmp(buildDate, month[u8Month], 3))
				{
					u8Month ++;
					break;
				}
			}

			u8Hour 	= ((buildTime[0]-'0')*10 + (buildTime[1]-'0'));
			u8Min 	= ((buildTime[3]-'0')*10 + (buildTime[4]-'0'));
			u8Sec 	= ((buildTime[6]-'0')*10 + (buildTime[7]-'0'));

			vast_log_setDate(&hlog, u16Year, u8Month, u8Day, u8Hour, u8Min, u8Sec);
		}
		return 0;
	}
	else
	{
		return 1;
	}
}

#if 0  // exp:
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

int main(int argc, char *argv[])
{
	vast_log_initialize(postToLog, HAL_GetTick);
	vast_log_setLevel(DBG_DEBUG);

	vast_log_printDate();
	uint8_t test_array[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
	DBG_ARRAY(DBG_DEBUG, "array test: ", "\r\n", (char *)test_array, sizeof(test_array));

	DBG_LOG(DBG_INFO, "this is debug info test.\r\n");
	DBG_INFO(DBG_INFO, "this is debug info test.\r\n");

	ASSERT(1 < 0,
			DBG_LOG(DBG_ERROR, "1 < 0 is error!\r\n");
		);

	while(1)
	{
		static uint32_t old_tick = 0;

		if(HAL_GetTick() - old_tick > 10) // must be 10ms per circle
		{
			old_tick = HAL_GetTick();
			EasyKey_Scan(&hkey_start);
		}
	}
	return 0;
}
#endif
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



