/**
 ******************************************************************************
 * @file    xxx.c
 * @author  chenzhipeng
 * @version V1.0.0
 * @date    29-Jun-2019
 * @brief
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 */

/** @addtogroup Project
  * @{
  */

/***********************************************
                    include
***********************************************/
#include "device/vast_ir.h"
#include "driver/drv_infrared.h"

/***********************************************
                    define
***********************************************/
#ifdef VAST_USING_EASYLOG
    #define LOG_LVL ELOG_LVL_INFO
    #define LOG_TAG "drv.ir"
    #include "elog.h"
#else
#endif

#define     INFRARED_RX_TIMEOUT     110

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/

/***********************************************
                   function
***********************************************/
/**
  * @brief  infrared_init.
  * @param
  * @retval
  */
static error_t infrared_init(device_t *dev)
{
    infrared_device_t *infrared = (infrared_device_t *)dev;
    infrared_init_t args;

    infrared->ops->init(infrared, &args);

    return 0;
}

/**
  * @brief  infrared_read.
  * @param
  * @retval
  */
static size_t infrared_read(struct device *dev, off_t pos, void *buffer, size_t size)
{
    infrared_device_t *infrared = (infrared_device_t *)dev;

    return 0;
}

/**
  * @brief  infrared_read.
  * @param
  * @retval
  */
static size_t infrared_write(struct device *dev, off_t pos, const void *buffer, size_t size)
{
    size_t ret = 0;
    infrared_device_t *infrared = (infrared_device_t *)dev;

    InfraRed_TX_Encoder(&IR_Obj, buffer, size);

    infrared->ops->tx_start(infrared);

    if (IR_Obj.RepeatCnt > 0)
    {
        log_d("RepeatInterval:%d", IR_Obj.RepeatInterval);
        soft_timer_mod(&infrared->tx_tmr, jiffies + IR_Obj.RepeatInterval);
        IR_Obj.RepeatCnt--;
    }

    return ret;
}

/**
  * @brief  led_ctrl.
  * @param
  * @retval
  */
static error_t infrared_ctrl(device_t *dev, uint8_t cmd, void *args)
{
    infrared_device_t *infrared = (infrared_device_t *)dev;

    switch (cmd)
    {
    case 0xFF:
    {
        break;
    }
    default:
    {
        infrared->ops->ctrl(infrared, cmd, args);
        break;
    }
    }

    return 0;
}

/**
  * @brief  infrared_task_cb.
  * @param
  * @retval
  */
static void infrared_task_cb(struct task_ctrl_blk *tcb, ubase_t data)
{
    infrared_device_t *infrared = (infrared_device_t *)data;

    tSTART(tcb);

    for (;;)
    {
        task_wait_signal(tcb);

        sig_t sig = task_signal(tcb);

        /* check state change or time out event */
        if (sigget(sig, SIG_ALARM))
        {
        }

        /* check recvd data event */
        if (sigget(sig, SIG_DATA))
        {
            if(IR_Obj.state & CAPTURE_STAT_CAPTURE_DONE)
            {
                for (uint16_t i = 0; i < IR_Obj.len; i++)
                {
                    printf("IR_Obj[%d]: %d, %d, len=%d\r\n", i,
                           IR_Obj.rx_buf[i].timer, IR_Obj.rx_buf[i].pin_state, IR_Obj.len);
                    break;
                }

                /* invoke callback */
                if ((infrared->parent.rx_indicate)) 
                {
                    infrared->parent.rx_indicate(&(infrared->parent), IR_Obj.len);
                }

                InfraRed_RX_Decoder();

                printf("ir receive: 0x%04X, 0x%02X, 0x%02X\r\n",
                       IR_Obj.value.address, IR_Obj.value.command, IR_Obj.value.command_check);

                //IR_Obj.len = 0;
            }
            else
            {
                IR_Obj.len = 0;
                printf("infrared receive fail, len:%d\r\n", IR_Obj.len);
            }
        }

    }
    tEND();
}

/**
  * @brief  infrared_isr_handler.
  * @param
  * @retval
  */
error_t infrared_rx_isr_handler(infrared_device_t *infrared, uint8_t level, uint16_t us)
{
    ir_rx_irq_callback(us, level);
    soft_timer_mod(&infrared->rx_tmr, jiffies + INFRARED_RX_TIMEOUT);

    return 0;
}

/**
  * @brief  infrared_isr_handler.
  * @param
  * @retval
  */
error_t infrared_tx_isr_handler(infrared_device_t *infrared)
{
    return 0;
}

/**
  * @brief  infrared_rx_tmr_cb.
  * @param
  * @retval
  */
static void infrared_rx_tmr_cb(struct soft_timer *st)
{
    infrared_device_t *infrared = (infrared_device_t *)st->data;

    if (infrared->parent.owner)
    {
        task_send_signal(infrared->parent.owner, SIG_DATA);
    }
}

/**
  * @brief  infrared_rx_tmr_cb.
  * @param
  * @retval
  */
static void infrared_tx_tmr_cb(struct soft_timer *st)
{
    infrared_device_t *infrared = (infrared_device_t *)st->data;

    log_d("tx_tmr_repeat:%d", IR_Obj.RepeatCnt);

    if (IR_Obj.RepeatCnt > 0)
    {
        soft_timer_mod(st, jiffies + IR_Obj.RepeatInterval);
        IR_Obj.RepeatCnt--;
    }

    InfraRed_TX_RepeatEncoder(&IR_Obj, NULL, 0);
    infrared->ops->tx_start(infrared);
}

/**
  * @brief  infrared_ops.
  * @param
  * @retval
  */
static struct device_ops infrared_ops =
{
    .init = infrared_init,
    .write = infrared_write,
    .read = infrared_read,
    .ctrl = infrared_ctrl,
};

/**
  * @brief  infrared_device_register
  * @param
  * @retval
  */
error_t infrared_device_register(infrared_device_t *infrared,
                                 const char *name, uint32_t flag, void *data)
{
    device_t *dev = &(infrared->parent);

    dev->ops        = &infrared_ops;
    dev->user_data  = data;
    dev->owner      = &infrared->tcb;

    struct soft_timer *st = &infrared->rx_tmr;
    st->cb      = infrared_rx_tmr_cb;
    st->data    = (ubase_t)infrared;
    st->expires = INITIAL_JIFFIES;

    st = &infrared->tx_tmr;
    st->cb      = infrared_tx_tmr_cb;
    st->data    = (ubase_t)infrared;
    st->expires = INITIAL_JIFFIES;

    task_create(&infrared->tcb, infrared_task_cb, "name", (ubase_t)infrared);

    return device_register(dev, name, flag);
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



