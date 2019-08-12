#ifndef PS_IP6_ADDR_H
#define PS_IP6_ADDR_H
/** 
  @file ps_ip6_addr.h 
  @brief 
   Contains common data declarations and function prototypes for
   operating on privacy IPv6 address.
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I P 6 _ A D D R . H

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
Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_ip6_addr.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

04/09/14    ash    Added Prefix Delegation support.
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup. Edited 
                   Doxygen markup and comments.

===========================================================================*/
/* group: tcpUdpIpIp6 */
/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ps_in.h"

/** @addtogroup constants 
  @{
*/
#define DEFAULT_IP6_DATA_UNIT 1280 /**< Minimum MTU for IPv6               */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Stores the PS iface addresses. Other parameters can also be added to 
  this structure (i.e., prefix to generate the address and IPsec options).
*/
typedef struct
{
  boolean app_request;            /**< Application request.                   */
  boolean is_unique;              /**< Indicates whether the address is unique 
                                       or shareable. */
  /* Other params such as prefix to generate the addr with and IPsec options
     Can also be added to this structure. */

} ps_ipv6_iid_params_type;
/** @} */ /* end_addtogroup datatypes */

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
extern uint16 ip6_default_mtu;
#ifdef FEATURE_DATA_PS_DHCPV6
extern boolean dhcp6_prefix_delegation_enable;
#endif /* FEATURE_DATA_PS_DHCPV6 */


#ifdef __cplusplus
extern "C" {
#endif
/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION PS_IFACE_IPV6_PRIV_ADDR_INC_REF_CNT_EX()
===========================================================================*/
/** @ingroup ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex
  Increments the reference count of a private IPv6 IID.

  @datatypes
  #ps_in6_addr

  @vertspace
  @param[in] ip_addr_ptr      IPv6 address.
  @param[in] incl_ext_addr    boolean to indicate whether to increment
                              ref count of external address.
 
  @return
   0 -- Reference count incremented successfully.\n
  -1 -- Failure.

  @dependencies 
  None.
  @newpage
*/
int ps_ip_addr_ipv6_priv_addr_inc_ref_cnt_ex
(
  struct ps_in6_addr *ip_addr_ptr,
  boolean             incl_ext_addr
);


/*===========================================================================
MACRO PS_IFACE_IPV6_PRIV_ADDR_INC_REF_CNT()
===========================================================================*/
/** @ingroup ps_ip_addr_ipv6_priv_addr_inc_ref_cnt
  Increments the reference count of a private IPv6 IID.

  @datatypes
  #ps_in6_addr

  @vertspace
  @param[in] ip_addr_ptr      IPv6 address.
 
  @return
   0 -- Reference count incremented successfully.\n
  -1 -- Failure.

  @dependencies 
  None.
  @newpage
*/
int ps_ip_addr_ipv6_priv_addr_inc_ref_cnt
(
  struct ps_in6_addr *ip_addr_ptr
);


/*===========================================================================
MACRO PS_IFACE_IPV6_PRIV_ADDR_DEC_REF_CNT()
===========================================================================*/
/** @ingroup ps_ip_addr_ipv6_priv_addr_dec_ref_cnt
  Decrements the reference count of a private IPv6 address. When the
  reference count goes to 0, the privacy address is deleted.

  @datatypes
  #ps_in6_addr

  @vertspace
  @param[in] ip_addr_ptr      IPv6 address.
 
  @return
   0 -- Reference count decremented successfully.\n
  -1 -- Failure.

  @dependencies 
  None.
  @newpage
*/
int ps_ip_addr_ipv6_priv_addr_dec_ref_cnt
(
  struct ps_in6_addr *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IP_ADDR_GET_IP6_DEFAULT_MTU()

DESCRIPTION
  This function returns the configured default IPv6 MTU

PARAMETERS
  None

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 ps_ip_addr_get_ip6_default_mtu
(
  void
);

/*===========================================================================
MACRO IN6_ARE_PREFIX_EQUAL()
===========================================================================*/
/** @ingroup in6_are_prefix_equal
  Indicates whether the prefix of the passed IPv6 addresses are the same.
*/
#define IN6_ARE_PREFIX_EQUAL in6_are_prefix_equal

/** @ingroup in6_are_prefix_equal
  Determines whether two IPv6 prefixes are the same.

  @datatypes
  #ps_in6_addr

  @vertspace
  @param[in] addr1        First IPv6 address.
  @param[in] addr2        Second IPv6 address.
  @param[in] prefix_len   Prefix length.
 
  @return
  TRUE -- Prefixes are same.\n
  FALSE -- Otherwise.

  @dependencies 
  None.
*/
INLINE boolean in6_are_prefix_equal
(
  struct ps_in6_addr *addr1,
  struct ps_in6_addr *addr2,
  uint32              prefix_len
)
{
  uint32 i;
  uint32 mask;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == prefix_len || 128 < prefix_len)
  {
    return FALSE;
  }

  for (i=0; i<(prefix_len/32); i++)
  {
    if (addr2->ps_s6_addr32[i] != addr1->ps_s6_addr32[i])
    {
      return FALSE;
    }
  }

  if (prefix_len%32)
  {
    mask = ~((uint32)((1<<(32-(prefix_len%32)))-1));
    mask = ps_htonl(mask);
    if ((addr1->ps_s6_addr32[i] & mask) != (addr2->ps_s6_addr32[i] & mask))
    {
      return FALSE;
    }
  }

  return TRUE;
} /* in6_are_prefix_equal() */

#ifdef __cplusplus
}
#endif


#endif /* PS_IP6_ADDR_H */
