#ifndef DS_SOCK_UDPSOCKET_H
#define DS_SOCK_UDPSOCKET_H
/*===========================================================================
  @file ds_Sock_UDPSocket.h

  This file defines the class which implements the ISocket interface for UDP
  sockets.

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_UDPSocket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ds_Sock_Socket.h"
#include "ps_iface_defs.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Sock_NetworkCtl.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class UDPSocket : public Socket                           /*lint !e578 */
    {
      public:
        virtual ~UDPSocket() throw() {}

        static UDPSocket * CreateInstance
        (
          AddrFamilyType    family,
          NetworkCtl        *pNetworkCtl
        );

        virtual ds::ErrorType CDECL SendToDSMChain
        (
          dsm_item_type **                       dsmItemPtrPtr,
          const ds::SockAddrStorageType          remoteAddrPtr,
          unsigned int                           flags,
          int32 *                                numBytesSentPtr
        );

        virtual ds::ErrorType CDECL RecvFromDSMChain
        (
          dsm_item_type **                 dsmItemPtrPtr,
          ds::SockAddrStorageType          remoteAddrPtr,
          unsigned int                     flags,
          int32 *                          numBytesRcvdPtr
        );

        virtual ds::ErrorType CDECL AddIPMembership
        (
          const IPMembershipInfoType *  ipMembershipPtr
        );

        virtual ds::ErrorType CDECL DropIPMembership
        (
          const IPMembershipInfoType *  ipMembershipPtr
        );

        virtual ds::ErrorType CDECL GetDoSAckInfo
        (
          DoSAckStatusType *  dosAckStatusPtr,
          int              *  overflowPtr
        );
      protected:
        virtual bool IsOptSupported
        (
          OptLevelType  optLevel,
          OptNameType   optName
        );

        virtual bool IsMulticastSupported
        (
          void
        );

        virtual bool IsConnectSupported
        (
          const SockAddrIN6InternalType *  v6RemoteAddrPtr,
          ds::ErrorType *          dsErrnoPtr
        );

        virtual bool IsPktInfoDifferent
        (
          const SockAddrInternalType*   remoteAddrPtr
        );

        virtual ds::ErrorType FillProtocolInfoInPktInfo
        (
          const SockAddrIN6InternalType *  v6RemoteAddrPtr,
          const SockAddrIN6InternalType *  v6LocalAddrPtr,
          ip_pkt_info_type *       pktInfoPtr
        );

        virtual void ProcessNetworkConfigChangedEvent
        (
          ds::ErrorType  reasonForChange
        );

        virtual void ProcessDoSAckEvent
        (
          ds::Sock::Event::DoSAckEventInfo *  dosEventInfoPtr
        );

      private:
        static void * operator new
        (
          unsigned int numBytes
        ) throw();

        UDPSocket
        (
          void
        ) throw();

        ds::Sock::DoSAckStatusType  dosAckStatus;
        uint32                      overflow;
#ifdef FEATUTE_DATA_PS_MCAST
        ps_iface_mcast_handle_type  mcastHandle;
#endif // FEATUTE_DATA_PS_MCAST
    }; /* class UDPSocket */
  } /* namespace Sock */
} /* namespace ds */


#endif /* DS_SOCK_UDPSOCKET_H */
