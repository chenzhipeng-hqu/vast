
#include <device/vast_ir.h>

#ifdef configUSING_IR
//

//
const IR_BufTypeDef IR_JVC_Head[] = {
	{8400, IR_PIN_HIGH},
	{4200, IR_PIN_LOW},
};

const IR_BufTypeDef IR_JVC_One[] = {
	{526,	 IR_PIN_HIGH},
	{1574, IR_PIN_LOW},
};

const IR_BufTypeDef IR_JVC_Zero[] = {
	{526, IR_PIN_HIGH},
	{524, IR_PIN_LOW},
};

const IR_BufTypeDef IR_JVC_Repeat[] = {
	{9000, IR_PIN_HIGH},
	{2200, IR_PIN_LOW},
	{600, IR_PIN_HIGH},
};


//
int InfraRed_RX_JVC_Init(IR_TypeDef *pIR_Obj)
{
	pIR_Obj->pHead = IR_JVC_Head;
	pIR_Obj->protocol_size = 25;
	pIR_Obj->state = CAPTURE_STAT_IDLE;
	return 0;
}

//
int InfraRed_RX_JVC_Calculate(IR_TypeDef *pIR_Obj)
{
	uint8_t idx = 0, byte = 0, _bit = 0x01;
	uint16_t min0, max0, min1, max1;
	uint8_t val[2] = {0};
	static uint8_t repeat_cnt = 0 , old_val[2] = {0};
	
	min0 = IR_JVC_Zero[1].timer * 0.8;
	max0 = IR_JVC_Zero[1].timer * 1.2;
	min1 = IR_JVC_One[1].timer * 0.8;
	max1 = IR_JVC_One[1].timer * 1.2;
	
	if(pIR_Obj->len != 0)
	{
		for(idx=sizeof(IR_JVC_Head)/sizeof(IR_JVC_Head[0])+1; idx<pIR_Obj->len; idx+=2)
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
				pIR_Obj->value.address = val[0];
				pIR_Obj->value.command = val[1];
				
			}
			else if(repeat_cnt < 3)
			{
				pIR_Obj->value.address = 0;
				pIR_Obj->value.command =0;
			}
			else
			{
				pIR_Obj->value.address = val[0];
				pIR_Obj->value.command = val[1];							
			}						
			
			HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);	
		}
		IR_Obj.state = CAPTURE_STAT_IDLE;
		pIR_Obj->len = 0;
	}	
	
	return 0;
}
#endif /* configUSING_IR */

