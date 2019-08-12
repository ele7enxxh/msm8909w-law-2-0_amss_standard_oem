/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            P S _ I C M P 6 _ N D . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Neighbor Discovery(IPv6 ND)

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2007-2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/inet/inc/ps_icmp6_nd.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#include "datamodem_variation.h"
#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)



#include "ps_iface.h"
#include "ps_in.h"
#include "ps_lan_llc.h"
#include "ps_icmp6_defs.h"
#include "ps_icmp6_msg.h"
#ifdef FEATURE_DATA_PS_DHCPV6
#include "ps_ifacei_addr_mgmt.h"
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  MAX_ICMP6_ND_CACHE_ENTRY - the maximum number of Neighbor Cache entries
                             supported per LLE instance
---------------------------------------------------------------------------*/
#define MAX_ICMP6_ND_CACHE_ENTRY 5

/*---------------------------------------------------------------------------
  MAX_ICMP6_ND_PREFIX_ENTRY - the maximum number of prefix entries
                              supported per LLE instance (keeps track of
                              default routers and onlink prefixes)
---------------------------------------------------------------------------*/
#define MAX_ICMP6_ND_PREFIX_ENTRY 5

typedef enum
{
  ICMP6_ND_INVALID_EV = 0,
  ICMP6_ND_RA_SENT_EV = 1,
  ICMP6_ND_LL_ADDR_RESOLVED_EV = 2
} ps_icmp6_nd_event_enum_type;

/*---------------------------------------------------------------------------
  Type definition of the function used by ICMP6 ND to request transmission of
  a packet (IP SDU) by LLC.
---------------------------------------------------------------------------*/
typedef int (*icmp6_lan_llc_tx_pkt_f_ptr_type)( lan_lle_enum_type instance,
                                                dsm_item_type**   pkt,
                                                uint16            ethertype,
                                                byte*             dst_hw_addr);

/*---------------------------------------------------------------------------
TYPEDEF ND_CONFIG_ITEMS

DESCRIPTION
  This is the type of Neighbor Discovery configuration items.
---------------------------------------------------------------------------*/
typedef struct
{
  uint16   max_nd_solicits;        /* Max Neighbor Discovery solicitations */
  uint16   max_nd_retransmits;                       /* Max ND retransmits */
  uint16   max_nd_reachable_time;    /* Max ND reachable time for ND cache */
  uint16   max_nd_delay_first_probe_time;   /* Max time to delay ND probes */
  uint16   max_nd_retransmit_interval;      /* ND retransmit interval time */
} nd_config_items_type;


/*===========================================================================

                    MACROS AND INLINE FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
MACRO PS_ICMP6_ND_LLADDR_ARE_EQUAL()

DESCRIPTION
  This macro checks to see if the length and value of the link layer address
  are equal.

PARAMETERS
  ll_addr1: 1st link layer addr to check
  ll_addr2: 2nd link layer addr to check

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_ICMP6_ND_LLADDR_ARE_EQUAL ps_icmp6_nd_lladdr_are_equal
INLINE boolean ps_icmp6_nd_lladdr_are_equal
(
  ps_link_layer_addr_type ll_addr1,
  ps_link_layer_addr_type ll_addr2
)
{
  if( (ll_addr1.ll_addr_len == ll_addr2.ll_addr_len) &&
      (0 == memcmp(ll_addr1.ll_addr,
                   ll_addr2.ll_addr,
                   ll_addr1.ll_addr_len)))
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION PS_ICMP6_NEIGHBOR_CACHE_ENTRY_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the neighbor cache entry
  pointer passed in is valid.

PARAMETERS
  ncache_ptr: pointer to the neighbor cache in question.

RETURN VALUE
  TRUE: if ncache_ptr is valid
  FALSE: otherwise.
===========================================================================*/
#define PS_ICMP6_NEIGHBOR_CACHE_ENTRY_IS_VALID(ncache_ptr)   \
  (((ncache_ptr) != NULL) &&                                 \
   ((ncache_ptr)->this_ptr == (ncache_ptr)))

/*===========================================================================
FUNCTION PS_ICMP6_PREFIX_ENTRY_IS_VALID()

DESCRIPTION
  This macro returns a boolean indicating whether the prefix entry
  pointer passed in is valid.

PARAMETERS
  prefix_ptr: pointer to the prefix cache in question.

RETURN VALUE
  TRUE: if prefix_ptr is valid
  FALSE: otherwise.
===========================================================================*/
#define PS_ICMP6_PREFIX_ENTRY_IS_VALID(prefix_ptr)   \
  (((prefix_ptr) != NULL) &&                                 \
   ((prefix_ptr)->this_ptr == (prefix_ptr)))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                            FORWARD DECLARATIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PS_ICMP6_GET_ND_CONFIG_ITEMS()

DESCRIPTION
  Access function for ND config items
 
PARAMETERS
  None
 
RETURN VALUE
  Ipv6 Neighbor discovery config items.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
nd_config_items_type* ps_icmp6_get_nd_config_items(void);

/*===========================================================================
FUNCTION PS_ICMP6_ND_INIT()

DESCRIPTION
  Initializes ps_icmp6_nd module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_init
(
  void
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_TIMER_CBACK()

DESCRIPTION
  This function is a callback for the neighbor discovery entries. When timers
  expire for any of the events: reachability expiration, retransmit timer
  expiration, and probe delay expiration this callback is called.

PARAMETERS
  user_data_ptr: the callback information which will be the prefix from
                 the onlink prefix list

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_timer_cback
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_PREFIX_LIST_ENTRY_TIMER_CBACK()

DESCRIPTION
  This function is a callback for the onlink prefix list. When a prefix
  expires it must be removed from the list as it can no longer be
  considered on link until an RA with the prefix is received again and
  the prefix lifetime updated.

PARAMETERS
  user_data_ptr: the callback information which will be the prefix from
                 the onlink prefix list

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_prefix_list_entry_timer_cback
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_START()

DESCRIPTION
  This function will initialize all the Neighbor Discovery structures,
  callback functions, tx functions, etc. The LLE instance represents the
  index into the ND arrays.

PARAMETERS
  lle_instance: The LLE instance which is the index to the corresponding
                ND structures.
  nd_config   : The neighbor discovery configuration structure.
  iface_ptr   : The interface on which the LLE instance is associated.
  pkt_tx_fcn  : The transmit function to send the pkt out once ND is
                complete.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_start
(
  lan_lle_enum_type                 lle_instance,
  ps_icmp6_nd_config_type         * nd_config,
  ps_iface_type                   * iface_ptr,
  icmp6_lan_llc_tx_pkt_f_ptr_type   llc_tx_f_ptr
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_FLUSH()

DESCRIPTION
  This function will delete all the neighbor discovery entries, but leave
  all initialized info intact.

PARAMETERS
  lle_instance: The LLE instance which is the index to the corresponding
                ND structures.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_flush
(
  lan_lle_enum_type  lle_instance
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_STOP()

DESCRIPTION
  This function will cleanup (reset) all the neighbor discovery structures
  including: neighbor cache, prefix list, destination cache, and the default
  router list for a particular device. This will be executed when the LLC
  instance is released.

PARAMETERS
  lle_instance: The LLE instance which is the index to the corresponding
                ND structures.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_icmp6_nd_stop
(
  lan_lle_enum_type  lle_instance
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_TX_PKT()

DESCRIPTION

  This function checks to see whether the packet is being sent on an
  on_link prefix or if it should be sent to the default router. It will then
  determine whether or not there is a link layer address available for the
  destination and will initiate ND if necessary.

PARAMETERS
  iface_ptr: The pointer to the interface on which to send the pkt.

DEPENDENCIES
  None

RETURN VALUE
  0  AEE_SUCCESS
 -1  FAILURE

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_icmp6_nd_tx_pkt
(
  lan_lle_enum_type                 lle_instance,
  struct ps_in6_addr               *dest,
  dsm_item_type                   **pkt_chain_ptr
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_RA()

DESCRIPTION

 The router advertisement message and parameters are all verified. Then
 action is taken to apply prefixes, if any were validated properly.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - RA processed properly
  FALSE - There was an error in processing the RA
SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_ra
(
  ps_iface_type      *iface_ptr,       /* Input interface                  */
  ps_icmp6_msg_type  *icmp6,           /* ICMP6 Message                    */
  struct ps_in6_addr *gateway_ptr      /* Gateway which sent RA            */
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_RS()

DESCRIPTION

 Validates the router solicitation and sends a router advertisement in
 response to the sender.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - RS processed properly
  FALSE - There was an error in processing the RS

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_rs
(
  ps_iface_type     *iface_ptr,       /* Input interface                   */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_NA()

DESCRIPTION

  This function is responsible for identifying and processing the neighbor
  advertisement. Based upon the type options included within the
  advertisement the appropriate actions will be taken to maintain the
  neighbor cache, restart DAD, or other

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - NA processed properly
  FALSE - There was an error in processing the NA

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_na
(
  ps_iface_type     *iface_ptr,       /* Input interface                   */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_NS()

DESCRIPTION

 The router advertisement message and parameters are all verified. Then
 action is taken to apply prefixes, if any were validated properly.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - NS processed properly
  FALSE - There was an error in processing the NS

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_ns
(
  ps_iface_type     *iface_ptr,       /* Input interface                   */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

/*===========================================================================

FUNCTION PS_ICMP6_ND_PROCESS_REDIRCT()

DESCRIPTION

 The router advertisement message and parameters are all verified. Then
 action is taken to apply prefixes, if any were validated properly.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - Redirect processed properly
  FALSE - There was an error in processing the Redirect
SIDE EFFECTS
  None
===========================================================================*/
boolean ps_icmp6_nd_process_redirect
(
  ps_iface_type     *iface_ptr,       /* Input interface                   */
  ps_icmp6_msg_type *icmp6,           /* ICMP6 Message                     */
  ip_pkt_info_type  *ip_pkt_info_ptr  /* IP packet information             */
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_NS()

DESCRIPTION
  This function will compose and send a neighbor solicitation based on passed
  parameters.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to cleanup the
                  neighbor discovery caches.
  dest:           The destination address to send to
  target:         The target address to use in the NS
  ns_type:        The type of neighbor solicitation we are sending

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_ns
(
  ps_iface_type          *this_iface_ptr,
  struct ps_in6_addr     *dest,
  ps_icmp6_nd_msg_type    ns_type
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_NA()

DESCRIPTION
  This function will compose and send a neighbor advertisement in response
  to a neighbor solicitation. The message type is formed according to passed
  parameters.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to cleanup the
                  neighbor discovery caches.
  dest:           The destination address to send to
  target:         The target address to use in the NS
  na_type:        The type of neighbor solicitation we are sending
  override:       Whether the included link layer addr should override
                  what's in the peers cache

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_na
(
  ps_iface_type          *this_iface_ptr,
  struct ps_in6_addr     *dest,
  struct ps_in6_addr     *target,
  ps_icmp6_nd_msg_type    na_type,
  boolean                 override
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_RA()

DESCRIPTION
  This function will compose and send a router advertisement in response
  to a router solicitation. The message type is formed according to passed
  parameters. Currently only responses on Rm interfaces are supported.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.
  src_iface_ptr (optional): If supplied, use RA info from this iface 
  src:            The source address it was sent to (mcast, link_local, etc)

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_ra
(
  ps_iface_type          *this_iface_ptr,
  ps_iface_type          *src_iface_ptr,
  struct ps_in6_addr     *dest_addr
);

/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_RA_TO_DEP_PREFIX()

DESCRIPTION
  This function will compose and send a router advertisement in order to 
  deprecate the old IPv6 prefix

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_ra_to_dep_prefix
(
  ps_iface_type          *this_iface_ptr
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION PS_ICMP6_ND_SEND_UNICAST_RA_TO_DEP_PREFIX()

DESCRIPTION
  This function will compose and send a router advertisement in order to 
  deprecate the old IPv6 prefix

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_send_unicast_ra_to_dep_prefix
(
  ps_iface_type                  *this_iface_ptr,
  struct ps_in6_addr             *dest_addr_ptr,
  ps_iface_addr_mgmt_prefix_type *prefix_to_deprecate
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION PS_ICMP6_ND_CREATE_RA()

DESCRIPTION
  This function will compose and send a router advertisement in response
  to a router solicitation. The message type is formed according to passed
  parameters. Currently only responses on Rm interfaces are supported.

PARAMETERS
  this_iface_ptr: The pointer to the interface on which to send the RA.
  src_iface_ptr (optional): If supplied, use RA info from this iface 
  dest_addr: The pointer to the destination address.
  dep:  Whether or not the prefix should be depricated (set the preferred value
        to 1).

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_nd_create_ra
(
  ps_iface_type                  *this_iface_ptr,
  ps_iface_type                  *src_iface_ptr,
  struct ps_in6_addr             *dest_addr,
  boolean		          deprecate_prefix,
  ps_iface_addr_mgmt_prefix_type *prefix_to_deprecate
);

/*===========================================================================
FUNCTION PS_ICMP6_GET_DEST_MAC_ADDR

DESCRIPTION
  This function gets the destination mac address for an ipv6 call. It 
  internally looks up ps_icmp6_nd_find_nd_entry to perform this.

PARAMETERS
  target_iface_ptr: Pointer to iface
  ipv6_addr  : IPv6 address
  dest_mac_addr : OUT parameter to store result

RETURN VALUE

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_icmp6_get_dest_mac_addr
(
  ps_iface_type       *iface_ptr,
  struct ps_in6_addr  *v6_addr_ptr,
  uint8               *dest_mac_addr,
  uint8                dest_buf_size
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
