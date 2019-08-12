/*======================================================

FILE:  DSSHDRRev0RateInteriaHandler.cpp

GENERAL DESCRIPTION:
Implementation of DSSHDRRev0RateInteriaHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
EDIT HISTORY FOR MODULE

Please notice that the changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_HDRRev0RateInteriaHandler.cpp#1 $
$DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

when       who what, where, why
---------- --- ------------------------------------------------------------
2010-04-18 en  History added.

===========================================================================*/

#include "DSS_HDRRev0RateInteriaHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetwork1xPrivScope.h"
#include "DSS_IDSNetwork1xScope.h"
#include "ps_mem.h"

#include "ds_Utils_DebugMsg.h"

using namespace ds::Net;

DSSHDRRev0RateInteriaHandler::DSSHDRRev0RateInteriaHandler()
{
  mEventType = EVENT_HANDLER_RATE_INTERIA;
}

dss_iface_ioctl_event_enum_type DSSHDRRev0RateInteriaHandler::registrationSuccess2Event(
  boolean registrationSuccess)
{
  if (registrationSuccess)
  {
    return DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV;
  }
  
  return DSS_IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV;
}

void DSSHDRRev0RateInteriaHandler::EventOccurred()
{
  DSSIDSNetworkScope IDSNetworkScope;
  DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;
  boolean registrationSuccess = FALSE;
  Network1xPrivHDRRev0RateInertiaFailureCodeType failureCode;
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

    res = IDSNetworkScope.Init(mpParent);
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                      "IDSNetworkScope.Init() failed: %d", res);
      return;
    }

    res = IDSNetwork1xPrivScope.Init(IDSNetworkScope.Fetch());
    if (AEE_SUCCESS != res)
    {
      LOG_MSG_ERROR_1("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                      "IDSNetworkScope.Init() failed: %d", res);
      return;
    }

    res = IDSNetwork1xPrivScope.Fetch()->GetHDRRev0RateInertiaResult(
      &registrationSuccess);
    if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                      "GetUpdatedInfoCode() failed: %d", res);
      return;
    }

    // Dispatch the user callback according to the received event.
    event = registrationSuccess2Event(registrationSuccess);

    // This if statement checks that the event raised
    // is relevant to this event handler object
    if (mEvent != event)
    {
      return;
    }
	
	LOG_MSG_INFO1_1("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                 "Event occurred = %d", mEvent);

    if (registrationSuccess) {
      memset(&eventInfo, 0, sizeof(dss_iface_ioctl_event_info_union_type));
    }
    else
    {
      res = IDSNetwork1xPrivScope.Fetch()->GetHDRRev0RateInertiaResultInfoCode(
        &failureCode);
      if (AEE_SUCCESS != res)
      {
        LOG_MSG_ERROR_0("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                        "GetHDRRev0RateInertiaResultInfoCode failed: %d");
        return;
      }

      res = DSSConversion::IDS2DSInertiaFailureInfoCode(
        failureCode,
        &(eventInfo.hdr_rev0_rate_inertia_failure_code));
      if (AEE_SUCCESS != res)
      {
        LOG_MSG_ERROR_0("DSSHDRRev0RateInteriaHandler::EventOccurred(): "
                        "Conversion of Failure Info Code Faild");
        return;
      }
    }

    DispatchCB(event, mpHandlerDataClone->mpEventData, &eventInfo);
  }
}

AEEResult DSSHDRRev0RateInteriaHandler::RegisterIDL()
{
  DSSIDSNetworkScope IDSNetworkScope;
  DSSIDSNetwork1xPrivScope IDSNetwork1xPrivScope;
  DSSWeakRefScope WeakRefScope;
  IQI* pRegObj = NULL;
  AEEResult res = AEE_SUCCESS;

  if(!WeakRefScope.Init(mpParent))
  {
    return AEE_EFAILED;
  }

  IDS_ERR_RET(IDSNetworkScope.Init(mpParent));
  IDS_ERR_RET(IDSNetwork1xPrivScope.Init(IDSNetworkScope.Fetch()));
  LOG_MSG_INFO1_1("DSSHDRRev0RateInteriaHandler::RegisterIDL(): "
                  "Registering to HDR_REV0_RATE_INERTIA_RESULT, Network1xPriv obj 0x%p",
                  IDSNetwork1xPrivScope.Fetch());

  res = IDSNetwork1xPrivScope.Fetch()->OnStateChange(
    mpSignal,
    Network1xPrivEvent::HDR_REV0_RATE_INERTIA_RESULT,
    &pRegObj);
  // regObj mechanism is currently not in effect by dsnet layer. 
  // No need for DSS to hold the allocated object to maintain the event registration. 
  // For design simplicity we release the regObj immediately.
  // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
  DSSCommon::ReleaseIf(&pRegObj);

  return res;
}

DSSHDRRev0RateInteriaHandler* DSSHDRRev0RateInteriaHandler::CreateInstance()
{
  return new DSSHDRRev0RateInteriaHandler;
}

