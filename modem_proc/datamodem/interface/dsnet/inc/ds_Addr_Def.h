#ifndef DS_ADDR_DEF_H
#define DS_ADDR_DEF_H

/*============================================================================
  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
  ============================================================================*/
#include "datamodem_variation.h"
#include "ds_Utils_Interface.h"
#if !defined(AEEINTERFACE_CPLUSPLUS)
#include "ds_Utils_StdDef.h"
typedef unsigned short ds_AddrFamilyType;
#define ds_AddrFamily_QDS_AF_UNSPEC 0
#define ds_AddrFamily_QDS_AF_INET 2
#define ds_AddrFamily_QDS_AF_INET6 3

/**
  * SockAddrStorageType is to be used for both IPv4 and IPv6 addresses.
  * Its content (including the family field) shall be in Network Byte Order.
  * Size of sockAddrStorage is derived from length of IPv6 Socket Address.
  * Usage:
  * - Allocate SockAddrStorageType instance
  * - Allocate a pointer to either AddrINType or AddrIN6Type (defined in separate, language specific headers)
  * - Assign the address of SockAddrStorageType instance to the AddrINType/AddrIN6Type pointer
  * - Fill in the address fields (in Network Byte order) using the AddrINType/AddrIN6Type pointer
  * - Provide the address of SockAddrStorageType instance to the desired Socket API 
  */
typedef unsigned char ds_SockAddrStorageType[28];
typedef unsigned int ds_INAddr6Type[4];
struct ds_IPAddrType {
   ds_AddrFamilyType family;
   ds_INAddr6Type addr;
};
typedef struct ds_IPAddrType ds_IPAddrType;

typedef struct ds_SockAddrINType ds_SockAddrINType;
#include "ds_Utils_beginpack.h"

/* Internet style IPv4 socket address in Network Byte order */
struct ds_SockAddrINType              
{
   ds_AddrFamilyType  family;         /* Address family - AF_INET */
   unsigned short     port;           /* transport layer port number */
   uint32             addr;           /* IPv4 address */
}
#include "ds_Utils_endpack.h"
;

/* Internet style IPv6 socket address in Network Byte order */
typedef struct ds_SockAddrIN6Type ds_SockAddrIN6Type;  
#include "ds_Utils_beginpack.h"
struct ds_SockAddrIN6Type
{
   ds_AddrFamilyType   family;         /* Address family - AF_INET6 */
   unsigned short      port;           /* transport layer port number */
   uint32              flowInfo;       /* IPv6 flow information */
   ds_INAddr6Type      addr;           /* IPv6 address */
   uint32              scopeId;        /* Set of interface for a scope */
} 
#include "ds_Utils_endpack.h"
;

#else /* C++ */
#include "ds_Utils_StdDef.h"
namespace ds
{
   typedef unsigned short AddrFamilyType;
   namespace AddrFamily
   {
      const ::ds::AddrFamilyType QDS_AF_UNSPEC = 0;
      const ::ds::AddrFamilyType QDS_AF_INET = 2;
      const ::ds::AddrFamilyType QDS_AF_INET6 = 3;
   };
   
   /**
     * SockAddrStorageType is to be used for both IPv4 and IPv6 addresses.
     * Its content (including the family field) shall be in Network Byte Order.
     * Size of sockAddrStorage is derived from length of IPv6 Socket Address.
     * Usage:
     * - Allocate SockAddrStorageType instance
     * - Allocate a pointer to either AddrINType or AddrIN6Type (defined in separate, language specific headers)
     * - Assign the address of SockAddrStorageType instance to the AddrINType/AddrIN6Type pointer
     * - Fill in the address fields (in Network Byte order) using the AddrINType/AddrIN6Type pointer
     * - Provide the address of SockAddrStorageType instance to the desired Socket API 
     */
   typedef unsigned char SockAddrStorageType[28];
   typedef unsigned int INAddr6Type[4];
   struct IPAddrType {
      AddrFamilyType family;
      INAddr6Type addr;
   };

   /* Internet style IPv4 socket address in Network Byte order */
   typedef struct SockAddrINType SockAddrINType;            
   #include "ds_Utils_beginpack.h"
   struct SockAddrINType
   {
      AddrFamilyType   family;         /* Address family - AF_INET */
      unsigned short   port;           /* transport layer port number */
      uint32           addr;           /* IPv4 address */
   }
   #include "ds_Utils_endpack.h"
   ;

   /* Internet style IPv6 socket address in Network Byte order */
   typedef struct SockAddrIN6Type SockAddrIN6Type;
   #include "ds_Utils_beginpack.h"
   struct SockAddrIN6Type
   {
      AddrFamilyType   family;         /* Address family - AF_INET6 */
      unsigned short   port;           /* transport layer port number */
      uint32           flowInfo;       /* IPv6 flow information */
      INAddr6Type      addr;           /* IPv6 address */
      uint32           scopeId;        /* Set of interface for a scope */
   }
   #include "ds_Utils_endpack.h"
   ;
};
#endif /* !defined(AEEINTERFACE_CPLUSPLUS) */
#endif //DS_ADDR_DEF_H
