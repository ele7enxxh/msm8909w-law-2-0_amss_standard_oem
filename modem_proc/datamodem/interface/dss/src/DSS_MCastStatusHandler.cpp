/*======================================================

FILE:  DSS_MCastStatusHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_MCastStatusHandler functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCastStatusHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "DSS_MCastStatusHandler.h"
#include "DSS_Common.h"
#include "ds_Net_IMCastSession.h"
#include "DSS_MCast.h"
#include "DSS_GenScope.h"
#include "DSS_WeakRefScope.h"

using namespace ds::Net;

DSSMCastStatusHandler::DSSMCastStatusHandler()
{
   mEventType = EVENT_HANDLER_MCAST_STATUS;
}

void DSSMCastStatusHandler::EventOccurred()
{

   if (mpHandlerDataClone->mpEventData->bReg)
   {
      MCastInfoCodeType infoCode;
      IMCastSessionPriv* piNetMCast = 0;
      DSSWeakRefScope ParentNetAppWeakRefScope;

      if(!ParentNetAppWeakRefScope.Init(mpParent)) {
         return;
      }

      AEEResult res = mpParent->GetMCastSession(&piNetMCast);
      DSSGenScope scopeNetMCast(piNetMCast,DSSGenScope::IDSIQI_TYPE);

      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastStatusHandler::EventOccurred(): "
                         "GetMCastSession() failed: %d", res);
         return;
      }
      res = piNetMCast->GetTechStatusInfoCode(&infoCode);
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_1("DSSMCastStatusHandler::EventOccurred(): "
                         "GetTechStatusInfoCode() failed: %d", res);
         return;
      }

      // Dispatch the user callback according to the received event.
      dss_iface_ioctl_event_info_union_type eventInfo;
      dss_iface_ioctl_event_enum_type eventStatus = DSS_IFACE_IOCTL_MCAST_STATUS_EV;

	    mpParent->GetMCastHandle(&eventInfo.mcast_info.handle);

      LOG_MSG_INFO3_1("DSSMCastStatusHandler::EventOccurred(): "
                      "MCastEvent info code was received: %d", infoCode);
      //update the info code
    
      eventInfo.mcast_info.info_code = (dss_iface_ioctl_mcast_info_code_enum_type)infoCode;

      DispatchCB(eventStatus, mpHandlerDataClone->mpEventData, &eventInfo);
   }
}

AEEResult DSSMCastStatusHandler::RegisterIDL()
{
   IMCastSessionPriv* piNetMCast = 0;
   DSSWeakRefScope ParentNetAppWeakRefScope;
   IQI* pRegObj = NULL;

   if(!ParentNetAppWeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(mpParent->GetMCastSession(&piNetMCast));
   DSSGenScope scopeNetMCast(piNetMCast,DSSGenScope::IDSIQI_TYPE);

   LOG_MSG_INFO1_1("DSSMCastStatusHandler::RegisterIDL(): "
                   "Registering to QDS_EV_TECHNOLOGY_STATUS_PRIV, MCastSessionPriv obj 0x%p",
                   piNetMCast);
   AEEResult res = piNetMCast->OnStateChange(mpSignal, MCastSessionPrivEvent::QDS_EV_TECHNOLOGY_STATUS_PRIV, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer. 
   // No need for DSS to hold the allocated object to maintain the event registration. 
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}

DSSMCastStatusHandler* DSSMCastStatusHandler::CreateInstance()
{
   return new DSSMCastStatusHandler;
}
#endif // FEATUTE_DATA_PS_MCAST
