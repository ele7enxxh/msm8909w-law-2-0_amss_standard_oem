#ifndef PS_SOCK_REMOTESOCKET_IMANAGER_H
#define PS_SOCK_REMOTESOCKET_IMANAGER_H
/*===========================================================================

               PS _ SOCK _ SOCKET _ IMANAGER . H

DESCRIPTION

  PS SOCK SOCKET IREMOTEMANGER - Definitions

  
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_RemoteSocket_IManager.h#1 $
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
#include "ps_sys.h"
#include "ps_system_heap.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/
const int maxAllowedSocketInCache = 20;

namespace PS
{
  namespace Sock
  {
    namespace RemoteSocket
    {
      /**
        @brief Data type for different states or Remote Socket Manager
      */
      typedef enum
      {
        INIT                      = 0,
        /*< Open for making socket request */
        REMOTE_SOCKET_REQUESTED   = 1,
        /*< Sockets request already in transit */
        REMOTE_SOCKET_RECIEVED    = 2,
        /*< Sockets already present, no need to make more request */
        DISABLED                  = 3 
        /*< No QMI clients, return ephemeral port */
      } State;    
      
      /**
        @brief Class representing Cache entry for Remote socket information
               of the sockets created by AP/netmgr. Each object will have
               port, handle, protocol and family information for remote
               socket
      */      
      class SockInfo: public ds::Utils::INode 
      {
        public:
          SockInfo(uint16                 port_num, 
                   uint32                 port_handle, 
                   ds::Sock::ProtocolType protocolVal, 
                   ds::AddrFamilyType     familyVal): 
                   refCnt(1),
                   weakRefCnt(1)
          {
            port       = port_num;
            handle     = port_handle;
            protocol   = protocolVal;
            family     = familyVal;
          }

          void Destructor() 
          {
          
          }

          inline int16 GetPort()
          {
            return port;
          }
          
          inline int16 GetHandle()
          {
            return handle;
          }
          
          inline ds::Sock::ProtocolType GetProtocol()
          {
            return protocol;
          }

          inline ds::AddrFamilyType GetFamily()
          {
            return family;
          }  

          /* Inherited from INode */
          virtual boolean Process (void*  pUserData)
          {
            (void) pUserData;
            
            return TRUE;
          }

          DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

          void * operator new
          (
            unsigned int numBytes
          )
          throw()
          {
            void * buf = NULL;
            PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void*);
            return buf;
          }

          void operator delete
          (
            void *  bufPtr
          )
          throw()
          {
            PS_SYSTEM_HEAP_MEM_FREE(bufPtr);
          }
          
          uint16                   port;
          uint32                  handle;
          ds::Sock::ProtocolType  protocol;
          ds::AddrFamilyType      family;
      };      

      /**
        @brief Interface representing the Remote Socket Manager. Functions
               defined here needs to be implemented by Main class. User of
               Remote Manager would only use this Interface to create 
               object.
      */
      class IManager
      {   
        public:
        
        /**
          @brief Destructor.

          @param None

          @retval None
        */        
        virtual ~IManager() = 0;
        
        /**
          @brief Returns Singleton Manager object.

          @param None

          @retval address  Manager is created successfully
          @retval 0        Out of memory
        */
        static IManager * GetInstance(void);   
        

        /** 
          @brief  This member function Requests Max number of sockets that
                  Remote manager can cache at a given time Currently max
                  socket cache is set to 20

          @param None

          @return  0            On Success.
          @return  -1           On failure. 
        */        
        virtual int16 RequestMaxSockets(void) = 0;
        
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
        virtual uint32 RetrieveAndRequestSocket
        (
          uint32                 ps_sock_platform_handle, 
          ds::Sock::ProtocolType protocol, 
          ds::AddrFamilyType     family
        ) = 0;
        
        /** 
          @brief  This member function releases the remote socket by sending
                  remote socket handle to AP/netmgr to close that socket

          @param[in] handle   Handle representing Remote socket on AP

          @return  0            On Success.
          @return  -1           On failure. 
        */            
        virtual int16 ReleaseSocket
        (
          uint32  handle
        ) = 0;
        
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
        virtual uint8 GetRemoteSockCacheCnt
        (
          ds::Sock::ProtocolType  protocol, 
          ds::AddrFamilyType      family
        ) = 0;
        
        /** 
          @brief  This member function retrieves socket cache entry
                  located at certain index for given protocol/family
                  remote socket cache

          @param[in] index                     Index into cache
          @param[in] protocol                  Type of protocol
          @param[in] family                    IP family of socket

          @return  Remote Socket Cache entry
        */         
        virtual SockInfo * GetRemoteSockCacheEntry
        (
          uint8                    index,
          ds::Sock::ProtocolType   protocol, 
          ds::AddrFamilyType       family
        ) = 0;
        
        /** 
          @brief  This member function returns current state of
                  Remote socket Manager

          @param None

          @return  Current State of the Remote Socket Manager
        */           
        virtual State GetState(void) = 0;        
        
        /** 
          @brief  This member function sends UDP encapsulation option
                  of socket to AP/netmgr

          @param[in] handle                    Remote Socket handle
          @param[in] udpEncapsSockOpt          Is udp encaps option set

          @return  None
        */        
        virtual void SendUDPEncapsSockOpt
        (
          uint32   handle, 
          boolean  udpEncapsSockOpt
        ) = 0;            
      };
    } /* namespace RemoteSocket */
  } /* namespace SOCK */
} /* namespace PS */

#endif /* PS_SOCK_REMOTESOCKET_IMANAGER_H */