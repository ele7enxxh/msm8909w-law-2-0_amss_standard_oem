/**
  @file ITDSCDMA.h

  @brief  This is the header file that describes an interface to TDSCDMA

*/
/*
  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/ITDSCDMA.h#1 $
  $DateTime: 2016/12/13 07:58:19 $


  when        who     what, where, why
  ---------   ---     ------------------------------------------------------------
 2013/09/20   hzhi    Update interface for DIME3.0 segment loading. 
 2013/05/10   hzhi    Added file hearder according to qualcomm standards.
 2013/01/01   ngeng   Created for segment loading.

===========================================================================*/
#ifndef _ITDSCDMA_H_
#define _ITDSCDMA_H_

#include "mcfg_seg_load.h"
#include "tdsrrcdataext.h"
#include "tdsrrc_rr_if.h"
//#include "tdsl2external.h"
#include "tdsl2dldatamodem.h"
#include "tdsl2uldatamodem.h"
#include "tdsrrccmd.h"
#include "tdsrrclsmif.h"
#include "tds_l1_api.h"
#include "tds_rrc_api.h"

extern PACKED void *tdscdmadiag_change_bler_rate(PACKED void* req_pkt, uint16 pkt_len);

//#define W_ARFCN_T ARFCN_T

/*  The component that is being used provides typedefs for pointers to each
    of its exported functions. 
 */
typedef boolean (*fn_rrc_is_tds_active)(void);
typedef sys_band_mask_type (*fn_tds_rrc_get_band_from_dl_uarfcn)(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  boolean *is_band_valid
);
typedef boolean (*fn_tds_rrc_rat_is_supported)(sys_plmn_id_s_type  camped_plmn_id);		

typedef boolean (*fn_tdsl2_dl_service_register)
(
  tdsl2_dl_service_register_type *srvc_ptr
);		
typedef boolean (*fn_tdsl2_dl_submit_datamodem_msg)
(
  tdsl2_dl_datamodem_api_type * cmd_ptr
);		
typedef boolean (*fn_tdsl2_ul_service_register)
(
  tdsl2_ul_service_register_type *srvc_ptr
);		
typedef boolean (*fn_tdsl2_ul_submit_datamodem_msg)
(
  tdsl2_ul_datamodem_api_type * cmd_ptr
);		
typedef tdsrrc_cell_bar_info_e_type (*fn_tdsrrc_check_cell_barred)
(
  uint16 freq, uint8 cpid
);		
typedef void (*fn_tdsrrc_delete_classmark_change_handover_info)(void);		
typedef rrc_rab_id_type (*fn_tdsrrc_find_rab_for_rb_in_est_rabs)
(
  uint8 rb_id
);		
	
typedef void (*fn_tdsrrc_get_classmark_change_handover_info)
(
  byte ** msg_ptr,    /* The command to be sent to UTRAN */
  uint8 * msg_len    /* Length of the message           */
  , boolean irat_ho_info_compressed

);		
typedef tdsrrc_hsdpa_status_enum_type (*fn_tdsrrc_get_hsdpa_status)
(
  void
);		
typedef tdsrrc_hsupa_status_enum_type (*fn_tdsrrc_get_hsupa_status)
(
  void
);		
typedef tdsrrc_mm_cmd_type * (*fn_tdsrrc_get_mm_cmd_buf)( void );		
typedef tdsrrc_rr_cmd_type * (*fn_tdsrrc_get_rr_cmd_buf)( void );	
typedef tdsrrc_tc_cmd_type * (*fn_tdsrrc_get_tc_cmd_buf)( void );		
typedef boolean (*fn_tdsrrc_is_arfcn_valid)
(
  uint32 UARFCN
);
		
typedef void * (*fn_tdsrrc_malloc_for_external_cmd)
(
  size_t size
);		
typedef void (*fn_tdsrrc_put_mm_cmd)
(
  tdsrrc_mm_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);		
typedef void (*fn_tdsrrc_put_rr_cmd)
(
  tdsrrc_rr_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);		
typedef void (*fn_tdsrrc_put_tc_cmd)
(
  tdsrrc_tc_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);		
typedef boolean (*fn_tdsrrc_register_serving_cell_change_ind_cb)
(
  TDSRRC_SERVING_CELL_CHANGE_IND_CB_FUNC_TYPE* cb_ptr
);		
typedef tdsrrc_status_e_type (*fn_tdsrrc_return_cs_data_call_parms)
(    
  uint32                  rab_id,
  tdsrrc_cs_data_parms_type *cs_data_parms_ptr
);
		
typedef tdsrrc_rab_search_e_type (*fn_tdsrrc_return_lc_info_for_rab)
(
  tdsrrc_user_plane_lc_info_type *ptr
);		
typedef void (*fn_tdsrrc_set_ue_in_deep_sleep)( boolean value);
		
typedef void (*fn_tdsrrc_update_band_preference)
(
  sys_band_mask_type band_pref_mask  /* band mask */
);		
typedef void (*fn_tdsrrc_vocoder_acquired)(void);		
typedef boolean (*fn_tdsrrc_vocoder_control_enabled)(void);		
//#ifdef FEATURE_DUAL_SIM
typedef boolean (*fn_tdsrrc_vocoder_control_enabled_sub) (sys_modem_as_id_e_type as_id);
//#endif
typedef boolean (*fn_tdsrrcmeas_get_cell_plmn_info)
(
  tdsrrcmeas_plmn_cell_info_struct_type* plmn_cell_info_ptr
);		
typedef boolean (*fn_tdsrxd_set_rx_chain_status)(tdsl1_cmapi_if_rx_status_type cmapi_rx_chain_config);		
typedef void (*fn_tdssrch_get_freq_syncdlRssi_tx_pwr_info) (tdssrch_freq_syncdlRssi_type* cm_freq_DwPTSRssi,tdssrch_uplink_tx_power_type* cm_tx_pwr_info);

/*==================================================================
 added by hzhi on 05/17/2013 for DIME2.0 CRM_M9625TAAAANAAM200096.1
 -> updated function pointer in vtble
 ===================================================================*/
typedef PACKED void * (*fn_tdscdmadiag_change_bler_rate)(PACKED void* req_pkt, uint16 pkt_len);

typedef void (*fn_tdsrrc_register_rrc_codec_ho_info_cb)
(
  TDSRRC_CODEC_HO_INFO_CB_FUNC_TYPE *cb_ptr
);

/*=================================================================
  added by hzhi on 09/20/2013 for DIME3.0 M8974AAAAANAAM200349.
  =================================================================*/

typedef  boolean 
  (*fn_tdsl1_api_send_acq_cmd)
    (
      tdsl1_api_acq_req_t *msg
    );

typedef  boolean 
  (*fn_tdsl1_api_send_setup_cmd)
    (
      tdsl1_api_cphy_setup_req_t *msg
    );

typedef  boolean 
  (*fn_tdsl1_api_send_start_cmd)
    (
      void
    );

typedef  boolean 
  (*fn_tdsl1_api_send_stop_cmd)
    (
      void
    );
/*
 * added by hzhi on 10/17/2013 to update DIME3.0 TDS interface. 
 */
typedef boolean 
  (*fn_tdsl1_api_send_timetag_cmd)
    (
      boolean allow_sleep_timetag
    );

/*
 * added on 01/07/2014
 */
typedef tdsrrc_ul_tx_power_range_e_type 
  (*fn_tdsrrc_get_ul_tx_power_range)
    (
      void
    );

typedef void 
  (*fn_tdsrrc_get_ul_rate_info)
    (
      tdsrrc_ul_rate_info_type *ul_rate_info_ptr
    );

typedef errno_enum_type 
  (*fn_tds_rrc_derive_freq_from_uarfcn)
    (
      uint16 uarfcn,
      uint16 *freq
    );

/*
 * added on 01/31/2014 needed by CR 607232
 */
typedef boolean 
  (*fn_tdssrch_get_timeslot_based_rxagc_info)
    (
      tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info
    );

typedef void 
  (*fn_tdssrch_cm_get_serving_cell_signal__info)
    (
      tds_l1_cm_scell_signal_status_ind_type *cm_serving_cell_signal_strength_info
    );

typedef void 
  (*fn_tdssrch_cm_get_serving_cell_signal_instance_info)
    (
      tds_l1_cm_scell_signal_instance_info_type *cm_serving_cell_signal_strength_info
    );

typedef boolean 
  (*fn_tdssrch_get_timeslot_based_txagc_info)
    (
      tds_l1_timeslot_based_signal_status_ind_type *cm_timeslot_based_signal_strength_info
    );

typedef uint32 
  (*fn_tdsrrc_get_ul_data_rate_for_rab)
    (
       uint8 rab_id,
       sys_modem_as_id_e_type subs_id  /*Assume DSDS is always on for interface*/
    );

/*
 * tx pwr statistics report to data service
 */
typedef void
  (*fn_tdsl1ulcfg_config_modem_power_stats_tx)
    (
      boolean en_pwr_stats
  );

typedef void
  (*fn_tdsl1ulcfg_get_modem_power_stats_tx)
    (
      uint32 *tx_power_bin
  );

/*================== end of update function pointers ================*/
        
        
		

/*  The component then provides a vtable beginning with add_ref, release,
    and qi, followed by the functions it exports.
 */
typedef struct
{
  add_ref_fn  add_ref;
  release_fn  release;
  qi_fn       qi;
  fn_rrc_is_tds_active     rrc_is_tds_active;
  fn_tds_rrc_get_band_from_dl_uarfcn		tds_rrc_get_band_from_dl_uarfcn;
  fn_tds_rrc_rat_is_supported			tds_rrc_rat_is_supported;
  fn_tdsl2_dl_service_register		tdsl2_dl_service_register;
  fn_tdsl2_dl_submit_datamodem_msg		tdsl2_dl_submit_datamodem_msg;
  fn_tdsl2_ul_service_register		tdsl2_ul_service_register;
  fn_tdsl2_ul_submit_datamodem_msg		tdsl2_ul_submit_datamodem_msg;
  fn_tdsrrc_check_cell_barred		tdsrrc_check_cell_barred;
  fn_tdsrrc_delete_classmark_change_handover_info		tdsrrc_delete_classmark_change_handover_info;
  fn_tdsrrc_find_rab_for_rb_in_est_rabs		tdsrrc_find_rab_for_rb_in_est_rabs;
  fn_tdsrrc_get_classmark_change_handover_info		tdsrrc_get_classmark_change_handover_info;
  fn_tdsrrc_get_hsdpa_status		tdsrrc_get_hsdpa_status;
  fn_tdsrrc_get_hsupa_status		tdsrrc_get_hsupa_status;
  fn_tdsrrc_get_mm_cmd_buf		tdsrrc_get_mm_cmd_buf;
  fn_tdsrrc_get_rr_cmd_buf		tdsrrc_get_rr_cmd_buf;
  fn_tdsrrc_get_tc_cmd_buf		tdsrrc_get_tc_cmd_buf;
  fn_tdsrrc_is_arfcn_valid		tdsrrc_is_arfcn_valid;
  fn_tdsrrc_malloc_for_external_cmd		tdsrrc_malloc_for_external_cmd;
  fn_tdsrrc_put_mm_cmd		tdsrrc_put_mm_cmd;
  fn_tdsrrc_put_rr_cmd		tdsrrc_put_rr_cmd;
  fn_tdsrrc_put_tc_cmd		tdsrrc_put_tc_cmd;
  fn_tdsrrc_register_serving_cell_change_ind_cb		tdsrrc_register_serving_cell_change_ind_cb;
  fn_tdsrrc_return_cs_data_call_parms		tdsrrc_return_cs_data_call_parms;
  fn_tdsrrc_return_lc_info_for_rab		tdsrrc_return_lc_info_for_rab;
  fn_tdsrrc_set_ue_in_deep_sleep		tdsrrc_set_ue_in_deep_sleep;
  fn_tdsrrc_update_band_preference		tdsrrc_update_band_preference;
  fn_tdsrrc_vocoder_acquired		tdsrrc_vocoder_acquired;
  fn_tdsrrc_vocoder_control_enabled		tdsrrc_vocoder_control_enabled;
  fn_tdsrrcmeas_get_cell_plmn_info		tdsrrcmeas_get_cell_plmn_info;
  fn_tdsrxd_set_rx_chain_status		tdsrxd_set_rx_chain_status;
  fn_tdssrch_get_freq_syncdlRssi_tx_pwr_info		tdssrch_get_freq_syncdlRssi_tx_pwr_info;

  /*==================================================================== 
    added by hzhi on 05/17/2013 for DIME2.0 CRM_M9625TAAAANAAM200096.1
    -> updated function pointer in vtble
    ====================================================================*/
  fn_tdscdmadiag_change_bler_rate   tdscdmadiag_change_bler_rate;
  fn_tdsrrc_register_rrc_codec_ho_info_cb   tdsrrc_register_rrc_codec_ho_info_cb;
  /*====================================================================
    added by hzhi on 09/20/2013 for DIME3.0 M8974AAAAANAAM200349.
    ====================================================================*/
  fn_tdsl1_api_send_acq_cmd tdsl1_api_send_acq_cmd;
  fn_tdsl1_api_send_setup_cmd tdsl1_api_send_setup_cmd;
  fn_tdsl1_api_send_start_cmd tdsl1_api_send_start_cmd;
  fn_tdsl1_api_send_stop_cmd tdsl1_api_send_stop_cmd;

/*
 * added by hzhi on 10/17/2013 to update DIME3.0 TDS interface. 
 */
  fn_tdsl1_api_send_timetag_cmd tdsl1_api_send_timetag_cmd;

/*
 * added on 01/07/2014
 */
  fn_tdsrrc_get_ul_tx_power_range tdsrrc_get_ul_tx_power_range;
  fn_tdsrrc_get_ul_rate_info tdsrrc_get_ul_rate_info;
  fn_tds_rrc_derive_freq_from_uarfcn tds_rrc_derive_freq_from_uarfcn;

/*
 * added on 01/31/2014 needed by CR 607232
 */
  fn_tdssrch_get_timeslot_based_rxagc_info tdssrch_get_timeslot_based_rxagc_info;
  fn_tdssrch_cm_get_serving_cell_signal__info tdssrch_cm_get_serving_cell_signal__info;
  fn_tdssrch_cm_get_serving_cell_signal_instance_info tdssrch_cm_get_serving_cell_signal_instance_info;
  fn_tdssrch_get_timeslot_based_txagc_info tdssrch_get_timeslot_based_txagc_info;
  /*==================== end of update function pointers ===============*/
/*
added on 02/28/14 for support for CR 599870
*/
  fn_tdsrrc_get_ul_data_rate_for_rab tdsrrc_get_ul_data_rate_for_rab;
//#ifdef FEATURE_DUAL_SIM
  fn_tdsrrc_vocoder_control_enabled_sub tdsrrc_vocoder_control_enabled_sub;
//#endif

  /* for tx pwr statistics report to data service*/
  fn_tdsl1ulcfg_config_modem_power_stats_tx tdsl1ulcfg_config_modem_power_stats_tx;
  fn_tdsl1ulcfg_get_modem_power_stats_tx tdsl1ulcfg_get_modem_power_stats_tx;


} iTDSCDMA_vtbl;



/*  The component provides a macro to get the pointer to its vtable from
    a pointer to an interface (defined in dynload.h).
 */
#define TDSCDMA_vtbl(pi)   ((iTDSCDMA_vtbl *) ((pi)->pVtable))


/*  Finally the component defines interface style macros 
 */
#define ITDSCDMA_tdsl1ulcfg_config_modem_power_stats_tx(pi, en_pwr_stats) (TDSCDMA_vtbl(pi))->tdsl1ulcfg_config_modem_power_stats_tx(en_pwr_stats)

#define ITDSCDMA_tdsl1ulcfg_get_modem_power_stats_tx(pi, tx_power_bin) (TDSCDMA_vtbl(pi))->tdsl1ulcfg_get_modem_power_stats_tx(tx_power_bin)
 
#define ITDSCDMA_rrc_is_tds_active(pi) (TDSCDMA_vtbl(pi))->rrc_is_tds_active()

#define ITDSCDMA_tds_rrc_get_band_from_dl_uarfcn(pi, uarfcn, camped_plmn_id, is_band_valid) (TDSCDMA_vtbl(pi))->tds_rrc_get_band_from_dl_uarfcn(uarfcn, camped_plmn_id, is_band_valid)

#define ITDSCDMA_tds_rrc_rat_is_supported(pi, camped_plmn_id) (TDSCDMA_vtbl(pi))->tds_rrc_rat_is_supported(camped_plmn_id)

#define ITDSCDMA_tdsl2_dl_service_register(pi, srvc_ptr) (TDSCDMA_vtbl(pi))->tdsl2_dl_service_register(srvc_ptr)

#define ITDSCDMA_tdsl2_dl_submit_datamodem_msg(pi, cmd_ptr) (TDSCDMA_vtbl(pi))->tdsl2_dl_submit_datamodem_msg(cmd_ptr)

#define ITDSCDMA_tdsl2_ul_service_register(pi, srvc_ptr) (TDSCDMA_vtbl(pi))->tdsl2_ul_service_register(srvc_ptr)

#define ITDSCDMA_tdsl2_ul_submit_datamodem_msg(pi, cmd_ptr) (TDSCDMA_vtbl(pi))->tdsl2_ul_submit_datamodem_msg(cmd_ptr)

#define ITDSCDMA_tdsrrc_check_cell_barred(pi, req, cpid) (TDSCDMA_vtbl(pi))->tdsrrc_check_cell_barred(req, cpid)

#define ITDSCDMA_tdsrrc_delete_classmark_change_handover_info(pi) (TDSCDMA_vtbl(pi))->tdsrrc_delete_classmark_change_handover_info()

#define ITDSCDMA_tdsrrc_find_rab_for_rb_in_est_rabs(pi, rb_id) (TDSCDMA_vtbl(pi))->tdsrrc_find_rab_for_rb_in_est_rabs(rb_id)

#define ITDSCDMA_tdsrrc_get_classmark_change_handover_info(pi, msg_ptr, msg_len, irat_ho_info_compressed) (TDSCDMA_vtbl(pi))->tdsrrc_get_classmark_change_handover_info(msg_ptr, msg_len, irat_ho_info_compressed)

#define ITDSCDMA_tdsrrc_get_hsdpa_status(pi) (TDSCDMA_vtbl(pi))->tdsrrc_get_hsdpa_status()

#define ITDSCDMA_tdsrrc_get_hsupa_status(pi) (TDSCDMA_vtbl(pi))->tdsrrc_get_hsupa_status()

#define ITDSCDMA_tdsrrc_get_mm_cmd_buf(pi) (TDSCDMA_vtbl(pi))->tdsrrc_get_mm_cmd_buf()

#define ITDSCDMA_tdsrrc_get_rr_cmd_buf(pi) (TDSCDMA_vtbl(pi))->tdsrrc_get_rr_cmd_buf()

#define ITDSCDMA_tdsrrc_get_tc_cmd_buf(pi) (TDSCDMA_vtbl(pi))->tdsrrc_get_tc_cmd_buf()

#define ITDSCDMA_tdsrrc_is_arfcn_valid(pi, UARFCN) (TDSCDMA_vtbl(pi))->tdsrrc_is_arfcn_valid(UARFCN)

#define ITDSCDMA_tdsrrc_malloc_for_external_cmd(pi, size) (TDSCDMA_vtbl(pi))->tdsrrc_malloc_for_external_cmd(size)

#define ITDSCDMA_tdsrrc_put_mm_cmd(pi, cmd_buf) (TDSCDMA_vtbl(pi))->tdsrrc_put_mm_cmd(cmd_buf)

#define ITDSCDMA_tdsrrc_put_rr_cmd(pi, cmd_buf) (TDSCDMA_vtbl(pi))->tdsrrc_put_rr_cmd(cmd_buf)

#define ITDSCDMA_tdsrrc_put_tc_cmd(pi, cmd_buf) (TDSCDMA_vtbl(pi))->tdsrrc_put_tc_cmd(cmd_buf)

#define ITDSCDMA_tdsrrc_register_serving_cell_change_ind_cb(pi, cb_ptr) (TDSCDMA_vtbl(pi))->tdsrrc_register_serving_cell_change_ind_cb(cb_ptr)

#define ITDSCDMA_tdsrrc_return_cs_data_call_parms(pi, rab_id, cs_data_parms_ptr) (TDSCDMA_vtbl(pi))->tdsrrc_return_cs_data_call_parms(rab_id, cs_data_parms_ptr)

#define ITDSCDMA_tdsrrc_return_lc_info_for_rab(pi, ptr) (TDSCDMA_vtbl(pi))->tdsrrc_return_lc_info_for_rab(ptr)

#define ITDSCDMA_tdsrrc_set_ue_in_deep_sleep(pi, value) (TDSCDMA_vtbl(pi))->tdsrrc_set_ue_in_deep_sleep(value)

#define ITDSCDMA_tdsrrc_update_band_preference(pi, band_pref_mask) (TDSCDMA_vtbl(pi))->tdsrrc_update_band_preference(band_pref_mask)

#define ITDSCDMA_tdsrrc_vocoder_acquired(pi ) (TDSCDMA_vtbl(pi))->tdsrrc_vocoder_acquired()

#define ITDSCDMA_tdsrrc_vocoder_control_enabled(pi) (TDSCDMA_vtbl(pi))->tdsrrc_vocoder_control_enabled()

#define ITDSCDMA_tdsrrcmeas_get_cell_plmn_info(pi, plmn_cell_info_ptr) (TDSCDMA_vtbl(pi))->tdsrrcmeas_get_cell_plmn_info(plmn_cell_info_ptr)

#define ITDSCDMA_tdsrxd_set_rx_chain_status(pi, tdsl1_cmapi_if_rx_status_type) (TDSCDMA_vtbl(pi))->tdsrxd_set_rx_chain_status(tdsl1_cmapi_if_rx_status_type)

#define ITDSCDMA_tdssrch_get_freq_syncdlRssi_tx_pwr_info(pi, cm_freq_DwPTSRssi, cm_tx_pwr_info) (TDSCDMA_vtbl(pi))->tdssrch_get_freq_syncdlRssi_tx_pwr_info(cm_freq_DwPTSRssi, cm_tx_pwr_info)


/*==================================================================
 added by hzhi on 05/17/2013 for DIME2.0 CRM_M9625TAAAANAAM200096.1
 -> updated function pointer in vtble
 ===================================================================*/
#define ITDSCDMA_tdscdmadiag_change_bler_rate(pi, req_pkt, pkt_len) (TDSCDMA_vtbl(pi))->tdscdmadiag_change_bler_rate(req_pkt, pkt_len)

#define ITDSCDMA_tdsrrc_register_rrc_codec_ho_info_cb(pi, cb_ptr) (TDSCDMA_vtbl(pi))->tdsrrc_register_rrc_codec_ho_info_cb(cb_ptr)

  /*====================================================================
    added by hzhi on 09/20/2013 for DIME3.0 M8974AAAAANAAM200349.
    ====================================================================*/
/*
#define ITDSCDMA_KeySchedule(pi, k) (TDSCDMA_vtbl(pi))->KeySchedule(k)
*/
#define ITDSCDMA_tdsl1_api_send_acq_cmd(pi, msg) (TDSCDMA_vtbl(pi))->tdsl1_api_send_acq_cmd(msg)
#define ITDSCDMA_tdsl1_api_send_setup_cmd(pi, msg) (TDSCDMA_vtbl(pi))->tdsl1_api_send_setup_cmd(msg)
#define ITDSCDMA_tdsl1_api_send_start_cmd(pi) (TDSCDMA_vtbl(pi))->tdsl1_api_send_start_cmd()
#define ITDSCDMA_tdsl1_api_send_stop_cmd(pi) (TDSCDMA_vtbl(pi))->tdsl1_api_send_stop_cmd()

/*
 * added by hzhi on 10/17/2013 to update DIME3.0 TDS interface. 
 */
#define ITDSCDMA_tdsl1_api_send_timetag_cmd(pi,allow_sleep_timetag) \
        (TDSCDMA_vtbl(pi))->tdsl1_api_send_timetag_cmd(allow_sleep_timetag)

/*
 * added on 01/07/2014
 */
#define ITDSCDMA_tdsrrc_get_ul_tx_power_range(pi) \
        (TDSCDMA_vtbl(pi))->tdsrrc_get_ul_tx_power_range()
#define ITDSCDMA_tdsrrc_get_ul_rate_info(pi, ul_rate_info_ptr) \
        (TDSCDMA_vtbl(pi))->tdsrrc_get_ul_rate_info(ul_rate_info_ptr)
#define ITDSCDMA_tds_rrc_derive_freq_from_uarfcn(pi, uarfcn, freq) \
        (TDSCDMA_vtbl(pi))->tds_rrc_derive_freq_from_uarfcn(uarfcn, freq)

/*
 * added on 01/31/2014 needed by CR 607232
 */
#define ITDSCDMA_tdssrch_get_timeslot_based_rxagc_info(pi, cm_timeslot_based_signal_strength_info) \
        (TDSCDMA_vtbl(pi))->tdssrch_get_timeslot_based_rxagc_info(cm_timeslot_based_signal_strength_info)
#define ITDSCDMA_tdssrch_cm_get_serving_cell_signal__info(pi, cm_serving_cell_signal_strength_info) \
        (TDSCDMA_vtbl(pi))->tdssrch_cm_get_serving_cell_signal__info(cm_serving_cell_signal_strength_info)
#define ITDSCDMA_tdssrch_cm_get_serving_cell_signal_instance_info(pi, cm_serving_cell_signal_strength_info) \
        (TDSCDMA_vtbl(pi))->tdssrch_cm_get_serving_cell_signal_instance_info(cm_serving_cell_signal_strength_info)
#define ITDSCDMA_tdssrch_get_timeslot_based_txagc_info(pi, cm_timeslot_based_signal_strength_info) \
        (TDSCDMA_vtbl(pi))->tdssrch_get_timeslot_based_txagc_info(cm_timeslot_based_signal_strength_info)

/*
added on 02/28/14 for support for CR 599870
*/
#define ITDSCDMA_tdsrrc_get_ul_data_rate_for_rab(pi, rab_id, subs_id) (TDSCDMA_vtbl(pi))->tdsrrc_get_ul_data_rate_for_rab(rab_id, subs_id)
/*================ end of updated function pointers ================*/
//#ifdef FEATURE_DUAL_SIM
#define ITDSCDMA_tdsrrc_vocoder_control_enabled_sub(pi,as_id) (TDSCDMA_vtbl(pi))->tdsrrc_vocoder_control_enabled_sub(as_id)
//#endif
#endif /* _ITDSCDMA_H_ */
