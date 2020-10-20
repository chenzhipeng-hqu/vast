
#ifndef _QPN_DRIVER_MCU_H_
#define _QPN_DRIVER_MCU_H_

#include "qpn.h"
#include "main.h"

#define LED_GREEN_Pin        SYS_GPIO_LED(GREEN, PIN )
#define LED_GREEN_GPIO_Port  SYS_GPIO_LED(GREEN, PORT)
#define LED_RED_Pin          SYS_GPIO_LED(RED  , PIN )
#define LED_RED_GPIO_Port    SYS_GPIO_LED(RED  , PORT)

enum {
	DUMMY_SIG = Q_USER_SIG,
	LED_WORK_NORMAL_SIG,  //???????,
	LED_WORK_BUSY_SIG,		//????busy???
	LED_INIT_PROCESS_SIG,					//	?????
	LED_REV_COMMAND_CORRECT_SIG,	// ??????
	LED_REV_COMMAND_ERROR_SIG,		// ??????
	LED_COMMAND_CALLBACK_SIG,			// ????
	MAX_PUB_SIG,          /* the last published signal */

	MAX_SIG               /* the last signal */
};

void Led_ctor(void);

extern struct LedTag AO_Led;






#endif 
