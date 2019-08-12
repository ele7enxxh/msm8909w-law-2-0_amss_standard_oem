/*=============================================================================

  FILE:   I2cSys.c

  OVERVIEW: This file contains the implementation for the QUP OS Services 
            Interface: IQupI2cDevice 
 
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Confidential and Proprietary
  ===========================================================================*/
/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cSys.c#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  08/25/15 VG     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cSys.h"
#include "busywait.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Creates an event object.

    @param[out] pEvent  Pointer to event object.
    @return          I2cSys_Result .
  */
int32
I2cSys_CreateEvent
(
   I2cSys_EventType *pEvent
)
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



/** @brief Destroys an event object.
 
 @param[out] pEvent  Pointer to event object.
 @return          I2cSys_Result .
 */
int32
I2cSys_DestroyEvent
(
   I2cSys_EventType *pEvent
)
{
#if 0        
   int32 res;

   res = I2CSYS_Free(pEvt);
   return res;
#endif
   return I2CSYS_RESULT_OK;
}


/** @brief Signals the event object pointed to by the handle.

    @param[in] pEvent  Pointer to event.
    @return           I2cSys_Result .
  */
int32
I2cSys_SetEvent
(
   I2cSys_EventType *pEvent
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

    @param[in] pEvent  Pointer to event.
    @return          I2cSys_Result .
  */
int32
I2cSys_ClearEvent
(
   I2cSys_EventType *pEvent
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

    @param[in] pEvent  Pointer to event.
    @param[in] dwMilliseconds  Event wait timeout.
    @return          I2cSys_Result .
  */
int32
I2cSys_Wait
(
   I2cSys_EventType   *pEvent,
   uint32              dwMilliseconds
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

/** @brief Destroys a critical section object and sets the handle pointer.
 
 @param[out] pCritSec  Pointer to critical section object.
 @return          I2cSys_Result .
 */
int32
I2cSys_DestroyCriticalSection
(
   I2cSys_SyncType    *pCritSec
)
{
   qurt_mutex_destroy(&pCritSec->i2c_mutex);
   return I2CSYS_RESULT_OK;
}


/** @brief Creates a critical section object and sets the handle pointer.

    @param[out] pCritSec  Pointer to critical section object.
    @return          I2cSys_Result .
  */
int32
I2cSys_CreateCriticalSection
(
   I2cSys_SyncType    *pCritSec
)
{
   qurt_mutex_init(&pCritSec->i2c_mutex);
   return I2CSYS_RESULT_OK;
}

/** @brief Enters the critical section.

    @param[in] pCritSection  Pointer to critical section object.
    @return          I2cSys_Result .
  */
int32
I2cSys_EnterCriticalSection
(
   I2cSys_SyncType    *pCritSec
)
{
   qurt_mutex_lock(&pCritSec->i2c_mutex);
   return I2CSYS_RESULT_OK;
}

/** @brief Leaves the critical section.

    @param[in] pCritSec   Pointer to critical section object.
    @return          I2cSys_Result .
  */
int32
I2cSys_LeaveCriticalSection
(
   I2cSys_SyncType    *pCritSec
)
{
   qurt_mutex_unlock(&pCritSec->i2c_mutex);
   return I2CSYS_RESULT_OK;
}


/** @brief Frees memory allocated by I2CSYS_Malloc.

    @param[in] pBuffer  pointer to buffer.
    @param[in] c        character byte to init the memory.
    @param[in] size     the size of the buffer. 
    @return          I2CSYS_Result .
  */
int32
I2cSys_Memset
(
   void  *pBuffer,
   int32  c,
   uint32 size
)
{
   //DALSYS_memset(pBuffer, c, size);
   return I2CSYS_RESULT_OK;
}

/** @brief Busy waits the uTimeMicrosec microseconds before returning.

  @param[in] uTimeMicrosec  wait time in microseconds.
  @return          Nothing.
  */
void
I2cSys_BusyWait
(
   uint32 uTimeMicrosec
)
{
   busywait (uTimeMicrosec);
}





