/*===========================================================================

                        QM_MSTATS.C 

DESCRIPTION

  QMI_MMODE source file for modem stats information module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qm_mstats.c#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
  

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/14/13    sm     Initial version
===========================================================================*/

#include "msg.h"
#include "qm_si.h"
#include "qm_util.h"
#include "qm_mstats.h"

//===========================================================================
// GLOBALS
//===========================================================================
struct cdma_mstats_s
{
  float cdma_rx0_agc;
  float cdma_rx1_agc;
};

struct hdr_mstats_s
{
  float hdr_rx0_agc;
  float hdr_rx1_agc;
};

struct gsm_mstats_s
{
  uint32                                 timing_advance_value; 
  sys_eng_mode_nmr_info_s_type           nmr_info; 
  sys_eng_mode_scell_info_s_type         scell_info;
  sys_eng_mode_scell_measurements_s_type scell_measurements;
  uint16                                 g_bcch;
  uint16                                 g_ta;
  sys_eng_mode_curr_l1_ts_s_type      current_timeslot;
  sys_eng_mode_scell_config_s_type    scell_config;
};

struct wcdma_mstats_s
{
  sys_wcdma_serving_cell_info_s_type        wcdma_serving_cell_info;
  sys_wcdma_serving_cell_power_info_s_type  wcdma_serving_cell_power_info;
  sys_wcdma_monitored_set_info_s_type       wcdma_monitored_set_info;
  sys_wcdma_rrc_state_e_type                wcdma_rrc_state;
  uint16                                    w_dl_bler;
  float                                     w_agc;
  float                                     w_txagc;
  sys_wcdma_active_set_reference_rl_info_s_type wcdma_active_set_reference_rl_info;
  sys_wcdma_active_set_info_s_type              wcdma_active_set_info;  
};

struct qm_mstats_s
{
  boolean                cdma_is_valid;
  struct cdma_mstats_s  *cdma_mstats;
  boolean                hdr_is_valid;
  struct hdr_mstats_s   *hdr_mstats;
  boolean                gsm_is_valid;
  struct gsm_mstats_s   *gsm_mstats;
  boolean                wcdma_is_valid;
  struct wcdma_mstats_s *wcdma_mstats;
};


struct qm_mstats_global_s 
{
  struct qm_mstats_s mstats[QM_SUBS_MAX];
};

static struct qm_mstats_global_s global;

//===========================================================================
// Function declarations
//===========================================================================

/*===========================================================================
  FUNCTION QM_MSTATS_INIT()

  DESCRIPTION
    This function initializes the modem stats information

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_init ( void )
{
  memset( &global, 0x00, sizeof(struct qm_mstats_global_s) );
}


/*===========================================================================
  FUNCTION QM_MSTATS_PROCESS_CM_MODEM_STATS_EVENT()

  DESCRIPTION
    This function processes incoming modem stats information.

  PARAMETERS
   p_mstats_info: pointer to modem stats information 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_process_cm_modem_stats_event ( sys_modem_stats_info_s_type *p_mstats_info )
{
  enum qm_subs_e subs_id;
  uint8 i=0;
  
  if ( p_mstats_info )
  {
    subs_id = qm_util_map_cm_asubs_to_qm_subs( p_mstats_info->asubs_id );

    if ( subs_id > QM_SUBS_MIN && subs_id < QM_SUBS_MAX )
    {
      switch ( p_mstats_info->type )
      {
        case SYS_MODEM_MODULE_CDMA:
          QM_MSG_HIGH_1("Received CDMA stats event with subs_id = %d", subs_id);
          if ( global.mstats[subs_id].cdma_mstats == NULL )
          {
            global.mstats[subs_id].cdma_mstats = (struct cdma_mstats_s*) modem_mem_alloc( sizeof( struct cdma_mstats_s ), MODEM_MEM_CLIENT_QMI_MMODE );
            memset( global.mstats[subs_id].cdma_mstats, 0x00, sizeof(struct cdma_mstats_s ) );
          }
  
          global.mstats[subs_id].cdma_mstats->cdma_rx0_agc = (float)p_mstats_info->prm.cdma_info.cdma_1x_rx0_agc;
          global.mstats[subs_id].cdma_mstats->cdma_rx1_agc = (float)p_mstats_info->prm.cdma_info.cdma_1x_rx1_agc;
          global.mstats[subs_id].cdma_is_valid = TRUE;
          break;
  
        case SYS_MODEM_MODULE_HDR:
          QM_MSG_HIGH_1("Received HDR stats event with subs_id = %d", subs_id);
          if ( global.mstats[subs_id].hdr_mstats == NULL )
          {
            global.mstats[subs_id].hdr_mstats = (struct hdr_mstats_s*) modem_mem_alloc( sizeof( struct hdr_mstats_s ), MODEM_MEM_CLIENT_QMI_MMODE );
            memset( global.mstats[subs_id].hdr_mstats, 0x00, sizeof(struct hdr_mstats_s ) );
          }
  
          global.mstats[subs_id].hdr_mstats->hdr_rx0_agc = (float)p_mstats_info->prm.hdr_info.cdma_evdo_rx0_agc;
          global.mstats[subs_id].hdr_mstats->hdr_rx1_agc = (float)p_mstats_info->prm.hdr_info.cdma_evdo_rx1_agc;
          global.mstats[subs_id].hdr_is_valid = TRUE;
          break;
  
        case SYS_MODEM_MODULE_GSM:
          QM_MSG_HIGH_2("Received GSM stats event with subs_id = %d, bitmask = %u",
                        subs_id, p_mstats_info->prm.gsm_info.changed_bitmask);
          if ( global.mstats[subs_id].gsm_mstats == NULL )
          {
            global.mstats[subs_id].gsm_mstats = (struct gsm_mstats_s*) modem_mem_alloc( sizeof( struct gsm_mstats_s ), MODEM_MEM_CLIENT_QMI_MMODE );
            memset( global.mstats[subs_id].gsm_mstats, 0x00, sizeof(struct gsm_mstats_s) );
          }
  
          if ( p_mstats_info->prm.gsm_info.changed_bitmask & CM_GERAN_ENG_MODE_NMR_INFO )
          {
            memscpy(&global.mstats[subs_id].gsm_mstats->nmr_info, sizeof(sys_eng_mode_nmr_info_s_type) , 
                         &p_mstats_info->prm.gsm_info.nmr_info, sizeof(sys_eng_mode_nmr_info_s_type) );
          }
          if ( p_mstats_info->prm.gsm_info.changed_bitmask & CM_GERAN_ENG_MODE_TIMING_ADVANCE )
          {
            memscpy(&global.mstats[subs_id].gsm_mstats->timing_advance_value, sizeof(uint32) , 
                         &p_mstats_info->prm.gsm_info.timing_advance_value, sizeof(uint32) );
          }
          if ( p_mstats_info->prm.gsm_info.changed_bitmask & CM_GERAN_ENG_MODE_SCELL_INFO )
          {
            memscpy(&global.mstats[subs_id].gsm_mstats->scell_info, sizeof(sys_eng_mode_scell_info_s_type),
                         &p_mstats_info->prm.gsm_info.scell_info, sizeof(sys_eng_mode_scell_info_s_type) );
          }
          if ( p_mstats_info->prm.gsm_info.changed_bitmask & CM_GERAN_ENG_MODE_SCELL_MEASUREMENTS )
          {
            memscpy( &global.mstats[subs_id].gsm_mstats->scell_measurements, sizeof(sys_eng_mode_scell_measurements_s_type), 
                           &p_mstats_info->prm.gsm_info.scell_measurements, sizeof(sys_eng_mode_scell_measurements_s_type) );
          }
          if ( p_mstats_info->prm.gsm_info.changed_bitmask & SYS_GERAN_ENG_MODE_CURRENT_L1_TIMESLOT )
          {
            global.mstats[subs_id].gsm_mstats->current_timeslot.timeslot = p_mstats_info->prm.gsm_info.current_timeslot.timeslot;
            QM_MSG_HIGH_1("Received GSM stats event with timeslot = %d", p_mstats_info->prm.gsm_info.current_timeslot.timeslot);
          }
          if( p_mstats_info->prm.gsm_info.changed_bitmask & SYS_GERAN_ENG_SCELL_CONFIG )
          {
            memscpy( &global.mstats[subs_id].gsm_mstats->scell_config,sizeof(sys_eng_mode_scell_config_s_type),
                     &p_mstats_info->prm.gsm_info.scell_config,sizeof(sys_eng_mode_scell_config_s_type));
            QM_MSG_HIGH_2("Received GSM stats scell config pbcch=%d,txpwr_cch=%d", p_mstats_info->prm.gsm_info.scell_config.pbcch_present, p_mstats_info->prm.gsm_info.scell_config.gprs_ms_txpwr_max_cch);
          }

          global.mstats[subs_id].gsm_mstats->g_bcch = p_mstats_info->prm.gsm_info.g_bcch;
          global.mstats[subs_id].gsm_mstats->g_ta = p_mstats_info->prm.gsm_info.g_ta;
  
          global.mstats[subs_id].gsm_is_valid = TRUE;
          break;
  
        case SYS_MODEM_MODULE_WCDMA:
          QM_MSG_HIGH_2("Received WCDMA stats event with subs_id = %d, bitmask = %u",
                        subs_id, p_mstats_info->prm.wcdma_info.changed_bitmask);
          if ( global.mstats[subs_id].wcdma_mstats == NULL )
          {
            global.mstats[subs_id].wcdma_mstats = (struct wcdma_mstats_s*) modem_mem_alloc( sizeof( struct wcdma_mstats_s ), MODEM_MEM_CLIENT_QMI_MMODE );
            memset( global.mstats[subs_id].wcdma_mstats, 0x00, sizeof(struct wcdma_mstats_s) );
          }
  
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & CM_WCDMA_MONITORED_SET_BIT_MASK )
          {
            memscpy( &global.mstats[subs_id].wcdma_mstats->wcdma_monitored_set_info, sizeof(sys_wcdma_monitored_set_info_s_type),
                          &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_monitored_set_info, sizeof(sys_wcdma_monitored_set_info_s_type) );
          }
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & CM_WCDMA_SERVING_CELL_INFO_BIT_MASK )
          {
            memscpy( &global.mstats[subs_id].wcdma_mstats->wcdma_serving_cell_info, sizeof(sys_wcdma_serving_cell_info_s_type),
                          &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_serving_cell_info, sizeof(sys_wcdma_serving_cell_info_s_type) );
          }
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & CM_WCDMA_SERVING_CELL_POWER_INFO_BIT_MASK )
          {
            memscpy( &global.mstats[subs_id].wcdma_mstats->wcdma_serving_cell_power_info, sizeof(sys_wcdma_serving_cell_power_info_s_type),
                          &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_serving_cell_power_info, sizeof(sys_wcdma_serving_cell_power_info_s_type) );
            QM_MSG_HIGH_1("received stats event with CELL_POWER_INFO_BIT_MASK,squal=%d", p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_serving_cell_power_info.squal);

          }
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & SYS_WCDMA_RRC_STATE )
          {
            memscpy(&global.mstats[subs_id].wcdma_mstats->wcdma_rrc_state, sizeof(sys_wcdma_rrc_state_e_type), 
                          &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_rrc_state, sizeof(sys_wcdma_rrc_state_e_type) );
          }
  
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & CM_WCDMA_ACTIVE_SET_REFERENCE_CELL_INFO_BIT_MASK )
          {
            memscpy( &global.mstats[subs_id].wcdma_mstats->wcdma_active_set_reference_rl_info, sizeof(sys_wcdma_active_set_reference_rl_info_s_type), &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_reference_rl_info, sizeof(sys_wcdma_active_set_reference_rl_info_s_type) );
          }
          
          if ( p_mstats_info->prm.wcdma_info.changed_bitmask & CM_WCDMA_ACTIVE_SET_INFO_BIT_MASK  )
          {
            memscpy( &global.mstats[subs_id].wcdma_mstats->wcdma_active_set_info, sizeof(sys_wcdma_active_set_info_s_type), &p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info, sizeof(sys_wcdma_active_set_info_s_type) );
            for(i=0;i<p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info.num_rls && i<SYS_WCDMA_MAX_ASET_CELLS ;i++)
            {
              QM_MSG_HIGH_4("sf=%d,phy_chan_type=%d,slot=%d,cellid=%d", 
                      p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info.sf, p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info.phy_chan_type,
                     p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info.slot_format, p_mstats_info->prm.wcdma_info.wcdma_event_info.wcdma_active_set_info.cell_id[i]);
            }
          }

          global.mstats[subs_id].wcdma_mstats->w_dl_bler = p_mstats_info->prm.wcdma_info.wcdma_event_info.gw_dl_bler;
          global.mstats[subs_id].wcdma_mstats->w_agc = (float)p_mstats_info->prm.wcdma_info.wcdma_event_info.w_agc;
          global.mstats[subs_id].wcdma_mstats->w_txagc = (float)p_mstats_info->prm.wcdma_info.wcdma_event_info.w_txagc;
  
          global.mstats[subs_id].wcdma_is_valid = TRUE;
          break;
  
        default: // we don't care
          break;
      }
    }
  }
  else
  {
    QM_MSG_ERROR("cm stats evt null pointer");
  }
}

/*===========================================================================
  FUNCTION QM_MSTATS_REPORT_NEW_INACTIVE_RATS()

  DESCRIPTION
    This function reports to modem stats module whether any RATs have newly gone out of
    service.  RATs listed in this mask have their modem stats information invalidated.

  PARAMETERS
    subs_id: subscription for which the RATs are inactive
    inactive_rat_mask: mask of newly inactive RATs for given subscription
 
  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_report_new_inactive_rats( enum qm_subs_e subs_id, rat_bitmask_type inactive_rat_mask )
{
  QM_MSG_HIGH_1("invalidate_inactive_rats rat mask %d", inactive_rat_mask);

  //CDMA
  if( inactive_rat_mask & QM_RAT_MASK_C )
  {
    global.mstats[subs_id].cdma_is_valid = FALSE;
  }
  //HDR
  if( inactive_rat_mask & QM_RAT_MASK_H )
  {
    global.mstats[subs_id].hdr_is_valid = FALSE;
  }
  //GSM
  if( inactive_rat_mask & QM_RAT_MASK_G )
  { 
    global.mstats[subs_id].gsm_is_valid = FALSE;
  }
  //WCDMA
  if( inactive_rat_mask & QM_RAT_MASK_W )
  { 
    global.mstats[subs_id].wcdma_is_valid = FALSE;
  }
}

/*===========================================================================
  FUNCTION QM_MSTATS_POPULATE_CELL_LOC_INFO()

  DESCRIPTION
    This function populates get_cell_location_info resp based on modem stats module
    information.

  PARAMETERS
    rsp_msg: pointer to GET_CELL_LOCATION_INFO response structure
    subs_id: subscription for which the response should be populated

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_mstats_populate_cell_loc_info ( nas_get_cell_location_info_resp_msg_v01 *rsp_msg, enum qm_subs_e subs_id )
{
  int i, j;

  if( qm_si_is_rat_active(subs_id, QM_RAT_1X) )
  {
    QM_MSG_HIGH_2("mstats:: subs_id: %d, cdma_is_valid:%d",
                  subs_id, global.mstats[subs_id].cdma_is_valid); 
    if( global.mstats[subs_id].cdma_is_valid )
    {
      struct cdma_mstats_s *p_cdma = global.mstats[subs_id].cdma_mstats;

      rsp_msg->cdma_rx_power.rx0_agc = p_cdma->cdma_rx0_agc;
      rsp_msg->cdma_rx_power.rx1_agc = p_cdma->cdma_rx1_agc;
      rsp_msg->cdma_rx_power_valid = TRUE;
    }
  }

  if( qm_si_is_rat_active(subs_id, QM_RAT_HDR) )
  {
    QM_MSG_HIGH_2("mstats:: subs_id:%d, hdr_is_valid:%d",
                  subs_id, global.mstats[subs_id].hdr_is_valid);
    if( global.mstats[subs_id].hdr_is_valid )
    {
      struct hdr_mstats_s *p_hdr = global.mstats[subs_id].hdr_mstats;

      rsp_msg->hdr_rx_power.rx0_agc = p_hdr->hdr_rx0_agc;
      rsp_msg->hdr_rx_power.rx1_agc = p_hdr->hdr_rx1_agc;
      rsp_msg->hdr_rx_power_valid = TRUE;
    }
  }

  if( qm_si_is_rat_active(subs_id, QM_RAT_GERAN) )
  {
    QM_MSG_HIGH_2("mstats::subs_id:%d, gsm_is_valid:%d",
                  subs_id, global.mstats[subs_id].gsm_is_valid);
    if( global.mstats[subs_id].gsm_is_valid )
    {
      struct gsm_mstats_s *p_gsm = global.mstats[subs_id].gsm_mstats;

      rsp_msg->geran_info.cell_id        = p_gsm->scell_info.cell_id;
      memscpy(rsp_msg->geran_info.plmn, sizeof(rsp_msg->geran_info.plmn),
                   p_gsm->scell_info.plmn.identity, sizeof(rsp_msg->geran_info.plmn) );
      rsp_msg->geran_info.lac            = p_gsm->scell_info.lac;
      rsp_msg->geran_info.arfcn          = p_gsm->scell_info.arfcn;
      rsp_msg->geran_info.bsic           = p_gsm->scell_info.bsic;
      rsp_msg->geran_info.timing_advance = p_gsm->timing_advance_value;
      rsp_msg->geran_info.rx_lev         = p_gsm->scell_measurements.rx_lev;

      //TLV 21
      rsp_msg->geran_info_ext.cell_id        = p_gsm->scell_info.cell_id;
      memscpy(rsp_msg->geran_info_ext.plmn, sizeof(rsp_msg->geran_info_ext.plmn),
                   p_gsm->scell_info.plmn.identity, sizeof(rsp_msg->geran_info_ext.plmn) );
      rsp_msg->geran_info_ext.lac            = p_gsm->scell_info.lac;
      rsp_msg->geran_info_ext.arfcn          = p_gsm->scell_info.arfcn;
      rsp_msg->geran_info_ext.bsic           = p_gsm->scell_info.bsic;
      rsp_msg->geran_info_ext.timing_advance = p_gsm->timing_advance_value;
      rsp_msg->geran_info_ext.rx_lev         = p_gsm->scell_measurements.rx_lev;


      if( p_gsm->nmr_info.num_of_items > NAS_NMR_MAX_NUM_V01 )
      {
        QM_MSG_ERROR_2("Recieved %d nmr items, max allowed %d", p_gsm->nmr_info.num_of_items, NAS_NMR_MAX_NUM_V01);
        rsp_msg->geran_info.nmr_cell_info_len = NAS_NMR_MAX_NUM_V01;
        rsp_msg->geran_info_ext.nmr_cell_info_len = NAS_NMR_MAX_NUM_V01;
      }
      else
      {
        rsp_msg->geran_info.nmr_cell_info_len = p_gsm->nmr_info.num_of_items;
        rsp_msg->geran_info_ext.nmr_cell_info_len = p_gsm->nmr_info.num_of_items;
      }

      for ( i=0; i<rsp_msg->geran_info.nmr_cell_info_len; i++ )
      {
        rsp_msg->geran_info.nmr_cell_info[i].nmr_cell_id = p_gsm->nmr_info.nmr[i].ncell_id.cell_id;
        memscpy(rsp_msg->geran_info.nmr_cell_info[i].nmr_plmn,sizeof(rsp_msg->geran_info.plmn) ,
                      p_gsm->nmr_info.nmr[i].ncell_id.plmn.identity, sizeof(rsp_msg->geran_info.plmn) );
        rsp_msg->geran_info.nmr_cell_info[i].nmr_lac    = p_gsm->nmr_info.nmr[i].ncell_id.lac;
        rsp_msg->geran_info.nmr_cell_info[i].nmr_arfcn  = p_gsm->nmr_info.nmr[i].arfcn;
        rsp_msg->geran_info.nmr_cell_info[i].nmr_bsic   = p_gsm->nmr_info.nmr[i].bsic;
        rsp_msg->geran_info.nmr_cell_info[i].nmr_rx_lev = p_gsm->nmr_info.nmr[i].rxlev;
      }

      for ( i=0; i<rsp_msg->geran_info_ext.nmr_cell_info_len; i++ )
      {
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_cell_id = p_gsm->nmr_info.nmr[i].ncell_id.cell_id;
        memscpy(rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_plmn,sizeof(rsp_msg->geran_info_ext.plmn) ,
                      p_gsm->nmr_info.nmr[i].ncell_id.plmn.identity, sizeof(rsp_msg->geran_info_ext.plmn) );
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_lac    = p_gsm->nmr_info.nmr[i].ncell_id.lac;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_arfcn  = p_gsm->nmr_info.nmr[i].arfcn;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_bsic   = p_gsm->nmr_info.nmr[i].bsic;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_rx_lev = p_gsm->nmr_info.nmr[i].rxlev;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_c1 = p_gsm->nmr_info.nmr[i].C1;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_c2 = p_gsm->nmr_info.nmr[i].C2;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_c31 = p_gsm->nmr_info.nmr[i].C31;
        rsp_msg->geran_info_ext.nmr_cell_info[i].nmr_c32 = p_gsm->nmr_info.nmr[i].C32;
      }

      rsp_msg->geran_info_valid = TRUE;
      rsp_msg->geran_info_ext_valid = TRUE;
      rsp_msg->gsm_info_ext.g_ta   = p_gsm->g_ta;
      rsp_msg->gsm_info_ext.g_bcch = p_gsm->g_bcch;
      rsp_msg->gsm_info_ext_valid  = TRUE;

    if( (p_gsm->scell_config.pbcch_present != 0) || (p_gsm->scell_config.gprs_ms_txpwr_max_cch != 0) 
        || (p_gsm->scell_config.gprs_rxlev_access_min != 0) )
    {
      rsp_msg->scell_geran_config_valid = TRUE;
      QM_MSG_HIGH_2("Scell Config pbcch=%d,gprs_rxlev_access_min=%d", p_gsm->scell_config.pbcch_present, p_gsm->scell_config.gprs_rxlev_access_min);
      rsp_msg->scell_geran_config.pbcch_present = p_gsm->scell_config.pbcch_present;
      rsp_msg->scell_geran_config.gprs_rxlev_access_min = p_gsm->scell_config.gprs_rxlev_access_min;
      rsp_msg->scell_geran_config.gprs_ms_txpwr_max_cch = p_gsm->scell_config.gprs_ms_txpwr_max_cch;
    }
    
    QM_MSG_HIGH_1("gsm timeslot = %d", p_gsm->current_timeslot.timeslot);
    
    if(p_gsm->current_timeslot.timeslot != 0xff)
    {
      rsp_msg->current_l1_ts = p_gsm->current_timeslot.timeslot;
      rsp_msg->current_l1_ts_valid = TRUE;
    }

    }
  }

  if( qm_si_is_rat_active(subs_id, QM_RAT_UMTS) )
  {
    QM_MSG_HIGH_2("mstats::subs_id :%d, wcdma_is_valid:%d",
                  subs_id, global.mstats[subs_id].wcdma_is_valid);
    if( global.mstats[subs_id].wcdma_is_valid )
    {
      struct wcdma_mstats_s *p_wcdma = global.mstats[subs_id].wcdma_mstats;
      
      int stack_index = STACK_INDEX_0;

      if( qmi_nasi_global.cm_msim_ss_info[subs_id].number_of_stacks > 1 &&
          qmi_nasi_global.cm_msim_ss_info[subs_id].stack_info[STACK_INDEX_1].sys_mode == SYS_SYS_MODE_WCDMA )
      {
        stack_index = STACK_INDEX_1;
      }
        
      rsp_msg->umts_info.cell_id   = qmi_nasi_global.cm_msim_ss_info[subs_id].stack_info[stack_index].cell_info.cell_id;
      rsp_msg->umts_ext_info.cell_id   = qmi_nasi_global.cm_msim_ss_info[subs_id].stack_info[stack_index].cell_info.cell_id;

#if (defined(FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH) && defined (FEATURE_WCDMA))
      if ( qmi_nasi_global.cm_msim_ss_info[subs_id].stack_info[stack_index].cell_info.cell_id == SYS_WCDMA_INVALID_CELL_ID )
      {
        QM_MSG_HIGH("cell id is invalid. call cm_ss_cmd_get_dch_cell_info");
        if ( !cm_ss_cmd_get_dch_cell_info( NULL, NULL, qmi_nasi_global.cm_clnt_id ) )
        {
          QM_MSG_ERROR("cm_ss_cmd_get_dch_cell_info failed");
        }
      }
#endif
      memscpy( rsp_msg->umts_info.plmn, sizeof(rsp_msg->umts_info.plmn) ,
                   p_wcdma->wcdma_serving_cell_info.plmn_id.identity, sizeof(rsp_msg->umts_info.plmn) );
      rsp_msg->umts_info.lac       = p_wcdma->wcdma_serving_cell_info.lac_id;
      rsp_msg->umts_info.uarfcn    = p_wcdma->wcdma_serving_cell_info.uarfcn;
      rsp_msg->umts_info.psc       = p_wcdma->wcdma_serving_cell_info.psc;
      rsp_msg->umts_info.rscp      = p_wcdma->wcdma_serving_cell_power_info.rscp;
      rsp_msg->umts_info.ecio      = p_wcdma->wcdma_serving_cell_power_info.ecio;

// TLV 0x22
      memscpy( rsp_msg->umts_ext_info.plmn, sizeof(rsp_msg->umts_ext_info.plmn) ,
                   p_wcdma->wcdma_serving_cell_info.plmn_id.identity, sizeof(rsp_msg->umts_ext_info.plmn) );
      rsp_msg->umts_ext_info.lac       = p_wcdma->wcdma_serving_cell_info.lac_id;
      rsp_msg->umts_ext_info.uarfcn    = p_wcdma->wcdma_serving_cell_info.uarfcn;
      rsp_msg->umts_ext_info.psc       = p_wcdma->wcdma_serving_cell_info.psc;
      rsp_msg->umts_ext_info.rscp      = p_wcdma->wcdma_serving_cell_power_info.rscp;
      rsp_msg->umts_ext_info.ecio      = p_wcdma->wcdma_serving_cell_power_info.ecio;
      rsp_msg->umts_ext_info.squal = p_wcdma->wcdma_serving_cell_power_info.squal;
      rsp_msg->umts_ext_info.srxlev = p_wcdma->wcdma_serving_cell_power_info.srxlev;

      QM_MSG_HIGH_1("%d WCDMA freqs, max expected 3", p_wcdma->wcdma_monitored_set_info.num_of_freqs);
      for ( i=0; i<p_wcdma->wcdma_monitored_set_info.num_of_freqs; i++ ) // freq
      {
        QM_MSG_HIGH_2("%d WCDMA cells for freq %d, max expected 8", p_wcdma->wcdma_monitored_set_info.freq[i].num_of_cells, p_wcdma->wcdma_monitored_set_info.num_of_freqs);
        for ( j=0; j<p_wcdma->wcdma_monitored_set_info.freq[i].num_of_cells && rsp_msg->umts_info.umts_monitored_cell_len<NAS_UMTS_MAX_MONITORED_CELL_SET_NUM_V01; j++ ) // cell
        {
          rsp_msg->umts_info.umts_monitored_cell[rsp_msg->umts_info.umts_monitored_cell_len].umts_uarfcn = p_wcdma->wcdma_monitored_set_info.freq[i].uarfcn;
          rsp_msg->umts_info.umts_monitored_cell[rsp_msg->umts_info.umts_monitored_cell_len].umts_psc    = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].psc;
          rsp_msg->umts_info.umts_monitored_cell[rsp_msg->umts_info.umts_monitored_cell_len].umts_rscp   = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].rscp;
          rsp_msg->umts_info.umts_monitored_cell[rsp_msg->umts_info.umts_monitored_cell_len].umts_ecio   = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].ecio;
          rsp_msg->umts_info.umts_monitored_cell_len++;
          // TLV 0x22
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_uarfcn = p_wcdma->wcdma_monitored_set_info.freq[i].uarfcn;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_psc    = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].psc;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_rscp   = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].rscp;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_ecio   = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].ecio;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_squal  = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].squal;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_srxlev = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].srxlev;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_rank = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].rank;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell[rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len].umts_set   = p_wcdma->wcdma_monitored_set_info.freq[i].cell[j].set;
          rsp_msg->umts_ext_info.umts_monitored_ext_ext_cell_len++;

        }
      }

      if( p_wcdma->wcdma_monitored_set_info.num_gsm_cells > NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01 )
      {
        QM_MSG_ERROR_2("Received %d wcdma instances, max allowed %d", p_wcdma->wcdma_monitored_set_info.num_gsm_cells, NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01);
        rsp_msg->umts_info.umts_geran_nbr_cell_len = NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01;
        rsp_msg->gncell_bcch_len                   = NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell_len = NAS_UMTS_GERAN_MAX_NBR_CELL_SET_NUM_V01;
      }
      else
      {
        rsp_msg->umts_info.umts_geran_nbr_cell_len = p_wcdma->wcdma_monitored_set_info.num_gsm_cells;
        rsp_msg->gncell_bcch_len                   = p_wcdma->wcdma_monitored_set_info.num_gsm_cells;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell_len = p_wcdma->wcdma_monitored_set_info.num_gsm_cells;
      }

      for ( i=0; i<rsp_msg->umts_info.umts_geran_nbr_cell_len; i++ )
      {
        rsp_msg->umts_info.umts_geran_nbr_cell[i].geran_arfcn    = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].arfcn;
        rsp_msg->umts_info.umts_geran_nbr_cell[i].geran_bsic_ncc = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].bsic_ncc;
        rsp_msg->umts_info.umts_geran_nbr_cell[i].geran_bsic_bcc = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].bsic_bcc;
        rsp_msg->umts_info.umts_geran_nbr_cell[i].geran_rssi     = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].rssi;
        rsp_msg->gncell_bcch[i]                                  = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].bcch;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell[i].geran_arfcn    = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].arfcn;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell[i].geran_bsic_ncc = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].bsic_ncc;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell[i].geran_bsic_bcc = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].bsic_bcc;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell[i].geran_rssi     = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].rssi;
        rsp_msg->umts_ext_info.umts_geran_ext_nbr_cell[i].geran_rank     = p_wcdma->wcdma_monitored_set_info.gsm_cell[i].rank;

      }

      if ( rsp_msg->gncell_bcch_len > 0 )
      {
        rsp_msg->gncell_bcch_valid = TRUE;
      }

      rsp_msg->umts_info_valid = TRUE;
      rsp_msg->umts_ext_info_valid = TRUE;

      rsp_msg->umts_cell_id = qmi_nasi_global.cm_msim_ss_info[subs_id].stack_info[stack_index].cell_info.cell_id;
      rsp_msg->umts_cell_id_valid = TRUE;

      switch( p_wcdma->wcdma_rrc_state )
      {
        case SYS_WCDMA_RRC_STATE_DISCONNECTED:
          rsp_msg->wcdma_lte.wcdma_rrc_state = (uint32)NAS_WCDMA_RRC_STATE_DISCONNECTED_V01;
          break;
        case SYS_WCDMA_RRC_STATE_CELL_PCH:
          rsp_msg->wcdma_lte.wcdma_rrc_state = (uint32)NAS_WCDMA_RRC_STATE_CELL_PCH_V01;
          break;
        case SYS_WCDMA_RRC_STATE_URA_PCH:
          rsp_msg->wcdma_lte.wcdma_rrc_state = (uint32)NAS_WCDMA_RRC_STATE_URA_PCH_V01;
          break;
        case SYS_WCDMA_RRC_STATE_CELL_FACH:
          rsp_msg->wcdma_lte.wcdma_rrc_state = (uint32)NAS_WCDMA_RRC_STATE_CELL_FACH_V01;
          break;
        case SYS_WCDMA_RRC_STATE_CELL_DCH:
          rsp_msg->wcdma_lte.wcdma_rrc_state = (uint32)NAS_WCDMA_RRC_STATE_CELL_DCH_V01;
          break;
      }

      rsp_msg->wcdma_lte.umts_lte_nbr_cell_len = MIN( p_wcdma->wcdma_monitored_set_info.num_lte_cells, SYS_WCDMA_MAX_MONITORED_LTE_CELLS );
      QM_MSG_HIGH_2("wcdma rrc state = %d, Number of LTE Cells received = %d", p_wcdma->wcdma_rrc_state, p_wcdma->wcdma_monitored_set_info.num_lte_cells);
      rsp_msg->lte_earfcn_len = rsp_msg->wcdma_lte.umts_lte_nbr_cell_len;

      for( i=0; i < rsp_msg->wcdma_lte.umts_lte_nbr_cell_len; i++ )
      {
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].earfcn = (uint16)p_wcdma->wcdma_monitored_set_info.lte_cell[i].earfcn;
        rsp_msg->lte_earfcn[i] = p_wcdma->wcdma_monitored_set_info.lte_cell[i].earfcn;
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].pci = p_wcdma->wcdma_monitored_set_info.lte_cell[i].cell_id;
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].rsrp = (float)(p_wcdma->wcdma_monitored_set_info.lte_cell[i].rsrp);
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].rsrq = (float)(p_wcdma->wcdma_monitored_set_info.lte_cell[i].rsrq);
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].srxlev = p_wcdma->wcdma_monitored_set_info.lte_cell[i].s_rxlev;
        rsp_msg->wcdma_lte.umts_lte_nbr_cell[i].cell_is_tdd = p_wcdma->wcdma_monitored_set_info.lte_cell[i].is_tdd_freq;

        QM_MSG_HIGH_3("earfcn = %d, cell id = %d, srxlev = %d", p_wcdma->wcdma_monitored_set_info.lte_cell[i].earfcn, p_wcdma->wcdma_monitored_set_info.lte_cell[i].cell_id, p_wcdma->wcdma_monitored_set_info.lte_cell[i].s_rxlev);
        QM_MSG_HIGH_3("rsrp = %d, rsrq = %d, TDD Cell = %d", p_wcdma->wcdma_monitored_set_info.lte_cell[i].rsrp, p_wcdma->wcdma_monitored_set_info.lte_cell[i].rsrq, p_wcdma->wcdma_monitored_set_info.lte_cell[i].is_tdd_freq);
      }
      rsp_msg->wcdma_lte_valid = TRUE;
      rsp_msg->lte_earfcn_valid = TRUE;

      rsp_msg->wcdma_info_ext.w_agc     = p_wcdma->w_agc;
      rsp_msg->wcdma_info_ext.w_txagc   = p_wcdma->w_txagc;
      rsp_msg->wcdma_info_ext.w_dl_bler = p_wcdma->w_dl_bler;
      rsp_msg->wcdma_info_ext_valid     = TRUE;

      // TLV 0x1F and 0x23
      if( p_wcdma->wcdma_rrc_state == SYS_WCDMA_RRC_STATE_CELL_DCH )
      {
        rsp_msg->cells_len = p_wcdma->wcdma_active_set_info.num_rls;
        rsp_msg->wcdma_active_set_cells_len = p_wcdma->wcdma_active_set_info.num_rls;
        if(rsp_msg->cells_len)
        {
          rsp_msg->cells_valid = TRUE;
          rsp_msg->wcdma_active_set_cells_valid = TRUE;
        }

        for( i=0; i<rsp_msg->cells_len && i<SYS_WCDMA_MAX_ASET_CELLS; i++ )
        {
          rsp_msg->cells[i].psc = p_wcdma->wcdma_active_set_info.psc[i];
          rsp_msg->cells[i].cell_id = p_wcdma->wcdma_active_set_info.cell_id[i];
          rsp_msg->cells[i].rscp = p_wcdma->wcdma_active_set_info.rscp[i];
          rsp_msg->cells[i].ecio = p_wcdma->wcdma_active_set_info.ecio[i];
          // wcdma_active_set_info doesn't have uarfcn, read it from wcdma_active_set_reference_rl_info since it is the strongest cell in active set
          // and all active set cells have same UARFCN
          rsp_msg->cells[i].uarfcn = p_wcdma->wcdma_active_set_reference_rl_info.uarfcn;

          rsp_msg->wcdma_active_set_cells[i].psc = p_wcdma->wcdma_active_set_info.psc[i];
          rsp_msg->wcdma_active_set_cells[i].cell_id = p_wcdma->wcdma_active_set_info.cell_id[i];
          rsp_msg->wcdma_active_set_cells[i].rscp = p_wcdma->wcdma_active_set_info.rscp[i];
          rsp_msg->wcdma_active_set_cells[i].ecio = p_wcdma->wcdma_active_set_info.ecio[i];
          // wcdma_active_set_info doesn't have uarfcn, read it from wcdma_active_set_reference_rl_info since it is the strongest cell in active set
          // and all active set cells have same UARFCN
          rsp_msg->wcdma_active_set_cells[i].uarfcn = p_wcdma->wcdma_active_set_reference_rl_info.uarfcn;
          rsp_msg->wcdma_active_set_cells[i].sf = (nas_wcdma_l1_sf_e_type_v01)p_wcdma->wcdma_active_set_info.sf;
          rsp_msg->wcdma_active_set_cells[i].phy_chan_type = (nas_wcdma_l1_dl_phychan_e_type_v01)p_wcdma->wcdma_active_set_info.phy_chan_type;
          rsp_msg->wcdma_active_set_cells[i].slot_format= p_wcdma->wcdma_active_set_info.slot_format;
          rsp_msg->wcdma_active_set_cells[i].is_compressed_mode_on= p_wcdma->wcdma_active_set_info.is_compressed_mode_on;

          QM_MSG_HIGH_3("WCDMA Active Set: psc %d, cell_id %d, rscp %d", p_wcdma->wcdma_active_set_info.psc[i], p_wcdma->wcdma_active_set_info.cell_id[i], p_wcdma->wcdma_active_set_info.rscp[i]);
        }

        // TLV 0x20   
        rsp_msg->wcdma_active_set_reference_rl.uarfcn = p_wcdma->wcdma_active_set_reference_rl_info.uarfcn;
        rsp_msg->wcdma_active_set_reference_rl.psc = p_wcdma->wcdma_active_set_reference_rl_info.psc;
        rsp_msg->wcdma_active_set_reference_rl.rac = p_wcdma->wcdma_active_set_reference_rl_info.rac_id;
        rsp_msg->wcdma_active_set_reference_rl.lac = p_wcdma->wcdma_active_set_reference_rl_info.lac_id;
        rsp_msg->wcdma_active_set_reference_rl.cell_id = p_wcdma->wcdma_active_set_reference_rl_info.cell_id;
        memscpy( rsp_msg->wcdma_active_set_reference_rl.plmn, sizeof(rsp_msg->wcdma_active_set_reference_rl.plmn), p_wcdma->wcdma_active_set_reference_rl_info.plmn_id.identity, sizeof(p_wcdma->wcdma_active_set_reference_rl_info.plmn_id.identity) );

        QM_MSG_HIGH_3("WCDMA Active Set Ref RL: uarfcn %d, psc %d, cell_id %d", p_wcdma->wcdma_active_set_reference_rl_info.uarfcn, p_wcdma->wcdma_active_set_reference_rl_info.psc, p_wcdma->wcdma_active_set_reference_rl_info.cell_id);
       
        QM_MSG_HIGH_2("WCDMA Active Set Ref RL: rac %d, lac %d", p_wcdma->wcdma_active_set_reference_rl_info.rac_id, p_wcdma->wcdma_active_set_reference_rl_info.lac_id);

        rsp_msg->wcdma_active_set_reference_rl_valid = TRUE;
      }
      else
      {
        QM_MSG_HIGH_1("WCDMA RRC state is %d, TLV 0x1F & 0x20 are not populated", p_wcdma->wcdma_rrc_state);
      }
    }
  }
}
