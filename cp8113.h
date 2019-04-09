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
#ifndef __CP8113_H__
#define __CP8113_H__

/***********************************************
                    include
***********************************************/

/***********************************************
                    define
***********************************************/
/*----------------------------------------------------------------------------*/
// Register Addresses
#define					CP8113_CHANNEL_1				(uint16_t)((x & 0x01) << 0)
#define					CP8113_CHANNEL_2				(uint16_t)((x & 0x01) << 1)
#define					CP8113_CHANNEL_3				(uint16_t)((x & 0x01) << 2)
#define					CP8113_CHANNEL_4				(uint16_t)((x & 0x01) << 3)


/***********************************************
                    typedef
***********************************************/
enum CP8113_HOLD_REG_ADDR
{
	CP8113_CHANNEL_SWITCH = 0,

	CP8113_CHANNEL_1_LED_NUM = 1,
	CP8113_CHANNEL_1_JUDGE_OUTPUT,
	CP8113_CHANNEL_1_V_MAX,
	CP8113_CHANNEL_1_V_MIN,
	CP8113_CHANNEL_1_I_MAX,
	CP8113_CHANNEL_1_I_MIN,

	CP8113_CHANNEL_2_LED_NUM = 7,
	CP8113_CHANNEL_2_JUDGE_OUTPUT,
	CP8113_CHANNEL_2_V_MAX,
	CP8113_CHANNEL_2_V_MIN,
	CP8113_CHANNEL_2_I_MAX,
	CP8113_CHANNEL_2_I_MIN,

	CP8113_CHANNEL_3_LED_NUM = 13,
	CP8113_CHANNEL_3_JUDGE_OUTPUT,
	CP8113_CHANNEL_3_V_MAX,
	CP8113_CHANNEL_3_V_MIN,
	CP8113_CHANNEL_3_I_MAX,
	CP8113_CHANNEL_3_I_MIN,

	CP8113_CHANNEL_4_LED_NUM = 19,
	CP8113_CHANNEL_4_JUDGE_OUTPUT,
	CP8113_CHANNEL_4_V_MAX,
	CP8113_CHANNEL_4_V_MIN,
	CP8113_CHANNEL_4_I_MAX,
	CP8113_CHANNEL_4_I_MIN,
};

enum CP8113_INPUT_REG_ADDR
{
	CP8113_MODIFY = 0,

	CP8113_CHANNEL_1_SR = 1,
	CP8113_CHANNEL_1_V_INPUT,
	CP8113_CHANNEL_1_I_INPUT,

	CP8113_CHANNEL_2_SR = 4,
	CP8113_CHANNEL_2_V_INPUT,
	CP8113_CHANNEL_2_I_INPUT,

	CP8113_CHANNEL_3_SR = 7,
	CP8113_CHANNEL_3_V_INPUT,
	CP8113_CHANNEL_3_I_INPUT,

	CP8113_CHANNEL_4_SR = 10,
	CP8113_CHANNEL_4_V_INPUT,
	CP8113_CHANNEL_4_I_INPUT,
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

/**
  * @}
  */

#endif  /* __CP8113_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


