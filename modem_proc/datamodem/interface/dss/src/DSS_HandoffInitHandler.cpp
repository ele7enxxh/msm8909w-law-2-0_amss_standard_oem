/*======================================================

FILE:  DSS_HandoffInitHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_HandoffInitHandler functions

=====================================================

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_HandoffInitHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-13 da  Created module.

===========================================================================*/

#include "DSS_HandoffInitHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetworkExtScope.h"
#include "ds_Net_INetworkExt.h"

using namespace ds::Net;

DSSHandoffInitHandler::DSSHandoffInitHandler()
{
   mEventType = EVENT_HANDLER_HANDOFF_INIT;
}

void DSSHandoffInitHandler::EventOccurred()
{
   DSSIDSNetworkExtScope IDSNetworkExtScope;
   HandoffinfoType handoffinfo;

   if ( AEE_SUCCESS != IDSNetworkExtScope.Init(mpParent) ) {
         return;
   }
   AEEResult res = IDSNetworkExtScope.Fetch()->GetHandoffInfo(&handoffinfo);
   if(AEE_SUCCESS != res)
   {
      return;
   }
   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      memset(&eventInfo, 0, sizeof(dss_iface_ioctl_event_info_union_type));
      eventInfo.handoff_info.srat = (dss_iface_ioctl_handoff_enum_type)handoffinfo.srat;
      eventInfo.handoff_info.trat = (dss_iface_ioctl_handoff_enum_type)handoffinfo.trat;
      DispatchCB(DSS_IFACE_IOCTL_HANDOFF_INIT_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSHandoffInitHandler::RegisterIDL()
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

   LOG_MSG_INFO1_1("DSSHandoffInitHandler::RegisterIDL(): "
                   "Registering to QDS_EV_HANDOFF_INIT, NetworkExt obj 0x%p",
                   piNetworkExt);
   res = piNetworkExt->OnStateChange(mpSignal, NetworkExtEvent::QDS_EV_HANDOFF_INIT, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   DSSCommon::ReleaseIf((IQI**)&piNetworkExt);

   return res;
}

DSSHandoffInitHandler* DSSHandoffInitHandler::CreateInstance()
{
   return new DSSHandoffInitHandler;
}

