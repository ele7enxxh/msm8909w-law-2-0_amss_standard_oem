/*======================================================

FILE:  DSS_OutageHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_OutageHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_OutageHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_OutageHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetworkPrivScope.h"

using namespace ds::Net;

DSSOutageHandler::DSSOutageHandler()
{
   mEventType = EVENT_HANDLER_OUTAGE;
}

void DSSOutageHandler::EventOccurred()
{

   DSSIDSNetworkScope IDSNetworkScope;

   if (mpHandlerDataClone->mpEventData->bReg) {
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }
      
      if ( AEE_SUCCESS != IDSNetworkScope.Init(mpParent) ) {
         return;
      }

      OutageInfoType outageInfo;
      AEEResult res = IDSNetworkScope.Fetch()->GetOutageInfo(&outageInfo);
      if ((AEE_SUCCESS != res) &&
          ((OutageState::QDS_INVALID == outageInfo.state) ||
           (OutageState::QDS_EXPIRED == outageInfo.state))) {
         LOG_MSG_ERROR_2("DSSOutageHandler::EventOccurred(): "
                         "GetOutageInfo() failed: %d or unexpected status: %d",
                         res, outageInfo.state);
         return;
      }
      dss_iface_ioctl_event_info_union_type eventInfo;
      eventInfo.outage_notification_info.time_to_outage = outageInfo.timeToOutage;
      eventInfo.outage_notification_info.duration = outageInfo.duration;
      DispatchCB(DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSOutageHandler::RegisterIDL()
{
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkPrivScope.Init(mpParent));
   
   LOG_MSG_INFO1_1("DSSOutageHandler::RegisterIDL(): "
                   "Registering to QDS_EV_OUTAGE, NetworkPriv obj 0x%p",
                   IDSNetworkPrivScope.Fetch());
   AEEResult res = IDSNetworkPrivScope.Fetch()->OnStateChange(mpSignal, NetworkEvent::QDS_EV_OUTAGE, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSOutageHandler* DSSOutageHandler::CreateInstance()
{
   return new DSSOutageHandler;
}

