#ifndef PS_ROUTEI_ACL_H
#define PS_ROUTEI_ACL_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   R O U T I N G   A C  L

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  This file defines the structure and management functions for
  the routing ACLs.

  Functions and data structures are specific to the IP version
  and named accordingly.

EXTERNALIZED FUNCTIONS
  routei_acl_list()       - Accessor function for the registered-ACL list

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 - 2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/10   asn     Support for PS Arbitration
01/21/09   ar      Move routing callback paramater list into structure
12/15/08   am      Added deny_list_size arg to acl_priority_cb.
12/14/08   pp      Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "ps_route.h"
#include "ps_socket_defs.h"

/* <EJECT> */ 
/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
/* Type of ACL */
typedef enum
{
  ROUTE_ACL_OUTBOUND_MIN      = 0,
  ROUTE_ACL_DEFAULT_OUTBOUND  = ROUTE_ACL_OUTBOUND_MIN,
  ROUTE_ACL_PHYSICAL_OUTBOUND = ROUTE_ACL_DEFAULT_OUTBOUND,
  ROUTE_ACL_LOGICAL_OUTBOUND  = 1,
  ROUTE_ACL_OUTBOUND_MAX      = 2,
  ROUTE_ACL_DEFAULT_INBOUND   = ROUTE_ACL_OUTBOUND_MAX,
  ROUTE_ACL_PHYSICAL_INBOUND  = ROUTE_ACL_DEFAULT_INBOUND,
  ROUTE_ACL_DEFAULT_NONTUNNEL_INBOUND = 3,
  ROUTE_ACL_TYPE_MAX
} route_acl_type_enum_type;

/* Priority of an ACL */
typedef enum
{
  ROUTE_ACL_ANY_PRIORITY = 0,          /* Append ACL to DMSS list          */
  ROUTE_ACL_TOP_PRIORITY,              /* Insert ACL at the top of a group */
  ROUTE_ACL_LEAST_PRIORITY             /* Insert ACL at the end of a group */
} route_acl_priority_enum_type;

/* Identifying an ACL by its ptr or associated iface & list for control */
typedef enum
{
  ROUTE_ACL_PTR_ID = 0,
  ROUTE_ACL_INFO_ID = 1
} route_acl_id_enum_type;

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
typedef union
{
  acl_type*       acl_ptr;
  struct
  {
    route_acl_type_enum_type list_type;
    ps_iface_type*           if_ptr;
  } acl_info;
} route_acl_id_type;

/*---------------------------------------------------------------------------
  Routing ACL priority search callback.
---------------------------------------------------------------------------*/
typedef struct {
  ip_pkt_info_type*        ip_pkt_ptr;
  acl_policy_info_type*    acl_pi_ptr;
  ps_ip_addr_type*         gateway_ptr;
  ps_iface_type**          deny_list_ptr;
  uint8                    deny_list_arr_size;
  ps_iface_type**          deny_iface_in_use_arr_ptr;
  uint8                    iface_in_use_arr_size;
  uint8*                   num_iface_in_use_ptr;
  ps_iface_type**          deny_iface_in_use_cfg_match_arr_ptr;
  uint8                    iface_in_use_cfg_match_arr_size;
  uint8*                   num_iface_in_use_cfg_match_ptr;
  ps_iface_type**          if_ptr;
} ps_route_lookup_params_type;

typedef int (*route_acl_priority_cb_func)
(
  ps_route_lookup_params_type * params_ptr,
  acl_type**                    acl_match_ptr,
  acl_rt_result_type*           result_ptr,
  int *                         deny_reason
);

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION   ROUTEI_ACL_LIST()

DESCRIPTION
  This functions returns the pointer to the first ACL in the
  kernel's list of routing ACLs.

DEPENDENCIES
  None

PARAMETERS
  list_type - Type of ACL list to look for.

RETURN VALUE
  Pointer to ACL list

SIDE EFFECTS
  None
===========================================================================*/
acl_type** routei_acl_list
(
  route_acl_type_enum_type list_type
);

/*===========================================================================

FUNCTION   ROUTEI_ACL_POST_PROCESSING()

DESCRIPTION
  This function is the common post processing function for the outbound
  routing ACLs. This function gets called for all ifaces and performs
  common post processing tasks for the iface that's picked up as a result
  of the execution of the ACL.

DEPENDENCIES
  None

PARAMETERS
  this_if_ptr - Iface ptr
  policy_info_ptr - Routing policy Info
  ipfilter_info_ptr - IP packet info

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void routei_acl_post_processing
(
  void*                    this_if_ptr,
  acl_policy_info_type*    policy_info_ptr,
  ip_pkt_info_type*        pkt_info_ptr
);

/*===========================================================================

FUNCTION   ROUTE_ACL_ADD

DESCRIPTION
  This function allows to add an Access Control List to the
  DMSS ACL list used for policy based routing.
  The ACL's priority can be specified globally (no interface
  class) or within a specific interface class.

DEPENDENCIES
  The ACL points to a valid interface instance.

PARAMETERS
  list_type  - ACL list to add to
  acl_ptr   - Ptr to ACL, memory provided by caller
  priority  - Priority for this ACL within the ACL list

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_add
(
  route_acl_type_enum_type           list_type,
  acl_type*                          new_acl_ptr,
  route_acl_priority_enum_type       priority
);

/*===========================================================================

FUNCTION   ROUTE_ACL_DELETE

DESCRIPTION
  This function allows to remove an existing ACL from the
  DMSS ACL list for policy based routing.

DEPENDENCIES
  None

PARAMETERS
  list_type     - ACL list to delete from
  del_acl_ptr  - Ptr to ACL, memory provided by caller

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_delete
(
  route_acl_type_enum_type  list_type,
  acl_type*                 del_acl_ptr
);

/*===========================================================================

FUNCTION   ROUTE_ACL_CONTROL

DESCRIPTION
  This function en- and disables an ACL to be used for routing lookups.
  ACLs are enabled by default when registered.

DEPENDENCIES
  The ACL points to a valid interface instance.

PARAMETERS
  acl_id_type  - Specifies whether ACL is identified by its pointer
                 or its associated interface and list type
  acl_id       - ACL identifier
  acl_enable   - Control flag (TRUE for enabling, FALSE for disabling)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void route_acl_control
(
  route_acl_id_enum_type   acl_id_type,
  route_acl_id_type        acl_id,
  boolean                  acl_enable
);

/*===========================================================================

FUNCTION   ROUTE_ACL_SET_PRIORITY

DESCRIPTION
  This function allows to update an ACL's priority globally
  or within a specific interface class.

DEPENDENCIES
  None

PARAMETERS
  list_type  - ACL list to operate on
  acl_ptr   - Ptr to ACL, memory provided by caller
  priority  - Priority for this ACL within the ACL list

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_set_priority
(
  route_acl_type_enum_type           list_type,
  acl_type*                          acl_ptr,
  route_acl_priority_enum_type       priority
);

/*===========================================================================

FUNCTION   ROUTE_ACL_EXECUTE

DESCRIPTION
  This function executes ACLs registered with a specified ACL list type and
  returns the iface associated with ACL which exhibits highest priority
  based upon the IP pkt and policy information passed.

DEPENDENCIES
  The ACL points to a valid interface instance.

PARAMETERS
  list_type    - ACL list to operate on
  ip_fi_ptr    - Ip pkt information
  acl_pi_ptr   - Policy information
  if_ptr       - returned iface ptr from highest priority ACL
                 NULL if no ACL passes or if error occurrs.

RETURN VALUE
 0 if successful, -1 in case of error.

SIDE EFFECTS
  ACL post processing function gets called for the highest priority ACL.
===========================================================================*/
int route_acl_execute
(
  route_acl_type_enum_type           list_type,
  ip_pkt_info_type*                  ip_pkt_ptr,
  acl_policy_info_type*              acl_pi_ptr,
  ps_iface_type**                    if_ptr
);

/*===========================================================================
FUNCTION PS_ROUTE_REGISTER_ACL_POLICY_ROUTING_FUNC()

DESCRIPTION
  This function registers an ACL policy routing routine for the specified
  processor ID.  If no callback is registered, the processor will be
  excluded from proxy IFACE routing selection.

  The callback table is ordered by the processor ID to ensure
  application/standalone processor is done last, after the proxy IFACE
  routing info has been updated.  Processor ID is assumed to be zero offset.

  The callback is invoked by ps_route_network_selection_by_policy().

DEPENDENCIES
  None.

PARAMETERS
  proc_id          - Processor identifier
  acl_priority_cb  - Routing ACL priority search callback

RETURN VALUE
  Previously registered callback pointer if any, NULL otherwise.

SIDE EFFECTS
  None
===========================================================================*/
route_acl_priority_cb_func ps_route_register_acl_policy_routing_func
(
  acl_proc_id_type            proc_id,
  route_acl_priority_cb_func  acl_priority_cb
);

#endif    /* ROUTEI_H */
