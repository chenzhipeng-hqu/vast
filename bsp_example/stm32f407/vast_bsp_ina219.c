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
#include "vast_device/ina219.h"
#include "vast_driver/vast_log.h"
#include "vast_driver/i2c.h"

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/
static ina219_device_t ina219_dev;


/***********************************************
                   function
***********************************************/
/**
  * @brief  ina219_init_bsp.
  * @param
  * @retval
  */
static err_t ina219_init_bsp(ina219_device_t *ina219, ina219_init_t *args)
{
	args->rnf 		= 10;	// 10 mΩ
	args->i_lsb 	= 200;  // calculate method in INA219_Config()
	args->addr 		= INA219_I2C_ADDRESS_CONF_0;
	args->bus_vol 	= INA219_CFG_BVOLT_RANGE_32V;
	args->pga 		= INA219_CFG_SVOLT_RANGE_40MV;
	args->badc 		= INA219_CFG_BADCRES_12BIT_128S_68MS;
	args->sadc 		= INA219_CFG_SADCRES_12BIT_128S_68MS;
	args->work_mode = INA219_CFG_MODE_SANDBVOLT_CONTINUOUS;

    return 0;
}

/**
  * @brief  ina219_read_bsp.
  * @param
  * @retval
  */
static size_t ina219_read_bsp(ina219_device_t *ina219, uint8_t reg, uint16_t *dat)
{
	size_t len = 0;
	struct i2c_bus_device *i2c_bus = (struct i2c_bus_device *)(ina219->parent.user_data);

    struct i2c_msg msgs[2];
    uint8_t val[2] = {0, 0};

    msgs[0].addr = ina219->addr >> 1;
    msgs[0].flags = I2C_WR;
    msgs[0].len = 1;
    msgs[0].buf = &reg;

    msgs[1].addr = ina219->addr >> 1;
    msgs[1].flags = I2C_RD;
    msgs[1].len = 2;
    msgs[1].buf = val;

    len = i2c_bus->ops->master_xfer(i2c_bus, msgs, 2);

    *dat = val[0]<<8 | val[1];

    return len;
}

/**
  * @brief  ina219_write_bsp.
  * @param
  * @retval
  */
static size_t ina219_write_bsp(ina219_device_t *ina219, uint8_t reg, uint16_t dat)
{
	size_t len = 0;
	struct i2c_bus_device *i2c_bus = (struct i2c_bus_device *)(ina219->parent.user_data);

	struct i2c_msg msgs[2];
	uint8_t val[2] = {0, 0};

	msgs[0].addr = ina219->addr >> 1;
	msgs[0].flags = I2C_WR;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	val[0] = (unsigned char)(dat >> 8);
	val[1] = (unsigned char)(dat & 0xff);
	msgs[1].addr = ina219->addr >> 1;
	msgs[1].flags = I2C_WR;
	msgs[1].len = 2;
	msgs[1].buf = val;

	len = i2c_bus->ops->master_xfer(i2c_bus, msgs, 2);

	return len;
}

/**
  * @brief  ina219_ops.
  * @param
  * @retval
  */
static const ina219_ops_t ina219_ops =
{
	.init 	= ina219_init_bsp,
	.read 	= ina219_read_bsp,
	.write 	= ina219_write_bsp,
};

/**
  * @brief  vast_ina219_bsp_init.
  * @param
  * @retval
  */
static int vast_ina219_bsp_init(void)
{
	err_t ret = 0;

	struct i2c_bus_device *i2c_bus = (struct i2c_bus_device *)device_find("i2c");

	if (i2c_bus == NULL)
	{
		return -ENOSYS;
	}

	ina219_dev.ops = &ina219_ops;
	ret = ina219_device_register(&ina219_dev, "ina219", 0, (void *)i2c_bus);

	ASSERT(ret == VAST_OK,
			printf("ret=%d\r\n", ret);
			return ret
			);

	// test
	uint16_t value = 0;
	device_open(&ina219_dev.parent, 0);
	device_ctrl(&ina219_dev.parent, GET_SHUNT_VOL, &value);
	printf("shunt vol = %duV\r\n", value);
	device_ctrl(&ina219_dev.parent, GET_CURRENT, &value);
	printf("current = %duA\r\n", value);
	device_ctrl(&ina219_dev.parent, GET_VOLTAGE, &value);
	printf("busvol = %dmV\r\n", value);
	device_ctrl(&ina219_dev.parent, GET_POWER, &value);
	printf("power = %dmW\r\n", value);
	device_close(&ina219_dev.parent);

	return 0;
}

device_initcall(vast_ina219_bsp_init);
/**
  * @}
  */

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/



