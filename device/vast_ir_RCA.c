
/**
  ******************************************************************************
  * @file    InfraRed_RCA.c
  * @author  CZP
  * @version V1.0.0
  * @date    29-July-2018
  * @brief   RCA protocol
  ******************************************************************************
  * @attention
	*----------------------- head[3:0] - code[7:0] - head[7:4] - check[7:0] -
  * ___   4ms   __4ms__   _ 4b head _ _ 8b code _ _ 4b ~head _ _ 8b ~code _ _
  *    |_______|       |_|XXXXXXXXXXX|XXXXXXXXXXX|XXXXXXXXXXXX|XXXXXXXXXXXX|
  * carry 38KHz, 19=500us
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

/*************************************
              variable
*************************************/
const IR_BufTypeDef IR_RCA_Head[] = {
	{4000, IR_PIN_HIGH},
	{4000, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RCA_One[] = {
	{500,	 IR_PIN_HIGH},
	{2000, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RCA_Zero[] = {
	{500, IR_PIN_HIGH},
	{1000, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RCA_Repeat[] = {
	{0, IR_PIN_HIGH},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_RCA_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_RC5_Init
  * @param  
  * @retval 
  */
int InfraRed_RCA_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_RCA_Head;
	pIR_Obj->protocol_size = 52;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->RepeatInterval = 114;	
	pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_RCA_Calculate;
	return 0;
}

/**
  * @brief  InfraRed_RX_RC5_Calculate
  * @param  
  * @retval 
  */
static int InfraRed_RX_RCA_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x80;
  uint16_t min0, max0, min1, max1;
	uint8_t val[4] = {0};
	
	min0 = IR_RCA_Zero[1].timer * 0.8;
	max0 = IR_RCA_Zero[1].timer * 1.2;
	min1 = IR_RCA_One[1].timer * 0.8;
	max1 = IR_RCA_One[1].timer * 1.2;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_RCA_Head)/sizeof(IR_RCA_Head[0])+1; idx<pIR_Obj->len; idx+=2)
		{			
			if((pIR_Obj->rx_buf[idx].timer >= min0) && (pIR_Obj->rx_buf[idx].timer <= max0))
			{
				
			}
			else if((pIR_Obj->rx_buf[idx].timer >= min1) && (pIR_Obj->rx_buf[idx].timer <= max1))
			{
				val[byte] |= _bit; 
			}
			else
			{
				break;
			}
			_bit >>= 1;
			
			if(_bit == 0x00)
			{
				_bit = 0x80;
				byte++;
			}
		}
		
//		if(idx >= pIR_Obj->protocol_size)
		{
			pIR_Obj->value.address = ((val[0]&0xf0)>>4) | ((val[1]&0x0f)<<4);
			pIR_Obj->value.command = ((val[0]&0x0f)<<4) | ((val[1]&0xf0)>>4);
			pIR_Obj->value.command_check = val[2];
			//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
		}
		IR_Obj.state = CAPTURE_STAT_IDLE;
		pIR_Obj->len = 0;
	}	
	
	return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void RCA_assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */
#endif /* VAST_USING_IR */

/************************ (C) COPYRIGHT chenzhipeng3472 *****END OF FILE****/
