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

static uint16_t write_modbus(modbus_t *modbus, list_frame_t *listframe)
{
	uint16_t crc = 0;
	smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;
	crc = CheckCRC((uint8_t *)modbusframe, smartframe->len);
	//modbusframe->data[0] = le16_to_cpu(crc);

    uint8_t *data = smartframe->data;
    //data[smartframe->len] = crc >> 8;
    //data[smartframe->len+1] = crc & 0x00ff;
    put_be_val(crc, &data[smartframe->len], 2);

	smartframe->len += sizeof(crc);
	smartframe->to = (uint32_t)((device_t *)modbus)->owner;

	list_add(&(listframe->entry), &modbus->tx_list);

	if (/*(is_sm_state(&modbus->sm, STATE_IDLE)) & */(list_is_singular(&modbus->tx_list))) {
		modbus->sm.tx_data = (void *)listframe;
        //modbus->slave_addr = modbusframe->slave_addr;
		//smartframe = (smart_frame_t *)modbus->rx_data;
		//smartframe->rx_ind = rx_ind;
        //smartframe->to = ((device_t *)modbus)->owner;
		state_machine_change(&modbus->sm, STATE_EXEC);
	}

    return 0;
}

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int readInputReg2(modbus_t *modbus, uint16_t slave_addr, 
                    uint16_t start_addr, uint16_t reg_cnt, rx_indicate rx_ind)
{
    int ret = 0;

	list_frame_t *listframe = (list_frame_t *)vast_malloc(sizeof(list_frame_t) + sizeof(smart_frame_t) + sizeof(modbus_frame_t));
    smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
    //log_d("%d %d", sizeof(smart_frame_t), sizeof(modbus_frame_t));

    if (listframe == NULL) {
        ret = 1;
        log_w("listframe is null");
        return ret;
    }

	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

	modbusframe->slave_addr = slave_addr;
	modbusframe->opcode = READ_INPUT_RES;
	modbusframe->start_addr = le16_to_cpu(start_addr);
	modbusframe->reg_cnt = le16_to_cpu(reg_cnt);
	smartframe->len = MODBUS_FRAME_HEAD + 0;
	listframe->rx_ind = rx_ind;

    write_modbus(modbus, listframe);

    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeMultiHoldReg2(modbus_t *modbus, uint16_t slave_addr, uint16_t start_addr, 
                            uint16_t reg_cnt, uint16_t *data, rx_indicate rx_ind)
{
    int ret = 0;

	list_frame_t *listframe = (list_frame_t *)vast_malloc(sizeof(list_frame_t) + sizeof(smart_frame_t) + sizeof(modbus_frame_t) + (reg_cnt * sizeof(reg_cnt)) + sizeof(reg_cnt)/2);
    smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
    //log_i("%d", sizeof(smart_frame_t)+sizeof(modbus_frame_t)+(reg_cnt*sizeof(reg_cnt)) + sizeof(reg_cnt)/2);
    //log_i("smart:%d, modbus:%d,reg_cnt:%d, %d", sizeof(smart_frame_t), sizeof(modbus_frame_t), (reg_cnt*sizeof(reg_cnt)), sizeof(reg_cnt)/2);

    if (listframe == NULL) {
        ret = 1;
        log_w("listframe is null");
        return ret;
    }

	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

	modbusframe->slave_addr = slave_addr;
	modbusframe->opcode = WRITE_MULTI_HOLD_RES;
	modbusframe->start_addr = le16_to_cpu(start_addr);
	modbusframe->reg_cnt = le16_to_cpu(reg_cnt);
	smartframe->len = MODBUS_FRAME_HEAD + reg_cnt * sizeof(reg_cnt) + sizeof(reg_cnt)/2;
	listframe->rx_ind = rx_ind;

	uint16_t * p = data;
    uint8_t *tx_data = (uint8_t *)modbusframe->data;

    int i = 0;
    tx_data[i] = reg_cnt * sizeof(uint16_t);
	for (i = 1; i < tx_data[0]; i += 2) {
		tx_data[i + 0] = (*p) >> 8;
		tx_data[i + 1] = (*p) & 0x00ff;
		p++;
	}

    write_modbus(modbus, listframe);

    return ret;
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int writeSingleHoldReg2(modbus_t *modbus, uint16_t slave_addr,
                        uint16_t reg_addr, uint16_t reg_val, rx_indicate rx_ind)
{
    int ret = 0;

	list_frame_t *listframe = (list_frame_t *)vast_malloc(sizeof(list_frame_t) + sizeof(smart_frame_t) + sizeof(modbus_frame_t));
    smart_frame_t *smartframe = (smart_frame_t *)listframe->data;

    if (listframe == NULL) {
        ret = 1;
        log_w("listframe is null");
        return ret;
    }

	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

	modbusframe->slave_addr = slave_addr;
	modbusframe->opcode = WRITE_SINGLE_HOLD_RES;
	modbusframe->start_addr = le16_to_cpu(reg_addr);
	modbusframe->reg_cnt = le16_to_cpu(reg_val);
	smartframe->len = MODBUS_FRAME_HEAD + 0;
	listframe->rx_ind = rx_ind;

    write_modbus(modbus, listframe);

    return ret;
}

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
int readHoldReg2(modbus_t *modbus, uint16_t slave_addr, 
                    uint16_t start_addr, uint16_t reg_cnt, rx_indicate rx_ind)
{
    int ret = 0;

	list_frame_t *listframe = (list_frame_t *)vast_malloc(sizeof(list_frame_t) + sizeof(smart_frame_t) + sizeof(modbus_frame_t));
    smart_frame_t *smartframe = (smart_frame_t *)listframe->data;

    if (listframe == NULL) {
        ret = 1;
        log_w("listframe is null");
        return ret;
    }

	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

	modbusframe->slave_addr = slave_addr;
	modbusframe->opcode = READ_HOLD_RES;
	modbusframe->start_addr = le16_to_cpu(start_addr);
	modbusframe->reg_cnt = le16_to_cpu(reg_cnt);
	smartframe->len = MODBUS_FRAME_HEAD + 0;
	listframe->rx_ind = rx_ind;

    write_modbus(modbus, listframe);

    return ret;
}

/*
 * do idle
 */
static base_t do_execute0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
//    log_d("%s", __FUNCTION__);
	modbus_t *modbus = container_of(sm, modbus_t, sm);
	list_frame_t *listframe = (list_frame_t *)out;
	smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
	device_t *dev = ((device_t *)modbus)->user_data;

#if 0
	smart_frame_t *iter, *n;
	list_for_each_entry_safe(iter, n, &modbus->tx_list, entry)
	{
		elog_hexdump("mb.tx", 10, iter->data, iter->len);
	}
#endif

	elog_hexdump("modbus.tx", 10, smartframe->data, smartframe->len);
	modbus->ops->tx_prepare(modbus, smartframe->data);
	device_write(dev, 0, smartframe->data, smartframe->len);

    return 0;
}
static base_t do_execute1(state_machine_t *sm, const void *arg)
{
//    log_d("%s", __FUNCTION__);
	modbus_t *modbus = container_of(sm, modbus_t, sm);

    if (sm->trycnt > 3) {   /* try max */
        //state_machine_change(sm, STATE_IDLE);
        //return STATE_CTR_ERROR;
//		list_frame_t *iter, *n;
//		list_for_each_entry_safe(iter, n, &modbus->tx_list, entry)
//		{
//			list_del(&iter->entry);
//			vast_free(iter);
//		}
        goto error;
    } else if (!arg) {   	/* time out */
        return STATE_CTR_RETRY;
    }

	device_t *dev = ((device_t *)modbus);
	list_frame_t *listframe = (list_frame_t *)arg;
	smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
	list_frame_t *listframe_tx = (list_frame_t *)modbus->sm.tx_data;
	smart_frame_t *smartframe_tx = (smart_frame_t *)listframe_tx->data;
    listframe->rx_ind = listframe_tx->rx_ind;
    smartframe->to = smartframe_tx->to;

    if (modbus->ops->rx_indicate) {
        modbus->ops->rx_indicate(modbus, smartframe->len);
    }

    if (listframe->rx_ind) {
        listframe->rx_ind(dev, smartframe->len);
    }

error:
	listframe = list_last_entry(&modbus->tx_list, list_frame_t, entry);
	if (listframe) {
		list_del(&listframe->entry);
		vast_free(listframe);
	}

    return STATE_CTR_NEXT;
}

static base_t do_idle0(state_machine_t *sm, uint8_t *out, size_t maxlen)
{
//    log_d("%s", __FUNCTION__);
    return 0;
}

static base_t do_idle1(state_machine_t *sm, const void *arg)
{
//    log_d("%s", __FUNCTION__);
	modbus_t *modbus = container_of(sm, modbus_t, sm);
	//device_t *dev = ((device_t *)modbus);

	if (!list_empty_careful(&modbus->tx_list)) {
		list_frame_t *listframe = list_last_entry(&modbus->tx_list, list_frame_t, entry);
		//smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
		modbus->sm.tx_data = (void *)listframe;
        //modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;
		//modbus->slave_addr = modbusframe->slave_addr;

		//smart_frame_t *smartframe_rx = (smart_frame_t *)modbus->rx_data;
		//smartframe_rx->rx_ind = smartframe->rx_ind;
        //smartframe_rx->to = smartframe->to;
		//state_machine_change(&modbus->sm, STATE_EXEC);
	    return STATE_CTR_NEXT;
	} else {
        return STATE_CTR_WAIT;
	}
}
/**
  * @brief  vast_func.
  * @param
  * @retval
  */
static const state_machine_state_op_t state_machine_states[] =
{
	state_machine_state(STATE_EXEC , STATE_IDLE , pdMS_TO_TICKS(100) , do_execute) ,
	state_machine_state(STATE_IDLE , STATE_EXEC , pdMS_TO_TICKS(3)   , do_idle)    ,
};

//static base_t do_process0(const void *arg, uint8_t *out, size_t len)
//{
    //return 0;
//}

//static int smart_frame_len(const smart_frame_t *frame)
//{
    //return SMART_FRAME_HEAD + frame->len + 1;
//}

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
	list_frame_t *listframe = (list_frame_t *)&in[i];
	smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
	modbus_frame_t *modbus_frame = (modbus_frame_t *)smartframe->data;

	return modbus_frame;
}

static size_t check_smart_frame(const void *arg, size_t len)
{
    //state_machine_t *sm = (state_machine_t *)arg;
	modbus_t *modbus = container_of(arg, modbus_t, sm);

    modbus_frame_t *modbusframe = get_modbus_frame(modbus->rx_data, len);
    if (!modbusframe) return 0;
    //sm->data = (void *)modbusframe;

    //len = smart_frame_len(modbusframe);
    //log_d("got smart frame[%d]:", len);
    //elog_hexdump("prot.rx", 10, (uint8_t *)modbusframe, len);
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

	list_frame_t *listframe = (list_frame_t *)modbus->rx_data;
	smart_frame_t *smartframe = (smart_frame_t *)listframe->data;
	modbus_frame_t *modbusframe = (modbus_frame_t *)smartframe->data;

    int len = 0;
    len = device_peek(dev, 0, modbusframe, MODBUS_RX_MAX);
//    len = smartframe->len = device_read(dev, 0, modbusframe, MODBUS_RX_MAX);
    elog_hexdump("modbus.rx", 10, (uint8_t *)modbusframe, len);
    elog_raw("\r\n");
    if (len)
    {
        int ret = check_smart_frame((const void *)sm, len);

		if (ret) {
            smartframe->len = device_read(dev, 0, modbusframe, ret);
            sm->rx_data = (void *)listframe;
//            elog_hexdump("modbus.rx", 10, (uint8_t *)modbusframe, len);
            if (len > ret)
                task_send_signal(&sm->tcb, SIG_DATA);
		} else {
            device_read(dev, 0, modbusframe, len);
		}
    }

    return 0;
}

static error_t modbus_tx_done(device_t *dev, void *buffer)
{
	modbus_t *modbus = container_of(dev->owner, modbus_t, sm.tcb);

	modbus->ops->tx_done(modbus, buffer);

    return 0;
}

static error_t modbus_rx_ind(device_t *dev, size_t size)
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

    //modbus->sm.process0 = do_process0;
    modbus->sm.process1 = do_process1;
    modbus->sm.states = state_machine_states;
    modbus->sm.states_size = array_size(state_machine_states);
    modbus->sm.size_max = MODBUS_TX_MAX;
    state_machine_init(&modbus->sm);
    state_machine_change(&modbus->sm, STATE_IDLE);
	return 0;
}

/**
  * @brief  led_gpio_init.
  * @param
  * @retval
  */
static error_t modbus_control(device_t *dev, uint8_t cmd, void *args)
{
	modbus_t *modbus = (modbus_t *)dev;
    error_t err = 0;

    switch (cmd)
    {
    default:
        err = modbus->ops->ctrl(modbus, cmd, args);
        break;
    }

    return err;
}

/**
  * @brief  ina219_ops.
  * @param
  * @retval
  */
static const struct device_ops modbus_ops =
{
	.init = modbus_init,
	.ctrl = modbus_control,
};

/**
  * @brief  vast_func.
  * @param
  * @retval
  */
error_t modbus_device_register(modbus_t *modbus, const char *name, uint32_t flag, void *data)
//int modbus_device_register(void)
{
	int ret = 0;

    device_set_rx_indicate((device_t *)data, modbus_rx_ind);
    device_set_tx_complete((device_t *)data, modbus_tx_done);

    device_t *dev = &modbus->parent;

    dev->ops = &modbus_ops;
    dev->user_data = (void *)data;
    ret = device_register(dev, name, 0);
    device_open(dev, DEVICE_FLAG_DMA_RX | DEVICE_FLAG_DMA_TX | DEVICE_FLAG_FASYNC);

    //elog_set_filter_tag_lvl(LOG_TAG, ELOG_LVL_INFO);

    return ret;
}
//late_initcall(modbus_device_register);

/**
  * @}
  */
#endif /*configUSING_xxx*/

/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/




