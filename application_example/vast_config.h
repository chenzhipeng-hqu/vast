#ifndef _CONFIG_H_
#define _CONFIG_H_

#define configSYS_CLOCK   (168000000U)
#define configHZ          (1000U)

//#define configUSE_PLL
//#define configUSE_HOSE
//#define OS_USING_HEAP
#define configTOTAL_HEAP_SIZE   (48*1024)

//#define configUSING_RTOS

#define configUSING_TFTP

//#define configUSING_STORE

#define configUSING_DEBUG
#define configUSING_SERIAL_DBG
//#define configUSING_SHELL
#define configUSING_CLI
#define configUSING_LOG
#define configUSING_EASYFLASH

#define configUSING_SERIAL
#define configUSING_SERIAL_DMA
//#define configUSING_SERIAL_INT
#define configUSING_USART1
#define configUSING_USART2
#define configUSING_USART3
//#define configUSING_UART4
//#define configUSING_UART5
//#define configSERIAL_DATA_READ_TIMEOUT     (2*configHZ)
//#define configUSING_FRAME_TIMEOUT_SOFT
//#define configSERIAL_RX_TO                (configHZ/50)

//#define configUSING_IR

#define configUSING_MODBUS_RTU

#define configUSING_I2C
//#define configUSING_I2C_HARD
#define configUSING_I2C_BIT
#define configUSING_I2C1

#define configUSING_INA219

//#define configUSING_EEPROM

#define configUSING_LED

#define configUSING_JSON

#define configUSING_SSR_V220

//#define configUSING_KEY

//#define configUSING_LOCALTIME
//#define configUSING_RTC

//#define configTEST
//#define configTEST_LED
//#define configTEST_SOFTTIMER
//#define configTEST_USART1
//#define configTEST_USART2
//#define configTEST_USART3
//#define configTEST_UART4
//#define configTEST_UART5
#define configTEST_MODBUS_RTU


#endif
