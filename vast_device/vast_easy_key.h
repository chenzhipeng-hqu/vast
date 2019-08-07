/**
 ******************************************************************************
 * @file    xxx.h
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-Dec-2018
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
#ifndef __VAST_EASY_KEY_H__
#define __VAST_EASY_KEY_H__

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
#ifndef false
	#define 	false		0
#endif
	
#ifndef true
	#define 	true		1
#endif
	
/***********************************************
                    typedef
***********************************************/
typedef enum{
	KEY_DOWN = 0,
	KEY_UP				,
}KEY_LevelTypeDef;

typedef enum{
	KEY_NULL = 0	,
	KEY_CLICK_ONCE,
	KEY_CLICK_DOUB,
	KEY_PRESS_LONG,
	KEY_CONTINOUS	,
}KEY_StateTypeDef;

typedef enum{
	KEY_SM_RELEASE = 0	,
	KEY_SM_SHAKE				,
	KEY_SM_PRESS				,
	KEY_SM_WAIT_RESLASE	,
}KEY_SMachineTypeDef;

typedef enum{
	KEY_SM_WAIT_CLICK_ONCE = 0,
	KEY_SM_CHECK_DOUBLE	 			,
}KEY_SMachine2TypeDef;

typedef enum
{
	EASYKEY_FUNC_INIT = 0,
	EASYKEY_FUNC_READ,
	EASYKEY_FUNC_CLICK_ONCE,
	EASYKEY_FUNC_CLICK_DOUB,
	EASYKEY_FUNC_PRESS_LONG,
	EASYKEY_FUNC_CONTINOUS,
}EASYKEY_SEL_FUNCx;

typedef struct _EasyKey_HandleTypeDef
{
	uint8_t				Key_Now:1;
	uint8_t				Key_Sm1:2;
	uint8_t				Key_Sm2:1;
	uint8_t				Key_ContinousTirg:1;
	uint32_t			Key_Time:27;
	uint8_t				(*pKeyProcess)	(struct _EasyKey_HandleTypeDef *pKey, EASYKEY_SEL_FUNCx selFunc);
}EasyKey_HandleTypeDef;

/***********************************************
                   variable
***********************************************/

/***********************************************
               function prototypes
***********************************************/
extern int16_t				EasyKey_Init	(EasyKey_HandleTypeDef *pKey);
extern KEY_StateTypeDef		EasyKey_Scan	(EasyKey_HandleTypeDef *pKey);

#ifdef __cplusplus
}
#endif
#endif  /* __VAST_EASY_KEY_H__ */

/**
  * @}
  */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/





