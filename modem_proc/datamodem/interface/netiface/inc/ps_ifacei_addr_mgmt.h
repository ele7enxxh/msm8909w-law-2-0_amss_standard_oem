#ifndef PS_IFACE_ADDR_MGMT_H
#define PS_IFACE_ADDR_MGMT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E _ A D D R _ M G M T . H

GENERAL DESCRIPTION
  Interface IP Address Management Layer

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c)2008-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_ifacei_addr_mgmt.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/25/15    skc    Provided new API to create unique address
08/18/14    cx     Added API to get v6 address from handle
04/25/14    ash    Added api to compare prefixes
02/18/14    vl     Modified IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME from 15 mins to 60 mins
11/01/13    rk     Removed feature mobileap.
03/28/12    ash    Modified IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME to 15 mins
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */
#ifdef FEATURE_DATA_PS_ADDR_MGMT
#include "ps_iface.h"
#ifdef FEATURE_DATA_PS_DHCPV6
#include "ps_ip6_sm.h"
#include "ps_hashtable.h"
#include "ps_in.h"
#endif /* FEATURE_DATA_PS_DHCPV6 */
#include "ps_ifacei_addr_v6.h"
/*===========================================================================

                         EXTERNAL DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  PS_IFACE_ADDR_MGMT_MASK_MIN            = 0x0000,
  PS_IFACE_ADDR_MGMT_MASK_PREFIX         = 0x0001,
  PS_IFACE_ADDR_MGMT_MASK_PREFIX_LEN     = 0x0002,
  PS_IFACE_ADDR_MGMT_MASK_IID            = 0x0004,
  PS_IFACE_ADDR_MGMT_MASK_GATEWAY_IID    = 0x0008,
  PS_IFACE_ADDR_MGMT_MASK_PREF_LIFETIME  = 0x0010,
  PS_IFACE_ADDR_MGMT_MASK_VALID_LIFETIME = 0x0020,
  PS_IFACE_ADDR_MGMT_MASK_ADDR_STATE     = 0x0040,
  PS_IFACE_ADDR_MGMT_MASK_ADDR_TYPE      = 0x0080,
  PS_IFACE_ADDR_MGMT_MASK_REF_CNT        = 0x0100,
  PS_IFACE_ADDR_MGMT_MASK_INTERNAL_ONLY  = 0x01C0,
  PS_IFACE_ADDR_MGMT_MASK_DAD_RETRIES    = 0x0200,
  PS_IFACE_ADDR_MGMT_MASK_ALL            = 0x01FF 
} ps_iface_addr_mgmt_addr_mask_enum_type;


typedef enum 
{
  IFACE_ADDR_MGMT_DAD_SUCCESS        = 0,
  IFACE_ADDR_MGMT_DAD_DUP_ADDR       = 1,
  IFACE_ADDR_MGMT_DAD_NO_MEM         = 2,
  IFACE_ADDR_MGMT_DAD_NETWORK_REJECT = 3,
  IFACE_ADDR_MGMT_DAD_INTERNAL_ERR   = 4,
  IFACE_ADDR_MGMT_DAD_TIMEOUT        = 5
} ps_iface_addr_mgmt_dad_enum_type;

typedef struct
{
  ps_iface_addr_mgmt_addr_mask_enum_type addr_mask;
  uint64  prefix;
  uint64  iid;
  uint64  gateway_iid;
  uint32  pref_lifetime;
  uint32  valid_lifetime;
  uint8   prefix_len;
  uint8   dad_retries;
  
  ps_iface_ipv6_addr_state_enum_type  addr_state;
  ps_iface_ipv6_addr_type_enum_type   addr_type;
  ps_iface_type * linked_iface_ptr;
} ps_iface_addr_mgmt_addr_info_type;

typedef struct
{
  ps_ip_addr_type                   ip_addr;
  ps_iface_ipv6_addr_type_enum_type addr_type;
} ps_iface_addr_mgmt_alloc_type;

typedef void ps_iface_addr_mgmt_free_type;

#ifdef FEATURE_DATA_PS_DHCPV6
/**
@brief Struct for Prefix & Prefix length. 
*/
typedef struct
{
  uint64           prefix;
  /**< Delegated Prefix. */

  uint8            prefix_len;
  /**< Delegated Prefix length. */

  struct ps_in6_addr  host_addr;
  /**< source address of the host requesting prefix. */

  ps_iface_type   *linked_iface_ptr;                    
    /**< Pointer to the "linked" interface, e.g. Rm iface for an EXTERNAL
         address; WWAN iface for Um addrs on soft-ap iface, and NULL for
         Global/Privacy address. */
} ps_ifacei_addr_mgmt_prefix_cache_type;
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*---------------------------------------------------------------------------
  The time interval in which to verify external addresses to ensure they
  are still being used.
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_INTERVAL_VERIFY_TIME 3600000 /* 60 mins */
#define IPV6_EXT_ADDR_WAIT_TIME            60000  /* 1 min   */

#define IPV6_MAX_ADDR_DAD_RETRIES          1


/*---------------------------------------------------------------------------
  Events for debug buffer
---------------------------------------------------------------------------*/
typedef enum
{
   INVALID = 0,
   ADDRESS_ADDED = 1,
   ADDRESS_PROBED = 2,
   ADDRESS_DELETED_UNUESD_IID_TIMER_EXP = 3,            /* usued iid timer expired */
   ADDRESS_DELETED_FREE_BY_IFACE = 4,               
   ADDRESS_DELETED_DO_DAD_FAILED = 5,
   ADDRESS_DELETED_DO_DAD_UPDATE_FAILED = 6,
   ADDRESS_DELETED_DO_DAD_UPDATE_NO_MEM = 7,
   ADDRESS_DELETED_DO_DAD_UPDATE_TIMEOUT = 8,
   ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS_RM = 9,
   ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS_UM =10,
   ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS = 11,
   ADDRESS_DELETED_UNUSED_IID_RM_IFACE_NULL = 12,
   ADDRESS_DELETED_UNUSED_IID_SEND_NS_FAIL = 13,
   ADDRESS_DELETED_PREFIX_CHANGED = 14,
   ADDRESS_DELETED_REMOVE_V6_PREFIX = 15,
   ADDRESS_DELETED_DELETE_ALL_PREFIXES = 16,
   ADDRESS_DELETED_DELETE_IN_TRAT = 17 
} ps_ifacei_addr_mgmt_debug_buf_event_enum;

/*---------------------------------------------------------------------------
  Type for entires in debug buffer
---------------------------------------------------------------------------*/
typedef struct ps_ifacei_addr_mgmt_debug_buf_type
{
    uint32                                      timestamp;  
    ps_iface_name_enum_type                     iface_name;           
    uint8                                       iface_instance;    
    uint64                            prefix;                      
    uint64                            iid;
    ps_ifacei_addr_mgmt_debug_buf_event_enum    event;  
}ps_ifacei_addr_mgmt_debug_buf_type;

/*----------------------------------------------------------------------------
  Prefix management Configuration Macros
----------------------------------------------------------------------------*/
/* Hashtable size for prefix mgmt */
#define IPV6_PREFIX_MGMT_HASH_TBL_SIZE     20

/* Hashtable bucket depth for prefix mgmt */
#define IPV6_PREFIX_MGMT_HASH_BUCKET_DEPTH 20 

/* Max Prefix LENGTH*/
#define IPV6_PREFIX_MGMT_MAX_PREFIX_LENGTH 64 

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC FUNCTION DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_INIT_DELEGATED_PREFIX()

DESCRIPTION
 This is initializes IPv6 Prefix delagtion module of addr mgmt.

PARAMETERS

RETURN VALUE
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_init_delegated_prefix
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_CLEANUP_DELEGATED_PREFIX()

DESCRIPTION
  This is cleanup IPv6 delegated prefix from addr mgmt module.

PARAMETERS

RETURN VALUE
 -1 for failure
  0 on success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_cleanup_delegated_prefix
(
  ps_iface_type * iface_ptr
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_DEFAULT_ADDR_CB_FUNC()

DESCRIPTION
  This is the default address callback function. It does nothing and exists
  simply to indicate that mode handlers should be setting their own functions
  or setting the f_ptr to NULL.

PARAMETERS

RETURN VALUE
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_default_addr_cb_func
(
  ps_ip_addr_type                  ip_addr,
  ps_iface_addr_mgmt_event_type    addr_event,
  ps_iface_type                  * linked_iface_ptr,
  void                           * user_data
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_DEFAULT_DAD_FUNC()

DESCRIPTION
  This is the default DAD callback function. It does nothing and exists
  simply to indicate that mode handlers should be setting their own functions
  or setting the f_ptr to NULL.

PARAMETERS

RETURN VALUE
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_default_dad_func
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle,
  void                           * user_data
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_IPV6_DEFAULT_DAD_F()

DESCRIPTION
  This function is used to initiate the standard method of DAD as per 
  RFC 2461. This can be set as the the dad_f_ptr for any interface utilizing
  this RFC.

PARAMETERS
  iface_ptr:    The interface on which the address verification is to be
                performed.
  ipv6_addr:    The address to perform duplicate address detection on.
  ps_errno:     The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int ps_iface_addr_mgmt_ipv6_default_dad_f
(
  ps_iface_type         * iface_ptr,
  struct ps_in6_addr    * ip_addr_ptr,
  int16                 * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_HANDLE_FROM_IP()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.

PARAMETERS
  ip_addr_ptr:  The ptr to the ip address

RETURN VALUE
  handle if successful
 -1      if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_addr_mgmt_handle_type ps_iface_addr_mgmt_get_handle_from_ip
(
  ps_iface_type       * iface_ptr,
  struct ps_in6_addr  * ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_HANDLE_FROM_IP_EX()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.
  While looking for the address it only searches through IP address
  of type specified in addr_type_mask.

PARAMETERS
  ip_addr_ptr:  The ptr to the ip address
  addr_type_mask: IP address type mask.

RETURN VALUE
  handle if successful
 -1      if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_addr_mgmt_handle_type ps_iface_addr_mgmt_get_handle_from_ip_ex
(
  ps_iface_type                         *iface_ptr,
  struct ps_in6_addr                    *ip_addr_ptr,
  ps_iface_ipv6_addr_type_mask_enum_type addr_type_mask
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_HANDLE_FROM_ADDR()

DESCRIPTION
  This function retrieves the handle given the IPv6 address structure.

PARAMETERS
  v6_addr_ptr:  The ptr to the address structure.
  handle_ptr:   The handle from which to derive the IP address structure

RETURN VALUE
  handle if successful
  -1     if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_addr_mgmt_handle_type ps_iface_addr_mgmt_get_handle_from_addr
(
  ps_ifacei_v6_addr_type        * v6_addr_ptr
);


/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMTI_SET_ADDR_INFO()

DESCRIPTION
  This internal function is used to set all address information in the 
  IP address structure. Anything not allowed to be set externally is filtered
  out in the externalized function.

PARAMETERS
  iface_ptr:     Interface on which the address exists.
  handle_ptr:    The handle to the IP address structure to update.
  addr_info_ptr: The address information structure from which to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the client's IP address information 
  structure to the IP address structure.
===========================================================================*/
int ps_iface_addr_mgmti_set_addr_info
(
  ps_iface_type                     * iface_ptr,
  ps_iface_addr_mgmt_handle_type    * handle_ptr,
  ps_iface_addr_mgmt_addr_info_type * addr_info_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_EXT_ADDR_PRESENT()

DESCRIPTION
  This function cancels the timer associated with verifying that an
  external address is still present and in use.

PARAMETERS
  handle_ptr:  The handle to the IP address structure.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_addr_mgmt_ext_addr_present
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMTI_ALLOC_ADDR_EX()

DESCRIPTION
  This function allocates an address buffer and returns the handle to the
  caller.

PARAMETERS
  iface_ptr:         Interface on which the address exists.
  linked_iface_ptr:  pointer to an iface to which this addr belongs.
  handle_ptr:        The handle to the newly alloc'd IP address structure.
  alloc_info:        Required information to allocate the address;
  ps_errno:          The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a ps mem buffer for the address.
===========================================================================*/
int ps_iface_addr_mgmti_alloc_addr_ex
(
  ps_iface_type                  * iface_ptr,
  ps_iface_type                  * linked_iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr,
  ps_iface_addr_mgmt_alloc_type  * create_info,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMTI_ALLOC_ADDR()

DESCRIPTION
  This function allocates an address buffer and returns the handle to the 
  caller.

PARAMETERS
  iface_ptr:       Interface on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  alloc_info:      Required information to allocate the address;
  ps_errno:        The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a ps mem buffer for the address.
===========================================================================*/
int ps_iface_addr_mgmti_alloc_addr
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr,
  ps_iface_addr_mgmt_alloc_type  * create_info,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_FREE_ADDR()

DESCRIPTION
  This function frees the address buffer associated with the passed handle.

PARAMETERS
  iface_ptr:       Interface on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  free_info_ptr:   Any additional required information needed.
  ps_errno:        The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Allocates a ps mem buffer for the address.
===========================================================================*/
int ps_iface_addr_mgmt_free_addr
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr,
  ps_iface_addr_mgmt_free_type   * free_info,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_FREE_ADDRS_BY_IFACE()

DESCRIPTION
  This function frees all the IPv6 address PS mem buffers stored on 
  src_iface_ptr, linked to key_iface_ptr

PARAMETERS
  src_iface_ptr: Interface on which to perform free operation
  key_iface_ptr: Iface to match for deletion

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_free_addrs_by_iface
(
  ps_iface_type * src_iface_ptr,
  ps_iface_type * key_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_ADDR_INFO()

DESCRIPTION
  This function is used to retrieve information about an address to the
  client.

PARAMETERS
  handle_ptr:    The handle to the address structure from which to retrieve
                 the information.
  addr_info_ptr: The address information structure to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the IP address structure to the 
  client's address information structure.
===========================================================================*/
int ps_iface_addr_mgmt_get_addr_info
(
  ps_iface_addr_mgmt_handle_type    * handle_ptr,
  ps_iface_addr_mgmt_addr_info_type * addr_info_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_SET_ADDR_INFO()

DESCRIPTION
  This function is used to set address information in the IP address
  structure.

PARAMETERS
  handle_ptr:    The handle to the IP address structure to update.
  addr_info_ptr: The address information structure from which to populate.
  ps_errno:      The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  Copies the IP address iformation from the client's IP address information 
  structure to the IP address structure.
===========================================================================*/
int ps_iface_addr_mgmt_set_addr_info
(
  ps_iface_type                     * iface_ptr,
  ps_iface_addr_mgmt_handle_type    * handle_ptr,
  ps_iface_addr_mgmt_addr_info_type * addr_info_ptr,
  int16                             * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMTI_IPV6_DO_DAD()

DESCRIPTION
  This function is used to initiate duplicate address detection on an address.

PARAMETERS
  iface_ptr:    The interface on which the address verification is to be
                performed.
  ipv6_addr:    The address to perform duplicate address detection on.
  ps_errno:     The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int ps_iface_addr_mgmti_ipv6_do_dad
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr,
  ps_iface_addr_mgmt_alloc_type  * alloc_info,
  int16                          * ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_IPV6_DAD_UPDATE()

DESCRIPTION
  This function is used for clients to indicate to the interface layer
  the status of a nonstandard DAD attempt.

PARAMETERS
  iface_ptr:    The interface on which the address verification is being
                performed.
  handle_ptr:   The handle to the IP address structure to update.
  dad_code:     The error/success code for the dad attempt.
  ps_errno:     The error code to return in case of a problem.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
int ps_iface_addr_mgmt_ipv6_dad_update
(
  ps_iface_type                    * iface_ptr,
  ps_iface_addr_mgmt_handle_type   * handle_ptr,
  ps_iface_addr_mgmt_dad_enum_type   dad_code,
  int16                            * ps_errno
);
/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_FREE_EXT_V6_ADDRESSES()

DESCRIPTION
  This function will delete all external IPv6 address from um_iface

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_addr_mgmt_free_ext_v6_addresses
(
  ps_iface_type *this_iface_ptr
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_PREFIX_MODE()

DESCRIPTION
  This function returns the prefix mode of the iface

PARAMETERS
  iface_ptr:    Interface on which the address exists.
  prefix_mode: [OUT] contains the prefix mode of the iface.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_get_prefix_mode
(
  ps_iface_type                 * iface_ptr,
  ps_iface_ip6_prefix_mode_type * prefix_mode
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_SET_PREFIX_MODE()

DESCRIPTION
  This function sets the prefix mode of the iface

PARAMETERS
  iface_ptr:    Interface on which the address exists.
  prefix_mode: [IN] contains the prefix mode for the iface.

RETURN VALUE
  0 success
 -1 failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_set_prefix_mode
(
  ps_iface_type                 * iface_ptr,
  ps_iface_ip6_prefix_mode_type   prefix_mode
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION PS_IFACE_REMOVE_DELEGATED_V6_PREFIXES()

DESCRIPTION
  This function removes the delegated prefixes from the prefix
  management table and free it up for further use by different
  host.

PARAMETERS
  ps_iface_ptr: the iface from which address needs to be flushed
  remove_all_prefixes:  If TRUE removes all delegated prefixes from iface.
  prefix_to_remove: Indicate which delegated prefix to remove if remove_all_prefixes is FALSE


RETURN VALUE
  0 on success
-1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_remove_delegated_v6_prefixes
(
   ps_iface_type                  * ps_iface_ptr, 
   boolean                          remove_all_prefixes, 
   ps_iface_addr_mgmt_prefix_type   prefix_to_remove, 
   int16                           *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GENERATE_V6_PREFIX()

DESCRIPTION
  This function generates the bit 64 prefix from delegated prefix.
  If the prefix is already generated for the host (identified by the
  v6 source address), this function does nothing.
  IPv6 lifetimes associated with generated prefix is inifinite.

PARAMETERS
  ps_iface_ptr: The iface on which address needs to be generated.
                This function operates on UM iface, if Rmnet iface is
                passed, UM iface is obtained from RM iface,
  host_addr_ptr: pointer to the source address of the requesting host.
  delegated_prefix : output delegated prefix.


RETURN VALUE
  0 on success
-1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_generate_v6_prefix
(
   ps_iface_type                  *ps_iface_ptr,
   ps_iface_type                  *linked_iface_ptr,
   struct ps_in6_addr             *host_addr_ptr,
   ps_iface_addr_mgmt_prefix_type *delegated_prefix,
   int16                          *ps_errno
);

#ifdef FEATURE_DATA_PS_DHCPV6
/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_SET_DELEGATED_V6_PREFIX()

DESCRIPTION
  This function sets the delegated prefix obtain from the network
  in the iface. All new prefix will be generated from this
  delegated prefix.

PARAMETERS
  ps_iface_ptr: The iface on which delegated prefix needs to be stored.
                This function operates on UM iface, if Rmnet iface is
                passed, UM iface is obtained from RM iface,
  delegated_prefix : delegated prefix.
  exclude_prefix : Indicated that a prefix or a subset of the prefixes
                   should be excluded from generation process.
  excluded_prefix: Excluded prefix(s)

RETURN VALUE
  0 on success
-1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_set_delegated_v6_prefix
(
   ps_iface_type                  *ps_iface_ptr,
   ps_iface_addr_mgmt_prefix_type  delegated_prefix,
   boolean                         exclude_prefix,
   ps_iface_addr_mgmt_prefix_type  prefix_to_exclude
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_DEPRECATE_DELEGATED_PREFIXES()

DESCRIPTION
  This function sends RA with 0 lifetimes for all delegated prefixes
  generated by the iface.
  the RA.

PARAMETERS
  ps_iface_ptr: The iface on which the prefix exist

RETURN VALUE
  0 on success
-1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_deprecate_delegated_prefixes
(
   ps_iface_type *ps_iface_ptr
);
#endif /* FEATURE_DATA_PS_DHCPV6 */

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_PREFIX()

DESCRIPTION
  This function returns the prefix configured on iface, and it's length.
  For prefix delegation mode, delegated prefix is returned.

PARAMETERS
  ps_iface_ptr: The iface on which the prefix exist
  prefix_info:  [OUT] struct to return prefix & prefix length

RETURN VALUE
  0 on success
-1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_get_prefix
(
   ps_iface_type                  * ps_iface_ptr, 
   ps_iface_addr_mgmt_prefix_type * prefix_info
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_COMPARE_PREFIXES()

DESCRIPTION
 This function does bit comparision of 2 prefixes.

PARAMETERS

RETURN VALUE
 TRUE on success
 FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_addr_mgmt_compare_prefixes
(
  uint64 prefix1,
  uint64 prefix2,
  uint8  prefix_len
);

/*===========================================================================
FUNCTION PS_IFACEI_ADDR_MGMT_DEBUG_ADD_LOG()

DESCRIPTION
 This function will add event to debug buffer

PARAMETERS
 event: The event that added to log
 iface_ptr: pointer to iface
 v6_addr_ptr: pointer to the ipv6 address

RETURN VALUE
 TRUE on success
 FALSE on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void
ps_ifacei_addr_mgmt_debug_buf_add_log
(
  ps_ifacei_addr_mgmt_debug_buf_event_enum event,
  ps_iface_type * iface_ptr,
  ps_ifacei_v6_addr_type * v6_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_GET_ADDR_FROM_HANDLE()

DESCRIPTION
  This function retrieves the IPv6 address structure for the given handle.

PARAMETERS
  handle_ptr:   The handle from which to derive the IP address structure

RETURN VALUE
  IPv6 address structure if successful
  NULL                   if failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ps_ifacei_v6_addr_type * ps_iface_addr_mgmt_get_addr_from_handle
(
  ps_iface_addr_mgmt_handle_type  *  handle_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_DEPRECATE_PREFIX()

DESCRIPTION
  This function will send depreacte prefix.

PARAMETERS
  this_iface_ptr: Iface for which to send deprecate prefix.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_addr_mgmt_deprecate_prefix
(
  ps_iface_type *this_iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMTI_SET_PREFIX_CACHE()

DESCRIPTION
  This function caches the prefix on the iface.

PARAMETERS
  iface_ptr :  The handle to iface.
  prefix    :  Prefix to cache.

RETURN VALUE
  0 for success
 -1 for error

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int16 ps_iface_addr_mgmt_set_prefix_cache 
(
  ps_iface_type * iface_ptr,
  uint64          prefix,
  uint8           prefix_len
);

/*===========================================================================
FUNCTION PS_IFACE_ADDR_MGMT_ALLOC_UNIQUE_ADDR()

DESCRIPTION
  This function allocates an address buffer as well as do dad() for
  the new allocated address.

PARAMETERS
  
  iface_ptr:       Interface on which the address exists.
  handle_ptr:      The handle to the newly alloc'd IP address structure.
  alloc_info:      Required information to allocate the address;
  ps_errno:        The error code to return in case of a problem.
  

RETURN VALUE
  0: create address successfully
  -1: create address failed

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_addr_mgmt_alloc_unique_addr
(
  ps_iface_type                  * iface_ptr,
  ps_iface_addr_mgmt_handle_type * handle_ptr,
  ps_iface_addr_mgmt_alloc_type  * create_info_ptr,
  int16                          * ps_errno
);

#endif /* FEATURE_DATA_PS_ADDR_MGMT */
#endif /* PS_IFACE_ADDR_MGMT_H */
