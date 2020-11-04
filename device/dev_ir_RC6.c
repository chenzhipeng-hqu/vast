
/**
  ******************************************************************************
  * @file    InfraRed_RC6.c
  * @author  CZP
  * @version V1.0.0
  * @date    29-June-2018
  * @brief   RC6 protocol
  ******************************************************************************
  * @attention
	* ----------------------------------- RC6
	*    _L_    _S_    _M_    _T_  Control  Imfo
	* __|   |__|   |__|   |__|   |XXXXXXXX|XXXXXXXX
	*  carry 36KHz,  T=0.444444ms,    1{{445,0},{444,1}},  0{{445,1},{444,0}}, repreat:114ms
  ******************************************************************************
  */
	
/** @addtogroup Template_Project
  * @{
  */ 
/*************************************
              include
*************************************/
#include <device/dev_ir.h>
#include "vast_common.h"

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
const IR_BufTypeDef IR_RC6_Head[] = {
	{6*445, IR_PIN_HIGH},
	{2*445, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RC6_One[] = {
	{445,	 IR_PIN_HIGH},
	{444, IR_PIN_LOW},
};

const IR_BufTypeDef IR_RC6_Zero[] = {
	{445, IR_PIN_LOW},
	{444, IR_PIN_HIGH},
};

const IR_BufTypeDef IR_RC6_Repeat[] = {
	{0, IR_PIN_HIGH},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_RC6_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_RC6_Init
  * @param  
  * @retval 
  */
int InfraRed_RC6_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_RC6_Head;
	pIR_Obj->protocol_size = 40;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->RepeatInterval = 110;	
	pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_RC6_Calculate;
	return 0;
}

/**
  * @brief  InfraRed_RX_RC6_Calculate
  * @param  
  * @retval 
  */
static int InfraRed_RX_RC6_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x01;
  uint16_t minS, maxS, minT, maxT, minTrailer, maxTrailer;
	uint8_t tog = 0, prev = 1;
	uint8_t val[5] = {0};
	
	minS = IR_RC6_Zero[0].timer * 0.8;
	maxS = IR_RC6_Zero[0].timer * 1.2;
	minT = minS * 2;
	maxT = maxS * 2;
	minTrailer = minS * 3;
	maxTrailer = maxS * 3;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_RC6_Head)/sizeof(IR_RC6_Head[0]); idx<pIR_Obj->len; idx++)
		{			
			if((pIR_Obj->rx_buf[idx].timer >= minS) && (pIR_Obj->rx_buf[idx].timer <= maxS))
			{
				tog += 1;
				if(idx == 8)
				{	
					byte++;
					_bit >>= 1;
					idx += 2;
					tog = 0;
					
					printf("1. idx=%d, prev=%d, tog=%d, _bit=0x%02x \r\n", idx, prev, tog, _bit);
				}
			}
			else if((pIR_Obj->rx_buf[idx].timer >= minT) && (pIR_Obj->rx_buf[idx].timer <= maxT))
			{
				tog += 4;
			}
			else if((pIR_Obj->rx_buf[idx].timer >= minTrailer) && (pIR_Obj->rx_buf[idx].timer <= maxTrailer))
			{
				tog += 6;		
			}
			else if((pIR_Obj->rx_buf[idx].timer >= minTrailer) && (pIR_Obj->rx_buf[idx].timer <= maxTrailer))
			{
				tog += 6;		
			}
			else
			{
				printf("6. idx=%d, prev=%d, tog=%d, _bit=0x%02x \r\n", idx, prev, tog, _bit);
				break;
			}
			
			if(tog == 6)
			{
				byte++;
				val[byte] |= _bit;
				_bit >>= 1;
				tog = 0;
				idx++;
				printf("2. idx=%d, prev=%d, tog=%d, _bit=0x%02x \r\n", idx, prev, tog, _bit);
			}
			else if((tog == 4) | (tog == 5))
			{
				prev = !prev;
				if(!prev)
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
			else if (tog > 6)
			{
				break;
			}
			
			if(_bit == 0x00)
			{
				if(byte == 0)
				{
					_bit = 0x04;
				}
				else if(byte == 1)
				{
					_bit = 0x01;
				}
				else if( (byte == 2) | (byte == 3))
				{
					printf("3. idx=%d, prev=%d\r\n", idx, prev);
					_bit = 0x80;
				}
				else if(byte == 4)
				{
					printf("4. idx=%d, prev=%d, tog=%d, _bit=0x%02x \r\n", idx, prev, tog, _bit);
					break;
				}
				byte++;
			}
		}
		
		if(tog == 0)
		{
			val[byte] |= _bit;
			_bit >>= 1;
		}
		
		printf("tog=%d, idx=%d, start: 0x%02x, mode: 0x%02x, trailer: 0x%02x, control: 0x%02x, info: 0x%02x\r\n",
										tog, idx, val[0], val[1], val[2], val[3], val[4]);
		
//		if(idx >= pIR_Obj->protocol_size)
		{
			pIR_Obj->value.address = val[3];
			pIR_Obj->value.command = val[4];
			pIR_Obj->value.command_check = val[4];
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
void RC6_assert_failed(uint8_t* file, uint32_t line)
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
