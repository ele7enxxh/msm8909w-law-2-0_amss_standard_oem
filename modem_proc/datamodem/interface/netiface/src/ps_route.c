/*===========================================================================

                             P S _ R O U T E . C

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

Copyright (c) 2002-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_route.c_v   1.14   24 Feb 2003 10:56:00   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_route.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
02/04/13    svj    New DIAG event to generate data cal status information   
10/30/10    asn    Return physical Ifaces for DUN calls
01/11/10    as     Added SU level header ps_route_iface.h
07/09/10    asn    Support for Arbitration
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
02/04/09    ar     Adjust crit sect usage for blocking mode handler calls.
01/21/09    ar     Move routing callback paramater list into structure
12/15/08    am     Added bound check before incr deny_list_ptr.
06/20/08    ar     Refactor policy logic for mode handler routing callbacks.
05/14/08    am     Added get_priority() call in bring_up_iface_by_policy().
04/29/08    am     Updated Arbitration/Policy Manager now supports OMH ph-2.
05/07/07    mct    IPv6 Neighbor Discovery
12/15/06    mct    ACL input function should accept IPv6 packets as well.
11/22/06    sv     Added support for DSS_AF_UNSPEC.
11/02/06    mct    Restructured routing code and added routing optimizations.
11/02/06    mct    Added support for RFC3041: IPv6 Privacy Extensions.
08/24/06    rt     Modified ps_route_scope_by_policy() to include Configuring
                   interfaces during acl checks.
07/31/06    rt     Route Scope modifications.
04/20/06    mct    Loopback routing optimizations.
03/29/06    ks     Typecasting void iface_ptr in ip_pkt_info_type to
                   ps_iface_type * before passing it to the macro.
02/22/06    msr    Using single critical section
02/07/06    msr    Updated with L4/tasklock changes.
10/12/05    mct    Fixed support for sending to v4/v4-map-v6 addrs over a
                   v6 socket.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
05/12/05    rt     Fixed Lint errors.
04/19/05    vp     IPv6 featurization in ps_route_scope_by_policy().
04/18/05    vp     Changes for enforcing netpolicy on input.
03/14/04    vp     Removed invalid ASSERT on non matching address families
                   between ip_pkt_ptr and acl policy in route_get()
10/13/04    vp     Removal of byte order conversion as addresses are stored
                   in network order.
09/23/04    mct    Added check for NULL ptr and added more V6 featurization.
08/20/04    mct    Fixed v6 featurization for sockets passing v4 mapped v6
                   addrs.  Cleaned up some messaging.
07/29/04    mct    Now doing v4/v6 address check and converting V4 mapped V6
                   addr to V4 in routing layer. Added family checking for
                   v4/v6/ANY.
06/11/04    vp     Removed inclusion of psglobal.h. Included ps_in.h.
                   Changes for representation of IP addresses as struct ps_in_addr.
05/10/04    mct    Fixed lint errors.
03/15/04    mct    Updated routing code, and added new checks in route_get().
                   Changed acl_ipfilter_info_type to ip_pkt_info_type.
03/01/04    mct    Added checks in route_get to verify v4/v6 policy and ip
                   pkt configurations
02/17/04    sv     Changed assert in route_get to check for either v4 or v6.
02/03/04    usb    Modified route_acl_control() prototype/definition.
01/30/04    usb    Added support for routing in "lookup_only" mode,
                   appropriately TASKLOCKed rouiting functions, added support
                   for multiple ACL lists.
07/18/03    usb    Always invalidate cache if iface is either DOWN or
                   DISABLED, don't use force_route.
06/25/03    ss     Changed a LOG_MSG_INFO2 to LOG_MSG_INFO3
02/21/03    usb    Fixed ACL deny list parsing.
02/20/03    om     More comments, added FEATURE_DATA_PS, fixed ACL deny list.
02/12/03    om     Added MSGs and consistency check to route_add,
                   fixed INTLOCKs.
11/25/02    usb    Init best_priority (in route_get()) to ACL_DEFAULT_CLASS
                   instead of 0.
11/19/02    om     Added ACL_DENY list to prevent table lookups from
                   overriding ACL results.
11/05/02    om     Changed ACL en-disable into acl_control, allow iface Id.
10/15/02    om     Added ACL en-/disabling functions.
09/25/02    om     Updates for forced routing.
09/24/02    om     Added pointer check to post-processor call.
09/18/02    om     Fixed call to ACL postprocessor.
09/09/02    om     Updates for ACL postprocessors.
08/30/02    om     Updated signature of routei_delete_v4().
08/13/02    mvl    updated ps_iface state macros for name change
03/08/02    om     Created Module
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "target.h"
#include "customer.h"


#include "msg.h"
#include "err.h"
#include "amssassert.h"
#include "IxErrno.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_route.h"
#include "ps_iface.h"
#include "ps_routei.h"
#include "ps_route_iface.h"
#include "ps_routei_lo.h"
#include "ps_route_scope.h"
#include "ps_routei_acl.h"
#include "ps_in.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ps_policy_mgr.h"
#include "ps_policyi_mgr.h"
#include "ps_iface_defs.h"
#include "ps_acli_trace.h"
#include "ps_data_call_info_logging.h"
#include "event_defs.h"
#include "ps_sys.h"

#include "ps_arbitration_mgr.h"

#ifdef FEATURE_DATA_PS_464XLAT
#include "ps_clat.h"
#endif

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#include "ps_ifacei_addr_v6.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ps_ifacei_utils.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_profile.h"
#include "ds_profile_3gpp.h"
#include "ds_profile_3gpp2.h"
#include "ds3gmgr.h"
#include "sys.h"

#include <stringl/stringl.h>

/*===========================================================================

                        DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Set when inbound ACL's are executed so that ACL trace messages are not
  printed
---------------------------------------------------------------------------*/
boolean  is_inbound_acl = FALSE;

/* Scratch space used by route_get to keep track of */
/* disabled and ACL_DENY'd interfaces               */
ps_iface_type*   _route_deny_list[MAX_SYSTEM_IFACES];

/* Loopback routing tables */
ps_lo_v4_route_type global_v4_lo_route_array[MAX_IPV4_LO_IFACES];
#ifdef FEATURE_DATA_PS_IPV6
ps_lo_v6_route_type global_v6_lo_route_array[MAX_IPV6_LO_IFACES];
#endif /* FEATURE_DATA_PS_IPV6 */

/* Forward prototype */
static int ps_route_local_acl_policy_search
(
  ps_route_lookup_params_type * params_ptr,
  acl_type**                    acl_match_ptr,
  acl_rt_result_type*           result_ptr,
  int*                          deny_reason
);

/* Routing ACL priority search callback table.
 * The local processor's callback MUST be last in list. */
#ifdef FEATURE_DATA_QMH
#define ACL_PROC_MAX (3)
route_acl_priority_cb_func _acl_priority_cb_tbl[ACL_PROC_MAX] =
{
  NULL,
  NULL,
  ps_route_local_acl_policy_search
};
#else
#define ACL_PROC_MAX (1)
route_acl_priority_cb_func _acl_priority_cb_tbl[ACL_PROC_MAX] =
{
  ps_route_local_acl_policy_search
};
#endif /* FEATURE_DATA_QMH */



/*===========================================================================

                      FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION   PS_ROUTEI_ACL_POLICY_TRACE_DUMP()

DESCRIPTION
  This function just prints acl policy parameters

DEPENDENCIES
  None

PARAMETERS
  acl_pi_ptr      - ACL Policy Info Pointer

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE void ps_routei_acl_policy_trace_dump
(
  acl_policy_info_type*    acl_pi_ptr,
  const char*              caller_name
)
{
  ds_profile_hndl_type       profile_hndl = NULL;
  ds_profile_info_type       param_info;
  ds_profile_status_etype    profile_status;
  byte                       apn_string[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ACL_TRACE_STR_DUMP_6(ACL_LEVEL_MED,
                      "%s : Policy_Flag %d, Iface 0x%x:%x, "
                      "IP_family %d, proc_id %d",
                      caller_name,
                      acl_pi_ptr->policy_flag,
                      acl_pi_ptr->iface.kind,
                      acl_pi_ptr->iface.info.id,
                      acl_pi_ptr->ip_family,
                      acl_pi_ptr->proc_id );


  ACL_TRACE_DUMP_6(ACL_LEVEL_MED,
                   "Is routeable %d IPSec_disabled %d, Lookup_only %d, Bring_Up %d, "
                   "App Identifier %llx,IM_CN flag %d ",
                   acl_pi_ptr->is_routeable,
                   acl_pi_ptr->ipsec_disabled,
                   acl_pi_ptr->lookup_only,
                   acl_pi_ptr->bring_up,
                   acl_pi_ptr->app_identifier,
                   acl_pi_ptr->im_cn_flag);

  ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   " 3GPP Profile ID: %d 3GPP2 profile ID: %d, "
                   " Proc id %d",
                   (uint32)acl_pi_ptr->pdp_info,
                   acl_pi_ptr->data_session_profile_id,
                   acl_pi_ptr->proc_id);

  if (acl_pi_ptr->apn.length > 0)
  {
    ACL_TRACE_STR_DUMP(ACL_LEVEL_MED,
                       "APN len %d, APN %s",
                        acl_pi_ptr->apn.length, acl_pi_ptr->apn.name, 0);
  }

  profile_status =  ds_profile_begin_transaction(
                      DS_PROFILE_TRN_READ,
                      DS_PROFILE_TECH_3GPP,
                      (ds_profile_num_type)((uint32)acl_pi_ptr->pdp_info),
                      &profile_hndl);
 
  if((DS_PROFILE_REG_RESULT_SUCCESS == profile_status) && (NULL != profile_hndl))
  {
    memset(&param_info, 0, sizeof(param_info));
    memset(&apn_string[0], 0, sizeof(apn_string));
    
    param_info.buf = apn_string;
    param_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
  
    if(DS_PROFILE_REG_RESULT_SUCCESS == ds_profile_get_param(profile_hndl, 
                                          DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN, 
                                          &param_info))
    {
      ACL_TRACE_STR_DUMP(ACL_LEVEL_MED, "3GPP APN Len %d, 3GPP APN %s",
                         param_info.len, apn_string,0);
    }
    
    (void)ds_profile_end_transaction(profile_hndl,DS_PROFILE_ACTION_COMMIT);
  }

  profile_status =  ds_profile_begin_transaction(
                      DS_PROFILE_TRN_READ,
                      DS_PROFILE_TECH_3GPP2,
                      (ds_profile_num_type)acl_pi_ptr->data_session_profile_id,
                      &profile_hndl);
  
  if((DS_PROFILE_REG_RESULT_SUCCESS == profile_status) && (NULL != profile_hndl))
  {
    memset(&param_info, 0, sizeof(param_info));
    memset(&apn_string[0], 0, sizeof(apn_string));
    
    param_info.buf = apn_string;
    param_info.len = DS_PROFILE_3GPP2_APN_MAX_VAL_LEN+1;
  
    if(DS_PROFILE_REG_RESULT_SUCCESS == ds_profile_get_param(profile_hndl, 
                                          DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING, 
                                          &param_info))
    {
      ACL_TRACE_STR_DUMP(ACL_LEVEL_MED, " 3GPP2 APN Len %d, 3GPP2 APN %s",
                         param_info.len, apn_string,0);
    }
    
     (void)ds_profile_end_transaction(profile_hndl,DS_PROFILE_ACTION_COMMIT);
  }

#if defined (FEATURE_DS_MOBILE_IP) && defined (FEATURE_DATA_PS_MIP_CCOA)
  ACL_TRACE_DUMP(ACL_LEVEL_MED,
                 "sip_iface_reqd %d",
                 acl_pi_ptr->sip_iface_reqd, 0, 0);
#endif
}

int validate_ip_pkt_info
(
  ip_pkt_info_type*        ip_pkt_ptr,
  acl_policy_info_type*    acl_pi_ptr,
  ps_iface_type**          if_ptr
)
{
  uint32                      temp_addr;
  byte                        temp_protocol;

#ifdef FEATURE_DATA_PS_IPV6
  ps_route_scope_type         pol_rscope;
  ps_route_scope_type         if_rscope;
  ps_ip_addr_type             ip_addr_ptr;
#endif

 /*------------------------------------------------------------------------
    If address is a V6 address it could be a V4 MAPPED V6 addr. If it is
    convert it to it's native V4 format.
  ------------------------------------------------------------------------*/
  if( ip_pkt_ptr && (ip_pkt_ptr->ip_vsn == IP_V6) )
  {
    if(PS_IN6_IS_ADDR_V4MAPPED(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                               base_hdr.dst_addr))
    {
      temp_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(
                  &ip_pkt_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr);
      temp_protocol = ip_pkt_ptr->ip_hdr.v6.hdr_body.base_hdr.next_hdr;

      ip_pkt_ptr->ip_vsn = IP_V4;
      ip_pkt_ptr->ip_hdr.v4.protocol = temp_protocol;
      ip_pkt_ptr->ip_hdr.v4.dest.ps_s_addr = temp_addr;
    }
#ifdef FEATURE_DATA_PS_IPV6
    else
    {
      /*---------------------------------------------------------------------
        Check if the IPV6 scope of destination and source address match if
        either of them is not in6_addrany.
      ---------------------------------------------------------------------*/
      if((PS_IN6_IS_ADDR_UNSPECIFIED(
                 &ip_pkt_ptr->ip_hdr.v6.base_v6_hdr.src_addr)
          ||
          PS_IN6_IS_ADDR_UNSPECIFIED(
                 &ip_pkt_ptr->ip_hdr.v6.base_v6_hdr.dst_addr)
         ) == FALSE )
      {
        if(PS_IN6_IS_ADDR_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                    base_hdr.dst_addr) ||
           PS_IN6_IS_ADDR_MC_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                       base_hdr.dst_addr))
        {
          /*-----------------------------------------------------------------
            Linklocal dest address. Dont allow if src is not link local
          -----------------------------------------------------------------*/
          if(!PS_IN6_IS_ADDR_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                       base_hdr.src_addr))
          {
            LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                            "SRC and DST addrs IPv6 scopes dont match");
            return -1;
          }
        }
        else if(PS_IN6_IS_ADDR_SITELOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                         base_hdr.dst_addr) ||
                PS_IN6_IS_ADDR_MC_SITELOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                            base_hdr.dst_addr))
        {
          /*-----------------------------------------------------------------
            Sitelocal dest address. Dont allow if src is not site local
          -----------------------------------------------------------------*/
          if(!PS_IN6_IS_ADDR_SITELOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                       base_hdr.src_addr))
          {
            LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                            "SRC and DST addrs IPv6 scopes dont match");
            return -1;
          }
        }
        else
        {
          /*-----------------------------------------------------------------
            Global destination address. Dont allow if src is link/site local
          -----------------------------------------------------------------*/
          if(PS_IN6_IS_ADDR_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                      base_hdr.src_addr) ||
             PS_IN6_IS_ADDR_SITELOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                      base_hdr.src_addr))
          {
            LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                            "SRC and DST addrs IPv6 scopes dont match");
            return -1;
          }
        }
      }

      if(PS_IN6_IS_ADDR_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                  base_hdr.dst_addr) ||
         PS_IN6_IS_ADDR_MC_LINKLOCAL(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                                     base_hdr.dst_addr))
      {
        /*-------------------------------------------------------------------
          If the destination address is linklocal then the application must
          provide the interface via scopeid field in ps_sockaddr_in6 structure.
          That interface is passed to route_get() via ip_pkt_ptr->if_ptr
        -------------------------------------------------------------------*/
        if(ip_pkt_ptr->if_ptr == NULL ||
           PS_IFACE_IS_VALID((ps_iface_type *)ip_pkt_ptr->if_ptr) == FALSE)
        {
          LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                          "No scopeid for linklocal IPv6 address, routing not "
                          "allowed");
          return -1;
        }

        ps_iface_get_linklocal_addr(ip_pkt_ptr->if_ptr, &ip_addr_ptr);
        if(ip_addr_ptr.type == IP_ADDR_INVALID)
        {
          LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                          "Route lookup failed, invalid if_ptr");
          return -1;
        }

        /*-------------------------------------------------------------------
          Determine if the interface belongs to the route_scope specified by
          the netpolicy.
        -------------------------------------------------------------------*/
        if (acl_pi_ptr != NULL)
        {
          memset(&pol_rscope , 0, sizeof(ps_route_scope_type));
          memset(&if_rscope , 0, sizeof(ps_route_scope_type));

          if (ps_route_scope_by_iface_handle(ip_pkt_ptr->if_ptr, &if_rscope) ==
                FALSE)
          {
            LOG_MSG_INFO1_0("Route lookup failed, invalid if_ptr");
            return -1;
          }

          acl_pi_ptr->proc_id = PS_IFACE_PROC_ID_ANY;

          pol_rscope = ps_route_scope_by_policy(ip_pkt_ptr, acl_pi_ptr);

          if_rscope = ps_route_scope_intersect(if_rscope, pol_rscope);

          if (ps_is_route_scope_empty(if_rscope))
          {
            LOG_MSG_INFO1_0("Route lookup failed, scopeid violates netpolicy");
            return -1;
          }
        }
        else
        {
          LOG_MSG_INFO1_0("validate_ip_pkt_info(): "
                          "Skipped route scope check as policy is NULL");
        }

        /*-------------------------------------------------------------------
          Interface does belong to the route scope hence valid. Return it.
        -------------------------------------------------------------------*/
        *if_ptr = ip_pkt_ptr->if_ptr;
        return 0;
      }/* end if dest ipv6 addr is linklocal */
    }/* end else */
#endif /* FEATURE_DATA_PS_IPV6 */
  } /* end if ip_vsn == IP_V6 */

  /*-------------------------------------------------------------------------
    Verify policy and ip_pkt families are equal, policy is set to ANY, and
    either policy or ip pkt info are non NULL.
  -------------------------------------------------------------------------*/
  if( acl_pi_ptr &&
      (acl_pi_ptr->ip_family != IFACE_ANY_ADDR_FAMILY) &&
      (acl_pi_ptr->ip_family != IFACE_UNSPEC_ADDR_FAMILY) &&
      ip_pkt_ptr &&
      (acl_pi_ptr->ip_family != ((ip_addr_enum_type)ip_pkt_ptr->ip_vsn)) )
  {
    LOG_MSG_INFO1_2("validate_ip_pkt_info(): "
                  "Route lookup failed family in policy: %d"
                  "ip type in packet info: %d"
                  ,acl_pi_ptr->ip_family
                  ,((ip_addr_enum_type)ip_pkt_ptr->ip_vsn));
    return -1;
  }

  return 0;
}


/*===========================================================================
FUNCTION   ROUTE_ADD()

DESCRIPTION
  This function allows to add a route to the DMSS routing
  table.

DEPENDENCIES
  None

PARAMETERS
  type        - Specifies whether it is a host or network route
  destination - IP address of the routing destination in network order
  netmask     - IP network mask the destination address belongs to
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to
  metric      - Specifies the hop metric of this route
  cost_ptr    - Pointer to route cost (optional, set to zero for n/a)

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

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
)
{
  int32 index;
  uint32 idx;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    Only IPv4 is supported for now
  -------------------------------------------------------------------------*/
  ASSERT( destination.type == IPV4_ADDR );
  ASSERT( netmask.type == IPV4_ADDR );
  ASSERT( gateway.type == IPV4_ADDR );

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Check if an entry for this route already exists
  -------------------------------------------------------------------------*/
  for ( idx=0; idx<routei_v4_routing_table.table_size_used; idx++ )
  {
    if ( (routei_v4_routing_table.entry[idx].dst_addr.ps_s_addr
          ==
          destination.addr.v4.ps_s_addr)
         &&
         (routei_v4_routing_table.entry[idx].gateway.ps_s_addr
          ==
          gateway.addr.v4.ps_s_addr) )
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }
  }

  /*-------------------------------------------------------------------------
    Allocate a new entry for this route ...
  -------------------------------------------------------------------------*/
  if ( (index = routei_alloc_v4( type,
                                 destination.addr.v4.ps_s_addr,
                                 metric,
                                 cost_ptr ))
       == -1 )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_ERROR_0("route_add(): "
                    "RT: Table is full"  );
    return -1;
  }

  /*-------------------------------------------------------------------------
    ...and fill in the routing info
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO1_3("route_add(): "
                  "RT: Adding 0x%x/0x%x gw 0x%x",
                  ps_ntohl(destination.addr.v4.ps_s_addr),
                  ps_ntohl(netmask.addr.v4.ps_s_addr),
                  ps_ntohl( gateway.addr.v4.ps_s_addr ) );
  switch( if_ptr->name )
  {
    case CDMA_SN_IFACE:
    LOG_MSG_INFO1_3("route_add(): "
                    "via 1x/SN if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
      break;
    case CDMA_AN_IFACE:
    LOG_MSG_INFO1_3("route_add(): "
                    "via 1x/AN if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
      break;
    case UMTS_IFACE:
    LOG_MSG_INFO1_3("route_add(): "
                    "via UMTS if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
      break;
    case SIO_IFACE:
    LOG_MSG_INFO1_3("route_add(): "
                    "via SIO if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
      break;
    case LO_IFACE:
    LOG_MSG_INFO1_3("route_add(): "
                    "via LO if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
      break;
    default:
    LOG_MSG_INFO1_3("route_add(): "
                    "via if 0x%x:%d (metric %d)",
                    if_ptr->name, if_ptr->instance, metric );
  } /* switch(interface name) */

  routei_v4_routing_table.entry[index].dst_addr.ps_s_addr =
    destination.addr.v4.ps_s_addr;
  routei_v4_routing_table.entry[index].dst_netmask = netmask.addr.v4.ps_s_addr;
  routei_v4_routing_table.entry[index].gateway.ps_s_addr =
    gateway.addr.v4.ps_s_addr;
  routei_v4_routing_table.entry[index].if_ptr = if_ptr;

  /*-------------------------------------------------------------------------
    Set the flags for this route according to it's type
  -------------------------------------------------------------------------*/
  switch( type )
  {
    case ROUTE_DEFAULT:
      routei_v4_routing_table.entry[index].flags.gateway = TRUE;
      routei_v4_routing_table.entry[index].flags.host = FALSE;
      break;

    case ROUTE_HOST:
      routei_v4_routing_table.entry[index].flags.gateway =
        ! ( (destination.addr.v4.ps_s_addr & netmask.addr.v4.ps_s_addr)
            ==
            (PS_IFACE_IP_V4_ADDR( if_ptr ) & netmask.addr.v4.ps_s_addr) );
      routei_v4_routing_table.entry[index].flags.host = TRUE;
      break;

    case ROUTE_NET:
      routei_v4_routing_table.entry[index].flags.gateway =
        ! ( (destination.addr.v4.ps_s_addr & netmask.addr.v4.ps_s_addr)
            ==
            (PS_IFACE_IP_V4_ADDR( if_ptr ) & netmask.addr.v4.ps_s_addr) );
      routei_v4_routing_table.entry[index].flags.host = FALSE;
      break;

    default:
      LOG_MSG_ERROR_1("route_add(): "
                      "RT: Unknown route type %d", type );
  } /* switch(route type) */

  routei_v4_routing_table.entry[index].flags.icmp_created = FALSE;
  routei_v4_routing_table.entry[index].flags.icmp_modified = FALSE;

  routei_v4_routing_table.entry[index].metric = metric;
  routei_v4_routing_table.entry[index].flags.metric_is_valid = TRUE;

  /*-------------------------------------------------------------------------
    Cost is an optional dimensionless parameter
  -------------------------------------------------------------------------*/
  if ( cost_ptr )
  {
    routei_v4_routing_table.entry[index].cost = *cost_ptr;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = TRUE;
  }
  else
  {
    routei_v4_routing_table.entry[index].cost = 0;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = FALSE;
  }

  /*-------------------------------------------------------------------------
    Sanity check the table
  -------------------------------------------------------------------------*/
  if ( routei_check_v4_table() != 0 )
  {
    LOG_MSG_ERROR_1("route_add(): "
                    "Rt: Deleting bad v4 entry %d", index );
    (void)routei_free_v4( index );
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;

} /* route_add() */


/*===========================================================================
FUNCTION ROUTE_DELETE()

DESCRIPTION
  This function allows to remove a route from the DMSS
  routing table.

DEPENDENCIES
  None

PARAMETERS
  description - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_delete
(
  ps_ip_addr_type   destination,
  ps_ip_addr_type*  gateway
)
{
  int ret_val = -1;
  int index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    For now we only support IPv4
  -------------------------------------------------------------------------*/
  ASSERT( destination.type == IPV4_ADDR );

  /*-------------------------------------------------------------------------
    Ckeck if a specific route to this destination was given
  -------------------------------------------------------------------------*/
  if ( gateway == NULL )
  {
    LOG_MSG_INFO1_1("route_delete(): "
                    "Flushing all routes for dst 0x%x",
                    destination.addr.v4.ps_s_addr);
  }
  else
  {
    ASSERT( gateway->type == IPV4_ADDR );
  }

  /*-------------------------------------------------------------------------
    Find the entry for this route in the table and delete it. These could be
    multiple entries with different gateways.
  -------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  for ( ;; )
  {
    if ( (index = routei_get_v4_route( destination.addr.v4.ps_s_addr,
                                       (gateway ? &(gateway->addr.v4.ps_s_addr)
                                                : NULL) ))
         == -1 )
    {
      break;
    }
    else
    {
      ret_val = 0;
    }
    (void)routei_free_v4( index );
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return ret_val;

} /* route_delete() */


/*===========================================================================
FUNCTION ROUTE_UPDATE()

DESCRIPTION
  This function allows to update an existing entry in the
  DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  destination - IP address of the routing destination in network order
  gateway     - IP address of the routing gateway in network order
  if_ptr      - Specifies a local interface to route to (optional)
  metric      - Specifies the hop metric of this route
  cost_ptr    - Specifies the cost of this route (set to NULL for n/a)

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

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
)
{
  int index;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Make sure this route exists */
  if ( (index = routei_get_v4_route( destination.addr.v4.ps_s_addr,
                                     &(gateway.addr.v4.ps_s_addr) ))
       == -1 )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /* Update the entrie's TX interface if it is given */
  if ( if_ptr )
  {
    routei_v4_routing_table.entry[index].if_ptr = if_ptr;
  }

  /* Update the entrie's metric if it is given */
  if ( metric_ptr )
  {
    routei_v4_routing_table.entry[index].metric = *metric_ptr;
    routei_v4_routing_table.entry[index].flags.metric_is_valid = TRUE;
  }

  /* Update the entrie's cost if it is given */
  if ( cost_ptr )
  {
    routei_v4_routing_table.entry[index].cost = *cost_ptr;
    routei_v4_routing_table.entry[index].flags.cost_is_valid = TRUE;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;

} /* route_update() */

/**
  Converts acl deny reason to ps net down reason

  @param[in]  deny reason
  
  @return
  none

  @dependencies
  None.
  
  @sideeffects
  None.
*/
ps_iface_net_down_reason_type ps_route_acl_deny_to_net_down_reason
(
  int deny_reason
)
{
  switch (deny_reason)
  {
    case ACL_DENY_REASON_MANDATORY_APN_DISABLED:
      return PS_NET_DOWN_REASON_MANDATORY_APN_DISABLED;
    case ACL_DENY_REASON_APN_DISABLED:
      return PS_NET_DOWN_REASON_INTERNAL_APN_DISABLED;
    case ACL_DENY_REASON_V6_DISABLED:
      return PS_NET_DOWN_REASON_IPV6_DISABLED;
    case ACL_DENY_REASON_V4_DISALLOWED_SUBS_LIMITED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED;
    case ACL_DENY_REASON_V6_DISALLOWED_SUBS_LIMITED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED;
    case ACL_DENY_REASON_MAX_V4_CONNECTIONS:
      return PS_NET_DOWN_REASON_MAX_V4_CONNECTIONS;
    case ACL_DENY_REASON_MAX_V6_CONNECTIONS:
      return PS_NET_DOWN_REASON_MAX_V6_CONNECTIONS;
    case ACL_DENY_REASON_IFACE_IN_USE:
      return PS_NET_DOWN_REASON_IFACE_IN_USE;
    case ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH:
      return PS_NET_DOWN_REASON_IFACE_IN_USE_CFG_MATCH;
    case ACL_DENY_REASON_APN_MISMATCH:
      return PS_NET_DOWN_REASON_APN_MISMATCH;
    case ACL_DENY_REASON_IP_VERSION_MISMATCH:
      return PS_NET_DOWN_REASON_IP_VERSION_MISMATCH;
    case ACL_DENY_REASON_DUN_CALL_DISALLOWED:
      return PS_NET_DOWN_REASON_DUN_CALL_DISALLOWED;
    case ACL_DENY_REASON_APN_FORMAT_INVALID:
      return PS_NET_DOWN_REASON_APN_FORMAT_INVALID;
    case ACL_DENY_REASON_INVALID_PROFILE:
      return PS_NET_DOWN_REASON_INVALID_PROFILE;
    case ACL_DENY_REASON_INVALID_PROFILE_ID:
      return PS_NET_DOWN_REASON_INVALID_PROFILE_ID;
    case ACL_DENY_REASON_EPC_NONEPC_TRANSITION:
      return PS_NET_DOWN_REASON_INTERNAL_EPC_NONEPC_TRANSITION;
    case ACL_DENY_REASON_UNPREFERRED_RAT:
      return PS_NET_DOWN_REASON_UNPREFERRED_RAT;
    case ACL_DENY_REASON_IFACE_MISMATCH:
      return PS_NET_DOWN_REASON_IFACE_MISMATCH;
    case ACL_DENY_REASON_COMPANION_IFACE_IN_USE:
      return PS_NET_DOWN_REASON_COMPANION_IFACE_IN_USE;
    case ACL_DENY_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED:
      return PS_NET_DOWN_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED;
    case ACL_DENY_REASON_IP_ADDRESS_MISMATCH:
      return PS_NET_DOWN_REASON_IP_ADDRESS_MISMATCH;
    case ACL_DENY_REASON_EMERGENCY_IFACE_ONLY:
      return PS_NET_DOWN_REASON_EMERGENCY_IFACE_ONLY;
    case ACL_DENY_REASON_IP_PDP_MISMATCH:
      return PS_NET_DOWN_REASON_IP_PDP_MISMATCH;
    case ACL_DENY_REASON_APN_DISALLOWED_ON_ROAMING:
      return PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING;
    case ACL_DENY_REASON_EMERGENCY_AUTH_FAILURE:
      return PS_NET_DOWN_REASON_AUTH_FAILURE_ON_EMERGENCY_CALL;
    case ACL_DENY_REASON_ERR_PDN_IPV4_CALL_THROTTLED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED;
    case ACL_DENY_REASON_ERR_PDN_IPV6_CALL_THROTTLED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED;
    case ACL_DENY_REASON_ERR_PDN_IPV4_CALL_DISALLOWED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_DISALLOWED;
    case ACL_DENY_REASON_ERR_PDN_IPV6_CALL_DISALLOWED:
      return PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_DISALLOWED;
    case ACL_DENY_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED:
      return PS_NET_DOWN_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED;
    case ACL_DENY_REASON_THERMAL_MITIGATION:
      return PS_NET_DOWN_REASON_THERMAL_MITIGATION;
    case ACL_DENY_REASON_SUBS_ID_MISMATCH:
      return PS_NET_DOWN_REASON_SUBS_ID_MISMATCH;
    case ACL_DENY_REASON_DATA_SETTING_DISABLED:
      return PS_NET_DOWN_REASON_DATA_SETTINGS_DISABLED;
    case ACL_DENY_REASON_DATA_ROAMING_SETTING_DISABLED:
      return PS_NET_DOWN_REASON_DATA_ROAMING_SETTINGS_DISABLED;
    case ACL_DENY_REASON_POL_NO_MATCH:
    default:
      return PS_NET_DOWN_REASON_INTERNAL_ERROR;
  }

}

/*===========================================================================
FUNCTION PS_ROUTE_LOCAL_ACL_POLICY_SEARCH_LIST()

DESCRIPTION
  This function does policy-based search over local ACL set for IFACE
  with highest proprity.

DEPENDENCIES
  None

PARAMETERS
  acl_list_id                - ACL list id
  params_ptr                 - Ptr to ACL policy information
  acl_match_ptr              - Ptr to ACL with highest prioity (output)
  result_ptr                 - Ptr to routing results
  deny_reason                - Ptr to deny reason

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
static int ps_route_local_acl_policy_search_list
(
  route_acl_type_enum_type     acl_list_id,
  ps_route_lookup_params_type* params_ptr,
  acl_type**                   acl_match_ptr,
  acl_rt_result_type*          result_ptr,
  int*                         deny_reason
)
{
  acl_type*                 acl_ptr = NULL;
  acl_type**                acl_list_ptr;
  int32                     best_priority = (int32)ACL_DEFAULT_CLASS;
  acl_type*                 best_acl_ptr = NULL;
  int32                     temp = (int32)ACL_DEFAULT_CLASS;
  uint8                     count1 = 0, count2 = 0;
  ps_iface_state_enum_type  state;
  uint8                     iface_deny_count = 0;
  uint8                     temp_deny_cnt = 0;

#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
  ps_routei_acl_match_list_type acl_match_list;
#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY  */
  int                       current_deny_reason = ACL_DENY_REASON_POL_NO_MATCH;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT( NULL != params_ptr );
  ASSERT( NULL != deny_reason );

  if( (NULL == params_ptr->acl_pi_ptr) ||
      (NULL == params_ptr->deny_list_ptr) ||
      (NULL == acl_match_ptr) ||
      (NULL == result_ptr) )
  {
    *deny_reason = ACL_DENY_REASON_POL_NO_MATCH;
    DATA_ERR_FATAL("ps_route_local_acl_policy_search_list():Invalid args");
    return -1;
  }

  if( NULL == (acl_list_ptr = routei_acl_list(acl_list_id)) )
  {
    /*-----------------------------------------------------------------------
      Only the Default ACL list is guranteed to be non-empty.
    -----------------------------------------------------------------------*/
    if (acl_list_id == ROUTE_ACL_DEFAULT_OUTBOUND)
    {
      ASSERT(0);
      *deny_reason = ACL_DENY_REASON_POL_NO_MATCH;
      return -1;
    }
    else
    {
      return 0;
    }
  }

#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
  routei_acl_res_list_reset(&acl_match_list);
#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY  */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Loop over all regiestered ACLs to find the best match
  -------------------------------------------------------------------------*/
  for ( acl_ptr = *acl_list_ptr;
        acl_ptr != NULL;
        acl_ptr = acl_ptr->next_ptr )
  {
    /*-----------------------------------------------------------------------
      Skip the ACL if it is temporarily disabled
    -----------------------------------------------------------------------*/
    if ( acl_ptr->disabled )
    {
      if ( iface_deny_count < params_ptr->deny_list_arr_size )
      {
        *(params_ptr->deny_list_ptr++) = ACL_IF_PTR( acl_ptr );
        temp_deny_cnt++;
      }
      iface_deny_count++;
      continue;
    }

    state = ps_iface_state( ACL_IF_PTR( acl_ptr ));
    if (state  == IFACE_DISABLED)
    {
      continue;
    }

     /*----------------------------------------------------------------------
       For data path routing, skip the ACL if the associated iface is
       disabled or if the policy flag doesn't match the state
     ----------------------------------------------------------------------*/
    if (params_ptr->acl_pi_ptr->bring_up == FALSE &&
        params_ptr->acl_pi_ptr->lookup_only == FALSE)
    {
      if (( params_ptr->acl_pi_ptr->data_path_policy_flag ==
              PS_ACL_POLICY_UP_ONLY && state != IFACE_UP) ||
          ( params_ptr->acl_pi_ptr->data_path_policy_flag ==
              PS_ACL_POLICY_ROUTEABLE_ONLY && state != IFACE_ROUTEABLE) ||
          ( params_ptr->acl_pi_ptr->data_path_policy_flag ==
              PS_ACL_POLICY_NOT_DOWN && state == IFACE_DOWN) ||
          ( params_ptr->acl_pi_ptr->data_path_policy_flag ==
              PS_ACL_POLICY_UP_OR_ROUTEABLE &&
            state != IFACE_ROUTEABLE && state != IFACE_UP))
      {
        continue;
      }
    }

    /*-----------------------------------------------------------------------
      Find the current ACL's priority.
    -----------------------------------------------------------------------*/
    temp = (acl_ptr->acl_fptr)( ACL_IF_PTR( acl_ptr ),
                                params_ptr->acl_pi_ptr,
                                params_ptr->ip_pkt_ptr );
    if ( temp == (int32)ACL_DEFAULT_CLASS )
    {
      continue;
    }
    /*-----------------------------------------------------------------------
      If the ACL denied this iface, store it in the deny-list to prevent it
      from being picked up by address based routing
    -----------------------------------------------------------------------*/
    else if ( temp < (int32)ACL_DEFAULT_CLASS )
    {
      if( iface_deny_count < params_ptr->deny_list_arr_size )
      {
        *(params_ptr->deny_list_ptr++) = ACL_IF_PTR( acl_ptr );
        temp_deny_cnt++;
      }
      iface_deny_count++;

      if ( params_ptr->deny_iface_in_use_arr_ptr != NULL &&
           temp == ACL_DENY_REASON_IFACE_IN_USE &&
           count1 < params_ptr->iface_in_use_arr_size )
      {
        *(params_ptr->deny_iface_in_use_arr_ptr++) = ACL_IF_PTR( acl_ptr );
        count1++;
      }
      else
      if( params_ptr->deny_iface_in_use_cfg_match_arr_ptr != NULL &&
           temp == ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH &&
           count2 < params_ptr->iface_in_use_cfg_match_arr_size )
      {
        *(params_ptr->deny_iface_in_use_cfg_match_arr_ptr++) = ACL_IF_PTR( acl_ptr );

        /* count the number of denied Iface with reason IN_USE_CFG_MATCH */
        count2++;
      }

      if (temp > current_deny_reason)
      {
        current_deny_reason = temp;
      }
      
      *deny_reason = current_deny_reason;
    }
    else
    {
#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
      routei_acl_update_local_srch_results (
                                temp,
                                acl_ptr,
                                (acl_list_id == ROUTE_ACL_DEFAULT_OUTBOUND),
                                &acl_match_list );
#else
      /*---------------------------------------------------------------------
        Check if the current ACL's match (priority) is better
        than what we already found.
      ---------------------------------------------------------------------*/
      if ( routei_acl_is_better_match (temp,
                                       acl_ptr,
                                       best_priority,
                                       best_acl_ptr) )
      {
        best_priority = temp;
        best_acl_ptr = acl_ptr;
      }
#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY  */
    }
  } /* end for */

  if (params_ptr->num_iface_in_use_ptr != NULL)
  {
    *params_ptr->num_iface_in_use_ptr = count1;
  }

  if (params_ptr->num_iface_in_use_cfg_match_ptr != NULL)
  {
    *params_ptr->num_iface_in_use_cfg_match_ptr = count2;
  }

  if ( iface_deny_count > params_ptr->deny_list_arr_size )
  {
    LOG_MSG_INFO2_2("ps_route_local_acl_policy_search_list(): "
                    "%d denied ifaces > %d deny list size",
                    iface_deny_count, params_ptr->deny_list_arr_size);
  }

  /*-----------------------------------------------------------------------
    Notify caller of best candidate.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IP_SESSION_CONTINUITY
  routei_acl_get_best_match ( &acl_match_list,
                              params_ptr->acl_pi_ptr,
                              &best_priority,
                              &best_acl_ptr );

#endif /* FEATURE_DATA_PS_IP_SESSION_CONTINUITY */

  if( best_acl_ptr )
  {
    /* Preserve proxy iface attributes if specified */
    *result_ptr = params_ptr->acl_pi_ptr->rt_result;

    result_ptr->priority = best_priority;

    /*-----------------------------------------------------------------------
      Call the ACL's iface specific post-processing routine if reg'ed
    -----------------------------------------------------------------------*/
    if ( best_acl_ptr->post_proc_fptr )
    {
      (best_acl_ptr->post_proc_fptr)( ACL_IF_PTR( best_acl_ptr ),
                                        params_ptr->acl_pi_ptr,
                                        params_ptr->ip_pkt_ptr );
    }

    /*----------------------------------------------------------------------
      Call common post-processing routine
    ----------------------------------------------------------------------*/
    routei_acl_post_processing( ACL_IF_PTR( best_acl_ptr ),
                                params_ptr->acl_pi_ptr,
                                params_ptr->ip_pkt_ptr );

    *acl_match_ptr = best_acl_ptr;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-----------------------------------------------------------------------
  Unused variable deny_list_ptr.
  CR 779279: To avoid crash commented out below usage of deny_list_ptr
  ----------------------------------------------------------------------*/
/* params_ptr->deny_list_ptr--; */

  return 0;
} /* ps_route_local_acl_policy_search_list() */

/*===========================================================================
FUNCTION PS_ROUTE_LOCAL_ACL_POLICY_SEARCH()

DESCRIPTION
  This function does policy-based search over local ACL sets for IFACE
  with highest proprity. This function tries to build a chain of IFACEs if
  necessary.

  The search process starts with the ACL list for physical ifaces (or default),
  then proceeds to ACL list for logical ifaces. Each ACL list search produces
  a compoment IFACE for the IFACE chain, and the IFACEs from the consecutive
  searches will be associated together.

  Note: This function only builds a chain of IFACE for an any-iface search.

DEPENDENCIES
  None

PARAMETERS
  params_ptr                 - Ptr to ACL policy information
  acl_match_ptr              - Ptr to ACL with highest prioity (output)
  result_ptr                 - Ptr to routing results

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
static int ps_route_local_acl_policy_search
(
  ps_route_lookup_params_type * params_ptr,
  acl_type**                    acl_match_ptr,
  acl_rt_result_type*           result_ptr,
  int*                          deny_reason
)
{
  acl_type*                 acl_ptr = NULL;
  acl_rt_result_type        rt_result;
#ifdef FEATURE_DATA_PS_464XLAT
  boolean                   clat_search_logical_iface = FALSE;
#endif /* FEATURE_DATA_PS_464XLAT */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset((void*)&rt_result,0x0,sizeof(rt_result));
  rt_result.priority = (int32)ACL_DEFAULT_CLASS;

  /*-----------------------------------------------------------------------
    Search on the physical list and logical lists
     1. search local ACL list for physical IFACEs first and then for logical
        IFACEs.
     2. update search polily for further search
     3. search until no IFACE can be found in a given iface list
     4. return the last found IFACE (the top of the IFACE chain)
  ------------------------------------------------------------------------*/

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if ( -1 == ps_route_local_acl_policy_search_list( ROUTE_ACL_DEFAULT_OUTBOUND,
                                                    params_ptr,
                                                    &acl_ptr,
                                                    &rt_result,
                                                    deny_reason) )
  {
    LOG_MSG_ERROR_1("ps_route_local_acl_policy_search(): "
                    "Routing search failed on local search(phys):%d",
                    ROUTE_ACL_DEFAULT_OUTBOUND);
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }


  /*------------------------------------------------------------------------
    Update the search result
  -------------------------------------------------------------------------*/
  if( acl_ptr )
  {
    LOG_MSG_INFO2_3("ps_route_local_acl_policy_search(): "
                    "ACL Srch (phys): Found ACL match on iface 0x%x:%d with priority %d",
                    ACL_IF_PTR(acl_ptr)->name,
                    ACL_IF_PTR(acl_ptr)->instance,
                    rt_result.priority );
    /*----------------------------------------------------------------------
      Preserve proxy iface attributes if specified
    -----------------------------------------------------------------------*/
    *result_ptr = params_ptr->acl_pi_ptr->rt_result;
    result_ptr->priority = rt_result.priority;

    *acl_match_ptr = acl_ptr;
  }

#ifdef FEATURE_DATA_PS_464XLAT
  if ( (*deny_reason == ACL_DENY_REASON_V4_DISALLOWED_SUBS_LIMITED ||
      *deny_reason == ACL_DENY_REASON_IP_VERSION_MISMATCH)  && 
       ps_clat_is_enabled() && params_ptr->acl_pi_ptr->ip_family == IPV4_ADDR)
  {
    clat_search_logical_iface = TRUE;
  }
#endif /* FEATURE_DATA_PS_464XLAT */

  /*-------------------------------------------------------------------
    Continue to search ACL LOGICAL LIST
  -------------------------------------------------------------------*/
  while ( acl_ptr 
         #ifdef FEATURE_DATA_PS_464XLAT
         || (clat_search_logical_iface == TRUE)
         #endif /* FEATURE_DATA_PS_464XLAT */ 
         )
  {
#ifdef FEATURE_DATA_PS_464XLAT
    clat_search_logical_iface = FALSE;
    /*----------------------------------------------------------------------
      Update policy with best IFACE selection to assist further search.
    -----------------------------------------------------------------------*/
    if (acl_ptr == NULL)
    {
      params_ptr->acl_pi_ptr->assoc_iface_info.ps_iface_ptr = NULL;
    }
    else
#endif /* FEATURE_DATA_PS_464XLAT */
    {
      params_ptr->acl_pi_ptr->assoc_iface_info.ps_iface_ptr = ACL_IF_PTR(acl_ptr);
    }

    acl_ptr = NULL;
    memset((void*)&rt_result,0x0,sizeof(rt_result));
    rt_result.priority = (int32)ACL_DEFAULT_CLASS;

    if ( ps_route_local_acl_policy_search_list( ROUTE_ACL_LOGICAL_OUTBOUND,
                                                params_ptr,
                                                &acl_ptr,
                                                &rt_result,
                                                deny_reason ) == -1 )
    {
      LOG_MSG_ERROR_1("ps_route_local_acl_policy_search(): "
                      "Routing search failed on local search(logical):%d",
                      ROUTE_ACL_LOGICAL_OUTBOUND);
      PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }

    /*------------------------------------------------------------------------
      Ready to return the search result
    -------------------------------------------------------------------------*/
    if( NULL == acl_ptr )
    {
      break;
    }
    else
    {
      LOG_MSG_INFO2_3("ps_route_local_acl_policy_search(): "
                      "ACL Srch(logical): Found ACL match on iface 0x%x:%d with priority %d",
                      ACL_IF_PTR(acl_ptr)->name,
                      ACL_IF_PTR(acl_ptr)->instance,
                      rt_result.priority );
      /* Preserve proxy iface attributes if specified */
      *result_ptr = params_ptr->acl_pi_ptr->rt_result;
      result_ptr->priority = rt_result.priority;

      *acl_match_ptr = acl_ptr;
    }

  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return 0;
}  /* ps_route_local_acl_policy_search */



/*===========================================================================
FUNCTION PS_ROUTE_NETWORK_SELECTION_BY_POLICY()

DESCRIPTION
  This function performs network selection based on the input policy. All
  interface ACLs are compared and the best match is returned.

DEPENDENCIES
  If ip_pkt_ptr is NULL, gateway_ptr must be NULL.

PARAMETERS
  ip_pkt_ptr       - Ptr to packet's IP information
  acl_pi_ptr       - Ptr to packet's policy information
  gateway_ptr      - Ptr to IP address of the routing gateway in network order
  deny_list_ptr              - Ptr to denied IFACE list
  deny_iface_in_use_arr_ptr  - Ptr to denied(in use) IFACE list
  iface_in_use_arr_size      - Denied(in use) IFACE list limit
  num_iface_in_use_ptr       - Ptr to denied(in use) IFACE list length (output)
  if_ptr           - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
static int ps_route_network_selection_by_policy
(
  ps_route_lookup_params_type *    params_ptr,
  ps_iface_net_down_reason_type *  net_down_reason
)
{
  acl_type                * acl_ptr = NULL;
  acl_type                * acl_match_ptr = NULL;
  ps_iface_type           * ps_iface_ptr;
  acl_rt_result_type        rt_result;
  int32                     temp = (int32) ACL_DEFAULT_CLASS;
  uint32                    index;
  int                       deny_reason = ACL_DENY_REASON_POL_NO_MATCH;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != params_ptr);
  ASSERT( NULL != net_down_reason);

  memset((void *) &rt_result, 0x0, sizeof(rt_result));
  rt_result.priority = (int32) ACL_DEFAULT_CLASS;

  if (params_ptr->acl_pi_ptr->iface.kind == DSS_IFACE_ID)
  {

    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    ps_iface_ptr = ps_iface_get_handle( params_ptr->acl_pi_ptr->iface.info.id);
    if(ps_iface_ptr == NULL)
    {
      LOG_MSG_ERROR_1("ps_route_network_selection_by_policy(): Invalid iface ID: %d",
                    params_ptr->acl_pi_ptr->iface.info.id);
      *net_down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }

    if (ps_iface_ptr->outgoing_acl_ptr == NULL)
    {
      LOG_MSG_ERROR_0("ps_route_network_selection_by_policy(): "
                    "outgoing_acl_ptr is null");
      if (NULL != net_down_reason)
      {
        *net_down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
      }
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }
    
    if (ps_iface_ptr->outgoing_acl_ptr->acl_fptr == NULL)
    {
      LOG_MSG_ERROR_0("ps_route_network_selection_by_policy(): "
                    "acl_fptr is null");
      *net_down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return -1;
    }
    
    temp = ps_iface_ptr->outgoing_acl_ptr->acl_fptr( ps_iface_ptr,
                                                     params_ptr->acl_pi_ptr,
                                                     params_ptr->ip_pkt_ptr);
    if (temp > (int32) ACL_DEFAULT_CLASS)
    {
      params_ptr->acl_pi_ptr->rt_result.priority = temp;
      acl_match_ptr = ps_iface_ptr->outgoing_acl_ptr;

      /*-----------------------------------------------------------------------
        Call the ACL's iface specific post-processing routine if reg'ed
      -----------------------------------------------------------------------*/
      if ( acl_match_ptr->post_proc_fptr )
      {
        (acl_match_ptr->post_proc_fptr)( ACL_IF_PTR( acl_match_ptr ),
                                         params_ptr->acl_pi_ptr,
                                         params_ptr->ip_pkt_ptr );
      }

      /*----------------------------------------------------------------------
        Call common post-processing routine
      ----------------------------------------------------------------------*/
      routei_acl_post_processing( ACL_IF_PTR( acl_match_ptr ),
                                  params_ptr->acl_pi_ptr,
                                  params_ptr->ip_pkt_ptr );


    }
    else
    {
      deny_reason = temp;
    }

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  }
  else
  {
    /*-----------------------------------------------------------------------
      Loop over callback table for all processors, find best IFACE overall.
      Note callback table to search is ordered to do remote processors first
      then the local processor, so the proxy IFACE routing data is available.
    -----------------------------------------------------------------------*/
    for (index = 0; index < (uint32) ACL_PROC_MAX; index++)
    {
      /*---------------------------------------------------------------------
        Invoke ACl search callback if registered for processor.
      ---------------------------------------------------------------------*/
      if (_acl_priority_cb_tbl[ index ])
      {
        /*-------------------------------------------------------------------
          Reset information so that stale information from attached modem is
          not used if local routing fails - CR 337479
        -------------------------------------------------------------------*/
        memset((void *) &rt_result, 0x0, sizeof(rt_result));
        acl_match_ptr = NULL;

        if (-1 == _acl_priority_cb_tbl[ index ]( params_ptr,
                                                 &acl_ptr,
                                                 &rt_result,
                                                 &deny_reason ) )
        {
          LOG_MSG_ERROR_1("ps_route_network_selection_by_policy(): "
                          "Routing search failed on processor:%d", index);
        }
        else
        {
          /*-----------------------------------------------------------------
            Check for priority match results.
          -----------------------------------------------------------------*/
          if (rt_result.priority == (int32)ACL_DEFAULT_CLASS)
          {
            continue;
          }
          /*-----------------------------------------------------------------
            Check if the current ACL's match (priority) is better than what
            was already found.  In case of tie give preference to the latter
            match.  This ensures the local processor IFACE in multi-processor
            case is selected.
          -----------------------------------------------------------------*/
          else if (rt_result.priority >=
                      (int32) params_ptr->acl_pi_ptr->rt_result.priority)
          {
            acl_match_ptr = acl_ptr;

            /*---------------------------------------------------------------
              Update policy with best IFACE selection
            ---------------------------------------------------------------*/
            PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
            params_ptr->acl_pi_ptr->rt_result = rt_result;
            PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          }
        }
      }
    }
  }

  /*-------------------------------------------------------------------------
    Check if we found a matching route from the ACLs
  -------------------------------------------------------------------------*/
  if (acl_match_ptr != NULL &&
      params_ptr->acl_pi_ptr->rt_result.priority != (int32) ACL_DEFAULT_CLASS)
  {
    LOG_MSG_INFO2_3("ps_route_network_selection_by_policy(): "
                    "RT: Found ACL match on iface 0x%x:%d with priority %d",
                    ACL_IF_PTR(acl_match_ptr)->name,
                    ACL_IF_PTR(acl_match_ptr)->instance,
                    params_ptr->acl_pi_ptr->rt_result.priority);

    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    *params_ptr->if_ptr = ACL_IF_PTR( acl_match_ptr );
    acl_match_ptr->pkt_count++;

    /*-----------------------------------------------------------------------
      Get the gateway address for this route if it's not
      a point-to-point link (NULL gateway pointer).
    -----------------------------------------------------------------------*/
    if( params_ptr->gateway_ptr )
    {
      if(params_ptr->ip_pkt_ptr->ip_vsn == IP_V4)
      {
        temp =
          routei_get_v4_route( params_ptr->ip_pkt_ptr->ip_hdr.v4.dest.ps_s_addr,
                               NULL);
        if (temp != -1)
        {
          params_ptr->gateway_ptr->addr.v4.ps_s_addr =
            routei_v4_routing_table.entry[temp].gateway.ps_s_addr;
        }
      }
    }

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }
  else
  {
    *net_down_reason = ps_route_acl_deny_to_net_down_reason(deny_reason);
    LOG_MSG_ERROR_2("ps_route_network_selection_by_policy(): "
                  "Routing failed net_down_reason: 0x%x acl_deny_reason: %d",
                  *net_down_reason, deny_reason);
    return -1;
  }

  return 0;
} /* ps_route_network_select_by_policy() */



/*===========================================================================
FUNCTION PS_ROUTE_BY_IPV4_ADDRESS()

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.

DEPENDENCIES
  Currently addr based routing is supported for v4 only.
  If ip_pkt_ptr is NULL, gateway_ptr must be NULL and addr_based_routing_only
  must be FALSE.

PARAMETERS
  ip_pkt_ptr       - Ptr to packet's IP information
  acl_pi_ptr       - Ptr to packet's policy information
  gateway_ptr      - Ptr to IP address of the routing gateway in network order
  if_ptr           - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
static int ps_route_by_ipv4_address
(
  ip_pkt_info_type*        ip_pkt_ptr,
  ps_ip_addr_type*         gateway_ptr,
  ps_iface_type**          deny_list_ptr,
  ps_iface_type**          if_ptr
)
{
  int                         temp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Do an IP address based route lookup - if the resulting iface is disabled
    or denied from the ACL lookup route_get() will just return E_NO_ROUTE.
    Adding this into the routing table lookup itself to cover multiple routes
    to the same destination is TBD.
  -------------------------------------------------------------------------*/
  temp = routei_get_v4_route( ip_pkt_ptr->ip_hdr.v4.dest.ps_s_addr, NULL );
  if ( temp == -1 )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_1("ps_route_by_ipv4_address(): "
                    "RT: No route found for dst 0x%x",
                    ip_pkt_ptr->ip_hdr.v4.dest.ps_s_addr );
    return -1;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Check deny_list_ptr to see if this iface is allowed to route this IP
      pkt. deny_list_ptr points at the next available location which does not
      have a valid iface entry. So we always decrement it by one and start
      searching backwards until we have processed the whole list.
    -----------------------------------------------------------------------*/
    deny_list_ptr--;
    for ( ; deny_list_ptr >= _route_deny_list; deny_list_ptr-- )
    {
      if ( routei_v4_routing_table.entry[temp].if_ptr == *deny_list_ptr )
      {
        LOG_MSG_INFO1_2("ps_route_by_ipv4_address(): "
                        "RT: Found denied interface 0x%x:%d",
                        (*deny_list_ptr)->name, (*deny_list_ptr)->instance );
        temp = -1;
        break;
      }
    }

    if ( temp != -1 )
    {
      LOG_MSG_INFO2_2("ps_route_by_ipv4_address(): "
                      "RT: Found table match for iface 0x%x:%d",
                      routei_v4_routing_table.entry[temp].if_ptr->name,
                      routei_v4_routing_table.entry[temp].if_ptr->instance );
      *if_ptr  = routei_v4_routing_table.entry[temp].if_ptr;

      if ( gateway_ptr )
      {
        gateway_ptr->addr.v4.ps_s_addr =
                         routei_v4_routing_table.entry[temp].gateway.ps_s_addr;
      }

      routei_v4_routing_table.entry[temp].pkt_count++;
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
} /* ps_route_by_ipv4_address() */


/*===========================================================================
FUNCTION ROUTE_GET()

DESCRIPTION
  This function allows to retrieve a route from the DMSS
  routing table.

DEPENDENCIES
  Currently addr based routing is supported for v4 only.
  If ip_pkt_ptr is NULL, gateway_ptr must be NULL and addr_based_routing_only
  must be FALSE.

PARAMETERS
  ip_pkt_ptr       - Ptr to packet's IP information
  acl_pi_ptr       - Ptr to packet's policy information
  addr_based_routing_only - Don't use ACLs for routing decision if true
  gateway_ptr      - Ptr to IP address of the routing gateway in network order
  if_ptr           - Ptr local interface route goes through

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

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
)
{
  boolean                     acl_lookup_failed = FALSE;
  ps_iface_type*              iface_ptr = NULL;
  ps_iface_type**             deny_list_ptr = _route_deny_list;
  unsigned int                deny_list_size = sizeof(_route_deny_list)/sizeof(_route_deny_list[0]);
  ps_route_lookup_params_type lookup_params;
  ps_iface_net_down_reason_type  net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* init lookup params */
  memset( &lookup_params, 0, sizeof(ps_route_lookup_params_type) );

  /* Set some default values and check for IPv4 */
  if(ip_pkt_ptr)
  {
    ASSERT( ip_pkt_ptr->ip_vsn == IP_V4 || ip_pkt_ptr->ip_vsn == IP_V6);
  }
  *if_ptr = NULL;

  /*-------------------------------------------------------------------------
    If ip filter info is specified, make sure its V4 only. Currently v6 is
    not supported.
    If IP filter info is not specified, address based routing cannot be
    performed and hence gateway also cannot be looked up. Policy routing must
    be allowed in this case, hence addr_based_routing_only should be FALSE.
  -------------------------------------------------------------------------*/
  if(ip_pkt_ptr == NULL)
  {
    if(gateway_ptr || addr_based_routing_only)
    {
      DATA_ERR_FATAL("route_get(): Null IP pkt info, addr routing not allowed");
      return -1;
    }
  }

  if ( gateway_ptr )
  {
    gateway_ptr->type = IPV4_ADDR;
    gateway_ptr->addr.v4.ps_s_addr = 0;
  }

  /*------------------------------------------------------------------------
    If address is a V6 address it could be a V4 MAPPED V6 addr. If it is
    convert it to it's native V4 format.
  ------------------------------------------------------------------------*/
  if( ip_pkt_ptr && validate_ip_pkt_info(ip_pkt_ptr, acl_pi_ptr, if_ptr) != 0 )
  {
    LOG_MSG_INFO1_0("route_get(): "
                    "Route lookup failed");
    return -1;
  }
  if(*if_ptr != NULL)
  {
    return 0;
  }

  if(acl_pi_ptr == NULL)
  {
    if (ip_pkt_ptr == NULL)
    {
      return -1;
    }

    /*-----------------------------------------------------------------
      We dont have any acl to run hence return the iface wanted by caller
    -----------------------------------------------------------------*/
    *if_ptr = ip_pkt_ptr->if_ptr;
    return 0;
  }

  if(acl_pi_ptr->subs_id < PS_SYS_DEFAULT_SUBS ||
     acl_pi_ptr->subs_id >= PS_SYS_SUBS_MAX)
  {
    LOG_MSG_ERROR_1("route_get(): "
                    "Invalid subs  %d", acl_pi_ptr->subs_id);
    ASSERT(0);
    return -1;
  }

  if(acl_pi_ptr->subs_id == PS_SYS_DEFAULT_SUBS)
  {
    acl_pi_ptr->subs_id = ps_sys_get_default_data_subscription();
  }

  LOG_MSG_INFO1_1("route_get: subs %d", acl_pi_ptr->subs_id);

  ps_routei_acl_policy_trace_dump (acl_pi_ptr,"route_get()");

  /*-------------------------------------------------------------------------
    First, try to do a policy based match if allowed
  -------------------------------------------------------------------------*/
  if( !addr_based_routing_only && acl_pi_ptr )
  {
    /*-----------------------------------------------------------------------
      Initialize policy routing search results.
    -----------------------------------------------------------------------*/
    acl_pi_ptr->rt_result.priority = (int32)ACL_DEFAULT_CLASS;
    acl_pi_ptr->rt_result.if_hndl  = -1;
    acl_pi_ptr->rt_result.rm_hndl  = -1;

    if( (acl_pi_ptr->bring_up == TRUE || acl_pi_ptr->lookup_only == TRUE))
    {
      /*---------------------------------------------------------------------
        Loop over all ACLs. Used for:
        1. Initial Network Selection
        2. Interface lookups
      ---------------------------------------------------------------------*/
      lookup_params.ip_pkt_ptr = ip_pkt_ptr;
      lookup_params.acl_pi_ptr = acl_pi_ptr;
      lookup_params.gateway_ptr = gateway_ptr;
      lookup_params.deny_list_ptr = deny_list_ptr;
      lookup_params.deny_list_arr_size = (uint8)deny_list_size;
      lookup_params.deny_iface_in_use_arr_ptr = NULL;
      lookup_params.iface_in_use_arr_size = 0;
      lookup_params.num_iface_in_use_ptr = NULL;
      lookup_params.if_ptr = &iface_ptr;

      if(-1 == ps_route_network_selection_by_policy( &lookup_params, &net_down_reason))
      {
        acl_lookup_failed = TRUE;
      }
    }
    else
    {
       /*---------------------------------------------------------------------
        Loop over ACLs for interfaces which are in "UP" state. Used for:
        1. Data path lookups
      ---------------------------------------------------------------------*/
      (void) ps_route_data_path_lookup(ip_pkt_ptr,
                                       acl_pi_ptr,
                                       addr_based_routing_only,
                                       &iface_ptr);
    }
  }

  if((addr_based_routing_only || acl_lookup_failed) && ip_pkt_ptr)
  {
    if(ip_pkt_ptr->ip_vsn == IP_V4)
    {
      /*---------------------------------------------------------------------
        Perform IPv4 address based routing. Used when:
        1. When address based routing is requested
        2. ACL lookups fail to match an interface

        Doesn't matter if function returns 0/-1 if iface_ptr is populated
        the lookup was successful.
      ---------------------------------------------------------------------*/
      (void)ps_route_by_ipv4_address(ip_pkt_ptr,
                                     gateway_ptr,
                                     deny_list_ptr,
                                     &iface_ptr);
    }
  }

  *if_ptr = iface_ptr;

  /*-------------------------------------------------------------------------
    Check the outcome of the lookup
  -------------------------------------------------------------------------*/
  if(iface_ptr == NULL)
  {
    LOG_MSG_INFO1_0("route_get(): "
                    "ROUTE LOOKUP FAILED");
    return -1;
  }
  else
  {
    return 0;
  }
} /* route_get() */


/*===========================================================================
FUNCTION ROUTE_FLUSH()

DESCRIPTION
  This function allows to flush entries for a specific interface
  or the complete DMSS routing table.

DEPENDENCIES
  None

PARAMETERS
  if_ptr  - Pointer to allow flushing interface specifc routes only
  ps_errno  - Ptr to error code, filled in by function in failure case

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  No IP traffic can be sent or forwarded until a new route
  is added to the DMSS routing table.
===========================================================================*/
int route_flush
(
  ps_iface_type*  if_ptr
)
{
  uint16        i = 0;
  volatile int  t_size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if ( if_ptr == NULL )
  {
    /* Flash the whole table */
    LOG_MSG_INFO1_0("route_flush(): "
                    "RT: Flushing table" );
    memset( &routei_v4_routing_table,
            0,
            sizeof( routei_v4_routing_table ) );
    t_size = 0;
  }
  else
  {
    /* Only delete this interface's routes */
    LOG_MSG_INFO1_2("route_flush(): "
                    "RT: Flushing table for iface 0x%x:%d",
                    if_ptr->name, if_ptr->instance);
    t_size = (int)routei_v4_routing_table.table_size_used;
    while( (t_size > 0) && (i < t_size) )
    {
      if ( routei_v4_routing_table.entry[i].if_ptr == if_ptr )
      {
        t_size = routei_free_v4( i );
      }
      else
      {
        i++;
      }
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return ((t_size == -1) ? -1 : 0);

} /* route_flush() */


/*===========================================================================
FUNCTION ROUTE_ACL_ADD()

DESCRIPTION
  This function allows to add an Access Control List to the
  DMSS ACL list used for policy based routing.
  The ACL's priority can be specified globally (no interface
  class) or within a specific interface class.

DEPENDENCIES
  The ACL points to a valid interface instance.

PARAMETERS
  list_type     - ACL list to add to
  new_acl_ptr  - Ptr to new ACL, memory provided by caller
  priority     - Priority for this ACL within the ACL list

RETURN VALUE
  0 for success, -1 for failure

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_add
(
  route_acl_type_enum_type           rlist_type,
  acl_type*                          new_acl_ptr,
  route_acl_priority_enum_type       priority
)
{
  acl_type**  acl_list_ptr;
  acl_type*   acl_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((acl_list_ptr = routei_acl_list(rlist_type)) == NULL)
  {
    ASSERT(0);
    return -1;
  }

  /* Initialize the non-application owned fields */
  new_acl_ptr->disabled  = FALSE;
  new_acl_ptr->pkt_count = 0;
  new_acl_ptr->next_ptr  = NULL;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Check for an empty list first */
  if ( *acl_list_ptr == NULL )
  {
    *acl_list_ptr = new_acl_ptr;
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;
  }

  acl_ptr = *acl_list_ptr;

  switch( priority )
  {
    case ROUTE_ACL_ANY_PRIORITY:
      /* Just append the ACL to the routing list */
      while( acl_ptr->next_ptr != NULL ) acl_ptr = acl_ptr->next_ptr;
      acl_ptr->next_ptr = new_acl_ptr;
      break;

    case ROUTE_ACL_TOP_PRIORITY:
      LOG_MSG_ERROR_0("route_acl_add(): "
                      "RT: Unsupported op" );
      break;

      default:
        break;
      // ...
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;

} /* route_acl_add() */


/*===========================================================================
FUNCTION ROUTE_ACL_DELETE()

DESCRIPTION
  This function allows to remove an existing ACL from the
  DMSS ACL list for policy based routing.

DEPENDENCIES
  None

PARAMETERS
  list_type     - ACL list to delete from
  del_acl_ptr  - Ptr to ACL to be deleted from list

RETURN VALUE
  0 for success, -1 for failure (ps_errno contains error code).

SIDE EFFECTS
  None
===========================================================================*/
int route_acl_delete
(
  route_acl_type_enum_type        rlist_type,
  acl_type*                       del_acl_ptr
)
{
  acl_type** acl_list_ptr;
  acl_type*  tmp_ptr;
  acl_type*  acl_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((acl_list_ptr = routei_acl_list(rlist_type)) == NULL)
  {
    ASSERT(0);
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Check if the first ACL in the list is to be deleted */
  if ( del_acl_ptr == *acl_list_ptr )
  {
    *acl_list_ptr = del_acl_ptr->next_ptr;
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;
  }

  tmp_ptr = acl_ptr = *acl_list_ptr;

  /* Appearently not - walk through until we find it */
  while ( (acl_ptr != del_acl_ptr)
          &&
          (acl_ptr != NULL ) )
  {
    tmp_ptr = acl_ptr;
    acl_ptr = acl_ptr->next_ptr;
  }

  if ( acl_ptr == NULL )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return -1;
  }

  /* If we found it, adjust the list to exclude it */
  tmp_ptr->next_ptr = acl_ptr->next_ptr;

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return 0;
} /* route_acl_delete() */


/*===========================================================================
FUNCTION ROUTE_ACL_CONTROL()

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
  route_acl_id_enum_type             acl_id_type,
  route_acl_id_type                  acl_id,
  boolean                            acl_enable
)
{
  acl_type** acl_list_ptr;
  acl_type*  acl_ptr = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  /* Check how the ACL is given to us */
  switch ( acl_id_type )
  {
    case ROUTE_ACL_PTR_ID:
      /* The caller gave us the ACL pointer - adjust the ACL's state */
      acl_ptr = acl_id.acl_ptr;
      break;

    case ROUTE_ACL_INFO_ID:
      if((acl_list_ptr = routei_acl_list(acl_id.acl_info.list_type)) == NULL)
      {
        ASSERT(0);
        break;
      }

      /* The caller gave us the associated iface pointer - find */
      /* the corresponding ACL and adjust it's state.           */
      for ( acl_ptr = *acl_list_ptr;
            acl_ptr != NULL;
            acl_ptr = acl_ptr->next_ptr )
      {
        if(acl_ptr->if_ptr == acl_id.acl_info.if_ptr)
        {
          break;
        }
      }
      break;

    default:
      break;
  }

  if ( acl_ptr == NULL)
  {
    LOG_MSG_ERROR_2("route_acl_control(): "
                    "RT: Bad ACL Info id:%d, 0x%x", acl_id_type, &acl_id );
  }
  else
  {
    /* Found the ACL, perform the control operation here */
    acl_ptr->disabled = (acl_enable) ? FALSE : TRUE;
    LOG_MSG_INFO1_2("route_acl_control(): "
                    "RT: ACL 0x%x enabled = %d", acl_ptr, acl_enable );
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

} /* route_acl_control() */


/*===========================================================================
FUNCTION ROUTE_ACL_SET_PRIORITY()

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
  route_acl_type_enum_type           rlist_type,
  acl_type*                          acl_ptr,
  route_acl_priority_enum_type       priority
)
{
  if ( route_acl_delete( rlist_type, acl_ptr ) == -1 )
  {
    return -1;
  }

  return route_acl_add( rlist_type, acl_ptr, priority );

} /* route_acl_set_priority() */


/*===========================================================================
FUNCTION ROUTE_ACL_EXECUTE()

DESCRIPTION
  This function executes ACLs registered with a specified ACL list type and
  returns the iface associated with ACL which exhibits highest priority
  based upon the IP pkt and policy information passed.

DEPENDENCIES
  The ACL points to a valid interface instance.

PARAMETERS
  list_type    - ACL list to operate on
  ip_pkt_ptr    - Ip pkt information
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
  route_acl_type_enum_type           rlist_type,
  ip_pkt_info_type*                  ip_pkt_ptr,
  acl_policy_info_type*              acl_pi_ptr,
  ps_iface_type**                    if_ptr
)
{
  acl_type**                  acl_list_ptr;
  acl_type*                   acl_ptr = NULL;
  acl_type*                   acl_match_ptr = NULL;
  int                         best_priority = ACL_DEFAULT_CLASS;
  int                         temp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  *if_ptr = NULL;

  if(ip_pkt_ptr && ip_pkt_ptr->ip_vsn != IP_V4 && ip_pkt_ptr->ip_vsn != IP_V6)
  {
    ASSERT(0);
    LOG_MSG_ERROR_1("route_acl_execute(): "
                    "Invalid IP protocol specified: %d", ip_pkt_ptr->ip_vsn);
    return -1;
  }

  if((acl_list_ptr = routei_acl_list(rlist_type)) == NULL)
  {
    ASSERT(0);
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if (rlist_type >= ROUTE_ACL_OUTBOUND_MAX)
  {
    is_inbound_acl = TRUE;
  }

  /*-------------------------------------------------------------------------
    Loop over all regiestered ACLs to find the best match
  -------------------------------------------------------------------------*/
  for ( acl_ptr = *acl_list_ptr;
        acl_ptr != NULL;
        acl_ptr = acl_ptr->next_ptr )
  {
    /*-----------------------------------------------------------------------
      Skip the ACL if it is temporarily disabled
    -----------------------------------------------------------------------*/
    if ( acl_ptr->disabled )
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Skip the ACL if the associated iface is disabled
    -----------------------------------------------------------------------*/
    if ( ps_iface_state( ACL_IF_PTR( acl_ptr ) ) == IFACE_DISABLED )
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Get the priority of this ACL
    -----------------------------------------------------------------------*/
    temp = (acl_ptr->acl_fptr)( ACL_IF_PTR( acl_ptr ),
                                acl_pi_ptr,
                                ip_pkt_ptr );


    /*-----------------------------------------------------------------------
      Check if the current ACL's match (priority) is better than what we
      already found.
    -----------------------------------------------------------------------*/
    if ( temp > best_priority )
    {
      /*---------------------------------------------------------------------
        Save this ACL's info as a possible candidate
      ---------------------------------------------------------------------*/
      best_priority = temp;
      acl_match_ptr = acl_ptr;
    }
  }

  /*-------------------------------------------------------------------------
    Check if we found a matching route from the ACLs
  -------------------------------------------------------------------------*/
  if( acl_match_ptr != NULL )
  {
    LOG_MSG_INFO3_3("route_acl_execute(): "
                    "Found ACL match for iface 0x%x:%d with priority %d",
                    ACL_IF_PTR(acl_match_ptr)->name,
                    ACL_IF_PTR(acl_match_ptr)->instance,
                    best_priority );
    *if_ptr = ACL_IF_PTR( acl_match_ptr );
    acl_match_ptr->pkt_count++;

    /*-----------------------------------------------------------------------
      Call the ACL's iface specific post-processing routine if reg'ed
    -----------------------------------------------------------------------*/
    if ( acl_match_ptr->post_proc_fptr )
    {
      (*acl_match_ptr->post_proc_fptr)( ACL_IF_PTR( acl_match_ptr ),
                                        acl_pi_ptr,
                                        ip_pkt_ptr );
    }
  }

  is_inbound_acl = FALSE;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*-------------------------------------------------------------------------
    Check the outcome of the lookup
  -------------------------------------------------------------------------*/
  if ( *if_ptr == NULL )
  {
    LOG_MSG_INFO3_1("route_acl_execute(): "
                    "No ACL match found for list %d", rlist_type);
    return -1;
  }

  return 0;

} /* route_acl_execute() */
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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(route_scope, 0, sizeof(ps_route_scope_type));

  if(FALSE == PS_IFACE_IS_VALID(if_ptr))
  {
    LOG_MSG_INFO1_0("Invalid iface");
    return (FALSE);
  }

  ps_route_scope_add(route_scope, PS_IFACE_GET_IFACE_INDEX(if_ptr));

  return (TRUE);

} /* ps_route_scope_by_iface_handle() */

/*===========================================================================
FUNCTION PS_ROUTE_SCOPE_BY_POLICY()

DESCRIPTION
  This function allows to retrieve a route_scope from the netpolicy.

DEPENDENCIES
  None.

PARAMETERS
  ip_pkt_ptr - Ptr to packets info.
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
)
{
  acl_type*                acl_ptr = NULL;
  acl_type**               acl_list_ptr;
  int                      temp;
  ps_iface_type            *if_ptr;
  ps_route_scope_type      route_scope;
  ps_route_scope_type      if_route_scope;
  route_acl_type_enum_type index;
#ifdef FEATURE_DATA_PS_IPV6
  uint8                    temp_protocol;
  uint32                   temp_addr;
#endif
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&route_scope,0,sizeof(ps_route_scope_type));

  if(acl_pi_ptr == NULL)
  {
    LOG_MSG_INFO1_0("NULL acl_policy_info passed to route_scope_by_policy");
    return (route_scope);
  }

  if(acl_pi_ptr->subs_id < PS_SYS_DEFAULT_SUBS ||
     acl_pi_ptr->subs_id >= PS_SYS_SUBS_MAX)
  {
    LOG_MSG_ERROR_1("route_scope_by_policy(): "
                    "Invalid subs  %d", acl_pi_ptr->subs_id);
    ASSERT(0);
    return (route_scope);
  }

  if(acl_pi_ptr->subs_id == PS_SYS_DEFAULT_SUBS)
  {
    acl_pi_ptr->subs_id = ps_sys_get_default_data_subscription();
  }

  LOG_MSG_INFO2_1("ps_route_scope_by_policy: subs %d", acl_pi_ptr->subs_id);

  /*---------------------------------------------------------------------
    Add the loopback route scope.
  ---------------------------------------------------------------------*/
  if (acl_pi_ptr->ip_family == IFACE_IPV6_ADDR_FAMILY)
  {
    ps_route_scope_add_lo(&route_scope, TRUE);
  }
  else
  {
    ps_route_scope_add_lo(&route_scope, FALSE);
  }

  /*------------------------------------------------------------------------
    If address is a V6 address it could be a V4 MAPPED V6 addr. If it is
    convert it to it's native V4 format.
  ------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
  if( ip_pkt_ptr && (ip_pkt_ptr->ip_vsn == IP_V6) )
  {
    if(PS_IN6_IS_ADDR_V4MAPPED(&ip_pkt_ptr->ip_hdr.v6.hdr_body.
                               base_hdr.src_addr))
    {
      ip_pkt_ptr->ip_vsn = IP_V4;
      temp_protocol = ip_pkt_ptr->ip_hdr.v4.protocol;
      ip_pkt_ptr->ip_hdr.v4.protocol = temp_protocol;



      temp_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(
                  &ip_pkt_ptr->ip_hdr.v6.hdr_body.base_hdr.src_addr);
      ip_pkt_ptr->ip_hdr.v4.source.ps_s_addr = temp_addr;

      temp_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(
                  &ip_pkt_ptr->ip_hdr.v6.hdr_body.base_hdr.dst_addr);
      ip_pkt_ptr->ip_hdr.v4.dest.ps_s_addr = temp_addr;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  for (index = ROUTE_ACL_OUTBOUND_MIN; index < ROUTE_ACL_OUTBOUND_MAX; index++)
  {
    acl_list_ptr = routei_acl_list(index);
    if (acl_list_ptr == NULL)
    {
      LOG_MSG_ERROR_1("Couldn't find acl list at idx %d", index);
      ASSERT(0);
      return route_scope;
    }

    /*-----------------------------------------------------------------------
      Loop over all regiestered ACLs to find the best match
    -----------------------------------------------------------------------*/
    for (acl_ptr = *acl_list_ptr; acl_ptr != NULL; acl_ptr = acl_ptr->next_ptr)
    {
      /*---------------------------------------------------------------------
        Skip the ACL if it is temporarily disabled
      ---------------------------------------------------------------------*/
      if (acl_ptr->disabled)
      {
        continue;
      }

      if_ptr = ACL_IF_PTR(acl_ptr);

      /*---------------------------------------------------------------------
        Skip the ACL if the associated iface is not UP or routeable.
      ---------------------------------------------------------------------*/
      if (ps_iface_state(if_ptr) != IFACE_UP &&
          ps_iface_state(if_ptr) != IFACE_ROUTEABLE &&
          ps_iface_state(if_ptr) != IFACE_CONFIGURING)
      {
        continue;
      }

      /*---------------------------------------------------------------------
        Find the current ACLs best match
      ---------------------------------------------------------------------*/
      temp = (acl_ptr->acl_fptr)( if_ptr, acl_pi_ptr, ip_pkt_ptr );

      if (temp > ACL_DEFAULT_CLASS)
      {
        if (ps_route_scope_by_iface_handle(if_ptr, &if_route_scope) == TRUE)
        {
          route_scope = ps_route_scope_join( route_scope, if_route_scope );
        }
      }
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return (route_scope);

} /* ps_route_scope_by_policy() */

/*===========================================================================
FUNCTION PS_ROUTE_LO_UPDATE()

DESCRIPTION
  This function is used to update the loopback routing table. It is
  called whenever and address changes on an interface, either when bringing
  it up initially or during an address change on handoff.

DEPENDENCIES
  Currently we support only one IPv4 address per IPv4 interface. For IPv6 we
  support only one prefix and IID per IPv6 interface. When support for
  RFC3041 is added multiple IID support will need to be added.


PARAMETERS
  this_iface_ptr - Pointer to the interface
  ip_addr_ptr    - Pointer to the IP address

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_route_lo_update
(
  ps_iface_type*       this_iface_ptr,
  ps_ip_addr_type*     ip_addr_ptr
)
{
  int                  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (!PS_IFACE_IS_VALID(this_iface_ptr)) || (ip_addr_ptr == NULL) )
  {
    LOG_MSG_ERROR_2("ps_route_lo_update(): "
                    "Invalid iface ptr 0x%p or ip addr ptr 0x%p",
                    this_iface_ptr, ip_addr_ptr);
    ASSERT(0);
    return;
  }

  if( (ps_iface_addr_family_is_v4(this_iface_ptr) &&
      (ip_addr_ptr->type != IPV4_ADDR))
#ifdef FEATURE_DATA_PS_IPV6
     || (ps_iface_addr_family_is_v6(this_iface_ptr) &&
        (ip_addr_ptr->type != IPV6_ADDR))
#endif /* FEATURE_DATA_PS_IPV6 */
    )
  {
    LOG_MSG_ERROR_2("ps_route_lo_update(): "
                    "Mismatched iface 0x%p and address type %d",
                    this_iface_ptr, ip_addr_ptr->type);
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop until an empty or matching routing entry is found. If the interface
    matches one currently in the table replace it with the new address(s), as
    a handoff may have occurred and the address changed.
  -------------------------------------------------------------------------*/
  switch(ip_addr_ptr->type)
  {
    case IPV4_ADDR:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

      for(index=0; index < (MAX_IPV4_LO_IFACES - 1); index++)
      {
        if( (global_v4_lo_route_array[index].iface_ptr == NULL) ||
            (global_v4_lo_route_array[index].iface_ptr == this_iface_ptr) )
        {
          global_v4_lo_route_array[index].iface_ptr = this_iface_ptr;
          global_v4_lo_route_array[index].ip_addr   = ip_addr_ptr->addr.v4;
          break;
        }
      }

      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

#ifdef FEATURE_DATA_PS_IPV6
    case IPV6_ADDR:
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

      for(index=0; index < (MAX_IPV6_LO_IFACES - 1); index++)
      {
        if( (global_v6_lo_route_array[index].iface_ptr == NULL) ||
            (global_v6_lo_route_array[index].iface_ptr == this_iface_ptr))
        {
          /*-----------------------------------------------------------------
            Only store iface ptrs for UP ifaces. This is less maintenance
            then storing a list of addresses for all privacy/public, etc. and
            the lookup function will be just as fast.
          -----------------------------------------------------------------*/
          global_v6_lo_route_array[index].iface_ptr = this_iface_ptr;
          break;
        }
      }

      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      LOG_MSG_ERROR_1("ps_route_lo_update(): "
                      "LO routing table update failed: invalid address type: %d",
                      ip_addr_ptr->type);
      break;
  }

  LOG_MSG_INFO2_2("ps_route_lo_update(): "
                  "LO routing table updated w/iface 0x%x:%d",
                  this_iface_ptr->name, this_iface_ptr->instance);
  return;

} /* ps_route_lo_update() */

/*===========================================================================
FUNCTION PS_ROUTE_LO_DELETE()

DESCRIPTION
  This function is used to clear out an interface's loopback entries when it
  is torn down.

DEPENDENCIES

PARAMETERS
  this_iface_ptr - Pointer to the interface

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_route_lo_delete
(
  ps_iface_type*       this_iface_ptr
)
{
  int                  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!PS_IFACE_IS_VALID(this_iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_route_lo_delete(): "
                    "Invalid iface ptr 0x%p", this_iface_ptr);
    ASSERT(0);
    return;
  }
  /*------------------------------------------------------------------------
    Find an interface's loopback routing entry and delete it. Move all
    following entries up.
  ------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v4(this_iface_ptr))
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    for(index=0; index < (MAX_IPV4_LO_IFACES - 1); index++)
    {
      if(global_v4_lo_route_array[index].iface_ptr == this_iface_ptr)
      {
        /*-----------------------------------------------------------------
          Matched an entry. If more entries exist pack the array first by
          moving each one up. Otherwise just clear the entry.
        -----------------------------------------------------------------*/
        while( (index < (MAX_IPV4_LO_IFACES - 1)) &&
               (global_v4_lo_route_array[index+1].iface_ptr != NULL))
        {
          memscpy(&global_v4_lo_route_array[index],
                  sizeof(ps_lo_v4_route_type),
                 &global_v4_lo_route_array[index+1],
                 sizeof(ps_lo_v4_route_type));

          index++;
        }
        memset(&global_v4_lo_route_array[index],
               0,
               sizeof(ps_lo_v4_route_type));

        LOG_MSG_INFO2_1("ps_route_lo_delete(): "
                        "LO routing entry cleared for iface 0x%p",
                        this_iface_ptr);
        break;
      }
    } /* end for */

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(ps_iface_addr_family_is_v6(this_iface_ptr))
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

    for(index=0; index < (MAX_IPV6_LO_IFACES - 1); index++)
    {
      if(global_v6_lo_route_array[index].iface_ptr == this_iface_ptr)
      {
        /*-----------------------------------------------------------------
          Matched an entry. If more entries exist pack the array first by
          moving each one up. Otherwise just clear the entry.
        -----------------------------------------------------------------*/
        while( (index < (MAX_IPV6_LO_IFACES - 1)) &&
               (global_v6_lo_route_array[index+1].iface_ptr != NULL))
        {
          memscpy(&global_v6_lo_route_array[index],
                  sizeof(ps_lo_v6_route_type),
                 &global_v6_lo_route_array[index+1],
                 sizeof(ps_lo_v6_route_type));
          index++;
        }
        memset(&global_v6_lo_route_array[index],
               0,
               sizeof(ps_lo_v6_route_type));

        LOG_MSG_INFO2_2("ps_route_lo_delete(): "
                        "LO routing entry cleared for iface 0x%x:%d",
                        this_iface_ptr->name, this_iface_ptr->instance);
        break;
      }
    } /* end for */

    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  return;
} /* ps_route_lo_delete() */

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
  if_ptr             - Ptr local interface route goes through
  app_prio_ptr point - Ptr to priority of selected Iface

RETURN VALUE
  Iface ptr : NULL if none found

SIDE EFFECTS
  None
===========================================================================*/
ps_iface_type * ps_route_look_up_iface_by_policy
(
  acl_policy_info_type          * acl_pi_ptr,
  boolean                       * arb_candidate,
  uint32                        * iface_priority_mask,
  ps_iface_net_down_reason_type * net_down_reason
)
{
#define ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE  8

  ps_iface_type   * deny_iface_in_use_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type   * deny_iface_in_use_cfg_match_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type   * logical_iface_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type  ** deny_list_ptr = _route_deny_list;
  unsigned int      deny_list_size = sizeof(_route_deny_list)/sizeof(_route_deny_list[0]);
  uint8             num_iface_in_use = 0;
  uint8             num_iface_in_use_cfg_match = 0;
  uint8             num_logical_iface          = 0;
  ps_route_lookup_params_type lookup_params;
  ps_iface_type  *if_ptr;
  int             result;
  ps_iface_type  ** arb_candidate_iface_arr_ptr = 0;
  uint8             num_arb_candidate_iface = 0;
  boolean           dun_flag = FALSE;
  ps_iface_type    *tmp_if_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ps_routei_acl_policy_trace_dump(acl_pi_ptr,
                                  "ps_route_look_up_iface_by_policy()");

  if_ptr = NULL;

  /* DUN call detection hack, do something better later */
  if( acl_pi_ptr->proc_id == 1 )
  {
    dun_flag = TRUE;
  }

  if(acl_pi_ptr->subs_id < PS_SYS_DEFAULT_SUBS ||
     acl_pi_ptr->subs_id >= PS_SYS_SUBS_MAX)
  {
    LOG_MSG_ERROR_1("ps_route_look_up_iface_by_policy(): "
                    "Invalid subs  %d", acl_pi_ptr->subs_id);
    ASSERT(0);
    return NULL;
  }

  if(acl_pi_ptr->subs_id == PS_SYS_DEFAULT_SUBS)
  {
    acl_pi_ptr->subs_id = ps_sys_get_default_data_subscription();
  }

  LOG_MSG_INFO1_1("ps_route_look_up_iface_by_policy: subs %d", 
                  acl_pi_ptr->subs_id);
  /*-----------------------------------------------------------------------
  Initialize policy routing search results.
  -----------------------------------------------------------------------*/
  acl_pi_ptr->rt_result.priority = -1;
  acl_pi_ptr->rt_result.if_hndl  = -1;
  acl_pi_ptr->rt_result.rm_hndl  = -1;

  /* Prep and fill lookup params */
  memset( &lookup_params, 0, sizeof(ps_route_lookup_params_type) );
  lookup_params.ip_pkt_ptr    = NULL;
  lookup_params.acl_pi_ptr    = acl_pi_ptr;
  lookup_params.gateway_ptr   = NULL;
  lookup_params.deny_list_ptr = deny_list_ptr;
  lookup_params.deny_list_arr_size        = (uint8)deny_list_size;
  lookup_params.deny_iface_in_use_arr_ptr = deny_iface_in_use_arr_ptr;
  lookup_params.iface_in_use_arr_size     = ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE;
  lookup_params.num_iface_in_use_ptr      = &num_iface_in_use;

  /* array of PS Iface pointers IN USE CFG MATCH */
  lookup_params.deny_iface_in_use_cfg_match_arr_ptr = deny_iface_in_use_cfg_match_arr_ptr;
  lookup_params.iface_in_use_cfg_match_arr_size     = ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE;
  lookup_params.num_iface_in_use_cfg_match_ptr      = &num_iface_in_use_cfg_match;

  lookup_params.if_ptr                    = &if_ptr;

  /* irrespective of result, if num_iface_in_use_cfg_match > 0 arbitration is needed */
  result = ps_route_network_selection_by_policy( &lookup_params, net_down_reason );

  LOG_MSG_INFO1_3("ps_route_look_up_iface_by_policy(): "
                  "lookup: selection done, in_use_cfg_match [%d], in_use [%d], Iface available? [%d]",
                  num_iface_in_use_cfg_match, num_iface_in_use, !result );

  /*--------------------------------------------------------------------------
    Give higher arbitration preference to list of Ifaces IN_USE_CFG_MATCH
    then check whether any Iface was returned, then arbitrate among IN_USE.
  --------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  if( num_iface_in_use_cfg_match > 0 )
  {
    LOG_MSG_INFO1_2("ps_route_look_up_iface_by_policy(): "
                    "lookup: in_use_cfg_match > 0, ARBITRATE! in_use_cfg_match [%d], in_use [%d]",
                    num_iface_in_use_cfg_match, num_iface_in_use );

    if(num_iface_in_use_cfg_match > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return NULL;
    }

    if ( ps_iface_get_root_iface( logical_iface_arr_ptr,
                                 &num_logical_iface,
                                  deny_iface_in_use_cfg_match_arr_ptr,
                                  num_iface_in_use_cfg_match ) != 0 )
    {
      /* failed, use phys Ifaces */
      LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                      "lookup: failed to find logical, use physical" );
      arb_candidate_iface_arr_ptr = deny_iface_in_use_cfg_match_arr_ptr;
      num_arb_candidate_iface     = num_iface_in_use_cfg_match;
    }
    else
    {
      /* found logical, arbitrate on logical Ifaces */
      LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                      "lookup: found logical, arbitrate on logical" );
      arb_candidate_iface_arr_ptr = logical_iface_arr_ptr;
      num_arb_candidate_iface     = num_logical_iface;
    }

    *arb_candidate = TRUE;

    if(num_arb_candidate_iface > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return NULL;
    }

    if_ptr = ps_arbitration_mgr_net_arbitrate_lookup( acl_pi_ptr,
                                                      arb_candidate_iface_arr_ptr,
                                                      num_arb_candidate_iface );

    /* After arbitration is done, find assoc physical Iface */
    if( dun_flag )
    {
      /* get associated physical Iface */
      tmp_if_ptr = PS_IFACE_GET_ASSOC_IFACE( if_ptr );
      if( tmp_if_ptr != NULL )
      {
        if_ptr = tmp_if_ptr;
        LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                        "lookup: TE-DUN, found physical" );
      }
    }

  }
  else /* Iface is available, in_use_cfg_match list is empty */
  if( result == 0 )
  {
    LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                    "lookup: Iface available, ARBITRATION NOT NEEDED!" );
    *arb_candidate = FALSE;
  }
  else /* Iface is not available, in_use_cfg_match list is empty */
  if ( result == -1 && num_iface_in_use > 0 )
  {
    MSG_HIGH("ps_route_look_up_iface_by_policy(): "
             "lookup: in_use > 0, ARBITRATE!, in_use [%d]", num_iface_in_use, 0, 0 );
    if(num_iface_in_use > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return NULL;
    }
    if ( ps_iface_get_root_iface( logical_iface_arr_ptr,
                                 &num_logical_iface,
                                  deny_iface_in_use_arr_ptr,
                                  num_iface_in_use ) != 0 )
    {
      /* failed, use physical Ifaces */
      LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                      "lookup: failed to find logical, use physical" );
      arb_candidate_iface_arr_ptr = deny_iface_in_use_arr_ptr;
      num_arb_candidate_iface     = num_iface_in_use;
    }
    else
    {
      /* found logical, not a TE-DUN call, return logical Ifaces */
      LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                      "lookup: found logical, arbitrate on logical" );
      arb_candidate_iface_arr_ptr = logical_iface_arr_ptr;
      num_arb_candidate_iface     = num_logical_iface;
    }


    *arb_candidate = TRUE;
     if(num_arb_candidate_iface > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return NULL;
    }
    if_ptr = ps_arbitration_mgr_net_arbitrate_lookup( acl_pi_ptr,
                                                      arb_candidate_iface_arr_ptr,
                                                      num_arb_candidate_iface );

    /* After arbitration is done, find asso physical Iface */
    if( dun_flag )
    {
      /* get associated physical Iface */
      tmp_if_ptr = PS_IFACE_GET_ASSOC_IFACE( if_ptr );
      if( tmp_if_ptr != NULL )
      {
        if_ptr = tmp_if_ptr;
        LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                        "lookup: TE-DUN, found physical" );
      }
    }

  } /* arb block */

  if ( if_ptr == NULL )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_0("ps_route_look_up_iface_by_policy(): "
                    "lookup: failed to find Iface" );
    return NULL;
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  *iface_priority_mask = PS_IFACEI_GET_PRIORITY_MASK( if_ptr );

  MSG_HIGH("ps_route_look_up_iface_by_policy(): "
           "lookup: done! Iface [0x%x:%x] available, prio mask [%x]",
            if_ptr->name, if_ptr->instance, *iface_priority_mask );
  return if_ptr;

} /* ps_route_lookup_up_iface_by_policy() */


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
)
{
  ps_iface_net_down_reason_type net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return (ps_route_bring_up_iface_by_policy_ex(acl_pi_ptr,
                                               if_ptr,
                                               ps_errno,
                                               NULL,
                                               &net_down_reason) );
}

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
  acl_pi_ptr                    - Ptr to packet's policy information
  if_ptr                        - Ptr local interface route goes through
  ps_errno                      - Ptr to dss errno value
  client_data_ptr               - pointer to client data passed in by client
  ps_iface_net_down_reason_type - pointer to net down reason

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
)
{
  int32             app_priority = PS_POLICY_MGR_PRIORITY_INVALID;
  ps_iface_type   * deny_iface_in_use_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type   * deny_iface_in_use_cfg_match_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type   * logical_iface_arr_ptr[ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE] = {0,};
  ps_iface_type  ** deny_list_ptr = _route_deny_list;
  unsigned int      deny_list_size = sizeof(_route_deny_list)/sizeof(_route_deny_list[0]);
  uint8             num_iface_in_use = 0;
  uint8             num_iface_in_use_cfg_match = 0;
  uint8             num_logical_iface          = 0;
  ps_route_lookup_params_type lookup_params;
  int               result;
  boolean           arb_flg = FALSE;
  ps_iface_type  ** arb_candidate_iface_arr_ptr;
  uint8             num_arb_candidate_iface;
  boolean           dun_flag = FALSE;
  ps_iface_type    *tmp_if_ptr;
  uint32            call_state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_errno == NULL)
  {
    DATA_ERR_FATAL("ps_route_bring_up_iface_by_policy_ex(): NULL ps_errno");
    return -1;
  }

  if (if_ptr == NULL || acl_pi_ptr == NULL)
  {
    *ps_errno = DS_EINVAL;
    DATA_ERR_FATAL("ps_route_bring_up_iface_by_policy_ex(): NULL args");
    return -1;
  }

  if (acl_pi_ptr->bring_up != TRUE)
  {
    *ps_errno = DS_EINVAL;
    DATA_ERR_FATAL("ps_route_bring_up_iface_by_policy_ex(): bring_up flag is not set");
    return -1;
  }

  if (net_down_reason == NULL)
  {
    DATA_ERR_FATAL("ps_route_bring_up_iface_by_policy_ex(): NULL net_down_reason");
    return -1;
  }

  if(acl_pi_ptr->subs_id < PS_SYS_DEFAULT_SUBS ||
     acl_pi_ptr->subs_id >= PS_SYS_SUBS_MAX)
  {
    DATA_ERR_FATAL("ps_route_bring_up_iface_by_policy_ex(): Invalid subs");
    return -1;
  }

  if(acl_pi_ptr->subs_id == PS_SYS_DEFAULT_SUBS)
  {
    acl_pi_ptr->subs_id = ps_sys_get_default_data_subscription();
  }

  LOG_MSG_INFO1_1("ps_route_bring_up_iface_by_policy_ex: subs %d", 
                   acl_pi_ptr->subs_id);

  ps_routei_acl_policy_trace_dump(acl_pi_ptr,
                                 "ps_route_bring_up_iface_by_policy_ex()");

  *if_ptr = NULL;

  /* DUN call detection hack, do something better later */
  if( acl_pi_ptr->proc_id == 1 )
  {
    dun_flag = TRUE;
  }

  /*-----------------------------------------------------------------------
    Initialize policy routing search results.
  -----------------------------------------------------------------------*/
  acl_pi_ptr->rt_result.priority = -1;
  acl_pi_ptr->rt_result.if_hndl  = -1;
  acl_pi_ptr->rt_result.rm_hndl  = -1;

  memset( &lookup_params, 0, sizeof(ps_route_lookup_params_type) );
  lookup_params.ip_pkt_ptr = NULL;
  lookup_params.acl_pi_ptr = acl_pi_ptr;
  lookup_params.gateway_ptr = NULL;
  lookup_params.deny_list_ptr = deny_list_ptr;
  lookup_params.deny_list_arr_size = (uint8)deny_list_size;
  lookup_params.deny_iface_in_use_arr_ptr = deny_iface_in_use_arr_ptr;
  lookup_params.iface_in_use_arr_size = ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE;
  lookup_params.num_iface_in_use_ptr = &num_iface_in_use;

  /* array of PS Iface pointers IN USE CFG MATCH */
  lookup_params.deny_iface_in_use_cfg_match_arr_ptr = deny_iface_in_use_cfg_match_arr_ptr;
  lookup_params.iface_in_use_cfg_match_arr_size     = ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE;
  lookup_params.num_iface_in_use_cfg_match_ptr      = &num_iface_in_use_cfg_match;

  lookup_params.if_ptr = if_ptr;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  result = ps_route_network_selection_by_policy( &lookup_params, net_down_reason );

  LOG_MSG_INFO1_3("ps_route_bring_up_iface_by_policy_ex(): "
                  "bringup: selection done, in_use_cfg_match [%d], "
                  "in_use [%d], Iface available? [%d]",
                  num_iface_in_use_cfg_match, num_iface_in_use, !result );

  if (result == 0)
  {
    LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                    "bringup: Iface available, ARBITRATION NOT NEEDED!" );
    arb_flg = FALSE;
  }
  else if (num_iface_in_use_cfg_match > 0)
  {
    LOG_MSG_INFO1_2("ps_route_bring_up_iface_by_policy_ex(): "
                    "bringup: in_use_cfg_match > 0, ARBITRATE! "
                    "in_use_cfg_match [%d], in_use [%d]",
                    num_iface_in_use_cfg_match, num_iface_in_use );

    if(num_iface_in_use_cfg_match > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return -1;
    }
    if ( ps_iface_get_root_iface(  logical_iface_arr_ptr,
                                   &num_logical_iface,
                                   deny_iface_in_use_cfg_match_arr_ptr,
                                   num_iface_in_use_cfg_match ) != 0 )
    {
      /* failed, use phys Iface */
      LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                      "lookup: failed to find logical, use physical" );
      arb_candidate_iface_arr_ptr = deny_iface_in_use_cfg_match_arr_ptr;
      num_arb_candidate_iface     = num_iface_in_use_cfg_match;
    }
    else
    {
      /* found logical, not TE-DUN return logical Ifaces */
      LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                      "lookup: found logical, arbitrate on logical" );
      arb_candidate_iface_arr_ptr = logical_iface_arr_ptr;
      num_arb_candidate_iface     = num_logical_iface;
    }

    arb_flg = TRUE;

    if(num_arb_candidate_iface > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return -1;
    }
    *if_ptr = ps_arbitration_mgr_net_arbitrate( acl_pi_ptr,
                                                arb_candidate_iface_arr_ptr,
                                                num_arb_candidate_iface );

    /* After arbitration is done, find asso physical Iface */
    if( dun_flag )
    {
      /* get associated physical Iface */
      tmp_if_ptr = PS_IFACE_GET_ASSOC_IFACE( *if_ptr );
      if( tmp_if_ptr != NULL )
      {
        *if_ptr = tmp_if_ptr;
        LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                        "lookup: TE-DUN, found physical" );
      }
    }

  }
  else if (result == -1 && num_iface_in_use > 0)
  {
    LOG_MSG_INFO1_1("ps_route_bring_up_iface_by_policy_ex(): "
                    "bringup: in_use > 0, ARBITRATE!, in_use [%d]",
                    num_iface_in_use );
    if(num_iface_in_use > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return -1;
    }
    if ( ps_iface_get_root_iface(  logical_iface_arr_ptr,
                                  &num_logical_iface,
                                   deny_iface_in_use_arr_ptr,
                                   num_iface_in_use ) != 0 )
    {
      /* failed, use phys Iface */
      LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                      "lookup: failed to find logical, use physical" );
      arb_candidate_iface_arr_ptr = deny_iface_in_use_arr_ptr;
      num_arb_candidate_iface     = num_iface_in_use;
    }
    else
    {
      /* found logical, not TE-DUN return logical Ifaces */
      LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                      "lookup: found logical, NOT TE-DUN, use logical" );
      arb_candidate_iface_arr_ptr = logical_iface_arr_ptr;
      num_arb_candidate_iface     = num_logical_iface;
    }

    arb_flg = TRUE;

     if(num_arb_candidate_iface > ROUTEI_DENY_IFACE_IN_USE_ARR_SIZE)
    {
       PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
       return -1;
    }
    *if_ptr = ps_arbitration_mgr_net_arbitrate( acl_pi_ptr,
                                                arb_candidate_iface_arr_ptr,
                                                num_arb_candidate_iface );

    /* After arbitration is done, find asso physical Iface */
    if( dun_flag )
    {
      /* get associated physical Iface */
      tmp_if_ptr = PS_IFACE_GET_ASSOC_IFACE( *if_ptr );
      if( tmp_if_ptr != NULL )
      {
        *if_ptr = tmp_if_ptr;
        LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                        "lookup: TE-DUN, found physical" );
      }
    }

  } /* arb block */

  if ( *if_ptr == NULL )
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    LOG_MSG_INFO1_0("ps_route_bring_up_iface_by_policy_ex(): "
                    "bringup: failed to find Iface" );
    *ps_errno = DS_ENETNONET;
    return -1;
  }

  app_priority = ps_policy_mgr_get_priority
                 (
                   acl_pi_ptr->app_identifier,
                   acl_pi_ptr->data_session_profile_id
                 );

  LOG_MSG_INFO1_2("ps_route_bring_up_iface_by_policy_ex(): "
                  "PS Iface bringup using prio [%d], profile [%d]",
                  app_priority, acl_pi_ptr->data_session_profile_id );

  /*------------------------------------------------------------------------
    Try to bringup the same Iface again only if Arb didn't happen else
    return -1 with the obvious ps code DS_ENETCLOSEINPROGRESS
  ------------------------------------------------------------------------*/
  if( !arb_flg )
  {
    result = ps_iface_bring_up_cmd_ex(*if_ptr,
                                      app_priority,
                                      ps_errno,
                                      client_data_ptr);
    LOG_MSG_INFO1_2("ps_route_bring_up_iface_by_policy_ex(): "
                    "PS Iface bringup no arb, result [%d], errno [%d]",
                    result, *ps_errno );
  }
  else
  {
    result = -1;
    *ps_errno = DS_ENETCLOSEINPROGRESS;
    LOG_MSG_INFO1_2("ps_route_bring_up_iface_by_policy_ex(): "
                    "PS Iface bringup after arb, result [%d], errno [%d]",
                    result, *ps_errno );
  }
  /*-------------------------------------------------------------------------
                   Logging events only on EPC_IFACE
  -------------------------------------------------------------------------*/
  if ((*if_ptr)->name == EPC_IFACE)
  {
    /*------------------------------------------------------------------------
      If result is 0, interface is already up. Else if result is -1 and
      err is DS_EWOULDBLOCK, then the interface is coming up.
      The call is in activating state.
    ------------------------------------------------------------------------*/
    if (( result == -1 && *ps_errno == DS_EWOULDBLOCK ) ||
         (result == 0))
    {
      if (result == 0)
      {
        call_state = PS_DATA_CALL_INFO_LOGGING_STATUS_ACTIVATED;
      }
      else
      {
        call_state = PS_DATA_CALL_INFO_LOGGING_STATUS_ACTIVATING;
      }
    
      LOG_MSG_INFO2_0("ps_route_bring_up_iface_by_policy_ex(): "
                      " Call status info ");

      /*---------------------------------------------------------------------
        Generate DIAG events with call status information
      ---------------------------------------------------------------------*/
      ps_data_call_info_logging_event_report(*if_ptr,
                                        call_state, 
                                        acl_pi_ptr->proc_id);
    }
  }

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return result;

} /* ps_route_bring_up_iface_by_policy() */

/*===========================================================================

FUNCTION PS_ROUTE_TEAR_DOWN_IFACE_BY_POLICY()

DESCRIPTION
  This function is used to tear down an iface.

PARAMETERS
  this_iface_prtr: ptr to interface control block on which to operate on.
  acl_pi_ptr: ptr to the policy info
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
  ps_iface_type             *this_iface_ptr,
  acl_policy_info_type      *acl_pi_ptr,
  int16                     *ps_errno,
  void                      *client_data_ptr
)
{
  int                         result;
  uint32                      call_type;
  int                         profile_id;
  int32                       app_priority;
  uint64                      app_id = 0ULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO2_0("ps_route_tear_down_iface_by_policy(): "
                  "Iface tear down cmd" );
  /*-----------------------------------------------------------------------
    Find the type of call based on the proc_id passed; If caller passes
    NULL policy use proc_id from iface, else use proc_id in the policy
  -----------------------------------------------------------------------*/
  if (acl_pi_ptr == NULL)
  {
     call_type  = PS_IFACE_GET_PROC_ID(this_iface_ptr);
     /*---------------------------------------------------------------------
      Get app id from proc id and then priority associated with app id
     ---------------------------------------------------------------------*/
     if ( !routei_get_app_id_from_proc_id(&app_id, 
                                   PS_IFACE_GET_PROC_ID(this_iface_ptr)) )
     {
       app_id = PS_DATA_CALL_INFO_LOGGING_APP_ID_APPS;
     }
     profile_id = PS_POLICY_MGR_ANY_PROFILE;
  }
  else
  {
     call_type  = acl_pi_ptr->proc_id;
     app_id     = acl_pi_ptr->app_identifier;
     profile_id = acl_pi_ptr->data_session_profile_id;
  }

  app_priority = ps_policy_mgr_get_priority( app_id, profile_id);

  LOG_MSG_INFO1_2("ps_route_tear_down_iface_by_policy(): "
                  "App prio [%d], profile [%d]", app_priority, profile_id );

  result = ps_iface_tear_down_cmd_ex(this_iface_ptr,
                                     app_priority,
                                     ps_errno,
                                     client_data_ptr);

  /*-------------------------------------------------------------------------
    If the interface is still up on the EPC_IFACE, update call info
   ------------------------------------------------------------------------*/                                          
  if ((result == 0) && (this_iface_ptr->name == EPC_IFACE))
  {
    LOG_MSG_INFO2_0("ps_route_tear_down_iface_by_policy(): "
                    " Call status info ");

    /*-----------------------------------------------------------------------
      Generate DIAG events with call status information
    -----------------------------------------------------------------------*/
    ps_data_call_info_logging_event_report(this_iface_ptr, 
                                 PS_DATA_CALL_INFO_LOGGING_STATUS_TERMINATED,
                                 call_type);
 }

  return result;
} /* ps_route_tear_down_iface_by_policy */

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
  acl_priority_cb  - Routing ACL priority search callback. If NULL callback
                     is used, registered routing handler gets disabled.

RETURN VALUE
  Previously registered callback pointer if any, NULL otherwise.

SIDE EFFECTS
  None
===========================================================================*/
route_acl_priority_cb_func ps_route_register_acl_policy_routing_func
(
  acl_proc_id_type            proc_id,
  route_acl_priority_cb_func  acl_priority_cb
)
{
  route_acl_priority_cb_func result = NULL;
  acl_proc_id_type temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  LOG_MSG_INFO2_2("ps_route_register_acl_policy_routing_func(): "
                  "Register ACL policy routing func: proc=%d cb=0x%p",
                  proc_id, acl_priority_cb);

  temp = (ACL_PROC_MAX-1);

  /* Validate processor ID; ensure local processor callback not overwritten. */
  /*lint -e{685} */
  if( proc_id >= temp )
  {
    DATA_ERR_FATAL("ps_route_register_acl_policy_routing_func(): invalid args");
    return NULL;
  }

  /* Update callback table, return previous value to caller. */
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  result = _acl_priority_cb_tbl[ proc_id ];
  _acl_priority_cb_tbl[ proc_id ] = acl_priority_cb;
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return result;
} /* ps_route_register_acl_policy_routing_func() */

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
)
{
#if 0
//TODO pkt info should have src addr if it is bound to an address alreadey
//that address should be used to check v6 scope
//If scope is set by app, pkt->if_ptr is set. Run ACL only if if_ptr matches
//ACL's if_ptr
//Remove rscope checks as well - basically we r making sure that app's scope
//matches the policy's scope and that can be done by skippig ACL's whivh
//doesnt match if_ptr
#endif
  acl_type                      * acl_ptr = NULL;
  acl_type                      * acl_match_ptr = NULL;
  ps_iface_type                 * ps_iface_ptr;
  ps_iface_type                ** deny_list_ptr = _route_deny_list;
  ps_route_lookup_params_type     lookup_params;
  unsigned int                    deny_list_size;
  int                             best_priority = ACL_DEFAULT_CLASS;
  boolean                         acl_lookup_failed = FALSE;
  acl_rt_result_type              rt_result;
  uint32                          index;
  int32                           temp = (int32) ACL_DEFAULT_CLASS;
  int                             deny_reason = ACL_DENY_REASON_POL_NO_MATCH;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* init lookup params */
  memset( &lookup_params, 0, sizeof(ps_route_lookup_params_type) );

  *if_ptr        = NULL;
  deny_list_size = sizeof(_route_deny_list) / sizeof(_route_deny_list[0]);

  /*------------------------------------------------------------------------
    If address is a V6 address it could be a V4 MAPPED V6 addr. If it is
    convert it to it's native V4 format.
  ------------------------------------------------------------------------*/
  if( ip_pkt_ptr && validate_ip_pkt_info(ip_pkt_ptr, acl_pi_ptr, if_ptr) != 0 )
  {
    LOG_MSG_INFO1_0("ps_route_data_path_lookup(): "
                    "Route lookup failed");
    return -1;
  }

  if(*if_ptr != NULL)
  {
    return 0;
  }

  if(acl_pi_ptr == NULL)
  {
    if (ip_pkt_ptr == NULL)
    {
      return -1;
    }

    /*-----------------------------------------------------------------
      We dont have any acl to run hence return the iface wanted by caller
    -----------------------------------------------------------------*/
    *if_ptr = ip_pkt_ptr->if_ptr;
    return 0;
  }

  if(acl_pi_ptr->subs_id < PS_SYS_DEFAULT_SUBS ||
     acl_pi_ptr->subs_id >= PS_SYS_SUBS_MAX)
  {
    LOG_MSG_ERROR_1("ps_route_data_path_lookup(): "
                    "Invalid subs  %d", acl_pi_ptr->subs_id);
    return -1;
  }

  if(acl_pi_ptr->subs_id == PS_SYS_DEFAULT_SUBS)
  {
    acl_pi_ptr->subs_id = ps_sys_get_default_data_subscription();
  }

  ps_routei_acl_policy_trace_dump(acl_pi_ptr,
                                  "ps_route_data_path_lookup()");

  *if_ptr = NULL;

  if (!addr_based_routing_only)
  {
    if (acl_pi_ptr->iface.kind == DSS_IFACE_ID)
    {
      ps_iface_ptr = ps_iface_get_handle( acl_pi_ptr->iface.info.id);
      if (ps_iface_ptr == NULL)
      {
        LOG_MSG_INFO1_1("ps_route_data_path_lookup(): "
                        "ps_iface_get_handle failed, handle %d",
                        	 acl_pi_ptr->iface.info.id);
        return -1;
      }

      temp = ps_iface_ptr->outgoing_acl_ptr->acl_fptr( ps_iface_ptr,
                                                       acl_pi_ptr,
                                                       ip_pkt_ptr);
      if (temp > (int32) ACL_DEFAULT_CLASS)
      {
        best_priority = acl_pi_ptr->rt_result.priority = temp;
        acl_match_ptr = ps_iface_ptr->outgoing_acl_ptr;
      }
    }
    else
    {
      /*---------------------------------------------------------------------
        Loop over callback table for all processors, find best IFACE overall.
        Note callback table to search is ordered to do remote processors first
        then the local processor, so the proxy IFACE routing data is
        available.
      ---------------------------------------------------------------------*/
      for (index = 0; index < (uint32) ACL_PROC_MAX; index++)
      {
        /*-------------------------------------------------------------------
          Invoke ACl search callback if registered for processor.
        -------------------------------------------------------------------*/
        if (_acl_priority_cb_tbl[ index ])
        {
          /*-----------------------------------------------------------------
            Reset information so that stale information from attached modem is
            not used if local routing fails - CR 337479
          -----------------------------------------------------------------*/
          memset((void *) &rt_result, 0x0, sizeof(rt_result));
          acl_match_ptr = NULL;

          lookup_params.ip_pkt_ptr                = ip_pkt_ptr;
          lookup_params.acl_pi_ptr                = acl_pi_ptr;
          lookup_params.gateway_ptr               = NULL;
          lookup_params.deny_list_ptr             = deny_list_ptr;
          lookup_params.deny_list_arr_size        = (uint8)deny_list_size;
          lookup_params.deny_iface_in_use_arr_ptr = NULL;
          lookup_params.iface_in_use_arr_size     = 0;
          lookup_params.num_iface_in_use_ptr      = NULL;
          lookup_params.if_ptr                    = NULL;

          if (-1 == _acl_priority_cb_tbl[ index ]( &lookup_params,
                                                   &acl_ptr,
                                                   &rt_result,
                                                   &deny_reason) )
          {
            LOG_MSG_ERROR_1("ps_route_data_path_lookup(): "
                            "Routing search failed on processor:%d", index);
          }
          else
          {
            /*---------------------------------------------------------------
              Check for priority match results.
            ---------------------------------------------------------------*/
            if (rt_result.priority == ACL_DEFAULT_CLASS)
            {
              continue;
            }

            /*---------------------------------------------------------------
              Check if the current ACL's match (priority) is better than what
              was already found. In case of tie give preference to the latter
              match.  This ensures the local processor IFACE in
              multi-processor case is selected.
            ---------------------------------------------------------------*/
            else if (rt_result.priority >= (int)acl_pi_ptr->rt_result.priority)
            {
              acl_match_ptr = acl_ptr;
              best_priority = rt_result.priority;

              /*-------------------------------------------------------------
                Update policy with best IFACE selection
              -------------------------------------------------------------*/
              PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
              acl_pi_ptr->rt_result = rt_result;
              PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
            }
          }
        }
      }
    }


    /*-----------------------------------------------------------------------
      Check if we found a matching route from the ACLs
    -----------------------------------------------------------------------*/
    if (acl_match_ptr != NULL && best_priority >= 0)
    {
      LOG_MSG_INFO2_3("ps_route_data_path_lookup(): "
                      "RT: Found ACL match on iface 0x%x:%d with priority %d",
                      ACL_IF_PTR(acl_match_ptr)->name,
                      ACL_IF_PTR(acl_match_ptr)->instance,
                      best_priority);

      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      *if_ptr = ACL_IF_PTR( acl_match_ptr);
      acl_match_ptr->pkt_count++;
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    }
    else
    {
      acl_lookup_failed = TRUE;
    }

  }
  if ((addr_based_routing_only || acl_lookup_failed) && ip_pkt_ptr != NULL)
  {
    if (ip_pkt_ptr->ip_vsn == IP_V4)
    {
      /*---------------------------------------------------------------------
        Perform IPv4 address based routing. Used when:
        1. When address based routing is requested
        2. ACL lookups fail to match an interface

        Doesn't matter if function returns 0/-1 if iface_ptr is populated
        the lookup was successful.
      ---------------------------------------------------------------------*/
      (void) ps_route_by_ipv4_address(ip_pkt_ptr,
                                      NULL,
                                      deny_list_ptr,
                                      if_ptr);
    }
  }

  /*-------------------------------------------------------------------------
    Check the outcome of the lookup
  -------------------------------------------------------------------------*/
  if (*if_ptr == NULL)
  {
    LOG_MSG_INFO1_0("ps_route_data_path_lookup(): "
                    "ROUTE LOOKUP FAILED");
    return -1;
  }
  else
  {
    return 0;
  }

} /* ps_route_data_path_lookup() */
