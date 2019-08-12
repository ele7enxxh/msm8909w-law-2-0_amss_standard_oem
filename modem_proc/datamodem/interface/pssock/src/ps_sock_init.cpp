/*==========================================================================*/
/*!
  @file ps_sock_init.c
  @brief
  This file provides functions that are used to perform initializations
  of PSSOCK module.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*==========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/ps_sock_init.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-01-21 bb  created module

===========================================================================*/

/*===========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "comdef.h"

#include "ps_sock_init.h"
#include "PS_Sock_Platform_MemManager.h"
#include "PS_Sock_Platform_EventManager.h"
#include "PS_Sock_Platform_ISocketFactory.h"
#include "dss_config.h"
#include "ps_handle_mgr.h"
#include "PS_Sock_RemoteSocket_Manager.h"

PS::Sock::Platform::ISocketFactory *  platformISockFactoryPtr;

/*===========================================================================

                       EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
extern "C" void ps_sock_powerup
(
  void
)
{  
  ps_handle_mgr_init_client( PS_HANDLE_MGR_CLIENT_SOCKET_PLATFORM,
                             DSS_MAX_SOCKS,
                             DSS_MAX_SOCKS,
                             0);

  PS::Sock::Platform::MemManager::Init();
  
  /*-------------------------------------------------------------------------
    Create singletons and only release during powerdown 
    Allocate a SocketFactory object if it is not already allocated.
  -------------------------------------------------------------------------*/
  platformISockFactoryPtr = PS::Sock::Platform::ISocketFactory::CreateInstance();
  ASSERT(0 != platformISockFactoryPtr);

  return;
} /* ps_sock_powerup() */

extern "C" void ps_sock_powerdown
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_UTILS_RELEASEIF(platformISockFactoryPtr);
  
}

extern "C" void ps_sock_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  PS::Sock::Platform::EventManager::Init();
  
  PS::Sock::RemoteSocket::Manager::Init();
  
  return;
} /* ps_sock_init() */

extern "C" void ps_sock_deinit
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS::Sock::RemoteSocket::Manager::DeInit();
}

