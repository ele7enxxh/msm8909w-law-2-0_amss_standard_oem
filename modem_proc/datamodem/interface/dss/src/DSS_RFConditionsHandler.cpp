/*======================================================

FILE:  DSS_RFConditionsHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSRFConditionsHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_RFConditionsHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_RFConditionsHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkPrivScope.h"

using namespace ds::Net;

DSSRFConditionsHandler::DSSRFConditionsHandler()
{
   mEventType = EVENT_HANDLER_RFC_COND;
}

void DSSRFConditionsHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }

      AEEResult res = mpParent->GetRFConditions(&eventInfo.rf_conditions_change_info);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSRFConditionsHandler::EventOccurred(): "
                         "GetRFConditions() failed: %d", res);
         return;
      }
      DispatchCB(DSS_IFACE_IOCTL_RF_CONDITIONS_CHANGED_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSRFConditionsHandler::RegisterIDL()
{
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }
   IDS_ERR_RET(IDSNetworkPrivScope.Init(mpParent));
   LOG_MSG_INFO1_1("DSSRFConditionsHandler::RegisterIDL(): "
                   "Registering to QDS_EV_RF_CONDITIONS_CHANGED, NetworkPriv obj 0x%p",
                   IDSNetworkPrivScope.Fetch());
   AEEResult res = IDSNetworkPrivScope.Fetch()->OnStateChange(mpSignal, NetworkEvent::QDS_EV_RF_CONDITIONS_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSRFConditionsHandler* DSSRFConditionsHandler::CreateInstance()
{
   return new DSSRFConditionsHandler;
}

