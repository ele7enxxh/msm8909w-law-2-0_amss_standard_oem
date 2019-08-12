/*===========================================================================
  FILE: ds_Sock_UDPSocket.cpp

  OVERVIEW: This file provides implementation of the UDPSocket class.

  DEPENDENCIES: None

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_UDPSocket.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-07-15 rp  QShrink2 changes: MSG_* macros in inline functions are removed.
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include <stringl.h>
#include "ds_Sock_AddrUtils.h"
#include "ds_Sock_UDPSocket.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "PS_Sock_Platform_ISocket.h"
#include "ps_mem.h"
#include "ps_iface_ioctl.h"
#include "ds_Net_Platform.h"
#include "ps_wmk_logging.h"
#include "ds_sys.h"

using namespace PS::Sock;
using namespace ds::Sock;
using namespace ds::Error;
using namespace NetPlatform;

/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
UDPSocket * UDPSocket::CreateInstance
(
  ds::AddrFamilyType    _family,
  NetworkCtl            *pNetworkCtl
)
{
  UDPSocket *  udpSocketPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("UDPSocket::CreateInstance(): "
                           "Family %d", _family);

  /*-------------------------------------------------------------------------
    Create a UDP Socket and initialize it
  -------------------------------------------------------------------------*/
  udpSocketPtr = new UDPSocket();
  if (0 == udpSocketPtr)
  {
    LOG_MSG_ERROR_0("UDPSocket::CreateInstance(): "
                    "No mem for UDP Sock");
    goto bail;
  }

  if (AEE_SUCCESS != udpSocketPtr->Init( _family,
                                         Type::QDS_DGRAM,
                                         Protocol::QDS_UDP,
                                         pNetworkCtl))
  {
    goto bail;
  }

  return udpSocketPtr;

  /*-------------------------------------------------------------------------
    Common error handling code - Delete the Socket instance if it was created
  -------------------------------------------------------------------------*/

bail:

  DS_UTILS_RELEASEIF(udpSocketPtr);

  return 0;
} /* UDPSocket::CreateInstance() */

UDPSocket::UDPSocket
(
  void
)
throw()
:
    dosAckStatus( 0),
    overflow( 0)
#ifdef FEATUTE_DATA_PS_MCAST
    ,mcastHandle( 0)
#endif // FEATUTE_DATA_PS_MCAST
{
} /* UDPSocket::UDPSocket() */

ds::ErrorType CDECL UDPSocket::SendToDSMChain
(
  dsm_item_type **             dsmItemPtrPtr,
  const ds::SockAddrStorageType remoteAddrPtr,
  unsigned int                 flags,
  int32 *                      numWrittenPtr
)
{
  const ds::SockAddrInternalType*    peerAddrPtr;
  SockAddrInternalType               tempAddr = {{0}};
  ps_pkt_meta_info_type *            pktMetaInfoPtr = 0;
  SockAddrInternalType               remoteSockAddr = {{0}};
  SockAddrIN6InternalType            v6RemoteAddr = {0};
  ds::ErrorType                      dsErrno;
  bool                               localSendPktMetaInfo = sendPktMetaInfo;
  ds::AddrFamilyType                _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32                             writeEventIndex = 0;
  boolean                            isReorig = FALSE;
  ps_iface_type *                    iface_ptr = NULL;  
  ps_iface_type *                    base_iface_ptr = NULL;  
  ds_sys_tech_enum_type              tech_type = DS_SYS_TECH_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == remoteAddrPtr)
  {
    LOG_MSG_INVALID_INPUT_1("UDPSocket::SendToDSMChain(): "
                            "NULL remote addr, sock 0x%x", this);
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Only MSG_EXPEDITE and MSG_FAST_EXPEDITE are supported in tx path
  -------------------------------------------------------------------------*/
  if (0 != flags)
  {
    if (SendFlags::QDS_MSG_EXPEDITE != flags &&
        SendFlags::QDS_MSG_FAST_EXPEDITE != flags)
    {
      LOG_MSG_INVALID_INPUT_2("UDPSocket::SendToDSMChain(): "
                              "Unsupported flag 0x%x, sock 0x%x", flags, this);
      return QDS_EOPNOTSUPP;
    }

    localSendPktMetaInfo = true;
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
          (void *) remoteAddrPtr,
          sizeof(ds::SockAddrStorageType));

  (void)ds::Sock::AddrUtils::GetFamily(&tempAddr,&_family);
  (void)ds::Sock::AddrUtils::SetFamily(&tempAddr,ps_ntohs(_family));

  /*-------------------------------------------------------------------------
    If destination address is not specified, make sure that socket is
    connected
  -------------------------------------------------------------------------*/
  dsErrno = AddrUtils::GetSockAddrIN6(&tempAddr, &v6RemoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    goto bail;
  }

  if (PS_IN6_ARE_ADDR_EQUAL( v6RemoteAddr.addr, &ps_in6addr_any))
  {
    dsErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
    if (AEE_SUCCESS != dsErrno)
    {
      dsErrno = QDS_EADDRREQ;
      goto bail;
    }

    peerAddrPtr = &tempAddr;
  }
  else
  {
    /*-----------------------------------------------------------------------
      If packet info differs, set NULL_ROUTING_INFO bit so that a fresh
      routing is performed
    -----------------------------------------------------------------------*/
    if (IsPktInfoDifferent(&tempAddr))
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
    LOG_MSG_ERROR_1("UDPSocket::SendToDSMChain(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[writeEventIndex])
  {
    dsErrno = HandleNonZeroWriteEventBitMask( peerAddrPtr, &isReorig);
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
      LOG_MSG_INFO1_3("UDPSocket::SendToDSMChain(): "
                      "Write ev bit mask 0x%x, sock 0x%x err 0x%x",
                      eventBitMask[writeEventIndex], this, dsErrno);
      goto bail;
    }
  }

  if (localSendPktMetaInfo)
  {
    dsErrno = GeneratePktMetaInfo( &pktMetaInfoPtr, flags);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("UDPSocket::SendToDSMChain(): "
                      "GeneratePktMetaInfo failed, sock 0x%x err 0x%x",
                      this, dsErrno);
      goto bail;
    }
  }

  /*-------------------------------------------------------------------------
    Call platform specific SendDSMChain(...) and return
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->SendDSMChain( &tempAddr,
                                           dsmItemPtrPtr,
                                           flags,
                                           numWrittenPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("UDPSocket::SendToDSMChain(): "
                    "SendDSMChain failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }

  PS_PKT_META_INFO_FREE( &pktMetaInfoPtr);

  critSectPtr->Leave();

  LOG_MSG_INFO4_2 ("UDPSocket::SendToDSMChain(): "
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

  LOG_MSG_ERROR_2("UDPSocket::SendToDSMChain(): "
                  "Sock 0x%x err 0x%x", this, dsErrno);

  critSectPtr->Leave();
  return dsErrno;

} /* UDPSocket::SendToDSMChain() */


ds::ErrorType CDECL UDPSocket::RecvFromDSMChain
(
  dsm_item_type **       dsmItemPtrPtr,
  ds::SockAddrStorageType   remoteAddrPtr,
  unsigned int           flags,
  int32 *                numReadPtr
)
{
  SockAddrInternalType  remoteSockAddr = {{0}};
  SockAddrInternalType  tempAddr = {{0}};
  ds::ErrorType         dsErrno;
  ds::ErrorType         tmpDSErrno;
  ds::AddrFamilyType   _family = ds::AddrFamily::QDS_AF_UNSPEC;
  uint32                readEventIndex = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Only MSG_ERRQUEUE is supported by RecvMSg
  -------------------------------------------------------------------------*/
  if (0 != ( flags & ~( RecvFlags::QDS_MSG_ERRQUEUE)))
  {
    return QDS_EOPNOTSUPP;
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
    Return error if socket is blocked for reading
      1. Return EWOULDBLOCK if socket is flow controlled
      2. ASSERT if err mask is unknown
  -------------------------------------------------------------------------*/
  if (AEE_SUCCESS != GetIndexFromEvent(SocketEvent::QDS_EV_READ, &readEventIndex)){
    LOG_MSG_ERROR_1("UDPSocket::RecvFromDSMChain(): "
                    "wrong event type, sock 0x%x", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }

  if (0 != eventBitMask[readEventIndex])
  {
    if (0 != ( FLOW_CONTROL_EVENT_BIT_MASK & eventBitMask[readEventIndex]))
    {
      LOG_MSG_INFO1_2("UDPSocket::RecvFromDSMChain(): "
                      "Read is blocked, sock 0x%x err mask 0x%x",
                      this, eventBitMask[readEventIndex]);
      dsErrno = AEE_EWOULDBLOCK;
      goto bail;
    }
    else
    {
      LOG_MSG_INFO1_2("UDPSocket::RecvFromDSMChain(): "
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
  dsErrno = platformSockPtr->RecvDSMChain( (remoteAddrPtr) ? (&tempAddr) : 0,
                                           dsmItemPtrPtr,
                                           flags,
                                           numReadPtr);


  if (0 != remoteAddrPtr)
  {
     memscpy((void *) remoteAddrPtr,
             sizeof(ds::SockAddrStorageType),
             (void *) &tempAddr,
             sizeof(ds::SockAddrStorageType));

     (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&_family);
     (void)ds::Sock::AddrUtils::SetFamily(remoteAddrPtr,ps_htons(_family));
  }

  /*-------------------------------------------------------------------------
    Return ENETDOWN as error if
      1. EWOULDBLOCK is returned from platform AND
      2. Routing cache is NULL AND
      3. Socket is not connected to a peer
  -------------------------------------------------------------------------*/
  if (AEE_EWOULDBLOCK == dsErrno)
  {
    if (0 == rtMetaInfoPtr ||
        0 == PS_RT_META_GET_ROUTING_CACHE( rtMetaInfoPtr))
    {
      tmpDSErrno = platformSockPtr->GetPeerName( &remoteSockAddr);
      if (AEE_SUCCESS != tmpDSErrno)
      {
        LOG_MSG_INFO1_1("UDPSocket::RecvFromDSMChain(): "
                        "NULL routing info, sock 0x%x", this);
        dsErrno = QDS_ENETDOWN;
      }
    }
  }

  critSectPtr->Leave();

  LOG_MSG_INFO4_3("UDPSocket::RecvFromDSMChain(): "
                  "Sock 0x%x read %d err 0x%x", this, *numReadPtr, dsErrno);
  return dsErrno;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:
  critSectPtr->Leave();
  LOG_MSG_ERROR_4("UDPSocket::RecvFromDSMChain(): "
                  " sock 0x%x flags %d err 0x%x platform sock 0x%x", 
                  this, flags, dsErrno, platformSockPtr);

  return dsErrno;

} /* UDPSocket::RecvFromDSMChain() */


ds::ErrorType CDECL UDPSocket::AddIPMembership
(
  const IPMembershipInfoType * ipMembershipPtr
)
{
  SockAddrInternalType *           localAddrPtr = NULL;
  SockAddrIN6InternalType          v6LocalAddr = {0};
#ifdef FEATUTE_DATA_PS_MCAST
  ps_iface_ioctl_mcast_join_type   mcastJoinInfo;
#endif // FEATUTE_DATA_PS_MCAST
  ds::ErrorType                    dsErrno;
  ds::Net::IfaceIdType             ifaceId;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ipMembershipPtr)
  {
    LOG_MSG_INVALID_INPUT_1("UDPSocket::AddIPMembership(): "
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
    LOG_MSG_INVALID_INPUT_1("UDPSocket::AddIPMembership(): "
                            "Sock 0x%x closed", this);
    dsErrno = QDS_EINVAL;
    goto bail;
  }
#ifdef FEATUTE_DATA_PS_MCAST
  if (0 != mcastHandle)
  {
    LOG_MSG_ERROR_2("UDPSocket::AddIPMembership(): "
                    "Sock 0x%x registered for Multicast already, w/handle %d",
                    this, mcastHandle);
    dsErrno = QDS_EINVAL;
    goto bail;
  }
#endif // FEATUTE_DATA_PS_MCAST
  v6LocalAddr.family = ds::AddrFamily::QDS_AF_INET6;
  localAddrPtr = reinterpret_cast <SockAddrInternalType *> ( &v6LocalAddr);

  dsErrno = platformSockPtr->GetSockName( localAddrPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("UDPSocket::AddIPMembership(): "
                    "GetSockName failed, sock 0x%x err 0x%x", this, dsErrno);
    goto bail;
  }
#ifdef FEATUTE_DATA_PS_MCAST
  if (ds::AddrFamily::QDS_AF_INET == ipMembershipPtr->mcastGroup.family)
  {
    mcastJoinInfo.ip_addr.type    = IPV4_ADDR;
    memscpy( &( mcastJoinInfo.ip_addr.addr.v4),
             sizeof(mcastJoinInfo.ip_addr.addr.v4),
             ipMembershipPtr->mcastGroup.addr,
             sizeof(uint32));
  }

  if (ds::AddrFamily::QDS_AF_INET6 == ipMembershipPtr->mcastGroup.family)
  {
     mcastJoinInfo.ip_addr.type    = IPV6_ADDR;
     memscpy( (void*)(mcastJoinInfo.ip_addr.addr.v6),
              sizeof(mcastJoinInfo.ip_addr.addr.v6),
              (void*)(ipMembershipPtr->mcastGroup.addr),
              16);
  }

  mcastJoinInfo.port            = v6LocalAddr.port;
  mcastJoinInfo.mcast_param_ptr = NULL;
  mcastJoinInfo.handle          = 0;
#endif // FEATUTE_DATA_PS_MCAST
  ifaceId = ipMembershipPtr->ifaceId;
  if (ds::Net::INVALID_IFACE_ID == ifaceId)
  {
    /*-----------------------------------------------------------------------
      Invalid Iface ID was specified ->Try to get iface_id from the networkCtl
    -----------------------------------------------------------------------*/
    if (0 == networkCtlPtr)
    {
      LOG_MSG_ERROR_2("UDPSocket::AddIPMembership(): "
                      "Sock 0x%x Failed to get ifaceid %d", this, ifaceId);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }

    dsErrno = networkCtlPtr->GetIfaceId( &ifaceId);
    if (AEE_SUCCESS != dsErrno || ds::Net::INVALID_IFACE_ID == ifaceId)
    {
      LOG_MSG_ERROR_3("UDPSocket::AddIPMembership(): "
                      "Sock 0x%x Failed to get ifaceid %d from netCtl obj 0x%x",
                      this, ifaceId, networkCtlPtr);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }
  }
#ifdef FEATUTE_DATA_PS_MCAST
  dsErrno =
    NetPlatform::IfaceIoctlByIfaceId( ifaceId,
                                      NetPlatform::IFACE_IOCTL_MCAST_JOIN,
                                      static_cast < void *> ( &mcastJoinInfo));
  if (AEE_SUCCESS != dsErrno)
  {
    goto bail;
  }

  mcastHandle = mcastJoinInfo.handle;
#endif // FEATUTE_DATA_PS_MCAST
  critSectPtr->Leave();
  return AEE_SUCCESS;

bail:
  critSectPtr->Leave();
  return dsErrno;

} /* UDPSocket::AddIPMembership() */


ds::ErrorType CDECL UDPSocket::DropIPMembership
(
  const IPMembershipInfoType * ipMembershipPtr
)
{
#ifdef FEATUTE_DATA_PS_MCAST
  ps_iface_ioctl_mcast_leave_type  mcastLeaveInfo;
#endif // FEATUTE_DATA_PS_MCAST
  ds::ErrorType                    dsErrno;
  ds::Net::IfaceIdType             ifaceId;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == ipMembershipPtr)
  {
    LOG_MSG_INVALID_INPUT_1("UDPSocket::DropIPMembership(): "
                            "NULL arg, sock 0x%x", this);
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

#ifdef FEATUTE_DATA_PS_MCAST
  if (0 == mcastHandle)
  {
    dsErrno = AEE_SUCCESS;
    goto bail;
  }

  mcastLeaveInfo.handle = mcastHandle;
#endif // FEATUTE_DATA_PS_MCAST
  ifaceId = ipMembershipPtr->ifaceId;
  if (ds::Net::INVALID_IFACE_ID == ifaceId)
  {
    /*-----------------------------------------------------------------------
      Invalid Iface ID was specified ->Try to get iface_id from the networkCtl
    -----------------------------------------------------------------------*/
    if (0 == networkCtlPtr)
    {
      LOG_MSG_ERROR_2("UDPSocket::DropIPMembership(): "
                      "Sock 0x%x Failed to get ifaceid %d", this, ifaceId);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }

    dsErrno = networkCtlPtr->GetIfaceId( &ifaceId);
    if (AEE_SUCCESS != dsErrno || ds::Net::INVALID_IFACE_ID == ifaceId)
    {
      LOG_MSG_ERROR_3("UDPSocket::DropIPMembership(): "
                      "Sock 0x%x Failed to get ifaceid %d from netCtl obj 0x%x",
                      this, ifaceId, networkCtlPtr);
      dsErrno = QDS_EOPNOTSUPP;
      goto bail;
    }
  }
#ifdef FEATUTE_DATA_PS_MCAST
  dsErrno =
    NetPlatform::IfaceIoctlByIfaceId( ifaceId,
                                      NetPlatform::IFACE_IOCTL_MCAST_LEAVE,
                                      static_cast < void *> ( &mcastLeaveInfo));
  if (AEE_SUCCESS != dsErrno)
  {
    goto bail;
  }

  mcastHandle = 0;
#endif // FEATUTE_DATA_PS_MCAST
  critSectPtr->Leave();
  return AEE_SUCCESS;

bail:
  critSectPtr->Leave();
  return dsErrno;

} /* UDPSocket::DropIPMembership() */


ds::ErrorType CDECL UDPSocket::GetDoSAckInfo
(
  DoSAckStatusType *  dosAckStatusPtr,
  int              *  overflowPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == dosAckStatusPtr || 0 == overflowPtr)
  {
    LOG_MSG_INVALID_INPUT_1("UDPSocket::GetDoSAckInfo(): "
                            "NULL arg, sock 0x%x", this);
    return QDS_EFAULT;
  }

  critSectPtr->Enter();

  /*-------------------------------------------------------------------------
    Update out parameters
  -------------------------------------------------------------------------*/
  *dosAckStatusPtr = dosAckStatus;
  *overflowPtr     = overflow;

  /*-------------------------------------------------------------------------
    Reset to default values
  -------------------------------------------------------------------------*/
  dosAckStatus = DoSAckStatus::DOSACK_NONE;
  overflow     = 0;

  critSectPtr->Leave();

  LOG_MSG_FUNCTION_EXIT_3("UDPSocket::GetDoSAckInfo(): "
                          "Success, sock 0x%x status %d overflow %d",
                          this, *dosAckStatusPtr, *overflowPtr);
  return AEE_SUCCESS;
} /* UDPSocket::GetDoSAckInfo() */


/*===========================================================================

                     PROTECTED MEMBER FUNCTIONS

===========================================================================*/
inline bool UDPSocket::IsMulticastSupported
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return true;
} /* UDPSocket::IsMulticastSupported() */


bool UDPSocket::IsConnectSupported
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  ds::ErrorType *              dsErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    UDP supports both disconnecting a socket and calling Connect() more than
    once on a socket. So always return true without performing any checks.

    Clear routing cache so that route scope is recalculated. Otherwise, socket
    will not be able to receive data from all the interfaces which match
    it's netpolicy
  -------------------------------------------------------------------------*/
  ResetRoutingCache(true);

  LOG_MSG_FUNCTION_EXIT_1("UDPSocket::IsConnectSupported(): "
                          "Success, sock 0x%x", this);
  return true;

} /* UDPSocket::IsConnectSupported() */


bool UDPSocket::IsOptSupported
(
  OptLevelType  optLevel,
  OptNameType   optName
)
{
  bool  isOptSupported;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (OptLevel::QDS_LEVEL_IP == optLevel || OptLevel::QDS_LEVEL_IPV6 == optLevel)
  {
    isOptSupported = true;
  }
  else
  {
    switch (optName)
    {
      case OptName::QDS_SO_REUSEADDR:
      case OptName::QDS_SO_ERROR_ENABLE:
      case OptName::QDS_SO_ERROR:
      case OptName::QDS_SO_TX_IFACE:
      case OptName::QDS_SO_RCVBUF:
      case OptName::QDS_SO_SNDBUF:
      case OptName::QDS_SO_FIONREAD:
      case OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
      case OptName::QDS_UDP_ENCAPS:
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

  LOG_MSG_FUNCTION_EXIT_4("UDPSocket::IsOptSupported(): "
                          "Sock 0x%x level %d, name %d ret %d",
                          this, optLevel, optName, isOptSupported);
  return isOptSupported;

} /* UDPSocket::IsOptSupported() */

bool UDPSocket::IsPktInfoDifferent
(
  const ds::SockAddrInternalType* remoteAddrPtr
)
{
  uint16                   remotePort;
  ds::AddrFamilyType addrFamily = ds::AddrFamily::QDS_AF_UNSPEC;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)ds::Sock::AddrUtils::GetFamily(remoteAddrPtr,&addrFamily);

  if (false == Socket::IsPktInfoDifferent( remoteAddrPtr))
  {
    if (ds::AddrFamily::QDS_AF_INET6 == addrFamily)
    {
      remotePort = remoteAddrPtr->v6.port;
    }
    else
    {
      remotePort = remoteAddrPtr->v4.port;
    }

    if (rtMetaInfoPtr->pkt_info.ptcl_info.udp.dst_port != remotePort)
    {
      return true;
    }

    return false;
  }

  return true;
} /* UDPSocket::IsPktInfoDifferent() */


ds::ErrorType UDPSocket::FillProtocolInfoInPktInfo
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  const ds::SockAddrIN6InternalType *  v6LocalAddrPtr,
  ip_pkt_info_type *       pktInfoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == v6RemoteAddrPtr || 0 == v6LocalAddrPtr || 0 == pktInfoPtr)
  {
    LOG_MSG_ERROR_1("UDPSocket::FillProtocolInfoInPktInfo(): "
                    "NULL args, sock 0x%x", this);
    ASSERT( 0);
    return QDS_EFAULT;
  }

  if (IP_V6 == pktInfoPtr->ip_vsn)
  {
    pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPPROTO_UDP;
  }
  else
  {
    pktInfoPtr->ip_hdr.v4.protocol = (uint8) PS_IPPROTO_UDP;
  }

  pktInfoPtr->ptcl_info.udp.src_port = v6LocalAddrPtr->port;
  pktInfoPtr->ptcl_info.udp.dst_port = v6RemoteAddrPtr->port;

  return AEE_SUCCESS;
} /* UDPSocket::FillProtocolInfoInPktInfo() */


void UDPSocket::ProcessNetworkConfigChangedEvent
(
  ds::ErrorType  reasonForChange
)
{
  SockAddrInternalType   remoteAddr = {{0}};
  ds::ErrorType          dsErrno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  (void)ds::Sock::AddrUtils::SetFamily(&remoteAddr,
                                       ds::AddrFamily::QDS_AF_INET6);

  /*-------------------------------------------------------------------------
    Make sure that platform socket is not closed yet. Else if platform socket
    is deleted, a 0 address would be accessed causing a crash
  -------------------------------------------------------------------------*/
  if (0 == platformSockPtr)
  {
    LOG_MSG_INFO1_1("UDPSocket::ProcessNetworkConfigChangedEvent(): "
                    "Sock 0x%x closed", this);
    return;
  }

  /*-------------------------------------------------------------------------
    Disconnect the platform socket as local address has changed
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->Connect( &remoteAddr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("UDPSocket::ProcessNetworkConfigChangedEvent(): "
                    "Connect failed, sock 0x%x err 0x%x", this, dsErrno);
    return;
  }

  if (QDS_ENETDOWN == reasonForChange)
  {
    ResetRoutingCache(true);
  }

  return;
} /* UDPSocket::ProcessNetworkConfigChangedEvent() */


void UDPSocket::ProcessDoSAckEvent
(
  Event::DoSAckEventInfo *  dosAckEventInfoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( 0 != dosAckEventInfoPtr);

  dosAckStatus = dosAckEventInfoPtr->dosAckStatus;
  overflow     = dosAckEventInfoPtr->overflow;

  SetOrClearEvent( SocketEvent::QDS_EV_DOS_ACK);
  return;

} /* UDPSocket::ProcessDoSAckEvent() */


/*===========================================================================

                         PRIVATE MEMBER FUNCTIONS

===========================================================================*/
void * UDPSocket::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_UDP_SOCKET_TYPE);
} /* UDPSocket::operator new() */


