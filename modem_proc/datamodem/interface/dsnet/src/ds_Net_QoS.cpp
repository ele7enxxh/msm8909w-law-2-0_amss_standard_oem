/*===========================================================================
  FILE: QoS.cpp

  OVERVIEW: This file provides implementation of the QoS class.

  DEPENDENCIES: None

  Copyright (c) 2007 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoS.cpp#1 $
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
#include "ds_Net_QoS.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_QoSFlowSpec.h"
#include "ds_Net_IQoS1x.h"
#include "ds_Net_QoS1X.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
QoS::QoS
(
  int32            ifaceHandle,
  int32            flowHandle,
  NetworkModeType  networkMode
) : Handle (flowHandle),
    mIfaceHandle (ifaceHandle),
    mpPhysLink (NULL),
    mpQoS1x(0),
    mNetworkMode(networkMode)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4 ("QoS::QoS(): Obj 0x%p, iface handle 0x%x, "
                            "flow handle 0x%x, mode %d",
                            this, ifaceHandle, flowHandle, networkMode);
} /* QoS::QoS() */

void QoS::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpPhysLink);
  DS_UTILS_RELEASEIF (mpQoS1x);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();

} /* QoS::Destructor() */

QoS::~QoS
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoS::~QoS(): ", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* QoS::~QoS() */

ds::ErrorType QoS::GetTXPhysLink
(
  ::IPhysLink **  txPhysLinkObj
)
{
  ds::ErrorType  result;
  int32          physLinkHandle;
  int32          flowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == txPhysLinkObj)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoS::GetTXPhysLink(): NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      A QoS object can at most be associated with only one flow object.
      If the phys link is already set, return the same.
    -----------------------------------------------------------------------*/
    if (0 != mpPhysLink)
    {
      *txPhysLinkObj = static_cast <IPhysLink *> (mpPhysLink);
      result         = AEE_SUCCESS;

      (void) mpPhysLink->AddRef();
      break;
    }

    /*-----------------------------------------------------------------------
      Get the phys link associated with the flow handle of this QoS object
      Get the flow handle for the QoS object.
    -----------------------------------------------------------------------*/
    flowHandle = GetHandle();
    result     = NetPlatform::PSGetPhysLinkFromFlow (flowHandle,
                                                     &physLinkHandle);
    if (AEE_SUCCESS != result || 0 == physLinkHandle)
    {
      /*---------------------------------------------------------------------
        Get the phys link from iface instead.
        This condition can happen if the iface is not yet brought up
        and hence the default flow is not bound to phys link.
        This is valid for default flow only
      ---------------------------------------------------------------------*/
      if (true == NetPlatform::IsPSFlowDefault(flowHandle))
      {
        result = NetPlatform::PSGetPhysLinkFromIface (mIfaceHandle,
                                                      &physLinkHandle);
        if (AEE_SUCCESS != result || 0 == physLinkHandle)
        {
          break;
        }
      }
      else // cannot fetch the valid handle from PS
      {
        break;
      }
    }

    /*-----------------------------------------------------------------------
      Create a new phys link object
    -----------------------------------------------------------------------*/
    mpPhysLink = new PhysLink (physLinkHandle, mNetworkMode);
    if (0 == mpPhysLink)
    {
      LOG_MSG_ERROR_1 ("QoS::GetTXPhysLink(): Failed to alloc PhysLink, "
                       "obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    *txPhysLinkObj = static_cast <IPhysLink *> (mpPhysLink);
    (void) mpPhysLink->AddRef();
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("QoS::GetTXPhysLink(): "
                           "Result 0x%x, tx phys link 0x%p, obj 0x%p",
                           result, *txPhysLinkObj, this);
  mpICritSect->Leave();

  return result;
} /* QoS::GetTXPhysLink() */


ds::ErrorType QoS::GetRXPhysLink
(
  ::IPhysLink **  rxPhysLinkObj
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Current implementation treats RX and TX as same phys links
  -------------------------------------------------------------------------*/
  return GetTXPhysLink(rxPhysLinkObj);

} /* QoS::GetRXPhysLink() */

ds::ErrorType QoS::GetModifyResult
(
  QoSResultType *  resultCode
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (GetHandle(),
                                FLOW_IOCTL_GET_MODIFY_RESULT,
                                static_cast <void *> (resultCode));

  LOG_MSG_FUNCTION_EXIT_2 ("QoS::GetModifyResult(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* QoS::GetModifyResult() */


ds::ErrorType QoS::GetUpdatedInfoCode
(
  QoSInfoCodeType *  infoCode
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (GetHandle(),
                                FLOW_IOCTL_GET_FLOW_UPDATED_INFO_CODE,
                                static_cast <void *> (infoCode));

  LOG_MSG_FUNCTION_EXIT_2 ("QoS::GetUpdatedInfoCode(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* QoS::GetUpdatedInfoCode() */

ds::ErrorType QoS::GetTXQueueLevel
(
  QoSTXQueueLevelType *  argTXQLevel
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (GetHandle(),
                                FLOW_IOCTL_GET_TX_QUEUE_LEVEL,
                                static_cast <void *> (argTXQLevel));

  LOG_MSG_FUNCTION_EXIT_2 ("QoS::GetTXQueueLevel(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* QoS::GetTXQueueLevel() */

ds::ErrorType QoS::GetGrantedFlowSpecInternal
(
  NetPlatform::FlowIoctlEnumType  ioctlKind,
  ::IQoSFlowPriv **               rxFlowObj,
  ::IQoSFlowPriv **               txFlowObj
)
{
  NetPlatform::QoSGetGrantedFlowSpecType    grantedFlowSpec;
  ds::ErrorType                             result;
  int32                                     flowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == txFlowObj || 0 == rxFlowObj)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoS::GetGrantedFlowSpecInternal(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (NetPlatform::FLOW_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2 != ioctlKind &&
      NetPlatform::FLOW_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC != ioctlKind)
  {
    LOG_MSG_INVALID_INPUT_2 ("QoS::GetGrantedFlowSpecInternal(): "
                             "Invalid ioctl kind %d, obj 0x%p",
                             ioctlKind, this);
    return QDS_EFAULT;
  }

  *txFlowObj = *rxFlowObj = NULL;

  mpICritSect->Enter();

  do
  {
    flowHandle = GetHandle();
    if (0 == flowHandle)
    {
      LOG_MSG_INFO1_1 ("QoS::GetGrantedFlowSpecInternal(): "
                       "NULL handle, obj 0x%p", this);
      result = QDS_EINVAL;
      break;
    }

    memset (&grantedFlowSpec, 0, sizeof(grantedFlowSpec));

    /*-----------------------------------------------------------------------
      Perform flow IOCTL to get granted flow spec.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (flowHandle,
                        ioctlKind,
                        static_cast <void *> (&grantedFlowSpec));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    /*-----------------------------------------------------------------------
      Convert to out args
    -----------------------------------------------------------------------*/
    *txFlowObj = static_cast <IQoSFlowPriv *>
                   (new QoSFlowSpec(&grantedFlowSpec.tx_ip_flow));
    *rxFlowObj = static_cast <IQoSFlowPriv *>
                   (new QoSFlowSpec(&grantedFlowSpec.rx_ip_flow));

    if (0 == *txFlowObj || 0 == *rxFlowObj)
    {
      LOG_MSG_ERROR_1 ("QoS::GetGrantedFlowSpecInternal(): "
                       "Failed to alloc QoSFlowSpec, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_5 ("QoS::GetGrantedFlowSpecInternal(): "
                           "Result 0x%x, tx flow spec 0x%p, "
                           "rx flow spec 0x%p, obj 0x%p, ioctlKind %d",
                           result, *txFlowObj, *rxFlowObj, this, ioctlKind);
  mpICritSect->Leave();

  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF (*txFlowObj);
    DS_UTILS_RELEASEIF (*rxFlowObj);
  }

  return result;
} /* QoS::GetGrantedFlowSpecInternal() */

ds::ErrorType QoS::GetQoS1x
(
  IQoS1x **  ppQoS1x
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppQoS1x)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoS::GetQoS1x(): NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Create QoS1x object and cache it in QoS object, if not cached before
    -----------------------------------------------------------------------*/
    if (0 == mpQoS1x)
    {
      mpQoS1x = new QoS1X(GetHandle());
      if (0 == mpQoS1x)
      {
        LOG_MSG_ERROR_1 ("QoS::GetQoS1x(): Failed to alloc QoS1x, obj 0x%p",
                         this);
        result = AEE_ENOMEMORY;
        break;
      }
    }

    *ppQoS1x = mpQoS1x;

    (void) mpQoS1x->AddRef();
  } while (0);

  LOG_MSG_FUNCTION_EXIT_3 ("QoS::GetQoS1x(): Result 0x%x, QoS 1x 0x%p, obj 0x%p",
                           result, *ppQoS1x, this);
  mpICritSect->Leave();

  return result;
} /* QoS::GetQoS1x() */

ds::ErrorType QoS::GetTechObject
(
  AEEIID   iid,
  void **  ppo
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoS::GetTechObject(): Obj 0x%p, iid 0x%x",
                            this, iid);

  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoS::GetTechObject(): NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (AEEIID_IQoS1x != iid)
  {
    LOG_MSG_INVALID_INPUT_2 ("QoS::GetTechObject(): Unknown iid ox%x, obj 0x%p",
                             iid, this);
    return AEE_ECLASSNOTSUPPORT;
  }

  return GetQoS1x((IQoS1x **) ppo);
} /* QoS::GetTechObject() */

int32 QoS::GetFlowhandle()
{
  return GetHandle();
} /* QoS::GetFlowhandle() */

