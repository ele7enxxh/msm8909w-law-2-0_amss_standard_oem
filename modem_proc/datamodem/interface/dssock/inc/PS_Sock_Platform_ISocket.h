#ifndef PS_SOCK_PLATFORM_ISOCKET_H
#define PS_SOCK_PLATFORM_ISOCKET_H

#include "comdef.h"
#include "customer.h"

#include "ds_Utils_StdErr.h"
#include "ds_Utils_IPort1.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Sock_Def.h"
#include "ds_Sock_IAncDataPriv.h"
#include "PS_Sock_Platform_EventDefs.h"
#include "PS_Sock_Platform_IEventListener.h"
#include "ps_route_scope.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_meta_info.h"
#include "ds_Net_Def.h"
#include "ds_Utils_SockAddrInternalTypes.h"
#include "ds_Sock_ISocketLocalPriv.h"

/*lint -esym(1510, ISocket) */
namespace PS
{
  namespace Sock
  {
    namespace Platform
    {
      class ISocket
      {
        public:
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
          ) = 0;

          /**
            @brief Cancels  registration of an event listener .

            @retval None

            @see PS::Sock::Platform::Event
          */
          virtual void DeregEventListener
          (
          ) = 0;


          virtual void SetCritSection
          (
            ICritSect *  critSectPtr
          ) = 0;

          virtual ds::ErrorType Bind
          (
            const ds::SockAddrInternalType  *  localAddrPtr
          ) = 0;

          virtual ds::ErrorType Listen
          (
            int32  backlog
          ) = 0;

          virtual ds::ErrorType Accept
          (
            Platform::ISocket **   newPlatformSockPtrPtr,
            ds::SockAddrInternalType *  remoteAddrPtr
          ) = 0;

          virtual ds::ErrorType Connect
          (
            const ds::SockAddrInternalType *  remoteAddrPtr
          ) = 0;
//TODO Fix doc
          /**
            @brief Returns socket's local address/port.

            Returns the local IP address and the port to which a socket is
            bound to.

            Caller must allocate an instance of SockAddrStorageType and pass it
            to this function. It is incorrect to allocate either
            SockAddrInternalType or SockAddrIN6InternalType, cast it to SockAddrStorageType
            and pass it to this function.

            If socket's family is INET, address family in localAddrPtr is set
            to ds::AddrFamily::QDS_AF_INET and addr field is populated with IPv4 address, else
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
            ds::SockAddrInternalType *  localAddrPtr
          ) throw() = 0;

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
            ds::SockAddrInternalType *  remoteAddrPtr
          ) = 0;

          virtual ds::ErrorType SetOpt
          (
            ds::Sock::OptLevelType   optLevel,
            ds::Sock::OptNameType    optName,
            int32                    optVal,
            int32                    optLen
          ) = 0;

          virtual ds::ErrorType GetOpt
          (
            ds::Sock::OptLevelType   optLevel,
            ds::Sock::OptNameType    optName,
            int32 *                  optValPtr,
            int32 *                  optLenPtr
          ) = 0;

          virtual ds::ErrorType SetOptPriv
          (
            ds::Sock::OptLevelType    optLevel,
            ds::Sock::OptNameTypePriv optName,
            int32                     optVal,
            int32                     optLen
          ) = 0;

          virtual ds::ErrorType GetOptPriv
          (
            ds::Sock::OptLevelType    optLevel,
            ds::Sock::OptNameTypePriv optName,
            int32 *                   optValPtr,
            int32 *                   optLenPtr
          ) = 0;

          virtual ds::ErrorType SetSOLingerReset
          (
            const ds::Sock::LingerType *  soLingerPtr
          ) throw() = 0;

          virtual ds::ErrorType GetSOLingerReset
          (
            ds::Sock::LingerType *  soLingerPtr
          ) = 0;

          virtual ds::ErrorType SetRtMetaInfo
          (
            ps_rt_meta_info_type *  routeMetaInfoPtr
          ) = 0;

          virtual ds::ErrorType SetRouteScope
          (
            ps_route_scope_type *  routeScopePtr
          ) = 0;

          virtual ds::ErrorType GetRouteScope
          (
            ps_route_scope_type *  routeScopePtr
          ) = 0;

          virtual void SetSystemOption
          (
            void
          ) = 0;

          virtual ds::ErrorType SendMsg
          (
          const ds::SockAddrInternalType *         remoteAddrPtr,
            const IPort1::SeqBytes                 ioVecArr[],
            int32                                  numIOVec,
            int32 *                                numWrittenPtr,
            ds::Sock::IAncDataPriv **              inAncillaryDataPtrPtr,
            int32                                  inAncillaryDataLen,
            int32                                  flags,
            ps_pkt_meta_info_type *                pktMetaInfoPtr
          ) = 0;

          virtual ds::ErrorType RecvMsg
          (
            ds::SockAddrInternalType *         remoteAddrPtr,
            IPort1::SeqBytes                   ioVecArr[],
            int32                              numIOVec,
            int32 *                            numReadPtr,
            ds::Sock::IAncDataPriv **          outAncillaryDataPtrPtr,
            int32                              outAncillaryDataLen,
            int32 *                            outAncillaryDataLenReqPtr,
            uint32 *                           outFlagsPtr,
            int32                              flags
          ) = 0;

          virtual ds::ErrorType SendDSMChain
          (
            const ds::SockAddrInternalType *             remoteAddrPtr,
            dsm_item_type **                       dsmItemPtrPtr,
            int32                                  flags,
            int32 *                                numWrittenPtr
          ) = 0;

          virtual ds::ErrorType RecvDSMChain
          (
            ds::SockAddrInternalType *               remoteAddrPtr,
            dsm_item_type **                   dsmItemPtrPtr,
            int32                              flags,
            int32 *                            numReadPtr
          ) = 0;

          virtual ds::ErrorType Shutdown
          (
            ds::Sock::ShutdownDirType  shutdownDir
          ) = 0;

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

            @retval AEE_SUCCESS                 Socket is closed successfully
            @retval AEE_EWOULDBLOCK             Socket needs to be closed
                                                asynchronously
            @retval AEE_ENOMEMORY               Out of resources

            @see PS::Sock::Platform::Event,
                 PS::Sock::Platform::ISocket::AsyncSelect, ds::Error
          */
          virtual ds::ErrorType Close
          (
            void
          ) throw() = 0;

          virtual bool IsConnInProgress
          (
            void
          ) = 0;

          virtual ds::ErrorType AbortConnection
          (
            ds::ErrorType  reasonForAbort
          ) = 0;
          
          virtual void SetRemoteSocktHandle
          (
            uint32  handle
          ) = 0;     
          
          virtual ds::AddrFamilyType GetFamily
          (
            void
          )= 0;       

          virtual ds::Sock::ProtocolType GetProtocol
          (
            void
          )= 0;  

          virtual uint32 GetRemoteSockHandle
          (
            void
          )= 0;   

          virtual int16 GetPort
          (
            void
          ) = 0;        

          virtual boolean IsSocketAlive
          (
            void
          ) = 0;
      };
    } /* namespace Platform */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_PLATFORM_ISOCKET_H */
