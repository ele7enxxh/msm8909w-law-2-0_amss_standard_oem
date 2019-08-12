/*===========================================================================
  FILE: QoSManager.cpp

  OVERVIEW: This file provides implementation of the QoSManager class.

  DEPENDENCIES: None

  Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoSManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2015-06-24 pg  Added to support qos handle when registration to net initiated qos.
  2008-12-22 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_QoSManager.h"
#include "ds_Net_Platform.h"
#include "ds_Net_QoSFlowSpec.h"
#include "ds_Net_IPFilterSpec.h"
#include "ds_Net_QoSSecondary.h"
#include "ds_Net_QoSDefault.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Net_Conversion.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"
#include "ds_Net_QoSSecondariesOutput.h"
#include "ds_Net_QoSSecondariesInput.h"
#include "ds_Net_QoSNetInitiatedReq.h"
#include "ds_Net_QoSNetInitiated.h"
#include "ps_system_heap.h"

using namespace ds::Error;
using namespace ds::Net;
using namespace ds::Net::Conversion;
using namespace NetPlatform;

QoSManager::QoSManager
(
  int32            ifaceHandle,
  NetworkModeType  networkMode

)
: Handle (ifaceHandle),
  mNetworkMode(networkMode),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("QoSManager::QoSManager(): "
                            "Obj 0x%p, iface handle 0x%x, mode %d",
                            this, ifaceHandle, networkMode);

  //TODO Move to init function
  (void) DS_Utils_CreateInstance(0,
                                 AEECLSID_CSignalBus,
                                 (void **) &mpSigBusProfilesChanged);

  mpQoSDefault = NULL;

  if (AEE_SUCCESS != Handle::Init(EventManager::qosObjList))
  {
    LOG_MSG_ERROR_1 ("QoSManager::QoSManager(): "
                     "Handle::Init failed, Obj 0x%p", this);
    return;
  }

} /* QoSManager::QoSManager() */

void QoSManager::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::Destructor(): Obj 0x%p", this);

  mpICritSect->Enter();

  DS_UTILS_RELEASEIF (mpQoSDefault);
  DS_UTILS_RELEASEIF (mpSigBusProfilesChanged);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor of the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();

} /* QoSManager::Destructor() */

QoSManager::~QoSManager
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::~QoSManager(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* QoSManager::~QoSManager() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoSManager.
---------------------------------------------------------------------------*/
ds::ErrorType QoSManager::GetQosDefault
(
  ::IQoS **  ppIDSNetQoSDefault
)
{
  ds::ErrorType  result;
  int32          ifaceHandle;
  int32          flowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::GetQosDefault(): Obj 0x%p", this);

  if (0 == ppIDSNetQoSDefault)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::GetQosDefault(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (0 == mpQoSDefault)
    {
      /*---------------------------------------------------------------------
        Get the default flow handle for the iface
      ---------------------------------------------------------------------*/
      ifaceHandle = GetHandle();
      result = NetPlatform::PSGetDefaultFlow(ifaceHandle, &flowHandle);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_ERROR_2 ("QoSManager::GetQosDefault(): "
                         "Couldn't get default flow on handle 0x%x, obj 0x%p",
                         ifaceHandle, this);
        break;
      }

      mpQoSDefault =
        new QoSDefault(ifaceHandle, flowHandle, mNetworkMode);
      if (0 == mpQoSDefault)
      {
        LOG_MSG_ERROR_1 ("QoSManager::GetQosDefault(): "
                         "Failed to alloc QoSDefault, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }
    }

    (void) mpQoSDefault->AddRef();

    *ppIDSNetQoSDefault = static_cast <IQoS *> (mpQoSDefault);
    result              = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::GetQosDefault(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::GetQosDefault() */


ds::ErrorType QoSManager::RequestSecondary
(
  const QoSSpecType *  pQoSSpec,
  ::IQoSSecondary **   ppIDSNetQoSSecondary
)
{
  QoSSecondary *     pSecQoS = NULL;
  QoSRequestType     qosRequestInfo;
  PSQoSSpecType      localQoSSpec;
  ds::ErrorType      result;
  int32              ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::RequestSecondary(): Obj 0x%p", this);

  if (0 == ppIDSNetQoSSecondary || 0 == pQoSSpec)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::RequestSecondary(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset (&qosRequestInfo, 0, sizeof(qosRequestInfo));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::RequestSecondary(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Validate QoS spec and construct QOS_REQUEST IOCTL arguments.
    -----------------------------------------------------------------------*/

    qosRequestInfo.flow_ptr  = NULL;
    qosRequestInfo.subset_id = 0;
    qosRequestInfo.qos_ptr   = &localQoSSpec;

    result = DS2PSQoSRequestSpec (pQoSSpec, qosRequestInfo.qos_ptr);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("QoSManager::RequestSecondary(): Failed to convert "
                       "QoSSpec from DS to PS format, obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Perform QoS Request IOCTL on the iface handle to get the secondary flow
    -----------------------------------------------------------------------*/
    ifaceHandle = GetHandle();
    result = IfaceIoctl (ifaceHandle,
                         IFACE_IOCTL_QOS_REQUEST,
                         static_cast <void *> (&qosRequestInfo));

    if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
    {
      /*---------------------------------------------------------------------
        Get the err mask set in the QoS Request info argument
      ---------------------------------------------------------------------*/
      PS2DSQoSSpec (qosRequestInfo.qos_ptr,
                    const_cast <QoSSpecType *> (pQoSSpec));

      LOG_MSG_INFO3_1 ("QoSManager::RequestSecondary(): "
                       "Failed to request qos, obj 0x%p", this);
      break;
    }

    /*-----------------------------------------------------------------------
      Create a SecondaryQoS object using the this flow handle
    -----------------------------------------------------------------------*/
    pSecQoS = new QoSSecondary (ifaceHandle,
                                (int32) qosRequestInfo.flow_ptr,
                                mNetworkMode);
    if (0 == pSecQoS)
    {
      LOG_MSG_ERROR_1 ("QoSManager::RequestSecondary(): "
                       "Failed to alloc QoSDefault, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    *ppIDSNetQoSSecondary = static_cast <IQoSSecondary *> (pSecQoS);
     result               = AEE_SUCCESS;
  } while (0);

  /*-------------------------------------------------------------------------
    Free the memory allocated in DS2PSQoSRequestSpec and for qos_ptr.
  -------------------------------------------------------------------------*/
  (void) CleanupPSQoSRequestSpec (qosRequestInfo.qos_ptr);

  /*-------------------------------------------------------------------------
    Free memory in case of failure
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASEIF (pSecQoS);
  }

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::RequestSecondary(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::RequestSecondary() */

ds::ErrorType QoSManager::RequestBundle
(
  const QoSSpecType *       specs,
  int                       specsLen,
  QoSRequestOpCodeType      opCode,
  IQoSSecondariesOutput **  sessions
)
{
  QoSSecondary **         ppQoSSecondary = 0;
  QoSSecondariesOutput *  pQoSOutput = 0;
  QoSRequestExType        qosRequestExInfo;
  ds::ErrorType           result;
  int32                   index;
  int32                   ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSManager::RequestBundle(): Obj 0x%p, opcode %d",
                            this, opCode);

  if (0 == specs || 0 == sessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::RequestBundle(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (0 >= specsLen)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::RequestBundle(): "
                             "Invalid arg, obj 0x%p", this);
    return QDS_EINVAL;
  }

  memset (&qosRequestExInfo, 0, sizeof (QoSRequestExType));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::RequestBundle(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct QOS_REQUEST_EX_IOCTL arguments.
    -----------------------------------------------------------------------*/
    qosRequestExInfo.num_qos_specs     = (uint8) specsLen;
    qosRequestExInfo.subset_id         = 0;
    qosRequestExInfo.qos_control_flags = 0;

    if (opCode == QoSRequestOpCode::QDS_REQUEST)
    {
      qosRequestExInfo.opcode = PS_IFACE_IOCTL_QOS_REQUEST_OP;
    }
    else
    {
      qosRequestExInfo.opcode = PS_IFACE_IOCTL_QOS_CONFIGURE_OP;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for flow_ptrs array. Here we are allocating memory
      for holding only the pointers to ps flows and not the entire
      flow structures.
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(qosRequestExInfo.flows_ptr,
                              sizeof (ps_flow_type *) * specsLen,
                              ps_flow_type**);
    if (0 == qosRequestExInfo.flows_ptr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    /*-----------------------------------------------------------------------
      Allocate memory for specs_ptr array.
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC(qosRequestExInfo.qos_specs_ptr,
                             sizeof (qos_spec_type) * specsLen,
                             qos_spec_type*);
    if (0 == qosRequestExInfo.qos_specs_ptr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    /*-----------------------------------------------------------------------
      Fill the filter and flow specs into the bundled IOCTL request arg.
    -----------------------------------------------------------------------*/
    for (index = 0; index < specsLen; index++)
    {
      qosRequestExInfo.flows_ptr[index] = NULL;
      (void) DS2PSQoSRequestSpec (&specs[index],
                                  &qosRequestExInfo.qos_specs_ptr[index]);
    }

    /*-----------------------------------------------------------------------
      Allocate memory for secondary QoS objects array. Here we are allocating
      memory for only the array of pointers and not the QoSSecondary objects
      themselves.
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_ALLOC( ppQoSSecondary,
                              sizeof (QoSSecondary *) * specsLen,
                              QoSSecondary **);
    if (0 == ppQoSSecondary)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    memset (ppQoSSecondary, 0, specsLen * sizeof(QoSSecondary *));

    /*-----------------------------------------------------------------------
      Issue a QOS_REQUEST_EX_IOCTL to request for a bundle of flows.
    -----------------------------------------------------------------------*/
    ifaceHandle = GetHandle();
    result = IfaceIoctl (ifaceHandle,
                         IFACE_IOCTL_QOS_REQUEST_EX,
                         static_cast <void *> (&qosRequestExInfo));

    if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
    {
      for (index = 0; index < specsLen; index++)
      {
        /*-------------------------------------------------------------------
          Get the err mask set in the QoS Request info argument.
        -------------------------------------------------------------------*/
        PS2DSQoSSpec (&qosRequestExInfo.qos_specs_ptr[index],
                      const_cast <QoSSpecType*> (&specs[index]));
      }

      break;
    }

    /*-----------------------------------------------------------------------
      Create corresponding secondary flow objects (QoSSecondary objects)
      associated with these flows.
    -----------------------------------------------------------------------*/
    pQoSOutput = new QoSSecondariesOutput();
    if (0 == pQoSOutput)
    {
      LOG_MSG_ERROR_1 ("QoSManager::RequestBundle(): "
                       "Failed to alloc QoSSecondariesOutput, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    for (index = 0;
         index < qosRequestExInfo.num_qos_specs && index < specsLen;
         index++)
    {
      ppQoSSecondary[index] =
        new QoSSecondary (ifaceHandle,
                          (int32) qosRequestExInfo.flows_ptr[index],
                          mNetworkMode);
      if (0 == ppQoSSecondary[index])
      {
        LOG_MSG_ERROR_1 ("QoSManager::RequestBundle(): "
                         "Failed to alloc QoSSecondary, obj 0x%p", this);
        result = AEE_ENOMEMORY;
        break;
      }

      pQoSOutput->AddQoSSecondary(ppQoSSecondary[index]);
    }

    /*-----------------------------------------------------------------------
      Populate the out parameter with the IQoSSecondary interface.
    -----------------------------------------------------------------------*/
    *sessions = static_cast<IQoSSecondariesOutput *> (pQoSOutput);
    result    = AEE_SUCCESS;
  } while (0);

  /*-------------------------------------------------------------------------
    Clean up array of QoSSecondary objects in case of failure
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    if (0 != ppQoSSecondary)
    {
      for (index = 0;
           index < qosRequestExInfo.num_qos_specs && index < specsLen;
           index++)
      {
        DS_UTILS_RELEASEIF (ppQoSSecondary[index]);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Do the common clean up
  -------------------------------------------------------------------------*/
  for (index = 0;
       index < qosRequestExInfo.num_qos_specs && index < specsLen;
       index++)
  {
    (void) CleanupPSQoSRequestSpec (&qosRequestExInfo.qos_specs_ptr[index]);
  }

  PS_SYSTEM_HEAP_MEM_FREE (qosRequestExInfo.flows_ptr);
  PS_SYSTEM_HEAP_MEM_FREE (qosRequestExInfo.qos_specs_ptr);
  PS_SYSTEM_HEAP_MEM_FREE (ppQoSSecondary);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::RequestBundle(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::RequestBundle() */

ds::ErrorType QoSManager::RequestNetInitiatedReq
(
  const QoSSpecNetInitiatedType* pQoSNetInitiatedSpec,
  ::IQoSNetInitiatedReq** ppIDSNetQoSInitiatedReq
)
{
  int32               result;
  QoSNetInitiatedReq* pQoSNetInitiatedReq = NULL;

  /* Specification for PS IOCTL */
  QoSNetInitiatedRequestType  qosNetInitiatedRequestInfo;

  /* PS Network Initiated QoS specification */
  int32                       ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("QoSManager::RequestNetInitiatedReq(): "
                             "Obj 0x%p", this);

  if (NetworkMode::QDS_MONITORED == mNetworkMode)
  {
    return AEE_EUNSUPPORTED;
  }

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == ppIDSNetQoSInitiatedReq || NULL == pQoSNetInitiatedSpec)
  {
    LOG_MSG_ERROR_0 ("QoSManager::RequestNetInitiatedReq(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    Validate QoS spec and construct QOS_REQUEST IOCTL arguments.
  -------------------------------------------------------------------------*/
  memset (&qosNetInitiatedRequestInfo, 0, sizeof(qosNetInitiatedRequestInfo));

  result = DS2PSQoSNetInitiatedRequestSpec(pQoSNetInitiatedSpec,
                                           &qosNetInitiatedRequestInfo.net_initiated_qos_spec);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Perform Network Initiated monitoring QoS Request IOCTL on the iface handle.
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();

  result = IfaceIoctl (ifaceHandle,
                       IFACE_IOCTL_QOS_NET_INITIATED_REQUEST,
                       static_cast <void *> (&qosNetInitiatedRequestInfo));

  /*-------------------------------------------------------------------------
    Get the err mask set in the QoS Request info argument.
  -------------------------------------------------------------------------*/
  PS2DSQoSNetInitiatedSpec (
    &qosNetInitiatedRequestInfo.net_initiated_qos_spec,
    const_cast <QoSSpecNetInitiatedType*> (pQoSNetInitiatedSpec)
  );

  if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
  {
    mpICritSect->Leave();
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create a Net Initiated QoS object using the this flow handle.
  -------------------------------------------------------------------------*/
  pQoSNetInitiatedReq = new QoSNetInitiatedReq (
     ifaceHandle,
     (int32)qosNetInitiatedRequestInfo.handle /* Shall be used as handle to
                                                 identify events */
     );
  if (NULL == pQoSNetInitiatedReq)
  {
    result = AEE_ENOMEMORY;
    mpICritSect->Leave();
    goto bail;
  }

  result = pQoSNetInitiatedReq->RealTimeCheck(pQoSNetInitiatedSpec);
  if ((AEE_SUCCESS != result) && (AEE_EWOULDBLOCK != result))
  {
    mpICritSect->Leave();
    goto bail;
  }

  *ppIDSNetQoSInitiatedReq =
    static_cast <IQoSNetInitiatedReq *> (pQoSNetInitiatedReq);

  /*-------------------------------------------------------------------------
    Free the memory allocated in DS2PSQoSRequestSpec and for qos_ptr.
  -------------------------------------------------------------------------*/
  (void) CleanupPSQoSNetInitiatedSpec (&qosNetInitiatedRequestInfo.net_initiated_qos_spec);
  mpICritSect->Leave();

  return result;

bail:
  LOG_MSG_ERROR_1 ("QoSManager::RequestNetInitiatedReq(): "
                   "Err %d", result);
  (void) CleanupPSQoSNetInitiatedSpec (&qosNetInitiatedRequestInfo.net_initiated_qos_spec);
  DS_UTILS_RELEASEIF (pQoSNetInitiatedReq);
  *ppIDSNetQoSInitiatedReq = NULL;
  return result;

} /* RequestNetInitiatedReq() */

ds::ErrorType QoSManager::CreateQoSNetInitiated
(
  ::IQoSNetInitiatedReq* pQoSNetInitiatedReq,
  ::IQoSNetInitiated** ppiQoSNetInitiated
)
{
  int32            result;

  /* ps flow handle */
  int32            flowHandle;

  QoSNetInitiated* pQoSNetInitiated = NULL;

  //int32                       ifaceHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1  ("QoSManager::CreateQoSNetInitiated(): "
                             "Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    Validate arguments.
  -------------------------------------------------------------------------*/
  if (NULL == pQoSNetInitiatedReq || NULL == ppiQoSNetInitiated)
  {
    LOG_MSG_ERROR_0 ("QoSManager::CreateQoSNetInitiated(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Fetch the PS flow handle corresponding to the new Net Initiated QoS object.
  -------------------------------------------------------------------------*/
  mpICritSect->Enter();

  result = pQoSNetInitiatedReq->GetLastMatchingFlowHandle(&flowHandle);
  if (AEE_SUCCESS != result)
  {
    mpICritSect->Leave();
     LOG_MSG_ERROR_1 ("QoSManager::CreateQoSNetInitiated(): "
                      "Last matching flow handle is not available. Cannot create QoSNetInitiated. Obj 0x%p",
                      this);
     goto bail;
  }

  /*-------------------------------------------------------------------------
    Create a Net Initiated QoS object using the this flow handle.
  -------------------------------------------------------------------------*/
  pQoSNetInitiated = new QoSNetInitiated (
     GetHandle(),
     flowHandle,
     mNetworkMode);

  if (NULL == pQoSNetInitiated)
  {
    LOG_MSG_ERROR_0 ("QoSManager::CreateQoSNetInitiated(): "
                     "Failed to create QoSNetInitiated");
    result = AEE_ENOMEMORY;
    mpICritSect->Leave();
    goto bail;
  }

  *ppiQoSNetInitiated = static_cast <IQoSNetInitiated *> (pQoSNetInitiated);
  mpICritSect->Leave();

  return AEE_SUCCESS;

bail:
  DS_UTILS_RELEASEIF (pQoSNetInitiated);
  *ppiQoSNetInitiated = NULL;
  return result;
} /* CreateQoSNetInitiated() */


ds::ErrorType QoSManager::Close
(
  IQoSSecondariesInput *  qosSessions
)
{
  IQoSSecondary *        pIQoSSecondary = 0;
  QoSSecondary *         pQoSSecondary = 0;
  QoSSecondariesInput *  qosSecondariesInput;
  QoSReleaseExType       releaseExInfo;
  ds::ErrorType          result;
  int                    index;
  int32                  flowHandle;
  int                    sessionsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::Close(): Obj 0x%p", this);

  if (0 == qosSessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::Close(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset (&releaseExInfo, 0, sizeof(releaseExInfo));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::Close(): Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct QOS_RELEASE_EX_IOCTL arguments.
    -----------------------------------------------------------------------*/
    qosSecondariesInput = static_cast <QoSSecondariesInput *> (qosSessions);

    result = qosSecondariesInput->GetNumElements(&sessionsLen);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("QoSManager::Close(): "
                       "Failed to get # of elements, obj 0x%p", this);
      break;
    }


    releaseExInfo.num_flows = (uint8) sessionsLen;
    PS_SYSTEM_HEAP_MEM_ALLOC( releaseExInfo.flows_ptr,
                              sizeof (ps_flow_type*) * sessionsLen,
                              ps_flow_type**);
    if (0 == releaseExInfo.flows_ptr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    for (index = 0; index < sessionsLen; index++)
    {
      result = qosSecondariesInput->GetNth(index, &pIQoSSecondary);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_2 ("QoSManager::Close(): "
                         "Failed to get element at index %d, obj 0x%p",
                         index, this);
        result = AEE_ENOMEMORY;
        break;
      }

      pQoSSecondary = static_cast <QoSSecondary *> (pIQoSSecondary);
      releaseExInfo.flows_ptr[index] =
        (ps_flow_type *)(pQoSSecondary->GetHandle());
      DS_UTILS_RELEASEIF(pIQoSSecondary);
    }

    /*-----------------------------------------------------------------------
      This is a bundled IOCTL operation to be performed on a set of flows.
      However, since this is a flow IOCTL, we need to specify a single flow
      handle to drive the FlowIoctl function. The flow handle in this case
      would be the first flow the bundle.
    -----------------------------------------------------------------------*/
    flowHandle = (int32) releaseExInfo.flows_ptr[0];

    /*-----------------------------------------------------------------------
      Call bundled close for the secondary QoS flows.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (flowHandle,
                        FLOW_IOCTL_QOS_RELEASE_EX,
                        static_cast <void *> (&releaseExInfo));
    if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
    {
      break;
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE (releaseExInfo.flows_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::Close(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::Close() */

ds::ErrorType QoSManager::Resume
(
  IQoSSecondariesInput *  qosSessions
)
{
  IQoSSecondary *        pIQoSSecondary = 0;
  QoSSecondary *         pQoSSecondary = 0;
  QoSSecondariesInput *  qosSecondariesInput;
  QoSResumeExType        resumeExInfo;
  ds::ErrorType          result;
  int32                  flowHandle;
  int                    index;
  int                    sessionsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::Resume(): Obj 0x%p", this);

  if (0 == qosSessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::Resume(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset (&resumeExInfo, 0, sizeof(resumeExInfo));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::Resume(): Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct QOS_RESUME_EX_IOCTL arguments.
    -----------------------------------------------------------------------*/
    qosSecondariesInput = static_cast <QoSSecondariesInput *> (qosSessions);

    result = qosSecondariesInput->GetNumElements(&sessionsLen);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("QoSManager::Resume(): "
                       "Failed to get # of elements, obj 0x%p", this);
      break;
    }

    resumeExInfo.num_flows = (uint8) sessionsLen;
    PS_SYSTEM_HEAP_MEM_ALLOC( resumeExInfo.flows_ptr,
                              sizeof (ps_flow_type*) * sessionsLen, 
                              ps_flow_type**);
    if (0 == resumeExInfo.flows_ptr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    for (index = 0; index < sessionsLen; index++)
    {
      result = qosSecondariesInput->GetNth(index, &pIQoSSecondary);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_2 ("QoSManager::Resume(): "
                         "Failed to get element at index %d, obj 0x%p",
                         index, this);
        result = AEE_ENOMEMORY;
        break;
      }

      pQoSSecondary = static_cast <QoSSecondary *> (pIQoSSecondary);
      resumeExInfo.flows_ptr[index] =
        (ps_flow_type *) (pQoSSecondary->GetHandle());
      DS_UTILS_RELEASEIF(pIQoSSecondary);
    }

    /*-----------------------------------------------------------------------
      This is a bundled IOCTL operation to be performed on a set of flows.
      However, since this is a flow IOCTL, we need to specify a single flow
      handle to drive the FlowIoctl function. The flow handle in this case
      would be the first flow the bundle.
    -----------------------------------------------------------------------*/
    flowHandle = (int32) resumeExInfo.flows_ptr[0];

    /*-----------------------------------------------------------------------
      Call bundled resume for the secondary QoS flows.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (flowHandle,
                        FLOW_IOCTL_QOS_RESUME_EX,
                        static_cast <void *> (&resumeExInfo));
    if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
    {
      break;
    }
  } while (0);

  PS_SYSTEM_HEAP_MEM_FREE (resumeExInfo.flows_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::Resume(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::Resume() */


ds::ErrorType QoSManager::Suspend
(
  IQoSSecondariesInput *  qosSessions
)
{
  IQoSSecondary *        pIQoSSecondary = 0;
  QoSSecondary *         pQoSSecondary = 0;
  QoSSecondariesInput *  qosSecondariesInput;
  QoSSuspendExType       suspendExInfo;
  ds::ErrorType          result;
  int32                  flowHandle;
  int                    index;
  int                    sessionsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::Suspend(): Obj 0x%p", this);

  if (0 == qosSessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::Suspend(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset (&suspendExInfo, 0, sizeof(suspendExInfo));

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::Suspend(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct QOS_SUSPEND_EX_IOCTL arguments.
    -----------------------------------------------------------------------*/
    qosSecondariesInput = static_cast<QoSSecondariesInput *> (qosSessions);

    result = qosSecondariesInput->GetNumElements(&sessionsLen);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("QoSManager::Suspend(): "
                       "Failed to get # of elements, obj 0x%p", this);
      break;
    }

    suspendExInfo.num_flows = (uint8) sessionsLen;
    PS_SYSTEM_HEAP_MEM_ALLOC(suspendExInfo.flows_ptr,
                             sizeof (ps_flow_type*) * sessionsLen,
                             ps_flow_type** );
    if (0 == suspendExInfo.flows_ptr)
    {
      result = AEE_ENOMEMORY;
      break;
    }

    for (index = 0; index < sessionsLen; index++)
    {
      result = qosSecondariesInput->GetNth(index, &pIQoSSecondary);
      if (AEE_SUCCESS != result)
      {
        LOG_MSG_INFO3_2 ("QoSManager::Suspend(): "
                         "Failed to get element at index %d, obj 0x%p",
                         index, this);
        result = AEE_ENOMEMORY;
        break;
      }

      pQoSSecondary = static_cast <QoSSecondary *> (pIQoSSecondary);
      suspendExInfo.flows_ptr[index] =
        (ps_flow_type *) (pQoSSecondary->GetHandle());
      if (0 == suspendExInfo.flows_ptr[index])
      {
        LOG_MSG_ERROR_1 ("QoSManager::Suspend(): "
                         "flow handle is 0 while QoS still exists, QoS obj 0x%x",
                         pQoSSecondary);
        result = AEE_EBADHANDLE;
        DS_UTILS_RELEASEIF(pIQoSSecondary); // GetNth used addref
        goto bail;
      }
      DS_UTILS_RELEASEIF(pIQoSSecondary); // GetNth used addref
    }

    /*-----------------------------------------------------------------------
      This is a bundled IOCTL operation to be performed on a set of flows.
      However, since this is a flow IOCTL, we need to specify a single flow
      handle to drive the FlowIoctl function. The flow handle in this case
      would be the first flow the bundle.
    -----------------------------------------------------------------------*/
    flowHandle = (int32) suspendExInfo.flows_ptr[0];

    /*-----------------------------------------------------------------------
      Call bundled suspend for the secondary QoS flows.
    -----------------------------------------------------------------------*/
    result = FlowIoctl (flowHandle,
                        FLOW_IOCTL_QOS_SUSPEND_EX,
                        static_cast <void *> (&suspendExInfo));
    if (AEE_SUCCESS != result && AEE_EWOULDBLOCK != result)
    {
      break;
    }
  } while (0);
bail:
  PS_SYSTEM_HEAP_MEM_FREE (suspendExInfo.flows_ptr);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::Suspend(): Result 0x%x, obj 0x%p",
                           result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::Suspend() */


ds::ErrorType QoSManager::GetSupportedProfiles
(
  QoSProfileIdType *  profiles,
  int                 profilesLen,
  int *               profilesLenReq
)
{
  ds::ErrorType      result;
  int32              nNumOfItemsToCopy;

  ps_iface_ioctl_get_network_supported_qos_profiles_type  stTempVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSManager::GetSupportedProfiles(): "
                            "Obj 0x%p, profile len %d", this, profilesLen);

  if (0 == profiles && 0 != profilesLen)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::GetSupportedProfiles(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    /*-----------------------------------------------------------------------
      Perform iface IOCTL for GET_NETWORK_SUPPORTED_QOS_PROFILES on the iface
      handle associated with this QoS Manager object.
    -----------------------------------------------------------------------*/
    result = IfaceIoctl (GetHandle(),
                         IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
                         static_cast <void *> (&stTempVal));
    if (AEE_SUCCESS != result)
    {
      break;
    }

    if (0 != profilesLenReq)
    {
      *profilesLenReq = stTempVal.profile_count;
    }

    /*-----------------------------------------------------------------------
      Copy profiles in to OUT param
    -----------------------------------------------------------------------*/
    nNumOfItemsToCopy = MIN(stTempVal.profile_count, profilesLen);
    for (int i = 0; i < nNumOfItemsToCopy; i++)
    {
      profiles[i] = (QoSProfileIdType) stTempVal.profile_value[i];
    }
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::GetSupportedProfiles(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::GetSupportedProfiles() */

ds::ErrorType QoSManager::CreateQoSSecondariesInput
(
  IQoSSecondariesInput **  newQoSSecondariesInput
)
{
  QoSSecondariesInput *  qosSecondariesInput;
  ds::ErrorType          result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSManager::CreateQoSSecondariesInput(): Obj 0x%p",
                            this);

  if (0 == newQoSSecondariesInput)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::CreateQoSSecondariesInput(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      LOG_MSG_INFO1_2 ("QoSManager::CreateQoSSecondariesInput(): "
                       "Not supported in mode %d, obj 0x%p",
                       mNetworkMode, this);
      result = AEE_EUNSUPPORTED;
      break;
    }

    /*-----------------------------------------------------------------------
      Construct QoSSecondariesInput object.
    -----------------------------------------------------------------------*/
    qosSecondariesInput = new QoSSecondariesInput();
    if (0 == qosSecondariesInput)
    {
      LOG_MSG_ERROR_1 ("QoSManager::CreateQoSSecondariesInput(): "
                       "Failed to alloc QoSSecondariesInput, obj 0x%p", this);
      result = AEE_ENOMEMORY;
      break;
    }

    *newQoSSecondariesInput =
      static_cast <IQoSSecondariesInput *> (qosSecondariesInput);

    result = AEE_SUCCESS;
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::CreateQoSSecondariesInput(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::CreateQoSSecondariesInput() */


boolean QoSManager::Process
(
  void *  userDataPtr
)
{
  EventInfoType *  eventInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == userDataPtr)
  {
    LOG_MSG_ERROR_1 ("QoSManager::Process(): NULL event info, obj 0x%p", this);
    return FALSE;
  }

  eventInfoPtr = static_cast <EventInfoType *> (userDataPtr);

  mpICritSect->Enter();

  do
  {
    LOG_MSG_INFO1_4 ("Network::Process(): "
                     "Handle 0x%x, event 0x%x, event handle 0x%x, obj 0x%p",
                     GetHandle(),
                     eventInfoPtr->eventName,
                     eventInfoPtr->handle,
                     this);

    /*-----------------------------------------------------------------------
      Ignore QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED as it is related to
      Network Initiated QoS
    -----------------------------------------------------------------------*/
    if (QoSNetInitiatedReqEvent::QDS_EV_STATE_CHANGED ==
          eventInfoPtr->eventName)
    {
      break;
    }

    if (EVENT_GROUP_QOS == eventInfoPtr->eventGroup)
    {
      if (GetHandle() == eventInfoPtr->handle)
      {
        Notify(eventInfoPtr->eventName);
      }
    }
  } while (0);

  mpICritSect->Leave();
  return TRUE;

} /* QoSManager::Process() */


ds::ErrorType QoSManager::OnStateChange
(
  ISignal *           signalObj,
  ds::Net::EventType  eventName,
  IQI **              regObj
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3 ("QoSManager::OnStateChange(): "
                            "Obj 0x%p, event 0x%x, signal 0x%p",
                            this, eventName, signalObj);

  mpICritSect->Enter();

  do
  {
    if (NetworkMode::QDS_MONITORED == mNetworkMode)
    {
      if (QoSMgrEvent::QDS_EV_PROFILES_CHANGED != eventName)
      {
        result = AEE_EUNSUPPORTED;
        break;
      }
    }

    result = Handle::OnStateChange(signalObj, eventName, regObj);
  } while (0);

  LOG_MSG_FUNCTION_EXIT_2 ("QoSManager::OnStateChange(): "
                           "Result 0x%x, obj 0x%p", result, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::OnStateChange() */


ds::ErrorType QoSManager::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSManager::GetSignalBus(): Obj 0x%p, event 0x%x",
                            this, eventID);

  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSManager::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case QoSMgrEvent::QDS_EV_PROFILES_CHANGED:
    {
      *ppISigBus = mpSigBusProfilesChanged;
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSManager::GetSignalBus(): "
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
      LOG_MSG_INFO1_2 ("QoSManager::GetSignalBus(): "
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

  LOG_MSG_FUNCTION_EXIT_3 ("QoSManager::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p",
                           result, *ppISigBus, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::GetSignalBus() */


ds::ErrorType QoSManager::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoSManager::QueryInterface(): Obj 0x%p, iid 0x%x",
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
    case AEEIID_IQoSManager:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <IQoSManager *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_IQoSManagerPriv:
    {
      *ppo   = static_cast <IQoSManagerPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2 ("QoSManager::QueryInterface(): "
                               "Unknown iid 0x%x, obj 0x%p", iid, this);
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_3 ("QoSManager::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p",
                           result, *ppo, this);
  mpICritSect->Leave();

  return result;
} /* QoSManager::QueryInterface() */

ds::ErrorType QoSManager::RequestFiltrMatch
(
  void * fltr_info_ptr
)
{
   ds::ErrorType  result = AEE_SUCCESS;
   int32   ifaceHandle;
   
   /*-------------------------------------------------------------------------
    Perform Network Initiated monitoring QoS Request IOCTL on the iface handle.
  -------------------------------------------------------------------------*/
  ifaceHandle = GetHandle();

  result = IfaceIoctl (ifaceHandle,
                       IFACE_IOCTL_FILTER_MATCH_REQUEST,
                       static_cast <void *> (fltr_info_ptr));
  return result;
} /* QoSManager::RequestFiltrMatch() */

