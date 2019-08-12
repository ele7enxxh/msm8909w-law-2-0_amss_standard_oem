#ifndef PS_ROUTE_IFACE_H
#define PS_ROUTE_IFACE_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              S U  -  L E V E L  -  I F A C E  A N D  R O U T I N G

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  This file provides SU Level api's used by DS + PS


Copyright (c) 2002-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/10   as      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ps_acl.h"
#include "ps_iface_defs.h"

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_ROUTE_LOOK_UP_IFACE_BY_POLICY()

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.
  It also calls net_arbitrate() to choose an iface if a
  requesting conflicting app was denied by the ACL rules.

DEPENDENCIES
  bring_up flag in policy info should be TRUE.

PARAMETERS
  acl_pi_ptr         - Ptr to packet's policy information
  arb_candidate      - If the iface will be arb candidate
  iface_priority_mask- Priority mask for iface
  net_down_reason    - Ptr to net down reason

RETURN VALUE
  Iface ptr - NULL on failure

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type * ps_route_look_up_iface_by_policy
(
  acl_policy_info_type          * acl_pi_ptr,
  boolean                       * arb_candidate,
  uint32                        * iface_priority_mask,
  ps_iface_net_down_reason_type * net_down_reason
);

/*===========================================================================
FUNCTION PS_ROUTE_BRING_UP_IFACE_BY_POLICY_EX()

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.
  It also calls net_arbitrate() to choose an iface if a
  requesting conflicting app was denied by the ACL rules.
  If an iface is selected for the app, that iface is brought
  up with the requesting app's priority.

DEPENDENCIES
  bring_up flag in policy info should be TRUE.

PARAMETERS
  acl_pi_ptr       - Ptr to packet's policy information
  if_ptr           - Ptr local interface route goes through
  ps_errno         - Ptr to dss errno value
  client_data_ptr  - Ptr to client data passed in by client
  net_down_reason  - Ptr to net down reason

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int ps_route_bring_up_iface_by_policy_ex
(
  acl_policy_info_type          * acl_pi_ptr,
  ps_iface_type                 ** if_ptr,
  int16                         * ps_errno,
  void                          * client_data_ptr,
  ps_iface_net_down_reason_type * net_down_reason
);

#endif /* PS_ROUTE_IFACE_H */

