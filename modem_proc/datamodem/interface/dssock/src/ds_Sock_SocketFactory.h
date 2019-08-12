#ifndef DS_SOCK_SOCKETFACTORY_H
#define DS_SOCK_SOCKETFACTORY_H
/*===========================================================================
  @file ds_Sock_SocketFactory.h

  This file defines the SocketFactory class that implements the ISocketFactory 
  interface.
  
  In addition, it also implements Factory interface in order to maintain a
  list of ISocket objects. This list is used to relay events 
  from various modules to all ISocket objects.
  
  This class should never be used directly.  Instead it should only be accessed
  using SocketFactory's CreateInstance function

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_SocketFactory.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                              INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Utils_CSSupport.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_INetwork.h"
#include "ds_Sock_Socket.h"
#include "ds_Utils_Factory.h"
#include "ds_Sock_Utils.h"
#include "ps_mem.h"

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

    /*lint -esym(1510, IQI) */
    class SocketFactory : public ds::Utils::Factory /*lint !e578 */
    {
      private:
        static SocketFactory* instance;
        
        SocketFactory() throw();
        virtual ~SocketFactory() throw();

        ds::ErrorType CDECL CreateSocketInternal
        (
          AddrFamilyType             family,
          SocketType                 sockType,
          ProtocolType               protocol,
          ISocket **                 newSockPtrPtr,
          NetworkCtl *               networkCtlPtr
        );

        DSSOCK_OVERLOAD_OPERATORS (PS_MEM_SOCKET_FACTORY_TYPE)

      public:

        static SocketFactory * CreateInstance
        (
          void
        )
        throw();

        /**
        @brief This function creates an instance of ISocket. 

        ISocket creation is supported only via ISocketFactory.
        In this API the Application specifies the network policy 
        to be used for the creation of the socket object.
        If NULL is specified for the policyObj default
        network policy shall be used for the relevant socket operations
        when those are applied on the socket.
        The type of socket created via this API depends on availability of
        a corresponding (i.e. with a corresponding policy)
        Network interface that is brought up before 
        applicable sockets operations are applied on the socket. 
        If such Network interface is not available
        such socket operations shall fail. 
        The application can explicitly create an ACTIVE network object 
        corresponding to the same policy to avoid such failures.

        ds::AddrFamilyType, ds::Sock::Type, and ds::Sock::Protocol define the
        supported options for family, sockType, and protocol respectively.
        If protocol is UNSPEC, TCP is used for STREAM sockets and UDP is
        used for DGRAM sockets.

        Newly created socket is added to the list of ISocket objects.

        This method returns error if
        @li Unsupported values are passed for family, sockType, or protocol
        @li TCP is not used as protocol for STREAM sockets
        @li UDP or ICMP is not used as protocol for DGRAM sockets
        @li newSockPtrPtr is NULL

        @param[in]  family        The family of socket to be created
        @param[in]  sockType      The type of socket to be created
        @param[in]  protocol      The protocol to be used
        @param[in]  netPolicyPtr  The policy to be used in data path
        @param[out] newSockPtrPtr Updated with the created socket

        @retval AEE_SUCCESS             Socket is created successfully
        @retval QDS_EINVAL          Family is invalid or
        newSockPtrPtr is already
        populated
        @retval QDS_ESOCKNOSUPPORT  Type is not supported
        @retval QDS_EPROTONOSUPPORT Protocol is not supported
        @retval QDS_EPROTOTYPE      (sockType, protocol)
        combination is not valid
        @retval QDS_EFAULT          NULL newSockPtrPtr
        @retval AEE_ENOMEMORY          Out of memory

        @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol,
        ds::Net::IPolicy, ds::Error
        */
        virtual ds::ErrorType CDECL CreateSocketByPolicy
        (
          AddrFamilyType      family,
          SocketType          sockType,
          ProtocolType        protocol,
          ds::Net::IPolicy *  netPolicyPtr,
          ISocket **          newSockPtrPtr
        );

        /**
        @brief This function creates an instance of ISocket.

        ISocket creation is supported only via ISocketFactory.
        In this API the Application specifies a Network object as a
        reference for the socket creation.
        It is guaranteed that the produced socket shall be
        associated to the same underlying network interface 
        as the one used for the INetwork object.
        This way it is guaranteed that any configurations 
        applied on the INetwork object shall be applicable 
        for the created socket as long as those configurations do not change.

        ds::AddrFamilyType, ds::Sock::Type, and ds::Sock::Protocol define the
        supported options for family, sockType, and protocol respectively.
        If protocol is UNSPEC, TCP is used for STREAM sockets and UDP is
        used for DGRAM sockets.

        Newly created socket is added to the list of ISocket objects.

        This method returns error if
        @li Unsupported values are passed for family, sockType, or protocol
        @li TCP is not used as protocol for STREAM sockets
        @li UDP or ICMP is not used as protocol for DGRAM sockets
        @li networkPtr is NULL
        @li newSockPtrPtr is NULL

        @param[in]  family        The family of socket to be created
        @param[in]  sockType      The type of socket to be created
        @param[in]  protocol      The protocol to be used
        @param[in]  networkPtr    The network instance to be used in data path
        @param[out] newSockPtrPtr Updated with the created socket

        @retval AEE_SUCCESS             Socket is created successfully
        @retval QDS_EINVAL          Family is invalid or
        newSockPtrPtr is already
        populated
        @retval QDS_ESOCKNOSUPPORT  Type is not supported
        @retval QDS_EPROTONOSUPPORT Protocol is not supported
        @retval QDS_EPROTOTYPE      (sockType, protocol)
        combination is not valid
        @retval QDS_EFAULT          NULL networkPtr or
        newSockPtrPtr
        @retval AEE_ENOMEMORY                  Out of memory

        @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol,
        ds::Net::INetwork, ds::Error
        */
        virtual ds::ErrorType CDECL CreateSocketByNetwork
        (
          AddrFamilyType       family,
          SocketType           sockType,
          ProtocolType         protocol,
          ds::Net::INetwork *  networkPtr,
          ISocket **           newSockPtrPtr
        );

        /**
        @brief Deletes a Socket object from the list of ISocket objects.

        Socket object is deleted from the list of ISocket.
        From this point onwards, events are not relayed 
        to the deleted Socket object.

        @param[in] sockPtr  The socket object to be deleted

        @retval AEE_SUCCESS      Socket is deleted successfully
        @retval QDS_EFAULT   NULL Socket object as input

        @see ds::Error
        */
        ds::ErrorType DeleteSocket
        (
          Socket *  sockPtr
        ) throw();

        /**
        @brief IQI interface Methods
        */
        DSIQI_IMPL_DEFAULTS_NO_QI()

    }; /* class SocketFactory */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_SOCKETFACTORY_H */
