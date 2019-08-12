#ifndef PS_SOCK_PLATFORM_SOCKET_H
#define PS_SOCK_PLATFORM_SOCKET_H
/*===========================================================================
  @file PS_Sock_Platform_Socket.h

  This file defines the class that implements the PS::Sock::Platform::ISocket
  interface.

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/src/PS_Sock_Platform_Socket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"

#include "PS_Sock_Platform_ISocket.h"
#include "PS_Sock_ProtocolStack_IEventListener.h"
#include "PS_Sock_IO_IReader.h"
#include "PS_Sock_IO_IWriter.h"
#include "PS_Sock_IO_IWriterListener.h"
#include "PS_Sock_IO_IReaderListener.h"
#include "ds_Utils_INode.h"
#include "ds_Utils_List.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Sock_Def.h"
#include "ps_route_scope.h"
#include "ps_socket_defs.h"
#include "ds_Utils_SockAddrInternalTypes.h"
#include "ds_Sock_ISocketLocalPriv.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
//TODO Document Class
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      typedef uint32  EventMaskType;
      typedef int32  StateType;

      class Socket : public ISocket,
                     public PS::Sock::IO::IReaderListener,
                     public PS::Sock::IO::IWriterListener,
                     public ds::Utils::INode,
                     public PS::Sock::ProtocolStack::IEventListener
      {
        public:
          //TODO Fix doc
          /**
            @brief Creates a Socket object.

            Creates a Socket object using the given family, sockType, and
            protocol parameters. ds::Sock::Family, ds::Sock::Type, and
            ds::Sock::Protocol define the supported options for family,
            sockType, and protocol respectively. If protocol is UNSPEC, TCP is
            used for STREAM sockets and UDP is used for DGRAM sockets.

            This method fails to create an Socket object if
              @li Unsupported values are passed for family, sockType, or
                  protocol OR
              @li TCP is not used as protocol for STREAM sockets OR
              @li UDP or ICMP is not used as protocol for DGRAM sockets

            This method calls in to ProtocolStack to create a PCB and also
            assigns reader/writer pairs for Socket object and PCB. Since the
            object is shared by both the sockets library and the ProtocolStack,
            refCnt is used to manage the lifetime. refCnt is set to 2 and the
            object is deleted only when refCnt is 0 (i.e. both sockets library
            and ProtocolStack give up caching the object).

            TCP socket goes to INIT state and UDP/ICMP socket goes to OPEN
            state.

            If the operation fails, dsErrnoPtr is populated with the failure
            reason. Possible values are
              @li QDS_EINVAL          Family is invalid
              @li QDS_ESOCKNOSUPPORT  Type is not supported
              @li QDS_EPROTONOSUPPORT Protocol is not supported
              @li QDS_EPROTOTYPE      (sockType, protocol)
                                                 combination is not supported
              @retval QDS_EFAULT      NULL eventListenerPtr
              @retval QDS_EMFILE      Out of memory

            @param[in]  family            The family of socket to be created
            @param[in]  sockType          The type of socket to be created
            @param[in]  protocol          The protocol to be used
            @param[in]  eventListenerPtr  Ptr to an IEventListener object
            @param[out] dsErrnoPtr        Updated with error value if the
                                          operation fails

            @retval object  Operation completed successfully
            @retval -1      Operation failed, and dsErrnoPtr is populated with
                            failure reason

            @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol,
                 ds::Error
          */
          static Socket * CreateInstance
          (
            ds::AddrFamilyType      family,
            ds::Sock::SocketType    sockType,
            ds::Sock::ProtocolType  protocol,
            ICritSect *             critSectPtr,
            ds::ErrorType *  dsErrnoPtr
          );

          /**
            @brief Registers an event listener for platform socket events.

            Registers the caller for all platform socket events. Process()
            method of IEventListener is invoked whenever an event needs to be
            posted.

            @param[in]  eventListenerPtr  Ptr to an IEventListener object

            @retval None

            @see PS::Sock::Platform::Event
          */
          virtual void RegEventListener
          (
            PS::Sock::Platform::IEventListener *  eventListenerPtr
          );

          /**
            @brief Cancels  registration of an event listener .

            @retval None

            @see PS::Sock::Platform::Event
          */
          virtual void DeregEventListener
          (
          );


          virtual void SetCritSection
          (
            ICritSect *  critSectPtr
          );     

          virtual ds::ErrorType Bind
          (
            const ds::SockAddrInternalType*   localAddrPtr
          );
          
          virtual ds::ErrorType Listen
          (
            int32  backlog
          );

          virtual ds::ErrorType Accept
          (
            Platform::ISocket **              newPlatformSockPtrPtr,
            ds::SockAddrInternalType*               remoteAddrPtr
          );

          virtual ds::ErrorType Connect
          (
            const ds::SockAddrInternalType*         remoteAddrPtr
          );

          /**
            @brief Returns socket's local address/port.

            Returns the local IP address and the port to which a socket is
            bound to.

            Caller must allocate an instance of SockAddrStorageType and pass
            it to this function. It is incorrect to allocate either
            SockAddrInternalType or SockAddrIN6InternalType, cast it to SockAddrStorageType
            and pass it to this function.

            If socket's family is INET, address family in localAddrPtr is set
            to ds::AddrFamily::QDS_AF_INET and addr field is populated with Ipv4 address, else
            address family is set to ds::AddrFamily::QDS_AF_INET6 and addr field is populated
            with IPv6 address.

            If the operation fails, dsErrnoPtr is populated with the failure
            reason. Possible values are
              @li QDS_EFAULT  localAddrPtr is NULL

            @param[out] localAddrPtr  Local IP address and port to which socket
                                      is bound to
            @param[out] dsErrnoPtr    Updated with error value if the
                                      operation fails

            @retval  0  Operation completed successfully
            @retval -1  Operation failed, and dsErrnoPtr is populated with
                        failure reason

            @see ds::Error
          */
          virtual ds::ErrorType GetSockName
          (
            ds::SockAddrInternalType*   localAddrPtr
          ) throw();

          /**
            @brief Returns socket's peer address/port.

            Returns the peer IP address and the port to which a socket is
            connected to. This function succeeds only when a socket is
            connected to a peer, i.e. it fails if socket is still trying to
            connect (as in caller called Connect() but 3-way handshake is not
            complete) or if socket was connected, but Close() is called.

            Caller must allocate an instance of SockAddrStorageType and pass
            it to this function. It is incorrect to allocate either
            SockAddrInternalType or SockAddrIN6InternalType, cast it to SockAddrStorageType
            and pass it to this function.

            If socket's family is INET, address family in remoteAddrPtr is set
            to ds::AddrFamily::QDS_AF_INET and addr field is populated with IPv4 address, else
            address family is set to ds::AddrFamily::QDS_AF_INET6 and addr field is populated
            with IPv6 address.

            If the operation fails, dsErrnoPtr is populated with the failure
            reason. Possible values are
              @li QDS_ENOTCONN  Socket is not in connected state
              @li QDS_EFAULT    remoteAddrPtr is NULL

            @param[out] remoteAddrPtr  Peer IP address and port to which socket
                                       is bound to
            @param[out] dsErrnoPtr    Updated with error value if the
                                      operation fails

            @retval  0  Operation completed successfully
            @retval -1  Operation failed, and dsErrnoPtr is populated with
                        failure reason

            @see ds::Error
          */
          virtual ds::ErrorType GetPeerName
          (
            ds::SockAddrInternalType*   remoteAddrPtr
          );

          virtual ds::ErrorType SetOpt
          (
            ds::Sock::OptLevelType   optLevel,
            ds::Sock::OptNameType    optName,
            int32                    optVal,
            int32                    optLen
          );

          virtual ds::ErrorType GetOpt
          (
            ds::Sock::OptLevelType   optLevel,
            ds::Sock::OptNameType    optName,
            int32 *                  optValPtr,
            int32 *                  optLenPtr
          );

          virtual ds::ErrorType SetOptPriv
          (
            ds::Sock::OptLevelType    optLevel,
            ds::Sock::OptNameTypePriv optName,
            int32                     optVal,
            int32                     optLen
          );

          virtual ds::ErrorType GetOptPriv
          (
            ds::Sock::OptLevelType    optLevel,
            ds::Sock::OptNameTypePriv optName,
            int32 *                   optValPtr,
            int32 *                   optLenPtr
          );

          virtual ds::ErrorType SetSOLingerReset
          (
            const ds::Sock::LingerType *  soLingerPtr
          ) throw();

          virtual ds::ErrorType GetSOLingerReset
          (
            ds::Sock::LingerType *  soLingerPtr
          );

          virtual ds::ErrorType SetRtMetaInfo
          (
            ps_rt_meta_info_type *  routeMetaInfoPtr
          );

          virtual ds::ErrorType SetRouteScope
          (
            ps_route_scope_type *  routeScopePtr
          );

          virtual ds::ErrorType GetRouteScope
          (
            ps_route_scope_type *  routeScopePtr
          );

          virtual void SetSystemOption
          (
            void
          );

          virtual ds::ErrorType SendMsg
          (
            const ds::SockAddrInternalType*        remoteAddrPtr,
            const IPort1::SeqBytes                 ioVecArr[],
            int32                                  numIOVec,
            int32 *                                numWrittenPtr,
            ds::Sock::IAncDataPriv **              inAncillaryDataPtrPtr,
            int32                                  inAncillaryDataLen,
            int32                                  flags,
            ps_pkt_meta_info_type *                pktMetaInfoPtr
          );

          virtual ds::ErrorType RecvMsg
          (
            ds::SockAddrInternalType*          remoteAddrPtr,
            IPort1::SeqBytes                   ioVecArr[],
            int32                              numIOVec,
            int32 *                            numReadPtr,
            ds::Sock::IAncDataPriv **          outAncillaryDataPtrPtr,
            int32                              outAncillaryDataLen,
            int32 *                            outAncillaryDataLenReqPtr,
            uint32 *                           outFlagsPtr,
            int32                              flags
          );

          virtual ds::ErrorType SendDSMChain
          (
            const ds::SockAddrInternalType*              remoteAddrPtr,
            dsm_item_type **                       dsmItemPtrPtr,
            int32                                  flags,
            int32 *                                numWrittenPtr
          );

          virtual ds::ErrorType RecvDSMChain
          (
            ds::SockAddrInternalType*                remoteAddrPtr,
            dsm_item_type **                   dsmItemPtrPtr,
            int32                              flags,
            int32 *                            numReadPtr
          );

          virtual ds::ErrorType Shutdown
          (
            ds::Sock::ShutdownDirType  shutdownDir
          );

          /**
            @brief Non-blocking API to close socket.

            Provides a non-blocking API to close a socket. It returns AEE_SUCCESS
            if socket can be closed immediately. For Connected TCP socket, it
            returns AEE_EWOULDBLOCK as the socket needs to run connection
            termination procedure. In this case, caller is expected to register
            for CLOSE_EV and wait for it. When socket is successfully closed,
            sockets library posts CLOSE_EV.

            Once the socket is closed, it is invalid to perform any operations
            on it.

            @param None

            @retval AEE_SUCCESS      Socket is closed successfully
            @retval AEE_EWOULDBLOCK  Socket needs to be closed
                                     asynchronously
            @retval AEE_ENOMEMORY    Out of resources

            @see PS::Sock::Platform::Event,
                 PS::Sock::Platform::ISocket::AsyncSelect, ds::Error
          */
          virtual ds::ErrorType Close
          (
            void
          ) throw();

          virtual bool IsConnInProgress
          (
            void
          );

          virtual ds::ErrorType AbortConnection
          (
            ds::ErrorType  reasonForAbort
          );

          bool IsSockAddrInUse
          (
            ds::Sock::ProtocolType              protocol,
            const ds::SockAddrIN6InternalType *             v6SockAddrPtr,
            bool                                reUseAddr
          );

          virtual PS::Sock::ProtocolStack::IEventListener * GetIEventListener
          (
            void
          );

          virtual void ProcessTCPConnectedEvent
          (
            const struct connection *  connPtr,
            int32                      optionsMask,
            uint32                     mss
          );

          virtual void ProcessPCBClosedEvent
          (
            uint8  pcbClosedReason
          );

          virtual void ProcessUDPConnectedEvent
          (
            const struct connection *  connPtr
          );

          virtual void ProcessICMPErrorEvent
          (
            int16  icmpError
          );

          virtual void ProcessPlatformEnabledEvent
          (
            void
          );

          virtual void ProcessPlatformDisabledEvent
          (
            void
          );

          virtual void BufferAvailable
          (
            void
          );

          virtual void DataAvailable
          (
            void
          ) throw();

          virtual void EmptyQueue
          (
            void
          );

          virtual boolean Process
          (
            void *  userDataPtr
          );
          
          virtual void SetRemoteSocktHandle
          (
            uint32  handle
          );              
          
          virtual ds::AddrFamilyType GetFamily
          (
            void
          );         

          virtual ds::Sock::ProtocolType GetProtocol
          (
            void
          );     

          virtual uint32 GetRemoteSockHandle
          (
            void
          );          

          virtual int16 GetPort
          (
            void
          );       

          virtual boolean IsSocketAlive
          (
            void
          );

          /* Implementation of IWeakRef methods */
          DS_UTILS_IWEAKREF_IMPL_DEFAULTS()

        private:
          /**
            @brief
            Destructor for the Platform socket object

          */
          virtual void Destructor
          (
            void
          )
          throw();

          /*!
          @brief Dummy destructor. Only used to free memory.

          @details For objects that support weak references, destruction of
          the object and freeing up of memory can happen at different
          times. Hence Destructor() is the one which performs actual
          destruction of the object, where as ~Object() would
          perform memory cleanup.
          @params     None.
          @return     None.
          */
          virtual ~Socket
          (
            void
          ) throw();

          /**
            @brief Overloaded operator new.

            Operator new is overloaded so that memory for PlatformSocket is
            allocated from ps_mem instead of from the system heap.

            As the C Runtime Library throws a std::bad_alloc exception if the
            memory allocation fails, and since memory allocation could fail as
            memory is allocated from ps_mem, this method also handles the
            exception.

            This method is defined as private so that user is forced to use
            CreateInstance() method to create a PlatformSocket object instead
            of using operator new.

            @param[in] sizeOfBuf The amount of memory to be allocated

            @retval address  Ptr to object if memory is allocated
            @retval 0        If memory couldn't be allocated
          */
          void * operator new
          (
            unsigned int sizeOfBuf
          ) throw();

          /**
            @brief Overloaded operator delete.

            Operator delete is overloaded so that memory for PlatformSocket is
            de-allocated and is put back in to ps_mem

            This method is defined as private so that user is forced to use
            Close() method to delete a PlatformSocket object instead of using
            operator delete.

            @param[in] bufPtr  The object to be deleted

            @retval None
          */
          void operator delete
          (
            void *  bufPtr
          ) throw();

          /**
            @brief Constructor for Socket class.
            //TODO Doc critSectPtr and ErrQ

            Initializes a Socket object. Sets all socket options to default
            values. Assigns default SNDBUF size to writer and default RCVBUF
            size to reader. Also registers for events with reader and writer
            objects.

            @param[in]  family            The family of socket to be created
            @param[in]  sockType          The type of socket to be created
            @param[in]  protocol          The protocol to be used
            @param[in]  readerPtr         Ptr to an IReader object
            @param[in]  writerPtr         Ptr to an IWriter object

            @retval None
         */
          Socket
          (
            ds::AddrFamilyType       family,
            ds::Sock::SocketType     sockType,
            ds::Sock::ProtocolType   protocol,
            ICritSect *              critSectPtr,
            PS::Sock::IO::IReader *  readerPtr,
            PS::Sock::IO::IWriter *  writerPtr,
            PS::Sock::IO::IReader *  errQReaderPtr
          );

          void SetState
          (
            StateType  newState
          ) throw();

          bool IsOptCacheSet
          (
            ds::Sock::OptNameType  optName
          );

          bool IsOptPrivCacheSet
          (
            ds::Sock::OptNameTypePriv  optName
          );

          void SetOptCache
          (
            ds::Sock::OptNameType  optName
          );

          void ResetOptCache
          (
            ds::Sock::OptNameType  optName
          );

          void SetOptPrivCache
          (
            ds::Sock::OptNameTypePriv  optName
          );

          ds::ErrorType AllocListener
          (
            void
          );

          Socket * Clone
          (
            void
          );

          void CloneOpt
          (
            Socket * sockPtr
          );
          
          void CloneOptMask
          (
            Socket * sockPtr
          );

          /**
            @brief Returns ds::Error for Connect().

            Returns appropriate ds::Error because of which Connect() failed.

            @param[in]  v6RemoteAddrPtr  Peer address to which socket is
                                         trying to connect to
            @param[out] dsErrnoPtr       Updated with ds::Error

            @retval None

            @see ds::Error
          */
          ds::ErrorType GetDSErrnoForConnect
          (
            ds::SockAddrIN6InternalType *  v6RemoteAddrPtr
          );

          ds::ErrorType GetDSErrnoForSockInput
          (
            void
          );

          ds::ErrorType GetDSErrnoForSockOutput
          (
            void
          );

          void SetOrClearEvent
          (
            PS::Sock::Platform::EventType  event
          )
          throw();

          void SetEventBitMask
          (
            PS::Sock::Platform::EventType  event,
            uint32                          bitMask
          ) throw();

          void ResetEventBitMask
          (
            PS::Sock::Platform::EventType  event,
            uint32                          bitMask
          ) throw();

          void ResetRDErrorAvailableEventBitMask
          (
            void
          );

          /**
            @brief Processes Ancillary Data.

            //TODO - add documentation


            //TODO - add documentation

            User provided buffer is padded with NULLs if size of provided
            buffer is greater than size of ancillary data available.
          */
          void ProcessAncillaryData
          (
            ds::Sock::IAncDataPriv *    ancillaryDataPtr[],
            int32                       numAncData,
            int32                       flags,
            ps_icmp_errq_meta_info *    icmpErrQMetaInfoPtr,
            ps_pkt_read_info *          readInfoPtr,
            int32 *                     numAncDataFilledPtr
          );

          int32                                 platformSockHandle;
          int32                                 pcbHandle;
          int32                                 backlog;
          ds::SockAddrIN6InternalType                   v6LocalSockAddr;
          ds::SockAddrIN6InternalType                   v6RemoteSockAddr;
          ICritSect *                           critSectPtr;
          PS::Sock::IO::IReader *               rcvQReaderPtr;
          PS::Sock::IO::IWriter *               sndQWriterPtr;
          PS::Sock::IO::IReader *               errQReaderPtr;
          PS::Sock::Platform::IEventListener *  eventListenerPtr;
          ps_route_scope_type                   routeScope;
          ds::Sock::ProtocolType                protocol;
          ds::Sock::SocketType                  type;
          ds::AddrFamilyType                    family;
          StateType                             state;                              
          uint32 eventBitMask[PS::Sock::Platform::Event::PLATFORM_ENABLED+1];
          uint32                                remoteSockHandle;

          struct
          {
            uint32                              optionsMask;
            int32                               maxBackoffTime;
            ds::Sock::LingerType                soLinger;
            int32                               sndBuf;
            int32                               rcvBuf;
            int32                               mss;
            uint8                               tos;
            uint8                               tClass;
            uint8                               ttl;
            uint8                               ttlMcast;
            uint8                               mcastLoop;
          } optCache;

          struct
          {
            uint32                              optionsMask;
            uint8                               icmpType;
            uint8                               icmpCode;
          } optPrivCache;

          ds::Utils::List                       listenList;
          ds::Utils::List                       acceptedList;
          Socket *                              listenHeadPtr;
          ds::ErrorType                         soError;
          uint8                                 pcbClosedReason;
          bool                                  isSystemSocket;

      };
    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_SOCKET_H */
