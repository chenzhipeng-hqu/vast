
#ifndef __AW20108_H__
#define __AW20108_H__

#include <stdint.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------*/
// I2C Address Options
#define	AW20108_I2C_ADDRESS_A7_A0									0x70
#define AW20108_I2C_ADDRESS_CONF_0               (uint8_t)(AW20108_I2C_ADDRESS_A7_A0|0x04)     // AD = GND
#define AW20108_I2C_ADDRESS_CONF_1               (uint8_t)(AW20108_I2C_ADDRESS_A7_A0|0x06)     // AD = VS+
#define AW20108_I2C_ADDRESS_CONF_2               (uint8_t)(AW20108_I2C_ADDRESS_A7_A0|0x02)     // AD = SDA
#define AW20108_I2C_ADDRESS_CONF_3               (uint8_t)(AW20108_I2C_ADDRESS_A7_A0|0x00)     // AD = SCL


/*----------------------------------------------------------------------------*/
// Pagex Register Addresses
#define AW20108_REG_PAGE	                       	(uint8_t)(0xF0)      // CONFIG REGISTER (R/W)

/*----------------------------------------------------------------------------*/
// Pagex Register
#define AW20108_PAGE(x)                        		(uint8_t)((x&0x07)|0xC0)            // Reset Bit
#define AW20108_PAGE0                        	 		AW20108_PAGE(0)            // Reset Bit
#define AW20108_PAGE1                        	 		AW20108_PAGE(1)            // Reset Bit
#define AW20108_PAGE2                        	 		AW20108_PAGE(2)            // Reset Bit
#define AW20108_PAGE3                        	 		AW20108_PAGE(3)            // Reset Bit
#define AW20108_PAGE4                        	 		AW20108_PAGE(4)            // Reset Bit
#define AW20108_PAGE5                        	 		AW20108_PAGE(5)            // Reset Bit

/*----------------------------------------------------------------------------*/
// Page0 Register Address
#define AW20108_P0_REG_IDR		                      (uint8_t)(0x00)            // Reset Bit
#define AW20108_P0_REG_SLPCR                        (uint8_t)(0x01)             // Reset Bit
#define AW20108_P0_REG_RSTR                        	(uint8_t)(0x02)             // Reset Bit
#define AW20108_P0_REG_GCCR                        	(uint8_t)(0x03)             // Reset Bit
#define AW20108_P0_REG_FCS	                        (uint8_t)(0x04)             // Reset Bit
#define AW20108_P0_REG_CLKSYS                       (uint8_t)(0x05)             // Reset Bit
#define AW20108_P0_REG_FLTCFG1                      (uint8_t)(0x09)             // Reset Bit
#define AW20108_P0_REG_FLTCFG2                      (uint8_t)(0x0A)             // Reset Bit
#define AW20108_P0_REG_ISRFLT                       (uint8_t)(0x0B)             // Reset Bit
#define AW20108_P0_REG_LEDON(x)                     (uint8_t)(0x31+x)           // Reset Bit
#define AW20108_P0_REG_PATCR                      	(uint8_t)(0x43)             // Reset Bit
#define AW20108_P0_REG_FADEH0                      	(uint8_t)(0x44)             // Reset Bit
#define AW20108_P0_REG_FADEH1                      	(uint8_t)(0x45)             // Reset Bit
#define AW20108_P0_REG_FADEH2                      	(uint8_t)(0x46)             // Reset Bit
#define AW20108_P0_REG_FADEL0                      	(uint8_t)(0x47)             // Reset Bit
#define AW20108_P0_REG_FADEL1                      	(uint8_t)(0x48)             // Reset Bit
#define AW20108_P0_REG_FADEL2                      	(uint8_t)(0x49)             // Reset Bit
#define AW20108_P0_REG_FAT0T0                      	(uint8_t)(0x4A)             // Reset Bit
#define AW20108_P0_REG_FAT0T1                      	(uint8_t)(0x4B)             // Reset Bit
#define AW20108_P0_REG_FAT0T2                      	(uint8_t)(0x4C)             // Reset Bit
#define AW20108_P0_REG_FAT0T3                      	(uint8_t)(0x4D)             // Reset Bit
#define AW20108_P0_REG_FAT1T0                      	(uint8_t)(0x4E)             // Reset Bit
#define AW20108_P0_REG_FAT1T1                      	(uint8_t)(0x4F)             // Reset Bit
#define AW20108_P0_REG_FAT1T2                      	(uint8_t)(0x50)             // Reset Bit
#define AW20108_P0_REG_FAT1T3                      	(uint8_t)(0x51)             // Reset Bit
#define AW20108_P0_REG_FAT2T0                      	(uint8_t)(0x52)             // Reset Bit
#define AW20108_P0_REG_FAT2T1                      	(uint8_t)(0x53)             // Reset Bit
#define AW20108_P0_REG_FAT2T2                      	(uint8_t)(0x54)             // Reset Bit
#define AW20108_P0_REG_FAT2T3                      	(uint8_t)(0x55)             // Reset Bit
#define AW20108_P0_REG_PAT0CFG                      (uint8_t)(0x56)             // Reset Bit
#define AW20108_P0_REG_PAT1CFG                      (uint8_t)(0x57)             // Reset Bit
#define AW20108_P0_REG_PAT2CFG                      (uint8_t)(0x58)             // Reset Bit
#define AW20108_P0_REG_PATGO                      	(uint8_t)(0x59)             // Reset Bit
#define AW20108_P0_REG_SIZE                      		(uint8_t)(0x80)             // Reset Bit

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_SLPCR
#define AW20108_SLPCR(x)                   					(uint8_t)((x & 0x01) << 8)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_RSTR
#define AW20108_RSTR_SW_RSTN(x)                   	(uint8_t)((x & 0x01) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// Global current configuration: AW20108_P0_REG_GCCR
#define AW20108_GCCR_IMAX(x)                       	(uint8_t)((x & 0x0F) << 4)       // Conversion Ready
#define AW20108_GCCR_ALLON(x)                       (uint8_t)((x & 0x01) << 3)       // Math Overflow Flag
#define AW20108_GCCR_EXPEN(x)                       (uint8_t)((x & 0x01) << 0)       // Math Overflow Flag

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_FCS
#define AW20108_FCS_FCDE(x)                   			(uint8_t)((x & 0x01) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_CLKSYS
#define AW20108_CLKSYS(x)                   				(uint8_t)((x & 0x03) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_ISRFLT
#define AW20108_ISRFLT_PATxIS(x)                   	(uint8_t)((x & 0x07) << 4)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_LEDON
#define AW20108_LEDON_ON(x)                   			(uint8_t)((x & 0x3F) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_PATCR
#define AW20108_PATCR_PATxEN(x)                   	(uint8_t)((x & 0x07) << 0)       // Conversion Ready
#define AW20108_PATCR_PATxIE(x)                   	(uint8_t)((x & 0x07) << 4)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_FADEH
#define AW20108_FADEH0(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready
#define AW20108_FADEH1(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready
#define AW20108_FADEH2(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_FADEL
#define AW20108_FADEL0(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready
#define AW20108_FADEL1(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready
#define AW20108_FADEL2(x)                   				(uint8_t)((x & 0xFF) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_FATxT0
#define AW20108_FATxT0_T1(x)                   			(uint8_t)((x & 0x0F) << 4)       // Conversion Ready
#define AW20108_FATxT0_T2(x)                   			(uint8_t)((x & 0x0F) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_FATxT1
#define AW20108_FATxT1_T3(x)                   			(uint8_t)((x & 0x0F) << 4)       // Conversion Ready
#define AW20108_FATxT1_T4(x)                   			(uint8_t)((x & 0x0F) << 0)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_PATxCFG
#define AW20108_PATxCFG_PATMD(x)                   	(uint8_t)((x & 0x01) << 0)       // Conversion Ready
#define AW20108_PATxCFG_RAMPE(x)                   	(uint8_t)((x & 0x01) << 1)       // Conversion Ready
#define AW20108_PATxCFG_SWITCH(x)                   (uint8_t)((x & 0x01) << 2)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// AW20108_P0_REG_PATGO
#define AW20108_PATGO_PAT0ST(x)                   	(uint8_t)((x & 0x01) << 4)       // Conversion Ready
#define AW20108_PATGO_PAT1ST(x)                   	(uint8_t)((x & 0x01) << 5)       // Conversion Ready
#define AW20108_PATGO_PAT2ST(x)                   	(uint8_t)((x & 0x01) << 6)       // Conversion Ready
#define AW20108_PATGO_RUN0(x)                   		(uint8_t)((x & 0x01) << 0)       // Conversion Ready
#define AW20108_PATGO_RUN1(x)                   		(uint8_t)((x & 0x01) << 1)       // Conversion Ready
#define AW20108_PATGO_RUN2(x)                   		(uint8_t)((x & 0x01) << 2)       // Conversion Ready

/*----------------------------------------------------------------------------*/
// Page1 Register Address
#define AW20108_P1_REG_DIM(x)		                    (uint8_t)(x & 0x7F)            // Reset Bit

/*----------------------------------------------------------------------------*/
// Page2 Register Address
#define AW20108_P2_REG_FADE(x)		                  (uint8_t)(x & 0x7F)            // Reset Bit

/*----------------------------------------------------------------------------*/
// Page3 Register Address
#define AW20108_P3_REG_PAT(x)		                    (uint8_t)(x & 0x7F)            // Reset Bit

/*----------------------------------------------------------------------------*/
// Page4 Register Address
#define AW20108_P4_REG_DIM(x)		                    (uint8_t)((2*x) & 0xD7)            // Reset Bit
#define AW20108_P4_REG_FADE(x)		                  (uint8_t)((2*x+1) & 0xD7)            // Reset Bit

/*----------------------------------------------------------------------------*/
// Page5 Register Address
#define AW20108_P5_REG_PAT_DIM(x)		                (uint8_t)((2*x) & 0xD7)            // Reset Bit
#define AW20108_P5_REG_FADE(x)		                  (uint8_t)((2*x+1) & 0xD7)            // Reset Bit

//AW20108_SLPCR_SLEEP(x)
#define AW20108_SLPCR_MASK                   				AW20108_SLPCR(1)       				// Conversion Ready
#define AW20108_SLPCR_ACTIVE                 				AW20108_SLPCR(0)       				// Conversion Ready
#define AW20108_SLPCR_SLEEP                 				AW20108_SLPCR(1)       				// Conversion Ready

//AW20108_RSTR_SW_RSTN(x)
#define AW20108_RSTR_SW_RSTN_MASK                   AW20108_RSTR_SW_RSTN(0)       				// Conversion Ready
#define AW20108_RSTR_SW_RSTN_RESET                 	AW20108_RSTR_SW_RSTN(1)       				// Conversion Ready

//AW20108_GCCR_IMAX(x)
#define AW20108_GCCR_IMAX_MASK             						AW20108_GCCR_IMAX(0)       // Bus Voltage Range Mask
#define AW20108_GCCR_IMAX_10MA             				 		AW20108_GCCR_IMAX(0)       // 0-16V Range
#define AW20108_GCCR_IMAX_20MA              					AW20108_GCCR_IMAX(1)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_30MA              					AW20108_GCCR_IMAX(2)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_40MA              					AW20108_GCCR_IMAX(3)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_60MA              					AW20108_GCCR_IMAX(4)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_80MA              					AW20108_GCCR_IMAX(5)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_120MA              					AW20108_GCCR_IMAX(6)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_160MA              					AW20108_GCCR_IMAX(7)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_3D3MA              					AW20108_GCCR_IMAX(8)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_6D7MA              					AW20108_GCCR_IMAX(9)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_13D3MA              				AW20108_GCCR_IMAX(11)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_26D7MA              				AW20108_GCCR_IMAX(13)       // 0-32V Range (default)
#define AW20108_GCCR_IMAX_53D3MA              				AW20108_GCCR_IMAX(15)       // 0-32V Range (default)

//AW20108_GCCR_ALLON(x)
#define AW20108_GCCR_ALLON_MASK             					AW20108_GCCR_ALLON(0)        // Shunt Voltage Range Mask
#define AW20108_GCCR_ALLON_ENABLE             				AW20108_GCCR_ALLON(1)        // Gain 1, 40mV Range
#define AW20108_GCCR_ALLON_DISABLE             				AW20108_GCCR_ALLON(0)        // Gain 2, 80mV Range

//AW20108_GCCR_EXPEN(x)
#define AW20108_GCCR_EXPEN_MASK             					AW20108_GCCR_EXPEN(0)        // Shunt Voltage Range Mask
#define AW20108_GCCR_EXPEN_ENABLE             				AW20108_GCCR_EXPEN(0)        // Gain 1, 40mV Range
#define AW20108_GCCR_EXPEN_DISABLE             				AW20108_GCCR_EXPEN(1)        // Gain 2, 80mV Range

//AW20108_FCS_FCDE(x)
#define AW20108_FCS_FCDE_MASK                   		AW20108_FCS_FCDE(0)       				// Conversion Ready
#define AW20108_FCS_FCDE_ENABLE                   	AW20108_FCS_FCDE(1)       				// Conversion Ready

//AW20108_CLKSYS(x)
#define AW20108_CLKSYS_MASK                   			AW20108_FCS_FCDE(0)       				// Conversion Ready
#define AW20108_CLKSYS_INTERNAL_CLK               	AW20108_FCS_FCDE(0)       				// Conversion Ready
#define AW20108_CLKSYS_SLAVE               					AW20108_FCS_FCDE(1)       				// Conversion Ready
#define AW20108_CLKSYS_MASTER               				AW20108_FCS_FCDE(2)       				// Conversion Ready
#define AW20108_CLKSYS_FORBIDDEN               			AW20108_FCS_FCDE(3)       				// Conversion Ready

//AW20108_ISRFLT_PATxIS(x)
#define AW20108_ISRFLT_PATxIS_MASK                  AW20108_ISRFLT_PATxIS(0)       				// Conversion Ready
#define AW20108_ISRFLT_PATxIS_PAT0IS               	AW20108_ISRFLT_PATxIS(1)       				// Conversion Ready
#define AW20108_ISRFLT_PATxIS_PAT1IS               	AW20108_ISRFLT_PATxIS(2)       				// Conversion Ready
#define AW20108_ISRFLT_PATxIS_PAT2IS               	AW20108_ISRFLT_PATxIS(4)       				// Conversion Ready

//AW20108_ISRFLT_PATxIS(x)
#define AW20108_ISRFLT_PATxIS_MASK                  AW20108_ISRFLT_PATxIS(0)       				// Conversion Ready
#define AW20108_ISRFLT_PAT0IS               				AW20108_ISRFLT_PATxIS(1)       				// Conversion Ready
#define AW20108_ISRFLT_PAT1IS               				AW20108_ISRFLT_PATxIS(2)       				// Conversion Ready
#define AW20108_ISRFLT_PAT2IS               				AW20108_ISRFLT_PATxIS(4)       				// Conversion Ready

//AW20108_PATCR_PATxEN(x)
#define AW20108_PATCR_PATxEN_MASK                  	AW20108_PATCR_PATxEN(0)       				// Conversion Ready
#define AW20108_PATCR_PAT0EN               					AW20108_PATCR_PATxEN(1)       				// Conversion Ready
#define AW20108_PATCR_PAT1EN               					AW20108_PATCR_PATxEN(2)       				// Conversion Ready
#define AW20108_PATCR_PAT2EN               					AW20108_PATCR_PATxEN(4)       				// Conversion Ready

//AW20108_PATCR_PATxIE(x)
#define AW20108_PATCR_PATxIE_MASK                  	AW20108_PATCR_PATxIE(0)       				// Conversion Ready
#define AW20108_PATCR_PAT0IE               					AW20108_PATCR_PATxIE(1)       				// Conversion Ready
#define AW20108_PATCR_PAT1IE               					AW20108_PATCR_PATxIE(2)       				// Conversion Ready
#define AW20108_PATCR_PAT2IE               					AW20108_PATCR_PATxIE(4)       				// Conversion Ready

//AW20108_FATxT0_T1(x)
#define AW20108_FATxT0_T1_MASK             					AW20108_FATxT0_T1(0)       // Bus Voltage Range Mask
#define AW20108_FATxT0_T1_0MS             				 	AW20108_FATxT0_T1(0)       // 0-16V Range
#define AW20108_FATxT0_T1_130MS              				AW20108_FATxT0_T1(1)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_260MS              				AW20108_FATxT0_T1(2)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_380MS              				AW20108_FATxT0_T1(3)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_510MS              				AW20108_FATxT0_T1(4)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_770MS              				AW20108_FATxT0_T1(5)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_1040MS              			AW20108_FATxT0_T1(6)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_1600MS              			AW20108_FATxT0_T1(7)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_2100MS              			AW20108_FATxT0_T1(8)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_2600MS              			AW20108_FATxT0_T1(9)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_3100MS              			AW20108_FATxT0_T1(10)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_4200MS              			AW20108_FATxT0_T1(11)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_5200MS              			AW20108_FATxT0_T1(12)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_6200MS              			AW20108_FATxT0_T1(13)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_7300MS              			AW20108_FATxT0_T1(14)       // 0-32V Range (default)
#define AW20108_FATxT0_T1_8300MS              			AW20108_FATxT0_T1(15)       // 0-32V Range (default)

//AW20108_FATxT0_T2(x)
#define AW20108_FATxT0_T2_MASK             					AW20108_FATxT0_T2(0)       // Bus Voltage Range Mask
#define AW20108_FATxT0_T2_40MS             				 	AW20108_FATxT0_T2(0)       // 0-16V Range
#define AW20108_FATxT0_T2_130MS              				AW20108_FATxT0_T2(1)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_260MS              				AW20108_FATxT0_T2(2)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_380MS              				AW20108_FATxT0_T2(3)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_510MS              				AW20108_FATxT0_T2(4)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_770MS              				AW20108_FATxT0_T2(5)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_1040MS              			AW20108_FATxT0_T2(6)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_1600MS              			AW20108_FATxT0_T2(7)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_2100MS              			AW20108_FATxT0_T2(8)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_2600MS              			AW20108_FATxT0_T2(9)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_3100MS              			AW20108_FATxT0_T2(10)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_4200MS              			AW20108_FATxT0_T2(11)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_5200MS              			AW20108_FATxT0_T2(12)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_6200MS              			AW20108_FATxT0_T2(13)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_7300MS              			AW20108_FATxT0_T2(14)       // 0-32V Range (default)
#define AW20108_FATxT0_T2_8300MS              			AW20108_FATxT0_T2(15)       // 0-32V Range (default)

//AW20108_FATxT1_T3(x)
#define AW20108_FATxT1_T3_MASK             					AW20108_FATxT1_T3(0)       // Bus Voltage Range Mask
#define AW20108_FATxT1_T3_0MS             				 	AW20108_FATxT1_T3(0)       // 0-16V Range
#define AW20108_FATxT1_T3_130MS              				AW20108_FATxT1_T3(1)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_260MS              				AW20108_FATxT1_T3(2)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_380MS              				AW20108_FATxT1_T3(3)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_510MS              				AW20108_FATxT1_T3(4)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_770MS              				AW20108_FATxT1_T3(5)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_1040MS              			AW20108_FATxT1_T3(6)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_1600MS              			AW20108_FATxT1_T3(7)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_2100MS              			AW20108_FATxT1_T3(8)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_2600MS              			AW20108_FATxT1_T3(9)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_3100MS              			AW20108_FATxT1_T3(10)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_4200MS              			AW20108_FATxT1_T3(11)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_5200MS              			AW20108_FATxT1_T3(12)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_6200MS              			AW20108_FATxT1_T3(13)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_7300MS              			AW20108_FATxT1_T3(14)       // 0-32V Range (default)
#define AW20108_FATxT1_T3_8300MS              			AW20108_FATxT1_T3(15)       // 0-32V Range (default)

//AW20108_FATxT1_T4(x)
#define AW20108_FATxT1_T4_MASK             					AW20108_FATxT1_T4(0)       // Bus Voltage Range Mask
#define AW20108_FATxT1_T4_40MS             				 	AW20108_FATxT1_T4(0)       // 0-16V Range
#define AW20108_FATxT1_T4_130MS              				AW20108_FATxT1_T4(1)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_260MS              				AW20108_FATxT1_T4(2)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_380MS              				AW20108_FATxT1_T4(3)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_510MS              				AW20108_FATxT1_T4(4)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_770MS              				AW20108_FATxT1_T4(5)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_1040MS              			AW20108_FATxT1_T4(6)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_1600MS              			AW20108_FATxT1_T4(7)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_2100MS              			AW20108_FATxT1_T4(8)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_2600MS              			AW20108_FATxT1_T4(9)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_3100MS              			AW20108_FATxT1_T4(10)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_4200MS              			AW20108_FATxT1_T4(11)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_5200MS              			AW20108_FATxT1_T4(12)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_6200MS              			AW20108_FATxT1_T4(13)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_7300MS              			AW20108_FATxT1_T4(14)       // 0-32V Range (default)
#define AW20108_FATxT1_T4_8300MS              			AW20108_FATxT1_T4(15)       // 0-32V Range (default)

//AW20108_PATxCFG_PATMD(x)
#define AW20108_PATMD_MASK             							AW20108_PATxCFG_PATMD(0)       // Bus Voltage Range Mask
#define AW20108_PATMD_MANUAL             						AW20108_PATxCFG_PATMD(0)       // Bus Voltage Range Mask
#define AW20108_PATMD_AUTO             							AW20108_PATxCFG_PATMD(1)       // Bus Voltage Range Mask

//AW20108_PATxCFG_RAMPE(x)
#define AW20108_RAMPE_MASK             							AW20108_PATxCFG_RAMPE(0)       // Bus Voltage Range Mask
#define AW20108_RAMPE_DISABLE             					AW20108_PATxCFG_RAMPE(0)       // Bus Voltage Range Mask
#define AW20108_RAMPE_ENABLE             						AW20108_PATxCFG_RAMPE(1)       // Bus Voltage Range Mask

//AW20108_PATxCFG_SWITCH(x)
#define AW20108_SWITCH_MASK             						AW20108_PATxCFG_SWITCH(0)       // Bus Voltage Range Mask
#define AW20108_SWITCH_LED_ON             					AW20108_PATxCFG_SWITCH(0)       // Bus Voltage Range Mask
#define AW20108_SWITCH_LED_OFF             					AW20108_PATxCFG_SWITCH(1)       // Bus Voltage Range Mask

//AW20108_PATGO_PAT0ST(x)
#define AW20108_PAT0ST_MASK             						AW20108_PATGO_PAT0ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT0ST_NOT_RUNNING             					AW20108_PATGO_PAT0ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT0ST_RUNNING           					AW20108_PATGO_PAT0ST(1)       // Bus Voltage Range Mask

//AW20108_PATGO_PAT1ST(x)
#define AW20108_PAT1ST_MASK             						AW20108_PATGO_PAT1ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT1ST_NOT_RUNNING             					AW20108_PATGO_PAT1ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT1ST_RUNNING           					AW20108_PATGO_PAT1ST(1)       // Bus Voltage Range Mask

//AW20108_PATGO_PAT2ST(x)
#define AW20108_PAT2ST_MASK             						AW20108_PATGO_PAT2ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT2ST_NOT_RUNNING             					AW20108_PATGO_PAT2ST(0)       // Bus Voltage Range Mask
#define AW20108_PAT2ST_RUNNING           					AW20108_PATGO_PAT2ST(1)       // Bus Voltage Range Mask

//AW20108_PATGO_RUN0(x)
#define AW20108_RUN0_MASK             						AW20108_PATGO_RUN0(0)       // Bus Voltage Range Mask
#define AW20108_RUN0_DISABLE             					AW20108_PATGO_RUN0(0)       // Bus Voltage Range Mask
#define AW20108_RUN0_ENABLE           					AW20108_PATGO_RUN0(1)       // Bus Voltage Range Mask

//AW20108_PATGO_RUN1(x)
#define AW20108_RUN1_MASK             						AW20108_PATGO_RUN1(0)       // Bus Voltage Range Mask
#define AW20108_RUN1_DISABLE             					AW20108_PATGO_RUN1(0)       // Bus Voltage Range Mask
#define AW20108_RUN1_ENABLE           					AW20108_PATGO_RUN1(1)       // Bus Voltage Range Mask

//AW20108_PATGO_RUN2(x)
#define AW20108_RUN2_MASK             						AW20108_PATGO_RUN2(0)       // Bus Voltage Range Mask
#define AW20108_RUN2_DISABLE             					AW20108_PATGO_RUN2(0)       // Bus Voltage Range Mask
#define AW20108_RUN2_ENABLE           					AW20108_PATGO_RUN2(1)       // Bus Voltage Range Mask

typedef struct
{
	uint8_t		DevAddr;
	uint8_t 	IMax;  	
	uint8_t		AllOn;								
	uint8_t 	ClkMode;							
	uint8_t 	Size;
	uint8_t 	(*Read)(uint8_t devAddr, uint8_t subAddr);
  uint8_t 	(*Write)(uint8_t devAddr, uint8_t subAddr, uint8_t val);
}AW20108_InitTypeDef;

typedef struct
{
	AW20108_InitTypeDef Init;
	uint8_t 						CurrentPage;
	uint8_t 						LedxOn;
	
}AW20108_HandleTypeDef;

extern int 			AW20108_Init			(AW20108_HandleTypeDef *pAw20108);
extern int 			AW20108_SetPage		(AW20108_HandleTypeDef *pAw20108, uint8_t page);
extern int 			AW20108_SetDIMx		(AW20108_HandleTypeDef *pAw20108, uint8_t dim);
extern int 			AW20108_SetFADEx	(AW20108_HandleTypeDef *pAw20108, uint8_t fade);
extern int 			AW20108_SetFATx		(AW20108_HandleTypeDef *pAw20108, uint8_t pat);
extern int 			AW20108_LEDxOn		(AW20108_HandleTypeDef *pAw20108, uint8_t ledxReg, uint8_t ledx);
extern uint8_t 	AW20108_ReadDevID	(AW20108_HandleTypeDef *pAw20108);
#endif

