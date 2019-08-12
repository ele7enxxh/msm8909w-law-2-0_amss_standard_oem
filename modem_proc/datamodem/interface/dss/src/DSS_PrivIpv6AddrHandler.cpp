/*======================================================

FILE:  DSS_PrivIpv6AddrHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSPrivIpv6AddrHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PrivIpv6AddrHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_Common.h"
#include "ds_Net_IIPv6Address.h"
#include "ps_mem.h"
#include "DSS_PrivIpv6AddrHandler.h"
#include "DSS_GenScope.h"
#include "DSS_MemoryManagement.h"

using namespace ds::Net;

DSSPrivIpv6AddrHandler::DSSPrivIpv6AddrHandler()
{
   mEventType = EVENT_HANDLER_IP6PRIV_ADDR;
}

void DSSPrivIpv6AddrHandler::EventOccurred()
{
   AEEResult res;

   if ((mpHandlerDataClone->mpEventData)->bReg)
   {
      IPv6AddrStateType state;
      IIPv6Address* piNetIpv6Address = 0;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }

      res = mpParent->GetIDSNetIpv6Address(&piNetIpv6Address);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSPrivIpv6AddrHandler::EventOccurred(): "
                         "GetIDSNetIpv6Address() failed: %d", res);
         return;
      }
      DSSGenScope scopeIPv6Address (piNetIpv6Address,DSSGenScope::IDSIQI_TYPE);

      if (NULL != piNetIpv6Address)
      {
         res = piNetIpv6Address->GetState(&state);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSPrivIpv6AddrHandler::EventOccurred(): "
                            "GetState() failed: %d", res);
            return;
         }
      }
      else
      {
        return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_info_union_type eventInfo;
      dss_iface_ioctl_event_enum_type eventStatus;


      if (IPv6AddrState::PRIV_ADDR_WAITING != state) {
         ::ds::INAddr6Type ip6Addr;
         mpParent->GetIsUnique(&eventInfo.priv_ipv6_addr.is_unique);
         if (AEE_SUCCESS != piNetIpv6Address->GetAddress(ip6Addr)) {
            return;
         }
         if (AEE_SUCCESS != DSSConversion::IDS2DSIp6Addr(ip6Addr,&eventInfo.priv_ipv6_addr.ip_addr)){
            return;
         }
      }


      // update the status
      switch(state)
      {
         case IPv6AddrState::PRIV_ADDR_AVAILABLE:
            eventStatus = DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV;
            break;
         case IPv6AddrState::PRIV_ADDR_DEPRECATED:
            eventStatus = DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV;

            break;
         case IPv6AddrState::PRIV_ADDR_DELETED:
            eventStatus = DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV;
            break;
         case IPv6AddrState::PRIV_ADDR_WAITING:
            // This status do not require event
            return;
         default:
            LOG_MSG_ERROR_1("DSSPrivIpv6AddrHandler::EventOccurred(): "
                            "Unsupported Ipv6 Address event was received: %d",
                            res);
            return;
      }

      if (DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV == eventStatus) {
            DSSNetActive* pNetActive;

            res = mpParent->GetDSSNetActive(&pNetActive);
            if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSPrivIpv6AddrHandler::EventOccurred(): "
                            "GetDSSNetActive() failed: %d", res);
               return;
            }

         res = pNetActive->RemoveDSSPrivIpv6Addr(piNetIpv6Address);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSPrivIpv6AddrHandler::EventOccurred(): "
                            "RemoveDSSPrivIpv6Addr() failed: %d", res);
            //GetDSSNetActive() gets strong ref, need to release
            PS_MEM_RELEASE(pNetActive);
            return;
         }
         //GetDSSNetActive() gets strong ref, need to release
         PS_MEM_RELEASE(pNetActive);
      }

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);

   }
}

AEEResult DSSPrivIpv6AddrHandler::RegisterIDL()
{
   IIPv6Address* piNetIpv6Address = NULL;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetIDSNetIpv6Address(&piNetIpv6Address));

   LOG_MSG_INFO1_1("DSSPrivIpv6AddrHandler::RegisterIDL(): "
                   "Registering to QDS_EV_STATE_CHANGED, IPv6Address obj 0x%p",
                   piNetIpv6Address);
   AEEResult res = piNetIpv6Address->OnStateChange(mpSignal, IPv6AddrEvent::QDS_EV_STATE_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   DSSCommon::ReleaseIf((IQI**)&piNetIpv6Address);

   return res;
}

DSSPrivIpv6AddrHandler* DSSPrivIpv6AddrHandler::CreateInstance()
{
   return new DSSPrivIpv6AddrHandler;
}

