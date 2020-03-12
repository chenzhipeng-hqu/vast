/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-19     MurphyZhao   the first version
 */

//#include <rtthread.h>
#include <string.h>
#include <stdlib.h>

#include "utest.h"
#include <utest_log.h>

#include "component/vast_cli.h"

#ifdef configUSING_UTEST

#undef DBG_TAG
#undef DBG_LVL

#define DBG_TAG          "utest"
#ifdef UTEST_DEBUG
#define DBG_LVL          DBG_LOG
#else
#define DBG_LVL          DBG_INFO
#endif

#define LOG_TAG "utest"
#include "elog.h"

#define RT_CONSOLEBUF_SIZE 256

#if RT_CONSOLEBUF_SIZE < 256
#error "RT_CONSOLEBUF_SIZE is less than 256!"
#endif

#ifdef UTEST_THR_STACK_SIZE
#define UTEST_THREAD_STACK_SIZE UTEST_THR_STACK_SIZE
#else
#define UTEST_THREAD_STACK_SIZE (4096)
#endif

#ifdef UTEST_THR_PRIORITY
#define UTEST_THREAD_PRIORITY   UTEST_THR_PRIORITY
#else
#define UTEST_THREAD_PRIORITY   FINSH_THREAD_PRIORITY
#endif

static uint8_t utest_log_lv = UTEST_LOG_ALL;
static utest_tc_export_t tc_table = NULL;
static size_t tc_num;
static uint32_t tc_loop;
static struct utest local_utest = {UTEST_PASSED, 0, 0};

#if defined(__ICCARM__) || defined(__ICCRX__)         /* for IAR compiler */
#pragma section="UtestTcTab"
#endif

void utest_log_lv_set(uint8_t lv)
{
    if (lv == UTEST_LOG_ALL || lv == UTEST_LOG_ASSERT)
    {
        utest_log_lv = lv;
    }
}

int utest_init(void)
{
    /* initialize the utest commands table.*/
#if defined(__CC_ARM) || defined(__CLANG_ARM)       /* ARM C Compiler */
    extern const int UtestTcTab$$Base;
    extern const int UtestTcTab$$Limit;
    tc_table = (utest_tc_export_t)&UtestTcTab$$Base;
    tc_num = (utest_tc_export_t)&UtestTcTab$$Limit - tc_table;
#elif defined (__ICCARM__) || defined(__ICCRX__)    /* for IAR Compiler */
    tc_table = (utest_tc_export_t)__section_begin("UtestTcTab");
    tc_num = (utest_tc_export_t)__section_end("UtestTcTab") - tc_table;
#elif defined (__GNUC__)                            /* for GCC Compiler */
    extern const int __rt_utest_tc_tab_start;
    extern const int __rt_utest_tc_tab_end;
    tc_table = (utest_tc_export_t)&__rt_utest_tc_tab_start;
    tc_num = (utest_tc_export_t) &__rt_utest_tc_tab_end - tc_table;
#endif

    log_i("utest is initialize success.");
    log_i("total utest testcase num: (%d)", tc_num);
    return tc_num;
}
//INIT_COMPONENT_EXPORT(utest_init);
late_initcall(utest_init);

static void utest_tc_list(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    size_t i = 0;

    log_i("Commands list : ");

    for (i = 0; i < tc_num; i++)
    {
        log_i("[testcase name]:%s; [run timeout]:%d", tc_table[i].name, tc_table[i].run_timeout);
    }
}
//MSH_CMD_EXPORT_ALIAS(utest_tc_list, utest_list, output all utest testcase);
CLI_CMD_EXPORT(utest_list, "output all utest testcase", utest_tc_list, 0);

static const char *file_basename(const char *file)
{
    char *end_ptr = NULL;
    char *rst = NULL;

    if (!((end_ptr = strrchr(file, '\\')) != NULL || \
        (end_ptr = strrchr(file, '/')) != NULL) || \
        (strlen(file) < 2))
    {
        rst = (char *)file;
    }
    else
    {
        rst = (char *)(end_ptr + 1);
    }
    return (const char *)rst;
}

static int utest_help(void)
{
    printf("\r\n");
    printf("Command: utest_run\r\n");
    printf("   info: Execute test cases.\r\n");
    printf(" format: utest_run [-thread or -help] [testcase name] [loop num]\r\n");
    printf("  usage:\r\n");
    printf("         1. utest_run\r\n");
    printf("            Do not specify a test case name. Run all test cases.\r\n");
    printf("         2. utest_run -thread\r\n");
    printf("            Do not specify a test case name. Run all test cases in threaded mode.\r\n");
    printf("         3. utest_run testcaseA\r\n");
    printf("            Run 'testcaseA'.\r\n");
    printf("         4. utest_run testcaseA 10\r\n");
    printf("            Run 'testcaseA' ten times.\r\n");
    printf("         5. utest_run -thread testcaseA\r\n");
    printf("            Run 'testcaseA' in threaded mode.\r\n");
    printf("         6. utest_run -thread testcaseA 10\r\n");
    printf("            Run 'testcaseA' ten times in threaded mode.\r\n");
    printf("         7. utest_run test*\r\n");
    printf("            support '*' wildcard. Run all test cases starting with 'test'.\r\n");
    printf("         8. utest_run -help\r\n");
    printf("            Show utest help information\r\n");
    printf("\n");
    return 0;
}

static void utest_run(const char *utest_name)
{
    size_t i;
    uint32_t index;
    int is_find;

    //rt_thread_mdelay(1000);

    for (index = 0; index < tc_loop; index ++)
    {
        i = 0;
        is_find = FALSE;
        log_i("[==========] [ utest    ] loop %d/%d", index + 1, tc_loop);
        log_i("[==========] [ utest    ] started");
        while(i < tc_num)
        {
            if (utest_name)
            {
                int len = strlen(utest_name);
                if (utest_name[len - 1] == '*')
                {
                    len -= 1;
                }
                if (memcmp(tc_table[i].name, utest_name, len) != 0)
                {
                    i++;
                    continue;
                }
            }
            is_find = TRUE;

            log_i("[----------] [ testcase ] (%s) started", tc_table[i].name);
            if (tc_table[i].init != NULL)
            {
                if (tc_table[i].init() != VAST_OK)
                {
                    log_e("[  FAILED  ] [ result   ] testcase (%s)", tc_table[i].name);
                    goto __tc_continue;
                }
            }

            if (tc_table[i].tc != NULL)
            {
                tc_table[i].tc();
                if (local_utest.failed_num == 0)
                {
                    log_i("[  PASSED  ] [ result   ] testcase (%s)", tc_table[i].name);
                }
                else
                {
                    log_e("[  FAILED  ] [ result   ] testcase (%s)", tc_table[i].name);
                }
            }
            else
            {
                log_e("[  FAILED  ] [ result   ] testcase (%s)", tc_table[i].name);
            }

            if (tc_table[i].cleanup != NULL)
            {
                if (tc_table[i].cleanup() != VAST_OK)
                {
                    log_e("[  FAILED  ] [ result   ] testcase (%s)", tc_table[i].name);
                    goto __tc_continue;
                }
            }

    __tc_continue:
            log_i("[----------] [ testcase ] (%s) finished", tc_table[i].name);

            i++;
        }

        if (i == tc_num && is_find == FALSE && utest_name != NULL)
        {
            log_i("[==========] [ utest    ] Not find (%s)", utest_name);
            log_i("[==========] [ utest    ] finished");
            break;
        }

        log_i("[==========] [ utest    ] finished");
    }
}

static void utest_testcase_run(CLI_HandleTypeDef *pCli, int argc, char** argv)
{
    //void *thr_param = NULL;

    static char utest_name[UTEST_NAME_MAX_LEN];
    memset(utest_name, 0x0, sizeof(utest_name));

    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);

    tc_loop = 1;

    if (argc == 2 || argc == 3)
    {
        strncpy(utest_name, argv[1], sizeof(utest_name) -1);
        if (argc == 3) tc_loop = atoi(argv[2]);
        utest_run(utest_name);
    }
    else
    {
        log_e("[  error   ] at (%s:%d), in param error.", __func__, __LINE__);
        utest_help();
    }
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_LVL | ELOG_FMT_DIR | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
}
//MSH_CMD_EXPORT_ALIAS(utest_testcase_run, utest_run, utest_run [-thread or -help] [testcase name] [loop num]);
CLI_CMD_EXPORT(utest_run, "utest_run [testcase name] [loop num]", utest_testcase_run, 0);

utest_t utest_handle_get(void)
{
    return (utest_t)&local_utest;
}

void utest_unit_run(test_unit_func func, const char *unit_func_name)
{
    //log_i("[==========] utest unit name: (%s)", unit_func_name);
    local_utest.error = UTEST_PASSED;
    local_utest.passed_num = 0;
    local_utest.failed_num = 0;

    if (func != NULL)
    {
        func();
    }
}

void utest_assert(int value, const char *file, int line, const char *func, const char *msg)
{
    if (!(value))
    {
        local_utest.error = UTEST_FAILED;
        local_utest.failed_num ++;
        log_e("[  ASSERT  ] [ unit     ] at (%s); func: (%s:%d); msg: (%s)", file_basename(file), func, line, msg);
    }
    else
    {
        if (utest_log_lv == UTEST_LOG_ALL)
        {
            log_d("[    OK    ] [ unit     ] (%s:%d) is passed", func, line);
        }
        local_utest.error = UTEST_PASSED;
        local_utest.passed_num ++;
    }
}

void utest_assert_string(const char *a, const char *b, int equal, const char *file, int line, const char *func, const char *msg)
{
    if (a == NULL || b == NULL)
    {
        utest_assert(0, file, line, func, msg);
    }

    if (equal)
    {
        if (strcmp(a, b) == 0)
        {
            utest_assert(1, file, line, func, msg);
        }
        else
        {
            utest_assert(0, file, line, func, msg);
        }
    }
    else
    {
        if (strcmp(a, b) == 0)
        {
            utest_assert(0, file, line, func, msg);
        }
        else
        {
            utest_assert(1, file, line, func, msg);
        }
    }
}

void utest_assert_buf(const char *a, const char *b, size_t sz, int equal, const char *file, int line, const char *func, const char *msg)
{
    if (a == NULL || b == NULL)
    {
        utest_assert(0, file, line, func, msg);
    }

    if (equal)
    {
        if (memcmp(a, b, sz) == 0)
        {
            utest_assert(1, file, line, func, msg);
        }
        else
        {
            utest_assert(0, file, line, func, msg);
        }
    }
    else
    {
        if (memcmp(a, b, sz) == 0)
        {
            utest_assert(0, file, line, func, msg);
        }
        else
        {
            utest_assert(1, file, line, func, msg);
        }
    }
}

static void test_xxx(void)
{
    uassert_true(1);
    uassert_false("123");
}

static error_t utest_tc_init(void)
{
    return VAST_OK;
}

static error_t utest_tc_cleanup(void)
{
    return VAST_OK;
}

static void testcase(void)
{
    UTEST_UNIT_RUN(test_xxx);
}
UTEST_TC_EXPORT(testcase, "utest.sample", utest_tc_init, utest_tc_cleanup, 10);

#endif /*configUSING_UTEST*/


