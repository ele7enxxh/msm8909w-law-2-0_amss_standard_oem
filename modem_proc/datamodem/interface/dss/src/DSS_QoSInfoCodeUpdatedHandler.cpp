/*======================================================

FILE:  DSS_QoSInfoCodeUpdatedHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSQoSInfoCodeUpdatedHandler functions

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSInfoCodeUpdatedHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "comdef.h"
  
#include "DSS_QoSInfoCodeUpdatedHandler.h"
#include "DSS_Common.h"
#include "DSS_GenScope.h"
#include "ps_mem.h"

using namespace ds::Net;

DSSQoSInfoCodeUpdatedHandler::DSSQoSInfoCodeUpdatedHandler()
{
   mEventType = EVENT_HANDLER_QOS_INF_CODE_UPD;
}

void DSSQoSInfoCodeUpdatedHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg)
   {
      QoSInfoCodeType infoCode;
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
         LOG_MSG_ERROR_1("DSSQoSInfoCodeUpdatedHandler::EventOccurred(): "
                         "GetQoS() failed: %d", res);
         return;
      }
      DSSGenScope scopeQos(piQoS, DSSGenScope::IDSIQI_TYPE);

      res = piQoS->GetUpdatedInfoCode(&infoCode);
      if (AEE_SUCCESS != res)
      {
         LOG_MSG_ERROR_1("DSSQoSInfoCodeUpdatedHandler::EventOccurred(): "
                         "GetUpdatedInfoCode() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_enum_type eventStatus = DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV;
      dss_iface_ioctl_event_info_union_type eventInfo;
      memset(&eventInfo, 0, sizeof(dss_iface_ioctl_event_info_union_type));

      // Update the info code
      DSSConversion::IDS2DSQoSInfoCode(infoCode, &eventInfo.qos_info.info_code);
            
      mpParent->GetHandle(&eventInfo.qos_info.handle);

      LOG_MSG_INFO1_3("DSSQoSInfoCodeUpdatedHandler::EventOccurred(): "
                      "QoS Obj 0x%p, event %d, qos handle 0x%x",
                      piQoS, eventStatus, eventInfo.qos_info.handle);

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSQoSInfoCodeUpdatedHandler::RegisterIDL()
{
   IQoS* piQoS = NULL;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent))
   {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetQoS(&piQoS));
   DSSGenScope scopeQos(piQoS, DSSGenScope::IDSIQI_TYPE);

   LOG_MSG_INFO1_1("DSSQoSInfoCodeUpdatedHandler::RegisterIDL(): "
                   "Registering to QDS_EV_INFO_CODE_UPDATED, QoS obj 0x%p",
                   piQoS);
   AEEResult res = piQoS->OnStateChange(mpSignal, QoSEvent::QDS_EV_INFO_CODE_UPDATED, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSQoSInfoCodeUpdatedHandler* DSSQoSInfoCodeUpdatedHandler::CreateInstance()
{
   return new DSSQoSInfoCodeUpdatedHandler;
}

