#ifndef PS_ACL_H
#define PS_ACL_H
/*===========================================================================

                   P R O T O C O L   S E R V I C E S

                A C C E S S   C O N T R O L   L I S T S

                          D E F I N I T I O N S

GENERAL DESCRIPTION
  Access control lists are used to define IP packet filters. These
  filters can be used e.g. for firewall functionality, address
  translation for selected ranges, or to build policy based routing
  rules.
  ACLs are defined as integer functions, which take information
  about the IP packet filtered as parameters. The return value
  is either '0' for a deny (packet drop) or the highest priority
  rule class for which a match was found.
  This file defines a common ACL function prototype and the data
  types to pass policy information to the ACL rules.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_acl.h_v   1.6   26 Nov 2002 14:49:14   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_acl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/2010 vk      Added flag in policy to support bypassing the preferred 
                   system check
05/12/2010 jy      Enhancements on data type acl_handoff_ctxt_type for
                   supporting Dual-IP bearer EPC handoff.
12/28/08   pp      Common Modem Interface: Public/Private API split.
02/05/07   sv      Added support for IM_CN flag.
04/28/06   mct     Added flag to netpolicy to ensure iface is set to inuse
                   only for bringup calls.
12/01/05   ks      Added APN to acl_policy_info for UMTS profile support
                   with QMI
06/11/04   vp      Removed declaration ip_pkt_info_s and placed it in
                   ps_pkt_info.h. Removal of inclusion of ps_iphdr.h,
                   ps_ip6_hdr.h and internet.h. Inclusion of ps_pkt_info.h
04/05/04   sv/rc   Featurized v6 specific code under FEATURE_DATA_PS_IPV6.
03/16/04   mct     Changed acl_ipfilter_info_type to ip_pkt_info_type.
01/31/04    usb    Removed redundant if_cache ptr from ip filter struct.
11/11/03   rc/aku  Added support for IPv6.
11/03/03    usb    Added lookup_only flag.
06/06/03    om     Added ipsec_required flag.
05/27/03    om     Added is_secure flag to IP info (used by IPSec).
10/15/02    om     Added disabled flag to ACL type.
09/25/02    om     Added forced routing to policy info.
09/24/02    om     Added UMTS PDP contect config to policy info.
09/09/02    om     Updates for ACL postprocessors.
08/30/02    om     Updated ipfilter and policy info definitions.
03/08/02    om     Created Module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "ps_iface_defs.h"
#include "dss_netpolicy.h"
#include "ps_pkt_info.h"


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
/* Pre-typedef's */
typedef struct acl_policy_info_s   acl_policy_info_type;

/*-------------------------------------------------------------------------
  ACL function types
-------------------------------------------------------------------------*/

/* An actual ACL's rules are grouped as this function type */
typedef int (*acl_fptr_type)
(
  void*                    this_if_ptr,
  acl_policy_info_type*    policy_info_ptr,
  ip_pkt_info_type*        pkt_info_ptr
);

/* Some ACL's require postprocessing after the routing decision */
/* this is the post-processor's function type.                  */
typedef void (*acl_post_process_fptr_type)
(
 void*                    this_if_ptr,
 acl_policy_info_type*    policy_info_ptr,
 ip_pkt_info_type*        pkt_info_ptr
);

/*-------------------------------------------------------------------------
  ACL data types
-------------------------------------------------------------------------*/

/* Accessor macro to access the interface pointer in ipfilter */
#define ACL_IPFI_IF_PTR(_ipfi_ptr) ((ps_iface_type*) ((_ipfi_ptr)->if_ptr))

typedef enum
{
  PS_ACL_POLICY_MIN             = 0,
  PS_ACL_POLICY_UP_ONLY         = PS_ACL_POLICY_MIN,
  PS_ACL_POLICY_ROUTEABLE_ONLY  = PS_ACL_POLICY_MIN + 1,
  PS_ACL_POLICY_UP_OR_ROUTEABLE = PS_ACL_POLICY_MIN + 2,
  PS_ACL_POLICY_NOT_DOWN        = PS_ACL_POLICY_MIN + 3,
  PS_ACL_POLICY_MAX
} ps_acl_data_path_policy_flags_enum_type;

/* Policy related information about an IP packet for routing ACLs */
typedef enum
{
  ACL_QOS_UNSPEC = 0,
  ACL_QOS_OPT_COST,
  ACL_QOS_OPT_THROUGHPUT,
  ACL_QOS_OPT_LATENCY
} acl_qos_enum_type;

/* UMTS PDP config info struct */
typedef void* acl_pdp_ctx_cfg_type;

/* Processor identifier */
typedef uint32  acl_proc_id_type;

/* Policy routing results, stores route_get() outcome */
typedef struct {
  int32                      priority; /* Policy priority          */
  int32                      rm_hndl;  /* QMI RmNet handle         */
  int32                      if_hndl;  /* Um IFACE handle          */
  ps_iface_name_enum_type    if_name;  /* Um IFACE name            */
} acl_rt_result_type;

/* handoff context -- supporting handoffs between techs */
typedef struct
{
  ps_iface_handoff_class_enum_type handoff_class; /* handoff indicator       */
  ps_iface_pdn_ip_config_type      pdn_ip_config; /* Ip cfg of the PDN       */
  ps_iface_bearer_ip_type          bearer_ip_type; /* bearer ip support      */
} acl_handoff_ctxt_type;

/* General policy based routing information */
struct acl_policy_info_s
{
  dss_iface_policy_flags_enum_type policy_flag; /* Desired policy behavior */
  ps_acl_data_path_policy_flags_enum_type  data_path_policy_flag;
  dss_iface_type       iface;        /* Specified iface - name, id, group  */
  uint64               app_identifier;     /* app identifier               */
  ps_iface_addr_family_type        ip_family;   /* IP family - v4, v6, ANY */
  boolean              is_routeable;
          /* Routeable to local stack during iface bridging/pkt forwarding */
  boolean              ipsec_disabled;     /* security not required        */
  boolean              lookup_only;        /* lookup but don't block iface */
  boolean              bring_up;           /* Allow the interface to be
                                              set to in use as app has
                                              called dsnet_start()         */
  boolean              im_cn_flag;         /* IM-CN flag for IMS           */
  boolean              sip_iface_reqd;     /* App wants to transfer data on
                                              physical iface               */
  boolean              allow_less_pref_sys;/* Allow selection of 
                                              non-preferred system         */ 

  acl_pdp_ctx_cfg_type pdp_info;           /* UMTS PDP context config info */
  /* to do - make this part of pdp_info */
  struct
  {
    uint8        length;
    byte         name[255];       /* UMTS apn string */
  } apn;

  int            data_session_profile_id; /* CDMA data session config info */

  acl_proc_id_type       proc_id;              /* Processor identifier     */
  int8                   rm_inst_handle;       /* Rmnet instance           */
  acl_rt_result_type     rt_result;            /* Policy routing results   */

  struct
  {
    ps_iface_type * ps_iface_ptr;
  } assoc_iface_info;

  acl_handoff_ctxt_type handoff_ctxt;
  boolean               mobileap_lookup; /* Routing initiated by one of the 
                                            MobileAP handlers. This field
                                            is set and used by MobileAP 
                                            interfaces only */ 
  uint32                subs_id;         /* Subscription Id */                                          
  ps_iface_apn_enum_type  apn_type;        /* APN type */                                        
  boolean               tlb_mode;        /*Is Test Loop Back mode set*/                                    
};

/* ACL handles provided by the originator allow         */
/* organization in a linked list by the routing module. */
typedef struct acl_s acl_type;
struct acl_s
{
  acl_fptr_type   acl_fptr;
  acl_post_process_fptr_type post_proc_fptr;
  void*           if_ptr;
  boolean         disabled;
  uint32          pkt_count;
  acl_type*       next_ptr;
};

#define ACL_IF_PTR( _a_ptr )  ((ps_iface_type*) ((_a_ptr)->if_ptr))


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION ACL_INIT_POLICY_INFO

DESCRIPTION
  Populates the policy info structure with appropriate default values.

DEPENDENCIES
  None.

PARAMETERS
  policy_info_ptr  pointer to policy info data structure.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the fields in the ACL policy structure to default values.

===========================================================================*/
extern void acl_init_policy_info
(
  acl_policy_info_type *policy_info_ptr       /* ACL policy info structure */
);


#ifdef __cplusplus
}
#endif

#endif    /* PS_ACL_H */
