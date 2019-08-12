/*===========================================================================

FILE:          PS_ACLRULES.C

OVERVIEW:      This file contains the implementation for various ACL rule
               functions along with additional ACL trace debug messages.

DEPENDENCIES:  None

               Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_aclrules.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when         who    what, where, why
----------   ---    ----------------------------------------------------------
2010-03-08   dm     Creating new file

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "amssassert.h"
#include "ps_aclrules.h"
#include "ps_acli_trace.h"


/*===========================================================================

                            EXTERNAL FUNCTIONS

===========================================================================*/
void acl_trace_pass
(
  uint32 acl_rule_val
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
} /* acl_trace_pass() */

void acl_trace_deny
(
  void   * this_if_ptr,
  int    deny_reason
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( deny_reason != ACL_DENY_REASON_POL_NO_MATCH)
  {
    ACL_TRACE_DUMP(ACL_LEVEL_HIGH, "ACL 0x%x:%d DENIED, Reason %d",         
                    ((ps_iface_type*)this_if_ptr)->name,                    
                    ((ps_iface_type*)this_if_ptr)->instance, deny_reason);
  }
  return;
}/* acl_trace_deny() */

void acl_trace_start
(
  void   * this_if_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
} /* acl_trace_start()  */

void acl_trace_end
(
  void   * this_if_ptr,
  uint32 acl_rule_val
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ACL_TRACE_DUMP(ACL_LEVEL_MED,                                             
               "PASSING Iface 0x%x:%d with priority %d",                    
               ((ps_iface_type*)this_if_ptr)->name,                         
               ((ps_iface_type*)this_if_ptr)->instance,                     
               acl_rule_val);                                                
  return;
} /* acl_trace_end() */

boolean is_pkt_info_null
(
  ip_pkt_info_type *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pkt_info_ptr == NULL)
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "IS_PKT_INFO_NULL: True", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* is_pkt_info_null() */


boolean is_pol_info_null
(
  acl_policy_info_type *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(policy_info_ptr == NULL)
  {

    ACL_TRACE_DUMP(ACL_LEVEL_MED, "IS_POL_INFO_NULL: True", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* is_pol_info_null() */


boolean proc_id_any
(
  acl_policy_info_type *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (policy_info_ptr->proc_id == PS_IFACE_PROC_ID_ANY)   
  {
    ACL_TRACE_DUMP (ACL_LEVEL_MED, "PROC_ID_ANY: True, Proc_id %d ",
                   ((policy_info_ptr)->proc_id), 0, 0);
    return TRUE;
  }

  return FALSE;
} /* proc_id_any() */


boolean iface_id_match
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (!(IFACE_TYPE_IS_ID()))
  {
    return FALSE;
  }

  if ((PS_IFACE_GET_ID((ps_iface_type*) this_if_ptr ) & 0xFF000000 ) ==
         (policy_info_ptr->iface.info.id & 0xFF000000))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                 "IFACE_ID_MATCH: Iface ID 0x%x, pol iface ID (Match) ",
                 policy_info_ptr->iface.info.id & 0xFF000000, 0, 0);
    return TRUE;
  }
  
  return FALSE;
} /* iface_id_match() */


boolean different_proc_id_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (((ps_iface_type *) this_if_ptr)->iface_private.proc_id != 
       PS_IFACE_PROC_ID_NONE && policy_info_ptr->proc_id !=  
       ((ps_iface_type *) this_if_ptr)->iface_private.proc_id)    
  {           
     ACL_TRACE_DUMP (ACL_LEVEL_MED, 
                     "DIFFERENT_PROC_ID_REQUIRED: True Iface proc_id %d"
                     "policy proc_id %d ",
                     ((ps_iface_type *) this_if_ptr)->iface_private.proc_id,
                     policy_info_ptr->proc_id, 0);
     return TRUE;
  }
  
  return FALSE;
  
} /* different_proc_id_required() */


boolean required_iface_type_is
(
  acl_policy_info_type         *policy_info_ptr,
  uint16                        ifname
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( IFACE_TYPE_IS_ID()  )
  {
    if ( PS_IFACE_GET_NAME_FROM_ID(policy_info_ptr->iface.info.id) ==
           (dss_iface_id_type) ifname )
    {
      return TRUE;
    }
  }
  else if ( IFACE_TYPE_IS_NAME()  )
  {
    if (policy_info_ptr->iface.info.name == (dss_iface_name_enum_type) ifname)
    {
      return TRUE;
    }
  }
  else if ( IFACE_TYPE_IS_GROUP(ifname)) 
  {
    if (policy_info_ptr->iface.info.name & (dss_iface_name_enum_type) ifname)
    {
      return TRUE;
    }
  }
  else
  {
      ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "REQUIRED_IFACE_TYPE_IS: Iface Group 0x%x, "
                   "Pol Iface Group 0x%x (No Match)",
                   (dss_iface_name_enum_type) ifname,
                    ((policy_info_ptr)?(policy_info_ptr->iface.info.name):0), 0);
    return FALSE;
  }
  return FALSE;

} /* required_iface_type_is() */


boolean incoming_iface_type_is
(
  ip_pkt_info_type          *pkt_info_ptr,
  uint16                     ifname
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_PKT_INFO( ACL_IPFI_IF_PTR(pkt_info_ptr)->name == ifname ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "INCOMING_IFACE_TYPE_IS: iface name 0x%x Pkt iface name, "
                   "(Match)", ifname, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* incoming_iface_type_is() */


boolean localhost
(
  ip_pkt_info_type    *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_PKT_INFO(PS_IFACE_IP_V4_ADDR( ACL_IPFI_IF_PTR( pkt_info_ptr ))))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "LOCALHOST: True", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* localhost() */


boolean protocol_is_ip
(
  ip_pkt_info_type *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_PKT_INFO( pkt_info_ptr->ip_hdr.v4.protocol == PS_IPPROTO_IP ) )
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "PROTOCOL_IS_IP: Protocol %d, True ",
                   pkt_info_ptr->ip_hdr.v4.protocol, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* protocol_is_ip() */


boolean src_addr_is_in
(
  ip_pkt_info_type    *pkt_info_ptr,
  uint32               netid,
  uint32               netmask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_PKT_INFO ((pkt_info_ptr->ip_hdr.v4.source.ps_s_addr & netmask)
        == netid ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "SRC_ADDR_IS_IN: Src Addr 0x%x in subnet 0x%x with  "
                   "netmask 0x%x, True",
                   pkt_info_ptr->ip_hdr.v4.source.ps_s_addr, netid, netmask);
    return TRUE;
  }

  return FALSE;
} /* src_addr_is_in() */


boolean dst_addr_is_in
(
   ip_pkt_info_type *pkt_info_ptr,
   uint32            netid,
   uint32            netmask
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_PKT_INFO ((pkt_info_ptr->ip_hdr.v4.dest.ps_s_addr & netmask) ==
         netid ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "DST_ADDR_IS_IN: Dest Addr 0x%x in subnet 0x%x with "
                   "netmask 0x%x, True",
                   pkt_info_ptr->ip_hdr.v4.dest.ps_s_addr, netid, netmask);
    return TRUE;
  }

  return FALSE;
} /* dst_addr_is_in() */


boolean specified_config_is_shareable
(
  boolean               (*is_cfg_shareable_fptr)
                                (void                       *this_if_ptr,
                                 acl_policy_info_type       *policy_info_ptr,
                                 ip_pkt_info_type           *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO( is_cfg_shareable_fptr(this_if_ptr,
                                           policy_info_ptr,
                                           pkt_info_ptr)))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "SPECIFIED_CONFIG_IS_SHAREABLE: True",
                   0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* specified_config_is_shareable() */


boolean iface_up_preferred_and_up
(
   acl_policy_info_type             *policy_info_ptr,
   ps_iface_type                    *this_if_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO((policy_info_ptr->policy_flag ==
         DSS_IFACE_POLICY_UP_PREFERRED) &&
       PS_IFACE_IS_UP((ps_iface_type*) this_if_ptr) ) )
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "IFACE_UP_PREF_AND_UP: True ", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* iface_up_preferred_and_up() */


boolean ip_pkt_family_is
(
  int               ipvsn,
  ip_pkt_info_type *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(USE_PKT_INFO( pkt_info_ptr->ip_vsn == (ip_version_enum_type) ipvsn ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "IP_PKT_FAMILY_IS: IP Pkt Family %d IP version, (Match)",
                   pkt_info_ptr->ip_vsn, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* boolean ip_pkt_family_is() */


boolean ip_pol_family_is
(
  int                   ipfam,
  acl_policy_info_type *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( USE_POL_INFO( policy_info_ptr->ip_family == ipfam) )
  {
    return TRUE;
  }
  ACL_TRACE_DUMP(ACL_LEVEL_MED,
	 "IP_POL_FAMILY_IS: IP Pol Family %d, IP family, (No Match)",
	  ((policy_info_ptr)?(policy_info_ptr->ip_family):IP_ADDR_INVALID), 0, 0);
  return FALSE;
} /* ip_pol_family_is() */


boolean iface_and_pkt_family_match
(
  ps_iface_type          *this_if_ptr,
  ip_pkt_info_type       *pkt_info_ptr
)
{
  uint8 if_family = IP_ADDR_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if_family = ps_iface_addr_family_is_v4(this_if_ptr)
                ? DSS_AF_INET
                : ps_iface_addr_family_is_v6(this_if_ptr)
                  ? DSS_AF_INET6
                  : DSS_AF_ANY;

  if ( ( IP_PKT_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY) &&
         ps_iface_addr_family_is_v6((ps_iface_type*) this_if_ptr)) ||
       ( IP_PKT_FAMILY_IS (IFACE_IPV4_ADDR_FAMILY) &&
         ps_iface_addr_family_is_v4((ps_iface_type*) this_if_ptr)))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "IFACE_AND_PKT_FAMILY_MATCH: Iface Family %d Pkt Family, "
                   "(Match)",
                   if_family, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* iface_and_pkt_family_match() */


boolean iface_and_pol_family_match
(
  ps_iface_type          *this_if_ptr,
  acl_policy_info_type   *policy_info_ptr
)
{
  uint8 if_family;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if_family = ps_iface_addr_family_is_v4(this_if_ptr)
                ? DSS_AF_INET
                : ps_iface_addr_family_is_v6(this_if_ptr)
                    ? DSS_AF_INET6
                    : DSS_AF_ANY;

  if ( IP_POL_FAMILY_IS(IFACE_UNSPEC_ADDR_FAMILY) ||
      (IP_POL_FAMILY_IS(IFACE_IPV6_ADDR_FAMILY) &&
       ps_iface_addr_family_is_v6((ps_iface_type*) this_if_ptr)) ||
      (IP_POL_FAMILY_IS(IFACE_IPV4_ADDR_FAMILY) &&
       ps_iface_addr_family_is_v4((ps_iface_type*) this_if_ptr)))
  {
    return TRUE;
  }
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "IFACE_AND_POL_FAMILY_MATCH: "
                   "Iface Family %d Pol IP Family, (No Match)",
                   if_family, 0, 0);
  return FALSE;
} /* iface_and_pol_family_match() */


boolean diff_iface_id_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO (policy_info_ptr->iface.kind == DSS_IFACE_ID) )
  {
    if ((policy_info_ptr->iface.info.id & 0xFF000000) !=
          (PS_IFACE_GET_ID((ps_iface_type*) this_if_ptr) & 0xFF000000))
    {
       ACL_TRACE_DUMP(ACL_LEVEL_MED,
                 "DIFF_IFACE_ID_REQD: Pol iface ID 0x%x, iface ID (Match)",
                 policy_info_ptr->iface.info.id & 0xFF000000, 0, 0);
      return TRUE;
    }
  }

  return FALSE;
} /* diff_iface_id_required() */


boolean diff_iface_name_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO (policy_info_ptr->iface.kind == DSS_IFACE_NAME))
  {
    if ((uint16)policy_info_ptr->iface.info.name & (uint16)IFACE_MASK)
    {
      if(policy_info_ptr->iface.info.name !=
          (dss_iface_name_enum_type)((ps_iface_type*) this_if_ptr)->name)
      {
        return TRUE;
      }
    }
  }
  ACL_TRACE_DUMP(ACL_LEVEL_MED,
		 "DIFF_IFACE_NAME_REQD: "
		 "Pol iface name 0x%x iface name, (No Match)",
		 ((policy_info_ptr)?(policy_info_ptr->iface.info.name):0),
                  0, 0);
  return FALSE;
} /* diff_iface_name_required() */


boolean diff_iface_group_required
(
  ps_iface_type            *this_if_ptr,
  acl_policy_info_type     *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO ((policy_info_ptr->iface.kind == DSS_IFACE_NAME)))
  {
    if (!((uint16)policy_info_ptr->iface.info.name & (uint16)IFACE_MASK))
    {
      if(!(policy_info_ptr->iface.info.name &
          ((ps_iface_type*) this_if_ptr)->group_mask))
      {
        ACL_TRACE_DUMP(ACL_LEVEL_MED,
                       "DIFF_IFACE_GRP_REQD: "
                       "Pol Iface Group 0x%x Iface Group, (Match)",
                       policy_info_ptr->iface.info.name & (uint16) IFACE_MASK,
                       0, 0);
        return TRUE;
      }
    }
  }

  return FALSE;
} /* diff_iface_group_required() */


boolean dest_addr6_is_unspecified
(
  ip_pkt_info_type    *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(USE_PKT_INFO(
    (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[0] == 0) &&
    (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[1] == 0)))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "DST_ADDR6_IS_UNSPECIFIED: Dest V6 Addr 0x%x, True",
                   pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[0],
                   pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[1],
                   0);
    return TRUE;
  }

  return FALSE;
} /* dest_addr6_is_unspecified() */


boolean iface_not_available
(
  boolean               (*is_available_fptr)
                                       (void                 *this_if_ptr,
                                        acl_policy_info_type *policy_ptr,
                                        ip_pkt_info_type     *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO((is_available_fptr)(this_if_ptr,
                                        policy_info_ptr,
                                        pkt_info_ptr) == FALSE ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "IFACE_NOT_AVAILABLE: True", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* iface_not_available() */


boolean specified_config_not_supported
(
  boolean               (*is_cfg_supported_fptr)
                                    (void                  *this_if_ptr,
                                     acl_policy_info_type  *policy_info_ptr,
                                     ip_pkt_info_type      *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO( is_cfg_supported_fptr(this_if_ptr,
                                           policy_info_ptr,
                                           pkt_info_ptr) == FALSE ))
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED,
                   "SPECIFIED_CONFIG_NOT_SUPPORTED: True",
                   0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* specified_config_not_supported() */


boolean iface_up_required_but_not_up
(
  acl_policy_info_type              *policy_info_ptr,
  ps_iface_type                     *this_if_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO( (policy_info_ptr->policy_flag ==
      DSS_IFACE_POLICY_UP_ONLY)))
  {
    if ((PS_IFACE_IS_UP((ps_iface_type*) this_if_ptr) == FALSE) )
    {
      ACL_TRACE_DUMP(ACL_LEVEL_MED,
                     "IFACE_UP_REQUIRED_BUT_NOT_UP: True",
                     0, 0, 0);
      return TRUE;
    }
  }

  return FALSE;
} /* iface_up_required_but_not_up() */


boolean pol_requires_sip_iface
(
  acl_policy_info_type              *policy_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( USE_POL_INFO(policy_info_ptr->sip_iface_reqd == TRUE) )
  {
    ACL_TRACE_DUMP(ACL_LEVEL_MED, "POL_REQUIRES_SIP_IFACE: True", 0, 0, 0);
    return TRUE;
  }

  return FALSE;
} /* pol_requires_sip_iface() */

