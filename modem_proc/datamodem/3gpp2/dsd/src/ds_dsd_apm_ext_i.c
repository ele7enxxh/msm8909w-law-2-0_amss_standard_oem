/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "ds_dsd_apm.h"
#include "ds_dsd_apm_ext_i.h"
#include "msg.h"
#include "data_msg.h"
#include "ds3gmgr.h" //NOTE check this added for ds3g_get_ps_subs_id()

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION       DS_DSD_APM_REQ_EX

DESCRIPTION    This is the external wrapper function called by external clients 
               to request for the passed sub:
                - ATTACH PROFILE_ID.
                - Mandatory P-CSCF
                - Mandatory IP
                - ATTACH PROFILE AVAILABILITY
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_dsd_apm_req_ex
(
  ds_dsd_apm_req_type        apm_req_event, 
  ds_dsd_apm_req_pay_load   *pay_load,
  sys_modem_as_id_e_type     subs_id
)
{
  ds_dsd_ext_error_code retVal = DS_DSD_APM_UNSEPCIFIED;

#ifdef FEATURE_DATA_LTE
  retVal =  ds_dsd_apm_req_internal(apm_req_event, pay_load, subs_id);
#else
  DATA_MSG0(MSG_LEGACY_MED,
            "APM request cannot be completed since, LTE is not supported");
#endif /* FEATURE_DATA_LTE*/
  return retVal;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_REQ

DESCRIPTION    This is the external wrapper function called by external clients 
               to request:
                - ATTACH PROFILE_ID.
                - Mandatory P-CSCF
                - Mandatory IP
                - ATTACH PROFILE AVAILABILITY
 
               The client supplies the memory to write the content.
 
DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_dsd_apm_req
(
  ds_dsd_apm_req_type        apm_req_event, 
  ds_dsd_apm_req_pay_load  * pay_load
)
{
  sys_modem_as_id_e_type subs_id;

  subs_id = ds3g_get_ps_subs_id();

  return ds_dsd_apm_req_ex(apm_req_event, pay_load, subs_id);
}


/*===========================================================================
FUNCTION       DS_DSD_APM_IND

DESCRIPTION    This is the external wrapper function called by external clients. 
               The clients have to notify the DSD APM module.

DEPENDENCIES   NONE

RETURN VALUE   error codes as defined

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_ext_error_code ds_dsd_apm_ind
(
  ds_dsd_apm_ind_type        apm_ind_event, 
  ds_dsd_apm_ind_pay_load    *pay_load
)
{
  ds_dsd_ext_error_code retVal = DS_DSD_APM_UNSEPCIFIED;
#ifdef FEATURE_DATA_LTE
  retVal = ds_dsd_apm_ind_internal(apm_ind_event, pay_load);
#else
  DATA_MSG0(MSG_LEGACY_HIGH,
            "APM module cannot be notified, LTE is not supported");
#endif /* FEATURE_DATA_LTE */
  return retVal;
}

/*===========================================================================
FUNCTION        DS_DSD_APM_INIT

DESCRIPTION     External Wrapper for DSD APM Module Initialization entry point. 
                This function is called from dsd_init function during
                power up.

DEPENDENCIES    NONE

RETURN VALUE    NONE

SIDE EFFECTS    All the variables are initialized. 
===========================================================================*/
void ds_dsd_apm_init(void)
{
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_init_internal();
#else
  DATA_MSG0(MSG_LEGACY_MED,"Cannot initialize the data structures, "
            "since NO LTE support available");
#endif /* FEATURE_DATA_LTE */
  return;
} /* ds_dsd_apm_init() */

/*===========================================================================
FUNCTION      DS_DSD_APM_INIT_PER_SUBS_ID

DESCRIPTION   External Wrapper for DSD APM Module Initialization entry point.
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  All the variables are initialized. .
===========================================================================*/
void ds_dsd_apm_init_per_subsid
(
  sys_modem_as_id_e_type  subs_id
)
{

#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_internal_init_per_subsid(subs_id);
#else
  DATA_MSG0(MSG_LEGACY_MED,"Cannot initialize the data structures, "
            "since NO LTE support available");
#endif /* FEATURE_DATA_LTE */
  return;
} /* ds_dsd_apm_init_per_subsid() */

/*===========================================================================
FUNCTION       DS_DSD_APM_FORBIDDEN_APN_LIST_IND

DESCRIPTION    Indicate the list of blocked APN name and IP type by 3GPP MH. 
               3GPP MH will call this function whenever blocked APN list
               changed.
               If there are APNs blocked at power up, 3GPP MH will call this
               function before ds_dsd_apm_ind(DS_DSD_APM_ATTACH_INIT_IND).
 
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_forbidden_apn_list_ind
( 
  sys_modem_as_id_e_type       subs_id,
  ds_dsd_apm_block_list_type  *forbidden_apn_list
)
{

#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_forbidden_apn_list_ind_internal (subs_id, forbidden_apn_list);
#else
  DATA_MSG0(MSG_LEGACY_MED,"LTE is not supported");
#endif /* FEATURE_DATA_LTE */
  return;
}

/*===========================================================================
 
FUNCTION DS_DSD_APM_UNSET_IP_RULE_REQUIRED

DESCRIPTION
  This is a utility unset rule required for current attach PDN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_dsd_apm_unset_ip_rule_required
(
  sys_modem_as_id_e_type    subs_id
)
{
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_unset_ip_rule_required_internal (subs_id);
#else
  DATA_MSG0(MSG_LEGACY_HIGH,"Can't unset the IP Rule, LTE is not supported");
#endif /* FEATURE_DATA_LTE */
  return;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_APN_CHANGE_CMD_PROCESS

DESCRIPTION    Processing of DSD APM APN change command

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_change_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_apn_change_cmd_process_internal(cmd_ptr);
#else
  DATA_MSG0(MSG_LEGACY_MED,
            "APN Change command could be processed, LTE is not supported");
#endif /* FEATURE_DATA_LTE */  
  return;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_IS_APN_IN_ATTACH_PDN_LIST

DESCRIPTION    This function checks whether a given APN is an attach APN
 
PARAMETERS     apn_name: APN which has to be checked

DEPENDENCIES   NONE

RETURN VALUE   TRUE: IF APN passed is Attach APN
               FALSE: Otherwise

SIDE EFFECTS   None
===========================================================================*/
boolean ds_dsd_apm_is_apn_in_attach_pdn_list
(
  sys_modem_as_id_e_type  subs_id,
  char                       *apn_name
)
{
  boolean                     ret_val = FALSE;
#ifdef FEATURE_DATA_LTE
  ret_val = ds_dsd_apm_is_apn_in_attach_pdn_list_internal(subs_id, apn_name);
#else
  DATA_MSG0(MSG_LEGACY_HIGH,
            "Cannot locate attach APN, LTE is not supported");
#endif /* FEATURE_DATA_LTE */  
  return ret_val;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_APN_DISABLE_CMD_PROCESS

DESCRIPTION    Processing of DSD APM APN disable command

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds_dsd_apm_apn_disable_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
#ifdef FEATURE_DATA_LTE
  ds_dsd_apm_apn_disable_cmd_process_internal ( cmd_ptr );
#else
  DATA_MSG0(MSG_LEGACY_MED,
            "APN Disable command could be processed, LTE is not supported");
#endif /* FEATURE_DATA_LTE */  
  return;
}

/*===========================================================================

FUNCTION DS_DSD_APM_GET_CURRENT_PLMN_ID

DESCRIPTION
  This function gets the current_plmn_id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_dsd_apm_get_current_plmn_id
(
  sys_modem_as_id_e_type     subs_id,
  sys_plmn_id_s_type        *current_plmn_id
)
{
  boolean retVal = FALSE;
#ifdef FEATURE_DATA_LTE
  retVal = ds_dsd_apm_get_current_plmn_id_internal(subs_id, current_plmn_id);
#else
  DATA_MSG0(MSG_LEGACY_MED,
            "Cannot fetch the current plmn id, LTE is not supported"); 
#endif /* FEATURE_DATA_LTE */
  return retVal;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_ATTACH_COMPLETED

DESCRIPTION    This is the utility function to check if ATTACH COMPLETED 
               from APM modules perspective 
  
DEPENDENCIES   NONE

RETURN VALUE   TRUE/FALSE

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_attach_completed
(
  sys_modem_as_id_e_type    subs_id
)
{
  boolean retVal = FALSE;
#ifdef FEATURE_DATA_LTE
  retVal = ds_dsd_apm_attach_completed_internal(subs_id, FALSE);
#else
  DATA_MSG0(MSG_LEGACY_MED,"Attach cannot be completed, LTE not supported");
#endif /* FEATURE_DATA_LTE */
  return retVal;
}

/*===========================================================================
FUNCTION       DS_DSD_APM_CMD_PROCESS

DESCRIPTION    This function processes APM commands.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.
===========================================================================*/
void ds_dsd_apm_cmd_process
(
  ds_cmd_type  *cmd_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(cmd_ptr == NULL)
  {
    DATA_MSG0(MSG_LEGACY_ERROR, "NULL cmd ptr");
    return;
  }
  DATA_MSG1(MSG_LEGACY_HIGH, "ds_dsd_apm_cmd_process. command ID=%d",
            cmd_ptr->hdr.cmd_id);

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_DSD_APM_APN_CHG:
      ds_dsd_apm_apn_change_cmd_process( cmd_ptr );
      break;

    case DS_CMD_DSD_APM_REFRESH_ATTACH_PDN_LIST:
#ifdef FEATURE_DATA_LTE
      ds_dsd_apm_refresh_attach_pdn_list( cmd_ptr );
#endif /*FEATURE_DATA_LTE*/
      break;

    case DS_CMD_DSD_APM_SET_APN_SWITCH_STATUS:
#ifdef FEATURE_DATA_LTE
      ds_dsd_apm_set_apn_switch_status( cmd_ptr );
#endif /*FEATURE_DATA_LTE*/
      break;

    case DS_CMD_DSD_APM_APN_DISABLE:
      ds_dsd_apm_apn_disable_cmd_process ( cmd_ptr );
      break;

    default:
      DATA_MSG1(MSG_LEGACY_ERROR, "Command %d: not for APM",
                cmd_ptr->hdr.cmd_id);
      break;
  }
  return;
}/* ds_dsd_apm_cmd_process()*/

/*===========================================================================
FUNCTION       DS_DSD_APM_MATCH_CAUSE_CODES

DESCRIPTION    This function compares the cause_type & cause code provided by 
               lowers layers with APN Switching cause codes in rule block 
 
PARAMETERS     subs_id: Current active PS subs 
               profile_id: Profile whose rule block entry is needed 
               cause_type: Detach/Reject Network cause
               int_cause: Cause code passed by lower layers
 
DEPENDENCIES   NONE

RETURN VALUE   TRUE: If the cause returned by lower layers is present in APM 
                     Rule block corresponding to the passed profile ID
               FALSE: If the cause code passed is not found in APM Rule block

SIDE EFFECTS   NONE
===========================================================================*/
boolean ds_dsd_apm_match_cause_codes
(
  uint16                               profile_id,
  sys_modem_as_id_e_type               subs_id,
  uint16                               int_cause,
  dsd_apm_rule_param_name_enum_type    cause_type
)
{
  boolean                              ret_val = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_DATA_LTE
  ret_val = ds_dsd_apm_match_cause_codes_internal(profile_id,
                                                  subs_id,
                                                  int_cause,
                                                  cause_type);
#else
  DATA_MSG0(MSG_LEGACY_MED, "LTE not supported, cant compare cause codes");
#endif /*FEATURE_DATA_LTE*/

  return ret_val;
}

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

