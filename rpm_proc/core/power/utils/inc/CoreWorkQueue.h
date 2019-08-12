#ifndef COREWORKQUEUE_H
#define COREWORKQUEUE_H

/*==============================================================================

FILE:      CoreWorkQueue.h

DESCRIPTION: Core Work Queue API.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreWorkQueue.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------

==============================================================================*/

#include "CoreWorkContainer.h"
#include "CorePool.h"


typedef struct CoreWorkQueueElemType
{
  struct CoreWorkQueueElemType *pNext;
  char                          pElemData[];
} CoreWorkQueueElemType;

typedef struct CoreWorkQueueType
{
  /* the actual queue */
  CoreWorkQueueElemType *pHead;
  CoreWorkQueueElemType *pTail;

  /* work queue element memory pool */
  CorePoolType           ElemPool;
  unsigned int           NumElems;
} CoreWorkQueueType;


/*
 * Public functions.
 */

/* Create a new instance of a DAL WorkQueue. */
DALResult Core_WorkQueueCreate(CoreWorkContainerHandle *phContainer,
                               CoreContainerProcessFcn  pfnProcess, 
                               uint32                   uElemNum,
                               uint32                   uElemNumSubsequent,
                               uint32                   uDataSize);

/* Function to allocate an element buffer from the proallocated memory pool. */
DALResult Core_WorkQueueAlloc(CoreWorkContainerHandle  hContainer,
                              void                     **ppData);

/* Function to enqueue an element */
DALResult Core_WorkQueuePut(CoreWorkContainerHandle  hContainer,
                            void                     *pData);

/* Function to insert an element at the head of the queue */
DALResult Core_WorkQueuePutAtHead(CoreWorkContainerHandle  hContainer,
                            void                     *pData);


/* Check to see if there are any elements. */
DALBOOL Core_WorkQueueIsEmpty(CoreWorkContainerHandle hContainer);

/* Check how many elements are in the queue. */
unsigned int Core_WorkQueueLength(CoreWorkContainerHandle hContainer);

/*
 * Container internal functions -- you probably shouldn't call these.
 */

/* Function to free an element buffer. */
DALResult Core_WorkQueueFree(CoreWorkContainerHandle  hContainer,
                             void                     *pData);

DALResult Core_WorkQueueGet(CoreWorkContainerHandle  hContainer,
                            void                     **ppData);

#endif /* COREWORKQUEUE_H */

