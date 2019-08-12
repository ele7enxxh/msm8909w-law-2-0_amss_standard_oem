#ifndef PS_IFACE_ADDR_V6_H
#define PS_IFACE_ADDR_V6_H
/** 
  @file ps_iface_addr_v6.h 
  @brief 
   Contains common data declarations and function prototypes for 
   accessing IPv6 address(s).
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E _ A D D R _ V 6 . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Interface Layer Functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the tcpUdpIpIp6 group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2007-2015 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_iface_addr_v6.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

03/24/15    skc    Added PS_IFACE_SET_V6_IID_EX api
08/18/14    cx     Added allocate timestamp in v6 address structure
02/10/12    rt     Name change, rm_iface_ptr to linked_iface_ptr
11/28/11    cp     Added API to get only v6 SIP server addresses.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.

===========================================================================*/
/* group: tcpUdpIpIp6 */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "pstimer.h"
#include "ps_iface_defs.h"
#include "ps_iface_ipfltr.h"

/** @addtogroup datatypes 
  @{
*/
/**
@brief Stores the PS interface addresses.
*/
typedef struct
{
  uint64  prefix;                                 
    /**< IPv6 prefix.           */
  uint64  iid;                                    
    /**< IPv6 IID.              */
  uint64  gateway_iid;                            
    /**< Gateway from which the prefix is acquired.       */
  ps_timer_handle_type  pref_lifetimer_handle;    
    /**< Preferred lifetime.    */
  ps_timer_handle_type  valid_lifetimer_handle;   
    /**< Valid lifetime.        */
  ps_timer_handle_type  unused_addr_timer_handle; 
    /**< Timer for the duration of time a privacy address can remain unused. If 
         no application binds before the timer expires, the address is freed. */
  ps_iface_ipv6_addr_state_enum_type  addr_state; 
    /**< IPv6 address state.    */
  ps_iface_ipv6_addr_type_enum_type   addr_type;  
    /**< IPv6 address type.     */
  uint8   ref_cnt;                                
    /**< Privacy extensions reference count. */
  uint8   prefix_len;                             
    /**< Length of the prefix.  */
  uint8   dad_retries;                            
    /**< Number of DAD attempts remaining. */

  ps_iface_type *linked_iface_ptr;                    
    /**< Pointer to the "linked" interface, e.g. Rm iface for an EXTERNAL
         address; WWAN iface for Um addrs on soft-ap iface, and NULL for
         Global/Privacy address. */

  void   * hw_fltr_handle;
    /**< Filter handle corresponding to default v6 destination based 
         filter installed when particular v6 address comes up or its 
         filter handle representing fragmentation filter when IPV6 
         address is of type external IPV6 address */ 
  
  uint32 alloc_timestamp;
    /**< The timestamp when the address is allocated>**/
} ps_ifacei_v6_addr_type;

/** @} */ /* end_addtogroup datatypes */

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/** @addtogroup macros_tcpUdpIpIp6
  @{
*/

/*===========================================================================
PS_IPV6_INFINITE_LIFE_TIME
===========================================================================*/
/** 
  @brief
  Defines the infinite life time constant.
 
  PS_IPV6_INFINITE_LIFE_TIME - Infinite life time

*/
#define PS_IPV6_INFINITE_LIFE_TIME  0xFFFFFFFFUL

/*===========================================================================
MACRO IPV6_ADDR_MSG()
===========================================================================*/
/**
  Prints an IPv6 address.

  @param[in] ip_addr   IPv6 address in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_ADDR_MSG(ip_addr) MSG_8(MSG_SSID_DS, \
                        MSG_LEGACY_HIGH, \
                        "IPV6 Address %x:%x:%x:%x:%x:%x:%x:%x", \
                        (uint16)(ps_ntohs(ip_addr[0])), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[0] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[0] >> 48)), \
                        (uint16)(ps_ntohs(ip_addr[1])), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 16)), \
                        (uint16)(ps_ntohs(ip_addr[1] >> 32)) , \
                        (uint16)(ps_ntohs(ip_addr[1] >> 48)))

/*===========================================================================
MACRO IPV6_PREFIX_MSG()
===========================================================================*/
/**
  Prints an IPv6 Prefix.

  @param[in] prefix   The IPv6 Prefix in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_PREFIX_MSG(prefix) MSG_4( MSG_SSID_DS, \
                                       MSG_LEGACY_HIGH, \
                                       "Prefix is %x:%x:%x:%x", \
                                       (uint16)(ps_ntohs(prefix)), \
                                       (uint16)(ps_ntohs(prefix >> 16)), \
                                       (uint16)(ps_ntohs(prefix >> 32)) , \
                                       (uint16)(ps_ntohs(prefix >> 48)))


/*===========================================================================
MACRO IPV6_IID_MSG()
===========================================================================*/
/**
  Prints an IPv6 IID.

  @param[in] iid   The IPv6 IID in network byte order.

  @return
  None.

  @dependencies 
  None.
*/
#define IPV6_IID_MSG(iid) MSG_4( MSG_SSID_DS, \
                                 MSG_LEGACY_HIGH, \
                                 "IID is %x:%x:%x:%x", \
                                 (uint16)(ps_ntohs(iid)), \
                                 (uint16)(ps_ntohs(iid >> 16)), \
                                 (uint16)(ps_ntohs(iid >> 32)) , \
                                 (uint16)(ps_ntohs(iid >> 48)))


/*===========================================================================
MACRO PS_IFACE_GET_IP_V6_IID()
===========================================================================*/
/**
  Returns the IPv6 interface identifier (the v6 address suffix)
  assigned to the interface.

  @note1hang This is not the same as ps_iface.

  @param[in] iface_ptr   Pointer to the ps_iface interface.

  @return
  IPv6 interface identifier (last 64 bits of the address).\n
  0 -- Interface pointer is NULL.

  @dependencies 
  None.
*/
#define PS_IFACE_GET_IP_V6_IID( iface_ptr )                     \
ps_iface_get_v6_iid(iface_ptr)

/*===========================================================================
MACRO PS_IFACE_GET_IP_V6_PREFIX()
===========================================================================*/
/**
  Returns the IPv6 prefix (the v6 address prefix)
  assigned to the interface.

  @note1hang This is not the same as ps_iface.

  @param[in] iface_ptr   Pointer to the ps_iface interface.

  @return
  IPv6 address prefix (first 64 bits of the address).\n
  0 -- Interface pointer is NULL.

  @dependencies 
  None.
*/
#define PS_IFACE_GET_IP_V6_PREFIX( iface_ptr )                     \
ps_iface_get_v6_prefix(iface_ptr)

/*===========================================================================
MACRO PS_IFACE_V6_ADDR_MATCH
===========================================================================*/
/**
  Matches the passed IPv6 address with the possible IPv6 addresses of the 
  passed interface.

  @param[in] v6_addr_ptr   Pointer to IPv6 address to match.
  @param[in] if_ptr        Interface pointer.

  @return
  TRUE  -- Passed address matches any of the IPv6 addresses of the interface.\n
  FALSE -- Otherwise.

  @dependencies 
  None.
*/
#define PS_IFACE_V6_ADDR_MATCH( v6_addr_ptr, if_ptr )              \
ps_iface_v6_addr_match( v6_addr_ptr, if_ptr)

/*===========================================================================
MACRO PS_IFACE_SET_IP_V6_IID()
===========================================================================*/
/**
  Sets the IPv6 interface identifier (the v6 address suffix) in
  the interface.

  @param[in] iface_ptr   Pointer to the interface.
  @param[in] iid 6       4-bit IPv6 interface identifier. The IPv6 address 
                         suffix.

  @return
  FALSE -- Interface pointer is NULL.\n
  TRUE  -- Otherwise.
  
  @dependencies 
  None.
*/
#define PS_IFACE_SET_IP_V6_IID( iface_ptr, iid )                        \
 ps_iface_set_v6_iid(iface_ptr,iid)

/*===========================================================================
MACRO PS_IFACE_SET_PREFIX_CACHE()
===========================================================================*/
/**
  Sets the prefix cache in the interface.

  @param[in] iface_ptr   Pointer to the interface.
  @param[in] prefix      64-bit IPv6 interface prefix. 

  @return
  0 for success
 -1 for error
  
  @dependencies 
  None.
*/
#define PS_IFACE_SET_PREFIX_CACHE( iface_ptr, prefix, prefix_len )         \
 ps_iface_addr_mgmt_set_prefix_cache( iface_ptr, prefix, prefix_len )

/** @} */ /* end_addtogroup macros_tcpUdpIpIp6 */
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_IFACE_GET_ALL_V6_PREFIXES()
===========================================================================*/
/** @ingroup ps_iface_get_all_v6_prefixes
  Retrieves all of the prefixes on an interface along with the state and length 
  of each prefix.

  @datatypes
  #ps_iface_type\n
  #ps_iface_prefix_info_type

  @vertspace
  @param[in] this_iface_ptr Pointer to the interface on which to cleanup the
                            neighbor discovery caches.
  @param[in] prefix_info    Pointer to the prefix name, state, and length.
  @param[in] num_prefixes   Pointer to the space allocated for prefixes and the
                            number passed back.

  @return
  None.
  
  @dependencies 
  None.
  @newpage
*/
void ps_iface_get_all_v6_prefixes
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_prefix_info_type *prefix_info,
  uint8                     *num_prefixes
);

/*===========================================================================
FUNCTION PS_IFACE_APPLY_V6_PREFIX()
===========================================================================*/
/** @ingroup ps_iface_apply_v6_prefix
  Applies a prefix to a particular interface. The initial instance only stores 
  a single prefix. 

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block on
                              which the prefix is to be applied.
  @param[in] gateway_iid      Interface ID of the router.
  @param[in] prefix           Prefix being added.
  @param[in] valid_lifetime   Lifetime of prefix (seconds). Refer to RFC 2461 
                              @xhyperref{S12,[S12]}, Section 4.6.2.
  @param[in] pref_lifetime    Preferred lifetime for prefix. Refer to RFC 2462 
                              @xhyperref{S13,[S13]}, Section 2.
  @param[in] prefix_length    Length of the prefix.
 
  @return
   0 -- Prefix was applied successfully.\n
  -1 -- Failure, or the prefix was not applied.
  
  @dependencies 
  None.
  @newpage
*/
int ps_iface_apply_v6_prefix
(
  ps_iface_type *this_iface_ptr,
  uint64         gateway_iid,
  uint64         prefix,
  uint32         valid_lifetime,
  uint32         pref_lifetime,
  uint8          prefix_length
);

/*===========================================================================
FUNCTION PS_IFACE_APPLY_V6_PREFIX_EX()
===========================================================================*/
/** @ingroup ps_iface_apply_v6_prefix_ex
  Applies a prefix to a particular interface. The initial instance only stores 
  a single prefix. 

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block on
                              which the prefix is to be applied.
  @param[in] gateway_iid      Interface ID of the router.
  @param[in] prefix           Prefix being added.
  @param[in] valid_lifetime   Lifetime of prefix (seconds). Refer to RFC 2461 
                              @xhyperref{S12,[S12]}, Section 4.6.2.
  @param[in] pref_lifetime    Preferred lifetime for prefix. Refer to RFC 2462 
                              @xhyperref{S13,[S13]}, Section 2.
  @param[in] prefix_length    Length of the prefix.
  @param[in] addfltr          flag to indicate whether to add HW filter
                              for primary address.
 
  @return
   0 -- Prefix was applied successfully.\n
  -1 -- Failure, or the prefix was not applied.
  
  @dependencies 
  None.
  @newpage
*/
int ps_iface_apply_v6_prefix_ex
(
  ps_iface_type *this_iface_ptr,
  uint64         gateway_iid,
  uint64         prefix,
  uint32         valid_lifetime,
  uint32         pref_lifetime,
  uint8          prefix_length,
  boolean        addfltr
);

/*===========================================================================
FUNCTION PS_IFACE_REMOVE_V6_PREFIX()
===========================================================================*/
/** @ingroup ps_iface_remove_v6_prefix
  Removes a prefix from the interface. The function fails only when the prefix 
  does not exist on this interface.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block on
                              which the prefix is to be removed.
  @param[in] prefix           Prefix being removed.
 
  @return
   0 -- Prefix is removed successfully.\n
  -1 -- Failure.
  
  @dependencies 
  None.
  @newpage
*/
int ps_iface_remove_v6_prefix
(
  ps_iface_type *this_iface_ptr,
  uint64         prefix
);

/*===========================================================================
FUNCTION PS_IFACE_DELETE_ALL_V6_PREFIXES()
===========================================================================*/
/** @ingroup ps_iface_delete_all_v6_prefixes
  Removes all prefixes associated with the interface.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block on
                              which all prefixes are to be removed.
 
  @return
   0 -- All prefixes are removed successfully.\n
  -1 -- Failure.
  
  @dependencies 
  None.
  @newpage
*/
int ps_iface_delete_all_v6_prefixes
(
  ps_iface_type *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GENERATE_IPV6_IID()
===========================================================================*/
/** @ingroup ps_iface_generate_ipv6_iid
  Generates a random IPv6 IID. This function ensures that the IID generated
  is unique on the interface and begins the DAD, if necessary.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in]  this_iface_ptr   Pointer to the interface control block for
                               which a random IPv6 IID is to be generated.
  @param[out] iid              Pointer to the IID to be returned by
                               this function.
  @param[out] ps_errno         Pointer to the error code when an error occurs.
 
  @return
  None.
  
  @dependencies 
  None.
  @newpage
*/
int ps_iface_generate_ipv6_iid
(
  ps_iface_type *this_iface_ptr, 
  uint64        *iid,            
  int16         *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_IID
===========================================================================*/
/** @ingroup ps_iface_get_v6_iid
  Returns the IPv6 IID of an interface. If the interface is NULL or IPv4, this
  function returns NULL.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in]  this_iface_ptr   Pointer to the target interface.
 
  @return
  IPv6 interface identifier. The last 64 bits of the address.\n
  0 -- Interface pointer is NULL, or interface is IPv4 family.
  
  @dependencies 
  None.
  @newpage
*/
uint64 ps_iface_get_v6_iid
(
  ps_iface_type       *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_PREFIX
===========================================================================*/
/** @ingroup ps_iface_get_v6_prefix
  Returns the IPv6 prefix of an interface. If the interface is NULL or IPv4, this
  function returns NULL.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in]  this_iface_ptr   Pointer to the target interface.
 
  @return
  IPv6 prefix. The first 64 bits of the address.\n
  0 -- Interface pointer is NULL, or interface is IPv4 family.
  
  @dependencies 
  None.
  @newpage
*/
uint64 ps_iface_get_v6_prefix
(
  ps_iface_type       *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_LINKLOCAL_ADDR()
===========================================================================*/
/** @ingroup ps_iface_get_linklocal_addr
  Gets the link local address of the IPv6 interface. This function sets the 
  address type to invalid if the call fails.

  @datatypes
  #ps_iface_type\n
  #ps_ip_addr_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block
                              for which the link local address is obtained.
  @param[out] ip_addr_ptr     Pointer to the link local IPv6 address.
 
  @return
  None.
  
  @dependencies 
  None.
  @newpage
*/
void ps_iface_get_linklocal_addr
(
  ps_iface_type    *this_iface_ptr,
  ps_ip_addr_type  *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_V6_ADDR_MATCH
===========================================================================*/
/** @ingroup ps_iface_v6_addr_match
  Matches the passed IPv6 address with the possible IPv6 addresses of the 
  passed interface.

  @datatypes
  #ps_in6_addr\n
  #ps_iface_type

  @vertspace
  @param[in] v6_addr_ptr      Pointer to the IPv6 address to match.
  @param[in] if_ptr           Pointer to the interface control block.
 
  @return
  TRUE  -- Passed address matches any of the IPv6 addresses of the interface.\n
  FALSE -- Otherwise.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_iface_v6_addr_match
(
  struct ps_in6_addr *v6_addr_ptr,
  ps_iface_type   *if_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_V6_IID_EX
===========================================================================*/
/** @ingroup ps_iface_set_v6_iid_ex
  Sets the IPv6 IID of an interface. If the interface is NULL or IPv4, this 
  function returns FALSE.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface.
  @param[in] iid              64-bit IPv6 interface identifier.
    						  The IPv6 address suffix.
  @param[in] addfltr          Flag to indicate whether to add IID based
                              filter or not.

 
  @return
  FALSE -- Interface pointer is NULL, IPv4, or Logical.\n
  TRUE -- Otherwise.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_iface_set_v6_iid_ex
(
  ps_iface_type       *this_iface_ptr,
  uint64               iid,
  boolean              addfltr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_V6_IID
===========================================================================*/
/** @ingroup ps_iface_set_v6_iid
  Sets the IPv6 IID of an interface. If the interface is NULL or IPv4, this 
  function returns FALSE.

  @datatypes
  #ps_iface_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface.
  @param[in] iid              64-bit IPv6 interface identifier.
                              The IPv6 address suffix.
 
  @return
  FALSE -- Interface pointer is NULL, IPv4, or Logical.\n
  TRUE -- Otherwise.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_iface_set_v6_iid
(
  ps_iface_type       *this_iface_ptr,
  uint64               iid
);

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_DNS_ADDRS
===========================================================================*/
/** @ingroup ps_iface_get_v6_dns_addrs
  Returns the primary and secondary DNS addresses on the IPv6 interface.

  @datatypes
  #ps_iface_type\n
  #ip_addr_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block
                              for which the primary and secondary DNS addresses 
                              are to be obtained.
  @param[out] prim_dns_ptr    Primary IPv6 DNS address.
  @param[out] sec_dns_ptr     Secondary IPv6 DNS address.
 
  @return
  None.  However, if the address family is not IPv6, the output parameters
  are stored with zero.
 
  @dependencies 
  None.
  @newpage
*/
void ps_iface_get_v6_dns_addrs
(
  ps_iface_type         *this_iface_ptr,
  ip_addr_type          *prim_dns_ptr,
  ip_addr_type          *sec_dns_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_SET_V6_DNS_ADDRS
===========================================================================*/
/** @ingroup ps_iface_set_v6_dns_addrs
  Sets the primary and secondary DNS addresses on the IPv6 interface.

  @datatypes
  #ps_iface_type\n
  #ps_ip_addr_type

  @vertspace
  @param[in] this_iface_ptr   Pointer to the interface control block
                              on which the primary and secondary DNS addresses 
                              are to be set.
  @param[in] prim_dns_ptr     Primary IPv6 DNS address.
  @param[in] sec_dns_ptr      Secondary IPv6 DNS address.
 
  @return
  0 -- Primary and secondary DNS addresses are set.\n
 -1 -- Failure.

  @dependencies 
  None.
  @newpage
*/
int ps_iface_set_v6_dns_addrs
(
  ps_iface_type         *this_iface_ptr,
  ps_ip_addr_type       *prim_dns_ptr,
  ps_ip_addr_type       *sec_dns_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_GET_V6_SIP_SERV_ADDR
===========================================================================*/
/** @ingroup ps_iface_get_v6_sip_serv_addr
  Returns all the SIP server addresses stored on the IPv6 interface.

  @datatypes
  #ps_iface_type\n
  #ip_addr_type

  @vertspace
  @param[in] this_iface_ptr            Pointer to the interface control block
                                       for which the primary and secondary DNS
                                       addresses are to be obtained.
  @param[out] sip_serv_addr_ptr_array  Pointer to the SIP server addresses 
                                       array.
  @param[out] count                    As input value it contains the count
                                       of the SIP server addresses asked for
                                       and as a return value it contains the
                                       actual number of addresses returned.
 
  @return
  None.  However, if the address family is not IPv6, the output parameters
  are stored with zero.
 
  @dependencies 
  None.
  @newpage
*/
void ps_iface_get_v6_sip_serv_addr
(
  ps_iface_type      *this_iface_ptr,
  ip_addr_type       *sip_serv_addr_ptr_array,
  uint32             *count
);

/*===========================================================================
FUNCTION PS_IFACE_IS_V6_ADDR_EXTERNAL
===========================================================================*/
/** @ingroup ps_iface_is_v6_address_external
  Returns whether given ip6 address is internal or not

  @datatypes
  #ps_iface_type\n
  #ps_in6_addr

  @vertspace
  @param[in] iface_ptr                 Pointer to the interface control block
                                       for which the primary and secondary DNS
                                       addresses are to be obtained.
  @param[in] ipv6_addr_ptr             Pointer to the IPV6 address 
  
 
  @return
  TRUE : IPv6 address is external.
  FALSE : IPv6 address is not external.
  
  @dependencies 
  None.
  @newpage
*/
boolean ps_iface_is_v6_address_external
(
  ps_iface_type             * iface_ptr,
  struct ps_in6_addr        * ipv6_addr_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_IPV6_ADD_FRAG_FILTER
===========================================================================*/
/** @ingroup ps_iface_ipv6_add_frag_filter
  Adds ipv6 fragmentation filter for given ipv6 address

  @datatypes
  #ps_iface_type\n
  #ps_in6_addr

  @vertspace
  @param[in] iface_ptr                 Pointer to the interface control block
                                       for which the primary and secondary DNS
                                       addresses are to be obtained.
  @param[out] ipv6_addr_ptr            Pointer to the IPV6 address 
  
 
  @return
  None
  
  @dependencies 
  None.
  @newpage
*/
void ps_iface_ipv6_add_frag_filter
(
  ps_iface_type             * iface_ptr,
  struct ps_in6_addr        * ipv6_addr_ptr
);
#endif /* PS_IFACE_ADDR_V6_H */
