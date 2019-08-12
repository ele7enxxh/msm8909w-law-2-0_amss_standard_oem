#ifndef PS_SOCK_PROTOCOLSTACK_IEVENTLISTENER_H
#define PS_SOCK_PROTOCOLSTACK_IEVENTLISTENER_H
//TODO Change this
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      S O C K P L A T F O R M L I B . H

GENERAL DESCRIPTION
  This header file defines the sockets platform library for all targets (WM,
  Linux, AMSS etc.). Any target which wants to support ds Sockets API shall
  implement this library

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_ProtocolStack_IEventListener.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ps_socki_defs.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace ProtocolStack
    {
      class IEventListener
      {
        public:
          virtual IEventListener * GetIEventListener
          (
            void
          ) = 0;

          /**
            @brief Processes TCP_CONNECTED event.

            Invoked when ProtocolStack posts TCP_CONNECTED event on a
            platform TCP socket. The socket moves to OPEN state and stores
            local IP address/port, peer IP address/port, MSS, and negotiated
            socket options in its cache. If it is a socket in partial Q of a
            listening socket, it is moved to accepted list
          */
          virtual void ProcessTCPConnectedEvent
          (
            const struct connection *  connPtr,
            int32                      optionsMask,
            uint32                     mss
          ) = 0;

          virtual void ProcessPCBClosedEvent
          (
            uint8  pcbClosedReason
          ) = 0;

          virtual void ProcessUDPConnectedEvent
          (
            const struct connection *  connPtr
          ) = 0;

          virtual void ProcessICMPErrorEvent
          (
            int16  soError
          ) = 0;

          virtual void ProcessPlatformEnabledEvent
          (
            void
          ) = 0;

          virtual void ProcessPlatformDisabledEvent
          (
            void
          ) = 0;
      };
    } /* namespace ProtocolStack */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PROTOCOLSTACK_IEVENTLISTENER_H */
