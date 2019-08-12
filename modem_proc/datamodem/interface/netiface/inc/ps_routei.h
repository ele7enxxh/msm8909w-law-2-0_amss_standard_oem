#ifndef ROUTEI_H
#define ROUTEI_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

              I N T E R N A L   R O U T I N G   T A B L E

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  This file defines the structure and management functions for
  the kernel's routing table. The table is a fixed size array
  for lookup efficiency.

  Functions and data structures are specific to the IP version
  and named accordingly.

EXTERNALIZED FUNCTIONS
  routei_alloc_v4()       - Allocate an entry in the IPv4 routing table
  routei_free_v4()        - Free an entry in the IPv4 routing table
  routei_check_v4_table() - Check all IPv4 entries for consistency
  routei_get_v4_route()   - Do an IPv4 address-based routing lookup
  routei_acl_list()       - Accessor function for the registered-ACL list

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/inc/ps_routei.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/04/13    svj    New DIAG event to generate data cal status information   
12/14/08    pp     Common Modem Interface: Public/Private API split.
06/11/04    vp     Changed IP addresses in routei_v4_route_entry_type to
                   struct ps_in_addr.
03/16/04    mct    Renamed acl_ipfilter_info_type to ip_pkt_info_type.
01/30/04    usb    Added common ACL post processing function, added support
                   for multiple ACL lists
02/20/03    om     More comments.
02/12/03    om     Added prototype for routei_check_v4_table().
08/30/02    om     Updated signature of routei_free_v4().
03/08/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"

#include "ps_route.h"
#include "ps_socket_defs.h"


/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/
typedef enum
{
  ROUTE_DATA_PATH_CLIENT_MIN       = 0,
  ROUTE_DATA_PATH_CLIENT_NORMAL    = ROUTE_DATA_PATH_CLIENT_MIN,
  ROUTE_DATA_PATH_CLIENT_ROUTEABLE = 1,
  ROUTE_DATA_PATH_CLIENT_SYSTEM    = 2,
  ROUTE_DATA_PATH_CLIENT_MAX
} route_data_path_client_enum_type;

/* Static size of the routing table */
#define ROUTEI_V4_ROUTING_TABLE_SIZE    10

/* Macro to compare route costs, prefers cost over metric */
#define ROUTEI_V4_ROUTE_IS_CHEAPER( _index, _metric, _cost_ptr )             \
          (routei_v4_routing_table.entry[_index].flags.cost_is_valid ?       \
            (_cost_ptr ?                                                     \
               (*_cost_ptr < routei_v4_routing_table.entry[_index].cost)     \
               : (_metric < routei_v4_routing_table.entry[_index].metric))   \
            : (_metric < routei_v4_routing_table.entry[_index].metric) )


/* Structure for a routing entry */
typedef struct
{
  struct ps_in_addr     dst_addr;     /* Destinations address / network id */
  uint32                dst_netmask;  /* Destination network mask          */
  struct ps_in_addr     gateway;      /* Gateway address                   */
  ps_iface_type*        if_ptr;       /* Pointer to local interface        */

  struct
  {
    unsigned int gateway:1;           /* Set if entry is an indirect route */
    unsigned int host:1;              /* Set if entry is a host route      */
    unsigned int icmp_created:1;      /* Set if entry was created by ICMP  */
    unsigned int icmp_modified:1;     /* Set if entry was modified by ICMP */

    unsigned int metric_is_valid:1;   /* Used flags for the fields quali-  */
    unsigned int cost_is_valid:1;     /* fying the attributes of a route   */
  } flags;

  uint8                 metric;       /* RIP metric (hop count)            */
  uint16                cost;         /* Dimensionless cost of route       */

  uint32                pkt_count;    /* Number of packets sent via route  */
} routei_v4_route_entry_type;

#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
/*============================================================================
    Structure for a routing result
      -  link:      for linking ACL results
      -  acl_ptr:   routing ACL result
      -  priority:  priority of the routing ACL result
=============================================================================*/
typedef struct ps_routei_rt_result_s ps_routei_rt_result_type;
struct ps_routei_rt_result_s
{
  list_link_type                 link;
  acl_type                      *acl_ptr;
  int32                          priority;
};

#define ROUTEI_ACL_RT_ITEM_NUM    5
typedef struct
{
  list_type                      acl_rt_item_list;
  ps_routei_rt_result_type       acl_rt_item_mem[ROUTEI_ACL_RT_ITEM_NUM];
  uint32                         acl_rt_item_mem_index;
} ps_routei_acl_match_list_type;
#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY */

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* Definition of the actual table - the default_start_index allows */
/* a lookup for a DEFAULT destination to skip host and network     */
/* routes for efficiency (most lookups just need a default route). */
typedef struct
{
  routei_v4_route_entry_type  entry[ROUTEI_V4_ROUTING_TABLE_SIZE];
  uint32                      table_size_used;
  uint32                      default_start_index;
} routei_v4_routing_table_type;

extern routei_v4_routing_table_type routei_v4_routing_table;


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION   ROUTEI_ALLOC_V4()

DESCRIPTION
  This functions returns the index to an entry in the routing
  table to be used for a new route.
  Route entries are prioritized as HOST -> NETWORK -> DEFAULT.

DEPENDENCIES
  None

PARAMETERS
  rtype  - Specifies the type of entry to allocate

RETURN VALUE
  Index of the new entry

SIDE EFFECTS
  None
===========================================================================*/
int routei_alloc_v4
(
  route_type_enum_type  rt_type,
  uint32                destination,
  uint8                 metric,
  uint16*               cost_ptr
);

/*===========================================================================

FUNCTION   ROUTEI_FREE_V4()

DESCRIPTION
  This functions frees up a used entry in the routing table.

DEPENDENCIES
  None

PARAMETERS
  rt_index  - Index of entry to remove

RETURN VALUE
  Number of remaining entries for success or -1 if index is not valid

SIDE EFFECTS
  None
===========================================================================*/
int routei_free_v4
(
  uint32  rt_index
);

/*===========================================================================

FUNCTION   ROUTEI_CHECK_V4_TABLE()

DESCRIPTION
  This functions does a consistency check on the v4 routing table.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  0 for success, otherwise the entry # to the offending line

SIDE EFFECTS
  None
===========================================================================*/
int routei_check_v4_table( void );

/*===========================================================================

FUNCTION   ROUTEI_GET_V4_ROUTE()

DESCRIPTION
  This functions returns the index to an entry in the routing
  table for a given destination.
  The result is purely based on IP address routing.

DEPENDENCIES
  None

PARAMETERS
  destination  - Destination address

RETURN VALUE
  Index of the routing entry

SIDE EFFECTS
  None
===========================================================================*/
int routei_get_v4_route
(
  uint32   destination,
  uint32*  gateway
);

/*===========================================================================
FUNCTION PS_ROUTE_DATA_PATH_LOOKUP()

DESCRIPTION
  This function performs data path interface lookups based on the input
  policy. All interface ACLs for interfaces which are UP, CONFIGURING, or
  GOING_DOWN are compared and the best match is returned.

DEPENDENCIES
  None

PARAMETERS
  ip_pkt_ptr              - Ptr to packet's IP information
  acl_pi_ptr              - Ptr to packet's policy information
  client_type             - Type of client which is calling this function.
  addr_based_routing_only - Don't use ACLs for routing decision if true
  if_ptr                  - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (_errno contains error code).
SIDE EFFECTS
  None
===========================================================================*/
int32 ps_route_data_path_lookup
(
  ip_pkt_info_type*                 ip_pkt_ptr,
  acl_policy_info_type*             acl_pi_ptr,
  boolean                           addr_based_routing_only,
  ps_iface_type**                   if_ptr
);


/*===========================================================================
FUNCTION   ROUTEI_ACL_IS_BETTER_MATCH ()

DESCRIPTION
  This function compares 2 ACL results and returns TRUE if the second ACL is
  is better than the first.

DEPENDENCIES
  None

PARAMETERS
  curr_res_priority - The first ACL result priority
  curr_acl_ptr      - The first ACL result ptr
  prev_res_priority - The second ACL result priority
  prev_acl_ptr      - The second ACL result ptr
  
RETURN VALUE
  boolean value

SIDE EFFECTS
  None
===========================================================================*/
boolean routei_acl_is_better_match
( 
  int            curr_res_priority,
  acl_type      *curr_acl_ptr,
  int            prev_res_priority,
  acl_type      *prev_acl_ptr
);


#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
/*===========================================================================
FUNCTION   ROUTEI_ACL_RES_LIST_RESET ()

DESCRIPTION
  This function alloc memory for a acl match list.

DEPENDENCIES
  should protected for multi thread access.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void routei_acl_res_list_reset
(
  ps_routei_acl_match_list_type *acl_match_list_ptr
);


/*===========================================================================
FUNCTION   ROUTEI_ACL_UPDATE_LOCAL_SRCH_RESULT()

DESCRIPTION
  This function is a utility function that collects the local ACL search
  results to various lists for the outbound routing ACLs.

DEPENDENCIES
  None

PARAMETERS
  res_priority - priority
  this_if_ptr - Iface ptr
  policy_info_ptr - Routing policy Info
  pkt_info_ptr - IP packet info

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void routei_acl_update_local_srch_results
(
  int                            priority,
  acl_type*                      acl_ptr,
  boolean                        is_default_list,
  ps_routei_acl_match_list_type* acl_match_list_ptr
);


/*===========================================================================
FUNCTION   ROUTEI_ACL_GET_BEST_MATCH ()

DESCRIPTION
  This function gets the best-match iface.

DEPENDENCIES
  None

PARAMETERS
  this_if_ptr - Iface ptr
  policy_info_ptr - Routing policy Info
  pkt_info_ptr - IP packet info

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void routei_acl_get_best_match
(
  ps_routei_acl_match_list_type* acl_match_list_ptr,
  acl_policy_info_type*          policy_info_ptr,
  int*                           priority,
  acl_type**                     acl_ptr  
);
#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY  */  

/*===========================================================================
FUNCTION   ROUTEI_GET_APP_ID_FROM_PROC_ID ()

DESCRIPTION
  This function gets the app_id from given proc_id

DEPENDENCIES
  None

PARAMETERS
  app_id  - Application identifier
  proc_id - Process identifier

RETURN VALUE
  TRUE  - SUCCESS
  FALSE - FAILURE

SIDE EFFECTS
  None
===========================================================================*/
boolean routei_get_app_id_from_proc_id(
  uint64                       *app_id,
  acl_proc_id_type              proc_id
); 

#ifdef __cplusplus
}
#endif

#endif    /* ROUTEI_H */

