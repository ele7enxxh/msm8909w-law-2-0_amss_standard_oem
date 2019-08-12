#ifndef DS_SOCK_ISOCKETFACTORY_H
#define DS_SOCK_ISOCKETFACTORY_H

/*============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Sock_Def.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_IPort1.h"
#include "ds_Sock_ISocket.h"
#include "ds_Net_IPolicy.h"
#include "ds_Net_DownReasons_Def.h"
#include "ds_Net_IEventManager.h"
#include "ds_Net_IPhysLink.h"
#include "ds_Net_INetwork.h"

namespace ds
{
   namespace Sock
   {
      const ::AEEIID AEEIID_ISocketFactory = 0x106d84a;
      struct ISocketFactory : public ::IQI
      {
         
         /**
           * This function creates an instance of ISocket. ISocket creation is
           * supported only via ISocketFactory.
           * @param family Address Family of the socket (ds::AF_*). Only
           *               QDS_AF_INET and QDS_AF_INET6 are supported.    
           * @param sockType Socket Type (ds::Sock::SOCKTYPE_*).
           * @param protocol Protocol Type (ds::Sock::PROTOCOL_*).
           * @param policy Network policy. If NULL, default network policy shall
           *               be used for the relevant socket operations when those
           *               are applied on the socket.
           * @param newSocket Output The newly created ISocket instance.
           * @retval AEE_SUCCESS ISocket created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateSocket(::ds::AddrFamilyType family, ::ds::Sock::SocketType sockType, ::ds::Sock::ProtocolType protocol, ::ds::Net::IPolicy* policy, ::ds::Sock::ISocket** newSocket) = 0;
         
         /**
           * This function creates an instance of ISocket. ISocket creation is
           * supported only via ISocketFactory. In this API the Application
           * specifies an ACTIVE mode Network object as a reference for the
           * socket creation. It is guaranteed that the produced socket shall be
           * associated to the same underlying network interface as the one used
           * for the INetwork object. This way it is guaranteed that any
           * configurations applied on the INetwork object shall be applicable
           * for the created socket as long as those configurations do not
           * change.
           * If the underlying network interface associated with the provided
           * network object goes DOWN (either due to release of the network
           * object by the application or because of another reason) socket
           * operations applied on the socket shall fail.
           * @param family Address Family of the socket (ds::AF_*). Only
           *               QDS_AF_INET and QDS_AF_INET6 are supported. Must be
           *               compatible to the Network object.   
           * @param sockType Socket Type (ds::Sock::SOCKTYPE_*).
           * @param protocol Protocol Type (ds::Sock::_PROTOCOL_*).
           * @param network Network object to be used as a reference for the
           *                creation of the socket.
           * @param newSocket Output The newly created ISocket instance.
           * @retval AEE_SUCCESS ISocket created successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL CreateSocketByNetwork(::ds::AddrFamilyType family, ::ds::Sock::SocketType sockType, ::ds::Sock::ProtocolType protocol, ::ds::Net::INetwork* network, ::ds::Sock::ISocket** newSocket) = 0;
      };
   };
};
#endif //DS_SOCK_ISOCKETFACTORY_H
