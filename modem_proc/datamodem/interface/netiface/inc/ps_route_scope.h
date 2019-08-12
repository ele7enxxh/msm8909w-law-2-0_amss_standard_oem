#ifndef PS_ROUTE_SCOPE_H
#define PS_ROUTE_SCOPE_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   R O U T I N G   S C O P E

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  This file defines the structure and management functions for
  the kernel's routing table. The table is a fixed size array
  for lookup efficiency.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/12   sj      Change PS_MAX_ROUTE_SCOPE_BITS to MAX_SYSTEM_IFACES
12/23/10   ss      Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
12/14/08   pp      Created module as part of Common Modem Interface:
                   Public/Private API split.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "ps_route.h"
#include "ps_routei_lo.h"
#include "ds_Utils_DebugMsg.h"


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Definitions for route_scope
---------------------------------------------------------------------------*/
#define PS_MAX_ROUTE_SCOPE_BITS  MAX_SYSTEM_IFACES
#define PS_MAX_ROUTE_SCOPE_INDEX                                            \
 ((PS_MAX_ROUTE_SCOPE_BITS + (sizeof(uint32) * 8) - 1) / (sizeof(uint32) * 8))

typedef struct ps_route_scope_s
{
  uint32 if_map[PS_MAX_ROUTE_SCOPE_INDEX];
} ps_route_scope_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#define PS_ROUTE_SCOPE_PRINT(route_scope_ptr)                            \
  LOG_MSG_INFO1_2("Route scope is 0x%x 0x%x",                            \
                route_scope_ptr->if_map[0], route_scope_ptr->if_map[1]);

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_ADD()

DESCRIPTION
  This function adds an interface to the route_scope list.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type * - Target route_scope.
  uint32                - Index for interface to add.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
INLINE void ps_route_scope_add
(
  ps_route_scope_type *route_scope,
  uint32               iface_index
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Check that iface index is not more than (PS_MAX_ROUTE_SCOPE_INDEX * 32)
  -------------------------------------------------------------------------*/
  if(iface_index >= PS_MAX_ROUTE_SCOPE_BITS)
  {
    /*lint -e{1534} */ ASSERT(0);
    return;
  }

  if(route_scope != NULL)
  {
    /*-----------------------------------------------------------------------
      We now need to find the bit which needs to be set to 1. We already have
      the list_index (iface_index / 32) in the if_map. (iface_index % 32)
      is essentially the bit in if_map[list_index] to be set to 1 for the
      passed iface_index.
    -----------------------------------------------------------------------*/
    route_scope->if_map[(iface_index / 32)] |= ( 1 << (iface_index % 32));
  }
} /* ps_route_scope_add() */

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_INTERSECT()

DESCRIPTION
  This function figures out if there are interfaces common to the two passed
  route_scope's and returns the intersection result.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type   - Route_scope 1.
  ps_route_scope_type   - Route_scope 2.

RETURN VALUE
  ps_route_scope_type - Intersection of Route scope 1 and 2

SIDE EFFECTS
  None
===========================================================================*/
INLINE ps_route_scope_type ps_route_scope_intersect
(
  ps_route_scope_type rscope1,
  ps_route_scope_type rscope2
)
{
  uint32              idx;
  ps_route_scope_type ret_rscope;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&ret_rscope, 0, sizeof(ps_route_scope_type));
  for (idx = 0; idx < PS_MAX_ROUTE_SCOPE_INDEX; idx++)
  {
    ret_rscope.if_map[idx] = rscope1.if_map[idx] & rscope2.if_map[idx];
  }

  return (ret_rscope);
} /* ps_route_scope_intersect() */

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_JOIN()

DESCRIPTION
  This functions finds the superset of the passed route scope's.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type - Route_scope 1.
  ps_route_scope_type - Route_scope 2.

RETURN VALUE
  Join of two route scope's

SIDE EFFECTS
  None
===========================================================================*/
INLINE ps_route_scope_type ps_route_scope_join
(
  ps_route_scope_type rscope1,
  ps_route_scope_type rscope2
)
{
  uint32              idx;
  ps_route_scope_type route_scope;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for (idx = 0; idx < PS_MAX_ROUTE_SCOPE_INDEX; idx++)
  {
    route_scope.if_map[idx] = rscope1.if_map[idx] | rscope2.if_map[idx];
  }

  return (route_scope);
} /* ps_route_scope_join() */

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_DELETE()

DESCRIPTION
  This function removes the specified iface index from the input route scope
  and returns the resultant route scope.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type *rscope - The original route scope.
  uint32  iface_index - Index of the Iface to remove from the route scope.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE void ps_route_scope_delete
(
  ps_route_scope_type *rscope,
  uint32              iface_index
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rscope != NULL)
  {
    rscope->if_map[(iface_index / 32)] &= ~((uint32) (1 << (iface_index % 32)));
  }

  return;
} /* ps_route_scope_delete() */

/*===========================================================================
FUNCTION PS_ARE_ROUTE_SCOPES_EQUAL()

DESCRIPTION
  This function determines whether the passed route scopes are equal or not.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type - Route_scope 1.
  ps_route_scope_type - Route_scope 2.

RETURN VALUE
  TRUE  if equal
  FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_are_route_scopes_equal
(
  ps_route_scope_type rscope1,
  ps_route_scope_type rscope2
)
{
  uint32 idx;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (idx = 0; idx < PS_MAX_ROUTE_SCOPE_INDEX; idx++)
  {
    if(rscope1.if_map[idx] != rscope2.if_map[idx])
    {
      return FALSE;
    }
  }

  return TRUE;
}/* ps_are_route_scopes_equal() */

/*===========================================================================
FUNCTION PS_IS_ROUTE_SCOPE_EMPTY()

DESCRIPTION
  This function determines whether the passed route scopes is empty (0) or not

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type - Route_scope.

RETURN VALUE
  TRUE  if equal
  FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_is_route_scope_empty
(
  ps_route_scope_type rscope
)
{
  uint32 idx;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (idx = 0; idx < PS_MAX_ROUTE_SCOPE_INDEX; idx++)
  {
    if(rscope.if_map[idx] != 0)
    {
      return FALSE;
    }
  }

  return TRUE;
}/* ps_is_route_scope_empty() */

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_ADD_LO()

DESCRIPTION
  This function adds the v4 or/and v6 interface to the route_scope.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type * - Target route_scope.
  boolean               - TRUE if called to set v6 lo interface.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
INLINE void ps_route_scope_add_lo
(
  ps_route_scope_type *route_scope,
  boolean              v6_type
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(route_scope != NULL)
  {
    ps_route_scope_add(route_scope, ps_get_v4_lo_rscope_idx());
#ifdef FEATURE_DATA_PS_IPV6
    if(v6_type == TRUE)
    {
      ps_route_scope_add(route_scope, ps_get_v6_lo_rscope_idx());
    }
#endif /*FEATURE_DATA_PS_IPV6*/
  }
} /* ps_route_scope_add_lo() */


/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_IS_IFACE_IDX_SET()

DESCRIPTION
  This function returns TRUE if the specified iface index is set in the
  route scope.

DEPENDENCIES
  None.

PARAMETERS
  ps_route_scope_type * route_scope - Route scope.
  uint32  iface_index - Index of the Iface to check in the Route Scope

RETURN VALUE
  TRUE  - The bit corresponding to the iface index is set in the Route
          Scope.
  FALSE - The bit corresponding to the iface index is not set in the
          Route Scope.

SIDE EFFECTS
  None
===========================================================================*/
INLINE boolean ps_route_scope_is_iface_idx_set
(
  ps_route_scope_type *route_scope,
  uint32               iface_index
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Check that iface index is not more than (PS_MAX_ROUTE_SCOPE_INDEX * 32)
  -------------------------------------------------------------------------*/
  if(iface_index >= PS_MAX_ROUTE_SCOPE_BITS || route_scope == NULL)
  {
    return FALSE;
  }

  if ((route_scope->if_map[(iface_index / 32)] &
                                            ( 1 << (iface_index % 32))) == 0)
  {
    return FALSE;
  }

  return TRUE;

} /* ps_route_scope_is_iface_idx_set */


/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_BY_IFACE_HANDLE()

DESCRIPTION
  This function allows to retrieve a route_scope from the interface_handle.

DEPENDENCIES
  None.

PARAMETERS
  ps_iface_type *       - Interface handle.
  ps_route_scope_type * - Target route scope.

RETURN VALUE
  TRUE  on success.
  FALSE on failure.

SIDE EFFECTS
  None
===========================================================================*/
boolean ps_route_scope_by_iface_handle
(
  ps_iface_type       *if_ptr,
  ps_route_scope_type *route_scope
);

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_BY_POLICY()

DESCRIPTION
  This function allows to retrieve a route_scope from the netpolicy.

DEPENDENCIES
  None.

PARAMETERS
  ip_pkt_ptr - Ptr to packet info
  acl_pi_ptr - Ptr to packet's policy information

RETURN VALUE
  0 on failure and the found route_scope on success

SIDE EFFECTS
  None
===========================================================================*/
ps_route_scope_type ps_route_scope_by_policy
(
  ip_pkt_info_type*        ip_pkt_ptr,
  acl_policy_info_type*    acl_pi_ptr
);

#ifdef __cplusplus
}
#endif

#endif    /* PS_ROUTEI_SCOPE_H */
