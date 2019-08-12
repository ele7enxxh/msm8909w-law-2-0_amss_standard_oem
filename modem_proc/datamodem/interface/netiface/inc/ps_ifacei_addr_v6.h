#ifndef PS_IFACEI_ADDR_V6_H
#define PS_IFACEI_ADDR_V6_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        P S _ I F A C E I _ A D D R _ V 6 . H

GENERAL DESCRIPTION
 Internet Protocol Version 6 - Interface Layer Functionality

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_ifacei_addr_v6.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

  when      who    what, where, why
--------    ---    ----------------------------------------------------------
08/22/14    skc    Changed default probe behavior
08/18/14    cx     Add support to not probe the recent added ext addresses
07/27/14    cx     Add support for reporting NA/NS transaction to diag
01/19/14    cx     Support for unused external addresses probe optimization
02/11/09    pp     NULL check for ipv6_addrs array elements before accessing
                   them from iface_ptr.
12/14/08    pp     Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#if defined (FEATURE_DATA_PS) && defined (FEATURE_DATA_PS_IPV6)
#include "ps_iface.h"
#include "ps_ifacei.h"
#include "ps_ip6_addr.h"
#include "ds_Utils_DebugMsg.h"
#include "event_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_ADDR_TYPE_MASK_ENUM_TYPE

DESCRIPTION
  The bitmask for the IPv6 address types. Public/Private, etc.
---------------------------------------------------------------------------*/
typedef enum
{
  // This mask is for public address
  IPV6_ADDR_TYPE_MASK_PUBLIC           = 1 << IPV6_ADDR_TYPE_PUBLIC,
  // This mask is for in use private shareable address
  IPV6_ADDR_TYPE_MASK_PRIV_SHARED      = 1 << IPV6_ADDR_TYPE_PRIV_SHARED,
  // This mask is for in use private unique address
  IPV6_ADDR_TYPE_MASK_PRIV_UNIQUE      = 1 << IPV6_ADDR_TYPE_PRIV_UNIQUE,
  // This mask is for in use by an external device
  IPV6_ADDR_TYPE_MASK_EXTERNAL         = 1 << IPV6_ADDR_TYPE_EXTERNAL,
  // This mask is for all address type
  IPV6_ADDR_TYPE_ALL                   = 0xFFFF
} ps_iface_ipv6_addr_type_mask_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF IPV6_EXT_ADDR_PROBE_CONFIG_ITEMS_TYPE

DESCRIPTION
  Configuration items for IPv6 external address probe 
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  // Threshold between probing phase in percentage 
  uint8     cache_threshold;  
  // The interval in sec when MDM should probe in pre-threhold phase
  uint16    pre_threshold_probe_interval;    
  // The period in sec when MDM should not probe in post-threshold phase
  uint16    post_threshold_probe_silent_period;        
} ipv6_ext_addr_probe_config_items_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_EXT_ADDR_EVENT_TRIGGER_ENUM_TYPE

DESCRIPTION
  The trigger events for reporting to diag. The events from 0 to 17 map to 
  events in ps_ifacei_addr_mgmt_debug_buf_event_enum
---------------------------------------------------------------------------*/
typedef enum
{
  IPV6_EXT_ADDRESS_INVALID_EVENT = 0,
  IPV6_EXT_ADDRESS_ADDED_EVENT = 1,
  IPV6_EXT_ADDRESS_PROBED_EVENT = 2,
  IPV6_EXT_ADDRESS_DELETED_UNUSED_IID_TIMER_EXP_EVENT = 3,
  IPV6_EXT_ADDRESS_DELETED_FREE_BY_LINKED_IFACE_EVENT = 4,
  IPV6_EXT_ADDRESS_DELETED_DO_DAD_FAILED_EVENT = 5,
  IPV6_EXT_ADDRESS_DELETED_DO_DAD_UPDATE_FAILED_EVENT = 6,
  IPV6_EXT_ADDRESS_DELETED_DO_DAD_UPDATE_NO_MEM_EVENT = 7,
  IPV6_EXT_ADDRESS_DELETED_DO_DAD_UPDATE_TIMEOUT_EVENT = 8,
  IPV6_EXT_ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS_RM_EVENT = 9,
  IPV6_EXT_ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS_UM_EVENT = 10,
  IPV6_EXT_ADDRESS_DELETED_FREE_ALL_EXT_V6_ADDRESS_EVENT = 11,
  IPV6_EXT_ADDRESS_DELETED_UNUSED_IID_RM_IFACE_NULL_EVENT = 12,   
  IPV6_EXT_ADDRESS_DELETED_UNUSED_IID_SEND_NS_FAIL_EVENT =13,
  IPV6_EXT_ADDRESS_DELETED_PREFIX_CHANGED_EVENT = 14,
  IPV6_EXT_ADDRESS_DELETED_REMOVE_V6_PREFIX_EVENT = 15,
  IPV6_EXT_ADDRESS_DELETED_DELETE_ALL_PREFIX_EVENT = 16,
  IPV6_EXT_ADDRESS_DELETED_DELETE_IN_TRAT_EVENT = 17, 
  IPV6_EXT_ADDRESS_PROBE_ACKED_EVENT = 18,
  IPV6_EXT_ADDRESS_LOOKUP_FAIL_EVENT = 19
} ps_iface_ipv6_ext_addr_event_trigger_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_IFACE_IPV6_EXT_ADDR_EVENT_TYPE

DESCRIPTION
  Payload for reporting to diag
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  uint32                                         profile_id;
  ps_iface_name_enum_type                        iface_name;
  uint8                                          iface_instance;
  uint64                                         prefix;
  uint64                                         iid;
  ps_iface_ipv6_ext_addr_event_trigger_enum_type trigger;  
} ps_iface_ipv6_ext_addr_event_payload_type;

/*----------------------------------------------------------------------------
  Default configuration for IPv6 external address probe  
  All time is in second, threshold is in percentage
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_DEFAULT_PRE_THRESHOLD_PROBE_INTERVAL  0
#define IPV6_EXT_ADDR_DEFAULT_POST_THRESHOLD_SILENT_PERIOD  30
#define IPV6_EXT_ADDR_DEFAULT_THRESHOLD_PERCENT             80

/*----------------------------------------------------------------------------
  Modem will not probe the external address who are allocated in the following
  seconds
---------------------------------------------------------------------------*/
#define IPV6_EXT_ADDR_TENTATIVE_TIME 3
/*----------------------------------------------------------------------------
  Check if it is at pre-threshold stage or post-threshold stage
---------------------------------------------------------------------------*/
#define PS_IFACEI_V6_EXT_ADDRS_THRESHOLD_TRIGGERED(ps_iface_ptr) \
        (ps_ifacei_v6_get_cache_count(ps_iface_ptr) >\
        ps_ifacei_v6_get_cache_threshold())

/*----------------------------------------------------------------------------
  Check if the silent timer is allocated
---------------------------------------------------------------------------*/
#define PS_IFACEI_V6_EXT_ADDRS_SILENT_TIMER_IS_ALLOC(this_iface_ptr) \
        (this_iface_ptr->iface_private.ipv6_ext_addr_silent_timer != \
        PS_TIMER_INVALID_HANDLE)

/*----------------------------------------------------------------------------
  Check if the silent timer is running
---------------------------------------------------------------------------*/
#define PS_IFACEI_V6_EXT_ADDRS_SILENT_TIMER_IS_RUNNING(this_iface_ptr) \
        (ps_timer_is_running\
        (this_iface_ptr->iface_private.ipv6_ext_addr_silent_timer))

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         PUBLIC MACRO DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
MACRO PS_IFACE_IPV6_IS_PRIV_IID_UNIQUE()

DESCRIPTION
  This macro checks to see if the iid is locally unique to the interface.

PARAMETERS
  iface_ptr:   Target iface ptr
  uint64:      IID to be verified as unique

RETURN VALUE
  TRUE  if the IID is unique on the interface
  FALSE if the IID is not unique.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IPV6_IS_PRIV_IID_UNIQUE ps_iface_ipv6_is_priv_iid_unique
INLINE boolean ps_iface_ipv6_is_priv_iid_unique
(
  ps_iface_type        *iface_ptr,
  uint64               *priv_iid
)
{
  uint8 addr_index;
  ps_iface_type          * base_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !PS_IFACE_IS_VALID( iface_ptr ) )
  {
    LOG_MSG_ERROR_0("Invalid iface_ptr in ps_iface_ipv6_is_priv_iid_unique");
    ASSERT(0);
    return FALSE;
  }

  base_iface_ptr = PS_IFACE_GET_BASE_IFACE( iface_ptr );  
  /*-------------------------------------------------------------------------
    Verify this IID does not already exist on this interface.
  -------------------------------------------------------------------------*/
  for(addr_index = 0; addr_index < MAX_IPV6_ADDRS; addr_index++)
  {
    if( base_iface_ptr->iface_private.ipv6_addrs[addr_index] == NULL )
    {
      continue;
    }

    if( (base_iface_ptr->iface_private.ipv6_addrs[addr_index]->iid == *priv_iid) ||
        (base_iface_ptr->iface_private.ipv6_addrs[addr_index]->gateway_iid ==
           *priv_iid))
    {
      return FALSE;
    }
  } /* end for */    
  
  return TRUE;
}


/*===========================================================================
MACRO PS_IFACE_IPV6_IS_ADDR_SPACE_AVAIL()

DESCRIPTION
  This macro checks to see if there is space available on the interface for
  an additional IPv6 address.

PARAMETERS
  iface_ptr:   Target iface ptr

RETURN VALUE
  TRUE  if space is available.
  FALSE if there is no space available.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IPV6_IS_ADDR_SPACE_AVAIL ps_iface_ipv6_is_addr_space_avail
INLINE boolean ps_iface_ipv6_is_addr_space_avail
(
  ps_iface_type        *iface_ptr,
  uint8                *index
)
{
  uint8  addr_index;
  ps_iface_type * base_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !PS_IFACE_IS_VALID( iface_ptr ) )
  {
    LOG_MSG_ERROR_0("Invalid iface_ptr in ps_iface_ipv6_is_addr_space_avail");
    ASSERT(0);
    return FALSE;
  }

  base_iface_ptr = PS_IFACE_GET_BASE_IFACE( iface_ptr );
  /*-------------------------------------------------------------------------
    If we hit NULL there is space available. Start from 1 as 0 is always the
    public addr.
  -------------------------------------------------------------------------*/
  for(addr_index = MAX_IPV6_PREFIXES; addr_index < MAX_IPV6_ADDRS; addr_index++)
  {

    if(base_iface_ptr->iface_private.ipv6_addrs[addr_index] == NULL)
    {
        *index = addr_index;
        return TRUE;
    }
  } /* end for */

  return FALSE;
}


/*===========================================================================
MACRO PS_IFACEI_FIND_IPV6_ADDR()

DESCRIPTION
  This macro looks for a particular type of privacy address on an
  interface.

PARAMETERS
  iface_ptr:   Target iface ptr
  addr_state:
  addr_type:

RETURN VALUE
  TRUE         If an address match is found.
  FALSE        If no privacy address match is found.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACEI_FIND_IPV6_ADDR_TYPE ps_ifacei_find_ipv6_addr_type
INLINE boolean ps_ifacei_find_ipv6_addr_type
(
  ps_iface_type                      *iface_ptr,
  ps_iface_ipv6_addr_state_enum_type  addr_state,
  ps_iface_ipv6_addr_type_enum_type   addr_type,
  ps_ifacei_v6_addr_type            **v6_addr_ptr_ptr
)
{
  uint8 addr_index;
  ps_iface_type          * base_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !PS_IFACE_IS_VALID( iface_ptr ) )
  {
    LOG_MSG_ERROR_0("Invalid iface_ptr in ps_ifacei_find_ipv6_addr_type");
    ASSERT(0);
    return FALSE ;
  }

  base_iface_ptr = PS_IFACE_GET_BASE_IFACE( iface_ptr );
  /*-------------------------------------------------------------------------
    The default index will always be a public address. Return that if no
    available address is found.
  -------------------------------------------------------------------------*/
  for(addr_index = 0; addr_index < MAX_IPV6_ADDRS; addr_index++)
  {
    if( base_iface_ptr->iface_private.ipv6_addrs[addr_index] == NULL )
  {
      continue;
    }

    if( (base_iface_ptr->iface_private.ipv6_addrs[addr_index]->addr_state ==
           addr_state) &&
        (base_iface_ptr->iface_private.ipv6_addrs[addr_index]->addr_type ==
           addr_type) )
    {
      *v6_addr_ptr_ptr = base_iface_ptr->iface_private.ipv6_addrs[addr_index];
      return TRUE;
    }
  } /* end for */

  return FALSE;
}

/*===========================================================================
MACRO PS_IFACE_IPV6_DEFAULT_ADDR_STATE_VALID()

DESCRIPTION
  This macro checks to see if the default prefix of the interface is in
  a valid state for sending. (ie. VALID state, not DEPRECATED)

PARAMETERS
  this_iface_ptr: Target iface ptr

RETURN VALUE
  TRUE  - the default prefix is in valid state
  FALSE - the default prefix is not valid

DEPENDENCIES
  None

SIDE EFFECTS
  None
================================================== =========================*/
#define PS_IFACE_IPV6_DEFAULT_ADDR_STATE_VALID ps_iface_ipv6_default_addr_state_valid
INLINE boolean ps_iface_ipv6_default_addr_state_valid
(
  ps_iface_type * iface_ptr
)
{
  ps_iface_type          * base_iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !PS_IFACE_IS_VALID( iface_ptr ) )
  {
    LOG_MSG_ERROR_0("Invalid iface_ptr in ps_iface_ipv6_default_addr_state_valid");
    ASSERT(0);
    return FALSE ;
  } 

  base_iface_ptr = PS_IFACE_GET_BASE_IFACE( iface_ptr );
  if( ( PS_IFACE_IS_VALID( base_iface_ptr ) ) &&
      ( base_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX] != NULL ) &&
      ( base_iface_ptr->iface_private.ipv6_addrs[DEFAULT_V6_INDEX]->addr_state == IPV6_ADDR_STATE_VALID)
    )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
MACRO PS_IFACE_IPV6_EXTRA_ADDR_IS_AVAIL()

DESCRIPTION
  This macro checks to see if there are available unassigned IPv6
  privacy addresses. If the iface is a broadcast or proxy interface it
  also checks to see if there are any DAD pending or completed addresses.

PARAMETERS
  this_iface_ptr: Target iface ptr
  v6_addr_ptr   : Ptr to the v6 address structure

RETURN VALUE
  TRUE is there is an extra address available
  FALSE if there is no extra address already available

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IPV6_EXTRA_ADDR_IS_AVAIL(this_iface_ptr,v6_addr_ptr)      \
     (PS_IFACEI_FIND_IPV6_ADDR_TYPE(this_iface_ptr,                        \
                                    IPV6_ADDR_STATE_UNASSIGNED,            \
                                    IPV6_ADDR_TYPE_INVALID,                \
                                    v6_addr_ptr) ||                        \
      ((PS_IFACE_GET_IS_BCAST_IFACE(this_iface_ptr) ||                     \
        PS_IFACE_GET_IS_PROXY_IFACE(this_iface_ptr)) &&                    \
       PS_IFACEI_FIND_IPV6_ADDR_TYPE(this_iface_ptr,                      \
                                     IPV6_ADDR_STATE_TENTATIVE,           \
                                     IPV6_ADDR_TYPE_INVALID,              \
                                     v6_addr_ptr) ))

/*===========================================================================
FUNCTION PS_IFACEI_IPV6_IID_TIMER_HANDLER()

DESCRIPTION
  This function is a callback for the privacy extension lifetime timers. It
  is called when an IPv6 private IID timer expires.  It will take the
  appropriate action depedning upon if the timer that expired was the
  preferred or valid lifetime.

PARAMETERS
  user_data_ptr: the callback information which will be the IPv6 address
                 information structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_ipv6_iid_timer_handler
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_IPV6_UNUSED_IID_HANDLER()

DESCRIPTION
  This function serves as a callback for three purposes:
  1)The retry timer for DAD verification expires. In this case (only for
    internal addresses) the timer is restarted (if tries remain) and the
    dad function called.
  2)The mobile needs to check that an external address is still being used.
  2)The privacy extension unsed timer to indicate when an IPv6 private address
    has not been bound by a socket in the required time. The function will
    notify the application(s) and free the address.

PARAMETERS
  user_data_ptr: the callback information which will be the IPv6 address
                 information structure

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_ipv6_unused_iid_handler
(
  void *user_data_ptr
);

/*===========================================================================
PS_IFACE_ADDR_V6_INIT

DESCRIPTION
  Initializes ps_iface_addr_v6 module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_iface_addr_v6_init
(
  void
);

/*===========================================================================
FUNCTION PS_IFACE_FIND_IPV6_ADDR()

DESCRIPTION
  This function locates an IPv6 address on any UP interface.

PARAMETERS
  ip_addr_ptr:       Pointer to the ip address
  v6_addr_ptr_ptr:   Pointer to the v6_addr_ptr to fill if the addr is found
  ps_iface_ptr_ptr:  Pointer to the ps_iface_ptr to return if the address
                     matches one owned by that interface.

RETURN VALUE
  TRUE  if the address is located
  FALSE if the address cannot be found

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_find_ipv6_addr
(
  struct ps_in6_addr       *ip_addr_ptr,
  ps_ifacei_v6_addr_type  **v6_addr_ptr_ptr,
  ps_iface_type           **ps_iface_ptr_ptr
);

/*===========================================================================
FUNCTION PS_IFACE_FIND_IPV6_ADDR_EX()

DESCRIPTION
  This function locates an IPv6 address on any UP interface. While searching
  it includes the IPv6 of address type specified in addr_type_mask mask.

PARAMETERS
  ip_addr_ptr:       Pointer to the ip address
  v6_addr_ptr_ptr:   Pointer to the v6_addr_ptr to fill if the addr is found
  ps_iface_ptr_ptr:  Pointer to the ps_iface_ptr to return if the address
                     matches one owned by that interface.
  addr_type_mask:    IPv6 address type mask that indicates what type of
                     addresses should be included in the search. 

RETURN VALUE
  TRUE  if the address is located
  FALSE if the address cannot be found

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_find_ipv6_addr_ex
(
  struct ps_in6_addr                      *ip_addr_ptr,
  ps_ifacei_v6_addr_type                 **v6_addr_ptr_ptr,
  ps_iface_type                          **ps_iface_ptr_ptr,
  ps_iface_ipv6_addr_type_mask_enum_type   addr_type_mask
);

/*===========================================================================
FUNCTION PS_IFACE_CHECK_IPV6_IID_CONFLICT()

DESCRIPTION
  This function checks for a conflict of passed IID with any of the
  associated iid's of an iface.

PARAMETERS
  *this_iface_ptr - Pointer to the interface to operate on.
   iid            - IID to check for conflict.
   exclude_primary_iid - if set dont include primary IID 
                         while checking for conflict

RETURN VALUE 
  -1 in case of any error. 
   1 in case of conflict
   0 otherwise.

DEPENDENCIES

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_check_ipv6_iid_conflict
(
  ps_iface_type *this_iface_ptr, /* Pointer to the interface to operate on */
  uint64         iid,            /* IID to check for conflict */
  boolean        exclude_primary_iid /* if set dont include primary IID 
                                        while checking for conflict */
);

/*===========================================================================
FUNCTION PS_IFACE_TRANSFER_IPV6_IID()

DESCRIPTION
  This function transfers the IPv6 IID from source iface to destination iface.
  It also takes care to checking if the IID is in conflict with any
  other IID on destination iface.

PARAMETERS
  *src_iface_ptr - Pointer to the interface from which IID will be copied.
  *dst_iface_ptr - Pointer to the interface to which IID will be copied.
   iid           - IID that need to be transfered.
  *ps_errno      - Error Reason when failed.


RETURN VALUE 
  -1 in case of any error. 
   0 in case of sucess

DEPENDENCIES
  This function should only be called when the primary IID is present
  on destination iface. 
 
SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_transfer_ipv6_iid
(
  ps_iface_type *src_iface_ptr, /* Pointer to the interface from which 
                                   ipv6 iid will be copied. */
  ps_iface_type *dst_iface_ptr, /* Pointer to the interface to where 
                                   ipv6 iid will get copied */
  uint64         iid,            /* The IID that will be transfered */
  int16         *ps_errno
);

#ifdef FEATURE_DATA_PS_MIPV6
/*===========================================================================
FUNCTION PS_IFACE_SET_MIP6_BOOTSTRAP_CFG_INFO

DESCRIPTION
  This function sets the MIP6 bootstrap config info on an iface. This function
  overwrites the previously stored info, if any.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  mip6_cfg_info : ptr to mip6 bootstrap config info
  ps_errno      : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  ps_errno is set if function fails
===========================================================================*/
int ps_iface_set_mip6_bootstrap_cfg_info
(
  ps_iface_type                         *this_iface_ptr,
  ps_iface_mip6_bootstrap_cfg_info_type *mip6_cfg_info,
  errno_enum_type                       *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_GET_MIP6_BOOTSTRAP_CFG_INFO

DESCRIPTION
  This function gets the MIP6 bootstrap config info stored in the iface.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  mip6_cfg_info : ptr to mip6 bootstrap config info
  ps_errno      : return error code.

RETURN VALUE
  0 if successful
 -1 if fails

DEPENDENCIES
  None

SIDE EFFECTS
  ps_errno is set if function fails
===========================================================================*/
int ps_iface_get_mip6_bootstrap_cfg_info
(
  ps_iface_type                         *this_iface_ptr,
  ps_iface_mip6_bootstrap_cfg_info_type *mip6_cfg_info,
  errno_enum_type                       *ps_errno
);
#endif /* FEATURE_DATA_PS_MIPV6 */

/*===========================================================================
FUNCTION PS_IFACE_GENERATE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will generate a new private IPv6 as per RFC 3041. Based on
  the input to the function it will create a private shared (interface
  based) or private unique (owned and useable only by a particular socket).

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    value return - the address will be stored here

RETURN VALUE
  0 on successful generation of private ipv6 address
 -1 on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_generate_priv_ipv6_addr
(
  ps_iface_type           *this_iface_ptr,
  ps_ip_addr_type         *ip_addr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  int16                   *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_ALLOC_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will verify address space is available, allocate a ps mem
  item for a new private IPv6 address, allocate timers for the address,
  and populate the address with the necessary state information. In addition
  this function will start the DAD process for broadcast interfaces.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  ip_addr_ptr:    return value - the address will be stored here
  iid_param_ptr:  The type of address to create as well as whether it was
                  an application that requested the address or not.
  ps_errno:       the returned error code

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_alloc_priv_ipv6_addr
(
  ps_iface_type           *this_iface_ptr,
  ps_ifacei_v6_addr_type **v6_addr_ptr,
  ps_ipv6_iid_params_type *iid_param_ptr,
  boolean                  extra_addr,
  int16                   *ps_errno
);

/*===========================================================================
FUNCTION PS_IFACE_DELETE_PRIV_IPV6_ADDR()

DESCRIPTION
  This function will delete an old private IPv6 address. Once the ref_cnt
  has gone to zero this function will take care of deleting and cleaning up
  the interface's ipv6 addresses.

PARAMETERS
  this_iface_ptr: ptr to interface control block on which to operate on.
  v6_addr_ptr:    the ipv6 address location to be removed from the interface

RETURN VALUE
  0 for success
 -1 for failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_iface_delete_priv_ipv6_addr
(
  ps_iface_type                  *this_iface_ptr,
  ps_ifacei_v6_addr_type         *v6_addr_ptr
);

#ifdef __cplusplus
}
#endif
/*===========================================================================
FUNCTION PS_IFACE_GET_IPV6_EXT_ADDR()

DESCRIPTION
  This function fetches the external IPv6 address that is not link local

PARAMETERS
  iface_ptr:       Pointer to iface
  ip_addr_ptr:     OUT Parameter. Pointer to ip address


RETURN VALUE
  TRUE  if the address is located
  FALSE if the address cannot be found

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_iface_get_ipv6_ext_addr
(
  ps_iface_type            *iface_ptr,
  ps_ip_addr_type           *ip_addr_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_GET_PROBE_CONFIG_ITEMS

DESCRIPTION
  This function returns the ipv6 external address probe config information

PARAMETERS

RETURN VALUE
  The pointer point to config items

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
ipv6_ext_addr_probe_config_items_type *
ps_ifacei_v6_get_probe_config_items
(
  void
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_GET_CACHE_COUNT

DESCRIPTION
  This function retunrs the number of entries in v6 iface address cache

PARAMETERS
  The pointer point to UM iface
  
RETURN VALUE
 The number of entries in v6 iface address cache

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ifacei_v6_get_cache_count
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_GET_CACHE_THRESHOLD()

DESCRIPTION
  This function returns the probing threshold in number

PARAMETERS
  None

RETURN VALUE
  The probing threshold
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_ifacei_v6_get_cache_threshold
( 
  void
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_PROBE_ALL_EXT_ADDRS()

DESCRIPTION
  Probes all valid external IPv6 addresses

PARAMETERS
  iface_ptr: ptr to interface on which to operate on.

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  It will cancel all the unused iid timers
===========================================================================*/
void ps_ifacei_v6_probe_all_ext_addrs
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_RESTART_ALL_UNUSED_IID_TIMER()

DESCRIPTION
  This function will restart all external addresses's unused iid timer  

PARAMETERS
  iface_ptr: ptr to interface on which to operate on.

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_v6_restart_all_unused_iid_timer
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_SILENT_TIMER_HANDLER()

DESCRIPTION
  A dummy call back function of silent timer 

PARAMETERS
  user_data_ptr: ptr to argument

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_ipv6_silent_timer_handler
(
  void *user_data_ptr
);

/*===========================================================================
FUNCTION PS_IFACEI_V6_EXT_ADDR_EVENT_REPORT()

DESCRIPTION
  This function report an ipv6 external address event to diag

PARAMETERS
  iface_ptr:   pointer to um iface
  event_type:  the event type report to diag
  v6_addr_ptr: pointer to ipv6 address

RETURN VALUE
  True: Success
  Fales: Falure
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_ifacei_ipv6_ext_addr_event_report
(
  ps_iface_type                                  *iface_ptr,
  ps_iface_ipv6_ext_addr_event_trigger_enum_type  event_type,
  ps_ifacei_v6_addr_type                         *v6_addr_ptr
);

#endif /* FEATURE_DATA_PS && FEATURE_DATA_PS_IPV6 */
#endif /* PS_IFACEI_ADDR_V6_H */
