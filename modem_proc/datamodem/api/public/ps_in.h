#ifndef PS_IN_H
#define PS_IN_H
/*===========================================================================

                   M O D E L    H E A D E R    F I L E

===========================================================================*/
/**
  @file ps_in.h
  @brief Contains the definitions related to IP/IPv6 Protocol.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the ipUtilities group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_in.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
04/09/14    ash    Added Prefix Delegation support.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
12/28/10    pp     PS_IN_IS_ADDR_LIMITED_BROADCAST MACRO fix.
12/16/10    dm     Added GRE protocol used for PPTP VPN Passthrough.
07/08/10    pp     Removed unused metacomments
10/13/09    ts     Added definition of PS_IN_ADDRSTRLEN and
                   PS_IN6_ADDRSTRLEN. Previously were defined in dssdns.h.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
05/21/09    pp     IN6 defs renamed to ps_in6 etc.,
03/11/09    jy     Changes to avoid ps/ds type name conflicts when build CDPS
                   with third-part software modules containing a data stack.
11/09/07    ssh    Added support for MIPv6
11/22/06    sv     Added support for IPSEC support for IPv6
11/21/06    rt     Added IGMP/MLD support for MBMS.
11/06/06    mct    Added new macro for identifying valid IPv6 addresses.
04/19/06    rt     Added new IPv6 header extensions.
02/21/06    mct    Compiler fix.
04/26/05    vp     Put parenthesis around IN6_GET_V4_FROM_V4_MAPPED_V6.
04/19/05    jd     Fixed limited broadcast macro to exclude experimental IP
04/18/05    vp     Macros for IPv6 sitelocal and global multicast addrs.
04/13/05    ifk    Added in6_are_prefix_equal
01/10/05    sv     Merged IPSEC changes.
12/27/04    lyr    Added macro to determine if an address is a bcast address
06/11/04    vp     Packed constants for IP protocol types into
                   ip_protocol_enum_type. Definition of byte ordering macros.
05/26/04    mct    Added IN6_IS_PREFIX_LINKLOCAL to properly decode the
                   linklocal prefix.
04/28/04    aku    Added macros to support BCMCS feature.
03/03/04    sv     Added IS_ADDR_MC_LINK_LOCAL. Fixed IS_ADDR_LINKLOCAL
                   macro to use the right mask.
12/07/03    aku/rc Added dss_htonll() and dss_ntohll() macros
11/11/03    aku/rc  Fixed macro IN6_IS_ADDR_LINKLOCAL to use the right address.
08/21/03    ss     Correctly use the last quad while returning the v4 address
                   from v4 mapped v6 address
08/14/03    sv     Created module
===========================================================================*/
/* group: ipUtilities */

/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"

/** @addtogroup datatypes 
  @{
*/
/** Identifies protocols and contains the extension header types for IPv6. 
  These protocol numbers are use in dss_socket() in the dssocket.h file.
*/
typedef enum
{
  PS_IPV6_BASE_HDR        = 4,                /**< IPv6 base header. */
  PS_IPPROTO_HOP_BY_HOP_OPT_HDR = 0,          /**< Hop-by-hop option header. */
  PS_IPPROTO_ICMP         = 1,                /**< ICMP protocol. */
  PS_IPPROTO_IGMP         = 2,                /**< IGMP protocol. */
  PS_IPPROTO_IP           = PS_IPV6_BASE_HDR, /**< IPv4 protocol. */
  PS_IPPROTO_TCP          = 6,                /**< TCP protocol. */
  PS_IPPROTO_UDP          = 17,               /**< UDP protocol. */
  PS_IPPROTO_IPV6         = 41,               /**< IPv6 protocol. */
  PS_IPPROTO_ROUTING_HDR  = 43,               /**< Routing header. */
  PS_IPPROTO_FRAG_HDR     = 44,               /**< Fragmentation header. */
  PS_IPPROTO_GRE          = 47,               /**< GRE protocol. */
  PS_IPPROTO_ESP          = 50,               /**< ESP protocol. */
  PS_IPPROTO_AH           = 51,               /**< Authentication header. */
  PS_IPPROTO_ICMP6        = 58,               /**< ICMPv6. */
  PS_NO_NEXT_HDR          = 59,               /**< No next header for IPv6. */
  PS_IPPROTO_DEST_OPT_HDR = 60,               /**< Destination options header. */
  PS_IPPROTO_MOBILITY_HDR = 135,              /**< Mobility header. */
  PS_IPPROTO_TCP_UDP      = 253               /**< Unspecified protocol. */
} ps_ip_protocol_enum_type;

/**
  @brief Internet-family specific host Internet address.
*/
struct ps_in_addr  /* structure defined for historic reasons.*/
{
  uint32 ps_s_addr; /**< Socket address.*/
};

/**
  Local IP wildcard address.
 */
enum
{
  PS_INADDR_ANY  = 0
};

/**
  @brief IPv6 address structure.
 */
struct ps_in6_addr
{
  /** IPV6 address structure. */
  union
  {
    uint8   u6_addr8[16];  /**< IPv6 address in uint8 format. */
    uint16  u6_addr16[8];  /**< IPv6 address in uint16 format. */
    uint32  u6_addr32[4];  /**< IPv6 address in uint32 format. */
    uint64  u6_addr64[2];  /**< IPv6 address in uint64 format. */
  } in6_u;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/** Replacement string for uint8 address format. */
#define ps_s6_addr    in6_u.u6_addr8
/** Replacement string for uint16 address format.*/
#define ps_s6_addr16  in6_u.u6_addr16
/** Replacement string for uint32 address format.*/
#define ps_s6_addr32  in6_u.u6_addr32
/** Replacement string for uint64 address format.*/
#define ps_s6_addr64  in6_u.u6_addr64
};

/*---------------------------------------------------------------------------
  IPv6 prefix related constants
---------------------------------------------------------------------------*/
typedef enum
{
  IPV6_PREFIX_LEN_INVALID = 0,
  IPV6_PREFIX_LEN_32      = 32,
  IPV6_PREFIX_LEN_40      = 40,
  IPV6_PREFIX_LEN_48      = 48,
  IPV6_PREFIX_LEN_56      = 56,
  IPV6_PREFIX_LEN_64      = 64,
  IPV6_PREFIX_LEN_96      = 96
} ipv6_prefix_len_enum_type;

#define IPV6_PREFIX_LEN_MAX_VALS 6
#define IPV6_DEFAULT_PREFIX_LEN  IPV6_PREFIX_LEN_64

/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Common link layer address structure.
*/
typedef struct
{
  byte   ll_addr[6];   /**< Link layer address.*/
  uint8  ll_addr_len;  /**< Length of the link layer address.*/
} ps_link_layer_addr_type;

#ifdef __cplusplus
extern "C" {
#endif

/** ::.  Defined as #PS_IN6ADDR_ANY_INIT. */
extern const struct ps_in6_addr ps_in6addr_any; 
/** ::1. Defined as #PS_IN6ADDR_LOOPBACK_INIT. */
extern const struct ps_in6_addr ps_in6addr_loopback;
/** Scopes the IPv6 address structure as external. */
extern const struct ps_in6_addr in6addr_all_hosts;

#ifdef __cplusplus
}
#endif

/**
  @brief Internet-family socket address structures for IPv4.
*/
struct ps_sockaddr_in           
{
  uint16 ps_sin_family;          /**< Internet socket family. */
  uint16 ps_sin_port;            /**< Internet socket port. */
  /** Internet socket address. */
  struct ps_in_addr ps_sin_addr; 
  char   ps_sin_zero[8];         /**< Zeros the data for this address type. */
};

/**
  @brief Internet-family socket address structures for IPv6.
*/
struct ps_sockaddr_in6                
{
  uint16             ps_sin6_family;   /**< Address family for IPv6. */
  uint16             ps_sin6_port;     /**< Transport layer port number. */
  uint32             ps_sin6_flowinfo; /**< IPv6 flow information. */
  /** IPv6 address. */
  struct ps_in6_addr ps_sin6_addr;     
  uint32             ps_sin6_scope_id; /**< Route_scope of the interface. */
};


/*===========================================================================

                          Socket Address Structures

===========================================================================*/
/*---------------------------------------------------------------------------
  The socket address structures follow the BSD convention, including data
  types, etc.  These are the BSD generic address structures needed to
  support Internet-family addressing.
---------------------------------------------------------------------------*/

/**
  @brief BSD generic socket address structure.
*/
struct ps_sockaddr               
{
  uint16         ps_sa_family;    /**< Socket address family. */
  unsigned char  ps_sa_data[14];  /**< Address data. */
};
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/*---------------------------------------------------------------------------
 Definitions used for ps_sockaddr_storage structure padding
---------------------------------------------------------------------------*/
#define PS_SS_MAXSIZE    32               
  /**< Maximum size of the sock address storage.*/
#define PS_SS_ALIGNSIZE  (sizeof(int64))  
  /**< Storage element alignment size of the sock address.*/
#define PS_SS_PADSIZE    ((PS_SS_MAXSIZE) - (2 * (PS_SS_ALIGNSIZE)))
  /**< Storage element padding size of the sock element.*/
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Sock address storage structure.
*/
struct ps_sockaddr_storage
{
 uint16       ps_ss_family;             /**< Address family. */
 int64        ps_ss_align;              /**< Forces alignment of the sock 
                                             address storage. */
 char         ps_ss_pad[PS_SS_PADSIZE]; /**< Padding at the end of the address
                                             for word boundary. */
};

/**
  @brief Non-contiguous buffer structure.
*/
struct ps_iovec
{
  byte  *ps_iov_base;  /**< Starting address of the buffer. */
  uint16 ps_iov_len;   /**< Size of the buffer in bytes. */
};

/*---------------------------------------------------------------------------
TYPEDEF IP_ADDR_ENUM_TYPE
---------------------------------------------------------------------------*/
/**
  Defines all address types.

  @note1hang The values are chosen for easy debugging.
 */
typedef enum
{
  IP_ANY_ADDR     = 0,                        
    /**< Address type can be IPv4 or IPv6. */
  IPV4_ADDR       = 4,                        
    /**< IPv4 address type. */
  IPV6_ADDR       = 6,                        
    /**< IPv6 address type. */
  IP_ADDR_INVALID           = 255,            
    /**< Invalid address type. */
  IFACE_ANY_ADDR_FAMILY     = IP_ANY_ADDR,    
    /**< Address family can be IPv4 or IPv6. */
  IFACE_IPV4_ADDR_FAMILY    = IPV4_ADDR,      
    /**< IPv4 address family. */
  IFACE_IPV6_ADDR_FAMILY    = IPV6_ADDR,      
    /**< IPv6 address family. */
  IFACE_UNSPEC_ADDR_FAMILY  = 8,              
    /**< Unspecified address family. */
  IFACE_INVALID_ADDR_FAMILY = IP_ADDR_INVALID 
    /**< Invalid address family. */
} ip_addr_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF IP_ADDR_TYPE
---------------------------------------------------------------------------*/
/**
  @brief Defines the IP addresses. For use with standard names.
*/
typedef struct ip_address
{
  ip_addr_enum_type type;     /**< Address type. */
  /** IP address structure. */
  union
  {
    uint32 v4;                /**< IPv4 address. */
    uint64 v6[2];             /**< IPv6 address. */
  } addr;

} ip_addr_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================
MACRO IP_ADDR_TYPE_IS_VALID()
===========================================================================*/
/**
  Indicates whether the address type passed in is valid.

  @param[in] addr_type  Address type to be checked.

  @return
  TRUE -- Address type is valid.\n
  FALSE -- Otherwise.
*/
#define IP_ADDR_TYPE_IS_VALID( addr_type )                               \
  ((addr_type) == IP_ANY_ADDR ||                                         \
   (addr_type) == IPV4_ADDR   ||                                         \
   (addr_type) == IPV6_ADDR)
/** @} */  /* end_addtogroup macros_ipUtilities */

/** @addtogroup datatypes
  @{
*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IP_ADDR_TYPE
---------------------------------------------------------------------------*/
/**
  @brief Defines the IP addresses. For use with defines names; recommended.
 */
typedef struct
{
  ip_addr_enum_type type;   /**< Address Type.*/
  /** IP Address. */
  union
  {
    struct ps_in_addr  v4;  /**< IPv4 address. */
    struct ps_in6_addr v6;  /**< IPv6 address. */
  } addr;

} ps_ip_addr_type;

/**
  Indicates the IP version type.
*/
typedef enum
{
  IP_V4 = 4,  /**< IPv4 version. */
  IP_V6 = 6   /**< IPv6 version. */
} ip_version_enum_type;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup macros_ipUtilities
  @{
*/
/*===========================================================================

MACRO PS_HTONL()

===========================================================================*/
/**
  Converts host-to-network long integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x  Unsigned long integer value to be converted.

  @return
  The network byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_htonl) The bit shifting is correct*/
#define ps_htonl(x)                                                      \
  (((((uint32)(x) & 0x000000FFU) << 24) |                                \
  (((uint32)(x) & 0x0000FF00U) <<  8) |                                  \
  (((uint32)(x) & 0x00FF0000U) >>  8) |                                  \
  (((uint32)(x) & 0xFF000000U) >> 24)))

/*===========================================================================

MACRO PS_HTONLL()

===========================================================================*/
/**
  Converts host-to-network long integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x  Unsigned long integer value to be converted.

  @return
  The host byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_htonll) The bit shifting is correct*/
#define ps_htonll(x)                                                     \
  (((((uint64)(x) & 0x00000000000000FFULL) << 56)   |                    \
  (((uint64)(x) & 0x000000000000FF00ULL) << 40)   |                      \
  (((uint64)(x) & 0x0000000000FF0000ULL) << 24)   |                      \
  (((uint64)(x) & 0x00000000FF000000ULL) << 8)    |                      \
  (((uint64)(x) & 0x000000FF00000000ULL) >> 8)    |                      \
  (((uint64)(x) & 0x0000FF0000000000ULL) >> 24)   |                      \
  (((uint64)(x) & 0x00FF000000000000ULL) >> 40)   |                      \
  (((uint64)(x) & 0xFF00000000000000ULL) >> 56)))

/*===========================================================================

MACRO PS_HTONS()

===========================================================================*/
/**
  Converts host-to-network short integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x Unsigned short integer value to be converted.

  @return
  The network byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_htons) The bit shifting is correct*/
#define ps_htons(x)                                                      \
  (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

/*===========================================================================

MACRO PS_NTOHL()

===========================================================================*/
/**
  Converts network-to-host long integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x Unsigned long integer value to be converted.

  @return
  The host byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_ntohl) The bit shifting is correct*/
#define ps_ntohl(x)                                                      \
  (((((uint32)(x) & 0x000000FFU) << 24) |                                \
  (((uint32)(x) & 0x0000FF00U) <<  8) |                                  \
  (((uint32)(x) & 0x00FF0000U) >>  8) |                                  \
  (((uint32)(x) & 0xFF000000U) >> 24)))

/*===========================================================================

MACRO PS_NTOHLL()

===========================================================================*/
/**
  Converts network-to-host long integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x Unsigned long integer value to be converted.

  @return
  The host byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_ntohll) The bit shifting is correct*/
#define ps_ntohll(x)                                                     \
  (((((uint64)(x) & 0x00000000000000FFULL) << 56)   |                    \
  (((uint64)(x) & 0x000000000000FF00ULL) << 40)   |                      \
  (((uint64)(x) & 0x0000000000FF0000ULL) << 24)   |                      \
  (((uint64)(x) & 0x00000000FF000000ULL) << 8)    |                      \
  (((uint64)(x) & 0x000000FF00000000ULL) >> 8)    |                      \
  (((uint64)(x) & 0x0000FF0000000000ULL) >> 24)   |                      \
  (((uint64)(x) & 0x00FF000000000000ULL) >> 40)   |                      \
  (((uint64)(x) & 0xFF00000000000000ULL) >> 56)))

/*===========================================================================

MACRO PS_NTOHS()

===========================================================================*/
/**
  Converts network-to-host short integer. Handles potential byte order
  differences between different computer architectures and different network
  protocols.

  @param[in] x Unsigned short integer value to be converted.

  @return
  The host byte-ordered value.

  @dependencies
  None.
*/
/*lint -emacro(572, ps_ntohs) The bit shifting is correct*/
#define ps_ntohs(x)                                                      \
  (((((uint16)(x) & 0x00FF) << 8) | (((uint16)(x) & 0xFF00) >> 8)))

/*---------------------------------------------------------------------------
 IN6 Macros
---------------------------------------------------------------------------*/
/** Replacement string for zero IPv6 address. */
#define PS_IN6ADDR_ANY_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
/** Replacement string for a loopback IPv6 address. */
#define PS_IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

/** All hosts multicast group FF02::1 general query. */
#define PS_MCAST_ALL_HOSTS_IPV6 { { { 0xF,0xF,0,2,0,0,0,0,0,0,0,0,0,0,0,1 } } }

/** All hosts multicast group FF02::2 done. */
#define PS_MCAST_ALL_ROUTERS_IPV6 { { { 0xF,0xF,0,2,0,0,0,0,0,0,0,0,0,0,0,2 } } }

/**
  Checks whether the IPv6 address is unspecified.
*/
#define PS_IN6_IS_ADDR_UNSPECIFIED(v6)                                      \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == 0 &&                                 \
   ((const unsigned int *) (v6))[3] == 0)

/**
  Checks whether the IPv6 address is a loopback address.
*/
#define PS_IN6_IS_ADDR_LOOPBACK(v6)                                         \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == 0 &&                                 \
   ((const unsigned int *) (v6))[3] == ps_htonl(1))

/** @cond
*/
/**
  TODO: Get the correct macro for this. Not used right now.
 */
#define PS_IN6_IS_ADDR_SCOPE_COMPATIBLE(v6_addr1, v6_addr2)    (TRUE)
/** @endcond */

/**
  Checks whether the IPv6 address is a link local address.
 */
#define PS_IN6_IS_ADDR_LINKLOCAL(v6)                                        \
  ((((const unsigned int *) (v6))[0] & ps_htonl(0xffc00000)) ==             \
     ps_htonl(0xfe800000))

/**
  Checks whether the IPv6 address is a site local address.
 */
#define PS_IN6_IS_ADDR_SITELOCAL(v6)                                        \
  ((((const unsigned int *) (v6))[0] & ps_htonl(0xffc00000)) ==             \
     ps_htonl(0xfec00000))

/**
  Checks whether the IPv6 address is an IPv4 mapped to an IPv6 address.
 */
#define PS_IN6_IS_ADDR_V4MAPPED(v6)                                         \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == ps_htonl(0xffff))

/**
  Checks whether the IPv6 address is an IPv4 compatible with an IPv6 address.
 */
#define PS_IN6_IS_ADDR_V4COMPAT(v6)                                         \
  (((const unsigned int *) (v6))[0] == 0 &&                                 \
   ((const unsigned int *) (v6))[1] == 0 &&                                 \
   ((const unsigned int *) (v6))[2] == 0 &&                                 \
   ps_ntohl(((const unsigned int *) (v6))[3]) > 1)

/**
  Checks whether the given IPv6 addresses are equal.
 */
#define PS_IN6_ARE_ADDR_EQUAL(a,b)                                          \
        ((((const unsigned int *) (a))[0] ==                                \
             ((const unsigned int *) (b))[0])                               \
         && (((const unsigned int *) (a))[1] ==                             \
             ((const unsigned int *) (b))[1])                               \
         && (((const unsigned int *) (a))[2] ==                             \
             ((const unsigned int *) (b))[2])                               \
         && (((const unsigned int *) (a))[3] ==                             \
             ((const unsigned int *) (b))[3]))

/**
  Checks whether the given IPv6 Prefixes are equal.
 */
#define PS_IN6_ARE_PREFIX_EQUAL(a,b)                                        \
        ((((const unsigned int *) (a))[0] ==                                \
             ((const unsigned int *) (b))[0])                               \
         && (((const unsigned int *) (a))[1] ==                             \
             ((const unsigned int *) (b))[1]))                               

/**
  Gets the IPv4 address from the IPv4 mapped to an IPv6 address.
 */
#define PS_IN6_GET_V4_FROM_V4_MAPPED_V6(v6)                                 \
  (PS_IN6_IS_ADDR_V4MAPPED(v6)                                              \
     ? ((const unsigned int *)(v6))[3]                                      \
     : 0 )

/**
  Gets the IPv4 address mapped to an IPv6 address from an IPv4 address.
 */
#define PS_IN6_GET_V4_MAPPED_V6_FROM_V4(v6, v4)                             \
{                                                                           \
  ((unsigned int *)(v6))[0] = 0;                                            \
  ((unsigned int *)(v6))[1] = 0;                                            \
  ((unsigned int *)(v6))[2] = ( v4 != 0) ? ps_htonl(0xFFFF) : 0;            \
  ((unsigned int *)(v6))[3] = v4;                                           \
}

/**
  Creates an IPv4-embedded-IPv6 formatted address from an IPv6 address,
  prefix length and IPv4 address
  RFC 6052 Section 2.2.  IPv4-Embedded IPv6 Address Format
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |PL| 0-------------32--40--48--56--64--72--80--88--96--104---------|
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |32|     prefix    |v4(32)         | u | suffix                    |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |40|     prefix        |v4(24)     | u |(8)| suffix                |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |48|     prefix            |v4(16) | u | (16)  | suffix            |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |56|     prefix                |(8)| u |  v4(24)   | suffix        |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |64|     prefix                    | u |   v4(32)      | suffix    |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |96|     prefix                                    |    v4(32)     |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    v4_in is encoded MSB first, so supply this in host format
 */
#define PS_IN6_GET_V4_EMBEDDED_V6( v6_in, prefix_len_in, v4_in, v6_out )      \
{                                                                             \
  *v6_out = *v6_in;                                                           \
  ((uint8 *)(v6_out))[8] = 0;                                                 \
  ((unsigned int *)(v6_out))[3] = 0;                                          \
  switch( prefix_len_in )                                                     \
  {                                                                           \
    case IPV6_PREFIX_LEN_96:                                                  \
      ((unsigned int *)(v6_out))[3] = ps_htonl(v4_in);                        \
      break;                                                                  \
    case IPV6_PREFIX_LEN_64:                                                  \
      ((unsigned int *)(v6_out))[2] = 0;                                      \
      ((unsigned int *)(v6_out))[2] |= ps_htonl((v4_in & 0xffffff00) >> 8);   \
      ((unsigned int *)(v6_out))[3] |= ps_htonl((v4_in & 0x000000ff) << 24);  \
      break;                                                                  \
    case IPV6_PREFIX_LEN_32:                                                  \
      ((unsigned int *)(v6_out))[1]  = v4_in;                                 \
      ((unsigned int *)(v6_out))[2]  = 0;                                     \
      break;                                                                  \
    case IPV6_PREFIX_LEN_40:                                                  \
      ((unsigned int *)(v6_out))[1] &= ps_htonl(0xff000000);                  \
      ((unsigned int *)(v6_out))[1] |= ps_htonl((v4_in & 0xffffff00) >> 8);   \
      ((unsigned int *)(v6_out))[2] = 0;                                      \
      ((unsigned int *)(v6_out))[2] |= ps_htonl((v4_in & 0x000000ff) << 16);  \
      break;                                                                  \
    case IPV6_PREFIX_LEN_48:                                                  \
      ((unsigned int *)(v6_out))[1] &= ps_htonl(0xffff0000);                  \
      ((unsigned int *)(v6_out))[1] |= ps_htonl((v4_in & 0xffff0000) >> 16);  \
      ((unsigned int *)(v6_out))[2] = 0;                                      \
      ((unsigned int *)(v6_out))[2] |= ps_htonl((v4_in & 0x0000ffff) << 8);   \
      break;                                                                  \
    case IPV6_PREFIX_LEN_56:                                                  \
      ((unsigned int *)(v6_out))[1] &= ps_htonl(0xffffff00);                  \
      ((unsigned int *)(v6_out))[1] |= ps_htonl((v4_in & 0xff000000) >> 24);  \
      ((unsigned int *)(v6_out))[2] = 0;                                      \
      ((unsigned int *)(v6_out))[2] |= ps_htonl((v4_in & 0x00ffffff));        \
      break;                                                                  \
    default:                                                                  \
      ((unsigned int *)(v6_out))[0]  = 0;                                     \
      ((unsigned int *)(v6_out))[1]  = 0;                                     \
      ((unsigned int *)(v6_out))[2]  = 0;                                     \
      ((unsigned int *)(v6_out))[3]  = 0;                                     \
      ASSERT(0);                                                              \
      break;                                                                  \
  } /* switch( prefix_len_in ) */                                             \
}

/**
  Extracts an IPv4 address from an IPv4-embedded-IPv6 formatted address 
  RFC 6052 Section 2.2.  IPv4-Embedded IPv6 Address Format
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |PL| 0-------------32--40--48--56--64--72--80--88--96--104---------|
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |32|     prefix    |v4(32)         | u | suffix                    |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |40|     prefix        |v4(24)     | u |(8)| suffix                |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |48|     prefix            |v4(16) | u | (16)  | suffix            |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |56|     prefix                |(8)| u |  v4(24)   | suffix        |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |64|     prefix                    | u |   v4(32)      | suffix    |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |96|     prefix                                    |    v4(32)     |
    +--+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 */
#define PS_IN6_GET_V4_FROM_V4_EMBEDDED_V6( v6_in, prefix_len_in, v4_out )            \
{                                                                                    \
  *v4_out = 0;                                                                       \
  switch( prefix_len_in )                                                            \
  {                                                                                  \
    case IPV6_PREFIX_LEN_96:                                                         \
      *v4_out = ((unsigned int *)(v6_in))[3];                                        \
      break;                                                                         \
    case IPV6_PREFIX_LEN_64:                                                         \
      *v4_out =                                                                      \
        ps_htonl((((ps_ntohl(((unsigned int *)(v6_in))[2]) & 0x00ffffff) << 8) |     \
                   ((ps_ntohl(((unsigned int *)(v6_in))[3]) & 0xff000000) >> 24) )); \
      break;                                                                         \
    case IPV6_PREFIX_LEN_32:                                                         \
      *v4_out = ((unsigned int *)(v6_in))[1];                                        \
      break;                                                                         \
    case IPV6_PREFIX_LEN_40:                                                         \
      *v4_out =                                                                      \
        ps_htonl((((ps_ntohl(((unsigned int *)(v6_in))[1]) & 0x00ffffff) << 8 ) |    \
                   ((ps_ntohl(((unsigned int *)(v6_in))[2]) & 0x00ff0000) >> 16) )); \
      break;                                                                         \
    case IPV6_PREFIX_LEN_48:                                                         \
      *v4_out =                                                                      \
        ps_htonl((((ps_ntohl(((unsigned int *)(v6_in))[1]) & 0x0000ffff) << 16 ) |   \
                   ((ps_ntohl(((unsigned int *)(v6_in))[2]) & 0x00ffff00) >> 8) ));  \
    break;                                                                           \
    case IPV6_PREFIX_LEN_56:                                                         \
      *v4_out =                                                                      \
        ps_htonl((((ps_ntohl(((unsigned int *)(v6_in))[1]) & 0x000000ff) << 24 ) |   \
                   ((ps_ntohl(((unsigned int *)(v6_in))[2]) & 0x00ffffff) )));       \
      break;                                                                         \
    default:                                                                         \
      ASSERT(0);                                                                     \
      break;                                                                         \
  } /* switch( prefix_len_in ) */                                                    \
}

/**
  Checks whether the specified value is an IPv6 address.
 */
#define PS_IN6_IS_ADDR_V6(v6)                                               \
  (!PS_IN6_IS_ADDR_V4MAPPED(v6) && !PS_IN6_IS_ADDR_UNSPECIFIED(v6))

/**
  Checks whether the IPv6 address is a multicast address.
 */
#define PS_IN6_IS_ADDR_MULTICAST(v6) (((uint8 *) (v6))[0] == 0xFF)

/**
  Checks whether the IPv6 address is a multicast link local address.
 */
#define PS_IN6_IS_ADDR_MC_LINKLOCAL(v6)                                     \
	(PS_IN6_IS_ADDR_MULTICAST(v6) && ((((uint8 *) (v6))[1] & 0x0F) == 0x2))

/**
  Checks whether the IPv6 address is a multicast site local address.
 */
#define PS_IN6_IS_ADDR_MC_SITELOCAL(v6)                                     \
	(PS_IN6_IS_ADDR_MULTICAST(v6) && ((((uint8 *) (v6))[1] & 0x0F) == 0x5))

/**
  Checks whether the IPv6 address is a multicast global address.
 */
#define PS_IN6_IS_ADDR_MC_GLOBAL(v6)                                        \
	(PS_IN6_IS_ADDR_MULTICAST(v6) && ((((uint8 *) (v6))[1] & 0x0F) == 0xe))

/**
  Checks whether the prefix is link local.
 */
#define PS_IN6_IS_PREFIX_LINKLOCAL(v6)                                      \
  ((ps_htonl(v6) & 0xffff0000) == 0xfe800000)

/**
  Checks whether the IPv4 address mapped to an IPv6 address is a multicast address.
 */
#define PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST(v6)                           \
  ( PS_IN6_IS_ADDR_V4MAPPED(v6) && PS_IN_IS_ADDR_MULTICAST(((int *)(v6))[3]) )

/** Destination is a multicast address when in the range of
  224.0.0.0 to 239.255.255.255 (i.e. address starts with 0xE).
*/
#define PS_IN_IS_ADDR_MULTICAST(v4)                                         \
  ((((unsigned long )(v4)) & ps_htonl(0xF0000000)) == ps_htonl(0xE0000000))

/** Destination is a broadcast address when:\n
  - Destination address ends with 0xFF (limited broadcast).\n
    @note1hang This macro only checks for this case.

  - Destination address is the subnet broadcast address, which is basically the 
    subnet number with all host bits set to 1.\n
    @note1hang The caller must check for this case.
*/
#define PS_IN_IS_ADDR_LIMITED_BROADCAST(v4)                                 \
  ((((unsigned long )(v4)) & ps_htonl(0x000000FF)) == ps_htonl(0x000000FF))
/** @} */ /* end_addtogroup macros_ipUtilities */

/** @addtogroup constants
  @{
*/
/** Upper bound on the memory for the presentation (printable) format of the  
  IPv4 address. Valid value: sizeof is 255.255.255.255.
*/
#define PS_IN_ADDRSTRLEN  16
/** Upper bound on the memory for the presentation (printable) format of the  
  IPv6 address. Valid value: sizeof is ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255.
*/
#define PS_IN6_ADDRSTRLEN 46
/** @} */ /* end_addtogroup constants */


#endif /* PS_IN_H */

