/*===========================================================================
               PS _ SOCK _ REMOTESOCKET _ Manager . C

DESCRIPTION

  PS SOCK REMOTESOCKET MANGER - Implementation.

FUNCTIONS

Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_RemoteSocket_Manager.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    bvd     Initial version 
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/

#include <stringl/stringl.h>
#include "ds_Utils_DebugMsg.h"
#include "ps_system_heap.h"
#include "ds_Utils_CCritSect.h"
#include "ds_Utils_CreateInstance.h"
#include "PS_Sock_RemoteSocket_Manager.h"
#include "ps_sys_event.h"
#include "ps_crit_sect.h"
#include "ps_sys_conf.h"
#include "ps_sys.h"
#include "ps_iface_defs.h"
#include "ps_iface.h"
#include "ps_ifacei_event.h"
#include "ps_handle_mgr.h"
#include "PS_Sock_Platform_SocketFactory.h"
#include "ps_mem.h"
#include "ds_Addr_Def.h"
#include "ds_Sock_Def.h"

using namespace PS::Sock::RemoteSocket;
using namespace ds::Error;
using namespace ds::Utils;

ps_crit_sect_type  ps_sock_remote_sock_mgr_crit_section;

Manager * Manager::instance = NULL;

/*===========================================================================

                         PUBLIC CLASS FUNCTIONS

===========================================================================*/
void Manager::Init(void)
{
  int16                     ps_errno;
  int32                     retVal;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == instance) 
  {
    /*--------------------------------------------------------------------
     Create instance of Remote Socket Manager if it hasnt been created
    ---------------------------------------------------------------------*/  
    instance = new Manager();
    if (0 == instance) {
      DATA_ERR_FATAL("Manager::CreateInstance(): No mem for Manager");
      return;
    }                  
             
    /*--------------------------------------------------------------------
      Register for AP/netmgr available event
    ---------------------------------------------------------------------*/               
    ps_sys_event_reg_ex(PS_SYS_TECH_ALL, 
                     PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE,
                     PS_SYS_PRIMARY_SUBS,
                     Manager::ps_sys_event_remote_socket_hdlr_change_cb, 
                     instance, 
                     &ps_errno);         
                     
    /*--------------------------------------------------------------------
      Register for ioctl callback when AP indicates, it has reserved
      requested sockets
    ---------------------------------------------------------------------*/   
    ps_sys_ioctl_register_ioctl_cback_ex(PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED,
                                         Manager::ps_sys_ioctl_socket_allocated,
                                         PS_SYS_PRIMARY_SUBS,
                                         &ps_errno);
                                         
   /*------------------------------------------------------------------------
     Subscribe to iface up events on iface
   ------------------------------------------------------------------------*/
    instance->psIfaceEvHandlePtr = 
      ps_iface_alloc_event_cback_buf( Manager::PSIfaceEventCback, instance);

    if (NULL == instance->psIfaceEvHandlePtr)
    {
      ASSERT(0);
    }
    
    retVal =
      ps_iface_event_cback_reg( NULL,
                                IFACE_UP_EV,
                                instance->psIfaceEvHandlePtr);                                           

    if (0 != retVal)
    {
      DATA_ERR_FATAL("Couldn't register for iface up event");
    }
    /*--------------------------------------------------------------------
      Initialize Remote Socket Manager State Machine to Init state
    ---------------------------------------------------------------------*/     
    instance->state = INIT;   
 
    ps_init_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  }
} /* Manager::Init */

void Manager::DeInit(void)
{
  int16 ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 != instance) 
  {
    ps_sys_event_dereg_ex(PS_SYS_TECH_ALL, 
                       PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE,
                       PS_SYS_PRIMARY_SUBS,
                       Manager::ps_sys_event_remote_socket_hdlr_change_cb, 
                       &ps_errno);                       
    
    /*---------------------------------------------------------------------
      Unsubscribe from iface up event
    ----------------------------------------------------------------------*/
    ps_iface_event_cback_dereg( NULL,
                                IFACE_UP_EV,
                                instance->psIfaceEvHandlePtr);
                                
    ps_iface_free_event_cback_buf(instance->psIfaceEvHandlePtr);
    
    ps_destroy_crit_section(&ps_sock_remote_sock_mgr_crit_section);
    
    delete instance;
  }
} /* Manager::DeInit */

void Manager::PSIfaceEventCback
(
  ps_iface_type *             psIfacePtr,
  ps_iface_event_enum_type    psIfaceEvent,
  ps_iface_event_info_u_type  psIfaceEventInfo,
  void *                      userDataPtr
)
{
  Manager  * instance;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_FUNCTION_ENTRY_2("Manager::PSIfaceEventCback(): "
                           "Ev %d Iface 0x%x", psIfaceEvent, psIfacePtr);
                           
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  instance = (Manager  *)userDataPtr; 

  /*--------------------------------------------------------------------
    Request for Max socket for cache only if Remote Socket Manager is
    in Init State
  ---------------------------------------------------------------------*/  
  if (INIT == instance->state)
  {  
    instance->RequestMaxSockets(); 
  }

  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  LOG_MSG_FUNCTION_EXIT_0("Manager::PSIfaceEventCback(): Success");
  return;

} /* Manager::PSIfaceEventCback() */

void Manager::ps_sys_event_remote_socket_hdlr_change_cb
(
  ps_sys_tech_enum_type              tech_type,
  ps_sys_event_enum_type             event_name,
  ps_sys_subscription_enum_type      subscription_id,
  void                             * event_info_ptr,
  void                             * user_data_ptr
)
{
  Manager                                      * instance;
  ps_sys_remote_socket_hdlr_avail_type         * hdlr_avail_ptr;
  int                                            numSockets = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/    
  if (PS_SYS_EVENT_REMOTE_SOCKET_HDLR_CHANGE != event_name)
  {
    LOG_MSG_ERROR_0("Manager::ps_sys_event_remote_socket_hdlr_change_cb : "
                    "Invalid event recieved");
    return;
  }
  
  instance = (Manager  *)user_data_ptr;
  hdlr_avail_ptr = 
    (ps_sys_remote_socket_hdlr_avail_type *)event_info_ptr;  
  
  LOG_MSG_INFO1_3("Manager::ps_sys_event_remote_socket_hdlr_change_cb : "
                  "instance 0x%x remote binding allowed %d curr state %d",
                  user_data_ptr, 
                  (NULL != hdlr_avail_ptr)? 
                  hdlr_avail_ptr->remote_socket_hdlr_avail:0,
                  (NULL != instance)?instance->state:0);     
 
  if (NULL == hdlr_avail_ptr || NULL == user_data_ptr) 
  {
    return;
  }
     
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section); 
  
  if (hdlr_avail_ptr->remote_socket_hdlr_avail == TRUE)
  {
    /*-----------------------------------------------------------------------
      Go through all PS Sock sockets and find all the sockets whose 
      remote socket handle is NULL and whose port is between 32000 to 36999
      and send indicaton to AP/netmgr to reserve those sockets
    -----------------------------------------------------------------------*/      
    numSockets = instance->RequestAllocatedSocket();    
    if (instance->state == DISABLED)
    {
      instance->state = INIT;
      
      /*---------------------------------------------------------------------
        If AP/netmgr came up late after first bind request was sent out
        then request for first 20 sockets for cache
      ---------------------------------------------------------------------*/       
      if (numSockets > 0)
      {
        instance->RequestMaxSockets();
      }      
    }
  }
  else
  {
    instance->state = DISABLED;
  }
 
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);   
} /* Manager::ps_sys_event_remote_socket_hdlr_change_cb */

int16 Manager::ps_sys_ioctl_socket_allocated
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  ps_sys_subscription_enum_type   subscription_id,
  int16                         * ps_errno
)
{
  Manager                                  * instance;
  ps_sys_remote_socket_allocated_type      * sockAllocatedPtr;
  ps_sys_remote_socket_allocated_info_type * sockInfoPtr;
  int                                        cnt_sock;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/    
  if (PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED != ioctl_name)
  {
    LOG_MSG_ERROR_0("Manager::ps_sys_ioctl_socket_allocated :"
                    "Invalid ioctl recieved");
    return -1;
  }
  
  instance = Manager::GetInstance();
  
  sockAllocatedPtr = 
    (ps_sys_remote_socket_allocated_type *)arg_val_ptr;
  
  if (NULL == sockAllocatedPtr)
  {
    return -1;
  }
  
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  /*-----------------------------------------------------------------------
    1. Go through all sockets reserved by AP/netmgr
    2. If socket was requested with port 0 (is_ephemeral_port was set to
       TRUE) then add the socket to cache
    3. If socket was requested with specific port(is_ephemeral_port was 
       set to FALSE) then find the socket in list of all PS Sock Sockets
       and add remote socket handle to that socket
  -----------------------------------------------------------------------*/     
  for (cnt_sock = 0; cnt_sock < sockAllocatedPtr->num_sockets; cnt_sock++)
  {
    sockInfoPtr = &(sockAllocatedPtr->socket_list[cnt_sock]);
    if (NULL != sockInfoPtr)
    {
      if (TRUE == sockInfoPtr->is_ephemeral_port)
      {
        /*-----------------------------------------------------------------
          Add Remote socket info to cache
        ------------------------------------------------------------------*/  
        instance->AddSockInfoToCache(sockInfoPtr);       
      }
      else
      {
        /*-----------------------------------------------------------------
          Add handle to existing socket
        ------------------------------------------------------------------*/  
        instance->AddHandleToSocket(sockInfoPtr);        
      }
    }
  }  
  
  /*-----------------------------------------------------------------
    Change State to Remote Socket Recieved state
  ------------------------------------------------------------------*/  
  instance->state = REMOTE_SOCKET_RECIEVED;
 
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);     
  
  return 0;
} /* Manager::ps_sys_ioctl_socket_allocated */


/*===========================================================================

                         PRIVATE MEMEBER FUNCTIONS

===========================================================================*/

boolean Manager::SocketRequestAllowed
(
  uint32          num_sockets,
  State           state
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  /*-----------------------------------------------------------------------
    If Remote socket manager is in disabled state then socket request
    is not allowed
  -----------------------------------------------------------------------*/        
  if(DISABLED == state)
  {
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
    In order to request max sockets for cache, Remote socket manager has
    to be in Init state.
  -----------------------------------------------------------------------*/      
  if (num_sockets == maxAllowedSocketInCache)
  {
    if (INIT != state)
    {
      return FALSE;
    }
  }  
  else
  {
    if (REMOTE_SOCKET_REQUESTED != state && REMOTE_SOCKET_RECIEVED != state)
    {
      return FALSE;
    }    
  }
  
  return TRUE;
} /* Manager::SocketRequestAllowed */

Socket * Manager::GetPlatformSocketPtr
(
  uint32   ps_sock_platform_handle
)
{
  Socket *  platformSockPtr = 0;
  int32     index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  if (PS_HANDLE_MGR_INVALID_HANDLE == ps_sock_platform_handle)
  {
    LOG_MSG_ERROR_1("Manager::GetPlatformSocketPtr(): "
                    "Invalid handle %d", ps_sock_platform_handle);
    return NULL;
  }  
  
  /*-------------------------------------------------------------------------
    Generate a handle for platform socket. This handle will be the platform
    socket descriptor
  -------------------------------------------------------------------------*/
  index = 
    ps_handle_mgr_get_index( PS_HANDLE_MGR_CLIENT_SOCKET_PLATFORM,
                              ps_sock_platform_handle);
  
  platformSockPtr = 
    (Socket *)ps_mem_index_to_buf(index, PS_MEM_PLATFORM_SOCKET_TYPE);
  if (NULL == platformSockPtr)
  {
    LOG_MSG_ERROR_2("Manager::GetPlatformSocketPtr(): "
                    "Invalid index %d sock ptr 0x%p", 
                    index, platformSockPtr);
  }
  
  return platformSockPtr;
} /* Manager::GetPlatformSocketPtr */

ds::Utils::List * Manager::GetSockCache
(
  ds::Sock::ProtocolType protocol,
  ds::AddrFamilyType     family
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds::Sock::Protocol::QDS_UDP == protocol)
  {
    if (ds::AddrFamily::QDS_AF_INET == family)
    {
      return &sockIPv4UDPCache;
    }
    if (ds::AddrFamily::QDS_AF_INET6 == family)
    {
      return &sockIPv6UDPCache;
    }    
  }
  
  if (ds::Sock::Protocol::QDS_TCP == protocol)
  {
    if (ds::AddrFamily::QDS_AF_INET == family)
    {
      return &sockIPv4TCPCache;
    }
    if (ds::AddrFamily::QDS_AF_INET6 == family)
    {
      return &sockIPv6TCPCache;
    }    
  }  
  return NULL;
} /* Manager::GetSockCache */

ip_version_enum_type Manager::ConvertToPSFamily
(
  ds::AddrFamilyType family
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  if (family == ds::AddrFamily::QDS_AF_INET)
  {
    return IP_V4;
  }
  else
  {
    return IP_V6;
  } 
}/* Manager::ConvertToPSFamily */

ps_ip_protocol_enum_type Manager::ConvertToPSProtocol
(
  ds::Sock::ProtocolType protocol
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  if (protocol == ds::Sock::Protocol::QDS_UDP)
  {
    return PS_IPPROTO_UDP;
  }
  else if (protocol == ds::Sock::Protocol::QDS_TCP)
  {
    return PS_IPPROTO_TCP;
  } 
  else
  {
    return PS_IPPROTO_TCP_UDP;
  }  
}/* Manager::ConvertToPSProtocol */

ds::AddrFamilyType Manager::ConvertToDSSockFamily
(
  ip_version_enum_type family
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  if (family == IP_V4)
  {
    return ds::AddrFamily::QDS_AF_INET;
  }
  else if (family == IP_V6)
  {
    return ds::AddrFamily::QDS_AF_INET6;
  } 
  else
  {
    return ds::AddrFamily::QDS_AF_UNSPEC;
  }  
}/* Manager::ConvertToDSSockFamily */

ds::Sock::ProtocolType Manager::ConvertToDSSockProtocol
(
  ps_ip_protocol_enum_type protocol
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  if (protocol == PS_IPPROTO_UDP)
  {
    return ds::Sock::Protocol::QDS_UDP;
  }
  else if (protocol == PS_IPPROTO_TCP)
  {
    return ds::Sock::Protocol::QDS_TCP;
  } 
  else
  {
    return ds::Sock::Protocol::QDS_UNSPEC;
  }  
}/* Manager::ConvertToDSSockProtocol */

void Manager::AddSockInfoToCache
(
  ps_sys_remote_socket_allocated_info_type * remoteSockInfoPtr
)
{
  SockInfo        * sockInfoPtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  LOG_MSG_INFO1_5("Manager::AddSockInfoToCache status %d  port_no %d handle %d"
                  "xport_proto %d ip_family %d",
                  remoteSockInfoPtr->status, 
                  remoteSockInfoPtr->socket_info.port_no, 
                  remoteSockInfoPtr->remote_socket_handle, 
                  remoteSockInfoPtr->socket_info.xport_proto, 
                  remoteSockInfoPtr->socket_info.ip_family);   

  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  /*-----------------------------------------------------------------------
    Create new object for storing Remote socket handle information and
    store in appropriate cache
  -----------------------------------------------------------------------*/    
  if (PS_SYS_REMOTE_SOCKET_ALLOC_SUCCESS == remoteSockInfoPtr->status)
  {
    sockInfoPtr =  
      new SockInfo
      (
        remoteSockInfoPtr->socket_info.port_no,
        remoteSockInfoPtr->remote_socket_handle,
        ConvertToDSSockFamily
          (remoteSockInfoPtr->socket_info.ip_family),
        ConvertToDSSockProtocol
          (remoteSockInfoPtr->socket_info.xport_proto)
      );         
  
    if (IP_V4 == remoteSockInfoPtr->socket_info.ip_family)
    {
       if (PS_IPPROTO_UDP == 
           remoteSockInfoPtr->socket_info.xport_proto)
       {
         sockIPv4UDPCache.PushBack(sockInfoPtr); 
       }
       else if (PS_IPPROTO_TCP == 
                remoteSockInfoPtr->socket_info.xport_proto)
       {         
         sockIPv4TCPCache.PushBack(sockInfoPtr); 
       }
    }
    else if (IP_V6 == remoteSockInfoPtr->socket_info.ip_family)
    {
       if (PS_IPPROTO_UDP == 
            remoteSockInfoPtr->socket_info.xport_proto)
       {         
         sockIPv6UDPCache.PushBack(sockInfoPtr); 
       }
       else if (PS_IPPROTO_TCP == 
                remoteSockInfoPtr->socket_info.xport_proto)
       {         
         sockIPv6TCPCache.PushBack(sockInfoPtr);
       }
    }
  }
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
} /* Manager::AddSockInfoToCache */

int Manager::RequestAllocatedSocket
(
  void
)
{
  SocketFactory                            * platformSockFactoryPtr;
  Socket                                   * platformSockPtr;
  ps_sys_remote_socket_request_type          sockRequestInfo;
  int16                                      ps_errno;
  int                                        numSockets = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/      
  LOG_MSG_INFO1_0("Manager::RequestAllocatedSocket ");   
  
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);   
     
  memset(&sockRequestInfo, 0, 
         sizeof(ps_sys_remote_socket_request_type));
         
  platformSockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);

  /*-------------------------------------------------------------------------
    1. Iterate through the list of platform sockets
    2. Find the platform socket information with remote socke handle as NULL
       and port between 32000 to 36999
    3. Send request to AP to block all those specific ports
  -------------------------------------------------------------------------*/  
  /*---------------------------------------------------------------------
    Step 1
  ---------------------------------------------------------------------*/  
  for (platformSockFactoryPtr->First(); 
       !platformSockFactoryPtr->IsDone(); 
       platformSockFactoryPtr->Next())
  {
    platformSockPtr = 
      static_cast <Socket *> ( platformSockFactoryPtr->CurrentItem());
    if (0 == platformSockPtr)
    {
      LOG_MSG_ERROR_0("Manager::RequestAllocatedSocket: "
                      "NULL sock");
      continue;
    }

    if (TRUE == platformSockPtr->GetStrongRef())
    {
      /*-----------------------------------------------------------------
        Step 2
      -----------------------------------------------------------------*/     
      if (ps_ntohs(platformSockPtr->GetPort()) > EPHEMERAL_PORT_BASE && 
          ps_ntohs(platformSockPtr->GetPort()) <= 
          EPHEMERAL_PORT_BASE + EPHEMERAL_PORT_MAX_OFFSET)
      {
        sockRequestInfo.socket_list[numSockets].port_no =  
           platformSockPtr->GetPort();
        sockRequestInfo.socket_list[numSockets].ip_family = 
          ConvertToPSFamily(platformSockPtr->GetFamily());
        sockRequestInfo.socket_list[numSockets].xport_proto =  
          ConvertToPSProtocol(platformSockPtr->GetProtocol());
        numSockets++;
        if (PS_SYS_MAX_REMOTE_SOCKETS <= numSockets)
        {
          break;
        }
      }
      
      (void) platformSockPtr->Release();
    }
  }  
  
  sockRequestInfo.num_sockets = numSockets;
  
  DS_UTILS_RELEASEIF(platformSockFactoryPtr);
  
  /*-----------------------------------------------------------------
    Step 3
  -----------------------------------------------------------------*/      
  if (numSockets > 0)
  {
    ps_sys_event_ind_ex(PS_SYS_TECH_ALL, 
                     PS_SYS_EVENT_REMOTE_SOCKET_REQUEST,
                     PS_SYS_PRIMARY_SUBS,
                     &sockRequestInfo,
                     &ps_errno);
  }                   
 
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);  

  return numSockets;
} /* Manager::RequestAllocatedSocket */

void Manager::AddHandleToSocket
(
  ps_sys_remote_socket_allocated_info_type * remoteSockInfoPtr
)
{
  SocketFactory                            * platformSockFactoryPtr;
  Socket                                   * platformSockPtr;
  boolean                                    socketConsumed = FALSE;
  int16                                      retVal;
  uint16                                     port_val;
  int32                                      udpEpncapsVal;
  int32                                      udpEpncapsLen;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  platformSockFactoryPtr = SocketFactory::CreateInstance();
  ASSERT( 0 != platformSockFactoryPtr);  
       
  /*-------------------------------------------------------------------------
    1. Iterate through the list of platform sockets
    2. Match platform socket information with remote socket information
    3. If they match then update platform socket's remote socket handle
       to handle sent by AP. If platform socket has UDP encaps option 
       set then send indication to AP to set UDP encaps option for that
       remote socket handle
    4. If remote socket information doesnt match any PS Sock Socket then
       free the remote socket
  -------------------------------------------------------------------------*/
  
  /*---------------------------------------------------------------------
    Step 1
  ---------------------------------------------------------------------*/   
  for (platformSockFactoryPtr->First(); 
       !platformSockFactoryPtr->IsDone(); 
       platformSockFactoryPtr->Next())
  {
    platformSockPtr = 
      static_cast <Socket *> ( platformSockFactoryPtr->CurrentItem());
    if (0 == platformSockPtr)
    {
      LOG_MSG_ERROR_0("Manager::AddHandleToSocket: "
                      "NULL sock");
      continue;
    }
        
    if (TRUE == platformSockPtr->GetStrongRef())
    {
      /*-----------------------------------------------------------------
        Step 2
      -----------------------------------------------------------------*/        
      port_val = platformSockPtr->GetPort();
      if ((port_val ==
          remoteSockInfoPtr->socket_info.port_no) && 
          (platformSockPtr->GetFamily() ==
          ConvertToDSSockFamily
          (remoteSockInfoPtr->socket_info.ip_family)) &&
          (platformSockPtr->GetProtocol() ==
          ConvertToDSSockProtocol
          (remoteSockInfoPtr->socket_info.xport_proto))  &&
          0 ==  platformSockPtr->GetRemoteSockHandle() &&
          TRUE == platformSockPtr->IsSocketAlive())
      {
        
        platformSockPtr->SetRemoteSocktHandle
        (
          remoteSockInfoPtr->remote_socket_handle
        );       
        socketConsumed = TRUE;
        
        if (AEE_SUCCESS == 
            platformSockPtr->GetOpt(ds::Sock::OptLevel::QDS_LEVEL_UDP, 
                                    ds::Sock::OptName::QDS_UDP_ENCAPS, 
                                    &udpEpncapsVal, 
                                    &udpEpncapsLen))                                    
        {
          if (0 != udpEpncapsVal)
          {
            SendUDPEncapsSockOpt(remoteSockInfoPtr->remote_socket_handle, TRUE);
          }            
        }                         
      }
      
      (void) platformSockPtr->Release();
      
      if (TRUE == socketConsumed)
      {
        break;
      }
    }
  } 

  /*-----------------------------------------------------------------
    Step 4
  -----------------------------------------------------------------*/    
  if (FALSE == socketConsumed) 
  {
    retVal = ReleaseSocket
             (
               remoteSockInfoPtr->remote_socket_handle
             );
    if (retVal != 0)
    {
      LOG_MSG_ERROR_1("Manager::AddHandleToSocket : "
                      "Release socket failed err %d", retVal);       
    }
  } 

  DS_UTILS_RELEASEIF(platformSockFactoryPtr); 
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
} /* Manager::AddHandleToSocket */

/*===========================================================================

                         PUBLIC MEMEBER FUNCTIONS

===========================================================================*/
Manager * Manager::GetInstance(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (0 == instance) 
  {
    ASSERT(0);
  }

  return instance;
} /* Manager::Instance() */


uint8 Manager::GetRemoteSockCacheCnt
(
  ds::Sock::ProtocolType  protocol, 
  ds::AddrFamilyType      family
)
{
  ds::Utils::List *  sockCachePtr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  sockCachePtr = GetSockCache(protocol,family); 
  if (NULL != sockCachePtr)
  {
    return sockCachePtr->Count();
  }
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  return 0;
} /* Manager::GetRemoteSockCacheCnt */

SockInfo * Manager::GetRemoteSockCacheEntry
(
  uint8                   index,
  ds::Sock::ProtocolType  protocol, 
  ds::AddrFamilyType      family
)
{
  ds::Utils::List *  sockCachePtr = NULL;
  SockInfo        *  sockInfoPtr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  sockCachePtr = GetSockCache(protocol,family); 
  
  if (NULL != sockCachePtr && sockCachePtr->Count() > 0)
  {
    sockInfoPtr = (SockInfo *)sockCachePtr->Get(index);
  }
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  return sockInfoPtr;
} /* Manager::GetRemoteSockCacheEntry */

State Manager::GetState
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return state;
} /* Manager::GetState */

int16 Manager::RequestMaxSockets
(
  void
)
{
  ps_sys_remote_socket_request_type  sockReqInfo;
  int                                cntSock;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  LOG_MSG_INFO1_0("Manager::RequestMaxSockets"); 

  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  /*-------------------------------------------------------------------------
    Request five sockets each for type IPV4UDPSocket, IPV4TCPSocket, 
    IPV6UDPSocket IPV6TCPSocket cache
  -------------------------------------------------------------------------*/  
  memset(&sockReqInfo, 0, sizeof(ps_sys_remote_socket_request_type));
  sockReqInfo.num_sockets = maxAllowedSocketInCache;
  for (cntSock = 0; cntSock < 5; cntSock++)
  {
    sockReqInfo.socket_list[cntSock].port_no = 0;
    sockReqInfo.socket_list[cntSock].xport_proto = PS_IPPROTO_UDP;
    sockReqInfo.socket_list[cntSock].ip_family = IP_V4;
  }
  
  for (cntSock = 5; cntSock < 10; cntSock++)
  {
    sockReqInfo.socket_list[cntSock].port_no = 0;
    sockReqInfo.socket_list[cntSock].xport_proto = PS_IPPROTO_UDP;
    sockReqInfo.socket_list[cntSock].ip_family = IP_V6;
  }

  for (cntSock = 10; cntSock < 15; cntSock++)
  {
    sockReqInfo.socket_list[cntSock].port_no = 0;
    sockReqInfo.socket_list[cntSock].xport_proto = PS_IPPROTO_TCP;
    sockReqInfo.socket_list[cntSock].ip_family = IP_V4;
  }  
  
  for (cntSock = 15; cntSock < 20; cntSock++)
  {
    sockReqInfo.socket_list[cntSock].port_no = 0;
    sockReqInfo.socket_list[cntSock].xport_proto = PS_IPPROTO_TCP;
    sockReqInfo.socket_list[cntSock].ip_family = IP_V6;
  }  
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  return RequestSocket(&sockReqInfo);
} /* Manager::RequestMaxSockets */

int16 Manager::RequestSocket
(
  ps_sys_remote_socket_request_type * sockReqInfoPtr
)
{
  ps_sys_remote_socket_hdlr_avail_type         sock_hdlr_avail;
  int16                                        ps_errno;
  int16                                        ret_val = -1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  LOG_MSG_INFO1_4("Manager::RequestSocket num_sockets %d port_no %d "
                  "xport_proto %d ip_family %d",
                  sockReqInfoPtr->num_sockets, 
                  sockReqInfoPtr->socket_list[0].port_no, 
                  sockReqInfoPtr->socket_list[0].xport_proto, 
                  sockReqInfoPtr->socket_list[0].ip_family);  

  /*-------------------------------------------------------------------------
    1. Check if socket request is permissible
    2. Check if AP is available, if not change the state to DISABLED
    3. Send request to AP to reserve sockets and change state to 
       REMOTE_SOCK_REQUESTED
  -------------------------------------------------------------------------*/  
  do
  {
    if (SocketRequestAllowed(sockReqInfoPtr->num_sockets, state))
    {      
      ps_sys_conf_get_ex(PS_SYS_TECH_ALL, 
                      PS_SYS_CONF_REMOTE_SOCKET_HDLR_AVAIL,
                      PS_SYS_PRIMARY_SUBS, 
                      &sock_hdlr_avail,
                      &ps_errno);  
      if (FALSE == sock_hdlr_avail.remote_socket_hdlr_avail)
      {
        LOG_MSG_INFO1_0("Manager::RequestSocket() : No client "
                        "registered with AP");
        state = DISABLED;
        ret_val = -1;
        break;
      }
      else
      {
        ps_sys_event_ind_ex(PS_SYS_TECH_ALL, 
                         PS_SYS_EVENT_REMOTE_SOCKET_REQUEST,
                         PS_SYS_PRIMARY_SUBS,
                         sockReqInfoPtr,
                         &ps_errno);
        state = REMOTE_SOCKET_REQUESTED;          
      }
      ret_val = 0;
    }
  } while(0);
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  return ret_val;
} /* Manager::RequestSocket */

uint32 Manager::RetrieveAndRequestSocket
(
  uint32                 ps_sock_platform_handle,
  ds::Sock::ProtocolType protocol,
  ds::AddrFamilyType     family
)
{
  uint32                               port_num = 0;
  int16                                result = -1;
  SocketFactory                      * platformSockFactoryPtr;
  Socket                             * platformSockPtr = 0;
  SockInfo                           * sockInfoPtr;
  ds::Utils::List                    * sockCachePtr;
  int16                                ps_errno;
  ps_sys_remote_socket_request_type    sockReqInfo;
  int32                                udpEpncapsVal;
  int32                                udpEpncapsLen;
  ds::SockAddrIN6InternalType          tmpSockAddr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
    
  LOG_MSG_INFO1_3("Manager::RetrieveAndRequestSocket ps_sock_platform_handle 0x%x "
                  "proto %d family %d",
                  ps_sock_platform_handle, protocol, family);

  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);

  /*-------------------------------------------------------------------------
    1. Request one socket from AP/netmgr
    2. If socket request is succesful the retrieve cache based on family and 
       protocol and if sockets are available in the cache then return
       the port number from the cache. Delete that entry from the cache.
       Store the remote socket handle in socket. If socket has udp encaps
       option set then send indication to AP to set udp encpas option for
       that remote socket handle
    3. Otherwise generate ephemeral port, if AP is alive but there are no 
       sockets in cache then send request to AP to allocate new socket
  -------------------------------------------------------------------------*/ 

  /*---------------------------------------------------------------------
    Step 1
  ---------------------------------------------------------------------*/  
  memset(&sockReqInfo, 0, sizeof(ps_sys_remote_socket_request_type));
  sockReqInfo.num_sockets = 1;
  
  sockReqInfo.socket_list[0].port_no = 0;
  sockReqInfo.socket_list[0].xport_proto = ConvertToPSProtocol(protocol);
  sockReqInfo.socket_list[0].ip_family = ConvertToPSFamily(family);
    
  result = RequestSocket(&sockReqInfo);
  
  platformSockPtr = GetPlatformSocketPtr(ps_sock_platform_handle);  
  sockCachePtr = GetSockCache(protocol, family);
 
  /*---------------------------------------------------------------------
    Step 2
  ---------------------------------------------------------------------*/  
  if (0 == result && NULL != sockCachePtr && sockCachePtr->Count() > 0)
  {   
    sockInfoPtr = (SockInfo *)sockCachePtr->PopFront();
    if (NULL != sockInfoPtr)
    {
      port_num = sockInfoPtr->port;
      LOG_MSG_INFO1_4("Manager::RetrieveAndRequestSocket : port %d, handle 0x%x"
                    "protocol %d, family 0x%x", 
                    sockInfoPtr->port, 
                    sockInfoPtr->handle,
                    sockInfoPtr->protocol,
                    sockInfoPtr->family); 
      if (NULL != platformSockPtr)
      {
        platformSockPtr->SetRemoteSocktHandle(sockInfoPtr->handle);
        if (AEE_SUCCESS == 
            platformSockPtr->GetOpt(ds::Sock::OptLevel::QDS_LEVEL_UDP, 
                                  ds::Sock::OptName::QDS_UDP_ENCAPS, 
                                  &udpEpncapsVal, 
                                  &udpEpncapsLen))
        {
          if (0 != udpEpncapsVal)
          {
            SendUDPEncapsSockOpt(sockInfoPtr->handle, TRUE);
          }            
        }       
      }     

      delete sockInfoPtr;   
    }    
  }    
  /*---------------------------------------------------------------------
    Step 3
  ---------------------------------------------------------------------*/   
  else
  {
    platformSockFactoryPtr = SocketFactory::CreateInstance();
    ASSERT( 0 != platformSockFactoryPtr);    
    
    /*-----------------------------------------------------------------
      For IPSEC, IPSEC module always sets the socket option first
      followed by bind operation. if they dont do that way it would result 
      in failure. If socket option for udp encaps is set and if AP
      is not avaiable then return 32012 port which is the port used
      by AP when remote socket bind is not enabled.
    -----------------------------------------------------------------*/     
    if ( NULL != platformSockPtr &&
         AEE_SUCCESS == 
         platformSockPtr->GetOpt(ds::Sock::OptLevel::QDS_LEVEL_UDP, 
                                ds::Sock::OptName::QDS_UDP_ENCAPS, 
                                &udpEpncapsVal, 
                                &udpEpncapsLen))                                    
    {
      if (0 != udpEpncapsVal)
      {
        tmpSockAddr.port = ps_htons(32012);;
        if (FALSE == 
             platformSockFactoryPtr->IsSockAddrInUse( protocol, 
                                                      &tmpSockAddr, 
                                                      false))
        {
          port_num = tmpSockAddr.port;
          LOG_MSG_INFO2_1("Manager::RetrieveAndRequestSocket : "
                          "Returning 32012 port %d for ipsec", port_num);          
        }
      }            
    }
    
    if (0 == port_num)
    {
      port_num = 
        platformSockFactoryPtr->GenerateEphemeralPort(protocol);
    }
    
    DS_UTILS_RELEASEIF(platformSockFactoryPtr);
    
    /*-----------------------------------------------------------------
      If state is not in Disabled state then send request for specific
      socket. This can happen when there are no sockets in cache
      and AP is alive but has not responded to sockets request.
    -----------------------------------------------------------------*/     
    if (state != DISABLED)
    {
      memset(&sockReqInfo, 0, sizeof(ps_sys_remote_socket_request_type));

      sockReqInfo.num_sockets = 1;
      sockReqInfo.socket_list[0].port_no = port_num;
      sockReqInfo.socket_list[0].xport_proto = ConvertToPSProtocol(protocol);
      sockReqInfo.socket_list[0].ip_family = ConvertToPSFamily(family);        
        
      ps_sys_event_ind_ex(PS_SYS_TECH_ALL, 
                       PS_SYS_EVENT_REMOTE_SOCKET_REQUEST,
                       PS_SYS_PRIMARY_SUBS,
                       &sockReqInfo,
                       &ps_errno);                                           
    }
  }
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  
  return port_num;
} /* Manager::RetrieveAndRequestSocket */


int16 Manager::ReleaseSocket
(
  uint32                 handle
)
{
  int16                                  result = 0;
  int16                                  ps_errno;
  ps_sys_remote_socket_release_type      sockReleaseInfo;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  LOG_MSG_INFO1_1("Manager::ReleaseSocket() : handle %d ", handle);
  
  ps_enter_crit_section(&ps_sock_remote_sock_mgr_crit_section);
 
  
  if (0 != handle)
  {
    memset(&sockReleaseInfo, 0, 
           sizeof(ps_sys_remote_socket_release_type));
    sockReleaseInfo.num_sockets = 1;
    sockReleaseInfo.socket_list[0] = handle;
    
    ps_sys_event_ind_ex(PS_SYS_TECH_ALL, 
                     PS_SYS_EVENT_REMOTE_SOCKET_RELEASE,
                     PS_SYS_PRIMARY_SUBS,
                     &sockReleaseInfo,
                     &ps_errno);
  } 
  
  ps_leave_crit_section(&ps_sock_remote_sock_mgr_crit_section);
  return result;
} /* Manager::ReleaseSocket */

void Manager::SendUDPEncapsSockOpt
(
  uint32    handle,
  boolean   udpEncapsSockOpt
)
{
  ps_sys_remote_socket_set_option_type       sockOptInfo;
  int16                                      ps_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  LOG_MSG_INFO1_1("Manager::SendUDPEncapsSockOpt() : handle %d ", handle);
  
  memset(&sockOptInfo, 0, sizeof(ps_sys_remote_socket_set_option_type));                       
  sockOptInfo.socket_handle =  handle;     
  sockOptInfo.is_udp_encaps = udpEncapsSockOpt;        
  ps_sys_event_ind_ex(PS_SYS_TECH_ALL, 
                   PS_SYS_EVENT_REMOTE_SOCKET_SET_OPTION,
                   PS_SYS_PRIMARY_SUBS,
                   &sockOptInfo,
                   &ps_errno);  
} /* Manager::SendUDPEncapsSockOpt */