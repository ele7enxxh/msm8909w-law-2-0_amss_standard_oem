/*======================================================

FILE:  DSS_PrimaryQoSModifyHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSPrimaryQoSModifyHandler functions

=====================================================

Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PrimaryQoSModifyHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_PrimaryQoSModifyHandler.h"
#include "DSS_Common.h"
#include "ps_mem.h"
#include "DSS_GenScope.h"
#include "ds_Errors_Def.h"

using namespace ds::Net;
using namespace ds::Error;

DSSPrimaryQoSModifyHandler::DSSPrimaryQoSModifyHandler()
{
   mEventType = EVENT_HANDLER_QOS_MODIFY;
}

void DSSPrimaryQoSModifyHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg) {
      QoSResultType result = ds::Net::QoSModifyResult::QDS_REJECTED;
      IQoS* piNetQoS = NULL;
      DSSWeakRefScope WeakRefScope;

      if(!WeakRefScope.Init(mpParent)) {
         return;
      }

      AEEResult res = mpParent->GetQoSDefault(&piNetQoS);
      if ((AEE_SUCCESS != res) || (NULL == piNetQoS)) {
         LOG_MSG_ERROR_1("DSSPrimaryQoSModifyHandler::EventOccurred(): "
                         "GetQoSDefault() failed: %d", res);
         return ;
      }
      DSSGenScope scopeQosDefault(piNetQoS,DSSGenScope::IDSIQI_TYPE);
      res = piNetQoS->GetModifyResult(&result);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSPrimaryQoSModifyHandler::EventOccurred(): "
                         "GetModifyResult() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_enum_type eventStatus = DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV;
      dss_iface_ioctl_event_info_union_type eventInfo;

      // update the status
      switch(result) {
         case QoSModifyResult::QDS_ACCEPTED:
            eventInfo.primary_qos_modify_result_info.is_modify_succeeded = true;
            break;
         case QoSModifyResult::QDS_REJECTED:
            eventInfo.primary_qos_modify_result_info.is_modify_succeeded = false;
            break;
         default:
            LOG_MSG_ERROR_1("DSSPrimaryQoSModifyHandler::EventOccurred(): "
                            "Unsupported QoSEvent was received: %d", res);
            return;
      }

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSPrimaryQoSModifyHandler::RegisterIDL()
{
   IQoS* piNetQoSDefault = 0;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }
   
   IDS_ERR_RET(mpParent->GetQoSDefault(&piNetQoSDefault));
   if (NULL == piNetQoSDefault) {
      return QDS_EINVAL;
   }
   DSSGenScope scopeQosDefault(piNetQoSDefault,DSSGenScope::IDSIQI_TYPE);

   LOG_MSG_INFO1_1("DSSPrimaryQoSModifyHandler::RegisterIDL(): "
                   "Registering to QDS_EV_MODIFIED, QoSDefault obj 0x%p",
                   piNetQoSDefault);
   AEEResult res = piNetQoSDefault->OnStateChange(mpSignal, QoSEvent::QDS_EV_MODIFIED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSPrimaryQoSModifyHandler* DSSPrimaryQoSModifyHandler::CreateInstance()
{
   return new DSSPrimaryQoSModifyHandler;
}

