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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "vast_cli.h"
#include "vast_core/init.h"
#include "vast_bsp.h"
#include "vast_core/device.h"
#include "vast_core/softtimer.h"
#include "vast_core/list.h"
#include "vast_core/kfifo.h"
#include "vast_core/heap.h"
#include "vast_driver/vast_log.h"

/***********************************************
                    define
***********************************************/
#define		USE_KFIFO				1
#define		KFIFO_BUFFER_SIZE		(128U)

/***********************************************
                    typedef
***********************************************/
typedef struct _msg_t
{
	list_t		entry;
	data_t		data;
}msg_t;


/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;

static CLI_HandleTypeDef	hcli;

static LIST_HEAD(uart_tx_list);

#if (USE_KFIFO == 1)
	static DECLARE_KFIFO(cli_rx_fifo, uint8_t, KFIFO_BUFFER_SIZE);
#endif

/***********************************************
                   function
***********************************************/
/**
  * @brief  postToDebugLog.
  * @param
  * @retval
  */
char SendBuf[128] = {0};
static uint8_t tx_buff[128] = {0};

int16_t postToLog(const char *format, ... )
{
	va_list arg;
	int len = 0;
	char inv = 0;

	va_start(arg, format);
	len = vsnprintf(SendBuf, sizeof(SendBuf), format, arg);
	va_end(arg);

	if((list_empty(&uart_tx_list)) & (huart1.gState == HAL_UART_STATE_READY)) {
		memcpy(tx_buff, SendBuf, len);
		HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buff, len);
	} else {
		while(1) {
			msg_t *pMsgPush = NULL;
			pMsgPush = (msg_t *)malloc(sizeof(msg_t)+len);

			if(pMsgPush != NULL) {
				pMsgPush->data.len = len;
				memcpy(pMsgPush->data.buf, SendBuf, pMsgPush->data.len);
				list_add(&(pMsgPush->entry), &uart_tx_list);
				break;
			} else if (inv == 0) {
				inv = 1;
				HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
			} else {
				if((!list_empty(&uart_tx_list)) & (huart1.gState == HAL_UART_STATE_READY) ) {
					msg_t *pMsgPop = NULL;
					pMsgPop = list_last_entry(&uart_tx_list, msg_t, entry);
					memcpy(tx_buff, pMsgPop->data.buf, pMsgPop->data.len);
					HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buff, pMsgPop->data.len);
					list_del(&pMsgPop->entry);
					free(pMsgPop);
				}
			}
		}
	}

	if(inv) {
		HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 5, 0);
	}

	return 0;
}


/**
  * @brief  CliProcess_uart1.
  * @param
  * @retval
  */
static uint8_t CliProcess_uart1 (CLI_HandleTypeDef *pCli, CLI_SEL_FUNCx selFunc)
{
	uint8_t ret = 0;

	switch(selFunc)
	{
		case CLI_FUNC_INIT :
		{
		#if (USE_KFIFO == 1)
			INIT_KFIFO(cli_rx_fifo);
			//kfifo_dma_in_prepare(&cli_rx_fifo, cli_rx_fifo.kfifo.esize - data);
			HAL_UART_Receive_DMA(&huart1, (uint8_t *)cli_rx_fifo.buf, kfifo_size(&cli_rx_fifo));
		#endif

			pCli->Init.Write("HCLK=%ldMHz, Tick=%ldms, V%d.%d.%d:[%s %s]\r\n", HAL_RCC_GetHCLKFreq()/1000000, HAL_GetTick(), VERSION[0], VERSION[1], VERSION[2], buildDate, buildTime);

			break;
		}
		case CLI_FUNC_TX_POP :
		{
			if((!list_empty(&uart_tx_list)) & (huart1.gState == HAL_UART_STATE_READY) )
			{
				msg_t *pMsgPop = NULL;
				pMsgPop = list_last_entry(&uart_tx_list, msg_t, entry);
				memcpy(tx_buff, pMsgPop->data.buf, pMsgPop->data.len);
				HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tx_buff, pMsgPop->data.len);
				list_del(&pMsgPop->entry);
				free(pMsgPop);
			}
			break;
		}
		case CLI_FUNC_RX_PUSH :
		{
			break;
		}
		case CLI_FUNC_RX_POP :
		{
		#if (USE_KFIFO == 1)
			if(!kfifo_is_empty(&cli_rx_fifo))
			{
				kfifo_out(&cli_rx_fifo, &ret, 1);
				//kfifo_get(&cli_rx_fifo, &ret);
			}
		#endif

//			extern void shell_get_chr(char ch);
//			if (ret != 0)
//			{
//				shell_get_chr(ret);
//				ret = 0;
//			}

			break;
		}
		default:
		{
			//printf("%s %d, err cli func!\r\n", __FUNCTION__, __LINE__);
			break;
		}
	}

	return ret;
}

/**
  * @brief  cli_tmr_cb.
  * @param
  * @retval
  */
static void cli_tmr_cb(struct soft_timer * st)
{
	st->expires += pdMS_TO_TICKS(10);
	soft_timer_add(st);

	CLI_Handle(&hcli);
}

static struct soft_timer cli_tmr =
{
	.expires = (INITIAL_JIFFIES) + pdMS_TO_TICKS(1000),
	.cb = cli_tmr_cb,
};

/**
  * @brief  vast_cli_bspInit
  * @param
  * @retval
  */
int vast_cli_bspInit(void)
{
	hcli.Init.Write = postToLog;
	hcli.pCLIProcess = CliProcess_uart1;
	CLI_Initialize(&hcli);

	soft_timer_add(&cli_tmr);

	return 0;
}

core_initcall(vast_cli_bspInit);

/**
  * @brief  vast_cli_bspRev
  * @param
  * @retval
  */
void vast_cli_bspRev(volatile uint8_t data)
{
	static volatile uint32_t old_cnt = KFIFO_BUFFER_SIZE;
#if (USE_KFIFO == 1)
//	kfifo_dma_in_finish(&cli_rx_fifo, cli_rx_fifo.kfifo.esize - data);
	if(old_cnt < data)
	{
		old_cnt += cli_rx_fifo.kfifo.mask + 1;
	}
//	cli_rx_fifo.kfifo.in += (old_cnt - data);
	kfifo_add_in(&cli_rx_fifo, old_cnt - data);
	old_cnt = data;
#endif
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
#if (USE_KFIFO == 1)
//	kfifo_dma_in_finish(&cli_rx_fifo, cli_rx_fifo.kfifo.esize - data);
//	cli_rx_fifo.kfifo.in += cli_rx_fifo.kfifo.mask;
#endif
}

/**
  * @brief  vast_cli_bspRev
  * @param
  * @retval
  */
void vast_cli_handle(void)
{
	CLI_Handle(&hcli);
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)!= RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		vast_cli_bspRev((uint8_t)__HAL_DMA_GET_COUNTER(huart1.hdmarx));
	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



