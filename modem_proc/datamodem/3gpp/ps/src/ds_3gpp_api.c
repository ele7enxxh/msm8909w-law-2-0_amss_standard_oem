/*!
  @file
  ds_api.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_api.c#1 $

when         who     what, where, why
--------     ---     -------------------------------------------------------- 
01/06/12     vb      Function call to dsBearerCntxt_is_ps_data_available() 

===========================================================================*/

#include "dsumtsps.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_bearer_context.h"
#include "dstask_v.h"
#include "ds3gpp_api.h"
#include "dsutil.h"
#include "ds3gcfgmgr.h"
#include "ds_3gpp_nv_manager.h"

#ifdef FEATURE_DATA_LTE
#include "ds_eps_pdn_throttle_sm.h"
#endif /* FEATURE_DATA_LTE */

#ifdef FEATURE_DATA_LTE
static ds_3gpp_bearer_err_info_type ds_3gpp_beaer_err_info = {0};
#endif /* FEATURE_DATA_LTE */

/*===========================================================================
FUNCTION DS_3GPP_NOTIFY_IND

DESCRIPTION
   This is a generic function, used to notify DS module.

PARAMETERS
 indication type and 
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void ds_3gpp_notify_ind(ds_3gpp_notify_ind_e_type indication, void *notify_ind_data_ptr)
{
  ds_cmd_type 		        *cmd_ptr = NULL; 
  sys_modem_as_id_e_type        *data_ptr = NULL;
  ds_3gpp_rau_event_s_type*      ind_ptr=
  	                  ((ds_3gpp_rau_event_s_type*)notify_ind_data_ptr);
/*--------------------------------------------------------------------------------*/
  if(ind_ptr == NULL)
  {
    DS_3GPP_MSG0_ERROR("ds_3gpp_notify_ind: recieved NULL ptr");
    return;
  }

   DS_3GPP_MSG3_HIGH("ds_3gpp_notify_ind: ind type:%d evt:%d rau type:%d", 
   	                    indication, ind_ptr->event, ind_ptr->rau_type);

  if(indication == DS_3GPP_RAU_IND)
  {
    switch(ind_ptr->event)
    {
     case DS_3GPP_RAU_COMPLETE_EVT:

	if(ind_ptr->rau_type != DS_3GPP_RA_CAHNGE_TYPE)
        {
          return;
	}
	if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type))) == NULL )
        {
          DS_3GPP_MSG0_ERROR("DS3GPP cant get cmd buf from DS task");
          return;
        }

        cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_RAU_COMPLTE_CMD;
        data_ptr = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;
        *data_ptr = (sys_modem_as_id_e_type)ind_ptr->subs_id;
        ds_put_cmd(cmd_ptr);
       break;
     default:
       break;			
  }
  }
}/*ds3gpp_notify_ind*/
/*===========================================================================
FUNCTION DS3GPP_IS_PLMN_BLOCKED_FORCE_LTE_PREF

DESCRIPTION
   This is called by lower layers during powere off to see if any PLMN is blocked.
   lower layer will set their next pref sys based on the PLMN blocked status

PARAMETERS
  subs_id 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If any PLMN is blocked
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_plmn_blocked_force_lte_pref
(
  sys_modem_as_id_e_type	  subs_id
)
{
  boolean ret_val = FALSE;
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_LTE
  ret_val = ds_eps_pdn_throt_is_any_lte_plmn_blocked(subs_id);
  DS_3GPP_MSG2_HIGH("ds_3gpp_plmn_unblock_force_lte_perf:%d, on subs_id: %d", 
                    ret_val, subs_id);
#endif /* FEATURE_DATA_LTE */

  return ret_val;
}/*ds3gpp_is_plmn_blocked_force_lte_pref*/

/*===========================================================================
FUNCTION DSUMTSPS_IS_PS_DATA_AVAILABLE

DESCRIPTION
   This function returns TRUE if data is available in any of the uplink
   buffers, otherwise, it returns FALSE.
   The function is accessible at the SU api level.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data is available
  FALSE - If Data is not available

SIDE EFFECTS
  None.

===========================================================================*/

boolean ds3gpp_is_ps_data_available(void)
{
  sys_modem_as_id_e_type subs_id = ds3g_get_ps_subs_id(); // Vishal: Temporary
  /*-----------------------------------------------------------------------*/

  return ds3gpp_per_subs_is_ps_data_available(subs_id);
}

boolean ds3gpp_per_subs_is_ps_data_available
(
  sys_modem_as_id_e_type subs_id
)
{  
  boolean ps_data_available;
  /*-----------------------------------------------------------------------*/

  ps_data_available = dsBearerCntxt_is_ps_data_available(subs_id);

  DS_3GPP_MSG1_MED("dsumtsps_is_ps_data_available returns %s",
				   (ps_data_available)?"TRUE":"FALSE");

  return ps_data_available;

}

/*===========================================================================
FUNCTION DS3GPP_IS_UE_INIT_QOS_SUPPORTED

DESCRIPTION
   This function returns TRUE if umts UE initiated QOS support
   is available, otherwise, it returns FALSE.

PARAMETERS
  None.
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If UE initiated UMTS QOS support is available
  FALSE - If UE initiated UMTS QOS support is not available

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_is_ue_init_qos_supported(void)
{
  boolean umts_qos_available = FALSE;
  
#if defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS)
  nv_stat_enum_type   status;
  char item_file_path[] = "/nv/item_files/modem/data/3gpp/umtsps/qos_support";
      
  status = ds3gcfgmgr_read_efs_nv(
              item_file_path,
              (void *) &umts_qos_available,
              sizeof(umts_qos_available)
           );
  
  if (status != NV_DONE_S)
  {
    umts_qos_available = FALSE;
  }
#endif /* defined (FEATURE_DATA_WCDMA_PS) || defined (FEATURE_GSM_GPRS) */

  DS_3GPP_MSG1_HIGH("ds3gpp_is_ue_init_qos_supported returns %d",
                    (umts_qos_available)?1:0);
  return umts_qos_available;
}/* ds3gpp_is_ue_init_qos_supported */

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PCH_STATE_SUB

DESCRIPTION
  This function is used by RRC in WCDMA and TDSCDMA mode to indicate when
  UTRAN moves UE to PCH states: CELL_PCH and URA_PCH. Same needs to be
  propagated to applications. It will be useful for saving power.

PARAMETERS
  subs_id   - Subscription ID
  sys_mode  - system network mode, PCH states are applicable only in case of
             TDSCDMA and WCDMA.
  pch_event - In or out of PCH state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_update_pch_state_sub
(
  sys_modem_as_id_e_type    subs_id,
  sys_sys_mode_e_type sys_mode,
  ds_3gpp_pch_event_e_type pch_event
)
{
  ds_cmd_type                        *cmd_ptr = NULL;
  ds_3gpp_rrc_pch_ind_proc_cmd_type  *data_ptr = NULL;

  DS_3GPP_MSG2_HIGH("PCH indication %d in sys mode %d", pch_event, sys_mode);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_rrc_pch_ind_proc_cmd_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id        = DS_CMD_3GPP_RRC_PCH_IND_PROC;
    data_ptr = (ds_3gpp_rrc_pch_ind_proc_cmd_type*)cmd_ptr->cmd_payload_ptr;

    data_ptr->pch_event = pch_event;
    data_ptr->call_mode = sys_mode;
    data_ptr->subs_id = subs_id;
    ds_put_cmd(cmd_ptr);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_UPDATE_PCH_STATE

DESCRIPTION
  This function is used by RRC in WCDMA and TDSCDMA mode to indicate when
  UTRAN moves UE to PCH states: CELL_PCH and URA_PCH. Same needs to be
  propagated to applications. It will be useful for saving power.

PARAMETERS
  sys_mode  - system network mode, PCH states are applicable only in case of
             TDSCDMA and WCDMA.
  pch_event - In or out of PCH state.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_update_pch_state
(
  sys_sys_mode_e_type sys_mode,
  ds_3gpp_pch_event_e_type pch_event
)
{
  sys_modem_as_id_e_type     subs_id = ds3g_get_ps_subs_id();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ds_3gpp_update_pch_state_sub(subs_id,sys_mode, pch_event);
}

/*===========================================================================
FUNCTION DS_TDSCDMA_CONFIGURE_UL_DATA_RATE

DESCRIPTION
  This function is used by RRC in TDSCDMA mode to indicate the updated
  data rate on a particular rab_id/nsapi changes. Based on which
  wm levels are adjusted on that bearer

PARAMETERS
  rab_id  - Identifier used to uniquely identify a data bearer
  ul_data_rate - Updated data rate of the bearer
  subs_id      - Subscription on which this call is made

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void ds_tdscdma_configure_ul_data_rate
(
   uint32 rab_id,  
   uint32  ul_data_rate,
   sys_modem_as_id_e_type subs_id
)
{
  
  ds_cmd_type               *cmd_ptr = NULL;
  ds_3gpp_ul_rab_info_type  *ul_rab_info_ptr = NULL;

  DS_3GPP_MSG3_HIGH("Configuring Ul rab id: %d data rate: %d subs_id %d ", rab_id, ul_data_rate, subs_id);

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_ul_rab_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id =	DS_CMD_3GPP_TD_CONFIG_UL_DATA_RATE;
    ul_rab_info_ptr = (ds_3gpp_ul_rab_info_type*)cmd_ptr->cmd_payload_ptr;
    ul_rab_info_ptr->rab_id = rab_id;
    ul_rab_info_ptr->ul_data_rate = ul_data_rate;
    ul_rab_info_ptr->subs_id = subs_id;
    ds_put_cmd(cmd_ptr);
  }
}

/*===========================================================================
FUNCTION DS_3GPP_COMPARE_PDP_ADDRESS

DESCRIPTION
  This function is used by NAS to compare the pdp_address when network
  sends MODIFY_REQUEST with pdp_address included. DS will compare the
  pdp_address passed by NAS against the pdp_address which is configured on
  V4/V6 interfaces.

PARAMETERS
  identifier   - Identifier used to uniquely identify a PDP/PDN Context.
                 (For W/G/TD it would be nsapi, For LTE it would be eps_bid)
  sys_mode     - Current sys_mode.
  subs_id      - Subscription on which the PDP/PDN is active.
  *pdp_address - Structure containing the pdp_address and pdp_type_num 
                 given by network, which would be used for comparison.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -  if PDP address sent by network is same as the one configured.
  FALSE - if PDP address sent by network is different than the one
          configured.

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_compare_pdp_address
(
  uint8 identifier,
  sys_sys_mode_e_type sys_mode,
  sys_modem_as_id_e_type subs_id,
  pdp_address_T *pdp_address
)
{
  boolean ret_val = FALSE;
  if (NULL == pdp_address)
  {
    DS_3GPP_MSG3_ERROR("Invalid pdp_address ptr passed", 0,0,0);
	return FALSE;
  }

  ret_val = ds_3gpp_pdn_cntxt_compare_pdp_address(identifier, 
  	                                           sys_mode, 
  	                                           subs_id, 
  	                                           pdp_address);
  DS_3GPP_MSG3_HIGH("PDP addr compare returning: %d", ret_val, 0, 0);
  return ret_val;
}

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_multi_pdn_same_apn

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

boolean ds3gpp_per_subs_get_config_multi_pdn_same_apn
(
  sys_modem_as_id_e_type subs_id
)
{
  return ds_3gpp_nv_manager_get_multi_pdn_same_apn(subs_id);
}

/*===========================================================================
FUNCTION ds3gpp_per_subs_get_config_umts_nw_init_qos_support

DESCRIPTION
  This function gets the umts_nw_init_qos_support NV item: 70331
  modem/data/3gpp/umts_nw_initiated_qos_support

PARAMETERS
  None
  
DEPENDENCIES
  None.

RETURN VALUE  
  Boolean Flag indicating whether UMTS NW Init QoS is enabled or not

SIDE EFFECTS
  None

===========================================================================*/
boolean ds3gpp_per_subs_get_config_umts_nw_init_qos_support
(
  sys_modem_as_id_e_type subs_id
)
{
  if (ds_3gpp_nv_manager_get_umts_nw_init_qos_support_nv() == DS_3GPP_NV_ENABLED)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#ifdef FEATURE_DATA_LTE

/*===========================================================================
FUNCTION DS_3GPP_GET_BEARER_ERR_INFO_PTR

DESCRIPTION
  This function returns pointer to to bearer error cache variable

PARAMETERS
None
  
DEPENDENCIES
  None.

RETURN VALUE
  Cache Pointer
  
SIDE EFFECTS
  None.
  
===========================================================================*/
ds_3gpp_bearer_err_info_type* ds_3gpp_get_bearer_err_info_ptr (void)
{
   return &ds_3gpp_beaer_err_info;
}

/*===========================================================================
FUNCTION DS_3GPP_GET_BEARER_ERR_THRESHHOLD_CNT

DESCRIPTION
  This function returns threshhold count value for malformed packets 
   cached at DS 

PARAMETERS
None
  
DEPENDENCIES
  None.

RETURN VALUE
  Threshhold packet count
  
SIDE EFFECTS
  None.
  
===========================================================================*/
uint8 ds_3gpp_get_bearer_err_threshhold_cnt (void)
{
  uint8    thresh_count = 0; 
  /* -----------------------------------------------------*/
  thresh_count = ds_3gpp_beaer_err_info.threshhold_pkt_cnt;

  DS_3GPP_MSG1_HIGH("ds_3gpp_get_bearer_err_threshhold_cnt(): thresh_count: %d", 
                                                                  thresh_count);
  return thresh_count;
}

/*===========================================================================
FUNCTION DS_3GPP_REG_PDCP_BEARER_ERR_CB

DESCRIPTION
  This function is exposed to pdcp to register a callback function to be 
  triggered when malformed packet count on particular bearer exceeds 
  threshhold limit.

PARAMETERS
  pdcp_cb_ptr - callback ptr paseed from PDCP
  threshhold count- malformed packet limit
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean ds_3gpp_reg_pdcp_bearer_err_cb 
( 
  ds_3gpp_bearer_err_ind_cb_type pdcp_cb_fptr, 
  uint8   threshhold_pkt_count 
)
{ 
  ds_3gpp_bearer_err_info_type* bearer_err_info_p = NULL;
  boolean                       ret_val = FALSE;
 /*-----------------------------------------------------------------*/
  DS_3GPP_MSG2_HIGH("ds_3gpp_reg_pdcp_bearer_err_cb(): cb_ptr: 0x%x, threshhold: %d", 
                                                 pdcp_cb_fptr, threshhold_pkt_count);
 
  bearer_err_info_p = ds_3gpp_get_bearer_err_info_ptr();
 
  if ((bearer_err_info_p != NULL) && (threshhold_pkt_count != 0))
  {
    bearer_err_info_p->pdpc_cb_fptr = pdcp_cb_fptr;
    bearer_err_info_p->threshhold_pkt_cnt = threshhold_pkt_count;
    
    ret_val = TRUE;
  }
  return ret_val;
}

#endif /* FEATURE_DATA_LTE */


