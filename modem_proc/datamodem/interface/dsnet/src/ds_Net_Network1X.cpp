/*===========================================================================
  FILE: Network1X.cpp

  OVERVIEW: This file provides implementation of the Network1X class.

  DEPENDENCIES: None

  Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_Network1X.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-30 kr  Free floating changes
  2008-03-10 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_Network1X.h"
#include "ds_Net_Platform.h"
#include "ds_Net_EventDefs.h"
#include "ds_Net_EventManager.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CSignalBus.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
Network1X::Network1X
(
  Network *  pParent,
  int32      objHandle
)
: Handle (objHandle),
  mpParent(NULL),
  mpSigBusHDRRev0RateInertia(0),
  mpSigBusHDRSlottedMode(0),
  mpSigBusHDRChangeSlottedMode(0),
  slotModeOpResult(0),
  rev0InertiaOpSuccess(0),
  rev0InertiaFailResult(0),
  refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network1X::Network1X(): "
                            "Obj 0x%p, handle 0x%x", this, objHandle);

  /* Store the parent, perform AddRefWeak() */
  ASSERT (NULL != pParent);

  mpParent = pParent;

  (void) mpParent->AddRefWeak();

} /* Network1X::Network1X() */

ds::ErrorType Network1X::Init
(
  void
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  do
  {
    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusHDRRev0RateInertia);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("Network1X::Init(): "
                       "Couldn't create SignalBus, obj 0x%x", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusHDRSlottedMode);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("Network1X::Init(): "
                       "Couldn't create SignalBus, obj 0x%x", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = DS_Utils_CreateInstance(0,
                                     AEECLSID_CSignalBus,
                                     (void **) &mpSigBusHDRChangeSlottedMode);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_INFO3_1 ("Network1X::Init(): "
                       "Couldn't create SignalBus, obj 0x%x", this);
      result = AEE_ENOMEMORY;
      break;
    }

    result = Handle::Init(EventManager::network1XObjList);
    if (AEE_SUCCESS != result)
    {
      LOG_MSG_ERROR_1 ("Network1X::Init():"
                       "Failed to init the Handle, obj 0x%p", this);
      break;
    }

    result = AEE_SUCCESS;
  } while (0);

  /*-------------------------------------------------------------------------
    Cleanup in case of error (::Destructor won't do this in case of error)
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != result)
  {
    DS_UTILS_RELEASE_WEAKREF_IF(mpParent);
    DS_UTILS_RELEASEIF (mpSigBusHDRRev0RateInertia);
    DS_UTILS_RELEASEIF (mpSigBusHDRSlottedMode);
    DS_UTILS_RELEASEIF (mpSigBusHDRChangeSlottedMode);
  }

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::Init(): Result 0x%x, obj 0x%p",
                           result, this);
  return result;

} /* Network1X::Init() */

void Network1X::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Due to 2 stage constructor pattern - Network1X::Init can fail.
    No need to free resources (including Handle::Destructor call)
    in this certain case as it is done in Network1X::Init
  -------------------------------------------------------------------------*/
  if (0 == mpICritSect)
  {
    LOG_MSG_ERROR_1("Network1X::Destructor(): Obj 0x%p "
                    "Network1X wasn't fully initialized", this);
    return;
  }

  mpICritSect->Enter();

  DS_UTILS_RELEASE_WEAKREF_IF(mpParent);
  DS_UTILS_RELEASEIF (mpSigBusHDRRev0RateInertia);
  DS_UTILS_RELEASEIF (mpSigBusHDRSlottedMode);
  DS_UTILS_RELEASEIF (mpSigBusHDRChangeSlottedMode);

  /*-------------------------------------------------------------------------
    Should be the last statement. Call destructor for the base class.
  -------------------------------------------------------------------------*/
  Handle::Destructor();

  mpICritSect->Leave();
} /* Network1X::Destructor() */


Network1X::~Network1X
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* Network1X::~Network1X() */


/*---------------------------------------------------------------------------
  Functions inherited from INetwork1X
---------------------------------------------------------------------------*/
ds::ErrorType Network1X::EnableHDRRev0RateInertia
(
  boolean enable
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_ENABLE_HDR_REV0_RATE_INERTIA,
                                 static_cast <void *> (&enable));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::EnableHDRRev0RateInertia(): "
                           "Result 0x%x, obj 0x%p, enable %d", result, this, enable);
  mpICritSect->Leave();

  return result;
} /* Network1X::EnableHDRRev0RateInteria() */

ds::ErrorType Network1X::GetHDRRev0RateInertiaResult
(
  boolean *  rev0InertiaOpResult
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == rev0InertiaOpResult)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetHDRRev0RateInertiaResult(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *rev0InertiaOpResult = rev0InertiaOpSuccess;

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetHDRSlottedModeResult()(): "
                           "HDR Rev0 Rate Inertia result %d, obj 0x%p",
                           *rev0InertiaOpResult, this);

  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network1X::GetHDRRev0RateInteriaResult() */

ds::ErrorType Network1X::GetHDRSlottedModeResult
(
  Network1xPrivResultCodeType *  resultCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == resultCode)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetHDRSlottedModeResult(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *resultCode = slotModeOpResult;

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetHDRSlottedModeResult()(): "
                           "HDR slotted mode result %d, obj 0x%p",
                           *resultCode, this);

  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network1X::GetHDRSlottedModeResult() */


ds::ErrorType Network1X::GetHDRSlottedModeCycleIndex
(
  uint32 *  sci
)
{
  Network1xPrivHDRSlottedModeArg  ioctlArg;
  ds::ErrorType                   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == sci)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetHDRSlottedModeCycleIndex(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  ioctlArg.getSlottedMode = TRUE;
  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE,
                                 static_cast <void *> (&ioctlArg));
  *sci = ioctlArg.slottedModeOption;

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetHDRSlottedModeCycleIndex()(): "
                           "SCI %d, obj 0x%p", *sci, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetHDRSlottedModeCycleIndex() */


ds::ErrorType Network1X::EnableHDRSlottedMode
(
  const Network1xPrivHDRSlottedModeArg *  argSlottedModeInfo
)
{
  Network1xPrivHDRSlottedModeArg  ioctlArg;
  ds::ErrorType                   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == argSlottedModeInfo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::EnableHDRSlottedMode(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    The function argument is constant whereas iface ioctl requires a
    non const argument.
  -------------------------------------------------------------------------*/
  ioctlArg = *argSlottedModeInfo;

  /*-------------------------------------------------------------------------
    Call the iface IOCTL for EnableHDRSlottedMode
  -------------------------------------------------------------------------*/
  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_707_ENABLE_HDR_SLOTTED_MODE,
                      static_cast <void *> (&ioctlArg));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::EnableHDRSlottedMode(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::EnableHDRSlottedMode() */

ds::ErrorType Network1X::EnableHDRHPTMode
(
  boolean enable
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_ENABLE_HDR_HPT_MODE,
                                 static_cast <void *> (&enable));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::EnableHDRHPTMode(): "
                           "Result 0x%x, obj 0x%p, enable %d", result, this, enable);

  mpICritSect->Leave();
  return result;

} /* Network1X::EnableHDRHPTMode() */

ds::ErrorType Network1X::EnableHoldDown
(
  boolean enable
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_ENABLE_HOLDDOWN,
                                 static_cast <void *> (&enable));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::EnableHoldDown(): "
                           "Result 0x%x, obj 0x%p, enable %d", result, this, enable);

  mpICritSect->Leave();
  return result;

} /* Network1X::EnableHoldDown() */


ds::ErrorType Network1X::QueryDoSSupport
(
  Network1xDoSFlagsType flags,
  boolean *             dosSupported
)
{
  SDBSupportQueryType  ioctlArg;
  ds::ErrorType        result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == dosSupported)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::QueryDoSSupport(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  memset((void *) &ioctlArg, 0, sizeof(SDBSupportQueryType));

  ioctlArg.flags      = flags;
  ioctlArg.can_do_sdb = FALSE;

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SDB_SUPPORT_QUERY,
                                 static_cast <void *> (&ioctlArg));

  *dosSupported = ioctlArg.can_do_sdb;

  LOG_MSG_FUNCTION_EXIT_4 ("Network1X::QueryDoSSupport(): "
                           "Result 0x%x, dossupported %d, obj 0x%p, flags 0x%x",
                           result, *dosSupported, this, flags);

  mpICritSect->Leave();
  return result;

} /* Network1X::QueryDoSSupport() */

ds::ErrorType Network1X::GetMDR
(
  int *  mdr
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_MDR,
                                 static_cast <void *> (mdr));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetMDR(): Result 0x%x, obj 0x%p",
                           result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetMDR() */

ds::ErrorType Network1X::SetMDR
(
  int mdr
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SET_MDR,
                                 static_cast <void *> (&mdr));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::SetMDR(): "
                           "Result 0x%x, obj 0x%p,  MDR %d", result, this, mdr);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetMDR() */


ds::ErrorType Network1X::GetDormancyTimer
(
  int *  dormancyTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_DORM_TIMER,
                                 static_cast <void *> (dormancyTimerVal));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetDormancyTimer(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetDormancyTimer() */

ds::ErrorType Network1X::SetDormancyTimer
(
  int dormancyTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SET_DORM_TIMER,
                                 static_cast <void *> (&dormancyTimerVal));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::SetDormancyTimer(): "
                           "Result 0x%x, obj 0x%p, dorm timer val %d",
                           result, this, dormancyTimerVal);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetDormancyTimer() */

ds::ErrorType Network1X::GetSessionTimer
(
  Network1xPrivSessionTimerType *  sessionTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_SESSION_TIMER,
                                 static_cast <void *> (sessionTimerVal));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetSessionTimer(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetSessionTimer() */


ds::ErrorType Network1X::SetSessionTimer
(
  const Network1xPrivSessionTimerType *  sessionTimerVal
)
{
  ds::ErrorType                  result;
  Network1xPrivSessionTimerType  ioctlArg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  ioctlArg = *sessionTimerVal;

  /*-------------------------------------------------------------------------
    Call the iface IOCTL for SetSessionTimer
    Need the conversion because input is const arg.
    TODO: Take int as arg instead of const int*
  -------------------------------------------------------------------------*/
  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_707_SET_SESSION_TIMER,
                      static_cast <void *> (&ioctlArg));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::GetSessionTimer(): "
                           "Result 0x%x, obj 0x%p, session timer val %d",
                           result, this, sessionTimerVal);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetSessionTimer() */

ds::ErrorType Network1X::GetQoSNAPriority
(
  int *  qosNAPriority
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_RLP_QOS_NA_PRI,
                                 static_cast <void *> (qosNAPriority));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetQoSNAPriority(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetQoSNAPriority() */

ds::ErrorType Network1X::SetQoSNAPriority
(
  int qosNAPriority
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SET_RLP_QOS_NA_PRI,
                                 static_cast <void *> (&qosNAPriority));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::SetQoSNAPriority(): "
                           "Result 0x%x, obj 0x%p, QoS NA pri %d",
                           result, this, qosNAPriority);

  mpICritSect->Leave();

  return result;

} /* Network1X::SetQoSNAPriority() */


ds::ErrorType Network1X::GetRLPAllCurrentNAK
(
  Network1xRLPOptionType *  rlpAllCurrentNAK
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK,
                                 static_cast <void *> (rlpAllCurrentNAK));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetRLPAllCurrentNAK(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetRLPAllCurrentNAK() */


ds::ErrorType Network1X::SetRLPAllCurrentNAK
(
  const Network1xRLPOptionType *  rlpAllCurrentNAK
)
{
  ds::ErrorType           result;
  Network1xRLPOptionType  ioctlArg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == rlpAllCurrentNAK)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::SetRLPAllCurrentNAK(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    The function argument is constant whereas iface ioctl requires a
    non const argument.
  -------------------------------------------------------------------------*/
  ioctlArg = *rlpAllCurrentNAK;

  /*-------------------------------------------------------------------------
    Call the iface IOCTL for SetRLPAllCurrentNAK
    TODO: Take int as arg instead of const int *
  -------------------------------------------------------------------------*/
  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK,
                      static_cast <void *> (&ioctlArg));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::SetRLPAllCurrentNAK(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetRLPAllCurrentNAK() */

ds::ErrorType Network1X::GetRLPDefCurrentNAK
(
  Network1xRLPOptionType *  rlpDefCurrentNAK
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK,
                                 static_cast <void *> (rlpDefCurrentNAK));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetRLPDefCurrentNAK(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetRLPDefCurrentNAK() */


ds::ErrorType Network1X::SetRLPDefCurrentNAK
(
  const Network1xRLPOptionType *  rlpDefCurrentNAK
)
{
  ds::ErrorType           result;
  Network1xRLPOptionType  ioctlArg;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == rlpDefCurrentNAK)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::SetRLPDefCurrentNAK(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  /*-------------------------------------------------------------------------
    The function argument is constant whereas iface ioctl requires a
    non const argument.
  -------------------------------------------------------------------------*/
  ioctlArg = *rlpDefCurrentNAK;

  /*-------------------------------------------------------------------------
    Call the iface IOCTL for SetRLPDefCurrentNAK
    TODO: Take int as arg instead of const int*
  -------------------------------------------------------------------------*/
  result = IfaceIoctl(GetHandle(),
                      IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK,
                      static_cast <void *> (&ioctlArg));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::SetRLPDefCurrentNAK(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetRLPDefCurrentNAK() */


ds::ErrorType Network1X::GetRLPNegCurrentNAK
(
  Network1xRLPOptionType *  rlpNegCurrentNAK
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK,
                                 static_cast <void *> (rlpNegCurrentNAK));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetRLPNegCurrentNAK(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetRLPNegCurrentNAK() */


ds::ErrorType Network1X::QueryInterface
(
  AEEIID   iid,
  void **  ppo
)
throw()
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppo)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::QueryInterface(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (iid)
  {
    case AEEIID_INetwork:
    {
      /*---------------------------------------------------------------------
        Get Strong reference to INetwork parent object and return
      ---------------------------------------------------------------------*/
      if (TRUE == mpParent->GetStrongRef())
      {
        /*-------------------------------------------------------------------
          No need to perform AddRef() again, GetStrongRef() already does that
        -------------------------------------------------------------------*/
        *ppo   = static_cast <INetwork *> (mpParent);
        result = AEE_SUCCESS;
      }
      else
      {
        LOG_MSG_INFO1_2 ("Network1X::QueryInterface(): "
                         "Failed to get strong ref to parent 0x%p, obj 0x%p",
                         mpParent, this);
        result = AEE_ECLASSNOTSUPPORT;
      }

      break;
    }

    case AEEIID_INetwork1x:
    case AEEIID_IQI:
    {
      *ppo   = static_cast <INetwork1x *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    case AEEIID_INetwork1xPriv:
    {
      *ppo   = static_cast <INetwork1xPriv *> (this);
      result = AEE_SUCCESS;

      (void) AddRef();
      break;
    }

    default:
    {
      result = AEE_ECLASSNOTSUPPORT;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4 ("Network1X::QueryInterface(): "
                           "Result 0x%x, ppo 0x%p, obj 0x%p, iid 0x%x",
                           result, ppo, this, iid);
  mpICritSect->Leave();

  return result;
} /* Network1X::QueryInterface() */

ds::ErrorType Network1X::GetHDRRev0RateInertiaResultInfoCode
(
  Network1xPrivHDRRev0RateInertiaFailureCodeType *  failureCode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == failureCode)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetHDRRev0RateInertiaResultInfoCode(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  *failureCode = rev0InertiaFailResult;

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetHDRRev0RateInertiaResultInfoCode(): "
                           "Rev0 rate inertial result 0x%x, obj 0x%p",
                           *failureCode, this);

  mpICritSect->Leave();
  return AEE_SUCCESS;

} /* Network1X::GetHDRRev0RateInertiaResultInfoCode() */


ds::ErrorType Network1X::GetHDR1xHandDownOption
(
  boolean *  hdr1xHandDownOption
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_HDR_1X_HANDDOWN_OPTION,
                                 static_cast <void *> (hdr1xHandDownOption));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::Gethdr1xHandDownOption(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::Gethdr1xHandDownOption() */

ds::ErrorType Network1X::SetHDR1xHandDownOption
(
  boolean  hdr1xHandDownOption
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network1X::Sethdr1xHandDownOption(): "
                            "Obj 0x%p, HDR 1x hand down option %d",
                            this, hdr1xHandDownOption);

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SET_HDR_1X_HANDDOWN_OPTION,
                                 static_cast <void *> (&hdr1xHandDownOption));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::Sethdr1xHandDownOption(): "
                           "Result 0x%x, obj 0x%p, HDR 1x hand down option %d",
                           result, this, hdr1xHandDownOption);

  mpICritSect->Leave();
  return result;

} /* Network1X::Sethdr1xHandDownOption() */


ds::ErrorType Network1X::GetHysteresisTimer
(
  int *  hysteresisTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == hysteresisTimerVal)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetHysteresisTimer(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER,
                                 static_cast <void *> (hysteresisTimerVal));

  LOG_MSG_FUNCTION_EXIT_2 ("Network1X::GetHysteresisTimer(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();
  return result;

} /* Network1X::GetHysteresisTimer() */

ds::ErrorType Network1X::SetHysteresisTimer
(
  int  hysteresisTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  mpICritSect->Enter();

  result = IfaceIoctlNonNullArg (GetHandle(),
                                 IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER,
                                 static_cast <void *> (&hysteresisTimerVal));

  LOG_MSG_FUNCTION_EXIT_3 ("Network1X::SetHysteresisTimer(): "
                           "Result 0x%x, obj 0x%p, hysteresis timer val %d",
                           result, this, hysteresisTimerVal);

  mpICritSect->Leave();
  return result;

} /* Network1X::SetHysteresisTimer() */

int Network1X::GetSIPMIPCallType
(
  Network1xSIPMIPCallType* callType
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("Network1X::GetSIPMIPCallType(): Obj 0x%p, handle 0x%x",
                          this, GetHandle());

  return IfaceIoctlNonNullArg (GetHandle(), IFACE_IOCTL_707_GET_SIP_MIP_DATA_CALL_TYPE,
                               (void *)callType);

}/* Network1X::GetSIPMIPCallType() */

ds::ErrorType Network1X::GetSignalBus
(
  ds::Net::EventType  eventID,
  ISignalBus **       ppISigBus
)
{
  ds::ErrorType  result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == ppISigBus)
  {
    LOG_MSG_INVALID_INPUT_1 ("Network1X::GetSignalBus(): NULL arg, obj 0x%p",
                             this);
    return QDS_EFAULT;
  }

  mpICritSect->Enter();

  switch (eventID)
  {
    case  Network1xPrivEvent::HDR_REV0_RATE_INERTIA_RESULT:
    {
      *ppISigBus = mpSigBusHDRRev0RateInertia;
      break;
    }

    case  Network1xPrivEvent::SLOTTED_MODE_RESULT:
    {
      *ppISigBus = mpSigBusHDRSlottedMode;
      break;
    }

    case  Network1xPrivEvent::SLOTTED_MODE_CHANGED:
    {
      *ppISigBus = mpSigBusHDRChangeSlottedMode;
      break;
    }

    default:
    {
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

  LOG_MSG_FUNCTION_EXIT_4 ("QoSDefault::GetSignalBus(): "
                           "Result 0x%x, signal bus 0x%p, obj 0x%p event 0x%x",
                           result, *ppISigBus, this, eventID);
  mpICritSect->Leave();
  return result;

} /* GetSignalBus() */

boolean Network1X::Process
(
  void *  pUserData
)
{
  EventInfoType *                       pEventInfo = NULL;
  SlottedModeFailureCodeType            slotModeResult;
  Rev0RateInertiaFailureCodeType        inertiaFailReason;
  int32                                 ifaceHandle = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    User data should never be NULL for event processing.
  -------------------------------------------------------------------------*/
  if (0 == pUserData)
  {
    LOG_MSG_ERROR_1 ("Network1X::Process(): NULL ev info, obj 0x%p", this);
    return FALSE;
  }

  pEventInfo = static_cast <EventInfoType *> (pUserData);

  mpICritSect->Enter();

  do
  {
    
    ifaceHandle = GetHandle();

    LOG_MSG_INFO1_4 ("Network1X::Process(): "
                     "Handle 0x%x, event %d, event handle 0x%x, obj 0x%p",
                     ifaceHandle,
                     pEventInfo->eventName,
                     pEventInfo->handle,
                     this);

      /*-----------------------------------------------------------------------
          Process the event only if the event occurred on this handle
      -----------------------------------------------------------------------*/
      if (ifaceHandle != pEventInfo->handle)
      {
        LOG_MSG_INFO3_4 ("Network1X::Process(): "
                         "Handle 0x%x != event handle 0x%x, event %d, obj 0x%p",
                         ifaceHandle,
                         pEventInfo->handle,
                         pEventInfo->psEventName,
                         this);
        break;
      }

    switch (pEventInfo->eventName)
    {
      case Network1xPrivEvent:: SLOTTED_MODE_RESULT:
      {
        slotModeResult =
          static_cast < SlottedModeFailureCodeType > ((uint32)(pEventInfo->psEventInfo));

        if (pEventInfo->psEventName ==
              IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_SUCCESS_EV)
        {
          slotModeOpResult = ds::Net::Network1xPrivSlottedMode::REQUEST_SUCCEES;
        }
        else if (pEventInfo->psEventName ==
                   IFACE_ENABLE_HDR_SET_EIDLE_SLOTTED_MODE_FAILURE_EV)
        {
          switch (slotModeResult)
          {
            case PS_HDR_SLOTTED_MODE_REQUEST_REJECTED:
            {
              slotModeOpResult =
                ds::Net::Network1xPrivSlottedMode::REQUEST_REJECTED;

              break;
            }

            case PS_HDR_SLOTTED_MODE_REQUEST_FAILED_TX:
            {
              slotModeOpResult =
                ds::Net::Network1xPrivSlottedMode::REQUEST_FAILED_TX;

              break;
            }

            case PS_HDR_SLOTTED_MODE_NOT_SUPPORTED:
            {
              slotModeOpResult =
                ds::Net::Network1xPrivSlottedMode::REQUEST_UNSUPPORTED;

              break;
            }

            case PS_HDR_SLOTTED_MODE_NO_NET:
            {
              slotModeOpResult =
                ds::Net::Network1xPrivSlottedMode::REQUEST_NO_NET;

              break;
            }

            case PS_HDR_SLOTTED_MODE_FAILURE_REASON_MAX:
            default:
            {
              slotModeOpResult =
                ds::Net::Network1xPrivSlottedMode::REQUEST_REJECTED;

              break;
            }
          }
        }

        break;
      }

      case Network1xPrivEvent:: HDR_REV0_RATE_INERTIA_RESULT:
      {
        if (pEventInfo->psEventName ==
              IFACE_ENABLE_HDR_REV0_RATE_INERTIA_SUCCESS_EV)
        {
          rev0InertiaOpSuccess = TRUE;
        }
        else if (pEventInfo->psEventName ==
                   IFACE_ENABLE_HDR_REV0_RATE_INERTIA_FAILURE_EV)
        {
          inertiaFailReason = static_cast <Rev0RateInertiaFailureCodeType>
                                ((int)(pEventInfo->psEventInfo));

          switch (inertiaFailReason)
          {
            case PS_HDR_REV0_RATE_INERTIA_REQUEST_REJECTED:
            {
              rev0InertiaFailResult =
                ds::Net::Network1xPrivHDRRev0RateInertiaFailureCode::REQUEST_REJECTED;
              break;
            }

            case PS_HDR_REV0_RATE_INERTIA_REQUEST_FAILED_TX:
            {
              rev0InertiaFailResult =
                ds::Net::Network1xPrivHDRRev0RateInertiaFailureCode::REQUEST_FAILED_TX;
              break;
            }

            case PS_HDR_REV0_RATE_INERTIA_NOT_SUPPORTED:
            {
              rev0InertiaFailResult =
                ds::Net::Network1xPrivHDRRev0RateInertiaFailureCode::NOT_SUPPORTED;
              break;
            }

            case PS_HDR_REV0_RATE_INERTIA_NO_NET:
            {
              rev0InertiaFailResult =
                ds::Net::Network1xPrivHDRRev0RateInertiaFailureCode::NO_NET;

              break;
            }

            case PS_HDR_REV0_RATE_INERTIA_FAILURE_REASON_MAX:
            default:
            {
              rev0InertiaFailResult =
                ds::Net::Network1xPrivHDRRev0RateInertiaFailureCode::REQUEST_REJECTED;
              break;
            }
          }
        }

        break;
      }

      default:
      {
        break;
      }
    }

    Notify (pEventInfo->eventName);
  } while (0);

  mpICritSect->Leave();
  return TRUE;

} /* Network1x::Process() */
