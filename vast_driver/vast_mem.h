#ifndef __MALLOC_H
#define __MALLOC_H

//#include <iostream>
//using namespace std;

//用户定义参数
#define MEMCRC   //屏蔽关闭调试信息

/*
#define malloc  vast_malloc
#define free   	vast_free
#define realloc vast_realloc
#define calloc 	vast_calloc
*/

#define MAX_SIZE	  10*1024 			//总字节数 = MAX_BLOCK * MIN_SIZE
#define MIN_SIZE	  32						//最小分配空间字节大小
#define MAX_BLOCK	  (MAX_SIZE/MIN_SIZE/2-1)					  //记录块大小  块大小必须 < MAX_SIZE/MIN_SIZE,注意不可等于因为内存对齐时预留空间。
#define END_BLOCK	  MAX_BLOCK-1		//记录块末端块

#define  MEM_UINT	 unsigned int
#define  MEM_UCHAR   unsigned char	
#define  MEM_INT     int

#define  MEM_ADD     unsigned int //地址强制转换类型

#ifdef MEMCRC
#include "usart.h"
#include "stdio.h"
void vast_getmemtablestatus(void);//获取内存表状态
#endif


//用户调用函数
void vast_meminit(void);														//内存块初始化.
void *vast_malloc(MEM_UINT size);									//内存分配.
void vast_free(void *ptr);													//内存释放.
void *vast_realloc(void *ptr, MEM_UINT nbytes);		//内存分配.
void *vast_calloc(MEM_UINT count, MEM_UINT size);	//内存分配.

#endif

