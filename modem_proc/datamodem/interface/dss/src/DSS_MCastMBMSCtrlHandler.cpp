/*======================================================

FILE:  DSS_MCastMBMSCtrlHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSSMCastMBMSCtrlHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCastMBMSCtrlHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "DSS_MCastMBMSCtrlHandler.h"
#include "DSS_Common.h"
#include "ds_Net_IMCastSession.h"
#include "ds_Net_IMCastMBMSCtrlPriv.h"
#include "DSS_EventHandlerMCastMBMSCtrl.h"
#include "DSS_NetMCastMBMSCtrl.h"
#include "DSS_GenScope.h"

using namespace ds::Net;

DSSMCastMBMSCtrlHandler::DSSMCastMBMSCtrlHandler()
{
   mEventType = EVENT_HANDLER_RATE_INTERIA;
}

void DSSMCastMBMSCtrlHandler::EventOccurred()
{
   if (mpHandlerDataClone->mpEventData->bReg)
   {
      MBMSStateType status;
      DSSWeakRefScope ParentNetAppWeakRefScope;

      if(!ParentNetAppWeakRefScope.Init(mpParent)) {
         return;
      }

      IMCastMBMSCtrlPriv* piNetMCastMBMS = 0;
      AEEResult res = mpParent->GetMBMSCtrl(&piNetMCastMBMS);
      DSSGenScope scopeMBMSCtrl(piNetMCastMBMS,DSSGenScope::IDSIQI_TYPE);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                         "GetMBMSCtrl() failed: %d", res);
         return;
      }
      res = piNetMCastMBMS->GetState(&status);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                         "GetState() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_info_union_type eventInfo;
      dss_iface_ioctl_event_enum_type eventStatus;

      // update the status
      switch(status)
      {
         case MBMSState::QDS_ACTIVATED:
            eventStatus = DSS_IFACE_IOCTL_MBMS_CONTEXT_ACT_SUCCESS_EV;
            break;
         case MBMSState::QDS_DEACTIVATED:
            eventStatus = DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_SUCCESS_EV;
            break;
         default:
            LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                            "Unsupported MCastMBMSEvent was received: %d",
                            res);
            return;
      }
      if (DSS_IFACE_IOCTL_MBMS_CONTEXT_DEACT_SUCCESS_EV != eventStatus) {
         DSSNetActive* pNetActive;
         res = mpParent->GetDSSNetActive(&pNetActive);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                            "GetDSSNetActive() failed: %d", res);
            return;
         }

         res = pNetActive->RemoveDSSMcastMBMSCtrl(piNetMCastMBMS);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                            "RemoveDSSMcastMBMSCtrl() failed: %d", res);
            //GetDSSNetActive() gets strong ref, need to release
            PS_MEM_RELEASE(pNetActive);
            return;
         }

         res = pNetActive->RemoveDSSMcastMBMSCtrl(piNetMCastMBMS);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::EventOccurred(): "
                            "RemoveDSSMcastMBMSCtrl() failed: %d", res);
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

AEEResult DSSMCastMBMSCtrlHandler::RegisterIDL()
{
   IMCastMBMSCtrlPriv* piNetMCastMBMS = 0;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   AEEResult res = mpParent->GetMBMSCtrl(&piNetMCastMBMS);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSMCastMBMSCtrlHandler::RegisterIDL(): "
                      "GetMBMSCtrl() failed: %d", res);
      return res;
   }
   
   LOG_MSG_INFO1_1("DSSMCastMBMSCtrlHandler::RegisterIDL(): "
                   "Registering to QDS_EV_ACTIVATE_STATE, MCastMBMSCtrlPriv obj 0x%p",
                   piNetMCastMBMS);
   res = piNetMCastMBMS->OnStateChange(mpSignal, MBMSEvent::QDS_EV_ACTIVATE_STATE, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);
   DSSGenScope scopeMBMSCtrl(piNetMCastMBMS,DSSGenScope::IDSIQI_TYPE);
   
   return res;
}

DSSMCastMBMSCtrlHandler* DSSMCastMBMSCtrlHandler::CreateInstance()
{
   return new DSSMCastMBMSCtrlHandler;
}
#endif // FEATUTE_DATA_PS_MCAST
