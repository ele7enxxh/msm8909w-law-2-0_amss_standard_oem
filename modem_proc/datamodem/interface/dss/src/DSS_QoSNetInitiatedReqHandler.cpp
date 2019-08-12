/*======================================================

FILE:  DSS_QoSNetInitiatedReqHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_QoSNetInitiatedReqHandler functions

=====================================================

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSNetInitiatedReqHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-05 sz  Created.

===========================================================================*/

#include "comdef.h"

#include "dss_iface_ioctl.h"

#include "DSS_QoSNetInitiatedReqHandler.h"
#include "DSS_Common.h"            
#include "ds_Net_IQoSNetInitiated.h"
#include "DSS_WeakRefScope.h"
#include "DSS_Globals.h"
#include "DSS_GenScope.h"
#include "DSS_MemoryManagement.h"

using namespace ds::Net;

DSSQoSNetInitiatedReqHandler::DSSQoSNetInitiatedReqHandler()
{
   mEventType = EVENT_HANDLER_QOS;
}

void DSSQoSNetInitiatedReqHandler::EventOccurred()
{
   AEEResult                              res;
   dss_iface_id_type                      iface_id;
   uint8                                  flowID;

   if (mpHandlerDataClone->mpEventData->bReg) {
      DSSWeakRefScope parentNetAppWeakRefScope;      
      dss_iface_ioctl_event_info_union_type  eventInfo;
      DSSNetActive* pNetActive = NULL;
      IQoSNetInitiatedReq* pIQoSNetInitiatedReq = NULL;
      dss_qos_handle_type qos_handle;
            
      // ParentNetApp is the DSSQoSNetInitiatedReq object
      if(!parentNetAppWeakRefScope.Init(mpParent)) {
         return;
      }

      res = mpParent->GetDSSNetActive(&pNetActive);
      if (AEE_SUCCESS != res) {
        LOG_MSG_ERROR_1("DSSQoSNetInitiatedReqHandler::EventOccurred(): "
                        "Failed to GetDSSNetActive. Req Object: 0x%p",
                        mpParent);
        return;
      }

      pNetActive->GetIfaceId(&iface_id);
      
      //GetDSSNetApp() gets strong ref, need to release
      PS_MEM_RELEASE(pNetActive);


      // Create DSSNetInitiatedFlow until there are no pending flows
      mpParent->GetQoSNetInitiatedReq(&pIQoSNetInitiatedReq);
      DSSGenScope GenScopeObject(pIQoSNetInitiatedReq, DSSGenScope::IDSIQI_TYPE);
      while (pIQoSNetInitiatedReq->IsPendingMatchingFlow()) {
         // set the request handle in the event info so app knows
         // to which request the new QoS is compatible
         mpParent->GetHandle(
           &eventInfo.qos_net_initiated_available_info.handle);
        
         // Generate a flow id in a similar way to qos request.
         // Application shall get that id in the callback and
         // shall be able to use it in applicable QoS operations.
         res = DSSGlobals::Instance()->GetFreeQoSFlowID(&flowID);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSQoSNetInitiatedReqHandler::EventOccurred(): "
                            "Failed to GetFreeQoSFlowID. Req Object: 0x%p",
                            mpParent);
            return;
         }

         qos_handle = iface_id | flowID;
         eventInfo.qos_net_initiated_available_info.qos_handle = qos_handle;

         // Create a new DSS object representing the new Network Initiated QoS
         res = mpParent->CreateDSSQoSNetInitiated(eventInfo.qos_net_initiated_available_info.qos_handle);
         if (AEE_SUCCESS != res) {
            if (AEE_EALREADY == res) {
               LOG_MSG_INFO1_1("DSSQoSNetInitiatedReqHandler::EventOccurred(): "
                               "DSSQoSNetInitiated already exist. Not calling app CB. Req Object: 0x%p",
                               mpParent);
               DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(flowID));
               return;
           }
           else {
              LOG_MSG_ERROR_1("DSSQoSNetInitiatedReqHandler::EventOccurred(): "
                              "Failed to create DSSQoSNetInitiated. Not calling app CB. Req Object: 0x%p",
                              mpParent);
              DSSGlobals::Instance()->ReleaseQoSNetInitiatedReqHandle(eventInfo.qos_net_initiated_available_info.handle);
              DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(flowID));
              return;
            }
         }

         LOG_MSG_INFO1_3("DSSQoSNetInitiatedReqHandler::EventOccurred(): "
                         "Dispatching DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV (event %d), "
                         "req handle 0x%x, qos handle 0x%x",
                         DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV,
                         eventInfo.qos_net_initiated_available_info.handle,
                         eventInfo.qos_net_initiated_available_info.qos_handle);

         DispatchCB(DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV, mpHandlerDataClone->mpEventData, &eventInfo);
      }
   }
}

AEEResult DSSQoSNetInitiatedReqHandler::RegisterIDL()
{
   IQoSNetInitiatedReq* piQoSNetInitiatedReq = NULL;
   DSSWeakRefScope parentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!parentNetAppWeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetQoSNetInitiatedReq(&piQoSNetInitiatedReq));
   DSSGenScope scopeQosNetInitiatedReq(piQoSNetInitiatedReq, DSSGenScope::IDSIQI_TYPE);

   // Register with dsnet to be notified when compatible Network Initiated QoS is created
   LOG_MSG_INFO1_1("DSSQoSNetInitiatedReqHandler::RegisterIDL(): "
                   "Registering to QDS_EV_STATE_CHANGED, QoSNetInitiatedReq obj 0x%p",
                   piQoSNetInitiatedReq);
   AEEResult res = piQoSNetInitiatedReq->OnStateChange(mpSignal, QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSQoSNetInitiatedReqHandler* DSSQoSNetInitiatedReqHandler::CreateInstance()
{
   return new DSSQoSNetInitiatedReqHandler;
}

