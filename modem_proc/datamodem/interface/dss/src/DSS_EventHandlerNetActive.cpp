/*======================================================

FILE:  DSS_EventHandlerNetActive.cpp

GENERAL DESCRIPTION:
   Implementation of DSSEventHandlerNetActive functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerNetActive.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 yl  File created.

===========================================================================*/

#include "DSS_EventHandlerNetActive.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"
#include "ds_Utils_CreateInstance.h"
#include "DSS_WeakRefScope.h"

void DSSEventHandlerNetActive::DispatchCB(
  dss_iface_ioctl_event_enum_type event, EventData* ped,
  dss_iface_ioctl_event_info_union_type* eventInfo)
{
  sint15 netHandle = -1;
  dss_iface_id_type ifaceId = DSS_IFACE_INVALID_ID;

  LOG_MSG_INFO1_1("DSSEventHandlerNetActive::DispatchCB(): "
                  "DispachCB of event number %d", event);
  
  if(NULL != ped)
  {
    // In case iface_id was cleared by IFACE_DOWN event
    // fetch the stale value.
    // Also, build iface id with policy bit on instead of app ID
    // in accordance with registration
    mpDSSNetActive->GetIfaceIdForEventInfo(ped->appId, &ifaceId);

    mpDSSNetActive->GetNetHandle(&netHandle);

    LOG_MSG_INFO1_3("DSSEventHandlerNetActive::DispatchCB(): "
                    "DSS called App callback, event: %d, ifaceId: %u, netHandle: %d",
                    event, ifaceId, netHandle);
     if((NULL != ped) && (NULL != ped->userCB))
     {
        ped->userCB(event, *eventInfo, ped->userData, netHandle, ifaceId);
        LOG_MSG_INFO1_3("DSSEventHandlerNetActive::DispatchCB(): "
                      "DSS returned from App callback, event: %d, ifaceId: %u, netHandle: %d",
                      event, ifaceId, netHandle);
     }
  }
}

AEEResult DSSEventHandlerNetActive::Init(DSSNetActive* parentNetAppParam)
{
  ISignalFactory *piSignalFactory = NULL;

  mpDSSNetActive = parentNetAppParam;
  (void) mpDSSNetActive->AddRefWeak();

  IDS_ERR_RET(DSSGlobals::Instance()->GetSignalFactory(&piSignalFactory));
  IDS_ERR_RET(piSignalFactory->CreateSignal(
    &signalHandler,
    SignalCB,
    this,
    &mpSignal,
    &mpSignalCtl));

  IDS_ERR_RET(DSSGlobals::Instance()->GetCritSect(&mpCritSect));

  // piSignalFactory is singleton and does not require a release

  return AEE_SUCCESS;
}

void DSSEventHandlerNetActive::Destructor() throw()
{
  DS_UTILS_RELEASE_WEAKREF_IF(mpDSSNetActive);

  DSSEventHandler::Destructor();
}

