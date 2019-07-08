/**
 ******************************************************************************
 * @file    xxx.h
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

/* Define to prevent recursive inclusion */
#ifndef __VAST_LINK_LIST_H__
#define __VAST_LINK_LIST_H__

#ifdef __cplusplus
 extern "C" {
#endif

/***********************************************
                    include
***********************************************/
#include "vast_common.h"

/***********************************************
                    define
***********************************************/
#define DBG_LINK_LIST_MSG 0

/***********************************************
                    typedef
***********************************************/
 //�ڵ�ṹ��
 typedef struct _NodeTypeDef{
 	struct _NodeTypeDef *pPrevNode;
 	struct _NodeTypeDef *pNextNode;

 	void *data;

 }NodeTypeDef;

 //����ṹ��
 typedef struct _LinkListTypeDef{
 	struct _NodeTypeDef *pHeadNode;
 	struct _NodeTypeDef *pTailNode;
 	Vast_LockTypedef lock:1;
 	volatile unsigned int len;
 }LinkListTypeDef;

/***********************************************
               function prototypes
***********************************************/
 extern int16_t LinkListInit(LinkListTypeDef **pList);
 extern int16_t LinkListPush(LinkListTypeDef **pList, const void *pdata);
 extern int16_t LinkListPop(LinkListTypeDef **pList, void **pdata);

 extern int16_t LinkListTestSelf(LinkListTypeDef **pList);

 #if DBG_LINK_LIST_MSG
 extern __weak int LinkListDbgMsg(char *file, char *function, int line);
 #endif

/***********************************************
	      		    inline
***********************************************/

/***********************************************
                   variable
***********************************************/

#ifdef __cplusplus
}
#endif

#endif /* __VAST_LINK_LIST_H__ */
/**
  * @}
  */


/*************** (C) COPYRIGHT chenzhipeng *******END OF FILE******/
