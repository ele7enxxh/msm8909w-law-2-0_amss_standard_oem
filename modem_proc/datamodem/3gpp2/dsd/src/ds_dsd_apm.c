/*===========================================================================
                     D S _ D S D _ A P M . C

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c) 2014 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_apm.c#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/28/14    ss     Fixed the issue with attach_profile when apn_disabled
                   flag is set to 0.
09/10/13    rs     Fixed KW errors 
02/14/13   fjia    Add the iWLAN mobility support 
08/22/12    ss     Removed efs create file call as that is moved to dsutil.
05/11/12    sk     Created Module 
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include "assert.h"
#include "ds_profile.h"
#include "mcfg_fs.h"

#include "queue.h"
#include "dsutil.h"
#include "modem_mem.h"
#include "ds_dsd_apm.h"
#include "ds_dsd_dsc.h"
#include "ps_sys_event.h"
#include "ps_sys_ioctl.h"
#include "ds_3gpp_nv_manager.h"
#include "ds_system_heap.h"


#ifdef FEATURE_DATA_LTE
#include "ds_dsd_apm_efs_parser.h"
#include "ds3gsubsmgr.h"
#include "ds3gmgr.h"
#endif /* FEATURE_DATA_LTE */

#include "ds_dsd_ext.h"
#include <stringl/stringl.h>

#ifdef FEATURE_EPC_HANDOFF
#include "ds_epc_util.h"
#endif /* FEATURE_EPC_HANDOFF */

#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_mppm_ext_i.h"
#include "ds_wlan_util.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_DATA_LTE

#ifdef FEATURE_DATA_WLAN_MAPCON
typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  uint8                     len;
  char                      apn_string[DSD_APM_MAX_APN_STRING_LEN];
}ds_dsd_apm_apn_name;
#endif /*FEATURE_DATA_WLAN_MAPCON*/

typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  attach_pdn_type           attach_pdn;
}ds_dsd_apm_attach_pdn;

typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  uint16                    profile_id;
}ds_dsd_apm_profile_id;

typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  ds_profile_info_type      profile_info;
}ds_dsd_apm_profile_info;

typedef struct
{
  sys_modem_as_id_e_type    subs_id;
  boolean                   lte_data_retry;
}ds_dsd_apm_apn_switch_cmd_type;

#ifndef min
#define min(a,b)  ((a) < (b) ? (a):(b))
#endif

/*===========================================================================

                          GLOBAL VARIABLES

===========================================================================*/

static q_type ds_dsd_apm_rules_q[DS3GSUBSMGR_SUBS_ID_COUNT] = {{{NULL}}};

static q_type ds_dsd_apm_attach_profile_q[DS3GSUBSMGR_SUBS_ID_COUNT]
                                                                = {{{NULL}}};

static q_type ds_dsd_apm_apn_priority_q[DS3GSUBSMGR_SUBS_ID_COUNT]
                                                                = {{{NULL}}};
/* 3GPP DS Profile callback function table */
ds_profile_3gpp_cb_func_tbl_type ds_profile_apm_cb_func_tbl = {{NULL}};

/*===========================================================================
Constant that defines the default PLMN blocking Interval in case NAS does 
not notify the changed value to MH 
===========================================================================*/
#define    DSD_APM_PLMN_BLOCKING_INTERVAL               720000

/*===========================================================================
Constant that defines the Invalid PLMN blocking Interval 
===========================================================================*/
#define    DSD_APM_INVALID_T3402_TIMER_VAL              0

/*===========================================================================
Default LTE attach profile 
===========================================================================*/
#define    DEFAULT_LTE_ATTACH_PROFILE                   1

/*---------------------------------------------------------------------------
  Maximum length of any profile parameter value. Refer to
  WDS_MAX_PROFILE_PARAM_VAL_LEN defined in ds_qmi_wds_profile.c
---------------------------------------------------------------------------*/
#define    APM_MAX_PROFILE_PARAM_VAL_LEN               150

/*------------------------------------------------------------------------
This is the singleton instance in UE to remember all information that needs 
to be sub id based.
------------------------------------------------------------------------*/
ds_dsd_apm_subs_info_type    subs_info[DS3GSUBSMGR_SUBS_ID_COUNT] = {{{0}}};  

/*------------------------------------------------------------------------ 
Define a critical section 
------------------------------------------------------------------------*/
rex_crit_sect_type           ds_dsd_apm_critical_section = {{0}};

/*------------------------------------------------------------------------ 
Define a critical section for dsd_apm_apn_switching_enable
------------------------------------------------------------------------*/
rex_crit_sect_type           ds_dsd_apm_crtc_sec_apn_switch = {{0}};

#ifdef FEATURE_DATA_WLAN_MAPCON
/*------------------------------------------------------------------------
The queue to store "LTE capability" info of APNs. If an APN is not LTE capable,
it cannot be used for LTE attach. 
------------------------------------------------------------------------*/
static q_type  ds_dsd_apm_apn_attach_cap_q[DS3GSUBSMGR_SUBS_ID_COUNT]
                                                              = {{{NULL}}};

/*------------------------------------------------------------------------ 
Define a critical section for LTE capable info
------------------------------------------------------------------------*/
rex_crit_sect_type           ds_dsd_apm_lte_capable_critical_section = {{0}};
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_TYPE

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_apn_type
(
  void *item_ptr, 
  void *target_val
);

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_NAME_APN_CAP_Q

DESCRIPTION   This is a utility to compare APN name of PDNs in 
              apn_attach_capable_q 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_apn_name_apn_cap_q
(
  void                        *item_ptr, 
  void                        *target_val
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_NEXT_ATTACH_APN_LTE_CAPABLE

DESCRIPTION    This utility function checks if the next attach APN is capable 
               for LTE attach or not. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_if_next_attach_apn_lte_capable
(
  sys_modem_as_id_e_type                       subs_id,
  uint16                                       profile_id
);

#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION       DS_DSD_APM_RESET_AND_COPY_ATTACH_INFO

DESCRIPTION    This utility function resets the ATTACH PDN info.
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_reset_and_copy_attach_info
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION       DS_DSD_APM_SET_ALL_APN_BLOCKED_VAL

DESCRIPTION    Set the value of if all_apn_blocked
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_all_apn_blocked_val
( 
  sys_modem_as_id_e_type    subs_id,
  boolean                   val 
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ALL_ATTACH_APN_BLOCKED

DESCRIPTION    This utility function checks if all APNs configured in EFS file 
               blocked. It is called whenever getting forbidden APN list from
               LTE MH.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_if_all_attach_apn_blocked
(
  q_type           *priority_q
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_PROFILES_AVAILABLE

DESCRIPTION    This utility function check if all the attach profiles are 
               available or not. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_dsd_apm_if_attach_profiles_available
(
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_APN_AVAILABLE

DESCRIPTION    This utility function checks if attach APN is available and why 
               it is not available. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_dsd_apm_if_attach_apn_available
(
  q_type           *priority_q
);

/*===========================================================================
FUNCTION      DS_DSD_APM_NEXT_ATTACH_PDN_CHAGE

DESCRIPTION   This is a utility changes the next attach pdn
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_next_attach_pdn_change
(
  sys_modem_as_id_e_type    subs_id,
  attach_pdn_type           next_attach_pdn
)
{
  boolean  switching_flag = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(subs_info[subs_indx].next_attach_pdn_info.permanent == TRUE) 
  {
    return;
  }

  switch(next_attach_pdn)
  {
    case ATTACH_PDN_PRIMARY:
      if(subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type !=
                                                            ATTACH_PDN_PRIMARY)
      {
        switching_flag = TRUE;
        rex_enter_crit_sect( &ds_dsd_apm_critical_section );
        subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type = 
                                                          ATTACH_PDN_PRIMARY;
        rex_leave_crit_sect( &ds_dsd_apm_critical_section );
      }
      break;
  
    case ATTACH_PDN_NEXT:
      if(subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type != 
                                                                 ATTACH_PDN_NEXT)
      {
        switching_flag = TRUE;
        rex_enter_crit_sect( &ds_dsd_apm_critical_section );
        subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type = 
                                                               ATTACH_PDN_NEXT;
        rex_leave_crit_sect( &ds_dsd_apm_critical_section );
      }
      break;
  
    default:
        DATA_MSG1(MSG_LEGACY_ERROR,"Invalid PDN:%d", next_attach_pdn);
      break;
  }

  if( switching_flag )
  {
    DATA_MSG1(MSG_LEGACY_HIGH, "Switching APN Type to: %d", next_attach_pdn);
  }
  return;
}/* ds_dsd_apm_next_attach_pdn_change()*/


/*===========================================================================
FUNCTION      DS_DSD_APM_NEXT_ATTACH_PDN_UNSET_PERMANENT

DESCRIPTION   This is a utility unsets permanent flag on next attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_next_attach_pdn_unset_permanent
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(subs_info[subs_indx].next_attach_pdn_info.permanent != FALSE)
  {
    subs_info[subs_indx].next_attach_pdn_info.permanent = FALSE;
  }
  return;
}/*ds_dsd_apm_next_attach_pdn_unset_permanent()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_SET_IP_RULE_REQUIRED

DESCRIPTION   This is a utility to set ip rule required for current attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_set_ip_rule_required
(
  sys_modem_as_id_e_type        subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].attach_pdn_info.ip_rule_required = TRUE;

  return;
}/* ds_dsd_apm_set_ip_rule_required() */

/*===========================================================================
FUNCTION      DS_DSD_APM_UNSET_IP_RULE_REQUIRED_INTERNAL

DESCRIPTION   This is a utility unset rule required for current attach PDN.
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_unset_ip_rule_required_internal
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG1(MSG_LEGACY_MED,
            "ds_dsd_apm_unset_ip_rule_required_internal on sub %d", subs_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].attach_pdn_info.ip_rule_required = FALSE;

  return;
}/* ds_dsd_apm_unset_ip_rule_required() */
/*===========================================================================
FUNCTION      DS_DSD_APM_SET_DEFAULT_APM_RULE

DESCRIPTION   This is a utility to set default APm rule if configure error 
              or APN switch feature is turned off. 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_set_default_apm_rule
(
  sys_modem_as_id_e_type    subs_id,
  uint16                    attach_profile_id,
  q_type*                   apm_rule_q
)
{
  ds_profile_info_type                 prof_pdn_type = {0, 0};
  ds_profile_3gpp_pdp_type_enum_type   pdn_type = 0;
  ds_dsd_apm_data_block*               rule_block = NULL;
  ipRequiredType                       value = 0;
  boolean                              pdn_type_valid = FALSE;
  ds_profile_info_type                 prof_apn_name;
  ds3gsubsmgr_subs_id_e_type           subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_HIGH,
            "Set APM default rule on subs_id:%d attach_profile_id:%d",
            subs_id, attach_profile_id);

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].dsd_apm_default_apm_rule_enabled = TRUE;

  if (apm_rule_q == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"APM rule queue ptr is NULL, return");
    return;
  }
  prof_pdn_type.buf = &pdn_type;
  prof_pdn_type.len = sizeof(pdn_type);

  if ( dsd_apm_get_profile_param_info(   
                attach_profile_id,
                DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                &prof_pdn_type,
                subs_id) == TRUE )
  {
    switch(pdn_type)
    {
      case DS_PROFILE_3GPP_PDP_IP:
        value = ipv4_mandatory_and_ipv6_donotcare;
        pdn_type_valid = TRUE;
        break;
      case DS_PROFILE_3GPP_PDP_IPV6:
        value = ipv6_mandatory_and_ipv4_donotcare;
        pdn_type_valid = TRUE;
        break;
      case DS_PROFILE_3GPP_PDP_IPV4V6:
        value = ipv4_mandatory_or_ipv6_mandatory;
        pdn_type_valid = TRUE;
        break;
      default:
        DATA_MSG1(MSG_LEGACY_ERROR,
                  "pdn type %d is invalid, no default APM rule", pdn_type);
    }
  }
  
  if( pdn_type_valid == TRUE )
  {

    rule_block = (ds_dsd_apm_data_block*)modem_mem_alloc
                                      ( sizeof(ds_dsd_apm_data_block),
                                        MODEM_MEM_CLIENT_DATA );

    if( rule_block == NULL )
    {
      DATA_ERR_FATAL("Allocate memory for default APM rule fail");
      return;
    }

    memset(rule_block, 0, sizeof(ds_dsd_apm_data_block));

    DATA_MSG1(MSG_LEGACY_MED,"Put default APM rule to apm queue, IP rule %d",
              value);
   /*-----------------------------------------------------------------------
      Need to initialize the cause code queue in the APM Rule Block
    -----------------------------------------------------------------------*/
    q_init(&(rule_block->apn_info.cause_code_info.cause_code_info_q));
    rule_block->apn_info.subs_id = subs_id;
    rule_block->apn_info.apn_type = ATTACH_PDN_DEFAULT;
    rule_block->apn_info.AttachPdn.config_rule.config_valid_flag = 
      CONFIG_IP_ADDRESS;
    rule_block->apn_info.AttachPdn.config_rule.ip_address = value;
    rule_block->apn_info.profile_id = attach_profile_id;
   /*-----------------------------------------------------------------------
    Get the attach APN name by checking the 3GPP profile database
    -----------------------------------------------------------------------*/
    prof_apn_name.buf = rule_block->apn_info.apn_name;
    prof_apn_name.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
    if( dsd_apm_get_profile_param_info(
                              attach_profile_id,
                              DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                              &prof_apn_name,
                              subs_id) == TRUE ) 
    {
      rule_block->apn_info.apn_name_len = (uint8)prof_apn_name.len;
    }

    q_link(rule_block, &(rule_block->link));
    q_put(apm_rule_q, &(rule_block->link));
  }
  return;
} /*ds_dsd_apm_set_default_apm_rule()*/

/*===========================================================================
FUNCTION       DS_DSD_APM_FREE_CAUSE_CODE_Q_FOR_RULE_BLOCK

DESCRIPTION    This utility function frees queue items in the queue for
               the passed subs

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_free_cause_code_q_for_rule_block
(
  q_type                               *cause_code_q_ptr
)
{
  ds_dsd_apm_cause_code_info_type      *cause_code_data_block = NULL;
  ds_dsd_apm_cause_code_info_type      *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != cause_code_q_ptr)
  {
    cause_code_data_block = (ds_dsd_apm_cause_code_info_type  *)
                                                  q_check(cause_code_q_ptr);

    while (cause_code_data_block != NULL)
    {
      prev_block = cause_code_data_block;
      cause_code_data_block = (ds_dsd_apm_cause_code_info_type *)
                                        q_next(cause_code_q_ptr,
                                        &(cause_code_data_block->link));
      q_delete(cause_code_q_ptr, &(prev_block->link));
      modem_mem_free (prev_block, MODEM_MEM_CLIENT_DATA);
    }
    q_destroy(cause_code_q_ptr);
  }

  return;
} /* ds_dsd_apm_free_cause_code_q_for_rule_block */

/*===========================================================================
FUNCTION      DS_DSD_APM_DELETE_DEFAULT_APM_RULE

DESCRIPTION   This is a utility to delete default APM rule
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_delete_default_apm_rule 
(
  sys_modem_as_id_e_type    subs_id,
  q_type                    *rule_q 
)
{
  ds_dsd_apm_data_block   *rule_block = NULL;
  ds_dsd_apm_attach_pdn   default_type;

  default_type.subs_id = subs_id;
  default_type.attach_pdn = ATTACH_PDN_DEFAULT;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if( rule_q == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"APM rule queue ptr is NULL, return");
    return;
  }
  subs_info[subs_indx].dsd_apm_default_apm_rule_enabled = FALSE;

  rule_block = (ds_dsd_apm_data_block*)q_linear_search (&ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_apn_type,
                                                         &default_type);

  if( rule_block != NULL )
  {
    DATA_MSG0(MSG_LEGACY_MED,"Delete default APM rule in apm rule queue");
    ds_dsd_apm_free_cause_code_q_for_rule_block(&(rule_block->apn_info.
                                                  cause_code_info.cause_code_info_q));
    q_delete(rule_q, &(rule_block->link));
    modem_mem_free(rule_block, MODEM_MEM_CLIENT_DATA);
  }

  return;
}/*ds_dsd_apm_delete_default_apm_rule*/

/*===========================================================================
FUNCTION      DS_DSD_APM_UPDATE_IP_RULE

DESCRIPTION   This is a utility to update IP rule

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void ds_dsd_apm_update_ip_rule
(
  sys_modem_as_id_e_type              subs_id,
  ds_profile_3gpp_pdp_type_enum_type  attach_pdp_type
)
{
  ds_dsd_apm_data_block  *apm_rule_block_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Update IP rule is called only for default APM rule or Single Queue
    entry scenarios and in this cases APM Rules queue will have only one entry
  -------------------------------------------------------------------------*/
  apm_rule_block_p = (ds_dsd_apm_data_block *)q_check(&ds_dsd_apm_rules_q[subs_id]);

  if( apm_rule_block_p!= NULL )
  {
    /* Update rule only if Subs id matches AND IP address configuration is valid */
    if( (apm_rule_block_p->apn_info.subs_id == subs_id) &&
        (apm_rule_block_p->apn_info.AttachPdn.config_rule.config_valid_flag
           & CONFIG_IP_ADDRESS) )
    {
      switch( attach_pdp_type )
      {
        case DS_PROFILE_3GPP_PDP_IP:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv4_mandatory_and_ipv6_donotcare;
          break;
        case DS_PROFILE_3GPP_PDP_IPV6:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv6_mandatory_and_ipv4_donotcare;
          break;
        case DS_PROFILE_3GPP_PDP_IPV4V6:
          apm_rule_block_p->apn_info.AttachPdn.config_rule.ip_address
            = ipv4_mandatory_or_ipv6_mandatory;
          break;
        default:
          DATA_MSG1(MSG_LEGACY_ERROR,
                    "Invalid attach pdp type:%d", attach_pdp_type);
      }
    }
  }

  return;
} /* ds_dsd_apm_update_ip_rule */

/*===========================================================================
FUNCTION      DS_DSD_APM_NEXT_ATTACH_PDN_SET_PERMANENT

DESCRIPTION   This is a utility sets permanent flag on next attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_next_attach_pdn_set_permanent
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].next_attach_pdn_info.permanent = TRUE;

  return;
}/*ds_dsd_apm_next_attach_pdn_set_permanent()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_NEXT_ATTACH_PDN_CLEAR_PERMANENT

DESCRIPTION   This is a utility sets permanent flag on next attach PDN
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_next_attach_pdn_clear_permanent
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].next_attach_pdn_info.permanent = FALSE;

  return;
}/*ds_dsd_apm_next_attach_pdn_clear_permanent()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_SET_PLMN_BLOCKING_INTERVAL_TIMER

DESCRIPTION   This is a utility sets the PLMN blocking Timer Value 
 
PARAMETERS    timer_val:         T3402 Timer value given by MH 
              subs_id:           Active Data subscription ID 
 
DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_apm_set_plmn_blocking_interval_timer
(
  uint32                        timer_val,
  sys_modem_as_id_e_type        subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].ds_dsd_apm_plmn_blocking_interval = timer_val;
  return;
}/*ds_dsd_apm_set_plmn_blocking_interval_timer()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_GET_PLMN_BLOCKING_INTERVAL_TIMER

DESCRIPTION   This is a utility gets the PLMN blocking Timer Value 
 
PARAMETERS    subs_id : Active data subscription id 
 
DEPENDENCIES  None.

RETURN VALUE  timer_val:         T3402 Timer value given by MH

SIDE EFFECTS  None.
===========================================================================*/
uint32 ds_dsd_apm_get_plmn_blocking_interval_timer
(
  sys_modem_as_id_e_type          subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return DSD_APM_INVALID_T3402_TIMER_VAL;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  return subs_info[subs_indx].ds_dsd_apm_plmn_blocking_interval;
}/*ds_dsd_apm_get_plmn_blocking_interval_timer*/

/*===========================================================================
FUNCTION      DS_DSD_APM_RESET_NW_ATTACH_STATE

DESCRIPTION   This is a utility that resets APM's previous attach state 
 
PARAMETERS    subs_id : Active data subscription id 
 
DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds_dsd_apm_reset_nw_attach_state
(
   sys_modem_as_id_e_type          subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].attach_pdn_info.ipv4_address = DS_DSD_APM_NOT_RCVD;
  subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address = DS_DSD_APM_NOT_RCVD;
  subs_info[subs_indx].attach_pdn_info.ipv6_address = DS_DSD_APM_NOT_RCVD;
  subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address = DS_DSD_APM_NOT_RCVD;

}/*ds_dsd_apm_reset_nw_attach_state*/

/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_NAME

DESCRIPTION   Utility function to compare apn name in queue search.
 
DEPENDENCIES           
           
PARAMETERS    item_ptr: block of apn_list (type: ds_dsd_apm_apn_block_info_type)
              target_val: block of priority queue (type: ds_dsd_apm_apn_block_info_type) 

RETURN VALUE  1: the items are equal
              0: the items are not equal

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_apn_name
(
  void *item_ptr, 
  void *target_val
)
{
  int                             ret_val = FALSE;
  apn_priority_data_block         *apn_block;
  ds_dsd_apm_apn_block_info_type  *forbidden_apn_block;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL )
  {
    return ret_val;
  }

  apn_block = (apn_priority_data_block*)item_ptr;
  forbidden_apn_block = (ds_dsd_apm_apn_block_info_type*)target_val;

  if (apn_block->apn_block_info.subs_id == forbidden_apn_block->subs_id)
  {
    if( apn_block->apn_block_info.apn_len == forbidden_apn_block->apn_len)
    {
      if ( apn_block->apn_block_info.apn_len == 0) 
      {
        ret_val = TRUE;
      }
      else
      {
        if( 0 == strncasecmp( apn_block->apn_block_info.apn_name,
                              forbidden_apn_block->apn_name,
                              apn_block->apn_block_info.apn_len) )
        {
          ret_val = TRUE;
        }
      }
    }
  }

  return ret_val;
} /* ds_dsd_apm_compare_apn_name() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_APN_NAME_IP_TYPE_BLOCKED

DESCRIPTION    This utility function to check if a certain type of APN 
               is blocked.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_if_apn_name_ip_type_blocked
(
  sys_modem_as_id_e_type                      subs_id,
  char                                       *name,
  uint16                                      name_len,
  uint16                                      ip_type,
  boolean                                    *blocked
)
{
  apn_priority_data_block                     *apn_info_block = NULL;
  ds_dsd_apm_apn_block_info_type              temp_apn_info;
  ds3gsubsmgr_subs_id_e_type       subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( name == NULL || blocked == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Passed name or blocked buff is NULL");
    return FALSE;
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*------------------------------------------------------------------
   This function checks for a given APN whether a partiuclar IP type
   has been blocked or not.
   
   We extract the entry from APN priority_q and perform the required
   checks
  -------------------------------------------------------------------*/ 
  memset(&temp_apn_info, 0, sizeof(ds_dsd_apm_apn_block_info_type));
  temp_apn_info.subs_id = subs_id;
  temp_apn_info.apn_len = name_len;
  strlcpy(temp_apn_info.apn_name, 
          name, 
          DS_DSD_APM_MAX_APN_STRING_LEN);

  apn_info_block = 
    (apn_priority_data_block*)q_linear_search(&ds_dsd_apm_apn_priority_q[subs_indx],
                                              ds_dsd_apm_compare_apn_name,
                                              &(temp_apn_info));
  if ( apn_info_block == NULL ) 
  {
    *blocked = FALSE;
  }
  else
  {
    if( (ip_type & ~(apn_info_block->apn_block_info.ip_type_blocked)) == 0 ) 
    {
      *blocked = TRUE;
    }
    else
    {
      *blocked = FALSE;
    }
  }

  DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,"APN: %s, ip_type: %d ",
                       name, ip_type);
  DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_apm is apn name ip type blocked: %d ",*blocked);

  return TRUE;

}/* ds_dsd_apm_if_apn_name_ip_type_blocked() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_PROFILE_ID_AVAILABLE

DESCRIPTION    This utility function to check if a certain profile is available.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info  ds_dsd_apm_if_profile_id_available
( 
  sys_modem_as_id_e_type               subs_id,
  uint16                               profile_id
)
{
  ds_profile_info_type                 prof_apn_name = {0, 0};
  char                                 apn_name
                                       [DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  ds_profile_info_type                 prof_pdn_type = {0, 0};
  ds_profile_3gpp_pdp_type_enum_type   pdn_type = 0;
  uint16                               ip_type = 0;
  boolean                              blocked = FALSE;
  attach_apn_availability_info         prof_avail = 0;
 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_apm_if_profile_id_available on sub %d: profile_id %d",
            subs_id, profile_id);

  prof_pdn_type.buf = &pdn_type;
  prof_pdn_type.len = sizeof(pdn_type);

  if( dsd_apm_get_profile_param_info (
                         profile_id,
                         DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                         &prof_pdn_type,
                         subs_id ) == TRUE )
  {
    switch( pdn_type ) 
    {
      case DS_PROFILE_3GPP_PDP_IP:
        ip_type = V4_PDN_TYPE;
        break;
      case DS_PROFILE_3GPP_PDP_IPV6:
        ip_type = V6_PDN_TYPE;
        break;
      case DS_PROFILE_3GPP_PDP_IPV4V6:
        ip_type = V4_V6_PDN_TYPE;
        break;
      default:
        ip_type = V4_V6_PDN_TYPE;
        break;
    }
  }
  else
  {
    ip_type = V4_V6_PDN_TYPE;
  }

  memset(apn_name, 0, DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
  prof_apn_name.buf = apn_name;
  prof_apn_name.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
  if( dsd_apm_get_profile_param_info( profile_id,
                                 DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                                 &prof_apn_name,
                                 subs_id) == TRUE ) 
  {



    if( ds_dsd_apm_if_apn_name_ip_type_blocked(subs_id,
                                               prof_apn_name.buf,
                                               prof_apn_name.len,
                                               ip_type,
                                               &blocked) == TRUE )
    {
      if ( blocked == TRUE ) 
      {
        prof_avail = DS_DSD_APM_ATTACH_APN_BLOCKED;
      }
      else
      {
        prof_avail = DS_DSD_APM_ATTACH_APN_AVAILABLE;
      }
    }
    else
    {
      prof_avail = DS_DSD_APM_PROF_CONFIG_ERROR;
    }
    
    DATA_MSG_SPRINTF_3(MSG_LEGACY_MED,"ds_dsd_apm profile id: %d, APN name: %s,"
                       " Length: %d, ",
                       profile_id, prof_apn_name.buf, prof_apn_name.len);
    DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_apm check if it is blocked %d",blocked);
  }
  else
  {
    prof_avail = DS_DSD_APM_PROF_CONFIG_ERROR;
  }

  return prof_avail;
}/* ds_dsd_apm_if_profile_id_available() */

/*===========================================================================
FUNCTION      DS_DSD_APM_IS_APN_BLOCKED

DESCRIPTION   This is a utility that checks if the APN for a particular prof 
              is blocked or not 
 
PARAMETERS    profile_id:        Profile whose APN blocked status has to be 
                                 checked 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:              If APN is blocked from attach 
              FALSE:             If APN is not blocked

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_apm_is_apn_blocked
(
  uint16                            profile_id,
  sys_modem_as_id_e_type            subs_id
)
{
  boolean                           ret_val = FALSE;
  attach_apn_availability_info      avail_info = 
                                           DS_DSD_APM_ATTACH_APN_AVAILABLE;
/*-------------------------------------------------------------------------*/
  avail_info = ds_dsd_apm_if_profile_id_available (subs_id, profile_id);

  if(avail_info == DS_DSD_APM_ATTACH_APN_BLOCKED)
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION      DS_DSD_APM_IS_APN_DISABLED

DESCRIPTION   This is a utility that checks if the APN for a particular prof 
              is disabled or not 
 
PARAMETERS    profile_id:        Profile whose APN disable status has to be 
                                 checked 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:              If APN is disabled 
              FALSE:             If APN is not disabled 

SIDE EFFECTS  None.
===========================================================================*/
boolean ds_dsd_apm_is_apn_disabled
(
   uint16                          profile_id,
   sys_modem_as_id_e_type          subs_id
)
{
  attach_profile_data_block        *profile_block = NULL;
  boolean                           ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type      subs_indx;
/*-------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  profile_block = (attach_profile_data_block *)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);

  while (profile_block != NULL)
  {
    if (profile_block->attach_profile.profile == profile_id &&
        profile_block->attach_profile.apn_disabled == TRUE &&
        profile_block->attach_profile.subs_id == subs_id)
    {
      ret_val = TRUE;
      break;
    }
    profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                      &(profile_block->link));
  }

  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  return ret_val;
}


/*===========================================================================
FUNCTION       DS_DSD_APM_SET_GENERIC_SWITCHING_FLAG

DESCRIPTION    To Enable or disable Generic APN Switching 
 
PARAMETERS     subs_id: Currently active PS subscription 
               generic_apn_switch: Flag containing information about enable/
                                   disable status
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_generic_switching_flag
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   generic_apn_switch
)
{
  ds3gsubsmgr_subs_id_e_type      subs_indx;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  subs_info[subs_indx].dsd_apm_generic_switching_status = generic_apn_switch;
  DATA_MSG2(MSG_LEGACY_MED,
            "Generic APN Switch on sub %d : %d", subs_id, generic_apn_switch);
  return;
}/* ds_dsd_apm_set_generic_switching_flag */

/*===========================================================================
FUNCTION      DS_DSD_APM_ACTION_RULE

DESCRIPTION   This is a utility to run action
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_action_rule
(
  sys_modem_as_id_e_type          subs_id,
  uint16                          action
)
{
  boolean                         switch_to_primary = FALSE;
  boolean                         switch_to_next = FALSE;
  attach_pdn_type                 next_pdn_type = ATTACH_PDN_DEFAULT;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if(action & (uint16)ACTION_CHANGE_TO_PRIMARY)
  {
    switch_to_primary = TRUE;
  }

  if(action & (uint16)ACTION_CHANGE_TO_NEXT)
  {
    switch_to_next = TRUE;
  }
  
  if(switch_to_primary == TRUE)
  {
    next_pdn_type = ATTACH_PDN_PRIMARY;
  }
  else
  {
    if(switch_to_next == TRUE)
    {
      next_pdn_type = ATTACH_PDN_NEXT;
    }
  }

  DATA_MSG3(MSG_LEGACY_HIGH,
            "Switching APN,action_rule: 0x%x on sub %d ,Next APN Type: %d",
            action, subs_id, next_pdn_type);

  ds_dsd_apm_next_attach_pdn_change(subs_id, next_pdn_type);

  if((action  & (uint16)ACTION_INIT_DETACH) &&
     (action  & (uint16)ACTION_BLOCK_PLMN))
  {
    ds_dsd_dsc_command(subs_id, DS_DSC_PLMN_BLOCKING_AND_PS_DETACH,
                        ds_dsd_apm_get_plmn_blocking_interval_timer(subs_id));
  }
  else if((action  & (uint16)ACTION_INIT_DETACH))
  {
    ds_dsd_dsc_command(subs_id, DS_DSC_PS_DETACH,
                       DSD_APM_INVALID_T3402_TIMER_VAL);
  }
  else if((action  & (uint16)ACTION_BLOCK_PLMN))
  {
    ds_dsd_dsc_command(subs_id, DS_DSC_PLMN_BLOCKING,
                       ds_dsd_apm_get_plmn_blocking_interval_timer(subs_id));
  }
  return;
}/* ds_dsd_apm_action_rule()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_TYPE

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_apn_type
(
  void *item_ptr, 
  void *target_val
)
{
  ds_dsd_apm_data_block   *rule_block = NULL;
  ds_dsd_apm_attach_pdn   *apn_type;
  int                      ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  rule_block = (ds_dsd_apm_data_block*)item_ptr;
  apn_type = (ds_dsd_apm_attach_pdn*)target_val;

  if(rule_block->apn_info.subs_id == apn_type->subs_id)
  {
    if( rule_block->apn_info.apn_type == apn_type->attach_pdn)
    {
      ret_val = TRUE;
    }
  }
  return ret_val;
  
}/* ds_dsd_apm_compare_apn_type() */
/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_NAME_RULE_Q

DESCRIPTION   This is a utility to compare APN name with Rule queue APN name
 
DEPENDENCIES  None.

RETURN VALUE  TRUEL If a match occurs 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
int ds_dsd_apm_compare_apn_name_rule_q
(
  void                   *item_ptr, 
  void                   *target_val
)
{
  ds_dsd_apm_data_block  *rule_block = NULL;
  char                   *apn_name = NULL;
  int                     ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL ) 
  {
    return ret_val;
  }
  rule_block = (ds_dsd_apm_data_block *)item_ptr;
  apn_name = (char *)target_val;

  if(strncasecmp(apn_name, rule_block->apn_info.apn_name,
                 rule_block->apn_info.apn_name_len) == 0)
  {
    ret_val = TRUE;
  }

  return ret_val;
}/*ds_dsd_apm_compare_apn_name_rule_q*/


#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_APN_NAME_APN_CAP_Q

DESCRIPTION   This is a utility to compare APN name of PDNs in 
              apn_attach_capable_q 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_apn_name_apn_cap_q
(
  void *item_ptr, 
  void *target_val
)
{
  apn_attach_capable_block       *apn_block = NULL;
  ds_dsd_apm_apn_name            *apn_name = NULL;
  int                             ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL ) 
  {
    return ret_val;
  }

  apn_block = (apn_attach_capable_block*)item_ptr;
  apn_name = (ds_dsd_apm_apn_name*)target_val;

  if(apn_block->subs_id == apn_name->subs_id) 
  {
    if( (apn_block->apn_attach_capable_info.apn_name_len == 0) &&
        (apn_name->len == 0) ) 
    {
      ret_val = TRUE;
    }

    if ( (apn_block->apn_attach_capable_info.apn_name_len == apn_name->len)&& 
         (strncasecmp(apn_block->apn_attach_capable_info.apn_name,
                      apn_name->apn_string,
                      apn_name->len) == 0)) 
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
} /* ds_dsd_apm_compare_apn_name_apn_cap_q() */
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_PROFILE_ID

DESCRIPTION   This is a utility to compare APN type of PDNs in APM rule           
              queue 
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_profile_id
(
  void *item_ptr, 
  void *target_val
)
{
  ds_dsd_apm_data_block   *rule_block = NULL;
  ds_dsd_apm_profile_id   *subs_profile_id = NULL;
  boolean                  ret_val = FALSE; 
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  rule_block = (ds_dsd_apm_data_block*)item_ptr;
  subs_profile_id = (ds_dsd_apm_profile_id *)target_val;

  if (rule_block->apn_info.subs_id == subs_profile_id->subs_id) 
  {
    if( rule_block->apn_info.profile_id == subs_profile_id->profile_id )
    {
      ret_val = TRUE;
    }
  }

  return ret_val;
  
}/* ds_dsd_apm_compare_profile_id() */

/*===========================================================================
FUNCTION      DS_DSD_APM_RULE

DESCRIPTION   This is a utility to run rules
 
DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_rule
(
  sys_modem_as_id_e_type    subs_id,
  ds_dsd_apm_apn_rule       rule
)
{
  apn_info_type          *apn_info_ptr = NULL; 
  attach_pdn_info_type   *attach_pdn_info_ptr = NULL;
  ds_dsd_apm_data_block  *rule_block = NULL;
  ds_dsd_apm_attach_pdn  attach_pdn;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  attach_pdn_info_ptr = &subs_info[subs_indx].attach_pdn_info;
  attach_pdn.subs_id = subs_id;
  attach_pdn.attach_pdn = attach_pdn_info_ptr->pdn_type;

  rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_apn_type,
                                                           &attach_pdn);
  if( rule_block == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Current attach PDN does not have rule");
    return;
  }
  apn_info_ptr = &(rule_block->apn_info);

  DATA_MSG3(MSG_LEGACY_MED,"Current attach PDN type %d, rule %d , subs id %d", 
            attach_pdn_info_ptr->pdn_type,
            rule,
            subs_id);

  switch(rule)
  {
    case  DSD_DSD_APM_RULE_IP_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
          CONFIG_IP_ADDRESS))
      {
        DATA_MSG2(MSG_LEGACY_MED,
                  "IP RULE CONFIG SUPPORTED, ip address v4 %d, v6 %d",
                  attach_pdn_info_ptr->ipv4_address,
                  attach_pdn_info_ptr->ipv6_address);
        switch(apn_info_ptr->AttachPdn.config_rule.ip_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_RCVD_FAIL) || 
               (attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_RCVD_FAIL))
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;
          
          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_RCVD_FAIL))
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.ip_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_LOW,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.ip_address);
      }

      break;
    case DS_DSD_APM_RULE_PCSCF_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF))
      {
        DATA_MSG2(MSG_LEGACY_MED,"PCSCF rule CONFIG SUPPORTED, "
                  "pcscf address v4 %d, v6 %d ",
                  attach_pdn_info_ptr->ipv4_pcscf_address,
                  attach_pdn_info_ptr->ipv6_pcscf_address);
        switch(apn_info_ptr->AttachPdn.config_rule.pcscf_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_pcscf_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_pcscf_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_pcscf_address == DS_DSD_APM_RCVD_FAIL) || 
               (attach_pdn_info_ptr->ipv6_pcscf_address == DS_DSD_APM_RCVD_FAIL))
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;
          
          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_pcscf_address == DS_DSD_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_pcscf_address == DS_DSD_APM_RCVD_FAIL))
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.pcscf_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_HIGH,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.pcscf_address);
      }
      break;

    case DS_DSD_APM_RULE_OOS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_OOS)
          == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.out_of_service);
      }
      break;

    case DS_DSD_APM_RULE_PLMN:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PLMN)
         == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.plmn_change);
      }
      break;

    case DS_DSD_APM_RULE_SYS_CHANGE:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_SYS_CHG) == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.sys_change);
      }
      break;

    case DS_DSD_APM_RULE_ATTACH_SUCCESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_ATTACH_SUCCESS) == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.attach_success);
      }
      break;

    case DS_DSD_APM_RULE_ATTACH_FAIL:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_ATTACH_FAIL) == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.attach_fail);
      }
      break;

    case DS_DSD_APM_RULE_DETACH_NORMAL:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag 
          & CONFIG_DETACH_NORMAL) == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.detach_normal);
      }
      break;

    case DS_DSD_APM_RULE_RESET_APN:
      if((q_cnt(&(apn_info_ptr->cause_code_info.cause_code_info_q)) > 0) == FALSE)
      {
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->AttachPdn.
                             action_rule.reset_attach_apn);
      }
      break;
      
    case  DS_DSD_APM_RULE_DNS_ADDRESS:
      if((apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
          CONFIG_DNS_ADDRESS))
      {
        DATA_MSG3(MSG_LEGACY_MED,
                  "DNS Address CONFIG SUPPORTED, dns addr v4 %d, v6 %d reqd %d",
                  attach_pdn_info_ptr->ipv4_dns_address,
                  attach_pdn_info_ptr->ipv6_dns_address,
                  apn_info_ptr->AttachPdn.config_rule.dns_address);

        switch(apn_info_ptr->AttachPdn.config_rule.dns_address)
        {
          case ipv4_mandatory_and_ipv6_donotcare:
            if(attach_pdn_info_ptr->ipv4_dns_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;
          case ipv6_mandatory_and_ipv4_donotcare:
            if(attach_pdn_info_ptr->ipv6_dns_address == DS_DSD_APM_RCVD_FAIL)
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;
          case ipv4_mandatory_and_ipv6_mandatory:
            if(
               (attach_pdn_info_ptr->ipv4_dns_address == DS_DSD_APM_RCVD_FAIL)||
               (attach_pdn_info_ptr->ipv6_dns_address == DS_DSD_APM_RCVD_FAIL)
               )
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;

          case ipv4_mandatory_or_ipv6_mandatory:
            if((attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_RCVD_FAIL) && 
               (attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_RCVD_FAIL))
            {
              ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                     AttachPdn.action_rule.dns_address);
            }
            break;

          default:
            break;
        }
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_HIGH,"CONFIG NOT SUPPORTED -- DEFAULT RULE");
        ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                               AttachPdn.action_rule.dns_address);
      }

    break;
      
    case DS_DSD_APM_RULE_PCSCF_DNS_ADDRESS:
      if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                             CONFIG_PCSCF_DNS_ADDRESS)
      {

        switch(apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address)
        {
          case ipv4_mandatory_or_ipv6_mandatory:
            ds_dsd_apm_action_rule(subs_id, apn_info_ptr->
                                   AttachPdn.action_rule.pcscf_or_dns_address);
            break;

          default:
            break;
        }
      }
      break;

    default:
      DATA_MSG1(MSG_LEGACY_HIGH,"Invalid rule :%d", rule);
      break;
  }
  return;
}/* ds_dsd_apm_rule()*/

/*===========================================================================
FUNCTION      DSD_APM_COMPARE_PROFILE_AND_RULE

DESCRIPTION   Utility function to compare apn name in queue search.
 
DEPENDENCIES           
           
PARAMETERS    item_ptr: block of APM rule (type: ds_dsd_apm_data_block)
              target_val: apn info in profile (type: ds_profile_info_type) 

RETURN VALUE  1: the items are equal
              0: the items are not equal

SIDE EFFECTS  
===========================================================================*/
int ds_dsd_apm_compare_profile_and_rule
(
  void *item_ptr, 
  void *target_val
)
{
  int                             ret_val = FALSE;
  ds_dsd_apm_data_block           *rule_block;
  ds_dsd_apm_profile_info         *apn_block;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( item_ptr == NULL || target_val == NULL )
  {
    return ret_val;
  }

  rule_block = (ds_dsd_apm_data_block*)item_ptr;
  apn_block = (ds_dsd_apm_profile_info*)target_val;

  if (rule_block->apn_info.subs_id == apn_block->subs_id) 
  {
    if( rule_block->apn_info.apn_name_len == apn_block->profile_info.len)
    {
      if( 0 == strncasecmp( rule_block->apn_info.apn_name,
                            apn_block->profile_info.buf,
                            apn_block->profile_info.len) )
      {
        ret_val = TRUE;
      }
    }
  }

  return ret_val;
} /* ds_dsd_apm_compare_profile_and_rule() */

/*===========================================================================
FUNCTION      DS_DSD_APM_COMPARE_PROFILE_ID_PROF_Q

DESCRIPTION   This is a utility to compare the attach profile ID in attach 
              profile queue 
 
DEPENDENCIES  None.

RETURN VALUE  TRUE:: If a match is found 
              FALSE:: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
int ds_dsd_apm_compare_profile_id_prof_q
(
  void                          *item_ptr, 
  void                          *target_val
)
{
  attach_profile_data_block     *profile_block = NULL;
  ds_dsd_apm_profile_id         *subs_profile_id = NULL;
  int                      ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( (item_ptr == NULL) || (target_val == NULL) )
  {
    return ret_val;
  }

  profile_block = (attach_profile_data_block  *)item_ptr;
  subs_profile_id = (ds_dsd_apm_profile_id *)target_val;

  if (profile_block->attach_profile.subs_id == subs_profile_id->subs_id) 
  {
    if(profile_block->attach_profile.profile == subs_profile_id->profile_id )
    {
      ret_val = TRUE;
    }
  }
  
  return ret_val;
  
}/* ds_dsd_apm_compare_profile_id_prof_q() */

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_LAST_PROF_BLOCK_PER_SUBS

DESCRIPTION    This utility function traverses the profile queue to return a 
               pointer to last block for a particular subscription
 
PARAMETERS     subs_id: Currently active Data subscription
 
DEPENDENCIES   NONE

RETURN VALUE   last_block: Last rule block in the queue for given subs

SIDE EFFECTS   NONE
===========================================================================*/
attach_profile_data_block* ds_dsd_apm_get_last_prof_block_per_subs
(
   sys_modem_as_id_e_type                  subs_id
)
{
   attach_profile_data_block              *prof_block = NULL;
   attach_profile_data_block              *last_block = NULL;
   ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return NULL;
   }
   subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

   rex_enter_crit_sect(&ds_dsd_apm_critical_section);

   prof_block = (attach_profile_data_block *)q_check (&ds_dsd_apm_attach_profile_q[subs_indx]);

   while(prof_block != NULL)
   {
     if(prof_block->attach_profile.subs_id == subs_id)
     {
       last_block = prof_block;
     }

     prof_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx], 
                                                  &(prof_block->link));
   }

   rex_leave_crit_sect(&ds_dsd_apm_critical_section);
   return last_block;
}

/*===========================================================================
FUNCTION      DS_DSD_APM_FILL_APN_DISABLE_INFO

DESCRIPTION   This function populates the values of APN Disable Flag in attach 
              profile queue 
 
PARAMETERS    profile_block: attach profile block whose APN disable flag is to 
                             be filled 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modification of attach_profile_q contents
===========================================================================*/
void ds_dsd_apm_fill_apn_disable_info
(
  sys_modem_as_id_e_type  subs_id
)
{
   ds_profile_info_type                           profile_info;
   attach_profile_data_block                     *profile_block = NULL;
   ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return;
   }
   subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);
  profile_block = (attach_profile_data_block *)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);
 
  while(profile_block != NULL)
  {
    memset(&profile_info, 0, sizeof(ds_profile_info_type));

    profile_info.buf = &(profile_block->attach_profile.apn_disabled);
    profile_info.len = sizeof(boolean);

    if ( dsd_apm_get_profile_param_info(profile_block->attach_profile.profile,
                            DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG,
                            &profile_info,
                            subs_id) == TRUE )
    {
      profile_block->attach_profile.apn_disabled = *((boolean *)profile_info.
                                                                        buf);
    }

    profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                        &(profile_block->link));
  }
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
}

/*===========================================================================
FUNCTION      DS_DSD_APM_MATCH_ATTACH_PROFILE_AND_RULE

DESCRIPTION   Try to match APM rule with attach profile based on APN name 
              and populate APN class to main APM structure queue. 
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_match_attach_profile_and_rule
(
  sys_modem_as_id_e_type    subs_id,
  q_type                    *rule_q,
  q_type                    *profile_q,
  uint16                    *error_code
)
{
  ds_dsd_apm_data_block        *rule_entry = NULL;
  attach_profile_data_block    *prof_entry = NULL;
  uint16                       profile_id;
  char                         apn_name[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  uint8                        apn_type;
  ds_dsd_apm_profile_info      subs_profile_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( (profile_q == NULL) || (rule_q == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"queue is empty");
    *error_code |= EFS_PROFILE_RULE_MISMATCH;
    return;
  }

  prof_entry = (attach_profile_data_block* )q_check(profile_q);
  if( prof_entry == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"No valid attach profile id");
    *error_code |= EFS_PROFILE_RULE_MISMATCH;
    return;
  }

  while( prof_entry != NULL)
  {
    if(prof_entry->attach_profile.subs_id == subs_id)
    {
      profile_id = prof_entry->attach_profile.profile;

      memset(apn_name, '\0', DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
      subs_profile_info.subs_id = subs_id;
      subs_profile_info.profile_info.buf = apn_name;
      subs_profile_info.profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
      if ( dsd_apm_get_profile_param_info(profile_id,
                                    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                                    &subs_profile_info.profile_info,
                                    subs_id) == TRUE )
      {
        rule_entry = (ds_dsd_apm_data_block*)q_linear_search(rule_q,
                                         ds_dsd_apm_compare_profile_and_rule,
                                         &subs_profile_info);
        if( rule_entry != NULL )
        {
          DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                             "Found the matching rule with apn name %s ,profile %d",
                             apn_name,profile_id);
          /*------------------------------------------------------------------
           Find matching PDN rules, and save profile id and PDN type
           to APM rule queue
          -------------------------------------------------------------------*/
          apn_type = 0;
          subs_profile_info.profile_info.buf = &apn_type;
          subs_profile_info.profile_info.len = sizeof(apn_type);
          
          if( dsd_apm_get_profile_param_info (profile_id,
                                        DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS,
                                        &subs_profile_info.profile_info, 
                                        subs_id) == TRUE)
          {
            rule_entry->apn_info.apn_type =
              (attach_pdn_type) (*((uint8 *)subs_profile_info.profile_info.buf));
            rule_entry->apn_info.profile_id = profile_id;
            DATA_MSG2(MSG_LEGACY_MED,
                      "Get the apn type %d to APM rule with profile id %d ", 
                      rule_entry->apn_info.apn_type,
                      rule_entry->apn_info.profile_id);
          }
          else
          {
            *error_code |= EFS_APM_RULE_APN_CLASS_ERR;
            break;
          }
        }
        else
        {
          *error_code |= EFS_PROFILE_RULE_MISMATCH;
          break;
        }
      }
      else
      {
        *error_code |= EFS_PROFILE_RULE_MISMATCH;
        break;
      }
    }
    prof_entry = q_next(profile_q, &(prof_entry->link));
  }

  if( *error_code )
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"Attach profile and APM rule in EFS is not "
              "matching, no APN switching",
              *error_code);
  }
  return;
}/*ds_dsd_apm_match_attach_profile_and_rule*/

/*===========================================================================
FUNCTION      DS_DSD_APM_CONFIG_GET_ATTACH_PROF_NUM

DESCRIPTION   returns the number of a attach profiles in the queue 
 
DEPENDENCIES  None

RETURN VALUE  uint16 - Number of attach profiles in the queue

SIDE EFFECTS  None
===========================================================================*/
uint16 ds_dsd_apm_config_get_attach_prof_num
(
  sys_modem_as_id_e_type  subs_id,
  q_type                  *attach_profile_q
)
{
  uint16                       num_attach_profs = 0;
  attach_profile_data_block   *attach_prof_block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);
  if(attach_profile_q != NULL)
  {
     attach_prof_block = (attach_profile_data_block *)q_check(attach_profile_q);

     while(attach_prof_block != NULL)
     {
       if(attach_prof_block->attach_profile.subs_id == subs_id)
       {
         num_attach_profs++;
       }
       attach_prof_block = (attach_profile_data_block *)
                             q_next(attach_profile_q,&(attach_prof_block->link));
     }
  }
  DATA_MSG1(MSG_LEGACY_MED,
            "number of attach profs %d ", num_attach_profs);
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);   
  return num_attach_profs;
}/* ds_dsd_apm_config_get_attach_prof_num() */

/*===========================================================================
FUNCTION      DS_DSD_APM_CONFIG_GET_APM_RULES_NUM

DESCRIPTION   returns the number of a attach rules in the queue 
 
DEPENDENCIES  None

RETURN VALUE  uint16 - Number of attach rules in the queue

SIDE EFFECTS  None
===========================================================================*/
uint16 ds_dsd_apm_config_get_apm_rules_num
(
  sys_modem_as_id_e_type   subs_id,
  q_type                  *rule_q
)
{
  uint16                 num_rule_q = 0;
  ds_dsd_apm_data_block  *block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( rule_q != NULL)
  {
    block = (ds_dsd_apm_data_block*)q_check(rule_q);
    while (block != NULL)
    {
        num_rule_q++;
      block = (ds_dsd_apm_data_block*)q_next(rule_q, &(block->link));
    }
  }

  DATA_MSG1(MSG_LEGACY_MED,
            "number of attach rules %d ", num_rule_q);  
  return num_rule_q;
}/* ds_dsd_apm_config_get_apm_rules_num() */

/*===========================================================================
FUNCTION      DS_DSD_APM_CONFIG_VALIDATE_ATTACH_PDN_CLASS

DESCRIPTION   Check whether the passed PDN class is Class 1 or Class 3 
 
PARAMETERS    pdn_type: PDN Class to be validated 
 
DEPENDENCIES  None

RETURN VALUE  TRUE: If the passed PDN type is Class 1/Class 3 
              FALSE: Otherwise 

SIDE EFFECTS  None
===========================================================================*/
boolean ds_dsd_apm_config_validate_attach_pdn_class
(
  attach_pdn_type           pdn_type
)
{
  boolean                   ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (pdn_type !=  ATTACH_PDN_PRIMARY) &&
       (pdn_type !=  ATTACH_PDN_NEXT) )
  {
    DATA_MSG1(MSG_LEGACY_MED,"APN class %d is not valid",
              pdn_type);
    ret_val = FALSE;
  }
  return ret_val;
} /* ds_dsd_apm_config_validate_attach_pdn_class */

/*===========================================================================
FUNCTION       DS_DSD_APM_VALIDATE_QUEUE_CNT_FOR_SUBS

DESCRIPTION    This utility function checks to see if queue elements exist 
               for the passed subs_id 
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE if there are queue items for passed sub 
               FALSE if no queue items exits for passed sub 

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_validate_queue_cnt_for_subs
(
  sys_modem_as_id_e_type    subs_id,
  q_type                    *rule_q
)
{
  attach_profile_data_block  *profile_block = NULL;
  uint8                       cnt = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  profile_block = (attach_profile_data_block*)q_check(rule_q);

  if (NULL != profile_block) 
  {
    while (profile_block != NULL) 
    {
      if (profile_block->attach_profile.subs_id == subs_id) 
      {
        cnt++;
      }

      profile_block = (attach_profile_data_block*)q_next(rule_q, 
                                                   &(profile_block->link));
    }

    if (cnt > 0) 
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  return FALSE;
}/*ds_dsd_apm_validate_queue_cnt_for_subs*/

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_SUBS_ID_FROM_PROFILE_ID

DESCRIPTION    This utility function gets the sub id associated with the 
               profile id of the apm rule 
 
DEPENDENCIES   NONE

RETURN VALUE   sub id for the passed profile id

SIDE EFFECTS   NONE
===========================================================================*/
sys_modem_as_id_e_type ds_dsd_apm_get_subs_id_from_profile_id
(
  uint16                        profile_id,
  q_type                       *rule_q
)
{
  attach_profile_data_block     *profile_block = NULL;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
  profile_block = (attach_profile_data_block*)q_check(rule_q);

  if (NULL != profile_block) 
  {
    while (profile_block != NULL) 
    {
      if (profile_block->attach_profile.profile == profile_id) 
      {
        subs_id = profile_block->attach_profile.subs_id;
        break;
      }

      profile_block = (attach_profile_data_block*)q_next(rule_q, 
                                                   &(profile_block->link));
    }
    return subs_id;
  }

  return subs_id;
}/*ds_dsd_apm_get_subs_id_from_profile_id*/

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_LAST_RULE_BLOCK_PER_SUBS

DESCRIPTION    This utility function traverses the rule queue to return a 
               pointer to last block for a particular subscription
 
PARAMETERS     subs_id: Currently active Data subscription
 
DEPENDENCIES   NONE

RETURN VALUE   last_block: Last rule block in the queue for given subs

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_apm_data_block* ds_dsd_apm_get_last_rule_block_per_subs
(
   sys_modem_as_id_e_type                  subs_id
)
{
   ds_dsd_apm_data_block                  *rule_block = NULL;
   ds_dsd_apm_data_block                  *last_block = NULL;
   ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
   {
     return NULL;
   }
   subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

   rule_block = (ds_dsd_apm_data_block *)q_check (&ds_dsd_apm_rules_q[subs_indx]);

   while(rule_block != NULL)
   {
     if(rule_block->apn_info.subs_id == subs_id)
     {
       last_block = rule_block;
     }

     rule_block = (ds_dsd_apm_data_block *)q_next(&ds_dsd_apm_rules_q[subs_indx], 
                                                  &(rule_block->link));
   }

   return last_block;
}/*ds_dsd_apm_get_last_rule_block_per_subs*/
/*===========================================================================
FUNCTION       DS_DSD_APM_FREE_BLOCK_QUEUE_FOR_SUBS

DESCRIPTION    This utility function frees queue items in the queue for 
               the passed subs 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_free_block_queue_for_subs
(
  sys_modem_as_id_e_type      subs_id,
  q_type                     *rule_q
)
{
  apn_priority_data_block     *block = NULL;
  apn_priority_data_block     *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (apn_priority_data_block *)q_check(rule_q);

    while (block != NULL)
    {
      if (block->apn_block_info.subs_id == subs_id) 
      {
        prev_block = block;
        block = (apn_priority_data_block *)q_next(rule_q,&(block->link));
        q_delete(rule_q, &(prev_block->link));
        modem_mem_free (prev_block, MODEM_MEM_CLIENT_DATA);
      }
      else
      {
        block = (apn_priority_data_block *)q_next(rule_q, &(block->link));
      }
    }
  }

  return;
} /* ds_dsd_apm_free_block_queue_for_subs */

/*===========================================================================
FUNCTION       DS_DSD_APM_FREE_RULE_QUEUE_FOR_SUBS

DESCRIPTION    This utility function frees queue items in the queue for 
               the passed subs 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_free_rule_queue_for_subs
(
  sys_modem_as_id_e_type    subs_id,
  q_type                    *rule_q
)
{
  ds_dsd_apm_data_block     *block = NULL;
  ds_dsd_apm_data_block     *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (ds_dsd_apm_data_block*)q_check(rule_q);

    while (block != NULL)
    {
      if (block->apn_info.subs_id == subs_id) 
      {
        prev_block = block;
        block = (ds_dsd_apm_data_block *)q_next(rule_q,&(block->link));
       /*---------------------------------------------------------------------
        Free the cause code queue for a rule block entry
        ---------------------------------------------------------------------*/
        ds_dsd_apm_free_cause_code_q_for_rule_block(&(prev_block->apn_info.
                                            cause_code_info.cause_code_info_q));
        q_delete(rule_q, &(prev_block->link));
        modem_mem_free (prev_block, MODEM_MEM_CLIENT_DATA);
      }
      else
      {
        block = (ds_dsd_apm_data_block*)q_next(rule_q, &(block->link));
      }
    }
  }
  return;
} /* ds_dsd_apm_free_rule_queue_for_subs */

/*===========================================================================
FUNCTION       DS_DSD_APM_FREE_PROF_QUEUE_FOR_SUBS

DESCRIPTION    This utility function frees queue items in the queue for 
               the passed subs 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_free_prof_queue_for_subs
(
  sys_modem_as_id_e_type           subs_id,
  q_type                          *rule_q
)
{
  attach_profile_data_block       *block = NULL;
  attach_profile_data_block       *prev_block = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (NULL != rule_q) 
  {
    block = (attach_profile_data_block *)q_check(rule_q);
    while (block != NULL)
    {
      if (block->attach_profile.subs_id == subs_id) 
      {
        prev_block = block;
        block = (attach_profile_data_block *)q_next(rule_q,&(block->link));
        q_delete(rule_q, &(prev_block->link));
        modem_mem_free (prev_block, MODEM_MEM_CLIENT_DATA);
      }
      else
      {
        block = (attach_profile_data_block *)q_next(rule_q, &(block->link));
      }
    }
  }
  return ;
} /* ds_dsd_apm_free_prof_queue_for_subs */

/*===========================================================================
FUNCTION       DS_DSD_APM_IS_INTERNAL_UPDATE_ALLOWED

DESCRIPTION    This internal function checks the APM SM state and decides 
               whether internal structures can be updated or not 
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF Update is permitted 
               FALSE: If update is not allowed 

SIDE EFFECTS   None
===========================================================================*/
boolean ds_dsd_apm_is_internal_update_allowed
(
  sys_modem_as_id_e_type        subs_id
)
{
  boolean                       ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if(subs_info[subs_indx].attach_pdn_info.state == ATTACH_PDN_STATE_DISCONNECTED
      || subs_info[subs_indx].attach_pdn_info.state == ATTACH_PDN_STATE_INVALID)
  {
    ret_val = TRUE;
  }

  DATA_MSG2(MSG_LEGACY_MED,"ds_dsd_apm update internal allowed: %d ,state: %d ",
               ret_val,subs_info[subs_indx].attach_pdn_info.state);
  return ret_val;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_IS_APN_IN_ATTACH_PDN_LIST_INTERNAL

DESCRIPTION    This function checks whether a given APN is an attach APN
 
PARAMETERS     apn_name: APN which has to be checked

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF APN passed is Attach APN
               FALSE: Otherwise

SIDE EFFECTS   None
===========================================================================*/
boolean ds_dsd_apm_is_apn_in_attach_pdn_list_internal
(
  sys_modem_as_id_e_type     subs_id,
  char                       *apn_name
)
{
  ds_dsd_apm_data_block      *rule_block = NULL;
  boolean                     ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rule_block = (ds_dsd_apm_data_block *)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                               ds_dsd_apm_compare_apn_name_rule_q,
                                               apn_name);

  if(rule_block != NULL)
  {
    ret_val = TRUE;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION       DSD_APM_IS_GENERIC_APN_SWITCHING_ENABLED

DESCRIPTION    This function checks whether Generic APN Switching has been 
               enabled or not 
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF Generic APN Switching is enabled
               FALSE: Otherwise

SIDE EFFECTS   None
===========================================================================*/
boolean ds_dsd_apm_is_generic_apn_switching_enabled
(
   sys_modem_as_id_e_type         subs_id
)
{
  boolean                         ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  return subs_info[subs_indx].dsd_apm_generic_switching_status;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_PRINT_APM_Q_CONTENT

DESCRIPTION    This is a utility function to print the contents of the apm 
               queues. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_print_apm_q_content
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_dsd_apm_data_block      *apm_data_block = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);


  DATA_MSG1(MSG_LEGACY_HIGH,"ds_dsd_apm_rules_q item cnt:%d",
            q_cnt(&ds_dsd_apm_rules_q[subs_indx]));

  apm_data_block = (ds_dsd_apm_data_block*)q_check(&ds_dsd_apm_rules_q[subs_indx]);
  while(apm_data_block != NULL)
  {
    DATA_MSG4(MSG_LEGACY_HIGH,
              "subs_id: %d apn_name: %s apn_type:%d profile_id:%d",
              apm_data_block->apn_info.subs_id,
              apm_data_block->apn_info.apn_name,
              apm_data_block->apn_info.apn_type,
              apm_data_block->apn_info.profile_id);

    apm_data_block = (ds_dsd_apm_data_block*)q_next(&ds_dsd_apm_rules_q[subs_indx],
                                                    &(apm_data_block->link));
  }

  return;
} /* ds_dsd_apm_print_apm_q_content */

/*===========================================================================
FUNCTION      DS_DSD_APM_RULE_RESET_APN_CLASS

DESCRIPTION   Clean mapped APN class and profile ID in APM rule queue
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void ds_dsd_apm_rule_reset_apn_class
(
  sys_modem_as_id_e_type  subs_id,
  q_type                  *q
)
{
  ds_dsd_apm_data_block          *block = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( q != NULL )
  {
    DATA_MSG0(MSG_LEGACY_MED,"Reset APN class to be invalid in APM rule queue");
    block = (ds_dsd_apm_data_block*)q_check(q);
    while( block != NULL )
    {
      if (block->apn_info.subs_id == subs_id) 
      {
        block->apn_info.apn_type = ATTACH_PDN_INVALID;
        block->apn_info.profile_id = 0;
      }
      block = (ds_dsd_apm_data_block*)q_next(q, &(block->link));
    }
  }
  return;
} /*ds_dsd_apm_rule_reset_apn_class()*/

/*===========================================================================
FUNCTION      DS_DSD_APM_SET_CLASS_DEPENDENT_ACTION_RULES

DESCRIPTION   This function sets the action rules depending on the 
              APN Class
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_dsd_apm_set_class_dependent_action_rules
(
   apn_info_type                 *apn_info
)
{
  uint16                          config_error = 0;
/*-------------------------------------------------------------------------*/
  if(apn_info == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "Invalid Input Parameter Passed, Action rules will not be set");
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  switch( apn_info->apn_type )
  {
    case ATTACH_PDN_PRIMARY:
      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
      {
      apn_info->AttachPdn.action_rule.ip_address =
        (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
      {
      apn_info->AttachPdn.action_rule.pcscf_address = 
        (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.dns_address = 
            (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.pcscf_or_dns_address = 
            (uint16)ACTION_CHANGE_TO_NEXT | (uint16)ACTION_INIT_DETACH;
      }

      apn_info->AttachPdn.action_rule.out_of_service = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.plmn_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.sys_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.attach_fail = 
        (uint16)ACTION_CHANGE_TO_NEXT;

      apn_info->AttachPdn.action_rule.attach_success = (uint16)0;

      apn_info->AttachPdn.action_rule.detach_normal = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                 ACTION_CHANGE_TO_NEXT;

      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                 ACTION_CHANGE_TO_PRIMARY;
      break;

    case ATTACH_PDN_NEXT:
      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
      {
      apn_info->AttachPdn.action_rule.ip_address = 
        (uint16)ACTION_INIT_DETACH | (uint16)ACTION_CHANGE_TO_PRIMARY |
        (uint16)ACTION_BLOCK_PLMN;
      }

      apn_info->AttachPdn.action_rule.pcscf_address = (uint16)0;

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.dns_address = 
            (uint16)ACTION_CHANGE_TO_PRIMARY | (uint16)ACTION_INIT_DETACH;
      }

      if(apn_info->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF_DNS_ADDRESS)
      {
        apn_info->AttachPdn.action_rule.pcscf_or_dns_address = 
            (uint16)ACTION_CHANGE_TO_PRIMARY | (uint16)ACTION_INIT_DETACH;
      }

      apn_info->AttachPdn.action_rule.out_of_service = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.plmn_change =
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.sys_change = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.attach_fail = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_success = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;

      apn_info->AttachPdn.action_rule.detach_normal = (uint16)0;

      apn_info->AttachPdn.action_rule.attach_blocked = 
        (uint16)ACTION_CHANGE_TO_PRIMARY;
                      
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                 ACTION_CHANGE_TO_PRIMARY;
                      
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR,"Wrong APN class passed", 
                apn_info->apn_type);
      config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  return config_error;
}

/*===========================================================================
FUNCTION      DS_DSD_APM_SET_GENERIC_ACTION_RULES

DESCRIPTION   This function sets the generic action rules for Switching on 
              attach failures 
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_dsd_apm_set_generic_action_rules 
(
   apn_info_type                 *apn_info
)
{
  ds_dsd_apm_data_block          *apm_rule_block = NULL;
  uint16                          config_error = 0;
  sys_modem_as_id_e_type          subs_id;
/*---------------------------------------------------------------------------*/
  if(apn_info == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "Invalid Input Parameter Passed, Action rules will not be set");
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  subs_id = apn_info->subs_id;
  apm_rule_block = ds_dsd_apm_get_last_rule_block_per_subs(subs_id);

  if(apm_rule_block == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "APM Rule queue contents messed up, return Error");
    config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  else
  { 
    /*------------------------------------------------------------------------ 
     If the current rule block is last entry in the rule queue, then we
     set action rules differently
    -------------------------------------------------------------------------*/
    if(memcmp((void *)&(apm_rule_block->apn_info), (void *)apn_info,
              sizeof(apn_info_type)) == 0)
    {
      apn_info->AttachPdn.action_rule.attach_fail = (uint16)0;
      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.attach_success = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.plmn_change = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.sys_change = (uint16) 
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.out_of_service = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
    }
    else
    {
      apn_info->AttachPdn.action_rule.attach_fail = (uint16)
                                                     ACTION_CHANGE_TO_NEXT;
      apn_info->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_NEXT;
      apn_info->AttachPdn.action_rule.plmn_change = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.sys_change = (uint16) 
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.out_of_service = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
      apn_info->AttachPdn.action_rule.reset_attach_apn = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
    }
  }

  return config_error;
}

/*===========================================================================
FUNCTION      DS_DSD_APM_SET_CAUSE_CODE_BASED_ACTION_RULES

DESCRIPTION   This function sets the action rules for Switching on basis
              on Attach PDN & RAT agnostic rules
 
PARAMETERS    apn_info : Rule block for whom the action rules have to be set
 
DEPENDENCIES  None

RETURN VALUE  uint16: contain an error code value if any

SIDE EFFECTS  Setting of Action rules in APM Rule queue block
===========================================================================*/
uint16 ds_dsd_apm_set_cause_code_based_action_rules
(
   apn_info_type                 *apn_info_ptr
)
{
  ds_dsd_apm_data_block          *apm_rule_block_ptr = NULL;
  uint16                          config_error = 0;
  uint16                          ip_addr_pcscf_addr_action_rule = 0;
  uint16                          attach_proc_action_rule = 0;
  sys_modem_as_id_e_type          subs_id;
/*---------------------------------------------------------------------------*/
  if(apn_info_ptr == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "Invalid Input Parameter Passed, Action rules will not be set");
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  subs_id = apn_info_ptr->subs_id;
  apm_rule_block_ptr = ds_dsd_apm_get_last_rule_block_per_subs(subs_id);

  if(apm_rule_block_ptr == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "APM Rule queue contents messed up, return Error");
    config_error = EFS_APM_RULE_APN_CLASS_ERR;
  }
  else
  { 
    /*------------------------------------------------------------------------ 
     If the current rule block is last entry in the rule queue, then we
     set action rules differently
    -------------------------------------------------------------------------*/
    if(memcmp((void *)&(apm_rule_block_ptr->apn_info), (void *)apn_info_ptr,
              sizeof(apn_info_type)) == 0)
    {
      ip_addr_pcscf_addr_action_rule = ACTION_CHANGE_TO_PRIMARY |
                                       ACTION_INIT_DETACH;
      attach_proc_action_rule = ACTION_CHANGE_TO_PRIMARY;
    }
    else
    {
      ip_addr_pcscf_addr_action_rule = ACTION_CHANGE_TO_NEXT | 
                                       ACTION_INIT_DETACH;
      attach_proc_action_rule = ACTION_CHANGE_TO_NEXT;
    }
    /*------------------------------------------------------------------------ 
     If the masks were set during EFS read then the action rules are configured
     for the APN
    -------------------------------------------------------------------------*/
    if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
    {
      apn_info_ptr->AttachPdn.action_rule.ip_address =
        (uint16)ip_addr_pcscf_addr_action_rule;
    }
    if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
    {
      apn_info_ptr->AttachPdn.action_rule.pcscf_address = 
        (uint16)ip_addr_pcscf_addr_action_rule;
    }
    if(apn_info_ptr->cause_code_info.config_valid_flag & CONFIG_ATTACH_FAIL)
    {
      apn_info_ptr->AttachPdn.action_rule.attach_fail = (uint16)
                                                     attach_proc_action_rule;
    }
    if(apn_info_ptr->cause_code_info.config_valid_flag & CONFIG_DETACH_NORMAL)
    {
      apn_info_ptr->AttachPdn.action_rule.detach_normal = (uint16)
                                                     attach_proc_action_rule;
    }
    apn_info_ptr->AttachPdn.action_rule.attach_blocked = (uint16)
                                                   attach_proc_action_rule;
  }

  return config_error;
}

/*===========================================================================
FUNCTION ds_dsd_apm_set_single_attach_prof_action_rules

DESCRIPTION
  This function sets the action rules for a single attach profile scenario

PARAMETERS 
  apn_info_ptr: APM rule queue block for which action rules need to be set

DEPENDENCIES
  None

RETURN VALUE
  uint16 - mask containing config error if any
   
SIDE EFFECTS
  None
===========================================================================*/
uint16 ds_dsd_apm_set_single_attach_prof_action_rules
(
   apn_info_type                 *apn_info_ptr
)
{
  uint16                          config_error = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(apn_info_ptr == NULL)
  {
    DATA_MSG0 (MSG_LEGACY_ERROR,
               "Invalid Input Parameter Passed, Action rules will not be set");
    config_error = EFS_APM_RULE_READ_ERR;
    return config_error;
  }

  apn_info_ptr->AttachPdn.action_rule.attach_blocked = (uint16)
                                                     ACTION_CHANGE_TO_PRIMARY;
  /*------------------------------------------------------------------------ 
   If the masks were set during EFS read then the action rules are configured
   for the APN
  -------------------------------------------------------------------------*/
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
  {
    apn_info_ptr->AttachPdn.action_rule.ip_address =
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
  {
    apn_info_ptr->AttachPdn.action_rule.pcscf_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
  {
    apn_info_ptr->AttachPdn.action_rule.dns_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                                       CONFIG_PCSCF_DNS_ADDRESS)
  {
    apn_info_ptr->AttachPdn.action_rule.pcscf_or_dns_address = 
      (uint16)(ACTION_CHANGE_TO_PRIMARY|ACTION_INIT_DETACH);
  }

  return config_error;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_START_BLOCK_RULE_Q

DESCRIPTION    This utility function traverses the rule queue to return a 
               pointer to first block for a particular subscription
 
PARAMETERS     subs_id: Currently active Data subscription
 
DEPENDENCIES   NONE

RETURN VALUE   first_block: first rule block in the queue for given subs

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_apm_data_block* ds_dsd_apm_get_start_block_rule_q
(
  sys_modem_as_id_e_type                     subs_id
)
{
  ds_dsd_apm_data_block                     *rule_block = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return NULL;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rule_block = (ds_dsd_apm_data_block *)q_check(&ds_dsd_apm_rules_q[subs_indx]);

  while(rule_block != NULL)
  {
    if(rule_block->apn_info.subs_id == subs_id)
    {
      break;
    }

    rule_block = (ds_dsd_apm_data_block *)q_next(&ds_dsd_apm_rules_q[subs_indx], 
                                                 &(rule_block->link));
  }

  return rule_block;
}

/*===========================================================================
FUNCTION      DSD_APM_SET_ACTION_RULE

DESCRIPTION   This function sets the action rules depending on the 
              configuration
 
PARAMETERS    subs_id:      Currently active PS subscription 
              rule_q:       APM Rule queue pointer
              config_error: If Generic APN switching is disabled we dont set
                            action rules and revert to default rule
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Setting of Action rules in APM Rule queue
===========================================================================*/
void dsd_apm_set_action_rules
(
  sys_modem_as_id_e_type          subs_id,
  q_type                         *rule_q,
  uint16                         *config_error
)
{
  ds_dsd_apm_data_block          *rule_block = NULL;
  apn_info_type                  *apn_info = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( rule_q == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Empty rule queue");
    return;
  }
  rule_block = (ds_dsd_apm_data_block*)q_check(rule_q);
  
  /*------------------------------------------------------------------------ 
   Here we set the action rules for VZW and Generic APN Switching config.
   1. If the APN Class is either Class 1/Class 3, we set VZW action rules
   2. Else if Generic APN switching is enabled, we set generic switching rules
   3. Else if we have reject/detach cause codes configured for APN switching
   4. Else if we check if we have a single APM rule entry then we need to
      set switching rules as per configuration in EFS file
   5. Else we return configuration error and revert to default APM Rule
   6. Else we return configuration error and revert to default APM Rule
  -------------------------------------------------------------------------*/
  while( rule_block != NULL )
  {
    if (rule_block->apn_info.subs_id == subs_id) 
    {
      apn_info = &(rule_block->apn_info);
      DATA_MSG1(MSG_LEGACY_MED,"Setting action rules for apn class %d", 
                apn_info->apn_type);
      if(ds_dsd_apm_config_validate_attach_pdn_class(apn_info->apn_type))
      {
        *config_error = ds_dsd_apm_set_class_dependent_action_rules (apn_info);
      }
      else if (ds_dsd_apm_is_generic_apn_switching_enabled(subs_id))
      {
        *config_error = ds_dsd_apm_set_generic_action_rules (apn_info);
      }
     else if(ds_dsd_apm_config_get_apm_rules_num(subs_id, rule_q) == 
                                                     DS_DSD_APM_SINGLE_QUEUE_ENTRY)
      {
        *config_error = ds_dsd_apm_set_single_attach_prof_action_rules(apn_info);
      } 
      else if(q_cnt(&(apn_info->cause_code_info.cause_code_info_q)) > 0)
      {
        *config_error = ds_dsd_apm_set_cause_code_based_action_rules (apn_info);
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_ERROR,
                  "Cannot set APN action rules, validation failures !!!");
        *config_error = EFS_APM_APN_ACTION_RULE_ERR;
      }

      /*--------------------------------------------------------------------- 
       Set the APN Type of first element in APM Rule Queue as PRIMARY
       1. This is done to ensure searching for next attach PDN is faster
      ----------------------------------------------------------------------*/
      if(*config_error == 0)
      {
        if(rule_block == ds_dsd_apm_get_start_block_rule_q (subs_id))
        {
          rule_block->apn_info.apn_type = ATTACH_PDN_PRIMARY;
        }
        else
        {
          rule_block->apn_info.apn_type = ATTACH_PDN_NEXT;
        }
      }
      else
      {
        DATA_MSG0(MSG_LEGACY_HIGH,"Configuration Error, breaking out !!!!!");
        break;
      }
    }
    rule_block = (ds_dsd_apm_data_block*)q_next(rule_q, &(rule_block->link));

  }
  return;
}/* dsd_apm_set_action_rule() */

/*===========================================================================
FUNCTION       DS_DSD_APM_NEXT_APN_INIT

DESCRIPTION    This is the utility function to initialize the next attach PDN 
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_next_apn_init
(
  sys_modem_as_id_e_type              subs_id
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  memset(&subs_info[subs_indx].next_attach_pdn_info, 0, 
         sizeof(subs_info[subs_indx].next_attach_pdn_info));

  DATA_MSG1(MSG_LEGACY_MED,"next attach pdn type is %d", ATTACH_PDN_PRIMARY);
  subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type = 
                                                          ATTACH_PDN_PRIMARY;
  return;
}/* ds_dsd_apm_next_apn_init()*/

/*===========================================================================
FUNCTION       DS_DSD_APM_CONFIGURE_IP_RULE_FOR_SINGLE_APM_RULE_ENTRY

DESCRIPTION    This is the utility function that configures the IP address 
               requirement for the single APM rule queue entry 
 
PARAMETERS     subs_id: Current active PS subscription
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_configure_ip_rule_for_single_apm_rule_entry
(
  sys_modem_as_id_e_type            subs_id
)
{
  ds_profile_info_type                 prof_pdn_type;
  boolean                              pdn_type_valid = TRUE;
  ipRequiredType                       ip_req_enum_value = 
                                           ipv4_dontcare_and_ipv6_donotcare;
  ds_profile_3gpp_pdp_type_enum_type   pdn_type = DS_PROFILE_3GPP_PDP_MAX;
  ds_dsd_apm_data_block*               apm_rule_data_block = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  apm_rule_data_block = (ds_dsd_apm_data_block *)q_check(&ds_dsd_apm_rules_q[subs_indx]);

  if(NULL != apm_rule_data_block)
  {
    memset(&prof_pdn_type, 0, sizeof(ds_profile_info_type));

    prof_pdn_type.buf = &pdn_type;
    prof_pdn_type.len = sizeof(pdn_type);

    if ( dsd_apm_get_profile_param_info(
                  apm_rule_data_block->apn_info.profile_id,
                  DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
                  &prof_pdn_type,
                  subs_id) == TRUE )
    {
      switch(pdn_type)
      {
        case DS_PROFILE_3GPP_PDP_IP:
          ip_req_enum_value = ipv4_mandatory_and_ipv6_donotcare;
          pdn_type_valid = TRUE;
          break;
        case DS_PROFILE_3GPP_PDP_IPV6:
          ip_req_enum_value = ipv6_mandatory_and_ipv4_donotcare;
          pdn_type_valid = TRUE;
          break;
        case DS_PROFILE_3GPP_PDP_IPV4V6:
          ip_req_enum_value = ipv4_mandatory_or_ipv6_mandatory;
          pdn_type_valid = TRUE;
          break;
        default:
          DATA_MSG1(MSG_LEGACY_ERROR,
                    "pdn type %d is invalid, no setting IP rule", 
                    pdn_type);
      }
    }

    if(TRUE == pdn_type_valid)
    {
      apm_rule_data_block->apn_info.AttachPdn.config_rule.config_valid_flag |= 
        CONFIG_IP_ADDRESS;
      apm_rule_data_block->apn_info.AttachPdn.config_rule.ip_address = 
        ip_req_enum_value;
    }
  }
}

/*===========================================================================
FUNCTION       DSD_APM_RULE_VALIDATION

DESCRIPTION    This is the utility function used for rule and profile 
               validation and matching for a VZW Configured UE
 
PARAMETERS     subs_id: Current active PS subscription
                
DEPENDENCIES   NONE

RETURN VALUE   config_error: uint16 to indicate config error if any

SIDE EFFECTS   NONE
===========================================================================*/
uint16 dsd_apm_rule_validation
(
   sys_modem_as_id_e_type                 subs_id
)
{
  uint16                                  config_error = 0;
  uint16                                  num_attach_profs = 0, num_rules = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------ 
   We currently support validation only if the 2 conditions are met:
   
   1. The number of attach profiles and rules should be equal
   2. The number should be greater than zero and less than or equal to 2
  -------------------------------------------------------------------------*/
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return config_error;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  num_attach_profs = ds_dsd_apm_config_get_attach_prof_num(subs_id, 
                                                        &ds_dsd_apm_attach_profile_q[subs_indx]);
 
  num_rules = ds_dsd_apm_config_get_apm_rules_num(subs_id, &ds_dsd_apm_rules_q[subs_indx]);

  if((num_attach_profs == num_rules) &&
     (num_attach_profs <= ATTACH_APN_MAX && num_attach_profs > 0))
  {
    DATA_MSG0(MSG_LEGACY_MED, "Number of attach profiles and rules match");
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_ERROR, 
              "Mismatch of number of attach profs and rules, default rule");
    config_error = EFS_APM_RULE_ATTACH_PROF_NUM_ERR;
  }

  /*------------------------------------------------------------------
   No configure error found in APM rule and attach profile, matching
   attach profile with APM rule. APM needs to find corresponding rules
   for each attach profile. 
  -------------------------------------------------------------------*/
  if( config_error == 0 )
  {
    /*------------------------------------------------------------------
    Match LTE attach profile to APM rule, and get first attach PDN
    -------------------------------------------------------------------*/
    ds_dsd_apm_match_attach_profile_and_rule(subs_id,
                                          &ds_dsd_apm_rules_q[subs_indx], 
                                          &ds_dsd_apm_attach_profile_q[subs_indx],
                                          &config_error);
  }
  /*----------------------------------------------------------------------
   No configure error found in matching APM rule & attach profile, setting
   the IP addr requirement based on profile's PDP type for single
   rule queue entry
  -----------------------------------------------------------------------*/
  if(0 == config_error &&
     DS_DSD_APM_SINGLE_QUEUE_ENTRY == num_rules)
  {
    ds_dsd_apm_configure_ip_rule_for_single_apm_rule_entry (subs_id);
  }

  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  return config_error;
}

/*===========================================================================
FUNCTION       DSD_APM_POPULATE_RULE_Q

DESCRIPTION    This is the utility function used to populate rule queue in 
               case of generic APN switching 
 
PARAMETERS     subs_id: Current active PS subscription
                
DEPENDENCIES   NONE

RETURN VALUE   config_error: uint16 to indicate config error if any

SIDE EFFECTS   NONE
===========================================================================*/
uint16 dsd_apm_populate_rule_q
(
  sys_modem_as_id_e_type      subs_id
)
{
  ds_dsd_apm_data_block        *rule_entry = NULL;
  attach_profile_data_block    *prof_entry = NULL;
  uint16                       profile_id;
  char                         apn_name[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  ds_dsd_apm_profile_info      subs_profile_info;
  uint16                       config_error = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return config_error;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG1(MSG_LEGACY_MED,
            "Adding APM rules with attach profile list on sub %d",subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);
  prof_entry = (attach_profile_data_block* )q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);
  if( prof_entry == NULL )
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"No valid attach profile id");
    config_error |= EFS_PROFILE_RULE_MISMATCH;
    rex_leave_crit_sect(&ds_dsd_apm_critical_section);
    return config_error;
  }

  while( prof_entry != NULL)
  {
    if(prof_entry->attach_profile.subs_id == subs_id)
    {

      profile_id = prof_entry->attach_profile.profile;
      DATA_MSG1(MSG_LEGACY_MED,"Looking for matching APM rule for profile %d", 
                profile_id);

      memset(apn_name, '\0', DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
      subs_profile_info.subs_id = subs_id;
      subs_profile_info.profile_info.buf = apn_name;
      subs_profile_info.profile_info.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;
      if ( dsd_apm_get_profile_param_info(profile_id,
                                    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
                                    &subs_profile_info.profile_info,
                                    subs_id) == TRUE )
      {
        rule_entry = (ds_dsd_apm_data_block *)modem_mem_alloc(
           sizeof(ds_dsd_apm_data_block), MODEM_MEM_CLIENT_DATA);

        if(rule_entry != NULL)
        {
          memset(rule_entry, 0, sizeof(ds_dsd_apm_data_block));

          rule_entry->apn_info.profile_id = profile_id;
          rule_entry->apn_info.subs_id = subs_id;
          /*--------------------------------------------------------------
            Need to initialize the cause code queue in the APM Rule Block
          ---------------------------------------------------------------*/
          q_init(&(rule_entry->apn_info.cause_code_info.cause_code_info_q));

          strlcpy(rule_entry->apn_info.apn_name, apn_name,
                  DSD_APM_MAX_APN_STRING_LEN);
          rule_entry->apn_info.apn_name_len = strlen (apn_name);
          q_link(rule_entry, &(rule_entry->link));
          q_put(&ds_dsd_apm_rules_q[subs_indx], &(rule_entry->link));
        }
        else
        {
          DATA_MSG0_ERROR ("Memory allocation failure, reverting to default "
                           "Rule");
          config_error |= EFS_ATTACH_PROFILE_READ_ERR;
          break;
        }
      }
      else
      {
        config_error |= EFS_ATTACH_PROFILE_READ_ERR;
        break;
      }
    }
    prof_entry = q_next(&ds_dsd_apm_attach_profile_q[subs_indx], &(prof_entry->link));
  }
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  return config_error;
}
/*===========================================================================
FUNCTION       DS_DSD_APM_SET_DEFAULT_CONFIGURATION

DESCRIPTION    This is the utility function to set the default configuration 
               in case of configuration failure or APN Switching/Blocking
               being dsiabled
 
PARAMETERS     subs_id: Current active PS subscription
                
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_default_configuration
(
   sys_modem_as_id_e_type                subs_id
)
{
  attach_profile_data_block             *q_head = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  q_head = (attach_profile_data_block*)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);

  /*------------------------------------------------------------------------ 
   Grab first rule on passed sub
  -------------------------------------------------------------------------*/
  while(q_head != NULL)
  {
    if(q_head->attach_profile.subs_id == subs_id)
    {
      /*------------------------------------------------------------------
       We clear the Rule queue since we wont be using Invalid Rule entries
       and add just the default rule since APN class configuration was not
       done correctly
      -------------------------------------------------------------------*/
      ds_dsd_apm_free_rule_queue_for_subs(subs_id, &ds_dsd_apm_rules_q[subs_indx]);

      /*------------------------------------------------------------------
        Set next attach PDN and set to be permanent
      -------------------------------------------------------------------*/
      memset(&subs_info[subs_id].next_attach_pdn_info, 0, 
             sizeof(subs_info[subs_id].next_attach_pdn_info));

      subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type = 
                                                         ATTACH_PDN_DEFAULT;
      ds_dsd_apm_next_attach_pdn_set_permanent(subs_id);
      ds_dsd_apm_set_default_apm_rule(subs_id,q_head->attach_profile.profile,
                                  &ds_dsd_apm_rules_q[subs_indx]);
      break;
    }
    q_head = q_next(&ds_dsd_apm_attach_profile_q[subs_indx], &(q_head->link));
  }
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  return;
}

/*===========================================================================
FUNCTION      DSD_APM_SET_CONFIGURATION

DESCRIPTION   Based on APM rule and attach profile, set next attach PDN, 
              APM rule and action based on PDN class 
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
===========================================================================*/
void dsd_apm_set_configuration
(
   sys_modem_as_id_e_type                 subs_id
)
{
  uint16                                  config_error = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);


  DATA_MSG1(MSG_LEGACY_MED,"dsd_apm_set_configuration on sub %d",subs_id);

  /*------------------------------------------------------------------
    Fill the APN Disable Info
  -------------------------------------------------------------------*/
  ds_dsd_apm_fill_apn_disable_info(subs_id);

  if(ds_dsd_apm_is_apn_switch_support(subs_id))
  {
    /*------------------------------------------------------------------
    Unset the permanent flag
    -------------------------------------------------------------------*/
    ds_dsd_apm_next_attach_pdn_unset_permanent(subs_id);
    /*------------------------------------------------------------------
    clear stale infomation in APM rule queue and delete default rule
    -------------------------------------------------------------------*/
    ds_dsd_apm_delete_default_apm_rule(subs_id, &ds_dsd_apm_rules_q[subs_indx]);
    ds_dsd_apm_rule_reset_apn_class(subs_id, &ds_dsd_apm_rules_q[subs_indx]);

    /*------------------------------------------------------------------
     If generic APN Switching is enabled that suggests no rule file is
     present, we populate the rule queue as per generic APN switching
     requirements
     Else we perform rule validation
    -------------------------------------------------------------------*/
    if(ds_dsd_apm_is_generic_apn_switching_enabled(subs_id))
    {
      ds_dsd_apm_free_rule_queue_for_subs(subs_id, &ds_dsd_apm_rules_q[subs_indx]);
      config_error = dsd_apm_populate_rule_q(subs_id);
    }
    else
    {
      config_error = dsd_apm_rule_validation(subs_id);
    }

    /*------------------------------------------------------------------
      Set the APM PDN action rules
    -------------------------------------------------------------------*/
    if( config_error == 0)
    {
      dsd_apm_set_action_rules(subs_id, &ds_dsd_apm_rules_q[subs_indx], &config_error);
    }
    else
    {
      DATA_MSG1(MSG_LEGACY_ERROR,
                "APM rule or attach profile configure error %d, do not set"
                " action rules", config_error);
       /*---------------------------------------------------------------
        Check if profile read fails due to read error and reset the 
        attach profile queue to default socket profile.
       ----------------------------------------------------------------*/
      if( (config_error & EFS_ATTACH_PROFILE_READ_ERR) )
      { 
         DATA_MSG0(MSG_LEGACY_ERROR,
                     "Resetting to default attach profile");
         ds_dsd_apm_free_prof_queue_for_subs(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]);
         if(!dsd_apm_read_default_attach_prof(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]))
         {
            DATA_MSG0(MSG_LEGACY_ERROR,
                     "Default attach profile read failed, block LTE");
         }
      }
    }

    if( config_error == 0 ) /* Configure success*/
    {
      /*------------------------------------------------------------------
        If next attach PDN is specified which means APM already switch
        attach PDN based on certain failure conditon. Otherwise, initiate
        next attach PDN with proper APN type which is power up case or
        configuration error before.
       -------------------------------------------------------------------*/
       if( subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type
            == ATTACH_PDN_DEFAULT ||
           subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type
            == ATTACH_PDN_INVALID )
       {
         ds_dsd_apm_next_apn_init(subs_id);
       }
       else
       {
         DATA_MSG1(MSG_LEGACY_HIGH,
                   "Not setting the profile. Exisiting profile type = %d",
                   subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type);
       }
    }
    else
    {
      /*------------------------------------------------------------------
        We revert to default APM rule in case of rule validation failure
        or action rules setup failure
       -------------------------------------------------------------------*/
      ds_dsd_apm_set_default_configuration (subs_id);
    }
  } 
  else
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"LTE apn blocking and Switching both disabled");
    /*------------------------------------------------------------------
      We revert to default APM rule
    -------------------------------------------------------------------*/
    ds_dsd_apm_set_default_configuration (subs_id);
  }
  return;

}/* dsd_apm_set_configuration*/

/*===========================================================================
FUNCTION      DS_DSD_APM_SEND_NEW_ATTACH_PROF_IND

DESCRIPTION   This function sends indication to clients that attach profile 
              have been changed 
 
PARAMETERS    subs_id : Current active PS subscription 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
                                    
===========================================================================*/
void ds_dsd_apm_send_new_attach_prof_ind
(
  sys_modem_as_id_e_type                    subs_id,
  uint16                                    config_error
)
{
  ps_sys_3gpp_lte_attach_pdn_list_type      attach_list;
  int16                                     ps_error = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  memset(&attach_list, 0, sizeof(ps_sys_3gpp_lte_attach_pdn_list_type));
  /*-------------------------------------------------------------------------
              convert attach list to attach array
  -------------------------------------------------------------------------*/
  if(ds_dsd_apm_get_attach_pdn_list(&attach_list, subs_id, &ps_error) == 0)
  {
    if(config_error & EFS_ATTACH_PROFILE_READ_ERR)
    {
      DATA_MSG0(MSG_LEGACY_MED, "Invalid Attach Profiles were provided");
      ps_error = DS_EINVAL;
    }
    /*-------------------------------------------------------------------------
                convert attach list to attach array
    -------------------------------------------------------------------------*/
    ps_sys_event_ind(  
       PS_SYS_TECH_3GPP,
       PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE,
       &attach_list, 
       &ps_error);
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_ERROR, "Error retrieving attach PDN List: %d",
              ps_error);
  }

  DATA_MSG0(MSG_LEGACY_MED, "Sent new attach Profile indication to Clients");
}

/*===========================================================================
FUNCTION      DS_DSD_APM_VALIDATE_PROF_Q

DESCRIPTION   This function checks if the attach profiles read in Queue 
              are valid or not 
 
PARAMETERS    subs_id : Current active PS subscription 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
                                    
===========================================================================*/
boolean ds_dsd_apm_validate_prof_q
(
  sys_modem_as_id_e_type                    subs_id,
  uint16                                   *attach_prof_count_ptr
)
{
  attach_profile_data_block                *profile_block = NULL;
  uint16                                    profile_id = 0;
  ds_umts_pdp_profile_status_etype          result = DS_UMTS_PDP_FAIL;
  boolean                                   ret_val = FALSE;                         
  boolean                                   profile_valid = FALSE;                         
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  if(attach_prof_count_ptr != NULL)
  {
    profile_block = (attach_profile_data_block *)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);

    while(profile_block != NULL)
    {
      if(profile_block->attach_profile.subs_id == subs_id)
      {
        profile_id = profile_block->attach_profile.profile;

        result = ds_umts_get_pdp_profile_context_info_is_valid_per_subs (
                        profile_id,
                        dsumts_subs_mgr_get_subs_id(subs_id), 
                                                              &profile_valid);

        if( DS_UMTS_PDP_SUCCESS == result && profile_valid == TRUE)
        {
          (*attach_prof_count_ptr)++;
          profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                          &(profile_block->link));
          ret_val = TRUE;
        }
        else
        {
          DATA_MSG1(MSG_LEGACY_ERROR, "Invalid Profile found: %d",profile_id);
          ret_val = FALSE;
          break;
        }
      }
      else
      {
        profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                          &(profile_block->link));
      }
    }
  }
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  return ret_val;
}
/*===========================================================================
FUNCTION      DS_DSD_APM_CREATE_PROF_FOR_TF

DESCRIPTION   This function checks if the attach profiles read in Queue 
              are valid or not, if not we create them 
 
PARAMETERS    subs_id : Current active PS subscription 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
                                    
===========================================================================*/
void ds_dsd_apm_create_prof_for_tf
(
  sys_modem_as_id_e_type                   subs_id
)
{
#ifdef TEST_FRAMEWORK
  #error code not present
#endif /* TEST_FRAMEWORK */
}
/*===========================================================================
FUNCTION      DS_DSD_APM_EFS_READ

DESCRIPTION   This function reads EFS file 
 
PARAMETERS    subs_id : Current active PS subscription 
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Populate contents in attach_profile_q & ds_dsd_apm_q (Rule Q)
                                    
===========================================================================*/
void ds_dsd_apm_info_efs_read
(
  sys_modem_as_id_e_type                  subs_id
)
{
  uint16                                  config_error = 0;
  uint16                                  attach_prof_count = 0;
  uint8                                   new_attach_prof_mask = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_apm_efs_read on sub %d",subs_id);

  memset(&(subs_info[subs_id].next_attach_pdn_info), 0, 
         sizeof(next_attach_pdn_info_type));

  ds_dsd_apm_set_generic_switching_flag(subs_id, FALSE);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  ds_dsd_apm_free_rule_queue_for_subs(subs_id, &ds_dsd_apm_rules_q[subs_indx]);
  ds_dsd_apm_free_prof_queue_for_subs(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]);
  
  new_attach_prof_mask = dsd_apm_read_attach_profile_list_from_efs(subs_id,
                                                        &ds_dsd_apm_attach_profile_q[subs_indx]);
  /*---------------------------------------------------------------------
     Read LTE attach profiles from EFS files, if EFS read fails we read
     the default socket profile
  -------------------------------------------------------------------*/
  if (new_attach_prof_mask == FALSE)
  {
    ds_dsd_apm_free_prof_queue_for_subs(subs_id ,&ds_dsd_apm_attach_profile_q[subs_indx]);

    if(!dsd_apm_read_default_attach_prof(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]))
    {
      config_error |= EFS_DEF_ATTACH_PROF_READ_ERR;
    }
  } /* read profile list from EFS failed */
  else
  {
    ds_dsd_apm_create_prof_for_tf (subs_id);
    /*---------------------------------------------------------------------
       Validate the contents of attach profile list, if none of the profiles
       are valid then we revert to default socket profile
    -------------------------------------------------------------------*/
    if(ds_dsd_apm_validate_prof_q (subs_id, &attach_prof_count) == FALSE)
    {
      ds_dsd_apm_free_prof_queue_for_subs(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]);
      if(!dsd_apm_read_default_attach_prof(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx]))
      {
        config_error |= EFS_DEF_ATTACH_PROF_READ_ERR;
      }
    }
    else if(attach_prof_count > ATTACH_APN_MAX)
    {
      DATA_MSG1(MSG_LEGACY_HIGH, 
                "More than 2 attach profiles specified, Enabling Generic "
                "Switching", attach_prof_count);
      config_error |= EFS_ATTACH_PROFILE_NUM_ERR;
      if(ds_dsd_apm_is_apn_switch_support(subs_id) == TRUE)
      {
        ds_dsd_apm_set_generic_switching_flag(subs_id, TRUE);
      }
    }
    else if(attach_prof_count < ATTACH_APN_MAX &&
            attach_prof_count > 0 &&
            TRUE == ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn(subs_id))
    {
      config_error |= EFS_ATTACH_PROFILE_NUM_ERR;
    }

    if(new_attach_prof_mask & ATTACH_PROF_CHANGED)
    {
      ds_dsd_apm_send_new_attach_prof_ind (subs_id, config_error);
    }
  }
  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  /*------------------------------------------------------------------
   If read attach profile ID from EFS succeed, read APM rule;
   If read attach profile ID from EFS failed, default behavior and no
   APM switch.
   -------------------------------------------------------------------*/
  if( config_error == 0 )
  {
      if ( dsd_apm_read_pdn_rule_list_from_efs(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx],
                                               &ds_dsd_apm_rules_q[subs_indx]) == FALSE )
      {
        config_error |= EFS_APM_RULE_READ_ERR;
        /*------------------------------------------------------------------
         If read APM rule from EFS fail, free APM rule queue and enable
         generic APN Switching
        -------------------------------------------------------------------*/
      ds_dsd_apm_free_rule_queue_for_subs(subs_id, &ds_dsd_apm_rules_q[subs_indx]);
      if(ds_dsd_apm_is_apn_switch_support(subs_id) == TRUE)
      {
        ds_dsd_apm_set_generic_switching_flag(subs_id, TRUE);
      }
    }
  }

  /*---------------------------------------------------------------------
     If the default socket profile read fails, then we block LTE since
     we have no valid attach profile
  -------------------------------------------------------------------*/  
  if(config_error & EFS_DEF_ATTACH_PROF_READ_ERR)
  {
    subs_info[subs_id].attach_pdn_info.if_lte_attach_allowed = FALSE;
  }
  else
  {
    /*------------------------------------------------------------------
     Based on APM rule and attach profile, set APM module configuration
     including next attach PDN, rule and actions
    -------------------------------------------------------------------*/
    dsd_apm_set_configuration(subs_id);

    /*--------------------------------------------------------------
     Reset the Attach PDN info and copy next attach PDN info to 
     attach PDN data structure. 
    ----------------------------------------------------------------*/
    ds_dsd_apm_reset_and_copy_attach_info(subs_id);
  }

  if (config_error != 0) 
  {
    DATA_MSG1(MSG_LEGACY_ERROR,"config error: %d",config_error);
  }

  return;
}/* ds_dsd_apm_info_efs_read() */

/*===========================================================================

                         UTILITY
===========================================================================*/
 
/*===========================================================================
FUNCTION       DS_DSD_APM_GET_NEXT_ATTACH_PROFILE_ID_INTERNAL

DESCRIPTION    This utility function returns next attach profile id
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
uint16 ds_dsd_apm_get_next_attach_profile_id_internal
(
  sys_modem_as_id_e_type               subs_id, 
  uint16                               profile_id
)
{
  ds_dsd_apm_data_block               *rule_block = NULL;
  attach_profile_data_block           *profile_block = NULL;
  uint16                               ret_val = 0;
  ds_dsd_apm_attach_pdn               *next_attach_pdn = NULL;
  ds_dsd_apm_profile_id               *subs_profile_id = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG1(MSG_LEGACY_MED,"PDN type %d of next attach PDN",
            subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type);

  /*---------------------------------------------------------------------- 
    Depending on the queue position we extract the next attach profile
    The if part extracts the head of the queue
    Else part is used to extract any other internal elements
  ----------------------------------------------------------------------*/

  if(subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type 
         == ATTACH_PDN_PRIMARY ||
     subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type 
         == ATTACH_PDN_DEFAULT
     )
  {
    next_attach_pdn = (ds_dsd_apm_attach_pdn *)modem_mem_alloc 
                      (sizeof(ds_dsd_apm_attach_pdn), MODEM_MEM_CLIENT_DATA);

    if(next_attach_pdn != NULL)
    {
      memset(next_attach_pdn, 0 , sizeof(ds_dsd_apm_attach_pdn));
      next_attach_pdn->subs_id = subs_id;
      next_attach_pdn->attach_pdn = 
            subs_info[subs_id].next_attach_pdn_info.next_attach_pdn_type;

      rule_block = (ds_dsd_apm_data_block*)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                                          ds_dsd_apm_compare_apn_type,
                                                          next_attach_pdn);
      if( rule_block != NULL)
      {
        ret_val = rule_block->apn_info.profile_id;
      }
      modem_mem_free (next_attach_pdn, MODEM_MEM_CLIENT_DATA);
    }
  }
  else
  {
    subs_profile_id = (ds_dsd_apm_profile_id *)modem_mem_alloc (
                       sizeof(ds_dsd_apm_profile_id), MODEM_MEM_CLIENT_DATA);
    if(subs_profile_id != NULL)
    {
      memset(subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));
      subs_profile_id->profile_id = profile_id;
      subs_profile_id->subs_id = subs_id;

      profile_block = (attach_profile_data_block *)q_linear_search(
                                                    &ds_dsd_apm_attach_profile_q[subs_indx],
                                          ds_dsd_apm_compare_profile_id_prof_q,
                                                    subs_profile_id);
      if(profile_block != NULL)
      {
        profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                        &(profile_block->link));
        if(profile_block != NULL && 
           profile_block->attach_profile.subs_id == subs_id)
        {
          ret_val = profile_block->attach_profile.profile;
        }
        else
        {
          DATA_MSG0(MSG_LEGACY_ERROR,
                    "Cannot fetch the next attach profile ID, using current one");
          ret_val = profile_id;
        }
      }
      modem_mem_free(subs_profile_id,MODEM_MEM_CLIENT_DATA);
    }
    else
    {
      DATA_ERR_FATAL("Memory allocation failure, ASSERT");
    }
  }

  return ret_val;
}/* ds_dsd_apm_get_next_attach_profile_id_internal */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
FUNCTION       DS_DSD_APM_GET_NEXT_ATTACH_APN_NAME

DESCRIPTION    This utility function returns the name of the next attach APN 
               by apn_type search. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_get_next_attach_apn_name
(
  sys_modem_as_id_e_type      subs_id,
  ds_dsd_apm_apn_name        *buf,
  uint16                      profile_id
)
{
  ds_dsd_apm_data_block      *rule_block = NULL;
  boolean                     ret_val = TRUE;
  ds_dsd_apm_profile_id       subs_profile_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if ( buf == NULL ) 
  {
    DATA_ERR_FATAL("Buf passed is NULL, return");
    return FALSE;
  }

  memset(&subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));
  subs_profile_id.subs_id = subs_id;
  subs_profile_id.profile_id = profile_id;

  rule_block = (ds_dsd_apm_data_block*)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                                  ds_dsd_apm_compare_profile_id,
                                                  &subs_profile_id);

  if( rule_block != NULL)
  {
    DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                       "Find the apn_name %s, apn_len %d",
                       rule_block->apn_info.apn_name,
                       rule_block->apn_info.apn_name_len);

    memset(buf, 0, sizeof(ds_dsd_apm_apn_name));
    buf->len = rule_block->apn_info.apn_name_len;
    strlcpy(buf->apn_string, 
            rule_block->apn_info.apn_name, 
            DSD_APM_MAX_APN_STRING_LEN);
  }
  else
  {
    ret_val = FALSE;
  }

  return ret_val;
}/* ds_dsd_apm_get_next_attach_apn_name() */

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_APN_NAME_BY_APN_TYPE

DESCRIPTION    This utility function returns the APN name by searching the 
               a specific apn type in ds_dsd_apm_rules_q. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_get_apn_name_by_apn_type
(
  sys_modem_as_id_e_type          subs_id,
  ds_dsd_apm_attach_pdn           pdn_type,
  ds_dsd_apm_apn_name             *buf
)
{
  ds_dsd_apm_data_block      *rule_block = NULL;
  boolean                     ret_val = TRUE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if ( buf == NULL ) 
  {
    DATA_ERR_FATAL("Buf passed is NULL, return");
    return FALSE;
  }

// if not valid range, assert
  rule_block = (ds_dsd_apm_data_block*)q_linear_search( 
                                                     &ds_dsd_apm_rules_q[subs_indx],
                                                     ds_dsd_apm_compare_apn_type,
                                                     &pdn_type );

  if( rule_block != NULL)
  {
    DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                       "Find the apn_name %s, apn_len %d",
                       rule_block->apn_info.apn_name,
                       rule_block->apn_info.apn_name_len);
    memset(buf, 0, sizeof(ds_dsd_apm_apn_name));
    buf->len = rule_block->apn_info.apn_name_len;
    strlcpy(buf->apn_string, 
            rule_block->apn_info.apn_name, 
            DSD_APM_MAX_APN_STRING_LEN);
    buf->subs_id = pdn_type.subs_id;
  }
  else
  {
    ret_val = FALSE;
  }

  DATA_MSG2(MSG_LEGACY_MED,"ds_dsd_apm_get_apn_name_by_apn_type, pdn_type %d"
                           ", ret val: %d", pdn_type.attach_pdn,ret_val);

  return ret_val;
}/* ds_dsd_apm_get_apn_name_by_apn_type() */

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_PROFILE_ID_BY_APN_TYPE

DESCRIPTION    This utility function returns the profile id by searching 
               the APN type in ds_dsd_apm_rules_q
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_get_profile_id_by_apn_type
(
  sys_modem_as_id_e_type       subs_id,
  ds_dsd_apm_attach_pdn        pdn_type,
  uint16                       *profile_id
) 
{
  ds_dsd_apm_data_block      *rule_block = NULL;
  boolean                     ret_val = TRUE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (profile_id == NULL) 
  {
    DATA_ERR_FATAL("Buf passed is NULL, return");
    return FALSE;
  }

  //validate pdn_type, and assert
  rule_block = (ds_dsd_apm_data_block *)q_linear_search(
                                                       &ds_dsd_apm_rules_q[subs_indx],
                                                       ds_dsd_apm_compare_apn_type,
                                                       &pdn_type );

  if (rule_block != NULL) 
  {
    DATA_MSG2(MSG_LEGACY_HIGH,"Found matching profile id %d with apn_type %d",
              rule_block->apn_info.profile_id,
              pdn_type.attach_pdn);
    *profile_id = rule_block->apn_info.profile_id;
    ret_val = TRUE;
  } 
  else 
  {
    //add assert
    ret_val = FALSE;
  }

  DATA_MSG2(MSG_LEGACY_MED,"ds_dsd_apm_get_profile_id_by_apn_type %d ret val: %d",
                           pdn_type.attach_pdn,ret_val);

  return ret_val;
} /* ds_dsd_apm_get_profile_id_by_apn_type() */
/*===========================================================================
FUNCTION       DS_DSD_APM_IF_NEXT_ATTACH_APN_LTE_CAPABLE

DESCRIPTION    This utility function checks if the next attach APN is capable 
               for LTE attach or not. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_if_next_attach_apn_lte_capable
(
  sys_modem_as_id_e_type                       subs_id,
  uint16                                       profile_id
) 
{
  ds_dsd_apm_apn_name                          apn_name;
  boolean                                      ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&apn_name, 0, sizeof(ds_dsd_apm_apn_name));

  if (ds_dsd_apm_get_next_attach_apn_name(subs_id, &apn_name, profile_id)) 
  {
    ret_val = ds_dsd_apm_is_the_apn_allowed_to_attach(subs_id,
                                                      apn_name.apn_string,
                                                      apn_name.len);
  } 

  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_apm_if_next_attach_pdn_lte_capable on sub %d: %d",
            subs_id ,ret_val);
  return ret_val;
} /*ds_dsd_apm_if_next_attach_apn_lte_capable()*/

#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION       DS_DSD_APM_IS_ADDITONAL_APN_SWITCH_REQUIRED

DESCRIPTION    This utility function checks if we need to switch the APN 
               depending on throttling info provided by MH, APN Disable flag
               status and APN blocking info given by MPPM
 
PARAMETERS     subs_id : Gives currently active PS subscription id 
               profile_id: Profile on which the above checks need to be run 
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE:      Additional Switch needed
               FALSE:     No additonal switching needed 

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_is_additonal_apn_switch_required
(
   sys_modem_as_id_e_type            subs_id,
   uint16                            profile_id
)
{
   boolean                           is_lte_attach_allowed = TRUE;
   boolean                           is_apn_disabled = FALSE;
   boolean                           is_apn_blocked = FALSE;
   boolean                           is_switch_required = FALSE;
   ds_dsd_apm_profile_id             subs_profile_id;
   ds_dsd_apm_data_block            *rule_block = NULL;
   ds_dsd_apm_data_block            *last_block = NULL;  
  ds3gsubsmgr_subs_id_e_type         subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return is_switch_required;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

#ifdef FEATURE_DATA_WLAN_MAPCON
   is_lte_attach_allowed = ds_dsd_apm_if_next_attach_apn_lte_capable (subs_id,
                                                                   profile_id);
#endif /*FEATURE_DATA_WLAN_MAPCON*/

   /*-----------------------------------------------------------------------
    Here we check for 3 Parameters before deciding we need to perform an
    additional switch or not, if any of the 3 are satisfied we switch
    1. MPPM blocking attach on an APN
    2. APN Disable
    3. APN blocking information given by MH 
   -------------------------------------------------------------------------*/

   is_apn_disabled = ds_dsd_apm_is_apn_disabled(profile_id, subs_id);

   is_apn_blocked = ds_dsd_apm_is_apn_blocked(profile_id,subs_id);

   DATA_MSG4_MED ("LTE attach status: %d, APN Disable Status: %d, APN blocked "
                  "status: %d for Profile: %d",is_lte_attach_allowed,
                  is_apn_disabled, is_apn_blocked, profile_id);

   is_switch_required = (!(is_lte_attach_allowed) || 
                                    (is_apn_blocked || is_apn_disabled));
   if(is_switch_required)
   {
     memset(&subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));
     subs_profile_id.subs_id = subs_id;
     subs_profile_id.profile_id = profile_id;
     rule_block = (ds_dsd_apm_data_block *)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                             ds_dsd_apm_compare_profile_id,
                                             &subs_profile_id);

     last_block = ds_dsd_apm_get_last_rule_block_per_subs(subs_id);

     if((rule_block == NULL) || (last_block == NULL))
     {
       DATA_ERR_FATAL("Invalid Rule block or Last block");
     }
     else
     {
       if(memcmp((void *)rule_block, (void *)last_block, 
                 sizeof(ds_dsd_apm_data_block)) == 0)
       {
         if(subs_info[subs_indx].dsd_apm_default_apm_rule_enabled == TRUE)
         {
           subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type =
                                                               ATTACH_PDN_DEFAULT;
         }
         else
         {
           subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type =
                                                               ATTACH_PDN_PRIMARY;
         }
       }
       else
       {
         subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type =
                                                              ATTACH_PDN_NEXT;
       }
     }
   }

   return is_switch_required;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_RESET_AND_COPY_ATTACH_INFO

DESCRIPTION    This utility function resets the ATTACH PDN info.
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_reset_and_copy_attach_info
(
  sys_modem_as_id_e_type            subs_id
) 
{
  attach_profile_data_block        *profile_block = NULL;
  attach_profile_data_block        *start_block = NULL;
  boolean                           is_switch_required = FALSE;
  uint16                            prev_prof_id;
  ds_dsd_apm_profile_id             subs_profile_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
   return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*-------------------------------------------------------------------------
   For debugging purpose only.
  -------------------------------------------------------------------------*/
  DATA_MSG4(MSG_LEGACY_MED,
            "Attach PDN info before re-initialize" 
            "sub: %d pdn_type: %d pdn_state: %d prof_id: %d",
            subs_id,
            subs_info[subs_indx].attach_pdn_info.pdn_type,
            subs_info[subs_indx].attach_pdn_info.state,
            subs_info[subs_indx].attach_pdn_info.attach_profile_id);

  DATA_MSG1(MSG_LEGACY_HIGH,"LTE attach allowed old value = %d",
            subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed);

  /*-------------------------------------------------------------------------
   Update the attach_pdn_info with next attach profile ID if available or
   block LTE attach if all APNs is blocked or disabled
   The logic for selecting attach profile is based on the concept of a
   circular queue
   If First entry fails, we move to the next one and keep on continuing till
   we come back the starting point at which we conclude that no profile is
   available and block LTE
  -------------------------------------------------------------------------*/
  do
  {
    prev_prof_id = subs_info[subs_indx].attach_pdn_info.attach_profile_id;

    memset(&subs_info[subs_indx].attach_pdn_info, 0, 
         sizeof(subs_info[subs_indx].attach_pdn_info));

    subs_info[subs_indx].attach_pdn_info.pdn_type = 
            subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type;
    subs_info[subs_indx].attach_pdn_info.attach_profile_id = 
            ds_dsd_apm_get_next_attach_profile_id_internal(subs_id, prev_prof_id);

    if (subs_info[subs_indx].attach_pdn_info.attach_profile_id != 
                                                    DSD_APM_INVALID_PROF_ID)
    {
      memset(&subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));

      subs_profile_id.subs_id = subs_id;
      subs_profile_id.profile_id = subs_info[subs_indx].attach_pdn_info.
                                     attach_profile_id;
      profile_block = (attach_profile_data_block *)q_linear_search(
                                             &ds_dsd_apm_attach_profile_q[subs_indx],
                                             ds_dsd_apm_compare_profile_id_prof_q,
                                             &subs_profile_id);
      /*---------------------------------------------------------------------- 
        Storing the starting point to break the circular loop
      ----------------------------------------------------------------------*/
      if(profile_block != NULL)
      {
        /*---------------------------------------------------------------------- 
          Storing the starting point to break the circular loop
        ----------------------------------------------------------------------*/
        if(!is_switch_required)
        {
          start_block = profile_block;
        }
        else
        {
          /*---------------------------------------------------------------------- 
            No profile found for attach so we will block LTE, before that we do
            state reset so as to make sure next attempt is successful
          ----------------------------------------------------------------------*/
          if((memcmp(start_block, profile_block, 
              sizeof(attach_profile_data_block)) == 0) &&
             (start_block == profile_block))
          {
            DATA_MSG0(MSG_LEGACY_ERROR, "No attach Profile available");
            subs_info[subs_indx].attach_pdn_info.attach_profile_id = prev_prof_id;
            break;
          }
        }
        /*---------------------------------------------------------------------- 
          This function checks if the currently selected profile is still
          available for attach and has not been blocked/disabled
        ----------------------------------------------------------------------*/
        is_switch_required = ds_dsd_apm_is_additonal_apn_switch_required (subs_id,
                             subs_info[subs_indx].attach_pdn_info.attach_profile_id);
      }
      else
      {
        DATA_ERR_FATAL("Possible Profile Queue corruption");
      }
    }
    else
    {
      /*---------------------------------------------------------------------- 
        Done to break out of the while loop in case an Invalid profile is
        returned at first attempt
      ----------------------------------------------------------------------*/
      is_switch_required = FALSE; 
    }
  } while (is_switch_required);

  if(subs_info[subs_indx].attach_pdn_info.attach_profile_id != 
      DSD_APM_INVALID_PROF_ID && !(is_switch_required))
  {
    subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed = TRUE;
  }
  else
  {
    subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed = FALSE;
  }

  subs_info[subs_id].attach_pdn_info.attach_pdp_type = DS_PROFILE_3GPP_PDP_MAX;

  DATA_MSG3(MSG_LEGACY_MED,
            "After Re-initialize PDN TYPE =%d PDN STATE =%d PROF_ID =%d",
            subs_info[subs_indx].attach_pdn_info.pdn_type,
            subs_info[subs_indx].attach_pdn_info.state,
            subs_info[subs_indx].attach_pdn_info.attach_profile_id);

  DATA_MSG1(MSG_LEGACY_MED,"LTE attach allowed: %d",
                subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed);
  
  return;
} /* ds_dsd_apm_reset_and_copy_attach_info() */


/*===========================================================================
FUNCTION       DS_DSD_APM_DECIDE_NEXT_ATTACH_PROF

DESCRIPTION    This function uses the action to switch to Next attach APN
 
PARAMTERS      subs_id: Current active PS subscription 
               profile_id: Current attach profile ID 

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_decide_next_attach_prof
(
   sys_modem_as_id_e_type                   subs_id,
   uint16                                   profile_id
)
{
   ds_dsd_apm_profile_id                    subs_profile_id;
   ds_dsd_apm_data_block                   *rule_block = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

   memset(&subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));

   subs_profile_id.profile_id = profile_id;
   subs_profile_id.subs_id = subs_id;

   rule_block = (ds_dsd_apm_data_block *)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_profile_id,
                                                &subs_profile_id);
  /*---------------------------------------------------------------------- 
    This function applies the action rule to switch to next appropriate APN
    when SM is in not in a valid state
  ----------------------------------------------------------------------*/

   if(rule_block != NULL)
   {
     ds_dsd_apm_action_rule(subs_id, 
                   rule_block->apn_info.AttachPdn.action_rule.attach_blocked);

     if(subs_info[subs_indx].dsd_apm_default_apm_rule_enabled)
     {
       subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type
                                                        = ATTACH_PDN_DEFAULT;
     }
   }
   else
   {
     DATA_MSG0(MSG_LEGACY_ERROR,
               "Cannot perform APN Switching, invalid rule block");
   }
}
/*===========================================================================
FUNCTION       DS_DSD_APM_APN_DISABLE_CB_HDLR

DESCRIPTION    Callback handler to process APN disable indication 
 
PARAMTERS      PROFILE_ID : profile that has changed 

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_disable_cb_hdlr
(
  uint16                  profile_id,
  sys_modem_as_id_e_type  subs_id,
  boolean                 apn_disable_flag
)
{
  attach_profile_data_block       *profile_block_ptr = NULL;
  ds_cmd_type                     *cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_apm_apn_disable_cb_hdlr. profile_id:%d",
            profile_id);

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(attach_profile_data_block));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_APM_APN_DISABLE;
  profile_block_ptr = (attach_profile_data_block*)cmd_ptr->cmd_payload_ptr;

  /*------------------------------------------------------------------
    Initialize and copy the profile id
  -------------------------------------------------------------------*/
  memset(profile_block_ptr,0,sizeof(attach_profile_data_block));

  profile_block_ptr->attach_profile.subs_id = subs_id;
  profile_block_ptr->attach_profile.profile = profile_id;
  profile_block_ptr->attach_profile.apn_disabled = apn_disable_flag;
  ds_put_cmd(cmd_ptr);

  return;
}/* ds_dsd_apm_apn_disable_cb_hdlr() */

/*===========================================================================
FUNCTION       DS_DSD_APM_UPDATE_APN_DISABLE_FLAG

DESCRIPTION    Updating the internal structure to keep track of APN's available 
               for switching 
 
PARAMTERS      profile_id : Profile whose APN is disabled 

DEPENDENCIES   NONE

RETURN VALUE   TRUE: If Internal Structures were updated succesfully 
               FALSE: If Updated failed 

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_update_apn_disable_flag
(
   sys_modem_as_id_e_type              subs_id,
   uint16                              profile_id,
   boolean                             apn_disable
)
{
  attach_profile_data_block           *profile_block = NULL;
  boolean                              ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  profile_block = (attach_profile_data_block *)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);

  while (profile_block != NULL)
  {
    if (profile_block->attach_profile.profile == profile_id)
    {
      profile_block->attach_profile.apn_disabled = apn_disable;
      ret_val = TRUE;
    }
    profile_block = (attach_profile_data_block *)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                        &profile_block->link);
  }

  rex_leave_crit_sect(&ds_dsd_apm_critical_section);

  return ret_val;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_APN_DISABLE_CMD_PROCESS_INTERNAL

DESCRIPTION    Processing of DSD APM DISABLE change command 
 
PARAMTERS      data_ptr : User data passed by command posting function 

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_disable_cmd_process_internal
(
  ds_cmd_type  *cmd_ptr
)
{
  attach_profile_data_block  *profile_block = NULL;
  boolean                     ret_val = FALSE;
  sys_modem_as_id_e_type      subs_id = SYS_MODEM_AS_ID_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"NULL ptr passed, return");
    return;
  }

  profile_block = (attach_profile_data_block *)(cmd_ptr->cmd_payload_ptr);
  subs_id = profile_block->attach_profile.subs_id;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  ret_val = ds_dsd_apm_update_apn_disable_flag(
              subs_id,
              profile_block->attach_profile.profile,
              profile_block->attach_profile.apn_disabled);

  DATA_MSG1 (MSG_LEGACY_MED, "APN Disable Flag, Updated Status: %d",
             ret_val);

  return;
} /* ds_dsd_apm_apn_disable_cmd_process_internal() */

/*===========================================================================

                          EXTERNAL INTERFACE IMPLEMENTATION

===========================================================================*/
/*===========================================================================
FUNCTION      DS_DSD_APM_INIT_INTERNAL

DESCRIPTION   This function initializes APM module
 
DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  1. APN INFO, ATTACH_PDN_INFO and NEXT_ATTACH_PDN_INFO 
                 data structure intialized. 
              2. Config rule and action rules are populated
                   - either reading from EFS or from static file.
===========================================================================*/
void ds_dsd_apm_init_internal(void)
{
  uint8                         i=0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
  Initialize the critical section.
  -------------------------------------------------------------------------*/
  memset(&ds_dsd_apm_critical_section, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&ds_dsd_apm_critical_section);

  memset(&ds_dsd_apm_crtc_sec_apn_switch, 0, sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&ds_dsd_apm_crtc_sec_apn_switch);

  for (i = DS3GSUBSMGR_SUBS_ID_MIN; i < DS3GSUBSMGR_SUBS_ID_COUNT; i++)
  {
    memset(&subs_info[i].attach_pdn_info, 0, sizeof(subs_info[i].attach_pdn_info));
    memset(&subs_info[i].next_attach_pdn_info, 0, sizeof(subs_info[i].next_attach_pdn_info));
    memset(&subs_info[i].apm_lte_plmn_id, 0, sizeof(subs_info[i].apm_lte_plmn_id));
    
    subs_info[i].dsd_apm_apn_switching_enable = TRUE;
    subs_info[i].dsd_apm_lte_apn_blocking_support = FALSE;
    subs_info[i].dsd_apm_profile_id_based_switch = TRUE;
    subs_info[i].dsd_apm_default_apm_rule_enabled = FALSE;
    subs_info[i].default_attach_apn_allowed = TRUE;
    subs_info[i].apm_attach_apn_all_blocked = FALSE;
    subs_info[i].attach_pdn_info.if_lte_attach_allowed = TRUE; 
    subs_info[i].attach_pdn_info.attach_pdp_type = DS_PROFILE_3GPP_PDP_MAX;

  /*-------------------------------------------------------------------------
  Initialize the APM rule Queue. It is done before reading the NV item so 
  that the QUEUE library can be used as UTILITY for furture. E.g. QUEUE SIZE 
  -------------------------------------------------------------------------*/
    q_init(&ds_dsd_apm_rules_q[i]);

  /*------------------------------------------------------------------
  Initialize the attach profile Queue.
  -------------------------------------------------------------------*/
    q_init(&ds_dsd_apm_attach_profile_q[i]);

  /*------------------------------------------------------------------
  Initialize the attach pdn priority Queue. It is used for LTE APN blocking. 
  By default, UE read attach profile id from EFS, and this queue is used 
  to store all blocked APNs received from 3GPP MH; 
  If attach profile id switching is off, UE read all the APN names from 
  APN name priority EFS files, and store the blocked info for these APN 
  names. 
  -------------------------------------------------------------------*/
    q_init(&ds_dsd_apm_apn_priority_q[i]);

     /*-----------------------------------------------------------------------
    Initialize the apn attach capable Queue.It stores the attach_allowed
    passed by MPPM module. If WLAN is available, a certain attach APN may not 
    able to perform LTE attach.
    -----------------------------------------------------------------------*/
    q_init(&ds_dsd_apm_apn_attach_cap_q[i]);
  }

#ifdef FEATURE_DATA_WLAN_MAPCON
  memset(&ds_dsd_apm_lte_capable_critical_section, 0, 
         sizeof(rex_crit_sect_type));
  rex_init_crit_sect(&ds_dsd_apm_lte_capable_critical_section);

  /*---------------------------------------------------------------------------
   Register LTE Attach allow event with MPPM to know if certain APN is currently
   can do LTE attach or not
  ----------------------------------------------------------------------------*/
  if(ds_wlan_get_wlan_offload_config_nv() ==  DS_WLAN_OFFLOAD_CONFIG_IWLAN_S2B)
  {
    if( ds_mppm_event_reg_client( DS_MPPM_AGGR_IS_LTE_ATTACH_CAP_EV,
                                  DS_MPPM_TYPE_EXTERNAL_CLIENT,
                                  ds_dsd_apm_apn_attach_allowed_chg_cback,
                                  NULL,
                                  NULL) == FALSE ) 
    {
      DATA_ERR_FATAL("Register IS_LTE_ATTACH_CAP_EV failed");
    }
  }
#endif /*FEATURE_DATA_WLAN_MAPCON*/

  /*-------------------------------------------------------------------------
   Register for APN change callback
  -------------------------------------------------------------------------*/
  ds_profile_apm_cb_func_tbl.apn_disable_flag_change_handler = 
    ds_dsd_apm_apn_disable_cb_hdlr;
  ds_profile_apm_cb_func_tbl.apn_name_change_handler = 
    NULL;
  ds_profile_apm_cb_func_tbl.link.next_ptr = NULL;
  ds_profile_3gpp_register_callback_table(&ds_profile_apm_cb_func_tbl);

  ds_dsd_apm_internal_init_per_subsid(SYS_MODEM_AS_ID_1);

  return;
}/* ds_dsd_apm_init_internal() */

/*===========================================================================
FUNCTION      DS_DSD_APM_INTERNAL_INIT_PER_SUBS_ID

DESCRIPTION   This function initializes APM module related NV items and EFS 
              items per subscription.
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Config rule and action rules are populated
                   - either reading from EFS or from static file.
===========================================================================*/
void ds_dsd_apm_internal_init_per_subsid
(
  sys_modem_as_id_e_type  subs_id
)
{
  boolean  ret_val=FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  /*-------------------------------------------------------------------------
  Read the NV item. If NV item is true, read EFS, else defaut behavior.
  -------------------------------------------------------------------------*/
  ds_dsd_apm_set_apn_switch_flag(subs_id, ds_3gpp_nv_manager_get_apm_apn_switching(subs_id));
     
  ds_dsd_apm_set_generic_switching_flag(subs_id, FALSE);
  /*-------------------------------------------------------------------------
   Set the PLMN blocking Interval to the default value
  --------------------------------------------------------------------------*/
  ds_dsd_apm_set_plmn_blocking_interval_timer(DSD_APM_PLMN_BLOCKING_INTERVAL,
                                                subs_id);

#ifdef FEATURE_DATA_REL10    /*NOTE Does this mean only rel 10 is supported on both subs id*/

  ret_val = ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id);
  subs_info[subs_id].dsd_apm_lte_apn_blocking_support = ret_val;

  DATA_MSG1(MSG_LEGACY_HIGH,"LTE release 10 support %d",ret_val);

#endif /* FEATURE_DATA_REL10*/

  /*-------------------------------------------------------------------------
   Read APM info from EFS
  -------------------------------------------------------------------------*/
  ds_dsd_apm_info_efs_read(subs_id);
  /*--------------------------------------------------------------
  At powerup, be default, APM will not run any rule.
  ----------------------------------------------------------------*/
  ds_dsd_apm_unset_ip_rule_required(subs_id);
  
} /* ds_dsd_apm_internal_init_per_subsid() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif

/*===========================================================================
FUNCTION ds_dsd_apm_is_dns_and_pcscf_addr_info_present_and_valid

DESCRIPTION
  This function checks to see if Network has provided DNS & PCSCF address
  information to UE

PARAMETERS 
  subs_id: Current active Packet Service subscription

DEPENDENCIES
  None

RETURN VALUE
  ds_dsd_apm_pcscf_dns_addr_state_type -
       Enum that tells whether PCSCF or DNS was received successfully or not
   
SIDE EFFECTS
  None
===========================================================================*/
ds_dsd_apm_pcscf_dns_addr_state_type 
                     ds_dsd_apm_is_pcscf_and_dns_addr_info_present_and_valid
(
  sys_modem_as_id_e_type              subs_id
)
{
  ds_dsd_apm_pcscf_dns_addr_state_type  ret_val = DS_DSD_APM_PCSCF_DNS_NOT_RCVD;
  ds_dsd_apm_nw_state_type              dns_addr_valid = DS_DSD_APM_NOT_RCVD;
  ds_dsd_apm_nw_state_type              pcscf_addr_valid = DS_DSD_APM_NOT_RCVD;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*--------------------------------------------------------------
   Here we check if DNS/PCSCF address is received successfully or not
   
   1. If both v4 and v6 address have failed then we mark it as invalid
   2. If either v4 or v6 is successfully received, we mark it valid
   3. Else we consider it as NOT_RECVD
  ----------------------------------------------------------------*/
  if(DS_DSD_APM_RCVD_FAIL == 
               subs_info[subs_indx].attach_pdn_info.ipv4_dns_address &&
     DS_DSD_APM_RCVD_FAIL == 
               subs_info[subs_indx].attach_pdn_info.ipv6_dns_address)
  {
    dns_addr_valid = DS_DSD_APM_RCVD_FAIL;
  }
  else if (DS_DSD_APM_RCVD_SUCC == 
               subs_info[subs_indx].attach_pdn_info.ipv4_dns_address ||
           DS_DSD_APM_RCVD_SUCC == 
               subs_info[subs_indx].attach_pdn_info.ipv6_dns_address)
  {
    dns_addr_valid = DS_DSD_APM_RCVD_SUCC;
  }

  if(DS_DSD_APM_RCVD_FAIL == 
               subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address &&
     DS_DSD_APM_RCVD_FAIL == 
               subs_info[subs_indx].attach_pdn_info.ipv6_pcscf_address)
  {
    pcscf_addr_valid = DS_DSD_APM_RCVD_FAIL;
  }
  else if (DS_DSD_APM_RCVD_SUCC == 
           subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address ||
           DS_DSD_APM_RCVD_SUCC == 
           subs_info[subs_indx].attach_pdn_info.ipv6_pcscf_address)
  {
    pcscf_addr_valid = DS_DSD_APM_RCVD_SUCC;
  }

  if(DS_DSD_APM_RCVD_FAIL == dns_addr_valid &&
     DS_DSD_APM_RCVD_FAIL == pcscf_addr_valid)
  {
    ret_val = DS_DSD_APM_PCSCF_DNS_INVALID;
  }
  else if(DS_DSD_APM_RCVD_SUCC == dns_addr_valid ||
          DS_DSD_APM_RCVD_SUCC == pcscf_addr_valid)
  {
    ret_val = DS_DSD_APM_PCSCF_DNS_VALID;
  }

  return ret_val;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_ATTACH_COMPLETED_INTERNAL

DESCRIPTION    This is the utility function to check if ATTACH COMPLETED 
               from APM modules perspective 
  
DEPENDENCIES   NONE

RETURN VALUE   TRUE/FALSE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_attach_completed_internal
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   print_detail
)
{
  boolean                retVal = TRUE;
  apn_info_type          *apn_info_ptr; 
  attach_pdn_info_type   *attach_pdn_info_ptr; 
  ds_dsd_apm_data_block  *rule_block;
  ds_dsd_apm_attach_pdn  attach_pdn;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  attach_pdn_info_ptr = &subs_info[subs_indx].attach_pdn_info;
  attach_pdn.subs_id = subs_id;
  attach_pdn.attach_pdn = attach_pdn_info_ptr->pdn_type;

  if(attach_pdn_info_ptr->state != ATTACH_PDN_STATE_CONNECTED)
  {
    DATA_MSG2(MSG_LEGACY_LOW,
              "APM attach not complete on sub %d, APM state %d, return FALSE",
              subs_id,
              attach_pdn_info_ptr->state);
    return FALSE;
  }

  /*--------------------------------------------------------------
   Attach PDN does not need to run rules, and LTE attach complete
   received, return TRUE
  ----------------------------------------------------------------*/ 


  rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_apn_type,
                                                           &attach_pdn);

  if((attach_pdn_info_ptr->is_tlb_mode_active == TRUE) ||
     (attach_pdn_info_ptr->ip_rule_required == FALSE)  ||
     ( rule_block == NULL ))
  {
    DATA_MSG3(MSG_LEGACY_ERROR,
              "APM attach complete on sub %d ,TLB Mode: %d , IP rule: %d, ",
              subs_id,
              attach_pdn_info_ptr->is_tlb_mode_active,
              attach_pdn_info_ptr->ip_rule_required);
    return TRUE;
  }

    apn_info_ptr = &(rule_block->apn_info);

    if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_IP_ADDRESS)
    {
      if (print_detail)
      {
        DATA_MSG4(MSG_LEGACY_MED,"attach pdn type: %d,attach PDN IP addr rule %d,"
                                 " IPv4 addr: %d ,IPV6 addr: %d",
                                 attach_pdn_info_ptr->pdn_type,
                                 apn_info_ptr->AttachPdn.config_rule.ip_address,
                                 attach_pdn_info_ptr->ipv4_address,
                                 attach_pdn_info_ptr->ipv6_address);
      }

    switch(apn_info_ptr->AttachPdn.config_rule.ip_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        /*--------------------------------------------------------------
        Change logic so that APM waits till: 
        1. V4 completes and V6 completes 
        2. V4 fails and V6 completes 
        3. V4 completes and V6 fails. 
         
        That way, ATTACH PDN announcement is after V6 iface up (mostly) 
        ----------------------------------------------------------------*/
        if(attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_NOT_RCVD)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_NOT_RCVD)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv4_address == DS_DSD_APM_RCVD_FAIL && 
           attach_pdn_info_ptr->ipv6_address == DS_DSD_APM_RCVD_FAIL)
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }


  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_PCSCF)
  {
    if (print_detail)
    {
      DATA_MSG4(MSG_LEGACY_MED,"attach PDN type %d, PCSCF address rule %d,"
                             "PCSCF IPV4 addr: %d,PCSCF IPV6 addr: %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.pcscf_address,
              attach_pdn_info_ptr->ipv4_pcscf_address,
              attach_pdn_info_ptr->ipv6_pcscf_address);
    }
    switch(apn_info_ptr->AttachPdn.config_rule.pcscf_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_pcscf_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_pcscf_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_pcscf_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_pcscf_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        if((attach_pdn_info_ptr->ipv4_pcscf_address != DS_DSD_APM_RCVD_SUCC) && 
           (attach_pdn_info_ptr->ipv6_pcscf_address != DS_DSD_APM_RCVD_SUCC))
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }

  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & CONFIG_DNS_ADDRESS)
  {
    if (print_detail)
    {
    DATA_MSG4(MSG_LEGACY_MED,"attach PDN type %d, DNS address rule %d ,"
                             "DNS IPv4 Addr: %d, DNS IPV6 addr: %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.dns_address,
              attach_pdn_info_ptr->ipv4_dns_address,
              attach_pdn_info_ptr->ipv6_dns_address);
    }
    switch(apn_info_ptr->AttachPdn.config_rule.dns_address)
    {
      case ipv4_mandatory_and_ipv6_donotcare:
        if(attach_pdn_info_ptr->ipv4_dns_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv6_mandatory_and_ipv4_donotcare:
        if(attach_pdn_info_ptr->ipv6_dns_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_mandatory_and_ipv6_mandatory:
        if(attach_pdn_info_ptr->ipv4_dns_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        if(attach_pdn_info_ptr->ipv6_dns_address != DS_DSD_APM_RCVD_SUCC)
        {
          return FALSE;
        }
        break;

      case ipv4_dontcare_and_ipv6_donotcare:
        break;

      case ipv4_mandatory_or_ipv6_mandatory:
        if((attach_pdn_info_ptr->ipv4_dns_address != DS_DSD_APM_RCVD_SUCC) && 
           (attach_pdn_info_ptr->ipv6_dns_address != DS_DSD_APM_RCVD_SUCC))
        {
          return FALSE;
        }
        break;

      default:
        {
          return TRUE;
        }
        break;
    }
  }
  /*---------------------------------------------------------------
    We need to make sure if PCSCF_OR_DNS address rule is specified
    then either PCSCF or DNS address is received successfully before
    posting FULL service indication to external clients
  ----------------------------------------------------------------*/
  if(apn_info_ptr->AttachPdn.config_rule.config_valid_flag & 
                                                    CONFIG_PCSCF_DNS_ADDRESS)
  {
    if (print_detail)
    {
    DATA_MSG2(MSG_LEGACY_MED,"attach PDN type %d, PCSCF_DNS address rule %d",
              attach_pdn_info_ptr->pdn_type,
              apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address);
    }

    if(ipv4_mandatory_or_ipv6_mandatory == 
                    apn_info_ptr->AttachPdn.config_rule.pcscf_or_dns_address)
    {
      if(DS_DSD_APM_PCSCF_DNS_NOT_RCVD ==
            ds_dsd_apm_is_pcscf_and_dns_addr_info_present_and_valid(subs_id))
      {
        DATA_MSG0(MSG_LEGACY_MED,
                  "Waiting for valid DNS or PCSCF address information");
        return FALSE;
      }
    }
   }

  DATA_MSG0(MSG_LEGACY_LOW, "APM rule is satisfied, LTE is in full service");
  return retVal;
}/* ds_dsd_apm_attach_completed() */

/*===========================================================================
FUNCTION       DS_DSD_APM_PAYLOAD_PROCESSING

DESCRIPTION    This is utility function for payload processing

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_payload_processing
(
  ds_dsd_apm_ind_pay_load            *pay_load
)
{
  uint16                              attach_profile_id;
  sys_modem_as_id_e_type              subs_id  = SYS_MODEM_AS_ID_NONE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pay_load == NULL)
  {
    return;
  }

  subs_id = pay_load->subs_id;

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*--------------------------------------------------------------
  Process IP field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_DSD_IND_IP_ADDRESS_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Check attach profile PDN type and if attach profile is v4 only, 
    set v6 address failure; if attach profile is v6 only, set v4
    address failure. Since in this case, MH will not send v4/v6 address 
    indication. 
    ----------------------------------------------------------------*/
    attach_profile_id = subs_info[subs_indx].attach_pdn_info.attach_profile_id;

    DATA_MSG2(MSG_LEGACY_HIGH,
                "ds_dsd_apm_payload_processing on sub: %d, pdn type: %d",
                 subs_id, subs_info[subs_id].attach_pdn_info.attach_pdp_type );

    switch( subs_info[subs_id].attach_pdn_info.attach_pdp_type )
      {
        case DS_PROFILE_3GPP_PDP_IP:
          subs_info[subs_indx].attach_pdn_info.ipv6_address = DS_DSD_APM_RCVD_FAIL;
          break;

        case DS_PROFILE_3GPP_PDP_IPV6:
          subs_info[subs_indx].attach_pdn_info.ipv4_address = DS_DSD_APM_RCVD_FAIL;
          break;

        default:
          break;

      }
    }

    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->ip_address.field_mask & DS_DSD_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->ip_address.v4_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_address = DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_address = DS_DSD_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->ip_address.field_mask & DS_DSD_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->ip_address.v6_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_address = DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_address = DS_DSD_APM_RCVD_FAIL;
      }
    }

  /*--------------------------------------------------------------
  Process PCSCF field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_DSD_IND_PCSCF_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->pcscf_address.field_mask & DS_DSD_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->pcscf_address.v4_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address = DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_pcscf_address  = DS_DSD_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->pcscf_address.field_mask & DS_DSD_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->pcscf_address.v6_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_pcscf_address = DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_pcscf_address = DS_DSD_APM_RCVD_FAIL;
      }
    }
  }

  /*--------------------------------------------------------------
  Process DNS field mask
  ----------------------------------------------------------------*/
  if((pay_load->ind_field_mask & DS_DSD_IND_DNS_ADDR_FIELD_MASK))
  {
    /*--------------------------------------------------------------
    Process V4 field mask
    ----------------------------------------------------------------*/
    if((pay_load->dns_address.field_mask & DS_DSD_APM_IP_V4_FIELD_MASK))
    {
      if(pay_load->dns_address.v4_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_dns_address = 
                                                   DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv4_dns_address = 
                                                   DS_DSD_APM_RCVD_FAIL;
      }
    }
    /*--------------------------------------------------------------
    Process V6 field mask
    ----------------------------------------------------------------*/
    if((pay_load->dns_address.field_mask & DS_DSD_APM_IP_V6_FIELD_MASK))
    {
      if(pay_load->dns_address.v6_address == TRUE)
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_dns_address = 
                                                   DS_DSD_APM_RCVD_SUCC;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.ipv6_dns_address = 
                                                   DS_DSD_APM_RCVD_FAIL;
      }
    }
  }

  return;
}/*ds_dsd_apm_payload_processing()*/


/*===========================================================================
FUNCTION       DS_DSD_APM_UPDATE_ATTACH_PDN_INFO_INTERNAL

DESCRIPTION    This internal function updates the attach pdn info global 
               structure to reflect an APN Switch or an update from MPPM.
 
PARAMETERS     NONE

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Resets the global attach_pdn_info structure
===========================================================================*/
void ds_dsd_apm_update_attach_pdn_info_internal
(
   sys_modem_as_id_e_type            subs_id
)
{
  struct fs_stat                     fs_buf;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
  memset(&fs_buf, 0, sizeof(struct fs_stat));

  /*---------------------------------------------------------------------- 
    This function is responsible for calling API that re-calculate the
    attach profile if the SM is in a valid state and attach APN has changed
   
    We also need to check whether AP has updated the attach profiles list,
    if so we need to read them from the temp file and use for next attach
    accordingly
  ----------------------------------------------------------------------*/
  if(subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed == FALSE)
  {
    if(subs_info[subs_indx].dsd_apm_default_apm_rule_enabled)
    {
      subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type =
                                                          ATTACH_PDN_DEFAULT;
    }
    else
    {
      subs_info[subs_indx].next_attach_pdn_info.next_attach_pdn_type =
                                                          ATTACH_PDN_PRIMARY;
    }
  }

  if (ds_dsd_apm_is_internal_update_allowed(subs_id) == TRUE)
  {
    if( mcfg_fs_stat(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)subs_id)
          == MCFG_FS_STATUS_OK )
    {
      DATA_MSG0(MSG_LEGACY_HIGH,
                "Temp attach PDN list file present, performing EFS read");
      ds_dsd_apm_info_efs_read(subs_id);
    }
    else
    {
      dsd_apm_set_configuration(subs_id);
      ds_dsd_apm_reset_and_copy_attach_info(subs_id);
    }
  }

}

/*===========================================================================
FUNCTION       DS_DSD_APM_IND_INTERNAL

DESCRIPTION    This is the external function called by external clients. 
               The clients have to notify the DSD APM module.

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_dsd_apm_ind_internal
(
  ds_dsd_apm_ind_type        apm_ind_event, 
  ds_dsd_apm_ind_pay_load    *pay_load
)
{
  ds_dsd_ext_error_code   retVal = DS_DSD_APM_SUCCESS;
  sys_plmn_id_s_type      new_plmn_id;
  sys_modem_as_id_e_type  subs_id  = SYS_MODEM_AS_ID_NONE;
  uint16                  num_rules = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
  struct fs_stat          fs_buf;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&fs_buf, 0, sizeof(struct fs_stat));
  
  if (pay_load != NULL) 
  {
    subs_id = pay_load->subs_id;
  }
  else
  {
    subs_id = ds3g_get_ps_subs_id();
  }

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return DS_DSD_APM_INVALID_APM;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG2(MSG_LEGACY_HIGH,"ds_dsd_apm_ind_internal on sub %d: EV: %d",
            subs_id,apm_ind_event);

  switch(apm_ind_event)
  {
    case DS_DSD_APM_ATTACH_INIT_IND:
      ds_dsd_apm_update_attach_pdn_info_internal(subs_id);
      
      /*-------------------------------------------------------------- 
      LTE attach procedure starts, APM starts run rules
      ----------------------------------------------------------------*/
      ds_dsd_apm_set_ip_rule_required(subs_id);

      /*-------------------------------------------------------------- 
      Set current attach PDN state to be LTE attaching.
      ----------------------------------------------------------------*/
      subs_info[subs_indx].attach_pdn_info.state = ATTACH_PDN_STATE_CONNECTING;

      /*-------------------------------------------------------------- 
      Print APM queue content.
      ----------------------------------------------------------------*/
      ds_dsd_apm_print_apm_q_content(subs_id);

      break;

    case DS_DSD_APM_ATTACH_COMPLETE_IND:
      if( pay_load == NULL )
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD;
      }
      else
      {
        if( pay_load->ind_field_mask & DS_DSD_IND_ATTACH_TYPE_MASK )
        {
          /*--------------------------------------------------------------
           GW to LTE handoff, APM will not receive LTE_ATTACH_INIT ind.
           Re-compute next attach PDN, and reset the Attach PDN info.
          ----------------------------------------------------------------*/
          if( pay_load->attach_type == DS_DSD_3GPP_IRAT_ATTACH )
          {
            dsd_apm_set_configuration(subs_id);
            ds_dsd_apm_reset_and_copy_attach_info(subs_id);
            /*--------------------------------------------------------------
             Set attach PDN info state after re-compute next attach PDN and
             reset the Attach PDN info.
            ----------------------------------------------------------------*/
            ds_dsd_apm_unset_ip_rule_required(subs_id);
            subs_info[subs_indx].attach_pdn_info.state = ATTACH_PDN_STATE_CONNECTED;
          }
          else if ( pay_load->attach_type == DS_DSD_NORMAL_ATTACH )
          {
            subs_info[subs_indx].attach_pdn_info.state = ATTACH_PDN_STATE_CONNECTED;
            ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_ATTACH_SUCCESS);
          }
          else
          {
            retVal = DS_DSD_APM_INVALID_ATTACH_TYPE;
          }

          /*--------------------------------------------------------------
          NOTIFY DSD PREF SYS Module
          ----------------------------------------------------------------*/
          if(ds_dsd_apm_attach_completed_internal(subs_id, TRUE) == TRUE)
          {
            DATA_MSG0(MSG_LEGACY_HIGH,"... APM NOTIFIES PREF SYS");
            ds_dsd_ext_notify_pref_system_apm(
                                      ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));
            ds_dsd_apm_unset_ip_rule_required(subs_id);
          }
        }
        else
        {
          retVal = DS_DSD_APM_FIELD_MASK_NOT_SET;
        }
      }

      ds_dsd_apm_print_apm_q_content(subs_id);

      break;

    case DS_DSD_APM_DETACH_IND:
      if( subs_info[subs_indx].attach_pdn_info.ip_rule_required == TRUE )
      {
        DATA_MSG1(MSG_LEGACY_MED,"LTE detach ind received during normal "
                  "attach procedure, teardown EPC with profile %d", 
                  subs_info[subs_id].attach_pdn_info.attach_profile_id);
      }
      if(ds_dsd_apm_is_internal_update_allowed(subs_id) == FALSE)
      {
        subs_info[subs_indx].attach_pdn_info.state = ATTACH_PDN_STATE_DISCONNECTED;
        ds_dsd_apm_unset_ip_rule_required(subs_id);
      }
      /*-------------------------------------------------------------- 
        APN Switch desired on detach for certain cause codes, if the
        flag is set we invoke the action rule for Switching APN
      ----------------------------------------------------------------*/
      if(pay_load != NULL)
      {
        if(pay_load->ind_field_mask & DS_DSD_IND_APN_SWITCH_MASK)
        {
          if(pay_load->switch_apn == TRUE)
          {
            DATA_MSG0(MSG_LEGACY_MED,"APN Switch Required on Detach. Run Rule");
            ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_DETACH_NORMAL);
          }
        }
      }

      /* Read the Temp file after detach otherwise attach profile update from AP
         wouldn't be successful */
      if( mcfg_fs_stat(DSD_APM_TMP_ATTACH_PROFILE_FILE,
                     &fs_buf,
                     MCFG_FS_TYPE_EFS,
                     (mcfg_fs_sub_id_e_type)subs_id)
          == MCFG_FS_STATUS_OK )
      {
        DATA_MSG0(MSG_LEGACY_HIGH,
                  "Pending attach PDN list file present, performing EFS read");
        ds_dsd_apm_info_efs_read(subs_id);
      }
      break;

    case DS_DSD_APM_ATTACH_REJECT_IND:
      subs_info[subs_indx].attach_pdn_info.state = ATTACH_PDN_STATE_DISCONNECTED;
      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD; 
      }
      else
      {
        if((pay_load->ind_field_mask & DS_DSD_IND_PROFILE_ID_FIELD_MASK))
        {
          if(pay_load->profile_id == subs_info[subs_indx].attach_pdn_info.attach_profile_id)
          {
            if((pay_load->ind_field_mask & DS_DSD_IND_APN_SWITCH_MASK))
            {
              if (pay_load->switch_apn)
              {
                DATA_MSG0(MSG_LEGACY_MED,"APN Switch Required. Run Rule");
                ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_ATTACH_FAIL);
              }
            }
            else
            {
              retVal = DS_DSD_APM_FIELD_MASK_NOT_SET;
            }
          }
          else
          {
            DATA_MSG2(MSG_LEGACY_LOW,"Profile id passed %d does not match %d", 
                      pay_load->profile_id, 
                      subs_info[subs_indx].attach_pdn_info.attach_profile_id);
            retVal = DS_DSD_APM_INVALID_PROF_ID;
          }
        }
        else
        {
          retVal = DS_DSD_APM_INVALID_PROF_ID;
        }
        ds_dsd_apm_unset_ip_rule_required(subs_id);
        }
      break;

    case DS_DSD_APM_IP_ADDRESS_IND:
      if( subs_info[subs_indx].attach_pdn_info.ip_rule_required == FALSE )
      {
        DATA_MSG0(MSG_LEGACY_HIGH,
                  "APM rule is not required, ignore IP/PCSCF addr ind");
        return DS_DSD_APM_SUCCESS;
      }

      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD; 
      }
      else
      {
        if((pay_load->ind_field_mask & DS_DSD_IND_PROFILE_ID_FIELD_MASK))
        {
          if(pay_load->profile_id == subs_info[subs_indx].attach_pdn_info.attach_profile_id)
          {
            ds_dsd_apm_payload_processing(pay_load);

            /*--------------------------------------------------------------
              Here we take the action only if we know both DNS and PCSCF
              address is invalid
            ----------------------------------------------------------------*/
            if(DS_DSD_APM_PCSCF_DNS_INVALID == 
                ds_dsd_apm_is_pcscf_and_dns_addr_info_present_and_valid(subs_id))
            {
              ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_PCSCF_DNS_ADDRESS);
            }

            /*--------------------------------------------------------------
             RUN IP ADDRESS RULE 
            ----------------------------------------------------------------*/
            if((pay_load->ind_field_mask & DS_DSD_IND_IP_ADDRESS_FIELD_MASK))
            {
              ds_dsd_apm_rule(subs_id,DSD_DSD_APM_RULE_IP_ADDRESS);
            }

            /*--------------------------------------------------------------
            RUN PCSCF RULE 
            ----------------------------------------------------------------*/
            if((pay_load->ind_field_mask & DS_DSD_IND_PCSCF_FIELD_MASK))
            {
              ds_dsd_apm_rule(subs_id,DS_DSD_APM_RULE_PCSCF_ADDRESS);
            }

            /*--------------------------------------------------------------
            RUN DNS RULE 
            ----------------------------------------------------------------*/
            if(pay_load->ind_field_mask & DS_DSD_IND_DNS_ADDR_FIELD_MASK)
            {
              ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_DNS_ADDRESS);
            }

            if(ds_dsd_apm_attach_completed_internal(subs_id, TRUE) == TRUE)
            {
              DATA_MSG0(MSG_LEGACY_HIGH,"... APM NOTIFIES PREF SYS");
              /*--------------------------------------------------------------
              APM verified APM rules
              ----------------------------------------------------------------*/
              ds_dsd_ext_notify_pref_system_apm(
                                      ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id));
              ds_dsd_apm_unset_ip_rule_required(subs_id);
            }
          }
          else
          {
            DATA_MSG2(MSG_LEGACY_LOW,"Profile id passed %d does not match %d", 
                      pay_load->profile_id, 
                      subs_info[subs_id].attach_pdn_info.attach_profile_id);
            retVal = DS_DSD_APM_INVALID_PROF_ID;
          }
        }
        else
        {
          retVal = DS_DSD_APM_FIELD_MASK_NOT_SET;
        }
      }
      break;
      
    case DS_DSD_APM_PLMN_CHANGED_IND:
      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD; 
      }
      else
      {
        if( pay_load->ind_field_mask & DS_DSD_IND_PLMN_FIELD_MASK )
        {
          memset(&new_plmn_id, 0, sizeof(new_plmn_id));
          memscpy(&new_plmn_id,sizeof(new_plmn_id), 
                  &(pay_load->plmn_id), sizeof(sys_plmn_id_s_type));
        /*--------------------------------------------------------------
         Check if PLMN changed
        ----------------------------------------------------------------*/
          DATA_MSG3(MSG_LEGACY_MED,"Get APM PLMN id %d %d %d",
                   new_plmn_id.identity[0],
                   new_plmn_id.identity[1],
                   new_plmn_id.identity[2]);

          if(subs_info[subs_indx].apm_lte_plmn_id.identity[0] == 0 &&
             subs_info[subs_indx].apm_lte_plmn_id.identity[1] == 0 &&
             subs_info[subs_indx].apm_lte_plmn_id.identity[2] == 0 )
          {
            memscpy(&subs_info[subs_indx].apm_lte_plmn_id,sizeof(sys_plmn_id_s_type), 
                    &new_plmn_id, sizeof(sys_plmn_id_s_type));
            return retVal;
          }

          if(((subs_info[subs_indx].apm_lte_plmn_id.identity[0] == new_plmn_id.identity[0]) &&
              (subs_info[subs_indx].apm_lte_plmn_id.identity[1] == new_plmn_id.identity[1]) &&
              (subs_info[subs_indx].apm_lte_plmn_id.identity[2] == new_plmn_id.identity[2])) == FALSE)
          {
            memscpy((void *)&subs_info[subs_indx].apm_lte_plmn_id, sizeof(subs_info[subs_indx].apm_lte_plmn_id), 
                    (const void *)&new_plmn_id, sizeof(new_plmn_id)); 
            ds_dsd_apm_rule(subs_id,DS_DSD_APM_RULE_PLMN);
          }
        }
        else
        {
          retVal = DS_DSD_APM_FIELD_MASK_NOT_SET;
        }
      }
      break;

    case DS_DSD_APM_SYS_CHANGED_IND:
      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD; 
      }
      else
      {
        if(pay_load->system == SYS_SYS_MODE_NO_SRV)
        {
          DATA_MSG0(MSG_LEGACY_HIGH,"UE moves to no service");
        }
        else
        {
          ds_dsd_apm_rule(subs_id,DS_DSD_APM_RULE_SYS_CHANGE);
        }
      }
      break;

    case DS_DSD_APM_RESET_ATTACH_APN_IND:

        if (pay_load == NULL)
        {
          retVal = DS_DSD_APM_NO_PAY_LOAD;
        }

        ds_dsd_apm_rule(subs_id, DS_DSD_APM_RULE_RESET_APN);  
        break;

    case DS_DSD_APM_T3402_CHG_IND:
      if (pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD;
      }
      else
      {
       if (pay_load->t3402_timer_val != DSD_APM_INVALID_T3402_TIMER_VAL)
       {
         ds_dsd_apm_set_plmn_blocking_interval_timer(pay_load->t3402_timer_val,
                                                     subs_id);
       }
       else
       {
         DATA_MSG0 (MSG_LEGACY_ERROR, "Invalid T3402 Timer value passed");
       }
      }
      break;

    case DS_DSD_APM_RPM_INIT_IND:
        subs_info[subs_indx].dsd_apm_lte_apn_blocking_support = TRUE;
        ds_dsd_apm_set_apn_switch_flag(subs_id, FALSE);
        break;

    case DS_DSD_APM_TLB_MODE_ACT_IND:
      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD;
      }
      else
      {
        subs_info[subs_indx].attach_pdn_info.is_tlb_mode_active = 
                                                 pay_load->tlb_mode_active;
        retVal = DS_DSD_APM_SUCCESS;
      }
      break;    

    case DS_DSD_APM_ATTACH_PDP_TYPE_IND:
      if(pay_load == NULL)
      {
        retVal = DS_DSD_APM_NO_PAY_LOAD;
      }
      else
      {
        /* Cache the Attach PDP type */
        subs_info[subs_id].attach_pdn_info.attach_pdp_type
          = pay_load->attach_pdp_type;

        /* Update IP Rules only if using default rules or single attach
           profile rule */
        num_rules = ds_dsd_apm_config_get_apm_rules_num(subs_id, &ds_dsd_apm_rules_q[subs_id]);
        if( (subs_info[subs_id].dsd_apm_default_apm_rule_enabled) ||
            (num_rules == DS_DSD_APM_SINGLE_QUEUE_ENTRY) )
        {
          ds_dsd_apm_update_ip_rule(subs_id, pay_load->attach_pdp_type);
        }

        retVal = DS_DSD_APM_SUCCESS;
      }
      break;

    default:
      retVal = DS_DSD_APM_INVALID_APM;
      break;
  }

  return retVal;
}/* ds_dsd_apm_ind_internal() */

/*===========================================================================
FUNCTION       DS_DSD_APM_REQ_INTERNAL

DESCRIPTION    This is the internal function called by external clients to 
               request:
                - ATTACH PROFILE_ID.
                - Mandatory P-CSCF
                - Mandatory IP
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_dsd_apm_req_internal
(
  ds_dsd_apm_req_type        apm_req_event, 
  ds_dsd_apm_req_pay_load    *pay_load,
  sys_modem_as_id_e_type     subs_id
)
{
  ds_dsd_ext_error_code   retVal = DS_DSD_APM_SUCCESS;
  apn_info_type           *apn_info_ptr; 
  ds_dsd_apm_data_block   *rule_block = NULL; 
  ds_dsd_apm_profile_id   subs_profile_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return DS_DSD_APM_INVALID_APM;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG2(MSG_LEGACY_MED,"ds_dsd_apm_req_internal on sub %d. EV:%d",
            subs_id,apm_req_event);

  if(pay_load == NULL)
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"No payload defined.");
    retVal = DS_DSD_APM_NO_PAY_LOAD;
    return retVal;
  }

  pay_load->subs_id = subs_id;

  switch(apm_req_event)
  {
    case DS_DSD_APM_ATTCH_REQ:
      if( subs_info[subs_indx].attach_pdn_info.if_lte_attach_allowed == FALSE ) 
      {
        return DS_DSD_APM_ATTACH_NOT_ALLOWED;
      }

      if ( subs_info[subs_indx].apm_attach_apn_all_blocked == TRUE ) 
      {
        DATA_MSG0(MSG_LEGACY_ERROR,"All request attach APN blocked");
        return DS_DSD_APM_ALL_ATTACH_APN_BLOCKED;
      }

      /*--------------------------------------------------------------
       If APN switching feature is disabled, pdn_type will be
       ATTACH_PDN_DEFAULT; if APN switching feature turned on properly,
       pdn_type will be a valid APN class.
      ----------------------------------------------------------------*/
      if( subs_info[subs_indx].attach_pdn_info.pdn_type == ATTACH_PDN_INVALID )
      {
        DATA_MSG0(MSG_LEGACY_ERROR,"Attach profile id is not initialized");
        pay_load->attach_apn_avail = ds_dsd_apm_if_attach_profiles_available(subs_id);
        return DS_DSD_APM_INVALID_APM;
      }
      pay_load->req_field_mask = DS_DSD_REQ_PROFILE_ID_FIELD_MASK;

      /*--------------------------------------------------------------
      Get next profile..
      ----------------------------------------------------------------*/
      pay_load->profile_id = subs_info[subs_indx].attach_pdn_info.attach_profile_id;
      DATA_MSG1(MSG_LEGACY_MED,"Return current attach profile %d", 
                pay_load->profile_id);
      break;

    case DS_DSD_APM_ATTACH_APN_AVAIL_REQ:
      if( subs_info[subs_indx].dsd_apm_profile_id_based_switch == FALSE) 
      {
        pay_load->attach_apn_avail = ds_dsd_apm_if_attach_apn_available
                                                     (&ds_dsd_apm_apn_priority_q[subs_indx]);
      }
      else
      {
        pay_load->attach_apn_avail = ds_dsd_apm_if_attach_profiles_available(subs_id);
      }
      break;

    case DS_DSD_APM_MANDATORY_PCSCF_REQ:
    {
      subs_profile_id.subs_id = subs_id;
      subs_profile_id.profile_id = pay_load->profile_id;
        rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_profile_id,
                                                         &(subs_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr =&(rule_block->apn_info);
        DATA_MSG2(MSG_LEGACY_MED,"Profile %d, PCSCF attach rule %d",
                    subs_profile_id.profile_id, pay_load->mandatory_pcscf);

        if( pay_load->rule_type == DS_DSD_APM_ATTACH_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_PCSCF_FIELD_MASK;
          pay_load->mandatory_pcscf = apn_info_ptr->AttachPdn.
                                           config_rule.pcscf_address;
        }
        else if( pay_load->rule_type == DS_DSD_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_PCSCF_FIELD_MASK;
          pay_load->mandatory_pcscf = apn_info_ptr->OnDemandPdn.
                                             config_rule.pcscf_address;
        }
        else
        {
          retVal = DS_DSD_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_MED,"No APM rule configured for profile %d",
                  subs_profile_id.profile_id);
        retVal = DS_DSD_APM_INVALID_APM;
      }
    }
    break;

    case DS_DSD_APM_MANDATORY_IP_REQ:
    {
      subs_profile_id.subs_id = subs_id;
      subs_profile_id.profile_id = pay_load->profile_id;
      rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_profile_id,
                                                      &(subs_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        DATA_MSG2(MSG_LEGACY_MED,"Profile %d, IP attach rule %d",
                  subs_profile_id.profile_id, pay_load->mandatory_ip);
        if( pay_load->rule_type == DS_DSD_APM_ATTACH_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_IP_ADDRESS_FIELD_MASK;
          pay_load->mandatory_ip = apn_info_ptr->AttachPdn.
                                          config_rule.ip_address;
        }
        else if( pay_load->rule_type == DS_DSD_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_IP_ADDRESS_FIELD_MASK;
          pay_load->mandatory_ip = apn_info_ptr->OnDemandPdn.
                                           config_rule.ip_address;
        }
        else
        {
          retVal = DS_DSD_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_MED,"No APM rule configured for profile %d",
                  subs_profile_id.profile_id);
        retVal = DS_DSD_APM_INVALID_APM;
      }
    }
    break;

    case DS_DSD_APM_MANDATORY_DNS_ADDR_REQ:
    {
      subs_profile_id.subs_id = subs_id;
      subs_profile_id.profile_id = pay_load->profile_id;
      rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_profile_id,
                                                         &(subs_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        if( pay_load->rule_type == DS_DSD_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_DNS_ADDRESS_FIELD_MASK;
          pay_load->mandatory_dns_addr = apn_info_ptr->OnDemandPdn.
                                             config_rule.dns_address;
          DATA_MSG2(MSG_LEGACY_MED,
                    "Profile %d, DNS Addr on-demand rule %d",
                    subs_profile_id.profile_id, pay_load->mandatory_dns_addr);
        }
        else
        {
          retVal = DS_DSD_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_MED,
                  "No APM rule configured for profile %d",
                  subs_profile_id.profile_id);
        retVal = DS_DSD_APM_INVALID_APM;
      }
    }
    break;

    case DS_DSD_APM_MANDATORY_PCSCF_OR_DNS_ADDR_REQ:
    {
      subs_profile_id.subs_id = subs_id;
      subs_profile_id.profile_id = pay_load->profile_id;
      rule_block = (ds_dsd_apm_data_block*)q_linear_search( &ds_dsd_apm_rules_q[subs_indx],
                                                ds_dsd_apm_compare_profile_id,
                                                         &(subs_profile_id));
      if( rule_block != NULL)
      {
        apn_info_ptr = &(rule_block->apn_info);
        if( pay_load->rule_type == DS_DSD_APM_ON_DEMAND_RULE )
        {
          pay_load->req_field_mask = DS_DSD_REQ_DNS_ADDRESS_FIELD_MASK;
          pay_load->mandatory_pcscf_or_dns_addr = apn_info_ptr->OnDemandPdn.
                                             config_rule.pcscf_or_dns_address;
          DATA_MSG2(MSG_LEGACY_MED,
                    "Profile %d, PCSCF or DNS addr on-demand rule %d",
                    subs_profile_id.profile_id, 
                    pay_load->mandatory_pcscf_or_dns_addr);
        }
        else
        {
          retVal = DS_DSD_APM_INVALID_RULE_TYPE;
        }
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_MED,
                  "No APM rule configured for profile %d",
                  subs_profile_id.profile_id);
        retVal = DS_DSD_APM_INVALID_APM;
      }
    }
    break;

    default:
      DATA_MSG1(MSG_LEGACY_HIGH,"Not supported request: %d", apm_req_event);
      retVal = DS_DSD_APM_UNSEPCIFIED;
      break;
  }

  if (retVal != DS_DSD_APM_SUCCESS) 
  {
    DATA_MSG1(MSG_LEGACY_MED," retval %d",retVal);
  }

  return retVal;
}/* ds_dsd_apm_req_internal()*/

/*===========================================================================

FUNCTION DS_DSD_APM_GET_CURRENT_PLMN_ID_INTERNAL

DESCRIPTION
  This function gets the current_plmn_id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_dsd_apm_get_current_plmn_id_internal
(
  sys_modem_as_id_e_type    subs_id,
  sys_plmn_id_s_type        *current_plmn_id
)
{
  boolean                   ret_val = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (NULL != current_plmn_id)
  {
    memscpy(current_plmn_id,sizeof(sys_plmn_id_s_type), 
            &subs_info[subs_indx].apm_lte_plmn_id, sizeof(sys_plmn_id_s_type));
    ret_val = TRUE;
  }
  return ret_val;
} /* ds3g_get_current_plmn_id_internal */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ALL_ATTACH_APN_BLOCKED

DESCRIPTION    This utility function checks if all APNs configured in EFS file 
               blocked. It is called whenever getting forbidden APN list from
               LTE MH.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_if_all_attach_apn_blocked
(
  q_type                                  *priority_q
)
{
  apn_priority_data_block                 *apn_info_block = NULL;
  boolean                                  all_blocked = TRUE;
  uint16                                   profile_ip_type = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( priority_q == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"APN prioriy queue ptr is NULL, return FALSE");
    return FALSE;
  }

  apn_info_block = (apn_priority_data_block*)q_check(priority_q);

  if ( apn_info_block == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"No valid apn in priority_q");
    all_blocked = FALSE;
  }

  while( apn_info_block != NULL ) 
  {
    /*------------------------------------------------------------------
     Check if APN is able to use for LTE attach procedure
    -------------------------------------------------------------------*/
    if ( apn_info_block->apn_block_info.apn_type != ATTACH_PDN_TYPE ) 
    {
      DATA_MSG0(MSG_LEGACY_MED,"APN type is not attach type, continue");
      apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                              &(apn_info_block->link));
      continue;
    }
    
    /*------------------------------------------------------------------
     Current implementation:
     If v4/v6 APN blocked, v4v6 dual IP profile is allowed
    -------------------------------------------------------------------*/


    profile_ip_type = (apn_info_block->apn_block_info.ip_type_requested) &
                       ~(apn_info_block->apn_block_info.ip_type_blocked); 

    DATA_MSG3(MSG_LEGACY_MED,"APN len %d, ip_type_request %d, ip_type_blocked %d,",
              apn_info_block->apn_block_info.apn_len,
              apn_info_block->apn_block_info.ip_type_requested,
              apn_info_block->apn_block_info.ip_type_blocked);

    if ( profile_ip_type != 0) 
    {
      DATA_MSG2(MSG_LEGACY_MED,"APN is not blocked, efs_config type %d, "
                "blocked type %d, continue searching",
                apn_info_block->apn_block_info.ip_type_requested,
                apn_info_block->apn_block_info.ip_type_blocked);
      all_blocked = FALSE;
      break;
    }

    apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                                  &(apn_info_block->link));
  }

  DATA_MSG1(MSG_LEGACY_MED,"ds_dsd_apm_if_all_attach_apn_blocked: %d", 
            all_blocked);
  return all_blocked;
}/* ds_dsd_apm_if_all_attach_apn_blocked() */

/*===========================================================================
FUNCTION       DS_DSD_APM_SET_ALL_APN_BLOCKED_VAL

DESCRIPTION    Set the value of if all_apn_blocked
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_all_apn_blocked_val
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   val
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG3(MSG_LEGACY_MED,
            "ds_dsd_apm_set_all_apn_blocked_val on sub %d, val %d, old_val %d", 
            subs_id, val, subs_info[subs_indx].apm_attach_apn_all_blocked);
  rex_enter_crit_sect( &ds_dsd_apm_critical_section );
  subs_info[subs_indx].apm_attach_apn_all_blocked = val;
  rex_leave_crit_sect( &ds_dsd_apm_critical_section );
  return;
}/* ds_dsd_apm_set_all_apn_blocked_val() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_PROFILES_AVAILABLE

DESCRIPTION    This utility function check if all the attach profiles are 
               available or not. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_dsd_apm_if_attach_profiles_available
(
  sys_modem_as_id_e_type             subs_id
)
{
  attach_apn_availability_info       reason = 0;
  attach_apn_availability_info       block_reason = 0;
  attach_profile_data_block          *profile_block = NULL;
  boolean                            all_attach_profile_blocked = TRUE;
  attach_apn_availability_info       prof_avail = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return prof_avail;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);

  profile_block = (attach_profile_data_block*)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);
  if( profile_block == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"no valid attach profile id read from EFS");
    rex_leave_crit_sect(&ds_dsd_apm_critical_section);
    return DS_DSD_APM_EFS_CONFIG_ERROR;
  }

  while( profile_block != NULL ) 
  {
    if(profile_block->attach_profile.subs_id == subs_id)
    {
      prof_avail = ds_dsd_apm_if_profile_id_available(subs_id,
                                                      profile_block->
                                                      attach_profile.profile);
      if( prof_avail == DS_DSD_APM_ATTACH_APN_AVAILABLE ) 
      {
        reason = DS_DSD_APM_ATTACH_APN_AVAILABLE; 
        break;
      }
      else
      {
        DATA_MSG1(MSG_LEGACY_MED,"Profile %d is blocked, continue checking", 
                  profile_block->attach_profile.profile);
        if( prof_avail != DS_DSD_APM_ATTACH_APN_BLOCKED) 
        {
          DATA_MSG2(MSG_LEGACY_HIGH,"Profile %d is not available, "
                    "and is not blocked by network: reason: %d",
                    profile_block->attach_profile.profile,
                    prof_avail);
          all_attach_profile_blocked = FALSE;
          block_reason = prof_avail;
        }
      }
    }
    profile_block =(attach_profile_data_block*)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                  &(profile_block->link));
  }

  rex_leave_crit_sect(&ds_dsd_apm_critical_section);

  if( reason != DS_DSD_APM_ATTACH_APN_AVAILABLE ) 
  {
    if( all_attach_profile_blocked == TRUE ) 
    {
      reason = DS_DSD_APM_ATTACH_APN_BLOCKED;
    }
    else
    {
      DATA_MSG0(MSG_LEGACY_ERROR,
                "no attach profile is available, and efs/profile config error");
      reason = block_reason;
    }
    DATA_MSG1(MSG_LEGACY_MED,"All attach profiles is blocked: %d",
              all_attach_profile_blocked);
  }

  return reason;
} /* ds_dsd_apm_if_attach_profile_available() */

/*===========================================================================
FUNCTION       DS_DSD_APM_IF_ATTACH_APN_AVAILABLE

DESCRIPTION    This utility function checks if attach APN is available and why 
               it is not available. 
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
attach_apn_availability_info ds_dsd_apm_if_attach_apn_available
(
  q_type                                  *priority_q
)
{
  apn_priority_data_block                 *apn_info_block = NULL;
  boolean                                  all_blocked = TRUE;
  uint16                                   profile_ip_type = 0;
  attach_apn_availability_info             ret_val = 0;
  boolean                                  attach_apn_configed = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( priority_q == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,
              "APN prioriy queue ptr is NULL, return DS_DSD_APM_INTER_ERR");
    return DS_DSD_APM_INTER_ERR;
  }

  apn_info_block = (apn_priority_data_block*)q_check(priority_q);

  if ( apn_info_block == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_HIGH,
              "No valid apn in priority_q, return DS_DSD_APM_EFS_CONFIG_ERROR");
    return DS_DSD_APM_EFS_CONFIG_ERROR;
  }

  while( apn_info_block != NULL ) 
  {
    /*------------------------------------------------------------------
     Check if APN is able to use for LTE attach procedure
    -------------------------------------------------------------------*/
    if ( apn_info_block->apn_block_info.apn_type != ATTACH_PDN_TYPE ) 
    {
      DATA_MSG0(MSG_LEGACY_MED,"APN type is not attach type, continue");
      apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                              &(apn_info_block->link));
      continue;
    }

    /*------------------------------------------------------------------
     Attach APN is configured in EFS.
    -------------------------------------------------------------------*/    
    attach_apn_configed = TRUE;

    /*------------------------------------------------------------------
     Current implementation:
     If v4/v6 APN blocked, v4v6 dual IP profile is allowed
    -------------------------------------------------------------------*/
    profile_ip_type = (apn_info_block->apn_block_info.ip_type_requested) &
                       ~(apn_info_block->apn_block_info.ip_type_blocked); 

    if ( profile_ip_type != 0) 
    {
      all_blocked = FALSE;
      break;
    }

    DATA_MSG3(MSG_LEGACY_HIGH,"is APN blocked %d, blocked_type %d, request_ip %d",
              all_blocked,
              apn_info_block->apn_block_info.ip_type_requested,
              apn_info_block->apn_block_info.ip_type_blocked);

    apn_info_block = (apn_priority_data_block*)q_next(priority_q,
                                                  &(apn_info_block->link));
  }

  if( all_blocked ) 
  {
    if( attach_apn_configed ) 
    {
      ret_val = DS_DSD_APM_ATTACH_APN_BLOCKED;
    }
    else
    {
      ret_val = DS_DSD_APM_EFS_CONFIG_ERROR;
    }
  }
  else
  {
    ret_val = DS_DSD_APM_ATTACH_APN_AVAILABLE;
  }

  DATA_MSG1(MSG_LEGACY_HIGH,"Attach APN configurated: %d ",ret_val);

  return ret_val;
}/* ds_dsd_apm_if_attach_apn_available() */

/*===========================================================================
FUNCTION       DS_DSD_APM_REFRESH_APN_BLOCK_QUEUE

DESCRIPTION    Refresh the APN block queue which only called when profile_id_ 
               switching is FALSE(which is the default option, hardcoded now).
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_refresh_apn_block_queue
( 
  sys_modem_as_id_e_type      subs_id,
  ds_dsd_apm_block_list_type  *forbidden_apn_list
)
{
  apn_priority_data_block                           *apn_info_block = NULL;
  uint8                                             i = 0;
  uint16                                            ip_blocked = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*------------------------------------------------------------------
   Remove the stale APN block info from 
  -------------------------------------------------------------------*/

  ds_dsd_apm_free_block_queue_for_subs(subs_id, &ds_dsd_apm_apn_priority_q[subs_indx]);

  if( forbidden_apn_list == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,
              "Passed forbidden apn list is NULL, no APN blocked");
    return;
  }

  for( i=0; i<forbidden_apn_list->num_apn; i++ )
  {
    if(forbidden_apn_list->pdn_block_info[i].attach_allowed == TRUE)
    {
      DATA_MSG_SPRINTF_1(MSG_LEGACY_HIGH, 
                         "No APN Blocking needed to be considered for APN: %s",
                         forbidden_apn_list->pdn_block_info[i].apn_name);
      continue;
    }

    DATA_MSG_SPRINTF_2(MSG_LEGACY_HIGH,"APN block info passed:"
                       "apn_name %s, ip_blocked type %d",
                       (forbidden_apn_list->pdn_block_info)[i].apn_name,
                       (forbidden_apn_list->pdn_block_info)[i].ip_type_blocked);
    ip_blocked = (forbidden_apn_list->pdn_block_info)[i].ip_type_blocked;
    /*------------------------------------------------------------------
     If both v4 and v6 IP blocked, that means v4v6 profile is blocked
    -------------------------------------------------------------------*/
    if ( (ip_blocked & DS_DSD_APM_IP_V4_BLOCKED_MASK) &&
         (ip_blocked & DS_DSD_APM_IP_V6_BLOCKED_MASK) )
    {
      ip_blocked = V4_PDN_TYPE | V6_PDN_TYPE | V4_V6_PDN_TYPE;
    }
    apn_info_block = (apn_priority_data_block*)modem_mem_alloc
                                              ( sizeof(apn_priority_data_block),
                                                MODEM_MEM_CLIENT_DATA );
    if ( apn_info_block == NULL ) 
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"Allocate memory failed");
      continue;
    }

    DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                       "Add new PDN %s to block list, ip_blocked %d",
                       (forbidden_apn_list->pdn_block_info)[i].apn_name,
                        ip_blocked);
              
    memset(apn_info_block, 0, sizeof(apn_priority_data_block));
    apn_info_block->apn_block_info.apn_len = (forbidden_apn_list->
                                              pdn_block_info)[i].apn_len;
    memscpy(apn_info_block->apn_block_info.apn_name,DSD_APM_MAX_APN_STRING_LEN,
             (forbidden_apn_list->pdn_block_info)[i].apn_name,
             apn_info_block->apn_block_info.apn_len );
    apn_info_block->apn_block_info.subs_id = subs_id;
    apn_info_block->apn_block_info.ip_type_blocked = ip_blocked;
    apn_info_block->apn_block_info.apn_type = PDN_TYPE_INVALID;
    apn_info_block->apn_block_info.ip_type_requested = V4_PDN_TYPE | 
                                                       V6_PDN_TYPE | 
                                                       V4_V6_PDN_TYPE;
    q_link(apn_info_block, &(apn_info_block->link));
    q_put(&ds_dsd_apm_apn_priority_q[subs_indx], &(apn_info_block->link));
  }
  return;
} /* ds_dsd_apm_refresh_apn_block_queue() */

/*===========================================================================
FUNCTION       DS_DSD_APM_FORBIDDEN_APN_LIST_IND_INTERNAL

DESCRIPTION    Indicate the list of blocked APN name and IP type by 3GPP MH. 
               3GPP MH will call this indication at power up and whenever
               blocked APN list changed.
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_forbidden_apn_list_ind_internal
( 
  sys_modem_as_id_e_type                            subs_id,
  ds_dsd_apm_block_list_type                       *forbidden_apn_list
)
{
  boolean                                           all_apn_blocked = FALSE;
  attach_apn_availability_info                      attach_prof_avai = 0;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }

  if( forbidden_apn_list == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Passed forbidden apn list is NULL");
    return;
  }

  ds_dsd_apm_refresh_apn_block_queue(subs_id, forbidden_apn_list);

  attach_prof_avai =  ds_dsd_apm_if_attach_profiles_available(subs_id);
  DATA_MSG2(MSG_LEGACY_MED,"Forbidden List indication recevied:LTE attach "
                           "profile avai info %d on sub id: %d", 
                            attach_prof_avai,subs_id);
  if( attach_prof_avai == DS_DSD_APM_ATTACH_APN_BLOCKED ) 
  {
    all_apn_blocked = TRUE;
  }
  else
  {
    all_apn_blocked = FALSE;
  }
  ds_dsd_apm_set_all_apn_blocked_val(subs_id, all_apn_blocked);


  return;
} /* ds_dsd_apm_forbidden_apn_list_ind_internal */

/*===========================================================================
FUNCTION       DS_DSD_APM_IS_APN_SWITCH_SUPPORT(void)

DESCRIPTION    Indicate if APN switch is supported or not
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_is_apn_switch_support
(
  sys_modem_as_id_e_type    subs_id
)
{
  boolean                   is_apn_switch = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return is_apn_switch;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);


  rex_enter_crit_sect( &ds_dsd_apm_crtc_sec_apn_switch );
  is_apn_switch = subs_info[subs_indx].dsd_apm_apn_switching_enable;
  rex_leave_crit_sect( &ds_dsd_apm_crtc_sec_apn_switch );
  DATA_MSG2(MSG_LEGACY_MED,"ds_dsd_apm_is_apn_switch_support on sub %d :%d", 
            subs_id, is_apn_switch);


  return is_apn_switch;
}/* ds_dsd_apm_is_apn_switch_support() */
/*===========================================================================
FUNCTION       DS_DSD_APM_SET_APN_SWITCH_FLAG

DESCRIPTION    to set if apn switch is enabled
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_apn_switch_flag
(
  sys_modem_as_id_e_type    subs_id,
  boolean                   is_apn_switch
)
{
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);


  rex_enter_crit_sect( &ds_dsd_apm_crtc_sec_apn_switch );
  subs_info[subs_indx].dsd_apm_apn_switching_enable = is_apn_switch;
  rex_leave_crit_sect( &ds_dsd_apm_crtc_sec_apn_switch );
  DATA_MSG2(MSG_LEGACY_HIGH,"ds_dsd_apm_set_apn_switch_flag on sub %d :%d", 
            subs_id, is_apn_switch);
  return;
}/* ds_dsd_apm_set_apn_switch_flag */


/*===========================================================================
FUNCTION       DS_DSD_APM_UPDATE_APN_NAME

DESCRIPTION    Check if the passed profile_id exists in apn_rule_q.  
               If exists, update the apn_name from profile db

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_update_apn_name
(
  sys_modem_as_id_e_type    subs_id,
  uint16                    prof_id,
  q_type                    *rule_q
)
{
  ds_profile_info_type     prof_apn_name = {0,0};
  ds_dsd_apm_data_block   *rule_entry = NULL;
  boolean                  retval = FALSE;
  char                     apn_name[DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1];
  ds_dsd_apm_profile_id    subs_profile_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rule_q == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"queue is empty");
    return FALSE;
  }

  /*---------------------------------------------------------------------
    Get the rule block with matching profile id
  ---------------------------------------------------------------------*/
  subs_profile_id.subs_id = subs_id;
  subs_profile_id.profile_id = prof_id;
  rule_entry = (ds_dsd_apm_data_block*)q_linear_search(rule_q,
                                       ds_dsd_apm_compare_profile_id,
                                       &subs_profile_id);

  if( rule_entry != NULL)
  {
    /*---------------------------------------------------------------------
      Get the apn_name from profile database
    ---------------------------------------------------------------------*/
    memset(apn_name, '\0', DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
    prof_apn_name.buf = apn_name;
    prof_apn_name.len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1;

    if ( dsd_apm_get_profile_param_info(
           prof_id,
           DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
           &prof_apn_name, subs_id) == TRUE )
    {
      /*---------------------------------------------------------------------
        update the apn_name in the rule block
      ---------------------------------------------------------------------*/
      rule_entry->apn_info.apn_name_len = (uint8)prof_apn_name.len;
      memset(rule_entry->apn_info.apn_name,0,DSD_APM_MAX_APN_STRING_LEN);
      memscpy(rule_entry->apn_info.apn_name,
              DSD_APM_MAX_APN_STRING_LEN,
              prof_apn_name.buf,
              prof_apn_name.len);
      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_MED,"No APM rule configured for profile %d",
              prof_id);
    retval = FALSE;
  }

  return retval;
}/* ds_dsd_apm_update_apn_name() */

/*===========================================================================
FUNCTION       DS_DSD_APM_APN_CHANGE_CMD_PROCESS_INTERNAL

DESCRIPTION    Processing of DSD APM APN change command

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_change_cmd_process_internal
(
  ds_cmd_type  *cmd_ptr
)
{
  uint16                   profile_id = 0;
  boolean                  retval = FALSE;
  char                    *apm_rules_string=NULL;
  sys_modem_as_id_e_type   subs_id = SYS_MODEM_AS_ID_NONE;
  ds_dsd_apm_profile_id   *data_block_ptr = NULL;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"NULL ptr passed, return");
    return;
  }

  do
  {
    data_block_ptr = (ds_dsd_apm_profile_id*)cmd_ptr->cmd_payload_ptr;
    profile_id = data_block_ptr->profile_id;
    subs_id    = data_block_ptr->subs_id;

    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }
    subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

    /*---------------------------------------------------------------------
     If default APM rules are being used, do not update the APM rules cache
     and the EFS file
    ---------------------------------------------------------------------*/
    if(subs_info[subs_id].dsd_apm_default_apm_rule_enabled == TRUE)
    {
      DATA_MSG0(MSG_LEGACY_HIGH,
                "Default APM rule enabled. Not updating APM rules");
      break;
    }

    /*---------------------------------------------------------------------
      Find rule block corresponding to profile_id and update its apn name 
    ---------------------------------------------------------------------*/
    DATA_MSG2(MSG_LEGACY_HIGH,
              "ds_dsd_apm_apn_change_cmd_process on sub %d. profile_id:%d",
              subs_id,profile_id);

    retval = ds_dsd_apm_update_apn_name(subs_id, profile_id, &ds_dsd_apm_rules_q[subs_indx]);
    if(retval == FALSE)
    {
      DATA_MSG1(MSG_LEGACY_MED,
                "APM rule not configured for profile: %d, Exiting !!!!!!",
                profile_id);
      break;
    }

    if(ds_dsd_apm_is_generic_apn_switching_enabled(subs_id))
    {
      DATA_MSG0(MSG_LEGACY_HIGH,
                "No Rules files required for Generic APN Switching");
      break;
    }

    /*---------------------------------------------------------------------
      Create a new string from APM rules queue
    ---------------------------------------------------------------------*/
    apm_rules_string = (char*)modem_mem_alloc(DSD_APM_PROFILE_MAX_BUFF_SIZE,
                                              MODEM_MEM_CLIENT_DATA);
    if(apm_rules_string == NULL)
    {
      /*------------------------------------------------------------------
      This condition can not be recovered
      -------------------------------------------------------------------*/
      DATA_MSG0(MSG_LEGACY_ERROR,"APM couldn't get data_block_ptr memory " );
      break;
    }

    memset(apm_rules_string,0,DSD_APM_PROFILE_MAX_BUFF_SIZE);
    retval = dsd_apm_gen_apm_rule_string(&ds_dsd_apm_rules_q[subs_indx], 
                                         apm_rules_string, 
                                         DSD_APM_PROFILE_MAX_BUFF_SIZE,
                                         subs_id);

    if(retval == FALSE)
    {
      DATA_MSG0(MSG_LEGACY_ERROR,
                "Error generating apn rule string. Not updating apn rules");
      break;
    }

      /*---------------------------------------------------------------------
        Overwrite the existing EFS file with new string
      ---------------------------------------------------------------------*/
      dsd_apm_update_apn_rule_efs_file(subs_id, &ds_dsd_apm_rules_q[subs_indx], apm_rules_string);

  } while (0);

  if(apm_rules_string != NULL)
  {
    modem_mem_free(apm_rules_string,MODEM_MEM_CLIENT_DATA);
  }

  return;
}/* ds_dsd_apm_apn_change_cmd_process() */

/*===========================================================================
FUNCTION       DS_DSD_APM_APN_CHANGE_CB_HDLR

DESCRIPTION    Callback handler to process apn_change

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_change_cb_hdlr
(
  uint16                  profile_id,
  sys_modem_as_id_e_type  subs_id,
  void                   *data_ptr
)
{
  ds_dsd_apm_profile_id  *data_block_ptr = NULL;
  ds_cmd_type            *cmd_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_apm_apn_change_cb_hdlr. profile_id:%d subs_id:%d",
            profile_id, subs_id);

  /*--------------------------------------------------------------------
    Get command buffer for new command
  --------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_apm_profile_id));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }

  cmd_ptr->hdr.cmd_id = DS_CMD_DSD_APM_APN_CHG;
  data_block_ptr = (ds_dsd_apm_profile_id*)cmd_ptr->cmd_payload_ptr;
  /*------------------------------------------------------------------
    Initialize and copy the profile id
  -------------------------------------------------------------------*/
  memset(data_block_ptr,0,sizeof(uint16));
  data_block_ptr->profile_id = profile_id;
  data_block_ptr->subs_id = subs_id;

  ds_put_cmd(cmd_ptr);

  return;
}/* ds_dsd_apm_apn_change_cb_hdlr() */

#ifdef FEATURE_DATA_WLAN_MAPCON
/*===========================================================================
 
FUNCTION DS_DSD_APM_REFRESH_APN_ATTACH_CAPABLE_Q

DESCRIPTION
  This is a function to update lte_attach_allowed info for APNs passed by
  MPPM. Currently, this function only called in DS task, so critical section
  does not enforced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_dsd_apm_refresh_apn_attach_capable_q
(
  sys_modem_as_id_e_type                   subs_id,
  ds_mppm_per_pdn_rat_info_s_type         *pdn_info[],
  uint8                                    pdn_num
)
{
  apn_attach_capable_block                 *temp_apn_block = NULL;
  apn_attach_capable_block                 *prev_apn_block = NULL;
  uint8                                     i;
  //Check if "hit the end of original queue"
  boolean                                   orig_q_end = FALSE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !ds3gsubsmgr_is_subs_id_valid(subs_id) )
  {
    return FALSE;
  }

 if( pdn_info == NULL ) 
  {
    DATA_ERR_FATAL("Passed pdn_info ptr is NULL");
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*------------------------------------------------------------------
  Read the first entry of apn_attach_capable_q, if no valid entry, set 
  orig_q_end to be TRUE. 
  -------------------------------------------------------------------*/
  temp_apn_block = (apn_attach_capable_block*)q_check(&ds_dsd_apm_apn_attach_cap_q[subs_indx]);
  if ( temp_apn_block == NULL ) 
  {
    DATA_MSG0(MSG_LEGACY_HIGH,"No valid entry in apn_attach_capable_q");
    orig_q_end = TRUE;
  }

  /*------------------------------------------------------------------
  Check if the current apn_attach_capable_q has the entry, if yes, update 
  the entry; if no, allocate new memory and put it to the queue. 
  -------------------------------------------------------------------*/
  for( i=0; i < MIN(pdn_num, DS_MAX_PDN_COUNT); i++ ) 
  {
    if( pdn_info[i] == NULL ) 
    {
      DATA_MSG0(MSG_LEGACY_MED,"Invalid pdn_info");
      continue;
    }
    /* If current q does not have space, allocate new memeory */
    if( orig_q_end == TRUE ) 
    {
      DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                         "Allocate memory for apn %s, is_attach_allowed %d",
                         pdn_info[i]->apn_string,
                         pdn_info[i]->is_lte_attach_allowed);

      temp_apn_block = (apn_attach_capable_block*)modem_mem_alloc
                                              ( sizeof(apn_attach_capable_block),
                                                MODEM_MEM_CLIENT_DATA );
      if( temp_apn_block == NULL ) 
      {
        DATA_ERR_FATAL("Allocate memory failed");
      }
      else
      {
       /*Fill the info*/
        memset(temp_apn_block, 0, sizeof(apn_attach_capable_block));
        temp_apn_block->apn_attach_capable_info.apn_name_len = 
                              (uint8)pdn_info[i]->apn_string_len;
        strlcpy(temp_apn_block->apn_attach_capable_info.apn_name, 
                pdn_info[i]->apn_string, 
                DSD_APM_MAX_APN_STRING_LEN);

        temp_apn_block->subs_id = subs_id;
        temp_apn_block->apn_attach_capable_info.is_lte_attach_allowed = 
                         pdn_info[i]->is_lte_attach_allowed;
        /*Put the new entry to apn_attach_capable_q*/
        q_link(temp_apn_block, &(temp_apn_block->link));
        q_put(&ds_dsd_apm_apn_attach_cap_q[subs_indx], &(temp_apn_block->link));
      }
    }
    else
    {
      DATA_MSG2(MSG_LEGACY_HIGH,
                "Update the info: apn_name_len %d, is_attach_allowed %d",
                pdn_info[i]->apn_string_len, 
                pdn_info[i]->is_lte_attach_allowed);
      /*Update the new infomation*/
      memset(&(temp_apn_block->apn_attach_capable_info), 
             0, 
             sizeof(temp_apn_block->apn_attach_capable_info));
      temp_apn_block->apn_attach_capable_info.apn_name_len =
                                (uint8)pdn_info[i]->apn_string_len;
      strlcpy(temp_apn_block->apn_attach_capable_info.apn_name, 
              pdn_info[i]->apn_string,
              DS_DSD_APM_MAX_APN_STRING_LEN+1);
      temp_apn_block->apn_attach_capable_info.is_lte_attach_allowed = 
                            pdn_info[i]->is_lte_attach_allowed;
      
      /*Get the next entry in apn_attach_capable_q*/
      temp_apn_block = (apn_attach_capable_block*)q_next(&ds_dsd_apm_apn_attach_cap_q[subs_indx],
                                                      &(temp_apn_block->link));
      if( temp_apn_block == NULL ) 
      {
        orig_q_end = TRUE;
      }
    }/*Else part*/
  }/*For Loop*/

  /*------------------------------------------------------------------
   If original apn_attach_capable_q has extra memory, free the old info.
   Consider a case where original queue has 3 blocks, now the new request
   contains info for 2 blocks only so we need to remove the last block
  -------------------------------------------------------------------*/
  if( orig_q_end == FALSE ) 
  {
    while(temp_apn_block != NULL ) 
    {
      prev_apn_block = temp_apn_block;
      temp_apn_block = (apn_attach_capable_block*)q_next(&ds_dsd_apm_apn_attach_cap_q[subs_indx],
                                                         &(temp_apn_block->link));
      q_delete(&ds_dsd_apm_apn_attach_cap_q[subs_indx], &(prev_apn_block->link));
     
      modem_mem_free(prev_apn_block, MODEM_MEM_CLIENT_DATA);
    }
  }

  return TRUE;
} /* ds_dsd_apm_refresh_apn_attach_capable_q() */

/*===========================================================================
 
FUNCTION DS_DSD_APM_IS_ATTACH_ALLOWED

DESCRIPTION
  This is a util function to check if an Attach APN is currently allowed to
  do lte attach.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_dsd_apm_is_the_apn_allowed_to_attach
(
  sys_modem_as_id_e_type                subs_id,
  char                                 *apn_string,
  uint8                                 apn_len
)
{
  apn_attach_capable_block              *temp_apn_block = NULL;
  ds_dsd_apm_apn_name                   apn_name;
  boolean                               is_attach_allowed = TRUE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return FALSE;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if ((apn_string == NULL) && (apn_len!=0))
  {
    DATA_MSG0(MSG_LEGACY_ERROR,
              "NULL apn string ptr passed, returning default");
    return subs_info[subs_indx].default_attach_apn_allowed;
  }
  /*------------------------------------------------------------------
  Print the debug message, may remove later.
  -------------------------------------------------------------------*/

  DATA_MSG_SPRINTF_3(MSG_LEGACY_MED,
                     "ds_dsd_apm_is_attach_allowed on sub %d: apn_string %s,"
                     " apn_len: %d", subs_id, apn_string, apn_len);

  apn_name.subs_id = subs_id;
  apn_name.len = apn_len;

  if (apn_string == NULL)
  {
    memset(apn_name.apn_string, '\0', DSD_APM_MAX_APN_STRING_LEN);
  }
  else
  {
    strlcpy(apn_name.apn_string, 
            apn_string, 
            DSD_APM_MAX_APN_STRING_LEN);
  }

  temp_apn_block = (apn_attach_capable_block*)q_linear_search(
                                             &ds_dsd_apm_apn_attach_cap_q[subs_indx],
                                             ds_dsd_apm_compare_apn_name_apn_cap_q,
                                             &apn_name);

  if ( temp_apn_block != NULL ) 
  {
     if( temp_apn_block->apn_attach_capable_info.apn_name_len != 0) 
     {
       DATA_MSG_SPRINTF_2(MSG_LEGACY_MED,
                          "APN str: %s, is_attach_allowed: %d",
                          temp_apn_block->apn_attach_capable_info.apn_name,
                          temp_apn_block->apn_attach_capable_info.is_lte_attach_allowed);
     }
     else
     {
       DATA_MSG1(MSG_LEGACY_HIGH,"NULL APN, is_lte_attach_allowed %d",
                 temp_apn_block->apn_attach_capable_info.is_lte_attach_allowed);
     }

    is_attach_allowed = temp_apn_block->apn_attach_capable_info.
                                                     is_lte_attach_allowed;
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_HIGH,"Read the default entry: is_attach_allowed %d",
              subs_info[subs_indx].default_attach_apn_allowed);
   is_attach_allowed = subs_info[subs_indx].default_attach_apn_allowed;
  }

  return is_attach_allowed;
} /* ds_dsd_apm_is_the_apn_allowed_to_attach() */

/*===========================================================================
 
FUNCTION DS_DSD_APM_APN_ATTACH_ALLOWED_CHG_CBACK

DESCRIPTION
  Received is_attach_allowed info from MPPM for all the APNs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_dsd_apm_apn_attach_allowed_chg_cback
(
  ds_mppm_event_type                mppm_event,
  ds_mppm_event_info_s_type        *mppm_event_info_ptr,
  void                             *data_ptr
)
{
  sys_modem_as_id_e_type            subs_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( mppm_event != DS_MPPM_AGGR_IS_LTE_ATTACH_CAP_EV ||
      mppm_event_info_ptr == NULL ) 
  {
    DATA_MSG2(MSG_LEGACY_ERROR,
              "Un-registered event %d or event ptr 0x%x is NULL",
              mppm_event, mppm_event_info_ptr);
    return;
  }

  subs_id = ds3gsubsmgr_subs_id_ds3g_to_cm(
            ds3gsubsmgr_subs_id_ds_to_ds3g((ds_sys_subscription_enum_type) 
            mppm_event_info_ptr->mppm_dsd_event_info_ptr->dsd_subs_id));

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*---------------------------------------------------------------------------
   Update the default_attach_apn_allowed variable.
   Enter the critical section first, then update the gloable variable.
  ---------------------------------------------------------------------------*/
  rex_enter_crit_sect( &ds_dsd_apm_lte_capable_critical_section );

  /*---------------------------------------------------------------------------
   Read the attach allowed info from MPPM indication and store the info
   to internal APN list.
  ---------------------------------------------------------------------------*/
   
  ds_dsd_apm_refresh_apn_attach_capable_q(subs_id, 
                                          mppm_event_info_ptr->rat_aggr_info,
                                          mppm_event_info_ptr->num_pdns_config);

  //default_attach_apn_allowed = TRUE;

  if( mppm_event_info_ptr->fallback_apn_idx < 
             mppm_event_info_ptr->num_pdns_config ) 
  {
    if(mppm_event_info_ptr->rat_aggr_info[
                                mppm_event_info_ptr->fallback_apn_idx] != NULL) 
    {
      subs_info[subs_id].default_attach_apn_allowed =  mppm_event_info_ptr->rat_aggr_info
               [mppm_event_info_ptr->fallback_apn_idx]->is_lte_attach_allowed;
      DATA_MSG2(MSG_LEGACY_HIGH,
                "fallback_apn_idx %d, default_attach_apn_allowed %d",
                mppm_event_info_ptr->fallback_apn_idx,
                subs_info[subs_indx].default_attach_apn_allowed);
    }
  }

  rex_leave_crit_sect( &ds_dsd_apm_lte_capable_critical_section );
  return;
}/* ds_dsd_apm_apn_attach_allowed_chg_cback() */
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================
FUNCTION       DS_DSD_APM_SET_ATTACH_PDN_LIST

DESCRIPTION    To set attach PDN list
 
DEPENDENCIES   NONE

RETURN VALUE   0: succeeds 
              -1: fails 

SIDE EFFECTS   NONE
===========================================================================*/
int ds_dsd_apm_set_attach_pdn_list
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ps_sys_3gpp_lte_attach_pdn_list_type  *attach_list_ptr = NULL;
  ds_cmd_type                           *cmd_ptr = NULL;
  uint8                                  max_attach_apn_num = 1;
  sys_modem_as_id_e_type                *data_ptr = NULL;
  uint8                                  index = 0;
  ds_umts_pdp_profile_status_etype       result = DS_UMTS_PDP_FAIL;
  boolean                                profile_valid = FALSE;
  boolean                                is_valid = FALSE;
  ds_umts_pdp_subs_e_type                ds_subs_id = DS_UMTS_PDP_ACTIVE_SUBSCRIPTION_NONE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }

  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    DATA_MSG0(MSG_LEGACY_ERROR,"set attach pdn list, null Pointer passed");
    return -1;
  }

  if(ds_dsd_apm_is_apn_switch_support(subs_id))
  {
    max_attach_apn_num = DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX;
  }
 
  attach_list_ptr = (ps_sys_3gpp_lte_attach_pdn_list_type*)arg_val_ptr;
  if(attach_list_ptr->attach_pdn_profile_list_num > max_attach_apn_num)
  {
    *ps_errno = DS_EDBOVERFLOW;
    DATA_MSG2(MSG_LEGACY_ERROR,"Invalid number of attach PDN: %d, max:%d",
              attach_list_ptr->attach_pdn_profile_list_num, 
              max_attach_apn_num);
    return -1;
  }

  for(index = 0; index < attach_list_ptr->attach_pdn_profile_list_num;
       index++)
  {

    ds_subs_id  = dsumts_subs_mgr_get_subs_id(subs_id);
    DATA_MSG1(MSG_LEGACY_HIGH, "DS subs id is %d",ds_subs_id);
    result = ds_umts_get_pdp_profile_context_info_is_valid_per_subs (
                         attach_list_ptr->attach_pdn_profile_list[index],
                         ds_subs_id,
                         &profile_valid);

    is_valid = (result == DS_UMTS_PDP_SUCCESS) && (profile_valid == TRUE);

    DATA_MSG2(MSG_LEGACY_MED, "is profile valid: %d Profile at Index: %d",
              is_valid,index);

    if(is_valid == FALSE )
    {
      *ps_errno = DS_EINVAL;
      return -1;
    }

  }
  /*------------------------------------------------------------------
   We store the new contents in a temp file in the EFS which will be
   used for next attach 
  -------------------------------------------------------------------*/  
  if(!ds_dsd_apm_efs_overwrite_attach_pdn_list(
                     subs_id,
                     attach_list_ptr->attach_pdn_profile_list,
                     attach_list_ptr->attach_pdn_profile_list_num))
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"write to attach pdn list fails");
    *ps_errno = DS_EINVAL;
    return -1;
  }

  /*------------------------------------------------------------------
   send msg to DS task to refresh attach pdn list in internal structure
   only if ATTACH SM is not up.
  -------------------------------------------------------------------*/
  if(ds_dsd_apm_is_internal_update_allowed(subs_id))
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type));
    if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
    {
      *ps_errno = DS_EINVAL;
      return -1;
    }

    data_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
    *data_ptr = subs_id;
    cmd_ptr->hdr.cmd_id = DS_CMD_DSD_APM_REFRESH_ATTACH_PDN_LIST;
    ds_put_cmd(cmd_ptr);
  }

  *ps_errno = DS_ENOERR;
  return 0;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_ATTACH_PDN_LIST

DESCRIPTION    To get attach PDN list
 
DEPENDENCIES   NONE

RETURN VALUE   0: succeeds 
              -1: fails 

SIDE EFFECTS   NONE
===========================================================================*/
int ds_dsd_apm_get_attach_pdn_list
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ps_sys_3gpp_lte_attach_pdn_list_type *attach_list_ptr = 0;
  attach_profile_data_block            *profile_block = NULL;
  uint8                                 i = 0; 
  uint8                                 max_attach_list = 0;
  ds_sys_ioctl_3gpp_lte_max_attach_pdn_num_type max_attach_profile_num = 0;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    DATA_MSG0(MSG_LEGACY_ERROR,"get attach pdn list, null Pointer passed");
    return -1;
  }

  attach_list_ptr = (ps_sys_3gpp_lte_attach_pdn_list_type*)arg_val_ptr;
  memset(attach_list_ptr, 0, sizeof(ps_sys_3gpp_lte_attach_pdn_list_type));

  rex_enter_crit_sect(&ds_dsd_apm_critical_section);
  if(ds_dsd_apm_validate_queue_cnt_for_subs(subs_id, &ds_dsd_apm_attach_profile_q[subs_indx])==FALSE)
  {
    attach_list_ptr->attach_pdn_profile_list_num = i;
    DATA_MSG0(MSG_LEGACY_HIGH,"get attach pdn list, no attach pdn configured");
    rex_leave_crit_sect(&ds_dsd_apm_critical_section);
    return 0;
  }

  /*------------------------------------------------------------------
   The above check makes sure we have attach profiles in the queue.
   
   Now we loop through the queue and store the contents in the info
   pointer passed.
  -------------------------------------------------------------------*/
  profile_block = (attach_profile_data_block*)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);

  ds_dsd_apm_get_max_attach_profile_num(&max_attach_profile_num, subs_id, ps_errno);

  max_attach_list = min( DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX,
			 max_attach_profile_num);

  while (profile_block != NULL)
  {

    if( i >= max_attach_list)
    {
      break;
    }

    if(profile_block->attach_profile.subs_id == subs_id)
    {
      attach_list_ptr->attach_pdn_profile_list[i] = profile_block->
                                                            attach_profile.profile;
      DATA_MSG1(MSG_LEGACY_MED,"get attach pdn list, i:%d",
                attach_list_ptr->attach_pdn_profile_list[i]);
      i++;

    }
    profile_block = (attach_profile_data_block*)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                       &(profile_block->link));
  }

  rex_leave_crit_sect(&ds_dsd_apm_critical_section);
  attach_list_ptr->attach_pdn_profile_list_num = i;

  *ps_errno = DS_ENOERR;
  return 0;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_REFRESH_ATTACH_PDN_LIST

DESCRIPTION    To refresh attach pdn list in the memory, this function is called 
               each time when attach_profile_q needs to be refreshed, e.g. changes
               in attach profile list in EFS or in attach profile in NV has been
               changed. 
 
DEPENDENCIES   NONE

RETURN VALUE   

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_refresh_attach_pdn_list
(
  ds_cmd_type  *cmd_ptr
)
{
  ps_sys_3gpp_lte_attach_pdn_list_type  attach_list = {0};
  attach_profile_data_block            *profile_block = NULL;
  attach_profile_data_block            *next_profile_block = NULL;
  int16                                 ps_error = 0;
  sys_modem_as_id_e_type                subs_id = SYS_MODEM_AS_ID_NONE;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL ptr passed");
    return;
  }

  subs_id = *((sys_modem_as_id_e_type*)(cmd_ptr->cmd_payload_ptr));

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  if (ds_dsd_apm_is_internal_update_allowed(subs_id))
  {
    /*------------------------------------------------------------------
                clear old attach list cache
    -------------------------------------------------------------------*/
    
    rex_enter_crit_sect(&ds_dsd_apm_critical_section);
	
    profile_block = (attach_profile_data_block*)q_check(&ds_dsd_apm_attach_profile_q[subs_indx]);
    while(profile_block != NULL)
    {
      next_profile_block = (attach_profile_data_block*)q_next(&ds_dsd_apm_attach_profile_q[subs_indx],
                                                       &(profile_block->link));
      if (profile_block->attach_profile.subs_id == subs_id)
      {
        q_delete(&ds_dsd_apm_attach_profile_q[subs_indx], &(profile_block->link));
      }
      profile_block = next_profile_block;
    }
    rex_leave_crit_sect(&ds_dsd_apm_critical_section);

    /*------------------------------------------------------------------
                update attach list cache
    -------------------------------------------------------------------*/
    ds_dsd_apm_info_efs_read(subs_id);

    /*------------------------------------------------------------------
                convert attach list to attach array
    -------------------------------------------------------------------*/
    if(ds_dsd_apm_get_attach_pdn_list(&attach_list, subs_id, &ps_error) == 0)
    {
      /*------------------------------------------------------------------
       send indication to AP to notify the attach PDN has been changed
      -------------------------------------------------------------------*/
      ps_sys_event_ind(  
         PS_SYS_TECH_3GPP,
         PS_SYS_EVENT_3GPP_LTE_ATTACH_PDN_LIST_CHANGE,
         &attach_list, 
         &ps_error);
    }
    else
    {
      DATA_MSG1(MSG_LEGACY_ERROR, "Error retrieving attach PDN List: %d",
                ps_error);
    }
  }

  return;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_GET_MAX_ATTACH_PROFILE_NUM

DESCRIPTION    To get current maximum number of attach profiles configured 
 
DEPENDENCIES   NONE

RETURN VALUE   0: succeeds 
              -1: fails 

SIDE EFFECTS   NONE
===========================================================================*/
int ds_dsd_apm_get_max_attach_profile_num
(
  void                   *arg_val_ptr,
  sys_modem_as_id_e_type  subs_id,
  int16                  *ps_errno
)
{
  ds_sys_ioctl_3gpp_lte_max_attach_pdn_num_type *max_num_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }

  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    DATA_MSG0(MSG_LEGACY_ERROR,
              "ds_dsd_apm_get_attach_pdn_list, null Pointer passed");
    return -1;
  }
  max_num_ptr = (ds_sys_ioctl_3gpp_lte_max_attach_pdn_num_type*)arg_val_ptr;
  if(ds_dsd_apm_is_apn_switch_support(subs_id))
  {
    *max_num_ptr = DS_SYS_LTE_ATTACH_PDN_PROFILE_LIST_MAX;
  }
  else
  {
    *max_num_ptr = 1;
  }
  DATA_MSG2(MSG_LEGACY_MED,
            "ds_dsd_apm_get_attach_pdn_list on sub %d, max num: %d",
            subs_id, *max_num_ptr);
  return 0;
}
/*===========================================================================
FUNCTION       DS_DSD_APM_APN_SWITCH_STATUS_CHANGE

DESCRIPTION    IOCTL for disable/enable apn switch
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
int ds_dsd_apm_apn_switch_status_change
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ds_cmd_type                       *cmd_ptr = NULL;
  ps_sys_ioctl_lte_data_retry_type  *data_retry_ptr = NULL;
  ds_dsd_apm_apn_switch_cmd_type    *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }

  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    DATA_MSG0(MSG_LEGACY_ERROR,
              "ds_dsd_apm_apn_switch_status_change, null Pointer passed");
    return -1;
  }
  data_retry_ptr = (ps_sys_ioctl_lte_data_retry_type*)arg_val_ptr;

  if(ds_dsd_apm_is_internal_update_allowed(subs_id))
  {
    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_dsd_apm_apn_switch_cmd_type));
    if( (NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr) )
    {
     *ps_errno = DS_ENOMEM;
      return -1;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_DSD_APM_SET_APN_SWITCH_STATUS;

    data_ptr = (ds_dsd_apm_apn_switch_cmd_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->subs_id = subs_id;
    data_ptr->lte_data_retry = data_retry_ptr->lte_data_retry;

    ds_put_cmd(cmd_ptr);
    return 0;
  }
  
  DATA_MSG2(MSG_LEGACY_HIGH, "ds_dsd_apm_apn_switch_status_change,on sub %d:%d,"
            "Attach in Progress, no change permitted to the APN switching NV",
             subs_id, data_retry_ptr->lte_data_retry);
  return -1;

}
/*===========================================================================
FUNCTION       DS_DSD_APM_GET_APN_SWITCH_STATUS

DESCRIPTION    IOCTL for disable/enable apn switch
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
int ds_dsd_apm_get_apn_switch_status
(
  void                    *arg_val_ptr,
  sys_modem_as_id_e_type   subs_id,
  int16                   *ps_errno
)
{
  ps_sys_ioctl_lte_data_retry_type              *data_retry_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return -1;
  }

  if ((arg_val_ptr == NULL) || (ps_errno == NULL))
  {
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    DATA_MSG0(MSG_LEGACY_ERROR,
              "ds_dsd_apm_get_apn_switch_status, null Pointer passed");
    return -1;
  }
  data_retry_ptr = (ps_sys_ioctl_lte_data_retry_type*)arg_val_ptr;
  data_retry_ptr->lte_data_retry = ds_dsd_apm_is_apn_switch_support(subs_id);
  return 0;
}
/*===========================================================================
FUNCTION       DS_DSD_APM_SET_APN_SWITCH_STATUS

DESCRIPTION    to disable/enable apn switch temperarily, this will not write 
               to the NV 
 
DEPENDENCIES   NONE

RETURN VALUE   

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_set_apn_switch_status
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_dsd_apm_apn_switch_cmd_type  *data_ptr = NULL;
  sys_modem_as_id_e_type           subs_id = SYS_MODEM_AS_ID_NONE;
  boolean                          apn_switch_enabled = FALSE;
  attach_pdn_state                 backup_state = ATTACH_PDN_STATE_INVALID;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "Null ptr passed return");
    return;
  }

  data_ptr = (ds_dsd_apm_apn_switch_cmd_type*)cmd_ptr->cmd_payload_ptr;
  subs_id  = data_ptr->subs_id;
  apn_switch_enabled = data_ptr->lte_data_retry;

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  DATA_MSG2(MSG_LEGACY_MED,
            "set apn switch status,apn_switch_enabled on sub %d:%d",
            subs_id, apn_switch_enabled);

  backup_state = subs_info[subs_indx].attach_pdn_info.state;
  ds_dsd_apm_set_apn_switch_flag(subs_id, apn_switch_enabled);
  ds_dsd_apm_refresh_attach_pdn_list(cmd_ptr);
  subs_info[subs_indx].attach_pdn_info.state = backup_state;
  return;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_MATCH_CAUSE_CODES_INTERNAL

DESCRIPTION    This function compares the cause_type & cause code provided by 
               lowers layers with APN Switching cause codes in rule block 
 
PARAMETERS     subs_id: Current active PS subs 
               profile_id: Profile whose rule block entry is needed 
               cause_type: Detach/Reject Network cause
               int_cause: Cause code passed by lower layers
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE: If the Rule block entry for profile contains the cause 
               code passed
               FALSE: If no match for cause code found or Rule block entry
               doesn't exist

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_match_cause_codes_internal
(
  uint16                               profile_id,
  sys_modem_as_id_e_type               subs_id,
  uint16                               int_cause,
  dsd_apm_rule_param_name_enum_type    cause_type
)
{
  ds_dsd_apm_cause_code_info_type  *cause_code_block_ptr = NULL;
  ds_dsd_apm_data_block            *rule_block_ptr = NULL;
  boolean                           ret_val = FALSE;
  ds_dsd_apm_profile_id             subs_profile_id;
  ds3gsubsmgr_subs_id_e_type  subs_indx;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ret_val;
  }
  subs_indx = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);

  /*-----------------------------------------------------------------------
    We extract the APM rule block based on Profile ID and check if a given
    cause code is configured for NETWORK DETACH or REJECT scenario
  -----------------------------------------------------------------------*/
  memset(&subs_profile_id, 0, sizeof(ds_dsd_apm_profile_id));

  subs_profile_id.profile_id = profile_id;
  subs_profile_id.subs_id = subs_id;

  rule_block_ptr = (ds_dsd_apm_data_block *)q_linear_search(&ds_dsd_apm_rules_q[subs_indx],
                                           ds_dsd_apm_compare_profile_id,
                                           &subs_profile_id);

  if(rule_block_ptr != NULL)
  {
    cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_check
           (&(rule_block_ptr->apn_info.cause_code_info.cause_code_info_q));
    while(cause_code_block_ptr != NULL)
    {
      if(cause_code_block_ptr->cause_type == cause_type &&
         cause_code_block_ptr->cause_code == int_cause)
      {
        DATA_MSG1(MSG_LEGACY_MED,
                 "Match found, APN Switching for cause code: %d",int_cause);
        ret_val = TRUE;
        break;
      }
      cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_next
           (&(rule_block_ptr->apn_info.cause_code_info.cause_code_info_q),
            &(cause_code_block_ptr->link));
    }
  }
  return ret_val;
}


/*===========================================================================
FUNCTION       DS_DSD_APM_GET_LAST_CAUSE_BLOCK_PER_TYPE

DESCRIPTION    This utility function traverses the cause code queue maintained 
               per APN block to return a pointer to last block for a particular
               cause type
 
PARAMETERS     apm_rule_block_ptr: Per APN Rule block 
               cause_type: DETACH/REJECT Cause type 
 
DEPENDENCIES   NONE

RETURN VALUE   last_block: Last rule block in the queue for given APN

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_apm_cause_code_info_type *ds_dsd_apm_get_last_cause_block_per_type
(
  ds_dsd_apm_data_block                    *apm_rule_block_ptr,
  uint16                                    cause_type
)
{
  ds_dsd_apm_cause_code_info_type          *cause_code_block_ptr = NULL;
  ds_dsd_apm_cause_code_info_type          *ret_val_block_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(apm_rule_block_ptr != NULL)
  {
    cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_check
      (&(apm_rule_block_ptr->apn_info.cause_code_info.cause_code_info_q));

    while(cause_code_block_ptr != NULL)
    {
      if(cause_code_block_ptr->cause_type == cause_type)
      {
        ret_val_block_ptr = cause_code_block_ptr;
      }

      cause_code_block_ptr = (ds_dsd_apm_cause_code_info_type *)q_next
        (&(apm_rule_block_ptr->apn_info.cause_code_info.cause_code_info_q),
         &(cause_code_block_ptr->link));
    }
  }
  else
  {
    DATA_MSG1(MSG_LEGACY_ERROR, 
              "Invalid Rule block ptr: 0x%x", apm_rule_block_ptr);
  }

  return ret_val_block_ptr;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* FEATURE_DATA_LTE */

