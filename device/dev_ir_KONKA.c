
#include <device/dev_ir.h>

#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_DEBUG
    #define LOG_TAG "ir.konka"
    #include "elog.h"
#else
#endif

#ifdef VAST_USING_IR
static const uint16_t carry_freq = 38000;
//
const IR_BufTypeDef IR_KONKA_Head[] =
{
    {3000, IR_PIN_HIGH},
    {3000, IR_PIN_LOW},
};

const IR_BufTypeDef IR_KONKA_One[] =
{
    {500,    IR_PIN_HIGH},
    {2500, IR_PIN_LOW},
};

const IR_BufTypeDef IR_KONKA_Zero[] =
{
    {500, IR_PIN_HIGH},
    {1500, IR_PIN_LOW},
};

const IR_BufTypeDef IR_KONKA_Sync[] =
{
    {500, IR_PIN_HIGH},
    {4000, IR_PIN_LOW},
    {500, IR_PIN_HIGH},
};


//----------------------------------- KONKA
//----------------------- head[0:7] - code[0:7]--
//___   3ms   __3ms__   _ 8b head _ _ 8b code _ _ 1.5b sync _ _
//   |_______|       |_|XXXXXXXXXXX|XXXXXXXXXXX|XXX|
// carry 38000Hz, 19=500us
const IR_BufTypeDef ir_head_konka[] =
{
    {114, 1},// 3ms
    {114, 0},// 3ms
    {19, 1},// 500us
};
const IR_BufTypeDef ir_1_konka[] =
{
    {95, 0}, // 2.5ms
    {19, 1},
};
const IR_BufTypeDef ir_0_konka[] =
{
    {57, 0}, // 1.5ms
    {19, 1},
};
const IR_BufTypeDef ir_tail_konka[] =
{
    {152, 0}, // 4ms
    {19, 1},
};

/**
  * @brief  Write KONKA data to IR.
  * @param  data.
  * @retval Success or fail.
  */
int InfraRed_TX_KONKA_Encoder(IR_TypeDef *pIR_Obj, const void *buffer, size_t size)
{
    uint8_t ret;
    uint32_t idx;
    //uint8_t data[4];
    uint8_t *u8Buff = (uint8_t *)buffer;
    uint8_t bit;
    if (pIR_Obj->idle)
    {
        pIR_Obj->tx_bufLen = 0;
        ir_tx_push_data(ir_head_konka, sizeof(ir_head_konka) / sizeof(ir_head_konka[0]));
        //data[0] = (head);
        //data[1] = (code);
        for (idx = 0; idx < size; idx++)
        {
            for (bit = 0x80; bit != 0; bit >>= 1)
            {
                if (u8Buff[idx] & bit)
                {
                    ir_tx_push_data(ir_1_konka, sizeof(ir_1_konka) / sizeof(ir_1_konka[0]));
                }
                else
                {
                    ir_tx_push_data(ir_0_konka, sizeof(ir_0_konka) / sizeof(ir_0_konka[0]));
                }
            }
        }
        ir_tx_push_data(ir_tail_konka, sizeof(ir_tail_konka) / sizeof(ir_tail_konka[0]));
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
}

//
int InfraRed_RX_KONKA_Decoder(IR_TypeDef *pIR_Obj)
{
    uint8_t idx = 0, byte = 0, _bit = 0x80;
    uint16_t min0, max0, min1, max1;
    uint8_t val[4] = {0};
    volatile IR_BufTypeDef *rx_buf = pIR_Obj->rx_buf;

    min0 = IR_KONKA_Zero[1].us * 0.8;
    max0 = IR_KONKA_Zero[1].us * 1.2;
    min1 = IR_KONKA_One[1].us * 0.8;
    max1 = IR_KONKA_One[1].us * 1.2;

    if (pIR_Obj->rx_len != 0)
    {
        // find head
        for (idx = 0; idx < 10; idx++)
        {
            if ((rx_buf[idx].us > IR_KONKA_Head[0].us * 0.8)
                    && (rx_buf[idx].us < IR_KONKA_Head[0].us * 1.2)
                    && (rx_buf[idx].level == IR_KONKA_Head[0].level))
            {
                if ((rx_buf[idx + 1].us > IR_KONKA_Head[1].us * 0.8)
                        && (rx_buf[idx + 1].us < IR_KONKA_Head[1].us * 1.2)
                        && (rx_buf[idx + 1].level == IR_KONKA_Head[1].level))
                {
                    log_d("find konka head, idx=%d", idx);
                    //rx_buf += idx;
                    break;
                }
            }
        }

        // find code
        for (idx = idx + sizeof(IR_KONKA_Head) / sizeof(IR_KONKA_Head[0]) + 1; idx < pIR_Obj->rx_len - 3; idx += 2)
        {
            if ((pIR_Obj->rx_buf[idx].us >= min0) && (pIR_Obj->rx_buf[idx].us <= max0))
            {

            }
            else if ((pIR_Obj->rx_buf[idx].us >= min1) && (pIR_Obj->rx_buf[idx].us <= max1))
            {
                val[byte] |= _bit;
            }
            else
            {
                printf("min0:%d, max0:%d, min1:%d, max1:%d, pin:%d, rx_buf[%d]:%d\r\n", min0, max0, min1, max1, pIR_Obj->rx_buf[idx].level, idx, pIR_Obj->rx_buf[idx].us);
                break;
            }
            _bit >>= 1;

            if (_bit == 0)
            {
                _bit = 0x80;
                byte++;
            }
        }

        if (idx + 3 >= pIR_Obj->protocol_size)
        {
            //if( 0xFF == (val[2]|val[3]))
            {
                pIR_Obj->value.address = val[0];
                pIR_Obj->value.command = val[1];
            }
        }
        //IR_Obj.state = CAPTURE_STAT_IDLE;
        pIR_Obj->rx_len = 0;
    }

    return 0;
}

//
int InfraRed_KONKA_Init(IR_TypeDef *pIR_Obj)
{
    pIR_Obj->pHead = IR_KONKA_Head;
    pIR_Obj->protocol_size = 36; //2*8*2+2+3
    //pIR_Obj->state = CAPTURE_STAT_IDLE;
	pIR_Obj->carry_freq = carry_freq;
	pIR_Obj->RepeatInterval = 0;	
    pIR_Obj->pInfraRed_RX_Decoder = InfraRed_RX_KONKA_Decoder;
    pIR_Obj->pInfraRed_TX_Encoder = InfraRed_TX_KONKA_Encoder;
    return 0;
}
#endif /* VAST_USING_IR */

