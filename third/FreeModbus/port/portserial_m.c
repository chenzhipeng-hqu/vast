/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#ifdef configUSING_FREEMODBUS

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
static uint8_t g_recv_byte;
/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/


/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
	//UART_HandleTypeDef *rs485_uart;
    //LL_USART_InitTypeDef USART_InitStruct = {0};
	
	////printf("===rs485 baud = %d, databits = %d, parity = %d\r\n", ulBaudRate, ucDataBits, eParity);
	
	////rs485_uart = &RS485_UART;	
    //USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    //USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    //USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    //USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	
	////rs485_uart->Init.BaudRate = ulBaudRate;
    //USART_InitStruct.BaudRate = ulBaudRate;
	
	//if(eParity == MB_PAR_NONE) {
		////rs485_uart->Init.Parity = UART_PARITY_NONE;
        //USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	//} else if(eParity == MB_PAR_ODD) {
		////rs485_uart->Init.Parity = UART_PARITY_ODD;
        //USART_InitStruct.Parity = LL_USART_PARITY_ODD;
		//ucDataBits++;
	//} else if(eParity == MB_PAR_EVEN) {
		////rs485_uart->Init.Parity = UART_PARITY_EVEN;
        //USART_InitStruct.Parity = LL_USART_PARITY_EVEN;
		//ucDataBits++;
	//} else {
		//return FALSE;
	//}
	
	//if(ucDataBits == 7) {
		////rs485_uart->Init.WordLength = UART_WORDLENGTH_7B;
        ////USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_7B;
	//} else if(ucDataBits == 8) {
		////rs485_uart->Init.WordLength = UART_WORDLENGTH_8B;
        //USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	//} else if(ucDataBits == 9) {
		////rs485_uart->Init.WordLength = UART_WORDLENGTH_9B;
        //USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
	//} else {
		//return FALSE;
	//}
	
  //if (HAL_UART_Init(rs485_uart) != HAL_OK) {
//  if (LL_USART_Init(UART4, &USART_InitStruct) != SUCCESS) {
//    return FALSE;
//  }

	return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
	if(xRxEnable == TRUE) {				
		//__HAL_UART_ENABLE_IT(&RS485_UART, UART_IT_RXNE);		
//        LL_USART_ClearFlag_RXNE(UART4);
		set_rs485_mode(RS485_RX);
        LL_USART_EnableIT_RXNE(UART4);
        //TODO:
		//HAL_UART_Receive_IT(&RS485_UART, &g_recv_byte, 1);
	} else {
		//__HAL_UART_DISABLE_IT(&RS485_UART, UART_IT_RXNE);
        LL_USART_DisableIT_RXNE(UART4);
	}	

	if(xTxEnable == TRUE) {
		set_rs485_mode(RS485_TX);
		//__HAL_UART_ENABLE_IT(&RS485_UART, UART_IT_TC);
        LL_USART_ClearFlag_TC(UART4);
        LL_USART_EnableIT_TC(UART4);
//        LL_USART_TransmitData8(UART4, 0);
	} else {
		set_rs485_mode(RS485_RX);
		//__HAL_UART_DISABLE_IT(&RS485_UART, UART_IT_TC);			
        LL_USART_DisableIT_TC(UART4);
	}
}

void vMBMasterPortClose(void)
{

}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
	//HAL_UART_Transmit_IT(&RS485_UART, (uint8_t *)&ucByte, 1);
    LL_USART_EnableIT_TC(UART4);
    LL_USART_TransmitData8(UART4, ucByte);
	return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
	*pucByte = g_recv_byte;
    //TODO:
	//HAL_UART_Receive_IT(&RS485_UART, &g_recv_byte, 1);
    g_recv_byte = LL_USART_ReceiveData8(UART4);
	*pucByte = g_recv_byte;
//	vMBMasterPortSerialEnable(TRUE, FALSE);
	return TRUE;
}

void UART4_IRQHandler(void)
{
    if (LL_USART_IsActiveFlag_TC(UART4)) {
        LL_USART_ClearFlag_TC(UART4);
        #if MB_MASTER_RTU_ENABLED > 0		
        {
            pxMBMasterFrameCBTransmitterEmpty();
        }
        #else		
        {
            pxMBFrameCBTransmitterEmpty();
        }
        #endif		
    }

    if (LL_USART_IsActiveFlag_RXNE(UART4)) {
        LL_USART_ClearFlag_RXNE(UART4);
        #if MB_MASTER_RTU_ENABLED > 0
		{
            pxMBMasterFrameCBByteReceived();
        }
        #else		
        {
            pxMBFrameCBByteReceived();
        }
        #endif
    }
}
#endif

#endif
