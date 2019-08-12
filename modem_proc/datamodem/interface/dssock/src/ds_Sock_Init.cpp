/*===========================================================================
  FILE: ds_Sock_Init.cpp

  OVERVIEW: This file provides functions that are used to perform DSSock module
  initialization

  DEPENDENCIES: None

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_Init.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Sock_Init.h"
#include "ds_Sock_MemManager.h"
#include "ds_Sock_EventManager.h"
#include "ds_Utils_DebugMsg.h"

#include "ds_Sock_ISocketFactory.h"
#include "ds_Sock_CreateInstance.h"
#include "ds_Sock_CSocketFactory.h"
#include "ds_Utils_CSSupport.h"

using namespace ds::Sock;

static ISocketFactory *ISocketFactoryPtr = 0;
static ISocketFactory *ISocketFactoryPrivPtr = 0;

/*===========================================================================

                           EXTERNAL FUNCTIONS

===========================================================================*/

void DSSockPowerup
(
  void
)
{
  int32  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MemManager::MemPoolInit();

  /*-------------------------------------------------------------------------
    Create singletons and only release during powerdown
    Allocate a SocketFactory object if it is not already allocated.
  -------------------------------------------------------------------------*/

  ret_val = DSSockCreateInstance(0, 
                                 AEECLSID_CSocketFactory, 
                                 (void**)&ISocketFactoryPtr);
  if (AEE_SUCCESS != ret_val)
  {
    LOG_MSG_ERROR_1 ("DSSockPowerup(): CreateInstance SocketFactory "
                     "returned %d ", ret_val);
    ASSERT(0);
    return;
  }

  LOG_MSG_FUNCTION_EXIT_0("DSSockPowerup(): "
                          "Success");

} /* DSSockPowerup() */

void DSSockPowerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UTILS_RELEASEIF(ISocketFactoryPrivPtr);
  DS_UTILS_RELEASEIF(ISocketFactoryPtr);
} /* DSPowerdown() */


void DSSockInit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds::Sock::EventManager::Init();

  LOG_MSG_FUNCTION_EXIT_0("DSSockInit(): "
                          "Success");

} /* DSSockInit() */

void DSSockDeinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  EventManager::Deinit();

} /* DSSockDeinit() */
