/*===========================================================================
  FILE: QoSNetInitiatedReq.cpp

  OVERVIEW: This file provides implementation of the QoSNetInitiatedReq class.

  DEPENDENCIES: None

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSNetInitiatedReq.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2011-06-05 sz  Created module.

===========================================================================*/
#include "comdef.h"

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ps_iface_defs.h"
#include "ds_Net_QoSNetInitiatedReq.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_IQoSNetInitiated.h"

using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace ds::Error;
using namespace NetPlatform;

/*-------------------------------------------------------------------------
List Node for mMatchingFlowHandlesQueue used in QoSNetInitiatedReq
-------------------------------------------------------------------------*/
class FlowHandle: public ds::Utils::INode 
{
public:
  FlowHandle(int32 flowHandle): 
    refCnt(1),
    weakRefCnt(1), 
    mFlowHandle(flowHandle)
  {
  
  }

  void Destructor() 
  {

  }

  inline int32 GetHandle()
  {
    return mFlowHandle;
  }

  /* Inherited from INode */
  virtual boolean Process (void*  pUserData)
  {
    (void) pUserData;
    
    return TRUE;
  }

  DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

  void * operator new
  (
    unsigned int numBytes
  )
  throw()
  {
    void * buf = NULL;
    PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void*);
    return buf;
  }

  void operator delete
  (
    void *  bufPtr
  )
  throw()
  {
    PS_SYSTEM_HEAP_MEM_FREE(bufPtr);
  }

private:
  int32 mFlowHandle;
};

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  CONSTRUCTOR/DESTRUCTOR
---------------------------------------------------------------------------*/
QoSNetInitiatedReq::QoSNetInitiatedReq
(
  int32                         ifaceHandle,
  int32                         qosNetInitiatedReqHandle
)
  /*-----------------------------------------------------------------------
    QoSNetInitiatedReqHandle is mObjHandle for this class.
  -----------------------------------------------------------------------*/
: Handle (qosNetInitiatedReqHandle),
  mIfaceHandle (ifaceHandle),
  mpSigBusStateChange(NULL),
  mMatchingFlowHandlesQueue(NULL),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_3 ("QoSNetInitiatedReq::QoSNetInitiatedReq(): "
                   "Creating object 0x%p, if handle 0x%x, req handle 0x%x",
                   this, ifaceHandle, qosNetInitiatedReqHandle);

  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusStateChange);

  mMatchingFlowHandlesQueue = new Utils::List();

  if (AEE_SUCCESS != Handle::Init(EventManager::qosObjList))
  {
    LOG_MSG_ERROR_1 ("QoSNetInitiatedReq::QoSNetInitiatedReq(): "
                     "Handle::Init failed, Obj 0x%p", this);
    return;
  }

} /* QoSNetInitiatedReq::QoSNetInitiatedReq() */

void QoSNetInitiatedReq::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("QoSNetInitiatedReq::Destructor(): "
                   "Deleting object 0x%p", this);

  mpICritSect->Enter();

  // prevent new events from coming from ps_iface objects
    Close();

  DS_UTILS_RELEASEIF (mpSigBusStateChange);

  if (0 != mMatchingFlowHandlesQueue) {
    delete mMatchingFlowHandlesQueue;
  }

  //Should be last statement. Call destructor for the base class.
  Handle::Destructor();

  mpICritSect->Leave();

} /* QoSNetInitiatedReq::Destructor() */


QoSNetInitiatedReq::~QoSNetInitiatedReq
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSNetInitiatedReq::~QoSNetInitiatedReq(): Obj 0x%p",
                            this);
  //NO-OP: only used for freeing memory.

} /* QoSNetInitiatedReq::~QoSNetInitiatedReq() */


ds::ErrorType QoSNetInitiatedReq::OnStateChange 
(
 ::ISignal*            signalObj, 
 ds::Net::EventType    eventID,
 IQI**                 regObj
 )
{
  AEEResult res = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    We are intentionally registering this object for NIQ notifications 
    from ps_iface before asking DSS to process real time NIQs.
    This shall allow us to get events for additional NIQs in case Network
    initiates them while DSS is handling NIQ notifications as part of real
    time check.
  -------------------------------------------------------------------------*/
  res = Handle::OnStateChange(signalObj, eventID, regObj);
  if (AEE_SUCCESS != res) {
    LOG_MSG_ERROR_3 ("QoSNetInitiatedReq::OnStateChange(): "
                     "Handle::OnStateChange failed, Obj 0x%p, event %d, error %d",
                     this, eventID, res);
    return res;
  }

  /*-------------------------------------------------------------------------
    Real Time check for matching flows
  -------------------------------------------------------------------------*/
  if (mMatchingFlowHandlesQueue->Count() > 0)
  {
    (void) signalObj->AddRef();
    (void) signalObj->Set();
    (void) signalObj->Release();
  }

  return res;
}

int QoSNetInitiatedReq::Close
(
  void
)
{
  /* Specification for PS IOCTL */
  QoSNetInitiatedReqReleaseType  qosNetInitiatedReqReleaseInfo;     
  AEEResult res = AEE_SUCCESS;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset (&qosNetInitiatedReqReleaseInfo, 0, sizeof(qosNetInitiatedReqReleaseInfo));
  
  /* Get the handle for the PS QoS Network Initiated object */
  qosNetInitiatedReqReleaseInfo.handle = GetHandle();
  
  if (0 != qosNetInitiatedReqReleaseInfo.handle) {
    
  /*-------------------------------------------------------------------------
    Perform Network Initiated QoS Release Monitoring IOCTL on the iface handle.
  -------------------------------------------------------------------------*/
  res =  IfaceIoctl (mIfaceHandle,
                     IFACE_IOCTL_QOS_NET_INITIATED_RELEASE,
                     static_cast <void *> (&qosNetInitiatedReqReleaseInfo));

  if (AEE_SUCCESS != res)
  {
    LOG_MSG_ERROR_3("QoSNetInitiatedReq::Close(): "
                    "IFACE_IOCTL_QOS_NET_INITIATED_RELEASE failed, "
                    "Obj 0x%p request handle 0x%x result %d",
                    this, qosNetInitiatedReqReleaseInfo.handle, res);
  }

  SetHandle(0);
  }

  return res;
}

int QoSNetInitiatedReq::GetLastMatchingFlowHandle
(
  int32* pFlowHandle
)
{
  if (NULL == pFlowHandle)
  {
     LOG_MSG_ERROR_0 ("QoSNetInitiatedReq::GetLastMatchingFlowHandle(): "
                      "NULL pFlowHandle");
     return QDS_EFAULT;
  }

  mpICritSect->Enter();

  FlowHandle* pHandle = (FlowHandle*) mMatchingFlowHandlesQueue->PopFront();
  if (NULL == pHandle)
  {
    mpICritSect->Leave();
    LOG_MSG_ERROR_0 ("QoSNetInitiatedReq::GetLastMatchingFlowHandle(): "
                     "FlowHandleQueue is empty");
    return QDS_EFAULT;
  }
  
  *pFlowHandle = pHandle->GetHandle();
  /*-------------------------------------------------------------------
    Need to call release twice here, because the object was added to the 
    list with refCnt 1
  -------------------------------------------------------------------*/
  pHandle->Release();
  DS_UTILS_RELEASEIF(pHandle);
  mpICritSect->Leave();

   return AEE_SUCCESS;
}

boolean QoSNetInitiatedReq::IsPendingMatchingFlow
(
 void
)
{
  return (mMatchingFlowHandlesQueue->Count() > 0);
}

/*---------------------------------------------------------------------------
  Event processing.
---------------------------------------------------------------------------*/
boolean QoSNetInitiatedReq::Process
(
  void* pUserData
)
{
  EventInfoType* pEventInfo = NULL;
  ps_iface_net_initiated_qos_available_info_type* flowAddedEventInfo = NULL;
  int32 handle = 0;
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    User data should never be NULL for event processing.
  -------------------------------------------------------------------------*/
  if (NULL == pUserData)
  {
    ASSERT (0);
    return FALSE;
  }

  pEventInfo = static_cast <EventInfoType *> (pUserData);

  LOG_MSG_INFO1_5 ("QoSNetInitiatedReq::Process(): "
                   "obj 0x%p handle 0x%x psEventName %d eventName 0x%x user handle 0x%x",
                   this,
                   GetHandle(),
                   pEventInfo->psEventName,
                   pEventInfo->eventName,
                   pEventInfo->userHandle);

  mpICritSect->Enter();

  do
  {     
    handle = GetHandle();

    // pEventInfo->userHandle (unlike pEventInfo->handle) may be 0 for some events.
    // Not all events use this data field.
    // QoSNetInitiatedReq object handle may be 0 if we already called Close(),
    // see dss_iface_ioctl_qos_net_initiated_release().
    // We can call Close() without releasing the object
    // (to prevent processing of further events).
    // So this if statement (handle == 0) prevents a case where we called Close()
    // and then Process() events with pEventInfo->userHandle 0,
    // which are irrelevant for this object.
    // I.e. QoSNetInitiatedReq object shouldn't process events if its handle is 0.
    if (handle != pEventInfo->userHandle || handle == 0)
    {
      // event should be disregarded
      break;
    }

    /* Store the flowHandle. It shall be needed to create a corresponding
       ds::Net::QoSNetInitiated object upon QCMAPP/DSS request */
    flowAddedEventInfo = (ps_iface_net_initiated_qos_available_info_type*)
                           (pEventInfo->psEventInfo);
    FlowHandle* flowHandle = new FlowHandle((int32)flowAddedEventInfo->flow_ptr);
    if((NULL != flowHandle) && mMatchingFlowHandlesQueue->Count())
    {
       boolean res = FALSE;
       res = CheckDup( mMatchingFlowHandlesQueue, (int32 *)flowHandle);
       if(res)
       {
         delete flowHandle;
         break;
       }
     }
    (void) mMatchingFlowHandlesQueue->PushBack(flowHandle);

    /*-----------------------------------------------------------------------
     Event belongs to this handle. Call Notify()
    -----------------------------------------------------------------------*/
    Notify (pEventInfo->eventName);
  } while (0);

  mpICritSect->Leave();

  return TRUE;

} /* Process() */


ds::ErrorType QoSNetInitiatedReq::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == ppISigBus)
  {
    DATA_ERR_FATAL("QoSNetInitiatedReq::GetSignalBus(): NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED:
      *ppISigBus = mpSigBusStateChange;
      (void)(*ppISigBus)->AddRef();
      mpICritSect->Leave();
      return AEE_SUCCESS;

    default:
      *ppISigBus = NULL;
      mpICritSect->Leave();
      return QDS_EINVAL;
  }

} /* GetSignalBus() */

int QoSNetInitiatedReq::QueryInterface
(
 AEEIID iid,
 void **ppo
 )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("QoSNetInitiatedReq::QueryInterface"
                             "Obj 0x%p", this);

  switch (iid)
  {
  case AEEIID_IQoSNetInitiatedReq:
    *ppo = static_cast <IQoSNetInitiatedReq *> (this);
    (void) AddRef();
    break;

  case AEEIID_IQI:
    *ppo = reinterpret_cast <void *> (this);
    (void) AddRef ();
    break;

  default:
    LOG_MSG_ERROR_2 ("QoSNetInitiatedReq::QueryInterface(): "
                     "Class 0x%x unsupported", this, iid);
    return AEE_ECLASSNOTSUPPORT;
  }

  return AEE_SUCCESS;

}/* QueryInterface() */

ds::ErrorType QoSNetInitiatedReq::RealTimeCheck
(
  const QoSSpecNetInitiatedType* pQoSNetInitiatedSpec
)
{
  LOG_MSG_FUNCTION_ENTRY_2  ("QoSNetInitiatedReq::RealTimeCheck"
                             "Obj 0x%p, if handle 0x%x", this, mIfaceHandle);

  /* Specification for PS IOCTL */
  QoSNetInitiatedReqRealTimeCheckType  qosNetInitiatedReqRealTimeCheckInfo;
  ps_iface_ioctl_matching_flow_type* matching_flow_ptr = NULL;
  FlowHandle* pFlowHandle = NULL;
  int res = 0, res2 = 0;

  memset (&qosNetInitiatedReqRealTimeCheckInfo, 0, sizeof(qosNetInitiatedReqRealTimeCheckInfo));
  
  res = DS2PSQoSNetInitiatedRequestSpec(pQoSNetInitiatedSpec, 
                                        &qosNetInitiatedReqRealTimeCheckInfo.net_initiated_qos_spec);
  if (AEE_SUCCESS != res)
  {
    LOG_MSG_ERROR_1("QoSNetInitiatedReq::RealTimeCheck(): "
                    "DS2PSQoSNetInitiatedRequestSpec() failed, "
                    "result %d", res);
    return res;
  }

  mpICritSect->Enter();
  list_init(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  /*-------------------------------------------------------------------------
    Perform Network Initiated QoS Real Time Check IOCTL on the iface handle.
  -------------------------------------------------------------------------*/
  res2 =  IfaceIoctl (mIfaceHandle,
                     IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK,
                     static_cast <void *> (&qosNetInitiatedReqRealTimeCheckInfo));
  
  if ((AEE_SUCCESS != res2) && (AEE_EWOULDBLOCK != res2))
  {
    LOG_MSG_ERROR_1("QoSNetInitiatedReq::RealTimeCheck(): "
                    "IFACE_IOCTL_QOS_NET_INITIATED_REAL_TIME_CHECK failed, "
                    "result %d", res2);
    goto bail;
  }
  matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
    list_pop_front(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  while (NULL != matching_flow_ptr) 
  {
    pFlowHandle = new FlowHandle((int32)matching_flow_ptr->flow_ptr);
    if (NULL == pFlowHandle) {
      LOG_MSG_ERROR_0("QoSNetInitiatedReq::RealTimeCheck(): "
                      "No memory for creating FlowHandle object");
      res = AEE_ENOMEMORY;
      goto bail;
    }

    mMatchingFlowHandlesQueue->PushBack(pFlowHandle);
    PS_SYSTEM_HEAP_MEM_FREE(matching_flow_ptr);
    matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
      list_pop_front(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  }

/* fall through */

bail:
  /*-------------------------------------------------------------------------
    Free the memory allocated in DS2PSQoSNetInitiatedRequestSpec
  -------------------------------------------------------------------------*/
  (void) CleanupPSQoSNetInitiatedSpec (&qosNetInitiatedReqRealTimeCheckInfo.net_initiated_qos_spec);

  /*-------------------------------------------------------------------------
    Empty the matching flow list and destroy it
  -------------------------------------------------------------------------*/
  matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
    list_pop_front(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  while (NULL != matching_flow_ptr) 
  {
    PS_SYSTEM_HEAP_MEM_FREE(matching_flow_ptr);
    matching_flow_ptr = (ps_iface_ioctl_matching_flow_type*)
      list_pop_front(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  }
  list_destroy(&qosNetInitiatedReqRealTimeCheckInfo.matching_flow_list);
  mpICritSect->Leave();
  
  LOG_MSG_FUNCTION_EXIT_3  ("QoSNetInitiatedReq::RealTimeCheck"
                            "Obj 0x%p, res %d, res2 %d", this, res, res2);

  if (AEE_EWOULDBLOCK == res2 )
  {
    res = AEE_EWOULDBLOCK;
  }
  return res;
}

boolean QoSNetInitiatedReq::CheckDup(ds::Utils::List *list, int32 *flow)
{
  FlowHandle *mflow = (FlowHandle *)flow;
  while(list->CurrentItem())
  {
    FlowHandle *mq = (FlowHandle *)list->CurrentItem();
    if(mflow->GetHandle() == mq->GetHandle())
    {
  	  return TRUE;
    }
	list->Next();
  }
  return FALSE;
}/* CheckDup */

