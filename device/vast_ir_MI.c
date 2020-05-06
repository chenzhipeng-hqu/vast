
/**
  ******************************************************************************
  * @file    InfraRed_MI.c
  * @author  CZP
  * @version V1.0.0
  * @date    21-July-2018
  * @brief   XIAOMI protocol
  ******************************************************************************
  * @attention
	*----------------------- 
  * ___   1ms   _588us_  C7~C0  	D7~D0		P3~P1 588
  *    |_______|       |XXXXXXXX|XXXXXXXX|XXXX|S
  * carry 37.9KHz,  P3P2P1P0 = (C7 C6 C5 C4)^(C3 C2 C1 C0)^(D7 D6 D5 D4)^(D3 D2 D1 D0)
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
#ifdef configUSING_IR

/**************************************
              typedef
**************************************/

/*************************************
              variable
*************************************/
const IR_BufTypeDef IR_MI_Head[] = {
	{1009, IR_PIN_HIGH},
	{588, IR_PIN_LOW},
};

const IR_BufTypeDef IR_MI_ZeroZero[] = {
	{588, IR_PIN_HIGH},
	{588, IR_PIN_LOW},
};

const IR_BufTypeDef IR_MI_ZeroOne[] = {
	{588,	 IR_PIN_HIGH},
	{882, IR_PIN_LOW},
};

const IR_BufTypeDef IR_MI_OneZero[] = {
	{588,	 IR_PIN_HIGH},
	{1176, IR_PIN_LOW},
};

const IR_BufTypeDef IR_MI_OneOne[] = {
	{588,	 IR_PIN_HIGH},
	{1470, IR_PIN_LOW},
};

const IR_BufTypeDef IR_MI_Repeat[] = {
	{0, IR_PIN_HIGH},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_MI_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_RC5_Init
  * @param  
  * @retval 
  */
int InfraRed_RX_MI_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_MI_Head;
	pIR_Obj->protocol_size = 23;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->RepeatInterval = 30;	
	pIR_Obj->pInfraRed_RX_Calculate = InfraRed_RX_MI_Calculate;
	return 0;
}

/**
  * @brief  InfraRed_RX_RC5_Calculate
  * @param  
  * @retval 
  */
static int InfraRed_RX_MI_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x03;
  uint16_t min00, max00, min01, max01, min10, max10, min11, max11;
	uint8_t val[3] = {0};
	
	min00 = IR_MI_ZeroZero[1].timer * 0.9;
	max00 = IR_MI_ZeroZero[1].timer * 1.1;
	min01 = IR_MI_ZeroOne[1].timer * 0.9;
	max01 = IR_MI_ZeroOne[1].timer * 1.1;
	min10 = IR_MI_OneZero[1].timer * 0.9;
	max10 = IR_MI_OneZero[1].timer * 1.1;
	min11 = IR_MI_OneOne[1].timer * 0.9;
	max11 = IR_MI_OneOne[1].timer * 1.1;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_MI_Head)/sizeof(IR_MI_Head[0])+1; idx<pIR_Obj->len; idx+=2)
		{			
			if((pIR_Obj->rx_buf[idx].timer >= min00) && (pIR_Obj->rx_buf[idx].timer <= max00))
			{
				val[byte] |= 0x00;
			}
			else if((pIR_Obj->rx_buf[idx].timer >= min01) && (pIR_Obj->rx_buf[idx].timer <= max01))
			{
				val[byte] |= 0x01; 
			}
			else if((pIR_Obj->rx_buf[idx].timer >= min10) && (pIR_Obj->rx_buf[idx].timer <= max10))
			{
				val[byte] |= 0x02; 
			}
			else if((pIR_Obj->rx_buf[idx].timer >= min11) && (pIR_Obj->rx_buf[idx].timer <= max11))
			{
				val[byte] |= 0x03; 
			}
			else
			{
				break;
			}
					
			if(_bit == 0xC0)
			{
				_bit = 0x03;
				byte++;
			}
			else if((byte != 2))
			{
				val[byte] <<= 2;
				_bit <<= 2;
			}
			else if ((byte == 2) && (_bit != 0x0C))
			{
				val[byte] <<= 2;
				_bit <<= 2;
			}
		}
		
//		if(idx >= pIR_Obj->protocol_size)
		{
			pIR_Obj->value.address = val[0];
			pIR_Obj->value.command = val[1];
			pIR_Obj->value.command_check = val[2];
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
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
void MI_assert_failed(uint8_t* file, uint32_t line)
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
#endif /* configUSING_IR */

/************************ (C) COPYRIGHT chenzhipeng3472 *****END OF FILE****/
