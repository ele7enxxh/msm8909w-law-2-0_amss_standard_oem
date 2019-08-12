/*======================================================

FILE:  DSS_NetworkStateHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_NetworkStateHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetworkStateHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "DSS_NetworkStateHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkPrivScope.h"
#include "DSS_IDSNetworkScope.h"

using namespace ds::Net;

DSSNetworkStateHandler::DSSNetworkStateHandler() :
  DSSEventHandlerNetApp()
{
  mEventType = EVENT_HANDLER_NETWORK_STATE;
}

void DSSNetworkStateHandler::EventOccurred()
{
  DSSIDSNetworkPrivScope IDSNetworkPrivScope;
  DSSIDSNetworkScope IDSNetworkScope;
  dss_iface_ioctl_event_info_union_type eventInfo;
  NetworkStateType previousNetState = NetworkState::QDS_CLOSED;
  AEEResult res = AEE_SUCCESS;

  if (mpHandlerDataClone->mpEventData->bReg) {
    DSSWeakRefScope WeakRefScope;

    if(!WeakRefScope.Init(mpParent))
    {
      return;
    }

    res = IDSNetworkScope.Init(mpParent);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSNetworkStateHandler::EventOccurred(): "
                      "IDSNetworkScope.Init() failed: %d", res);
      return;
    }

    LOG_MSG_INFO1_1("DSSNetworkStateHandler::EventOccurred(): "
                    "Event occurred = %d", mEvent);
    
    res = IDSNetworkPrivScope.Init(mpParent);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSNetworkStateHandler::EventOccurred(): "
                      "IDSNetworkPrivScope.Init() failed: %d", res);
      return;
    }

    res = IDSNetworkPrivScope.Fetch()->GetPreviousState(&previousNetState);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSNetworkStateHandler::EventOccurred(): "
                      "GetPreviousState() failed: %d", res);
      return;
    }

    // convert net state to iface state
    eventInfo.iface_state_info = DSSConversion::NetState2IfaceState(previousNetState);

    DispatchCB(mEvent, mpHandlerDataClone->mpEventData, &eventInfo);
  }
}

AEEResult DSSNetworkStateHandler::RegisterIDL()
{
  DSSIDSNetworkPrivScope IDSNetworkPrivScope;
  DSSWeakRefScope WeakRefScope;
  IQI* pRegObj = NULL;

  if(!WeakRefScope.Init(mpParent)) {
    return AEE_EFAILED;
  }

  IDS_ERR_RET(IDSNetworkPrivScope.Init(mpParent));

  mRegisteredIDLEvent = DSSConversion::Event2DsnetEvent(mEvent);

  LOG_MSG_INFO1_2("DSSNetworkStateHandler::RegisterIDL(): "
                  "Registering to event id %d , NetworkPriv obj 0x%p",
                  mRegisteredIDLEvent, IDSNetworkPrivScope.Fetch());

  AEEResult res = IDSNetworkPrivScope.Fetch()->OnStateChange(mpSignal, mRegisteredIDLEvent, &pRegObj);
  // regObj mechanism is currently not in effect by dsnet layer. 
  // No need for DSS to hold the allocated object to maintain the event registration. 
  // For design simplicity we release the regObj immediately.
  // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
  DSSCommon::ReleaseIf(&pRegObj);

  return res;
}

DSSNetworkStateHandler* DSSNetworkStateHandler::CreateInstance()
{
  return new DSSNetworkStateHandler;
}

