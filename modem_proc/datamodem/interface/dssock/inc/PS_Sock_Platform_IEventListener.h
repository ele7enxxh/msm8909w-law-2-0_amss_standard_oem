#ifndef PS_SOCK_PLATFORM_IEVENTLISTENER_H
#define PS_SOCK_PLATFORM_IEVENTLISTENER_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      S O C K P L A T F O R M L I B . H

GENERAL DESCRIPTION
  This header file defines the sockets platform library for all targets (WM,
  Linux, AMSS etc.). Any target which wants to support ds Sockets API shall
  implement this library

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/PS_Sock_Platform_IEventListener.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "PS_Sock_Platform_EventDefs.h"
#include "ds_Utils_IWeakRef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*lint -esym(1510, IEventListener) */
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      class IEventListener : public ds::Utils::IWeakRef
      {
        public:
          virtual void SetEvent
          (
            EventType  event
          ) throw() = 0;

          virtual void ClearEvent
          (
            EventType  event
          ) throw() = 0;
      };

    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_IEVENTLISTENER_H */
