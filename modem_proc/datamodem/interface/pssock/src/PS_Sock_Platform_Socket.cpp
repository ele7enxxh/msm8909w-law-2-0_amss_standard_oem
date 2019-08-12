/*===========================================================================
  FILE: PS_Sock_Platform_Socket.cpp

  OVERVIEW: This file implements PS::Sock::Platform::Socket class.

  DEPENDENCIES: None

  Copyright (c) 2008-2011,2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_Socket.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-10-28 aa  Support for truncate flags added to RecvMsg
  2010-09-07 sy  Fixed compiler warnings.
  2009-11-10 ss  IP Multicast Loop feature addition
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "PS_Sock_Platform_SocketFactory.h"
#include "PS_Sock_Platform_Socket.h"
#include "PS_Sock_IO_SocketIOFactory.h"
#include "ds_Net_Platform.h"
#include "ds_Sock_AddrUtils.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "ds_Sock_SocketDef.h"
#include "ds_Sock_RecvIFInfo.h"
#include "ds_Sock_ICMPErrInfo.h"
#include "ds_Utils_Conversion.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_List.h"

#include "ps_socket_cmd.h"
#include "ps_socket_event_defs.h"
#include "ps_mem.h"
#include "ps_handle_mgr.h"
#include "dsstcp.h"
#include "dssudp.h"
#include "dssicmp.h"
#include "ps_pcb_close_reason_codes.h"
#include "ps_tcp_config.h"
#include "PS_Sock_RemoteSocket_Manager.h"


using namespace ds::Sock;
using namespace ds::Error;
using namespace ds::Utils;
using namespace ds::Utils::Conversion;
using namespace PS::Sock::IO;
using namespace PS::Sock::Platform;
using namespace PS::Sock::RemoteSocket;


/*===========================================================================

                         LOCAL DATA DECLARATIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      namespace State
      {
        const StateType  AVAIL         = 0;
        const StateType  INIT          = 1;
        const StateType  LISTEN        = 2;
        const StateType  OPENING       = 3;
        const StateType  OPEN          = 4;
        const StateType  READ_ONLY     = 5;
        const StateType  WRITE_ONLY    = 6;
        const StateType  NO_READ_WRITE = 7;
        const StateType  CLOSING       = 8;
        const StateType  CLOSED        = 9;
      }

      namespace Opt
      {
        typedef struct
        {
          bool         isBoolVal;
          int32        defaultVal;
        } InfoType;

        const InfoType sockOptInfoArr[ ] =
        {
          /* SO_KEEPALIVE */
          { true, SO_KEEPALIVE_DEF_VAL },

          /* SO_REUSEADDR */
          { true, SO_REUSEADDR_DEF_VAL },

          /* SO_ERROR_ENABLE */
          { true, SO_ERROR_ENABLE_DEF_VAL },

          /* SO_TX_IFACE */
          { false, 0 },

          /* SO_RCVBUF */
          { false, SO_RCVBUF_DEF_VAL },

          /* SO_SNDBUF */
          { false, SO_SNDBUF_DEF_VAL },

          /* SO_ERROR */
          { false, 0 },

          /* TCP_NODELAY */
          { true, TCP_NODELAY_DEF_VAL },

          /* TCP_DELAYED_ACK */
          { true, TCP_DELAYED_ACK_DEF_VAL },

          /* TCP_SACK */
          { true, TCP_SACK_DEF_VAL },

          /* TCP_TIMESTAMP */
          { true, TCP_TIMESTAMP_DEF_VAL },

          /* TCP_FIONREAD */
          { false, 0 },

          /* TCP_MAXSEG */
          { false, TCP_MAXSEG_DEF_VAL },

          /* IP_RECVIF */
          { true, IP_RECVIF_DEF_VAL },

          /* IP_RECVERR */
          { true, IP_RECVERR_DEF_VAL },

          /* IP_TOS */
          { false, IP_TOS_DEF_VAL },

          /* IP_TTL */
          { false, IP_TTL_DEF_VAL },

          /* IP_MULTICAST_TTL */
          { false, IP_MCAST_TTL_DEF_VAL },

          /* IPV6_RECVERR */
          { true, IPV6_RECVERR_DEF_VAL },

          /* IPV6_TCLASS */
          { false, IPV6_TCLASS_DEF_VAL },

          /* QDS_TCP_MAX_BACKOFF_TIME */
          {false, TCP_MAX_TOTAL_BACKOFF_TIME},

          /* IP_MULTICAST_LOOP */
          { true, IP_MCAST_LOOP_DEF_VAL },

          /* SO_FIONREAD */
          { false, 0 },

          /* SO_SKIP_ROUTE_CHECK */
          { false, SO_SKIP_ROUTE_SCOPE_DEF_VAL },

          /* UDP_ENCAPS */
          { true, UDP_ENCAPS_DEF_VAL }
        };
      } /* namespace Opt */

      namespace OptPriv
      {
        typedef struct
        {
          bool         isBoolVal;
          int32        defaultVal;
        } InfoType;

        const InfoType sockOptPrivInfoArr[ ] =
        {
          /* ICMP_ECHO_ID */
          { false, 0 },

          /* ICMP_ECHO_SEQ_NUM */
          { false, 0 },

          /* ICMP_TYPE */
          { false, 0 },

          /* ICMP_CODE */
          { false, 0 },
        };
      } /* namespace OptPriv */

      /**
        @brief Mapping from reason for closing to ds::Error for Connect().

        Provides a mapping from the reason why a socket is closed to
        ds::Error. This mapping is used to return appropriate error from
        Connect() when TCP socket is in CLOSED state.
      */
      /*---------------------------------------------------------------------
        This array is intentionally defined with out explicit size so that
        if a new reason code is added and if mapping is not added to this
        array, static analysis tools will throw boundary exception error.
        Else, new reason code will get a mapping of default value 0 and this
        bug will not be caught unless a test case is written for it.
      ---------------------------------------------------------------------*/
      /*lint -e{578} */
      static ds::ErrorType  pcbClosedReasonToConnErrMapping[] =
        {QDS_ENOTCONN, QDS_ECONNREFUSED, QDS_ETIMEDOUT, QDS_ENETDOWN,
         QDS_EIPADDRCHANGED};

      /**
        @brief Mapping from reason for closing to ds::Error for Socket I/O.

        Provides a mapping from the reason why a socket is closed to
        ds::Error. This mapping is used to return appropriate error from
        Socket I/O routines when TCP socket is in CLOSED state.
      */
      /*---------------------------------------------------------------------
        These arrays are intentionally defined with out explicit size so that
        if a new reason code is added and if mapping is not added to this
        array, static analysis tools will throw boundary exception error.
        Else, new reason code will get a mapping of default value 0 and this
        bug will not be caught unless a test case is written for it.
      ---------------------------------------------------------------------*/
      /*lint -e{578} */
      static ds::ErrorType  pcbClosedReasonToSockInputErrMapping[] =
        {QDS_EEOF, QDS_ECONNRESET, QDS_ECONNABORTED, QDS_ENETDOWN,
         QDS_EIPADDRCHANGED};

      /*lint -e{578} */
      static ds::ErrorType  pcbClosedReasonToSockOutputErrMapping[] =
        {QDS_ENOTCONN, QDS_ECONNRESET, QDS_ECONNABORTED, QDS_ENETDOWN,
         QDS_EIPADDRCHANGED};

    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#define PERMANENT_ERR_BIT_MASK     0x0000FFFF
#define FLOW_CONTROL_ERR_BIT_MASK  0xFFFF0000

#define NON_WRITEABLE_STATE    0x00000001
#define WR_ERROR_AVAILABLE     0x00000002
#define SNDQ_FULL              0x00010000
#define CONNECTION_IN_PROGRESS 0x00020000

#define NON_READABLE_STATE 0x00000001
#define RD_ERROR_AVAILABLE 0x00000002
#define RCVQ_EMPTY         0x00010000

#define NON_LISTENABLE_STATE 0x00000001
#define ACCEPTED_LIST_EMPTY  0x00010000
/*---------------------------------------------------------------------------
 //TODO Reword
  App pri < PS pri. Cud cause PS to process listen cmd and ProtocolStack could
  accept a conn before LISTEN() operation is complete. If so ACCEPT_EV is
  posted if it is registered prior to app calling Listen() and we dont want to
  post ACCEPT_EV until Listen() completes and is successful
---------------------------------------------------------------------------*/
#define LISTEN_NOT_PERFORMED 0x00020000

#define CLOSING_STATE   0x00010000
#define CLOSEABLE_STATE 0x00020000

#define PLATFORM_IS_DISABLED 0x00010000

/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
//TODO Fix this
Socket * Socket::CreateInstance
(
  ds::AddrFamilyType   _family,
  SocketType           sockType,
  ProtocolType         _protocol,
  ICritSect *          _critSectPtr,
  ds::ErrorType *      dsErrnoPtr
)
{
  Socket *   platformSockPtr       = 0;
  IReader *  inetSndQReaderPtr     = 0;
  IReader *  platformRcvQReaderPtr = 0;
  IReader *  platformErrQReaderPtr = 0;
  IWriter *  inetRcvQWriterPtr     = 0;
  IWriter *  platformSndQWriterPtr = 0;
  IWriter *  inetErrQWriterPtr     = 0;
  Manager *  remoteSockManagerPtr  = 0;
  int32      bufIndex;
  int32      retVal;
  int16      psErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::CreateInstance(): "
                           "Proto %d family %d type %d",
                           _protocol, _family, sockType);

  if (0 == dsErrnoPtr)
  {
    DATA_ERR_FATAL("Socket::CreateInstance(): NULL errno");
    return 0;
  }

  if (0 == _critSectPtr)
  {
    DATA_ERR_FATAL("Socket::CreateInstance(): NULL CS");
    *dsErrnoPtr = QDS_EFAULT;
    return 0;
  }

  /*-------------------------------------------------------------------------
    Allocate Reader and Writer pairs for sockets platform and ProtocolStack
  -------------------------------------------------------------------------*/
  *dsErrnoPtr = SocketIOFactory::GetReaderAndWriter( _protocol,
                                                     &platformRcvQReaderPtr,
                                                     &inetRcvQWriterPtr,
                                                     _critSectPtr);
  if (AEE_SUCCESS != *dsErrnoPtr ||
      0 == platformRcvQReaderPtr ||
      0 == inetRcvQWriterPtr)
  {
    LOG_MSG_INFO3_1("Socket::CreateInstance(): "
                    "SocketIOFactory::GetReaderAndWriter failed, err 0x%x",
                    *dsErrnoPtr);
    goto bail;
  }

  *dsErrnoPtr = SocketIOFactory::GetReaderAndWriter( _protocol,
                                                     &inetSndQReaderPtr,
                                                     &platformSndQWriterPtr,
                                                     _critSectPtr);
  if (AEE_SUCCESS != *dsErrnoPtr ||
      0 == inetSndQReaderPtr ||
      0 == platformSndQWriterPtr)
  {
    LOG_MSG_INFO3_1("Socket::CreateInstance(): "
                    "SocketIOFactory::GetReaderAndWriter failed, err 0x%x",
                    *dsErrnoPtr);
    goto bail;
  }

  if (Protocol::QDS_UDP == _protocol)
  {
    *dsErrnoPtr = SocketIOFactory::GetReaderAndWriter( _protocol,
                                                       &platformErrQReaderPtr,
                                                       &inetErrQWriterPtr,
                                                       _critSectPtr);
    if (AEE_SUCCESS != *dsErrnoPtr)
    {
      LOG_MSG_INFO3_1("Socket::CreateInstance(): "
                      "SocketIOFactory::GetReaderAndWriter failed, err 0x%x",
                      *dsErrnoPtr);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Create platform socket
  -------------------------------------------------------------------------*/
  platformSockPtr = new Socket( _family,
                                sockType,
                                _protocol,
                                _critSectPtr,
                                platformRcvQReaderPtr,
                                platformSndQWriterPtr,
                                platformErrQReaderPtr);
  if (0 == platformSockPtr)
  {
    LOG_MSG_ERROR_0("Socket::CreateInstance(): "
                    "No mem for Platform Sock");
    *dsErrnoPtr = QDS_EMFILE;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Generate a handle for platform socket. This handle will be the platform
    socket descriptor
  -------------------------------------------------------------------------*/
  bufIndex = ps_mem_buf_to_index( platformSockPtr);
  if (-1 == bufIndex)
  {
    LOG_MSG_ERROR_1("Socket::CreateInstance(): "
                    "Invalid buf 0x%p", platformSockPtr);
    ASSERT( 0);
    *dsErrnoPtr = QDS_EINVAL;
    goto bail;
  }

  platformSockPtr->platformSockHandle =
    ps_handle_mgr_get_handle( PS_HANDLE_MGR_CLIENT_SOCKET_PLATFORM,
                              static_cast <int16> ( bufIndex));

  if (PS_HANDLE_MGR_INVALID_HANDLE == platformSockPtr->platformSockHandle)
  {
    LOG_MSG_INFO3_1("Socket::CreateInstance(): "
                    "Invalid handle %d", platformSockPtr->platformSockHandle);
    ASSERT(0);
    *dsErrnoPtr = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Call protocol stack to create a PCB. Store PCB handle in the platform
    socket
  -------------------------------------------------------------------------*/
  if (Protocol::QDS_TCP == _protocol)
  {
    retVal = dsstcp_socket( (uint16) _family,
                            platformSockPtr,
                            platformSockPtr->platformSockHandle,
                            &( platformSockPtr->pcbHandle),
                            inetSndQReaderPtr,
                            inetRcvQWriterPtr,
                            inetErrQWriterPtr,
                            &psErrno);
  }
  else if (Protocol::QDS_UDP == _protocol)
  {
    retVal = dssudp_socket( (uint16) _family,
                            platformSockPtr,
                            platformSockPtr->platformSockHandle,
                            &( platformSockPtr->pcbHandle),
                            inetSndQReaderPtr,
                            inetRcvQWriterPtr,
                            inetErrQWriterPtr,
                            &psErrno);
  }
  else /* Protocol::ICMP == protocol */
  {
    retVal = dssicmp_int_socket( (uint16) _family,
                                 platformSockPtr,
                                 platformSockPtr->platformSockHandle,
                                 &( platformSockPtr->pcbHandle),
                                 inetSndQReaderPtr,
                                 inetRcvQWriterPtr,
                                 inetErrQWriterPtr,
                                 &psErrno);
  }

  if (DSS_ERROR == retVal)
  {
    LOG_MSG_ERROR_2("Socket::CreateInstance(): "
                    "Couldn't create PCB, sock %d errno %d",
                    platformSockPtr->platformSockHandle, psErrno);
    *dsErrnoPtr = QDS_EMFILE;
    goto bail;
  }

  platformSockPtr->SetEventBitMask( PS::Sock::Platform::Event::READ,
                                    RCVQ_EMPTY);

  platformSockPtr->SetEventBitMask( PS::Sock::Platform::Event::ACCEPT,
                                    LISTEN_NOT_PERFORMED);

  /*-------------------------------------------------------------------------
    Set State appropriately and initialize all event bit masks
  -------------------------------------------------------------------------*/
  if (Protocol::QDS_TCP == _protocol)
  {
    platformSockPtr->SetState( State::INIT);
  }
  else
  {
    platformSockPtr->SetState( State::OPEN);
  }
  
  /*-------------------------------------------------------------------------
    Initialize Remote socket handle to zero
  -------------------------------------------------------------------------*/  
  platformSockPtr->SetRemoteSocktHandle(0);

  /*-------------------------------------------------------------------------
    A socket is
      1. Created when Sockets library calls ISocketFactory::CreateSocket().
         In this case, both Sockets library and PCB hold pointer to it
      2. In partial Q of a listening socket. In this case, both partial Q and
         PCB hold pointer to it
      3. In accepted list of a listening socket. In this case, both accepted
         list and PCB hold pointer to it
      4. Accepted when Sockets library calls ISocket::Accept(). In this case,
         both Sockets library and PCB hold pointer to it

    Since in all these case, 2 modules are holding pointer to a socket object,
    set ref cnt to 2
  -------------------------------------------------------------------------*/
  platformSockPtr->refCnt = 2;
  
  /*-------------------------------------------------------------------------
    Query 20 sockets from AP/netmgr. Those sockets would be cached. When
    bind is called with port 0 the port number would be picked up from that 
    cache if sockets are available in cache else Remote Socket manger would
    generate random port number between 32000 to 36999
    Note : Query for 20 sockets would happen only when 1st socket is created
    For other sockets, this call wont query any sockets.
  -------------------------------------------------------------------------*/  
  remoteSockManagerPtr = PS::Sock::RemoteSocket::Manager::GetInstance();
  remoteSockManagerPtr->RequestMaxSockets(); 

  LOG_MSG_INFO1_1("Socket::CreateInstance(): "
                  "Created sock %d", platformSockPtr->platformSockHandle);
  return platformSockPtr;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  /*-------------------------------------------------------------------------
    Free reader and writer that are allocated for ProtocolStack
  -------------------------------------------------------------------------*/
  if (0 != inetSndQReaderPtr)
  {
    inetSndQReaderPtr->ReleaseReader();
  }

  if (0 != inetRcvQWriterPtr)
  {
    inetRcvQWriterPtr->ReleaseWriter();
  }

  if (0 != inetErrQWriterPtr)
  {
    inetErrQWriterPtr->ReleaseWriter();
  }

  /*-------------------------------------------------------------------------
    If platform socket is created, platform reader/writer are released in
    socket's destructor. Otherwise, they need to be explicitly released
  -------------------------------------------------------------------------*/
  if (0 != platformSockPtr)
  {
    (void) platformSockPtr->Release();
    platformSockPtr = NULL;
  }
  else
  {
    if (0 != platformRcvQReaderPtr)
    {
      platformRcvQReaderPtr->ReleaseReader();
    }

    if (0 != platformSndQWriterPtr)
    {
      platformSndQWriterPtr->ReleaseWriter();
    }

    if (0 != platformErrQReaderPtr)
    {
      platformErrQReaderPtr->ReleaseReader();
    }
  }

  return 0;
} /* Socket::CreateInstance() */

void Socket::Destructor
(
  void
)
throw()
{
  SocketFactory *  platformSockFactoryPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("Socket::Destructor(): "
                           "Sock %d", platformSockHandle);

  /*-------------------------------------------------------------------------
    Socket object is deleted only when refCnt is 0 which means PCB is already
    freed. Freak out if that is not the case
  -------------------------------------------------------------------------*/
  if (0 != pcbHandle)
  {
    LOG_MSG_ERROR_1("Socket::Destructor(): "
                    "PCB is not closed, sock %d", platformSockHandle);
    ASSERT( 0);
  }

  /*lint -save -e1550, -e1551 */
  DS_UTILS_RELEASEIF(critSectPtr);

  /*-------------------------------------------------------------------------
    Release reader and writer if they weren't released already
  -------------------------------------------------------------------------*/
  if (0 != rcvQReaderPtr)
  {
    rcvQReaderPtr->ReleaseReader();
  }

  if (0 != sndQWriterPtr)
  {
    sndQWriterPtr->ReleaseWriter();
  }

  if (0 != errQReaderPtr)
  {
    errQReaderPtr->ReleaseReader();
  }

  if (0 != eventListenerPtr)
  {
    eventListenerPtr->ReleaseWeak();
    eventListenerPtr = 0;
  }
  /*-----------------------------------------------------------------------
    Delete self from SocketFactory
  -----------------------------------------------------------------------*/
  platformSockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);

  platformSockFactoryPtr->DeleteSocket( this);
  DS_UTILS_RELEASEIF(platformSockFactoryPtr);
  /*lint -restore */

  return;

}

Socket::~Socket
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr = NULL;
  rcvQReaderPtr = NULL;
  sndQWriterPtr = NULL;
  errQReaderPtr = NULL;
  listenHeadPtr = NULL;

} /* Socket::~Socket() */


void Socket::RegEventListener
(
  PS::Sock::Platform::IEventListener *  newEventListenerPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_EXIT_2("Socket::RegEventListener(): "
                          "Success, sock %d, listener 0x%x",
                          platformSockHandle, newEventListenerPtr);

  if (0 == newEventListenerPtr)
  {
   ASSERT( 0);
   return;
  }

  eventListenerPtr = newEventListenerPtr;

  /* Hold a weak reference to the registered event listener */
  (void) eventListenerPtr->AddRefWeak();

  /* Reconcile the state of the eventListener with the state of the socket */
  for (EventType event = PS::Sock::Platform::Event::READ;
       event < PS::Sock::Platform::Event::PLATFORM_ENABLED + 1;
       event++)
  {
    SetOrClearEvent((PS::Sock::Platform::EventType) event);
  }

  return;

} /* Socket::RegEventListener() */


void Socket::DeregEventListener
(
)
{

  /* Release the weak reference to registered event listener */
  if (0 != eventListenerPtr)
  {
    eventListenerPtr->ReleaseWeak();
    eventListenerPtr = 0;
  }

  LOG_MSG_FUNCTION_EXIT_2("Socket::DeregEventListener(): "
                          "Success, sock %d,  listener 0x%x",
                          platformSockHandle, eventListenerPtr);
}


void Socket::SetCritSection
(
  ICritSect *  newCritSectPtr
)
{
  int16        psErrno;
  ICritSect *  tmpCritSectPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_EXIT_2("Socket::SetCritSection(): "
                          "Success, sock %d, crit sect 0x%x",
                          platformSockHandle, newCritSectPtr);


  if (0 == newCritSectPtr)
  {
   LOG_MSG_ERROR_1("Socket::SetCritSection(): "
                   "NULL crit sect, sock %d", platformSockHandle);
   ASSERT( 0);
   return;
  }

  tmpCritSectPtr = critSectPtr;
  tmpCritSectPtr->Enter();

  critSectPtr = newCritSectPtr;
  (void) critSectPtr->AddRef();

  if (NULL!= rcvQReaderPtr)
  {
    (void) rcvQReaderPtr->SetRdCritSection( newCritSectPtr, &psErrno);
  }

  if (NULL != sndQWriterPtr)
  {
    (void) sndQWriterPtr->SetWrCritSection( newCritSectPtr, &psErrno);
  }

  if (NULL != errQReaderPtr)
  {
    (void) errQReaderPtr->SetRdCritSection( newCritSectPtr, &psErrno);
  }

  tmpCritSectPtr->Leave();
  DS_UTILS_RELEASEIF(tmpCritSectPtr);

  return;

} /* Socket::SetCritSection() */


ds::ErrorType Socket::Bind
(
  const ds::SockAddrInternalType* localAddrPtr
)
{
  ps_socket_cmd_type *  bindPSCmdBufPtr;
  SocketFactory *       platformSockFactoryPtr;
  ds::SockAddrIN6InternalType   v6SockAddr;
  ds::ErrorType         dsErrno;
  bool                  reUseAddr;
  Manager             * remoteSockManagerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::Bind(): "
                           "Sock %d, localAddrPtr 0x%x, state %d",
                           platformSockHandle, localAddrPtr, state);

  if (0 == localAddrPtr)
  {
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    A TCP socket can only be bound in INIT state and UDP socket can be bound
    in OPEN state
  -------------------------------------------------------------------------*/
  if (State::INIT != state && State::OPEN != state)
  {
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    If socket is already bound, it is not allowed to bind again.

    One can rely on whether local port is non-zero, to determine if a socket
    is already bound (Even if the applications calls Bind() with 0 as port,
    a random port is chosen by the stack and the socket is bound to that
    port).

    One can also check if local addr is in6_addr_any.
  -------------------------------------------------------------------------*/
  if (0 != v6LocalSockAddr.port)
  {
    LOG_MSG_INFO1_1("Socket::Bind(): "
                    "Sock %d is bound already", platformSockHandle);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Translate the address passed by application in to a v6 address
  -------------------------------------------------------------------------*/
  dsErrno = AddrUtils::GetSockAddrIN6( localAddrPtr, &v6SockAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Bind(): "
                    "GetSockAddrIN6 failed, sock %d err 0x%x",
                    platformSockHandle, dsErrno);
    return dsErrno;
  }

  /*-------------------------------------------------------------------------
    Generate ephemeral port if port is unspecified. Otherwise if it is not a
    listening socket, check if other sockets are using this IP address/port
    combination for this socket's protocol. Local address re-use is not
    supported unless SO_REUSEADDR socket option is set.

    As all sockets in a listening socket's backlog queue bind to same port as
    the listening socket, address reuse check must not be performed for them.
  -------------------------------------------------------------------------*/
  platformSockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);

  if (0 == v6SockAddr.port)
  {
    /*-----------------------------------------------------------------------
      Request Port from Remote socket manager.
    -----------------------------------------------------------------------*/
    remoteSockManagerPtr = PS::Sock::RemoteSocket::Manager::GetInstance();
    v6SockAddr.port = 
      remoteSockManagerPtr->RetrieveAndRequestSocket( platformSockHandle, 
                                                      protocol,
                                                      family);
    if (0 == v6SockAddr.port)
    {
      LOG_MSG_INFO2_1("Socket::Bind(): "
                      "Couldn't get ephem port, sock %d. Requested for "
                      "ports from AP", platformSockHandle);
      DS_UTILS_RELEASEIF(platformSockFactoryPtr);   
      ASSERT(0);
      return QDS_EINVAL;
    }    
  }
  else if (0 == listenHeadPtr)
  {
    reUseAddr = IsOptCacheSet( ds::Sock::OptName::QDS_SO_REUSEADDR);
    if (platformSockFactoryPtr->IsSockAddrInUse( protocol,
                                                 &v6SockAddr,
                                                 reUseAddr))
    {
      LOG_MSG_INVALID_INPUT_2("Socket::Bind(): "
                              "Port %d in use, sock %d",
                              dss_ntohs( v6SockAddr.port), platformSockHandle);
      DS_UTILS_RELEASEIF(platformSockFactoryPtr);
      return QDS_EADDRINUSE;
    }
  }

  DS_UTILS_RELEASEIF(platformSockFactoryPtr);

  /*-------------------------------------------------------------------------
    Post a cmd to PS to trigger bind at the protocol stack layer
  -------------------------------------------------------------------------*/
  bindPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_BIND);
  if (0 == bindPSCmdBufPtr)
  {
    return AEE_ENOMEMORY;
  }

  LOG_MSG_INFO1_2("Socket::Bind(): "
                  "Binding sock %d to port %d",
                  platformSockHandle, dss_ntohs( v6SockAddr.port));

  memscpy( &v6LocalSockAddr, sizeof( ds::SockAddrIN6InternalType),
           &v6SockAddr, sizeof( ds::SockAddrIN6InternalType));

  memscpy( &( bindPSCmdBufPtr->user_data.local_addr),
          sizeof(struct ps_sockaddr_in6),
          &v6SockAddr,
          sizeof( ds::SockAddrIN6InternalType));

  ps_socket_cmd_send( &bindPSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  return AEE_SUCCESS;
} /* Socket::Bind() */

void Socket::SetRemoteSocktHandle
(
  uint32  handle
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  remoteSockHandle = handle;
} /* Socket::SetRemoteSocktHandle */    

uint32 Socket::GetRemoteSockHandle
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return remoteSockHandle;
} /* Socket::GetRemoteSockHandle */ 

int16 Socket::GetPort
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return v6LocalSockAddr.port;
} /* Socket::GetPort */   

ds::AddrFamilyType Socket::GetFamily
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return family;
} /* Socket::GetFamily */  

ds::Sock::ProtocolType Socket::GetProtocol
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return protocol;
} /* Socket::GetProtocol */  

boolean Socket::IsSocketAlive
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return !(State::CLOSED == state || State::CLOSING == state);
} /* Socket::IsSocketAlive */  

ds::ErrorType Socket::Connect
(
  const ds::SockAddrInternalType *  remoteAddrPtr
)
{
  ps_socket_cmd_type *          connectPSCmdBufPtr = 0;
  ds::SockAddrIN6InternalType   v6RemoteAddr = {0};
  ds::SockAddrInternalType      tmpLocalAddr = {{0}};
  ds::ErrorType                 dsErrno = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::Connect(): "
                           "Sock %d, remoteAddrPtr 0x%x",
                           platformSockHandle, remoteAddrPtr);

  if (0 == remoteAddrPtr)
  {
    return QDS_EFAULT;
  }

  dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, &v6RemoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Connect(): "
                    "GetSockAddrIN6 failed, sock %d err 0x%x",
                    platformSockHandle, dsErrno);
    return dsErrno;
  }

  /*-------------------------------------------------------------------------
    If the socket is not connected already and if the client is trying to
    disconnect the socket, return success silently
  -------------------------------------------------------------------------*/
  if (true == PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddr.addr, &ps_in6addr_any) &&
      0 == v6RemoteSockAddr.port)
  {
    LOG_MSG_INFO1_1( "Socket not in connected state, sock %d",
                     platformSockHandle);
    return AEE_SUCCESS;
  }

  /*-------------------------------------------------------------------------
    Allocate connect command buffer
  -------------------------------------------------------------------------*/
  connectPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_CONNECT);
  if (0 == connectPSCmdBufPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Connect() is supported in
      1. INIT state for TCP sockets
      2. OPEN state for UDP sockets
  -------------------------------------------------------------------------*/
  if (Protocol::QDS_TCP == protocol)
  {
    if (State::INIT != state)
    {
      goto bail;
    }
  }
  else if (State::OPEN != state)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If socket is not disconnecting, check whether local port is 0. Since 0 is
    not a valid port, it means that application didn't call Bind() on this
    socket. So, call Bind() with 0 as local port so that socket is bound to an
    ephemeral port
  -------------------------------------------------------------------------*/
  if (false == PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddr.addr, &ps_in6addr_any) &&
      0 == v6LocalSockAddr.port)
  {

    (void)ds::Sock::AddrUtils::SetFamily(&tmpLocalAddr,family);

    dsErrno = Bind( &tmpLocalAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO2_1("Socket::Connect(): "
                      "Bind failed, sock %d", platformSockHandle);
      goto bail;
    }
  }

  memscpy( &( connectPSCmdBufPtr->user_data.dest_addr),
           sizeof(struct ps_sockaddr_in6),
           &v6RemoteAddr,
           sizeof( ds::SockAddrIN6InternalType));

  ps_socket_cmd_send( &connectPSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  /*-------------------------------------------------------------------------
    If socket is getting disconnected, reset local IP address. From this
    point, data can go on any interface that matches socket's netpolicy
  -------------------------------------------------------------------------*/
  if (PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddr.addr, &ps_in6addr_any))
  {
    memset( v6LocalSockAddr.addr, 0, sizeof( ds::INAddr6Type));
  }

  /*-------------------------------------------------------------------------
    Update peer addr
  -------------------------------------------------------------------------*/
  memscpy( &( v6RemoteSockAddr), sizeof(struct ps_sockaddr_in6),
           &v6RemoteAddr, sizeof( ds::SockAddrIN6InternalType));

  /*-------------------------------------------------------------------------
    Update state

    TCP socket goes to OPENING state and UDP socket remains in it's current
    state. DON'T change UDP socket's state to OPEN explicitly because
    Connect() could have been called in READ_ONLY state for example
  -------------------------------------------------------------------------*/
  if (State::INIT == state)
  {
    SetState( State::OPENING);
    dsErrno = AEE_EWOULDBLOCK;
  }
  else
  {
    dsErrno = AEE_SUCCESS;
  }

  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code

    It is possible that Bind() succeeded but a cmd to post Connect to PS stack
    couldn't be allocated. Ideally socket should be unbound but it doesn't
    hurt if it is not done cause app never intended to call Bind() anyway
  -------------------------------------------------------------------------*/
bail:
  if (0 != connectPSCmdBufPtr)
  {
    ps_socket_cmd_free( &connectPSCmdBufPtr);
  }

  if (AEE_SUCCESS == dsErrno)
  {
    /*-----------------------------------------------------------------------
      Update dsErrnoPtr appropriately based on state
    -----------------------------------------------------------------------*/
    dsErrno = GetDSErrnoForConnect( &v6RemoteAddr);
  }

  return dsErrno;
} /* Socket::Connect() */


ds::ErrorType Socket::Listen
(
  int32  _backlog
)
{
  ps_socket_cmd_type *  closePSCmdBufPtr   = 0;
  ds::ErrorType         dsErrno;
  int32                 count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::Listen(): "
                           "Sock %d, backlog %d, state %d",
                           platformSockHandle, _backlog, state);

  /*-------------------------------------------------------------------------
    Listen operation is supported only if Connect/Listen operations were not
    performed on the socket before. So ensure that socket is in INIT state
  -------------------------------------------------------------------------*/
  if (State::INIT != state)
  {
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Make sure that socket is bound to a port, otherwise Listen operation is
    not allowed
  -------------------------------------------------------------------------*/
  if (0 == v6LocalSockAddr.port)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Listen(): "
                            "Sock %d is not bound", platformSockHandle);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    Since listening socket doesn't have its own TCP context, close PCB
  -------------------------------------------------------------------------*/
  closePSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_CLOSE);
  if (0 == closePSCmdBufPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    return dsErrno;
  }

  /*-------------------------------------------------------------------------
    Change state to LISTEN.
  -------------------------------------------------------------------------*/
  SetState( State::LISTEN);

  closePSCmdBufPtr->user_data.silent_close_flag = TRUE;
  ps_socket_cmd_send( &closePSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  pcbHandle = 0;
  backlog   = _backlog;

  /*-------------------------------------------------------------------------
    Allocate backlog number of platform sockets and keep them in partial Q
  -------------------------------------------------------------------------*/
  for (count = 0; count < backlog; count ++)
  {
    dsErrno = AllocListener();
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO2_1("Socket::Listen(): "
                      "AllocListener failed, sock %d", platformSockHandle);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Release reader and writer
  -------------------------------------------------------------------------*/
  rcvQReaderPtr->ReleaseReader();
  rcvQReaderPtr = 0;
  sndQWriterPtr->ReleaseWriter();
  sndQWriterPtr = 0;

  /*-------------------------------------------------------------------------
    Reset LISTEN_NOT_PERFORMED since Listen() is complete
  -------------------------------------------------------------------------*/
  ResetEventBitMask( PS::Sock::Platform::Event::ACCEPT, LISTEN_NOT_PERFORMED);

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  /*-------------------------------------------------------------------------
    Free the close CMD buffer
  -------------------------------------------------------------------------*/
  if (0 != closePSCmdBufPtr)
  {
    ps_socket_cmd_free( &closePSCmdBufPtr);
  }

  /*-------------------------------------------------------------------------
    Close the platform socket. Theoretically socket could go back to INIT
    state, but it adds complexity. Most likely app is gonna exit if this
    operation fails and hence it is ok to close the socket

    Increment refCnt so that platform socket is not deleted. Otherwise,
    since sockets library doesn't know that platform socket is deleted, it
    accesses dangling pointer
  -------------------------------------------------------------------------*/
  refCnt++;
  (void) Close();

  return dsErrno;
} /* Socket::Listen() */


ds::ErrorType Socket::Accept
(
 //TODO See if this can be changed to Socket **
  ISocket **             newPlatformSockPtrPtr,
  ds::SockAddrInternalType*    remoteAddrPtr
)
{
  Socket *       acceptPlatformSockPtr;
  ds::ErrorType  dsErrno;
  int32          count;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::Accept(): "
                           "Sock %d, state %d", platformSockHandle, state);

  if (0 == newPlatformSockPtrPtr || 0 == remoteAddrPtr)
  {
    LOG_MSG_ERROR_1("Socket::Accept(): "
                    "NULL args, sock %d", platformSockHandle);
    return AEE_ENOMEMORY;
  }

  /*-------------------------------------------------------------------------
    Accept operation is supported only if Listen operation is performed on the
    socket before. So ensure that socket is in LISTEN state
  -------------------------------------------------------------------------*/
  if (State::LISTEN != state)
  {
    return QDS_EINVAL;
  }

  if (0 != eventBitMask[ PS::Sock::Platform::Event::ACCEPT])
  {
    if (0 == listenList.Count())
    {
      LOG_MSG_INFO1_1("Socket::Accept(): "
                      "Partial Queue is empty, returning ENOMEM, sock %d",
                      platformSockHandle);
      return AEE_ENOMEMORY;
    }
    else
    {
      LOG_MSG_INFO1_2("Socket::Accept(): "
                      "Sock %d blocked for accept, mask 0x%x",
                      platformSockHandle,
                      eventBitMask[ PS::Sock::Platform::Event::ACCEPT]);
      return AEE_EWOULDBLOCK;
    }
  }

  /*-------------------------------------------------------------------------
    Get first platform socket from the accepted list.
  -------------------------------------------------------------------------*/
  acceptPlatformSockPtr = static_cast <Socket *> ( acceptedList.PopFront());
  if (0 == acceptPlatformSockPtr)
  {
    LOG_MSG_ERROR_1("Socket::Accept(): "
                    "Empty PartialQ, sock %d", platformSockHandle);
    dsErrno = QDS_EFAULT;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    PopFront() obtains a new strong reference before returning us the socket.
    Discard it to keep the reference count correct.
  -------------------------------------------------------------------------*/
  (void) acceptPlatformSockPtr->Release();

  /*-------------------------------------------------------------------------
    Set ACCEPTED_LIST_EMPTY if no more connections are available in partial Q
  -------------------------------------------------------------------------*/
  if (0 == acceptedList.Count())
  {
    SetEventBitMask( PS::Sock::Platform::Event::ACCEPT, ACCEPTED_LIST_EMPTY);
  }

  /*-------------------------------------------------------------------------
    Update remoteAddr with peer address
  -------------------------------------------------------------------------*/
  dsErrno =
    AddrUtils::GetSockAddrStorage( &( acceptPlatformSockPtr->v6RemoteSockAddr),
                                   acceptPlatformSockPtr->family,
                                   remoteAddrPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::Accept(): "
                    "GetSockAddrStorage failed, sock %d err 0x%x",
                    platformSockHandle, dsErrno);
    ASSERT( 0);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    As a socket is no longer related to the listening socket once it is
    accepted, reset listenHeadPtr
  -------------------------------------------------------------------------*/
  acceptPlatformSockPtr->listenHeadPtr = 0;

  LOG_MSG_INFO1_1("Socket::Accept(): "
                  "Accepted sock %d",
                  acceptPlatformSockPtr->platformSockHandle);

  /*-------------------------------------------------------------------------
    Fill the partial Q and ensure that it has backlog number of platform
    sockets
  -------------------------------------------------------------------------*/
  for (count = listenList.Count(); count < backlog; count ++)
  {
    dsErrno = AllocListener();
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_1("Socket::Accept(): "
                      "AllocListener failed, sock %d", platformSockHandle);
    }
  }

  *newPlatformSockPtrPtr = acceptPlatformSockPtr;

  LOG_MSG_FUNCTION_EXIT_2("Socket::Accept(): "
                          "Success, sock %d, partial count %d",
                          platformSockHandle, listenList.Count());
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  /*-------------------------------------------------------------------------
    Free accepted sock
  -------------------------------------------------------------------------*/
  if (0 != acceptPlatformSockPtr)
  {
    /*-----------------------------------------------------------------------
      Set linger timeout to 0 to force silent close. Since the socket is
      already accepted, calling Close() without silent close will put the
      socket in CLOSED state. Since the socket library doesn't know about this
      socket, a second Close() will never be called resulting in memory leak
    -----------------------------------------------------------------------*/
    acceptPlatformSockPtr->optCache.soLinger.lingerEnabled = TRUE;
    acceptPlatformSockPtr->optCache.soLinger.timeInSec  = 0;
    (void) acceptPlatformSockPtr->Close();
  }

  return dsErrno;
} /* Socket::Accept() */


ds::ErrorType Socket::GetSockName
(
  ds::SockAddrInternalType*  localAddrPtr
)
throw()
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_EXIT_2("Socket::GetSockName(): "
                          " sock %d, localAddrPtr 0x%x",
                          platformSockHandle, localAddrPtr);


  if (0 == localAddrPtr)
  {
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Translate cached local address in to SockAddrStorageType
  -------------------------------------------------------------------------*/
  dsErrno =
    AddrUtils::GetSockAddrStorage( &v6LocalSockAddr, family, localAddrPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GetSockName(): "
                    "GetSockAddrStorage failed, sock %d err 0x%x",
                    platformSockHandle, dsErrno);
    return dsErrno;
  }

  return AEE_SUCCESS;

} /* Socket::GetSockName() */


ds::ErrorType Socket::GetPeerName
(
  ds::SockAddrInternalType*   remoteAddrPtr
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_EXIT_2("Socket::GetPeerName(): "
                          " sock %d, remoteAddrPtr 0x%x",
                          platformSockHandle, remoteAddrPtr);


  if (0 == remoteAddrPtr)
  {
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Return ENOTCONN if socket is not connected to a peer. Checking if
    remote port is 0 will indicate whether socket is connected (As one can't
    communicate with port 0, port is always non-zero if socket is connected)

    Also check the state so that peer name is not returned when socket is
    closing
  -------------------------------------------------------------------------*/
  if (0 == v6RemoteSockAddr.port ||
      ( State::OPEN != state && State::READ_ONLY != state &&
        State::WRITE_ONLY != state && State::NO_READ_WRITE != state))
  {
    LOG_MSG_INFO1_1("Socket::GetPeerName(): "
                    "Sock %d not connected", platformSockHandle);

    if (State::LISTEN == state)
    {
      return QDS_EINVAL;
    }
    else
    {
      if (PS_PCB_CLOSE_REASON_CODE_NETWORK == pcbClosedReason)
      {
        return QDS_ENETDOWN;
      }
      else
      {
        return QDS_ENOTCONN;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Translate cached peer address in to SockAddrStorageType
  -------------------------------------------------------------------------*/
  dsErrno =
    AddrUtils::GetSockAddrStorage( &v6RemoteSockAddr, family, remoteAddrPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("Socket::GetPeerName(): "
                    "GetSockAddrStorage failed, sock %d err 0x%x",
                    platformSockHandle, dsErrno);
    return dsErrno;
  }

  return AEE_SUCCESS;

} /* Socket::GetPeerName() */


ds::ErrorType Socket::SetOpt
(
  ds::Sock::OptLevelType   optLevel,
  ds::Sock::OptNameType    optName,
  int32                    optVal,
  int32                    optLen
)
{
  ps_socket_cmd_type *  setOptPSCmdBufPtr = 0;
  Socket *              listenPlatformSockPtr;
  ds::ErrorType         dsErrno;
  bool                  postCmdToPS;
  ps_route_scope_type   routeScope;
  Manager *             remoteSockManagerPtr  = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_5("Socket::SetOpt(): "
                           "Sock %d level %d name %d val %d len %d",
                           platformSockHandle,
                           optLevel,
                           optName,
                           optVal,
                           optLen);

  /*-------------------------------------------------------------------------
    Update optCache
  -------------------------------------------------------------------------*/
  switch (optName)
  {
    case ds::Sock::OptName::QDS_SO_SNDBUF:
    case ds::Sock::OptName::QDS_SO_ERROR_ENABLE:
    case ds::Sock::OptName::QDS_SO_REUSEADDR:
    case ds::Sock::OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
    case ds::Sock::OptName::QDS_UDP_ENCAPS:
    {
      postCmdToPS = false;
      break;
    }

    case ds::Sock::OptName::QDS_SO_RCVBUF:    
    case ds::Sock::OptName::QDS_TCP_MAXSEG:    
    case ds::Sock::OptName::QDS_IP_TOS:    
    case ds::Sock::OptName::QDS_IP_TTL:    
    case ds::Sock::OptName::QDS_IP_MULTICAST_TTL:
    case ds::Sock::OptName::QDS_IP_MULTICAST_LOOP:
    case ds::Sock::OptName::QDS_IPV6_TCLASS:
    case ds::Sock::OptName::QDS_SO_KEEPALIVE:
    case ds::Sock::OptName::QDS_TCP_NODELAY:
    case ds::Sock::OptName::QDS_TCP_DELAYED_ACK:
    case ds::Sock::OptName::QDS_TCP_SACK:
    case ds::Sock::OptName::QDS_TCP_TIMESTAMP:
    case ds::Sock::OptName::QDS_IP_RECVIF:
    case ds::Sock::OptName::QDS_IP_RECVERR:
    case ds::Sock::OptName::QDS_IPV6_RECVERR:
    case ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME:
    {      
      postCmdToPS = true;
      break;
    }

    case ds::Sock::OptName::QDS_SO_TX_IFACE:
    case ds::Sock::OptName::QDS_SO_ERROR:
    case ds::Sock::OptName::QDS_TCP_FIONREAD:
    case ds::Sock::OptName::QDS_SO_FIONREAD:
    {
      LOG_MSG_INFO1_1("Socket::SetOpt(): "
                      "SetOpt not supported for %d", optName);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::SetOpt(): "
                      "Unknown opt %d", optName);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }
  } /* switch (optName) */

  /*-------------------------------------------------------------------------
    Check if socket command can be allocated successfully
  -------------------------------------------------------------------------*/
  if (postCmdToPS && 0 != pcbHandle)
  {
    setOptPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_SETOPT);
    if (0 == setOptPSCmdBufPtr)
    {
      dsErrno = AEE_ENOMEMORY;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Update optCache
  -------------------------------------------------------------------------*/
  switch (optName)
  {
    case ds::Sock::OptName::QDS_SO_SNDBUF:
    {
      if (0 == sndQWriterPtr)
      {
        LOG_MSG_INFO1_2( "sndQWriterPtr is NULL, sock %d SetOpt %d",
                         platformSockHandle, optName);
        dsErrno = QDS_EINVAL;
        goto bail;
      }

      optCache.sndBuf = optVal;
      sndQWriterPtr->SetSndBufSize( optCache.sndBuf);    
      break;
    }

    case ds::Sock::OptName::QDS_SO_RCVBUF:
    {
      if (0 == rcvQReaderPtr)
      {
        LOG_MSG_INFO1_2( "rcvQReaderPtr is NULL, sock %d SetOpt %d",
                         platformSockHandle, optName);
        dsErrno = QDS_EINVAL;
        goto bail;
      }
  
      optCache.rcvBuf = optVal;
      rcvQReaderPtr->SetRcvBufSize( optCache.rcvBuf);       
      break;
    }
  
    case ds::Sock::OptName::QDS_TCP_MAXSEG:
    {
      optCache.mss = optVal;        
      break;
    }
  
    case ds::Sock::OptName::QDS_IP_TOS:
    {
      optCache.tos = (uint8) optVal;      
      break;
    }
  
    case ds::Sock::OptName::QDS_IP_TTL:
    {
      optCache.ttl = (uint8) optVal;        
      break;
    }
  
    case ds::Sock::OptName::QDS_IP_MULTICAST_TTL:
    {
      optCache.ttlMcast = (uint8) optVal;     
      break;
    }
  
    case ds::Sock::OptName::QDS_IP_MULTICAST_LOOP:
    {
      optCache.mcastLoop = (uint8) optVal;
      break;
    }
  
    case ds::Sock::OptName::QDS_IPV6_TCLASS:
    {
      optCache.tClass = (uint8) optVal;        
      break;
    }

    case ds::Sock::OptName::QDS_SO_ERROR_ENABLE:
    case ds::Sock::OptName::QDS_SO_REUSEADDR:
    {
      if (0 != optVal)
      {
        SetOptCache( optName);
      }
      else
      {
        ResetOptCache( optName);
      }      
      break;
    }

    case ds::Sock::OptName::QDS_UDP_ENCAPS:
    {
      if (0 != optVal)
      {
        SetOptCache( optName);

        /*-------------------------------------------------------------------
          If socket option was set and if remote socket handle is present
          then ask remote socket manager to send indication to AP to set
          udpe encaps option for that remote socket handle
        -------------------------------------------------------------------*/
        remoteSockManagerPtr = 
          PS::Sock::RemoteSocket::Manager::GetInstance();
        if (0 != remoteSockHandle)
        {
          remoteSockManagerPtr->SendUDPEncapsSockOpt(remoteSockHandle, 
                                                     TRUE);
        }
      }
      else
      {
        ResetOptCache( optName);
      }     

      break;
    }
  
    case ds::Sock::OptName::QDS_SO_KEEPALIVE:
    case ds::Sock::OptName::QDS_TCP_NODELAY:
    case ds::Sock::OptName::QDS_TCP_DELAYED_ACK:
    case ds::Sock::OptName::QDS_TCP_SACK:
    case ds::Sock::OptName::QDS_TCP_TIMESTAMP:
    case ds::Sock::OptName::QDS_IP_RECVIF:
    case ds::Sock::OptName::QDS_IP_RECVERR:
    case ds::Sock::OptName::QDS_IPV6_RECVERR:
    {
      if (0 != optVal)
      {
        SetOptCache( optName);
      }
      else
      {
        ResetOptCache( optName);
      }     
      break;
    }
  
    case ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME:
    {
      if(0 == optVal)
      {
        optVal = TCP_MAX_TOTAL_BACKOFF_TIME;
      }

      optCache.maxBackoffTime = (int32)optVal;       
      break;
    }      

    case ds::Sock::OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
    {
      /* If it is TCP socket, make sure it is in LISTEN state, otherwise
         return error */
      if (Protocol::QDS_TCP == protocol)
      {
        if (State::LISTEN != state)
        {
          LOG_MSG_ERROR_2("Socket::SetOpt(): QDS_SO_SKIP_ROUTE_SCOPE_CHECK: "
                          "TCP socket is not in LISTEN state: sock %d TCP state %d",
                          platformSockHandle, state);
          dsErrno = QDS_EINVAL;
          goto bail;
        }
      }

      /* Check if we have to set the socket route scope to all 1s */
      if (optVal != TRUE)
      {
        LOG_MSG_ERROR_1("Socket::SetOpt(): QDS_SO_SKIP_ROUTE_SCOPE_CHECK: "
                        "Invalid value to set: sock %d", platformSockHandle);
        dsErrno = QDS_EINVAL;
        goto bail;
      }
      else
      {
         /* Set all the route scope bits to 1s */
         for (uint32 index = 0; index < PS_MAX_ROUTE_SCOPE_INDEX; index++)
         {
           /* -1 is equivalent to 0xffffffff */
           routeScope.if_map[index] = -1;
         }
      }

      /* Set route scope back to the socket */
      dsErrno = SetRouteScope(&routeScope);
      if (AEE_SUCCESS != dsErrno)
      {
        LOG_MSG_ERROR_2("Socket::SetOpt(): QDS_SO_SKIP_ROUTE_SCOPE_CHECK: "
                        "SetRouteScope() operation failed sock %d error no %d",
                        platformSockHandle, dsErrno);
        goto bail;
      }

      /* Now set the corresponding socket option cache */
      SetOptCache(optName);
      break;
	 }
  
    default:
    {
      dsErrno = QDS_EOPNOTSUPP;
      LOG_MSG_ERROR_2("Socket::SetOpt(): "
                      "Unknown opt %d error no %d", optName, dsErrno);
      goto bail;
    }
  } /* switch (optName) */

  /*-------------------------------------------------------------------------
    Post cmd to PS if option need to be set in Protocol Stack as well
  -------------------------------------------------------------------------*/
  if (postCmdToPS && 0 != pcbHandle && 0 != setOptPSCmdBufPtr)
  {
    setOptPSCmdBufPtr->user_data.sock_opt_data.level = optLevel;
    setOptPSCmdBufPtr->user_data.sock_opt_data.name  = optName;
    setOptPSCmdBufPtr->user_data.sock_opt_data.val   = optVal;
    setOptPSCmdBufPtr->user_data.sock_opt_data.len   = optLen;

    ps_socket_cmd_send( &setOptPSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);
  }

  /*-------------------------------------------------------------------------
    If this is a listening socket, set option for all sockets in listen Q
  -------------------------------------------------------------------------*/
  if (State::LISTEN == state)
  {
    for (listenList.First(); !listenList.IsDone(); listenList.Next())
    {
      listenPlatformSockPtr =
        static_cast <Socket *> ( listenList.CurrentItem());

      if( 0 == listenPlatformSockPtr)
      {
        continue;
      }

      if (TRUE == listenPlatformSockPtr->GetStrongRef())
      {
        //TODO Err handling if this fails???
        (void) listenPlatformSockPtr->SetOpt( optLevel, optName, optVal, optLen);
        (void) listenPlatformSockPtr->Release();
      }
    }
  }

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free the cmd buffer
  -------------------------------------------------------------------------*/
bail:
  if (0 != setOptPSCmdBufPtr)
  {
    ps_socket_cmd_free( &setOptPSCmdBufPtr);
  }

  return dsErrno;
} /* Socket::SetOpt() */


ds::ErrorType Socket::GetOpt
(
  ds::Sock::OptLevelType   optLevel,
  ds::Sock::OptNameType    optName,
  int32 *                  optValPtr,
  int32 *                  optLenPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == optValPtr || 0 == optLenPtr)
  {
    LOG_MSG_ERROR_1("Socket::GetOpt(): "
                    "NULL args, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  switch (optName)
  {
    case ds::Sock::OptName::QDS_SO_SNDBUF:
    {
      *optValPtr = optCache.sndBuf;
      break;
    }

    case ds::Sock::OptName::QDS_SO_RCVBUF:
    {
      *optValPtr = optCache.rcvBuf;
      break;
    }

    case ds::Sock::OptName::QDS_TCP_MAXSEG:
    {
      *optValPtr = optCache.mss;
      break;
    }

    case ds::Sock::OptName::QDS_IP_TOS:
    {
      *optValPtr = optCache.tos;
      break;
    }

    case ds::Sock::OptName::QDS_IP_TTL:
    {
      *optValPtr = optCache.ttl;
      break;
    }

    case ds::Sock::OptName::QDS_IP_MULTICAST_TTL:
    {
      *optValPtr = optCache.ttlMcast;
      break;
    }

    case ds::Sock::OptName::QDS_IP_MULTICAST_LOOP:
    {
      *optValPtr = optCache.mcastLoop;
      break;
    }

    case ds::Sock::OptName::QDS_IPV6_TCLASS:
    {
      *optValPtr = optCache.tClass;
      break;
    }

    case ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME:
    {
      *optValPtr = (int32) optCache.maxBackoffTime;
      break;
    }

    case ds::Sock::OptName::QDS_TCP_FIONREAD:
    case ds::Sock::OptName::QDS_SO_FIONREAD:
    {
      if (0 == rcvQReaderPtr)
      {
        LOG_MSG_INFO1_2( "rcvQReaderPtr is NULL, sock %d GetOpt %d",
                         platformSockHandle, optName);
        return QDS_EINVAL;
      }
      *optValPtr = rcvQReaderPtr->GetRcvCnt();
      break;
    }

    case ds::Sock::OptName::QDS_SO_ERROR:
    {
      if (IsOptCacheSet( ds::Sock::OptName::QDS_SO_ERROR_ENABLE))
      {
        *optValPtr = soError;
        soError = 0;
        ResetEventBitMask( PS::Sock::Platform::Event::WRITE,
                           WR_ERROR_AVAILABLE);
        ResetRDErrorAvailableEventBitMask();
      }
      else
      {
        LOG_MSG_INFO1_1("Socket::GetOpt(): "
                        "soError is not set, sock %d", platformSockHandle);
        return QDS_EFAULT;
      }

      break;
    }

    case ds::Sock::OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
    {
      /* Return the current socket route scope value */
      *optValPtr = IsOptCacheSet(optName);
      break;
    }
    case ds::Sock::OptName::QDS_SO_ERROR_ENABLE:
    case ds::Sock::OptName::QDS_SO_REUSEADDR:
    case ds::Sock::OptName::QDS_SO_KEEPALIVE:
    case ds::Sock::OptName::QDS_TCP_NODELAY:
    case ds::Sock::OptName::QDS_TCP_DELAYED_ACK:
    case ds::Sock::OptName::QDS_TCP_SACK:
    case ds::Sock::OptName::QDS_TCP_TIMESTAMP:
    case ds::Sock::OptName::QDS_IP_RECVIF:
    case ds::Sock::OptName::QDS_IP_RECVERR:
    case ds::Sock::OptName::QDS_IPV6_RECVERR:
    case ds::Sock::OptName::QDS_UDP_ENCAPS:
    {
      *optValPtr = IsOptCacheSet( optName);
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::GetOpt(): "
                      "Unknown opt %d", optName);
      return QDS_EOPNOTSUPP;
    }
  } /* switch (optName) */

  *optLenPtr = sizeof( int32);

  LOG_MSG_FUNCTION_EXIT_4("Socket::GetOpt(): "
                          "Success, sock %d,level, %d name %d, optVal %d",
                           platformSockHandle, optLevel, optName, *optValPtr);
  return AEE_SUCCESS;

} /* Socket::GetOpt() */

ds::ErrorType Socket::SetOptPriv
(
  ds::Sock::OptLevelType    optLevel,
  ds::Sock::OptNameTypePriv optName,
  int32                     optVal,
  int32                     optLen
)
{
  ps_socket_cmd_type *  setOptPSCmdBufPtr = 0;
  Socket *              listenPlatformSockPtr;
  ds::ErrorType         dsErrno;
  bool                  postCmdToPS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_5("Socket::SetOptPriv(): "
                           "Sock %d level %d name %d val %d len %d",
                           platformSockHandle,
                           optLevel,
                           optName,
                           optVal,
                           optLen);

  /*-------------------------------------------------------------------------
    Update optPrivCache
  -------------------------------------------------------------------------*/
  switch (optName)
  {
    case ds::Sock::OptNamePriv::QDS_ICMP_TYPE:
    case ds::Sock::OptNamePriv::QDS_ICMP_CODE:
    case ds::Sock::OptNamePriv::QDS_ICMP_ECHO_ID:
    case ds::Sock::OptNamePriv::QDS_ICMP_ECHO_SEQ_NUM:
    {      
      postCmdToPS = true;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::SetOptPriv(): "
                      "Unknown opt %d", optName);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }
  } /* switch (optName) */

  /*-------------------------------------------------------------------------
    Post cmd to PS if option need to be set in Protocol Stack as well
  -------------------------------------------------------------------------*/
  if (postCmdToPS && 0 != pcbHandle)
  {
    setOptPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_SETOPT);
    if (0 == setOptPSCmdBufPtr)
    {
      dsErrno = AEE_ENOMEMORY;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Update optPrivCache
  -------------------------------------------------------------------------*/
  switch (optName)
  {
    case ds::Sock::OptNamePriv::QDS_ICMP_TYPE:
    {
      optPrivCache.icmpType = (uint8) optVal;
      break;
    }

    case ds::Sock::OptNamePriv::QDS_ICMP_CODE:
    {
      optPrivCache.icmpCode = (uint8) optVal;
      break;
    }

    case ds::Sock::OptNamePriv::QDS_ICMP_ECHO_ID:
    case ds::Sock::OptNamePriv::QDS_ICMP_ECHO_SEQ_NUM:
    {     
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::SetOptPriv(): "
                      "Unknown opt %d", optName);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }
  } /* switch (optName) */

  if (postCmdToPS && 0 != pcbHandle)
  {
    setOptPSCmdBufPtr->user_data.sock_opt_data.level = optLevel;
    setOptPSCmdBufPtr->user_data.sock_opt_data.name  = optName;
    setOptPSCmdBufPtr->user_data.sock_opt_data.val   = optVal;
    setOptPSCmdBufPtr->user_data.sock_opt_data.len   = optLen;

    ps_socket_cmd_send( &setOptPSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);
  }

  /*-------------------------------------------------------------------------
    If this is a listening socket, set option for all sockets in listen Q
  -------------------------------------------------------------------------*/
  if (State::LISTEN == state)
  {
    for (listenList.First(); !listenList.IsDone(); listenList.Next())
    {
      listenPlatformSockPtr =
         static_cast <Socket *> ( listenList.CurrentItem());

      ASSERT( 0 != listenPlatformSockPtr);

      //TODO Err handling if this fails???
      (void) listenPlatformSockPtr->SetOptPriv( optLevel, optName, optVal, optLen);
    }
  }

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free the cmd buffer
  -------------------------------------------------------------------------*/
bail:
  if (0 != setOptPSCmdBufPtr)
  {
    ps_socket_cmd_free( &setOptPSCmdBufPtr);
  }

  return dsErrno;
} /* Socket::SetOptPriv() */

ds::ErrorType Socket::GetOptPriv
(
  ds::Sock::OptLevelType    optLevel,
  ds::Sock::OptNameTypePriv optName,
  int32 *                   optValPtr,
  int32 *                   optLenPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::GetOptPriv(): "
                           "Sock %d level %d name %d",
                           platformSockHandle, optLevel, optName);

  if (0 == optValPtr || 0 == optLenPtr)
  {
    LOG_MSG_ERROR_1("Socket::GetOptPriv(): "
                    "NULL args, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  switch (optName)
  {
    case ds::Sock::OptNamePriv::QDS_ICMP_TYPE:
    {
      *optValPtr = optPrivCache.icmpType;
      break;
    }

    case ds::Sock::OptNamePriv::QDS_ICMP_CODE:
    {
      *optValPtr = optPrivCache.icmpCode;
      break;
    }

    default:
    {
      LOG_MSG_ERROR_1("Socket::GetOptPriv(): "
                      "Unknown opt %d", optName);
      return QDS_EOPNOTSUPP;
    }
  } /* switch (optName) */

  *optLenPtr = sizeof( int32);

  return AEE_SUCCESS;

} /* Socket::GetOptPriv() */

ds::ErrorType Socket::SetSOLingerReset
(
  const LingerType *  soLingerPtr
)
throw()
{
  ps_socket_cmd_type *  lingerResetPSCmdBufPtr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == soLingerPtr)
  {
    LOG_MSG_ERROR_1("Socket::SetSOLingerReset(): "
                    "NULL linger val, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  if (State::LISTEN == state || State::CLOSED == state)
  {
    LOG_MSG_ERROR_1("Socket::SetSOLingerReset(): "
                    "Invalid sock state %d", state);
    return QDS_EINVAL;
  }

  if (TRUE == optCache.soLinger.lingerEnabled)
  {
    LOG_MSG_INVALID_INPUT_1("Socket::SetSOLingerReset(): "
                            "Linger is enabled, sock %d", platformSockHandle);
    return QDS_EINVAL;
  }

  /*-------------------------------------------------------------------------
    This option makes sense for TCP sockets only as, for other kind of sockets,
    there is no 3-way handshake for socket teardown.
  -------------------------------------------------------------------------*/
  if (Protocol::QDS_TCP == protocol)
  {
    /*-----------------------------------------------------------------------
      Post a cmd to PS
    -----------------------------------------------------------------------*/
    lingerResetPSCmdBufPtr = ps_socket_cmd_alloc(PS_SOCKET_CMD_LINGER_RESET);
    if (0 == lingerResetPSCmdBufPtr)
    {
      return AEE_ENOMEMORY;
    }

    lingerResetPSCmdBufPtr->user_data.linger_reset_data.is_linger_on =
      soLingerPtr->lingerEnabled;
    lingerResetPSCmdBufPtr->user_data.linger_reset_data.time_in_sec =
      soLingerPtr->timeInSec;

    LOG_MSG_INFO1_3("Socket::SetSOLingerReset(): "
                    "sock %d, Linger enabled %d, time %d",
                    platformSockHandle, soLingerPtr->lingerEnabled,
                    soLingerPtr->timeInSec);

    ps_socket_cmd_send( &lingerResetPSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);
  }

  optCache.soLinger = *soLingerPtr;

  return AEE_SUCCESS;

} /* Socket::SetSOLingerReset() */


ds::ErrorType Socket::GetSOLingerReset
(
  LingerType *  soLingerPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2("Socket::GetSOLingerReset(): "
                  " sock %d, soLingerPtr 0x%x",
                  platformSockHandle, soLingerPtr);

  if (0 == soLingerPtr)
  {
    return QDS_EFAULT;
  }

  *soLingerPtr = optCache.soLinger;

  return AEE_SUCCESS;

} /* Socket::GetSOLingerReset() */


ds::ErrorType Socket::SetRtMetaInfo
(
  ps_rt_meta_info_type *  rtMetaInfoPtr
)
{
  ps_socket_cmd_type *    rtPSCmdBufPtr = 0;
  ps_rt_meta_info_type *  dupRtMetaInfoPtr = 0;
  ds::ErrorType           dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == rtMetaInfoPtr)
  {
    LOG_MSG_ERROR_0("Socket::SetRtMetaInfo(): "
                    "NULL rt meta info");
    dsErrno = QDS_EFAULT;
    goto bail;
  }

  if (0 == pcbHandle)
  {
    LOG_MSG_ERROR_1("Socket::SetRtMetaInfo(): "
                    "PCB is already freed, sock %d", platformSockHandle);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Dup the meta info, otherwise following scenario will happen

    1. Socket's routing cache is NULL
    2. App writes a data packet, and sockets library performs routing
    3. Sockets library calls SetRtMetaInfo and a command is posted to PS
    4. PS updates IPCB with rtMetaInfoPtr
    5. App writes few more data packets with the same (dest addr, dest port)
       as the first packet and commands are posted to PS (commands are not
       yet processed by PS)
    6. App writes a packet with different (dest addr, dest port)
    7. Sockets library performs routing since pktInfo differs and calls
       ps_mem_free() on the rtMetaInfoPtr
    8. Since ref cnt is only 1, rtMetaInfoPtr is freed and IPCB contains a
       dangling pointer
    9. When PS gets to process data, it tries to send data out with a dangling
       rtMetaInfoPtr which could have been assigned to somebody else
  -------------------------------------------------------------------------*/
  PS_RT_META_INFO_DUP( rtMetaInfoPtr, &dupRtMetaInfoPtr);

  /*-------------------------------------------------------------------------
    Post cmd to PS
  -------------------------------------------------------------------------*/
  rtPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_SET_ROUTE_META_INFO);
  if (0 == rtPSCmdBufPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  rtPSCmdBufPtr->user_data.rt_meta_info_ptr = dupRtMetaInfoPtr;
  ps_socket_cmd_send( &rtPSCmdBufPtr, platformSockHandle, pcbHandle, protocol);

  LOG_MSG_FUNCTION_EXIT_1("Socket::SetRtMetaInfo(): "
                          "Success, sock %d", platformSockHandle);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free rt meta info
  -------------------------------------------------------------------------*/
bail:
  PS_RT_META_INFO_FREE( &dupRtMetaInfoPtr);
  return dsErrno;

} /* Socket::SetRtMetaInfo() */


ds::ErrorType Socket::SetRouteScope
(
  ps_route_scope_type *  newRouteScopePtr
)
{
  ps_socket_cmd_type *  setRouteScopePSCmdBufPtr = 0;
  Socket *              listenPlatformSockPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_2( "Socket::SetRouteScope(): "
                   "sock %d, newRouteScopePtr 0x%x",
                    platformSockHandle, newRouteScopePtr);

  if (0 == newRouteScopePtr)
  {
    return QDS_EFAULT;
  }

  /* If route scope skip option is enabled, then we don't need to update
     the route scope */
  if (IsOptCacheSet(OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK))
  {
    LOG_MSG_INFO1_1( "Socket::SetRouteScope(): Skip Route Scope "
                     "option is set. Sock %d", platformSockHandle);
    return AEE_SUCCESS;
  }

  PS_ROUTE_SCOPE_PRINT( newRouteScopePtr);

  routeScope = *newRouteScopePtr;

  if (0 != pcbHandle)
  {
    setRouteScopePSCmdBufPtr =
      ps_socket_cmd_alloc( PS_SOCKET_CMD_SET_ROUTE_SCOPE);

    if (0 == setRouteScopePSCmdBufPtr)
    {
      return AEE_ENOMEMORY;
    }

    setRouteScopePSCmdBufPtr->user_data.route_scope = *newRouteScopePtr;

    ps_socket_cmd_send( &setRouteScopePSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);
  }

  /*-------------------------------------------------------------------------
    If this is a listening socket, set option for all sockets in listen Q
  -------------------------------------------------------------------------*/
  if (State::LISTEN == state)
  {
    for (listenList.First(); !listenList.IsDone(); listenList.Next())
    {
      listenPlatformSockPtr =
        static_cast <Socket *> ( listenList.CurrentItem());

      if (0 == listenPlatformSockPtr)
      {
        continue;
      }

      if (TRUE == listenPlatformSockPtr->GetStrongRef())
      {
        (void) listenPlatformSockPtr->SetRouteScope( newRouteScopePtr);
        (void) listenPlatformSockPtr->Release();
      }
    }
  }

  return AEE_SUCCESS;

} /* Socket::SetRouteScope() */


ds::ErrorType Socket::GetRouteScope
(
  ps_route_scope_type *  routeScopePtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == routeScopePtr)
  {
    LOG_MSG_ERROR_0( "NULL args");
    return QDS_EFAULT;
  }

  *routeScopePtr = routeScope;

  return AEE_SUCCESS;

} /* Socket::GetRouteScope() */


void Socket::SetSystemOption
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  isSystemSocket = true;

  return;

} /* Socket::SetSystemOption() */


ds::ErrorType Socket::SendMsg
(
  const ds::SockAddrInternalType*    remoteAddrPtr,
  const IPort1::SeqBytes             ioVecArr[],
  int32                              numIOVec,
  int32 *                            numWrittenPtr,
  IAncDataPriv **                    inAncillaryDataPtrPtr,
  int32                              inAncillaryDataLen,
  int32                              flags,
  ps_pkt_meta_info_type *            pktMetaInfoPtr
)
{
  ps_pkt_meta_info_type *  dupPktMetaInfoPtr = 0;
  ps_socket_cmd_type *     writePSCmdBufPtr = 0;
  ds::SockAddrIN6InternalType *    v6RemoteAddrPtr;
  ds::SockAddrInternalType       tmpLocalAddr = {{0}}; // for bind to port 0
  ds::SockAddrIN6InternalType      tmpV6RemoteAddr = {0};
  ds::ErrorType            dsErrno;
  int32                    writeEventBitMask;
  int32                    retVal;
  int16                    psErrno = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("Socket::SendMsg(): "
                  "Sock %d", platformSockHandle);

  if (0 == numWrittenPtr)
  {
    LOG_MSG_ERROR_1("Socket::SendMsg(): "
                    "NULL arg, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    If socket is already connected, make sure that passed in address is same
    as the cached peer addr
  -------------------------------------------------------------------------*/
  if (0 != v6RemoteSockAddr.port && 0 != remoteAddrPtr)
  {
    dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, &tmpV6RemoteAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendMsg(): "
                      "GetSockAddrIN6 failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      goto bail;
    }

    if (false == PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr, &ps_in6addr_any))
    {
      if (tmpV6RemoteAddr.port != v6RemoteSockAddr.port ||
          false == PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr,
                                          tmpV6RemoteAddr.addr))
      {
        LOG_MSG_INVALID_INPUT_1("Socket::SendMsg(): "
                                "Sock %d is connected", platformSockHandle);
        dsErrno = QDS_EISCONN;
        goto bail;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Return error if socket is not writeable
  -------------------------------------------------------------------------*/
  writeEventBitMask = eventBitMask[ PS::Sock::Platform::Event::WRITE];
  if (0 != writeEventBitMask)
  {
    dsErrno = GetDSErrnoForSockOutput();
    LOG_MSG_INFO1_3("Socket::SendMsg(): "
                    "Sock %d not writeable, mask 0x%x err 0x%x",
                    platformSockHandle, writeEventBitMask, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Generate ephemeral port if socket is not bound already
  -------------------------------------------------------------------------*/
  if (0 == v6LocalSockAddr.port)
  {
    (void)ds::Sock::AddrUtils::SetFamily(&tmpLocalAddr,family);

    /*-----------------------------------------------------------------------
      Return AEE_EWOULDBLOCK if Bind fails because of lack of PS cmd buffers.
      App will be notified with WRITE_EV when cmd buffers are available and
      will be again able to write data
    -----------------------------------------------------------------------*/
    dsErrno = Bind( &tmpLocalAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendMsg(): "
                      "Bind failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      if (AEE_ENOMEMORY == dsErrno)
      {
        dsErrno = AEE_EWOULDBLOCK;
      }

      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Return EWOULDBLOCK if PS is out of cmd buffers. App will be notified with
    WRITE_EV when cmd buffers are available and will be again able to write
    data
  -------------------------------------------------------------------------*/
  writePSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_PAYLOAD);
  if (0 == writePSCmdBufPtr)
  {
    dsErrno = AEE_EWOULDBLOCK;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Write data to the data pipe
  -------------------------------------------------------------------------*/
  retVal = sndQWriterPtr->Write( ioVecArr, numIOVec, 0, 0, &psErrno);
  if (-1 == retVal)
  {
    /*-----------------------------------------------------------------------
      IWriter returns AEE_EWOULDBLOCK when data pipe is full. So set
      SNDQ_FULL event bit mask
    -----------------------------------------------------------------------*/
    if (DS_EWOULDBLOCK == psErrno)
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, SNDQ_FULL);
    }

    dsErrno = PSErrnoGetDSErrno( psErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Update Write PS cmd buffer with remote addr
  -------------------------------------------------------------------------*/
  if (0 != remoteAddrPtr)
  {
    v6RemoteAddrPtr =
      reinterpret_cast <ds::SockAddrIN6InternalType *>
        ( &( writePSCmdBufPtr->user_data.write_data.dest_addr));

    dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, v6RemoteAddrPtr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendMsg(): "
                      "GetSockAddrIN6 failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      goto bail;
    }
  }
  else
  {
    memset( &( writePSCmdBufPtr->user_data.write_data.dest_addr),
            0,
            sizeof( struct ps_sockaddr_in6));
  }

  /*-------------------------------------------------------------------------
    Post write cmd to PS
  -------------------------------------------------------------------------*/
  PS_PKT_META_INFO_DUP( pktMetaInfoPtr, &dupPktMetaInfoPtr);

  writePSCmdBufPtr->user_data.write_data.length            = retVal;
  writePSCmdBufPtr->user_data.write_data.pkt_meta_info_ptr = dupPktMetaInfoPtr;

  ps_socket_cmd_send( &writePSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  *numWrittenPtr = retVal;
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free the cmd buffer
  -------------------------------------------------------------------------*/
bail:
  if (0 != writePSCmdBufPtr)
  {
    ps_socket_cmd_free( &writePSCmdBufPtr);
  }

  PS_PKT_META_INFO_FREE( &dupPktMetaInfoPtr);

  return dsErrno;
} /* Socket::SendMsg() */


ds::ErrorType Socket::SendDSMChain
(
  const ds::SockAddrInternalType*    remoteAddrPtr,
  dsm_item_type **             dsmItemPtrPtr,
  int32                        flags,
  int32 *                      numWrittenPtr
)
{
  ps_socket_cmd_type *      writePSCmdBufPtr = 0;
  ds::SockAddrIN6InternalType *     v6RemoteAddrPtr;
  ds::SockAddrInternalType        tmpLocalAddr = {{0}};
  ds::SockAddrIN6InternalType       tmpV6RemoteAddr = {0};
  ds::ErrorType             dsErrno;
  int32                     writeEventBitMask;
  int32                     retVal;
  int16                     psErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("Socket::SendDSMChain(): "
                  "Sock %d", platformSockHandle);

  if (0 == numWrittenPtr)
  {
    LOG_MSG_ERROR_1("Socket::SendDSMChain(): "
                    "NULL arg, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    If socket is already connected, make sure that passed in address is same
    as the cached peer addr
  -------------------------------------------------------------------------*/
  if (0 != v6RemoteSockAddr.port && 0 != remoteAddrPtr)
  {
    dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, &tmpV6RemoteAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendDSMChain(): "
                      "GetSockAddrIN6 failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      goto bail;
    }

    if (false == PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr, &ps_in6addr_any))
    {
      if (tmpV6RemoteAddr.port != v6RemoteSockAddr.port ||
          false == PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr,
                                          tmpV6RemoteAddr.addr))
      {
        LOG_MSG_INVALID_INPUT_1("Socket::SendDSMChain(): "
                                "Sock %d is connected", platformSockHandle);
        dsErrno = QDS_EISCONN;
        goto bail;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Return error if socket is not writeable
  -------------------------------------------------------------------------*/
  writeEventBitMask = eventBitMask[ PS::Sock::Platform::Event::WRITE];
  if (0 != writeEventBitMask)
  {
    dsErrno = GetDSErrnoForSockOutput();
    LOG_MSG_INFO1_3("Socket::SendDSMChain(): "
                    "Sock %d not writeable, mask 0x%x err 0x%x",
                    platformSockHandle, writeEventBitMask, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Generate ephemeral port if socket is not bound already
  -------------------------------------------------------------------------*/
  //TODO Think about handling this as part of sock not writeable mask
  if (0 == v6LocalSockAddr.port)
  {

    (void)ds::Sock::AddrUtils::SetFamily(&tmpLocalAddr,family);

    /*-----------------------------------------------------------------------
      Return AEE_EWOULDBLOCK if Bind fails because of lack of PS cmd buffers.
      App will be notified with WRITE_EV when cmd buffers are available and
      will be again able to write data
    -----------------------------------------------------------------------*/
    dsErrno = Bind(&tmpLocalAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendDSMChain(): "
                      "Bind failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      if (AEE_ENOMEMORY == dsErrno)
      {
        dsErrno = AEE_EWOULDBLOCK;
      }

      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Return AEE_EWOULDBLOCK if PS is out of cmd buffers. App will be notified
    with WRITE_EV when cmd buffers are available and will be again able to
    write data
  -------------------------------------------------------------------------*/
  writePSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_PAYLOAD);
  if (0 == writePSCmdBufPtr)
  {
    dsErrno = AEE_EWOULDBLOCK;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Write data to the data pipe
  -------------------------------------------------------------------------*/
  retVal = sndQWriterPtr->Write( dsmItemPtrPtr,
                                 dsm_length_packet( *dsmItemPtrPtr),
                                 0,
                                 0,
                                 &psErrno);
  if (-1 == retVal)
  {
    /*-----------------------------------------------------------------------
      IWriter returns AEE_EWOULDBLOCK when data pipe is full. So set
      SNDQ_FULL event bit mask
    -----------------------------------------------------------------------*/
    if (DS_EWOULDBLOCK == psErrno)
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, SNDQ_FULL);
    }

    dsErrno = PSErrnoGetDSErrno( psErrno);
  }

  /*-------------------------------------------------------------------------
    Update Write PS cmd buffer with remote addr
  -------------------------------------------------------------------------*/
  if (0 != remoteAddrPtr)
  {
    v6RemoteAddrPtr =
      reinterpret_cast <ds::SockAddrIN6InternalType *>
        ( &( writePSCmdBufPtr->user_data.write_data.dest_addr));

    dsErrno = AddrUtils::GetSockAddrIN6( remoteAddrPtr, v6RemoteAddrPtr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("Socket::SendDSMChain(): "
                      "GetSockAddrIN6 failed, sock %d err 0x%x",
                      platformSockHandle, dsErrno);
      goto bail;
    }
  }
  else
  {
    memset(&(writePSCmdBufPtr->user_data.write_data.dest_addr), 0,
           sizeof(struct ps_sockaddr_in6));
  }

  writePSCmdBufPtr->user_data.write_data.pkt_meta_info_ptr = 0;

  /*-------------------------------------------------------------------------
    Post write cmd to PS
  -------------------------------------------------------------------------*/
  writePSCmdBufPtr->user_data.write_data.length = retVal;
  ps_socket_cmd_send( &writePSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  *numWrittenPtr = retVal;
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free the cmd buffer
  -------------------------------------------------------------------------*/
bail:
  if (0 != writePSCmdBufPtr)
  {
    ps_socket_cmd_free( &writePSCmdBufPtr);
  }

  return dsErrno;
} /* Socket::SendDSMChain() */


ds::ErrorType Socket::RecvMsg
(
  ds::SockAddrInternalType*      remoteAddrPtr,
  IPort1::SeqBytes               ioVecArr[],
  int32                          numIOVec,
  int32 *                        numReadPtr,
  IAncDataPriv *                 outAncillaryDataPtr[],
  int32                          outAncillaryDataLen,
  int32 *                        outAncillaryDataLenReqPtr,
  uint32 *                       outFlagsPtr,
  int32                          flags
)
{
  ds::SockAddrIN6InternalType *   tmpV6RemoteSockAddrPtr;
  ps_icmp_errq_meta_info  icmpErrQMetaInfo;
  ps_pkt_read_info        readInfo;
  ds::ErrorType           dsErrno;
  int32                   icmpErrMetaDataLen = sizeof( ps_icmp_errq_meta_info);
  int32                   metaDataLen        = sizeof( ps_pkt_read_info);
  int32                   readEventBitMask;
  uint16                  pktCnt;
  int16                   psErrno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("Socket::RecvMsg(): "
                  "Sock %d", platformSockHandle);

  if (0 == numReadPtr)
  {
    LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                    "NULL arg, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  if (0 != outFlagsPtr)
  {
    *outFlagsPtr = 0;
  }

  /*-------------------------------------------------------------------------
    Return error if socket is not readable. Note that RD_ERROR_AVAILABLE
    doesn't make the socket unreadable
  -------------------------------------------------------------------------*/
  readEventBitMask = eventBitMask[ PS::Sock::Platform::Event::READ];
  if (0 != readEventBitMask)
  {
    if (0 == ( RD_ERROR_AVAILABLE & readEventBitMask))
    {
      *numReadPtr = 0;
      dsErrno = GetDSErrnoForSockInput();
      LOG_MSG_INFO1_3("Socket::RecvMsg(): "
                      "Sock %d not readable, mask 0x%x err 0x%x",
                      platformSockHandle, readEventBitMask, dsErrno);
      return dsErrno;
    }
  }

  /*-------------------------------------------------------------------------
    If MSG_ERRQUEUE is set, read from ICMP err Q, else read from data pipe
  -------------------------------------------------------------------------*/
  if (0 != ( flags & RecvFlags::QDS_MSG_ERRQUEUE))
  {
    if (0 == errQReaderPtr)
    {
      LOG_MSG_ERROR_1("Socket::RecvMsg(): "
                      "ErrQ not supported, sock %d", platformSockHandle);
      *numReadPtr = 0;
      return QDS_EOPNOTSUPP;
    }

    *numReadPtr =
      errQReaderPtr->Read( ioVecArr,
                           numIOVec,
                           reinterpret_cast <char *> ( &icmpErrQMetaInfo),
                           &icmpErrMetaDataLen,
                           &psErrno);

    tmpV6RemoteSockAddrPtr = reinterpret_cast <ds::SockAddrIN6InternalType *>
                               ( &( icmpErrQMetaInfo.original_dgram_dest_addr));

    if(0 != outFlagsPtr && DS_EMSGTRUNC == psErrno)
    {
      /* if psErrno is DS_EMSGTRUNC, RecvFrom should return SUCCESS */
      psErrno = 0;
      *outFlagsPtr |= RecvFlags::QDS_MSG_CTRUNC;
    }

    ResetRDErrorAvailableEventBitMask();
  }
  else
  {
    *numReadPtr = rcvQReaderPtr->Read( ioVecArr,
                                       numIOVec,
                                       reinterpret_cast <char *> ( &readInfo),
                                       &metaDataLen,
                                       &psErrno);

    tmpV6RemoteSockAddrPtr =
       reinterpret_cast <ds::SockAddrIN6InternalType *> ( &( readInfo.v6RemoteSockAddr));

    if(0 != outFlagsPtr && DS_EMSGTRUNC == psErrno)
    {
      /* if psErrno is DS_EMSGTRUNC, RecvFrom should return SUCCESS */
      psErrno = 0;
      *outFlagsPtr |= RecvFlags::QDS_MSG_TRUNC;
    }
  }

  if (0 <= *numReadPtr)
  {
    /*-----------------------------------------------------------------------
      Process meta data
    -----------------------------------------------------------------------*/
    if (0 != metaDataLen)
    {
      /*---------------------------------------------------------------------
        Fill remoteAddrPtr
      ---------------------------------------------------------------------*/
      if (0 != remoteAddrPtr)
      {
        dsErrno = AddrUtils::GetSockAddrStorage( tmpV6RemoteSockAddrPtr,
                                                 family,
                                                 remoteAddrPtr);
        if (AEE_SUCCESS != dsErrno)
        {
          LOG_MSG_INFO3_2("Socket::RecvMsg(): "
                          "GetSockAddrStorage failed, sock %d err 0x%x",
                          platformSockHandle, dsErrno);
          return dsErrno;
        }
      }

      if (0 != outAncillaryDataPtr && outAncillaryDataLen > 0 &&
          0 != outAncillaryDataLenReqPtr)
      {
        ProcessAncillaryData( outAncillaryDataPtr,
                              outAncillaryDataLen,
                              flags,
                              &icmpErrQMetaInfo,
                              &readInfo,
                              outAncillaryDataLenReqPtr);
      }
    }

    /*-----------------------------------------------------------------------
      Even when IReader returns success, dsErrno needs to be updated as
      IReader returns 0 and sets psErrno to QDS_EEOF to indicate end of file
    -----------------------------------------------------------------------*/
    dsErrno = PSErrnoGetDSErrno( psErrno);
  }
  else
  {
    /*-----------------------------------------------------------------------
      Return soError if set
    -----------------------------------------------------------------------*/
    if (DS_EWOULDBLOCK == psErrno &&
        IsOptCacheSet( ds::Sock::OptName::QDS_SO_ERROR_ENABLE) &&
        0 != soError)
    {
      dsErrno = soError;
      soError = 0;

      ResetEventBitMask( PS::Sock::Platform::Event::WRITE,
                         WR_ERROR_AVAILABLE);
      ResetRDErrorAvailableEventBitMask();
    }
    else
    {
      dsErrno = PSErrnoGetDSErrno( psErrno);
    }
  }

  if (0 != outFlagsPtr)
  {
    if (0 == errQReaderPtr)
    {
      LOG_MSG_INFO1_1("Socket::RecvMsg(): "
                      "ErrQ not supported, sock %d", platformSockHandle);
    }
    else
    {
      if (-1 == errQReaderPtr->GetPktCnt( &pktCnt, &psErrno))
      {
        LOG_MSG_INFO3_2("Socket::RecvMsg(): "
                        "GetPktCnt failed, sock %d psErr %d",
                        platformSockHandle, psErrno);
        ASSERT( 0);
      }
      else
      {
        if (0 != pktCnt &&
            ( IsOptCacheSet( ds::Sock::OptName::QDS_IP_RECVERR) ||
              IsOptCacheSet( ds::Sock::OptName::QDS_IPV6_RECVERR)))
        {
          *outFlagsPtr |= RecvFlags::QDS_MSG_ERRQUEUE;
        }
      }
    }
  }

  return dsErrno;
} /* Socket::RecvMsg() */


ds::ErrorType Socket::RecvDSMChain
(
  ds::SockAddrInternalType*    remoteAddrPtr,
  dsm_item_type **       dsmItemPtrPtr,
  int32                  flags,
  int32 *                numReadPtr
)
{
  ds::SockAddrIN6InternalType *  tmpV6RemoteSockAddrPtr;
  ps_pkt_read_info       readInfo;
  ds::ErrorType          dsErrno;
  int32                  metaDataLen = sizeof( ps_pkt_read_info);
  int32                  readEventBitMask;
  int16                  psErrno = 0;
/*- - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1("Socket::RecvDSMChain(): "
                  "Sock %d", platformSockHandle);

  if (0 == numReadPtr)
  {
    LOG_MSG_ERROR_1("Socket::RecvDSMChain(): "
                    "NULL arg, sock %d", platformSockHandle);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Return error if socket is not readable
  -------------------------------------------------------------------------*/
  readEventBitMask = eventBitMask[ PS::Sock::Platform::Event::READ];
  if (0 != readEventBitMask)
  {
    *numReadPtr = 0;
    dsErrno = GetDSErrnoForSockInput();
    LOG_MSG_INFO1_3("Socket::RecvDSMChain(): "
                    "Sock %d not readable, mask 0x%x err 0x%x",
                    platformSockHandle, readEventBitMask, dsErrno);
    return dsErrno;
  }

  *numReadPtr = rcvQReaderPtr->Read( dsmItemPtrPtr,
                                     reinterpret_cast <char *> ( &readInfo),
                                     &metaDataLen,
                                     &psErrno);
  if (0 <= *numReadPtr)
  {
    /*-----------------------------------------------------------------------
      Process meta data
    -----------------------------------------------------------------------*/
    if (0 != metaDataLen)
    {
      /*---------------------------------------------------------------------
        Fill remoteAddrPtr
      ---------------------------------------------------------------------*/
      if (0 != remoteAddrPtr)
      {
        tmpV6RemoteSockAddrPtr =
           reinterpret_cast <ds::SockAddrIN6InternalType *> ( &( readInfo.v6RemoteSockAddr));

        dsErrno = AddrUtils::GetSockAddrStorage( tmpV6RemoteSockAddrPtr,
                                                 family,
                                                 remoteAddrPtr);
        if (AEE_SUCCESS != dsErrno)
        {
          LOG_MSG_INFO3_2("Socket::RecvDSMChain(): "
                          "GetSockAddrStorage failed, sock %d err 0x%x",
                          platformSockHandle, dsErrno);
          return dsErrno;
        }
      }
    }

    /*-----------------------------------------------------------------------
      Even when IReader returns success, dsErrno needs to be updated as
      IReader returns 0 and sets psErrno to QDS_EEOF to indicate end of file
    -----------------------------------------------------------------------*/
    dsErrno = PSErrnoGetDSErrno( psErrno);
  }
  else
  {
    /*-----------------------------------------------------------------------
      IReader returns AEE_EWOULDBLOCK when data pipe is empty. So set
      RCVQ_EMPTY event bit mask
    -----------------------------------------------------------------------*/
    if (DS_EWOULDBLOCK == psErrno)
    {
      SetEventBitMask( PS::Sock::Platform::Event::READ, RCVQ_EMPTY);
    }

    dsErrno = PSErrnoGetDSErrno( psErrno);
  }

  return dsErrno;
} /* Socket::RecvDSMChain() */


ds::ErrorType Socket::Shutdown
(
  ShutdownDirType  shutdownDir
)
{
  ps_socket_cmd_type *  shutdownPSCmdBufPtr;
  StateType             newState;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::Shutdown(): "
                           "Sock %d, dir %d, state %d",
                           platformSockHandle, shutdownDir, state);

  /*-------------------------------------------------------------------------
    Make sure that requested shutdown direction is supported in socket's
    current state
  -------------------------------------------------------------------------*/
  if (ShutdownDir::QDS_READ_DIR == shutdownDir)
  {
    switch (state)
    {
      case State::OPEN:
      {
        newState = State::WRITE_ONLY;
        break;
      }

      case State::READ_ONLY:
      {
        newState = State::NO_READ_WRITE;
        break;
      }

      case State::WRITE_ONLY:
      case State::NO_READ_WRITE:
      {
        return AEE_SUCCESS;
      }

      default:
      {
        return QDS_ENOTCONN;
      }
    }
  }
  else if (ShutdownDir::QDS_WRITE_DIR == shutdownDir)
  {
    switch (state)
    {
      case State::OPEN:
      {
        newState = State::READ_ONLY;
        break;
      }

      case State::WRITE_ONLY:
      {
        newState = State::NO_READ_WRITE;
        break;
      }

      case State::READ_ONLY:
      case State::NO_READ_WRITE:
      {
        return AEE_SUCCESS;
      }

      default:
      {
        return QDS_ENOTCONN;
      }
    }
  }
  else
  {
    if (State::NO_READ_WRITE == state)
    {
      return AEE_SUCCESS;
    }
    else if (State::OPEN != state)
    {
      return QDS_ENOTCONN;
    }

    newState = State::NO_READ_WRITE;
  }

  /*-------------------------------------------------------------------------
    Since UDP sockets are always in OPEN state, make sure that it is
    connected by checking if peer address is not IN6_ADDR_ANY
  -------------------------------------------------------------------------*/
  if (PS_IN6_ARE_ADDR_EQUAL( v6RemoteSockAddr.addr, &ps_in6addr_any))
  {
    LOG_MSG_INVALID_INPUT_1("Socket::Shutdown(): "
                            "Sock %d is not connected", platformSockHandle);
    return QDS_ENOTCONN;
  }

  /*-------------------------------------------------------------------------
    Post a cmd to PS to trigger shutdown at the protocol stack layer
  -------------------------------------------------------------------------*/
  shutdownPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_SHUTDOWN);
  if (0 == shutdownPSCmdBufPtr)
  {
    return AEE_ENOMEMORY;
  }

  shutdownPSCmdBufPtr->user_data.shutdown_dir = shutdownDir;
  ps_socket_cmd_send( &shutdownPSCmdBufPtr,
                      platformSockHandle,
                      pcbHandle,
                      protocol);

  /*-------------------------------------------------------------------------
    Since app doesn't want to read any more data, close the reader
  -------------------------------------------------------------------------*/
  if (State::WRITE_ONLY == newState || State::NO_READ_WRITE == newState)
  {
    /*-------------------------------------------------------------------------
      Ensure that rcvQReader is not NULL. Otherwise, page fault occurs if
      application does a WR_SHUTDOWN after doing a RD_SHUTDOWN.
    -------------------------------------------------------------------------*/
    if (0 != rcvQReaderPtr)
    {
      rcvQReaderPtr->ReleaseReader();
      rcvQReaderPtr = 0;
    }
  }

  /*-------------------------------------------------------------------------
    Change state appropriately
  -------------------------------------------------------------------------*/
  SetState( newState);

  LOG_MSG_FUNCTION_EXIT_1("Socket::Shutdown(): "
                          "Success, sock %d", platformSockHandle);
  return AEE_SUCCESS;

} /* Socket::Shutdown() */


ds::ErrorType Socket::Close
(
  void
)
throw()
{
  Socket *              listenPlatformSockPtr;
  Socket *              acceptPlatformSockPtr;
  ps_socket_cmd_type *  closePSCmdBufPtr;
  int32                 tmpPlatformSockHandle;
  ds::ErrorType         dsErrno;
  boolean               silentCloseFlag = TRUE;
  Manager             * remoteSockManagerPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_6("Socket::Close(): "
                           "Sock %d ref cnt %d state %d PCB 0x%x "
                           "linger on %d linger val %d",
                           platformSockHandle,
                           refCnt,
                           state,
                           pcbHandle,
                           optCache.soLinger.lingerEnabled,
                           optCache.soLinger.timeInSec);

  /*-------------------------------------------------------------------------
    Do nothing if socket is already in CLOSING or CLOSED state
  -------------------------------------------------------------------------*/
  if (State::CLOSED == state)
  {
    /*-----------------------------------------------------------------------
      Decrement refCnt since sockets library is not holding pointer to
      platform socket anymore. Delete platform socket if refCnt is 0 as both
      sockets library and PCB are not holding this pointer

      App could call ds::Sock::Socket::Close() either as part of CLOSE_EV
      callback or because app has higher priority than PS. Hence, although
      platform socket goes to CLOSED state only when PCB posts PCB_CLOSED_EV,
      refCnt must be checked before deleting the socket as platform may not
      have finished with processing PCB_CLOSED_EV

      Store platformSockHandle in a temporary variable as once the object is
      deleted, its members are not accessible
    -----------------------------------------------------------------------*/
    tmpPlatformSockHandle = platformSockHandle;

    (void) Release();

    return AEE_SUCCESS;
  }
  else if (State::CLOSING == state)
  {
    if (FALSE == optCache.soLinger.lingerEnabled ||
        0 != optCache.soLinger.timeInSec)
    {
      LOG_MSG_FUNCTION_EXIT_2("Socket::Close(): "
                              "Sock %d ret 0x%x",
                              platformSockHandle, AEE_EWOULDBLOCK);
      return AEE_EWOULDBLOCK;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      If this is a listening socket, silently close
        1. All the sockets which are connected but not yet accepted by the
           application AND
        2. All the sockets in partial Q
    -----------------------------------------------------------------------*/
    if (State::LISTEN == state)
    {
      LOG_MSG_INFO1_2("Socket::Close(): "
                      "Clearing accepted list, sock %d cnt %d",
                      platformSockHandle, acceptedList.Count());

      while (0 != ( acceptPlatformSockPtr =
                      static_cast <Socket *> ( acceptedList.PopFront())))
      {
        /*-------------------------------------------------------------------
          Reset the listenHeadPtr. Otherwise, the module clones a new socket
          upon receiving ProcessPCBClosedEvent() not knowing that the parent
          socket is already closed
        -------------------------------------------------------------------*/
        acceptPlatformSockPtr->listenHeadPtr = 0;

        /*-------------------------------------------------------------------
          Set linger timeout to 0 to force silent close. Since the socket is
          not accepted yet, graceful close is not required
        -------------------------------------------------------------------*/
        acceptPlatformSockPtr->optCache.soLinger.lingerEnabled = TRUE;
        acceptPlatformSockPtr->optCache.soLinger.timeInSec  = 0;
        (void) acceptPlatformSockPtr->Close();
        (void) acceptPlatformSockPtr->Release();
      }

      LOG_MSG_INFO1_2("Socket::Close(): "
                      "Clearing listen list, sock %d cnt %d",
                      platformSockHandle, listenList.Count());

      while (0 != ( listenPlatformSockPtr =
                      static_cast <Socket *> ( listenList.PopFront())))
      {
        /*-------------------------------------------------------------------
          Reset the listenHeadPtr. Otherwise, the module clones a new socket
          upon receiving ProcessPCBClosedEvent() not knowing that the parent
          socket is already closed
        -------------------------------------------------------------------*/
        listenPlatformSockPtr->listenHeadPtr = 0;

        /*-------------------------------------------------------------------
          Set linger timeout to 0 to force silent close. Since the socket is
          still in partial Q, graceful close is not required
        -------------------------------------------------------------------*/
        listenPlatformSockPtr->optCache.soLinger.lingerEnabled = TRUE;
        listenPlatformSockPtr->optCache.soLinger.timeInSec  = 0;
        (void) listenPlatformSockPtr->Close();
        (void) listenPlatformSockPtr->Release();
      }
    }

    /*-----------------------------------------------------------------------
      Socket can be silently closed if
        1. It is a UDP or ICMP socket OR
        2. It is a TCP socket and is not connected yet or is connecting OR
        3. SO_LINGER_RESET option is set and linger timer value is set to 0
    -----------------------------------------------------------------------*/
    if (Protocol::QDS_TCP == protocol &&
        ( State::OPEN == state || State::OPENING == state ||
          State::READ_ONLY == state || State::WRITE_ONLY == state ||
          State::NO_READ_WRITE == state))
    {
      if (FALSE == optCache.soLinger.lingerEnabled ||
          0 != optCache.soLinger.timeInSec)
      {
        silentCloseFlag = FALSE;
      }
    }
  }
  
/*-------------------------------------------------------------------------
    Close the remote socket on AP
  -------------------------------------------------------------------------*/
  remoteSockManagerPtr = PS::Sock::RemoteSocket::Manager::GetInstance();
  remoteSockManagerPtr->ReleaseSocket( GetRemoteSockHandle());  
 
  /*-----------------------------------------------------------------------
    Reset Remote socket handle to zero
  -----------------------------------------------------------------------*/  
  SetRemoteSocktHandle(0);

  /*-------------------------------------------------------------------------
    If PCB is not closed already, post a cmd to PS to trigger close at the
    protocol stack layer
  -------------------------------------------------------------------------*/
  if (0 != pcbHandle)
  {
    closePSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_CLOSE);
    if (0 == closePSCmdBufPtr)
    {
      return AEE_ENOMEMORY;
    }

    closePSCmdBufPtr->user_data.silent_close_flag = silentCloseFlag;
    ps_socket_cmd_send( &closePSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);
  }

  /*-------------------------------------------------------------------------
    Set state to CLOSED if socket is silently closed or if PCB has been
    closed already. Otherwise, return AEE_EWOULDBLOCK and wait for PCB to close
  -------------------------------------------------------------------------*/
  if (0 == pcbHandle || TRUE == silentCloseFlag)
  {
    pcbHandle        = 0;

    /* Release the weak reference to registered event listener */
    if (0 != eventListenerPtr)
    {
      eventListenerPtr->ReleaseWeak();
      eventListenerPtr = 0;
    }

    SetState( State::CLOSED);

    /*-----------------------------------------------------------------------
      Delete platform socket only if refCnt is 0. Otherwise if PCB is not yet
      deleted and if it posts an event to platform socket, Socket platform
      would be accessing a deleted pointer
    -----------------------------------------------------------------------*/
    (void) Release();
    dsErrno = AEE_SUCCESS;
  }
  else
  {
    SetState( State::CLOSING);
    dsErrno = AEE_EWOULDBLOCK;
  }

  LOG_MSG_FUNCTION_EXIT_2("Socket::Close(): "
                          "Sock %d ret 0x%x", platformSockHandle, dsErrno);
  return dsErrno;

} /* Socket::Close() */


ds::ErrorType Socket::AbortConnection
(
  ds::ErrorType  reasonForAbort
)
{
  Socket *              acceptPlatformSockPtr;
  ps_socket_cmd_type *  closePSCmdBufPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::AbortConnection(): "
                           "Sock %d state %d", platformSockHandle, state);

  /*-------------------------------------------------------------------------
    Do nothing if socket is already in CLOSING or CLOSED state
  -------------------------------------------------------------------------*/
  if (State::INIT == state || State::CLOSED == state)
  {
    LOG_MSG_INFO1_2("Socket::AbortConnection(): "
                    "Can't abort as sock %d in state %d",
                    platformSockHandle, state);
  }
  else if (State::LISTEN == state)
  {
    /*-----------------------------------------------------------------------
      If this is a listening socket, silently close all the sockets which are
      connected but not yet accepted by the application
    -----------------------------------------------------------------------*/
    while (0 != ( acceptPlatformSockPtr =
                    static_cast <Socket *> ( acceptedList.PopFront())))
    {
      acceptPlatformSockPtr->listenHeadPtr = 0;
      (void) acceptPlatformSockPtr->AbortConnection( reasonForAbort);
      (void) acceptPlatformSockPtr->Release();
      /*-------------------------------------------------------------------------
        PopFront() obtains a new strong reference before returning us the socket.
        Discard it to keep the reference count correct. 
      -------------------------------------------------------------------------*/
      (void) acceptPlatformSockPtr->Release();
    }
  }
  else
  {
    closePSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_CLOSE);
    if (0 == closePSCmdBufPtr)
    {
      return AEE_ENOMEMORY;
    }

    closePSCmdBufPtr->user_data.silent_close_flag = TRUE;
    ps_socket_cmd_send( &closePSCmdBufPtr,
                        platformSockHandle,
                        pcbHandle,
                        protocol);

    /*-----------------------------------------------------------------------
      Map reasonForAbort to corresponding PCB closed reason and update it
    -----------------------------------------------------------------------*/
    switch (reasonForAbort)
    {
      case QDS_ENETDOWN:
      {
        pcbClosedReason = PS_PCB_CLOSE_REASON_CODE_NETWORK;
        break;
      }

      case QDS_EIPADDRCHANGED:
      {
        pcbClosedReason = PS_PCB_CLOSE_REASON_CODE_RESET;
        break;
      }

      default:
      {
        LOG_MSG_ERROR_2("Socket::AbortConnection(): "
                        "Unknown reason 0x%x, sock %d",
                        reasonForAbort, platformSockHandle);

        break;
      }
    }

    /*-----------------------------------------------------------------------
      Set state to CLOSED
    -----------------------------------------------------------------------*/
    SetState( State::CLOSED);
  }

  return AEE_SUCCESS;

} /* Socket::AbortConnection() */


bool Socket::IsSockAddrInUse
(
  ProtocolType                  _protocol,
  const ds::SockAddrIN6InternalType *   v6SockAddrPtr,
  bool                          reUseAddr
)
{
  bool  isSockAddrInUse = false;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == v6SockAddrPtr)
  {
    DATA_ERR_FATAL("Socket::IsSockAddrInUse(): NULL addr");
    return true;
  }

  /*-------------------------------------------------------------------------
    It is invalid for port to be 0. When port is 0, ephemeral port needs to
    be generated and there is no need to check if address is in use
  -------------------------------------------------------------------------*/
  if (0 == v6SockAddrPtr->port)
  {
    DATA_ERR_FATAL("Socket::IsSockAddrInUse(): 0 port");
    return true;
  }

 /*-------------------------------------------------------------------------
    In the case when application has to explicitly bind to a port, the
    following scenario is failing
      1. Application opens a UDP socket using dss_socket()
      2. Application calls dss_bind() on the socket
      3. Socket is closed using dss_close()
      4. Application opens another UDP socket immediately using dss_socket()
      5. Application does a dss_bind() on the new socket

   The reason for failure is that the platform socket is not deleted until
   it is closed from the protocol stack as well. This causes platform to
   think that port is in use even though application had already closed the
   socket and SUCCESS is returned synchronously. So ignore sockets which are
   in CLOSED state.
  -------------------------------------------------------------------------*/
  if (State::CLOSED == state)
  {
    LOG_MSG_INFO1_1("Socket::IsSockAddrInUse(): "
                    "Ignoring sock %d because it is in CLOSED state",
                    platformSockHandle);
    return false;
  }

  /*-------------------------------------------------------------------------
    Passed in SockAddr is in use if
      1. Socket's protocol is same as passed in protocol AND
      2. Socket's local port is same as passed in port AND
      3. Socket's local address is same as passed in address OR
         passed in address is ps_in6addr_any (because interface's local IP
         address will be used which will be same as this socket's IP address)

    Note that because routing cache of a UDP socket can change at any time,
    two sockets can't bind to same port even if they are bound to different
    interfaces
  -------------------------------------------------------------------------*/
  if (_protocol == protocol &&
      v6SockAddrPtr->port == v6LocalSockAddr.port &&
      ( PS_IN6_ARE_ADDR_EQUAL( v6LocalSockAddr.addr, v6SockAddrPtr->addr) ||
        PS_IN6_ARE_ADDR_EQUAL( v6SockAddrPtr->addr, &ps_in6addr_any)))
  {
    /*-----------------------------------------------------------------------
      It is ok to re-use address or port if SO_REUSE_ADDR socket option is
      set (indicated by reUseAddr flag) and if address is a multicast address

      Address re-use is allowed if
        1. Both addresses are multicast addresses OR
        2. Socket's local address is in6addrany, but passed in address is a
           multicast address OR
        3. Passed in address is in6addrany, but socket's local address is a
           multicast address
        4. it is a system socket (need to support multi-RmNet where a DHCP
           server is created per each RmNet interface and all DHCP servers
           bind to same port)

      Address re-use is not allowed if both addresses are in6addrany because
      it is not guaranteed that socket will be bound to a multicast address
    -----------------------------------------------------------------------*/
    if (reUseAddr &&
        ( PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST( v6LocalSockAddr.addr) ||
          PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST( v6SockAddrPtr->addr) ||
          PS_IN6_IS_ADDR_MULTICAST( v6LocalSockAddr.addr) ||
          PS_IN6_IS_ADDR_MULTICAST( v6SockAddrPtr->addr)))
    {
      LOG_MSG_INFO1_1("Socket::IsSockAddrInUse(): "
                      "Allowing re-use of addr of sock %d",
                      platformSockHandle);
      isSockAddrInUse = false;
    }
    else if (isSystemSocket)
    {
      LOG_MSG_INFO1_1("Socket::IsSockAddrInUse(): "
                      "Allowing re-use of addr of system sock %d",
                      platformSockHandle);
      isSockAddrInUse = false;
    }
    else
    {
      isSockAddrInUse = true;
    }
  }

  LOG_MSG_FUNCTION_EXIT_4("Socket::IsSockAddrInUse(): "
                          "Returning %d, sock %d Proto %d reUseAddr %d",
                          isSockAddrInUse, platformSockHandle, 
                          _protocol, reUseAddr);
  return isSockAddrInUse;

} /* Socket::IsSockAddrInUse() */


PS::Sock::ProtocolStack::IEventListener * Socket::GetIEventListener
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return static_cast <IEventListener *> ( this);
} /* Socket::GetIEventListener() */

void Socket::ProcessTCPConnectedEvent
(
  const struct connection *  connPtr,
  int32                      optionsMask,
  uint32                     mss
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("Socket::ProcessTCPConnectedEvent(): "
                           "Sock %d, connPtr 0x%x, state %d",
                           platformSockHandle, connPtr, state);

  if (0 == connPtr)
  {
    LOG_MSG_ERROR_1("Socket::ProcessTCPConnectedEvent(): "
                    "NULL conn, sock %d", platformSockHandle);
    ASSERT( 0);
    return;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Race conditions such as application closing the socket right when PCB is
    about post TCP_CONN_EV can happen. So ignore this event if state is not
    OPENING (for client socket) or INIT (for sockets in partial Q)
  -------------------------------------------------------------------------*/
  if (State::INIT != state && State::OPENING != state)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Update platform socket with event info
  -------------------------------------------------------------------------*/
  memscpy( v6LocalSockAddr.addr,
           sizeof(ds::INAddr6Type),
           &( connPtr->local.address),
           sizeof( ds::INAddr6Type));

  memscpy( v6RemoteSockAddr.addr,
           sizeof(ds::INAddr6Type),
           &( connPtr->remote.address),
           sizeof( ds::INAddr6Type));

  v6LocalSockAddr.port  = connPtr->local.port;
  v6RemoteSockAddr.port = connPtr->remote.port;
  optCache.mss          = mss;
  optCache.optionsMask |= optionsMask;

  /*-------------------------------------------------------------------------
    If this is a newly accepted socket, move the platform socket to
    accepted list
  -------------------------------------------------------------------------*/
  if (0 != listenHeadPtr)
  {
    v6RemoteSockAddr.scopeId = v6LocalSockAddr.scopeId;

    listenHeadPtr->listenList.RemoveItem( this);
    (void) listenHeadPtr->acceptedList.PushBack( this);

    /*-----------------------------------------------------------------------
      Since a new connection is established, reset ACCEPTED_LIST_EMPTY
    -----------------------------------------------------------------------*/
    listenHeadPtr->ResetEventBitMask( PS::Sock::Platform::Event::ACCEPT,
                                      ACCEPTED_LIST_EMPTY);
  }

  /*-------------------------------------------------------------------------
    Since connection is established, move to OPEN state
  -------------------------------------------------------------------------*/
  SetState( State::OPEN);

  critSectPtr->Leave();
  return;

bail:
  critSectPtr->Leave();
  return;

} /* Socket::ProcessTCPConnectedEvent() */


void Socket::ProcessPCBClosedEvent
(
  uint8  newPCBClosedReason
)
{
  int32 numBytes =0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::ProcessPCBClosedEvent(): "
                           "Sock %d reason %d",
                           platformSockHandle, pcbClosedReason);

  critSectPtr->Enter();

  pcbHandle = 0;

  /*-------------------------------------------------------------------------
    Set pcbClosedReason only if it wasn't set before (It happens only if
    socket was aborted from sockets library)
  -------------------------------------------------------------------------*/
  if (0 == pcbClosedReason)
  {
    pcbClosedReason = newPCBClosedReason;
  }

  /*-------------------------------------------------------------------------
    PCB is closed for listening sockets as soon as Listen() is called. In this
    case, platform socket should not transition to CLOSED state.

    But if it is a half-open connection which was closed, a new TCB should
    be created to re-fill the backlog.
  -------------------------------------------------------------------------*/
  if (0 != listenHeadPtr)
  {
    /*-----------------------------------------------------------------------
      If this socket is in OPEN state, remove it from listening socket's
      accepted list, so that Accept() wouldn't return a deleted platform socket

      If this socket is in OPENING state, remove it from listening socket's
      listen list, so that a deleted platform socket is not maintained in the
      list. Otherwise, closing the listening socket leads to a crash as it
      tries to close an object which was already deleted
    -----------------------------------------------------------------------*/
    if (State::OPEN == state)
    {
      listenHeadPtr->acceptedList.RemoveItem( this);

      /*---------------------------------------------------------------------
        If this is the only node in accepted list, set ACCEPTED_LIST_EMPTY
        bit mask as the last node is deleted
      ---------------------------------------------------------------------*/
      if (0 == listenHeadPtr->acceptedList.Count())
      {
        listenHeadPtr->SetEventBitMask( PS::Sock::Platform::Event::ACCEPT,
                                        ACCEPTED_LIST_EMPTY);
      }
    }
    else
    {
      listenHeadPtr->listenList.RemoveItem( this);
    }

    if (AEE_SUCCESS != listenHeadPtr->AllocListener())
    {
      LOG_MSG_INFO3_1("Socket::ProcessPCBClosedEvent(): "
                      "AllocListener failed, sock %d",
                      listenHeadPtr->platformSockHandle);
    }

    SetState( State::CLOSED);
    (void) Close();
  }
  else if (State::LISTEN != state)
  {
    SetState( State::CLOSED);

    if (0 != rcvQReaderPtr)
    {
      numBytes = rcvQReaderPtr->GetRcvCnt();
    }

    /*-----------------------------------------------------------------------
      Set bit masks appropriately so that events are posted. Reason why these
      are not combined with SetState() is explaiened in that function
    -----------------------------------------------------------------------*/
    if (0 == numBytes)
    {
      SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
    }

    SetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
    SetEventBitMask( PS::Sock::Platform::Event::ACCEPT, NON_LISTENABLE_STATE);
    ResetEventBitMask( PS::Sock::Platform::Event::CLOSE,
                       CLOSEABLE_STATE | CLOSING_STATE);
  }

  /*-------------------------------------------------------------------------
    Decrement refCnt since PCB is not holding pointer to platform socket
    anymore. Delete platform socket if refCnt is 0 as both sockets library
    and PCB are not holding this pointer any more
  -------------------------------------------------------------------------*/
  critSectPtr->Leave();
  (void) Release();

  return;
} /* Socket::ProcessPCBClosedEvent() */


void Socket::ProcessUDPConnectedEvent
(
  const struct connection *  connPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::ProcessUDPConnectedEvent(): "
                           "Sock %d, connPtr 0x%x", platformSockHandle, connPtr);

  if (0 == connPtr)
  {
    LOG_MSG_ERROR_1("Socket::ProcessUDPConnectedEvent(): "
                    "NULL conn, sock %d", platformSockHandle);
    ASSERT( 0);
    return;
  }

  critSectPtr->Enter();

  memscpy( v6LocalSockAddr.addr,
          sizeof(ds::INAddr6Type),
          &( connPtr->local.address),
          sizeof( ds::INAddr6Type));

  memscpy( v6RemoteSockAddr.addr,
          sizeof(ds::INAddr6Type),
          &( connPtr->remote.address),
          sizeof( ds::INAddr6Type));

  v6LocalSockAddr.port  = connPtr->local.port;
  v6RemoteSockAddr.port = connPtr->remote.port;

  critSectPtr->Leave();
  return;

} /* Socket::ProcessUDPConnectedEvent() */


void Socket::ProcessICMPErrorEvent
(
  int16  icmpError
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::ProcessICMPErrorEvent(): "
                           "Sock %d, ICMP err %d",
                           platformSockHandle, icmpError);

  critSectPtr->Enter();

  if (IsOptCacheSet( ds::Sock::OptName::QDS_SO_ERROR_ENABLE) ||
      IsOptCacheSet( ds::Sock::OptName::QDS_IP_RECVERR) ||
      IsOptCacheSet( ds::Sock::OptName::QDS_IPV6_RECVERR))
  {
    if (IsOptCacheSet( ds::Sock::OptName::QDS_SO_ERROR_ENABLE))
    {
      soError = PSErrnoGetDSErrno( icmpError);
    }

    SetEventBitMask( PS::Sock::Platform::Event::WRITE, WR_ERROR_AVAILABLE);
    SetEventBitMask( PS::Sock::Platform::Event::READ, RD_ERROR_AVAILABLE);
  }

  critSectPtr->Leave();
  return;

} /* Socket::ProcessICMPErrorEvent() */


void Socket::ProcessPlatformEnabledEvent
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  ResetEventBitMask( PS::Sock::Platform::Event::PLATFORM_ENABLED,
                     PLATFORM_IS_DISABLED);
  critSectPtr->Leave();

  return;
} /* Socket::ProcessPlatformEnabledEvent() */


void Socket::ProcessPlatformDisabledEvent
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  SetEventBitMask( PS::Sock::Platform::Event::PLATFORM_ENABLED,
                   PLATFORM_IS_DISABLED);
  critSectPtr->Leave();

  return;
} /* Socket::ProcessPlatformDisabledEvent() */


void Socket::BufferAvailable
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  ResetEventBitMask( PS::Sock::Platform::Event::WRITE, SNDQ_FULL);
  critSectPtr->Leave();

  return;
} /* Socket::BufferAvailable() */


void Socket::DataAvailable
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter(); /*lint !e1550 */
  ResetEventBitMask( PS::Sock::Platform::Event::READ, RCVQ_EMPTY);
  critSectPtr->Leave(); /*lint !e1550 */

  return;
} /* Socket::DataAvailable() */


void Socket::EmptyQueue
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();
  if (State::CLOSED == state)
  {
    SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
  }
  SetEventBitMask( PS::Sock::Platform::Event::READ, RCVQ_EMPTY);
  critSectPtr->Leave();

  return;
} /* Socket::EmptyQueue() */


boolean Socket::Process
(
  void *  userDataPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_ERR_FATAL("Socket::Process(): Unexpected call");
  return TRUE;

} /* Socket::Process() */


/*===========================================================================

                         PRIVATE MEMBER FUNCTIONS

===========================================================================*/
void * Socket::operator new
(
  unsigned int bufSize
) throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_PLATFORM_SOCKET_TYPE);
} /* Socket::operator new() */


void Socket::operator delete
(
  void *  bufPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_MEM_FREE( bufPtr);
  return;

} /* Socket::operator delete() */


Socket::Socket
(
  ds::AddrFamilyType    argFamily,
  SocketType            argType,
  ProtocolType          argProtocol,
  ICritSect *           argCritSectPtr,
  IReader *             argRcvQReaderPtr,
  IWriter *             argSndQWriterPtr,
  IReader *             argErrQReaderPtr
) :
    refCnt( 0),
    weakRefCnt( 1),
    platformSockHandle( 0),
    pcbHandle( 0),
    backlog( 0),
    rcvQReaderPtr( argRcvQReaderPtr),
    sndQWriterPtr( argSndQWriterPtr),
    errQReaderPtr( argErrQReaderPtr),
    eventListenerPtr( 0),
    protocol( argProtocol),
    type( argType),
    family( argFamily),
    state( State::AVAIL),
    listenHeadPtr( 0),
    soError( 0),
  	pcbClosedReason( 0),
    isSystemSocket( false)
{
  int32  optIndex;
  int32  optPrivIndex;
  int32  numOpt;
  int32  numOptPriv;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr = argCritSectPtr;
  (void) critSectPtr->AddRef();

  memset( &routeScope, 0, sizeof( ps_route_scope_type));
  memset( &v6LocalSockAddr, 0, sizeof( ds::SockAddrIN6InternalType));
  memset( &v6RemoteSockAddr, 0, sizeof( ds::SockAddrIN6InternalType));
  memset( eventBitMask, 0, sizeof (eventBitMask));
  memset(&optCache, 0, sizeof(optCache));
  memset(&optPrivCache, 0, sizeof(optPrivCache));

  /*-------------------------------------------------------------------------
    Initialize all socket options to default values
  -------------------------------------------------------------------------*/
  numOpt = sizeof( Opt::sockOptInfoArr) / sizeof( Opt::sockOptInfoArr[0]);
  for (optIndex = 0; optIndex < numOpt; optIndex++)
  {
    if (true == Opt::sockOptInfoArr[ optIndex].isBoolVal &&
        TRUE == Opt::sockOptInfoArr[ optIndex].defaultVal)
    {
      SetOptCache( optIndex);
    }
  }

  /*-------------------------------------------------------------------------
    Initialize all socket priv options to default values
  -------------------------------------------------------------------------*/
  numOptPriv = sizeof( OptPriv::sockOptPrivInfoArr) / sizeof( OptPriv::sockOptPrivInfoArr[0]);
  for (optPrivIndex = 0; optPrivIndex < numOptPriv; optPrivIndex++)
  {
    if (true == OptPriv::sockOptPrivInfoArr[ optPrivIndex].isBoolVal &&
        TRUE == OptPriv::sockOptPrivInfoArr[ optPrivIndex].defaultVal)
    {
      SetOptPrivCache( optPrivIndex);
    }
  }

  optCache.sndBuf =
    Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_SO_SNDBUF].defaultVal;
  optCache.rcvBuf =
    Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_SO_RCVBUF].defaultVal;
  optCache.mss =
    Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_TCP_MAXSEG].defaultVal;
  optCache.tos =
    (uint8) Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_IP_TOS].defaultVal;
  optCache.ttl =
    (uint8) Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_IP_TTL].defaultVal;
  optCache.tClass =
    (uint8) Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_IPV6_TCLASS].defaultVal;
  optCache.ttlMcast =
    (uint8) Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_IP_MULTICAST_TTL].defaultVal;
  optCache.maxBackoffTime =
    (uint32) Opt::sockOptInfoArr[ ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME].defaultVal;
  optCache.soLinger.lingerEnabled = FALSE;
  optCache.soLinger.timeInSec  = 0;

  /*-------------------------------------------------------------------------
    Set SNDBUF and RCVBUF sizes for writer and reader, and register for events
    on them
  -------------------------------------------------------------------------*/
  sndQWriterPtr->SetSndBufSize( optCache.sndBuf);
  rcvQReaderPtr->SetRcvBufSize( optCache.rcvBuf);

  rcvQReaderPtr->RegisterReaderListener( this);
  sndQWriterPtr->RegisterWriterListener( this);

  return;
} /* Socket::Socket() */


void Socket::SetState
(
  StateType  newState
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_STATE_CHANGE_3("Socket::SetState(): "
                         "Sock %d new state %d old state %d",
                         platformSockHandle, newState, state);
  state = newState;

  switch (state)
  {
    case State::INIT:
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::ACCEPT, NON_LISTENABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::CLOSE, CLOSEABLE_STATE);
      break;
    }

    case State::OPENING:
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE,
                       CONNECTION_IN_PROGRESS);
      ResetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
      break;
    }

    case State::OPEN:
    {
      ResetEventBitMask( PS::Sock::Platform::Event::WRITE,
                         NON_WRITEABLE_STATE | CONNECTION_IN_PROGRESS);
      ResetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::ACCEPT, NON_LISTENABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::CLOSE, CLOSEABLE_STATE);
      break;
    }

    case State::LISTEN:
    {
      SetEventBitMask( PS::Sock::Platform::Event::ACCEPT, ACCEPTED_LIST_EMPTY);
      ResetEventBitMask( PS::Sock::Platform::Event::ACCEPT,
                         NON_LISTENABLE_STATE);
      break;
    }

    case State::READ_ONLY:
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
      break;
    }

    case State::WRITE_ONLY:
    {
      SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
      break;
    }

    case State::NO_READ_WRITE:
    {
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
      break;
    }

    case State::CLOSING:
    {
      SetEventBitMask( PS::Sock::Platform::Event::READ, NON_READABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::WRITE, NON_WRITEABLE_STATE);
      SetEventBitMask( PS::Sock::Platform::Event::CLOSE, CLOSING_STATE);
      ResetEventBitMask( PS::Sock::Platform::Event::CLOSE, CLOSEABLE_STATE);
      break;
    }

    case State::CLOSED:
    {
      /*---------------------------------------------------------------------
        Event bit masks can be automatically set or reset for CLOSED state
        as well. But this would post CLOSED event even if socket is silently
        closed and Close() returns AEE_SUCCESS. This could be undesirable as it
        is documented that CLOSE event is posted only when socket is
        asynchronously closed
      ---------------------------------------------------------------------*/
      break;
    }
  }

} /* Socket::SetState() */


bool Socket::IsOptCacheSet
(
  ds::Sock::OptNameType  optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 return ( 0 != ( optCache.optionsMask & ( 1 << optName)));
} /* Socket::IsOptSet() */

bool Socket::IsOptPrivCacheSet
(
  ds::Sock::OptNameTypePriv  optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 return ( 0 != ( optPrivCache.optionsMask & ( 1 << optName)));
} /* Socket::IsOptPrivCacheSet() */


void Socket::SetOptCache
(
  ds::Sock::OptNameType  optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  optCache.optionsMask |= ( 1 << optName);
} /* Socket::SetOptCache() */

void Socket::SetOptPrivCache
(
  ds::Sock::OptNameTypePriv  optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  optPrivCache.optionsMask |= ( 1 << optName);
} /* Socket::SetOptPrivCache() */

void Socket::ResetOptCache
(
  ds::Sock::OptNameType  optName
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  optCache.optionsMask &= ~( static_cast <uint32> ( 1 << optName));
} /* Socket::ResetOptCache() */

ds::ErrorType Socket::AllocListener
(
  void
)
{
  Socket *              newPlatformSockPtr = 0;
  ps_socket_cmd_type *  listenPSCmdBufPtr  = 0;
  ds::ErrorType         dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate listen cmd and post command to PS stack
  -------------------------------------------------------------------------*/
  listenPSCmdBufPtr = ps_socket_cmd_alloc( PS_SOCKET_CMD_LISTEN);
  if (0 == listenPSCmdBufPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  newPlatformSockPtr = Clone();
  if (0 == newPlatformSockPtr)
  {
    LOG_MSG_INFO3_0("Socket::AllocListener(): "
                    "No mem for backlog queue");
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  (void) listenList.PushBack(newPlatformSockPtr);

  listenPSCmdBufPtr->user_data.listen_data.backlog = backlog;
  listenPSCmdBufPtr->user_data.listen_data.rcv_buf =
    newPlatformSockPtr->optCache.rcvBuf;

  ps_socket_cmd_send( &listenPSCmdBufPtr,
                      newPlatformSockPtr->platformSockHandle,
                      newPlatformSockPtr->pcbHandle,
                      newPlatformSockPtr->protocol);

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free the cloned platform socket
  -------------------------------------------------------------------------*/
bail:
  if(0 != listenPSCmdBufPtr)
  {
    ps_socket_cmd_free( &listenPSCmdBufPtr);
  }

  if (0 != newPlatformSockPtr)
  {
    (void) newPlatformSockPtr->Close();
  }

  return dsErrno;

} /* Socket::AllocListener() */

void Socket::CloneOptMask
(
  Socket * sockPtr
)
{
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_SO_KEEPALIVE))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_SO_KEEPALIVE,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_TCP_NODELAY))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_NODELAY,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_TCP_DELAYED_ACK))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_DELAYED_ACK,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_TCP_SACK))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_SACK,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_TCP_TIMESTAMP))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_TIMESTAMP,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_IP_RECVIF))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IP,
                     ds::Sock::OptName::QDS_IP_RECVIF,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_IP_RECVERR))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IP,
                     ds::Sock::OptName::QDS_IP_RECVERR,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_IPV6_RECVERR))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IPV6,
                     ds::Sock::OptName::QDS_IPV6_RECVERR,
                     TRUE,
                     sizeof(int));
  }

  if (IsOptCacheSet(ds::Sock::OptName::QDS_SO_ERROR_ENABLE))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_SOCKET,
                     ds::Sock::OptName::QDS_SO_ERROR_ENABLE,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptCacheSet(ds::Sock::OptName::QDS_SO_REUSEADDR))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_SOCKET,
                     ds::Sock::OptName::QDS_SO_REUSEADDR,
                     TRUE,
                     sizeof(int));
  }
  
  if (IsOptPrivCacheSet(ds::Sock::OptNamePriv::QDS_ICMP_ECHO_ID))
  {
    sockPtr->SetOptPriv( ds::Sock::OptLevel::QDS_LEVEL_ICMP,
                         ds::Sock::OptNamePriv::QDS_ICMP_ECHO_ID,
                         TRUE,
                         sizeof(int));
  }

  if (IsOptPrivCacheSet(ds::Sock::OptNamePriv::QDS_ICMP_ECHO_SEQ_NUM))
  {
    sockPtr->SetOptPriv( ds::Sock::OptLevel::QDS_LEVEL_ICMP,
                         ds::Sock::OptNamePriv::QDS_ICMP_ECHO_SEQ_NUM,
                         TRUE,
                         sizeof(int));
  }

  if (IsOptCacheSet(ds::Sock::OptName::QDS_UDP_ENCAPS))
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_UDP,
                     ds::Sock::OptName::QDS_UDP_ENCAPS,
                     TRUE,
                     sizeof(int));
  }
  
} /* Socket::CloneOptMask() */

void Socket::CloneOpt
(
  Socket * sockPtr
)
{
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME].defaultVal
        != optCache.maxBackoffTime)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_MAX_BACKOFF_TIME,
                     optCache.maxBackoffTime,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_SO_RCVBUF].defaultVal
        != optCache.rcvBuf)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_SOCKET,
                     ds::Sock::OptName::QDS_SO_RCVBUF,                     
                     optCache.rcvBuf,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_TCP_MAXSEG].defaultVal
        != optCache.mss)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_TCP,
                     ds::Sock::OptName::QDS_TCP_MAXSEG,                     
                     optCache.mss,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_IP_TOS].defaultVal
        != optCache.tos)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IP,
                     ds::Sock::OptName::QDS_IP_TOS,                     
                     optCache.tos,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_IPV6_TCLASS].defaultVal
        != optCache.tClass)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IPV6,
                     ds::Sock::OptName::QDS_IPV6_TCLASS,                     
                     optCache.tClass,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_IP_MULTICAST_TTL].defaultVal
        != optCache.ttlMcast)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IP,
                     ds::Sock::OptName::QDS_IP_MULTICAST_TTL,                     
                     optCache.ttlMcast,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_IP_MULTICAST_LOOP].defaultVal
        != optCache.mcastLoop)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_IP,
                     ds::Sock::OptName::QDS_IP_MULTICAST_LOOP,                     
                     optCache.mcastLoop,
                     sizeof(int));
  }
  
  if (Opt::sockOptInfoArr[ds::Sock::OptName::QDS_SO_SNDBUF].defaultVal
        != optCache.sndBuf)
  {
    sockPtr->SetOpt( ds::Sock::OptLevel::QDS_LEVEL_SOCKET,
                     ds::Sock::OptName::QDS_SO_SNDBUF,                     
                     optCache.sndBuf,
                     sizeof(int));
  }
  
  if (OptPriv::sockOptPrivInfoArr[ds::Sock::OptNamePriv::QDS_ICMP_TYPE].defaultVal
        != optPrivCache.icmpType)
  {
    sockPtr->SetOptPriv( ds::Sock::OptLevel::QDS_LEVEL_ICMP,
                         ds::Sock::OptNamePriv::QDS_ICMP_TYPE,                         
                         optPrivCache.icmpType,
                         sizeof(int));
  }
  
  if (OptPriv::sockOptPrivInfoArr[ds::Sock::OptNamePriv::QDS_ICMP_CODE].defaultVal
        != optPrivCache.icmpCode)
  {
    sockPtr->SetOptPriv( ds::Sock::OptLevel::QDS_LEVEL_ICMP,
                         ds::Sock::OptNamePriv::QDS_ICMP_CODE,
                         optPrivCache.icmpCode,
                         sizeof(int));
  }
  
  if (TRUE == optCache.soLinger.lingerEnabled)
  {
    (void)sockPtr->SetSOLingerReset( &optCache.soLinger);
  }
  
  CloneOptMask(sockPtr);
  
} /* Socket::CloneOpt() */

Socket * Socket::Clone
(
  void
)
{
  SocketFactory *      platformSockFactoryPtr;
  Socket *             clonePlatformSockPtr;
  ds::SockAddrInternalType   localAddr = {{0}};
  ds::ErrorType        dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("Socket::Clone(): "
                           "Sock %d", platformSockHandle);

  /*-------------------------------------------------------------------------
    Get pointer to platform socket factory
  -------------------------------------------------------------------------*/
  platformSockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);

  /*-------------------------------------------------------------------------
    Create platform socket using platform socket factory. Socket could have
    been created using Socket::CreateInstance() but the socket wouldn't have
    been added to list of platform sockets maintained by
    platform socket factory
  -------------------------------------------------------------------------*/
  clonePlatformSockPtr = platformSockFactoryPtr->CreateSocket( family,
                                                               type,
                                                               protocol,
                                                               critSectPtr,
                                                               &dsErrno);
  if (0 == clonePlatformSockPtr)
  {
    LOG_MSG_INFO3_1("Socket::Clone(): "
                    "Out of mem to clone sock %d", platformSockHandle);
    goto bail;
  }

  DS_UTILS_RELEASEIF(platformSockFactoryPtr);
  platformSockFactoryPtr = 0;

  clonePlatformSockPtr->listenHeadPtr = this;
  clonePlatformSockPtr->isSystemSocket = isSystemSocket;

  /*-------------------------------------------------------------------------
    Bind the cloned platform socket
  -------------------------------------------------------------------------*/
  (void) AddrUtils::GetSockAddrStorage( &v6LocalSockAddr, family, &localAddr);

  dsErrno = clonePlatformSockPtr->Bind( &localAddr);
  if (AEE_SUCCESS != dsErrno)
  {
     LOG_MSG_INFO3_1("Socket::Clone(): "
                     "BIND failed, sock %d", platformSockHandle);
     goto bail;
  }

/*-------------------------------------------------------------------------
    Update route scope for the cloned platform socket
  -------------------------------------------------------------------------*/
  dsErrno = clonePlatformSockPtr->SetRouteScope( &routeScope);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1( "SetRouteScope failed, sock %d", platformSockHandle);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    TODO: Optimize by combining all the socket options in one command
  -------------------------------------------------------------------------*/  
  CloneOpt(clonePlatformSockPtr);
  
  LOG_MSG_FUNCTION_EXIT_1("Socket::Clone(): "
                          "Returning cloned sock %d",
                          clonePlatformSockPtr->platformSockHandle);
  return clonePlatformSockPtr;

  /*-------------------------------------------------------------------------
    Common error handling code - Free socket and socket factory
  -------------------------------------------------------------------------*/
bail:
  DS_UTILS_RELEASEIF(platformSockFactoryPtr);
  DS_UTILS_RELEASEIF(clonePlatformSockPtr);

  return 0;
} /* Socket::Clone() */


ds::ErrorType Socket::GetDSErrnoForConnect
(
  ds::SockAddrIN6InternalType *  v6RemoteAddrPtr
)
{
  ds::ErrorType  dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::GetDSErrnoForConnect(): "
                           "Sock %d state %d", platformSockHandle, state);

  ASSERT( 0 != v6RemoteAddrPtr);

  /*-------------------------------------------------------------------------
    Error is chosen based on socket's state
  -------------------------------------------------------------------------*/
  switch (state)
  {
    /*-----------------------------------------------------------------------
      Connect() is not supported in LISTEN/CLOSING states
    -----------------------------------------------------------------------*/
    case State::LISTEN:
    case State::CLOSING:
    {
      dsErrno = QDS_EINVAL;
      break;
    }

    /*-----------------------------------------------------------------------
      TCP socket is in the process of connection establishment
    -----------------------------------------------------------------------*/
    case State::OPENING:
    {
      dsErrno = QDS_EINPROGRESS;
      break;
    }

    case State::OPEN:
    case State::READ_ONLY:
    case State::WRITE_ONLY:
    case State::NO_READ_WRITE:
    {
      /*---------------------------------------------------------------------
        Once a TCP socket is connected, it doesn't allow connecting to a
        different peer. So, check if passed in peer name is same as the peer
        that the socket is connected to.

        Connect() is always allowed for UDP sockets. So if the control reached
        here, it means that Connect() failed for some other reason than the
        invalid state
      ---------------------------------------------------------------------*/
      if (Protocol::QDS_TCP == protocol)
      {
        if (v6RemoteAddrPtr->port == v6RemoteSockAddr.port &&
            PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddrPtr->addr,
                                   v6RemoteSockAddr.addr))
        {
          dsErrno = QDS_EISCONN;
        }
        else
        {
          dsErrno = QDS_EINVAL;
        }
      }
      else
      {
        dsErrno = AEE_SUCCESS;
      }

      break;
    }

    case State::CLOSED:
    {
      /*---------------------------------------------------------------------
        Update dsErrno either based on pcbClosedReason or soError for TCP
        sockets. Connected UDP socket never goes to CLOSED state unless either
        application closed it or interface went down. So return QDS_ENETDOWN.
      ---------------------------------------------------------------------*/
      if (Protocol::QDS_TCP == protocol)
      {
        /*-------------------------------------------------------------------
          Return soError if set
        -------------------------------------------------------------------*/
        if (IsOptCacheSet( ds::Sock::OptName::QDS_SO_ERROR_ENABLE) &&
            0 != soError)
        {
          dsErrno = soError;
          soError = 0;

          ResetEventBitMask( PS::Sock::Platform::Event::WRITE,
                             WR_ERROR_AVAILABLE);
          ResetRDErrorAvailableEventBitMask();
        }
        else
        {
          dsErrno = pcbClosedReasonToConnErrMapping[ pcbClosedReason];
        }
      }
      else
      {
        dsErrno = QDS_ENETDOWN;
      }

      break;
    }

    default:
    {
      LOG_MSG_ERROR_2("Socket::GetDSErrnoForConnect(): "
                      "Unknown state %d, sock %d", state, platformSockHandle);
      ASSERT( 0);
      dsErrno = QDS_EINVAL;
      break;
    }
  }

  LOG_MSG_FUNCTION_EXIT_2("Socket::GetDSErrnoForConnect(): "
                          "Err 0x%x, sock %d", dsErrno, platformSockHandle);
  return dsErrno;

} /* Socket::GetDSErrnoForConnect() */


ds::ErrorType Socket::GetDSErrnoForSockInput
(
  void
)
{
  int32  readEventBitMask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Socket::GetDSErrnoForSockInput(): "
                           "Sock %d, state %d", platformSockHandle, state);

  readEventBitMask = eventBitMask[ PS::Sock::Platform::Event::READ];

  /*-------------------------------------------------------------------------
    If socket is in non-readable state, return error based on state.
    Otherwise, socket is flow controlled and return AEE_EWOULDBLOCK

    Note that RD_ERROR_AVAILABLE doesn't block the app from reading data. It
    is used only to post READ_EV
  -------------------------------------------------------------------------*/
  if (0 != ( NON_READABLE_STATE & readEventBitMask))
  {
    switch (state)
    {
      case State::INIT:
      {
        return QDS_ENOTCONN;
      }

      case State::LISTEN:
      {
        return QDS_EINVAL;
      }

      case State::OPENING:
      {
        return AEE_EWOULDBLOCK;
      }

      case State::CLOSED:
      {
        if (Protocol::QDS_TCP == protocol)
        {
          return pcbClosedReasonToSockInputErrMapping[ pcbClosedReason];
        }
        else
        {
          return QDS_ENETDOWN;
        }
      }

      case State::CLOSING:
      case State::WRITE_ONLY:
      case State::NO_READ_WRITE:
      {
        return QDS_EEOF;
      }

      default:
      {
        LOG_MSG_ERROR_2("Socket::GetDSErrnoForSockInput(): "
                        "Unknown state %d, sock %d",
                        state, platformSockHandle);
        ASSERT( 0);
        return QDS_EINVAL;
      }
    }
  }
  else if (0 != ( RCVQ_EMPTY & readEventBitMask))
  {
    return AEE_EWOULDBLOCK;
  }
  else
  {
    LOG_MSG_ERROR_2("Socket::GetDSErrnoForSockInput(): "
                    "Unknown ev bit mask 0x%x, sock %d",
                    readEventBitMask, platformSockHandle);
    ASSERT(0);
    return QDS_EOPNOTSUPP;
  }

} /* Socket::GetDSErrnoForSockInput() */


ds::ErrorType Socket::GetDSErrnoForSockOutput
(
  void
)
{
  ds::ErrorType  dsErrno;
  int32          writeEventBitMask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_FUNCTION_ENTRY_2("Socket::GetDSErrnoForSockOutput(): "
                           "Sock %d, state %d", platformSockHandle, state);

  writeEventBitMask = eventBitMask[ PS::Sock::Platform::Event::WRITE];

  /*-------------------------------------------------------------------------
    If soError is set, return it and reset the soError. Else if socket is in
    non-writeable state, return error based on state. Otherwise, socket is
    flow controlled and return AEE_EWOULDBLOCK
  -------------------------------------------------------------------------*/
  if (0 != ( WR_ERROR_AVAILABLE & writeEventBitMask))
  {
    dsErrno = soError;
    soError = 0;

    ResetEventBitMask( PS::Sock::Platform::Event::WRITE, WR_ERROR_AVAILABLE);
    ResetEventBitMask( PS::Sock::Platform::Event::READ, RD_ERROR_AVAILABLE);

    return dsErrno;
  }
  else if (0 != ( NON_WRITEABLE_STATE & writeEventBitMask))
  {
    switch (state)
    {
      case State::INIT:
      {
        return QDS_ENOTCONN;
      }

      case State::LISTEN:
      {
        return QDS_EINVAL;
      }

      case State::READ_ONLY:
      case State::NO_READ_WRITE:
      case State::CLOSING:
      {
        return QDS_ESHUTDOWN;
      }

      case State::CLOSED:
      {
        if (Protocol::QDS_TCP == protocol)
        {
          return pcbClosedReasonToSockOutputErrMapping[ pcbClosedReason];
        }
        else
        {
          return QDS_ENETDOWN;
        }
      }

      default:
      {
        LOG_MSG_ERROR_2("Socket::GetDSErrnoForSockOutput(): "
                        "Unknown state %d, sock %d",
                        state, platformSockHandle);
        ASSERT( 0);
        return QDS_EINVAL;
      }
    } /* switch (socket's state) */
  }
  else
  {
    return AEE_EWOULDBLOCK;
  }

} /* Socket::GetDSErrnoForSockOutput() */

void Socket::SetOrClearEvent
(
  PS::Sock::Platform::EventType  event
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If there is no listener registered, do nothing - the state of
    the listener will be reconciled on RegEventListener call.
  -------------------------------------------------------------------------*/
  if (0 == eventListenerPtr)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get a strong reference to event listener before posting event.
  -------------------------------------------------------------------------*/
  if (TRUE == eventListenerPtr->GetStrongRef())
  {

    LOG_MSG_INFO1_2("Socket::SetOrClearEvent(): "
                    "Posting ev %d, sock %d", event, platformSockHandle);

    /*-----------------------------------------------------------------------
      Post event if
        1. Any of the permanent error bits are set OR
        2. None of the flow control error bits are set
      i.e.
        0 != ( PERMANENT_ERR_BIT_MASK & eventBitMask[ eventIdx]) ||
        0 == ( FLOW_CONTROL_ERR_BIT_MASK & eventBitMask[ eventIdx])
    -----------------------------------------------------------------------*/
    if (0 != ( PERMANENT_ERR_BIT_MASK & eventBitMask[ event]) ||
        0 == ( FLOW_CONTROL_ERR_BIT_MASK & eventBitMask[ event]))
    {
      eventListenerPtr->SetEvent( event);
    }
    else
    {
      eventListenerPtr->ClearEvent( event);
    }

    (void) eventListenerPtr->Release();
  }

} /* Socket::SetOrClearEvent() */

void Socket::SetEventBitMask
(
  PS::Sock::Platform::EventType  event,
  uint32                         bitMask
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  eventBitMask[ event] |= bitMask;

  LOG_MSG_INFO1_4("Socket::SetEventBitMask(): "
                  "Sock %d ev %d bit mask 0x%x -> ev mask 0x%x",
                  platformSockHandle, event, bitMask, eventBitMask[ event]);

  SetOrClearEvent( event);

  return;
} /* Socket::SetEventBitMask() */


void Socket::ResetEventBitMask
(
  PS::Sock::Platform::EventType  event,
  uint32                         bitMask
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  eventBitMask[ event] &= ~bitMask;

  LOG_MSG_INFO1_4("Socket::ResetEventBitMask(): "
                  "Sock %d ev %d bit mask 0x%x -> ev mask 0x%x",
                  platformSockHandle, event, bitMask, eventBitMask[ event]);

  SetOrClearEvent( event);

  return;
} /* Socket::ResetEventBitMask() */


void Socket::ResetRDErrorAvailableEventBitMask
(
  void
)
{
  uint16  pktCnt;
  int16   psErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == errQReaderPtr)
  {
    LOG_MSG_INFO1_1( "errQReaderPtr is null, sock %d", platformSockHandle);
    return;
  }

  if (-1 == errQReaderPtr->GetPktCnt( &pktCnt, &psErrno))
  {
    LOG_MSG_INFO3_2("Socket::ResetRDErrorAvailableEventBitMask(): "
                    "GetPktCnt failed, sock %d ps_err %d",
                    platformSockHandle, psErrno);
    ASSERT( 0);
    return;
  }

  if (0 == pktCnt ||
      ( false == IsOptCacheSet( ds::Sock::OptName::QDS_IP_RECVERR) &&
        false == IsOptCacheSet( ds::Sock::OptName::QDS_IPV6_RECVERR)))
  {
    ResetEventBitMask( PS::Sock::Platform::Event::READ, RD_ERROR_AVAILABLE);
  }

  return;
} /* Socket::ResetRDErrorAvailableEventBitMask() */


void Socket::ProcessAncillaryData
(
  IAncDataPriv *            ancillaryDataPtr[],
  int32                     numAncData,
  int32                     flags,
  ps_icmp_errq_meta_info *  icmpErrQMetaInfoPtr,
  ps_pkt_read_info *        readInfoPtr,
  int32 *                   numAncDataFilledPtr
)
{
  RecvIFInfo *             recvIFInfoPtr;
  ICMPErrInfo *            icmpErrInfoPtr;
  ds::SockAddrInternalType remoteAddr = {{0}};
  int32                    cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 != ( RecvFlags::QDS_MSG_ERRQUEUE & flags) &&
      ( IsOptCacheSet( ds::Sock::OptName::QDS_IP_RECVERR) ||
        IsOptCacheSet( ds::Sock::OptName::QDS_IPV6_RECVERR)))
  {

    if (ds::AddrFamily::QDS_AF_INET == family)
    {
      remoteAddr.v4.family = ps_htons(ds::AddrFamily::QDS_AF_INET);//network byte order
      remoteAddr.v4.port   = 0;
      remoteAddr.v4.addr   =
        PS_IN6_GET_V4_FROM_V4_MAPPED_V6( &( icmpErrQMetaInfoPtr->v6_offender));
    }
    else
    {
      remoteAddr.v6.family = ps_htons(ds::AddrFamily::QDS_AF_INET6);//network byte order
      remoteAddr.v6.port   = 0;
      memscpy( remoteAddr.v6.addr,
               sizeof(ds::INAddr6Type),
               &( icmpErrQMetaInfoPtr->v6_offender),
               sizeof( struct ps_in6_addr));

      if (0 != icmpErrQMetaInfoPtr->extended_err.ee_data)
      {
        /*-------------------------------------------------------------------
          Address is linklocal so ee_data stores the scope_id
        -------------------------------------------------------------------*/
        remoteAddr.v6.scopeId = icmpErrQMetaInfoPtr->extended_err.ee_data;
        icmpErrQMetaInfoPtr->extended_err.ee_data = 0;
      }
    }

    icmpErrInfoPtr =
       new ICMPErrInfo( icmpErrQMetaInfoPtr->extended_err, &remoteAddr);

    ancillaryDataPtr[cnt++] = icmpErrInfoPtr;
  }

  if (cnt < numAncData &&
      IsOptCacheSet( ds::Sock::OptName::QDS_IP_RECVIF))
  {
    recvIFInfoPtr = new RecvIFInfo( readInfoPtr->ifaceId);
    ancillaryDataPtr[cnt++] = recvIFInfoPtr;
  }

  *numAncDataFilledPtr = cnt;

  while (cnt < numAncData)
  {
    ancillaryDataPtr[cnt++] = 0;
  }

  return;
} /* Socket::ProcessAncillaryData() */

bool Socket::IsConnInProgress
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( State::OPENING == state);
} /* Socket::IsConnInProgress() */
