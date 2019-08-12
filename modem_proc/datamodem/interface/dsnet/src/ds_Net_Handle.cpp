/*==========================================================================*/
/*!
  @file
  ds_Net_Handle.cpp

  @brief
  This file provides the implementation of the ds::Net::Handle object.

  @see  ds_Net_Handle.h

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Handle.cpp#1 $$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_Handle.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_Utils.h"
#include "ds_Utils_CCritSect.h"

using namespace ds::Net;
using namespace ds::Utils;
using namespace ds::Error;

/*===========================================================================

                      PROTECTED MEMBER FUNCTIONS

===========================================================================*/
Handle::Handle
(
  int32  objHandle
)
throw()
: mObjHandle(objHandle),
  mpParentFactory(0),
  mpICritSect(0),
  mStaleIfaceHandle(0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* Handle::Handle() */

ds::ErrorType Handle::Init
(
  ds::Utils::IFactory *pIFactory
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Handle::Init(): Obj 0x%p, factory 0x%p",
                            this, pIFactory);
  if (NULL == pIFactory)
  {
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Create critical sect for this object
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL,
                                              AEECLSID_CCritSect,
                                              (void **) &mpICritSect))
  {
    LOG_MSG_ERROR_1 ("Handle::Init(): Failed to alloc CritSect, obj 0x%p",
                     this);
    return AEE_ENOMEMORY;
  }

  /*-------------------------------------------------------------------------
    Add self to parent factory
  -------------------------------------------------------------------------*/
  mpParentFactory = pIFactory;

  (void) mpParentFactory->AddItem (static_cast <ds::Utils::INode *> (this));

  return AEE_SUCCESS;
} /* Handle::Init() */

void Handle::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Handle::Destructor(): Obj 0x%p mpICritSect 0x%x", this, mpICritSect);

  /*-------------------------------------------------------------------------
    Due to 2 stage constructor pattern - Handle::Init can fail to create 
    Critical Section and then it remains uninitialized.
    No need to remove this object from factory list as it was not added
    in Handle::Init()
  -------------------------------------------------------------------------*/
  if (0 == mpICritSect)
  {
    return;
  }

  mpICritSect->Enter();

  mObjHandle        = 0;
  mStaleIfaceHandle = 0;

  /*-------------------------------------------------------------------------
    Remove self from parent factory
  -------------------------------------------------------------------------*/
  if (NULL != mpParentFactory)
  {
    mpParentFactory->RemoveItem (static_cast <ds::Utils::INode *> (this));
  }

  mpParentFactory = NULL;

  mpICritSect->Leave();
} /* Handle::Destructor() */


Handle::~Handle
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Critical section is in use by the ::Destructor method of
    classes inheriting from Handle class, so mpICritSect can't be
    released in Handle::Destructor
  -------------------------------------------------------------------------*/
  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

} /* Handle::~Handle() */


void Handle::SetHandle
(
  int32 objHandle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  if (0 == objHandle)
  {
    /*-----------------------------------------------------------------------
      Defensive check so that if SetHandle(0) is called back to back,
      mStaleIfaceHandle is not set to 0
    -----------------------------------------------------------------------*/
    if (0 != mObjHandle)
    {
      mStaleIfaceHandle = mObjHandle;
    }
  }
  else
  {
    mStaleIfaceHandle = 0;
  }

  mObjHandle = objHandle;

  LOG_MSG_FUNCTION_EXIT_3 ("Handle::SetHandle(): "
                           "Obj 0x%p, handle 0x%x staleHandle 0x%x",
                           this, mObjHandle, mStaleIfaceHandle);
  mpICritSect->Leave();

  return;
} /* Handle::SetHandle() */

/*---------------------------------------------------------------------------
  Inherited function definitions.
---------------------------------------------------------------------------*/
ds::ErrorType Handle::OnStateChange
(
  ISignal *           pISignal,
  ds::Net::EventType  eventName,
  IQI **              regObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return RegEvent(pISignal, eventName, regObj);

} /* OnStateChange() */

ds::ErrorType Handle::RegEvent
(
  ISignal *    pISignal,
  int32        eventName,
  IQI **       regObj
)
{
  ISignalBus *   pISigBus = NULL;
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("Handle::RegEvent(): Obj 0x%p, event 0x%x, sig 0x%p",
                            this, eventName, pISignal);

  if (NULL == pISignal)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    result = GetSignalBus (eventName, &pISigBus);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    result = pISigBus->Add(pISignal);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Due to a possible security issue where applications may invoke this
      function many times thus causing undesirable extra memory allocation
      that may be entailed in an implementation of socket events registration,
      we should return an object that the application will hold for each
      RegEvent invocation. A Limit that CS imposes on the number of objects
      the application can hold resolves the security issue.

      If the implementation of RegEvent is changed an appropriate
      object must be returned.
    -----------------------------------------------------------------------*/
    *regObj = new RegBound();
  } while (0);

  DS_UTILS_RELEASEIF(pISigBus);

  LOG_MSG_FUNCTION_EXIT_2 ("Handle::RegEvent(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* Handle::RegEvent() */

void Handle::Notify
(
  int32  eventName
)
{
  ISignalBus *   pISigBus = 0;
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  do
  {
    result = GetSignalBus (eventName, &pISigBus);
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Strobe the signal bus to notify
    -----------------------------------------------------------------------*/
    result = pISigBus->Strobe();
    if (AEE_SUCCESS != result)
    {
      break;
    }
  } while (0);

  DS_UTILS_RELEASEIF(pISigBus);

  LOG_MSG_FUNCTION_EXIT_3 ("Handle::Notify(): Result 0x%x, obj 0x%p event 0x%x",
                           result, this, eventName);
  mpICritSect->Leave();

  return;
} /* Handle::Notify() */

boolean Handle::Process
(
  void *  pUserData
)
{
  EventInfoType *  pEventInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    User data should never be NULL for event processing.
  -------------------------------------------------------------------------*/
  if (0 == pUserData)
  {
    LOG_MSG_ERROR_1 ("Handle::Process(): NULL event info, obj 0x%p", this);
    return FALSE;
  }

  pEventInfo = static_cast <EventInfoType *> (pUserData);

  mpICritSect->Enter();

  LOG_MSG_INFO1_4 ("Handle::Process(): "
                   "Handle 0x%x, event 0x%x, event handle 0x%x, obj 0x%p",
                   mObjHandle,
                   pEventInfo->eventName,
                   pEventInfo->handle,
                   this);

  if (mObjHandle == pEventInfo->handle)
  {
    /*-----------------------------------------------------------------------
      Event belongs to this handle. Call Notify()
    -----------------------------------------------------------------------*/
    Notify (pEventInfo->eventName);
  }

  mpICritSect->Leave();
  return TRUE;

} /* Handle::Process() */
