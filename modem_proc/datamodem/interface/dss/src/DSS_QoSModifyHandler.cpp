/*======================================================

FILE:  DSS_QoSModifyHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSQoSModifyHandler functions

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSModifyHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_QoSModifyHandler.h"
#include "DSS_Common.h"
#include "DSS_GenScope.h"
#include "ds_Net_IQoSSecondary.h"
#include "ps_mem.h"

using namespace ds::Net;

DSSQoSModifyHandler::DSSQoSModifyHandler()
{
   mEventType = EVENT_HANDLER_QOS_MODIFY;
}

void DSSQoSModifyHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg)
   {
      QoSResultType result = ds::Net::QoSModifyResult::QDS_REJECTED;
      QoSStateInfoType statusInfo;
      IQoS* piQoS = NULL;
      AEEResult res = AEE_SUCCESS;
      DSSWeakRefScope ParentNetAppWeakRefScope;

      if(!ParentNetAppWeakRefScope.Init(mpParent))
      {
         return;
      }

      res = mpParent->GetQoS(&piQoS);
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_ERROR_1("DSSQoSModifyHandler::EventOccurred(): "
                         "GetQoS() failed: %d", res);
         return;
      }
      DSSGenScope scopeNetQosSecondary(piQoS, DSSGenScope::IDSIQI_TYPE);

      res = piQoS->GetState(&statusInfo);
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_ERROR_1("DSSQoSModifyHandler::EventOccurred(): "
                         "GetState() failed: %d", res);
         return;
      }

      res = piQoS->GetModifyResult(&result);
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_ERROR_1("DSSQoSModifyHandler::EventOccurred(): "
                         "GetModifyResult() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_enum_type eventStatus;
      dss_iface_ioctl_event_info_union_type eventInfo;
      memset(&eventInfo, 0, sizeof(dss_iface_ioctl_event_info_union_type));

      // Update the info code
      DSSConversion::IDS2DSQoSInfoCode(statusInfo.infoCode,
                                       &eventInfo.qos_info.info_code);

      // update the status
      switch(result)
      {
         case QoSModifyResult::QDS_ACCEPTED:
            eventStatus = DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV;
            break;
         case QoSModifyResult::QDS_REJECTED:
            eventStatus = DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV;
            break;
         default:
            LOG_MSG_ERROR_1("DSSQoSModifyHandler::EventOccurred(): "
                            "Unsupported QoSEvent was received: %d", res);
            return;
      }

      mpParent->GetHandle(&eventInfo.qos_info.handle);

      LOG_MSG_INFO1_3("DSSQoSModifyHandler::EventOccurred(): "
                      "QoS Obj 0x%p, event %d, qos handle 0x%x",
                      piQoS, eventStatus, eventInfo.qos_info.handle);

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSQoSModifyHandler::RegisterIDL()
{
   IQoS* piQoS = NULL;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent))
   {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetQoS(&piQoS));
   DSSGenScope scopeNetQosSecondary(piQoS, DSSGenScope::IDSIQI_TYPE);

   LOG_MSG_INFO1_1("DSSQoSModifyHandler::RegisterIDL(): "
                   "Registering to QDS_EV_MODIFY_RESULT, QoSSecondary obj 0x%p",
                   piQoS);
   AEEResult res = piQoS->OnStateChange(mpSignal, QoSEvent::QDS_EV_MODIFY_RESULT, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSQoSModifyHandler* DSSQoSModifyHandler::CreateInstance()
{
   return new DSSQoSModifyHandler;
}

