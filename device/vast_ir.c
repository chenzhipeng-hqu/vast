
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
#ifdef VAST_USING_IR

/**************************************
              typedef
**************************************/
typedef struct _IR_TypeList_t
{
	IRType_e IRType;
	int (*pInfraRed_RX_Init)(struct _IR_TypeDef *);
}IR_FuncList_t;

/*************************************
         function prototypes
*************************************/
extern int InfraRed_RX_NEC_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_TX_NEC(IR_TypeDef *pIR_Obj, uint16_t head, uint8_t code, uint8_t check);
extern int InfraRed_RX_SONY_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_SAMSUNG_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_PANASONIC_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_RC5_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_RC6_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_RCA_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_MI_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RX_TOSHIBA_Init(IR_TypeDef *pIR_Obj);

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
	{IRType_NEC, 				InfraRed_RX_NEC_Init},
	{IRType_RC5, 				InfraRed_RX_RC5_Init},
	{IRType_RC6, 				InfraRed_RX_RC6_Init},
	{IRType_RCA, 				InfraRed_RX_RCA_Init},
//	{IRType_JVC, 				NULL},
//	{IRType_TRC, 				NULL},
	{IRType_SONY, 			InfraRed_RX_SONY_Init},
//	{IRType_SHARP, 			NULL},
//	{IRType_MIT, 				NULL},
//	{IRType_KONK, 			NULL},
	{IRType_MI, 				InfraRed_RX_MI_Init},
//	{IRType_NEC_WB, 		NULL},
//	{IRType_TOP, 				NULL},
	{IRType_SAMSUNG, 		InfraRed_RX_SAMSUNG_Init},
	{IRType_PANASONIC, 	InfraRed_RX_PANASONIC_Init},
	{IRType_TOSHIBA, 	InfraRed_RX_TOSHIBA_Init},
	
};
/*
 *@}
 */ /* end of paraments of IR_FuncList_t */

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_Init
  * @param  
  * @retval 
  */
int InfraRed_RX_Init(void)
{
	uint8_t IR_Typex;
	IR_Obj.IRType = IRType_NEC;
	
	for (IR_Typex=0; IR_Typex<sizeof(IR_FuncList)/sizeof(IR_FuncList[0]); IR_Typex++)
	{
		if(IR_FuncList[IR_Typex].IRType == IR_Obj.IRType)
		{
			IR_FuncList[IR_Typex].pInfraRed_RX_Init(&IR_Obj);
		}
	}	
	
	return 0;
}
/**
  * @brief  InfraRed_RX_Init
  * @param  
  * @retval 
  */
int InfraRed_RX_ChangeProtocol(IRType_e IRType)
{
	uint8_t IR_Typex;
	
	IR_Obj.IRType = IRType;
	
	for (IR_Typex=0; IR_Typex<sizeof(IR_FuncList)/sizeof(IR_FuncList[0]); IR_Typex++)
	{
		if(IR_FuncList[IR_Typex].IRType == IR_Obj.IRType)
		{
			IR_FuncList[IR_Typex].pInfraRed_RX_Init(&IR_Obj);
		}
	}	
	return 0;
}
/**
  * @brief  InfraRed_RX_Calculate
  * @param  
  * @retval 
  */
int InfraRed_RX_Calculate(void)
{
	if( NULL != IR_Obj.pInfraRed_RX_Calculate)
	{
		IR_Obj.pInfraRed_RX_Calculate(&IR_Obj);
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
#else
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

#endif
#if 0
int main(int argc, char *argv[])
{

	MX_TIM2_Init(); // PB10 TIM2_CH3
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_3);
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
