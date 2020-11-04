
/**
  ******************************************************************************
  * @file    InfraRed_PANASONIC.c
  * @author  CZP
  * @version V1.0.0
  * @date    28-June-2018
  * @brief   PANASONIC protocol
  ******************************************************************************
  * @attention
	* ----------------------------------- PANASONIC
	* ___    8T=3.49ms    __4T=1.75ms_ A   B    C   D    E   F    G   H    I   J    K   L    
	*    |_______________|            |XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX|XXXXXXXX
	*  carry 36.67KHz,  T=0.4364ms,  E=A^B^C^D,  LK=FE^HG^JI,  1{{436,1},{1309,0}},  0{{436,1},{436,0}}
  ******************************************************************************
  */
	
/** @addtogroup Template_Project
  * @{
  */ 
/*************************************
              include
*************************************/
#include <device/dev_ir.h>

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
const IR_BufTypeDef IR_PANASONIC_Head[] = {
	{3490, IR_PIN_HIGH},
	{1750, IR_PIN_LOW},
};

const IR_BufTypeDef IR_PANASONIC_One[] = {
	{436,	 IR_PIN_HIGH},
	{1309, IR_PIN_LOW},
};

const IR_BufTypeDef IR_PANASONIC_Zero[] = {
	{436, IR_PIN_HIGH},
	{436, IR_PIN_LOW},
};

const IR_BufTypeDef IR_PANASONIC_Repeat[] = {
	{0, IR_PIN_HIGH},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_PANASONIC_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  InfraRed_RX_PANASONIC_Init
  * @param  
  * @retval 
  */
int InfraRed_PANASONIC_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_PANASONIC_Head;
	pIR_Obj->protocol_size = 97;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_PANASONIC_Calculate;
	return 0;
}

/**
  * @brief  InfraRed_RX_PANASONIC_Calculate
  * @param  
  * @retval 
  */
int InfraRed_RX_PANASONIC_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x01;
	uint16_t min0, max0, min1, max1;
	uint8_t val[12] = {0};
	
	min0 = IR_PANASONIC_Zero[1].timer * 0.8;
	max0 = IR_PANASONIC_Zero[1].timer * 1.2;
	min1 = IR_PANASONIC_One[1].timer * 0.8;
	max1 = IR_PANASONIC_One[1].timer * 1.2;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_PANASONIC_Head)/sizeof(IR_PANASONIC_Head[0])+1; idx<pIR_Obj->len; idx+=2)
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
			_bit <<= 1;
			
			if(_bit == 0x10)
			{
				_bit = 0x01;
				byte++;
			}
		}
		
		if(idx >= pIR_Obj->protocol_size)
		{
			if( (val[4] == (val[0] ^ val[1] ^ val[2] ^ val[3]))
					&& ((val[10]|(val[11]<<4)) == ((val[4]|(val[5]<<4)) ^ (val[6]|(val[7]<<4)) ^ (val[8]|(val[9]<<4))) )
			)
			{
				pIR_Obj->value.address = val[5]<<8 | val[7]<<4 | val[6];
				pIR_Obj->value.command = val[8] | (val[9]<<4);
				pIR_Obj->value.command_check = val[10] | (val[11]<<4);
				//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
			}		
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
void PANASONIC_assert_failed(uint8_t* file, uint32_t line)
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
