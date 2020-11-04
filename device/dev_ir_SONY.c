
#include <device/dev_ir.h>

#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "ir.sony"
    #include "elog.h"
#else
#endif

#ifdef VAST_USING_IR
//
static int InfraRed_RX_SONY_Calculate(IR_TypeDef *pIR_Obj);

//
const IR_BufTypeDef IR_SONY_Head[] = {
	{2400, IR_PIN_HIGH},
	{600, IR_PIN_LOW},
};

const IR_BufTypeDef IR_SONY_One[] = {
	{1200,	 IR_PIN_HIGH},
	{600, IR_PIN_LOW},
};

const IR_BufTypeDef IR_SONY_Zero[] = {
	{600, IR_PIN_HIGH},
	{600, IR_PIN_LOW},
};

const IR_BufTypeDef IR_SONY_Repeat[] = {
	{9000, IR_PIN_HIGH},
	{2500, IR_PIN_LOW},
};

// carry 4000Hz, 21=560us
const uint32_t carry_freq = 40000;
const IR_BufTypeDef ir_head_sony[] = {
  {92, 1},// 2.4ms
  {24, 0},// 600us
};
const IR_BufTypeDef ir_1_sony[] = {
  {48, 1},
  {24, 0},
};
const IR_BufTypeDef ir_0_sony[] = {
  {24, 1},
  {24, 0},
};
const IR_BufTypeDef ir_tail_sony[] = {
  {24, 1},
  {24, 0},
};

/**
  * @brief  Write NEC data to IR.
  * @param  data.
  * @retval Success or fail.
  */
int InfraRed_TX_SONY_Encoder(IR_TypeDef *pIR_Obj, const void *buffer, size_t size)
{
    int ret;
    uint32_t idx;
    //uint8_t data[2];
    uint8_t *u8Buff = (uint8_t *)buffer;
    uint8_t bit, stop_bit[2] = {0x80, 0x20};
    if (pIR_Obj->idle)
    {
        pIR_Obj->tx_bufLen = 0;
        ir_tx_push_data(ir_head_sony, sizeof(ir_head_sony)/sizeof(ir_head_sony[0]));
        //data[0] = head;
        //data[1] = code;
        for(idx=0; idx<2; idx++)
        {
            for(bit=1; bit!=stop_bit[idx]; bit<<=1)
            {
                if(u8Buff[idx] & bit)
                {
                    ir_tx_push_data(ir_1_sony, sizeof(ir_1_sony)/sizeof(ir_1_sony[0]));
                }
                else
                {
                    ir_tx_push_data(ir_0_sony, sizeof(ir_0_sony)/sizeof(ir_0_sony[0]));
                }
            }
        }
        //io_irStartTim();
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
}

//
int InfraRed_RX_SONY_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x01;
	uint16_t min0, max0, min1, max1;
	uint8_t val[2] = {0};
	static uint8_t repeat_cnt = 0 , old_val[2] = {0};
    volatile IR_BufTypeDef *rx_buf = pIR_Obj->rx_buf;
	
	min0 = IR_SONY_Zero[0].timer * 0.8;
	max0 = IR_SONY_Zero[0].timer * 1.2;
	min1 = IR_SONY_One[0].timer * 0.8;
	max1 = IR_SONY_One[0].timer * 1.2;
	
	if(pIR_Obj->len != 0)
	{
        // find head
        for (idx = 0; idx < 10; idx++)
        {
            if ((rx_buf[idx].timer > IR_SONY_Head[0].timer * 0.8)
                    && (rx_buf[idx].timer < IR_SONY_Head[0].timer * 1.2)
                    && (rx_buf[idx].pin_state == IR_SONY_Head[0].pin_state))
            {
                if ((rx_buf[idx + 1].timer > IR_SONY_Head[1].timer * 0.8)
                        && (rx_buf[idx + 1].timer < IR_SONY_Head[1].timer * 1.2)
                        && (rx_buf[idx + 1].pin_state == IR_SONY_Head[1].pin_state))
                {
                    log_d("find sony head, idx=%d", idx);
                    //rx_buf += idx;
                    break;
                }
            }
        }

        // find code
		for(idx=idx + sizeof(IR_SONY_Head)/sizeof(IR_SONY_Head[0]); idx<pIR_Obj->len; idx+=2)
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
			
			if( (_bit == 0x80) && (byte == 0))
			{
				_bit = 0x01;
				byte++;
			}
			else if( (_bit == 0x00) && (byte == 1))
			{
				_bit = 0x01;
				byte++;
			}
		}
		if((old_val[0] == val[0]) && (old_val[1] == val[1]))
		{
			repeat_cnt++;
		}
		else
		{
			repeat_cnt = 0;
		}
		old_val[0] = val[0];
		old_val[1] = val[1];
		
		//if(idx >= pIR_Obj->protocol_size)
		{
			if( repeat_cnt == 0)
			{
				pIR_Obj->value.address = val[1];
				pIR_Obj->value.command = val[0];
				
			}
			else if(repeat_cnt < 3)
			{
				pIR_Obj->value.address = 0;
				pIR_Obj->value.command =0;
			}
			else
			{
				pIR_Obj->value.address = val[1];
				pIR_Obj->value.command = val[0];							
			}						
			
			//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
		}
		IR_Obj.state = CAPTURE_STAT_IDLE;
		pIR_Obj->len = 0;
	}	
	
	return 0;
}

//
int InfraRed_SONY_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_SONY_Head;
	pIR_Obj->protocol_size = 25;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_SONY_Calculate;
	pIR_Obj->carry_freq = carry_freq;
    pIR_Obj->pInfraRed_TX_Encoder = InfraRed_TX_SONY_Encoder;
    pIR_Obj->RepeatInterval = 45;
    //pIR_Obj->pInfraRed_TX_RepeatEncoder = InfraRed_TX_SONY_RepeatEncoder;
	return 0;
}

#endif /* VAST_USING_IR */

