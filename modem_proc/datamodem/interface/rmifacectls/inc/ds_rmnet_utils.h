#ifndef _DS_RMNET_UTILS_H
#define _DS_RMNET_UTILS_H
/*===========================================================================

                        D S _ R M N E T _ U T I L S . H

DESCRIPTION

  Rm Network device - utility definitions internal header file.

EXTERNALIZED FUNCTIONS

Copyright (c) 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/rmifacectls/inc/ds_rmnet_utils.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/06/15    wc     Fix packet filtering issue
04/10/13    wc     MBIM mPDN support
02/01/12    cy     Initial implementation
===========================================================================*/

/*===========================================================================

                                INCLUDE FILES 

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "dsm.h"
#include "ds_rmnet_meta_sm.h"

/*===========================================================================

                                 DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  This is to generate a unique id based on ips_id present or not
---------------------------------------------------------------------------*/
#define IPS_ID_TO_FILTER_ID(ips_id_valid, ips_id, inst) \
            ((ips_id_valid) ? ((ips_id) | 0x0100) : ((inst) | 0x0200))

/*---------------------------------------------------------------------------
  Struct defintions for packet filter cache to store handles and rm ifaces
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type     link;
  ps_iface_type * rm_iface_ptr1;
  uint32          handle1;
  ps_iface_type * rm_iface_ptr2;
  uint32          handle2;
} rmnet_utils_packet_filter_handle;


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_CHANGE()

  DESCRIPTION
    Enable or disable packet filter setting for both the ifaces

  PARAMETERS
    filter_id  : packet filter id
    enabled    : enabled(1) or disabled(0)
    restrictive: Whitelist(1) or blacklist(0)

  RETURN VALUE
    boolean    : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_change
(
  uint16                filter_id,
  boolean               enabled,
  boolean               restrictive
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_STATE()

  DESCRIPTION
    Retrieve current packet filter setting for one of valid ifaces

  PARAMETERS
    filter_id  : packet filter id
    enabled    : enabled(TRUE) or disabled(FALSE)
    restrictive: Whitelist(TRUE) or blacklist(FALSE)

  RETURN VALUE
    boolean    : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_state
(
  uint16                filter_id,
  uint8               * enabled,
  uint8               * restrictive
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_ADD_RULE()

  DESCRIPTION
    Add one filter rule to both ifaces
    
  PARAMETERS
    filter_id: packet filter id
    handle   : pointer to rule handle
    len      : size of filter pattern/mask
    pattern  : pointer to filter pattern array
    mask     : pointer to filter mask array

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_add_rule
(
  uint16                filter_id,
  uint32              * handle,
  uint8                 len,
  uint8               * pattern,
  uint8               * mask
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_DELETE_ALL_RULES()

  DESCRIPTION
    Delete all filter rules for valid iface

  PARAMETERS
    filter_id    :   packet filter id

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_delete_all_rules
(
  uint16                filter_id
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_DELETE_RULE()

  DESCRIPTION
    Delete  filter rule using rule handle

  PARAMETERS
    handle: filter handle

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_delete_rule
(
  uint32                handle
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_RULE_HANDLES()

  DESCRIPTION
    Get the list of rule handles for valid iface

  PARAMETERS
    filter_id: packet filter id
    p_num    : pointer to number of filters
    p_handle : pointer to array of handles

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_rule_handles
(
  uint16                filter_id,
  uint8               * p_num,
  uint32              * p_handle
);

/*===========================================================================
  FUNCTION RMNET_UTILS_PACKET_FILTER_GET_RULE()

  DESCRIPTION
    Get filter rule using rule handle

  PARAMETERS
    handle   : rule handle
    p_buf    : pointer to bytes array in sequence of len/pattern/len/mask

  RETURN VALUE
    boolean  : TRUE for success, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean rmnet_utils_packet_filter_get_rule
(
  uint32                handle,
  uint8                *p_buf
);

/*===========================================================================
  FUNCTION RMNET_UTILS_INIT()

  DESCRIPTION
    Initializes rmnet utils module

  PARAMETERS
    
  RETURN VALUE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_utils_init
(
  void
);


/*===========================================================================
  FUNCTION rmnet_utils_packet_filter_handle_iface_down()

  DESCRIPTION
    Delete  filter rule using rule handle

  PARAMETERS
    filter_id    :   packet filter id
    rm_iface_ptr :   rm iface ptr

  RETURN VALUE
    void 

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void rmnet_utils_packet_filter_handle_iface_down
(
  uint16                filter_id,
  ps_iface_type       * rm_iface_ptr
);

#endif /* _DS_RMNET_UTILS_H */
