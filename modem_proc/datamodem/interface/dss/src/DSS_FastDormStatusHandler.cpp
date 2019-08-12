/*======================================================

FILE:  DSS_FastDormStatusHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_FastDormStatusHandler functions

=====================================================

Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_FastDormStatusHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-05-20 mct History added.

===========================================================================*/

#include "DSS_FastDormStatusHandler.h"
#include "DSS_Common.h"
#include "DSS_IDSNetworkPrivScope.h"

using namespace ds::Net;

DSSFastDormStatusHandler::DSSFastDormStatusHandler()
{
   mEventType = EVENT_HANDLER_FAST_DORM_STATUS;
}

void DSSFastDormStatusHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg) {
      dss_iface_ioctl_event_info_union_type eventInfo;
      DSSWeakRefScope WeakRefScope;

      if (!WeakRefScope.Init(mpParent)) {
         return;
      }

      AEEResult res = mpParent->GetFastDormStatus(&eventInfo.fast_dorm_status);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSFastDormStatusHandler::EventOccurred(): "
                         "GetFastDormStatus() failed: %d", res);
         return;
      }

      LOG_MSG_INFO1_3("DSSFastDormStatusHandler::EventOccurred(): "
                      "FastDormStatus->EventOccurred(): Sending fast dormancy event %d with info code: %d",
                      DSS_IFACE_IOCTL_FAST_DORMANCY_STATUS_EV,
                      eventInfo.fast_dorm_status.dorm_status,
                      	 0);
      DispatchCB(DSS_IFACE_IOCTL_FAST_DORMANCY_STATUS_EV, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSFastDormStatusHandler::RegisterIDL()
{   
   DSSIDSNetworkPrivScope IDSNetworkPrivScope;

   DSSWeakRefScope WeakRefScope;

   IQI* pRegObj = NULL;

   AEEResult res = AEE_SUCCESS;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkPrivScope.Init(mpParent));

   res = IDSNetworkPrivScope.Fetch()->OnStateChange(mpSignal, NetworkEvent::QDS_EV_FAST_DORM_STATUS, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSFastDormStatusHandler* DSSFastDormStatusHandler::CreateInstance()
{
   return new DSSFastDormStatusHandler;
}

