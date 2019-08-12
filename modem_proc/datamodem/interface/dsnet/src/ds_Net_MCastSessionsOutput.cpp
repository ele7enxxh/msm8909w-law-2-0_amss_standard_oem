/*===========================================================================
  FILE: ds_Net_MCastSessionsOutput.cpp

  OVERVIEW: This file provides implementation of the MCastSessionsOutput class.

  DEPENDENCIES: None

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
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
#include "ds_Net_MCastSessionsOutput.h"
#include "ds_Net_MCastSession.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Error;
using namespace ds::Net;

MCastSessionsOutput::MCastSessionsOutput
(
  void
)
: refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* MCastSessionsOutput::MCastSessionsOutput() */

void MCastSessionsOutput::Destructor
(
  void
)
throw()
{
  ds::Utils::INode *pItem = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while (0 != (pItem = mMCastSessionsList.PopFront()))
  {
    (void) pItem->Release();
  }

} /* MCastSessionsOutput::Destructor() */

MCastSessionsOutput::~MCastSessionsOutput()
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    NO-OP: used only for freeing memory
  -------------------------------------------------------------------------*/

} /* MCastSessionsOutput::~MCastSessionsOutput() */


/*---------------------------------------------------------------------------
  Inherited functions from IMCastSessionsOutput.
---------------------------------------------------------------------------*/
ds::ErrorType MCastSessionsOutput::GetNth
(
  int               index,
  IMCastSession **  mcastSession
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == mcastSession)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsOutput::GetNth(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (0 > index)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsOutput::GetNth(): "
                             "Invalid arg, obj 0x%p", this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Get the Nth IMCastSession object from the list
  -------------------------------------------------------------------------*/
  *mcastSession = static_cast <MCastSession *> (mMCastSessionsList.Get(index));
  if (0 == *mcastSession)
  {
    LOG_MSG_ERROR_2 ("MCastSessionsOutput::GetNth(): "
                     "NULL item at index %d, obj 0x%p", index, this);
    return AEE_EFAILED;
  }

  (void) (*mcastSession)->AddRef();

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsOutput::GetNth(): "
                           "Nth session 0x%p, obj 0x%p", *mcastSession, this);

  return AEE_SUCCESS;
} /* MCastSessionsOutput::GetNth() */


ds::ErrorType MCastSessionsOutput::GetnumElements
(
  int *  numElements
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == numElements)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsOutput::GetnumElements(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Get the number of elements from the list
  -------------------------------------------------------------------------*/
  *numElements = mMCastSessionsList.Count();

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsOutput::GetnumElements(): "
                           "Num elements %d, obj 0x%p", *numElements, this);

  return AEE_SUCCESS;
} /* MCastSessionsOutput::GetnumElements() */

ds::ErrorType MCastSessionsOutput::AddMCastSession
(
  IMCastSession *  pSession
)
{
  MCastSession *  pMCastSession;
  ds::ErrorType   result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pSession)
  {
    LOG_MSG_INVALID_INPUT_1 ("MCastSessionsOutput::AddMCastSession(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Push the MCastSession object to the beginning of the list.
    Take a reference to MCastSession object, caller is responsible
    to release it
  -------------------------------------------------------------------------*/
  pMCastSession = static_cast <MCastSession *> (pSession);

  result = mMCastSessionsList.PushFront(pMCastSession);
  if (AEE_SUCCESS == result)
  {
    (void) pMCastSession->AddRef();
  }

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsOutput::AddMCastSession(): "
                           "Result 0x%x, obj 0x%p", result, this);

  return result;
} /* MCastSessionsOutput::AddMCastSession() */
  #endif // FEATUTE_DATA_PS_MCAST
