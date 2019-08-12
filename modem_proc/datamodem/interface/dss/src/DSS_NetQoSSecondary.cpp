/*======================================================

FILE:  DSS_NetQoSSecondary.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSNetQoSSecondary class

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_NetQoSSecondary.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-04-18 en  History added.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "DSS_Common.h"
#include "dserrno.h"

#include "DSS_NetQoSSecondary.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandler.h"
#include "DSS_QoSHandler.h"
#include "DSS_QoSModifyHandler.h"
#include "DSS_QoSInfoCodeUpdatedHandler.h"
#include "DSS_MemoryManagement.h"
#include "ds_Errors_Def.h"

using namespace ds::Net;
using namespace ds::Error;
//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------

//===================================================================
//              Macro Definitions
//===================================================================

//===================================================================
//            DSSNetQoSSecondary Functions Definitions
//===================================================================

// TODO: documentation
DSSNetQoSSecondary::DSSNetQoSSecondary(IQoSSecondary* pNetQoSSecondary, uint32 flowID):
   refCnt(1), weakRefCnt(1), mpNetQoSSecondary(pNetQoSSecondary), mNext(NULL), mFlowID(flowID),
   mpNetQoSHandler(NULL),mpNetQoSModifyHandler(NULL),mpQoSInfoCodeUpdatedHandler(NULL), mparentNetActive(NULL)
{
   if (NULL != pNetQoSSecondary) {
      (void)mpNetQoSSecondary->AddRef();
   }
}

AEEResult DSSNetQoSSecondary::GetDSSNetActive(DSSNetActive** ppDSSNetActive) {
   if(!mparentNetActive->GetStrongRef())
   {
     return AEE_EFAILED;
   }

   *ppDSSNetActive = mparentNetActive;
   return AEE_SUCCESS;
}

void DSSNetQoSSecondary::SetDSSNetActive(DSSNetActive* pDSSNetActive) {
   mparentNetActive = pDSSNetActive;
   mparentNetActive->AddRefWeak();
}

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::~DSSNetQoSSecondary
//
//  DESCRIPTION:
//  Destructor of the DSSNetQoSSecondary class.
//===================================================================

/*lint -e{1551} */
 void DSSNetQoSSecondary::Destructor() throw()
{
   LOG_MSG_FUNCTION_ENTRY_1 ("DSSNetQoSSecondary::Destructor()"
                             "Obj 0x%p", this);

   DS_UTILS_RELEASE_WEAKREF_IF(mparentNetActive);
   DSSCommon::ReleaseIf((IQI**)&mpNetQoSSecondary);

   // Pass the 8 LSBits of mFlowID to ReleaseFlowID since they
   // represent the flow ID that is now being released.
   DSSGlobals::Instance()->ReleaseQoSFlowID((uint8)(mFlowID & 0xFF));

   PS_MEM_RELEASE(mpNetQoSHandler);
   PS_MEM_RELEASE(mpNetQoSModifyHandler);
   PS_MEM_RELEASE(mpQoSInfoCodeUpdatedHandler);
}
/*lint –restore */

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::InsertToSecList
//
//  DESCRIPTION:
//
//===================================================================

void DSSNetQoSSecondary::InsertToList(DSSNetQoSSecondary* pDSSQoS)
{
   mNext = pDSSQoS;
}

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::GetFlowID
//
//  DESCRIPTION:
//
//===================================================================
void DSSNetQoSSecondary::GetFlowID(uint32* pFlowID)
{
   *pFlowID = mFlowID;
}

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::GetHandle
//
//  DESCRIPTION:
//
//===================================================================
void DSSNetQoSSecondary::GetHandle(dss_qos_handle_type* pHandle)
{
   *pHandle = mFlowID;
}

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::GetQoSSecondary
//
//  DESCRIPTION:
//
//===================================================================
AEEResult DSSNetQoSSecondary::GetNetQoSSecondary(IQoSSecondary** ppNetQoSSecondary)
{
   if (NULL == mpNetQoSSecondary) {
      LOG_MSG_ERROR_1("DSSNetQoSSecondary::GetNetQoSSecondary():"
                      "Obj 0x%p, mpNetQoSSecondary is NULL", this);
      return QDS_EINVAL;
   }
   *ppNetQoSSecondary = mpNetQoSSecondary;
   (void)mpNetQoSSecondary->AddRef();
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:   DSSNetQoSSecondary::GetQoS
//
//  DESCRIPTION:
//
//===================================================================
AEEResult DSSNetQoSSecondary::GetQoS(IQoS** ppQoS)
{
   if (NULL == mpNetQoSSecondary) {
      LOG_MSG_ERROR_1("DSSNetQoSSecondary::GetQoS():"
                      "Obj 0x%p, mpNetQoSSecondary is NULL", this);
      return QDS_EINVAL;
   }
   *ppQoS = mpNetQoSSecondary;
   (void)mpNetQoSSecondary->AddRef();
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetQoSSecondary::RegEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB
//===================================================================
AEEResult DSSNetQoSSecondary::RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                                         dss_iface_id_type ifaceId)
{
   //TODO: implement
   // outline: ???
   // (1)(a) create a network, if necessary.
   // (1)(b) find the right DSSEventHandler instance

   // Done in DSSEventHandler:
   // (2) fetch the right interface for the event.
   // (3) register for the event using a single ISignal (per event "group").
   // (4) save the user's callback, it's just a CB/signal pair because only one CB is allowed per app.
   // (5) when the event comes, the handler dispatches the callback with the needed data.


   DSSEventHandler* ppEventHandler = 0;
   IDS_ERR_RET(GetEventHandler(pEvArg->event, &ppEventHandler, true));

   // Register to the event.
   IDS_ERR_RET(ppEventHandler->Register(pEvArg->event, pEvArg->event_cb, pEvArg->user_data_ptr, ifaceId));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetQoSSecondary::GetEventHandler
//
//  DESCRIPTION: Returns the DSSEventHandler for the specified event.
//               If bInit is true, this function will also initialize
//               the handler if it's not initialized.
//===================================================================
AEEResult DSSNetQoSSecondary::GetEventHandler(dss_iface_ioctl_event_enum_type event,
                                              DSSEventHandler** ppEventHandler,
                                              bool bInit)
{
   switch (event) {
      case DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV:
      case DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV:
      case DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV:
         return FetchHandler(&mpNetQoSHandler, ppEventHandler, bInit);
      case DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV:
      case DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV:
         return FetchHandler(&mpNetQoSModifyHandler, ppEventHandler, bInit);
      case DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV:
         return FetchHandler(&mpQoSInfoCodeUpdatedHandler, ppEventHandler, bInit);

      default:
         LOG_MSG_ERROR_2("DSSNetQoSSecondary::GetEventHandler(): "
                         "Obj 0x%p, unhandled event=%d case", this, event);
         return QDS_EFAULT;
   }
}

template<typename HandlerType>
AEEResult DSSNetQoSSecondary::FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit)
{
   if (bInit) {
      if (NULL == *pHandler) {
         *pHandler = HandlerType::CreateInstance();
         if (NULL == *pHandler) {
           return AEE_ENOMEMORY;
         }
         IDS_ERR_RET((*pHandler)->Init(this));
      }
   } else {
      // bInit is false , we expect pHandler to have a value here
      if (0 == *pHandler) {
         return QDS_EFAULT;
      }
   }
   *ppEventHandler = *pHandler;
   return AEE_SUCCESS;
}


void * DSSNetQoSSecondary::operator new
(
   unsigned int numBytes
)  throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_QOS_SECONDARY_TYPE);
} /* DSSNetQoSSecondary::operator new() */


void DSSNetQoSSecondary::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSNetQoSSecondary::operator delete() */
