/**
 ******************************************************************************
 * @file    xxx.h
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

/* Define to prevent recursive inclusion */
#ifndef __VAST_LED_H__
#define __VAST_LED_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include <core/device.h>

/***********************************************
                    define
***********************************************/

/***********************************************
                    typedef
***********************************************/
 struct _led_device_t;

 typedef struct _led_ops_t
 {
	error_t (*on)		(struct _led_device_t *);
	error_t (*off)	(struct _led_device_t *);
	error_t (*toggle)	(struct _led_device_t *);
 }led_ops_t;

 typedef struct _led_device_t
 {
 	device_t 			parent;
 	const led_ops_t  	*ops;
 	struct soft_timer 	tmr;
 	uint8_t				status:1;
 	uint8_t				flash_cnt:3;
 	uint8_t				mode:4;

 }led_device_t;

/***********************************************
               function prototypes
***********************************************/
error_t led_device_register(led_device_t *led, const char *name, uint32_t flag, void *data);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __VAST_LED_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


