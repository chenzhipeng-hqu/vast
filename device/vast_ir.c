
/**
  ******************************************************************************
  * @file    InfraRed.c
  * @author  CZP
  * @version V1.0.0
  * @date    29-June-2018
  * @brief   InfraRed protocol
  ******************************************************************************
  * @attention
	* 
  ******************************************************************************
  */
	
/** @addtogroup Template_Project
  * @{
  */ 
	
/*************************************
              include
*************************************/
#include <device/vast_ir.h>

/*************************************
              define
*************************************/
#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "ir.dev"
    #include "elog.h"
#else
#endif

#ifdef VAST_USING_IR

/**************************************
              typedef
**************************************/
typedef struct _IR_TypeList_t
{
	IRType_e IRType;
	int (*pInfraRed_Init)(struct _IR_TypeDef *);
}IR_FuncList_t;

/*************************************
         function prototypes
*************************************/

/*************************************
              variable
*************************************/
IR_TypeDef IR_Obj = {0};

/**
 *@brief:	paraments of IR_FuncList_t
 *@{
 */
const IR_FuncList_t IR_FuncList[] = { 	//don't change sequence, add after last
//	{IRType_NONE, 			NULL},
	{IRType_NEC, 				InfraRed_NEC_Init},
	{IRType_RC5, 				InfraRed_RC5_Init},
	{IRType_RC6, 				InfraRed_RC6_Init},
	{IRType_RCA, 				InfraRed_RCA_Init},
//	{IRType_JVC, 				NULL},
//	{IRType_TRC, 				NULL},
	{IRType_SONY, 			InfraRed_SONY_Init},
//	{IRType_SHARP, 			NULL},
//	{IRType_MIT, 				NULL},
//	{IRType_KONK, 			NULL},
	{IRType_MI, 				InfraRed_MI_Init},
//	{IRType_NEC_WB, 		NULL},
//	{IRType_TOP, 				NULL},
	{IRType_SAMSUNG, 		InfraRed_SAMSUNG_Init},
	{IRType_PANASONIC, 	InfraRed_PANASONIC_Init},
	{IRType_TOSHIBA, 	InfraRed_TOSHIBA_Init},
	{IRType_KONKA, 	InfraRed_KONKA_Init},
	
};
/*
 *@}
 */ /* end of paraments of IR_FuncList_t */

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_Init
  * @param  
  * @retval 
  */
int InfraRed_Init(IRType_e IRType)
{
	uint8_t idx;
	
	for (idx=0; idx<sizeof(IR_FuncList)/sizeof(IR_FuncList[0]); idx++)
	{
		if(IR_FuncList[idx].IRType == IRType)
		{
			IR_FuncList[idx].pInfraRed_Init(&IR_Obj);
            IR_Obj.IRType = IRType;
		}
	}	
	
	return 0;
}
/**
  * @brief  InfraRed_RX_Init
  * @param  
  * @retval 
  */
int InfraRed_SetProtocol(IRType_e IRType)
{
    InfraRed_Init(IRType);
	return 0;
}

/**
  * @brief  InfraRed_RX_Decoder
  * @param  
  * @retval 
  */
int InfraRed_RX_Decoder(void)
{
    IR_Obj.RxRepeat = 0;
	if( NULL != IR_Obj.pInfraRed_RX_Decoder)
	{
		IR_Obj.pInfraRed_RX_Decoder(&IR_Obj);
	}
    else
    {
        log_w("didn't set ir decoder");
    }
	return 0;
}

/**
  * @brief  InfraRed_TX_Encoder
  * @param  
  * @retval 
  */
int InfraRed_TX_Encoder(struct _IR_TypeDef *pIr, const void *buffer, size_t size)
{
	if( NULL != pIr->pInfraRed_TX_Encoder)
	{
		pIr->pInfraRed_TX_Encoder(&IR_Obj, buffer, size);
	}
    else
    {
        log_w("didn't set ir encoder");
    }
	return 0;
}

/**
  * @brief  InfraRed_TX_Encoder
  * @param  
  * @retval 
  */
int InfraRed_TX_RepeatEncoder(struct _IR_TypeDef *pIr, const void *buffer, size_t size)
{
	if (NULL != pIr->pInfraRed_TX_RepeatEncoder)
	{
		pIr->pInfraRed_TX_RepeatEncoder(&IR_Obj, buffer, size);
	}
	return 0;
}

/**
  * @brief  Push ir data to send buf.
  * @param  data.
  * @retval Success or fail.
  */
int ir_tx_push_data(const IR_BufTypeDef *pData, uint32_t len)
{
  uint8_t ret = VAST_OK;
  uint32_t idx;
  for(idx=0; idx<len; idx++)
  {
    IR_Obj.tx_buf[IR_Obj.tx_bufLen++] = pData[idx];
    if(IR_Obj.tx_bufLen >= INFRARED_BUFF_SIZE)
    {
      ret = VAST_ERROR;
      break;
    }
  }
  return ret;
}

#if 0
/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
    {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
			IR_Obj.rx_buf[IR_Obj.len].timer = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_3);			
			IR_Obj.rx_buf[IR_Obj.len].pin_state = (IR_PIN_State)IR_RX_GET_PIN_STATE();
			
			if( (IR_Obj.state == CAPTURE_STAT_IDLE)
					&& (IR_Obj.rx_buf[0].timer > IR_Obj.pHead[0].timer*0.8) 
					&& (IR_Obj.rx_buf[0].timer < IR_Obj.pHead[0].timer*1.2)
					&& (IR_Obj.rx_buf[0].pin_state == IR_Obj.pHead[0].pin_state)
			)
			{
				if (IR_Obj.len == 0)
				{
					IR_Obj.len++;
					IR_Obj.rx_buf[1].timer = 0;
				}	
				else if((IR_Obj.state == CAPTURE_STAT_IDLE)
					&& (IR_Obj.rx_buf[1].timer > IR_Obj.pHead[1].timer*0.8) 
					&& (IR_Obj.rx_buf[1].timer < IR_Obj.pHead[1].timer*1.2)
					&& (IR_Obj.rx_buf[1].pin_state == IR_Obj.pHead[1].pin_state)
				)
				{
					IR_Obj.state |= CAPTURE_STAT_CAPTURE_HEAD_FLAG;
				}	
				else
				{
					IR_Obj.len = 0;
				}				
			}
			
			if( (IR_Obj.state & CAPTURE_STAT_CAPTURE_HEAD_FLAG) 
			)
			{
				
				IR_Obj.len++;
				if( (IR_Obj.len > IR_Obj.protocol_size)
				)
				{
					IR_Obj.state &= ~CAPTURE_STAT_CAPTURE_HEAD_FLAG;
					IR_Obj.state |= CAPTURE_STAT_CAPTURE_DONE;
				}
			}
			
			if(IR_Obj.len >= 100)
			{
				IR_Obj.len = 0;
				IR_Obj.state = CAPTURE_STAT_IDLE;
			}
			__HAL_TIM_SET_COUNTER(htim, 0);	
		}
	}
}
#elif 0
/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void ir_rx_irq_callback(uint32_t cnt, IR_PIN_State pin_state)
{
    IR_Obj.rx_buf[IR_Obj.len].timer = cnt;			
    IR_Obj.rx_buf[IR_Obj.len].pin_state = pin_state;

    if( (IR_Obj.state == CAPTURE_STAT_IDLE)
            && (IR_Obj.rx_buf[0].timer > IR_Obj.pHead[0].timer*0.8) 
            && (IR_Obj.rx_buf[0].timer < IR_Obj.pHead[0].timer*1.2)
            && (IR_Obj.rx_buf[0].pin_state == IR_Obj.pHead[0].pin_state)
      )
    {
        if (IR_Obj.len == 0)
        {
            IR_Obj.len++;
            IR_Obj.rx_buf[1].timer = 0;
        }	
        else if((IR_Obj.state == CAPTURE_STAT_IDLE)
                && (IR_Obj.rx_buf[1].timer > IR_Obj.pHead[1].timer*0.8) 
                && (IR_Obj.rx_buf[1].timer < IR_Obj.pHead[1].timer*1.2)
                && (IR_Obj.rx_buf[1].pin_state == IR_Obj.pHead[1].pin_state)
               )
        {
            IR_Obj.state |= CAPTURE_STAT_CAPTURE_HEAD_FLAG;
        }	
        else
        {
            IR_Obj.len = 0;
        }				
    }

    if( (IR_Obj.state & CAPTURE_STAT_CAPTURE_HEAD_FLAG) 
      )
    {

        IR_Obj.len++;
        if( (IR_Obj.len > IR_Obj.protocol_size)
          )
        {
            IR_Obj.state &= ~CAPTURE_STAT_CAPTURE_HEAD_FLAG;
            IR_Obj.state |= CAPTURE_STAT_CAPTURE_DONE;
        }
    }

    if(IR_Obj.len >= 100)
    {
        IR_Obj.len = 0;
        IR_Obj.state = CAPTURE_STAT_IDLE;
    }
}
#else
/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  * @retval None
  */
void ir_rx_irq_callback(uint32_t cnt, IR_PIN_State pin_state)
{
    if (IR_Obj.len < INFRARED_BUFF_SIZE)
    {
        IR_Obj.rx_buf[IR_Obj.len].timer = cnt;			
        IR_Obj.rx_buf[IR_Obj.len].pin_state = pin_state;
        IR_Obj.len++;
    }
}

#endif
#if 0
void EXTI15_10_IRQHandler(void)
{
  if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10) != RESET)
  {
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
    ir_rx_irq_callback(LL_TIM_GetCounter(TIM6), IR_RX_GET_PIN_STATE());
    LL_TIM_SetCounter(TIM6, 0);
  }
}
int main(int argc, char *argv[])
{
    MX_GPIO_Init();  // set PB10 rising/falling edge
    MX_TIM6_Init();
    //printf("SystemCoreClock:%ld\r\n", (SystemCoreClock/1000000) - 1);
    LL_TIM_SetPrescaler(TIM6, (SystemCoreClock/1000000) - 1); // div to 1us
    LL_TIM_EnableCounter(TIM6);
	InfraRed_RX_Init();

	while(1)
	{
		if(IR_Obj.state & CAPTURE_STAT_CAPTURE_DONE)
		{
			for(i=0; i<IR_Obj.len; i++)
			{
					printf("IR_Obj[%d]: %d, %d, len=%d\r\n", i,
							IR_Obj.rx_buf[i].timer, IR_Obj.rx_buf[i].pin_state, IR_Obj.len);

			}

			InfraRed_RX_Calculate();

			printf("IR_Obj: 0x%04X, 0x%02X, 0x%02X\r\n",
					IR_Obj.value.address, IR_Obj.value.command, IR_Obj.value.command_check);

			IR_Obj.len = 0;
		}
	}
	return 0;
}

#endif /*test*/

#endif /*VAST_USING_IR*/
/**
  * @}
  */

/************************ (C) COPYRIGHT chenzhipeng3472 *****END OF FILE****/
