/*======================================================

FILE:  DSS_EventHandlerNetApp.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_EventHandlerNetApp functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerNetApp.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerNetApp.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandlerNetApp.h"
#include "DSS_IDSNetworkScope.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"


DSSEventHandlerNetApp::DSSEventHandlerNetApp() :
   DSSEventHandler(),
   mpParent(NULL)
{
}

void DSSEventHandlerNetApp::DispatchCB(dss_iface_ioctl_event_enum_type event, EventData* ped,
                                       dss_iface_ioctl_event_info_union_type* eventInfo)
{
   sint15 netHandle = -1;
   dss_iface_id_type ifaceId = 0;

   LOG_MSG_INFO1_1("DSSEventHandlerNetApp::DispatchCB(): "
                   "DispachCB of event number %d", event);

   if(NULL != ped)
   {
     // In case iface_id was cleared by IFACE_DOWN event
     // fetch the stale value.
     // Also, build iface id with policy bit on instead of app ID
     // in accordance with registration
     mpParent->GetIfaceIdForEventInfo(ped->appId, &ifaceId);

     mpParent->GetNetHandle(&netHandle);

     LOG_MSG_INFO1_3("DSSEventHandlerNetApp::DispatchCB(): "
                     "DSS called App callback, event: %d, ifaceId: %u, netHandle: %d",
                     event, ifaceId, netHandle);
      if((NULL != ped) && (NULL != ped->userCB))
      {
         ped->userCB(event, *eventInfo, ped->userData, netHandle, ifaceId);
         LOG_MSG_INFO1_3("DSSEventHandlerNetApp::DispatchCB(): "
                         "DSS returned from App callback, event: %d, ifaceId: %u, netHandle: %d",
                         event, ifaceId, netHandle);
      }
   }
}

AEEResult DSSEventHandlerNetApp::Init(DSSNetMonitored* parentNetAppParam,
                                      dss_iface_ioctl_event_enum_type event)
{
   ISignalFactory *piSignalFactory = NULL;

   mpParent = parentNetAppParam;
   (void) mpParent->AddRefWeak();

   IDS_ERR_RET(DSSGlobals::Instance()->GetSignalFactory(&piSignalFactory));
   IDS_ERR_RET(piSignalFactory->CreateSignal(&signalHandler,
                                             SignalCB,
                                             this,
                                             &mpSignal, &mpSignalCtl));

   IDS_ERR_RET(DSSGlobals::Instance()->GetCritSect(&mpCritSect));

   mEvent = event;

   // piSignalFactory is singleton and does not require a release

   return AEE_SUCCESS;
}

void DSSEventHandlerNetApp::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mpParent);

   DSSEventHandler::Destructor();
}

AEEResult DSSEventHandlerNetApp::RegisterIDL()
{
   DSSIDSNetworkScope IDSNetworkScope;
   DSSWeakRefScope WeakRefScope;
   IQI* pRegObj = NULL;

   if(!WeakRefScope.Init(mpParent)) {
      return AEE_EFAILED;
   }

   IDS_ERR_RET(IDSNetworkScope.Init(mpParent));

   LOG_MSG_INFO1_1("DSSEventHandler::RegisterIDL(): "
                   "Registering to %d, NetworkPriv obj 0x%p",
                   IDSNetworkScope.Fetch());

   AEEResult res = IDSNetworkScope.Fetch()->OnStateChange(mpSignal, mRegisteredIDLEvent, &pRegObj);
   // regObj mechanism is currently not in effect by dsnet layer.
   // No need for DSS to hold the allocated object to maintain the event registration.
   // For design simplicity we release the regObj immediately.
   // If and when dsnet layer enforces the registration object concept this code need to be adapted accordingly.
   DSSCommon::ReleaseIf(&pRegObj);

   return res;
}
