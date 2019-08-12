/*======================================================

FILE:  DSS_EventHandlerMCast.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_EventHandlerMCast functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerMCast.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "customer.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"

#include "DSS_EventHandlerMCast.h"
#include "DSS_MCast.h"
#include "DSS_EventHandlerNetApp.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"

void DSSEventHandlerMCast::DispatchCB(dss_iface_ioctl_event_enum_type event, EventData* ped,
                                 dss_iface_ioctl_event_info_union_type* eventInfo)
{

   dss_iface_id_type ifaceId = 0;
   mpParent->GetMCastHandle(&ifaceId);

   // take only the appID part of the iface ID (the middle 16 bits)
   sint15 appID =  (sint15)((ifaceId >> 8) & 0xFFFF);

   LOG_MSG_INFO1_3("DSSEventHandlerMCast::DispatchCB(): "
                   "DSS called App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
   ped->userCB(event, *eventInfo, ped->userData, appID, ifaceId);
   LOG_MSG_INFO1_3("DSSEventHandlerMCast::DispatchCB(): "
                   "DSS returned from App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
}

AEEResult DSSEventHandlerMCast::Init(DSSMCast* parentNetAppParam)
{
   AEEResult res = AEE_SUCCESS;
   ISignalFactory *piSignalFactory = NULL;

   mpParent = parentNetAppParam;
   (void) mpParent->AddRefWeak();
   
   IDS_ERR_RET(DSSGlobals::Instance()->GetSignalFactory(&piSignalFactory));
   IDS_ERR_RET(piSignalFactory->CreateSignal(&signalHandler,
                                             SignalCB,
                                             this,
                                             &mpSignal, &mpSignalCtl));

   IDS_ERR_RET(DSSGlobals::Instance()->GetCritSect(&mpCritSect));

   mpCritSect->Enter();
   res = RegisterIDL();
   mpCritSect->Leave();

   // piSignalFactory is singleton and does not require a release

   return res;
}

void DSSEventHandlerMCast::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mpParent);

   DSSEventHandler::Destructor();
}
#endif // FEATUTE_DATA_PS_MCAST
