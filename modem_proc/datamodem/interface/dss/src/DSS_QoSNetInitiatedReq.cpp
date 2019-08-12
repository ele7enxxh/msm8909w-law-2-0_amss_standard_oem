/*======================================================

FILE:  DSS_NetQoSNetInitiated.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of DSSQoSNetInitiatedReq class

=====================================================

Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoSNetInitiatedReq.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2011-06-07 sz  Created.

===========================================================================*/

//===================================================================
//   Includes and Public Data Declarations
//===================================================================

#include "comdef.h"

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "DSS_Common.h"
#include "dserrno.h"
#include "dss_iface_ioctl.h"

#include "DSS_QoSNetInitiated.h"
#include "DSS_QoSNetInitiatedReqHandler.h"
#include "DSS_Globals.h"
#include "DSS_CritScope.h"
#include "DSS_EventHandler.h"
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
//            DSSQoSNetInitiatedReq Functions Definitions
//===================================================================

// TODO: documentation
DSSQoSNetInitiatedReq::DSSQoSNetInitiatedReq(IQoSNetInitiatedReq* pQoSNetInitiatedReq, 
                                             dss_qos_net_initiated_req_handle_type  handle,
                                             dss_iface_id_type ifaceId):
   refCnt(1), weakRefCnt(1), mpQoSNetInitiatedReq(pQoSNetInitiatedReq), 
   mNext(NULL), mHandle(handle), mIfaceId(ifaceId), mpQoSNetInitiatedReqHandler(NULL),
   mParentNetActive(NULL), mNetHandle(0)
{
   if (NULL != pQoSNetInitiatedReq) {
      (void)mpQoSNetInitiatedReq->AddRef();
   }

   memset(&mCbType, 0, sizeof(dss_iface_ioctl_ev_cb_type));
}

AEEResult DSSQoSNetInitiatedReq::GetDSSNetActive(DSSNetActive** ppDSSNetActive) {
   if(!mParentNetActive->GetStrongRef())
   {
     LOG_MSG_ERROR_0("DSSQoSNetInitiatedReq::GetDSSNetActive(): "
                     "GetStrongRef failed");
     return AEE_EFAILED;
   }

   *ppDSSNetActive = mParentNetActive;
   return AEE_SUCCESS;
}

void DSSQoSNetInitiatedReq::SetDSSNetActive(DSSNetActive* pDSSNetActive) {
   mParentNetActive = pDSSNetActive;
   mParentNetActive->AddRefWeak();
}

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::~DSSQoSNetInitiatedReq
//
//  DESCRIPTION:
//  Destructor of the DSSQoSNetInitiatedReq class.
//===================================================================

/*lint -e{1551} */
 void DSSQoSNetInitiatedReq::Destructor() throw()
{
   LOG_MSG_INFO1_1 ("DSSQoSNetInitiatedReq::Destructor(): "
                    "Deleting object 0x%p", this);

   DS_UTILS_RELEASE_WEAKREF_IF(mParentNetActive);
   DSSCommon::ReleaseIf((IQI**)&mpQoSNetInitiatedReq);

   DSSGlobals::Instance()->ReleaseQoSNetInitiatedReqHandle(mHandle);

   PS_MEM_RELEASE(mpQoSNetInitiatedReqHandler);
}
/*lint –restore */

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::InsertToSecList
//
//  DESCRIPTION:
//
//===================================================================

void DSSQoSNetInitiatedReq::InsertToList(DSSQoSNetInitiatedReq* pDSSQoSNetInitiated)
{
   mNext = pDSSQoSNetInitiated;
}

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::GetHandle
//
//  DESCRIPTION:
//
//===================================================================
void DSSQoSNetInitiatedReq::GetHandle(dss_qos_net_initiated_req_handle_type* pHandle)
{
   *pHandle = mHandle;
}
//===================================================================

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::SetHandle
//
//  DESCRIPTION:
//
//===================================================================
void DSSQoSNetInitiatedReq::SetHandle(dss_qos_net_initiated_req_handle_type handle)
{
   mHandle = handle;
}

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::GetIfaceId
//
//  DESCRIPTION:
//
//===================================================================
void DSSQoSNetInitiatedReq::GetIfaceId(dss_iface_id_type* pIfaceId)
{
   *pIfaceId = mIfaceId;
}

//===================================================================


//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::Close
//
//  DESCRIPTION:
//
//===================================================================
AEEResult DSSQoSNetInitiatedReq::Close()
{
   if (NULL == mpQoSNetInitiatedReq) {
      LOG_MSG_ERROR_0("DSSQoSNetInitiatedReq::Close(): "
                      "NULL value");
      return QDS_EINVAL;
   }

   return mpQoSNetInitiatedReq->Close();
}

//===================================================================
//  FUNCTION:   DSSQoSNetInitiatedReq::GetQoSNetInitiatedReq
//
//  DESCRIPTION:
//
//===================================================================
AEEResult DSSQoSNetInitiatedReq::GetQoSNetInitiatedReq(IQoSNetInitiatedReq** ppQoSNetInitiatedReq)
{
   if (NULL == mpQoSNetInitiatedReq) {
      LOG_MSG_ERROR_0("DSSQoSNetInitiatedReq::GetQoSNetInitiatedReq(): "
                      "NULL value");
      return QDS_EINVAL;
   }
   *ppQoSNetInitiatedReq = mpQoSNetInitiatedReq;
   (void)mpQoSNetInitiatedReq->AddRef();
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSQoSNetInitiatedReq::RegEventCB  
//
//  DESCRIPTION: Serves DSS_IFACE_IOCTL_REG_EVENT_CB
//===================================================================
AEEResult DSSQoSNetInitiatedReq::RegEventCB(dss_iface_ioctl_ev_cb_type* pEvArg,
                                            dss_iface_id_type ifaceId)
{
   DSSEventHandler* ppEventHandler = 0;
   IDS_ERR_RET(GetEventHandler(pEvArg->event, &ppEventHandler, true));

   // There is a single app CB for QoSNetInitiatedReq throughout the lifetime
   // of this object.
   if(mCbType.app_id == 0)
   {
     mCbType.app_id = pEvArg->app_id;
     mCbType.event_cb = pEvArg->event_cb;
     mCbType.user_data_ptr = pEvArg->user_data_ptr;
   }
   mCbType.event = pEvArg->event;

   // Register to the event.
   IDS_ERR_RET(ppEventHandler->Register(pEvArg->event, pEvArg->event_cb, pEvArg->user_data_ptr, ifaceId));

   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSQoSNetInitiatedReq::RegCB  
//
//  DESCRIPTION: Initiates member variables
//===================================================================
AEEResult DSSQoSNetInitiatedReq::RegCB(dss_iface_ioctl_ev_cb_type* pEvArg)
{
   mCbType.app_id = pEvArg->app_id;
   mCbType.event_cb = pEvArg->event_cb;
   mCbType.user_data_ptr = pEvArg->user_data_ptr;
   return AEE_SUCCESS;
}

//===================================================================
//  FUNCTION:    DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated
//
//  DESCRIPTION: Asks the DSSNetActive to create and add a QoSNetInitiated
//               objects to its list and registers QoS events on it.
//===================================================================
AEEResult DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(dss_qos_handle_type qos_handle)
{
   AEEResult res = AEE_SUCCESS;
   DSSWeakRefScope parentWeakRefScope;
   DSSQoSNetInitiated* pDSSQoSNetInitiated = NULL;
   dss_iface_ioctl_ev_cb_type localCbType;
   dss_qos_handle_type pHandle = 0;

   localCbType.app_id = mCbType.app_id;
   localCbType.event_cb = mCbType.event_cb;
   localCbType.user_data_ptr = mCbType.user_data_ptr;
   
   if(!parentWeakRefScope.Init(mParentNetActive)) {
      LOG_MSG_ERROR_1("DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(): "
                      "Obj 0x%p, Init failed", this);
      return AEE_EFAILED;
   }

   // Call DSSNetActive to Create the DSSQoSNetInitiated object and add it to its list
   res = mParentNetActive->AddQoSNetInitiated(this,
                                           qos_handle,
                                           &pDSSQoSNetInitiated);
   if ( (AEE_SUCCESS != res) || (NULL == pDSSQoSNetInitiated) ) {
      LOG_MSG_ERROR_3("DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(): "
                      "AddQoSNetInitiated failure, request Obj 0x%p, res = %d, obj = 0x%p",
                      this, res, pDSSQoSNetInitiated);
      return res;
   }
   
   // registering DSSIFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV will cause to register also:
   // DSSIFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV
   // DSSIFACE_IOCTL_QOS_UNAVAILABLE_EV
   localCbType.event = DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV;
   res = pDSSQoSNetInitiated->RegEventCB(&localCbType, qos_handle);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_3("DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(): "
                      "RegEventCB failed res %d event %d qos_handle 0x%x",
                      res, localCbType.event, qos_handle);
      goto bail;
   }

   // registering DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV will cause to register also:
   // DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV
   localCbType.event = DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV;
   res = pDSSQoSNetInitiated->RegEventCB(&localCbType, qos_handle);
   if (AEE_SUCCESS != res) {
     LOG_MSG_ERROR_3("DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(): "
                     "RegEventCB failed res %d event %d qos_handle 0x%x",
                     res, localCbType.event, qos_handle);
      goto bail;
   }

   localCbType.event = DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV;
   res = pDSSQoSNetInitiated->RegEventCB(&localCbType, qos_handle);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_3("DSSQoSNetInitiatedReq::CreateDSSQoSNetInitiated(): "
                      "RegEventCB failed res %d event %d qos_handle 0x%x",
                      res, localCbType.event, qos_handle);
      goto bail;
   }
   
   return AEE_SUCCESS;

bail:
   // release QoS since event registration failed
   (void)pDSSQoSNetInitiated->GetHandle(&pHandle);
   (void)mParentNetActive->RemoveDSSNetQoSNetInitiatedFromList(pHandle);
   return res;
}

//===================================================================
//  FUNCTION:    DSSQoSNetInitiatedReq::GetEventHandler
//
//  DESCRIPTION: Returns the DSSEventHandler for the specified event.
//               If bInit is true, this function will also initialize
//               the handler if it's not initialized.
//===================================================================
AEEResult DSSQoSNetInitiatedReq::GetEventHandler(dss_iface_ioctl_event_enum_type event,
                                                 DSSEventHandler** ppEventHandler,
                                                 bool bInit)
{
   switch (event) {
      case DSS_IFACE_IOCTL_QOS_NET_INITIATED_AVAILABLE_EV:
         return FetchHandler(&mpQoSNetInitiatedReqHandler, ppEventHandler, bInit);         

      default:
         LOG_MSG_ERROR_2("DSSQoSNetInitiatedReq::GetEventHandler(): "
                         "Obj 0x%p, unhandled event=%d case", this, event);
         return QDS_EFAULT;
   }
}

template<typename HandlerType>
AEEResult DSSQoSNetInitiatedReq::FetchHandler(HandlerType** pHandler, DSSEventHandler** ppEventHandler, bool bInit)
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


void * DSSQoSNetInitiatedReq::operator new
(
   unsigned int numBytes
)  throw()
{
   return ps_mem_get_buf( PS_MEM_DSAL_NET_QOS_NET_INITIATED_REQ_TYPE);
} /* DSSQoSNetInitiatedReq::operator new() */


void DSSQoSNetInitiatedReq::operator delete
(
   void *  bufPtr
)
{
   PS_MEM_FREE(bufPtr);
   return;
} /* DSSQoSNetInitiatedReq::operator delete() */
