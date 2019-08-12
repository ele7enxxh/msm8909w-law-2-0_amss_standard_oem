/*===========================================================================
  FILE: QoSSecondariesInput.cpp

  OVERVIEW: This file provides implementation of the QoSSecondariesInput class.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATUTE_DATA_PS_MCAST
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_MCastSessionsInput.h"
#include "ds_Net_MCastSession.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Error;
using namespace ds::Net;

MCastSessionsInput::MCastSessionsInput
(
  void
)
: refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* MCastSessionsInput::MCastSessionsInput() */

void MCastSessionsInput::Destructor
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    MCastSessionsInput doesn't hold reference to MCastSession itself,
    so there is no need to release it here.
    The memory associated with list node allocated for each added session
    is reclaimed in ~MCastSessionsInput (list is stored "by value")
  -------------------------------------------------------------------------*/

} /* MCastSessionsInput::Destructor() */

MCastSessionsInput::~MCastSessionsInput()
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* MCastSessionsInput::~MCastSessionsInput() */


/*---------------------------------------------------------------------------
  Inherited functions from IMCastSessionsInput.
---------------------------------------------------------------------------*/
ds::ErrorType MCastSessionsInput::Associate
(
  IMCastSession *  mcastSession
)
{
  MCastSession *  pMCastSession;
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == mcastSession)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsInput::Associate(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Push the MCastSession object to the beginning of the list
  -------------------------------------------------------------------------*/
  pMCastSession = static_cast <MCastSession *> (mcastSession);

  result = mMCastSessionsList.PushFront(pMCastSession);

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsInput::Associate(): "
                           "Result 0x%x, obj 0x%p", result, this);

  return result;
} /* MCastSessionsInput::Associate() */


ds::ErrorType MCastSessionsInput::Disassociate
(
  IMCastSession *  mcastSession
)
{
  MCastSession *  pMCastSession;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == mcastSession)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsInput::Disassociate(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Remove the MCastSession object from the list
  -------------------------------------------------------------------------*/
  pMCastSession = static_cast <MCastSession *> (mcastSession);
  mMCastSessionsList.RemoveItem(pMCastSession);

  return AEE_SUCCESS;
} /* MCastSessionsInput::Disassociate() */

ds::ErrorType MCastSessionsInput::GetNth
(
  int               index,
  IMCastSession **  mcastSession
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == mcastSession)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsInput::GetNth(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (0 > index)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsInput::GetNth(): "
                             "Invalid arg, obj 0x%p", this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Get the Nth IMCastSession object from the list
  -------------------------------------------------------------------------*/
  *mcastSession = static_cast <MCastSession *> (mMCastSessionsList.Get(index));
  if (0 == *mcastSession)
  {
    LOG_MSG_ERROR_2 ("MCastSessionsInput::GetNth(): "
                     "NULL item at index %d, obj 0x%p", index, this);
    return AEE_EFAILED;
  }

  (void) (*mcastSession)->AddRef();

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsInput::GetNth(): "
                           "Nth session 0x%p, obj 0x%p", *mcastSession, this);

  return AEE_SUCCESS;
} /* MCastSessionsInput::GetNth() */

ds::ErrorType MCastSessionsInput::GetNumOfSessions
(
  int32 *  numSessions
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == numSessions)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsInput::GetNumOfSessions(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  *numSessions = mMCastSessionsList.Count();

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsOutput::GetNumOfSessions(): "
                           "Num sessions %d, obj 0x%p", *numSessions, this);

  return AEE_SUCCESS;
} /* MCastSessionsInput::GetNumOfSessions() */
#endif // FEATUTE_DATA_PS_MCAST
