/*======================================================

FILE:  DSS_dsapi.cpp

SERVICES:

GENERAL DESCRIPTION:
Implementation of legacy ds API - entry point for legacy applications.

=====================================================

Copyright (c) 2008 - 2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_dsapi.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-11-01 rk  Removed feature mobileap.
  2010-10-28 aa  Support for truncate flags added to dss_recvmsg and dss_recvfrom
  2010-04-13 en  History added.

===========================================================================*/

// TODO: if anything here needs locking, it should use the relevant DSSSocket's
// critical section - using DSSSocket::GetCritSect().


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include <stringl/stringl.h>
#include "DSS_Common.h"
#include "customer.h"
#include "dssocket.h"
#include "dssicmp_api.h"
#include "dserrno.h"
#include "DSS_Conversion.h"
#include "ds_Utils_StdErr.h"
#include "ds_Utils_Conversion.h"
#include "IDSMUtils.h"
#include "ds_Net_CreateInstance.h"
#include "ds_Sock_Def.h"
#include "ds_Sock_IICMPErrInfoPriv.h"
#include "ds_Sock_IRecvIFInfoPriv.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "ds_Sock_AddrUtils.h"
#include "DSS_Socket.h"
#include "DSS_IDSNetPolicyPrivScope.h"
#include "DSS_IDSNetPolicyScope.h"
#include "DSS_MemoryManagement.h"
#include "ds_Addr_Def.h"
#include "DSS_GenScope.h"
#include "ds_Utils_SockAddrInternalTypes.h"
#include "ps_in.h"
#include "ds_Net_CreateInstance.h"
#include "DSS_IDSNetworkScope.h"
#include "DSS_IDSNetActiveScope.h"
#include "ps_system_heap.h"
#include "dss_errors_def.h"

using namespace ds::Net;
using namespace ds::Sock;
using namespace dss::Error;
#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

#ifndef DSS_MAX_SINT15
#define DSS_MAX_SINT15 32767
#endif

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

const struct ps_in6_addr ps_in6addr_any = PS_IN6ADDR_ANY_INIT;            /* :: */
const struct ps_in6_addr ps_in6addr_loopback = PS_IN6ADDR_LOOPBACK_INIT ; /* ::1 */

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------
// Structures for notifying ICMP errors to applications
typedef struct {
  dss_sock_extended_err ee;
  struct ps_sockaddr_in ps_sin_addr;
} dssocki_err_msg_in;

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

extern "C"
{
//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------
void dssocki_net_cb(sint15 dss_nethandle, dss_iface_id_type iface_id, sint15 dss_errno, void *user_data_ptr);
void dssocki_sock_cb(sint15 dss_nethandle, sint15 sockfd, uint32 event_mask, void *user_data_ptr);

//===================================================================
//                 ds API Functions
//===================================================================
/*==========================================================================
FUNCTION DSS_OPEN_NETLIB()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress.  The callback are called with a pointer
  to a sint15 containing the application ID for the callback.
  NOTE: the memory for the application ID is ephemeral and likely will not be
    available after the callback returns - if it is desired to use this
    information outside the scope of the callback it should be COPIED, a
    pointer MUST not be used.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

  This function is wrapper around dss_opennetlib2. This calls opennetlib2
  with standard network and socket call back functions and user data as
  the user specified call back functions. The standard socket and network
  call back functions call the user specified call back functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.

SIDE EFFECTS
  Puts data services manager into "socket" mode.
===========================================================================*/
sint15 dss_open_netlib
(
  void   (*net_callback_fcn)(void *),         /* network callback function */
  void   (*socket_callback_fcn)(void *),       /* socket callback function */
  sint15 *dss_errno                               /* error condition value */
)
{
  void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
  LOG_MSG_FUNCTION_ENTRY_3("dss_open_netlib(): "
                            "net_callback_fcn 0x%p, socket_callback_fcn 0x%p, caller 0x%p",
                            net_callback_fcn, socket_callback_fcn, caller_ptr);
   /*lint -save -e611 we need to cast one function type to another, for that we first cast net/socket_callback_fcn to "void*" and then cast it back to another
     function type, using helper functions dssocki_net/sock_cb */
   return dss_open_netlib2(dssocki_net_cb, (void *)net_callback_fcn,
                           dssocki_sock_cb, (void *)socket_callback_fcn,
                           NULL, dss_errno);

   /*lint -restore Restore lint error 611*/
}

/*===========================================================================
FUNCTION DSS_OPEN_NETLIB2()

DESCRIPTION

  Opens up the network library.  Assigns application ID and sets the
  application-defined callback functions to be called when library and
  socket calls would make progress. Stores the network policy info and
  uses it in further calls.

  Puts data services manager into "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns application ID on success.

  On error, return DSS_SUCCESS and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EMAPP      no more applications available - max apps exceeded.
  DS_EFAULT     Policy structure is uninitialized.

SIDE EFFECTS
  Puts data services manager into "socket" mode.
===========================================================================*/
sint15 dss_open_netlib2
(
  dss_net_cb_fcn net_cb,                      /* network callback function */
  void * net_cb_user_data,              /* User data for network call back */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,               /*User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 * dss_errno                              /* error condition value */
)
{
   DSSNetApp* pNetApp = NULL;
   dss_sock_cb_fcn_type socketCallback;
   DSSIDSNetPolicyScope IDSNetPolicyScope;
   INetworkFactory* pIDSNetworkFactory = NULL;
   INetwork* pIDSNetwork = NULL;
   sint15 netHandle = -1;
   AEEResult res = AEE_SUCCESS;
   bool netAppListFull = false;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_4("dss_open_netlib2(): "
                            "net_cb 0x%p, sock_cb 0x%p policy_info ptr 0x%p,errno 0x%x",
                            net_cb, sock_cb, policy_info_ptr,dss_errno);

   // check whether dss_errno is NULL
   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   IDS_ERR_RET_ERRNO(IDSNetPolicyScope.Init());

   // Checking if there is any space left
   netAppListFull = DSSGlobals::Instance()->NetAppListFull();
   if (true == netAppListFull) {
      LOG_MSG_ERROR_0("dss_open_netlib2(): "
                      "No more applications are available");
      *dss_errno = DS_EMAPP;
      return DSS_ERROR;
   }

   res = DSSConversion::DS2IDSNetPolicy(policy_info_ptr, IDSNetPolicyScope.Fetch());
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_0("dss_open_netlib2(): "
                      "Can't convert Net Policy");
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return DSS_ERROR;
   }

   res = DSSGlobals::Instance()->GetNetworkFactory(&pIDSNetworkFactory);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return DSS_ERROR;
   }
   DSSGenScope scopeNetworkFactory(pIDSNetworkFactory, DSSGenScope::IDSIQI_TYPE);

   res = pIDSNetworkFactory->CreateNetwork(ds::Net::NetworkMode::QDS_MONITORED,
   IDSNetPolicyScope.Fetch(), &pIDSNetwork);
   if (AEE_SUCCESS != res) {
      DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_CREATE_NETWORK_FAILED;
      goto bail;
   }

   pNetApp = DSSNetApp::CreateInstance();
   if (NULL == pNetApp) {
      dss_error_code = DSS_ERROR_DSSNETAPP_ALLOCATION_FAILED;
      DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
      *dss_errno = DS_ENOMEM;
      goto bail;
   }

   // DSSNetApp takes care of AddRef pIDSNetwork once it is set
   res = pNetApp->Init(pIDSNetwork);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_DSSNETAPP_INITIALIZATION_FAILED;
      (void) pNetApp->Release();
      DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      goto bail;
   }

   netHandle = DSSGlobals::Instance()->InsertNetApp(pNetApp);
   if (-1 == netHandle) {
      DATA_ERR_FATAL("dss_open_netlib2(): Internal error: Failed to add new NetApp to apps list");
      *dss_errno = DS_EFAULT;
      goto bail;
   }
   else {
      LOG_MSG_INFO1_1("dss_open_netlib2(): "
                      "New app ID %d", netHandle);
   }

   pNetApp->SetNetHandle(netHandle);
   socketCallback.sock_cb_fcn = sock_cb;
   socketCallback.sock_cb_user_data = sock_cb_user_data;
   pNetApp->SetSocketCallback(&socketCallback);

   pNetApp->SetNetworkCallback(net_cb);
   pNetApp->SetNetworkUserData(net_cb_user_data);

   res = pNetApp->SetPolicy(IDSNetPolicyScope.Fetch());
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_POLICY_FAILED;
      
      if (AEE_SUCCESS != DSSGlobals::Instance()->RemoveNetApp(netHandle)) {
         dss_error_code = DSS_ERROR_DSSNETAPP_DELETION_FAILED;
      }
      (void) pNetApp->Release();
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      goto bail;
   }

   res = pNetApp->SetLegacyPolicy(policy_info_ptr);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_POLICY_FAILED;
      
      if (AEE_SUCCESS != DSSGlobals::Instance()->RemoveNetApp(netHandle)) {
         dss_error_code = DSS_ERROR_DSSNETAPP_DELETION_FAILED;
      }
      (void) pNetApp->Release();
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      goto bail;
   }

   pNetApp->SetNumOfSockets(0);

   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);

   LOG_MSG_FUNCTION_EXIT_1( "dss_open_netlib2(): "
                            "netHandle 0x%x", netHandle);
   return netHandle;

bail:
   LOG_MSG_ERROR_2("dss_open_netlib2(): failed"
                   "error %d  res %d",dss_error_code, res);
   return DSS_ERROR;
}

/*===========================================================================
FUNCTION DSS_SOCKET()

DESCRIPTION
 Create a socket and related data structures,and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using this call are bound to the dss_nethandle used in
         creating this socket.

DEPENDENCIES
  The function dss_open_netlib() must be called to open the network library
  and put the ds/PS managers into sockets mode.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  invalid or unsupported protocol
  DS_EMFILE           too many descriptors open.  A socket is already open
                      or has not closed completely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket
(
  sint15 dss_nethandle,
  byte   family,                      /* Address family - DSS_AF_INET only */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSNetApp *pNetApp = NULL;
   DSSNetMonitoredScope netMonitoredScope;
   ds::Sock::ISocketFactory *pSockFactory = NULL;
   ds::AddrFamilyType IDSAddrFamily;
   ds::Sock::SocketType IDSSockType;
   ds::Sock::ProtocolType IDSProtocol;
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   INetwork* pIDSNetwork = NULL;
   DSSIDSNetPolicyScope IDSNetPolicyScope;
   dss_sock_cb_fcn_type sockCb;
   sint15 sSockFd, sRet;
   AEEResult res = AEE_SUCCESS;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_4("dss_socket(): "
                            "NetHandle: %d, family:%d, type:%d, caller 0x%p",
                            dss_nethandle, family, type, caller_ptr);

   if (NULL == dss_errno) {
     dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetNetApp(dss_nethandle, &pNetApp);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_INVALID_APPLICATION_ID;
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   pNetApp->GetSocketCallback(&sockCb);

   res = DSSConversion::DS2IDSAddrFamily((int) family,&IDSAddrFamily);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_ADDRESS_FAMILY_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }
   res = DSSConversion::DS2IDSSockType((int)type, &IDSSockType);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_SOCKET_TYPE_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }
   if (0 == protocol) {
      protocol = ((byte)DSS_SOCK_STREAM == type) ? (byte)PS_IPPROTO_TCP:(byte)PS_IPPROTO_UDP;
   }
   res = DSSConversion::DS2IDSProtocol((int)protocol, &IDSProtocol);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_PROTOCOL_TYPE_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->CreateSocket(sockCb, dss_nethandle,
      &sSockFd, &pDSSSocket, 1, protocol);

   if (AEE_ENOMEMORY == res || 0 == pDSSSocket) {
      res = QDS_EMFILE;
   }

   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   DSSGlobals::Instance()->GetSockFactory(&pSockFactory);
   if (NULL == pSockFactory) {
      DATA_ERR_FATAL("dss_socket: pSockFactory is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_FACTORY;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void) netMonitoredScope.Init(pNetApp);
   res = netMonitoredScope.Fetch()->GetIDSNetworkObject(&pIDSNetwork);
   if (AEE_SUCCESS != res) {
      LOG_MSG_ERROR_1("dss_socket(): "
                      "Failed to fetch IDSNetwork object: %d", res);
   }
   if (NULL != pIDSNetwork) {
      res = pSockFactory->CreateSocketByNetwork(IDSAddrFamily,
         IDSSockType, IDSProtocol, pIDSNetwork, &pIDSSocket);
   }
   else {
      res = IDSNetPolicyScope.Init(pNetApp); //TODO: Is this correct? do we need this check if we have the second one
      if (AEE_SUCCESS != res) { //If Init failed then we will check if the member of the scop is NULL (?)
         LOG_MSG_ERROR_0("dss_socket(): "
                         "Warning: There is no policy set ");
      }

      res = pSockFactory->CreateSocket(
                                         IDSAddrFamily,
                                         IDSSockType,
                                         IDSProtocol,
                                         IDSNetPolicyScope.Fetch(),
                                         &pIDSSocket
                                      );
   }

   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }

   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SOCKET_CREATION_FAILED;
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);

      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->SetIDSSocket(pIDSSocket);

   sRet = sSockFd;

bail:
   DSSCommon::ReleaseIf((IQI**)&pSockFactory);
   DSSCommon::ReleaseIf((IQI**)&pIDSNetwork);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_2( "dss_socket(): "
                            "socket %d, error %d", sRet,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_SOCKET2()

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

 The mapping to actual protocols is as follows:

 ADDRESS FAMILY         Stream          Datagram

 DSS_AF_INET                TCP             UDP

  Note this function must be called to obtain a valid socket descriptor, for
  use with all other socket-related functions.  Thus, before any socket
  functions can be used (e.g. I/O, asynchronous notification, etc.), this
  call must have successfully returned a valid socket descriptor.  The
  application must also have made a call to dss_open_netlib() to obtain
  a valid application ID, and to put the Data Services task into "sockets"
  mode.

  Note:  This implementation version has no support for Raw IP sockets, and
         will return an error, if the application attempts to create one.

         Sockets created using socket2 are not bound to any particular dss_nethandle.

DEPENDENCIES
  Netpolicy structure needs to be initialized by calling dss_init_netpolicy.

RETURN VALUE
  On successful creation of a socket, this function returns socket file
  descriptor which is a sint15 value between 0x1000 and 0x1FFF.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EAFNOSUPPORT     address family not supported
  DS_EBADAPP          invalid application ID
  DS_EPROTOTYPE       specified protocol invalid for socket type
  DS_ESOCKNOSUPPORT   invalid or unsupported socket parameter specified
  DS_EPROTONOSUPPORT  specified protocol not supported
  DS_EMFILE           too many descriptors open.  A socket is already open or
                      has not closed completely.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_socket2
(
  byte   family,                                         /* Address family */
  byte   type,                                              /* socket type */
  byte   protocol,                                        /* protocol type */
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
)
{
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocketFactory *pSockFactory = NULL;
   ds::AddrFamilyType IDSAddrFamily;
   ds::Sock::SocketType IDSSockType;
   ds::Sock::ProtocolType IDSProtocol;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   ds::Sock::ISocket *pIDSSocket = NULL;
   sint15 sSockFd;
   sint15 sRet = DSS_SUCCESS;
   AEEResult res;
   ds::Net::IPolicyPriv* pIPolicy = NULL;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_4("dss_socket2(): "
                            "family:%d, type:%d, protocol:%d, caller 0x%p",
                            family, type, protocol, caller_ptr);

   // check whether dss_errno is NULL
   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSConversion::DS2IDSAddrFamily((int) family, &IDSAddrFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_ADDRESS_FAMILY_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }
   res = DSSConversion::DS2IDSSockType((int)type, &IDSSockType);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SOCKET_TYPE_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }
   if (0 == protocol) {
      protocol = ((byte)DSS_SOCK_STREAM == type) ? (byte)PS_IPPROTO_TCP:(byte)PS_IPPROTO_UDP;
   }
   res = DSSConversion::DS2IDSProtocol((int)protocol, &IDSProtocol);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_PROTOCOL_TYPE_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL != policy_info_ptr) {
      res = IDSNetPolicyPrivScope.Init();
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_NET_POLICY_CREATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }

      res = DSSConversion::DS2IDSNetPolicy(policy_info_ptr, IDSNetPolicyPrivScope.Fetch());
      if (AEE_SUCCESS != res) {
         dss_error_code= DSS_ERROR_NET_POLICY_CONVERSION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   dss_sock_cb_fcn_type sockCb;
   sockCb.sock_cb_fcn = sock_cb;
   sockCb.sock_cb_user_data = sock_cb_user_data;
   res = DSSGlobals::Instance()->CreateSocket(sockCb, -1, &sSockFd, &pDSSSocket, 2, protocol);
   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }
   if ((AEE_SUCCESS != res) || (NULL == pDSSSocket)){
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((NULL != policy_info_ptr)) {
      res = pDSSSocket->SetLegacyPolicy(policy_info_ptr);
      if (AEE_SUCCESS != res) {
         (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   DSSGlobals::Instance()->GetSockFactory(&pSockFactory);
   if (NULL == pSockFactory) {
      DATA_ERR_FATAL("dss_socket2: pSockFactory is NULL"); 
      dss_error_code = DSS_ERROR_NULL_SOCKET_FACTORY;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   pIPolicy = IDSNetPolicyPrivScope.Fetch();

   res = pSockFactory->CreateSocket(
                                     IDSAddrFamily,
                                     IDSSockType,
                                     IDSProtocol,
                                     pIPolicy,
                                     &pIDSSocket
                                   );

   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }

   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SOCKET_CREATION_FAILED;

      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);

      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->SetIDSSocket(pIDSSocket);

   sRet = sSockFd;

bail:
   DSSCommon::ReleaseIf((IQI**)&pSockFactory);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_2( "dss_socket2(): "
                            "socket 0x%x, error %d ", sRet, dss_error_code);
   return sRet;
}

sint15 dssicmp_socket
(
  dssicmp_icmp_ver_type  icmp_ver,
  uint8  type,
  uint8  code,
  dss_sock_cb_fcn sock_cb,                     /* socket callback function */
  void * sock_cb_user_data,              /* User data for socket call back */
  dss_net_policy_info_type * policy_info_ptr,       /* Network policy info */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocketFactory *pSockFactory = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSPrivSokect = NULL;
   sint15 sSockFd;
   AEEResult res;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   ds::AddrFamilyType IDSAddrFamily;
   ds::Sock::SocketType IDSSockType;
   ds::Sock::ProtocolType IDSProtocol;
   ds::Sock::OptLevelType IDSLevel;
   ds::Sock::OptNameTypePriv IDSOptName;
   byte protocol = 0;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_3("dssicmp_socket(): type:%u, code:%u, errno 0x%x", type, code, dss_errno);

   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   if(icmp_ver == DSSICMP_ICMP_V4){
      protocol = (byte)PS_IPPROTO_ICMP;
   }
   else{
      protocol = (byte)PS_IPPROTO_ICMP6;
   }

   dss_sock_cb_fcn_type sockCb;
   sockCb.sock_cb_fcn = sock_cb;
   sockCb.sock_cb_user_data = sock_cb_user_data;
   res = DSSGlobals::Instance()->CreateSocket(sockCb, -1, &sSockFd, &pDSSSocket, 2, protocol);
   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }
   if ((AEE_SUCCESS != res) || (NULL == pDSSSocket)){
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      return DSS_ERROR;
   }

   if (NULL != policy_info_ptr) {
      res = pDSSSocket->SetLegacyPolicy(policy_info_ptr);
      if (AEE_SUCCESS != res) {
         (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         return DSS_ERROR;
      }
   }

   if(icmp_ver != DSSICMP_ICMP_V4 && icmp_ver != DSSICMP_ICMP_V6)
   {
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      *dss_errno = DS_EFAULT;
       return DSS_ERROR;
   }

   if (DSSICMP_ICMP_V4 == icmp_ver) {
      IDSAddrFamily =ds::AddrFamily::QDS_AF_INET;
   } else {
      IDSAddrFamily = ds::AddrFamily::QDS_AF_INET6;
   }

   IDSSockType = ds::Sock::Type::QDS_DGRAM;

   IDSProtocol =  ds::Sock::Protocol::QDS_ICMP;

   DSSGlobals::Instance()->GetSockFactory(&pSockFactory);
   if (NULL == pSockFactory) {
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      DATA_ERR_FATAL("dssicmp_socket: pSockFactory is NULL");
      *dss_errno = DS_EFAULT;
      return DSS_ERROR;
   }

   if (NULL != policy_info_ptr) {
      res = IDSNetPolicyPrivScope.Init();
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("dssicmp_socket(): "
                         "Can't create DSNetPolicy");
         (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);;
         return DSS_ERROR;
      }

      res = DSSConversion::DS2IDSNetPolicy(policy_info_ptr, IDSNetPolicyPrivScope.Fetch());
      if (AEE_SUCCESS != res) {
         LOG_MSG_ERROR_0("dssicmp_socket(): "
                         "Can't convert Net Policy");
         (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         return DSS_ERROR;
      }
   }

   res = pSockFactory->CreateSocket(
                                    IDSAddrFamily,
                                    IDSSockType,
                                    IDSProtocol,
                                    IDSNetPolicyPrivScope.Fetch(),
                                    &pIDSSocket
                                   );

   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }

   DSSGenScope scopeSocket(pIDSSocket,DSSGenScope::IDSIQI_TYPE);

   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SOCKET_CREATION_FAILED;
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sSockFd = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&(pIDSPrivSokect));
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sSockFd = DSS_ERROR;
      goto bail;
   }

   IDSLevel = ds::Sock::OptLevel::QDS_LEVEL_ICMP;
   IDSOptName = ds::Sock::OptNamePriv::QDS_ICMP_CODE;

   res = pIDSPrivSokect->SetOptPriv(IDSLevel, IDSOptName,(int)code);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_SOCKET_OPT_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      sSockFd = DSS_ERROR;
      goto bail;
   }

   IDSOptName = ds::Sock::OptNamePriv::QDS_ICMP_TYPE;

   res = pIDSPrivSokect->SetOptPriv(IDSLevel, IDSOptName,(int)type);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_SOCKET_OPT_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      (void) DSSGlobals::Instance()->RemoveSocket(sSockFd);
      sSockFd = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->SetIDSSocket(pIDSSocket);

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSPrivSokect);
   LOG_MSG_FUNCTION_EXIT_3( "dssicmp_socket(): "
                            "socket %d, error %d, dss_error = %d", sSockFd, dss_error_code,*dss_errno);
   return sSockFd;
}

/*===========================================================================
FUNCTION DSS_BIND()

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding to a local IPv4 address, but rather ONLY a local
  port number.  The local IPv4 address is assigned automatically by the sockets
  library.  This function does support binding to a local IPv6 address,
  however, as this is required for using IPv6 Privacy Extensions (RFC 3041).
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_EAFNOSUPPORT     address family not supported
  DS_EOPNOTSUPP       operation not supported
  DS_EADDRINUSE       the local address is already in use.
  DS_EINVAL           the socket is already attached to a local name
  DS_EFAULT           invalid address and/or invalid address length

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_bind
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_sockaddr *localaddr,                          /* local address */
  uint16 addrlen,                                     /* length of address */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_SUCCESS;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType      tempFamily;
   int tmpDSFamily;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_bind(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (AEE_SUCCESS != res)
   {
      LOG_MSG_ERROR_1("dss_bind(): "
                      "GetSocketById failed:%d ", res);
   }

   if (NULL == pDSSSocket) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_bind: pIDSSocket is NULL");
      *dss_errno = DS_EFAULT;
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      sRet = DSS_ERROR;
      goto bail;
   }

   // There is a backward compatibility issue here: the old API supports unbinding and the new API does not
   if (NULL == localaddr) {
      dss_error_code = DSS_ERROR_NULL_LOCAL_ADDRESS;
      *dss_errno = DS_EOPNOTSUPP;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET != localaddr->ps_sa_family &&
       DSS_AF_INET6 != localaddr->ps_sa_family)
   {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET == localaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV4_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   if (DSS_AF_INET6 == localaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in6) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV6_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // Memscpy ps_sockaddr buffer into ps_sockaddr storage buffer
   // The copy is necessary since the parameter to bind must be the specific type declared
   // for bind's parameter in the ISocket interface. This parameter is ds::SockAddrStorageType.
   // If we attempt to merely cast the provided struct ps_sockaddr to ds::SockAddrStorageType
   // and provide that as a parameter, remoting code may attempt to copy unallocated memory (depends
   // on how localaddr was allocated) which may result in an exception (because remoting code
   // expects ds::Sock::SockAddrStorageType)
   addrlen = min(addrlen,sizeof(ds::SockAddrStorageType));
   memscpy(tempSockAddr, addrlen, localaddr, addrlen);

   res = DSSConversion::DS2IDSAddrFamily(localaddr->ps_sa_family, &tempFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, ps_htons(tempFamily));

   res = pIDSSocket->Bind(tempSockAddr);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_BIND_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->GetSockName(tempSockAddr);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_GET_SOCK_NAME_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }
   // copy the address, now including the allocated port, to localaddr
   memscpy(localaddr, addrlen, tempSockAddr, addrlen);
   (void)ds::Sock::AddrUtils::GetFamily( tempSockAddr,&tempFamily);
   tempFamily = ps_ntohs(tempFamily);

   res = DSSConversion::IDS2DSAddrFamily(tempFamily,&tmpDSFamily) ;
   if (AEE_SUCCESS != res) {
    dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
    *dss_errno = DSSConversion::IDS2DSErrorCode(res);
    sRet = DSS_ERROR;
    goto bail;
   }

   localaddr->ps_sa_family = (uint16)tmpDSFamily;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_3( "dss_bind(): "
                           "sRet 0x%x, socket %d, error %d", 
                           sRet, sockfd,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_CONNECT()

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  For udp it fills in the peers ipaddr in the
  socket control block and binds the socket to an interface.
  The function must receive (as a parameter) a valid socket descriptor,
  implying a previous successful call to dss_socket().

DEPENDENCIES
  Network subsystem must be established and available.

RETURN VALUE
  Return DSS_ERROR and places the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specified
  DS_ECONNREFUSED     connection attempt refused
  DS_ETIMEDOUT        connection attempt timed out
  DS_EFAULT           addrlen parameter is invalid
  DS_EIPADDRCHANGED   IP address changed due to PPP resync
  DS_EINPROGRESS      connection establishment in progress
  DS_EISCONN          a socket descriptor is specified that is already
                      connected
  DS_ENETDOWN         network subsystem unavailable
  DS_EOPNOTSUPP       invalid server address specified
  DS_EADDRREQ         destination address is required.
  DS_NOMEM            not enough memory to establish connection

SIDE EFFECTS
  For TCP, initiates active open for connection.
===========================================================================*/
sint15 dss_connect
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *servaddr,                     /* destination address */
  uint16 addrlen,                                    /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res;
   sint15 sRet = DSS_SUCCESS;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType tempFamily;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_connect(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_connect: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == servaddr) {
      dss_error_code = DSS_ERROR_NULL_SERVER_ADDRESS;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET != servaddr->ps_sa_family && DSS_AF_INET6 != servaddr->ps_sa_family){
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET == servaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV4_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   if (DSS_AF_INET6 == servaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in6) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV6_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // Memscpy ps_sockaddr buffer into ps_sockaddr storage buffer
   addrlen = min(addrlen,sizeof(ds::SockAddrStorageType));
   memscpy(tempSockAddr, addrlen, servaddr, addrlen);

   res = DSSConversion::DS2IDSAddrFamily(servaddr->ps_sa_family, &tempFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, ps_htons(tempFamily));

   res = pIDSSocket->Connect(tempSockAddr);
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_CONNECT_OPERATION_IN_PROGRESS;
      } else if (QDS_EISCONN == res) {
         dss_error_code = DSS_ERROR_SOCKET_ALREADY_CONNECTED;
      } else {
         dss_error_code = DSS_ERROR_CONNECT_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_3( "dss_connect(): "
                            "socket %d, ret 0x%x, error %d", 
                            sockfd, sRet, dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_LISTEN()

DESCRIPTION

  For TCP, this starts a passive open for connections.  Upon a
  successful connection, the socket callback function is invoked
  asserting DS_ACCEPT_EVENT as TRUE.  The application should respond
  with a call to dss_accept(). If a connection is received and there
  are no free queue slots the new connection is rejected
  (QDS_ECONNREFUSED).  The backlog queue is for ALL unaccepted sockets
  (half-open, or completely established).

  A listening UDP doesn't make sense, and as such isn't supported.
  DS_EOPNOTSUPP is returned.

  The sockfd parameter is a created (dss_socket) and bound (dss_bind)
  socket that will become the new listening socket.  The backlog
  parameter indicates the maximum length for the queue of pending
  sockets.  If backlog is larger than the maximum, it will be
  reduced to the maximum (see DSS_SOMAXCONN). This is the BSD behavior.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind) .

RETURN VALUE

  Returns DSS_SUCCESS on success.  If the backlog was truncated
  DS_EFAULT will be set in errno, but the call will still be
  successful.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block (PJ: I don't think this CAN happen)
  DS_EBADF            invalid socket descriptor is specified
  DS_EOPNOTSUPP       The socket is not capable of listening (UDP)
  DS_EFAULT           backlog parameter is invalid
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.
  DS_EINVAL           Socket already open, closed, unbound or not one
                      you can listen on.

SIDE EFFECTS
  For TCP, initiates passive open for new connections.
===========================================================================*/
sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res;
   sint15 sRet = DSS_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_listen(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_listen: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   // Check for errors in backlog parameter - It must be greater than 0
   if (0 >= backlog) {
      dss_error_code = DSS_ERROR_INVALIED_BACKLOG_PARAMETER;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   // Silently truncate queue to SOMAXCONN
   if (DSS_SOMAXCONN < backlog) {
      LOG_MSG_INFO1_0("dss_listen(): "
                      "Backlog parameter is too high");
      *dss_errno = DS_EFAULT;
      backlog = DSS_SOMAXCONN;
   }

   res = pIDSSocket->Listen((long)backlog);
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_LISTEN_OPERATION_IN_PROGRESS;
      } else {
        dss_error_code = DSS_ERROR_LISTEN_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_3( "dss_listen(): "
                            "socket %d, ret 0x%x, error %d" ,
                            sockfd, sRet,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_ACCEPT()

DESCRIPTION

  The accept function is used on listening sockets to respond when
  DS_ACCEPT_EVENT is asserted.  The first backlog queued connection is
  removed from the queue, and bound to a new connected socket (as if
  you called dss_socket).  The newly created socket is in the
  connected state.  The listening socket is unaffected and the queue size is
  maintained (i.e. there is not need to call listen again.)

  The argument sockfd is the file descriptor of the listening socket

  The argument remote addr is a pointer to a struct ps_sockaddr.  This
  structure is populated with the address information for the remote
  end of the new connection. addrlen should initially contain the
  length of the struct ps_sockaddr passed in.  The length of the real
  address is placed in this location when the struct is populated.

  The argument dss_error should point to a memory location in which
  error conditions can be recorded.

DEPENDENCIES

  Network subsystem must be established and available.

  The sockfd should get a valid socket descriptor (implying a
  previously successful call to dss_socket) This socket should be
  bound to a specific port number (implying a previously successful
  call to dss_bind).  The socket should be listening (implying a
  previously successful call to dss_listen).

RETURN VALUE
  Returns the socket descriptor of the new socket on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block
  DS_EBADF            invalid socket descriptor is specified
  DS_EOPNOTSUPP       The socket is not of type DSS_SOCK_STREAM
  DS_EINVAL           Socket is not listening.
  DS_EFAULT           The addr parameter is bogus.
  DS_ENETNONET        network subsystem unavailable for some unknown reason
  DS_ENETINPROGRESS   network subsystem establishment currently in progress
  DS_ENETCLOSEINPROGRESS network subsystem close in progress.
  DS_NOMEM            not enough memory to establish backlog connections.

SIDE EFFECTS

  The head backlog item from the queue of the listening socket is
  removed from that queue.
===========================================================================*/
sint15 dss_accept
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct ps_sockaddr *remoteaddr,                    /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL, *pNewDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL, *pNewIDSSocket = NULL;
   AEEResult res;
   sint15 sRet = DSS_SUCCESS;
   ds::SockAddrStorageType tempSockAddr;
   int tmpDSFamily;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   size_t copylen;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_accept(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      ASSERT(0);
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   // Verify remoteaddr and addrlen are valid
   if ((NULL == remoteaddr) || (NULL == addrlen)) {
      dss_error_code = DSS_ERROR_NULL_REMOTE_ADDRESS_OR_NULL_ADDRLEN;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   memset(tempSockAddr, 0, sizeof(tempSockAddr));

   res = pIDSSocket->Accept(tempSockAddr, &pNewIDSSocket);
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_ACCEPT_OPERATION_IN_PROGRESS;
      } else {
         dss_error_code = DSS_ERROR_ACCEPT_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::GetFamily(tempSockAddr,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, tmpDSFamily);


   if (ds::AddrFamily::QDS_AF_INET == family) {
      if (sizeof(struct ps_sockaddr_in) < *addrlen){
         *addrlen = sizeof(struct ps_sockaddr_in);
      }
   }
   else if (ds::AddrFamily::QDS_AF_INET6 == family) {
      if (sizeof(struct ps_sockaddr_in6) < *addrlen){
         *addrlen = sizeof(struct ps_sockaddr_in6);
      }
   }
   else {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   copylen = min(*addrlen,sizeof(ds::SockAddrStorageType));
   memscpy(remoteaddr, copylen, tempSockAddr, copylen);

   // Fetch the NetApp of the socket in order to assign it later to the NetApp field of the new DSS socket.
   sint15 sNewSockFd;
   int kind;
   pDSSSocket->GetSockKind(&kind);
   res = DSSGlobals::Instance()->CreateSocket(pDSSSocket->GetCb(), pDSSSocket->GetNetApp(),
                                              &sNewSockFd, &pNewDSSSocket, kind, PS_IPPROTO_TCP);

   if (AEE_ENOMEMORY == res) {
      res = QDS_EMFILE;
   }

   if ((AEE_SUCCESS != res) || (NULL == pNewDSSSocket)){
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   pNewDSSSocket->SetIDSSocket(pNewIDSSocket);

   sRet = sNewSockFd;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   DSSCommon::ReleaseIf((IQI**)&pNewIDSSocket);
   LOG_MSG_FUNCTION_EXIT_3( "dss_accept(): "
                            "socket %d, ret 0x%x, error %d",
                            sockfd, sRet, dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_ASYNC_SELECT()

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function. This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

DEPENDENCIES
  None.

RETURN VALUE

  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified

SIDE EFFECTS
  Sets the relevant event mask in the socket control block.  Will also
  notify the application via the callback function.

IMPORTAND NOTE REGARDING BACKWARD COMPATIBILITY
  In legacy code, whenever dss_async_select is invoked, and appropriate event can be asserted - it is asserted, regardless if
  dss_getnextevent was called afterwards, meaning that we can call several dss_async_select-s in a raw and each time get the event asserted if
  it can be asserted

  In current implementation if 2 dss_async_select-s are invoked, then the second call would cause events to be asserted *only* if the callback for the
  first dss_async_select was invoked, before user called the second dss_async_select. In order to reach full compatibilty with the legacy code, we will
  have to enable checking the status of signal buses in DSNet/DSSock layers inside DSS.
===========================================================================*/
sint31 dss_async_select
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 interest_mask,                        /* bitmask of events to set */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   AEEResult res;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_async_select(): "
                            "socket:%d, bitmask:%d, caller 0x%p",
                            sockfd, interest_mask, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      goto bail;
   }

   res = pDSSSocket->RegEvent(interest_mask);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      goto bail;
   }

   return DSS_SUCCESS;

bail:
   LOG_MSG_ERROR_2("dss_async_select(): failed"
                   "socket %d, error %d", sockfd, dss_error_code);
   return DSS_ERROR;
}

/*===========================================================================
FUNCTION DSS_ASYNC_DESELECT()

DESCRIPTION
  Clears events of interest in the socket control block interest mask.  The
  application specifies a bitmask of events that it wishes to clear; events
  for which it will no longer receive notification.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified

SIDE EFFECTS
  Clears specified events from the relevant event mask.
===========================================================================*/
sint15 dss_async_deselect
(
  sint15 sockfd,                                      /* socket descriptor */
  sint31 clr_interest_mask,                  /* bitmask of events to clear */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   AEEResult res;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_async_deselect(): "
                            "socket:%d, clear_bitmask:%d, caller 0x%p",
                            sockfd, clr_interest_mask, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      goto bail;
   }

   res = pDSSSocket->DeRegEvent(clr_interest_mask);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
       goto bail;
   }

   return DSS_SUCCESS;

bail:
   LOG_MSG_ERROR_2("dss_async_deselect():failed"
                   "socket %d, error %d", sockfd, dss_error_code);
   return DSS_ERROR;
}

/*===========================================================================
FUNCTION DSS_GETNEXTEVENT()

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket control block's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  dss_async_select().  The application may pass in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket

  Alternatively, the application may set this pointer's value to NULL (0)
  (note, not to be confused with a NULL pointer, but rather a pointer whose
  value is 0) in which case the function will return values for the next
  available socket.  The next available socket's descriptor will be placed
  in the socket descriptor pointer, and the function will return.  If no
  sockets are available (no events have occurred across all sockets for
  that application) the pointer value will remain NULL (originally value
  passed in), and the function will return 0, indicating that no events
  have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  Returns an event mask of the events that were asserted for sockets whose
  app ID matches that passed in.  A value of 0 indicates that no events have
  occurred.

  On passing a pointer whose value is NULL into the function for
  the socket descriptor (not to be confused with a NULL pointer), places
  the next available socket descriptor in *sockfd_ptr and returns the
  event mask for that socket. If no sockets are available (no events have
  occurred across all sockets for that application) the pointer value
  will remain NULL (originally value passed in), and the function will
  return 0, indicating that no events have occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP           invalid app descriptor is specified
  DS_EBADF             invalid socket descriptor is specified

SIDE EFFECTS
  Clears the bits in the socket control block event mask, corresponding to
  the events that have occurred.
===========================================================================*/
sint31 dss_getnextevent
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *sockfd_ptr,                                 /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   sint15 socketNetHandle;
   uint32 eventOccurred, sigOnOff, relevantEvents;
   AEEResult res;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_getnextevent(): "
                            "net_handle:%d, socket:%d, caller 0x%p",
                            dss_nethandle,
                            (NULL == sockfd_ptr)? 0:(*sockfd_ptr),
                            caller_ptr);

   if (NULL == dss_errno) {
     dss_error_code = DSS_ERROR_NULL_ERRNO;
     goto bail;
   }

   if (NULL == sockfd_ptr) {
     dss_error_code = DSS_ERROR_NULL_SOCKFD_PTR;
     *dss_errno = DS_EBADF;
     goto bail;
   }

   if (!DSSGlobals::Instance()->IsValidNetApp(dss_nethandle)) {
      dss_error_code = DSS_ERROR_INVALID_APPICALTION_DESCRIPTOR;
      *dss_errno = DS_EBADAPP;
      goto bail;
   }

   if (0 != *sockfd_ptr) {
      res = DSSGlobals::Instance()->GetSocketById(*sockfd_ptr, &pDSSSocket);
      if (NULL == pDSSSocket || AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
         *dss_errno = DS_EBADF;
         goto bail;
      }
      socketNetHandle = pDSSSocket->GetNetApp();
      if (socketNetHandle != dss_nethandle) {
         dss_error_code = DSS_ERROR_SOCKET_APPLICATION_MISMATCH;
         *dss_errno = DS_EBADF;
         goto bail;
      }
      pDSSSocket->GetEventOccurredMask(&eventOccurred);
      pDSSSocket->GetSigOnOffMask(&sigOnOff);

      relevantEvents = eventOccurred & sigOnOff;
      if (relevantEvents) {
         pDSSSocket->SetSigOnOffMask(sigOnOff & ~relevantEvents);
         pDSSSocket->SetEventOccurredMask(eventOccurred & ~relevantEvents);
         return relevantEvents;
      }
   }
   else {
      res = DSSGlobals::Instance()->FindSocketWithEvents(dss_nethandle, sockfd_ptr, &eventOccurred, &sigOnOff);
      if (0 != *sockfd_ptr && AEE_SUCCESS == res) {
         res = DSSGlobals::Instance()->GetSocketById(*sockfd_ptr, &pDSSSocket);
         if(AEE_SUCCESS == res)
         {
            LOG_MSG_ERROR_1("dss_getnextevent(): "
                            "GetSocketById failed:%d", res);
         }
         relevantEvents = eventOccurred & sigOnOff;
         if (relevantEvents) {
            pDSSSocket->SetSigOnOffMask(sigOnOff & ~relevantEvents);
            pDSSSocket->SetEventOccurredMask(eventOccurred & ~relevantEvents);
            return relevantEvents;
         }
      }
   }

   // No events have occurred, so just return DSS_SUCCESS.
   return DSS_SUCCESS;

bail:
   LOG_MSG_ERROR_2("dss_getnextevent():failed"
                   "socket %d, error %d", 
                   (NULL == sockfd_ptr)? 0:(*sockfd_ptr),dss_error_code);
   return DSS_ERROR;
}

/*===========================================================================
FUNCTION DSS_READ()

DESCRIPTION
  Reads up to nbytes of data into buffer from the transport specified
  by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_read
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,                     /* user buffer to which to copy data */
  uint16 nbytes,                 /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   int nBufLenReq = 0;
   AEEResult res;
   sint15 sRet = DSS_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   
   LOG_MSG_FUNCTION_ENTRY_2("dss_read(): socket:%d, errno 0x%x", sockfd,dss_errno);

   if (NULL == dss_errno) {
      return DSS_ERROR;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_read: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == buffer && 0 != nbytes) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
   }

   // the maximal size of data supported is DSS_MAX_SINT15
   // in case user provides larger buffer -
   // silently truncating it
   if (DSS_MAX_SINT15 < nbytes){
      LOG_MSG_INFO1_1("dss_read(): "
                      "Can't handle more than DSS_MAX_SINT15 bytes"
                      "user provided [%d], truncating", nbytes);
      nbytes = DSS_MAX_SINT15;
   }

   res = pIDSSocket->Read((byte*)buffer, (int)nbytes, &nBufLenReq);
   if (AEE_SUCCESS != res && QDS_EEOF != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_READ_OPERATION_IN_PROGRESS;
      } else {
         dss_error_code = DSS_ERROR_READ_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   *dss_errno = DS_EEOF;
   sRet = nBufLenReq;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
  LOG_MSG_FUNCTION_EXIT_4("dss_read(): "
                          "socket %d, ret 0x%x, dss_errno = %d, error %d",
                          sockfd, sRet, *dss_errno,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_READV()

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read, which could be less than the number of
      bytes specified. A return of 0 indicates that an End-of-File condition
      has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ------------
  DS_EBADF            invalid socket descriptor is specified
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_readv
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],        /* array of data buffers to copy data into */
  sint15 iovcount,               /* number of bytes to be read from socket */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSPrivSokect = NULL;
   IPort1::SeqBytes *bufs = NULL;
   int nBufLenReq = 0;
   int i = 0;
   AEEResult res;
   sint15 sRet;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_readv(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_readv: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == iov) {
      if (0 == iovcount) {
         sRet = 0;
      }
      else {
         dss_error_code = DSS_ERROR_NULL_IOV_PARAMETER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
      }
      goto bail;
   }

   // array entities validation
   for (i = 0; i < iovcount; i++) {
      if ((NULL == iov[i].ps_iov_base) && (0 != iov[i].ps_iov_len)) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // allocate space for bufs
   if (iovcount > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(bufs, sizeof(IPort1::SeqBytes)*iovcount,
                              IPort1::SeqBytes*);
      if (NULL == bufs) {
         *dss_errno = DS_EMSGSIZE;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // fill bufs
   for (i = 0; i < iovcount; i++) {
      bufs[i].data = iov[i].ps_iov_base;
      bufs[i].dataLen = (int)iov[i].ps_iov_len;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&(pIDSPrivSokect));
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSPrivSokect->ReadV(bufs, (int)iovcount, &nBufLenReq);

   if (AEE_SUCCESS != res && QDS_EEOF != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_READV_OPERATION_IN_PROGRESS;
      } else {
         dss_error_code = DSS_ERROR_READV_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   *dss_errno = DS_EEOF;
   sRet = nBufLenReq;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSPrivSokect);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   PS_SYSTEM_HEAP_MEM_FREE (bufs);

   LOG_MSG_FUNCTION_EXIT_3( "dss_readv(): "
                            "socket %d, ret 0x%x, error %d", 
                            sockfd, sRet,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_RECVFROM()

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the transport specified by the
  socket descriptor.  Fills in address structure with values from who sent
  the data.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom
(
  sint15 sockfd,                                      /* socket descriptor */
  void   *buffer,               /* user buffer from which to copy the data */
  uint16 nbytes,                          /* number of bytes to be written */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   int nBufLenReq = 0;
   ds::SockAddrStorageType idsSockAddrStorage;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_ERROR;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   int tmpDSFamily;
   unsigned int outFlags;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   int nControlLenReq; // used as parameter to RecvMsg, but never used.
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_recvfrom(): "
                            "socket:%d, flags:%u, caller 0x%p",
                            sockfd, flags, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if ((NULL != fromaddr) && (NULL == addrlen)) {
      dss_error_code = DSS_ERROR_NULL_ADDRESS_LENGTH;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == buffer && 0 != nbytes) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
   }

   // the maximal size of data supported is DSS_MAX_SINT15
   // in case user provides larger buffer -
   // silently truncating it
   if (DSS_MAX_SINT15 < nbytes){
      LOG_MSG_INFO1_1("dss_recvfrom(): "
                      "Can't handle more than DSS_MAX_SINT15 bytes"
                      "user provided [%d], truncating", nbytes);
      nbytes = DSS_MAX_SINT15;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_recvfrom: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   memset(idsSockAddrStorage, 0, sizeof(idsSockAddrStorage));

   res = pIDSSocket->RecvMsg(idsSockAddrStorage,
                             (byte *) buffer,
                             (int) nbytes,
                              &nBufLenReq,
                              0,
                              0,
                              &nControlLenReq,
                              &outFlags,
                              flags);

   *dss_errno = DSSConversion::IDS2DSErrorCode(res);

   if (AEE_SUCCESS != res && QDS_EEOF != res) {

      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_RECVFROM_OPERATION_IN_PROGRESS;
      } else {
         dss_error_code = DSS_ERROR_RECVFROM_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if(0 != (outFlags & RecvFlags::QDS_MSG_TRUNC)) {
        *dss_errno = DS_EMSGTRUNC;
   }

   (void)ds::Sock::AddrUtils::GetFamily(idsSockAddrStorage,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      sRet = DSS_ERROR;
      res = family;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(idsSockAddrStorage, tmpDSFamily);

   if (NULL != addrlen) {
      switch (family)
      {
         case ds::AddrFamily::QDS_AF_INET:
            if (sizeof(struct ps_sockaddr_in) < *addrlen)
            {
               *addrlen = sizeof(struct ps_sockaddr_in);
            }
            break;
         case ds::AddrFamily::QDS_AF_INET6:
            if (sizeof(struct ps_sockaddr_in6) < *addrlen)
            {
               *addrlen = sizeof(struct ps_sockaddr_in6);
            }
            break;

         default:
            LOG_MSG_ERROR_1("dss_recvfrom: The address family must be INET or INET6 %i",
                            family);
            ASSERT(0);
      }
   }

   if (NULL != addrlen){
      size_t copylen = min(sizeof(ds::SockAddrStorageType), *addrlen);

      if (NULL != fromaddr){
         memscpy(fromaddr, copylen, idsSockAddrStorage, copylen);
      }
      else{
         *addrlen = 0;
      }
   }

   sRet = nBufLenReq;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_recvfrom(): "
                            "socket %d, ret 0x%x, error %d, res %d", 
                            sockfd, sRet,dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSsocketPriv = NULL;
   IPort1::SeqBytes *bufs = NULL;
   ds::Sock::IAncDataPriv **pOutAncData = NULL;
   struct dss_cmsghdr *cmsg_ptr = NULL;
   int nOutAncLen = 0, nBufLenReq = 0, nOutAncLenReq = 0, i = 0;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   unsigned int nOutFlags;
   int isRecvIfSet = 0;
   int isRecvErrSet =0;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet;
   uint16 controllen  = 0;
   ds::ErrorType tempRes = AEE_SUCCESS;
   unsigned int dsFlags;
   int tmpDSFamily;
   size_t copylen;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_recvmsg(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSConversion::IDS2DSRcvMsgFlag(flags,&dsFlags);
   if (AEE_SUCCESS != res) {
       dss_error_code = DSS_ERROR_INVALID_RECVMSG_FLAG;
       *dss_errno = DSSConversion::IDS2DSErrorCode(res);
       res = flags;
       sRet = DSS_ERROR;
       goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   if(NULL == msg) {
      dss_error_code = DSS_ERROR_NULL_MSG_PARAMETER;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_recvmsg: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == msg->msg_iov) {
      if (0 == msg->msg_iovlen) {
         sRet = 0;
         // TODO: is it possible that msg_iov is NULL and msg_control is not? Currently we don't support this.
      }
      else {
         dss_error_code = DSS_ERROR_NULL_MSG_IOV_PARAMETER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
      }
      goto bail;
   }

   // array entities validation
   for (i = 0; i < msg->msg_iovlen; i++) {
      if ((NULL ==  msg->msg_iov[i].ps_iov_base) && (0 != msg->msg_iov[i].ps_iov_len)) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // allocate space for bufs

   bufs = NULL;
   if (msg->msg_iovlen > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(bufs, sizeof(IPort1::SeqBytes)*(msg->msg_iovlen),
                                IPort1::SeqBytes*);
      if (NULL == bufs) {
         dss_error_code = DSS_ERROR_BUFFER_MEMORY_ALLOCATION_FAILED;
         *dss_errno = DS_ENOMEM;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // fill bufs
   for (i = 0; i < msg->msg_iovlen; i++) {
      bufs[i].data = msg->msg_iov[i].ps_iov_base;
      bufs[i].dataLen = (int)(msg->msg_iov[i].ps_iov_len);
   }

   // calculate the number of pointers to ancillary data that we need to allocate
   // This number is an upper bound of the number of ancillary data we can populate
   // to the user according to the msg_controllen that the user provided.
   nOutAncLen = msg->msg_controllen / sizeof(struct dss_cmsghdr);
   pOutAncData = NULL;

   // allocate array of pointers of type IAncData*
   if (0 != nOutAncLen)
   {
      PS_SYSTEM_HEAP_MEM_ALLOC(pOutAncData, sizeof(ds::Sock::IAncDataPriv *)*nOutAncLen,
                                ds::Sock::IAncDataPriv**);
      if (NULL == pOutAncData) {
         LOG_MSG_ERROR_0("dss_recvmsg(): "
                         "Can't allocate pOutAncData");
         *dss_errno = DS_ENOMEM;
         PS_SYSTEM_HEAP_MEM_FREE (bufs);
         return DSS_ERROR;
      }
   }

   // We don't need the inAncData so we pass dummy parameters
   memset(tempSockAddr, 0, sizeof(tempSockAddr));
   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&pIDSsocketPriv);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     sRet = DSS_ERROR;
     goto bail;
   }
   res = pIDSsocketPriv->RecvMsg(tempSockAddr,
                                 bufs, (int)msg->msg_iovlen, &nBufLenReq,
                                 pOutAncData, nOutAncLen, &nOutAncLenReq, &nOutFlags,
                                 dsFlags);

   *dss_errno = DSSConversion::IDS2DSErrorCode(res);
   if (AEE_SUCCESS != res && QDS_EEOF != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if(0 != (nOutFlags & (RecvFlags::QDS_MSG_TRUNC | RecvFlags::QDS_MSG_CTRUNC))) {
     *dss_errno = DS_EMSGTRUNC;
   }

   (void)ds::Sock::AddrUtils::GetFamily(tempSockAddr,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      res = family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, tmpDSFamily);

   if (ds::AddrFamily::QDS_AF_INET == family) {
      if (sizeof(struct ps_sockaddr_in) < msg->msg_namelen){
         msg->msg_namelen = sizeof(struct ps_sockaddr_in);
      }
   }
   else if (ds::AddrFamily::QDS_AF_INET6 == family) {
      if (sizeof(struct ps_sockaddr_in6) < msg->msg_namelen){
         msg->msg_namelen = sizeof(struct ps_sockaddr_in6);
      }
   }
   else {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   copylen = min(msg->msg_namelen,sizeof(ds::SockAddrStorageType));

   if (NULL != msg->msg_name){
      memscpy(msg->msg_name, copylen, tempSockAddr, copylen);
   }
   else{
      msg->msg_namelen = 0;
   }

   // Check if RECVIF was set to the socket
   res = pIDSSocket->GetOpt(/*in*/ ::ds::Sock::OptLevel::QDS_LEVEL_IP,
                            /*in*/ ::ds::Sock::OptName::QDS_IP_RECVIF,
                            /*rout*/ &isRecvIfSet);
   if (AEE_SUCCESS != res)
   {
      dss_error_code = DSS_ERROR_GET_OPT_OPERATION_FAILED;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   // Check if RECVERR was set to the socket
   res = pIDSSocket->GetOpt(/*in*/ ::ds::Sock::OptLevel::QDS_LEVEL_IP,
      /*in*/ ::ds::Sock::OptName::QDS_IP_RECVERR,
      /*rout*/ &isRecvErrSet);
   if (AEE_SUCCESS != res)
   {
      dss_error_code = DSS_ERROR_GET_OPT_OPERATION_FAILED;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   // convert all the ancillary data structures that we got from RecvMsg
   for (i=0 , cmsg_ptr = DSS_CMSG_FIRSTHDR(msg) ; i < nOutAncLenReq; i++)
   {
      dssocki_err_msg_in err_info;
      ds::Sock::AncDataIDType AncID = 0;
      ds::Sock::ExtendedErrType Err;
      ds::SockAddrStorageType AddrStorage;
      ds::SockAddrIN6InternalType AddrIN6;
      unsigned int RecvIFHandle;
      dss_in_pktinfo_type    in_pktinfo;
      DSSNetApp* pNetApp;
      sint15 socketDSSNetApp = 0;
      dss_iface_id_type DSSNetAppIfaceID;

      memset(&err_info, 0, sizeof(dssocki_err_msg_in));

      if (NULL == cmsg_ptr){
         dss_error_code = DSS_ERROR_DSS_CMSG_FIRSTHDR_OR_DSS_CMSG_NXTHDR_FAILED;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }

      // Get the type of this ancillary data
      if (NULL != pOutAncData){
         res = pOutAncData[i]->GetAncID(&AncID);
         if (AEE_SUCCESS != res) {
            LOG_MSG_ERROR_1("dss_recvmsg(): "
                            "GetAncID failed:%d", res);
         }
      }

      // Extract data from pOutAncData[i] in accordance with the Ancillary data type
      switch (AncID)
      {
         case ::ds::Sock::AncData::ICMP_ERROR_INFO:
         {
            if ((DSS_MSG_ERRQUEUE & flags) && (TRUE == isRecvErrSet))
            {
               res = ((ds::Sock::IICMPErrInfoPriv*)pOutAncData[i])->GetExtendedErr(&Err);
               if (AEE_SUCCESS != res) {
                  LOG_MSG_ERROR_1("dss_recvmsg(): "
                                  "GetErr failed:%d", res);
               }

               res = ((ds::Sock::IICMPErrInfoPriv*)pOutAncData[i])->GetAddr(AddrStorage);
               if (AEE_SUCCESS != res) {
                  LOG_MSG_ERROR_1("dss_recvmsg(): "
                                  "GetAddr failed:%d", res);
               }

               (void)ds::Sock::AddrUtils::GetFamily(AddrStorage, &family);
               family = ps_ntohs(family);

               res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
               if (AEE_SUCCESS != res) {
                  dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
                  *dss_errno = DSSConversion::IDS2DSErrorCode(res);
                  res = family;
                  sRet = DSS_ERROR;
                  goto bail;
               }

               (void)ds::Sock::AddrUtils::SetFamily(AddrStorage, tmpDSFamily);

               res = ::ds::Sock::AddrUtils::GetSockAddrIN6(AddrStorage,&AddrIN6);
               if (AEE_SUCCESS != res) {
                  LOG_MSG_ERROR_1("dss_recvmsg(): "
                                  "GetSockAddrIN6 failed:%d", res);
               }

               // TODO: are the values be the same in both of the APIs, or should we make a conversion?

               err_info.ee.ee_errno = (uint32)Err.error_number;
               err_info.ee.ee_origin = (uint8)Err.origin;
               err_info.ee.ee_type = (uint8)Err.type;
               err_info.ee.ee_code = (uint8)Err.code;
               err_info.ee.ee_info = (uint32)Err.info;
               // We don't update the pad and data fields, which are currently unused
               //Convert the family back to host byte order for legacy API
               err_info.ps_sin_addr.ps_sin_family = AddrIN6.family;
               err_info.ps_sin_addr.ps_sin_port = AddrIN6.port;
               memscpy(&(err_info.ps_sin_addr.ps_sin_addr.ps_s_addr),
                 sizeof(uint32),
                 (const void*)&(AddrIN6.addr[3]), sizeof(uint32));

               // TODO: do we want to fetch the level as well (i.e. to add it to the IDL)
               tempRes =
                  ds::Utils::Conversion::PutCMsg(msg,
                                                 cmsg_ptr,
                                                 (int16)DSS_IPPROTO_IP,
                                                (int16)DSS_IP_RECVERR, sizeof(err_info),
                                                (void*)&err_info,&controllen);

               if (AEE_SUCCESS != tempRes){
                 LOG_MSG_ERROR_1("dss_recvmsg(): "
                                 "Failed to put cmsg with err %d", tempRes );
               }
            }
            break;
         }

         case ::ds::Sock::AncData::RECV_IF_INFO:
         {
            if (TRUE == isRecvIfSet){
               socketDSSNetApp = pDSSSocket->GetNetApp();
               // if the socket is connected to DSSNetApp then we get the iface_id
               // that is stored in this DSSNetApp and pass it to the application via
               // ancillary data.
               if (0 != socketDSSNetApp){
                  res = DSSGlobals::Instance()->GetNetApp(socketDSSNetApp, &pNetApp);
                  if (AEE_SUCCESS != res) {
                   LOG_MSG_ERROR_1("dss_recvmsg(): "
                                   "GetNetApp failed:%d", res);
                  }
                  pNetApp->GetIfaceId(&DSSNetAppIfaceID);
                  in_pktinfo.if_index = DSSNetAppIfaceID;
               }
               else // The socket is not connected to NetApp so we pass the iface_id that we got from ISocket
               {
                  res = ((ds::Sock::IRecvIFInfoPriv*)pOutAncData[i])->GetRecvIFHandle(&RecvIFHandle);
                  if (AEE_SUCCESS != res) {
                   LOG_MSG_ERROR_1("dss_recvmsg(): "
                                   "GetRecvIFHandle failed:%d", res);
                  }
                  in_pktinfo.if_index = RecvIFHandle;
               }
               // Write the ancillary data to the old msg structure
               tempRes =
                  ds::Utils::Conversion::PutCMsg(msg,
                                                 cmsg_ptr,
                                                 (int16)PS_IPPROTO_IP,
                                                 (int16)DSS_IP_RECVIF,
                                                 sizeof(in_pktinfo),
                                                 (void*)&in_pktinfo,
                                                 &controllen );

               if (AEE_SUCCESS != tempRes){
                 LOG_MSG_ERROR_1("dss_recvmsg(): "
                                 "Failed to put cmsg with err %d", tempRes );
               }
            }

            break;
         }

         default:
            LOG_MSG_ERROR_1("dss_recvmsg(): "
                            "Unsupported Ancillary Data received :%d", AncID);
      }

      DSSCommon::ReleaseIf((IQI**) &(pOutAncData[i]));

      cmsg_ptr = DSS_CMSG_NXTHDR(msg, cmsg_ptr);
   }

   if(controllen < msg->msg_controllen)
   {
      msg->msg_controllen = controllen;
   }

   sRet = nBufLenReq;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   DSSCommon::ReleaseIf((IQI**)&pIDSsocketPriv);
   PS_SYSTEM_HEAP_MEM_FREE (pOutAncData);
   PS_SYSTEM_HEAP_MEM_FREE (bufs);
   LOG_MSG_FUNCTION_EXIT_4( "dss_recvmsg(): "
                            "socket %d, ret 0x%x, error %d, res %d", 
                            sockfd, sRet, dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_WRITE()

DESCRIPTION
  Writes up to nbytes of data from buffer over the transport specified
  by the socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_write
(
  sint15 sockfd,                                      /* socket descriptor */
  const void *buffer,               /* user buffer from which to copy data */
  uint16 nbytes,                /* number of bytes to be written to socket */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   int nNumWritten = 0;
   AEEResult res;
   sint15 sRet = DSS_ERROR;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_write(): socket:%d, buffer:0x%p, caller 0x%p",
                            sockfd, buffer, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == buffer && 0 != nbytes) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
   }

   // the maximal size of data supported is DSS_MAX_SINT15
   // in case user provides larger buffer -
   // silently truncating it
   if (DSS_MAX_SINT15 < nbytes){
      LOG_MSG_INFO1_1("dss_write(): "
                      "Can't handle more than DSS_MAX_SINT15 bytes"
                      "user provided [%d], truncating", nbytes);
      nbytes = DSS_MAX_SINT15;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_write: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->Write((const byte*)buffer, (int)nbytes, &nNumWritten);
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_WRITE_OPERATION_IN_PROGRESS;
      } else {
      dss_error_code = DSS_ERROR_WRITE_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = (sint15)nNumWritten;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_3( "dss_write(): "
                            "socket %d, ret 0x%x, error %d", sockfd, sRet, dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_WRITEV()

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which
  allows the application to write from non-contiguous buffers.    Sends
  specified number of bytes in the buffer over the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written, which could be less than the number of
      bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specified
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           Cant write to a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_writev
(
  sint15 sockfd,                                      /* socket descriptor */
  struct ps_iovec iov[],  /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSPrivSokect = NULL;
   IPort1::SeqBytes *bufs = NULL;
   int nNumWritten = 0;
   int i;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_2("dss_writev(): socket:%d, caller 0x%p",
     sockfd, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == iov) {
      if (0 == iovcount) {
         sRet = 0;
      }
      else {
         dss_error_code = DSS_ERROR_NULL_IOV_PARAMETER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
      }
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_writev: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      ASSERT(0);
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   // validate array entities.
   for (i = 0; i < iovcount; i++) {
      if ((NULL == iov[i].ps_iov_base) && (0 != iov[i].ps_iov_len)) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // allocate space for bufs
   if (iovcount > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(bufs, sizeof(IPort1::SeqBytes)*iovcount, 
                               IPort1::SeqBytes*);
      if (NULL == bufs) {
         dss_error_code = DSS_ERROR_BUFFER_MEMORY_ALLOCATION_FAILED;
         *dss_errno = DS_EMSGSIZE;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // fill bufs
   for (i = 0; i < iovcount; i++) {
      bufs[i].data = iov[i].ps_iov_base;
      bufs[i].dataLen = (int)iov[i].ps_iov_len;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&(pIDSPrivSokect));
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSPrivSokect->WriteV(bufs, (int)iovcount, &nNumWritten);
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_WRITEV_OPERATION_IN_PROGRESS;
      } else {
      dss_error_code = DSS_ERROR_WRITEV_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }


   sRet = (sint15)nNumWritten;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSPrivSokect);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   PS_SYSTEM_HEAP_MEM_FREE (bufs);
   LOG_MSG_FUNCTION_EXIT_4( "dss_writev(): "
                            "socket %d, ret 0x%x, error %d, res %d", 
                            sockfd, sRet, dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_SENDTO()

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the transport specified by the
  socket descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specified
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendto
(
  sint15 sockfd,                /* socket descriptor                       */
  const void *buffer,           /* user buffer from which to copy the data */
  uint16 nbytes,                /* number of bytes to be written           */
  uint32 flags,                 /* used for SDB (if enabled)               */
  struct ps_sockaddr *toaddr,      /* destination address                  */
  uint16 addrlen,               /* address length                          */
  sint15 *dss_errno             /* error condition value                   */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   int nNumSent = 0;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_ERROR;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType tempFamily;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_3("dss_sendto(): "
                            "socket:%d, buffer:%d, caller 0x%p",
                            sockfd, buffer, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == buffer && 0 != nbytes) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
   }

   // the maximal size of data supported is DSS_MAX_SINT15
   // in case user provides larger buffer -
   // silently truncating it
   if (DSS_MAX_SINT15 < nbytes){
      LOG_MSG_INFO1_1("dss_sendto(): "
                      "Can't handle more than DSS_MAX_SINT15 bytes"
                      "user provided [%d], truncating", nbytes);
      nbytes = DSS_MAX_SINT15;
   }

   if (NULL == toaddr) {
      dss_error_code = DSS_ERROR_NULL_DESTINATION_ADDRESS;
      *dss_errno = DS_EADDRREQ;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_sendto(): pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET != toaddr->ps_sa_family && DSS_AF_INET6 != toaddr->ps_sa_family){
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET == toaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV4_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   if (DSS_AF_INET6 == toaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in6) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV6_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // Memscpy ps_sockaddr buffer into ps_sockaddr storage buffer
   addrlen = min(addrlen,sizeof(ds::SockAddrStorageType));
   memscpy(tempSockAddr, addrlen, toaddr, addrlen);

   res = DSSConversion::DS2IDSAddrFamily(toaddr->ps_sa_family, &tempFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      res = toaddr->ps_sa_family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, ps_htons(tempFamily));

   res = pIDSSocket->SendTo((const byte*)buffer, (int)nbytes, tempSockAddr, flags, &nNumSent);

   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
        dss_error_code = DSS_ERROR_SENDTO_OPERATION_IN_PROGRESS;
      } else {
      dss_error_code = DSS_ERROR_SENDTO_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = (sint15)nNumSent;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_sendto(): "
                            "socket %d, ret = %d, error %d, res", 
                            sockfd, sRet,dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specified
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSsocketPriv = NULL;
   IPort1::SeqBytes *bufs = NULL;
   ds::Sock::IAncDataPriv *pInAncData = NULL;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   int nNumWritten = 0;
   int i;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet;
   ds::AddrFamilyType tmpIDSFamily = ds::AddrFamily::QDS_AF_UNSPEC;
   int lengthToCopy = 0;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_4("dss_sendmsg(): "
                            "socket:%d, flags:%d, msg:0x%p, caller 0x%p",
                            sockfd, flags, msg, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if(NULL == msg) {
      dss_error_code = DSS_ERROR_NULL_MSG_PARAMETER;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if(0 >= msg->msg_namelen) {
     dss_error_code = DSS_ERROR_MSG_LEN_ZERO_OR_LESS;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_sendmsg(): pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == msg->msg_iov) {
      if (0 == msg->msg_iovlen) {
         sRet = 0;
      }
      else {
         dss_error_code = DSS_ERROR_NULL_MSG_IOV_PARAMETER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
      }
      goto bail;
   }

   if (0 < msg->msg_controllen) {
      dss_error_code = DSS_ERROR_ANCILLARY_DATA_NOT_SUPPORTED;
      *dss_errno = DS_EOPNOTSUPP;
      sRet = DSS_ERROR;
      goto bail;
   }

   // validate array entities.
   for (i = 0; i < msg->msg_iovlen; i++) {
      if ((NULL == msg->msg_iov[i].ps_iov_base) && (0 != msg->msg_iov[i].ps_iov_len)) {
         dss_error_code = DSS_ERROR_NULL_BUFFER;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // allocate space for bufs
   if (msg->msg_iovlen > 0) {
      PS_SYSTEM_HEAP_MEM_ALLOC(bufs, sizeof(IPort1::SeqBytes)*(msg->msg_iovlen), 
                               IPort1::SeqBytes*);
      if (NULL == bufs) {
         dss_error_code = DSS_ERROR_BUFFER_MEMORY_ALLOCATION_FAILED;
         *dss_errno = DS_ENOMEM;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   // fill bufs
   for (i = 0; i < msg->msg_iovlen; i++) {
      bufs[i].data = msg->msg_iov[i].ps_iov_base;
      bufs[i].dataLen = (int)(msg->msg_iov[i].ps_iov_len);
   }

   lengthToCopy = min(msg->msg_namelen, sizeof(ds::SockAddrStorageType));

   memscpy(tempSockAddr, lengthToCopy, msg->msg_name, lengthToCopy);
   // address family is retrieved in DS format
   (void)ds::Sock::AddrUtils::GetFamily(tempSockAddr, &family);
   // need to convert it to IDS format
   res = DSSConversion::DS2IDSAddrFamily(family, &tmpIDSFamily);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     res = family;
     sRet = DSS_ERROR;
     goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, ps_htons(tmpIDSFamily));

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&pIDSsocketPriv);
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     sRet = DSS_ERROR;
     goto bail;
   }
   res = pIDSsocketPriv->SendMsg(tempSockAddr,
                                 bufs, (int)msg->msg_iovlen, &nNumWritten,
                                &pInAncData, 0, 0);

   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = (sint15)nNumWritten;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   DSSCommon::ReleaseIf((IQI**)&pIDSsocketPriv);
   PS_SYSTEM_HEAP_MEM_FREE (bufs);
   LOG_MSG_FUNCTION_EXIT_4( "dss_sendmsg(): "
                            "socket %d, ret = %d, error %d, res %d",
                            sockfd, sRet, dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_SHUTDOWN

DESCRIPTION
  Shuts down the connection of the specified socket depending on the
  'how' parameter as follows:

  DSS_SHUT_RD:   Disallow subsequent calls to recv function
  DSS_SHUT_WR:   Disallow subsequent calls to send function
  DSS_SHUT_RDWR: Disallow subsequent calls to both recv and send functions

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  how       -  action to be performed: shutdown read-half, write-half or
               both
  dss_errno -  error number

RETURN VALUE
  In case of successful completion, returns DSS_SUCCESS. Otherwise, returns
  DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specified
  DS_ENOTCONN             the socket is not connected
  DS_EINVAL               invalid operation (e.g., how parameter is invalid)
  DS_ENOMEM               insufficient memory available to complete the
                          operation

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_3("dss_shutdown(): socket:%d, action:%u, errno 0x%x",
                            sockfd, how,dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_shutdown: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->Shutdown((ds::Sock::ShutdownDirType)how);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SHUTDOWN_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = DSS_SUCCESS;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_shutdown(): "
                            "socket %d, ret = %d, dss_errno = %d. error %d",
                            sockfd, sRet, *dss_errno,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_CLOSE()

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data
  structures and frees the socket for re-use.  For TCP initiates the active
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT
  will become TRUE, and the application can call dss_close() again to free
  the socket for re-use.  UDP sockets also need to call this to
  clean-up the socket and free it for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  Returns DSS_SUCCESS on success.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EWOULDBLOCK      operation would block - TCP close in progress
  DS_EBADF            invalid socket descriptor is specified

SIDE EFFECTS
  Initiates active close for TCP connections.
===========================================================================*/
sint15 dss_close
(
  sint15 sockfd,                                      /* socket descriptor */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_close(): socket:%d, errno 0x%x", sockfd,dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_close: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->Close();
   if (AEE_SUCCESS != res) {
      if (AEE_EWOULDBLOCK == res) {
         dss_error_code = DSS_ERROR_CLOSE_OPERATION_IN_PROGRESS;
      } else {
         dss_error_code = DSS_ERROR_CLOSE_OPERATION_FAILED;
      }
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   // De-Register all IP filter that were registered using this sockfd
   // We should not bail out here on error. App could have already freed
   // the filters using dss_dereg_ip_filter() API.
   // De-reg filters only after FIN is received for TCP sockets
   // (pIDSSock->Close() returns AEE_SUCCESS).
   (void) DSSGlobals::Instance()->RemoveFilterRegObjectFromList(sockfd);

   // Synchronized dss_close: If Stop returns AEE_SUCCESS, it means that the socket is already
   // closed and we don't need to wait for event
   (void) DSSGlobals::Instance()->RemoveSocket(sockfd);

   sRet = DSS_SUCCESS;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_close(): "
                            "socket %d, ret = %d, dss_errno = %d, error %d",
                            sockfd, sRet, *dss_errno,dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_CLOSE_NETLIB()

DESCRIPTION

  Closes the network library for the application.  All sockets must have
  been closed for the application, prior to closing.  If this is the last
  remaining application, the network subsystem (PPP/traffic channel) must
  have been brought down, prior to closing the network library.  If this
  is the last active application using the network library, this function
  takes the data services manager out of "socket" mode.

  This function is called from the context of the socket client's task.

DEPENDENCIES
  None.

RETURN VALUE
  On success, returns DSS_SUCCESS.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADAPP        invalid application ID
  DS_SOCKEXIST     there are existing sockets
  DS_ENETEXIST      the network subsystem exists

SIDE EFFECTS
  Puts data services manager into "autodetect" mode.
===========================================================================*/
sint15 dss_close_netlib
(
  sint15 dss_nethandle,                                         /* application ID */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSNetApp *pNetApp = NULL;
   int nNumOfSocks = 0;
   sint15 sRet = DSS_SUCCESS;
   AEEResult res = AEE_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_close_netlib(): net_handle:%d,errno 0x%x",
                            dss_nethandle,dss_errno );

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetNetApp(dss_nethandle, &pNetApp);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_APPLICATION_ID;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   pNetApp->GetNumOfSockets(&nNumOfSocks);
   if (0 != nNumOfSocks) {
      dss_error_code = DSS_ERROR_SOCKETS_STILL_PRESENT;
      *dss_errno = DS_SOCKEXIST;
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = dss_pppclose(dss_nethandle, dss_errno);
   if (DSS_SUCCESS != sRet) {
      LOG_MSG_ERROR_1("dss_close_netlib(): "
                      "dss_close_netlib failed due to pppclose error: %d",
                      sRet);
   }

   if (AEE_SUCCESS != DSSGlobals::Instance()->RemoveNetApp(dss_nethandle)) {
      LOG_MSG_ERROR_0("dss_close_netlib(): "
                      "Internal error: Failed to remove NetApp from apps list");
   }
   (void) pNetApp->Release();  // IDSNetworkPriv object of this DSSNetApp is deallocated here

   sRet = DSS_SUCCESS;

bail:
   LOG_MSG_FUNCTION_EXIT_3( "dss_close_netlib(): "
                            "ret = %d, dss_errno = %d, error %d",
                            sRet, *dss_errno, dss_error_code);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_SETSOCKOPT()

DESCRIPTION
  Set the options associated with a socket. This function expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specified
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_setsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   DSSNetActiveScope netActiveScope;
   ds::Sock::ISocketLocalPriv *pIDSPrivSokect = NULL;
   ds::Sock::ISocketExt *pIDSExtSokect = NULL;
   ds::Sock::OptLevelType IDSLevel;
   ds::Sock::OptNameType IDSOptName;
   ds::Sock::OptNameTypePriv IDSOptNamePriv;
   ds::Sock::IPMembershipInfoType IDSMembership;
   ds::Sock::LingerType IDSLinger;
   DSSIDSNetPolicyPrivScope IDSNetPolicyPrivScope;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_SUCCESS;
   dss_iface_id_type tmpIfaceId = INVALID_IFACE_ID;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;
   void *caller_ptr = DS_UTILS_CALLER_ADDRESS(0);

   LOG_MSG_FUNCTION_ENTRY_4("dss_setsockopt(): "
                            "socket:%d, level:%d, option_name:%d, caller 0x%p",
                            sockfd, level, optname, caller_ptr);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if ((NULL == optval) || (NULL == optlen)) {
      dss_error_code = DSS_ERROR_NULL_OPTVAL_OR_OPTLEN;
      *dss_errno = DS_EFAULT;
      sRet =(DSS_ERROR);
      goto bail;
   }

   // Even though this socket option is deprecated, operation should succeed, no value shall be stored,
   // to support the legacy apps. Some legacy apps expect success on this operation,
   // they don't care that this operation makes no change to any values in the system
   if ((int)DSS_SO_QOS_SHARE_HANDLE == optname){
      dss_error_code = DSS_ERROR_DSS_SO_QOS_SHARE_HANDLE_SOCKOPT_DEPRECATED;
      sRet = DSS_SUCCESS;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_setsockopt: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   /* Silently return success for DISABLE_FLOW_FWDING */
   if ((int)DSS_SO_DISABLE_FLOW_FWDING == optname) {
      pDSSSocket->SetFlowForwarding(*((int*)optval));
      *dss_errno = 0;
      sRet = DSS_SUCCESS;
      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketExt, (void**)&(pIDSExtSokect));
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_IP_ADD_MEMBERSHIP == optname || (int)DSS_IP_DROP_MEMBERSHIP == optname) {
#ifdef FEATUTE_DATA_PS_MCAST
      dss_ip_mreqn* mcastArg = (dss_ip_mreqn*)optval;
      struct ps_in_addr mcast_addr = mcastArg->mcast_grp;
      IDSMembership.mcastGroup.family = ds::AddrFamily::QDS_AF_INET;
      (void)memsmove(IDSMembership.mcastGroup.addr, 4, &mcast_addr.ps_s_addr, 4);
      tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(mcastArg->iface_id );
#endif // FEATUTE_DATA_PS_MCAST
      IDSMembership.ifaceId = static_cast<ds::Net::IfaceIdType>(tmpIfaceId); // lower layers are expecting iface id with FFFF's
      if (PS_IFACE_INVALID_ID == IDSMembership.ifaceId) {
         IDSMembership.ifaceId = INVALID_IFACE_ID;
      }
      if ((int)DSS_IP_ADD_MEMBERSHIP == optname) {
         res = pIDSExtSokect->AddIPMembership(&IDSMembership);
      }
      else {
         res = pIDSExtSokect->DropIPMembership(&IDSMembership);
      }

      if (AEE_SUCCESS != res) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }

      goto bail;
      }

   if ((int)DSS_IPV6_ADD_MEMBERSHIP == optname || (int)DSS_IPV6_DROP_MEMBERSHIP == optname) {
#ifdef FEATUTE_DATA_PS_MCAST
      dss_ipv6_mreqn* mcastArg = (dss_ipv6_mreqn*)optval;
      struct ps_in6_addr mcast_addr = mcastArg->mcast_grp_v6;
      IDSMembership.mcastGroup.family = ds::AddrFamily::QDS_AF_INET6;
      (void)memsmove(IDSMembership.mcastGroup.addr, 16, mcast_addr.ps_s6_addr, 16);
      tmpIfaceId = DSSIfaceId::DSSIfaceId2DSNetIfaceId(mcastArg->iface_id);
#endif // FEATUTE_DATA_PS_MCAST
      IDSMembership.ifaceId = static_cast<ds::Net::IfaceIdType>(tmpIfaceId); // lower layers are expecting iface id with FFFF's
      if ((int)DSS_IPV6_ADD_MEMBERSHIP == optname) {
         res = pIDSExtSokect->AddIPMembership(&IDSMembership);
      } else {
         res = pIDSExtSokect->DropIPMembership(&IDSMembership);
      }
      if (AEE_SUCCESS != res) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   if ((int)DSS_BCMCS_JOIN == optname || (int)DSS_BCMCS_LEAVE == optname) {

      DSSNetApp *pNetApp = NULL;
      DSSIDSNetActiveScope IDSNetActiveScope;
#ifdef FEATUTE_DATA_PS_MCAST
      struct ps_in_addr *mcast_addr = (struct ps_in_addr*)optval;
      IDSMembership.mcastGroup.family = ds::AddrFamily::QDS_AF_INET;
      (void)memsmove(IDSMembership.mcastGroup.addr, 4, &(mcast_addr->ps_s_addr), 4);
#endif // FEATUTE_DATA_PS_MCAST
      int kind;
      pDSSSocket->GetSockKind(&kind);
      if (2 == kind) {
        //No app for sock2
        IDSMembership.ifaceId = INVALID_IFACE_ID;
      }
      else {
        // Get the iface id from the IDSNetwork object from the DSSNetApp.
        sint15 appid = pDSSSocket->GetNetApp();

        res = DSSGlobals::Instance()->GetNetApp(appid, &pNetApp);
        if (AEE_SUCCESS != res) {
           *dss_errno = DSSConversion::IDS2DSErrorCode(res);
           sRet = DSS_ERROR;
           goto bail;
        }

        res = netActiveScope.Init(pNetApp);
        if (AEE_SUCCESS != res) {
           LOG_MSG_ERROR_2("dss_setsockopt(): "
                           "Failed to fetch NetActive from NetApp object. res=%d pNetApp=0x%p",
                           res, pNetApp);
           *dss_errno = DS_ENETDOWN;
           sRet = DSS_ERROR;
           goto bail;
        }

        res = IDSNetActiveScope.Init(netActiveScope.Fetch());
        if (AEE_SUCCESS != res) {
           *dss_errno = DSSConversion::IDS2DSErrorCode(res);
           sRet = DSS_ERROR;
           goto bail;
        }

        // Network is up, since we are using NetActive

        ds::Net::IfaceIdType iface_id;
        res = IDSNetActiveScope.Fetch()->GetIfaceId(&iface_id);
        if (AEE_SUCCESS != res) {
           *dss_errno = DSSConversion::IDS2DSErrorCode(res);
           sRet = DSS_ERROR;
           goto bail;
        }

        IDSMembership.ifaceId = iface_id;
      }//Sock kind !=2

      if ((int)DSS_BCMCS_JOIN == optname) {
         res = pIDSExtSokect->AddIPMembership(&IDSMembership);
      } else {
         res = pIDSExtSokect->DropIPMembership(&IDSMembership);
      }
      if (AEE_SUCCESS != res) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   if ((int)DSS_SO_LINGER == optname) {
      res = DSSConversion::DS2IDSLinger((dss_so_linger_type*)optval, &IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_DS2IDSLINGER_OPERATION_FAILED;
      }

      res = pIDSSocket->SetSOLingerReset(&IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_SOLINGER_OPERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   if ((int)DSS_SO_LINGER_RESET == optname) {
      res = DSSConversion::DS2IDSLinger((dss_so_linger_type*)optval, &IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_DS2IDSLINGER_OPERATION_FAILED;
      }

      res = pIDSSocket->SetSOLingerReset(&IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_SOLINGERRESET_OPERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      } else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   if ((int)DSS_SO_SILENT_CLOSE == optname) {

      IDSLinger.lingerEnabled = TRUE;
      IDSLinger.timeInSec = 0;

      res = pIDSSocket->SetSOLingerReset(&IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_SOLINGERRESET_OPERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      } else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   if ((int)DSS_SO_SDB_ACK_CB == optname) {
      res = pDSSSocket->SetSdbAckCb((dss_so_sdb_ack_cb_type*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_SDBACKSIGNAL_OPERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      } else {
         sRet = DSS_SUCCESS;
      }

      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&(pIDSPrivSokect));
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_SO_NETPOLICY == optname) {
      if (-1 != pDSSSocket->GetNetApp()) {
         // This option is supported only for sockets created using dss_socket2()
         dss_error_code = DSS_ERROR_DSS_SO_CB_FCN_SUPPORTED_ONLY_FOR_DSS_SOCKET2_SOCKETS;
         *dss_errno = DS_EOPNOTSUPP;
         sRet = DSS_ERROR;
      }
      else {
         res = IDSNetPolicyPrivScope.Init();
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_NET_POLICY_CREATION_FAILED;
            *dss_errno = DSSConversion::IDS2DSErrorCode(res);
            sRet = DSS_ERROR;
            goto bail;
         }

         res = DSSConversion::DS2IDSNetPolicy((dss_net_policy_info_type*)optval, IDSNetPolicyPrivScope.Fetch());
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_NET_POLICY_CONVERSION_FAILED;
            *dss_errno = DSSConversion::IDS2DSErrorCode(res);
            sRet = DSS_ERROR;
            goto bail;
         }

         // TODO: AddRef?
         res = pIDSExtSokect->SetNetPolicy(IDSNetPolicyPrivScope.Fetch());
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_SET_POLICY_FAILED;
            *dss_errno = DSSConversion::IDS2DSErrorCode(res);
            sRet = DSS_ERROR;
            goto bail;
         }
         res = pDSSSocket->SetLegacyPolicy((dss_net_policy_info_type*)optval);
         if (AEE_SUCCESS != res) {
            dss_error_code = DSS_ERROR_SET_LEGACY_POLICY_OPERATION_FAILED;
            *dss_errno = DSSConversion::IDS2DSErrorCode(res);
            sRet = DSS_ERROR;
            goto bail;
         }
      }
      goto bail;
   }

   if ((int)DSS_SO_CB_FCN == optname) {
      if (-1 != pDSSSocket->GetNetApp()) {
         // This option is supported only for sockets created using dss_socket2()
         dss_error_code = DSS_ERROR_DSS_SO_CB_FCN_SUPPORTED_ONLY_FOR_DSS_SOCKET2_SOCKETS;
         *dss_errno = DS_EOPNOTSUPP;
         sRet = DSS_ERROR;
      }
      else {
         pDSSSocket->SetCb((dss_sock_cb_fcn_type*)optval);
         sRet = DSS_SUCCESS;
         goto bail;
      }
   }

   if (sizeof(int) > *optlen) {
      dss_error_code = DSS_ERROR_INVALID_OPTLEN;
      *dss_errno = DS_EINVAL;
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_SO_SYS_SOCK == optname) {
      ds::Sock::ISocketLocalPriv *pIDSsocketPriv = NULL;
      if (DSS_SOL_SOCKET != (dss_sockopt_levels_type)level) {
         dss_error_code = DSS_ERROR_INCORRECT_OPTION_LEVEL;
         *dss_errno = DS_EINVAL;
         sRet = DSS_ERROR;
         goto bail;
      }
      res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&pIDSsocketPriv);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
      res = pIDSsocketPriv->SetSystemOption(TRUE);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_SYSTEM_OPTION_FAILED;
      }
      DSSCommon::ReleaseIf((IQI**)&pIDSsocketPriv);
      sRet = DSS_SUCCESS;
      goto bail;
   }

   res = DSSConversion::DS2IDSOptLevel((dss_sockopt_levels_type)level, &IDSLevel);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_OPTION_LEVEL_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   // Check if the option should be translated to a OptNamePriv
   res = DSSConversion::DS2IDSOptNamePriv((dss_sockopt_names_type)optname, &IDSOptNamePriv);
   if (AEE_SUCCESS == res) {
      // temporarily convert optlen from uint32* to int*
      res = pIDSPrivSokect->SetOptPriv(IDSLevel, IDSOptNamePriv, *(int*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SETOPTPRIV_OPERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   res = DSSConversion::DS2IDSOptName((dss_sockopt_names_type)optname, &IDSOptName);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_OPTION_NAME_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   // temporarily convert optlen from uint32* to int*
   res = pIDSSocket->SetOpt(IDSLevel, IDSOptName, *(int*)optval);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SET_OPT_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
   }
   else {
      sRet = DSS_SUCCESS;
   }

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSPrivSokect);
   DSSCommon::ReleaseIf((IQI**)&pIDSExtSokect);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_setsockopt(): "
                            "socket %d, ret = %d, error %d, res %d",
                            sockfd, sRet, dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_GETSOCKOPT()

DESCRIPTION
  Return an option associated with a socket. This function expects the
  following parameters:

DEPENDENCIES
  None.

PARAMETERS
  int sockfd        -     Socket file descriptor.
  int level         -     Socket option level.
  int optname,      -     Option name.
  void *optval      -     Pointer to the option value.
  uint32 *optlen    -     Pointer to the size of the option value.
  sint15 *errno     -     Error condition value.

RETURN VALUE
  optlen is a value-result parameter, initially containing the size of
  the buffer pointed to by optval, and modified on return to indicate the
  actual  size  of the value returned. On error, return DSS_ERROR and places
  the error condition value in *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF                invalid socket descriptor is specified
  DS_ENOPROTOOPT          the option is unknown at the level indicated
  DS_EINVAL               invalid option name or invalid option value
  DS_EFAULT               Invalid buffer or argument

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockopt
(
  int sockfd,                            /* socket descriptor              */
  int level,                             /* socket option level            */
  int optname,                           /* option name                    */
  void *optval,                          /* value of the option            */
  uint32 *optlen,                        /* size of the option value       */
  sint15 *dss_errno                      /* error condition value          */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::Sock::ISocketLocalPriv *pIDSPrivSokect = NULL;
   ds::Sock::OptLevelType IDSLevel;
   ds::Sock::OptNameType IDSOptName;
   ds::Sock::OptNameTypePriv IDSOptNamePriv;
   ds::Sock::LingerType IDSLinger;
   DSSIDSNetPolicyScope IDS2DSNetPolicyScope;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet = DSS_SUCCESS;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_4("dss_getsockopt(): "
                            "socket:%d, level:%d, option_name:%d, errno 0x%x",
                            sockfd, level, optname, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if ((NULL == optval) || (NULL == optlen)){
      dss_error_code = DSS_ERROR_NULL_OPTVAL_OR_OPTLEN;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   // Even though this socket option is deprecated, operation should succeed and arbitrary value shall be returned
   // to support the legacy apps. Some legacy apps expect success on this operation even though the optval is meaningless for them
   if ((int)DSS_SO_QOS_SHARE_HANDLE == optname){
      dss_error_code = DSS_ERROR_DSS_SO_QOS_SHARE_HANDLE_SOCKOPT_DEPRECATED;
      *((unsigned int *) optval) = 17;
      sRet = DSS_SUCCESS;
      goto bail;
   }

   // DSS_BCMCS_JOIN and DSS_BCMCS_LEAVE are not supported for dss_getsockopt
   if ( ((int)DSS_BCMCS_JOIN == optname) || ((int)DSS_BCMCS_LEAVE == optname) ) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_OPTIONS;
      *dss_errno = DS_ENOPROTOOPT;   // DS_ENOPROTOOPT error code preserves backward compatibility
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_getsockopt(): dss_setsockopt: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   /* Silently return success for DISABLE_FLOW_FWDING */
   if ((int)DSS_SO_DISABLE_FLOW_FWDING == optname) {
      *dss_errno = 0;
      pDSSSocket->GetFlowForwarding((int*)optval);
      sRet = DSS_SUCCESS;
      goto bail;
   }

   if ((int)DSS_ICMP_ECHO_ID == optname || (int)DSS_ICMP_ECHO_SEQ_NUM == optname) {
      // These options are supported for set only
      dss_error_code = DSS_ERROR_OPTION_NOT_SUPPORTED_FOR_DSS_GETSOCKOPT;
      *dss_errno = DS_EINVAL;
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_SO_LINGER == optname) {
      res = pIDSSocket->GetSOLingerReset(&IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_GET_SOLINGER_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
      res = DSSConversion::IDS2DSLinger(&IDSLinger, (dss_so_linger_type*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_IDS2DSLINGER_FAILED;
      }

      *optlen = sizeof(dss_so_linger_type);
      sRet = DSS_SUCCESS;
      goto bail;
   }

   if ((int)DSS_SO_LINGER_RESET == optname ||
       (int)DSS_SO_SILENT_CLOSE == optname) {
      res = pIDSSocket->GetSOLingerReset(&IDSLinger);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_GET_SOLINGERRESET_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
      res = DSSConversion::IDS2DSLinger(&IDSLinger, (dss_so_linger_type*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_IDS2DSLINGER_FAILED;
      }

      *optlen = sizeof(dss_so_linger_type);
      sRet = DSS_SUCCESS;
      goto bail;
   }

   if ((int)DSS_SO_SDB_ACK_CB == optname) {
      sRet = pDSSSocket->GetSdbAckCb((dss_so_sdb_ack_cb_type*)optval);
      *optlen = sizeof(dss_so_sdb_ack_cb_type);
      goto bail;
   }

   if ((int)DSS_SO_NETPOLICY == optname) {
      res = IDS2DSNetPolicyScope.Init(pIDSSocket);
         if (AEE_SUCCESS != res) {
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
         }

      // Convert IDSPOlicy to legacy policy representation. 3rd parameter -1 means app_id is not applicable
      // TODO: check if there is use case that requires us here to actually try and fetch the app_id via the DSSNetApp
      //       to which the DSSSocket is bound (if any), and set it in the iface_id in the policy accordingly.
      res = DSSConversion::IDS2DSNetPolicy(IDS2DSNetPolicyScope.Fetch(), (dss_net_policy_info_type*)optval, -1);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_NET_POLICY_CONVERSION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
      res = pDSSSocket->GetLegacyPolicy((dss_net_policy_info_type*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_SET_LEGACY_POLICY_OPERATION_FAILED;
         sRet = DSS_ERROR;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         goto bail;
      }
      *optlen = sizeof(dss_net_policy_info_type);
      goto bail;
   }

   if (sizeof(int) > *optlen) {
      dss_error_code = DSS_ERROR_INVALID_OPTLEN;
      *dss_errno = DS_EINVAL;
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_SO_SYS_SOCK == optname) {
      ds::Sock::ISocketLocalPriv *pIDSsocketPriv = NULL;
      boolean bSystemOption;
      if (DSS_SOL_SOCKET != (dss_sockopt_levels_type)level) {
         dss_error_code = DSS_ERROR_INCORRECT_OPTION_LEVEL;
         *dss_errno = DS_EINVAL;
         sRet = DSS_ERROR;
         goto bail;
      }
      res = DSSConversion::IDS2DSErrorCode(pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&pIDSsocketPriv));
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
         goto bail;
      }
      res = pIDSsocketPriv->GetSystemOption(&bSystemOption);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_GET_SYSTEM_OPTION_FAILED;
      }

      *((int*)optval) = bSystemOption ? 1 : 0;
      *optlen = sizeof(int);
      DSSCommon::ReleaseIf((IQI**)&pIDSsocketPriv);
      sRet = DSS_SUCCESS;
      goto bail;
   }

   res = DSSConversion::DS2IDSOptLevel((dss_sockopt_levels_type)level, &IDSLevel);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_OPTION_LEVEL_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::Sock::AEEIID_ISocketLocalPriv, (void**)&(pIDSPrivSokect));
   if (AEE_SUCCESS != res) {
     dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
     *dss_errno = DSSConversion::IDS2DSErrorCode(res);
     sRet = DSS_ERROR;
     goto bail;
   }

   // Check if the option should be translated to a OptNamePriv
   res = DSSConversion::DS2IDSOptNamePriv((dss_sockopt_names_type)optname, &IDSOptNamePriv);
   if (AEE_SUCCESS == res) {
      res = pIDSPrivSokect->GetOptPriv(IDSLevel, IDSOptNamePriv, (int*)optval);
      if (AEE_SUCCESS != res) {
         dss_error_code = DSS_ERROR_GETOPTPRIV_OPRERATION_FAILED;
         *dss_errno = DSSConversion::IDS2DSErrorCode(res);
         sRet = DSS_ERROR;
      }
      else {
         sRet = DSS_SUCCESS;
      }
      goto bail;
   }

   res = DSSConversion::DS2IDSOptName((dss_sockopt_names_type)optname, &IDSOptName);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_OPTION_NAME_CONVERSION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   // temporarily convert optlen from uint32* to int*
   res = pIDSSocket->GetOpt(IDSLevel, IDSOptName, (int*)optval);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_GET_OPT_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   if ((int)DSS_SO_ERROR == optname)
   {
      *((int *) optval) = DSSConversion::IDS2DSErrorCode(*((int *) optval));
   }
//   *optlen = sizeof(int);

   sRet = DSS_SUCCESS;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSPrivSokect);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_5( "dss_getsockopt():socket = %d, "
                            " ret = %d, dss_errno = %d, error %d, res %d",
                            sockfd, sRet, *dss_errno,dss_error_code,res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_GETSOCKNAME

DESCRIPTION
  Returns the current local address assigned to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  local address currently associated with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the socket
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specified
  DS_EFAULT               addr parameter points to an invalid memory
                          location

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getsockname
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res = AEE_SUCCESS;
   sint15 sRet;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   int tmpDSFamily;
   size_t copylen;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_getsockname(): socket:%d, errno 0x%x", sockfd, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if (NULL == addr){
      dss_error_code = DSS_ERROR_NULL_ADDRESS;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == addrlen) {
      dss_error_code = DSS_ERROR_NULL_ADDRESS_LENGTH;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_getsockname: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   memset(tempSockAddr, 0, sizeof(tempSockAddr));

   res = pIDSSocket->GetSockName(tempSockAddr);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_GETSOCKNAME_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::GetFamily(tempSockAddr,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      res = family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, tmpDSFamily);

   if (ds::AddrFamily::QDS_AF_INET == family) {
      if (sizeof(struct ps_sockaddr_in) < *addrlen)
      {
         *addrlen = sizeof(struct ps_sockaddr_in);
      }
   }
   else if (ds::AddrFamily::QDS_AF_INET6 == family) {
      if (sizeof(struct ps_sockaddr_in6) < *addrlen)
      {
         *addrlen = sizeof(struct ps_sockaddr_in6);
      }
   }
   else {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   copylen = min(*addrlen,sizeof(ds::SockAddrStorageType));

   memscpy(addr, copylen, tempSockAddr, copylen);

   sRet = DSS_SUCCESS;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_5( "dss_getsockname(): "
                            "socket = %d, ret = %d, dss_errno = %d, error %d, res %d",
                            sockfd, sRet, *dss_errno,dss_error_code, res);
   return sRet;
}

/*===========================================================================
FUNCTION DSS_GETPEERNAME

DESCRIPTION
  Returns the address of the peer connected to the specified socket.

DEPENDENCIES
  None.

PARAMETERS
  sockfd    -  socket file descriptor
  addr      -  address of the peer connected with the socket
  addrlen   -  address length. This parameter is initialized to indicate
               the amount of space pointed by addr and on return, it
               contains the actual size of the address returned.
  dss_errno -  error number

RETURN VALUE
  Returns DSS_SUCCESS upon successful completion and places the peer
  address and the address length in addr and addrlen parameters, resp.

  If the address is larger than the supplied buffer then it is silently
  truncated. The value returned in addrlen indicates the size prior to
  truncation, if any.

  On error, returns DSS_ERROR and places the error number in dss_errno.

  Errno Values
  ------------
  DS_EBADF                invalid socket descriptor is specified
  DS_EFAULT               addr parameter points to an invalid memory
                          location
  DS_ENOTCONN             the socket is not connected

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_getpeername
(
  sint15              sockfd,               /* socket descriptor           */
  struct ps_sockaddr* addr,                 /* address of the socket       */
  uint16*             addrlen,              /* address length              */
  sint15*             dss_errno             /* error number                */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   AEEResult res = DSS_SUCCESS;
   sint15 sRet = DSS_SUCCESS;
   ds::SockAddrStorageType tempSockAddr;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   int tmpDSFamily;
   size_t copylen;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_1("dss_getpeername(): socket:%d", sockfd);

   if (NULL == dss_errno) {
      dss_error_code = DSS_ERROR_NULL_ERRNO;
      sRet = DSS_ERROR;
      goto bail;
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if (NULL == addr) {
      dss_error_code = DSS_ERROR_NULL_ADDRESS;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (NULL == addrlen) {
      dss_error_code = DSS_ERROR_NULL_ADDRESS_LENGTH;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL== pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_getpeername: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   memset(tempSockAddr, 0, sizeof(tempSockAddr));

   res = pIDSSocket->GetPeerName(tempSockAddr);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_GET_PEERNAME_OPERATION_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::GetFamily(tempSockAddr,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      res = family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(tempSockAddr, tmpDSFamily);

   if (ds::AddrFamily::QDS_AF_INET == family) {
      if (sizeof(struct ps_sockaddr_in) < *addrlen)
      {
         *addrlen = sizeof(struct ps_sockaddr_in);
      }
   }
   else if (ds::AddrFamily::QDS_AF_INET6 == family) {
      if (sizeof(struct ps_sockaddr_in6) < *addrlen)
      {
         *addrlen = sizeof(struct ps_sockaddr_in6);
      }
   }
   else {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DS_EAFNOSUPPORT;
      sRet = DSS_ERROR;
      goto bail;
   }

   copylen = min(*addrlen,sizeof(ds::SockAddrStorageType));

   memscpy(addr, copylen, tempSockAddr, copylen);

   LOG_MSG_FUNCTION_EXIT_3( "dss_getpeername(): "
                            "socket = %d, ret = %d, dss_errno = %d",
                            sockfd, sRet, *dss_errno);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   return sRet;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_ERROR_3("dss_getpeername():failed"
                   "socket = %d, error = %d, res = %d",
                   sockfd,dss_error_code, res);
   return sRet;
}


//===================================================================
//                 Helper / Internal Functions
//===================================================================

/*===========================================================================
FUNCTION DSSOCKI_NET_CB()
DESCRIPTION
   This function is a common network call back function for application
   using old opennetlib function.

DEPENDENCIES
   NOTE: This function must be called in a TASKLOCK().

RETURN VALUE
   None

SIDE EFFECTS
   Calls the user specified network call back which is passed in user_data_ptr.
===========================================================================*/
void dssocki_net_cb
(
   sint15            dss_nethandle,
   dss_iface_id_type iface_id,
   sint15            dss_errno,
   void            * user_data_ptr
)
{
   /*lint -save -e611 we need to cast one function type to another, for that we first cast net/socket_callback_fcn to "void*" and then cast it back to another
   function type, using helper functions dssocki_net/sock_cb */
   if(user_data_ptr != NULL)
   {
      ((void (*)(void *))user_data_ptr)((void *)&dss_nethandle);
   }
   /*lint -restore Restore lint error 611*/
} /* dssocki_net_cb() */

/*===========================================================================
FUNCTION DSSOCKI_SOCK_CB()
DESCRIPTION
   This function is a common socket call back function for application
   using old opennetlib function.

DEPENDENCIES
   NOTE: This function must be called in a TASKLOCK().

RETURN VALUE
   None

SIDE EFFECTS
   Calls the user specified socket call back which is passed in user_data_ptr.
===========================================================================*/
void dssocki_sock_cb
(
   sint15 dss_nethandle,
   sint15 sockfd,
   uint32 event_mask,
   void * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   /*lint -save -e611 we need to cast one function type to another, for that we first cast net/socket_callback_fcn to "void*" and then cast it back to another
   function type, using helper functions dssocki_net/sock_cb */
   if(user_data_ptr != NULL) {
      ((void (*)(void *))user_data_ptr)((void *)&dss_nethandle);
   }
   /*lint -restore Restore lint error 611*/
} /* dssocki_sock_cb() */


/*===========================================================================
FUNCTION DSS_WRITE_DSM_CHAIN()

DESCRIPTION
Sends the specified DSM item chain over the TCP transport.  Lower layers
will be responsible for deallocating the DSM item chain.

DEPENDENCIES
None.

RETURN VALUE
n - the number of bytes written.

On error, return DSS_ERROR and places the error condition value in
*dss_errno.

dss_errno Values
----------------
DS_EBADF            invalid socket descriptor is specfied
DS_ENOTCONN         socket not connected
DS_ECONNRESET       TCP connection reset by server
DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
DS_EPIPE            broken pipe
DS_EADDRREQ         destination address required
DS_ENETDOWN         network subsystem unavailable
DS_EFAULT           bad memory address
DS_EWOULDBLOCK      operation would block
DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
The pointer to the DSM item chain is set to NULL.
===========================================================================*/
sint15 dss_write_dsm_chain
(
 sint15 sockfd,                                      /* socket descriptor */
 dsm_item_type **item_ptr,          /* DSM item chain containing the data */
 sint15 *dss_errno                               /* error condition value */
 )
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::IDSMUtils *pIDSMUtils = NULL;
   sint15 sRet;
   AEEResult res;
   int32 nBytesWritten = 0;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_write_dsm_chain(): socket:%d, errno 0x%x", sockfd, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_write_dsm_chain: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::AEEIID_IDSMUtils, (void**)&pIDSMUtils);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSMUtils->WriteDSMChain(item_ptr, &nBytesWritten);  // TODO: implemented as part of ISocket (also in adapter)
   DSSCommon::ReleaseIf((IQI**)&pIDSMUtils);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_WRITE_DSM_CHAIN_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = (sint15)nBytesWritten;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_write_dsm_chain(): "
                            "socket = %d, ret = %d, dss_errno = %d, error %d",
                            sockfd, sRet, *dss_errno,dss_error_code);
   return sRet;

} /* dss_write_dsm_chain() */

/*===========================================================================
FUNCTION DSS_READ_DSM_CHAIN()

DESCRIPTION
Reads a DSM item chain from the TCP transport.

DEPENDENCIES
None.

RETURN VALUE
n - the number of bytes read.  A return of 0 indicates that an End-of-File
condition has occurred.

On error, return DSS_ERROR and places the error condition value in
*dss_errno.

dss_errno Values
----------------
DS_EBADF            invalid socket descriptor is specfied
DS_ENOTCONN         socket not connected
DS_ECONNRESET       TCP connection reset by server
DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
DS_EPIPE            broken pipe
DS_EADDRREQ         destination address required
DS_ENETDOWN         network subsystem unavailable
DS_EFAULT           bad memory address
DS_EWOULDBLOCK      operation would block
DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
None.
===========================================================================*/
sint15 dss_read_dsm_chain
(
 sint15 sockfd,                                      /* socket descriptor */
 dsm_item_type  **item_ptr,          /* ptr to item chain containing data */
 sint15 *dss_errno                               /* error condition value */
 )
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::IDSMUtils *pIDSMUtils = NULL;
   sint15 sRet;
   AEEResult res;
   int32 nBytesRead = 0;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_2("dss_read_dsm_chain(): socket:%d, errno 0x%x", sockfd, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_read_dsm_chain: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface(ds::AEEIID_IDSMUtils, (void**)&pIDSMUtils);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSMUtils->ReadDSMChain(item_ptr, &nBytesRead);   // TODO: implemented as part of ISocket (also in adapter)
   DSSCommon::ReleaseIf((IQI**)&pIDSMUtils);
   if (AEE_SUCCESS != res && QDS_EEOF != res) {
      dss_error_code = DSS_ERROR_READ_DSM_CHAIN_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   *dss_errno = DS_EEOF;
   sRet = (sint15)nBytesRead;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_4( "dss_read_dsm_chain(): "
                            "socket = %d, ret = %d, dss_errno = %d, error %d",
                            sockfd, sRet, *dss_errno, dss_error_code);
   return sRet;
} /* dss_read_dsm_chain() */

/*===========================================================================
FUNCTION DSS_SENDTO_DSM_CHAIN()

DESCRIPTION
  Sends the specified DSM item chain over the TCP transport.  Lower layers
  will be responsible for deallocating the DSM item chain.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes written.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EOPNOTSUPP       invalid server address specified

SIDE EFFECTS
  The pointer to the DSM item chain is set to NULL.
===========================================================================*/
sint15 dss_sendto_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type **item_ptr_ptr,      /* DSM item chain containing the data */
  uint32 flags,                               /* used for SDB (if enabled) */
  struct ps_sockaddr *toaddr,                       /* destination address */
  uint16 addrlen,                                        /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{
   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::IDSMUtils *pIDSMUtils = NULL;
   ds::SockAddrStorageType remoteSockAddr;
   sint15 sRet;
   AEEResult res = AEE_SUCCESS;
   int32 nBytesSent = 0;
   ds::AddrFamilyType tempFamily;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_4("dss_sendto_dsm_chain(): "
                            "socket:%d, flags:%u, addrlen:%d, errno 0x%x",
                            sockfd, flags, addrlen, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
	  return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if (NULL == toaddr ||
       NULL == item_ptr_ptr)
   {
      dss_error_code = DSS_ERROR_INVALID_ARGUMENTS;
      sRet = DSS_ERROR;
      *dss_errno = DS_EFAULT;
      goto bail;
   }

   if (DSS_AF_INET != toaddr->ps_sa_family && DSS_AF_INET6 != toaddr->ps_sa_family){
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      res = toaddr->ps_sa_family;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   if (DSS_AF_INET == toaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV4_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   if (DSS_AF_INET6 == toaddr->ps_sa_family){
      if (sizeof(struct ps_sockaddr_in6) != addrlen) {
         dss_error_code = DSS_ERROR_ILLEGAL_IPV6_ADDRESS_LENGTH;
         *dss_errno = DS_EFAULT;
         sRet = DSS_ERROR;
         goto bail;
      }
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      LOG_MSG_ERROR_1 ("dss_sendto_dsm_chain(): "
                       "dss_sendto_dsm_chain: pIDSSocket is NULL: 0x%x",
                       pIDSSocket);
      ASSERT(0);
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface (ds::AEEIID_IDSMUtils, (void**)&pIDSMUtils);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   memset (remoteSockAddr, 0, sizeof(remoteSockAddr));
   addrlen = min(addrlen,sizeof(ds::SockAddrStorageType));
   memscpy (remoteSockAddr, addrlen, toaddr, addrlen);

   res = DSSConversion::DS2IDSAddrFamily(toaddr->ps_sa_family, &tempFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      res = toaddr->ps_sa_family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(remoteSockAddr, ps_htons(tempFamily));

   res = pIDSMUtils->SendToDSMChain(item_ptr_ptr, remoteSockAddr, flags, &nBytesSent);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_SENDTO_DSM_CHAIN_FAILED;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      sRet = DSS_ERROR;
      goto bail;
   }

   sRet = (sint15)nBytesSent;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSMUtils);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_5( "dss_sendto_dsm_chain(): socket:%d, "
                            "ret 0x%x, dss_errno = x%x, error %d, res %d",
                            sockfd, sRet, *dss_errno, dss_error_code, res);
   return sRet;
} /* dss_sendto_dsm_chain() */

/*===========================================================================
FUNCTION DSS_RECVFROM_DSM_CHAIN()

DESCRIPTION
  Reads a DSM item chain from the TCP transport.

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes read.  A return of 0 indicates that an End-of-File
      condition has occurred.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_ENOTCONN         socket not connected
  DS_ECONNRESET       TCP connection reset by server
  DS_ECONNABORTED     TCP connection aborted due to timeout or other failure
  DS_EIPADDRCHANGED   IP address changed, causing TCP connection reset
  DS_EPIPE            broken pipe
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EWOULDBLOCK      operation would block
  DS_EINVAL           can't read from a listen socket

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dss_recvfrom_dsm_chain
(
  sint15 sockfd,                                      /* socket descriptor */
  dsm_item_type  **item_ptr_ptr,      /* ptr to item chain containing data */
  uint32 flags,                                                  /* unused */
  struct ps_sockaddr *fromaddr,                     /* destination address */
  uint16 *addrlen,                                       /* address length */
  sint15 *dss_errno                               /* error condition value */
)
{

   DSSSocket *pDSSSocket = NULL;
   ds::Sock::ISocket *pIDSSocket = NULL;
   ds::IDSMUtils *pIDSMUtils = NULL;
   ds::SockAddrStorageType remoteSockAddr;
   ds::AddrFamilyType family = ds::AddrFamily::QDS_AF_UNSPEC;
   int32 nBytesRecvd = 0;
   sint15 sRet;
   AEEResult res = AEE_SUCCESS;
   int tmpDSFamily;
   dss_error_enum_type dss_error_code = DSS_ERROR_MIN;

   LOG_MSG_FUNCTION_ENTRY_3("dss_recvfrom_dsm_chain(): socket: %d, flags: %u, errno 0x%x",
                            sockfd, flags, dss_errno);

   if (NULL == dss_errno) {
      sRet = DSS_ERROR;
      return sRet; // to avoid deref of dss_errno in bail
   }

   DSS_DS_API_CALLED_RET_ERRNO();

   if (((NULL != fromaddr) && (NULL == addrlen)) ||
       (NULL == item_ptr_ptr)) {
      dss_error_code = DSS_ERROR_INVALID_ARGUMENTS;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = DSSGlobals::Instance()->GetSocketById(sockfd, &pDSSSocket);
   if (NULL == pDSSSocket || AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_SOCKET_DESCRIPTOR;
      *dss_errno = DS_EBADF;
      sRet = DSS_ERROR;
      goto bail;
   }

   pDSSSocket->GetIDSSocket(&pIDSSocket);
   if (NULL == pIDSSocket) {
      DATA_ERR_FATAL("dss_recvfrom_dsm_chain: pIDSSocket is NULL");
      dss_error_code = DSS_ERROR_NULL_SOCKET_ID;
      *dss_errno = DS_EFAULT;
      sRet = DSS_ERROR;
      goto bail;
   }

   res = pIDSSocket->QueryInterface (ds::AEEIID_IDSMUtils, (void**)&pIDSMUtils);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      dss_error_code = DSS_ERROR_QUERY_INTERFACE_FAILED;
      sRet = DSS_ERROR;
      goto bail;
   }

   memset (remoteSockAddr, 0, sizeof(remoteSockAddr));

   res = pIDSMUtils->RecvFromDSMChain(item_ptr_ptr, remoteSockAddr, flags, &nBytesRecvd);
   if (AEE_SUCCESS != res) {
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      if (*dss_errno != DS_EWOULDBLOCK) {
         dss_error_code = DSS_ERROR_DSS_RECVFROM_DSM_CHAIN_FAILED;
      }
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::GetFamily(remoteSockAddr,&family);
   family = ps_ntohs(family);

   res = DSSConversion::IDS2DSAddrFamily(family, &tmpDSFamily);
   if (AEE_SUCCESS != res) {
      dss_error_code = DSS_ERROR_INVALID_ADDRESS_FAMILY;
      *dss_errno = DSSConversion::IDS2DSErrorCode(res);
      res = family;
      sRet = DSS_ERROR;
      goto bail;
   }

   (void)ds::Sock::AddrUtils::SetFamily(remoteSockAddr, tmpDSFamily);

   if (NULL != addrlen) {
      switch (family)
      {
         case ds::AddrFamily::QDS_AF_INET:
            if (sizeof(struct ps_sockaddr_in) < *addrlen)
            {
               *addrlen = sizeof(struct ps_sockaddr_in);
            }
            break;
         case ds::AddrFamily::QDS_AF_INET6:
            if (sizeof(struct ps_sockaddr_in6) < *addrlen)
            {
               *addrlen = sizeof(struct ps_sockaddr_in6);
            }
            break;

         default:
            LOG_MSG_ERROR_1("dss_recvfrom_dsm_chain(): "
                            "dss_recvfrom: The address family must be INET or INET6 %i",
                            family);
            ASSERT(0);
      }
   }

   if (NULL != addrlen){
      size_t copylen = min(sizeof(ds::SockAddrStorageType), *addrlen);

      if (NULL != fromaddr){
         memscpy(fromaddr, copylen, remoteSockAddr, copylen);
      }
      else{
         *addrlen = 0;
      }
   }

   sRet = (sint15)nBytesRecvd;

bail:
   DSSCommon::ReleaseIf((IQI**)&pIDSMUtils);
   DSSCommon::ReleaseIf((IQI**)&pIDSSocket);
   LOG_MSG_FUNCTION_EXIT_5( "dss_recvfrom_dsm_chain(): socket:%d,"
                            " ret 0x%x, dss_errno = x%x, error %d, res %d",
                            sockfd, sRet, *dss_errno,dss_error_code, res);
   return sRet;

} /* dss_recvfrom_dsm_chain() */

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_ADDR()

DESCRIPTION
  This function return the interface id matching an IP address

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  iface_id: If a valid iface could be obtained based on address
  On error, return DSS_IFACE_INVALID_ID and places the error condition value
  in *dss_errno.

  dss_errno Values
  ----------------
  DS_EFAULT      Invalid arguments passed to the function.
  DS_ENOROUTE    No interface can be determined for the address.
  DS_EOPNOTSUPP  This operation is not supported.

SIDE EFFECTS
  None.
===========================================================================*/
dss_iface_id_type dss_get_iface_id_by_addr
(
  ip_addr_type * ip_addr_ptr,                     /* IP address info       */
  sint15       * dss_errno                        /* error condition value */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2_0("dss_get_iface_id_by_addr() called ");

  // check whether dss_errno is NULL
  if( dss_errno == NULL )
  {
    LOG_MSG_ERROR_0("dss_get_iface_id_by_addr(): "
                    "Invalid params");
    return(DSS_IFACE_INVALID_ID);
  }

  DSS_DS_API_CALLED_RET_ERRNO_PARAM(DSS_IFACE_INVALID_ID);

  *dss_errno = DS_EOPNOTSUPP;
  return (DSS_IFACE_INVALID_ID);

} /* dss_get_iface_id_by_addr() */

/*===========================================================================
FUNCTION DSS_GET_QOS_SHARE_HANDLE()

DESCRIPTION
  This function returns a qos share handle associated with  the set of QOS
  instances requested using the nethandle. Applications can only use this
  handle to set the QOS_SHARE_HANDLE socket option for sockets created using
  socket2. Setting the share handle will enable sockets to use the QOS
  instances created by the nethandle.

DEPENDENCIES
  None.

PARAMETERS
  net_handle: network handle
  dss_errno -  error number.

RETURN VALUE

 dss_errno Values
  ---------------
  DS_EBADAPP               invalid Network handle specified

SIDE EFFECTS
  None.
  // Even though this socket option is deprecated, operation should succeed and arbitrary value shall be returned
  // to support the legacy apps. Some legacy apps expect success on this operation even though the value is meaningless for them
===========================================================================*/
int32 dss_get_qos_share_handle
(
   sint15  net_handle,
   sint15  *dss_errno
)
{
   LOG_MSG_ERROR_0("dss_get_qos_share_handle is deprecated");
   return 17;
} /* dss_get_qos_share_handle() */

} /* extern "C" */

