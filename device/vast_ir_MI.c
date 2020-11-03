
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
#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "ir.mi"
    #include "elog.h"
#else
#endif

#ifdef VAST_USING_IR
// ___   1ms   _588us_  C7~C0  	D7~D0		P3~P1 588
//    |_______|       |XXXXXXXX|XXXXXXXX|XXXX|S
//  carry 37.9KHz,  P3P2P1P0 = (C7 C6 C5 C4)^(C3 C2 C1 C0)^(D7 D6 D5 D4)^(D3 D2 D1 D0) 
static const uint16_t carry_freq = 37900;

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

const IR_BufTypeDef ir_head_mi[] = {
  {38, 1},// 1.0088ms
  {22, 0},// 0.588ms
	{22, 1},
};
const IR_BufTypeDef ir_00_mi[] = {
  {22, 0},
  {22, 1},
};
const IR_BufTypeDef ir_01_mi[] = {
  {33, 0},
  {22, 1},
};
const IR_BufTypeDef ir_10_mi[] = {
  {44, 0},
  {22, 1},
};
const IR_BufTypeDef ir_11_mi[] = {
  {55, 0},
  {22, 1},
};


/*************************************
         function prototypes
*************************************/
static int InfraRed_RX_MI_Calculate(IR_TypeDef *pIR_Obj);

/*************************************
              function
*************************************/

/**
  * @brief  Write NEC data to IR.
  * @param  data.
  * @retval Success or fail.
  */
int InfraRed_TX_MI_Encoder(IR_TypeDef *pIR_Obj, const void *buffer, size_t size)
{
    uint8_t ret;
    static uint32_t idx;
    uint8_t data[3];
    uint8_t *u8Buff = (uint8_t *)buffer;
    uint8_t bit, send_data, stop_bit[3] = {8, 8, 4};
    uint16_t head = u8Buff[0] << 8 | u8Buff[1];
    uint8_t code1 = u8Buff[2];
    if(pIR_Obj->idle)
    {
        pIR_Obj->tx_bufLen = 0;
        ir_tx_push_data(ir_head_mi, sizeof(ir_head_mi)/sizeof(ir_head_mi[0]));
        data[0] = head&0x0ff;
        data[1] = code1;		
        data[2] = ((head>>4) ^ head ^ (code1>>4) ^ code1)&0x0f;
        for(idx=0; idx<3; idx++)
        {
            for(bit=0; bit!=stop_bit[idx]; bit+=2)
            {
                send_data = (data[idx]>>(stop_bit[idx]-2-bit))&0x03;
                if((send_data) == 0x00)
                {
                    ir_tx_push_data(ir_00_mi, sizeof(ir_00_mi)/sizeof(ir_00_mi[0]));
                }
                else if((send_data) == 0x01)
                {
                    ir_tx_push_data(ir_01_mi, sizeof(ir_01_mi)/sizeof(ir_01_mi[0]));
                }
                else if((send_data) == 0x02)
                {
                    ir_tx_push_data(ir_10_mi, sizeof(ir_10_mi)/sizeof(ir_10_mi[0]));
                }
                else if((send_data) == 0x03)
                {
                    ir_tx_push_data(ir_11_mi, sizeof(ir_11_mi)/sizeof(ir_11_mi[0]));
                }
            }
        }
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
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
    volatile IR_BufTypeDef *rx_buf = pIR_Obj->rx_buf;
	
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
        // find head
        for (idx = 0; idx < 10; idx++)
        {
            if ((rx_buf[idx].timer > IR_MI_Head[0].timer * 0.8)
                    && (rx_buf[idx].timer < IR_MI_Head[0].timer * 1.2)
                    && (rx_buf[idx].pin_state == IR_MI_Head[0].pin_state))
            {
                if ((rx_buf[idx + 1].timer > IR_MI_Head[1].timer * 0.8)
                        && (rx_buf[idx + 1].timer < IR_MI_Head[1].timer * 1.2)
                        && (rx_buf[idx + 1].pin_state == IR_MI_Head[1].pin_state))
                {
                    log_d("find mi head, idx=%d", idx);
                    //rx_buf += idx;
                    break;
                }
            }
        }

        // find code
		for(idx=idx + sizeof(IR_MI_Head)/sizeof(IR_MI_Head[0])+1; idx<pIR_Obj->len; idx+=2)
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
			//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
		}
		IR_Obj.state = CAPTURE_STAT_IDLE;
		pIR_Obj->len = 0;
	}	
	
	return 0;
}

/**
  * @brief  InfraRed_RX_RC5_Init
  * @param  
  * @retval 
  */
int InfraRed_MI_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_MI_Head;
	pIR_Obj->protocol_size = 23;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->RepeatInterval = 30;	
	pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_MI_Calculate;
	pIR_Obj->carry_freq = carry_freq;
    pIR_Obj->pInfraRed_TX_Encoder = InfraRed_TX_MI_Encoder;
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
#endif /* VAST_USING_IR */

/************************ (C) COPYRIGHT chenzhipeng3472 *****END OF FILE****/
