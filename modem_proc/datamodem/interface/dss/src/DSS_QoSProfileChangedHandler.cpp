/*======================================================

FILE:  DSS_QoSProfileChangedHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSQoSProfileChangedHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSProfileChangedHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_QoSProfileChangedHandler.h"
#include "DSS_Common.h"
#include "ds_Net_IMCastSession.h"
#include "DSS_MCast.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetwork1xPrivScope.h"
#include "ds_Net_IQoSManager.h"

using namespace ds::Net;

DSSQoSProfileChangedHandler::DSSQoSProfileChangedHandler()
{
   mEventType = EVENT_HANDLER_QOS_PROF_CHNG;
}

void DSSQoSProfileChangedHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg)
   {

      dss_iface_ioctl_event_enum_type eventStatus = DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV;
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }

      memset(&eventInfo, 0, sizeof(dss_iface_ioctl_event_info_union_type));

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSQoSProfileChangedHandler::RegisterIDL()
{
   IQoSManager* piQoSManager = 0;
   AEEResult res = AEE_SUCCESS;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetNetQoSManager(&piQoSManager));

   LOG_MSG_INFO1_1("DSSQoSProfileChangedHandler::RegisterIDL(): "
                   "Registering to QDS_EV_PROFILES_CHANGED, QoSManager obj 0x%p",
                   piQoSManager);
   res = piQoSManager->OnStateChange(mpSignal, QoSMgrEvent::QDS_EV_PROFILES_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   DSSCommon::ReleaseIf((IQI**)&piQoSManager);

   return res;
}

DSSQoSProfileChangedHandler* DSSQoSProfileChangedHandler::CreateInstance()
{
   return new DSSQoSProfileChangedHandler;
}

