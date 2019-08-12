#ifndef PS_SOCK_PLATFORM_EVENTMANAGER_H
#define PS_SOCK_PLATFORM_EVENTMANAGER_H
/*===========================================================================
  @file PS_Sock_Platform_EventManager.h

  This file declares a method that initializes event manager module to handle
  socket platform events.

  Since ProtocolStack, ps_mem, and DSM modules are implemented in C, the events
  are posted via callbacks. But socket platform is implemented in C++ and
  callbacks make C++ code look non-OOO. So this module acts as a bridge.
  It registers callbacks with ProtocolStack, ps_mem, and DSM modules and
  translates the callbacks in to interface methods resulting in C++ code to
  not deal with callbacks but with cleaner interfaces.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_EventManager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      /**
        @brief Defines event manager module for socket platform.

        Defines a module to listen to events from ProtocolStack, ps_mem, and
        DSM modules.

        @see PS::PS::Sock::ProtocolStack::IEventListener,
             PS::Sock::ProtocolStack::IGlobalEventListener
      */
      namespace EventManager
      {
        /**
          @brief Initializes the event manager module.

          Initializes the event manager module for socket platform. It
          registers for events with ProtocolStack, ps_mem, and DSM modules.

          @param None

          @retval None
        */

        void Init
        (
          void
        );

      } /* namespace EventManager */
    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_EVENTMANAGER_H */
