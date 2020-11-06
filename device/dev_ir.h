/**
 ******************************************************************************
 * @file    demo.h
 * @author  chenzhipeng3472
 * @version V1.0.0
 * @date    10-sep-2018
 * @brief   
 ******************************************************************************
 * @attention
 * 
 ******************************************************************************
 */
    
/** @addtogroup Project
  * @{
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef		__DEV_IR_H__
#define		__DEV_IR_H__

#include "vast_common.h"
#ifdef VAST_USING_IR

#define     INFRARED_BUFF_SIZE      256

typedef enum _IRType_e  //don't change sequence, add after last, before IRType_MAX
{
	IRType_NONE = 0,
	IRType_NEC,
	IRType_RC5,// PHILIPS
	IRType_RC6,
	IRType_RCA,// THOMSON
	IRType_JVC,
	IRType_TRC,
	IRType_SONY,
	IRType_SHARP,
	IRType_MIT,
	IRType_KONK,
	IRType_MI,
	IRType_NEC_WB,
	IRType_TOP,
	IRType_SAMSUNG,
	IRType_PANASONIC,
	IRType_TOSHIBA,
	IRType_KONKA,

	IRType_MAX,
}IRType_e;

typedef enum _IR_PIN_State
{
	IR_PIN_LOW = 0,
	IR_PIN_HIGH
}IR_PIN_State;

struct data_nec
{
    uint16_t addr;
    uint8_t key;
    uint8_t repeat;
};

struct data_mi
{
    uint8_t addr;
    uint8_t key;
    uint8_t repeat;
};

typedef union data_ir
{
    struct data_nec   nec;          /* Temperature.         unit: dCelsius    */
    struct data_mi    mi;          /* Temperature.         unit: dCelsius    */
}data_ir_t;

typedef struct _IR_DataTypeDef
{
	uint16_t address;
	uint8_t command;
	uint8_t command_check;
}IR_DataTypeDef;

typedef struct _IR_BufTypeDef
{
    uint16_t us;
    IR_PIN_State level;
}IR_BufTypeDef;


//#define	CAPTURE_STAT_IDLE 						(0)
//#define	CAPTURE_STAT_CAPTURE_HEAD_FLAG			(1<<6)
//#define CAPTURE_STAT_CAPTURE_DONE				(1<<7)
//#define CAPTURE_STAT_CAPTURE_TIMES				(0x0f)

typedef struct _IR_TypeDef
{
	//volatile uint8_t state;   // bit7: capture complete，bit6：is_capture_high, bit5:falling flag after high level, bit4~0: hight_level us overflow times
	volatile IR_BufTypeDef rx_buf[INFRARED_BUFF_SIZE];
	volatile IR_BufTypeDef tx_buf[INFRARED_BUFF_SIZE];
	volatile uint8_t tx_bufIdx;
	volatile uint8_t tx_bufLen;
	volatile uint8_t rx_len;
	IR_DataTypeDef value;
    data_ir_t data;
	uint16_t carry_freq;
	const IR_BufTypeDef *pHead;
	uint8_t idle;
	uint8_t protocol_size;
	uint8_t RepeatInterval;  //ms
	uint8_t RepeatCnt;  //ms
	uint8_t RxRepeat;
	IRType_e IRType;
	int (*pInfraRed_RX_Decoder)(struct _IR_TypeDef *);
	int (*pInfraRed_TX_Encoder)(struct _IR_TypeDef *, const void *buffer, size_t size);
	int (*pInfraRed_TX_RepeatEncoder)(struct _IR_TypeDef *, const void *buffer, size_t size);
}IR_TypeDef;


//#define	IR_RX_PORT			GPIOB
//#define IR_RX_PIN				GPIO_PIN_10
//#define	IR_RX_GET_PIN()	IR_RX_PIN
//#define	IR_RX_OFF()			SETB(LED0_PORT,LED0_PIN)
//#define	IR_RX_GET_PIN_STATE()	HAL_GPIO_ReadPin(IR_RX_PORT, IR_RX_PIN)
//#define	IR_RX_OFF()			SETB(LED0_PORT,LED0_PIN)

extern IR_TypeDef IR_Obj;

extern int InfraRed_NEC_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_SONY_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_SAMSUNG_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_PANASONIC_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RC5_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RC6_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_RCA_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_MI_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_TOSHIBA_Init(IR_TypeDef *pIR_Obj);
extern int InfraRed_KONKA_Init(IR_TypeDef *pIR_Obj);

extern int InfraRed_Init(IRType_e IRType);
extern int InfraRed_RX_Decoder(void);
extern int InfraRed_TX_Encoder(struct _IR_TypeDef *, const void *buffer, size_t size);
extern int InfraRed_TX_RepeatEncoder(struct _IR_TypeDef *pIr, const void *buffer, size_t size);
//extern int InfraRed_RX_ChangeProtocol(IRType_e IRType);
extern void ir_rx_irq_callback(uint32_t cnt, IR_PIN_State level);
extern int ir_tx_push_data(const IR_BufTypeDef *pData, uint32_t len);


#endif /* VAST_USING_IR */

#endif /* __DEV_IR_H__ */
