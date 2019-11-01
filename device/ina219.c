
#include <device/ina219.h>

INA219_struct ina219Obj;

static size_t ina219_Write_Register(INA219_HandleTypeDef *ina219, unsigned char reg, unsigned int dat)
{
    unsigned char val[2] = {0};
    
//    val[0] = (unsigned char)(dat >> 8);
//    val[1] = (unsigned char)(dat & 0xFF);
    //INA219_WRITE_REGISTER(ina219->Addr, reg, val);
//		HAL_I2C_Mem_Write(ina219->pI2cx, ina219->Addr, reg, 1, val, 2, 50);

    struct i2c_msg msgs[2];
    size_t len = 0;

	msgs[0].addr = ina219->Addr >> 1;
	msgs[0].flags = I2C_WR;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	val[0] = (unsigned char)(dat >> 8);
	val[1] = (unsigned char)(dat & 0xff);
	msgs[1].addr = ina219->Addr >> 1;
	msgs[1].flags = I2C_WR;
	msgs[1].len = 2;
	msgs[1].buf = val;

	len = i2c_transfer(ina219->i2c_bus, msgs, 2);

	return len;
}

static size_t ina219_Read_Register(INA219_HandleTypeDef *ina219, unsigned char reg, unsigned short *dat)
{
    //printf("read reg == %d\r\n",reg);
	unsigned char val[2] = {0};
    struct i2c_msg msgs[2];
	size_t len = 0;
  
  //INA219_READ_REGISTER(ina219addr, reg, val);
//	HAL_I2C_Mem_Read(ina219->pI2cx, ina219->Addr, reg, 1, val, 2, 50);
  
	msgs[0].addr = ina219->Addr >> 1;
	msgs[0].flags = I2C_WR;
	msgs[0].len = 1;
	msgs[0].buf = &reg;

	msgs[1].addr = ina219->Addr >> 1;
	msgs[1].flags = I2C_RD;
	msgs[1].len = 2;
	msgs[1].buf = val;
	len = i2c_transfer(ina219->i2c_bus, msgs, 2);

	*dat = ((unsigned int)(val[0]) << 8) + val[1]; 

	return len;
}

void INA219_Config(INA219_HandleTypeDef *ina219)
{
	uint16_t	CalValue = 0;
		// By default we use a pretty huge range for the input voltage,
  // which probably isn't the most appropriate choice for system
  // that don't use a lot of power.  But all of the calculations
  // are shown below if you want to change the settings.  You will
  // also need to change any relevant register settings, such as
  // setting the VBUS_MAX to 16V instead of 32V, etc.
  
  // VBUS_MAX     = 32V   (Assumes 16V, can also be set to 32V)
  // VSHUNT_MAX   = 0.04  (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
  // RSHUNT       = 0.01   (Resistor value in ohms)
  
  // 1. Determine max possible current
  // MaxPossible_I = VSHUNT_MAX / RSHUNT
  // MaxPossible_I = 4A
  
  // 2. Determine max expected current
  // MaxExpected_I = 4A
  
  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
  // MinimumLSB = MaxExpected_I/32768
  // MinimumLSB = 0.000122            (0.122mA per bit)
  // MaximumLSB = MaxExpected_I/4096
  // MaximumLSB = 0.000976            (0.976mA per bit)
  
  // 4. Choose an LSB between the min and max values
  //    (Preferrably a roundish number close to MinLSB)
  // CurrentLSB = 0.00020            (200uA per bit)
  
  // 5. Compute the calibration register
  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
  // Cal = 20480 (0x5000)
  
	
	CalValue = (uint16_t)(0.04096/((ina219->CurrentLSB * 0.000001) * (ina219->ShuntR * 0.001)));
  
  // 6. Calculate the power LSB
  // PowerLSB = 20 * CurrentLSB
  // PowerLSB = 0.01 (10mW per bit)
  
//	INA219_InitStruct->PowerLSB = 20 * INA219_InitStruct->CurrentLSB;
	
  // 7. Compute the maximum current and shunt voltage values before overflow
  //
  // Max_Current = Current_LSB * 32767
  // Max_Current = 16.3835A before overflow
  //
  // If Max_Current > Max_Possible_I then
  //    Max_Current_Before_Overflow = MaxPossible_I
  // Else
  //    Max_Current_Before_Overflow = Max_Current
  // End If
  //
  // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
  // Max_ShuntVoltage = 0.32V
  //
  // If Max_ShuntVoltage >= VSHUNT_MAX
  //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
  // Else
  //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
  // End If
  
  // 8. Compute the Maximum Power
  // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
  // MaximumPower = 1.6 * 16V
  // MaximumPower = 256W
  
  // Set multipliers to convert raw current/power values
//  ina219_current_LSB_uA = 200;     // Current LSB = 500uA per bit
//  ina219_power_LSB_mW = 4;        // Power LSB = 10mW per bit = 20 * Current LSB
  
  // Set Calibration register to 'Cal' calculated above
//	printf("ina219->CalValue = 0x%04X \r\n", ina219->CalValue);
  ina219_Write_Register(ina219, INA219_REG_CALIBRATION, CalValue);
}

void INA219_Init(INA219_HandleTypeDef *ina219, INA219_InitTypeDef *INA219_InitStruct)
{
	uint16_t configValue;
		  
  // Set Config register to take into account the settings above
  configValue = ( INA219_InitStruct->BusVol 
								| INA219_InitStruct->PGAMode 
								| INA219_InitStruct->BADC 
								| INA219_InitStruct->SADC 
								| INA219_InitStruct->WordMode );
  
//	printf("configValue = %02X \r\n", configValue);
  ina219_Write_Register(ina219, INA219_REG_CONFIG, configValue);
	
//	ina219Obj.pI2cx = ina219->pI2cx;  //Ĭ������ina219ʹ��ͬһ��i2c
	
	INA219_Config(ina219);
}

uint16_t ina219_GetBusVoltage_mV(INA219_HandleTypeDef *ina219x)
{
  uint16_t value;
  
  ina219_Read_Register(ina219x, INA219_REG_BUSVOLTAGE, &value);
//	printf("ina219_GetBusVoltage_mV val = %d \r\n", value);
  value >>= 3;                      // Shift to the right 3 to drop CNVR and OVF
  value *= 4;   // multiply by LSB(4mV) �ٷ��涨�������4mv
	
	ina219x->data.voltage = value;
	
  return (value);
}

uint16_t ina219_GetShuntVoltage_uV(INA219_HandleTypeDef *ina219x)
{
  uint16_t val;
	
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  //ina219_Write_Register(ina219addr, INA219_REG_CALIBRATION, ina219_calValue);
  
  ina219_Read_Register(ina219x, INA219_REG_SHUNTVOLTAGE, &val);
  val = (uint16_t)val * 10;   // multiply by LSB(10uV) ����datasheet��Ҫ��10uV
  
	ina219x->data.shuntV = val;
	
  return (val);
}

uint32_t ina219_GetCurrent_uA(INA219_HandleTypeDef *ina219x)
{
	uint16_t val;
  
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  //ina219_Write_Register(ina219addr, INA219_REG_CALIBRATION, ina219_calValue);
  
  // Now we can safely read the CURRENT register!
  ina219_Read_Register(ina219x, INA219_REG_CURRENT, &val);
  
	ina219x->data.current = (uint32_t)(val * ina219x->CurrentLSB);
  return ina219x->data.current;
}

uint16_t ina219_GetPower_mW(INA219_HandleTypeDef *ina219x)
{
  uint16_t val;
  
  // Sometimes a sharp load will reset the INA219, which will
  // reset the cal register, meaning CURRENT and POWER will
  // not be available ... avoid this by always setting a cal
  // value even if it's an unfortunate extra step
  //ina219_Write_Register(ina219addr, INA219_REG_CALIBRATION, ina219_calValue);
  
  // Now we can safely read the POWER register!
  ina219_Read_Register(ina219x, INA219_REG_POWER, &val);
  
	//PowerLSB = ina219x->CurrentLSB * 20;
  val = (uint16_t)(val * (ina219x->CurrentLSB * 20 / 1000));
  
  return (val);
}

void INA219_Get_All_Value(INA219_HandleTypeDef *ina219x)
{
	ina219x->data.current = ina219_GetCurrent_uA(ina219x);
	ina219x->data.shuntV	=	ina219_GetShuntVoltage_uV(ina219x);
	ina219x->data.power		=	ina219_GetPower_mW(ina219x);
	ina219x->data.voltage	= ina219_GetBusVoltage_mV(ina219x);	
}


/**
  * @brief  ina219_init.
  * @param
  * @retval
  */
static error_t ina219_init(device_t *dev)
{
	ina219_device_t *ina219 = (ina219_device_t *)dev;
	ina219_init_t args;
	uint16_t	value = 0;

	ina219->ops->init(ina219, &args);

	ina219->addr 	= args.addr;
	ina219->i_lsb 	= args.i_lsb;

	value = (uint16_t)(0.04096/((ina219->i_lsb * 0.000001) * (args.rnf * 0.001)));

	ina219->ops->write(ina219, INA219_REG_CALIBRATION, value);

	value = args.bus_vol
				| args.pga
				| args.badc
				| args.sadc
				| args.work_mode;

	ina219->ops->write(ina219, INA219_REG_CONFIG, value);

	return 0;
}

/**
  * @brief  led_ctrl.
  * @param
  * @retval
  */
static error_t ina219_ctrl(device_t *dev, uint8_t cmd, void *args)
{
	ina219_device_t *ina219 = (ina219_device_t *)dev;
	uint16_t *value = args;

	switch (cmd)
	{
		case GET_SHUNT_VOL:
		{
			ina219->ops->read(ina219, INA219_REG_SHUNTVOLTAGE, value);
			*value *= 10;  //multiply by LSB(10uV), as datasheet result should multiply by 10uv
			break;
		}
		case GET_CURRENT:
		{
			// Sometimes a sharp load will reset the INA219, which will
			// reset the cal register, meaning CURRENT and POWER will
			// not be available ... avoid this by always setting a cal
			// value even if it's an unfortunate extra step
			//ina219_Write_Register(ina219addr, INA219_REG_CALIBRATION, ina219_calValue);
			ina219_init(dev);

			// Now we can safely read the CURRENT register!
			ina219->ops->read(ina219, INA219_REG_CURRENT, value);
			*value *= ina219->i_lsb;
			break;
		}
		case GET_VOLTAGE:
		{
			ina219->ops->read(ina219, INA219_REG_BUSVOLTAGE, value);
//			*value >>= 3;  // Shift to the right 3 to drop CNVR and OVF
//			*value *= 4;   // multiply by LSB(4mV), as datasheet result should multiply by 4mv
			// summary
			*value >>= 1;
			break;
		}
		case GET_POWER:
		{
			// Sometimes a sharp load will reset the INA219, which will
			// reset the cal register, meaning CURRENT and POWER will
			// not be available ... avoid this by always setting a cal
			// value even if it's an unfortunate extra step
			//ina219_Write_Register(ina219addr, INA219_REG_CALIBRATION, ina219_calValue);
			ina219_init(dev);

			// Now we can safely read the POWER register!
			ina219->ops->read(ina219, INA219_REG_POWER, value);
			*value = *value * (ina219->i_lsb * 20 / 1000);
			break;
		}
		default:
		{
			break;
		}
	}

	return 0;
}

/**
  * @brief  ina219_ops.
  * @param
  * @retval
  */
static struct device_ops ina219_ops =
{
	.init = ina219_init,
	.ctrl = ina219_ctrl,
};

error_t ina219_device_register(ina219_device_t *ina219, const char *name, uint32_t flag, void *data)
{
	device_t *dev = &(ina219->parent);

	dev->ops 		= &ina219_ops;
	dev->user_data	= data;

	return device_register(dev, name, flag);
}

