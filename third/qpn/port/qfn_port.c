/*****************************************************************************
* Product: BSP for Blinky example, POSIX with cooperative QV kernel
* Last updated for version 5.4.0
* Last updated on  2015-05-24
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) Quantum Leaps, LLC. All rights reserved.
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
* Web:   www.state-machine.com
* Email: info@state-machine.com
*****************************************************************************/
#include "qpn.h"
#include "qpn_driver_mcu.h"  /* Application interface */

#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* for memcpy() and memset() */ 

/* Local-scope objects -----------------------------------------------------*/
//static struct termios l_tsav; [> structure with saved terminal attributes <]

/*..........................................................................*/

/*..........................................................................*/
void Q_onAssert(char_t const Q_ROM * const file, int_t line) {

}

/*--------------------------------------------------------------------------*/
void QF_onStartup(void) {
    //QF_setTickRate(BSP_TICKS_PER_SEC);
}

/*..........................................................................*/
void QF_onCleanup(void) {
    /* restore the saved terminal attributes */
    printf("\nBye! Bye! \r\n");
}

/*..........................................................................*/
void QF_stop(void) 
{
    //l_isRunning = false;    [> cause exit from the event loop <]
}

inline void QV_onIdle(void)  /* CATION: called with interrupts DISABLED, NOTE01 */
{
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    QV_CPU_SLEEP();  /* atomically go to sleep and enable interrupts */
#else
    QF_INT_ENABLE(); /* just enable interrupts */
#endif
}

/****************************************************************************/
/**
* @description
* QF_run() is typically called from your startup code after you initialize
* the QF and start at least one active object with QActive_start().
* This implementation of QF_run() is for the cooperative Vanilla kernel.
*
* @returns QF_run() typically does not return in embedded applications.
* However, when QP runs on top of an operating system,  QF_run() might
* return and in this case the return represents the error code (0 for
* success). Typically the value returned from QF_run() is subsequently
* passed on as return from main().
*/
int_t QF_run(void) {
    uint_fast8_t p;
    QActive *a;
		static  bool initialized = false;
	
		if(!initialized)
		{
			/* set priorities all registered active objects... */
			for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
					a = QF_ROM_ACTIVE_GET_(p);
					a->prio = (uint8_t)p; /* set the priority of the active object */
			}

			/* trigger initial transitions in all registered active objects... */
			for (p = (uint_fast8_t)1; p <= QF_maxActive_; ++p) {
					a = QF_ROM_ACTIVE_GET_(p);
					QHSM_INIT(&a->super); /* take the initial transition in the SM */
			}

			QF_onStartup(); /* invoke startup callback */

			/* the event loop of the cooperative QV-nano kernel... */
			QF_INT_DISABLE();
			initialized = true;
		}
		
    for (;;) {
        if (QF_readySet_ != (uint_fast8_t)0) {
            QActiveCB const Q_ROM *acb;

            /* hi nibble non-zero? */
            if ((QF_readySet_ & (uint_fast8_t)0xF0) != (uint_fast8_t)0) {
                p = (uint_fast8_t)(
                      (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_ >> 4])
                      + (uint_fast8_t)4);
            }
            else { /* hi nibble of QF_readySet_ is zero */
                p = (uint_fast8_t)Q_ROM_BYTE(QF_log2Lkup[QF_readySet_]);
            }

            acb = &QF_active[p];
            a = QF_ROM_ACTIVE_GET_(p);

            --a->nUsed;
            Q_SIG(a) = QF_ROM_QUEUE_AT_(acb, a->tail).sig;
#if (Q_PARAM_SIZE != 0)
            Q_PAR(a) = QF_ROM_QUEUE_AT_(acb, a->tail).par;
#endif
            if (a->tail == (uint8_t)0) { /* wrap around? */
                a->tail = Q_ROM_BYTE(acb->qlen);
            }
            --a->tail;
            QF_INT_ENABLE();

            QHSM_DISPATCH(&a->super); /* dispatch to the HSM (RTC step) */

            QF_INT_DISABLE();
            /* empty queue? */
            if (a->nUsed == (uint8_t)0) {
                /* clear the bit corresponding to 'p' */
                QF_readySet_ &=
                    (uint_fast8_t)~((uint_fast8_t)1 << (p - (uint_fast8_t)1));
            }
        }
        else {
            /* QV_onIdle() must be called with interrupts DISABLED because
            * the determination of the idle condition (no events in the
            * queues) can change at any time by an interrupt posting events
            * to a queue. QV_onIdle() MUST enable interrupts internally,
            * perhaps at the same time as putting the CPU into a power-saving
            * mode.
            */
            QV_onIdle();

            QF_INT_DISABLE();
						break;
        }
    }
		return (int_t)0;
}
