/*======================================================

FILE:  DSS_NetworkExtIPv6DelAddrHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSNetworkExtIPv6DelAddrHandler functions

=====================================================

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetworkExtIPv6DelAddrHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-08-07 vm  Created.

===========================================================================*/

#include "DSS_NetworkExtIPv6DelAddrHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"

using namespace ds::Net;

DSSNetworkExtIPv6DelAddrHandler::DSSNetworkExtIPv6DelAddrHandler()
{
   mEventType = EVENT_HANDLER_EXT_IPV6_DEL_ADDR;
}

void DSSNetworkExtIPv6DelAddrHandler::EventOccurred()
{

   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }
      
      // The info code should contain the previous IP address
      AEEResult res = mpParent->GetExtIPv6DelAddr(&eventInfo.ip6_ext_addr_deleted_info);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSNetworkIPHandler::EventOccurred(): "
                         "GetIPAddress() failed: %d", res);
         return;
      }

      DispatchCB(DSS_IFACE_IOCTL_IPV6_EXTERNAL_ADDR_DELETED_EV, 
                 mpHandlerDataClone->mpEventData, 
                 &eventInfo);
   }
}

AEEResult DSSNetworkExtIPv6DelAddrHandler::RegisterIDL()
{
   DSSIDSNetworkScope IDSNetworkScope;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkScope.Init(mpParent));
   
   LOG_MSG_INFO1_1("DSSNetworkExtIPv6DelAddrHandler::RegisterIDL(): "
                   "Registering to QDS_EV_IPV6_EXT_ADDR_DEL_EV, NetworkPriv obj 0x%p",
                   IDSNetworkScope.Fetch());

   AEEResult res = IDSNetworkScope.Fetch()->OnStateChange(mpSignal, 
                                                          NetworkEvent::QDS_EV_IPV6_EXT_ADDR_DEL_EV, 
                                                          &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

void DSSNetworkExtIPv6DelAddrHandler::Destructor() throw()
{
  DS_UTILS_RELEASE_WEAKREF_IF(mpParent);
  DSSEventHandler::Destructor();
}
DSSNetworkExtIPv6DelAddrHandler* DSSNetworkExtIPv6DelAddrHandler::CreateInstance()
{
   return new DSSNetworkExtIPv6DelAddrHandler;
}

