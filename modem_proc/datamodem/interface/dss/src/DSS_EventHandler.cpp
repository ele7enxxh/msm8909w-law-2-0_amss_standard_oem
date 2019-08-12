/*======================================================

FILE:  DSS_EventHandler.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_EventHandler functions

=====================================================

Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandler.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-15 rp  QShrink2 changes: MSG_* macros are removed from inline functions.
  2010-04-13 en  History added.

===========================================================================*/

#include "customer.h"
#include <stringl/stringl.h>
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandler.h"
#include "DSS_MemoryManagement.h"
#include "ds_Utils_Atomic.h"
#include "ds_Errors_Def.h"
#include "ps_system_heap.h"

using namespace ds::Error;

DSSEventHandler::DSSEventHandler() :
   mpSignal(NULL),
   mpHandlerData(NULL),
   mEventType(EVENT_HANDLER_UNDEFINED),
   mpHandlerDataClone(NULL),
   mpCritSect(NULL),
   mpSignalCtl(NULL),
   mRegisteredIDLEvent(0),
   mEvent(DSS_IFACE_IOCTL_MIN_EV)
{
}

void DSSEventHandler::SignalCB(SignalHandlerBase* pSignalHandlerBaseParam)
{

   LOG_MSG_FUNCTION_ENTRY_1("DSSEventHandler::SignalCB(): "
                            "pSignalHandlerBaseParam:0x%p",
                            pSignalHandlerBaseParam);

   if (NULL == pSignalHandlerBaseParam)
   {
      return;
   }
   // Event Handler may be released from within the CBHandler (EventOccured) function
   // if so , we want to finally delete it just in the very end of SignalCB
   // TODO: we get a strong ref to EventHandler through SignalHandler::Notify(), is
   // this AddRef(), Release is needed?
   //(void) static_cast<DSSEventHandler*>(pSignalHandlerBaseParam)->AddRef();
   static_cast<DSSEventHandler*>(pSignalHandlerBaseParam)->CBHandler();
   //(void) static_cast<DSSEventHandler*>(pSignalHandlerBaseParam)->Release();
}

void DSSEventHandler::CBHandler()
{
   // copy all event data to mpHandlerDataClone
   mpCritSect->Enter();
   memscpy(mpHandlerDataClone->mpEventData, sizeof(DSSEventHandler::EventData),
     mpHandlerData->mpEventData, sizeof(DSSEventHandler::EventData));
   mpCritSect->Leave();

   EventOccurred();

   AEEResult res = mpSignalCtl->Enable();
   // Assuming no events occurred during the invoking of user app callback

   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("DSSEventHandler::CBHandler(): "
                      "Signal Enable() failed: %d", res);
      return;
   }
}

AEEResult DSSEventHandler::SetEventData
(
   dss_iface_ioctl_event_enum_type event,
   bool bReg,
   dss_iface_ioctl_event_cb userCB,
   void* userData,
   sint15 appId
)
{
   if (NULL == mpHandlerData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerData, sizeof(DSSEventHandler::HandlerData),
                               DSSEventHandler::HandlerData*);
      if (NULL == mpHandlerData) {
         LOG_MSG_ERROR_0("DSSEventHandler::SetEventData(): "
                         "Can't allocate event data");
         return AEE_ENOMEMORY;
      }

      mpHandlerData->mpEventData = NULL;
   }

   if(NULL == mpHandlerDataClone) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone, sizeof(DSSEventHandler::HandlerData),
                               DSSEventHandler::HandlerData*);
      if (NULL == mpHandlerDataClone) {
         LOG_MSG_ERROR_0("DSSEventHandler::SetEventData(): "
                         "Can't allocate event data");
         return AEE_ENOMEMORY;
      }

      mpHandlerDataClone->mpEventData= NULL;
   }

   if (NULL == mpHandlerData->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerData->mpEventData, sizeof(DSSEventHandler::EventData),
                               DSSEventHandler::EventData*);
      if (NULL == mpHandlerData->mpEventData) {
         LOG_MSG_ERROR_0("DSSEventHandler::SetEventData(): "
                         "Can't allocate event data");
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerData->mpEventData, 0, sizeof (DSSEventHandler::EventData));
   }

   if (NULL == mpHandlerDataClone->mpEventData) {
      PS_SYSTEM_HEAP_MEM_ALLOC(mpHandlerDataClone->mpEventData, sizeof(DSSEventHandler::EventData),
                               DSSEventHandler::EventData*);
      if (NULL == mpHandlerDataClone->mpEventData) {
         LOG_MSG_ERROR_0("DSSEventHandler::SetEventData(): "
                         "Can't allocate event data");
         return AEE_ENOMEMORY;
      }

      memset (mpHandlerDataClone->mpEventData, 0, sizeof (DSSEventHandler::EventData));
   }

   IDS_ERR_RET(InitEventData(mpHandlerData->mpEventData,bReg,userCB,userData, appId));

   return AEE_SUCCESS;
}

AEEResult DSSEventHandler::InitEventData
(
   EventData* pEd,
   bool bReg,
   dss_iface_ioctl_event_cb userCB,
   void* userData,
   sint15 appId
)
{
   // if we are trying to register and we are already registered
   if (bReg && pEd->bReg) {
      return QDS_EINPROGRESS;
   }

   // if we are trying to deregister and we not registered
   if (!bReg && !pEd->bReg) {
      return QDS_ENOTCONN;
   }

   pEd->bReg = bReg;
   pEd->userCB = userCB;
   pEd->userData = userData;
   pEd->appId = appId;

   return AEE_SUCCESS;

}

AEEResult DSSEventHandler::Register
(
   dss_iface_ioctl_event_enum_type event,
   dss_iface_ioctl_event_cb userCB,
   void* userData,
   dss_iface_id_type ifaceId
)
{
   AEEResult res = AEE_SUCCESS;
   sint15 appId = -1;

   LOG_MSG_FUNCTION_ENTRY_2("DSSEventHandler::Register(): event %d callback 0x%x", event, userCB);

   if (NULL == userCB) 
   {
      return QDS_EFAULT;
   }

   // get appId or policy flag
   appId = DSSIfaceId::GetAppIdOrPolicy(ifaceId);

   {
      DSSCritScope cs(*mpCritSect);

      res = SetEventData(event,
                         true,
                         userCB,
                         userData,
                         appId);

      if (AEE_SUCCESS != res) {
         if (QDS_EINPROGRESS == res ||
             QDS_ENOTCONN == res) {
            return res;
         } else {
            return QDS_EFAULT;
         }
      }
   }

   // Before calling this function, bReg flag need to be true.
   res = RegisterIDL();
   if (AEE_SUCCESS != res ) {
     // in case (QDS_EINUSE == res) the Signal is already on corresponding
     // signal bus - so we shall not clean event data
     if(QDS_EINUSE != res){
         DSSCritScope cs(*mpCritSect);

         // in case RegisterIDL API fails we are only interested
         // in result of RegisterIDL and not in result of SetEventData below
         // error messages are printed inside SetEventData in case of errors
         (void) SetEventData(event, false, 0, 0, 0);
     }else{
        // registration succeeded from the point of view of DSS
        res = AEE_SUCCESS;
     }
   }
   return res;
}

AEEResult DSSEventHandler::DeRegister(dss_iface_ioctl_event_enum_type event)
{
   DSSCritScope cs(*mpCritSect);
   AEEResult res;

   LOG_MSG_FUNCTION_ENTRY_1("DSSEventHandler::DeRegister(): event %d", event);

   res = SetEventData(event,
                      false,
                      0,
                      0,
                      0);

   return res;
}

/*lint -e{1551} */
void DSSEventHandler::Destructor() throw()
{
   DSSCommon::ReleaseIf((IQI**)&mpSignal);
   DSSCommon::ReleaseIf((IQI**)&mpSignalCtl);
   if (NULL != mpHandlerData){
      PS_SYSTEM_HEAP_MEM_FREE (mpHandlerData->mpEventData);
   }
   PS_SYSTEM_HEAP_MEM_FREE (mpHandlerData);
   if (NULL != mpHandlerDataClone){
      PS_SYSTEM_HEAP_MEM_FREE (mpHandlerDataClone->mpEventData);
   }
   PS_SYSTEM_HEAP_MEM_FREE (mpHandlerDataClone);
   DSSCommon::ReleaseIf((IQI**)&mpCritSect);

   SignalHandlerBase::Destructor();
}
/*lint -e{1531} */

/* should have compared argument against sizeof(class) - disabled since the comparison below is done according to the special memory management */
void * DSSEventHandler::operator new
(
   unsigned int numBytes
) throw()
{
   // We need to make sure that all the handlers are of the same size
   // and that they are equal to EVENT_HANDLER_SIZE
   if (EVENT_HANDLER_SIZE != numBytes){
      LOG_MSG_ERROR_1 ("new(): "
                       "Can't allocate, the size of the handler should be EVENT_HANDLER_SIZE = %d",
                       EVENT_HANDLER_SIZE);
      ASSERT(0);
      return NULL;
   }
   return ps_mem_get_buf(PS_MEM_DSAL_EVENT_HANDLER);
} /* DSSEventHandler::operator new() */
