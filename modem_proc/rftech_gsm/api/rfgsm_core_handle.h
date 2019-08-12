/*!
   @file
   rfgsm_core_handle.h

   @brief

   @details

*/
/*=============================================================================
Copyright (c) 2008-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_core_handle.h#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why

--------   ---     ----------------------------------------------------------
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
01/28/15   mz      Fix wrong jdet_counter reset when rx band changes
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/16/14   ec      Add flags for new debug NV mechanism
1/13/15    sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
01/12/14   sml     Add Flag for GL1 to indicate that we are in probe burst
11/28/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/25/14   sb      HL/LL algorithm changes and bug fixes
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
10/27/14   sc      Fix sub mask being manipulated outside of enter-exit mutex
10/17/14   rr      Separate rx/tx alternate path core handles
10/08/14   ch      Store GMSK flag for skipping CL tuner 
10/02/14  ec/cpm   Add specific override for when SAWless Linearity override is applied to a channel
09/23/14   zbz     changes to support GSM and LTE/TDS coexistence related RFC changes
09/23/14   sw      Boolean to indicate ASD scripts to be built only once (and deprecate
                   skip_tuner_program)
09/19/14   sc      Force RxLM update for every wakeup
09/12/14   ec      Changed JDET thresholds to signed values
09/11/14   sw      Remove ASDiv featurisation to avoid data misalignments
08/26/14   zbz     Reducing number of tuner writes during ASDIv
08/22/14   ec      Added num_table_entries to track HL table entries allocated and trap out of bounds access
08/01/14   ec      Add enhanced override type and HL rx switch point table for SAWless targets
07/31/14   tsr     Alternate path support
07/30/14   hoh     Add enhanced rx feature support 
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/14/14   ec      Add variables for SAWLess Linearity Switching, per band SAWless support
06/14/14   sb      Add hooks for sawless support
06/12/14   ry      Backout rfgsm_core_therm_read_type changes
06/11/14   sc/ggs  Add flag to check if rxlm settings update is required during
                   wake up
06/10/14   hk      Edge core parameter from GL1.
06/06/14   sb      Reduce GSM heap memory usage
05/29/14   ry      Added parameter to rfgsm_core_therm_read_type to block simultaneous therm reads
06/05/14   ggs     AMAM/AMPM tables should only be recalculated 
                   for GSM when modulation type is 8PSK
04/24/14   ggs     Port CR581892: Add parameters to core_handle to track 
                   previous backoffs from GL1
04/16/14   sb      IP2 cal Interface and functionality changes
03/27/14   ch      Remove obsolete tunerID from core handles
03/26/14   tsr     Add support to query rfm device for TX or RX capability 
03/26/14   sb      Add support for IP2 cal for Dime PM
03/24/14   sc      Remove dynamic task priority manipulation
03/19/14   hoh     Added support for 8 PA gain states
03/18/14   sc      Add subscription based execution intelligence
03/10/14   zbz     Add feature to control pa icq bias through NV item
03/04/14   sw      Introduce new structure for ASD response
02/25/14   ggs     Port CR5082237: Add rfgsm_gsm_multislot_tx_pwr_limit and 
                   rfgsm_edge_multislot_tx_pwr_limit in rfgsm_core handles so
                   that it can hold adjusted values of Tx max pwr limit.
02/17/14   sc      Clean up mDSP APIs
02/10/14   cj      rfgsm_buffer flag added to decide whether to use home/away buffer
01/14/14   av      Delay Calibration and npler based delay support
12/12/13   sc      Add modem-specific max AMAM/AMPM table entries
10/28/13   ggs     Add Ping-pong AMAM AMPM ID to core handle
10/23/13   sg      Added new flag to know if wakeup was done before the start of GSM burst
10/14/13   sc      Mainline core DM and RFLM code 
10/14/13   sml     Limiting the number of LNA ranges to five
                   depending upon a flag which needs to be 
                   updated by WTR 
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34) 
10/03/13   sc      RFLM DM interface data and dynamic event development
10/02/13   tws     QPOET GSM Optimisations. 
10/01/13   sb      Disable SGLTE Coexistence filtered path cal data 
09/17/13   sb      Fix check for right rx burst type 
09/17/13   sb      Support for SGLTE/SGTDS coexistence 
09/12/13   sc      RFLM interface encompassed by core DM 
08/28/13   ec      Creating RF GSM Core state machine with functions to init and
                   drive
08/05/13   sc      Add PA ramp management core handle definitions
07/31/13   av      GBTA support for both GL1 sub1 and GL1 sub2 call RF to build RX scripts
07/30/13   pl      Enable new therm read interface
07/29/13   sml     Generic RFSW-FW interaction to generate an ACK for a CCS event
07/29/13   sml/svi ASD support for GSM tech
07/18/13   sc      Update RFLM data information (featurised)
07/12/13   av      GTA EXIT mode 
06/06/13   ec	   Move mc_state to core handle.
05/30/13   tws     Move the therm_read struct from rfgsm_core_types.h. Add
                   the UMID to be used to trigger therm_read.
04/22/13   sb      Add FTM hooks to override QPOET bias 
04/09/13   sr      removed legacy antenna tuner code which is not used.
03/28/13   aka     GSM frame sweep feature changes.
03/27/13   sb      Added support for Enhanced APT 
03/18/13   sc      Cleanup and comment core handle vars, grouping init once vars
02/28/13   tws     Move temp and vbatt read timers to core handles.
02/22/13   tws     Make the IQ capture buffer dynamic.
02/11/13   sc      Add GSM mode flag enum keeping track of iRAT/Standalone mode
01/30/13   sc      Add RF Task Priority adjust handle vars
01/15/13   sc      Add device token for iRAT abort mechanism 
01/14/13   tsr     Added changes for therm read implementation 
01/11/13   aka     Added support for simultaneous Tx SAR
01/03/12   sc      Added therm read full call flow framework  
12/11/12   sb      Add hooks for GSM APT  
12/10/12   tws     Move global variables to the gsm core handler.
11/28/12   aka     Added new fields in rfgsm_core_handle enum
11/26/12   aka     Fixing some OFT warnings
11/26/12   aka     Updated rfgsm_core_handle_type with new elements
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
07/26/12   sr      Initial version 
============================================================================*/

#ifndef RFGSM_CORE_HANDLE_H
#define RFGSM_CORE_HANDLE_H

#include "rfgsm_core_types.h"
#include "rfgsm_nv_cmn.h"
#include "rfc_card_gsm.h"
#include "ftm_gsm.h"
#include "msgr.h"

#include "rfgsm_msg.h"
#include "rfcom.h"

#include "rfgsm_core_log.h"

#define RFGSM_CORE_HANDLES_MAX 2
#define RFGSM_MAX_IQ_BUFFER_LEN 32768
#define RFGSM_DEVICE_STATUS_READ_MAX_REGS 8
#define RFGSM_DEVICE_STATUS_MAX_BURST_TYPE 2
#define MAX_NUM_ARFCNS_IN_BAND 374

/*----------------------------------------------------------------------------*/


typedef struct
{
    /*! @brief The device this asd switch is associated with */
  rfm_device_enum_type rfm_dev;

  /*! @brief L1 callback function */
  rfgsm_l1_cb_type l1_cb;

  /*! Subscription ID to be returned to GL1 */
  uint8 sub_id;

  /*! @brief the switch position */
  uint8 set_antenna_position;

} rfgsm_core_asd_switch_event_info_type;

/*----------------------------------------------------------------------------*/

typedef uint8 sawless_lin_table_entry;

/*----------------------------------------------------------------------------*/
typedef struct
{
  /* jdet_high_thresh - readings above this level indicate strong jammer;
     in idle mode we will switch to HL mode regardless of other metrics*/
  int16  jdet_high_thresh; 
  /* jdet_med_thresh - readings above this level indicate moderate jammer; 
     we may switch to HL if other metrics indicate SNR impact */
  int16  jdet_med_thresh;
  /* snr_thresh - if the SNR is below this level we may switch to HL
     to mitigate a jammer */
  uint16  snr_thresh;
  /* rssi_less_snr_thresh_idle - if the (RSSI - SNR) is below this level
     we may switch to HL in the presence of a jammer */
  dBx16_T rssi_less_snr_thresh_idle; 
  /* rssi_thresh_pscan - if the RSSI is below this level we may switch 
      to HL in the presence of a jammer (SNR is not measured in pscan) */
  dBx16_T rssi_thresh_pscan; 
} rfgsm_core_rx_lin_metrics_thresholds_type;
/*----------------------------------------------------------------------------*/

typedef enum
{
  RFGSM_SAWLESS_NO_OVERRIDE = 0, //No override is applied, HL table and algorithm used
  RFGSM_SAWLESS_HL_OVERRIDE, //Global HL override
  RFGSM_SAWLESS_LL_OVERRIDE, //Global LL override
  RFGSM_SAWLESS_CHAN_OVERRIDE, //Specific channels have been overriden in the HL table,
                               //do not update using the algorithm
  RFGSM_SAWLESS_OVERRIDE_MAX_VAL
} rfgsm_core_sawless_override_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfgsm_core_rx_lin_metrics_thresholds_type linearity_switching_thresholds;
  uint8*                                    rx_hl_flag_table;
  uint8                                     num_table_entries;
  rfgsm_core_sawless_override_type          linearity_override;
  boolean                                   enable_logging;
  boolean                                   ignore_jdet_in_algorithm;
  boolean                                   disable_hl_power_scan;
  boolean                                   disable_jdet_configuration;
} rfgsm_core_sawless_linearity_data_type;

/*----------------------------------------------------------------------------*/


typedef struct
{
  /*! @brief The device this thermistor is associated with */
  rfm_device_enum_type rfm_dev;

  /*! @brief shared memory pointer for scratch memory */
  void* therm_read_shared_mem_ptr;

  /*! @brief UMID used to trigger thermistor read in GFW */
  msgr_umid_type fw_therm_read_umid;
  
  /*! @brief UMID used to get GFW response*/
  msgr_umid_type fw_therm_rsp_umid;
  
  /*! @brief incrementing tag to align therm triggers and results */
  uint8  event_handle_tag;

  void*   event_handle; 

  /*! @brief previous raw thermistor read */
  uint16  prev_therm_read;      

  /*! @brief  current raw thermistor read */
  uint16  curr_therm_read;      

  /*! @brief raw thermistor read from RF device driver */
  uint16 raw_therm;                

  boolean rfgsm_core_temp_comp_suspended;

  rf_lock_data_type crit_sect;

  boolean enable_logging;

} rfgsm_core_therm_read_type;

typedef struct
{
  ftm_iqcapture_type capture_type;

  /* Number of iq samples copied*/
  int32 total_iq_samples_copied;

  /*static buffer for IQ capture*/
  int16 rfgsm_iq_buffer[RFGSM_MAX_IQ_BUFFER_LEN];
} rfgsm_core_iq_capture_type;


typedef struct
{
  boolean enable;
  rf_hal_bus_resource_type resource_type[RFGSM_DEVICE_STATUS_READ_MAX_REGS]; 
  uint16 address[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  uint8 slave_id[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  uint8 channel[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  uint8 half_rate[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  uint8 rd_delay[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  boolean extended_cmd[RFGSM_DEVICE_STATUS_READ_MAX_REGS];
  uint8 num_regs;
  boolean enable_grfc_read;
}rfgsm_core_read_device_status_params;

typedef struct
{
  //rf_burst_type burst_type;
  uint8 event_tag;
  msgr_umid_type device_status_rsp_umid;
  rfgsm_core_read_device_status_params params[RFGSM_DEVICE_STATUS_MAX_BURST_TYPE];
}rfgsm_core_read_device_status;



typedef struct
{
  /*************************************************************************/
  /**************************** INIT ONCE VARIABLES ************************/
  /* Flag used to only initialise some variables once */
  boolean init_once;

  /* GSM Standalone = RFGSM_STANDALONE_MODE; GSM iRAT = RFGSM_IRAT_MODE; Other = RFGSM_DEFAULT_MODE */
  rfgsm_mode_type rfgsm_mode;

  /*Flag to denote whether filtered path should be used (based on appropriate channel) or not. Ideally this flag should be set to FALSE in a coexistence scenario.*/
  boolean use_coex_filtered_path_tx;

  boolean use_coex_filtered_path_rx;

  /* Pointers to all the RF GSM script buffers */
  rfgsm_mdsp_scripts_ptrs scripts;

   /* Log for RF API calls or other important event logging */
  rfgsm_cmd_buffer_type cmd_buffs;

  /* Callback pointer to the vbatt compensation calculator */
  timer_type rfgsm_core_vbatt_comp_clk_cb;

  /* Callback pointer to the Temp compensation calculator */
  timer_type rfgsm_core_temp_comp_clk_cb;

  /* GSM rf band before we go to sleep.*/
  /* This needs to be remembered so that RF driver can wakeup in same band.*/
  rfcom_gsm_band_type band_before_sleep;

  /* RF GSM copy of link manager buffer index passed from L1*/
  uint32 rxlm_buf_index;

  /* RF GSM copy of link manager buffer index passed from L1*/
  uint32 txlm_buf_index;

  /* Current APT Mode */
  apt_mode_type apt_mode;
  
  /* Mask indicating subs that are operating on the device at any one time */
  /* SHOULD ONLY BE MANIPULATED INSIDE CORE ENTER/EXIT WAKEUP/SLEEP APIS   */
  uint32 sub_id_mask;

  /**** IMPORTANT ****/
  /* This variable should ONLY be assigned a value in:      */
  /* -core init for initialisation                          */
  /* -core enter and core wakeup                            */
  /* -within the rfgsm_core_enter_exit_crit_sect mutex lock */
  /* This variable is used to determine whether CCS events  */
  /* need to be created for enter mode and wakeup based on  */
  /* minimised call-flow.                                   */
  boolean no_active_subs;

  boolean sawless_support[RFGSM_MAX_UNIQUE_RFCOM_BANDS];

  boolean ssbi_protocol_supported;

  /* RF GSM copy of link manager buffer index passed from L1*/
  uint32 rxlm_buf_index_ip2_cal;

  /* RF GSM copy of link manager buffer index passed from L1*/
  uint32 txlm_buf_index_ip2_cal;

  int32  ip2_cal_alpha;

  /*************************************************************************/
  /************* VARIABLES RESET ON INIT, WAKEUP & EXIT MODE ***************/
  /* RFC configuration pointer */
  rfc_gsm_core_config_type *rfc_gsm_core_ptr;

  /* Current band */
  rfcom_gsm_band_type band;

  /* Current band for Regular RX burst only*/
  rfcom_gsm_band_type rx_band;

  /* Current tx band, this get updated in rfgsm_core_tune_tx() */
  rfcom_gsm_band_type tx_band;

  rf_burst_type rx_burst;

  /* This flag needs to be set true when calibrating the filtered path is necessary*/
  boolean coex_filtered_path_cal_flag;

  /* Transmit/Receive channel numbers */
  uint16 rf_tx_arfcn;
  uint16 rf_rx_arfcn;

  /* Current PA range. Only used for Linear PA targets. */
  uint16 parange_override_val;

  /* Current RGI. Only used for Linear PA targets. */
  uint16 rgi_override_val;

  /*current RGI, updated in both of FTM and online modes, including da cal */
  uint16 current_rgi;

  /* Current LNA range */
  uint8 lna_range;

  /* PA start/stop times */
  int16 rf_pa_start_time;
  int16 rf_pa_stop_time;

  /* Determine whether to adjust the RFC Tx timings based on band */
  boolean tx_timing_adjust_flag;

  /* Flag to determine an update to the Tx power profile information is needed */
  boolean tx_profile_update_required;

  /* Only update the amam tables in 8psk to save processing time */
  boolean amam_ampm_recalc_required;

  /* Used during Tx calibration to ensure unrequried power profile code is not executed */
  boolean slot_override_flag;

  /* Determine whether to update the Tx power profile info if num Tx slots changed from last frame */
  uint8 last_num_tx_slots;

  /* Freq error from the AFC loop */
  int32 rfgsm_curr_freq_error;

  /* Tx Freq error from the AFC loop */
  int32 rfgsm_curr_freq_error_tx;

  /* will be set false when frequency adjust API is called */
  boolean freq_error_flag_from_api;

  /* Flag to in a Higher Multi-Slot Class mode */
  boolean hmsc_active;

  /* Determine whether to use path delay */
  boolean ftm_path_delay_cal_override;

  /* When using FTM delay override, this is the delay value used */
  uint16 ftm_path_delay_override_val;

  /* Controls the Tx slot characteristics in FTM mode */
  rfgsm_core_ftm_slot_override_type ftm_slot_override[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  uint8 rfgsm_core_ftm_bias_override_flag;

  /* Store PCLs and number of PCLs passed on from GL1 for comparison later*/
  rfgsm_modulation_type last_mod_type[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Store desense power backoffs from GL1 for comparison later*/
  uint8 last_desense_backoff[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Store desense power backoffs from GL1 for comparison later*/
  uint8 last_vbatt_backoff[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Determine whether the Pwr lvls have changed to then update the Tx power profile info */
  uint16 last_pwr_level[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Gain info for each Tx slot to be passed to Fw on frame-by-frame basis */
  rfgsm_GfwTxRfGainParam rfc_gsmctl_linear_pa_tx_gain_buff[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Simultaneos Tx SAR pwr limit for GMSK modulation*/
  int16 simult_sar_limit_gmsk[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Simultaneos Tx SAR pwr limit for 8PSK modulation*/
  int16 simult_sar_limit_8psk[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* Stored power tables for each gain state with 32RGI level granularity */
  int32 rfgsm_pmeas_g0[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g1[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g2[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g3[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g4[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g5[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g6[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_pmeas_g7[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g0[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g1[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g2[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g3[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g4[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g5[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g6[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  int32 rfgsm_edge_pmeas_g7[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];

  /* Tx power for each PCL with PCL vs pwr err compensation*/
  int16 rfgsm_gsm_pcl_vs_power[RFNV_NUMBER_OF_PCLS];
  int16 rfgsm_edge_pcl_vs_power[RFNV_NUMBER_OF_PCLS];
  boolean rfgsm_enable_pcl_logging;

  /* Multislot Tx pwr limit */
  int16 rfgsm_gsm_multislot_tx_pwr_limit[RFGSM_MAX_MULTISLOTS];
  int16 rfgsm_edge_multislot_tx_pwr_limit[RFGSM_MAX_MULTISLOTS];
  /* Error-checking flag to determine whether Rx tune was successful */
  boolean rf_tune_commanded;

  /* Pointers used to refer to Tx and Rx NV data */
  rfgsm_nv_band_data_type *rfgsm_rx_nv_tbl;
  rfgsm_nv_band_data_type *rfgsm_tx_nv_tbl;

  /* pointer to GSM/EGSM rx switch structure */
  rfgsm_nv_rx_switch_type *rfgsm_nv_rx_freq_comp_switch_ptr;
 
  /* Log useful Tx/Rx information */
  rfgsm_core_tx_log_type tx_log;
  rfgsm_core_rx_log_type rx_log;

  /* Compensated power in dbx16*/
  int16 valx16;

  /* Token to keep track of device image snap-shots during iRAT */
  uint8 rfgsm_meas_device_backup_token;

  /* Pointer to IQ capture data */
  rfgsm_core_iq_capture_type * iq_capture;

  /* Store current VBatt comp information */
  rfgsm_core_vbatt_comp_info_type vbatt_comp_info;

  /* Thermistor read and compensation data */
  rfgsm_core_therm_read_type therm_read;

  rfgsm_core_temp_comp_type temp_comp;

  boolean rfgsm_core_temp_comp_started;

  uint16 rfgsm_core_temp_comp_therm_value;

  /* Temp comp values for GMSK, *PSK and Unknown modulation schemes */
  int16 rfgsm_core_temp_comp_scaled_value;

  /* Buffer to store the baseband headroom for compensation */
  int32 rfgsm_core_bb_headroom_tbl[RFCOM_NUM_GSM_BANDS];

  /* Store Set Antenna Requests before they are being processed.*/
  uint8 set_antenna_position;
  
  /*Boolean to indicate ASD scripts to be built only once*/
  boolean configure_asd_scripts;

  /* A flag to indicate that antenna switch GFW ACK is required 
     for only upcoming Rx busrt in a frame and not all Rx burst*/  
  boolean asd_rsp; 

  /*UMID of GFW ACK*/
  uint32 asd_fw_rsp_umid;

  rfgsm_l1_cb_type asd_l1_cb_func;

  /*! Subscription ID to be returned to GL1 */
  uint8 asd_sub_id;
  
  /* Flag to indicate to set antenna to previous spostion */
  /* Required after wakeup and on enter_mode to ensure we use correct antenna position */
  boolean set_prev_ant_pos;

  /*Flag for GL1 to indicate that we are in probe burst*/
  rfgsm_rx_burst_type probe_burst;

  /* Callback pointer to the asd response received or not */
  timer_type rfgsm_core_asd_rsp_check_cb;
  
  /*! @brief This stores the asd swtich event info */
  rfgsm_core_asd_switch_event_info_type asd_switch_info;

  rf_lock_data_type rfgsm_core_crit_sect;
  rf_lock_data_type rfgsm_core_enter_exit_crit_sect;

  /* RFLM specific vars */
  uint8 buffer_id; //Triple buffer ID
  uint8 rx_burst_num; //current Rx being processed
  rfgsm_rx_burst_type rx_burst_type; //Sync Rx, SCH or ACQ Rx, OR Monitor Rx

  /* Ping-pong ramp ID */
  uint8 pa_ramp_buffer_id;

  /* Ping-pong AMAM AMPM ID */
  uint8 amam_ampm_buffer_id;

  /* Number of device-specific PA ramp table entries */
  uint32 num_pa_ramp_table_entries;

  /* Number of modem-specific AMAM AMPM table entries */
  uint32 num_amam_table_entries;
  uint32 num_ampm_table_entries;

  /* Device specific ramp determines the ramp saturation */
  uint16 pa_ramp_saturation_high;
  uint16 pa_ramp_saturation_low;
  boolean rf_has_five_gainranges;
  
  /*GSM standalone buffer = RFGSM_HOME_BUFFER,  GSM X2G IRAT buffer = RFGSM_AWAY_BUFFER */
  rfgsm_buffer_type rfgsm_buffer;

  /* Tx power and modulation structure per Tx slot */
  rfgsm_power_mod_type rfgsm_core_tx_pow_mod[RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /* The following variable holds frequency compensated linear AMAM and AMPM tables:
  ** This variable is static to this file. There are set functions to set this variable. 
  */
  rfgsm_amam_ampm_tables_type rfgsm_mdsp_linear_amam_ampm_table;
  /* The following variable holds the frequency and temperature compensated linear 
  AMAM/AMPM table. This is the table that is sent to mDSP. 
  **/
  rfgsm_amam_ampm_tables_type rfgsm_mdsp_linear_amam_ampm_table_final;

  rfgsm_core_device_capability_type device_capability;
  
  rfgsm_ip2_cal_params_type ip2_cal_params;

  uint8 edge_core;

  boolean ip2_cal_first_iter;

  /* Data to control the linearity mode of SAWLess device */
  rfgsm_core_sawless_linearity_data_type sawless_linearity_control;

  rfgsm_nv_rx_switch_type *rfgsm_nv_hl_rx_switch_ptr;

  boolean enh_rx_on_flag;

  int32 enh_rx_freq_offset;
 
  /* pointer to GSM/EGSM rx enh switch structure */
  rfgsm_nv_enh_rx_switch_type *rfgsm_nv_enh_rx_switch_ptr;
  
  /* New path returned by the concurreny manager , in case there is another
  tech active on the default path, the concurrency manager will give us
  an alternative path. To tune to the newly assigned path we will need to
  first query for the alternate path and the update rf port map */
  uint8 rx_alternate_path;
  uint8 tx_alternate_path;

  /* Flag for storing modulation type in all slots within the frame is GMSK (i.e. not unknown or 8PSK)*/
  boolean gmsk_modulation;

  /* Current state of the sub based state machine */
  /* Used for logging purposes */
  uint8 curr_state;
  
  /*Variable that stores the info of device regsiters to be read and 
       also controls enabling and disabling of them*/
  rfgsm_core_read_device_status device_config_to_read; 

    /*Counter for jdet readings < mid threshold*/
  uint8 jdet_counter[MAX_NUM_ARFCNS_IN_BAND];

  /* Band on which HL/LL switch was applied*/
  rfcom_gsm_band_type hl_ll_band;

  boolean rx_debug_logging_enable;

} rfgsm_core_handle_type;

#endif /* #ifndef RFGSM_CORE_HANDLE_H */


