 /*!
  @file
  ds_pdn_context.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_pdn_context.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
07/04/14     ss      Support for new profile parameters to fetch DNS and 
                     PCSCF via DHCP
06/30/14     ss      New event added: IFACE_EXTENDED_IP_CONFIG_EX_EV
04/09/14     ash     Added Prefix Delegation support.
12/26/13     vl      Replacing the direct MTU assignment in IPv6 to macro 
                     PS_IFACE_SET_MTU
09/23/13     ssb     Changing variable name from ds_3gpp_pdn_bearer_wm_type
                     to ds_3gpp_pdn_bearer_wm_info
06/21/13     ash     Support MTU upto 2000
05/02/13     vs      NV item to check call origination is allowed before PS attach
09/25/12     ss      3GPP MH LTE-Off compilation fixes
09/10/12     ss      Added support for IPV4 link MTU IE for Rel10.
08/13/12     ss      Moving efs write operation to dsutil performed during 
                     bootup.
08/10/12     ss      Delegating RAT specific handling during call_end.
06/19/12     nd      MT PDP pkt call.
06/27/12     kv      Support for mode T in Apn bearer field.
03/14/12     nd      Race codition when RMnet client and network ends the call
                     simultaneously.
02/27/12     ss      Fixed deadlock between DS and IMS tasks.
02/22/12     rr      Generate IFACE_EXTENDED_IP_CONFIG_EV after receiving PCSCF address update
02/01/12     ss      RPM: Fixed of sending ATTACH request again due to PS session
                     initiation after PS ATTACH is ignored by network.
12/08/11     rr      Added support to update p-cscf address from NW bearer modification 
                     indiacation 
12/21/11     vb      Added change to handle IOCTL query PS_SYS_IOCTL_LTE_ATTACH_PARAMS 
09/01/11     jee     To fix Arbitration issue with v4/v6 calls
10/21/11     nd      Fixed compilation warnings.
10/05/11     ttv     Added DSDS changes for new mode handler.
10/03/11     ash     Added new ioctl support PS_IFACE_IOCTL_GET_IPV6_REMAINING_RTR_LIFETIME
9/09/11      rr      Added support for fallback from Dual IP to single IP when nw rejects 
                     with specific cause codes in UMTS mode
09/05/11     ss      Changes to support RPM.
08/30/11     bd      Added handler for ioctl to retrieve pdp context id
08/23/11     sa      Added support to retrieve network allocated APN for NULL 
                     APN profile activation.
07/11/11     ttv     Fixed klocwork errors.
07/11/11     ss      Enable flow on the phys_link of the default bearer after
                     iface has gone out_of_use.
06/21/11     dvk     Fixed compilation warnings.
05/25/11     sa      Added support for 3gpp_rel_version NV.
05/04/11     ttv     Added changes needed for SESSION_PARAMS IOCTL and NSAPI api.
04/07/11     ttv     Addec changes for IOCTL PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG.
04/07/11     ttv     Fixed compilation warning.
04/05/11     sch     Added func to fill DNS server Ipv4 addr req container
03/30/11     sa      Added fix to update the ds_3gpp_sys_mode to be inline 
                     with the ds3gi_mode.
03/24/11     sa      Added MT PDP Auto answer support.
03/17/11     sa      Added support for bearer tech change event indication.
03/14/11     ars     Support for NV configurable MTU value
02/10/11     ss      Added support for configurable RAB_REESTAB RETRY.
02/23/11     ash     Initialize sm_config regardless of whether nv item is active or not.
02/22/11     ttv     Merged the changes of adding wrapper function to get 
                     cumulative iface stats.
01/31/11     ss      Added support for reporting bearer tech type in WCDMA 
                     and GPRS.
11/19/10     az      Fixed an issue that v6 iface is not cleaned up when RA
                     failure happens
11/19/10     ars     Changed proc of setupBridgeInterfaceEvent to command based
11/12/10     vk      Special handling for arbitration scenarios
10/12/10     ars     Fixed a race condition where rm um bridge could be torn
                     down even when pdn/bearer context is not initialized.
10/1/10      ars     Enabled processing auth parameters from info
07/10/10     vk      Support for data path bridging for sdio ports
05/17/10     hs      Added APN disable and PDN inactivity feature support.
05/12/10     sa      Added support for retrieving dynamic pdn state info.
04/21/10     sa      Added support for Loopback Mode B.
04/06/10     sa      Updated ds_pdn_cntxt_get_pdn_context_pdp_addr().
04/03/10     vk      Added support for MSG2.0 based logging
04/02/10     vrk     UMTS interRAT support
03/15/10     hs      Made changes to increase PDN connections from 3 to 4
02/08/10     sa      Added function ds_pdn_cntxt_get_pdn_context_pdp_addr. 
02/04/10     vd      Using the changed flow function names.
01/27/10     vd      Using the changed flow manager function names.
01/23/10     vk      Added helper macros for virtual function dispatch
01/04/10     vk      Removing certain macros from PDP reg API header file
                     that cause problems with autogenerated RPC stub code
12/12/09     sa      Added Function ds_pdn_cntxt_get_pdn_context_state. 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "ds_3gppi_utils.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_3gpp_dyn_mem_hdlr.h"
#include "cm.h"
#include "cm_gw.h"
#include "sys.h"
#include "msg.h"
#include "err.h"
#include "IxErrno.h"
#include "amssassert.h"
#include "rex.h"
#include "ds3gmgr.h"
#include "ds3gutil.h"
#include "ds3geventmgr.h"
#include "ds3gcmif.h"
#include "ds3gmshif.h"
#include "ds3gflowmgr.h"
#include "dstask_v.h"
#include "task.h"
#include "dsm.h"
#include "dsumtspspco.h"
#include "ds_3gpp_op_pco_hdlr.h"
#include "ds_3gpp_low_prio_sig_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#ifdef FEATURE_DATA_TE_MT_PDP
#include "dsumtspsmthdlr.h"
#endif /* FEATURE_DATA_TE_MT_PDP */
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_pppi.h"
#include "ds_flow_control.h"
#include "ps_in.h"
#include "ds_3gpp_rt_acl.h"
#include "ps_crit_sect.h"
#include "ps_utils.h"
#include "ps_ifacei_utils.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_fsm.h"
#include "ps_stat_iface.h"
#include "ps_sys_ioctl.h"
#include "dserrno.h"
#include "ps_iface_defs.h"
#include "ps_iface_ioctl.h"
#include "ps_iface_addr_v6.h"
#include "ps_ifacei.h"
#include "dsgen_iface_hdlr.h"
#include "dstaski.h"
#include "nv_items.h"
#include "ps_rohc.h"
#include "dsumtspdpreg.h"
#include "ps_flow.h"
#include <stringl/stringl.h>
#include "ds_3gpp_flow_manager.h"
#include "ds_3gpp_bearer_flow_manager.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_3gpp_flow_context.h"
#include "ds_3gpp_apn_table.h"
#include "ps_stat_iface.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "dsumts_rmsm.h"
#include "ds_umts_bearer_context.h"
#include "ds_umts_pdn_context.h"
#include "ds_wcdma_bearer_context.h"
#include "ds_gprs_bearer_context.h"
#include "ds_umts_flow_manager.h"
#include "ds_umts_bearer_flow_manager.h"
#include "ds_3gpp_auth_mgr.h"
#include "ds_3gpp_rmsm_atif.h"
#include "ds_3gpp_rmsm_ati.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_plmn_hdlr.h"
#ifdef FEATURE_DATA_LTE
#include "ds_eps_bearer_context.h"
#include "ds_3gpp_apn_switch_mgr.h"
#include "ds_eps_bearer_flow_manager.h"
#include "ds_eps_pdn_context.h"
#include "ds_eps_pdn_throttle_sm.h"
#include "ds_eps_tlb.h"
#include "ds_eps_kamgr_ext.h"
#include "lte.h"
#include "lte_rrc_ext_api.h"
#endif /* FEATURE_DATA_LTE */
#include "ds_dsd_apm_ext_i.h"
#include "ds3gmmgsdiif.h"
#include "dsutil.h"
#include "ran.h"
#include "ds_3gpp_pdn_redial_hdlr.h"
#include "ds3gdsdif.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_iface_addr_v6.h"
#include "ps_ip6_sm.h"
#endif /* FEATURE_DATA_PS_IPV6 */



#include "log.h"

#ifdef FEATURE_DATA_TEST_LOOPBACK
#include "ds_um_lo.h"
#endif /* FEATURE_DATA_TEST_LOOPBACK */

#ifdef FEATURE_DATA_RPM
#include "ds_3gpp_rpm.h"
#endif /* FEATURE_DATA_RPM */


#ifdef FEATURE_DATA_LTE
#include "ds_3gpp_pdn_limit_hdlr.h"
#endif /*FEATURE_DATA_LTE*/
#include "ds3gpp_api.h"
#include "ps_sys.h"
#include "sys_cnst.h"
#include "ds_3gpp_roaming_hdlr.h"
#ifdef FEATURE_DATA_PS_IPV6
#include "dhcp6_client.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds_3gpp_profile_api.h"
#include "ds_3gpp_embms_hdlr.h"
#include "ds_3gpp_device_ev_hdlr.h"
#include "ds3gprofile_validator.h"

#ifdef FEATURE_DATA_ANDSF
#include "ds_andsf_api.h"
#include "ds_andsf_ran_meas.h"
#endif /* FEATURE_DATA_ANDSF*/

#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_util.h"
#endif /*FEATURE_EPC_HANDOFF*/


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================
  Cookie to ensure that dynamic memory allocated is valid
===========================================================================*/
#define DS_3GPP_PDN_CNTXT_PER_SUBS_INFO_COOKIE 0xabcabcab

#define DS_LTE_DISABLE 0
#define DS_LTE_PLMN_INFINITE_BLOCKING 0xFFFFFFFF

/*---------------------------------------------------------------------------
  Define the ether types for V4 and V6. These will be used while configuring
  A2 hardware with Ethernet header
---------------------------------------------------------------------------*/
#define DS_3GPP_PDN_CONTEXT_ETHER_TYPE_V4 2048  //translates to 0x0800
#define DS_3GPP_PDN_CONTEXT_ETHER_TYPE_V6 34525 //translates to 0x86DD


/*---------------------------------------------------------------------------
  IP address allocation mechanism container IDs
  ref: TS24.008 sec 10.5.6.3
---------------------------------------------------------------------------*/
#define IP_ADDR_ALLOC_NAS_CONTAINER_ID 0x0A
#define IPV4_ADDR_ALLOC_DHCPV4_CONTAINER_ID 0x0B

/*---------------------------------------------------------------------------
  Buffer to hold the challenge generated for CHAP
---------------------------------------------------------------------------*/
static uint8 chap_challenge_buffer[CHAP_CHALLENGE_LEN] = {0};

#define DS_3GPP_RMNET_ETHERNET_HEADER_SIZE  14
#define MAX_HEADER_LEN                      DS_3GPP_RMNET_ETHERNET_HEADER_SIZE

#define DS_3GPP_LINK_PROTO_TYPE_RMNET_INVALID_MODE 0x0
#define DS_3GPP_LINK_PROTO_TYPE_RMNET_ETHERNET_MODE 0x1
#define DS_3GPP_LINK_PROTO_TYPE_RMNET_IP_MODE 0x2
#define DS_3GPP_LINK_PROTO_TYPE_RMNET_ALL_MODE 0x3

/*---------------------------------------------------------------------------
  Structure containing subscription specific globals in ds_3gpp_pdn_context
---------------------------------------------------------------------------*/
typedef struct
{
  uint32                  cookie;
  sys_modem_as_id_e_type  subs_id;
  sys_plmn_id_s_type      current_plmn;

  /*---------------------------------------------------------------------------
    Below list tracks the blocked PLMNs at any given point. If a PLMN 
    gets unblocked it would be removed from this list.
  ---------------------------------------------------------------------------*/
  list_type               blocked_plmn_list;
  uint8                   max_simul_active_pdp_context; 
} ds_3gpp_pdn_cntxt_per_subs_info_type;

/*---------------------------------------------------------------------------
  Structure containing all globals in ds_3gpp_pdn_context
---------------------------------------------------------------------------*/
typedef struct
{
  ds_3gpp_pdn_cntxt_per_subs_info_type
                                     *per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX];

  boolean                             rpm_plmn_blocked;

  /* Variable to hold the current thermal mitigation state */
  ds3geventmgr_th_mit_action_type     th_mit_state;

} ds_3gpp_pdn_context_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/*---------------------------------------------------------------------------
  PDN context table. We define DS_3GPP_MAX_PDN_CONTEXT as the upper bound on
  the number of PDNs we support connectivity to. This number stems from the
  maximum number of available ifaces.
  The default Ifaces can be used to establish connectivity to one PDN (either
  single address bearer or Dual address bearer). The remaining 4 ifaces can
  establish connectivity to 4 other PDNs at the maximum. This is under the
  assumption that we connect to all these PDNs using single address bearers.
  If dual address bearers are used in any of these non default APNs, then we
  will end up using lesser PDN context structures than what we have.
  DS_3GPP_MAX_PDN_CONTEXT has been set to 5 and this is the worst case that
  is to be supported.
---------------------------------------------------------------------------*/
ds_pdn_context_s ds_pdn_context_tbl[DS_3GPP_MAX_PDN_CONTEXT] = {{NULL}};

ds_pdn_context_vftbl_s ds_pdn_context_ftbl[DS_3GPPI_SYS_MODE_MAX] = {{NULL}};

bearer_context_notifier_cb_tbl_s bearer_context_notifier_cb_tbl = {NULL};

/*---------------------------------------------------------------------------
  Table of DS ifaces to be used. The total number of Ifaces available for use
  is restricted tp DS_3GPP_MAX_PS_IFACES. This currently corresponds to 6.
  Out of these 6, two of them are reserved for attach case. One of these 2
  ifaces is marked as V4 and the other is V6. These two ifaces are available
  only for the default APN and not for any others.
  The remaining 4 ifaces are generic and can be set to V4 or V6 in post proc
  ACL rules. These 4 ifaces are not tied to any PDN context either.
---------------------------------------------------------------------------*/
ds_3gpp_iface_s ds_iface_tbl[DS_3GPP_MAX_PS_IFACES] = {{NULL}};

/*---------------------------------------------------------------------------
  Structure containing all globals in ds_3gpp_pdn_context
---------------------------------------------------------------------------*/
static ds_3gpp_pdn_context_info_type ds_3gpp_pdn_context_info = {{NULL}};


#ifdef FEATURE_EMERGENCY_PDN
/*-------------------------------------------------------------------------- 
  Variable to handle T3420/T3418 timer for auth failure for Emergency call
 ---------------------------------------------------------------------------*/
static boolean ds_3gpp_emergency_auth_fail =  FALSE; 
#endif /*FEATURE_EMERGENCY_PDN*/

/*--------------------------------------------------------------------------- 
  This counter is used during LPM processing. This keeps track of the number
  of DS IPV6 ifaces (associated DSSNET SM's as well)  which are already up
  and which needs graceful clean up before modem enters into low power mode.
  When there are no IPV6 ifaces/internal state machines to clean up (or)
  if all the IPV6 SM's are cleaned up successfully, notify DSMGR
  ---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6
static int8 ds_3gpp_num_ipv6_ifaces_to_cleanup[DS3GSUBSMGR_SUBS_ID_MAX] = {0};
#endif

/* dynamic ptr to hold Delay Dormancy Info*/
static ds_3gpp_delay_dormancy_s*  ds_3gpp_delay_dormancy_info_p = NULL;

#ifdef FEATURE_EPC_HANDOFF

/*---------------------------------------------------------------------------
  Variable to check if a call is allowed with 3GPP MH
---------------------------------------------------------------------------*/
ds_3gpp_if_call_allowed_hdlr_ex_f_type 
  ds_3gpp_pdn_cntxt_call_allowed_hdlr_ex_f=NULL;

/*---------------------------------------------------------------------------
  Variable to check the attach type - Initial or handover.
---------------------------------------------------------------------------*/
ds_3gpp_if_call_attach_type_hdlr_ex_f_type 
  ds_3gpp_pdn_cntxt_is_handover_attach_ex_f = NULL;


#endif /* FEATURE_EPC_HANDOFF */

/*=========================================================================== 
 
FUNCTION : DS_3GPP_PDN_CONTEXT_RETRY_IFACE_BRING_UP
 
DESCRIPTION:                                                                                        
    This function calls the iface up command handler to retry iface bring up
    
PARAMETERS   
  None 

DEPENDENCIES  
  The PDN context state should be in 'AWAITING_BEARER_ALLOC' state
  to retry iface bring up

RETURN VALUE  
  None 
 
===========================================================================*/

static void ds_3gpp_pdn_context_retry_iface_bring_up
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SEARCH_AND_UPDATE_PDN_DISCONN_IN_PROGRESS_LIST

DESCRIPTION 
  This function checks, if the incoming 'pdn_cntxt_tbl_index' is already
  available in 'pdn disconn list' and adds the incoming tbl index
  to the list.

PARAMETERS 
  
  pdn_cntxt_tbl_index   - PDN tbl index in pdn_context_tbl that needs to be
                          checked, if that PDN bring down is in progress
 
  pdn_disconn_list      - Pointer to the temporary list holding the list of
                          PDN table index's for which PDN disconnect is
                          already issued. 
                             
DEPENDENCIES 
  * Used during a prioritized PDN bring up where bearer mgmt algorithm is
    applied.
 
  * sizeof(pdn_disconn_list) = DS_3GPP_MAX_PDN_CONTEXT

RETURN VALUE
  FALSE   - If the search returned failure. Update the list with the
            incoming value for subsequent searches
  TRUE    - If the search found a matching PDN tbl index in the
            'pdn_tbl_index_list'
            
SIDE EFFECTS
  If sizeof(pdn_disconn_list) < DS_3GPP_MAX_PDN_CONTEXT then the incoming
  pdn_instance might not be stored. 

===========================================================================*/
static boolean ds_3gpp_pdn_cntxt_search_and_update_pdn_disconn_in_progress_list
(
  int pdn_cntxt_tbl_index,
  int *pdn_disconn_list
);

/*===========================================================================
FUNCTION DS_3GPP_GET_EMERG_PDN_CNTXT

DESCRIPTION 
    This function loops through the PDN context table and
    returns the emergency PDN context, if found
 
PARAMETERS
    None
DEPENDENCIES
    None.

RETURN VALUE
    ds_pdn_context_s* - Pointer to the emergency PDN context
    NULL              - If no Emergency PDN context found 
   
SIDE EFFECTS 
  None.
===========================================================================*/

ds_pdn_context_s* ds_3gpp_get_emerg_pdn_cntxt(void);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_NUM_VALID_PDN_CONTEXTS

DESCRIPTION 
  This function, returns the number of
  valid PDN contexts for which bearer resoures are already allocated.
 
  p.s: This may not necessarily return the number of active PDN connection
  when there are one or more PDN contexts in coming up (or) PDN
  teardown is in progress.
  However in such cases
 
  num_valid_pdn's >= num_active_pdn_contexts
 
  This is currently being used while applying Bearer mgmt algorithm
PARAMETERS
    None
DEPENDENCIES
    None.

RETURN VALUE
  integer 0-255
   
SIDE EFFECTS 
  None.
===========================================================================*/

int ds_3gpp_pdn_cntxt_get_num_valid_pdn_contexts
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_APPLY_BEARER_MGMT

DESCRIPTION 
    Bearer Management Algorithm is applied during iface bring up, when the
    device runs out of bearers to bring up an prioritized PDN connection
    and we need to release bearers from one or more existing PDN connections.
    This function does the following.
    * Checks if there are sufficient bearers available to bring up the
      prioritized PDN connection during iface bring up 
    * If there aren't sufficient bearers,
      - (A)If 'APN disconnect order' list is configured in EFS
        -> read 'APN identifiers' from the EFS and bring down corresponding
           PDN's in order as in EFS provided they are not either emergency PDN
           or the last active PDN contexts
        -> If its the last active PDN, attempt to release ded-bearers
           from last PDN case (C)
      - If the EFS file is not present (or) (A) is partially successful
        (B)-> bring down PDN's one by one as long as it is not the last
              active PDN and not emergency PDN
        (C)-> If (A)/(B) has failed and there is only one active PDN
              release ded-bearers from last PDN context.
PARAMETERS 
  iface_p          - the ps iface that is being brought up
  num_reqd_bearers - Required number of bearers to bring up iface                        
                             
DEPENDENCIES
  Invoked during iface bring up, where we make one more attempt, before denying
  the iface bring up  due to insufficient bearer resources for prioritized PDN
  connections(Eg: emerg PDN)

RETURN VALUE
DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL    -     Bearer Mgmt action Failed. 
                                            No Bearer to bring up iface
 
DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS - Bearer mgmt action in progress. 
                                            Possibly bearers will be
                                            available in some time
                                        
DS_3GPP_PDN_CNTXT_BEARER_MGMT_NOT_REQD    - Bearer mgmt action not required 
 
SIDE EFFECTS
  None.

===========================================================================*/

static ds_3gpp_pdn_cntxt_bearer_mgmt_status_e ds_3gpp_pdn_cntxt_apply_bearer_mgmt
(
  ps_iface_type *iface_p,
  uint8 num_reqd_bearers 
);


/*===========================================================================
FUNCTION DS_3GPP_GET_PDN_DISCONN_LIST_FRM_APN_DISCONN_ORDER_LIST

DESCRIPTION
  This routine gets PDN contexts associated with APN identifiers in
  APN disconnect order list and updates the input 
  'pdn_disconn_list' one by one in order they match  

PARAMETERS
  pdn_disconn_list  - List holding the PDN tbl index for which PDN
                      connection is already issued

DEPENDENCIES
  APN disconnect order list should not be empty else
  the 'pdn_disconn_list' isnt updated

RETURN VALUE
  None
=============================================================================*/
void ds_3gpp_get_pdn_disconn_list_frm_apn_disonn_order_list
(
  int* pdn_disconn_list,
  sys_modem_as_id_e_type subs_id
);

static void ds_3gpp_pdn_cntxt_handle_dssnet6_ev_cback
(
  dssnet6_sm_cb_type    *instance,
  dssnet6_sm_event_type  cur_event,
  dssnet6i_state_type    cur_state,
  void                  *user_data
);

static void ds_3gpp_pdn_cntxt_handle_v6_iface_down
(
  ps_iface_type   *this_iface_ptr,
  ps_iface_net_down_reason_type down_reason
);
static void ds_pdn_context_cleanup_after_iface_down_ind
( 
  ds_pdn_context_s        *pdn_cntx_p
);

void ds_pdn_cntxt_get_iface_stat_adjustment
(
  ps_iface_type *iface_ptr,
  ps_iface_stat_adjustment_type* adjustment
);

static int ds_pdn_cntxt_setupBridgeInterfaceCmdHdlr
(
  const ds_cmd_type *cmd_ptr
  /*lint -esym(715,client_data) */
  /*lint -esym(818,client_data) */
);

static void ds_3gpp_pdn_cntxt_sys_chg_ss_info_cmd_hdlr
(
  const ds3g_sys_chg_ss_info_type  *sys_chg_ss_info_ptr
);

#ifdef FEATURE_DATA_LTE
static boolean ds_pdn_cntxt_is_pdn_context_tearable(ds_pdn_context_s *pdn_cntx_p);
#endif /*FEATURE_DATA_LTE*/

ps_iface_type* ds_3gpp_pdn_get_companion_iface(ps_iface_type *iface_p);


static boolean ds_3gpp_pdn_cntx_alloc_dyn_mem
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

#ifdef FEATURE_DATA_PS_IPV6
static int ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
);
#endif /* FEATURE_DATA_PS_IPV6 */

void ds_3gpp_pdn_cntxt_process_cm_notify_cmd
(
  const ds_cmd_type *cmd_ptr
);

void ds_3gpp_pdn_cntxt_process_thermal_mitigation_cmd
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_PDN

DESCRIPTION
  This function tears down the PDN connection.
  
PARAMETERS 
  pdn_cntxt_p - PDN control block
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_pdn_cntx_teardown_PDN
( 
  ds_pdn_context_s *pdn_cntxt_p,
  ps_iface_net_down_reason_type tear_down_reason
);


static void ds_pdn_cntxt_set_throt_down_reason
(
  ps_iface_type                  *iface_p, 
  ps_iface_net_down_reason_type  *down_reason_p
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function fetches a handle to the PDN Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  Handle to the PDN Context Information for the given Subscription.

SIDE EFFECTS
  None.
===========================================================================*/
static ds_3gpp_pdn_cntxt_per_subs_info_type* 
         ds_3gpp_pdn_context_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets the handle to the PDN Context Information
  corresponding to the given Subscription.

PARAMETERS
  subs_id          : Subscription Id
  hndl             : Handle to the PDN Context Information for
                     the given Subscription.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_pdn_context_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type                 subs_id,
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl
);

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates the handle to the Subscription Specific
  PDN Context Information. 
  
PARAMETERS
  hndl  : Handle to the Subscription specific
          PDN Context Information

DEPENDENCIES
  None.

RETURN VALUE
  TRUE,  if the handle is valid
  FALSE, if invalid

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_3gpp_pdn_context_validate_per_subs_info_hndl
(
  ds_3gpp_pdn_cntxt_per_subs_info_type *hndl
);

/*===========================================================================
FUNCTION      DS_PDN_CNTXT_GET_THROTTLE_OVERRIDE

DESCRIPTION
  This function decides whether throttling can be overridden for this APN.
  This is a REL-11 Spec CR 2157 (C1-122386) which mandates that UEs of 
  Access Class 11-15 should be allowed to send PDP Context/PDN Context
  request to network even though APN is throttled as per 
  T3396 throttling requirements.

PARAMETERS
  subs_id       - Subscription id in the current PDN context.
  apn           - The APN which is throttled.
  iface_p       - Pointer to the iface that is to be brought up.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - If throttling needs to be overridden.
  FALSE - If throttling need not be overridden.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_get_throttle_override
(
  sys_modem_as_id_e_type  subs_id,
  byte                    *apn,
  ps_iface_type           *iface_p
);

/*===========================================================================


                               FUNCTIONS

===========================================================================*/



/*===========================================================================
FUNCTION DSUMTSPS_FILL_CHAP_ORIG_PARMS

DESCRIPTION
  This function fills the CHAP specific protocol information in 
  the PDP context req. message. The chap challenge and response are passed in
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    chal_str_ptr      - pointer to challenge generated by the challenger
    chal_str_len      - length of challenge
    chal_name_ptr     - pointer to the name string of the challenger
    chal_name_len     - length of the challenger's name
    digest_str_ptr    - pointer to md5 digest (challenge response) string
    digest_str_len    - length of the digest
    resp_str_ptr      - pointer to responders name
    resp_str_len      - length og the responders name
    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
  orig_params_ptr->config_options.num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function
  
===========================================================================*/
static boolean ds_pdn_cntxt_fill_chap_orig_params
(
  protocol_cfg_options_T      *protocol_config_options_p,
  const byte                  *chal_str_ptr,
  byte                         chal_str_len,
  const byte                   *chal_name_ptr,
  byte                         chal_name_len,
  const byte                   *digest_str_ptr,
  byte                         digest_str_len,
  const byte                   *resp_str_ptr,
  byte                         resp_str_len,
  byte                         auth_id
)
{
  int     chap_chal_pkt_len,chap_res_pkt_len;
  uint16  proto_info_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   DS_3GPP_MSG0_HIGH("Fill CHAP params from PPP ");

  /*-------------------------------------------------------------------------
   Get the protocol info index. This index is used to fill in PPP packets
   in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    check if we have room to store this protocol info pkt in the pdp ctx req
    we need room to store 2 pkts : chap challenge & response
  -------------------------------------------------------------------------*/
  if( (proto_info_index + 2 ) >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store CHAP pkts");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Construct the challenge packet. Pass the parameters received
    to  construct the CHAP challenge
  -------------------------------------------------------------------------*/
  chap_chal_pkt_len = dsumtsps_ppp_gen_chap_challenge_pkt
    (
    &protocol_config_options_p->procol_info[proto_info_index].
     protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    auth_id,
    chal_str_ptr,
    chal_str_len,
    chal_name_ptr,
    chal_name_len
    );

  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing and 
    return an error
  -------------------------------------------------------------------------*/
  if( chap_chal_pkt_len < 0 )
  {
    DS_3GPP_MSG1_ERROR("CHAP pkt gen fail C:%d ",chap_chal_pkt_len);
    return FALSE;
  }
  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0] = ( (PPP_CHAP_PROTOCOL & 0xff00) >> 8);
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1] = ( (PPP_CHAP_PROTOCOL & 0x00ff) );
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len = (uint8)chap_chal_pkt_len;    

  /*-------------------------------------------------------------------------
    Increment the protocol info index. 
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  /*-------------------------------------------------------------------------
    Construct the response packet. Pass the parameters received for the
    repsonse & construct the chap response packet. 
  -------------------------------------------------------------------------*/
  chap_res_pkt_len = dsumtsps_ppp_fill_chap_response_pkt
    (
    &protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    auth_id,
    digest_str_ptr,
    digest_str_len,
    resp_str_ptr,
    resp_str_len
    );
  
  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing.
    Adjust the protocol_info_index : remove the chap challenge
    packet which has been included.
  -------------------------------------------------------------------------*/
  if( chap_res_pkt_len <0 )
  {
    DS_3GPP_MSG1_ERROR("CHAP pkt gen fail  R:%d",chap_res_pkt_len);
    protocol_config_options_p->num_protocol_ids --; 
    return FALSE;
  }
  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ( (PPP_CHAP_PROTOCOL & 0xff00) >> 8);
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = ( (PPP_CHAP_PROTOCOL & 0x00ff) );
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len = (uint8)chap_res_pkt_len;    
  DS_3GPP_MSG2_HIGH("Chal len :%d res len :%d",chap_chal_pkt_len,chap_res_pkt_len);
    
  /*-------------------------------------------------------------------------
    Increment the protocol info index. 
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;
  protocol_config_options_p->valid            = TRUE;
  return TRUE;

} /* ds_pdn_cntxt_fill_chap_orig_params */


/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_MCC_MNC_TO_PCO

DESCRIPTION
  This function fills the mcc/mnc to PCO
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    MCC
    MNC
    mnc_includes_pcs_digit
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
static boolean ds_pdn_cntxt_fill_mcc_mnc_to_pco
(
  protocol_cfg_options_T       *protocol_config_options_p,
  uint16                        mcc,
  uint16                        mnc_digits,
  boolean                       mnc_includes_pcs_digit
)
{
  uint8 mcc_digit_1,mcc_digit_2,mcc_digit_3;
  uint8 mnc_digit_1,mnc_digit_2,mnc_digit_3;
  uint8 temp ;
  boolean              ret_val = FALSE;
  nv_ehrpd_imsi_type   imsi_data;
  uint16               proto_info_index  = 0;
 /*------------------------------------------------------------------------*/
  if (protocol_config_options_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("PCO ptr is NULL");
    return ret_val;
  }

  temp = 0;
   mcc_digit_1 = mcc_digit_2 = mcc_digit_3 = 0;
   mnc_digit_1 = mnc_digit_2 = mnc_digit_3 = 0;

  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    check if we have room to store this protocol info pkt 
  -------------------------------------------------------------------------*/
  if( (proto_info_index + 1 ) >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store OPER PCO pkts");
    return ret_val;
  }

   /* If mcc and mnc are both zero; then we need to get the mcc,mnc value from the sim card else
      use the values from the profile.*/
   if (mcc==0 && mnc_digits==0 )
   {
    memset(&imsi_data, 0, sizeof(nv_ehrpd_imsi_type));

    if (FALSE == ds3g_mmgsdi_get_imsi_data(&imsi_data))
    {
      DS_3GPP_MSG0_ERROR("Failed to read IMSI data from SIM");
      return ret_val;
    }

    if (imsi_data.num_of_mnc_digits == 2)
    {
      imsi_data.ehrpd_mnc[2] = 0x0F;
    }
    
    mcc_digit_1= imsi_data.ehrpd_mcc[0];
    mcc_digit_2= imsi_data.ehrpd_mcc[1];
    mcc_digit_3= imsi_data.ehrpd_mcc[2];

    mnc_digit_1= imsi_data.ehrpd_mnc[0];
    mnc_digit_2= imsi_data.ehrpd_mnc[1];
    mnc_digit_3= imsi_data.ehrpd_mnc[2];
   }
   else
   {
    /* The mcc is supposed to be a 3 digit number is thus divided by 100 to get the mcc_digit_1.
       Similarly  we  get mcc_digit_2 & mcc_digit_3 by appropriately removing the hundred's place and ten's place
       respectively */

    mcc_digit_1 = mcc /100;
    temp= mcc -(100*mcc_digit_1);
    mcc_digit_2=  temp/10;
    mcc_digit_3= mcc - (100*mcc_digit_1 + 10*mcc_digit_2);

   /* The mnc is supposed to be a 3 or 2 digit number is thus divided by 100 to get the mcc_digit_1.
   Similarly  we  get mnc_digit_2 & mnc_digit_3 by appropriately removing the hundred's place and ten's place
   respectively */

    mnc_digit_1 = mnc_digits /100;
    temp= mnc_digits -(100*mnc_digit_1);
    mnc_digit_2=  temp/10;
    mnc_digit_3= mnc_digits - (100*mnc_digit_1 + 10*mnc_digit_2);


    /*If the mnc_digit_1=0 amd 
      mnc_includes_pcs_digit is FALSE  Example: 34 will be considered as 
     mnc_includes_pcs_digit is  TRUE  Example: 34 will be considered as  034 */
    if (mnc_digit_1==0 && !(mnc_includes_pcs_digit))
    {
      mnc_digit_1=mnc_digit_2;
      mnc_digit_2=mnc_digit_3;
      mnc_digit_3=0x0F; 
    }

   }

   /*---------------------------------------------------------------------- 
       3 Octets: How we encode mcc/mnc as part of Pco contents:"
        OCTET1:-   mcc_digit_2  mcc_digit_1
        OCTET2:-   mnc_digit_3  mcc_digit_3
        OCTET3:-   mnc_digit_2  mnc_digit_1
   -----------------------------------------------------------------------*/

  protocol_config_options_p->procol_info[proto_info_index].
     protocol_contents[0] = mcc_digit_1 | (mcc_digit_2 << 4);
  protocol_config_options_p->procol_info[proto_info_index].
     protocol_contents[1] = mcc_digit_3 | (mnc_digit_3 << 4);
	protocol_config_options_p->procol_info[proto_info_index].
     protocol_contents[2]= mnc_digit_1 | (mnc_digit_2 << 4);
  protocol_config_options_p->procol_info[proto_info_index].
     protocol_len= MCC_MNC_LENGTH;


  ret_val = TRUE;

  DS_3GPP_MSG1_MED("ds_pdn_cntxt_fill_mcc_mnc_to_pco returns %d",ret_val);
  return ret_val;
}

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_OPER_PCO_INFO_FROM_PROFILE

DESCRIPTION
  This function fills the operator specific protocol information in 
  the PDP context req. message. 
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    profile_p - Prt to the profile info    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_cntxt_fill_oper_pco_info_from_profile
(
  protocol_cfg_options_T       *protocol_config_options_p,
  ds_umts_pdp_profile_type     *profile_p
)
{

  uint16  proto_info_index = 0;
  uint16  mcc = 0,mnc_digits = 0;
  boolean ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((profile_p == NULL) || 
      (protocol_config_options_p == NULL))
  {
    DS_3GPP_MSG3_HIGH("Profile Passed is Null",0,0,0);
    return ret_val;
  }

  if (profile_p->operator_reserved_pco < DSUMTS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN) 
  {
    DS_3GPP_MSG1_HIGH("Invalid OPERATOR RESERVED PCO id %d ",profile_p->operator_reserved_pco);
    return ret_val; 
  }

     /*-------------------------------------------------------------------------
      Get the protocol info index.
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    check if we have room to store this protocol info pkt 
  -------------------------------------------------------------------------*/
  if( (proto_info_index + 1 ) >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store OPER PCO pkts");
    return ret_val;
  }

  DS_3GPP_MSG3_HIGH("Fill OPER PCO params Mcc: %d Mnc: %d Operator Reserved Pco %d ",profile_p->mcc,profile_p->mnc.mnc_digits,profile_p->operator_reserved_pco);

  mcc=profile_p->mcc;
  mnc_digits=profile_p->mnc.mnc_digits;

  /*-------------------------------------------------------------------------
    Get the protocol info index.
  --------------------------------------------------------------------------*/

  proto_info_index =  protocol_config_options_p->num_protocol_ids;

    /*-------------------------------------------------------------------------
    Store the MSB of the protocol id field  
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
     protocol_id[0]  = ((profile_p->operator_reserved_pco & 0xff00) >> 8);


    /*-------------------------------------------------------------------------
    Store the LSB of the protocol id field  
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_id[1]  = 
   (profile_p->operator_reserved_pco & 0x00ff );

  ds_pdn_cntxt_fill_mcc_mnc_to_pco(protocol_config_options_p,mcc,mnc_digits,
                                   profile_p->mnc.mnc_includes_pcs_digit);


  protocol_config_options_p->valid = TRUE;
  protocol_config_options_p->num_protocol_ids ++;

  ret_val = TRUE;
  return ret_val;
}

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_OPER_PCO_INFO_FROM_APP

DESCRIPTION
  This function fills the operator specific protocol information in 
  the PDP context req. message. 
    
PARAMETERS   :
    protocol_config_options_p  - Ptr to config options 
    profile_p - Prt to the profile info    
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_cntxt_fill_oper_pco_info_from_app
(
  protocol_cfg_options_T       *protocol_config_options_p,
  void                         *user_data_to_query_pco
)
{

  uint16                          proto_info_index = 0;
  uint16                          mcc = 0,mnc_digits = 0;
  boolean                         ret_val = FALSE;
  ds_3gpp_op_pco_q_block         *op_pco_q_blk_ptr = NULL;
  ps_iface_op_reserved_pco_type  *op_pco_fill_ptr = NULL;
  uint8                           contents_len = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((protocol_config_options_p == NULL) ||
      (user_data_to_query_pco == NULL))
  {
    DS_3GPP_MSG2_HIGH("Input param passed is NULL:protol_config_ptr:0x%x:"
                   "profile:0x%x:user_data_ptr:0x%x",
                   protocol_config_options_p,user_data_to_query_pco);
    return ret_val;
  }

  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)user_data_to_query_pco;

  if ((op_pco_q_blk_ptr->op_pco_holder_ptr == NULL) || 
      (op_pco_q_blk_ptr->op_pco_holder_ptr->get_op_pco_f_ptr == NULL))
  {
    DS_3GPP_MSG1_ERROR("OP PCO holder:0x%x",
                       op_pco_q_blk_ptr->op_pco_holder_ptr);
    return ret_val;
  }

  /*---------------------------------------------------------------------- 
    Now invoke the fn ptr with the apn name and pass the ptr whose
    values will be filled by atp
    ----------------------------------------------------------------------*/
  op_pco_fill_ptr =(ps_iface_op_reserved_pco_type *)modem_mem_alloc
                   (sizeof(ps_iface_op_reserved_pco_type),
                    MODEM_MEM_CLIENT_DATA);

  if (op_pco_fill_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cannot allocate mem for op_pco_fill_ptr");
    return ret_val;
  }

  memset((void *)op_pco_fill_ptr,0,sizeof(ps_iface_op_reserved_pco_type));

  if (!op_pco_q_blk_ptr->op_pco_holder_ptr->get_op_pco_f_ptr
      (op_pco_q_blk_ptr->op_pco_holder_ptr->apn_name,op_pco_fill_ptr))
  {
    DS_3GPP_MSG0_ERROR("Failure to fill OP PCO FILL PTR");
    goto mem_free;
  }

  /*---------------------------------------------------------------------- 
    OP PCO fill ptr is successfully populated, Now populate the OP PCO
    contents from OP PCO fill ptr
    ----------------------------------------------------------------------*/

  if (op_pco_fill_ptr->container_id < DSUMTS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN) 
  {
    DS_3GPP_MSG1_HIGH("Invalid OPERATOR RESERVED PCO id %d ",
                      op_pco_fill_ptr->container_id);
    goto mem_free; 
  }

  if ((op_pco_fill_ptr->app_specific_info_len + MCC_MNC_LENGTH) > 
      MAX_PROTOCOL_ID_CONTENT )
  {
    DS_3GPP_MSG1_HIGH("APP specific len is: %d ",
                      op_pco_fill_ptr->app_specific_info_len);
    goto mem_free; 
  }



  DS_3GPP_MSG3_HIGH("Fill OPER PCO params Mcc: %d Mnc: %d Operator Reserved Pco %d ",
                    op_pco_fill_ptr->mcc,op_pco_fill_ptr->mnc
                    ,op_pco_fill_ptr->container_id);

  mcc=op_pco_fill_ptr->mcc;
  mnc_digits=op_pco_fill_ptr->mnc;
  
  /*-------------------------------------------------------------------------
    Get the protocol info index.
  --------------------------------------------------------------------------*/

  proto_info_index =  protocol_config_options_p->num_protocol_ids;

  /*-------------------------------------------------------------------------
    check if we have room to store this protocol info pkt 
  -------------------------------------------------------------------------*/
  if( (proto_info_index + 1 ) >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store OPER PCO pkts");
    goto mem_free;
  }

  /*-------------------------------------------------------------------------
    Store the MSB of the protocol id field  
  -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
     protocol_id[0]  = ((op_pco_fill_ptr->container_id & 0xff00) >> 8);


  /*-------------------------------------------------------------------------
    Store the LSB of the protocol id field  
  -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[1]  = (op_pco_fill_ptr->container_id & 0x00ff );

  ds_pdn_cntxt_fill_mcc_mnc_to_pco(protocol_config_options_p,mcc,mnc_digits,
                                   op_pco_fill_ptr->mnc_includes_pcs_digit);

  DS_3GPP_MSG1_HIGH("PCO contents length from APP:%d",op_pco_fill_ptr->
                    app_specific_info_len);
  /*------------------------------------------------------------------------- 
    Now MCC,MNC have been populated, Populate the contents
    ------------------------------------------------------------------------*/
  for(contents_len = MCC_MNC_LENGTH;contents_len < 
       (MCC_MNC_LENGTH + op_pco_fill_ptr->app_specific_info_len);contents_len++)
  {
	  protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[contents_len] = op_pco_fill_ptr->app_specific_info 
      [contents_len - MCC_MNC_LENGTH];

  }

  protocol_config_options_p->procol_info[proto_info_index].protocol_len
    = contents_len;
  protocol_config_options_p->valid = TRUE;
  protocol_config_options_p->num_protocol_ids++;

  ret_val = TRUE;

  mem_free:
    modem_mem_free((void *)op_pco_fill_ptr,MODEM_MEM_CLIENT_DATA);
    return ret_val;
}


/*===========================================================================
FUNCTION DS_PDN_CNTXT_CHECK_OP_PCO_COLLISION

DESCRIPTION
  This function checks if the op_res_pco id is configured to be the
  same in profiles and from the app.
    
PARAMETERS   :
    profile_p - Ptr to the profile info
    void        user_data_to_query_pco 
 
DEPENDENCIES
  None.
  
RETURN VALUE 
  TRUE - Successfully filled the operator info
  FALSE- Error
  
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_pdn_cntxt_check_op_pco_collision
(
  ds_umts_pdp_profile_type     *profile_p,
  void                         *user_data_to_query_pco
)
{
  boolean                         ret_val = FALSE;
  ps_iface_op_reserved_pco_type  *op_pco_fill_ptr = NULL;
  ds_3gpp_op_pco_q_block         *op_pco_q_blk_ptr = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if((user_data_to_query_pco == NULL) || (profile_p == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input parameters passed is NULL: App_op_pco_ptr:0x%x"
                       " Profile_p:0x%x",
                       user_data_to_query_pco,profile_p);
    return ret_val;
  }


  op_pco_q_blk_ptr = (ds_3gpp_op_pco_q_block *)user_data_to_query_pco;

  if ((op_pco_q_blk_ptr->op_pco_holder_ptr == NULL) || 
      (op_pco_q_blk_ptr->op_pco_holder_ptr->get_op_pco_f_ptr == NULL))
  {
    DS_3GPP_MSG1_ERROR("OP PCO holder:0x%x",
                       op_pco_q_blk_ptr->op_pco_holder_ptr);
    return ret_val;
  }

  /*---------------------------------------------------------------------- 
    Now invoke the fn ptr with the apn name and pass the ptr whose
    values will be filled by atp
    ----------------------------------------------------------------------*/
  op_pco_fill_ptr =(ps_iface_op_reserved_pco_type *)modem_mem_alloc
                   (sizeof(ps_iface_op_reserved_pco_type),
                    MODEM_MEM_CLIENT_DATA);

  if (op_pco_fill_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Cannot allocate mem for op_pco_fill_ptr");
    return ret_val;
  }

  memset((void *)op_pco_fill_ptr,0,sizeof(ps_iface_op_reserved_pco_type));

  if (!op_pco_q_blk_ptr->op_pco_holder_ptr->get_op_pco_f_ptr
      (op_pco_q_blk_ptr->op_pco_holder_ptr->apn_name,op_pco_fill_ptr))
  {
    DS_3GPP_MSG0_ERROR("Failure to fill OP PCO FILL PTR");
    goto mem_free;
  }

  /*---------------------------------------------------------------------- 
    OP PCO fill ptr is successfully populated, Now populate the OP PCO
    contents from OP PCO fill ptr
    ----------------------------------------------------------------------*/

  if (op_pco_fill_ptr->container_id < DSUMTS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN) 
  {
    DS_3GPP_MSG1_HIGH("Invalid OPERATOR RESERVED PCO id %d ",
                      op_pco_fill_ptr->container_id);
    goto mem_free; 
  }


  if(op_pco_fill_ptr->container_id == profile_p->operator_reserved_pco)
  {
    DS_3GPP_MSG1_HIGH("Op reserved PCO ID collision:%d",op_pco_fill_ptr->container_id);
    ret_val = TRUE;
  }

  mem_free:
    modem_mem_free((void *)op_pco_fill_ptr,MODEM_MEM_CLIENT_DATA);
    return ret_val;
}



/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_AUTH_PARAMS_FROM_INFO

DESCRIPTION
  This function fills the authentication information obtained from the info_ptr
  in the iface up cmd hdlr.
  
PARAMETERS 
  *protocol_config_options_p    - config option that has to be filled
  *profile_p,                   - PDP profile
  *ppp_info_p                   - info pointer
  subs_id                       - Subscription Id

                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Auth info not included in PDP context request.
  
SIDE EFFECTS 
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_auth_params_from_info
(
  protocol_cfg_options_T          *protocol_config_options_p,
  ds_umts_pdp_profile_type        *profile_p, 
  const network_params_info_type  *ppp_info_p,
  sys_modem_as_id_e_type           subs_id
)
{
  boolean result;
  ds_umts_pdp_auth_enum_type auth_type_to_use;

  if(profile_p == NULL || ppp_info_p == NULL)
    return DS3G_FAILURE;

  ds_3gpp_auth_mgr_get_auth_type
    (profile_p->context.apn,
    (ds_umts_pdp_auth_enum_type)ppp_info_p->auth_info.auth_type,
    &auth_type_to_use, subs_id);

  if(auth_type_to_use == DS_UMTS_PDP_AUTH_CHAP_OR_PAP) 
  {
    if(DS3G_PDN_AUTH_CHAP == ds3g_get_config_pdn_level_auth(subs_id))
    {
      auth_type_to_use = DS_UMTS_PDP_AUTH_CHAP;
    }
    else
    {
      auth_type_to_use = DS_UMTS_PDP_AUTH_PAP;
    }
  }

  switch (auth_type_to_use) 
  {
    case DS_UMTS_PDP_AUTH_NONE:
      DS_3GPP_MSG0_HIGH("No Auth params. None filled in PDP ctx req");
      /* Returning failure ensures that auth info is not sent out for eps */
      return DS3G_FAILURE;
      

    case DS_UMTS_PDP_AUTH_PAP:
      DS_3GPP_MSG0_HIGH("PAP Auth specfied: ");
      result = ds_pdn_cntxt_fill_pap_orig_parms 
                (
                 protocol_config_options_p,
                 (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                 (byte)ppp_info_p->auth_info.params.user_id_len,
                 (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                 (byte)ppp_info_p->auth_info.params.passwd_len
                );

      if(! result )
      {
        /*-------------------------------------------------------------------
          If an error occurs in encoding pkt, do not include the pkt but 
          proceed with call If we return error here, call setup would fail  
          and the user would'nt know why?  and all subsequent call attemtps 
          for the PDP ctxt would also fail (using the same params)
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("PAP Pkt gen failed: Ignoring Auth: ");
        return DS3G_FAILURE;
      }
      else
      { 
        /*-------------------------------------------------------------------
          If the authentication type, username and password are used from
          the ppp info, then store these parameters in the pdp profile
          params.
        -------------------------------------------------------------------*/
        (void)strlcpy((char*)profile_p->auth.username,
              (char*)ppp_info_p->auth_info.params.user_id_info,
                sizeof(profile_p->auth.username));

        (void)strlcpy((char*)profile_p->auth.password,
                (char*)ppp_info_p->auth_info.params.passwd_info,
                sizeof(profile_p->auth.password));
      }
      break;

    case DS_UMTS_PDP_AUTH_CHAP:

      DS_3GPP_MSG1_HIGH("CHAP Auth specfied: %d",
                ppp_info_p->auth_info.auth_type);
      /*lint -e655 */
      if ((ppp_info_p->valid_fields & NET_CFG_PRM_CHAP_CHAL_INFO_MASK)!= 0)
      /*lint +e655 */
      {     
          result = ds_pdn_cntxt_fill_chap_orig_params 
                 (
                  protocol_config_options_p,
                  (byte*)&ppp_info_p->auth_info.params.challenge_info[0],
                  (byte)ppp_info_p->auth_info.params.challenge_len,
                  (byte*)&ppp_info_p->auth_info.params.challenge_name[0],
                  (byte)ppp_info_p->auth_info.params.challenge_name_len,
                  (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                  (byte)ppp_info_p->auth_info.params.passwd_len,
                  (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                  (byte)ppp_info_p->auth_info.params.user_id_len,
                  (byte)ppp_info_p->auth_info.auth_id
                 );
      }
      else
      {
        result = ds_pdn_cntxt_gen_and_fill_chap_orig_parms 
                   (
                    protocol_config_options_p,
                    (byte*)&ppp_info_p->auth_info.params.user_id_info[0],
                    (byte)ppp_info_p->auth_info.params.user_id_len,
                    (byte*)&ppp_info_p->auth_info.params.passwd_info[0],
                    (byte)ppp_info_p->auth_info.params.passwd_len
                   );
      }

      if(! result )
      {
        /*-------------------------------------------------------------------
          If an error occurs in encoding pkt, do not include the pkt but
          proceed with call If we return error here, call setup would fail
          and the user would'nt know why?  and all subsequent call attemtps
          for the PDP ctxt would also fail (using the same params)
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR("CHAP Pkt gen failed: Ignoring Auth: ");
        return DS3G_FAILURE;
      }
      else
      {
        /*-------------------------------------------------------------------
         If the authentication type, username and password are used from
         the ppp info, then store these parameters in the pdp profile
         params.
        -------------------------------------------------------------------*/
        (void)strlcpy((char*)profile_p->auth.username,
                (char*)ppp_info_p->auth_info.params.user_id_info,
                sizeof(profile_p->auth.username));

        (void)strlcpy((char*)profile_p->auth.password,
                (char*)ppp_info_p->auth_info.params.passwd_info,
                sizeof(profile_p->auth.password));
      }
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid Auth type :%d", profile_p->auth.auth_type);
      return DS3G_FAILURE;
  }
  return DS3G_SUCCESS;
}

/*===========================================================================
FUNCTION DS_PDN_CNTXT_FILL_AUTH_PARAMS_FROM_PROFILE

DESCRIPTION
  This function fills the authentication information obtained from profile
  in the PDP context request message.
  
PARAMETERS 
  *protocol_config_options_p    - Config option that has to be filled
  *profile_p,                   - PDP profile
   subs_id                      - Subscription id
 
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Auth info successfully included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
    
===========================================================================*/
boolean ds_pdn_cntxt_fill_auth_params_from_profile
(
  protocol_cfg_options_T           *protocol_config_options_p,
  const ds_umts_pdp_profile_type   *profile_p,
  sys_modem_as_id_e_type            subs_id
)
{
  boolean result;
  ds_umts_pdp_auth_enum_type auth_type_to_use;

  if(profile_p == NULL)
    return DS3G_FAILURE;

  ds_3gpp_auth_mgr_get_auth_type(profile_p->context.apn,
                                 profile_p->auth.auth_type,
                                 &auth_type_to_use,
                                 subs_id);
  if(auth_type_to_use == DS_UMTS_PDP_AUTH_CHAP_OR_PAP) 
  {
    if(DS3G_PDN_AUTH_CHAP == ds3g_get_config_pdn_level_auth(subs_id))
    {
      auth_type_to_use = DS_UMTS_PDP_AUTH_CHAP;
    }
    else
    {
      auth_type_to_use = DS_UMTS_PDP_AUTH_PAP;
    }
  }

  switch (auth_type_to_use) 
  {
    case DS_UMTS_PDP_AUTH_NONE:
      DS_3GPP_MSG0_HIGH("No Auth params. None filled in PDP ctx req");
      /* Returning failure will ensure that auth info is not sent out in eps
       */
      return DS3G_FAILURE;

    case DS_UMTS_PDP_AUTH_PAP:
      DS_3GPP_MSG0_HIGH("PAP Auth specfied: ");
      result = ds_pdn_cntxt_fill_pap_orig_parms
                (
                  protocol_config_options_p,
                  (byte*) profile_p->auth.username,
                  (byte)strlen((char*)profile_p->auth.username),
                  (byte*) profile_p->auth.password,
                  (byte)strlen((char*)profile_p->auth.password)
                );

      if(! result )
      {
        /*-------------------------------------------------------------------
          If an error occurs in encoding pkt, do not include the pkt but 
          proceed with call If we return error here, call setup would fail  
          and the user would'nt know why?  and all subsequent call attemtps 
          for the PDP ctxt would also fail (using the same params)
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_ERROR("PAP Pkt gen failed: Ignoring Auth: ");
        return DS3G_FAILURE;
      }
      break;

    case DS_UMTS_PDP_AUTH_CHAP:

      DS_3GPP_MSG1_HIGH("CHAP Auth specfied: %d",
                profile_p->auth.auth_type);
      result = ds_pdn_cntxt_gen_and_fill_chap_orig_parms
                 (
                  protocol_config_options_p,
                  (byte*) profile_p->auth.username,
                  (byte)strlen((char*)profile_p->auth.username),
                  (byte*) profile_p->auth.password,
                  (byte)strlen((char*)profile_p->auth.password)
                 );

      if(! result )
      {
        /*-------------------------------------------------------------------
          If an error occurs in encoding pkt, do not include the pkt but
          proceed with call If we return error here, call setup would fail
          and the user would'nt know why?  and all subsequent call attemtps
          for the PDP ctxt would also fail (using the same params)
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("CHAP Pkt gen failed: Ignoring Auth: ");
        return DS3G_FAILURE;
      }
      break;

    default:
      DS_3GPP_MSG1_ERROR("Invalid Auth type :%d", profile_p->auth.auth_type);
      return DS3G_FAILURE;
  }/* switch */
  return DS3G_SUCCESS;
}

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPCP_ORIG_PARAMS

DESCRIPTION
  This function fills the IPCP (Primary and Secondary DNS only) specific 
  protocol information in the PDP context request message.
  
PARAMETERS 
    *orig_params_ptr     - Ptr to PDN origination parameters 
    req_spec_dns_flg     - IS dns address requested
    primary_dns          - primary dns address requested
    secondary_dns        - secondary dns address requested
    req_spec_nbns_flg    - IS nbns address requested
    primary_nbns         - primary WINS address
    secondary_nbns       - secondary WINS address
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPCP packet generated succesfully and included in PDP context request
         message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of PPP packets included by this function
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_ipcp_orig_params
(
  protocol_cfg_options_T       *protocol_config_options_p,
  boolean                       req_spec_dns_flg,/* Should spec DNS add req*/
  uint32                        primary_dns,    /* primary DNS             */
  uint32                        secondary_dns,  /* secondary DNS           */
  boolean                       req_spec_nbns_flg,/*Should spec NBNS add req*/
  uint32                        primary_nbns,   /* primary WINS            */
  uint32                        secondary_nbns, /* secondary WINS          */
  net_cfg_params_mask_e_type    valid_fields    /* IPCP fileds to request */
)
{
  int    ipcp_pkt_len=0;                          /* Length of IPCP packet */
  uint16 proto_info_index;
  uint32 p_dns_req,s_dns_req,p_nbns_req, s_nbns_req;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Should we ask for the DNS addr specified or the values 0 implying that
    net should assin us one. If laptop specified a specific DNS addr in
    ipcp, we should include it, but for socks call, we always get it from
    the network first
  -------------------------------------------------------------------------*/
  if ( ! req_spec_dns_flg )
  {
    p_dns_req = s_dns_req = 0x0;
  }
  else
  {
    p_dns_req = primary_dns;
    s_dns_req = secondary_dns;
  }

 
  /*-------------------------------------------------------------------------
    Should we ask for the NBNS addr specified or the values 0 implying that
    net should assin us one. If laptop specified a specific NBNS addr in
    ipcp, we should include it, but for socks call, we always get it from
    the network first-- Need to remove it for socket calls
  -------------------------------------------------------------------------*/
  if ( ! req_spec_nbns_flg )
  {
    p_nbns_req = s_nbns_req = 0x0;
  }
  else
  {
    p_nbns_req = primary_nbns;
    s_nbns_req = secondary_nbns;
  }
 
  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store DNS pkt");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Encode a IPCP packet with Primary and Secondary DNS and NBNS request 
    options and include it in protocol options field.
  -------------------------------------------------------------------------*/
  ipcp_pkt_len= dsumtsps_gen_ipcp_req_pkt(
    &protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    0,
    p_dns_req,
    s_dns_req,
    p_nbns_req,
    s_nbns_req,
    valid_fields
  );
  
  if(ipcp_pkt_len < 0 )
  {
    DS_3GPP_MSG0_ERROR("Error generating IPCP packet");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Update the protocol info fields - defined in sm_common.h
    Protocol ID = 0x8021  for IPCP
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*-------------------------------------------------------------------------
    Increment the number of protocol ids by 1.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->num_protocol_ids ++;

  /*-------------------------------------------------------------------------
    Store the MSB of the protocol id field - 80 
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ((PPP_IPCP_PROTOCOL & 0xff00) >> 8);

  /*-------------------------------------------------------------------------
    Store the LSB of the protocol id field - 21 
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = (PPP_IPCP_PROTOCOL & 0x00ff );

  /*-------------------------------------------------------------------------
    Store the length of the protocol identifier contents
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len    = (uint8)ipcp_pkt_len;
   
  /*-----------------------------------------------------------------------
    Increment the protocol info index in the call record. 
    This index indicates how many PPP packets have been filled in the 
    PDP context request message so far
  -----------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  DS_3GPP_MSG0_HIGH("Included IPCP cfg req pkt in PDP ctx req");
  return TRUE;

} /* ds_pdn_cntxt_fill_ipcp_orig_params() */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FILL_IPV4_DNS_CONTAINER

DESCRIPTION
  This function appends DNS server Ipv4 address request container to the 
  PCO buffer. 
  
PARAMETERS   
  protocol_cfg_options_T       *protocol_config_options_p
    
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV4 DNS packet container appended succesfully
  FALSE- Error

SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_fill_ipv4_dns_container
(
  protocol_cfg_options_T       *protocol_config_options_p
)
{
  uint16 proto_info_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;  

  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room to store DNS server ipv4 address request container in PCO");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_support,
    if mode is not LTE. For LTE always fill this container.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*---------------------------------------------------------------------
   Store the MSB of the container id field
  ---------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_id[0]
            = ((IPV4_DNS_CONTAIN_ID & 0xff00) >> 8);

  /*---------------------------------------------------------------------
   Store the LSB of the container id field
  ---------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_id[1]  
            = (IPV4_DNS_CONTAIN_ID & 0x00ff );

  /*-----------------------------------------------------------------------
   Since IPV4 DNS container identifier contents should be empty and 
   Container length should be 0, no packet generation is needed. Fill the 
   length of container identifier contents to be 0
  ---------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_len
            = 0;

  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  DS_3GPP_MSG0_HIGH("Included IPV4 DNS container req pkt in orig params");
  return TRUE;
}

#ifdef FEATURE_DATA_PS_IPV6

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV6_DNS_PARAMS

DESCRIPTION
  This function fills the IPV6 (DNS only) specific protocol information 
  in the PDP context request message.
  
PARAMETERS 
    protocol_cfg_options_T       *protocol_config_options_p
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - IPV6 DNS packet generated succesfully and included in PDP context 
  request message
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
boolean ds_pdn_cntxt_fill_ipv6_dns_params
(
  protocol_cfg_options_T       *protocol_config_options_p
)
{
  int    ipv6_dns_container_len=0; /* Length of IPV6 DNS Container Length */
  uint16 proto_info_index;
  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store IPV6 DNS pkt");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Encode Protocol Config Options  packet with IPV6 Primary and Secondary 
    DNS  request 
  -------------------------------------------------------------------------*/
  ipv6_dns_container_len = dsumtsps_gen_ipv6_dns_req_pkt(
    &protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[0]);
  
  if( ipv6_dns_container_len < 0 )
  {
    DS_3GPP_MSG0_ERROR("Error generating IPv6 DNS packet");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*-------------------------------------------------------------------------
    Increment the number of protocol ids by 1.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->num_protocol_ids ++;

  /*-------------------------------------------------------------------------
    Store the MSB of the protocol id field - 0x00 
  -------------------------------------------------------------------------*/  
  /*lint -e572 */
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ((IPV6_DNS_CONTAIN_ID & 0xff00) >> 8);
  /*lint +e572 */

  /*-------------------------------------------------------------------------
    Store the LSB of the protocol id field - 03 
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = (IPV6_DNS_CONTAIN_ID & 0x00ff );

  /*-------------------------------------------------------------------------
    Store the length of the protocol identifier contents
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len    = (uint8)ipv6_dns_container_len;
   
  /*-----------------------------------------------------------------------
    Increment the protocol info index in the call record. 
    This index indicates how many  packets have been filled in the 
    PDP context request message so far
  -----------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  DS_3GPP_MSG0_HIGH("Included IPV6 DNS container req pkt in PDP ctx req");
  return TRUE;

} /* DSPDNCNTX_FILL_IPV6_DNS_PARAMS() */

#endif /* FEATURE_DATA_PS_IPV6 */


/*===========================================================================
FUNCTION DSPDNCNTX_FILL_ADDRESS_ALLOC_PARAM

DESCRIPTION
  This function fills the IPV4 address allocation container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p           - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  TRUE - Successfully filled IPv4 Address allocation in PCO
  FALSE- Error
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_address_alloc_param
(
  protocol_cfg_options_T    *protocol_config_options_p,
  ds_umts_pdp_profile_type  *profile_p
)
{
  uint16 proto_info_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store IPV6 DNS pkt");
    return;
  }

  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*-------------------------------------------------------------------------
    Increment the number of protocol ids by 1.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->num_protocol_ids++;

  

  if (profile_p->context.ipv4_addr_alloc == DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4)
  {
    /*-------------------------------------------------------------------------
      Store the MSB of the protocol id field - 0x00 
    -------------------------------------------------------------------------*/  
    /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[0]  = ((IPV4_ADDR_ALLOC_DHCPV4_CONTAINER_ID & 0xff00) >> 8);
    /*lint +e572 */
    /*-------------------------------------------------------------------------
      Store the LSB of the protocol id field - 0x0B
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[1]  = (IPV4_ADDR_ALLOC_DHCPV4_CONTAINER_ID & 0x00ff );
    DS_3GPP_MSG0_HIGH("Filling addr alloc via dhcpv4 in PDP ctx req");
  }
  else if (profile_p->context.ipv4_addr_alloc == DS_UMTS_PDP_IPV4_ADDR_ALLOC_NAS)
  {
    /*-------------------------------------------------------------------------
      Store the MSB of the protocol id field - 0x00 
    -------------------------------------------------------------------------*/  
    /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[0]  = ((IP_ADDR_ALLOC_NAS_CONTAINER_ID & 0xff00) >> 8);
    /*lint +e572 */
  
    /*-------------------------------------------------------------------------
      Store the LSB of the protocol id field - 0x0A
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[1]  = (IP_ADDR_ALLOC_NAS_CONTAINER_ID & 0x00ff );
    DS_3GPP_MSG0_HIGH("Filling addr alloc via NAS in PDP ctx req");
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid ip addr alloc field in profile");
    protocol_config_options_p->num_protocol_ids--;
    return;
  }


  /*-------------------------------------------------------------------------
    Store the length of the protocol identifier contents
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
  
  return;

} /* ds_pdn_cntxt_fill_address_alloc_via_dhcpv4_param() */

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV4V6_LINK_MTU_CONTAINER

DESCRIPTION
  This function fills the IPV4V6 mtu PCO container
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
    *profile_p                 Ptr to ds profile
    subs_id                    Subscription Id
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_ipv4v6_link_mtu_container
(
  protocol_cfg_options_T    *protocol_config_options_p,
  ds_umts_pdp_profile_type  *profile_p,
  sys_modem_as_id_e_type     subs_id
)
{
  
  uint8                rel_version;
  uint16               proto_info_index;
  sys_sys_mode_e_type  mode = SYS_SYS_MODE_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store IPV4/IPV6 MTU");
    return;
  }

  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_version for non-LTE and 
    LTE_REL version (lte_3gpp_release_ver) for LTE sys mode.
  -------------------------------------------------------------------------*/
  rel_version = ds_3gpp_nv_manager_get_3gpp_rel_version();
  mode = ds3gpp_get_current_network_mode(subs_id);

  if (( ( mode != SYS_SYS_MODE_LTE )&&( rel_version < (uint8)DS_3GPP_REL10 ))
#ifdef FEATURE_DATA_LTE
        || (( mode == SYS_SYS_MODE_LTE )&& 
           (ds_3gpp_nv_manager_get_nv_lte_rel_version() < (uint8)LTE_3GPP_REL10))
#endif /* FEATURE_DATA_LTE */
    )
  {
    DS_3GPP_MSG0_ERROR("Release version is not Rel:10, not filling IPV4/IPV6 "
                       "link MTU container"); 
    return;
  }

  /*-------------------------------------------------------------------------
    Fill IPV4 MTU only if profile pdp_type is IPV4 or IPV4V6
  --------------------------------------------------------------------------*/
  if (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4 || 
      profile_p->context.pdp_type == DS_UMTS_PDP_IPV6 ||
      profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    /*-------------------------------------------------------------------------
      Indicates if the option is valid or not.
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->valid = TRUE;

    /*-------------------------------------------------------------------------
      Increment the number of protocol ids by 1.
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->num_protocol_ids++;

    /*-------------------------------------------------------------------------
      Store the MSB of the protocol id field - 0x00 
    -------------------------------------------------------------------------*/  
    /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[0]  = ((IPV4_LINK_MTU_CONTAINER_ID & 0xff00) >> 8);
    /*lint +e572 */
    /*-------------------------------------------------------------------------
      Store the LSB of the protocol id field - 0x10
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[1]  = (IPV4_LINK_MTU_CONTAINER_ID & 0x00ff );
    DS_3GPP_MSG0_HIGH("Filling IPV4/IPV6 Link MTU container in PDP ctx req");
  
    /*-------------------------------------------------------------------------
      Store the length of the protocol identifier contents
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Profile pdp_type is not IPV4/IPV6 or IPV4V6, not filling "
                       "IPV4/IPV6 link MTU container");
  }

  return;
}

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_IPV4V6_LOCAL_ADDRESS_SUPPORT_CONTAINER

DESCRIPTION
  This function fills the local address support in TFT indicator
  
PARAMETERS 
    protocol_cfg_options_T -  *protocol_config_options_p 
    *profile_p             - Ptr to ds profile
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  None.
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_ipv4v6_local_address_support_container
(
  protocol_cfg_options_T    *protocol_config_options_p,
  ds_umts_pdp_profile_type  *profile_p
)
{
  uint16 proto_info_index;

  if (FALSE == ds_3gppi_is_rel_11_supported())
  {
    DS_3GPP_MSG0_ERROR("Rel11 is not supported not filling local address support");
    return;
  }
	
  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store local address support");
    return;
  }

   /*-------------------------------------------------------------------------
    Fill IPV4 MTU only if profile pdp_type is IPV4 or IPV4V6
  --------------------------------------------------------------------------*/
  if (profile_p->context.pdp_type == DS_UMTS_PDP_IPV4 || 
      profile_p->context.pdp_type == DS_UMTS_PDP_IPV6 ||
      profile_p->context.pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    /*-------------------------------------------------------------------------
      Indicates if the option is valid or not.
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->valid = TRUE;

    /*-------------------------------------------------------------------------
      Increment the number of protocol ids by 1.
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->num_protocol_ids++;

    /*-------------------------------------------------------------------------
      Store the MSB of the protocol id field - 0x00 
    -------------------------------------------------------------------------*/  
    /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[0]  = 
           ((LOCAL_ADDRESS_SUPPORT_IN_TFT_CONTAINER_ID & 0xff00) >> 8);
    /*lint +e572 */
    /*-------------------------------------------------------------------------
      Store the LSB of the protocol id field - 0x11
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].
      protocol_id[1]  = 
           (LOCAL_ADDRESS_SUPPORT_IN_TFT_CONTAINER_ID & 0x00ff );
    DS_3GPP_MSG0_HIGH("Filling Local Address support in TFT container");
  
    /*-------------------------------------------------------------------------
      Store the length of the protocol identifier contents
    -------------------------------------------------------------------------*/  
    protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Profile pdp_type is not IPV4/IPV6 or IPV4V6, not filling "
                       "Local address support in TFT container");
  }

  return;  
}

/*===========================================================================
FUNCTION DSPDNCNTX_FILL_BCM_REQUEST

DESCRIPTION
  This function fills the BCM container in PCO
  
PARAMETERS 
    protocol_cfg_options_T    *protocol_config_options_p 
                               
DEPENDENCIES 
  None.

RETURN VALUE  
  none
  
SIDE EFFECTS 
  Updates Orig_params contents : 
     orig_params_ptr->protocol_config_options.num_protocol_ids will be updated
     to reflect the no. of protocol config options included by this function
     
===========================================================================*/
void ds_pdn_cntxt_fill_bcm_indication
(
  protocol_cfg_options_T    *protocol_config_options_p
)
{
  uint16 proto_info_index;

  if(protocol_config_options_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL pco pointer passed!");
    return;
  }

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;
  
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to fill BCM info");
    return;
  }

  /*-------------------------------------------------------------------------
    Fill BCM parameter for rel version higher than rel6 and if umts_nw_init_qos 
    NV is enabeled
  --------------------------------------------------------------------------*/
  if( (ds_3gpp_nv_manager_get_umts_nw_init_qos_support_nv() != DS_3GPP_NV_ENABLED)|| 
      (ds_3gpp_nv_manager_get_3gpp_rel_version() <= (uint8)DS_3GPP_REL6))
  {
    DS_3GPP_MSG0_ERROR("umts_nw_init_qos NV not enabled or rel ver <=rel6"
                       " not filling BCM info");
    return;
  }
  /*-------------------------------------------------------------------------
    Indicates if the option is valid or not.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->valid = TRUE;

  /*-------------------------------------------------------------------------
    Increment the number of protocol ids by 1.
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->num_protocol_ids++;

  

  /*-------------------------------------------------------------------------
    Store the MSB of the protocol id field - 0x00 
  -------------------------------------------------------------------------*/  
  /*lint -e572 */
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ((BEARER_CONTROL_MODE_CONTAINER_ID & 0xff00) >> 8);
  /*lint +e572 */
  /*-------------------------------------------------------------------------
    Store the LSB of the protocol id field - 0x05
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = (BEARER_CONTROL_MODE_CONTAINER_ID & 0x00ff );
  DS_3GPP_MSG3_HIGH("Filling BCM container in PDP ctx req",0,0,0);


  /*-------------------------------------------------------------------------
    Store the length of the protocol identifier contents
  -------------------------------------------------------------------------*/  
  protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
  
  return;

} /* ds_pdn_cntxt_fill_bcm_indication() */



/*===========================================================================
FUNCTION DSPDNCNTX_FILL_PAP_ORIG_PARMS

DESCRIPTION
  This function fills the PAP specific protocol information in the PDP context
  req. message

PARAMETERS   :
    protocol_cfg_options_T *protocol_config_options_p 
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  Updates Orig_params contents :
  orig_params_ptr->config_options.num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function

===========================================================================*/
boolean ds_pdn_cntxt_fill_pap_orig_parms
(
  protocol_cfg_options_T *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                         password_len
)
{
  int   pap_pkt_len=0;               /* Length of PAP packet               */
  uint16 proto_info_index;           /* Index into protocol config options */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Get the protocol info index. This index is used to fill in PPP packets
   in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index = protocol_config_options_p->num_protocol_ids;

  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp ctx req
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store PAP pkt");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Encode a PAP packet based on user specified username & password and
    include it in protocol options field
  -------------------------------------------------------------------------*/
  pap_pkt_len= dsumtsps_ppp_gen_pap_auth_req_pkt(
    &protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    0,
    username,
    user_id_len,
    password,
    password_len
   );

  if(pap_pkt_len < 0 )
  {
    DS_3GPP_MSG0_ERROR("Error generating PAP packet:Not including PAP pkt");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Update the protocol_info fields
  -------------------------------------------------------------------------*/
  protocol_config_options_p->valid = TRUE;
  protocol_config_options_p->num_protocol_ids ++;
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ((PPP_PAP_PROTOCOL & 0xff00) >> 8);
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = (PPP_PAP_PROTOCOL & 0x00ff );
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len    = (uint8)pap_pkt_len;

  /*-------------------------------------------------------------------------
    Increment the protocol info index in the call record.
    This index indicates how many PPP packets have been filled inthe
    PDP context req. msg so far
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;
  DS_3GPP_MSG0_HIGH("Included PAP pkt in PDP ctx req");
  return TRUE;

} /* ds_pdn_cntxt_fill_pap_orig_parms() */


/*===========================================================================
FUNCTION DSPDNCNTX_GEN_AND_FILL_CHAP_ORIG_PARMS

DESCRIPTION
  This function generates and fills the CHAP specific protocol information in
  the PDP context req. message. The chap challenge and response are generated
  internally.

PARAMETERS   :
    protocol_cfg_options_T       *protocol_config_options_p
    *username            - Ptr to User id
    user_id_len          - Length of User id
    *password            - Ptr to password
    password_len         - Length of password

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - PAP packet generated succesfully and included in PDP ctx req msg
  FALSE- Error

SIDE EFFECTS
  Updates Orig_params contents :
  orig_params_ptr->config_options.num_protocol_ids will be updated
  to reflect the no. of PPP packets included by this function

===========================================================================*/
boolean ds_pdn_cntxt_gen_and_fill_chap_orig_parms
(
  protocol_cfg_options_T       *protocol_config_options_p,
  const byte                   *username,
  byte                         user_id_len,
  const byte                   *password,
  byte                        password_len
)
{
  int     chap_chal_pkt_len,chap_res_pkt_len;
  uint32  dsrand,i;
  uint16    proto_info_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("GEN & Fill CHAP params from profile ");

  /*-------------------------------------------------------------------------
   Get the protocol info index. This index is used to fill in PPP packets
   in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index = protocol_config_options_p->num_protocol_ids;

  /*-------------------------------------------------------------------------
    check if we have room to store this protocol info pkt in the pdp ctx req
    we need room to store 2 pkts : chap challenge & response
  -------------------------------------------------------------------------*/
  if( (proto_info_index + 2 ) >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store CHAP pkts");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Generate a random stream to be used as the challenge., Use the
    32 bit clock tick count repeatedly to form a challenge pattern
  -------------------------------------------------------------------------*/
  dsrand = msclock();
  for(i=0; i < CHAP_CHALLENGE_LEN - 3 ; i+=4 )
  {
    chap_challenge_buffer[i]   = (uint8)( dsrand        & 0xff);
    chap_challenge_buffer[i+1] = (uint8)( (dsrand >>8)  & 0xff); 
    chap_challenge_buffer[i+2] = (uint8)( (dsrand >>16) & 0xff);
    chap_challenge_buffer[i+3] = (uint8)( (dsrand >>16) & 0xff);
    DS_3GPP_MSG2_HIGH("challenge :%d val for 4 bytes :%d",i,dsrand);
  } 
  /*-------------------------------------------------------------------------
    Construct the challenge packet. Use pre-provisioned parameters
    from atcop and locally generated Challenge & system name
  -------------------------------------------------------------------------*/
  chap_chal_pkt_len = dsumtsps_ppp_gen_chap_challenge_pkt(
    &protocol_config_options_p->procol_info[proto_info_index].
     protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    0,
    chap_challenge_buffer,
    CHAP_CHALLENGE_LEN,
    (byte *)username,
    (byte)strlen((char*)username));

  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing and
    return an error
  -------------------------------------------------------------------------*/
  if( chap_chal_pkt_len < 0 )
  {
    DS_3GPP_MSG1_ERROR("CHAP pkt gen fail C:%d ",chap_chal_pkt_len);
    return FALSE;
  }

  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0] = ( (PPP_CHAP_PROTOCOL & 0xff00) >> 8);
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1] = ( (PPP_CHAP_PROTOCOL & 0x00ff) );
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len = (uint8)chap_chal_pkt_len;

  /*-------------------------------------------------------------------------
    Increment the protocol info index.
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  /*-------------------------------------------------------------------------
    Construct the response packet. Use pre-provisioned parameters
    from atcop and locally generated Challenge & system name
  -------------------------------------------------------------------------*/
  chap_res_pkt_len = dsumtsps_ppp_gen_chap_response_pkt(
    &protocol_config_options_p->procol_info[proto_info_index].
      protocol_contents[0],
    MAX_PROTOCOL_ID_CONTENT,
    0,
    chap_challenge_buffer,
    CHAP_CHALLENGE_LEN,
    password,
    password_len,
    username,
    user_id_len
    );
  
  /*-------------------------------------------------------------------------
    If an error occurs in encoding pkt, abort further processing.
    Adjust the protocol_info_index : remove the chap challenge
    packet which has been included.
  -------------------------------------------------------------------------*/
  if( chap_res_pkt_len <0 )
  {
    DS_3GPP_MSG1_ERROR("CHAP pkt gen fail  R:%d",chap_res_pkt_len);
    protocol_config_options_p->num_protocol_ids --; 
    return FALSE;
  }
  
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0]  = ( (PPP_CHAP_PROTOCOL & 0xff00) >> 8);
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1]  = ( (PPP_CHAP_PROTOCOL & 0x00ff) );
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len = (uint8)chap_res_pkt_len;    
  DS_3GPP_MSG2_HIGH("Chal len :%d res len :%d",chap_chal_pkt_len,chap_res_pkt_len);
    
  /*-------------------------------------------------------------------------
    Increment the protocol info index. 
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;
  protocol_config_options_p->valid            = TRUE;
  return TRUE;

} /* ds_pdn_cntxt_gen_and_fill_chap_orig_parms() */

/*===========================================================================
FUNCTION      ds_pdn_cntxt_compute_ethernet_header

DESCRIPTION
  This function computes the ethernet header that needs to be attached 
  for rmnet calls operating in ethernet mode

PARAMETERS
  target_iface_ptr   -  pointer to Rm iface 
  header_ptr         -  Stores the computed header
  result             -  result of operation 0 success, -1 failure

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_context_decode_bcm
(
  const byte       *protocol_pkt_ptr, /* Pointer to the buffer for bcm info */
  int               protocol_pkt_len,  /* Length of the bcm container */
  ds_pdn_context_s *pdn_context_ptr
)
{
  ds_flow_manager_s * flow_manager_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
  {
    return FALSE;
  }

  if(protocol_pkt_len != 1)
  {
    DS_3GPP_MSG0_HIGH("Invalid length for BCM container");
    return FALSE;
  }

  flow_manager_ptr = ds_flow_manager_find_flow_manager(pdn_context_ptr);

  if(!ds_flow_manager_validate_flow_manager(flow_manager_ptr))
  {
    return FALSE;
  }
  
  switch(*protocol_pkt_ptr)
  {
    case 0x1:
      flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = 
        DS_3GPP_BCM_UE_ONLY;
      break;
    case 0x2:
      flow_manager_ptr->ds_flow_manager_dyn_p->bcm_info = 
        DS_3GPP_BCM_UE_NW;
      break;
    default:
      DS_3GPP_MSG0_HIGH("Invalid BCM info, ignored");
      break;
  }
  return TRUE;
}

/*===========================================================================
FUNCTION      ds_pdn_context_this_iface_is_for_emergency_pdn

DESCRIPTION
  This function checks if the passed in 3gpp Iface has been reserved for
  connections to the emergency PDN.

PARAMETERS
  iface_p   -  pointer to the 3gpp Iface.
 
DEPENDENCIES  None.

RETURN VALUE  TRUE if passed in 3gpp Iface is reserved for emergency pdn connections. 
              FALSE otherwise. 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_pdn_context_this_iface_is_for_emergency_pdn(ds_3gpp_iface_s *iface_p)
{
  boolean retVal = FALSE;
  #ifdef FEATURE_DATA_DISABLE_MULTI_PDN
     return retVal;
  #else
  #ifdef FEATURE_EMERGENCY_PDN
  if ( ((&ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V4_PS_IFACE_IDX]) == iface_p)
    #ifdef FEATURE_DATA_PS_IPV6
        ||
       ((&ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V6_PS_IFACE_IDX]) == iface_p)
    #endif /* FEATURE_DATA_PS_IPV6 */
       )
  {
    retVal = TRUE;
  }
  #else
  DS_3GPP_MSG0_HIGH("_iface_is_not_for_Emergency_PDN, feature not supported");
  retVal = FALSE;
  #endif /* FEATURE_EMERGENCY_PDN */
  #endif /* FEATURE_DATA_DISABLE_MULTI_PDN */
  return retVal;
}

/*===========================================================================
FUNCTION      ds_pdn_cntxt_decode_ip4v6_link_mtu

DESCRIPTION
  This function decodes the ipv4 link mtu size from the PCO.

PARAMETERS
  protocol_pkt_ptr   -  pointer to the PCO contents field
  protocol_pkt_len   -  container length
  pdn_context_p      -  Pointer to current PDN context
  v4_iface           -  v4_iface of the PDN
  v6_iface           -  v6_iface of the PDN
  decode_mask        -  Mask pointer containing status of decode operation

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
static void ds_pdn_context_decode_ipv4v6_link_mtu
(
  const byte           *protocol_pkt_ptr, /* Pointer to the buffer forMTU info */
  byte                  protocol_pkt_len,  /* Length of the MTU container */
  ds_pdn_context_s     *pdn_cntxt_p,
  ps_iface_type        *v4_iface,
  ps_iface_type        *v6_iface,
  uint32               *decode_mask_ptr
)
{
  uint16                 mtu_len = 0;
  uint16                 rel_version;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*------------------------------------------------------------------------*/

  if ((NULL == v4_iface && NULL == v6_iface) ||
      (NULL == decode_mask_ptr))
  {
    DS_3GPP_MSG0_ERROR("Invalid input ptrs, Exiting !!!!");
    return;
  }

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntxt_p, &subs_id)== FALSE)
  {
    DS_3GPP_MSG1_ERROR("PDN context could not fetch valid Subs id(CM) %d",
                       subs_id);
    return;
  }

  /*-------------------------------------------------------------------------
    If IPV4 Link MTU container length is not 2, then the link MTU content 
    should be ignored. Ref: TS24.008 sec 10.5.6.3
  --------------------------------------------------------------------------*/
  if (protocol_pkt_len != IPV4_LINK_MTU_CONTAINER_LEN)
  {
    DS_3GPP_MSG1_ERROR("IPV4V6 Link MTU container length is: %d. Ignoring MTU", 
                        protocol_pkt_len);
    return;
  }
  /*-------------------------------------------------------------------------
    Check if we have to decode this protocol info based on NV item
    3gpp_rel_version for non-LTE and LTE_REL version (lte_3gpp_release_ver)
    for LTE sys mode.
  -------------------------------------------------------------------------*/
  rel_version = ds_3gpp_nv_manager_get_3gpp_rel_version();

  if (( ( ds3gpp_get_current_network_mode(subs_id) != SYS_SYS_MODE_LTE ) &&
        ( rel_version < (uint8)DS_3GPP_REL10 ) )
#ifdef FEATURE_DATA_LTE
        || (( ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_LTE )&&
            (ds_3gpp_nv_manager_get_nv_lte_rel_version() < LTE_3GPP_REL10 ))
#endif /* FEATURE_DATA_LTE */
    )
  {
    DS_3GPP_MSG0_ERROR("Release version is not Rel:10, not decoding IPV4/IPV6 "
                       "link MTU container"); 
    return;
  }

  /*---------------------------------------------------------------------------
    The container identifier contents field contains the binary coded 
    representation of the IPv4 link MTU size in octets. Bit 8 of the first 
    octet of the container identifier contents field contains the most 
    significant bit and bit 1 of the second octet of the container identifier 
    contents field contains the least significant bit. 
    Ref: TS24.008 sec 10.5.6.3
  ----------------------------------------------------------------------------*/
  mtu_len = (uint16)(protocol_pkt_ptr[0] << 8) | (uint16)(protocol_pkt_ptr[1]);

  if(MAX_SYSTEM_MTU < mtu_len)
  {
    DS_3GPP_MSG2_ERROR("MTU: %d given by NW exceeds UE supported size: %d",
                       mtu_len, MAX_SYSTEM_MTU);
    *decode_mask_ptr |= DS_3GPP_IPV4V6_LINK_MTU_INVALID;
  }
  else
  {
  if(v4_iface != NULL && 
     (pdn_cntxt_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
     == DS_UMTS_PDP_IPV4 ||
     pdn_cntxt_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
     == DS_UMTS_PDP_IPV4V6)
     )
  {
    DS_3GPP_MSG0_MED("DEBUG: Filling MTU length for V4 iface");
      PS_IFACE_SET_MTU(v4_iface, mtu_len);
  }

  if(v6_iface != NULL && 
     (pdn_cntxt_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
     == DS_UMTS_PDP_IPV6 ||
     pdn_cntxt_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
     == DS_UMTS_PDP_IPV4V6)
     )
  {
    DS_3GPP_MSG0_MED("DEBUG: Filling MTU length for V6 iface");

    /*---------------------------------------------------------------------------
      As per IPv6 RFC, the minimum supported MTU is 1280. PS_IFACE_SET_MTU 
      macro checks the MTU size given as input against default IPv6 MTU 1280 
      and ensures that IPv6 MTU being set is equal to or greater than 1280.
    ----------------------------------------------------------------------------*/
    PS_IFACE_SET_MTU( v6_iface, mtu_len );
  }

  DS_3GPP_MSG1_HIGH("MTU %d set for iface decoded from pco",
                     mtu_len);
    *decode_mask_ptr |= DS_3GPP_IPV4V6_LINK_MTU_DECODED_MASK;
  }
}


/*===========================================================================
FUNCTION      ds_pdn_cntxt_setupBridgeInterfaceEvent

DESCRIPTION
  This function is called to bridge/Unbridge UM and RM. This posts a command 
  to the DS task.

PARAMETERS
  *this_iface_ptr   - Ptr to the interface to operate on
  *target_iface_ptr - Ptr to the interface to bridge to
  *client_data      - Ptr to any client data that needs to be passed in

DEPENDENCIES  None.

RETURN VALUE  0 - Success

SIDE EFFECTS  None.
===========================================================================*/
int ds_pdn_cntxt_setupBridgeInterfaceEvent
(
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void *client_data
  /*lint -esym(715,client_data) */
  /*lint -esym(818,client_data) */
)
{
  ds_cmd_type                         *cmd_ptr;  /* DS Task msg */
  ds_3gpp_pdn_setup_bridge_info_type  *setup_bridge_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(this_iface_ptr == NULL)
  {
    DATA_ERR_FATAL("this_iface_ptr is NULL. Assert");
    return 0;
  }

  DS_3GPP_MSG3_HIGH("this_iface_ptr:0x%x target_iface_ptr: 0x%x", 
                    this_iface_ptr, target_iface_ptr,0);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_setup_bridge_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return -1;
  }

  DS_3GPP_MSG2_HIGH("Posting DS3GPP_SETUP_BRIDGE_INTERFACE_CMD "
                    "this_iface_ptr:0x%x target_iface_ptr: 0x%x", 
                    this_iface_ptr, target_iface_ptr);

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_BRIDGE_INTERFACE_SETUP;
  setup_bridge_info_ptr = 
    (ds_3gpp_pdn_setup_bridge_info_type*)cmd_ptr->cmd_payload_ptr;

  setup_bridge_info_ptr->this_iface_ptr = this_iface_ptr;
  setup_bridge_info_ptr->target_iface_ptr = target_iface_ptr;
  setup_bridge_info_ptr->client_data = client_data;
  ds_put_cmd_ext(cmd_ptr);

  return 0;
}


/*===========================================================================
FUNCTION      ds_pdn_cntxt_setupBridgeInterfaceCmdHdlr

DESCRIPTION
  This function process the DS_3GPP_SETUP_BRIDGE_INTERFACE_CMD. It 
  registers for desired events with the bridged interface. The
  only event that we register for is the IFACE_FLOW_ENABLED_EV.

PARAMETERS
  *cmd_ptr - ds cmd ptr

DEPENDENCIES  None.

RETURN VALUE  0 - Success

SIDE EFFECTS  None.
===========================================================================*/
static int ds_pdn_cntxt_setupBridgeInterfaceCmdHdlr
(
  const ds_cmd_type *cmd_ptr
  /*lint -esym(715,client_data) */
  /*lint -esym(818,client_data) */
)
{
  ds_pdn_context_s                    *pdn_cntx_p;
  ds_bearer_context_s                 *bearer_context_p;
  ds_3gpp_iface_s                     *ds_iface_ptr;
  ds_3gpp_pdn_setup_bridge_info_type  *setup_bridge_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL ptr passed, return");
    return 0;
  }

  setup_bridge_info_ptr = 
    (ds_3gpp_pdn_setup_bridge_info_type*)(cmd_ptr->cmd_payload_ptr);
  
  /* This will be called even if target_iface_ptr is NULL   */

  if(setup_bridge_info_ptr->this_iface_ptr == NULL)
  {
    DATA_ERR_FATAL("this_iface_ptr is NULL. Assert");
    return 0;
  }

  /*-------------------------------------------------------------------------
    Get the DS Iface from the PS Iface
  -------------------------------------------------------------------------*/
  ds_iface_ptr = 
    (ds_3gpp_iface_s*)setup_bridge_info_ptr->this_iface_ptr->client_data_ptr;

  if(ds_iface_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS IFACE cannot be obtained from the PS IFace. Not bridging");
    return 0;
  }

  DS_3GPP_MSG1_MED("setupBridgeInterface Iface state: %d", ds_iface_ptr->state);

  if(setup_bridge_info_ptr->target_iface_ptr == NULL)
  {
    DS_3GPP_MSG0_HIGH("Setting bridge Iface Ptr to NULL");

    /*------------------------------------------------------------------
    We grab PS and DS crit sections here to protect changes made to the
    cached rm_iface_ptr, sio_port, sio_tx_wm_ptr. These cached values
    get accessed during PS_SYS_IOCTL_SWITCH_DATA_PATH_TO_HW from a2, ps,
    dss task contexts.
    -------------------------------------------------------------------*/
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    ds3gpp_enter_global_crit_section();

    /*---------------------------------------------------------------------
     Update the DS Cache with the Rm Iface pointer, sio_port and WM
    ----------------------------------------------------------------------*/
    ds_iface_ptr->rm_iface_ptr = NULL;
    
    /*---------------------------------------------------------------------
     Leave the crit sections we entered earlier
    ----------------------------------------------------------------------*/
    ds3gpp_leave_global_crit_section();
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

    return 0;
  } /* UNBRIDGE DONE*/

/*----------------------------------------------------------------------------
****************      SETUP BRIDGE   ********************************
-----------------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------
  Check the state of the UM iface
  ------------------------------------------------------------------------*/
  if(ps_iface_state(&(ds_iface_ptr->ps_iface)) != IFACE_UP &&  
       ps_iface_state(&(ds_iface_ptr->ps_iface)) != IFACE_ROUTEABLE )
  {
    DS_3GPP_MSG1_ERROR("Invalid iface state %d. Do not setup bridge",
                                   ps_iface_state(&(ds_iface_ptr->ps_iface)));

    return 0;
  }

  /*-------------------------------------------------------------------------
    Get the pdn cntx instance.
  -------------------------------------------------------------------------*/
  
  pdn_cntx_p = (ds_pdn_context_s*)(ds_iface_ptr->client_data_ptr);

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return 0;
  }

  /*---------------------------------------------------------------------------
    Get the default bearer context pointer from the PDN context.
  ---------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->
                                  ds_pdn_context_dyn_p->def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
  /*----------------------------------------------------------------------
  No need to bridge or unbridge
  -----------------------------------------------------------------------*/
    return 0;
  }

  /*------------------------------------------------------------------------
   Update the DS Cache with the Rm Iface pointer and sio_port passed 
   into the function
  ------------------------------------------------------------------------*/
  ds_iface_ptr->rm_iface_ptr = setup_bridge_info_ptr->target_iface_ptr;
  
  return(0);

} /* ds_pdn_cntxt_setupBridgeInterfaceEvent() */
  /*lint +esym(715,client_data) */
  /*lint +esym(818,client_data) */


#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION dsPdnContext_bringUpIPv6PSIface

DESCRIPTION
  This function brings up  IPV6 PS iface in response to bearer up ind  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  

===========================================================================*/
void dsPdnContext_bringUpIPv6PSIface
( 
    ds_3gpp_iface_s  *ds3gpp_iface_p
)
{                 
     
    /*----------------------------------------------------------------------
      Valid IID for V6 has been received. IPV6 prefix is neede for this
      Iface to start to bring up the interface. Send Link up event to
      DSSNET6 SM which will initiate router solicitation for Prefix.
     ----------------------------------------------------------------------*/  
  if (dssnet6_sm_post_event(&(ds3gpp_iface_p->dssnet6_sm_cb),
                            DSSNET6_LINK_UP_EV, 
                            NULL) != 0) 
  {
      DS_3GPP_MSG0_ERROR("Error in posting dssnet6 event ");      
  }
  
  
}

#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION dsPdnContext_bringUpIPv4PSIface

DESCRIPTION
  This function completes IPv4 PS iface configuration  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  

===========================================================================*/
void dsPdnContext_bringUpIPv4PSIface
(   
  ds_3gpp_iface_s  *ds3gpp_iface_p  
)
{   
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
  ps_iface_type *iface_ptr = &(ds3gpp_iface_p->ps_iface);
  ds_pdn_context_s *pdn_cntx_p = (ds_pdn_context_s *)ds3gpp_iface_p->client_data_ptr;
  ds_dsd_apm_ind_pay_load ind_payload;

  memset(&ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

   if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }  

  /*----------------------------------------------------------------------
   Send flow activate indication.   
  ----------------------------------------------------------------------*/
  ps_flow_activate_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                       PS_EIC_NOT_SPECIFIED);
   
  /*----------------------------------------------------------------------
  Send IFACE UP indication for the primary
  ----------------------------------------------------------------------*/
  ps_iface_up_ind(iface_ptr);     
  
  //Inform APM that the V4 address has been configured.
  if ( TRUE == ds_pdn_cntxt_is_default(pdn_cntx_p))
  {
    DS_3GPP_APM_SET_VALID_V4_IPADDR(ind_payload);     
    DS_3GPP_APM_SET_PROFILE_ID(ind_payload, (uint16)pdn_cntx_p->
                               ds_pdn_context_dyn_p->pdp_profile_num);  
    ind_payload.subs_id = subs_id;
    (void)ds_dsd_apm_ind(DS_DSD_APM_IP_ADDRESS_IND, &ind_payload);
  }
  return;
}

/*===========================================================================
FUNCTION DSPDNCNTX_DHCP_STATUS_CB()

DESCRIPTION
  This function processes the DHCP client indications for a DHCPv4 address
  allocation.

PARAMETERS
  callback_data     -   pointer to store the call instance
  dhcp_status       -   enum to indicate dhcp status
  
RETURN VALUE
  none

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_dhcp_status_cb
(
  void                    *callback_data,
  dhcp_client_status_enum  dhcp_status
)
{
  ds_pdn_context_s *pdn_cntx_p;         /* Index into call table for call */
  ds_3gpp_iface_s *ds3gpp_iface_p;       /* ptr to 3GPP iface tbl          */
  ps_iface_type    *iface_ptr;          /* ptr to ps_iface */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3gpp_iface_p = (ds_3gpp_iface_s*) callback_data;
  pdn_cntx_p = (ds_pdn_context_s*) ds3gpp_iface_p->client_data_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  DS_3GPP_MSG2_HIGH("DHCP Status: ind = %d, PDN cntx Ptr = 0x%x",
                     dhcp_status, pdn_cntx_p); 

  switch(dhcp_status)
  {
    case DHCP_CLIENT_IP_ASSIGNED:
      if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.ipv4_addr_alloc 
          == DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4)
      {
        /* bring up the iface when DHCP addr assignment is completed. Addr 
           configuration is handled by DHCP client
        */
        dsPdnContext_bringUpIPv4PSIface(ds3gpp_iface_p);
      }
      break;

    case DHCP_CLIENT_IP_RELEASED:
    case DHCP_CLIENT_IP_EXPIRED:
      // client will automatically attempt to renew
    case DHCP_CLIENT_IP_FAIL:
      // This event is ignored for now, DHCP renew process will start again
      // when the application acquires the iface again to provide a retry
      // mechanism
    case DHCP_CLIENT_INIT_FAIL:
      break;
    case DHCP_CLIENT_PROVISIONED:
      DS_3GPP_MSG0_HIGH("DHCP client provisioned callback recvd.");
      break;
    default:
      DS_3GPP_MSG1_ERROR("Address client ignoring unknown DHCP ind %d",
                          dhcp_status);
  }

  // If DHCP client is started only for fetching extended ip config,
  // we can stop the client
  if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.ipv4_addr_alloc 
      != DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4)
  {
  iface_ptr = &(ds3gpp_iface_p->ps_iface);

  if(ds3gpp_iface_p->dhcp_handle != NULL)
  {
    if(ps_iface_addr_family_is_v6(iface_ptr)) 
    {
#ifdef FEATURE_DATA_PS_IPV6
      dhcp6_client_free(&(ds3gpp_iface_p->dhcp_handle));
#else
      DS_3GPP_MSG0_ERROR("IFACE addr family is V6 but FEATURE_DATA_PS_IPV6 is undefined.");
#endif /* FEATURE_DATA_PS_IPV6 */
    }
    else
    {
      dhcp_client_stop(&(ds3gpp_iface_p->dhcp_handle));
    }
  }

  }
} /* ds_pdn_cntxt_dhcp_status_cb() */


/*===========================================================================
FUNCTION  ds_pdn_cntxt_dhcpStartV4AddrAlloc

DESCRIPTION
   This function creates a dhcp client and requests the v4 address.
   The success or failure to obtain the address is returned by a callback
   from DHCP server.


PARAMETERS   :
  *ds3gpp_iface_p  - Pointer to the umtsps iface call table
  pdn_cntx_p       - PDN context pointer
  
DEPENDENCIES
  None.

RETURN VALUE
   TRUE - DHCP request has been sent out successfully or the dhcp request
          flag was not set.
   FALSE - DHCP request could not be sent out due to some error.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_pdn_cntxt_dhcpStartV4AddrAlloc
( 
  ds_3gpp_iface_s       *ds3gpp_iface_p,       /* ptr to 3GPP iface tbl  */
  ds_pdn_context_s      *pdn_cntx_p         /* call instance for the call*/
)  
{
  dhcp_client_params         dhcp_config;     /* dhcp config parameters */
  void                       *dhcp_handle;    /* Handle to DHCP6 Client */
  ps_iface_type              *iface_ptr;      /* Pointer to ps iface */
  uint64                     duid_addr;       /* Device Unique ID for DHCP */
  ip_addr_type               pcscf_v4_addr_array[MAX_PCSCF_ADDRESSES];
  uint32                     count = 3;
  /*-----------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  /* Assign the predefined value for DUID */
  duid_addr = DHCP_CLIENT_DUID_ADDR;
  iface_ptr = &(ds3gpp_iface_p->ps_iface);

  if (dhcp_client_params_init(&dhcp_config,
                &duid_addr, 
                DHCP_CLIENT_DUID_ADDR_LEN ) == -1)
  {
    DS_3GPP_MSG0_HIGH("Failed to initialize DHCP parameters");
    return FALSE;
  }
  
  /*-----------------------------------------------------------------------
      DHCP config initialization
  -----------------------------------------------------------------------*/
  dhcp_config.domain_name_server_option = FALSE;
  dhcp_config.subnet_mask_option = FALSE;
  dhcp_config.router_option = FALSE;
  dhcp_config.interface_mtu_option = FALSE;  
  dhcp_config.sip_server_option = FALSE;

#ifdef FEATURE_DATA_IMS
  if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
     request_pcscf_address_using_dhcp_flag == TRUE)
  {
    /*-------------------------------------------------------------------------
      Check if PCSCF address are obtained through PCO, if not then we will
      request that
    -------------------------------------------------------------------------*/
    ps_iface_get_sip_serv_addr(iface_ptr, pcscf_v4_addr_array, &count);
    if (count == 0)
    {
      dhcp_config.sip_server_option = TRUE;
    }  
  }
#endif /*FEATURE_DATA_IMS*/



  dhcp_handle = dhcp_client_start(
    PS_IFACE_GET_ID(iface_ptr),
    &dhcp_config,
    ds_pdn_cntxt_dhcp_status_cb,
    iface_ptr->client_data_ptr);

  if(dhcp_handle == NULL)
  {
    DS_3GPP_MSG1_HIGH("Could not allocate DHCP handle. PDN cntx inst: %d", 
         (uint32)(((ds_3gpp_iface_s*)\
                       (iface_ptr->client_data_ptr))->client_data_ptr));
    return FALSE;
  }
  else
  {
    /*---------------------------------------------------------------------
      Store the dhcp handle in the call table and initiate the DHCP query.
      The Inform function will generate a DHCP request. It returns the
      callback with the status of the request.
      The pcscf addresses are stored within the ps iface.
    ---------------------------------------------------------------------*/
    ds3gpp_iface_p->dhcp_handle = dhcp_handle;
    dhcp_client_renew(dhcp_handle);
  }
  
  return TRUE;
}/* ds_pdn_cntxt_dhcpStartV4AddrAlloc */

/*===========================================================================
FUNCTION  ds_pdn_cntxt_dhcpStopClient

DESCRIPTION
   This function stops the DHCP v4 client


PARAMETERS   :
  *ds3gpp_iface_p  - Pointer to the umtsps iface call table
  
DEPENDENCIES
  None.

RETURN VALUE
   TRUE - DHCP request has been sent out successfully or the dhcp request
          flag was not set.
   FALSE - DHCP request could not be sent out due to some error.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds_pdn_cntxt_dhcpStopClient
( 
  ds_3gpp_iface_s       *ds3gpp_iface_p       /* ptr to 3GPP iface tbl  */
)
{
  if(ds3gpp_iface_p->dhcp_handle != NULL)
  {
    dhcp_client_release(ds3gpp_iface_p->dhcp_handle);
    dhcp_client_stop(&(ds3gpp_iface_p->dhcp_handle));
  }
  return;
}

/*===========================================================================
FUNCTION  ds_pdn_cntxt_dhcpRequestExtConfig

DESCRIPTION
   This function creates a dhcp client and requests extended config such as 
   the pcscf address and dns address if requested in the profile.
   The success or failure to obtain the address is returned by a callback
   from DHCP server.

   This function performs the following steps.
   1) Initialize the DHCP Parameters: to indicate which all config items to 
      request based on profile parameters.
   2) Start DHCP client
   3) DHCP_Inform - sends out the query

PARAMETERS   :
  *ds3gpp_iface_p  - Pointer to the umtsps iface call table
  pdn_instance     - Call Instance for the call
  
DEPENDENCIES
  None.

RETURN VALUE
   TRUE - DHCP request has been sent out successfully or the dhcp request
          flag was not set.
   FALSE - DHCP request could not be sent out due to some error.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_pdn_cntxt_dhcpRequestExtConfig
( 
  ds_3gpp_iface_s       *ds3gpp_iface_p,       /* ptr to 3GPP iface tbl  */
  ds_pdn_context_s      *pdn_cntx_p         /* call instance for the call*/
)  
{
#ifdef FEATURE_DATA_PS_IPV6
  dhcp6_client_params        dhcp6_config;    /* dhcp6 config parameters */
#endif /* FEATURE_DATA_PS_IPV6 */
  dhcp_client_params         dhcp_config;     /* dhcp config parameters */
  void                       *dhcp_handle;    /* Handle to DHCP6 Client */

  ps_iface_type              *iface_ptr;      /* Pointer to ps iface */
   
  uint64             duid_addr;       /* Device Unique ID for DHCP */

  /* Assign the predefined value for DUID */
  duid_addr = DHCP_CLIENT_DUID_ADDR;

  /*-------------------------------------------------------------------------
    Figure out if we need to start the DHCP client.
    Would be needed in following cases.
    If PCSCF Address Discovery using DHCP is requested.
    If DNS address via DHCP is requested.

    The parameters to be requested (PCSCF Address and DNS address) are 
    automatically set inside the dhcp_client_params_init() function.
  -------------------------------------------------------------------------*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  
  DS_3GPP_MSG2_HIGH( "ds_pdn_cntxt_dhcpRequestExtConfig(): "
                     "request_pcscf_address_using_dhcp_flag: %d"
                     "request_dns_address_using_dhcp_flag: %d",
                     pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
                       request_pcscf_address_using_dhcp_flag,
                     pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
                       request_dns_address_using_dhcp_flag );
  /*-----------------------------------------------------------------------
    Start DHCP client in case either PCSCF or DNS is to be requested via
    DHCP as per profile configuration.
  -----------------------------------------------------------------------*/
  if( pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
      request_pcscf_address_using_dhcp_flag == TRUE ||
      pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
      request_dns_address_using_dhcp_flag == TRUE )
  {
    iface_ptr = &(ds3gpp_iface_p->ps_iface);
  /*-----------------------------------------------------------------------
    Configure the DHCP client and start it. 
  -----------------------------------------------------------------------*/
    if(ps_iface_addr_family_is_v6(iface_ptr)) 
    {
#ifdef FEATURE_DATA_PS_IPV6
    if (dhcp6_client_params_init(&dhcp6_config,
                                 DHCP6_CLIENT_MODE_STATELESS,
                                 &duid_addr, 
                                 DHCP_CLIENT_DUID_ADDR_LEN ) == -1)
    {
      DS_3GPP_MSG0_HIGH("Failed to initialize DHCP parameters");
      return FALSE;
    }

    /*-----------------------------------------------------------------------
    DHCP initializes all the options to TRUE by default. Uninitialize 
      what is not requested based on profile.
    -----------------------------------------------------------------------*/
    if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
      request_dns_address_using_dhcp_flag != TRUE)
    {
      dhcp6_config.dns_servers_option = FALSE;
      dhcp6_config.dns_domain_searchlist_option = FALSE;
    }
    if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
      request_pcscf_address_using_dhcp_flag != TRUE)
    {
      dhcp6_config.sip_domain_name_list_option = FALSE;
      dhcp6_config.sip_ipv6_list_option = FALSE;
    }

    /* Use ds_3gpp_iface_s as cookie */
    DS_3GPP_MSG0_HIGH( "ds_pdn_cntxt_dhcpRequestExtConfig():"
                       " invoking dhcp6_client_start()" );
    dhcp_handle = dhcp6_client_start(
      PS_IFACE_GET_ID(iface_ptr),
      &dhcp6_config,
      ds_pdn_cntxt_dhcp_status_cb,
      iface_ptr->client_data_ptr);

      if(dhcp_handle == NULL)
    {
      DS_3GPP_MSG1_HIGH("Could not allocate DHCP handle. pdn cntx inst: %d", 
           (uint32)(((ds_3gpp_iface_s*)\
                     iface_ptr->client_data_ptr)->client_data_ptr));
      return FALSE;
    }
    else
    {
      /*---------------------------------------------------------------------
    Store the dhcp handle in the call table and initiate the DHCP query.
    The Inform function will generate a DHCP request. It returns the
    callback with the status of the request.
    The pcscf addresses are stored within the ps iface.
      ---------------------------------------------------------------------*/
    ds3gpp_iface_p->dhcp_handle = dhcp_handle;
    }
    dhcp6_client_inform(dhcp_handle);
#else
    DS_3GPP_MSG0_ERROR("IFACE addr family is V6 but FEATURE_DATA_PS_IPV6 is undefined.");
#endif
  }
  else
  {
    if (dhcp_client_params_init(&dhcp_config,
                  &duid_addr, 
                  DHCP_CLIENT_DUID_ADDR_LEN ) == -1)
    {
      DS_3GPP_MSG0_HIGH("Failed to initialize DHCP parameters");
      return FALSE;
    }
    
    /*-----------------------------------------------------------------------
        DHCP initializes all the options to TRUE by default. Uninitialize 
        everything except PCSCF address request.
      -----------------------------------------------------------------------*/
    dhcp_config.domain_name_server_option = FALSE;
    dhcp_config.subnet_mask_option = FALSE;
    dhcp_config.router_option = FALSE;
    dhcp_config.interface_mtu_option = FALSE;      
    dhcp_handle = dhcp_client_start(
      PS_IFACE_GET_ID(iface_ptr),
      &dhcp_config,
      ds_pdn_cntxt_dhcp_status_cb,
      iface_ptr->client_data_ptr);

    if(dhcp_handle == NULL)
    {
    DS_3GPP_MSG1_HIGH("Could not allocate DHCP handle. PDN cntx inst: %d", 
         (uint32)(((ds_3gpp_iface_s*)\
                       (iface_ptr->client_data_ptr))->client_data_ptr));
    return FALSE;
    }
    else
    {
    /*---------------------------------------------------------------------
      Store the dhcp handle in the call table and initiate the DHCP query.
      The Inform function will generate a DHCP request. It returns the
      callback with the status of the request.
      The pcscf addresses are stored within the ps iface.
    ---------------------------------------------------------------------*/
    ds3gpp_iface_p->dhcp_handle = dhcp_handle;
    }
    dhcp_client_inform(dhcp_handle, PS_IFACE_IP_V4_ADDR(iface_ptr));
  }
  }
  return TRUE;
}/* ds_pdn_cntxt_dhcpRequestExtConfig */

/*===========================================================================
FUNCTION ds_pdn_cntxt_ifaceUpCmd

DESCRIPTION
  This function posts Iface Up Command.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_pdn_cntxt_ifaceUpCmd
(
  ps_iface_type    *iface_ptr_i,
  void             *info_ptr
  /*lint -esym(818, iface_ptr_i) */
)
{   
  const ps_iface_type         *iface_ptr = iface_ptr_i;
  ds_cmd_type                 *cmd_ptr;                   /* DS Task msg */
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Bringing iface. At this point of time the PDN context and the bearer
    context is not allocated or bound to each other. We need the iface_ptr
    in the iface_up_cmd_hdlr to be able to bring it up. We pass this info
    in the user_data of the command we enqueue in the DS task.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG2_HIGH("IFACE UP CMD for 3GPP iface pdn cntx ptr:0x%x info:%lx",
           (((ds_3gpp_iface_s*)(iface_ptr->client_data_ptr))->client_data_ptr),\
           (uint32)info_ptr);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_UP;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->info_ptr = info_ptr;
  call_info_ptr->user_data = (void*)iface_ptr_i;
  ds_put_cmd_ext(cmd_ptr);

  return(0);
 
}
 /*lint +esym(818, iface_ptr_i) */

 /*===========================================================================
 FUNCTION      DS_PDN_CNTXT_IFACELINGERCMD
 
 DESCRIPTION
   This function is called when iface goes to LINGERING state.
 
 PARAMETERS
   iface_ptr - Interface ptr for the call
 
 DEPENDENCIES
   None.
 
 
 RETURN VALUE
   0 - Success
 
 SIDE EFFECTS
   None.
 
 ===========================================================================*/
 /*ARGSUSED*/
 static int ds_pdn_cntxt_ifaceLingerCmd
 (
   ps_iface_type *iface_ptr,
   void          *not_used
 )
 {
   /*Do nothing*/
   DS_3GPP_MSG0_HIGH("IfaceLingerCmd: do nothing");
   return 0;
 } /* ds_pdn_cntxt_ifaceLingerCmd */

/*===========================================================================
FUNCTION ds_pdn_cntxt_emergency_pdn_ifaceUpCmd

DESCRIPTION
  This function is called from the PS framework when an emergency Iface is to
  be brought up.

PARAMETERS
  iface_ptr_i - IFace pointer
  info_ptr - client data. The PDN context pointer in this case.

DEPENDENCIES
  None.

RETURN VALUE  
  Unused.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_pdn_cntxt_emergency_pdn_ifaceUpCmd
(
  ps_iface_type    *iface_ptr_i,
  void             *info_ptr
  /*lint -esym(818, iface_ptr_i) */
)
{   
#ifdef FEATURE_EMERGENCY_PDN
  ds_cmd_type                 *cmd_ptr;                   /* DS Task msg */
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Bringing iface. At this point of time the PDN context and the bearer
    context is not allocated or bound to each other. We need the iface_ptr
    in the iface_up_cmd_hdlr to be able to bring it up. We pass this info
    in the user_data of the command we enqueue in the DS task.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG2_HIGH("IFACE UP CMD for 3GPP Emergency Iface 0x%x:%x",
                    iface_ptr_i->name, iface_ptr_i->instance);
           
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("DS3GPP can't get cmd buf for DS task");
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_UP_EMERGENCY;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->info_ptr = info_ptr;
  call_info_ptr->user_data = (void*)iface_ptr_i;
  ds_put_cmd_ext(cmd_ptr);

#else
  DS_3GPP_MSG0_HIGH("Emergency iface cannot be brought up,feature not supported");
#endif /* FEATURE_EMERGENCY_PDN */
  return(0);
 
}
 /*lint +esym(818, iface_ptr_i) */

/*===========================================================================
FUNCTION ds_pdn_cntxt_ifaceDownCmd

DESCRIPTION
  This function posts Iface Down Command.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context

SIDE EFFECTS
  None.

===========================================================================*/
static int ds_pdn_cntxt_ifaceDownCmd
(
  ps_iface_type    *iface_ptr_i,
  void             *info_ptr  
    /*lint -esym(818,iface_ptr_i,info_ptr) */
)
{   
  ds_cmd_type                 *cmd_ptr;                /* DS Task msg */
  ds_3gpp_pdn_call_info_type  *call_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_call_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_ERR_FATAL("DS3GPP can't get cmd buf for DS task");
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_DOWN;
  call_info_ptr = (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

  call_info_ptr->user_data = (void*) iface_ptr_i;
  call_info_ptr->info_ptr = info_ptr;

  ds_put_cmd_ext(cmd_ptr);

  return(0); 
}
  /*lint +esym(818,iface_ptr_i,info_ptr) */

/*===========================================================================
FUNCTION      DSPDNCNTX_IFACEINUSECMD

DESCRIPTION
  Post a command to indicate the active use of the iface by application.

PARAMETERS
  iface_ptr - Interface ptr for the call

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static int ds_pdn_cntxt_ifaceInUseCmd
(
  ps_iface_type    *iface_ptr_i,
  void             *info_ptr
  /*lint -esym(818, iface_ptr_i) */
)
{
  ds_cmd_type                 *cmd_ptr;                     /* DS Task msg */
  ds_3gpp_pdn_iface_info_type *iface_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    This function can only be called for an active iface
    // Replace this with the new Ps API
  ------------------------------------------------------------------------*/
  if ( (iface_ptr_i)->iface_private.is_active != TRUE ) 
  {
    DATA_ERR_FATAL("DSUMTSPS iface_in_use called for an in-active iface");
    return (0);
  }

  /*-------------------------------------------------------------------------
    Iface has already been brought up. Now this function is called when the 
  first app starts to use the iface.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG2_HIGH("DSUMTSPS IFACE IN_USE CMD ds_iface_p:0x%x info:%lx",
           (uint32)(iface_ptr_i->client_data_ptr),(uint32)info_ptr);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_iface_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return (-1);
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_IN_USE;
  iface_info_ptr = (ds_3gpp_pdn_iface_info_type*)cmd_ptr->cmd_payload_ptr;

  iface_info_ptr->iface_ptr = iface_ptr_i;
  ds_put_cmd_ext(cmd_ptr);

  return(0);
} /* ds_pdn_cntxt_ifaceInUseCmd() */
  /*lint +esym(818, iface_ptr_i) */


/*===========================================================================
FUNCTION      DSPDNCNTX_IFACAEOUTOFUSECMD

DESCRIPTION
  Post a command to indicate the iface is not in use by any application.

PARAMETERS
  iface_ptr - Interface ptr for the call
  info_ptr  - void pointer

DEPENDENCIES
  None.

RETURN VALUE
  0 - Success

SIDE EFFECTS
  None.

===========================================================================*/
static int ds_pdn_cntxt_ifaceOutofUseCmd
(
  ps_iface_type    *iface_ptr,
  void             *info_ptr
  /*lint -esym(818,iface_ptr_i,info_ptr) */
)
{
  ps_iface_type               *companion_iface_p = NULL;
  ds_cmd_type                 *cmd_ptr = NULL;/* DS Task msg            */
  int                          ret_val = 0;
  boolean                      ota_teardown = FALSE;
  boolean                      is_companion_iface_in_use = FALSE;
  ds_3gpp_iface_s             *ds_3gpp_iface_p = NULL;
  ds_pdn_context_s            *pdn_cntx_p = NULL;
  ds_bearer_context_s         *def_bearer_context_p = NULL;
  sys_sys_mode_e_type          mode = SYS_SYS_MODE_NONE;
  boolean                      linger_flag  = FALSE;
  sys_modem_as_id_e_type       subs_id  = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_pdn_iface_info_type *iface_info_ptr = NULL;
#ifdef FEATURE_DATA_LTE
  boolean                      allow_def_pdn_teardown= FALSE;
#endif /* FEATURE_DATA_LTE */
  boolean                      local_abort = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    This function can only be called for an active iface
    // replace this with the PS API
  ------------------------------------------------------------------------*/
  if ( (iface_ptr)->iface_private.is_active != TRUE ) 
  {
    DATA_ERR_FATAL("DSUMTSPS iface_out_use called for an in-active iface");
  return (0);
  }

  DS_3GPP_MSG1_HIGH("DSUMTSPS IFACE OUT_USE cmd UMTSPS Iface Ptr:0x%x",
            iface_ptr);
  /*-------------------------------------------------------------------------
    Extract the PDN CNTX pointer from the iface.
  -------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();
  ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_ptr->client_data_ptr);

  pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

  /*-------------------------------------------------------------------------
    Verify that the PDN context exits for this call. There is every chance
    that the PDN context has been freed.
  -------------------------------------------------------------------------*/

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    DS_3GPP_MSG3_ERROR("PDN context could not fetch valid Subs id(CM) %d, "
                       "iface ptr: 0x%x: %d",
                       subs_id, iface_ptr->name, iface_ptr->instance);
    ds3gpp_leave_global_crit_section();
    return 0;
  }

  /*-------------------------------------------------------------------------
    Get the local abort flag, This will determine if a PDN disconnect request
    will have to be sent OTA or not
    -------------------------------------------------------------------------*/
  local_abort = PS_IFACE_GET_PDN_TEARDOWN_LOCAL_ABORT_CONFIG(iface_ptr);
  /*-------------------------------------------------------------------------
    Get the Primary bearer context
  -------------------------------------------------------------------------*/
  def_bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p
                                                ->def_bearer_context_ptr);
  
  /*-------------------------------------------------------------------------   
    if the iface is in routable state,
    reset it to UP state since its is OUTOFUSE.
    iface in use is checked to prevent a race condition in which another app
    grabbed this iface right after the iface is put to out of use state
  -------------------------------------------------------------------------*/
  if ( (!PS_IFACE_IS_IN_USE(iface_ptr)) && 
     (ps_iface_state(iface_ptr) == IFACE_ROUTEABLE) )
  {
    ps_iface_up_ind(iface_ptr);
  }  

 /*-------------------------------------------------------------------------
  Check to see if all other ifaces in this pdn context are either down or
  out of use, if so, we need to do additional processing. Since only 
  default pdn connectivity is supported, this pdn context cannot be torn
  down, we will leave the out of use ifaces in UP state.
  -------------------------------------------------------------------------*/
  companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_ptr);
  if (PS_IFACE_IS_IN_USE(companion_iface_p))
    {
      DS_3GPP_MSG0_HIGH("Companion iface is in use");
      is_companion_iface_in_use = TRUE;
    }

  if(!is_companion_iface_in_use)
  {
  /*---------------------------------------------------------------------------------
        Check if the PDN context can be torn down
        The PDN cannot be the default PDN. 
        If it is non default, there should be atleast another PDN apart from this 
        one that is in UP state.
        The PDN can also be torn down if the mode is not LTE anymore
        
        Get the current network mode. If the network mode is a 3GPP2 mode then continue
        using network mode as the most up-to-date information about the technology that
        we are camped on.
        However if the network mode is a 3GPP mode, use the bearer call mode as the most
        up-to-date information. The reason being that - unlike the network mode, bearer
        call mode will be updated when we receive a Bearer Transfer Indication from CM
        but Tracking Area Update is not yet complete.

        If we have moved to a non-3GPP mode, we are expected to tear down the PDN
        context.   
   -----------------------------------------------------------------------------------*/
      mode = ds_3gpp_get_apn_current_mode(pdn_cntx_p);

      if ( DS_3GPP_IS_3GPP_CURRENT_MODE(mode))
      {
        if(ds_bearer_cntx_validate_bearer_context(def_bearer_context_p))
        {
          mode = ( sys_sys_mode_e_type ) def_bearer_context_p->ds_bearer_context_dyn_p
                                                                          ->call_mode;
          DS_3GPP_MSG1_HIGH("Bearer call mode is %d", mode);
        }
        else
        {
          DS_3GPP_MSG1_ERROR("Default bearer context is NULL for pdn cntx ptr 0x%x ", 
                              pdn_cntx_p);
        }
      }

    pdn_cntx_p->ds_pdn_context_dyn_p->ue_initiated_tear_down = TRUE;
     if ( (iface_ptr->iface_private.linger_timeout_val != 0 &&
          iface_ptr->linger_cmd_f_ptr != NULL) )
     {
       DS_3GPP_MSG0_HIGH("lingering is enabled OTA tear Down");
       linger_flag = TRUE;
     }
    /* If the mode is not LTE we should always tear down the PDN at this point*/
    if (mode != SYS_SYS_MODE_LTE)
    {
      DS_3GPP_MSG0_HIGH("Tearing down PDN context");
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
      ota_teardown = TRUE;
    }
    else if (local_abort)
    {
      DS_3GPP_MSG0_HIGH("Tearing down PDN context, Local abort set");
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
      ota_teardown = TRUE;
    }
#ifdef FEATURE_DATA_LTE

    else if (ds_3gpp_roaming_is_apn_disallowed
              (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
                 subs_id))
    {
      ds_3gpp_roaming_clear_wait_timer_entry
        (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_context_number);

      DS_3GPP_MSG0_HIGH("Tearing down PDN context");
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
      ota_teardown = TRUE;
    }
    else /*mode is LTE*/
    {
      /*
       If PDN is default, teardown the PDN if
          a) Allow Def PDN Teardown Flag is Set
          AND
          b) Another PDN is still Up
       
       If PDN is non-default, teardown the PDN if
          a) "Always connect Default PDN" flag is set
          OR
          b) Another PDN is still Up      

       Is PDN Default | Any other PDN Active | Always Connect Default PDN | Allow Default PDN Teardown | OTA Teardown ? 
             YES      |      YES             |      YES/NO                |      YES                   |  YES
             YES      |      YES             |      YES/NO                |      NO                    |  NO
             YES      |      NO              |      YES/NO                |      YES                   |  NO
             YES      |      NO              |      YES/NO                |      NO                    |  NO
             NO       |      YES             |      YES                   |      YES/NO                |  YES
             NO       |      YES             |      NO                    |      YES/NO                |  YES
             NO       |      NO              |      YES                   |      YES/NO                |  YES
             NO       |      NO              |      NO                    |      YES/NO                |  NO
        */
     
      // If PDN is Default
      if (ds_pdn_cntxt_is_default(pdn_cntx_p))
      {
        allow_def_pdn_teardown = ds_3gpp_nv_manager_get_allow_def_pdn_teardown(subs_id);

        if ( allow_def_pdn_teardown
            && 
            dsPdnContext_is_any_other_normal_pdn_up(pdn_cntx_p, subs_id))
        {
          DS_3GPP_MSG0_HIGH("Tearing down PDN context");
          pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
          ota_teardown = TRUE;
        }
      }

      // If PDN is Non-Default
      else 
      {
        if (ds_pdn_cntxt_is_pdn_context_tearable(pdn_cntx_p) == TRUE)
        {
          DS_3GPP_MSG0_HIGH("Tearing down PDN context");
          pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
          ota_teardown = TRUE;
        }
      }

       /* If this is the emergency PDN or if force_ota_teardown is TRUE or if sip 
           failure cause type is set to permanet for that PDN; always allow teardown. */
      if ((DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == TRUE) || 
          (pdn_cntx_p->ds_pdn_context_dyn_p->force_ota_teardown) ||
          (pdn_cntx_p->ds_pdn_context_dyn_p->ims_sip_reg_failure_flag))
      {

        DS_3GPP_MSG2_HIGH("Emergency PDN teardown OR Force OTA teardown set to:%d" 
                           " OR ims_sip_reg_failure_flag set to :%d",
                          pdn_cntx_p->ds_pdn_context_dyn_p->force_ota_teardown,
                          pdn_cntx_p->ds_pdn_context_dyn_p->ims_sip_reg_failure_flag);
        pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
        ota_teardown = TRUE;
      }
    }
#endif /* FEATURE_DATA_LTE */
  }
  ds3gpp_leave_global_crit_section();
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_iface_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return -1;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_OUT_OF_USE;
  iface_info_ptr = (ds_3gpp_pdn_iface_info_type*)cmd_ptr->cmd_payload_ptr;

  iface_info_ptr->iface_ptr = iface_ptr;
  iface_info_ptr->is_companion_iface_in_use = is_companion_iface_in_use;
  iface_info_ptr->ota_teardown = ota_teardown;

    iface_info_ptr->info_ptr = NULL;

  if (local_abort)
  {
    iface_info_ptr->info_ptr = (void*)((uint32)local_abort);
  }

  ds_put_cmd_ext(cmd_ptr);
  
  /*----------------------------------------------------------------
    No OTA teardown if companion iface is in_use or if pdn context is not 
    tearable return -1
    All other cases result in an OTA teardown, return 0
  ------------------------------------------------------------------*/
  
  if(ota_teardown && linger_flag)
  {
     DS_3GPP_MSG0_HIGH("OTA teardown with lingering enabled");
     ret_val = 0;
  }
  else if(ota_teardown)
  {
     DS_3GPP_MSG0_ERROR("OTA teardown");
     ret_val = -1;
  }
  else
  {
     DS_3GPP_MSG0_HIGH("No OTA teardown");
     ret_val = 0;
  }

  return ret_val;
} /* ds_pdn_cntxt_ifaceOutofUseCmd() */
/*lint +esym(818,iface_ptr_i,info_ptr) */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_GET_THR0TTLE_INFO

DESCRIPTION
  Handler for PS_SYS_IOCTL_PDN_THROTTLE_INFO. Retrieves throttle info
  for all apns for the current plmn, saved in the Per PLMN throttle
  table.

PARAMETERS 
  arg_val_ptr: Argument to be populated
  subs_id:     Subscription id
  ps_errno:    Err number if IOCTL cant be handled
   
  
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
 
SIDE EFFECTS
  None.
===========================================================================*/ 
int16 ds_3gpp_pdn_cntx_get_throttle_info
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  sys_plmn_id_s_type                   current_plmn_id;
  ps_sys_ioctl_pdn_throttle_info_type  *throttle_info;
  ds_3gpp_throt_rat_e_type             rat_index = DS_3GPP_THROT_RAT_GLOBAL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(arg_val_ptr == NULL)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-----------------------------------------------------------------------  
    Reset output buffer
    num_apn = 0 implicit
   -----------------------------------------------------------------------*/
  memset(arg_val_ptr, 0, sizeof(ps_sys_ioctl_pdn_throttle_info_type));
  throttle_info = (ps_sys_ioctl_pdn_throttle_info_type *)arg_val_ptr;

  ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    rat_index = ds_3gpp_throt_map_cm_sys_mode_to_rat_type
                  ((ds_3gpp_pdn_throt_determine_mode(subs_id)));
  } 

  /*------------------------------------------------------------------------
    For current plmn, get throttle info for all apns 
   -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_sm_get_throttle_info_for_plmn
    (&current_plmn_id, throttle_info, rat_index, subs_id); 

  *ps_errno = 0; /* No error */
  return 0; 
} /*ds_3gpp_pdn_cntx_get_throttle_info() */

/*===========================================================================
FUNCTION DS_3GPP_GET_COST_OF_MODEM_PWR

DESCRIPTION This function gets the modem power cost on the current RAT 
            

PARAMETERS
  *argval_ptr       - Pointer to fill in the cost of modem power
  *ps_errno         - Ptr to error code (if any)
   subs_id          - Subscription Id
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0           - SUCCESS, if the cost of modem power is obtained successfully
  -1          - FAILURE, otherwise

SIDE EFFECTS 
None
  
===========================================================================*/
int16 ds_3gpp_get_cost_of_modem_pwr
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  int16  ret_val = -1;
/*- - - - - - - - - -  - - - - - - - - - -  - - - - - - - - - - - - - -  - */

  DS_PDN_CTXT_VFR_CALL
  (
     ret_val,
     cost_of_modem_power,
     subs_id,
     arg_val_ptr,
     ps_errno
  );
    
  return ret_val;
} /* ds_3gpp_get_cost_of_modem_pwr */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_PS_SYS_IOCTL_HDLR

DESCRIPTION
  Generic handler for PS SYS IOCTLs. Dispatches functions to handle
  the IOCTLs.

PARAMETERS 
  ioctl_name[in]  -      Operation name
  arg_val_ptr[in] -  Pointer to operation specific argument
  ps_errno [out]    -    Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
 
SIDE EFFECTS
  None.
===========================================================================*/ 

int16 ds_3gpp_pdn_cntx_ps_sys_ioctl_hdlr
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                          * arg_val_ptr,
  int16                         * ps_errno
)
{
  int ret_val = -1;

  switch(ioctl_name)
  {
    default:
      DS_3GPP_MSG1_ERROR("ds_3gpp_pdn_cntx_ps_sys_ioctl_hdlr:" 
                          "Legacy IOCTL not supported %d", ioctl_name);
      *ps_errno = (int16) DS_EFAULT;
      return -1;
  }

  return ret_val;
} /*ds_3gpp_pdn_cntx_ps_sys_ioctl_hdlr() */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_PS_SYS_IOCTL_EX_HDLR

DESCRIPTION
  Generic handler for PS SYS EX IOCTLs. Dispatches functions to handle
  the IOCTLs.

PARAMETERS 
  ioctl_name[in]      -  Operation name
  arg_val_ptr[in]     -  Pointer to operation specific argument
  subscription_id[in] -  Subscription id
  ps_errno [out]      -  Specifies type of error in case of error.
                             DS_EFAULT if parameter is invalid
                             DS_EINVAL if operation is invalid
                             DS_NOMEMORY if memory allocation fails
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - Success
  -1 - Failure
 
SIDE EFFECTS
  None.
===========================================================================*/ 
int16 ds_3gpp_pdn_cntx_ps_sys_ioctl_ex_hdlr
(
  ps_sys_ioctl_enum_type          ioctl_name,
  void                           *arg_val_ptr,
  ps_sys_subscription_enum_type   subscription_id,
  int16                          *ps_errno
)
{
  int                     ret_val = -1;
  sys_modem_as_id_e_type  subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_id = 
    ds3gsubsmgr_subs_id_ds_to_cm((ds_sys_subscription_enum_type)subscription_id);

  switch(ioctl_name)
  {
    case PS_SYS_IOCTL_PDN_THROTTLE_INFO:
      ret_val = ds_3gpp_pdn_cntx_get_throttle_info(arg_val_ptr,
                                                   subs_id,
                                                   ps_errno);
      break;

#ifdef FEATURE_DATA_LTE
    case PS_SYS_IOCTL_3GPP_LTE_ATTACH_PDN_PARAMS:
      ret_val = ds_eps_pdn_cntxt_get_lte_attach_params(arg_val_ptr,
                                                       subs_id,
                                                       ps_errno);
      break;
#endif /* FEATURE_DATA_LTE */

    case PS_SYS_IOCTL_DUN_CTRL_RESPONSE:
      ret_val = dsumts_rmsm_dun_ctrl_resp(arg_val_ptr, ps_errno);
      break;

#ifdef FEATURE_DATA_LTE
    case PS_SYS_IOCTL_3GPP_SET_LTE_ATTACH_PDN_LIST:
      ret_val = ds_dsd_apm_set_attach_pdn_list(arg_val_ptr,
                                               subs_id,
                                               ps_errno);
      break;

    case PS_SYS_IOCTL_3GPP_GET_LTE_ATTACH_PDN_LIST:
      ret_val = ds_dsd_apm_get_attach_pdn_list(arg_val_ptr,
                                               subs_id,
                                               ps_errno);
      break;

    case PS_SYS_IOCTL_3GPP_GET_LTE_MAX_ATTACH_PDN_NUM:
      ret_val = ds_dsd_apm_get_max_attach_profile_num(arg_val_ptr,
                                                      subs_id,
                                                      ps_errno);
      break;
	  
    case PS_SYS_IOCTL_3GPP_SET_LTE_DATA_RETRY:
      ret_val = ds_dsd_apm_apn_switch_status_change(arg_val_ptr,
                                                    subs_id,
                                                    ps_errno);
      break;

    case PS_SYS_IOCTL_3GPP_GET_LTE_DATA_RETRY:
      ret_val = ds_dsd_apm_get_apn_switch_status(arg_val_ptr,
                                                 subs_id,
                                                 ps_errno);
      break;

    case PS_SYS_IOCTL_MODEM_POWER_COST:
      ret_val = ds_3gpp_get_cost_of_modem_pwr(arg_val_ptr,
                                              subs_id,
                                              ps_errno);
      break;
#endif /* FEATURE_DATA_LTE */

    case PS_SYS_IOCTL_3GPP_NSAPI:
      ret_val = ds3gpp_get_nsapi_from_apn(arg_val_ptr,
                                          subs_id,
                                          ps_errno);
      break;

    case PS_SYS_IOCTL_RELEASE_SIGNALLING_CONNECTION:
      ret_val = ds_3gpp_bearer_cntxt_handle_connection_release_request
                                               (arg_val_ptr, ps_errno);
      break;
#ifdef FEATURE_DATA_ANDSF
    //case PS_SYS_IOCTL_3GPP_ANDSF_POLICY_REFRESH:
    //   ret_val = ds_andsf_ps_ioctl_policy_refresh_req(arg_val_ptr, ps_errno);
    // break;

    case PS_SYS_IOCTL_SET_APN_CALL_TYPE:
      ret_val = ds_andsf_sys_ioctl_set_apn_call_type(arg_val_ptr, 
                                                     ps_errno);
      break;
#endif /*FEATURE_DATA_ANDSF*/
	  
    case PS_SYS_IOCTL_IS_CLAT_SUPPORTED_BY_PROFILE:
       ret_val = ds_3gpp_rt_acl_is_clat_supported(arg_val_ptr,
                                                  subs_id,
                                                  ps_errno);
        break;
	  
    default:
      DS_3GPP_MSG1_ERROR("ds_3gpp_pdn_cntx_ps_sys_ioctl_hdlr: Invalid IOCTL %d",
                         ioctl_name);
      *ps_errno = (int16) DS_EFAULT;
      return -1;
  }

  return ret_val;
} /* ds_3gpp_pdn_cntx_ps_sys_ioctl_ex_hdlr() */

/*===========================================================================
FUNCTION DS_3GPP_BRINGUP_SEC_CTX

DESCRIPTION
  Called from application context to activate secondary PDP context.

PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on
  *ioctl_info       - Ptr to QoS specified by the application
   subs_id          - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  int               - "0" if the sec ctx is brought up.
SIDE EFFECTS
  The function access data structures from APP context.

===========================================================================*/
static int ds_3gpp_bringup_sec_ctx
(
  ps_iface_type                    *iface_ptr,
  ps_iface_ioctl_qos_request_type  *ioctl_info,
  sint15                           *ps_errno,
  sys_modem_as_id_e_type            subs_id
)
{
  //ps_phys_link_type *phys_link_ptr = NULL;
  int ret_val;

  qos_spec_type              *input_qos = NULL;
  ds_pdn_context_s           *pdn_ptr  = NULL;
  ds_3gpp_iface_s            *ds_iface_ptr = NULL;    
  ds_3gpp_app_filter_op       app_filter_op = DS_3GPP_APP_FLTR_INVALID_OP;
  boolean                     qos_support = FALSE;
  uint8                       max_active_pdp_context = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qos_support = ds_3gpp_nv_manager_get_umts_ue_init_qos_support_nv();

  if (FALSE == qos_support)
  {
    DS_3GPP_MSG0_ERROR("QOS is not supported in this mode.");
    *ps_errno = DS_EOPNOTSUPP;
    return -1;
  }
 
  if (ds_3gppi_is_rel_11_supported())
  {
    /*-------------------------------------------------------------------------
     Spec 24.301 revision 11.4.0 Handling maximum number of active umts/eps contexts.
     -------------------------------------------------------------------------*/
    max_active_pdp_context = ds_3gpp_pdn_get_max_simul_active_pdp_context(subs_id);
    if( ds_3gpp_pdn_get_current_active_context(subs_id) >= max_active_pdp_context )
    {
      DS_3GPP_MSG1_ERROR("PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED,"
                         "Max PDP = (%d)",  
                          max_active_pdp_context); 
     *ps_errno = DS_ECONNREFUSED;
      return -1;
    }
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 /*-------------------------------------------------------------------------
   We are initiating a secondary context.

   Check to see if the iface is in UP state.
   Verify the input parameters (precedence and filter identifier). If the 
   user is requesting precedece, assign a unique precedence.
   
   Do not allocate a bearer context for LTE. Wait for the respone.
   We do not need a copy of the Qos information, as the APP decides whether
   to accept the request.
   
   For UMTS we need to allocate a bearer context and store the info in
   the bearer. Copy the QoS into the bearer flow manager and send an event 
   to UMTSPS to bringup the phys link.

   QoS and TFT are given by the application dynamically. Need to send
   an error to application if the QoS parameters are not given correctly.
   First convert the generic QoS given by the application to UMTS specific
   parameters. For now we are given two sets of QoS in no particular order.
   Sort the QoS according to the rules given in 3GPP TS 23.107 Annex C.
   Find the QoS required and QoS minimum. Copy the TFT fields. Any parameter
   not given by the application is set to subscribed (default)

   Note that in later releases more than two sets of QoS can be given by
   the application. In that case, all the given sets have to be sorted to
   find the requested and minimum QoS.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Error checking : iface should be in UP state
  -------------------------------------------------------------------------*/
  if ( (ps_iface_state(iface_ptr) != IFACE_UP ) &&
      (ps_iface_state(iface_ptr) != IFACE_ROUTEABLE))
  {
    DS_3GPP_MSG0_ERROR("Secondary activation: IFACE not in UP state");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    The Qos passed as input.
  -------------------------------------------------------------------------*/
  input_qos = ioctl_info->qos_ptr;

  /*-------------------------------------------------------------------------
    Find the 3GPP Iface associated with PS iface.
  -------------------------------------------------------------------------*/
  ds_iface_ptr = (ds_3gpp_iface_s *)(iface_ptr->client_data_ptr);

  if(ds_iface_ptr == NULL)
  {
    DATA_ERR_FATAL("PS IFace client data ptr is NULL");
  }

  /*-------------------------------------------------------------------------
    Find the PDN associated with 3GPP Iface.
  -------------------------------------------------------------------------*/ 
  pdn_ptr = (ds_pdn_context_s *)(ds_iface_ptr->client_data_ptr);

  /*-------------------------------------------------------------------------
    Check the input field mask.
  -------------------------------------------------------------------------*/ 
  if(input_qos->field_mask & QOS_MASK_RX_FLOW)
  {
    app_filter_op |= DS_3GPP_APP_RX_FLTR_OP;
  }

  if(input_qos->field_mask & QOS_MASK_TX_FLOW)
  {
    app_filter_op |= DS_3GPP_APP_TX_FLTR_OP;
  }

  if(app_filter_op != DS_3GPP_APP_FLTR_INVALID_OP)
  {
    /*-----------------------------------------------------------------------
      Verify the Identifer for the filters in APP Qos.
    -----------------------------------------------------------------------*/
    if(ds_bearer_flow_manager_verify_app_filter_identifier(
                              input_qos, app_filter_op )==FALSE)
    {
      DS_3GPP_MSG0_ERROR("App Filter Identifier are not valid");
      *ps_errno = DS_EFAULT;
      return -1;
    }

    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

    /*-----------------------------------------------------------------------
      Verify the Precedence for the filters in APP Qos.
    -----------------------------------------------------------------------*/
    if(ds_flow_manager_verify_app_filter_precedence(
                           pdn_ptr,input_qos,FALSE, app_filter_op) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("App Filter Precedence is not valid");
      *ps_errno = DS_EFAULT;
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
       return -1;
    }
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  } /* !=DS_3GPP_APP_FLTR_INVALID_OP */
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid Field Mask sent by the application");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  /*-------------------------------------------------------------------------
    Call 3GPP Function to create PS flow.
    For UMTS, this will create a bearer context and create the flow
    For LTE, the bearer context is not created.
  -------------------------------------------------------------------------*/
  ret_val = ds_flow_manager_3gpp_process_sec_ctx(
                                                 pdn_ptr,
                                                 ds_iface_ptr,
                                                 iface_ptr,
                                                 ioctl_info
                                                 );

  return ret_val;
}/* ds_3gpp_bringup_sec_ctx */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_HANDOFF_CONTEXT_INFO

DESCRIPTION This function handles the PS_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT
            IOCTL.
    
PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on 
  *argval_ptr       - HO context information stored here
  *ps_errno         - Ptr to error code (if any)
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
None
  
===========================================================================*/
LOCAL int ds_3gpp_pdn_cntx_get_handoff_context_info
( 
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds_pdn_context_s                *pdn_cntx_ptr = NULL;
  uint32                           ds_iface_index;
  acl_handoff_ctxt_type           *handoff_cntx_ptr = NULL;
  boolean                          current_handle_found = FALSE;
  boolean                          companion_handle_found = FALSE;
  ps_iface_type                   *temp_iface_ptr = NULL;
  ds_apn_ip_support_type_e         ip_support;
  ps_iface_addr_family_type        current_iface_iptype
                                              = IFACE_INVALID_ADDR_FAMILY;
  ps_iface_addr_family_type        companion_iface_iptype 
                                              = IFACE_INVALID_ADDR_FAMILY;
  ps_iface_ip_config_type         *current_iface_ip_cfg_ptr = NULL;
  ps_iface_ip_config_type         *companion_iface_ip_cfg_ptr = NULL;
  boolean                          exit_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_ptr == NULL || argval_ptr == NULL) 
  {
    DS_3GPP_MSG2_ERROR("Invalid arguments. iface_ptr = 0x%x, argval_ptr= 0x%x",
                       iface_ptr, argval_ptr);
    return -1;
  }

  if (ps_iface_state(iface_ptr) == IFACE_DOWN)
  {
    DS_3GPP_MSG0_ERROR("IOCTL to get HO context called on Down Iface. Ret -1");
    return -1;
  }

  do
  {
    PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
    ds3gpp_enter_global_crit_section();

    /*------------------------------------------------------------------------
      Valid arguments. Proceed with processing the request
    ------------------------------------------------------------------------*/
    handoff_cntx_ptr = (acl_handoff_ctxt_type*)argval_ptr;

    /*------------------------------------------------------------------------
      Get the PDN context pointer
    ------------------------------------------------------------------------*/
    pdn_cntx_ptr = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)
                              (iface_ptr->client_data_ptr))->client_data_ptr);

    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
    {
      break;
    }

    /*------------------------------------------------------------------------
      Determine the IP types for the current iface and companion iface
      and set the appropriate ip cfg container in the handoff ctxt.
      Though we set the companion iface ip cfg container based on the
      current iface ip type, the PDN might be a single bearer and may not
      have a companion iface
    ------------------------------------------------------------------------*/
    current_iface_iptype = ps_iface_get_addr_family(iface_ptr);
    if (IFACE_IPV4_ADDR_FAMILY == current_iface_iptype)
    {
      current_iface_ip_cfg_ptr = 
        &(handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config);
      companion_iface_ip_cfg_ptr = 
        &(handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config);
      companion_iface_iptype = IFACE_IPV6_ADDR_FAMILY;
    }
    else if (IFACE_IPV6_ADDR_FAMILY == current_iface_iptype)
    {
      current_iface_ip_cfg_ptr = 
        &(handoff_cntx_ptr->pdn_ip_config.iface_ipv6_config);
      companion_iface_ip_cfg_ptr = 
        &(handoff_cntx_ptr->pdn_ip_config.iface_ipv4_config);
      companion_iface_iptype = IFACE_IPV4_ADDR_FAMILY;
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Invalid Iface IP Type. Return -1 from IOCTL hdlr");
      break;
    }

    /*------------------------------------------------------------------------
      Init the IP cfg container to be invalid for both IP types
    ------------------------------------------------------------------------*/
    current_iface_ip_cfg_ptr->is_ip_cfg_valid = FALSE;
    companion_iface_ip_cfg_ptr->is_ip_cfg_valid = FALSE;

    for(ds_iface_index = 0; 
        ds_iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
        ds_iface_index++)
    {
      if(pdn_cntx_ptr->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[ds_iface_index]
           == NULL)
      {
        /*----------------------------------------------------------------------
          DS Iface. continue to the next DS iface in PDN context
        ----------------------------------------------------------------------*/
        continue;
      }
      else
      {
        temp_iface_ptr = 
                &(pdn_cntx_ptr->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[ds_iface_index]->ps_iface);
        if( temp_iface_ptr == iface_ptr)
        {
          /*------------------------------------------------------------------
            Mark that we were able to locate the DS Iface corresponding to the
            PS Iface that was passed into the function
          ------------------------------------------------------------------*/
          current_handle_found = TRUE;

          /*------------------------------------------------------------------
            Get the IP Cfg of the iface passed in
          ------------------------------------------------------------------*/
          if (-1 == ps_iface_get_ip_config
                    (iface_ptr, current_iface_ip_cfg_ptr)
             )
          {
            exit_status = TRUE;
            break;
          }
          
#ifdef FEATURE_DATA_PS_IPV6
          /*------------------------------------------------------------------
            Fill up router life time if iface IP type is IPV6
          ------------------------------------------------------------------*/
          if (IFACE_IPV6_ADDR_FAMILY == current_iface_iptype)
          {
            if (-1 == ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime
                      (iface_ptr,
                       &(current_iface_ip_cfg_ptr->router_life_time),
                       ps_errno)
               )
            {
              DS_3GPP_MSG2_ERROR("Unable to fetch router life time for "
                                 "Iface 0x%x:%d",
                                 iface_ptr->name,
                                 iface_ptr->instance);
              current_iface_ip_cfg_ptr->router_life_time = 0;
            }
          }
#endif /* FEATURE_DATA_PS_IPV6 */

          current_iface_ip_cfg_ptr->is_ip_cfg_valid = TRUE;
        } //Found Iface passed into this function
        else
        {
          /*-----------------------------------------------------------------
            This is a Dual IP bearer call. Get the companion IP address
            Mark that we found the companion handle.
          -----------------------------------------------------------------*/
          if (0 == ps_iface_get_ip_config
                    (temp_iface_ptr, companion_iface_ip_cfg_ptr)
             )
          {
#ifdef FEATURE_DATA_PS_IPV6
            /*---------------------------------------------------------------
              Fill up router life time if iface IP type is IPV6
            ---------------------------------------------------------------*/
            if (IFACE_IPV6_ADDR_FAMILY == companion_iface_iptype)
            {
              if (-1 == ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime
                        (temp_iface_ptr,
                         &(companion_iface_ip_cfg_ptr->router_life_time),
                         ps_errno)
                 )
              {
                DS_3GPP_MSG2_ERROR("Unable to fetch router life time for "
                                   "Iface 0x%x:%d", temp_iface_ptr->name,
                                   temp_iface_ptr->instance);
                companion_iface_ip_cfg_ptr->router_life_time = 0;
              }
            }
#endif /* FEATURE_DATA_PS_IPV6 */

            companion_handle_found = TRUE;
            companion_iface_ip_cfg_ptr->is_ip_cfg_valid = TRUE;
          } // Got companion iface's IP cfg
        } //Found companion Iface to the iface passed into this function
      } // DS Iface is non NULL
    } //loop over all DS Ifaces

    if (exit_status == TRUE)
    {
      break;
    }

    if(current_handle_found == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Could not find the iface in the PDN context. Ret -1");
      break;
    }

    if(ds_pdn_cntxt_get_ip_support_info_for_pdn(pdn_cntx_ptr,&ip_support) !=
                                                                   DS3G_SUCCESS)
    {
      DS_3GPP_MSG1_ERROR("Could not get IP Support info. PDN: 0x%x", 
                         pdn_cntx_ptr);
      break;
    }

    /*-----------------------------------------------------------------------
      Make sure that the info that was got from the APN table is consistent
      with the info that has been gathered from this function.
    -----------------------------------------------------------------------*/
    if( (companion_handle_found == TRUE) &&
        (ip_support == DS_IPV4_ONLY ||
         ip_support == DS_IPV6_ONLY ||
         ip_support == DS_IPV4_SINGLE_BEARER || 
         ip_support == DS_IPV6_SINGLE_BEARER) )
    {
      DS_3GPP_MSG0_ERROR("Found companion handle when not expected");
      break;
    }
    else if(companion_handle_found == FALSE && ip_support == DS_IPV4V6)
    {
      DS_3GPP_MSG0_ERROR("Companion Handle not found in Dual IP bearer case");
      break;
    }

    switch(ip_support)
    {
      case DS_IPV4_ONLY:
        handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_V4_ONLY;
        break;

      case DS_IPV6_ONLY:
        handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_V6_ONLY;
        break;

      case DS_IPV4_SINGLE_BEARER: //lint fallthrough
      case DS_IPV6_SINGLE_BEARER:
        handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_SINGLE_BEARER;
        break;

      case DS_IPV4V6:
        handoff_cntx_ptr->bearer_ip_type = PS_IFACE_BEARER_IP_DUAL_BEARER;
        break;

      default:
        DS_3GPP_MSG0_ERROR("Invalid IP Type");
        exit_status = TRUE;
        break;
    } //switch ip_support

    if (exit_status)
    {
      break;
    }

    ds3gpp_leave_global_crit_section();
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return 0;
  } while (0);

  /*----------------------------------------------------------------------
    Handle error conditions. Any error condition will break to here
    and return -1
  ----------------------------------------------------------------------*/
  ds3gpp_leave_global_crit_section();
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
  return -1;
} /* ds_3gpp_pdn_cntx_get_handoff_context_info */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_BEARER_TYPE_HDLR

DESCRIPTION This function handles the PS_IFACE_IOCTL_BEARER_TECHNOLOGY_TYPE
            IOCTL.
    
PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on 
  *argval_ptr       - Bearer information stored here
  *ps_errno         - Ptr to error code (if any)
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
  None
===========================================================================*/
LOCAL int ds_3gpp_pdn_cntx_bearer_type_hdlr
( 
  const ps_iface_type      *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds_bearer_context_s              *bearer_context_ptr = NULL;
  ds_pdn_context_s                 *pdn_cntx_ptr = NULL;
  ps_iface_bearer_technology_type  *bearer_type_ptr =
                                    (ps_iface_bearer_technology_type*)argval_ptr;
  sys_modem_as_id_e_type            subs_id = SYS_MODEM_AS_ID_NONE;
  int                               ret_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("iface_ptr = NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds3gpp_enter_global_crit_section();

  if(ps_iface_state((ps_iface_type *)iface_ptr) == IFACE_DOWN)
  {
    DS_3GPP_MSG0_ERROR("Iface in DOWN state,can't use BEARER_TECH IOCTL");
    *ps_errno = DS_ENETDOWN;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  ASSERT(iface_ptr->client_data_ptr != NULL);

  /*-------------------------------------------------------------------------
    Get the pdn cntx instance.
  -------------------------------------------------------------------------*/
  pdn_cntx_ptr = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)
                              (iface_ptr->client_data_ptr))->client_data_ptr);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get the subs_id from iface_ptr. Validate?
  -------------------------------------------------------------------------*/
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_ptr));

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  /*---------------------------------------------------------------------------
    Get the default bearer context pointer from the PDN context.
  ---------------------------------------------------------------------------*/
  bearer_context_ptr = (ds_bearer_context_s*)
                   (pdn_cntx_ptr->ds_pdn_context_dyn_p->def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
  {
    DS_3GPP_MSG0_ERROR("Bearer Context passed is invalid");
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  ret_val = ds_3gpp_bearer_cntxt_get_bearer_type(bearer_context_ptr,
                                                 bearer_type_ptr,
                                                 ps_errno);
  ds3gpp_leave_global_crit_section();
  return ret_val;
} /* ds_3gpp_pdn_cntx_bearer_type_hdlr */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_BEARER_TYPE_EX_HDLR

DESCRIPTION This function handles the PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX
            IOCTL.
    
PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on 
  *argval_ptr       - Bearer information stored here
  *ps_errno         - Ptr to error code (if any)
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
None
  
===========================================================================*/
LOCAL int ds_3gpp_pdn_cntx_bearer_type_ex_hdlr
( 
  const ps_iface_type      *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds_bearer_context_s             *bearer_context_ptr = NULL;
  ds_pdn_context_s                *pdn_cntx_ptr = NULL;
  int                              ret_val = 0;
  ds_sys_system_status_info_type  *bearer_type_ex_ptr =
                                  (ds_sys_system_status_info_type *)argval_ptr;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type             sys_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("iface_ptr = NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  if (argval_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("argval_ptr = NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds3gpp_enter_global_crit_section();
  if(ps_iface_state((ps_iface_type *)iface_ptr) == IFACE_DOWN)
  {
    DS_3GPP_MSG0_ERROR("Iface in DOWN state,can't use BEARER_TECH IOCTL");
    *ps_errno = DS_ENETDOWN;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  ASSERT(iface_ptr->client_data_ptr != NULL);

  /*-------------------------------------------------------------------------
    Get the pdn cntx instance.
  -------------------------------------------------------------------------*/
  
  pdn_cntx_ptr = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)
                              (iface_ptr->client_data_ptr))->client_data_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  /*-------------------------------------------------------------------------
    Get the subs_id from iface_ptr.
  -------------------------------------------------------------------------*/
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_ptr) );
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  /*---------------------------------------------------------------------------
    Get the default bearer context pointer from the PDN context.
  ---------------------------------------------------------------------------*/
  bearer_context_ptr =
    (ds_bearer_context_s*)
      (pdn_cntx_ptr->ds_pdn_context_dyn_p->def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

//Here the bearer tech should be reported for the underline iface and should be 
//based on the iface call mode.
  sys_mode = bearer_context_ptr->ds_bearer_context_dyn_p->call_mode;

  ret_val = ds_3gpp_bearer_cntxt_get_bearer_type_ex(sys_mode,
                                                    bearer_type_ex_ptr,
                                                    ps_errno,
                                                    subs_id);
  ds3gpp_leave_global_crit_section();
  return ret_val;
} /* ds_3gpp_pdn_cntx_bearer_type_ex_hdlr */
#endif /* FEATURE_DATA_WLAN_MAPCON */

/*==========================================================================
FUNCTION DS_3GPP_PDN_CNTX_BEARER_RATE_HDLR

DESCRIPTION This function returns the tx and rx rates of current call's
            channel.
    
PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on 
  *argval_ptr       - Bearer rate info will be put in here
  *ps_errno         - Ptr to error code (if any)
  
  
DEPENDENCIES
  None.
  
RETURN VALUE
  0 - SUCCESS
  -1 - FAILURE

SIDE EFFECTS 
  None
===========================================================================*/
int ds_3gpp_pdn_cntx_bearer_rate_hdlr
(
  const ps_iface_type      *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_phys_link_type              *phys_link_ptr;
  uint32                          call_inst;
  int                             ret_val = 0;
  sys_modem_as_id_e_type          subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("iface_ptr = NULL");
    return -1;
  }

  /*-------------------------------------------------------------------------
  Get the primary phys link of this iface
  -------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();
  phys_link_ptr = PS_IFACE_GET_PHYS_LINK(((ps_iface_type *)iface_ptr));

  if (phys_link_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("phys_link_ptr = NULL");
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  /*-------------------------------------------------------------------------
  Get the call instance associated with this phys link
  -------------------------------------------------------------------------*/
  call_inst = (uint32)phys_link_ptr->client_data_ptr;
  /*-------------------------------------------------------------------------
  Based on the network mode, call the associated mode handler function
  -------------------------------------------------------------------------*/
  ret_val = -1;

  /*-------------------------------------------------------------------------
    Get the subs_id from iface_ptr. Validate?
  -------------------------------------------------------------------------*/
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_ptr) );

  ret_val = ds_3gpp_bearer_cntxt_get_bearer_rate (call_inst,
                                                  argval_ptr,
                                                  ps_errno,
                                                  subs_id);

  ds3gpp_leave_global_crit_section();
  return ret_val;
} /* ds_3gpp_pdn_cntx_bearer_rate_hdlr() */

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTX_GET_V6_REM_RTR_LIFETIME

DESCRIPTION   Handles the IOCTL to retrive remaining IPv6 router lifetime.
    
PARAMETERS    iface_ptr: iface associated IPv6 PDN 
              argnal_ptr: void pointer to return value
              ps_errno: to indicate any error
              
DEPENDENCIES  

RETURN VALUE   0 on success
               -1 on Failure

SIDE EFFECTS  None.
===========================================================================*/
static int ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ds_3gpp_iface_s*                         ds3gpp_iface_p = NULL;
  ip6_sm_type*                             ip6_sm_ptr = NULL;
  ps_iface_ioctl_v6_router_lifetime_type*  ipv6_rem_rtr_lifetime_ptr;
  int32                                    ipv6_rtr_lifetime;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_MED("3GPP IOCTL - get remaining router lifetime");

  if (ps_errno == NULL) 
  {
    DS_3GPP_MSG0_ERROR("Invalid ps_errno ptr, fail ioctl");
    return -1;
  }

  if (NULL == argval_ptr)
  {
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds3gpp_enter_global_crit_section();

  if (!PS_IFACE_IS_VALID(iface_ptr)) 
  {
    DS_3GPP_MSG0_ERROR("Invalid IFACE ptr, fail ioctl");
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  DS_3GPP_MSG2_MED("processing 3GPP IOCTL - get remaining router lifetime "
                    "for IFACE 0x%x:%d",
                    iface_ptr->name, iface_ptr->instance);

  /*------------------------------------------------------------------------- 
    Get remaining IPv6 router lifetime from ds3gpp_iface_p->ip6_sm.
  -------------------------------------------------------------------------*/
  ipv6_rem_rtr_lifetime_ptr = 
                           (ps_iface_ioctl_v6_router_lifetime_type *)argval_ptr;

  ds3gpp_iface_p = (ds_3gpp_iface_s*)iface_ptr->client_data_ptr;
  if (NULL == ds3gpp_iface_p)
  {
    DS_3GPP_MSG0_ERROR("Invalid ds3gpp_iface_p ptr, fail ioctl");
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  ip6_sm_ptr = &(ds3gpp_iface_p->ip6_sm);
  if (NULL == ip6_sm_ptr)
  {
    DS_3GPP_MSG0_ERROR("Invalid ip6_sm_ptr, fail ioctl");
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  if ((ipv6_rtr_lifetime = ip6_sm_get_remaining_router_lifetime(ip6_sm_ptr)) < 0)
  {
    *ipv6_rem_rtr_lifetime_ptr = 0;
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  *ipv6_rem_rtr_lifetime_ptr = 
                         (ps_iface_ioctl_v6_router_lifetime_type)ipv6_rtr_lifetime;

  DS_3GPP_MSG1_MED( "IPv6 remaining router lifetime is %d", 
                    *ipv6_rem_rtr_lifetime_ptr);    

  DS_3GPP_MSG0_MED( "3GPP IOCTL: Get remaining IPv6 router lifetime successful");
  ds3gpp_leave_global_crit_section();
  return 0;
} /* ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime */

#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_GET_RAB_REESTAB_CALL_TYPE

DESCRIPTION 
  This function returns the RAB RE-ESTAB CALL TYPE associated with the PS
  Iface
 
PARAMETERS    iface_ptr: iface ptr associated with the PDN 
              argval_ptr: void pointer to return value
              ps_errno: to indicate any error
DEPENDENCIES
  The Iface associated with the PDn context has to be UP to
  query the re-establishment call type

RETURN VALUE
  0  - Success
  -1 - Failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_pdn_cntxt_get_rab_reestab_call_type
(
  ps_iface_type *iface_p,
  void          *argval_ptr,
  sint15        *ps_errno
)
{
  int                        ret_val                = -1;
#ifdef FEATURE_DATA_LTE
  ds_pdn_context_s          *pdn_cntx_p             = NULL;
  ds_3gpp_iface_s           *ds_3gpp_iface_p        = NULL;
  ps_iface_state_enum_type   iface_state            = IFACE_STATE_INVALID;
  sys_modem_as_id_e_type     subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type        mode;
  ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type *rab_reestab_call_type_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds3gpp_enter_global_crit_section();
  do
  {
    /*----------------------------------------------------------------------- 
      Validate Input Arguments
      -----------------------------------------------------------------------*/
    if (iface_p == NULL || argval_ptr == NULL) 
    {
      DS_3GPP_MSG2_ERROR("Invalid arguments. iface_ptr = 0x%x, argval_ptr= 0x%x",
                         iface_p, argval_ptr);
      *ps_errno = DS_EFAULT;
      break;
    }

    subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_p) );
    mode = ds3gpp_get_current_network_mode(subs_id);

    /*----------------------------------------------------------------------- 
      Check if the current network Mode is LTE. Set IOCTL HDLR is not
      supported when sys mode is not LTE
      -----------------------------------------------------------------------*/
    if (!DS_3GPP_IS_3GPP_CURRENT_MODE(mode))
    {
      DS_3GPP_MSG1_ERROR("IOCTL Not supported when current network is %d",
                         mode);
      *ps_errno = DS_EOPNOTSUPP;
      break;
    }

    /*----------------------------------------------------------------------- 
      Check if the Iface is in UP state
      -----------------------------------------------------------------------*/
    iface_state = ps_iface_state(iface_p);
    if((iface_state != IFACE_UP) && (iface_state != IFACE_ROUTEABLE))
    {
      DS_3GPP_MSG2_ERROR("Iface_ptr 0x%x in State %d.Cannot get Re-estab Call"
                         "type",iface_p,iface_state);
      *ps_errno = DS_EINVAL;
      break;
    }

    /*----------------------------------------------------------------------- 
      Fetch PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
    if (ds_3gpp_iface_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("NULL DS Iface ptr. Cannot get Re-estab Call type");
      *ps_errno = DS_EFAULT;
      break;
    }
    pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

    /*----------------------------------------------------------------------- 
      Validate the PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      *ps_errno = DS_EFAULT;
      break;
    }
    rab_reestab_call_type_p = (ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type *)
                                 argval_ptr; 
    /*----------------------------------------------------------------------- 
      Depending on the DS_3GPP iface associated with the incoming ps_iface
      return the re-establishment call type for the ps iface
      -----------------------------------------------------------------------*/
    if (ds_3gpp_iface_p == pdn_cntx_p->ds_pdn_context_dyn_p->
                           ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE])
    {
      *rab_reestab_call_type_p = pdn_cntx_p->ds_pdn_context_dyn_p->
                                   rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
    }
    else
    {
      *rab_reestab_call_type_p = pdn_cntx_p->ds_pdn_context_dyn_p->
                                   rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

    }
    DS_3GPP_MSG2_HIGH("RAB Re-estab Call type %d for Iface 0x%x",
                      *rab_reestab_call_type_p,
                      iface_p);
    ret_val = 0;

  }while(0);
  ds3gpp_leave_global_crit_section();
#else
  *ps_errno = DS_EOPNOTSUPP;
  DS_3GPP_MSG1_ERROR("IOCTL req for Iface 0x%x not supported", iface_p);
#endif
  return ret_val;
} /* ds_3gpp_pdn_cntxt_get_rab_reestab_call_type() */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SET_SILENT_REDIAL

DESCRIPTION 
  This IOCTL is called when a client chooses to not end the call,
  It will be MH's responsibility to perform silent redial.

PARAMETERS    iface_ptr: iface ptr associated with the PDN 
              argval_ptr: void pointer to return value
              ps_errno: to indicate any error
DEPENDENCIES
  * The Iface associated with the PDN context has to be UP to
    perform silent redial
  * The current sys mode any of the 3GPP RAT's 
 
RETURN VALUE
  0  - Success
  -1 - Failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_pdn_cntxt_set_silent_redial
(
  ps_iface_type *iface_p,
  void          *argval_ptr,
  sint15        *ps_errno
)
{
  boolean                    ret_val = -1;
  ds_pdn_context_s           *pdn_cntx_p          = NULL;
  ds_3gpp_iface_s            *ds_3gpp_iface_p     = NULL;
  ps_iface_state_enum_type   iface_state          = IFACE_STATE_INVALID;
  ds_cmd_type                *cmd_ptr             = NULL;
  void                       **data_ptr           = NULL;
  boolean                    do_silent_redial     = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ds3gpp_enter_global_crit_section();
  do
  {
    /*----------------------------------------------------------------------- 
      Validate Input Arguments
      -----------------------------------------------------------------------*/
    if ((iface_p == NULL) || (argval_ptr == NULL) || (ps_errno == NULL))
    {
      DS_3GPP_MSG2_ERROR("Invalid arguments. iface_ptr = 0x%x, argval_ptr= 0x%x",
                         iface_p, argval_ptr);
      if (ps_errno != NULL)
      {
        *ps_errno = DS_EFAULT;
      }
      break;
    }

    /*----------------------------------------------------------------------- 
      Do Silent redial is extracted from the ioctl
      -----------------------------------------------------------------------*/
    do_silent_redial = *(boolean*)argval_ptr;
    DS_3GPP_MSG1_HIGH("Do silent redial is %d",do_silent_redial);
    /*----------------------------------------------------------------------- 
      Check the PS iface state, If it is not UP or routable return
      -----------------------------------------------------------------------*/
    iface_state = ps_iface_state(iface_p);
    if((iface_state != IFACE_UP) && (iface_state != IFACE_ROUTEABLE))
    {
      DS_3GPP_MSG2_ERROR("Iface_ptr 0x%x in State %d.Cannot perform silent redial"
                         "type",iface_p,iface_state);
      *ps_errno = DS_EINVAL;
      break;
    }

    /*----------------------------------------------------------------------- 
      Fetch PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
    if (ds_3gpp_iface_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("NULL DS Iface ptr. Cannot perform silent redial");
      *ps_errno = DS_EFAULT;
      break;
    }
    pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

    /*----------------------------------------------------------------------- 
      Validate the PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      *ps_errno = DS_EFAULT;
      break;
    }
    ret_val = 0;

    if(do_silent_redial)
    {
      DS_3GPP_MSG0_MED("Silent redial call back");

      cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        break;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_DO_SILENT_REDIAL;
      data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
      *data_ptr = (void*)pdn_cntx_p;

      ds_put_cmd_ext(cmd_ptr);
    }
    else
    {
      /*------------------------------------------------------------------------ 
      Clear the apn param chg timer, set silent redial = FALSE indicates the 
      client would tear down the call. MH would not have to worry about 
      tearing down the call. 
      However set the force_ota_teardown to TRUE. 
      Force OTA teardown is used for the last pdn, IF not for this boolean, 
      we will never be able to tear down the call.
     ------------------------------------------------------------------------*/
      (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->
                              apn_param_chg_timer));

      DS_3GPP_MSG0_HIGH("Setting force ota teardown to TRUE");

      pdn_cntx_p->ds_pdn_context_dyn_p->force_ota_teardown = TRUE;

    }
  }while(0);
  ds3gpp_leave_global_crit_section();
  return ret_val;

}


/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SET_RAB_REESTAB_CALL_TYPE

DESCRIPTION 
  This function Sets the RAB RE-ESTAB CALL TYPE associated with the PS
  Iface to the specified value

PARAMETERS    iface_ptr: iface ptr associated with the PDN 
              argval_ptr: void pointer to return value
              ps_errno: to indicate any error
DEPENDENCIES
  * The Iface associated with the PDN context has to be UP to
  set the re-establishment call type
  * The current sys mode any of the 3GPP RAT's 
 
RETURN VALUE
  0  - Success
  -1 - Failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_pdn_cntxt_set_rab_reestab_call_type
(
  ps_iface_type *iface_p,
  void          *argval_ptr,
  sint15        *ps_errno
)
{
  int                        ret_val = -1;
#ifdef FEATURE_DATA_LTE
  ds_pdn_context_s          *pdn_cntx_p             = NULL;
  ds_3gpp_iface_s           *ds_3gpp_iface_p        = NULL;
  ps_iface_state_enum_type   iface_state            = IFACE_STATE_INVALID;
  sys_modem_as_id_e_type     subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type        mode;
  ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type rab_reestab_call_type;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds3gpp_enter_global_crit_section();
  do
  {
    /*----------------------------------------------------------------------- 
      Validate Input Arguments
      -----------------------------------------------------------------------*/
    if (iface_p == NULL || argval_ptr == NULL) 
    {
      DS_3GPP_MSG2_ERROR("Invalid arguments. iface_ptr = 0x%x, argval_ptr= 0x%x",
                         iface_p, argval_ptr);
      *ps_errno = DS_EFAULT;
      break;
    }

    subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_p) );
    mode = ds3gpp_get_current_network_mode(subs_id);

    /*-----------------------------------------------------------------------
      Check if the current network Mode is LTE. Set IOCTL HDLR is not
      supported when sys mode is not LTE
      -----------------------------------------------------------------------*/
    if (!DS_3GPP_IS_3GPP_CURRENT_MODE(mode))
    {
      DS_3GPP_MSG1_ERROR("IOCTL Not supported when current network is %d",
                         ds3gpp_get_current_network_mode(subs_id));
      *ps_errno = DS_EOPNOTSUPP;
      break;
    }

    /*----------------------------------------------------------------------- 
      Check if the call type passed in the set IOCTL request is valid
      -----------------------------------------------------------------------*/
    rab_reestab_call_type = 
              *(ps_iface_ioctl_3gpp_lte_reestab_calltype_enum_type*)argval_ptr;

    if ((rab_reestab_call_type != PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_NORMAL) &&
        (rab_reestab_call_type != PS_IFACE_3GPP_LTE_REESTAB_CALLTYPE_VOLTE))
    {
      DS_3GPP_MSG1_ERROR("Invalid ReEstab Call type %d in Set IOCTL request",
                         rab_reestab_call_type);
      *ps_errno = DS_EFAULT;
      break;
    }

    /*----------------------------------------------------------------------- 
      Check if the Iface is in UP state
      -----------------------------------------------------------------------*/
    iface_state = ps_iface_state(iface_p);
    if((iface_state != IFACE_UP) && (iface_state != IFACE_ROUTEABLE))
    {
      DS_3GPP_MSG2_ERROR("Iface_ptr 0x%x in State %d.Cannot set Re-estab Call"
                         "type",iface_p,iface_state);
      *ps_errno = DS_EINVAL;
      break;
    }

    /*----------------------------------------------------------------------- 
      Fetch PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
    if (ds_3gpp_iface_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("NULL DS Iface ptr. Cannot set Re-estab Call type");
      *ps_errno = DS_EFAULT;
      break;
    }
    pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

    /*----------------------------------------------------------------------- 
      Validate the PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      *ps_errno = DS_EFAULT;
      break;
    }

    /*----------------------------------------------------------------------- 
      Set the Re-establishment call type corresponding to the PS Iface
      in the set IOCTL request
      -----------------------------------------------------------------------*/
    if (ds_3gpp_iface_p == pdn_cntx_p->ds_pdn_context_dyn_p->
                               ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE])
    {
      pdn_cntx_p->ds_pdn_context_dyn_p->
          rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV6_IFACE] = rab_reestab_call_type;
    }
    else
    {
      pdn_cntx_p->ds_pdn_context_dyn_p->
          rab_reestab_call_type[DS_3GPP_PDN_CONTEXT_IPV4_IFACE] = rab_reestab_call_type;
    }
    ret_val = 0;
  }while(0);
  ds3gpp_leave_global_crit_section();
#else
  *ps_errno = DS_EOPNOTSUPP;
  DS_3GPP_MSG1_ERROR("IOCTL req for Iface 0x%x not supported",iface_p);
#endif
  return ret_val;
} /* ds_3gpp_pdn_cntxt_set_rab_reestab_call_type() */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SET_OTA_PDN_DISCONNECT_FLAG

DESCRIPTION 
  This function Sets the ota pdn disconnect flag in PDN context
     when IMS requests for it using IOCTL due to SIP failures

PARAMETERS    iface_ptr: iface ptr associated with the PDN 
              argval_ptr: void pointer to return value
              ps_errno: to indicate any error
DEPENDENCIES
  * The Iface associated with the PDN context has to be UP to
  set the re-establishment call type
  * The current sys mode any of the 3GPP RAT's 
 
RETURN VALUE
  0  - Success
  -1 - Failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_pdn_cntxt_set_ota_pdn_disconnect_flag
(
  ps_iface_type *iface_p,
  void          *argval_ptr,
  sint15        *ps_errno
)
{
  int ret_val = 0; 
#ifdef FEATURE_DATA_LTE
  ds_pdn_context_s           *pdn_cntx_p    = NULL;
  ds_3gpp_iface_s            *ds_3gpp_iface_p  = NULL;
  ps_iface_state_enum_type    iface_state   = IFACE_STATE_INVALID;
  sys_modem_as_id_e_type      subs_id = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type         mode = SYS_SYS_MODE_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ds3gpp_enter_global_crit_section();
  do
  {
    /*----------------------------------------------------------------------- 
       Validate Input Arguments
      -----------------------------------------------------------------------*/
    if (NULL == iface_p) 
    {
      DS_3GPP_MSG1_ERROR("Invalid arguments. iface_ptr = 0x%x", iface_p);
     *ps_errno = DS_EFAULT;
      break;
    }

    if(NULL == ps_errno)
    {
      DS_3GPP_MSG1_ERROR("Invalid arguments. ps_errno = 0x%x", ps_errno);
      break;
    }
    
    subs_id = ds3gsubsmgr_subs_id_ds_to_cm( PS_IFACE_GET_SUBS_ID(iface_p) );
    mode = ds3gpp_get_current_network_mode(subs_id);
  
    /*-----------------------------------------------------------------------
         Check if the current network Mode is LTE. Set IOCTL HDLR is not
         supported when sys mode is not LTE
      -----------------------------------------------------------------------*/
    if (!DS_3GPP_IS_3GPP_CURRENT_MODE(mode))
    {
      DS_3GPP_MSG1_ERROR("IOCTL Not supported when current network is %d",
                          ds3gpp_get_current_network_mode(subs_id));
     *ps_errno = DS_EOPNOTSUPP;
     break;
    }
  
    /*----------------------------------------------------------------------- 
       Check if the Iface is in UP state
      -----------------------------------------------------------------------*/
    iface_state = ps_iface_state(iface_p);
    if((iface_state != IFACE_UP) && (iface_state != IFACE_ROUTEABLE))
    {
      DS_3GPP_MSG2_ERROR("Iface_ptr 0x%x in State %d, Exiting !!!!",
                         iface_p, iface_state);
      *ps_errno = DS_EINVAL;
      break;
    }
  
    /*----------------------------------------------------------------------- 
       Fetch PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
    if (ds_3gpp_iface_p == NULL)
    {
      DS_3GPP_MSG0_ERROR("NULL DS Iface ptr.Can't set IMS SIP reg failure flag");
      *ps_errno = DS_EFAULT;
      break;
    }
    pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);
    
    /*----------------------------------------------------------------------- 
      Validate the PDN context associated with the Iface
      -----------------------------------------------------------------------*/
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
    {
      *ps_errno = DS_EFAULT;
      break;
    }
  
    /*----------------------------------------------------------------------- 
      Set the sip failure causel type in the set IOCTL request
     -----------------------------------------------------------------------*/
  
    pdn_cntx_p->ds_pdn_context_dyn_p->ims_sip_reg_failure_flag = TRUE;

    DS_3GPP_MSG0_HIGH("IOCTL req to set ims_sip_reg_failure_flag ");
    ret_val = 0;
  }while(0);

  ds3gpp_leave_global_crit_section();
#else
  *ps_errno = DS_EOPNOTSUPP;
  DS_3GPP_MSG1_ERROR("IOCTL req for Iface 0x%x not supported",iface_p);
#endif
  return ret_val;
}/*  ds_3gpp_pdn_cntxt_set_ota_pdn_disconnect_flag */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_IFACE_IOCTL_GO_NULL

DESCRIPTION   Handles the IOCTL move the state of iface to NULL i.e. tear 
              down the iface

RETURN VALUE  0  : if the IOCTL is processed successfully
              -1 : if the IOCTL is does not get processed

DEPENDENCIES  

SIDE EFFECTS  
===========================================================================*/
static int  ds_3gpp_pdn_cntx_iface_ioctl_go_null
(
  ps_iface_type            *iface_ptr,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  int ret_val = 0; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(iface_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("3GPP iface ioctl go null called with NULL iface");
    return -1;
  }

  DS_3GPP_MSG0_HIGH("3GPP iface ioctl - go NULL");

  ret_val = ps_iface_go_null_cmd(iface_ptr, ps_errno, NULL);

  if (ret_val == 0)
  {
    DS_3GPP_MSG0_HIGH("Iface already down");
    return ret_val;
  }
  if ((ret_val < 0) &&
      (*ps_errno != DS_EWOULDBLOCK))
  {
    DS_3GPP_MSG1_HIGH("Iface tear down failed, errno: %d",ps_errno);
    return ret_val;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Iface tear down in progress");
    return 0;
  }
} /* ds_3gpp_pdn_cntx_iface_ioctl_go_null */

/*===========================================================================
FUNCTION DSPDNCNTX_IFACEIOCTLHDLR

DESCRIPTION
  iface ioctl Callback called from application context for secondary PDP
  context activation. Central function for embedded applications to
  activate the secondary context.

PARAMETERS
  *iface_ptr        - Ptr to the interface to operate on
  ioctl_name        - Type of request
  argval_ptr        - QoS specified by the application
  *ps_errno         - Ptr to error code (if any)
              
DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure

SIDE EFFECTS
VRK:TODO Add Ioctl hdlrs
===========================================================================*/
static int ds_pdn_cntxt_ifaceIoctlHdlr
(
  ps_iface_type            *iface_ptr,
  ps_iface_ioctl_type      ioctl_name,
  void                     *argval_ptr,
  sint15                   *ps_errno
)
{
  ps_iface_ioctl_qos_request_type               *ioctl_info = NULL;
  ps_iface_ioctl_3gpp_session_params_info_type  *session_ptr;
  int                                            ret_val = 1;
  ds_3gpp_iface_s                               *ds_iface_ptr = NULL;
  ds_pdn_context_s                              *pdn_ptr  = NULL;
  sys_modem_as_id_e_type                         subs_id =SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_IMS
  boolean                                       *im_cn_flag_status = NULL;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    For bringing up or down primary PDP context, phys link command callback
    functions of ps iface are used.
    For bringing up the secondary PDP context, ps iface IOCTL function call
    back and phys link command call back function are used.
    For bring down or modifying secondary PDP context, phys link ioctl
    callback and phys link command callback functions of phys link are used.

    Bringup Iface                        : dsumtsps_iface_up
    Teardown Iface                       : dsumtsps_iface_down
    Bringup Primary Phys link            : dsumtsps_phys_link_up
    Teardown Primary Phys link           : dsumtsps_phys_link_down
    Bringup  Secondary Context           : dsumtsps_iface_ioctl_hdlr
    Teardown Secondary Context           : dsumtsps_flow_ioctl_hdlr
    Bringup Secondary context phys link  : dsumtsps_phys_link_up
    Teardown Secondary context phys link : dsumtsps_phys_link_down
  -------------------------------------------------------------------------*/

  DS_3GPP_MSG1_MED("Received %d", ioctl_name);

  switch ( ioctl_name )
  {
    /*-----------------------------------------------------------------------
      IOCTL for returning bearer technology type WCDMA/HS/GPRS/EDGE
    -----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY:
      ret_val = ds_3gpp_pdn_cntx_bearer_type_hdlr(iface_ptr,
                                                  argval_ptr,
                                                  ps_errno);
      break;

#ifdef FEATURE_DATA_WLAN_MAPCON
     case PS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY_EX:
      ret_val = ds_3gpp_pdn_cntx_bearer_type_ex_hdlr(iface_ptr,
                                                     argval_ptr,
                                                     ps_errno);
      break;
#endif /* FEATURE_DATA_WLAN_MAPCON */

      /*---------------------------------------------------------------------
        IOCTL for returning bearer rates: max, avg & current
      ---------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_GET_DATA_BEARER_RATE:
      ret_val = ds_3gpp_pdn_cntx_bearer_rate_hdlr(iface_ptr,
                                                  argval_ptr,
                                                  ps_errno );
      break;

    case PS_IFACE_IOCTL_QOS_REQUEST:
      ioctl_info = (ps_iface_ioctl_qos_request_type*)argval_ptr;
      ASSERT ( iface_ptr != NULL );
      ASSERT ( ioctl_info != NULL );
      /*---------------------------------------------------------------------
      Find the 3GPP Iface associated with PS iface.
      ---------------------------------------------------------------------*/
      ds_iface_ptr = (ds_3gpp_iface_s *)(iface_ptr->client_data_ptr);
      if(ds_iface_ptr == NULL)
      {
        DATA_ERR_FATAL("PS IFace client data ptr is NULL");
      }
      /*---------------------------------------------------------------------
      Find the PDN associated with 3GPP Iface.
      ---------------------------------------------------------------------*/
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      ds3gpp_enter_global_crit_section();
      pdn_ptr = (ds_pdn_context_s *)(ds_iface_ptr->client_data_ptr);

      if (ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_ptr, &subs_id))
      {
        if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_ptr) == TRUE)
        {
          DS_3GPP_MSG0_ERROR("Emergency PDN:UE Inited dedicated bearer "
                             "allocation not allowed");
          ret_val = -1;
          *ps_errno = DS_ECONNREFUSED;
        }

         if( ds_3gpp_pdn_throt_is_apn_globally_throttled(
               pdn_ptr->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
               subs_id ))
         {
           DS_3GPP_MSG0_ERROR("APN is blocked, can't proceed with QOS req");
           ret_val = -1;
           *ps_errno = DS_ECONNREFUSED;
         }
         else
         {
          ret_val = ds_3gpp_bringup_sec_ctx(iface_ptr,
                                            ioctl_info,
                                            ps_errno,
                                            subs_id);
         }
      }
      else
      {
        DS_3GPP_MSG1_ERROR("PDN context could not fetch valid Subs id(CM) %d",
                           subs_id);
      }
      ds3gpp_leave_global_crit_section();
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      break;

    /*----------------------------------------------------------------------
      IOCTL for registering a callback for a Mobile Terminated request.
    ----------------------------------------------------------------------*/
     case PS_IFACE_IOCTL_MT_REG_CB:
      *ps_errno =  DS_EOPNOTSUPP;
      ret_val = -1;
      //   mt_reg_callback_info = (ps_iface_ioctl_mt_reg_cb_type*)argval_ptr;
      /*ret_val = dsumtsps_mt_register_callback(mt_reg_callback_info,iface_ptr,
                                                                 ps_errno); */
      break;

     /*----------------------------------------------------------------------
       IOCTL for de-registering a callback for a Mobile Terminated request
     ----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_MT_DEREG_CB:
      *ps_errno =  DS_EOPNOTSUPP ;
      ret_val = -1;
      //mt_dereg_callback_info = (ps_iface_ioctl_mt_dereg_cb_type*)argval_ptr;
      //ret_val = dsumtsps_mt_deregister_callback(mt_dereg_callback_info, ps_errno);
      break;

#ifdef FEATURE_DATA_IMS
    /*----------------------------------------------------------------------
      IOCTL for querying the value of IM CN Flag on the iface.
    ----------------------------------------------------------------------*/
    case PS_IFACE_IOCTL_UMTS_GET_IM_CN_FLAG:
      im_cn_flag_status = (boolean *)argval_ptr;
      ret_val = ds_bearer_cntxt_get_IMCN_subsystem_flag_for_iface(iface_ptr,
                   im_cn_flag_status, ps_errno);
      break;


#endif

    case PS_IFACE_IOCTL_3GPP_GET_LTE_REESTAB_CALLTYPE:
      ret_val = ds_3gpp_pdn_cntxt_get_rab_reestab_call_type(iface_ptr,
                                                            argval_ptr,
                                                            ps_errno);
      break;

    case PS_IFACE_IOCTL_3GPP_SET_LTE_REESTAB_CALLTYPE:
      ret_val = ds_3gpp_pdn_cntxt_set_rab_reestab_call_type(iface_ptr,
                                                            argval_ptr,
                                                            ps_errno);
      break;

    case PS_IFACE_IOCTL_3GPP_FORCE_PDN_DISCONNECT:
      ret_val = ds_3gpp_pdn_cntxt_set_ota_pdn_disconnect_flag(iface_ptr,
                                                              argval_ptr,
                                                              ps_errno);
      break;

    case PS_IFACE_IOCTL_SET_SILENT_REDIAL:
      ret_val = ds_3gpp_pdn_cntxt_set_silent_redial(iface_ptr,
                                                    argval_ptr,
                                                    ps_errno);
      break;

    case PS_IFACE_IOCTL_GET_EPC_HANDOFF_CTXT:
      ret_val = ds_3gpp_pdn_cntx_get_handoff_context_info(iface_ptr,
                                                          argval_ptr,
                                                          ps_errno);
      break;

    case PS_IFACE_IOCTL_GO_NULL:
      ret_val = ds_3gpp_pdn_cntx_iface_ioctl_go_null(iface_ptr,
                                                     argval_ptr,
                                                     ps_errno);
      break;

    case PS_IFACE_IOCTL_UMTS_GET_SESSION_PARAMS:
      session_ptr = (ps_iface_ioctl_3gpp_session_params_info_type*)argval_ptr;
      ret_val = ds_3gpp_pdn_cntxt_get_session_params(iface_ptr,
                                                     session_ptr,
                                                     ps_errno);
      break;

    case PS_IFACE_IOCTL_UMTS_GET_PDP_CONTEXT_ID:
      ret_val = ds_3gpp_pdn_cntxt_get_pdp_profile_num(iface_ptr,
                                                      argval_ptr,
                                                      ps_errno);
      break;

    /* This is a NO OP for 3GPP. Added for consistency among 3gpp/3gpp2 */
    case PS_IFACE_IOCTL_RECONFIG:
      ret_val = 0;
      break;


#ifdef FEATURE_DATA_PS_IPV6
    case PS_IFACE_IOCTL_GET_IPV6_REMAINING_ROUTER_LIFETIME:
      ret_val = ds_3gpp_pdn_cntx_get_v6_rem_rtr_lifetime(iface_ptr,
                                                         argval_ptr,
                                                         ps_errno);
      break;
#endif /* FEATURE_DATA_PS_IPV6 */

    default:
      DS_3GPP_MSG1_ERROR("Invalid request from IOCTL: %d", ioctl_name);
      *ps_errno =  DS_EINVAL;
      ret_val = -1;
      break;
  }

  return ret_val;
} /* ds_pdn_cntxt_ifaceIoctlHdlr() */

/*===========================================================================
FUNCTION DSPDNCNTX_RETURN_PDN_CNTX_TO_FREE_POOL

DESCRIPTION
  This function is used to return a PDN context to the free pool.

PARAMETERS
  IN
    pdn_cntx_p - This is the PDN context that has to be marked as free

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds_pdn_cntxt_return_pdn_cntx_to_free_pool
(
  ds_pdn_context_s* pdn_context_p
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ds3gpp_enter_global_crit_section();
  if (!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)))
  {
    ds3gpp_leave_global_crit_section();
    return;
  }

  ds_3gpp_pdn_cntx_free_dyn_mem(pdn_context_p, subs_id);
  ds3gpp_leave_global_crit_section();
} /* ds_pdn_cntxt_return_pdn_cntx_to_free_pool */


/*===========================================================================
FUNCTION      ds_3gpp_pdn_context_cleanupDSIface

DESCRIPTION
  Resets certain ds3gpp_iface fields

PARAMETERS
  ds3gpp_iface_p   - Pointer to the DS iface that is to be cleaned up.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_pdn_context_cleanupDSIface
(
  ds_3gpp_iface_s  *ds3gpp_iface_p
)
{
  ds_pdn_context_s    *pdn_cntx_p =  NULL; 
  uint8                index = 0;                                   
    /*---------------------------------------------------------------------*/
  
  ASSERT (ds3gpp_iface_p != NULL);
  ds3gpp_enter_global_crit_section();
 
  pdn_cntx_p = ds3gpp_iface_p->client_data_ptr;
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    for(index = 0; index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; index++)
    {
      if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[index] == ds3gpp_iface_p)
      {
         pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[index] = NULL;
         break;
      }
    }
  }
  /*-----------------------------------------------------------------------
  Break the binding between DS Iface and PDN context. Clear the in_use
  flag as well. 
  Also set the post proc done flag to FALSE
  Also clear the a2_bridging flags to FALSE 
  -----------------------------------------------------------------------*/

  ds3gpp_iface_p->client_data_ptr = NULL;
  ds3gpp_iface_p->in_use = FALSE;


  ds3gpp_iface_p->post_proc_done_flag = FALSE;
  ds3gpp_iface_p->state = DS_3GPP_IFACE_INVALID_STATE;
    
  ds3gpp_iface_p->ue_init_net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
    
  ds3gpp_leave_global_crit_section();
  return;
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_HANDLE_BEARER_DOWN_IND

DESCRIPTION
  We can reset the Attach SM only if following conditions are met:
  1. Bearer down indication is received for the default bearer
  2. PDN context corresponding to that bearer is the only active PDN context.
  3. Attach SM is in DS_EPS_PROCESS_GET_PDN_CONN_IND or DS_EPS_ATTACHED state.

PARAMETERS
  Bearer context
  Net down reason

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_handle_bearer_down_ind
(
  ds_bearer_context_s *bearer_cntx_p,
  ps_iface_net_down_reason_type down_reason
)
{
  ds_pdn_context_s         *pdn_cntx_p          = NULL;
  sys_modem_as_id_e_type    subs_id             = SYS_MODEM_AS_ID_NONE;
  ds_eps_attach_sm_s       *attach_sm_ptr       = NULL;
  ds_eps_attach_state_e     eps_attach_state    = DS_EPS_ATTACH_STATE_NULL;
  ds_dsd_ext_error_code     apm_error_code;
  /*-----------------------------------------------------------------------*/

  DS_3GPP_MSG0_HIGH("Entered ds_3gpp_pdn_cntxt_handle_bearer_down_ind ");

  
  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }


  pdn_cntx_p = (ds_pdn_context_s*)bearer_cntx_p->ds_bearer_context_dyn_p->
     pdn_context_ptr;


  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }
  if (FALSE == bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    /* No special handling for dedicated bearers */
    return;
  }

  if(!dsPdnContext_is_any_other_normal_pdn_up(pdn_cntx_p, subs_id) &&
      (ds_3gpp_pdn_cntxt_is_any_pdn_pending_redial(subs_id)) &&
      (ds_eps_attach_sm_get_state(subs_id) == DS_EPS_PROCESSED_CNTXT_TXFR_IND))
  {
    /* Reset Attach SM here and return to allow Next Attach to happen */
    DS_3GPP_MSG1_HIGH("DBG:Reset the attach SM to NULL for Subs Id (CM) %d", subs_id);
    ds_eps_pdn_cntx_reset_attach_sm_params(subs_id);
    return;
  }

  if (TRUE == dsPdnContext_is_any_other_normal_pdn_active(pdn_cntx_p))
  {
    /* We have other PDN connections active. */
    return;
  }

  /*-------------------------------------------------------------------
    Notify APM module for LTE EMM DETACH OR ATTACH REJECT
  --------------------------------------------------------------------*/
  apm_error_code = ds_3gpp_apn_switch_mgr_send_ind_to_apm(subs_id);

  DS_3GPP_MSG1_MED("Posted Indication to APM, Err code: %d", 
                  apm_error_code);

  /* If there is an emergency call up and this is not it; then when this
     call ends only the emergency call will be up. That means that the
     UE enters the Emergency Attached state. */
  if ( (FALSE == DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p)) &&
       (TRUE == ds_3gpp_pdn_context_emergency_pdn_is_up(FALSE, subs_id)) )
  {
      ds_eps_attach_sm_set_state(DS_EPS_EMERGENCY_ATTACHED,subs_id);            
  }
  else 
  {
    /*----------------------------------------------------------------------- 
       We might be emergency attached and the emergency PDN is going down.
       Or there might be no emergency call and this is the last connected PDN.
       In either case when this call is ended; we have to reset our
       SM since we will no longer be attached on LTE.
      -----------------------------------------------------------------------*/
      eps_attach_state = ds_eps_attach_sm_get_state(subs_id);
      switch (eps_attach_state)
      {
        case DS_EPS_ATTACHED:
        case DS_EPS_EMERGENCY_ATTACHED:
        case DS_EPS_PROCESSED_CNTXT_TXFR_IND:
          DS_3GPP_MSG1_HIGH("Reset the attach SM to NULL for Subs Id (CM) %d",
                           subs_id);
          ds_eps_pdn_cntx_reset_attach_sm_params(subs_id);
          break;
        case DS_EPS_PROCESSING_GET_PDN_CONN_IND:
          ds_eps_pdn_cntxt_reset_target_rat(pdn_cntx_p);
          ds_eps_pdn_cntx_reset_attach_sm_params(subs_id);
          attach_sm_ptr = ds_eps_get_attach_sm_per_sub_id(subs_id); 
          if (attach_sm_ptr != NULL)
          {
            attach_sm_ptr->prev_attach_status.is_attach_fail  = TRUE;
            attach_sm_ptr->prev_attach_status.down_reason     = down_reason;
            DS_3GPP_MSG1_HIGH("Record attach down reason for next attempt:0x%x",
                              attach_sm_ptr->prev_attach_status.down_reason);
          }
          else
          {
            DS_3GPP_MSG1_ERROR("Attach SM for Subs Id (CM) %d NULL."
                              "Cannot Record down reason", subs_id);
          } 
          break;
        default:
         break;
      }

    /*--------------------------------------------------------------------- 
      In the global throttling case do not reset the table, as throttling 
      should be carried forward if it attaches to same PLMN again
    -----------------------------------------------------------------------*/
    if((!ds_3gpp_nv_manager_get_global_throttling(subs_id)) && 
        !ds_3gppi_is_rel_11_supported())
    {
      ds_3gpp_network_override_hdlr_reset(subs_id);
    }
  }


} /* ds_3gpp_pdn_cntxt_handle_bearer_down_ind */
#endif /* FEATURE_DATA_LTE */  

/*===========================================================================
FUNCTION ds_pdn_cntxt_freeInstance

DESCRIPTION
  This function frees the pdn context.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cntxt_freeInstance
(
  ds_pdn_context_s    *pdn_cntx_p          /* Unique PDN context instance  */  
)
{   
  
  ds_3gpp_iface_s   *ds3gpp_iface_p = NULL;
  int               iface_index;
  ds_bearer_context_s  *bearer_context_p = NULL;
  uint8                 bearer_index = 0;
  uint8                 num_active_bearers = 0;
#ifdef FEATURE_EMERGENCY_PDN
  sys_modem_as_id_e_type    subs_id;
#endif /* FEATURE_EMERGENCY_PDN */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p) == FALSE )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG1_HIGH("Freeing pdn cntx Pointer:0x%x",pdn_cntx_p);
    
  for(bearer_index = 0; 
      bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT; 
      bearer_index++ )
  {
    if (pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[bearer_index] != NULL)
    {     
      bearer_context_p = 
         pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[bearer_index];
      num_active_bearers++;
    }
	 	
  }

  DS_3GPP_MSG3_HIGH("ds_pdn_cntxt_freeInstance: number of active bearerduring pdn freeing %d", 
  	                num_active_bearers, 0, 0);
  ASSERT(num_active_bearers <= 1);
  
  /*-----------------------------------------------------------------------
    Break the link that the bearer has to the PDN context
  -----------------------------------------------------------------------*/
  if(ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    for(iface_index = 0; iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
         iface_index++)
    {
      ds3gpp_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
        [iface_index];

      if (ds3gpp_iface_p != NULL)
      {
        /*---------------------------------------------------------------
        Clean up RM iface for sanity
        ---------------------------------------------------------------*/
        ds3gpp_iface_p->rm_iface_ptr = NULL;
      }
    }
#ifdef FEATURE_DATA_LTE
    /*----------------------------------------------------------------------- 
      Reset the Attach state machine, if this is the last of PDN's to be
      freed. This can happen when DS receives
      1.	PDN Connection Reject
      2.	PDN Connection Fail
      3.	Network Detach
      (or)
      4.	DS rejects Activate bearer indication for last PDN
      -----------------------------------------------------------------------*/
     
    ds_3gpp_pdn_cntxt_handle_bearer_down_ind(bearer_context_p,
                            pdn_cntx_p->ds_pdn_context_dyn_p->net_down_reason);


#endif
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    /*-----------------------------------------------------------------------
      Release the bearer flow manager
    -----------------------------------------------------------------------*/
    ds_bearer_flow_manager_release_flow_manager(bearer_context_p);
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

    ds3gpp_enter_global_crit_section();
  /*-----------------------------------------------------------------------
      Return the bearer context to the free pool
      This is the only place where the default bearer is marked as out of use
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_deregister_pdn_context(bearer_context_p);

    /*-----------------------------------------------------------------------
      Return the bearer context to the free pool
      This is the only place where the default bearer is marked as out of use
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
    ds3gpp_leave_global_crit_section();
  }
  
  /*---------------------------------------------------------------------
    This feature will never be used. It is only to track this change so
    that we can undo it later.
  ----------------------------------------------------------------------*/  

  /*-----------------------------------------------------------------------
    Make changes to the fields of the PDN context.
    Reset the in_use flag and Change the state to DOWN 
  -----------------------------------------------------------------------*/
  pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_DOWN;
  
  pdn_cntx_p->ds_pdn_context_dyn_p->pdp_group_id = 0;
  
  pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr = NULL;  

#ifdef FEATURE_EMERGENCY_PDN
  if(DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == TRUE) 
  {
    ds_3gpp_pdn_cntxt_set_emergency_auth_fail(FALSE);
    /*------------------------------------------------------------------------
    For emergency PDN clean up, notify DSD that emergency PDN is down.
    ------------------------------------------------------------------------*/
    if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== TRUE)
    {
      ds3gdsdif_set_lte_emergency_call_in_prog(subs_id, FALSE);
    }
  }
#endif /* FEATURE_EMERGENCY_PDN*/

  memset(pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl,0,
         sizeof(pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl));

  memset( (void*) &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile),
          0,sizeof(ds_umts_pdp_profile_type) );

  #ifdef FEATURE_EMERGENCY_PDN
  ds_3gpp_pdn_cntxt_handle_emergency_attach_status(pdn_cntx_p);
  #endif /* FEATURE_EMERGENCY_PDN*/
  /*------------------------------------------------------------------------
    Reset the HO Attach Flag
  ------------------------------------------------------------------------*/
  pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag = FALSE;

  /*------------------------------------------------------------------------
    Reset the local disconnect flag
  ------------------------------------------------------------------------*/
  pdn_cntx_p->ds_pdn_context_dyn_p->local_disconnect_sent = FALSE;

  /*------------------------------------------------------------------------
    Reset the allow_iface_bring_up
  ------------------------------------------------------------------------*/
  pdn_cntx_p->ds_pdn_context_dyn_p->allow_iface_bring_up = FALSE;

  pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress = FALSE;

  pdn_cntx_p->ds_pdn_context_dyn_p->is_default = FALSE;
  /*------------------------------------------------------------------------
    Free the event callback buffers that were allocated as part of 
    default bearer bring up
  ------------------------------------------------------------------------*/
  for(iface_index = 0; 
       iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
       iface_index++)
  {
    ds3gpp_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[iface_index];
    if (ds3gpp_iface_p != NULL)
    {
      ds_3gpp_pdn_context_cleanupDSIface(ds3gpp_iface_p);    
    }
  } /* for Iface_index */

  /*------------------------------------------------------------------------
     Undefine the timers. 
  ------------------------------------------------------------------------*/

  rex_undef_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer));


  rex_undef_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->polling_timer));

  rex_undef_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_timer));
  /*---------------------------------------------------------------------------
    Clean up the DS Ifaces.
  ---------------------------------------------------------------------------*/
  memset(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl,0,
           sizeof(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl));
  
  /*-----------------------------------------------------------------------
    Release the flow manager
  ---------------------------------------------------------------------*/
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  ds_flow_manager_release_flow_manager(pdn_cntx_p); 
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  /*---------------------------------------------------------------------------
    The reclaiming of the PDN context happens through this function and is
    protected by a critical section.
  ---------------------------------------------------------------------------*/
  ds_pdn_cntxt_return_pdn_cntx_to_free_pool(pdn_cntx_p);        

}

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_validate_pcscf_dns_address

DESCRIPTION
  This function validates if there is a requirement for an APN to have either
  for DNS or PCSCF address
 
  If the requirement is not met then the function triggers a PDN teardown

PARAMETERS 
  pdn_context_p (IN) :  PDN context object

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the requirement to have either PCSCF or DNS is met
            FALSE if the above requirement is not met
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_pcscf_dns_addr
(
  ds_pdn_context_s                           *pdn_context_p,
  boolean                                     dns_addr_valid,
  boolean                                     pcscf_addr_valid
)
{
  boolean                                     pcscf_or_dns_addr_valid = FALSE;
  sys_modem_as_id_e_type                      subs_id = SYS_MODEM_AS_ID_NONE;
  ds_dsd_apm_req_pay_load                     apm_req_payload;      
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return pcscf_or_dns_addr_valid;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return pcscf_or_dns_addr_valid;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return pcscf_or_dns_addr_valid;
  }

  /*---------------------------------------------------------------------------
    This function queries APM module if the PDN is an on-demand PDN to check
    whether either of PCSCF or DNS address is mandatory,
   
    If its mandatory and we have neither of them valid (either v4 or v6)
    then we initiate a PDN teardown
  ---------------------------------------------------------------------------*/

  if(FALSE == dns_addr_valid &&
     FALSE == pcscf_addr_valid)
  {
    if(TRUE == dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id))
    {
      memset(&apm_req_payload, 0, sizeof(ds_dsd_apm_req_pay_load));

      apm_req_payload.profile_id =
         pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num;

      apm_req_payload.rule_type = DS_DSD_APM_ON_DEMAND_RULE;
      ds_dsd_apm_req_ex(DS_DSD_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ,
                        &apm_req_payload, 
                        subs_id);

      if((ipv4_mandatory_or_ipv6_mandatory == 
                          apm_req_payload.mandatory_pcscf_or_dns_addr))
      {
        /* Bring down this pdn context. */
        ds_3gpp_pdn_cntx_teardown_PDN(pdn_context_p,
                             PS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR);
      
      } /*Print message if returned enum from APM is not valid*/
      else
      {
        DS_3GPP_MSG1_MED("Invalid Enum value for PCSCF or DNS address: %d", 
                          apm_req_payload.mandatory_pcscf_or_dns_addr);
        pcscf_or_dns_addr_valid = TRUE;
      }
    }
    else
    {
      DS_3GPP_MSG0_MED("No validation needed for attach since APM handles it");
      pcscf_or_dns_addr_valid = TRUE;
    }
  }
  else
  {
  /*debug*/
   memset(&apm_req_payload, 0, sizeof(ds_dsd_apm_req_pay_load));

      apm_req_payload.profile_id =
         pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num;

      apm_req_payload.rule_type = DS_DSD_APM_ON_DEMAND_RULE;
      ds_dsd_apm_req_ex(DS_DSD_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ,
                        &apm_req_payload,
                        subs_id);
          DS_3GPP_MSG1_MED("debug: Enum value for PCSCF or DNS address: %d", 
                          apm_req_payload.mandatory_pcscf_or_dns_addr);
  /*debug*/   
    DS_3GPP_MSG2_MED("No PDN teardown since either DNS or PCSCF addr "
                      "is valid: %d %d", dns_addr_valid, pcscf_addr_valid);
    pcscf_or_dns_addr_valid = TRUE;
  }

  return pcscf_or_dns_addr_valid;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_validate_dns_addr

DESCRIPTION
  This function validates if the network returned DNS address is consistent
  with the rules specified in the APM.
 
  If the rules defined in APM are not met by the Ifaces in this PDN context;
  a command is queued to tear down the PDN connection.

PARAMETERS 
  pdn_context_p (IN) :  PDN context object

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if DNS addreess is valid according to the rules
            FALSE if DNS adderess is not valid according to the rules
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_dns_addr
(
  ds_pdn_context_s                           *pdn_context_p,
  boolean                                    *v4_or_v6_dns_addr_valid
)
{
  sys_modem_as_id_e_type                      subs_id = SYS_MODEM_AS_ID_NONE;
  ps_iface_type                              *v4_ps_iface_ptr = NULL;
  ps_iface_type                              *v6_ps_iface_ptr = NULL;
  ip_addr_type                                prim_dns, sec_dns;
  boolean                                     v4_dns_addr_valid = FALSE;
  boolean                                     v6_dns_addr_valid = FALSE;
  boolean                                     dns_addr_valid = TRUE;
  ds_dsd_apm_ind_pay_load                     apm_ind_payload;          
  ds_dsd_apm_req_pay_load                     apm_req_payload;          
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return FALSE;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  if(NULL == v4_or_v6_dns_addr_valid)
  {
    DS_3GPP_MSG0_ERROR("Invalid Boolean params passed for DNS addr validation");
    return FALSE;
  }

  /*---------------------------------------------------------------------------
    This function queries APM module if the PDN is an on-demand PDN to check
    whether either DNS address is mandatory,
   
    If either v4 or v6 is mandatory and its invalid then we initiate a PDN
    teardown
   
    For Attach PDN we indicate to APM the status of DNS address, both v4 & v6
  ---------------------------------------------------------------------------*/

  v4_ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);
  v6_ps_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_context_p);

  if(NULL != v4_ps_iface_ptr)
  {
    memset(&prim_dns, 0, sizeof(ip_addr_type));
    memset(&sec_dns, 0, sizeof(ip_addr_type));
    ps_iface_get_v4_dns_addrs(v4_ps_iface_ptr, &prim_dns, &sec_dns);

    if(IP_ADDR_INVALID == prim_dns.type &&
       IP_ADDR_INVALID == sec_dns.type)
    {
      v4_dns_addr_valid = FALSE;
    }
    else
    {
      v4_dns_addr_valid = TRUE;
    }
  }

  if(NULL != v6_ps_iface_ptr)
  {
    memset(&prim_dns, 0, sizeof(ip_addr_type));
    memset(&sec_dns, 0, sizeof(ip_addr_type));
    ps_iface_get_v6_dns_addrs(v6_ps_iface_ptr, &prim_dns, &sec_dns);

    if(IP_ADDR_INVALID == prim_dns.type &&
       IP_ADDR_INVALID == sec_dns.type)
    {
      v6_dns_addr_valid = FALSE;
    }
    else
    {
      v6_dns_addr_valid = TRUE;
    }
  }

  if(FALSE == dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id))
  {
    memset(&apm_ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));
    DS_3GPP_APM_SET_V4_DNSADDR(apm_ind_payload, v4_dns_addr_valid, subs_id);
    DS_3GPP_APM_SET_V6_DNSADDR(apm_ind_payload, v6_dns_addr_valid, subs_id);
    DS_3GPP_APM_SET_PROFILE_ID(apm_ind_payload, 
                    pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num);

    ds_dsd_apm_ind(DS_DSD_APM_IP_ADDRESS_IND, &apm_ind_payload);
  }
  else
  {
    memset(&apm_req_payload, 0, sizeof(ds_dsd_apm_req_pay_load));

    apm_req_payload.profile_id =
       pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num;

    apm_req_payload.rule_type = DS_DSD_APM_ON_DEMAND_RULE;
    ds_dsd_apm_req_ex(DS_DSD_APM_MANDATORY_DNS_ADDR_REQ,
                      &apm_req_payload, 
                      subs_id);

    if(FALSE == v4_dns_addr_valid &&
       (ipv4_mandatory_and_ipv6_donotcare == apm_req_payload.mandatory_dns_addr ||
        ipv4_mandatory_and_ipv6_mandatory == apm_req_payload.mandatory_dns_addr))
    {
      dns_addr_valid = FALSE;
    }

    if(TRUE == dns_addr_valid &&
       FALSE == v6_dns_addr_valid && 
       (ipv6_mandatory_and_ipv4_donotcare == apm_req_payload.mandatory_dns_addr ||
        ipv4_mandatory_and_ipv6_mandatory == apm_req_payload.mandatory_dns_addr))
    {
      dns_addr_valid = FALSE;
    }

    if(TRUE == dns_addr_valid &&
       FALSE == v6_dns_addr_valid &&
       FALSE == v4_dns_addr_valid &&
       ipv4_mandatory_or_ipv6_mandatory == apm_req_payload.mandatory_dns_addr)
    {
      dns_addr_valid = FALSE;
    }

    if (FALSE == dns_addr_valid)
    {
      /* Bring down this pdn context. */
      ds_3gpp_pdn_cntx_teardown_PDN(pdn_context_p,
                           PS_NET_DOWN_REASON_INVALID_DNS_ADDR);
    }
  }

  *v4_or_v6_dns_addr_valid = (v4_dns_addr_valid | v6_dns_addr_valid);
  return dns_addr_valid;
}

/*===========================================================================
FUNCTION dsPdnContext_bearerUpIndCb

DESCRIPTION
  This function completes PS iface configuration in response to bearer up ind  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  

===========================================================================*/
void dsPdnContext_bearerUpIndCb
( 
  void                       *object,
  ds_bearer_context_s        *bearer_cntx_p,
  const cm_call_mode_info_s_type   *mode_info_p
)
{ 
  ps_iface_type                 *iface_v4_ptr = NULL;
  /* Interface ptr for the call */
  ds_3gpp_iface_s               *ds3gpp_v4_iface_p= NULL;
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type                 *iface_v6_ptr = NULL;
  /* Interface ptr for the call */
  ds_3gpp_iface_s               *ds3gpp_v6_iface_p= NULL;
#endif
  /* PDN  cntx ptr for the call */
  ds_pdn_context_s              *pdn_cntx_p= NULL;
  ds_umts_pdp_type_enum_type     pdn_pdp_type;
  ds_umts_pdp_auth_enum_type     requested_auth_type;
#ifdef FEATURE_LTE
  ds_eps_bearer_context_s       *eps_bearer_context_p = NULL;
#endif
  network_params_info_type      *ppp_info_p         = NULL;
  boolean                        valid_pcscf_address = FALSE;
#ifdef FEATURE_DATA_LTE
  ds_cmd_type                   *cmd_ptr = NULL;
  ps_iface_type                 *ps_v4_iface_p = NULL;
  ds_eps_tlb_per_subs_info_type *eps_tlb_hndl = NULL;
  ds_pdn_context_s              **data_ptr = NULL;
  boolean                        remove_unused_pdn = FALSE;
#endif /* FEATURE_DATA_LTE */
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  ds_umts_pdp_type_enum_type    pdp_type;
  boolean                        dns_addr_valid = FALSE;
  boolean                        pcscf_or_dns_addr_valid = FALSE;
  boolean                        v4_or_v6_pcscf_present = FALSE; 
  boolean                        v4_or_v6_dns_present = FALSE; 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdn_cntx_p = (ds_pdn_context_s*)object;

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }

  if(!(pdn_cntx_p->ds_pdn_context_dyn_p->info_null))
  {
    ppp_info_p = &(pdn_cntx_p->ds_pdn_context_dyn_p->info);
  }

  if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    if(ppp_info_p &&
       ((ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)     ||
        (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK) ||
        (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) ))
    {
      requested_auth_type = (ds_umts_pdp_auth_enum_type) 
                            pdn_cntx_p->ds_pdn_context_dyn_p->
                            info.auth_info.auth_type;
      DS_3GPP_MSG1_HIGH("Read auth_type %d from info", requested_auth_type);
    }
    else
    {
      requested_auth_type
        = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.auth.auth_type;

      DS_3GPP_MSG1_HIGH("Read auth_type %d from profile", requested_auth_type);
    }     /* Indicate to the auth manager that the bearer activation succeeded*/ 
    ds_3gpp_auth_mgr_call_success(pdn_cntx_p->ds_pdn_context_dyn_p->
                                  pdp_profile.context.apn, 
                                  requested_auth_type, subs_id);
  }

#ifdef FEATURE_DATA_LTE
  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);
#endif /* FEATURE_DATA_LTE */
  pdp_type = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;

  /* Register with Ipa using dpm */
  if((pdp_type != DS_UMTS_PDP_PPP)&&
     (!bearer_cntx_p->ds_bearer_context_dyn_p->is_registered_with_dpm))
  {
#ifdef FEATURE_DATA_LTE
    if((ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl)) &&
         (eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_A))
    {
      dsEpsTlb_set_tlb_mode_A_params(bearer_cntx_p->
                                          ds_bearer_context_dyn_p->index);
      ds_bearer_cntx_register_dpm_tlb_mode(bearer_cntx_p);
    }
    else
    {
      ds_bearer_cntx_register_dpm(bearer_cntx_p);
    }
#else
    ds_bearer_cntx_register_dpm(bearer_cntx_p);
#endif /* FEATURE_DATA_LTE */
  }

  /*-------------------------------------------------------------------------
    If this is a dedicated bearer no need to bring up the iface.
    
    Also process the bearer up indication only if PDN context is not in UP 
    state already.
    This check is needed to filter out any duplicate bearer up indication that
    may be sent during abnormal scenarios.
  -------------------------------------------------------------------------*/
  if((bearer_cntx_p->ds_bearer_context_dyn_p->is_default != TRUE) ||
     (pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP) ||
     (pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN))
  {
    DS_3GPP_MSG1_HIGH("Received dedicated or duplicate bearer up ind. bearer_cnt:%d",
       pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context);
    return;
  }

  /*--------------------------------------------------------------------------- 
    Get the Iface. This can be obtained by looking at the PDP type in profile
  ---------------------------------------------------------------------------*/
  if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type != DS_UMTS_PDP_PPP) 
  {    
  pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_cntx_p);
  if(pdn_pdp_type == DS_UMTS_PDP_IPV4)
  {
    ds3gpp_v4_iface_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
  {
    ds3gpp_v6_iface_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  }
  else if(pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    ds3gpp_v4_iface_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    ds3gpp_v6_iface_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  }
#endif /*FEATURE_DATA_PS_IPV6*/
  else
  {
    DATA_ERR_FATAL("Invalid pdp type");
  }

  if (ds3gpp_v4_iface_p != NULL)
  {
    iface_v4_ptr = &(ds3gpp_v4_iface_p->ps_iface);
  }
#ifdef FEATURE_DATA_PS_IPV6
  if (ds3gpp_v6_iface_p != NULL)
  {
    iface_v6_ptr = &(ds3gpp_v6_iface_p->ps_iface);
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*--------------------------------------------------------------------------
  Send IFACE UP indication for the primary. 
  Do not send IFACE UP in case if TLB Loopback mode A is active
  --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  eps_bearer_context_p = (ds_eps_bearer_context_s *)
                    (DS_BEARER_CTXT_VOBJ(bearer_cntx_p,SYS_SYS_MODE_LTE));
  if (!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p))
  {
    return;
  }
  /*---------------------------------------------------------------------------- 
    Register the phys link enable flow cb for TLB Mode A
  ----------------------------------------------------------------------------*/
  bearer_cntx_p->uplink_flow_enable_buf_ptr =
      ps_phys_link_alloc_event_cback_buf(dsEpsTlb_bearer_phys_link_enable_flow_cb,
                                         bearer_cntx_p);
  if (bearer_cntx_p->uplink_flow_enable_buf_ptr != NULL &&
      0 != ps_phys_link_event_cback_reg(&(bearer_cntx_p->phys_link),
      PHYS_LINK_FLOW_ENABLED_EV, bearer_cntx_p->uplink_flow_enable_buf_ptr))
  {
    DS_LTE_MSG0_ERROR(
        "Could not register for bearer - PHYS_LINK_FLOW_ENABLED_EV");
  }

  if ((eps_bearer_context_p->ds_eps_bearer_context_dyn_p->is_in_tlb == FALSE) ||
       ( ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl) &&
         eps_tlb_hndl->tlb_state.mode != LTE_TLB_MODE_A ))
#endif /* FEATURE_DATA_LTE */
  {
    if (iface_v4_ptr != NULL)
    {
      /*----------------------------------------------------------------------
        If the v4 address is configured through NAS, we can bring up the 
        iface
      --------------------------------------------------------------------*/
      if (ps_iface_get_v4_addr(iface_v4_ptr) != 0x00000000)
      {
        dsPdnContext_bringUpIPv4PSIface(ds3gpp_v4_iface_p); 
#ifdef FEATURE_DATA_IMS
        DS_3GPP_MSG0_HIGH("Now calling DHCP to request PCSCF addr for v4 iface");
        if(ds_pdn_cntxt_dhcpRequestExtConfig(ds3gpp_v4_iface_p,
                                                 pdn_cntx_p)!= TRUE)
        {
          DS_3GPP_MSG0_ERROR("IPV4 DHCP REQUEST returned failure");
        }
#endif  
      }    
      else if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.ipv4_addr_alloc 
               == DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4)
      {
        /*----------------------------------------------------------------------
          If DHCP is enabled, we should decide whether DHCP client needs to be 
          started immediately or we can delay it 
        --------------------------------------------------------------------*/
        if ((pdn_pdp_type == DS_UMTS_PDP_IPV4) ||
            ((pdn_pdp_type == DS_UMTS_PDP_IPV4V6) && PS_IFACEI_IS_IN_USE(iface_v4_ptr)))
        {
          // For a V4 only call, or v4v6 call where the iface is in use, we should kick start dhcp
          // client right away
          ps_iface_configuring_ind(iface_v4_ptr);
          if (ds_pdn_cntxt_dhcpStartV4AddrAlloc(ds3gpp_v4_iface_p, pdn_cntx_p) != TRUE)
          {
            DS_3GPP_MSG1_ERROR("Could not start DHCP Client for iface: %x!", iface_v4_ptr);
            // need to figure out error handling. Whether to assert or tear down pdn context.
          }
        }
        else if (pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
        {
          /* If we reach here, which means this pdn context is of type v4v6 and v4 iface is 
             not in use, we can delay DHCP until the iface is in use.
          */
          ps_iface_configuring_ind(iface_v4_ptr);
          DS_3GPP_MSG0_HIGH("Delaying DHCP address allocation since iface not in use.");
        }
        else
        {
          DATA_ERR_FATAL("Invalid pdp type for v4 iface bringup");
        }

      }
      else
      {
        /*---------------------------------------------------------------------- 
          Should not reach here as if DHCP is not enalbed in the profile, bearer 
          activation handler should reject this bearer activation 
        ----------------------------------------------------------------------*/
        DS_3GPP_MSG1_ERROR("DHCP is not enabled for PDN context: %x with NULL v4 address!",
                  pdn_cntx_p);
        ASSERT(0);
      }
    }    
#ifdef FEATURE_DATA_PS_IPV6
    if (iface_v6_ptr != NULL) 
    {
      dsPdnContext_bringUpIPv6PSIface(ds3gpp_v6_iface_p);
      #ifdef FEATURE_DATA_IMS
        if(ds_pdn_cntxt_dhcpRequestExtConfig(ds3gpp_v6_iface_p, pdn_cntx_p)!= TRUE)
        {
          DS_3GPP_MSG0_ERROR("IPV6 DHCP REQUEST returned failure");
        }
      #endif /* FEATURE_DATA_IMS */
    }
#endif /* FEATURE_DATA_PS_IPV6 */
  }
  }
  else
  {  
    DS_3GPP_MSG0_HIGH("Bearer UP Ind for UMTS PDP PPP call");
  }
    
  //Validate PCSCF address.
  valid_pcscf_address = ds_3gpp_pdn_context_validate_PCSCF_address(pdn_cntx_p,
                                                     &v4_or_v6_pcscf_present);
  //Validate DNS address.  
  dns_addr_valid = ds_3gpp_pdn_cntxt_validate_dns_addr(pdn_cntx_p,
                                                       &v4_or_v6_dns_present);
  //Validate PCSCF or DNS address.
  pcscf_or_dns_addr_valid = ds_3gpp_pdn_cntxt_validate_pcscf_dns_addr(
                                                        pdn_cntx_p,
                                                        v4_or_v6_dns_present,
                                                        v4_or_v6_pcscf_present
                                                        );
    
  /*----------------------------------------------------------------------------
    Move the state of the PDN context to UP
  ----------------------------------------------------------------------------*/    
  if ((pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_COMING_UP)||
      (pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC)) 
  {
    pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_UP;
    DS_3GPP_MSG1_HIGH("bearerUpIndCb:PDN cntx 0x%x is in COMING_UP moving it to"
                      " UP state",pdn_cntx_p);

    #ifdef FEATURE_EMERGENCY_PDN
    ds_3gpp_pdn_cntxt_handle_emergency_attach_status(pdn_cntx_p);
    #endif /* FEATURE_EMERGENCY_PDN*/
    
    if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.inactivity_timer_val > 0)
    {
      DS_3GPP_MSG1_HIGH("Starting PDN Inactivity timer: %d seconds",
                        pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.inactivity_timer_val);

      (void)rex_set_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer), 
                          (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.inactivity_timer_val)*1000);

      /*------------------------------------------------------------------------
        Start the polling timer so that we can query the lower layers about
        data activity when this expires.
      ------------------------------------------------------------------------*/
      (void)rex_set_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->polling_timer), 
                          DS_3GPP_POLLING_INTERVAL);
    } /* pdn_inactivity_timer > 0 */

  }
  else
  { 
    DS_3GPP_MSG3_HIGH("bearerUpIndCb: PDN cntx 0x%x is in %d state",
                       pdn_cntx_p,pdn_cntx_p->ds_pdn_context_dyn_p->state,0);
  }
  
#ifdef FEATURE_EMERGENCY_PDN
  ds_3gpp_pdn_cntxt_check_and_teardown_non_emergency_pdns();
#endif /*FEATURE_EMERGENCY_PDN*/
  /*
    Enter the below code only if NV 70334 is set which needs to removes unused
    pdn with no client associated with them. PCSCF address is checked so that
    the up-coming PDN is not torn down 
  */
  #ifdef FEATURE_DATA_LTE
   ps_v4_iface_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
   remove_unused_pdn = ds_3gpp_nv_manager_get_remove_unused_pdn(subs_id);
   if ( remove_unused_pdn  &&
       (PS_IFACE_IS_IN_USE(ps_v4_iface_p )) && 
       (valid_pcscf_address == TRUE) && 
        (dns_addr_valid == TRUE) &&
        (pcscf_or_dns_addr_valid == TRUE))
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return;
    }
   
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_REMOVE_UNUSED;
    data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
    *data_ptr = pdn_cntx_p;

    ds_put_cmd_ext(cmd_ptr);
  }
  #endif /* FEATURE_DATA_LTE*/

  /*-------------------------------------------------------------------------
   Need to post it always because if a new data call is originated, flowmgr 
   state would be changed to ORIG_STATE. Also, unless PDN state moves to UP
   CALL_ACTIVE_EV wouldn't be posted.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG0_MED("PS_DATA_CALL_ACTIVE_IND");
  ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                           DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
  return;
} /* dsPdnContext_bearerUpIndCb */

/*===========================================================================
FUNCTION ds_pdn_context_cleanup_after_iface_down_ind

DESCRIPTION
  This function does the clean up of apn table and pdn context after 
  iface is down.  

PARAMETERS
  pdn_cntx_p - Pointer to pdn context

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
static void ds_pdn_context_cleanup_after_iface_down_ind
( 
  ds_pdn_context_s        *pdn_cntx_p
)
{
  ds_3gpp_iface_s         *ds_3gpp_v4_iface_p;
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s         *ds_3gpp_v6_iface_p;
#endif
  boolean                 can_cleanup = TRUE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  uint8                   index = 0;
#endif /*FEATURE_DATA_LTE*/

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }

  ds_3gpp_v4_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->
    ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_v6_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->
    ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
#endif 

  if(ds_3gpp_v4_iface_p != NULL)
  {
    if(ps_iface_state(&ds_3gpp_v4_iface_p->ps_iface) != IFACE_DOWN)
    {
      can_cleanup = FALSE;
      DS_3GPP_MSG2_HIGH("V4 Iface 0x:%x not in down state. State: %d",
                                   &(ds_3gpp_v4_iface_p->ps_iface),
                                   ps_iface_state(&ds_3gpp_v4_iface_p->ps_iface));
    }
  }
#ifdef FEATURE_DATA_PS_IPV6
  if(ds_3gpp_v6_iface_p != NULL)
  {
    if(ps_iface_state(&ds_3gpp_v6_iface_p->ps_iface) != IFACE_DOWN)
    {
      can_cleanup = FALSE;
      DS_3GPP_MSG2_HIGH("V6 Iface 0x:%x not in down state, State: %d",
                                     &(ds_3gpp_v6_iface_p->ps_iface),
                                      ps_iface_state(&ds_3gpp_v6_iface_p->ps_iface));
    }
  }
#endif
  if( can_cleanup == TRUE)
  {
    DS_3GPP_MSG3_HIGH("Cleaning up APN entry and pdn context for "
                      "pdn_cntx: 0x%x, v4_iface: 0x%x, v6_iface: 0x%x",
                       pdn_cntx_p,
                      &ds_3gpp_v4_iface_p->ps_iface,
                      &ds_3gpp_v6_iface_p->ps_iface);
    /*-------------------------------------------------------------------------- 
    remove entry for this APN. This has to be done irrespective of whether the
    call was connected or not.
    --------------------------------------------------------------------------*/
    if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
    {
      DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
    }
#ifdef FEATURE_DATA_LTE
    /*----------------------------------------------------------------------- 
      Find an entry in the PDN Limit Table.If an entry is found,
      validate and clear the entry at this point
      -----------------------------------------------------------------------*/
    if((index = ds_3gpp_pdn_limit_tbl_find_entry
        ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
           subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
    {
      ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
    }
#endif /*FEATURE_DATA_LTE*/

    ds_pdn_cntxt_freeInstance(pdn_cntx_p);

#ifdef FEATURE_DATA_LTE
   ds_bearer_cntxt_deactivate_last_bearer_if_needed(subs_id);
#endif
    /*---------------------------------------------------------------- 
      ds_pdn_cntxt_freeInstance() returns the bearer resources back
      to the bearer resource pool. bring up if there is any PDN in
      Bearer Alloc state
      ----------------------------------------------------------------*/
    if (ds_3gpp_pdn_cntxt_is_any_pdn_awaiting_bearer_allocation(subs_id))
    {
      ds_3gpp_pdn_context_retry_iface_bring_up(subs_id);
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("Did not clean up APN entry PDN context");
  }
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_handle_v6_iface_down

DESCRIPTION
  This function does additional handling when v6 iface goes down  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
static void ds_3gpp_pdn_cntxt_handle_v6_iface_down
(
  ps_iface_type   *this_iface_ptr,
  ps_iface_net_down_reason_type down_reason
)
{
  ds_pdn_context_s *pdn_cntx_p = NULL;
  ds_3gpp_iface_s * ds3gpp_iface_p= NULL;
  ds_3gpp_iface_s         *ds3gpp_iface_v4_p;
  ps_iface_down_mh_event_info_type   down_info;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
  /*------------------------------------------------------------------------*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
  down_info.down_reason = down_reason;

  if(this_iface_ptr == NULL)
  {
    DATA_ERR_FATAL("Bad Args");
    return;
  }

subs_id = 
    (sys_modem_as_id_e_type)
      ds3gsubsmgr_subs_id_ds_to_cm(PS_IFACE_GET_SUBS_ID(this_iface_ptr));

  DS_3GPP_MSG2_HIGH("Entering ds_3gpp_pdn_cntxt_handle_v6_iface_down for iface [0x%x:%x]",
                    this_iface_ptr->name, this_iface_ptr->instance);

  ds3gpp_iface_p=(ds_3gpp_iface_s *)this_iface_ptr->client_data_ptr; 
  if(ds3gpp_iface_p == NULL)
  {
    ASSERT(0);
    return;
  }

  //If this is a UE initiated teardown; use the stored reason code.
  if (ds3gpp_iface_p->ue_init_net_down_reason != PS_NET_DOWN_REASON_NOT_SPECIFIED)
  {    
    down_reason = ds3gpp_iface_p->ue_init_net_down_reason;
    DS_3GPP_MSG1_HIGH("Using netdown reason %d", down_reason);
  }

  /*------------------------------------------------------------
   Get the PDN context pointer from the iface pointer 
   -------------------------------------------------------------*/
  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    DS_3GPP_MSG3_HIGH("Null PDN cntxt: skip PDN cleanup ds_iface 0x%x ps_iface 0x%x:%x",
                                                ds3gpp_iface_p,
                                                this_iface_ptr->name,
                                                this_iface_ptr->instance);    

    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.rat_value = 
      ds_3gpp_map_sys_mode_to_ds_sys_rat 
        (ds3gpp_get_current_network_mode(subs_id));
    down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);

   /*------------------------------------------------------------
    Post IFACE_DOWN event
   -------------------------------------------------------------*/
    ps_iface_down_ind(this_iface_ptr, &down_info);

    return;
  }
  else
  {
    down_info.bearer_tech = pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech;

    if (down_info.bearer_tech.rat_value == DS_SYS_RAT_EX_NULL_BEARER)
    {
      down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
      down_info.bearer_tech.rat_value = 
        ds_3gpp_map_sys_mode_to_ds_sys_rat
          (ds3gpp_get_current_network_mode(subs_id));
      down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
    }
        
   /*------------------------------------------------------------
    Post IFACE_DOWN event
   -------------------------------------------------------------*/
    ps_iface_down_ind(this_iface_ptr, &down_info);
  }

  /*-------------------------------------------------------------
  Get the v4 iface. It is possible for this to be NULL
  --------------------------------------------------------------*/
  ds3gpp_iface_v4_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

 /*----------------------------------------------------------------
   Check for Dual IP case. The v4 iface is set to down only after
   v6 iface is down. 
  -----------------------------------------------------------------*/
  if(ds3gpp_iface_v4_p != NULL)
  {
    if(pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress)
    {
      /*---------------------------------------------------------------------------
      If it is already down, no need to post down twice. 
      ---------------------------------------------------------------------------*/
      if( ps_iface_state(&ds3gpp_iface_v4_p->ps_iface) != IFACE_DOWN )
      {
        DS_3GPP_MSG2_HIGH("MH posting iface down ind on 0x%x, reason: 0x%x",
                      &(ds3gpp_iface_v4_p->ps_iface),down_reason);
        ps_iface_down_ind(&ds3gpp_iface_v4_p->ps_iface, &down_info);
      }
      else
      {
        DS_3GPP_MSG2_HIGH("Did not post iface down ind on 0x%x, state: %d",
                      &(ds3gpp_iface_v4_p->ps_iface),
                       ps_iface_state(&ds3gpp_iface_v4_p->ps_iface));
      }
    }
    else
    {
       DS_3GPP_MSG2_ERROR("PDN context state %d prevents iface_down_ind on 0x%x",
                                         pdn_cntx_p->ds_pdn_context_dyn_p->state,
                                                &(ds3gpp_iface_v4_p->ps_iface));
    }
  }
 else
  {
     DS_3GPP_MSG0_MED("No need to bring down V4 iface");
  }

  //Reset the end call reason.
  dssnet6_reset_call_end_reason(&(ds3gpp_iface_p->dssnet6_sm_cb));

  /*--------------------------------------------------------------------
  Perform clean up of apn table and pdn context. We need to 
  check for pdn_cntx state here because:
  - it is possible that for a Single IP v6 call RA failure happens and 
  control reaches here
  - In such a case we should not tear down pdn
  - If we did so, then eventually when we get a bearer_down_ind,
  we will not have a valid pdn to work with.
  -------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress) 
  {
    ds_pdn_context_cleanup_after_iface_down_ind(pdn_cntx_p);
  }
}
#endif // FEATURE_DATA_PS_IPV6

/*===========================================================================
FUNCTION ds_pdn_cntxt_tearDownPSIface

DESCRIPTION
  This function tears down the PS iface.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void ds_pdn_cntxt_tearDownPSIface
( 
  ps_iface_type *iface_ptr,                   /* Interface ptr for the call*/
  ps_iface_net_down_reason_type down_reason
)
{   
  ds_3gpp_iface_s   *ds3gpp_iface_p;
  ds_pdn_context_s  *pdn_cntx_p;
  ds_bearer_context_s     *bearer_context_p;
  ps_iface_type     *rm_iface_ptr;
#ifdef FEATURE_DATA_PS_IPV6
  ds_v6_call_status_type   v6_call_status;
#endif /*FEATURE_DATA_PS_IPV6*/
  ds_3gpp_iface_s        *ds3gpp_v6_iface_p;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;
  ps_iface_down_mh_event_info_type   down_info;
/*-------------------------------------------------------------------------*/

  ASSERT (iface_ptr != NULL);

#ifdef FEATURE_DATA_PS_IPV6
  memset(&v6_call_status, 0, sizeof(ds_v6_call_status_type));
#endif /*FEATURE_DATA_PS_IPV6*/
  /* Get the DS Iface pointer from PS Iface */
  ds3gpp_iface_p = (ds_3gpp_iface_s*)iface_ptr->client_data_ptr;

  /* Get teh PDN context from the DS Iface */
  pdn_cntx_p = (ds_pdn_context_s*)(((ds_3gpp_iface_s*)\
                           iface_ptr->client_data_ptr)->client_data_ptr);

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return;
  }

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
  down_info.down_reason = down_reason;

  down_info.bearer_tech = pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech;

  if (down_info.bearer_tech.rat_value == DS_SYS_RAT_EX_NULL_BEARER)
  {
    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.rat_value =
      ds_3gpp_map_sys_mode_to_ds_sys_rat
        (ds3gpp_get_current_network_mode(subs_id));
    down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
  }
  
  /*-------------------------------------------------------------------------
    Get the Primary bearer context
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->
                                ds_pdn_context_dyn_p->def_bearer_context_ptr);


  /*--------------------------------------------------------------------------- 
    See if there are any pending flows bound to this iface. If so
    we need to clean them up now.
    ---------------------------------------------------------------------------*/
  {
    ds_flow_manager_s *flow_mgr_p = ds_flow_manager_find_flow_manager(pdn_cntx_p);
    ds_flow_context_s        *bflow_context_p  = NULL;
    ds_bearer_flow_manager_s *bflow_mgr_p = ds_bearer_flow_manager_find_flow_manager(bearer_context_p);
    if(ds_flow_manager_validate_flow_manager(flow_mgr_p))
    {
      ds_flow_context_s *flow_context_p = list_peek_front(&(flow_mgr_p->
                                         ds_flow_manager_dyn_p->pending_flow_cntx));
            
      while(ds_flow_context_validate_flow(flow_context_p))
      {      
        ds_flow_context_s *next_flow_cntxt = list_peek_next(&(flow_mgr_p->
        ds_flow_manager_dyn_p->pending_flow_cntx),
                                      &(flow_context_p->link));
        if (flow_context_p->ds_flow_context_dyn_p->ds_iface_ptr == ds3gpp_iface_p)
        {
          list_pop_item(&(flow_mgr_p->ds_flow_manager_dyn_p->pending_flow_cntx), 
                        &(flow_context_p->link));
          ds_flow_context_release_flow_context(flow_context_p);                          
        }
        flow_context_p = next_flow_cntxt;
                
      } /* while*/

    }
  /*---------------------------------------------------------------------------------------
  If there are any pending flow bound to the default bearer and TFT's are installed on 
  the default bearer and if flows are network initiated we clear all the flow if 
  Iface Down is triggered
  -----------------------------------------------------------------------------------------*/
     if(ds_bearer_flow_manager_validate_flow_manager(bflow_mgr_p))
     {
        bflow_context_p = list_peek_front(&(bflow_mgr_p->ds_bearer_flow_manager_dyn_p
                                     ->flow_context));
	while(ds_flow_context_validate_flow(bflow_context_p))
	{
	  ds_flow_context_s *next_bflow_cntxt = list_peek_next(&(bflow_mgr_p->
                                  ds_bearer_flow_manager_dyn_p->flow_context),
                                      &(bflow_context_p->link));
	  if ((bflow_context_p->ds_flow_context_dyn_p->ds_iface_ptr == ds3gpp_iface_p) 
			         && (bflow_context_p->ds_flow_context_dyn_p->flow_type & (DS_3GPP_NW_FLOW_TYPE)))
          {
	    list_pop_item(&(bflow_mgr_p->ds_bearer_flow_manager_dyn_p->flow_context), 
                          &(bflow_context_p->link));
	    ds_flow_context_release_flow_context(bflow_context_p);
	  }
	   bflow_context_p = next_bflow_cntxt;
	}

      }

  }

  /*-------------------------------------------------------------------------
    Unbind the default flow
  -------------------------------------------------------------------------*/
  ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr), 
                    PS_EIC_NOT_SPECIFIED);

  /*-------------------------------------------------------------------------
  Call ppp_abort() to do final cleanup on the device.When multiple PDP
  contexts are supported, the appropriate device type will have to be
  saved in the call table and used as parameter to ppp_abort().
  -------------------------------------------------------------------------*/

  if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type == 
      DS_UMTS_PDP_PPP )
  {
    (void)ppp_abort(PPP_UM_SN_DEV);
  }

  rm_iface_ptr = ds_3gpp_pdn_context_get_bridged_iface(
             (ds_3gpp_iface_s*)iface_ptr->client_data_ptr);

  ps_iface_set_bridge(iface_ptr, NULL);
  
  if(ps_iface_addr_family_is_v4(iface_ptr))
  {
    DS_3GPP_MSG0_HIGH("Flush Route: Last phys link of the interface");
    (void)route_flush(iface_ptr);            

    // if DHCP is used, stop the DHCP Client
    if (ds3gpp_iface_p->dhcp_handle != NULL)
    {
      ds_pdn_cntxt_dhcpStopClient(ds3gpp_iface_p);
    }

    ds3gpp_v6_iface_p = 
        pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
      [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

    /*------------------------------------------------------------------------
    For Single IP v4 case, it is safe to post down ind here
    ----------------------------------------------------------------------*/
    if(ds3gpp_v6_iface_p == NULL)
    {
      DS_3GPP_MSG2_HIGH("MH posting iface down ind on 0x%x, reason: 0x%x",
                      iface_ptr,down_reason);

      if(ps_iface_state(iface_ptr) != IFACE_DOWN)
      {
        ps_iface_down_ind(iface_ptr, &down_info);
      }
    }
#ifdef FEATURE_DATA_PS_IPV6
    else
    {
     /*----------------------------------------------------------------------
       For the dual ip case, we need to make sure that v6 is down before 
       posting down on v4. Posting down on v4 iface may happen 
       here only if v6 goes down for RA failure or other reasons. 
       Under normal scenario v4 down is triggered by the callback from 
       dsgen_iface6_hdlr.
       It is okay to post v4 down here for other cases where iface_tear_down
       is not triggered by bearer_down_ind_cb
     ----------------------------------------------------------------------*/
      if(ps_iface_state(&ds3gpp_v6_iface_p->ps_iface) == IFACE_DOWN ||
            pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress == FALSE)
      {
        DS_3GPP_MSG3_HIGH("Dual IP case: MH posting iface down ind on 0x%x, reason: 0x%x, "
                          "PDN tear_down_in_progress: %d",
        iface_ptr,down_reason,pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress);

        if(ps_iface_state(iface_ptr) != IFACE_DOWN)
  {
          ps_iface_down_ind(iface_ptr, &down_info);
  }
      }
    }
#endif
  }
#ifdef FEATURE_DATA_PS_IPV6
    
  else if(ps_iface_addr_family_is_v6(iface_ptr))
  {
    if(ps_iface_state(iface_ptr) != IFACE_DOWN)
    {
      /* populate the v6 status info struct*/
      v6_call_status.call_type = DS_CALL_CATEGORY_NON_PPP_TYPE;
      v6_call_status.call_end_reason_info.call_status = (int32) down_reason;
      /*--------------------------------------------------------------------    
        Send a Link down event to DSSNET6 SM. Link Down event transitions 
        SM to closed state. In closed state SM sends iface down indication.
      --------------------------------------------------------------------*/    
      DS_3GPP_MSG1_HIGH("Posting DSSNET6_LINK_DOWN_EV on 0x%x",
                        iface_ptr);

      if (dssnet6_sm_post_event(&(ds3gpp_iface_p->dssnet6_sm_cb), 
                                DSSNET6_LINK_DOWN_EV,
                                (void*) (&v6_call_status)) != 0)
      {
          DS_3GPP_MSG1_ERROR("Error posting event to dssnet6", iface_ptr);
      }
    }
  }
  else
   {
    DS_3GPP_MSG1_ERROR("Iface has invalid IP type",iface_ptr);
   }
#endif  
}

/*===========================================================================
FUNCTION ds_pdn_cntxt_tearDownDSIface

DESCRIPTION
  This function tears down the DS iface. This function also tears down the
  corresponding PS iface and disassociates the DS iface from the PDN cntxt.

PARAMETERS
  ds_ifaced_p - pointer to the DS iface to be torn down
  down_reason - reason for tearing down the iface - will be passed to the
                PS iface

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS 
Disassociation of DS Iface and PDN context. 
===========================================================================*/

void ds_3gpp_pdn_cntxt_tearDownDSIface
(
  ds_3gpp_iface_s *ds_iface_p,
  ps_iface_net_down_reason_type down_reason
)
{
  ds_pdn_context_s               *pdn_cntx_p = NULL;
  ds_bearer_context_s            *bearer_context_p = NULL;
  int                             index = 0;

  if(ds_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS iface ptr NULL, cannot teardown");
    return;
  }

  pdn_cntx_p = ds_iface_p->client_data_ptr;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get the Primary bearer context
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->
                                ds_pdn_context_dyn_p->def_bearer_context_ptr);

  if(bearer_context_p->ds_bearer_context_dyn_p->is_registered_with_dpm)
  {
    ds_bearer_cntx_deregister_dpm_iface (pdn_cntx_p, &(ds_iface_p->ps_iface));
  }
  /*--------------------------------------------------------------------------- 
    Acquire lock on global_ps_crit_section before acquiring 3gpp crit section 
    to avoid deadlock when ps_global_crit_section is acquired inside 
    ds_pdn_cntxt_tearDownPSIface. 
  ---------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ds3gpp_enter_global_crit_section();

  ds_pdn_cntxt_tearDownPSIface(&(ds_iface_p->ps_iface),
                                 down_reason);

  /*--------------------------------------------------------------------------
      Mark the DS Iface as out of use.
  --------------------------------------------------------------------------*/
  ds_iface_p->in_use = FALSE;

  /*--------------------------------------------------------------------------
      Set the post_proc_done_flag to FALSE as we are returning this iface to the
      free pool
  --------------------------------------------------------------------------*/
  ds_iface_p->post_proc_done_flag = FALSE;

  /*------------------------------------------------------------------------
    Clear the entry for the DS Iface in PDN context
  ------------------------------------------------------------------------*/

  for(index = 0; index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; index++)
    if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[index] == ds_iface_p)
    {
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[index] = NULL;
      break;
    }

  /*--------------------------------------------------------------------------
    Remove the binding that DS Iface has towards PDN context
  --------------------------------------------------------------------------*/
  ds_iface_p->client_data_ptr = NULL;
  
  ds3gpp_leave_global_crit_section();
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
}

/*===========================================================================
FUNCTION dsPdnContext_bearerDownIndCb

DESCRIPTION
  This function handles bearer teardown.
  This could be of 2 types:
  1. If this is the last bearer in a PDN context(default bearer); all the ifaces
     attached to that PDN are brought down and the PDN is cleaned up.
  2. If this is a dedicated bearer; the flow data structures allocated for that
     bearer are cleaned up. 

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
===========================================================================*/
void dsPdnContext_bearerDownIndCb
( 
  void                          *object,
  ds_bearer_context_s           *bearer_cntx_p,
  ds_3gpp_down_reason_t          net_down_reason,
  boolean                        call_was_connected
)
{   
  /*  v4 Interface ptr for the call */
  ds_3gpp_iface_s               *ds3gpp_iface_v4_p=NULL ;
  /*  v6 Interface ptr for the call */
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s               *ds3gpp_iface_v6_p=NULL ;
#endif
  /* PDN  cntx ptr for the call */
  ds_pdn_context_s              *pdn_cntx_p=NULL;
  ds_umts_pdp_type_enum_type     pdn_pdp_type;
  ps_iface_net_down_reason_type  down_reason;
  ds_umts_pdp_auth_enum_type     requested_auth_type;
  network_params_info_type*      ppp_info_p = NULL;
  ds_bearer_context_s*           def_bearer_cntxt_p = NULL;
  boolean                        is_deafult_bearer_down = FALSE;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                        any_other_bearer_active = FALSE;
  /*-------------------------------------------------------------------------*/

  pdn_cntx_p = (ds_pdn_context_s*)object;  
  down_reason = DS_3GPP_PS_IFACE_NET_DOWN_REASON(net_down_reason);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return;
  }

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return;
  }

  if(!(pdn_cntx_p->ds_pdn_context_dyn_p->info_null))
  {
    ppp_info_p = &(pdn_cntx_p->ds_pdn_context_dyn_p->info);
  }
  
  DS_3GPP_MSG3_HIGH("DBG:Bearer default/ded: %d, call_mode: %d, subs_id: %d", 
                     bearer_cntx_p->ds_bearer_context_dyn_p->is_default,
                     bearer_cntx_p->ds_bearer_context_dyn_p->call_mode,
                     subs_id);
  /*---------------------------------------------------------------------------- 
    Free phys link flow enable buffer
  ----------------------------------------------------------------------------*/
  #ifdef FEATURE_DATA_LTE
  if (bearer_cntx_p->uplink_flow_enable_buf_ptr != NULL)
  {
    ps_phys_link_event_cback_dereg(&(bearer_cntx_p->phys_link),
                                   PHYS_LINK_FLOW_ENABLED_EV, 
                                   bearer_cntx_p->uplink_flow_enable_buf_ptr);
    ps_phys_link_free_event_cback_buf(
        bearer_cntx_p->uplink_flow_enable_buf_ptr);
    bearer_cntx_p->uplink_flow_enable_buf_ptr = NULL;
  }
  #endif /* FEATURE_DATA_LTE */
  
  /*------------------------------------------------------------------------- 
    Update the Network down reason in the PDN context table
    -------------------------------------------------------------------------*/
  pdn_cntx_p->ds_pdn_context_dyn_p->net_down_reason = down_reason;

  switch(bearer_cntx_p->ds_bearer_context_dyn_p->state)
  {
  case DS_BEARER_CONTEXT_STATE_PEND_REDIAL:
    if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default) 
    {
      if(ppp_info_p && ((ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
         (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
         (ppp_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK) ))
      {
        requested_auth_type = (ds_umts_pdp_auth_enum_type) 
        pdn_cntx_p->ds_pdn_context_dyn_p->info.auth_info.auth_type;
        DS_3GPP_MSG1_HIGH("Read auth_type %d from info", requested_auth_type);
      }
      else
      {
        requested_auth_type = pdn_cntx_p->ds_pdn_context_dyn_p->
                              pdp_profile.auth.auth_type;
        DS_3GPP_MSG1_HIGH("Read auth_type %d from profile", requested_auth_type);

      }    
      /* Indicate to the auth manager that the bearer activation failed*/
      ds_3gpp_auth_mgr_call_failure
        (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
         requested_auth_type, down_reason, call_was_connected, subs_id);
    }
    DS_3GPP_MSG1_HIGH("PDN cntx:0x%x Calling redial handler",
              pdn_cntx_p);
    
   /*Move the PDN state to PENDING REDIAL only if it was in COMING UP state*/
   
    if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_COMING_UP)
    {
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_PEND_REDIAL;
    }
	
    ds_3gpp_pdn_cntxt_redial_hdlr(pdn_cntx_p, bearer_cntx_p,
                                       net_down_reason, call_was_connected);

    return;
       
  case DS_BEARER_CONTEXT_STATE_DOWN:
    any_other_bearer_active = 
      ds_bearer_cntx_is_any_other_bearer_per_sub_active(bearer_cntx_p);
   
    /*--------------------------------------------------------------------------- 
      Get the Iface. This can be obtained by looking at the PDP type in the
      profile if the call was not connected.
      If the call had been connected, then the APN table is the place to get this
      info from because there is a possibility that the n/w did not give what was
      asked. The APN table is updated to reflect this info correctly only if the
      call was connected
    ---------------------------------------------------------------------------*/
    if(call_was_connected == TRUE)
    {
      pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_cntx_p);
    }
    else
    {
      pdn_pdp_type = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type;
    }
  
    DS_3GPP_MSG3_HIGH("Bearer down for PDN cntx:0x%x. pdp_type: %d." 
                      "Any other bearers active on same sub (Y/N): %d",
                       pdn_cntx_p, pdn_pdp_type, any_other_bearer_active);

    ds3gpp_iface_v4_p = 
        (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

  #ifdef FEATURE_DATA_PS_IPV6
    ds3gpp_iface_v6_p = 
        (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  #endif /* FEATURE_DATA_PS_IPV6 */
  
    /*----------------------------------------------------------------------------
      Decrement the number of active bearer contexts if the number of bearer cntx
      is greater than 0
    ----------------------------------------------------------------------------*/
    ASSERT(pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context > 0);
    pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context--;
    
    DS_3GPP_MSG1_HIGH("Number of active bearer_cntx %d",
              pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context);
  
    if(pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context == 0)
    {
      DS_3GPP_MSG1_HIGH("Freeing Last Phys Link pdn cntx 0x%x", pdn_cntx_p);  

      pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech.rat_value = 
        ds_3gpp_map_sys_mode_to_ds_sys_rat
          (bearer_cntx_p->ds_bearer_context_dyn_p->call_mode);
      pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech.so_mask = 
        DS_SYS_SO_EX_UNSPECIFIED;
       pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech.technology = 
         DS_SYS_NETWORK_3GPP;

      DS_3GPP_MSG1_HIGH("Set RAT Mask as %d", 
                        pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech.rat_value); 

      /*--------------------------------------------------------------------
        Set tear_down_in_progress to TRUE. This is useful state to
        synchronize down indications on v4 and v6 ifaces for dual IP
      ------------------------------------------------------------------------*/
      pdn_cntx_p->ds_pdn_context_dyn_p->tear_down_in_progress = TRUE;
      
      pdn_cntx_p->ds_pdn_context_dyn_p->ue_initiated_tear_down = FALSE;
      
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;

#ifdef FEATURE_EMERGENCY_PDN
      ds_3gpp_pdn_cntxt_check_and_teardown_non_emergency_pdns();
#endif /* FEATURE_EMERGENCY_PDN*/
      /*--------------------------------------------------------------------------
        Grab PS global crit section. This is to ensure that if we post DOWN on an
        iface, we ensure that it is not picked until we dissociate the ifaces from
        PDN context and free the PDN context.
        This is similar to the clean up of V6 iface in DSSNET6 SM
      --------------------------------------------------------------------------*/  
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      // if(ds3gpp_iface_p->state != DS_3GPP_IFACE_INVALID_STATE)
      if (ds3gpp_iface_v4_p != NULL)
      {
        //If this is a UE initiated teardown; use the stored reason code.
        if (ds3gpp_iface_v4_p->ue_init_net_down_reason != PS_NET_DOWN_REASON_NOT_SPECIFIED)
        {    
          ds_pdn_cntxt_tearDownPSIface(&(ds3gpp_iface_v4_p->ps_iface),
                                       ds3gpp_iface_v4_p->ue_init_net_down_reason);
          DS_3GPP_MSG1_HIGH("Using netdown reason %d", ds3gpp_iface_v4_p->
                            ue_init_net_down_reason);
        }
        else 
        {
        ds_pdn_cntxt_tearDownPSIface(&(ds3gpp_iface_v4_p->ps_iface),down_reason);
        }
        /* clean up 3gpp iface */
        ds3gpp_iface_v4_p->state = DS_3GPP_IFACE_INVALID_STATE;
      }

  #ifdef FEATURE_DATA_PS_IPV6
      if (ds3gpp_iface_v6_p != NULL)
      {
        //If this is a UE initiated teardown; use the stored reason code.
        if (ds3gpp_iface_v6_p->ue_init_net_down_reason != PS_NET_DOWN_REASON_NOT_SPECIFIED)
        {    
          ds_pdn_cntxt_tearDownPSIface(&(ds3gpp_iface_v6_p->ps_iface),
                                       ds3gpp_iface_v6_p->ue_init_net_down_reason);
          DS_3GPP_MSG1_HIGH("Using netdown reason %d", ds3gpp_iface_v6_p
                            ->ue_init_net_down_reason);
        }
        else 
        {
        ds_pdn_cntxt_tearDownPSIface(&(ds3gpp_iface_v6_p->ps_iface),down_reason);
        }
        ds3gpp_iface_v6_p->state = DS_3GPP_IFACE_INVALID_STATE;
      }
  #endif /* FEATURE_DATA_PS_IPV6 */
  
      DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*--------------------------------------------------------------------------
        Release Bearer Flow Manager associated with the bearer.
      --------------------------------------------------------------------------*/
      if(ds_flow_manager_remove_bearer_context_from_flow_manager(
                                         pdn_cntx_p, bearer_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Error removing beaer context from flow mgr");
      }
      ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  
      /*--------------------------------------------------------------------------
        Clear the PDN context timer. This needs to be done only if the timer was 
        started.
      --------------------------------------------------------------------------*/
      if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.inactivity_timer_val > 0)
      {
        DS_3GPP_MSG0_HIGH("Stopping PDN Inactivity and Polling timer");
  
        (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer));
        (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->polling_timer));
      } /* pdn_inactivity_timer > 0 */
  
      (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_timer));
      ds_pdn_context_cleanup_after_iface_down_ind(pdn_cntx_p);
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    }
    else
    {
      /* Dedicated Bearer */
      PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
      ds3gpp_enter_global_crit_section();
      /* @begin unsolicited packet filtering changes */
      
 /* This is to handle following scenario CR683210/695888
           1) 1 Default and 1 dedicated bearers up on W
           2)  NW bringd down Default bearer, Default down dedicated up
                 (in UMTS it is possible)
           3)  Context transfer to LTE
           4) NW ends dedicated bearer as well
           5) But call cleanup will not be complete and 
                 deafult bearer will be hanging */
      if ((pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context == 1) &&
          (bearer_cntx_p->ds_bearer_context_dyn_p->is_default == FALSE))
      {
        def_bearer_cntxt_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
        if(!ds_bearer_cntx_validate_bearer_context(def_bearer_cntxt_p))
        {
          return;
        }
        if (def_bearer_cntxt_p->ds_bearer_context_dyn_p->state == 
                                                   DS_BEARER_CONTEXT_STATE_DOWN)
        {
          DS_3GPP_MSG0_HIGH("Default bearer is down but Valid, do Cleanup");
          is_deafult_bearer_down = TRUE;
        }
      }
      ds_bearer_cntxt_free_bearer_inst(bearer_cntx_p);
      if(ds_pdn_cntxt_release_bearer_context(
                   pdn_cntx_p,(void *)bearer_cntx_p)!=DS3G_SUCCESS)
      {
        DS_3GPP_MSG0_HIGH("could not release the allocated bearer");
      }
  
      /*--------------------------------------------------------------------------
        Release Bearer Flow Manager associated with the bearer.
      --------------------------------------------------------------------------*/
      if(ds_flow_manager_remove_bearer_context_from_flow_manager(
                                         pdn_cntx_p, bearer_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("error when removing Bearer context from Flow mnger");
      }
      ds_bearer_flow_manager_release_flow_manager(bearer_cntx_p);
      ds_bearer_cntxt_return_bearer_to_free_pool(bearer_cntx_p);
      ds3gpp_leave_global_crit_section();
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

      if (is_deafult_bearer_down == TRUE )
      {
        ds_3gpp_bearer_cntxt_go_null(def_bearer_cntxt_p, 
                                          net_down_reason, FALSE);
      }
      /*---------------------------------------------------------------- 
        Bring up PDN context, if its in BEARER ALLOC STATE and
        waiting for Bearer Resource allocation.
       ----------------------------------------------------------------*/
      if (ds_3gpp_pdn_cntxt_is_any_pdn_awaiting_bearer_allocation(subs_id))
      {        
        ds_3gpp_pdn_context_retry_iface_bring_up(subs_id);
      }
    }/* else */

      /*---------------------------------------------------------------------
        If no other bearer is active, It means this is the last bearer to go
        down.
        Send call Call end event (i.e. all calls for this sub has ended) to
        DS3G flowmgr
       
        Also send PS_CALL_INACTIVE_IND to DS3G flowmgr.
        This check is added to avoid sending duplicate indications to
        DS3Gflowmgr
      ---------------------------------------------------------------------*/
      if(!any_other_bearer_active)
      {
        DS_3GPP_MSG0_MED("PS_DATA_CALL_INACTIVE_IND");
        ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                                 DS3GFLOWMGR_PS_CALL_END_EV);
      }
      else
      {
        DS_3GPP_MSG0_MED("At least one bearer active post PS_DATA_CALL_ACTIVE_IND");
        ds3gflowmgr_notify_ps_call_state_per_sub(subs_id,
                                                 DS3GFLOWMGR_PS_CALL_ACTIVE_EV);
      }
    break;

  default:
      DS_3GPP_MSG1_HIGH("Unhandled bearer state %d",
                        bearer_cntx_p->ds_bearer_context_dyn_p->state);
    break;
  }
  return;
}/* dsPdnContext_bearerDownIndCb */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_INACTIVITY_TIMER_CB

DESCRIPTION
  This function is called when the PDN inactivity timer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_inactivity_timer_cb(unsigned long callback_data)
{
  ds_cmd_type *cmd_ptr = NULL;
  void        **data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("PDN Inactivity timer call back");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_INACTIVITY_TMR_EXP;
  data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
  *data_ptr = (void*)callback_data;

  ds_put_cmd_ext(cmd_ptr);
} /* ds_3gpp_pdn_cntx_inactivity_timer_cb */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_POLLING_TIMER_CB

DESCRIPTION
  This function is called when the PDN inactivity timer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_polling_timer_cb(unsigned long callback_data)
{
  ds_cmd_type *cmd_ptr = NULL;
  void        **data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_MED("Polling timer call back");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_POLLING_TMR_EXP;
  data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
  *data_ptr = (void*)callback_data;

  ds_put_cmd_ext(cmd_ptr);

} /* ds_3gpp_pdn_cntx_polling_timer_cb */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_APN_PARAM_CHG_TIMER_CB

DESCRIPTION
  This function is called when the APN_PARAM_CHGtimer expires. This call
  back function simply posts a command to DS. DS then processes the command
  and takes suitable action

PARAMETERS
  unsigned long - Call back data, points to the PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_apn_param_chg_timer_cb(unsigned long callback_data)
{
  ds_cmd_type *cmd_ptr = NULL;
  void        **data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_MED("apn_param_chg_timer_cb");

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_APN_PARAM_CHG_TMR_EXP;
  data_ptr = (void**)cmd_ptr->cmd_payload_ptr;
  *data_ptr = (void*)callback_data;

  ds_put_cmd_ext(cmd_ptr);

} /* ds_3gpp_pdn_cntx_polling_timer_cb */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_GET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function returns DS_3GPP_PDN_CONTEXT's per subscription info handle

PARAMETERS
  subs_id: subscription ID

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static ds_3gpp_pdn_cntxt_per_subs_info_type* 
         ds_3gpp_pdn_context_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type *hndl = NULL;
  ds3gsubsmgr_subs_id_e_type           subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    hndl = ds_3gpp_pdn_context_info.per_subs_info_p[subs_indx];
  }

  return hndl;
} /* ds_3gpp_pdn_context_get_per_subs_info_hndl() */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_SET_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function sets DS_3GPP_PDN_CONTEXT's per subscription info handle

PARAMETERS
  subs_id: subscription ID
  hndl:    handle to DS_3GPP_PDN_CONTEXT's per subscription info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_pdn_context_set_per_subs_info_hndl
(
  sys_modem_as_id_e_type                 subs_id,
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx = DS3GSUBSMGR_SUBS_ID_INVALID;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    ds_3gpp_pdn_context_info.per_subs_info_p[subs_indx] = hndl;
  }
}/* ds_3gpp_pdn_context_set_per_subs_info_hndl()*/

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_VALIDATE_PER_SUBS_INFO_HNDL

DESCRIPTION
  This function validates DS_3GPP_PDN_CONTEXT's per subscription info handle

PARAMETERS
  hndl:    handle to DS_3GPP_PDN_CONTEXT's per subscription info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_pdn_context_validate_per_subs_info_hndl
(
  ds_3gpp_pdn_cntxt_per_subs_info_type *hndl
)
{
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (hndl != NULL && hndl->cookie == DS_3GPP_PDN_CNTXT_PER_SUBS_INFO_COOKIE)
  {
    ret_val = TRUE;
  }
  return ret_val;
} /* ds3gpp_validate_per_subs_info_hndl */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_PER_SUBS_INFO_INIT

DESCRIPTION
  This function initializes DS_3GPP_PDN_CONTEXT's per subscription info.

PARAMETERS
  subs_id          : Subscription Id 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_context_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type* per_subs_info_p = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  per_subs_info_p = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);

  do
  {
    if (per_subs_info_p != NULL)
    {
      DS_3GPP_MSG1_ERROR ("Memory already allocated for Subs id(CM): %d", 
                          subs_id );
      break;
    }
    else
    {
      per_subs_info_p = 
        (ds_3gpp_pdn_cntxt_per_subs_info_type*)
          modem_mem_alloc(sizeof(ds_3gpp_pdn_cntxt_per_subs_info_type),
                          MODEM_MEM_CLIENT_DATA);
      if (per_subs_info_p == NULL)
      {
        DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs id(CM): %d",
                            subs_id );
        break;
      }

      per_subs_info_p->cookie = DS_3GPP_PDN_CNTXT_PER_SUBS_INFO_COOKIE;
      per_subs_info_p->subs_id = subs_id;
      per_subs_info_p->max_simul_active_pdp_context = DS_3GPP_MAX_BEARER_CONTEXT;

      list_init(&(per_subs_info_p->blocked_plmn_list));
  
      memset(&(per_subs_info_p->current_plmn),
             0,
             sizeof(sys_plmn_id_s_type));

      ds_3gpp_pdn_context_set_per_subs_info_hndl(subs_id, per_subs_info_p);

      ds3g_profile_validator_per_subs_info_init(subs_id);

    }
  }
  while (0);
} /* ds_3gpp_pdn_context_per_subs_info_init() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CONTEXT_INFO_INIT

DESCRIPTION
  This function initializes all the globals in DS_3GPP_PDN_CONTEXT.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_context_info_init
(
  void
)
{
  /*======================================================================
     During powerup, we allocate PDN Context Info structure only for
     DDS.
   
     If Dual SIM / Triple SIM Device mode event is received at
     a later stage, we will allocate the structure for other
     subscriptions. 
  ======================================================================*/

  //ds_3gpp_pdn_context_per_subs_info_init(ds3g_get_ps_subs_id());

  ds_3gpp_pdn_cntx_set_rpm_plmn_blocked(FALSE);

  ds_3gpp_pdn_cntxt_set_th_mit_pdn_state(DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS);

} /* ds_3gpp_pdn_context_info_init */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_IF_APN_UP

DESCRIPTION
  This function is used to determin if an APN and IP type is up on 3GPP.
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntx_if_apn_up
(
  char                                        *apn,
  ps_iface_type                              **v4_iface_ptr,
  ps_iface_type                              **v6_iface_ptr,
  ps_sys_subscription_enum_type               subs_id
)
{
  uint8                                pdn_cntxt_index = 0;
  ds_pdn_context_s                    *pdn_cntxt_ptr = NULL;
  ds_umts_pdp_profile_type            *profile_p = NULL;
  boolean                              ret_val = FALSE;
  sys_modem_as_id_e_type               cm_sub_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((apn == NULL) || (v4_iface_ptr == NULL) || (v6_iface_ptr == NULL))
  {
    DS_3GPP_MSG0_ERROR("Input passed is NULL");
    return FALSE;
  }

  //Initialize the v4 and v6 ptrs
  *v4_iface_ptr = NULL;
  *v6_iface_ptr = NULL;

  cm_sub_id = ds3gsubsmgr_subs_id_ds_to_cm((ds_sys_subscription_enum_type)subs_id);
  for (pdn_cntxt_index=0; pdn_cntxt_index < DS_3GPP_MAX_PDN_CONTEXT;
       pdn_cntxt_index++)
  {
    pdn_cntxt_ptr = &ds_pdn_context_tbl[pdn_cntxt_index];

    if(!(ds_pdn_cntxt_validate_pdn_subscription(pdn_cntxt_ptr, cm_sub_id)))
    {
      continue;
    }

    profile_p = &(pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile);

    if (profile_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Profile ptr not populated for index:%d",
                         pdn_cntxt_index);
      continue;
    }

    /*------------------------------------------------------------------------
      Check if the apn passed matches with the profile_apn name or resolved
      apn name. If a match occurs, check if the pdn is up, return TRUE.
     ------------------------------------------------------------------------*/
    if((strncasecmp((char*)apn,(char*)profile_p->context.apn,
                     DS_UMTS_MAX_APN_STRING_LEN+1)== 0))
    {
      DS_3GPP_MSG1_HIGH("APN match for PDN context:0x%x",pdn_cntxt_ptr);

      if(pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
      {
        /*----------------------------------------------------------------------
        Don't break and continue loop. Maybe v4 single bearer and v6 single 
        bearer PDN context are UP in different PDN control block.  
        ----------------------------------------------------------------------*/
        if( *v4_iface_ptr == NULL )
        {
          *v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntxt_ptr);
        }
        if( *v6_iface_ptr == NULL )
        {
          *v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntxt_ptr);
        }
        ret_val = TRUE;
      }
    }
  }

  return ret_val;
}/*ds_3gpp_if_apn_up*/

/*===========================================================================
FUNCTION DS3GPP_INIT

DESCRIPTION
  This function initializes the 3GPP Packet-Switched Data mode-specific
  handlers. It is invoked during DS Task power-up.
  It creates interfaces for each pdn context and initializes the
  parameters for each pdn context.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask containing the REX signals that the handlers want to wait on.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_cntxt_3gpp_init( void )
{
  acl_type         *acl_ptr;                        /* ACL ptr               */
  //uint32           phys_link=0;  
  uint32           iface_index;
  ps_iface_type    *iface_ptr;  
  uint32           pdn_cntx_inst;             /* Unique PDN context instance */
  ds_3gpp_iface_s  *ds3gpp_iface_p;
#ifdef FEATURE_DATA_PS_IPV6
  ip6_sm_type               *ip6_sm_ptr;
  dssnet6_sm_cb_type        *dssnet6_sm_cb_ptr;
  ip6_sm_config_type        *ip6_sm_config_ptr=NULL;
#endif
  int                        iface_inst;
  int16                      ps_errno = 0;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
#if (DS_3GPP_MAX_PS_IFACES > DS_3GPP_MAX_DPL_IFACE_SUPPORTED)
  #error Need to update DPL as it is inconsistent with number of ifaces.
#endif
  /*-------------------------------------------------------------------------
    DPL iface ID which uniquely identifies each ps_iface
  -------------------------------------------------------------------------*/
  dpl_iid_ifname_enum_type  ifname[DS_3GPP_MAX_DPL_IFACE_SUPPORTED] =
  {
    DPL_IID_IFNAME_3GPP_CONTEXT_0,
    DPL_IID_IFNAME_3GPP_CONTEXT_1,
    DPL_IID_IFNAME_3GPP_CONTEXT_2,
    DPL_IID_IFNAME_3GPP_CONTEXT_3,
#ifndef FEATURE_DATA_DISABLE_MULTI_PDN
    DPL_IID_IFNAME_3GPP_CONTEXT_4,
    DPL_IID_IFNAME_3GPP_CONTEXT_5,
    DPL_IID_IFNAME_3GPP_CONTEXT_6,
    DPL_IID_IFNAME_3GPP_CONTEXT_7,
#ifndef FEATURE_DATA_MPSS_ULT /* !Low Tier */
    DPL_IID_IFNAME_3GPP_CONTEXT_8,
    DPL_IID_IFNAME_3GPP_CONTEXT_9,
    DPL_IID_IFNAME_3GPP_CONTEXT_10,
    DPL_IID_IFNAME_3GPP_CONTEXT_11,
    DPL_IID_IFNAME_3GPP_CONTEXT_12,
    DPL_IID_IFNAME_3GPP_CONTEXT_13,
#ifdef FEATURE_EMERGENCY_PDN
    DPL_IID_IFNAME_3GPP_EMERGENCY_0,
    DPL_IID_IFNAME_3GPP_EMERGENCY_1,
#else
    DPL_IID_IFNAME_3GPP_CONTEXT_14,
    DPL_IID_IFNAME_3GPP_CONTEXT_15,
#endif /* FEATURE_EMERGENCY_PDN */
#endif /* FEATURE_DATA_MPSS_ULT */
#endif /* FEATURE_DATA_DISABLE_MULTI_PDN */
  };

#ifdef FEATURE_DATA_PS_IPV6
  /*-------------------------------------------------------------------------
    determine if IPv6 is enabled in this build.
  -------------------------------------------------------------------------*/
  ip6_sm_config_ptr = ds3g_get_ipv6_sm_config_info(subs_id);
  ASSERT(ip6_sm_config_ptr);
  
  /*-------------------------------------------------------------------------
    Read failover NV settings for FAILOVER behavior.
  -------------------------------------------------------------------------*/

#endif /* FEATURE_DATA_PS_IPV6 */

  ds_3gpp_pdn_context_info_init();

  for(pdn_cntx_inst=0;pdn_cntx_inst<DS_3GPP_MAX_PDN_CONTEXT;pdn_cntx_inst++)
  {

    /*-------------------------------------------------------------------------
      Memset (with 0) the entry in the PDN context table for this instance.
    -------------------------------------------------------------------------*/
    memset((void*)&(ds_pdn_context_tbl[pdn_cntx_inst]),0x0,
           (sizeof(ds_pdn_context_s) ));



  } /* for DS_3GPP_MAX_PDN_CONTEXT */
  
  /*---------------------------------------------------------------------------
    Set the bearer context notifier call back table with the appropriate call
    back functions.
    When the bearer goes up or down, PDN context gets to know of the changes
    in the bearer state through these two functions.
  ---------------------------------------------------------------------------*/
  bearer_context_notifier_cb_tbl.bearer_up_ind_cb = dsPdnContext_bearerUpIndCb;
  bearer_context_notifier_cb_tbl.bearer_down_ind_cb = dsPdnContext_bearerDownIndCb;

  /*------------------------------------------------------------------------
    Assume that A2 level bridging is always possible. This value will be
    updated using a configuration item.
  ------------------------------------------------------------------------*/


  for(iface_index = 0; iface_index < DS_3GPP_MAX_PS_IFACES; iface_index++)
  {

    ds3gpp_iface_p = &(ds_iface_tbl[iface_index]);

    ds3gpp_iface_p->state = DS_3GPP_IFACE_INVALID_STATE;

    ds3gpp_iface_p->rm_iface_ptr = NULL;

    ds3gpp_iface_p->post_proc_done_flag = FALSE;

    ds3gpp_iface_p->ue_init_net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

    iface_ptr = &(ds3gpp_iface_p->ps_iface);

    /*-----------------------------------------------------------------------
      Get the acl_ptr.
    -----------------------------------------------------------------------*/

    if (ds_pdn_context_this_iface_is_for_emergency_pdn(ds3gpp_iface_p))
    {
      acl_ptr       = &(ds3gpp_iface_p->acl);
      acl_ptr->acl_fptr = ds3gpp_rt_acl_emergency_calls;
      acl_ptr->post_proc_fptr = ds3gpp_rt_acl_emergency_calls_post_proc;
    }
    else 
    {
       acl_ptr       = &(ds3gpp_iface_p->acl);
       acl_ptr->acl_fptr = ds3gpp_rt_acl;
       acl_ptr->post_proc_fptr = ds3gpp_rt_acl_post_proc;
    }

    ds3gpp_iface_p->in_use = FALSE;

    ds3gpp_iface_p->client_data_ptr = NULL;


    /*-----------------------------------------------------------------------
      Always associate the PS and DS iface. Set the client data
      pointer to point to the DS Iface struct

      But do not bind the DS Iface and the PDN context. This is because we do
      it dynamically.
    -----------------------------------------------------------------------*/
    iface_ptr->client_data_ptr = (void *)ds3gpp_iface_p;
    
    /*-----------------------------------------------------------------------
      Initialize the PS_IFACE info for this pdn context. PS Iface will be 
      created with no Phys links. This does not mean that we are creating a
      logical iface. It is a new provision whereby we can create an iface
      and bind it to a phys link later on. The Phys links are created as a
      part of bearer context init.
    -----------------------------------------------------------------------*/
    iface_inst = ps_iface_create(
                                   iface_ptr,
                                   UMTS_IFACE,
                                   acl_ptr,
                                   NULL,
                                   NULL,
                                   0
         );

    ASSERT(iface_inst >= 0);

    /*-----------------------------------------------------------------------
      These bring up/tear down the Interface.
    -----------------------------------------------------------------------*/
    iface_ptr->bring_up_cmd_f_ptr   = ds_pdn_cntxt_ifaceUpCmd;

    if (ds_pdn_context_this_iface_is_for_emergency_pdn(ds3gpp_iface_p))
    {
      iface_ptr->bring_up_cmd_f_ptr   = ds_pdn_cntxt_emergency_pdn_ifaceUpCmd;
    }

    iface_ptr->tear_down_cmd_f_ptr  = ds_pdn_cntxt_ifaceDownCmd;

    /*-----------------------------------------------------------------------
      Active iface in_use/out_use cmd callback.
    -----------------------------------------------------------------------*/
    iface_ptr->in_use_cmd_f_ptr   = ds_pdn_cntxt_ifaceInUseCmd;
    iface_ptr->out_use_cmd_f_ptr  = ds_pdn_cntxt_ifaceOutofUseCmd;

    /*-----------------------------------------------------------------------
      iface_ptr->iface_ioctl_f_ptr = dsumtsps_iface_ioctl_hdlr;
    -----------------------------------------------------------------------*/
    iface_ptr->iface_ioctl_f_ptr = ds_pdn_cntxt_ifaceIoctlHdlr;
      
    /*-----------------------------------------------------------------------
      Lingering callback.
    -----------------------------------------------------------------------*/
    iface_ptr->linger_cmd_f_ptr = ds_pdn_cntxt_ifaceLingerCmd;
      
    iface_ptr->bridge_proc_f_ptr = ds_pdn_cntxt_setupBridgeInterfaceEvent;  

    PS_IFACE_SET_CAPABILITY(iface_ptr,PS_IFACE_CAPABILITY_A2_CAPABLE);
    PS_IFACE_SET_IS_ACTIVE(iface_ptr, TRUE); 
    ps_iface_reset_logical(iface_ptr);

    /*-------------------------------------------------------------------------
      Set the group mask.
    -------------------------------------------------------------------------*/
    iface_ptr->group_mask        = (uint16)IFACE_EPC_GROUP;

    /*-----------------------------------------------------------------------
      Reset iface tx cmd as well
    -----------------------------------------------------------------------*/
    ps_iface_set_tx_function(iface_ptr, NULL, NULL);

    /*-----------------------------------------------------------------------
      Enable flow on the interface.
    -----------------------------------------------------------------------*/
    ps_iface_enable_ind(iface_ptr);

    /*-----------------------------------------------------------------------
      Enable the Linger capability on this IFACE.
    -----------------------------------------------------------------------*/
    if (ps_iface_enable_iface_linger(iface_ptr) < 0)
    {
      DS_3GPP_MSG1_ERROR("No Linger Timer associated with iface: 0x%p", iface_ptr);
    }

#ifdef FEATURE_DATA_PS_ADDR_MGMT
    /*-------------------------------------------------------------------------
      PS Iface Addr Mgmt layer requires all Um ifaces to either provide 
      a function for the iface addr_cb_f_ptr and dad_f_ptr or set them
      to NULL.
    -------------------------------------------------------------------------*/
    iface_ptr->addr_cb_f_ptr = NULL;
    iface_ptr->dad_f_ptr = NULL;
#endif /*FEATURE_DATA_PS_ADDR_MGMT*/

    /*-----------------------------------------------------------------------
      For all the ifaces, configure to work in either V4 or V6 mode. Set
      the IP address family to IPV4 as default. We set the IP family type
      while post processing route acl rules. Also we will deny an iface based
      on IP family only if it is already associated with a PDN context.
    -----------------------------------------------------------------------*/    
    {
#ifdef FEATURE_DATA_PS_IPV6
      /*-----------------------------------------------------------------------
        Get the pointers to DSSNET6 SM and IP6 SM
      -----------------------------------------------------------------------*/
      ip6_sm_ptr        = &(ds3gpp_iface_p->ip6_sm);
      dssnet6_sm_cb_ptr    = &(ds3gpp_iface_p->dssnet6_sm_cb);
     /*-----------------------------------------------------------------------
        Initialize the IP6 State Machine for this pkt instance.
      -----------------------------------------------------------------------*/
      if (ip6_sm_create( ip6_sm_ptr,
                    iface_ptr,
                    ip6_sm_config_ptr) != 0)
      {
        DATA_ERR_FATAL("Could not create ip6 sm");
      }
      /*-----------------------------------------------------------------------
       Initialize the DSSNET6 State Machine for this pkt instance. Last
       argument is for not using PPP.
      -----------------------------------------------------------------------*/
      if (dssnet6_sm_create( dssnet6_sm_cb_ptr,
                             iface_ptr,
                             ip6_sm_ptr,
                             FALSE,
                             subs_id) < 0)
      {
        DS_3GPP_MSG0_ERROR("Could not create dssnet6");
      }

      /* register callback for DSSNET UP/DOWN events for throttling */
      if (-1 == dssnet6_event_callback_reg(dssnet6_sm_cb_ptr,
                                  (dssnet6_event_cback_type)
                                  ds_3gpp_pdn_cntxt_handle_dssnet6_ev_cback,
                                  (void*)ds3gpp_iface_p))
      {
        DATA_ERR_FATAL("Could not register cb with DSSNET6");
      }

      /*---------------------------------------------------------------------
       dssnet6_sm_create sets Iface type to IPV6. Set the Iface to IPV4 by 
        default. 
      ----------------------------------------------------------------------*/

      if (ps_iface_set_addr_family(iface_ptr, IFACE_IPV4_ADDR_FAMILY) != 0)
      {
        DATA_ERR_FATAL("Could not set addr family to V4");
      }

      /**
        For now defining FEATURE_DATA_DISABLE_MULTI_PDN is incompatible with
        support for Emergency Calls. If emergency calls need to be supported in 
        this configuration; we need to analyse how to do it and what to do with the
        attach Iface.
      **/ 
#ifdef FEATURE_EMERGENCY_PDN
      #ifndef FEATURE_DATA_DISABLE_MULTI_PDN     
      #ifdef FEATURE_DATA_PS_IPV6
         if (&ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V6_PS_IFACE_IDX] == ds3gpp_iface_p)
         {
           //This iface is reserved for v6 emergency calls.
           if (ps_iface_set_addr_family(iface_ptr, IFACE_IPV6_ADDR_FAMILY) != 0)
           {
             DATA_ERR_FATAL("Could not set addr family to V4");
           }
         }
      #endif
      #endif
#endif /* FEATURE_EMERGENCY_PDN */

      /*---------------------------------------------------------------------
       Register callback for V6 iface down Ind from dssnet6
      -------------------------------------------------------------------- */
      if (-1 == dssnet6_iface_down_ind_callback_reg(dssnet6_sm_cb_ptr,
                                     (dssnet6_iface_down_ind_cb_type)
                                     ds_3gpp_pdn_cntxt_handle_v6_iface_down
                                  ))
      {
        DATA_ERR_FATAL("Could not register iface_down_ind cb with DSSNET6");
      }
#endif /* FEATURE_DATA_PS_IPV6 */
    }

    /*-----------------------------------------------------------------------
      Enable Logging on UMTS interfaces.        
    -----------------------------------------------------------------------*/
    (void) ps_iface_dpl_support_network_logging(iface_ptr,
                                     ifname[iface_index]);
    (void) ps_iface_dpl_set_iface_desc(iface_ptr, NULL);

    /* For now link logging and flow logging will be commented out
    (void) ps_iface_dpl_support_link_logging(iface_ptr,
                                     ifname[iface_index]);

    (void) ps_iface_dpl_support_flow_logging(iface_ptr,
                                     ifname[iface_index]); */


  }/* for DS_3GPP_MAX_PS_IFACES */

  /*Register for LPM call back*/
  ds_3gpp_event_register_cb(DS_3GPP_OPRT_MODE_CHG_EV, 
                              ds_3gpp_pdn_cntxt_handle_lpm_mode, 0);



  /* Registering ps_sys_ioctl_ex API for PS_SYS_PRIMARY_SUBS */
  if(ps_sys_ioctl_register_tech_cback_ex(PS_SYS_TECH_3GPP,
                                         ds_3gpp_pdn_cntx_ps_sys_ioctl_ex_hdlr,
                                         PS_SYS_PRIMARY_SUBS,
                                         &ps_errno) == -1)
  {
    DS_3GPP_MSG2_ERROR("ds_pdn_cntxt_3gpp_init: PS sys IOCTL handler reg" 
                       "failed with cause %d for Subs id(PS):%d",
                       ps_errno, PS_SYS_PRIMARY_SUBS);
  }

  //Vishal: Need to loop through. Ask Varun to change this.
  /* Registering ps_sys_ioctl_ex API for PS_SYS_SECONDARY_SUBS */
  if(ps_sys_ioctl_register_tech_cback_ex(PS_SYS_TECH_3GPP,
                                         ds_3gpp_pdn_cntx_ps_sys_ioctl_ex_hdlr,
                                         PS_SYS_SECONDARY_SUBS,
                                         &ps_errno) == -1)
  {
    DS_3GPP_MSG2_ERROR("ds_pdn_cntxt_3gpp_init: PS sys IOCTL handler reg" 
                       "failed with cause %d for Subs id(PS):%d",
                       ps_errno, PS_SYS_SECONDARY_SUBS);
  }

  /* Registering ps_sys_ioctl_ex API for PS_SYS_TERTIARY_SUBS */
  if(ps_sys_ioctl_register_tech_cback_ex(PS_SYS_TECH_3GPP,
                                         ds_3gpp_pdn_cntx_ps_sys_ioctl_ex_hdlr,
                                         PS_SYS_TERTIARY_SUBS,
                                         &ps_errno) == -1)
  {
    DS_3GPP_MSG2_ERROR("ds_pdn_cntxt_3gpp_init: PS sys IOCTL handler reg" 
                       "failed with cause %d for Subs id(PS):%d",
                       ps_errno, PS_SYS_TERTIARY_SUBS);
  }

  ds_3gpp_auth_mgr_init();

  /* Register with EPC for 3GPP PDN is up */
#ifdef FEATURE_EPC_HANDOFF
  ds_epc_util_reg_if_apn_up_on_3gpp(ds_3gpp_pdn_cntx_if_apn_up);
#endif /*FEATURE_EPC_HANDOFF*/

#ifdef FEATURE_DATA_LTE
  ds_3gpp_pdn_limit_powerup_init(subs_id);
#endif


}

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CONTEXT_V6_SM_UPDATE

DESCRIPTION
  This function updates the v6 sm with iface info

PARAMETERS
  IN
  *ds3gpp_iface_p - pointer to iface
  subs_id - subs id associated with iface

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_context_v6_sm_update
(
  ds_3gpp_iface_s  *ds3gpp_iface_p,
  sys_modem_as_id_e_type subs_id
)
  {
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type             *iface_ptr; 
  ip6_sm_type               *ip6_sm_ptr;
  dssnet6_sm_cb_type        *dssnet6_sm_cb_ptr;
  ip6_sm_config_type        *ip6_sm_config_ptr=NULL;
  /*-----------------------------------------------------------------------
    Get the pointers to DSSNET6 SM and IP6 SM
   -----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  iface_ptr = &(ds3gpp_iface_p->ps_iface); 
      
  ip6_sm_ptr        = &(ds3gpp_iface_p->ip6_sm);
  dssnet6_sm_cb_ptr = &(ds3gpp_iface_p->dssnet6_sm_cb);
  ip6_sm_config_ptr = ds3g_get_ipv6_sm_config_info(subs_id);


  if(ip6_sm_ptr == NULL    ||
    iface_ptr == NULL      ||
    dssnet6_sm_cb_ptr == NULL   ||
    iface_ptr == NULL )
  {
    DS_3GPP_MSG0_ERROR(" ip6 sm");
    return ;
  }

   /*-----------------------------------------------------------------------
   Initialize the IP6 State Machine for this pkt instance.
   -----------------------------------------------------------------------*/
   if (ip6_sm_update_config( ip6_sm_ptr,
        iface_ptr,
        ip6_sm_config_ptr) != 0)
    {
      DATA_ERR_FATAL("Could not create ip6 sm");
}
    /*-----------------------------------------------------------------------
    Initialize the DSSNET6 State Machine for this pkt instance. Last
    argument is for not using PPP.
    -----------------------------------------------------------------------*/
    if (dssnet6_sm_update( dssnet6_sm_cb_ptr,
                           iface_ptr,
                           ip6_sm_ptr,
                           FALSE,
                           subs_id) < 0)
    {
      DS_3GPP_MSG0_ERROR("Could not create dssnet6");
    }
#endif
} /* ds_3gpp_pdn_context_v6_sm_update */

/*===========================================================================
FUNCTION      DSPDNCNTX_SET_DEFAULT_BEARER

DESCRIPTION
  This function is used to set the default bearer field of a PDN context with
  the pointer to the bearer context provided.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to store
                       the default bearer info
    bearer_context_p - This is the pointer to the default bearer context

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cntxt_set_default_bearer
(
  ds_pdn_context_s    *pdn_context_p,
  void *bearer_context_p
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return;
  }

  pdn_context_p->ds_pdn_context_dyn_p
    ->def_bearer_context_ptr=(ds_bearer_context_s *)bearer_context_p;
} /*ds_pdn_cntxt_setDefaultBearer()*/

/*===========================================================================
FUNCTION      DSPDNCNTX_STORE_BEARER_CONTEXT

DESCRIPTION
  This function is used to store the bearer context information in the PDN
  context.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to store
                       the bearer
    bearer_context_p - This is the pointer to the bearer context
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - on successfully storing the bearer info
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_store_bearer_context
(
  ds_pdn_context_s    *pdn_context_p,
  void  *bearer_p
)
{
  int             bearer_index;
  boolean         retVal = DS3G_FAILURE;
  ds_bearer_context_s *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  bearer_context_p=(ds_bearer_context_s *)bearer_p;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return FALSE;
  }

  for(bearer_index = 0; 
      bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
      bearer_index++)
  {
    if(pdn_context_p->ds_pdn_context_dyn_p->
       bearer_context_ptr_tbl[bearer_index] == NULL)
    {
      /*---------------------------------------------------------------------
        We have an empty index to store the bearer context pointer.
      ---------------------------------------------------------------------*/
      pdn_context_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl
        [bearer_index] = (void*) bearer_context_p;
      retVal = DS3G_SUCCESS;
      break;
    }
  }
  return retVal;
}

/*===========================================================================
FUNCTION      DSPDNCNTX_RELEASE_BEARER_CONTEXT

DESCRIPTION
  This function is used to release the bearer context information in the PDN
  context.

PARAMETERS
  IN
    pdn_context_p    - This is the PDN context inside which we want to release
                       the bearer
    bearer_context_p - This is the pointer to the bearer context
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - on successfully releasing the bearer info
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_release_bearer_context
(
  ds_pdn_context_s    *pdn_context_p,
  void  *bearer_p
)
{
  int             bearer_index;
  boolean         retVal = DS3G_FAILURE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return FALSE;
  }

  for(bearer_index = 0; 
      bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
      bearer_index++)
  {
    if(pdn_context_p->ds_pdn_context_dyn_p->
       bearer_context_ptr_tbl[bearer_index] == bearer_p)
    {
      /*---------------------------------------------------------------------
        We have an empty index to store the bearer context pointer.
      ---------------------------------------------------------------------*/
      pdn_context_p->ds_pdn_context_dyn_p->
         bearer_context_ptr_tbl[bearer_index] =NULL;
      retVal = DS3G_SUCCESS;
      break;
    }
  }
  return retVal;
} /* ds_pdn_cntxt_release_bearer_context*/

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CONTEXT_GET_COMPANION_IFACE

DESCRIPTION
  Fetches the other Iface from the current Iface pointer.

PARAMETERS
  iface_p       - Pointer to the current iface.  

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  companion_iface_p - Pointer to the other iface.  

SIDE EFFECTS
  None.
===========================================================================*/
ps_iface_type* ds_3gpp_pdn_get_companion_iface
(
  ps_iface_type *iface_p
)
{
  uint32            ds_companion_iface_index = 0;
  ps_iface_type     *companion_iface_p = NULL;
  ds_pdn_context_s  *pdn_cntx_p = NULL;
  ds_3gpp_iface_s   *ds3gpp_iface_p = NULL;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;

  ASSERT(iface_p != NULL);
/*-------------------------------------------------------------------------
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    PDN context pointer.
-------------------------------------------------------------------------*/
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  ASSERT (ds3gpp_iface_p != NULL);

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return NULL;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return NULL;
  }


#ifdef FEATURE_DATA_PS_IPV6
  switch(ps_iface_get_addr_family(iface_p))
  {
    case IFACE_IPV4_ADDR_FAMILY:
      if (TRUE == ds3g_is_ipv6_enabled())
      {
        ds_companion_iface_index = DS_3GPP_PDN_CONTEXT_IPV6_IFACE;
      }
      else 
      {
        return NULL;
      }
      break;

    case IFACE_IPV6_ADDR_FAMILY:
      ds_companion_iface_index = DS_3GPP_PDN_CONTEXT_IPV4_IFACE;
      break;

    default:
      DATA_ERR_FATAL("Invalid IP type for Iface being brought up");
      /* To prevent lint error */
      ds_companion_iface_index = DS_3GPP_PDN_CONTEXT_IPV4_IFACE;
      break;
  }

  /*-----------------------------------------------------------------------
    Examine if the other iface entry in the PDN context has a valid entry
  -----------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->
     ds_3gpp_iface_tbl[ds_companion_iface_index] != NULL)
  {
    companion_iface_p = 
        &(pdn_cntx_p->ds_pdn_context_dyn_p->
          ds_3gpp_iface_tbl[ds_companion_iface_index]->ps_iface);
  } /* If the iface table has a valid entry for the other iface */
#endif /* FEATURE_DATA_PS_IPV6 */

  return companion_iface_p;
}

/*===========================================================================
FUNCTION      ds_pdn_cntxt_check_and_set_pdn_is_default

DESCRIPTION
  This function is called from ifaceup_cmd_hdlr and it sets a PDN as 
  default if none exists.

PARAMETERS
  pdn_cntx_p   - This PDN.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_check_and_set_pdn_is_default
(
  ds_pdn_context_s        *pdn_cntx_p
)
{
  ds_umts_pdp_context_type                   attach_pdp_context;
  ds_umts_pdp_profile_status_etype status =  DS_UMTS_PDP_FAIL;
  ds_pdn_context_s                          *def_pdn_cntx_p=NULL;
  uint16                                     def_profile_num = 0;
  sys_modem_as_id_e_type                    subs_id 
                                            = SYS_MODEM_AS_ID_NONE;
  sys_sys_mode_e_type                       network_mode = SYS_SYS_MODE_NONE;
/*-------------------------------------------------------------------------
    There is no default PDN. 
    Get the default profile and compare its APN and IP type with this PDN
    context's APN and IP type.
    If a call uses attach profile, then its PDN is default PDN
-------------------------------------------------------------------------*/
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

#ifdef FEATURE_DATA_LTE
  def_profile_num = ds_eps_get_attach_prof(subs_id);
#else
  def_profile_num = DS_3GPP_DEF_TETHERED_PROFILE;
#endif
  
 /*------------------------------------------------------------------------
  Check if we already have a default PDN. 
  is_default is set for LTE in bring_up_attach_pdn. 
  If in case is_default not set there, due to one of the following 
  reasons:
  - W case
  - App brings up the call with attach profile before LTE attach happens
  then we go ahead and set it here.
 -------------------------------------------------------------------------*/
  def_pdn_cntx_p = ds_3gpp_pdn_cntxt_get_def_pdn(subs_id);

  if(ds_3gpp_pdn_cntx_validate_pdn_context(def_pdn_cntx_p))
  {
    return DS3G_SUCCESS;
  }

  status = ds_umts_get_pdp_profile_context_info_per_subs( def_profile_num, 
                                         dsumts_subs_mgr_get_subs_id(subs_id),
                                                   &attach_pdp_context );
  /*-------------------------------------------------------------------------
     In case of attach profile is not present in EFS ,

     IF LTE_FEATURE is ENABLED GSM/WCDMA/TDSCDMA calls are to be allowed even
     if attach profile is not found in EFS, LTE call returns Failure when 
     attach profile is not found in EFS

     ELSE LTE_FEATURE is DISABLED GSM/WCDMA/TDSCDMA calls are to be allowed 
     even if DEF_TETHERED_PROFILE is not found in EFS ,
   
     If Network mode is not in G/W/T , return failure
 
     This function is used to update is_default flag which is used during
     L<->(G/W/T) IRAT
  -------------------------------------------------------------------------*/

  if ( status != DS_UMTS_PDP_SUCCESS )
  {
    network_mode = ds3gpp_get_current_network_mode(subs_id);
    DS_3GPP_MSG2_ERROR("Invalid attach profile %d found on network mode %d"
                        ,def_profile_num,network_mode);

    if ((network_mode == SYS_SYS_MODE_GSM) ||
        (network_mode == SYS_SYS_MODE_WCDMA)
  #ifdef FEATURE_TDSCDMA    
        ||(network_mode == SYS_SYS_MODE_TDS)
  #endif /*FEATURE_TDSCDMA*/
       )
    {
      DS_3GPP_MSG1_MED("Ignoring Attach profile on sys mode %d ",network_mode);
      pdn_cntx_p->ds_pdn_context_dyn_p->is_default = FALSE;
      return DS3G_SUCCESS;
    }

#ifdef FEATURE_DATA_LTE
    if ((network_mode == SYS_SYS_MODE_LTE ) && ds_eps_attach_sm_is_ue_attached(subs_id) )
    {
      DS_3GPP_MSG0_HIGH("ue is attached and attach profile is blocked "
                            "or invalid; not trying to set default pdn");
      return DS3G_SUCCESS;
    }
#endif

    return DS3G_FAILURE;
  }
  /*---------------------------------------------------------------------
    Do strict comparison for APN name.
    IP comparison need not be strict for the following reason
    - Assume a configuration error such that attach profile has apn
      APN1 and IP as v4v6 and profileX has apn as APN1 and IP as 
      v4only. 
    - In this case, if app brings up a call with profileX, before LTE 
      attach happens, then we assume PDN with profileX as default.
  ----------------------------------------------------------------------*/
  if( strcasecmp((char*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
                     (char *)attach_pdp_context.apn) == 0 )
  {
    switch(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type)
    {
    case DS_UMTS_PDP_IPV4:
      if( attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV4V6 )
      {
        DS_3GPP_MSG2_HIGH("Configuration error. Attach profile IP type is :%d "
                         "and current PDN's IP type is %d. ",
                         pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
                          context.pdp_type,
                         attach_pdp_context.pdp_type);
        DS_3GPP_MSG1_HIGH("Marking this PDN 0x%x as default, since APN "
                         "name matches.",pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else if (attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV4) 
      {
        DS_3GPP_MSG1_HIGH("APN and IP type matches. Setting PDN 0x%x as default",
                     pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else
      {
        //For any other pdp_type of attach profile, this PDN cannot be 
        // set default.
        DS_3GPP_MSG1_MED("PDN 0x%x is not default",
                     pdn_cntx_p);
      }
      break;

    case DS_UMTS_PDP_IPV6:
      if( attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV4V6 )
      {
        DS_3GPP_MSG2_HIGH("Configuration error. Attach profile IP type is :%d "
                         "and current PDN's IP type is %d. ",
                         pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type,
                         attach_pdp_context.pdp_type);
        DS_3GPP_MSG1_HIGH("Marking this PDN 0x%x as default, since APN "
                         "name matches.",pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else if (attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV6) 
      {
        DS_3GPP_MSG1_HIGH("APN and IP type matches. Setting PDN 0x%x as default",
                     pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else
      {
        //For any other pdp_type of attach profile, this PDN cannot be 
        // set default.
        DS_3GPP_MSG1_MED("PDN 0x%x is not default",
                     pdn_cntx_p);
      }
      break;

    case DS_UMTS_PDP_IPV4V6:
      if( attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV4 ||
          attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV6 ) 
      {
        DS_3GPP_MSG2_HIGH("Configuration error. Attach profile IP type is :%d "
                         "and current PDN's IP type is %d. ",
                         pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type,
                         attach_pdp_context.pdp_type);
        DS_3GPP_MSG1_HIGH("Marking this PDN 0x%x as default, since APN "
                         "name matches.",pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else if (attach_pdp_context.pdp_type == DS_UMTS_PDP_IPV4V6) 
      {
        DS_3GPP_MSG1_HIGH("APN and IP type matches. Setting PDN 0x%x as default",
                     pdn_cntx_p);
        pdn_cntx_p->ds_pdn_context_dyn_p->is_default = TRUE;
      }
      else
      {
        //For any other pdp_type of attach profile, this PDN cannot be 
        // set default.
        DS_3GPP_MSG1_MED("PDN 0x%x is not default",
                     pdn_cntx_p);
      }
      break;

    default:
        DS_3GPP_MSG1_ERROR("Invalid PDP type: %d",
                     pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type);
        break;
    }//Switch
  }//APN comparison pass
  else
  {
    DS_3GPP_MSG1_MED("PDN 0x%x is not default. APN name comparison fails.",
                     pdn_cntx_p);
  }
  
  return DS3G_SUCCESS;
}

/*===========================================================================
FUNCTION      ds_3gpp_pdn_context_cleanup_pdn

DESCRIPTION
  Moves the current Iface and other iface to DOWN state. Cleans the APN table 
  entry. Also frees the PDN context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought down.  
  down_reason   - Reason to cleanup 

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_pdn_context_cleanup_pdn
(
  ps_iface_type    *iface_p,
  ps_iface_net_down_reason_type down_reason
)
{
  ds_3gpp_iface_s  *ds3gpp_iface_p = NULL;
  ps_iface_type    *companion_iface_p = NULL;
  ds_pdn_context_s        *pdn_cntx_p = NULL;
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  uint8                    index = 0;
#endif /*FEATURE_DATA_LTE*/
  ps_iface_down_mh_event_info_type   down_info;

/*-------------------------------------------------------------------------*/
  
  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
  down_info.down_reason = down_reason;

  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  ASSERT (ds3gpp_iface_p != NULL);

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);
  ASSERT (pdn_cntx_p != NULL);

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }


  companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_p);

  down_info.bearer_tech = pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech;

  if (down_info.bearer_tech.rat_value == DS_SYS_RAT_EX_NULL_BEARER)
  {
    down_info.bearer_tech.rat_value = 
      ds_3gpp_map_sys_mode_to_ds_sys_rat
        (ds3gpp_get_current_network_mode(subs_id));
    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
  }

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);

  ps_iface_down_ind(iface_p, &down_info);  
  
  /*----------------------------------------------------------------------- 
    If the companion iface is in use and the anchor iface is throttled
    then do not post an iface down indication on the companion iface
  -----------------------------------------------------------------------*/
  if((companion_iface_p != NULL ) && (PS_IFACE_IS_IN_USE(companion_iface_p)))
  {
    DS_3GPP_MSG2_HIGH("PDN:0x%x not being torn down,companion_iface:0x%x in use",
                      pdn_cntx_p,companion_iface_p);
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return;
  }
  
  if( (companion_iface_p != NULL) &&
      (ps_iface_state(companion_iface_p) != IFACE_DOWN) )
  {      
    DS_3GPP_MSG0_HIGH("Posting down on companion iface");
    ds_pdn_cntxt_tearDownPSIface(companion_iface_p, down_reason);
  }
  /*------------------------------------------------------------------------
    Remove the entry from the APN table
  ------------------------------------------------------------------------*/
  if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
  {
    DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
  }

#ifdef FEATURE_DATA_LTE
  /* Find an entry in the PDN Limit Table.
     If an entry is found, validate if the entry can be cleared at this point.
     If yes, clear the entry */
  if((index = ds_3gpp_pdn_limit_tbl_find_entry
       ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
         subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
  {
    ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
  }
#endif /*FEATURE_DATA_LTE*/

  ds_pdn_cntxt_freeInstance(pdn_cntx_p);

  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_APN_THROTTLED

DESCRIPTION
  This function checks whether throttling is allowed for the Given PDN.
 
PARAMETERS 
  pdn_cnt_p      - Pointer to the PDN context
    
DEPENDENCIES

RETURN VALUE
  True -  if throttling is allowed
  False - otherwise

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_pdn_cntxt_throttling_is_allowed
(
  ds_pdn_context_s *pdn_cntx_p
)
{
#ifdef FEATURE_DATA_LTE
  ds_3gpp_pdn_throttle_sm_type sm_type;
  sys_modem_as_id_e_type       subs_id  = SYS_MODEM_AS_ID_NONE;


  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }
  /*---------------------------------------------------------------------- 
    If 3GPP Sys mode is not LTE we allow throttling.
    If 3GPP Sys mode is LTE but UE is not yet attached on LTE,
    we disallow throttling.
    If UE is attached on LTE and PDN limit Handling is Disabled,
    we allow throttling.
    If PDN Limit Handling is Enabled and this is a handover scenario,
    we disallow throttling.
  --------------------------------------------------------------------*/
  if (ds3g_get_current_network_mode_ex(subs_id) != SYS_SYS_MODE_LTE)
  {
    return TRUE;
  }

  else if (!ds_eps_attach_sm_is_ue_attached(subs_id))
  {
    return FALSE;
  }

  if (ds_3gpp_pdn_limit_is_enabled(subs_id) == FALSE)
  {
    return TRUE;
  }
  else
  {
    if (ds_3gpp_pdn_throt_get_sm_type
         (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
            subs_id, TRUE, &sm_type))
    {
      if ((sm_type == DS_3GPP_THROTTLE_TYPE_PDN_CONN_LIMIT || 
           sm_type == DS_3GPP_THROTTLE_TYPE_PDN_REQ_WAIT_TIME)
              && 
          (pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag == TRUE))
      {
        return FALSE;
      }
    }
  }
#endif /*FEATURE_DATA_LTE*/

  return TRUE;
}
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_APN_THROTTLED

DESCRIPTION
  This function checks if an apn is throttled or not
 
PARAMETERS 
  pdn_cnt_p      - Pointer to the PDN context
  iface_p        - Pointer to the iface which is to be examined
    
DEPENDENCIES

RETURN VALUE
  True -  if iface is throttled
  False - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_apn_throttled
(
  ds_pdn_context_s *pdn_cntx_p,
  ps_iface_type       *iface_p
)
{
  ds_pdn_context_type_e call_type = DS_3GPP_PDP_TYPE_IP_ANY;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return FALSE;
  }
  /*-----------------------------------------------------------------------
    Perform throttling check only for the following case:
    - this is not an initial attach
    - this is not a handover attach
  -----------------------------------------------------------------------*/
  if (ds_3gpp_pdn_cntxt_throttling_is_allowed(pdn_cntx_p) == FALSE)
  {
    return FALSE;
  }
 
  switch(ps_iface_get_addr_family(iface_p))
  {
    case IFACE_IPV4_ADDR_FAMILY:
      call_type = DS_3GPP_PDP_TYPE_IPV4;
      break;
    case IFACE_IPV6_ADDR_FAMILY:
      call_type = DS_3GPP_PDP_TYPE_IPV6;
      break;
    default:
      DS_3GPP_MSG2_ERROR("ds_3gpp_pdn_cntxt_is_apn_throttled: "
                         "Invalid IP type %d for Iface 0x%x being brought up",
                          call_type, iface_p);
      return FALSE;
  }
  /*-----------------------------------------------------------------------
    Check if this IP type is throttled  
  -----------------------------------------------------------------------*/
  if (ds_3gpp_pdn_throt_is_apn_throttled(pdn_cntx_p->ds_pdn_context_dyn_p->
                                         pdp_profile.context.apn, 
                                         call_type,
                                         subs_id))
  {      
    DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,
                            "ds_3gpp_pdn_cntxt_is_apn_throttled:"
                            "APN %s is throttled",
                            pdn_cntx_p->ds_pdn_context_dyn_p->
                            pdp_profile.context.apn);

     return TRUE;
  }
 
  return FALSE;
}
#ifdef FEATURE_DATA_DISABLE_MULTI_PDN
/*===========================================================================
FUNCTION      DS_PDN_CNTXT_MULTIPLE_PDP_ALLOWED

DESCRIPTION
  Validates if multiple pdn contexts are allowed   

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.    

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  pass/fail flag

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_multiple_pdp_allowed( ps_iface_type* iface_p)
{

  int                   loop_index;
  ds_3gpp_iface_s      *ds3gpp_iface_p = NULL;
  ds_pdn_context_s     *pdn_cntx_p = NULL;  
  
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  ASSERT (ds3gpp_iface_p != NULL);

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);
   for(loop_index = 0;loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
    {
    if ((ds_3gpp_pdn_cntx_validate_pdn_context_internal(
                                          &ds_pdn_context_tbl[loop_index])) &&
           (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->state != 
            DS_PDN_CONTEXT_STATE_DOWN ))
     {
       /*---------------------------------------------------------------------------
       Corner case to allow two interfaces to be brought up for Dual IP. 
       When the interafce is bound to a PDN context and when the state is coming up 
       DONT DENY the call, This would also handle the race condition when another 
       call is attempted.
       ----------------------------------------------------------------------------*/  
       if ((pdn_cntx_p != &ds_pdn_context_tbl[loop_index] ))
       {
           DS_3GPP_MSG0_ERROR("Multiple PDNs not supported");
      
           return DS3G_FAILURE;
       }
     }
    }
  return DS3G_SUCCESS;
}
#endif
/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_APPLY_BEARER_MGMT

DESCRIPTION 
    Bearer Management Algorithm is applied during iface bring up, when the
    device runs out of bearers to bring up an prioritized PDN connection
    and we need to release bearers from one or more existing PDN connections.
    This function does the following.
    * Checks if there are sufficient bearers available to bring up the
      prioritized PDN connection during iface bring up 
    * If there aren't sufficient bearers,
      - (A)If 'APN disconnect order' list is configured in EFS
        -> read 'APN identifiers' from the EFS and bring down corresponding
           PDN's in order as in EFS provided they are not either emergency PDN
           or the last active PDN contexts
        -> If its the last active PDN, attempt to release ded-bearers
           from last PDN case (C)
      - If the EFS file is not present (or) (A) is partially successful
        (B)-> bring down PDN's one by one as long as it is not the last
              active PDN and not emergency PDN
        (C)-> If (A)/(B) has failed and there is only one active PDN
              release ded-bearers from last PDN context.
PARAMETERS 
  iface_p          - the ps iface that is being brought up
  num_reqd_bearers - Required number of bearers to bring up iface                        
                             
DEPENDENCIES
  Invoked during iface bring up, where we make one more attempt, before denying
  the iface bring up  due to insufficient bearer resources for prioritized PDN
  connections(Eg: emerg PDN)

RETURN VALUE
DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL    -     Bearer Mgmt action Failed. 
                                            No Bearer to bring up iface
 
DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS - Bearer mgmt action in progress. 
                                            Possibly bearers will be
                                            available in some time
                                        
DS_3GPP_PDN_CNTXT_BEARER_MGMT_NOT_REQD    - Bearer mgmt action not required 
 
SIDE EFFECTS
  None.

===========================================================================*/

ds_3gpp_pdn_cntxt_bearer_mgmt_status_e ds_3gpp_pdn_cntxt_apply_bearer_mgmt
(
   ps_iface_type *iface_p,
   uint8 num_bearers_reqd 
)
{
  ds_3gpp_pdn_cntxt_bearer_mgmt_status_e ret_val = DS_3GPP_PDN_CNTXT_BEARER_MGMT_NOT_REQD;
  uint8                num_bearers_released       = 0;
  int                  num_valid_pdn        = 0;
  const int            INVALID_PDN_CNTXT_TBL_IDX  = -1;
  ds_pdn_context_s    *last_pdn_cntxt_p           = NULL;   
  ds_pdn_context_s    *pdn_disconn_p              = NULL;
  ds_pdn_context_s    *incoming_pdn_cntxt_p       = NULL;
  ds_3gpp_iface_s     *ds3gpp_iface_p             = NULL; 
  ds_bearer_context_s *def_bearer_cntxt_p         = NULL;
  ds_bearer_context_s *bearer_to_teardown_p       = NULL;
  int                  pdn_disconn_list[DS_3GPP_MAX_PDN_CONTEXT];
  /** holds the PDN contexts for APN's in the disconnect list */
  int                  idx                        = 0;
  uint32               filter_preced[DS_FI_PRECED_ARR_SIZE];
  ps_iface_net_down_reason_type  net_down_reason  = 
                                            PS_NET_DOWN_REASON_NOT_SPECIFIED;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
                  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  memset(filter_preced,0,(sizeof(uint32)*DS_FI_PRECED_ARR_SIZE));
  memset(pdn_disconn_list,INVALID_PDN_CNTXT_TBL_IDX,(sizeof(int)*
                                                    DS_3GPP_MAX_PDN_CONTEXT));
  /*----------------------------------------------------------------------- 
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    incoming PDN context pointer for which the Bearer mgmt is being applied
    -----------------------------------------------------------------------*/
  ds3gpp_iface_p        = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  incoming_pdn_cntxt_p  = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr); 
  /*-------------------------------------------------------------------------
    Validate Incoming PDN context. If the incoming PDN context itself is
    not valid, bearer mgmt need not be applied
    -------------------------------------------------------------------------*/
  /*remove */
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(incoming_pdn_cntxt_p))
  {
    return ret_val;
  }

   

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(incoming_pdn_cntxt_p, &subs_id))
  {
    return ret_val;
  }

  /*-------------------------------------------------------------------- 
      Validate number of bearers required for iface bringup
    --------------------------------------------------------------------*/
  if (num_bearers_reqd <= 0) 
  {   
    DS_3GPP_MSG1_MED("Num bearers reqd %d",num_bearers_reqd);
    return ret_val;
  }
      
  /*------------------------------------------------------------------------ 
    Fetch number of valid PDN contexts and free bearers available
    ------------------------------------------------------------------------*/
  num_bearers_released    = ds_3gpp_bearer_cntxt_get_num_free_bearers();
  num_valid_pdn           = ds_3gpp_pdn_cntxt_get_num_valid_pdn_contexts(subs_id);
  DS_3GPP_MSG3_MED("PDN's: '%d' Free Bearers: '%d' To release: '%d' bearers",
                       num_valid_pdn, num_bearers_released,
                       (num_bearers_reqd-num_bearers_released));
  /*------------------------------------------------------------------------
    Return if there are already sufficient number of bearers.
    Bearer mgmt policy need not be applied
    ------------------------------------------------------------------------*/
  if ((num_bearers_released >= num_bearers_reqd)||
      (num_valid_pdn <= 0)) 
  {
    return ret_val;
  }

  /************************************************************************ 
    CASE (A): FOR A MATCHING 'APN IDENTIFIER' <=> PDN
              TEAR DOWN PDN's SEQUENTIALLY AS CONFIGURED IN THE EFS
              UNTIL SUFFICIENT BEARERS ARE AVAILABLE FOR EMERGENCY PDN
   *************************************************************************/
  /*----------------------------------------------------------------------- 
    Get the matching PDN context(s) for all the APN identifiers in the list
    if they exist in 'pdn_disconn_list'
    -----------------------------------------------------------------------*/
  if (!ds_3gppi_utils_is_apn_disconn_order_list_empty(subs_id))
  {
    /*--------------------------------------------------------------------- 
      APN disconnect order list not emmty. Get all matching PDN context
      tbl index's for APN identifiers into 'pdn_disconn_list'
      ---------------------------------------------------------------------*/
    DS_3GPP_MSG0_MED("Find matching PDN <=> APN identifiers from APN "
                     "disconnect order list");
    ds_3gpp_get_pdn_disconn_list_frm_apn_disonn_order_list(pdn_disconn_list,subs_id);

    /*--------------------------------------------------------------------- 
      Loop through 'pdn_disconn_list' and bring down PDN's until we get
      required number of bearers 
      ---------------------------------------------------------------------*/
    for (idx=0;idx<DS_3GPP_MAX_PDN_CONTEXT;idx++)
    {
      if (pdn_disconn_list[idx]<0)
      {
        DS_3GPP_MSG0_MED("Reached end of APN disconnect order list. No other"
                         " matching PDN cntxts found");
        break;
      }
      pdn_disconn_p = &ds_pdn_context_tbl[pdn_disconn_list[idx]];
      /*--------------------------------------------------------------------- 
        PDN context found at PDN context table index 'pdn_disconn_list[idx]'
        Validate PDN context for teardown
        ---------------------------------------------------------------------*/
      if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_disconn_p))
      {
        continue;
      }
      /*--------------------------------------------------------------------- 
        Do not tear down the PDN context for which we are applying
        bearer mgmt
        ---------------------------------------------------------------------*/
      if (incoming_pdn_cntxt_p == pdn_disconn_p)
      {
        DS_3GPP_MSG1_HIGH("Cannot tear down incoming PDN Cntxt 0x%x"
                          "Bearer mgmt is in progress for this PDN"
                          ,pdn_disconn_p);
        continue;
      }
      /*--------------------------------------------------------------------- 
          Emergency PDN contexts are left untouched, in case they are active
        ---------------------------------------------------------------------*/
      if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_disconn_p))
      {
        DS_3GPP_MSG1_HIGH("Cannot tear down emerg PDN Cntxt 0x%x"
                          ,pdn_disconn_p);
        continue;
      }
      /*------------------------------------------------------------- 
        Break the loop if this is the last PDN context
        Possibly, this last PDN context almost holds almost
        all the bearers the device supports
        -------------------------------------------------------------*/
      if (num_valid_pdn == 1)
      {
        DS_3GPP_MSG1_HIGH("Cannot tear down the last PDN Cntxt 0x%x",
                           pdn_disconn_p);
        last_pdn_cntxt_p   = pdn_disconn_p;
        break;
      }
      /*--------------------------------------------------------------------- 
        In a normal case when the existing PDN connections are all active,
        the loop will break when num_active_pdns = 1. However,we are
        considering a case, where there are other upcoming (or) going down
        PDN connections which are not completely up/down but there is only
        one active PDN connection at present.
        ---------------------------------------------------------------------*/
      
      if (DS_3GPP_IS_PDN_CONTEXT_STATE_UP(pdn_disconn_p) && 
          (!dsPdnContext_is_any_other_normal_pdn_up(pdn_disconn_p, subs_id)))
      { 
        DS_3GPP_MSG1_HIGH("Possibly last active PDN Cntxt 0x%x at present"
                          "Do not tear down",pdn_disconn_p);
        /*------------------------------------------------------------------
          This may be the last 'active' PDN, but not the last of the valid
          PDN contexts. In this case, see if we can bring down other PDN
          contexts, which are not fully up/down.If this attempts fail,
          finally we will make an attempt to release dedicated bearers from
          the last PDN (case(C)) if possible
          ------------------------------------------------------------------*/
        last_pdn_cntxt_p   = pdn_disconn_p;
        continue;
        
      }

      /*----------------------------------------------------------------- 
        Decrement number of active PDN contexts
        -----------------------------------------------------------------*/
        num_valid_pdn--;

      /*--------------------------------------------------------------------- 
        Initiate PDN disconnect. Upon successful disconnect, we will get
        all the beaerers associated with this PDN.
        ---------------------------------------------------------------------*/
      net_down_reason = PS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
      pdn_disconn_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN; 
      if (!ds_3gpp_pdn_cntx_teardown_PDN(pdn_disconn_p,net_down_reason))
      {
        DS_3GPP_MSG1_HIGH("PDN disconnect for 0x%x failed",
                          pdn_disconn_p);
        continue;
      }
      /*--------------------------------------------------------------------- 
        Fetch the number of bearers that will be available from the
        PDN context for which we will initiate a PDN tear down.
        ---------------------------------------------------------------------*/
      num_bearers_released   += (uint8)pdn_disconn_p->ds_pdn_context_dyn_p
                                                  ->num_active_bearer_context;

      DS_3GPP_MSG2_HIGH("bringing down PDN 0x%x.Still Need to rel %d bearers",
                       pdn_disconn_p,(num_bearers_reqd-num_bearers_released));


      if (num_bearers_released >= num_bearers_reqd) 
      {
        ret_val =  DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS;
        break;
      }
    }/* for (idx =0)*/
    if (ret_val == DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS) 
    {
      return ret_val;
    }
  }/*ds_eps_pdn_cntx_is_apn_disconn_order_list_empty*/
  else
  {
      DS_3GPP_MSG0_MED("APN disconnect order list not configured in the EFS."
                       "Attempting to allocate %d bearers from existing active"
                       "PDN connections");
  }
  /*------------------------------------------------------------------------
    case (B): if case(A) partially succeeds (or) if 'apn disconnect order
    list' is not configured in the EFS, teardown active PDN's to get
    sufficient bearers (or) until, there is only one active PDN connection
    remaining.
   
    Skip (B) and move to case(C) if there is only one active PDN connection
    from (A)
    -----------------------------------------------------------------------*/
  if ((num_valid_pdn > 1)||(last_pdn_cntxt_p == NULL))
  {

    /************************************************************************ 
      CASE (B): TEAR DOWN PDN's. THE ORDER THEY WILL BE BROUGHT
                DOWN DEPENDS ON THE INDEX THEY OCCUPY IN THE PDN CONTEXT TBL
                p.s: We will hit case (B) when bearer mgmt action on case (A)
                either fails (or) partially succeeds
     *************************************************************************/

    DS_3GPP_MSG0_HIGH("Check if we can disconnect any other PDN");

    /*----------------------------------------------------------------------- 
      Loop through PDN context table and tear down non-default/non-emerg
      PDN's. Start traversing through the table in reverse order as there
      are higher chances that the PDN contxt's created later, may not be
      the attach PDN context.
      -----------------------------------------------------------------------*/
    for (idx = (DS_3GPP_MAX_PDN_CONTEXT-1); idx >=0;idx--)
    {
      pdn_disconn_p = &ds_pdn_context_tbl[idx];

      if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_disconn_p))
      {
        continue;
      }
      /*--------------------------------------------------------------------- 
        Do not tear down the PDN context for which we are applying
        bearer mgmt
        ---------------------------------------------------------------------*/
      if (incoming_pdn_cntxt_p == pdn_disconn_p)
      {
        DS_3GPP_MSG1_HIGH("Cannot tear down incoming PDN Cntxt 0x%x. Bearer "
                          "mgmt in progress for this PDN",pdn_disconn_p);
        continue;
      }
      /*-------------------------------------------------------------------- 
        Skip if this the emergency PDN context
        --------------------------------------------------------------------*/
      if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_disconn_p))
      {
         DS_3GPP_MSG1_HIGH("Cannot tear down emerg PDN 0x%x",pdn_disconn_p);
         continue;
      }

      /*--------------------------------------------------------------------- 
        Check if we have already triggered PDN disconnect for this
        PDN context.
        ---------------------------------------------------------------------*/
      if (ds_3gpp_pdn_cntxt_search_and_update_pdn_disconn_in_progress_list
          (idx,pdn_disconn_list)) 
      {
        DS_3GPP_MSG1_HIGH("PDN teardown for 0x%x already in progress",
                          pdn_disconn_p);
        continue;
      }
      /*------------------------------------------------------------- 
        Break the loop if this is the last active PDN context
        Possibly, this PDN context almost holds almost
        all the bearers the device supports
        -------------------------------------------------------------*/
      
      if ( num_valid_pdn == 1)           
      {
        DS_3GPP_MSG1_HIGH("Cannot tear down the last PDN Cntxt 0x%x",
                          pdn_disconn_p);
        last_pdn_cntxt_p   = pdn_disconn_p;
        break;
      }

      /*--------------------------------------------------------------------- 
        In a normal case when the existing PDN connections are all active,
        the loop will break when num_active_pdns = 1. However,we are
        considering a case, where there are other upcoming (or) going down
        PDN connections which are not completely up/down but there is only
        one active PDN connection at present.
        ---------------------------------------------------------------------*/
      if (DS_3GPP_IS_PDN_CONTEXT_STATE_UP(pdn_disconn_p) && 
          (!dsPdnContext_is_any_other_normal_pdn_up(pdn_disconn_p, subs_id)))
      { 
        DS_3GPP_MSG1_HIGH("Possibly last active PDN Cntxt 0x%x"
                          "Do not tear down",pdn_disconn_p);
        /*------------------------------------------------------------------
          This may be the last 'active' PDN, but not the last of the valid
          PDN contexts. In this case, see if we can bring down other PDN
          contexts, which are not fully up/down.If this attempts fail,
          finally we will make an attempt to release dedicated bearers from
          the last PDN (case(C)) if possible
          ------------------------------------------------------------------*/
        last_pdn_cntxt_p   = pdn_disconn_p;
        continue;
        
      }
      /*----------------------------------------------------------------- 
        Decrement number of active PDN contexts
        -----------------------------------------------------------------*/
      num_valid_pdn--;

      /*--------------------------------------------------------------------- 
        Initiate PDN disconnect. Upon successful disconnect, we will get
        all the beaerers associated with this PDN.
        ---------------------------------------------------------------------*/
      DS_3GPP_MSG2_HIGH("Initiate PDN disconnect for 0x%x PDN context index"
                        " %d",pdn_disconn_p,idx);
      net_down_reason = PS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
      pdn_disconn_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN; 
      if (!ds_3gpp_pdn_cntx_teardown_PDN(pdn_disconn_p,net_down_reason))
      {
        DS_3GPP_MSG1_HIGH("Issuing PDN disconnect for 0x%x failed",
                          pdn_disconn_p);
        continue;
      }

      /*------------------------------------------------------------------- 
        Fetch the number of bearers that will be available from the
        PDN context for which we will initiate a PDN tear down.
        -------------------------------------------------------------------*/
      num_bearers_released   += (uint8)pdn_disconn_p->ds_pdn_context_dyn_p->
                                    num_active_bearer_context;

      DS_3GPP_MSG2_MED("PDN disconnect for 0x%x Initiated. Still Need"
                       " to release %d active bearers",pdn_disconn_p,
                       (num_bearers_reqd-num_bearers_released));

      if (num_bearers_released >= num_bearers_reqd) 
      {
        ret_val =  DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS;
        break;
      }
    }/*for (idx = (DS_3GPP_MAX_PDN_CONTEXT-1)*/    
    /*-------------------------------------------------------------------- 
      Return if we have successfully brought down PDN's to get
      sufficient bearers we need
      --------------------------------------------------------------------*/
    if (ret_val == DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS) 
    {
       return ret_val;
    }
  }/*if (num_active_pdns > 1)*/
  
  /************************************************************************ 
    CASE (C): TEAR DOWN DED-BEARERS OF THE LAST PDN IF POSSIBLE. THE
              DED-BEARERS ARE PICKED UP BASED ON THE INDEX THEY OCCUPY
              IN THE 'bearer_context_ptr_tbl' they occupy for the last
              PDN context
               p.s: We will hit case (C) when bearer mgmt action on
               case (A)/Case(B) either fails (or) partially succeeds
   *************************************************************************/
  /*------------------------------------------------------------------------ 
    Last PDN holds all the Bearer resources supported by the device.
    Release only dedicated Bearers as releasing the default bearer will
    trigger PDN disconnect.
    -----------------------------------------------------------------------*/
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(last_pdn_cntxt_p)) 
  {
    ret_val = DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL;
    return ret_val;
  }

  DS_3GPP_MSG1_HIGH("Check if we can release any ded-bearers frm PDN 0x%x",
                    last_pdn_cntxt_p);
  /*------------------------------------------------------------------------ 
    Fetch the default Bearer context of the last PDN. This is reqd to
    make sure, we do not bring down default bearer, as tearing down
    default bearer will trigger a PDN disconnect.
    ------------------------------------------------------------------------*/
  def_bearer_cntxt_p = last_pdn_cntxt_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

  for (idx =(DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT-1); idx>=0;idx--)
  {
    bearer_to_teardown_p = last_pdn_cntxt_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[idx];

    if ((!ds_bearer_cntx_validate_bearer_context(bearer_to_teardown_p))||
        (bearer_to_teardown_p == def_bearer_cntxt_p))
    {
      continue;
    }
    /*---------------------------------------------------------------------- 
      Found a dedicated bearer context. Trigger UE bearer modify request
      for deactivating dedicated bearer
      ----------------------------------------------------------------------*/
    DS_3GPP_MSG1_HIGH("Tearing down ded bearer 0x%x",bearer_to_teardown_p);

    if (ds_flow_manager_teardown_bearer((void*)bearer_to_teardown_p,
                                        filter_preced,
                                        FALSE) != 0)
    {
      DS_3GPP_MSG0_HIGH("Tearing down ded bearer cntxt failed");
      continue;
    }
    /*---------------------------------------------------------------------- 
      Compute total number of bearers that will be released. Check if we have
      sufficient bearer resources to bring up PDN later
      ----------------------------------------------------------------------*/
    num_bearers_released++;

    DS_3GPP_MSG1_MED("Initiated 1 ded-bearer release. Still Need %d "
                     "bearers",(num_bearers_reqd-num_bearers_released));

    if (num_bearers_released >= num_bearers_reqd) 
    {    
      ret_val =  DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS;
      break;
    }
  }  

  /*-------------------------------------------------------------------- 
    Return failure, if still we cannot allocate bearers to bringup
    iface
    --------------------------------------------------------------------*/

  if (ret_val == DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL)  
  {
    DS_3GPP_MSG0_HIGH("No sufficient Bearers to bring up Iface."
                      "Bearer Mgmt on iface failed");  
  }

   return ret_val;
}

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CONTEXT_CHECK_FOR_PHYS_LINK_TEARDOWN

DESCRIPTION
  This function checks if Phys Link needs to be torn down, based on
  State of Ifaces whether they are in use or not

PARAMETERS 
  pdn_context - PDN context
  ps_iface    - iface being torn down
  other_ds_iface_p - companion ds iface of the iface being torn

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If Phys Link can be torn down
  FALSE: otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_context_check_for_phys_link_teardown
(
  ds_pdn_context_s                   *pdn_context_p,
  ps_iface_type                      *ps_iface_p,
  ds_3gpp_iface_s                    *other_ds_iface_p
)
{
  boolean                             ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate incoming iface ptr
  -------------------------------------------------------------------------*/
  if(NULL == ps_iface_p)
  {
    DS_3GPP_MSG0_ERROR("NULL iface passed as input");
    return ret_val;
  }

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    DS_3GPP_MSG0_ERROR("Invalid PDN context, No Phys Link tearing !!!");
    return ret_val;
  }

  if(
     (NULL == other_ds_iface_p) ||
     ((FALSE == PS_IFACE_IS_IN_USE(&(other_ds_iface_p->ps_iface)))
#if defined FEATURE_DATA_LTE
   && (!ds_pdn_cntx_is_last_prevailing_pdn_in_lte(pdn_context_p))
#endif                   
        )
   ||  (PS_IFACEI_IS_IN_ARBITRATION(ps_iface_p)) 
   ||  (DS_PDN_CONTEXT_STATE_GOING_DOWN == pdn_context_p->ds_pdn_context_dyn_p->state)
   )
  {
    DS_3GPP_MSG1_MED("Phys Link can be torn down for PDN: 0x%x",pdn_context_p);
    ret_val = TRUE;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION      DS_3GPP_BRING_DOWN_PHYS_LINK

DESCRIPTION
  Called in DS TASK context in response to an attempt to bring down the
  physical link associated with the incoming Iface

PARAMETERS
  ps_iface - Ps iface pointer
  info_ptr - void pointer

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_validate_and_bring_down_phys_link
(
  ps_iface_type   *iface_p,
  void            *info_ptr,
  ps_iface_net_down_reason_type  down_reason
)
{

  int16                ps_errno             = 0;             
  ps_phys_link_type   *phys_link_p          = NULL; 
  ds_pdn_context_s    *pdn_cntx_p           = NULL;
  ds_3gpp_iface_s     *ds_iface_p           = NULL;
  ds_3gpp_iface_s     *other_ds_iface_p     = NULL;
  ds_bearer_context_s *def_bearer_cntxt_p     = NULL;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  uint8                     index = 0;
#endif   /* FEATURE_DATA_LTE */
  ps_iface_down_mh_event_info_type   down_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));
  down_info.down_reason = down_reason;

  /*------------------------------------------------------------------------- 
    Validate incoming iface ptr
    -------------------------------------------------------------------------*/
  if(iface_p == NULL)
  {
    DATA_ERR_FATAL("NULL iface passed as input");
    return;
  }
  /*------------------------------------------------------------------------- 
    No need to continue if the iface is already down
    -------------------------------------------------------------------------*/
  if ((ps_iface_state(iface_p) == IFACE_DOWN)           ||
      (ps_iface_state(iface_p) == IFACE_STATE_INVALID)  ||
      (ps_iface_state(iface_p) == IFACE_DISABLED)
     )
  {
    DS_3GPP_MSG1_HIGH("Iface 0x%x state %d  Ignoring",iface_p);
    return;
  }
  /*------------------------------------------------------------------------- 
    Fetch/Validate the PDN context pointer from the iface pointer
    -------------------------------------------------------------------------*/
  ds_iface_p      = (ds_3gpp_iface_s *)(iface_p->client_data_ptr); 

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG2_ERROR("No DS iface associated with PS iface 0x%x:%d",
                       iface_p,iface_p->instance);
    ASSERT(0);
    return;
  }

  pdn_cntx_p      = (ds_pdn_context_s*)(ds_iface_p->client_data_ptr);

   if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
     DS_3GPP_MSG1_ERROR("PDN context could not fetch valid sub id%d"
                        ,subs_id);
    return;
  }

  down_info.bearer_tech = pdn_cntx_p->ds_pdn_context_dyn_p->bearer_tech;

  if (down_info.bearer_tech.rat_value == DS_SYS_RAT_EX_NULL_BEARER)
  {
    down_info.bearer_tech.rat_value =
      ds_3gpp_map_sys_mode_to_ds_sys_rat
        (ds3gpp_get_current_network_mode(subs_id));
    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
  }

  /*------------------------------------------------------------------------- 
     Find the companion iface from the PDN context associated with the
     iface
    -------------------------------------------------------------------------*/
  if (ps_iface_addr_family_is_v4(iface_p))
  {
#ifdef FEATURE_DATA_PS_IPV6
    other_ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                                            [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
#endif
    /*--------------------------------------------------------------------- 
      Invoke Flow manager out of use cmd to clean up
      dedicated bearer (if any)
      ---------------------------------------------------------------------*/ 
    ds_flow_manager_iface_out_of_use(pdn_cntx_p,
                                     ds_iface_p,
                                     DS_3GPP_PDP_TYPE_IPV4,TRUE);

  }

#ifdef FEATURE_DATA_PS_IPV6
  else if (ps_iface_addr_family_is_v6(iface_p))
  {
    other_ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                                              [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

     /*--------------------------------------------------------------------- 
       Invoke Flow manager out of use cmd to clean up
       dedicated bearer (if any)
       ---------------------------------------------------------------------*/ 
     ds_flow_manager_iface_out_of_use(pdn_cntx_p,
                                      ds_iface_p,
                                      DS_3GPP_PDP_TYPE_IPV6,TRUE);
  }
#endif
  else
  {
    DATA_ERR_FATAL("iface family type unset");
  }

  /*-------------------------------------------------------------------------
    Get the phys link pointer. It is possible that the bearer context pointer
    is null. That is because of a bearer rejection due to unacceptable IP type
    In such a case, we will locally clean up the PDN context.

    If the other iface is up and in use in a dual IP pdn, we will keep the
    PDN connection active, otherwise we will call phys link down, which will
    in turn trigger pdn disconnection with the exception of the following
    sceanrio. In a dual IP pdn on LTE, if the ifaces to be torn down
    belongs to the last PDN that is up, we try to retain the other iface
    (so that we are attached on LTE) even if it is not in use. 

    If the PDN context is in Going Down state, we do not care about the 
    "in-use" flag of the other iface. It is a given that we have to tear
    down the context. Note that this will happen if the PDN inactivity timer
    has expired.
    -------------------------------------------------------------------------*/
  DS_3GPP_MSG3_HIGH( "check if physlink can be brought down for Iface [0x%x:%d]"
                     "Is iface in arbitration? %d", 
                     iface_p->name,
                     iface_p->instance,
                     PS_IFACEI_IS_IN_ARBITRATION(iface_p) );

  /*------------------------------------------------------------------------- 
    If no bearer is associated with the PDN context to bring down phys link
    perform local cleanup
    -------------------------------------------------------------------------*/
  def_bearer_cntxt_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

  if(!ds_bearer_cntx_validate_bearer_context(def_bearer_cntxt_p))
  {
    ps_iface_down_ind(iface_p,&down_info);

    /*----------------------------------------------------------------------- 
      No bearer context associated with the PDN. In that case free the PDN
      instance if the iface associated with the PDN is down (local cleanup)
      -----------------------------------------------------------------------*/
    if ((other_ds_iface_p == NULL) ||
        (ps_iface_state(&(other_ds_iface_p->ps_iface)) == IFACE_DOWN))
    {
      DS_3GPP_MSG1_ERROR("No bearer context associated with. Remove the PDN "
                         "entry from the APN table and free the PDN 0x%x",
                         pdn_cntx_p);

      /*------------------------------------------------------------------------
        Remove the entry from the APN table
      ------------------------------------------------------------------------*/
      if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
      }

#ifdef FEATURE_DATA_LTE
      /* Find an entry in the PDN Limit Table.
         If an entry is found, validate if the entry can be cleared at this point.
         If yes, clear the entry */
      if((index = ds_3gpp_pdn_limit_tbl_find_entry
           ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
             subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
      {
        ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
      }
#endif /*FEATURE_DATA_LTE*/

      ds_pdn_cntxt_freeInstance(pdn_cntx_p);
    }
  }
  else
  {
    if (TRUE == ds_3gpp_pdn_context_check_for_phys_link_teardown(pdn_cntx_p, 
                                                                 iface_p,
                                                                 other_ds_iface_p))
    {
      phys_link_p = &(def_bearer_cntxt_p->phys_link);
      /*--------------------------------------------------------------------- 
        Disable flow on the primary phys link.
        ---------------------------------------------------------------------*/
      ps_phys_link_disable_flow(phys_link_p,DS_FLOW_UMTS_MASK);

      DS_3GPP_MSG2_HIGH( "bringing down physlink associated with Iface "
                         "[0x%x:%d]", iface_p->name,iface_p->instance);

      /*---------------------------------------------------------------------
        Bring down the physical link by calling ps_phys_link_down_cmd
      -----------------------------------------------------------------------*/
      if (0 == ps_phys_link_down_cmd(phys_link_p,&ps_errno,info_ptr)) 
      {
        /*----------------------------------------------------------------------- 
          Change the state of the PDN context to going down
          -----------------------------------------------------------------------*/
        pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;

        /*-------------------------------------------------------------------
          If the physical link is already down, initiate a call end. This
          part of the code is executed only when an iface down is received
          while the call is dormant. In such a case, we will just post a
          DS_PHYS_LINK_DOWN cmd. This will make sure that the bearer is
          cleaned up.
          -------------------------------------------------------------------*/
        if(ds_bearer_cntxt_phys_link_down(phys_link_p,info_ptr) != 0)
        {
          DS_3GPP_MSG0_ERROR("Bearer phys link down returned error");

          ps_iface_down_ind(iface_p,&down_info);
          /*----------------------------------------------------------------- 
            This is just an error handling. In case Mode handler attempt
            to bring down physlink fails
            -----------------------------------------------------------------*/
          if((other_ds_iface_p == NULL) ||
             (ps_iface_state(&(other_ds_iface_p->ps_iface)) == IFACE_DOWN))
          {
            DS_3GPP_MSG1_ERROR("Failed to bring down physlink. Remove PDN the "
                               "entry from the APN table and free the PDN 0x%x",
                               pdn_cntx_p);

            /*------------------------------------------------------------------------
              Remove the entry from the APN table
            ------------------------------------------------------------------------*/
            if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
            }

#ifdef FEATURE_DATA_LTE
           /* Find an entry in the PDN Limit Table. If an entry is found,
              validate if the entry can be cleared at this point. If yes,
              clear the entry */
            if((index = ds_3gpp_pdn_limit_tbl_find_entry
                 ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
                   subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
            {
              ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
            }
#endif /*FEATURE_DATA_LTE*/

            ds_pdn_cntxt_freeInstance(pdn_cntx_p);
          }

        }

      } /* ps_phys_link_down_cmd */
      else 
      {
        /*------------------------------------------------------------------- 
          Perform local clean up if the Physlink cannot be brought down for
          some reason
          -------------------------------------------------------------------*/
        if(ps_errno != DS_EWOULDBLOCK)
        {
          ps_iface_down_ind(iface_p,&down_info);

          if((other_ds_iface_p == NULL) ||
             (ps_iface_state(&(other_ds_iface_p->ps_iface)) == IFACE_DOWN))
          {
            DS_3GPP_MSG1_ERROR("Failed to bring down physlink. Remove PDN the "
                               "entry from the APN table and free the PDN 0x%x",
                               pdn_cntx_p);

            /*------------------------------------------------------------------------
              Remove the entry from the APN table
            ------------------------------------------------------------------------*/
            if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
            {
              DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
            }

#ifdef FEATURE_DATA_LTE
            /* Find an entry in the PDN Limit Table. If an entry is found,
               validate if the entry can be cleared at this point. If yes,
               clear the entry */
            if((index = ds_3gpp_pdn_limit_tbl_find_entry
                 ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
                   subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
            {
              ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
            }
#endif /*FEATURE_DATA_LTE*/

            ds_pdn_cntxt_freeInstance(pdn_cntx_p);
          }
        }
        else if (ps_errno == DS_EWOULDBLOCK)
        {
           pdn_cntx_p->ds_pdn_context_dyn_p->state = 
                                          DS_PDN_CONTEXT_STATE_GOING_DOWN;
        }
      }/* ps_phys_link_down_cmd */
    } /* (other_ds_iface_p == NULL)*/
    else
    {
      /*--------------------------------------------------------------------- 
         Physlink was not brought down since the other iface is either
         up (or) not completely down yet
       
         Also change the ip type of this PDN to the other ip type to
         block future bringups of this ip type
        ---------------------------------------------------------------------*/

      if(ps_iface_addr_family_is_v4(iface_p))
      {
        ds_pdn_cntxt_update_allowed_ip_type(pdn_cntx_p, DS_IPV6_ONLY);  
      }
      else
      {        
        ds_pdn_cntxt_update_allowed_ip_type(pdn_cntx_p, DS_IPV4_ONLY); 
      }

      /*--------------------------------------------------------------------- 
        Teardown the PS Iface
        ---------------------------------------------------------------------*/
      ds_3gpp_pdn_cntxt_tearDownDSIface(ds_iface_p, 
                                        PS_NET_DOWN_REASON_NOT_SPECIFIED);
    }/* else (other_ds_iface_p == NULL)*/

  }
  return;
  
} /* ds_3gpp_bring_down_physlink() */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DS_PDN_CNTX_IS_APM_REJECT_SM_THROTTLED

DESCRIPTION
  This function checks to see if apn_reject_sm for the particular PDN
  is blocked.
  However if it is a HO attach scenario, then no throttling will be performed.
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn_reject_sm is blocked
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_is_apn_reject_sm_throttled
(
  ds_pdn_context_s     *pdn_cntx_p  
)
{
  boolean ret_val = FALSE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id))
  {
    return ret_val; 
  }

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }

  if((pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag) ||
     (!ds_3gpp_pdn_throt_validate_apn_reject_name(subs_id)))
  {
    DS_3GPP_MSG1_HIGH("HO attach scenario:%d",pdn_cntx_p->
                      ds_pdn_context_dyn_p->handover_attach_flag);
    return ret_val;
  }

  /*---------------------------------------------------------------------- 
    if the apn match occurs and apn_reject_sm's pdn blocked is TRUE
    return TRUE
    ----------------------------------------------------------------------*/
  if((strcasecmp((char*)ds_3gpp_pdn_throt_apn_reject_sm[subs_id].apn,
     (char*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn) == 0) 
     &&(ds_3gpp_pdn_throt_apn_reject_sm[subs_id].block_pdn))
  {
    ret_val = TRUE;
  }

  DS_3GPP_MSG1_HIGH("The APN needs to be blocked:%d",ret_val);
  return ret_val;

}
/*===========================================================================
FUNCTION      DS_PDN_CNTX_REJECT_NULL_APN

DESCRIPTION
  This function checks to see if null apn is supported
  Null apn is supported only if the PDN is attach PDN
 
PARAMETERS

DEPENDENCIES

RETURN VALUE
  True -  If the apn in the pdn context is NULL
          and the PDN is not attach PDN
  False - Otherwise
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntx_reject_null_apn
(
  ds_pdn_context_s     *pdn_cntx_p  
)
{
  boolean                 ret_val = FALSE;
  sys_sys_mode_e_type     network_mode = SYS_SYS_MODE_NONE;
  ds3g_apn_name_type      apn_name;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return ret_val;
  }

  subs_id = pdn_cntx_p->ds_pdn_context_dyn_p->subs_id;

 /*------------------------------------------------------------------------- 
   Return False
   If the network mode is not LTE
   If attach SM is not Attached (This will take care of the attach PDN)
   if PDN is emergency PDN
  -------------------------------------------------------------------------*/
  network_mode = ds_3gpp_get_apn_current_mode(pdn_cntx_p);

  if ((network_mode != SYS_SYS_MODE_LTE) || 
      (ds_eps_attach_sm_get_state(subs_id) != DS_EPS_ATTACHED) ||
      DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p))
  {
    DS_3GPP_MSG0_HIGH("Do not reject null_apn");
    return ret_val;
  }

  /*------------------------------------------------------------------------ 
    If we have reached this stage, It is an additional PDN
    Check if the apn in the PDN context is NULL, If NULL apn return TRUE
   ------------------------------------------------------------------------*/
  (void)ds_3gpp_get_apn_from_pdn_context(pdn_cntx_p,&apn_name);
  
  if (apn_name.apn_string_len == 0)
  {
    ret_val = TRUE;
    DS_3GPP_MSG1_HIGH("NULL APN for pdn_cntx:0x%x: reject null apn",
                      pdn_cntx_p);
  }

  return ret_val;
}
#endif

/*===========================================================================
FUNCTION      DS_3GPP_PDN_CNTXT_VALIDATE_AUTH_PARAMS_AND_RESET_T3396_THROT

DESCRIPTION
  Check for username/password match - Call originated username/passwd is
  compared with pdn throttling config info, If they are not the same.
  throttling info is cleared.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.    
  info_ptr      - Call param info
  subs_id
 
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  void

SIDE EFFECT 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt
(
  ds_umts_pdp_profile_type        *pdp_profile,
  void                            *info_ptr,
  sys_modem_as_id_e_type           subs_id   
)
{
  ds_3gpp_pdn_throttle_sm_s       *pdn_throt_sm_ptr = NULL;
  boolean                          globally_blocking = FALSE;
  network_params_info_type        *call_info_p = NULL;
  ds_3gpp_throt_user_data         *throt_user_data_p_v4 = NULL;
  ds_3gpp_throt_user_data         *throt_user_data_p_v6 = NULL;
                                 
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    DS_3GPP_MSG1_ERROR("Invalid Subs Id %d",subs_id);
    return;
  }

  if(pdp_profile == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDP profile is NULL");
    return;
  }

  globally_blocking = 
                   !ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id);

  /*------------------------------------------------------------------------ 
    Get the pdn throt ptr for the apn and check if the throttle type is set
    to T3396_Absent. 
    If it is compare the originated auth_info user name and
    password with the throttled user name and password. 
    ------------------------------------------------------------------------*/

  pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(
                        pdp_profile->context.apn,
                        globally_blocking, FALSE,
                        DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, subs_id);

  if(pdn_throt_sm_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("PDN throt sm ptr is NULL");
    return;
  }

  if(pdn_throt_sm_ptr->sm_type == DS_3GPP_THROTTLE_TYPE_T3396_ABSENT)
  {
     call_info_p = ((network_params_info_type *)info_ptr);

     if(pdn_throt_sm_ptr->v4_throttle_sm != NULL)
     {
       throt_user_data_p_v4 = ds_3gppi_throt_sm_get_user_data
                                      (pdn_throt_sm_ptr->v4_throttle_sm);
     }

     if(pdn_throt_sm_ptr->v6_throttle_sm != NULL)
     {
       throt_user_data_p_v6 = ds_3gppi_throt_sm_get_user_data
                                       (pdn_throt_sm_ptr->v6_throttle_sm);
     }

/*---------------------------------------------------------------------------------- 
  If policy information is set, compare the policy auth info with the stored auth info
  If the username/password is not the same unset the throttling.
  If policy information is not set, use the profile information
  ----------------------------------------------------------------------------------*/
     if((call_info_p != NULL) &&
        ((call_info_p->valid_fields & NET_CFG_PRM_AUTH_PREF_MASK)||
         (call_info_p->valid_fields & NET_CFG_PRM_AUTH_USERNAME_MASK)||
         (call_info_p->valid_fields & NET_CFG_PRM_AUTH_PASSWORD_MASK)))
     {
       if((throt_user_data_p_v4 != NULL) &&(throt_user_data_p_v4->auth_info_p != NULL))
       {
         if((memcmp(throt_user_data_p_v4->auth_info_p->password,
                   call_info_p->auth_info.params.passwd_info,
                   PPP_MAX_PASSWD_LEN) != 0) ||
            (memcmp(throt_user_data_p_v4->auth_info_p->username,
                    call_info_p->auth_info.params.user_id_info,
                    PPP_MAX_USER_ID_LEN) != 0))
         {
           ds_3gpp_pdn_throt_unthrottle_and_clear_user_data(pdn_throt_sm_ptr,
                                                            throt_user_data_p_v4,
                                                            DS_UMTS_PDP_IPV4,
                                                            subs_id);
         }
       }

       if((throt_user_data_p_v6 != NULL) && (throt_user_data_p_v6->auth_info_p != NULL))
       {
         if((memcmp(throt_user_data_p_v6->auth_info_p->password,
                    call_info_p->auth_info.params.passwd_info,
                    PPP_MAX_PASSWD_LEN) != 0) ||
            (memcmp(throt_user_data_p_v6->auth_info_p->username,
                    call_info_p->auth_info.params.user_id_info,
                    PPP_MAX_USER_ID_LEN) != 0))
         {
           ds_3gpp_pdn_throt_unthrottle_and_clear_user_data(pdn_throt_sm_ptr,
                                                            throt_user_data_p_v6,
                                                            DS_UMTS_PDP_IPV6,
                                                            subs_id);
         }
       }
     }
     else
     {
       if((throt_user_data_p_v4 != NULL) && (throt_user_data_p_v4->auth_info_p != NULL))
       {
         if((memcmp(throt_user_data_p_v4->auth_info_p->password,
                    pdp_profile->auth.password,
                   DS_UMTS_MAX_QCPDP_STRING_LEN) != 0) ||
            (memcmp(throt_user_data_p_v4->auth_info_p->username,
                    pdp_profile->auth.username,
                    DS_UMTS_MAX_QCPDP_STRING_LEN) != 0))
         {
            ds_3gpp_pdn_throt_unthrottle_and_clear_user_data(pdn_throt_sm_ptr,
                                                             throt_user_data_p_v4,
                                                             DS_UMTS_PDP_IPV4,
                                                             subs_id);
         }
       }

       if((throt_user_data_p_v6 != NULL) && (throt_user_data_p_v6->auth_info_p != NULL))
       {
         if((memcmp(throt_user_data_p_v6->auth_info_p->password,
                    pdp_profile->auth.password,
                    DS_UMTS_MAX_QCPDP_STRING_LEN) != 0) ||
            (memcmp(throt_user_data_p_v6->auth_info_p->username,
                    pdp_profile->auth.username,
                    DS_UMTS_MAX_QCPDP_STRING_LEN) != 0))
         {
            ds_3gpp_pdn_throt_unthrottle_and_clear_user_data(pdn_throt_sm_ptr,
                                                             throt_user_data_p_v6,
                                                             DS_UMTS_PDP_IPV6,
                                                             subs_id);
         }
       }
     }
  }
}/* ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt */

/*===========================================================================
FUNCTION      DSPDNCNTX_VALIDATE_IFACE_UP_CMD

DESCRIPTION
  Validates state, family for this iface and the other iface(for Dual IP).   

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.    
  info_p        - Pointer to call information 
DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  pass/fail flag

SIDE EFFECT 
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_validate_iface_up_cmd
(
  ps_iface_type *iface_p,
  void          *info_p  
)
{
  ds_3gpp_iface_s                    *ds3gpp_iface_p = NULL;
  ds_pdn_context_s                   *pdn_cntx_p = NULL;  
  ds_pdn_context_s                   *pdn_cntx_awaiting_bearer_alloc_p = NULL;
  ps_iface_type                      *companion_iface_p = NULL;
  sys_sys_mode_e_type                 network_mode; 
  sys_srv_status_e_type               srv_status = SYS_SRV_STATUS_NO_SRV;
  ps_iface_net_down_reason_type       down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_apn_ip_support_type_e            ip_support = DS_IP_TYPE_INVALID;
  sys_srv_status_e_type               curr_srv_status;
  cm_srv_domain_pref_e_type           curr_oprt_mode_srv_domain;
  sys_srv_domain_e_type               curr_regd_srv_domain;
  boolean                             bring_down_call = FALSE;
  sys_sys_mode_e_type                 pref_pkt_mode = SYS_SYS_MODE_NONE;
  boolean                             pref_pkt_mode_is_3gpp = FALSE;
  ds_network_overrridden_ip_type_e    overridden_ip_type = PDP_UNDEFINED;
  boolean                             call_orig_before_ps_attach = TRUE;
  boolean                             is_rpm_enabled = FALSE;
  sys_modem_as_id_e_type              subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  ds_umts_pdp_profile_status_etype    result = DS3G_FAILURE;
  uint8                               handle = 0;
#endif /*FEATURE_DATA_LTE*/          
  uint8                               max_active_pdp_context = 0; 
  ds_bearer_context_s*                def_bearer_cntxt = NULL;  
  ps_iface_down_mh_event_info_type    down_info;               
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT (iface_p != NULL);
  
  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  /*-------------------------------------------------------------------------
    Make sure the iface state is down. Can bring the primary call up only
    for the iface which is in down state.
  -------------------------------------------------------------------------*/
  if (ps_iface_state(iface_p) == IFACE_UP )
  {
    DS_3GPP_MSG1_ERROR("Invalid iface state for Iface 0x%x",iface_p);
    ASSERT(0);
    return DS3G_FAILURE;
  }  

  /*-------------------------------------------------------------------------
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    PDN context pointer.
  -------------------------------------------------------------------------*/
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  ASSERT (ds3gpp_iface_p != NULL);

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    DS_3GPP_MSG2_ERROR("PDN Context is NULL for iface 0x%x for Subs id(CM) %d"
                       ,ds3gpp_iface_p,subs_id);  

    down_info.down_reason = PS_NET_DOWN_REASON_VALIDATION_FAILURE;
    ps_iface_down_ind(iface_p, &down_info);
    ds_3gpp_pdn_context_cleanupDSIface(ds3gpp_iface_p);
    return DS3G_FAILURE;
  } 

  /*----------------------------------------------------------------- 
    Typically the Subs Id on Iface would be set during Post Proc
    itself. However there can be race conditions because of which 
    the Subs Id on Iface gets reset. Hence we explicitly set the Subs id
    on the Iface here.
    -----------------------------------------------------------------*/
  PS_IFACE_SET_SUBS_ID(iface_p, ds3gsubsmgr_subs_id_cm_to_ds(subs_id));

  down_info.bearer_tech.rat_value = 
    ds_3gpp_map_sys_mode_to_ds_sys_rat
      (ds3gpp_get_current_network_mode(subs_id));
  down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);

  /*----------------------------------------------------------------- 
    Do not allow iface up command if there is any PDN
    waiting for bearer resources. 
    -----------------------------------------------------------------*/
  pdn_cntx_awaiting_bearer_alloc_p = 
            ds_3gpp_get_pdn_cntxt_awaiting_bearer_allocation(subs_id);
  if ((pdn_cntx_awaiting_bearer_alloc_p != NULL)    &&
      (pdn_cntx_awaiting_bearer_alloc_p != pdn_cntx_p))
  {
    DS_3GPP_MSG3_ERROR("PDN Context ptr:%x is not same as iface 0x%x "
                       "clientptr for Subs id(CM) %d",
                       pdn_cntx_p,ds3gpp_iface_p,subs_id);  

    down_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
    ps_iface_down_ind(iface_p, &down_info);
    ds_3gpp_pdn_context_cleanupDSIface(ds3gpp_iface_p);
    return DS3G_FAILURE;
  }

  if (pdn_cntx_p->ds_pdn_context_dyn_p->profile_is_valid == FALSE) 
  {
     DS_3GPP_MSG3_ERROR("profile id %d is not valid for iface 0x%x for Subs id(CM) %d",
                       pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num
                        ,ds3gpp_iface_p,subs_id); 
     down_reason = PS_NET_DOWN_REASON_INVALID_PROFILE_ID;
     ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
     return DS3G_FAILURE;
  }

  /*--------------------------------------------------------------------------- 
    Check if NULL APN is supported
   --------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  if(!ds_3gpp_nv_manager_get_null_apn_support_for_non_attach_pdn(subs_id) &&
      ds_pdn_cntx_reject_null_apn(pdn_cntx_p))
  { 
    DS_3GPP_MSG0_ERROR("REJECT PDN CNTXT - NULL APN not allowed");
    down_reason = PS_NET_DOWN_REASON_NULL_APN_DISALLOWED;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }
#endif  /* FEATURE_DATA_LTE */

  if(TRUE == ds3g_profile_validator_is_apn_forbidden(pdn_cntx_p, subs_id))
  {
    DS_3GPP_MSG0_ERROR("REJECT Call Bring-up - Forbidden APN !!!");
    down_reason = PS_NET_DOWN_REASON_INVALID_APN_NAME;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

  ds3gpp_enter_global_crit_section();
  if (ds_3gpp_roaming_get_status(subs_id) == TRUE)
  {
    if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.roaming_disallowed)
    {
      ds3gpp_leave_global_crit_section();
      DS_3GPP_MSG1_ERROR("Profile %d is not allowed while on roaming",
                        pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num);
      down_reason = PS_NET_DOWN_REASON_APN_DISALLOWED_ON_ROAMING;
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;
    }
  }
  ds3gpp_leave_global_crit_section();

  /*-------------------------------------------------------------------------
    Reject the Call if APN is disallow due to UI Settings
  -------------------------------------------------------------------------*/
  if(!ds3gpp_device_ev_hdlr_is_apn_allowed
       ((char*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
        subs_id, &down_reason))
  {
    DATA_3GPP_MSG_SPRINTF_1 
      (MSG_LEGACY_HIGH, "UI Data Disabled for APN %s", 
      pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn);

    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

  if(ds_3gppi_is_rel_11_supported())
  {
    /*-------------------------------------------------------------------------
       Check if maximum simultanoues pdp has reached before triggering 
       another call and bringing up iface 
    -------------------------------------------------------------------------*/
    max_active_pdp_context = ds_3gpp_pdn_get_max_simul_active_pdp_context(subs_id);
    if( ds_3gpp_pdn_get_current_active_context(subs_id) >= max_active_pdp_context)
    {
      down_reason = PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED;
      DS_3GPP_MSG1_ERROR("PS_NET_DOWN_REASON_MAX_ACTIVE_PDP_CONTEXT_REACHED,"
                         "Max PDP = (%d)",  
                          max_active_pdp_context);
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;
    }
  }

  /*--------------------------------------------------------------------- 
   Check if throttle type is T3396_ABSENT, IF the throttled username/Password
   is different from the originated call information, unthrottle the
   PDN throt sm
    ---------------------------------------------------------------------*/
  if(ds_3gpp_nv_manager_get_global_throttling(subs_id))
  {
    DS_3GPP_MSG0_HIGH("Global throttling enabled, Perform T3396 validation check");
    ds_3gpp_pdn_cntxt_validate_auth_params_and_reset_t3396_throt(
                            &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile),
                            info_p,
                            pdn_cntx_p->ds_pdn_context_dyn_p->subs_id);
  }
  /*----------------------------------------------------------------------
    Perform throttling check.
      - only for the first iface out of a dual IP call (the iface which
        application requested, in which case default bearer will be null)
      - if apn is throttled, cleanup pdn and return failure
        if Release 11 is enabled then, check if any of the EFacc classes are
        enabled.
        Criteria: Even if APN is throttled and Efacc is set, We send PDP 
                  context request towards the network.
  ----------------------------------------------------------------------*/
     if( (pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == NULL && 
	  ds_3gpp_pdn_cntxt_is_apn_throttled(pdn_cntx_p, iface_p)) &&
	  !(ds_pdn_cntxt_get_throttle_override(subs_id, 
	                pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
	                iface_p))
       )
       {
         ds_pdn_cntxt_set_throt_down_reason(iface_p, &down_reason);
#ifdef FEATURE_DATA_RPM
	  if (ds_3gpp_is_rpm_enabled(subs_id))
          {
            /*----------------------------------------------------------------------------- 
              Ensuring that if CPDP counter is already incremented once as part of
              V4 iface validation, its not incremented again during V6 iface validation
              or vice versa.
            ------------------------------------------------------------------------------*/
            companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_p);
            if ( !(PS_IFACE_IS_VALID(companion_iface_p) &&
                   (pdn_cntx_p->ds_pdn_context_dyn_p->net_down_reason == PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED ||
                    pdn_cntx_p->ds_pdn_context_dyn_p->net_down_reason == PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED)) )
            {
              DS_3GPP_MSG0_HIGH("ds_3gpp_pdn_cntxt_validate_apn_throttling:"
                                "APN is throttled, increment CPDP counter");
              ds_3gpp_rpm_inc_cpdp(pdn_cntx_p->ds_pdn_context_dyn_p->
                                   pdp_profile.context.apn,
                                   subs_id);
             }
             pdn_cntx_p->ds_pdn_context_dyn_p->net_down_reason = down_reason;
           }
#endif /* FEATURE_DATA_RPM */
	  ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
	  return DS3G_FAILURE;	
	}
  /*---------------------------------------------------------------------------
  Attach PDN must have the is_default flag set to TRUE. So decide on that now
  ---------------------------------------------------------------------------*/
  if(ds_pdn_cntxt_check_and_set_pdn_is_default(pdn_cntx_p) == DS3G_FAILURE)
  { 
    DS_3GPP_MSG0_ERROR("attach PDN is default falg is not set ");
    down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

#ifdef FEATURE_EMERGENCY_PDN
  if ((ds_3gpp_pdn_cntxt_get_emergency_auth_fail()== TRUE) && 
      (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == FALSE )) 
  {
    DS_3GPP_MSG0_HIGH("Emergency auth failure , do not allow further call");
    down_reason = PS_NET_DOWN_REASON_AUTH_FAILURE_ON_EMERGENCY_CALL;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }
#endif /*FEATURE_EMERGENCY_PDN*/

#ifdef FEATURE_DATA_DISABLE_MULTI_PDN  
  if (ds_pdn_cntxt_multiple_pdp_allowed(iface_p) == DS3G_FAILURE )
  {
    DS_3GPP_MSG0_ERROR("mutli PDN not allowed ");
    down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }
#endif

  if ( (ps_iface_get_addr_family(iface_p) != IFACE_IPV4_ADDR_FAMILY) &&
       (ds3gpp_iface_p == pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE])    
     )
  {
    DATA_ERR_FATAL("Trying to bring up V4 index iface with V6 family");
    return DS3G_FAILURE;
  }
  
#ifdef FEATURE_DATA_PS_IPV6
  if ( (ps_iface_get_addr_family(iface_p) != IFACE_IPV6_ADDR_FAMILY) &&
       (ds3gpp_iface_p == pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE])    
     )
  {
    DATA_ERR_FATAL("Trying to bring up V6 index iface with V4 family");
    return DS3G_FAILURE;
  }

  companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_p);

#endif /* FEATURE_DATA_PS_IPV6 */

  // Clean up other iface if this is v4v6 call, free this iface as well
  if (ps_iface_state(iface_p) != IFACE_COMING_UP )
  {
    DS_3GPP_MSG1_ERROR("Invalid iface state for Iface 0x%x",iface_p);    
    down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

#ifdef FEATURE_DATA_PS_IPV6
      /*---------------------------------------------------------------------
        If the iface is in going_down state, active_bring_up will return 
        ENETCLOSEINPROGRESS and MH will not get IFACE_UP_CMD
        This will result in flow not getting bound to phys_link for the other
        iface
      ---------------------------------------------------------------------*/
  if (companion_iface_p != NULL)
  {        
    if(ps_iface_state(companion_iface_p) == IFACE_GOING_DOWN)
    {
      DS_3GPP_MSG2_ERROR("Companion Iface %x:%x is in going_down state. Bring up Failed. ",
                         companion_iface_p->name,companion_iface_p->instance);          
      down_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
      ps_iface_down_ind(iface_p, &down_info);
      return DS3G_FAILURE;
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  network_mode = ds_3gpp_get_apn_current_mode(pdn_cntx_p);
  /*--------------------------------------------------------------------------- 
    Check if network has overridden the IP type.
    If a call orig IP type is different from the overridden ip type then
    bring down the call
  ---------------------------------------------------------------------------*/
  overridden_ip_type = 
    ds_3gpp_network_override_hdlr_get_overridden_ip
     (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, subs_id);

  if (overridden_ip_type != PDP_UNDEFINED) 
  {
     /* When the Call being originated is not allowed due to Network Override
       Handler , there can two possibility and three scenario
       Case 1:Attach Scenario : Assume Network Override Handler is V6 
              Step 1: UE tries to Attach V4V6,
              Step 2: V4 Iface should be put down and PDN should not be cleaned
              up unless the profile is IPV4 only
       Case 2:Bring up only IPV4 profile IPV4V6 ,Assume Network Override Handler is V6 
              Step 1: UE tries to bring up IPV4 Iface first
              Step 2: Since client wants only IPV4 Iface , this PDN should be
                      cleaned up
       Case 3:Bring up IPV4 call , given IPV6 is already up ,
              Assume Network Override Handler is V6
              Step 1: UE tries to bring up IPV4 Iface only
              Step 2: Since IPV4 IFACE is not allowed , IPV4 should be brought down
       Note: This scenario should be caught at routing
       Case 4: Bring up IPV4,IPV6 back to back,,Assume Network Override Handler is V6
              Step 1: UE tries to bring up IPV4 IFace first
              Step 2: This PDN should not be cleaned because IPV6 will use the
              same PDN to initiate the call
       Attach 
      |V4 (in use flag)	|V6 (in use flag)| Profile | NW override | Action 
      |      0          |     0          |  V4V6...|.....V4......| Clean IFACE(V4)
      |      0          |     -          |  V4  ...|.....V6......| Clean PDN
      |      -          |     0          |  V6  ...|.....V4......| Clean PDN
      |      0          |     -          |  V4.....|.....V4......| Allow call
      |      -          |     0          |  V6.....|.....V4......| Allow call
      
      
      Routing
      |V4 (in use flag)	|V6 (in use flag)| Profile | NW override | Action 
      |      1          |     1          |  V4V6...|.....V4......| Clean IFACE(V4)
      |      1          |     0          |  V4V6...|.....V6......| Clean PDN
      |      0          |     1          |  V4V6...|.....V4......| Clean PDN
      |      1          |     -          |  V4  ...|.....V6......| Clean PDN
      |      -          |     1          |  V6  ...|.....V4......| Clean PDN
      |      1          |     -          |  V4  ...|.....V4......| Allow call
      |      -          |     1          |  V6  ...|.....V6......| Allow call
    */
    if(((ps_iface_get_addr_family(iface_p) == IFACE_IPV4_ADDR_FAMILY) &&
       (overridden_ip_type != PDP_IPV4)) 
       ||
      ((ps_iface_get_addr_family(iface_p) == IFACE_IPV6_ADDR_FAMILY) &&
       (overridden_ip_type != PDP_IPV6)))
    {
      DS_3GPP_MSG0_ERROR("Iface IP type and network overridden IP type"
                         "don't match");

      if (overridden_ip_type == PDP_IPV4)
      {
        down_reason = PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED;
      }
      else
      {
        down_reason = PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED;
      }	  

      down_info.down_reason = down_reason;

      DS_3GPP_MSG1_ERROR("overridden_ip_type: %d only allowed ", overridden_ip_type);
      if((companion_iface_p != NULL ) && 
         (PS_IFACE_IS_IN_USE(companion_iface_p) == PS_IFACE_IS_IN_USE(iface_p)))
      {
        ps_iface_down_ind(iface_p, &down_info);
        ds_3gpp_pdn_context_cleanupDSIface(ds3gpp_iface_p);
      }
      else
      {
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      }
      return DS3G_FAILURE; 
    }
    /*--------------------------------------------------------------------- 
      Do not send out a PDN connectivity request if the iface is not in use.
      An example could be
      Profile type is IPV4V6, Ntw overrides the IP type as V6 only
      Now V4 iface bring up will fail, As part of the companion iface bring up
      we should not allow V6 iface to be up because it does not have a client.
      But for LTE attach we should allow attach request with V6(network 
      allowed PDN type) even iface not in use.
      ----------------------------------------------------------------------*/
    else if (PS_IFACE_IS_IN_USE(iface_p) == FALSE)
    {
       if( (network_mode != SYS_SYS_MODE_LTE) ||
           ( (network_mode == SYS_SYS_MODE_LTE) 
#ifdef FEATURE_DATA_LTE
         &&(ds_eps_attach_sm_get_state(subs_id) == DS_EPS_ATTACHED) 
#endif /* FEATURE_DATA_LTE */
           )  )
      {
        DS_3GPP_MSG3_ERROR("Iface is not in use, bringing down the iface",0,0,0);
        down_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ps_iface_down_ind(iface_p, &down_info);
        return DS3G_FAILURE;
      }
    }
  }
   
  network_mode = ds_3gpp_get_apn_current_mode(pdn_cntx_p);
   
  srv_status = ds3g_get_current_srv_status_ex(subs_id);
  DS_3GPP_MSG3_HIGH("Service status (%d), network mode (%d) ",
                     srv_status,network_mode,0);

  /*------------------------------------------------------------------------- 
    Fail the call if we are not in one of 3GPP modes
   -------------------------------------------------------------------------*/
  if(network_mode == SYS_SYS_MODE_WCDMA)
  {
    pref_pkt_mode = SYS_SYS_MODE_WCDMA;
    pref_pkt_mode_is_3gpp = TRUE;
  }
  else if(network_mode == SYS_SYS_MODE_GSM)
  {
    pref_pkt_mode = SYS_SYS_MODE_GSM;
    pref_pkt_mode_is_3gpp = TRUE;
  }
  else if(network_mode == SYS_SYS_MODE_LTE)
  {
    pref_pkt_mode = SYS_SYS_MODE_LTE;
    pref_pkt_mode_is_3gpp = TRUE;
  }
#ifdef FEATURE_TDSCDMA
  else if (network_mode == SYS_SYS_MODE_TDS)
  {
    pref_pkt_mode = SYS_SYS_MODE_TDS;
    pref_pkt_mode_is_3gpp = TRUE;
  }
#endif /*FEATURE_TDSCDMA*/
  if (pref_pkt_mode_is_3gpp == FALSE)
   {
    down_reason = PS_NET_DOWN_REASON_UNPREFERRED_RAT;
    DS_3GPP_MSG0_ERROR("Preferred packet mode is not W/G/L !");    
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
    //clean up this and other iface (if V4V6) and free PDN and clean up APN table
   }

  /*-------------------------------------------------------------------------
    Fail the call if the profile cannot be used with the current preferred pkt
    mode
   -------------------------------------------------------------------------*/
  if(
#ifdef FEATURE_TDSCDMA 
     (pref_pkt_mode == SYS_SYS_MODE_TDS
     && ((pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.apn_bearer 
          & DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_T) == 0) )
     ||
#endif /*FEATURE_TDSCDMA*/
    (pref_pkt_mode == SYS_SYS_MODE_WCDMA
     && ((pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.apn_bearer 
          & DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_W) == 0))
     || (pref_pkt_mode == SYS_SYS_MODE_GSM
     && ((pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.apn_bearer 
          & DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_G) == 0))
     || (pref_pkt_mode == SYS_SYS_MODE_LTE
     && ((pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.apn_bearer 
          & DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_L) == 0))
    )
  {
    DS_3GPP_MSG2_ERROR("ds_pdn_cntxt_validate_iface_up_cmd: profile %d cannot "
                       "be used in %d mode", pdn_cntx_p->ds_pdn_context_dyn_p
                       ->pdp_profile_num, 
                       (int)pref_pkt_mode);
    down_reason = PS_NET_DOWN_REASON_PROFILE_BEARER_INCOMPATIBLE;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Fail PPP calls on LTE
   -------------------------------------------------------------------------*/
   if ((network_mode == SYS_SYS_MODE_LTE)
       && pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
       context.pdp_type == DS_UMTS_PDP_PPP)
   {
     DS_3GPP_MSG0_ERROR("ds_pdn_cntxt_validate_iface_up_cmd: PPP calls"
                        "not supported over LTE mode");
     down_reason = PS_NET_DOWN_REASON_PDP_PPP_NOT_SUPPORTED;
     ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
     return DS3G_FAILURE;
   }

  /*-------------------------------------------------------------------------
    Allow call origination even if data_suspend is TRUE
    In this scenario, CM will first send data_available_req to NAS while
    holding onto the call orig req.
    For interRAT with same RA id, NAS will not start RAU procedure until
    there is a trigger from DS/CM. Without RAU, data will not resume.
    The call orig request would force RAU in this scenario.
  -------------------------------------------------------------------------*/
  if(ds3gpp_get_ps_data_suspend_flag(subs_id) == TRUE)
  {    
    
    DS_3GPP_MSG3_HIGH("data in suspend mode, attach %d, bringing up iface 0x%x:0x%x ", 
                      ds3gpp_get_is_attach_initiated_flag(subs_id),
                      iface_p->name,
                      iface_p->instance
                      );
  }

  /*------------------------------------------------------------------------
    We skip PDN Connection Limit Handling if this is initial attach or
    handover attach.
    We will first check if there is entry corresponding to the APN in the
    PDN Limit Table. If there is one, we get an index to that entry.
    If not, we create a new entry in the PDN Limit Table with the given
    parameters.
    Next we check if iface bringup is allowed for the given index.
    If yes, we continue. Otherwise we set the throttle down reason and return
    failure to the application.
   
   ------------------------------------------------------------------------*/
#ifdef  FEATURE_DATA_LTE
  if (ds_3gpp_pdn_limit_is_enabled(subs_id))
  {
    if (pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag == TRUE || 
        ds_eps_attach_sm_is_ue_attached(subs_id) == FALSE )
     {
      DS_3GPP_MSG0_HIGH("Initial or handover attach is in progress. "
                        "PDN Limit Handling skipped");
    }
    else
    {
      result = ds_3gpp_pdn_cntxt_validate_and_update_pdn_limit_entry(
                             pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num , 
                             &handle,
                             pdn_cntx_p->ds_pdn_context_dyn_p->subs_id);

      if((result == DS3G_FAILURE) ||
         ((handle != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY) && 
          (ds_3gpp_pdn_limit_call_bringup_is_allowed(handle, subs_id) == FALSE))) 
      {
        ds_pdn_cntxt_set_throt_down_reason(iface_p, &down_reason);
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);	
        DS_3GPP_MSG1_ERROR("PDN limit not allowed,but handle is %d ",handle);
        return DS3G_FAILURE ;
      }
    }
  }

/*------------------------------------------------------------------------- 
  APN reject throttle SM check
  -------------------------------------------------------------------------*/
  if (ds_pdn_cntxt_is_apn_reject_sm_throttled(pdn_cntx_p))
  {
    DS_3GPP_MSG0_ERROR("APN reject throttle SM is set");
    ds_pdn_cntxt_set_throt_down_reason(iface_p, &down_reason);
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }
#endif /*FEATURE_DATA_LTE*/

#ifdef FEATURE_DATA_RPM
  is_rpm_enabled = ds_3gpp_is_rpm_enabled(subs_id);
#endif /* FEATURE_DATA_RPM */

  call_orig_before_ps_attach = ds_3gpp_nv_manager_get_call_orig_before_ps_attach(subs_id);
          	 
            	 
  if ( ( is_rpm_enabled == TRUE || call_orig_before_ps_attach == FALSE ) 
#ifdef FEATURE_DATA_LTE
      && (ds3gpp_get_current_network_mode(subs_id) != SYS_SYS_MODE_LTE)
#endif /* FEATURE_DATA_LTE */
     )
  {
    /*-------------------------------------------------------------------------
      Allow call origination when srv_status is FULL_SERVICE 
      in the following scnearios:
      1. UE oprt_mode CS+PS and regd_srv_domain CS_PS or PS_Only.
      2. UE oprt_mode PS_only and regd_srv_domain PS_Only.
      3. UE oprt_mode CS_Only.
      This is applicable only for G/W/T. For LTE, when attach procedure is
      initiated, the srv_status will be lim_srv.
    -------------------------------------------------------------------------*/
    curr_srv_status = ds3g_get_current_srv_status_ex(subs_id);
    curr_oprt_mode_srv_domain = 
                      ds3g_get_current_oprt_mode_srv_domain_ex(subs_id);
    curr_regd_srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);

    DS_3GPP_MSG3_HIGH("curr_srv_status: %d, curr_oprt_mode_srv_domain: %d, curr_regd_srv_domain: %d",
                       curr_srv_status, curr_oprt_mode_srv_domain, curr_regd_srv_domain);
  
    if (SYS_SRV_STATUS_SRV == curr_srv_status)
    {
      if (curr_oprt_mode_srv_domain == CM_SRV_DOMAIN_PREF_CS_PS)
      {
        if ((curr_regd_srv_domain != SYS_SRV_DOMAIN_CS_PS) &&
            (curr_regd_srv_domain != SYS_SRV_DOMAIN_PS_ONLY))
        {
          DS_3GPP_MSG2_HIGH("oprt_mode_srv_domain: %d, regd_srv_domain: %d "
                            "bringing down call",
                            curr_oprt_mode_srv_domain, curr_regd_srv_domain);
          bring_down_call = TRUE; 
        }
      }
      else if (curr_oprt_mode_srv_domain == CM_SRV_DOMAIN_PREF_PS_ONLY)
      {
        if (curr_regd_srv_domain != SYS_SRV_DOMAIN_PS_ONLY)
        {
          DS_3GPP_MSG2_HIGH("oprt_mode_srv_domain: %d, regd_srv_domain: %d "
                            "bringing down call",
                            curr_oprt_mode_srv_domain, curr_regd_srv_domain);
          bring_down_call = TRUE; 
        }
      }
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Current srv_status : %d is not full-serv, bringing down call",
                         curr_srv_status);
      bring_down_call = TRUE; 
    }

    if (TRUE == bring_down_call)
    {    
      def_bearer_cntxt = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
      if (ds_bearer_cntx_validate_bearer_context(def_bearer_cntxt))
      {
        DS_3GPP_MSG0_HIGH("Request is sent OTA for companion iface bringup, Not bringing down");
        return DS3G_SUCCESS;
      }

      DS_3GPP_MSG0_ERROR("bring_down_call: is true ");
      ds_pdn_cntxt_set_throt_down_reason(iface_p, &down_reason);
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;  
    }
  }

  /*-------------------------------------------------------------------------
    Allow Iface to be brought up if the attach_initiated_flag is true
    or if we have full service
  -------------------------------------------------------------------------*/
  if( (ds_3gpp_is_context_pref_pkt_mode(pdn_cntx_p,SYS_SYS_MODE_LTE) == TRUE) &&
      (ds3gpp_get_is_attach_initiated_flag(subs_id) == FALSE) && 
      (ds3g_get_current_srv_status_ex(subs_id) != SYS_SRV_STATUS_SRV) )
  {
    /*--------------------------------------------------------------------------
      We have entered a case where the attach init flag is FALSE in LTE and we
      do not have full service. We could be in a case where we are processing
      the iface up command on the second of the two ifaces in a dual IP bearer
      PDN context

      In such a case, we have to see if the PDN type is V4V6 and if the PDN
      has been flagged for Iface bring up
    --------------------------------------------------------------------------*/
    if((pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type == DS_UMTS_PDP_IPV4V6) &&
       (pdn_cntx_p->ds_pdn_context_dyn_p->allow_iface_bring_up == TRUE))
    {
      DS_3GPP_MSG0_HIGH("Bringing up the second of the 2 ifaces of PDN cntx" 
                        " - Attach");
    }
    else
    {
      def_bearer_cntxt = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
      if (ds_bearer_cntx_validate_bearer_context(def_bearer_cntxt))
      {
        DS_3GPP_MSG0_HIGH("OTA request sent for companion iface bringup,"
                          " Not bringing down iface");
        return DS3G_SUCCESS;
      }
      DS_3GPP_MSG0_ERROR("Attach not init or Full service not available." ""
                        "Post Iface Down");
      down_reason = PS_NET_DOWN_REASON_NO_SRV;
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;
    }
  }

  /*----------------------------------------------------------------------------
    This check has been added to address the following scenario:
    1. Attach req sent OTA with NULL APN
    2. Interface selection picks a different iface for APN "ABC"
    3. ACT_DEF_BEARER_CONTEXT processing updates resolved APN name as ABC
    4. Post proc tries to add new PDN in the same entry as attach PDN. This is
       failed.
    5. Deny iface bring up if the PDN being brought up is not present in the 
       APN table
  ----------------------------------------------------------------------------*/
  if(ds_pdn_cntxt_get_ip_support_info_for_pdn(pdn_cntx_p, &ip_support) == FALSE)
  {
    DS_3GPP_MSG1_ERROR("PDN: 0x%x not present in APN table. Deny iface bring up",
		      pdn_cntx_p);
    down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }

#if defined (FEATURE_EPC_HANDOFF)

  if (ds_pdn_cntxt_is_default_iface(iface_p) && network_mode == SYS_SYS_MODE_LTE)
  {
    if (ds_3gpp_pdn_cntxt_is_handover_attach_ex_f != NULL)
    {
      boolean is_handover_attach = 
       ds_3gpp_pdn_cntxt_is_handover_attach_ex_f
         ((uint16)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num, 
          (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id));

      ds_pdn_cntxt_set_handover_attach_flag(pdn_cntx_p, is_handover_attach);
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      Invoke the function that EPC mode handler registered with LTE MH
      to query EPC to see if iface bring up is allowed, if not we should clean up
    -------------------------------------------------------------------------*/
    if(ds_3gpp_pdn_cntxt_call_allowed_hdlr_ex_f != NULL)
    {
      if(!ds_3gpp_pdn_cntxt_call_allowed_hdlr_ex_f
           ((uint16)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num, 
            (ps_sys_subscription_enum_type)ds3gsubsmgr_subs_id_cm_to_ds(subs_id)))
      {
        DS_3GPP_MSG2_HIGH("EPC does not allow call bring up pf = %d, "
                          "Subs id(CM): %d", 
                           pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num,
                           subs_id);
        down_reason = PS_NET_DOWN_REASON_APN_PENDING_HANDOVER;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return DS3G_FAILURE;
      }
    }
  }
#endif /* FEATURE_EPC_HANDOFF */

#ifdef FEATURE_DATA_LTE
  /*-------------------------------------------------------------------------
    We should ensure that we are allowing only the default iface to be 
    brought up when the attach_init_flag is true. However the only exception
    to this will be during a handover attach
  -------------------------------------------------------------------------*/
  if(ds3gpp_get_is_attach_initiated_flag(subs_id) == TRUE)
  {
    if(
       (ds_pdn_cntxt_is_default_iface(iface_p) != TRUE) &&
       (pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag == FALSE)
      )
    {
      DS_3GPP_MSG0_ERROR("Trying to bring up regular non def PDN conn before attach");
      down_reason = PS_NET_DOWN_REASON_NO_SRV;
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;
    }

    if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      /*----------------------------------------------------------------------
        Mark in the pdn context that we should allow the next iface bring up
        This is because we are gonna reset the is_attach_init_flag and there
        is no way we can figure out if we can allow the other iface in this 
        PDN context to be brought up

        This is applicable only to Dual IP bearer PDN contexts
      ----------------------------------------------------------------------*/
      pdn_cntx_p->ds_pdn_context_dyn_p->allow_iface_bring_up = TRUE;
    }

    /*------------------------------------------------------------------------
      Reset the is_attach_initiated_flag
    ------------------------------------------------------------------------*/
    ds3gpp_set_is_attach_initiated_flag(FALSE, subs_id);
  }
  else
  {
    /*--------------------------------------------------------------------------
      In LTE mode, if UE is not yet attached, we do not allow any non-default 
      iface to come up
    --------------------------------------------------------------------------*/
    if ( (ds_3gpp_is_context_pref_pkt_mode(pdn_cntx_p,SYS_SYS_MODE_LTE) == TRUE) &&
         (ds_eps_attach_sm_is_ue_attached(subs_id) == FALSE) &&
         (pdn_cntx_p->ds_pdn_context_dyn_p->allow_iface_bring_up == FALSE) )
    {
      DS_3GPP_MSG2_ERROR("Iface up is not allowed before UE completes attach: 0x%x:%d",
                        iface_p->name,iface_p->instance);
      down_reason = PS_NET_DOWN_REASON_NO_SRV;                  
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return DS3G_FAILURE;
    }
  }

  /*--------------------------------------------------------------------------
    If the APN disable flag is set in profile, we should not attempt to bring
    up the Iface.
    Post Iface down and return.
  --------------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.apn_disable_flag == TRUE)
  {
    DS_3GPP_MSG0_ERROR("APN Disabled. Post Iface Down");
    down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
    ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
    return DS3G_FAILURE;
  }
#endif /* FEATURE_DATA_LTE */

  return DS3G_SUCCESS;

} /* ds_pdn_cntxt_validate_iface_up_cmd */

/*===========================================================================
FUNCTION      DS_PDN_CNTXT_GET_THROTTLE_OVERRIDE

DESCRIPTION
  This function decides whether throttling can be overridden for this APN.
  This is a REL-11 Spec CR 2157 (C1-122386) which mandates that UEs of 
  Access Class 11-15 should be allowed to send PDP Context/PDN Context
  request to network even though APN is throttled as per 
  T3396 throttling requirements.

PARAMETERS
  subs_id       - Subscription id in the current PDN context.
  apn           - The APN which is throttled.
  iface_p       - Pointer to the iface that is to be brought up.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - If throttling needs to be overridden.
  FALSE - If throttling need not be overridden.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_get_throttle_override
(
  sys_modem_as_id_e_type  subs_id,
  byte                    *apn,
  ps_iface_type           *iface_p
)
{
  boolean throttling_override = FALSE;
#ifdef FEATURE_DATA_REL11
  ps_iface_net_down_reason_type ps_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_3gpp_pdn_throttle_sm_s *pdn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_type     sm_type    = DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED;
  boolean globally_blocking = FALSE;
  globally_blocking = 
                   !ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id);  


  if ( (ds_3gpp_nv_manager_get_nv_umts_rel_version() >= NV_NAS_COMPLIANCE_REL11) ||
#ifdef FEATURE_DATA_LTE
       (ds_3gpp_nv_manager_get_nv_lte_rel_version(subs_id) >= LTE_3GPP_REL11)
#endif /* FEATURE_DATA_LTE */ 
     )
  {
    pdn_throt_sm_ptr = ds_3gpp_pdn_throt_sm_get_throttle_sm(
						 apn, globally_blocking, FALSE,
						 DS_3GPP_THROTTLE_TYPE_NOT_SPECIFIED, subs_id);
  
    if (NULL != pdn_throt_sm_ptr)
    {
      sm_type = pdn_throt_sm_ptr->sm_type;
      if (ps_iface_get_addr_family(iface_p) == IFACE_IPV4_ADDR_FAMILY)
      {
        throt_user_data = ds_3gppi_throt_sm_get_user_data
                                          (pdn_throt_sm_ptr->v4_throttle_sm);
        if(throt_user_data != NULL)
        {
        ps_down_reason = (ps_iface_net_down_reason_type)
                                    throt_user_data->cause_code;
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Throt user data is NULL");
        }  
      }
      else if (ps_iface_get_addr_family(iface_p) == IFACE_IPV6_ADDR_FAMILY)
      {
        throt_user_data = ds_3gppi_throt_sm_get_user_data
                                          (pdn_throt_sm_ptr->v6_throttle_sm);
        if(throt_user_data != NULL)
        {
        ps_down_reason = (ps_iface_net_down_reason_type)
                                    throt_user_data->cause_code;
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Throt user data is NULL");
        }
      }
    }
  
    /* Override throttling if UE is Access Class11-15 only if throttling is due to T3396 */
    if ( ((sm_type == DS_3GPP_THROTTLE_TYPE_T3396) || 
          (sm_type == DS_3GPP_THROTTLE_TYPE_T3396_ABSENT))&&
         ( (ps_down_reason == PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES) ||
	   (ps_down_reason == PS_NET_DOWN_REASON_UNKNOWN_APN) )
       )
    {
      if (ds3g_mmgsdi_is_efacc_class_set(subs_id))
      {
        throttling_override = TRUE;
      }
    }
  }
#endif /* FEATURE_DATA_REL11 */
  DS_3GPP_MSG1_HIGH("Returning throttling override: %d", throttling_override);
  return throttling_override;
} /* ds_pdn_cntxt_get_throttle_override */

/*===========================================================================
FUNCTION      DSPDNCNTX_IFACE_UP_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Brings up the iface requested after allocating a PDN context and a bearer
  context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.
  info_ptr      - Pointer to data passed in from the app.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  If MT table has matching call info as current call,
  this will clear the table entry.
===========================================================================*/
static void ds_pdn_cntxt_ifaceUpCmdHdlr
(
  ps_iface_type *iface_p,
  void          *info_ptr
)
{
  int16                         ps_errno;  /* Errno value   */
  int                           ret_val;
  ps_phys_link_type             *phys_link_ptr; /* Ptr to the phys link of call*/
  ds_bearer_context_s           *bearer_context_p;
  ds_pdn_context_s              *pdn_cntx_p;
  ds_3gpp_iface_s               *ds3gpp_iface_p;  
  sys_sys_mode_e_type           network_mode; 
  ps_iface_type                 *companion_iface_p = NULL;
  ds_bearer_flow_manager_s      *bflow_manager_p = NULL;  
  ps_iface_net_down_reason_type down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_TE_MT_PDP
  cm_call_id_type              call_id = CM_CALL_ID_INVALID;
#endif /* FEATURE_DATA_TE_MT_PDP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(ds_pdn_cntxt_validate_iface_up_cmd(iface_p,info_ptr) == DS3G_FAILURE)
  {
    DS_3GPP_MSG1_ERROR("Validation for up cmd failed for Iface 0x%x",
                       iface_p);
    return;
  }
  /*-------------------------------------------------------------------------
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    PDN context pointer.
  -------------------------------------------------------------------------*/
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  
  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);  

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }
   /*------------------------------------------------------------------------- 
    Save info_ptr in pdn_context. It may be required later for redialing the
    call.
   -------------------------------------------------------------------------*/
   if(info_ptr == NULL)
   {
     pdn_cntx_p->ds_pdn_context_dyn_p->info_null = TRUE;     
   }
   else
   {
     pdn_cntx_p->ds_pdn_context_dyn_p->info_null = FALSE;
     pdn_cntx_p->ds_pdn_context_dyn_p->info = *((network_params_info_type *)info_ptr);     
   }
#ifdef FEATURE_DUAL_SIM
       network_mode = ds3g_get_current_network_mode();
#else
       network_mode = ds_3gpp_get_apn_current_mode(pdn_cntx_p);
#endif /* FEATURE_DUAL_SIM */

#ifdef FEATURE_DATA_LTE
   if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE)&&
      (network_mode != SYS_SYS_MODE_LTE))
   {   	  
     ds_eps_pdn_throt_clear_t3402_timer(subs_id);
     
     /* Stop PLMN unblock timer as it might have been started after a successful 
        PDP Context Activation */
     if (!ds_3gpp_pdn_throt_is_any_apn_throttled_for_max_failure(subs_id))
     {
       ds_eps_pdn_throt_sm_stop_plmn_unblock_tmr(subs_id);
     }
   }
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_PS_IPV6

  companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_p);

#endif /* FEATURE_DATA_PS_IPV6 .*/

  if (ds_3gpp_hdlr_nv_refresh_in_progress(subs_id))
  {
     down_reason = PS_NET_DOWN_REASON_NV_REFRESH_IN_PROGRESS;
     ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
     return;
  }


  /*---------------------------------------------------------------------------
    Check to see if the default bearer is already available. It is possible 
    that we are acting on the Iface UP command for a second Iface attached to 
    the same PDN context. In that case, there is no need to allocate a bearer.
  ---------------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == NULL)
  {
    /* check if it is a MT call, if then bearer context will be 
       already allocated 
    */

    if( pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_ANSWER )
    {
      /*-----------------------------------------------------------------------
        Allocate a bearer context from the free pool of bearers. If no bearer 
        is available, we should fail the call.
      -----------------------------------------------------------------------*/
      bearer_context_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

      if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
      {
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }

      if(ds_pdn_cntxt_store_bearer_context(pdn_cntx_p,(void *)bearer_context_p) != \
                                                  DS3G_SUCCESS)
      {
        /*---------------------------------------------------------------------
          We should not hit this case of running out of placeholders for bearer
          during iface bring up. If this were to happen, it is probably because
          the bearer pointers were not cleaned up from a prior use of the PDN
          context.
        ---------------------------------------------------------------------*/
        //Post iface down
        DS_3GPP_MSG0_HIGH("Could not store the allocated bearer");

        //Deallocate bearer
        ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }
      
#ifdef FEATURE_DATA_TE_MT_PDP
      /* Check if call orig is due to a MT request */
      DS_3GPP_MSG0_HIGH("Debug MT PDP: chec call orig type ");
      if ( (call_id = dsumtsps_mt_check_call_orig(pdn_cntx_p)) 
          != CM_CALL_ID_INVALID )
      {
        DS_3GPP_MSG2_HIGH("MT call orig, pdn_cntxt: %x, cm_call_id: %d",
                           pdn_cntx_p, call_id);
        pdn_cntx_p->ds_pdn_context_dyn_p->state  = DS_PDN_CONTEXT_STATE_ANSWER; /* I think not required */
        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = call_id;
        bearer_context_p->ds_bearer_context_dyn_p->state = DS_BEARER_CONTEXT_STATE_ANSWER;
      }
#endif /* FEATURE_DATA_TE_MT_PDP */
    }
    else
    {
      bearer_context_p = pdn_cntx_p->ds_pdn_context_dyn_p->
        bearer_context_ptr_tbl[0];
      if((ds_bearer_cntx_validate_bearer_context(bearer_context_p)) && 
         (bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == 0xFF))
      {
        //Deallocate bearer
        ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }
      else
      {
        DS_3GPP_MSG0_ERROR("bearer_context_p is NULL.");
      }
    }
    // Increment the number of active bearer contexts in the PDN
    pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context++;

    ds_pdn_cntxt_set_default_bearer(pdn_cntx_p, bearer_context_p);
    
    //API to bind phys link and Iface

    /*-----------------------------------------------------------------------
      Register this bearer with the PDN context. Set is_default as true since
      this is the default bearer for that PDN context.
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_register_pdn_context(
                                        pdn_cntx_p, 
                            bearer_context_p,
                            TRUE
                          );
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    /*-----------------------------------------------------------------------
      Allocate a PDN flow manager and bearer flow manager
    -----------------------------------------------------------------------*/
    if(ds_flow_manager_allocate_new_flow_manager(pdn_cntx_p) == NULL)
    {
      DS_3GPP_MSG0_ERROR("Unable to allocate Flow Manager");
      ds_3gpp_dyn_mem_hdlr_free_flow_mgr_dyn_mem(pdn_cntx_p);
      DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      return;
    }

    bflow_manager_p = 
      ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);

    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
       ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem(bearer_context_p);
       DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
       return;
    }
    /*-------------------------------------------------------------------------
      Add Bearer Flow manager to Flow manager.
    -------------------------------------------------------------------------*/
    if ((ds_flow_manager_add_bearer_context_to_flow_manager(
                pdn_cntx_p,bearer_context_p)) == FALSE)
    {
      DATA_ERR_FATAL("Error in adding bearer flow manager to flow manager");
    } 
    DS_3GPP_MSG0_HIGH("Allocated Flow Manager and bearer Flow Manager");
    /*---------------------------------------------------------------------
      Update the PDN PTR.
    ---------------------------------------------------------------------*/
    bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_cntx_p;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  }
  else
  {
    bearer_context_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
  }

  ds_flow_manager_configure_default_flow(pdn_cntx_p,
                                         bearer_context_p,
                                         ps_iface_get_addr_family(iface_p));

  /*----------------------------------------------------------------------------
    Now check if there is an Iface of the other IP type bound to the same PDN 
    context. If there is one, call active bring up on the iface if the state
    of the Iface is not COMING_UP.

    Note: this need not be done for Default PDN, but the fact that we check the
    state of the iface takes care of this automatically.
   
  ----------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6

  if(companion_iface_p != NULL)
  {
    if(ps_iface_state(companion_iface_p) != IFACE_COMING_UP)
    {
      DS_3GPP_MSG0_ERROR("Bringing up other IFACE");
      ret_val = ps_iface_active_bring_up_cmd(companion_iface_p,
                                             &ps_errno, 
                                             NULL);
      /* If PS_IFACE returns CLOSEINPROGRESS,then up_cmd won't be posted */
      if(ret_val < 0 )
      {
        if(ps_errno == DS_ENETCLOSEINPROGRESS)
        {
          DS_3GPP_MSG0_ERROR("Other iface in going_down state!!");            
        }
      }
    }/* If PS Iface is not in coming up state, bring it up*/
    
  }/* companion_iface is valid */    
  
#endif /* FEATURE_DATA_PS_IPV6 */  

  /*-----------------------------------------------------------------------
    Change the state to COMING_UP for MO Calls.
    For MT calls, the state would be set to DS_PDN_CONTEXT_STATE_ANSWER 
  -----------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_DOWN)
  {
    pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
  }

  DS_3GPP_MSG2_HIGH("iface up cmd hdlr PDN cntx Iface:0x%x :info:%lx",\
            iface_p,(uint32) info_ptr);

  /*-------------------------------------------------------------------------
    Get the Primary Phys link pointer and make sure the phys link is down
  -------------------------------------------------------------------------*/
 /* only for attach case; that is PDN and bearer are known */
  phys_link_ptr = &(bearer_context_p->phys_link);

  /*-------------------------------------------------------------------------
    Bring up the physical link. For IPV6 calls, we should call dssnet_start.
    But for UMTS calls, dssnet_start just calls ps_phys_link_up_cmd. To
    avoid featurization, call ps_phys_link_up_cmd here itself.
  -------------------------------------------------------------------------*/      
  switch(PS_PHYS_LINK_GET_STATE(phys_link_ptr))
  {
    case PHYS_LINK_UP:
    case PHYS_LINK_COMING_UP:
    case PHYS_LINK_DOWN:
    case PHYS_LINK_NULL:
      ret_val = ps_phys_link_up_cmd(phys_link_ptr,
                                &ps_errno,
                                info_ptr);
      
        
      
      if (ret_val == 0)
      {
        DS_3GPP_MSG0_HIGH(" Bringing up phys link when already up / coming up:");
      }
      else if ( ps_errno != DS_EWOULDBLOCK)
      {
        ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_p),
                            PS_EIC_NOT_SPECIFIED);
        ps_phys_link_gone_ind(phys_link_ptr);
        down_reason = PS_NET_DOWN_REASON_PHYS_LINK_CLOSE_IN_PROGRESS;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }
      break;
    default:
      DS_3GPP_MSG1_ERROR("Iface_up_cmd called in unexpected state: %d.", PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
  }/* Switch */
  return;

} /* ds_pdn_cntxt_ifaceUpCmdHdlr() */

/*===========================================================================
FUNCTION      ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr

DESCRIPTION
  Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Brings up the iface requested after allocating a PDN context and a bearer
  context.

PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.
  info_ptr      - Pointer to data passed in from the app.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  If MT table has matching call info as current call,
  this will clear the table entry.
===========================================================================*/
static void ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr
(
  ps_iface_type *iface_p,
  void          *info_ptr
)
{
#ifdef FEATURE_EMERGENCY_PDN
  int16                ps_errno;           /* Errno value                 */
  int                  ret_val;
  ps_phys_link_type        *phys_link_ptr      = NULL; 
      /** Ptr to the phys link of call*/
  ds_bearer_context_s      *bearer_context_p   = NULL;
  ds_pdn_context_s         *pdn_cntx_p         = NULL;
  ds_3gpp_iface_s          *ds3gpp_iface_p     = NULL;  
  ps_iface_type            *companion_iface_p  = NULL;
  ds_bearer_flow_manager_s *bflow_manager_p    = NULL;  
  ps_iface_net_down_reason_type down_reason    = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_3gpp_pdn_cntxt_bearer_mgmt_status_e 
                            bearer_mgmt_status = DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL;
#ifdef FEATURE_DATA_TE_MT_PDP
  cm_call_id_type           call_id            = CM_CALL_ID_INVALID;
#endif /* FEATURE_DATA_TE_MT_PDP */
  sys_sys_mode_e_type       network_mode       = SYS_SYS_MODE_NONE;
  sys_modem_as_id_e_type    subs_id            = SYS_MODEM_AS_ID_NONE;
  
  /*-------------------------------------------------------------------------
    Get the DS Iface pointer from the PS Iface. Once this is done, get the
    PDN context pointer.
  -------------------------------------------------------------------------*/
  ds3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  
  if (ds3gpp_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("DS Iface is NULL. Returning");
    return;
  }

  pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_p->client_data_ptr);
  
  if (!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }  

  /*-------------------------------------------------------------------------
    When making Emergency Calls; the whole idea is that CM and NAS have
    decided that LTE is the best system to originate the call. (Even though we
    might not have full service on LTE).
   
    So DS will just respect CM and DS's decisions. Internal system mode will be
    forced to LTE so that DS thinks and operates like we have LTE full service.
  -------------------------------------------------------------------------*/
  network_mode = ds3gpp_get_current_network_mode(subs_id);
  if(network_mode != SYS_SYS_MODE_LTE)
  {
    DS_3GPP_MSG1_ERROR("Current network_mode is :%d",network_mode);
    DS_3GPP_MSG0_HIGH("Updating DS3GPP internal mode to LTE");
    ds3gpp_update_network_mode(SYS_SYS_MODE_LTE, subs_id);
  }
  
   /*-------------------------------------------------------------------------
    Notify DSD that emergency call starts
   -------------------------------------------------------------------------*/
    ds3gdsdif_set_lte_emergency_call_in_prog(subs_id, TRUE);

   /*------------------------------------------------------------------------- 
    Save info_ptr in pdn_context. It may be required later for redialing the
    call.
   -------------------------------------------------------------------------*/
   if(info_ptr == NULL)
   {
     pdn_cntx_p->ds_pdn_context_dyn_p->info_null = TRUE;     
   }
   else
   {
     pdn_cntx_p->ds_pdn_context_dyn_p->info_null = FALSE;
     pdn_cntx_p->ds_pdn_context_dyn_p->info = *((network_params_info_type *)info_ptr);     
   }
  

#ifdef FEATURE_DATA_PS_IPV6

  companion_iface_p = ds_3gpp_pdn_get_companion_iface(iface_p);

#endif /* FEATURE_DATA_PS_IPV6 .*/

  /*---------------------------------------------------------------------------
    Check to see if the default bearer is already available. It is possible 
    that we are acting on the Iface UP command for a second Iface attached to 
    the same PDN context. In that case, there is no need to allocate a bearer.
  ---------------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr == NULL)
  {
    bearer_mgmt_status = ds_3gpp_pdn_cntxt_apply_bearer_mgmt(iface_p,
                                    DS_3GPP_MIN_BEARER_CONTEXT_EMERG);
    if (bearer_mgmt_status == DS_3GPP_PDN_CNTXT_BEARER_MGMT_IN_PROGRESS)
    {
      /*--------------------------------------------------------------------- 
        Currently no Bearer resources available. There is an ongoing
        attempt to allocate bearer resources from existing active PDN
        connections
        ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("Bearer mgmt for emerg iface 0x%x in progress. "
                        "Moving PDN cntxt state to AWAITING_BEARER_ALLOC",
                        iface_p);
      pdn_cntx_p->ds_pdn_context_dyn_p->state = 
                                    DS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC;
      return;
    }
    else if (bearer_mgmt_status == DS_3GPP_PDN_CNTXT_BEARER_MGMT_FAIL) 
    {
      /*----------------------------------------------------- 
        No Bearer resources available to bring up emergency
        iface. Clean up Emergency PDN
        -----------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("Bearer mgmt Failed.No bearer to bring up emerg "
                        "iface 0x%x",iface_p);
      down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
      ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
      return;
    }
    else
    {
      /* ----------------------------------------------------------
         Validation success. Sufficient Bearer resources available.
         Continue bringing up Emergency Iface
         ----------------------------------------------------------*/
      DS_3GPP_MSG1_MED("Bearer mgmt for iface up not reqd now."
                        "Continue iface 0x%x bring up",iface_p);
    }
    /* check if it is a MT call, if then bearer context will be 
       already allocated 
    */

    if( pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_ANSWER )
    {
      /*-----------------------------------------------------------------------
        Allocate a bearer context from the free pool of bearers. If no bearer 
        is available, we should fail the call.
      -----------------------------------------------------------------------*/
      bearer_context_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);
      if(bearer_context_p == NULL)
      {
        DS_3GPP_MSG0_HIGH("Could not allocate bearer");
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }

      if(ds_pdn_cntxt_store_bearer_context(pdn_cntx_p,(void *)bearer_context_p) != \
                                                  DS3G_SUCCESS)
      {
        /*---------------------------------------------------------------------
          We should not hit this case of running out of placeholders for bearer
          during iface bring up. If this were to happen, it is probably because
          the bearer pointers were not cleaned up from a prior use of the PDN
          context.
        ---------------------------------------------------------------------*/
        //Post iface down
        DS_3GPP_MSG0_HIGH("Could not store the allocated bearer");

        //Deallocate bearer
        ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }      
    }
        
    // Increment the number of active bearer contexts in the PDN
    pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context++;

    ds_pdn_cntxt_set_default_bearer(pdn_cntx_p, bearer_context_p);
    
    //API to bind phys link and Iface

    /*-----------------------------------------------------------------------
      Register this bearer with the PDN context. Set is_default as true since
      this is the default bearer for that PDN context.
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_register_pdn_context(
                                        pdn_cntx_p, 
                            bearer_context_p,
                            TRUE
                          );
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    /*-----------------------------------------------------------------------
      Allocate a PDN flow manager and bearer flow manager
    -----------------------------------------------------------------------*/
    if(ds_flow_manager_allocate_new_flow_manager(pdn_cntx_p) == NULL)
    {
      DATA_ERR_FATAL("Unable to allocate Flow Manager");
    }

    bflow_manager_p = 
      ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);

    if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
    {
       DATA_ERR_FATAL("Unable to allocate bearer Flow Manager");
    }
   
    /*-------------------------------------------------------------------------
      Add Bearer Flow manager to Flow manager.
    -------------------------------------------------------------------------*/
    if ((ds_flow_manager_add_bearer_context_to_flow_manager(
                pdn_cntx_p,bearer_context_p)) == FALSE)
    {
      DATA_ERR_FATAL("Error in adding bearer flow manager to flow manager");
    } 
    DS_3GPP_MSG0_HIGH("Allocated Flow Manager and bearer Flow Manager");
    /*---------------------------------------------------------------------
      Update the PDN PTR.
    ---------------------------------------------------------------------*/
    bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_cntx_p;    
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  }
  else
  {
    bearer_context_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;    
  }

  ds_flow_manager_configure_default_flow(pdn_cntx_p,
                                         bearer_context_p,
                                         ps_iface_get_addr_family(iface_p));

  /*----------------------------------------------------------------------------
    Now check if there is an Iface of the other IP type bound to the same PDN 
    context. If there is one, call active bring up on the iface if the state
    of the Iface is not COMING_UP.

    Note: this need not be done for Default PDN, but the fact that we check the
    state of the iface takes care of this automatically.
  ----------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_IPV6

  if (companion_iface_p != NULL)
  {
    if(ps_iface_state(companion_iface_p) != IFACE_COMING_UP)
    {
      DS_3GPP_MSG0_ERROR("Bringing up other IFACE");
      ret_val = ps_iface_active_bring_up_cmd(companion_iface_p,
                                             &ps_errno, 
                                             info_ptr);
      /* If PS_IFACE returns CLOSEINPROGRESS,then up_cmd won't be posted */
      if(ret_val < 0 )
      {
        if(ps_errno == DS_ENETCLOSEINPROGRESS)
        {
          DS_3GPP_MSG0_ERROR("Other iface in going_down state!!");            
        }
      }
    }/* If PS Iface is not in coming up state, bring it up*/
    
  }/* companion_iface is valid */    
  
#endif /* FEATURE_DATA_PS_IPV6 */  

  /*-----------------------------------------------------------------------
    Change the state to COMING_UP for MO Calls.
    For MT calls, the state would be set to DS_PDN_CONTEXT_STATE_ANSWER 
  -----------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_DOWN)
  {
    pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
  }  

  DS_3GPP_MSG2_HIGH("Emerg iface up cmd hdlr PDN cntx Iface:0x%x :info:%lx",\
            iface_p,(uint32) info_ptr);

  /*-------------------------------------------------------------------------
    Get the Primary Phys link pointer and make sure the phys link is down
  -------------------------------------------------------------------------*/
 /* only for attach case; that is PDN and bearer are known */
  phys_link_ptr = &(bearer_context_p->phys_link);

  /*-------------------------------------------------------------------------
    Bring up the physical link. For IPV6 calls, we should call dssnet_start.
    But for UMTS calls, dssnet_start just calls ps_phys_link_up_cmd. To
    avoid featurization, call ps_phys_link_up_cmd here itself.
  -------------------------------------------------------------------------*/      
  switch(PS_PHYS_LINK_GET_STATE(phys_link_ptr))
  {
    case PHYS_LINK_UP:
    case PHYS_LINK_COMING_UP:
    case PHYS_LINK_DOWN:
    case PHYS_LINK_NULL:
      ret_val = ps_phys_link_up_cmd(phys_link_ptr,
                                &ps_errno,
                                info_ptr);
      
        
      
      if (ret_val == 0)
      {
        DS_3GPP_MSG0_HIGH(" Bringing up phys link when already up / coming up:");
      }
      else if ( ps_errno != DS_EWOULDBLOCK)
      {
        ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_p),
                            PS_EIC_NOT_SPECIFIED);
        ps_phys_link_gone_ind(phys_link_ptr);
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        ds_3gpp_pdn_context_cleanup_pdn(iface_p, down_reason);
        return;
      }
      break;
    default:
      DS_3GPP_MSG1_ERROR("Iface_up_cmd called in unexpected state: %d.", PS_PHYS_LINK_GET_STATE(phys_link_ptr));
      return;
  }/* Switch */
#else
  /*------------------------------------------------------------------------- 
    Do not apply Bearer mgmt policy for Emergency PDN
    -------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntxt_apply_bearer_mgmt(iface_p,0);
  DS_3GPP_MSG0_HIGH("Emergency PDN iface cannnot be brought up, not supported ");
#endif /* FEATURE_EMERGENCY_PDN */
  return;

} /* ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr() */

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_any_pending_v6_iface_cleanup_for_lpm

DESCRIPTION
  This function gives if anu V6 iface cleanup is pending during 
   lpm command processing

PARAMETERS
  cmd_ptr - cmd_ptr that holds the cm notify info
  
DEPENDENCIES
  None.

RETURN VALUE  
  boolean.

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_context_any_pending_v6_iface_cleanup_for_lpm
( 
  void 
)
{
  ds3gsubsmgr_subs_id_e_type    subs_id = DS3GSUBSMGR_SUBS_ID_INVALID;
  boolean                       ret_val = FALSE;
  /* ----------------------------------------------------------- */
 for (subs_id = DS3GSUBSMGR_SUBS_ID_1; subs_id < DS3GSUBSMGR_SUBS_ID_MAX; subs_id++)
 {
   if (ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_id] > 0)
   {
     ret_val = TRUE;
     DS_3GPP_MSG2_HIGH("num_ipv6_ifaces_to_cleanup during lpm:%d on subs_id:%d",
                          ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_id], subs_id);
     break;
   }
 }
 return ret_val;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_process_cm_notify_cmd

DESCRIPTION
  This function processes the cm notify cmd

PARAMETERS
  cmd_ptr - cmd_ptr that holds the cm notify info
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_process_cm_notify_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
  ds_pdn_context_s        *pdn_cntxt_p = NULL;
  /*  v4 Interface ptr for the call */
  ds_3gpp_iface_s         *ds3gpp_iface_v4_p = NULL;
#ifdef FEATURE_DATA_PS_IPV6
  /*  v6 Interface ptr for the call */
  ds_3gpp_iface_s         *ds3gpp_iface_v6_p = NULL;
  ps_iface_type           *v6_ps_iface_p      = NULL;   
  dssnet6i_state_type      dssnet_state       = DSSNET6I_CLOSED_STATE;
  dssnet6_sm_cb_type      *dssnet6_cb_p       = NULL;
#endif
  ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type 
                          *cm_notify_cmd_info_ptr = NULL;
  ps_iface_net_down_reason_type
                           net_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED; 
  int                      index = 0;
  sys_modem_as_id_e_type   pdn_subs_id = SYS_MODEM_AS_ID_NONE;
/*----------------------------------------------------------------------------*/

  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL cmd_ptr passed");
    return;
  }

  DS_3GPP_MSG0_HIGH("3GPP CARD REFRESH/LPM Processing");

  cm_notify_cmd_info_ptr = 
    (ds_3gpp_pdn_cntxt_cm_notify_cmd_info_type*)cmd_ptr->cmd_payload_ptr;

  if (cm_notify_cmd_info_ptr->cm_notify_type 
        == DS_3GPP_PDN_CNTXT_PWROFF_LPM_MODE) 
  {
    net_down_reason = PS_NET_DOWN_REASON_INTERNAL_LPM_OR_PWR_DOWN;
  }
  else if (cm_notify_cmd_info_ptr->cm_notify_type 
             == DS_3GPP_PDN_CNTXT_SUB_NOT_AVAIL_NOTIFY) 
  {
    net_down_reason = PS_NET_DOWN_REASON_INTERNAL_MMGSDI_CARD_EVT;
  }

  for (index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    pdn_cntxt_p = &ds_pdn_context_tbl[index];

      if((ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal(pdn_cntxt_p,&pdn_subs_id)== FALSE) ||
         !ds3gsubsmgr_is_subs_id_valid(pdn_subs_id))
      {
        DS_3GPP_MSG0_LOW("Not a valid PDN context. Skip lpm/refresh processing");
        continue;
      }

    if (cm_notify_cmd_info_ptr->cm_notify_type 
        != DS_3GPP_PDN_CNTXT_PWROFF_LPM_MODE)
      {
        if (pdn_subs_id != cm_notify_cmd_info_ptr->subs_id)
        {
          DS_3GPP_MSG2_LOW("PDN subs id %d does not match card refresh subs id %d."
                           " Skip processing",
                           pdn_subs_id,cm_notify_cmd_info_ptr->subs_id);
          continue;
        }
      }

    ds3gpp_iface_v4_p =
      (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

#ifdef FEATURE_DATA_PS_IPV6
    ds3gpp_iface_v6_p =
      (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
#endif /* FEATURE_DATA_PS_IPV6 */

    if (ds3gpp_iface_v4_p != NULL)
    {
      ds3gpp_iface_v4_p->ue_init_net_down_reason = net_down_reason;
    }

#ifdef FEATURE_DATA_PS_IPV6
    if (ds3gpp_iface_v6_p != NULL)
    {
      ds3gpp_iface_v6_p->
        ue_init_net_down_reason  = net_down_reason;
      /*--------------------------------------------------------------------- 
        Initiate graceful cleanup of the IPV6 SM and DSSNET SM's. Also
        the counter is incremented once for every graceful cleanup
        initiations as DSMGR needs to be notified once the cleanup of all
        IPV6 ifaces are done successfully
        ---------------------------------------------------------------------*/
      v6_ps_iface_p              = &(ds3gpp_iface_v6_p->ps_iface);
      dssnet6_cb_p               = &(ds3gpp_iface_v6_p->dssnet6_sm_cb);
      dssnet_state               = dssnet6_get_instance_state(dssnet6_cb_p);

      if ( PS_IFACE_IS_VALID(v6_ps_iface_p)                   
                        &&
          ((ps_iface_state(v6_ps_iface_p) == IFACE_UP)       ||
           (ps_iface_state(v6_ps_iface_p) == IFACE_ROUTEABLE)||
           (ps_iface_state(v6_ps_iface_p) == IFACE_CONFIGURING)) 
                        &&
           /*---------------------------------------------------------------- 
             Avoid incrementing the count/initiating clean up
             for redundant LPM calls to DS3GPP MH
             ----------------------------------------------------------------*/
           ((dssnet_state ==  DSSNET6I_WAITING_FOR_NET_UP_STATE) ||
            (dssnet_state ==  DSSNET6I_LINK_RESYNC_STATE)        ||
            (dssnet_state ==  DSSNET6I_NET_UP_STATE))
         )
      {
        (void)dssnet_stop(v6_ps_iface_p,dssnet6_cb_p);
        ds_3gpp_num_ipv6_ifaces_to_cleanup[pdn_subs_id]++;
      }
    }
#endif /* FEATURE_DATA_PS_IPV6 */
  }

  if (cm_notify_cmd_info_ptr->cm_notify_type 
        == DS_3GPP_PDN_CNTXT_PWROFF_LPM_MODE)
  {
    /*----------------------------------------------------------------------- 
      In case of LPM PWR OFF mode, Notify DSMGR which in turn will notify
      CM after all the other internal DS modules are done processing
    -----------------------------------------------------------------------*/
    if ((ds_3gpp_pdn_context_any_pending_v6_iface_cleanup_for_lpm() != TRUE)
        && (ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress() == TRUE))  
    {
      ds3geventmgr_clr_and_notify_lpm_pwr_off_status(
        DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK);
    }
  }
  else if (cm_notify_cmd_info_ptr->cm_notify_type 
             == DS_3GPP_PDN_CNTXT_SUB_NOT_AVAIL_NOTIFY) 
  {
    /*----------------------------------------------------------------------- 
      In case of LPM PWR OFF mode, Notify DSMGR which in turn will notify
      CM after all the other internal DS modules are done processing
    -----------------------------------------------------------------------*/
    if((ds3gsubsmgr_is_subs_id_valid(cm_notify_cmd_info_ptr->subs_id)) &&
       (ds_3gpp_num_ipv6_ifaces_to_cleanup[cm_notify_cmd_info_ptr->subs_id]<=0)
        && (ds3geventmgr_is_sub_not_avail_processing_in_progress(
                                              cm_notify_cmd_info_ptr->subs_id) == TRUE))  
    {
      ds3geventmgr_clr_and_notify_sub_not_avail_status(
        DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK,
        cm_notify_cmd_info_ptr->subs_id,
        ds3gi_sub_not_avail_subs_id_mask);
    }
  }
} /* ds_3gpp_pdn_cntxt_process_cm_notify_cmd */

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_process_mmgsdi_uicc_3g_reset_cmd

DESCRIPTION
  This function processes the MMGSDO UICC, 3G reset cmd

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_process_mmgsdi_uicc_3g_reset_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("NULL cmd_ptr. Return");
    return;
  }

  subs_id = *((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr);

  if(ds_3gpp_throt_get_srv_req_throttle_behavior(subs_id) 
       == DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402)
  {
    DS_3GPP_MSG1_HIGH("Resetting pdn blocked in apn reject sm for "
                      "mmgsdi refresh on subs id %d",
                      subs_id);
    ds_3gpp_pdn_throt_set_apn_reject_pdn_blocked(FALSE,subs_id);
  }
  else
  {
    DS_3GPP_MSG0_HIGH("srv req throttle behavior is not "
                      "DS_3GPP_SRV_REQ_THROT_BLOCK_PLMN_PREF_T3402. "
                      "Not resetting pdn blocked in apn reject sm for "
                      "mmgsdi refresh");
  }
}
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_process_thermal_mitigation_cmd

DESCRIPTION
  This function processes the thermal mitigation command

PARAMETERS
    cmd_ptr - cmd_ptr that holds the thermal mitigation action
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_process_thermal_mitigation_cmd
(
  const ds_cmd_type *cmd_ptr
)
{
  ds3geventmgr_th_mit_action_type th_mit_action = DS3GEVENTMGR_TH_MIT_MIN;
  int                             index = 0;
  ds_pdn_context_s               *pdn_cntxt_p = NULL;
  ds_3gpp_iface_s                *ds3gpp_iface_v4_p = NULL;
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s                *ds3gpp_iface_v6_p = NULL;
#endif /* FEATURE_DATA_PS_IPV6 */
  sys_sys_mode_e_type             current_nw_mode = SYS_SYS_MODE_NONE; 
#ifdef FEATURE_DATA_LTE
  boolean                          ca_scell_active;
#endif /* FEATURE_DATA_LTE */
  boolean                         tear_down_pdn = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DS_3GPP_MSG0_ERROR("NULL cmd_ptr passed");
    return;
  }

  th_mit_action = *((ds3geventmgr_th_mit_action_type*)cmd_ptr->cmd_payload_ptr);

  DS_3GPP_MSG1_HIGH("Received Thermal Mitigation action: %d",
                    th_mit_action);

  ds_3gpp_pdn_cntxt_set_th_mit_pdn_state(th_mit_action);
  
  switch (th_mit_action)
  {
    case DS3GEVENTMGR_TH_MIT_ALLOW_ALL_CALLS:
      DS_3GPP_MSG0_HIGH("Unblock all PDNs");
      break;

    case DS3GEVENTMGR_TH_MIT_ALLOW_ONLY_IMS_CALLS:
      DS_3GPP_MSG0_HIGH("Tearing down all non-IMS PDNs");

      for (index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
      {
        tear_down_pdn = TRUE;

        pdn_cntxt_p = &ds_pdn_context_tbl[index];

        if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p))
        {
          continue;
        }

        /* Do not tear down emergency call */
        if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntxt_p) == TRUE)
        {
          DS_3GPP_MSG0_MED("PDN is emergency. Do not tear down.");
          continue;
        }

        ds3gpp_iface_v4_p =
          (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
           [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

#ifdef FEATURE_DATA_PS_IPV6
        ds3gpp_iface_v6_p =
          (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
           [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
#endif /* FEATURE_DATA_PS_IPV6 */

        DS_3GPP_MSG2_MED("v4 iface 0x%x. v6 iface 0x%x ",
                         ds3gpp_iface_v4_p, ds3gpp_iface_v6_p);

        /*-------------------------------------------------------------------
         Logic for tear down is as follows:
           - By default, all PDNs will be torn down
           - If sys mode is LTE:
              1) If an iface has IMS apn type, then don't tear down
              2) If no ifaces are in use, then don't tear down
        -------------------------------------------------------------------*/
        current_nw_mode = ds3gpp_get_current_network_mode(pdn_cntxt_p->
                                                 ds_pdn_context_dyn_p->subs_id);
        if (current_nw_mode == SYS_SYS_MODE_LTE)
        {
          if (ds3gpp_iface_v4_p != NULL && 
              PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v4_p->ps_iface))
                 == PS_IFACE_APN_TYPE_IMS)
          {
            DS_3GPP_MSG1_MED("Do not Tear down v4 apn type: %d",
                         PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v4_p->ps_iface)));
            tear_down_pdn = FALSE;
          }

#ifdef FEATURE_DATA_PS_IPV6
          if (ds3gpp_iface_v6_p != NULL && 
              PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v6_p->ps_iface))
                 == PS_IFACE_APN_TYPE_IMS)
          {
            DS_3GPP_MSG1_MED("Do not Tear down v6 apn type: %d,",
                         PS_IFACE_GET_APN_TYPE(&(ds3gpp_iface_v6_p->ps_iface)));
            tear_down_pdn = FALSE;
          }
#endif /* FEATURE_DATA_PS_IPV6 */
          /*-------------------------------------------------------------------
           For 3rd party IMS, the APN Type will not be set in the policy.
           Instead, the APN type will be set from the UI for a particular
           APN. In this case, MH needs to match the APN for the PDN with
           the IMS APN in the device settings. If they match, then it is
           a 3rd part IMS PDN and MH should not tear down.
          -------------------------------------------------------------------*/

          if(ds_3gpp_pdn_cntxt_is_pdn_third_party_ims(pdn_cntxt_p,
                                                      pdn_cntxt_p->
                                                  ds_pdn_context_dyn_p->subs_id) 
             == TRUE)
          {
            DS_3GPP_MSG0_MED("PDN APN matches with IMS PDN APN in device "
                             "config. Do not tear down");
            tear_down_pdn = FALSE;
          }
        }

#ifdef FEATURE_DATA_LTE
        /* Get the LTE cell info to determine carrier aggregation = on/off */
        ca_scell_active 
          = ds_eps_pdn_cntxt_get_active_scell(pdn_cntxt_p->
                                                 ds_pdn_context_dyn_p->subs_id);
#endif /* FEATURE_DATA_LTE */

        /*-------------------------------------------------------------------
           If both ifaces are not in use, then the PDN is the attach PDN
           and it has no clients. There is no need to tear it down.
           Otherwise, detach will happen.
         
           If Carrier aggregation is on, then always tear down the attach
           PDN as well.
        -------------------------------------------------------------------*/
        if (((ds3gpp_iface_v4_p != NULL &&
            PS_IFACE_IS_IN_USE(&(ds3gpp_iface_v4_p->ps_iface)) == FALSE) ||
            (ds3gpp_iface_v4_p == NULL))
#ifdef FEATURE_DATA_PS_IPV6
            && ((ds3gpp_iface_v6_p != NULL &&
                PS_IFACE_IS_IN_USE(&(ds3gpp_iface_v6_p->ps_iface)) == FALSE) ||
                (ds3gpp_iface_v6_p == NULL))
#endif /* FEATURE_DATA_PS_IPV6 */
#ifdef FEATURE_DATA_LTE
            && (ca_scell_active == FALSE)
#endif /* FEATURE_DATA_LTE */
           )
        {
          DS_3GPP_MSG0_MED("ifaces not in use and scell is not active. "
                           "Do not tear down");

          tear_down_pdn = FALSE;
        }

        if (tear_down_pdn == TRUE)
        {
          DS_3GPP_MSG0_MED("Active tearing down calls");

          pdn_cntxt_p->ds_pdn_context_dyn_p->state = 
                                               DS_PDN_CONTEXT_STATE_GOING_DOWN;
          ds_3gpp_pdn_cntx_teardown_PDN(pdn_cntxt_p,
                                        PS_NET_DOWN_REASON_THERMAL_MITIGATION);
        }
      }
      break;

    case DS3GEVENTMGR_TH_MIT_TEAR_DOWN_BLOCK_ALL_CALLS:

        DS_3GPP_MSG0_HIGH("Tearing down all Non-LTE PDNs ");

        for (index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
        {
          pdn_cntxt_p = &ds_pdn_context_tbl[index];

        if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p))
          {
            continue;
          }
         current_nw_mode = ds3gpp_get_current_network_mode(pdn_cntxt_p->
                                                 ds_pdn_context_dyn_p->subs_id);
         if( current_nw_mode != SYS_SYS_MODE_LTE)
         {
          /* Do not tear down emergency call */
          if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntxt_p) == TRUE)
          {
            DS_3GPP_MSG0_MED("PDN is emergency. Do not tear down.");
            continue;
          }

          ds3gpp_iface_v4_p =
            (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
             [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

#ifdef FEATURE_DATA_PS_IPV6
          ds3gpp_iface_v6_p =
            (pdn_cntxt_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
             [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
#endif /* FEATURE_DATA_PS_IPV6 */

          DS_3GPP_MSG2_MED("v4 iface 0x%x. v6 iface 0x%x ",
                         ds3gpp_iface_v4_p, ds3gpp_iface_v6_p);

          if (ds3gpp_iface_v4_p != NULL
#ifdef FEATURE_DATA_PS_IPV6
            || ds3gpp_iface_v6_p != NULL
#endif /* FEATURE_DATA_PS_IPV6 */
            )
         {
           DS_3GPP_MSG0_MED("Active tearing down calls");

           pdn_cntxt_p->ds_pdn_context_dyn_p->state = 
                                               DS_PDN_CONTEXT_STATE_GOING_DOWN;
           ds_3gpp_pdn_cntx_teardown_PDN(pdn_cntxt_p,
                                        PS_NET_DOWN_REASON_THERMAL_MITIGATION);
         }
       }
     }
     break;

    default:
      break;
  }
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DSPDNCNTX_IS_LAST_PREVAILING_PDN_IN_LTE

DESCRIPTION
  Indicates if the passed PDN context is the only prevailing one
  in LTE mode. 
  
PARAMETERS
  pdn_cntx_ptr

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  True - If it is the last prevailing PDN
  False - Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntx_is_last_prevailing_pdn_in_lte
(
  ds_pdn_context_s *pdn_cntx_p 
)
{
  boolean                     ret_val = FALSE;
  sys_modem_as_id_e_type      subs_id  = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return ret_val;
  }
  /*------------------------------------------------------------------------ 
    Return true if all the below conditions are satisfed
    1. System mode is LTE
    2. Attached
    3. No other PDN is UP
    4. Bearer context's call mode associated with the PDN is also LTE
   
    The 4th check is necessary in race conditions where we have received a
    context transfer to WCDMA but haven't yet received the system mode change
    to LTE. 
    ------------------------------------------------------------------------*/
  if ((ds3g_is_pref_pkt_mode_ex((SYS_SYS_MODE_LTE),subs_id)) &&
      (ds_eps_attach_sm_is_ue_attached(subs_id)) &&
      (!dsPdnContext_is_any_other_normal_pdn_up(pdn_cntx_p, subs_id)) &&
      (((ds_bearer_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p->
      def_bearer_context_ptr))->ds_bearer_context_dyn_p->call_mode 
					 == SYS_SYS_MODE_LTE))
  {
    DS_3GPP_MSG0_MED("Last standing PDN in LTE, returning TRUE");
    ret_val = TRUE;
  }

  return ret_val;

}
#endif
/*===========================================================================
FUNCTION      DSPDNCNTX_IFACE_DOWN_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Adds a default route and call a function to bring up the physical link.

PARAMETERS
  call_instance - Index into call table for the call
  info_ptr      - void pointer

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_pdn_cntxt_ifaceDownCmdHdlr
(
  ps_iface_type*  iface_p,
  void *info_ptr
)
{
  ds_pdn_context_s *pdn_cntx_p              = NULL;
  ds_3gpp_iface_s     *ds_iface_p           = NULL;
  ds_3gpp_iface_s     *other_ds_iface_p     = NULL;  
  dssnet6i_state_type   dssnet_state  = DSSNET6I_MAX_STATE;
  int                   ret_val       = 0;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------- 
    Validate incoming iface ptr
    -------------------------------------------------------------------------*/
  if(iface_p == NULL)
  {
    DATA_ERR_FATAL("PdnCntx ifaceDownHdlr passed NULL as input");
    return;
  }
  /*------------------------------------------------------------------------- 
    No need to continue if the iface is already down
    -------------------------------------------------------------------------*/
  if ((ps_iface_state(iface_p) == IFACE_DOWN)           ||
      (ps_iface_state(iface_p) == IFACE_STATE_INVALID)  ||
      (ps_iface_state(iface_p) == IFACE_DISABLED)
     )
  {
    DS_3GPP_MSG1_HIGH("Iface 0x%x already down (or)Invalid",iface_p);
    return;
  }
  /*------------------------------------------------------------------------- 
    Fetch/Validate the PDN context pointer from the iface pointer
    -------------------------------------------------------------------------*/
  ds_iface_p      = (ds_3gpp_iface_s *)(iface_p->client_data_ptr); 

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG2_ERROR("No DS iface associated with PS iface 0x%x:%d",
                       iface_p,iface_p->instance);
    ASSERT(0);
    return;
  }
  pdn_cntx_p      = (ds_pdn_context_s*)(ds_iface_p->client_data_ptr);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    DATA_ERR_FATAL("PdnCntx in ifaceDownHdlr is NULL");
    return;
  }
  /*-------------------------------------------------------------------------
    Bringing down iface. It also means we are going to bring down the
    physlink (if companion iface is not in use) using  PS iface API's 
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("iface down cmd hdlr PDN cntx ptr:0x%x",pdn_cntx_p);

  /*-------------------------------------------------------------------------
     Find the companion iface from the PDN context associated with the
     iface
  -------------------------------------------------------------------------*/
    if (ps_iface_addr_family_is_v4(iface_p))
    {
#ifdef FEATURE_DATA_PS_IPV6
      other_ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
        [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      /*----------------------------------------------------------------------- 
        Do not continue with posting physlink down if the companion iface
        (IPV6)is still not completely down, even though it may not be in use
        Physlink will be brought down as a part of companion iface clean up.
        -----------------------------------------------------------------------*/
      if ((other_ds_iface_p != NULL)                                            &&
          (ps_iface_state(&(other_ds_iface_p->ps_iface)) != IFACE_DOWN)         &&
          (ps_iface_state(&(other_ds_iface_p->ps_iface)) != IFACE_STATE_INVALID)&&
          (ps_iface_state(&(other_ds_iface_p->ps_iface)) != IFACE_DISABLED))
      {
      /*---------------------------------------------------------------------
          Invoke Flow manager out of use cmd to clean up
          dedicated bearer (if any)
      ---------------------------------------------------------------------*/ 
        ds_flow_manager_iface_out_of_use(pdn_cntx_p,
                                         ds_iface_p,
                                         DS_3GPP_PDP_TYPE_IPV4,TRUE);
        return;
      }
#endif  
    }
#ifdef FEATURE_DATA_PS_IPV6
  else if (ps_iface_addr_family_is_v6(iface_p))
    {
    /*------------------------------------------------------------------ 
      Info Pointer contains local abort flag status. So, when the flag
      is set, IPV6 SM need not be gracefully brought down. Instead
      IPV6 SM will be aborted as a part of_CALL_END handling
      ------------------------------------------------------------------*/
    if ((info_ptr == NULL)||
        ((info_ptr != NULL)&&(info_ptr == (void *)FALSE)))
    {
    dssnet_state = dssnet6_get_instance_state(&(ds_iface_p->dssnet6_sm_cb));
    if((dssnet_state ==  DSSNET6I_WAITING_FOR_NET_UP_STATE) ||
       (dssnet_state ==  DSSNET6I_LINK_RESYNC_STATE)        ||
       (dssnet_state ==  DSSNET6I_NET_UP_STATE))       
      {
      ret_val = dssnet_stop(iface_p,&(ds_iface_p->dssnet6_sm_cb));
      DS_3GPP_MSG2_HIGH("dssnet_stop returned %d for Iface 0x%lx",
                         ret_val,iface_p);
        if (ret_val == 0) 
        {
         /*------------------------------------------------------------------
           DSSNET command posted. Physlink will be brought down, after the
           DSSNET6/IP6 SM is gracefully brought down
           ------------------------------------------------------------------*/
      return;

      }
      }
    }
  }
#endif
    

  /*------------------------------------------------------------------------- 
    Bring down Physlink
    -------------------------------------------------------------------------*/
  ds_3gpp_validate_and_bring_down_phys_link(iface_p,info_ptr, PS_NET_DOWN_REASON_CLIENT_END);
      return;

} /* ds_pdn_cntxt_ifaceDownCmdHdlr() */


/*===========================================================================
FUNCTION      DSPDNCNTX_IFACE_IN_USE_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Adds a default route and call a function to bring up the physical link.

PARAMETERS
  call_instance - Index into call table for the call
  info_ptr      -

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_pdn_cntxt_ifaceInUseCmdHdlr
(
  ps_iface_type*  iface_p
)
{
  ds_pdn_context_s *pdn_cntx_p= NULL;
  ds_3gpp_iface_s *ds_3gpp_iface_p = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    verify the input iface pointer.
  -------------------------------------------------------------------------*/
  if(iface_p == NULL)
  {
    DATA_ERR_FATAL("ds_pdn_cntxt ifaceInUse passed invalid argument");
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PDN context from the iface pointer.
  -------------------------------------------------------------------------*/
  ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);
  pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }
  /*-------------------------------------------------------------------------
    Verify the iface state is not in DOWN state.
  -------------------------------------------------------------------------*/
  if(ps_iface_state(iface_p) == IFACE_DOWN )
  {
     DS_3GPP_MSG1_ERROR("Invalid iface state for pdn cntx ptr 0x%x",pdn_cntx_p);
    //ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Change the 3GPP IFace state to IN_USE.
  -------------------------------------------------------------------------*/
  ds_3gpp_iface_p->state = DS_3GPP_IFACE_IN_USE_STATE;

  /*-------------------------------------------------------------------------
    If the iface state is in CONFIGURING state, and DHCP can be used, start
    the DHCP client
  -------------------------------------------------------------------------*/
  if((PS_IFACE_IS_ADDR_FAMILY_V4(iface_p)) &&
     (ps_iface_state(iface_p) == IFACE_CONFIGURING) && 
     (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.ipv4_addr_alloc 
      == DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4) &&
     (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_addr.
      pdp_addr.pdp_addr_ipv4 == 0))
  {
    if (ds_3gpp_iface_p->dhcp_handle != NULL)
    {
      /* 
        this is the case that DHCP failed for the default v4 only connection,
        in this case we will retry DHCP to obtain IP address
      */
      dhcp_client_renew(ds_3gpp_iface_p->dhcp_handle);
    }
    else if (ds_pdn_cntxt_dhcpStartV4AddrAlloc(ds_3gpp_iface_p, pdn_cntx_p) != TRUE)
    {
      DS_3GPP_MSG1_ERROR("Could not start DHCP Client for iface: %x!", iface_p);
      // need to figure out error handling. Whether to assert or tear down pdn context.
    }
    return;
  }

  /*-------------------------------------------------------------------------
    If iface state is in UP state, return iface_up_ind
  -------------------------------------------------------------------------*/
  if(ps_iface_state(iface_p) == IFACE_UP)
  {
    ps_iface_up_ind(iface_p);
    return;
  }

  /*-------------------------------------------------------------------------
    For other iface states we just return.
  -------------------------------------------------------------------------*/
  return;

} /* ds_pdn_cntxt_ifaceInUseCmdHdlr */

/*===========================================================================
FUNCTION dsPdnContext_is_any_other_normal_pdn_diff_profile_num_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) and not the same profile number in UP state.
  Whether the PDN passed in is UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_diff_profile_num_active
(
  ds_pdn_context_s *pdn_cntx_p
)
{
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;
  int                     pdn_instance;
  sys_modem_as_id_e_type  local_subs_id = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== FALSE)
  {
    return FALSE;
  }

  for(pdn_instance = 0;
      pdn_instance < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_instance++)
  {
    local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
    if((local_pdn_cntx_p == pdn_cntx_p) ||
       (!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal(local_pdn_cntx_p,
                                                 &local_subs_id))))
    {
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p))
    {
      DS_3GPP_MSG1_MED("PDN 0x%x is emergency PDN", local_pdn_cntx_p);
      continue;
    }
    else if (ds3gsubsmgr_is_subs_id_valid(subs_id) && subs_id != local_subs_id )
    {
      DS_3GPP_MSG2_MED("Local Subs id(CM): %d does not match passed "
                       "Subs id(CM): %d", local_subs_id, subs_id);
      continue;
    }
    else
    {
      DS_3GPP_MSG2_MED("PDN 0x%x is in %d state",
                        local_pdn_cntx_p, 
                        local_pdn_cntx_p->ds_pdn_context_dyn_p->state );

      /* This is a PDN context different from the one passed into the func and
         a different profile number*/
      if((local_pdn_cntx_p->ds_pdn_context_dyn_p->state ==
          DS_PDN_CONTEXT_STATE_UP) &&
         (local_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num != 
          pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num ))
      {
        return TRUE;
      }
    }

  } /* Loop over all PDN contexts*/

  return FALSE;
} /* dsPdnContext_is_any_other_normal_pdn_active */


/*===========================================================================
FUNCTION dsPdnContext_is_any_other_normal_pdn_active

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_active
(
  ds_pdn_context_s *pdn_cntx_p
)
{
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;
  int                     pdn_instance;
  sys_modem_as_id_e_type  local_subs_id = SYS_MODEM_AS_ID_NONE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== FALSE)
  {
    return FALSE;
  }

  for(pdn_instance = 0;
      pdn_instance < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_instance++)
  {
    local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
    if((local_pdn_cntx_p == pdn_cntx_p) ||
       (!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal(local_pdn_cntx_p,
                                                 &local_subs_id))))
    {
      DS_3GPP_MSG1_LOW("PDN 0x%x not in use or same as the one passed in",
                       local_pdn_cntx_p);
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p))
    {
      DS_3GPP_MSG1_LOW("PDN 0x%x is emergency PDN", local_pdn_cntx_p);
      continue;
    }
    else if (ds3gsubsmgr_is_subs_id_valid(subs_id) && subs_id != local_subs_id )
    {
      DS_3GPP_MSG2_LOW("Local Subs id(CM): %d does not match passed "
                       "Subs id(CM): %d", local_subs_id, subs_id);
      continue;
    }
    else
    {
      DS_3GPP_MSG2_LOW("PDN 0x%x is in %d state",
                        local_pdn_cntx_p, 
                        local_pdn_cntx_p->ds_pdn_context_dyn_p->state );

      /* This is a PDN context different from the one passed into the func */
      if(local_pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_DOWN)
      {
        return TRUE;
      }
    }

  } /* Loop over all PDN contexts*/

  return FALSE;
} /* dsPdnContext_is_any_other_normal_pdn_active */

/*===========================================================================
FUNCTION dsPdnContext_is_any_other_normal_pdn_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN (apart from the one passed in) in UP state. Whether the PDN passed in is 
  UP or not, does not matter.
 
  This function does not take emergency PDNs into consideration.
 
PARAMETERS
  PDN Context

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is a PDN in UP state, other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_other_normal_pdn_up
(
  ds_pdn_context_s       *pdn_cntx_p,
  sys_modem_as_id_e_type subs_id
)
{
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;
  int                     pdn_instance;
  sys_modem_as_id_e_type  local_subs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(pdn_instance = 0;
      pdn_instance < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_instance++)
  {
    local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
    if((local_pdn_cntx_p == pdn_cntx_p) ||
       (!(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal(local_pdn_cntx_p,
                                                 &local_subs_id))))
    {
      DS_3GPP_MSG1_LOW("PDN 0x%x not in use or same as the one passed in",
                       local_pdn_cntx_p);
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p))
    {
      DS_3GPP_MSG1_MED("PDN 0x%x is emergency PDN", local_pdn_cntx_p);
      continue;
    }
    else if (ds3gsubsmgr_is_subs_id_valid(subs_id) && subs_id != local_subs_id )
    {
      DS_3GPP_MSG2_MED("Local Subs id(CM): %d does not match passed "
                       "Subs id(CM): %d", local_subs_id, subs_id);
      continue;
    }
    else
    {
      DS_3GPP_MSG2_MED("PDN 0x%x is in %d state",
                        local_pdn_cntx_p, 
                        local_pdn_cntx_p->ds_pdn_context_dyn_p->state );

      /* This is a PDN context different from the one passed into the func */
      if(local_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
      {
        return TRUE;
      }
    }

  } /* Loop over all PDN contexts*/

  return FALSE;
} /* dsPdnContext_is_any_other_normal_pdn_up */

/*===========================================================================
FUNCTION dsPdnContext_is_any_normal_pdn_coming_up

DESCRIPTION
  This function loops over all PDN contexts to see if there is atleast one more 
  PDN are in coming UP state.
  This function does not take emergency PDNs into consideration.
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_normal_pdn_coming_up(void)
{
  return (dsPdnContext_is_any_normal_pdn_coming_up_ex(ds3gsubsmgr_get_def_data_subs_id()));
} /* dsPdnContext_is_any_normal_pdn_coming_up */

/*===========================================================================
FUNCTION dsPdnContext_is_any_normal_pdn_coming_up_ex

DESCRIPTION
  This function loops over all PDN contexts for the passed subs_id to see if there is atleast one more 
  PDN are in coming UP state.
  This function does not take emergency PDNs into consideration.
 
PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if there is an active PDN other than the PDN passed in.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean dsPdnContext_is_any_normal_pdn_coming_up_ex
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;
  int                    pdn_instance;
  sys_modem_as_id_e_type pdn_subs_id = SYS_MODEM_AS_ID_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_instance = 0;
      pdn_instance < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_instance++)
  {
    local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
    if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(local_pdn_cntx_p, &pdn_subs_id))
    {
      /* This is either not in use, or is the same PDN as passed in */
      continue;
    }
    else if (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p))
    {
      continue;
    }
    else
    {
      /* This is a PDN context different from the one passed into the func */
      if((pdn_subs_id == subs_id) &&
         (local_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_COMING_UP))
      {
        return TRUE;
      }
    }

  } /* Loop over all PDN contexts*/

  return FALSE;
}/*dsPdnContext_is_any_normal_pdn_coming_up_ex*/

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION      DSPDNCNTX_IS_PDN_CONTEXT_TEARABLE

DESCRIPTION
  This function determines if the PDN context passed in can be torn down.
  The logic that is used to arrive at the conclusion is based on the existence
  if atleast one more PDN that is in the UP state

  However, if the configurable item is set such that we always tear down
  non-default PDN connection as soon as requested, this function returns TRUE.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context that needs to be examined

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  TRUE is PDN context can be torn down, FALSE otherwise

SIDE EFFECTS
  None.
===========================================================================*/
static boolean ds_pdn_cntxt_is_pdn_context_tearable(ds_pdn_context_s *pdn_cntx_p)
{
  boolean          ret_val = FALSE;
  sys_modem_as_id_e_type  subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Loop over all PDN contexts to see if there is atleast one more PDN (apart
    from the one passed in) in UP state.
    If so, set the return TRUE
  -------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return ret_val;
  }
  
  ret_val = dsPdnContext_is_any_other_normal_pdn_up(pdn_cntx_p, subs_id);

  /*--------------------------------------------------------------------------
    OR the result of the operation just done with the configuration that has
    been made wrt device operation.

    If the flag ds_3gpp_connect_def_pdn_flag is set, it means that we need to
    tear down non default connections as soon as requested. Thus even if this
    PDN context is the last standing context, we return TRUE
  --------------------------------------------------------------------------*/
  if( FALSE == ret_val )
  {
    ret_val = ds_3gpp_nv_manager_get_always_connect_def_pdn_flag(subs_id);
  }

  else
  {
    if (pdn_cntx_p->ds_pdn_context_dyn_p->keep_alive_pdn == FALSE)
    {
      ret_val = TRUE;
    }
    else
    {
      ret_val = ds_eps_kamgr_check_for_pdn_teardown(subs_id);
    }
  }
  return ret_val;
} /* ds_pdn_cntxt_is_pdn_context_tearable */
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION      DSPDNCNTX_IFACE_OUTOF_USE_CMD_HDLR

DESCRIPTION
  Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Adds a default route and call a function to bring up the physical link.

PARAMETERS
  call_instance - Index into call table for the call
  info_ptr      -

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_pdn_cntxt_ifaceOutofUseCmdHdlr
(
  ps_iface_type*  iface_p,
  boolean is_companion_iface_in_use,
  boolean ota_teardown,
  void *info_ptr
)
{
  ds_pdn_context_s       *pdn_cntx_p = NULL;
  ds_3gpp_iface_s        *ds_3gpp_iface_p = NULL, *ds_3gpp_iface_ptr = NULL;
  ds_bearer_context_s    *def_bearer_context_p = NULL;
  int                    iface_index = 0, tear_down_val = 0;
  ds_umts_pdp_type_enum_type  pdn_pdp_type;
  int16                   ps_errno = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    verify the input iface pointer.
  -------------------------------------------------------------------------*/
  if(iface_p == NULL)
  {
    DATA_ERR_FATAL("ds_pdn_cntxt ifaceInUse passed invalid argument");
    return;
  }

  /*-------------------------------------------------------------------------
    Extract the PDN CNTX pointer from the iface.
  -------------------------------------------------------------------------*/
  ds_3gpp_iface_p = (ds_3gpp_iface_s*)(iface_p->client_data_ptr);

  pdn_cntx_p = (ds_pdn_context_s*)(ds_3gpp_iface_p->client_data_ptr);

  /*-------------------------------------------------------------------------
    Verify that the PDN context exits for this call. There is every chance
    that the PDN context has been freed.
  -------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get the Primary bearer context
  -------------------------------------------------------------------------*/
  def_bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->
                                    ds_pdn_context_dyn_p->def_bearer_context_ptr);
  
  /*-------------------------------------------------------------------------
    Verify the iface state is not in DOWN state.
  -------------------------------------------------------------------------*/
  if(ps_iface_state(iface_p) == IFACE_DOWN )
  {
    DS_3GPP_MSG1_HIGH("Invalid iface state for pdn cntx ptr 0x%x",pdn_cntx_p);
    // ASSERT(0);
    // if app calls iface down after call end is being processed, iface will
    // be down
    return;
  }

  /*------------------------------------------------------------------------
    Enable flow on all the phys links for the bearers in this PDN context
  -------------------------------------------------------------------------*/
  if(ds_bearer_cntx_validate_bearer_context(def_bearer_context_p))
  {
    ps_phys_link_enable_flow( &(def_bearer_context_p->phys_link),
                              DS_FLOW_UMTS_MASK );
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Default bearer cntxt ptr is NULL");
  }

  /*-------------------------------------------------------------------------
    Change the 3GPP IFace state to NOT_IN_USE.
  -------------------------------------------------------------------------*/
  ds_3gpp_iface_p->state = DS_3GPP_IFACE_NOT_IN_USE_STATE;
 
  /*-------------------------------------------------------------------------
    Check if DCHP client is started, if so we may need to stop it.
  -------------------------------------------------------------------------*/
  if (ds_3gpp_iface_p->dhcp_handle != NULL)
  {
    if ((ps_iface_state(iface_p) == IFACE_UP) ||
        (ps_iface_state(iface_p) == IFACE_ROUTEABLE)
       )
    {
      pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_cntx_p);
    }
    else
    {
      pdn_pdp_type = pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
        context.pdp_type;
    }

    /*-------------------------------------------------------------------------
     If DHCP is used and pdp type is ipv4v6, we can release address and  stop
     the DHCP client.
    -------------------------------------------------------------------------*/
    if (pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      ds_pdn_cntxt_dhcpStopClient(ds_3gpp_iface_p);
    }
  }
  
  /*------------------------------------------------------------------------ 
   Clear the bridge and also clear the a2_bridging_context 
   ------------------------------------------------------------------------*/
  ps_iface_set_bridge(iface_p, NULL);
  /*---------------------------------------------------------------------
    Invoke iface out of use cmd to clean up dedicated bearer
  ---------------------------------------------------------------------*/ 
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]
                   == ds_3gpp_iface_p)
  {
    ds_flow_manager_iface_out_of_use(pdn_cntx_p,
                         ds_3gpp_iface_p,DS_3GPP_PDP_TYPE_IPV4,FALSE);
  }
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]
                   == ds_3gpp_iface_p)
  {
    ds_flow_manager_iface_out_of_use(pdn_cntx_p,
                         ds_3gpp_iface_p,DS_3GPP_PDP_TYPE_IPV6,FALSE);
  }

  if (!is_companion_iface_in_use)
  {
    ds_bearer_cntxt_cleanup_call_resources(pdn_cntx_p);

    if(ota_teardown)
    {
      for(iface_index = 0; 
          iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
          iface_index++)
      {
        ds_3gpp_iface_ptr = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[iface_index];
        if (ds_3gpp_iface_ptr == NULL)
        {
          continue;
        }
        else
        {
          tear_down_val = ps_iface_active_tear_down_cmd(&(ds_3gpp_iface_ptr->ps_iface),
                                                  &ps_errno, info_ptr);

          if(tear_down_val == 0)
          {
            DS_3GPP_MSG0_HIGH("Iface in DOWN state already");
          }
          else if(ps_errno != DS_EWOULDBLOCK)
          {
            DS_3GPP_MSG1_HIGH("Error while tearing down the iface. Err Code: %d",
                     ps_errno);
          }
        }
      } /* for each iface in PDN context */
      /*------------------------------------------------------------------------ 
      Clear the apn param chg timer, Out of use cmd indicates that apn param
      chg redial is no longer needed since the timer has torn down the call
     ------------------------------------------------------------------------*/
     (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_timer));
    }
  } /* If this is the last Iface to go out of use*/

  return;
} /* ds_pdn_cntxt_ifaceOutofUseCmdHdlr */

/*===========================================================================

FUNCTION DSUMTSPS_RESEND_PDN_CONN_REQ

DESCRIPTION

  This function is used to resend the PDN connectivity request. This function
  is applicable only to the LTE mode of operation.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context on which the request is to be sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds_pdn_cntxt_resend_pdn_conn_req
(
  ds_pdn_context_s*       pdn_cntx_p
)
{
  ds_3gpp_iface_s          *ds_iface_p;
  ds_bearer_context_s      *bearer_context_p;
  int                       ret_val;
  int16                     ps_errno;
  ds_bearer_flow_manager_s *bflow_manager_p= NULL;
  ds_flow_manager_s        *flow_manager_p = NULL;
  void                     *info_ptr;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
#ifdef FEATURE_DATA_LTE
  uint8                     index = 0;
#endif /* FEATURE_DATA_LTE */
  ps_iface_down_mh_event_info_type   down_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== FALSE)
  {
    return;
  }

  if(pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_COMING_UP)
  {
    pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
    DS_3GPP_MSG0_HIGH("changing PDN context state to COMING_UP state");
  }
  else
  {
    DS_3GPP_MSG0_HIGH(" PDN context state : COMING_UP state");
  }

  down_info.bearer_tech.rat_value = 
    ds_3gpp_map_sys_mode_to_ds_sys_rat
      (ds3gpp_get_current_network_mode(subs_id));
  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);

  /*-----------------------------------------------------------------------
    Get the DS Iface based on the PDP type
  -----------------------------------------------------------------------*/
  if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type == 
     DS_UMTS_PDP_IPV4)
  {
    ds_iface_p = 
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
      [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
  }
  else 
  {
    ds_iface_p = 
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
      [DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
  }
  
  bearer_context_p = pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
  /*------------------------------------------------------------------------
     If the bearer is already allocated, we only need to bring up the bearer
  ------------------------------------------------------------------------*/
  if( bearer_context_p == NULL )
  {

    /*-----------------------------------------------------------------------
    Allocate a bearer context from the free pool of bearers. If no bearer 
    is available, we should fail the call. 
    -----------------------------------------------------------------------*/
    bearer_context_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      DS_3GPP_MSG1_HIGH("Could not allocate bearer - Insufficient Resources. "
                        "Remove the PDN entry from the APN table and free the "
                        "PDN 0x%x",
                        pdn_cntx_p);

      down_info.down_reason = PS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
      ps_iface_down_ind(&(ds_iface_p->ps_iface), &down_info);

      /*-----------------------------------------------------------------------
        Remove the entry from the APN table
      -----------------------------------------------------------------------*/
      if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
      }

#ifdef FEATURE_DATA_LTE
      /* Find an entry in the PDN Limit Table. If an entry is found,
         validate if the entry can be cleared at this point. If yes,
         clear the entry */
      if((index = ds_3gpp_pdn_limit_tbl_find_entry
           ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
             subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
      {
        ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
      }
#endif /*FEATURE_DATA_LTE*/

      /*------------------------------------------------------------------------
        Do a local clean up. This has to be done only if the PDN context / iface
        is not default
      ------------------------------------------------------------------------*/
      ds_pdn_cntxt_freeInstance(pdn_cntx_p);
      return;
    }
  
    if(ds_pdn_cntxt_store_bearer_context(pdn_cntx_p,(void *)bearer_context_p) != \
                                                DS3G_SUCCESS)
    {
      /*---------------------------------------------------------------------
        We should not hit this case of running out of placeholders for bearer
        during iface bring up. If this were to happen, it is probably because
        the bearer pointers were not cleaned up from a prior use of the PDN
        context.
      ---------------------------------------------------------------------*/
      //Post iface down
      DS_3GPP_MSG1_HIGH("Could not store the allocated bearer. Remove the entry "
                        "from the APN table and free the PDN 0x%x",
                        pdn_cntx_p);

      down_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;

      ps_iface_down_ind(&(ds_iface_p->ps_iface), &down_info);

      /*-----------------------------------------------------------------------
        Remove the entry from the APN table
      -----------------------------------------------------------------------*/
      if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
      }

#ifdef FEATURE_DATA_LTE
      /* Find an entry in the PDN Limit Table. If an entry is found,
         validate if the entry can be cleared at this point. If yes,
         clear the entry */
      if((index = ds_3gpp_pdn_limit_tbl_find_entry
           ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
             subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
      {
        ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
      }
#endif /*FEATURE_DATA_LTE*/
       
      //Deallocate bearer
      ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
  
      /*------------------------------------------------------------------------
        Do a local clean up. This has to be done only if the PDN context / iface
        is not default
      ------------------------------------------------------------------------*/
      ds_pdn_cntxt_freeInstance(pdn_cntx_p);
      return;
    } 
      
    // Increment the number of active bearer contexts in the PDN
    pdn_cntx_p->ds_pdn_context_dyn_p->num_active_bearer_context++;
      
    ds_pdn_cntxt_set_default_bearer(pdn_cntx_p, bearer_context_p);
  
    /*-----------------------------------------------------------------------
      Register this bearer with the PDN context. Set is_default as true since
      this is the default bearer for that PDN context.
    -----------------------------------------------------------------------*/
    ds_bearer_cntxt_register_pdn_context(
                                        pdn_cntx_p, 
                                      bearer_context_p,
                                    TRUE
                                  );
  
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
      /*-----------------------------------------------------------------------
      Allocate a PDN flow manager and bearer flow manager
      Find to see if a PDN has a flow manager associated, If not allocate a
      new flow manager.
  -----------------------------------------------------------------------*/
     flow_manager_p = ds_flow_manager_find_flow_manager(pdn_cntx_p);
     if (flow_manager_p != NULL)
     {
       DS_3GPP_MSG0_HIGH("Found a Flow manager associated with the PDN context");
     }
     else
     {
       DS_3GPP_MSG0_HIGH("Allocating a new PDN Flow Manager");
       if(ds_flow_manager_allocate_new_flow_manager(pdn_cntx_p) == NULL)
       {
         DS_3GPP_MSG0_ERROR("Unable to allocate Flow Manager");
         ds_3gpp_dyn_mem_hdlr_free_flow_mgr_dyn_mem(pdn_cntx_p);
         DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);	 
         return;
       }
     }

    bflow_manager_p = 
    ds_bearer_flow_manager_allocate_flow_manager(bearer_context_p);
    
      if(!ds_bearer_flow_manager_validate_flow_manager(bflow_manager_p))
      {
        ds_3gpp_dyn_mem_hdlr_free_bearer_flow_mgr_dyn_mem(bearer_context_p);
        DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
        return;
      }
      DS_3GPP_MSG0_HIGH("Allocated Flow Manager and bearer Flow Manager");    
       /*---------------------------------------------------------------------
        Update the PDN PTR.
      ---------------------------------------------------------------------*/
      bflow_manager_p->ds_bearer_flow_manager_dyn_p->pdn_ptr = pdn_cntx_p;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  
    /*-----------------------------------------------------------------------
      Bind the Phys link to the default flow of the Iface
    -----------------------------------------------------------------------*/
    ret_val = ps_flow_bind_phys_link(PS_IFACE_GET_DEFAULT_FLOW(&(ds_iface_p->ps_iface)),
                           &(bearer_context_p->phys_link));
    if(ret_val !=0 )
    {
      DATA_ERR_FATAL("Cannot bind phys link to default flow");
    }

    /*-----------------------------------------------------------------------
      Bring up the phys link
    -----------------------------------------------------------------------*/
    ret_val = ps_phys_link_up_cmd(&(bearer_context_p->phys_link),
                                  &ps_errno,
                                  &(pdn_cntx_p->ds_pdn_context_dyn_p->info));
          
    if (ret_val == 0)
    {
      DS_3GPP_MSG0_HIGH(" Bringing up phys link when already up / coming up:");
    }
    else if ( ps_errno != DS_EWOULDBLOCK)
    {
      DS_3GPP_MSG1_ERROR("Failed to bring up the physlink. Remove the PDN entry "
                        "from the APN table and free the PDN 0x%x",
                        pdn_cntx_p);

      ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(&(ds_iface_p->ps_iface)),
                          PS_EIC_NOT_SPECIFIED);
      ps_phys_link_gone_ind(&(bearer_context_p->phys_link));

      down_info.down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;

      ps_iface_down_ind(&(ds_iface_p->ps_iface), &down_info);

      /*-----------------------------------------------------------------------
        Remove the entry from the APN table
      -----------------------------------------------------------------------*/
      if(ds_pdn_cntxt_remove_entry_in_apn_table(pdn_cntx_p) == FALSE)
      {
        DS_3GPP_MSG0_ERROR("Invalid PDN Passed to remove apn table");
      }

#ifdef FEATURE_DATA_LTE
      /* Find an entry in the PDN Limit Table. If an entry is found,
         validate if the entry can be cleared at this point. If yes,
         clear the entry */
      if((index = ds_3gpp_pdn_limit_tbl_find_entry
           ((byte*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn,
             subs_id)) != DS_3GPP_PDN_LIMIT_TBL_INVALID_ENTRY)
      {
        ds_3gpp_pdn_limit_tbl_remove_entry(index, subs_id);
      }
#endif /*FEATURE_DATA_LTE*/

      /* Not needed since the phys link will be unbound un ps_flow_go_null_ind
        ps_flow_unbind_phys_link(
                        PS_IFACE_GET_DEFAULT_FLOW(&(ds_iface_p->ps_iface)));
                        */
  
      ds_pdn_cntxt_freeInstance(pdn_cntx_p);
      return;
    }
  }
  else
  {
    DS_3GPP_MSG0_HIGH("ds_pdn_cntxt_resend_pdn_conn_req: "
                     "Bearer already allocated. Invoking bearer bringup");
    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      return;
    }

    if (pdn_cntx_p->ds_pdn_context_dyn_p->info_null)
    {
      info_ptr = NULL;
    }
    else
    {
      info_ptr = &(pdn_cntx_p->ds_pdn_context_dyn_p->info);
    }
    ds_3gpp_bearer_cntxt_invoke_bearer_bringup ((uint32) (bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr),
                                            info_ptr);
  }

} /* ds_pdn_cntxt_resend_pdn_conn_req */
#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_PROCESS_PDN_INACTIVITY_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the PDN inactivity timer expires. This command cleans up the PDN context
  irrespective of whether it is the last one standing or otherwise.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_process_pdn_inactivity_cmd(ds_pdn_context_s* pdn_cntx_p)
{
  ds_3gpp_iface_s      *ds_3gpp_iface_p = NULL;
  int                  iface_index;
  int                  ret_val;
  int16                ps_errno;
  int                  bearer_index;
  ds_bearer_context_s  *bearer_context_p;
  boolean              data_transfer_occured = FALSE;
  uint8                ignore_inactivity_timer = FALSE;
  uint32               remaining_timer_val = 0;
  sys_modem_as_id_e_type    subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return;
  }
 /*-----------------------------------------------------------------------------
   This timer will be re-started in pooling timer expiry handler, Check if the 
   remaining timer value is non-zero if so do not process inactivity timer expiry 
   specific function
  -----------------------------------------------------------------------------*/
  remaining_timer_val = rex_get_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer));
  if (remaining_timer_val > 0) 
  {
    DS_3GPP_MSG0_ERROR("PDN inactivity timer re-started, returning !!!");
    return;
  }

  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
  {
    /*--------------------------------------------------------------------------
      Loop over all bearers and make them update the data transfer statictics
      This is needed to find out if any data has flown since the last polling
      timer expiry
    --------------------------------------------------------------------------*/
    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
        bearer_index++)
    {
      bearer_context_p =
        (ds_bearer_context_s*)pdn_cntx_p->ds_pdn_context_dyn_p->
        bearer_context_ptr_tbl[bearer_index];

      if(bearer_context_p == NULL)
      {
        /*-----------------------------------------------------------------------
          This entry in the bearer context table is NULL. Other entries could
          have non-NULL values.
        -----------------------------------------------------------------------*/
        continue;
      }
      if(ds_3gpp_bearer_cntxt_get_data_trfr_activity(bearer_context_p) == TRUE)
      {
        /*-----------------------------------------------------------------------
          Mark that data transfer has happened in this interval between polling
          timer expiry and inactivity timer expiry.
        -----------------------------------------------------------------------*/
        data_transfer_occured = TRUE;
      }
    }//loop over all bearers

    //See if this is the last PDN and if so check the inactivity timer.
    if (FALSE == dsPdnContext_is_any_other_normal_pdn_up(pdn_cntx_p, subs_id))
    {
      ignore_inactivity_timer = ds_3gpp_nv_manager_get_ignore_inactivity_timer(subs_id);        
      
      DS_3GPP_MSG1_HIGH("Last PDN. Flag IgnoreInactivityTimer is set to %d",
                         ignore_inactivity_timer);
    }

    if ((TRUE == data_transfer_occured) || (TRUE == ignore_inactivity_timer))
    {
      DS_3GPP_MSG1_HIGH("Data transfer occured. restarting inactivity timer: %ds",
                        pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
                        inactivity_timer_val);
      /*-------------------------------------------------------------------------
        Restart the PDN inactivity timer. We need not restart the polling timer.
        We can let it run freely.
      -------------------------------------------------------------------------*/
      (void)rex_set_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer), 
                          (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
                           inactivity_timer_val)*1000);
    }
    else
    {
      /*-------------------------------------------------------------------------
        Data inactivity established during the entire duration of inactivity
        timer.
        Loop over all ifaces and call active tear down
      -------------------------------------------------------------------------*/
      for(iface_index = 0; 
          iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
          iface_index++)
      {
        ds_3gpp_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->
          ds_3gpp_iface_tbl[iface_index];

        if (ds_3gpp_iface_p == NULL)
        {
          continue;
        }
        else
        {
          ret_val = ps_iface_active_tear_down_cmd(&(ds_3gpp_iface_p->ps_iface),
                                                  &ps_errno, NULL);

          if(ret_val == 0)
          {
            DS_3GPP_MSG0_HIGH("Iface in DOWN state already");
          }
          else if(ps_errno != DS_EWOULDBLOCK)
          {
            DS_3GPP_MSG1_HIGH("Error while tearing down the iface. Err Code: %d",
                     ps_errno);
          }
        } /* DS Iface is non-NULL */
      } /* for each iface in the PDN context */

      /*------------------------------------------------------------------------
        Change the state of the PDN context so that we can make an informed
        decision while processing iface down command
      ------------------------------------------------------------------------*/
      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
    } // data transfer occured check
  } /* if PDN context is in UP state */
  else
  {
    DS_3GPP_MSG1_ERROR("Ignore DS3GPP_PDN_INACTIVITY_TMR_EXP_CMD in state: %d",
                       pdn_cntx_p->ds_pdn_context_dyn_p->state);
  } /* PDN context not in UP state */
} /* ds_3gpp_pdn_cntx_process_pdn_inactivity_cmd */
#endif /* FEATURE_DATA_LTE */
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_PROCESS_POLLING_TMR_EXP_CMD

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the polling timer expires. This function queries the lower layers to figure
  out if there has been any data activity since the last polling timer expiry

  If there has been data activity, we restart the PDN inactivity timer
  Otherwise, we let it run.

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_process_polling_tmr_exp_cmd(ds_pdn_context_s* pdn_cntx_p)
{
  int                  bearer_index;
  ds_bearer_context_s  *bearer_context_p;
  boolean              data_transfer_occured = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  if(pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
  {
    /*--------------------------------------------------------------------------
      Loop over all bearers and make them update the data transfer statictics
    --------------------------------------------------------------------------*/
    for(bearer_index = 0;
        bearer_index < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT;
        bearer_index++)
    {
      bearer_context_p =
        (ds_bearer_context_s*)pdn_cntx_p->ds_pdn_context_dyn_p->
        bearer_context_ptr_tbl[bearer_index];

      if(bearer_context_p == NULL)
      {
        /*----------------------------------------------------------------------
          This entry in the bearer context table is NULL. Other entries could
          have non-NULL values.
        ----------------------------------------------------------------------*/
        continue;
      }
      if(ds_3gpp_bearer_cntxt_get_data_trfr_activity(bearer_context_p) == TRUE)
      {
        /*----------------------------------------------------------------------
          Mark that data transfer has happened in this polling interval
        ----------------------------------------------------------------------*/
        data_transfer_occured = TRUE;
      }
    }//loop over all bearers

    if(data_transfer_occured == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Data transfer occured. restart inactivity timer: %ds",
                        pdn_cntx_p->ds_pdn_context_dyn_p->
                        pdp_profile.inactivity_timer_val);
      /*------------------------------------------------------------------------
        Restart the PDN inactivity timer
      ------------------------------------------------------------------------*/
      (void)rex_set_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdn_inactivity_timer), 
                          (pdn_cntx_p->ds_pdn_context_dyn_p->
                           pdp_profile.inactivity_timer_val)*1000);
    }

    DS_3GPP_MSG0_HIGH("Starting polling timer for next polling period");
    /*------------------------------------------------------------------------
      Start the polling timer for the next interval
    ------------------------------------------------------------------------*/
    (void)rex_set_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->polling_timer), 
                        DS_3GPP_POLLING_INTERVAL);
  }//pdn context in UP state
  else
  {
    DS_3GPP_MSG1_HIGH("Ignore DS3GPP_PDN_POLLING_TMR_EXP_CMD in state: %d",
                       pdn_cntx_p->ds_pdn_context_dyn_p->state);
  }
} /* ds_3gpp_pdn_cntx_process_polling_tmr_exp_cmd */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_PROCESS_APN_PARAM_CHG_TMR_EXP_CMD

DESCRIPTION
The function is called when the apn param chg timer expires, This would imply that 
the client will not tear down the call. IT will be MH's responsibility 
to clean up the call. 

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_process_apn_param_chg_tmr_exp_cmd
(
   ds_pdn_context_s* pdn_cntx_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("In apn_param_chg_tmr_exp_cmd");

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

#ifdef FEATURE_DATA_LTE
   ds_eps_apn_param_change_cmd_hdlr(pdn_cntx_p->ds_pdn_context_dyn_p->
                                    pdp_profile_num);
#endif /* FEATURE_DATA_LTE */
  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_DO_SILENT_REDIAL_CMD_HDLR

DESCRIPTION 
On receiving the DO_SILENT_REDIAL IOCTL, MH will have to perform silent redial 
i.e send out a PDN disconnect and bring up the PDN again. 

PARAMETERS
  pdn_cntx_p - Pointer to the PDN context to be torn down

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_do_silent_redial_cmd_hdlr
(
   ds_pdn_context_s* pdn_cntx_p
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_3GPP_MSG0_HIGH("In apn_param_chg_tmr_exp_cmd");

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  (void)rex_clr_timer(&(pdn_cntx_p->ds_pdn_context_dyn_p->apn_param_chg_timer));
#ifdef FEATURE_DATA_LTE
  ds_eps_apn_param_change_cmd_hdlr(pdn_cntx_p->ds_pdn_context_dyn_p->
                                   pdp_profile_num);
#endif /* FEATURE_DATA_LTE */
  return;
}


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_PROCESS_PDN_INCOMING_CMD_HDLR

DESCRIPTION
  This function is called while processing the command enqueued to DS when
  the Auto answer is enabled. This command originates a PS call using 
  default profile.

PARAMETERS
  incom_call_info_ptr - Pointer to the incoming call info
  
DEPENDENCIES
  None.

RETURN VALUE
  0 --  for Success 
  -1 -- for Failure

SIDE EFFECTS
  None.

===========================================================================*/
static int ds_3gpp_pdn_cntx_process_pdn_incoming_cmd_hdlr
(
  const ds_3gpp_pdn_incom_call_info_type*  incom_call_info_ptr
)
{
  byte                             prof_num = DS_UMTS_DEF_SOCKET_PROFILE;
  uint32                           pdp_profile_num;
  ds_bearer_context_s*             bearer_context_p = NULL;
  int                              ret_val = 0;
  int                              bring_up_ret_val = -1;
  ds_umts_pdp_type_enum_type       ds_pdp_type = DS_UMTS_PDP_MAX;
  ds_umts_pdp_context_type*        prof_cntxt_info_ptr;
  acl_policy_info_type             *acl_policy_info = NULL;
  ip_pkt_info_type                 ip_info;
  ps_iface_type                    *v4_ps_iface_ptr= NULL;
  ps_iface_type                    *v6_ps_iface_ptr= NULL;
  ps_iface_type                    *assoc_iface_ptr = NULL;
  ds_cmd_type  *                   rej_cmd_ptr = NULL;
  int16                            ps_iface_errno = DS_EWOULDBLOCK;
  ds_3gpp_iface_s*                 ds_iface_p = NULL;
  ds_pdn_context_s                 *pdn_cntx_p = NULL;
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  ds_umts_pdn_context_s            *umts_pdn_cntxt_p=NULL;
#endif
  ds_3gpp_pdn_incom_call_info_type *local_incom_call_info_ptr = NULL;
  sys_modem_as_id_e_type            subs_id = ds3g_get_ps_subs_id();
  ps_iface_down_mh_event_info_type  down_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  down_info.down_reason = PS_NET_DOWN_REASON_VALIDATION_FAILURE;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(subs_id);
  down_info.bearer_tech.rat_value =  
    ds_3gpp_map_sys_mode_to_ds_sys_rat
      (ds3gpp_get_current_network_mode(subs_id));

  prof_cntxt_info_ptr = (ds_umts_pdp_context_type *)modem_mem_alloc(
        sizeof(ds_umts_pdp_context_type),
        MODEM_MEM_CLIENT_DATA );

  acl_policy_info = (acl_policy_info_type *)modem_mem_alloc(
                                     sizeof(acl_policy_info_type),
                                     MODEM_MEM_CLIENT_DATA_CRIT );

  if (prof_cntxt_info_ptr == NULL || acl_policy_info == NULL ) 
  {
    DS_3GPP_MSG0_ERROR("FAIL modem_mem_alloc for ds_umts_pdp_context_type");
    ret_val = -1;
  }
  /* Get default Socket profile number */
  if( (ret_val != -1) &&(DS_UMTS_PDP_SUCCESS != 
           ds_umts_get_pdp_profile_num_for_sock_calls_per_subs(
                         dsumts_subs_mgr_get_subs_id(ds3g_get_ps_subs_id()),
                         &prof_num)) )
  {
    DS_3GPP_MSG0_ERROR("Invalid default sock profile ");
    ret_val = -1;
  }

  pdp_profile_num = prof_num;
  /* check if the APN is already activated */
  if( (ret_val != -1) && 
      (incom_call_info_ptr->apn_name.valid == TRUE) &&
      (incom_call_info_ptr->apn_name.apn_addr_len == 0)  &&
      (TRUE == ds_pdn_cntxt_is_pdn_associated_for_apn (
                   (byte*)incom_call_info_ptr->apn_name.address, ds3g_get_ps_subs_id())))
  {
    DS_3GPP_MSG0_ERROR("APN s already activated ");
    ret_val = -1;
  }

  ds_pdp_type = incom_call_info_ptr->ds_pdp_type;
  if(ret_val != -1)
  {
    /*-------------------------------------------------------------------------
      Based on call type do a route get followed by iface bring up.
    -------------------------------------------------------------------------*/
    /* Update the ACL info policy */
    acl_init_policy_info(acl_policy_info);
    memset( acl_policy_info, 0, sizeof(acl_policy_info_type) );
    acl_policy_info->iface.kind = DSS_IFACE_NAME; 
    acl_policy_info->iface.info.name = DSS_IFACE_UMTS;
    acl_policy_info->policy_flag = DSS_IFACE_POLICY_SPECIFIED;
    acl_policy_info->pdp_info = (acl_pdp_ctx_cfg_type)pdp_profile_num;
    if(incom_call_info_ptr->apn_name.valid == TRUE)
    {
      acl_policy_info->apn.length = (uint8)incom_call_info_ptr->apn_name.apn_addr_len;

      if( acl_policy_info->apn.length > MAX_APN_ADR_LEN  )
      {
        DS_3GPP_MSG1_ERROR("Incorrect APN Address length: %d - truncate to MAX_APN_ADR_LEN and proceed", 
                            acl_policy_info->apn.length);
 	acl_policy_info->apn.length = (uint8)MIN(acl_policy_info->apn.length, MAX_APN_ADR_LEN);
      }
	  
      memscpy((acl_policy_info->apn.name),
               sizeof(acl_policy_info->apn.name),
	       incom_call_info_ptr->apn_name.address,
               acl_policy_info->apn.length);
    }

    /*-------------------------------------------------------------------------
      Bring_up has to be set to TRUE here so that the in_use flag is to TRUE
      when the IFACE gets assigned.
    -------------------------------------------------------------------------*/
    acl_policy_info->bring_up = TRUE;

    /* ip_info population */
    memset( &ip_info, 0, sizeof(ip_info) );

    switch(ds_pdp_type)
    {
      case DS_UMTS_PDP_IPV4:
#ifdef FEATURE_DUAL_IP_BEARER
      case DS_UMTS_PDP_IPV4V6: 
#endif /* FEATURE_DUAL_IP_BEARER */
      case DS_UMTS_PDP_PPP:
       ip_info.ip_vsn = IP_V4;
       ip_info.ip_hdr.v4.dest.ps_s_addr = ROUTE_DEFAULT_DST;
       acl_policy_info->ip_family = IPV4_ADDR;
       if ( route_get( &ip_info, 
                       acl_policy_info, 
                       FALSE, 
                       NULL, 
                       &v4_ps_iface_ptr ) < 0 )
       {
         DS_3GPP_MSG0_ERROR("route_get: no v4 iface available");
         v4_ps_iface_ptr = NULL;
         ret_val = -1 ;
       }
       else
       {
         DS_3GPP_MSG1_HIGH("V4 iface allocated: 0x%x",v4_ps_iface_ptr->name);
       }
#if defined (FEATURE_DATA_PS_IPV6) && defined (FEATURE_DUAL_IP_BEARER)
       if( DS_UMTS_PDP_IPV4V6 != ds_pdp_type )
       {
         break;
       }
#else
       break;
#endif /* FEATURE_DATA_PS_IPV6 && FEATURE_DUAL_IP_BEARER */

#ifdef FEATURE_DATA_PS_IPV6
     case DS_UMTS_PDP_IPV6:
       ip_info.ip_vsn = IP_V6;
       ip_info.ip_hdr.v4.dest.ps_s_addr = ROUTE_DEFAULT_DST;
       acl_policy_info->ip_family = IPV6_ADDR;
       if ( route_get( &ip_info, 
                       acl_policy_info, 
                       FALSE, 
                       NULL, 
                       &v6_ps_iface_ptr ) < 0 )
       {
         DS_3GPP_MSG0_ERROR("route_get: no v6 iface available");
         v6_ps_iface_ptr = NULL;
         ret_val = -1;
       }
       else
       {
         DS_3GPP_MSG1_HIGH("V6 iface allocated: 0x%x",v6_ps_iface_ptr->name);
       }
       break;
 #endif /*FEATURE_DATA_PS_IPV6*/
      default:
        ret_val = -1;
        break;
    }
  }
  if ( (ret_val != -1) &&
       (v6_ps_iface_ptr != NULL) &&
       (IFACE_DOWN == ps_iface_state(v6_ps_iface_ptr) ) )
  {
    /*---------------------------------------------------------------------
        Bring up the Um IFACE interface. If there is an error, 
        Send reject and return -1.
     ---------------------------------------------------------------------*/
    bring_up_ret_val  = ps_iface_bring_up_cmd( v6_ps_iface_ptr, 
                                &ps_iface_errno,
                                NULL ) == 0 ;

    if ( (bring_up_ret_val == -1) &&
         ( ps_iface_errno != DS_EWOULDBLOCK ) )
    {
      DS_3GPP_MSG2_ERROR("UM i/f 0x%x error %d in bring up", 
                (uint32)v6_ps_iface_ptr->name, ps_iface_errno);
      /* Return ERROR */
      ret_val = -1;
    }
  }
  if (ret_val != -1)
  {
    if ( (v4_ps_iface_ptr != NULL) &&
       (IFACE_DOWN == ps_iface_state(v4_ps_iface_ptr) ) )
    {
      /*---------------------------------------------------------------------
        Bring up the Um IFACE interface. If there is an error, 
        Send reject and return -1.
      ---------------------------------------------------------------------*/
      bring_up_ret_val  = ps_iface_bring_up_cmd( v4_ps_iface_ptr, 
                                  &ps_iface_errno,
                                  NULL ) == 0 ;

      if ( (bring_up_ret_val == -1) &&
           ( ps_iface_errno != DS_EWOULDBLOCK ) )
      {
        DS_3GPP_MSG2_ERROR("UM i/f 0x%x error %d in bring up", 
                  (uint32)v4_ps_iface_ptr->name, ps_iface_errno);
        /* Return ERROR */
        ret_val = -1;
      }
    }
  }

  if(ret_val != -1) 
  {
    if (v4_ps_iface_ptr != NULL)
    {
      if(PS_IFACE_IS_LOGICAL(v4_ps_iface_ptr))
      {
        /*--------------------------------------------------------------------
         assoc iface should be valid for EPC IFACE
        ---------------------------------------------------------------------*/
        assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(v4_ps_iface_ptr);
        if( !PS_IFACE_IS_VALID(assoc_iface_ptr))
        {
            DS_3GPP_MSG2_HIGH("EPC iface 0x%x:%d INVALID assoc iface.", 
                    v4_ps_iface_ptr->name,
                    v4_ps_iface_ptr->instance);
            ret_val = -1;
        }
      }
      else
      {
        assoc_iface_ptr = v4_ps_iface_ptr;
      }
    }
    else if (v6_ps_iface_ptr != NULL)
    {
      if(PS_IFACE_IS_LOGICAL(v6_ps_iface_ptr))
      {
        /*--------------------------------------------------------------------
        assoc iface should be valid
        ---------------------------------------------------------------------*/
        assoc_iface_ptr = PS_IFACE_GET_ASSOC_IFACE(v6_ps_iface_ptr);
        if( !PS_IFACE_IS_VALID(assoc_iface_ptr))
        {
          DS_3GPP_MSG2_HIGH("EPC iface 0x%x:%d INVALID assoc iface.", 
                    v6_ps_iface_ptr->name,
                    v6_ps_iface_ptr->instance);
          ret_val = -1;
        }
      }
      else
      {
        assoc_iface_ptr = v6_ps_iface_ptr;
      }
    }
  }
  if(ret_val != -1)
  {
    if(assoc_iface_ptr == NULL)
    {
      DS_3GPP_MSG0_ERROR("assoc_iface_ptr is NULL");
      ret_val = -1;
    }
    else
    {
      DS_3GPP_MSG1_HIGH("Ps_iface 0x%x is being brought up ",
                        (uint32)assoc_iface_ptr->name);
      /* Check for the PDN context */
      ds_iface_p = (ds_3gpp_iface_s*)(assoc_iface_ptr->client_data_ptr);
      ASSERT( ds_iface_p != NULL);
    
      pdn_cntx_p = (ds_pdn_context_s *)ds_iface_p->client_data_ptr;
    
      if (ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id) == FALSE)
      {
        ASSERT(0);
        ret_val = -1;
      }
    
      if (ret_val != -1)
      {
        /* Set PDN state to ANSWER  */
        pdn_cntx_p->ds_pdn_context_dyn_p->state  = DS_PDN_CONTEXT_STATE_ANSWER;
        DS_3GPP_MSG1_HIGH("pdn_cntx_p 0x%lx state is set to ANSWER",
                          (uint32)pdn_cntx_p);
    
        /*-----------------------------------------------------------------------
          Allocate a bearer context from the free pool of bearers. If no bearer 
          is available, we should fail the call.
        -----------------------------------------------------------------------*/
        bearer_context_p = ds_bearer_cntxt_allocate_new_bearer(subs_id);

        if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
        {
          ret_val = -1;
        }
        else if(ds_pdn_cntxt_store_bearer_context
                 (pdn_cntx_p, (void *)bearer_context_p) 
                    != DS3G_SUCCESS)
        {
          /*---------------------------------------------------------------------
            We should not hit this case of running out of placeholders for bearer
            during iface bring up. If this were to happen, it is probably because
            the bearer pointers were not cleaned up from a prior use of the PDN
            context.
          ---------------------------------------------------------------------*/
          //Post iface down
          DS_3GPP_MSG0_ERROR("Could not store the allocated bearer");
    
          //Deallocate bearer
          ds_bearer_cntxt_return_bearer_to_free_pool(bearer_context_p);
          ps_iface_down_ind(assoc_iface_ptr, &down_info);
          ret_val = -1;
        }
      }
    
      if(ret_val != -1)
      {
        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id = 
          incom_call_info_ptr->call_id;
    #if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
        umts_pdn_cntxt_p=( (ds_umts_pdn_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ]) );
        if(!dsUmtsPdnCntx_validate_pdn_context(umts_pdn_cntxt_p))
        {
          DS_3GPP_MSG0_ERROR("Umts PDN cntxt passed is not valid");
          ret_val = -1;
        }
        else
        {
         umts_pdn_cntxt_p->ds_umts_pdn_context_dyn_p->offered_mt_address =
                incom_call_info_ptr->pdp_addr;
        }
   #endif
      }
    }
  }

  /* reject the MT call when ret_val = -1 */
  if(ret_val == -1)
  {
    rej_cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_pdn_incom_call_info_type));
    if( (rej_cmd_ptr == NULL) || (rej_cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return -1;
    }

    rej_cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_MT_SEND_REJECT;
    local_incom_call_info_ptr = 
      (ds_3gpp_pdn_incom_call_info_type*)rej_cmd_ptr->cmd_payload_ptr;

    memscpy(local_incom_call_info_ptr,
            sizeof(ds_3gpp_pdn_incom_call_info_type),
            incom_call_info_ptr,
            sizeof(ds_3gpp_pdn_incom_call_info_type));

    ds_put_cmd(rej_cmd_ptr);

  }

  if ( prof_cntxt_info_ptr != NULL)
  {
    modem_mem_free((void *)prof_cntxt_info_ptr, MODEM_MEM_CLIENT_DATA );
  }

  if ( acl_policy_info != NULL)
  {
    modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
  }
    
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_PROCESS_PDN_MT_SEND_REJECT

DESCRIPTION
  This function sends a reject for a MT call when the auto answer
  fails.
  
PARAMETERS
  incom_call_info_ptr -  This is the incoming call info that 
                         that has the info like APN, PDP type,
                         call_id.
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_3gpp_pdn_cntx_process_pdn_mt_send_reject 
(
  const ds_3gpp_pdn_incom_call_info_type*  incom_call_info_ptr
)
{
  cm_call_id_type                call_id;               /* CM call id */
  ds_3gpp_pdn_call_info_type     mode_sp_info;  /* Mode specific info */

  call_id = incom_call_info_ptr->call_id;

  DS_3GPP_MSG3_MED("MT_SEND_REJECT: call_id [%d], call DS3G_MSH_HANGUP_CALL()!", 
                    call_id , 0, 0);

  /*-------------------------------------------------------------------------
    Set the MT flag to true in mode specific info and call hangup call
    which in turn calls cm_mm_call_cmd_end().
  -------------------------------------------------------------------------*/
  mode_sp_info.mt_flag = TRUE;

  if (ds3g_msh_hangup_call_ex(call_id ,
                         &mode_sp_info, ds3g_get_ps_subs_id()) == DS3G_FAILURE)
  {
    /*-----------------------------------------------------------------------
      The call has already ended.  Do nothing here, as the call_ended handler
      has been/will be called.  That will do most of the work. Since 
      application did not originate a call, do not set call_state of dsumtsps_
      call_tbl as call_instance for call_id is invalid.
    -----------------------------------------------------------------------*/
    DS_3GPP_MSG1_HIGH("MT_SEND_REJECT: Call already ended call_id :%d",call_id);
  }

} /* ds_3gpp_pdn_cntx_process_pdn_mt_send_reject */


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_PDN

DESCRIPTION
  This function tears down the PDN connection.
  
PARAMETERS 
  pdn_cntxt_p - PDN control block
  tear_down_reason - reason for initiating PDN tear down.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean ds_3gpp_pdn_cntx_teardown_PDN
( 
  ds_pdn_context_s *pdn_cntxt_p,
  ps_iface_net_down_reason_type tear_down_reason
)
{  
  int i = 0;
  int16 ps_errno = 0;
  boolean result = FALSE;

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {        
    for (i = 0; i< DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
    {
      ds_3gpp_iface_s *ds_3gpp_iface_p = pdn_cntxt_p->ds_pdn_context_dyn_p
        ->ds_3gpp_iface_tbl[i];
      if (ds_3gpp_iface_p != NULL)
      {
        int ret_val = ps_iface_active_tear_down_cmd(&(ds_3gpp_iface_p->ps_iface),
                                                    &ps_errno, NULL);

        if(ret_val == 0)
        {
          DS_3GPP_MSG0_HIGH("Iface in DOWN state already");
          result |= FALSE;
        }
        else if(ps_errno != DS_EWOULDBLOCK)
        {
          DS_3GPP_MSG1_HIGH("Error while tearing down the iface. Err Code: %d",
                             ps_errno);
          result |= TRUE;
        }
        else 
        {
          ds_3gpp_iface_p->ue_init_net_down_reason = tear_down_reason;
          result |= TRUE;
        }
      }
    }
  }
  /*-----------------------------------------------------------------
    Returns false, only if active teardown on both the ifaces fail
    -----------------------------------------------------------------*/
  return result;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_ROAMING_STATUS_CHG_CMD_HDLR

DESCRIPTION
  This function handles roaming status chg

PARAMETERS 
  roaming status
  subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_roaming_status_chg_cmd_hdlr
(
   sys_roam_status_e_type roam_status,
   sys_modem_as_id_e_type subs_id
);

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_PS_CMD

DESCRIPTION

  This function processes the PS commands received by the DS task.
  The  commands that can be received by the mode specific handler
  are as follows:

  DSUMTSPS_IFACE_UP_CMD: Command to bring up the UMTS Interface  
  DSUMTSPS_IFACE_DOWN_CMD: Command to tear down the UMTS Interface
  DSPDNCNTX_IFACE_IN_USE_CMD: Command to notify active iface use by app(s)
  DSPDNCNTX_IFACE_OUTOF_USE_CMD: Command to notify 
  DSUMTSPS_SEND_MT_REJECT_CMD: Command to send MT reject when the MT timer
                               expires

PARAMETERS
  cmd_ptr   - Ptr to the command information

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_process_ps_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  int result = -1;
  ds3g_roaming_ind_type             *roaming_ind_ptr = NULL;
  ds_3gpp_pdn_incom_call_info_type  *incom_call_info_ptr = NULL;
  ds_3gpp_pdn_call_info_type        *pdn_call_info_ptr = NULL;
  ds_3gpp_pdn_iface_info_type       *iface_call_info_ptr = NULL;
  sys_modem_as_id_e_type             subs_id = SYS_MODEM_AS_ID_NONE;    
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL cmd_ptr passed, return");
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  
  DS_3GPP_MSG1_MED("ds3gpp_processPSCmd: received %d", cmd_ptr->hdr.cmd_id);
  
  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_PDN_CONN_REQ_RESEND:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        /*----------------------------------------------------------------------
          This command sends out the PDN connectivity request
        ----------------------------------------------------------------------*/
        ds_pdn_cntxt_resend_pdn_conn_req((ds_pdn_context_s*)
                             (*((ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr)));
      }
      break;

    case DS_CMD_3GPP_IFACE_UP:

      if (cmd_ptr->cmd_payload_ptr != NULL)
      {

        pdn_call_info_ptr = 
          (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

#ifdef FEATURE_DATA_TEST_LOOPBACK

       if (dsumtsps_loopback_test_pc_is_active() == TRUE) {
          dsumtsps_loopback_iface_up_cmd_hdlr(
             (ps_iface_type*)(pdn_call_info_ptr->user_data),
             pdn_call_info_ptr->info_ptr);
        } 
        else {
          ds_pdn_cntxt_ifaceUpCmdHdlr(
             (ps_iface_type*)(pdn_call_info_ptr->user_data),
             pdn_call_info_ptr->info_ptr);
        }
#else
        ds_pdn_cntxt_ifaceUpCmdHdlr(
           (ps_iface_type*)(pdn_call_info_ptr->user_data),
           pdn_call_info_ptr->info_ptr);
#endif
      }
      break;

    case DS_CMD_3GPP_IFACE_DOWN:

      /*---------------------------------------------------------------------
        Command to tear down the UMTS Interface
        the parameter is actually iface_inst and not call_instance
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        pdn_call_info_ptr = 
          (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;
        ds_pdn_cntxt_ifaceDownCmdHdlr((ps_iface_type*)
                                      (pdn_call_info_ptr->user_data), 
                                       pdn_call_info_ptr->info_ptr);
      }
      break;

    case DS_CMD_3GPP_IFACE_IN_USE:
      /*---------------------------------------------------------------------
        Command to notify iface when it is being actively used by an APP
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        iface_call_info_ptr = 
          (ds_3gpp_pdn_iface_info_type*)cmd_ptr->cmd_payload_ptr;

        if(!PS_IFACE_IS_VALID(iface_call_info_ptr->iface_ptr))
        {
          DS_3GPP_MSG1_ERROR("Invalid iface ptr %d",
                             iface_call_info_ptr->iface_ptr);
          ASSERT(0);
          return;
        }
        ds_pdn_cntxt_ifaceInUseCmdHdlr(
                    (ps_iface_type*)(iface_call_info_ptr->iface_ptr));
      }
      break;

    case DS_CMD_3GPP_IFACE_OUT_OF_USE:
      /*---------------------------------------------------------------------
        Command to notify iface when it is not actively being used by an APP
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        iface_call_info_ptr = 
          (ds_3gpp_pdn_iface_info_type*)cmd_ptr->cmd_payload_ptr;

        if(!PS_IFACE_IS_VALID(iface_call_info_ptr->iface_ptr))
        {
          DS_3GPP_MSG1_ERROR("Invalid iface ptr %d",iface_call_info_ptr->iface_ptr);
          ASSERT(0);
          return;
        }
        ds_pdn_cntxt_ifaceOutofUseCmdHdlr(
                    (ps_iface_type*)(iface_call_info_ptr->iface_ptr),
                     iface_call_info_ptr->is_companion_iface_in_use,
                     iface_call_info_ptr->ota_teardown,
                     iface_call_info_ptr->info_ptr);
      }
      break;

    case DS_CMD_3GPP_BRIDGE_INTERFACE_SETUP:
      result = ds_pdn_cntxt_setupBridgeInterfaceCmdHdlr(cmd_ptr);
      if(result != 0)
      {
        DS_3GPP_MSG0_MED("ds3gpp_processPSCmd: Could not process "
                                  "setup_bridge_interface_cmd");
      }
      break;

    case DS_CMD_3GPP_IFACE_BRING_UP_RSP:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        incom_call_info_ptr = 
          (ds_3gpp_pdn_incom_call_info_type*)cmd_ptr->cmd_payload_ptr;
        result = 
          ds_3gpp_pdn_cntx_process_pdn_incoming_cmd_hdlr(incom_call_info_ptr);
        if(result != 0)
        {
          DS_3GPP_MSG0_MED("ds3gpp_processPSCmd: Could not process "
                           "pdn_incoming_cmd_hdlr");
        }
      }
      break;

    case DS_CMD_3GPP_MT_SEND_REJECT:
      /*---------------------------------------------------------------------
        Command to send a reject for a MT request which is auto answered.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        incom_call_info_ptr = 
          (ds_3gpp_pdn_incom_call_info_type*)cmd_ptr->cmd_payload_ptr;
        ds_3gpp_pdn_cntx_process_pdn_mt_send_reject(incom_call_info_ptr);
      }
      break;

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA) || \
    defined(FEATURE_GSM_GPRS) || defined(FEATURE_DATA_LTE)
    case DS_CMD_3GPP_SYS_CHG_SS_INFO:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        DS_3GPP_MSG3_HIGH("ds3gpp_processPSCmd: received sys_chg_ss_info_cmd", 
                          0,0,0);

        ds_3gpp_pdn_cntxt_sys_chg_ss_info_cmd_hdlr(
           (ds3g_sys_chg_ss_info_type*)cmd_ptr->cmd_payload_ptr );
      }
      break;
#endif/* (FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA) || 
         (FEATURE_GSM_GPRS) || (FEATURE_DATA_LTE) */

    case DS_CMD_3GPP_PDN_POLLING_TMR_EXP:
      /*---------------------------------------------------------------------
        Command to process polling timer expiry.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_cntx_process_polling_tmr_exp_cmd((ds_pdn_context_s*)
                                         (*((void**)cmd_ptr->cmd_payload_ptr)));
      }
      break;

    case DS_CMD_3GPP_DO_SILENT_REDIAL:
      /*---------------------------------------------------------------------
        Command to process apn param chg expiry.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_cntx_do_silent_redial_cmd_hdlr((ds_pdn_context_s*)
                                         (*((void**)cmd_ptr->cmd_payload_ptr)));
      }
      break;

    case DS_CMD_3GPP_PDN_APN_PARAM_CHG_TMR_EXP:
      /*---------------------------------------------------------------------
        Command to process apn param chg expiry.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_cntx_process_apn_param_chg_tmr_exp_cmd((ds_pdn_context_s*)
                                         (*((void**)cmd_ptr->cmd_payload_ptr)));
      }
      break;
#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_PDN_INACTIVITY_TMR_EXP:
      /*---------------------------------------------------------------------
        Command to process inactivity timer expiry.
      ---------------------------------------------------------------------*/
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_cntx_process_pdn_inactivity_cmd((ds_pdn_context_s*)
                                         (*((void**)cmd_ptr->cmd_payload_ptr)));
      }
      break;
#endif /* FEATURE_DATA_LTE */
    case DS_CMD_3GPP_MMGSDI_CARD_ERROR:
      /*---------------------------------------------------------------------
        Command to clear INFINITE THROTTLING
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG0_HIGH("At mmgsdiif card error ev");
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        subs_id = (sys_modem_as_id_e_type)
                  (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr));

        /*-------------------------------------------------------------------- 
          As per REL 11, Clear the network override handler tbl here.
          Network override hdlr will have to be cleared when SIM ERROR EV occurs
        ---------------------------------------------------------------------*/
        ds_3gpp_network_override_hdlr_reset(subs_id);

        ds_3gpp_pdn_throt_clear_infinite_throttling(subs_id,FALSE);

        ds_3gpp_pdn_throt_clear_t3396_on_usim_removal(subs_id, FALSE);
      }
      else
      {
        DS_3GPP_MSG0_HIGH("cmd payload pte is null in mmgsdiif");
      }
      break;

#ifdef FEATURE_DATA_TE_MT_PDP
    case DS_CMD_3GPP_MT_TE_ANS_TIMEOUT:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        /* This is DS task context, post event to MT-UMSM */
        DS_3GPP_MSG1_MED("PROCESS_PS_CMD: post CALL_ANS_TIMEOUT_EV to MT_UMSM, "
                         "mt_index [%d]!", 
                         (uint8)(*((uint8*)cmd_ptr->cmd_payload_ptr)));

        dsumtsps_mt_umsm_post_ev( (uint8)(*((uint8*)cmd_ptr->cmd_payload_ptr)), 
                                  MT_UMSM_CALL_ANS_TIMEOUT_EV, NULL );
      }
      break;
#endif /*FEATURE_DATA_TE_MT_PDP*/

    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF :
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        /* command to tear down the PDN connection because of invalid PCSCF */      
        (void)ds_3gpp_pdn_cntx_teardown_PDN(
           (ds_pdn_context_s*)cmd_ptr->cmd_payload_ptr,   
           PS_NET_DOWN_REASON_INVALID_PCSCF_ADDR);          
      }
      break;

    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_DNS_ADDR :
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        /* command to tear down the PDN connection because of invalid PCSCF */      
        (void)ds_3gpp_pdn_cntx_teardown_PDN(
           (ds_pdn_context_s*)cmd_ptr->cmd_payload_ptr,   
           PS_NET_DOWN_REASON_INVALID_DNS_ADDR);          
      }
      break;

    case DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF_DNS_ADDR :
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        /* command to tear down the PDN connection because of invalid PCSCF */      
        (void)ds_3gpp_pdn_cntx_teardown_PDN(
           (ds_pdn_context_s*)cmd_ptr->cmd_payload_ptr,   
           PS_NET_DOWN_REASON_INVALID_PCSCF_DNS_ADDR);          
      }
      break;

    case DS_CMD_3GPP_IFACE_UP_EMERGENCY:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        pdn_call_info_ptr = 
          (ds_3gpp_pdn_call_info_type*)cmd_ptr->cmd_payload_ptr;

        ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr(
           (ps_iface_type*)(pdn_call_info_ptr->user_data),
           pdn_call_info_ptr->info_ptr);
      }
      break;

    case DS_CMD_3GPP_PDN_REMOVE_UNUSED:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        ds_3gpp_pdn_cntxt_remove_unused_pdn((ds_pdn_context_s*)
                             (*((ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr)));
      }
      break;

    case DS_CMD_3GPP_PDN_THROT_RESET:
    	if(cmd_ptr->cmd_payload_ptr != NULL && 
           ds_3gpp_throt_sm_is_cc_sp_throttling_enabled ((sys_modem_as_id_e_type)
           (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr))) == TRUE )
    	{
        ds_3gpp_pdn_throt_reset_pdn_throttle(
           (sys_modem_as_id_e_type)
           (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr)));
#ifdef FEATURE_DATA_LTE
        ds_eps_pdn_throt_sm_clear_plmn_blocking(
           (sys_modem_as_id_e_type)
           (*((sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr)));    
#endif /* FEATURE_DATA_LTE */ 
    	}
    	break;

    case DS_CMD_3GPP_ROAMING_STATUS_CHG:
      if(cmd_ptr->cmd_payload_ptr != NULL)
      {
        roaming_ind_ptr = (ds3g_roaming_ind_type*)cmd_ptr->cmd_payload_ptr;
        ds_3gpp_pdn_cntxt_roaming_status_chg_cmd_hdlr
            (roaming_ind_ptr->gw_roam_status, 
             roaming_ind_ptr->subscription_id);
      }
      break;

    case DS_CMD_3GPP_CM_NOTIFY:
      ds_3gpp_pdn_cntxt_process_cm_notify_cmd(cmd_ptr);
      break;

#ifdef FEATURE_DATA_LTE
    case DS_CMD_3GPP_MMGSDI_UICC_3G_RESET:
      ds_3gpp_pdn_cntxt_process_mmgsdi_uicc_3g_reset_cmd(cmd_ptr);
      break;
#endif /* FEATURE_DATA_LTE */

    case DS_CMD_3GPP_THERMAL_MITIGATION:
      ds_3gpp_pdn_cntxt_process_thermal_mitigation_cmd(cmd_ptr);
      break;

    default:
      DS_3GPP_MSG1_HIGH("ds3gpp_processPSCmd: ignored unexpected cmd: %d", 
               cmd_ptr->hdr.cmd_id);
      break;
  } /* switch( cmd_id ) */

} /* ds_pdn_cntxt_processPSCmd() */

/*===========================================================================
FUNCTION dsPdnContext_getProfilePtr

DESCRIPTION
  This function allocates unused pdn context.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context

SIDE EFFECTS
  None.

VRK:TODO Make this a macro
===========================================================================*/
ds_umts_pdp_profile_type *ds_pdn_cntxt_getProfilePtr
( 
  ds_pdn_context_s*           pdn_cntx_p
)
{ 

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    DATA_ERR_FATAL("PDN cntxt passed is not valid");
  }

  return &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile);
  
}

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PS_IFACE_V4_PARAMS 
 
DESCRIPTION
  This function updates IPV4 param in PS IFace structure with Primary DNS,
  secondary DNS and Gateway Address

PARAMETERS 
  Iface_ptr : PS IFACE ptr
  decode_ps_param : IPCP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  return TRUE:  Successfully updated PS IFACE structure
  return FALSE: UnSuccessful in updating PS IFACE structure
 
SIDE EFFECTS 
  None 
===========================================================================*/
boolean ds_3gpp_update_ps_iface_v4_params
(
  ps_iface_type           *iface_ptr,
  dsumtsps_ipcp4_options   decode_param_v4
)
{
  /*--------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v4(iface_ptr))
  {
    /*-----------------------------------------------------------------------
      Set the DNS, NBNS, GW address for the iface. Also set the PPP options.
    -----------------------------------------------------------------------*/
    iface_ptr->v4_net_info.primary_dns.ps_s_addr   =
            ps_htonl(decode_param_v4.primary_dns);
    iface_ptr->v4_net_info.secondary_dns.ps_s_addr =
            ps_htonl(decode_param_v4.secondary_dns);
    iface_ptr->v4_net_info.primary_nbns.ps_s_addr   =
            ps_htonl(decode_param_v4.primary_nbns);
    iface_ptr->v4_net_info.secondary_nbns.ps_s_addr =
            ps_htonl(decode_param_v4.secondary_nbns);
    iface_ptr->v4_net_info.gateway.ps_s_addr =
            ps_htonl(decode_param_v4.gateway_addr);


    DS_3GPP_MSG3_HIGH("Setting up Type IP V4 PS Iface:           \
                       Gateway address for type IP call :%lx,    \
                       PDNS address for type IP call :%lx,       \
                       SDNS address for type IP call :%lx",      \
                       (unsigned long)decode_param_v4.gateway_addr,  \
                       (unsigned long)decode_param_v4.primary_dns,   \
                       (unsigned long)decode_param_v4.secondary_dns);

    return TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid PS IFACE ptr being passed");
    return FALSE;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PS_IFACE_V6_PARAMS 
 
DESCRIPTION
  This function updates IPV6 param in PS IFace structure with Primary DNS,
  secondary DNS and Gateway Address

PARAMETERS 
  Iface_ptr : PS IFACE ptr
  decode_ps_param : IPCP option parameters
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  return TRUE:  Successfully updated PS IFACE structure
  return FALSE: UnSuccessful in updating PS IFACE structure
 
SIDE EFFECTS 
  None 
===========================================================================*/
boolean ds_3gpp_update_ps_iface_v6_params
(
  ps_iface_type           *iface_ptr,
  dsumtsps_ipv6cp_options  decode_param_v6
)
{
  /*--------------------------------------------------------------------------*/
  if(ps_iface_addr_family_is_v6(iface_ptr))
  {
    /*-------------------------------------------------------------------------
      Set the DNS, NBNS, GW address for the iface. Also set the PPP options.
    -------------------------------------------------------------------------*/
    iface_ptr->v6_net_info.primary_dns   = decode_param_v6.primary_dns;
    iface_ptr->v6_net_info.secondary_dns = decode_param_v6.secondary_dns;

    /*lint -e572*/ 
    DS_3GPP_MSG2_HIGH("PDNS address for type IPV6 call :%lx%lx",
    ps_ntohll(decode_param_v6.primary_dns.ps_s6_addr64[0]),
    ps_ntohll(decode_param_v6.primary_dns.ps_s6_addr64[1]));

    DS_3GPP_MSG2_HIGH("SDNS address for type IPV6 call :%lx%lx",
    ps_ntohll(decode_param_v6.secondary_dns.ps_s6_addr64[0]),
    ps_ntohll(decode_param_v6.secondary_dns.ps_s6_addr64[1]));
    /*lint +e572*/ 
    return TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Iface passed not IPV6");
    return FALSE;
  }
}

/*===========================================================================
FUNCTION ds_pdn_cntxt_allocBearerCntx

DESCRIPTION
  This function allocates an unused bearer cntx associated with the specified
  pdn cntx   

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated bearer context

SIDE EFFECTS
  None.

===========================================================================*/
void *ds_pdn_cntxt_alloc_bearer_cntx
( 
  ds_pdn_context_s *pdn_cntx_p,
  cm_call_id_type   call_id
)
{   
  uint32              phys_link;
  ds_bearer_context_s *bearer_cntx_p;
  
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return NULL;
  }

  for(phys_link = 0; phys_link < DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT; phys_link++)
  {
    bearer_cntx_p = pdn_cntx_p->ds_pdn_context_dyn_p->bearer_context_ptr_tbl[phys_link];
    if(ds_bearer_cntx_validate_bearer_context(bearer_cntx_p) &&
        (bearer_cntx_p->ds_bearer_context_dyn_p->state == DS_BEARER_CONTEXT_STATE_DOWN))
    {
        break;
    }
  }

  if (phys_link == DS_3GPP_MAX_BEARER_CONTEXT_PER_PDN_CONTEXT) 
  {
    DS_3GPP_MSG0_ERROR("No free bearer context available");
    return NULL;
  }

  if(!ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------
    Update the active bearer cnt
  -----------------------------------------------------------------------*/      
  if(bearer_cntx_p->ds_bearer_context_dyn_p->is_default)
  {
    pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr = bearer_cntx_p;
  }
  ds_bearer_cntxt_alloc(bearer_cntx_p,call_id);
  return bearer_cntx_p;  
}

/*===========================================================================
FUNCTION DSUMTSPS_FLOW_IOCTL_HDLR

DESCRIPTION
  Flow ioctl Callback called from application context for secondary PDP
  context activation. Central function for embedded applications deactivate a 
  secondary context,resume a dormant secondary context or modify qos 
  parameters of a secondary context.

PARAMETERS
  *flow_ptr         - Ptr to the flow to operate on (validated by calling fn)
  ioctl_name        - Type of request
  *argval_ptr        - QoS specified by the application (validated by calling fn)
  *ps_errno         - Ptr to error code (if any)


DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
   
SIDE EFFECTS

===========================================================================*/
int ds_pdn_cntxt_flow_ioctl_hdlr
(
  ps_flow_type            *flow_ptr,
  ps_flow_ioctl_type      ioctl_name,
  void                    *argval_ptr,
  int16                   *ps_errno
  /*lint -esym(715,argval_ptr) */
)
{
/* VRK:TODO Move to Flow Cntx */
  return 0;
} /* dsumtsps_flow_ioctl_hdlr() */
  /*lint +esym(715,argval_ptr) */



/*===========================================================================
FUNCTION ds_pdn_cntxt_setupIPv4PSIface

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setup_ipv4_ps_iface
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,                
  cm_end_params_s_type           *end_params 
)
{   
  ps_iface_type *iface_ptr;                   /* Interface ptr for the call*/
  ps_ip_addr_type ip_addr;                    /* Ip address                */
  ds_3gpp_iface_s *ds3gpp_iface_p;
  boolean result = DS3G_SUCCESS;
  ds_dsd_apm_ind_pay_load   ind_payload;  
  sys_modem_as_id_e_type    subs_id=SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }  

  DS_3GPP_MSG0_HIGH("Setting up Type IP V4 PS Iface");

  do
  {
  
  /*-------------------------------------------------------------------------
    Get the ps iface ptr for this call instance    
  -------------------------------------------------------------------------*/
  ds3gpp_iface_p = 
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
                                                [DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

  iface_ptr = &(ds3gpp_iface_p->ps_iface);    


  if ((ds_3gpp_pdp_addr_p->pdp_addr.pdp_addr_ipv4 == 0)&&
      (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.ipv4_addr_alloc != 
                                        DS_UMTS_PDP_IPV4_ADDR_ALLOC_DHCPV4))
  {
      DS_3GPP_MSG0_ERROR("Invalid PDP Address for type IP call: 0 ");    
      DS_3GPP_APM_SET_INVALID_V4_IPADDR(ind_payload, PS_NET_DOWN_REASON_NOT_SPECIFIED);
      result =  DS3G_FAILURE;
      break;
  }

    DS_3GPP_MSG1_HIGH("PDP address for type IP call :%lx",
              ps_ntohl(ds_3gpp_pdp_addr_p->pdp_addr.pdp_addr_ipv4));

    DS_3GPP_APM_SET_VALID_V4_IPADDR(ind_payload);

  ip_addr.type = IPV4_ADDR;
  ip_addr.addr.v4.ps_s_addr = ds_3gpp_pdp_addr_p->pdp_addr.pdp_addr_ipv4;

  if(ps_iface_set_addr(iface_ptr, &ip_addr)!=0)
  {
    DS_3GPP_MSG1_HIGH("Setting address failed for iface_ptr: %d",iface_ptr);
      result =  DS3G_FAILURE;;
      break;
  }
  } while (0);

  if ( ( TRUE == (ds_pdn_cntxt_is_default(pdn_cntx_p)) &&
       ind_payload.ind_field_mask != 0))
  {
    DS_3GPP_APM_SET_PROFILE_ID(ind_payload, (uint16)pdn_cntx_p->
                               ds_pdn_context_dyn_p->pdp_profile_num);
    ind_payload.subs_id =  subs_id;
    (void)ds_dsd_apm_ind(DS_DSD_APM_IP_ADDRESS_IND, &ind_payload);
  }

  return result;
  
} /* ds_pdn_cntxt_setupIPv4PSIface() */

#ifdef  FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION ds_pdn_cntxt_setupIPv6PSIface

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setupIPv6PSIface
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,                
  cm_end_params_s_type           *end_params 
)
{ 
  ps_iface_type *iface_ptr;                   /* Interface ptr for the call*/
  ds_3gpp_iface_s *ds3gpp_iface_p = NULL;
  uint64 iid = (uint64)0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Setting up Type PDP-IP stack");

  /*-------------------------------------------------------------------------
    Get the ps iface ptr for this call instance    
  -------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  ds3gpp_iface_p = 
    pdn_cntx_p->ds_pdn_context_dyn_p->
    ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

  iface_ptr = &(ds3gpp_iface_p->ps_iface);

  if(ps_iface_addr_family_is_v6(iface_ptr))
  {
    /*-------------------------------------------------------------------------
     Get the IID from the profile. We stored it in profile as a part of 
     activateBearerInd
    -------------------------------------------------------------------------*/

    //iid = profile_ptr->context.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET];
    iid = ds_3gpp_pdp_addr_p->pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[DS_3GPP_IPV6_IID_OFFSET];
      
    if (iid == 0)
    {
      DS_3GPP_MSG0_ERROR("Invalid iid  for type IPV6 call: 0 ");
      return DS3G_FAILURE;
    }
    else
    {
      DS_3GPP_MSG2_HIGH("PDN cntx Inst:%d iid for type IPV6 call :%lx",
               (uint32)(((ds_3gpp_iface_s*)\
                           iface_ptr->client_data_ptr)->client_data_ptr),\
               ps_ntohll(iid));
    }

    if(PS_IFACE_SET_IP_V6_IID(iface_ptr, iid) == FALSE)
    {
      DS_3GPP_MSG1_ERROR("IPv6 IID Configuration Failed for Iface 0x%x",iface_ptr);
      return DS3G_FAILURE;
    }
  }
  else
  {
   DATA_ERR_FATAL("Invalid iface");
  }

  return ( DS3G_SUCCESS );
  
} /* ds_pdn_cntxt_setupIPv6PSIface() */

#endif /*FEATURE_DATA_PS_IPV6*/

/*===========================================================================
FUNCTION ds_pdn_cntxt_decodeProtocolParams

DESCRIPTION
  This function decodes the protocol parameters received for a PDP-IP call.

PARAMETERS
    call_id        - Call id of the call in progress
   *mode_info_ptr  - Pointer to the mode-specific call information provided
                     by Call Manager
DEPENDENCIES
  None.

RETURN VALUE
  Integer bitmask representing the decoded packets 

SIDE EFFECTS
  None.

===========================================================================*/
uint32 ds_pdn_cntxt_decodeProtocolParams
(
  ds_3gpp_iface_s                *ds3gpp_iface_v4_p,
                                      /* Index into array of call states  */
  ds_3gpp_iface_s                *ds3gpp_iface_v6_p,
  const protocol_cfg_options_T   *protocol_config_options_p  
)
{
  word    num_pids    = 0;                       /* Number of Protocol IDs */
  word    protocol_id = 0;                                  /* Protocol ID */
  boolean valid;                          /* Protocol options valid or not */
  byte    index       = 0;                       /* Protocol options index */
#ifdef FEATURE_DATA_PS_IPV6
  int     num_dns_ipv6 = 0;
#endif
  uint32     num_dns_ipv4 = 0;
  ds_pdn_context_s      *pdn_cntx_p = NULL;  /* Ptr to PDN context of the call */
  ps_iface_type         *ps_iface_v4_p = NULL;   /* v4 Interface ptr for the call */
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type         *ps_iface_v6_p = NULL;   /* v6 Interface ptr for the call */
#endif
  errno_enum_type       ps_errno;
#ifdef FEATURE_DATA_IMS
  uint32                num_v4_pcscf_addresses = 0; /* number of v4 pcscf address*/
  uint32                num_v6_pcscf_addresses = 0; /* number of v6 pcscf address*/
  ps_ip_addr_type       pcscf_v4_addr_array[MAX_PCSCF_ADDRESSES];
  ps_ip_addr_type       pcscf_v6_addr_array[MAX_PCSCF_ADDRESSES];
                                        /* Array of pcscf address pointers */
#endif
  uint32                decode_mask = 0;
  boolean               profile_pco_id_configured = FALSE;
                           /* Bit vector representing decoded packet types */
  ds3gpp_ipcpv4v6_options_s decode_ipcp_param;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_3GPP_MSG0_HIGH("Decode protocol param");
  memset(&decode_ipcp_param, 0, sizeof(ds3gpp_ipcp_options_s));

  /*-------------------------------------------------------------------------
    Get the pointer to PDN context from the DS Iface, in the dual IP bearer
    case, both ifaces should point to the same PDN context
  -------------------------------------------------------------------------*/
  if (ds3gpp_iface_v4_p != NULL)
  {
    pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_v4_p->client_data_ptr);
    ps_iface_v4_p = &(ds3gpp_iface_v4_p->ps_iface);
  }
#ifdef FEATURE_DATA_PS_IPV6
  if (ds3gpp_iface_v6_p != NULL)
  {
    ps_iface_v6_p = &(ds3gpp_iface_v6_p->ps_iface);
    if(pdn_cntx_p == NULL)
    {
      pdn_cntx_p = (ds_pdn_context_s*)(ds3gpp_iface_v6_p->client_data_ptr);
    }    
  }
#endif
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    DATA_ERR_FATAL("INVALID pdn context pointer in ds_pdn_cntxt_decodeProtocolParams");
  }
  
  valid = protocol_config_options_p->valid;

  if (valid == FALSE)
  {
    DS_3GPP_MSG1_HIGH("decode params pdn cntx:%x : Valid = FALSE",
              pdn_cntx_p);
    return decode_mask;
  }

  memset((void*)pcscf_v6_addr_array, 0, sizeof(ps_ip_addr_type)*MAX_PCSCF_ADDRESSES);
  memset((void*)pcscf_v4_addr_array, 0, sizeof(ps_ip_addr_type)*MAX_PCSCF_ADDRESSES);
  memset(&decode_ipcp_param, 0, sizeof(ds3gpp_ipcpv4v6_options_s));
  /*-------------------------------------------------------------------------
    Get the number of protocol IDs received. Max is NUM_PROTOCOL_ID.
  -------------------------------------------------------------------------*/
  num_pids = protocol_config_options_p->num_protocol_ids;

  /*-------------------------------------------------------------------------
    Process the protocol IDs received. The only protocol Identifer that
    is decoded is 0x8021 (IPCP).
  -------------------------------------------------------------------------*/
  while (num_pids > 0)
  {
    /*----------------------------------------------------------------------
      Get the Protocol ID.
    ----------------------------------------------------------------------*/
    protocol_id = protocol_config_options_p->
              procol_info[index].protocol_id[0] << 8;
    protocol_id |= protocol_config_options_p->
              procol_info[index].protocol_id[1];

    DS_3GPP_MSG3_HIGH("For PDN cntx %x Decode: Protocol ID %d",
             pdn_cntx_p,protocol_id,0);

    switch (protocol_id)
    {
    case PPP_IPCP_PROTOCOL:
        /* VRK: TODO add the check for secondary 
       if(dsumtsps_call_tbl->secondary_flag == TRUE)
       {
         DS_3GPP_MSG3_HIGH("Proto ID not valid for seccall: PDN cntx:%x Decode: Protocol ID %d",
              pdn_cntx_p,protocol_id,0);
         return DS3G_SUCCESS;
       }
       */
       if (ds3gpp_iface_v4_p != NULL)
       {
          /*-----------------------------------------------------------------
          IPCP Protocol (0x8021). Decode the packet and store the options
          in the call specific ipcp_options structure.          
          -----------------------------------------------------------------*/
         if 
         (
           ds3gpp_decode_ipcp_pkt(
              &protocol_config_options_p->
              procol_info[index].protocol_contents[0],
              protocol_config_options_p->
              procol_info[index].protocol_len,
              &(decode_ipcp_param.ipcp4_options)
              )!= DS3G_SUCCESS
         )
         {
           DS_3GPP_MSG1_HIGH("Could not decode ipcp pkt for pdn cntx %x",
                              pdn_cntx_p);
           decode_mask |= DS_3GPP_PCO_DECODE_FAILED_MASK; 
           return decode_mask;
         }
         ds_3gpp_update_ps_iface_v4_params(ps_iface_v4_p,decode_ipcp_param.ipcp4_options);
         decode_mask |= DS_3GPP_IPCP_PKT_DECODED_MASK; 
       }
       else
       {
         DS_3GPP_MSG0_ERROR("Ignoring V4 ipcp pkt for V6 only call ");
       }
       break;
    case IPV4_DNS_CONTAIN_ID:
       if (ds3gpp_iface_v4_p != NULL)
       {
          /*------------------------------------------------------------------
            IPV4 DNS (0x000D). Decode the packet and store the options
            in the call specific ipv4 options          
          ------------------------------------------------------------------*/
          ds3gpp_decode_ipv4_pkt(
              &protocol_config_options_p->
              procol_info[index].protocol_contents[0],
              protocol_config_options_p->
              procol_info[index].protocol_len,
             &(decode_ipcp_param.ipcp4_options),
              &num_dns_ipv4
              );

         ds_3gpp_update_ps_iface_v4_params(ps_iface_v4_p,decode_ipcp_param.ipcp4_options);
         decode_mask |= DS_3GPP_IPV4_PKT_DECODED_MASK;
       }
       else
       {
         DS_3GPP_MSG0_ERROR("Ignoring IPV4 DNS pkt for v6 only call");
       }

       break;
#ifdef FEATURE_DATA_PS_IPV6
    case IPV6_DNS_CONTAIN_ID:
       /* VRK: TODO add the check for secondary 
       if(dsumtsps_call_tbl->secondary_flag == TRUE)
       {
         DS_3GPP_MSG3_HIGH("Proto ID not valid for seccall: Callinst :%d Decode: Protocol ID %d",
              call_instance,protocol_id,0);
         return DS3G_SUCCESS;
       }
       */
       if (ds3gpp_iface_v6_p != NULL)
       {
        /*------------------------------------------------------------------
          IPV6 DNS (0x0003). Decode the packet and store the options
          in the call specific ipv6 options          
        ------------------------------------------------------------------*/
        ds3gpp_decode_ipv6_pkt(
            &protocol_config_options_p->
            procol_info[index].protocol_contents[0],
            protocol_config_options_p->
            procol_info[index].protocol_len,
            &(decode_ipcp_param.ipv6cp_options),
            &num_dns_ipv6
            );

        ds_3gpp_update_ps_iface_v6_params(ps_iface_v6_p,decode_ipcp_param.ipv6cp_options);
        decode_mask |= DS_3GPP_IPV6_PKT_DECODED_MASK;
       }
       else
       {
         DS_3GPP_MSG0_ERROR("Ignoring IPV6 DNS pkt for V4 only call ");
       }
         break;
#endif

#ifdef FEATURE_DATA_IMS
    case SIP_PCSCF_V6_ADDRESS_PROTOCOL:

        DS_3GPP_MSG0_HIGH("recd SIP_PCSCF_V6_ADDRESS_PROTOCOL");

        /* VRK: TODO add the check for secondary 
        if(dsumtsps_call_tbl->secondary_flag == TRUE)
        {
          DS_3GPP_MSG3_HIGH("Proto ID not valid for seccall: Callinst :%d Decode: Protocol ID %d",
               call_instance,protocol_id,0);
          return DS3G_SUCCESS;
        }
        */
        /*-------------------------------------------------------------------
           Decode v6 pcscf address packet (0x0001), and store address to v4 
           array or v6 array respectively since NW may return both v4 and v6 
           pcscf address in the dual IP bearer case
        -------------------------------------------------------------------*/
        if(ds3gpp_decode_v6_pcscf_pkt(
             &protocol_config_options_p->
             procol_info[index].protocol_contents[0],
             protocol_config_options_p->
             procol_info[index].protocol_len,
             pcscf_v4_addr_array,
             &num_v4_pcscf_addresses,
             pcscf_v6_addr_array,
             &num_v6_pcscf_addresses
             ) == FALSE)
        {
          DS_3GPP_MSG2_HIGH("Could not decode p-cscf addr. pdn cntx %x: Proco ID %d",
                   pdn_cntx_p, protocol_id);
        }
        else
        {
          decode_mask |= DS_3GPP_PCSCF_V6_PKT_DECODED_MASK;
        }
        break;
    case SIP_PCSCF_V4_ADDRESS_PROTOCOL:

        DS_3GPP_MSG0_HIGH("recd SIP_PCSCF_V4_ADDRESS_PROTOCOL");

        /*-------------------------------------------------------------------
           Decode v4 pcscf address packet (0x000C), and store address to v4 
           array 
        -------------------------------------------------------------------*/
        if(ds3gpp_decode_v4_pcscf_pkt(
             &protocol_config_options_p->
             procol_info[index].protocol_contents[0],
             protocol_config_options_p->
             procol_info[index].protocol_len,
             pcscf_v4_addr_array,
             &num_v4_pcscf_addresses) == FALSE)
        {
          DS_3GPP_MSG2_HIGH("Could not decode p-cscf addr. pdn cntx %x: Proco ID %d",
                   pdn_cntx_p, protocol_id);
        }
        else
        {
          decode_mask |= DS_3GPP_PCSCF_V4_PKT_DECODED_MASK;
        }
        break;

#endif // FEATURE_DATA_IMS
      case BEARER_CONTROL_MODE_CONTAINER_ID:
        if(ds_pdn_context_decode_bcm(
             &protocol_config_options_p->
             procol_info[index].protocol_contents[0],
             protocol_config_options_p->
             procol_info[index].protocol_len,
             pdn_cntx_p) == FALSE)
        {
          DS_3GPP_MSG2_HIGH("Could not decode BCM info. pdn cntx %x: Proco ID %d",
                   pdn_cntx_p, protocol_id);
        }
        else
        {
          decode_mask |= DS_3GPP_BEARER_CONTROL_MODE_DECODED_MASK;
        }
        break;

      case IPV4_LINK_MTU_CONTAINER_ID:
        ds_pdn_context_decode_ipv4v6_link_mtu(
             &protocol_config_options_p->
             procol_info[index].protocol_contents[0],
             protocol_config_options_p->
             procol_info[index].protocol_len,
               pdn_cntx_p, ps_iface_v4_p, 
             ps_iface_v6_p, &decode_mask);
        break;

      default:
         if (DSUMTS_OPERATOR_RESERVED_PCO_ID_RANGE_BEGIN <= protocol_id) 
         {
           ps_iface_type *pco_iface_v4_ptr = NULL;        /* v4 Interface ptr for the call*/
           ps_iface_type *pco_iface_v6_ptr = NULL;        /* v6 Interface ptr for the call*/

           if (ds3gpp_iface_v4_p != NULL)
           {
             pco_iface_v4_ptr = &(ds3gpp_iface_v4_p->ps_iface);
           }
           if (ds3gpp_iface_v6_p != NULL)
           {
             pco_iface_v6_ptr = &(ds3gpp_iface_v6_p->ps_iface);
           }

           if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
               operator_reserved_pco == protocol_id)
           {
             DS_3GPP_MSG0_HIGH("PCO ID is configured in the profile");
             profile_pco_id_configured = TRUE;
           }

           if (ds3gpp_decode_operator_reserved_pco_pkt(
               &protocol_config_options_p->
               procol_info[index].protocol_contents[0],
               protocol_config_options_p->
               procol_info[index].protocol_len, 
               pco_iface_v4_ptr, 
               pco_iface_v6_ptr,
               (uint16)protocol_id,
               profile_pco_id_configured) == FALSE) 
           {

             decode_mask |= DS_3GPP_OPER_DECODED_MASK;
           }
         }
         else 
         {
           DS_3GPP_MSG2_HIGH("pdn cntx:%x Protocol ID not supp. %d",
                   pdn_cntx_p,protocol_id);
         }

    }
    index++;
    num_pids--;
  }

#ifdef FEATURE_DATA_IMS
  /*-------------------------------------------------------------------------
    If one or more pcscf addresses were returned by network, store them in
    the ps_iface structure.
    In the dual IP bearer case, we will set the v4 address to the v4 iface
    and v6 address to the v6 iface
  -------------------------------------------------------------------------*/
  if (ds3gpp_iface_v4_p != NULL)
  {    

    DS_3GPP_MSG1_HIGH("recd %d v4_pcscf address", num_v4_pcscf_addresses);    
        
  if(num_v4_pcscf_addresses > 0)
  { 
    /*-----------------------------------------------------------------------
       ds3gpp_decode_v6_pcscf_pkt() decodes both v4 and v6 pcscf addresses.
       We need to update decode_mask with DS_3GPP_PCSCF_V4_PKT_DECODED here
       since it may have been missed before
    -----------------------------------------------------------------------*/
    decode_mask |= DS_3GPP_PCSCF_V4_PKT_DECODED_MASK;      

    if( (ps_iface_set_sip_serv_addr( ps_iface_v4_p,
                     pcscf_v4_addr_array,
                     num_v4_pcscf_addresses,
                     &ps_errno) != 0) && ((int)ps_errno != 0) )
    {
      DS_3GPP_MSG0_HIGH("Unable to store v4 p-cscf addresses");
    }
  }    
     
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Ignoring V4 PCSCF addr for V6 only call ");
  }

#ifdef FEATURE_DATA_PS_IPV6
  if (ds3gpp_iface_v6_p != NULL)
  {    
    DS_3GPP_MSG1_HIGH("recd %d v6_pcscf address", num_v6_pcscf_addresses);

  if(num_v6_pcscf_addresses > 0)
  {       
    if( (ps_iface_set_sip_serv_addr( ps_iface_v6_p,
                     pcscf_v6_addr_array,
                     num_v6_pcscf_addresses,
                     &ps_errno) != 0) && ((int)ps_errno != 0) )
    {
      DS_3GPP_MSG0_HIGH("Unable to store v6 p-cscf addresses");
    }
  }
        
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Ignoring V6 PCSCF addr for V4 only call ");
  }
#endif /* FEATURE_DATA_PS_IPV6 */
#endif

  return decode_mask;
} /* ds_pdn_cntxt_decodeProtocolParams() */

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_update_pco

DESCRIPTION
  This function is used to update PCO. This inturn invokes
  ds_pdn_cntxt_decodeProtocolParams to perform the actual update.
PARAMETERS
    call_id        - Call id of the call in progress
   *mode_info_ptr  - Pointer to the mode-specific call information provided
                     by Call Manager
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void
ds_3gpp_pdn_cntx_update_pco
(
  cm_call_mode_info_s_type *mode_info_ptr, 
  cm_call_id_type          call_id
)
{
  
  protocol_cfg_options_T *config_options_p;
  ds_bearer_context_s    *bearer_context_p; 
  ds_pdn_context_s       *pdn_context_p;
  uint32                  decode_mask = 0;
  ps_iface_type          *ps_v4_iface_p;
  ps_iface_type          *ps_v6_iface_p;
  ps_iface_ioctl_extended_ip_config_ex_type config_change_mask = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntx_update_pco: Entered with call_id %d",
                    call_id);

  if(mode_info_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_pdn_cntx_update_pco: mode_info_ptr NULL,"
                       " returning");
    return;
  }

  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntx_update_pco: %d",
                        mode_info_ptr->info_type); 
						
  switch(mode_info_ptr->info_type)
  {
    case CM_CALL_MODE_INFO_GW_PS:
      config_options_p = &(mode_info_ptr->info.gw_ps_call.config_options);        
      break;
      
    case CM_CALL_MODE_INFO_LTE:
      config_options_p = &(mode_info_ptr->info.lte_call.config_options);
      break;
    default:
      DS_3GPP_MSG1_ERROR("ds_3gpp_pdn_cntx_update_pco: Invalid mode info type %d,"
                       " returning", mode_info_ptr->info_type);
      return;
  }

  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(call_id);
  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }
  pdn_context_p = (ds_pdn_context_s*) bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    DS_3GPP_MSG2_ERROR("ds_3gpp_pdn_cntx_update_pco: pdn_cntx_ptr NULL for "
                       "bearer inst %d, call_id %d, returning", 
                       bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr, call_id);
    return;
  }

  if(config_options_p->valid)
  {
    decode_mask = ds_pdn_cntxt_decodeProtocolParams(
         pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[0],
         pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[1],
         config_options_p
      );
   
    ps_v4_iface_p =  DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);
    ps_v6_iface_p =  DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_context_p);

    if(ps_v4_iface_p != NULL 
       && DS_3GPP_PCSCF_V4_ADDR_DECODED(decode_mask))
    {
      DS_3GPP_MSG1_HIGH("Generating extended ip config PCSCF event for v4 ps iface %d",
                        ps_v4_iface_p);
      config_change_mask = PS_IFACE_EXT_IP_CFG_MASK_PCSCF_ADDR;
      ps_iface_generic_ind(ps_v4_iface_p, 
                           IFACE_EXTENDED_IP_CONFIG_EX_EV,
                           (void *)&config_change_mask);
    }


    if(ps_v6_iface_p != NULL 
       && DS_3GPP_PCSCF_V6_ADDR_DECODED(decode_mask))
    { 
      DS_3GPP_MSG1_HIGH("Generating extended ip config PCSCF event for v6 ps iface %d",
                        ps_v6_iface_p);
      config_change_mask = PS_IFACE_EXT_IP_CFG_MASK_PCSCF_ADDR;
      ps_iface_generic_ind(ps_v6_iface_p, 
                           IFACE_EXTENDED_IP_CONFIG_EX_EV,
                           (void *)&config_change_mask);
    }

  }
} /*ds_3gpp_pdn_cntx_update_pco() */

/*===========================================================================
FUNCTION ds_pdn_cntxt_setupPdpIPprotoStack

DESCRIPTION
  This function configures IFACE with CM provided mode_info.  

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
  Returns success or failure depending validity of configuration/mode_info.

SIDE EFFECTS
  If failed, cause code is populated in cm_end_params. 

===========================================================================*/
boolean ds_pdn_cntxt_setup_pdp_ip_proto_stack
( 
  ds_pdn_context_s               *pdn_cntx_p,
  const ds_umts_pdp_addr_type    *ds_3gpp_pdp_addr_p,  
  const protocol_cfg_options_T   *protocol_config_options_p,
  cm_end_params_s_type           *end_params 
)
{    
  ps_iface_type *iface_v4_ptr = NULL;        /* v4 Interface ptr for the call*/
  ds_3gpp_iface_s *ds3gpp_iface_v4_p = NULL; /* Pointer to the DS v4 Iface struct */
#ifdef FEATURE_DATA_PS_IPV6
  ps_iface_type *iface_v6_ptr = NULL;        /* v6 Interface ptr for the call*/
  ds_3gpp_iface_s *ds3gpp_iface_v6_p = NULL; /* Pointer to the DS v6 Iface struct */
#endif
  ds_umts_pdp_type_enum_type  pdn_pdp_type;  
  uint32 decode_mask = 0;  /* Bit vector representing PCO decode info*/ 
  sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_NONE;
  uint16 mtu_size;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Setting up Type PDP-IP stack");

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p,&subs_id))
  {
    return FALSE;
  }

  mtu_size = ds_3gpp_nv_manager_get_mtu_size(subs_id);
  /* Get the DS Iface based on the profile information */
  pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_cntx_p);
  if(pdn_pdp_type == DS_UMTS_PDP_PPP ||
     pdn_pdp_type == DS_UMTS_PDP_IPV4)
  {
    ds3gpp_iface_v4_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    DS_3GPP_MSG0_HIGH("PDP type IPV4 is selected in setupPdpIP"); 
  }
#ifdef FEATURE_DATA_PS_IPV6
  else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
  {
    ds3gpp_iface_v6_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
    DS_3GPP_MSG0_HIGH("PDP type IPV6 is selected in setupPdpIP"); 
  }
  else if (pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
  {
    ds3gpp_iface_v4_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
    ds3gpp_iface_v6_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);
  }
#endif /* FEATURE_DATA_PS_IPV6 */
  else
  {
    DATA_ERR_FATAL("PDP type is not PPP or IP V4 or IP V6. Dual IP bearer not sup yet");
    return(DS3G_FAILURE);
  }

  /*-------------------------------------------------------------------------
    Get the PS Iface pointer(s) from the DS Iface(s)
  -------------------------------------------------------------------------*/
  if (ds3gpp_iface_v4_p != NULL)
  {
    iface_v4_ptr = &(ds3gpp_iface_v4_p->ps_iface);
  }
  if (ds3gpp_iface_v6_p != NULL)
  {
    iface_v6_ptr = &(ds3gpp_iface_v6_p->ps_iface);
  }
  

  /*-------------------------------------------------------------------------
    Get any other protocol specific options that are received and store it
    in the call specific structure.
    Note: If the network had assigned the DNS, we would have decoded it and
    overwritten the ones selected by US during call origination.    
  -------------------------------------------------------------------------*/
  decode_mask = ds_pdn_cntxt_decodeProtocolParams(
       ds3gpp_iface_v4_p,
       ds3gpp_iface_v6_p,
       protocol_config_options_p
    );

  if(DS_3GPP_PCO_DECODE_FAILED(decode_mask) ||
     (decode_mask & DS_3GPP_IPV4V6_LINK_MTU_INVALID))
  {
    return DS3G_FAILURE;
  }
  
  if(iface_v4_ptr != NULL)
  {
    ASSERT(ps_iface_addr_family_is_v4(iface_v4_ptr));
    if (ds_pdn_cntxt_setup_ipv4_ps_iface(pdn_cntx_p,
                                         ds_3gpp_pdp_addr_p,
                                         end_params) == DS3G_FAILURE)
    {
      return DS3G_FAILURE;
    }

    if (!(decode_mask & DS_3GPP_IPV4V6_LINK_MTU_DECODED_MASK))
    {
      /*-------------------------------------------------------------------------
        Set MTU value based on the NV item.
      -------------------------------------------------------------------------*/
      DS_3GPP_MSG1_MED("MTU %d set for v4 iface from NV", mtu_size);
      PS_IFACE_SET_MTU(iface_v4_ptr, mtu_size);
    }
    
  }
#ifdef FEATURE_DATA_PS_IPV6
  if(iface_v6_ptr != NULL)
  {
    ASSERT(ps_iface_addr_family_is_v6(iface_v6_ptr));
    if (ds_pdn_cntxt_setupIPv6PSIface(pdn_cntx_p,
                                   ds_3gpp_pdp_addr_p,
                                   end_params) == DS3G_FAILURE)
    {
      return DS3G_FAILURE;
    }

    if (!(decode_mask & DS_3GPP_IPV4V6_LINK_MTU_DECODED_MASK))
    {
      /*-------------------------------------------------------------------------
        Set MTU value based on the NV item.
      -------------------------------------------------------------------------*/
      DS_3GPP_MSG1_MED("MTU %d set for v6 iface from NV", mtu_size);

      /*----------------------------------------------------------------------
        As per IPv6 RFC, the minimum supported MTU is 1280. PS_IFACE_SET_MTU 
        macro checks the MTU size given as input against default IPv6 MTU 1280 
       and ensures that IPv6 MTU being set is equal to or greater than 1280.
       ----------------------------------------------------------------------*/
      PS_IFACE_SET_MTU(iface_v6_ptr, mtu_size);
    }
  
  }
  return DS3G_SUCCESS;
#endif //FEATURE_DATA_PS_IPV6

} /* ds_pdn_cntxt_setupPdpIPprotoStack */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_DETERMINE_PDP_TYPE
DESCRIPTION
  This functions returns the PDP type to be used based on the current mode
  and roaming conditions per subscription.

PARAMETERS
  subs_id        - subscription id
  context_info_p - ptr to PDP context
  roaming_status - UE roaming status

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - V4, V6 or V4V6

SIDE EFFECTS 
  None.
===========================================================================*/
ds_umts_pdp_type_enum_type ds_3gpp_pdn_context_determine_pdp_type
(
  sys_modem_as_id_e_type           subs_id,
  const ds_umts_pdp_context_type  *context_info_p,
  boolean                          roaming_status
)
{
  sys_sys_mode_e_type         pref_mode = SYS_SYS_MODE_NO_SRV;
  ds_umts_pdp_type_enum_type  pdp_type_to_use = DS_UMTS_PDP_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( context_info_p == NULL ) 
  {
    DS_3GPP_MSG0_ERROR( "Null context_info ptr passed ");
    return pdp_type_to_use;
  }

  if( ds3gsubsmgr_is_subs_id_valid(subs_id) == FALSE )
  {
    return pdp_type_to_use;
  }

  pref_mode = ds3g_get_current_network_mode_ex(subs_id);

  /* Set PDP Type to legacy PDP type */
  pdp_type_to_use = context_info_p->pdp_type;

  if( roaming_status )
  {
    switch( pref_mode )
    {
      case SYS_SYS_MODE_LTE:
        if( context_info_p->lte_roaming_pdp_type != DS_UMTS_PDP_MAX )
        {
          pdp_type_to_use = context_info_p->lte_roaming_pdp_type;
        }
        break;

      case SYS_SYS_MODE_GSM:
      case SYS_SYS_MODE_WCDMA:
      case SYS_SYS_MODE_TDS:
        if( context_info_p->umts_roaming_pdp_type != DS_UMTS_PDP_MAX )
        {
          pdp_type_to_use = context_info_p->umts_roaming_pdp_type;
        }
        break;

      default:
        DS_3GPP_MSG0_ERROR("Non-3GPP mode");
        break;
    }
  }
  else /* Home scenario i.e. roaming_status == FALSE */
  {
    if( context_info_p->override_home_pdp_type != DS_UMTS_PDP_MAX )
    {
      pdp_type_to_use = context_info_p->override_home_pdp_type;
    }
  }

  DS_3GPP_MSG3_LOW("roaming_status:%d pref_mode:%d pdp_type_to_use:%d",
                   roaming_status, pref_mode, pdp_type_to_use);
  return pdp_type_to_use;
} /* ds_3gpp_pdn_context_determine_pdp_type() */

/*===========================================================================
FUNCTION DSUMTSPS_PROCESS_PDP_PROFILE_INFO

DESCRIPTION
  This function Sets the PDP profile to be used for the call instance
  specified. It also gets the profile parameters for the call and stores
  it in the umtsps iface table.

  This function is called by the routing layer before to set this information.
  Subsequent to this, the call origination happens.

PARAMETERS
  *umtspsps_iface_ptr - ptr to umtsps iface instance
  pdp_profile_number - contains the PDP profile information

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cntxt_process_pdp_profile_info
(
  ds_pdn_context_s          *pdn_cntx_p,
  uint32                    profile_number, /* PDP context config info*/
  ds_apn_ip_support_type_e  *ip_support_p,
  ds_umts_pdp_profile_type  *prof_context
)
{
  sys_modem_as_id_e_type      cm_subs_id;
  ds_umts_pdp_type_enum_type  pdp_type_to_use;
  boolean                     roaming_status = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    This function is called by the ACL routines to set the PDP
    profile number selected in the dial string for a laptop call or for a
    sockets call.
    Store all the parameters of the profile in the PDN context.  
  -------------------------------------------------------------------------*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  if(NULL == prof_context)
  {
    DS_3GPP_MSG0_ERROR("Profile cntxt passed is not valid");
    return FALSE;
  }

  cm_subs_id = pdn_cntx_p->ds_pdn_context_dyn_p->subs_id;
  roaming_status = ds_3gpp_roaming_get_status(cm_subs_id);
  
  memscpy(&(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile), 
          sizeof(ds_umts_pdp_profile_type),
          prof_context, sizeof(ds_umts_pdp_profile_type));

  pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile_num = profile_number; 

  /*-------------------------------------------------------------------------
    Update the PDP type in PDN context based on Home/Roaming conditions
  -------------------------------------------------------------------------*/
  pdp_type_to_use = ds_3gpp_pdn_context_determine_pdp_type(
                      cm_subs_id,
                      &pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context,
                      roaming_status);

    DS_3GPP_MSG2_MED("roaming_status:%d pdp_type_to_use:%d",
                   roaming_status, pdp_type_to_use);
  pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
    = pdp_type_to_use;

  /*-------------------------------------------------------------------------
    Store in DS Iface, whether the call was originated from TE or by the 
    sockets layer.

    Fill in the out param - ip_support_p based on the PDP type
  -------------------------------------------------------------------------*/
  if( (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type
                                                       == DS_UMTS_PDP_PPP) ||
      (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type
                                                       == DS_UMTS_PDP_IPV4) )
  {
    if(ip_support_p != NULL)
    {
      *(ip_support_p) = DS_IPV4_SINGLE_BEARER;
    }
  }
  else if(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type 
          == DS_UMTS_PDP_IPV6)
  {
    if(ip_support_p != NULL)
    {
      *(ip_support_p) = DS_IPV6_SINGLE_BEARER;
    }
  }
  else if (pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type
            == DS_UMTS_PDP_IPV4V6)
  {
    if(ip_support_p != NULL)
    {
      *(ip_support_p) = DS_IPV4V6;
    }
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Unknow PDP type in profile: %d",
              pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type);

    return FALSE;
  }

  return TRUE;
} /* ds_pdn_cntxt_processPdpProfileInfo() */

/*===========================================================================

FUNCTION ds_pdn_cntxt_storeEncodedApn

DESCRIPTION
  This function encodes the apn name contained in apn_src
  and stores it in apn_dest.


PARAMETERS

  apn_dest:   Destination to store the encoded apn.

  apn_src:    Uncoded APN name.

DEPENDENCIES
  None.

RETURN VALUE
  Length of the encoded APN

SIDE EFFECTS

===========================================================================*/
uint8 ds_pdn_cntxt_store_encoded_apn
(
  byte *apn_dest,
  byte *apn_src
)
{
  uint8 apn_encoded_length,label_length;
  byte  *label_ptr,*label_len_ptr,*pdp_addr_str_ptr;
  int   pdp_addr_str_len,apn_len_parsed;

  if(apn_dest == NULL)
  {
    ASSERT(0);
    return 0;
  }

  /*-------------------------------------------------------------------------
    Get the length of the apn string.
  -------------------------------------------------------------------------*/
  pdp_addr_str_len = strlen((char *)apn_src);

  /*-------------------------------------------------------------------------
    If the apn is a null apn, set the first byte of the destination apn to 0
  -------------------------------------------------------------------------*/
  if(pdp_addr_str_len == 0)
  {
    DS_3GPP_MSG0_HIGH("Null APN");
    apn_dest[0] = 0;
    return 1;
  }

  if(( pdp_addr_str_len + 1 ) > MAX_APN_ADR_LEN  )
  {
    DS_3GPP_MSG0_ERROR("APN:larger than space allowed:not including");
    DS_3GPP_MSG2_HIGH("APN len:%d,MAX space to encode:%d",pdp_addr_str_len,
      MAX_APN_ADR_LEN);
    return 1;
  }

  /*-------------------------------------------------------------------------
    We have a valid APN.
    Check if we have room to encode the APN first. Since APN is specified as
    label1.label2.label3 (e.g www.ya-hoo.com ), and encoded as
    <len1><label1><len2><label2><len3L<label3> (3www6ya-hoo3com)
    the total length required to encode will be one more than the
    original length of the APN string
  -------------------------------------------------------------------------*/

#define LABEL_DELIMITOR 0x2e

  /*-------------------------------------------------------------------------
    Encode APN as per encoding rules defined in TS23.003 section 9.1
    The APN consists of one or more labels. Each label is coded as one octet
    length field followed by that number of octets coded as 8 bit ASCII chars

    label_len_ptr is used to point to the byte where the length of the
    current label must be stored. It is initialized to the start of the apn
    storage area.It is set to be one byte before the label starts.
    label_ptr is used to encode each label. It is initialized to the start of
    the apn storage area + 1.
    apn_encoded_length is used to track the total number of bytes encoded
    It initilized to '1' to account for the label length of the first label
  -------------------------------------------------------------------------*/
  label_len_ptr = &apn_dest[0];
  label_ptr = &apn_dest[1];
  label_length  = 0;
  apn_encoded_length  = 1;

  /*-------------------------------------------------------------------------
    Get the total length of the APN string and initiliaze the bytes parsed to
    to zero.
    pdp_addr_str_ptr tracks the current byte which is being parsed in the
    APN. Initilize it to the start of the APN string
  -------------------------------------------------------------------------*/
  apn_len_parsed = 0;
  pdp_addr_str_ptr = (byte*)&apn_src[0];

  /*-------------------------------------------------------------------------
    Now do the encoding while parsing the APN string. If we detect an error,
    we abort encoding and return as though NO apn is included.
  -------------------------------------------------------------------------*/
  for(apn_len_parsed = 0;
      apn_len_parsed < pdp_addr_str_len;
      apn_len_parsed++,pdp_addr_str_ptr++)
  {
    if( *pdp_addr_str_ptr == LABEL_DELIMITOR )
    {
      /*---------------------------------------------------------------------
        At the end of the current label. Encode the label length and reset
        the fields for the next label
      ---------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("Encoding APN: label len:%d",label_length);
      *label_len_ptr = label_length;
      /*---------------------------------------------------------------------
        The length of the next label starts at the end of the current label.
        and label_ptr starts at one byte off the label_len_ptr
        Increment apn_encoded_length by 1 to account for the storage reqd for
        the legnth of the next label
      ---------------------------------------------------------------------*/
      label_len_ptr = label_ptr;
      label_ptr++;
      apn_encoded_length++;
      label_length  = 0;
    }
    else
    {
      /*---------------------------------------------------------------------
        Encode the character and keep track of the length encoded so far
        increment the current label's ptr
        increment the length encoded so far
        increment the total space used in encoding all labels till now
      ---------------------------------------------------------------------*/
      *label_ptr = *pdp_addr_str_ptr ;
      label_ptr++;
      label_length++;
      apn_encoded_length++;
    }

  }
  if(apn_encoded_length > 1 )
  {
    /*-----------------------------------------------------------------------
      Tie of the length of the last label encoded if we had any
    -----------------------------------------------------------------------*/
    *label_len_ptr = label_length;
    DS_3GPP_MSG1_HIGH("Encoding APN: label len:%d",label_length);


  }
  return apn_encoded_length;
} /* ds_pdn_cntxt_storeEncodedApn */

/*===========================================================================
FUNCTION ds_pdn_cntxt_fillOriginationApn

DESCRIPTION
  This function fills up the APN  in the origination parameters
  based on the parameters in the ATCommand value passed

PARAMETERS
  pdp_profile_parms  - Ptr to PDP profile parameters for the current call
  *orig_params_ptr   - Ptr to origination parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_cntxt_fill_origination_apn
(
    ds_umts_pdp_profile_type     *pdp_profile_parms,
    cm_gw_ps_orig_params_s_type  *orig_params_ptr
)
{
  //uint8 apn_encoded_length = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    The Access Point Name is a label or a full qualified domain name
    according to DNS naming conventions . The IE is defined in 10.5.6.1.
    valid APN specified in AT +CDGCONT : fill it
  -------------------------------------------------------------------------*/

  orig_params_ptr->apn_name.valid         = FALSE;

  /*-------------------------------------------------------------------------
    Start with a default APN
  -------------------------------------------------------------------------*/
  orig_params_ptr->apn_name.apn_addr_len  = 0;
  //orig_params_ptr->apn_name.address[0]    = 0;

/*  apn_encoded_length = ds_pdn_cntxt_storeEncodedApn(
    orig_params_ptr->apn_name.address, (byte*)pdp_profile_parms->context.apn);
*/
#if 0
// Ifdef to prevent lint info. Remove the ifdef when we call
//   storeEncodedApn
  if(apn_encoded_length > 1 )
  {
    orig_params_ptr->apn_name.valid         = TRUE;

    /*-----------------------------------------------------------------------
      Fill the total length of the encoded APN
    -----------------------------------------------------------------------*/
    orig_params_ptr->apn_name.apn_addr_len = apn_encoded_length;

  }
#endif
  return;
} /* ds_pdn_cntxt_fillOriginationApn() */


/*===========================================================================
FUNCTION ds_pdn_cntxt_convDataCompFromPdp_tToSys_t

DESCRIPTION
  This function converts Data Compression parameter from PDP Profile's 
  native type to sys_pdp_data_comp_e_type.

PARAMETERS
    ds_umts__pdp_data_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_data_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_pdp_data_comp_e_type ds_pdn_cntxt_conv_data_comp_from_pdp_t_to_sys_t
(
  const ds_umts__pdp_data_comp_e_type *pdp_d_comp
)
{
  sys_pdp_data_comp_e_type sys_d_comp;

  switch (*pdp_d_comp) {
  case DS_UMTS_PDP_DATA_COMP_OFF:
      sys_d_comp = SYS_PDP_DATA_COMP_OFF;
      break;
  case DS_UMTS_PDP_DATA_COMP_ON:
      sys_d_comp = SYS_PDP_DATA_COMP_ON;
      break;
  case DS_UMTS_PDP_DATA_COMP_V42_BIS:
      sys_d_comp = SYS_PDP_DATA_COMP_V42_BIS;
      break;
  case DS_UMTS_PDP_DATA_COMP_V44:
      sys_d_comp = SYS_PDP_DATA_COMP_V44;
      break;
  default:
      ASSERT(0);
      sys_d_comp = SYS_PDP_DATA_COMP_MAX;
  }

  return sys_d_comp;
}

/*===========================================================================
FUNCTION ds_pdn_cntxt_convHeaderCompFromPdp_tToSys_t

DESCRIPTION
  This function converts Header Compression parameter from PDP Profile's 
  native type to sys_pdp_header_comp_e_type.

PARAMETERS
    ds_umts_pdp_header_comp_e_type - PDP Profile Header Compression type

DEPENDENCIES
  None.

RETURN VALUE
  sys_pdp_header_comp_e_type - Header Compression type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_pdp_header_comp_e_type ds_pdn_cntxt_conv_header_comp_from_pdp_t_to_sys_t
(
  const ds_umts_pdp_header_comp_e_type *pdp_h_comp
)
{
  sys_pdp_header_comp_e_type sys_h_comp;

  switch (*pdp_h_comp) {
  case DS_UMTS_PDP_HEADER_COMP_OFF:
      sys_h_comp = SYS_PDP_HEADER_COMP_OFF;
      break;
  case DS_UMTS_PDP_HEADER_COMP_ON:
      sys_h_comp = SYS_PDP_HEADER_COMP_ON;
      break;
  case DS_UMTS_PDP_HEADER_COMP_RFC1144:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC1144;
      break;
  case DS_UMTS_PDP_HEADER_COMP_RFC2507:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC2507;
      break;
  case DS_UMTS_PDP_HEADER_COMP_RFC3095:
      sys_h_comp = SYS_PDP_HEADER_COMP_RFC3095;
      break;
  default:
      ASSERT(0);
      sys_h_comp = SYS_PDP_HEADER_COMP_MAX;
  }

  return sys_h_comp;
}

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DSPDNCNTX_GEN_PCSCF_ADDRESS_REQUEST_PACKET

DESCRIPTION
  This function generates a PCSCF request packet to request the 
  pcscf address.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the pcscf request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_pdn_cntxt_gen_pcscf_address_request_packet
(
  protocol_cfg_options_T *protocol_config_options_p,
  const ds_umts_pdp_profile_type *profile_params,/* profile parameters */
  sys_modem_as_id_e_type          subs_id 
)
{
  uint16  proto_info_index;
  boolean request_pcscf_flag;
  uint8 rel_version;
  ds_dsd_apm_req_pay_load req_payload = {0};
  ds_dsd_ext_error_code dsd_result = DS_DSD_APM_UNSEPCIFIED;

  //Get the attach APN first.
  dsd_result = ds_dsd_apm_req_ex(DS_DSD_APM_ATTCH_REQ, &req_payload,subs_id);

  if (dsd_result == DS_DSD_APM_SUCCESS)
  {
    if (req_payload.profile_id == (uint16)profile_params->context.pdp_context_number)
    {
      //The profile that we are querying about is the current attach APN.
      req_payload.rule_type = DS_DSD_APM_ATTACH_RULE;
    }
  }
  else 
  {
    //The profile that we are querying on is not the current attach APN.
    //So its an on demand PDN.
    req_payload.rule_type = DS_DSD_APM_ON_DEMAND_RULE;
  }
  
  /* Get the PCSCF rules from APM */  

  //Need to pass the profile id also when making the request.
  req_payload.profile_id = (uint16)profile_params->context.pdp_context_number;

  (void)ds_dsd_apm_req_ex( DS_DSD_APM_MANDATORY_PCSCF_REQ , 
                           &req_payload ,
                           subs_id);
  DS_3GPP_MSG3_HIGH("APM response to MANDATORY_PCSCF_REQ is %d",
                    req_payload.mandatory_pcscf, 0, 0); 


  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;

  request_pcscf_flag = profile_params->request_pcscf_address_flag;

  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_support,
    if mode is not LTE. For LTE always fill this container.
  -------------------------------------------------------------------------*/
  if ( ds3gpp_get_current_network_mode(subs_id) != SYS_SYS_MODE_LTE )
  {
    rel_version = ds_3gpp_nv_manager_get_3gpp_rel_version();
    if ( rel_version < (uint8)DS_3GPP_REL8 )
    {
      /* No need to generate pcscf address request; return SUCCESS from here */
      DS_3GPP_MSG0_HIGH("PCSCF addr container not filled"); 
      return DS3G_SUCCESS;
    }
  }

  /* If V6 is mandatory in the APM rules or the profile is configured to
     request it; then include the request packet.*/
  if ( ipv6_mandatory_and_ipv4_donotcare == req_payload.mandatory_pcscf ||
       ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ( ( TRUE == request_pcscf_flag ) &&
         ( DS_UMTS_PDP_IPV6 == profile_params->context.pdp_type ||
           DS_UMTS_PDP_IPV4V6 == profile_params->context.pdp_type ) ) )
  {
  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request. 
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store pcscf pkt");
    return DS3G_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Encode a pcscf request IE within the configuration options.
    The container Id for pcscf address request is SIP_PCSCF_ADDRESS_PROTOCOL
    Length of packet = 0 and the contents is empty.
  -------------------------------------------------------------------------*/
  /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].protocol_id[0] 
                                = ((SIP_PCSCF_V6_ADDRESS_PROTOCOL & 0XFF00) >> 8);
  /*lint +e572 */
    protocol_config_options_p->procol_info[proto_info_index].protocol_id[1] 
                                = (SIP_PCSCF_V6_ADDRESS_PROTOCOL & 0X00FF);

    protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
    /*-------------------------------------------------------------------------
      Increment the protocol info index.
    -------------------------------------------------------------------------*/
    proto_info_index++;
    protocol_config_options_p->num_protocol_ids = proto_info_index;
    DS_3GPP_MSG0_HIGH("Included V6 SIP pcscf address req pkt in PDP ctx req");
  }

  /* If V4 is mandatory in the APM rules or the profile is configured to
     request it; then include the request packet.*/

  if ( ipv4_mandatory_and_ipv6_donotcare == req_payload.mandatory_pcscf ||
       ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf ||
       ( ( TRUE == request_pcscf_flag ) &&
         ( DS_UMTS_PDP_IPV4 == profile_params->context.pdp_type ||
           DS_UMTS_PDP_IPV4V6 == profile_params->context.pdp_type ) ) )  
  {
    /*-------------------------------------------------------------------------
      Check if we have room to store this protocol info pkt in the pdp 
      context request. 
    -------------------------------------------------------------------------*/
    if(proto_info_index >= NUM_PROTOCOL_ID)
    {
      DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store pcscf pkt");
      return DS3G_FAILURE;
    }
    /*-------------------------------------------------------------------------
      Encode a pcscf request IE within the configuration options.
      The container Id for pcscf address request is SIP_PCSCF_ADDRESS_PROTOCOL
      Length of packet = 0 and the contents is empty.
    -------------------------------------------------------------------------*/
    /*lint -e572 */
    protocol_config_options_p->procol_info[proto_info_index].protocol_id[0] 
                                = ((SIP_PCSCF_V4_ADDRESS_PROTOCOL & 0XFF00) >> 8);
    /*lint +e572 */
    protocol_config_options_p->procol_info[proto_info_index].protocol_id[1] 
                                = (SIP_PCSCF_V4_ADDRESS_PROTOCOL & 0X00FF);

    protocol_config_options_p->procol_info[proto_info_index].protocol_len = 0;
    /*-------------------------------------------------------------------------
      Increment the protocol info index.
    -------------------------------------------------------------------------*/
    proto_info_index++;
    protocol_config_options_p->num_protocol_ids = proto_info_index;
    DS_3GPP_MSG0_HIGH("Included V4 SIP pcscf address req pkt in PDP ctx req");
  }

  return DS3G_SUCCESS;

} /* ds_pdn_cntxt_gen_pcscf_address_request_packet */


/*===========================================================================
FUNCTION DSPDNCNTX_GEN_IM_CN_SUBSYSTEM_REQUEST_PACKET

DESCRIPTION
  This function generates a IM CN request packet to request the 
  signaling only context.

PARAMETERS   
  orig_params_ptr  - Pointer to the buffer to store the IM CN request packet
    
DEPENDENCIES 
  None.

RETURN VALUE  
  DS3G_SUCCESS - packet generated succesfully
  DS3G_FAILURE - pkt not generated
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean ds_pdn_cntxt_gen_im_cn_subsystem_request_packet
(
  protocol_cfg_options_T *protocol_config_options_p 
)
{
  uint16  proto_info_index;

  /*-------------------------------------------------------------------------
    Get the protocol info index. This index is used to fill in PPP packets
    in the PDP context req. msg
  -------------------------------------------------------------------------*/
  proto_info_index =  protocol_config_options_p->num_protocol_ids;

  /*-------------------------------------------------------------------------
    Check if we have room to store this protocol info pkt in the pdp 
    context request. 
  -------------------------------------------------------------------------*/
  if(proto_info_index >= NUM_PROTOCOL_ID)
  {
    DS_3GPP_MSG0_ERROR("No room in PDP ctx msg to store IM CN pkt");
    return DS3G_FAILURE;
  }
  /*-------------------------------------------------------------------------
    Encode a IM_CN IE within the configuration options.
    The container Id for IM CN address request is IM_CN_ADDRESS_PROTOCOL
    Length of packet = 0 and the contents is empty.
  -------------------------------------------------------------------------*/
  /*lint -e572 */
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[0] = ( (IM_CN_SUBSYSTEM_PROTOCOL & 0XFF00) >> 8);
  /*lint +e572 */
  protocol_config_options_p->procol_info[proto_info_index].
    protocol_id[1] = (IM_CN_SUBSYSTEM_PROTOCOL & 0X00FF);

  protocol_config_options_p->procol_info[proto_info_index].
    protocol_len = 0;

  /*-------------------------------------------------------------------------
    Increment the protocol info index.
  -------------------------------------------------------------------------*/
  proto_info_index++;
  protocol_config_options_p->num_protocol_ids = proto_info_index;

  protocol_config_options_p->valid = TRUE;

  DS_3GPP_MSG0_HIGH("Included IM CN subsystem pkt in PDP ctx req");

  return DS3G_SUCCESS;

} /* ds_pdn_cntxt_gen_im_cn_subsystem_request_packet */
#endif // FEATURE_DATA_IMS

/*
  This function is used to check the Iface address family agains the type of
  the call to be originated. If the types are the same, this function returns
  TRUE. Otherwise it returns FALSE.
*/
boolean ds_pdn_cntxt_check_iface_type_matches(ps_iface_type         *iface_p,
                                        ds_pdn_context_type_e orig_call_type)
{
  ds_pdn_context_type_e iface_addr_family;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ps_iface_addr_family_is_v6(iface_p))
  {
    iface_addr_family = DS_3GPP_PDP_TYPE_IPV6;
  }
  else if(ps_iface_addr_family_is_v4(iface_p))
  {
    iface_addr_family = DS_3GPP_PDP_TYPE_IPV4;
  }
  else
  {
    iface_addr_family = DS_3GPP_PDP_TYPE_FAIL;
  }

  /*
    we route the PPP and IP V4 calls on the IPV4 iface.
    Hence if incoming call is of type PPP, we consider it to
    be V4 type
  */
  if(orig_call_type == DS_3GPP_PDP_TYPE_PPP)
  {
    orig_call_type = DS_3GPP_PDP_TYPE_IPV4;
  }

  return (orig_call_type == iface_addr_family) || (orig_call_type == DS_3GPP_PDP_TYPE_IP_ANY);
}

/*===========================================================================
FUNCTION DSPDNCNTX_ALLOCATE_NEW_PDN_CONTEXT

DESCRIPTION
  This function allocates unused pdn context. The allocation of the PDN cntx
  is protected within a critical section since this function could be called
  as a part of route acl post processing. Since the route acl post processing
  happens from the application's context, we need this critical section.

PARAMETERS
  subs_id - Subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to newly allocated pdn context or NULL if there is none left.

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s* ds_pdn_cntxt_allocate_new_pdn_context
(
  sys_modem_as_id_e_type subs_id
)
{
  int               pdn_cntx_index;
  ds_pdn_context_s  *pdn_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Protect allocation of PDN contexts under critical section
  -------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();
  for(pdn_cntx_index = 0; 
      pdn_cntx_index < DS_3GPP_MAX_PDN_CONTEXT; 
      pdn_cntx_index++)
  {
    
    if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(
                                       &(ds_pdn_context_tbl[pdn_cntx_index])))
    {
      if(!ds_3gpp_pdn_cntx_alloc_dyn_mem(pdn_cntx_index, subs_id))
      {
        DS_3GPP_MSG0_ERROR("Dyn PDN context unallocated, "
                           "Heap Allocation failure");
        break;
      }

      pdn_context_p = &(ds_pdn_context_tbl[pdn_cntx_index]);
       /*-----------------------------------------------------------------------
         Initialize the state to DOWN
       -----------------------------------------------------------------------*/
      pdn_context_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_DOWN;
      pdn_context_p->ds_pdn_context_dyn_p->num_active_bearer_context = 0;
      pdn_context_p->ds_pdn_context_dyn_p->is_default = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->keep_alive_pdn = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->force_ota_teardown = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->ims_sip_reg_failure_flag = FALSE;
      /*-------------------------------------------------------------------------
        Set the flags allow_iface_bring_up and handover_attach_flag to FALSE
      -------------------------------------------------------------------------*/
      pdn_context_p->ds_pdn_context_dyn_p->allow_iface_bring_up = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->handover_attach_flag = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->tear_down_in_progress = FALSE;
      /*------------------------------------------------------------------------
        Define the PDN inactivity timer. Specify the call back function and the
        call back data.
      ------------------------------------------------------------------------*/
      rex_def_timer_ex(&(pdn_context_p->ds_pdn_context_dyn_p->pdn_inactivity_timer),
                       (rex_timer_cb_type) ds_3gpp_pdn_cntx_inactivity_timer_cb,
                       (unsigned long)(pdn_context_p));

      /*------------------------------------------------------------------------
        Define the polling timer. Specify the call back function and the
        call back data.
      ------------------------------------------------------------------------*/
      rex_def_timer_ex(&(pdn_context_p->ds_pdn_context_dyn_p->polling_timer),
                       (rex_timer_cb_type) ds_3gpp_pdn_cntx_polling_timer_cb,
                       (unsigned long)(pdn_context_p));

      /*------------------------------------------------------------------------
        Define the apn timer. Specify the call back function and the
        call back data.
      ------------------------------------------------------------------------*/
      rex_def_timer_ex(&(pdn_context_p->ds_pdn_context_dyn_p->apn_param_chg_timer),
                       (rex_timer_cb_type) ds_3gpp_pdn_cntx_apn_param_chg_timer_cb,
                       (unsigned long)(pdn_context_p));

      /*------------------------------------------------------------------------
        Reset the local disconnect flag
      ------------------------------------------------------------------------*/
      pdn_context_p->ds_pdn_context_dyn_p->local_disconnect_sent = FALSE;
      pdn_context_p->ds_pdn_context_dyn_p->subs_id = subs_id;

      /*------------------------------------------------------------------------ 
        Register function ptrs
       --------------------------------------------------------------------------*/
  #if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) 
      pdn_context_p->ds_pdn_context_dyn_p->vobj_data_ptr_tbl[DS_3GPP_UMTS_VOBJ] =
       (void*)&(umts_pdn_context_tbl[pdn_cntx_index]);
  #endif

      break;
    }



  } //end for loop over pdn_cntx_index
  ds3gpp_leave_global_crit_section();

  /*--------------------------------------------------------------------------- 
   If all pdn contexts have been allocated, print out a msg saying its not the
   heap allocation that is failing
  ---------------------------------------------------------------------------*/
  if (pdn_cntx_index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    DS_3GPP_MSG0_HIGH("MAX PDN contexts have been allocated");
  }

  return pdn_context_p;
}

/*===========================================================================
FUNCTION DSPDNCNTX_USE_DEFAULT_DIAL_STRING

DESCRIPTION
  This function is used by bearer context to figure out if it has to use the
  default dial string to bring up the call.

PARAMETERS
  IN
    ps_ip_type - This is the IP type of the PS Iface

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE  - if default dial string is to be used
  FALSE - if otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_cntxt_use_default_dial_string
(
  ds_pdn_context_s  *pdn_cntx_p,
  void             *info_ptr
)
{
  boolean                   useDefaultDialStr = TRUE;
  ds_3gpp_iface_s           *ds3gpp_v4_iface_p;
  ps_iface_type             *ps_iface_ptr = NULL;
  network_params_info_type  *ppp_info_ptr = (network_params_info_type*)info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
 
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }
  /*-------------------------------------------------------------------------
   Default dial string is set during UE init. TE dial string is set in RMSM
   Socket Call: use default dial string
   Non-Socket Call (TE/RMNET): check whether dial str is valid then use it, 
   else use default 
  --------------------------------------------------------------------------*/
  ds3gpp_v4_iface_p = 
      (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);  

  if( info_ptr == NULL ) 
  {
    /* PPP call, use default dial str */
    useDefaultDialStr = TRUE;    
  }
  else
  {
    if(ds3gpp_v4_iface_p != NULL)
    {
      ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
      if (PS_IFACE_PROC_ID_LOCAL != PS_IFACE_GET_PROC_ID(ps_iface_ptr)) 
      {
        /*TE PDP_IP or RMNET call*/
        if ( ppp_info_ptr->valid_fields & (uint32)NET_CFG_PRM_DIAL_STR_MASK )
        {
          useDefaultDialStr = FALSE;
        }
        else
        {
          useDefaultDialStr = TRUE;
        }
      }
    }   
  }
  DS_3GPP_MSG1_MED("Using Default Dail str %d ", useDefaultDialStr);
  return useDefaultDialStr;
}

/*===========================================================================
FUNCTION DSPDNCNTX_IS_DEFAULT

DESCRIPTION
  This function checks whether the pdn context pointer points to the 
  default pdn context

PARAMETERS   :
    ds_pdn_context_s * - pdn context pointer
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - This pdn context pointer points to the default pdn context
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_default(ds_pdn_context_s* pdn_cntx_p)
{
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  return ( pdn_cntx_p->ds_pdn_context_dyn_p->is_default);
}

/*===========================================================================
FUNCTION DSPDNCNTX_IS_DEFAULT_IFACE

DESCRIPTION
  This function checks whether if the iface pointer points to one of the 
  default ifaces

PARAMETERS   :
    ps_iface_type * - iface pointer
DEPENDENCIES
  Assumes a valid pdn context.

RETURN VALUE
  TRUE - This iface pointer points to a default iface
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_default_iface(ps_iface_type *iface_p)
{
  boolean ret_val = FALSE;
  ds_3gpp_iface_s*   ds_iface_p;
  ds_pdn_context_s*  pdn_cntx_p;

  ds_iface_p = (iface_p->client_data_ptr);
  pdn_cntx_p = ds_iface_p->client_data_ptr;

  if( ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p) == FALSE )
  {
    ASSERT(0);
    return FALSE;
  }

  if ( pdn_cntx_p->ds_pdn_context_dyn_p->is_default == TRUE )
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SETUP_PDN_WITH_DS_IFACE

DESCRIPTION
  This function associates a valid DS IFACE to a newly allocated pdn context
  Updated the pdp type in the profile and the APN table as required.
 
PARAMETERS 
  new_pdn_cnt_p      - Pointer to the newly allocated PDN context
  profile_num        - Profile from which pdn is to be updated
  ds_iface_p         - Pointer to a previously valid DS iface that is being
                       associated with the newly allocated PDN
  ds_iface_index     - Index into which ds_iface_p needs to be placed within
                       the DS iface table of the new pdn context
  old_apn            - Pointer to the apn string
  old_pdn_cntx_p    - Pointer to the old PDN context
DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  Unbinds the phys link from the flow of the DS iface passed.
===========================================================================*/
void ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface 
(
ds_pdn_context_s          *new_pdn_cntx_p, 
ds_umts_pdp_profile_type  *old_pdp_profile_p,
ds_3gpp_iface_s           *ds_iface_p, 
int                       ds_iface_index,
ds_pdn_context_s          *old_pdn_cntx_p
)
{
  ds_apn_ip_support_type_e ip_support_to_request = DS_IP_TYPE_INVALID;
  ds_apn_ip_support_type_e ip_support_granted = DS_IP_TYPE_INVALID;

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(new_pdn_cntx_p))
  {
    return;
  }

  if(old_pdp_profile_p == NULL)
  {
    DATA_ERR_FATAL("PDP profile ptr passed is NULL");
    return;
  }

  memscpy((void *)&(new_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile),
          sizeof(ds_umts_pdp_profile_type),
          (void *)old_pdp_profile_p,
          sizeof(ds_umts_pdp_profile_type));

  if(ds_iface_index == DS_3GPP_PDN_CONTEXT_IPV4_IFACE)
  {
     new_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
       context.pdp_type = DS_UMTS_PDP_IPV4;
     new_pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV4_IFACE] = ds_iface_p;
     ip_support_to_request = DS_IPV4_SINGLE_BEARER;
     ip_support_granted = DS_IPV6_SINGLE_BEARER;
  }
  else if(ds_iface_index == DS_3GPP_PDN_CONTEXT_IPV6_IFACE)
  {
    new_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.
      context.pdp_type = DS_UMTS_PDP_IPV6;
    new_pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
      [DS_3GPP_PDN_CONTEXT_IPV6_IFACE] = ds_iface_p;
    ip_support_to_request = DS_IPV6_SINGLE_BEARER;
    ip_support_granted = DS_IPV4_SINGLE_BEARER;
  }

  /*----------------------------------------------------------------------
  Point the client data pointer of the DS Iface to the PDN context
  ----------------------------------------------------------------------*/
  ds_iface_p->client_data_ptr = (void*)new_pdn_cntx_p;

  /*----------------------------------------------------------------------
  Unbind the phys link from the flow of this Iface
  ----------------------------------------------------------------------*/
  ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(&(ds_iface_p->ps_iface)),
                          PS_EIC_NOT_SPECIFIED);
  
  /*-----------------------------------------------------------------------
   Update the new PDN context with old PDN SUBS id
  -------------------------------------------------------------------------*/
  new_pdn_cntx_p->ds_pdn_context_dyn_p->subs_id =
                            old_pdn_cntx_p->ds_pdn_context_dyn_p->subs_id;
  
 /*----------------------------------------------------------------------
  Update the APN table with the new PDN context pointer and the IP type
  being requested for this connection.
  This has to be done in two steps. First we have to change the IP support
  in APN table to reflect the fact that we are going to establish two
  Single bearer PDN connections.
  Then update the APN table with the PDN context pointer. This is because
  of the limitation in APN table. The APN table has to be redesigned
  to accommodate this case.
  ----------------------------------------------------------------------*/
  (void)ds_pdn_cntxt_update_apn_table(new_pdn_cntx_p->ds_pdn_context_dyn_p
                                      ->pdp_profile.context.apn,
                                      ip_support_granted, old_pdn_cntx_p);
  
  (void)ds_apn_table_add_pdn(
                                new_pdn_cntx_p->ds_pdn_context_dyn_p
                                ->pdp_profile.context.apn,
                                ip_support_to_request, new_pdn_cntx_p
                              );
  /*------------------------------------------------------------------------- 
   Save info_ptr in pdn_context. It may be required later for redialing the
   call.
  -------------------------------------------------------------------------*/
  if(!(old_pdn_cntx_p->ds_pdn_context_dyn_p->info_null))
  {
    new_pdn_cntx_p->ds_pdn_context_dyn_p->info_null = FALSE;
    memscpy(&(new_pdn_cntx_p->ds_pdn_context_dyn_p->info),
	    sizeof(network_params_info_type),
            &(old_pdn_cntx_p->ds_pdn_context_dyn_p->info),
            sizeof(network_params_info_type));
    DS_3GPP_MSG0_HIGH("copied info from old PDN to new PDN");
  }
  else
  {
    new_pdn_cntx_p->ds_pdn_context_dyn_p->info_null = TRUE;
  }

}  /* ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface */

/*===========================================================================
FUNCTION DSPDNCNTX_DISSOCIATE_IFACE

DESCRIPTION
  This function dissociates the PDN context from the Iface that was not
  granted in Activate Bearer Indication

PARAMETERS   
  pdn_cnt_p          - Pointer to the PDN context
  ip_support_granted - The IP support that was returned by the Network
    
DEPENDENCIES
  Should always be called from within a critical section 
  ds3gpp_global_crit_sect

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_dissociate_iface
(
  ds_pdn_context_s         *pdn_cntx_p, 
  ds_apn_ip_support_type_e ip_support_granted
)
{
  ds_3gpp_iface_s          *ds_iface_p = NULL;
  int                      ds_iface_index = -1;
  ds_pdn_context_s         *new_pdn_cntx_p = NULL;
  ps_iface_net_down_reason_type down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_cmd_type              *cmd_ptr = NULL;
  ds_pdn_context_s         **data_ptr = NULL;
  sys_modem_as_id_e_type    recorded_subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( FALSE == (ip_support_granted != DS_IPV4V6 &&
                pdn_cntx_p != NULL &&
                (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))) )
  {
    ASSERT(0);
    return;
  }

  DS_3GPP_MSG1_HIGH("Entered dissociate iface. IP support granted was: %d ",
                     ip_support_granted);

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, 
                                            &recorded_subs_id)== FALSE)
  {
    return;
  }
  /*--------------------------------------------------------------------------
    Get the DS Iface to dissociate based on the IP support granted.
    Example, if V4 was granted, the V6 DS Iface has to be dissociated.
  --------------------------------------------------------------------------*/
  switch(ip_support_granted)
  {
    case DS_IPV4_ONLY:
      down_reason = PS_NET_DOWN_REASON_IP_V4_ONLY_ALLOWED;
      ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      ds_iface_index = DS_3GPP_PDN_CONTEXT_IPV6_IFACE;
      break;

    case DS_IPV4_SINGLE_BEARER:
      down_reason = PS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY;
      ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      ds_iface_index = DS_3GPP_PDN_CONTEXT_IPV6_IFACE;
      break;

    case DS_IPV6_ONLY: 
      down_reason = PS_NET_DOWN_REASON_IP_V6_ONLY_ALLOWED;
      ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      ds_iface_index = DS_3GPP_PDN_CONTEXT_IPV4_IFACE;
      break;

    case DS_IPV6_SINGLE_BEARER:
      down_reason = PS_NET_DOWN_REASON_SINGLE_ADDR_BEARER_ONLY;
      ds_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      ds_iface_index = DS_3GPP_PDN_CONTEXT_IPV4_IFACE;
      break;
       
    default:
      DS_3GPP_MSG1_HIGH("Error while trying to dissociate. IP support returned: %d",
               ip_support_granted);
      ASSERT(0);
      break;
  }/* switch ip_support_granted */

  if(ds_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_iface_p is NULL");
    return;
  }

  ASSERT(ds_iface_index > -1 && 
         ds_iface_index <= DS_3GPP_PDN_CONTEXT_IPV6_IFACE);

  /*--------------------------------------------------------------------------- 
    Acquire lock on global_ps_crit_section before acquiring 3gpp crit section 
    to avoid deadlock when ps_global_crit_section is acquired inside 
    ds_pdn_cntxt_tearDownPSIface. 
  ---------------------------------------------------------------------------*/
  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ds3gpp_enter_global_crit_section();
  /*--------------------------------------------------------------------------
    Tear down the Iface for which IP support was not returned
    This is to be done only when the iface is out of use or if the IP support
    returned is IPV4_ONLY or IPV6_ONLY.
    In case of emergency PDN connection, teardown the other iface for which
    IP support was not returned.
  --------------------------------------------------------------------------*/
  if(PS_IFACE_IS_IN_USE(&(ds_iface_p->ps_iface)) == FALSE ||
     ip_support_granted == DS_IPV4_ONLY 
     || ip_support_granted == DS_IPV6_ONLY 
#ifdef FEATURE_EMERGENCY_PDN
     || DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == TRUE
#endif /* FEATURE_EMERGENCY_PDN */
    )
  {
    ds_pdn_cntxt_tearDownPSIface(&(ds_iface_p->ps_iface),
                                 down_reason);

    ds_3gpp_pdn_context_cleanupDSIface(ds_iface_p);

  } /* Iface to be dissociated is not in use */

  /*------------------------------------------------------------------------
    Clear the entry for the DS Iface in PDN context
  ------------------------------------------------------------------------*/
  
  pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[ds_iface_index] = NULL;

  /*--------------------------------------------------------------------------
    Remove the binding that DS Iface has towards PDN context
  --------------------------------------------------------------------------*/
  ds_iface_p->client_data_ptr = NULL;
  
  
  ds3gpp_leave_global_crit_section();
  PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);

  /*--------------------------------------------------------------------------
    If the Iface is IN USE, then we have to resend PDN connection request.
    This however will not hold good if the iface is default. Also we should 
    not resend the PDN connection request if the IP support that was returned
    is DS_IPV4_ONLY or DS_IPV6_ONLY.

    The steps are as follows:
      Allocate a new PDN context
      Cache the Profile info into new PDN context. Ensure that we set the PDP
      type to the one we want to get. Never set it as IPV4V6
      Do the binding between the PDN context and DS Iface
      Then enqueue DS3GPP_RESEND_PDN_CONN_REQ_CMD which will allocate bearer 
      and send out a new request
  --------------------------------------------------------------------------*/
  if(PS_IFACE_IS_IN_USE(&(ds_iface_p->ps_iface)) == TRUE && 
     ip_support_granted != DS_IPV4_ONLY &&
     ip_support_granted != DS_IPV6_ONLY &&
     (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == FALSE))
  {
    /*------------------------------------------------------------------------
      We need to bring up a new PDN context for this Iface that is in use
    ------------------------------------------------------------------------*/
    new_pdn_cntx_p = ds_pdn_cntxt_allocate_new_pdn_context(recorded_subs_id);

    /*---------------------------------------------------------------------- 
        We use the recorded subs id from the old PDN Context
     -----------------------------------------------------------------------*/    

    if(ds_3gpp_pdn_cntx_validate_pdn_context(new_pdn_cntx_p) == FALSE)
    {
      /*----------------------------------------------------------------------
        Restore the binding between the old PDN context and the DS Iface.
        We need this so that we can tear down the DS Iface
      ----------------------------------------------------------------------*/
      pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[ds_iface_index] = ds_iface_p;
      ds_iface_p->client_data_ptr = pdn_cntx_p;

      /*----------------------------------------------------------------------
        Resources needed are not available. So tear down this iface.
      ----------------------------------------------------------------------*/
      ds_pdn_cntxt_tearDownPSIface(&(ds_iface_p->ps_iface),
                              PS_NET_DOWN_REASON_NOT_SPECIFIED);

      /*--------------------------------------------------------------------------
        Mark the DS Iface as out of use
      --------------------------------------------------------------------------*/
      ds_iface_p->in_use = FALSE;
    }
    else
    {       
      ds_3gpp_pdn_cntxt_setup_pdn_with_ds_iface(new_pdn_cntx_p,
                                                &(pdn_cntx_p->ds_pdn_context_dyn_p
                                                ->pdp_profile),
                                                ds_iface_p,
                                                ds_iface_index,
                                                pdn_cntx_p);
    
      /*----------------------------------------------------------------------
        Change the state to COMING_UP for MO Calls. 
      ----------------------------------------------------------------------*/
      if(new_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_DOWN)
      {
        new_pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_COMING_UP;
      }

      cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
      if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
      {
        ASSERT(0);
        return;
      }

      cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_CONN_REQ_RESEND;
      data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
      *data_ptr = new_pdn_cntx_p;

      ds_put_cmd_ext(cmd_ptr);

   }   /* obtained a valid PDN context to work with*/
 }
} /* ds_pdn_cntxt_dissociate_iface */

/*===========================================================================
FUNCTION DSPDNCNTX_RECOMPUTE_IP_SUPPORT_FOR_APN

DESCRIPTION
  This function recomputes the IP support for a given APN. Recomputation is
  done based on two aspects:
  1. apriori knowledge about the IP support of an APN. This is usually 
     assumed based on the PDP type
  2. New data from the network - The new data that this function uses is
     from the PDN reject indication. If the call is accepted, there is no
     need to recompute as the assumed IP support is correct.

  This function gets the recomputed IP support and removes the PDN context
  pointer (that was rejected) from the APN table. It also updates the APN
  table so that the correct info is present in the table.

PARAMETERS
  IN
    rejected_pdn_cntx_p - PDN context that has to be removed from the table
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If APN table has an entry for this apn name
  FALSE- Otherwise

SIDE EFFECTS
  When bearer_down_indication is processed, we will call remove_entry_from_apn
  with the same PDN context once again. However, that function call will result
  in no operation since the PDN context pointer was already removed.

===========================================================================*/
void ds_pdn_cntxt_recompute_ip_support_for_apn
(
  ds_pdn_context_s  *rejected_pdn_cntx_p
)
{
  byte                     *apn = NULL;
  ds_pdn_context_s         *v4_pdn_cntx_p = NULL;
  ds_pdn_context_s         *v6_pdn_cntx_p = NULL;
  ds_apn_ip_support_type_e ip_support;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(rejected_pdn_cntx_p))
  {
    return;
  }
  /*----------------------------------------------------------------------------
    This function is used to recompute the IP support for a given APN

    This function is of use only is the present IP support is Single Bearer
    and there are two PDN context pointers in the APN table entry.

    If the support is Single Bearer and there is only one PDN context, this
    function does nothing. The APN table entry in such a case will be cleaned
    up during processing of Bearer Down indication
  ----------------------------------------------------------------------------*/

    /*------------------------------------------------------------------------
      Get the APN name from the PDN context whose connection was rejected
    ------------------------------------------------------------------------*/
    apn = rejected_pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;

    if(ds_pdn_cntxt_get_ip_support_info_for_pdn(rejected_pdn_cntx_p, &ip_support) 
                                                        == DS3G_SUCCESS)
    {
      DS_3GPP_MSG1_HIGH("PDN rej ind recvd. Updating APN tbl. Curr Support: %d",
               ip_support);

      /*----------------------------------------------------------------------
        Get the V6 and the V4 PDN context pointers.
      ----------------------------------------------------------------------*/
      if(ds_pdn_cntxt_get_v4_and_v6_pdn_context(rejected_pdn_cntx_p, 
                                                &v4_pdn_cntx_p,
                                                &v6_pdn_cntx_p
                                                ) != DS3G_SUCCESS)
      {
        DS_3GPP_MSG0_ERROR("APN tbl entry not found. Trying to get V4/V6 PDN ptr");
      }

      if( (v4_pdn_cntx_p != NULL) &&
          (v6_pdn_cntx_p != NULL)          &&
          (v4_pdn_cntx_p != v6_pdn_cntx_p) )
      {
        /*---------------------------------------------------------------------
          We have established that we have 2 different PDN contexts to the same
          APN. We will not remove the PDN context pointer from the APN table
          now since we do it while processing bearer_down_ind.

          All we do here is to set the APN IP support in the APN table.
        ---------------------------------------------------------------------*/

        /*---------------------------------------------------------------------
          Determine the IP support and update the APN table
        ---------------------------------------------------------------------*/
        if(rejected_pdn_cntx_p == v4_pdn_cntx_p)
        {
          (void)ds_pdn_cntxt_update_apn_table(apn, DS_IPV6_ONLY, v6_pdn_cntx_p);
        }
        else if (rejected_pdn_cntx_p == v6_pdn_cntx_p)
        {
          (void)ds_pdn_cntxt_update_apn_table(apn, DS_IPV4_ONLY, v4_pdn_cntx_p);
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Rejected PDN cntxt does not match any in APN tbl");
        }
      } //Switch based on existing IP Support

    } // Found the APN entry in the table


}/* ds_pdn_cntxt_recompute_ip_support_for_apn */


/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_PDN_CONTEXT_INSTANCE

DESCRIPTION
  This function returns the instance of the pdn context passed into the fn.
  The instance is simply the index of the pdn context in the table of
  PDN objects

PARAMETERS   
  pdn_cntx_p - Pointer to the PDN context whose index is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  Int - Instance is positive if found, -1 otherwise

SIDE EFFECTS
  None

===========================================================================*/
int ds_pdn_cntxt_get_pdn_context_instance
(
  ds_pdn_context_s*           pdn_cntx_p
)
{
  int loop_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find the PDN context in the table that corresponds to the pointer passed
    in
  -------------------------------------------------------------------------*/
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if(pdn_cntx_p == &ds_pdn_context_tbl[loop_index])
    {
      break;
    }
  }

  /*-------------------------------------------------------------------------
    If the loop_index has hit max index of the table, set the value to -1
    This indicates that the PDN context could not be located in the table
  -------------------------------------------------------------------------*/
  if(loop_index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    loop_index = -1;
  }

  return loop_index;
} /* ds_pdn_cntxt_get_pdn_context_instance */

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_PDN_CONTEXT_PROFILES

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.
  
PARAMETERS   
  profile_list_ptr  - list of profile numbers which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntx_get_all_pdn_context_profiles
(
  sys_modem_as_id_e_type                 subs_id,
  ds_3gpp_atif_prim_profile_list_type*   profile_list_ptr
)
{
  int loop_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(profile_list_ptr != NULL);
  memset(profile_list_ptr,0x0,sizeof (ds_3gpp_atif_prim_profile_list_type));

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  /*-------------------------------------------------------------------------
    Iterate through the pdn context table and update profile_list
    with all PDN profile number which are in active state..
  -------------------------------------------------------------------------*/
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if(ds_3gpp_pdn_cntx_validate_pdn_context_internal(
                                              &ds_pdn_context_tbl[loop_index]) &&
      (subs_id == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->subs_id)&&
       (DS_PDN_CONTEXT_STATE_DOWN != ds_pdn_context_tbl[loop_index].
        ds_pdn_context_dyn_p->state )&&
       (DS_PDN_CONTEXT_STATE_MAX != ds_pdn_context_tbl[loop_index].
        ds_pdn_context_dyn_p->state ))
    {
      profile_list_ptr->data_ptr[profile_list_ptr->data_len++] = 
        ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
        pdp_profile_num;
    }
  }
}/* ds_pdn_cntx_get_all_pdn_context_profiles */

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_PDN_CONTEXT_PROFILES_FOR_DDS

DESCRIPTION
  This function returns the list of all primary Profile number that are 
  currently active.
  
PARAMETERS   
  profile_list_ptr  - list of profile numbers which are active.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntx_get_all_pdn_context_profiles_for_dds
(
  ds_3gpp_atif_prim_profile_list_type*   profile_list_ptr
)
{
  int loop_index;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(profile_list_ptr != NULL);
  memset(profile_list_ptr,0x0,sizeof (ds_3gpp_atif_prim_profile_list_type));

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  /*-------------------------------------------------------------------------
    Iterate through the pdn context table and update profile_list
    with all PDN profile number which are in active state..
  -------------------------------------------------------------------------*/
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if(ds_3gpp_pdn_cntx_validate_pdn_context_internal(
                                              &ds_pdn_context_tbl[loop_index]) &&
      (subs_id == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->subs_id)&&
       (DS_PDN_CONTEXT_STATE_DOWN != ds_pdn_context_tbl[loop_index].
        ds_pdn_context_dyn_p->state )&&
       (DS_PDN_CONTEXT_STATE_MAX != ds_pdn_context_tbl[loop_index].
        ds_pdn_context_dyn_p->state ))
    {
      profile_list_ptr->data_ptr[profile_list_ptr->data_len++] = 
        ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
        pdp_profile_num;
    }
  }
}/* ds_pdn_cntx_get_all_pdn_context_profiles_for_dds */

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ALL_IFACE_STATS

DESCRIPTION
  This function returns the cumulative rx and tx byte statistics for ifaces
  associated with a PDN context.

PARAMETERS   
  cm_call_id[IN]     - The call_id.
  txcntp[OUT]        - Pointer to Tx byte count.
  rxcntp[OUT]        - Pointer to Rx byte count.

DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - On successfully returning iface byte count.
  DS3G_FAILURE - On failure to return iface byte count.

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_get_iface_stats
(
  cm_call_id_type cm_call_id,
  uint32          *rxcntp,
  uint32          *txcntp
)
{
  uint64 temp_txbytes = 0;
  uint64 temp_rxbytes = 0;
  errno_enum_type ret_val;
  ds_pdn_context_s *pdn_cntxt_ptr = NULL;
  ds_bearer_context_s *bearer_context_p = NULL;
  ds_3gpp_iface_s  *ds3gpp_iface_v4_p = NULL ;   /*  v4 Interface ptr for the call */
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s  *ds3gpp_iface_v6_p = NULL ;   /*  v6 Interface ptr for the call */
#endif
  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntx_get_iface_stats is called, cm_call_id: %d",
                     cm_call_id);
  if (NULL == txcntp || NULL == rxcntp)
  {
    DS_3GPP_MSG0_ERROR("Tx and Rx byte count pointers passed are NULL");
    return DS3G_FAILURE;
  }
  
  *txcntp = 0;
  *rxcntp = 0;

  PS_ENTER_CRIT_SECTION(&global_ps_crit_section);
  ds3gpp_enter_global_crit_section();
  bearer_context_p = ds_bearer_cntxt_get_bearer_from_call_id(cm_call_id);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    ds3gpp_leave_global_crit_section();
    PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
    return DS3G_FAILURE;
  }
  else
  {
    pdn_cntxt_ptr = (ds_pdn_context_s *)(bearer_context_p->ds_bearer_context_dyn_p->
                                          pdn_context_ptr);
    DS_3GPP_MSG1_HIGH("PDN context ptr is 0x%x", pdn_cntxt_ptr);
    
    if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_ptr))
    {
      ds3gpp_leave_global_crit_section();
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return DS3G_FAILURE;
    }
    else
    {
      ds3gpp_iface_v4_p 
        = pdn_cntxt_ptr->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];
      if (NULL == ds3gpp_iface_v4_p)
      {
        DS_3GPP_MSG1_ERROR("V4 iface ptr for pdn context 0x%x is NULL",
                            pdn_cntxt_ptr);
      }
      else
      {
        ret_val = ps_stat_get_iface(PS_STAT_IFACE_BYTES_RX, 
                                    (void *)&(ds3gpp_iface_v4_p->ps_iface),
                                    (void *)&(temp_rxbytes), 
                                    sizeof(temp_rxbytes));        
        if ( ret_val != E_SUCCESS )
        {
          DS_3GPP_MSG2_ERROR("ps_stat_get_iface returns error %d for iface: 0x%x", 
                              ret_val, 
                              (ps_iface_type *)&(ds3gpp_iface_v4_p->ps_iface));
          ds3gpp_leave_global_crit_section();
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return DS3G_FAILURE;
        }
        *rxcntp += (uint32)temp_rxbytes;  
        DS_3GPP_MSG1_HIGH("V4: Rx bytes returned is %d", temp_rxbytes);

        ret_val = ps_stat_get_iface(PS_STAT_IFACE_BYTES_TX, 
                                    (void *)&(ds3gpp_iface_v4_p->ps_iface),
                                    (void *)&(temp_txbytes), 
                                    sizeof(temp_txbytes));
        if ( ret_val != E_SUCCESS )
        {
          DS_3GPP_MSG2_ERROR("ps_stat_get_iface returns error %d for iface: 0x%x", 
                              ret_val, 
                              (ps_iface_type *)&(ds3gpp_iface_v4_p->ps_iface));
          ds3gpp_leave_global_crit_section();
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return DS3G_FAILURE;
        }
        *txcntp += (uint32)temp_txbytes;
        DS_3GPP_MSG1_HIGH("V4: Tx bytes returned is %d", temp_txbytes);
      }

#ifdef FEATURE_DATA_PS_IPV6
      ds3gpp_iface_v6_p 
        = pdn_cntxt_ptr->ds_pdn_context_dyn_p->
        ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];
      if (NULL == ds3gpp_iface_v6_p)
      {
        DS_3GPP_MSG3_ERROR("V6 iface ptr for pdn context 0x%x is NULL",
                            pdn_cntxt_ptr, 0, 0);
      }
      else
      {
        temp_txbytes = 0;
        temp_rxbytes = 0;
        ret_val = ps_stat_get_iface(PS_STAT_IFACE_BYTES_RX, 
                                    (void *)&(ds3gpp_iface_v6_p->ps_iface), 
                                    (void *)&(temp_rxbytes), 
                                    sizeof(temp_rxbytes));
        if ( ret_val != E_SUCCESS )
        {
          DS_3GPP_MSG2_ERROR("ps_stat_get_iface returns error %d for iface: 0x%x", 
                              ret_val, 
                              (ps_iface_type *)&(ds3gpp_iface_v6_p->ps_iface));
          ds3gpp_leave_global_crit_section();
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return DS3G_FAILURE;
        }
        *rxcntp += (uint32)temp_rxbytes;
        DS_3GPP_MSG1_HIGH("V6: Rx bytes returned is %d", temp_rxbytes);

        ret_val = ps_stat_get_iface(PS_STAT_IFACE_BYTES_TX, 
                                    (void *)&(ds3gpp_iface_v6_p->ps_iface), 
                                    (void *)&(temp_txbytes), 
                                    sizeof(temp_txbytes));
        if ( ret_val != E_SUCCESS )
        {
          DS_3GPP_MSG2_ERROR("ps_stat_get_iface returns error %d for iface: 0x%x", 
                              ret_val, 
                              (ps_iface_type *)&(ds3gpp_iface_v6_p->ps_iface));
          ds3gpp_leave_global_crit_section();
          PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
          return DS3G_FAILURE;
        }
        *txcntp += (uint32)temp_txbytes;
        DS_3GPP_MSG1_HIGH("V6: Tx bytes returned is %d", temp_txbytes);
      }
#endif /* FEATURE_DATA_PS_IPV6 */

      DS_3GPP_MSG2_HIGH("Cumulative byte count: Tx: %d; Rx: %d",
                         *txcntp, *rxcntp);
      if (ds3gpp_iface_v6_p == NULL && ds3gpp_iface_v4_p == NULL)
      {
        DS_3GPP_MSG0_ERROR("Both the DS iface ptrs are NULL");
        ds3gpp_leave_global_crit_section();
        PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
        return DS3G_FAILURE;
      }
      ds3gpp_leave_global_crit_section();
      PS_LEAVE_CRIT_SECTION(&global_ps_crit_section);
      return DS3G_SUCCESS;
    }
  }
} /* ds_3gpp_pdn_cntx_get_iface_stats */

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_PDN_CONTEXT_FROM_CID

DESCRIPTION
  Retrieve the PDN context from context id.

PARAMETERS
  cid              - context id.
.
DEPENDENCIES
  None.

RETURN VALUE
  NULL          - Returns NULL if no valid pdn context pointers
                else valid pdn_context_pointer.
  
SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s* ds_pdn_cntxt_get_pdn_context_from_cid
( 
  uint32                   cid,
  sys_modem_as_id_e_type   subs_id 
)
{
  uint8 loop = 0;
  ds_pdn_context_s   *pdn_cntxt_p = NULL;

  for(loop =0; loop < DS_3GPP_MAX_PDN_CONTEXT;loop++)
  {
    if( ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[loop]) &&
        (cid == ds_pdn_context_tbl[loop].ds_pdn_context_dyn_p->pdp_profile_num)&&
        (subs_id == ds_pdn_context_tbl[loop].ds_pdn_context_dyn_p->subs_id))
    {
      pdn_cntxt_p = &ds_pdn_context_tbl[loop];
      break;
    }
  }
  return pdn_cntxt_p;
}/* ds_pdn_cntxt_get_pdn_context_from_cid */

/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_FROM_PDP_GROUP_ID

DESCRIPTION
  This function is used to find the pdn context, given pdp group id

PARAMETERS
  IN
    pdp_group_id - pdp group id provided by CM

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the pdn context if there is a pdp group id match
  NULL if there is no match

SIDE EFFECTS
  None.

===========================================================================*/
ds_pdn_context_s* ds_pdn_cntxt_get_pdn_from_pdp_group_id(byte pdp_group_id)
{
  uint8 loop = 0;
  ds_pdn_context_s   *pdn_cntxt_p = NULL;

  for(loop =0; loop < DS_3GPP_MAX_PDN_CONTEXT;loop++)
  {
    if( ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[loop]) &&
        (pdp_group_id == ds_pdn_context_tbl[loop].ds_pdn_context_dyn_p->
          pdp_group_id))
    {
      pdn_cntxt_p = &ds_pdn_context_tbl[loop];
      break;
    }
  }
  return pdn_cntxt_p;
} /* ds_pdn_cntxt_get_pdn_from_pdp_group_id() */

/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_STATE

DESCRIPTION
  This function returns the PDN state of the Profile number passed into the fn.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  PDN state - if no valid instance found, 
              return DS_PDN_CONTEXT_STATE_DOWN state

SIDE EFFECTS
  None

===========================================================================*/
ds_pdn_context_state_e ds_pdn_cntxt_get_pdn_context_state
(
  uint32                    profile_num,
  sys_modem_as_id_e_type    subs_id 
)
{

  ds_pdn_context_state_e    state = DS_PDN_CONTEXT_STATE_DOWN;
  ds_pdn_context_s*  pdn_cntxt_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find the PDN context in the table that corresponds to the 
    profile number passed.
  -------------------------------------------------------------------------*/
  pdn_cntxt_p = ds_pdn_cntxt_get_pdn_context_from_cid(profile_num, subs_id);

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {
    state = pdn_cntxt_p->ds_pdn_context_dyn_p->state;
  }

  /*-------------------------------------------------------------------------
    pdn_cntxt_p == NULL, indicates that the PDN context
    could not be located in the table
  -------------------------------------------------------------------------*/
  return state;
} /* ds_pdn_cntxt_get_pdn_context_state */

/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_CONTEXT_STATE_FOR_DDS

DESCRIPTION
  This function returns the PDN state of the Profile number passed into the fn.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  PDN state - if no valid instance found, 
              return DS_PDN_CONTEXT_STATE_DOWN state

SIDE EFFECTS
  None

===========================================================================*/
ds_pdn_context_state_e ds_pdn_cntxt_get_pdn_context_state_for_dds
(
  uint32                    profile_num
)
{
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  ds_pdn_context_state_e    state = DS_PDN_CONTEXT_STATE_DOWN;
  ds_pdn_context_s*  pdn_cntxt_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Find the PDN context in the table that corresponds to the 
    profile number passed.
  -------------------------------------------------------------------------*/
  pdn_cntxt_p = ds_pdn_cntxt_get_pdn_context_from_cid(profile_num, subs_id);

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {
    state = pdn_cntxt_p->ds_pdn_context_dyn_p->state;
  }

  /*-------------------------------------------------------------------------
    pdn_cntxt_p == NULL, indicates that the PDN context
    could not be located in the table
  -------------------------------------------------------------------------*/
  return state;
} /* ds_pdn_cntxt_get_pdn_context_state_for_dds */

/*===========================================================================
FUNCTION DS_PDN_CNTX_GET_ATI_DYNAMIC_INFO

DESCRIPTION
  This function returns the PDN state info of the Profile number 
  passed into the function. It wil return v4,v6 iface pointer,
  eps_bearer_id and APN.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if no valid instance found, 
  TRUE - if valid pdn_state info is present

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_ati_dynamic_info
(
  uint32                             p_cid,
  sys_modem_as_id_e_type             subs_id,
  ds_3gpp_ati_pdn_dynamic_info_type *pdn_state_info_ptr
)
{
  ds_bearer_context_s*     bearer_context_p = NULL;
  ds_pdn_context_s*        pdn_cntx_p = NULL;
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s* eps_bearer_context_p = NULL;
#endif /* FEATURE_DATA_LTE */
  ps_iface_type            *v4_iface_ptr = NULL;
  ps_iface_type            *v6_iface_ptr = NULL;
  byte                     *apn_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(pdn_state_info_ptr != NULL);

  pdn_cntx_p = ds_pdn_cntxt_get_pdn_context_from_cid(p_cid, subs_id);

  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
       || (pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_UP))
  {
    return FALSE;
  }
  /* clear all value */
  memset(pdn_state_info_ptr,0x0,sizeof(ds_3gpp_ati_pdn_dynamic_info_type));
  
  /*---------------------------------------------------------------------------
    Get the default bearer context pointer from the PDN context.
    //TO DO -- remove the access of eps_bearer adn add VOBJ 
               to return bearer id / NSAPI as the case be.
  ---------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p->
                                             def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
#ifdef FEATURE_DATA_LTE
  if (ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_LTE)
  {
    eps_bearer_context_p = (ds_eps_bearer_context_s*)
                      DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);
    if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p))
    {
      pdn_state_info_ptr->bearer_id = 
              eps_bearer_context_p->ds_eps_bearer_context_dyn_p->eps_bearer_id;
    }
    else
    {
      return FALSE;
    }
  }
  else
#endif /* FEATURE_DATA_LTE */
  {
    pdn_state_info_ptr->bearer_id = bearer_context_p->
      ds_bearer_context_dyn_p->nsapi;
  }
  DS_3GPP_MSG1_HIGH("Bearer ID  %d",pdn_state_info_ptr->bearer_id);

  /* Get APN */

  /*--------------------------------------------------------------------------
    Get the resolved APN if applicable
  --------------------------------------------------------------------------*/
  if(strlen((const char*)(pdn_cntx_p->ds_pdn_context_dyn_p->
                          pdp_profile.context.apn)) == 0)
  {
    if(ds_pdn_cntxt_get_resolved_apn_name(
                       pdn_cntx_p, 
                       &(apn_ptr)) == FALSE)
    {
      DS_3GPP_MSG0_HIGH("Resolved APN unavailable in the APN table");
    }
    else
    {
      memscpy((pdn_state_info_ptr->apn), 
	       sizeof(pdn_state_info_ptr->apn),
               apn_ptr,
               sizeof(pdn_state_info_ptr->apn));
    }
  }
  else
  {
    memscpy((pdn_state_info_ptr->apn), sizeof(pdn_state_info_ptr->apn),
            (pdn_cntx_p->ds_pdn_context_dyn_p->
             pdp_profile.context.apn),
             sizeof(pdn_state_info_ptr->apn));
  }

  /* Get V4 iface pointer */
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
     [DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL)
  {
    v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    pdn_state_info_ptr->v4_iface_ptr = v4_iface_ptr;
  }

  /* Get V6 iface pointer */
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
     [DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL)
  {
    v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);
    
    pdn_state_info_ptr->v6_iface_ptr = v6_iface_ptr;
  }
  return TRUE;
}/* ds_pdn_cntx_get_ati_dynamic_info */


/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_ATI_DYNAMIC_INFO_FOR_DDS

DESCRIPTION
  This function returns the PDN state info of the Profile number 
  passed into the function. It wil return v4,v6 iface pointer,
  eps_bearer_id and APN.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if no valid instance found, 
  TRUE - if valid pdn_state info is present

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_ati_dynamic_info_for_dds
(
  uint32                             p_cid,
  ds_3gpp_ati_pdn_dynamic_info_type *pdn_state_info_ptr
)
{
  ds_bearer_context_s*     bearer_context_p = NULL;
  ds_pdn_context_s*        pdn_cntx_p = NULL;
#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_context_s* eps_bearer_context_p = NULL;
#endif /* FEATURE_DATA_LTE */
  ps_iface_type            *v4_iface_ptr = NULL;
  ps_iface_type            *v6_iface_ptr = NULL;
  byte                     *apn_ptr = NULL;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  ASSERT(pdn_state_info_ptr != NULL);

  pdn_cntx_p = ds_pdn_cntxt_get_pdn_context_from_cid(p_cid, subs_id);

  if((!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
       || (pdn_cntx_p->ds_pdn_context_dyn_p->state != DS_PDN_CONTEXT_STATE_UP))
  {
    return FALSE;
  }
  /* clear all value */
  memset(pdn_state_info_ptr,0x0,sizeof(ds_3gpp_ati_pdn_dynamic_info_type));
  
  /*---------------------------------------------------------------------------
    Get the default bearer context pointer from the PDN context.
    //TO DO -- remove the access of eps_bearer adn add VOBJ 
               to return bearer id / NSAPI as the case be.
  ---------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)(pdn_cntx_p->ds_pdn_context_dyn_p->
                                             def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return FALSE;
  }
#ifdef FEATURE_DATA_LTE
  if (ds3gpp_get_current_network_mode(subs_id) == SYS_SYS_MODE_LTE)
  {
    eps_bearer_context_p = (ds_eps_bearer_context_s*)
                      DS_BEARER_CTXT_VOBJ(bearer_context_p, SYS_SYS_MODE_LTE);
    if(ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p))
    {
      pdn_state_info_ptr->bearer_id = 
              eps_bearer_context_p->ds_eps_bearer_context_dyn_p->eps_bearer_id;
    }
    else
    {
      return FALSE;
    }
  }
  else
#endif /* FEATURE_DATA_LTE */
  {
    pdn_state_info_ptr->bearer_id = bearer_context_p->
      ds_bearer_context_dyn_p->nsapi;
  }
  DS_3GPP_MSG1_HIGH("Bearer ID  %d",pdn_state_info_ptr->bearer_id);

  /* Get APN */

  /*--------------------------------------------------------------------------
    Get the resolved APN if applicable
  --------------------------------------------------------------------------*/
  if(strlen((const char*)(pdn_cntx_p->ds_pdn_context_dyn_p->
                          pdp_profile.context.apn)) == 0)
  {
    if(ds_pdn_cntxt_get_resolved_apn_name(
                       pdn_cntx_p, 
                       &(apn_ptr)) == FALSE)
    {
      DS_3GPP_MSG0_HIGH("Resolved APN unavailable in the APN table");
    }
    else
    {
      memscpy((pdn_state_info_ptr->apn), 
	       sizeof(pdn_state_info_ptr->apn),
               apn_ptr,
               sizeof(pdn_state_info_ptr->apn));
    }
  }
  else
  {
    memscpy((pdn_state_info_ptr->apn), sizeof(pdn_state_info_ptr->apn),
            (pdn_cntx_p->ds_pdn_context_dyn_p->
             pdp_profile.context.apn),
             sizeof(pdn_state_info_ptr->apn));
  }

  /* Get V4 iface pointer */
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
     [DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL)
  {
    v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    pdn_state_info_ptr->v4_iface_ptr = v4_iface_ptr;
  }

  /* Get V6 iface pointer */
  if(pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
     [DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL)
  {
    v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);
    
    pdn_state_info_ptr->v6_iface_ptr = v6_iface_ptr;
  }
  return TRUE;
}/* ds_pdn_cntxt_get_ati_dynamic_info_for_dds */

/*===========================================================================
FUNCTION DSPDNCNTX_GET_PDN_CONTEXT_PDP_ADDR

DESCRIPTION
  This function returns the PDP addr of the PDN connection for which 
  the Profile number passed.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if the call succeds
  FALSE : if it fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_pdn_context_pdp_addr
(
  uint32                    profile_num,
  sys_modem_as_id_e_type    subs_id,
  ds_umts_pdp_addr_type     *pdp_addr_ptr
)
{
  int loop_index;
  ip_addr_type pdp_addr;
  sint15       ps_error_no;
  ps_iface_type            *ps_iface_ptr = NULL;
  ds_umts_pdp_addr_type  ds_pdp_addr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(pdp_addr_ptr != NULL);
  /*-------------------------------------------------------------------------
    Find the PDN context in the table that corresponds to the 
    profile number passed.
  -------------------------------------------------------------------------*/
  memset ((void*)&ds_pdp_addr,0x0,sizeof(ds_umts_pdp_addr_type));
  
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if( ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[loop_index]) &&
       (profile_num == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p
        ->pdp_profile_num) &&
        (subs_id == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->subs_id)&&
        (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->state 
         != DS_PDN_CONTEXT_STATE_DOWN) )
    {
      if ( ( (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.
                context.pdp_type == DS_UMTS_PDP_IPV4 ) ||
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.
                context.pdp_type == DS_UMTS_PDP_IPV4V6)) &&
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
               ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL) )
      {
        
        ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(&(ds_pdn_context_tbl[loop_index]));
        if ( 0 == ps_iface_ioctl( ps_iface_ptr,
                         PS_IFACE_IOCTL_GET_IPV4_ADDR,
                         &pdp_addr,
                         &ps_error_no) )
        {
          if( 0 != pdp_addr.addr.v4)
          {
            ds_pdp_addr.ip_vsn = DS_IP_V4;
            memscpy((void *)(&ds_pdp_addr.pdp_addr.pdp_addr_ipv4),
                    sizeof(uint32),
                    &pdp_addr.addr.v4,sizeof( uint32));
          }
        }
        else
        {
          DS_3GPP_MSG0_ERROR("PS_iface call failed, return false");
          return FALSE;
        }
      }
      if ( ((ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
               context.pdp_type == DS_UMTS_PDP_IPV6 ) ||
             ( ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
               context.pdp_type == DS_UMTS_PDP_IPV4V6)) &&
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
               ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL) )
      {
        ps_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(&(ds_pdn_context_tbl[loop_index]));
        if ( 0 == ps_iface_ioctl( ps_iface_ptr,
                         PS_IFACE_IOCTL_GET_IPV6_ADDR,
                         &pdp_addr,
                         &ps_error_no) )
        {
          if ( 0 != pdp_addr.addr.v6[1] )
          {
            ds_pdp_addr.ip_vsn = (ds_ip_version_enum_type)
                  ((int) ds_pdp_addr.ip_vsn + (int)DS_IP_V6);
            memscpy((void *)(&ds_pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0]),
		    (sizeof(uint64)*2),
                    &pdp_addr.addr.v6[0],
                    (sizeof(uint64)*2));
          }
        }
        else
        {
          DS_3GPP_MSG0_ERROR("PS_iface call failed, return false");
          return FALSE;
        }
      }
      if(ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
              context.pdp_type == DS_UMTS_PDP_PPP )
      {
        ds_pdp_addr.ip_vsn = DS_IP_V4;
      }
      memscpy(pdp_addr_ptr, sizeof(ds_umts_pdp_addr_type),
              &ds_pdp_addr,
              sizeof(ds_umts_pdp_addr_type));
      return TRUE;
    }
  }
  /*-------------------------------------------------------------------------
    If the loop_index has hit max index of the table, return DOWN state.
    This indicates that the PDN context could not be located in the table
  -------------------------------------------------------------------------*/
  return FALSE;
} /* ds_pdn_cntxt_get_pdn_context_pdp_addr */


/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_PDN_CONTEXT_PDP_ADDR_FOR_DDS

DESCRIPTION
  This function returns the PDP addr of the PDN connection for which 
  the Profile number passed.
  
PARAMETERS   
  profile_num - Profile number of the PDP context whose PDN state is needed
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  : if the call succeds
  FALSE : if it fails

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_pdn_context_pdp_addr_for_dds
(
  uint32                    profile_num,
  ds_umts_pdp_addr_type     *pdp_addr_ptr
)
{
  int loop_index;
  ip_addr_type pdp_addr;
  sint15       ps_error_no;
  ps_iface_type            *ps_iface_ptr = NULL;
  ds_umts_pdp_addr_type  ds_pdp_addr;
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(pdp_addr_ptr != NULL);
  /*-------------------------------------------------------------------------
    Find the PDN context in the table that corresponds to the 
    profile number passed.
  -------------------------------------------------------------------------*/
  memset ((void*)&ds_pdp_addr,0x0,sizeof(ds_umts_pdp_addr_type));
  
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    if( ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[loop_index]) &&
       (profile_num == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p
        ->pdp_profile_num) &&
        (subs_id == ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->subs_id)&&
        (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->state 
         != DS_PDN_CONTEXT_STATE_DOWN) )
    {
      if ( ( (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.
                context.pdp_type == DS_UMTS_PDP_IPV4 ) ||
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.
                context.pdp_type == DS_UMTS_PDP_IPV4V6)) &&
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
               ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE] != NULL) )
      {
        
        ps_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(&(ds_pdn_context_tbl[loop_index]));
        if ( 0 == ps_iface_ioctl( ps_iface_ptr,
                         PS_IFACE_IOCTL_GET_IPV4_ADDR,
                         &pdp_addr,
                         &ps_error_no) )
        {
          if( 0 != pdp_addr.addr.v4)
          {
            ds_pdp_addr.ip_vsn = DS_IP_V4;
            memscpy((void *)(&ds_pdp_addr.pdp_addr.pdp_addr_ipv4),
                    sizeof(uint32),
                    &pdp_addr.addr.v4,sizeof( uint32));
          }
        }
        else
        {
          DS_3GPP_MSG0_ERROR("PS_iface call failed, return false");
          return FALSE;
        }
      }
      if ( ((ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
               context.pdp_type == DS_UMTS_PDP_IPV6 ) ||
             ( ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
               context.pdp_type == DS_UMTS_PDP_IPV4V6)) &&
             (ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->
               ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE] != NULL) )
      {
        ps_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(&(ds_pdn_context_tbl[loop_index]));
        if ( 0 == ps_iface_ioctl( ps_iface_ptr,
                         PS_IFACE_IOCTL_GET_IPV6_ADDR,
                         &pdp_addr,
                         &ps_error_no) )
        {
          if ( 0 != pdp_addr.addr.v6[1] )
          {
            ds_pdp_addr.ip_vsn = (ds_ip_version_enum_type)
                  ((int) ds_pdp_addr.ip_vsn + (int)DS_IP_V6);
            memscpy((void *)(&ds_pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0]),
		    (sizeof(uint64)*2),
                    &pdp_addr.addr.v6[0],
                    (sizeof(uint64)*2));
          }
        }
        else
        {
          DS_3GPP_MSG0_ERROR("PS_iface call failed, return false");
          return FALSE;
        }
      }
      if(ds_pdn_context_tbl[loop_index].ds_pdn_context_dyn_p->pdp_profile.\
              context.pdp_type == DS_UMTS_PDP_PPP )
      {
        ds_pdp_addr.ip_vsn = DS_IP_V4;
      }
      memscpy(pdp_addr_ptr, sizeof(ds_umts_pdp_addr_type),
              &ds_pdp_addr,
              sizeof(ds_umts_pdp_addr_type));
      return TRUE;
    }
  }
  /*-------------------------------------------------------------------------
    If the loop_index has hit max index of the table, return DOWN state.
    This indicates that the PDN context could not be located in the table
  -------------------------------------------------------------------------*/
  return FALSE;
} /* ds_pdn_cntxt_get_pdn_context_pdp_addr_for_dds */


/*===========================================================================
FUNCTION DSPDNCNTX_ALLOCATE_DS_IFACE

DESCRIPTION
  This function is used to allocate a DS Iface primarily for Dual IP bearer
  scenarios.

  It loops through all the DS Ifaces and allocates one that is not marked as
  being in use. Since the allocation happens in Route ACL post processing, it
  could happen in a different task context. As a result, allocation and
  deallocation should be protected under critical sections.

PARAMETERS   
  None.
    
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_iface_s* - Pointer to the DS iface allocated.

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_iface_s* ds_pdn_cntxt_allocate_ds_iface(void)
{
  int             loop_index;
  ds_3gpp_iface_s *ds_iface_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3gpp_enter_global_crit_section();
  /*------------------------------------------------------------------------
    Loop over all DS Ifaces to locate a free one
  ------------------------------------------------------------------------*/
  for(loop_index = 0; loop_index < DS_3GPP_MAX_PS_IFACES; loop_index++)
  {
    if (ds_pdn_context_this_iface_is_for_emergency_pdn(&ds_iface_tbl[loop_index]))
    {
      //Don't use the Ifaces reserved for emergency calls.
      continue;
    }
    if(ds_iface_tbl[loop_index].in_use == FALSE)
    {
      /*----------------------------------------------------------------------
        We have located a free Iface. Break from the loop and allocate it
      ----------------------------------------------------------------------*/
      ds_iface_p = &ds_iface_tbl[loop_index];
      break;
    }
  }

  if(ds_iface_p != NULL)
  {
    ds_iface_p->in_use = TRUE;
  }
  ds3gpp_leave_global_crit_section();

  return ds_iface_p;
} /* ds_pdn_cntxt_allocate_ds_iface */

/*===========================================================================
FUNCTION ds_pdn_cntxt_get_companion_iface_for_emergency_calls

DESCRIPTION
  1 v4 Iface and 1 v6 Iface are always reserved for making emergency calls.
  This function returns the companion Iface 

PARAMETERS   
  ds_iface_s - The passed in Iface whose companion will be returned.
    
DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_iface_s* - Pointer to the companion Iface.
  NULL - If this is the passed in Iface is not the Iface reserved to make
         emergency calls.

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_iface_s* ds_pdn_cntxt_get_companion_iface_for_emergency_calls
(
  ds_3gpp_iface_s *ds_iface_s
)
{
#ifndef FEATURE_DATA_DISABLE_MULTI_PDN
#ifdef FEATURE_EMERGENCY_PDN
  if (ds_iface_s == &ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V4_PS_IFACE_IDX])
  {
    return &ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V6_PS_IFACE_IDX];
  }
  else if (ds_iface_s == &ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V6_PS_IFACE_IDX])
  {
    return &ds_iface_tbl[DS_3GPP_EMERGENCY_PDN_V4_PS_IFACE_IDX];
  }
#else
  DS_3GPP_MSG0_HIGH("No Companion iface could be found, "
                    "Emergency PDN Support not available ");
#endif /* FEATURE_EMERGENC_PDN */
#endif /* FEATURE_DATA_DISABLE_MULTI_PDN */
    return NULL;

} /* ds_pdn_cntxt_get_companion_iface_for_emergency_calls */

/*===========================================================================
FUNCTION DSPDNCNTX_IS_UE_ATTACHED

DESCRIPTION
  This function is used to find out if the UE is already attached to any PDN
  It traverses through the list of PDNs and finds out if any of the PDN 
  contexts is in UP state

  If at least one PDN is up and the attach complete has been sent over the air
  and acknowledged by L2, this function will return TRUE.

PARAMETERS   
  subs_id: Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  DS3G_SUCCESS - if the UE is connected to atleast one PDN
  DS3G_FAILURE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_is_ue_attached
(
  sys_modem_as_id_e_type    subs_id
)
{
  int                       loop_index;
  boolean                   ret_val = FALSE;
  ds_pdn_context_s          *pdn_context_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(loop_index = 0; loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++) 
  {
    pdn_context_p = &(ds_pdn_context_tbl[loop_index]);

    if(!ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_context_p, subs_id))
    {
      continue;
    }

    DS_3GPP_MSG2_MED("Check whether UE is attached. Subs id(CM): %d, "
                     "PDN State: %d",
                     subs_id, pdn_context_p->ds_pdn_context_dyn_p->state);

    if(pdn_context_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
    {
      ret_val = TRUE;
    }
  }
  return (ret_val && ds3gpp_get_is_attach_complete_flag(subs_id));
}

/*===========================================================================
FUNCTION DSPDNCNTX_HANDLE_CNTXT_TRANSFER_IND

DESCRIPTION
  This function is used to invoke any target-rat specific operations on
  receiving context-transfer indication.

PARAMETERS   
  target rat
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_handle_cntxt_transfer_ind
(
  sys_sys_mode_e_type    target_rat,
  sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_DATA_LTE
  if (target_rat == SYS_SYS_MODE_LTE)
  {
    ds_eps_pdn_cntxt_handle_cntxt_txfr_ind(subs_id);
  }
#endif

}

/*===========================================================================
FUNCTION DSPDNCNTX_UNBIND_DEFAULT_FLOWS

DESCRIPTION
  This function is used to unbind the phys link from all the default flows
  associated with the Ifaces of a given PDN context

PARAMETERS   
  void* - pointer to the PDN context
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_unbind_default_flows(void* input_param_p)
{
  int              iface_index;
  ds_3gpp_iface_s  *ds_3gpp_iface_p;
  ds_pdn_context_s *pdn_cntx_p = (ds_pdn_context_s*) input_param_p;
  ps_flow_type     *ps_flow_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  for(iface_index = 0; 
      iface_index < DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT;
      iface_index++)
  {
    ds_3gpp_iface_p = pdn_cntx_p->ds_pdn_context_dyn_p->
      ds_3gpp_iface_tbl[iface_index];
    if (ds_3gpp_iface_p == NULL)
    {
      continue;
    }

    ps_flow_ptr = PS_IFACE_GET_DEFAULT_FLOW(&(ds_3gpp_iface_p->ps_iface));
    
    if(ps_flow_ptr != NULL)
    {
      PS_FLOW_RESET_CAPABILITY(ps_flow_ptr, PS_FLOW_CAPABILITY_DATA_DISALLOWED);
    }

    ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(&(ds_3gpp_iface_p->ps_iface)),
                        PS_EIC_NOT_SPECIFIED);

  }/* for each Iface in the PDN context */

} /* ds_pdn_cntxt_unbind_default_flows */


/*===========================================================================
FUNCTION DSPDNCNTX_SET_HANDOVER_ATTACH_FLAG

DESCRIPTION
  This function sets the handover attach flag maintained in the PDN context
  This flag will be used to findout whether we have to send initial request
  or handover attach while in the EPS mode of operation

PARAMETERS   
  handover_attach_flag - 
    
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void ds_pdn_cntxt_set_handover_attach_flag
(
  ds_pdn_context_s *pdn_cntx_p,
  boolean          handover_attach_flag
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return;
  }

  pdn_cntx_p->ds_pdn_context_dyn_p->handover_attach_flag = handover_attach_flag;
  return;
} /* ds_pdn_cntxt_set_handover_attach_flag */

/*===========================================================================
FUNCTION DSPDNCNTX_ACTIVATEIND

DESCRIPTION
  Extracts IFACE configuration information from passed in cm_call_mode_info,
  validates it and configures IFACE with it. Returns success or failure 
  depending on whether passed in configuration info is valid or not. If not,
  cause code is populated in cm_end_params. Note that this is a virtual 
  function and calls the mode-specific function. 

PARAMETERS   
  pdn_cntx_p   - ptr to pdn context
  mode_info_p  - mode info ptr
  end_params   - call end params 
  subs_id      - Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_activate_ind
(
  ds_pdn_context_s                  *pdn_cntx_p,
  const cm_call_mode_info_s_type    *mode_info_p,
  cm_end_params_s_type              *end_params,
  sys_modem_as_id_e_type             subs_id
)
{
  boolean rval = DS3G_FAILURE;

   if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    return FALSE;
  }

  DS_PDN_CTXT_VFR_CALL
  (
    rval, 
    activateInd, 
    subs_id,
    pdn_cntx_p,
    mode_info_p,
    end_params
  );

  return rval;
}

/*===========================================================================
FUNCTION DSPDNCNTX_GETORIGPARAMS

DESCRIPTION
  This function returns the call origination parameters.

PARAMETERS   
  pdn_cntx_p        - ptr to pdn context
  mm_orig_params_p  - mm orig params
  call_info         - call info
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_pdn_cntxt_get_orig_params
(
  ds_pdn_context_s                    *pdn_cntx_p,
  ds_3gpp_orig_params_s               *mm_orig_params_p,                          
  const ds_3gpp_pdn_call_info_type    *call_info
)
{
  boolean                rval = DS3G_FAILURE;
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  /*-----------------------------------------------------------------------*/

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &subs_id)== FALSE)
  {
    return DS3G_FAILURE;
  }

  DS_PDN_CTXT_VFR_CALL
  (
    rval, 
    getOrigParams, 
    subs_id,
    pdn_cntx_p,
    mm_orig_params_p,
    call_info
  );

  return rval;
}

/*===========================================================================
FUNCTION DSPDNCNTX_BEARERACTIVATEIND

DESCRIPTION
  Called to indicate berarer activation request indication.

PARAMETERS   
  call_id           - call id
  mode_info_ptr_i   - mode info ptr
  subs_id           - subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  boolean - TRUE if success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean ds_pdn_cntxt_bearer_activate_ind
(
  cm_call_id_type                   call_id,
  const cm_call_mode_info_s_type   *mode_info_ptr_i,
  cm_act_bearer_rsp_s_type         *cm_act_res_p,
  sys_modem_as_id_e_type            subs_id
)
{
  boolean rval = DS3G_FAILURE;

  DS_PDN_CTXT_VFR_CALL
  (
    rval, 
    bearerActivateInd, 
    subs_id,
    call_id,
    mode_info_ptr_i,
    cm_act_res_p,
    subs_id
  );

  return rval;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_BRIDGED_IFACE

DESCRIPTION
  This function returns the cached Rm Iface pointer.

PARAMETERS   
  ds_iface_ptr - The DS iface whose bridged iface is needed
    
DEPENDENCIES
  None.

RETURN VALUE
  RM Iface pointer if the TE call is up
  NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
ps_iface_type* ds_3gpp_pdn_context_get_bridged_iface
(
  ds_3gpp_iface_s  *ds_iface_ptr
)
{
  ps_iface_type* ret_val = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds_iface_ptr != NULL)
  {
    ret_val = ds_iface_ptr->rm_iface_ptr;
  }

  DS_3GPP_MSG1_MED("Returning bridge iface ptr: 0x%x", ret_val);
  return ret_val;
}


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
FUNCTION DSSNET6I_HANDLE_IP6_EV_CBACK()

DESCRIPTION
  Called when the IP6 comes up/down.  This posts an appropriate event to the
  dssnet6 state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_3gpp_pdn_cntxt_handle_dssnet6_ev_cback
(
  dssnet6_sm_cb_type    *instance,
  dssnet6_sm_event_type  cur_event,
  dssnet6i_state_type    cur_state,
  void                  *user_data
)
{
  ds_3gpp_iface_s            *ds_3gpp_iface_ptr = NULL;
  ds_pdn_context_s           *pdn_context_ptr = NULL;
  ds_dsd_apm_ind_pay_load     ind_payload;  
  sys_modem_as_id_e_type      subs_id=SYS_MODEM_AS_ID_NONE; 
  ds3gsubsmgr_subs_id_e_type  subs_indx;
#ifdef FEATURE_DATA_LTE
  ds_cmd_type                *cmd_ptr = NULL;
  boolean                     throttling_is_allowed = FALSE;
                            /* throttling_is_allowed flag ensures that we 
                               don't increment throttling counter for LTE
                               Attach Failure scenarios if this is a VZW 
                               configuration. */
  ds_pdn_context_s           **data_ptr = NULL;
  boolean                     remove_unused_pdn = FALSE;
#endif /* FEATURE_DATA_LTE */  
  ds_bearer_context_s        *bearer_cntx_p = NULL;
  uint32                      rat_mask = DS_3GPP_THROT_RAT_MASK_GLOBAL;
                             /* RATs on which throttling needs to be unblocked*/
  ds_3gpp_throt_rat_e_type    rat_type = DS_3GPP_THROT_RAT_GLOBAL;
                             /* RAT on which throttling needs to be
                                performed */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

  /*------------------------------------------------------------------------- 
    Validate Input arguments
    -------------------------------------------------------------------------*/
  if ((instance == NULL)||(user_data == NULL))
  {
    DS_3GPP_MSG0_ERROR("NULL instance ptr (or) userdata ptr NULL");
    return;
  }

  DS_3GPP_MSG3_MED("Recd DSSNET6 event %d, state %d, on ds iface 0x%x",
          cur_event, cur_state, user_data);

  ds_3gpp_iface_ptr = (ds_3gpp_iface_s*)(user_data);

  pdn_context_ptr = (ds_pdn_context_s *)(ds_3gpp_iface_ptr->client_data_ptr);

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_ptr,&subs_id))
  {
    return;
  }

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    bearer_cntx_p = pdn_context_ptr->ds_pdn_context_dyn_p->def_bearer_context_ptr;
  
    if (ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
    {
      rat_type = ds_3gpp_throt_map_cm_sys_mode_to_rat_type
                   (bearer_cntx_p->ds_bearer_context_dyn_p->call_mode);

      rat_mask |= ds_3gpp_map_cm_sys_mode_to_rat_mask
                   (bearer_cntx_p->ds_bearer_context_dyn_p->call_mode);
    }
  }

  /*------------------------------------------------------------------------- 
    Get and validate ds3gsubs index.
    The index can be used to access subs_id array in this function to avoid
    klocwork errors.
    -------------------------------------------------------------------------*/
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  if(ds3gsubsmgr_is_ds3g_subs_index_valid(subs_indx) == FALSE)
  {
    DATA_3GMGR_MSG1( MSG_LEGACY_ERROR, "invalid subs index:%d return error",
                     subs_indx);
    return;
  }

  switch (cur_event)
  {
    case DSSNET6_NET_DOWN_EV:
      switch (cur_state)
      {
        case DSSNET6I_WAITING_FOR_NET_UP_STATE:

          /*----------------------------------------------------------------- 
            We allow throttling in the following cases:
            == ===== ========== == === ========= =====
           1. Given PDN does not have is_default flag set.
           2. Given PDN has is_default flag set, but there is 
              another PDN which is already in UP State. 
           3. VZW Configuration is not enabled.
            -----------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
        if (ds_pdn_cntxt_is_default(pdn_context_ptr) == FALSE || 
             ds_3gpp_nv_manager_get_reg_pdn_throttling(subs_id))
        {
          throttling_is_allowed = TRUE;
        }

        if (ds_pdn_cntxt_is_default(pdn_context_ptr) == TRUE &&
            dsPdnContext_is_any_other_normal_pdn_up(pdn_context_ptr, subs_id) == TRUE)
        {
          throttling_is_allowed = TRUE;
        }

        if (throttling_is_allowed&&(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE))
        {
            /*--------------------------------------------------------------- 
           Get the ps_down_reason and use it to reset the cause code.
           Stateless autoconfig failed, call throttling control
              ---------------------------------------------------------------*/
            ds_3gpp_pdn_throt_perform_reg_throttling
              (NULL, pdn_context_ptr,  
              (void *)PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE,
              DS_UMTS_PDP_IPV6, rat_type);
        }
#endif /*FEATURE_DATA_LTE*/
        /*------------------------------------------------------------------- 
          update apn table to allow only v4 call
         ------------------------------------------------------------------*/
        ds_pdn_cntxt_update_allowed_ip_type(pdn_context_ptr, DS_IPV4_ONLY);

          DS_3GPP_APM_SET_INVALID_V6_IPADDR(ind_payload, 
                                PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);

        break;

        case DSSNET6I_NET_UP_STATE:
#ifdef FEATURE_DATA_LTE
          /*----------------------------------------------------------------- 
          Get the ps_down_reason and use it to reset the cause code.
          Stateless autoconfig failed, call throttling control
            -----------------------------------------------------------------*/
         if(ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE)
         {
            ds_3gpp_pdn_throt_perform_reg_throttling
              (NULL, pdn_context_ptr, 
              (void *)PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE,
               DS_UMTS_PDP_IPV6,rat_type);
         }
#endif /*FEATURE_DATA_LTE*/
          /*----------------------------------------------------------------- 
            Update apn table to allow only v4 call
            -----------------------------------------------------------------*/
          ds_pdn_cntxt_update_allowed_ip_type(pdn_context_ptr, DS_IPV4_ONLY);
          DS_3GPP_APM_SET_INVALID_V6_IPADDR(ind_payload, 
                               PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);
          ds_3gpp_validate_and_bring_down_phys_link(instance->ps_iface_ptr,NULL, 
                                  PS_NET_DOWN_REASON_IPV6_ERR_PREFIX_UNAVAILABLE);
        break;

        case DSSNET6I_WAITING_FOR_NET_DOWN_STATE:
          /*----------------------------------------------------------------- 
            This is the case of client initiated call end (or) Call end due
            to LPM mode.
            -----------------------------------------------------------------*/
          if (!ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress() &&
              !ds3geventmgr_is_sub_not_avail_processing_in_progress(subs_id))
          {
            /*--------------------------------------------------------------- 
              This is a case of Client initiated tear down. Continue
              bringing down physlink 
              ---------------------------------------------------------------*/
          ds_3gpp_validate_and_bring_down_phys_link(instance->ps_iface_ptr,NULL,
                                               PS_NET_DOWN_REASON_CLIENT_END);
          }
          else
          {
            /*--------------------------------------------------------------- 
              This is a case of LPM mode/card refrehs clean up. Notify DS MGR
              if the IPV6 SM for all active PDN connections have been gracefully
              brought down DSMGR will notify CM after all the other internal
              DS modules are done with graceful cleanup
              ---------------------------------------------------------------*/
            ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_indx] =
                       (ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_indx] > 0)?
                             (ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_indx] -1):0;
            
            if ((ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_indx] <= 0) &&
                (ds3geventmgr_is_lpm_pwr_off_mode_processing_in_progress()
                 == TRUE))
            {
              ds3geventmgr_clr_and_notify_lpm_pwr_off_status(
                 DS3GEVENTMGR_LPM_PWROFF_3GPP_CTRL_BIT_MSK);
            }
            else if ((ds_3gpp_num_ipv6_ifaces_to_cleanup[subs_indx] <= 0) &&
                   (ds3geventmgr_is_sub_not_avail_processing_in_progress(subs_id)
                     == TRUE)) 
            {
              ds3geventmgr_clr_and_notify_sub_not_avail_status(
                 DS3GEVENTMGR_SUB_NOT_AVAIL_3GPP_CTRL_BIT_MSK,
                 subs_id,
                 ds3gi_sub_not_avail_subs_id_mask);
            }
          }
          break;

        default:
          DS_3GPP_MSG1_HIGH("Ignoring DSSNET6 state %d for NET DOWN EV",
                            cur_state);
          break;
      }/* End switch(cur_state)*/

      break;

    case DSSNET6_NET_UP_EV:
      /*--------------------------------------------------------------------- 
        V6 is up, Clean Throttle State for v6
        ---------------------------------------------------------------------*/
      ds_3gpp_pdn_throt_clear_throttle_state(pdn_context_ptr->ds_pdn_context_dyn_p->
                                             pdp_profile.context.apn, 
                                             DS_UMTS_PDP_IPV6,
                                             subs_id,
                                             rat_mask);
      DS_3GPP_APM_SET_VALID_V6_IPADDR(ind_payload);
      
      #ifdef FEATURE_DATA_LTE      
      remove_unused_pdn = ds_3gpp_nv_manager_get_remove_unused_pdn(subs_id);
      if (remove_unused_pdn)
      {
        cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_pdn_context_s*));
        if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
        {
          ASSERT(0);
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_PDN_REMOVE_UNUSED;
        data_ptr = (ds_pdn_context_s**)cmd_ptr->cmd_payload_ptr;
        *data_ptr = pdn_context_ptr;

        ds_put_cmd_ext(cmd_ptr);
      }
      #endif /* FEATURE_DATA_LTE */
      break;

    default:
      DS_3GPP_MSG2_ERROR("DS iface 0x%x got unsupported DSSNET6 event %d, ignored",
                         ds_3gpp_iface_ptr,
                         cur_event);
  }

  if (( TRUE == (ds_pdn_cntxt_is_default(pdn_context_ptr)) &&
       ind_payload.ind_field_mask != 0) )
  {
    DS_3GPP_APM_SET_PROFILE_ID(ind_payload, (uint16)pdn_context_ptr->ds_pdn_context_dyn_p->
                               pdp_profile_num);
    ind_payload.subs_id = subs_id;
    (void)ds_dsd_apm_ind(DS_DSD_APM_IP_ADDRESS_IND, &ind_payload);
  }
  
} /* dssnet6i_handle_ip6_ev_cback() */

/*===========================================================================
FUNCTION DS_PDN_CNTXT_GET_IFACE_STAT_ADJUSTMENT

DESCRIPTION
  Called to get stats adjustment for iface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_get_iface_stat_adjustment
(
  ps_iface_type *iface_ptr,
  ps_iface_stat_adjustment_type* adjustment
)
{

  memset(adjustment, 0, sizeof(ps_iface_stat_adjustment_type));
  /* TX result is accurate, fill in 0 */
  adjustment->bytes_tx = 0;
  adjustment->pkts_tx  = 0;

  DS_3GPP_MSG3_HIGH("adjust result,rx pkt: %d, rx byte: %d",
                    adjustment->pkts_rx,
                    adjustment->bytes_rx,
                    0);
}

/*===========================================================================
FUNCTION PS_STAT_UPDATE_IFACE()

DESCRIPTION
  This function update the iface statistics with adjustment from client

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  instance_ptr    - IN - Handle to instance
  adjustment      - IN - pointer to stat adjustment

SIDE EFFECTS
  None
===========================================================================*/
void ds_pdn_cntxt_update_iface_stat
(
  ps_iface_stat_adjustment_type  *adjustment,
  ps_iface_type                  *iface_ptr
)
{
  if ((adjustment == NULL) || iface_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid input for ps_stat_update_iface ");
    return;
  }
  IFACE_INC_INSTANCE_STATS(iface_ptr, pkts_tx, (adjustment->pkts_tx));
  IFACE_INC_INSTANCE_STATS(iface_ptr, pkts_rx, (adjustment->pkts_rx));
  IFACE_INC_INSTANCE_STATS(iface_ptr, bytes_tx, (adjustment->bytes_tx));
  IFACE_INC_INSTANCE_STATS(iface_ptr, bytes_rx, (adjustment->bytes_rx));
}


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_CURRENT_PLMN_EX

DESCRIPTION
  This function sets the current PLMN that we are operating on.
  This function can be called from any mode specific handler. It fills up
  the container we maintain in PDN context to hold the current PLMN

  This function does not check if the incoming PLMN ID is 0. It is the caller's
  responsibility to check this.

PARAMETERS
  plmn_id_ptr - pointer to PLMN id that has to be copied to the DS container
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_current_plmn_ex
(
  sys_plmn_id_s_type      *plmn_id_ptr,
  sys_modem_as_id_e_type   subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (plmn_id_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("plmn_id_ptr is NULL");
    return;
  }

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);

  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Per Subs Hndl is Invalid for Subs id(CM): %d", subs_id);
    return;
  }

  ds3g_print_plmn_id(plmn_id_ptr);
  memscpy((void*)&(hndl->current_plmn),
           sizeof(sys_plmn_id_s_type),
           (void*)plmn_id_ptr, 
           sizeof(sys_plmn_id_s_type));

  return;
} /* ds_3gpp_pdn_cntx_set_current_plmn_ex() */


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_CURRENT_PLMN_EX

DESCRIPTION
  This function fetches the current PLMN Id corresponding to the given Subs Id

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_current_plmn_ex
(
  sys_plmn_id_s_type*     plmn_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(plmn_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid PLMN pointer passed");
    return;
  }

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);
  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Per Subs Hndl is Invalid for Subs id(CM): %d", subs_id);
    return;
  }

  if( ds3gpp_get_current_network_mode(subs_id) != SYS_SYS_MODE_LTE )
  {
    ds3g_get_current_plmn_id(plmn_ptr, subs_id);
  }
  else
  {
    memscpy((void*)plmn_ptr,
            sizeof(sys_plmn_id_s_type),
            &(hndl->current_plmn),
            sizeof(sys_plmn_id_s_type));
  }
  return;
} /* ds_3gpp_pdn_cntx_get_current_plmn_ex() */
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_RPM_PLMN_BLOCKED

DESCRIPTION
  This function sets the RPM PLMN blocked flag.
 
PARAMETERS
  blocked - RPM PLMN blocked or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_rpm_plmn_blocked
(
  boolean                  blocked
)
{
  ds_3gpp_pdn_context_info.rpm_plmn_blocked = blocked;
} /* ds_3gpp_pdn_cntx_set_rpm_plmn_blocked() */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_RPM_PLMN_BLOCKED

DESCRIPTION
  This function gets the RPM PLMN blocked.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  RPM PLMN blocked
  FALSE: RPM PLMN not blocked

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntx_get_rpm_plmn_blocked
(
  void
)
{
  return ds_3gpp_pdn_context_info.rpm_plmn_blocked;
} /* ds_3gpp_pdn_cntx_get_rpm_plmn_blocked() */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BLOCKED_PLMN_LIST

DESCRIPTION
  This function gets the Blocked PLMN List.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  Blocked PLMN List

SIDE EFFECTS
  None
===========================================================================*/
list_type* ds_3gpp_pdn_cntx_get_blocked_plmn_list
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);
  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Per Subs Hndl is Invalid for Subs id(CM): %d", subs_id);
    return NULL;
  }

  return &(hndl->blocked_plmn_list);
} /* ds_3gpp_pdn_cntx_get_blocked_plmn_list */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_SET_LOCAL_DISCON_FLAG

DESCRIPTION
  This function is used to set the local disconnect flag for a given PDN
  context

PARAMETERS
  pdn_cntx_ptr - PDN context where this has to be set

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_set_local_discon_sent_flag
(
  ds_pdn_context_s* pdn_cntx_ptr
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_ptr))
  {
    pdn_cntx_ptr->ds_pdn_context_dyn_p->local_disconnect_sent = TRUE;
  }
  else
  {
    DS_3GPP_MSG0_ERROR("PDN context pointer NULL. Not setting flag");
  }
} /* ds_3gpp_pdn_cntx_set_local_discon_sent_flag */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BEARER_FOR_PDN_IN_GOING_DOWN

DESCRIPTION
  This function is used to determine if there are any PDN contexts that are
  in going Down state for which local disconnect has not been sent yet. 
  If yes, this function returns the pointer to the default bearer context
  of that PDN

  Note: This is called only during local deactivation of contexts after IRAT
  This should not be called under any other circumstances

PARAMETERS
  None.

DEPENDENCIES
  None

RETURN VALUE
  Void* - pointer to the def bearer of the PDN of interest

SIDE EFFECTS
  None
===========================================================================*/
void* ds_3gpp_pdn_cntx_get_bearer_for_pdn_in_going_down
(
  sys_modem_as_id_e_type subs_id
)
{
  int                    pdn_table_index;
  ds_pdn_context_s*      pdn_cntx_p;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_table_index = 0; 
      pdn_table_index < DS_3GPP_MAX_PDN_CONTEXT; 
      pdn_table_index++) 
  {
    pdn_cntx_p = &(ds_pdn_context_tbl[pdn_table_index]);

    if(((ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_cntx_p, subs_id)) &&
       (pdn_cntx_p->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_GOING_DOWN) &&
       (pdn_cntx_p->ds_pdn_context_dyn_p->local_disconnect_sent == FALSE)))
    {
      /*------------------------------------------------------------------------
        We have found a PDN context that is in Going down state and a local
        disconnect has not been sent on this

        Return the def bearer context of this PDN
      ------------------------------------------------------------------------*/
      return pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
    }
  }/* loop over all the PDN context*/

  return NULL;

} /* ds_3gpp_pdn_cntx_get_bearer_for_pdn_in_going_down */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_PENDING_REDIAL

DESCRIPTION
  This function returns whether there is a pdn pending redial

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context pending redial

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_pending_redial
( 
  sys_modem_as_id_e_type    subs_id  
)
{
  uint8 pdn_cntxt_inst;

  DS_3GPP_MSG0_HIGH("Checking for pending redial PDN.");
  for(pdn_cntxt_inst=0;pdn_cntxt_inst<DS_3GPP_MAX_PDN_CONTEXT;pdn_cntxt_inst++)
  {
    if ((ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl
                                               [pdn_cntxt_inst]))  &&
        (ds_pdn_context_tbl[pdn_cntxt_inst].ds_pdn_context_dyn_p->state == 
                                         DS_PDN_CONTEXT_STATE_PEND_REDIAL) &&
        (ds_pdn_context_tbl[pdn_cntxt_inst].ds_pdn_context_dyn_p->subs_id == subs_id))
    {
      DS_3GPP_MSG1_HIGH("PDN instance %d is pending redial.",
                        pdn_cntxt_inst);
      return TRUE;
    }
  }
  return FALSE;
}/* ds_3gpp_pdn_cntxt_is_any_pdn_pending_redial */


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_AWAITING_BEARER_ALLOCATION

DESCRIPTION
  This function returns the status, if  there is a pdn waiting for bearer
  resource allocation

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If there is atleast one PDN in AWAITING_BEARER_ALLOC state
  FALSE - otherwise
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_awaiting_bearer_allocation
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8                      index;
  ds_pdn_context_s          *pdn_cntxt_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index=0;index<DS_3GPP_MAX_PDN_CONTEXT;index++)
  {
    pdn_cntxt_p = &ds_pdn_context_tbl[index];
    /*----------------------------------------------------------------------- 
      Check if the PDN context is valid and if the PDN context state is in
      DS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC
      -----------------------------------------------------------------------*/
    if (ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p)  &&
        DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC(pdn_cntxt_p, subs_id))
    {
      DS_3GPP_MSG1_HIGH("PDN context 0x%x in AWAITING_BEARER_ALLOC state.",
                        pdn_cntxt_p);
      return TRUE;
    }
  }
  return FALSE;
}/* ds_3gpp_pdn_cntxt_is_any_pdn_awaiting_bearer_allocation */

/*===========================================================================
FUNCTION DS_3GPP_GET_PDN_CNTXT_AWAITING_BEARER_ALLOCATION

DESCRIPTION
  This function returns the PDN context waiting for bearer resource
  allocation

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  ds_pdn_context_s - If there is a PDN waiting for bearer allocation
  NULL             - otherwise

SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_context_s* ds_3gpp_get_pdn_cntxt_awaiting_bearer_allocation
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8               index       = 0;
  ds_pdn_context_s   *pdn_cntxt_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(index=0;index<DS_3GPP_MAX_PDN_CONTEXT;index++)
  {
    if ((ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[index])) &&
        DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC((&ds_pdn_context_tbl[index]), subs_id)) 
    {
      pdn_cntxt_p = &ds_pdn_context_tbl[index];
      DS_3GPP_MSG1_HIGH("PDN cntxt 0x%x is awaiting bearer resource allocation",
                        pdn_cntxt_p);
      break;
    }
  }
  return pdn_cntxt_p;
}/*ds_3gpp_get_pdn_cntxt_awaiting_bearer_allocation*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_BEARER_TECH_TYPE

DESCRIPTION 
  This function translates CM mode to one of the IFACE tech
  type enum values.
  
PARAMETERS
  mode        - mode defined in CM
  bearer_tech - structure to be populated
  subs_id     - Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_bearer_tech_type
(
  sys_sys_mode_e_type               mode,
  ps_iface_bearer_technology_type  *bearer_tech,
  sys_modem_as_id_e_type            subs_id
)
{
  cm_cell_srv_ind_s_type  prev_cell_srv_ind;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Based on mode passed in, fill up ps_iface structure with corresponding
    rat_mask values.
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG1_HIGH("Mode passed in is %d", mode);

  prev_cell_srv_ind = ds_3gpp_bearer_cntxt_get_cell_srv_ind(subs_id);
  
  switch (mode)
  {
 #ifdef FEATURE_GSM_GPRS
    case SYS_SYS_MODE_GSM: 
    {
      bearer_tech->current_network = PS_IFACE_NETWORK_3GPP;
      bearer_tech->data.umts_type.rat_mask = 
        ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support
          (prev_cell_srv_ind.egprs_supp, subs_id);
      break;
    }
#endif /* FEATURE_GSM_GPRS */
#ifdef FEATURE_TDSCDMA
    case SYS_SYS_MODE_TDS:
    {
      bearer_tech->current_network = PS_IFACE_NETWORK_3GPP;
#ifndef FEATURE_HSPA_CALL_STATUS_IND
      bearer_tech->data.umts_type.rat_mask = PS_IFACE_3GPP_TDSCDMA;
#else
      bearer_tech->data.umts_type.rat_mask = 
           ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
           (
             ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind
               (&prev_cell_srv_ind), 
             subs_id
           );
#endif /* FEATURE_HSPA_CALL_STATUS_IND */
     break;
    }
#endif
#ifdef FEATURE_WCDMA
    case SYS_SYS_MODE_WCDMA: 
    {
      bearer_tech->current_network = PS_IFACE_NETWORK_3GPP;
#ifndef FEATURE_HSPA_CALL_STATUS_IND
      bearer_tech->data.umts_type.rat_mask = PS_IFACE_3GPP_WCDMA;
#else
      bearer_tech->data.umts_type.rat_mask = 
           ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
           (
             ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind
               (&prev_cell_srv_ind),
             subs_id
            );
#endif /* FEATURE_HSPA_CALL_STATUS_IND */
     break;
    }
#endif  /* FEATURE_WCDMA */
#ifdef FEATURE_LTE
    case SYS_SYS_MODE_LTE:
    {
      bearer_tech->current_network = PS_IFACE_NETWORK_3GPP;
      bearer_tech->data.umts_type.rat_mask = PS_IFACE_3GPP_LTE;
      break;
    }
#endif /* FEATURE_LTE */
    default:
    {
      DS_3GPP_MSG0_HIGH("Mode passed in is INVALID");
      bearer_tech->current_network = PS_IFACE_NETWORK_MAX;
      bearer_tech->data.umts_type.rat_mask = 0x00;
      bearer_tech->data.cdma_type.rat_mask = 0x00;
      break;
    }
  }
}/* ds_3gpp_pdn_cntx_get_bearer_tech_type */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_POST_IFACE_GENERIC_IND_FOR_PDN

DESCRIPTION
  This function posts IFACE generic indication to all valid
  Iface pointers for a single PDN context.

PARAMETERS
  event -- IFACE Event that needs to be posted.
  event_data_ptr -- data associated with this event.
  pdn_cntxt_p  -- PDN context

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_post_iface_generic_ind_for_pdn
(
  ds_pdn_context_s                   * pdn_cntxt_p,
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr
)
{
  ps_iface_type* v4_iface_ptr = NULL;
  ps_iface_type* v6_iface_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {
    return;
  }
  v4_iface_ptr = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntxt_p);
  v6_iface_ptr = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntxt_p);
  if ( PS_IFACE_IS_VALID(v4_iface_ptr) &&
  	  (v4_iface_ptr->iface_private.state == IFACE_UP ||
	   v4_iface_ptr->iface_private.state == IFACE_ROUTEABLE ||
	   v4_iface_ptr->iface_private.state == IFACE_CONFIGURING )) 
  {
    ps_iface_generic_ind (v4_iface_ptr,
                          event,
                          event_data_ptr);
  }
  if ( PS_IFACE_IS_VALID(v6_iface_ptr) &&
  	 (v6_iface_ptr->iface_private.state == IFACE_UP ||
	  v6_iface_ptr->iface_private.state == IFACE_ROUTEABLE ||
	  v6_iface_ptr->iface_private.state == IFACE_CONFIGURING ))
  {
    ps_iface_generic_ind (v6_iface_ptr,
                          event,
                          event_data_ptr);
  }
}/* ds_3gpp_pdn_cntx_post_iface_generic_ind_for_pdn */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_POST_IFACE_GENERIC_IND

DESCRIPTION
  This function posts IFACE generic indication to all valid
  Iface pointers.

PARAMETERS
  event -- IFACE Event that needs to be posted.
  event_data_ptr -- data associated with this event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_post_iface_generic_ind
(
  ps_iface_event_enum_type             event,
  void                                *event_data_ptr,
  sys_modem_as_id_e_type               subs_id
)
{
  uint8              pdn_inst = 0;
  ds_pdn_context_s  *pdn_cntxt_p = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(pdn_inst = 0; pdn_inst < DS_3GPP_MAX_PDN_CONTEXT; pdn_inst++)
  {
    if(ds_pdn_cntxt_validate_pdn_subscription_internal
         (&ds_pdn_context_tbl[pdn_inst], subs_id)) 
    {
      pdn_cntxt_p = &ds_pdn_context_tbl[pdn_inst];
      DS_3GPP_MSG2_HIGH("Posting event %d, for PDN instance %d",
                        (int)event,pdn_inst);
      ds_3gpp_pdn_cntx_post_iface_generic_ind_for_pdn(
                                       pdn_cntxt_p,
                                       event,
                                       event_data_ptr );
    }
  }
}/* ds_3gpp_pdn_cntx_post_iface_generic_ind*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SYS_CHG_SS_INFO_CMD_HDLR

DESCRIPTION
  This function handles the DSUMTSPS_SYS_CHG_SS_INFO_CMD command. This 
  command is posted when CM reports a sys change with change in cell serv
  ind.

PARAMETERS
  sys_chg_ss_info_ptr - ptr to structure containng the cell srv ind.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void ds_3gpp_pdn_cntxt_sys_chg_ss_info_cmd_hdlr
(
  const ds3g_sys_chg_ss_info_type  *sys_chg_ss_info_ptr
)
{
  ps_iface_ioctl_bearer_tech_changed_type  bearer_tech_chg;
  sys_modem_as_id_e_type                   subs_id;
  sys_sys_mode_e_type                      sys_mode;

#ifdef FEATURE_DATA_LTE
  sys_div_duplex_e_type                    new_lte_div_duplex;
  sys_div_duplex_e_type                    prev_lte_div_duplex;
#endif /* FEATURE_DATA_LTE */

#if ((defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA)) && \
    defined(FEATURE_HSPA_CALL_STATUS_IND)) || defined(FEATURE_GSM_GPRS)
  cm_cell_srv_ind_s_type                   new_cell_srv_ind;
  cm_cell_srv_ind_s_type                   prev_cell_srv_ind;
#endif /* ((defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_TDSCDMA)) &&
           defined(FEATURE_HSPA_CALL_STATUS_IND)) || defined(FEATURE_GSM_GPRS)*/
  boolean                                  is_ps_srv_domain_pref = FALSE;
  sys_srv_domain_e_type                    srv_domain = SYS_SRV_DOMAIN_NO_SRV;
#ifdef FEATURE_DATA_WLAN_MAPCON
  ds_sys_system_status_info_type           bearer_tech_ex;
  sint15                                   ps_errno;
#endif /* FEATURE_DATA_WLAN_MAPCON */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  subs_id = sys_chg_ss_info_ptr->subs_id;
  sys_mode = sys_chg_ss_info_ptr->sys_mode;

  srv_domain = ds3g_get_current_regd_srv_domain_ex(subs_id);

  if(srv_domain == SYS_SRV_DOMAIN_PS_ONLY ||
     srv_domain == SYS_SRV_DOMAIN_CS_PS)
  {
    is_ps_srv_domain_pref = TRUE;
  }

  switch(sys_mode)
  {
#ifdef FEATURE_DATA_LTE
    case SYS_SYS_MODE_LTE:
    {
      prev_lte_div_duplex = ds_3gpp_bearer_cntxt_get_prev_div_duplex(subs_id);
      new_lte_div_duplex = sys_chg_ss_info_ptr->div_duplex;

      if (new_lte_div_duplex != prev_lte_div_duplex)
      {
        ds_3gpp_bearer_cntxt_set_prev_div_duplex(new_lte_div_duplex, subs_id);

        DS_3GPP_MSG0_HIGH("Posting event from SS info for LTE");

#ifdef FEATURE_DATA_WLAN_MAPCON
        if (ds_3gpp_bearer_cntxt_get_bearer_type_ex( sys_mode,
                                                     &bearer_tech_ex,
                                                     &ps_errno ,
                                                     subs_id) == 0)
        {
          ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                  IFACE_BEARER_TECH_EX_CHANGED_EV,
                                  (void *)&bearer_tech_ex,
                                  subs_id);
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Could not fetch bearer tech ex info");
        }
#endif /* FEATURE_DATA_WLAN_MAPCON */
      }
      break;
    }
#endif

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)) && \
     defined(FEATURE_HSPA_CALL_STATUS_IND)
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
    {
      prev_cell_srv_ind = ds_3gpp_bearer_cntxt_get_cell_srv_ind(subs_id);
      new_cell_srv_ind = sys_chg_ss_info_ptr->cell_srv_ind;

      if ((new_cell_srv_ind.hs_call_status != prev_cell_srv_ind.hs_call_status) ||
          (new_cell_srv_ind.hs_ind != prev_cell_srv_ind.hs_ind)) 
      {
        bearer_tech_chg.old_bearer_tech.current_network = PS_IFACE_NETWORK_UMTS;
        bearer_tech_chg.old_bearer_tech.data.umts_type.rat_mask = 
          ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
          (
            ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind
              (&prev_cell_srv_ind),
            subs_id
          );

        ds_3gpp_bearer_cntxt_set_cell_srv_ind(new_cell_srv_ind, subs_id);

        if(is_ps_srv_domain_pref == TRUE)
        {
          bearer_tech_chg.new_bearer_tech.current_network = PS_IFACE_NETWORK_UMTS;
          bearer_tech_chg.new_bearer_tech.data.umts_type.rat_mask =
          ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_hs_status
          (
            ds_wcdma_bearer_cntxt_get_hs_status_from_cell_srv_ind
              (&new_cell_srv_ind),
            subs_id
          );
        } 
        else
        {
          bearer_tech_chg.new_bearer_tech.current_network = PS_IFACE_NETWORK_UMTS;
          bearer_tech_chg.new_bearer_tech.data.umts_type.rat_mask =
                                                      PS_IFACE_3GPP_NULL_BEARER;
        }

        /*-------------------------------------------------------------------------
          For all valid calls in WCDMA mode, post tech changed event.
        -------------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("Posting event from SS info for WCDMA");
        ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                  IFACE_BEARER_TECH_CHANGED_EV,
                                  (void *)&bearer_tech_chg,
                                  subs_id);

#ifdef FEATURE_DATA_WLAN_MAPCON
        if (ds_3gpp_bearer_cntxt_get_bearer_type_ex( sys_mode,
                                                     &bearer_tech_ex,
                                                     &ps_errno,
                                                     subs_id) == 0)
        {
      
          ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                  IFACE_BEARER_TECH_EX_CHANGED_EV,
                                  (void *)&bearer_tech_ex,
                                  subs_id);
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Could not fetch bearer tech ex info");
        }
#endif /* FEATURE_DATA_WLAN_MAPCON */

      }
      break;
    }
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA)) && 
              defined(FEATURE_HSPA_CALL_STATUS_IND) */

#if defined(FEATURE_GSM_GPRS)
    case SYS_SYS_MODE_GSM:
    {
      prev_cell_srv_ind = ds_3gpp_bearer_cntxt_get_cell_srv_ind(subs_id);
      new_cell_srv_ind = sys_chg_ss_info_ptr->cell_srv_ind;

      if (new_cell_srv_ind.egprs_supp != prev_cell_srv_ind.egprs_supp) 
      {
        bearer_tech_chg.old_bearer_tech.current_network =
          PS_IFACE_NETWORK_UMTS;
        bearer_tech_chg.old_bearer_tech.data.umts_type.rat_mask = 
         ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support(
            prev_cell_srv_ind.egprs_supp, subs_id);

        ds_3gpp_bearer_cntxt_set_cell_srv_ind(new_cell_srv_ind, subs_id);

        if(is_ps_srv_domain_pref == TRUE) 
        {
          bearer_tech_chg.new_bearer_tech.current_network = PS_IFACE_NETWORK_UMTS;
          bearer_tech_chg.new_bearer_tech.data.umts_type.rat_mask =
          ds_3gpp_bearer_cntxt_get_ps_rat_mask_for_egprs_support(
            new_cell_srv_ind.egprs_supp, subs_id);
        }
        else
        {
          bearer_tech_chg.new_bearer_tech.current_network = PS_IFACE_NETWORK_UMTS;
          bearer_tech_chg.new_bearer_tech.data.umts_type.rat_mask =
                                                      PS_IFACE_3GPP_NULL_BEARER;
        }

        /*-------------------------------------------------------------------
          For all valid calls in GSM mode, post tech changed event.
        -------------------------------------------------------------------*/
        DS_3GPP_MSG0_HIGH("Posting event from SS info for GSM");
        ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                  IFACE_BEARER_TECH_CHANGED_EV,
                                  (void *)&bearer_tech_chg,
                                  subs_id);

  #ifdef FEATURE_DATA_WLAN_MAPCON
        if (ds_3gpp_bearer_cntxt_get_bearer_type_ex( sys_mode,
                                                     &bearer_tech_ex,
                                                     &ps_errno,
                                                     subs_id ) == 0)
        {
          ds_3gpp_pdn_cntx_post_iface_generic_ind(
                                IFACE_BEARER_TECH_EX_CHANGED_EV,
                                (void *)&bearer_tech_ex,
                                subs_id);
        }
        else
        { 
          DS_3GPP_MSG0_ERROR("Could not fetch bearer tech ex info");
        }
  #endif /* FEATURE_DATA_WLAN_MAPCON */

      }
      break;
    }
#endif /* defined(FEATURE_GSM_GPRS) */

    default:
      DS_3GPP_MSG1_ERROR("Invalid sys mode: %d. Not posting bearer tech",
                         sys_mode);
      break;
  }

  return;
} /* ds_3gpp_pdn_cntxt_sys_chg_ss_info_cmd_hdlr */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_RESUME_IND_HDLR

DESCRIPTION
  This function is used to perform any action at the PDN context level when a
  resume indication is received.

  This function in turn invokes the mode specific resume indication handler.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_resume_ind_hdlr
(
  sys_modem_as_id_e_type subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Call the mode specific resume indication handler function .
    Note that only LTE Mode registers this function as of now.
  ------------------------------------------------------------------------*/
  DS_PDN_CTXT_VF_CALL (resume_ind, subs_id, subs_id);
}

/*===========================================================================
FUNCTION DS3GPP_GET_SESSION_PARAMS

DESCRIPTION
  Function for to retrieve the profile params related to a call from the
  iface table and call info table.

PARAMETERS
  iface_ptr;   - Ptr to ps iface inst    
  session_ptr  - Pointer to stiore the session params
  ps_errno     - Error information

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
       DS_EFAULT   if NULL pointers passed.
   
SIDE EFFECTS
  None
===========================================================================*/
int ds_3gpp_pdn_cntxt_get_session_params
(
  ps_iface_type                                 *iface_ptr,
                                                  /* Ptr to ps iface inst    */
  ps_iface_ioctl_3gpp_session_params_info_type  *session_ptr,
                                            /* Pointer to store session data */
  sint15                                        *ps_errno
)
{       
  uint32                                 call_instance;
  ds_pdn_context_s                       *pdn_context_ptr;
  ds_3gpp_iface_s                        *ds3gpp_iface_ptr;
  ds_bearer_context_s                    *bearer_context_ptr;
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  ds_umts_umts_qos_params_type           *umts_netw_alloc_qos;
  ds_umts_gprs_qos_params_type           *gprs_netw_alloc_qos;
#endif
#ifdef FEATURE_DATA_LTE
  ds_3gpp_lte_qos_params_type            lte_netw_alloc_qos;
#endif /* FEATURE_DATA_LTE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("start ds_3gpp_pdn_cntxt_get_session_params");

  if (session_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("session_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }

  memset(session_ptr, 0 , sizeof(ps_iface_ioctl_3gpp_session_params_info_type));
  if (iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("iface_ptr is NULL");
    *ps_errno = DS_EFAULT;
    return -1;
  }
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
  ds3gpp_enter_global_crit_section();
  ds3gpp_iface_ptr   = (ds_3gpp_iface_s*)(iface_ptr->client_data_ptr);
  if (ds3gpp_iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("ds3gpp_iface_ptr is NULL");
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }
  
  pdn_context_ptr    = (ds_pdn_context_s*)(ds3gpp_iface_ptr->client_data_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr)) 
  {
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }

  if (pdn_context_ptr->ds_pdn_context_dyn_p->state != 
      DS_PDN_CONTEXT_STATE_UP)
  {
    DS_3GPP_MSG1_ERROR("pdn_context state is %d returning error", 
                        pdn_context_ptr->ds_pdn_context_dyn_p->state);
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }    

  bearer_context_ptr = 
         (ds_bearer_context_s*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                                def_bearer_context_ptr);

  if(!ds_bearer_cntx_validate_bearer_context(bearer_context_ptr)) 
  {
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);
    return -1;
  }
  
  call_instance = (uint32)(bearer_context_ptr->ds_bearer_context_dyn_p->
                            client_data_ptr);
  /*-----------------------------------------------------------------------
    Copy the parameters related to profile name and authentication info
    from iface table.
  -----------------------------------------------------------------------*/

  /*----------------------------------------------------------------------- 
    Enter 3gpp_crit_section as PDN context, apn table and other 3gpp
    data structures are accessed in DCC task context
  -----------------------------------------------------------------------*/

  
  session_ptr->profile_number = pdn_context_ptr->ds_pdn_context_dyn_p
    ->pdp_profile_num;

  (void)strlcpy((char*)session_ptr->profile_name,
                    (char*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                            pdp_profile.profile_name),
                    sizeof(session_ptr->profile_name));

  (void)strlcpy((char*)session_ptr->auth.username,
                    (char*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                            pdp_profile.auth.username),
                    sizeof(session_ptr->auth.username));

 if ( (pdn_context_ptr->ds_pdn_context_dyn_p->info.valid_fields & NET_CFG_PRM_AUTH_PREF_MASK) == 0)
  {
      session_ptr->auth.auth_type = 
     (ds_umts_pdp_auth_enum_type)(pdn_context_ptr->ds_pdn_context_dyn_p->
                                  pdp_profile.auth.auth_type);
  }
 else
  {
    session_ptr->auth.auth_type = 
      (ds_umts_pdp_auth_enum_type)(pdn_context_ptr->
                          ds_pdn_context_dyn_p->info.auth_info.auth_type);
  }


  (void)strlcpy((char*)session_ptr->apn, 
                    (char*)(pdn_context_ptr->ds_pdn_context_dyn_p->
                            pdp_profile.context.apn),
                    sizeof(session_ptr->apn));

  session_ptr->pdp_type = 
          ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_ptr);

  DS_3GPP_MSG3_HIGH("profile_num = %d, Profile_name = %s, apn = %s",
           session_ptr->profile_number,
           session_ptr->profile_name,
           session_ptr->apn);
  DS_3GPP_MSG3_HIGH(" pdp_type= %d, username = %s, auth_type = %d",
           session_ptr->pdp_type,
           session_ptr->auth.username,
           session_ptr->auth.auth_type);

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
  umts_netw_alloc_qos = 
       ds_umts_bearer_flow_manager_get_umts_netw_alloc_qos(call_instance);
  gprs_netw_alloc_qos = 
       ds_umts_bearer_flow_manager_get_gprs_netw_alloc_qos(call_instance);
  /*-----------------------------------------------------------------------
    Copy the network granted UMTS and GPRS parameters in the 
    session_ptr structure.
  -----------------------------------------------------------------------*/

  if (umts_netw_alloc_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("dsUmtsPdnCntx_resumeIndhdlr: umts_netw_alloc_qos is NULL");
  }
  else if(umts_netw_alloc_qos->valid_flg == TRUE)
  {
    memscpy((void*)(&session_ptr->umts_qos), 
	     sizeof(ds_umts_umts_qos_params_type),
            (void*)umts_netw_alloc_qos,
            sizeof(ds_umts_umts_qos_params_type));
  }

  if (gprs_netw_alloc_qos == NULL)
  {
    DS_3GPP_MSG0_ERROR("dsUmtsPdnCntx_resumeIndhdlr: gprs_netw_alloc_qos is NULL");
  }
  else if(gprs_netw_alloc_qos->valid_flg == TRUE)
  {
    memscpy((void*)(&session_ptr->gprs_qos),
            sizeof(ds_umts_gprs_qos_params_type),
            (void*)gprs_netw_alloc_qos,
            sizeof(ds_umts_gprs_qos_params_type));
  }
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) */

  session_ptr->request_pcscf_address_flag = 
      pdn_context_ptr->ds_pdn_context_dyn_p->pdp_profile.request_pcscf_address_flag;

#ifdef FEATURE_DATA_LTE
  ds_eps_bearer_flow_manager_get_lte_netw_alloc_qos(bearer_context_ptr,
                                                    &lte_netw_alloc_qos);
  /*-----------------------------------------------------------------------
    Copy the network granted LTE parameters in the session_ptr structure.
  -----------------------------------------------------------------------*/
  if(lte_netw_alloc_qos.valid_flg == TRUE)
  {
    memscpy((void*)(&session_ptr->lte_qos),
            sizeof(ds_3gpp_lte_qos_params_type),
            (void*)(&lte_netw_alloc_qos),
            sizeof(ds_3gpp_lte_qos_params_type));
  }
#endif /* FEATURE_DATA_LTE */
  ds3gpp_leave_global_crit_section();
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_flow_crit_sect);

  return 0;
} /* ds_3gpp_pdn_cntxt_get_session_params */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_PDP_PROFILE_NUM

DESCRIPTION
  Function to retrieve the profile params related to a call from the
  iface table and call info table.

PARAMETERS
  iface_ptr;   - Ptr to ps iface inst    
  argval_ptr   - Pointer to store pdp profile number
  ps_errno     - Error information

DEPENDENCIES
  None.

RETURN VALUE
  0  - On Success
  -1 - On Failure
       DS_EFAULT   if NULL pointers passed.
   
SIDE EFFECTS
  None
===========================================================================*/

int ds_3gpp_pdn_cntxt_get_pdp_profile_num
(
  ps_iface_type     *iface_ptr,
  void              *argval_ptr,
  int16             *ps_errno
)
{
  ds_3gpp_iface_s                        *ds3gpp_iface_ptr;
  ds_pdn_context_s                       *pdn_context_ptr;
  ps_iface_ioctl_pdp_context_id_type     *pdp_profile_num;

  if(ps_errno == NULL)
  {
    DS_3GPP_MSG0_ERROR("ps_errno argument is NULL");
    return -1;
  }

  if (argval_ptr == NULL) 
  {
    DS_3GPP_MSG1_ERROR("Invalid arguments argval_ptr= 0x%x",
                       argval_ptr);
    *ps_errno = DS_EFAULT;
    return -1;
  }

  ds3gpp_enter_global_crit_section();

  if(FALSE == PS_IFACE_IS_VALID(iface_ptr))
  {
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  if (ps_iface_state(iface_ptr) == IFACE_DOWN || 
      ps_iface_state(iface_ptr) == IFACE_DISABLED )
  {
    DS_3GPP_MSG0_ERROR("iface in down or disabled state");
    *ps_errno = DS_EINVAL;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  ds3gpp_iface_ptr   = (ds_3gpp_iface_s*)(iface_ptr->client_data_ptr);
  if (ds3gpp_iface_ptr == NULL) 
  {
    DS_3GPP_MSG0_ERROR("ds3gpp_iface_ptr is NULL");
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }

  pdn_context_ptr    = (ds_pdn_context_s*)(ds3gpp_iface_ptr->client_data_ptr);
  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_ptr))
  {
    *ps_errno = DS_EFAULT;
    ds3gpp_leave_global_crit_section();
    return -1;
  }
  
  pdp_profile_num = (ps_iface_ioctl_pdp_context_id_type *)argval_ptr;
  *pdp_profile_num = pdn_context_ptr->ds_pdn_context_dyn_p->pdp_profile_num;
  ds3gpp_leave_global_crit_section();
  return 0;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_GET_DEF_PDN

DESCRIPTION
  This function is used to get to the default PDN context.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_pdn_context_s* - Pointer to the default PDN context irrespective of the
                      state of the PDN. 
   
SIDE EFFECTS
  None
===========================================================================*/
ds_pdn_context_s* ds_3gpp_pdn_cntxt_get_def_pdn
(
  sys_modem_as_id_e_type subs_id
)
{
  int                     index;
  ds_pdn_context_s       *def_pdn_p=NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(index = 0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
    if(ds_pdn_cntxt_validate_pdn_subscription_internal(
                                     &ds_pdn_context_tbl[index], subs_id) &&
       (ds_pdn_context_tbl[index].ds_pdn_context_dyn_p->is_default == TRUE))
    {
      def_pdn_p = &(ds_pdn_context_tbl[index]);
      break;
    }
  }

  return def_pdn_p;
}



/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_DECODE_APN_NAME_NON_STRICT

DESCRIPTION
  This function is used to decode the APN name returned by the network. This
  function leaves out the operator identified. It picks up only the labels
  that reflect the network identifier. This does not check if the APN string
  has more than DS_3GPP_OP_IDENTIFIER_LABEL_COUNT labels.

PARAMETERS
  IN
  encoded_apn - Pointer to the APN returned by the network. This is encoded
  encoded_apn_length - length of the encoded string

  OUT
  decoded_apn - Pointer to the byte array in which the decoded APN is returned

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if decoding is successful
            FALSE if there is any error
   
SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_context_decode_apn_name_non_strict
(
  byte*  encoded_apn,
  uint32 encoded_apn_length,
  byte*  decoded_apn
)
{
  uint32  label_count = 0;
  uint32  label_length = 0;
  uint32  index = 0;
  boolean ret_val = TRUE;
  uint32  apn_string_offset = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG1_MED("Encoded APN length: %d",encoded_apn_length);
  DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Encoded APN name: %s",encoded_apn);

  /*------------------------------------------------------------------------
    Traverse the encoded APN to determine the number of labels in the string
  ------------------------------------------------------------------------*/
  while(index < encoded_apn_length)
  {
    /*----------------------------------------------------------------------
      Get the length of the label
      Increment the label count
      Increase the offset into the string to point to the next label length
      indicator
    ----------------------------------------------------------------------*/
    label_length = (uint32)(*(encoded_apn+index));

    if(label_length > 0)
    {
      label_count++;
    }
    /*--------------------------------------------------------------------------
      Always increment by label_length+1. This is to ensure that we get to the
      next label_length indicator
    --------------------------------------------------------------------------*/
    index += (label_length+1);

  }// while loop to traverse the encoded APN string

  DS_3GPP_MSG1_MED("Number of labels in APN string: %d",label_count);

  /*--------------------------------------------------------------------------
    Reset the index to 1.
    Also reset the label_length to 0
  --------------------------------------------------------------------------*/
  index = 1;
  label_length = 0;

  /*------------------------------------------------------------------------
    Remember the following while copying from src to dest:
   
    Apn_string_offset is used to index into both buffers, but the offset
    in the src string is always one greater than dest
  ------------------------------------------------------------------------*/
    
  while(index <= label_count)
  {
    label_length = (uint32)(*(encoded_apn+apn_string_offset));

    if((apn_string_offset + label_length) < encoded_apn_length)
    {
      memscpy(decoded_apn+apn_string_offset, 
	      label_length,
              encoded_apn+apn_string_offset+1,
              label_length);

      /*----------------------------------------------------------------------
        Increment the apn_string_offset by label_length
      ----------------------------------------------------------------------*/
      apn_string_offset += label_length;

      if(index == label_count)
      {
        /*--------------------------------------------------------------------
          Terminate the string with NULL
        --------------------------------------------------------------------*/
        *(decoded_apn+apn_string_offset) = 0x0;
      }
      else
      {
        /*--------------------------------------------------------------------
          Write the label delimitor in the decoded string
        --------------------------------------------------------------------*/
        *(decoded_apn+apn_string_offset) = LABEL_DELIMITOR;

        /*--------------------------------------------------------------------
          Increment the apn_string_offset by one.
        --------------------------------------------------------------------*/
        apn_string_offset++;
      }
      index++;
    }
    else
    {
      DS_3GPP_MSG0_ERROR("Error decoding. Avoiding buf overrun on Src Str");
      ret_val = FALSE;
      break;
    }
  } //while loop to extract labels corresponding to N/w Identifier

  return ret_val;
} /* ds_3gpp_pdn_context_decode_apn_name_non_strict */

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_validate_PCSCF_address

DESCRIPTION
  This function validates if the network returned PCSCF address is consistent
  with the rules specified in the APM.
 
  If the rules defined in APM are not met by the Ifaces in this PDN context;
  a command is queued to tear down the PDN connection.

PARAMETERS 
 
  pdn_context_p (IN) :  PDN context object


DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if PCSCF addreess is valid according to the rules
            FALSE if  PCSCF adderess is not valid according to the rules
   
SIDE EFFECTS
  None
===========================================================================*/

boolean ds_3gpp_pdn_context_validate_PCSCF_address
(
  ds_pdn_context_s          *pdn_context_p,
  boolean                   *v4_or_v6_pcscf_is_present
)
{
  ds_dsd_apm_req_pay_load        req_payload = {0};  
  boolean                        v4_pcscf_is_present = FALSE;
  boolean                        v6_pcscf_is_present = FALSE;  
  boolean                        pcscf_addr_is_valid = TRUE;  
  int                            i = 0;
  sys_modem_as_id_e_type         subs_id=SYS_MODEM_AS_ID_NONE;
  ds_cmd_type                   *cmd_ptr;
#ifdef FEATURE_DATA_LTE
  ps_iface_net_down_reason_type ps_down_reason = 
                                    PS_NET_DOWN_REASON_INVALID_PCSCF_ADDR;
  boolean                       throttling_is_allowed = FALSE;
                                 /* throttling_is_allowed flag ensures that we 
                                    don't increment throttling counter for LTE
                                    Attach Failure scenarios if this is a VZW 
                                    configuration. */
#endif /*FEATURE_DATA_LTE*/
  ds_dsd_apm_ind_pay_load       ind_payload;
  ds_bearer_context_s           *bearer_cntx_p = NULL;
  ds_3gpp_throt_rat_e_type      rat_type = DS_3GPP_THROT_RAT_GLOBAL; 
                                /* RAT on which throttling needs to be
                                   performed */  
  /*---------------------------------------------------------------------------*/

  memset(&ind_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));

  if(!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
  {
    return FALSE;
  }

  if(! ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,&subs_id))
  {
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  if(NULL == v4_or_v6_pcscf_is_present)
  {
    DS_3GPP_MSG0_ERROR("Invalid Input parameters passed for Boolean params");
    return FALSE;
  }

  if (ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat(subs_id))
  {
    bearer_cntx_p = pdn_context_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
  
    if (ds_bearer_cntx_validate_bearer_context(bearer_cntx_p))
    {
      rat_type = ds_3gpp_throt_map_cm_sys_mode_to_rat_type
                   (bearer_cntx_p->ds_bearer_context_dyn_p->call_mode);
    }
  }

  /* We allow throttling in the following cases:
     1. Given PDN does not have is_default flag set.
     2. Given PDN has is_default flag set, but there is 
        another PDN which is already in UP State 
     3. VZW Configuration is not enabled
  */

#ifdef FEATURE_DATA_LTE
  if (ds_pdn_cntxt_is_default(pdn_context_p) == FALSE || 
      ds_3gpp_nv_manager_get_reg_pdn_throttling(subs_id))
  {
    throttling_is_allowed = TRUE;
  }

  if (ds_pdn_cntxt_is_default(pdn_context_p) == TRUE &&
      dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id) == TRUE)
  {
    throttling_is_allowed = TRUE;
  }
#endif /*FEATURE_DATA_LTE*/

  /* Get the PCSCF rules from APM */  
  req_payload.profile_id = (uint16)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num;
  req_payload.rule_type = (TRUE == dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id)) 
                          ? DS_DSD_APM_ON_DEMAND_RULE : DS_DSD_APM_ATTACH_RULE;

  (void)ds_dsd_apm_req_ex( DS_DSD_APM_MANDATORY_PCSCF_REQ, 
                           &req_payload,
                           subs_id );
  DS_3GPP_MSG3_HIGH("APM response to MANDATORY_PCSCF_REQ is %d",
                    req_payload.mandatory_pcscf, 0, 0); 

  for (i = 0; i< DS_3GPP_MAX_IFACE_PER_PDN_CONTEXT; i++)
  {
    ds_3gpp_iface_s *iface_p = pdn_context_p->
      ds_pdn_context_dyn_p->ds_3gpp_iface_tbl[i];
    ps_iface_type *ps_iface = NULL;
    ip_addr_type pscf_addr[PS_IFACE_MAX_SIP_SERVER_ADDRESSES];
    uint32 count = PS_IFACE_MAX_SIP_SERVER_ADDRESSES;
    uint32 index = 0;

    //If this is a single bearer call; validate only the granted Iface.
    if (NULL == iface_p)
    {
      continue;
    }

    ps_iface = &(iface_p->ps_iface);

    /* Get all the SIP addressess configured for this Iface and check if a V4 or 
    V6 address is present */
    ps_iface_get_sip_serv_addr(ps_iface, pscf_addr, &count);

    for (index = 0; index < count; index ++)
    {
      if (IPV4_ADDR == pscf_addr[index].type)
      {
        v4_pcscf_is_present = TRUE;        
      }
      else if (IPV6_ADDR == pscf_addr[index].type)
      {
        v6_pcscf_is_present = TRUE;        
      }
    }

    switch (ps_iface_get_addr_family(ps_iface))
    {
      case IFACE_IPV4_ADDR_FAMILY :
        if ( ipv4_mandatory_and_ipv6_donotcare == req_payload.mandatory_pcscf ||
             ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf )
         { 
            /* V4 address was mandatory by the NW did not supply it */
            if (FALSE == v4_pcscf_is_present)
            {              
              pcscf_addr_is_valid = FALSE;

#ifdef FEATURE_DATA_LTE
              if (throttling_is_allowed&&
                 (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE))
              {
                ds_3gpp_pdn_throt_perform_reg_throttling
                  (NULL, pdn_context_p, (void *)ps_down_reason, 
                   DS_UMTS_PDP_IPV4, rat_type);
              }
#endif /*FEATURE_DATA_LTE*/
            }
        }
        break;

      case IFACE_IPV6_ADDR_FAMILY:
        if ( ipv6_mandatory_and_ipv4_donotcare == req_payload.mandatory_pcscf ||
             ipv4_mandatory_and_ipv6_mandatory == req_payload.mandatory_pcscf )
        { 
          /* V6 address was mandatory by the NW did not supply it. */
          if (FALSE == v6_pcscf_is_present)
          {          
            pcscf_addr_is_valid = FALSE;

#ifdef FEATURE_DATA_LTE
            if (throttling_is_allowed&&
               (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) != TRUE))
            {
              ds_3gpp_pdn_throt_perform_reg_throttling
                (NULL, pdn_context_p, (void *)ps_down_reason, 
                 DS_UMTS_PDP_IPV6, rat_type);
            }
#endif /*FEATURE_DATA_LTE*/
          }
        }
        break;

      default:
        break;

    }

  }

  if (ipv4_mandatory_or_ipv6_mandatory == req_payload.mandatory_pcscf)
  {
    if ( (FALSE == v4_pcscf_is_present) &&
         (FALSE == v6_pcscf_is_present) )
    {
      pcscf_addr_is_valid = FALSE;
    }
  }

  /* No validation required in this case. */
  if (ipv4_dontcare_and_ipv6_donotcare == req_payload.mandatory_pcscf)
  {
    pcscf_addr_is_valid = TRUE;
  }

  if (FALSE == pcscf_addr_is_valid)
  {
    /* Post a command to bring down this pdn context. */
    cmd_ptr = ds_allocate_cmd_buf(sizeof(void*));
    if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
    {
      ASSERT(0);
      return FALSE;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_IFACE_TEARDOWN_INVALID_PCSCF;
    cmd_ptr->cmd_payload_ptr = (void*)pdn_context_p;

    ds_put_cmd_ext(cmd_ptr);
  }

  //Inform DSD about the PCSCF address.
  if (FALSE == dsPdnContext_is_any_other_normal_pdn_up(pdn_context_p, subs_id))
  {
    if (v6_pcscf_is_present)
    {
      DS_3GPP_APM_SET_VALID_V6_PCSCFADDR(ind_payload);
    }
    else 
    {
      DS_3GPP_APM_SET_INVALID_V6_PCSCFADDR(ind_payload);
    }

    if (v4_pcscf_is_present)
    {
      DS_3GPP_APM_SET_VALID_V4_PCSCFADDR(ind_payload);
    }
    else 
    {
      DS_3GPP_APM_SET_INVALID_V4_PCSCFADDR(ind_payload);
    }    
    DS_3GPP_APM_SET_PROFILE_ID(ind_payload, (uint16)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num;);
    ind_payload.subs_id = subs_id;
    (void)ds_dsd_apm_ind(DS_DSD_APM_IP_ADDRESS_IND, &ind_payload);  
  }

  *v4_or_v6_pcscf_is_present = (v4_pcscf_is_present | v6_pcscf_is_present);

  return pcscf_addr_is_valid;
 
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_VALIDATE_PDN_CONTEXT_INTERNAL

DESCRIPTION
  This function checks if the pdn context tbl has been allocated dynamic memory.
  This function does not print F3 if context is invalid.

PARAMETERS
  pdn_context_p       - pdn_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_validate_pdn_context_internal
(
  ds_pdn_context_s *pdn_context_p
)
{
  boolean                  ret_val = FALSE;
  ds_pdn_context_dyn_s    *pdn_context_dyn_p = NULL;
  sys_modem_as_id_e_type   subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pdn_context_p != NULL)
  {
    pdn_context_dyn_p = pdn_context_p->ds_pdn_context_dyn_p;

    if ((pdn_context_dyn_p != NULL) &&
       (pdn_context_dyn_p->cookie == PDN_CNTXT_VALID_COOKIE))
    {
      DS_3GPP_MSG3_LOW("Dynamic memory allocated at index:%d is valid",
                       pdn_context_dyn_p->index,0,0);
      subs_id = pdn_context_dyn_p->subs_id;
      if (ds3gsubsmgr_is_subs_id_valid(subs_id))
      {
        ret_val = TRUE;
      }
    }
  }

  return ret_val;
} /*ds_3gpp_pdn_cntx_validate_pdn_context_internal*/

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_validate_pdn_context

DESCRIPTION
  This function checks if the pdn context tbl has been allocated dynamic memory

PARAMETERS
  pdn_context_p       - pdn_context to be validated
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_pdn_cntx_validate_pdn_context
(
  ds_pdn_context_s *pdn_context_p
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_context_p) == FALSE)
  {
    DS_3GPP_MSG1_ERROR("PDN context:0x%x is invalid",pdn_context_p);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_alloc_dyn_mem

DESCRIPTION
  This function allocates dyn memory.
 
PARAMETERS
  index   - index where the memory would be allocated
  subs_id - Subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
static boolean ds_3gpp_pdn_cntx_alloc_dyn_mem
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  boolean ret_val = FALSE;
  ds_pdn_context_dyn_s *pdn_context_dyn_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((index < 0) || (index >= DS_3GPP_MAX_PDN_CONTEXT))
  {
    DS_3GPP_MSG1_ERROR("Index:%d is out of bounds",index);
    return ret_val;
  }

  pdn_context_dyn_p = (ds_pdn_context_dyn_s *)ds_pdn_context_tbl[index].
                                                         ds_pdn_context_dyn_p;

  if (pdn_context_dyn_p  == NULL)
  {
    pdn_context_dyn_p = (ds_pdn_context_dyn_s *)
    modem_mem_alloc(sizeof(ds_pdn_context_dyn_s), MODEM_MEM_CLIENT_DATA);

    if (pdn_context_dyn_p != NULL)
    {
      memset(pdn_context_dyn_p,0,sizeof(ds_pdn_context_dyn_s));
      pdn_context_dyn_p->cookie = PDN_CNTXT_VALID_COOKIE;
      pdn_context_dyn_p->index = index; 

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined(FEATURE_TDSCDMA)
      if(!ds_3gpp_dyn_mem_hdlr_allocate_umts_pdn_context(index, subs_id))
      {
        ds_pdn_context_tbl[index].ds_pdn_context_dyn_p = pdn_context_dyn_p;
        ds_3gpp_pdn_cntx_free_dyn_mem(&ds_pdn_context_tbl[index], subs_id);
        DS_3GPP_MSG0_ERROR("Dynamic memory alloc failed during umts pdn context allocation, return NULL");
        ret_val = FALSE;
        return ret_val;
      }
#endif /* (FEATURE_DATA_WCDMA_PS) || (FEATURE_GSM_GPRS) || (FEATURE_TDSCDMA) */
      ds_pdn_context_tbl[index].ds_pdn_context_dyn_p = pdn_context_dyn_p;
      DS_3GPP_MSG1_MED("PDN context dyn mem allocated at index:%d",
                       pdn_context_dyn_p->index);
      ret_val = TRUE;
    }
    else
    {
       DS_3GPP_MSG0_ERROR("Failed to allocate heap memory");
    }
  }
  else
  {
    //assert
    DS_3GPP_MSG1_ERROR("Dynamic memory already allocated at index:%d",
                        index);
  }

  return ret_val;
}



/*===========================================================================
FUNCTION ds_3gpp_pdn_cntx_free_dyn_mem

DESCRIPTION
  This function frees dyn memory.
 
PARAMETERS 
  ds_pdn_context_s* pdn_context_p 
  subs_id           subscription id
    
DEPENDENCIES
  None.

RETURN VALUE
  Void 
 
SIDE EFFECTS
  None

=============================================================================*/
void ds_3gpp_pdn_cntx_free_dyn_mem
(
   ds_pdn_context_s*      pdn_context_p,
   sys_modem_as_id_e_type subs_id
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*----------------------------------------------------------------------------- 
  Don't care about the cookie because we are anyways freeing memory
  -----------------------------------------------------------------------------*/
  if((pdn_context_p != NULL) && (pdn_context_p->ds_pdn_context_dyn_p != NULL))
  {
    DS_3GPP_MSG0_MED("Freeing dyn allocated PDN pointer");

   /* FRREING Umts pdn context*/
    ds_3gpp_dyn_mem_hdlr_free_umts_pdn_context
      (pdn_context_p->ds_pdn_context_dyn_p->index, subs_id);

    modem_mem_free(pdn_context_p->ds_pdn_context_dyn_p,
                 MODEM_MEM_CLIENT_DATA);

    pdn_context_p->ds_pdn_context_dyn_p = NULL;

  }
  else
  {
    DS_3GPP_MSG0_ERROR("Invalid pdn context passed");
  }

}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_HANDLE_EMERGENCY_ATTACH_STATUS

DESCRIPTION
  This function send event to ps whether emergency pdn is only pdn active 
  or not
 
PARAMETERS
  pdn_cntx_p - ds_pdn_context_s 

DEPENDENCIES
  None.

RETURN VALUE  
  none

SIDE EFFECTS  
===========================================================================*/

void ds_3gpp_pdn_cntxt_handle_emergency_attach_status
(
  ds_pdn_context_s    *pdn_cntx_p
)
{
   ps_sys_is_pdn_only_emergency_type pdn_only_emergency ;
   int16                             dss_errno = 0;
   sys_modem_as_id_e_type            local_subs_id = SYS_MODEM_AS_ID_NONE;
   /*---------------------------------------------------------------------*/

   if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntx_p, &local_subs_id))
   {
      pdn_only_emergency.emergency_pdn_only = 
        ds_3gpp_pdn_context_only_emergency_pdn_is_up(local_subs_id);

      ps_sys_conf_set_ex(PS_SYS_TECH_3GPP,
                         PS_SYS_CONF_3GPP_EMERGENCY_PDN_STATUS,
                         (ps_sys_subscription_enum_type)
                           ds3gsubsmgr_subs_id_cm_to_ds(local_subs_id),
                         &pdn_only_emergency, &dss_errno);
   }
} /* ds_3gpp_pdn_cntxt_handle_emergency_attach_status */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CONTEXT_ONLY_EMERGENCY_PDN_IS_UP

DESCRIPTION
  This function loops over all PDN contexts to see if emergency is the only 
  pdn is active.
 
PARAMETERS
  subs_id 

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, If a only Emergency APN is up.
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_3gpp_pdn_context_only_emergency_pdn_is_up
(
   sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_EMERGENCY_PDN
  int                     pdn_instance = 0;
  boolean                 emergency_pdn_up = FALSE;
  boolean                 non_emergency_pdn_up = FALSE;
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_instance = 0; pdn_instance < DS_3GPP_MAX_PDN_CONTEXT; pdn_instance++)
  {
    local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
    if(!ds_pdn_cntxt_validate_pdn_subscription_internal(local_pdn_cntx_p, subs_id))
    {
      continue;
    }
    if ((local_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP) &&
             (TRUE == DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p)) )
    {
      emergency_pdn_up = TRUE;
    }
    else if ((local_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP) &&
             (FALSE == DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p)) )
    {
      non_emergency_pdn_up = TRUE;
    }
  }
  return ((!non_emergency_pdn_up) & emergency_pdn_up);
#else
  DS_3GPP_MSG0_HIGH("Emergency PDN not Supported!!");
  return FALSE;
#endif /* FEATURE_EMERGENCY_PDN */  
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_context_emergency_pdn_is_up

DESCRIPTION
  This function loops over all PDN contexts to see if a data call to the 
  emergency APN is active.
 
PARAMETERS
  boolean: coming_up, to check whether emrgency PDN is in coming_up state. 
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, If a call to the Emergency APN is up or coming_up 
        (based on coming_up flag).
  FALSE, otherwise

SIDE EFFECTS  
===========================================================================*/
boolean ds_3gpp_pdn_context_emergency_pdn_is_up
(
  boolean                coming_up,
  sys_modem_as_id_e_type subs_id
)
{
#ifdef FEATURE_EMERGENCY_PDN
  int               pdn_instance;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for(pdn_instance = 0; pdn_instance < DS_3GPP_MAX_PDN_CONTEXT; pdn_instance++)
  {
    ds_pdn_context_s *local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);

    if (!(ds_pdn_cntxt_validate_pdn_subscription_internal(local_pdn_cntx_p, subs_id)))
    {     
      continue;
    }
    else if (FALSE == DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntx_p))
    {
      continue;
    }
    else
    {
      /* Return TRUE if PDN is in coming_up state if caller has indicated this
         using the coming_up parameter */
      if (coming_up)
      {
        if ( (local_pdn_cntx_p->ds_pdn_context_dyn_p->state == 
				               DS_PDN_CONTEXT_STATE_UP) ||
             (local_pdn_cntx_p->ds_pdn_context_dyn_p->state == 
	                                       DS_PDN_CONTEXT_STATE_COMING_UP) )  
        {
          return TRUE;
        }
      }
      else
      {
      if(local_pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
        {
          return TRUE;
        }
      }
    }
  }
#else
  DS_3GPP_MSG0_HIGH("No data call to emergency APN active");
#endif /* FEATURE_EMERGENCY_PDN */
  return FALSE;
} /* ds_3gpp_pdn_context_emergency_pdn_is_up */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_TEAR_DOWN

DESCRIPTION
  This function when invoked tears down all pdn connections which does not
  have active clients

PARAMETERS
  subs_id: Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/

void ds_3gpp_pdn_cntxt_tear_down
(
  sys_modem_as_id_e_type subs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int                     pdn_inst          = DS_3GPP_MAX_PDN_CONTEXT;
  ds_pdn_context_s        *pdn_cntx_p       = NULL;
  boolean                 retval            = DS3G_FAILURE;
  ds_bearer_context_s*    bearer_context_p  = NULL;
  boolean                 tear_down         = FALSE;  
#ifdef FEATURE_EPC_HANDOFF
  ps_iface_type*          v4_iface_p        = NULL;
  ps_iface_type*          v6_iface_p        = NULL;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
  Loop over all PDN contexts to find out which ones we have to tear
  -----------------------------------------------------------------------*/
  for(pdn_inst = 0; pdn_inst < DS_3GPP_MAX_PDN_CONTEXT; pdn_inst++)
  {
    pdn_cntx_p = &ds_pdn_context_tbl[pdn_inst];

    /*---------------------------------------------------------------------
      Initialize the tear_down to be FALSE. If it is subsequently set to
      TRUE, we will tear down this PDN context
    ---------------------------------------------------------------------*/
    tear_down = FALSE;
    /*---------------------------------------------------------------------
    If the PDN context is in DOWN state, continue with the next one
    ---------------------------------------------------------------------*/
    if(!(ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_cntx_p, subs_id)) ||
        pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_DOWN ||
        pdn_cntx_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_GOING_DOWN)
    {
      continue;
    }
    /*---------------------------------------------------------------------
    Get the bearer context pointer as we will need this to tear down, if
    we have to 
    ps: If there are any dedicated bearers for in active clients, they will 
    be automatically torn down before tearing down the default bearer .
    ---------------------------------------------------------------------*/
    bearer_context_p = (ds_bearer_context_s*)
                        pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      continue;
    }

#ifndef FEATURE_EPC_HANDOFF
    /*---------------------------------------------------------------------
    This is the case where there is no EPC MH. Tear down the context on
    sys mode change
    ---------------------------------------------------------------------*/
    tear_down = TRUE;
#else
    /*---------------------------------------------------------------------
      In this case we should tear down only contexts that have no client.
      This is because EPC mode handler transfers only those contexts
      that have clients
    ---------------------------------------------------------------------*/

    v4_iface_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_cntx_p);
    v6_iface_p = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_cntx_p);

    if((v4_iface_p == NULL || PS_IFACE_IS_IN_USE(v4_iface_p) == FALSE) &&
       (v6_iface_p == NULL || PS_IFACE_IS_IN_USE(v6_iface_p) == FALSE))
    {
      /*-------------------------------------------------------------------
      Both Ifaces in the PDN context are out of use
      -------------------------------------------------------------------*/
      tear_down = TRUE;
    } /* are ifaces out of use? */
#endif /* FEATURE_EPC_HANDOFF */

    if(tear_down == TRUE)
    {
      DS_3GPP_MSG1_HIGH("Tearing down all bearers associated with PDN %0x",pdn_cntx_p);
#ifdef FEATURE_DATA_LTE
      retval = ds_eps_bearer_cntxt_tear_down_pdn_connection(
                                    bearer_context_p,NULL, NULL);
#else
      retval = DS3G_FAILURE;
#endif 
      if(retval != DS3G_SUCCESS)
      {
        ds_pdn_cntxt_unbind_default_flows(pdn_cntx_p);
        
        /*---------------------------------------------------------------------
          Each enqueue func_ptr needs to be reset when bearer inst is freed.
          This needs to be done to avoid race conditions when phys_link_gone is
          posted and flow gets enabled on the phys_link leading to each_enq_f
          called when the bearer is getting freed.
        ---------------------------------------------------------------------*/
        /*--------------------------------------------------------------- 
            Do a local clean up since there was an error in tearing down
            the call.
        ---------------------------------------------------------------*/
        ds_bearer_cntxt_free_bearer_inst(bearer_context_p);            
		
        ps_phys_link_gone_ind(&(bearer_context_p->phys_link));
        
        ps_phys_link_enable_flow(&(bearer_context_p->phys_link),
                                 DS_FLOW_UMTS_MASK);
                
        /*--------------------------------------------------------------- 
          Post bearer down ind so that we can clean up the PDN context.
          ---------------------------------------------------------------*/
        if(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
           ->bearer_down_ind_cb != NULL)
        {
          ds_3gpp_down_reason_t net_down_reason;
          net_down_reason.reason.ps_iface_down_reason = 
            PS_NET_DOWN_REASON_INTERNAL_CALL_ENDED;
          net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;

          bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr->
                           bearer_down_ind_cb(
                           bearer_context_p->ds_bearer_context_dyn_p->
                           pdn_context_ptr,\
                           bearer_context_p,
                           net_down_reason,
                           FALSE);
        }/* Post bearer down indication with call_was_connected as False*/
      }/* Tear down request returned failure */      
    } /* if we decided to tear down the context*/
  } /* loop over all PDN contexts*/
} /* ds_eps_pdn_cntxt_sys_change_handler */

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_PLMN_BLOCKED

DESCRIPTION
  This function is used to check if the given PLMN is blocked due to t3396 throttling
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN is blocked
  FALSE, otherwise

SIDE EFFECTS
None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_plmn_blocked
(
  sys_plmn_id_s_type	 plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  list_type				 *list_ptr = NULL;
  boolean 		 	         ret = FALSE;
  ds_3gpp_pdn_cntxt_blocked_plmn_type    *curr_blocked_plmn_ptr;
  /*-----------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);
  if (list_ptr == NULL||list_size(list_ptr) == 0)
  {
    DS_3GPP_MSG0_ERROR("list ptr NULL");
    return FALSE;
  }

  curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

  while (curr_blocked_plmn_ptr != NULL)
  {
    if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                &plmn_id,
                sizeof(sys_plmn_id_s_type)) == 0)
    {
      ret = TRUE;
      break;
    }
          
    /* Get the next item in the list */
    curr_blocked_plmn_ptr = 
     list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
  }
  DS_3GPP_MSG1_HIGH("ds_3gpp_pdn_cntxt_is_plmn_blocked returns %d",ret);
  return ret;
}/*ds_3gpp_pdn_cntxt_is_plmn_blocked*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_DISABLE_LTE_PER_PLMN_SELECTION_EX

DESCRIPTION
  This function disables LTE per PLMN for the given PLMN Id. It also adds
  the PLMN to blocked PLMN List.

PARAMETERS
  PLMN Id
  Timer Value for which PLMN will be blocked
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PLMN Disabled successfully
  FALSE, if PLMN Disable failed or PLMN already disabled

SIDE EFFECTS
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex
(
  sys_plmn_id_s_type      plmn_id,
  uint32                  throttle_timer_val,
  sys_modem_as_id_e_type  subs_id,
  boolean                 detach
)
{
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *new_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *current_blocked_plmn_ptr = NULL;
  boolean                                ret = FALSE;
  uint8                                  failure_count = DS_THROT_LOG_PARAM_NA;
  /*-----------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  /*--------------------------------------------------------------------------   
    Here we check if the passed PLMN is already blocked. If it is, we do not 
    need to block it again. 
    If it is not blocked, we block the current PLMN and add it to the blocked PLMN 
    list. 
  ----------------------------------------------------------------------------*/

  DS_3GPP_MSG1_HIGH("Trying to block PLMN for %d msecs", throttle_timer_val);

  ds3g_print_plmn_id(&plmn_id);

  if(ds_3gpp_throt_prep_for_lte_blocking_and_log(plmn_id, 
                                                 throttle_timer_val,
                                                 subs_id,
                                                 failure_count, detach))
  {
    DS_3GPP_MSG0_HIGH("Updating blocked PLMN List");

    list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);

    if (list_ptr != NULL)
    {
      // Check if the plmn is already blocked
        current_blocked_plmn_ptr =
          (ds_3gpp_pdn_cntxt_blocked_plmn_type *)list_peek_front(list_ptr);
      while (current_blocked_plmn_ptr != NULL)
      {
        if (memcmp(&(current_blocked_plmn_ptr->plmn_id), &plmn_id,
                     sizeof(sys_plmn_id_s_type)) == 0)
        {
          DS_3GPP_MSG1_HIGH("PLMN is already blocked on subs ID %d",
                             subs_id);
          return FALSE;
        }

        current_blocked_plmn_ptr =
          list_peek_next(list_ptr, &(current_blocked_plmn_ptr->link));
      }
      
      new_blocked_plmn_ptr  = 
       (ds_3gpp_pdn_cntxt_blocked_plmn_type *)modem_mem_alloc
         (sizeof(ds_3gpp_pdn_cntxt_blocked_plmn_type),MODEM_MEM_CLIENT_DATA);

      if (new_blocked_plmn_ptr != NULL)
      {
        new_blocked_plmn_ptr->plmn_id = plmn_id;

        /*----------------------------------------------------------------------
         Push the list item into the list
        ----------------------------------------------------------------------*/
        list_push_front(list_ptr, &(new_blocked_plmn_ptr->link));
        ret = TRUE;
      }
      else
      {
          DS_3GPP_MSG0_ERROR("Could not allocate memory");
      }
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("PLMN blocking failed ");
    ds3g_print_plmn_id(&plmn_id);
  }
  return ret;

} /* ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex */

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_PLMN_BLOCKING_ON_PLMN_ID_EX

DESCRIPTION
  This function enables tyhe given PLMN and removes it from the
  blocked PLMN List.  
PARAMETERS  
  PLMN id
  Susbcription id
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE, if PLMN Unblocked successfully
  FALSE, if PLMN was already unblocked or PLMN unblock failed.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
(
  sys_plmn_id_s_type     plmn_id,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_cntxt_blocked_plmn_type       *curr_blocked_plmn_ptr;
  list_type                                 *list_ptr = NULL;
  boolean                                   ret = FALSE;
  boolean                                   found_match = FALSE;

  /*--------------------------------------------------------------------------   
    Here we check if the passed PLMN is unblocked. If it is not blocked, 
    no need to unblock again.
  ----------------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }

  DS_3GPP_MSG0_HIGH("Trying to unblock PLMN ");

  ret = ds3g_msh_enable_lte_plmn_blocking_on_plmn_id_ex(plmn_id, 
              CM_PLMN_BLOCKING_PREF_PLMN_UNBLOCKING_ONLY, subs_id);

  ds3g_print_plmn_id(&plmn_id);

  if (ret)
  {
    list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);

    if (list_ptr != NULL)
    {
      curr_blocked_plmn_ptr = 
        (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );

      while (curr_blocked_plmn_ptr != NULL)
      {
        if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                    &plmn_id,
                    sizeof(sys_plmn_id_s_type)) == 0)
        {
          found_match = TRUE;
          break;
        }
              
        /* Get the next item in the list */
        curr_blocked_plmn_ptr = 
          list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
      }
    }

    if (found_match)
    {
      list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );

      modem_mem_free(curr_blocked_plmn_ptr,MODEM_MEM_CLIENT_DATA);
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("PLMN unblocking failed ");
    ds3g_print_plmn_id(&plmn_id);
  }

  return ret;

} /* ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_LTE_UNBLOCK_ALL_PLMNS

DESCRIPTION
  This function unblocks all the blocked PLMNs.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns
(
  sys_modem_as_id_e_type  subs_id
)
{
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
   list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }

  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);

  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

      ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
        (curr_blocked_plmn_ptr->plmn_id, subs_id);

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_BLOCKED_PLMN_LIST

DESCRIPTION
  This function frees the Blocked PLMN List.

PARAMETERS
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_blocked_plmn_list
(
  sys_modem_as_id_e_type  subs_id
)
{
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
   list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */

   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }

  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/

  list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);

  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

      list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );

      modem_mem_free(curr_blocked_plmn_ptr,MODEM_MEM_CLIENT_DATA);

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} /* ds_3gpp_pdn_cntxt_flush_blocked_plmn_list */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_FLUSH_PLMN_FROM_LIST

DESCRIPTION
  This function frees the PLMN specified in the blocking List.
PARAMETERS 
 sys_plmn_id_s_type unblock_plmn_id  - UNBLOCK PLMN ID
 sys_modem_as_id_e_type subs_id - SUBS_ID 

DEPENDENCIES
  None.

RETURN VALUE  
  None.
SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntxt_flush_plmn_from_list
(
  sys_plmn_id_s_type unblock_plmn_id,
  sys_modem_as_id_e_type  subs_id
)
{
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
   ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
   list_type                             *list_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - */
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }

  /*--------------------------------------------------------------------------   
    Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/
  list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);
  if((list_ptr != NULL) && (list_size(list_ptr) != 0))
  {
    curr_blocked_plmn_ptr = 
      (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );    
    while ( curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
        list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));

      if ( memcmp(&(curr_blocked_plmn_ptr->plmn_id),
                    &unblock_plmn_id,
                    sizeof(sys_plmn_id_s_type)) == 0)
      {
        list_pop_item(list_ptr, &(curr_blocked_plmn_ptr->link) );
        modem_mem_free(curr_blocked_plmn_ptr,MODEM_MEM_CLIENT_DATA);
        break;
      }
      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} /* ds_3gpp_pdn_cntxt_flush_plmn_from_list */

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_CHECK_IF_ATTACH_PROFILE_AVAILABLE
DESCRIPTION
  This function checks if Attach Profile is Available. 
  We check both case where attach profile list is configured and not configured.
PARAMETERS  
  Subscription Id
DEPENDENCIES 
  None.  
RETURN VALUE 
  DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN - If Attach profile status is unknown.
  DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL - If Attach profile is available
  DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL - If Attach profile is not available
SIDE EFFECTS 
  None.
===========================================================================*/
ds_pdn_cntxt_attach_profile_status_type
  ds_3gpp_pdn_cntxt_check_if_attach_profile_available
(
  sys_modem_as_id_e_type    subs_id
)
{

  byte                                    def_sock_prof;
  ds_umts_pdp_profile_status_etype        profile_status;
  ds_dsd_apm_req_pay_load                 apm_payload = {0};
  ds_dsd_ext_error_code                   dsd_result = DS_DSD_APM_SUCCESS;
  byte                                    prof_apn_name
                                           [DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_pdn_cntxt_attach_profile_status_type attach_profile_status = 
                                            DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
 /*----------------------------------------------------------------------------*/

  do
  {
    DS_3GPP_MSG1_MED("Checking if Attach profile is available. Subs Id: %d", subs_id);

    dsd_result = ds_dsd_apm_req_ex( DS_DSD_APM_ATTACH_APN_AVAIL_REQ, 
                                     &apm_payload,
                                     subs_id );
     
    if(dsd_result != DS_DSD_APM_SUCCESS)
    {
      DS_3GPP_MSG0_ERROR("APM should always return success for this request:api has gone bad "
                         "DS_DSD_APM_ATTACH_APN_AVAIL_REQ ");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN;
      break;
    }

    if (DS_DSD_APM_ATTACH_APN_AVAILABLE == apm_payload.attach_apn_avail)
    {
      DS_3GPP_MSG0_HIGH("Atleast one attach APN is available");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
    }
    else if (DS_DSD_APM_ATTACH_APN_BLOCKED == apm_payload.attach_apn_avail )
    {
      DS_3GPP_MSG0_HIGH("All Attach APNs are blocked");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
    }

    else if((DS_DSD_APM_EFS_CONFIG_ERROR == apm_payload.attach_apn_avail)|| 
             (DS_DSD_APM_PROF_CONFIG_ERROR == apm_payload.attach_apn_avail))
    {    
      profile_status = ds_umts_get_pdp_profile_num_for_sock_calls(&def_sock_prof);
      if (DS_UMTS_PDP_SUCCESS == profile_status)
      {
        memset(prof_apn_name, 0, DS_UMTS_MAX_APN_STRING_LEN+1);
        profile_status = ds_umts_get_pdp_profile_context_info_apn_per_subs(
                                             (uint16)def_sock_prof,
                                             dsumts_subs_mgr_get_subs_id(subs_id),
                                              prof_apn_name,
                                              DS_UMTS_MAX_APN_STRING_LEN);
        if (DS_UMTS_PDP_SUCCESS == profile_status)
        {
          /*In cc_sp_throttling case block Attach only if attach APN is t3396 blocked*/
          if( (ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == TRUE)&&
                (!ds_3gpp_pdn_throt_is_apn_globally_throttled(prof_apn_name,subs_id)))
          {
            DS_3GPP_MSG1_HIGH("Default socket profile %d is not throttled",
                              def_sock_prof);
        
            attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
          }
          else if((ds_3gpp_throt_sm_is_cc_sp_throttling_enabled(subs_id) == FALSE)&&
                 (FALSE == (ds_3gpp_pdn_throt_is_apn_throttled(
                             prof_apn_name, DS_3GPP_PDP_TYPE_IP_ANY, subs_id))))
          {
            DS_3GPP_MSG1_HIGH("Default socket profile %d is not throttled",
                              def_sock_prof);        
            attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL;
          }
          else
          {
            DS_3GPP_MSG1_HIGH("Default socket profile: %d has APN throttled, "
                              "No APN to attach", def_sock_prof);
            attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
          }
        }
        else
        {
          DS_3GPP_MSG1_ERROR("Cannot find default socket profile %d. "
                             "Assume Attach profile is unavailable", def_sock_prof);
          attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR("Cannot find default socket profile id. "
                           "Assume Attach profile is unavailable");
        attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL;
      }
    } //attach_apn_avail
    else
    {
      DS_3GPP_MSG0_HIGH("APM returned known error. Do-not block/unblock PLMN");
      attach_profile_status = DS_PDN_CNTXT_ATTACH_PROFILE_UNKNOWN;
    }

  } while (0); 
  return attach_profile_status;
} /* ds_3gpp_pdn_cntxt_check_if_attach_profile_available */

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable
(
  sys_modem_as_id_e_type    subs_id
)
{
  boolean     detach_and_block_plmn = FALSE;
  
  if(FALSE == ds_bearer_cntxt_is_active_bearer_cntxt(subs_id))
  { 
    if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available(subs_id) == 
          DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL)
    {
      detach_and_block_plmn = TRUE;
    }
  } //is_active_bearer_cntxt

  if (detach_and_block_plmn)
  {
    ds_3gpp_pdn_cntxt_detach_and_block_plmn(subs_id, TRUE);
  }
} /* ds_3gpp_pdn_cntxt_detach_and_block_plmn_if_attach_profile_unavailable */

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DISABLE_LTE_IF_ATTACH_PROFILE_UNAVAILABLE
DESCRIPTION
  This function disables lte if there are no active bearers and all the attach apn's
  are blocked.we check both case where attach profile list is configured and not configured.
PARAMETERS  
  None.
DEPENDENCIES 
  None.  
RETURN VALUE 
  None
SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable
(
  sys_modem_as_id_e_type    subs_id
)
{
  boolean                               block_plmn = FALSE;
  
  if(FALSE == ds_bearer_cntxt_is_active_bearer_cntxt(subs_id))
  { 
    if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available(subs_id) == 
          DS_PDN_CNTXT_ATTACH_PROFILE_UNAVAIL)
    {
      block_plmn = TRUE;
    }
  } //is_active_bearer_cntxt

  if (block_plmn)
  {
    ds_3gpp_pdn_cntxt_block_plmn(subs_id);
  }
} /* ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable */

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_IF_ATTACH_PROFILE_AVAILABALE

DESCRIPTION
  This function enabled lte if attach apn's are available.
  we check both case where attach profile list is configured
  and not configured (default socket profile).
  
PARAMETERS  
  None.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.
  
===========================================================================*/

void  ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available
(
  sys_modem_as_id_e_type    subs_id
)
{
  boolean unblock_plmn = FALSE;
 
  if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available(subs_id) 
        == DS_PDN_CNTXT_ATTACH_PROFILE_AVAIL)
  {
    unblock_plmn = TRUE;
  }

  if(ds_bearer_cntxt_is_active_bearer_cntxt(subs_id))
  { 
    DS_3GPP_MSG0_HIGH("LTE can be enabled as atleast one call is up");
    unblock_plmn = TRUE;
  }

  if (unblock_plmn)
  {    
    ds_3gpp_pdn_cntxt_enable_lte_unblock_all_plmns(subs_id);
  }
} /* ds_3gpp_pdn_cntxt_enable_lte_if_attach_profile_available */

/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_ENABLE_LTE_EACH_PLMNID

DESCRIPTION
  This function enabled lte on each blocked plmn
  if atleast one attach apn are available. on the blocked plmn list

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_3gpp_pdn_cntxt_enable_lte_each_plmnid
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *curr_blocked_plmn_ptr;
  ds_3gpp_pdn_cntxt_blocked_plmn_type   *next_blocked_plmn_ptr;
  list_type                             *list_ptr = NULL;
  ps_sys_3gpp_lte_attach_pdn_list_type  attach_list_ex = {0};
  int16     ps_errno = 0;
  /*---------------------------------------------------------------------------*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ds_dsd_apm_get_attach_pdn_list(&attach_list_ex, subs_id, &ps_errno);
  /*--------------------------------------------------------------------------
   Here we loop through all the blocked PLMNs and unblock them one by one.
  ----------------------------------------------------------------------------*/
  list_ptr = ds_3gpp_pdn_cntx_get_blocked_plmn_list(subs_id);
  if (list_ptr != NULL)
  {
    curr_blocked_plmn_ptr = 
       (ds_3gpp_pdn_cntxt_blocked_plmn_type*)list_peek_front( list_ptr );
    while (curr_blocked_plmn_ptr != NULL )
    {
      next_blocked_plmn_ptr = 
          list_peek_next(list_ptr, &(curr_blocked_plmn_ptr->link));
      if (ds_3gpp_pdn_cntxt_check_if_attach_profile_available_plmn_id(
                   subs_id,
                   curr_blocked_plmn_ptr->plmn_id,
                   attach_list_ex)) 
      {
         ds_3gpp_pdn_cntxt_enable_lte_plmn_blocking_on_plmn_id_ex
             (curr_blocked_plmn_ptr->plmn_id, subs_id);
      }

      curr_blocked_plmn_ptr = next_blocked_plmn_ptr;
    }
  }
} 
/*===========================================================================
FUNCTION DS_3PP_PDN_CNTXT_IF_ATTACH_PROFILE_AVAILABLE_PLMN_ID

DESCRIPTION
  This function checks if the attach profile is available on the passed plmnid and attach list combination

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_check_if_attach_profile_available_plmn_id
(
  sys_modem_as_id_e_type                       subs_id,
  sys_plmn_id_s_type                           plmn_id,
  ps_sys_3gpp_lte_attach_pdn_list_type  attach_list
)
{
  ds_3gpp_pdn_throt_apn_list_type      *blocked_apn_list = NULL;
  uint8                                 num_blocked_apn = 0;
  byte                                  prof_apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_3gpp_pdn_throt_apn_list_type       *temp_apn_list = NULL;
  boolean                               attach_profile_blocked = FALSE; 
  boolean                               plmn_unblock = FALSE;
  ds_umts_pdp_profile_status_etype      result = DS_UMTS_PDP_FAIL;
  int                                   index = 0;
  int                                   profile_index=0;  
  int                                   blocked_index=0;


  blocked_apn_list = (ds_3gpp_pdn_throt_apn_list_type*)modem_mem_alloc(
                      DS_MAX_APN_NUM*sizeof(ds_3gpp_pdn_throt_apn_list_type),
                      MODEM_MEM_CLIENT_DATA);


  
  if (blocked_apn_list != NULL) 
  {
    memset(blocked_apn_list, 0, sizeof(ds_3gpp_pdn_throt_apn_list_type));
    ds_3gpp_pdn_throt_get_blocked_apn_list_on_plmn_id(
                    blocked_apn_list,
                    &num_blocked_apn, 
                    subs_id,
                    plmn_id);
  }


  for(index=0;index < attach_list.attach_pdn_profile_list_num;index++) 
  {
    attach_profile_blocked = FALSE; 
    memset(prof_apn_name,0,DS_UMTS_MAX_APN_STRING_LEN+1);
    profile_index= attach_list.attach_pdn_profile_list[index];
    result = ds_umts_get_pdp_profile_context_info_apn_per_subs(
                                  profile_index,
                                                     dsumts_subs_mgr_get_subs_id(subs_id),
                                                                      prof_apn_name,
                                                        DS_UMTS_MAX_APN_STRING_LEN);
    
    if( result != DS_UMTS_PDP_SUCCESS )
    {
      DS_3GPP_MSG2_ERROR("Err %d in getting APN info. Prof: %d. Cont.",
                                     result,profile_index);
      plmn_unblock = TRUE;
      continue;
    }
    
    temp_apn_list=blocked_apn_list;
    
    for (blocked_index=0;blocked_index < num_blocked_apn;blocked_index++) 
    {

      DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"Attach List Apn name %s Blocked List Apn Name  %s"
                               ,prof_apn_name,temp_apn_list->apn);
      
      if ((strncasecmp((char*)(temp_apn_list->apn),
           (char*)prof_apn_name,
           DS_UMTS_MAX_APN_STRING_LEN+1)==0) &&
          (temp_apn_list->attach_allowed == FALSE))
      {
        attach_profile_blocked = TRUE; 
      }
      temp_apn_list++;
    }
    if (attach_profile_blocked == FALSE) 
    {
      plmn_unblock = TRUE;
      break;
    }
     
  }
  if (attach_list.attach_pdn_profile_list_num == 0) 
  {
    plmn_unblock = TRUE;
  }

  modem_mem_free(blocked_apn_list, MODEM_MEM_CLIENT_DATA);
  return plmn_unblock;
}

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_DETACH_AND_BLOCK_PLMN
DESCRIPTION
  This function performs LTE detach and blocks the current PLMN.

PARAMETERS  
  Subscription Id

DEPENDENCIES 
  None.  

RETURN VALUE 
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_detach_and_block_plmn
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   detach
)
{
  uint32                throttle_timer_val = 0;
  sys_plmn_id_s_type    plmn_id;
  /*-----------------------------------------------------------------------*/

  throttle_timer_val = 
    ds_3gpp_pdn_throt_lte_disable_throttle_timer_value(subs_id);

  if (throttle_timer_val != 0)
  {
    ds_3gpp_pdn_cntx_get_current_plmn_ex(&plmn_id, subs_id);

    if(ds_3gpp_pdn_cntxt_disable_lte_per_plmn_selection_ex
         (plmn_id,throttle_timer_val,subs_id,detach))
    {
      DS_3GPP_MSG0_HIGH("Plmn Blocking as all the Lte Attach"
                        "Profiles are blocked on current PLMN");
    }
  }
} /* ds_3gpp_pdn_cntxt_detach_and_block_plmn */

/*===========================================================================
FUNCTION DS_3GPP_PDNT_CNTXT_BLOCK_PLMN
DESCRIPTION
  This function function blocks the current PLMN.

PARAMETERS  
  Subscription Id

DEPENDENCIES 
  None.  

RETURN VALUE 
  None

SIDE EFFECTS 
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_block_plmn
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_pdn_cntxt_detach_and_block_plmn(subs_id, FALSE);
} /* ds_3gpp_pdn_cntxt_block_plmn */


#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif //FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SET_THROT_DOWN_REASON

DESCRIPTION
  This function set the throttle down reason based on the IP Type of the
  Iface being passed.
 
PARAMETERS 
  iface_p        - Pointer to the iface which is to be examined
  down_reason    - To be populated
    
DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ds_pdn_cntxt_set_throt_down_reason
(
  ps_iface_type                  *iface_p, 
  ps_iface_net_down_reason_type  *down_reason_p
)
{                        
  ASSERT(iface_p != NULL && down_reason_p != NULL)   
  switch(ps_iface_get_addr_family(iface_p))                         
  {                                                                 
    case IFACE_IPV4_ADDR_FAMILY:                                    
      *down_reason_p = PS_NET_DOWN_REASON_ERR_PDN_IPV4_CALL_THROTTLED; 
      break;                                                         
    case IFACE_IPV6_ADDR_FAMILY:                                    
      *down_reason_p = PS_NET_DOWN_REASON_ERR_PDN_IPV6_CALL_THROTTLED; 
      break;                                                        
    default:                                                        
      *down_reason_p = PS_NET_DOWN_REASON_NOT_SPECIFIED;               
  }                                                                 
}

/*===========================================================================
FUNCTION  ds_3gpp_pdn_cntxt_remove_unused_pdn

DESCRIPTION
  This function when invoked tears down all pdn connections which does not
  have active clients

PARAMETERS
  pdn_cntx: PDN context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void ds_3gpp_pdn_cntxt_remove_unused_pdn
(
  ds_pdn_context_s *curr_pdn_cntx
)
{
  int                      pdn_inst          = 0;
  ds_pdn_context_s        *pdn_cntx_p       = NULL;
  ds_bearer_context_s     *bearer_context_p = NULL;
  ds_3gpp_iface_s         *ds3gpp_v4_iface_p= NULL;    /* Interface ptr for the call */
#ifdef FEATURE_DATA_PS_IPV6
  ds_3gpp_iface_s         *ds3gpp_v6_iface_p= NULL;
#endif
  sys_modem_as_id_e_type   curr_subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("remove unused pdn function ");

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(curr_pdn_cntx, 
                                            &curr_subs_id)== FALSE)
  {
    return;
  }

  /*-----------------------------------------------------------------------
  Loop over all PDN contexts to find out which ones we have to tear
  -----------------------------------------------------------------------*/
  for(pdn_inst = 0; pdn_inst < DS_3GPP_MAX_PDN_CONTEXT; pdn_inst++)
  {
    pdn_cntx_p = &ds_pdn_context_tbl[pdn_inst];
    /*---------------------------------------------------------------------
    If the PDN context is in DOWN state, continue with the next one
    ---------------------------------------------------------------------*/
    if(!(ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_cntx_p, curr_subs_id)) ||
        pdn_cntx_p->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_DOWN  ||
        pdn_cntx_p->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_GOING_DOWN ||
        pdn_cntx_p->ds_pdn_context_dyn_p->index == 
          curr_pdn_cntx->ds_pdn_context_dyn_p->index )
    {
      continue;
    }
    /*---------------------------------------------------------------------
    Get the bearer context pointer as we will need this to tear down, if
    we have to 
    ps: If there are any dedicated bearers for in active clients, they will 
    be automatically torn down before tearing down the default bearer .
    ---------------------------------------------------------------------*/
    bearer_context_p = (ds_bearer_context_s*)
                      pdn_cntx_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;

    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      continue;
    }

    ds3gpp_v4_iface_p = 
         (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
          [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);
 
    ds3gpp_v6_iface_p = 
       (pdn_cntx_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
       [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);

    /* Check the PS IFACE IN USE Flag and in the case of both ifaces being
       false we teardown the PDN */ 

    if (ds3gpp_v4_iface_p == NULL && ds3gpp_v6_iface_p == NULL)
    { 
       DS_3GPP_MSG0_ERROR("Trying to teardown PDN which does not have an valid IFaces ");
       return;
    }
    if( ((ds3gpp_v4_iface_p != NULL) && (PS_IFACE_IS_IN_USE(&(ds3gpp_v4_iface_p->ps_iface))))
        ||
        ((ds3gpp_v6_iface_p != NULL) && (PS_IFACE_IS_IN_USE(&(ds3gpp_v6_iface_p->ps_iface)))) )
    {
      continue;
    }
    else
    {
      DS_3GPP_MSG0_ERROR("PDN is being torn down because there are no "
                         "Ifaces associated with it ");

      pdn_cntx_p->ds_pdn_context_dyn_p->state = DS_PDN_CONTEXT_STATE_GOING_DOWN;
     /*-------------------------------------------------------------------
      Both Ifaces in the PDN context are out of use
      -------------------------------------------------------------------*/
      ds_3gpp_pdn_cntx_teardown_PDN(pdn_cntx_p,PS_NET_DOWN_REASON_INTERNAL_CALL_ENDED);
    } /* are ifaces out of use? */
  } /* loop over all PDN contexts*/
} /* ds_3gpp_remove_unused_pdn */

/*===========================================================================
  FUNCTION DS_3GPP_IS_CONTEXT_PREF_PKT_MODE
  
  DESCRIPTION
    Checks if the current passed system mode is the preferred packet mode for
    the given pdn context
    
  PARAMETERS  
    pdn_context_p     :           Current pdn context
    mode              :           Mode to be checked for preference
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE 
    TRUE  :  When the preferred mode is same as the mode passed in
    FALSE :  WHen the preferred mode does not match the passed in mode
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/

boolean ds_3gpp_is_context_pref_pkt_mode
(  
  ds_pdn_context_s              * pdn_context_p,
  sys_sys_mode_e_type             mode
)
{
  boolean                           ret_val = FALSE;
  /*-----------------------------------------------------------------------*/
  DS_3GPP_MSG0_MED("ds_3gpp_is_context_pref_pkt_mode: Entered");

  if(mode == ds_3gpp_get_apn_current_mode(pdn_context_p))
  {
    ret_val = TRUE;
  }

  DS_3GPP_MSG1_MED("ds_3gpp_is_context_pref_pkt_mode: Exiting with value %d",
                   ret_val);

  return ret_val;
} /* ds_3gpp_is_context_pref_pkt_mode */

/*===========================================================================
  FUNCTION DS_3GPP_GET_APN_CURRENT_MODE
  
  DESCRIPTION
    This function extracts the APN from current PDN context and queries
    DSD/MAPCON to get the current mode for the APN
    
  PARAMETERS  
    pdn_context_p     :           Current pdn context
      
  DEPENDENCIES 
    None.
    
  RETURN VALUE
  Current(preferred)sys sys mode
     
  SIDE EFFECTS 
    None.
    
===========================================================================*/
sys_sys_mode_e_type ds_3gpp_get_apn_current_mode
(
  ds_pdn_context_s       * pdn_context_p
)
{
  ds_pkt_sys_mode_e_type      mode = DS_PKT_SYS_MODE_NONE;
  byte                        *apn_ptr = NULL;
  ds3g_apn_name_type          apn_name;
  sys_modem_as_id_e_type      subs_id=SYS_MODEM_AS_ID_NONE;
/*-------------------------------------------------------------------------*/
  DS_3GPP_MSG0_MED("ds_3gpp_get_apn_current_mode: Entered");

  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id))
  {
    return SYS_SYS_MODE_NO_SRV;
  }
  if(SYS_SYS_MODE_LTE == ds3g_get_target_rat_ex(subs_id))
  {
    DS_3GPP_MSG0_LOW("ds_3gpp_get_apn_current_mode: LTE attach in progress");
    return SYS_SYS_MODE_LTE;  
  }

  memset(&apn_name,0,sizeof(ds3g_apn_name_type));

  if(ds_3gpp_get_apn_from_pdn_context(pdn_context_p,&apn_name))
  {
    if((apn_name.apn_string_len) == 0)
    {
      if(ds_pdn_cntxt_get_resolved_apn_name(pdn_context_p,&(apn_ptr)) == 
         TRUE)
      {
        (void)strlcpy(apn_name.apn_string,(const char *)apn_ptr,
                      DS_UMTS_MAX_APN_STRING_LEN+1);
        apn_name.apn_string_len = strlen((const char *)apn_name.apn_string);
      }
    }

    mode = ds3g_get_apn_pref_pkt_mode_ex(&apn_name, subs_id);
    
    DS_3GPP_MSG1_MED("ds_3gpp_get_apn_current_mode: Current preferred DS Mode is "
                     "%d",mode);
  }

  return ds3g_pkt_sys_mode_to_sys_sys_mode(mode);
}

/*===========================================================================
FUNCTION      DS_3GPP_GET_APN_FROM_PDN_CONTEXT

DESCRIPTION   Fills the apn name in the pointer passed and length for 
              a given PDN Context
 
PARAMETERS    pdn_context_p 
              apn_name  
 
DEPENDENCIES  None.

RETURN VALUE  TRUE: if APN was successfully extracted from PDN context 
              FALSE: if PDN context or its contents were not valid 

SIDE EFFECTS  None.
===========================================================================*/

boolean ds_3gpp_get_apn_from_pdn_context
(
  ds_pdn_context_s            * pdn_context_p,
  ds3g_apn_name_type          * apn_name
)
{

  
  DS_3GPP_MSG0_MED("ds_3gpp_get_apn_from_pdn_context: Entered");

  if(apn_name == NULL)
  {
    DS_3GPP_MSG0_HIGH("ds_3gpp_get_apn_from_pdn_context: APN data structure"
                      "is NULL");
    return FALSE;
  }

  if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p) == FALSE)
  {
    return FALSE;
  }
  
  /*-------------------------------------------------------------------------
    pdn_context stores only apn string, we need to explicitly extract
    the apn length using strlen function, since ds3gutil function uses a
    data-type which has both apn length and string (ds3g_apn_name_type)
   -------------------------------------------------------------------------*/

  (void)strlcpy(apn_name->apn_string,(const char *)pdn_context_p->
                ds_pdn_context_dyn_p->pdp_profile.context.apn,
                DS_UMTS_MAX_APN_STRING_LEN+1);
  apn_name->apn_string_len = strlen((const char *)apn_name->apn_string);
  return TRUE;
}


/*===========================================================================
FUNCTION  DS_3GPP_REPORT_THROUGHPUT_INFO_FOR_IFACE

DESCRIPTION 
 For the given iface, this utility function reports the iface
 throughput stats

PARAMETERS 
  
  ps_iface_type*                - Pointer to the PS iface type
 
  ps_sys_throughput_info_type*  - Pointer to fill in the iface throughput
                                  stats
 
DEPENDENCIES 
  This utility function will be used only when
  * Iface is active
        &
  * Throughput Info Indication timer is running

RETURN VALUE
  TRUE   - If the throughput stats for the incoming iface is reported
           successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_throughput_info_for_iface
(
  ps_iface_type                 *ps_iface_p,
  ps_sys_throughput_info_type   *iface_info_list_p
)
{
  boolean                                   ret_val               = FALSE;
  ds_3gpp_iface_s                          *ds_iface_p            = NULL;
  ps_sys_bearer_throughput_info_list_type  *bearer_info_list_p    = NULL;
  ds_pdn_context_s                         *pdn_cntxt_p           = NULL;
  ps_sys_ip_addr_enum_type                  iface_ip_addr         = DS_SYS_IP_TYPE_V4;
  byte                                     *apn_p                 = NULL;
  sys_modem_as_id_e_type                    subscription_id       = SYS_MODEM_AS_ID_NONE;
  uint32                                    actual_ul_rate        = 0;
  uint32                                    ul_queue_size         = 0;
  uint8                                     index                 = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Input arguments
    -------------------------------------------------------------------------*/
  if (ps_iface_p == NULL)
  {
    DS_3GPP_MSG0_ERROR("Not a valid iface ptr. Cannot fill throughput report");
    return ret_val;
  }

  if (iface_info_list_p == NULL)
  {
    DS_3GPP_MSG1_ERROR("NULL Iface Info List ptr. Cannot fill throughput report for "
                     "iface 0x%x",ps_iface_p);
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Get the PDN context from PS iface
    -------------------------------------------------------------------------*/
  ds_iface_p = (ds_3gpp_iface_s *)ps_iface_p->client_data_ptr;
  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG1_ERROR("Not a valid DS iface for PS iface 0x%x. Cannot fill"
                       "throughput report", ps_iface_p);
    return ret_val;
  }

  pdn_cntxt_p = (ds_pdn_context_s*)ds_iface_p->client_data_ptr;

  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
  {
    return ret_val;
  }

  /*------------------------------------------------------------------------- 
    Valid PDN context. Fill APN name from PDN context
    -------------------------------------------------------------------------*/
  if (TRUE == ds_pdn_cntxt_get_resolved_apn_name(pdn_cntxt_p,&apn_p))
  {
    (void)strlcpy(iface_info_list_p->apn_string,
                  (const char *)apn_p,
                  DS_UMTS_MAX_APN_STRING_LEN+1);
  }
  else
  {
    (void)strlcpy(iface_info_list_p->apn_string,
                  (const char *)pdn_cntxt_p->ds_pdn_context_dyn_p->
                                pdp_profile.context.apn,
                  DS_UMTS_MAX_APN_STRING_LEN+1);
  }
  /*------------------------------------------------------------------------- 
    Fill Iface and address family
    -------------------------------------------------------------------------*/
  iface_ip_addr = ((ps_iface_addr_family_is_v6(ps_iface_p))?DS_SYS_IP_TYPE_V6:
                                                           DS_SYS_IP_TYPE_V4);

  iface_info_list_p->iface_ptr  =  ps_iface_p;
  iface_info_list_p->ip_type    =  iface_ip_addr;
  /*------------------------------------------------------------------------- 
    TECH TYPE
    -------------------------------------------------------------------------*/
  iface_info_list_p->tech_type  =  DS_SYS_TECH_3GPP;

  /*------------------------------------------------------------------------- 
    SUBSCRIPTION ID
    -------------------------------------------------------------------------*/
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_TRIPLE_SIM)
  subscription_id                 = pdn_cntxt_p->ds_pdn_context_dyn_p->subs_id;
#else
  subscription_id                 = ds3g_get_ps_subs_id();
#endif
  iface_info_list_p->subscription = ds3gsubsmgr_subs_id_cm_to_ds(subscription_id);
  /*------------------------------------------------------------------------- 
    BEARER INFO LIST
    -------------------------------------------------------------------------*/
  bearer_info_list_p              = &iface_info_list_p->bearer_throughput_info_list;
  
  if (TRUE == ds_3gpp_report_bearer_throughput_stats_for_pdn(pdn_cntxt_p,
                                                             bearer_info_list_p))
  {
    /*------------------------------------------------------------------------- 
      Calculate the aggregate actual throughput and queue cnt size
      -------------------------------------------------------------------------*/
    for (index = 0; index < bearer_info_list_p->num_bearers;index++) 
    {
      actual_ul_rate += bearer_info_list_p->
                        bearer_trpt_info[index].bearer_uplink_actual_rate;
      ul_queue_size  += bearer_info_list_p->
                        bearer_trpt_info[index].bearer_uplink_queue_size; 
    }
  }
  /*------------------------------------------------------------------------- 
    AGGREGATE ACTUAL THROUGHPUT/QUEUE SIZE
    -------------------------------------------------------------------------*/
  iface_info_list_p->uplink_actual_rate = actual_ul_rate;
  iface_info_list_p->uplink_queue_size  = ul_queue_size;

  ret_val = TRUE;

  return ret_val;
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SEARCH_AND_UPDATE_PDN_DISCONN_IN_PROGRESS_LIST

DESCRIPTION 
  This function checks, if the incoming 'pdn_cntxt_tbl_index' is already
  available in 'pdn disconn list' and adds the incoming tbl index
  to the list.

PARAMETERS 
  
  pdn_cntxt_tbl_index   - PDN tbl index in pdn_context_tbl that needs to be
                          checked, if that PDN bring down is in progress
 
  pdn_disconn_list      - Pointer to the temporary list holding the list of
                          PDN table index's for which PDN disconnect is
                          already issued. 
                             
DEPENDENCIES 
  * Used during a prioritized PDN bring up where bearer mgmt algorithm is
    applied.
 
  * sizeof(pdn_disconn_list) = DS_3GPP_MAX_PDN_CONTEXT

RETURN VALUE
  FALSE   - If the search returned failure. Update the list with the
            incoming value for subsequent searches
  TRUE    - If the search found a matching PDN tbl index in the
            'pdn_tbl_index_list'
            
SIDE EFFECTS
  If sizeof(pdn_disconn_list) < DS_3GPP_MAX_PDN_CONTEXT then the incoming
  pdn_instance might not be stored. 

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_search_and_update_pdn_disconn_in_progress_list
(
 int  pdn_cntxt_tbl_index,
 int *pdn_disconn_list
)
{
  uint8       index                       = DS_3GPP_MAX_PDN_CONTEXT;
  boolean     ret_val                     = FALSE;
  int         free_index                  = -1;
  int        *local_pdn_disconn_list    = pdn_disconn_list;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------- 
    Return if PDN instance is not valid
    ---------------------------------------------------------------*/
  if ((pdn_cntxt_tbl_index < 0)||
      (pdn_cntxt_tbl_index >= DS_3GPP_MAX_PDN_CONTEXT)) 
  {
      DS_3GPP_MSG0_MED("Invalid PDN context tbl index");
      return ret_val;
  }

  /*----------------------------------------------------------------------- 
    pdn_instance_list is of size DS_3GPP_MAX_PDN_CONTEXT.Loop through the
    entire list to see, if this same instance is already cached for
    teardown.Return TRUE if this instance is already cached.
    -----------------------------------------------------------------------*/
  for (index =0;index < DS_3GPP_MAX_PDN_CONTEXT;index++)
  {
    
    if (local_pdn_disconn_list!=NULL)
    {
      if (*local_pdn_disconn_list == pdn_cntxt_tbl_index) 
      {
         DS_3GPP_MSG1_MED("Matching entry found at Index:%d",index);
         break;
      }
      else if ((*local_pdn_disconn_list < 0)&&(free_index < 0)) 
      {
         free_index = index;
      }
      else
      {
        /*--------------------------------------------------------------- 
          Some other valid PDN cntxt index in PDN instance list
          ---------------------------------------------------------------*/
      }
      local_pdn_disconn_list++;
    }
  }
  if (index >= DS_3GPP_MAX_PDN_CONTEXT) 
  {
    /*--------------------------------------------------------------------- 
      No matching entry found 
      ---------------------------------------------------------------------*/
    DS_3GPP_MSG1_MED("PDN disconnect not issued for index %d yet",
                      pdn_cntxt_tbl_index);

    if (free_index < 0)
    {
      /*------------------------------------------------------------------- 
        if sizeof(pdn_tbl_index_list) < DS_3GPP_MAX_PDN_CONTEXT
        this case might happen. 
        -------------------------------------------------------------------*/
      DS_3GPP_MSG1_HIGH("pdn disconn full. Cannot store PDN index %d",index);
    }
    else
    {
      /*------------------------------------------------------------------- 
        Update the PDN tbl index list
        -------------------------------------------------------------------*/
      pdn_disconn_list+= free_index;
      *pdn_disconn_list = pdn_cntxt_tbl_index;
      DS_3GPP_MSG1_MED("Adding PDN cntxt index %d to PDN disconn list",
                                          pdn_cntxt_tbl_index);
    }
      
  }
  else
  {
    DS_3GPP_MSG1_HIGH("PDN disconnect for index %d already in progress",
                      pdn_cntxt_tbl_index);
    ret_val = TRUE;
  }

  return ret_val;
}
/*=========================================================================== 
 
FUNCTION : DS_3GPP_PDN_CONTEXT_RETRY_IFACE_BRING_UP
 
DESCRIPTION:                                                                                        
    This function calls the iface up command handler to retry iface bring up
    
PARAMETERS   
  None 

DEPENDENCIES  
  The PDN context state should be in 'AWAITING_BEARER_ALLOC' state
  to retry iface bring up

RETURN VALUE  
  None 
 
===========================================================================*/

void ds_3gpp_pdn_context_retry_iface_bring_up
(
  sys_modem_as_id_e_type subs_id
)
{
  ps_iface_type             *ps_v4_iface_p       = NULL;
  ps_iface_type             *ps_v6_iface_p       = NULL;
  ds_pdn_context_s          *pdn_cntxt_p         = NULL;
  uint8                      num_free_bearers    = DS_3GPP_MAX_BEARER_CONTEXT;
  network_params_info_type  *info                = NULL;
  uint8                      index               = 0;
  ps_iface_net_down_reason_type down_reason      = PS_NET_DOWN_REASON_NOT_SPECIFIED;
  ds_3gpp_iface_s           *ds3gpp_v4_iface_p   = NULL;
  ds_3gpp_iface_s           *ds3gpp_v6_iface_p   = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------- 
    Check if Emergency Iface is waiting on Bearer allocation. If so, Emergency
    PDN takes the priority
    --------------------------------------------------------------------------*/
  if ((pdn_cntxt_p = ds_3gpp_get_emerg_pdn_cntxt())!= NULL)
  {
    if ((ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))&&
        (DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC(pdn_cntxt_p, subs_id)))
    {
      /*----------------------------------------------------------------- 
        Get number of available free bearers in bearer resource pool
        -----------------------------------------------------------------*/
      num_free_bearers  = ds_3gpp_bearer_cntxt_get_num_free_bearers();
      if(num_free_bearers < DS_3GPP_MIN_BEARER_CONTEXT_EMERG)
      {
        /*----------------------------------------------------------------- 
          'Return', even if there are other (non emerg) PDN's awaiting
          Bearer resources as emergency iface bring up holds the priority
          -----------------------------------------------------------------*/
        DS_3GPP_MSG2_HIGH("Need Min %d Bearers to retry emerg iface up."
                          " Available %d.",DS_3GPP_MIN_BEARER_CONTEXT_EMERG,
                          num_free_bearers);
        return;
      }
      else
      {
        ds3gpp_v4_iface_p = pdn_cntxt_p->ds_pdn_context_dyn_p->
                       ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

        ds3gpp_v6_iface_p = pdn_cntxt_p->ds_pdn_context_dyn_p->
                       ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

        if (ds3gpp_v4_iface_p != NULL)
        {
          ps_v4_iface_p = &(ds3gpp_v4_iface_p->ps_iface);
        }
        if (ds3gpp_v6_iface_p != NULL)
        {
        
          ps_v6_iface_p = &(ds3gpp_v6_iface_p->ps_iface);
        }

        info =  &(pdn_cntxt_p->ds_pdn_context_dyn_p->info);

        /*------------------------------------------------------------------ 
          Check if the iface went down while the PDN context is in
          'AWATIGIN BEARER ALLOC' state
          ------------------------------------------------------------------*/
        if (!(PS_IFACE_IS_IN_USE(ps_v4_iface_p)||PS_IFACE_IS_IN_USE(ps_v6_iface_p)))
        {
          down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
          /*------------------------------------------------------------------- 
            If both the ifaces are available but not in use, it may be
            enough to call clean up PDN context once.
            -------------------------------------------------------------------*/
          if (ps_v4_iface_p != NULL)
          {
            ds_3gpp_pdn_context_cleanup_pdn(ps_v4_iface_p, down_reason);
          }
          else if (ps_v6_iface_p != NULL)
          {
            ds_3gpp_pdn_context_cleanup_pdn(ps_v6_iface_p, down_reason);
          }
          else
          {
            /* Do nothing*/
          }
        }
        else
        {
          /*------------------------------------------------------------------- 
            Bring up emergency ifaces
            -------------------------------------------------------------------*/
          if((ps_v4_iface_p != NULL)&&PS_IFACE_IS_IN_USE(ps_v4_iface_p))
          {
            ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr(ps_v4_iface_p,info);
          }
          if((ps_v6_iface_p != NULL)&&PS_IFACE_IS_IN_USE(ps_v6_iface_p))
          {
            ds_pdn_cntxt_emergency_pdn_ifaceUpCmdHdlr(ps_v6_iface_p,info);
          }
          /*----------------------------------------------------------------- 
            'Return', even if there are other (non emerg) PDN's awaiting
            Bearer resources as emergency iface bring up holds the priority
            -----------------------------------------------------------------*/
          return;
        }
      }
    }/*if DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC*/
    else
    {
        DS_3GPP_MSG0_HIGH("No Emerg PDN found (or) Emerg PDN not in AWAITING_BEARER_ALLOC STATE");
    }
  }/*(pdn_cntxt_p = ds_3gpp_get_emerg_pdn_cntxt())!= NULL*/
  /*--------------------------------------------------- 
    No Emergency PDN in AWAITING BEARER ALLOC STATE
    Locate other PDN cntxt in AWAITING_BEARER_ALLOC
    and attempt to bring up iface for that PDN
    ---------------------------------------------------*/
  for (index =0; index < DS_3GPP_MAX_PDN_CONTEXT;index++)
  {
    pdn_cntxt_p = &ds_pdn_context_tbl[index];
    /*--------------------------------------------- 
      Validate PDN context
      ---------------------------------------------*/
    if(!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p))
    {
      continue;
    }
    if (!DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC(pdn_cntxt_p, subs_id))
    {
      continue;
    }
    /*----------------------------------------------------------------- 
      Get number of available free bearers in bearer resource pool
      -----------------------------------------------------------------*/
    num_free_bearers  = ds_3gpp_bearer_cntxt_get_num_free_bearers();

    if(num_free_bearers < DS_3GPP_MIN_BEARER_CONTEXT)
    {
      DS_3GPP_MSG2_HIGH("Need Min %d Bearers to Retry. Available now %d."
                        ,DS_3GPP_MIN_BEARER_CONTEXT,
                        num_free_bearers);
    }
    else
    {
      ds3gpp_v4_iface_p = pdn_cntxt_p->ds_pdn_context_dyn_p->
			    ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV4_IFACE];

      ds3gpp_v6_iface_p = pdn_cntxt_p->ds_pdn_context_dyn_p->
                            ds_3gpp_iface_tbl[DS_3GPP_PDN_CONTEXT_IPV6_IFACE];

      if (ds3gpp_v4_iface_p != NULL)
      {
        ps_v4_iface_p = &(ds3gpp_v4_iface_p->ps_iface);
      }
      if (ds3gpp_v6_iface_p != NULL)
      {
        ps_v6_iface_p = &(ds3gpp_v6_iface_p->ps_iface);
      }

      info =  &(pdn_cntxt_p->ds_pdn_context_dyn_p->info);
      /*------------------------------------------------------------------ 
        Check if the iface went down while the PDN context is in
        'AWAITING BEARER ALLOC' state
        ------------------------------------------------------------------*/
      if (!(PS_IFACE_IS_IN_USE(ps_v4_iface_p)||PS_IFACE_IS_IN_USE(ps_v6_iface_p)))
      {
        down_reason = PS_NET_DOWN_REASON_INTERNAL_ERROR;
        /*------------------------------------------------------------------- 
          If both the ifaces are available but not in use, it may be
          enough to call clean up PDN context once.
          -------------------------------------------------------------------*/
        if (ps_v4_iface_p != NULL)
        {
          ds_3gpp_pdn_context_cleanup_pdn(ps_v4_iface_p, down_reason);
        }
        else if (ps_v6_iface_p != NULL)
        {
          ds_3gpp_pdn_context_cleanup_pdn(ps_v6_iface_p, down_reason);
        }
        else
        {
          /* Do nothing*/
        }
      }
      else
      {
        /*------------------------------------------------------------------- 
          Bring up ifaces
          -------------------------------------------------------------------*/
        if((ps_v4_iface_p != NULL)&&PS_IFACE_IS_IN_USE(ps_v4_iface_p))
        {
          ds_pdn_cntxt_ifaceUpCmdHdlr(ps_v4_iface_p,info);
        }
        if((ps_v6_iface_p != NULL)&&PS_IFACE_IS_IN_USE(ps_v6_iface_p))
        {
          ds_pdn_cntxt_ifaceUpCmdHdlr(ps_v6_iface_p,info);
        }
      }
    }
    break;
  }/*for*/
}
/*===========================================================================
FUNCTION DS_3GPP_GET_EMERG_PDN_CNTXT

DESCRIPTION 
    This function loops through the PDN context table and
    returns the emergency PDN context, if found
 
PARAMETERS
    None
DEPENDENCIES
    None.

RETURN VALUE
    ds_pdn_context_s* - Pointer to the emergency PDN context
    NULL              - If no Emergency PDN context found 
   
SIDE EFFECTS 
  None.
===========================================================================*/

ds_pdn_context_s* ds_3gpp_get_emerg_pdn_cntxt()
{
  uint8             index               = 0;
  ds_pdn_context_s* pdn_cntxt_p         = NULL;
  /*------------------------------------------------------------------*/

  for(index =0; index < DS_3GPP_MAX_PDN_CONTEXT; index++)
  {
     if(ds_3gpp_pdn_cntx_validate_pdn_context_internal(&ds_pdn_context_tbl[index]) &&
        DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES((&ds_pdn_context_tbl[index])))
     {
       pdn_cntxt_p = &ds_pdn_context_tbl[index];
       DS_3GPP_MSG1_LOW("Emerg PDN cntxt 0x%x",pdn_cntxt_p);
       break;
     }
  }
  return pdn_cntxt_p;
}
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_NUM_VALID_PDN_CONTEXTS

DESCRIPTION 
  This function, returns the number of
  valid PDN contexts for which bearer resoures are already allocated.
 
  p.s: This may not necessarily return the number of active PDN connection
  when there are one or more PDN contexts in coming up (or) PDN
  teardown is in progress.
  However in such cases
 
  num_valid_pdn's >= num_active_pdn_contexts
 
  This is currently being used while applying Bearer mgmt algorithm
PARAMETERS
    None
DEPENDENCIES
    None.

RETURN VALUE
  integer 0-255
   
SIDE EFFECTS 
  None.
===========================================================================*/

int ds_3gpp_pdn_cntxt_get_num_valid_pdn_contexts
(
  sys_modem_as_id_e_type subs_id
)
{
  int                    num_pdn_cntxts     = 0;
  ds_pdn_context_s        *pdn_cntxt_p      = NULL;
  uint8                    index            = 0;
  /*------------------------------------------------------------------*/
  for (index = 0; index < DS_3GPP_MAX_PDN_CONTEXT;index++)
  {
    pdn_cntxt_p = &ds_pdn_context_tbl[index];
    if(ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p))
    {
      if ((pdn_cntxt_p->ds_pdn_context_dyn_p->state!=DS_PDN_CONTEXT_STATE_DOWN)
          &&
          (!DS_3GPP_IS_PDN_CONTEXT_STATE_AWAITING_BEARER_ALLOC(pdn_cntxt_p, subs_id)))
      {
        num_pdn_cntxts++;
      }
    }
  }
  return num_pdn_cntxts;
}
/*===========================================================================
FUNCTION  DS_3GPP_REPORT_ALL_IFACE_THROUGHPUT_INFO

DESCRIPTION 
 This Utility function reports the the throughput Information for all
 active DS 3GPP ifaces

PARAMETERS 
  sys_modem_as_id_e_type            - subscription ID to report per iface
                                      bearer stats
  ps_sys_throughput_info_list_type  - Pointer to the iface information
                                      list to fill in iface throughput
                                      related stats
  uint16                            - maximum number if ds ifaces that
                                      will be reported in the list

DEPENDENCIES 
  This utility function will be used only when
  * there is atleast one active DS 3GPP Iface
        &
  * Throughput Info Indication timer is running

RETURN VALUE
  TRUE   - If the throughput Info for the iface is reported successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds_3gpp_report_all_iface_throughput_info
(
   sys_modem_as_id_e_type             cm_subs_id,
   ps_sys_throughput_info_list_type   *report_p,
   uint16                             num_max_ifaces
)
{
  uint8                             index                           = 0;
  boolean                           ret_val                         = FALSE;
  ds_3gpp_iface_s                  *ds_iface_p                      = NULL;
  ds_3gpp_throughput_quality_s      throughput_quality;
  boolean                           tput_quality_status             = FALSE;
  ds_pdn_context_s                 *pdn_context_p                   = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate Arguments
    -------------------------------------------------------------------------*/
  if ((report_p == NULL)|| (num_max_ifaces ==0))
  {
    DS_3GPP_MSG0_ERROR("No memory available to report throughput list for active"
                       "DS 3GPP ifaces");
    return ret_val;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(cm_subs_id))
  {
    return ret_val;
  }

  memset(&throughput_quality, 0, sizeof(ds_3gpp_throughput_quality_s));
  /*--------------------------------------------------------------------------- 
    Initialize the number of elements in the List to 0
    ---------------------------------------------------------------------------*/
  report_p->num_trpt_info = 0;
  DS_PDN_CTXT_VFR_CALL
  (
     tput_quality_status,
     report_ul_throughput_quality,
     cm_subs_id,
     cm_subs_id,
     &throughput_quality
  ); 
  /* To get around Compiler Warning*/
  DS_3GPP_MSG1_LOW("Throughput quality acquired? %d", tput_quality_status);

  for (index = 0; index < DS_3GPP_MAX_PS_IFACES;index++)
  {
    if (report_p->num_trpt_info >= num_max_ifaces)
    {
      DS_3GPP_MSG1_HIGH("Filled max %d iface t'put stats", num_max_ifaces);
      break;
    }
    /*----------------------------------------------------------------------- 
      Get PDN context from DS iface Tbl
      -----------------------------------------------------------------------*/
    ds_iface_p = &ds_iface_tbl[index];
    pdn_context_p = ds_iface_p->client_data_ptr;
    if (!ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_context_p,
                                                         cm_subs_id))
    {
      continue;
    }

    /*----------------------------------------------------------------------- 
      Valid DS iface. Report Throughput Info for this iface
      -----------------------------------------------------------------------*/
    if (FALSE == ds_3gpp_report_throughput_info_for_iface(&ds_iface_p->ps_iface,
                                                 &report_p->trpt_info[report_p->num_trpt_info]))
    {
      continue;
    }

    /*-----------------------------------------------------------------------
      Report generated for the current iface. Move the index to to next
      location
      -----------------------------------------------------------------------*/
    report_p->trpt_info[report_p->num_trpt_info].
                uplink_allowed_rate = (uint32)throughput_quality.allowed_ul_rate;
    report_p->trpt_info[report_p->num_trpt_info].
                throughput_quality  = throughput_quality.ul_throughput_quality;

    report_p->num_trpt_info++;
  }
  ret_val = TRUE;
  return ret_val;
}
/*===========================================================================
FUNCTION DS_3GPP_GET_PDN_DISCONN_LIST_FRM_APN_DISCONN_ORDER_LIST

DESCRIPTION
  This routine gets PDN contexts associated with APN identifiers in
  APN disconnect order list and updates the input 
  'pdn_disconn_list' one by one in order they match  

PARAMETERS
  pdn_disconn_list  - List holding the PDN tbl index for which PDN
                      connection is already issued

DEPENDENCIES
  APN disconnect order list should not be empty else
  the 'pdn_disconn_list' isnt updated

RETURN VALUE
  None
=============================================================================*/
void ds_3gpp_get_pdn_disconn_list_frm_apn_disonn_order_list
(
  int*  pdn_disconn_list,
  sys_modem_as_id_e_type subs_id
)
{
  uint8                                pdn_cntx_tbl_idx         = 0;
  uint8                                list_idx                 = 0;
  ds_pdn_context_s                    *pdn_cntx_p               = NULL;
  ds_profile_3gpp_param_enum_type      profile_identifier       = 
                                        DS_PROFILE_3GPP_PROFILE_PARAM_INVALID;
  ds_umts_pdp_profile_type            *profile_p                = NULL;
  void                                *apn_identifier           = NULL;
  ds_3gpp_apn_disconn_order_list_type *apn_disconn_order_list_p = NULL;
  byte                                *resolved_apn             = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    pdn_disconn_list is an array of size 'DS_3GPP_MAX_PDN_CONTEXT'
    -------------------------------------------------------------------------*/
  if (pdn_disconn_list == NULL)
  {
    DS_3GPP_MSG0_MED("No Mem Available to cache PDN's to disconnect");
    return;
  }
  /*------------------------------------------------------------------------- 
    Loop through the APN disconnect order list. Find all the PDN contexts
    matching the current APN identifier.
    -------------------------------------------------------------------------*/
  for (list_idx =0; list_idx < DS_3GPP_MAX_APN_IDENTIFIERS;list_idx++) 
  {
    apn_disconn_order_list_p = (ds_3gpp_apn_disconn_order_list_type*)
                  ds_3gppi_utils_get_apn_disconn_order_list_info(list_idx,subs_id);
    if (apn_disconn_order_list_p == NULL)
    {
      DS_3GPP_MSG0_MED("End of APN disconn list");
      break;
    }
    /*----------------------------------------------------------------------- 
        Get the Profile Identifiers and APN identifiers from the 'apn
        disconnect order list' at 'list_idx'„£
      -----------------------------------------------------------------------*/

    profile_identifier  = apn_disconn_order_list_p->profile_parm;
    apn_identifier      = apn_disconn_order_list_p->identifier;
    /*-----------------------------------------------------------------------
      Currently, we support having only 'APN name' as identifier. However,
      this can be expanded for other profile identifier
      -----------------------------------------------------------------------*/
    if (profile_identifier!= DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN)
    {
      DS_3GPP_MSG1_MED("prof parm %d on apn_disconn_order list not supported",
                       profile_identifier);
      continue;
    }
    /*------------------------------------------------------------------------- 
      If a valid APN identifier is found in the APN disconnect order list
      -------------------------------------------------------------------------*/
    if (apn_identifier != NULL)
    {
      DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"Prof identifier:%d APN identifier"
                              " %s",profile_identifier,apn_identifier);
      for (pdn_cntx_tbl_idx=0;pdn_cntx_tbl_idx<DS_3GPP_MAX_PDN_CONTEXT;
                                                            pdn_cntx_tbl_idx++)
      {
        /*------------------------------------------------------------------- 
          Check if we have memory to store the next matching PDN context
          -------------------------------------------------------------------*/
        if (pdn_disconn_list == NULL)
        {
          DS_3GPP_MSG0_MED("pdn disconn list full. cannot store next PDN context match");
          break;
        }
        /*------------------------------------------------------------------- 
          Check if the PDN context is valid
          -------------------------------------------------------------------*/
        pdn_cntx_p  = &ds_pdn_context_tbl[pdn_cntx_tbl_idx];
        if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntx_p))
        {
          continue;
        }
        /*------------------------------------------------------------------- 
          Get the profile context from PDN context, check if there is a
          match on APN identifier depending on the profile parameter type
          -------------------------------------------------------------------*/
        profile_p = &(pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile);
        if (profile_identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN)
        {
          (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_cntx_p,&(resolved_apn));
          /*----------------------------------------------------------------- 
            Check if there is a match in APN name, in the profile
                              (or)
            in the resolved APN name
            -----------------------------------------------------------------*/
          if((strncasecmp((char*)apn_identifier,(char*)profile_p->context.apn,
                         DS_UMTS_MAX_APN_STRING_LEN+1)== 0)
                                                        ||
             ((resolved_apn != NULL) &&
              (strncasecmp((char*)apn_identifier,(char*)resolved_apn,
                                DS_UMTS_MAX_APN_STRING_LEN+1)==0))
            )
          {
            /*---------------------------------------------------------------
              Redundancy Check for APN identifiers are done while filling in
              the 'apn disconnect order list'. so we will not duplicate
              entries while generating 'pdn_disconn_list'
              ---------------------------------------------------------------*/
            *pdn_disconn_list = pdn_cntx_tbl_idx;
            pdn_disconn_list++;
            DATA_3GPP_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"PDN Cntxt 0x%x found for "
                                   "APN ident %s",pdn_cntx_p,apn_identifier);
          }
        }
      }/* end for (pdn_cntx_tbl_idx)*/
    }/*apn_identifier != NULL*/
  }/* end for (list_idx)*/
}

/*===========================================================================
FUNCTION      DS_3GPP_GET_NUM_ACTIVE_DS_IFACES_ON_SUBS_ID

DESCRIPTION 
    This function returns the total number of ds3gpp ifaces that 
    are currently in use on the incoming subscription ID

PARAMETERS 
  sys_modem_as_id_e_type  -   subscription Id to check number of ifaces
                              active on that subscription
 
RETURN VALUE 
  uint8                   -   Number of ds3gpp DS ifaces in use
 
DEPENDENCIES 
  None
 
SIDE EFFECTS 
  None
===========================================================================*/
uint8 ds3gpp_get_num_active_ds_ifaces_on_subs_id
(
  sys_modem_as_id_e_type  subs_id
)
{
  uint8                      num_ifaces       = 0;
  uint8                      index            = 0;
  ds_3gpp_iface_s            *ds_iface_p      = NULL;
  ds_pdn_context_s           *pdn_context_p   = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return num_ifaces;
  }

  for (index = 0; index < DS_3GPP_MAX_PS_IFACES;index++)
  {
    ds_iface_p = &ds_iface_tbl[index];
    /*----------------------------------------------------------------------- 
      Consider Iface as valid is there is a PDN context associated with the
      DS iface
      -----------------------------------------------------------------------*/
    pdn_context_p = ds_iface_p->client_data_ptr;
    if (!ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_context_p, subs_id))
    {
      continue;
    }
    num_ifaces++;
  }
  
  if (num_ifaces > DS_3GPP_MAX_PS_IFACES)
  {
    num_ifaces = DS_3GPP_MAX_PS_IFACES;
  }
  return num_ifaces;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_compare_pdp_address

DESCRIPTION
  This function compares the pdp_address passed by network with the address
  set on the PDN during call origination.
 
PARAMETERS
  identifier: for W/G/TD its nsapi, for LTE its eps_bid
  sys_mode: current sys_mode.
  subs_id:  subscription id of the call.
  pdp_address * : pointer to the pdp_address struct passed by nas

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE if address is same.
  FALSE if address is different.

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
)
{
  ps_iface_type  *v4_iface_ptr = NULL;
  ps_iface_type  *v6_iface_ptr = NULL;
  ps_ip_addr_type v4_ip_addr;
  ps_ip_addr_type v6_ip_addr;
  boolean         v4_addr_compare = FALSE;
  boolean         v6_addr_compare = FALSE;

  //For Debug
  uint32          received_v4_address;
  uint64          received_v6_address[2];

  DS_3GPP_MSG3_HIGH("ds_3gpp_pdn_cntxt_compare_pdp_address", 0,0,0);

  memset(&v4_ip_addr, 0, sizeof(ps_ip_addr_type));  
  memset(&v6_ip_addr, 0, sizeof(ps_ip_addr_type)); 

  if ( DS_3GPP_IS_3GPP_CURRENT_MODE(sys_mode) &&
       (SYS_SYS_MODE_LTE !=  sys_mode) )
  {
    v4_iface_ptr = ds_3gpp_bearer_cntxt_get_v4_iface_ptr_from_nsapi(identifier,
                                                                    subs_id);
    v6_iface_ptr = ds_3gpp_bearer_cntxt_get_v6_iface_ptr_from_nsapi(identifier,
                                                                    subs_id);

    if (PS_IFACE_IS_VALID(v4_iface_ptr))
    {
      v4_ip_addr.type = IPV4_ADDR;
      ps_iface_get_addr(v4_iface_ptr, &v4_ip_addr);
    }

    if (PS_IFACE_IS_VALID(v6_iface_ptr))
    {
      v6_ip_addr.type = IPV6_ADDR;
      ps_iface_get_addr(v6_iface_ptr, &v6_ip_addr);
    }

    //For Debug
    DS_3GPP_MSG3_HIGH("IPV4 address set: %x IPV6 address set in host order:",
                       v4_ip_addr.addr.v4.ps_s_addr, 
                       0, 0);

    IPV6_ADDR_MSG(v6_ip_addr.addr.v6.in6_u.u6_addr64);

    switch(pdp_address->pdp_type_num)
    {
      case DS_3GPP_PDP_TYPE_IPV4_NUM:
      //For Debug
      memscpy(&received_v4_address, sizeof(uint32),
              (pdp_address->address), sizeof(uint32));

      DS_3GPP_MSG3_HIGH("Received V4 address: %x",
                         received_v4_address, 0, 0);
		
      v4_addr_compare = memcmp(&(v4_ip_addr.addr.v4.ps_s_addr),
                                (pdp_address->address),
                                sizeof(uint32)
                              );
      return (!v4_addr_compare);
  
      case DS_3GPP_PDP_TYPE_IPV6_NUM:
        //For Debug
        memscpy(received_v6_address, (2*sizeof(uint64)),
                (pdp_address->address), (2*sizeof(uint64)));

        DS_3GPP_MSG3_HIGH("Received V6 address: Host Order: ",
                           0,0,0);

        IPV6_ADDR_MSG(received_v6_address);
		
        v6_addr_compare = memcmp((v6_ip_addr.addr.v6.in6_u.u6_addr64),
                                 (pdp_address->address),
                                 (sizeof(uint64)) //Match only prefix
			                    );
        return (!v6_addr_compare);

      case DS_3GPP_PDP_TYPE_IPV4V6_NUM:
        //For Debug
        memscpy(&received_v4_address, sizeof(uint32),
                (pdp_address->address), sizeof(uint32));

        DS_3GPP_MSG3_HIGH("Received V4 address: %x",
                           received_v4_address, 0, 0);

        memscpy(received_v6_address, (2*sizeof(uint64)),
                &(pdp_address->address[4]), (2*sizeof(uint64)));

        DS_3GPP_MSG3_HIGH("Received V6 address: Host Order: ",
                           0,0,0);

        IPV6_ADDR_MSG(received_v6_address);
  
        v4_addr_compare = memcmp(&(v4_ip_addr.addr.v4.ps_s_addr),
                                  (pdp_address->address),
                                  sizeof(uint32)
                                );

        v6_addr_compare = memcmp((v6_ip_addr.addr.v6.in6_u.u6_addr64),
                                 &(pdp_address->address[4]),
                                 (sizeof(uint64)) //Match only prefix
                                );

        return ( (!v4_addr_compare) & (!v6_addr_compare) );

      default:
        DS_3GPP_MSG3_ERROR("Invalid pdp_type_num: %d", 
                            pdp_address->pdp_type_num, 0, 0);
        return FALSE;
	  	
    }
  }

  return FALSE;
  
}


/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_set_sub_id

DESCRIPTION
  This function sets the Subscription ID for the PDN which remain constant
  until the PDN is torn down

PARAMETERS
  ds_pdn_context_s           pdn_cntx_p, // PDN context
  sys_modem_as_id_e_type     subs_id     // Subscription ID

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE  = SUCCESS
  FALSE = FAILURE

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_set_sub_id
(
   ds_pdn_context_s          *pdn_cntx_p,
   sys_modem_as_id_e_type     subs_id
)
{  
  boolean ret_val = FALSE;

  if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
     DS_3GPP_MSG1_MED("Updated PDN cntxt ptr with Subs id(CM) %d ",
                      subs_id);
     pdn_cntx_p->ds_pdn_context_dyn_p->subs_id = subs_id;
     ret_val = TRUE;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("PDN cntxt ptr is NULL:0x%x",pdn_cntx_p);
  }

  return ret_val;
  }

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_sub_id

DESCRIPTION
  This function returns the Subscription ID for the PDN 

PARAMETERS
  ds_pdn_context_s           pdn_cntx_p, // PDN context

DEPENDENCIES
  None.

RETURN VALUE  
  sys_modem_as_id_e_type  

SIDE EFFECTS

===========================================================================*/
sys_modem_as_id_e_type ds_3gpp_pdn_cntxt_get_sub_id
(
   ds_pdn_context_s           * pdn_cntx_p
)
{
  sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
  if (ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntx_p))
  {
    subs_id = pdn_cntx_p->ds_pdn_context_dyn_p->subs_id;
    DS_3GPP_MSG1_MED("SUB ID %d in PDN cntxt ptr",subs_id);
    ASSERT(subs_id != SYS_MODEM_AS_ID_NONE);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("PDN cntxt ptr is NULL:0x%x",pdn_cntx_p);
    subs_id = SYS_MODEM_AS_ID_NONE;
  }
  return subs_id; 
}
/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_active_pdn

DESCRIPTION
  This function checks if any pdn context is active and UP for the apn passed

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE  
  ds_pdn_context_s * - If a PDN with the passed apn is UP/COMING_UP
  NULL -- If a PDN with the passed apn is not UP/COMING_UP

SIDE EFFECTS

===========================================================================*/
ds_pdn_context_s *ds_3gpp_pdn_cntxt_get_active_pdn
(
   char *apn
)
{
  uint8                                pdn_cntxt_index = 0;
  ds_pdn_context_s                    *pdn_cntxt_ptr = NULL;
  byte                                *resolved_apn  = NULL;
  ds_umts_pdp_profile_type            *profile_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input apn passed is NULL");
    return NULL;
  }

  for (pdn_cntxt_index=0; pdn_cntxt_index < DS_3GPP_MAX_PDN_CONTEXT; 
       pdn_cntxt_index++)
  {
    pdn_cntxt_ptr = &ds_pdn_context_tbl[pdn_cntxt_index];

    if(!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_ptr))
    {
      continue;
    }

    profile_p = &(pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile);

    if (profile_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Profile ptr not populated for index:%d",
                         pdn_cntxt_index);
      continue;
    }

    /*------------------------------------------------------------------------ 
      Check if the apn passed matches with the profile_apn name or resolved
      apn name. If a match occurs, check if the pdn is up, return TRUE.
     ------------------------------------------------------------------------*/
                              
    (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_cntxt_ptr,&(resolved_apn));

    if((strncasecmp((char*)apn,(char*)profile_p->context.apn,
                     DS_UMTS_MAX_APN_STRING_LEN+1)== 0) ||
       ((resolved_apn != NULL) && (strncasecmp((char*)apn,(char*)resolved_apn,
                     DS_UMTS_MAX_APN_STRING_LEN+1)== 0)))
    {
      DS_3GPP_MSG1_HIGH("APN match for PDN context:0x%x",pdn_cntxt_ptr);

      if ((pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_UP) || 
          (pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_COMING_UP))
      {
        DS_3GPP_MSG1_HIGH("PDN context state is %d",
        pdn_cntxt_ptr->ds_pdn_context_dyn_p->state);
        break; 
      }
    }
  }

  /*------------------------------------------------------------------------ 
    If the pdn_context_index has reached the MAX value, return NULL
  -------------------------------------------------------------------------*/
  if(pdn_cntxt_index == DS_3GPP_MAX_PDN_CONTEXT)
  {
    DS_3GPP_MSG0_MED("ds_3gpp_pdn_cntxt_is_apn_active returns NULL");
    pdn_cntxt_ptr = NULL;
  }

  return pdn_cntxt_ptr;
}


/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_local_teardown

DESCRIPTION
  This function is used to teardown calls when DDS switch or LPM Online transition occurs
 
PARAMETERS
  void
 
DEPENDENCIES
  void

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_local_teardown
(
  boolean                cleanup_all_calls,
  sys_modem_as_id_e_type subs_id
)
{
  ds_pdn_context_s        *pdn_cntxt_p = NULL;
  sys_modem_as_id_e_type   pdn_cntxt_subs_id = SYS_MODEM_AS_ID_NONE;
  ds_bearer_context_s     * bearer_context_p= NULL;
  uint32                    bearer_cntx_inst= 0;
  uint32                   ds3g_call_inst = DS3GI_INVALID_CALL_INSTANCE;  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  for( bearer_cntx_inst=0;\
       bearer_cntx_inst<DS_3GPP_MAX_BEARER_CONTEXT;\
       bearer_cntx_inst++ )
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(bearer_cntx_inst);

    if(!ds_bearer_cntx_validate_bearer_context (bearer_context_p))
    {
      continue;
    }

    pdn_cntxt_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

    if (bearer_context_p->ds_bearer_context_dyn_p->state == 
	  	    DS_BEARER_CONTEXT_STATE_GOING_DOWN)
    {
      DS_3GPP_MSG0_HIGH("Bearer in GOING_DOWN state during LPM, local_cleanup");
      ds3g_call_inst = 
	      ds3gi_find_call_instance(
			 bearer_context_p->ds_bearer_context_dyn_p->cm_call_id);
        
      bearer_context_p->ds_bearer_context_dyn_p->state =
        DS_BEARER_CONTEXT_STATE_DOWN;
      ds_bearer_cntx_local_cleanup(bearer_context_p);

      if (DS3GI_INVALID_CALL_INSTANCE != ds3g_call_inst)
      {
        ds3gi_free_call_instance_record(ds3g_call_inst);
      }      
    }

    if (cleanup_all_calls == TRUE)
    {
      ds_bearer_cntxt_phys_link_down_cmd_hdlr(bearer_cntx_inst, NULL);
    }
    else if( (cleanup_all_calls == FALSE) &&
             (pdn_cntxt_subs_id == subs_id) &&
             (bearer_context_p->ds_bearer_context_dyn_p->state == 
              DS_BEARER_CONTEXT_STATE_PEND_REDIAL || 
              bearer_context_p->ds_bearer_context_dyn_p->cm_call_id == 
              CM_CALL_ID_INVALID)  )
    {   
      DS_3GPP_MSG3_HIGH("Bearer_inst:%d, call_id:%d in state: %d"
                        ", ending call",(uint32)bearer_cntx_inst,
                        bearer_context_p->ds_bearer_context_dyn_p->cm_call_id,
                        bearer_context_p->ds_bearer_context_dyn_p->state);
  
      ds_bearer_cntxt_phys_link_down_cmd_hdlr(bearer_cntx_inst, NULL);

      /*---------------------------------------------------------------------
        If bearer was locally deactivated in DS then state will be DOWN and
        not GOING_DOWN. We do not redial calls for this local deactivation
        in DS scenario
      ---------------------------------------------------------------------*/
    }
  }
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_handle_lpm_mode

DESCRIPTION
  This function is register as call back to DDS switch and is used to clean up 
  calls

PARAMETERS
  void

DEPENDENCIES
  void

RETURN VALUE  
  void

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_handle_lpm_mode
(
  ds_3gpp_event_info_u_type event_info,
  uint32                    user_data
)
{
  uint8 subs_id = 0;
  ds_dsd_apm_ind_pay_load  dsd_apm_pay_load;
  /*----------------------------------------------------------------------*/


  if(event_info.oprt_mode == SYS_OPRT_MODE_LPM || 
      event_info.oprt_mode == SYS_OPRT_MODE_OFFLINE)
  {
    DS_3GPP_MSG0_HIGH("LPM mode transition, tearing down pending calls");
    ds_3gpp_pdn_cntxt_local_teardown(TRUE, SYS_MODEM_AS_ID_NONE);
    ds_3gpp_throt_sm_clr_srv_req_throt(event_info,user_data);

    for (subs_id=0; subs_id < DS3GSUBSMGR_SUBS_ID_MAX; subs_id++)
    {
#ifdef FEATURE_DATA_LTE
      ds_eps_tlb_free_subs_info_hndl(subs_id);    
#endif /* FEATURE_DATA_LTE */

      if(ds_3gppi_is_rel_11_supported())
      {
        DS_3GPP_MSG1_HIGH("Reset max active pdp's for subs_id in LPM:%d",
                          subs_id);
        ds_3gpp_pdn_set_max_simul_active_pdp_context(DS_3GPP_MAX_BEARER_CONTEXT, subs_id);
      }

      /*---------------------------------------------------------------------
        This takes care of clearing all the throttling structures for
        global and per_plmn_throttling
        Also network overridden throttling will be cleared
      -----------------------------------------------------------------------*/
      ds_3gpp_pdn_throt_clear_throttling_on_lpm(subs_id);
      DS_3GPP_MSG1_HIGH("Reset ntw override hdlr for subs_id in LPM:%d",
                          subs_id);
      ds_3gpp_network_override_hdlr_reset(subs_id);

      /*---------------------------------------------------------------------- 
        Send out an indication to APM to reset ATTACH PDN MGR
      -----------------------------------------------------------------------*/
      memset(&dsd_apm_pay_load, 0, sizeof(ds_dsd_apm_ind_pay_load));
      dsd_apm_pay_load.subs_id = subs_id;
      ds_dsd_apm_ind(DS_DSD_APM_RESET_ATTACH_APN_IND, &dsd_apm_pay_load);
    }
     /*---------------------------------------------------------------------------------- 
      Advertize the blocked APN's to MPPM
     ----------------------------------------------------------------------------------*/
    ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn(subs_id);
  }

  return;
} /* ds_3gpp_pdn_cntxt_handle_lpm_mode */

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal

DESCRIPTION
  This function gets the Subscription id from PDN pointer. No F3 when fails to
  get the subs id.

PARAMETERS
  pdn_ptr- PDN pointer
  Subs_id_ptr - pointer to subs_id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE-- if successful
  FALSE-- inavalid PDN pointer/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal
(
  ds_pdn_context_s* pdn_cntxt_ptr,
  sys_modem_as_id_e_type* subs_id_ptr
)
{
  if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_ptr))
  {
    return FALSE;
  }

  if(subs_id_ptr == NULL)
  {
    return FALSE;
  }
  *subs_id_ptr = pdn_cntxt_ptr->ds_pdn_context_dyn_p->subs_id;

  return TRUE;
} /*ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal*/

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_subs_id_from_pdn

DESCRIPTION
  This function gets the Subscription id from PDN pointer

PARAMETERS
  pdn_cntxt_ptr - PDN ptr
  Subs_id--susscription ID
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE- if successfull
  FALSE- invalid PDN ptr/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_subs_id_from_pdn
(
  ds_pdn_context_s* pdn_cntxt_ptr,
  sys_modem_as_id_e_type* subs_id_ptr
)
{  
  if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_ptr))
  {
    return FALSE;
  }

  if(subs_id_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Subs_id ptr is not valid");
    return FALSE;
  }
  
  *subs_id_ptr = pdn_cntxt_ptr->ds_pdn_context_dyn_p->subs_id;
  return TRUE;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_ROAMING_STATUS_CHG_CMD_HDLR

DESCRIPTION
  This function handles roaming status chg

PARAMETERS 
  roaming status
  subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_roaming_status_chg_cmd_hdlr
(
   sys_roam_status_e_type roam_status,
   sys_modem_as_id_e_type subs_id
)
{
  boolean is_roaming_on = TRUE;
 /*- - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - */
  DS_3GPP_MSG1_HIGH("Roaming status is :%d",roam_status);

  if (roam_status == SYS_ROAM_STATUS_OFF)
  {
    is_roaming_on = FALSE;
  }
  
  if (ds_3gpp_roaming_get_home_plmn_list_size(subs_id) == 0 && 
      ds_3gpp_nv_manager_get_check_ehplmn_list(subs_id) == FALSE)
  {
    ds_3gpp_roaming_set_status(subs_id, is_roaming_on);
  }

}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_get_active_pdns

DESCRIPTION
  This function checks if any pdn contexts are active and UP for the apn passed

PARAMETERS
   char *apn,
   ds_pdn_context_s  **pdn_cntxt_ptrs 
 
DEPENDENCIES
  None.

RETURN VALUE 
  void 

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_get_active_pdns
(
   char *apn,
   ds_pdn_context_s  **pdn_cntxt_ptrs
)
{
  uint8                                pdn_cntxt_index = 0;
  ds_pdn_context_s                    *pdn_cntxt_ptr = NULL;
  byte                                *resolved_apn  = NULL;
  ds_umts_pdp_profile_type            *profile_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (apn == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input apn passed is NULL");
    return;
  }


  for (pdn_cntxt_index=0; pdn_cntxt_index < DS_3GPP_MAX_PDN_CONTEXT; 
       pdn_cntxt_index++)
  {
    pdn_cntxt_ptr = &ds_pdn_context_tbl[pdn_cntxt_index];

    if(!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_ptr))
    {
      continue;
    }

    profile_p = &(pdn_cntxt_ptr->ds_pdn_context_dyn_p->pdp_profile);

    if (profile_p == NULL)
    {
      DS_3GPP_MSG1_ERROR("Profile ptr not populated for index:%d",
                         pdn_cntxt_index);
      continue;
    }

    /*------------------------------------------------------------------------ 
      Check if the apn passed matches with the profile_apn name or resolved
      apn name. If a match occurs, check if the pdn is up, return TRUE.
     ------------------------------------------------------------------------*/
                              
    (void)ds_pdn_cntxt_get_resolved_apn_name(pdn_cntxt_ptr,&(resolved_apn));

    if((strncasecmp((char*)apn,(char*)profile_p->context.apn,
                     DS_UMTS_MAX_APN_STRING_LEN+1)== 0) ||
       ((resolved_apn != NULL) && (strncasecmp((char*)apn,(char*)resolved_apn,
                     DS_UMTS_MAX_APN_STRING_LEN+1)== 0)))
    {
      DS_3GPP_MSG1_HIGH("APN match for PDN context:0x%x",pdn_cntxt_ptr);

      if ((pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_UP) || 
          (pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == 
          DS_PDN_CONTEXT_STATE_COMING_UP))
      {
        DS_3GPP_MSG1_HIGH("PDN context state is %d",
        pdn_cntxt_ptr->ds_pdn_context_dyn_p->state);
        if(pdn_cntxt_ptrs != NULL)
        {
          *pdn_cntxt_ptrs = pdn_cntxt_ptr;
          pdn_cntxt_ptrs++;
        }
        else
        {
          DS_3GPP_MSG0_ERROR("pdn_cntxt_ptrs is NULL");
        }
      }
    }
  }

  return;
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_PREPARE_FOR_REDIAL

DESCRIPTION
  This function tears down the bearer and puts the bearer and PDN context
  in an appropriate state to redial

PARAMETERS 
  bearer_cntxt_p - Bearer on which redial is needed.
  redial_cause - Why a redial needs to happen
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_prepare_for_redial
(
   void *bearer_context_ptr,
   ds_3gpp_redial_e_type redial_cause
)
{
   boolean retVal = FALSE;
   ds_3gpp_pdn_call_info_type mode_sp_info;
   ds_3gpp_down_reason_t  net_down_reason;
   ds_pdn_context_s *pdn_context_p = NULL;
   ds_bearer_context_s *bearer_context_p = NULL;
   sys_sys_mode_e_type network_mode  = SYS_SYS_MODE_NONE;
   sys_modem_as_id_e_type subs_id = SYS_MODEM_AS_ID_NONE;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   bearer_context_p = (ds_bearer_context_s *)bearer_context_ptr;
 
   if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
   {
     return;
   }

   retVal = ds_bearer_cntxt_prepare_for_teardown(&mode_sp_info,bearer_context_p);

   if (!retVal)
   {
     DS_3GPP_MSG1_ERROR("ds_bearer_cntxt_prepare_for_teardown returned:%d",retVal);
     return;
   }

  pdn_context_p = bearer_context_p->ds_bearer_context_dyn_p
                                        ->pdn_context_ptr;

  if(ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p, &subs_id)== FALSE)
  {
    return;
  }
  network_mode = ds3gpp_get_current_network_mode(subs_id);

   /*------------------------------------------------------------------------ 
     Disable the flow and this will ensure that no data retries will be made
     in the whole process of disconnecting and reconnecting the PDN.
     Flow will be enabled when a bearer is reestablished.
     COMING_UP state will not need phys link to be disabled.
     -----------------------------------------------------------------------*/
   if (bearer_context_p->ds_bearer_context_dyn_p->state == 
       DS_BEARER_CONTEXT_STATE_UP)
   {
     ps_phys_link_disable_flow(&(bearer_context_p->phys_link),DS_FLOW_UMTS_MASK);
   }
   /*-------------------------------------------------------------------------- 
     Send out a PDN disconnect only on the default bearer, The dedicated
     bearers will be torn down with this anyways.
     ------------------------------------------------------------------------*/

   if (bearer_context_p->ds_bearer_context_dyn_p->is_default)
   {
     if(ds_bearer_cntxt_tear_down_call(bearer_context_p,&mode_sp_info,NULL)
         == DS3G_FAILURE )
     {
      /*-----------------------------------------------------------------------
       The call has already ended. Do local cleanup
      -----------------------------------------------------------------------*/
       DS_3GPP_MSG1_HIGH("Call already ended Bearer Inst:%d",\
          (uint32)bearer_context_p->ds_bearer_context_dyn_p->client_data_ptr);

       ds_pdn_cntxt_unbind_default_flows(bearer_context_p->ds_bearer_context_dyn_p
                                        ->pdn_context_ptr);

      /*---------------------------------------------------------------------
        Disable signal if this is the last phys link. 
        Each enqueue func_ptr needs to be reset when bearer inst is freed.
        This needs to be done to avoid race conditions when phys_link_gone is
        posted and flow gets enabled on the phys_link leading to each_enq_f
        called when the bearer is getting freed..
       ---------------------------------------------------------------------*/
       ds_bearer_cntxt_free_bearer_inst(bearer_context_p);
       ps_disable_sig(bearer_context_p->rx_sig);
       ps_phys_link_enable_flow(&(bearer_context_p->phys_link),DS_FLOW_UMTS_MASK);
       ps_phys_link_gone_ind(&(bearer_context_p->phys_link));

       if(bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
           ->bearer_down_ind_cb != NULL)
       {
         net_down_reason.reason.ps_iface_down_reason = PS_NET_DOWN_REASON_NOT_SPECIFIED;

         net_down_reason.reason_T = DS_3GPP_PS_IFACE_NET_DOWN_REASON_T;

         bearer_context_p->ds_bearer_context_dyn_p->pdn_context_cb_tbl_ptr
              ->bearer_down_ind_cb(
                      bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr,
                      bearer_context_p,net_down_reason,FALSE);
       }/* Post bearer down indication with call_was_connected as False*/
     }
     else
     {
       /*----------------------------------------------------------------------------- 
         Get the associated PDN context, Only if it is not the last standing PDN context
         on LTE, Move the default bearer and corresponding PDN context to PENDING REDIAL
         -----------------------------------------------------------------------------*/
       pdn_context_p = bearer_context_p->ds_bearer_context_dyn_p->pdn_context_ptr;

       if (!ds_3gpp_pdn_cntx_validate_pdn_context(pdn_context_p))
       {
         return;                                  
       }

       if (network_mode == SYS_SYS_MODE_LTE)
       {
         retVal = dsPdnContext_is_any_other_normal_pdn_active(pdn_context_p);
       }

       if ((retVal) || (network_mode != SYS_SYS_MODE_LTE))
       {
         DS_3GPP_MSG1_HIGH("bearer context state is %d",
                           bearer_context_p->ds_bearer_context_dyn_p->state);

         ds_3gpp_pdn_cntxt_prepare_iface_state_and_timers_for_redial(bearer_context_p);

         bearer_context_p->ds_bearer_context_dyn_p->state = 
           DS_BEARER_CONTEXT_STATE_PEND_REDIAL;

         pdn_context_p->ds_pdn_context_dyn_p->state =  DS_PDN_CONTEXT_STATE_PEND_REDIAL;

         pdn_context_p->ds_pdn_context_dyn_p->redial_reason = redial_cause;

         DS_3GPP_MSG2_HIGH("Bearer context:0x%x in pending redial state, "
         "Putting pdn context:0x%x in pending redial state",bearer_context_p,pdn_context_p);
       }
     }
   }

   return;
}


/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_PDN_PER_PROFILE

DESCRIPTION
  This function tears down the PDN Connections
  using the passed profile number.

PARAMETERS
 Profile Number
 DOWN Reason to be given to Client Applications

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN was torn down successfully
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntx_teardown_pdn_per_profile
( 
  uint8                         profile_num,
  ps_iface_net_down_reason_type tear_down_reason
)
{
  int               loop_index = 0;
  ds_pdn_context_s *pdn_cntx_p = NULL;
  boolean           ret= FALSE;
  /*------------------------------------------------------------------------------------*/

  for(loop_index = 0;loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    pdn_cntx_p  = &ds_pdn_context_tbl[loop_index];

    if (ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntx_p))
    {
      if( pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.\
            pdp_context_number == profile_num )
      {
        DS_3GPP_MSG1_HIGH("Tearing Down PDN with Profile: %d",profile_num);

        pdn_cntx_p->ds_pdn_context_dyn_p->state = 
          DS_PDN_CONTEXT_STATE_GOING_DOWN;

        ret = ds_3gpp_pdn_cntx_teardown_PDN(pdn_cntx_p, tear_down_reason);
      }
    }
  }

  return ret;
} /* ds_3gpp_pdn_cntx_teardown_pdn_per_profile */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_TEARDOWN_APN

DESCRIPTION
  This function tears down the PDN Connections
  using the given APN Name

PARAMETERS
 APN Name
 DOWN Reason to be given to Client Applications
 Subscription Id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN was torn down successfully
  FALSE, otherwise

SIDE EFFECTS
  None
===========================================================================*/

boolean ds_3gpp_pdn_cntx_teardown_apn
( 
  char                          *apn,
  sys_modem_as_id_e_type         subs_id,
  ps_iface_net_down_reason_type  tear_down_reason
)
{
  int               loop_index = 0;
  ds_pdn_context_s *pdn_cntx_p = NULL;
  boolean           ret= FALSE;
  /*------------------------------------------------------------------------------------*/

  for(loop_index = 0;loop_index < DS_3GPP_MAX_PDN_CONTEXT; loop_index++)
  {
    pdn_cntx_p  = &ds_pdn_context_tbl[loop_index];

    if (ds_pdn_cntxt_validate_pdn_subscription_internal(pdn_cntx_p, subs_id) &&
        DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntx_p) == FALSE)
    {
      if( strcasecmp
          ((char*)pdn_cntx_p->ds_pdn_context_dyn_p->pdp_profile.context.apn, 
           (char *)apn) == 0 )
      {
         DATA_3GPP_MSG_SPRINTF_1(MSG_LEGACY_HIGH,"Tearing down PDN with APN:%s",
                                 apn);

        pdn_cntx_p->ds_pdn_context_dyn_p->state = 
          DS_PDN_CONTEXT_STATE_GOING_DOWN;

        ret = ds_3gpp_pdn_cntx_teardown_PDN(pdn_cntx_p, tear_down_reason);
      }
    }
  }

  return ret;
} /* ds_3gpp_pdn_cntx_teardown_apn */

/*===========================================================================
FUNCTION DS_3GPP_APN_PARAM_CHANGE_CMD_HDLR

DESCRIPTION
  This function detects if any of following profile params has changed or not
  1.  APN disabled flag
  2.  APN Name
  3.  Maximum PDN connections per block
  4.  Maximum PDN connection wait timer
  5.  PDN request wait timer
  6.  APN bearer
  7.  APN class
  8.  PDP Type
 
  If any of those above params have changed then it calls another which handles
  PDN disconnect processing if any PDN is up with the changed profile
 
PARAMETERS 
  prof_num:                 Profile whose contents have changed
  subs_id                   Profile Subscription ID
  prf_data:                 Profile specific data (mask, new values ....)
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_apn_param_change_cmd_hdlr
(
  uint16                           prof_num,
  sys_modem_as_id_e_type           subs_id,
  void                            *prf_data
)
{
  sys_sys_mode_e_type              current_sys_mode = SYS_SYS_MODE_MAX;
  ds_umts_pdp_profile_status_etype     result = DS_UMTS_PDP_FAIL;    
  ds_umts_pdp_profile_type             pdp_profile_data;   
  ds_umts_pdp_type_enum_type           new_pdp_type = DS_UMTS_PDP_IPV4;
  ds_umts_pdp_type_enum_type           old_pdp_type = DS_UMTS_PDP_IPV4;
  char                                 *new_apn = NULL;  
  char                                 *old_apn = NULL;   
  ds_pdn_context_s                     *pdn_context_p = NULL;
  uint8                                pdn_index = 0;      
  ps_iface_type                        *v4_iface_p = NULL;
  ps_iface_type                        *v6_iface_p = NULL;
  ps_iface_changed_apn_param_info_type apn_param_info = {0};
  boolean                              apn_change = FALSE;
  boolean                              pdp_change = FALSE;
  boolean                              pdn_state_valid = FALSE;
  boolean                              delay_teardown = FALSE;
/*---------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------- 
   For non LTE modes do not process the request
  -------------------------------------------------------------------------*/

  if((current_sys_mode = ds3gpp_get_current_network_mode(subs_id)) != 
       SYS_SYS_MODE_LTE)
  {
    DS_3GPP_MSG1_HIGH ("Current SYS mode is %d, PDN disconnect feature "
                       "disabled", current_sys_mode);
    return;
  }


  /*-------------------------------------------------------------------------- 
    Check if the changed apn parameters are one of
    1. apn_name
    2. pdp_type.
    If they are then delay proceesing. Send an apn_param_chg ind to the
    framework and allow the client to tear down the call. Also start a 5s timer
    If the timer expires then continue with the normal silent redial.
  -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------ 
    Get the profile data from the cache
  ------------------------------------------------------------------------*/
  result = ds_umts_get_pdp_profile_all_data_per_subs(prof_num,
                                          dsumts_subs_mgr_get_subs_id(subs_id), 
                                          &pdp_profile_data);

  if(( result != DS_UMTS_PDP_SUCCESS ) || 
     (!pdp_profile_data.context.valid_flg))
  {
    DS_3GPP_MSG3_ERROR("Error in retrieving definition for prof number:%d"
                       "result:%d, valid_flag:%d",
                        prof_num,result,
                        pdp_profile_data.context.valid_flg);
    return;
  }

  new_pdp_type = pdp_profile_data.context.pdp_type;
  new_apn =  (char *)pdp_profile_data.context.apn;

  for(pdn_index = 0; pdn_index < DS_3GPP_MAX_PDN_CONTEXT; pdn_index++)
  {
    pdn_context_p = &ds_pdn_context_tbl[pdn_index];
    pdn_state_valid = FALSE;
    delay_teardown = FALSE;

    /*---------------------------------------------------------------- 
      PDN state is valid only if it is UP or COMING_UP state
    ----------------------------------------------------------------*/

     if((ds_pdn_cntxt_validate_pdn_subscription_internal(
                                  pdn_context_p,subs_id)) &&
       ((pdn_context_p->ds_pdn_context_dyn_p->state == 
         DS_PDN_CONTEXT_STATE_UP)  || (pdn_context_p->
         ds_pdn_context_dyn_p->state == 
         DS_PDN_CONTEXT_STATE_COMING_UP)))
    { 
      pdn_state_valid = TRUE;
      DS_3GPP_MSG0_HIGH("PDN state is valid");
    }

    /*------------------------------------------------------------------ 
      Ensure that the pdn context's profile num match occurs
      and the state is up.
      Also if apn_param_chg_teardown is set to TRUE ignore because MH
      is already processing a silent redial.
      Also if the pdn_context is in an Emergency call simply continue
      Also if the timer is already running, no need to do anything 
    -----------------------------------------------------------------*/
    if(!pdn_state_valid ||
       (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile_num != prof_num) ||
       (pdn_context_p->ds_pdn_context_dyn_p->apn_param_chg_teardown) ||
       (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_context_p)))
    {
      continue;
    }

    if(rex_get_timer(&(pdn_context_p->ds_pdn_context_dyn_p
                       ->apn_param_chg_timer)))
    {
      DS_3GPP_MSG0_ERROR("apn block timer already running not starting again");
      continue;
    }
   
    /*------------------------------------------------------------------- 
      Extract pdp_type from user_data because pdn_context's pdp type
      can be over written
    -------------------------------------------------------------------*/
    old_pdp_type = (ds_umts_pdp_type_enum_type)prf_data;

    old_apn = (char*)pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.apn;

    if((strncasecmp((char*)old_apn,(char*)new_apn,
                        DS_UMTS_MAX_APN_STRING_LEN+1)!= 0))
    {
      apn_param_info.apn_mask |= PS_IFACE_APN_PARAM_APN_NAME;
      (void)strlcpy((char*)apn_param_info.new_apn_param_values.apn_name,
                    (char*)new_apn,
                    DS_UMTS_MAX_APN_STRING_LEN+1); 
      (void)strlcpy((char*)apn_param_info.old_apn_param_values.apn_name,
                    (char*)old_apn,
                    DS_UMTS_MAX_APN_STRING_LEN+1); 
      apn_change = TRUE;
    }

    if(old_pdp_type != new_pdp_type)
    {
      apn_param_info.apn_mask |= PS_IFACE_APN_PARAM_PDP_TYPE;
      apn_param_info.new_apn_param_values.pdp_type = new_pdp_type;
      apn_param_info.old_apn_param_values.pdp_type = old_pdp_type;
      pdp_change = TRUE;
    }

    /*------------------------------------------------------------------------ 
      The 5 second timer will need to be started in the below cases
      1. Pdp change occurs
      2. APN chnage occurs
      3. PDN context state is COMING_UP --> Delay tearing down and bringing
      up the pdn context until the timer expires(5s). This will hopefully give
      enough time for the PDN context to change to UP or DOWN.
    ------------------------------------------------------------------------*/
    if((pdp_change) || (apn_change) || (pdn_context_p->
                                        ds_pdn_context_dyn_p->state
                                         == DS_PDN_CONTEXT_STATE_COMING_UP))
    {
      DS_3GPP_MSG3_HIGH("Delay APN param change teardown by 5 secs"
                        "pdp_change:%d,apn_change:%d,"
                        "pdn_state:%d",pdp_change,apn_change,
                        pdn_context_p->ds_pdn_context_dyn_p->state);
      delay_teardown = TRUE;
    }
    
    /*------------------------------------------------------------------------ 
      if pdp change or apn change occurs send the indication to the client 
    ------------------------------------------------------------------------*/
    if((pdp_change) || (apn_change))
    {
      DS_3GPP_MSG0_HIGH("Send Client teardown indication");

      v4_iface_p = DSPDNCNTXT_GET_V4_PS_IFACE_PTR(pdn_context_p);
      v6_iface_p = DSPDNCNTXT_GET_V6_PS_IFACE_PTR(pdn_context_p);
    
      if(v4_iface_p != NULL)
      {
        DS_3GPP_MSG0_HIGH("APN PARAM CHG EV for V4 iface");
        ps_iface_generic_ind(v4_iface_p, 
                             IFACE_APN_PARAM_CHANGED_EV ,
                             (void *)&apn_param_info);
      }

      if(v6_iface_p != NULL)
      {
        DS_3GPP_MSG0_HIGH("APN PARAM CHG EV for V6 iface");
        ps_iface_generic_ind(v6_iface_p, 
                             IFACE_APN_PARAM_CHANGED_EV ,
                             (void *)&apn_param_info);
      }
    }

    /*------------------------------------------------------------------------ 
      If this stage is reached it means that there is no apn_name or pdp_type
      change
      Now check for all the other apn params to see if really an apn parameter
      changed
    -----------------------------------------------------------------------*/
    if(((pdp_change == FALSE) && (apn_change == FALSE)) &&
        (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.apn_disable_flag ==
         pdp_profile_data.apn_disable_flag) &&
        (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.apn_class ==
         pdp_profile_data.apn_class) &&
         (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.apn_bearer == 
         pdp_profile_data.apn_bearer) &&
         (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.pdn_req_wait_time == 
          pdp_profile_data.pdn_req_wait_time) &&
         (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.max_pdn_conn_per_blk ==
          pdp_profile_data.max_pdn_conn_per_blk) &&
         (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.max_pdn_conn_time ==
          pdp_profile_data.max_pdn_conn_time))
    {
      DS_3GPP_MSG0_HIGH("No Real APN Param chg, Return");
      return;
    }

    /*------------------------------------------------------------------------ 
      Start the timer if delay_teardown flag is set. If we reach here, it 
      means that any of the APN params have changed.
    ------------------------------------------------------------------------*/
    if(delay_teardown)
    {
      DS_3GPP_MSG0_HIGH("Starting delay teardown timer as apn param has changed");
      
      /*------------------------------------------------------------------------
        Start the timer interval 
      ------------------------------------------------------------------------*/
      (void)rex_set_timer(&(pdn_context_p->ds_pdn_context_dyn_p->
                            apn_param_chg_timer),DS_3GPP_APN_PARAM_CHG_INTERVAL);
      return;
    }
 }

 DS_3GPP_MSG0_MED("APN param chg cmd processed");
#ifdef FEATURE_DATA_LTE
 ds_eps_apn_param_change_cmd_hdlr(prof_num);
#endif /* FEATURE_DATA_LTE */
}

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_PREPARE_IFACE_STATE_AND_TIMERS_FOR_REDIAL

DESCRIPTION
  This function puts the ifaces in configuring state and posts an event
  to stop IP6 state machine.
  It also stops the MCS timer maintained per bearer

PARAMETERS 
  bearer_cntxt_p
 
DEPENDENCIES
  None.

RETURN VALUE
  void 
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_prepare_iface_state_and_timers_for_redial
(
   void  *bearer_context_ptr
)
{
  ds_pdn_context_s                      *pdn_context_p = NULL;
  ds_3gpp_iface_s                       *ds3gpp_v6_iface_p = NULL;
  ds_3gpp_iface_s                       *ds3gpp_v4_iface_p = NULL;
  ds_umts_pdp_type_enum_type             pdn_pdp_type = DS_UMTS_PDP_MAX;
  ds_bearer_context_s                   *bearer_context_p = NULL;
/*-------------------------------------------------------------------------*/


  bearer_context_p = (ds_bearer_context_s *)bearer_context_ptr;

  if (!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
  {
    return;
  }

  pdn_context_p = (ds_pdn_context_s *)bearer_context_p->ds_bearer_context_dyn_p
                  ->pdn_context_ptr;

  if (!ds_3gpp_pdn_cntx_validate_pdn_context (pdn_context_p))
  {
    return;
  }

  if (pdn_context_p->ds_pdn_context_dyn_p->pdp_profile.context.pdp_type != 
                                                               DS_UMTS_PDP_PPP) 
  {    
    pdn_pdp_type = ds_pdn_cntxt_get_pdn_context_ip_support(pdn_context_p);
    DS_3GPP_MSG1_MED ("PDP Type in PDN context is %d",pdn_pdp_type);

    if(pdn_pdp_type == DS_UMTS_PDP_IPV4)
    {
      ds3gpp_v4_iface_p = 
        (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

      if(ds3gpp_v4_iface_p != NULL)
      {
        DS_3GPP_MSG0_MED ("Putting v4 iface in configuring state");
        ps_iface_configuring_ind(&(ds3gpp_v4_iface_p->ps_iface));
      }
      else
      {
        DS_3GPP_MSG0_ERROR ("Invalid V4 iface");
      }
    }
#ifdef FEATURE_DATA_PS_IPV6
    else if(pdn_pdp_type == DS_UMTS_PDP_IPV6)
    {
      ds3gpp_v6_iface_p = 
        (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);

      if(ds3gpp_v6_iface_p != NULL)
      {
        DS_3GPP_MSG0_MED ("Putting v6 iface in configuring state and IP6 state"
                          " machine in Re-Sync state");
        if (dssnet6_sm_post_event(&(ds3gpp_v6_iface_p->dssnet6_sm_cb),
                                  DSSNET6_LINK_RESYNC_EV, 
                                  NULL) != 0) 
        {
          DS_3GPP_MSG0_ERROR("Error in posting dssnet6 Re-Sync event ");      
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR ("Invalid V6 iface");
      }
    }
    else if(pdn_pdp_type == DS_UMTS_PDP_IPV4V6)
    {
      ds3gpp_v4_iface_p = 
        (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV4_IFACE]);

      if(ds3gpp_v4_iface_p != NULL)
      {
        DS_3GPP_MSG0_MED ("Putting v4 iface in configuring state");
        ps_iface_configuring_ind(&(ds3gpp_v4_iface_p->ps_iface));
      }
      else
      {
        DS_3GPP_MSG0_ERROR ("Invalid V4 iface");
      }

      ds3gpp_v6_iface_p = 
        (pdn_context_p->ds_pdn_context_dyn_p->ds_3gpp_iface_tbl
         [DS_3GPP_PDN_CONTEXT_IPV6_IFACE]);

      if(ds3gpp_v6_iface_p != NULL)
      {
        DS_3GPP_MSG0_MED ("Putting v6 iface in configuring state and IP6 state"
                          " machine in Re-Sync state");
        if (dssnet6_sm_post_event(&(ds3gpp_v6_iface_p->dssnet6_sm_cb),
                                  DSSNET6_LINK_RESYNC_EV, 
                                  NULL) != 0) 
        {
          DS_3GPP_MSG0_ERROR("Error in posting dssnet6 Re-Sync event ");      
        }
      }
      else
      {
        DS_3GPP_MSG0_ERROR ("Invalid V6 iface");
      }
    }
#endif /*FEATURE_DATA_PS_IPV6*/ 
    else
     
    {
      DATA_ERR_FATAL("Invalid PDP Type for a PDN context");
    }
  }
}

/*===========================================================================
FUNCTION DS_PDN_CNTXT_VALIDATE_PDN_SUBSCRIPTION_INTERNAL

DESCRIPTION
  This function validates whether the PDN Context belong to the
  specified subscription id. This is for checking valid, no F3 if invalid.

PARAMETERS
  pdn_context_p: PDN Context Pointer
  subs_id:       Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN Context belong to the specified subscription id.
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_validate_pdn_subscription_internal
(
  ds_pdn_context_s        *pdn_context_p,
  sys_modem_as_id_e_type   subs_id
)
{
  sys_modem_as_id_e_type   pdn_subs_id = SYS_MODEM_AS_ID_NONE;
  /*-----------------------------------------------------------------------*/
  return (ds_3gpp_pdn_cntxt_get_subs_id_from_pdn_internal(pdn_context_p,
                                                          &pdn_subs_id)  &&
          pdn_subs_id == subs_id);

} /*ds_pdn_cntxt_validate_pdn_subscription_internal*/

/*===========================================================================
FUNCTION DS_PDN_CNTXT_VALIDATE_PDN_SUBSCRIPTION

DESCRIPTION
  This function validates whether the PDN Context belong to the
  specified subscription id.

PARAMETERS
  pdn_context_p: PDN Context Pointer
  subs_id:       Subscription Id
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE, if PDN Context belong to the specified subscription id.
  FALSE, otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_pdn_cntxt_validate_pdn_subscription
(
  ds_pdn_context_s        *pdn_context_p,
  sys_modem_as_id_e_type   subs_id
)
{
  sys_modem_as_id_e_type   pdn_subs_id = SYS_MODEM_AS_ID_NONE;
  /*-----------------------------------------------------------------------*/

  return (ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_context_p,
                                                 &pdn_subs_id)  &&
          pdn_subs_id == subs_id);

} /* ds_pdn_cntxt_validate_pdn_subscription */


/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_CURRENT_ACTIVE_CONTEXT

DESCRIPTION
  This function returns the number of active bearer contexts.

PARAMETERS
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of active bearer contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_pdn_get_current_active_context(sys_modem_as_id_e_type subs_id)
{
  uint8  loop_index=0;
  uint8  ref_count=0;
  ds_bearer_context_s  *bearer_context_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for(loop_index=0;loop_index<DS_3GPP_MAX_BEARER_CONTEXT;loop_index++)
  {
    bearer_context_p = ds_bearer_cntxt_get_instance_ptr(loop_index);
    if(!(ds_bearer_cntx_validate_bearer_subscription_internal
          (bearer_context_p, subs_id)))
    {
      continue;
    }
    /*------------------------------------------------------------------- 
     Check bearer context table to check Bearer's which is in UP state,
     and increment count.
    -------------------------------------------------------------------*/
    if(bearer_context_p->ds_bearer_context_dyn_p->state ==
            DS_BEARER_CONTEXT_STATE_UP)
    {
      ref_count=ref_count+1;
    }
  }
  return ref_count;
}/* ds_3gpp_pdn_get_current_active_context */

/*===========================================================================
FUNCTION DS_3GPP_PDN_GET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function gets the maximum number of sumultanous active pdp contexts for
  the current RAT and PLMN.

PARAMETERS
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  Number of simultanous active pdp contexts.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_pdn_get_max_simul_active_pdp_context
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);
  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Hndl is Invalid for subs id: %d", subs_id);
    return 0;
  }

  return hndl->max_simul_active_pdp_context; 
}/* ds_3gpp_pdn_get_max_simul_active_pdp_context */

/*===========================================================================
FUNCTION DS_3GPP_PDN_SET_MAX_SIMUL_ACTIVE_PDP_CONTEXT

DESCRIPTION
  This function sets the maximum number of sumiltanous active pdp contexts 
  for the current PLMN and RAT.
  pdn connection.

PARAMETERS
  max_active_pdp_context
  subs_id          : Subscription Id 
  
DEPENDENCIES
  None.

RETURN VALUE  
  void

SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_set_max_simul_active_pdp_context
(
  uint8 max_active_pdp_context, 
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);
  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Hndl is Invalid for subs id: %d", subs_id);
  }
  else
  {
    hndl->max_simul_active_pdp_context = max_active_pdp_context;
  }
 
} /* ds_3gpp_pdn_set_max_simul_active_pdp_context */


/*===========================================================================
FUNCTION DS_3GPP_GET_IPV6_PREFIX_DELEGATION_MODE
 
DESCRIPTION
  This function goes through the Profile cache to check if the changed apn is
  present in any of the other profiles.
  Return true if it is present in any other profiles
  Return false if it is not present in any other profiles.

PARAMETERS 
  profile_num
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_3gpp_get_ipv6_prefix_delegation_mode
(
   ps_iface_type * ps_iface_p
)
{
  boolean           ret_val         = FALSE;
#if defined (FEATURE_DATA_PS_IPV6)
  ds_3gpp_iface_s   *ds_iface_p     = NULL;
  ds_pdn_context_s  *pdn_context_p  = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_iface_p == NULL)
  {
    DS_3GPP_MSG1_HIGH("PS iface Ptr NULL",ps_iface_p);
    return ret_val;
  }

  ds_iface_p = (ds_3gpp_iface_s*)ps_iface_p->client_data_ptr;

  if (ds_iface_p == NULL)
  {
    DS_3GPP_MSG1_HIGH("No DS iface associated with PS iface 0x%x",ps_iface_p);
    return ret_val;
  }

  pdn_context_p = (ds_pdn_context_s*)ds_iface_p->client_data_ptr;

  if (!ds_3gpp_pdn_cntx_validate_pdn_context (pdn_context_p))
  {
    return ret_val;
  }

  ret_val = pdn_context_p->ds_pdn_context_dyn_p->
            pdp_profile.ipv6_prefix_delegation_flag;
#else
  DS_3GPP_MSG0_ERROR ("FEATURE NOT Supported. Return False");
#endif

  return ret_val;
}


/*===========================================================================
  FUNCTION DS_3GPP_PDN_CNTX_APN_NAME_CHG_CMD_HDLR

  DESCRIPTION
  This function handles the apn name change, It extracts the apn name
  for the profile number passed and sends both the old and new apn as
  an event to ds3gmgr
  
  PARAMETERS
  1. Cmd Ptr
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_pdn_cntx_apn_name_chg_cmd_hdlr
(
  const ds_cmd_type      *cmd_ptr
)
{
  ds_profile_db_apn_name_chg_info_type *apn_name_chg_info_ptr = NULL;
  ds_umts_pdp_profile_status_etype      result = DS_UMTS_PDP_FAIL;
  byte                                  new_apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds3geventmgr_event_info_type          event_info;
  ds3geventmgr_filter_type              filter_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_3GPP_MSG0_HIGH(" In ds_3gpp_pdn_cntx_apn_name_chg_cmd_hdlr");

  if (cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input cmd ptr is NULL");
    return;
  }

  apn_name_chg_info_ptr = (ds_profile_db_apn_name_chg_info_type*)
                                                    cmd_ptr->cmd_payload_ptr;

  /*------------------------------------------------------------------------- 
    Get the apn name, This will represent the new apn name
  ---------------------------------------------------------------------------*/
  result = ds_umts_get_pdp_profile_context_info_apn_per_subs(
                            apn_name_chg_info_ptr->profile_num,
                            dsumts_subs_mgr_get_subs_id(apn_name_chg_info_ptr
                                                        ->subs_id),
                            new_apn_name,
                            DS_UMTS_MAX_APN_STRING_LEN);

  if (result != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot get apn name from profile:%d",
                       apn_name_chg_info_ptr->profile_num);
    return;
  }

  filter_info.ps_subs_id =  (ps_sys_subscription_enum_type) 
                                 ds3gsubsmgr_subs_id_cm_to_ds
                                    (apn_name_chg_info_ptr->subs_id);

  filter_info.tech = PS_SYS_TECH_3GPP;

  strlcpy((char *)event_info.apn_name_chg_info.new_apn_name,
          (char *)new_apn_name,
          DS_SYS_MAX_APN_LEN);

  strlcpy((char *)event_info.apn_name_chg_info.old_apn_name, 
          (char *)apn_name_chg_info_ptr->old_apn_name,
          DS_SYS_MAX_APN_LEN);

  /*--------------------------------------------------------------------------------- 
    Reset the resolved apn here since the system is no longer on LTE
   ---------------------------------------------------------------------------------*/
  if(ds3geventmgr_set_event_info(DS3GEVENTMGR_PROFILE_APN_NAME_CHG_EV,
                                 &event_info, &filter_info)== FALSE)
  {
    DS_3GPP_MSG0_ERROR("Unable to provide Resolved APN info to internal"
                       " clients");
  }

  ds_3gpp_apn_name_change_cb_hdlr(apn_name_chg_info_ptr->profile_num,
                                  apn_name_chg_info_ptr->subs_id);

  return;
}/*ds_3gpp_pdn_cntx_apn_name_chg_cmd_hdlr*/


/*===========================================================================
  FUNCTION DS_3GPP_PDN_CNTX_PDP_TYPE_CHG_CMD_HDLR

  DESCRIPTION
  This function handles the pdp type change, It extracts the apn name
  for the profile number passed and clears the network override handler

  PARAMETERS
  1. Cmd Ptr

  DEPENDENCIES
  None.

  RETURN VALUE
  None.

  SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntx_pdp_type_chg_cmd_hdlr
(
  const ds_cmd_type      *cmd_ptr
)
{
  byte                              apn_name[DS_UMTS_MAX_APN_STRING_LEN+1];
  ds_network_overrridden_ip_type_e  overridden_ip = PDP_UNDEFINED;
  ds_umts_pdp_profile_status_etype  result = DS_UMTS_PDP_FAIL;
  ds_profile_db_pdp_chg_info_type   *pdp_chg_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd_ptr == NULL || cmd_ptr->cmd_payload_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Input cmd ptr is NULL");
    return;
  }

  pdp_chg_info_ptr = (ds_profile_db_pdp_chg_info_type*)cmd_ptr->cmd_payload_ptr;

  /*------------------------------------------------------------------------- 
    Get the apn name and reset the network override handler
  ---------------------------------------------------------------------------*/
  result = ds_umts_get_pdp_profile_context_info_apn_per_subs(pdp_chg_info_ptr->profile_num,
                                                    dsumts_subs_mgr_get_subs_id(pdp_chg_info_ptr->subs_id),
                                                    apn_name,
                                                    DS_UMTS_MAX_APN_STRING_LEN);

  if (result != DS_UMTS_PDP_SUCCESS)
  {
    DS_3GPP_MSG1_ERROR("Cannot get apn name from profile:%d",
                       pdp_chg_info_ptr->profile_num);
    return;
  }

  (void)ds_3gpp_network_override_hdlr_set_overridden_ip
                      (apn_name,overridden_ip,
                       (sys_modem_as_id_e_type)pdp_chg_info_ptr->subs_id);

  return;

}

#ifdef FEATURE_EMERGENCY_PDN
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SET_PENDING_TEARDOWN_FLAG 
 
DESCRIPTION
  This function set boolean pending teardown flag in pdn_cntxt for all
  non-emergency COMING UP call , UP call and Pending Redial calls

PARAMETERS 
  None
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_pdn_cntxt_set_pending_teardown_flag(void)
{
  uint8                     pdn_instance = 0 ;
  ds_pdn_context_s         *local_pdn_cntxt_p = NULL;
  ds_pdn_context_state_e    pdn_state = DS_PDN_CONTEXT_STATE_DOWN;
/*--------------------------------------------------------------------------*/
  
  for(pdn_instance = 0; pdn_instance < DS_3GPP_MAX_PDN_CONTEXT; pdn_instance++)
  {
    local_pdn_cntxt_p = &(ds_pdn_context_tbl[pdn_instance]);
    if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(local_pdn_cntxt_p))
    {     
      continue;
    }

    pdn_state = local_pdn_cntxt_p->ds_pdn_context_dyn_p->state ;
    if(DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntxt_p) == FALSE)
    {
      if((pdn_state == DS_PDN_CONTEXT_STATE_PEND_REDIAL)||
         (pdn_state == DS_PDN_CONTEXT_STATE_COMING_UP) ||
         (pdn_state == DS_PDN_CONTEXT_STATE_UP))
      {
        DS_3GPP_MSG1_HIGH("Setting Emergency teardown flag to true for PDN 0x%x",
                           local_pdn_cntxt_p);
        local_pdn_cntxt_p->ds_pdn_context_dyn_p->pending_teardown = TRUE;
      }
    }
  }
  return;
}/*ds_3gpp_pdn_cntxt_set_pending_teardown_flag*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_CHECK_AND_TEARCDOWN_NON_EMERGENCY_PDNS
 
DESCRIPTION
  This function checks if there are any calls in coming up state
  If YES: Do not perform any action
  If NO :
        If Auth fail occured :
              Teardown all calls in UP state which have emerg_auth_fail
              flag set in the PDN
  This Function is used to teardown all active calls after Emergency call was
  made with authentication failure.DS allows all the active UP and COMING UP
  calls to be brought up completely and then be torn down
   
PARAMETERS 
  None
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  None
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
void ds_3gpp_pdn_cntxt_check_and_teardown_non_emergency_pdns(void)
{
  uint8                  pdn_instance = 0;
  ds_pdn_context_s       *local_pdn_cntx_p = NULL;

/*---------------------------------------------------------------------------*/
  if(ds_3gpp_pdn_cntxt_check_non_emergency_pdn_teardown() == TRUE) 
  {
    for(pdn_instance = 0; pdn_instance < DS_3GPP_MAX_PDN_CONTEXT; pdn_instance++)
    {
      local_pdn_cntx_p = &(ds_pdn_context_tbl[pdn_instance]);
      if((ds_3gpp_pdn_cntx_validate_pdn_context_internal(local_pdn_cntx_p)) &&
         (ds_3gpp_pdn_cntxt_get_pending_teardown_flag(local_pdn_cntx_p) == TRUE))
      {
        local_pdn_cntx_p->ds_pdn_context_dyn_p->state =
                                       DS_PDN_CONTEXT_STATE_GOING_DOWN;
        DS_3GPP_MSG1_HIGH("Teardown non-emergency PDN 0x%x",local_pdn_cntx_p);
        ds_3gpp_pdn_cntx_teardown_PDN(local_pdn_cntx_p, 
                             PS_NET_DOWN_REASON_AUTH_FAILURE_ON_EMERGENCY_CALL);
      }
    }
  }
 return;
}/*ds_3gpp_pdn_cntxt_check_and_teardown_non_emergency_pdns*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_CHECK_NON_EMERGENCY_PDN_TEARDOWN
 
DESCRIPTION
  This function checks if there are any calls in coming up state which have
  pending_teardown_pdn flag set and checks for UP calls and Pending Redial 
  calls .If there are no Coming UP calls found and there is atleast one UP/
  PEND_REDIAL call avaliable with pending_teardown flag set , this function
  true indicating to teardown the active data calls 
   
PARAMETERS 
  None
             
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean : TRUE : Teardown non emergency PDN 
          : FALSE: No action
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_check_non_emergency_pdn_teardown(void)
{
  uint8                  pdn_instance = 0;
  ds_pdn_context_s       *local_pdn_cntxt_p = NULL;
  boolean                found_pdn_in_coming_up_state = FALSE;
  boolean                found_pdn_to_be_torn_down = FALSE;
  boolean                allow_teardown = FALSE;
/*----------------------------------------------------------------------------*/
  for(pdn_instance = 0; pdn_instance < DS_3GPP_MAX_PDN_CONTEXT; pdn_instance++)
  {
    local_pdn_cntxt_p = &(ds_pdn_context_tbl[pdn_instance ]);

    if (!ds_3gpp_pdn_cntx_validate_pdn_context_internal(local_pdn_cntxt_p))
    {     
      continue;
    }
    
    if((ds_3gpp_pdn_cntxt_get_pending_teardown_flag(local_pdn_cntxt_p) == TRUE) ||
       (DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(local_pdn_cntxt_p) == TRUE))
    {
      DS_3GPP_MSG2_MED("PDN cntxt 0x%x in state %d ",
                       local_pdn_cntxt_p,
                       local_pdn_cntxt_p->ds_pdn_context_dyn_p->state);   
      switch (local_pdn_cntxt_p->ds_pdn_context_dyn_p->state)  
      {
        case DS_PDN_CONTEXT_STATE_COMING_UP:
          found_pdn_in_coming_up_state = TRUE;
          break;

        case DS_PDN_CONTEXT_STATE_UP:
        case DS_PDN_CONTEXT_STATE_PEND_REDIAL:
          found_pdn_to_be_torn_down = TRUE;          
          break;
        
        default:
          //ignore
          break;
      }
    }
  }
  /* For teardown , there should be no call in coming up state 
     and atleast one call in UP or Pending redial state , only then 
     teardown is allowed because there PDN have been marked to be torn down
     because of Emergnecy authenication failure */
  if(!found_pdn_in_coming_up_state && found_pdn_to_be_torn_down) 
  {
    allow_teardown = TRUE;
  }
  else
  {
    allow_teardown = FALSE;
  }
  return allow_teardown; 
}/*ds_3gpp_pdn_cntxt_check_non_emergency_pdn_teardown*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_PENDING_TEARDOWN_FLAG
 
DESCRIPTION
  This function gets the pending teardown flag from PDN being passed
   
PARAMETERS 
  pdn_cntxt_p : PDN context pointer
             
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  boolean : TRUE : Pending Teardown is set  
          : FALSE: Pending Teardown is not set
 
SIDE EFFECTS 
  
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_pending_teardown_flag
(
  ds_pdn_context_s    *pdn_cntxt_p
)
{
   boolean teardown_flag = FALSE;
/*--------------------------------------------------------------------------*/
   if(ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_p))
   {
     teardown_flag = pdn_cntxt_p->ds_pdn_context_dyn_p->pending_teardown;
   }
   else
   {
     DS_3GPP_MSG1_HIGH("Invalid PDN being passed 0x%x",pdn_cntxt_p);
   }
   return teardown_flag;

}/*ds_3gpp_pdn_cntxt_get_pending_teardown_flag*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_GET_EMERGENCY_AUTH_FAIL

DESCRIPTION
  This function gets the variable ds_3gpp_emergency_auth_fail
 
PARAMETERS 
  Boolean Value
 
DEPENDENCIES
  None.

RETURN VALUE
  Boolean : ds_3gpp_emergency_auth_fail : TRUE: Authentication failure on Emerg call
                                          FALSE: No Authentication Ffailure
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_get_emergency_auth_fail(void)
{
  boolean emergency_auth_fail_flag = FALSE;
/*------------------------------------------------------------------------*/
  ds3gpp_enter_global_crit_section();
  emergency_auth_fail_flag = ds_3gpp_emergency_auth_fail ;
  ds3gpp_leave_global_crit_section(); 
  return emergency_auth_fail_flag;
}/*ds_3gpp_pdn_cntxt_get_emergency_auth_fail*/

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_SET_EMERGENCY_AUTH_FAIL

DESCRIPTION
  This function sets the variable ds_3gpp_emergency_auth_fail to variable 
  being passed
 
PARAMETERS 
  Boolean Value: ds_3gpp_emergency_auth_fail gets updated to value being passed
 
DEPENDENCIES
  None.

RETURN VALUE
  None
 
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_pdn_cntxt_set_emergency_auth_fail(boolean value)
{
  ds3gpp_enter_global_crit_section();
  ds_3gpp_emergency_auth_fail = value;
  ds3gpp_leave_global_crit_section(); 
  DS_3GPP_MSG1_HIGH("Set Emergency Auth Fail flag to %d ",value);
}/*ds_3gpp_pdn_cntxt_set_emergency_auth_fail*/

#endif /* FEATURE_EMERGENCY_PDN */

#ifdef FEATURE_UE_INITIATED_DORMANCY 
/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_alloc_memory_for_delay_dormancy_stats

DESCRIPTION
  This function allocated dynamic memory required for for delay 
   dormancy algorithm

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to dynamic memory

SIDE EFFECTS

===========================================================================*/
ds_3gpp_delay_dormancy_s* ds_3gpp_pdn_cntxt_alloc_memory_for_delay_dormancy_stats()
{
  ds_3gpp_delay_dormancy_s*  dormancy_info_p = NULL;
  /* ----------------------------------------------------------------------- */
  DS_3GPP_MSG0_HIGH("Alloc memory for delay dormancy statistics");
  dormancy_info_p = (ds_3gpp_delay_dormancy_s*) 
                           modem_mem_alloc(sizeof(ds_3gpp_delay_dormancy_s),
                                                       MODEM_MEM_CLIENT_DATA);

  if (dormancy_info_p != NULL)
  {
    /* reset the dynamic memory allocated to 0 */
    memset(dormancy_info_p, 0, sizeof(ds_3gpp_delay_dormancy_s));
    ds_3gpp_delay_dormancy_info_p = dormancy_info_p;
  }
  
  return dormancy_info_p;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats

DESCRIPTION
  This function frees the allocated dynamic memory for for delay 
   dormancy algorithm

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_pdn_cntxt_free_memory_for_delay_dormancy_stats()
{
  /* ----------------------------------------------------------------------- */
  DS_3GPP_MSG0_HIGH("Free memory for delay dormancy statistics");
  if (ds_3gpp_delay_dormancy_info_p != NULL)
  {
    modem_mem_free(ds_3gpp_delay_dormancy_info_p, MODEM_MEM_CLIENT_DATA);
  }
  /* Setting the pointer to NULL */
  ds_3gpp_delay_dormancy_info_p = NULL;
  
  return;
}

/*===========================================================================
FUNCTION ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr

DESCRIPTION
  This function gets the dynamic pointer allocated for stats

PARAMETERS
  adress of variable
  
DEPENDENCIES
  None.

RETURN VALUE  
  Pointer to dynamic memory

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_validate_and_get_delay_dormancy_info_ptr
(
  ds_3gpp_delay_dormancy_s** info_p
)
{ 
  boolean ret_val = FALSE;
  /* ---------------------------------------------------------------------- */ 
  if (ds_3gpp_delay_dormancy_info_p == NULL)
  {
    ret_val = FALSE;
  }
  else
  {
    *info_p = ds_3gpp_delay_dormancy_info_p;
     ret_val = TRUE;
  }
  DS_3GPP_MSG1_HIGH("get and validate delay dormancy ptr,ret_val:%d ",ret_val);
  return ret_val;
}

/*===========================================================================
 DS_3GPP_BEARER_CNTXT_INIT_OR_CHECK_CURRENT_DATA_STATISTICS
  
 DESCRIPTION
  This function caches present IFACE statistics or checks for data activity 
  based on input operation type given.
  
 PARAMETERS
  operation_type: to Cache/Check data activity
 DEPENDENCIES
  None.
  
 RETURN VALUE
TRUE - for Cache operation
TRUE/FALSE - for Data activity check
  
 SIDE EFFECTS
  None.
  
 ===========================================================================*/
boolean ds_3gpp_bearer_cntxt_init_or_check_data_statistics
(
  ds_3gpp_operation_type_e op_type
)
{
  uint8                call_id;
  uint8                pdn_instance;
  ds_bearer_context_s *bearer_context_p = NULL;
  ds_pdn_context_s    *pdn_cntxt_p = NULL;
  boolean              rtval;
  boolean              activity_check = FALSE;
  ds_3gpp_dormancy_delay_stats_s *stats_info;
  ds_3gpp_dormancy_delay_stats_s cached_stats_info;


  for (pdn_instance=0;\
       pdn_instance<DS_3GPP_MAX_PDN_CONTEXT;\
       pdn_instance++ )
  {
    pdn_cntxt_p = &ds_pdn_context_tbl[pdn_instance];
    if(!ds_3gpp_pdn_cntx_validate_pdn_context_internal(pdn_cntxt_p))
    {
      continue;
    }
    bearer_context_p = 
                pdn_cntxt_p->ds_pdn_context_dyn_p->def_bearer_context_ptr;
    if(!ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      continue;
    }
    call_id = bearer_context_p->ds_bearer_context_dyn_p->cm_call_id;
    stats_info = &(ds_3gpp_delay_dormancy_info_p->stats[pdn_instance]);
    switch(op_type)
    {
      case DS_3GPP_INIT_DATA_STATS:
        rtval = ds_3gpp_pdn_cntx_get_iface_stats(call_id, 
                                                 &(stats_info->rxbyte_cnt),
                                                 &(stats_info->txbyte_cnt));
        if (rtval == TRUE)
        {
          DS_3GPP_MSG3_HIGH("stats cache successfull for call_id: %d "
                             "rxbyte_cnt: %d, txbyte_cnt: %d",call_id, 
                             stats_info->rxbyte_cnt, stats_info->txbyte_cnt);
        } 
        break;

      case DS_3GPP_CHECK_DATA_ACTIVITY:
        if(pdn_cntxt_p->ds_pdn_context_dyn_p->state == DS_PDN_CONTEXT_STATE_UP)
        {
          cached_stats_info.rxbyte_cnt = stats_info->rxbyte_cnt;
          cached_stats_info.txbyte_cnt = stats_info->txbyte_cnt;
          rtval = ds_3gpp_pdn_cntx_get_iface_stats(call_id, 
                                                   &(stats_info->rxbyte_cnt),
                                                   &(stats_info->txbyte_cnt));
          if ((rtval == DS3G_SUCCESS)&&
             ((cached_stats_info.rxbyte_cnt!= stats_info->rxbyte_cnt)||
              (cached_stats_info.txbyte_cnt!= stats_info->txbyte_cnt)))
          {
            DS_3GPP_MSG3_HIGH("Data activity present for call_id:%d," 
                               "prev_rxnt: %d, new_rxcnt: %d",call_id,
                               cached_stats_info.rxbyte_cnt,
                               stats_info->rxbyte_cnt);
            DS_3GPP_MSG2_HIGH("prev_txnt: %d, new_txcnt: %d",
                               cached_stats_info.txbyte_cnt,
                               stats_info->txbyte_cnt);
            activity_check = TRUE;
          }
        }
        break;

      default:
        DS_3GPP_MSG0_HIGH("Invalid Operation type");
        break;
    }

    if (activity_check == TRUE)
    {
      break;
    }
  }

  if (op_type == DS_3GPP_CHECK_DATA_ACTIVITY)
  {
    return activity_check;
  }
  return TRUE;
}
#endif /*FEATURE_UE_INITIATED_DORMANCY */
/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTXT_IS_ANY_PDN_GOING_DOWN

DESCRIPTION
  This function returns whether there is a pdn in going down

PARAMETERS
  

DEPENDENCIES
  None

RETURN VALUE
  boolean - whether there is a pdn context in going_down state

SIDE EFFECTS
  None
===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_any_pdn_going_down
( 
  sys_modem_as_id_e_type subs_id 
)
{
  uint8   pdn_cntxt_inst = 0;
  boolean ret_val = FALSE;
  ds_pdn_context_s *pdn_cntxt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_3GPP_MSG0_HIGH("Checking for going down PDN.");

  for(pdn_cntxt_inst=0; pdn_cntxt_inst < DS_3GPP_MAX_PDN_CONTEXT;
      pdn_cntxt_inst++)
  {
    pdn_cntxt_ptr = &(ds_pdn_context_tbl[pdn_cntxt_inst]);
    if ((ds_3gpp_pdn_cntx_validate_pdn_context(pdn_cntxt_ptr))  &&
        (pdn_cntxt_ptr->ds_pdn_context_dyn_p->state == 
                                         DS_PDN_CONTEXT_STATE_GOING_DOWN) &&
        (pdn_cntxt_ptr->ds_pdn_context_dyn_p->subs_id == subs_id))
    {
      DS_3GPP_MSG1_HIGH("PDN instance %d is going down.",
                        pdn_cntxt_inst);
      ret_val = TRUE;
      break;
    }
  }

  return ret_val;
}/* ds_3gpp_pdn_cntxt_is_any_pdn_going_down */

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_GET_TH_MIT_PDN_STATE

DESCRIPTION
  This function returns the 3GPP thermal mitigation pdn state

PARAMETERS
   void            
 
DEPENDENCIES
  None.

RETURN VALUE 
   ds_3gpp_th_mit_state - 3GPP thermal mitigation pdn state

SIDE EFFECTS
  None.

===========================================================================*/
ds3geventmgr_th_mit_action_type ds_3gpp_pdn_cntxt_get_th_mit_pdn_state(void)
{
  ds3geventmgr_th_mit_action_type th_mit_state = DS3GEVENTMGR_TH_MIT_MIN;
/* ----------------------------------------------------------------------- */

  ds3gpp_enter_global_crit_section();
  th_mit_state = ds_3gpp_pdn_context_info.th_mit_state;
  ds3gpp_leave_global_crit_section();

  return th_mit_state;
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_SET_TH_MIT_PDN_STATE

DESCRIPTION
  This function sets the 3GPP thermal mitigation pdn state

PARAMETERS
  th_mit_action - thermal mitigation action to be set            
 
DEPENDENCIES
  None.

RETURN VALUE 
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_set_th_mit_pdn_state
(
  ds3geventmgr_th_mit_action_type th_mit_state
)
{
  ds3gpp_enter_global_crit_section();
  ds_3gpp_pdn_context_info.th_mit_state = th_mit_state;
  ds3gpp_leave_global_crit_section();
}

/*===========================================================================
FUNCTION  DS_3GPP_PDN_CNTXT_IS_PDN_THIRD_PARTY_IMS

DESCRIPTION
  This function checks to see if the PDN is a 3rd party IMS PDN by matching
  the APN name with the APN in the device config for IMS

PARAMETERS
  pdn_cntxt_p - PDN context pointer
  subs_id - subscription for the PDN
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE - PDN is 3rd party IMS PDN
  FALSE - PDN is NOT 3rd part IMS PDN

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_pdn_cntxt_is_pdn_third_party_ims
(
  ds_pdn_context_s       *pdn_cntxt_p,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_sys_apn_info_type  apn_info;
  byte                 *resolved_apn_ptr = NULL;
  boolean               ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Get the APN settings from the device for the IMS APN.
  -----------------------------------------------------------------------*/
  if (ds3gdevmgr_get_apn_settings(&apn_info,subs_id) == TRUE)
  {
    if(apn_info.curr_apn_info[DS_SYS_APN_TYPE_IMS].is_configured 
       == TRUE)
    {
      /*-----------------------------------------------------------------------
       Compare the APN in the PDN against the APN in the device setting.
      -----------------------------------------------------------------------*/
      if(strcasecmp((char*)pdn_cntxt_p->ds_pdn_context_dyn_p->
                      pdp_profile.context.apn,
                    (char*)apn_info.curr_apn_info[DS_SYS_APN_TYPE_IMS].apn_name)
         == 0)
      {
        DS_3GPP_MSG0_MED("Found matching IMS APN from PDN");
        ret_val = TRUE;
      }
      else
      {
        /*---------------------------------------------------------------------
         Compare the resolved APN against the APN in the device setting. It
         can be that UE attached with NULL APN but network resolved and saved
         the resolved APN. In this case, we need to compare the resolved APN.
        ---------------------------------------------------------------------*/
        if (ds_pdn_cntxt_get_resolved_apn_name(pdn_cntxt_p,&resolved_apn_ptr)
            == TRUE)
        {
          if(strcasecmp((char*)resolved_apn_ptr,
                        (char*)apn_info.curr_apn_info
                                [DS_SYS_APN_TYPE_IMS].apn_name)
           == 0)
          {
            DS_3GPP_MSG0_MED("Found matching IMS APN from resolved APN");
            ret_val = TRUE;
          }
        }
        else
        {
          DS_3GPP_MSG0_ERROR("Could not get resolved APN");
        }
      }
    }
    else
    {
      DS_3GPP_MSG0_MED("APN not configured. Ignoring Data Settings");
    }
  }
  else
  {
    DS_3GPP_MSG0_ERROR("Could not get device settings");
  }

  return ret_val;
} /* ds_3gpp_pdn_cntxt_is_pdn_third_party_ims() */

/*===========================================================================
FUNCTION DS_3GPP_PDN_CNTX_GET_CURRENT_LTE_PLMN

DESCRIPTION
  This function fetches the current PLMN Id corresponding to the given Subs Id

PARAMETERS
  plmn_id_ptr - pointer to PLMN id to be returned
  subs_id     - Subscription id

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_pdn_cntx_get_current_lte_plmn
(
  sys_plmn_id_s_type*     plmn_ptr,
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_pdn_cntxt_per_subs_info_type  *hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(plmn_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("Invalid PLMN pointer passed");
    return;
  }

  hndl = ds_3gpp_pdn_context_get_per_subs_info_hndl(subs_id);
  if (!ds_3gpp_pdn_context_validate_per_subs_info_hndl(hndl))
  {
    DS_3GPP_MSG1_ERROR("Per Subs Hndl is Invalid for Subs id(CM): %d", subs_id);
    return;
  }

  memscpy((void*)plmn_ptr,
          sizeof(sys_plmn_id_s_type),
          &(hndl->current_plmn),
          sizeof(sys_plmn_id_s_type));
  return;
} /* ds_3gpp_pdn_cntx_get_current_lte_plmn */

#ifdef FEATURE_EPC_HANDOFF
/*=========================================================================== 
FUNCTION DS_EPS_PDN_CNTXT_CALL_ALLOWED_ATTACH_EX_HDLR

DESCRIPTION
  This function is used to register the call allowed handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler. 

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_reg_call_allowed_ex_hdlr
(
  ds_3gpp_if_call_allowed_hdlr_ex_f_type call_allowed_hdlr_f
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Store the pointer to the call allowed handler provided by the EPC mode
    handler
  ------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntxt_call_allowed_hdlr_ex_f = call_allowed_hdlr_f;
} /* ds_eps_pdn_cntxt_reg_call_allowed_hdlr_ex */

/*===========================================================================
FUNCTION ds_eps_pdn_cntxt_reg_attach_type_ex_hdlr

DESCRIPTION
  This function is used to register the Attach Type handler with the LTE
  Mode Handler. This API is called by EPC Mode Handler.
 
  LTE MH calls the registered function during the Attach process when it needs
  to find out the Attach Type (Initial Attach or Handover Attach)

PARAMETERS
  attach_type_hdlr_f - Pointer to the Attach Type handler.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_pdn_cntxt_reg_attach_type_ex_hdlr
(
  ds_3gpp_if_call_allowed_hdlr_ex_f_type attach_type_hdlr_f
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Store the pointer to the call allowed handler provided by the EPC mode
    handler
  ------------------------------------------------------------------------*/
  ds_3gpp_pdn_cntxt_is_handover_attach_ex_f  = attach_type_hdlr_f;
} /* ds_eps_pdn_cntxt_reg_attach_type_hdlr_ex */

#endif /* #ifdef FEATURE_EPC_HANDOFF */

#endif /* FEATURE_DATA_3GPP */
