/**
  @file wcdma_interface.c

  @brief  
*/
/*
  Copyright (c) 2013 Qualcomm Technologies, Inc.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/common/src/wcdma_interface.c#1 $
  $DateTime: 2016/12/13 08:00:12 $


  when        who     what, where, why
  ---------   ---     ------------------------------------------------------------
05/12/16      sas     Added api interface fun to provide acqdb cell list info to LTE.
12/02/15      gv      Compiler error fixes for GPS compile out flavor
09/16/15      sr      Initial checkin for FR 26957 - TxAGC logging
02/09/15      sp      Made changes to add a new API is added which takes as_id as agrument and calls rrcdormancy_check_if_nw_support_fast_dormancy() 
11/07/14      as      Fixed compilation error
05/28/14      geg     Mainlined feature FEATURE_WCDMA_GET_RAT_MEAS_INFO
04/28/14      sp      Made changes to add rrc_vocoder_control_enabled_sub for DUAL SIM cases
04/23/14      geg     Fix a compilation error that occured after enabling segment loading feature
04/14/14      cc      Create an API for RF to call idle W2X search cfg to WFW in FTM mode
01/08/14      geg     Segment Loading - WCDMA interface updates for DIME3.0 
11/26/13      geg     Segment loading FSL: add dummy task 
10/17/13      hzhi    Support new FEATURE_DUAL_SIM on DIME3.0. 
10/09/13      geg     Clean up WCDMA private API from interface inclusion chain for Feature Segment Loading DIME 3.0
2013/10/01    geg     Updated WCDMA interface for segment loading on DIME 3.0.
2013/05/10    hzhi    Added file hearder according to qualcomm standards.
2013/01/01    ngeng   Created for segment loading.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "sys_type.h"
#include "sys.h"

#ifdef FEATURE_SEGMENT_LOADING
/* for wcdma_interface.c */
#include "wl1drxmanager.h"
#include "drx.h"
#include "wl1m.h"
#include "l1msetup.h"
#include "rrcccm.h"
#include "rrcscmgr.h"
#include "rrclcm.h"
#include "wcdma_rrc_api.h"
#include "wl1sleep.h"
#include "rrcrrif.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#include "wsrch.h"

iWCDMA_vtbl  wcdma_vtbl = 
{
		NULL,
		NULL,
		NULL,
#ifdef FEATURE_CGPS_UMTS_UP
		l1_cgps_register_event_cb,
#else
                NULL,
#endif
		l1_get_cmd_buf,
		l1_is_sub_chan_avail_for_asc,
		l1_put_cmd,
		l2_dl_get_cmd_buf,
		l2_dl_put_cmd,
		l2_ul_get_cmd_buf,
		l2_ul_put_cmd,
		mdsp_sync_iq_sample_capture_cmd,
		mdsp_sync_send_iq_cap_cmd,
		mdsp_sync_send_mod_timing_cmd,
		mdsp_sync_send_rf_init_cmd,
		mdsp_sync_send_rxagc_cmd,
		mdsp_sync_send_txlm_cfg_cmd,
		mdsp_sync_send_ul_dpcch_cfg_cmd,
                mdsp_send_ftm_idle_mode_w2x_cfg_cmd,
		rlc_dl_deregister_srvc_handler,
		rlc_dl_register_srvc_handler,
		rlc_ul_deregister_srvc_handler,
		rlc_ul_register_srvc_handler,
		rrc_delete_classmark_change_handover_info,
		rrc_find_rab_for_rb_in_est_rabs,
		rrc_free_cmd_buf,
		rrc_get_classmark_change_handover_info,
		rrc_get_cm_cmd_buf,
		rrc_get_data_rate_for_ds,
		rrc_get_l1_cmd_buf,
  #if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
		rrc_get_lsm_cmd_buf,
  #else
                NULL,
  #endif
		rrc_get_mm_cmd_buf,
		rrc_get_rr_cmd_buf,
		rrc_get_tc_cmd_buf,
		rrc_is_arfcn_valid,
		rrc_is_wcdma_active,
		rrc_malloc_for_external_cmd,
		rrc_put_cm_cmd,
  #if defined (FEATURE_CGPS_UMTS_CP_WCDMA)
		rrc_put_lsm_cmd,
  #else
                NULL,
  #endif
		rrc_put_mm_cmd,
		rrc_put_rr_cmd,
		rrc_put_tc_cmd,
		rrc_register_rrc_codec_ho_info_cb,
		rrc_register_serving_cell_change_ind_cb,
		rrc_reset_mcm_irat_resel_act_cause,
		rrc_return_cs_data_call_parms,
		rrc_return_lc_info_for_rab,
		rrc_rr_check_cell_barred,
		rrc_set_ue_in_deep_sleep,
		rrc_update_band_preference,
		rrc_vocoder_acquired,
		rrc_vocoder_control_enabled,
  #ifdef FEATURE_CGPS_UMTS_CP_WCDMA
		rrcgps_register_cgps_event_cb,
		rrcgps_register_cgps_ue_pos_capability_cb,
  #else
                NULL,
                NULL,
  #endif
  #if defined (FEATURE_CGPS)
		rrcmeas_get_cell_plmn_info,
  #else
                NULL,
  #endif
		rrcscr_get_signalling_connection_status,
		rxd_overide_nv_settings,
		rxd_set_active_rx_chain,
		seq_get_cfn,
		wcdma_get_signal_info,
		wcdma_l2_dl_get_cmd_buf,
		wcdma_l2_dl_put_cmd,
		wcdma_l2_ul_get_cmd_buf,
		wcdma_l2_ul_put_cmd,
		wcdma_query_tx_power_dbm,
#ifdef FEATURE_LTE_TO_WCDMA
		wcdma_rrc_get_band_from_dl_uarfcn,
#else   /*FEATURE_LTE_TO_WCDMA */
                NULL,
#endif
		wl1_check_state_for_delayed_gstmr_int,
/*======================================================================================
  added by hzhi on 09/23/2013 to update WCDMA interface on DIME3.0 for segment loading. 
  ======================================================================================*/
    mdsp_sync_send_et_txlm_cfg_cmd,
    mdsp_sync_send_rxlm_dyn_update_cmd,
    mdsp_sync_send_sample_server_cmd,
    rrc_gstk_get_utran_nmr_req,
    wcdma_get_modem_stats_info,
    wcdma_set_modem_stats_mask,
    wmcpmdrv_rf_ftm_mode_req, 
    wcdma_start_capture_scan_data,
    wcdma_stop_capture_scan_data,
/*
 added by hzhi on 10/17/2013 to support new FEATURE_DUAL_SIM on DIME3.0.
 */
    rrc_return_wcdma_rrc_version,
#ifdef FEATURE_DUAL_SIM
    rrc_delete_classmark_change_handover_info_sub_id,
    rrc_get_classmark_change_handover_info_sub_id,
    rrc_is_arfcn_valid_sub_id,
    rrc_rr_check_cell_barred_sub_id,
    rrc_set_ue_in_deep_sleep_sub_id,
#else /*FEATURE_DUAL_SIM*/
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#endif/*FEATURE_DUAL_SIM*/
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
    rrc_update_band_preference_sub_id,
    wl1trm_can_gsm_disable_fw_app_wcdma,
    rrc_vocoder_control_enabled_sub,
#else /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    NULL,
    NULL,
    NULL,
#endif/* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */
    wcdma_rrc_get_wcsg_lfp_info,
/*
 * added on 01/07/2014
 */
    wl1_eul_stats,
    wl1_query_modem_tx_power,
    wcdma_convert_uarfcn,
    wcdma_derive_freq_from_uarfcn,
/*
 * added on 02/25/2014 for MCS customer request. 
 */
    wcdma_get_rat_meas_info,
    wcdma_is_rat_meas_info_ready,
/*
 * added on 02/06/2015
 */
    rrc_check_if_nw_support_fd,

/*======================================================================================
  added by sramanna on 05/27/2015 to update WCDMA interface for TxAGC logging
  ======================================================================================*/
    wl1_config_modem_power_stats_tx,
    wl1_get_modem_power_stats,
/*
 * added on 03/10/2015 to provide acqdb info to LTE. 
 */
  wcdma_rrc_get_acqdb_cell_list,
/*====================================== end of edit ====================================*/
};

iWCDMA_mmcp_vtbl  wcdma_mmcp_vtbl = 
{
		NULL,
		NULL,
		NULL,
    dl_dec_deregister_ctch_pdu_decode_error_cb,
    dl_dec_register_ctch_pdu_decode_error_cb
};

iWCDMA_rftech_vtbl  wcdma_rftech_vtbl = 
{
		NULL,
		NULL,
		NULL,
    mdsp_sync_send_start_stop_cmd
};

iWCDMA_qchat_vtbl  wcdma_qchat_vtbl = 
{
		NULL,
		NULL,
		NULL,
    drx_manager_wake_from_sleep,
		drx_sleep_abort,
		l1m_setup_next_cmd,
		rrc_ccm_get_serv_plmn_cellid,
		rrc_get_int_cmd_buf,
		rrc_get_state,
		rrc_put_int_cmd,
#ifdef FEATURE_HSPA_CALL_STATUS_IND
		rrc_send_hspa_call_status_info,
#else /*FEATURE_HSPA_CALL_STATUS_IND*/
    NULL,
#endif /* FEATURE_HSPA_CALL_STATUS_IND */
		rrccu_clear_procedure,
		rrccu_convert_t302_to_ms,
		rrccu_return_cu_substate,
		rrccu_send_cell_update_initiate_req,
		rrccu_send_cell_update_msg,
		rrccu_set_cu_substate,
		rrclcm_check_ul_rlc_lc_id,
		rrctmr_get_remaining_time,
		rrctmr_start_timer,
		seq_get_align_type,
		wl1_send_msg_to_controller,
    sleep_abort,
    l1_free_cmd_buf
};

/*==========================================================================================*/

void wcdma_interface_dtor(void  *pMe)
{
  if(!pMe){
    return;
  }
  rrc_free(pMe);
  
}


interface_t * wcdma_interface_ctor(void)
{
	interface_t *pi = (interface_t *) rrc_malloc(sizeof(interface_t));
  if(!pi)
  {
    return NULL;
  }
	ref_cnt_obj_init(pi, wcdma_interface_dtor);

	pi->pVtable = (iunknown_t *) &wcdma_vtbl;

	return pi;
}


void wcdma_interface_mmcp_dtor(void  *pMe)
{
  if(!pMe){
    return;
  }
	rrc_free(pMe);
}

interface_t * wcdma_interface_mmcp_ctor(void)
{
#ifndef FEATURE_UMTS_BMC
  return NULL;
#endif /* FEATURE_UMTS_BMC */
	interface_t *pi = (interface_t *) rrc_malloc(sizeof(interface_t));
  if(!pi)
  {
    return NULL;
  }
	ref_cnt_obj_init(pi, wcdma_interface_mmcp_dtor);

	pi->pVtable = (iunknown_t *) &wcdma_mmcp_vtbl;

	return pi;
}


void wcdma_interface_rftech_dtor(void  *pMe)
{
  if(!pMe){
    return;
  }
	rrc_free(pMe);
}

interface_t * wcdma_interface_rftech_ctor(void)
{
	interface_t *pi = (interface_t *) rrc_malloc(sizeof(interface_t));
  if(!pi)
  {
    return NULL;
  }
	ref_cnt_obj_init(pi, wcdma_interface_rftech_dtor);

	pi->pVtable = (iunknown_t *) &wcdma_rftech_vtbl;

	return pi;
}


void wcdma_interface_qchat_dtor(void  *pMe)
{
  if(!pMe){
    return;
  }
	rrc_free(pMe);
}


interface_t * wcdma_interface_qchat_ctor(void)
{
	interface_t *pi = (interface_t *) rrc_malloc(sizeof(interface_t));
  if(!pi)
  {
    return NULL;
  }
	ref_cnt_obj_init(pi, wcdma_interface_qchat_dtor);

	pi->pVtable = (iunknown_t *) &wcdma_qchat_vtbl;

	return pi;
}

void wcdma_task_null(void) __attribute__((section(".task_text")));
void wcdma_task_null(void)
{
#ifndef FEATURE_MODEM_RCINIT
  rex_sigs_type      rex_signals_mask;  /* Mask of signals returned by rex */
  /* Send an ACK and wait for Task Start signal */
  tmc_task_start();
#else /*FEATURE_MODEM_RCINIT*/
  /* Send Handshake to RCINIT */
  rcinit_handshake_startup();
#endif /* FEATURE_MODEM_RCINIT */


  for ( ;; )
  { /* Never exit this loop ... */
#ifndef FEATURE_MODEM_RCINIT
    rex_signals_mask = rex_wait(
                                 TASK_OFFLINE_SIG   |
                                 TASK_STOP_SIG
                                 );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      (void) rex_clr_sigs( rex_self(), TASK_STOP_SIG );
  //    (void) rex_set_sigs( &tmc_tcb, TMC_ACK_SIG );
    }

    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      (void) rex_clr_sigs( rex_self(), TASK_OFFLINE_SIG );
  //    (void) rex_set_sigs( &tmc_tcb, TMC_ACK_SIG );
    }

#else /* FEATURE_MODEM_RCINIT */
    rex_wait(
             TASK_OFFLINE_SIG   |
             TASK_STOP_SIG
            );

#endif /* FEATURE_MODEM_RCINIT */

  } /* end for */
}

#endif /* FEATURE_SEGMENT_LOADING */
