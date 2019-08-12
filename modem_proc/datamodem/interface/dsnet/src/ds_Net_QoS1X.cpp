/*===========================================================================
  FILE: QoS1X.cpp

  OVERVIEW: This file provides implementation of the QoS1X class.

  DEPENDENCIES: None

  Copyright (c) 2007 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_QoS1X.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-03-25 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_QoS1X.h"
#include "ds_Net_Platform.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Net;
using namespace ds::Error;
using namespace NetPlatform;


/*===========================================================================

                     PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
QoS1X::QoS1X
(
  int32 flowHandle
) 
: mFlowHandle(flowHandle),
  refCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2 ("QoS1X::QoS1X(): Obj 0x%p, flow handle 0x%x",
                            this, flowHandle);
  
  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL, 
                                              AEECLSID_CCritSect,
                                             (void **) &mpICritSect))
  {
    DATA_ERR_FATAL("QoS1X::QoS1X(): Cannot create critical section");
  }
} /* QoS1X::QoS1X() */

QoS1X::~QoS1X
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoS1X::~QoS1X(): Obj 0x%p", this);

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(mpICritSect);
  /*lint -restore */

} /* ~QoS1X() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoS1X.
---------------------------------------------------------------------------*/
ds::ErrorType QoS1X::GetRMAC3Info
(
  RMAC3InfoType* rmac3Info
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoS1X::GetRMAC3Info(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (mFlowHandle,
                              FLOW_IOCTL_HDR_GET_RMAC3_INFO,
                              (void *)rmac3Info);

  LOG_MSG_FUNCTION_EXIT_2 ("QoS1X::GetRMAC3Info(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();

  return result;

} /* QoS1X::GetRMAC3Info() */

ds::ErrorType QoS1X::GetTXStatus
(
  boolean* txStatus
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoS1X::GetTXStatus(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (mFlowHandle,
                              FLOW_IOCTL_707_GET_TX_STATUS,
                              (void *)txStatus);

  LOG_MSG_FUNCTION_EXIT_2 ("QoS1X::GetTXStatus(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();

  return result;

} /* QoS1X::GetTXStatus() */


ds::ErrorType QoS1X::GetInactivityTimer
(
  int *  inactivityTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoS1X::GetInactivityTimer(): Obj 0x%p", this);

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (mFlowHandle,
                              FLOW_IOCTL_707_GET_INACTIVITY_TIMER,
                                (void *) inactivityTimerVal);

  LOG_MSG_FUNCTION_EXIT_2 ("QoS1X::GetInactivityTimer(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();

  return result;

} /* QoS1X::GetInactivityTimer() */


ds::ErrorType QoS1X::SetInactivityTimer
(
  int  inactivityTimerVal
)
{
  ds::ErrorType  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2  ("QoS1X::SetInactivityTimer(): "
                             "Obj 0x%p, inactivity timer val %d",
                             this, inactivityTimerVal);

  mpICritSect->Enter();

  result = FlowIoctlNonNullArg (mFlowHandle,
                              FLOW_IOCTL_707_SET_INACTIVITY_TIMER,
                                (void *) &inactivityTimerVal);

  LOG_MSG_FUNCTION_EXIT_2 ("QoS1X::SetInactivityTimer(): "
                           "Result 0x%x, obj 0x%p", result, this);

  mpICritSect->Leave();

  return result;

} /* QoS1X:;SetInactivityTimer() */
