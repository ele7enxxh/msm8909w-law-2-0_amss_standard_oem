#include "datamodem_variation.h"
#include "customer.h" 
#include "comdef.h"
#include "nv.h"
#include "ds3gcfgmgr.h"
#include "ds3gmgr.h"
#include "ds_eps_throughput_hdlr.h"
#include "ds3gmmgsdiif.h"

#ifdef FEATURE_DATA_3GPP
#include "ds_3gpp_nv_manager.h"
#endif
#ifdef FEATURE_DATA_LTE
#include "lte.h"
#endif

ds_3gpp_nv_per_subs_info_type*  ds_3gpp_nv_per_subs_info_p[DS3GSUBSMGR_SUBS_ID_MAX];

#ifdef FEATURE_DATA_LTE
static   lte_3gpp_release_ver_e      ds_3gpp_nv_lte_rel_version;    

/*----------------------------------------------------------------------------
  NV item to indicate whether DS will notify LTE RRC of TRM priority
-----------------------------------------------------------------------------*/
static boolean ds_eps_enable_send_trm_priority = FALSE; 
#endif

static ue_nas_rel_compliance_type ds_3gpp_nv_umts_rel_version;
static boolean             ds_3gpp_nv_l_to_w_qos_continuity = FALSE;
static boolean             ds_3gpp_nv_bcm_validation_on_bearer_mod = TRUE;
static uint8               ds_3gpp_nv_umts_nw_init_qos_support = 0;
static boolean             ds_3gpp_nv_umts_ue_init_qos_support = FALSE;
/*------------------------------------------------------------------------------
 store 3GPP release version and re-use it instead of calling read everytime 
------------------------------------------------------------------------------*/
static uint8 ds_3gpp_3gpp_rel_version = 0;

/*----------------------------------------------------------------------------
  NV item to indicate whether Multi PDN DUN is enabled or not
-----------------------------------------------------------------------------*/
static boolean dsumts_rmsmi_multi_pdn_dun_is_enabled = TRUE;

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_INIT

DESCRIPTION
  This function initializes the nv manager module. This function is called
  during boot up. This function is responsible for intializing the nv info
  for primary subscription.
 
	PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_nv_manager_init()
{
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id();
  ds_3gpp_nv_manager_init_per_device_nv();
  ds_3gpp_nv_manager_init_per_subs_id(subs_id);
}
/*===========================================================================
FUNCTION ds_3gpp_nv_manager_init_per_device_nv

DESCRIPTION
  This function is called during boot up. This function is responsible for 
  intializing the nv info for device specific nv
 
	PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_nv_manager_init_per_device_nv()
{

#ifdef FEATURE_DATA_LTE
  ds_3gpp_nv_manager_read_lte_rel_ver_from_nv();
  ds_3gpp_nv_manager_read_enable_send_trm_priority_nv();
#endif

  ds_3gpp_nv_manager_read_nas_umts_rel_version();
  ds_3gpp_nv_manager_read_l_to_w_qos_continuity();
  ds_3gpp_nv_manager_read_bcm_validation();
  ds_3gpp_nv_manager_read_umts_nw_init_qos_support_nv();
  ds_3gpp_nv_manager_read_umts_ue_init_qos_support_nv();
  ds_3gpp_nv_manager_read_multi_pdn_dun_nv();
  ds_3gpp_nv_manager_read_efs_3gpp_rel_version();
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_INIT_PER_SUBS_ID

DESCRIPTION
  This function is responsible for intializing the nv info for each
	subscription.
 
  PARAMETERS
  subs_id - subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_nv_manager_init_per_subs_id
(
  sys_modem_as_id_e_type subs_id
)
{
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return;  
  }

  if(!ds_3gpp_nv_manager_validate_nv_subs_info(ds_3gpp_nv_per_subs_info_p[subs_id]))
  {
    ds_3gpp_nv_per_subs_info_p[subs_id] = (ds_3gpp_nv_per_subs_info_type *)
      modem_mem_alloc(sizeof(ds_3gpp_nv_per_subs_info_type), MODEM_MEM_CLIENT_DATA);
    ds_3gpp_nv_per_subs_info_p[subs_id]->cookie = NV_MANAGER_COOKIE;
    ds_3gpp_nv_per_subs_info_p[subs_id]->subs_id = subs_id;
    ds_3gpp_nv_per_subs_info_p[subs_id]->efsnas_config = NULL;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Nv per subs info handle is already allocated for subs id"
                       ,subs_id);
    return;
  }
 
  ds_3gpp_nv_manager_read_apm_apn_switching(subs_id); 
  ds_3gpp_nv_manager_read_auth_fallback(subs_id);
  ds_3gpp_nv_manager_read_max_rab_reestab_req_retry_cnt(subs_id);
  ds_3gpp_nv_manager_read_back_pressure_params(subs_id);
  ds_3gpp_nv_manager_read_tcp_threshold_ack_cnt(subs_id);
  ds_3gpp_nv_manager_read_um_wm_threshold(subs_id);
  ds_3gpp_nv_manager_read_mtu_from_nv(subs_id);
  ds_3gpp_nv_manager_read_redial_no_cause_code_from_nv(subs_id);
  ds_3gpp_nv_manager_read_rel_10_throttling_per_plmn_from_nv(subs_id);
  ds_3gpp_nv_manager_read_regular_pdn_throttling_from_nv(subs_id);
  ds_3gpp_nv_manager_read_multi_pdn_same_apn_nv(subs_id);
#ifdef FEATURE_DATA_LTE
  ds_3gpp_nv_manager_read_always_connect_def_pdn_flag(subs_id);
  ds_3gpp_nv_manager_read_remove_unused_pdn(subs_id);
  ds_3gpp_nv_manager_read_allow_default_pdn_teardown(subs_id);
  ds_3gpp_nv_manager_read_allow_infinite_throttling(subs_id);
  ds_3gpp_nv_manager_read_switch_apn_on_reject(subs_id);
  ds_3gpp_nv_manager_read_null_apn_support_for_non_attach_pdn_from_nv(subs_id);
  ds_3gpp_nv_manager_read_dl_tput_t_accumulate_from_nv(subs_id);
  ds_3gpp_nv_manager_read_lte_ca_display_threshold(subs_id);
#endif /*FEATURE_DATA_LTE */
  ds_3gpp_nv_manager_read_ps_data_avail_on_power_save(subs_id);
  ds_3gpp_nv_manager_read_global_throttling_nv(subs_id);
  ds_3gpp_nv_manager_read_ser_req_thor_sm_nv(subs_id);
  ds_3gpp_nv_manager_read_nbns_enabled(subs_id);
  ds_3gpp_nv_manager_read_disable_apn_matching_from_nv(subs_id);
  ds_3gpp_nv_manager_read_apn_param_chg_cfg_from_nv(subs_id);
  ds_3gpp_nv_manager_read_check_ehplmn_list_from_nv(subs_id);
  ds_3gpp_nv_manager_read_mnc_len_from_nv(subs_id);
  ds_3gpp_nv_manager_read_enable_pdn_throt_per_rat_from_nv(subs_id);
  ds_3gpp_nv_manager_read_ignore_ui_data_throttling_from_nv(subs_id);
  ds_3gpp_nv_manager_read_efnas_config(subs_id);
}

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_VALIDATE_NV_SUBS_INFO

DESCRIPTION
  This function is used to reset the nv info structure per subscription
 
  PARAMETERS
  subs_id    subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_reset_per_subs_id
(
  sys_modem_as_id_e_type subs_id
)
{
  if(ds_3gpp_nv_manager_validate_nv_subs_info(ds_3gpp_nv_per_subs_info_p[subs_id]))
  {
    ds_3gpp_nv_per_subs_info_p[subs_id]->cookie = 0;

    if (ds_3gpp_nv_per_subs_info_p[subs_id]->efsnas_config)
    {
      modem_mem_free(ds_3gpp_nv_per_subs_info_p[subs_id]->efsnas_config,
                     MODEM_MEM_CLIENT_DATA);
    }
    ds_3gpp_nv_per_subs_info_p[subs_id]->efsnas_config = NULL;

    modem_mem_free(ds_3gpp_nv_per_subs_info_p[subs_id],MODEM_MEM_CLIENT_DATA);
    ds_3gpp_nv_per_subs_info_p[subs_id] = NULL;
  }
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_VALIDATE_NV_SUBS_INFO

DESCRIPTION
  This function is used to validate the nv info pointer.
 
  PARAMETERS
  nv_validation_info- nv validation info pointer

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_nv_manager_validate_nv_subs_info
(
   ds_3gpp_nv_per_subs_info_type* nv_validation_info
)
{
  boolean valid_ret_val= FALSE;
  if (nv_validation_info != NULL && 
      nv_validation_info->cookie == NV_MANAGER_COOKIE)
  {
    valid_ret_val= TRUE;
  }
  else
  {
    DS_3GPP_MSG1_ERROR("NV validation info:0x%x is INVALID",
                       nv_validation_info);
  }

  return valid_ret_val;

}


/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_GET_NV_SUBS_INFO

DESCRIPTION
  This function is used to return the nv validation info pointer.for the
  subscription passed
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_nv_per_subs_info_type - nv validation info pointer

SIDE EFFECTS
  None.

===========================================================================*/
ds_3gpp_nv_per_subs_info_type* ds_3gpp_nv_manager_get_nv_subs_info
(
  sys_modem_as_id_e_type subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_info_handle = NULL;
  /*----------------------------------------------------------*/

  if (ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    nv_info_handle = ds_3gpp_nv_per_subs_info_p[subs_id];
  }

  return nv_info_handle;
}


/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_MAX_RAB_REESTAB_REQ_RETRY_CNT

DESCRIPTION
  This function is used to read mx rab reestablishment count nv item
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_max_rab_reestab_req_retry_cnt
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;

  }

  nv_per_subs_info->max_rab_reestab_req_retry_cnt = RAB_REESTAB_RETRY_CNT_INVALID;
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_MAX_RAB_REESTAB_COUNT, 
                           (uint8 *)&nv_per_subs_info->max_rab_reestab_req_retry_cnt,
                            sizeof(uint32),
                            subs_id) > 0 )
  {
    DS_3GPP_MSG1_HIGH("NV item for max_rab_reestab_req_retry_cnt was set: %d",
                      nv_per_subs_info->max_rab_reestab_req_retry_cnt);
  }
  else
  {
  DS_3GPP_MSG0_HIGH("NV item for max_rab_reestab_req_retry_cnt was not set"
                     "Reverting to default retries");
    nv_per_subs_info->max_rab_reestab_req_retry_cnt = RAB_REESTAB_RETRY_CNT_DEFAULT;
  }

}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_BACK_PRESSURE_PARAMS

DESCRIPTION
  This function is used to read back pressure parameters nv item
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_back_pressure_params
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  int   status = 0;
  ds_bearer_cntxt_bp_removal_struct_type ds_bearer_cntxt_bp_removal_params;


  memset(&ds_bearer_cntxt_bp_removal_params, 0,
    sizeof(ds_bearer_cntxt_bp_removal_params)); 



  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info ))
  {
    return;

  }
  

  status = ds_3gpp_cfg_get_subsid( DS_3GPP_TPUT_OPT_PARAMS, 
                      (uint8 *)&(ds_bearer_cntxt_bp_removal_params), 
                       sizeof(ds_bearer_cntxt_bp_removal_struct_type),
                       subs_id);
  
  if (status <  0)
  {
    DS_3GPP_MSG0_ERROR("Error opening file for bp_removal");
    nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.timer_val = 
                                      DS3GPP_THROUGHPUT_TMR_DEFAULT_VAL;
    nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.back_pressure_removal = TRUE;
    nv_per_subs_info->is_tput_opt_params_present_in_nv = FALSE;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("bp_removal file read success");
    nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.back_pressure_removal =
                   ds_bearer_cntxt_bp_removal_params.back_pressure_removal;
    if (0 == ds_bearer_cntxt_bp_removal_params.timer_val)
    {
      DS_3GPP_MSG0_HIGH("timer_val is 0 setting default value: 2secs");
      nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.timer_val = 
                                      DS3GPP_THROUGHPUT_TMR_DEFAULT_VAL;
    }
    else
    {
      nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.timer_val = 
                              ds_bearer_cntxt_bp_removal_params.timer_val;
    }
    nv_per_subs_info->is_tput_opt_params_present_in_nv = TRUE;
  }
   
}


/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_PS_DATA_AVAIL_ON_POWER_SAVE

DESCRIPTION
  This function is used to read ps data available on power save nv item
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_ps_data_avail_on_power_save
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info ))
  {
    return;

  }

  nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save = FALSE;

 /* Read NV item ds_3gpp_send_ps_data_avail_on_power_save */
  if (ds_3gpp_cfg_get_subsid( DS_3GPP_SEND_PS_DATA_AVAIL_ON_POWER_SAVE, 
                      (uint8 *)&(nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save), 
                       sizeof(nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save),
                       subs_id) > 0) 
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_send_ps_data_avail_on_power_save set to %d in EFS", 
                      nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save);
  }
  else
  {
    nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save = TRUE;
    DS_3GPP_MSG1_HIGH("ds_3gpp_send_ps_data_avail_on_power_save not set in EFS "
                      "or can't read from EFS, default value %d",
                      nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save);
  }
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_SER_REQ_THOR_SM_NV

DESCRIPTION
  This function is used to read service request throttle info nv item
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  ds_3gpp_nv_manager_read_ser_req_thor_sm_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  int                    cfg_get_retval; /* result of ds_3gpp_cfg_get()*/
  uint32                 ser_req_throt_sm_nv;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info ))
  {
    return;

  }
	
  cfg_get_retval = ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_SRV_REQ_THROTTLE_BEHAVIOR,
                        (unsigned char *) &ser_req_throt_sm_nv,
                        sizeof(uint32),
                        subs_id);

  if(cfg_get_retval == -1 ) 
  {
    ser_req_throt_sm_nv = 0;
    DS_3GPP_MSG1_ERROR("ds_3gppi_throt_sm_core_sm_init: Cannot read EFS Config"
                       "item DS_3GPP_CFG_SRV_REQ_THROTTLE_BEHAVIOR. SM configured as %d", 
                        ser_req_throt_sm_nv);
  }
  nv_per_subs_info->ser_req_throt_sm_nv  = ser_req_throt_sm_nv; 

}

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_GET_SER_REQ_THOR_SM_NV

DESCRIPTION
  This function is used to retrieve service request throttle info per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint32. - Service request throttling info

SIDE EFFECTS
  None.

===========================================================================*/
uint32  ds_3gpp_nv_manager_get_ser_req_thor_sm_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  uint32 ser_req_throt_sm = 0;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }

  ser_req_throt_sm = nv_per_subs_info->ser_req_throt_sm_nv;
  return ser_req_throt_sm;

}



/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_GET_PS_DATA_AVAIL_ON_POWER_SAVE

DESCRIPTION
  This function is used to retrieve ps data available on power save per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_nv_manager_get_ps_data_avail_on_power_save
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info->ds_3gpp_send_ps_data_avail_on_power_save;
  return ret_val;
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_TCP_THRESHOLD_ACK_CNT

DESCRIPTION
  This function is used to read tcp threshold acknowledgement count per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_tcp_threshold_ack_cnt
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  uint16 threshold_ack_cnt = TCP_THRESHOLD_ACK_CNT_DEF;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info ))
  {
    return;

  }
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_TCP_THRESHOLD_ACK_CNT, 
                  (uint8 *)&threshold_ack_cnt,
                   sizeof(uint16),
                   subs_id) > 0 )
  {
    DS_3GPP_MSG1_HIGH("Threshold ack_cnt is set in NV: %d",
                       threshold_ack_cnt);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Threshold ack_cnt is not set in NV default: %d",
                       threshold_ack_cnt);
  }
  if ( threshold_ack_cnt != 0 )
  {
    nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.tcp_threshold_ack_cnt = threshold_ack_cnt;
  }
  else
  {
    nv_per_subs_info->ds_bearer_cntxt_tput_opt_params.tcp_threshold_ack_cnt = TCP_THRESHOLD_ACK_CNT_DEF;
  }
}

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGGER_READ_UM_WM_THRESHOLD

DESCRIPTION
  This function is used to read um watermark threshold information per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_um_wm_threshold
(
  sys_modem_as_id_e_type    subs_id
)
{
  int  retval;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  ds_bearer_cntxt_um_wm_threshold_struct_type  um_wm_threshold;
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type  tds_um_wm_threshold;
  
  
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info ))
  {
    return;

  }


  memset(&um_wm_threshold, 0, sizeof(um_wm_threshold));
  memset(&tds_um_wm_threshold, 0, sizeof(tds_um_wm_threshold));

  /*-----------------------------------------------------------------------
     Initialize the um wm struct with default WM values
   -----------------------------------------------------------------------*/

  /* GPRS */
#ifdef FEATURE_GSM_GPRS
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_dne = DSGPSD_UL_WM_DEFAULT_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_hi  = DSGPSD_UL_WM_DEFAULT_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_lo  = DSGPSD_UL_WM_DEFAULT_LO;

  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_dne = DSGPSD_DL_WM_DEFAULT_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_hi  = DSGPSD_DL_WM_DEFAULT_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_lo  = DSGPSD_DL_WM_DEFAULT_LO;
#endif /*FEATURE_GSM_GPRS*/

  /* WCDMA */
#ifdef FEATURE_WCDMA
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_dne = DSWPSD_UL_WM_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_hi  = DSWPSD_UL_WM_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_lo  = DSWPSD_UL_WM_LO;

  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_dne = DSWPSD_DL_WM_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_hi  = DSWPSD_DL_WM_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_lo  = DSWPSD_DL_WM_LO;
#endif /* FEATURE_WCDMA */

  /* LTE */
#ifdef FEATURE_DATA_LTE
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_dne = DSLTEPS_UL_WM_DEFAULT_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_hi  = DSLTEPS_UL_WM_DEFAULT_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_lo  = DSLTEPS_UL_WM_DEFAULT_LO;

  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_dne = DSLTEPS_DL_WM_DEFAULT_DNE;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_hi  = DSLTEPS_DL_WM_DEFAULT_HI;
  nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_lo  = DSLTEPS_DL_WM_DEFAULT_LO;
#endif /* FEATURE_DATA_LTE */

  /* TDS */
#ifdef FEATURE_TDSCDMA
  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_dne = DSTPSD_UL_WM_DNE;
  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_hi  = DSTPSD_UL_WM_HI;
  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_lo  = DSTPSD_UL_WM_LO;

  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_dne = DSTPSD_DL_WM_DNE;
  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_hi  = DSTPSD_DL_WM_HI;
  nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_lo  = DSTPSD_DL_WM_LO;
#endif /* FEATURE_TDSCDMA */

  /*----------------------------------------------------------------------
     Read the NV item
   -----------------------------------------------------------------------*/
  retval = ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_UM_WM_THRESHOLD,
                           (unsigned char *)(&um_wm_threshold),
                           sizeof(um_wm_threshold),
                           subs_id);
  if (retval == -1)
  {
    DS_3GPP_MSG0_HIGH("TDS WM NV read failed. Using the default values");
  }
  else
  {
    DS_3GPP_MSG0_HIGH("NV read Success");

    if (TRUE == um_wm_threshold.gprs_wm_levels_valid)
    {

      if ((um_wm_threshold.gprs_ul_dne > um_wm_threshold.gprs_ul_hi) &&
          (um_wm_threshold.gprs_ul_hi > um_wm_threshold.gprs_ul_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_dne 
                                           = um_wm_threshold.gprs_ul_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_hi 
                                           = um_wm_threshold.gprs_ul_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_ul_lo 
                                           = um_wm_threshold.gprs_ul_lo;
      }
      if ((um_wm_threshold.gprs_dl_dne > um_wm_threshold.gprs_dl_hi) &&
          (um_wm_threshold.gprs_dl_hi > um_wm_threshold.gprs_dl_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_dne 
                                           = um_wm_threshold.gprs_dl_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_hi 
                                           = um_wm_threshold.gprs_dl_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.gprs_dl_lo 
                                           = um_wm_threshold.gprs_dl_lo;
      }
      nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.
        gprs_wm_levels_valid = TRUE;
    }

    if (TRUE == um_wm_threshold.wcdma_wm_levels_valid)
    {
      if ((um_wm_threshold.wcdma_ul_dne > um_wm_threshold.wcdma_ul_hi) &&
          (um_wm_threshold.wcdma_ul_hi > um_wm_threshold.wcdma_ul_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_dne 
                                           = um_wm_threshold.wcdma_ul_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_hi 
                                           = um_wm_threshold.wcdma_ul_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_ul_lo 
                                           = um_wm_threshold.wcdma_ul_lo;
      }
      if ((um_wm_threshold.wcdma_dl_dne > um_wm_threshold.wcdma_dl_hi) &&
          (um_wm_threshold.wcdma_dl_hi > um_wm_threshold.wcdma_dl_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_dne 
                                           = um_wm_threshold.wcdma_dl_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_hi 
                                           = um_wm_threshold.wcdma_dl_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.wcdma_dl_lo 
                                           = um_wm_threshold.wcdma_dl_lo;
      }
      nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.
        wcdma_wm_levels_valid = TRUE;
    }

    if (TRUE == um_wm_threshold.lte_wm_levels_valid)
    {
      if ((um_wm_threshold.lte_ul_dne > um_wm_threshold.lte_ul_hi) &&
          (um_wm_threshold.lte_ul_hi > um_wm_threshold.lte_ul_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_dne 
                                           = um_wm_threshold.lte_ul_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_hi 
                                           = um_wm_threshold.lte_ul_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_ul_lo 
                                           = um_wm_threshold.lte_ul_lo;
      }
      if ((um_wm_threshold.lte_dl_dne > um_wm_threshold.lte_dl_hi) &&
          (um_wm_threshold.lte_dl_hi > um_wm_threshold.lte_dl_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_dne 
                                           = um_wm_threshold.lte_dl_dne;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_hi 
                                           = um_wm_threshold.lte_dl_hi;
        nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.lte_dl_lo 
                                           = um_wm_threshold.lte_dl_lo;
      }
      nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold.
       lte_wm_levels_valid = TRUE;
    }
  }
  
  retval = ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_TDS_WM_THRESHOLD,
                           (unsigned char *)(&tds_um_wm_threshold),
                           sizeof(tds_um_wm_threshold),
                           subs_id);
  if (retval == -1)
  {
    DS_3GPP_MSG0_HIGH("TDS WM NV read failed. Using the default values");
  }
  else
  {
    DS_3GPP_MSG0_HIGH("TDS WM NV read success");
    /* TDS */
    if (TRUE == tds_um_wm_threshold.tds_wm_levels_valid)
    {
      if ((tds_um_wm_threshold.tds_ul_dne > tds_um_wm_threshold.tds_ul_hi) &&
          (tds_um_wm_threshold.tds_ul_hi > tds_um_wm_threshold.tds_ul_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_dne 
                                           = tds_um_wm_threshold.tds_ul_dne;
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_hi 
                                           = tds_um_wm_threshold.tds_ul_hi;
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_ul_lo 
                                           = tds_um_wm_threshold.tds_ul_lo;
      }
      if ((tds_um_wm_threshold.tds_dl_dne > tds_um_wm_threshold.tds_dl_hi) &&
          (tds_um_wm_threshold.tds_dl_hi > tds_um_wm_threshold.tds_dl_lo))
      {
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_dne 
                                           = tds_um_wm_threshold.tds_dl_dne;
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_hi 
                                           = tds_um_wm_threshold.tds_dl_hi;
        nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.tds_dl_lo 
                                           = tds_um_wm_threshold.tds_dl_lo;
      }
      nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold.
        tds_wm_levels_valid = TRUE;
    }
  }

}
/*===========================================================================
FUNCTION DS_3GPP_NV_GET_BACK_PRESSURE_PARAMS

DESCRIPTION
  This function is used to retrieve the back pressure parameters per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_cntxt_tput_opt_struct_type -back pressure parameters information

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_cntxt_tput_opt_struct_type ds_3gpp_nv_get_back_pressure_params
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_cntxt_tput_opt_struct_type back_pressure_params;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  memset(&back_pressure_params, 0,
         sizeof(ds_bearer_cntxt_tput_opt_struct_type));

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    back_pressure_params = nv_per_subs_info->ds_bearer_cntxt_tput_opt_params;
  }
  else
  {
    back_pressure_params.timer_val = 
                                      DS3GPP_THROUGHPUT_TMR_DEFAULT_VAL;
    back_pressure_params.back_pressure_removal = TRUE;
  }
  return back_pressure_params;
}
/*===========================================================================
FUNCTION DS_3GPP_NV_GET_BTPUT_OPT_PARAMS_PRESENT_IN_NV

DESCRIPTION
  This function is used to retrieve whether the throughput optimization
  parameters are present per subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean -present or not present

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_nv_get_tput_opt_params_present_in_nv
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean is_tput_opt_params_present_in_nv;
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    is_tput_opt_params_present_in_nv = nv_per_subs_info->is_tput_opt_params_present_in_nv;
  }
  else
  {
    is_tput_opt_params_present_in_nv = FALSE; 
  }
  return is_tput_opt_params_present_in_nv;
}

/*===========================================================================
FUNCTION DS_3GPP_NV_GET_TDS_WM_THRESHOLD

DESCRIPTION
  This function is used to retrieve tds watermark threshold values per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type - td wm info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_cntxt_tds_um_wm_threshold_struct_type  ds_3gpp_nv_get_tds_wm_threshold
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_cntxt_tds_um_wm_threshold_struct_type tds_um_wm_threshold;
  ds_3gpp_nv_per_subs_info_type                    *nv_per_subs_info;

  memset(&tds_um_wm_threshold, 0,
         sizeof(ds_bearer_cntxt_tds_um_wm_threshold_struct_type));

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    tds_um_wm_threshold = nv_per_subs_info->ds_bearer_cntxt_tds_um_wm_threshold;
  }
	else
	{
		tds_um_wm_threshold.tds_wm_levels_valid = FALSE;
	}

  return tds_um_wm_threshold;
}
/*===========================================================================
FUNCTION DS_3GPP_NV_GET_UM_WM_THRESHOLD

DESCRIPTION
  This function is used to retrieve um watermark threshold values per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  ds_bearer_cntxt_um_wm_threshold_struct_type - um wm info

SIDE EFFECTS
  None.

===========================================================================*/
ds_bearer_cntxt_um_wm_threshold_struct_type  ds_3gpp_nv_get_um_wm_threshold
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_bearer_cntxt_um_wm_threshold_struct_type um_wm_threshold;
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  memset(&um_wm_threshold, 0,
         sizeof(ds_bearer_cntxt_um_wm_threshold_struct_type));

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    um_wm_threshold = nv_per_subs_info->ds_bearer_cntxt_um_wm_threshold;
  }
  else
  {
    um_wm_threshold.gprs_wm_levels_valid = FALSE;
    um_wm_threshold.wcdma_wm_levels_valid = FALSE;
    um_wm_threshold.lte_wm_levels_valid = FALSE;
  }
  return um_wm_threshold;
}

/*===========================================================================
FUNCTION DS_3GPP_NV_GET_MAX_RAB_REESTAB_RETRY_CNT

DESCRIPTION
  This function is used to retrieve umax rab reestablishment retry count per
  subscription
 
  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint32 -max rab reestab count

SIDE EFFECTS
  None.

===========================================================================*/

uint32 ds_3gpp_nv_get_max_rab_reestab_retry_cnt
(
  sys_modem_as_id_e_type subs_id
)
{
  uint8 max_rab_reestab_req_retry_cnt;
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);


  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    max_rab_reestab_req_retry_cnt = nv_per_subs_info->max_rab_reestab_req_retry_cnt;
  }
  else
  {
    max_rab_reestab_req_retry_cnt = RAB_REESTAB_RETRY_CNT_DEFAULT;
  }
  return max_rab_reestab_req_retry_cnt;
     
}

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_MTU_FROM_NV

DESCRIPTION
  This function is used to read mtu value from nv item per subscription

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_nv_manager_read_mtu_from_nv 
(
   sys_modem_as_id_e_type    subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_MTU,
                    (unsigned char *)&(nv_per_subs_info->mtu_size),
                    sizeof(uint16),
                    subs_id ) > 0 )
  {
    DS_3GPP_MSG1_HIGH("NV read Success. MTU set to %d", nv_per_subs_info->mtu_size);
  }
  else
  {
    nv_per_subs_info->mtu_size = DEFAULT_DATA_UNIT;
  } 

  return;
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_AUTH_FALL_BACK

DESCRIPTION
  This function is used to read auth fall back from nv item per subscription

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_nv_manager_read_auth_fallback
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  uint8 ds_3gpp_auth_fb_enabled = 0;
  int  cfg_get_retval;  /* result of ds_3gpp_cfg_get()*/

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  cfg_get_retval = ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_AUTH_TYPE_FALLBACK,
                        (unsigned char *) &ds_3gpp_auth_fb_enabled,
                        sizeof(ds_3gpp_auth_fb_enabled),
                        subs_id);

  if(cfg_get_retval == -1 ) 
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_auth_mgr_init: Cannot read EFS Config item"
                       " DS_3GPP_CFG_AUTH_TYPE_FALLBACK");
  }
  nv_per_subs_info->auth_fb_enabled =  ds_3gpp_auth_fb_enabled;
}
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_GET_AUTH_FALLBACK

DESCRIPTION
  This function is used to get auth fall back from nv item per subscription

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - auth fallback infor

SIDE EFFECTS
  None.

===========================================================================*/

uint8 ds_3gpp_nv_manager_get_auth_fallback
(
   sys_modem_as_id_e_type    subs_id
)
{
	uint8 ds_3gpp_auth_fb_enabled;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if(ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    ds_3gpp_auth_fb_enabled = nv_per_subs_info->auth_fb_enabled;
  }
  else
  {
    ds_3gpp_auth_fb_enabled = FALSE;
  }
  return ds_3gpp_auth_fb_enabled;

}


/*===========================================================================
FUNCTION  DS_3GPP_NV_MANGER_READ_REDIAL_NO_CAUSE_CODE_FROM_NV

DESCRIPTION
  This function reads the redial for no cause from NV item
  NV item per subscription: 72563
 
PARAMETERS
  subs_id  subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_redial_no_cause_code_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - */
  if( ds_3gpp_cfg_get_subsid( DS_3GPP_REDIAL_FOR_NO_CAUSE_CODE, 
                   (uint8 *)&(nv_per_subs_info->ds_3gpp_redial_for_no_cause_code), 
                   sizeof(uint8),
                   subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_REDIAL_FOR_NO_CAUSE_CODE,"
                      "using default");
    nv_per_subs_info->ds_3gpp_redial_for_no_cause_code = TRUE;
  }
  return ;
}/*ds_3gpp_pdn_cntxt_read_redial_no_cause_code_from_nv*/

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_GET_AUTH_FALLBACK

DESCRIPTION
  This function is used to get auth fall back from nv item per subscription

  PARAMETERS
  subs_id- subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - auth fallback infor

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_nv_manager_read_apm_apn_switching
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info; 
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  /* - - - - - - - - - - - - - - - - - - -- - - - - - - - - - - - - - - - - - */
  if( ds_3gpp_cfg_get_subsid( DS_3GPP_APM_APN_SWITCHING, 
                            (uint8 *)&(nv_per_subs_info->apm_apn_switching), 
                            sizeof(uint8),
                            subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_APM_APN_SWITCHING,"
                      "using default");
  nv_per_subs_info->apm_apn_switching = TRUE;
  }
  return ;
}


#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_lte_rel_ver_from_nv

DESCRIPTION
  This function gets the Lte release version from NV item

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_lte_rel_ver_from_nv()
{

  ds_3gpp_nv_lte_rel_version = lte_rrc_get_3gpp_release_version();

  DS_3GPP_MSG3_HIGH("LTE Rel ver set to %d", ds_3gpp_nv_lte_rel_version, 0, 0);

  return;
}

#endif


/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_EFS_3GPP_REL_VERSION

DESCRIPTION 
  This function reads the 3GGP_REL_VERSION NV and saves for caching 
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  None
SIDE EFFECTS
  None.
===========================================================================*/
void ds_3gpp_nv_manager_read_efs_3gpp_rel_version()
{
  ds_3gpp_3gpp_rel_version = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Check if we have to fill this protocol info pkt in the pdp 
    context request based on NV item 3gpp_rel_version.
  -------------------------------------------------------------------------*/
  if (ds_3gpp_cfg_get( DS_3GPP_REL_VERSION, 
                      (uint8 *)&(ds_3gpp_3gpp_rel_version), 
                      sizeof(ds_3gpp_3gpp_rel_version)) > 0) 
  {
    DS_3GPP_MSG1_HIGH("3gpp_rel_version set to %d in EFS", 
                      ds_3gpp_3gpp_rel_version);
  }
  else
  {
    #ifdef FEATURE_DATA_PRE_REL8
      ds_3gpp_3gpp_rel_version = (uint8)DS_3GPP_REL7;
    #else
      ds_3gpp_3gpp_rel_version = (uint8)DS_3GPP_REL8;
                              /* Default to MAX version supported */
    #endif /* FEATURE_DATA_PRE_REL8 */

    DS_3GPP_MSG1_HIGH("3gpp_rel_version is set to default: %d",
                       ds_3gpp_3gpp_rel_version);
  }
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_3gpp_rel_version

DESCRIPTION 
  This function returns 3GGP_REL_VERSION from cache  
 
PARAMETERS
  None
DEPENDENCIES
  None.

RETURN VALUE
  cached 3GPP_REL_VERSION NV value
SIDE EFFECTS
  None.
===========================================================================*/
uint8 ds_3gpp_nv_manager_get_3gpp_rel_version()
{
  DS_3GPP_MSG3_MED("3gpp_rel_version is : %d",
                       ds_3gpp_3gpp_rel_version,0,0);
  return ds_3gpp_3gpp_rel_version;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_l_to_w_qos_continuity

DESCRIPTION
		This function reads the nv l_to_w qos continuity
	
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/
void ds_3gpp_nv_manager_read_l_to_w_qos_continuity()
{
#ifdef FEATURE_DATA_LTE

  ds_3gpp_l_to_w_qos_cont_s      l_to_w_qos_cont;

  if(ds_3gpp_cfg_get(DS_3GPP_L_TO_W_QOS_CONTINUITY,
                    (unsigned char *)&l_to_w_qos_cont,
                    sizeof(l_to_w_qos_cont) ) > 0 )
  {
     ds_3gpp_nv_l_to_w_qos_continuity = l_to_w_qos_cont.l_to_w_qos_cont_enabled;
     DS_3GPP_MSG1_HIGH("ds_3gpp_nv_l_to_w_qos_continuity NV: is %d in EFS", 
                        ds_3gpp_nv_l_to_w_qos_continuity);
  }
  else
  {
    ds_3gpp_nv_l_to_w_qos_continuity = FALSE;
    DS_3GPP_MSG0_ERROR("Failed to read NV item to check "
                       "ds_3gpp_nv_l_to_w_qos_continuity, setting to FALSE");
  }  
#endif 
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_l_to_w_qos_continuity_nv

DESCRIPTION
  This function gets the L to W QOS continuity NV item

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE/FALSE

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_nv_manager_get_l_to_w_qos_continuity_nv()
{
  DS_3GPP_MSG1_HIGH("ds_3gpp_nv_manager_get_l_to_w_qos_continuity_nv : %d ", 
  	                ds_3gpp_nv_l_to_w_qos_continuity);
  return ds_3gpp_nv_l_to_w_qos_continuity;
}
/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_umts_nw_init_qos_support_nv

DESCRIPTION
 This function gets the ds_3gpp_nv_umts_nw_init_qos_support NV item
 NV: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/
void ds_3gpp_nv_manager_read_umts_nw_init_qos_support_nv()
{
    /*-------------------------------------------------------------------------
    Read the NV item that configures if NW initiated QOS is supported on UMTS.    
  -------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get(DS_3GPP_ENABLE_UMTS_NETWORK_INITIATED_QOS,
                     (uint8 *) &ds_3gpp_nv_umts_nw_init_qos_support,
                     sizeof(uint8)) > 0)
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_nv_umts_nw_init_qos_support NV read: %d", 
                       ds_3gpp_nv_umts_nw_init_qos_support);

  }
  else
  {
    DS_3GPP_MSG0_HIGH("ds_3gpp_nv_umts_nw_init_qos_support NV not configured,enabling it by default");
    ds_3gpp_nv_umts_nw_init_qos_support = DS_3GPP_NV_ENABLED;
  } 
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_umts_nw_init_qos_support_nv

DESCRIPTION
  This function gets the ds_3gpp_nv_umts_nw_init_qos_support NV item
  NV: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  1 - If NV enabled
  0 - otherwise

SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_3gpp_nv_manager_get_umts_nw_init_qos_support_nv()
{
  DS_3GPP_MSG1_HIGH("ds_3gpp_nv_manager_get_umts_nw_init_qos_support_nv : %d ", 
  	                ds_3gpp_nv_umts_nw_init_qos_support);
  return ds_3gpp_nv_umts_nw_init_qos_support;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_umts_ue_init_qos_support_nv

DESCRIPTION
 This function gets the umts_ue_init_qos_support NV item
 NV: 66044
  modem/data/3gpp/umtsps/qos_support

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/
void ds_3gpp_nv_manager_read_umts_ue_init_qos_support_nv()
{
    /*-------------------------------------------------------------------------
    Read the NV item that configures if NW initiated QOS is supported on UMTS.    
  -------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get(DS_3GPP_CFG_QOS_SUPPORT,
                     (unsigned char*) &ds_3gpp_nv_umts_ue_init_qos_support,
                     sizeof(boolean)) > 0)
  {
    DS_3GPP_MSG1_HIGH("ds_3gpp_nv_umts_ue_init_qos_support NV read: %d", 
                       ds_3gpp_nv_umts_ue_init_qos_support);

  }
  else
  {
    DS_3GPP_MSG0_HIGH("ds_3gpp_nv_umts_ue_init_qos_support:Not configured,disabling it by default");
    ds_3gpp_nv_umts_ue_init_qos_support = FALSE;
  } 
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_umts_ue_init_qos_support_nv

DESCRIPTION
  This function gets the umts_nw_init_qos_support NV item
  NV: 66044
  modem/data/3gpp/umtsps/qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE - If NV enabled
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_umts_ue_init_qos_support_nv()
{
  DS_3GPP_MSG1_HIGH("ds_3gpp_nv_manager_get_umts_ue_init_qos_support_nv : %d ", 
  	                ds_3gpp_nv_umts_ue_init_qos_support);
  return ds_3gpp_nv_umts_ue_init_qos_support;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_bcm_validation

DESCRIPTION
  This function reads the nv bcm validation.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/

void ds_3gpp_nv_manager_read_bcm_validation()
{
  ds_3gpp_bcm_info_validation_s  bcm_info_validation;
  if(ds_3gpp_cfg_get(DS_3GPP_BCM_INFO_VALIDATION,
                    (unsigned char *)&bcm_info_validation,
                    sizeof(bcm_info_validation) ) > 0 )
  {
     ds_3gpp_nv_bcm_validation_on_bearer_mod = bcm_info_validation.bcm_info_validation_enabled;
     DS_3GPP_MSG1_HIGH("ds_3gpp_nv_bcm_validation_on_bearer_mod NV: is %d in EFS", 
                        ds_3gpp_nv_bcm_validation_on_bearer_mod);
  }
  else
  {
    ds_3gpp_nv_bcm_validation_on_bearer_mod = TRUE;
    DS_3GPP_MSG0_ERROR("Failed to read NV item to check "
                       "ds_3gpp_nv_bcm_validation_on_bearer_mod, setting to TRUE");
  }
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_multi_pdn_dun_nv

DESCRIPTION
  This function reads the multi pdn dun nv.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE  
..None.

SIDE EFFECTS
 .None
===========================================================================*/



void ds_3gpp_nv_manager_read_multi_pdn_dun_nv()
{
  uint8 multi_pdn_dun_is_enabled = FALSE;
  if(ds_3gpp_cfg_get(DS_3GPP_CFG_MULTI_PDN_DUN_ENABLED,
                     (unsigned char*) &multi_pdn_dun_is_enabled, 
                      sizeof(uint8)) == -1 )
  {
    dsumts_rmsmi_multi_pdn_dun_is_enabled = FALSE;
    DS_3GPP_MSG0_HIGH("Multi PDN DUN NV cannot be read, default: disabled");
  }
  else
  {
    if(multi_pdn_dun_is_enabled == 0x1)
    {
      dsumts_rmsmi_multi_pdn_dun_is_enabled = TRUE;
      DS_3GPP_MSG0_HIGH("Multi PDN DUN is enabled");
    }
    else
    {
      dsumts_rmsmi_multi_pdn_dun_is_enabled = FALSE;
      DS_3GPP_MSG0_HIGH("Multi PDN DUN is disabled");
    }
  }
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_enable_pdn_throt_per_rat_from_nv

DESCRIPTION
  This function reads the enable_pdn_throt_per_rat NV item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_nv_manager_read_enable_pdn_throt_per_rat_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p = NULL;
  /*----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_ENABLE_PDN_THROTTLING_PER_RAT,
                     (void*)&(nv_per_subs_info_p->enable_pdn_throt_per_rat),
                     sizeof(boolean),
                     subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                      "DS_3GPP_ENABLE_PDN_THROTTLING_PER_RAT"
                      "Default is false");
    nv_per_subs_info_p->enable_pdn_throt_per_rat = FALSE;
  }
} /* ds_3gpp_nv_manager_read_enable_pdn_throt_per_rat_from_nv */

/*===========================================================================
  FUNCTION ds_3gpp_nv_manager_get_multi_pdn_dun_nv

  DESCRIPTION
    This function gets the multi pdn dun nv item

  PARAMETERS
    None

  DEPENDENCIES
    None.

  RETURN VALUE  
    TRUE /FALSE

  SIDE EFFECTS

===========================================================================*/

boolean ds_3gpp_nv_manager_get_multi_pdn_dun_nv()
{  
  DS_3GPP_MSG1_HIGH("ds_3gpp_nv_manager_get_multi_pdn_dun_nv : %d ", 
                    dsumts_rmsmi_multi_pdn_dun_is_enabled);

  return dsumts_rmsmi_multi_pdn_dun_is_enabled;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_bcm_info_validation_nv

DESCRIPTION
  This function gets the BCM validation NV setting.

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  TRUE /FALSE

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_nv_manager_get_bcm_info_validation_nv()
{
  DS_3GPP_MSG1_HIGH("ds_3gpp_nv_manager_get_bcm_info_validation_nv : %d ", 
                  ds_3gpp_nv_bcm_validation_on_bearer_mod);
  return ds_3gpp_nv_bcm_validation_on_bearer_mod;
}

/*===========================================================================
  FUNCTION ds_3gpp_nv_manager_READ_NAS_UMTS_REL_VERSION
  
  DESCRIPTION
  This function is is called in 3gpp nv manager init.It reads the NAS
  Complaince NV item and updates the global variable ds_3gpp_nv_umts_rel_version
  accordingly for umts specific throttling scenerios.
    
  PARAMETERS  
  None.
      
  DEPENDENCIES 
  None.
    
  RETURN VALUE 
  None.
     
  SIDE EFFECTS 
  None.
    
===========================================================================*/

void ds_3gpp_nv_manager_read_nas_umts_rel_version()
{
  nv_stat_enum_type               nv_status = NV_DONE_S;
  nv_item_type                    ds_nv_item;
	sys_modem_as_id_e_type           subs_id =SYS_MODEM_AS_ID_1;

   memset(&ds_nv_item, 0, sizeof(nv_item_type));

  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_NAS_RELEASE_COMPLIANCE_I, &ds_nv_item,subs_id);

  if(nv_status == NV_DONE_S)
  {
    ds_3gpp_nv_umts_rel_version = (ue_nas_rel_compliance_type)ds_nv_item.nas_release_compliance;
    DATA_MSG1_HIGH("NV_NAS_RELEASE_COMPLIANCE_I is set as %d",ds_3gpp_nv_umts_rel_version);	
  }
  else
  {
    DATA_MSG0_ERROR("Unable to read the NV_NAS_RELEASE_COMPLIANCE_I setting as REL7");
    ds_3gpp_nv_umts_rel_version = NV_NAS_COMPLIANCE_REL7;
  }

} /* ds_3gpp_nv_manager_read_nas_umts_rel_version */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_check_ehplmn_list_from_nv

DESCRIPTION
  This function reads the check_ehplmn_list NV item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_nv_manager_read_check_ehplmn_list_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  /*-----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CHECK_EHPLMN_LIST,
                           (void*)&(nv_per_subs_info_p->check_ehplmn_list),
                           sizeof(boolean),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_CHECK_EHPLMN_LIST"
                       "Default is true");
    nv_per_subs_info_p->check_ehplmn_list = TRUE;
  }
} /* ds_3gpp_nv_manager_read_check_ehplmn_list_from_nv */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_mnc_len_from_nv

DESCRIPTION
  This function reads the MNC length NV item for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_nv_manager_read_mnc_len_from_nv
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_nv_per_subs_info_type  *nv_per_subs_info_p;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_MNC_LEN,
                           (void*)&(nv_per_subs_info_p->mnc_len),
                           sizeof(uint8),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_HIGH("Cannot read EFS Config item DS_3GPP_CFG_MNC_LEN"
                      "Default is 3");
    nv_per_subs_info_p->mnc_len = DS3G_MMGSDI_MNC_3_DIGITS;
  }
} /* ds_3gpp_nv_manager_read_mnc_len_from_nv */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_ignore_ui_data_throttling_from_nv

DESCRIPTION
  This function reads the  ignore_ui_data_throttling NV item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_nv_manager_read_ignore_ui_data_throttling_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  /*-----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_IGNORE_DATA_SETTINGS,
                           (void*)&(nv_per_subs_info_p->ignore_ui_data_throttling),
                           sizeof(boolean),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_IGNORE_UI_DATA_THROTTLING"
                       "Default is false");
    nv_per_subs_info_p->ignore_ui_data_throttling = TRUE;
  }
} /* ds_3gpp_nv_manager_read_ignore_ui_data_throttling_from_nv */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_rel_10_throttling_per_plmn__from_nv

DESCRIPTION
  This function reads the rel_10_throttling_per_plmn from NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_rel_10_throttling_per_plmn_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_REL10_THROTTLING_PER_PLMN,
                     (void*)&(nv_per_subs_info->nv_rel10_throttling_per_plmn),
                     sizeof(boolean),
                     subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                      "DS_3GPP_CFG_REL10_THROTTLING_PER_PLMN"
                      "Default is false");
    nv_per_subs_info->nv_rel10_throttling_per_plmn = FALSE;
  }

}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_read_regular_pdn_throttling_from_nv

DESCRIPTION
  This function reads the regular pdn throttling  NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/


void ds_3gpp_nv_manager_read_regular_pdn_throttling_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

   if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_REGULAR_PDN_THROTTLING,
                             (boolean *)(&nv_per_subs_info->ds_3gpp_reg_pdn_throttling),
                             sizeof(boolean),
                             subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                      "DS_3GPP_CFG_REGULAR_PDN_THROTTLING."
                      "Default is false");
    nv_per_subs_info->ds_3gpp_reg_pdn_throttling = FALSE;
  }
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_multi_pdn_same_apn_nv

DESCRIPTION
  This function reads the multi pdn same apn NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_3gpp_nv_manager_read_multi_pdn_same_apn_nv
(
  sys_modem_as_id_e_type    subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_MULTI_PDN_SAME_APN,
                            (unsigned char *)&nv_per_subs_info->allow_multi_pdn_same_apn,
                            sizeof(nv_per_subs_info->allow_multi_pdn_same_apn),
                            subs_id ) > 0 )
 {
    DS_3GPP_MSG1_HIGH("NV read Success. Allow multi pdn set as : %d", 
                       nv_per_subs_info->allow_multi_pdn_same_apn);
 }
 else
 {
    DS_3GPP_MSG0_HIGH("NV read failed. Using the default value");
    nv_per_subs_info->allow_multi_pdn_same_apn = FALSE;
 }



  return;
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_global_throttling_nv

DESCRIPTION
  This function reads the global throttling NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_3gpp_nv_manager_read_global_throttling_nv
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_GLOBAL_THROTTLING,
                    (unsigned char *)&nv_per_subs_info->global_throttling_nv,
                    sizeof(nv_per_subs_info->global_throttling_nv),
                    subs_id ) > 0 )
  {
     DS_3GPP_MSG1_HIGH("ds_3gpp_global_throttling_enabled NV: is %d in EFS", 
                        nv_per_subs_info->global_throttling_nv.global_throttling );
  }
  else
  {
    nv_per_subs_info->global_throttling_nv.global_throttling = FALSE;
    DS_3GPP_MSG1_ERROR("Failed to read NV item to check "
                       "ds_3gpp_global_throttling_enabled, setting to default"
                       " value:%d",nv_per_subs_info->global_throttling_nv.global_throttling );
  }

}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_multi_pdn_same_apn

DESCRIPTION
  This function gets the multi pdn same apn nv value for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether Multi Pdn same apn nv is enabled.

SIDE EFFECTS
===========================================================================*/
boolean ds_3gpp_nv_manager_get_multi_pdn_same_apn
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
 
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return FALSE;
  }

  return nv_per_subs_info->allow_multi_pdn_same_apn;
} /* ds_3gpp_get_reg_pdn_throttling */


/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_global_throttling

DESCRIPTION
  This function gets the global throttling nv value for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether global throttling  nv is enabled.

SIDE EFFECTS
===========================================================================*/

boolean ds_3gpp_nv_manager_get_global_throttling
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  ret_val = nv_per_subs_info->global_throttling_nv.global_throttling;
  return ret_val;
} /* ds_3gpp_get_reg_pdn_throttling */




/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_reg_pdn_throttling 

DESCRIPTION
  This function gets the Regular PDN Throttling Flag for each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether Regular PDN Throttling is enabled.

SIDE EFFECTS
===========================================================================*/
boolean ds_3gpp_nv_manager_get_reg_pdn_throttling 
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return FALSE;
  }
  ret_val = nv_per_subs_info->ds_3gpp_reg_pdn_throttling;
  return ret_val;
} /* ds_3gpp_get_reg_pdn_throttling */

/*===========================================================================
FUNCTION DS_3GPP_SET_REG_PDN_THROTTLING

DESCRIPTION
  This function sets the Regular PDN Throttling Flag for each subscription

PARAMETERS
  Boolean Flag
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
===========================================================================*/
void ds_3gpp_nv_manager_set_reg_pdn_throttling 
(
  boolean flag,
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  nv_per_subs_info->ds_3gpp_reg_pdn_throttling = flag;
} /* ds_3gpp_set_reg_pdn_throttling */


/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_DISABLE_APN_MATCHING_FROM_NV

DESCRIPTION
  This function reads the disable_apn_matching NV(73529 ) item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_disable_apn_matching_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_DISABLE_APN_MATCHING,
                           (void*)&(nv_per_subs_info->ds_3gpp_disable_apn_matching),
                           sizeof(boolean),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_DISABLE_APN_MATCHING"
                       "Default is false");
    nv_per_subs_info->ds_3gpp_disable_apn_matching = FALSE;
  }
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN_FROM_NV

DESCRIPTION
  This function reads null_apn_support_for_non_attach_pdn NV item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_null_apn_support_for_non_attach_pdn_from_nv
(
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN,
                           (void*)&(nv_per_subs_info->
                           null_apn_support_for_non_attach_pdn),
                           sizeof(boolean),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                       "NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN"
                       "Default is false");
    nv_per_subs_info->null_apn_support_for_non_attach_pdn = FALSE;
  }
}/*ds_3gpp_nv_manager_read_null_apn_support_for_non_attach_pdn_from_nv*/
#endif

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_APN_PARAM_CHG_CFG_FROM_NV

DESCRIPTION
  This function reads the apn_param_chg_cfg NV(73603 ) item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_apn_param_chg_cfg_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_APN_PARAM_CHG,
                           (void*)&(nv_per_subs_info->apn_param_chg_cfg),
                           sizeof(boolean),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_CFG_APN_PARAM_CHG"
                       "Default is false");
    nv_per_subs_info->apn_param_chg_cfg = FALSE;
  }
}


#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_DL_TPUT_T_ACCUMULATE_FROM_NV

DESCRIPTION
  This function reads the t_accumuate NV(73587) item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_3gpp_nv_manager_read_dl_tput_t_accumulate_from_nv
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  int16                             dss_errno = 0;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }


  if(ds_3gpp_cfg_get_subsid(DS_3GPP_DL_TPUT_T_ACCUMULATE,
                           (void*)&(nv_per_subs_info->dl_tput_t_accumulate),
                           sizeof(uint32),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_DL_TPUT_T_ACCUMULATE"
                       "Default is false");
    nv_per_subs_info->dl_tput_t_accumulate = 
       DS_LTE_DL_TPUT_T_ACCUMULATE_DEFAULT_VALUE;
  }

  ps_sys_conf_set_ex(PS_SYS_TECH_ALL,
                         PS_SYS_CONF_DL_MIN_THROUGHPUT_TIMER,
                         (ps_sys_subscription_enum_type)
                           ds3gsubsmgr_subs_id_cm_to_ds(subs_id),
                         &(nv_per_subs_info->dl_tput_t_accumulate), &dss_errno);


}
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_mtu_size

DESCRIPTION
  This function gets the mtu size for the nv item for each subscriprion

PARAMETERS
  subs_id subscription id
  
DEPENDENCIES
  None.

RETURN VALUE  
  uint16  mtu value

SIDE EFFECTS

===========================================================================*/
uint16 ds_3gpp_nv_manager_get_mtu_size
( 
  sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  uint32 mtu_size = 0;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return mtu_size;
  }

  mtu_size = nv_per_subs_info->mtu_size;

  return mtu_size;
}



/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_is_no_cause_code_redial_set

DESCRIPTION
  This function checks  whether ds_3gpp_redial_for_no_cause_code is set for
  each subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  
  TRUE- if successfull
  FALSE- invalid PDN ptr/invalid SUBS_ID

SIDE EFFECTS

===========================================================================*/
boolean ds_3gpp_nv_manager_get_is_no_cause_code_redial_set
(
  sys_modem_as_id_e_type        subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  ret_val = nv_per_subs_info->ds_3gpp_redial_for_no_cause_code;
  return ret_val;
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_apm_apn_switching

DESCRIPTION
  This function returns true ;if the nv item corresponding to the 
  apm apn switching per subscription is set to true


PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/


boolean ds_3gpp_nv_manager_get_apm_apn_switching
(
  sys_modem_as_id_e_type        subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = TRUE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  ret_val = nv_per_subs_info->apm_apn_switching;
  return ret_val;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_check_ehplmn_list

DESCRIPTION
  This function returns true if check_ehplmn_list NV item is set to TRUE

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_check_ehplmn_list
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  boolean                        ret_val = FALSE;
  /*----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info_p->check_ehplmn_list;

  return ret_val;
} /* ds_3gpp_nv_manager_get_check_ehplmn_list */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_mnc_len

DESCRIPTION
  This function gets mnc len for imsi from mnc len NV item

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - mnc length

SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_3gpp_nv_manager_get_mnc_len
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  uint8                          ret_val = DS3G_MMGSDI_MNC_3_DIGITS;
  /*----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info_p->mnc_len;

  return ret_val;
} /* ds_3gpp_nv_manager_get_mnc_len */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat

DESCRIPTION
  This function returns true if the enable_pdn_throt_per_rat NV item is set to
  TRUE

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  boolean                        ret_val = FALSE;
  /*-----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info_p->enable_pdn_throt_per_rat;

  return ret_val; 
} /* ds_3gpp_nv_manager_get_enable_pdn_throt_per_rat */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_ignore_ui_data_throttling

DESCRIPTION
  This function returns the value of ignore_ui_data_throttling
  NV item

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Value of ignore_ui_data_throttling NV item

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_ignore_ui_data_throttling
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info_p;
  boolean                        ret_val = FALSE;
  /*-----------------------------------------------------------------------*/

  nv_per_subs_info_p = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info_p))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info_p->ignore_ui_data_throttling;

  return ret_val; 
} /* ds_3gpp_nv_manager_get_ignore_ui_data_throttling */

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn

DESCRIPTION
  This function returns true ;if the nv item corresponding to the 
  release 10 throttling per plmn is set to true


PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean ds_3gpp_nv_manager_get_nv_rel_10_throttling_per_plmn
(
   sys_modem_as_id_e_type        subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
 if(nv_per_subs_info->nv_rel10_throttling_per_plmn)
  {
    ret_val=TRUE;
  }

  return ret_val; 
}


/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_nv_umts_rel_version

DESCRIPTION
  This Function returns the NAS Complaince NV item  read during nv manager
  initialization

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  boolean - True -> Umts Nas release version 10 or above
          - False -> Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_is_umts_rel_10_enabled()
{
  boolean ret_val = FALSE;

  if( (ds_3gpp_nv_umts_rel_version >= NV_NAS_COMPLIANCE_REL10)
#ifdef FEATURE_DATA_LTE 
      || (ds_3gpp_nv_lte_rel_version >= LTE_3GPP_REL10)
#endif /* FEATURE_DATA_LTE */
    )
  {
    ret_val=TRUE;
  }

  return ret_val; 
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_nv_lte_rel_10

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/lte/lte_3gpp_release_ver is LTE_3GPP_REL10
  and release 10 throttling is enabled

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_nv_lte_rel_10
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
#ifdef FEATURE_DATA_LTE
  if(ds_3gpp_nv_lte_rel_version >= LTE_3GPP_REL10)
  {
    ret_val=TRUE;
  }
#endif /*FEATURE_DATA_LTE */
  return ret_val; 
}


/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_DISABLE_APN_MATCHING

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/data/3gpp/lteps/disable_apn_matching 

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_disable_apn_matching
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
  ret_val = nv_per_subs_info->ds_3gpp_disable_apn_matching;
  return ret_val;
}

#ifdef FEATURE_DATA_LTE
/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_NULL_APN_SUPPORT_FOR_NON_ATTACH_PDN

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/data/3gpp/lteps/null_apn_support_for_non_attach_pdn

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - NV set to 1
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_null_apn_support_for_non_attach_pdn
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
  ret_val = nv_per_subs_info->null_apn_support_for_non_attach_pdn;
  return ret_val;
}/*ds_3gpp_nv_manager_get_null_apn_support_for_non_attach_pdn*/
#endif /*FEATURE_DATA_LTE*/

/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_APN_PARAM_CHG_CFG

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/data/3gpp/lteps/disable_apn_matching 

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_nv_manager_get_apn_param_chg_cfg
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
  ret_val = nv_per_subs_info->apn_param_chg_cfg;
  return ret_val;
}
/*ds_3gpp_nv_manager_get_apn_param_chg_cfg*/





/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_DL_TPUT_T_ACCUMULATE

DESCRIPTION
  This function returns true if the nv item corresponding to the file
  /nv/item_files/modem/data/3gpp/lteps/t_accumulate  (73587)

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  Value in the efs or default value

SIDE EFFECTS
  None

===========================================================================*/

uint32 ds_3gpp_nv_manager_get_dl_tput_t_accumulate
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info = NULL;
  uint32 ret_val = 0;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
  ret_val = nv_per_subs_info->dl_tput_t_accumulate;
  DS_3GPP_MSG2_LOW("ds_3gpp_nv_manager_get_dl_tput_t_accumulate %d for subs id %d"
                        ,ret_val, subs_id);
  return ret_val;
}



/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_nv_umts_rel_version

DESCRIPTION
  This Function reads the NAS Complaince NV item and updates the global variable 
  ds_3gpp_nv_umts_rel_version accordingly for umts specific scenerios.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  ue_nas_rel_compliance_type enum type

SIDE EFFECTS
  None

===========================================================================*/
ue_nas_rel_compliance_type ds_3gpp_nv_manager_get_nv_umts_rel_version()
{
  return ds_3gpp_nv_umts_rel_version; 
}/* ds_3gpp_pdn_get_nv_umts_rel_version */


/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_nbns_enabled

DESCRIPTION
  This Function reads the  nbns nv item per subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None

===========================================================================*/


void ds_3gpp_nv_manager_read_nbns_enabled
(
   sys_modem_as_id_e_type subs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        ds_nv_item;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
   return;
  }
  memset(&ds_nv_item, 0, sizeof(nv_item_type));
  /*-------------------------------------------------------------------------
    determine if NBNS is enabled in this build.
  -------------------------------------------------------------------------*/
  nv_status = ds3gcfgmgr_read_legacy_nv_ex(NV_UMTS_NBNS_ENABLE_I, &ds_nv_item,subs_id);

  /*-------------------------------------------------------------------------
    Use the NV value, If there was some problem reading NV, enable NBNS by 
        default.  For v6, NBNS will not be enabled for now.
  -------------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    nv_per_subs_info->nbns_enabled = ds_nv_item.umts_nbns_enable;
  }
  else 
  {
    DS_3GPP_MSG1_HIGH("Bad NV read status %d", nv_status);
    nv_per_subs_info->nbns_enabled = TRUE;
  }
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_nbns_enabled

DESCRIPTION
  This Function returns true; if nbns nv item is enabled

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean true-  if nbns nv item is enabled
          false   if nbns nv item is disabled

SIDE EFFECTS
  None

===========================================================================*/


boolean ds_3gpp_nv_manager_get_nbns_enabled
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = TRUE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
	ret_val = nv_per_subs_info->nbns_enabled;

	return ret_val; 
}


/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_call_orig_before_ps_attach

DESCRIPTION
  This function returns true if call orig is allowed before ps attach.
..This value is read per subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_nv_manager_get_call_orig_before_ps_attach
(
  sys_modem_as_id_e_type subs_id
)
{
  boolean call_orig_before_ps_attach = TRUE;
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return call_orig_before_ps_attach;
  }
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CALL_ORIG_ALLOWED_BEFORE_PS_ATTACH,
                           (boolean *)&call_orig_before_ps_attach,
                            sizeof(call_orig_before_ps_attach),
                            subs_id ) > 0 )
  {
    DS_3GPP_MSG1_HIGH("call_orig_before_ps_attach NV: 70332 is %d in EFS", 
                      call_orig_before_ps_attach);
  }
  else
  {
    DS_3GPP_MSG1_ERROR("Failed to read NV item to check call orig allowed before"
                       " ps attach NV: 70332 setting to default value: %d",
                         call_orig_before_ps_attach);
  }
  return call_orig_before_ps_attach;
}

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_ignore_inactivity_timer

DESCRIPTION
  This function returns true if ignore inactivity timer nv is set to true

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - if call orig is allowed before ps attach
  False - 
 
SIDE EFFECTS
  None.
===========================================================================*/

uint8  ds_3gpp_nv_manager_get_ignore_inactivity_timer
(
  sys_modem_as_id_e_type subs_id
)
{  
	uint8  ignore_inactivity_timer = FALSE;
  if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
    return ignore_inactivity_timer;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_IGNORE_INACTIVITY_TIMER_FOR_LAST_PDN, 
                           (unsigned char *)&ignore_inactivity_timer, 
                           sizeof(uint8),
                           subs_id) > 0)
  {
    DS_3GPP_MSG1_HIGH("ignore_inactivity_timer_for_last_pdn  NV:  is %d in EFS", 
                      ignore_inactivity_timer);
  }
  else
  {
    DS_3GPP_MSG1_HIGH("Falied to read ignore_inactivity_timer_for_last_pdn :settinf default  value %d ", 
                       ignore_inactivity_timer);
  }
  return ignore_inactivity_timer;
}


/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_always_connect_def_pdn_flag

DESCRIPTION
  This function reads the always connect default pdn NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_3gpp_nv_manager_read_always_connect_def_pdn_flag
(
   sys_modem_as_id_e_type subs_id
)
{
  uint8  always_conn_def_pdn_cfg_item = 0x0;
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  /*------------------------------------------------------------------------
    Initialize the global variable to hold this cfg item to FALSE
  ------------------------------------------------------------------------*/
  nv_per_subs_info->ds_eps_always_connect_def_pdn_flag = FALSE;

  /*------------------------------------------------------------------------
    Get the config item that is used to figure out if we need to establish
    the connection to default PDN (if it does not exist) after moving to L
    from G/W.

    This config item also determines if we honor disconnection of the non
    def PDN even if it is the last standing PDN.

    If this item is present and has a value set to 0x01 (uint8), both the
    aforementioned actions can be performed.
  ------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_ALWAYS_CONNECT_DEF_PDN,
                     (unsigned char *)&always_conn_def_pdn_cfg_item,
                     sizeof(uint8),
                     subs_id)
                                                                  == -1)
  {
    DS_LTE_MSG0_HIGH("Cannot get DEF PDN CONN cfg item. Setting val: FALSE");
  }
  else
  {
    DS_LTE_MSG1_HIGH("Got DEF PDN CONN cfg item. Val:0x%x",
                     always_conn_def_pdn_cfg_item);
    if(always_conn_def_pdn_cfg_item == 0x1)
    {
      nv_per_subs_info->ds_eps_always_connect_def_pdn_flag = TRUE;
    } /* if cfg item is set to 0x1*/
  } //if cfg item was read successfully
} 

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_remove_unused_pdn

DESCRIPTION
  This function reads the remove unused pdn NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/

void ds_3gpp_nv_manager_read_remove_unused_pdn
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

   if( ds_3gpp_cfg_get_subsid( DS_3GPP_REMOVE_UNUSED_PDN, 
                   (boolean *)&nv_per_subs_info->ds_eps_remove_unused_pdn, 
                   sizeof(uint8),
                   subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_REMOVE_UNUSED_PDN,"
                      "using default");
    nv_per_subs_info->ds_eps_remove_unused_pdn = FALSE;
  }

}
/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_allow_default_pdn_teardown

DESCRIPTION
  This function reads the always  default pdn tear down  NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/


void ds_3gpp_nv_manager_read_allow_default_pdn_teardown
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  uint8 eps_allow_def_pdn_teardown = 0x0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  nv_per_subs_info->ds_eps_allow_def_pdn_teardown = FALSE;

  if (ds_3gpp_cfg_get_subsid(DS_3GPP_ALLOW_DEF_LTE_PDN_TEARDOWN,
                     (unsigned char *)&eps_allow_def_pdn_teardown,
                     sizeof(uint8),
                     subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_ALLOW_DEF_PDN_TEARDOWN, "
                      "using default");
  }
  else
  {
    if(eps_allow_def_pdn_teardown == 0x1)
    {
      nv_per_subs_info->ds_eps_allow_def_pdn_teardown = TRUE;
    }
    else
    {
      nv_per_subs_info->ds_eps_allow_def_pdn_teardown = FALSE;
    }
  }

    
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_allow_infinite_throttling

DESCRIPTION
  This function reads the always  infinite throttling  NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_allow_infinite_throttling
(
   sys_modem_as_id_e_type subs_id
)
{
 	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }
  /*------------------------------------------------------------------------- 
    Read ALLOW_INFINITE_THROT value from NV, This would determine if we
    must perform the IT check in rej_ind_hdlr
  -------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_ALLOW_INFINITE_THROT,
                    (boolean *) &nv_per_subs_info->ds_eps_allow_infinite_throt,
                     sizeof(nv_per_subs_info->ds_eps_allow_infinite_throt),
                     subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item DS_3GPP_CFG_ALLOW_INFINITE_"
                      "THROT");
    nv_per_subs_info->ds_eps_allow_infinite_throt = FALSE;
  }

}



/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_switch_apn_on_reject

DESCRIPTION
  This function reads the apn switch on reject  NV item for each
  subscription
 

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/


void ds_3gpp_nv_manager_read_switch_apn_on_reject
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }



  if( ds_3gpp_cfg_get_subsid( DS_3GPP_CFG_SWITCH_APN_ON_REJECT, 
                       (boolean *)&nv_per_subs_info->ds_eps_switch_apn_on_reject, 
                       sizeof(boolean),
                       subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                      "DS_3GPP_CFG_SWITCH_APN_ON_REJECT," 
                      "using default as TRUE");
    nv_per_subs_info->ds_eps_switch_apn_on_reject = TRUE;
  }
}





/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_allow_def_pdn_teardown

DESCRIPTION
  This function informs whether default LTE PDN can be torn down based on the
   nv value per subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_nv_manager_get_allow_def_pdn_teardown
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info->ds_eps_allow_def_pdn_teardown;
  
  return ret_val;
}

/*===========================================================================
FUNCTION ds_3gpp_nv_manager_get_allow_infinite_throttling
DESCRIPTION
  This function informs whether infinite throttling is enabled based on the
   nv value per subscription

PARAMETERS
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/


boolean ds_3gpp_nv_manager_get_allow_infinite_throttling
(
   sys_modem_as_id_e_type    subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = TRUE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }

  ret_val = nv_per_subs_info->ds_eps_allow_infinite_throt;
  
  return ret_val;
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_switch_apn_on_reject
DESCRIPTION
  This function informs whether apn switch on reject is enabled based on the
   nv value per subscription

PARAMETERS
  subs_id subscription id
 
 
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_nv_manager_get_switch_apn_on_reject
(
   sys_modem_as_id_e_type    subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = TRUE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  ret_val = nv_per_subs_info->ds_eps_switch_apn_on_reject;
  
  return ret_val;
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_always_connect_def_pdn_flag
 
DESCRIPTION
  This function informs whether always connect on default pdn is enabled based on the
   nv value per subscription

PARAMETERS
  subs_id

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds_3gpp_nv_manager_get_always_connect_def_pdn_flag
(
   sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  ret_val = nv_per_subs_info->ds_eps_always_connect_def_pdn_flag;
  return ret_val;
}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_remove_unused_pdn

DESCRIPTION
  This function gets the value of NV 70334 REMOVE_UNUSED_PDN per subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - REMOVE_UNUSED_PDN =1.
  False - REMOVE_UNUSED_PDN =0.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_nv_manager_get_remove_unused_pdn
(
  sys_modem_as_id_e_type subs_id
)
{

  ds_3gpp_nv_per_subs_info_type* nv_per_subs_info;
  boolean ret_val = FALSE;

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return ret_val;
  }
  
  ret_val = nv_per_subs_info->ds_eps_remove_unused_pdn;
  return ret_val;

}

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_nv_lte_rel_version

DESCRIPTION
  This function returns  the LTE release version, read during nv manager
  initialzizing.

PARAMETERS
  void

DEPENDENCIES
  None.

RETURN VALUE
  lte_3gpp_release_ver_e enum type

SIDE EFFECTS
  None

===========================================================================*/
lte_3gpp_release_ver_e ds_3gpp_nv_manager_get_nv_lte_rel_version()
{
  return ds_3gpp_nv_lte_rel_version; 
}/* ds_3gpp_pdn_get_nv_lte_rel_version */

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_ENABLE_SEND_TRM_PRIORITY_NV

DESCRIPTION
  This function reads the ENABLE_SEND_TRM_PRIORITY NV(73562) item for each
  subscription

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE  
  None

SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_enable_send_trm_priority_nv()
{
  if( ds_3gpp_cfg_get( DS_3GPP_ENABLE_SEND_TRM_PRIORITY, 
                      (boolean *)&ds_eps_enable_send_trm_priority, 
                       sizeof(boolean)) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item "
                      "DS_3GPP_ENABLE_SEND_TRM_PRIORITY," 
                      "using default as FALSE");
    ds_eps_enable_send_trm_priority = FALSE;
  };
}

/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_ENABLE_SEND_TRM_PRIORITY

DESCRIPTION
  This function gets the value of NV 73562 ENABLE_SEND_TRM_PRIORITY per
  subscription

PARAMETERS 
  subs_id subscription id
 
DEPENDENCIES
  None.

RETURN VALUE
  True  - ENABLE_SEND_TRM_PRIORITY = 1.
  False - ENABLE_SEND_TRM_PRIORITY = 0.
 
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_3gpp_nv_manager_get_enable_send_trm_priority()
{
  DS_3GPP_MSG1_HIGH("ds_eps_enable_send_trm_priority : %d ", 
                     ds_eps_enable_send_trm_priority);
  return ds_eps_enable_send_trm_priority;
}

/*===========================================================================
FUNCTION DS_3GPP_NV_MANAGER_READ_LTE_CA_DISPLAY_THRESHOLD

DESCRIPTION
  This function reads LTE_CA_DISPLAY_THRESHOLD NV item for each
  subscription

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_lte_ca_display_threshold
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_nv_per_subs_info_type  *nv_per_subs_info = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    DS_3GPP_MSG1_ERROR("Pdn Cntxt Nv per subs info handle invalid for subs id",
                       subs_id);
    return;
  }

  if(ds_3gpp_cfg_get_subsid(DS_3GPP_LTE_CA_DISPLAY_THRESHOLD,
                           (void*)&(nv_per_subs_info->lte_ca_display_threshold),
                           sizeof(ds_eps_ca_display_threshold_s),
                           subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item LTE_CA_DISPLAY_THRESHOLD"
                       "Setting default values");
    memset( &(nv_per_subs_info->lte_ca_display_threshold),
            0,
            sizeof(ds_eps_ca_display_threshold_s) );
  }

  return;
}/* ds_3gpp_nv_manager_read_lte_ca_display_threshold */

/*===========================================================================
FUNCTION  DS_3GPP_NV_MANAGER_GET_LTE_CA_DISPLAY_THRESHOLD

DESCRIPTION
  This function returns the value of LTE_CA_DISPLAY_THRESHOLD configuration.

PARAMETERS
  subs_id  subscription 
    
DEPENDENCIES
  None.

RETURN VALUE
  ds_eps_ca_display_threshold_s*

SIDE EFFECTS
  None

===========================================================================*/
ds_eps_ca_display_threshold_s* ds_3gpp_nv_manager_get_lte_ca_display_threshold
(
  sys_modem_as_id_e_type  subs_id
)
{
  ds_3gpp_nv_per_subs_info_type  *nv_per_subs_info = NULL;
  ds_eps_ca_display_threshold_s  *ca_display_threshold_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);
  if (ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    ca_display_threshold_ptr = &(nv_per_subs_info->lte_ca_display_threshold);
  }

  return ca_display_threshold_ptr;
}/* ds_3gpp_nv_manager_get_lte_ca_display_threshold */

#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_read_efnas_config

DESCRIPTION
  This function reads the NAS configuration NV

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
void ds_3gpp_nv_manager_read_efnas_config
(
   sys_modem_as_id_e_type subs_id
)
{
 	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    return;
  }

  /*------------------------------------------------------------------------ 
    Allocate memory for nasconfig from the heap
   ------------------------------------------------------------------------*/
  if (nv_per_subs_info->efsnas_config == NULL)
  {
    nv_per_subs_info->efsnas_config = (byte *)modem_mem_alloc(
                                                (unsigned int)DS_3GPP_EFNAS_CONFIG_SIZE,
                                                MODEM_MEM_CLIENT_DATA);

    if (nv_per_subs_info->efsnas_config == NULL )
    {
      DS_3GPP_MSG0_ERROR("Heap alloc for efsnasconfig failed");
      return;
    }
  }

  /*------------------------------------------------------------------------- 
    Read EFNAS value from NAS
  -------------------------------------------------------------------------*/
  if(ds_3gpp_cfg_get_subsid(DS_3GPP_CFG_EFNAS_CONFIG_PATH,
                    (byte *)nv_per_subs_info->efsnas_config,
                     DS_3GPP_EFNAS_CONFIG_SIZE,
                     subs_id) == -1)
  {
    DS_3GPP_MSG0_ERROR("Cannot read EFS Config item EFNAS_CONFIG");
    modem_mem_free(nv_per_subs_info->efsnas_config,MODEM_MEM_CLIENT_DATA);
    nv_per_subs_info->efsnas_config = NULL ;
  }

  return;
}/*ds_3gpp_nv_manager_read_efnas_config*/

/*===========================================================================
FUNCTION  ds_3gpp_nv_manager_get_efnas_config

DESCRIPTION
  This function gets the NAS configuration

PARAMETERS
  subs_id subscription id

DEPENDENCIES
  None.

RETURN VALUE  


SIDE EFFECTS

===========================================================================*/
byte  *ds_3gpp_nv_manager_get_efnas_config
(
   sys_modem_as_id_e_type subs_id
)
{
 	ds_3gpp_nv_per_subs_info_type* nv_per_subs_info = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nv_per_subs_info = ds_3gpp_nv_manager_get_nv_subs_info(subs_id);

  if (!ds_3gpp_nv_manager_validate_nv_subs_info(nv_per_subs_info))
  {
    DS_3GPP_MSG1_ERROR("Nv per subs info handle is invalid for subs id"
                       ,subs_id);
    return NULL;
  }

  return nv_per_subs_info->efsnas_config;

}/*ds_3gpp_nv_manager_get_efnas_config*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
