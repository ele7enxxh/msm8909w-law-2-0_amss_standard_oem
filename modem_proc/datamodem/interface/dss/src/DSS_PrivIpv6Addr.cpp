/*======================================================

FILE:  DSS_PrivIpv6Addr.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSPrivIpv6Addr class

=====================================================

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_PrivIpv6Addr.cpp#1 $
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
#include "DSS_CritScope.h"
#include "DSS_EventHandler.h"
#include "DSS_PrivIpv6Addr.h"
#include "DSS_PrivIpv6AddrHandler.h"
#include "DSS_MemoryManagement.h"

using namespace ds::Net;

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

DSSPrivIpv6Addr::DSSPrivIpv6Addr(IIPv6Address* pNetIpv6Address,uint32 flowID,boolean isUnique):
refCnt(1), weakRefCnt(1),
mpNetIpv6Address(pNetIpv6Address), mNext(NULL), mFlowID(flowID), mbIsUnique(isUnique), mpIpv6PrivAddrEventHandler(NULL), mparentNetActive(NULL)
{
   if (NULL != pNetIpv6Address) {
      (void)mpNetIpv6Address->AddRef();
   }
}

AEEResult DSSPrivIpv6Addr::GetDSSNetActive(DSSNetActive** ppDSSNetActive) {
   if(!mparentNetActive->GetStrongRef())
   {
      return AEE_EFAILED;
   }

   *ppDSSNetActive = mparentNetActive;
   return AEE_SUCCESS;
}

void DSSPrivIpv6Addr::SetDSSNetActive(DSSNetActive* pDSSNetActive) {
   mparentNetActive = pDSSNetActive;
   mparentNetActive->AddRefWeak();
}


//===================================================================
//  FUNCTION:   DSSPrivIpv6Addr::~DSSPrivIpv6Addr
//
//  DESCRIPTION:
//  Destructor of the DSSPrivIpv6Addr class.
//===================================================================

/*lint -e{1551} */
void DSSPrivIpv6Addr::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mparentNetActive);
   PS_MEM_RELEASE(mpIpv6PrivAddrEventHandler);
   DSSCommon::ReleaseIf((IQI**)&mpNetIpv6Address);
}
/*lint -restore */

//===================================================================
//  FUNCTION:   DSSPrivIpv6Addr::InsertToList
//
//  DESCRIPTION:
//
//===================================================================

void DSSPrivIpv6Addr::InsertToList(DSSPrivIpv6Addr* pDSSPrivIpv6Addr)
{
   mNext = pDSSPrivIpv6Addr;
}

//===================================================================
//  FUNCTION:    DSSPrivIpv6Addr::RegEventCB
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB
//===================================================================
AEEResult DSSPrivIpv6Addr::RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                                      dss_iface_id_type ifaceId)
{
   DSSEventHandler* ppEventHandler = 0;
   IDS_ERR_RET(GetEventHandler(pEvArg->event, &ppEventHandler, true));

   // Register to the event.
   IDS_ERR_RET(ppEventHandler->Register(pEvArg->event,
                                        pEvArg->event_cb,
                                        pEvArg->user_data_ptr,
                                        ifaceId));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSNetQoSSecondary::GetEventHandler
//
//  DESCRIPTION: Returns the DSSEventHandler for the specified event.
//               If bInit is true, this function will also initialize
//               the handler if it's not initialized.
//===================================================================
AEEResult DSSPrivIpv6Addr::GetEventHandler(dss_iface_ioctl_event_enum_type event, DSSEventHandler** ppEventHandler, bool bInit)
{
   switch (event) {
      case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV:
      case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV:
      case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV:
         return FetchHandler(&mpIpv6PrivAddrEventHandler, ppEventHandler, bInit);

      default:
        LOG_MSG_ERROR_1("DSSNetMonitored::GetEventHandler(): "
                        "Unhandled event=%d case", event);
         return QDS_EFAULT;
   }
}

template<typename HandlerType>
AEEResult DSSPrivIpv6Addr::FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit)
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


void * DSSPrivIpv6Addr::operator new
(
   unsigned int numBytes
)  throw()
{
   return ps_mem_get_buf(PS_MEM_DSAL_PRIV_IPV6_ADDR);
} /* DSSPrivIpv6Addr::operator new() */


void DSSPrivIpv6Addr::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSPrivIpv6Addr::operator delete() */
