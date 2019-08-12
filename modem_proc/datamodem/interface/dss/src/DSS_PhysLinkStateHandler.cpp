/*======================================================

FILE:  DSS_PhysLinkStateHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSPhysLinkStateHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PhysLinkStateHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "DSS_PhysLinkStateHandler.h"
#include "DSS_Common.h"
#include "DSS_GenScope.h"

using namespace ds::Net;

DSSPhysLinkStateHandler::DSSPhysLinkStateHandler() :
  DSSEventHandlerNetApp()
{
  mEventType = EVENT_HANDLER_PHYS_LINK_STATE;
  mRegisteredIDLEvent = PhysLinkEvent::QDS_EV_STATE_CHANGED;
}

void DSSPhysLinkStateHandler::EventOccurred()
{
  IPhysLink* pPhysLink = NULL;
  DSSGenScope physLinkScope;
  PhysLinkStateType physState = 0;
  PhysLinkStateType previousPhysState = 0;
  dss_iface_ioctl_event_info_union_type eventInfo;
  dss_iface_ioctl_event_enum_type event = DSS_IFACE_IOCTL_MIN_EV;
  AEEResult res = AEE_SUCCESS;

  if (mpHandlerDataClone->mpEventData->bReg)
  {
    DSSWeakRefScope WeakRefScope;

    if(!WeakRefScope.Init(mpParent))
    {
      return;
    }

    res = mpParent->GetPhysLink(&pPhysLink);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSPhysLinkStateHandler::EventOccurred(): "
                      "GetPhysLink() failed: %d", res);
      return;
    }

    physLinkScope.SetParams(pPhysLink,DSSGenScope::IDSIQI_TYPE);

    res = pPhysLink->GetState(&physState);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSPhysLinkStateHandler::EventOccurred(): "
                      "GetState() failed: %d", res);
      return;
    }

    // transform physical link state to event
    event = DSSConversion::PhysLinkState2Event(physState);
    
    // This if statement checks that the event raised
    // is relevant to this event handler object
    if (mEvent != event)
    {
      return;
    }
	
	LOG_MSG_INFO1_1("DSSPhysLinkStateHandler::EventOccurred(): "
                 "Event occurred = %d", mEvent);

    res = pPhysLink->GetPreviousState(&previousPhysState);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSPhysLinkStateHandler::EventOccurred(): "
                      "GetPreviousState() failed: %d", res);
      return;
    }

    eventInfo.phys_link_state_info = DSSConversion::IDS2DSPhysLinkState(previousPhysState);
    
    DispatchCB(event, mpHandlerDataClone->mpEventData, &eventInfo);
  }
}

AEEResult DSSPhysLinkStateHandler::RegisterIDL()
{
   IPhysLink* pPhysLink = NULL;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent))
   {
      return AEE_EFAILED;
   }

   AEEResult res = mpParent->GetPhysLink(&pPhysLink);
   if (AEE_SUCCESS != res)
   {
      LOG_MSG_ERROR_1("DSSPhysLinkStateHandler::RegisterIDL(): "
                      "GetPhysLink() failed: %d", res);
      return res;
   }

   LOG_MSG_INFO1_1("DSSPhysLinkStateHandler::RegisterIDL(): "
                   "Registering to QDS_EV_STATE_CHANGED, PhysLink obj 0x%p",
                   pPhysLink);
   res = pPhysLink->OnStateChange(mpSignal, mRegisteredIDLEvent, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);
   
   DSSCommon::ReleaseIf((IQI**)&pPhysLink);

   return res;
}

DSSPhysLinkStateHandler* DSSPhysLinkStateHandler::CreateInstance()
{
  return new DSSPhysLinkStateHandler;
}

