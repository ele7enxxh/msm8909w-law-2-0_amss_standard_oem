/*===========================================================================
  FILE: QoSDefault.cpp

  OVERVIEW: This file provides implementation of the QoSDefault class.

  DEPENDENCIES: None

  Copyright (c) 2007 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSDefault.cpp#1 $
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
#include "ds_Net_QoSDefault.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ds_Net_EventManager.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;


/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
QoSDefault::QoSDefault
(
  int32            ifaceHandle,
  int32            flowHandle,
  NetworkModeType  networkMode
)
: QoS (ifaceHandle, flowHandle, networkMode),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4 ("QoSDefault::QoSDefault(): Obj 0x%p, "
                            "iface handle 0x%x, flow handle 0x%x, mode %d",
                            this, ifaceHandle, flowHandle, networkMode);

  //TODO Move to Init function
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusInfoCodeUpdated);
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusModifyResult);
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusDefaultModify);

  if (AEE_SUCCESS != Handle::Init(EventManager::qosObjList))
  {
    LOG_MSG_ERROR_1 ("QoSDefault::QoSDefault(): "
                     "Handle::Init failed, Obj 0x%p", this);
    return;
  }

} /* QoSDefault::QoSDefault() */

void QoSDefault::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSDefault::Destructor(): Obj 0x%p", this);

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpSigBusInfoCodeUpdated);
  DS_UTILS_RELEASEIF (mpSigBusModifyResult);
  DS_UTILS_RELEASEIF (mpSigBusDefaultModify);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  QoS::Destructor();

  mpICritSect->Leave();

} /* QoSDefault::Destructor() */

QoSDefault::~QoSDefault
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSDefault::~QoSDefault(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* QoSDefault::~QoSDefault() */


/*---------------------------------------------------------------------------
   Handle interface forwarders:
---------------------------------------------------------------------------*/
ds::ErrorType QoSDefault::OnStateChange
(
  ::ISignal *         signalObj,
  ds::Net::EventType  eventID,
  IQI **              regObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("QoSDefault::OnStateChange(): "
                            "Obj 0x%p, event 0x%x, signal 0x%p",
                            this, eventID, signalObj);

  return Handle::OnStateChange(signalObj, eventID, regObj);
} /* QoSDefault::OnStateChange() */

/*---------------------------------------------------------------------------
  Inherited functions from IQoSDefaultPriv.
---------------------------------------------------------------------------*/
ds::ErrorType QoSDefault::ModifyDefaultPriv
(
  const QoSSpecPrimaryType *  requestedSpec,
  QoSModifyMaskType           mask
)
{
  PrimaryQoSModifyType           modifyInfo;
  PrimaryQoSModifySpecType       modifySpec;
  ds::ErrorType                  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSDefault::ModifyDefaultPriv(): Obj 0x%p", this);

  if (0 == requestedSpec)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSDefault::ModifyDefaultPriv(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Create arguments for PRIMARY_QOS_MODIFY ioctl.
  -------------------------------------------------------------------------*/
  memset (&modifySpec, 0, sizeof (modifySpec));
  modifyInfo.primary_qos_spec_ptr = &modifySpec;

  result = Conversion::DS2PSQoSModifySpec (requestedSpec->rxFlows,
                                           requestedSpec->rxFlowsLen,
                                           requestedSpec->txFlows,
                                           requestedSpec->txFlowsLen,
                                           mask,
                                           &modifySpec);
  if (AEE_SUCCESS != result)
  {
    LOG_MSG_ERROR_1 ("QoSDefault::ModifyDefaultPriv(): Failed to convert "
                     "QoSModifySpec from DS to PS format, obj 0x%p", this);
    return result;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Call the flow IOCTL to modify the primary QoS flow.
  -------------------------------------------------------------------------*/
  result = FlowIoctl (GetHandle(),
                      FLOW_IOCTL_PRIMARY_QOS_MODIFY,
                      static_cast <void *> (&modifyInfo));

  Conversion::CleanupPSQoSModifySpec (&modifySpec);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSDefault::ModifyDefaultPriv(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSDefault::ModifyDefaultPriv() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoS.
---------------------------------------------------------------------------*/
ds::ErrorType QoSDefault::GetTXPhysLink
(
  IPhysLink **  txPhysLinkObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QoS::GetTXPhysLink(txPhysLinkObj);
} /* QoSDefault::GetTXPhysLink() */

ds::ErrorType QoSDefault::GetRXPhysLink
(
  IPhysLink **  rxPhysLinkObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QoS::GetRXPhysLink(rxPhysLinkObj);
} /* QoSDefault::GetRXPhysLink() */

ds::ErrorType QoSDefault::GetUpdatedInfoCode
(
  QoSInfoCodeType *  infoCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QoS::GetUpdatedInfoCode(infoCode);
} /* QoSDefault::GetUpdatedInfoCode() */

ds::ErrorType QoSDefault::GetTXQueueLevel
(
  QoSTXQueueLevelType *  txQueueLevel
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSDefault::GetTXQueueLevel(): Obj 0x%p", this);

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSDefault::GetTXQueueLevel(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    result = QoS::GetTXQueueLevel(txQueueLevel);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSDefault::GetTXQueueLevel(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();
  return result;

} /* QoSDefault::GetTXQueueLevel() */

ds::ErrorType QoSDefault::GetTechObject
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSDefault::GetTechObject(): Obj 0x%p, iid 0x%x",
                            this, iid);

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSDefault::GetTechObject(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    result = QoS::GetTechObject(iid, ppo);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("QoSDefault::GetTechObject(): "
                           "Result 0x%x, ppo 0x%x, obj 0x%p",
                           result, *ppo, this);
  mpICritSect->Leave();

  return result;
} /* QoSDefault::GetTechObject() */

/*---------------------------------------------------------------------------
  Inherited functions from IQoS.
---------------------------------------------------------------------------*/
ds::ErrorType QoSDefault::GetGrantedFlowSpecPriv
(
 IQoSFlowPriv **  rxFlow,
 IQoSFlowPriv **  txFlow
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QoS::GetGrantedFlowSpecInternal
         (
           FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
           rxFlow,
           txFlow
         );
} /* QoSDefault::GetGrantedFlowSpecInternal() */

ds::ErrorType QoSDefault::GetModifyResult
(
  QoSResultType *  modifyResult
)
{
  ds::ErrorType  result;
  boolean        ioctlArg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSDefault::GetModifyResult(): Obj 0x%p", this);

  if (0 == modifyResult)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSDefault::GetModifyResult(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSDefault::GetModifyResult(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    result = FlowIoctl (GetHandle(),
                        FLOW_IOCTL_GET_PRIMARY_QOS_MODIFY_RESULT,
                        static_cast <void *> (&ioctlArg));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    if (TRUE == ioctlArg)
    {
      *modifyResult = QoSModifyResult::QDS_ACCEPTED;
    }
    else
    {
      *modifyResult = QoSModifyResult::QDS_REJECTED;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("QoSDefault::ModifyDefaultPriv(): "
                           "Result 0x%x, modify result %d, obj 0x%p",
                           result, *modifyResult, this);
  mpICritSect->Leave();

  return result;
} /* QoSDefault::GetModifyResult() */


ds::ErrorType QoSDefault::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSDefault::GetSignalBus(): Obj 0x%p, event 0x%x",
                            this, eventID);

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSDefault::GetSignalBus(): NULL arg, obj 0x%p",
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

    case QoSEvent::QDS_EV_MODIFIED:
      {
        *ppISigBus = mpSigBusDefaultModify;
        break;
      }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSDefault::GetSignalBus(): "
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
      LOG_MSG_INFO1_2 ("QoSDefault::GetSignalBus(): "
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

  LOG_MSG_FUNCTION_EXIT_3 ("QoSDefault::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);
  mpICritSect->Leave();

  return result;
} /* QoSDefault::GetSignalBus() */

ds::ErrorType QoSDefault::GetState
(
  QoSStateInfoType* state
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return AEE_EUNSUPPORTED;
} /* QoSDefault::GetState() */

ds::ErrorType QoSDefault::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSDefault::QueryInterface(): Obj 0x%p, iid 0x%x",
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
    case AEEIID_IQoS:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <IQoS *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IQoSDefaultPriv:
    {
      *ppo   = static_cast <IQoSDefaultPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSDefault::QueryInterface(): "
                               "Unknown iid 0x%x, obj 0x%p", iid, this);
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_3 ("QoSDefault::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p",
                           result, *ppo, this);
  mpICritSect->Leave();

  return result;
} /* QoSDefault::QueryInterface() */

int32 QoSDefault::GetFlowPriv()
{
  return QoS::GetHandle();
} /* QoSDefault::GetFlowPriv() */

