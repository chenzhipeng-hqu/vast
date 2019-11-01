
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
#include "vast_bsp.h"
#include "vast_core/device.h"
#include "vast_core/init.h"
#include "vast_core/kfifo.h"
#include "vast_driver/vast_serial.h"
#include "vast_third/croutine/croutine.h"
#include "vast_third/croutine/port.h"
#include "gpio.h"
#include "usart.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/
typedef struct _uart_list_t
{
    USART_TypeDef   *uartx;
    IRQn_Type       irq;
    gpio_list_t     rx_io;
    gpio_list_t     tx_io;
    dma_list_t      rx_dma;
    dma_list_t      tx_dma;
}uart_list_t;

typedef struct _msg_t
{
	list_t 		entry;
	data_t		data;
}msg_t;

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
static struct serial_device uart2_dev;

static const uart_list_t uart2 = {
    .uartx = USART2,
    .irq = USART2_IRQn,
    .rx_io = {
            .GPIOx = GPIOA,
            .PinMask = LL_GPIO_PIN_3
        },
    .tx_io = {
            .GPIOx = GPIOA,
            .PinMask = LL_GPIO_PIN_2
        },
    .rx_dma = {
            .DMAx = DMA1,
            .chx = LL_DMA_STREAM_5,
            .irq = DMA1_Stream5_IRQn
        },
    .tx_dma = {
            .DMAx = DMA1,
            .chx = LL_DMA_STREAM_6,
            .irq = DMA1_Stream6_IRQn
        },
};

static uint8_t tx_dma_buff[256] = {0};

/***********************************************
                   function
***********************************************/
/**
  * @brief  serial_gpio_init.
  * @param
  * @retval
  */
static void uart_gpio_init(void)
{
    //LL_GPIO_SetOutputPin(uart2_gpio.GPIOx, uart2_gpio.PinMask);
    return;
}

/**
  * @brief  led_set_bsp.
  * @param
  * @retval
  */
static error_t uart_configure(struct serial_device *serial, struct serial_configure *cfg)
{
    //struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;

    if(serial->parent.open_flag & DEVICE_FLAG_DMA_TX) {
		LL_DMA_SetPeriphAddress(uart->tx_dma.DMAx , uart->tx_dma.chx, LL_USART_DMA_GetRegAddr(uart->uartx));

		/* (5) Enable DMA transfer complete/error interrupts  */
		LL_DMA_EnableIT_TC(uart->tx_dma.DMAx, uart->tx_dma.chx);
		LL_DMA_EnableIT_TE(uart->tx_dma.DMAx, uart->tx_dma.chx);

		/* Enable DMA TX Interrupt */
		LL_USART_EnableDMAReq_TX(uart->uartx);
    }

    if (serial->parent.open_flag & DEVICE_FLAG_DMA_RX) {
		LL_USART_EnableIT_IDLE(uart->uartx);

		/* Enable DMA RX Interrupt */
		LL_USART_EnableDMAReq_RX(uart->uartx);

		LL_DMA_ConfigAddresses(uart->rx_dma.DMAx, uart->rx_dma.chx,
									LL_USART_DMA_GetRegAddr(uart->uartx),
									(uint32_t)serial->rx_kfifo.buf,
									LL_DMA_GetDataTransferDirection(uart->rx_dma.DMAx, uart->rx_dma.chx));

		LL_DMA_SetDataLength(uart->rx_dma.DMAx, uart->rx_dma.chx, KFIFO_BUFFER_SIZE);

		LL_DMA_EnableStream(uart->rx_dma.DMAx, uart->rx_dma.chx);
    }

    return 0;
}

/**
  * @brief  led_set_bsp.
  * @param
  * @retval
  */
static error_t uart_control(struct serial_device *serial, int cmd, void *arg)
{
    //struct _uart *uart = (struct _uart *)serial->parent.user_data;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;

    switch (cmd)
    {
    case DEVICE_CTRL_CLR_INT:
        //UART_ITConfig(uart->uart_device, UART_IT_RB, DISABLE);
    	LL_USART_DisableIT_RXNE(uart->uartx);
        break;
    case DEVICE_CTRL_SET_INT:
        //UART_ITConfig(uart->uart_device, UART_IT_RB, ENABLE);
    	LL_USART_EnableIT_RXNE(uart->uartx);
        break;
    case DEVICE_CTRL_CLR_TX_INT:
        //UART_ITConfig(uart->uart_device, UART_IT_TB, DISABLE);
    	LL_USART_DisableIT_TXE(uart->uartx);
        break;
    case DEVICE_CTRL_SET_TX_INT:
        //UART_ITConfig(uart->uart_device, UART_IT_TB, ENABLE);
    	LL_USART_EnableIT_TXE(uart->uartx);
        break;
    case DEVICE_CTRL_INT_PRIO: {
			int priority = 0;
			priority = *(int *)arg;
			NVIC_SetPriority(DMA2_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, 0));
			break;
		}
    }

    return 0;
}

static int uart_putc(struct serial_device *serial, char c)
{
    //struct _uart *uart = (struct _uart *)serial->parent.user_data;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;

    //while (!(uart->uart_device->IF.Word & UART_FLAG_TB));
//    uart->uart_device->TBW.Byte[0] = c;
    while(!LL_USART_IsActiveFlag_TXE(uart->uartx));
    LL_USART_TransmitData8(uart->uartx, c);

    return 1;
}

static int uart_getc(struct serial_device *serial)
{
    int ch = -1;
    //struct es8p508_uart *uart = (struct es8p508_uart *)serial->parent.user_data;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;

    // if (uart->uart_device->IF.Word & UART_FLAG_RB)
    if (LL_USART_IsActiveFlag_RXNE(uart->uartx))
    {
        LL_USART_ClearFlag_RXNE(uart->uartx);
    	ch = LL_USART_ReceiveData8(uart->uartx);
        //ch = uart->uart_device->RBR.Byte[0] & 0xff;
    }

    return ch;
}

static int uart_write(struct serial_device *serial, off_t pos, const void *buffer, size_t size)
{
    //struct es8p508_uart *uart = (struct es8p508_uart *)serial->parent.user_data;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;
    int i = size;

#if 0
	for (i=pos; i<size; i++)
	{
	   while(!LL_USART_IsActiveFlag_TXE(uart->uartx));
	   LL_USART_TransmitData8(uart->uartx, *(uint8_t *)(buffer+i));
	}
//   while(!LL_USART_IsActiveFlag_TXE(uart->uartx));
//   LL_DMA_SetMemoryAddress(uart->tx_dma.DMAx, uart->tx_dma.chx, (uint32_t)buffer);
//   LL_DMA_SetDataLength(uart->tx_dma.DMAx, uart->tx_dma.chx, size);
//   LL_DMA_EnableStream(uart->tx_dma.DMAx, uart->tx_dma.chx);
//   while(!LL_USART_IsActiveFlag_TXE(uart->uartx));
#else
	char inv = 0;
//	static uint8_t buff_tmp[256] = {0};
    OS_CPU_SR cpu_sr;

	if((list_empty(&serial->tx_list)) & (LL_USART_IsActiveFlag_TXE(uart->uartx))) {
		memcpy(tx_dma_buff, buffer, size);
		size = min(255, size);
		tx_dma_buff[size] = 0;
		LL_DMA_SetMemoryAddress(uart->tx_dma.DMAx, uart->tx_dma.chx, (uint32_t)tx_dma_buff);
		LL_DMA_SetDataLength(uart->tx_dma.DMAx, uart->tx_dma.chx, size);
		LL_DMA_EnableStream(uart->tx_dma.DMAx, uart->tx_dma.chx);
	} else {
		enter_critical();
		while(1) {
			msg_t *pMsgPush = NULL;
			pMsgPush = (msg_t *)malloc(sizeof(msg_t)+size);

			if((pMsgPush != NULL)) {
				pMsgPush->data.len = size;
				memcpy(pMsgPush->data.buf, buffer, pMsgPush->data.len);
				list_add(&(pMsgPush->entry), &serial->tx_list);

//				if(LL_USART_IsActiveFlag_TXE(uart->uartx)){
//		        	msg_t *pMsgPop = NULL;
//		            pMsgPop = list_last_entry(&uart2_dev.tx_list, msg_t, entry);
//		            memcpy(tx_dma_buff, pMsgPop->data.buf, pMsgPush->data.len);
//		            LL_DMA_SetMemoryAddress(uart2.tx_dma.DMAx, uart2.tx_dma.chx, (uint32_t)tx_dma_buff);
//		            LL_DMA_SetDataLength(uart2.tx_dma.DMAx, uart2.tx_dma.chx, pMsgPush->data.len);
//		            LL_DMA_EnableStream(uart2.tx_dma.DMAx, uart2.tx_dma.chx);
//		            list_del(&pMsgPop->entry);
//		            free(pMsgPop);
//				}
				break;
			} else if (inv == 0) {
				inv = 1;
				NVIC_SetPriority(DMA2_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
			}
		}
		exit_critical();
	}
	if(inv) {
		NVIC_SetPriority(DMA2_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),7, 0));
	}
#endif
	fflush(stdout); //刷新标准输出缓冲区，把输出缓冲区里的东西打印到标准输出设备上 。
    return i;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static const struct uart_ops uart_bsp_ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
    uart_write,
};

static void uart_interrupt_handler(struct serial_device *serial)
{
    //USART_TypeDef *uart_dev = ((struct _uart *)serial->parent.user_data)->uartx;
    struct _uart_list_t *uart = (struct _uart_list_t *)serial->parent.user_data;

    if (LL_USART_IsActiveFlag_TXE(uart->uartx))
    {
        serial_device_isr(serial, SERIAL_EVENT_TX_RDY);
    }

    if (LL_USART_IsActiveFlag_RXNE(uart->uartx))
    {
        serial_device_isr(serial, SERIAL_EVENT_RX_IND);
    }
}

/**
  * @brief  led_set_bsp.
  * @param
  * @retval
  */
void USART2_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_IDLE(uart2.uartx))
	{
		LL_USART_ClearFlag_IDLE(uart2.uartx);
        int dma_cnt = LL_DMA_GetDataLength(uart2.rx_dma.DMAx, uart2.rx_dma.chx);
	    static volatile uint32_t old_cnt = KFIFO_BUFFER_SIZE;

        if(old_cnt < dma_cnt)
        {
            old_cnt += uart2_dev.rx_kfifo.kfifo.mask+1;
        }
        kfifo_add_in(&uart2_dev.rx_kfifo, (old_cnt-dma_cnt));
        old_cnt = dma_cnt;

        if (uart2_dev.parent.owner)
        {
            task_send_signal(uart2_dev.parent.owner, SIG_DATA);
        }
	}

	uart_interrupt_handler(&uart2_dev);
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel6_IRQn 0 */


  /* USER CODE END DMA1_Channel6_IRQn 0 */
  
  /* USER CODE BEGIN DMA1_Channel6_IRQn 1 */

  /* USER CODE END DMA1_Channel6_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */

  /* Check whether DMA transfer complete caused the DMA interruption */

    if(LL_DMA_IsActiveFlag_TC7(uart2.tx_dma.DMAx))
    {
        LL_DMA_DisableStream(uart2.tx_dma.DMAx, uart2.tx_dma.chx);
        //LL_DMA_ClearFlag_GI7(uart2.tx_dma.DMAx);
        LL_DMA_ClearFlag_TC7(uart2.tx_dma.DMAx);

        if((!list_empty(&uart2_dev.tx_list))/* & (LL_USART_IsActiveFlag_TXE(uart2.uartx))*/)
        {
            OS_CPU_SR cpu_sr;
        	msg_t *pMsgPop = NULL;
            pMsgPop = list_last_entry(&uart2_dev.tx_list, msg_t, entry);
            memcpy(tx_dma_buff, pMsgPop->data.buf, pMsgPop->data.len);
            LL_DMA_SetMemoryAddress(uart2.tx_dma.DMAx, uart2.tx_dma.chx, (uint32_t)tx_dma_buff);
            LL_DMA_SetDataLength(uart2.tx_dma.DMAx, uart2.tx_dma.chx, pMsgPop->data.len);
            LL_DMA_EnableStream(uart2.tx_dma.DMAx, uart2.tx_dma.chx);
			enter_critical();
            list_del(&pMsgPop->entry);
			exit_critical();
            free(pMsgPop);
        }

        /* Call function Transmission complete Callback */
    }
    else if(LL_DMA_IsActiveFlag_TE7(uart2.tx_dma.DMAx))
    {
        LL_DMA_ClearFlag_TE7(uart2.tx_dma.DMAx);
        /* Call Error function */
        //USART_TransferError_Callback();
    }

  /* USER CODE END DMA1_Channel7_IRQn 0 */
  
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */

  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

//#ifdef __GNUC__
//int _write(int file, char *ptr, int len)
//{
//#if 1
//	int DataIdx;
//    OS_CPU_SR cpu_sr;
//
//	enter_critical();
//	for (DataIdx = 0; DataIdx < len;DataIdx++) {
//       while(!LL_USART_IsActiveFlag_TXE(uart2.uartx));
//       LL_USART_TransmitData8(uart2.uartx, *(ptr+DataIdx));
//	}
//	exit_critical();
////	if(uart2_dev.parent.ops->write)
////		uart2_dev.parent.ops->write(&uart2_dev.parent, 0, ptr, len);
//#else
//	char inv = 0;
//
//    len = len < 128 ? len : 128;
//
//	if((list_empty(&uart2_dev.tx_list)) & (LL_USART_IsActiveFlag_TXE(uart2.uartx)))
//	{
//        memcpy(buff_tmp, ptr, len);
//        LL_DMA_SetMemoryAddress(uart2.tx_dma.DMAx, uart2.tx_dma.chx, (uint32_t)buff_tmp);
//        LL_DMA_SetDataLength(uart2.tx_dma.DMAx, uart2.tx_dma.chx, len);
//        LL_DMA_EnableStream(uart2.tx_dma.DMAx, uart2.tx_dma.chx);
//	}
//	else
//	{
//		while(1)
//		{
//			msg_t *pMsgPush = NULL;
//			pMsgPush = (msg_t *)malloc(sizeof(msg_t)+len);
//			if((pMsgPush != NULL))
//			{
//				pMsgPush->len = len;
//				memcpy(pMsgPush->data, ptr, pMsgPush->len);
//				list_add(&(pMsgPush->entry), &uart2_dev.tx_list);
//				break;
//			}
//			else if (inv == 0)
//			{
//				inv = 1;
//                NVIC_SetPriority(DMA1_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
//			}
//		}
//	}
//
//	if(inv)
//	{
//        NVIC_SetPriority(DMA1_Stream7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),7, 0));
//	}
//#endif
//
//	return len;
//
//}
//#endif

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static int vast_uart_bsp_init(void)
{
	error_t ret = 0;
    struct _uart_list_t const *uart;
    struct serial_configure config = SERIAL_CONFIG_DEFAULT;

    uart_gpio_init();

    uart = &uart2;
    config.bufsz   = 0x100;
	uart2_dev.ops = &uart_bsp_ops;
    uart2_dev.config = config;

    ret = serial_device_register(&uart2_dev, "uart2", 0, (void *)uart);

//	device_open(&uart2_dev.parent, DEVICE_FLAG_INT_RX | DEVICE_FLAG_INT_TX);
//	printf("led_device_t size=%d\r\n", sizeof(led_device_t));

	return ret;
}

subsys_initcall(vast_uart_bsp_init);
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



