/*==========================================================================*/
/*!
  @file
  ds_Utils_SignalFactory.cpp

  @brief
  This file provides implementation for ds::Utils::SignalFactory class.

  @see
  ds_Utils_SignalFactory.h

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_SignalFactory.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-11-26 mt  Branched file from SignalFactory.cpp

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"

#include "ds_Utils_SignalFactory.h"
#include "ds_Utils_SignalCtl.h"

using namespace ds::Utils;
using namespace ds::Error;


/*===========================================================================

                       CONSTRUCTOR/DESTRUCTOR

===========================================================================*/
/**
  @brief      This is the constructor of the SignalFactory object.

              This is the constructor of the SignalFactory object.

  @param      None.

  @see        No external dependencies

  @return     None.
*/

SignalFactory::SignalFactory
(
  void
)
: Factory()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_1 ("Creating SignalFactory obj 0x%p", this);

} /* SignalFactory() */


/**
  @brief      This is the destructor of the SignalFactory object.

              This is the destructor of the SignalFactory object.

  @param      None.

  @see        No external dependencies

  @return     None.
*/

SignalFactory::~SignalFactory
(
  void
)
throw()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_1 ("Deleting SignalFactory obj 0x%p", this);

  /* By default the factory class's destructor would be called, which will
     pop each item and delete them */

} /* ~SignalFactory() */

/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/**
  @brief      This function creates a Signal object and adds it to the
              factory.

              This function creates a Signal object and adds it to the
              factory.

  @param      None.

  @see        No external dependencies

  @return     None.
*/
int CDECL SignalFactory::CreateSignal
(
  ISignalHandler * piHandler,
  SignalHandlerCallBack pSignalHandlerCallBack,
  SignalHandlerBase* pSignalHandlerBaseParam,
  ISignal**        ppiSig,
  ISignalCtl**     ppiSigCtl
)
throw()
{
  Signal*         signalPtr = NULL;
  SignalCtl*      signalCtlPtr = NULL;

#if 0
  int32           result;
#endif

/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO1_1 ("SignalFactory::CreateSignal(): "
                   "CreateSignal() called on signal fac 0x%p", this);

  if (NULL == piHandler || NULL == pSignalHandlerCallBack || NULL == ppiSig || NULL == ppiSigCtl)
  {
    LOG_MSG_ERROR_0 ("SignalFactory::CreateSignal(): "
                     "NULL args");
    return QDS_EFAULT;
  }

  signalPtr = Signal::CreateInstance (piHandler, pSignalHandlerCallBack, pSignalHandlerBaseParam);
  if (NULL == signalPtr)
  {
    LOG_MSG_ERROR_0 ("SignalFactory::CreateSignal(): "
                     "Cant create signal");
    goto bail;
  }

  signalCtlPtr = SignalCtl::CreateInstance (signalPtr);
  if (NULL == signalCtlPtr)
  {
    LOG_MSG_ERROR_0 ("SignalFactory::CreateSignal(): "
                     "Cant create signalctl");
    goto bail;
  }

  *ppiSig = static_cast <ISignal *> (signalPtr);
  *ppiSigCtl = static_cast <ISignalCtl *> (signalCtlPtr);
  return AEE_SUCCESS;

bail:
  DS_UTILS_RELEASEIF(signalPtr);
  DS_UTILS_RELEASEIF(signalCtlPtr);
  *ppiSig = NULL;
  *ppiSigCtl = NULL;
  return AEE_ENOMEMORY;

} /* CreateSignal() */


/*-------------------------------------------------------------------------
    CreateInstance method for SignalFactory.
  -------------------------------------------------------------------------*/
static SignalFactory *instance;

int SignalFactory::CreateInstance
(
  void*    env,
  AEECLSID clsid,
  void**   newObj
)

{
  (void) env;

  if (AEECLSID_CSignalFactory == clsid)
  {
    if (0 == instance)
    {
      instance = new SignalFactory();
      if (0 == instance)
      {
        LOG_MSG_ERROR_0 ("SignalFactory::CreateInstance(): "
                         "Cant create SignalFactory singleton");
        return AEE_ENOMEMORY;
      }
    }

    *newObj = (ISignalFactory *)instance;
    return AEE_SUCCESS;
  }
  return AEE_ECLASSNOTSUPPORT;
} /* CreateInstance() */

