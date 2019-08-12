#ifndef DS_ERRORS_DEF_H
#define DS_ERRORS_DEF_H

/*============================================================================
  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
typedef AEEResult ds_ErrorType;
#define ds_Error_QDS_ERRORS_BASE_UNIQUE 0x106d075
#define ds_Error_QDS_ERRORS_BASE 0x6d07500
#define ds_Error_QDS_EEOF 0x6d07501
#define ds_Error_QDS_EINTR 0x6d07502
#define ds_Error_QDS_EBADF 0x6d07503
#define ds_Error_QDS_EFAULT 0x6d07504
#define ds_Error_QDS_EAFNOSUPPORT 0x6d07505
#define ds_Error_QDS_EPROTOTYPE 0x6d07506
#define ds_Error_QDS_ESOCKNOSUPPORT 0x6d07507
#define ds_Error_QDS_EPROTONOSUPPORT 0x6d07508
#define ds_Error_QDS_EMFILE 0x6d07509
#define ds_Error_QDS_EOPNOTSUPP 0x6d0750a
#define ds_Error_QDS_EADDRINUSE 0x6d0750b
#define ds_Error_QDS_EADDRREQ 0x6d0750c
#define ds_Error_QDS_EINPROGRESS 0x6d0750d
#define ds_Error_QDS_EISCONN 0x6d0750e
#define ds_Error_QDS_EIPADDRCHANGED 0x6d0750f
#define ds_Error_QDS_ENOTCONN 0x6d07510
#define ds_Error_QDS_ECONNREFUSED 0x6d07511
#define ds_Error_QDS_ETIMEDOUT 0x6d07512
#define ds_Error_QDS_ECONNRESET 0x6d07513
#define ds_Error_QDS_ECONNABORTED 0x6d07514
#define ds_Error_QDS_EPIPE 0x6d07515
#define ds_Error_QDS_ENETDOWN 0x6d07516
#define ds_Error_QDS_EMAPP 0x6d07517
#define ds_Error_QDS_EBADAPP 0x6d07518
#define ds_Error_QDS_ESOCKEXIST 0x6d07519
#define ds_Error_QDS_EINVAL 0x6d0751a
#define ds_Error_QDS_EMSGSIZE 0x6d0751b
#define ds_Error_QDS_EMSGTRUNC 0x6d0751c
#define ds_Error_QDS_ENOROUTE 0x6d0751d
#define ds_Error_QDS_ESHUTDOWN 0x6d0751e
#define ds_Error_QDS_INTERNAL 0x6d0751f
#define ds_Error_QDS_ERRORS_SO_BASE 0x6d075c8
#define ds_Error_QDS_ENOPROTOOPT 0x6d075c9
#define ds_Error_QDS_ERRORS_ICMP_BASE 0x6d0762c
#define ds_Error_QDS_ENETUNREACH 0x6d0762d
#define ds_Error_QDS_EHOSTUNREACH 0x6d0762e
#define ds_Error_QDS_EHOSTDOWN 0x6d0762f
#define ds_Error_QDS_ENONET 0x6d07630
#define ds_Error_QDS_EPROTO 0x6d07631
#define ds_Error_QDS_EACCES 0x6d07632
#define ds_Error_QDS_ERRORS_QOS_BASE 0x6d07690
#define ds_Error_QDS_EQOSUNAWARE 0x6d07691
#define ds_Error_QDS_ERRORS_IPv6_BASE 0x6d076f4
#define ds_Error_QDS_ENOSRCADDR 0x6d076f5
#define ds_Error_QDS_ERRORS_NET_BASE 0x6d07758
#define ds_Error_QDS_ENETISCONN 0x6d07759
#define ds_Error_QDS_ENETINPROGRESS 0x6d0775a
#define ds_Error_QDS_ENETNONET 0x6d0775b
#define ds_Error_QDS_ENETCLOSEINPROGRESS 0x6d0775c
#define ds_Error_QDS_ENETEXIST 0x6d0775d
#define ds_Error_QDS_ENETGOINGDORMANT 0x6d0775e
#define ds_Error_QDS_ERRORS_DNS_BASE 0x6d077bc
#define ds_Error_QDS_ENAMEERR 0x6d077bd
#define ds_Error_QDS_EINUSE 0x6d077be
#define ds_Error_QDS_EHOSTNOTFOUND 0x6d077bf
#define ds_Error_QDS_ETRYAGAIN 0x6d077c0
#define ds_Error_QDS_ENORECOVERY 0x6d077c1
#define ds_Error_QDS_ENOADDRESS 0x6d077c2
#define ds_Error_QDS_ETRUNCATED 0x6d077c3
#define ds_Error_QDS_ESYSTEM 0x6d077c4
#define ds_Error_QDS_EMAXQUERIES 0x6d077c5
#define ds_Error_QDS_ERRORS_BCMCSDB_BASE 0x6d07820
#define ds_Error_QDS_BCMCSDB_OVERFLOW 0x6d07821
#define ds_Error_QDS_BCMCSDB_NO_OVERWRITE 0x6d07822
#else /* C++ */
#include "ds_Utils_StdDef.h"
namespace ds
{
   typedef ::AEEResult ErrorType;
   namespace Error
   {
      const ::AEEResult QDS_ERRORS_BASE_UNIQUE = 0x106d075;
      const ::AEEResult QDS_ERRORS_BASE = 0x6d07500;
      const ::AEEResult QDS_EEOF = 0x6d07501;
      const ::AEEResult QDS_EINTR = 0x6d07502;
      const ::AEEResult QDS_EBADF = 0x6d07503;
      const ::AEEResult QDS_EFAULT = 0x6d07504;
      const ::AEEResult QDS_EAFNOSUPPORT = 0x6d07505;
      const ::AEEResult QDS_EPROTOTYPE = 0x6d07506;
      const ::AEEResult QDS_ESOCKNOSUPPORT = 0x6d07507;
      const ::AEEResult QDS_EPROTONOSUPPORT = 0x6d07508;
      const ::AEEResult QDS_EMFILE = 0x6d07509;
      const ::AEEResult QDS_EOPNOTSUPP = 0x6d0750a;
      const ::AEEResult QDS_EADDRINUSE = 0x6d0750b;
      const ::AEEResult QDS_EADDRREQ = 0x6d0750c;
      const ::AEEResult QDS_EINPROGRESS = 0x6d0750d;
      const ::AEEResult QDS_EISCONN = 0x6d0750e;
      const ::AEEResult QDS_EIPADDRCHANGED = 0x6d0750f;
      const ::AEEResult QDS_ENOTCONN = 0x6d07510;
      const ::AEEResult QDS_ECONNREFUSED = 0x6d07511;
      const ::AEEResult QDS_ETIMEDOUT = 0x6d07512;
      const ::AEEResult QDS_ECONNRESET = 0x6d07513;
      const ::AEEResult QDS_ECONNABORTED = 0x6d07514;
      const ::AEEResult QDS_EPIPE = 0x6d07515;
      const ::AEEResult QDS_ENETDOWN = 0x6d07516;
      const ::AEEResult QDS_EMAPP = 0x6d07517;
      const ::AEEResult QDS_EBADAPP = 0x6d07518;
      const ::AEEResult QDS_ESOCKEXIST = 0x6d07519;
      const ::AEEResult QDS_EINVAL = 0x6d0751a;
      const ::AEEResult QDS_EMSGSIZE = 0x6d0751b;
      const ::AEEResult QDS_EMSGTRUNC = 0x6d0751c;
      const ::AEEResult QDS_ENOROUTE = 0x6d0751d;
      const ::AEEResult QDS_ESHUTDOWN = 0x6d0751e;
      const ::AEEResult QDS_INTERNAL = 0x6d0751f;
      const ::AEEResult QDS_ERRORS_SO_BASE = 0x6d075c8;
      const ::AEEResult QDS_ENOPROTOOPT = 0x6d075c9;
      const ::AEEResult QDS_ERRORS_ICMP_BASE = 0x6d0762c;
      const ::AEEResult QDS_ENETUNREACH = 0x6d0762d;
      const ::AEEResult QDS_EHOSTUNREACH = 0x6d0762e;
      const ::AEEResult QDS_EHOSTDOWN = 0x6d0762f;
      const ::AEEResult QDS_ENONET = 0x6d07630;
      const ::AEEResult QDS_EPROTO = 0x6d07631;
      const ::AEEResult QDS_EACCES = 0x6d07632;
      const ::AEEResult QDS_ERRORS_QOS_BASE = 0x6d07690;
      const ::AEEResult QDS_EQOSUNAWARE = 0x6d07691;
      const ::AEEResult QDS_ERRORS_IPv6_BASE = 0x6d076f4;
      const ::AEEResult QDS_ENOSRCADDR = 0x6d076f5;
      const ::AEEResult QDS_ERRORS_NET_BASE = 0x6d07758;
      const ::AEEResult QDS_ENETISCONN = 0x6d07759;
      const ::AEEResult QDS_ENETINPROGRESS = 0x6d0775a;
      const ::AEEResult QDS_ENETNONET = 0x6d0775b;
      const ::AEEResult QDS_ENETCLOSEINPROGRESS = 0x6d0775c;
      const ::AEEResult QDS_ENETEXIST = 0x6d0775d;
      const ::AEEResult QDS_ENETGOINGDORMANT = 0x6d0775e;
      const ::AEEResult QDS_ERRORS_DNS_BASE = 0x6d077bc;
      const ::AEEResult QDS_ENAMEERR = 0x6d077bd;
      const ::AEEResult QDS_EINUSE = 0x6d077be;
      const ::AEEResult QDS_EHOSTNOTFOUND = 0x6d077bf;
      const ::AEEResult QDS_ETRYAGAIN = 0x6d077c0;
      const ::AEEResult QDS_ENORECOVERY = 0x6d077c1;
      const ::AEEResult QDS_ENOADDRESS = 0x6d077c2;
      const ::AEEResult QDS_ETRUNCATED = 0x6d077c3;
      const ::AEEResult QDS_ESYSTEM = 0x6d077c4;
      const ::AEEResult QDS_EMAXQUERIES = 0x6d077c5;
      const ::AEEResult QDS_ERRORS_BCMCSDB_BASE = 0x6d07820;
      const ::AEEResult QDS_BCMCSDB_OVERFLOW = 0x6d07821;
      const ::AEEResult QDS_BCMCSDB_NO_OVERWRITE = 0x6d07822;
   };
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_ERRORS_DEF_H
