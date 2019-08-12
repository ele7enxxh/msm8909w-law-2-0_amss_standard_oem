#ifndef PS_SOCK_PLATFORM_SOCKETFACTORY_H
#define PS_SOCK_PLATFORM_SOCKETFACTORY_H
/*===========================================================================
  @file PS_Sock_Platform_SocketFactory.h

  This file defines the class that implements the
  PS::Sock::Platform::ISocketFactory interface.

  Copyright (c) 2009 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_SocketFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Sock_Utils.h"
#include "ps_mem.h"

#include "ds_Utils_Factory.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_ICritSect.h"
#include "PS_Sock_Platform_ISocketFactory.h"
#include "PS_Sock_Platform_IEventListener.h"
#include "PS_Sock_ProtocolStack_IGlobalEventListener.h"
#include "PS_Sock_Platform_Socket.h"
#include "ds_Utils_SockAddrInternalTypes.h"

#define EPHEMERAL_PORT_BASE        32000
#define EPHEMERAL_PORT_MAX_OFFSET  4999

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
//TODO Document Class
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      typedef uint32  PlatformDisabledEventBitMaskType;

      namespace PlatformDisabledEventBitMask
      {
        const PlatformDisabledEventBitMaskType PS_MEM = 0x01;
        const PlatformDisabledEventBitMaskType DSM    = 0x02;
      }

      class SocketFactory: public ISocketFactory,
                           public ds::Utils::Factory,
                           public PS::Sock::ProtocolStack::IGlobalEventListener
      {
        private:

          static SocketFactory* instance;

          SocketFactory() throw();
          virtual ~SocketFactory() throw();

          PlatformDisabledEventBitMaskType  platformDisabledEventMask;

          void SetPlatformDisabledEventMask
          (
              PlatformDisabledEventBitMaskType  eventBitMask
          );

          void ResetPlatformDisabledEventMask
          (
            PlatformDisabledEventBitMaskType  eventBitMask
          );

          /**
            @brief Ptr to singleton instance of this class.

            Set to a non-NULL when CreateInstance(...) is called the first time.
            All subsequent calls to CreateInstance(...) get this instance.

            Since this instance is never deleted once it is allocated, it
            could have been statically allocated. But dynamical allocation
            gives the flexibility of allocating SocketFactory from custom
            memory.
          */

          DSSOCK_OVERLOAD_OPERATORS(PS_MEM_PLATFORM_SOCKET_FACTORY_TYPE)

        public:

          /**
            @brief Creates a Socket object.

            Creates a Socket object by calling CreateInstance() method of
            Socket interface and adds it to a list of Socket objects.

            If the platform is disabled (because of lack of resources),
            PLATFORM_DISABLED event is posted on the created socket.

            If the operation fails, dsErrnoPtr is populated with the failure
            reason. Possible values are
              @li QDS_EINVAL          Family is invalid
              @li QDS_ESOCKNOSUPPORT  Type is not supported
              @li QDS_EPROTONOSUPPORT Protocol is not supported
              @li QDS_EPROTOTYPE      (sockType, protocol)
                                                 combination is not supported
              @retval QDS_EMFILE      Out of memory

            @param[in]  family      The family of socket to be created
            @param[in]  sockType    The type of socket to be created
            @param[in]  protocol    The protocol to be used
            @param[out] dsErrnoPtr  Updated with error value if the operation
                                    fails

            @retval object  Operation completed successfully
            @retval -1      Operation failed, and dsErrnoPtr is populated with
                            failure reason

            @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol,
                 ds::Error
          */
          virtual Platform::Socket * CreateSocket
          (
            ds::AddrFamilyType      family,
            ds::Sock::SocketType    sockType,
            ds::Sock::ProtocolType  protocol,
            ICritSect *             critSectPtr,
            ds::ErrorType *         dsErrnoPtr
          );

          /**
            @brief Deletes a Socket object from the list of Socket objects.

            Socket object is deleted from the list of Socket objects maintained
            by the SocketFactory object. From this point onwards, events are
            not relayed to the deleted Socket object.

            @param[in] sockPtr  The socket object to be deleted

            @retval None
          */
          void DeleteSocket
          (
            Socket *  sockPtr
          ) throw();

          bool IsSockAddrInUse
          (
            ds::Sock::ProtocolType             protocol,
            const ds::SockAddrIN6InternalType *            v6LocalAddrPtr,
            bool                               reUseAddr
          );

          uint16 GenerateEphemeralPort
          (
            ds::Sock::ProtocolType  protocol
          );

          virtual PS::Sock::ProtocolStack::IGlobalEventListener *
            GetIGlobalEventListener
          (
            void
          );

          virtual void ProcessEvent
          (
            PS::Sock::ProtocolStack::GlobalEventType  globalEvent
          );

          /**
            @brief IQI interface Methods
          */
          DSIQI_IMPL_DEFAULTS_NO_QI()

          static SocketFactory * CreateInstance
          (
            void
          )
          throw();

      };
    } /* namespace Platfrom */
  } /* namespace Sock */
} /* namespace PS */

inline PS::Sock::ProtocolStack::IGlobalEventListener *
  PS::Sock::Platform::SocketFactory::GetIGlobalEventListener
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return static_cast <PS::Sock::ProtocolStack::IGlobalEventListener *> ( this);
} /* PS::Sock::Platform::SocketFactory::GetGlobalEventListener() */

#endif /* PS_SOCK_PLATFORM_SOCKETFACTORY_H */
