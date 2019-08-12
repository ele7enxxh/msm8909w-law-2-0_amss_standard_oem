/*======================================================

FILE:  DSS_BearerTechHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_BearerTechHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_BearerTechHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-13 en  History added.

===========================================================================*/

#include "DSS_BearerTechHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "ds_Net_INetworkExt.h"

using namespace ds::Net;

DSSBearerTechHandler::DSSBearerTechHandler()
{
   mEventType = EVENT_HANDLER_BEARER_TECH;
}

void DSSBearerTechHandler::EventOccurred()
{

   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }
      
      AEEResult res = mpParent->GetBearerTech(&eventInfo.tech_change_info.new_bearer_tech);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSBearerTechHandler::EventOccurred(): "
                         "GetBearerTech() failed: %d", res);
         return;
      }
      res = mpParent->GetPreviousBearerTech(&eventInfo.tech_change_info.old_bearer_tech);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSBearerTechHandler::EventOccurred(): "
                         "GetPreviousBearerTech() failed: %d", res);
         return;
      }

      DispatchCB(DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSBearerTechHandler::RegisterIDL()
{
   DSSIDSNetworkScope IDSNetworkScope;
   INetworkExt *piNetworkExt = 0;
   AEEResult res = AEE_SUCCESS;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkScope.Init(mpParent));

   IDS_ERR_RET(IDSNetworkScope.Fetch()->QueryInterface(AEEIID_INetworkExt,
                                                       (void**)&piNetworkExt));

   LOG_MSG_INFO1_1("DSSBearerTechHandler::RegisterIDL(): "
                   "Registering to QDS_EV_BEARER_TECH_CHANGED, NetworkExt obj 0x%p",
                   piNetworkExt);
   res = piNetworkExt->OnStateChange(mpSignal, NetworkExtEvent::QDS_EV_BEARER_TECH_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   DSSCommon::ReleaseIf((IQI**)&piNetworkExt);

   return res;
}

DSSBearerTechHandler* DSSBearerTechHandler::CreateInstance()
{
   return new DSSBearerTechHandler;
}

