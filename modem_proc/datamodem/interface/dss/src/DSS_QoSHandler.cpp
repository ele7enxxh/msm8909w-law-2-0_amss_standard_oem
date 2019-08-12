/*======================================================

FILE:  DSS_QoSHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSQoSHandler functions

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_QoSHandler.h"
#include "DSS_Common.h"
#include "DSS_GenScope.h"
#include "DSS_MemoryManagement.h"

using namespace ds::Net;

DSSQoSHandler::DSSQoSHandler()
{
   mEventType = EVENT_HANDLER_QOS;
}

void DSSQoSHandler::EventOccurred()
{
   AEEResult res = AEE_SUCCESS;

   if (mpHandlerDataClone->mpEventData->bReg)
   {
      QoSStateInfoType statusInfo;
      IQoS* piQoS = 0;
      DSSWeakRefScope ParentNetAppWeakRefScope;

      if(!ParentNetAppWeakRefScope.Init(mpParent)) {
         return;
      }

      res = mpParent->GetQoS(&piQoS);
      if ( (AEE_SUCCESS != res) || (NULL == piQoS) ) {
         LOG_MSG_ERROR_1("DSSQoSHandler::EventOccurred(): "
                         "GetQoS() failed: %d", res);
         return;
      }
      DSSGenScope scopeQos(piQoS, DSSGenScope::IDSIQI_TYPE);

      res = piQoS->GetState(&statusInfo);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSQoSHandler::EventOccurred(): "
                         "GetState() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_info_union_type eventInfo;
      dss_iface_ioctl_event_enum_type eventStatus;

      // update the status
      switch(statusInfo.state)
      {
         case QoSState::QDS_AVAILABLE_MODIFIED:
            eventStatus = DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV;
            break;
         case QoSState::QDS_SUSPENDED:
            eventStatus = DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV;
            break;
         case QoSState::QDS_UNAVAILABLE:
            eventStatus = DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV;
            break;
         case QoSState::QDS_ACTIVATING:
         case QoSState::QDS_SUSPENDING: 
         case QoSState::QDS_RELEASING:
         case QoSState::QDS_CONFIGURING:
            // These statuses do not require event
            return;
         case QoSState::QDS_INVALID: // fallthrough
         default:
            LOG_MSG_ERROR_1("DSSQoSHandler::EventOccurred(): "
                            "Unsupported QoSEvent was received: %d", res);
            return;
      }

      mpParent->GetHandle(&eventInfo.qos_info.handle);

      LOG_MSG_INFO1_3("DSSQoSHandler::EventOccurred(): "
                      "QoS Obj 0x%p, event %d, qos handle 0x%x",
                      piQoS, eventStatus, eventInfo.qos_info.handle);

      // Update the info code
      DSSConversion::IDS2DSQoSInfoCode(statusInfo.infoCode, &eventInfo.qos_info.info_code);

      if (DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV == eventStatus) {
         DSSNetActive* pNetActive;

         // Get a strong reference to parent NetApp
         res = mpParent->GetDSSNetActive(&pNetActive);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSQoSHandler::EventOccurred(): "
                            "GetDSSNetActive() failed: %d", res);
            return;
         }

         res = pNetActive->RemoveDSSNetQoSSecondary(eventInfo.qos_info.handle);
         if (AEE_SUCCESS != res) {
            // This QoS may be network initiated. 
            // Try to remove from DSSQoSNetInitiated list.
            res = pNetActive->RemoveDSSNetQoSNetInitiatedFromList(eventInfo.qos_info.handle);
            if (AEE_SUCCESS != res) {
               LOG_MSG_ERROR_1("DSSQoSHandler::EventOccurred(): "
                               "QoS object removal failed: %d", res);
               //GetDSSNetActive() gets strong ref, need to release
               PS_MEM_RELEASE(pNetActive);
               return;
            }
         }
         //GetDSSNetActive() gets strong ref, need to release
         PS_MEM_RELEASE(pNetActive);
      }

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);

   }
}

AEEResult DSSQoSHandler::RegisterIDL()
{
   IQoS* piQoS = NULL;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent)) {
     LOG_MSG_ERROR_1("DSSQoSHandler::RegisterIDL(): "
                     "Obj 0x%p, scope init failed", this);
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetQoS(&piQoS));
   DSSGenScope scopeQos(piQoS, DSSGenScope::IDSIQI_TYPE);

   LOG_MSG_INFO1_1("DSSQoSHandler::RegisterIDL(): "
                   "Registering to QDS_EV_STATE_CHANGED, QoS obj 0x%p", piQoS);
   AEEResult res = piQoS->OnStateChange(mpSignal, QoSEvent::QDS_EV_STATE_CHANGED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSQoSHandler* DSSQoSHandler::CreateInstance()
{
   return new DSSQoSHandler;
}

