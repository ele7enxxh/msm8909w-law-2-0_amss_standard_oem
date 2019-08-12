#ifndef PS_IN_ALIAS_H
#define PS_IN_ALIAS_H
/*===========================================================================

                   P S _ I N _ A L I A S _ H

===========================================================================*/
/**
  @file ps_in_alias.h
  @brief Provides featurized aliases for type names in ps_in.h and
  dssock_defs.h.

  To resolve name conflicts when CDPS stack is built with third-party module
  which also contain data stack (e.g, Linux, Windows), some CDPS data/data
  type names are prefixed with either "ps_/PS_" (in ps_in.h) and "dss_" in
  (dssock_defs.h).

  For example, "struct in_addr" is now "struct ps_in_addr" in ps_in.h.

  Moving forward,the preferred types are the ones such as ps_in_addr, and
  the use of the previous types such as in_addr is deprecated with the
  intention of removing it in the future.

  Note:
    1. Per CMI rule, no feature is allowed in api files
    2. ###--- Special Exception Is Made for This File ---###
        This file is featurized with feature FEATURE_DATA_PS_IN_ALIASES
    3. The reason being:
       a. The aliases in this file are needed for building apps which already
          use types such as "in_addr". For example, 6K targets, BMP targets ...
       b. The approach of using header guards in place of the feature is not
          adopted since it has un-acceptable side effects. The order of
          including CDPS stack and third-party stack matters.

          For example, if we use header guard for "winsock.h": _WINSOCKAPI_ in
          stead of the featuer string here, when build CDPS with Windows
          platform, "windows.h" must be included ahead of "ps_in.h" by the
          user. Otherwise, the type name conflicts remain.

  Usage: For off-target builds,
    a. feature  FEATURE_DATA_PS_IN_ALIASES should be disabled,
    b. the user of the CDPS stack should use names with prefix, e.g.,
       ps_in_addr (see ps_in_alias.h for the affected names).

  On-target CDPS users should use the original version for the affected names.

  Usage: For AMSS target builds,
    a. feature  FEATURE_DATA_PS_IN_ALIASES should be enabled,
    b. the user of the CDPS stack can use names without prefix, e.g., in_addr
       (see ps_in_alias.h for the affected names).
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
Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_in_alias.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
10/13/09    ts     Renamed INET/6_ADDRSTRLEN to PS_IN/6_ADDRSTRLEN
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
03/11/09    jy     Changes to avoid ps/ds type name conflicts when build CDPS
                   with third-part software modules containing a data stack
===========================================================================*/
/* group: ipUtilities */

/** @addtogroup constants 
  @{
*/
/*  mapping enums of type ps_ip_protocol_enum_type in ps_in.h */
#define IPV6_BASE_HDR              PS_IPV6_BASE_HDR
   /**< Maps the PS_IPV6_BASE_HDR enumerator in ps_in.h. */
#define IPPROTO_HOP_BY_HOP_OPT_HDR PS_IPPROTO_HOP_BY_HOP_OPT_HDR
   /**< Maps the PS_IPPROTO_HOP_BY_HOP_OPT_HDR enumerator in ps_in.h. */
#define IPPROTO_ICMP               PS_IPPROTO_ICMP
   /**< Maps the PS_IPPROTO_ICMP enumerator in ps_in.h. */
#define IPPROTO_IGMP               PS_IPPROTO_IGMP
   /**< Maps the PS_IPPROTO_IGMP enumerator in ps_in.h. */
#define IPPROTO_IP                 PS_IPPROTO_IP
   /**< Maps the PS_IPPROTO_IP enumerator in ps_in.h. */
#define IPPROTO_TCP                PS_IPPROTO_TCP
   /**< Maps the PS_IPPROTO_TCP enumerator in ps_in.h. */
#define IPPROTO_UDP                PS_IPPROTO_UDP
   /**< Maps the PS_IPPROTO_UDP enumerator in ps_in.h. */
#define IPPROTO_IPV6               PS_IPPROTO_IPV6
   /**< Maps the PS_IPPROTO_IPV6 enumerator in ps_in.h. */
#define IPPROTO_ROUTING_HDR        PS_IPPROTO_ROUTING_HDR
   /**< Maps the PS_IPPROTO_ROUTING_HDR enumerator in ps_in.h. */
#define IPPROTO_FRAG_HDR           PS_IPPROTO_FRAG_HDR
   /**< Maps the PS_IPPROTO_FRAG_HDR enumerator in ps_in.h. */
#define IPPROTO_ESP                PS_IPPROTO_ESP
   /**< Maps the PS_IPPROTO_ESP enumerator in ps_in.h. */
#define IPPROTO_AH                 PS_IPPROTO_AH
   /**< Maps the PS_IPPROTO_AH enumerator in ps_in.h. */
#define IPPROTO_ICMP6              PS_IPPROTO_ICMP6
   /**< Maps the PS_IPPROTO_ICMP6 enumerator in ps_in.h. */
#define NO_NEXT_HDR                PS_NO_NEXT_HDR
   /**< Maps the PS_NO_NEXT_HDR enumerator in ps_in.h. */
#define IPPROTO_DEST_OPT_HDR       PS_IPPROTO_DEST_OPT_HDR
   /**< Maps the PS_IPPROTO_DEST_OPT_HDR enumerator in ps_in.h. */
#define IPPROTO_MOBILITY_HDR       PS_IPPROTO_MOBILITY_HDR
   /**< Maps the PS_IPPROTO_MOBILITY_HDR enumerator in ps_in.h. */
#define ip_protocol_enum_type      ps_ip_protocol_enum_type
   /**< Maps ps_ip_protocol_enum_type in ps_in.h. */

/* map struct type and its data members in ps_in.h*/
#define in_addr          ps_in_addr            
  /**< Maps the ps_in_addr structure type in ps_in.h. */
#define s_addr           ps_s_addr             
  /**< Maps the ps_s_addr data member in ps_in.h. */
#define s6_addr          ps_s6_addr            
  /**< Maps the ps_s6_addr data member in ps_in.h. */
#define s6_addr16        ps_s6_addr16          
  /**< Maps the ps_s6_addr16 data member in ps_in.h. */
#define s6_addr32        ps_s6_addr32          
  /**< Maps the ps_s6_addr32 data member in ps_in.h. */
#define s6_addr64        ps_s6_addr64          
  /**< Maps the ps_s6_addr64 data member in ps_in.h. */

/* map enum in ps_in.h */
#define INADDR_ANY       PS_INADDR_ANY     
  /**< Maps the PS_INADDR_ANY enumerator in ps_in.h. */
#define INET_ADDRSTRLEN  PS_IN_ADDRSTRLEN  
  /**< Maps the PS_IN_ADDRSTRLEN enumerator in ps_in.h. */


#define IN6ADDR_ANY_INIT               PS_IN6ADDR_ANY_INIT
   /**< Maps the PS_IN6ADDR_ANY_INIT enumerator in ps_in.h. */
#define IN6ADDR_LOOPBACK_INIT         PS_IN6ADDR_LOOPBACK_INIT
   /**< Maps the PS_IN6ADDR_LOOPBACK_INIT enumerator in ps_in.h. */
#define IN6_IS_ADDR_UNSPECIFIED       PS_IN6_IS_ADDR_UNSPECIFIED
   /**< Maps the PS_IN6_IS_ADDR_UNSPECIFIED enumerator in ps_in.h. */
#define IN6_IS_ADDR_LOOPBACK          PS_IN6_IS_ADDR_LOOPBACK
   /**< Maps the PS_IN6_IS_ADDR_LOOPBACK enumerator in ps_in.h. */
#define IN6_IS_ADDR_SCOPE_COMPATIBLE PS_IN6_IS_ADDR_SCOPE_COMPATIBLE
   /**< Maps the PS_IN6_IS_ADDR_SCOPE_COMPATIBLE enumerator in ps_in.h. */
#define IN6_IS_ADDR_LINKLOCAL         PS_IN6_IS_ADDR_LINKLOCAL
   /**< Maps the PS_IN6_IS_ADDR_LINKLOCAL enumerator in ps_in.h. */
#define IN6_IS_ADDR_SITELOCAL         PS_IN6_IS_ADDR_SITELOCAL
   /**< Maps the PS_IN6_IS_ADDR_SITELOCAL enumerator in ps_in.h. */
#define IN6_IS_ADDR_V4MAPPED          PS_IN6_IS_ADDR_V4MAPPED
   /**< Maps the PS_IN6_IS_ADDR_V4MAPPED enumerator in ps_in.h. */
#define IN6_IS_ADDR_V4COMPAT          PS_IN6_IS_ADDR_V4COMPAT
   /**< Maps the PS_IN6_IS_ADDR_V4COMPAT enumerator in ps_in.h. */
#define IN6_ARE_ADDR_EQUAL            PS_IN6_ARE_ADDR_EQUAL
   /**< Maps the PS_IN6_ARE_ADDR_EQUAL enumerator in ps_in.h. */
#define IN6_GET_V4_FROM_V4_MAPPED_V6 PS_IN6_GET_V4_FROM_V4_MAPPED_V6
   /**< Maps the PS_IN6_GET_V4_FROM_V4_MAPPED_V6 enumerator in ps_in.h. */
#define IN6_GET_V4_MAPPED_V6_FROM_V4 PS_IN6_GET_V4_MAPPED_V6_FROM_V4
   /**< Maps the PS_IN6_GET_V4_MAPPED_V6_FROM_V4 enumerator in ps_in.h. */
#define IN6_IS_ADDR_V6                 PS_IN6_IS_ADDR_V6
   /**< Maps the PS_IN6_IS_ADDR_V6 enumerator in ps_in.h. */
#define IN6_IS_ADDR_MULTICAST         PS_IN6_IS_ADDR_MULTICAST
   /**< Maps the PS_IN6_IS_ADDR_MULTICAST enumerator in ps_in.h. */
#define IN6_IS_ADDR_MC_LINKLOCAL      PS_IN6_IS_ADDR_MC_LINKLOCAL
   /**< Maps the PS_IN6_IS_ADDR_MC_LINKLOCAL enumerator in ps_in.h. */
#define IN6_IS_ADDR_MC_SITELOCAL      PS_IN6_IS_ADDR_MC_SITELOCAL
   /**< Maps the PS_IN6_IS_ADDR_MC_SITELOCAL enumerator in ps_in.h. */
#define IN6_IS_ADDR_MC_GLOBAL         PS_IN6_IS_ADDR_MC_GLOBAL
   /**< Maps the PS_IN6_IS_ADDR_MC_GLOBAL enumerator in ps_in.h. */
#define IN6_IS_PREFIX_LINKLOCAL       PS_IN6_IS_PREFIX_LINKLOCAL
   /**< Maps the PS_IN6_IS_PREFIX_LINKLOCAL enumerator in ps_in.h. */
#define IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST
   /**< Maps the PS_IN6_IS_V4_MAPPED_V6_ADDR_MULTICAST enumerator in ps_in.h. */
#define IN_IS_ADDR_MULTICAST          PS_IN_IS_ADDR_MULTICAST
   /**< Maps the PS_IN_IS_ADDR_MULTICAST enumerator in ps_in.h. */
#define IN_IS_ADDR_LIMITED_BROADCAST PS_IN_IS_ADDR_LIMITED_BROADCAST
   /**< Maps the PS_IN_IS_ADDR_LIMITED_BROADCAST enumerator in ps_in.h. */
#define INET6_ADDRSTRLEN              PS_IN6_ADDRSTRLEN
   /**< Maps the PS_IN6_ADDRSTRLEN enumerator in ps_in.h. */


/**
  Maps the ps_in6_addr structure type in ps_in.h.
*/
#define in6_addr         ps_in6_addr

/**
  Maps the ps_in6addr_any external global variables in ps_in.h.
*/
#define in6addr_any      ps_in6addr_any
/**
  Maps the ps_in6addr_loopback external global variables in ps_in.h.
*/
#define in6addr_loopback ps_in6addr_loopback

/* map struct type and its data members in ps_in.h */
#define sockaddr_in      ps_sockaddr_in        
  /**< Maps the ps_sockaddr_in structure type in ps_in.h. */
#define sin_family       ps_sin_family         
  /**< Maps the ps_sin_family data member in ps_in.h. */
#define sin_port         ps_sin_port           
  /**< Maps the ps_sin_port data member in ps_in.h. */
#define sin_addr         ps_sin_addr           
  /**< Maps the ps_sin_addr data member in ps_in.h. */
#define sin_zero         ps_sin_zero           
  /**< Maps the ps_sin_zero data member in ps_in.h. */

/* map struct type and its data members in ps_in.h */
#define sockaddr_in6     ps_sockaddr_in6       
  /**< Maps the ps_sockaddr_in6 structure type in ps_in.h. */
#define sin6_family      ps_sin6_family        
  /**< Maps the ps_sin6_family data member in ps_in.h. */
#define sin6_port        ps_sin6_port          
  /**< Maps the ps_sin6_port data member in ps_in.h. */
#define sin6_flowinfo    ps_sin6_flowinfo      
  /**< Maps the ps_sin6_flowinfo data member in ps_in.h. */
#define sin6_addr        ps_sin6_addr          
  /**< Maps the ps_sin6_addr data member in ps_in.h. */
#define sin6_scope_id    ps_sin6_scope_id      
  /**< Maps the ps_sin6_scope_id data member in ps_in.h. */

/* map struct type and its data memebers in ps_in.h */
#define sockaddr         ps_sockaddr           
  /**< Maps the ps_sockaddr structure type in ps_in.h. */
#define sa_family        ps_sa_family          
  /**< Maps the ps_sa_family data member in ps_in.h. */
#define sa_data          ps_sa_data            
  /**< Maps the ps_sa_data data member in ps_in.h. */

/* map struct type and its data memebers in ps_in.h */
#define _SS_MAXSIZE      PS_SS_MAXSIZE         
  /**< Maps PS_SS_MAXSIZE in ps_in.h. */
#define _SS_ALIGNSIZE    PS_SS_ALIGNSIZE       
  /**< Maps PS_SS_ALIGNSIZE in ps_in.h. */
#define _SS_PADSIZE      PS_SS_PADSIZE         
  /**< Maps PS_SS_PADSIZE in ps_in.h. */
#define sockaddr_storage ps_sockaddr_storage   
  /**< Maps the ps_sockaddr_storage structure type in ps_in.h. */
#define _ss_align        ps_ss_align           
  /**< Maps the ps_ss_align data member in ps_in.h. */
#define _ss_pad          ps_ss_pad             
  /**< Maps the ps_ss_pad data member in ps_in.h. */
#define ss_family        ps_ss_family          
  /**< Maps the ps_ss_family data member in ps_in.h. */

/* map struct type and its data memebers in ps_in.h */
#define iovec            ps_iovec              
  /**< Maps the ps_iovec structure type in ps_in.h. */
#define iov_base         ps_iov_base           
  /**< Maps the ps_iov_base data member in ps_in.h. */
#define iov_len          ps_iov_len            
  /**< Maps the ps_iov_len data member in ps_in.h. */

/**
  Maps PF_INET to DSS_PF_INET in dssock_defs.h.
 */
#define PF_INET          DSS_PF_INET
/**
  Maps PF_INET6 to DSS_PF_INET6 in dssock_defs.h.
 */
#define PF_INET6         DSS_PF_INET6

/* map enums in dssock_defs.h */
#define AF_INET          DSS_AF_INET   
  /**< Maps the DSS_AF_INET enumerator in dssock_defs.h. */
#define AF_INET6         DSS_AF_INET6  
  /**< Maps the DSS_AF_INET6 enumerator in dssock_defs.h. */
#define AF_UNSPEC        DSS_AF_UNSPEC 
  /**< Maps the DSS_AF_UNSPEC enumerator in dssock_defs.h. */
#define AF_ANY           DSS_AF_ANY    
  /**< Maps the DSS_AF_ANY enumerator in dssock_defs.h. */

/**
  Maps SOCK_STREAM to DSS_SOCK_STREAM in dssock_defs.h.
 */
#define SOCK_STREAM      DSS_SOCK_STREAM
/**
  Maps SOCK_DGRAM to DSS_SOCK_DGRAM in dssock_defs.h.
 */
#define SOCK_DGRAM       DSS_SOCK_DGRAM


/**
  Maps the htonl definition to the ps_htonl definition in ps_in.h.
*/
#ifndef htonl
#define htonl(x) ps_htonl(x)
#endif /* htonl */

/**
  Maps the htonll definition to the ps_htonll definition in ps_in.h.
*/
#define htonll(x) ps_htonll(x)

/**
  Maps the htons definition to the ps_htons definition in ps_in.h.
*/
#ifndef htons
#define htons(x) ps_htons(x)
#endif /* htons */

/**
  Maps the ntohl definition to the ps_ntohl definition in ps_in.h.
*/
#ifndef ntohl
#define ntohl(x) ps_ntohl(x)
#endif /* htonl */

/**
  Maps the ntohll definition to the ps_ntohll definition in ps_in.h.
*/
#define ntohll(x) ps_ntohll(x)

/**
  Maps the ntohs definition to the ps_ntohs definition in ps_in.h.
*/
#ifndef ntohs
#define ntohs(x) ps_ntohs(x)
#endif /* htons */

/** @} */ /* end_addtogroup constants */

#endif /* PS_IN_ALIAS_H */
