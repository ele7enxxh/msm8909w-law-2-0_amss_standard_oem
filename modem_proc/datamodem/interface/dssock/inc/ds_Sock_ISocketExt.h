#ifndef DS_SOCK_ISOCKETEXT_H
#define DS_SOCK_ISOCKETEXT_H

/*============================================================================
  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/

#include "ds_Utils_Interface.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_ISignal.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
#include "ds_Net_IPolicy.h"

namespace ds
{
   namespace Sock
   {
      
      /**
        * This module includes socket events
        */
      typedef int SocketEventType;
      namespace SocketEvent
      {
         
         /**
           * Socket events to be used with RegEvent API.
           * The values do not imply any specific meaning.
           */
         const ::ds::Sock::SocketEventType QDS_EV_WRITE = 0x1;
         const ::ds::Sock::SocketEventType QDS_EV_READ = 0x2;
         const ::ds::Sock::SocketEventType QDS_EV_CLOSE = 0x4;
         const ::ds::Sock::SocketEventType QDS_EV_ACCEPT = 0x8;
         const ::ds::Sock::SocketEventType QDS_EV_DOS_ACK = 0x8000;
      };
      struct IPMembershipInfoType {
         ::ds::Net::IfaceIdType ifaceId;
         ::ds::IPAddrType mcastGroup; /**< IP address of the Multicast group. */
         AEEINTERFACE_PADMEMBERS(__pad, 2)
      };
      typedef int DoSAckStatusType;
      namespace DoSAckStatus
      {
         
         /**
           * Status is not available.
           */
         const ::ds::Sock::DoSAckStatusType DOSACK_NONE = -1;
         
         /**
           * Packet is sent successfully.
           */
         const ::ds::Sock::DoSAckStatusType DOSACK_OK = 0;
         
         /**
           * Hold orig timer expired and hence failed to send the packet.
           */
         const ::ds::Sock::DoSAckStatusType HOLD_ORIG_RETRY_TIMEOUT = 1;
         
         /**
           * Unable to process the packet because hold orig is true.
           */
         const ::ds::Sock::DoSAckStatusType HOLD_ORIG = 2;
         
         /**
           * Failed to send the packet due to lack of service.
           */
         const ::ds::Sock::DoSAckStatusType DOSACK_NO_SRV = 3;
         
         /**
           * Aborted the operation.
           */
         const ::ds::Sock::DoSAckStatusType DOSACK_ABORT = 4;
         
         /**
           * DOS is not supported in analog mode.
           */
         const ::ds::Sock::DoSAckStatusType NOT_ALLOWED_IN_AMPS = 5;
         
         /**
           * DOS is not supported when in a HDR call.
           */
         const ::ds::Sock::DoSAckStatusType NOT_ALLOWED_IN_HDR = 6;
         
         /**
           * Failed to receive Layer 2 ACK.
           */
         const ::ds::Sock::DoSAckStatusType L2_ACK_FAILURE = 7;
         
         /**
           * Unable to process the packet because of lack of resources.
           */
         const ::ds::Sock::DoSAckStatusType OUT_OF_RESOURCES = 8;
         
         /**
           * Packet is too big to be sent over access channel.
           */
         const ::ds::Sock::DoSAckStatusType ACCESS_TOO_LARGE = 9;
         
         /**
           * Packet is too big to be sent over DTC.
           */
         const ::ds::Sock::DoSAckStatusType DTC_TOO_LARGE = 10;
         
         /**
           * Access channel is blocked for traffic based on service option.
           */
         const ::ds::Sock::DoSAckStatusType ACCT_BLOCK = 11;
         
         /**
           * Failed to receive Layer 3 ACK.
           */
         const ::ds::Sock::DoSAckStatusType L3_ACK_FAILURE = 12;
         
         /**
           * Failed for some other reason.
           */
         const ::ds::Sock::DoSAckStatusType DOSACK_OTHER = 13;
      };
      const ::AEEIID AEEIID_ISocketExt = 0x1098bc6;
      
      /** @interface ISocketExt
        * 
        * The ISocketExt interface provides extended socket functionality.
        */
      struct ISocketExt : public ::IQI
      {
         
         /**
           * This function registers a signal to be set when event eventID occurs.
           * Cannot use the signal for more than one eventID. Can use more than one
           * signal for the same event.
           * @param signal The signal to Set() when the state associated with
           *               the eventID changes. To cancel the registration the
           *               application should release this signal. 
           * @param eventID The event for which the signal shall be fired.
           * @param regObj Output The application must hold this output registration
           *                      object to ensure its Signal stays registered. The
           *                      application should release this object once it has
           *                      released the signal object.
           * @retval AEE_SUCCESS Signal set completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL RegEvent(::ISignal* signal, ::ds::Sock::SocketEventType eventID, ::IQI** regObj) = 0;
         
         /**
           * Join a multicast group.
           * @param ipMembership Information of the Multicast group to join and
           *                     an iface identification. If supported, 
           *                     applications can join the same Multicast group
           *                     on different ifaces.
           *                     If INVALID_IFACE_ID is specified for ifaceId an
           *                     appropriate iface would be selected for the
           *                     operation or an applicable error returned if 
           *                     appropriate iface cannot be found. 
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL AddIPMembership(const ::ds::Sock::IPMembershipInfoType* ipMembership) = 0;
         
         /**
           * Leave a multicast group.
           * @param ipMembership Information of the multicast group to leave.
           *                     @See AddIPMembership
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL DropIPMembership(const ::ds::Sock::IPMembershipInfoType* ipMembership) = 0;
         
         /**
           * Get DoS (Data Over Signaling) Ack information.
           * DoS and SDB (Short Data Burst) are parallel terms.
           * DoS is the term used in 1xEVDO
           * SDB is the term used in CDMA 1x.      
           * @param dosAckInfo Information of DoS Ack.
           * @param overflow Set to a nonzero value, if the number of outstanding
           *                 SDB/DOS packets (the packets for which mobile is
           *                 still waiting for an ACK) is more than that the
           *                 mobile can handle.
           * @retval AEE_SUCCESS The operation completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Sock_DoSAckStatusType
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL GetDoSAckInfo(::ds::Sock::DoSAckStatusType* dosAckInfo, int* overflow) = 0;
         
         /**
           * This function sets the network policy to be used with the socket. For TCP sockets,
           * policy change is not supported after listen or connect have been issued on the socket.
           * For UDP sockets, policy change is supported as long as the socket is not connected.
           * @param policy Network Policy (specifies technology, profile etc.). See IPolicy.
           * @retval AEE_SUCCESS Network Selection completed successfully.
           * @retval Other ds designated error codes might be returned.
           * @see ds_Errors_Def.idl.
           */
         virtual ::AEEResult AEEINTERFACE_CDECL SetNetPolicy(::ds::Net::IPolicy* policy) = 0;
      };
   };
};
#endif //DS_SOCK_ISOCKETEXT_H
