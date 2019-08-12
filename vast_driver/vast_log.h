/**
 ******************************************************************************
 * @file    xxx.h
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

/* Define to prevent recursive inclusion */
#ifndef __VAST_LOG_H__
#define __VAST_LOG_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "vast_common.h"

/***********************************************
                    define
***********************************************/
//Black(30m) , Red(31m) , GREEN(32m) , YELLOW(33m) , BLUE(34m) , MAGENTA(35m) , CYAN(36m) , WHITE(37m)

#define ENABLE_DEBUG_COLOR  1
#if( ENABLE_DEBUG_COLOR )
    #define FONT_BLACK    "\x1b[30m"
    #define FONT_RED      "\x1b[31m"
    #define FONT_GREEN    "\x1b[32m"
    #define FONT_YELLOW   "\x1b[33m"
    #define FONT_BLUE     "\x1b[34m"
    #define FONT_MAGENTA  "\x1b[35m"
    #define FONT_CYAN     "\x1b[36m"
    #define FONT_WHITE    "\x1b[37m"

    #define BACKGROUND_BLACK    "\033[40m"
    #define BACKGROUND_RED      "\033[41m"
    #define BACKGROUND_GREEN    "\033[42m"
    #define BACKGROUND_YELLOW   "\033[43m"
    #define BACKGROUND_BLUE     "\033[44m"
    #define BACKGROUND_MAGENTA  "\033[45m"
    #define BACKGROUND_CYAN     "\033[46m"
    #define BACKGROUND_WHITE    "\033[46m"

    #define RESET_COLOR   "\033[0m"
#else

    #define FONT_BLACK    ""
    #define FONT_RED      ""
    #define FONT_GREEN    ""
    #define FONT_YELLOW   ""
    #define FONT_BLUE     ""
    #define FONT_MAGENTA  ""
    #define FONT_CYAN     ""
    #define FONT_WHITE    ""

    #define BACKGROUND_BLACK    ""
    #define BACKGROUND_RED      ""
    #define BACKGROUND_GREEN    ""
    #define BACKGROUND_YELLOW   ""
    #define BACKGROUND_BLUE     ""
    #define BACKGROUND_MAGENTA  ""
    #define BACKGROUND_CYAN     ""
    #define BACKGROUND_WHITE    ""

    #define RESET_COLOR  ""
#endif


#define DBG_PRINTF_TASK(level, fmt, ...)    do{ \
												if( level <= pLog->Init.dbgLevel) {\
													switch (level) {\
														case E_DBG_CRISIS: {\
															hlog.WriteLog(FONT_MAGENTA "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																break;\
														}\
														case E_DBG_ERROR: {\
															hlog.WriteLog(FONT_RED "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																break;\
														}\
														case E_DBG_WARNING: {\
															hlog.WriteLog(FONT_YELLOW "[%08d][%s][%s]: " fmt RESET_COLOR, osKernehlog.TicklSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																break;\
														}\
														case E_DBG_INFO: {\
																hlog.WriteLog(FONT_GREEN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
																break;\
														}\
														case E_DBG_DEBUG: {\
															hlog.WriteLog(FONT_CYAN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
																break;\
														}\
														case E_DBG_QPSIG: {\
															hlog.WriteLog(FONT_BLUE "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																}\
																break;\
														}\
														default: {\
																break;    \
														}\
													}\
												}\
											}while(0)

#define DBG_LOG(level, fmt, ...)     do{ \
										if( level <= hlog.dbgLevel) {\
											switch (level) {\
												case DBG_CRISIS: {\
													hlog.WriteLog(FONT_MAGENTA "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_ERROR: {\
													hlog.WriteLog(FONT_RED "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_WARNING: {\
													hlog.WriteLog(FONT_YELLOW "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_INFO: {\
													hlog.WriteLog(FONT_GREEN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_DEBUG: {\
													hlog.WriteLog(FONT_CYAN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
														break;\
												}\
												default: {\
														break;    \
												}\
											}\
										}\
									}while(0)

#define DBG_INFO(level, fmt, ...)     do{ \
										if( level <= hlog.dbgLevel) {\
											switch (level) {\
												case DBG_CRISIS: {\
													hlog.WriteLog(FONT_MAGENTA fmt RESET_COLOR, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_ERROR: {\
													hlog.WriteLog(FONT_RED fmt RESET_COLOR, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_WARNING: {\
													hlog.WriteLog(FONT_YELLOW fmt RESET_COLOR, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_INFO: {\
													hlog.WriteLog(FONT_GREEN fmt RESET_COLOR, ##__VA_ARGS__);\
														break;\
												}\
												case DBG_DEBUG: {\
													hlog.WriteLog(FONT_CYAN fmt RESET_COLOR, ##__VA_ARGS__);\
														break;\
												}\
												default: {\
														break;    \
												}\
											}\
										}\
									}while(0)

#define DBG_PRINT(level, fmt, ...)  do{ \
										switch (level) {\
											case DBG_CRISIS: {\
												hlog.WriteLog(FONT_MAGENTA "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
													break;\
											}\
											case DBG_ERROR: {\
												hlog.WriteLog(FONT_RED "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
													break;\
											}\
											case DBG_WARNING: {\
												hlog.WriteLog(FONT_YELLOW "[%08d][%s][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, __FUNCTION__, ##__VA_ARGS__);\
													break;\
											}\
											case DBG_INFO: {\
												hlog.WriteLog(FONT_GREEN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
													break;\
											}\
											case DBG_DEBUG: {\
												hlog.WriteLog(FONT_CYAN "[%08d][%s]: " fmt RESET_COLOR, hlog.Tick(), #level, ##__VA_ARGS__);\
													break;\
											}\
											default: {\
													break;    \
											}\
										}\
									}while(0)

#define DBG_ARRAY(level, aHEAD, aTAIL, aARRAY, nLEN)		do{ \
																if(level <= hlog.dbgLevel) {\
																	switch (level) {\
																		case DBG_DEBUG: {\
																				vast_log_array(FONT_CYAN aHEAD, aTAIL RESET_COLOR, aARRAY, nLEN);\
																				break;\
																		}\
																		default: {\
																				break;    \
																		}\
																	}\
																}\
															}while(0)

#ifdef NDEBUG
	#define     ASSERT(test_, err_exe) 		((void)0)
#else
	#define     ASSERT(test_, err_exe) 		do{ \
												if((test_) == 0) {\
													printf("\033[15;31m Assertion failed in %s:%d\033[0m \r\n", __FILE__, __LINE__);\
													err_exe;\
												}\
											}while(0)
#endif

#define		BUFF_LEN_MAX 		128

/***********************************************
                    typedef
***********************************************/
enum DBG_LEVEL{
	DBG_CRISIS,
	DBG_ERROR,
	DBG_WARNING,
	DBG_INFO,
	DBG_DEBUG,
	DBG_QPSIG,
	DBG_MAX
};

typedef enum
{
	LOG_FUNC_INIT,
	LOG_FUNC_TX_POP,
	LOG_FUNC_RX_PUSH,
	LOG_FUNC_RX_POP,
}LOG_SEL_FUNCx;

typedef int16_t (*WriteLogFunc)( const char *format, ... );
typedef uint32_t (*LogTickFunc)(void);

typedef struct _Log_InitTypeDef
{
	uint16_t 	year;
	uint8_t 	month;
	uint8_t 	day;
	uint8_t 	hour;
	uint8_t 	min;
	uint8_t 	sec;
}Log_InitTypeDef;

typedef struct _Log_HandleTypeDef
{
	Log_InitTypeDef	 		Init;
	uint8_t 				dbgLevel;
	int16_t 				(*Read)		(uint8_t *pData, uint8_t len);
	WriteLogFunc 			WriteLog;
	LogTickFunc	 			Tick;
	uint8_t					(*pLogProcess)		(struct _Log_HandleTypeDef *pLog, LOG_SEL_FUNCx selFunc);

}Log_HandleTypeDef;

/***********************************************
               function prototypes
***********************************************/
int16_t vast_log_initialize(WriteLogFunc writeLogFunc, LogTickFunc tickFunc);
int16_t vast_log_printDate(void);
int16_t vast_log_setLevel(uint8_t dbg_level);
int16_t vast_log_array(const char *head_buff, const char *tail_buff, const char *pArray, uint16_t size);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/
extern const char buildDate[];
extern const char buildTime[];
extern Log_HandleTypeDef 	hlog;

#ifdef __cplusplus
}
#endif

#endif /* __VAST_LOG_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


