/*======================================================

FILE:  DSS_SlottedResultHandler.cpp

GENERAL DESCRIPTION:
  Implementation of DSSSlottedResultHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_SlottedResultHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_SlottedResultHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetwork1xPrivScope.h"

using namespace ds::Net;

DSSSlottedResultHandler::DSSSlottedResultHandler()
{
  mEventType = EVENT_HANDLER_SLOTT_RES;
}

void DSSSlottedResultHandler::EventOccurred()
{
  DSSIDSNetworkScope IDSNetworkScope;
  DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;
  dss_iface_ioctl_event_enum_type event = DSS_IFACE_IOCTL_MIN_EV;
  dss_iface_ioctl_event_info_union_type eventInfo;
  ds::Net::Network1xPrivResultCodeType resultCode;
  DSSWeakRefScope WeakRefScope;

  if (mpHandlerDataClone->mpEventData->bReg)
  {
    if(!WeakRefScope.Init(mpParent))
    {
      return;
    }

    if (AEE_SUCCESS != IDSNetworkScope.Init(mpParent))
    {
      return;
    }

    if (AEE_SUCCESS != IDSNetwork1xPrivScope.Init(IDSNetworkScope.Fetch()))
    {
      return;
    }

    AEEResult nRet = IDSNetwork1xPrivScope.Fetch()->GetHDRSlottedModeResult(&resultCode);
    if (AEE_SUCCESS != nRet) 
    {
      return;
    }

    // memset 0 the event info.
    memset(&eventInfo,0,sizeof(dss_iface_ioctl_event_info_union_type));
    
    switch (resultCode)
    {
    case ds::Net::Network1xPrivSlottedMode::REQUEST_SUCCEES:
      event = DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV;
      break;

    case ds::Net::Network1xPrivSlottedMode::REQUEST_REJECTED:
      event = DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV;
      eventInfo.slotted_mode_info.hdr_slotted_mode_failure_code =
        DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_REQUEST_REJECTED;
      break;

    case ds::Net::Network1xPrivSlottedMode::REQUEST_FAILED_TX:
      event = DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV;
      eventInfo.slotted_mode_info.hdr_slotted_mode_failure_code = 
        DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_REQUEST_FAILED_TX;
      break;

    case ds::Net::Network1xPrivSlottedMode::REQUEST_UNSUPPORTED:
      event = DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV;
      eventInfo.slotted_mode_info.hdr_slotted_mode_failure_code =
        DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_NOT_SUPPORTED;
      break;

    case ds::Net::Network1xPrivSlottedMode::REQUEST_NO_NET:
      event = DSS_IFACE_IOCTL_707_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV;
      eventInfo.slotted_mode_info.hdr_slotted_mode_failure_code =
        DSS_IFACE_IOCTL_HDR_SLOTTED_MODE_NO_NET;
      break;

    default:
      LOG_MSG_ERROR_1("DSSSlottedResultHandler::EventOccurred(): "
                      "unsupported result code %d", resultCode);
      return;
    }

    // This if statement checks that the event raised
    // is relevant to this event handler object
    if(mEvent != event)
    {
      return;
    }

    LOG_MSG_INFO1_1("DSSSlottedResultHandler::EventOccurred(): "
                    "Event occurred = %d", mEvent);

    DispatchCB(event, mpHandlerDataClone->mpEventData, &eventInfo);
  }
}

AEEResult DSSSlottedResultHandler::RegisterIDL()
{
  DSSIDSNetworkScope IDSNetworkScope;
  DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;
  AEEResult res = AEE_SUCCESS;
  DSSWeakRefScope WeakRefScope;
  IQI* pRegObj = NULL;
  
  if(!WeakRefScope.Init(mpParent)) {
    return AEE_EFAILED;
  }

  IDS_ERR_RET(IDSNetworkScope.Init(mpParent));
  IDS_ERR_RET(IDSNetwork1xPrivScope.Init(IDSNetworkScope.Fetch()));

  LOG_MSG_INFO1_1("DSSSlottedResultHandler::RegisterIDL(): "
                  "Registering to SLOTTED_MODE_RESULT, Network1xPriv obj 0x%p",
                  IDSNetwork1xPrivScope.Fetch());

  res = IDSNetwork1xPrivScope.Fetch()->OnStateChange(mpSignal, Network1xPrivEvent::SLOTTED_MODE_RESULT, &pRegObj);
  // regObj mechanism is currently not in effect by dsnet layer. 
  // No need for DSS to hold the allocated object to maintain the event registration. 
  // For design simplicity we release the regObj immediately.
  // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
  DSSCommon::ReleaseIf(&pRegObj);

  return res;
}

DSSSlottedResultHandler* DSSSlottedResultHandler::CreateInstance()
{
  return new DSSSlottedResultHandler;
}

