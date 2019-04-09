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
#ifndef __IM1227_H__
#define __IM1227_H__

/***********************************************
                    include
***********************************************/

/***********************************************
                    define
***********************************************/
/*----------------------------------------------------------------------------*/
// Register Addresses
#define					IM1227_CHANNEL_1				(uint16_t)((x & 0x01) << 0)
#define					IM1227_CHANNEL_2				(uint16_t)((x & 0x01) << 1)
#define					IM1227_CHANNEL_3				(uint16_t)((x & 0x01) << 2)
#define					IM1227_CHANNEL_4				(uint16_t)((x & 0x01) << 3)


/***********************************************
                    typedef
***********************************************/
enum IM1227_HOLD_REG_ADDR
{
	IM1227_MODEL_1 = 0,
	IM1227_MODEL_2,
	IM1227_RANGE_V,
	IM1227_RANGE_I,
	IM1227_CR,		//config register addr and baudrate
	
	IM1227_TOTAL_ACTIVE_ENERGY_H = 0x000C,	
	IM1227_TOTAL_ACTIVE_ENERGY_L,	
	
	IM1227_ACTIVE_V = 0x0048,
	IM1227_ACTIVE_I,
	IM1227_ACTIVE_W,
	IM1227_ACTIVE_KWH_H,
	IM1227_ACTIVE_KWH_L,
	IM1227_PF,		//power factor
	IM1227_CARBON_DIOXIDE_EMISSIONS_H,
	IM1227_CARBON_DIOXIDE_EMISSIONS_L,
	IM1227_TEMP,
	IM1227_FREQ,
};

enum IM1227_INPUT_REG_ADDR
{
	IM1227_CHANNEL_1_SR = 0,
	IM1227_CHANNEL_1_I_INPUT,
	IM1227_CHANNEL_1_V_INPUT,

	IM1227_CHANNEL_2_SR = 3,
	IM1227_CHANNEL_2_I_INPUT,
	IM1227_CHANNEL_2_V_INPUT,

	IM1227_CHANNEL_3_SR = 6,
	IM1227_CHANNEL_3_I_INPUT,
	IM1227_CHANNEL_3_V_INPUT,

	IM1227_CHANNEL_4_SR = 9,
	IM1227_CHANNEL_4_I_INPUT,
	IM1227_CHANNEL_4_V_INPUT,
	
	IM1227_CHANNEL_1_V_PEAK = 12,
	IM1227_CHANNEL_1_I_PEAK,
	IM1227_CHANNEL_1_V_VALLEY,
	IM1227_CHANNEL_1_I_VALLEY,
	
	IM1227_CHANNEL_2_V_PEAK = 16,
	IM1227_CHANNEL_2_I_PEAK,
	IM1227_CHANNEL_2_V_VALLEY,
	IM1227_CHANNEL_2_I_VALLEY,
	
	IM1227_CHANNEL_3_V_PEAK = 20,
	IM1227_CHANNEL_3_I_PEAK,
	IM1227_CHANNEL_3_V_VALLEY,
	IM1227_CHANNEL_3_I_VALLEY,
	
	IM1227_CHANNEL_4_V_PEAK = 24,
	IM1227_CHANNEL_4_I_PEAK,
	IM1227_CHANNEL_4_V_VALLEY,
	IM1227_CHANNEL_4_I_VALLEY,
	
	IM1227_MODEL_NUMBER = 60,	
	IM1227_SOFT_VERSION,
	IM1227_SYS_ERR_REG,
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

#endif  /* __IM1227_H__ */

/*************** (C) COPYRIGHT Guangzhou Shiyuan Electronics Co.,Ltd *******END OF FILE******/


