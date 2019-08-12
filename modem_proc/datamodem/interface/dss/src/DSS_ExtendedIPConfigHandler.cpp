/*======================================================

FILE:  DSS_ExtendedIPConfigHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_ExtendedIPConfigHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_ExtendedIPConfigHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_ExtendedIPConfigHandler.h"
#include "DSS_Common.h"
#include "ds_Net_INetworkControl.h"
#include "DSS_IDSNetworkScope.h"
#include "ps_mem.h"

using namespace ds::Net;

DSSExtendedIPConfigHandler::DSSExtendedIPConfigHandler()
{
   mEventType = EVENT_HANDLER_EXT_IP_CONFIG;
}

void DSSExtendedIPConfigHandler::EventOccurred()
{
   DSSIDSNetworkScope IDSNetworkScope;
   INetworkControl *pNetworkControl = NULL;
   AEEResult res = AEE_SUCCESS;

   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }

      if (AEE_SUCCESS != IDSNetworkScope.Init(mpParent)) {
         return;
      }

      res = IDSNetworkScope.Fetch()->QueryInterface(AEEIID_INetworkControl,
                                                    (void**)&pNetworkControl);
      if (AEE_SUCCESS != res) {
        LOG_MSG_ERROR_1("DSSExtendedIPConfigHandler::EventOccurred(): "
                        "QueryInterface() for INetworkControl failed: %d",
                        res);
        return;
      }
      res = pNetworkControl->GetDHCPRefreshResult(&eventInfo.extended_ip_config_info);

      DSSCommon::ReleaseIf((IQI**)&pNetworkControl);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSExtendedIPConfigHandler::EventOccurred(): "
                         "GetDHCPRefreshResult() failed: %d", res);
         return;
      }
      DispatchCB(DSS_IFACE_IOCTL_EXTENDED_IP_CONFIG_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSExtendedIPConfigHandler::RegisterIDL()
{
   DSSIDSNetworkScope IDSNetworkScope;
   INetworkControl *piNetworkControl = 0;
   AEEResult res = AEE_SUCCESS;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkScope.Init(mpParent));

   IDS_ERR_RET(IDSNetworkScope.Fetch()->QueryInterface(AEEIID_INetworkControl,
                                                 (void**)&piNetworkControl));

   LOG_MSG_INFO1_1("DSSExtendedIPConfigHandler::RegisterIDL(): "
                   "Registering to QDS_EV_EXTENDED_IP_CONFIG, NetworkControl obj 0x%p",
                   piNetworkControl);
   res = piNetworkControl->OnStateChange(mpSignal, NetworkControlEvent::QDS_EV_EXTENDED_IP_CONFIG, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   DSSCommon::ReleaseIf((IQI**)&piNetworkControl);

   return res;
}

DSSExtendedIPConfigHandler* DSSExtendedIPConfigHandler::CreateInstance()
{
   return new DSSExtendedIPConfigHandler;
}

