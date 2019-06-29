/*************************************************************************
    > File Name: log.c
    > Author: 
 ************************************************************************/
#include <stdio.h>
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <pthread.h>
#include "debug.h"


int safe_asprintf(char **strp, const char *fmt, ...);
int safe_vasprintf(char **strp, const char *fmt, va_list ap);

void plog(const char *format, ...);
void pinfo(const char *format, ...);

static pthread_mutex_t fileMutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * safe_asprintf();
 */
int safe_asprintf(char **strp, const char *fmt, ...) 
{
    va_list ap;
    int retval;

    va_start(ap, fmt);
    retval = safe_vasprintf(strp, fmt, ap);
    va_end(ap);

    return retval;
}

/*
 * safe_vasprintf();
 */
int safe_vasprintf(char **strp, const char *fmt, va_list ap) 
{
    int retval;

    retval = vasprintf(strp, fmt, ap);
    if (retval == -1) 
    {
        printf("Failed to vasprintf: %s.  Bailing out\n", strerror(errno));
        return 1;
    }
    return retval;
}

size_t limit_file_size(char *file_path)
{
    size_t size = 0;
    FILE *fp = NULL;

    if (!(fp = fopen(file_path, "rb"))) {
        return -1;
    }
    
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);

    if (size > FILE_SIZE_MAX)
    {
        //printf("size=%ld\r\n", size);
        remove(file_path);
    }

    return size;    
}

/*
 * plog();
 */
void plog(const char *format, ...) 
{

    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    limit_file_size("log.txt");

    // Open debug info output file.
    if (!(fp = fopen("log.txt", "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    time_t timep;
    struct tm *ptm = NULL;
    time(&timep);
    ptm = localtime(&timep);
    fprintf(fp, "[%04d-%02d-%02d-%02d-%02d-%02d] %s", 
            ptm->tm_year + 1900, 
            ptm->tm_mon + 1,
            ptm->tm_mday, 
            ptm->tm_hour, 
            ptm->tm_min, 
            ptm->tm_sec, 
            fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}

/*
 * pinfo();
 */
void pinfo(const char *format, ...) 
{
    pthread_mutex_lock(&fileMutex);

    FILE *fp = NULL;
    va_list vlist;
    char *fmt = NULL;

    // Open debug info output file.
    if (!(fp = fopen("log.txt", "a+"))) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    va_start(vlist, format);
    safe_vasprintf(&fmt, format, vlist);
    va_end(vlist);
    if (!fmt) {
        pthread_mutex_unlock(&fileMutex);
        return;
    }

    fprintf(fp, "%s", fmt);

    free(fmt);
    fsync(fileno(fp));
    fclose(fp);

    pthread_mutex_unlock(&fileMutex);
}


