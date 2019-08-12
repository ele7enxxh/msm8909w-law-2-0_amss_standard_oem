/*==============================================================================
  Copyright (c) 2009 Qualcomm Technologies Incorporated.
           All Rights Reserved.

$Header: //components/rel/rpm.bf/2.1.1/core/dal/framework/src/DALHeap.c#1 $

when       who     description
--------   ---     ----------------------------------------------------------
04/06/09   ts      First cut

==============================================================================*/
#include "DALHeap.h"

struct _HeapNode
{
   HeapNode * pNext;
   HeapNode * pPrev;
};

/* Macros used */
#define ADDR(node)       (unsigned int)node + sizeof(HeapNode)
#define NSIZE(node)      ((unsigned int)NEXT(node) - (unsigned int)node - sizeof(HeapNode))
#define NODE(addr)       (HeapNode *)((unsigned int)addr - sizeof(HeapNode))
#define PREV(node)       ((HeapNode *)node)->pPrev
#define NEXT(node)       (HeapNode *)((unsigned int)((HeapNode *)node)->pNext & 0xfffffffe)
#define MARK_INUSE(node) ((HeapNode *)node)->pNext = (HeapNode *)((unsigned int)(((HeapNode *)node)->pNext) | 0x1)
#define MARK_AVAIL(node) ((HeapNode *)node)->pNext = (HeapNode *)((unsigned int)(((HeapNode *)node)->pNext) & 0xfffffffe)
#define ISAVAIL(node)    !((unsigned int)(((HeapNode *)node)->pNext)&0x1)

int DALHeap_Init(DALHeap * pHeap, void * pHeapMem, unsigned int iHeapSize)
{
   HeapNode * pEndNode;

   /* sizeof(heap_node) to account for atleast 1 free node and 1 end marker node */   
   if(!pHeap || !pHeapMem || iHeapSize < 2*sizeof(HeapNode))
      return DAL_ERROR;

   /* setup end marker node */
   pEndNode = (HeapNode *)((unsigned int)pHeapMem + iHeapSize - sizeof(HeapNode));
   pEndNode->pNext = (HeapNode *)0x1;
   pEndNode->pPrev = (HeapNode *)pHeapMem;
   
   /* setup first free node */
   pHeap->pFreeNode = (HeapNode*)pHeapMem;
   pHeap->pFreeNode->pNext = pEndNode;
   pHeap->pFreeNode->pPrev = pEndNode;

   /* initialze the crit sect used to protect alloc/free access of the heap */
   if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                     &pHeap->hHeapLock, (void *)pHeap->HeapLockMem))
      return DAL_ERROR;
 
   return DAL_SUCCESS;
}

void * DALHeap_Malloc(DALHeap * pHeap, unsigned int iReqSize)
{
   unsigned int iAddr = 0;
   HeapNode * pHeapNode = 0x0;

   if(!pHeap || !iReqSize)
      return 0;

   /* Align size to 4 byte boundry */
   iReqSize = ((iReqSize%4)?((iReqSize&0xfffffffc)+4):iReqSize);

   DALSYS_SyncEnter(pHeap->hHeapLock);

   /* Find an appropriate free node by traversing from the begining.
      We search in fwd direction only because pHeap->pFreeNode should
      always point to the first free node i.e. no node is free prior 
      to this node. 
   */
   pHeapNode = pHeap->pFreeNode;  
   while(pHeapNode)
   { 
      if(ISAVAIL(pHeapNode) && NSIZE(pHeapNode) >= iReqSize)
         break;

      pHeapNode = NEXT(pHeapNode);
   }

   if(!pHeapNode) /* No memory available */
      goto exit;

   iAddr = ADDR(pHeapNode); /* The first address of the memory to be returned */
   
   /* Check if we can fragment the current node to a smaller one */
   if(NSIZE(pHeapNode) > iReqSize + sizeof(HeapNode))
   {
      /* Setup ptr to the next free node available */
      HeapNode * pFreeNode = (HeapNode *)(iAddr + iReqSize);
 
      pFreeNode->pNext = pHeapNode->pNext;
      pFreeNode->pPrev = pHeapNode;
      pHeapNode->pNext->pPrev = pFreeNode;
      pHeapNode->pNext = pFreeNode;

      if(pHeapNode == pHeap->pFreeNode)
         pHeap->pFreeNode = pFreeNode;
   }

   MARK_INUSE(pHeapNode); /* Mark the consumed free node to be in use */

exit:

   DALSYS_SyncLeave(pHeap->hHeapLock);
   
   return (void*)iAddr;
}


void DALHeap_Free(DALHeap * pHeap, void * pMem) 
{
   HeapNode *pNextNode, *pPrevNode;

   if(!pMem)
      return;
   
   DALSYS_SyncEnter(pHeap->hHeapLock);

   /* note: we don't check if pMem is in the range of this pHeap */

   /* mark the heap node as available */
   MARK_AVAIL(NODE(pMem));

   pNextNode = pPrevNode = NODE(pMem);

   /* find out available nodes in fwd direction to current node */
   while(ISAVAIL(NEXT(pNextNode)))
      pNextNode = NEXT(pNextNode);

   /* find out available nodes in rev direction to current node */
   while(ISAVAIL(PREV(pPrevNode)))
      pPrevNode = PREV(pPrevNode);

   pPrevNode->pNext = pNextNode->pNext;
   pNextNode->pNext->pPrev = pPrevNode;

   if(pHeap->pFreeNode > pPrevNode)
      pHeap->pFreeNode = pPrevNode;

   DALSYS_SyncLeave(pHeap->hHeapLock);

   return;
}

