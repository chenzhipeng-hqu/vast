#include <core/device.h>
#include <core/types.h>
#include <core/utils.h>
//#include "core/croutine.h"
//#include <printk.h>

#if (defined configUSING_SERIAL || defined VAST_USING_SERIAL)

#ifdef configUSING_FRAME_TIMEOUT_SOFT
static void serial_tmr_cb(struct soft_timer *st)
{
    struct serial_device *serial = (struct serial_device *)st->data;

    if (serial->parent.owner)
    {
        task_send_signal(serial->parent.owner, SIG_DATA);
    }
}
#endif
static error_t serial_init(struct device *dev)
{
    error_t result = 0;
    struct serial_device *serial = (struct serial_device *)dev;

    /* apply configuration */
    if (serial->ops->cfg) result = serial->ops->cfg(serial, &serial->config);

#ifdef configUSING_FRAME_TIMEOUT_SOFT
    {
        struct soft_timer *st = &serial->rxto;

        st->cb      = serial_tmr_cb;
        st->data    = (ubase_t)serial;
        st->expires = INITIAL_JIFFIES;
    }
#endif

    return result;
}

static error_t serial_open(struct device *dev, uint16_t oflag)
{
    struct serial_device *serial = (struct serial_device *)dev;

    /* get open flags */
    dev->open_flag = oflag & 0xff;

#ifdef configUSING_SERIAL_INT
    if (oflag & DEVICE_FLAG_INT_RX)
    {
        chn_init(&serial->rx, serial->config.bufsz);
        dev->open_flag |= DEVICE_FLAG_INT_RX;
        serial->ops->control(serial, DEVICE_CTRL_SET_INT, NULL);
    }

    if (oflag & DEVICE_FLAG_INT_TX)
    {
        chn_init(&serial->tx, serial->config.bufsz);
        dev->open_flag |= DEVICE_FLAG_INT_TX;
    }
#endif

#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    if (oflag & DEVICE_FLAG_DMA_RX)
    {
        INIT_KFIFO(serial->rx_kfifo);
        serial->rx_dma_old_cnt = serial->rx_kfifo.kfifo.mask+1;
        dev->open_flag |= DEVICE_FLAG_DMA_RX;
    }

    if (oflag & DEVICE_FLAG_DMA_TX)
    {
        INIT_KFIFO(serial->tx_kfifo);
        dev->open_flag |= DEVICE_FLAG_DMA_TX;
    }
#endif

    return 0;
}

static error_t serial_close(struct device *dev)
{
    struct serial_device *serial = (struct serial_device *)dev;

#ifdef configUSING_SERIAL_INT
    if (dev->open_flag & DEVICE_FLAG_INT_RX)
    {
        dev->open_flag &= ~DEVICE_FLAG_INT_RX;
        serial->ops->control(serial, DEVICE_CTRL_CLR_INT, (void *)0);
    }

    if (dev->open_flag & DEVICE_FLAG_INT_TX)
    {
        dev->open_flag &= ~DEVICE_FLAG_INT_TX;
        serial->ops->control(serial, DEVICE_CTRL_CLR_INT, (void *)0);
    }
#endif

#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    if (dev->open_flag & DEVICE_FLAG_DMA_RX)
    {
        dev->open_flag &= ~DEVICE_FLAG_DMA_RX;
        serial->ops->control(serial, DEVICE_CTRL_CLR_INT, (void *)0);
    }

    if (dev->open_flag & DEVICE_FLAG_DMA_TX)
    {
        dev->open_flag &= ~DEVICE_FLAG_DMA_TX;
        serial->ops->control(serial, DEVICE_CTRL_CLR_INT, (void *)0);
    }
#endif

    return 0;
}

static size_t serial_read(struct device *dev, off_t pos, void *buffer, size_t size)
{
    struct serial_device *serial = (struct serial_device *)dev;

#ifdef configUSING_SERIAL_INT
    if (dev->open_flag & DEVICE_FLAG_INT_RX)
    {
        return chn_get(&serial->rx, buffer, size);
        //        return serial->ops->get_c(serial);
    }
#endif

#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    if (dev->open_flag & DEVICE_FLAG_DMA_RX)
    {
        //if(!kfifo_is_empty(&serial->rx_kfifo))
        {
            return kfifo_out(&serial->rx_kfifo, buffer, size);
        }
    }
#endif

    return 0;
}

static size_t serial_peek(struct device *dev, off_t pos, void *buffer, size_t size)
{
    struct serial_device *serial = (struct serial_device *)dev;

#ifdef configUSING_SERIAL_INT
    if (dev->open_flag & DEVICE_FLAG_INT_RX)
    {
        return chn_peek(&serial->rx, buffer, size);
    }
#endif

#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    if (dev->open_flag & DEVICE_FLAG_DMA_RX)
    {
        return kfifo_out_peek(&serial->rx_kfifo, buffer, size);
    }
#endif

    return 0;
}

static size_t serial_write(struct device *dev, off_t pos, const void *buffer, size_t size)
{
    size_t ret = 0;
    struct serial_device *serial = (struct serial_device *)dev;

    //log_d("send data to %s\r\n", dev->parent.name);
    //log_arr_d(buffer, size);

#ifdef configUSING_SERIAL_INT
    if (dev->open_flag & DEVICE_FLAG_INT_TX)
    {
    	//while(1) {
    		ret = 0;
			ret = chn_put(&serial->tx, buffer, size);
			size = size - ret;

			if(size > 0) {
				ret = 1;
			} else {
			//	break;
			}
    	//}

		if (ret > 0) {
			serial->ops->control(serial, DEVICE_CTRL_SET_TX_INT, NULL);
		}
    }
#endif

#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    if (dev->open_flag & DEVICE_FLAG_DMA_TX)
    {
        int inv = 0, priority = 0;
        //uint8_t *buff = (uint8_t *)buffer;

    	do {
            //ret += kfifo_in(&serial->tx_kfifo, &buff[ret], size-ret);
            ret += kfifo_in(&serial->tx_kfifo, (uint8_t *)(buffer + ret), size-ret);
            //ret += kfifo_in(&serial->tx_kfifo, buffer + ret, size-ret);
            if (ret > 0) {
                serial->ops->control(serial, DEVICE_CTRL_SET_TX_INT, NULL);
            }

            if ((ret != size) & (inv == 0)) {
                inv = 1;
                priority = 0;
                serial->ops->control(serial, DEVICE_CTRL_INT_PRIO, &priority);
            }

    	}while(ret != size);

        if (inv == 1) {
            priority = 15;
            serial->ops->control(serial, DEVICE_CTRL_INT_PRIO, &priority);
        }
    }
#endif

    return ret;
}

static error_t serial_control(struct device *dev, uint8_t cmd, void *args)
{
    error_t err = 0;
    struct serial_device *serial = (struct serial_device *)dev;

    switch (cmd)
    {
    case SERIAL_CTRL_GETCFG:
        *(struct serial_configure *)args = serial->config;
        break;
    case SERIAL_CTRL_SETCFG:
    {
        struct serial_configure *pconfig = (struct serial_configure *)args;

        err = serial->ops->cfg(serial, pconfig);
        if (err == 0) serial->config = *pconfig;
    }
    break;
    case DEVICE_CTRL_ADD_OUT:
    {
        unsigned int len = *(size_t *)(args);
        len = min(len, kfifo_len(&serial->rx_kfifo));
        kfifo_add_out(&serial->rx_kfifo, len);
    }
    break;

    default:
        err = serial->ops->control(serial, cmd, args);
        break;
    }

    return err;
}

static const struct device_ops serial_ops =
{
    .init  = serial_init,
    .open  = serial_open,
    .close = serial_close,
    .read  = serial_read,
    .peek  = serial_peek,
    .write = serial_write,
    .ctrl  = serial_control,
};

error_t serial_device_register(struct serial_device *serial, const char *name, uint32_t flag, void *data)
{
    struct device *dev = &(serial->parent);

    dev->ops       = &serial_ops;
    dev->user_data = data;

    device_register(dev, name, flag);
    return 0;
}

void serial_device_isr(struct serial_device *dev, int event)
{
    u8 c;

    (void)c;

    switch (event & 0xff)
    {
#ifdef configUSING_SERIAL_INT
    case SERIAL_EVENT_TX_RDY:
        if (chn_get(&dev->tx, &c, 1))
        {
            dev->ops->put_c(dev, c);
        }
        else
        {
            dev->ops->control(dev, DEVICE_CTRL_CLR_TX_INT, NULL);
        }
        break;
#endif
#if (defined configUSING_SERIAL_DMA || defined VAST_USING_SERIAL_DMA)
    case SERIAL_EVENT_TX_DMADONE: {
            unsigned int len = kfifo_out(&dev->tx_kfifo, (uint8_t *)&(dev->tx_dma_buff), sizeof(&dev->tx_dma_buff));
            if (len > 0)
            {
                dev->ops->control(dev, DEVICE_CTRL_CLR_TX_DMA, NULL);
                dev->ops->control(dev, DEVICE_CTRL_SET_TX_DMA_LEN, &len);
                dev->ops->control(dev, DEVICE_CTRL_SET_TX_DMA, NULL);
            }
            else
            {
                dev->ops->control(dev, DEVICE_CTRL_CLR_TX_INT, NULL);
//                if (dev->parent.owner) {
//					task_send_signal(dev->parent.owner, SIG_ALARM);
//				}

                /* invoke callback */
                if (dev->parent.tx_complete != NULL) {
                	dev->parent.tx_complete(&dev->parent, (void*)dev->tx_dma_buff);
                }
            }
        }
        break;
    case SERIAL_EVENT_RX_IDLE: {
            //int dma_cnt = LL_DMA_GetDataLength(uart->rx_dma.DMAx, uart->rx_dma.chx);
            int len = 0;

            dev->ops->control(dev, DEVICE_CTRL_GET_RX_DMA_LEN, &len);

            if(dev->rx_dma_old_cnt < len) {
                dev->rx_dma_old_cnt += dev->rx_kfifo.kfifo.mask+1;
            }

            kfifo_add_in(&dev->rx_kfifo, (dev->rx_dma_old_cnt-len));

//            len = sizeof(dev->rx_dma_buff) - len;
//            dev->ops->control(dev, DEVICE_CTRL_CLR_RX_DMA, NULL);
//            len = kfifo_in(&dev->rx_kfifo, (dev->rx_dma_buff), len);
//            len = sizeof(dev->rx_dma_buff);
//            dev->ops->control(dev, DEVICE_CTRL_SET_RX_DMA_LEN, &len);
//            dev->ops->control(dev, DEVICE_CTRL_SET_RX_DMA, NULL);

//            if ((dev->parent.owner) && (len > 0)) {
//                task_send_signal(dev->parent.owner, SIG_DATA);
//            }

            /* invoke callback */
            if ((dev->parent.rx_indicate) && (len > 0)) {
            	dev->parent.rx_indicate(&(dev->parent), (dev->rx_dma_old_cnt-len));
            }

            dev->rx_dma_old_cnt = len;
        }
    	break;
    case SERIAL_EVENT_RX_DMADONE: {
//            int len2 = 0;
//
//            len2 = kfifo_in(&dev->rx_kfifo, (dev->rx_dma_buff), sizeof(dev->rx_dma_buff));
//
//            if ((dev->parent.owner) && (len2 > 0)) {
//                task_send_signal(dev->parent.owner, SIG_DATA);
//            }
        }
        break;
#endif

    case SERIAL_EVENT_RX_IND:
    {
        #ifdef configUSING_SERIAL_INT
            int ch = -1;

            while (1)
            {
                ch = dev->ops->get_c(dev);
                if (ch == -1) break;

                c = ch;
                chn_put(&dev->rx, &c, 1);
            }
        #endif

        #ifdef configUSING_FRAME_TIMEOUT_SOFT
        {
            struct soft_timer *st = &dev->rxto;
            soft_timer_del(st);
            st->expires = jiffies + configSERIAL_RX_TO;
            soft_timer_add(st);
        }
        #endif
        break;
    }
    }
}
#endif

