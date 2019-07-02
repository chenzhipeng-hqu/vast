

#include <stdlib.h>
#include <vast_link_list.h>


#if DBG_LINK_LIST_MSG
#include "usart.h"
#include "cmsis_os.h"
#include "string.h"

char tmp_buf[200];
#endif

LinkListTypeDef **LinkListInit(LinkListTypeDef **pList)
{
	(*pList) = (LinkListTypeDef *)malloc(sizeof(LinkListTypeDef));
	(*pList)->len = 0;
	(*pList)->lock = LINK_LIST_UNLOCKED;
	
	if(*pList == NULL)
	{
		return NULL;
	}
	
	(*pList)->pHeadNode = (*pList)->pTailNode = NULL;

#if DBG_LINK_LIST_MSG	
	sprintf(tmp_buf, "one node size = %d bytes, LinkList size = %d bytes \r\n", sizeof(NodeTypeDef), sizeof(LinkListTypeDef));
	HAL_UART_Transmit_DMA(&huart1,  (uint8_t *)(tmp_buf) , strlen(tmp_buf));
	osDelay(10);
#endif
	
	return pList;
}

int LinkListPush(LinkListTypeDef **pList, const void *pdata)
{
	NodeTypeDef *pNode = NULL;
	
	if(pdata == NULL)
	{
	#if DBG_LINK_LIST_MSG	
		LinkListDbgMsg(__FILE__, "LinkListPush", __LINE__);
	#endif
		return 1;
	}
	
	do
	{
		pNode = (NodeTypeDef *)malloc(sizeof(NodeTypeDef));
		if( pNode == NULL)
		{
		#if DBG_LINK_LIST_MSG	
			LinkListDbgMsg(__FILE__, "LinkListPush", __LINE__);
		#endif
		}
	}while(pNode == NULL);
	
	while((*pList)->lock == LINK_LIST_LOCKED){
	#if DBG_LINK_LIST_MSG	
		LinkListDbgMsg(__FILE__, "LinkListPush", __LINE__);
	#endif
	}
	(*pList)->lock = LINK_LIST_LOCKED;
	
#if DBG_LINK_LIST_MSG	
	
//	sprintf(tmp_buf, "pdata = %s, pdata = 0x%x \r\n", pdata, (int)pdata);
//	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tmp_buf, strlen((const char *)tmp_buf));
//	osDelay(10);
#endif
	
	pNode->data = (void *)pdata;
	pNode->pPrevNode = (*pList)->pTailNode;
	pNode->pNextNode = NULL;
	
	if((*pList)->pTailNode != NULL)  //���β�ڵ㲻Ϊ��
	{
		(*pList)->pTailNode->pNextNode = pNode;	
	}
	
	(*pList)->pTailNode = pNode;

	if((*pList)->pHeadNode == NULL)
	{
		(*pList)->pHeadNode = pNode;
	}
		
	((*pList)->len)++;
	(*pList)->lock = LINK_LIST_UNLOCKED;
	
#if DBG_LINK_LIST_MSG	
	sprintf(tmp_buf, "push--> pNode = 0x%x, pdata = 0x%x, data=<%s>, len = %d\r\n", (int)(*pList)->pTailNode, (int)(*pList)->pTailNode->data, (char *)(*pList)->pTailNode->data, (*pList)->len);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)(tmp_buf) , strlen(tmp_buf));
	osDelay(10);
#endif
	return 0;
}

NodeTypeDef LinkListPop(LinkListTypeDef **pList, void **pdata)
{
	NodeTypeDef pNode = {NULL};
	
	if((*pList)->pHeadNode == NULL)
	{
		*pdata = NULL;
		return pNode;
	}
		
	while((*pList)->lock == LINK_LIST_LOCKED){
	#if DBG_LINK_LIST_MSG	
		LinkListDbgMsg(__FILE__, "LinkListPush", __LINE__);
	#endif
	}
	(*pList)->lock = LINK_LIST_LOCKED;
	
	pNode = *((*pList)->pHeadNode);
	*pdata = pNode.data;
	
#if DBG_LINK_LIST_MSG
	sprintf(tmp_buf, "pop--> pNode = 0x%x, pdata = 0x%x, data=<%s>\r\n", (int)(*pList)->pHeadNode, (int)*pdata, (char *)*pdata);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)(tmp_buf) , strlen(tmp_buf));
	osDelay(10);
#endif	
	
	if((*pList)->pHeadNode->pNextNode != NULL)  // ���ͷ�ڵ����һ�ڵ㲻Ϊ��
	{
		(*pList)->pHeadNode = (*pList)->pHeadNode->pNextNode;
		free(((*pList)->pHeadNode->pPrevNode));
		(*pList)->pHeadNode->pPrevNode = NULL;
	}
	else
	{
		free(((*pList)->pHeadNode));
		(*pList)->pHeadNode = NULL;
	}
	
	(*pList)->len--;
	if((*pList)->len == 0)
	{
		(*pList)->pTailNode = NULL;
	}
	(*pList)->lock = LINK_LIST_UNLOCKED;
	return pNode;
}

#if DBG_LINK_LIST_MSG
__weak int LinkListDbgMsg(char *file, char *function, int line)
{
	sprintf(tmp_buf, "file=%s, function=%s, line=%d\r\n", file, function, line);
	HAL_UART_Transmit_DMA(&huart1, (uint8_t *)tmp_buf, strlen((const char *)tmp_buf));
	osDelay(10);
	return 0;
}
#endif	

int LinkListTestSelf(LinkListTypeDef **pList)
{
	char *pPush = NULL;
	char *pPop = NULL;
	char ret = 0;
	char i, test_times = 5;
	
	for(i=0; i<test_times; i++) // push to linklist
	{
		do		
		{
			pPush = (char *)malloc(1*sizeof(char));
		}while(pPush == NULL);
		pPush[0] = i;
		LinkListPush(pList, (void *)pPush);
	}
	
	for(i=0; i<test_times; i++)   // pop from linklist
	{
		LinkListPop(pList, (void **)&pPop);		// '7''8''9' pop from linklist
		if( pPop[0] != i )
		{
			ret = 1;
			break;
		}
		free(pPop);
	}
	
	return ret;
}

