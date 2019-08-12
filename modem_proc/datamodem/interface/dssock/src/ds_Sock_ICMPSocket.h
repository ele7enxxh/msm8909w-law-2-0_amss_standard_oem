#ifndef DS_SOCK_ICMPSOCKET_H
#define DS_SOCK_ICMPSOCKET_H
/*===========================================================================
  @file ds_Sock_ICMPSocket.h

  This file defines the class which implements the ISocket interface for ICMP
  sockets.

                Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_ICMPSocket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Sock_Socket.h"
#include "ds_Sock_NetworkCtl.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class ICMPSocket : public Socket                          /*lint !e578 */
    {
      public:
        virtual ~ICMPSocket() throw() {}

        static ICMPSocket * CreateInstance
        (
          AddrFamilyType     _family,
          NetworkCtl         *pNetworkCtl
        );

        virtual ds::ErrorType CDECL Shutdown
        (
          ShutdownDirType  shutdownDir
        );

      protected:
        virtual bool IsConnectSupported
        (
          const SockAddrIN6InternalType *  v6RemoteAddrPtr,
          ds::ErrorType *          dsErrnoPtr
        );

        virtual bool IsOptSupported
        (
          OptLevelType  optLevel,
          OptNameType   optName
        );

        virtual bool IsOptPrivSupported
        (
          OptLevelType    optLevel,
          OptNameTypePriv optName
        );

        virtual bool IsSetNetPolicySupported
        (
          void
        );

        virtual ds::ErrorType FillProtocolInfoInPktInfo
        (
          const SockAddrIN6InternalType *      v6RemoteAddrPtr,
          const SockAddrIN6InternalType *      v6LocalAddrPtr,
          ip_pkt_info_type *       pktInfoPtr
        );

        virtual void ProcessNetworkConfigChangedEvent
        (
          ds::ErrorType  reasonForChange
        );

        virtual ds::ErrorType CDECL GetDoSAckInfo
        (
          ds::Sock::DoSAckStatusType *  dosAckStatusPtr,
          int                        *  overflowPtr
        )
        {
           // TODO implement
           return 0;
        }

      private:
        static void * operator new
        (
          unsigned int numBytes
        ) throw();

    }; /* class ICMPSocket */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_ICMPSOCKET_H */
