/*==============================================================================

FILE:      CoreWorkContainer.c

DESCRIPTION: Implementation of Container Workloop.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================
                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/src/CoreWorkContainer.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
9/14/2010  mabel   Added synchronous container process function,
                   and synchronous add container function to enable
				   synchronous util (as well as asynch)
==============================================================================*/

#include "DALSys.h"
#include "CoreVerify.h"
#include "CoreWorkContainer.h"

#ifndef FEATURE_NO_WORKLOOPS
/* processes a workloop asynchronously by the worker thread */
static DALResult Core_WorkLoopProcess(DALSYSEventHandle hEvent, void *pData)
{
  DALResult RetVal;
  DALBOOL bKeepGoing;
  CoreContainerWorkLoopHandle hWorkLoop = (CoreContainerWorkLoopHandle)pData;
  CoreContainerWorkLoopType *pCWL = (CoreContainerWorkLoopType *)hWorkLoop;
  CoreWorkContainerType *pWC;

  if(NULL == pCWL)
    return DAL_ERROR;

  /* initially reset the event or DAL won't block on it */
  RetVal = DALSYS_EventCtrl(pCWL->hContainerEvent, DALSYS_EVENT_CTRL_RESET);
  if(DAL_SUCCESS != RetVal) return RetVal;

  RetVal = DALSYS_EventCtrl(pCWL->hContainerProcessStartEvent, DALSYS_EVENT_CTRL_TRIGGER);
  if(DAL_SUCCESS != RetVal) return RetVal;

  while(1)
  {
    /* wait for any of our containers to be ready, DALSYS_EventWait() consumes
       the event and reset the event flag (pEventObj->eventRcvd) */
    if(DAL_SUCCESS != DALSYS_EventWait(pCWL->hContainerEvent))
      continue; /* wait error?  try again. */

	/* lock to protect the container list */
	DALSYS_SyncEnter(pCWL->hContainerListLock);

	/* loop through all attached containers until there's nothing left to do */
	do 
	{
      bKeepGoing = FALSE;
      
	  /* retrieve the head container */
      pWC = (CoreWorkContainerType*)pCWL->hContainerList;

	  /* highest priority container, so process all of it */
      if (DAL_SUCCESS == Core_WorkContainerProcess(pWC))
		pWC = (CoreWorkContainerType*)pWC->hLink;

	  while(pWC) 
	  {
		/* if an element can be processed, do so then break 
	     * to check the higher priority containers */
        if (DAL_SUCCESS == Core_WorkElementProcess(pWC)) 
		{
		  bKeepGoing = TRUE;
		  break;
		}
	    pWC = (CoreWorkContainerType*)pWC->hLink;
      }
	} while(bKeepGoing);
	DALSYS_SyncLeave(pCWL->hContainerListLock);
  }
}
#endif

DALResult Core_WorkElementProcess(CoreWorkContainerType *pWC) 
{
  void *pElement;
  DALResult RetVal = DAL_SUCCESS;
  if (pWC == NULL)
    return DAL_ERROR;
  CORE_VERIFY(pWC->pfnGet && pWC->pfnProcess);
  /* try to get something from this container */
  if(DAL_SUCCESS == pWC->pfnGet(pWC, &pElement))
  {
    /* found something from this container */
    pWC->pfnProcess(pElement);
    if(pWC->pfnFree)
      pWC->pfnFree(pWC, pElement);
	RetVal = DAL_SUCCESS;
  }
  else 
  {
    RetVal = DAL_ERROR;
  }
  return RetVal;
}

/* processes a single container */
DALResult Core_WorkContainerProcess(CoreWorkContainerType *pWC)
{
  DALResult RetVal = DAL_SUCCESS;

  /* make sure its a valid container, if it is process the first element */
  if (pWC)
    while(DAL_SUCCESS == Core_WorkElementProcess(pWC)){}
  else 
    RetVal = DAL_ERROR;
  return RetVal;
}

#ifndef FEATURE_NO_WORKLOOPS
DALResult Core_RegisterContainerWorkLoop(uint32 dwPriority,
                                         CoreContainerWorkLoopHandle *phContainerWorkLoop)
{
  DALResult RetVal;
  CoreContainerWorkLoopType *pCWL;
  DALBOOL CleanupMalloc = FALSE, CleanupWorkLoop = FALSE, CleanupWLEvt = FALSE, 
          CleanupWLAdd = FALSE, CleanupCtnrEvt = FALSE, CleanupStartEvt = FALSE, 
          CleanupLock = FALSE;

  if(NULL == phContainerWorkLoop)
    return DAL_ERROR;

  do
  {
    /* allocate memory for our structure */
    RetVal = DALSYS_Malloc(sizeof(CoreContainerWorkLoopType), (void**)phContainerWorkLoop);
    pCWL = (CoreContainerWorkLoopType *)(*phContainerWorkLoop);
    if(DAL_SUCCESS != RetVal) break; else CleanupMalloc = TRUE;
    DALSYS_memset(pCWL, 0, sizeof(CoreContainerWorkLoopType));
  
    /* create the DAL base work loop to work with */
	RetVal = DALSYS_RegisterWorkLoop(dwPriority,         /* pass through user priority */
                                     1,                  /* max num events */
                                     &(pCWL->hWorkLoop), /* return pointer to user */
                                     NULL);              /* workloop is hard to preallocate for */
	if(DAL_SUCCESS != RetVal) break; else CleanupWorkLoop = TRUE;
	/* create the workloop event that we'll auto-fire to trigger the container process function */
	RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, /* workloop event */
                               &(pCWL->hWorkLoopEvent),          /* handle for it */
                               NULL); /* worry about preallocation support later */
	if(DAL_SUCCESS != RetVal) break; else CleanupWLEvt = TRUE;
	/* add the workloop event to the workloop */
	RetVal = DALSYS_AddEventToWorkLoop(pCWL->hWorkLoop,
                                Core_WorkLoopProcess,
                                (void*)pCWL,
                                pCWL->hWorkLoopEvent,
                                NULL);
	if(DAL_SUCCESS != RetVal) break; else CleanupWLAdd = TRUE;
	  /* create a 'client default' event which is supposed to map to native signal types */
	  RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
								&(pCWL->hContainerEvent),
								NULL); /* worry about preallocation support later */
	if(DAL_SUCCESS != RetVal) break; else CleanupCtnrEvt = TRUE;

	RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
								&(pCWL->hContainerProcessStartEvent),
								NULL); /* worry about preallocation support later */
	if(DAL_SUCCESS != RetVal) break; else CleanupStartEvt = TRUE;

	RetVal = DALSYS_EventCtrl(pCWL->hContainerProcessStartEvent, DALSYS_EVENT_CTRL_RESET);
	if(DAL_SUCCESS != RetVal) break;

	/* create a lock to protect the container list */
	RetVal = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
							   &(pCWL->hContainerListLock),
							   NULL); /* worry about preallocation support later */
	if(DAL_SUCCESS != RetVal) break; else CleanupLock = TRUE;

	RetVal = DALSYS_EventCtrl(pCWL->hWorkLoopEvent, DALSYS_EVENT_CTRL_TRIGGER);
	if(DAL_SUCCESS != RetVal) break;
	
	RetVal = DALSYS_EventWait(pCWL->hContainerProcessStartEvent);
  } while(FALSE);

  /* If we succeeded, just exit. */
  if(DAL_SUCCESS == RetVal) return RetVal;

  /* Something failed, so clean up. */
  if(CleanupLock)
  {
    DALSYS_DestroyObject(pCWL->hContainerListLock);
  }

  if(CleanupStartEvt)
  {
    DALSYS_DestroyObject(pCWL->hContainerProcessStartEvent);
  }

  if(CleanupCtnrEvt)
  {
    DALSYS_DestroyObject(pCWL->hContainerEvent);
  }
  
  if(CleanupWLAdd)
  {
    DALSYS_DeleteEventFromWorkLoop(pCWL->hWorkLoop,
                                   pCWL->hWorkLoopEvent);
  }
  
  if(CleanupWLEvt)
  {
    DALSYS_DestroyObject(pCWL->hWorkLoopEvent);
  }

  if(CleanupWorkLoop)
  {
    /* destroy object is not "recommended" for workloops, but it doesn't say
     * you can't use it.  it seems like this function should manage adding an
     * event to the workloop that does nothing but exit with an error code if
     * that's really the "right" way to kill off a workloop. */
    DALSYS_DestroyObject(pCWL->hWorkLoop);
  }
  
  if(CleanupMalloc)
  {
    DALSYS_Free(*phContainerWorkLoop);
    *phContainerWorkLoop = NULL;
  }

  return RetVal;
}

 DALResult Core_AddContainerToWorkLoop(CoreWorkContainerHandle     hContainer,
                                       CoreContainerWorkLoopHandle hWorkLoop)
 {
   CoreContainerWorkLoopType *pCWL = (CoreContainerWorkLoopType *)hWorkLoop;
   CoreWorkContainerType     *pWC = (CoreWorkContainerType *)hContainer;
   CoreWorkContainerType     *pInsertPoint;

   /* Check for bad arguments. */
   if((NULL == pWC) || (NULL == pCWL) || (NULL == pCWL->hContainerListLock))
     return DAL_ERROR;

   /* Check that this container isn't already a part of a workloop. */
   if((NULL != pWC->hWorkLoop) || (NULL != pWC->hLink))
     return DAL_ERROR;

   /* this operation should be ok--lock the workloop and append the container */
   DALSYS_SyncEnter(pCWL->hContainerListLock);

   /* find where to put the container */
   if(NULL == pCWL->hContainerList)
   {
     /* first container added! */
     pCWL->hContainerList = hContainer;
   }
   else
   {
     /* find the last container in the loop */
     pInsertPoint = (CoreWorkContainerType *)pCWL->hContainerList;
     while(pInsertPoint->hLink)
       pInsertPoint = (CoreWorkContainerType *)pInsertPoint->hLink;

     /* found it--now insert */
     pInsertPoint->hLink = hContainer;
   }

   /* also populate the back-link */
   pWC->hWorkLoop = hWorkLoop;

   DALSYS_SyncLeave(pCWL->hContainerListLock);

   return DAL_SUCCESS;
 }
#endif

