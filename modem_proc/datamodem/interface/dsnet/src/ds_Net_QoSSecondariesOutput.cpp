/*===========================================================================
  FILE: QoSSecondariesOutput.cpp

  OVERVIEW: This file provides implementation of the QoSSecondariesOutput class.

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_QoSSecondariesOutput.h"
#include "ds_Net_QoSSecondary.h"
#include "ds_Utils_CreateInstance.h"

using namespace ds::Error;
using namespace ds::Net;

QoSSecondariesOutput::QoSSecondariesOutput
(
  void
)
: refCnt (1),
  weakRefCnt (1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::QoSSecondariesOutput(): "
                            "Obj 0x%p", this);

} /* QoSSecondariesOutput::QoSSecondariesOutput() */

void QoSSecondariesOutput::Destructor
(
  void
)
throw()
{
  QoSSecondary *   pQoSSecondary = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::Destructor(): "
                            "Obj 0x%p", this);

  /*-------------------------------------------------------------------------
    Release the reference to all the QoSSecondaries objects
  -------------------------------------------------------------------------*/
  for (int i = 0; i < mQoSSecondariesList.Count(); i++)
  {
     pQoSSecondary = static_cast <QoSSecondary *> (mQoSSecondariesList.Get(i));
     DS_UTILS_RELEASEIF(pQoSSecondary);
  }

} /* QoSSecondariesOutput::Destructor() */

QoSSecondariesOutput::~QoSSecondariesOutput
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::~QoSSecondariesOutput(): "
                            "Obj 0x%p", this);

} /* QoSSecondariesOutput::~QoSSecondariesOutput() */


/*---------------------------------------------------------------------------
  Inherited functions from IQoSSecondariesOutput.
---------------------------------------------------------------------------*/
ds::ErrorType QoSSecondariesOutput::GetNth
(
 int                          index,
 ::ds::Net::IQoSSecondary **  qosSecondary
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::GetNth(): Obj 0x%p", this);

  if (0 == qosSecondary)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondariesOutput::GetNth(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  if (0 > index)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondariesOutput::GetNth(): "
                             "Invalid arg, obj 0x%p", this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Get the Nth QoSSecondary object from the list
  -------------------------------------------------------------------------*/
  *qosSecondary = static_cast <QoSSecondary *> (mQoSSecondariesList.Get(index));
  if (0 == *qosSecondary)
  {
    LOG_MSG_ERROR_2 ("QoSSecondariesOutput::GetNth(): "
                     "NULL item at index %d, obj 0x%p", index, this);
    return AEE_EFAILED;
  }

  (void) (*qosSecondary)->AddRef();

  LOG_MSG_FUNCTION_EXIT_2 ("MCastSessionsOutput::GetNth(): "
                           "Nth QoSSecondary 0x%p, obj 0x%p",
                           *qosSecondary, this);

  return AEE_SUCCESS;
} /* QoSSecondariesOutput::GetNth() */


ds::ErrorType QoSSecondariesOutput::GetnumElements
(
 int *  numElements
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::GetnumElements(): "
                            "Obj 0x%p", this);

  if (0 == numElements)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondariesOutput::GetnumElements(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Get the number of elements from the list
  -------------------------------------------------------------------------*/
  *numElements = mQoSSecondariesList.Count();

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondariesOutput::GetnumElements(): "
                           "Num elements %d, obj 0x%p", *numElements, this);
  return AEE_SUCCESS;

} /* QoSSecondariesOutput::GetnumElements() */

ds::ErrorType QoSSecondariesOutput::AddQoSSecondary
(
  IQoSSecondary *  qosSecondary
)
{
  QoSSecondary *  pQoSSecondary;
  ds::ErrorType   result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1 ("QoSSecondariesOutput::AddQoSSecondary(): "
                            "Obj 0x%p", this);

  if (0 == qosSecondary)
  {
    LOG_MSG_INVALID_INPUT_1 ("QoSSecondariesOutput::AddQoSSecondary(): "
                             "NULL arg, obj 0x%p", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Push the QoSSecondary object to the beginning of the list
  -------------------------------------------------------------------------*/
  pQoSSecondary = static_cast <QoSSecondary *> (qosSecondary);
  result = mQoSSecondariesList.PushBack(pQoSSecondary);

  if (AEE_SUCCESS != result)
  {
    /*-----------------------------------------------------------------------
      Add a reference to this object. This is the reference of the
      QoSSecondariesOutput object to the session. This reference is released
      in the QoSSecondariesOutput destructor. The AddRef() in GetNth() is
      the reference that the user holds to the QoSSessions.
    -----------------------------------------------------------------------*/
    (void) pQoSSecondary->AddRef();
  }

  LOG_MSG_FUNCTION_EXIT_2 ("QoSSecondariesOutput::AddQoSSecondary(): "
                           "Result 0x%x, obj 0x%p", result, this);
  return result;
} /* QoSSecondariesOutput::AddQoSSecondary() */
