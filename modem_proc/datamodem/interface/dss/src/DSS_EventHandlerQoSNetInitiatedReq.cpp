/*======================================================

FILE:  DSS_EventHandlerQoSNetInitiatedReq.cpp

GENERAL DESCRIPTION:
   Implementation of DSS_EventHandlerQoSNetInitiatedReq functions

=====================================================

Copyright (c) 2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_EventHandlerQoSNetInitiatedReq.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-07-05 sz  Created.

===========================================================================*/

#include "comdef.h"

#include "DSS_EventHandlerQoSNetInitiatedReq.h"
#include "DSS_Globals.h"
#include "DSS_Common.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandlerNetApp.h"
#include "DSS_QoSNetInitiatedReq.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"


void DSSEventHandlerQoSNetInitiatedReq::DispatchCB(
   dss_iface_ioctl_event_enum_type event, 
   EventData* ped,
   dss_iface_ioctl_event_info_union_type* eventInfo)
{
   dss_iface_id_type ifaceId = 0;
   sint15 appID;

   // Application NetHandle was stored in DSSQoSNetInitiated upon its creation
   mpParent->GetNetHandle(&appID);

   // DSSNetApp iface id was stored in DSSQoSNetInitiated upon its creation
   mpParent->GetIfaceId(&ifaceId);

   LOG_MSG_INFO1_3("DSSEventHandlerQoSNetInitiatedReq::DispatchCB(): "
                   "DSS called App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
   ped->userCB(event, *eventInfo, ped->userData, appID, ifaceId);
   LOG_MSG_INFO1_3("DSSEventHandlerQoSNetInitiatedReq::DispatchCB(): "
                   "DSS returned from App callback, event: %d, ifaceId: %u, appID: %d",
                   event, ifaceId, appID);
}

AEEResult DSSEventHandlerQoSNetInitiatedReq::Init(DSSQoSNetInitiatedReq* parentNetAppParam)
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

void DSSEventHandlerQoSNetInitiatedReq::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mpParent);

   DSSEventHandler::Destructor();
}

