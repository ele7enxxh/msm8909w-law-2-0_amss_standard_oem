/*==============================================================================

FILE:      CoreWorkQueue.c

DESCRIPTION: Implementation of Work Queue that can be added to Container 
Workloop.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/src/CoreWorkQueue.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------

==============================================================================*/


#include "DALSys.h"
#include "CoreWorkQueue.h"
#include "CoreVerify.h"

#define TAIL_INSERT_FLAG 1
#define HEAD_INSERT_FLAG 0

DALResult Core_WorkQueueCreate(CoreWorkContainerHandle *phContainer,
                               CoreContainerProcessFcn  pfnProcess,
                               uint32                   uElemNum,
                               uint32                   uElemNumSubsequent,
                               uint32                   uDataSize)
{
  DALResult RetVal; 
  CoreWorkContainerType *pContainer;
  CoreWorkQueueType *pWkQ;
  uint32 uElemSize;
  CorePoolType *pPool;
  DALBOOL bCleanupCtnr = FALSE, bCleanupLock = FALSE, bCleanupWkQ = FALSE;

  do
  {
    /* general work container data */
    RetVal = DALSYS_Malloc(sizeof(CoreWorkContainerType), (void**)phContainer);
    pContainer = (CoreWorkContainerType *)(*phContainer);
    if((DAL_SUCCESS != RetVal) || (NULL == *phContainer)) break; else bCleanupCtnr = TRUE;
    DALSYS_memset(pContainer, 0, sizeof(CoreWorkContainerType));
    
    /* fill in the function pointers */
    pContainer->pfnPut = Core_WorkQueuePut;
    pContainer->pfnGet = Core_WorkQueueGet;
    pContainer->pfnProcess = pfnProcess;
    pContainer->pfnFree = Core_WorkQueueFree;

    /* create the lock object */
    RetVal = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                               &(pContainer->hLock),
                               NULL);
    if(DAL_SUCCESS != RetVal) break; else bCleanupLock = TRUE;

    /* hWorkLoop and link_hdl are set to 0, dont need to do anything */

    /* work queue specific data */
    RetVal = DALSYS_Malloc(sizeof(CoreWorkQueueType), &(pContainer->pCntrData));
    if(DAL_SUCCESS != RetVal) break; else bCleanupWkQ = TRUE;
    CORE_VERIFY_PTR(pContainer->pCntrData);
    pWkQ = (CoreWorkQueueType *)pContainer->pCntrData;
    DALSYS_memset(pContainer->pCntrData, 0, sizeof(CoreWorkQueueType));

    /* allocate work queue memory pool */
    uElemSize = sizeof(CoreWorkQueueElemType) + uDataSize;
    DALSYS_SyncEnter(pContainer->hLock);
    pPool = &pWkQ->ElemPool;
    Core_PoolCreate(pPool, uElemNum, uElemSize);
    if ( 0 != uElemNumSubsequent )
      Core_PoolSetAllocationSize( pPool, uElemNumSubsequent );
    DALSYS_SyncLeave(pContainer->hLock);

  } while(FALSE);

  /* if everything succeeded, just return */
  if(DAL_SUCCESS == RetVal)
    return RetVal;

  /* something failed, so we have cleanup to do */
  if(bCleanupWkQ)
  {
    DALSYS_Free(pContainer->pCntrData);
  }

  if(bCleanupLock)
  {
    DALSYS_DestroyObject(pContainer->hLock);
  }

  if(bCleanupCtnr)
  {
    DALSYS_Free(pContainer);
    *phContainer = NULL;
  }

  return RetVal;
}

DALResult Core_WorkQueueAlloc(CoreWorkContainerHandle  hContainer,
                              void                     **ppData)
{
  char *pElem;
  CoreWorkQueueType *pWkQ;

  /* if failed or pool empty, return NULL *ppData */
  *ppData = NULL;

  if((NULL == hContainer) || (NULL == hContainer->pCntrData))
    return DAL_ERROR;
  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  pElem = Core_PoolAlloc(&pWkQ->ElemPool);
  if(NULL == pElem) return DAL_ERROR;
  
  /* The only data member of CoreWorkQueueElemType is a structure pointer, the
   * most restrictive type of the ARM processor. Ideally this should enforce 
   * the memory alignment of *ppData */
  *ppData = pElem + sizeof(CoreWorkQueueElemType);

  return DAL_SUCCESS;
}

DALResult Core_WorkQueueFree(CoreWorkContainerHandle  hContainer,
                             void                     *pData)
{
  char *pElem;
  CoreWorkQueueType *pWkQ;

  if((NULL == hContainer) || (NULL == hContainer->pCntrData) || (NULL == pData))
    return DAL_ERROR;
  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  pElem = (char *)pData - sizeof(CoreWorkQueueElemType);
  Core_PoolFree(&pWkQ->ElemPool, pElem);

  return DAL_SUCCESS;
}

static DALResult Core_WorkQueueInsert(CoreWorkContainerHandle  hContainer,
                            void *pData, int tailFlag)
{
  CoreWorkQueueType *pWkQ;
  CoreWorkQueueElemType *pElem;
  DALResult RetVal = DAL_SUCCESS;

  if((NULL == hContainer) || (NULL == hContainer->pCntrData) || (NULL == pData))
    return DAL_ERROR;
  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;
    
  DALSYS_SyncEnter(hContainer->hLock);

  pWkQ->NumElems++;
  pElem = (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType)); 
  pElem->pNext = NULL;

  if(pWkQ->pTail)
  {
    /* insert at tail of queue - non modem event */
    if (tailFlag)
    {
      /* insertion into nonempty queue */
      pWkQ->pTail->pNext = pElem;
      pWkQ->pTail = pElem;
    }
    /* insert at head of queue - modem event */
    else
    {
      pElem->pNext = pWkQ->pHead;
      pWkQ->pHead = pElem;
    }
  }
  else
  {
    /* first element in queue */
    pWkQ->pHead = pWkQ->pTail = pElem;

    #ifndef FEATURE_NO_WORKLOOPS
    /* signal workloop that queue is not empty */
    if(hContainer->hWorkLoop)
    {
      CoreContainerWorkLoopHandle pCWL = hContainer->hWorkLoop;
      if(pCWL->hContainerEvent)
      {
        RetVal = DALSYS_EventCtrlEx(pCWL->hContainerEvent, DALSYS_EVENT_CTRL_TRIGGER, 0, 0, 0);
      }
    }
    #endif
  }

  DALSYS_SyncLeave(hContainer->hLock);

  return RetVal;
}

DALResult Core_WorkQueuePutAtHead(CoreWorkContainerHandle hContainer,
                                  void *pData)
{
  return Core_WorkQueueInsert(hContainer, pData, HEAD_INSERT_FLAG);
}

DALResult Core_WorkQueuePut(CoreWorkContainerHandle hContainer,
                                  void                    *pData)
{
  return Core_WorkQueueInsert(hContainer, pData, TAIL_INSERT_FLAG);
}


DALResult Core_WorkQueueGet(CoreWorkContainerHandle  hContainer,
                            void                   **ppData)
{
  CoreWorkQueueType *pWkQ;
  CoreWorkQueueElemType *pElem;
  DALResult RetVal = DAL_SUCCESS;

  if(NULL == ppData)
    return DAL_ERROR;
  
  if((NULL == hContainer) || (NULL == hContainer->pCntrData))
  {
    *ppData = NULL;
    return DAL_ERROR;
  }
  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;
  
  DALSYS_SyncEnter(hContainer->hLock);

  pElem = pWkQ->pHead;
  if(pElem)
  {
    pWkQ->NumElems--;
    *ppData = pElem->pElemData;

    /* update the queue head */
    if(pElem->pNext)
    {
      /* there's at least one more left */
      pWkQ->pHead = pElem->pNext;
    }
    else
    {
      /* we're taking the last element */
      pWkQ->pHead = NULL;
      pWkQ->pTail = NULL;
    }
  }
  else
  {
    /* nothing in the queue */
    *ppData = NULL;
    RetVal = DAL_ERROR;
  }

  DALSYS_SyncLeave(hContainer->hLock);

  return RetVal;
}

DALBOOL Core_WorkQueueIsEmpty(CoreWorkContainerHandle  hContainer)
{
  if(!hContainer || !hContainer->pCntrData)
    return TRUE;
  
  return (NULL == ((CoreWorkQueueType *)hContainer->pCntrData)->pHead);
}

unsigned int Core_WorkQueueLength(CoreWorkContainerHandle  hContainer)
{
  if(!hContainer || !hContainer->pCntrData)
    return TRUE;
  
  return ((CoreWorkQueueType *)hContainer->pCntrData)->NumElems;
}

