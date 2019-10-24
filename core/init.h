#ifndef _INIT_H
#define _INIT_H

#include <core/compiler.h>

typedef int (*initcall_t)(void);

#define __define_initcall(fn, id) \
    static const initcall_t __initcall_##fn##id __attribute__((used)) \
    __attribute__((__section__("initcall" #id "init"))) = fn;


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


static inline void do_init_call(void)
{
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
    extern initcall_t __initcall_start, __initcall_end; /*申明外部变量,在ld的脚本文件中定义*/
    initcall_t *fn = &__initcall_start;

    do{
    	(*fn++)();
    }while(fn < &__initcall_end);

#endif
}

#endif  /* end of _INIT_H */
