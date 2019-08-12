#ifndef PS_SOCK_PROTOCOLSTACK_IGLOBALEVENTLISTENER_H
#define PS_SOCK_PROTOCOLSTACK_IGLOBALEVENTLISTENER_H
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_ProtocolStack_IGlobalEventListener.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "PS_Sock_ProtocolStack_GlobalEventDefs.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace ProtocolStack
    {
      class IGlobalEventListener
      {
        public:
          virtual IGlobalEventListener * GetIGlobalEventListener
          (
            void
          ) = 0;

          virtual void ProcessEvent
          (
            ProtocolStack::GlobalEventType  globalEvent
          ) = 0;
      };
    } /* namespace ProtocolStack */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PROTOCOLSTACK_IGLOBALEVENTLISTENER_H */
