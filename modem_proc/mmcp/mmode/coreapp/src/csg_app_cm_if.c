#include "csg_app_cm_if.h"
#include "ui_base_msgr_msg.h"
#include "msgr_types.h"
#include "msgr_rex.h"

static csg_app_cm_if_ph_evt_cb_s csg_app_ph_evt_global;
static csg_app_cm_if_ss_evt_cb_s csg_app_ss_evt_global;

extern void ui_base_timer_reset(uint16 interval);

extern int ui_base_get_cm_client_id(void);

ui_base_csg_config_global_s_type ui_base_csg_config_global;

/*===========================================================================
  FUNCTION csg_app_cm_init()

  DESCRIPTION
    This function attempts to register to CM Call/SS/Phone events.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void csg_app_cm_if_init()
{

  uint8 i=0;

  const struct csg_app_cm_if_ph_event_s  ph_event_list[] = { 
    { CM_PH_EVENT_INFO, CM_PH_EVENT_INFO },
    { CM_PH_EVENT_OPRT_MODE, CM_PH_EVENT_OPRT_MODE },
    {CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,CM_PH_EVENT_AVAILABLE_NETWORKS_CONF}
  };

  const struct csg_app_cm_if_cmss_event_s ss_event_list[] = { 
      { CM_SS_EVENT_SRV_CHANGED, CM_SS_EVENT_SRV_CHANGED }
  };

  for ( i = 0; i < CSG_APP_CM_IF_ARR_SIZE( ph_event_list ); i++ )
  {
    if ( cm_client_ph_reg( ui_base_get_cm_client_id(), 
                             csg_app_cm_if_ph_evt_cb, 
                             CM_CLIENT_EVENT_REG,
                             ph_event_list[ i ].beg, 
                             ph_event_list[ i ].end, 
                             NULL 
                           ) != CM_CLIENT_OK ) 
    {
      TUI_MSG_HIGH_2("Fail to register CM Ph event :  %d->%d", ph_event_list[ i ].beg, ph_event_list[ i ].end);
    }
  }

  for ( i = 0; i < CSG_APP_CM_IF_ARR_SIZE( ss_event_list ); i++ )
  {
    if ( cm_mm_client_ss_reg_msim( ui_base_get_cm_client_id(), 
                             csg_app_cm_if_ss_evt_cb, 
                             CM_CLIENT_EVENT_REG,
                             ss_event_list[ i ].beg, 
                             ss_event_list[ i ].end, 
                             NULL,
                             SYS_MODEM_AS_ID_1_MASK|SYS_MODEM_AS_ID_2_MASK
                           ) != CM_CLIENT_OK ) 
    {
      TUI_MSG_HIGH_2("Fail to register CM SS event :  %d->%d", ss_event_list[ i ].beg, ss_event_list[ i ].end);
    }
  }

  if(cm_client_act(ui_base_get_cm_client_id())!= CM_CLIENT_OK )
  {
    TUI_MSG_HIGH("cm_client_act failed in CSG APP");
  }

}

void csg_app_cm_send_select_config_update_ind(void)
{
  ui_base_msgr_msg_csg_select_config_cmd_msg_type select_config;
  memset(&select_config,0,sizeof(ui_base_msgr_msg_csg_select_config_cmd_msg_type));
  //Power up scenario where we read the EFS for first time.
  select_config.csg_sel.asubs_id = ui_base_csg_config_global.csg_search_select_config.asubs_id;
  select_config.csg_sel.select_config = UI_BASE_SYS_CSG_SELECTION_CONFIG_1;
  TUI_MSG_HIGH_1("Send config update indication to CM during power up = %d", select_config.csg_sel.select_config);
  csg_app_cm_send_config_update_ind(&select_config);
}

void csg_app_cm_trigger_search(boolean is_app_triggered, boolean send_config_update, boolean is_immediate_search)
{
  cm_network_list_type_e_type network_list_type=CM_NETWORK_LIST_CSG_SELECTION_APP;
  cm_mode_pref_e_type mode_pref = CM_MODE_PREF_NO_CHANGE;
  sys_modem_as_id_e_type asubs_id = SYS_MODEM_AS_ID_1;
  cm_band_pref_e_type         umts_band_pref     = CM_BAND_PREF_ANY,
                              lte_band_pref = CM_BAND_PREF_LTE_ANY,
                              tds_band_pref = CM_BAND_PREF_TDS_ANY;
  uint32 csg_id=0;
  ui_base_msgr_msg_csg_select_config_cmd_msg_type select_config;
  size_t ss_info_max = sizeof(csg_app_ss_evt_global.ss_info)/sizeof(csg_app_ss_evt_global.ss_info[0]);

  memset(&select_config,0,sizeof(ui_base_msgr_msg_csg_select_config_cmd_msg_type));

  TUI_MSG_HIGH_2("Inside csg_app_cm_trigger_search client_id= %d,oprt_mode=%d", ui_base_get_cm_client_id(), csg_app_ph_evt_global.ph_info.oprt_mode);

  if(ui_base_csg_config_global.is_efs_read && ui_base_csg_config_global.timer.is_valid)
  {
    mode_pref = ui_base_csg_config_global.csg_search_select_config.network_type;
    asubs_id = (ui_base_csg_config_global.csg_search_select_config.asubs_id == (uint8)-1)?SYS_MODEM_AS_ID_1:ui_base_csg_config_global.csg_search_select_config.asubs_id;
    umts_band_pref = ui_base_csg_config_global.csg_search_select_config.umts_band_pref;
    lte_band_pref = ui_base_csg_config_global.csg_search_select_config.lte_band_pref;
    TUI_MSG_HIGH_2("original interval=%d,search_timer", ui_base_csg_config_global.timer.interval, ui_base_csg_config_global.csg_search_select_config.search_timer);
    ui_base_csg_config_global.timer.interval = (timetick_type)ui_base_csg_config_global.csg_search_select_config.search_timer;
  }
  else if(TRUE == ui_base_read_csg_config())
  {
    mode_pref = ui_base_csg_config_global.csg_search_select_config.network_type;
    asubs_id = (ui_base_csg_config_global.csg_search_select_config.asubs_id == (uint8)-1)?SYS_MODEM_AS_ID_1:ui_base_csg_config_global.csg_search_select_config.asubs_id;
    umts_band_pref = ui_base_csg_config_global.csg_search_select_config.umts_band_pref;
    lte_band_pref = ui_base_csg_config_global.csg_search_select_config.lte_band_pref;
  }
  else
  {
    TUI_MSG_ERROR_1("EFS not set/read properly during search type= %d", is_app_triggered);
    return;
  }

  if(asubs_id >= ss_info_max)
  {
    TUI_MSG_HIGH_2("Index out of bounds for csg_app_ss_evt_global.ss_info. asubs_id: %d -> %d", asubs_id, SYS_MODEM_AS_ID_1);
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  //For now only check Stack Zero no SXLTE support
  if(csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].sys_mode == SYS_SYS_MODE_WCDMA)
  {
    csg_id = csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].rat_mode_info.wcdma_mode_info.csg_info.csg_id;
  }
  else if(csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].sys_mode == SYS_SYS_MODE_GSM)
  {
    csg_id = csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].rat_mode_info.gsm_mode_info.csg_info.csg_id;
  }
  else if(csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].sys_mode == SYS_SYS_MODE_LTE)
  {
    csg_id = csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].rat_mode_info.lte_mode_info.csg_info.csg_id;
  }

  TUI_MSG_HIGH_3("asubs_id=%d, sys_mode=%d, csg_id = %d", asubs_id, csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].sys_mode, csg_id);

  if(send_config_update == TRUE)
  {
    select_config.csg_sel.asubs_id = asubs_id;
    select_config.csg_sel.select_config = UI_BASE_SYS_CSG_SELECTION_CONFIG_1;
    csg_app_cm_send_config_update_ind(&select_config);
  }

  if( (csg_id == SYS_CSG_ID_INVALID || csg_id == 0 )
      && ( csg_app_ph_evt_global.ph_info.oprt_mode != SYS_OPRT_MODE_LPM && 
           csg_app_ph_evt_global.ph_info.oprt_mode != SYS_OPRT_MODE_FTM 
          ) )
  {

    if(is_app_triggered == FALSE)
    {
      network_list_type = CM_NETWORK_LIST_CSG_USER;
    }

    if((ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0) || is_immediate_search == TRUE)
    {
      TUI_MSG_HIGH_4("Trigger interval %d in minutes, mode=%d, list type %d,network_type=%d", 
                     ui_base_csg_config_global.csg_search_select_config.search_timer, ui_base_csg_config_global.csg_search_select_config.search_mode,
                     ui_base_csg_config_global.csg_search_select_config.list_type, ui_base_csg_config_global.csg_search_select_config.network_type);
      
      TUI_MSG_HIGH_5("Trigger band_pref %ld,select_config = %d, lte_band_pref %ld,mode_pref = %d,nw_list_type=%d", 
                     ui_base_csg_config_global.csg_search_select_config.umts_band_pref, ui_base_csg_config_global.csg_search_select_config.select_config,
                     ui_base_csg_config_global.csg_search_select_config.lte_band_pref, mode_pref, network_list_type);
      
      TUI_MSG_HIGH_3("Roaming status= %d,srv_status=%d,timer=%d", csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].roam_status, 
                     csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].srv_status, ui_base_csg_config_global.timer.interval);
      
      if(ui_base_csg_config_global.csg_search_select_config.search_mode == 1)
      {

        cm_ph_cmd_get_networks_per_subs_extn(csg_app_cm_ph_cmd_cb,NULL,ui_base_get_cm_client_id(), network_list_type, mode_pref, asubs_id, umts_band_pref, lte_band_pref, tds_band_pref);
        if(ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0)
        {
          ui_base_timer_reset(ui_base_csg_config_global.timer.interval);
        }
      }
      else if(ui_base_csg_config_global.csg_search_select_config.search_mode == 0 && (csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].roam_status == SYS_ROAM_STATUS_OFF ||
              csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].roam_status == SYS_ROAM_STATUS_UNKNOWN) )
      {
        cm_ph_cmd_get_networks_per_subs_extn(csg_app_cm_ph_cmd_cb,NULL,ui_base_get_cm_client_id(), network_list_type, mode_pref, asubs_id, umts_band_pref, lte_band_pref, tds_band_pref);
        if(ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0)
        {
          ui_base_timer_reset(ui_base_csg_config_global.timer.interval);
        }
      }
      else
      {
        TUI_MSG_HIGH("Skipping search ....as search mode and roam status doesn't match");
        if(ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0)
        {
          ui_base_timer_reset(ui_base_csg_config_global.timer.interval);
        }
      }
    }
  }
  else if (csg_app_ph_evt_global.ph_info.oprt_mode == SYS_OPRT_MODE_LPM || csg_app_ph_evt_global.ph_info.oprt_mode == SYS_OPRT_MODE_FTM)
  {
    TUI_MSG_HIGH_1("In LPM, so ignoring search and resetting timer, CSG Main srv_status %d", 
                   csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].srv_status );
    if(ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0)
    {
      ui_base_timer_reset(ui_base_csg_config_global.timer.interval);
    }
  }
  else
  {
    TUI_MSG_HIGH_1("Already on CSG Femto cell so ignoring search and resetting timer, CSG Main srv_status %d", 
                    csg_app_ss_evt_global.ss_info[asubs_id].stack_info[0].srv_status);
    if(ui_base_csg_config_global.timer.is_valid && ui_base_csg_config_global.timer.interval > 0)
    {
      ui_base_timer_reset(ui_base_csg_config_global.timer.interval);
    }
  }
  
}

/*===========================================================================
  FUNCTION csg_app_cm_if_ph_evt_cb()

  DESCRIPTION
    CM PH event call back function.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_cm_if_ph_evt_cb (cm_ph_event_e_type ph_event, const cm_ph_info_s_type *ph_info_ptr)
{

  TUI_MSG_HIGH_5("Inside csg_app_cm_if_ph_evt_cb():evt=%d,subs_id=%d,oprt_mode=%d, mode_cap=%d,band_cap=%ld", 
                  ph_event, ph_info_ptr->asubs_id, ph_info_ptr->oprt_mode, ph_info_ptr->mode_capability, ph_info_ptr->band_capability);
  
  csg_app_ph_evt_global.event = ph_event;
  memscpy( &csg_app_ph_evt_global.ph_info,sizeof(cm_ph_info_s_type), ph_info_ptr, sizeof(cm_ph_info_s_type));

  if (ph_event == CM_PH_EVENT_AVAILABLE_NETWORKS_CONF)
  {
    TUI_MSG_HIGH_2("TUI PH event network_list_type=%d, conf=%d", ph_info_ptr->network_list_type, ph_info_ptr->available_networks_list_cnf);
    if( ph_info_ptr->available_networks_list_cnf != SYS_PLMN_LIST_SUCCESS && 
        (ph_info_ptr->network_list_type == CM_NETWORK_LIST_CSG_USER || 
         ph_info_ptr->network_list_type == CM_NETWORK_LIST_CSG_SELECTION_APP))
    {
      ui_base_timer_reset(1);
      TUI_MSG_HIGH("Shortened search time as CSG search Abort/RLF happened in CSG APP");
    }
  }
}

/*===========================================================================
  FUNCTION csg_app_cm_if_ss_evt_cb()

  DESCRIPTION
    CM SS event call back function.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_cm_if_ss_evt_cb (  cm_ss_event_e_type ss_event,  const cm_mm_msim_ss_info_s_type *ss_info_ptr)
{
  sys_modem_as_id_e_type asid=SYS_MODEM_AS_ID_1;
  sys_csg_id_type              csg_id = SYS_CSG_ID_INVALID;
  
  TUI_MSG_HIGH_2("Inside csg_app_cm_if_ss_evt_cb():no_of_stacks=%d,subs_id=%d", ss_info_ptr->number_of_stacks, ss_info_ptr->asubs_id);
  if(ss_info_ptr->number_of_stacks <=2 && ss_info_ptr->asubs_id < SYS_MODEM_AS_ID_3 )
  {
    if(ss_info_ptr->asubs_id != SYS_MODEM_AS_ID_NONE)
    {
      asid = ss_info_ptr->asubs_id;
    }
    TUI_MSG_HIGH_5("TUI ss_event %d,sys_mode=%d,srv_status=%d ,change_fields=%ld,change_fields2=%ld", 
                   ss_event, ss_info_ptr->stack_info[0].sys_mode, ss_info_ptr->stack_info[0].srv_status, 
                   ss_info_ptr->stack_info[0].changed_fields, ss_info_ptr->stack_info[0].changed_fields2);
    //For now only check Stack Zero no SXLTE support
    if(ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_WCDMA)
    {
      csg_id = ss_info_ptr->stack_info[0].rat_mode_info.wcdma_mode_info.csg_info.csg_id;
    }
    else if(ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_GSM)
    {
      csg_id = ss_info_ptr->stack_info[0].rat_mode_info.gsm_mode_info.csg_info.csg_id;
    }
    else if(ss_info_ptr->stack_info[0].sys_mode == SYS_SYS_MODE_LTE)
    {
      csg_id = ss_info_ptr->stack_info[0].rat_mode_info.lte_mode_info.csg_info.csg_id;
    }

    TUI_MSG_HIGH_1("csg_id = %d", csg_id);
    
    csg_app_ss_evt_global.event = ss_event;
    memscpy( &csg_app_ss_evt_global.ss_info[asid],sizeof(cm_mm_msim_ss_info_s_type), ss_info_ptr, sizeof(cm_mm_msim_ss_info_s_type));
  }
}

/*===========================================================================
  FUNCTION csg_app_cm_ph_cmd_cb()

  DESCRIPTION
    CM PH cmd call back function.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_cm_ph_cmd_cb( void *p_data, cm_ph_cmd_e_type cmd, cm_ph_cmd_err_e_type ph_cmd_err)
{
   TUI_MSG_HIGH_2("CSG APP ph cmd %d err %d", cmd, ph_cmd_err);        

  switch (ph_cmd_err)
  {
    case CM_PH_CMD_ERR_NET_TYPE_P:
    case CM_PH_CMD_ERR_MODE_PREF_S:
    case CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P:
      break;

    case CM_PH_CMD_ERR_IN_USE_S:
      //retry csg search after 1-min as device in use.
      ui_base_timer_reset((uint16)CSG_SEARCH_SHORTER_TIMER);
      break;

    default:
      break;
  }

}

/*===========================================================================
  FUNCTION csg_app_cm_send_config_update_ind()

  DESCRIPTION
   Send config update indication to CM with selection params.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean csg_app_cm_send_config_update_ind(ui_base_msgr_msg_csg_select_config_cmd_msg_type *sel_config)
{
  errno_enum_type             msgr_err = E_SUCCESS;

  msgr_init_hdr( (msgr_hdr_struct_type*)sel_config, MSGR_MM_TUI, MM_TUI_CSG_SELECTION_CMD );

  msgr_err = msgr_send((msgr_hdr_struct_type*)sel_config, sizeof(ui_base_msgr_msg_csg_select_config_cmd_msg_type));

  TUI_MSG_HIGH_3("Sent config_update_ind with msgr_err= %d,sel_config=%d,as_id=%d", msgr_err, sel_config->csg_sel.select_config, sel_config->csg_sel.asubs_id);
  if(msgr_err == E_SUCCESS)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
  FUNCTION csg_app_search_select_config_handler()

  DESCRIPTION
   Send search and select configuration to CM and trigger CSG search based on timer.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_search_select_config_handler( mmode_qmi_nas_csg_search_select_config_cmd_msg_type *srch_sel_cfg )

{
  boolean status = FALSE;
  csg_search_select_config_s_type csg_config;

  memset(&csg_config,0x0,sizeof(csg_search_select_config_s_type));

  csg_config.asubs_id = srch_sel_cfg->csg_search_select_config_req.asubs_id;
  csg_config.list_type = srch_sel_cfg->csg_search_select_config_req.list_type;
  csg_config.lte_band_pref = srch_sel_cfg->csg_search_select_config_req.lte_band_pref;
  csg_config.network_type = srch_sel_cfg->csg_search_select_config_req.network_type;
  csg_config.search_mode = srch_sel_cfg->csg_search_select_config_req.search_mode;
  csg_config.search_timer = srch_sel_cfg->csg_search_select_config_req.search_timer;
  csg_config.select_config = srch_sel_cfg->csg_search_select_config_req.select_config;
  csg_config.umts_band_pref =  srch_sel_cfg->csg_search_select_config_req.umts_band_pref;
  
  status = ui_base_write_csg_config( &csg_config,sizeof(csg_search_select_config_s_type) );

  if (status == TRUE)
  {
    csg_app_cm_trigger_search(FALSE,TRUE,FALSE);
  }
}

/*===========================================================================
  FUNCTION csg_app_immediate_search_handler()

  DESCRIPTION
   Send Immediate CSG search to CM based on timer existing configuration.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_immediate_search_handler( qmi_mmode_msgr_csg_immd_search_select_cmd_msg_type *immed_srch_sel_req )
{
  csg_app_cm_trigger_search(FALSE,FALSE,TRUE);
}

/*===========================================================================
  FUNCTION csg_app_search_trigger_handler()

  DESCRIPTION
   Send Immediate CSG search request to CM based upon reciving trigger for Refresh indication from lower layers.
    
  PARAMETERS
    NONE

  RETURN VALUE
    NONE
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void csg_app_search_trigger_handler( cm_csg_srch_trigger_ind_s_type *search_trigger_req )
{
  csg_app_cm_trigger_search(TRUE,FALSE,FALSE);
}
