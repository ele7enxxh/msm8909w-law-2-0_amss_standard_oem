#ifndef PS_ROUTE_H
#define PS_ROUTE_H
/*===========================================================================

                        D A T A   S E R V I C E S

                    I P   R O U T I N G   M O D U L E

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  The IP routing module enables IP address based routing between
  up interfaces as well as policy based routing.
  Routing decisions are based on policies defined as ACLs with
  IP address routing as the default fallback mechanism.

  ACLs (Access Control Lists) provide a set of rules to define
  how suitable an interface is for transmission of a given IP
  packet. The rules allow consideration of parameters such as
  port numbers, quality-of-service identifiers, air-interface
  type and the like and hence allow routing decisions beyond the
  regular address based routing. A set of rules is also referred
  to as the interfaces routing policy. The syntax of ACL rules is
  defined in ps_aclrules.h.

  This file provides the API to the routing module. It is independent
  of the IP version used. The API functions differentiate between
  IPv4 and IPv6 and call the appropriate internal functions (routei_...),
  which are version specific.

EXTERNALIZED FUNCTIONS
  route_add()         - Add a route to the routing table
  route_delete()      - Remove a route from the routing table
  route_update()      - Update an existing route in the routing table
  route_get()         - Find a route via ACLs and the routing table
  route_flush()       - Flush all routes or all routes for one iface
  route_acl_add()     - Add an ACL for a specific interface
  route_acl_delete()  - Remove an ACL for a specific interface
  route_acl_control() - En- / disable an existing ACL temporarily
  route_acl_execute() - Execute the specified ACL list to get best match if

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_route.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/04/13   svj     New DIAG event to generate data cal status information 
07/15/10   as      Brought over api: ps_route_bring_up_iface_by_policy from ps_routei
12/14/08   pp      Common Modem Interface: Public/Private API split.
11/02/06   mct     Added support for RFC3041: IPv6 Privacy Extensions.
07/31/06   rt      Route Scope modifications.
04/20/06   mct     Added loopback optimizations.
04/18/05   vp      Changes for enforcing netpolicy on input.
06/11/04   vp      Changes for renaming os ip_addr_type to ps_ip_addr_type
03/16/04   mct     Renamed acl_ipfilter_info_type to ip_pkt_info_type.
02/03/04   usb     Modified route_acl_control() prototype/definition.
01/31/04   usb     Added support for multiple ACL lists.
02/20/03   om      More comments.
11/05/02   om      Added enum and union for ACL identifiers.
03/08/02   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_acl.h"
#include "ps_in.h"
#include "ps_iface_defs.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/* Types of routes */
typedef enum
{
  ROUTE_DEFAULT = 0,                  /* Default route (dst 0.0.0.0)       */
  ROUTE_HOST,                         /* Host specific route               */
  ROUTE_NET                           /* Network specific route            */
} route_type_enum_type;

/* Define the address for default routes */
#define ROUTE_DEFAULT_DST  0x00000000


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION   ROUTE_ADD

DESCRIPTION
  This function allows to add a route to the DMSS routing
  table.

DEPENDENCIES
  None

PARAMETERS
  type        - Specifies whether it is a host or network route
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  ds_errno    - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_add
(
  route_type_enum_type  type,
  ps_ip_addr_type       destination,
  ps_ip_addr_type       netmask,
  ps_ip_addr_type       gateway,
  ps_iface_type*        if_ptr,
  uint8                 metric,
  uint16*               cost_ptr
);

/*===========================================================================

FUNCTION   ROUTE_DELETE

DESCRIPTION
  This function allows to remove a route from the DMSS
  routing table.

DEPENDENCIES
  None

PARAMETERS
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_delete
(
  ps_ip_addr_type   destination,
  ps_ip_addr_type*  gateway
);

/*===========================================================================

FUNCTION   ROUTE_UPDATE

DESCRIPTION
  This function allows to update an existing entry in the
  DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  ds_errno    - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_update
(
  ps_ip_addr_type destination,
  ps_ip_addr_type gateway,
  ps_iface_type*  if_ptr,
  uint8*          metric_ptr,
  uint16*         cost_ptr
);

/*===========================================================================

FUNCTION   ROUTE_GET

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.

DEPENDENCIES
  Currently addr based routing is supported for v4 only.
  If ip_fi_ptr is NULL, gateway_ptr must be NULL and addr_based_routing_only
  must be FALSE.

PARAMETERS
  ip_fi_ptr        - Ptr to packet's IP information
  acl_pi_ptr       - Ptr to packet's policy information
  addr_based_routing_only - Don't use ACLs for routing decision if true
  gateway_ptr      - Ptr to IP address of the routing gateway in network order
  if_ptr           - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_get
(
  ip_pkt_info_type*        ip_pkt_ptr,
  acl_policy_info_type*    acl_pi_ptr,
  boolean                  addr_based_routing_only,
  ps_ip_addr_type*         gateway_ptr,
  ps_iface_type**          if_ptr
);

/*===========================================================================

FUNCTION   ROUTE_FLUSH

DESCRIPTION
  This function allows to flush the complete DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  iface_ptr  - Pointer to a specific interface to flush all routes
               for, set to NULL to flush entire routing table
  ds_errno   - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ds_errno contains error code).

SIDE EFFECTS
  No IP traffic can be sent or forwarded until a new route
  is added to the DMSS routing table.
===========================================================================*/
int route_flush
(
  ps_iface_type*  iface_ptr
);

/*===========================================================================
FUNCTION PS_ROUTE_BRING_UP_IFACE_BY_POLICY()

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

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int ps_route_bring_up_iface_by_policy
(
  acl_policy_info_type    * acl_pi_ptr,
  ps_iface_type          ** if_ptr,
  int16                   * ps_errno
);

/*===========================================================================

FUNCTION PS_ROUTE_TEAR_DOWN_IFACE_BY_POLICY()

DESCRIPTION
  This function is used to tear down an iface.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  acl_pi_ptr: ptr to the policy
  ps_errno: error code to return to caller.
  client_data_ptr: data, that is passed to the client handler function.

RETURN VALUE
  0: on success
 -1: on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int ps_route_tear_down_iface_by_policy
(
  ps_iface_type         *this_iface_ptr,
  acl_policy_info_type      *acl_pi_ptr,
  int16                       *ps_errno,
  void                 *client_data_ptr
);

#ifdef __cplusplus
  }
#endif

#endif    /* PS_ROUTE_H */
