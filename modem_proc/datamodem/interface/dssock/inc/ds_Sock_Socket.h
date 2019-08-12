#ifndef DS_SOCK_SOCKET_H
#define DS_SOCK_SOCKET_H
/*===========================================================================
  @file ds_Sock_Socket.h

  This file defines the class that implements the ISocket interface.

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_Socket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Utils_CSSupport.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_OnDemandSignalBus.h"
#include "ds_Utils_SignalHandlerBase.h"
#include "ds_Net_IPolicy.h"
#include "ds_Sock_ISocket.h"
#include "ds_Sock_ISocketLocalPriv.h"
#include "IDSMUtils.h"
#include "PS_Sock_Platform_ISocket.h"
#include "PS_Sock_Platform_IEventListener.h"
#include "ds_Sock_EventDefs.h"
#include "ds_Utils_ISignal.h"
#include "ds_Utils_ISignalCtl.h"
#include "ds_Utils_ISignalHandler.h"
#include "ds_Utils_INode.h"
#include "ds_Net_INetwork.h"
#include "ps_rt_meta_info.h"
#include "ps_pkt_info.h"
#include "ps_system_heap.h"
#include "dsm.h"
#include "ds_Utils_SockAddrInternalTypes.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
//MSR draw a state diagram look at TCP RFC
//Add it to coding guidelines
namespace ds
{
  namespace Sock
  {
    /**
    Forward declaration for NetworkCtl
    */
    class NetworkCtl;

    typedef int32  PSMemEventType;

    namespace PSMemEvent
    {
      const PSMemEventType  PS_MEM_BUF_AVAILABLE = 0;
    }

    namespace SocketEvent
    {
      /*------------------------------------------------------------------
        The number of possible events for the socket
      ------------------------------------------------------------------*/
      const uint32 NUM_EV = 5;

      typedef uint32  BitMaskType;
      /*--------------------------------------------------------------------
        For each event bit mask, there are two parts:
        1. Transient errors, which need action from the application.
        This means that event must be asserted whenever a transient error is
        set.

        2. Flow control events, which are set when resources are not available.
        This means that event can't be asserted.

        Between above two, transient errors always take precedence, as in
        if transient bit is set, event must be asserted even though flow
        control bits are also set.

        SetEventBitMask can set either a transient error or a flow control
        error.
        As explained above, event must be posted only if transient error is set
        and must not be posted if flow control event is set.
        So checking if bit mask has any transient errors bit set is good enough
        to decide if event needs to be posted.

        ResetEventBitMask can reset either a transient error or a flow control
        error. As explained above, event can't be posted if transient error
        is reset and event can be posted only if none of the flow control bits
        are set. So checking if all of flow control bits are 0 in a event bit
        mask is good enough to decide if event needs to be posted.
      --------------------------------------------------------------------*/
      #define TRANSIENT_ERR_BIT_MASK          0x0000FF00
      #define DONT_CARE_EVENT_BIT_MASK        0x000000FF
      #define FLOW_CONTROL_EVENT_BIT_MASK     0xFFFF0000

      typedef enum
      {
        /*------------------------------------------------------------------
          Dont Care Errors
        ------------------------------------------------------------------*/
        WRITE_BIT_MASK_NULL_ROUTING_INFO               = 0x00000001,
        WRITE_BIT_MASK_DORMANT_TRAFFIC_CHANNEL         = 0x00000002,

        /*------------------------------------------------------------------
          Flow control events
        ------------------------------------------------------------------*/
        WRITE_BIT_MASK_IFACE_NOT_WRITEABLE             = 0x00010000,
        WRITE_BIT_MASK_FLOW_DISABLED                   = 0x00020000,
        WRITE_BIT_MASK_PS_FLOW_SUSPENDED               = 0x00040000,
        WRITE_BIT_MASK_SOCKET_PLATFORM_DISABLED        = 0x00080000,
        WRITE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED = 0x00100000,
        WRITE_BIT_MASK_PS_MEM_BUF_NOT_AVAILABLE        = 0x00200000,
        WRITE_BIT_MASK_FLOW_FWDING_DISABLED            = 0x00400000

      } WriteBitMaskEnumType;

      typedef enum
      {
        /*------------------------------------------------------------------
          Dont Care Errors
        ------------------------------------------------------------------*/
        READ_BIT_MASK_NULL_ROUTING_INFO                = 0x00000001,
        /*-------------------------------------------------------------------
          Flow control events
        -------------------------------------------------------------------*/
        READ_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED  = 0x00010000

      } ReadBitMaskEnumType;

      typedef enum
      {
        /*-------------------------------------------------------------------
          Flow control events
        -------------------------------------------------------------------*/
        ACCEPT_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED  = 0x00010000

      } AcceptBitMaskEnumType;

      typedef enum
      {
        /*-------------------------------------------------------------------
          Flow control events
        -------------------------------------------------------------------*/
        CLOSE_BIT_MASK_SOCKET_PLATFORM_FLOW_CONTROLLED   = 0x00010000

      } CloseBitMaskEnumType;

      typedef enum
      {
        /*-------------------------------------------------------------------
          Transient errors
        -------------------------------------------------------------------*/
        DOS_ACK_BIT_MASK_INFO_AVAILABLE  = 0x00000001

      } DoSAckBitMaskEnumType;

    } /* namespace Event */

    /*lint -esym(1510, IQI) */
    /*lint -esym(1510, ISocketExt) */
    /*lint -esym(1510, ISocketLocalPriv) */
    class Socket : public ISocket,
                   public ISocketLocalPriv,
                   public ISocketExt,
                   public IDSMUtils,
                   public PS::Sock::Platform::IEventListener,
                   public ds::Utils::INode,
                   public ds::Utils::SignalHandlerBase
    {
      public:
        /**
           @brief Binds a socket to an adress/port.

           This function associates a local IP address and a port number to the socket
           identified by the platformHandle. A random port is used to bind if zero port
           is used. This function also makes sure that the port is not in use by
           any other socket.

           @param[in] platformHandle Platform level socket descriptor
           @param[in] localAddrPtr Local IP address and port to be used in binding
           @see ds::SockAddrStorageType
           @param[out] dsErrnoPtr Updated with error value if the operation fails
           @see ds::Error

           @retval 0 The operation completed successfully
           @retval -1 The operation failed and dsErrnoPtr is updated with an error code
        */

        virtual ds::ErrorType CDECL Bind
        (
          const SockAddrStorageType   localAddrPtr
        );

        virtual ds::ErrorType CDECL Connect
        (
          const SockAddrStorageType   remoteAddrPtr
        );

        virtual ds::ErrorType CDECL Listen
        (
          int  backlog
        );

        virtual ds::ErrorType CDECL Accept
        (
          SockAddrStorageType    remoteAddrPtr,
          ISocket **             newSockPtr
        );

        /**
          @brief Returns socket's local address/port.

          Returns the local IP address and the port to which a socket is
          bound to.

          Caller must allocate an instance of SockAddrStorageType and pass it
          to this function. It is incorrect to allocate either SockAddrInternalType
          or SockAddrIN6InternalType, cast it to SockAddrStorageType and pass it to
          this function.

          If socket's family is INET, address family in localAddrPtr is set
          to ds::DSS_AF_INET and addr field is populated with IPv4 address,
          else address family is set to ds::DSS_AF_INET6 and addr field is
          populated with IPv6 address.

          @param[out] localAddrPtr  Local IP address and port to which socket
                                    is bound to

          @retval AEE_SUCCESS     Address/port is obtained successfully
          @retval QDS_EFAULT  localAddrPtr is NULL
        */
        virtual ds::ErrorType CDECL GetSockName
        (
          SockAddrStorageType   localAddrPtr
        );

        /**
          @brief Returns socket's peer address/port.

          Returns the peer IP address and the port to which a socket is
          connected to. This function succeeds only when a socket is connected
          to a peer, i.e. it fails if socket is still trying to connect
          (as in caller called Connect() but 3-way handshake is not complete)
          or if socket was connected, but Close() is called.

          Caller must allocate an instance of SockAddrStorageType and pass it
          to this function. It is incorrect to allocate either SockAddrInternalType
          or SockAddrIN6InternalType, cast it to SockAddrStorageType and pass it to
          this function.

          If socket's family is INET, address family in remoteAddrPtr is set
          to ds::DSS_AF_INET and addr field is populated with IPv4 address,
          else address family is set to ds::DSS_AF_INET6 and addr field is
          populated with IPv6 address.

          @param[out] remoteAddrPtr  Peer IP address and port to which socket
                                     is bound to

          @retval AEE_SUCCESS       Address/port is obtained
                                           successfully
          @retval QDS_ENOTCONN  Socket is not in connected state
          @retval QDS_EFAULT    remoteAddrPtr is NULL
        */
        virtual ds::ErrorType CDECL GetPeerName
        (
          SockAddrStorageType   remoteAddrPtr
        );

        virtual ds::ErrorType CDECL Write
        (
          const byte   bufArr[],
          int          bufLen,
          int *        numWrittenPtr
        );

        virtual ds::ErrorType CDECL WriteV
        (
          const IPort1::SeqBytes  ioVecArr[],
          int                     numIOVec,
          int   *                 numWrittenPtr
        );

        virtual ds::ErrorType CDECL SendTo
        (
          const byte                    bufArr[],
          int                           bufLen,
          const SockAddrStorageType     remoteAddrPtr,
          unsigned int                  flags,
          int   *                       numWrittenPtr
        );

        virtual ds::ErrorType CDECL Read
        (
          byte   bufArr[],
          int    bufLen,
          int *  numReadPtr
        );

        virtual ds::ErrorType CDECL ReadV
        (
          IPort1::SeqBytes  ioVecArr[],
          int               numIOVec,
          int *             numReadPtr
        );

        virtual ds::ErrorType CDECL RecvFrom
        (
          byte                   bufArr[],
          int                    bufLen,
          int *                  numReadPtr,
          unsigned int           flags,
          SockAddrStorageType    remoteAddrPtr
        );

        virtual ds::ErrorType CDECL SendMsg
        (
          const SockAddrStorageType msg_name,
          const byte *           msg_buf,
          int                    msg_bufLen,
          int *                  numWritten,
          const byte *           msg_control,
          int                    msg_controlLen,
          unsigned int           flags
        );

        virtual ds::ErrorType CDECL RecvMsg
        (
          SockAddrStorageType   msg_name,
          byte *                msg_buf,
          int                   msg_bufLen,
          int *                 msg_bufLenReq,
          byte *                msg_control,
          int                   msg_controlLen,
          int *                 msg_controlLenReq,
          unsigned int *        msg_flags,
          unsigned int          flags
        );

        virtual ds::ErrorType CDECL SendMsg
        (
          const SockAddrStorageType    remoteAddrPtr,
          const IPort1::SeqBytes *     ioVecArr,
          int                          numIOVec,
          int *                        numWrittenPtr,
          IAncDataPriv **              inAncillaryDataPtrPtr,
          int                          inAncillaryDataLen,
          unsigned int                 flags
        );

        virtual ds::ErrorType CDECL RecvMsg
        (
          SockAddrStorageType    remoteAddrPtr,
          IPort1::SeqBytes *     ioVecArr,
          int                    numIOVec,
          int *                  numReadPtr,
          IAncDataPriv **        outAncillaryDataPtrPtr,
          int                    outAncillaryDataLen,
          int *                  outAncillaryDataLenReqPtr,
          unsigned int *         outFlagsPtr,
          unsigned int           flags
        );

        virtual ds::ErrorType CDECL RegEvent
        (
          ISignal *  signalObjPtr,
          SocketEventType  event,
          IQI**            regObj
        );

        virtual ds::ErrorType CDECL Shutdown
        (
          ShutdownDirType  shutdownDir
        );

        virtual ds::ErrorType CDECL SetOpt
        (
          OptLevelType  optLevel,
          OptNameType   optName,
          int           optVal
        );

        virtual ds::ErrorType CDECL GetOpt
        (
          OptLevelType  optLevel,
          OptNameType   optName,
          int *         optValPtr
        );

        /**
          @brief Updates network policy.

          Updates socket's network policy.
          //TODO Copy all info from IsSetNetPolicySupported

          @param[in] netPolicyPtr  Ptr to IPolicy object

          @retval AEE_SUCCESS         Network policy is updated
                                             successfully
          @retval QDS_EFAULT      netPolicyPtr is NULL
          @retval QDS_EOPNOTSUPP  Network policy can't be updated
                                             in current state

          @see ds::Net::IPolicy, ds::Net::INetwork, ds::Error
        */
        virtual ds::ErrorType CDECL SetNetPolicy
        (
          ds::Net::IPolicy *  netPolicyPtr
        );

        /**
          @brief Associates socket with a network object.

          Associates socket with the passed in network object. From this
          point, socket must use the network interface pointed to by INetwork
          object in data path. Socket can never use any other interface.

          It is not allowed to perform this operation if
            1. SetNetPolicy() was called before OR
            2. SetNetwork() was called before OR
            3. Connect(), Listen(), or Close() were called
            //TODO Case 3 is not handled

          @param[in] netObjPtr  Ptr to INetwork object

          @retval AEE_SUCCESS         Socket is associated with
                                             INetwork object successfully
          @retval QDS_EFAULT      netObjPtr is NULL
          @retval QDS_EOPNOTSUPP  Socket can't be associated with
                                             INetwork object in current state
          @retval QDS_EINVAL      IPolicy object couldn't be
                                             fetched from INetwork object

          @see ds::Net::INetwork, ds::Net::IPolicy, ds::Error
        */
        virtual ds::ErrorType SetNetwork
        (
          ds::Net::INetwork *  netObjPtr
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

          //TODO Talk about linger reset
          @param None

          @retval AEE_SUCCESS                 Socket is closed successfully
          @retval AEE_EWOULDBLOCK             Socket needs to be closed
                                              asynchronously
          @retval AEE_ENOMEMORY               Out of resources

          @see ds::Sock::Event, ds::Sock::Socket::RegEvent, ds::Error
        */
        virtual ds::ErrorType CDECL Close
        (
          void
        ) throw();

        virtual ds::ErrorType CDECL AddIPMembership
        (
          const IPMembershipInfoType *  ipMembershipPtr
        );

        virtual ds::ErrorType CDECL DropIPMembership
        (
          const IPMembershipInfoType *  ipMembershipPtr
        );

        virtual ds::ErrorType CDECL GetSOLingerReset
        (
          LingerType *  soLingerPtr
        );

        virtual ds::ErrorType CDECL SetSOLingerReset
        (
          const LingerType *  soLingerPtr
        );

        virtual ds::ErrorType CDECL GetDoSAckInfo
        (
          ds::Sock::DoSAckStatusType *  dosAckStatusPtr,
          int                        *  overflowPtr
        );

        /*-------------------------------------------------------------------
          Platform::IEventListener methods
        -------------------------------------------------------------------*/
        virtual void SetEvent
        (
          PS::Sock::Platform::EventType  event
        ) throw();

        virtual void ClearEvent
        (
          PS::Sock::Platform::EventType  event
        ) throw();

        /*-------------------------------------------------------------------
          ds::Utils::INode methods
        -------------------------------------------------------------------*/
        virtual boolean Process
        (
          void *  userDataPtr
        );

        /*-------------------------------------------------------------------
          ISocketLocalPriv methods
        -------------------------------------------------------------------*/
        virtual ds::ErrorType CDECL GetSystemOption
        (
          boolean *  isSystemSocketPtr
        );

        virtual ds::ErrorType CDECL SetSystemOption
        (
          boolean  isSystemSocket
        );

        virtual ds::ErrorType CDECL GetOptPriv
        (
          OptLevelType     optLevel,
          OptNameTypePriv  optName,
          int *            optValPtr
        );

        virtual ds::ErrorType CDECL SetOptPriv
        (
          OptLevelType     optLevel,
          OptNameTypePriv  optName,
          int              optValPtr
        );

        /*-------------------------------------------------------------------
          IDSMUtils methods
        -------------------------------------------------------------------*/
        virtual ds::ErrorType CDECL ReadDSMChain
        (
          dsm_item_type **  dsmItemPtrPtr,
          int32 *           numBytesReadPtr
        );

        virtual ds::ErrorType CDECL WriteDSMChain
        (
          dsm_item_type **  dsmItemPtrPtr,
          int32 *           numBytesWrittenPtr
        );

        virtual ds::ErrorType CDECL SendToDSMChain
        (
          dsm_item_type **                       dsmItemPtrPtr,
          const SockAddrStorageType              remoteAddrPtr,
          unsigned int                           flags,
          int32 *                                numBytesSentPtr
        );

        virtual ds::ErrorType CDECL RecvFromDSMChain
        (
          dsm_item_type **                 dsmItemPtrPtr,
          SockAddrStorageType              remoteAddrPtr,
          unsigned int                     flags,
          int32 *                          numBytesRcvdPtr
        );

        /*-------------------------------------------------------------------
          IQI interface Methods
        -------------------------------------------------------------------*/
        virtual uint32 CDECL AddRef
        (
          void
        ) throw();

        virtual uint32 CDECL Release
        (
          void
        ) throw();

        virtual ds::ErrorType CDECL QueryInterface
        (
          AEEIID   iid,
          void **  objPtrPtr
        ) throw();

        /*-------------------------------------------------------------------
          IWeakRef methods
        -------------------------------------------------------------------*/
        virtual uint32 AddRefWeak
        (
          void
        ) throw();

        virtual uint32 ReleaseWeak
        (
          void
        ) throw();

        virtual boolean GetStrongRef
        (
          void
        ) throw();

      protected:
        /**
          @brief Constructor for Socket class.

          Initializes a Socket object.
        */
        Socket
        (
          void
        );

        /**
          @brief Destructor for Socket class.

          Resets the socket object. Method is defined as virtual
          so that destructors of derived classes are called when Socket
          object is deleted.
        */
        virtual void Destructor
        (
          void
        ) throw();

        /**
           @brief This destructor is not used.

           In Weak Ref-using classes, function Destructor() does the
           cleanup, and the real destructor is a no-op.
        */
        virtual ~Socket() throw() { /* NO-OP */ };

        /**
          @brief Initializes a socket object.

          Initializes the rest of the socket object. These operations could
          fail and since constructor can't fail, these operations can't be
          performed as part of constructor.

          This method performs following operations
            @li Creates a platform socket
            @li Registers for PLATFORM_ENABLED and PLATFORM_DISABLED events
                with platform socket

          Socket object is deleted if any of the above operations fail.

          @param[in] family    The family of socket to be created
          @param[in] sockType  The type of socket to be created
          @param[in] protocol  The protocol to be used

          @retval AEE_SUCCESS  Socket is initialized successfully
          @retval EMFILE   Out of memory

          @see ds::Sock::Family, ds::Sock::Type, ds::Sock::Protocol, ds::Error
        */
        virtual ds::ErrorType Init
        (
          AddrFamilyType  family,
          SocketType     sockType,
          ProtocolType    protocol,
          NetworkCtl      *networkCtlPtr
        );

        /**
          @brief Copies information from a socket object.

          Copies members from passed in socket object. This method is used
          after a new TCP socket is accepted.

          @param[in] sockPtr  Socket object from which members are to be copied

          @retval None
        */
        ds::ErrorType CloneSocket
        (
          Socket *  sockPtr
        );

        virtual bool IsConnectSupported
        (
          const SockAddrIN6InternalType *      v6RemoteAddr,
          ds::ErrorType *              dsErrnoPtr
        ) = 0;

        virtual bool IsOptSupported
        (
          OptLevelType  optLevel,
          OptNameType   optName
        ) = 0;

        virtual bool IsOptPrivSupported
        (
          OptLevelType    optLevel,
          OptNameTypePriv optName
        );

        /**
          @brief Checks if socket object's NetPolicy can be updated

          Socket's policy can't be updated if
            1. Socket is created via SocketFactory::CreateSocketByNetwork() OR
            2. Socket is created via Accept() OR
            3. Connect() had been called OR
            4. Listen() had been called OR
            5. Close() had been called

          In case 1, socket object's netObjPtr is non-0. As, socket must use
          the interface pointed to by INetwork object in this case, it is
          invalid to update policy

          In cases 2 & 3, socket is connected to a peer and updating policy
          make make this peer unreachable.
          //TODO Why case 4?

          @param None

          @retval true   If policy can be updated
          @retval false  If policy can't be updated

          @see ds::Net::IPolicy
        */
        virtual bool IsSetNetPolicySupported
        (
          void
        );

        /**
          @brief Checks if Multicast is supported.

          Multicast is supported only by UDP. So this method returns if a
          socket object supports Multicast.

          @param None

          @retval true   If Multicast is supported
          @retval false  If Multicast is not supported
        */
        virtual bool IsMulticastSupported
        (
          void
        );

        virtual ds::ErrorType FillProtocolInfoInPktInfo
        (
          const SockAddrIN6InternalType *  v6RemoteAddrPtr,
          const SockAddrIN6InternalType *  v6LocalAddrPtr,
          ip_pkt_info_type *       pktInfoPtr
        ) = 0;

        /**
          @brief Sets an event bit mask for the given event.

          Sets an event bit mask for the given event. After setting the event
          bit mask, posts event if
            1. Any of the transient error bits are set OR
            2. None of the event error bits are set

          @param[in] event    Event whose bit mask need to be updated
          @param[in] bitMask  bit mask to set

          @retval None

          @see ds::Sock::Event
        */
        void SetEventBitMask
        (
          SocketEventType  event,
          uint32     bitMask
        ) throw();

        /**
          @brief Resets an event bit mask for the given event.

          Resets an event bit mask for the given event. After resetting the
          event bit mask, posts event if
            1. Any of the transient error bits are set OR
            2. None of the event error bits are set

          @param[in] event    Event whose bit mask need to be updated
          @param[in] bitMask  bit mask to reset

          @retval None

          @see ds::Sock::Event
        */
        void ResetEventBitMask
        (
          SocketEventType  event,
          uint32     bitMask
        ) throw();

        virtual bool IsPktInfoDifferent
        (
          const SockAddrInternalType*   remoteAddrPtr
        );

        bool IsFlowEnabled
        (
          void
        );
        
        bool IsFlowDoSEnabled
        (
          void
        );

        virtual ds::ErrorType HandleNonZeroWriteEventBitMask
        (
          const SockAddrInternalType*   remoteAddrPtr,
          boolean *                     isReorig,
          byte *                        data_buf = NULL,
          int                           data_len = 0,
          bool                          isDoS = false
        );

        ds::ErrorType GeneratePktMetaInfo
        (
          ps_pkt_meta_info_type **  pktMetaInfoPtr,
          uint32                    flags
        );

        ds::ErrorType UpdateSrcAddrInPktInfo
        (
          const SockAddrIN6InternalType * v6RemoteAddrPtr,
          ps_rt_meta_info_type *          rtMetaInfoPtr
        );

        virtual void ProcessNetworkConfigChangedEvent
        (
          ds::ErrorType  reasonForChange
        ) = 0;

        virtual void ProcessDoSAckEvent
        (
          ds::Sock::Event::DoSAckEventInfo *  dosEventInfoPtr
        );

        /**
          @brief Maps the socket event to the appropriate index in the
                 signalBus array.

          The SignalBuses and event bit masks are stored in arrays according
          to the event types. Thus when receiving event a transition to the
          appropriate index should be done using this function.

          @param[in]  event          socket event type
          @param[out] eventIndexPtr  The index returned is used to access
                                     arrays in the socket that hold SignalBuses
                                     for an appropriate event type



          @retval Success    If the event is with legal type and eventIndexPtr
                             holds the correct index is the array
          @retval QDS_EFAULT If event is not a legal event type

          @see ds::Sock::Event
        */
        ds::ErrorType GetIndexFromEvent
        (
          ds::Sock::SocketEventType event,
          uint32*                   eventIndexPtr
        ) throw();

        /**
          @brief Resets socket's routing cache

          Resets socket's routing cache to NULL and also resets all the event
          bit masks associated with routing so that stale information is not
          maintained on receiving events such as IFACE_DOWN_EV. For 
          IFACE_IPFLTR_UPDATED_EV meta info is not freed

          This function must be called whenever
          WRITE_BIT_MASK_NULL_ROUTING_INFO bit mask needs to be set.

          @param[in] bool TRUE if RT Meta Info needs to be freed

          @retval None

          @see DS::Sock::Event::WriteBitMaskEnumType
        */
        void ResetRoutingCache
        (
          bool freeRtMetaInfo
        );

        /**
          @brief Check for event bit mask and set/clear the signal bus.

          The SignalBus for the event is SET if
            1. Any of the transient error bits are set OR
            2. None of the event error bits are set

          It is CLEARed otherwise.

          @param[in] event    Event whose bit mask need to be checked.

          @retval None

          @see ds::Sock::Event
        */
        void SetOrClearEvent
        (
          ds::Sock::SocketEventType  event
        )
        throw();

        ICritSect *                    critSectPtr;
        PS::Sock::Platform::ISocket *  platformSockPtr;
        SocketEvent::BitMaskType       eventBitMask[SocketEvent::NUM_EV];
        ps_rt_meta_info_type *         rtMetaInfoPtr;
        bool                           sendPktMetaInfo;
        NetworkCtl                     *networkCtlPtr;


      private:
        virtual ds::ErrorType RoutePacket
        (
          const SockAddrInternalType*  remoteAddrPtr,
          boolean *                    isReorig,
          byte *                       data_buf = NULL,
          int                          len = 0,
          bool                         isDoS = false
        );


        virtual ds::ErrorType AllocateMemberObjects
        (
          void
        );

        virtual ds::ErrorType GeneratePktInfo
        (
          const SockAddrIN6InternalType *        v6RemoteAddrPtr,
          ip_pkt_info_type *         pktInfoPtr
        );

        ds::ErrorType RegNetworkStateChangedEvent
        (
          void
        );

        /**
          @brief Called when IFACE_UP event occurs
          Static.

          @param[in] userDataPtr data passed on Signal registration

          @retval None

        */
        static void EventCallbackIfaceUp
        (
          SignalHandlerBase *  pSignalHandlerBaseParam
        );

        /**
          @brief Called when IFACE_DOWN event occurs
          Static.

          @param[in] userDataPtr data passed on Signal registration

          @retval None

        */
        static void EventCallbackIfaceDown
        (
          SignalHandlerBase *  pSignalHandlerBaseParam
        );

        /**
          @brief Releases network state change signal controlers.

          Detaches and releases non-NULL signal controlers.

          @param None

          @retval None

        */
        void DetachNetworkStateChangeSignalCtls
        (
          void
        );

        void UpdateRouteScopeByPolicy
        (
          void
        );

        void DeleteRoutingCacheFromRouteScope
        (
          int32  routingCache
        );

        ds::ErrorType UpdateRtMetaInfoWithFlowFwding
        (
          void
        );

        ds::ErrorType UseOrigFltrResult
        (
          void
        );

        void UseDefaultPSFlow
        (
          bool isFlowNotActivated
        );

        ds::AddrFamilyType            family;
        ISignalCtl *                  ifaceUpSignalCtlPtr;
        ISignalCtl *                  ifaceDownSignalCtlPtr;

        /*-------------------------------------------------------------------
          Since many Socket signal buses will never be used by clients, use
          OnDemandSignalBus throughout to conserve resources
        -------------------------------------------------------------------*/
        ds::Utils::OnDemandSignalBus  eventSignalBuses[SocketEvent::NUM_EV];
        int32                         origFltrResult;
        bool                          isSystemSocket;

        /**
          This class implements the IQI interface and used
          to create regObj in RegEvent function.
        */
        class RegBound : public IQI
        {
          public:
            RegBound():refCnt(1){};
            virtual ~RegBound() throw(){};
            void * operator new
            (
              unsigned int numBytes
            )  throw()
            {
               void * buf = NULL;
               PS_SYSTEM_HEAP_MEM_ALLOC(buf, sizeof (RegBound), void*);
               return buf; 
            }

            void operator delete
            (
              void *  bufPtr
            )
            throw()
            {
              if(NULL != bufPtr)
              {
                PS_SYSTEM_HEAP_MEM_FREE (bufPtr);
                return;
              }
            }
           DSIQI_DECL_LOCALS()
           DSIQI_ADDREF()
           DSIQI_RELEASE()
           virtual int CDECL QueryInterface (AEEIID iid, void **ppo){
              switch (iid)
              {
                 case AEEIID_IQI:
                    *ppo = reinterpret_cast <void *> (this);
                    (void) AddRef ();
                    break;

                 default:
                    return AEE_ECLASSNOTSUPPORT;
              }

              return AEE_SUCCESS;
           }
        };
    };
  }
}

#endif /* DS_SOCK_SOCKET_H */

