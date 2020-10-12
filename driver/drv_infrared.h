
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
#ifndef __DRV_INFRARED_H__
#define __DRV_INFRARED_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "vast_common.h"
#include "core/device.h"
#include "core/croutine.h"

/***********************************************
                    define
***********************************************/
#define     IR_RX_SIZE      256
#define     IR_TX_SIZE      256

/***********************************************
                    typedef
***********************************************/
typedef struct _ir_data_t
{
    uint16_t level : 1,
             us    : 15;
}ir_data_t;

struct _infrared_device_t;

typedef struct _infrared_init_t
{
}infrared_init_t;

typedef struct _infrared_ops_t
{
	error_t	(*init)		(struct _infrared_device_t *infrared, infrared_init_t *args);
	error_t	(*decode)	(struct _infrared_device_t *infrared, ir_data_t *data, uint16_t len);
    error_t (*ctrl)     (struct _infrared_device_t *infrared, uint8_t cmd, void *args);
    error_t (*tx_start) (struct _infrared_device_t *infrared);
    size_t  (*send)     (struct _ir_data_t *data, size_t size);
}infrared_ops_t;

typedef struct _infrared_device_t
{
	struct device 		    parent;
 	const infrared_ops_t    *ops;
	ir_data_t               rx_buf[IR_RX_SIZE];
	ir_data_t               tx_buf[IR_TX_SIZE];
    uint16_t                rx_len;
    uint16_t                tx_len;
    tcb_t                   tcb;
    struct soft_timer       tmr;
}infrared_device_t;

/***********************************************
               function prototypes
***********************************************/
error_t infrared_device_register(infrared_device_t *infrared, 
                                    const char *name, uint32_t flag, void *data);
error_t infrared_isr_handler(infrared_device_t *infrared, uint8_t level, uint16_t us);

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __DRV_INFRARED_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/


