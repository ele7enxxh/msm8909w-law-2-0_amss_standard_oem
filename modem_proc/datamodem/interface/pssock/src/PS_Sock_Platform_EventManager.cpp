/*===========================================================================
  FILE: PS_Sock_Platform_EventManager.cpp

  OVERVIEW: This file implements EventManager module for socket platform. This
  module registers events with ProtocolStack and translates the callbacks in to
  C++ interfaces.

  DEPENDENCIES: None

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_EventManager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Utils_DebugMsg.h"
#include "PS_Sock_Platform_EventManager.h"
#include "PS_Sock_Platform_SocketFactory.h"
#include "PS_Sock_Platform_Socket.h"
#include "PS_Sock_ProtocolStack_IGlobalEventListener.h"
#include "PS_Sock_ProtocolStack_IEventListener.h"
#include "ps_socket_event_defs.h"
#include "ps_socket_cmd.h"
#include "ps_mem.h"
#include "dss_ps.h"

using namespace ds::Utils;
using namespace PS::Sock;


/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Variable which keeps track of implementation of IGlobalEventListener
  interface by the platform's SocketFactory
---------------------------------------------------------------------------*/
static ProtocolStack::IGlobalEventListener *  globalEventListenerPtr = 0;


/*===========================================================================

                        PRIVATE FUNCTION DEFINITIONS

===========================================================================*/
static void ProtocolStackEventCback
(
  ps_socket_event_enum_type    event,
  ps_socket_event_info_type *  eventInfoPtr
)
{
  Platform::Socket *         platformSockPtr;
  ProtocolStack::IEventListener *  eventListenerPtr;
  ProtocolStack::GlobalEventType   globalEvent;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == eventInfoPtr)
  {
    DATA_ERR_FATAL("ProtocolStackEventCback(): NULL arg");
    return;
  }

  LOG_MSG_INFO1_1("ProtocolStackEventCback(): "
                  "Rcvd ev %d", event);

  if (PS_SOCKET_DSM_ITEM_AVAILABLE_EV == event)
  {
    /*-----------------------------------------------------------------------
      Post DSM_ITEM_AVAILABLE event to socket factory
    -----------------------------------------------------------------------*/
    if (0 != globalEventListenerPtr)
    {
      globalEvent = ProtocolStack::GlobalEvent::DSM_ITEM_AVAILABLE;
      globalEventListenerPtr->ProcessEvent( globalEvent);
    }
  }
  else if (PS_SOCKET_DSM_ITEM_NOT_AVAILABLE_EV == event)
  {
    /*-----------------------------------------------------------------------
      Post DSM_ITEM_NOT_AVAILABLE event to socket factory
    -----------------------------------------------------------------------*/
    if (0 != globalEventListenerPtr)
    {
      globalEvent = ProtocolStack::GlobalEvent::DSM_ITEM_NOT_AVAILABLE;
      globalEventListenerPtr->ProcessEvent( globalEvent);
    }
  }
  else
  {
    platformSockPtr =
      reinterpret_cast <Platform::Socket *> ( eventInfoPtr->scb_ptr);

    if (0 == platformSockPtr)
    {
      DATA_ERR_FATAL("ProtocolStackEventCback(): NULL IEventListener");
      return;
    }

    eventListenerPtr = platformSockPtr->GetIEventListener();
    if (0 == eventListenerPtr)
    {
      LOG_MSG_ERROR_1("ProtocolStackEventCback(): "
                      "NULL evListener, sock 0x%x", platformSockPtr);
      ASSERT( 0);
      return;
    }

    switch (event)
    {
      case PS_SOCKET_TCP_CONNECTED_EV:
      {
        eventListenerPtr->ProcessTCPConnectedEvent
        (
          &( eventInfoPtr->data.tcp_connected_event.conn),
          eventInfoPtr->data.tcp_connected_event.options_mask,
          eventInfoPtr->data.tcp_connected_event.mss
        );

        break;
      }

      case PS_SOCKET_PCB_CLOSED_EV:
      {
        eventListenerPtr->ProcessPCBClosedEvent
        (
          eventInfoPtr->data.pcb_closed_reason
        );

        break;
      }

      case PS_SOCKET_UDP_CONNECTED_EV:
      {
        eventListenerPtr->ProcessUDPConnectedEvent
        (
          &( eventInfoPtr->data.conn)
        );

        break;
      }

      case PS_SOCKET_ICMP_ERROR_EV:
      {
        eventListenerPtr->ProcessICMPErrorEvent
        (
          eventInfoPtr->data.icmp_error
        );

        break;
      }

      default:
      {
        LOG_MSG_ERROR_1("ProtocolStackEventCback(): "
                        "Unknown ev %d", event);
        ASSERT( 0);
        break;
      }
    }
  }

  return;
} /* ProtocolStackEventCback() */


static void PSSockCmdOutOfMemEventCback
(
  void
)
{
  ProtocolStack::GlobalEventType  globalEvent;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Post PS_MEM_BUF_NOT_AVAILABLE event to socket factory
  -------------------------------------------------------------------------*/
  if (0 != globalEventListenerPtr)
  {
    globalEvent = ProtocolStack::GlobalEvent::PS_MEM_BUF_NOT_AVAILABLE;
    globalEventListenerPtr->ProcessEvent( globalEvent);
  }

  return;

} /* PSSockCmdOutOfMemEventCback() */


static void PSMemAvailEventCback
(
  int16  wmVal
)
{
  ProtocolStack::GlobalEventType  globalEvent;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Post PS_MEM_BUF_AVAILABLE event to socket factory
  -------------------------------------------------------------------------*/
  if (0 != globalEventListenerPtr)
  {
    globalEvent = ProtocolStack::GlobalEvent::PS_MEM_BUF_AVAILABLE;
    globalEventListenerPtr->ProcessEvent( globalEvent);
  }

  return;

} /* PSMemAvailEventCback() */


/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
void Platform::EventManager::Init
(
  void
)
{
  Platform::SocketFactory *  platformSockFactoryPtr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get IGlobalEventListener object from platform's socket factory
  -------------------------------------------------------------------------*/
  platformSockFactoryPtr = Platform::SocketFactory::CreateInstance();
  if (0 == platformSockFactoryPtr)
  {
    DATA_ERR_FATAL("Platform::EventManager::Init(): NULL sock factory");
    return;
  }

  globalEventListenerPtr = platformSockFactoryPtr->GetIGlobalEventListener();
  DS_UTILS_RELEASEIF(platformSockFactoryPtr);

  /*-------------------------------------------------------------------------
    Register with PS Stack for events
  -------------------------------------------------------------------------*/
  dssps_reg_event_cback( ProtocolStackEventCback);

  /*-------------------------------------------------------------------------
    Register with ps_socket_cmd module for out of cmd buf event
  -------------------------------------------------------------------------*/
  ps_socket_cmd_reg_out_of_mem_cback( PSSockCmdOutOfMemEventCback);

  /*-------------------------------------------------------------------------
    Register with PS Mem for memory available event
  -------------------------------------------------------------------------*/
  ps_mem_reg_mem_avail_cb( PS_MEM_PS_SOCKET_CMD_TYPE, PSMemAvailEventCback);

  return;
} /* Platform::EventManager::Init() */

