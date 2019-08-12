/*======================================================

FILE:  DSS_MCast.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSS_MCast class

=====================================================

Copyright (c) 2008 - 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_MCast.cpp#1 $
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
#ifdef FEATUTE_DATA_PS_MCAST
#include "DSS_Common.h"
#include "dserrno.h"
#include "ds_Errors_Def.h"

#include "DSS_MCast.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandler.h"
#include "DSS_MemoryManagement.h"

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
DSSMCast::DSSMCast(IMCastSessionPriv* pMCastSession, uint32 MCastHandle):
   refCnt(1), weakRefCnt(1), mpNetMCastSession(pMCastSession), mNext(NULL), mMCastHandle(MCastHandle),
   mpMCastStatusHandler(NULL), mpMCastRegisterHandler1_0(NULL),
   mpMCastRegisterHandler2_0(NULL), mparentNetActive(NULL)
{
   if (NULL != pMCastSession) {
      (void)mpNetMCastSession->AddRef();
   }
}


AEEResult DSSMCast::GetDSSNetActive(DSSNetActive** ppDSSNetActive) {
   if(!mparentNetActive->GetStrongRef())
   {
      return AEE_EFAILED;
   }

   *ppDSSNetActive = mparentNetActive;
   return AEE_SUCCESS;
}

void DSSMCast::SetDSSNetActive(DSSNetActive* pDSSNetActive) {
   mparentNetActive = pDSSNetActive;
   mparentNetActive->AddRefWeak();
}


//===================================================================
//  FUNCTION:   DSSMCast::~DSSMCast
//
//  DESCRIPTION:
//  Destructor of the DSSMCast class.
//===================================================================
/*lint -e{1551} */
void DSSMCast::Destructor() throw()
{
   DS_UTILS_RELEASE_WEAKREF_IF(mparentNetActive);
   PS_MEM_RELEASE(mpMCastStatusHandler);
   PS_MEM_RELEASE(mpMCastRegisterHandler2_0);
   PS_MEM_RELEASE(mpMCastRegisterHandler1_0);
   // release the MCast Session
   //mpNetMCastSession->Close();
   DSSCommon::ReleaseIf((IQI**)&mpNetMCastSession);

   // Pass the 8 LSBits of mFlowID to ReleaseFlowID since they
   // represent the flow ID that is now being released.
   DSSGlobals::Instance()->ReleaseMCastFlowID((uint8)(mMCastHandle & 0xFF));

}
/*lint –restore */

//===================================================================
//  FUNCTION:   DSSMCast::InsertToSecList
//
//  DESCRIPTION:
//
//===================================================================

void DSSMCast::InsertToList(DSSMCast* pDSSMCast)
{
   mNext = pDSSMCast;
}

//===================================================================
//  FUNCTION:   DSSMCast::GetMCastHandle
//
//  DESCRIPTION:
//
//===================================================================
void DSSMCast::GetMCastHandle(uint32* pMCastHandle)
{
   *pMCastHandle = mMCastHandle;
}

//===================================================================
//  FUNCTION:   DSSMCast::RegEventCB
//
//  DESCRIPTION:
//
//===================================================================
AEEResult DSSMCast::RegEventCB(dss_iface_ioctl_event_cb cback_fn,
                               void* user_data,
                               dss_iface_ioctl_event_enum_type event,
                               dss_iface_id_type ifaceId)
{
   AEEResult res = AEE_SUCCESS;
   DSSEventHandler* ppEventHandler = NULL;

   switch (event) {
      case DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV:
      case DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV:
      case DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV:
        res = FetchHandler(&mpMCastRegisterHandler1_0, &ppEventHandler, true);
        if(AEE_SUCCESS != res){
          goto bail;
        }
        // Register to the event.
        IDS_ERR_RET(ppEventHandler->Register(event, cback_fn, user_data, ifaceId));
        res = FetchHandler(&mpMCastRegisterHandler2_0, &ppEventHandler, true);
        if(AEE_SUCCESS != res){
          goto bail;
        }
        // Register to the event.
        IDS_ERR_RET(ppEventHandler->Register(event, cback_fn, user_data, ifaceId));
        res = AEE_SUCCESS;
        break;

      case DSS_IFACE_IOCTL_MCAST_STATUS_EV:
        res = FetchHandler(&mpMCastStatusHandler, &ppEventHandler, true);
        if(AEE_SUCCESS != res){
          goto bail;
        }
        // Register to the event.
        IDS_ERR_RET(ppEventHandler->Register(event, cback_fn, user_data, ifaceId));
        res = AEE_SUCCESS;
        break;
        
      default:
        res = QDS_EFAULT;
        break;
   }

/* fall through */

bail:
   
   return res;
}

template<typename HandlerType>
AEEResult DSSMCast::FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit)
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

void * DSSMCast::operator new
(
   unsigned int numBytes
)  throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_MCAST_TYPE);
} /* DSSMCast::operator new() */


void DSSMCast::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSMCast::operator delete() */

boolean DSSMCast::DeregisterHandler
(
   ds::Net::EventType handlerType
)
{
   switch(handlerType)
   {
   case MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS1_0:
      PS_MEM_RELEASE(mpMCastRegisterHandler1_0);
   break;
   case MCastSessionPrivEvent::QDS_EV_REGISTRATION_STATE_PRIV_BCMCS2_0:
      PS_MEM_RELEASE(mpMCastRegisterHandler2_0);
   break;
   default:
   break;
   }
   /* in case both handlers are removed - DSSMCast object can be deleted*/
   if((0 == mpMCastRegisterHandler1_0) &&
     (0 == mpMCastRegisterHandler2_0)){
       return TRUE;
   }
   return FALSE;
}
#endif // FEATUTE_DATA_PS_MCAST
