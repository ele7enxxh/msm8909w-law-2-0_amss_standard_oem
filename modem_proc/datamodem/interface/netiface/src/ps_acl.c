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

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_acl.c_v   1.2   12 Feb 2003 20:35:28   omichael  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_acl.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/14    ssh    IP Translation Engine 464XLAT 
05/31/12   am      Bumped up NAT class priority and added mobileap_lookup
                   in acl_policy for NAT routing.
11/17/2010 vk      Added flag in policy to support bypassing the preferred 
                   system check
04/02/09   ar      Added ACL_PROXY_CONFIG_CLASS
01/29/09   pp      Redefined ACL rule priorities with the introduction of
                   SLIP mode class.
05/04/07   vp      Added MIP_CLASS
06/14/04   mct     Changed dss_iface_policy_flag_enum names.
11/03/03   usb     Initialize lookup_only flag.
07/20/03   usb     Redefined ACL rule priorities.
                   Added acl_init_policy_info() to initialize policy struct.
02/12/03   om      Added comments to identify the class array contents.
03/08/02   om      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "ps_acl.h"
#include "ps_aclrules.h"
#include "ps_sys.h"

/* <EJECT> */ 
/*===========================================================================

                        DATA DEFINITIONS

===========================================================================*/

/* Priority lookup table to map rule classes into routing priorities */
const int acl_class_priorities[ACL_NUM_CLASSES] =
{
  0,  /* ACL_DEFAULT_CLASS   */
  1,  /* ACL_IFGROUP_CLASS   */
  4,  /* ACL_IFNAME_CLASS    */
  7,  /* ACL_IPADDR_CLASS    */
  6,  /* ACL_IPPORT_CLASS    */
  8,  /* ACL_QOS_CLASS       */
  15, /* ACL_LOOPBACK_CLASS  */
  13, /* ACL_SLIP_MODE_CLASS */
  11, /* ACL_IPSEC_CLASS     */
  2,  /* ACL_PREF_MODE_CLASS */
  9,  /* ACL_CONFIG_CLASS    */
  5,  /* ACL_UP_PREF_CLASS   */
  18, /* ACL_EMRG_CLASS   */
  12, /* ACL_MIP_CLASS       */
  10, /* ACL_IWLAN_CLASS     */
  14, /* ACL_PROXY_CONFIG_CLASS */
  16, /* ACL_NAT_CLASS       */
  17  /* ACL_CLAT_CLASS       */
};


/* <EJECT> */ 
/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

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
void acl_init_policy_info
(
  acl_policy_info_type *policy_info_ptr       /* ACL policy info structure */
)
{
  if(policy_info_ptr != NULL)
  {
    policy_info_ptr->policy_flag        = DSS_IFACE_POLICY_ANY;
    policy_info_ptr->iface.kind         = DSS_IFACE_NAME;
    policy_info_ptr->iface.info.name    = DSS_IFACE_ANY_DEFAULT;
    policy_info_ptr->ip_family          = IFACE_IPV4_ADDR_FAMILY;
    policy_info_ptr->is_routeable       = FALSE;
    policy_info_ptr->ipsec_disabled     = FALSE;
    policy_info_ptr->lookup_only        = FALSE;
    policy_info_ptr->allow_less_pref_sys = FALSE;
    policy_info_ptr->pdp_info           = 0;
    policy_info_ptr->proc_id            = PS_IFACE_PROC_ID_LOCAL;
    policy_info_ptr->mobileap_lookup    = FALSE;
    policy_info_ptr->rm_inst_handle     = -1;
    policy_info_ptr->subs_id            = PS_SYS_DEFAULT_SUBS;
    policy_info_ptr->tlb_mode           = FALSE;

    memset(&(policy_info_ptr->handoff_ctxt), 0, sizeof(acl_handoff_ctxt_type));
  }
} /* acl_init_policy_info() */

