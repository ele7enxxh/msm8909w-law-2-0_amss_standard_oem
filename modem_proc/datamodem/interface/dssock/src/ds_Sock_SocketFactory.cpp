/*===========================================================================
  FILE: ds_Sock_SocketFactory.cpp

  OVERVIEW: This file provides implementation of the SocketFactory class.

  DEPENDENCIES: None

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_SocketFactory.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2010-05-23 vm Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


#include "ds_Sock_SocketFactory.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Sock_TCPSocket.h"
#include "ds_Sock_UDPSocket.h"
#include "ds_Sock_ICMPSocket.h"
#include "ps_mem.h"

#include "ds_Sock_NetworkCtl.h"
#include "ds_Sock_Sock1NetworkCtl.h"
#include "ds_Sock_Sock2NetworkCtl.h"

using namespace ds::Sock;
using namespace ds::Error;
using namespace ds::Utils;
using namespace ds::Net;

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Declaration of static member of SocketFactory class
---------------------------------------------------------------------------*/
SocketFactory * SocketFactory::instance = 0;

/*===========================================================================

                          CONSTRUCTOR / DESTRUCTOR

===========================================================================*/
SocketFactory::SocketFactory
(
  void
)
throw()
: refCnt(0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("SocketFactory::SocketFactory(): "
                   "Creating SocketFactory 0x%p", this);

} /* SocketFactory::SocketFactory() */


SocketFactory::~SocketFactory
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO1_1 ("SocketFactory::~SocketFactory(): "
                   "Deleting SocketFactory 0x%p", this);
  
  instance = 0;

} /* SocketFactory::~SocketFactory() */

/*===========================================================================

                         PUBLIC MEMBER FUNCTIONS

===========================================================================*/
SocketFactory * SocketFactory::CreateInstance
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate a SocketFactory object if it is not already allocated.

    Since factory pattern is used, a new object is not allocated each time
    CreateInstance() is called
  -------------------------------------------------------------------------*/
  if (0 == instance)
  {
    instance = new SocketFactory();
    if (0 == instance)
    {
      DATA_ERR_FATAL("SocketFactory::CreateInstance(): No mem for SocketFactory");
      return 0;
    }
  }

  (void)instance->AddRef();

  return instance;

} /* SocketFactory::Instance() */

ds::ErrorType CDECL SocketFactory::CreateSocketByPolicy
(
  ds::AddrFamilyType  family,
  SocketType      socketType,
  ProtocolType    protocol,
  IPolicy *       netPolicyPtr,
  ISocket **      newSockPtrPtr
)
{
  ds::ErrorType  dsErrno;
  NetworkCtl *networkCtlPtr = NULL;
  ds::Sock::ISocketExt *pIDSSokectExt = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("SocketFactory::CreateSocketByPolicy(): "
                           "Family %d type %d proto %d",
                           family, socketType, protocol);

  /*-------------------------------------------------------------------------
    Create Network Controller
  -------------------------------------------------------------------------*/
  networkCtlPtr = static_cast <NetworkCtl *>
                  (
                    Sock2NetworkCtl::CreateInstance()
                  );

  if (0 == networkCtlPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create socket with a default policy
  -------------------------------------------------------------------------*/
  dsErrno = CreateSocketInternal( family, 
                                  socketType, 
                                  protocol, 
                                  newSockPtrPtr,
                                  networkCtlPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("SocketFactory::CreateSocketByPolicy(): "
                    "Couldn't create sock, err %d", dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Update socket's policy to the user passed in policy
  -------------------------------------------------------------------------*/
  if(0 != netPolicyPtr)
  {
    dsErrno = (*newSockPtrPtr)->QueryInterface
      (
        ds::Sock::AEEIID_ISocketExt,
        (void**)&(pIDSSokectExt)
      );
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_ERROR_1("SocketFactory::CreateSocketByPolicy(): "
                      "QueryInterface failed: %d", dsErrno);
      goto bail;
    }

    dsErrno = pIDSSokectExt->SetNetPolicy( netPolicyPtr);
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_INFO3_2("SocketFactory::CreateSocketByPolicy(): "
                      "Couldn't set policy, sock 0x%p err %d",
                      *newSockPtrPtr, dsErrno);
      
      (void) pIDSSokectExt->Release();
      
      goto bail;
    }

    (void) pIDSSokectExt->Release();
  }

  (void) networkCtlPtr->Release();

  LOG_MSG_FUNCTION_EXIT_1("SocketFactory::CreateSocketByPolicy(): "
                          "Returning sock 0x%p", *newSockPtrPtr);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:

  DS_UTILS_RELEASEIF(networkCtlPtr);
  DS_UTILS_RELEASEIF(*newSockPtrPtr);

  return dsErrno;

} /* SocketFactory::CreateSocketByPolicy() */


ds::ErrorType CDECL SocketFactory::CreateSocketByNetwork
(
  ds::AddrFamilyType  family,
  SocketType      socketType,
  ProtocolType    protocol,
  INetwork *      dsNetObjPtr,
  ISocket **      newSockPtrPtr
)
{
  Socket *       sockPtr = NULL;
  ds::ErrorType  dsErrno = 0;
  NetworkCtl     *networkCtlPtr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("SocketFactory::CreateSocketByNetwork(): "
                           "Family %d type %d proto %d",
                           family, socketType, protocol);

  if (0 == dsNetObjPtr)
  {
    LOG_MSG_INVALID_INPUT_0("SocketFactory::CreateSocketByNetwork(): "
                            "NULL net obj");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Create Network Controller
  -------------------------------------------------------------------------*/
  networkCtlPtr = static_cast <NetworkCtl *>
                  (
                    Sock1NetworkCtl::CreateInstance()
                  );

  if (0 == networkCtlPtr)
  {
    dsErrno = AEE_ENOMEMORY;
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Create socket
  -------------------------------------------------------------------------*/
  dsErrno = CreateSocketInternal( family, 
                                  socketType, 
                                  protocol, 
                                  newSockPtrPtr,
                                  networkCtlPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_INFO3_1("SocketFactory::CreateSocketByNetwork(): "
                    "Couldn't create sock, err %d", dsErrno);
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Force socket to use interface pointed to by the user passed in network
    object.

    Since SetNetwork is not a method of ISocket interface, ISocket object is
    cast to Socket object
  -------------------------------------------------------------------------*/
  sockPtr = reinterpret_cast <Socket *> ( *newSockPtrPtr);
  dsErrno = sockPtr->SetNetwork( dsNetObjPtr);
  if (AEE_SUCCESS != dsErrno)
  {
    LOG_MSG_ERROR_2("SocketFactory::CreateSocketByNetwork(): "
                    "Couldn't set network, sock 0x%p err %d",
                    *newSockPtrPtr, dsErrno);
    goto bail;
  }

  ( void) networkCtlPtr->Release();

  LOG_MSG_FUNCTION_EXIT_1("SocketFactory::CreateSocketByNetwork(): "
                          "Returning sock 0x%p", *newSockPtrPtr);
  return AEE_SUCCESS;

  /*-------------------------------------------------------------------------
    Common error handling code
  -------------------------------------------------------------------------*/
bail:

  DS_UTILS_RELEASEIF(networkCtlPtr);
  DS_UTILS_RELEASEIF(*newSockPtrPtr);

  return dsErrno;

} /* SocketFactory::CreateSocketByNetwork() */


ds::ErrorType SocketFactory::DeleteSocket
(
  Socket *  sockPtr
)
 throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == sockPtr)
  {
    DATA_ERR_FATAL("SocketFactory::DeleteSocket(): NULL arg");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Remove object from the list of socket objects
  -------------------------------------------------------------------------*/
  RemoveItem( static_cast <INode *> ( sockPtr));
  
  return AEE_SUCCESS;

} /* SocketFactory::DeleteSocket() */

ds::ErrorType CDECL SocketFactory::CreateSocketInternal
(
  ds::AddrFamilyType    family,
  SocketType            socketType,
  ProtocolType          protocol,
  ISocket **            newSockPtrPtr,
  NetworkCtl *          networkCtlPtr
)
{
  Socket *  sockPtr = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_3("SocketFactory::CreateSocketInternal(): "
                           "Family %d type %d proto %d",
                           family, socketType, protocol);

  /*-------------------------------------------------------------------------
    Validate the input.
    1. Family must be either INET or INET6
    2. Socket type must be either STREAM or DGRAM
    3. Protocol must be one of TCP, UDP, ICMP, or UNSPEC
    4. Protocol could be unspecified in which case TCP is used for STREAM
       sockets and UDP is used for DGRAM sockets
    5. (socketType, protocol) tuple must be either (STREAM, TCP), or
       (DGRAM, UDP), or (DGRAM, ICMP)
    6. newSockPtrPtr must be non-NULL
  -------------------------------------------------------------------------*/
  if (ds::AddrFamily::QDS_AF_INET != family && ds::AddrFamily::QDS_AF_INET6 != family)
  {
    LOG_MSG_INVALID_INPUT_1("SocketFactory::CreateSocketInternal(): "
                            "Invalid family %d", family);
    return QDS_EAFNOSUPPORT;
  }

  if (Type::QDS_STREAM != socketType && Type::QDS_DGRAM != socketType)
  {
    LOG_MSG_INVALID_INPUT_1("SocketFactory::CreateSocketInternal(): "
                            "Invalid type %d", socketType);
    return QDS_ESOCKNOSUPPORT;
  }

  if (Protocol::QDS_UNSPEC == protocol)
  {
    protocol = (socketType == Type::QDS_STREAM) ? Protocol::QDS_TCP : Protocol::QDS_UDP;
  }

  if (false == ( Protocol::QDS_TCP == protocol ||
      Protocol::QDS_UDP == protocol ||
      Protocol::QDS_ICMP == protocol))
  {
    LOG_MSG_INVALID_INPUT_1("SocketFactory::CreateSocketInternal(): "
                            "Invalid proto %d", protocol);
    return QDS_EPROTONOSUPPORT;
  }

  if (( Type::QDS_STREAM == socketType && Protocol::QDS_TCP != protocol) ||
      ( Type::QDS_DGRAM == socketType &&
      ( Protocol::QDS_UDP != protocol && Protocol::QDS_ICMP != protocol)))
  {
    LOG_MSG_INVALID_INPUT_2("SocketFactory::CreateSocketInternal(): "
                            "Invalid (type %d, proto %d) combo",
                            socketType, protocol);
    return QDS_EPROTOTYPE;
  }

  if (0 == newSockPtrPtr)
  {
    LOG_MSG_INVALID_INPUT_0("SocketFactory::CreateSocketInternal(): "
                            "NULL newSockPtrPtr");
    return QDS_EFAULT;
  }

  /*-------------------------------------------------------------------------
    Create socket
  -------------------------------------------------------------------------*/
  if (Protocol::QDS_TCP == protocol)
  {
    sockPtr = static_cast <Socket *> ( TCPSocket::CreateInstance( family,
                                                                  networkCtlPtr));
  }
  else if (Protocol::QDS_UDP == protocol)
  {
    sockPtr = static_cast <Socket *> (UDPSocket::CreateInstance( family,
                                                                 networkCtlPtr));
  }
  else
  {
    sockPtr = static_cast <Socket *> ( ICMPSocket::CreateInstance( family,
                                                                   networkCtlPtr));
  }

  if (0 == sockPtr)
  {
    LOG_MSG_ERROR_0("SocketFactory::CreateSocketInternal(): "
                    "No mem for Sock");
    return QDS_EMFILE;
  }

  /*-------------------------------------------------------------------------
  Add socket to the list of Socket objects
  -------------------------------------------------------------------------*/
  (void) AddItem( static_cast <INode *> ( sockPtr));

  *newSockPtrPtr = static_cast <ISocket *> ( sockPtr);
  LOG_MSG_FUNCTION_EXIT_1("SocketFactory::CreateSocketInternal(): "
                          "Returning sock 0x%p", *newSockPtrPtr);
  return AEE_SUCCESS;

} /* SocketFactory::CreateSocketInternal() */

