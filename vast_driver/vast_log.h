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
																							if( level <= me->dbgLevel) {\
																								switch (level) {\
																									case E_DBG_CRISIS: {\
																											postToDebugLog(FONT_MAGENTA "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_ERROR: {\
																											postToDebugLog(FONT_RED "[%08d][%s][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_WARNING: {\
																											postToDebugLog(FONT_YELLOW "[%08d][%s][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_INFO: {\
																											postToDebugLog(FONT_GREEN "[%08d][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_DEBUG: {\
																											postToDebugLog(FONT_CYAN "[%08d][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_QPSIG: {\
																											if((e->sig == Q_EMPTY_SIG)\
																												| (e->sig == TIME_TICK_SIG)\
																												| (e->sig == SOCK_ACCEPT_TIMEOUT_SIG)\
																												| (e->sig == SOCKET_DISPATCH_TIMEOUT_SIG)\
																												| (e->sig == SOCK_RX_TIMEOUT_SIG)\
																												| (e->sig == SET_LINK_TIMEOUT_SIG)\
																												| (e->sig == ATSX_POW_TIMEOUT_SIG)\
																												| (e->sig == LWIP_RX_READY_SIG)\
																												| (e->sig == PHASE_DET_SIG)\
																												| (e->sig == KEY_TIMEOUT_SIG)\
																												| (e->sig == TEST_TIMEOUT_SIG) ){\
																											}else {\
																												postToDebugLog(FONT_BLUE "[%08d][%s][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
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
																				if( level <= dbgLevel) {\
																					switch (level) {\
																						case E_DBG_CRISIS: {\
																								postToDebugLog(FONT_MAGENTA "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_ERROR: {\
																								postToDebugLog(FONT_RED "[%08d][%s][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_WARNING: {\
																								postToDebugLog(FONT_YELLOW "[%08d][%s][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, __FUNCTION__, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_INFO: {\
																								postToDebugLog(FONT_GREEN "[%08d][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_DEBUG: {\
																								postToDebugLog(FONT_CYAN "[%08d][%s]: " fmt RESET_COLOR, osKernelSysTick(), #level, ##__VA_ARGS__);\
																								break;\
																						}\
																						default: {\
																								break;    \
																						}\
																					}\
																				}\
																			}while(0)

#define DBG_INFO(level, fmt, ...)     do{ \
																				if( level <= dbgLevel) {\
																					switch (level) {\
																						case E_DBG_CRISIS: {\
																								postToDebugLog(FONT_MAGENTA fmt RESET_COLOR, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_ERROR: {\
																								postToDebugLog(FONT_RED fmt RESET_COLOR, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_WARNING: {\
																								postToDebugLog(FONT_YELLOW fmt RESET_COLOR, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_INFO: {\
																								postToDebugLog(FONT_GREEN fmt RESET_COLOR, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_DEBUG: {\
																								postToDebugLog(FONT_CYAN fmt RESET_COLOR, ##__VA_ARGS__);\
																								break;\
																						}\
																						default: {\
																								break;    \
																						}\
																					}\
																				}\
																			}while(0)

#define DBG_ARRAY(level, aHEAD, aTAIL, aARRAY, nLEN)		do{ \
																													if(level <= dbgLevel) {\
																														switch (level) {\
																															case E_DBG_DEBUG: {\
																																	arrayPostToDebugLog(FONT_CYAN aHEAD, aTAIL RESET_COLOR, aARRAY, nLEN);\
																																	break;\
																															}\
																															default: {\
																																	break;    \
																															}\
																														}\
																													}\
																												}while(0)

#define     ASSERT(test_, err_exe) 				do{ \
																						if((test_) == 0) {\
																							printf("\033[15;31m Assertion failed in %s:%d\033[0m \r\n", __FILE__, __LINE__);\
																							err_exe;\
																						}\
																					}while(0)

/***********************************************
                    typedef
***********************************************/
enum DBG_LEVEL{
	E_DBG_CRISIS,
	E_DBG_ERROR,
	E_DBG_WARNING,
	E_DBG_INFO,
	E_DBG_DEBUG,
	E_DBG_QPSIG,
	E_DBG_MAX
};

/***********************************************
               function prototypes
***********************************************/

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/
extern int dbgLevel;

#ifdef __cplusplus
}
#endif

#endif /* __VAST_LOG_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


