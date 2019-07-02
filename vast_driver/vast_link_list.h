

#ifndef __LINK_LIST_H__
#define __LINK_LIST_H__

typedef enum {
	LINK_LIST_UNLOCKED,
	LINK_LIST_LOCKED,
}LinkListLock;

//节点结构体 
typedef struct _NodeTypeDef{
	struct _NodeTypeDef *pPrevNode;
	struct _NodeTypeDef *pNextNode;
	
	void *data;
	
}NodeTypeDef;


//链表结构体 
typedef struct _LinkListTypeDef{
	struct _NodeTypeDef *pHeadNode;
	struct _NodeTypeDef *pTailNode;
	LinkListLock lock:1;
	unsigned int len;
}LinkListTypeDef;


#define DBG_LINK_LIST_MSG 0

extern LinkListTypeDef **LinkListInit(LinkListTypeDef **pList);
extern int LinkListPush(LinkListTypeDef **pList, const void *pdata);
extern NodeTypeDef LinkListPop(LinkListTypeDef **pList, void **pdata);

extern int LinkListTestSelf(LinkListTypeDef **pList);

#if DBG_LINK_LIST_MSG
extern __weak int LinkListDbgMsg(char *file, char *function, int line);
#endif

#endif

