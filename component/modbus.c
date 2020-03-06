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
#include "component/modbus.h"
#include <core/utils.h>
#include <core/device.h>

#define LOG_TAG    "modbus"
#include <elog.h>

/***********************************************
                    define
***********************************************/
#ifdef configUSING_MODBUS

/***********************************************
                    typedef
***********************************************/
enum
{
    STATE_IDLE = 1,
	STATE_EXEC,
};

/***********************************************
               function prototypes
***********************************************/

/***********************************************
                   variable
***********************************************/

/***********************************************
                   function
***********************************************/
static unsigned short CheckCRC(unsigned char *pData, unsigned int siLen)
{
	if (NULL == pData || siLen <= 0) {
		return 0;
	}

	unsigned short u16CRC = 0xFFFF;
	for (int i = 0; i < siLen; i++) {
		u16CRC ^= (unsigned short)(pData[i]);
		for(int j = 0; j <= 7; j++) {
			if (u16CRC & 0x0001) {
				u16CRC = (u16CRC >> 1) ^ 0xA001;
			} else {
				u16CRC = u16CRC >> 1;
			}
		}
	}

	unsigned short siRet = 0;
	siRet = (u16CRC & 0x00FF) << 8;
	siRet |= u16CRC >> 8;

	return siRet;
}

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int readInputReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt)
{
    int ret = 0;
    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeMultiHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_cnt, uint16_t *data)
{
    int ret = 0;
    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeSingleHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t reg_addr, uint16_t reg_val)
{
    int ret = 0;
    return ret;
}

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t start_addr, uint16_t reg_cnt, rx_indicate rx_ind)
{
    int ret = 0;
	uint16_t crc = 0;

	smart_frame_t *smartframe = (smart_frame_t *)malloc(sizeof(smart_frame_t)+(reg_cnt * sizeof(reg_cnt)) + sizeof(crc));;

    if (smartframe == NULL) {
        ret = 1;
        return ret;
    }

	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

	modbus->slave_addr = modbusframe->slave_addr = slave_addr;
	modbusframe->opcode = READ_HOLD_RES;
	modbusframe->start_addr = le16_to_cpu(start_addr);
	modbusframe->reg_cnt = le16_to_cpu(reg_cnt);
	smartframe->len = MODBUS_FRAME_HEAD + 0;

	crc = CheckCRC((uint8_t *)modbusframe, smartframe->len);
	modbusframe->data[0] = le16_to_cpu(crc);

	smartframe->len = smartframe->len + sizeof(crc);

	smartframe->rx_ind = rx_ind;

	list_add(&(smartframe->entry), &modbus->tx_list);

	if (is_sm_state(&modbus->sm, STATE_IDLE)) {
		modbus->sm.buffer = (void *)smartframe;
		smartframe = (smart_frame_t *)modbus->rx_data;
		smartframe->rx_ind = rx_ind;
		state_machine_change(&modbus->sm, STATE_EXEC);
	}

    return ret;
}

/*
 * do idle
 */
static base_t do_execute0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
//    log_d("%s", __FUNCTION__);

	smart_frame_t *smartframe = (smart_frame_t *)out;
    return smartframe->len;
}
static base_t do_execute1(state_machine_t *sm, const void *arg)
{
//    log_d("%s", __FUNCTION__);
    if (sm->trycnt > 3) {   /* try max */
        state_machine_change(sm, STATE_IDLE);
        return STATE_CTR_ERROR;
    } else if (!arg) {   	/* time out */
        return STATE_CTR_RETRY;
    }

	modbus_t *modbus = container_of(sm, modbus_t, sm);
	device_t *dev = ((device_t *)modbus);
	smart_frame_t *smartframe = (smart_frame_t *)arg;

	smartframe->rx_ind(dev, smartframe->len);

	smartframe = list_last_entry(&modbus->tx_list, smart_frame_t, entry);
	list_del(&smartframe->entry);
	free(smartframe);

	if (!list_empty(&modbus->tx_list)) {
		smartframe = list_last_entry(&modbus->tx_list, smart_frame_t, entry);
		modbus->sm.buffer = (void *)smartframe;

		smart_frame_t *smartframe_rx = (smart_frame_t *)modbus->rx_data;
		smartframe_rx->rx_ind = smartframe->rx_ind;
		state_machine_change(&modbus->sm, STATE_EXEC);
	    return STATE_CTR_CHAGE;
	} else {
	    return STATE_CTR_NEXT;
	}
}

static base_t do_idle0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
//    log_d("%s", __FUNCTION__);
    return 0;
}

static base_t do_idle1(state_machine_t *sm, const void *arg)
{
//    log_d("%s", __FUNCTION__);
    return STATE_CTR_WAIT;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
static const state_machine_state_op_t state_machine_states[] =
{
	state_machine_state(STATE_EXEC     , STATE_IDLE   , pdMS_TO_TICKS(2000) , do_execute)    ,
	state_machine_state(STATE_IDLE     , STATE_IDLE   , 0                   , do_idle)       ,
};

static base_t do_process0(const void *arg, uint8_t *out, size_t len)
{
	modbus_t *modbus = container_of(arg, modbus_t, sm);
	smart_frame_t *smartframe = (smart_frame_t *)out;
	device_t *dev = ((device_t *)modbus)->user_data;

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	elog_hexdump("modbus.tx", 10, smartframe->data, smartframe->len);
	device_write(dev, 0, smartframe->data, smartframe->len);

    return 0;
}

static int smart_frame_len(const smart_frame_t *frame)
{
    return SMART_FRAME_HEAD + frame->len + 1;
}

/**************************************************************************
  * @brief	   : 解析一帧报文
  * @param[in] : 
  * @param[out]: 
  * @return    : 
  * @others    : 
***************************************************************************/
modbus_frame_t * get_modbus_frame(const uint8_t * in, uint32_t len)
{
	int i = 0;
	smart_frame_t * smartframe = (smart_frame_t *) &in[i];
	modbus_frame_t * modbus_frame_t = smartframe->data;

	return modbus_frame_t;
}

static size_t check_smart_frame(const void *arg, size_t len)
{
    //state_machine_t *sm = (state_machine_t *)arg;
	modbus_t *modbus = container_of(arg, modbus_t, sm);

    modbus_frame_t *modbusframe = get_modbus_frame(modbus->rx_data, len);
    if (!modbusframe) return 0;
    //sm->data = (void *)modbusframe;

    //len = smart_frame_len(modbusframe);
    log_d("got smart frame[%d]:", len);
    elog_hexdump("prot.rx", 10, (uint8_t *)modbusframe, len);
    //len += ((uint8_t *)modbusframe - (uint8_t *)sm->data);
    //uint16_t crc = 0xffff;
	//crc = CheckCRC((uint8_t *)modbusframe, len);
    //log_d("cnt:%#X, cal_crc:%#X, get_crc:%#X", modbusframe->reg_cnt, crc, cpu_to_le16(modbusframe->data[modbusframe->reg_cnt/2]));

    return CheckCRC((uint8_t *)modbusframe, len) ? 0 : len;
}

static base_t do_process1(const void *arg)
{
	modbus_t *modbus = container_of(arg, modbus_t, sm);
    state_machine_t *sm = (state_machine_t *)arg;
	device_t *dev = ((device_t *)modbus)->user_data;

	smart_frame_t *smartframe = (smart_frame_t *)modbus->rx_data;
	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

    int len = 0;
    len = device_peek(dev, 0, modbusframe, MODBUS_RX_MAX);
//    len = smartframe->len = device_read(dev, 0, modbusframe, MODBUS_RX_MAX);
    elog_hexdump("modbus.rx", 10, (uint8_t *)modbusframe, len);
    if (len)
    {
        int ret = check_smart_frame((const void *)sm, len);

		if (ret) {
            smartframe->len = device_read(dev, 0, modbusframe, ret);
            sm->data = (void *)smartframe;
            if (len > ret)
                task_send_signal(&sm->tcb, SIG_DATA);
		} else {
            device_read(dev, 0, modbusframe, len);
		}
    }

    return 0;
}

error_t modbus_tx_done(device_t *dev, void *buffer)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    return 0;
}

error_t modbus_rx_ind(device_t *dev, size_t size)
{
    tcb_t *tcb = (tcb_t *)dev->owner;
    task_send_signal(tcb, SIG_DATA);
    return 0;
}

/**
  * @brief  ina219_init.
  * @param
  * @retval
  */
static error_t modbus_init(device_t *dev)
{
	modbus_t *modbus = (modbus_t *)dev;

    INIT_LIST_HEAD(&modbus->tx_list);

    modbus->sm.process0 = do_process0;
    modbus->sm.process1 = do_process1;
    modbus->sm.states = state_machine_states;
    modbus->sm.states_size = array_size(state_machine_states);
    modbus->sm.size_max = MODBUS_TX_MAX;
    state_machine_init(&modbus->sm);
    state_machine_change(&modbus->sm, STATE_IDLE);
	return 0;
}

/**
  * @brief  ina219_ops.
  * @param
  * @retval
  */
static const struct device_ops modbus_ops =
{
	.init = modbus_init
};

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
//error_t modbus_device_register(modbus_t *modbus, const char *name, uint32_t flag, void *data)
int modbus_device_register(void)
{
	int ret = 0;
	const char *name = "modbus";

    static modbus_t modbus;

    device_t *bus = device_find("uart4");
	device_open(bus, DEVICE_FLAG_DMA_RX | DEVICE_FLAG_DMA_TX | DEVICE_FLAG_FASYNC);

    device_set_rx_indicate(bus, modbus_rx_ind);
    device_set_tx_complete(bus, modbus_tx_done);
    device_set_owner(bus, (void *)&modbus.sm.tcb);

    device_t *dev = &modbus.parent;

    dev->ops = &modbus_ops;
    dev->user_data = (void *)bus;
    ret = device_register(dev, name, 0);
    device_open(dev, DEVICE_FLAG_DMA_RX | DEVICE_FLAG_DMA_TX | DEVICE_FLAG_FASYNC);

    return ret;
}
late_initcall(modbus_device_register);

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
error_t hold_reg_rx_ind(device_t *dev, size_t size)
{
	modbus_t *modbus = (modbus_t *)dev;
	smart_frame_t *smartframe = (smart_frame_t *)modbus->sm.data;

	elog_hexdump("modbus.rx1", 10, smartframe->data, smartframe->len);
	return 0;
}

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
error_t hold_reg_rx_ind2(device_t *dev, size_t size)
{
	modbus_t *modbus = (modbus_t *)dev;
	smart_frame_t *smartframe = (smart_frame_t *)modbus->sm.data;

	elog_hexdump("modbus.rx2", 10, smartframe->data, smartframe->len);
	return 0;
}

/**
 *   * @brief  CLICmd_Print
 *     * @param  
 *       * @retval 
 *         */
void CLICmd_SMmodbus(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    if(argc == 4) {
        device_t *dev = device_find("modbus");
        readHoldReg2((modbus_t *)dev, str2u32(argv[1]), str2u32(argv[2]), str2u32(argv[3]), hold_reg_rx_ind);
        readHoldReg2((modbus_t *)dev, str2u32(argv[1]), str2u32(argv[2]), str2u32(argv[3])+1, hold_reg_rx_ind2);
    } else {
        pCli->Init.Write("Please input: smmb <slave> <reg> <nums>.\r\n");
    }
}
CLI_CMD_EXPORT(smmb, "state machine modbus test", CLICmd_SMmodbus, 0);

/**
  * @}
  */
#endif /*configUSING_xxx*/

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




