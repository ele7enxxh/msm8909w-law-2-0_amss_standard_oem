/*==========================================================================*/
/*!
  @file
  ds_Utils_SignalCtl.cpp

  @brief
  This file provides implementation for ds::Utils::SignalCtl class. 
  To simplify things all logic is in the Signal code, SignalCtl merely
  forwards the calls.

  @see
  ds_Utils_SignalCtl.h
  AEEISignalCtl.h

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are signaled in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_SignalCtl.cpp#1 $
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
#include "ds_Utils_SignalCtl.h"
#include "ds_Utils_StdErr.h"

using namespace ds::Utils;
using namespace ds::Error;


/*===========================================================================

                       CONSTRUCTOR/DESTRUCTOR

===========================================================================*/
SignalCtl* SignalCtl::CreateInstance
(
  Signal *pSignal
)
throw()
{
  return new SignalCtl(pSignal);
} /* CreateInstance() */

SignalCtl::SignalCtl
(
  Signal* argSig
)
throw()
: sigPtr(argSig),
  refCnt(1)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2 ("SignalCtl::CreateInstance(): "
                   "SigCtl obj 0x%p for Sig 0x%p", this, argSig);

  (void) sigPtr->AddRef();

} /* SignalCtl() */


SignalCtl::~SignalCtl
(
  void 
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("SignalCtl::CreateInstance(): "
                   "obj 0x%p", this);

  if (NULL != sigPtr)
  {
    sigPtr->Detach();
    (void) sigPtr->Release();
    sigPtr = NULL;
  }

} /* ~SignalCtl() */


/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
ds::ErrorType CDECL SignalCtl::Detach
(
  void
)
throw()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO3_1 ("SignalCtl::Detach(): "
                   "Detach signal ctl 0x%p", this);

  sigPtr->Detach();
  return AEE_SUCCESS;

} /* Detach() */

ds::ErrorType CDECL SignalCtl::Enable
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("SignalCtl::Enable(): "
                   "Enable signal 0x%p", this);

  sigPtr->Enable();
  return AEE_SUCCESS;

} /* Enable() */

ds::ErrorType CDECL SignalCtl::Set
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("SignalCtl::Set(): "
                   "Set() signalCtl 0x%p", this);

  return sigPtr->SignalCtlSet();

} /* Set() */

