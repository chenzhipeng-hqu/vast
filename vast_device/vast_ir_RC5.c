
/**
  ******************************************************************************
  * @file    InfraRed_RC5.c
  * @author  CZP
  * @version V1.0.0
  * @date    29-June-2018
  * @brief   RC5 protocol
  ******************************************************************************
  * @attention
	* ----------------------------------- RC5
	*    _S1_     _S2_ T 
	* __|    |___|    |X|XXXXX|XXXXXX
	*  carry 36KHz,  T=0.889ms,    1{{889,0},{889,1}},  0{{889,1},{889,0}}, repreat:114ms
  ******************************************************************************
  */
	
/** @addtogroup Template_Project
  * @{
  */ 
/*************************************
              include
*************************************/
#include <vast_ir.h>

/*************************************
              define
*************************************/

/**************************************
              typedef
**************************************/

/*************************************
              variable
*************************************/
const IR_BufTypeDef IR_RC5_Head[] = {
	{889, IR_PIN_HIGH},
	{889, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RC5_One[] = {
	{889,	 IR_PIN_LOW},
	{889, IR_PIN_HIGH},
};

const IR_BufTypeDef IR_RC5_Zero[] = {
	{889, IR_PIN_HIGH},
	{889, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RC5_Repeat[] = {
	{0, IR_PIN_HIGH},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_RC5_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_RC5_Init
  * @param  
  * @retval 
  */
int InfraRed_RX_RC5_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_RC5_Head;
	pIR_Obj->protocol_size = 30;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->RepeatInterval = 114;	
	pIR_Obj->pInfraRed_RX_Calculate = InfraRed_RX_RC5_Calculate;
	return 0;
}

/**
  * @brief  InfraRed_RX_RC5_Calculate
  * @param  
  * @retval 
  */
static int InfraRed_RX_RC5_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x40;
  uint16_t minS, maxS, minT, maxT;
	uint8_t tog = 0, prev = 1;
	uint8_t val[4] = {0};
	
	minS = IR_RC5_Zero[0].timer * 0.8;
	maxS = IR_RC5_Zero[0].timer * 1.2;
	minT = minS * 2;
	maxT = maxS * 2;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=0; idx<pIR_Obj->len; idx++)
		{			
			if((pIR_Obj->rx_buf[idx].timer >= minS) && (pIR_Obj->rx_buf[idx].timer <= maxS))
			{
				tog += 1;
			}
			else if((pIR_Obj->rx_buf[idx].timer >= minT) && (pIR_Obj->rx_buf[idx].timer <= maxT))
			{
				tog += 4;
			}
			else
			{
				break;
			}
			
			if(tog == 4)
			{
				prev = !prev;
				if(prev)
				{
					val[byte] |= _bit;
				}
				_bit >>= 1;
				tog = 0;
			}
			else if(tog == 2)
			{
				if(prev)
        {
          val[byte] |= _bit;
        }
        _bit >>= 1;
        tog = 0;
			}
			else if (tog > 4)
			{
				break;
			}
			
			if(_bit == 0x00)
			{
				_bit = 0x40;
				byte++;
			}
		}
		
//		if(idx >= pIR_Obj->protocol_size)
		{
			pIR_Obj->value.address = val[0]&0x1f;
			pIR_Obj->value.command = ((~val[0]&0x40) + (val[1]>>1))&0x3f;
			pIR_Obj->value.command_check = (val[0]&0x20)?1:0;
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
void RC5_assert_failed(uint8_t* file, uint32_t line)
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

/************************ (C) COPYRIGHT chenzhipeng3472 *****END OF FILE****/
