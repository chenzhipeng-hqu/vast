
#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"
#include "vast_driver/i2c.h"

//#define INA219_I2C_PORT                   GPIOB
//#define INA219_I2C_GPIO_CLOCK            RCC_APB2Periph_GPIOB
//#define INA219_I2C_SCL_PIN                GPIO_Pin_12
//#define INA219_I2C_SDA_PIN                GPIO_Pin_14

//#define INA_SCL_SET     GPIO_SetBits(INA219_I2C_PORT,INA219_I2C_SCL_PIN)
//#define INA_SDA_SET     GPIO_SetBits(INA219_I2C_PORT, INA219_I2C_SDA_PIN)

//#define INA_SCL_CLR     GPIO_ResetBits(INA219_I2C_PORT,INA219_I2C_SCL_PIN)
//#define INA_SDA_CLR     GPIO_ResetBits(INA219_I2C_PORT,INA219_I2C_SDA_PIN)

//#define INA_SDA_TST     GPIO_ReadInputDataBit(INA219_I2C_PORT,INA219_I2C_SDA_PIN)

/*----------------------------------------------------------------------------*/
// I2C Address Options
#define INA219_I2C_ADDRESS_CONF_0               (uint8_t)(0x40 << 1)     // A0 = GND, A1 = GND
#define INA219_I2C_ADDRESS_CONF_1               (uint8_t)(0x41 << 1)     // A0 = VS+, A1 = GND
#define INA219_I2C_ADDRESS_CONF_2               (uint8_t)(0x42 << 1)     // A0 = SDA, A1 = GND
#define INA219_I2C_ADDRESS_CONF_3               (uint8_t)(0x43 << 1)     // A0 = SCL, A1 = GND
#define INA219_I2C_ADDRESS_CONF_4               (uint8_t)(0x44 << 1)     // A0 = GND, A1 = VS+
#define INA219_I2C_ADDRESS_CONF_5               (uint8_t)(0x45 << 1)     // A0 = VS+, A1 = VS+
#define INA219_I2C_ADDRESS_CONF_6               (uint8_t)(0x46 << 1)     // A0 = SDA, A1 = VS+
#define INA219_I2C_ADDRESS_CONF_7               (uint8_t)(0x47 << 1)     // A0 = SCL, A1 = VS+
#define INA219_I2C_ADDRESS_CONF_8               (uint8_t)(0x48 << 1)     // A0 = GND, A1 = SDA
#define INA219_I2C_ADDRESS_CONF_9               (uint8_t)(0x49 << 1)     // A0 = VS+, A1 = SDA
#define INA219_I2C_ADDRESS_CONF_A               (uint8_t)(0x4A << 1)     // A0 = SDA, A1 = SDA
#define INA219_I2C_ADDRESS_CONF_B               (uint8_t)(0x4B << 1)     // A0 = SCL, A1 = SDA
#define INA219_I2C_ADDRESS_CONF_C               (uint8_t)(0x4C << 1)     // A0 = GND, A1 = SCL
#define INA219_I2C_ADDRESS_CONF_D               (uint8_t)(0x4D << 1)     // A0 = VS+, A1 = SCL
#define INA219_I2C_ADDRESS_CONF_E               (uint8_t)(0x4E << 1)     // A0 = SDA, A1 = SCL
#define INA219_I2C_ADDRESS_CONF_F               (uint8_t)(0x4F << 1)     // A0 = SCL, A1 = SCL
#define INA219_I2C_ADDRESS                      INA219_I2C_ADDRESS_CONF_0


/*----------------------------------------------------------------------------*/
// Register Addresses
#define INA219_REG_CONFIG                       (uint8_t)(0x00)      // CONFIG REGISTER (R/W)
#define INA219_REG_SHUNTVOLTAGE                 (uint8_t)(0x01)      // SHUNT VOLTAGE REGISTER (R)
#define INA219_REG_BUSVOLTAGE                   (uint8_t)(0x02)      // BUS VOLTAGE REGISTER (R)
#define INA219_REG_POWER                        (uint8_t)(0x03)      // POWER REGISTER (R)
#define INA219_REG_CURRENT                      (uint8_t)(0x04)      // CURRENT REGISTER (R)
#define INA219_REG_CALIBRATION                  (uint8_t)(0x05)      // CALIBRATION REGISTER (R/W)


/*----------------------------------------------------------------------------*/
// Macros for assigning config bits
#define INA219_CFGB_RESET(x)                    (uint16_t)((x & 0x01) << 15)     // Reset Bit
#define INA219_CFGB_BUSV_RANGE(x)               (uint16_t)((x & 0x01) << 13)     // Bus Voltage Range
#define INA219_CFGB_PGA_RANGE(x)                (uint16_t)((x & 0x03) << 11)     // Shunt Voltage Range
#define INA219_CFGB_BADC_RES_AVG(x)             (uint16_t)((x & 0x0F) << 7)      // Bus ADC Resolution/Averaging
#define INA219_CFGB_SADC_RES_AVG(x)             (uint16_t)((x & 0x0F) << 3)      // Shunt ADC Resolution/Averaging
#define INA219_CFGB_MODE(x)                     (uint16_t) (x & 0x07)            // Operating Mode


/*----------------------------------------------------------------------------*/
// Configuration Register
#define INA219_CFG_RESET                        INA219_CFGB_RESET(1)            // Reset Bit

//INA219_CFGB_BUSV_RANGE(x)
#define INA219_CFG_BVOLT_RANGE_MASK             INA219_CFGB_BUSV_RANGE(1)       // Bus Voltage Range Mask
#define INA219_CFG_BVOLT_RANGE_16V              INA219_CFGB_BUSV_RANGE(0)       // 0-16V Range
#define INA219_CFG_BVOLT_RANGE_32V              INA219_CFGB_BUSV_RANGE(1)       // 0-32V Range (default)

//INA219_CFGB_PGA_RANGE(x)
#define INA219_CFG_SVOLT_RANGE_MASK             INA219_CFGB_PGA_RANGE(3)        // Shunt Voltage Range Mask
#define INA219_CFG_SVOLT_RANGE_40MV             INA219_CFGB_PGA_RANGE(0)        // Gain 1, 40mV Range
#define INA219_CFG_SVOLT_RANGE_80MV             INA219_CFGB_PGA_RANGE(1)        // Gain 2, 80mV Range
#define INA219_CFG_SVOLT_RANGE_160MV            INA219_CFGB_PGA_RANGE(2)        // Gain 4, 160mV Range
#define INA219_CFG_SVOLT_RANGE_320MV            INA219_CFGB_PGA_RANGE(3)        // Gain 8, 320mV Range (default)

//INA219_CFGB_BADC_RES_AVG(x)
#define INA219_CFG_BADCRES_MASK                 INA219_CFGB_BADC_RES_AVG(15)    // Bus ADC Resolution and Averaging Mask
#define INA219_CFG_BADCRES_9BIT_1S_84US         INA219_CFGB_BADC_RES_AVG(0)     // 1 x 9-bit Bus sample
#define INA219_CFG_BADCRES_10BIT_1S_148US       INA219_CFGB_BADC_RES_AVG(1)     // 1 x 10-bit Bus sample
#define INA219_CFG_BADCRES_11BIT_1S_276US       INA219_CFGB_BADC_RES_AVG(2)     // 1 x 11-bit Bus sample
#define INA219_CFG_BADCRES_12BIT_1S_532US       INA219_CFGB_BADC_RES_AVG(3)     // 1 x 12-bit Bus sample (default)
#define INA219_CFG_BADCRES_12BIT_2S_1MS         INA219_CFGB_BADC_RES_AVG(9)     // 2 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_4S_2MS         INA219_CFGB_BADC_RES_AVG(10)    // 4 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_8S_4MS         INA219_CFGB_BADC_RES_AVG(11)    // 8 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_16S_8MS        INA219_CFGB_BADC_RES_AVG(12)    // 16 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_32S_17MS       INA219_CFGB_BADC_RES_AVG(13)    // 32 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_64S_34MS       INA219_CFGB_BADC_RES_AVG(14)    // 64 x 12-bit Bus samples averaged together
#define INA219_CFG_BADCRES_12BIT_128S_68MS      INA219_CFGB_BADC_RES_AVG(15)    // 128 x 12-bit Bus samples averaged together

//INA219_CFGB_SADC_RES_AVG(x)
#define INA219_CFG_SADCRES_MASK                 INA219_CFGB_SADC_RES_AVG(15)    // Shunt ADC Resolution and Averaging Mask
#define INA219_CFG_SADCRES_9BIT_1S_84US         INA219_CFGB_SADC_RES_AVG(0)     // 1 x 9-bit Shunt sample
#define INA219_CFG_SADCRES_10BIT_1S_148US       INA219_CFGB_SADC_RES_AVG(1)     // 1 x 10-bit Shunt sample
#define INA219_CFG_SADCRES_11BIT_1S_276US       INA219_CFGB_SADC_RES_AVG(2)     // 1 x 11-bit Shunt sample
#define INA219_CFG_SADCRES_12BIT_1S_532US       INA219_CFGB_SADC_RES_AVG(3)     // 1 x 12-bit Shunt sample (default)
#define INA219_CFG_SADCRES_12BIT_2S_1MS         INA219_CFGB_SADC_RES_AVG(9)     // 2 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_4S_2MS         INA219_CFGB_SADC_RES_AVG(10)    // 4 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_8S_4MS         INA219_CFGB_SADC_RES_AVG(11)    // 8 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_16S_8MS        INA219_CFGB_SADC_RES_AVG(12)    // 16 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_32S_17MS       INA219_CFGB_SADC_RES_AVG(13)    // 32 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_64S_34MS       INA219_CFGB_SADC_RES_AVG(14)    // 64 x 12-bit Shunt samples averaged together
#define INA219_CFG_SADCRES_12BIT_128S_68MS      INA219_CFGB_SADC_RES_AVG(15)    // 128 x 12-bit Shunt samples averaged together

//INA219_CFGB_MODE(x)
#define INA219_CFG_MODE_MASK                    INA219_CFGB_MODE(7)             // Operating Mode Mask
#define INA219_CFG_MODE_POWERDOWN               INA219_CFGB_MODE(0)             // Power-Down
#define INA219_CFG_MODE_SVOLT_TRIGGERED         INA219_CFGB_MODE(1)             // Shunt Voltage, Triggered
#define INA219_CFG_MODE_BVOLT_TRIGGERED         INA219_CFGB_MODE(2)             // Bus Voltage, Triggered
#define INA219_CFG_MODE_SANDBVOLT_TRIGGERED     INA219_CFGB_MODE(3)             // Shunt and Bus, Triggered
#define INA219_CFG_MODE_ADCOFF                  INA219_CFGB_MODE(4)             // ADC Off (disabled)
#define INA219_CFG_MODE_SVOLT_CONTINUOUS        INA219_CFGB_MODE(5)             // Shunt Voltage, Continuous
#define INA219_CFG_MODE_BVOLT_CONTINUOUS        INA219_CFGB_MODE(6)             // Bus Voltage, Continuous
#define INA219_CFG_MODE_SANDBVOLT_CONTINUOUS    INA219_CFGB_MODE(7)             // Shunt and Bus, Continuous (default)


/*----------------------------------------------------------------------------*/
// Bus Voltage Register
#define INA219_BVOLT_CNVR                       (uint16_t)(0x0002)       // Conversion Ready
#define INA219_BVOLT_OVF                        (uint16_t)(0x0001)       // Math Overflow Flag

typedef struct
{
	//const master_i2c_public *pMi2c;
//	I2C_HandleTypeDef *pI2cx;
	uint8_t temp;

}INA219_struct;

typedef struct
{
	uint16_t 	BusVol;  		//  @ref:INA219_CFGB_BUSV_RANGE
	uint16_t	PGAMode;										//@ref:INA219_CFGB_PGA_RANGE(x)
	uint16_t 	BADC;
	uint16_t 	SADC;
	uint16_t 	WordMode;
}INA219_InitTypeDef;

typedef struct
{
  uint16_t voltage;
  uint16_t shuntV;
  uint32_t current;
  uint32_t power;
}INA219_DATA;

typedef struct
{
//	I2C_HandleTypeDef *pI2cx;  //i2c interface
	struct i2c_bus_device *i2c_bus;
	uint16_t 	Addr;
	uint16_t	ShuntR;		//uint�� m��  ��������
	uint16_t	CurrentLSB;  //unit: uA
//	uint16_t	PowerLSB;  //unit: mw
//	uint16_t	CalValue;
	INA219_DATA data;
}INA219_HandleTypeDef;

struct _ina219_device_t;

typedef struct _ina219_init_t
{
	uint16_t rnf;
	uint16_t bus_vol;
	uint16_t pga;
	uint16_t badc;
	uint16_t sadc;
	uint16_t work_mode;
	uint16_t addr;
	uint16_t i_lsb;
}ina219_init_t;

typedef struct _ina219_ops_t
{
	err_t	(*init)		(struct _ina219_device_t *ina219, ina219_init_t *args);
	size_t 	(*read)		(struct _ina219_device_t *ina219, uint8_t reg, uint16_t *dat);
	size_t 	(*write)	(struct _ina219_device_t *ina219, uint8_t reg, uint16_t dat);
}ina219_ops_t;

typedef struct _ina219_device_t
{
	struct device 		parent;
 	const ina219_ops_t 	*ops;
	uint16_t 			addr;
//	uint16_t			rnf;		//uint: mΩ RNF:sampling resistor
	uint16_t			i_lsb;  	//unit: uA
}ina219_device_t;

enum INA219_CTRL
{
    GET_SHUNT_VOL,
    GET_CURRENT,
    GET_VOLTAGE,
    GET_POWER,
};

extern INA219_struct ina219Obj;

//#define	INA219_WRITE_REGISTER(addr, reg, buf) 	ina219Obj.pMi2c->i2cWrite(addr, reg, 2, buf);
//#define	INA219_READ_REGISTER(addr, reg, buf) 		ina219Obj.pMi2c->i2cRead(addr, reg, 2, buf);
#define	INA219_WRITE_REGISTER(addr, reg, buf) 	HAL_I2C_Mem_Write(ina219Obj.pI2cx, addr, reg, 1, buf, 2, 50);
#define	INA219_READ_REGISTER(addr, reg, buf) 		HAL_I2C_Mem_Read(ina219Obj.pI2cx, addr, reg, 1, buf, 2, 50);

extern void INA219_Init(INA219_HandleTypeDef *ina219, INA219_InitTypeDef *INA219_InitStruct);
extern uint16_t ina219_GetBusVoltage_mV(INA219_HandleTypeDef *ina219x);
extern uint16_t ina219_GetShuntVoltage_uV(INA219_HandleTypeDef *ina219x);
extern uint32_t ina219_GetCurrent_uA(INA219_HandleTypeDef *ina219x);
extern uint16_t ina219_GetPower_mW(INA219_HandleTypeDef *ina219x);
extern void INA219_Get_All_Value(INA219_HandleTypeDef *ina219x);
extern void INA219_Config(INA219_HandleTypeDef *ina219 );


err_t ina219_device_register(ina219_device_t *ina219, const char *name, uint32_t flag, void *data);

#endif

