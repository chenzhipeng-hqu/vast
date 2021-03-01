/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for non-os stm32f10x.
 * Created on: 2015-04-28
 */

// #include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "core/init.h"

#define LOG_TAG    "elog.port"
#include "elog.h"

//#define ELOG_TEST

#ifdef ELOG_TEST
/**
  * @brief  cli_tmr_cb.
  * @param
  * @retval
  */
static void test_tmr_cb(struct soft_timer * st)
{
    st->expires = jiffies + pdMS_TO_TICKS(2000);
    soft_timer_add(st);

    //HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);

    log_a("Hello EasyLogger 0!");
    log_e("Hello EasyLogger 1!");
    log_w("Hello EasyLogger 2!");
    log_i("Hello EasyLogger 3!");
    log_d("Hello EasyLogger 4!");
    log_v("Hello EasyLogger 5!");
}

static struct soft_timer test_tmr =
{
    .expires = (INITIAL_JIFFIES) + pdMS_TO_TICKS(1000),
    .cb = test_tmr_cb,
    .name = "elog_test",
};
#endif

void assert_hook(const char* ex, const char* func, size_t line) 
{
    //cmb_println("(%s) has assert failed at %s:%ld.", ex, func, line);
    //cm_backtrace_assert(cmb_get_sp());
}

static int elog_initilize(void)
{
	int ret = 0;

    /* initialize EasyLogger */
    elog_init();
    elog_set_text_color_enabled(true);
    //elog_assert_set_hook(assert_hook);

//#if defined(EF_USING_ENV)
#if 0
    /* set EasyLogger log format */
    size_t fmt_lvl = 0;
    char *c_fmt_lvl;

    c_fmt_lvl = ef_get_env("fmt_assert");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_ASSERT, fmt_lvl);
        c_fmt_lvl = NULL;
    }

    c_fmt_lvl = ef_get_env("fmt_error");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_ERROR, fmt_lvl);
        c_fmt_lvl = NULL;
    }

    c_fmt_lvl = ef_get_env("fmt_warn");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_WARN, fmt_lvl);
        c_fmt_lvl = NULL;
    }

    c_fmt_lvl = ef_get_env("fmt_info");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_INFO, fmt_lvl);
        c_fmt_lvl = NULL;
    }

    c_fmt_lvl = ef_get_env("fmt_debug");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_DEBUG, fmt_lvl);
        c_fmt_lvl = NULL;
    }

    c_fmt_lvl = ef_get_env("fmt_verbose");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_fmt(ELOG_LVL_VERBOSE, fmt_lvl);
        c_fmt_lvl = NULL;
    }
#else
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_FUNC | ELOG_FMT_LINE | ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_DIR | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
#endif
    /* start EasyLogger */
    elog_start();

//#if defined(EF_USING_ENV)
#if 0
    c_fmt_lvl = ef_get_env("filter_lvl");
    if(c_fmt_lvl != NULL)
    {
        fmt_lvl = atol(c_fmt_lvl);
        elog_set_filter_lvl(fmt_lvl);
        c_fmt_lvl = NULL;
    }
#elif defined(NDEBUG)
	elog_set_filter_lvl(ELOG_LVL_WARN);
#else
	elog_set_filter_lvl(ELOG_LVL_DEBUG);
#endif
	return ret;
}
arch_initcall(elog_initilize);

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    #ifdef ELOG_TEST
    	soft_timer_add(&test_tmr);
	#endif

    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    /* output to terminal */
    printf("%.*s", size, log);
    //TODO output to flash
    // printf("1234\r\n");
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
//    __disable_irq();
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
//    __enable_irq();
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
	static char buf[16] = {0};
	//sprintf(buf, "%08ld", HAL_GetTick());
    
    // time_t now = time(NULL);
    // struct tm ptm = {0};
    // localtime_r(&now, &ptm);
    
    // static char buf[16] = {0};
    // strftime(buf, 16, "%H:%M:%S", &ptm);

    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *t = NULL;
    t = localtime(&tv.tv_sec);
    strftime(buf, 16, "%H:%M:%S", t);
    sprintf(&buf[8], ".%03.0f", tv.tv_usec*0.001);

    return buf;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    return "pid:xx";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    return "tid:xx";
}

#if (defined configUSING_CLI || defined VAST_USING_SHELL)
/**
  * @brief  CLICmd_Print
  * @param
  * @retval
  */
void CLICmd_log_lvl(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    uint8_t level = 0;

    if(argc > 2)
    {
        level = str2u32(argv[2]);

        if (level > ELOG_LVL_VERBOSE)
        {
            pCli->Init.Write("Please input correct level (0-%d).\r\n", ELOG_FILTER_LVL_ALL);
            return ;
        }

        if(!strcmp(argv[1], "all"))
        {
            elog_set_filter_lvl(level);

		//#if defined(EF_USING_ENV)
		#if 0
            char c_new_filter_lvl[5] = {0};
            sprintf(c_new_filter_lvl,"%d", level);
            ef_set_env("filter_lvl", c_new_filter_lvl);
		#endif
        }
        else
        {
            elog_set_filter_tag_lvl(argv[1], level);
        }
    }
    else if (argc > 1)
    {
        level = elog_get_filter_tag_lvl(argv[1]);
    }
    else
    {
        pCli->Init.Write("Please input: log_lvl <tag|all> [level].\r\n");
        pCli->Init.Write("Assert  : 0\r\n");
        pCli->Init.Write("Error   : 1\r\n");
        pCli->Init.Write("Warning : 2\r\n");
        pCli->Init.Write("Info    : 3\r\n");
        pCli->Init.Write("Debug   : 4\r\n");
        pCli->Init.Write("verbose : 5\r\n");
        return ;
    }

    pCli->Init.Write("%s level %d.\r\n", argv[1], level);
}
//CLI_CMD_EXPORT(log_lvl, "log_lvl <tag|all> [0~5]", CLICmd_log_lvl, 0);

/**
  * @brief  CLICmd_Print
  * @param
  * @retval
  */
void CLICmd_log_kw(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    if(argc > 1)
    {
        elog_set_filter_kw(argv[1]);
    }
    else
    {
        elog_set_filter_kw("");
    }
}
//CLI_CMD_EXPORT(log_kw, "log_kw [keyword]", CLICmd_log_kw, 0);

/**
  * @brief  CLICmd_Print
  * @param
  * @retval
  */
void CLICmd_log_tag(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    if(argc > 1)
    {
        elog_set_filter_tag(argv[1]);
    }
    else
    {
        elog_set_filter_tag("");
    }
}
//CLI_CMD_EXPORT(log_tag, "log_tag [tag]", CLICmd_log_tag, 0);

/**
  * @brief  CLICmd_Print
  * @param
  * @retval
  */
void CLICmd_log_fmt(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    if(argc > 2)
    {
        uint8_t level = str2u32(argv[1]);
        uint8_t set = str2u32(argv[2]);

        elog_set_fmt(level, set);

	//#if defined(EF_USING_ENV)
	#if 0
        char c_new_fmt[5] = {0};
        sprintf(c_new_fmt,"%d", set);
        if(level == ELOG_LVL_ASSERT) {
            ef_set_env("fmt_assert", c_new_fmt);
        } else if(level == ELOG_LVL_ERROR) {
            ef_set_env("fmt_error", c_new_fmt);
        } else if(level == ELOG_LVL_WARN) {
            ef_set_env("fmt_warn", c_new_fmt);
        } else if(level == ELOG_LVL_INFO) {
            ef_set_env("fmt_info", c_new_fmt);
        } else if(level == ELOG_LVL_DEBUG) {
            ef_set_env("fmt_debug", c_new_fmt);
        } else if(level == ELOG_LVL_VERBOSE) {
            ef_set_env("fmt_verbose", c_new_fmt);
        }
	#endif
    }
    else
    {
        pCli->Init.Write("Please input: log_fmt <level> <format>.\r\n");
        pCli->Init.Write("level    : %#x\r\n", ELOG_FMT_LVL);
        pCli->Init.Write("tag      : %#x\r\n", ELOG_FMT_TAG);
        pCli->Init.Write("time     : %#x\r\n", ELOG_FMT_TIME);
        pCli->Init.Write("process  : %#x\r\n", ELOG_FMT_P_INFO);
        pCli->Init.Write("thread   : %#x\r\n", ELOG_FMT_T_INFO);
        pCli->Init.Write("file     : %#x\r\n", ELOG_FMT_DIR );
        pCli->Init.Write("function : %#x\r\n", ELOG_FMT_FUNC);
        pCli->Init.Write("line     : %#x\r\n", ELOG_FMT_LINE);
    }
}
/**
  * @brief  CLICmd_Print
  * @param
  * @retval
  */
void CLICmd_log_test(CLI_HandleTypeDef *pCli, int argc, char *argv[])
{
    log_a("Hello EasyLogger assert level 0!");
    log_e("Hello EasyLogger error level 1!");
    log_w("Hello EasyLogger warning level 2!");
    log_i("Hello EasyLogger info level 3!");
    log_d("Hello EasyLogger debug level 4!");
    log_v("Hello EasyLogger verbose level 5!");
}

const CLICmdTypedef cliCmdTableElog[] =
{
  {"lvl" , "lvl <tag|all> [0~5]"  , CLICmd_log_lvl , 0} ,
  {"kw"  , "kw [keyword]"         , CLICmd_log_kw  , 0} ,
  {"tag" , "tag [tag]"            , CLICmd_log_tag , 0} ,
  {"fmt" , "fmt <level> <format>" , CLICmd_log_fmt , 0} ,
  {"test", "print test log"       , CLICmd_log_test , 0} ,
  {0, 0, 0, 0}
};
CLI_CMD_EXPORT(log, "log dir", 0, cliCmdTableElog);

#endif

