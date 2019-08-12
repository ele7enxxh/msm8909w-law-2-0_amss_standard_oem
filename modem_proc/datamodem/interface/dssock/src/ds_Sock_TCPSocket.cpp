/*===========================================================================
  FILE: ds_Sock_TCPSocket.cpp

  OVERVIEW: This file provides implementation of the TCPSocket class.

  DEPENDENCIES: None

              Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_TCPSocket.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Sock_TCPSocket.h"
#include "ds_Sock_SocketFactory.h"
#include "ds_Sock_SocketDef.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_DebugMsg.h"
#include "PS_Sock_Platform_ISocket.h"
#include "ps_mem.h"
#include "ds_Sock_AddrUtils.h"
#include "ds_Utils_CSignalFactory.h"
#include "ds_Utils_ISignalFactory.h"
#include "dss_config.h"
#include "ps_wmk_logging.h"
#include "ds_sys.h"
#include "ps_ifacei.h"

using namespace PS::Sock;
using namespace ds::Sock;
using namespace ds::Error;
using namespace ds::Utils;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
TCPSocket * TCPSocket::CreateInstance
(
  ds::AddrFamilyType  _family,
  NetworkCtl          *pNetworkCtl
)
{
  TCPSocket *  tcpSocketPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("TCPSocket::CreateInstance(): "
                           "Family %d", _family);

  /*-------------------------------------------------------------------------
    Create a TCP Socket
  -------------------------------------------------------------------------*/
  tcpSocketPtr = new TCPSocket();
  if (0 == tcpSocketPtr)
  {
    LOG_MSG_ERROR_0("TCPSocket::CreateInstance(): "
                    "No mem for TCP Sock");
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Initialize the socket
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != tcpSocketPtr->Init( _family,
                                     Type::QDS_STREAM,
                                     Protocol::QDS_TCP,
                                     pNetworkCtl))
  {
    goto bail;
  }

  return tcpSocketPtr;

  /*-------------------------------------------------------------------------
    Common error handling code - Delete the Socket instance if it was created
  -------------------------------------------------------------------------*/

bail:

  DS_UTILS_RELEASEIF(tcpSocketPtr);

  return 0;
} /* TCPSocket::CreateInstance() */

TCPSocket::TCPSocket()
{

} /* TCPSocket::TCPSocket() */

void TCPSocket::Destructor()
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  Socket::Destructor();

} /* TCPSocket::~TCPSocket() */

ds::ErrorType CDECL TCPSocket::Listen
(
  int  backlog
)
{
  ds::ErrorType    dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("TCPSocket::Listen(): "
                           "Sock 0x%x backlog %d", this, backlog);

  /*-------------------------------------------------------------------------
    Make sure that backlog > 0
  -------------------------------------------------------------------------*/
  if (0 >= backlog)
  {
    LOG_MSG_INVALID_INPUT_2("TCPSocket::Listen(): "
                            "Invalid backlog %d, sock 0x%x", backlog, this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Truncate backlog to SO_MAX_CONN and call platform specific Listen(...)
  -------------------------------------------------------------------------*/
  if (backlog > DSS_SOMAXCONN)
  {
    backlog = DSS_SOMAXCONN;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("TCPSocket::Listen(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  dsErrno = platformSockPtr->Listen( backlog);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_2("TCPSocket::Listen(): "
                    "Listen failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  LOG_MSG_FUNCTION_EXIT_1("TCPSocket::Listen(): "
                          "Success, sock 0x%x", this);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  return dsErrno;

} /* TCPSocket::Listen() */


ds::ErrorType CDECL TCPSocket::Accept
(
  ds::SockAddrStorageType     remoteAddrPtr,
  ISocket **             newSockPtr
)
{
  TCPSocket *                 newTCPSockPtr      = 0;
  SockAddrInternalType        tempAddr = {{0}};
  Platform::ISocket *         newPlatformSockPtr = 0;
  Platform::IEventListener *  eventListenerPtr;
  INode *                     iNodePtr;
  SocketFactory *             sockFactoryPtr;
  LingerType                  soLinger;
  ds::ErrorType               dsErrno;
  ds::AddrFamilyType         _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32                      acceptEventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == remoteAddrPtr || 0 == newSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("TCPSocket::Accept(): "
                            "NULL args, sock 0x%x", this);
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INVALID_INPUT_1("TCPSocket::Accept(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return AEE_EWOULDBLOCK if ACCEPT event is flow controlled
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_ACCEPT, &acceptEventIndex)){
    LOG_MSG_ERROR_1("TCPSocket::Accept(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[acceptEventIndex])
  {
    dsErrno = AEE_EWOULDBLOCK;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Check if a new connection is established
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Accept( &newPlatformSockPtr, &tempAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("TCPSocket::Accept(): "
                    "Accept failed, sock 0x%x err 0x%x", this, dsErrno);

    goto bail;
  }

  memscpy((void *) remoteAddrPtr,
          sizeof(ds::SockAddrStorageType),
          (void *) &tempAddr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(remoteAddrPtr,ps_htons(_family));

  /*-------------------------------------------------------------------------
    A new connection is available. As newly accepted socket should have exact
    same configuration as the listener socket, clone the current socket and
    set platformSockPtr
  -------------------------------------------------------------------------*/
  newTCPSockPtr = new TCPSocket();
  if (0 == newTCPSockPtr)
  {
    LOG_MSG_INFO3_0("TCPSocket::Accept(): "
                    "No mem for sock");
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  dsErrno = newTCPSockPtr->CloneSocket( this);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("TCPSocket::Accept(): "
                    "CloneSocket failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Assign the platform socket after the DS socket completed initialization
  -------------------------------------------------------------------------*/
  newTCPSockPtr->platformSockPtr = newPlatformSockPtr;

  /*-------------------------------------------------------------------------
    Register an event listener with platform socket
  -------------------------------------------------------------------------*/
  eventListenerPtr = static_cast <Platform::IEventListener *> ( newTCPSockPtr);
  newPlatformSockPtr->RegEventListener( eventListenerPtr);

  /*-------------------------------------------------------------------------
    Update critical section for the platform socket
  -------------------------------------------------------------------------*/
  newPlatformSockPtr->SetCritSection( newTCPSockPtr->critSectPtr);

  /*-------------------------------------------------------------------------
    Add the Accept Socket to the SocketFactory list
  -------------------------------------------------------------------------*/
  sockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != sockFactoryPtr);

  iNodePtr = static_cast <ds::Utils::INode *> ( newTCPSockPtr);

  (void) sockFactoryPtr->AddItem( iNodePtr);
  DS_UTILS_RELEASEIF(sockFactoryPtr);

  *newSockPtr = static_cast <ISocket *> ( newTCPSockPtr);
  LOG_MSG_FUNCTION_EXIT_2("TCPSocket::Accept(): "
                          "Success, sock 0x%x new sock 0x%x",
                          this, newTCPSockPtr);

  critSectPtr->Leave();
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code - Free accepted socket
  -------------------------------------------------------------------------*/
bail:
  if (0 != newPlatformSockPtr)
  {
    /*-----------------------------------------------------------------------
      Set linger timeout to 0 to force silent close. Otherwise, platform will
      merely change the socket to CLOSED state and wait for sock lib to call
      Close() again to delete it. Since the app doesn't know about this
      socket, a second Close() will never be called resulting in memory leak
    -----------------------------------------------------------------------*/
    soLinger.lingerEnabled = TRUE;
    soLinger.timeInSec  = 0;

    (void) newPlatformSockPtr->SetSOLingerReset( &soLinger);
    (void) newPlatformSockPtr->Close();
  }

  DS_UTILS_RELEASEIF(newTCPSockPtr);

  critSectPtr->Leave();
  return dsErrno;

} /* TCPSocket::Accept() */


ds::ErrorType CDECL TCPSocket::WriteDSMChain
(
  dsm_item_type **  dsmItemPtrPtr,
  int32 *           numWrittenPtr
)
{
  SockAddrInternalType remoteSockAddr;
  ds::ErrorType        dsErrno;
  uint32               writeEventIndex = 0;
  boolean               isReorig = FALSE;
  ps_iface_type        *iface_ptr = NULL;  
  ps_iface_type        *base_iface_ptr = NULL;  
  ds_sys_tech_enum_type tech_type = DS_SYS_TECH_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_ENETDOWN;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    If Write event bit mask is set, handle the transient/flow control errors.
    Return error if all the errors couldn't be handled
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_WRITE, &writeEventIndex))
  {
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
      LOG_MSG_INFO3_2("TCPSocket::WriteDSMChain(): "
                      "GetPeerName failed, sock 0x%x err 0x%x", this, dsErrno);
      goto bail;
    }

    dsErrno = HandleNonZeroWriteEventBitMask( &remoteSockAddr, &isReorig);
    if (isReorig == TRUE)
    {
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
        ps_wmk_logging_reorig_pkt(tech_type,
                                  PS_WMK_UPLINK_PKT, 
                                  *dsmItemPtrPtr);
      }
    }

    if (0 != eventBitMask[writeEventIndex])
    {
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific SendDSMChain(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SendDSMChain( 0,
                                           dsmItemPtrPtr,
                                           0,
                                           numWrittenPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("TCPSocket::WriteDSMChain(): "
                    "SendDSMChain failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  critSectPtr->Leave();

  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  LOG_MSG_ERROR_3("TCPSocket::WriteDSMChain(): "
                  "sock 0x%x dsErrno 0x%x Write ev bit mask 0x%x", 
                  this, dsErrno, eventBitMask[writeEventIndex]);

  critSectPtr->Leave();
  return dsErrno;

} /* TCPSocket::WriteDSMChain() */


ds::ErrorType CDECL TCPSocket::ReadDSMChain
(
  dsm_item_type **  dsmItemPtrPtr,
  int32 *           numReadPtr
)
{
  ds::ErrorType  dsErrno;
  uint32         readEventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    dsErrno = QDS_ENETDOWN;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Return error if socket is blocked for reading
      1. Return EWOULDBLOCK if socket is flow controlled
      2. ASSERT if err mask is unknown
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_READ, &readEventIndex))
  {
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[readEventIndex])
  {
    if (0 != ( FLOW_CONTROL_EVENT_BIT_MASK & eventBitMask[readEventIndex]))
    {
      LOG_MSG_INFO1_2("TCPSocket::ReadDSMChain(): "
                      "Read is blocked, sock 0x%x err mask 0x%x",
                      this, eventBitMask[readEventIndex]);
      dsErrno = AEE_EWOULDBLOCK;
      goto bail;
    }
    else
    {
      LOG_MSG_INFO1_2("TCPSocket::ReadDSMChain(): "
                      "Unknown err mask 0x%x, sock 0x%x",
                      eventBitMask[readEventIndex], this);
      ASSERT( 0);
      dsErrno = AEE_EWOULDBLOCK;
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific RecvDSMChain(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->RecvDSMChain( 0, dsmItemPtrPtr, 0, numReadPtr);

  /*-------------------------------------------------------------------------
    Return ENETDOWN as error if
      1. EWOULDBLOCK is returned from platform AND
      2. Routing cache is NULL AND
  -------------------------------------------------------------------------*/
  if (AEE_EWOULDBLOCK == dsErrno)
  {
    if (0 == rtMetaInfoPtr ||
        0 == PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr))
    {
      LOG_MSG_INFO1_1("TCPSocket::ReadDSMChain(): "
                      "NULL routing info, sock 0x%x", this);
      dsErrno = QDS_ENETDOWN;
    }
  }

  critSectPtr->Leave();

  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  LOG_MSG_ERROR_3("TCPSocket::ReadDSMChain(): "
                  "sock 0x%x dsErrno 0x%x Read ev bit mask 0x%x", 
                  this, dsErrno, eventBitMask[readEventIndex]);

  critSectPtr->Leave();
  return dsErrno;

} /* TCPSocket::ReadDSMChain() */


/*===========================================================================

                     PROTECTED MEMBER FUNCTIONS

===========================================================================*/
bool TCPSocket::IsConnectSupported
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  ds::ErrorType *                      dsErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == dsErrnoPtr)
  {
    LOG_MSG_ERROR_1("TCPSocket::IsConnectSupported(): "
                    "NULL errno, sock 0x%x", this);
    ASSERT( 0);
    return false;
  }

  if (0 == v6RemoteAddrPtr)
  {
    LOG_MSG_ERROR_1("TCPSocket::IsConnectSupported(): "
                    "NULL peer addr, sock 0x%x", this);
    ASSERT( 0);
    *dsErrnoPtr = QDS_EFAULT;
    return false;
  }

  /*-------------------------------------------------------------------------
    App is trying to disconnect a socket, if it passes ps_in6addr_any as remote
    addr. Since TCP doesn't support disconnecting a socket, return false
  -------------------------------------------------------------------------*/
  if (PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddrPtr->addr, &ps_in6addr_any))
  {
    LOG_MSG_ERROR_1("TCPSocket::IsConnectSupported(): "
                    "Disconnect not supported, sock 0x%x", this);
    *dsErrnoPtr = QDS_EADDRREQ;
    return false;
  }

  LOG_MSG_FUNCTION_EXIT_1("TCPSocket::IsConnectSupported(): "
                          "Success, sock 0x%x", this);
  return true;

} /* TCPSocket::IsConnectSupported() */


bool TCPSocket::IsOptSupported
(
  OptLevelType  optLevel,
  OptNameType   optName
)
{
  bool  isOptSupported;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (OptLevel::QDS_LEVEL_TCP == optLevel)
  {
    isOptSupported = true;
  }
  else
  {
    switch (optName)
    {
      case OptName::QDS_SO_KEEPALIVE:
      case OptName::QDS_SO_TX_IFACE:
      case OptName::QDS_SO_RCVBUF:
      case OptName::QDS_SO_SNDBUF:
      case OptName::QDS_IP_TOS:
      case OptName::QDS_IP_TTL:
      case OptName::QDS_IPV6_TCLASS:
      case OptName::QDS_SO_FIONREAD:
      case OptName::QDS_SO_REUSEADDR:
      case OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
      case OptName::QDS_TCP_MAX_BACKOFF_TIME:
      {
        isOptSupported = true;
        break;
      }

      default:
      {
        isOptSupported = false;
        break;
      }
    }
  }

  LOG_MSG_FUNCTION_EXIT_4("TCPSocket::IsOptSupported(): "
                          "Sock 0x%x level %d, name %d ret %d",
                          this, optLevel, optName, isOptSupported);
  return isOptSupported;

} /* TCPSocket::IsOptSupported() */

bool TCPSocket::IsPktInfoDifferent
(
  const ds::SockAddrInternalType* remoteAddrPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return false;
} /* TCPSocket::IsPktInfoDifferent() */


ds::ErrorType TCPSocket::FillProtocolInfoInPktInfo
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  const ds::SockAddrIN6InternalType *  v6LocalAddrPtr,
  ip_pkt_info_type *       pktInfoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == v6RemoteAddrPtr || 0 == v6LocalAddrPtr || 0 == pktInfoPtr)
  {
    LOG_MSG_ERROR_1("TCPSocket::FillProtocolInfoInPktInfo(): "
                    "NULL args, sock 0x%x", this);
    ASSERT( 0);
    return QDS_EFAULT;
  }

  if (IP_V6 == pktInfoPtr->ip_vsn)
  {
    pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPPROTO_TCP;
  }
  else
  {
    pktInfoPtr->ip_hdr.v4.protocol = (uint8) PS_IPPROTO_TCP;
  }

  pktInfoPtr->ptcl_info.tcp.src_port = v6LocalAddrPtr->port;
  pktInfoPtr->ptcl_info.tcp.dst_port = v6RemoteAddrPtr->port;

  return AEE_SUCCESS;
} /* TCPSocket::FillProtocolInfoInPktInfo() */


void TCPSocket::ProcessNetworkConfigChangedEvent
(
  ds::ErrorType  reasonForChange
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 != platformSockPtr)
  {
    /*-------------------------------------------------------------------------
      Call in to platform to abort connection
    -------------------------------------------------------------------------*/
    if (AEE_SUCCESS != platformSockPtr->AbortConnection( reasonForChange))
    {
      LOG_MSG_INFO3_1("TCPSocket::ProcessNetworkConfigChangedEvent(): "
                      "AbortConnection failed, sock 0x%x", this);
    }
  }

  return;
} /* TCPSocket::ProcessNetworkConfigChangedEvent() */


/*===========================================================================

                         PRIVATE MEMBER FUNCTIONS

===========================================================================*/
void * TCPSocket::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_TCP_SOCKET_TYPE);
} /* TCPSocket::operator new() */

