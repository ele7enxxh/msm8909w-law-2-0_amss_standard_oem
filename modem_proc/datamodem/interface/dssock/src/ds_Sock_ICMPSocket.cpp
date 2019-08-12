/*===========================================================================
  FILE: ds_Sock_ICMPSocket.cpp

  OVERVIEW: This file provides implementation of the ICMPSocket class.

  DEPENDENCIES: None

                  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_ICMPSocket.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-14 msr Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Sock_ICMPSocket.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ps_mem.h"

using namespace ds::Sock;
using namespace ds::Error;


/*===========================================================================

                     PUBLIC MEMBER FUNCTIONS

===========================================================================*/
ICMPSocket * ICMPSocket::CreateInstance
(
 ds::AddrFamilyType    _family,
  NetworkCtl           *pNetworkCtl
)
{
  ICMPSocket *  icmpSocketPtr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_1("ICMPSocket::CreateInstance(): "
                           "Family %d", _family);

  /*-------------------------------------------------------------------------
    Create a ICMP Socket and initialize it
  -------------------------------------------------------------------------*/
  icmpSocketPtr= new ICMPSocket();
  if (0 == icmpSocketPtr)
  {
    LOG_MSG_ERROR_0("ICMPSocket::CreateInstance(): "
                    "No mem for ICMP Sock");
    goto bail;
  }

  if (AEE_SUCCESS != icmpSocketPtr->Init(_family,
                                         Type::QDS_DGRAM,
                                         Protocol::QDS_ICMP,
                                         pNetworkCtl))
  {
    goto bail;
  }

  return icmpSocketPtr;

  /*-------------------------------------------------------------------------
    Common error handling code - Delete the Socket instance if it was created
  -------------------------------------------------------------------------*/
bail:
  DS_UTILS_RELEASEIF(icmpSocketPtr);

  return 0;
} /* ICMPSocket::CreateInstance() */


ds::ErrorType CDECL ICMPSocket::Shutdown
(
  ShutdownDirType  shutdownDir
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return QDS_EOPNOTSUPP;
} /* ICMPSocket::Shutdown() */


/*===========================================================================

                      PROTECTED MEMBER FUNCTIONS

===========================================================================*/
bool ICMPSocket::IsConnectSupported
(
  const ds::SockAddrIN6InternalType *    v6RemoteAddrPtr,
  ds::ErrorType *                dsErrnoPtr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == dsErrnoPtr)
  {
    LOG_MSG_ERROR_1("ICMPSocket::IsConnectSupported(): "
                    "NULL errno, sock 0x%x", this);
    ASSERT( 0);
    return false;
  }

  /*-------------------------------------------------------------------------
    ICMP doesn't support Connect(). So always return false
  -------------------------------------------------------------------------*/
  *dsErrnoPtr = QDS_EINVAL;

  LOG_MSG_FUNCTION_EXIT_1("ICMPSocket::IsConnectSupported(): "
                          "Success, sock 0x%x", this);
  return false;

} /* ICMPSocket::IsConnectSupported() */


bool ICMPSocket::IsOptSupported
(
  OptLevelType  optLevel,
  OptNameType   optName
)
{
  bool  isOptSupported;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (OptLevel::QDS_LEVEL_ICMP == optLevel)
  {
    isOptSupported = true;
  }
  else
  {
    switch (optName)
    {
      case OptName::QDS_IP_TOS:
      case OptName::QDS_IP_TTL:
      case OptName::QDS_IPV6_TCLASS:
      case OptName::QDS_SO_REUSEADDR:
      case OptName::QDS_SO_ERROR:
      case OptName::QDS_SO_ERROR_ENABLE:
      case OptName::QDS_SO_TX_IFACE:
      case OptName::QDS_SO_RCVBUF:
      case OptName::QDS_SO_SNDBUF:
      case OptName::QDS_SO_FIONREAD:
      case OptName::QDS_SO_SKIP_ROUTE_SCOPE_CHECK:
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

  LOG_MSG_FUNCTION_EXIT_4("ICMPSocket::IsOptSupported(): "
                          "Sock 0x%x level %d, name %d ret %d", this, optLevel, optName, isOptSupported);
  return isOptSupported;

} /* ICMPSocket::IsOptSupported() */

bool ICMPSocket::IsOptPrivSupported
(
  OptLevelType    optLevel,
  OptNameTypePriv optName
)
{
  bool  isOptSupported;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (OptLevel::QDS_LEVEL_ICMP == optLevel)
  {
    isOptSupported = true;
  }
  else
  {
    isOptSupported = false;
  }

  LOG_MSG_FUNCTION_EXIT_4("ICMPSocket::IsOptPrivSupported(): "
                          "Sock 0x%x level %d, name %d ret %d", this, optLevel, optName, isOptSupported);
  return isOptSupported;

} /* ICMPSocket::IsOptPrivSupported() */

bool ICMPSocket::IsSetNetPolicySupported
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return true;
} /* ICMPSocket::IsSetNetPolicySupported() */


ds::ErrorType ICMPSocket::FillProtocolInfoInPktInfo
(
  const ds::SockAddrIN6InternalType *  v6RemoteAddrPtr,
  const ds::SockAddrIN6InternalType *  v6LocalAddrPtr,
  ip_pkt_info_type *       pktInfoPtr
)
{
  ds::ErrorType  dsErrno;
  int32          optVal;
  int32          optLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == v6RemoteAddrPtr || 0 == v6LocalAddrPtr || 0 == pktInfoPtr)
  {
    LOG_MSG_ERROR_1("ICMPSocket::FillProtocolInfoInPktInfo(): "
                    "NULL args, sock 0x%x", this);
    ASSERT( 0);
    return QDS_EFAULT;
  }

  if (IP_V6 == pktInfoPtr->ip_vsn)
  {
    pktInfoPtr->ip_hdr.v6.hdr_type = PS_IPPROTO_ICMP;
  }
  else
  {
    pktInfoPtr->ip_hdr.v4.protocol = (uint8) PS_IPPROTO_ICMP;
  }

  /*-------------------------------------------------------------------------
    Populate ICMP type by fetching it from platform
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetOptPriv( OptLevel::QDS_LEVEL_ICMP,
                                         OptNamePriv::QDS_ICMP_TYPE,
                                         &optVal,
                                         &optLen);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("ICMPSocket::FillProtocolInfoInPktInfo(): "
                    "GetOpt failed for ICMP_TYPE, sock 0x%x err 0x%x",
                    this, dsErrno);
    return dsErrno;
  }

  pktInfoPtr->ptcl_info.icmp.type = static_cast <uint8> ( optVal);

  /*-------------------------------------------------------------------------
    Populate ICMP code by fetching it from platform
  -------------------------------------------------------------------------*/
  dsErrno = platformSockPtr->GetOptPriv( OptLevel::QDS_LEVEL_ICMP,
                                         OptNamePriv::QDS_ICMP_CODE,
                                         &optVal,
                                         &optLen);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_2("ICMPSocket::FillProtocolInfoInPktInfo(): "
                    "GetOpt failed for ICMP_CODE, sock 0x%x err 0x%x",
                    this, dsErrno);
    return dsErrno;
  }

  pktInfoPtr->ptcl_info.icmp.code = static_cast <uint8> ( optVal);

  return AEE_SUCCESS;
} /* ICMPSocket::FillProtocolInfoInPktInfo() */


void ICMPSocket::ProcessNetworkConfigChangedEvent
(
  ds::ErrorType  reasonForChange
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (QDS_ENETDOWN == reasonForChange)
  {
    ResetRoutingCache(true);
  }

  return;
} /* ICMPSocket::ProcessNetworkConfigChangedEvent() */


/*===========================================================================

                         PRIVATE MEMBER FUNCTIONS

===========================================================================*/
void * ICMPSocket::operator new
(
  unsigned int numBytes
) throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_mem_get_buf( PS_MEM_ICMP_SOCKET_TYPE);
} /* ICMPSocket::operator new() */

