/*===========================================================================
  FILE: QoSSecondary.cpp

  OVERVIEW: This file provides implementation of the QoSSecondary class.

  DEPENDENCIES: None

  Copyright (c) 2007-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSSecondary.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-03-25 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_QoSFlowSpec.h"
#include "ds_Net_QoSSecondary.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Network.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ds_Net_EventManager.h"

using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
QoSSecondary::QoSSecondary
(
  int32           ifaceHandle,
  int32           flowHandle,
  NetworkModeType networkMode
)
: QoS (ifaceHandle, flowHandle, networkMode),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4 ("QoSSecondary::QoSSecondary(): Obj 0x%p, "
                            "iface handle 0x%x, flow handle 0x%x, mode %d",
                            this, ifaceHandle, flowHandle, networkMode);

  //Move to Init function
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusInfoCodeUpdated);
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusModifyResult);
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusStateChange);

  if (AEE_SUCCESS != Handle::Init(EventManager::qosObjList))
  {
    LOG_MSG_ERROR_1 ("QoSSecondary::QoSSecondary(): "
                     "Handle::Init failed, Obj 0x%p", this);
    return;
  }

} /* QoSSecondary::QoSSecondary() */

void QoSSecondary::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondary::Destructor(): Obj 0x%p", this);

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpSigBusInfoCodeUpdated);
  DS_UTILS_RELEASEIF (mpSigBusModifyResult);
  DS_UTILS_RELEASEIF (mpSigBusStateChange);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  QoS::Destructor();

  mpICritSect->Leave();

} /* QoSSecondary::Destructor() */


QoSSecondary::~QoSSecondary
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondary::~QoSSecondary(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* QoSSecondary::~QoSSecondary() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoSSecondary.
---------------------------------------------------------------------------*/
ds::ErrorType QoSSecondary::Close
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondary::Close(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctl (GetHandle(), FLOW_IOCTL_QOS_RELEASE, NULL);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondary::Close(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::Close() */

ds::ErrorType QoSSecondary::ModifySecondaryPriv
(
  const QoSSpecType *  requestedSpec,
  QoSModifyMaskType    modifyMask
)
{
  QoSModifyType   modifyInfo;
  PSQoSSpecType   localQoSSpec;
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("QoSSecondary::ModifySecondaryPriv(): "
                             "Obj 0x%p", this);

  if (0 == requestedSpec)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondary::ModifySecondaryPriv(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Create IOCTL argument for QOS_MODIFY
  -------------------------------------------------------------------------*/
  memset (&modifyInfo, 0, sizeof (QoSModifyType));
  memset (&localQoSSpec, 0, sizeof (localQoSSpec));

  modifyInfo.subset_id = 0;
  modifyInfo.qos_ptr   = &localQoSSpec;

  /*-------------------------------------------------------------------------
    Create Spec according to the mask
  -------------------------------------------------------------------------*/
  result = DS2PSQoSRequestSpec(requestedSpec, modifyInfo.qos_ptr);
  if (AEE_SUCCESS != result)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondary::ModifySecondaryPriv(): Failed to "
                             "convert QoS spec from DS to PS format, obj 0x%p",
                             this);
    return result;
  }

  /*-------------------------------------------------------------------------
    If a RX_SPEC_DELETE mask is set then we should set QOS_MASK_RX_FLOW mask
    and IPFLOW_MASK_NONE should be set in the flow.
    No need to check the min_req_flow and
    aux flow specs here as this will checked by lower layers.
  -------------------------------------------------------------------------*/
  //TODO What is the need for validation here. All this will be done by
  //framework
  if (QoSModifyMask::QDS_RX_SPEC_DELETE & modifyMask)
  {
    modifyInfo.qos_ptr->field_mask |= (uint32) QOS_MASK_RX_FLOW;
    if ( (uint32) IPFLOW_MASK_NONE !=
            modifyInfo.qos_ptr->rx.flow_template.req_flow.field_mask )
    {
      LOG_MSG_ERROR_2 ("QoSSecondary::ModifySecondaryPriv(): "
                       "field_mask 0x%x, rx field_mask 0x%x",
                       modifyInfo.qos_ptr->field_mask,
                       modifyInfo.qos_ptr->rx.flow_template.req_flow.field_mask);
      return QDS_EINVAL;
    }

    modifyInfo.qos_ptr->rx.flow_template.req_flow.field_mask =
      (uint32) IPFLOW_MASK_NONE;
    modifyInfo.qos_ptr->rx.flow_template.num_aux_flows = 0;
  }

  if (QoSModifyMask::QDS_TX_SPEC_DELETE & modifyMask)
  {
    modifyInfo.qos_ptr->field_mask |= (uint32) QOS_MASK_TX_FLOW;
    if ( (uint32) IPFLOW_MASK_NONE !=
         modifyInfo.qos_ptr->tx.flow_template.req_flow.field_mask )
    {
      LOG_MSG_ERROR_2 ("QoSSecondary::ModifySecondaryPriv(): "
                       "field_mask 0x%x, tx field_mask 0x%x",
                       modifyInfo.qos_ptr->field_mask,
                       modifyInfo.qos_ptr->tx.flow_template.req_flow.field_mask);
      return QDS_EINVAL;
    }

    modifyInfo.qos_ptr->tx.flow_template.num_aux_flows = 0;
  }

  /*-------------------------------------------------------------------------
    If the RX_FILTER is set then we should modify the RX filters, thus
    a QOS_MODIFY_MASK_RX_FLTR_MODIFY must be set in the spec
    that is passed to PS according to the semantics all the
    filters (new and old) should be provided.
  -------------------------------------------------------------------------*/
  if (QoSModifyMask::QDS_RX_FILTER & modifyMask)
  {
    modifyInfo.qos_ptr->field_mask |= (uint32) QOS_MODIFY_MASK_RX_FLTR_MODIFY;
  }

  /*-------------------------------------------------------------------------
    Same for the case of TX
  -------------------------------------------------------------------------*/
  if (QoSModifyMask::QDS_TX_FILTER & modifyMask)
  {
    modifyInfo.qos_ptr->field_mask |= (uint32) QOS_MODIFY_MASK_TX_FLTR_MODIFY;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the flow IOCTL to modify the Qos session.
  -------------------------------------------------------------------------*/
  result = FlowIoctl  (GetHandle(),
                       FLOW_IOCTL_QOS_MODIFY,
                       static_cast <void *> (&modifyInfo));

  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1("QoSSecondary::ModifySecondaryPriv(): "
                    "FLOW_IOCTL_QOS_MODIFY failed, "
                    "result %d", result);
  }

  /*-------------------------------------------------------------------------
    Get the err mask set in the QoS Request info argument.
  -------------------------------------------------------------------------*/
  PS2DSQoSSpec (modifyInfo.qos_ptr,
                const_cast <QoSSpecType *> (requestedSpec));

  /*-------------------------------------------------------------------------
    Cleanup the dynamic memories allocated for the Qos specs.
  -------------------------------------------------------------------------*/
  (void) CleanupPSQoSRequestSpec (modifyInfo.qos_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondary::ModifyDefaultPriv(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::ModifySecondaryPriv() */

ds::ErrorType QoSSecondary::Resume
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondary::Resume(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctl (GetHandle(), FLOW_IOCTL_QOS_RESUME, NULL);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondary::Resume(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::Resume() */

ds::ErrorType QoSSecondary::Suspend
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondary::Suspend(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctl (GetHandle(), FLOW_IOCTL_QOS_SUSPEND, NULL);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondary::Suspend(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::Suspend() */

ds::ErrorType QoSSecondary::GetState
(
  QoSStateInfoType* argStatusInfo
)
{
  ds::ErrorType          result;
  PSFlowStateType        flowState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("QoSSecondary::GetState(): Obj 0x%p", this);

  if (0 == argStatusInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondary::GetState(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (0 == GetHandle())
    {
      LOG_MSG_INFO1_1 ("QoSSecondary::GetState(): NULL handle, obj 0x%p",
                       this);

      argStatusInfo->infoCode = QoSInfoCode::QDS_NOT_SPECIFIED;
      argStatusInfo->state    = QoSState::QDS_UNAVAILABLE;
      result                  = AEE_SUCCESS;

      break;
    }

    /*-----------------------------------------------------------------------
      Call the flow IOCTL to get the status of the Qos session.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (GetHandle(),
                        FLOW_IOCTL_QOS_GET_STATUS,
                        static_cast <void *> (&flowState));
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("QoSSecondary::GetState(): "
                      "FLOW_IOCTL_QOS_GET_STATUS failed, "
                      "result %d", result);
      break;
    }

    argStatusInfo->state = PS2DSFlowState(flowState);

    /*-----------------------------------------------------------------------
      Call the flow IOCTL to get the info code of the Qos session.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (GetHandle(),
                        FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE,
                        static_cast <void *> (&(argStatusInfo->infoCode)));
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1("QoSSecondary::GetState(): "
                      "FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE failed, "
                      "result %d", result);
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_4 ("QoSSecondary::GetState(): "
                           "Result 0x%x, state %d, info code %d, obj 0x%p",
                           result,
                           argStatusInfo->state,
                           argStatusInfo->infoCode,
                           this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::GetState() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoSPriv.
---------------------------------------------------------------------------*/
ds::ErrorType QoSSecondary::GetGrantedFlowSpecPriv
(
 ::IQoSFlowPriv ** rxFlow,
 ::IQoSFlowPriv ** txFlow
 )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QoS::GetGrantedFlowSpecInternal( FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2,
                                          rxFlow,
                                          txFlow);
} /* QoSSecondary::GetGrantedFlowSpecPriv() */


/*---------------------------------------------------------------------------
  Event processing.
---------------------------------------------------------------------------*/
boolean QoSSecondary::Process
(
  void *  pUserData
)
{
  EventInfoType *  pEventInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    User data should never be NULL for event processing.
  -------------------------------------------------------------------------*/
  if (0 == pUserData)
  {
    LOG_MSG_ERROR_1 ("QoSSecondary::Process(): NULL ev info, obj 0x%p", this);
    return FALSE;
  }

  pEventInfo = static_cast <EventInfoType *> (pUserData);

  /*-------------------------------------------------------------------------
    Ignore QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED as it is for
    Network Initiated QoS
  -------------------------------------------------------------------------*/
  if (QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED == pEventInfo->eventName)
  {
    return TRUE;
  }

  mpICritSect->Enter();

  LOG_MSG_INFO1_4 ("QoSSecondary::Process(): "
                   "Handle 0x%x, event 0x%x, event handle 0x%x, obj 0x%p",
                   GetHandle(),
                   pEventInfo->eventName,
                   pEventInfo->handle,
                   this);

  if (GetHandle() == pEventInfo->handle)
  {
    /*-----------------------------------------------------------------------
      For FLOW_NULL event, reset the handle before notifying.
    -----------------------------------------------------------------------*/
    if (FLOW_NULL_EV == pEventInfo->psEventName)
    {
      SetHandle (0);
    }

    Notify (pEventInfo->eventName);
  }

  mpICritSect->Leave();
  return TRUE;

} /* QoSSecondary()::Process() */


ds::ErrorType QoSSecondary::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSSecondary::GetSignalBus(): Obj 0x%p, event 0x%x",
                            this, eventID);

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondary::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case QoSEvent::QDS_EV_INFO_CODE_UPDATED:
    {
      *ppISigBus = mpSigBusInfoCodeUpdated;
      break;
    }

    case QoSEvent::QDS_EV_MODIFY_RESULT:
    {
      *ppISigBus = mpSigBusModifyResult;
      break;
    }

    case QoSEvent::QDS_EV_STATE_CHANGED:
    {
      *ppISigBus = mpSigBusStateChange;
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSSecondary::GetSignalBus(): "
                               "Unknown event 0x%x, obj 0x%p", eventID, this);
      result = QDS_EINVAL;
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Handle the case where event is supported by this object.
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS == result)
  {
    if (0 == *ppISigBus)
    {
      LOG_MSG_INFO1_2 ("MCastSession::GetSignalBus(): "
                       "NULL signal bus for event 0x%x, obj 0x%p",
                       eventID, this);
      result = QDS_EINVAL;
    }
    else
    {
      result = AEE_SUCCESS;
      (void) (*ppISigBus)->AddRef();
    }
  }

  LOG_MSG_FUNCTION_EXIT_3 ("QoSSecondary::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::GetSignalBus() */


ds::ErrorType QoSSecondary::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSSecondary::QueryInterface(): Obj 0x%p, iid 0x%x",
                            this, iid);

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::QueryInterface(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_IQoSSecondary:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <IQoSSecondary *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IQoSSecondaryPriv:
    {
      *ppo   = static_cast <IQoSSecondaryPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSSecondary::QueryInterface(): "
                               "Unknown iid 0x%x, obj 0x%p", iid, this);
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_3 ("QoSSecondary::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p",
                           result, *ppo, this);
  mpICritSect->Leave();

  return result;
} /* QoSSecondary::QueryInterface() */

int32 QoSSecondary::GetFlowPriv()
{
  return QoS::GetHandle();
} /* QoSSecondary::GetFlowPriv() */

