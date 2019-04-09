
#include "ina219.h"

INA219_struct ina219Obj;

static void ina219_Write_Register(INA219_HandleTypeDef *ina219, unsigned char reg, unsigned int dat)
{
    unsigned char val[2];
    
    val[0] = (unsigned char)(dat >> 8);
    val[1] = (unsigned char)(dat & 0xFF);
    //INA219_WRITE_REGISTER(ina219->Addr, reg, val);
		HAL_I2C_Mem_Write(ina219->pI2cx, ina219->Addr, reg, 1, val, 2, 50);;
}

static void ina219_Read_Register(INA219_HandleTypeDef *ina219, unsigned char reg, unsigned short *dat)
{
    //printf("read reg == %d\r\n",reg);
  unsigned char val[2];
  
  //INA219_READ_REGISTER(ina219addr, reg, val);
	HAL_I2C_Mem_Read(ina219->pI2cx, ina219->Addr, reg, 1, val, 2, 50);
  
	*dat = ((unsigned int)(val[0]) << 8) + val[1]; 
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
	
	ina219Obj.pI2cx = ina219->pI2cx;  //默认所有ina219使用同一个i2c
	
	INA219_Config(ina219);
}

uint16_t ina219_GetBusVoltage_mV(INA219_HandleTypeDef *ina219x)
{
  uint16_t value;
  
  ina219_Read_Register(ina219x, INA219_REG_BUSVOLTAGE, &value);
//	printf("ina219_GetBusVoltage_mV val = %d \r\n", value);
  value >>= 3;                      // Shift to the right 3 to drop CNVR and OVF
  value *= 4;   // multiply by LSB(4mV) 官方规定结果乘以4mv
	
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
  val = (uint16_t)val * 10;   // multiply by LSB(10uV) 按照datasheet需要乘10uV
  
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
	
//	ina219_Read_Register(ina219x->Addr, INA219_REG_CURRENT, (uint16_t *)&ina219x->ina219_data.current_ina219);
//	ina219_Read_Register(ina219x->Addr, INA219_REG_SHUNTVOLTAGE, &ina219x->ina219_data.shunt_ina219);
//	ina219_Read_Register(ina219x->Addr, INA219_REG_POWER, &ina219x->ina219_data.power_ina219);
//	ina219_Read_Register(ina219x->Addr, INA219_REG_BUSVOLTAGE, &ina219x->ina219_data.voltage_ina219);
	
//	printf("voltage is %d\r\n", (uint16_t)ina219x->ina219_data.voltage_ina219);
//	printf("shunt is %d\r\n", (uint16_t)ina219x->ina219_data.shunt_ina219);
//	printf("current is %d \r\n", (uint32_t)ina219x->ina219_data.current_ina219);
//	printf("power is %d\r\n", (uint16_t)ina219x->ina219_data.power_ina219);
//	printf("\r\n");
}
