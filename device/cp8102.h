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
#ifndef __CP8102_H__
#define __CP8102_H__

/***********************************************
                    include
***********************************************/

/***********************************************
                    define
***********************************************/
/*----------------------------------------------------------------------------*/
// Register Addresses
#define					CP8102_CHANNEL_1				(uint16_t)((x & 0x01) << 0)
#define					CP8102_CHANNEL_2				(uint16_t)((x & 0x01) << 1)
#define					CP8102_CHANNEL_3				(uint16_t)((x & 0x01) << 2)
#define					CP8102_CHANNEL_4				(uint16_t)((x & 0x01) << 3)


/***********************************************
                    typedef
***********************************************/
enum CP8102_HOLD_REG_ADDR
{
	CP8102_CHANNEL_SWITCH = 0,
	
	CP8102_CHANNEL_1_MODE = 1,
	CP8102_CHANNEL_1_VO,
	CP8102_CHANNEL_1_IO,
	CP8102_CHANNEL_1_RD,
	CP8102_CHANNEL_1_RC,
	CP8102_CHANNEL_1_VON = 6,
	CP8102_CHANNEL_1_INPUT_LINE_IMPEDANCE = 7,
	CP8102_CHANNEL_1_JUDGE_OUTPUT,
	CP8102_CHANNEL_1_V_MAX,
	CP8102_CHANNEL_1_V_MIN,
	CP8102_CHANNEL_1_I_MAX,
	CP8102_CHANNEL_1_I_MIN,
	
	CP8102_CHANNEL_2_MODE = 14,
	CP8102_CHANNEL_2_VO,
	CP8102_CHANNEL_2_IO,
	CP8102_CHANNEL_2_RD,
	CP8102_CHANNEL_2_RC,
	CP8102_CHANNEL_2_VON = 19,
	CP8102_CHANNEL_2_INPUT_LINE_IMPEDANCE = 20,
	CP8102_CHANNEL_2_JUDGE_OUTPUT,
	CP8102_CHANNEL_2_V_MAX,
	CP8102_CHANNEL_2_V_MIN,
	CP8102_CHANNEL_2_I_MAX,
	CP8102_CHANNEL_2_I_MIN,
	
	CP8102_CHANNEL_3_MODE = 27,
	CP8102_CHANNEL_3_VO,
	CP8102_CHANNEL_3_IO,
	CP8102_CHANNEL_3_RD,
	CP8102_CHANNEL_3_RC,
	CP8102_CHANNEL_3_VON = 32,
	CP8102_CHANNEL_3_INPUT_LINE_IMPEDANCE = 33,
	CP8102_CHANNEL_3_JUDGE_OUTPUT,
	CP8102_CHANNEL_3_V_MAX,
	CP8102_CHANNEL_3_V_MIN,
	CP8102_CHANNEL_3_I_MAX,
	CP8102_CHANNEL_3_I_MIN,
	
	CP8102_CHANNEL_4_MODE = 40,
	CP8102_CHANNEL_4_VO,
	CP8102_CHANNEL_4_IO,
	CP8102_CHANNEL_4_RD,
	CP8102_CHANNEL_4_RC,
	CP8102_CHANNEL_4_VON = 45,
	CP8102_CHANNEL_4_INPUT_LINE_IMPEDANCE = 46,	
	CP8102_CHANNEL_4_JUDGE_OUTPUT,
	CP8102_CHANNEL_4_V_MAX,
	CP8102_CHANNEL_4_V_MIN,
	CP8102_CHANNEL_4_I_MAX,
	CP8102_CHANNEL_4_I_MIN,
	
	CP8102_CHANNEL_1_VonLatch = 245,
	CP8102_CHANNEL_2_VonLatch,
	CP8102_CHANNEL_3_VonLatch,
	CP8102_CHANNEL_4_VonLatch,
	
	CP8102_CHANNEL_IN_PARALLEL_MODE = 249,
	CP8102_V_I_AVERAGE_UPDATE_PERIOD = 250,  	//default: 38000
	CP8102_V_I_PEAK_UPDATE_PERIOD = 251,			//default: 30500
};

enum CP8102_INPUT_REG_ADDR
{
	CP8102_CHANNEL_1_SR = 0,
	CP8102_CHANNEL_1_I_INPUT,
	CP8102_CHANNEL_1_V_INPUT,

	CP8102_CHANNEL_2_SR = 3,
	CP8102_CHANNEL_2_I_INPUT,
	CP8102_CHANNEL_2_V_INPUT,

	CP8102_CHANNEL_3_SR = 6,
	CP8102_CHANNEL_3_I_INPUT,
	CP8102_CHANNEL_3_V_INPUT,

	CP8102_CHANNEL_4_SR = 9,
	CP8102_CHANNEL_4_I_INPUT,
	CP8102_CHANNEL_4_V_INPUT,
	
	CP8102_CHANNEL_1_V_PEAK = 12,
	CP8102_CHANNEL_1_I_PEAK,
	CP8102_CHANNEL_1_V_VALLEY,
	CP8102_CHANNEL_1_I_VALLEY,
	
	CP8102_CHANNEL_2_V_PEAK = 16,
	CP8102_CHANNEL_2_I_PEAK,
	CP8102_CHANNEL_2_V_VALLEY,
	CP8102_CHANNEL_2_I_VALLEY,
	
	CP8102_CHANNEL_3_V_PEAK = 20,
	CP8102_CHANNEL_3_I_PEAK,
	CP8102_CHANNEL_3_V_VALLEY,
	CP8102_CHANNEL_3_I_VALLEY,
	
	CP8102_CHANNEL_4_V_PEAK = 24,
	CP8102_CHANNEL_4_I_PEAK,
	CP8102_CHANNEL_4_V_VALLEY,
	CP8102_CHANNEL_4_I_VALLEY,
	
	CP8102_MODEL_NUMBER = 60,	
	CP8102_SOFT_VERSION,
	CP8102_SYS_ERR_REG,
};

enum SYSTEM_STATUS_REG
{
	CP8102_CHANNEL_SR_NO_RESPONSE = 0,
	CP8102_CHANNEL_SR_LOAD_INPUT_OFF,
	CP8102_CHANNEL_SR_LOAD_INPUT_ON,
	CP8102_CHANNEL_SR_OVER_TEMP_PROTECTION,
	CP8102_CHANNEL_SR_OVER_CURRENT_PROTECTION,
	CP8102_CHANNEL_SR_OVER_POWER_PROTECTION,
};

enum OUTPUT_ENABLE_TYPE
{
	CP8102_CHANNEL_NO_OUTPUT_ENABLE = 0,
	CP8102_CHANNEL_OUTPUT_ENABLE_VOLTAGE,
	CP8102_CHANNEL_OUTPUT_ENABLE_CURRENT,
	CP8102_CHANNEL_OUTPUT_ENABLE_CURRENT_VOLTAGE,
};

enum
{
	CP8102_CHANNEL_MODE_CC = 0,
	CP8102_CHANNEL_MODE_CV,
	CP8102_CHANNEL_MODE_CR,
	CP8102_CHANNEL_MODE_CP,
	CP8102_CHANNEL_MODE_LED,
};

typedef struct inputRegAddrMap
{
	unsigned char status;
	unsigned char input_I;
	unsigned char input_V;
}INPUTREG_ADDR;


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

#endif  /* __CP8102_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


