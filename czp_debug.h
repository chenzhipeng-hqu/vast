
/////////////////////////////////////////////////////////////
// Copyright Statement: CVTE
// Copyright © 2018 Guangzhou Shiyuan Electronics Co.,Ltd.
// All rights reserved.
//      ____________        _______________  ___________
//     / / ________ \      / / _____   ____|| |  _______|
//    / / /      \ \ \    / / /   | | |     | | |
//   | | |        \ \ \  / / /    | | |     | | |_______
//   | | |         \ \ \/ / /     | | |     | |  _______|
//   | | |          \ \ \/ /      | | |     | | |
//    \ \ \______    \ \  /       | | |     | | |_______
//     \_\_______|    \_\/        |_|_|     |_|_________|
//
/////////////////////////////////////////////////////////////

/**
 ******************************************************************************
 * @file    debug.h
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
#ifndef __CZP_DEBUG_H__
#define __CZP_DEBUG_H__

/***********************************************
                    include
***********************************************/


/***********************************************
               function prototypes
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
																											postToDebugLog(FONT_RED "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_WARNING: {\
																											postToDebugLog(FONT_YELLOW "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_INFO: {\
																											postToDebugLog(FONT_GREEN "[%s]: " fmt RESET_COLOR, #level, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_DEBUG: {\
																											postToDebugLog(FONT_CYAN "[%s]: " fmt RESET_COLOR, #level, ##__VA_ARGS__);\
																											break;\
																									}\
																									case E_DBG_QPSIG: {\
																											if((e->sig == Q_EMPTY_SIG)\
																												| (e->sig == TIME_TICK_SIG)\
																												| (e->sig == TEST_TIMEOUT_SIG) ){\
																											}else {\
																												postToDebugLog(FONT_BLUE "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																											}\
																											break;\
																									}\
																									default: {\
																											postToDebugLog("[%s][%s]: " fmt, #level, __FUNCTION__, ##__VA_ARGS__);\
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
																								postToDebugLog(FONT_RED "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_WARNING: {\
																								postToDebugLog(FONT_YELLOW "[%s][%s]: " fmt RESET_COLOR, #level, __FUNCTION__, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_INFO: {\
																								postToDebugLog(FONT_GREEN "[%s]: " fmt RESET_COLOR, #level, ##__VA_ARGS__);\
																								break;\
																						}\
																						case E_DBG_DEBUG: {\
																								postToDebugLog(FONT_CYAN "[%s]: " fmt RESET_COLOR, #level, ##__VA_ARGS__);\
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

/************************************************
                    typedef
************************************************/
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
                   variable
***********************************************/
extern int dbgLevel;




/**
  * @}
  */

#endif

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



