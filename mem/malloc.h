#ifndef __MALLOC_H
#define __MALLOC_H

//#include <iostream>
//using namespace std;

//�û��������
#define MEMCRC   //���ιرյ�����Ϣ

#define malloc  mymalloc
#define free   	myfree 
#define realloc myrealloc 
#define calloc 	mycalloc 


#define MAX_SIZE	  110*1024 			//���ֽ��� = MAX_BLOCK * MIN_SIZE
#define MIN_SIZE	  32						//��С����ռ��ֽڴ�С   
#define MAX_BLOCK	  50					  //��¼���С  ���С���� < MAX_SIZE/MIN_SIZE,ע�ⲻ�ɵ�����Ϊ�ڴ����ʱԤ���ռ䡣 
#define END_BLOCK	  MAX_BLOCK-1		//��¼��ĩ�˿�

#define  MEM_UINT	   unsigned int
#define  MEM_UCHAR   unsigned char	
#define  MEM_INT     int

#define  MEM_ADD     unsigned int //��ַǿ��ת������

#ifdef MEMCRC
#include "Uart.h"
//#include "stdio.h" 
void Getmemtablestatus(void);//��ȡ�ڴ��״̬
#endif


//�û����ú���
void mymeminit(void);														//�ڴ���ʼ��.
void *mymalloc(MEM_UINT size);									//�ڴ����.
void myfree(void *ptr);													//�ڴ��ͷ�.  
void *myrealloc(void *ptr, MEM_UINT nbytes);		//�ڴ����.
void *mycalloc(MEM_UINT count, MEM_UINT size);	//�ڴ����. 

#endif

