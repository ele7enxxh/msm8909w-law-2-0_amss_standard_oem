#ifndef DS_DSD_APM_H
#define DS_DSD_APM_H
/*===========================================================================

                      DS_DSD_APM.H

DESCRIPTION


EXTERNALIZED FUNCTIONS

 Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_apm.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/12/12    sk    Created Module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "comdef.h"
#include "queue.h"

#include "ds_dsd_apm_ext_i.h"
#include "ps_iface_defs.h"
#ifdef FEATURE_DATA_WLAN_MAPCON
#include "ds_mppm_ext_i.h"
#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/
#ifdef FEATURE_DATA_LTE

#define ATTACH_APN_MAX 2
#define DSD_APM_MAX_APN_STRING_LEN    DS_DSD_APM_MAX_APN_STRING_LEN
#define DSD_APM_INVALID_PROF_ID       0

typedef enum
{
  DS_DSD_APM_RULE_INVALID           = 0,
  DSD_DSD_APM_RULE_IP_ADDRESS       = 1,
  DS_DSD_APM_RULE_PCSCF_ADDRESS     = 2,
  DS_DSD_APM_RULE_OOS               = 3,
  DS_DSD_APM_RULE_PLMN              = 4,
  DS_DSD_APM_RULE_SYS_CHANGE        = 5,
  DS_DSD_APM_RULE_ATTACH_SUCCESS    = 6,
  DS_DSD_APM_RULE_ATTACH_FAIL       = 7,
  DS_DSD_APM_RULE_ATTACH_BLOCKED    = 8,
  DS_DSD_APM_RULE_DETACH_NORMAL     = 9,
  DS_DSD_APM_RULE_RESET_APN         = 10,
  DS_DSD_APM_RULE_DNS_ADDRESS       = 11,
  DS_DSD_APM_RULE_PCSCF_DNS_ADDRESS = 12,
  DS_DSD_APM_RULE_MAX = DS_DSD_APM_RULE_PCSCF_DNS_ADDRESS
}ds_dsd_apm_apn_rule;

/* 
According to Simi 
  DSAT_APN_CLASS_ONE     =  0
  DSAT_APN_CLASS_TWO    =  1 
  DSAT_APN_CLASS_THREE  = 2
  DSAT_APN_CLASS_FOUR   = 3
  DSAT_APN_CLASS_FIVE     = 4

at$qcpdpcfge=<profile id>,<flag>,<timer>,<apn class>,[<apn_bearer>]
*/
typedef enum
{
  ATTACH_PDN_INVALID     = 0,
  ATTACH_PDN_MIN         = 1,
  ATTACH_PDN_PRIMARY     = ATTACH_PDN_MIN,
  ATTACH_PDN_NEXT        = ATTACH_PDN_MIN+2,
  ATTACH_PDN_MAX         = 0xFF,
  ATTACH_PDN_DEFAULT     = ATTACH_PDN_MAX
}attach_pdn_type;

/*------------------------------------------------------------------------
Data structure to maintain APN information for config rule and acFtion rule
------------------------------------------------------------------------*/
/*----------------------------------------
      Config Flag
-----------------------------------------*/
#define CONFIG_IP_ADDRESS         0x0001
#define CONFIG_PCSCF              0x0002
#define CONFIG_OOS                0x0004
#define CONFIG_PLMN               0x0008
#define CONFIG_SYS_CHG            0x0010
#define CONFIG_ATTACH_SUCCESS     0x0020
#define CONFIG_ATTACH_FAIL        0x0040
#define CONFIG_DETACH_NORMAL      0x0080
#define CONFIG_DNS_ADDRESS        0x0100
#define CONFIG_PCSCF_DNS_ADDRESS  0x0200

/*----------------------------------------
      Action Flag
-----------------------------------------*/
#define ACTION_BLOCK_PLMN         0x0001
#define ACTION_INIT_DETACH        0x0002
#define ACTION_CHANGE_TO_PRIMARY  0x0004
#define ACTION_CHANGE_TO_NEXT     0x0008

/*------------------------------------------------------------
      Enum used to check whether single APM Rule is presemt
-------------------------------------------------------------*/
#define DS_DSD_APM_SINGLE_QUEUE_ENTRY    1

typedef struct
{
  sys_modem_as_id_e_type     subs_id;
  attach_pdn_type            apn_type;
  uint8                      apn_name_len;
  char                       apn_name[DSD_APM_MAX_APN_STRING_LEN];    /*  char e.g. qcomIMS.com, power up*/
  uint16                     profile_id;       /*  This value is read  power up   */

  /*----------------------------------------
  ATTACH PDN CONFIG and ACTION RULE
  -----------------------------------------*/
  struct
  {
    boolean                  attach_rule_valid;
    //Configuration Rule
    struct
    {
      uint16                 config_valid_flag;
      ipRequiredType         ip_address;
      ipRequiredType         pcscf_address;
      ipRequiredType         dns_address;
      ipRequiredType         pcscf_or_dns_address;
    }config_rule;

    //Action Rule
    struct
    {
      uint16                 ip_address;
      uint16                 pcscf_address;
      uint16                 out_of_service;
      uint16                 plmn_change;
      uint16                 sys_change;
      uint16                 attach_success;
      uint16                 attach_fail;
      uint16                 detach_normal;
      uint16                 attach_blocked;
      uint16                 reset_attach_apn;
      uint16                 dns_address;
      uint16                 pcscf_or_dns_address;
    }action_rule;
  }AttachPdn;

  /*----------------------------------------
  ONDEMAND PDN CONFIG and ACTION RULE
  -----------------------------------------*/
  struct
  {
    boolean                  on_demand_rule_valid;
    struct
    {
      uint16                 config_valid_flag;
      ipRequiredType         ip_address;
      ipRequiredType         pcscf_address;
      ipRequiredType         dns_address;
      ipRequiredType         pcscf_or_dns_address;
    }config_rule;
  }OnDemandPdn;

  /*-------------------------------------------
   Cause Code struct to store Cause code info
  --------------------------------------------*/
  struct
  {
    boolean                  cause_code_rule_valid;
    uint16                   config_valid_flag;
    q_type                   cause_code_info_q;
  }cause_code_info;

}apn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain currently attaching pdn information. The 
data structures are used to run the config and action rule. 
------------------------------------------------------------------------*/

/*----------------------------------------
ATTACH PDN STATE (in DSDs view)
-----------------------------------------*/
typedef enum
{
  ATTACH_PDN_STATE_INVALID        = 0,
  ATTACH_PDN_STATE_DISCONNECTED   = 1,
  ATTACH_PDN_STATE_CONNECTING     = 2,
  ATTACH_PDN_STATE_CONNECTED      = 3,
  ATTACH_PDN_STATE_DISCONNECTING  = 4
}attach_pdn_state;

/*----------------------------------------
Network state of a message.
-----------------------------------------*/
typedef enum
{
  DS_DSD_APM_NOT_RCVD       = 0,
  DS_DSD_APM_RCVD_SUCC      = 1,
  DS_DSD_APM_RCVD_FAIL      = 2
}ds_dsd_apm_nw_state_type;

/*-----------------------------------------------
PCSCF or DNS address state. 
 
 0 - Either PCSCF or DNS address is not received
 1 - Both PCSCF & DNS address are invalid
 2 - Either PCSCF or DNS address is valid
------------------------------------------------*/
typedef enum
{
  DS_DSD_APM_PCSCF_DNS_NOT_RCVD = 0,
  DS_DSD_APM_PCSCF_DNS_INVALID  = 1,
  DS_DSD_APM_PCSCF_DNS_VALID    = 2
}ds_dsd_apm_pcscf_dns_addr_state_type;

/*------------------------------------------------------
ATTACH PDN related data structes. This object is 
re-initialized every-time the the ATTACH state goes 
to ATTACH_PDN_CONNECTING.  
------------------------------------------------------*/
typedef struct
{
  attach_pdn_type                   pdn_type; 
  attach_pdn_state                  state; 
  boolean                           ip_rule_required;
  ds_dsd_apm_nw_state_type          ipv4_address;
  ds_dsd_apm_nw_state_type          ipv6_address;
  ds_dsd_apm_nw_state_type          ipv4_pcscf_address;
  ds_dsd_apm_nw_state_type          ipv6_pcscf_address;
  ds_dsd_apm_nw_state_type          ipv4_dns_address;
  ds_dsd_apm_nw_state_type          ipv6_dns_address;
  uint16                            attach_profile_id;
  boolean                           if_lte_attach_allowed;
  boolean                           is_tlb_mode_active;
  ds_profile_3gpp_pdp_type_enum_type  attach_pdp_type;
}attach_pdn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain next attach pdn information
------------------------------------------------------------------------*/
typedef struct
{
  attach_pdn_type               next_attach_pdn_type;
  boolean                       permanent; 
}next_attach_pdn_info_type;

/*------------------------------------------------------------------------
Data structure to maintain attach pdn information provided through EFS. 
 
TODO: this will be obsolete once EFS reading becomes robust. That will 
provide the number of items and profiles in order or user preference. 
------------------------------------------------------------------------*/
typedef struct
{
  boolean   valid;
  uint16    profile_id;
}efs_profile_input;

typedef struct  
{
  q_link_type                   link; 
  apn_info_type                 apn_info;
}ds_dsd_apm_data_block;

/*------------------------------------------------------------------------
Define a struct for attach_profile_q entry
------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type         subs_id;
  uint16                         profile;
  boolean                        apn_disabled;
}apm_attach_profile;

typedef struct
{
  q_link_type                     link;
  apm_attach_profile              attach_profile;
}attach_profile_data_block;

/*------------------------------------------------------------------------
Define a struct for cause code Queue block entry
------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                         link;
  uint16                              cause_code;
  dsd_apm_rule_param_name_enum_type   cause_type;
}ds_dsd_apm_cause_code_info_type;

/*------------------------------------------------------------------------
Define a struct for ds_dsd_apn_priority_q entry
------------------------------------------------------------------------*/
typedef enum
{
  PDN_TYPE_INVALID          = 0,
  ATTACH_PDN_TYPE           = 1,
  ON_DEMAND_PDN_TYPE        = 2
}apm_pdn_type;

#define V4_PDN_TYPE        0x01 
#define V6_PDN_TYPE        0x02
#define V4_V6_PDN_TYPE     0x04

#define V4_PROF_PREFER     0x01
#define V6_PROF_PREFER     0x02
#define V4V6_PROF_PREFER   0x04

/*------------------------------------------------------------------------
  Data structure to store APN block/unblock info. 
  ip_type_requested:
            0x01 - V4 only (V4_PDN_TYPE)
            0x02 - V6 only (V6_PDN_TYPE)
            0x04 - V4V6 dual IP only (V4_V6_PDN_TYPE)
            0x05 - V4 or V4V6, V4 prefer (V4_V6_PDN_TYPE & V4_PDN_TYPE)
            0x06 - V6 or V4V6, V6 prefer (V4_V6_PDN_TYPE & V6_PDN_TYPE)
            0x07 - V4 or V6
 
  ip_type_blocked:
            0x01 - V4 block
            0x02 - V6 block
            0x04 - V4V6 block
 
  ip_type_preferred:
            0x01 - v4 prefer
            0x02 - v6 prefer
            0x04 - v4v6 prefer
------------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type   subs_id;
  uint16                   ip_type_blocked; 
  uint16                   ip_type_requested; 
  apm_pdn_type             apn_type;
  uint16                   apn_len;
  char                     apn_name[DSD_APM_MAX_APN_STRING_LEN];
}apm_apn_block_info_type;

typedef struct
{
  q_link_type                    link;
  apm_apn_block_info_type        apn_block_info;
}apn_priority_data_block;

/*----------------------------------------
      Configure Error Flag
-----------------------------------------*/
#define EFS_APM_RULE_READ_ERR                    0x0001
#define EFS_ATTACH_PROFILE_READ_ERR              0x0002
#define EFS_PROFILE_RULE_MISMATCH                0x0004
#define EFS_ATTACH_PROFILE_NUM_ERR               0x0008
#define EFS_APM_RULE_APN_CLASS_ERR               0x0010
#define EFS_DEF_ATTACH_PROF_READ_ERR             0x0020
#define EFS_APM_APN_ACTION_RULE_ERR              0x0040
#define EFS_APM_RULE_ATTACH_PROF_NUM_ERR         0x0080

#ifdef FEATURE_DATA_WLAN_MAPCON
/*------------------------------------------------------------------------
Define a struct for apn_attach_capable_q
------------------------------------------------------------------------*/

/*------------------------------------------------------
APN attach allowed/not allowed info
------------------------------------------------------*/
typedef struct
{
  boolean         is_lte_attach_allowed;
  uint8           apn_name_len;
  char            apn_name[DSD_APM_MAX_APN_STRING_LEN];
}apn_attach_capable_info_type;

typedef struct  
{
  q_link_type                             link;
  sys_modem_as_id_e_type                  subs_id; 
  apn_attach_capable_info_type            apn_attach_capable_info;
}apn_attach_capable_block;

/*------------------------------------------------------
MPPM event info type, used for post cmd to DS task
------------------------------------------------------*/
typedef struct
{
  uint8                             num_pdns_config;  
  uint8                             fallback_apn_idx;
  ds_mppm_per_pdn_rat_info_s_type   rat_aggr_info[DS_MAX_PDN_COUNT];
}ds_apm_mppm_event_info_s_type;

#endif /*FEATURE_DATA_WLAN_MAPCON*/

/*------------------------------------------------------------------------
Define a struct for info required per sub
------------------------------------------------------------------------*/
typedef struct
{
  attach_pdn_info_type          attach_pdn_info;
  next_attach_pdn_info_type     next_attach_pdn_info;
  sys_plmn_id_s_type            apm_lte_plmn_id;
  boolean                       dsd_apm_apn_switching_enable;
  boolean                       dsd_apm_generic_switching_status;
  boolean                       dsd_apm_lte_apn_blocking_support;
  boolean                       dsd_apm_profile_id_based_switch;
  boolean                       dsd_apm_default_apm_rule_enabled;
  boolean                       default_attach_apn_allowed;
  boolean                       apm_attach_apn_all_blocked;
  uint32                        ds_dsd_apm_plmn_blocking_interval;
}ds_dsd_apm_subs_info_type;

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
);

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
);

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
);

/*===========================================================================
 
FUNCTION DS_DSD_APM_UNSET_IP_RULE_REQUIRED_INTERNAL

DESCRIPTION
  This is a utility unset rule required for current attach PDN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_dsd_apm_unset_ip_rule_required_internal
(
  sys_modem_as_id_e_type    subs_id
);

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
);

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
  ds_dsd_apm_ind_pay_load  * pay_load
);

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
void ds_dsd_apm_init_internal(void);

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
);

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
  sys_modem_as_id_e_type      subs_id,
  ds_dsd_apm_block_list_type  *forbidden_apn_list
);

#ifdef FEATURE_DATA_WLAN_MAPCON
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
  sys_modem_as_id_e_type    subs_id,
  char                      *apn_string,
  uint8                     apn_len
);

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
  ds_mppm_event_type         mppm_event,
  ds_mppm_event_info_s_type  *mppm_event_info_ptr,
  void                       *data_ptr
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/
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
);

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
);

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
  sys_modem_as_id_e_type  subs_id,
  char                       *apn_name
);

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
);

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
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_DATA_LTE */ 

#endif /* DS_DSD_APM_H */
