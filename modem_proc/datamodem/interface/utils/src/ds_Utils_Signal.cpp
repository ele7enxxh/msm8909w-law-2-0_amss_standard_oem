/*==========================================================================*/
/*!
  @file
  ds_Utils_Signal.cpp

  @brief
  This file provides implementation for ds::Utils::Signal class.

  @see
  ds_Utils_Signal.h
  AEEISignal.h

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are signaled in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_Signal.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"

#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_Signal.h"
#include "ds_Utils_SignalBus.h"
#include "ds_Utils_CritSect.h"
#include "ds_sig_svc.h"

using namespace ds::Utils;
using namespace ds::Error;

/*!
  @brief
  Signal object state masks.

  Signal can be in one or more of the following states.
  SIG_ENABLED    - Ready to receive notifications
  SIG_SET        - Signal has been set.
  SIG_ATTACHED   - This signal is attached to a SignalCtl.
  SIG_ASSOCIATED - This signal is/was associated with a SignalBus.
*/
#define SIG_ENABLED    1
#define SIG_SET        2
#define SIG_ATTACHED   4
#define SIG_ASSOCIATED 8

extern ICritSect * pDSSigCritSect;

/*===========================================================================

                       CONSTRUCTOR/DESTRUCTOR

===========================================================================*/
Signal::Signal
(
  ISignalHandler *piSignalHandler,
  SignalHandlerCallBack pSignalHandlerCallBack,
  SignalHandlerBase* pSignalHandlerBaseParam
)
throw()
: mpiSignalHandler (piSignalHandler),
  mpSignalHandlerCallBack (pSignalHandlerCallBack),
  mpSignalHandlerBaseParam (pSignalHandlerBaseParam),
  mMagic (DS_UTILS_SIGNAL_MAGIC_NUMBER),
  mpSignalBus (NULL),
  mSigState (SIG_ENABLED | SIG_ATTACHED),
  refCnt (1),
  weakRefCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Signal::Signal(): Obj 0x%p", this);
  (void) mpiSignalHandler->AddRef();                  /*lint !e1550 !e1551 */

} /* Signal() */

void Signal::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Signal::Destructor(): Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    Release the SignalHandler, if the SignalCtl hasn't done it beforehand
  -------------------------------------------------------------------------*/
  Detach();

} /* Signal::Destructor() */

Signal::~Signal
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Signal::Destructor(): Obj 0x%p", this);

} /* ~Signal() */


/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
bool Signal::Check
(
  void
)
throw()
{
  uint8 umask = SIG_ENABLED | SIG_SET | SIG_ATTACHED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((umask & mSigState) == umask)
  {
    /*-------------------------------------------------------------------------
      Increment signal's refcnt and signal handler's to ensure that
      signal or the signal handler are not released while callback is pending
    -------------------------------------------------------------------------*/
    (void) AddRef();
    (void) mpiSignalHandler->AddRef();                  /*lint !e1550 !e1551 */

    mSigState &= ~(SIG_ENABLED | SIG_SET);
    return true;
  }
  else
  {
    return false;
  }
} /* Signal::Check() */

void Signal::Schedule
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Schedule() may be called from
    1. APP context (For ISignal_Set() and ISignalCtl_Set())
       In this case, post a command to perform notification later.
    2. DS_SIG context (For ISignalBus_Set())
       In this case, no task-switch is necessary.
  -------------------------------------------------------------------------*/
  if (ds_sig_is_current_task())
  {
    Callback();
    (void) Release();
  }
  else
  {
    ds_sig_send_cmd (DS_SIG_SIGNAL_DISPATCH_CMD, (void *) this);
  }
} /* Signal::Schedule() */

ds::ErrorType CDECL Signal::Set
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  LOG_MSG_INFO1_2 ("Signal::Set(): signal 0x%p, state %d", this, mSigState);

  if (!IsValid())
  {
    pDSSigCritSect->Leave();

    LOG_MSG_INVALID_INPUT_1 ("Signal::Set(): invalid signal 0x%p", this);
    return QDS_EINVAL;
  }

  mSigState |= SIG_SET;
  if (false == Check())
  {
    pDSSigCritSect->Leave();
    return AEE_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Leave the critical section before calling Schedule() so that callback
    is not called in critical section
  -------------------------------------------------------------------------*/
  pDSSigCritSect->Leave();
  Schedule();

  return AEE_SUCCESS;
} /* Signal::Set() */


ds::ErrorType Signal::SignalCtlSet
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  LOG_MSG_INFO1_2 ("Signal::SignalCtlSet(): signal 0x%p, state %d",
                   this, mSigState);

  mSigState |= SIG_SET | SIG_ENABLED;

  if (false == Check())
  {
    pDSSigCritSect->Leave();
    return AEE_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Leave the critical section before calling Schedule() so that callback
    is not called in critical section
  -------------------------------------------------------------------------*/
  pDSSigCritSect->Leave();
  Schedule();

  return AEE_SUCCESS;
} /* Signal::SignalCtlSet() */

void Signal::Enable
(
  void
)
throw()
{
  SignalBus  * pTempSignalBus = NULL;
  bool         stateChanged;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  LOG_MSG_INFO2_2 ("Signal::Enable(): signal 0x%p, state %d", this, mSigState);

  stateChanged = !(mSigState & SIG_ENABLED) && (mSigState & SIG_ATTACHED);
  mSigState    |= SIG_ENABLED;

  /*-------------------------------------------------------------------------
    Save signal bus in a temporary variable so that signal bus methods can be
    invoked outside of critical section.

    Calling SignalBus methods inside critical section could cause deadlock
    between critical sections used by signal and list modules
  -------------------------------------------------------------------------*/
  if ((NULL != mpSignalBus) && (true == stateChanged))
  {
    if (TRUE == mpSignalBus->GetStrongRef())
    {
      pTempSignalBus = mpSignalBus;
    }
  }

  if (false == Check())
  {
    pDSSigCritSect->Leave();

    /*-----------------------------------------------------------------------
      If signalbus is in set state, then if client calls enable, then
      siganl needs to go in set state and callback needs to be callsed
    -----------------------------------------------------------------------*/
    if (NULL != pTempSignalBus)
    {
      pTempSignalBus->SignalEnabled(this);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Leave the critical section before calling Schedule() so that callback
      is not called in critical section.
      Callback is called since  signal was enabled and signal is in set
      state (look at Check())
    -----------------------------------------------------------------------*/
    pDSSigCritSect->Leave();
    Schedule();
  }

  /*------------------------------------------------------------------------
    Release the strong reference obtained a short while ago
  ------------------------------------------------------------------------*/
  if (NULL != pTempSignalBus)
  {
    pTempSignalBus->Release();
  }

} /* Signal::Enable() */

ds::ErrorType Signal::AssociateSignalBus
(
  SignalBus *pSignalBus
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  LOG_MSG_INFO3_3 ("Signal::AssociateSignalBus(): Sig %p, SB %p",
                   this, mSigState, pSignalBus);

  /*-------------------------------------------------------------------------
    It is invalid to associate a signal bus to a signal if it is already
    detached or if it is already associated to another signal bus
  -------------------------------------------------------------------------*/
  if (0 == (mSigState & SIG_ATTACHED))
  {
    pDSSigCritSect->Leave();
    return QDS_EINVAL;
  }

  if (SIG_ASSOCIATED == (mSigState & SIG_ASSOCIATED))
  {
    pDSSigCritSect->Leave();
    return QDS_EINUSE;
  }

  mpSignalBus = pSignalBus;

  mpSignalBus->AddRefWeak();

  mSigState |= SIG_ASSOCIATED;

  pDSSigCritSect->Leave();
  return AEE_SUCCESS;

} /* Signal::AssociateSignalBus() */

void Signal::DisconnectSignalBus
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  if (NULL != mpSignalBus)
  {
    mpSignalBus->ReleaseWeak();
    mpSignalBus = NULL;
  }

  pDSSigCritSect->Leave();
  // The signal retains the associated state to prevent reuse
} /* Signal::DisconnectSignalBus() */


void Signal::Detach
(
  void
)
throw()
{
  SignalBus  * pTempSignalBus = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pDSSigCritSect->Enter();

  LOG_MSG_INFO2_2 ("Signal::Detach(): signal 0x%p, state %d", this, mSigState);

  /*-------------------------------------------------------------------------
    Return immediately if signal is already detached
  -------------------------------------------------------------------------*/
  if (SIG_ATTACHED != (mSigState & SIG_ATTACHED))
  {
    pDSSigCritSect->Leave();
    return;
  }

  mSigState &= ~SIG_ATTACHED;

  /*-------------------------------------------------------------------------
    Save signal bus in a temporary variable so that signal bus methods can
    be invoked outside of critical section.

    Calling SignalBus methods inside critical section could cause deadlock
    between critical sections used by signal and list modules
  -------------------------------------------------------------------------*/
  if (mpSignalBus != NULL)
  {
    if (TRUE == mpSignalBus->GetStrongRef())
    {
      pTempSignalBus = mpSignalBus;
      DisconnectSignalBus();
    }
  }

  (void) mpiSignalHandler->Release();                 /*lint !e1550 !e1551 */

  pDSSigCritSect->Leave();

  if (NULL != pTempSignalBus)
  {
    (void) pTempSignalBus->Remove(this);

    /*-----------------------------------------------------------------------
      Release the strong reference obtained a short while ago
    -----------------------------------------------------------------------*/
    pTempSignalBus->Release();
  }
} /* Signal::Detach() */

boolean Signal::Process
(
  void* argUserDataPtr
)
throw()
{
  SignalProcessEnumType     cmd;
  boolean                   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == argUserDataPtr)
  {
    LOG_MSG_ERROR_0 ("Signal::Process(): NULL args");
    return FALSE;
  }

  cmd = *((SignalProcessEnumType *) argUserDataPtr);
  LOG_MSG_INFO3_2 ("Signal::Process(): "
                   "Process called for signal ctl 0x%p cmd %d", this, cmd);

  switch (cmd)
  {
    case SIGNAL_PROCESS_ENUM_SET:
      (void) Set();
      result = TRUE;
      break;

    case SIGNAL_PROCESS_ENUM_DISCONNECT_SIGNAL_BUS:
      DisconnectSignalBus();
      result = TRUE;
      break;

    default:
      ASSERT (0);
      result = FALSE;
      break;
  }

  return result;

} /* Signal::Process() */

void Signal::SignalDispatcher
(
  ds_sig_cmd_enum_type    cmd,
  void                   *pUserData
)
{
  ds::Utils::Signal      *pSignal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (DS_SIG_SIGNAL_DISPATCH_CMD != cmd)
  {
    LOG_MSG_ERROR_1 ("Signal::SignalDispatcher(): Inv. cmd %d", cmd);
    ASSERT (0);
    return;
  }

  if (NULL == pUserData)
  {
    DATA_ERR_FATAL("Signal::SignalDispatcher(): NULL user data");
    return;
  }

  pSignal = reinterpret_cast<ds::Utils::Signal *> (pUserData);
  if (!pSignal->IsValid())
  {
    LOG_MSG_INVALID_INPUT_1 ("Signal::SignalDispatcher(): invalid signal 0x%p",
                             pSignal);
    return;
  }

  pSignal->Callback();

  /*-------------------------------------------------------------------------
    Release the ref obtained in Schedule()
  -------------------------------------------------------------------------*/
  (void) pSignal->Release();

} /* Signal::SignalDispatcher() */

/* Note that this function doesn't check the signal's Attached status. When
the signal is detached, Set() and Enable() should stop scheduling callbacks,
but any callbacks scheduled before that have to be completed. That's why
we take SH references in Schedule(). */
void Signal::Callback
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("Signal::Callback(): signal 0x%p", this);

  if (0 == (mSigState & SIG_ATTACHED))
  {
    LOG_MSG_INFO1_1 ("Signal::Callback(): signal 0x%p is already detached",
                     this);
  }
  else
  {
    if (AEE_SUCCESS == mpiSignalHandler->Notify(mpSignalHandlerCallBack, mpSignalHandlerBaseParam)) /*lint !e1550 !e1551 */
    {
       Enable();
    }
  }

  /*-------------------------------------------------------------------------
    Release the ref obtained in Schedule()
  -------------------------------------------------------------------------*/
  (void) mpiSignalHandler->Release();                 /*lint !e1550 !e1551 */

} /* Signal::Callback() */

Signal *Signal::CreateInstance
(
  ISignalHandler *piSignalHandler,
  SignalHandlerCallBack pSignalHandlerCallBack,
  SignalHandlerBase* pSignalHandlerBaseParam
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return new Signal(piSignalHandler, pSignalHandlerCallBack, pSignalHandlerBaseParam);
} /* Signal::CreateInstance() */

bool Signal::IsEnabled
(
  void
)
throw()
{
  uint8 flags;
  bool  isSigEnabled;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    A signal is not considered as enabled if it is already detached, since it
    can't be used anyway
  -------------------------------------------------------------------------*/
  flags = SIG_ATTACHED | SIG_ENABLED;

  pDSSigCritSect->Enter();
  isSigEnabled = (flags == (mSigState & flags));
  pDSSigCritSect->Leave();

  return isSigEnabled;
} /* Signal::IsEnabled() */
