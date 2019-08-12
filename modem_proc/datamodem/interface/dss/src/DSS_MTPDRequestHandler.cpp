/*======================================================

FILE:  DSS_MTPDRequestHandler.h

GENERAL DESCRIPTION:
   Implementation of DSS_MTPDRequestHandler functions

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MTPDRequestHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_MTPDRequestHandler.h"
#include <stringl/stringl.h>
#include "DSS_Common.h"
#include "DSS_IDSNetPolicyScope.h"
#include "DSS_MemoryManagement.h"
#include "ds_Net_CreateInstance.h"

using namespace ds::Net;

DSSMTPDRequestHandler::DSSMTPDRequestHandler()
{
   mEventType = EVENT_HANDLER_MTPD_REQUEST;
}

AEEResult DSSMTPDRequestHandler::SetEventData(dss_iface_ioctl_event_enum_type event,
                                  bool bReg,
                                  dss_iface_ioctl_event_cb userCB,
                                  void* userData,
                                  sint15 appId)
{
   if (NULL == mpHandlerData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerData, sizeof(DSSMTPDRequestData), DSSMTPDRequestData*);
      if (NULL == mpHandlerData) {
         return AEE_ENOMEMORY;
      }

      mpHandlerData->mpEventData = NULL;
   }

   if (NULL == mpHandlerDataClone) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone, sizeof(DSSMTPDRequestData), DSSMTPDRequestData*);
      if (NULL == mpHandlerDataClone) {
         return AEE_ENOMEMORY;
      }

      mpHandlerDataClone->mpEventData = NULL;
   }

   if (NULL == mpHandlerData->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerData->mpEventData, sizeof(EventData), EventData*);
      if (NULL == mpHandlerData->mpEventData) {
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerData->mpEventData, 0, sizeof (EventData));
   }

   if (NULL == mpHandlerDataClone->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone->mpEventData, sizeof(EventData), EventData*);
      if (NULL == mpHandlerDataClone->mpEventData) {
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerDataClone->mpEventData, 0, sizeof (EventData));
   }

   ((DSSMTPDRequestData*)mpHandlerData)->pMTPDReg = NULL;

   IDS_ERR_RET(InitEventData(mpHandlerData->mpEventData,bReg,userCB,userData,appId));

   return AEE_SUCCESS;
}

void DSSMTPDRequestHandler::EventOccurred()
{
   {
      // for DSSMTPDRequestHandler, event data has different size, so it should copy
      // all the data to mpHandlerDataClone
      DSSCritScope cs(*mpCritSect);
      if ((((DSSMTPDRequestData*)mpHandlerDataClone)->mpEventData)->bReg) {

         memscpy(((DSSMTPDRequestData*)mpHandlerDataClone)->mpEventData,
           sizeof(DSSEventHandler::EventData),
           ((DSSMTPDRequestData*)mpHandlerData)->mpEventData,
           sizeof(DSSEventHandler::EventData));

         ((DSSMTPDRequestData*)mpHandlerDataClone)->pMTPDReg = ((DSSMTPDRequestData*)mpHandlerData)->pMTPDReg;
      }
      else {
         return;
      }
   } // release lock

   dss_iface_ioctl_event_info_union_type eventInfo;
   DSSWeakRefScope WeakRefScope;

   if(!WeakRefScope.Init(mpParent)) {
      return;
   }

   memset(&eventInfo,0,sizeof(dss_iface_ioctl_event_info_union_type));
   eventInfo.mt_handle = (dss_iface_ioctl_mt_handle_type)this;
   DispatchCB(DSS_IFACE_IOCTL_MT_REQUEST_EV, ((DSSMTPDRequestData*)mpHandlerDataClone)->mpEventData, &eventInfo);

}

AEEResult DSSMTPDRequestHandler::RegisterIDL()
{
   DSSIDSNetPolicyScope   IDSNetPolicyScope;
   AEEResult              res = AEE_SUCCESS;
   ITechUMTS*             piTechUMTS = 0;
   DSSWeakRefScope WeakRefScope;
   DSSNetApp* pNetApp = NULL;
   sint15 netHandle;

   mpParent->GetNetHandle(&netHandle);

   res = DSSGlobals::Instance()->GetNetApp(netHandle, &pNetApp);
   if (AEE_SUCCESS != res ) {
      return AEE_EUNSUPPORTED;
   }

   if(!WeakRefScope.Init(pNetApp)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetPolicyScope.Init(pNetApp));

   res = mpParent->GetTechUMTS(&piTechUMTS);

   if (AEE_SUCCESS != res ) {
      return AEE_EFAILED;
   }

   res = piTechUMTS->RegMTPD(IDSNetPolicyScope.Fetch(), mpSignal, &(((DSSMTPDRequestData*)mpHandlerData)->pMTPDReg));

   DSSCommon::ReleaseIf((IQI**)&piTechUMTS);

   return res;
}

/*lint -e{1551} */
void DSSMTPDRequestHandler::Destructor() throw()
{
   DSSCommon::ReleaseIf(&(((DSSMTPDRequestData*)mpHandlerData)->pMTPDReg));
   DS_UTILS_RELEASE_WEAKREF_IF(mpParent);
   DSSEventHandler::Destructor();
}

DSSMTPDRequestHandler* DSSMTPDRequestHandler::CreateInstance()
{
   return new DSSMTPDRequestHandler;
}

