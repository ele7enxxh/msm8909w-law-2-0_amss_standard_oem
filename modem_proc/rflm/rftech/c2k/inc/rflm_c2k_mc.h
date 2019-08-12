#ifndef RFLM_C2K_MC_H
#define RFLM_C2K_MC_H
/*
  @file
  rflm_c2k_mc.h

  @brief
  This file contains RFLM  c2k mc definitions, enumerations and prototypes that
  are used in rflm scope.

  @addtogroup RFLM_C2K_MC
  @{
*/

/*=============================================================================

                R F L M C2K MC  H E A D E R   F I L E 
       
DESCRIPTION
  This file contains declarations and definitions to support RF front end driver 
  module.

Copyright (c) 2013-2014  by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/c2k/inc/rflm_c2k_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/20/15   vr      Autopin check-in
12/01/15   vr      Synchronized Therm Read with TxAGC event
04/07/15   vc      Flag to check hdet progress and skip txagc
03/26/15   pk      Added flags for rf off requested and rf off action time 
01/13/15   zhw     Run WBEE Jammer detection for 1x every PCG
01/05/15   zhw     Make sure HDR runs TxAGC in normal mode
12/09/14   spa     Added rflm_c2k_abort_asd
12/01/14   zhw     Support for RF ON/OFF handling in DTX HDET
11/25/14   pl      Add support for multiple tasks for prepare issue seq API
11/17/14   cd      Add support for Tx DAC gating during RF on/off
09/04/14   JJ      Add prev_iref_idx to rflm_cdma_data 
08/19/14   spa     Remove "tx_dynamic_data" from C2k tx data struct (unused)
08/06/14   zhw     Add Macro to convert USTMR to Cx1
07/24/14   cd      Use double-buffering to update the ET delay value
07/23/14   cd      Update ET delay interface to RFLM, to avoid computation in 
                   critical path
06/17/14   hdz     Added rflm_c2k_commit_resend_seq()
06/12/14   JJ      Modify rflm_c2k_enqueue_fbrx_ldo_on_off_event()
05/28/14   zhw     FTM HDET support 
05/22/14   JJ      Added support for FBRX CCS Enable/Disable Scripts 
05/21/14   spa     Move CCS memory #defs to HAL layer (memory is modem specific)
05/12/14   zhw     Optimize static event & AOL/CL memory allocation to below 3K
04/30/14   spa     Update CCS memory usage for 4K memory
04/23/14   sty     Use double buffered approach for Tx on/off
04/17/14   zhw     Enablt HDET
04/11/14   spa     Move "rflm_c2k_tech_data" to rflm_c2k_mc.h
04/07/14   zhw     RFLM C2K HDET support
04/01/14   spa     Add task index as an argument in schedule issue seq
03/31/14   zhw     Tuner AOL Abort support
03/28/14   zhw     Tuner AOL/CL optimization
03/27/14   spa     Add task index as an argument in schedule LNA update 
03/11/14   zhw     HDET/Tuner CL frame work support. Add TxAGC skip flag
03/03/14   JJ      Increase LNA buffer size in CCS mem
02/28/14   hdz     Added AsDiv support
02/27/14   cd      Provide cal-mode support for FBRx
02/26/14   JJ      Added a debug flag for enable FBRx
02/10/14   spa     Increase memory allocated for DLNA Rx on/off events
02/10/14   spa     Add debug flag to enable/disable DLNA
02/10/14   spa     Add support to prepare/schedule isse seq events
01/20/14   spa     Added API to enable/disable LNA updates in RFLM
01/20/14   spa     Moved CDMA debug flag structure to C2K_MC header
01/16/14   hdz/JJ  Added halt/resume txagc api     
01/13/14   cd      Add ET delay programming to RFLM for online mode
01/12/14   hdz     Added double buffer support for txagc seq
12/16/13   zhw     C2K Tuner AOL Support
12/13/13   sty     Deleted unused API - rflm_c2k_schedule_immediate_lna_update
12/06/13   cd      Load nominal baseband gains to RFLM
12/01/13   hdz     Increased ccs buffer size
11/27/13   APU     IRAT Changes FOR bolt.
11/19/13   hdz     Added c2k buffer size
11/14/13   spa     Updated immediate LNA update API interface 
11/11/13   spa     Add API to schedule immediate LNA update
10/26/13   hdz     Removed un-used API
10/25/13   hdz     Added K10
10/17/13   hdz     Added support for txagc callback
10/16/13   ra      Remove CCS init
10/15/13   spa     Added calback and callback params for LNA event
10/08/13   spa     Removed unused APIs & structure params/added #def for cell ID
10/08/13   spa     compiler error fix (temp)
10/07/13   spa     Added NB index structure to RFLM CDMA data for 1x and HDR
09/13/13   hdz     Added doc note
08/29/13   hdz     Initial creation

===========================================================================*/
#include "rflm.h"
#include "rflm_txagc.h"
#include "rflm_api_c2k.h"
#include "rflm_rxagc_api.h" /* for cell_id_t */
#include "rflm_ccs_rf_intf.h"
#include "rflm_ccs_intf.h"
#include "rflm_cmn_intf.h"
#include "rflm_ccs_task_defs.h"
#include "rflm_api_autopin.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define maximum number of tasks for prepare issue seq API
 */
#define RFLM_CDMA_MAX_ISSUE_SEQ_TASKS                                           4 

/*----------------------------------------------------------------------------*/
/*! Convert USTMR count to Cx1.
Ratio is 19.2e6 / (1.2288e6) = 125 / 8 */
#define RFLM_CDMA_USTMR_TO_CX1(ustmr) (((ustmr) << 3) / 125)

/*----------------------------------------------------------------------------*/
/*! CDMA cell ID, CDMA has only one cell therefore all layers need to use the 
    same cell ID when querying for RxAGC data . This value can be any number as 
    long as its less than INVALID CELL ID
*/
#define RFLM_CDMA_GENERIC_CELL_ID                                             10

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define Tx Reverse Link Configuration Types for IQ Gain support
 
  @note
  Some enums are repeated below for e.g. 
  RFLM_CDMA_1X_TX_IQGAIN_RC3_VOICE = 4,
  RFLM_CDMA_1X_TX_IQGAIN_RC4_VOICE = 4,
  the reason for this is these waveforms are not different on
  the FW side... and they have the same Peak to Average ratio.

  
*/
typedef enum
{
  RFLM_CDMA_1X_TX_IQGAIN_PILOT = 0,

  RFLM_CDMA_1X_TX_IQGAIN_RC1 = 1,
  RFLM_CDMA_1X_TX_IQGAIN_RC2 = 1,

  RFLM_CDMA_1X_TX_IQGAIN_RC8_VOICE = 2,
  RFLM_CDMA_1X_TX_IQGAIN_RC8_DATA = 3,

  RFLM_CDMA_1X_TX_IQGAIN_RC3_VOICE = 4,
  RFLM_CDMA_1X_TX_IQGAIN_RC4_VOICE = 4,

  RFLM_CDMA_1X_TX_IQGAIN_RC3_DATA = 5,
  RFLM_CDMA_1X_TX_IQGAIN_RC4_DATA = 5,

  RFLM_CDMA_1X_TX_IQGAIN_RC_NUM

} rflm_cdma_1x_tx_iqgain_rc_type ;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Define Tx Reverse Link Configuration Types for IQ Gain support
*/
typedef enum
{
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_PILOT = 0,
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_RRI0 = 1,
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_RRI1 = 2,
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_RRI2 = 3,
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_RRI3 = 4,
  RFLM_CDMA_HDR_TX_IQGAIN_DOR0_RRI4 = 5,

  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS0 = 6,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS1 = 7,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS2 = 8,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS3 = 9,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS4 = 10,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS5 = 11,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS6 = 12,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS7 = 13,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS8 = 14,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS9 = 15,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS10 = 16,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS11 = 17,
  RFLM_CDMA_HDR_TX_IQGAIN_DORA_PS12 = 18,

  RFLM_CDMA_HDR_TX_IQGAIN_RC_NUM

} rflm_cdma_hdr_tx_iqgain_rc_type ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure definition used to store Tx IQ gain data for different 1x and
  HDR reverse link configurations, used for APT backoff
*/
typedef struct
{
  uint16 iq_gain_scale_factor;
  /*!< IQ gain scale in 2Q14 HW units. Store the baseband gain adjust value 
  specific to the reverse link configuration. This is computed based on 
  NV data */

  int16  lutDbOffset;
  /*!< Linearizer power offset in dBx10 units. This will be used 
  by FW to offset the txTotal in order to ensure that the linearizer is 
  shifted correctly to compensate for IQ gain changes. */ 

} rflm_cdma_tx_iq_params_t ;

/*----------------------------------------------------------------------------*/
/* CDMA debug flags, needed to control functionality in RFLM CDMA side*/
typedef struct
{
  boolean skip_gain_update;
  boolean do_lna_steps;
  boolean skip_wbdc_update;
  boolean skip_tx_tune;
  boolean skip_rx_tune;
  boolean enable_trace_mssgs;
  boolean enable_tuner_aol;
  boolean enable_rx_on_off;
  boolean enable_fbrx;
  boolean enable_asdiv;
  boolean enable_tuner_cl;
  boolean enable_hdet;
  boolean enable_jammer_detection;
  boolean enable_therm;
  boolean enable_auto_pin;
} rflm_c2k_debug_s;

/*----------------------------------------------------------------------------*/
extern rflm_c2k_debug_s rflm_cdma_debug_flags;

/*----------------------------------------------------------------------------*/
/*! @brief
  c2k dedicated ccs taskQ structure
*/
typedef struct 
{
  /*! @brief CCS RF TQ  taskQ pointer */
  rflm_ccs_rf_tq_handle_t *tq_handle;
  
  /*! @brief CCS RF TQ data memory pointer */
  uint32 *tq_data_ptr;
  
  /*! @brief Length (in bytes) of the data memory */
  uint16 tq_data_bytes;
  
  /*! @brief TQ index corresponding to tq_handle */
  uint8 tq_index;

} rflm_c2k_dedicated_ccs_tq_t;

/*----------------------------------------------------------------------------*/
/*! @brief
  c2k dedicated ccs pair taskQ structure
*/
typedef struct 
{
  /*! c2k dedicated tx taskQ */
  rflm_c2k_dedicated_ccs_tq_t c2k_tx_tq;
  /*! c2k dedicated rx taskQ */  
  rflm_c2k_dedicated_ccs_tq_t c2k_rx_tq;
  
} rflm_c2k_pair_dedicated_ccs_tq_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  c2k asdiv data type
*/
typedef struct
{
  /*! 1= immediate; 0=timed*/
  boolean immediate;

  /*! pending adsiv event */
  boolean asdiv_pending;

  /*! c2k rf state*/
  rflm_c2k_rf_state rf_state;

  /*! action time for ads script*/
  uint32 action_time;

  /*!handle id*/
  rflm_handle_rx_t prx_handle;

}rflm_c2k_asdiv_data_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  c2k hdet data type
*/
typedef struct
{
  /*! @brief Flag set to TRUE by RF MC to indicate an pending hdet event, 
   * once RFLM is able to pick the event up, it sets the flag to FALSE to
   * indicate RFLM has started HDET process
   */
  boolean                                 hdet_pending;

  /*! @brief Number of HDET requests successfully serviced by FW from the
   * start of time. Will wrap-around to 0 after 2^31 -1 . SW should read this
   * value before sending a HDET request to FW and expect the incremented
   * value when FW finishes servicing the request. FW never resets this. So
   * the value is non-volatile between TX reinits. The only time this is
   * cleared is when the shared mem is cleared.*/
  uint32                                  num_hdet_serviced;

  /*! @brief Indicates if a HDET request is sucessfully serviced. SW should
   * first wait for the numHdetServiced to be incremented and then read this
   * variable.
   */
  boolean                                 hdet_success;

  /*! @brief The "frozen" tx total power value during the HDET measurement
   * process.
   */
  int16                                   hdet_tx_power;

  /*! @brief Flag to indicate HDET is in FTM mode. This allows RFLM to bypass 
    1x RPC indicator.
   */
  boolean                                 hdet_in_ftm_mode;

  /*! @brief Flag to indicate whether HDET scheduling is allowed which is TX 
    ON. This flag is set by tx_rf_on_ctrl API that FW calls to turn on/off
    RF TX. By default this flag is 0 (FALSE) which matches the HW status.
    The flag is turned on by RF_ON CCS CALLBACK, which will only happen
    after PA is turned on. And the flag is turned off as soon as tx_rf_on_ctrl
    API is called with off.
   */
  boolean                                 hdet_tx_on;
  

  boolean                                 hdet_read_in_process;
}rflm_c2k_hdet_data_t;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  c2k therm data type
*/
typedef struct
{
  /*! @brief Flag set to THERM by RF MC to indicate an pending therm event, 
   * once RFLM is able to pick the event up, it sets the flag to FALSE to
   * indicate RFLM has started THERM process
   */
  boolean                                 therm_pending;

  /*! @brief Number of THERM requests successfully serviced by FW from the
   * start of time. Will wrap-around to 0 after 2^31 -1 . SW should read this
   * value before sending a THERM request to FW and expect the incremented
   * value when FW finishes servicing the request. FW never resets this. So
   * the value is non-volatile between TX reinits. The only time this is
   * cleared is when the shared mem is cleared.*/
  uint32                                  num_therm_serviced;

  /*! @brief Indicates if a THERM request is sucessfully serviced. SW should
   * first wait for the num_therm_serviced to be incremented and then read this
   * variable.
   */
  boolean                                 therm_success;

  boolean 				  therm_read_in_process;

}rflm_c2k_therm_data_t;

typedef struct
{

 uint8 device;
 
 boolean autopin_tx_on;

 boolean autopin_par_support;

 rflm_handle_tx_t handle_id;

 uint32 delay_in_capture_time;

 }rflm_c2k_auto_pin_data_t;


/*----------------------------------------------------------------------------*/
/*! @brief
  c2k dedicated ccs pair taskQ structure
*/
typedef struct 
{
  
  /*! c2k default xpt settings */  
  struct rflm_c2k_xpt_setting_s{
    uint32 default_apt_iq_gain;
    uint32 default_ept_iq_gain;
    uint32 default_et_iq_gain;
    uint32 default_env_scale_gain;

    int32  et_delay_ns_x10;
    rflm_txagc_et_path_delay_t et_path_delay_data[2];
    uint32 et_path_delay_addr;
    boolean delay_data_idx;
  }xpt_setting;

  /*Onex iref update data*/
  struct rflm_c2k_iref_update_s
  {
    boolean iref_update_en;
    uint32  iref_idx;
    uint32  iref_start_time;
    uint32  iref_stop_time;
  }iref_update_data;

  /*Turnaround contant*/
  int16 k10;
    
  /* The iref idx from the last txagc update, it's updated in the last txagc
  update and used to compare with the iref idx in the current txagc update 
  to decide if iref programming will be skipped or not. This will reduce 
  txagc update time */
  uint8 prev_iref_idx;

  /* Tx AGC update state information */
  struct rflm_c2k_txagc_state_s
  {
    /*Flag to indicate if txagc_update is halt or running*/
    /* TRUE - stop txagc update by doing early return before txagc algorithm */
    /* FALSE - no effect to txagc updata api */
    boolean update_halted;

    /* Flag to indicate if RF is on/off */
    boolean is_rf_on;

    /* Flag to indicate if Tx DAC is on/off */
    boolean is_dac_enabled;

    /* Flag to indicate if ET DAC is on/off */
    boolean is_etdac_enabled;

    /* Flag to indicate if FW has requested RF off event */
    boolean rf_off_requested;

    /* USTMR time at which RF off event has been requested */
    rflm_ustmr_t rf_off_time;

  }txagc_state_data;

  /* Tx RF on/off call-back data */
  struct rflm_c2k_tx_rf_on_cb_data_s
  {
    rflm_handle_tx_t handle_id;
    boolean cb_for_rf_on;
    rflm_tech_id_t tech_id;
  }tx_rf_on_cb_data;

} rflm_c2k_tx_data_t;


/*----------------------------------------------------------------------------*/
/*! @brief
  rflm 1x global data structure
*/
typedef struct
{
  /*! @brief device mask used in 1x enter and 1x exit  */
  uint32 rflm_1x_device_mask;

  /*! @brief 1x dedicated taskQ pair  */
  rflm_c2k_pair_dedicated_ccs_tq_t onex_dedicated_pair_tqs;

  /*! @brief rflm c2k data buffer pointer*/
  rflm_c2k_tx_data_t* rflm_c2k_tx_data_ptr;

  /*! @brief rflm Tx IQ array */
  rflm_cdma_tx_iq_params_t rflm_1x_tx_iq_params[RFLM_CDMA_1X_TX_IQGAIN_RC_NUM] ;

  /*! @brief asdiv data */
  rflm_c2k_asdiv_data_t asdiv_data;

  /*! @brief hdet data */
  rflm_c2k_hdet_data_t hdet_data;

    /*! @brief hdet data */
  rflm_c2k_therm_data_t therm_data;

  /*! @Variable  used as toggle for txagc double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean txagc_ccs_mem_offset_idx ;

  /*! @Variable  used as toggle for RxAGC double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean rx_lna_ccs_mem_offset_idx ;

  /*! @Variable  used as toggle for TX on/off double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean blk_used_for_tx_onoff ;

  /*! @brief Flag to indicate next TxAGC is to be skipped. Set to TRUE to 
    skip the next 1x txagc. It will be set back to FALSE once TxAGC is skipped
  */
  boolean skip_next_txagc;
  
  /*! @brief Flag to indicate HDET is in progress. Set to TRUE to 
    skip the txagc update. It will be set back to FALSE in HDET callback function
  */
  boolean hdet_progress;

  rflm_c2k_auto_pin_data_t autopin_data;

  rflm_autopin_get_run_decision_in_t autopin_in;

  rflm_autopin_get_run_decision_out_t autopin_out;

}rflm_1x_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief
  rflm hdr global data structure
*/
typedef struct
{
  /*!@brief device mask used in hdr enter and hdr exit */
  uint32 rflm_hdr_device_mask;

  /*!@brief  hdr dedicated taskQ pair */
  rflm_c2k_pair_dedicated_ccs_tq_t hdr_dedicated_pair_tqs;

  /*! @brief rflm hdr data buffer pointer*/
  rflm_c2k_tx_data_t* rflm_hdr_tx_data_ptr;  

  /*! @brief rflm Tx IQ array */
  rflm_cdma_tx_iq_params_t rflm_hdr_tx_iq_params[RFLM_CDMA_HDR_TX_IQGAIN_RC_NUM] ;

  /*! @brief asdiv data */
  rflm_c2k_asdiv_data_t asdiv_data;

  /*! @brief hdet data */
  rflm_c2k_hdet_data_t hdet_data;

  /*! @brief hdet data */
  rflm_c2k_therm_data_t therm_data;

  /*! @Variable  used as toggle for txagc double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean txagc_ccs_mem_offset_idx ;

  /*! @Variable  used as toggle for RxAGC double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean rx_lna_ccs_mem_offset_idx ;

  /*! @Variable  used as toggle for TX on/off double buffer, 
      0 = use the first half buffer, 1 = use the second half buffer */
  boolean blk_used_for_tx_onoff ;

    /*! @brief Flag to indicate HDET is in progress. Set to TRUE to 
    skip the txagc update. It will be set back to FALSE in HDET callback function
  */
  boolean hdet_progress;

  rflm_c2k_auto_pin_data_t autopin_data;

  rflm_autopin_get_run_decision_in_t autopin_in;

  rflm_autopin_get_run_decision_out_t autopin_out;

}rflm_hdr_data_t;

/*----------------------------------------------------------------------------*/
/*! @brief
  rflm c2k global data structure
*/
typedef struct
{
  /*! @brief 1x data */
  rflm_1x_data_t onex_data;
  
  /*! @brief hdr data */
  rflm_hdr_data_t hdr_data;
  
  /*! 
    @brief
    Flag to indicate if CDMA RFLM is operating in cal mode or not
  */
  boolean cal_mode;
  
}rflm_c2k_data_t;

/*----------------------------------------------------------------------------*/
/*!
  @name DC accumulator info
 
  @brief
  This structure holds the last known good I and Q values for WBDC estimator 1
  and WBDC estimator 2. This table has 2 Rows, one for each estimator and  
  one column for each LNA gain state */
typedef struct
{
  uint32 dc_accum_init_i[2][RFLM_CMN_RXAGC_MAX_LNA_STATES];
  uint32 dc_accum_init_q[2][RFLM_CMN_RXAGC_MAX_LNA_STATES];
}rflm_dc_accum_info;

/*----------------------------------------------------------------------------*/
/*!
  @name WB jammer detect info
 
  @brief
  This structure holds information related to Wide Band Jammer detection
  algorithm*/
typedef struct
{
  /*! Flag for enabling/disabling jammer detection */
  boolean jd_algo_enabled;

  /*! Flag for freezing/unfreezing jammer detection */
  boolean jd_algo_frozen;

  /*! Flag to indicate presence of a jammer  */
  boolean jammer_present;

  /*! Flag to check for previous jammer status  */
  boolean previous_jammer_status;

  /*! Flag to check if the first read was complete  */
  boolean first_read_done;

  /*! WBEE thresholds per LNA state,used for determining presence of jammer */
  uint32 jd_thresh[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! WBEE thresholds minus hysteresis per LNA state,used for determining 
      presence of jammer */
  uint32 jd_thresh_minus_hyst[RFLM_CMN_RXAGC_MAX_LNA_STATES];

  /*! WBEE Accumulator status, when we start the algorithm for the first time 
   this will be false */
  boolean wbee_accum_status;

  /*! WBEE accumulation start time */
  rflm_ustmr_t wbee_accum_start_time;

  /*! WBEE accumulation start time */
  rflm_ustmr_t wbee_accum_stop_time;

}rflm_wb_jammer_algo_info;

/*----------------------------------------------------------------------------*/
/*!
  @name Generic structure to hold CDMA specific data unique for each WB 
 
  @brief
  This structure holds CDMA data to be accessed through scratch mem provided by
  common modules, example: common RxAGC module
*/

/*! @{ */
typedef struct
{
  /*! DC accumulator data for the ith Wideband , contains the last known good 
  Estimator 1 and 2 I and Q register values for each LNA state, and unique 
  for each WB */ 
  rflm_dc_accum_info dc_accum_data;

  /*! Wideband index for which WBDC update is taking place */
  uint8  wb_idx;

  /*! Pointer to nb indices array (needed for CSR phase offsets)*/
  uint8 nb_indices[RFLM_CMN_RXAGC_MAX_NB_PER_WB];

  /*! LNA state requested */
  uint8  lna_state;

  /*! DC ACQ and TRACK SHIFT values for est1 and est 2 */
  uint8 dc_acq_shift_est1;
  uint8 dc_acq_shift_est2;
  uint8 dc_trk_shift_est1;
  uint8 dc_trk_shift_est2;

  /*! CSR phase offsets (per LNA state)*/
  uint32 csr_phase_offset;

  /*! Rx on settle time, used for Rx on/off control API */
  rflm_ustmr_t rx_on_settle_time;

  /*! Task index in the TQ where LNA task was scheduled */
  uint32 lna_task_index;

  /*! Task index in the TQ where Rx off task was scheduled */
  uint32 rx_off_task_idx;

  /*! Task index in the TQ where Rx on task was scheduled */
  uint32 rx_on_task_idx;

  /*! Information related to jammer detection algorithm */
  rflm_wb_jammer_algo_info jammer_detect_info;

}rflm_c2k_tech_data;

/*---------------------------------------------------------------------------*/
uint32
rflm_c2k_get_ccs_mem_addr
(
  rflm_cdma_mdsp_script_id_t script_id,
  rflm_tech_id_t  tech_id
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_data_init
(
  void
);

/*---------------------------------------------------------------------------*/
boolean 
rflm_c2k_get_tq_pair
(
  uint8                   device  ,
  rflm_tech_id_t          tech    ,
  boolean                 is_irat ,
  uint8*                  tq_pair
) ;

/*---------------------------------------------------------------------------*/
void
rflm_1x_mc_enter
(
  uint8     dev_num ,
  uint32 rflm_handle,
  boolean   is_irat 
);

/*---------------------------------------------------------------------------*/
void
rflm_hdr_mc_enter
(
  uint8     dev_num , 
  uint32 rflm_handle,
  boolean   is_irat 
);

/*---------------------------------------------------------------------------*/
void
rflm_1x_mc_exit
(
  uint8 dev_num
);

/*---------------------------------------------------------------------------*/
void
rflm_hdr_mc_exit
(
  uint8 dev_num
);

/*---------------------------------------------------------------------------*/
boolean
rflm_1x_mc_config_tx_agc_params
(
  uint32 default_apt_iq_gain,
  uint32 default_ept_iq_gain,
  uint32 default_et_iq_gain,
  uint32 default_env_scale_gain,
  int16  k10,
  uint8  txagc_cal_mode
);

/*---------------------------------------------------------------------------*/
boolean
rflm_hdr_mc_config_tx_agc_params
(
  uint32 default_apt_iq_gain,
  uint32 default_ept_iq_gain,
  uint32 default_et_iq_gain,
  uint32 default_env_scale_gain,
  int16  k10,
  uint8  txagc_cal_mode
);

/*---------------------------------------------------------------------------*/
boolean
rflm_1x_mc_config_tx_et_delay
(
  uint32 rflm_handle,
  int32 et_delay_val
);

/*---------------------------------------------------------------------------*/
boolean
rflm_hdr_mc_config_tx_et_delay
(
  uint32 rflm_handle,
  int32 et_delay_val
);

/*---------------------------------------------------------------------------*/
uint32
rflm_c2k_ccs_commit_txagc_seq
(
  rflm_ccs_rf_seq_t *issue_seq_ptr,
  const rflm_c2k_dedicated_ccs_tq_t *rf_tq,
  uint32 ccs_mem_offset,
  uint32 ustmr_action_time,
  uint32 handle_id
);

/*---------------------------------------------------------------------------*/
rflm_c2k_dedicated_ccs_tq_t*
rflm_c2k_ccs_get_tx_tq
(
  rflm_tech_id_t  tech_id
);

/*---------------------------------------------------------------------------*/
rflm_c2k_dedicated_ccs_tq_t*
rflm_c2k_ccs_get_rx_tq
(
  rflm_tech_id_t  tech_id
);

/*---------------------------------------------------------------------------*/
boolean 
rflm_c2k_create_rxagc_tasks
(
  rflm_handle_rx_t handle,
  rflm_tech_id_t tech,
  cell_id_t cell_id
);


/*---------------------------------------------------------------------------*/
boolean 
rflm_c2k_schedule_lna_update
(
  rflm_handle_rx_t handle,
  rflm_tech_id_t tech,
  cell_id_t cell_id,
  rflm_rxagc_lna_state_e gain_state,
  uint32 action_time,
  uint32 *task_idx,
  void* lna_update_done_callback,
  void* lna_update_done_cb_params
);

/*---------------------------------------------------------------------------*/
rflm_c2k_data_t*
rflm_c2k_get_data
(
  void
);

/*---------------------------------------------------------------------------*/
void 
rflm_c2k_create_dummy_task
(
  rflm_c2k_dedicated_ccs_tq_t* tq,
  uint32 action_time,
  void* callback_api,
  void* callback_params
);

/*---------------------------------------------------------------------------*/
uint32
rflm_c2k_get_script_buffer_size
(
  rflm_cdma_mdsp_script_id_t script_id
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_halt_txagc_update
(
  rflm_tech_id_t tech_id
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_resume_txagc_update
(
  rflm_tech_id_t tech_id
);

/*---------------------------------------------------------------------------*/
void
rflm_cdma_mc_debug_enable_imm_lna_writes
(
  boolean lna_update_enabled
);

/*---------------------------------------------------------------------------*/
boolean
rflm_c2k_prepare_issue_seq_for_event
(
  rflm_tech_id_t  tech_id,
  uint32 handle_id,
  uint32 event_id,
  rfmeas_event_infor_type *temp_ccs_loc,
  rflm_ccs_rf_seq_t* ptr_to_dm_loc
);

/*---------------------------------------------------------------------------*/
boolean
rflm_c2k_schedule_issue_seq_for_event
(
  rflm_tech_id_t  tech_id,
  uint32 handle_id,
  rflm_cdma_mdsp_script_id_t event_id,
  rflm_ustmr_t action_time,
  uint32 task_priority,
  uint32 *task_idx,
  void* callback,
  void* cb_params
);

/*---------------------------------------------------------------------------*/
boolean
rflm_c2k_mc_set_iq_gain_params_specs 
(  
  rflm_tech_id_t  tech_id ,
  rflm_cdma_tx_iq_params_t* tx_iq_gain_params ,
  uint8 size_of_cdma_iq_params  
) ;

/*---------------------------------------------------------------------------*/
void
rflm_c2k_schedule_tuner_open_loop_update
(
   rflm_cmn_tuner_event_data_type* tuner_event_ptr,
   rflm_tech_id_t tech_id,
   rflm_ustmr_t   action_time
);

/*---------------------------------------------------------------------------*/
boolean
rflm_c2k_schedule_tuner_closed_loop_update
(
   rflm_cmn_tuner_event_data_type* tuner_event_ptr,
   rflm_tech_id_t tech_id,
   int16 raw_txagc,
   rflm_ustmr_t action_time
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_abort_tuner_open_loop_update
(
  rflm_tech_id_t tech_id,
  rflm_handle_tx_t txlm_handle
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_schedule_hdet
(
   rflm_tech_id_t tech_id,
   rflm_dm_handle_id_t handle_id,
   rflm_c2k_hdet_data_t* hdet_data,   
   rflm_ustmr_t   action_time
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_schedule_therm
(
   rflm_tech_id_t tech_id,
   rflm_dm_handle_id_t handle_id,
   rflm_c2k_therm_data_t* therm_data,   
   rflm_ustmr_t   action_time
);

/*---------------------------------------------------------------------------*/


void
rflm_c2k_schedule_autopin_trigger
(
   rflm_tech_id_t tech_id,
   rflm_dm_handle_id_t handle_id,
   rflm_c2k_auto_pin_data_t *autopin_data,
   int16 raw_txagc,
   rflm_ustmr_t   action_time
);

/*---------------------------------------------------------------------------*/
void
rflm_cdma_mc_run_jammer_detection
(
  rflm_tech_id_t tech_id,
  rflm_c2k_tech_data *tech_data_ptr
);

/*----------------------------------------------------------------------------*/
void 
rflm_c2k_enqueue_fbrx_ldo_on_off_event
(  
   rflm_handle_tx_t handle, 
   rflm_c2k_dedicated_ccs_tq_t *tq_ptr,
   rflm_tech_id_t tech_id,
   rflm_ustmr_t effect_time 
);

/*---------------------------------------------------------------------------*/
uint32
rflm_c2k_commit_resend_seq
(
  rflm_ccs_rf_seq_t *issue_seq_ptr,
  const rflm_c2k_dedicated_ccs_tq_t *rf_tq,
  uint32 ccs_mem_offset,
  uint32 ustmr_action_time,
  uint32 handle_id
);

/*---------------------------------------------------------------------------*/
void
rflm_c2k_abort_asd
(
  rflm_handle_rx_t handle
);

/*---------------------------------------------------------------------------*/

extern void
rflm_cdma_msgr_send_autopin_trigger
(
 void
);
/*---------------------------------------------------------------------------*/

void
rflm_c2k_schedule_autopin_trigger
(
   rflm_tech_id_t tech_id,
   rflm_dm_handle_id_t handle_id,
   rflm_c2k_auto_pin_data_t *autopin_data,
   int16 raw_txagc,
   rflm_ustmr_t   action_time
);
#ifdef __cplusplus
}
#endif

/*!   @} */

#endif /* #ifndef RFLM_C2K_MC_H */
