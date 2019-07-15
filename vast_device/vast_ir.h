

#ifndef		__INFRA_RED_H__   
#define		__INFRA_RED_H__ 

#include "stm32f4xx_hal.h"


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

	IRType_MAX,
}IRType_e;

typedef enum _IR_PIN_State
{
	IR_PIN_LOW = 0,
	IR_PIN_HIGH
}IR_PIN_State;

typedef struct _IR_DataTypeDef
{
	uint16_t address;
	uint8_t command;
	uint8_t command_check;
}IR_DataTypeDef;

typedef struct _IR_BufTypeDef
{
	uint16_t timer;
	IR_PIN_State pin_state;
}IR_BufTypeDef;


#define	CAPTURE_STAT_IDLE 							(0)
#define	CAPTURE_STAT_CAPTURE_HEAD_FLAG	(1<<6)
#define CAPTURE_STAT_CAPTURE_DONE				(1<<7)
#define CAPTURE_STAT_CAPTURE_TIMES			(0x0f)

typedef struct _IR_TypeDef
{
	__IO uint8_t state;   // bit7: ������ɱ�־��bit6���Ƿ񲶻񵽸ߵ�ƽ��bit5�����񵽸ߵ�ƽ֮����½��ر�־��bit4~0������ߵ�ƽ��ʱ������Ĵ���
	__IO IR_BufTypeDef rx_buf[100];
	__IO uint8_t len;
	IR_DataTypeDef value;
	uint16_t carry_freq;
	const IR_BufTypeDef *pHead;
	uint8_t protocol_size;
	uint8_t RepeatInterval;  //ms
	IRType_e IRType;
	int (*pInfraRed_RX_Calculate)(struct _IR_TypeDef *);
}IR_TypeDef;


#define	IR_RX_PORT			GPIOB
#define IR_RX_PIN				GPIO_PIN_10
//#define	IR_RX_GET_PIN()	IR_RX_PIN
//#define	IR_RX_OFF()			SETB(LED0_PORT,LED0_PIN)
#define	IR_RX_GET_PIN_STATE()	HAL_GPIO_ReadPin(IR_RX_PORT, IR_RX_PIN)
//#define	IR_RX_OFF()			SETB(LED0_PORT,LED0_PIN)

extern IR_TypeDef IR_Obj;

extern int InfraRed_RX_Init(void);
extern int InfraRed_RX_Calculate(void);
extern int InfraRed_RX_ChangeProtocol(IRType_e IRType);



#endif /* __INFRA_RED_H__ */
