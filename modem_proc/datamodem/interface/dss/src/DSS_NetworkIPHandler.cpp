/*======================================================

FILE:  DSS_NetworkIPHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_NetworkIPHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetworkIPHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_NetworkIPHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkPrivScope.h"

using namespace ds::Net;

DSSNetworkIPHandler::DSSNetworkIPHandler()
{
   mEventType = EVENT_HANDLER_NETWORK_IP;
}

void DSSNetworkIPHandler::EventOccurred()
{

   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }
      
      // The info code should contain the previous IP address
      AEEResult res = mpParent->GetPreviousIPAddress(&eventInfo.addr_change_info);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSNetworkIPHandler::EventOccurred(): "
                         "GetIPAddress() failed: %d", res);
         return;
      }
      DispatchCB(DSS_IFACE_IOCTL_ADDR_CHANGED_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSNetworkIPHandler::RegisterIDL()
{
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkPrivScope.Init(mpParent));
   
   LOG_MSG_INFO1_1("DSSNetworkIPHandler::RegisterIDL(): "
                   "Registering to QDS_EV_IP_ADDR_CHANGED, NetworkPriv obj 0x%p",
                   IDSNetworkPrivScope.Fetch());
   AEEResult res = IDSNetworkPrivScope.Fetch()->OnStateChange(mpSignal, NetworkEvent::QDS_EV_IP_ADDR_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSNetworkIPHandler* DSSNetworkIPHandler::CreateInstance()
{
   return new DSSNetworkIPHandler;
}

