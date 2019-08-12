#ifndef COREWORKCONTAINER_H
#define COREWORKCONTAINER_H

/*==============================================================================

FILE:      CoreWorkContainer.h

DESCRIPTION: Core Container Workloop API.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/rpm.bf/2.1.1/core/power/utils/inc/CoreWorkContainer.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------

==============================================================================*/

#include "DALSysTypes.h"

/* declare struct handle types */
typedef struct CoreContainerWorkLoopType *     CoreContainerWorkLoopHandle;
typedef struct CoreWorkContainerType *         CoreWorkContainerHandle;

typedef DALResult (*CoreContainerPutFcn) (CoreWorkContainerHandle hContainer, void *pData);
typedef DALResult (*CoreContainerGetFcn) (CoreWorkContainerHandle hContainer, void **ppData);
typedef DALResult (*CoreContainerProcessFcn)(void *pData);
typedef DALResult (*CoreContainerFreeFcn)(CoreWorkContainerHandle hContainer,void *pData);

typedef struct CoreWorkContainerType
{
  CoreContainerPutFcn          pfnPut;        /* function for adding an element to this container */
  CoreContainerGetFcn          pfnGet;        /* function for retrieving the next element from this container */
  CoreContainerProcessFcn      pfnProcess;    /* (optional) function for processing an element from this container */
  CoreContainerFreeFcn         pfnFree;       /* (optional) function for freeing resources of an element after processing */
  DALSYSSyncHandle             hLock;         /* lock object to protect operations on this container */
  CoreContainerWorkLoopHandle  hWorkLoop;     /* the container-workloop this workloop is a part of */
  CoreWorkContainerHandle      hLink;         /* link pointer for when containers are added to workloops */
  void                       * pCntrData;     /* placeholder for container-type-specific data */
} CoreWorkContainerType;

typedef struct CoreContainerWorkLoopType
{
  DALSYSWorkLoopHandle    hWorkLoop;                   /* Handle to the Container WorkLoop */
  DALSYSEventHandle       hWorkLoopEvent;              /* Used to trigger the infinite loop in the WorkLoop */
  DALSYSEventHandle       hContainerEvent;             /* Used to wait on new container items */
  DALSYSEventHandle       hContainerProcessStartEvent; /* Used to sync the calling context and the WorkLoop */
  DALSYSSyncHandle        hContainerListLock;          /* Lock to protect the container list */
  CoreWorkContainerHandle hContainerList;              /* Handle to the container list */
} CoreContainerWorkLoopType;

/*
  @brief Core_RegisterContainerWorkLoop - create a container workloop

  This function creates a Container WorkLoop, a subclass of the normal DALSYS
  WorkLoop that supports the processing of items from multiple Work Containers.

  Users should create a Container WorkLoop and one or more Work Containers, 
  such as a Work Queue, and call Core_AddContainerToWorkLoop() to 
  associate the created container(s) with the Container WorkLoop. 
  Then users can put items into the container(s) for them to be processed 
  by the Container WorkLoop.

  Example:
    Core_WorkQueueCreate(&hWkC, <Process Fcn>);

    Core_WorkQueuePreallocate(hWkC, <...>, <...>, <Data Size>);

    Core_RegisterContainerWorkLoop(<Priority>, &hWL);

    Core_AddContainerToWorkLoop(hWkC, hWL);

    ......

    // Allocate a buffer from the preallocated memory pool.
    Core_WorkQueueAlloc(hWkC, <pointer to item handle>);

    // Initialize the item data
    ......

    // Put the item into the Work Container (Work Queue)
    Core_WorkQueuePut(hWkC, <item handle>);
  
  @param dwPriority          : Priority of this execution context
  @param phContainerWorkLoop : Return parameter, points to the WorkLoop Handle
                               upon successful invocation
*/
DALResult Core_RegisterContainerWorkLoop(uint32                        dwPriority,
                                         CoreContainerWorkLoopHandle  *phContainerWorkLoop);


/*
  @brief Core_AddContainerToWorkLoop - add a Work Container to a Container
  WorkLoop

  The order in which containers are added determines the priority in which 
  the work loop processes items from the containers.  That is:
    - If the first container added has an item that item is processed.
    - Otherwise if the second container has an item, that item is proceesed.
    - This pattern repeats, onward to the nth container.
    - After an item is processed, the workloop restarts at the first container.

  @param hContainer : handle to the Work Container
  @param hWorkLoop  : handle to the Container WorkLoop
*/
DALResult Core_AddContainerToWorkLoop(CoreWorkContainerHandle         hContainer,
                                      CoreContainerWorkLoopHandle     hWorkLoop);
/*
  @brief Core_WorkContainerProcess - takes a single Container and processes it

  @param *pWC  : pointer to the work container to be processed
*/
DALResult Core_WorkContainerProcess(CoreWorkContainerType *pWC);

/*
  @brief Core_WorkElementProcess - takes a single Container and processes
  the first element in the container 

  @param *pWC  : pointer to the work container to be processed
*/
DALResult Core_WorkElementProcess(CoreWorkContainerType *pWC);


#endif /* COREWORKCONTAINER_H */

