#ifndef DS_SOCK_TCPSOCKET_H
#define DS_SOCK_TCPSOCKET_H
/*===========================================================================
  @file ds_Sock_TCPSocket.h

  This file defines the class which implements the ISocket interface for TCP
  sockets.

  Copyright (c) 2008 - 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/src/ds_Sock_TCPSocket.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

extern "C" {
#include "pstimer.h"
}
#include "ds_Sock_Socket.h"
#include "ds_Utils_CSSupport.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    class TCPSocket : public Socket                           /*lint !e578 */
    {
      public:
        TCPSocket();
        virtual void Destructor(
          void
        ) throw();

        static TCPSocket * CreateInstance
        (
          AddrFamilyType    family,
          NetworkCtl        *pNetworkCtl
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

        virtual ds::ErrorType CDECL WriteDSMChain
        (
          dsm_item_type **  dsmItemPtrPtr,
          int32 *           numBytesWrittenPtr
        );

        virtual ds::ErrorType CDECL ReadDSMChain
        (
          dsm_item_type **  dsmItemPtrPtr,
          int32 *           numBytesReadPtr
        );

      protected:
        virtual bool IsOptSupported
        (
          OptLevelType  optLevel,
          OptNameType   optName
        );

        virtual bool IsConnectSupported
        (
          const SockAddrIN6InternalType *  v6RemoteAddrPtr,
          ds::ErrorType *          dsErrnoPtr
        );

        virtual bool IsPktInfoDifferent
        (
          const SockAddrInternalType*  remoteAddrPtr
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

      private:
        static void * operator new
        (
          unsigned int numBytes
        ) throw();

    }; /* class TCPSocket */
  } /* namespace Sock */
} /* namespace ds */


#endif /* DS_SOCK_TCPSOCKET_H */
