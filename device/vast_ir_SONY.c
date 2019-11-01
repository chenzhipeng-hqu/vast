
#include <device/vast_ir.h>

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


//
int InfraRed_RX_SONY_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_SONY_Head;
	pIR_Obj->protocol_size = 25;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->pInfraRed_RX_Calculate = InfraRed_RX_SONY_Calculate;
	return 0;
}

//
int InfraRed_RX_SONY_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x01;
	uint16_t min0, max0, min1, max1;
	uint8_t val[2] = {0};
	static uint8_t repeat_cnt = 0 , old_val[2] = {0};
	
	min0 = IR_SONY_Zero[0].timer * 0.8;
	max0 = IR_SONY_Zero[0].timer * 1.2;
	min1 = IR_SONY_One[0].timer * 0.8;
	max1 = IR_SONY_One[0].timer * 1.2;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_SONY_Head)/sizeof(IR_SONY_Head[0]); idx<pIR_Obj->len; idx+=2)
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
		
		if(idx >= pIR_Obj->protocol_size)
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
			
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
		}
		IR_Obj.state = CAPTURE_STAT_IDLE;
		pIR_Obj->len = 0;
	}	
	
	return 0;
}

