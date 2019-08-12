#ifndef FTM_GSM_RFCTL_H
#define FTM_GSM_RFCTL_H

/*=======================================================================================

                    G S M   R F   C O N T R O L   F U N C T I O N S

DESCRIPTION
   This file contains implementations for FTM GSM RF control test functions
   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.

========================================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/ftm_gsm_rfctl.h#1 $
  $DateTime: 2016/12/13 07:59:51 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/27/16   mz      FR 30990: SAWless LTE TDD B39: enable IIP2 CAL on GSM SAW device
10/13/15   bp      made get_frame_sweep_cal_running accepting an rfm device as argument.
09/14/15   par     Fix crash in FTM GSM deact sequence
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx
                   burst along with GRFC status
11/13/14   ck      add gsm diversity switch
10/30/14   ck      add parameter for hooking ftm concurrency manager for gsm diversity
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
08/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/14/14   ec      Function prototypes for SAWLess control interface
06/10/14   hk      sub_id interface in FTM mode
05/29/14   sb      IP2 cal bug fixes, updates
04/22/14   ggs     Port CR636669: Set flag if frame sweep calibration is running.
04/16/14   sb      IP2 cal Interface and functionality changes
02/27/14   tsr     GSM receive diversity
08/07/13   ec      Add declaration of ftm_gsm_init_iqcapture_buffer to fix compiler
                   warning
05/06/13   aka     Added frame_sweep_header_ptr in GSM FTM handle
04/17/13   aka     Changes to support predist_override
04/02/13   aka     Support for band change in frame sweep functionality.
03/28/13   aka     GSM frame sweep feature changes.
02/27/13   tws     Move static variables to device specific structure.
02/21/13   tws     Return the ftm gsm vars pointer after init.
02/15/13   sb      Added FTM hook to override tuner tune code
02/04/13   tws     Update for geran_test_mode_api change.
01/29/13   ec      Add members to ftm common variables for Non Signalling
01/17/12   aka     Changes to enable freq offset setting for multiple chains
12/10/12   tws     Path delay variables moved to core.
12/06/12   tws     Fix Klocwork errors.
11/27/12   tws     Add device id to FTM.
11/07/12   ec	   Fix compiler warning due to unused prototype
10/01/12   sb      Removed GRFC engine references in Dime code
08/17/12   ec	   Fixing Compilation Error
08/09/12   ec      Checked in front end for first part of FTM tx refactoring, certain commands
                   will now signal RF through MSGR if RF Task is defined.
07/18/12   ec      Added new set_tx_pow functionality, to use MSGR to comm with RF Task
06/25/12   ggs     Added ftm_gsm_rf_task_set_pa_range prototype
05/03/12   zc      Thread protection for ftm_rf_mode commands
04/01/09   sr      include file cleanup
08/09/07   jfc     Mainlining T_MSM6280 feature
03/04/05   rsr     added ant_sel3  definitions
09/27/04    et     added || T_MSM6500 to #defines
09/19/03   lcl     Created.

=============================================================================*/

/*===========================================================================
                            INCLUDES
=============================================================================*/
#include "comdef.h"
#include "ftm.h"
#include "ftm_gsm.h"
#include "rfgsm_core_types.h"
#include "rfgsm_mdsp_cal.h"
#include "gl1_defs.h"
#include "ftm_gsm_ber.h"
#include "ftm_egprs_ber.h"
#include "rfgsm_ftm_msg.h"
#include "rfcom.h"

extern boolean override_gsm_port_to_sawless;
#define ALTPATH_FOR_IIP2_ON_SAW_RFC 1

#define RFGSM_SLOT_LOCK(rfgsm_mutex_ptr) rf_common_enter_critical_section((rfgsm_mutex_ptr))
#define RFGSM_SLOT_UNLOCK(rfgsm_mutex_ptr) rf_common_leave_critical_section((rfgsm_mutex_ptr))


#define FTM_GSM_RFCTL_SIGNAL_INFO_ARR_SIZE  (5)
#define FTM_GSM_RFCTL_SIGNAL_INFO_MEM_SIZE  (FTM_GSM_RFCTL_SIGNAL_INFO_ARR_SIZE * sizeof(gl1_hw_rx_signal_type))
#define FTM_LINEAR_PA_TX_GAIN_MEM_SIZ  (RFGSM_MAX_TX_SLOTS_PER_FRAME * sizeof(rfgsm_GfwTxRfGainParam))
#define FTM_GRFC_AUX_TIMING_MEM_SIZ    (FTM_GRFC_AUX_MAX * sizeof(ftm_grfc_timing_offset_type))


#define FTM_MAX_GSM_SLOT_PER_FRAME    8
#define FTM_MAX_GSM_TX_SLOT           4
#define FTM_MAX_GSM_BURST_TAGS        3

typedef struct
{
   int16 final_i_code;
   int16 final_q_code;
   int32 min_im2_val;
   boolean status;
}rfgsm_ip2_cal_results_type;


typedef struct
{
  uint8                slot_num;
  boolean              on_off;
  int16                powdBm;
  gl1_hw_channel_type  mcs;
  gl1_hw_tx_tags_type  tags;
  uint8                pa_range_r0_val;
  uint8                pa_range_r1_val;
} ftm_slot_type;

typedef struct
{
  /* Receive signal strength */
  uint32 ftm_rssi;
  /* I/Q DC offset */
  uint32 ftm_i_dc_offset;
  uint32 ftm_q_dc_offset;
  /* Counter for number of frames RSSI is accumulated over */
  uint32 ftm_rssi_counter;
 /* RXD Receive signal strength */
  uint32 ftm_rssi_drx;
  uint32 ftm_i_dc_offset_drx;
  uint32 ftm_q_dc_offset_drx;
  uint32 ftm_rssi_counter_drx;
  /* Counter for the transition RX frame count */
  uint8 transit_rx_frame_cnt;
  uint8 burst_num;

} ftm_gsm_rx_cmd_rx_burst_metrics_type;



typedef struct
{
  boolean do_ip2_cal;  // Status for IP2 cal
  uint8 num_frames;   // Number of iterations of IP2 cal requested
  uint8 start_frame_ctr;  // Counter to track number of frames from IP2 cal start
  uint8 num_cal_steps;  // Number of IP2 cal steps
  uint16 step_duration_qs;  // duration of an IP2 al step
  uint8 num_frames_per_iter;  // Number of iterations, IP2 cal is requested for
  uint16 dc_settling_time_qs;
  boolean use_default_params;
  uint8  band_in_use;
}ftm_gsm_ip2_cal_type;


/*! @brief This structure holds the variables for GSM RF calibration */
typedef struct
{
  rfm_device_enum_type rfm_dev;

  uint8 sub_id;

  ftm_gsm_state_type ftm_gsm_state;

  sys_band_T ftm_gl1_hw_gsm_band;

  boolean ftm_gsm_idle_proc_flag;

  boolean gsm_frame_sweep_enabled;

  uint16 frame_sweep_fn_count;

  ftm_gsm_frame_sweep_rx_tag_type  gsm_frame_sweep_tags[FTM_MAX_GSM_BURST_TAGS];

  uint8 rx_slot_num;

  uint8 gsm_frame_sweep_paused_count;

  uint8 *frame_sweep_header_ptr;

  volatile uint32 ftm_frame_count;

  ftm_gsm_rx_cmd_rx_burst_metrics_type burst_metrics;

  /* OPLL BW related stuff... temporary until these values are fixed*/
  /* whether it's continuous Tx */
  boolean ftm_cont_tx;

  /* whether it's continuously issued Rx bursts*/
  boolean ftm_cont_rx;

  /* the number of tx bursts */
  uint32  ftm_num_bursts_tx;

  /* the number of rx bursts */
  uint32  ftm_num_bursts_rx;

  /* Kv Cal V2 - Global Variables */
  uint16 ftm_gsm_rfctl_kv_cal_per_tx_on;
  uint16 ftm_gsm_rfctl_kv_tx_on_off;
  boolean ftm_gsm_rfctl_kv_cal_v2_state;

  boolean do_multi_slot_rx_cal_flag;

  /* mDSP Rx debug buffer */
  gl1_defs_metrics_rpt gsm_rx_burst_metrics;
  gl1_msg_metrics_rpt  gsm_rx_burst_msg_rpts[FTM_MAX_GSM_TX_SLOT];
  gl1_msg_metrics_rpt* gsm_rx_burst_msg_ptrs[FTM_MAX_GSM_TX_SLOT];

  gl1_hw_rx_tags_type  ftm_gsm_rx_tags[FTM_MAX_GSM_BURST_TAGS];
  uint16               tags_rd_index;
  uint16               tags_wr_index;


  gl1_defs_rx_pkt_metrics_type gsm_rx_multislot_metrics[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS];
  gl1_defs_rx_pkt_metrics_type *gsm_rx_multislot_metrics_ptr[GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS];

  gl1_hw_rx_signal_type  signal_info[FTM_GSM_RFCTL_SIGNAL_INFO_ARR_SIZE];
  gl1_hw_rx_tags_type  ftm_gsm_rx_multislot_tags[3][GERAN_API_MAX_GL1_DEFS_MAX_DL_MSGS];

  boolean average_rssi_available;

  uint16 num_rx_slots;
  boolean multislot_rx_on;

  boolean tx_cont_on;

  boolean tx_frame_matrix_persistent;

  int16 last_power[FTM_MAX_GSM_SLOT_PER_FRAME];

  rfa_rf_gsm_ftm_set_pa_range_type_rsp_s  rfa_rf_gsm_ftm_set_pa_range_rsp;

  rfa_rf_gsm_ftm_set_tx_pow_dbm_type_rsp_s  rfa_rf_gsm_ftm_set_tx_pow_dbm_rsp;

  rfa_rf_gsm_ftm_tuner_override_type_rsp_s  rfa_rf_gsm_ftm_tuner_override_rsp;

  rf_gain_T ftm_gain_range;
  rf_gain_T ftm_gain_range_drx;

  uint16  data_mod;
  volatile uint16 ftm_FNmod4;

  gl1_hw_tx_signal_type ftm_tx_signal[FTM_MAX_GSM_TX_SLOT];

  ftm_slot_type ftm_slot_assign[FTM_MAX_GSM_SLOT_PER_FRAME];

  /*! @brief No of frames over which RX RSSI is averaged. */
  uint8 ftm_gsm_rssi_no_frames_avg;

  boolean ftm_gsm_lna_range_in_rx_lvl_parm;

  boolean ftm_gsm_rfctl_reset_rssi;

  //for burst metrics
  int32 local_frame_cnt;
  boolean wait_for_next_lna_range;
  int8 next_lna_range_order;
  uint32 first_call_fn;

  /*! @brief inittialize the sequence of lna ranges in rx cal for a particular band and RX level */
  uint16 ftm_gsm_next_lna_range[FTM_MAX_GSM_SLOT_PER_FRAME];
  boolean ftm_gsm_lna_range_ind_flag;
  uint8 frames_to_wait;

  /*! @brief Flags used to specify when to start sending rx burst commands to MDSP and when to stop */
  uint8 beginning_rx;
  uint8 stop_rx;

  /*! @brief Flag used to enable stop Tx count*/
  uint8 stop_tx_triggered;

  int32 ftm_gsm_tx_freq_offset;

  /*! @brief flag to reset the transition RX frame count in the burst metrics function */
  boolean ftm_gsm_rfctl_reset_transit_rx_frame_cnt;

  /*! @brief to check whether Rx Burst has started */
  boolean rxBurstOn;

  /*! @brief TX burst parameters */
  ftm_tx_burst_type tx_brst;

  boolean gsm_tx_cal_start;

  boolean do_tx_da_cal;

  ftm_gsm_ip2_cal_type ip2_cal_data;

  /* flag to indicate whether the FTM cal is in progress or not */
  boolean ftm_gsm_cal_flag;

  boolean ftm_gsm_predist_override;

  boolean gl1_freq_error_saved;
  int32 gl1_freq_error;
  int32 prev_freq_error;
  int32 new_freq_error;

  /* pointer to the TX command function */
  void (*ftm_tx_funct)(rfm_device_enum_type rfm_dev );
  /* pointer to the TX stop command function */
  void (*ftm_tx_stop_funct)(rfm_device_enum_type rfm_dev );
  /* pointer to the RX command function */
  void (*ftm_rx_funct)(rfm_device_enum_type rfm_dev );

  /* pointer to the ISR function */
  void (*ftm_tdma_isr_fnc)( sys_modem_as_id_e_type );

  /*! @brief MDSP cal variables */
  rfgsm_mdsp_cal_variables_s mdsp_cal_vars;

  /*! @brief Variables and queues for Non-Signalling */
  /* Used to map this device instance to a GL1 as_id - mapped at init */
  sys_modem_as_id_e_type                 ns_as_id;

  /* Status of GSM BER */
  ftm_gsm_ber_status_type                ftm_gsm_ber_status;

  /* current state of FTM EGPRS scenario */
  ftm_egprs_ber_state_type           current_egprs_state;
  ftm_egprs_ber_reconfig_state_type  current_egprs_reconfig_state;

  /* Buffer for DL and UL TBF configuration parameters that are controllable via FTM interface */
  ftm_egprs_ber_dl_tbf_status_type   current_egprs_dl_tbf_info;
  ftm_egprs_ber_ul_tbf_status_type   current_egprs_ul_tbf_info;

  /* Buffers the DL and UL TBF configurations that get sent to L1 */
  mac_dynamic_tbf_information_T      current_egprs_l1_dynamic_tbf_info;
  mac_l1_dl_tbf_config_t             current_egprs_l1_dl_tbf_info;

  /* ftm mode id */
  ftm_mode_id_type ftm_mode_id;

  /*diversity enabled */
  boolean diversity_on;

  /*! @brief New flag that indicates when we enter in FTM GSM mode.
  Flag set as FALSE when FTM GSM exit is done */
  boolean ftm_gsm_mode_enter;

} ftm_gsm_rfctl_vars_s;



/*================================================================================

                            Function Prototypes

================================================================================ */

ftm_gsm_rfctl_vars_s * ftm_gsm_get_vars_ptr( rfm_device_enum_type rfm_dev );

ftm_gsm_rfctl_vars_s * ftm_gsm_create_and_init_vars( rfm_device_enum_type rfm_dev );

boolean ftm_gsm_vars_created( rfm_device_enum_type rfm_dev );

void ftm_gsm_free_vars( rfm_device_enum_type rfm_dev );

void ftm_gsm_init_ftm_slot_mutex(void);

int8 ftm_gsm_set_tx_pow_dbm ( rfm_device_enum_type rfm_dev, int16 pow_dbm, int8 index);

void ftm_gsm_set_path_delay( rfm_device_enum_type rfm_dev, uint16 input_delay_value );

void ftm_gsm_set_band ( int8 gsm_band );

boolean ftm_gsm_cal_mode_enabled(rfm_device_enum_type rfm_dev);

boolean ftm_gsm_get_predist_override(rfm_device_enum_type rfm_dev);

void ftm_gsm_rf_task_set_band ( int8 gsm_band );

boolean ftm_gsm_get_lna_range_from_rx_lvl( rfm_device_enum_type rfm_dev );

void ftm_get_active_slot_list( rfm_device_enum_type rfm_dev,
                               uint8* num_active_slots,
                               uint8* active_slot_list );

void ftm_gsm_set_ave_rssi_update_flag( rfm_device_enum_type rfm_dev,
                                       boolean update );

boolean ftm_gsm_ber_create_and_init_vars( ftm_gsm_rfctl_vars_s * ftm_vars_p );


uint32 ftm_gsm_rf_task_tuner_override ( rfm_device_enum_type rfm_dev,
                                        uint8 override_flag,
                                        void* data,
                                        uint8* tuner_nv_ptr,
                                        uint8* tuner_id_ptr);

void ftm_gsm_init_iqcapture_buffer( rfm_device_enum_type rfm_dev );

boolean ftm_gsm_set_ip2_cal_params_override ( rfm_device_enum_type rfm_dev,
                                     ftm_pkt_type* cmd_ptr,
                                     ftm_rsp_pkt_type* rsp_pkt_ptr );

void ftm_gsm_do_ip2_cal(rfm_device_enum_type rfm_dev);

void ftm_gsm_reset_ip2_cal_vars(ftm_gsm_rfctl_vars_s * vars);

boolean ftm_gsm_set_ip2_cal_params_default(rfm_device_enum_type rfm_dev, uint8 rfcom_band);

boolean ftm_gsm_get_ip2_cal_results(rfm_device_enum_type rfm_dev,
                                    uint8 rfcom_band,
                                    uint8 flag_mask,
                                    rfgsm_ip2_cal_results_type *,
                                    boolean cal_end);

boolean ftm_gsm_get_frame_sweep_cal_running(rfm_device_enum_type rfm_dev);

boolean ftm_gsm_sawless_lin_override( rfm_device_enum_type rfm_dev,
                                      ftm_gsm_sawless_lin_type* lin_params );

boolean ftm_gsm_get_sawless_lin_mode( rfm_device_enum_type rfm_dev,
                                      ftm_gsm_sawless_lin_type* lin_params );

boolean ftm_gsm_set_diversity(rfm_device_enum_type rfm_dev, word on_off );


rfcom_gsm_band_type ftm_gsm_convert_ftm_mode_id_type_to_rfcom(ftm_mode_id_type mode);

void ftm_gsm_set_grfc_status_read_enable( rfm_device_enum_type rfm_dev,
                                          rf_burst_type* burst_type, boolean enable);

void ftm_gsm_set_device_status_enable( rfm_device_enum_type rfm_dev,
                                       rf_burst_type* burst_type, boolean enable);

void ftm_gsm_get_device_status_reg_list( rfm_device_enum_type rfm_dev,
                                         ftm_read_device_status* device_status);

#endif /* FTM_GSM_RFCTL_H */
