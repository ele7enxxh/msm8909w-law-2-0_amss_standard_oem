/*===========================================================================
  FILE: ds_Sock_Socket.cpp

  OVERVIEW: This file provides implementation of the Socket class.

  DEPENDENCIES: None

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_Socket.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2012-07-27 ash Added support for IPv6 Hybrid Architecture.
  2012-06-01 pgm Skipping IFACE_IPFLTR_UPDATED_EV processing for IPSEC_OUTPUT
                 fltrs and others to encapsulate the first TCP SYN pkt.
  2012-03-12 pgm Checking if Route scope has changed before posting socket
                 cmd to PS for updating it.
  2010-01-10 ts  Add implementation for RecvMsg and SendMsg public QCMAPIs
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "ds_Sock_Socket.h"
#include "ds_Sock_SocketDef.h"
#include "ds_Sock_SocketFactory.h"
#include "ds_Sock_RoutingManager.h"
#include "ds_Sock_SocketIPSec.h"
#include "ds_Sock_IRecvIFInfoPriv.h"
#include "ds_Sock_IICMPErrInfoPriv.h"
#include "PS_Sock_Platform_ISocketFactory.h"
#include "PS_Sock_Platform_ISocket.h"
#include "ds_Net_Platform.h"
#include "ds_Sock_AddrUtils.h"
#include "ds_Sock_MemManager.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Sock_EventDefs.h"
#include "ds_Net_INetworkPriv.h"
#include "ds_Utils_ISignalFactory.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_Atomic.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_Conversion.h"
#include "ps_in.h"
#include "ps_route_scope.h"
#include "ps_iface_defs.h"
#include "ps_metai_info.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_tcp_config.h"
#include "ds_Sock_NetworkCtl.h"
#include "ps_wmk_logging.h"
#include "ds_sys.h"

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    namespace OptName
    {
      const OptNameType NAME_MIN = 0;
      const OptNameType NAME_MAX = 25;
    }

    namespace OptNamePriv
    {
      const OptNameTypePriv NAME_MIN = 0;
      const OptNameTypePriv NAME_MAX = 4;
    }

    const int32  OPT_MAX_VAL = 0x7FFFFFFF;

    typedef struct
    {
      OptLevelType    optLevel;
      int32           defaultVal;
      int32           minVal;
      int32           maxVal;
    } OptInfoType;

    const OptInfoType sockOptInfoArr[ ] =
    {
         /* DSS_SO_KEEPALIVE */
      { OptLevel::QDS_LEVEL_SOCKET, SO_KEEPALIVE_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_SO_REUSEADDR */
      { OptLevel::QDS_LEVEL_SOCKET, SO_REUSEADDR_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_SO_ERROR_ENABLE */
      { OptLevel::QDS_LEVEL_SOCKET, SO_ERROR_ENABLE_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_SO_TX_IFACE */
      { OptLevel::QDS_LEVEL_SOCKET, 0, 0, OPT_MAX_VAL },

         /* DSS_SO_RCVBUF */
      { OptLevel::QDS_LEVEL_SOCKET, SO_RCVBUF_DEF_VAL, SO_RCVBUF_MIN_VAL,
          SO_RCVBUF_MAX_VAL },

         /* DSS_SO_SNDBUF */
      { OptLevel::QDS_LEVEL_SOCKET, SO_SNDBUF_DEF_VAL, SO_SNDBUF_MIN_VAL,
          SO_SNDBUF_MAX_VAL },

         /* DSS_SO_ERROR */
      { OptLevel::QDS_LEVEL_SOCKET, 0, 0, 0 },

         /* DSS_TCP_NODELAY */
      { OptLevel::QDS_LEVEL_TCP, TCP_NODELAY_DEF_VAL, 0,  OPT_MAX_VAL },

         /* DSS_TCP_DELAYED_ACK */
      { OptLevel::QDS_LEVEL_TCP, TCP_DELAYED_ACK_DEF_VAL, 0,  OPT_MAX_VAL },

         /* DSS_TCP_SACK */
      { OptLevel::QDS_LEVEL_TCP, TCP_SACK_DEF_VAL, 0,  OPT_MAX_VAL },

         /* DSS_TCP_TIMESTAMP */
      { OptLevel::QDS_LEVEL_TCP, TCP_TIMESTAMP_DEF_VAL, 0,  OPT_MAX_VAL },

         /* DSS_TCP_FIONREAD */
      { OptLevel::QDS_LEVEL_TCP, 0, 0, SO_RCVBUF_MAX_VAL },

         /* DSS_TCP_MAXSEG */
      { OptLevel::QDS_LEVEL_TCP, TCP_MAXSEG_DEF_VAL, TCP_MAXSEG_MIN_VAL,
        TCP_MAXSEG_MAX_VAL },

         /* DSS_IP_RECVIF */
      { OptLevel::QDS_LEVEL_IP, IP_RECVIF_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_IP_RECVERR */
      { OptLevel::QDS_LEVEL_IP, IP_RECVERR_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_IP_TOS */
      { OptLevel::QDS_LEVEL_IP, IP_TOS_DEF_VAL, 0, 255 },

         /* DSS_IP_TTL */
      { OptLevel::QDS_LEVEL_IP, IP_TTL_DEF_VAL, 1, 255 },

         /* DSS_IP_MULTICAST_TTL */
      { OptLevel::QDS_LEVEL_IP, IP_MCAST_TTL_DEF_VAL, 1, 255 },

         /* DSS_IPV6_RECVERR */
      { OptLevel::QDS_LEVEL_IPV6, IPV6_RECVERR_DEF_VAL, 0, OPT_MAX_VAL },

         /* DSS_IPV6_TCLASS */
      { OptLevel::QDS_LEVEL_IPV6, IPV6_TCLASS_DEF_VAL, 0, 255 },

         /* DSS_TCP_MAX_BACKOFF_TIME */
      { OptLevel::QDS_LEVEL_TCP, TCP_MAX_TOTAL_BACKOFF_TIME, 0, TCP_MAX_TOTAL_BACKOFF_TIME},

         /* QDS_IP_MULTICAST_LOOP */
      { OptLevel::QDS_LEVEL_IP, IP_MCAST_LOOP_DEF_VAL, 0, OPT_MAX_VAL },

         /* QDS_SO_FIONREAD */
      { OptLevel::QDS_LEVEL_SOCKET, 0, 0, SO_RCVBUF_MAX_VAL },

      /* QDS_SO_SKIP_ROUTE_SCOPE_CHECK */
      { OptLevel::QDS_LEVEL_SOCKET, SO_SKIP_ROUTE_SCOPE_DEF_VAL, 0, OPT_MAX_VAL },

       /* DSS_UDP_ENCAPS */
      { OptLevel::QDS_LEVEL_UDP, UDP_ENCAPS_DEF_VAL, 0, OPT_MAX_VAL}
       
    };

    const OptInfoType sockOptPrivInfoArr[ ] =
    {
         /* DSS_ICMP_ECHO_ID */
      { OptLevel::QDS_LEVEL_ICMP, 0, 0, OPT_MAX_VAL },

         /* DSS_ICMP_ECHO_SEQ_NUM */
      { OptLevel::QDS_LEVEL_ICMP, 0, 0, OPT_MAX_VAL },

         /* DSS_ICMP_TYPE */
      { OptLevel::QDS_LEVEL_ICMP, 0, 0, 255 },

         /* DSS_ICMP_CODE */
      { OptLevel::QDS_LEVEL_ICMP, 0, 0, 255 },
    };

  } /* namespace Sock */
} /* namespace ds */

using namespace PS::Sock;
using namespace ds::Sock;
using namespace ds::Net;
using namespace ds::Error;
using namespace ds::Utils;


/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
ds::ErrorType CDECL Socket::SetNetPolicy
(
  IPolicy *  newPolicyPtr
)
{
  ps_route_scope_type    routeScope;
  ds::ErrorType          dsErrno;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::SetNetPolicy(): "
                           "Sock 0x%x, newPolicyPtr 0x%x, platformSockPtr 0x%x ",
                           this, newPolicyPtr, platformSockPtr);

  if (0 == newPolicyPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Check if it is allowed to update policy
  -------------------------------------------------------------------------*/
  if (false == IsSetNetPolicySupported())
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SetNetPolicy(): "
                            "Not supported, sock 0x%x", this);
    dsErrno = QDS_EOPNOTSUPP;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Generate route scope and set it in platform
  -------------------------------------------------------------------------*/
  dsErrno = NetPlatform::GetRouteScopeByPolicy( newPolicyPtr, &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "GetRouteScopeByPolicy failed, sock 0x%x err 0x%x",
                     this, dsErrno);
    goto bail;
  }

  dsErrno = platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "SetRouteScope failed, sock 0x%x err 0x%x",
                     this, dsErrno);
    goto bail;
  }

  dsErrno = networkCtlPtr->SetNetPolicy(newPolicyPtr);
  if(AEE_SUCCESS != dsErrno)
  {
    goto bail;
  }

  LOG_MSG_FUNCTION_EXIT_1("Socket::SetNetPolicy(): "
                          "Success, sock 0x%x", this);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::SetNetPolicy() */


ds::ErrorType Socket::SetNetwork
(
  INetwork *  dsNetObjPtr
)
{
  ds::ErrorType   dsErrno = AEE_SUCCESS;
  ds::Net::IfaceIdType ifaceId = INVALID_IFACE_ID;
  ps_route_scope_type routeScope;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("Socket::SetNetwork(): Sock 0x%x", this);

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Register with network controller object for state change events.
    This event is used to take appropriate action as network state changes.
    For example, connected TCP sockets are closed once
    network object goes DOWN
  -------------------------------------------------------------------------*/
  dsErrno = RegNetworkStateChangedEvent();
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetNetwork(): "
                    "RegNetworkStateChangedEvent failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  dsErrno = networkCtlPtr->SetNetwork(dsNetObjPtr);
  if(AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetNetwork(): "
                    "networkCtlPtr->SetNetwork failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Generate route scope and set it in platform
  -----------------------------------------------------------------------*/
  dsErrno = networkCtlPtr->GetIfaceId( &ifaceId);
  if (AEE_SUCCESS != dsErrno || ds::Net::INVALID_IFACE_ID == ifaceId)
  {
    LOG_MSG_INFO3_3("Socket::SetNetwork(): "
                    "Sock 0x%x Failed to get ifaceId %d from netCtl obj 0x%x",
                    this, ifaceId, networkCtlPtr);
    goto bail;
  }

  dsErrno = NetPlatform::GetRouteScopeByIfaceId( ifaceId,
                                                 ( ds::AddrFamily::QDS_AF_INET6 == family),
                                                 &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetNetwork(): "
                    "GetRouteScopeByIfaceId failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  dsErrno = platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetNetwork(): "
                    "SetRouteScope failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

/* fall through */

bail:

  critSectPtr->Leave();

  return dsErrno;
} /* Socket::SetNetwork() */


ds::ErrorType CDECL Socket::Bind
(
   const ds::SockAddrStorageType   localAddrPtr
)
{
   ds::SockAddrInternalType         tempLocalAddr={{0}};
   ds::ErrorType                    dsErrno;
   ds::AddrFamilyType               paramFamily = ds::AddrFamily::QDS_AF_UNSPEC;
   boolean                          isSystemSocket;
   boolean                          includeExtAddr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::Bind(): Sock 0x%x, "
                           "localAddrPtr 0x%x, platformSockPtr 0x%x",
                           this, localAddrPtr, platformSockPtr);

  if (0 == localAddrPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  memscpy((void *) &tempLocalAddr,
          sizeof(ds::SockAddrInternalType),
          (void *) localAddrPtr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(&tempLocalAddr,&paramFamily);
  paramFamily = ps_ntohs(paramFamily);
  (void)ds::Sock::AddrUtils::SetFamily(&tempLocalAddr,paramFamily);
  /*-------------------------------------------------------------------------
    Validate localAddrPtr
  -------------------------------------------------------------------------*/

  switch (paramFamily)
  {
    case ds::AddrFamily::QDS_AF_INET6:
    {
      /*---------------------------------------------------------------------
        Make sure that socket is V6, as a V6 address can't be bound to a
        V4 socket
      ---------------------------------------------------------------------*/
      if (ds::AddrFamily::QDS_AF_INET == family)
      {
        LOG_MSG_INVALID_INPUT_1("Socket::Bind(): "
                                "Can't bind V6 addr to a V4 sock 0x%x", this);
        dsErrno = QDS_EINVAL;
        goto bail;
      }

      /*---------------------------------------------------------------------
        If the application is trying to bind a socket to a multicast
        address, make sure that socket supports multicast (Only UDP supports
        multicast). Since it could be a V4 mapped V6 address, make sure that
        the address is neither V6 multicast nor V4 mapped V6 multicast.

        Applications are not allowed to bind to a specific IPv4 unicast
        address as the IP address of the interface in routingCache can
        change at any time. If they are allowed, socket must be closed if
        interface's IP address changes. So make sure that the application
        didn't pass V4 mapped V6 unicast address.

        Since applications can use private IPv6 addresses, they are allowed
        to bind to a specific IPv6 unicast address
      ---------------------------------------------------------------------*/

      if (PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST( tempLocalAddr.v6.addr) ||
          PS_IN6_IS_ADDR_MULTICAST( tempLocalAddr.v6.addr))
      {
        if (false == IsMulticastSupported())
        {
          LOG_MSG_INVALID_INPUT_1("Socket::Bind(): "
                                  "Sock 0x%x doesn't support multicast", this);
          dsErrno = QDS_EINVAL;
          goto bail;
        }
      }
      else if (PS_IN6_IS_ADDR_V4MAPPED( tempLocalAddr.v6.addr))
      {
        LOG_MSG_INVALID_INPUT_1("Socket::Bind(): "
                                "V4 unicast must be INADDR_ANY, sock 0x%x",
                                this);
        dsErrno = QDS_EOPNOTSUPP;
        goto bail;
      }
      else if (false == PS_IN6_IS_ADDR_UNSPECIFIED( tempLocalAddr.v6.addr))
      {
        /*-------------------------------------------------------------------
          Iface address management layer needs to keep track of how many
          clients are using each IPv6 address so that it can manage the
          lifetime of addresses. So increment ref cnt since client is binding
          to a IPv6 address.
        -------------------------------------------------------------------*/
        /*lint -e{545} */

        /*-------------------------------------------------------------------
          Check if the socket is system socket or not. For system socket
          binding to external addresses is allowed.
        -------------------------------------------------------------------*/
        if (QDS_EFAULT == GetSystemOption(&isSystemSocket))
        {
          LOG_MSG_ERROR_1("Socket::Bind(): "
                          "Could not obtain system socket information, Sock 0x%x.",
                          this);
          dsErrno = QDS_EOPNOTSUPP;
          goto bail;
        }

        /*-------------------------------------------------------------------
          include external address while incrementing ref count for system
          sockets.
        -------------------------------------------------------------------*/
        includeExtAddr = isSystemSocket;

        if (DSS_ERROR ==
              NetPlatform::IPv6PrivAddrIncRefCntEx( &(tempLocalAddr.v6.addr),
                                                    includeExtAddr,
                                                    &dsErrno))
        {
          LOG_MSG_INFO3_2("Socket::Bind(): "
                          "IPv6PrivAddrIncRefCnt failed, sock 0x%x err 0x%x",
                          this, dsErrno);
          goto bail;
        }
      }

      break;
    }

    case ds::AddrFamily::QDS_AF_INET:
    {
      /*---------------------------------------------------------------------
        If the application is trying to bind a socket to a multicast
        address, make sure that socket supports multicast (Only UDP supports
        multicast).

        Applications are not allowed to bind to a specific IPv4 unicast
        address as the IP address of the interface in routingCache can
        change at any time. If they are allowed to bind, socket must be closed
        when interface's IP address changes.
      ---------------------------------------------------------------------*/

      if (PS_IN_IS_ADDR_MULTICAST( tempLocalAddr.v4.addr))
      {
        if (false == IsMulticastSupported())
        {
          LOG_MSG_INVALID_INPUT_1("Socket::Bind(): "
                                  "Sock 0x%x doesn't support multicast", this);
          dsErrno = QDS_EINVAL;
          goto bail;
        }
      }
      else if (PS_INADDR_ANY != tempLocalAddr.v4.addr)
      {
        LOG_MSG_INVALID_INPUT_1("Socket::Bind(): "
                                "V4 unicast must be INADDR_ANY, sock 0x%x",
                                this);
        dsErrno = QDS_EOPNOTSUPP;
        goto bail;
      }

      break;
    }

    default:
    {
      LOG_MSG_INVALID_INPUT_2("Socket::Bind(): "
                              "Unsupported family %d, sock 0x%x",
                              paramFamily, this);
      dsErrno = QDS_EAFNOSUPPORT;
      goto bail;
    }
  } /* switch (localAddr's family) */

  /*-------------------------------------------------------------------------
    Call platform specific Bind()
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Bind( &tempLocalAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Bind(): "
                    "Bind failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::Bind() */

ds::ErrorType CDECL Socket::Connect
(
  const ds::SockAddrStorageType   newRemoteAddrPtr
)
{
  void *               routingCachePtr;
  SockAddrIN6InternalType newV6RemoteAddr={0};
  SockAddrInternalType tempAddr = {{0}};
  ps_route_scope_type  routeScope;
  ds::ErrorType        dsErrno;
  ds::ErrorType        tmpDSErrno;
  int32                routingCache;
  ds::AddrFamilyType  _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32               writeEventIndex = 0;
  boolean              isReorig = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::Connect(): "
                           "Sock 0x%x, newRemoteAddrPtr 0x%x, platformSockPtr 0x%x ",
                           this, newRemoteAddrPtr, platformSockPtr);

  if (0 == newRemoteAddrPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  memscpy((void *) &tempAddr,
          sizeof(ds::SockAddrInternalType),
          (void *) newRemoteAddrPtr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(&tempAddr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(&tempAddr,ps_ntohs(_family));

  dsErrno = AddrUtils::GetSockAddrIN6( &tempAddr, &newV6RemoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Connect(): "
                    "GetSockAddrIN6 failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }


  /*-------------------------------------------------------------------------
    Check if connect is supported on this socket as
      1. TCP doesn't support disconnecting a socket
      2. ICMP doesn't support Connect() at all

    Note that since this a protocol specific behaviour, and not platform
    specific, this check is done in sockets library
  -------------------------------------------------------------------------*/
  if (false == IsConnectSupported(&newV6RemoteAddr,&dsErrno))
  {
    LOG_MSG_ERROR_2("Socket::Connect(): "
                    "Connect not supported, err 0x%x sock 0x%x",
                    dsErrno, this);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If remote addr is not ps_in6addr_any, socket is trying to connect to a peer
  -------------------------------------------------------------------------*/
  if (false == PS_IN6_ARE_ADDR_EQUAL( newV6RemoteAddr.addr, &ps_in6addr_any))
  {
    /*-----------------------------------------------------------------------
      0 is not a valid port and hence a socket can't communicate with it
    -----------------------------------------------------------------------*/
    if (0 == newV6RemoteAddr.port)
    {
      LOG_MSG_INVALID_INPUT_1("Socket::Connect(): "
                              "0 remote port, sock 0x%x", this);
      dsErrno = QDS_EINVAL;
      goto bail;
    }

    /*-----------------------------------------------------------------------
      Perform routing only if routing cache is NULL. Otherwise, routing would
      be performed repeatedly on a TCP socket, if app polls for Connect() to
      succeed instead of waiting for WRITE_EV
    -----------------------------------------------------------------------*/
    if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex)) {
      LOG_MSG_ERROR_1("Socket::Connect(): "
                      "wrong event type, sock 0x%x", this);
      dsErrno = QDS_EINVAL;
      goto bail;
    }

    if (0 != ( (uint32)SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO &
                eventBitMask[writeEventIndex]))
    {
      dsErrno = RoutePacket( &tempAddr, &isReorig);
      if (AEE_SUCCESS != dsErrno)
      {
        if (AEE_EWOULDBLOCK != dsErrno ||
              (AEE_EWOULDBLOCK == dsErrno &&
               (0 != ( (uint32)SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO &
                        eventBitMask[writeEventIndex]))))
        {
          LOG_MSG_INFO3_2("Socket::Connect(): "
                          "RoutePacket failed, sock 0x%x err 0x%x",
                          this, dsErrno);
          goto bail;
        }
      }
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific Connect()
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Connect( &tempAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    if (AEE_EWOULDBLOCK != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::Connect(): "
                      "Connect failed, sock 0x%x err 0x%x", this, dsErrno);
      goto bail;
    }
  }
  else
  {
    dsErrno = AEE_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Generate route scope and set it in platform
  -------------------------------------------------------------------------*/
  routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
  routingCache    = reinterpret_cast <int32> ( routingCachePtr);

  tmpDSErrno =
    NetPlatform::GetRouteScopeByIfaceHandle( routingCache,
                                             ( ds::AddrFamily::QDS_AF_INET6 == family),
                                             &routeScope);
  if (AEE_SUCCESS != tmpDSErrno)
  {
    LOG_MSG_INFO3_2( "GetRouteScopeByIfaceHandle failed, sock 0x%x err 0x%x",
                     this, tmpDSErrno);
    dsErrno = tmpDSErrno;
    goto bail;
  }

  tmpDSErrno = platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != tmpDSErrno)
  {
    LOG_MSG_INFO3_2( "SetRouteScope failed, sock 0x%x err 0x%x",
                     this, tmpDSErrno);
    dsErrno = tmpDSErrno;
    goto bail;
  }

  critSectPtr->Leave();
  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::Connect() */


ds::ErrorType CDECL Socket::Listen
(
  int  backlog
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;

} /* Socket::Listen() */


ds::ErrorType CDECL Socket::Accept
(
  ds::SockAddrStorageType   remoteAddrPtr,
  ISocket **             newSockPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;

} /* Socket::Accept() */


ds::ErrorType CDECL Socket::GetSockName
(
   ds::SockAddrStorageType  localAddrPtr
)
{
  ds::ErrorType  dsErrno;
  SockAddrInternalType tempAddr = {{0}};
  ds::AddrFamilyType _family = ds::AddrFamily::QDS_AF_UNSPEC;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::GetSockName():  Sock 0x%x, "
                           "localAddrPtr 0x%x, platformSockPtr 0x%x",
                            this, localAddrPtr, platformSockPtr);

  if (0 == localAddrPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific GetSockName(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetSockName( &tempAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GetSockName(): "
                    "GetSockName failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  memscpy((void *) localAddrPtr,
          sizeof(ds::SockAddrStorageType),
          (void *) &tempAddr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(localAddrPtr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(localAddrPtr,ps_htons(_family));

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetSockName() */


ds::ErrorType CDECL Socket::GetPeerName
(
  ds::SockAddrStorageType remoteAddrPtr
)
{
  ds::ErrorType  dsErrno;
  SockAddrInternalType tempAddr = {{0}};
  ds::AddrFamilyType _family = ds::AddrFamily::QDS_AF_UNSPEC;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::GetPeerName(): Sock 0x%x, "
                           "remoteAddrPtr 0x%x, platformSockPtr 0x%x",
                            this, remoteAddrPtr, platformSockPtr);

  if (0 == remoteAddrPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific GetPeerName(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetPeerName( &tempAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_2("Socket::GetPeerName(): "
                    "GetPeerName failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  memscpy((void *) remoteAddrPtr,
          sizeof(ds::SockAddrStorageType),
          (void *) &tempAddr,
          sizeof(ds::SockAddrInternalType));

  (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(remoteAddrPtr,ps_htons(_family));

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetPeerName() */


ds::ErrorType CDECL Socket::SetOpt
(
  OptLevelType  optLevel,
  OptNameType   optName,
  int           optVal
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4("Socket::SetOpt(): "
                           "Sock 0x%x level %d name %d val %d",
                           this, optLevel, optName, optVal);

  /*-------------------------------------------------------------------------
    Validate that optName is in the supported range
  -------------------------------------------------------------------------*/
  if (OptName::NAME_MIN > optName || OptName::NAME_MAX <= optName)
  {
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Return error if option is not supported
  -------------------------------------------------------------------------*/
  if (false == IsOptSupported( optLevel, optName))
  {
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    Validate that
      1. optLevel is same as the defined option level for optName
      2. optLen is 4 bytes (A separate API is defined to support socket
         options with optVal > 4 bytes
      3. minVal <= optVal <= maxVal
      4. Socket's family is V6 if optLevel is V6
  -------------------------------------------------------------------------*/
  if (optLevel != sockOptInfoArr[optName].optLevel)
  {
    LOG_MSG_INVALID_INPUT_3("Socket::SetOpt(): "
                            "Invalid level %d name %d sock 0x%x",
                            optLevel, optName, this);
    return QDS_EINVAL;
  }

  if (sockOptInfoArr[optName].minVal > optVal ||
      sockOptInfoArr[optName].maxVal < optVal)
  {
    LOG_MSG_INVALID_INPUT_5("Socket::SetOpt(): "
                            "Val %d is out of range (%d - %d), "
                            "name %d sock 0x%x",
                            optVal,
                            sockOptInfoArr[optName].minVal,
                            sockOptInfoArr[optName].maxVal,
                            optName,
                            this);
    return QDS_EINVAL;
  }

  if (OptLevel::QDS_LEVEL_IPV6 == optLevel && ds::AddrFamily::QDS_AF_INET6 != family)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SetOpt(): "
                            "Opt %d - invalid on V4 sock 0x%x", optName, this);
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    It is not allowed to set values for following options:
      SO_TX_IFACE
      SO_ERROR
      TCP_FIONREAD
      SO_FIONREAD
  -------------------------------------------------------------------------*/
  if (OptName::QDS_SO_TX_IFACE == optName ||
      OptName::QDS_SO_ERROR == optName ||
      OptName::QDS_TCP_FIONREAD == optName ||
      OptName::QDS_SO_FIONREAD == optName)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SetOpt(): "
                            "Not supported, name %d sock 0x%x", optName, this);
    return QDS_EOPNOTSUPP;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SetOpt(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific SetOpt(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SetOpt( optLevel, optName, optVal, sizeof( int));
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetOpt(): "
                    "SetOpt failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::SetOpt() */


ds::ErrorType CDECL Socket::GetOpt
(
  OptLevelType  optLevel,
  OptNameType   optName,
  int *         optValPtr
)
{
  int            optLen;
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate that optName is in the supported range
  -------------------------------------------------------------------------*/
  if (OptName::NAME_MIN > optName || OptName::NAME_MAX <= optName)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOpt(): "
                            "Invalid name %d, sock 0x%x", optName, this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Return error if option is not supported
  -------------------------------------------------------------------------*/
  if (false == IsOptSupported( optLevel, optName))
  {
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    Validate that
      1. optLevel is same as the defined option level for optName
      2. Socket's family is V6 if optLevel is V6
  -------------------------------------------------------------------------*/
  if (optLevel != sockOptInfoArr[optName].optLevel)
  {
    LOG_MSG_INVALID_INPUT_3("Socket::GetOpt(): "
                            "Invalid level %d, name %d sock 0x%x",
                            optLevel, optName, this);
    return QDS_EINVAL;
  }

  if (OptLevel::QDS_LEVEL_IPV6 == optLevel && ds::AddrFamily::QDS_AF_INET6 != family)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOpt(): "
                            "Opt %d - invalid on V4 sock 0x%x", optName, this);
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    It is not allowed to get values for ICMP level options
  -------------------------------------------------------------------------*/
  if (OptLevel::QDS_LEVEL_ICMP == optLevel)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOpt(): "
                            "Not supported, name %d sock 0x%x", optName, this);
    return QDS_EOPNOTSUPP;
  }

  critSectPtr->Enter();

  if (OptName::QDS_SO_TX_IFACE == optName)
  {
    if (0 != rtMetaInfoPtr)
    {
      *optValPtr =
        reinterpret_cast <int32> (PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr));
    }
    else
    {
      *optValPtr = 0;
    }

    optLen = sizeof( int32);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Make sure that platform socket is not closed yet. Else if platform socket
      is deleted, a 0 address would be accessed causing a crash
    -----------------------------------------------------------------------*/
    if (0 == platformSockPtr)
    {
      LOG_MSG_INVALID_INPUT_1("Socket::GetOpt(): "
                              "Sock 0x%x closed", this);
      dsErrno = QDS_EINVAL;
      goto bail;
    }

    /*-----------------------------------------------------------------------
      Call platform specific GetOpt(...) and return
    -----------------------------------------------------------------------*/
    dsErrno = platformSockPtr->GetOpt( optLevel,
                                       optName,
                                       reinterpret_cast <int32 *> ( optValPtr),
                                       reinterpret_cast <int32 *> ( &optLen));
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::GetOpt(): "
                      "GetOpt failed, sock 0x%x err 0x%x", this, dsErrno);
      goto bail;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4("Socket::GetOpt(): "
                          "Success, sock 0x%x, level %d, name %d, optVal %d", 
                           this, optLevel, optName, *optValPtr);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetOpt() */

ds::ErrorType CDECL Socket::SetOptPriv
(
  OptLevelType     optLevel,
  OptNameTypePriv  optName,
  int              optVal
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4("Socket::SetOptPriv(): "
                           "Sock 0x%x level %d name %d val %d",
                           this, optLevel, optName, optVal);

  /*-------------------------------------------------------------------------
    Validate that optName is in the supported range
  -------------------------------------------------------------------------*/
  if (OptNamePriv::NAME_MIN > optName || OptNamePriv::NAME_MAX <= optName)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SetOptPriv(): "
                            "Invalid name %d, sock 0x%x", optName, this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Return error if option is not supported
  -------------------------------------------------------------------------*/
  if (false == IsOptPrivSupported( optLevel, optName))
  {
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    Validate that
      1. optLevel is same as the defined option level for optName
      2. optLen is 4 bytes (A separate API is defined to support socket
         options with optVal > 4 bytes
      3. minVal <= optVal <= maxVal
      4. Socket's family is V6 if optLevel is V6
  -------------------------------------------------------------------------*/
  if (optLevel != sockOptPrivInfoArr[optName].optLevel)
  {
    LOG_MSG_INVALID_INPUT_3("Socket::SetOptPriv(): "
                            "Invalid level %d name %d sock 0x%x",
                            optLevel, optName, this);
    return QDS_EINVAL;
  }

  if (sockOptPrivInfoArr[optName].minVal > optVal ||
      sockOptPrivInfoArr[optName].maxVal < optVal)
  {
    LOG_MSG_INVALID_INPUT_5("Socket::SetOptPriv(): "
                            "Val %d is out of range (%d - %d), "
                            "name %d sock 0x%x",
                            optVal,
                            sockOptPrivInfoArr[optName].minVal,
                            sockOptPrivInfoArr[optName].maxVal,
                            optName,
                            this);
    return QDS_EINVAL;
  }

  if (OptLevel::QDS_LEVEL_IPV6 == optLevel &&
      ds::AddrFamily::QDS_AF_INET6 != family)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SetOptPriv(): "
                            "Opt %d - invalid on V4 sock 0x%x", optName, this);
    return QDS_ENOPROTOOPT;
  }


  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SetOptPriv(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific SetOptPriv(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SetOptPriv( optLevel, optName, optVal, sizeof( int));
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetOptPriv(): "
                    "SetOptPriv failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::SetOptPriv() */


ds::ErrorType CDECL Socket::GetOptPriv
(
  OptLevelType    optLevel,
  OptNameTypePriv optName,
  int *           optValPtr
)
{
  int            optLen;
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate that optName is in the supported range
  -------------------------------------------------------------------------*/
  if (OptNamePriv::NAME_MIN > optName || OptNamePriv::NAME_MAX <= optName)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOptPriv(): "
                            "Invalid name %d, sock 0x%x", optName, this);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Return error if option is not supported
  -------------------------------------------------------------------------*/
  if (false == IsOptPrivSupported( optLevel, optName))
  {
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    Validate that
      1. optLevel is same as the defined option level for optName
      2. Socket's family is V6 if optLevel is V6
  -------------------------------------------------------------------------*/
  if (optLevel != sockOptPrivInfoArr[optName].optLevel)
  {
    LOG_MSG_INVALID_INPUT_3("Socket::GetOptPriv(): "
                            "Invalid level %d, name %d sock 0x%x",
                            optLevel, optName, this);
    return QDS_EINVAL;
  }

  if (OptLevel::QDS_LEVEL_IPV6 == optLevel &&
      ds::AddrFamily::QDS_AF_INET6 != family)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOptPriv(): "
                            "Opt %d - invalid on V4 sock 0x%x", optName, this);
    return QDS_ENOPROTOOPT;
  }

  /*-------------------------------------------------------------------------
    It is not allowed to get values for ICMP level options
  -------------------------------------------------------------------------*/
  if (OptLevel::QDS_LEVEL_ICMP == optLevel)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::GetOptPriv(): "
                            "Not supported, name %d sock 0x%x", optName, this);
    return QDS_EOPNOTSUPP;
  }

  critSectPtr->Enter();

  /*-----------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -----------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::GetOptPriv(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Call platform specific GetOptPriv(...) and return
  -----------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetOptPriv( optLevel,
                                         optName,
                                         reinterpret_cast <int32 *> ( optValPtr),
                                         reinterpret_cast <int32 *> ( &optLen));
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GetOptPriv(): "
                    "GetOptPriv failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  LOG_MSG_FUNCTION_EXIT_4("Socket::GetOptPriv(): "
                           "Sock 0x%x, level %d, name %d, value %d",
                           this, optLevel, optName, *optValPtr);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetOptPriv() */


ds::ErrorType CDECL Socket::Shutdown
(
  ShutdownDirType  shutdownDir
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ShutdownDir::QDS_READ_DIR != shutdownDir && ShutdownDir::QDS_WRITE_DIR != shutdownDir &&
      ShutdownDir::QDS_READ_WRITE_DIR != shutdownDir)
  {
    LOG_MSG_INVALID_INPUT_2("Socket::Shutdown(): "
                            "Invalid dir %d, sock 0x%x", shutdownDir, this);
    return QDS_EINVAL;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Shutdown(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific Shutdown(...)
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Shutdown( shutdownDir);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_3("Socket::Shutdown(): "
                    "Shutdown failed, sock 0x%x err 0x%x dir %d", this, dsErrno, shutdownDir);
    goto bail;
  }

  LOG_MSG_FUNCTION_EXIT_2("Socket::Shutdown(): "
                          "Success, sock 0x%x  dir %d", this, shutdownDir);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::Shutdown() */


ds::ErrorType CDECL Socket::Close
(
  void
)
throw()
{
   ds::SockAddrInternalType   localAddr = {{0}};
   ds::ErrorType        dsErrno;
   ds::ErrorType        tmpDSErrno;
   ds::AddrFamilyType   addrFamily = ds::AddrFamily::QDS_AF_UNSPEC;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Socket::Close(): "
                           "Sock 0x%x, platformSockPtr 0x%x",
                           this, platformSockPtr);

  if (0 != critSectPtr)
  {
    critSectPtr->Enter();
  }

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = AEE_SUCCESS;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Fetch the address to which socket is bound to. If it is a IPv6 address,
    its refcnt may need to be decremented later
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetSockName( &localAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Close(): "
                    "GetSockName failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific Close(...)

    Do not treat AEE_EWOULDBLOCK as error as TCP socket may return it if the
    socket was connected
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Close();
  if (AEE_SUCCESS == dsErrno)
  {
    platformSockPtr = 0;
    dsErrno         = AEE_SUCCESS;

    /*-----------------------------------------------------------------------
      Iface address management layer needs to keep track of how many
      clients are using each IPv6 address so that it can manage the
      lifetime of addresses. So decrement ref cnt since client has
      successfully closed the socket.

      If ref cnt is decremented before Close() returns success, private
      address could be deleted and TCP socket won't be able to send FIN out
      because address is not valid anymore.

      It is not necessary to check if application explicitly bound to this
      address. Ref cnt is maintained only for private addresses, and unless
      an application calls Bind(), socket will not be bound to a private
      address.
    -----------------------------------------------------------------------*/
    (void)ds::Sock::AddrUtils::GetFamily(&localAddr,&addrFamily);

    if (ds::AddrFamily::QDS_AF_INET6 == addrFamily)
    {
      if (PS_IN6_IS_ADDR_V6( localAddr.v6.addr) &&
        false == PS_IN6_IS_ADDR_MULTICAST( localAddr.v6.addr))
      {
        /*lint -e{545} */
        (void) NetPlatform::IPv6PrivAddrDecRefCnt( &( localAddr.v6.addr),
                                                   &tmpDSErrno);
      }
    }

    /*-----------------------------------------------------------------------
      Detach NetworkCtl from Network events
    -----------------------------------------------------------------------*/
    if(0 != networkCtlPtr)
    {
      (void) networkCtlPtr->Close();
    }

    /*-----------------------------------------------------------------------
      Detach self from NetworkCtl events
    -----------------------------------------------------------------------*/
    DetachNetworkStateChangeSignalCtls();
  }
  else if (AEE_EWOULDBLOCK != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Close(): "
                    "Close failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Common error handling code + normal exit
  -------------------------------------------------------------------------*/
bail:
  if (0 != critSectPtr)
  {
    critSectPtr->Leave();
  }
  return dsErrno;

} /* Socket::Close() */


ds::ErrorType CDECL Socket::RegEvent
(
  ISignal *            signalObjPtr,
  ds::Sock::SocketEventType  event,
  IQI**                      regObj
)
{
  ds::ErrorType        dsErrno;
  uint32               eventIndex      = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::RegEvent(): "
                           "Sock 0x%x, platformSockPtr 0x%x, signalObjPtr 0x%x",
                           this, platformSockPtr, signalObjPtr);
  /*-------------------------------------------------------------------------
    Due to a possible security issue where applications may invoke this
    function many times thus causing undesirable extra memory allocation that
    may be entailed in an implementation of socket events registration,
    we should return an object that the application will hold for each
    RegEvent invocation. A Limit that CS imposes on the number of objects the
    application can hold resolves the security issue.

    If the implementation of RegEvent is changed an appropriate
    object must be returned.
  -------------------------------------------------------------------------*/

  *regObj = new RegBound();

  if (0 == signalObjPtr)
  {
    return QDS_EFAULT;
  }

  if (AEE_SUCCESS != GetIndexFromEvent (event, &eventIndex))
  {
    LOG_MSG_INVALID_INPUT_2("Socket::RegEvent(): "
                            "Invalid ev %d, sock 0x%x", event, this);
    return QDS_EINVAL;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Enqueue signal in to the signal bus. Bus takes care of setting the
    signal if the event is TRUE.
  -------------------------------------------------------------------------*/
  (void) eventSignalBuses[eventIndex].Add( signalObjPtr);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::RegEvent() */


ds::ErrorType CDECL Socket::Write
(
  const byte  bufArr[],
  int         bufLen,
  int *       numWrittenPtr
)
{
  SockAddrInternalType  remoteSockAddr = {{0}};
  IPort1::SeqBytes      ioVec[1];
  ds::ErrorType         dsErrno;
  uint32                writeEventIndex = 0;
  boolean               isReorig = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == numWrittenPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Write(): "
                            "NULL arg, sock 0x%x", this);
    dsErrno = QDS_EFAULT;
    goto bail;
  }

  if (NULL == bufArr && 0 != bufLen)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Write(): "
                            "0 len, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Write(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_ENETDOWN;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If Write event bit mask is set, handle the transient/flow control errors.
    Return error if all the errors couldn't be handled
  -------------------------------------------------------------------------*/

  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex)){
    LOG_MSG_ERROR_1("Socket::Write(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[writeEventIndex])
  {
    /*-----------------------------------------------------------------------
      Routing relies on destination address. So get peer address and also
      make sure that socket is connected
      //TODO handles all cases?? (states)
    -----------------------------------------------------------------------*/
    dsErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::Write(): "
                      "GetPeerName failed, sock 0x%x err 0x%x", this, dsErrno);
      goto bail;
    }

    dsErrno = HandleNonZeroWriteEventBitMask( &remoteSockAddr, 
                                              &isReorig, 
                                              (byte *) bufArr, 
                                              bufLen);
    if (0 != eventBitMask[writeEventIndex])
    {
      LOG_MSG_INFO1_3("Socket::Write(): "
                      "Write ev bit mask 0x%x, sock 0x%x err 0x%x",
                      eventBitMask[writeEventIndex], this, dsErrno);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific SendMsg(...) and return
  -------------------------------------------------------------------------*/
  ioVec[0].data       = const_cast <byte *> ( bufArr);
  ioVec[0].dataLen    = bufLen;
  ioVec[0].dataLenReq = 0;

  dsErrno =
    platformSockPtr->SendMsg( 0,
                              ioVec,
                              1,
                              reinterpret_cast <int32 *> ( numWrittenPtr),
                              0,
                              0,
                              0,
                              0);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Write(): "
                    "SendMsg failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();

  LOG_MSG_INFO4_3("Socket::Write(): "
                  "Success sock 0x%x, wrote %d, len %d", this, *numWrittenPtr, bufLen);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::Write() */


ds::ErrorType CDECL Socket::WriteV
(
  const IPort1::SeqBytes   ioVecArr[],
  int                      numIOVec,
  int *                    numWrittenPtr
)
{
  SockAddrInternalType remoteSockAddr;
  ds::ErrorType        dsErrno;
  uint32               writeEventIndex = 0;
  boolean               isReorig = FALSE;
  ps_iface_type        *iface_ptr = NULL;  
  ps_iface_type        *base_iface_ptr = NULL;  
  ds_sys_tech_enum_type tech_type = DS_SYS_TECH_INVALID;
  byte                  packet[PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE] = {0};
  int                   i = 0;
  byte                 *packet_ptr = NULL;
  int                   packet_len = PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == numWrittenPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::WriteV(): "
                            "NULL arg, sock 0x%x", this);
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::WriteV(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_ENETDOWN;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If Write event bit mask is set, handle the transient/flow control errors.
    Return error if all the errors couldn't be handled
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex)){
    LOG_MSG_ERROR_1("Socket::WriteV(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }


  if (0 != eventBitMask[writeEventIndex])
  {
    /*-----------------------------------------------------------------------
      Routing relies on destination address. So get peer address and also
      make sure that socket is connected
      //TODO handles all cases?? (states)
    -----------------------------------------------------------------------*/
    dsErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::WriteV(): "
                      "GetPeerName failed, sock 0x%x err 0x%x", this, dsErrno);
      goto bail;
    }

    dsErrno = HandleNonZeroWriteEventBitMask( &remoteSockAddr, &isReorig);
    if (isReorig == TRUE)
    {
      packet_ptr = packet;

      for (i = 0; i < numIOVec; i++) 
      {
        if (packet_len <= 0)
        {
          break;
        }
        memscpy(packet_ptr, 
                packet_len, 
                ioVecArr[i].data, 
                ioVecArr[i].dataLen); 
        packet_ptr += ioVecArr[i].dataLen;
        packet_len -= ioVecArr[i].dataLen;
      }

      iface_ptr      = (ps_iface_type *)PS_RT_META_GET_ROUTING_CACHE(rtMetaInfoPtr);
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);

      if (base_iface_ptr != NULL)
      {
        if (base_iface_ptr->name == UMTS_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP;
        }
        else if (base_iface_ptr->name == CDMA_SN_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP2;
        }
        if (packet != NULL)
        {
          ps_wmk_logging_first_reorig_ip_pkt_hdr( tech_type, 
                                                  PS_WMK_UPLINK_PKT, 
                                                  packet);
        }
      }
    }

    if (0 != eventBitMask[writeEventIndex])
    {
      LOG_MSG_INFO1_3("Socket::WriteV(): "
                      "Write ev bit mask 0x%x, sock 0x%x err 0x%x",
                      eventBitMask[writeEventIndex], this, dsErrno);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific SendMsg(...) and return
  -------------------------------------------------------------------------*/
  dsErrno =
    platformSockPtr->SendMsg( 0,
                              ioVecArr,
                              numIOVec,
                              reinterpret_cast <int32 *> ( numWrittenPtr),
                              0,
                              0,
                              0,
                              0);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::WriteV(): "
                    "SendMsg failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();

  LOG_MSG_INFO4_2("Socket::WriteV(): "
                  "Success, sock 0x%x wrote %d", this, *numWrittenPtr);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::WriteV() */


ds::ErrorType CDECL Socket::WriteDSMChain
(
  dsm_item_type **  dsmItemPtrPtr,
  int32 *           numBytesWrittenPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::WriteDSMChain() */


ds::ErrorType Socket::SendMsg
(
  const ds::SockAddrStorageType msg_name,
  const byte *                msg_buf,
  int                         msg_bufLen,
  int *                       numWritten,
  const byte *                msg_control,
  int                         msg_controlLen,
  unsigned int                flags
)
{
  IPort1::SeqBytes   ioVecArr[1];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validating only the parameters that are not being validated by the
    private SendMsg API used by this function.
  -------------------------------------------------------------------------*/
  if ( 0 != msg_controlLen && 0 == msg_control )
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SendMsg(): "
                            "NULL msg_control with msg_controlLen %d, sock 0x%x",
                            msg_controlLen, this);
    return QDS_EINVAL;
  }

  if ( 0 != flags )
  {
    LOG_MSG_INVALID_INPUT_2("Socket::SendMsg(): "
                            "No support for flags %x in SendMsg, sock 0x%x",
                            flags, this);
    return QDS_EAFNOSUPPORT;
  }
  /*Fill the ioVec with the data to send */
  ioVecArr[0].data       = const_cast <byte *> ( msg_buf );
  ioVecArr[0].dataLen    = msg_bufLen;
  ioVecArr[0].dataLenReq = 0;
//TODO remove the int32 casting
  return SendMsg(msg_name,ioVecArr, 1, numWritten,
                  0, 0, 0);

}

ds::ErrorType Socket::SendMsg
(
  const ds::SockAddrStorageType   remoteAddrPtr,
  const IPort1::SeqBytes *        ioVecArr,
  int                             numIOVec,
  int *                           numWrittenPtr,
  IAncDataPriv **                 inAncillaryDataPtrPtr,
  int                             inAncillaryDataLen,
  unsigned int                    flags
)
{
  const SockAddrInternalType*     peerAddrPtr;
  ps_pkt_meta_info_type *         pktMetaInfoPtr = 0;
  SockAddrInternalType            remoteSockAddr= {{0}};
  SockAddrIN6InternalType         v6RemoteAddr= {0};
  SockAddrInternalType            tempAddr = {{0}};
  ds::ErrorType                   dsErrno;
  bool                            localSendPktMetaInfo = sendPktMetaInfo;
  ds::AddrFamilyType              _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32                          writeEventIndex = 0;
  bool                            isDoS = false;
  boolean                         isReorig = FALSE;
  ps_iface_type                  *iface_ptr = NULL;  
  ps_iface_type                  *base_iface_ptr = NULL;  
  ds_sys_tech_enum_type           tech_type = DS_SYS_TECH_INVALID;
  byte                            packet[PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE]={0};
  int                             i = 0;
  byte                           *packet_ptr = NULL;
  int                             packet_len = PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == remoteAddrPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SendMsg(): "
                            "NULL remote addr, sock 0x%x", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Ancillary data is not supported for SendMsg
  -------------------------------------------------------------------------*/
  if ( 0 != inAncillaryDataPtrPtr && 0 != *inAncillaryDataPtrPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SendMsg(): "
                            "Anc Data isn't supported, sock 0x%x", this);
    return QDS_EOPNOTSUPP;
  }

  /*-------------------------------------------------------------------------
    Only MSG_EXPEDITE, MSG_FAST_EXPEDITE, MSG_SOCK_WAKEUP flags are supported
    in tx path
  -------------------------------------------------------------------------*/
  if (0 != flags)
  {
    if (((SendFlags::QDS_MSG_EXPEDITE | SendFlags ::QDS_MSG_SOCK_WAKEUP) != flags) &&
        ((SendFlags::QDS_MSG_FAST_EXPEDITE | SendFlags ::QDS_MSG_SOCK_WAKEUP) != flags) &&
        (SendFlags::QDS_MSG_EXPEDITE != flags)&&
        (SendFlags::QDS_MSG_FAST_EXPEDITE != flags))
    {
      LOG_MSG_INVALID_INPUT_2("Socket::SendMsg(): "
                              "Unsupported flag 0x%x, sock 0x%x", flags, this);
      return QDS_EOPNOTSUPP;
    }

    localSendPktMetaInfo = true;
    isDoS = true;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SendMsg(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  memscpy((void *) &tempAddr,
          sizeof(ds::SockAddrInternalType),
          (void *) remoteAddrPtr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(&tempAddr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(&tempAddr,ps_ntohs(_family));
  /*-------------------------------------------------------------------------
    If destination address is not specified, make sure that socket is
    connected
  -------------------------------------------------------------------------*/
  dsErrno = AddrUtils::GetSockAddrIN6( &tempAddr, &v6RemoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("Socket::SendMsg(): "
                    "GetSockAddrIN6 failed, err 0x%x", dsErrno);
    goto bail;
  }

  if (PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddr.addr, &ps_in6addr_any))
  {
    dsErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendMsg(): "
                      "GetPeerName failed, sock 0x%x err 0x%x", this, dsErrno);
      dsErrno = QDS_EADDRREQ;
      goto bail;
    }

    peerAddrPtr = &remoteSockAddr;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If packet info differs, set NULL_ROUTING_INFO bit so that a fresh
      routing is performed
    -----------------------------------------------------------------------*/
    if (IsPktInfoDifferent( &tempAddr)|| isDoS == true)
    {
      ResetRoutingCache(true);
    }

    peerAddrPtr = &tempAddr;
  }

  /*-------------------------------------------------------------------------
    If Write event bit mask is set, handle the transient/flow control errors.
    Return error if all the errors couldn't be handled
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex)){
    LOG_MSG_ERROR_1("Socket::SendMsg(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[writeEventIndex])
  {
    dsErrno = HandleNonZeroWriteEventBitMask( peerAddrPtr, 
                                              &isReorig,
                                              NULL,
                                              0,
                                              isDoS);
    if (isReorig == TRUE)
    {
      packet_ptr = packet;

      for (i = 0; i < numIOVec; i++) 
      {
        if (packet_len <= 0)
        {
          break;
        }
        memscpy(packet_ptr, 
                packet_len, 
                ioVecArr[i].data, 
                ioVecArr[i].dataLen); 
        packet_ptr += ioVecArr[i].dataLen;
        packet_len -= ioVecArr[i].dataLen;
      }

      iface_ptr      = (ps_iface_type *)PS_RT_META_GET_ROUTING_CACHE(rtMetaInfoPtr);
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
      if (base_iface_ptr != NULL)
      {
        if (base_iface_ptr->name == UMTS_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP;
        }
        else if (base_iface_ptr->name == CDMA_SN_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP2;
        }
        if (packet != NULL)
        {
          ps_wmk_logging_first_reorig_ip_pkt_hdr( tech_type, 
                                                  PS_WMK_UPLINK_PKT, 
                                                  packet);
        }
      }
    }

    if (0 != eventBitMask[writeEventIndex])
    {
      /*-----------------------------------------------------------------------
        Since any of these options means we shouldn't re-orig, set to false
      -----------------------------------------------------------------------*/
      if (isDoS == true)
      {
        if ( (eventBitMask[writeEventIndex] &
             ~(SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED |
               SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED |
               SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL)) != 0)
        {
          LOG_MSG_INFO1_3("Socket::SendMsg(): "
                          "Write ev bit mask for DoS 0x%x, sock 0x%x err"
                          " 0x%x",
                          eventBitMask[writeEventIndex], this, dsErrno);
          goto bail;
        }
        else if ((eventBitMask[writeEventIndex] &
                  SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED) != 0)
        {
          if(!IsFlowDoSEnabled())
          {
            LOG_MSG_INFO1_2("Socket::SendMsg(): "
                            "Flow disabled for DoS, sock 0x%x err 0x%x",
                            this, dsErrno);
            goto bail;
          }
        }
      }
      else
      {
        LOG_MSG_INFO1_3("Socket::SendMsg(): "
                        "Write ev bit mask 0x%x, sock 0x%x err 0x%x",
                        eventBitMask[writeEventIndex], this, dsErrno);
        goto bail;
      }
    }
  }

  if (localSendPktMetaInfo)
  {
    dsErrno = GeneratePktMetaInfo( &pktMetaInfoPtr, flags);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendMsg(): "
                      "GeneratePktMetaInfo failed, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific SendMsg(...) and return
  -------------------------------------------------------------------------*/
  dsErrno =
    platformSockPtr->SendMsg( &tempAddr,
                              ioVecArr,
                              numIOVec,
                              reinterpret_cast <int32 *> ( numWrittenPtr),
                              inAncillaryDataPtrPtr,
                              inAncillaryDataLen,
                              flags,
                              pktMetaInfoPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SendMsg(): "
                    "SendMsg failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  PS_PKT_META_INFO_FREE( &pktMetaInfoPtr);

  critSectPtr->Leave();
  LOG_MSG_INFO4_2("Socket::SendMsg(): "
                  "Success, sock 0x%x wrote %d", this, *numWrittenPtr);

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  if (0 != pktMetaInfoPtr)
  {
    PS_PKT_META_INFO_FREE( &pktMetaInfoPtr);
  }

  critSectPtr->Leave();
  return dsErrno;

} /* Socket::SendMsg() */


ds::ErrorType CDECL Socket::SendTo
(
  const byte                   bufArr[],
  int                          bufLen,
  const ds::SockAddrStorageType remoteAddrPtr,
  unsigned int                  flags,
  int *                         numWrittenPtr
)
{
  IPort1::SeqBytes   ioVecArr[1];
  ds::ErrorType      dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Fill the IOVEC array
  -------------------------------------------------------------------------*/
  ioVecArr[0].data       = const_cast <byte *> ( bufArr);
  ioVecArr[0].dataLen    = bufLen;
  ioVecArr[0].dataLenReq = 0;

  /*-------------------------------------------------------------------------
    Call Base Socket SendMsg
  -------------------------------------------------------------------------*/
    dsErrno =
      SendMsg( remoteAddrPtr, ioVecArr, 1, numWrittenPtr, 0, 0, flags);

  return dsErrno;

} /* Socket::SendTo() */


ds::ErrorType CDECL Socket::SendToDSMChain
(
  dsm_item_type **             dsmItemPtrPtr,
  const ds::SockAddrStorageType    remoteAddrPtr,
  unsigned int                 flags,
  int32 *                      numBytesSentPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::SendToDSMChain() */


ds::ErrorType CDECL Socket::Read
(
  byte   bufArr[],
  int    bufLen,
  int *  numReadPtr
)
{
  IPort1::SeqBytes   ioVecArr[1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ioVecArr[0].data       = bufArr;
  ioVecArr[0].dataLen    = bufLen;
  ioVecArr[0].dataLenReq = 0;

  return RecvMsg( 0, ioVecArr, 1, numReadPtr, 0, 0, 0, 0, 0);
} /* Socket::Read() */


ds::ErrorType CDECL Socket::ReadV
(
  IPort1::SeqBytes  ioVecArr[],
  int               numIOVec,
  int *             numReadPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return RecvMsg( 0, ioVecArr, numIOVec, numReadPtr, 0, 0, 0, 0, 0);
} /* Socket::ReadV() */


ds::ErrorType CDECL Socket::ReadDSMChain
(
  dsm_item_type **  dsmItemPtrPtr,
  int32 *           numBytesReadPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::ReadDSMChain() */


ds::ErrorType CDECL Socket::RecvFrom
(
  byte                   bufArr[],
  int                    bufLen,
  int *                  numReadPtr,
  unsigned int            flags,
  ds::SockAddrStorageType remoteAddrPtr
)
{
  IPort1::SeqBytes   ioVecArr[1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ioVecArr[0].data       = bufArr;
  ioVecArr[0].dataLen    = bufLen;
  ioVecArr[0].dataLenReq = 0;

  return RecvMsg( remoteAddrPtr, ioVecArr, 1, numReadPtr, 0, 0, 0, 0, flags);
} /* Socket::RecvFrom() */


ds::ErrorType CDECL Socket::RecvFromDSMChain
(
  dsm_item_type **       dsmItemPtrPtr,
  ds::SockAddrStorageType remoteAddrPtr,
  unsigned int           flags,
  int32 *                numBytesRcvdPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::RecvFromDSMChain() */


ds::ErrorType CDECL Socket::RecvMsg
(
  ds::SockAddrStorageType  msg_name,
  byte *                   msg_buf,
  int                      msg_bufLen,
  int *                    msg_bufLenReq,
  byte *                   msg_control,
  int                      msg_controlLen,
  int *                    msg_controlLenReq,
  unsigned int *           msg_flags,
  unsigned int             flags
)
{
  ds::Sock::IAncDataPriv **  pOutAncData = NULL;
  int                        nOutAncLen = 0;
  int                        nOutAncLenReq = 0, i = 0;
  int                        isRecvIfSet = 0;
  int                        isRecvErrSet =0;
  ds::ErrorType              res;
  ds::ErrorType              tempRes = AEE_SUCCESS;
  IPort1::SeqBytes           ioVecArr[1];
  uint16                     controllen  = 0;
  dss_msghdr                 msg;
  dss_cmsghdr*               cmsg_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_1("Socket::RecvMsg(): "
                           " Public RecvMsg Sock 0x%x", this);

  /*-------------------------------------------------------------------------
    Validating only the parameters that are not being validated by the
    private RecvMsg API used by this function.
    -----------------------------------------------------------------------*/
  if ( 0 != msg_controlLen && NULL == msg_control )
  {
    LOG_MSG_INVALID_INPUT_2("Socket::RecvMsg(): "
                            "NULL msg_control with msg_controlLen %d, sock 0x%x",
                            msg_controlLen, this);
    return QDS_EINVAL;
  }

  ioVecArr[0].data       = msg_buf;
  ioVecArr[0].dataLen    = msg_bufLen;
  ioVecArr[0].dataLenReq = 0;

  /*-------------------------------------------------------------------------
    Calculate the number of ancillary data pointers that we need to allocate.
    This number is limited by the length of buffer provided by the user.
  -------------------------------------------------------------------------*/
  nOutAncLen = (unsigned int)msg_controlLen / sizeof(struct dss_cmsghdr);
  pOutAncData = NULL;

  /*-------------------------------------------------------------------------
    Allocate ancillary data pointers
  -------------------------------------------------------------------------*/
  if (0 != nOutAncLen)
  {
     PS_SYSTEM_HEAP_MEM_ALLOC(pOutAncData,
                             sizeof(ds::Sock::IAncDataPriv *)*nOutAncLen,
                             ds::Sock::IAncDataPriv**);
    if (NULL == pOutAncData)
    {
      return AEE_ENOMEMORY;
    }
  }

  res = RecvMsg( msg_name, ioVecArr, 1, msg_bufLenReq, pOutAncData,
                 nOutAncLen, &nOutAncLenReq, msg_flags, flags);
  if ( AEE_SUCCESS != res &&
       QDS_EEOF != res &&
       QDS_EMSGTRUNC != res )
  {
    goto bail;
  }

  *msg_controlLenReq = nOutAncLenReq * sizeof(struct dss_cmsghdr);

  if ( pOutAncData == NULL || nOutAncLenReq == 0 )
  {
    //No ANC data
    goto bail;
  }
  /* Created a local msghdr structure in order to reuse the existing cmsghdr macros*/
  memset( &msg, 0, sizeof( dss_msghdr));
  msg.msg_control = msg_control;
  msg.msg_controllen = msg_controlLen;

  // Check if RECVIF was set to the socket
  res = GetOpt( OptLevel::QDS_LEVEL_IP,
                OptName::QDS_IP_RECVIF,
                &isRecvIfSet );

  if ( AEE_SUCCESS != res )
  {
    LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                    "GetOpt QDS_IP_RECVIF failed: %d", res);
    goto bail;
  }

  // Check if RECVERR was set to the socket
  res = GetOpt( OptLevel::QDS_LEVEL_IP,
                OptName::QDS_IP_RECVERR,
                &isRecvErrSet );
  if ( AEE_SUCCESS != res )
  {
    LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                    "GetOpt QDS_IP_RECVERR failed: %d", res);
    goto bail;
  }
   // convert all the ancillary data structures that we got from RecvMsg
  for (i=0 , cmsg_ptr = DSS_CMSG_FIRSTHDR(&msg); i < nOutAncLenReq; i++)
  {
    AncDataIDType       AncID = 0;
    ExtendedErrInfoType ErrInfo = {{0,0}};
    RecvIfaceInfoType   IfaceInfo = {0};
    if (NULL == cmsg_ptr)
    {
      LOG_MSG_ERROR_0("Socket::RecvMsg(): "
                      "msg_control or DSS_CMSG_NXTHDR failed and returned NULL");
      res = DS_EFAULT;
      goto bail;
    }
    // Get the type of this ancillary data
    if (NULL != pOutAncData)
    {
      res = pOutAncData[i]->GetAncID(&AncID);
      if ( AEE_SUCCESS != res)
      {
        LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                        "GetAncID failed:%d", res);
        goto bail;
      }
    }

    // Extract data from pOutAncData[i] in accordance with the Ancillary data type
    switch (AncID)
    {
      case AncData::ICMP_ERROR_INFO:
      {
        if ((RecvFlags::QDS_MSG_ERRQUEUE & flags) && (TRUE == isRecvErrSet))
        {
          res = ((IICMPErrInfoPriv*)pOutAncData[i])->GetExtendedErr(&ErrInfo.err);
          if ( AEE_SUCCESS != res )
          {
            LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                            "GetErr failed:%d", res);
            goto bail;
          }
          // Convert to little-endian
          ErrInfo.err.error_number = dss_htolel(ErrInfo.err.error_number);
          ErrInfo.err.info = dss_htolel(ErrInfo.err.info);

          res = ((IICMPErrInfoPriv*)pOutAncData[i])->GetAddr(ErrInfo.addr);
          if ( AEE_SUCCESS != res )
          {
            LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                            "GetAddr failed:%d", res);
            goto bail;
          }

          /*------------------------------------------------------------------
            Sanity check for not exceeding msg_control buf.
            It was already tested by setting nOutAncLen to the correct len
            before calling the private recvmsg.
          -------------------------------------------------------------------*/
          if ( (controllen + DSS_CMSG_SPACE(sizeof(ErrInfo)) ) > msg.msg_controllen )
          {
            LOG_MSG_ERROR_0("Socket::RecvMsg(): "
                            "cmsg buf too short for err_info");
            res = QDS_ETRUNCATED;
            goto bail;
          }
          tempRes += ds::Utils::Conversion::PutCMsg(&msg, cmsg_ptr,
                                (int16)OptLevel::QDS_LEVEL_IP,
                                (int16)OptName::QDS_IP_RECVERR,
                                sizeof(ErrInfo),
                                (void*)&ErrInfo,
                                &controllen);
          if ( AEE_SUCCESS != tempRes )
          {
            LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                            "Failed to put cmsg with err %d", tempRes );
          }
        }
        break;
      }

      case ::ds::Sock::AncData::RECV_IF_INFO:
      {
        if (TRUE == isRecvIfSet)
        {
          res = ((ds::Sock::IRecvIFInfoPriv*)
                          pOutAncData[i])->GetRecvIFHandle(&IfaceInfo.IfaceId);
          if (AEE_SUCCESS != res)
          {
            LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                            "GetRecvIFHandle failed:%d", res);
          }
          // Convert to little-endian
          IfaceInfo.IfaceId = dss_htolel(IfaceInfo.IfaceId);
          /*-------------------------------------------------------------------------
            Sanity check for not exceeding msg_control buf.
            It was already requested by setting nOutAncLen
            to the correct len before calling the private recvmsg.
          -------------------------------------------------------------------------*/
          if ( (controllen + DSS_CMSG_SPACE(sizeof(IfaceInfo)) ) > msg.msg_controllen )
          {
            LOG_MSG_ERROR_0("Socket::RecvMsg(): "
                            "cmsg buf too short for in_pktinfo");
            goto bail;
          }
          // Write the ancillary data to the old msg structure
          tempRes = ds::Utils::Conversion::PutCMsg( &msg, cmsg_ptr,
                                                    (int16)OptLevel::QDS_LEVEL_IP,
                                                    (int16)OptName::QDS_IP_RECVIF,
                                                    sizeof(IfaceInfo),
                                                    (void*)&IfaceInfo,
                                                    &controllen);
          if ( AEE_SUCCESS != tempRes )
          {
            LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                            "Failed to put cmsg with err %d", tempRes );
          }

        }
        break;
      }
      default:
          LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                          "Unsupported Ancillary Data received :%d", AncID);
    }

    DS_UTILS_RELEASEIF(pOutAncData[i]);

    cmsg_ptr = DSS_CMSG_NXTHDR(&msg, cmsg_ptr);
  }

  if(controllen < msg.msg_controllen)
  {
    *msg_controlLenReq = (int)controllen;
  }
  else
  {
    *msg_controlLenReq = (int)msg.msg_controllen;
  }

bail:
  PS_SYSTEM_HEAP_MEM_FREE(pOutAncData);

  return res;
}

ds::ErrorType CDECL Socket::RecvMsg
(
  ds::SockAddrStorageType  remoteAddrPtr,
  IPort1::SeqBytes         ioVecArr[],
  int                      numIOVec,
  int *                    numReadPtr,
  IAncDataPriv **          outAncillaryDataPtrPtr,
  int                      outAncillaryDataLen,
  int *                    outAncillaryDataLenReqPtr,
  unsigned int *           outFlagsPtr,
  unsigned int             flags
)
{
  SockAddrInternalType     tempAddr = {{0}};
  ds::ErrorType            dsErrno;
  ds::AddrFamilyType      _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32                   readEventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Only MSG_ERRQUEUE is supported by RecvMSg
  -------------------------------------------------------------------------*/
  if (0 != ( flags & ~( RecvFlags::QDS_MSG_ERRQUEUE)))
  {
    LOG_MSG_INVALID_INPUT_2("Socket::RecvMsg(): "
                            "Invalid flags 0x%x, sock 0x%x", flags, this);
    return QDS_EOPNOTSUPP;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::RecvMsg(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_ENETDOWN;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return error if socket is blocked for reading
      1. Return EWOULDBLOCK if socket is flow controlled
      2. ASSERT if err mask is unknown
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_READ, &readEventIndex)){
    LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[readEventIndex])
  {
    if (0 != ( FLOW_CONTROL_EVENT_BIT_MASK & eventBitMask[readEventIndex]))
    {
      LOG_MSG_INFO1_2("Socket::RecvMsg(): "
                      "Read is blocked, sock 0x%x err mask 0x%x",
                      this, eventBitMask[readEventIndex]);
      dsErrno = AEE_EWOULDBLOCK;
      goto bail;
    }
    else
    {
      LOG_MSG_INFO1_2("Socket::RecvMsg(): "
                      "Unknown err mask 0x%x, sock 0x%x",
                      eventBitMask[readEventIndex], this);
      ASSERT( 0);
      dsErrno = AEE_EWOULDBLOCK;
      goto bail;
    }
  }

  if (0 == numReadPtr)
  {
    LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                    "NULL arg, sock 0x%x", this);

    dsErrno = QDS_EFAULT;

    goto bail;
  }

#if 0
  /*-------------------------------------------------------------------------
  In case dataLen is 0, should return SUCCESS
  -------------------------------------------------------------------------*/

  for (index = 0; index < numIOVec; index++)
  {
    if (0 != ioVecArr[index].dataLen)
    {
      bIsZeroLen = false;
      break;
    }
  }

  if (bIsZeroLen) {
    dsErrno = AEE_SUCCESS;
    *numReadPtr = 0;
    goto bail;
  }
#endif
  /*-------------------------------------------------------------------------
    Call platform specific RecvMsg(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->RecvMsg( (remoteAddrPtr) ? (&tempAddr) : 0,
                                      ioVecArr,
                                      numIOVec,
                                      reinterpret_cast <int32 *> ( numReadPtr),
                                      outAncillaryDataPtrPtr,
                                      outAncillaryDataLen,
                                      (int32 *) outAncillaryDataLenReqPtr,
                                      (uint32 *) outFlagsPtr,
                                      flags);

  if (0 != remoteAddrPtr)
  {
    memscpy((void *) remoteAddrPtr,
            sizeof(ds::SockAddrStorageType),
            (void *) &tempAddr,
            sizeof(ds::SockAddrInternalType));

    (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&_family);
    (void)ds::Sock::AddrUtils::SetFamily(remoteAddrPtr,ps_htons(_family));
  }

  critSectPtr->Leave();

  LOG_MSG_INFO4_3("Socket::RecvMsg(): "
                  "Sock 0x%x read %d err 0x%x", this, *numReadPtr, dsErrno);
  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::RecvMsg() */


ds::ErrorType CDECL Socket::AddIPMembership
(
  const IPMembershipInfoType *  ipMembershipPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::AddIPMembership() */


ds::ErrorType CDECL Socket::DropIPMembership
(
  const IPMembershipInfoType *  ipMembershipPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::DropIPMembership() */


ds::ErrorType CDECL Socket::GetSOLingerReset
(
  LingerType *  soLingerPtr
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::GetSOLingerReset(): "
                          " Sock 0x%x, platformSockPtr 0x%x",
                           this, platformSockPtr);
  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific GetSOLingerReset(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetSOLingerReset( soLingerPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GetSOLingerReset(): "
                    "GetSOLingerReset failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetSOLingerReset() */


ds::ErrorType CDECL Socket::SetSOLingerReset
(
  const LingerType *  soLingerPtr
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Socket::SetSOLingerReset(): "
                          " Sock 0x%x, platformSockPtr 0x%x",
                           this, platformSockPtr);

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call platform specific SetSOLingerReset(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SetSOLingerReset( soLingerPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::SetSOLingerReset(): "
                    "SetSOLingerReset failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::GetSOLingerReset() */


ds::ErrorType CDECL Socket::GetDoSAckInfo
(
  DoSAckStatusType *  dosAckStatusPtr,
  int              *  overflowPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* Socket::GetDoSAckInfo() */


ds::ErrorType CDECL Socket::GetSystemOption
(
  boolean *  isSystemSocketPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == isSystemSocketPtr)
  {
    LOG_MSG_INVALID_INPUT_0("Socket::GetSystemOption(): "
                            "NULL arg");
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  if (isSystemSocket)
  {
    *isSystemSocketPtr = TRUE;
  }
  else
  {
    *isSystemSocketPtr = FALSE;
  }

  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_2("Socket::GetSystemOption(): "
                          "Success, isSysSock %d sock 0x%x",
                          *isSystemSocketPtr, this);
  return AEE_SUCCESS;

} /* Socket::GetSystemOption() */


ds::ErrorType CDECL Socket::SetSystemOption
(
  boolean  _isSystemSocket
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::SetSystemOption(): "
                          " Sock 0x%x, platformSockPtr 0x%x, isSystemSocket %d ",
                          this, platformSockPtr, isSystemSocket);
  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (TRUE == _isSystemSocket)
  {
    isSystemSocket = true;
  }
  else
  {
    isSystemSocket = false;
  }

  /*-------------------------------------------------------------------------
    Call platform specific SetSystemOption(...) and return
  -------------------------------------------------------------------------*/
  platformSockPtr->SetSystemOption();

  critSectPtr->Leave();

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::SetSystemOption() */


void Socket::SetEvent
(
  Platform::EventType  event
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Socket::SetEvent(): "
                  "Ev %d, sock 0x%x", event, this);

  /*-------------------------------------------------------------------------
    Make sure that the object is not deleted.
  -------------------------------------------------------------------------*/
  if (NULL == critSectPtr || 0 == refCnt)
  {
    LOG_MSG_INFO1_1 ("Socket::SetEvent(): "
                     "Obj %p already deleted, return", this);
    return;
  }

  /*-------------------------------------------------------------------------
    Increment refCnt. Otherwise, if app calls Release() once signal is posted,
    refCnt could become 0 and socket object would be destroyed. This becomes
    an issue since object could be accessed by sockets library even after
    signal is posted to clean up state. So increment refCnt before posting an
    event and release after event is posted
  -------------------------------------------------------------------------*/
  (void) AddRef();

  switch (event)
  {
    case Platform::Event::PLATFORM_ENABLED:
    {
      ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32) SocketEvent::WRITE_BIT_MASK_SOCKET_PLATFORM_DISABLED);
      break;
    }

    case Platform::Event::WRITE:
    {
      ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32)SocketEvent::WRITE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::READ:
    {
      ResetEventBitMask( SocketEvent::QDS_EV_READ,
                         (uint32)SocketEvent::READ_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::ACCEPT:
    {
      ResetEventBitMask(SocketEvent::QDS_EV_ACCEPT,
                        (uint32)SocketEvent::ACCEPT_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::CLOSE:
    {
      ResetEventBitMask( SocketEvent::QDS_EV_CLOSE,
                         (uint32)SocketEvent::CLOSE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::SetEvent(): "
                      "Unknown ev %d", event);
      ASSERT( 0);
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Decrement refCnt since event has been posted and since the control is
    back to sockets library
  -------------------------------------------------------------------------*/
  (void) Release();
  return;

} /* Socket::SetEvent() */

void Socket::ClearEvent
(
  Platform::EventType  event
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Socket::ClearEvent(): "
                  "Ev %d, sock 0x%x", event, this);

  /*-------------------------------------------------------------------------
    Make sure that the object is not deleted.
  -------------------------------------------------------------------------*/
  if (NULL == critSectPtr || 0 == refCnt)
  {
    LOG_MSG_INFO1_1 ("Socket::ClearEvent(): "
                     "Obj %p already deleted, return", this);
    return;
  }

  /*-------------------------------------------------------------------------
    Increment refCnt. Otherwise, if app calls Release() once signal is posted,
    refCnt could become 0 and socket object would be destroyed. This becomes
    an issue since object could be accessed by sockets library even after
    signal is posted to clean up state. So increment refCnt before posting an
    event and release after event is posted
  -------------------------------------------------------------------------*/
  (void) AddRef();

  switch (event)
  {
    case Platform::Event::PLATFORM_ENABLED:
    {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_SOCKET_PLATFORM_DISABLED);
      break;
    }

    case Platform::Event::WRITE:
    {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::READ:
    {
      SetEventBitMask( SocketEvent::QDS_EV_READ,
                       (uint32) SocketEvent::READ_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::ACCEPT:
    {
      SetEventBitMask( SocketEvent::QDS_EV_ACCEPT,
                       (uint32) SocketEvent::ACCEPT_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    case Platform::Event::CLOSE:
    {
      SetEventBitMask( SocketEvent::QDS_EV_CLOSE,
                         (uint32)SocketEvent::CLOSE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::ClearEvent(): "
                      "Unknown ev %d", event);
      ASSERT( 0);
      break;
    }
  }

  /*-------------------------------------------------------------------------
    Decrement refCnt since event has been posted and since the control is
    back to sockets library
  -------------------------------------------------------------------------*/
  (void) Release();
  return;

} /* Socket::ClearEvent() */


boolean Socket::Process
(
  void *  userDataPtr
)
{
  Event::EventInfo *          eventInfoPtr;
  Event::DoSAckEventInfo *    dosAckEventInfoPtr;
  Event::IPFilterEventInfo *  ipFilterEventInfoPtr;
  void *                      routingCachePtr;
  int32                       routingCache;
  int32                       defaultPSFlowHandle;
  int32                       psFlowHandle;
  int32                       physLinkHandle;
  int32                       origPhysLinkHandle;
  int32                       primaryPhysLinkHandle;
  uint32                      reqIfaceState;
  phys_link_state_type        physLinkState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == userDataPtr)
  {
    LOG_MSG_ERROR_1("Socket::Process(): NULL arg sock 0x%x", this);
    ASSERT( 0);
    return FALSE;
  }

  eventInfoPtr = reinterpret_cast <Event::EventInfo *> ( userDataPtr);

#if 0
  LOG_MSG_INFO1_3("Socket::Process(): "
                  "Processing ev %d from group %d, sock 0x%x",
                  eventInfoPtr->eventMask, eventInfoPtr->eventGroup, this);
#endif

  if (TRUE == GetStrongRef())
  {
    critSectPtr->Enter();

    switch (eventInfoPtr->eventGroup)
    {
      case Event::EVENT_GROUP_NETWORK:
      {
        /*-------------------------------------------------------------------
          All Socket APIs return failure if platformSockPtr is 0. So, skip
          event processing in that case, as it is not going to help with any
          Socket APIs

          Additional Note: CR 695629
        -------------------------------------------------------------------*/
        if (0 == platformSockPtr)
        {
          break;
        }

        if ( (int32) IFACE_CONFIGURING_EV == eventInfoPtr->eventMask ||
             (int32) IFACE_UP_EV == eventInfoPtr->eventMask ||
             (int32) IFACE_ROUTEABLE_EV == eventInfoPtr->eventMask)
        {
          UpdateRouteScopeByPolicy();
        }

        /*-------------------------------------------------------------------
          Each socket must process only those NETWORK events which are raised
          on iface/flow/phys link to which socket is bound to. So, if
          rtMetaInfoPtr is 0, then socket is not bound to any
          iface/flow/phys link, and hence event processing can be skipped
        -------------------------------------------------------------------*/
        if (0 == rtMetaInfoPtr)
        {
          break;
        }

        switch (eventInfoPtr->eventMask)
        {
          case IFACE_CONFIGURING_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              if (isSystemSocket)
              {
                reqIfaceState = (uint32) IFACE_CONFIGURING |
                                (uint32) IFACE_UP;
                if (0 != ( reqIfaceState &
                           (uint32) NetPlatform::GetIfaceState( routingCache)) &&
                    SocketIPSec::IsIfaceListInReqState( rtMetaInfoPtr,
                                                        reqIfaceState))
                {
                  ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                     (uint32)SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE);
                }
              }
              else
              {
                SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32)SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE);
              }
            }

            break;
          }

          case IFACE_UP_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              reqIfaceState = (uint32) IFACE_UP;
              if (isSystemSocket)
              {
                reqIfaceState |= (uint32) IFACE_CONFIGURING;
              }

              if (0 != ( reqIfaceState &
                         (uint32) NetPlatform::GetIfaceState( routingCache)) &&
                  SocketIPSec::IsIfaceListInReqState( rtMetaInfoPtr,
                                                      reqIfaceState))
              {
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE);
              }
            }

            break;
          }

          case IFACE_ROUTEABLE_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache)
            {
              ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32)SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE);
            }

            break;
          }

          case IFACE_GOING_DOWN_EV:
          case IFACE_LINGERING_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              if (false == isSystemSocket)
              {
                SocketIPSec::ClearIfaceList( rtMetaInfoPtr);
                ProcessNetworkConfigChangedEvent( QDS_ENETDOWN);
              }
            }

            break;
          }

          case IFACE_DOWN_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              DeleteRoutingCacheFromRouteScope( routingCache);

              SocketIPSec::ClearIfaceList( rtMetaInfoPtr);
              ProcessNetworkConfigChangedEvent( QDS_ENETDOWN);
            }

            break;
          }

          case IFACE_ADDR_CHANGED_EV:
          case IFACE_ADDR_FAMILY_CHANGED_EV:
          case IFACE_PREFIX_UPDATE_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              ProcessNetworkConfigChangedEvent( QDS_EIPADDRCHANGED);
            }

            break;
          }

          case IFACE_IPFLTR_UPDATED_EV:
          {

            ipFilterEventInfoPtr =
              reinterpret_cast <Event::IPFilterEventInfo *> ( userDataPtr);

            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            /*---------------------------------------------------------------
              Skip event processing unless QOS_OUTPUT filters are updated on
                   routing cache
            ---------------------------------------------------------------*/
            if (IP_FLTR_CLIENT_QOS_OUTPUT != ipFilterEventInfoPtr->clientID ||
                ipFilterEventInfoPtr->handle != routingCache)
            {
              LOG_MSG_INFO2_2("Socket::Process(): Ignoring "
                              "IFACE_IPFLTR_UPDATED_EV sock 0x%x, client id %d",
                              this, ipFilterEventInfoPtr->clientID);
              break;
            }

            /*---------------------------------------------------------------
              Reset filter result in metainfo and update platform socket so
              that any data originating from TCP/IP stack would get filtered at
              PS_IFACE layer (CR 688872)
            ---------------------------------------------------------------*/
            PS_RT_META_RESET_FILTER_RESULT(rtMetaInfoPtr,
                                           IP_FLTR_CLIENT_QOS_OUTPUT);
            (void) platformSockPtr->SetRtMetaInfo( rtMetaInfoPtr);
            ResetRoutingCache(true);

            break;
          }

          case IFACE_FLOW_DISABLED_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                               (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
            }

            break;
          }

          case IFACE_FLOW_ENABLED_EV:
          {
            routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
            routingCache    = reinterpret_cast <int32> ( routingCachePtr);

            if (eventInfoPtr->handle == routingCache ||
                SocketIPSec::IsHandleInIfaceList( eventInfoPtr->handle,
                                                  rtMetaInfoPtr))
            {
              if (IsFlowEnabled())
              {
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
              }
            }

            break;
          }

          case FLOW_ACTIVATED_EV:
          {
            psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == psFlowHandle)
            {
              ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32)SocketEvent::WRITE_BIT_MASK_PS_FLOW_SUSPENDED);
            }
            else if (eventInfoPtr->handle == origFltrResult)
            {
              if (AEE_SUCCESS != UseOrigFltrResult())
              {
                LOG_MSG_INFO3_1("Socket::Process(): "
                                "UseOrigFltrResult failed, sock 0x%x", this);
                break;
              }
            }

            break;
          }

          case FLOW_SUSPENDED_EV:
          {
            psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == psFlowHandle)
            {
              if (AEE_SUCCESS != UpdateRtMetaInfoWithFlowFwding())
              {
                LOG_MSG_INFO3_1("Socket::Process(): "
                                "UpdateRtMetaInfoWithFlowFwding failed, "
                                "sock 0x%x", this);
                break;
              }
            }

            break;
          }

          case FLOW_TX_DISABLED_EV:
          {
            psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == psFlowHandle)
            {
              if (FLOW_ACTIVATED != NetPlatform::GetPSFlowState( psFlowHandle))
              {
                if (AEE_SUCCESS != UpdateRtMetaInfoWithFlowFwding())
                {
                  LOG_MSG_INFO3_1("Socket::Process(): "
                                  "UpdateRtMetaInfoWithFlowFwding failed, "
                                  "sock 0x%x", this);
                  break;
                }
              }
              else
              {
                SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
              }
            }

            break;
          }

          case FLOW_TX_ENABLED_EV:
          {
            psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == psFlowHandle)
            {
              if (IsFlowEnabled())
              {
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED);
              }
            }

            break;
          }

          case PHYS_LINK_UP_EV:
          {
            physLinkHandle =
              NetPlatform::GetPhysLinkFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == physLinkHandle)
            {
              ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32)SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
            }
            else
            {
              (void) NetPlatform::PSGetPhysLinkFromFlow( origFltrResult,
                                                         &origPhysLinkHandle);
              if (eventInfoPtr->handle == origPhysLinkHandle)
              {
                if (AEE_SUCCESS != UseOrigFltrResult())
                {
                  LOG_MSG_INFO3_1("Socket::Process(): "
                                  "UseOrigFltrResult failed, sock 0x%x", this);
                  break;
                }
              }
            }

            break;
          }

          case PHYS_LINK_DOWN_EV:
          {
            physLinkHandle =
              NetPlatform::GetPhysLinkFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == physLinkHandle)
            {
              psFlowHandle =
                NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

              defaultPSFlowHandle =
                NetPlatform::GetDefaultPSFlowFromRtMetaInfo( rtMetaInfoPtr);
              (void) NetPlatform::PSGetPhysLinkFromFlow( defaultPSFlowHandle,
                                                         &primaryPhysLinkHandle);

              if (psFlowHandle != defaultPSFlowHandle)
              {
                if (eventInfoPtr->handle != primaryPhysLinkHandle)
                {
                  if (AEE_SUCCESS != UpdateRtMetaInfoWithFlowFwding())
                  {
                    LOG_MSG_INFO3_1("Socket::Process(): "
                                    "UpdateRtMetaInfoWithFlowFwding failed, "
                                    "sock 0x%x", this);
                    break;
                  }
                }
                else
                {
                  LOG_MSG_INFO1_1("Socket::Process(): "
                                  "Not fwding data to default flow as event is "
                                  "received on primary phys link, sock 0x%x",
                                  this);
                }
              }
              else
              {
                if (0 == origFltrResult && 0 != platformSockPtr &&
                    false == platformSockPtr->IsConnInProgress())
                {
                  SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
                }
              }
            }

            break;
          }

          case PHYS_LINK_FLOW_DISABLED_EV:
          {
            physLinkHandle =
              NetPlatform::GetPhysLinkFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == physLinkHandle)
            {
              (void) NetPlatform::PhysLinkIoctl
                     (
                       physLinkHandle,
                       NetPlatform::PHYS_LINK_IOCTL_GET_STATE,
                       (void *) &physLinkState
                     );

              psFlowHandle =
                NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

              defaultPSFlowHandle =
                NetPlatform::GetDefaultPSFlowFromRtMetaInfo( rtMetaInfoPtr);
              (void) NetPlatform::PSGetPhysLinkFromFlow( defaultPSFlowHandle,
                                                         &primaryPhysLinkHandle);

              if (psFlowHandle != defaultPSFlowHandle &&
                  PHYS_LINK_UP != physLinkState)
              {
                if (eventInfoPtr->handle != primaryPhysLinkHandle)
                {
                  if (AEE_SUCCESS != UpdateRtMetaInfoWithFlowFwding())
                  {
                    LOG_MSG_INFO3_1("Socket::Process(): "
                                    "UpdateRtMetaInfoWithFlowFwding failed, "
                                    "sock 0x%x", this);
                    break;
                  }
                }
                else
                {
                  LOG_MSG_INFO1_1("Socket::Process(): "
                                  "Not fwding data to default flow as event is "
                                  "received on primary phys link, sock 0x%x",
                                  this);
                }
              }
              else
              {
                SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                 (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
              }
            }

            break;
          }

          case PHYS_LINK_FLOW_ENABLED_EV:
          {
            physLinkHandle =
              NetPlatform::GetPhysLinkFromRtMetaInfo( rtMetaInfoPtr);

            if (eventInfoPtr->handle == physLinkHandle)
            {
              if (IsFlowEnabled())
              {
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
                ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                                   (uint32)SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED);
              }
            }

            break;
          }

          case PHYS_LINK_707_DOS_ACK_EV:
          {
            dosAckEventInfoPtr =
              reinterpret_cast <Event::DoSAckEventInfo *> ( userDataPtr);

            if (dosAckEventInfoPtr->handle == reinterpret_cast <int32> ( this))
            {
              ProcessDoSAckEvent( dosAckEventInfoPtr);
            }

            break;
          }
        }

        break;
      }

      case Event::EVENT_GROUP_PS_MEM:
      {
        ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                           (uint32)SocketEvent::WRITE_BIT_MASK_PS_MEM_BUF_NOT_AVAILABLE);
        break;
      }

      default:
      {
        LOG_MSG_ERROR_2("Socket::Process(): "
                        "Unknown ev group %d, sock 0x%x",
                        eventInfoPtr->eventGroup, this);
          break;
      }

    } /* switch (eventInfoPtr->eventGroup)  */

    critSectPtr->Leave();

    (void) Release();

  } /* if (TRUE == GetStrongRef()) */

  return TRUE;
} /* Socket::Process() */


ds::ErrorType Socket::QueryInterface
(
  AEEIID   iid,
  void **  objPtrPtr
)
throw()
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_3("Socket::QueryInterface(): "
                          "Sock 0x%x, iid 0x%x, objPtrPtr 0x%x",
                           this, iid, objPtrPtr);

  if (0 == objPtrPtr)
  {
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  switch (iid)
  {
    case AEEIID_ISocket:
    {
      *objPtrPtr = static_cast <ISocket *> ( this);
      break;
    }

    case AEEIID_ISocketLocalPriv:
    {
      *objPtrPtr = static_cast <ISocketLocalPriv *> ( this);
      break;
    }

    case AEEIID_ISocketExt:
    {
      *objPtrPtr = static_cast <ISocketExt *> ( this);
      break;
    }

    case ds::AEEIID_IDSMUtils:
    {
      *objPtrPtr = static_cast <ds::IDSMUtils *> ( this);
      break;
    }

    case AEEIID_IQI:
    {
      //TODO Fix this. fails if static_cast is used coz IQI is present from
      //multiple base interfaces
      *objPtrPtr = reinterpret_cast <IQI *> ( this);
      break;
    }

    default:
    {
      dsErrno = AEE_ECLASSNOTSUPPORT;
      goto bail;
    }
  } /* switch (iid) */

  (void) AddRef();
  critSectPtr->Leave();

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* Socket::QueryInterface() */


/*===========================================================================

                         PROTECTED MEMBER FUNCTIONS

===========================================================================*/
Socket::Socket
(
  void
) :
    critSectPtr( 0),
    platformSockPtr( 0),
    rtMetaInfoPtr( 0),
    sendPktMetaInfo( false),
    networkCtlPtr(0),
    ifaceUpSignalCtlPtr(0),
    ifaceDownSignalCtlPtr(0),
    origFltrResult( 0)
{
  uint32  idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set members to initial values. CritSect will be created
    in Init() because their creation can fail.
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < SocketEvent::NUM_EV; idx++)
  {
    eventBitMask[idx] = 0;
  }

  LOG_MSG_FUNCTION_EXIT_1("Socket::QueryInterface(): "
                          "Created sock 0x%p", this);
  return;

} /* Socket::Socket() */

ds::ErrorType Socket::AllocateMemberObjects
(
  void
)
{
  uint32                       idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL,
                                              AEECLSID_CCritSect,
                                              (void **) &critSectPtr))
  {
    LOG_MSG_ERROR_0 ("Socket::AllocateMemberObjects(): "
                     "Cannot create crit sect");
    return AEE_ENOMEMORY;
  }

  for (idx = 0; idx < SocketEvent::NUM_EV; idx++)
  {
    (void) eventSignalBuses[idx].Init();
  }

  return AEE_SUCCESS;

} /* Socket::AllocateMemberObjects() */

ds::ErrorType Socket::Init
(
  ds::AddrFamilyType  _family,
  SocketType          sockType,
  ProtocolType        protocol,
  NetworkCtl          *_networkCtlPtr
)
{
  Platform::ISocketFactory *  platformSockFactoryPtr;
  Platform::IEventListener *  eventListenerPtr;
  ps_route_scope_type         routeScope;
  ds::ErrorType               dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_4("Socket::Init(): "
                           "Initing sock 0x%x, _family %d, type %d, proto %d",
                           this, _family, sockType, protocol);

  /*-------------------------------------------------------------------------
    There is no need to validate (family, sockType, protocol) as they
    are already validated by SocketFactory::CreateSocket(...) and that function
    is the only entry point to create a socket
  -------------------------------------------------------------------------*/
  dsErrno = AllocateMemberObjects();
  if (AEE_SUCCESS != dsErrno)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Set Network Controller
  -------------------------------------------------------------------------*/
  ASSERT(0 != _networkCtlPtr);

  networkCtlPtr = _networkCtlPtr;
  (void)networkCtlPtr->AddRef();
  dsErrno = networkCtlPtr->Init(critSectPtr);
  if(AEE_SUCCESS != dsErrno)
  {
    DS_UTILS_RELEASEIF(networkCtlPtr);
    goto bail;
  }

  if(TRUE == networkCtlPtr->IsNetworkBound())
  {
    /*-------------------------------------------------------------------------
    Register with network controller object for state change events.
    This event is used to take appropriate action as network state changes.
    For example, connected TCP sockets are closed once
    network object goes DOWN
    -------------------------------------------------------------------------*/
    dsErrno = RegNetworkStateChangedEvent();
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::Init(): "
                      "RegNetworkStateChangedEvent failed, sock 0x%x err 0x%x",
                      this, dsErrno);
    }
  }

  /*-------------------------------------------------------------------------
    Create a Socket at platform layer and store the handle

    Cast to IEventListener before calling CreateSocket(). Otherwise, since
    platform gets an object whose vtable is pointing to Socket class's methods,
    when it invokes methods of IEventListener, methods of Socket object will
    be invoked.
  -------------------------------------------------------------------------*/
  platformSockFactoryPtr = Platform::ISocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);

  platformSockPtr = platformSockFactoryPtr->CreateSocket( _family,
                                                          sockType,
                                                          protocol,
                                                          critSectPtr,
                                                          &dsErrno);
  if (0 == platformSockPtr)
  {
    goto bail;
  }

  DS_UTILS_RELEASEIF(platformSockFactoryPtr);

  /*-------------------------------------------------------------------------
    Register an event listener with platform socket
  -------------------------------------------------------------------------*/
  eventListenerPtr = static_cast <Platform::IEventListener *> ( this);
  platformSockPtr->RegEventListener( eventListenerPtr);

  /*-------------------------------------------------------------------------
    Generate route scope and set it in platform
  -------------------------------------------------------------------------*/
  dsErrno = NetPlatform::GetRouteScopeByPolicy( 0, &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "GetRouteScopeByPolicy failed, sock 0x%x err 0x%x",
                     this, dsErrno);
    goto bail;
  }

  dsErrno = platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "SetRouteScope failed, sock 0x%x err 0x%x",
                     this, dsErrno);
    goto bail;
  }

  family = _family;

  /*-------------------------------------------------------------------------
    Since routing hasn't been performed yet, set
    WRITE_BIT_MASK_NULL_ROUTING_INFO
  -------------------------------------------------------------------------*/
  origFltrResult = 0;
  SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                   (uint32)SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO);

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  return dsErrno;

} /* Socket::Init() */

void Socket::Destructor
(
  void
)
throw()
{
  SocketFactory *  sockFactoryPtr;
  LingerType       soLinger;
  ds::ErrorType    dsErrno;
  uint32           idx;
  ::ICritSect *    localCritSectPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("Socket::Destructor(): "
                           "Sock 0x%x", this);

  /*lint -save -e1550, -e1551 */

  if (0 == critSectPtr)
  {
    return;
  }

  localCritSectPtr = critSectPtr;
  localCritSectPtr->Enter();

  origFltrResult = 0;

  /*-------------------------------------------------------------------------
    Clean up all the resources which are still held by the socket

    Release() is called on the resources which are cached and delete is used
    on resources which are locally allocated and not shared with anybody else
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Close platform socket if it hasn't been closed already. Also set
    SO_LINGER_RESET option so that socket is closed immediately as application
    is not waiting for socket to close gracefully
  -------------------------------------------------------------------------*/
  if (0 != platformSockPtr)
  {
    soLinger.lingerEnabled = TRUE;
    soLinger.timeInSec  = 0;

    dsErrno = platformSockPtr->SetSOLingerReset( &soLinger);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::Destructor(): "
                      "SetSOLingerReset failed, sock 0x%x err 0x%x",
                      this, dsErrno);
    }

    (void) Socket::Close();

    platformSockPtr = 0;
  }

  DS_UTILS_RELEASEIF(networkCtlPtr);

  DetachNetworkStateChangeSignalCtls();

  for (idx = 0; idx < SocketEvent::NUM_EV; idx++)
  {
    eventSignalBuses[idx].Release();
  }

  if (0 != rtMetaInfoPtr)
  {
    PS_RT_META_INFO_FREE( &rtMetaInfoPtr);            /*lint !e1550 !e1551 */
  }

  /*-------------------------------------------------------------------------
    Delete socket from socket factory's list
  -------------------------------------------------------------------------*/
  sockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != sockFactoryPtr);

  (void) sockFactoryPtr->DeleteSocket( this);
  DS_UTILS_RELEASEIF(sockFactoryPtr);

  /*-------------------------------------------------------------------------
    Perform reset of object's critSectPtr within critical section.
  -------------------------------------------------------------------------*/
  critSectPtr = 0;

  localCritSectPtr->Leave();
  DS_UTILS_RELEASEIF(localCritSectPtr);

  /*lint -restore */

  SignalHandlerBase::Destructor();

} /* Socket::~Destructor() */


ds::ErrorType Socket::CloneSocket
(
  Socket *  sockPtr
)
{
  ds::ErrorType  dsErrno;
  uint32         idx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == sockPtr)
  {
    DATA_ERR_FATAL("Socket::CloneSocket(): NULL sock");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Set critical section of cloned socket to be the same as the current socket
  -------------------------------------------------------------------------*/
  sockPtr->critSectPtr->AddRef();
  critSectPtr = sockPtr->critSectPtr;

  /*-------------------------------------------------------------------------
    Copy event bit masks form sockPtr in to socket object. Event registrations
    are not copied though
  -------------------------------------------------------------------------*/
  for (idx = 0; idx < SocketEvent::NUM_EV; idx++)
  {
    eventBitMask[idx]      = sockPtr->eventBitMask[idx];
    (void) eventSignalBuses[idx].Init();
  }

  /*-------------------------------------------------------------------------
    Copy policy and network objects. Since objects are cached in yet another
    socket object, increment the refCnt
  -------------------------------------------------------------------------*/
  dsErrno = (sockPtr->networkCtlPtr)->Clone(
                                            critSectPtr,
                                            &networkCtlPtr
                                          );
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::CloneSocket(): "
                    "Clone NetCtl failed, sock 0x%x err 0x%x", this, dsErrno);
    return dsErrno;
  }

  if(TRUE == networkCtlPtr->IsNetworkBound())
  {
    dsErrno = RegNetworkStateChangedEvent();
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::CloneSocket(): "
                      "RegNetworkStateChangedEvent failed, sock 0x%x err 0x%x",
                      this, dsErrno);
      return dsErrno;
    }
  }

  //TODO Check why metainfo can't be copied
  rtMetaInfoPtr = 0;

  family         = sockPtr->family;
  isSystemSocket = sockPtr->isSystemSocket;
  LOG_MSG_FUNCTION_EXIT_1("Socket::CloneSocket(): "
                          "Success, sock 0x%x", this);
  return AEE_SUCCESS;

} /* Socket::CloneSocket() */


bool Socket::IsOptPrivSupported
(
  OptLevelType    optLevel,
  OptNameTypePriv optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return false;
} /* Socket::IsOptPrivSupported() */


bool Socket::IsMulticastSupported
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return false;
} /* Socket::IsMulticastSupported() */


bool Socket::IsSetNetPolicySupported
(
  void
)
{
  SockAddrInternalType   remoteSockAddr;
  SockAddrIN6InternalType  v6RemoteSockAddr;
  ds::ErrorType    dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Socket's policy can't be updated if
      1. Socket is created via
         a. SocketFactory::CreateSocketByNetwork() OR
         b. SocketFactory::CreateSocket() OR
      2. Socket is created via Accept() OR
      3. Connect() had been called OR
      4. Listen() had been called OR
      5. Close() had been called

    In cases 1 & 2, socket's peer address is not ps_in6addr_any.
    //TODO Cases 3 & 4 are not supported as ENOTCONN are returned and we
    //return true today
    //TODO Update doc
    //Seems totally broken. GetPeerName returns ENOTCONN in OPENING state
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Check if Network Controller supports Policy Update
  -------------------------------------------------------------------------*/
  if (TRUE == networkCtlPtr->IsPolicyUpdateEnabled())
  {
    LOG_MSG_INFO1_1("Socket::IsSetNetPolicySupported(): "
                    "Socket's 0x%x policy can't be updated", this);
    return false;
  }

  /*-------------------------------------------------------------------------
    Fetch socket's peer address
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    /*-----------------------------------------------------------------------
      Since GetPeerName() returns QDS_ENOTCONN if socket is not connected,
      return true
    -----------------------------------------------------------------------*/
    if (QDS_ENOTCONN == dsErrno)
    {
      return true;
    }
    else
    {
      LOG_MSG_INFO3_2("Socket::IsSetNetPolicySupported(): "
                      "GetPeerName failed, sock 0x%x err ox%x", this, dsErrno);
      return false;
    }
  }

  dsErrno = AddrUtils::GetSockAddrIN6( &remoteSockAddr, &v6RemoteSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::IsSetNetPolicySupported(): "
                    "GetSockAddrIN6 failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    return false;
  }

  //TODO what is the point of this check?
  return PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr, &ps_in6addr_any);

} /* Socket::IsSetNetPolicySupported() */


void Socket::SetEventBitMask
(
  ds::Sock::SocketEventType  event,
  uint32                     bitMask
)
throw()
{
  uint32  eventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (AEE_SUCCESS != GetIndexFromEvent( event, &eventIndex))
  {
    LOG_MSG_INFO3_1("Socket::SetEventBitMask(): "
                    "GetIndexFromEvent failed, sock 0x%x", this);
    return;
  }

  critSectPtr->Enter();

  eventBitMask[eventIndex] |= bitMask;

  LOG_MSG_INFO1_3( "SetEventBitMask :: Sock 0x%x ev %d ev mask 0x%x",
                   this, event, eventBitMask[eventIndex]);

  SetOrClearEvent( event);

  critSectPtr->Leave();

  return;

} /* Socket::SetEventBitMask() */


void Socket::ResetEventBitMask
(
  ds::Sock::SocketEventType  event,
  uint32                     bitMask
)
throw()
{
  uint32  eventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (AEE_SUCCESS != GetIndexFromEvent( event, &eventIndex))
  {
    LOG_MSG_INFO3_1("Socket::ResetEventBitMask(): "
                    "GetIndexFromEvent failed, sock 0x%x", this);
    return;
  }

  critSectPtr->Enter();

  eventBitMask[eventIndex] &= ~bitMask;

  LOG_MSG_INFO1_3( "ResetEventBitMask :: Sock 0x%x ev %d ev mask 0x%x",
                   this, event, eventBitMask[eventIndex]);

  SetOrClearEvent( event);

  critSectPtr->Leave();

  return;

} /* Socket::ResetEventBitMask() */


void Socket::SetOrClearEvent
(
  ds::Sock::SocketEventType  event
)
throw()
{
  uint32  eventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Socket::SetOrClearEvent(): "
                           "Sock 0x%x ev %d", this, event);

  if (AEE_SUCCESS != GetIndexFromEvent( event, &eventIndex))
  {
    LOG_MSG_INFO3_1("Socket::SetOrClearEvent(): "
                    "GetIndexFromEvent failed, sock 0x%x", this);
    return;
  }

  /*-------------------------------------------------------------------------
    An event is TRUE if
      1. Any of transient error bits are set OR
      2. None of flow control event bits are set
    Otherwise it is FALSE.

    Explanation:
    1. Any of transient error bits are set:
       These error bits indicate that application has to take action.
       For ex, if NULL routing info bit mask is set, application should try
       to write data for DSSOCK to perform routing. Hence WRITE_EV is TRUE.
    2. None of flow control event bits are set
       These bits indicate that underlying layers are flow-controlled.
       For ex, if underlying flow is disabled, we cannot send out data until
       this error gets cleared. So WRITE_EV is FALSE. When all of the flow
       control bit masks are cleared, WRITE_EV becomes TRUE. If flow control
       event bits are set AND also transient error bits are set, then
       the event is true indicating that action is still required from
       application.

    Scenarios:
    +----------------------------+------------------------+-------------+
    |FLOW_CONTROL_EVENT_BIT_MASK | TRANSIENT_ERR_BIT_MASK | Event state |
    +----------------------------+------------------------+-------------+
    |           SET              |         SET            |    TRUE     |
    |           SET              |        CLEAR           |    FALSE    |
    |          CLEAR             |         SET            |    TRUE     |
    |          CLEAR             |        CLEAR           |    TRUE     |
    +----------------------------+------------------------+-------------+

    Note:
    1. The above explanation is TRUE for all sockets events even though
       the example just mentions WRITE_EV.
    2. Setting of SignalBus in SET state is a NO-OP, similarly Clearing
       of SignalBus in CLEAR state is a NO-OP. No additional action is
       required for checking if the bus is set/cleared.
  -------------------------------------------------------------------------*/
  if ( 0 != (TRANSIENT_ERR_BIT_MASK & eventBitMask[eventIndex]) ||
       0 == (FLOW_CONTROL_EVENT_BIT_MASK & eventBitMask[eventIndex])
     )
  {
    /*-----------------------------------------------------------------------
      QDS_EV_DOS_ACK is not "one shot" event so we use Strobe for it
    -----------------------------------------------------------------------*/
    if(SocketEvent::QDS_EV_DOS_ACK == event)
    {
      (void) eventSignalBuses[eventIndex].Strobe();
    }
    else
    {
      (void) eventSignalBuses[eventIndex].Set();
    }

    LOG_MSG_INFO1_2("Socket::SetOrClearEvent(): "
                    "sock 0x%x event %d set", this, event);
  }
  else
  {
    (void) eventSignalBuses[eventIndex].Clear();
    LOG_MSG_INFO1_2("Socket::SetOrClearEvent(): "
                    "sock 0x%x event %d cleared", this, event);

  }

  LOG_MSG_FUNCTION_EXIT_1("Socket::SetOrClearEvent(): "
                          "Success, sock 0x%x", this);
  return;

} /* Socket::SetOrClearEvent() */

void Socket::ResetRoutingCache
(
  bool freeRtMetaInfo
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Socket::ResetRoutingCache(): "
                  "Resetting routing cache for sock 0x%p freeMetaInfo %d",
                  this, freeRtMetaInfo);

  /*-------------------------------------------------------------------------
    Free routing cache
  -------------------------------------------------------------------------*/
  if (true == freeRtMetaInfo)
  {
    PS_RT_META_INFO_FREE( &rtMetaInfoPtr);
    rtMetaInfoPtr  = 0;
  }

  origFltrResult = 0;

  /*-------------------------------------------------------------------------
    Reset all routing related event bit masks
  -------------------------------------------------------------------------*/
  ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                     SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE |
                     SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED |
                     SocketEvent::WRITE_BIT_MASK_PS_FLOW_SUSPENDED |
                     SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED |
                     SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);

  SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                   SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO);
  return;
} /* Socket::ResetRoutingCache() */


/*===========================================================================

                         PRIVATE MEMBER FUNCTIONS

===========================================================================*/
ds::ErrorType Socket::HandleNonZeroWriteEventBitMask
(
  const ds::SockAddrInternalType*   remoteAddrPtr,
  boolean *                         isReorig,
  byte *                            data_buf,
  int                               data_len,
  bool                              isDoS
)
{
  ds::ErrorType        dsErrno = AEE_SUCCESS;
  int32                physLinkHandle;
  uint32               writeEventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE,
                                       &writeEventIndex))
  {
    LOG_MSG_INFO3_1("Socket::HandleNonZeroWriteEventBitMask(): "
                    "GetIndexFromEvent failed, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  LOG_MSG_FUNCTION_ENTRY_3("Socket::HandleNonZeroWriteEventBitMask(): "
                           "Sock 0x%x ev mask 0x%x isDoS %d",
                           this, eventBitMask[writeEventIndex], isDoS);

  /*-----------------------------------------------------------------------
    Initializing to an error so that if a new transient error is added to
    the list and if it is not handled in the function, a legal value is
    returned
  -----------------------------------------------------------------------*/
  dsErrno = QDS_EINVAL;

  /*-----------------------------------------------------------------------
    Generate routing info if it is NULL
  -----------------------------------------------------------------------*/
  if (0 != ((uint32) SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO &
            eventBitMask[writeEventIndex]))
  {
    dsErrno = RoutePacket( remoteAddrPtr, isReorig, data_buf, data_len, isDoS);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_1("Socket::HandleNonZeroWriteEventBitMask(): "
                      "Routing failed, sock 0x%x", this);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
   If flow control events are set, return EWOULDBLOCK, we need not check
   for traffic channel dormant state and bring up the channel.
  -------------------------------------------------------------------------*/
  if (0 != ( FLOW_CONTROL_EVENT_BIT_MASK & eventBitMask[writeEventIndex]))
  {
    dsErrno = AEE_EWOULDBLOCK;
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Bring up the traffic channel if it is dormant. It is important to
    check for this condition independently of NULL_ROUTING_INFO because,
    a dormant traffic channel could be part of new routing info
  -----------------------------------------------------------------------*/
  if ((0 != ( (uint32)SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL &
             eventBitMask[writeEventIndex])) && isDoS == false)
  {
    physLinkHandle =
      NetPlatform::GetPhysLinkFromRtMetaInfo( rtMetaInfoPtr);

    /*---------------------------------------------------------------------
      This could happen if context switch happened and if iface went DOWN
    ---------------------------------------------------------------------*/
    if (0 == physLinkHandle)
    {
      LOG_MSG_INFO3_1("Socket::HandleNonZeroWriteEventBitMask(): "
                      "GetPhysLinkFromRtMetaInfo returned 0, sock 0x%x ",
                      this);
      dsErrno = AEE_EWOULDBLOCK;

      ResetRoutingCache(true);
      goto bail;
    }

    dsErrno = NetPlatform::PhysLinkUpCmd( physLinkHandle, 0);

    if (AEE_SUCCESS != dsErrno && AEE_EWOULDBLOCK != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::HandleNonZeroWriteEventBitMask(): "
                      "PhysLinkBringUpCmd failed, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }
    
    /*---------------------------------------------------------------------
      If physlinkupcmd returns success, traffic channel came out of 
      dormancy and hence reset the transient error.
    ---------------------------------------------------------------------*/
    if (AEE_SUCCESS == dsErrno)
    {
      ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32)SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
    }

  }

  LOG_MSG_FUNCTION_EXIT_3("Socket::HandleNonZeroWriteEventBitMask(): "
                          "Returning 0x%x, sock 0x%x ev mask 0x%x",
                          dsErrno, this, eventBitMask[writeEventIndex]);
  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  return dsErrno;
} /* Socket::HandleNonZeroWriteEventBitMask() */


bool Socket::IsPktInfoDifferent
(
  const ds::SockAddrInternalType*   remoteAddrPtr
)
{
  ds::AddrFamilyType addrFamily = ds::AddrFamily::QDS_AF_UNSPEC;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == rtMetaInfoPtr)
  {
    return true;
  }

  (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&addrFamily);

  if (ds::AddrFamily::QDS_AF_INET6 == addrFamily)
  {

    if (PS_IN6_IS_ADDR_V4MAPPED( remoteAddrPtr->v6.addr))
    {
      if (rtMetaInfoPtr->pkt_info.ip_hdr.v4.dest.ps_s_addr !=
            PS_IN6_GET_V4_FROM_V4_MAPPED_V6( remoteAddrPtr->v6.addr))
      {
        return true;
      }
    }
    else
    {
      if (false ==
            PS_IN6_ARE_ADDR_EQUAL
            (
              &( rtMetaInfoPtr->pkt_info.ip_hdr.v6.hdr_body.base_hdr.dst_addr),
              remoteAddrPtr->v6.addr
            ))
      {
        return true;
      }

      /*---------------------------------------------------------------------
        Make sure that scopeId in remote addr is same as the cached scopeId.
        Scope Id is specified for link local addresses
      ---------------------------------------------------------------------*/
      if (PS_IN6_IS_ADDR_MC_LINKLOCAL( remoteAddrPtr->v6.addr) ||
          PS_IN6_IS_ADDR_LINKLOCAL( remoteAddrPtr->v6.addr))
      {
        if (0 != remoteAddrPtr->v6.scopeId &&
            rtMetaInfoPtr->pkt_info.if_ptr !=
              NetPlatform::PSIfaceGetHandle( remoteAddrPtr->v6.scopeId))
        {
          return true;
        }
      }
    }
  }
  else
  {
    if (rtMetaInfoPtr->pkt_info.ip_hdr.v4.dest.ps_s_addr != remoteAddrPtr->v4.addr)
    {
      return true;
    }
  }

  return false;
} /* Socket::IsPktInfoDifferent() */


ds::ErrorType Socket::RoutePacket
(
  const ds::SockAddrInternalType*   remoteAddrPtr,
  boolean *                         isReorig,
  byte *                            data_buf,
  int                               len,
  bool                              isDoS
)
{
  ps_rt_meta_info_type *  newRtMetaInfoPtr = 0;
  SockAddrIN6InternalType v6RemoteAddr;
  ds::ErrorType           dsErrno = AEE_SUCCESS;
  int32                   psFlowHandle;
  int32                   physLinkHandle;
  phys_link_state_type    physLinkState;
  bool                    fwdDataToDefaultPSFlow = false;
  bool                    reorigPhysLink         = false;
  IPolicy                *policyPtr = 0;
  uint32                  writeEventIndex = 0;
  bool                    isFlowNotActivated = false;
  ds_sys_tech_enum_type   tech_type = DS_SYS_TECH_INVALID;
  byte                    packet[PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE] = {0};
  ps_iface_type          *iface_ptr = NULL;  
  ps_iface_type          *base_iface_ptr = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex)){
    LOG_MSG_ERROR_1("Socket::RoutePacket(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  LOG_MSG_FUNCTION_ENTRY_3("Socket::RoutePacket(): "
                           "Sock 0x%x write ev mask 0x%x isDoS %d",
                           this, eventBitMask[writeEventIndex], isDoS);

  /*-------------------------------------------------------------------------
    Since a fresh routing is performed, rtMetaInfo needs to be updated. So
    allocate a new buffer and update it with routing results
  -------------------------------------------------------------------------*/
  newRtMetaInfoPtr = (ps_rt_meta_info_type *)
                            ps_ext_mem_get_buf(PS_EXT_MEM_RT_META_INFO_TYPE);
  if (0 == newRtMetaInfoPtr)
  {
    //TODO Wrong causes infinite loop
    SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                     (uint32) SocketEvent::WRITE_BIT_MASK_PS_MEM_BUF_NOT_AVAILABLE);

    /*-----------------------------------------------------------------------
      Return AEE_EWOULDBLOCK instead of ENOMEM because this is a recoverable
      error. Memory might be available next time application writes a data
      packet
    -----------------------------------------------------------------------*/
    dsErrno = AEE_EWOULDBLOCK;
    goto bail;
  }

  /*-----------------------------------------------------------------------
  Init the buffer before use.
  -----------------------------------------------------------------------*/
  memset((newRtMetaInfoPtr), 0, sizeof(ps_rt_meta_info_type));

  /*-------------------------------------------------------------------------
    Generate pkt info, which is used in routing and filtering
  -------------------------------------------------------------------------*/
  dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, &v6RemoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::RoutePacket(): "
                    "GetSockAddrIN6 failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  dsErrno = GeneratePktInfo( &v6RemoteAddr,
                             &( PS_RT_META_GET_PKT_INFO( newRtMetaInfoPtr)));
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::RoutePacket(): "
                    "GeneratePktInfo failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Route the packet and fetch routing cache
  -------------------------------------------------------------------------*/

  policyPtr = networkCtlPtr->GetPolicy();

  dsErrno = RoutingManager::RoutePacket( this,
                                         isSystemSocket,
                                         policyPtr,
                                         newRtMetaInfoPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::RoutePacket(): "
                    "RoutingManager::RoutePacket failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  DS_UTILS_RELEASEIF(policyPtr);

  /*-------------------------------------------------------------------------
    Update the pkt info with source address if it isn't already set
  -------------------------------------------------------------------------*/
  dsErrno = UpdateSrcAddrInPktInfo( &v6RemoteAddr, newRtMetaInfoPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::RoutePacket(): "
                    "UpdateSrcAddrInPktInfo failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Filter various clients and store the results in rtMetaInfo
  -------------------------------------------------------------------------*/
  RoutingManager::FltrClient( this,
                              IP_FLTR_CLIENT_HEADER_COMP,
                              newRtMetaInfoPtr);
  SocketIPSec::FltrIPSecClient( this, newRtMetaInfoPtr, rtMetaInfoPtr);
  RoutingManager::FltrClient( this,
                              IP_FLTR_CLIENT_QOS_OUTPUT,
                              newRtMetaInfoPtr);

  /*-------------------------------------------------------------------------
    Send the new rtMetaInfo to platform
        //TODO What err handling?
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SetRtMetaInfo( newRtMetaInfoPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::RoutePacket(): "
                    "SetRtMetaInfo failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Reset old routing information
  -------------------------------------------------------------------------*/
  ResetRoutingCache(TRUE);
  rtMetaInfoPtr = newRtMetaInfoPtr;
  newRtMetaInfoPtr = 0;

  /*-------------------------------------------------------------------------
    Block socket from writing data if IPSec ifaces are not in UP state
  -------------------------------------------------------------------------*/
  if (false == SocketIPSec::IsIfaceListInReqState (rtMetaInfoPtr,
                                                   (uint32) IFACE_UP))
  {
    SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                     (uint32)SocketEvent::WRITE_BIT_MASK_IFACE_NOT_WRITEABLE);
    dsErrno = AEE_EWOULDBLOCK;
  }

  /*-------------------------------------------------------------------------
    If ps_flow is not in ACTIVATED state, data can't be transmitted and hence
    set PS_FLOW_SUSPENDED bit mask
  -------------------------------------------------------------------------*/
  psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);
  if (FLOW_ACTIVATED != NetPlatform::GetPSFlowState( psFlowHandle))
  {
    fwdDataToDefaultPSFlow = true;
    isFlowNotActivated = true;
  }

  /*-------------------------------------------------------------------------
    If phys link is in DOWN state, data can't be transmitted as
    traffic channel is dormant. So set DORMANT_TRAFFIC_CHANNEL bit mask
  -------------------------------------------------------------------------*/
  (void) NetPlatform::PSGetPhysLinkFromFlow( psFlowHandle, &physLinkHandle);
  if (0 != physLinkHandle)
  {
    (void) NetPlatform::PhysLinkIoctl( physLinkHandle,
                                       NetPlatform::PHYS_LINK_IOCTL_GET_STATE,
                                       (void *) &physLinkState);
    if (PHYS_LINK_UP != physLinkState)
    {
      if (PHYS_LINK_DOWN == physLinkState)
      {
        if(isDoS == true)
        {
          SetEventBitMask( eventBitMask[writeEventIndex],
                           SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
        }
        else
        {
          reorigPhysLink = true;
        }
      }
      if (isDoS != true)
      { 
        fwdDataToDefaultPSFlow = true;
      }
    }
  }
  else
  {
    fwdDataToDefaultPSFlow = true;
  }

  if (fwdDataToDefaultPSFlow)
  {
    UseDefaultPSFlow(isFlowNotActivated);
  }

  /*-------------------------------------------------------------------------
    Check if any entity in data path (routing cache, ps_flow and phys link)
    is flow controlled. This must be done only after socket is updated with
    newRtMetaInfoPtr as the method uses rtMetaInfoPtr member of socket.
    Otherwise, stale rt meta info is used in determining if data path is
    flow controlled.

    If any entity in data path is flow controlled, do not reorig phys link.
    For example if UE is OOS, there is no point in attempting to bring up
    phys link as it will never come up.
  -------------------------------------------------------------------------*/
  if (false == IsFlowEnabled())
  {
    if (0 == origFltrResult)
    {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
    }
    else
    {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED);
    }

    /*-----------------------------------------------------------------------
      Since phys link reorigination is blocked, set the bit mask so that
      phys link is reoriginated next time
    -----------------------------------------------------------------------*/
    if (true == reorigPhysLink)
    {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
    }

    dsErrno = AEE_EWOULDBLOCK;
  }
  else if (reorigPhysLink)
  {
    dsErrno = NetPlatform::PhysLinkUpCmd( physLinkHandle, 0);
    if (AEE_SUCCESS != dsErrno && AEE_EWOULDBLOCK != dsErrno)
    {
      LOG_MSG_INFO3_2( "Socket::RoutePacket(): "
                       "PhysLinkBringUpCmd failed, sock 0x%x err 0x%x",
                       this, dsErrno);
      goto bail;
    }
    else if (AEE_EWOULDBLOCK == dsErrno)
    {
      /*-------------------------------------------------------------------------
         Log the first packet which caused reorigination from dormancy 
      -------------------------------------------------------------------------*/
      *isReorig = TRUE;
      iface_ptr = (ps_iface_type *)PS_RT_META_GET_ROUTING_CACHE(rtMetaInfoPtr);
      base_iface_ptr = PS_IFACE_GET_BASE_IFACE(iface_ptr);
      if (base_iface_ptr != NULL)
      {
        if (base_iface_ptr->name == UMTS_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP;
        }
        else if (base_iface_ptr->name == CDMA_SN_IFACE)
        {
          tech_type = DS_SYS_TECH_3GPP2;
        }
        if (data_buf != NULL)
        {
          memscpy(packet, PS_WMK_LOGGING_REORIG_PKT_LOG_SIZE, data_buf, len);
          ps_wmk_logging_first_reorig_ip_pkt_hdr( tech_type, 
                                                  PS_WMK_UPLINK_PKT, 
                                                  packet);
        }
      }
      if (0 == origFltrResult)
      {
        SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
      }
    }
  }

  /*-------------------------------------------------------------------------
    Finally, reset NULL_ROUTING_INFO bit as all the information is
    available now
  -------------------------------------------------------------------------*/
  ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                     (uint32) SocketEvent::WRITE_BIT_MASK_NULL_ROUTING_INFO);

  /*-------------------------------------------------------------------------
    Check if pktMetaInfo needs to be generated for every packet
  -------------------------------------------------------------------------*/
  sendPktMetaInfo = NetPlatform::IsPSFlowDelaySensitive( psFlowHandle);

  LOG_MSG_FUNCTION_EXIT_1("Socket::RoutePacket(): "
                          "Success, sock 0x%x", this);
  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:

  DS_UTILS_RELEASEIF(policyPtr);

  if (0 != newRtMetaInfoPtr)
  {
    PS_RT_META_INFO_FREE( &newRtMetaInfoPtr);
  }

  return dsErrno;

} /* Socket::RoutePacket() */



ds::ErrorType Socket::GeneratePktInfo
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  ip_pkt_info_type *                   pktInfoPtr
)
{
  SockAddrInternalType       localSockAddr = {{0}};
  SockAddrIN6InternalType    tmpV6LocalSockAddr;
  ds::ErrorType              dsErrno;
  int32                      optVal;
  int32                      optLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset( pktInfoPtr, 0, sizeof( ip_pkt_info_type));

  if (false == PS_IN6_IS_ADDR_V4MAPPED( v6RemoteAddrPtr->addr))
  {
    /*-----------------------------------------------------------------------
      Populate IP Version and destination address
    -----------------------------------------------------------------------*/
    pktInfoPtr->ip_vsn             = IP_V6;
    pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPV6_BASE_HDR;

    pktInfoPtr->ip_hdr.v6.hdr_body.base_hdr.version  = (uint8) IP_V6;
    memscpy( &( pktInfoPtr->ip_hdr.v6.hdr_body.base_hdr.dst_addr),
             sizeof( struct ps_in6_addr),
             v6RemoteAddrPtr->addr,
             sizeof( ds::INAddr6Type));

    /*-----------------------------------------------------------------------
      Populate trafic class by fetching it from platform
    -----------------------------------------------------------------------*/
    dsErrno = platformSockPtr->GetOpt( OptLevel::QDS_LEVEL_IPV6,
                                       OptName::QDS_IPV6_TCLASS,
                                       &optVal,
                                       &optLen);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::GeneratePktInfo(): "
                      "GetOpt failed for IPV6_TCLASS, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }

    pktInfoPtr->ip_hdr.v6.hdr_body.base_hdr.trf_cls =
      static_cast <uint8> ( optVal);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Populate IP Version and destination address
    -----------------------------------------------------------------------*/
    pktInfoPtr->ip_vsn                = IP_V4;
    pktInfoPtr->ip_hdr.v4.dest.ps_s_addr =
      PS_IN6_GET_V4_FROM_V4_MAPPED_V6( v6RemoteAddrPtr->addr);

    /*-----------------------------------------------------------------------
      Populate TOS by fetching it from platform
    -----------------------------------------------------------------------*/
    dsErrno = platformSockPtr->GetOpt( OptLevel::QDS_LEVEL_IP,
                                       OptName::QDS_IP_TOS,
                                       &optVal,
                                       &optLen);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::GeneratePktInfo(): "
                      "GetOpt failed for IP_TOS, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }

    pktInfoPtr->ip_hdr.v4.tos = static_cast < uint8> ( optVal);
  }

  /*-------------------------------------------------------------------------
    Make sure that valid scopeId is specified if remote addr is linklocal
  -------------------------------------------------------------------------*/
  if (PS_IN6_IS_ADDR_MC_LINKLOCAL( v6RemoteAddrPtr->addr) ||
      PS_IN6_IS_ADDR_LINKLOCAL( v6RemoteAddrPtr->addr))
  {
    if (0 == v6RemoteAddrPtr->scopeId)
    {
      LOG_MSG_INVALID_INPUT_1("Socket::GeneratePktInfo(): "
                              "ScopeId not specified for linklocal addr, "
                              "sock 0x%x", this);
      dsErrno = QDS_ENOROUTE;
      goto bail;
    }

    pktInfoPtr->if_ptr =
      NetPlatform::PSIfaceGetHandle( v6RemoteAddrPtr->scopeId);

    if (0 == pktInfoPtr->if_ptr)
    {
      LOG_MSG_INVALID_INPUT_2("Socket::GeneratePktInfo(): "
                              "Invalid scopeId %d specified for linklocal "
                              "addr, sock 0x%x",
                              v6RemoteAddrPtr->scopeId, this);
      dsErrno = QDS_EINVAL;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    If socket is bound to an address, populate src address in pkt info.
    Otherwise leave it as 0.
  -------------------------------------------------------------------------*/
  // TODO how do we know whether its not bound/connected or there was a
  // legitimate error?
  dsErrno = platformSockPtr->GetSockName( &localSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GeneratePktInfo(): "
                    "GetSockName failed, sock 0x%x err 0x%x", this, dsErrno);
    dsErrno = AEE_SUCCESS;
    goto bail;
  }

  dsErrno = AddrUtils::GetSockAddrIN6( &localSockAddr, &tmpV6LocalSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GeneratePktInfo(): "
                    "GetSockAddrIN6 failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  if (false == PS_IN6_IS_ADDR_UNSPECIFIED( tmpV6LocalSockAddr.addr))
  {
    if (false == PS_IN6_IS_ADDR_V4MAPPED( tmpV6LocalSockAddr.addr))
    {
      memscpy( &( pktInfoPtr->ip_hdr.v6.hdr_body.base_hdr.src_addr),
               sizeof( struct ps_in6_addr),
               tmpV6LocalSockAddr.addr,   
               sizeof( ds::INAddr6Type));
    }
    else
    {
      pktInfoPtr->ip_hdr.v4.source.ps_s_addr =
        PS_IN6_GET_V4_FROM_V4_MAPPED_V6( tmpV6LocalSockAddr.addr);
    }
  }
  
  /*-------------------------------------------------------------------------
    Populate protocol specfic info
  -------------------------------------------------------------------------*/
  dsErrno = FillProtocolInfoInPktInfo( v6RemoteAddrPtr,
                                       &tmpV6LocalSockAddr,
                                       pktInfoPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GeneratePktInfo(): "
                    "FillProtocolInfoInPktInfo failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
  Mark Packet info valid. Even though Potocol will populate rest of the information,Soket will 
  set the flag.This flag set will make sure Protocol don't end up doing frag queuing again
  Additional Note: CR: 741257 
  -------------------------------------------------------------------------*/
  pktInfoPtr->is_pkt_info_valid = TRUE;

  LOG_MSG_FUNCTION_EXIT_1("Socket::GeneratePktInfo(): "
                          "Success, sock 0x%x", this);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  return dsErrno;

} /* Socket::GeneratePktInfo() */


ds::ErrorType Socket::UpdateSrcAddrInPktInfo
(
 const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  ps_rt_meta_info_type *              newRtMetaInfoPtr
)
{
  void *                   routingCachePtr;
  ip_pkt_info_type *       pktInfoPtr;
  SockAddrInternalType     localSockAddr;
  SockAddrIN6InternalType  tmpV6LocalSockAddr;
  ds::INAddr6Type          ifaceIPAddr = {0};
  ds::ErrorType            dsErrno;
  int32                    routingCache;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsErrno = platformSockPtr->GetSockName( &localSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::UpdateSrcAddrInPktInfo(): "
                    "GetSockName failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  dsErrno = AddrUtils::GetSockAddrIN6( &localSockAddr, &tmpV6LocalSockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::UpdateSrcAddrInPktInfo(): "
                    "GetSockAddrIN6 failed, sock 0x%x err 0x%x",
                    this, dsErrno);
    goto bail;
  }

  if (true == PS_IN6_IS_ADDR_UNSPECIFIED( tmpV6LocalSockAddr.addr))
  {
    pktInfoPtr = &( PS_RT_META_GET_PKT_INFO( newRtMetaInfoPtr));

    routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( newRtMetaInfoPtr);
    routingCache    = reinterpret_cast <int32> ( routingCachePtr);

    dsErrno = NetPlatform::IfaceGetAddr( routingCache,
                                         &( v6RemoteAddrPtr->addr),
                                         &ifaceIPAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::UpdateSrcAddrInPktInfo(): "
                      "NetPlatform::IfaceGetAddr failed, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }

    if (false == PS_IN6_IS_ADDR_UNSPECIFIED( ifaceIPAddr) &&
        false == PS_IN6_IS_ADDR_V4MAPPED( ifaceIPAddr))
    {
      /*---------------------------------------------------------------------
        If no address was bound then the default prefix is used. Ensure that
        the prefix is in a valid state to be used. (ie. It cannot be in the
        DEPRECATED or any other state than VALID)
      ---------------------------------------------------------------------*/
      if (false == NetPlatform::IsDefaultIPv6AddrStateValid( routingCache) &&
          false == PS_IN6_IS_ADDR_LINKLOCAL( v6RemoteAddrPtr->addr))
      {
        LOG_MSG_ERROR_1("Socket::UpdateSrcAddrInPktInfo(): "
                        "Can't use src IP addr in invalid state, sock 0x%x",
                        this);
        dsErrno = QDS_ENOSRCADDR;
        goto bail;
      }

      memscpy( &( pktInfoPtr->ip_hdr.v6.hdr_body.base_hdr.src_addr),
               sizeof( struct ps_in6_addr),
               ifaceIPAddr,              
               sizeof( ds::INAddr6Type));
    }
    else
    {
      pktInfoPtr->ip_hdr.v4.source.ps_s_addr =
        PS_IN6_GET_V4_FROM_V4_MAPPED_V6( ifaceIPAddr);
    }
  }

  LOG_MSG_FUNCTION_EXIT_1("Socket::UpdateSrcAddrInPktInfo(): "
                          "Success, sock 0x%x", this);
  return AEE_SUCCESS;

bail:
  LOG_MSG_FUNCTION_EXIT_2("Socket::UpdateSrcAddrInPktInfo(): "
                          "Fail, sock 0x%x err 0x%x", this, dsErrno);
  return dsErrno;

} /* Socket::UpdateSrcAddrInPktInfo() */



ds::ErrorType Socket::GeneratePktMetaInfo
(
  ps_pkt_meta_info_type **  pktMetaInfoPtrPtr,
  uint32                    flags
)
{
  time_type  timestamp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == pktMetaInfoPtrPtr)
  {
    LOG_MSG_ERROR_1("Socket::GeneratePktMetaInfo(): "
                    "NULL arg, sock 0x%x", this );
    ASSERT( 0);
    return QDS_EFAULT;
  }

  *pktMetaInfoPtrPtr = (ps_pkt_meta_info_type *)
                           ps_ext_mem_get_buf(PS_EXT_MEM_PKT_META_INFO_TYPE);

  if (0 == *pktMetaInfoPtrPtr)
  {
    SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                     (uint32) SocketEvent::WRITE_BIT_MASK_PS_MEM_BUF_NOT_AVAILABLE);

    /*-----------------------------------------------------------------------
      Return EWOULDBLOCK instead of ENOMEM because this is a recoverable
      error. Memory might be available next time application writes a data
      packet
    -----------------------------------------------------------------------*/
    return AEE_EWOULDBLOCK;
  }

  /*-----------------------------------------------------------------------
    Init the buffer before use.
  -----------------------------------------------------------------------*/
  memset((*pktMetaInfoPtrPtr), 0, sizeof(ps_pkt_meta_info_type));

  /*-----------------------------------------------------------------------
    Set the transmit flags and socket file descriptor
  -----------------------------------------------------------------------*/
  PS_PKT_META_SET_TX_FLAGS( *pktMetaInfoPtrPtr, flags);
  PS_PKT_META_SET_DOS_ACK_HANDLE( *pktMetaInfoPtrPtr,
                                  reinterpret_cast <int32> ( this));

  /*-------------------------------------------------------------------------
    Set time stamp
  -----------------------------------------------------------------------*/
  time_get_ms( timestamp);
  PS_PKT_META_SET_TIMESTAMP( *pktMetaInfoPtrPtr, timestamp);

  return AEE_SUCCESS;
} /* Socket::GeneratePktMetaInfo() */

bool Socket::IsFlowEnabled
(
  void
)
{
  void *  routingCachePtr;
  int32   routingCache;
  int32   psFlowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
  routingCache    = reinterpret_cast <int32> ( routingCachePtr);

  psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

  if (NetPlatform::IsPSIfaceCommonTxEnabled( routingCache, psFlowHandle) &&
      SocketIPSec::IsIfaceListFlowEnabled( rtMetaInfoPtr))
  {
    return true;
  }

  return false;
} /* Socket::IsFlowEnabled() */

bool Socket::IsFlowDoSEnabled
(
  void
)
{
  void *  routingCachePtr;
  int32   routingCache;
  int32   psFlowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  routingCachePtr = PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr);
  routingCache    = reinterpret_cast <int32> ( routingCachePtr);

  psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

  if (NetPlatform::IsPSIfaceCommonTxDoSEnabled( routingCache, psFlowHandle) &&
      SocketIPSec::IsIfaceListFlowEnabled( rtMetaInfoPtr))
  {
    return true;
  }

  return false;
} /* Socket::IsFlowDoSEnabled() */

void Socket::ProcessDoSAckEvent
(
  ds::Sock::Event::DoSAckEventInfo *  dosAckEventInfoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_ERR_FATAL("Socket::ProcessDoSAckEvent(): DOS_ACK_EV not supported");

  return;
} /* Socket::ProcessDoSAckEvent() */

ds::ErrorType Socket::RegNetworkStateChangedEvent
(
  void
)
{
  ISignal *         ifaceUpSignalPtr = 0;
  ISignal *         ifaceDownSignalPtr = 0;
  ISignalFactory *  signalFactoryPtr = 0;
  ds::ErrorType     dsErrno;
  IQI               *pRegObj = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Create a SignalCBFactory object
  -------------------------------------------------------------------------*/
  dsErrno =
    DS_Utils_CreateInstance(0,
                            AEECLSID_CSignalFactory,
                            reinterpret_cast <void **> (&signalFactoryPtr));
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("Socket::RegNetworkStateChangedEvent(): "
                    "Couldn't create SignalFactory, sock 0x%x", this);
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create a signal using signalFactory
  -------------------------------------------------------------------------*/
  dsErrno =
    signalFactoryPtr->CreateSignal(&signalHandler,
                                   EventCallbackIfaceUp,
                                   this,
                                   &ifaceUpSignalPtr,
                                   &ifaceUpSignalCtlPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("Socket::RegNetworkStateChangedEvent(): "
                    "Couldn't create IFACE_UP event Signal, sock 0x%x", this);
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create a signal using signalFactory
  -------------------------------------------------------------------------*/
  dsErrno =
    signalFactoryPtr->CreateSignal(&signalHandler,
                                   EventCallbackIfaceDown,
                                   this,
                                   &ifaceDownSignalPtr,
                                   &ifaceDownSignalCtlPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("Socket::RegNetworkStateChangedEvent(): "
                    "Couldn't create IFACE_DOWN event Signal, sock 0x%x",
                    this);
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register with network controller
  -------------------------------------------------------------------------*/
  dsErrno = networkCtlPtr->RegisterClientSignal
             (
               ifaceUpSignalPtr,
               ifaceDownSignalPtr,
               &pRegObj
             );

  /*-------------------------------------------------------------------------
    regObj mechanism is currently not in effect by dsnet layer.
    No need to hold the allocated object to maintain the event registration.
    For design simplicity we release the regObj immediately.
    If and when dsnet layer enforces the registration object concept
    this code need to be adapted accordingly.
  -------------------------------------------------------------------------*/
   DS_UTILS_RELEASEIF(pRegObj);

  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("Socket::RegNetworkStateChangedEvent(): "
                    "Couldn't reg ev, sock 0x%x", this);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Release the Signal objects as they are not needed anymore. Enabling and
    disabling the signals can be done by using the Signal Ctrl objects
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF(ifaceUpSignalPtr);
  DS_UTILS_RELEASEIF(ifaceDownSignalPtr);

  /*-------------------------------------------------------------------------
    Delete SignalFactory object. Could have created a SignalFactory
    object at power up and not ever delete it but this object is not needed
    most of the time and by deleting the object, less memory is consumed
  -------------------------------------------------------------------------*/
  DS_UTILS_RELEASEIF(signalFactoryPtr);

  LOG_MSG_FUNCTION_EXIT_3("Socket::RegNetworkStateChangedEvent(): "
                          "Success, sock 0x%x, up sig ctl 0x%x, down sig ctl 0x%x",
                          this, ifaceUpSignalCtlPtr, ifaceDownSignalCtlPtr);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Release all objects
  -------------------------------------------------------------------------*/
bail:
  DetachNetworkStateChangeSignalCtls();
  
  DS_UTILS_RELEASEIF(ifaceUpSignalPtr);
  DS_UTILS_RELEASEIF(ifaceDownSignalPtr);

  DS_UTILS_RELEASEIF(signalFactoryPtr);

  return dsErrno;
} /* Socket::RegNetworkStateChangedEvent() */

void Socket::DetachNetworkStateChangeSignalCtls
(
  void
)
{
  NetworkCtl::DetachSignal(&ifaceUpSignalCtlPtr);
  NetworkCtl::DetachSignal(&ifaceDownSignalCtlPtr);
} /* Socket::DetachNetworkStateChangeSignalCtl() */

void Socket::EventCallbackIfaceUp
(
  SignalHandlerBase *  pSignalHandlerBaseParam
)
{
  Socket *                sockPtr = 0;
  ds::Net::IfaceIdType    ifaceId = INVALID_IFACE_ID;
  ds::ErrorType           dsErrno = AEE_SUCCESS;
  ps_route_scope_type     routeScope;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pSignalHandlerBaseParam)
  {
    DATA_ERR_FATAL("Socket::EventCallbackIfaceUp(): NULL arg");
    return;
  }

  sockPtr = static_cast <Socket *> ( pSignalHandlerBaseParam);

  /*-------------------------------------------------------------------------
    Make sure that the object is not deleted.
  -------------------------------------------------------------------------*/
  if (NULL == sockPtr->critSectPtr || 0 == sockPtr->refCnt)
  {
    LOG_MSG_INFO1_1 ("Socket::EventCallbackIfaceUp(): "
                     "Obj %p already deleted, return", sockPtr);
    return;
  }

  sockPtr->critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == sockPtr->platformSockPtr)
  {
    LOG_MSG_INFO1_1("Socket::EventCallbackIfaceUp(): "
                    "Sock 0x%x closed", sockPtr);
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Generate route scope and set it in platform
  -----------------------------------------------------------------------*/
  (void) sockPtr->networkCtlPtr->GetIfaceId( &ifaceId);
  dsErrno = NetPlatform::GetRouteScopeByIfaceId( ifaceId,
                                                 ( ds::AddrFamily::QDS_AF_INET6 == sockPtr->family),
                                                 &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "GetRouteScopeByIfaceId failed, sock 0x%x err 0x%x",
                     sockPtr, dsErrno);
    goto bail;
  }

  dsErrno = sockPtr->platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "SetRouteScope failed, sock 0x%x err 0x%x",
                     sockPtr, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Re-enable the signal so that subsequent state changes are received
  -------------------------------------------------------------------------*/
  (void) sockPtr->ifaceUpSignalCtlPtr->Enable();

  sockPtr->critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("Socket::EventCallbackIfaceUp(): "
                          "Success, sock 0x%x", sockPtr);
  return;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  sockPtr->critSectPtr->Leave();
  return;
} /* Socket::EventCallbackIfaceUp() */

void Socket::EventCallbackIfaceDown
(
  SignalHandlerBase *  pSignalHandlerBaseParam
)
{
  Socket *                sockPtr = 0;
  ds::ErrorType           dsErrno = AEE_SUCCESS;
  ps_route_scope_type     routeScope;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == pSignalHandlerBaseParam)
  {
    DATA_ERR_FATAL("Socket::EventCallbackIfaceDown(): NULL arg");
    return;
  }

  sockPtr = static_cast <Socket *> ( pSignalHandlerBaseParam);
  
  /*-------------------------------------------------------------------------
    Make sure that the object is not deleted.
  -------------------------------------------------------------------------*/
  if (NULL == sockPtr->critSectPtr || 0 == sockPtr->refCnt)
  {
    LOG_MSG_INFO1_1 ("Socket::EventCallbackIfaceDown(): "
                     "Obj %p already deleted, return", sockPtr);
    return;
  }

  sockPtr->critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == sockPtr->platformSockPtr)
  {
    LOG_MSG_INFO1_1("Socket::EventCallbackIfaceDown(): "
                    "Sock 0x%x closed", sockPtr);
    goto bail;
  }

  /*-----------------------------------------------------------------------
    Set route scope to loopback interfaces
  -----------------------------------------------------------------------*/
  memset( &routeScope, 0, sizeof( ps_route_scope_type));
  (void) NetPlatform::AddLoopbackToRouteScope( ( ds::AddrFamily::QDS_AF_INET6 == sockPtr->family),
                                               &routeScope);

  dsErrno = sockPtr->platformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2( "SetRouteScope failed, sock 0x%x err 0x%x",
                     sockPtr, dsErrno);
    goto bail;
  }

  SocketIPSec::ClearIfaceList( sockPtr->rtMetaInfoPtr);
  sockPtr->ProcessNetworkConfigChangedEvent( QDS_ENETDOWN);

  /*-------------------------------------------------------------------------
    Re-enable the signal so that subsequent state changes are received
  -------------------------------------------------------------------------*/
  (void) sockPtr->ifaceDownSignalCtlPtr->Enable();

  sockPtr->critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_1("Socket::EventCallbackIfaceDown(): "
                          "Success, sock 0x%x", sockPtr);

  return;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  sockPtr->critSectPtr->Leave();
  return;
} /* Socket::EventCallbackIfaceDown() */

void Socket::UpdateRouteScopeByPolicy
(
  void
)
{
  ps_route_scope_type  currRouteScope;
  ps_route_scope_type  newRouteScope;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if route scope has changed before updating it in Platfrom socket
    and posting socket cmd to PS. Else it might result in too many
    SET_ROUTE_SCOPE socket cmds being posted to PS task causing cmd buffer
    exhaustion.
  -------------------------------------------------------------------------*/
  if (0 != platformSockPtr)
  {
    /*-----------------------------------------------------------------------
      Get New Route Scope
    -----------------------------------------------------------------------*/
    if(AEE_SUCCESS != networkCtlPtr->UpdateRouteScope(&newRouteScope))
    {
      LOG_MSG_INFO3_1( "GetRouteScopeByPolicy failed, obj 0x%x", this);
      return;
    }

    /*-----------------------------------------------------------------------
      Get current Route Scope from Platform Socket
    -----------------------------------------------------------------------*/
    if (AEE_SUCCESS != platformSockPtr->GetRouteScope(&currRouteScope))
    {
      LOG_MSG_INFO3_1( "GetRouteScope failed, obj 0x%x", this);
      return;
    }

    /*-----------------------------------------------------------------------
      If Route Scope has changed update the new route scope in Platform Socket
    -----------------------------------------------------------------------*/
    if (FALSE == ps_are_route_scopes_equal( newRouteScope, currRouteScope))
    {
      if (AEE_SUCCESS != platformSockPtr->SetRouteScope(&newRouteScope))
      {
        LOG_MSG_INFO3_1( "SetRouteScope failed, obj 0x%x", this);
        return;
      }
    }
  }

  return;

} /* Socket::UpdateRouteScopeByPolicy() */


void Socket::DeleteRoutingCacheFromRouteScope
(
  int32  routingCache
)
{
  ps_route_scope_type  routeScope;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();

  if (0 != platformSockPtr)
  {
    /*-----------------------------------------------------------------------
      Delete routing cache from route scope and set it in platform
    -----------------------------------------------------------------------*/
    if (AEE_SUCCESS != platformSockPtr->GetRouteScope( &routeScope))
    {
      LOG_MSG_INFO3_1( "GetRouteScope failed, sock 0x%x", this);
      goto bail;
    }

    if (AEE_SUCCESS != NetPlatform::DeleteIfaceHandleFromRouteScope( &routeScope,
                                                                 routingCache))
    {
      LOG_MSG_INFO3_1( "DeleteIfaceHandleFromRouteScope failed, sock 0x%x",
                       this);
      goto bail;
    }

    if (AEE_SUCCESS != platformSockPtr->SetRouteScope( &routeScope))
    {
      LOG_MSG_INFO3_1( "SetRouteScope failed, sock 0x%x", this);
      goto bail;
    }
  }

  critSectPtr->Leave();

  return;

bail:
  critSectPtr->Leave();
  return;

} /* Socket::DeleteRoutingCacheFromRouteScope() */


ds::ErrorType Socket::UpdateRtMetaInfoWithFlowFwding
(
  void
)
{
  ps_rt_meta_info_type *  newRtMetaInfoPtr;
  int32                   defaultPSFlowHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 != platformSockPtr)
  {
    /*-----------------------------------------------------------------------
      Since flow is forwarded on to default flow, rtMetaInfo needs to be
      updated to reflect it
    -----------------------------------------------------------------------*/
    newRtMetaInfoPtr = (ps_rt_meta_info_type *)
                            ps_ext_mem_get_buf(PS_EXT_MEM_RT_META_INFO_TYPE);
    if (0 == newRtMetaInfoPtr)
    {
      //TODO err handling
        return AEE_ENOMEMORY;
    }

    /*-----------------------------------------------------------------------
      Copy the existing Metainfo buffer.
    -----------------------------------------------------------------------*/
    memscpy( newRtMetaInfoPtr, sizeof( ps_rt_meta_info_type), 
             rtMetaInfoPtr, sizeof( ps_rt_meta_info_type));

    defaultPSFlowHandle =
      NetPlatform::GetDefaultPSFlowFromRtMetaInfo( rtMetaInfoPtr);
    PS_RT_META_SET_FILTER_RESULT( newRtMetaInfoPtr,
                                  IP_FLTR_CLIENT_QOS_OUTPUT,
                                  defaultPSFlowHandle);

    /*-----------------------------------------------------------------------
      Send the new rtMetaInfo to platform
    -----------------------------------------------------------------------*/
    //TODO Look in to error handling
    (void) platformSockPtr->SetRtMetaInfo( newRtMetaInfoPtr);

    /*-----------------------------------------------------------------------
      Store the actual filter result for subsequent event handling
    -----------------------------------------------------------------------*/
    origFltrResult = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);

    /*-----------------------------------------------------------------------
      Free the old rtMetaInfo and store the new one in the socket
    -----------------------------------------------------------------------*/
    PS_RT_META_INFO_FREE( &rtMetaInfoPtr);
    rtMetaInfoPtr  = newRtMetaInfoPtr;

    LOG_MSG_INFO1_1("Socket::UpdateRtMetaInfoWithFlowFwding(): "
                    "Fwding sock 0x%x to default flow", this);

    if (false == IsFlowEnabled())
    {
        SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED);
    }

    ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
  }

  LOG_MSG_FUNCTION_EXIT_1("Socket::UpdateRtMetaInfoWithFlowFwding(): "
                          "Success, sock 0x%x", this);
  return AEE_SUCCESS;

} /* Socket::UpdateRtMetaInfoWithFlowFwding() */


ds::ErrorType Socket::UseOrigFltrResult
(
  void
)
{
  ps_rt_meta_info_type *   newRtMetaInfoPtr;
  int32                    physLinkHandle;
  ps_flow_state_enum_type  psFlowState;
  phys_link_state_type     physLinkState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 != platformSockPtr)
  {
    psFlowState  = NetPlatform::GetPSFlowState( origFltrResult);

    (void) NetPlatform::PSGetPhysLinkFromFlow( origFltrResult, &physLinkHandle);
    (void) NetPlatform::PhysLinkIoctl( physLinkHandle,
                                       NetPlatform::PHYS_LINK_IOCTL_GET_STATE,
                                       (void *) &physLinkState);

    if (FLOW_ACTIVATED == psFlowState && PHYS_LINK_UP == physLinkState)
    {
      /*---------------------------------------------------------------------
        Since flow is forwarded on to default flow, rtMetaInfo needs to be
        updated to reflect it
      ---------------------------------------------------------------------*/
      newRtMetaInfoPtr = (ps_rt_meta_info_type *)
                            ps_ext_mem_get_buf(PS_EXT_MEM_RT_META_INFO_TYPE);

      if (0 == newRtMetaInfoPtr)
      {
        //TODO what to do?
        return AEE_ENOMEMORY;
      }

      memscpy( newRtMetaInfoPtr, sizeof( ps_rt_meta_info_type),
               rtMetaInfoPtr, sizeof( ps_rt_meta_info_type));

      PS_RT_META_SET_FILTER_RESULT( newRtMetaInfoPtr,
                                    IP_FLTR_CLIENT_QOS_OUTPUT,
                                    origFltrResult);

      /*---------------------------------------------------------------------
        Send the new rtMetaInfo to platform
      ---------------------------------------------------------------------*/
      //TODO Look in to error handling
      (void) platformSockPtr->SetRtMetaInfo( newRtMetaInfoPtr);

      /*---------------------------------------------------------------------
        Free the old rtMetaInfo and store the new one in the socket
      ---------------------------------------------------------------------*/
      PS_RT_META_INFO_FREE( &rtMetaInfoPtr);
      rtMetaInfoPtr  = newRtMetaInfoPtr;
      origFltrResult = 0;

      LOG_MSG_INFO1_1("Socket::UseOrigFltrResult(): "
                      "Using origFltrResult, sock 0x%x", this);

      if (false == IsFlowEnabled())
      {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
      }
      else
      {
      ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                         (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_DISABLED);
      }

    ResetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_FLOW_FWDING_DISABLED);
    }
#if 0
    else
    {
      if (PHYS_LINK_DOWN == physLinkState)
      {
      SetEventBitMask( SocketEvent::QDS_EV_WRITE,
                       (uint32) SocketEvent::WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL);
      }
    }
#endif
  }

  return AEE_SUCCESS;

} /* Socket::UseOrigFltrResult() */


void Socket::UseDefaultPSFlow
(
  bool isFlowNotActivated
)
{
  int32  psFlowHandle;
  int32  defaultPSFlowHandle;
  int32  physLinkHandle;
  int32  secPhysLinkHandle;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  psFlowHandle = NetPlatform::GetPSFlowFromRtMetaInfo( rtMetaInfoPtr);
  defaultPSFlowHandle =
      NetPlatform::GetDefaultPSFlowFromRtMetaInfo( rtMetaInfoPtr);
  
  if (true == isFlowNotActivated)
  {
    if (psFlowHandle != defaultPSFlowHandle)
    {
      (void) UpdateRtMetaInfoWithFlowFwding();
    }    
  }
  else
  {
    (void) NetPlatform::PSGetPhysLinkFromFlow( psFlowHandle, 
                                               &secPhysLinkHandle);
    (void) NetPlatform::PSGetPhysLinkFromFlow( defaultPSFlowHandle, 
                                               &physLinkHandle);

    LOG_MSG_INFO3_3("Socket::UseDefaultPSFlow(): "
                    "sock 0x%x secPhysLinkHandle:0x%x physLinkHandle:0x%x",
                    this, secPhysLinkHandle, physLinkHandle);

    if (secPhysLinkHandle != physLinkHandle)
    {
      (void) UpdateRtMetaInfoWithFlowFwding();
    }
  }

  return;
} /* Socket::UseDefaultPSFlow() */

/* Due to multiple inheritance (3 of the interfaces and SignalHandlerBase
   have IQI) we must override these IQI methods to specify which instance do
   we want to call, as 3 of those 4 are abstract.

   Virtual inheritance could have worked too, but a) AFAIK it is not supported
   by the IDL compiler and b) it would still have produced warning messages
   re. dominance inheritance (C4250 in MS compiler) */
uint32 CDECL Socket::AddRef
(
)
throw()
{
  return SignalHandlerBase::AddRef();
} /* Socket::AddRef() */

uint32 CDECL Socket::Release
(
)
throw()
{
  return SignalHandlerBase::Release();
} /* Socket::Release() */

ds::ErrorType Socket::GetIndexFromEvent
(
  ds::Sock::SocketEventType event,
  uint32*                   eventIndexPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(0 != eventIndexPtr);

  switch (event)
  {
    case SocketEvent::QDS_EV_WRITE:
      *eventIndexPtr = 0;
      break;

    case SocketEvent::QDS_EV_READ:
      *eventIndexPtr = 1;
      break;

    case SocketEvent::QDS_EV_CLOSE:
      *eventIndexPtr = 2;
      break;

    case SocketEvent::QDS_EV_ACCEPT:
      *eventIndexPtr = 3;
      break;

    case SocketEvent::QDS_EV_DOS_ACK:
      *eventIndexPtr = 4;
      break;

    default:
      return QDS_EFAULT;
  }

  return AEE_SUCCESS;

} /* Socket::GetIndexFromEvent() */

uint32 Socket::AddRefWeak
(
 void
 ) throw()
{
  return SignalHandlerBase::AddRefWeak();

} /* Socket::AddRefWeak() */

uint32 Socket::ReleaseWeak
(
 void
 ) throw()
{
  return SignalHandlerBase::ReleaseWeak();

} /* Socket::ReleaseWeak() */

boolean Socket::GetStrongRef
(
 void
 ) throw()
{
  return SignalHandlerBase::GetStrongRef();
} /* Socket::GetStrongRef() */


