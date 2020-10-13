
#include <device/vast_ir.h>

#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "ir.nec"
    #include "elog.h"
#else
#endif

#ifdef VAST_USING_IR
//
//----------------------------------- NEC
//___     9ms     _4.5ms_   _ 8b head _ _ 8b ~head _ _ 8b code _ _ 8b ~code _ _
//   |___________|       |_|XXXXXXXXXXX|XXXXXXXXXXXX|XXXXXXXXXXX|XXXXXXXXXXXX|
// carry 37500Hz, 21=560us
static const uint16_t carry_freq = 37500;

//
const IR_BufTypeDef IR_NEC_Head[] =
{
    {9000, IR_PIN_HIGH},
    {4500, IR_PIN_LOW},
};

const IR_BufTypeDef IR_NEC_One[] =
{
    {560,    IR_PIN_HIGH},
    {1685, IR_PIN_LOW},
};

const IR_BufTypeDef IR_NEC_Zero[] =
{
    {560, IR_PIN_HIGH},
    {560, IR_PIN_LOW},
};

const IR_BufTypeDef IR_NEC_Repeat[] =
{
    {9000, IR_PIN_HIGH},
    {2250, IR_PIN_LOW},
    {560, IR_PIN_HIGH},
};

const IR_BufTypeDef ir_head_nec[] =
{
    {168, 1},// 9/2ms
    {168, 1},// 9/2ms
    {168, 0},// 4.5ms
    {21, 1},// 560us
};
const IR_BufTypeDef ir_1_nec[] =
{
    {63, 0},
    {21, 1},
};
const IR_BufTypeDef ir_0_nec[] =
{
    {21, 0},
    {21, 1},
};
const IR_BufTypeDef ir_repeat_nec[] =
{
    {168, 1},// 9/2ms
    {168, 1},// 9/2ms
    {84, 0},// 4.5ms
    {21, 1},// 560us
};

//
int InfraRed_TX_NEC_Encoder(IR_TypeDef *pIR_Obj, const void *buffer, size_t size)
{
    uint8_t ret;
    uint32_t idx;
    //uint8_t data[4];
    uint8_t *u8Buff = (uint8_t *)buffer;
    uint8_t bit;
    if (pIR_Obj->idle)
    {
        pIR_Obj->tx_bufLen = 0;
        ir_tx_push_data(ir_head_nec, sizeof(ir_head_nec) / sizeof(ir_head_nec[0]));
        //data[0] = head>>8;  // head high bit first
        //data[1] = head&0x0ff;
        //data[2] = code;
        //data[3] = check;
        for (idx = 0; idx < size; idx++)
        {
            for (bit = 1; bit != 0; bit <<= 1)
            {
                if (u8Buff[idx] & bit)
                {
                    ir_tx_push_data(ir_1_nec, sizeof(ir_1_nec) / sizeof(ir_1_nec[0]));
                }
                else
                {
                    ir_tx_push_data(ir_0_nec, sizeof(ir_0_nec) / sizeof(ir_0_nec[0]));
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
int InfraRed_TX_NEC_RepeatEncoder(IR_TypeDef *pIR_Obj, const void *buffer, size_t size)
{
    pIR_Obj->tx_bufLen = 0;
    ir_tx_push_data(ir_repeat_nec, sizeof(ir_repeat_nec) / sizeof(ir_repeat_nec[0]));
    return 0;
}

//
int InfraRed_RX_NEC_Decoder(IR_TypeDef *pIR_Obj)
{
    uint8_t idx = 0, byte = 0, _bit = 0x01;
    uint16_t min0, max0, min1, max1;
    uint8_t val[4] = {0};
    volatile IR_BufTypeDef *rx_buf = pIR_Obj->rx_buf;

    min0 = IR_NEC_Zero[1].timer * 0.8;
    max0 = IR_NEC_Zero[1].timer * 1.2;
    min1 = IR_NEC_One[1].timer * 0.8;
    max1 = IR_NEC_One[1].timer * 1.2;

    if (pIR_Obj->len != 0)
    {
        // find head
        for (idx = 0; idx < 10; idx++)
        {
            if ((rx_buf[idx].timer > IR_NEC_Head[0].timer * 0.8)
                    && (rx_buf[idx].timer < IR_NEC_Head[0].timer * 1.2)
                    && (rx_buf[idx].pin_state == IR_NEC_Head[0].pin_state))
            {
                if ((rx_buf[idx + 1].timer > IR_NEC_Head[1].timer * 0.8)
                        && (rx_buf[idx + 1].timer < IR_NEC_Head[1].timer * 1.2)
                        && (rx_buf[idx + 1].pin_state == IR_NEC_Head[1].pin_state))
                {
                    log_d("find nec head, idx=%d", idx);
                    //rx_buf += idx;
                    break;
                }
            }
        }

        // find code
        for (idx = idx + sizeof(IR_NEC_Head) / sizeof(IR_NEC_Head[0]) + 1; idx < pIR_Obj->len; idx += 2)
        {
            if ((rx_buf[idx].timer >= min0) && (rx_buf[idx].timer <= max0))
            {
            }
            else if ((rx_buf[idx].timer >= min1) && (rx_buf[idx].timer <= max1))
            {
                val[byte] |= _bit;
            }
            else
            {
                log_d("min0:%d, max0:%d, min1:%d, max1:%d, pin:%d, rx_buf[%d]:%d", min0, max0, min1, max1, rx_buf[idx].pin_state, idx, rx_buf[idx].timer);
                break;
            }
            _bit <<= 1;

            if (_bit == 0)
            {
                _bit = 0x01;
                byte++;
            }
        }

        //find repeat
        for (idx = idx; idx + 2 < pIR_Obj->len; idx++)
        {
            if ((rx_buf[idx].timer > IR_NEC_Repeat[0].timer * 0.8)
                    && (rx_buf[idx].timer < IR_NEC_Repeat[0].timer * 1.2)
                    && (rx_buf[idx].pin_state == IR_NEC_Repeat[0].pin_state))
            {
                if ((rx_buf[idx + 1].timer > IR_NEC_Repeat[1].timer * 0.8)
                        && (rx_buf[idx + 1].timer < IR_NEC_Repeat[1].timer * 1.2)
                        && (rx_buf[idx + 1].pin_state == IR_NEC_Repeat[1].pin_state))
                {
                    if ((rx_buf[idx + 2].timer > IR_NEC_Repeat[2].timer * 0.8)
                            && (rx_buf[idx + 2].timer < IR_NEC_Repeat[2].timer * 1.2)
                            && (rx_buf[idx + 2].pin_state == IR_NEC_Repeat[2].pin_state))
                    {
                        log_d("find nec repeat, idx=%d", idx);
                        pIR_Obj->RxRepeat++;
                        break;
                    }
                }
            }
        }


        //if(idx >= pIR_Obj->protocol_size)
        {
//          if( 0xFF == (val[2]|val[3]))
            {
                pIR_Obj->value.address = val[0] << 8 | val[1];
                pIR_Obj->value.command = val[2];
                pIR_Obj->value.command_check = val[3];
//              HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_9);
            }
        }
        //IR_Obj.state = CAPTURE_STAT_IDLE;
        pIR_Obj->len = 0;
    }

    return 0;
}

//
int InfraRed_NEC_Init(IR_TypeDef *pIR_Obj)
{
    pIR_Obj->pHead = IR_NEC_Head;
    pIR_Obj->protocol_size = 66; //4*8*2+2
    pIR_Obj->carry_freq = carry_freq;
    pIR_Obj->state = CAPTURE_STAT_IDLE;
    pIR_Obj->RepeatInterval = 110;
    pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_NEC_Decoder;
    pIR_Obj->pInfraRed_TX_Encoder = InfraRed_TX_NEC_Encoder;
    pIR_Obj->pInfraRed_TX_RepeatEncoder = InfraRed_TX_NEC_RepeatEncoder;
    return 0;
}

#endif /* VAST_USING_IR */
