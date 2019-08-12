/*======================================================

FILE:  DSS_EventHandlerQoS.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_EventHandlerQoS functions

=====================================================

Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerQoS.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

#include "DSS_EventHandlerQoS.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandlerNetApp.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"


void DSSEventHandlerQoS::DispatchCB(dss_iface_ioctl_event_enum_type event, EventData* ped,
                                 dss_iface_ioctl_event_info_union_type* eventInfo)
{
   dss_iface_id_type ifaceId = 0;

   mpParent->GetHandle(&ifaceId);

   // take only the appID part of the iface ID (the middle 16 bits)
   sint15 appID =  DSSIfaceId::GetAppId(ifaceId);

   LOG_MSG_INFO1_3("DSSEventHandlerQoS::DispatchCB(): "
                   "DSS called App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
   ped->userCB(event, *eventInfo, ped->userData, appID, ifaceId);
   LOG_MSG_INFO1_3("DSSEventHandlerQoS::DispatchCB(): "
                   "DSS returned from App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
}

AEEResult DSSEventHandlerQoS::Init(DSSQoS* parentNetAppParam)
{
   mpParent = parentNetAppParam;
   (void) mpParent->AddRefWeak();
   ISignalFactory *piSignalFactory = 0;
   IDS_ERR_RET(DSSGlobals::Instance()->GetSignalFactory(&piSignalFactory));
   IDS_ERR_RET(piSignalFactory->CreateSignal(&signalHandler,
                                             SignalCB,
                                             this,
                                             &mpSignal,
                                             &mpSignalCtl));

   IDS_ERR_RET(DSSGlobals::Instance()->GetCritSect(&mpCritSect));

   // piSignalFactory is singleton and does not require a release

   return AEE_SUCCESS;
}

void DSSEventHandlerQoS::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mpParent);

   DSSEventHandler::Destructor();
}
