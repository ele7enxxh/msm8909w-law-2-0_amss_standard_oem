#ifndef DS_SOCK_DEF_H
#define DS_SOCK_DEF_H

/*============================================================================
  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
typedef int ds_Sock_SocketType;
#define ds_Sock_Type_QDS_UNSPEC 0
#define ds_Sock_Type_QDS_STREAM 1
#define ds_Sock_Type_QDS_DGRAM 2

/**
  * Socket Protocol Values.
  */
typedef int ds_Sock_ProtocolType;
#define ds_Sock_Protocol_QDS_UNSPEC 0
#define ds_Sock_Protocol_QDS_TCP 6
#define ds_Sock_Protocol_QDS_UDP 17

/**
  * consts of shutdown type for the Shutdown() method.
  * @see Shutdown()
  */
typedef int ds_Sock_ShutdownDirType;
#define ds_Sock_ShutdownDir_QDS_READ_DIR 0
#define ds_Sock_ShutdownDir_QDS_WRITE_DIR 1
#define ds_Sock_ShutdownDir_QDS_READ_WRITE_DIR 2

/**
  * This module includes constant definitions for flags used by IDSSock's methods.
  * See each method's definition for specific usage instructions.
  */
typedef unsigned int ds_Sock_FlagType;
#define ds_Sock_SendFlags_QDS_MSG_EXPEDITE 0x1
#define ds_Sock_SendFlags_QDS_MSG_FAST_EXPEDITE 0x2
#define ds_Sock_SendFlags_QDS_MSG_SOCK_WAKEUP 0x4

/** @memberof ds_Sock_RecvFlags
  * 
  * Flag name: QDS_MSG_CTRUNC
  * Description: Indicates amount of incoming ancillary data that is 
  *              larger than buffer supplied by the application. The 
  *              part of ancillary data that fits into the buffer is 
  *              provided to the application. The rest of the ancillary
  *              data cannot be fetched.
  */
#define ds_Sock_RecvFlags_QDS_MSG_CTRUNC 0x8

/** @memberof ds_Sock_RecvFlags
  * 
  * Flag name: QDS_MSG_TRUNC
  * Description: Indicates amount of incoming data that is larger than
  *              buffer supplied by the application. The part of data 
  *              that fits into the buffer is provided to the 
  *              application. The rest of the data cannot be fetched.
  */
#define ds_Sock_RecvFlags_QDS_MSG_TRUNC 0x20

/** @memberof ds_Sock_RecvFlags
  * 
  * Flag name: QDS_MSG_ERRQUEUE
  * Description: Indicates that application wants to ream ICMP error
  *              ancillary data.
  */
#define ds_Sock_RecvFlags_QDS_MSG_ERRQUEUE 0x2000
typedef int ds_Sock_OptLevelType;

/** @memberof ds_Sock_OptLevel
  * 
  * Option level: QDS_LEVEL_IP
  * Level: IP
  * Description: used in socket options (GetOpt / SetOpt) and for 
  * ancillary data.
  */
#define ds_Sock_OptLevel_QDS_LEVEL_IP 1

/** @memberof ds_Sock_OptLevel
  * 
  * Option level: QDS_LEVEL_SOCKET
  * Level: Socket
  * Description: used in socket options (GetOpt / SetOpt) and for 
  * ancillary data.
  */
#define ds_Sock_OptLevel_QDS_LEVEL_SOCKET 2

/** @memberof ds_Sock_OptLevel
  * 
  * Option level: QDS_LEVEL_TCP
  * Level: TCP
  * Description: used in socket options (GetOpt / SetOpt) and for 
  * ancillary data.
  */
#define ds_Sock_OptLevel_QDS_LEVEL_TCP 3

/** @memberof ds_Sock_OptLevel
  * 
  * Option level: QDS_LEVEL_IPV6
  * Level: IPV6
  * Description: used in socket options (GetOpt / SetOpt) and for 
  * ancillary data.
  */
#define ds_Sock_OptLevel_QDS_LEVEL_IPV6 4

/** @memberof ds_Sock_OptLevel
  * 
  * Option level: QDS_LEVEL_UDP
  * Level: UDP
  * Description: used in socket options (GetOpt / SetOpt) and for 
  * ancillary data.
  */
#define ds_Sock_OptLevel_QDS_LEVEL_UDP 7
typedef int ds_Sock_OptNameType;

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_KEEPALIVE
  * Level: Socket
  * Value type: Boolean
  * Description: Enables periodic probes on idle TCP connection.
  */
#define ds_Sock_OptName_QDS_SO_KEEPALIVE 0

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_REUSEADDR
  * Level: Socket
  * Value type: Boolean
  * Description: Enable socket port reuse.
  */
#define ds_Sock_OptName_QDS_SO_REUSEADDR 1

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_ERROR_ENABLE
  * Level: Socket
  * Value type: Boolean
  * Description: Enable retrieving of ICMP errors using SO_ERROR.
  */
#define ds_Sock_OptName_QDS_SO_ERROR_ENABLE 2

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_TX_IFACE
  * Level: Socket
  * Value type: unsigned long
  * Description: Get tx iface id.
  */
#define ds_Sock_OptName_QDS_SO_TX_IFACE 3

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_RCVBUF
  * Level: Socket
  * Value type: long
  * Description: Change the receive buffer size for the socket.
  */
#define ds_Sock_OptName_QDS_SO_RCVBUF 4

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_SNDBUF
  * Level: Socket
  * Value type: long
  * Description: Change the send buffer size for the socket.
  */
#define ds_Sock_OptName_QDS_SO_SNDBUF 5

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_ERROR
  * Level: Socket
  * Value type: long
  * Description: Get ICMP error on the socket.
  */
#define ds_Sock_OptName_QDS_SO_ERROR 6

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_NODELAY
  * Level: TCP
  * Value type: Boolean
  * Description: Disable Nagle's algorithm.
  */
#define ds_Sock_OptName_QDS_TCP_NODELAY 7

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_DELAYED_ACK
  * Level: TCP
  * Value type: Boolean
  * Description: Reduce the number of ACKs sent to half.
  */
#define ds_Sock_OptName_QDS_TCP_DELAYED_ACK 8

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_SACK
  * Level: TCP
  * Value type: Boolean
  * Description: Enables TCP selective ACK option.
  */
#define ds_Sock_OptName_QDS_TCP_SACK 9

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_TIMESTAMP
  * Level: TCP
  * Value type: Boolean
  * Description: Enables TCP time stamp option.
  */
#define ds_Sock_OptName_QDS_TCP_TIMESTAMP 10

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_FIONREAD
  * Level: TCP
  * Value type: unsigned long
  * Description: Retrieve TCP queue size.
  */
#define ds_Sock_OptName_QDS_TCP_FIONREAD 11

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_MAXSEG
  * Level: TCP
  * Value type: long
  * Description: Change the TCP maximum segment size.
  */
#define ds_Sock_OptName_QDS_TCP_MAXSEG 12

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IP_RECVIF
  * Level: IP
  * Value type: Boolean
  * Description: Enable retrieving of Rx iface of packet.
  */
#define ds_Sock_OptName_QDS_IP_RECVIF 13

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IP_RECVERR
  * Level: IP
  * Value type: Boolean
  * Description: Enable retrieving of extended ICMP error info.
  */
#define ds_Sock_OptName_QDS_IP_RECVERR 14

/** @memberof ds_Sock_OptName
  * 
  * Option name: IP_TOS
  * Level: IP
  * Value type: long
  * Description: Set TOS for outgoing IP packets.
  */
#define ds_Sock_OptName_QDS_IP_TOS 15

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IP_TTL
  * Level: IP
  * Value type: long
  * Description: Change the Time To Live value.
  */
#define ds_Sock_OptName_QDS_IP_TTL 16

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IP_MULTICAST_TTL
  * Level: IP
  * Value type: octet (larger values supplied to SetOpt API shall be
  *             truncated to 8 bit value).
  * Description: Specifies the time-to-live value for Multicast
  *              datagrams sent through this socket.
  */
#define ds_Sock_OptName_QDS_IP_MULTICAST_TTL 17

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IPV6_RECVERR
  * Level: IPv6
  * Value type: Boolean
  * Description: Enable retrieving of extended ICMPv6 error info.
  */
#define ds_Sock_OptName_QDS_IPV6_RECVERR 18

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IPV6_TCLASS
  * Level: IPV6
  * Value type: long
  * Description: Set the traffic class for the IPv6 socket.
  */
#define ds_Sock_OptName_QDS_IPV6_TCLASS 19

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_TCP_MAX_BACKOFF_TIME 
  * Level: TCP
  * Value type: unsigned long
  * Description: Maximal TCP backoff time
  */
#define ds_Sock_OptName_QDS_TCP_MAX_BACKOFF_TIME 20

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_IP_MULTICAST_LOOP
  * Level: IP
  * Value type: octet (larger values supplied to SetOpt API shall be
  *             truncated to 8 bit value).
  * Description: Enable/Disable Multicast datagrams to be looped back.
  */
#define ds_Sock_OptName_QDS_IP_MULTICAST_LOOP 21

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_FIONREAD
  * Level: Socket
  * Value type: unsigned long
  * Description: No. of bytes that can be read from a
  *              TCP/UDP/ICMP socket
  *
  */
#define ds_Sock_OptName_QDS_SO_FIONREAD 22

/** @memberof ds_Sock_OptName
  * 
  * Option name: QDS_SO_SKIP_ROUTE_SCOPE_CHECK 
  * Level: Socket 
  * Value type: boolean 
  * Description: TRUE if the check is enabled
  *             otherwise FALSE. Valid for both TCP & UDP
  */
#define ds_Sock_OptName_QDS_SO_SKIP_ROUTE_SCOPE_CHECK 23

/**
  * LingerType. Used with Socket option SO_LINGER
  */
struct ds_Sock_LingerType {
   boolean lingerEnabled; /**< Linger on or off. */
   AEEINTERFACE_PADMEMBERS(timeInSec, 3)
   int timeInSec; /**< Linger time, in seconds. */
};
typedef struct ds_Sock_LingerType ds_Sock_LingerType;

/**
  * Extended error type.
  */
struct ds_Sock_ExtendedErrType {
   unsigned int error_number; /**< Error number */
   unsigned char origin; /**< Where the error originated */
   unsigned char type; /**< Type field from the ICMP header */
   unsigned char code; /**< Code field from the ICMP header */
   AEEINTERFACE_PADMEMBERS(info, 1)
   unsigned int info; /**< Discovered MTU for EMSGSIZE errors */
};
typedef struct ds_Sock_ExtendedErrType ds_Sock_ExtendedErrType;

/**
  * Extended error info type.
  * This structure represents information embedded in the msg_control 
  * parameter when the IP_RECVERR socket option is set and the 
  * MSG_ERRQUEUE flag is set. 
  * The cmsg header level is set to QDS_LEVEL_IP and the type to 
  * QDS_IP_RECVERR to indicate this payload.
  * See the RecvMsg API documentation for how to extract 
  * this info from msg_control parameter using utility macros.
  * Byte order of the err member is little endian.
  * Byte order of the addr member is network order.
  */
struct ds_Sock_ExtendedErrInfoType {
   ds_Sock_ExtendedErrType err;
   ds_SockAddrStorageType addr;
};
typedef struct ds_Sock_ExtendedErrInfoType ds_Sock_ExtendedErrInfoType;

/**
  * Received Iface info type.
  * This structure represents information embedded in the msg_control 
  * parameter when the IP_RECVIF socket option is set.
  * The cmsg header level is set to QDS_LEVEL_IP and the type to 
  * QDS_IP_RECVIF to indicate this payload.
  * See the RecvMsg API documentation for how to extract 
  * this info from msg_control parameter using utility macros.
  * Byte order of the ifaceId member is little endian.
  * Byte order of the addr member is network order.
  * @See INetwork_IfaceId
  */
struct ds_Sock_RecvIfaceInfoType {
   ds_Net_IfaceIdType IfaceId;
   ds_SockAddrStorageType Addr;
};
typedef struct ds_Sock_RecvIfaceInfoType ds_Sock_RecvIfaceInfoType;
#else /* C++ */
#include "ds_Utils_StdDef.h"
#include "ds_Addr_Def.h"
#include "ds_Net_Def.h"
namespace ds
{
   namespace Sock
   {
      typedef int SocketType;
      namespace Type
      {
         const ::ds::Sock::SocketType QDS_UNSPEC = 0;
         const ::ds::Sock::SocketType QDS_STREAM = 1;
         const ::ds::Sock::SocketType QDS_DGRAM = 2;
      };
      
      /**
        * Socket Protocol Values.
        */
      typedef int ProtocolType;
      namespace Protocol
      {
         const ::ds::Sock::ProtocolType QDS_UNSPEC = 0;
         const ::ds::Sock::ProtocolType QDS_TCP = 6;
         const ::ds::Sock::ProtocolType QDS_UDP = 17;
      };
      
      /**
        * consts of shutdown type for the Shutdown() method.
        * @see Shutdown()
        */
      typedef int ShutdownDirType;
      namespace ShutdownDir
      {
         const ::ds::Sock::ShutdownDirType QDS_READ_DIR = 0;
         const ::ds::Sock::ShutdownDirType QDS_WRITE_DIR = 1;
         const ::ds::Sock::ShutdownDirType QDS_READ_WRITE_DIR = 2;
      };
      
      /**
        * This module includes constant definitions for flags used by IDSSock's methods.
        * See each method's definition for specific usage instructions.
        */
      typedef unsigned int FlagType;
      namespace SendFlags
      {
         const ::ds::Sock::FlagType QDS_MSG_EXPEDITE = 0x1;
         const ::ds::Sock::FlagType QDS_MSG_FAST_EXPEDITE = 0x2;
         const ::ds::Sock::FlagType QDS_MSG_SOCK_WAKEUP = 0x4;
      };
      namespace RecvFlags
      {
         
         /**
           * Flag name: QDS_MSG_CTRUNC
           * Description: Indicates amount of incoming ancillary data that is 
           *              larger than buffer supplied by the application. The 
           *              part of ancillary data that fits into the buffer is 
           *              provided to the application. The rest of the ancillary
           *              data cannot be fetched.
           */
         const ::ds::Sock::FlagType QDS_MSG_CTRUNC = 0x8;
         
         /**
           * Flag name: QDS_MSG_TRUNC
           * Description: Indicates amount of incoming data that is larger than
           *              buffer supplied by the application. The part of data 
           *              that fits into the buffer is provided to the 
           *              application. The rest of the data cannot be fetched.
           */
         const ::ds::Sock::FlagType QDS_MSG_TRUNC = 0x20;
         
         /**
           * Flag name: QDS_MSG_ERRQUEUE
           * Description: Indicates that application wants to ream ICMP error
           *              ancillary data.
           */
         const ::ds::Sock::FlagType QDS_MSG_ERRQUEUE = 0x2000;
      };
      typedef int OptLevelType;
      namespace OptLevel
      {
         
         /**
           * Option level: QDS_LEVEL_IP
           * Level: IP
           * Description: used in socket options (GetOpt / SetOpt) and for 
           * ancillary data.
           */
         const ::ds::Sock::OptLevelType QDS_LEVEL_IP = 1;
         
         /**
           * Option level: QDS_LEVEL_SOCKET
           * Level: Socket
           * Description: used in socket options (GetOpt / SetOpt) and for 
           * ancillary data.
           */
         const ::ds::Sock::OptLevelType QDS_LEVEL_SOCKET = 2;
         
         /**
           * Option level: QDS_LEVEL_TCP
           * Level: TCP
           * Description: used in socket options (GetOpt / SetOpt) and for 
           * ancillary data.
           */
         const ::ds::Sock::OptLevelType QDS_LEVEL_TCP = 3;
         
         /**
           * Option level: QDS_LEVEL_IPV6
           * Level: IPV6
           * Description: used in socket options (GetOpt / SetOpt) and for 
           * ancillary data.
           */
         const ::ds::Sock::OptLevelType QDS_LEVEL_IPV6 = 4;
         
         /**
           * Option level: QDS_LEVEL_UDP
           * Level: UDP
           * Description: used in socket options (GetOpt / SetOpt) and for 
           * ancillary data.
           */
         const ::ds::Sock::OptLevelType QDS_LEVEL_UDP = 7;
      };
      typedef int OptNameType;
      namespace OptName
      {
         
         /**
           * Option name: QDS_SO_KEEPALIVE
           * Level: Socket
           * Value type: Boolean
           * Description: Enables periodic probes on idle TCP connection.
           */
         const ::ds::Sock::OptNameType QDS_SO_KEEPALIVE = 0;
         
         /**
           * Option name: QDS_SO_REUSEADDR
           * Level: Socket
           * Value type: Boolean
           * Description: Enable socket port reuse.
           */
         const ::ds::Sock::OptNameType QDS_SO_REUSEADDR = 1;
         
         /**
           * Option name: QDS_SO_ERROR_ENABLE
           * Level: Socket
           * Value type: Boolean
           * Description: Enable retrieving of ICMP errors using SO_ERROR.
           */
         const ::ds::Sock::OptNameType QDS_SO_ERROR_ENABLE = 2;
         
         /**
           * Option name: QDS_SO_TX_IFACE
           * Level: Socket
           * Value type: unsigned long
           * Description: Get tx iface id.
           */
         const ::ds::Sock::OptNameType QDS_SO_TX_IFACE = 3;
         
         /**
           * Option name: QDS_SO_RCVBUF
           * Level: Socket
           * Value type: long
           * Description: Change the receive buffer size for the socket.
           */
         const ::ds::Sock::OptNameType QDS_SO_RCVBUF = 4;
         
         /**
           * Option name: QDS_SO_SNDBUF
           * Level: Socket
           * Value type: long
           * Description: Change the send buffer size for the socket.
           */
         const ::ds::Sock::OptNameType QDS_SO_SNDBUF = 5;
         
         /**
           * Option name: QDS_SO_ERROR
           * Level: Socket
           * Value type: long
           * Description: Get ICMP error on the socket.
           */
         const ::ds::Sock::OptNameType QDS_SO_ERROR = 6;
         
         /**
           * Option name: QDS_TCP_NODELAY
           * Level: TCP
           * Value type: Boolean
           * Description: Disable Nagle's algorithm.
           */
         const ::ds::Sock::OptNameType QDS_TCP_NODELAY = 7;
         
         /**
           * Option name: QDS_TCP_DELAYED_ACK
           * Level: TCP
           * Value type: Boolean
           * Description: Reduce the number of ACKs sent to half.
           */
         const ::ds::Sock::OptNameType QDS_TCP_DELAYED_ACK = 8;
         
         /**
           * Option name: QDS_TCP_SACK
           * Level: TCP
           * Value type: Boolean
           * Description: Enables TCP selective ACK option.
           */
         const ::ds::Sock::OptNameType QDS_TCP_SACK = 9;
         
         /**
           * Option name: QDS_TCP_TIMESTAMP
           * Level: TCP
           * Value type: Boolean
           * Description: Enables TCP time stamp option.
           */
         const ::ds::Sock::OptNameType QDS_TCP_TIMESTAMP = 10;
         
         /**
           * Option name: QDS_TCP_FIONREAD
           * Level: TCP
           * Value type: unsigned long
           * Description: Retrieve TCP queue size.
           */
         const ::ds::Sock::OptNameType QDS_TCP_FIONREAD = 11;
         
         /**
           * Option name: QDS_TCP_MAXSEG
           * Level: TCP
           * Value type: long
           * Description: Change the TCP maximum segment size.
           */
         const ::ds::Sock::OptNameType QDS_TCP_MAXSEG = 12;
         
         /**
           * Option name: QDS_IP_RECVIF
           * Level: IP
           * Value type: Boolean
           * Description: Enable retrieving of Rx iface of packet.
           */
         const ::ds::Sock::OptNameType QDS_IP_RECVIF = 13;
         
         /**
           * Option name: QDS_IP_RECVERR
           * Level: IP
           * Value type: Boolean
           * Description: Enable retrieving of extended ICMP error info.
           */
         const ::ds::Sock::OptNameType QDS_IP_RECVERR = 14;
         
         /**
           * Option name: IP_TOS
           * Level: IP
           * Value type: long
           * Description: Set TOS for outgoing IP packets.
           */
         const ::ds::Sock::OptNameType QDS_IP_TOS = 15;
         
         /**
           * Option name: QDS_IP_TTL
           * Level: IP
           * Value type: long
           * Description: Change the Time To Live value.
           */
         const ::ds::Sock::OptNameType QDS_IP_TTL = 16;
         
         /**
           * Option name: QDS_IP_MULTICAST_TTL
           * Level: IP
           * Value type: octet (larger values supplied to SetOpt API shall be
           *             truncated to 8 bit value).
           * Description: Specifies the time-to-live value for Multicast
           *              datagrams sent through this socket.
           */
         const ::ds::Sock::OptNameType QDS_IP_MULTICAST_TTL = 17;
         
         /**
           * Option name: QDS_IPV6_RECVERR
           * Level: IPv6
           * Value type: Boolean
           * Description: Enable retrieving of extended ICMPv6 error info.
           */
         const ::ds::Sock::OptNameType QDS_IPV6_RECVERR = 18;
         
         /**
           * Option name: QDS_IPV6_TCLASS
           * Level: IPV6
           * Value type: long
           * Description: Set the traffic class for the IPv6 socket.
           */
         const ::ds::Sock::OptNameType QDS_IPV6_TCLASS = 19;
         
         /**
           * Option name: QDS_TCP_MAX_BACKOFF_TIME 
           * Level: TCP
           * Value type: unsigned long
           * Description: Maximal TCP backoff time
           */
         const ::ds::Sock::OptNameType QDS_TCP_MAX_BACKOFF_TIME = 20;
         
         /**
           * Option name: QDS_IP_MULTICAST_LOOP
           * Level: IP
           * Value type: octet (larger values supplied to SetOpt API shall be
           *             truncated to 8 bit value).
           * Description: Enable/Disable Multicast datagrams to be looped back.
           */
         const ::ds::Sock::OptNameType QDS_IP_MULTICAST_LOOP = 21;
         
         /**
           * Option name: QDS_SO_FIONREAD
           * Level: Socket
           * Value type: unsigned long
           * Description: No. of bytes that can be read from a
           *              TCP/UDP/ICMP socket
           *
           */
         const ::ds::Sock::OptNameType QDS_SO_FIONREAD = 22;

         /** @memberof ds_Sock_OptName
           * 
           * Option name: QDS_SO_SKIP_ROUTE_SCOPE_CHECK 
           * Level: Socket 
           * Value type: boolean 
           * Description: TRUE if the check is enabled
           *             otherwise FALSE. Valid for both TCP & UDP
           */
	const ::ds::Sock::OptNameType QDS_SO_SKIP_ROUTE_SCOPE_CHECK = 23;

         /**
           * Option name: QDS_UDP_ENCAPS
           * Level: UDP
           * Value type: Boolean
           * Description: Enable UDP encapsulation.
           */
         const ::ds::Sock::OptNameType QDS_UDP_ENCAPS  = 24;

      };
      
      /**
        * LingerType. Used with Socket option SO_LINGER
        */
      struct LingerType {
         boolean lingerEnabled; /**< Linger on or off. */
         AEEINTERFACE_PADMEMBERS(timeInSec, 3)
         int timeInSec; /**< Linger time, in seconds. */
      };
      
      /**
        * Extended error type.
        */
      struct ExtendedErrType {
         unsigned int error_number; /**< Error number */
         unsigned char origin; /**< Where the error originated */
         unsigned char type; /**< Type field from the ICMP header */
         unsigned char code; /**< Code field from the ICMP header */
         AEEINTERFACE_PADMEMBERS(info, 1)
         unsigned int info; /**< Discovered MTU for EMSGSIZE errors */
      };
      
      /**
        * Extended error info type.
        * This structure represents information embedded in the msg_control 
        * parameter when the IP_RECVERR socket option is set and the 
        * MSG_ERRQUEUE flag is set. 
        * The cmsg header level is set to QDS_LEVEL_IP and the type to 
        * QDS_IP_RECVERR to indicate this payload.
        * See the RecvMsg API documentation for how to extract 
        * this info from msg_control parameter using utility macros.
        * Byte order of the err member is little endian.
        * Byte order of the addr member is network order.
        */
      struct ExtendedErrInfoType {
         ExtendedErrType err;
         ::ds::SockAddrStorageType addr;
      };
      
      /**
        * Received Iface info type.
        * This structure represents information embedded in the msg_control 
        * parameter when the IP_RECVIF socket option is set.
        * The cmsg header level is set to QDS_LEVEL_IP and the type to 
        * QDS_IP_RECVIF to indicate this payload.
        * See the RecvMsg API documentation for how to extract 
        * this info from msg_control parameter using utility macros.
        * Byte order of the ifaceId member is little endian.
        * Byte order of the addr member is network order.
        * @See INetwork::IfaceId
        */
      struct RecvIfaceInfoType {
         ::ds::Net::IfaceIdType IfaceId;
         ::ds::SockAddrStorageType Addr;
      };
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_SOCK_DEF_H
