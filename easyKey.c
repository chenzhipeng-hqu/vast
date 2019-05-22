/**
 ******************************************************************************
 * @file    xxx.c
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-sep-2018
 * @brief   
 ******************************************************************************
 * @attention
 *   --------------------------------------------------------------------
		|  short click                                                       |
		|  ______                   ________                                 |
		|     |  \                 /  |                                      |
		|     |   \_______________/   |                                      |
		|     |     |           |     |                                      |
		|     |shake|  < 500ms  |shake|                                      |
		|                                                                    |
		 -------------------------------------------------------------------
		|  double click                                                      |
		|  ______                   _____________                   ____     |
		|     |  \                 /  |       |  \                 /  |      |
		|     |   \_______________/   |       |   \_______________/   |      |
		|     |     |           |     | < max |     |           |     |      |
		|     |shake|  < 500ms  |shake|<220ms |shake|  any time |shake|      |
		|                                                                    |
		 --------------------------------------------------------------------
		|  long click                                                        |
		|  ______                                           ________         |
		|     |  \                                         /  |              |
		|     |   \_______________________________________/   |              |
		|     |     |                                   |     |              |
		|     |shake|   500ms  < long click  <1000ms    |shake|              |
		|                                                                    |
		 --------------------------------------------------------------------
		|  long tirgger                                                      |
		|  ______                                                        ____|
		|     |  \                                                      / |  |
		|     |   \____________________________________________________/  |  |
		|     |     |                         |                 |   |     |  |
		|     |shake|  1000ms  < long click   |tirgger per 200ms|   |shake|  |
		|                                                                    |
		 --------------------------------------------------------------------
 ******************************************************************************
 */
    
/** @addtogroup Project
  * @{
  */ 

/***********************************************
                    include
***********************************************/
#include <stdio.h>
#include "easyKey.h"

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

/***********************************************
                   function
***********************************************/
/**
  * @brief EasyKey_Init.
  * @param  
  * @retval 
  */
int EasyKey_Init(EasyKey_HandleTypeDef *pKey)
{
	pKey->pKeyProcess(pKey, EASYKEY_FUNC_INIT);
	pKey->Key_Sm1 = KEY_SM_RELEASE;
	pKey->Key_Sm2 = KEY_SM_WAIT_CLICK_ONCE;
	
	return 0;
}
/**
  * @brief EasyKey_Driver.
	* @param  
  * @retval 
  */
KEY_StateTypeDef EasyKey_Driver(EasyKey_HandleTypeDef *pKey)
{
	KEY_StateTypeDef key_ret = KEY_NULL;
	
	pKey->Key_Now =	(KEY_LevelTypeDef)pKey->pKeyProcess(pKey, EASYKEY_FUNC_READ);
	
	switch(pKey->Key_Sm1)
	{
		case KEY_SM_RELEASE:
		{
			if(pKey->Key_Now == KEY_DOWN)
			{
				pKey->Key_Sm1 = KEY_SM_SHAKE;
			}
			
			break;
		}
		case KEY_SM_SHAKE:
		{
			if(pKey->Key_Now == KEY_DOWN)
			{
				pKey->Key_Time = 0;
				pKey->Key_Sm1 = KEY_SM_PRESS;
			}
			else
			{
				pKey->Key_Sm1 = KEY_SM_RELEASE;
			}
			
			break;
		}
		case KEY_SM_PRESS:
		{
			if(pKey->Key_Now == KEY_UP)
			{
				key_ret = KEY_CLICK_ONCE;
				pKey->Key_Sm1 = KEY_SM_RELEASE;
			}
			else if(++pKey->Key_Time >= 50)
			{
				pKey->Key_Sm1 = KEY_SM_WAIT_RESLASE;
				
				if(pKey->Key_ContinousTirg == false)
				{
					key_ret = KEY_PRESS_LONG;
					pKey->pKeyProcess(pKey, EASYKEY_FUNC_PRESS_LONG);
				}
			}
			
			break;
		}
		case KEY_SM_WAIT_RESLASE:
		{
			if(pKey->Key_Now == KEY_UP)
			{
				pKey->Key_Sm1 = KEY_SM_RELEASE;
				
				if((pKey->Key_Time < 100) & (pKey->Key_ContinousTirg == true))
				{
					key_ret = KEY_PRESS_LONG;
					pKey->pKeyProcess(pKey, EASYKEY_FUNC_PRESS_LONG);
				}
			}
			else if((pKey->Key_ContinousTirg == true) & (++pKey->Key_Time >= 100))
			{
				uint8_t fac = 50;
				
				if(pKey->Key_Time > 500)
				{
					fac = 1;
				}
				else if(pKey->Key_Time > 400)
				{
					fac = 5;
				}
				else if(pKey->Key_Time > 300)
				{
					fac = 20;
				}
				
				if(pKey->Key_Time%fac == 0)
				{
					key_ret = KEY_CONTINOUS;
					pKey->pKeyProcess(pKey, EASYKEY_FUNC_CONTINOUS);
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return key_ret;
}

/**
  * @brief EasyKey_Scan.
	* @param  
  * @retval 
  */
KEY_StateTypeDef EasyKey_Scan(EasyKey_HandleTypeDef *pKey)
{
	KEY_StateTypeDef key_sta, key_ret = KEY_NULL;
	
	key_sta = EasyKey_Driver(pKey);
	
	switch(pKey->Key_Sm2)
	{
		case KEY_SM_WAIT_CLICK_ONCE:
		{
			if(key_sta == KEY_CLICK_ONCE)
			{
				pKey->Key_Sm2 = KEY_SM_CHECK_DOUBLE;
			}
			else
			{
				key_ret = key_sta;
			}
			break;
		}
		case KEY_SM_CHECK_DOUBLE:
		{
			if(key_sta == KEY_CLICK_ONCE)
			{
				pKey->Key_Sm2 = KEY_SM_WAIT_CLICK_ONCE;
				key_ret = KEY_CLICK_DOUB;				
				pKey->pKeyProcess(pKey, EASYKEY_FUNC_CLICK_DOUB);
			}
			else if(++pKey->Key_Time >= 23)
			{
				pKey->Key_Sm2 = KEY_SM_WAIT_CLICK_ONCE;
				key_ret = KEY_CLICK_ONCE;
				pKey->pKeyProcess(pKey, EASYKEY_FUNC_CLICK_ONCE);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	
	return key_ret;
}

#if 0  // exp:
/***********************************************
                   variable
***********************************************/
EasyKey_HandleTypeDef	hkey_start;

/***********************************************
                   function
***********************************************/
/**
  * @brief  KeyProcess_Start.
  * @param  
  * @retval 
  */
uint8_t KeyProcess_Start (EasyKey_HandleTypeDef *pKey, EASYKEY_SEL_FUNCx selFunc)
{
	uint8_t ret = 0;
	
	switch(selFunc)
	{
		case EASYKEY_FUNC_INIT :
		{		
			GPIO_InitTypeDef  GPIO_InitStruct;
			
			GPIO_InitStruct.Pin       = GPIO_PIN_15;
			GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull      = GPIO_PULLUP;
			GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
			HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
			
			break;
		}
		case EASYKEY_FUNC_READ :
		{		
			ret = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15);
			break;
		}
		case EASYKEY_FUNC_CLICK_ONCE :
		{			
			UsartPrintf("%s click once, t:%d, %d\r\n", __FUNCTION__, osKernelSysTick(), pKey->Key_ClickInterval);
			break;
		}
		case EASYKEY_FUNC_CLICK_DOUB :
		{			
			UsartPrintf("%s click double, t:%d, %d, %d, %d, %d, %d, %d, %d\r\n"				, 
																					__FUNCTION__			, 
																					osKernelSysTick()	, 
																					pKey->Key_Time		, 
																					sizeof(KEY_StateTypeDef), 
																					sizeof(KEY_SMachineTypeDef), 
																					sizeof(uint8_t), 
																					sizeof(uint32_t), 
																					sizeof(EasyKey_HandleTypeDef)
																				);
			break;
		}
		case EASYKEY_FUNC_PRESS_LONG :
		{			
			UsartPrintf("%s press long, t:%d, %d\r\n", __FUNCTION__, osKernelSysTick(), pKey->Key_Time);
			break;
		}
		case EASYKEY_FUNC_CONTINOUS :
		{			
			UsartPrintf("%s continous, t:%d, %d\r\n", __FUNCTION__, osKernelSysTick(), pKey->Key_Time);
			break;
		}
		default:
		{
			UsartPrintf("%s %d, err key func!\r\n", __FUNCTION__, __LINE__);
			break;
		}
	}
	
	return ret;
}

/**
  * @brief main.
	* @param  
  * @retval 
  */
int main(int argc, char *argv[])
{
	hkey_start.pKeyProcess = KeyProcess_Start;
	hkey_start.Key_ContinousTirg = true;
	EasyKey_Init(&hkey_start);
	
	while(1)
	{
		static uint32_t old_tick = 0;
		
		if(HAL_GetTick() - old_tick > 10)
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

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/



