/*=============================================================================

  FILE:   I2cSys.c

  OVERVIEW: This file contains the implementation for the QUP OS Services 
            Interface: IQupI2cDevice 
 
            Copyright (c) 2011 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
  ===========================================================================*/
/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/i2c/src/dev/I2cSys.c#1 $
  $DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  09/26/11 LK     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cSys.h"
#include "DALStdDef.h"
#include "DALDeviceId.h"
#include "DALSys.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/

typedef enum I2CSYS_Error
{
   I2CSYS_ERROR_BASE = I2CSYS_RESULT_FIRST_SECTION,

   I2CSYS_ERROR_TIMEOUT_EVT_CREATE_FAILED,
   I2CSYS_ERROR_CLIENT_EVT_CREATE_FAILED,

   I2CSYS_ERROR_MEM_FREE_FAILED,
   I2CSYS_ERROR_MEM_MALLOC_FAILED,
   I2CSYS_ERROR_PHYS_MEM_MALLOC_FAILED,
   I2CSYS_ERROR_PHYS_MEM_INFO_FAILED,
   I2CSYS_ERROR_MEM_SET_FAILED,
   I2CSYS_ERROR_TIMEOUT_EVT_CLEAR_FAILED,
   I2CSYS_ERROR_CLIENT_EVT_CLEAR_FAILED,
   I2CSYS_ERROR_EVT_CTRL_FAILED,
   I2CSYS_ERROR_EVT_WAIT_FAILED,
   I2CSYS_ERROR_SYNC_CREATE_FAILED,
   I2CSYS_ERROR_SYNC_DESTROY_FAILED,
   I2CSYS_ERROR_INVALID_HANDLE_TYPE,
   I2CSYS_ERROR_NULL_PTR,

} I2CSYS_Error;


typedef enum I2cOs_HandleType
{
   I2cOs_EventHandleType,
   I2cOs_CritSecHandleType,
} I2cOs_HandleType;

typedef struct I2cOs_EventHandle
{
   I2cOs_HandleType    hType;
   DALSYSEventHandle      hQupEvents[2];
   DALSYSEventObj         aQupEventObj[2];

} I2cOs_EventHandle;

typedef struct I2cOs_SyncHandle
{
   I2cOs_HandleType    hType;
   DALSYSSyncHandle  hDeviceSynchronization;
   DALSYS_SYNC_OBJECT(DeviceSyncObject);

} I2cOs_SyncHandle;


typedef struct I2cOs_Handle
{
   I2cOs_HandleType    hType;
} I2cOs_Handle;


/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Creates an event object and sets handle pointer.

    @param[out] pEventHandle  Pointer to event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateEvent(I2CSYS_EVENT_HANDLE *phEvt)
{
#if 0
   int32 res;
   I2cOs_EventHandle *pEvt;
   DALResult dalRes;
   enum I2CSYS_CreatEventState
   {
      I2CSYS_CreatEventState_Reset,
      I2CSYS_CreatEventState_Malloced,
      I2CSYS_CreatEventState_1stEvt,
      I2CSYS_CreatEventState_2ndEvt,
   }crEvtState;

   crEvtState = I2CSYS_CreatEventState_Reset;
   do {
      res = I2CSYS_Malloc((void **)&pEvt, sizeof(I2cOs_EventHandle));
      if ( I2CSYS_RESULT_OK != res ) {
         return res;
      }
      crEvtState = I2CSYS_CreatEventState_Malloced;

      /* Create a timout object used to timeout operations.               */
      dalRes = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT,
                                  &pEvt->hQupEvents[0],
                                  &pEvt->aQupEventObj[0]);
      if ( dalRes != DAL_SUCCESS ) {
         res = I2CSYS_ERROR_TIMEOUT_EVT_CREATE_FAILED;
         break; 
      }
      crEvtState = I2CSYS_CreatEventState_1stEvt;

      /* Create a wait object on which the client will wait.              */
      dalRes = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                                  &pEvt->hQupEvents[1],
                                  &pEvt->aQupEventObj[1]);
      if ( dalRes != DAL_SUCCESS ) {
         res = I2CSYS_ERROR_CLIENT_EVT_CREATE_FAILED;
         break; 
      }
      pEvt->hType = I2cOs_EventHandleType;
      crEvtState = I2CSYS_CreatEventState_2ndEvt;
   } while ( 0 );

   if ( res != I2CSYS_RESULT_OK ) {
      switch ( crEvtState ) {
         case I2CSYS_CreatEventState_1stEvt:
            DALSYS_DestroyObject(&pEvt->aQupEventObj[0]);
         case I2CSYS_CreatEventState_Malloced:
            I2CSYS_Free(pEvt);
            break;
          /* unreachable added to avoid compiler warnings. */
         case I2CSYS_CreatEventState_Reset:
         case I2CSYS_CreatEventState_2ndEvt:
           break;
      }
   }
   else {
      *phEvt = (I2CSYS_EVENT_HANDLE) pEvt;
   }

   return res;
#endif
   return I2CSYS_RESULT_OK;
}



int32
I2CSYS_DestroyEvent(I2cOs_EventHandle *pEvt)
{
#if 0        
   int32 res;

   res = I2CSYS_Free(pEvt);
   return res;
#endif
   return I2CSYS_RESULT_OK;
}


/** @brief Signals the event object pointed to by the handle.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_SetEvent
(
   I2CSYS_EVENT_HANDLE hEvent
)
{
   /*     
   DALResult dalRes;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvent;

   dalRes = DALSYS_EventCtrl(pEvt->hQupEvents[1], DALSYS_EVENT_CTRL_TRIGGER);
   if (dalRes != DAL_SUCCESS) {
      return I2CSYS_ERROR_EVT_CTRL_FAILED;
   }
   */
   return I2CSYS_RESULT_OK;
}

/** @brief Clears outstanding signals on the event object.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_ClearEvent
(
   I2CSYS_EVENT_HANDLE hEvt
)
{
#if 0        
   DALResult dalRes;
   int32 res = I2CSYS_RESULT_OK;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvt;

   /* TODO: THIS IS A BUG IN DAL, IT FAILS TO CLEAR THE TIMEOUT EVENT
   dalRes = DALSYS_EventCtrl(pEvt->hQupEvents[0], DALSYS_EVENT_CTRL_RESET);
   if (dalRes != DAL_SUCCESS) {
      res = I2CSYS_ERROR_TIMEOUT_EVT_CLEAR_FAILED;
   }
   */
   dalRes = DALSYS_EventCtrl(pEvt->hQupEvents[1], DALSYS_EVENT_CTRL_RESET);
   if ( dalRes != DAL_SUCCESS ) {
      res = I2CSYS_ERROR_CLIENT_EVT_CLEAR_FAILED;
   }

   return res;
#endif
   return I2CSYS_RESULT_OK;
}

/** @brief Wait with a timeout on the event object.

    @param[in] hEvent  Event handle.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Wait
(
   I2CSYS_EVENT_HANDLE hEvent,
   uint32 dwMilliseconds
)
{
/*
   DALResult dalRes;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvent;
   
   dalRes = DALSYS_EventWait(pEvt->hQupEvents[1]);
   if(DAL_SUCCESS != dalRes) {
      return I2CSYS_ERROR_EVT_WAIT_FAILED;
   }
*/
   return I2CSYS_RESULT_OK;

#if 0
   DALResult dalRes;
   I2cOs_EventHandle *pEvt = (I2cOs_EventHandle *)hEvent;

   uint32 uEvtIdx;

   
   dalRes = DALSYS_EventMultipleWait(pEvt->hQupEvents, 2,
                                  dwMilliseconds*1000, &uEvtIdx);

   if(((DAL_SUCCESS != dalRes) && (DAL_ERROR_TIMEOUT != dalRes) )
         || (uEvtIdx > 1)){
         return I2CSYS_ERROR_EVT_WAIT_FAILED;
   }
   if((DAL_ERROR_TIMEOUT == dalRes) || (0 == uEvtIdx)){  //Timeout event.
      return I2CSYS_RESULT_ERROR_EVT_WAIT_TIMEOUT;
   }
      
   return I2CSYS_RESULT_OK;
#endif
}


int32
I2CSYS_DestroyCriticalSection(I2cOs_SyncHandle *pCritSec)
{
   int32 res = I2CSYS_RESULT_OK;
   DALResult dalRes;

   dalRes = DALSYS_DestroyObject( pCritSec->hDeviceSynchronization );
   if (dalRes != DAL_SUCCESS) {
      res = I2CSYS_ERROR_SYNC_DESTROY_FAILED;
   }
   if (I2CSYS_RESULT_OK == res) {
      res = I2CSYS_Free(pCritSec);
   }
   else {
      I2CSYS_Free(pCritSec);
   }

   return res;
}


/** @brief Closes a handle destroying the object associated with it.

    @param[in] hEvent  Event handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CloseHandle(I2CSYS_EVENT_HANDLE hEvt)
{
   I2cOs_Handle *hOsEvt = (I2cOs_Handle *)hEvt;

   if (I2cOs_EventHandleType == hOsEvt->hType) {
      return I2CSYS_DestroyEvent((I2cOs_EventHandle *)hEvt);
   }
   else if (I2cOs_CritSecHandleType == hOsEvt->hType) {
      return I2CSYS_DestroyCriticalSection((I2cOs_SyncHandle *)hEvt);
   }
   else {
      return I2CSYS_ERROR_INVALID_HANDLE_TYPE;
   }
}

/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSecHandle  Pointer to critical section handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_CreateCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE *pCritSecHandle
)
{
   I2cOs_SyncHandle *pCritSec ;
   int32 res = I2CSYS_RESULT_OK;

   res = I2CSYS_Malloc((void **)&pCritSec,sizeof(I2cOs_SyncHandle));
   if ( I2CSYS_RESULT_OK != res ) {
      return res;
   }

   if ( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                                         &(pCritSec->hDeviceSynchronization),
                                         &(pCritSec->DeviceSyncObject) )) {
      I2CSYS_Free(pCritSec);
      res = I2CSYS_ERROR_SYNC_CREATE_FAILED;
   }
   else {
       pCritSec->hType = I2cOs_CritSecHandleType;
       *pCritSecHandle = (I2CSYS_CRITSECTION_HANDLE)pCritSec;
   }

   return res;
}

/** @brief Enters the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_EnterCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
)
{
   I2cOs_SyncHandle *pCritSec = (I2cOs_SyncHandle *)hCriticalSection;
   /* Enter the Critical Section now */
  (void)DALSYS_SyncEnter(pCritSec->hDeviceSynchronization);
   return I2CSYS_RESULT_OK;
}

/** @brief Leaves the critical section.

    @param[in] hCriticalSection  Critical section handle handle.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_LeaveCriticalSection
(
   I2CSYS_CRITSECTION_HANDLE hCriticalSection
)
{
   I2cOs_SyncHandle *pCritSec = (I2cOs_SyncHandle *)hCriticalSection;
   (void)DALSYS_SyncLeave(pCritSec->hDeviceSynchronization);
   return I2CSYS_RESULT_OK;
}

/** @brief Allocates a memory buffer of the given size and sets the pointer to it.

    @param[out] ppBuffer pointer to pointer to allocated memory.
    @param[in]  size     size of the memory to be allocated.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Malloc
(
   void   **pBuffer,
   uint32   size
)
{
   DALResult dalRes;
   void *pTmpBuffer;

   dalRes = DALSYS_Malloc(size , &pTmpBuffer);
   if ( (DAL_SUCCESS != dalRes) || (NULL == pTmpBuffer) ) {
      return I2CSYS_ERROR_MEM_MALLOC_FAILED;
   }

   *pBuffer = pTmpBuffer;
   return I2CSYS_RESULT_OK;
}


/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to buffer.
    @param[in] c        character byte to init the memory.
    @param[in] size     the size of the buffer. 
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Memset
(
   void  *pBuffer,
   int32  c,
   uint32 size
)
{
   DALSYS_memset(pBuffer, c, size);
   return I2CSYS_RESULT_OK;
}

/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to allocated memory.
    @return          I2CSYS_Result .
  */
int32
I2CSYS_Free
(
   void *pBuffer
)
{
   DALResult dalRes;
  
   dalRes = DALSYS_Free(pBuffer);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CSYS_ERROR_MEM_FREE_FAILED;
   }
 
   return I2CSYS_RESULT_OK;
}

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @param[in] uTimeMicrosec  wait time in microseconds.
  @return          Nothing.
  */
void
I2CSYS_BusyWait
(
   uint32 uTimeMicrosec
)
{
   DALSYS_BusyWait( uTimeMicrosec );
}

#if 0
/**
   @brief Allocate physical memory
 
   This function allocates physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemAlloc
(
   I2CSYS_PhysMemDesc *pPhysMem
)
{
   int32 res;   
   DALSYSMemInfo MemInfo;

   if ( NULL == pPhysMem ) {
      return I2CSYS_ERROR_NULL_PTR;
   }

   res  = DALSYS_MemRegionAlloc(
                    DALSYS_MEM_PROPS_UNCACHED|DALSYS_MEM_PROPS_PHYS_CONT,
                    DALSYS_MEM_ADDR_NOT_SPECIFIED,
                    DALSYS_MEM_ADDR_NOT_SPECIFIED,
                    pPhysMem->uSize, &pPhysMem->pObj, NULL);
      
   if ( res != DAL_SUCCESS ) {
      return I2CSYS_ERROR_PHYS_MEM_MALLOC_FAILED;
   }
   
   res = DALSYS_MemInfo(pPhysMem->pObj, &MemInfo);

   if ( res != DAL_SUCCESS ) {
      I2CSYS_PhysMemFree(pPhysMem->pObj);
      return I2CSYS_ERROR_PHYS_MEM_INFO_FAILED;
   }

   pPhysMem->pVirtAddress = (uint8*)MemInfo.VirtualAddr;
   pPhysMem->pPhysAddress = (uint8*)MemInfo.PhysicalAddr;
   
   return  I2CSYS_RESULT_OK;
}


/**
   @brief Release physical memory
 
   This function releases physical memory
  
   @param[in] pPhysMem  Pointer to physical memory structure.
 
   @return I2CSYS_RESULT_OK on success, error code on error
 */
int32
I2CSYS_PhysMemFree
(
   I2CSYS_PhysMemDesc *pPhysMem
)
{
   int32 res;   

   if ( NULL == pPhysMem ) {
      return I2CSYS_ERROR_NULL_PTR;
   }

   res  = DALSYS_DestroyObject(pPhysMem->pObj);
   if ( res != DAL_SUCCESS ) {
      return I2CSYS_ERROR_MEM_FREE_FAILED;
   }
   
   return   I2CSYS_RESULT_OK;
}

#endif



