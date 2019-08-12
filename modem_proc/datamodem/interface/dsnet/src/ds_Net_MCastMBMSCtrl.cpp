/*===========================================================================
  FILE: MCastMBMSCtrl.cpp

  OVERVIEW: This file provides implementation of the MCastMBMSCtrl class.

  DEPENDENCIES: None

  Copyright (c) 2007 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_MCastMBMSCtrl.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-04-07 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_MCastMBMSCtrl.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Net_Utils.h"
#include "ds_Utils_CSignalBus.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

MCastMBMSCtrl::MCastMBMSCtrl
(
  int32    ifaceHandle,
  int32    mbmsHandle
) : Handle (ifaceHandle),
    mMBMSHandle (mbmsHandle),
    refCnt (1),
    weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("MCastMBMSCtrl::MCastMBMSCtrl(): "
                            "Obj 0x%p, iface handle 0x%x, MBMS handle 0x%x",
                            this, ifaceHandle, mbmsHandle);

  //TODO Move out to Init function. Go through constructors of all dsnet/dss
  //objects and fix this
  if (AEE_SUCCESS != DS_Utils_CreateInstance (0,
                                              AEECLSID_CSignalBus,
                                              (void **) &mpSigBusStateChange))
  {
    LOG_MSG_ERROR_1 ("MCastMBMSCtrl::MCastMBMSCtrl(): "
                     "Failed to alloc Signal Bus, obj 0x%p", this);
    return;
  }

  if (AEE_SUCCESS != Handle::Init(EventManager::networkMBMSObjList))
  {
    LOG_MSG_ERROR_3 ("MCastMBMSCtrl::MCastMBMSCtrl: "
                     "Failed to init the Handle   "
                     "obj 0x%p, if handle 0x%x, mMBMSHandle 0x%x",
                     this, ifaceHandle, mMBMSHandle);
    return;
  }

} /* MCastMBMSCtrl::MCastMBMSCtrl() */

void MCastMBMSCtrl::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpSigBusStateChange);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor for the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();

} /* MCastMBMSCtrl::Destructor()() */


MCastMBMSCtrl::~MCastMBMSCtrl
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* MCastMBMSCtrl::~MCastMBMSCtrl() */

/*---------------------------------------------------------------------------
  Functions inherited from IMCastMBMSCtrlPriv
---------------------------------------------------------------------------*/
ds::ErrorType MCastMBMSCtrl::DeActivate
(
  void
)
{
  NetPlatform::MBMSContextDeactType   deActInfo;
  ds::ErrorType                       result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Construct the deactivate IOCTL arg.
  -------------------------------------------------------------------------*/
  memset (&deActInfo, 0, sizeof (deActInfo));

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Perform iface ioctl to de-activate MBMS multicast context.
  -------------------------------------------------------------------------*/
  deActInfo.handle = mMBMSHandle;
  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_MBMS_MCAST_CONTEXT_DEACTIVATE,
                      static_cast <void *> (&deActInfo));

  LOG_MSG_FUNCTION_EXIT_2 ("MCastMBMSCtrl::DeActivate(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastMBMSCtrl::DeActivate() */


ds::ErrorType MCastMBMSCtrl::GetState
(
  MBMSStateType *  status
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Perform iface ioctl to get the MBMS MCast context status.
  -------------------------------------------------------------------------*/
  result = IfaceIoctl (GetHandle(),
                       IFACE_IOCTL_MBMS_MCAST_CONTEXT_GET_STATUS,
                       static_cast <void *> (status));

  LOG_MSG_FUNCTION_EXIT_2 ("MCastMBMSCtrl::GetState(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* MCastMBMSCtrl::GetState() */

ds::ErrorType MCastMBMSCtrl::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastMBMSCtrl::QueryInterface(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_IMCastMBMSCtrlPriv:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <IMCastMBMSCtrlPriv *>(this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("MCastMBMSCtrl::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, *ppo, this ,iid);
  mpICritSect->Leave();

  return AEE_SUCCESS;
} /* MCastMBMSCtrl::QueryInterface() */


ds::ErrorType MCastMBMSCtrl::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("MCastMBMSCtrl::GetSignalBus(): "
                            "Obj 0x%p, event 0x%x, ppISigBus 0x%x",
                             this, eventID, ppISigBus);

  if (0 == ppISigBus)
  {
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case  MBMSEvent::QDS_EV_ACTIVATE_STATE:
    {
      *ppISigBus = mpSigBusStateChange;
      break;
    }

    default:
    {
      result = QDS_EINVAL;
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Handle the case where event is supported by this object.
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS == result)
  {
    if (0 == *ppISigBus)
    {
      LOG_MSG_INFO1_2 ("MCastMBMSCtrl::GetSignalBus(): "
                       "NULL signal bus for event 0x%x, obj 0x%p",
                       eventID, this);
      result = QDS_EINVAL;
    }
    else
    {
      result = AEE_SUCCESS;
      (void) (*ppISigBus)->AddRef();
    }
  }

  LOG_MSG_FUNCTION_EXIT_3 ("MCastMBMSCtrl::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);
  mpICritSect->Leave();

  return result;
} /* MCastMBMSCtrl::GetSignalBus() */
#endif // FEATUTE_DATA_PS_MCAST
