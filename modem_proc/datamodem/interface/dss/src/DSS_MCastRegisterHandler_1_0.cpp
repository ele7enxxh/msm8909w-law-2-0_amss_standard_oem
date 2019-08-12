/*======================================================

FILE:  DSS_MCastRegisterHandler_1_0.cpp

GENERAL DESCRIPTION:
Implementation of DSSMCastRegisterHandler1_0 functions

=====================================================

Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCastRegisterHandler_1_0.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-29 vm  Module created.

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "DSS_MCastRegisterHandler_1_0.h"
#include "DSS_Common.h"
#include "ds_Net_IMCastSessionPriv.h"
#include "DSS_MCast.h"
#include "DSS_GenScope.h"
#include "DSS_MemoryManagement.h"

using namespace ds::Net;
DSSMCastRegisterHandler1_0::DSSMCastRegisterHandler1_0()
{
   mEventType = EVENT_HANDLER_MCAST_REGISTER;
}

void DSSMCastRegisterHandler1_0::EventOccurred()
{

   if (mpHandlerDataClone->mpEventData->bReg)
   {
      MCastStateChangedType stateInfo;
      IMCastSessionPriv* piNetMCast = 0;
      DSSWeakRefScope ParentNetAppWeakRefScope;

      if(!ParentNetAppWeakRefScope.Init(mpParent)) {
         return;
      }

      AEEResult res = mpParent->GetMCastSession(&piNetMCast);
      DSSGenScope scopeNetMCast(piNetMCast,DSSGenScope::IDSIQI_TYPE);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                         "GetMCastSession() failed: %d", res);
         return;
      }

      res = piNetMCast->GetRegStateAndInfoCodeBCMCS1_0(&stateInfo);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                         "GetRegStateAndInfoCodeBCMCS1_0() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_info_union_type eventInfo;
      dss_iface_ioctl_event_enum_type eventStatus;

      eventInfo.mcast_info.force_dereg_cbacks = false;
      // update the status
      switch(stateInfo.regState) {
         case MCastRegState::MCAST_REGISTER_SUCCESS:
            eventStatus = DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV;
            break;
         case MCastRegState::MCAST_REGISTER_FAILURE:
            eventStatus = DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV;
            eventInfo.mcast_info.force_dereg_cbacks = true;
            break;
         case MCastRegState::MCAST_DEREGISTERED:
            eventStatus = DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV;
            eventInfo.mcast_info.force_dereg_cbacks = true;
            break;
         default:
            LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                            "Unsupported MCastEvent was received: %d",
                            stateInfo.regState);
            return;
      }

      mpParent->GetMCastHandle(&eventInfo.mcast_info.handle);

      //update the info code
      LOG_MSG_INFO3_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                      "MCastEvent info code was received: %d",
                      stateInfo.infoCode);
      eventInfo.mcast_info.info_code = (dss_iface_ioctl_mcast_info_code_enum_type)stateInfo.infoCode;
   
      if (DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV == eventStatus) {
         DSSNetActive* pNetActive;
         uint32 MCastHandle;
         res = mpParent->GetDSSNetActive(&pNetActive);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                            "GetDSSNetActive() failed: %d", res);
            return;
         }

         mpParent->GetMCastHandle(&MCastHandle);

         res = pNetActive->RemoveDSSMCast
                        (
                           MCastHandle,
                           MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0
                        );
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::EventOccurred(): "
                            "RemoveDSSMCast() failed: %d", res);
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

AEEResult DSSMCastRegisterHandler1_0::RegisterIDL()
{
   IMCastSessionPriv* piNetMCast = 0;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   AEEResult res = mpParent->GetMCastSession(&piNetMCast);
   DSSGenScope scopeNetMCast(piNetMCast,DSSGenScope::IDSIQI_TYPE);

   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSMCastRegisterHandler1_0::RegisterIDL(): "
                      "GetMCastSession() failed: %d", res);
      return res;
   }
   
   LOG_MSG_INFO1_1("DSSMCastRegisterHandler1_0::RegisterIDL(): "
                   "Registering to QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0, MCastSessionPriv obj 0x%p",
                   piNetMCast);
   res = piNetMCast->OnStateChange(mpSignal, MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSMCastRegisterHandler1_0* DSSMCastRegisterHandler1_0::CreateInstance()
{
   return new DSSMCastRegisterHandler1_0;
}
#endif // FEATUTE_DATA_PS_MCAST
