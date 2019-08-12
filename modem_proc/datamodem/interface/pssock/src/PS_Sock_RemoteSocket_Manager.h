#ifndef PS_SOCK_REMOTESOCKET_Manager_H
#define PS_SOCK_REMOTESOCKET_Manager_H
/*===========================================================================

                 PS _ SOCK _ REMOTESOCKET _ Manager . H

DESCRIPTION

  PS SOCK REMOTESOCKET MANGER - Definitions

  
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_RemoteSocket_Manager.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/09/13    BVD    First revision
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "ds_Utils_ICritSect.h"
#include "ds_Sock_AddrUtils.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_List.h"
#include "ds_Utils_CSSupport.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"
#include "ps_sys.h"
#include "PS_Sock_Platform_Socket.h"
#include "PS_Sock_RemoteSocket_IManager.h"
#include "ps_in.h"

using namespace PS::Sock::Platform;

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
namespace PS
{
  namespace Sock
  {
    namespace RemoteSocket
    {  
      /**
        @brief Remote Socket Manager Class implementing the Remote Socket
               Manager Interface.
      */    
      class Manager : public IManager
      {
        private:
        
        /**
          @brief Singleton instance of Remote Socket Manager.
        */          
        static Manager  * instance;
        
        /**
          @brief Private Constructor Singleton instance.
        */         
        Manager() 
        { //Private Constructor
        }
        
        /**
          @brief Remote Socket cache for sockets of family IPV4 and protocol UDP
                 It will have objects of type SockInfo
        */         
        ds::Utils::List            sockIPv4UDPCache;
        
        /**
          @brief Remote Socket cache for sockets of family IPV4 and protocol TCP
                 It will have objects of type SockInfo
        */         
        ds::Utils::List            sockIPv4TCPCache;
        
        /**
          @brief Remote Socket cache for sockets of family IPV6 and protocol UDP
                 It will have objects of type SockInfo
        */         
        ds::Utils::List            sockIPv6UDPCache;
        
        /**
          @brief Remote Socket cache for sockets of family IPV6 and protocol TCP
                 It will have objects of type SockInfo
        */         
        ds::Utils::List            sockIPv6TCPCache;      
        
        /**
          @brief Represents current state of Remote Socket Manager
        */        
        State                      state;
        
        /**
          @brief Remote socket manger registers for iface up indication and
                 requests for max remote sockets when first call is up. 
                 psIfaceEvHandlePtr is handle to iface up event registration
        */         
        void                     * psIfaceEvHandlePtr;
        
        /** 
          @brief  This member function checks if given state of Remote socket
                  manager and number of sockets requested, if it is feasible to
                  send the request to AP/netmgr
                  
          @param[in] num_sockets   Number of remote sockets request
          @param[in] state         Current state of Remote Socket Manager

          @return  TRUE if request is feasible else FALSE
        */  
        boolean  SocketRequestAllowed
        (
          uint32  num_sockets, 
          State   state
        );  
        
        /** 
          @brief  This member function returns PS Sock Socket pointer from 
                   its handle
                  
          @param[in] ps_sock_platform_handle Platform Socket handle
          
          @return  Pointer to PS Sock Socket
        */        
        Socket * GetPlatformSocketPtr
        (
          uint32  ps_sock_platform_handle
        );    
        
        /** 
          @brief  This member function returns pointer to Remote Socket cache
                  for given protocol and family

          @param[in] protocol                  Type of protocol
          @param[in] family                    IP family of socket

          @return  Pointer to Remote Socket Cache
        */        
        ds::Utils::List * GetSockCache
        (
          ds::Sock::ProtocolType  protocol, 
          ds::AddrFamilyType      family
        );                                               
        
        /** 
          @brief  This member function adds Remote socket information for
                  reserved socket on AP into Socket cache.

          @param[in] remoteSockInfoPtr   Remote Socket information

          @return  None
        */         
        void AddSockInfoToCache
        (
          ps_sys_remote_socket_allocated_info_type * remoteSockInfoPtr
        );
        
        /** 
          @brief  This member function goes through all PS Sock Sockets
                  and request Remote Socket for all the sockets for which
                  there is not Remote Socket Handle and whose ports are
                  between 32000 to 36999

          @param None

          @return  Number of Remote Sockets requested
        */         
        int RequestAllocatedSocket
        (
          void
        );
        
        /** 
          @brief  This member function converts protocol from PS Protocol type
                  to dS Sock Type

          @param[in] protocol   PS Type protocol

          @return  DS Sock type Protocol
        */           
        ds::Sock::ProtocolType ConvertToDSSockProtocol
        (
          ps_ip_protocol_enum_type protocol
        );
  
        /** 
          @brief  This member function converts family from PS IP family type
                  to dS Addr Type family

          @param[in] family   PS Type IP Family

          @return  DS Addr type IP family
        */  
        ds::AddrFamilyType ConvertToDSSockFamily
        (
          ip_version_enum_type family
        );
          
        /** 
          @brief  This member function converts protocol from DS Sock type
                  to PS type protocol

          @param[in] protocol   DS Sock type protocol

          @return  PS type Protocol
        */      
        ps_ip_protocol_enum_type ConvertToPSProtocol
        (
          ds::Sock::ProtocolType  protocol
        );
          
        /** 
          @brief  This member function converts family from DS Addr type
                  to PS type family

          @param[in] family   DS Addr type family

          @return  PS type family
        */          
        ip_version_enum_type ConvertToPSFamily
        (
          ds::AddrFamilyType family
        );
        
        /** 
          @brief  This member function checks if remote socket request is
                  permissible and if so it will send request to AP/netmgr
                  to query remote sockets
                  
          @param[in] sockReqInfoPtr   Structure containing sockets to be reserved

          @return  0 on SUCCESS otherwise FAILURE
        */        
        int16 RequestSocket
        (
          ps_sys_remote_socket_request_type * sockReqInfoPtr
        );  
        
        /** 
          @brief  This member function goes through all PS Sock Sockets and finds
                  the matching socket and provides the remote socket handle to
                  matching PS Sock Socket.
                  
          @param[in] remoteSockInfoPtr   Structure containing socket reserved by AP

          @return  None
        */        
        void AddHandleToSocket
        (
          ps_sys_remote_socket_allocated_info_type * remoteSockInfoPtr
        );
        
        public:
        /**
          @brief Returns Singleton Manager object.

          @param None

          @retval address  Manager is created successfully
          @retval 0        Out of memory
        */
        static Manager * GetInstance(void);   
        
        /**
          @brief Initializes Remote Socket Manager

          @param None

          @retval None
        */        
        static void Init(void); 
        
        /**
          @brief Frees Remote Socket Manager

          @param None

          @retval None
        */          
        static void DeInit(void);
        
        /**
          @brief Destructor of Remote Socket Manager

          @param None

          @retval None
        */      
        ~Manager()
        {
        }        
        
        /** 
          @brief  Remote socket manger registers for iface up indication and
                  requests for max remote sockets when first call is up. 
                  psIfaceEvHandlePtr is handle to iface up event registration

          @param[in] psIfacePtr        PS Iface ptr
          @param[in] psIfaceEvent      PS iface event
          @param[in] psIfaceEventInfo  Event info
          @param[in] userDataPtr       Ptr to User Data

          @return  None
        */           
        static void PSIfaceEventCback
        (
          ps_iface_type *             psIfacePtr,
          ps_iface_event_enum_type    psIfaceEvent,
          ps_iface_event_info_u_type  psIfaceEventInfo,
          void *                      userDataPtr
        );   

        /** 
          @brief  Remote socket manger registers for AP/netmgr availabe 
                  event at bootup. On recieving the event, it will check
                  if there were any sockets created before AP/netmgr
                  was available and if so it will request remote sockets
                  matching those sockets. It will also send request for
                  reserving sockets for its cache.

          @param[in] tech_type        Technology type
          @param[in] event_name       PS Sys Event
          @param[in] event_info_ptr   Event info
          @param[in] user_data_ptr    Ptr to User Data

          @return  None
        */         
        static void ps_sys_event_remote_socket_hdlr_change_cb
        (
          ps_sys_tech_enum_type              tech_type,
          ps_sys_event_enum_type             event_name,
          ps_sys_subscription_enum_type      subscription_id,
          void                             * event_info_ptr,
          void                             * user_data_ptr
        );
           
        /** 
          @brief  Remote socket manger registers for callback for ioctl
                  PS_SYS_IOCTL_REMOTE_SOCKET_ALLOCATED. AP/netmgr would
                  send indication informing the sockets have been reserved.
                  On recieving this callback, remote socket manager would
                  go through all sockets reserved and depending on type
                  of socket, it would either add socket to its cache or
                  it will go through all PS Sock Sockets and find the 
                  matching socket and sets its Remote Socket Handle                  

          @param[in] ioctl_name        Ioctl name
          @param[in] arg_val_ptr       Ioctl information
          @param[in] subscription_id   Subscription id
          @param[out] ps_errno         Error information

          @return  0 on SUCCESS otherwise FAILURE
        */           
        static int16 ps_sys_ioctl_socket_allocated
        (
          ps_sys_ioctl_enum_type          ioctl_name,
          void                          * arg_val_ptr,
          ps_sys_subscription_enum_type   subscription_id,
          int16                         * ps_errno
        );       
        
        /** 
          @brief  This member function Requests Max number of sockets that
                  Remote manager can cache at a given time Currently max
                  socket cache is set to 20

          @param None

          @return  0            On Success.
          @return  -1           On failure. 
        */         
        int16 RequestMaxSockets(void);
        
        /** 
          @brief  This member function retrieves socket from the cache and
                  uses its port information if cache is available. If there
                  are no entries in cache, it will generate one random port
                  number between 32000 to 36999 and will return it to user.
                  For every socket consumed from the cache it will make 
                  requet to AP/netmger to reserve one more socket. This 
                  way total entries in cache is max sockets (20 sockets)

          @param[in] ps_sock_platform_handle   Handle representing PS Socket
          @param[in] protocol                  Type of protocol
          @param[in] family                    IP family of socket

          @return  port number            On Success.
          @return  0                      On failure. 
        */         
        uint32 RetrieveAndRequestSocket
        (
          uint32                 ps_sock_platform_handle, 
          ds::Sock::ProtocolType protocol, 
          ds::AddrFamilyType     family
        );
        
        /** 
          @brief  This member function releases the remote socket by sending
                  remote socket handle to AP/netmgr to close that socket

          @param[in] handle   Handle representing Remote socket on AP

          @return  0            On Success.
          @return  -1           On failure. 
        */            
        int16 ReleaseSocket
        (
          uint32  handle
        );
        
        /** 
          @brief  This member function retrieves number of entries in 
                  Remote Socket Cache count for given protocol and family.
                  Note there are four cache one for each combination of
                  protocol and family i.e IPV4UDPSocket Cache, 
                  IPV6UDPSocket Cache, IPV4TCPSocket Cache and
                  IPV6TCPSocket Cache

          @param[in] protocol                  Type of protocol
          @param[in] family                    IP family of socket

          @return  number of entries in cache
        */        
        uint8 GetRemoteSockCacheCnt
        (
          ds::Sock::ProtocolType  protocol, 
          ds::AddrFamilyType      family
        );
        
        /** 
          @brief  This member function retrieves socket cache entry
                  located at certain index for given protocol/family
                  remote socket cache

          @param[in] index                     Index into cache
          @param[in] protocol                  Type of protocol
          @param[in] family                    IP family of socket

          @return  Remote Socket Cache entry
        */         
        SockInfo * GetRemoteSockCacheEntry
        (
          uint8                    index,
          ds::Sock::ProtocolType   protocol, 
          ds::AddrFamilyType       family
        );
        
        /** 
          @brief  This member function returns current state of
                  Remote socket Manager

          @param None

          @return  Current State of the Remote Socket Manager
        */           
        State GetState(void); 
        
        /** 
          @brief  This member function sends UDP encapsulation option
                  of socket to AP/netmgr

          @param[in] handle                    Remote Socket handle
          @param[in] udpEncapsSockOpt          Is udp encaps option set

          @return  None
        */        
        void SendUDPEncapsSockOpt
        (
          uint32   handle, 
          boolean  udpEncapsSockOpt
        );
      };
    } /* namespace RemoteSocket */
  } /* namespace SOCK */
} /* namespace PS */

#endif /* PS_SOCK_REMOTESOCKET_Manager_H */