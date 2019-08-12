#ifndef DS_SOCK_SOCKETFACTORY_CLIENT_H
#define DS_SOCK_SOCKETFACTORY_CLIENT_H
/*===========================================================================
  @file ds_Sock_SocketFactoryClient.h

  This file defines the class that implements the ISocketFactory interface.
  It stores the client privileges and act as a proxy to the SocketFactory
  singleton.
  When a client calls CreateInstance for creating a socket factory, an instance
  of SocketFactoryClient is returned to the client. Client shall use this
  instance to create sockets.

                    Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                    Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_SocketFactoryClient.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_Utils_CSSupport.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_INetwork.h"
#include "ds_Sock_ISocketFactory.h"
#include "ds_Sock_Socket.h"
#include "ds_Utils_Factory.h"

/*===========================================================================
  FORWARD DECLERATION
===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace ds
{
  namespace Sock
  {
    /**
      Forward deceleration for SocketFactory
    */
    class SocketFactory;

    /*lint -esym(1510, IQI) */
    /*lint -esym(1510, ISocketFactory) */
    class SocketFactoryClient : public ISocketFactory /*lint !e578 */
    {
      private:

        SocketFactory  * socketFactoryPtr;

        /**
          @brief Constructor for SocketFactoryClient class.

          Initializes a SocketFactoryClient object.

          @param[in]  socketFactory SocketFactory instance that will be used
                                    internally for creating sockets.

          @retval SocketFactoryClient instance
        */
        SocketFactoryClient
        (
          SocketFactory * socketFactory
        )throw();

        /**
          @brief Destructor for SocketFactoryClient class.

          Resets the SocketFactoryClient object. Method is defined as virtual
          so that destructors of derived classes are called when Socket
          object is deleted.

          @param None

          @retval None
        */
        virtual ~SocketFactoryClient() throw();

        /* Overloaded new operator */
        void* operator new (unsigned int num_bytes) throw();

        /* Overloaded delete operator */
        void operator delete (void* objPtr) throw();

    public:

        /**
          Definitions of IQI Methods
        */
        DSIQI_IMPL_DEFAULTS(ISocketFactory)

        /**
          @brief Returns a SocketFactoryClient object.

          Returns a SocketFactoryClient object.

          @param clsID    ClassID for the requested underlying factory.
                          Supported class IDs are:
                          ds::Sock::AEECLSID_CSocketFactory,
                          ds::Sock::AEECLSID_CSocketFactoryService,
                          ds::Sock::AEECLSID_DSSockSocketFactoryPriv,
                          For any other class a NULL will be returned.

          @retval address  Socket factory is created successfully
          @retval 0        Out of memory or unsupported class ID.
        */
        static void * CreateInstance
        (
          AEECLSID   clsID
        );

        /**
          Definitions of CreatSocket methods that implements ISocketFactory.
          Since the SocketFactoryClient is only a proxy to the SocketFactory please
          refer to ds_Sock_SocketFactory.h for the documentation on these functions.
          The only difference with the functions defined in ds_Sock_SocketFactory.h
          is that on ds_Sock_SocketFactory.h there is an additional parameter that
          specifies the privilege. The SocketFactoryClient specifies the
          privilege of the specific client when forwarding the request for socket
          creation to the SocketFactory.
        */
        virtual ds::ErrorType CDECL CreateSocket
        (
          AddrFamilyType      family,
          SocketType          sockType,
          ProtocolType        protocol,
          ds::Net::IPolicy *  netPolicyPtr,
          ISocket **          newSockPtrPtr
        );

        virtual ds::ErrorType CDECL CreateSocketByNetwork
        (
          AddrFamilyType       family,
          SocketType           sockType,
          ProtocolType         protocol,
          ds::Net::INetwork *  networkPtr,
          ISocket **           newSockPtrPtr
        );

    }; /* class SocketFactoryClient */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_SOCKETFACTORY_CLIENT_H */
