#ifndef _INIT_H
#define _INIT_H

#include <stdio.h>
#include <core/compiler.h>
#include "bsp.h"

typedef int (*initcall_t)(void);

#define DEBUG_INIT

#ifdef DEBUG_INIT
	struct init_desc
	{
		const char* fn_name;
		const initcall_t fn;
	};
	#define __define_initcall(fn, id) \
		static const struct init_desc __initcall_##fn##id __attribute__((used)) \
		__attribute__((__section__("initcall" #id "init"))) = {#fn, fn};
#else
	#define __define_initcall(fn, id) \
		static const initcall_t __initcall_##fn##id __attribute__((used)) \
		__attribute__((__section__("initcall" #id "init"))) = fn;
#endif


#define pure_initcall(fn)       __define_initcall(fn, 0)
#define core_initcall(fn)       __define_initcall(fn, 1)
#define postcore_initcall(fn)   __define_initcall(fn, 2)
#define arch_initcall(fn)       __define_initcall(fn, 3)
#define subsys_initcall(fn)     __define_initcall(fn, 4)
#define fs_initcall(fn)         __define_initcall(fn, 5)
#define device_initcall(fn)     __define_initcall(fn, 6)
#define late_initcall(fn)       __define_initcall(fn, 7)
#define qf_initcall(fn)       	__define_initcall(fn, 8)
#define qfrun_initcall(fn)      __define_initcall(fn, 9)

#define INIT_EXPORT(fn, id)    __define_initcall(fn, id)

/* board init routines will be called in board_init() function */
#define INIT_BOARD_EXPORT(fn)           INIT_EXPORT(fn, 1)
/* pre/device/component/env/app init routines will be called in init_thread */
/* components pre-initialization (pure software initilization) */
#define INIT_PREV_EXPORT(fn)            INIT_EXPORT(fn, 2)
/* device initialization */
#define INIT_DEVICE_EXPORT(fn)          INIT_EXPORT(fn, 3)
/* components initialization (dfs, lwip, ...) */
#define INIT_COMPONENT_EXPORT(fn)       INIT_EXPORT(fn, 5)
/* environment initialization (mount disk, ...) */
#define INIT_ENV_EXPORT(fn)             INIT_EXPORT(fn, 6)
/* appliation initialization (rtgui application etc ...) */
#define INIT_APP_EXPORT(fn)             INIT_EXPORT(fn, 7)

#define INIT_QF_EXPORT(fn)              INIT_EXPORT(fn, 8)

/**
 * @brief configures IWDG and enable watchdog.
 * @param None
 * @retval None
 */
static inline void do_init_call(void)
{
    vast_bsp_init();

#if defined(__CC_ARM)
    extern initcall_t initcall0init$$Base[];
    extern initcall_t initcall7init$$Limit[];

    initcall_t *fn;

    for (fn = initcall0init$$Base;
            fn < initcall7init$$Limit;
            fn++)
    {
        (*fn)();
    }
#elif defined (__GNUC__)

	#ifdef DEBUG_INIT
    	extern struct init_desc __initcall_start, __initcall_end; /*申明外部变量,在ld的脚本文件中定义*/
    	const struct init_desc *desc = &__initcall_start;

		do{
	        printf("init %s", desc->fn_name);
	        printf(":%d done\r\n", desc->fn());
		}while(desc++ < &__initcall_end);
	#else
		extern initcall_t __initcall_start, __initcall_end; /*申明外部变量,在ld的脚本文件中定义*/

		initcall_t *fn = &__initcall_start;

		do{
			(*fn++)();
		}while(fn < &__initcall_end);
	#endif

#endif
}

extern void vast_system_heap_init(void *begin_addr, void *end_addr);

#endif  /* end of _INIT_H */
