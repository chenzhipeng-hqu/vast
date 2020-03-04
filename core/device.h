#ifndef __DEVICE_H__
#define __DEVICE_H__

#include <core/kfifo.h>
#include <core/object.h>
#include <core/softtimer.h>
#include <core/types.h>
#include "vast_config.h"

/**
 * device (I/O) class type
 */
enum device_class_type
{
    Device_Class_Char = 0,                  /**< character device */
    Device_Class_Block,                     /**< block device */
    Device_Class_SPIBUS,
    Device_Class_SPIDevice,
    Device_Class_I2CBUS,                    /**< i2c device */
    Device_Class_Unknown                    /**< unknown device */
};
/**
 * device flags defitions
 */
#define DEVICE_FLAG_ACTIVATED        0x0001     /* device is activated */
#define DEVICE_FLAG_OPENED           0x0002     /* device is opened */
#define DEVICE_FLAG_FASYNC           0x0100

#define DEVICE_FLAG_INT_RX           0x01       /* INT mode on Rx */
#define DEVICE_FLAG_DMA_RX           0x02       /* DMA mode on Rx */
#define DEVICE_FLAG_INT_TX           0x04       /* INT mode on Tx */
#define DEVICE_FLAG_DMA_TX           0x08       /* DMA mode on Tx */

#define SERIAL_CTRL_GETCFG           0x01
#define SERIAL_CTRL_SETCFG           0x02
#define DEVICE_CTRL_SET_INT          0x10       /* enable receive irq */
#define DEVICE_CTRL_CLR_INT          0x11       /* disable receive irq */
#define DEVICE_CTRL_GET_INT          0x12
#define DEVICE_CTRL_SET_TX_INT       0x13       /* enable transmit irq */
#define DEVICE_CTRL_CLR_TX_INT       0x14       /* disable transmit irq */
#define DEVICE_CTRL_ADD_OUT          0x15       /* add fifo out */
#define DEVICE_CTRL_INT_PRIO      	 0x16       /* modify Priority */
#define DEVICE_CTRL_SET_TX_DMA       0x17       /* enable transmit dma */
#define DEVICE_CTRL_CLR_TX_DMA       0x18       /* disnable transmit dma */
#define DEVICE_CTRL_SET_RX_DMA       0x19       /* enable transmit dma */
#define DEVICE_CTRL_CLR_RX_DMA       0x1a       /* disnable transmit dma */
#define DEVICE_CTRL_SET_TX_DMA_LEN   0x1b       /* set tramsimt dma length */
#define DEVICE_CTRL_SET_RX_DMA_LEN   0x1c       /* set receive dma length */
#define DEVICE_CTRL_GET_RX_DMA_LEN   0x1d       /* get receive dma length */

#define DEVICE_CTRL_BLK_GETGEOME     0x10       /**< get geometry information   */
#define DEVICE_CTRL_BLK_SYNC         0x11       /**< flush data to block device */
#define DEVICE_CTRL_BLK_ERASE        0x12       /**< erase block on block device */
#define DEVICE_CTRL_BLK_AUTOREFRESH  0x13       /**< block device : enter/exit auto refresh mode */

#define RTC_CTRL_GET_TIME            0x01
#define RTC_CTRL_SET_TIME            0x02
#define RTC_CTRL_SET_DATE            0x03
#define RTC_CTRL_ADD_ALARM           0x04
#define RTC_CTRL_DEL_ALARM           0x05

enum KEY_CTRL
{
    KEY_CTRL_GET_KEY,
};

enum SWITCH_CTRL
{
	CTRL_ON,
	CTRL_OFF,
	CTRL_NORMAL,
	CTRL_EMCY,
	CTRL_ON_OFF,
	CTRL_BACKLIGHT,
	CTRL_RGB,
	CTRL_MODE_RGB,
};

typedef struct device device_t;

typedef struct device_ops
{
    error_t(*init)(device_t *dev);
    error_t(*open)(device_t *dev, uint16_t oflag);
    error_t(*close)(device_t *dev);
    size_t (*read)(device_t *dev, off_t pos, void *buffer, size_t size);
    size_t (*peek)(device_t *dev, off_t pos, void *buffer, size_t size);
    size_t (*write)(device_t *dev, off_t pos, const void *buffer, size_t size);
    error_t(*ctrl)(device_t *dev, uint8_t cmd, void *args);
}device_ops_t;

#define ARGS2U80(args) (__VAST_GET_BITS((u32)(args), 0,  7))
#define ARGS2U81(args) (__VAST_GET_BITS((u32)(args), 8,  15))
#define ARGS2U82(args) (__VAST_GET_BITS((u32)(args), 16, 23))
#define ARGS2U83(args) (__VAST_GET_BITS((u32)(args), 24, 31))

struct device
{
    struct object  parent;
    u16  flag;
    u16  open_flag;
    u8   ref_count;
    const struct device_ops *ops;
    void  *owner;
    void  *user_data;

    /* device call back */
    error_t (*rx_indicate)(device_t *dev, size_t size);
    error_t (*tx_complete)(device_t *dev, void *buffer);
};


device_t *device_find(const char *name);
error_t    device_register(device_t *dev, const char *name, uint16_t flags);
void     device_unregister(device_t *dev);
error_t    device_open(device_t *dev, uint16_t oflag);
error_t    device_close(device_t *dev);
size_t   device_peek(device_t *dev, off_t pos, void *buffer, size_t size);
size_t   device_read(device_t *dev, off_t pos, void *buffer, size_t size);
size_t   device_write(device_t *dev, off_t pos, const void *buffer, size_t size);
error_t    device_ctrl(device_t *dev, uint8_t cmd, void *arg);
void     device_set_owner(device_t *dev, const void *owner);
error_t device_set_rx_indicate(device_t *dev, error_t (*rx_ind)(device_t *dev, size_t size));
error_t device_set_tx_complete(device_t *dev, error_t (*tx_done)(device_t *dev, void *buffer));

#ifndef pdMS_TO_TICKS
# define pdMS_TO_TICKS(xTimeInMs) \
    ((time_t)(((time_t)(xTimeInMs)*(time_t)configHZ)/(time_t)1000))
#endif

#ifdef configUSING_SERIAL
	#include <driver/vast_serial.h>
#endif

#ifdef OS_USING_HEAP
	#include <core/heap.h>
#endif


#ifdef configUSING_EASY_KEY
	#include <vast_key.h>
#endif

#ifdef configUSING_SPI
	#include "drivers/spi/spi.h"
	#ifdef configUSING_SPI_BITBANG
		#include "drivers/spi/spi_bitbang.h"
	#endif
#endif

#ifdef configUSING_W25Qxx
	#include "drivers/flash/spi_flash.h"
	#include "drivers/flash/spi_flash_w25qxx.h"
#endif

#ifdef configUSING_W5500
	#include "drivers/net/w5500.h"
#endif

#ifdef configUSING_I2C
	#include <driver/i2c/i2c.h>
	#include <driver/i2c/i2c_dev.h>
	#include <driver/i2c/i2c-bit-ops.h>
#endif

#ifdef configUSING_INA219
	#include <device/ina219.h>
#endif

#ifdef configUSING_CLI
	#include <component/vast_cli.h>
#endif

#ifdef configUSING_EEPROM
	#include <component/vast_simulatite_eeprom.h>
#endif

#ifdef configUSING_IR
	#include <device/vast_ir.h>
#endif

#ifdef configUSING_LED
	#include <device/vast_led.h>
#endif

#ifdef configUSING_RX8025
	#include "drivers/rtc/rx8025.h"
#endif

#ifdef configUSING_AT24C
	#include "drivers/eeprom/at24cxx.h"
#endif

#ifdef configUSING_HDC1080
	#include "drivers/hdc1080.h"
#endif

#ifdef configUSING_SI7020
	#include "drivers/si7020.h"
#endif

#ifdef configUSING_LCD
	#include "drivers/lcd.h"
#endif

#ifdef configUSING_MODBUS_RTU
    #include "component/modbus_rtu.h"
#endif

#ifdef configUSING_MODBUS_RTU2
    #include "driver/modbus_rtu.h"
#endif


#endif /* __DEVICE_H__ */

