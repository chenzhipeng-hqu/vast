/*****************************************************************************
* Product: "DPP" example on STM32F4-Discovery board, cooperative QV kernel
* Last Updated for Version: 6.5.0
* Date of the Last Update:  2019-05-09
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2005-2019 Quantum Leaps, LLC. All rights reserved.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Alternatively, this program may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GNU General Public License and are specifically designed for
* licensees interested in retaining the proprietary status of their code.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact information:
* https://www.state-machine.com
* mailto:info@state-machine.com
*****************************************************************************/
#include "qpc.h"

#include "stm32f4xx.h"  /* CMSIS-compliant header file for the MCU used */
#include "vast_core/softtimer.h"
#include "vast_core/init.h"
#include "../qpc_task/qpc_common.h"

/* add other drivers if necessary... */

//Q_DEFINE_THIS_FILE

/* ISRs defined in this BSP ------------------------------------------------*/
//void SysTick_Handler(void);

/* Local-scope defines -----------------------------------------------------*/

#ifdef Q_SPY
    static QSTimeCtr QS_tickTime_;
    static QSTimeCtr QS_tickPeriod_;

    /* event-source identifiers used for tracing */
    static uint8_t const l_SysTick;

    enum AppRecords { /* application-specific trace records */
        PHILO_STAT = QS_USER,
        COMMAND_STAT
    };

#endif

/* ISRs used in this project ===============================================*/
//void SysTick_Handler(void) {
//
//#ifdef Q_SPY
//    {
//        tmp = SysTick->CTRL; /* clear SysTick_CTRL_COUNTFLAG */
//        QS_tickTime_ += QS_tickPeriod_; /* account for the clock rollover */
//    }
//#endif
//
//    QF_TICK_X(0U, &l_SysTick); /* process time events for tick rate 0 */
//}

/*..........................................................................*/
#ifdef Q_SPY
/*
* ISR for receiving bytes from the QSPY Back-End
* NOTE: This ISR is "QF-unaware" meaning that it does not interact with
* the QF/QK and is not disabled. Such ISRs don't need to call QK_ISR_ENTRY/
* QK_ISR_EXIT and they cannot post or publish events.
*/
void USART2_IRQHandler(void) {
    if ((USART2->SR & USART_SR_RXNE) != 0) {
        uint32_t b = USART2->DR;
        QS_RX_PUT(b);
    }
}
#else

#endif

/* QF init ============================================================*/
int QF_init_bsp(void)
{
	static QSubscrList 					subscrSto[MAX_PUB_SIG];
	static QF_MPOOL_EL(DebugLevelEvt) 	smlPoolSto[256] __attribute__ ((section (".ccmram"))); /* small pool */

	QF_init(); /* initialize the framework */

	QF_psInit(subscrSto, Q_DIM(subscrSto));

	QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

	Test_Ctor(1);

	return 0;
}

qf_initcall(QF_init_bsp);

/* QF callbacks ============================================================*/
void QF_onStartup(void) {

    /* enable IRQs... */
#ifdef Q_SPY
    NVIC_EnableIRQ(USART2_IRQn); /* USART2 interrupt used for QS-RX */
#endif
}
/*..........................................................................*/
void QF_onCleanup(void) {
}
/*..........................................................................*/
void QV_onIdle(void) { /* CATION: called with interrupts DISABLED, NOTE01 */

#ifdef Q_SPY
    QF_INT_ENABLE();
    QS_rxParse();  /* parse all the received bytes */

    if ((USART2->SR & USART_FLAG_TXE) != 0) { /* TXE empty? */
        uint16_t b;

        QF_INT_DISABLE();
        b = QS_getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {  /* not End-Of-Data? */
            USART2->DR  = (b & 0xFFU);  /* put into the DR register */
        }
    }
#elif defined NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    /* !!!CAUTION!!!
    * The WFI instruction stops the CPU clock, which unfortunately disables
    * the JTAG port, so the ST-Link debugger can no longer connect to the
    * board. For that reason, the call to __WFI() has to be used with CAUTION.
    *
    * NOTE: If you find your board "frozen" like this, strap BOOT0 to VDD and
    * reset the board, then connect with ST-Link Utilities and erase the part.
    * The trick with BOOT(0) is that it gets the part to run the System Loader
    * instead of your broken code. When done disconnect BOOT0, and start over.
    */
    //QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
    QF_INT_ENABLE(); /* just enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */

    soft_timer_task();
#endif
}

/*..........................................................................*/
void Q_onAssert(char const *module, int loc) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)loc;
    QS_ASSERTION(module, loc, (uint32_t)10000U); /* report assertion to QS */
//    NVIC_SystemReset();
    printf("%s %d, %d\r\n" ,module, loc, QF_getPoolMin(1));
//    printf("%s %d, %d,%d,%d\r\n" ,module, loc, QF_getPoolMin(1), QF_getPoolMin(2), QF_getPoolMin(3));
   // while(1);
}

/* QS callbacks ============================================================*/
#ifdef Q_SPY
/*..........................................................................*/
uint8_t QS_onStartup(void const *arg) {
    static uint8_t qsBuf[2*1024]; /* buffer for QS-TX channel */
    static uint8_t qsRxBuf[100];  /* buffer for QS-RX channel */
    GPIO_InitTypeDef GPIO_struct;
    USART_InitTypeDef USART_struct;

    (void)arg; /* avoid the "unused parameter" compiler warning */
    QS_initBuf(qsBuf, sizeof(qsBuf));
    QS_rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    /* enable peripheral clock for USART2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* GPIOA clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* GPIOA Configuration:  USART2 TX on PA2 and RX on PA3 */
    GPIO_struct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_struct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_struct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_struct.GPIO_OType = GPIO_OType_PP;
    GPIO_struct.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_struct);

    /* Connect USART2 pins to AF2 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); /* TX = PA2 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); /* RX = PA3 */

    USART_struct.USART_BaudRate = 115200;
    USART_struct.USART_WordLength = USART_WordLength_8b;
    USART_struct.USART_StopBits = USART_StopBits_1;
    USART_struct.USART_Parity = USART_Parity_No;
    USART_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_struct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_struct);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); /* enable RX interrupt */
    USART_Cmd(USART2, ENABLE); /* enable USART2 */

    QS_tickPeriod_ = SystemCoreClock / BSP_TICKS_PER_SEC;
    QS_tickTime_ = QS_tickPeriod_; /* to start the timestamp at zero */

    /* setup the QS filters... */
    QS_FILTER_ON(QS_SM_RECORDS); /* state machine records */
    QS_FILTER_ON(QS_AO_RECORDS); /* active object records */
    QS_FILTER_ON(QS_UA_RECORDS); /* all user records */

    return (uint8_t)1; /* return success */
}
/*..........................................................................*/
void QS_onCleanup(void) {
}
/*..........................................................................*/
QSTimeCtr QS_onGetTime(void) {  /* NOTE: invoked with interrupts DISABLED */
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0) { /* not set? */
        return QS_tickTime_ - (QSTimeCtr)SysTick->VAL;
    }
    else { /* the rollover occured, but the SysTick_ISR did not run yet */
        return QS_tickTime_ + QS_tickPeriod_ - (QSTimeCtr)SysTick->VAL;
    }
}
/*..........................................................................*/
void QS_onFlush(void) {
    uint16_t b;

    QF_INT_DISABLE();
    while ((b = QS_getByte()) != QS_EOD) { /* while not End-Of-Data... */
        QF_INT_ENABLE();
        while ((USART2->SR & USART_FLAG_TXE) == 0) { /* while TXE not empty */
        }
        USART2->DR = (b & 0xFFU); /* put into the DR register */
        QF_INT_DISABLE();
    }
    QF_INT_ENABLE();
}
/*..........................................................................*/
/*! callback function to reset the target (to be implemented in the BSP) */
void QS_onReset(void) {
    NVIC_SystemReset();
}
/*..........................................................................*/
/*! callback function to execute a user command (to be implemented in BSP) */
void QS_onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    void assert_failed(char const *module, int loc);
    (void)cmdId;
    (void)param1;
    (void)param2;
    (void)param3;
    QS_BEGIN(COMMAND_STAT, (void *)1) /* application-specific record begin */
        QS_U8(2, cmdId);
        QS_U32(8, param1);
        QS_U32(8, param2);
        QS_U32(8, param3);
    QS_END()

    if (cmdId == 10U) {
        Q_ERROR();
    }
    else if (cmdId == 11U) {
        assert_failed("QS_onCommand", 123);
    }
}

#endif /* Q_SPY */
/*--------------------------------------------------------------------------*/

/*****************************************************************************
* NOTE00:
* The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
* ISR priority that is disabled by the QF framework. The value is suitable
* for the NVIC_SetPriority() CMSIS function.
*
* Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
* with the numerical values of priorities equal or higher than
* QF_AWARE_ISR_CMSIS_PRI) are allowed to call any QF services. These ISRs
* are "QF-aware".
*
* Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
* level (i.e., with the numerical values of priorities less than
* QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
* Such "QF-unaware" ISRs cannot call any QF services. The only mechanism
* by which a "QF-unaware" ISR can communicate with the QF framework is by
* triggering a "QF-aware" ISR, which can post/publish events.
*
* NOTE01:
* The QV_onIdle() callback is called with interrupts disabled, because the
* determination of the idle condition might change by any interrupt posting
* an event. QV_onIdle() must internally enable interrupts, ideally
* atomically with putting the CPU to the power-saving mode.
*
* NOTE02:
* One of the LEDs is used to visualize the idle loop activity. The brightness
* of the LED is proportional to the frequency of invcations of the idle loop.
* Please note that the LED is toggled with interrupts locked, so no interrupt
* execution time contributes to the brightness of the User LED.
*/
