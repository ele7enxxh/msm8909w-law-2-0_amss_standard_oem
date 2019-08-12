#ifndef AEENETADDRTYPES_H
#define AEENETADDRTYPES_H
/*=============================================================================

FILE:         AEENetAddrTypes.h

SERVICES:     None

DESCRIPTION:  definitions of network and socket related address types.

===============================================================================
   Copyright (c) 2006-2008 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
=============================================================================*/

#include "datamodem_variation.h"
#include "ds_Utils_StdDef.h"

// Network/Socket Address Family
#define AEE_AF_UNSPEC 0
#define AEE_AF_LOCAL  1
#define AEE_AF_INET   2
#define AEE_AF_INET6  3

// The types INAddr and INPort imply NETWORK BYTE ORDERING, which is important
// to keep straight when going between simulator and phone environments.
typedef uint32 INAddr;
typedef uint16 INPort;

typedef struct
{
   INAddr addr;
   INPort port;
} INSockAddr;

// wildcard address
#define AEE_INADDR_ANY      ((INAddr) 0)

// loopback address: 127.0.0.1 in host byte order
#define AEE_INADDR_LOOPBACK (0x7f000001L)

// BREW loopback address: 127.0.0.69 (a la *69) in host byte order
#define AEE_BREW_LOOPBACK   (0x7f000045L)

typedef union {
   byte   Byte[16];
   uint16 Word[8];
   uint32 DWord[4];
} INAddr6;

typedef struct IPAddr {
   int16 wFamily;    // AEE_AF_*
   union
   {
      INAddr v4;
      INAddr6 v6;
      //
   } addr;
} IPAddr;

#define AEE_MAX_LOCAL_NAME   128

typedef struct {
   int16 wFamily;               // AEE_AF_LOCAL
   char  szName[AEE_MAX_LOCAL_NAME];
} AEESockAddrLocal;

typedef struct {
   int16  wFamily;              // AEE_AF_INET
   INPort port;
   INAddr addr;
} AEESockAddrInet;

typedef struct {
   int16   wFamily;             // AEE_AF_INET6
   INPort  port;
   uint32  dwFlowinfo;
   INAddr6 addr;
   uint32  dwScopeId;
} AEESockAddrInet6;

typedef union AEESockAddrStorage {
   int16            wFamily;    // AEE_AF_*
   AEESockAddrLocal local;
   AEESockAddrInet  inet;
   AEESockAddrInet6 inet6;
} AEESockAddrStorage;

//
// IPv6 Privacy Address definitions
//

// IPv6 Privacy Address state
#define AEE_IPV6_PRIV_ADDR_STATE_VALID       0
#define AEE_IPV6_PRIV_ADDR_STATE_DEPRECATED  1
#define AEE_IPV6_PRIV_ADDR_STATE_DELETED     2
#define AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING 3
#define AEE_IPV6_PRIV_ADDR_STATE_GEN_FAILED  4
typedef uint32 AEEIPv6PrivAddrState;

// struct holding information regarding an IPv6 private address
typedef struct AEEIPv6PrivAddrInfo {
   AEEIPv6PrivAddrState    uState;
   boolean                 bIsUnique;
   INAddr6                 ipaAddr;
} AEEIPv6PrivAddrInfo;

/*-------------------------------------------------------------------
      Helper Macros
-------------------------------------------------------------------*/
// test for special addresses
static int __inline AEE_IN6_IS_ADDR_UNSPECIFIED(const INAddr6* pina6)
{
   return ((pina6->DWord[0] == 0) &&
           (pina6->DWord[1] == 0) &&
           (pina6->DWord[2] == 0) &&
           (pina6->DWord[3] == 0));
}

#if !defined(AEE_BIGENDIAN)
#define __AEENET_HTONL(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                   (((uint32)(x) & 0x0000ff00U) <<  8) | \
                   (((uint32)(x) & 0x00ff0000U) >>  8) | \
                   (((uint32)(x) & 0xff000000U) >> 24)))

#else /* #if !defined(AEE_BIGENDIAN) */
#define __AEENET_HTONL(x) (x)
#endif /* #if !defined(AEE_BIGENDIAN) */

static boolean __inline AEE_IS_ADDR_UNSPECIFIED(const AEESockAddrStorage* psas)
{
   switch (psas->wFamily)
   {
      case AEE_AF_UNSPEC:
         return TRUE;

      case AEE_AF_INET:
         return (psas->inet.addr == __AEENET_HTONL(AEE_INADDR_ANY));

      case AEE_AF_INET6:
         return (boolean)AEE_IN6_IS_ADDR_UNSPECIFIED(&psas->inet6.addr);

      default:
         return FALSE;
   }
}

static int __inline AEE_IN6_IS_ADDR_LOOPBACK(const INAddr6* pina6)
{
   return ((pina6->DWord[0] == 0) &&
           (pina6->DWord[1] == 0) &&
           (pina6->DWord[2] == 0) &&
           (pina6->DWord[3] == __AEENET_HTONL(1)));
}

static boolean __inline AEE_IS_ADDR_LOOPBACK(const AEESockAddrStorage* psas)
{
   switch (psas->wFamily)
   {
      case AEE_AF_INET:
         return (psas->inet.addr == __AEENET_HTONL(AEE_INADDR_LOOPBACK));

      case AEE_AF_INET6:
         return (boolean)AEE_IN6_IS_ADDR_LOOPBACK(&psas->inet6.addr);

      default:
         return FALSE;
   }
}

static int __inline AEE_IN6_IS_ADDR_BREW_LOOPBACK(const INAddr6* pina6)
{
   return ((pina6->DWord[0] == 0) &&
           (pina6->DWord[1] == 0) &&
           (pina6->DWord[2] == __AEENET_HTONL(0xffff)) &&
           (pina6->DWord[3] == __AEENET_HTONL(AEE_BREW_LOOPBACK)));
}

static boolean __inline AEE_IS_ADDR_BREW_LOOPBACK(const AEESockAddrStorage* psas)
{
   switch (psas->wFamily)
   {
      case AEE_AF_INET:
         return (psas->inet.addr == __AEENET_HTONL(AEE_BREW_LOOPBACK));

      case AEE_AF_INET6:
         return (boolean)AEE_IN6_IS_ADDR_BREW_LOOPBACK(&psas->inet6.addr);

      default:
         return FALSE;
   }
}

static int __inline AEE_IN6_IS_ADDR_V4MAPPED(const INAddr6* pina6)
{
   return ((pina6->Word[0] == 0) &&
           (pina6->Word[1] == 0) &&
           (pina6->Word[2] == 0) &&
           (pina6->Word[3] == 0) &&
           (pina6->Word[4] == 0) &&
           (pina6->Word[5] == 0xffff));
}

static boolean __inline AEE_IS_ADDR_V4MAPPED(const AEESockAddrStorage* psas)
{
   switch (psas->wFamily)
   {
      case AEE_AF_INET:
         return FALSE;

      case AEE_AF_INET6:
         return (boolean)AEE_IN6_IS_ADDR_V4MAPPED(&psas->inet6.addr);

      default:
         return FALSE;
   }
}

// initialize for special IPv6 addresses
static void __inline AEE_IN6ADDR_SET_ANY(AEESockAddrInet6* psain6)
{
   psain6->wFamily = AEE_AF_INET6;
   psain6->port = 0;
   psain6->dwFlowinfo = 0;
   psain6->dwScopeId = 0;

   psain6->addr.DWord[0] = 0;
   psain6->addr.DWord[1] = 0;
   psain6->addr.DWord[2] = 0;
   psain6->addr.DWord[3] = 0;
}

static void __inline AEE_IN6ADDR_SET_LOOPBACK(AEESockAddrInet6* psain6)
{
   psain6->wFamily = AEE_AF_INET6;
   psain6->port = 0;
   psain6->dwFlowinfo = 0;
   psain6->dwScopeId = 0;

   psain6->addr.DWord[0] = 0;
   psain6->addr.DWord[1] = 0;
   psain6->addr.DWord[2] = 0;
   psain6->addr.DWord[3] = __AEENET_HTONL(1);
}

static void __inline AEE_IN6ADDR_SET_BREW_LOOPBACK(AEESockAddrInet6* psain6)
{
   psain6->wFamily = AEE_AF_INET6;
   psain6->port = 0;
   psain6->dwFlowinfo = 0;
   psain6->dwScopeId = 0;

   psain6->addr.DWord[0] = 0;
   psain6->addr.DWord[1] = 0;
   psain6->addr.DWord[2] = __AEENET_HTONL(0xffff);
   psain6->addr.DWord[3] = __AEENET_HTONL(AEE_BREW_LOOPBACK);
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================
Network-Socket Address Family

Description:
   This data type is an enumeration of supported Network/Socket Address Family.

Definition:
   #define AEE_AF_UNSPEC 0
   #define AEE_AF_LOCAL  1
   #define AEE_AF_INET   2
   #define AEE_AF_INET6  3

Members:
   AEE_AF_UNSPEC: unspecified address family. used for selecting the "ANY" CDMA
                  interface. Introduced BREW Client 3.1.4.
   AEE_AF_LOCAL:  local domain
   AEE_AF_INET:   IPv4
   AEE_AF_INET6:  IPv6, Introduced BREW Client 3.1.4.

Comments:
   None

See Also:
   ISockPort_OpenEx()
   IAddrInfo_StartQuery()

=======================================================================

INAddr

Description:
   This data type denotes the network byte-order values for an IP address of
   an IPv4 socket or endpoint.

Definition:
   typedef uint32 INAddr;

Members:
   None

Comments:
   None

See Also:
   INPort
   ISockPort_Bind()
   ISockPort_SendTo()
   ISockPort_Connect()
   ISockPort_RecvFrom()

=======================================================================

INPort

Description:
   This data type denotes the network byte-order values for the port of
   an IP socket or endpoint.

Definition:
   typedef uint16 INPort;

Members:
   None

Comments:
   None

See Also:
   INAddr
   ISockPort_Bind()
   ISockPort_SendTo()
   ISockPort_Connect()
   ISockPort_RecvFrom()

=======================================================================

INSockAddr

Description:
   An internet IP endpoint: address and port.

Definition:
   typedef struct {
      INAddr addr;
      INPort port;
   } INSockAddr;

Members:
   addr  :  the IP address in network byte order
   port  :  the TCP or UDP port in network byte order

Comments:
   None

See Also:
   INAddr
   INPort
   ISockPort_IOCtl()

====================================================================

AEE_INADDR_ANY

Description:
   This is a wildcard address that indicates an interest in any
   available IP address.

Definition:
   #define AEE_INADDR_ANY      ((INAddr) 0)

Members:
   None

Comments:
   None

See Also:
   ISockPort_Bind()

=======================================================================

AEE_INADDR_LOOPBACK

Description:
   This is the loopback address, in host byte order, which can be
   used for socket communication without accessing the network.  This
   is not supported in all devices.

Definition:
   #define AEE_INADDR_LOOPBACK (0x7f000001L)

Members:
   None

Comments:
   None

See Also:
   ISockPort_Bind(), ISockPort_Connect(), ISockPort_SendTo(), ISockPort_RecvFrom()

=======================================================================

AEE_BREW_LOOPBACK

Description:
   This is the BREW loopback address, in host byte order, which can
   be used for socket communication without accessing the network.
   This can be used on devices that do not support AEE_INADDR_LOOPBACK.

Definition:
   #define AEE_BREW_LOOPBACK   (0x7f000045L)

Members:
   None

Comments:
   None

See Also:
   ISockPort_Bind(), ISockPort_Connect(), ISockPort_SendTo(), ISockPort_RecvFrom()

=======================================================================

INAddr6

Description:
   This data type denotes the network byte-order values for the IP address of
   an IPv6 socket or endpoint.

Definition:
   typedef union {
      byte   Byte[16];
      uint16 Word[8];
      uint32 DWord[4];
   } INAddr6;

Members:
   Byte:  the IPv6 address as a byte array
   Word:  the IPv6 address as a word array
   DWord: the IPv6 address as a double word array

Comments:
   None

See Also:
   None

=======================================================================

IPAddr

Description:
   This data type denotes a generic address of a socket or endpoint.

Definition:
   typedef struct IPAddr {
      int16 wFamily;
      union
      {
         INAddr v4;
         INAddr6 v6;
      } addr;
   } IPAddr;

Members:
   wFamily: address family.
   addr: address-family specific address

Comments:
   None

See Also:
   Socket Address Family

=====================================================================

AEESockAddrInet

Description:
   This data type denotes an IPv4 socket address structure

Definition:
   typedef struct {
      int16  wFamily;
      INPort port;
      INAddr addr;
   } AEESockAddrInet;

Members:
   wFamily: the address family (AEE_AF_INET)
   port  :  the TCP or UDP port in network byte order
   addr  :  the IP address in network byte order

Comments:
   None

See Also:
   INAddr
   INPort

=======================================================================

AEESockAddrInet6

Description:
   This data type denotes an IPv6 socket address structure

Definition:
   typedef struct {
      int16   wFamily;
      INPort  port;
      uint32  dwFlowinfo;
      INAddr6 addr;
      uint32  dwScopeId;
   } AEESockAddrInet6;

Members:
   wFamily:    the address family (AEE_AF_INET6)
   port  :     the TCP or UDP port in network byte order
   dwFlowinfo: the IPv6 flow info value in network byte order
   addr  :     the IP address in network byte order
   dwScopeId : the IPv6 scope id value in network byte order

Comments:
   None

See Also:
   INAddr6
   INPort

=======================================================================

AEESockAddrStorage

Description:
   This data type denotes a generic socket address structure.

Definition:
   typedef union {
      int16            wFamily;
      AEESockAddrLocal local;
      AEESockAddrInet  inet;
      AEESockAddrInet6 inet6;
   } AEESockAddrStorage;

Members:
   wFamily: the address family (AEE_AF_*)
   local  : a local domain socket address, currently not supported!
   inet  :  an IPv4 socket address
   inet6 :  an IPv6 socket address

Comments:
   Some BREW functions accept an opaque (void*) pointer to an address.
   When calling one of these functions, it is applicable to either use a
   pointer to an AEESockAddrStorage, or a pointer to the appropriate
   member of AEESockAddrStorage (for example, a pointer to an AEESockAddrInet
   in the case of an IPv4 socket).

See Also:
   AEESockAddrInet
   AEESockAddrInet6
   ISockPort_Bind()
   ISockPort_Connect()
   ISockPort_SendTo()
   ISockPort_RecvFrom()
   ISockPort_GetSockName()
   ISockPort_GetPeerName()

=======================================================================

AEEIPv6PrivAddrState

Description:
   This data type defines the various states in which an IPv6 Private Address
   may be.

Definition:
   typedef uint32 AEEIPv6PrivAddrState;

Members:
   AEE_IPV6_PRIV_ADDR_STATE_VALID:        The address is in a valid state.
   AEE_IPV6_PRIV_ADDR_STATE_DEPRECATED:   The address is in a deprecated state.
   AEE_IPV6_PRIV_ADDR_STATE_DELETED:      The address is in a deleted state.
   AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING:  A "Generate" request was received by
                                          the lower levels, but the address was not
                                          generated yet.
   AEE_IPV6_PRIV_ADDR_STATE_GEN_FAILED:   Address generation has failed.

Comments:
   Note that in future versions, additional states may be added.

See Also:
   AEEIPv6PrivAddrInfo
   INetwork_GetIPv6PrivAddrInfo()
   INetwork_GenerateIPv6PrivAddress()

=======================================================================

AEEIPv6PrivAddrInfo

Description:
   This data type holds the information regarding a private IPv6 address.

Definition:
   typedef struct AEEIPv6PrivAddrInfo {
      AEEIPv6PrivAddrState    uState;
      boolean                 bIsUnique;
      INAddr6                 ipaAddr;
   } AEEIPv6PrivAddrInfo;

Members:
   uState:     The state of the address.
   bIsUnique:  Is the address unique or shared.
   IPAddr:     The address itself.

Comments:
   None

See Also:
   AEEIPv6PrivAddrState
   INetwork_GetIPv6PrivAddrInfo()
   INetwork_GenerateIPv6PrivAddress()

=======================================================================*/

/*=======================================================================
  HELPER DOCUMENTATION
=======================================================================

  AEE_IN6_IS_ADDR_UNSPECIFIED()

Description:
  Determine if a given IPv6 address is unspecified.
  Supports IPv6 addresses only.
  Unspecified IPv6 address is ::0.

Prototype:
  int AEE_IN6_IS_ADDR_UNSPECIFIED(const INAddr6* pina6)

Parameters:
   pina6: address to be checked.

Return Value:
   TRUE if pina6 is an IPv6 unspecified address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

AEE_IS_ADDR_UNSPECIFIED()

Description:
  Determine if a given IP address is unspecified.
  Supports both IPv4 and IPv6 addresses.
  Unspecified IPv4 address is 0.0.0.0.
  Unspecified IPv6 address is ::0.

Prototype:
  boolean AEE_IS_ADDR_UNSPECIFIED(const AEESockAddrStorage* psas)

Parameters:
   psas: socket address to be checked.

Return Value:
   TRUE if psas contains an unspecified address.
   FALSE otherwise.

Comments:
   If the address family of the given address is AEE_AF_UNSPEC the address
   is considered anyway unspecified.

Side Effects:
   None

See Also:
   None

=======================================================================

  AEE_IN6_IS_ADDR_LOOPBACK()

Description:
  Determine if a given IPv6 address is a loopback address.
  Supports IPv6 addresses only.
  Loopback IPv6 address is ::1.

Prototype:
  int AEE_IN6_IS_ADDR_LOOPBACK(const INAddr6* pina6)

Parameters:
   pina6: address to be checked.

Return Value:
   TRUE if pina6 is an IPv6 loopback address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

AEE_IS_ADDR_LOOPBACK()

Description:
  Determine if a given IP address is a loopback address.
  Supports both IPv4 and IPv6 addresses.
  Loopback IPv4 address is 127.0.0.1.
  Loopback IPv6 address is ::1.

Prototype:
  boolean AEE_IS_ADDR_LOOPBACK(const AEESockAddrStorage* psas)

Parameters:
   psas: socket address to be checked.

Return Value:
   TRUE if psas contains the loopback address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

  AEE_IN6_IS_ADDR_BREW_LOOPBACK()

Description:
  Determine if a given IPv6 address is a BREW loopback address.
  Supports IPv6 addresses only.
  BREW Loopback IPv6 address is ::ffff:127.0.0.69.

Prototype:
  int AEE_IN6_IS_ADDR_BREW_LOOPBACK(const INAddr6* pina6)

Parameters:
   pina6: address to be checked.

Return Value:
   TRUE if pina6 is an IPv6 BREW loopback address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

AEE_IS_ADDR_BREW_LOOPBACK()

Description:
  Determine if a given IP address is a BREW loopback address.
  Supports both IPv4 and IPv6 addresses.
  BREW Loopback IPv4 address is 127.0.0.69.
  BREW Loopback IPv6 address is ::ffff:127.0.0.69.

Prototype:
  boolean AEE_IS_ADDR_BREW_LOOPBACK(const AEESockAddrStorage* psas)

Parameters:
   psas: socket address to be checked.

Return Value:
   TRUE if psas contains the BREW loopback address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

  AEE_IN6_IS_ADDR_V4MAPPED()

Description:
  Determine if a given IPv6 address is a V4 mapped address.

Prototype:
  int AEE_IN6_IS_ADDR_V4MAPPED(const INAddr6* pina6)

Parameters:
   pina6: address to be checked.

Return Value:
   TRUE if pina6 is an IPv6, v4 mapped, address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================

AEE_IS_ADDR_V4MAPPED()

Description:
  Determine if a given IP address is an IPv6, V4 mapped, address.

Prototype:
  boolean AEE_IS_ADDR_V4MAPPED(const AEESockAddrStorage* psas)

Parameters:
   psas: socket address to be checked.

Return Value:
   TRUE if psas contains an IPv6, V4 mapped, address.
   FALSE otherwise.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
*/

#endif  // AEENETADDRTYPES_H
