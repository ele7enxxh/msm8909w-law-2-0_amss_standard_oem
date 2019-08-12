/*!
  @file
  rflm_wcdma_sw_api.h

  @brief
  RF LM API definitions for WCDMA that is to be accessed by sw
  
  @detail
  Provides the following API's for sw to call:
  - rx agc create task
  - ccs mem init 
  - ccs init to initilise the tq
  
*/

/*==============================================================================

  Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/wcdma/rflm_wcdma_sw_api.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/16/15   ag      Added configs for Autopin
09/01/15   vs      Add phase info to TxAGC static LUT
04/16/15   bn      0x1842 logging fixes
03/12/15   gh      Added therm_enable configs to rflm_wcdma_tx_dm_buf_type
                   Added fields for keeping track of timing
02/27/15   cd      TxAGC updates for max power override support
12/12/14   rmb     Add Meas config which has Preload, TRx and non Trx script exec time.
12/02/14   ac      Workaround to ABORT all pending Snums during disable Tx
11/05/14   ac      ftm flag to denote the txagc state to FED via DM, skip fbrx_ldo_off call  
11/05/14   kai     Move DAC Iref data into DM
09/02/14   aro     Flag indicating the TxAGC fixup was skipped
09/11/14   ak      Adding Support to consider static DVGA offsets in the DVGA gain calculations
09/26/14   ac      FBRX_GPS change for ldo on state
09/09/14   ac      FBRx + GPS concurrency SM implementation
08/28/14   vbh     Added device parameter for ccs_init() and release_tq_pair()
08/15/14   zhh     populate the new MPR to DM in rfwcdma_core_get_tx_mpr_param()
08/15/14   aro/gh  Add fields in rflm_wcdma_tx_status_type to capture ILPC state
08/01/14   dw      Initial support for single Tx DSDA
07/23/14   kai     Move ET Delay alignment calculations off the critical path
07/21/14   vbh     LNA race condition fix
07/17/09   ac      featurise trace logs for mppps contribution improvement for rxagc
07/14/14   rmb     Fix Compilation Error because of rflm_wcdma_abort_tuner_open_loop_update().
07/01/14   aro     Collision Prevention between TxPlim and TempComp
06/25/14   bn      Dynamic TxAGC timeline adjustment feature for WCDMA Voice
06/12/14   zhh     Pass event size while allocate memory for CCS task
06/05/14   kai     splitting the ET DAC on sequence between FBRx event
06/04/14   dw      Reduce the RxAGC trace log size to 32
05/30/14   bn      RFLM WCDMA Binary Log packet support
05/28/14   aa      TQ management APIs
05/15/14   aro     Support for Power Meter
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/18/14   rmb     Add IRAT Meas enum for W2X and X2W scenarios.
04/17/14   dw      Added intitial tuner closed loop support
04/11/14   dw      Add Tx status logging in DM
04/08/14   ac      jammer detect support
03/24/14   rmb     Move the btf delay paramter from Tx config to Rx config band.
03/13/14   rmb     Add the W2W IRAT flag to Rx chan and band structure.
02/27/14    rmb    Change AGC Trace log size to 256 from 16 for easy debugging.
02/19/14    ac     removing release of tq for sleep()
01/31/14    rmb    Added new FED API to get RX information.
01/08/14    dw     Add mdsp/FED support for wakeup-tune config
01/03/13    dw     Added warm time, PLL settling time and RF transaction time
12/13/13    dw     Merge type definition between RFLM and RFWCDMA_MDSP
11/19/13    ac     warning fix
11/12/13    ac     splitting to ccs exit() and sleep()
10/22/13    ac     ccs_init return type 
10/15/13    ac     moved exit() from ccs header file, mem_init() gtest related fix
10/15/13    dw     Remove unnecessary header includsion
10/10/13    ac     initial version, all apis that is accessed by sw
==============================================================================*/

#ifndef RFLM_WCDMA_SW_API_H
#define RFLM_WCDMA_SW_API_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
#include "npa.h"
#endif

#include "rflm_api_cmn.h"
#include "rflm_dm_api.h"
#include "rflm_rxagc_api.h"
#include "rflm_txagc_api.h"
#include "rflm_api_wcdma.h"
#include "rflm_cmn_intf.h"
#include "rflm_api_fbrx.h"
/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES:
 
    Data structure definition of DM tech buffer, need to be in sync with the
    type definition in rfwcdma_mdsp_types.h. Duplicate definition here for
    FED standlone build.

==============================================================================*/
#define RFLM_WCDMA_TRACE_LOG_SIZE 32

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of LNA states for WCDMA
    Need to be in sync with Common RxAGC definition
*/
#define RFLM_WDMA_MAX_NUM_LNA_STATES              RFLM_CMN_RXAGC_MAX_LNA_STATES

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of PA states for WCDMA
    Need to be in sync with Common TxAGC definition
*/
#define RFLM_WCDMA_MAX_NUM_PA_STATES                    RFLM_TXAGC_PA_STATE_MAX


/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of TX uplink carriers
    Need to be in sync with FW definition 
*/
#define RFLM_WCDMA_MAX_NUM_TX_CARRIERS                                        2

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of RX downlink carriers
    Need to be in sync with FW definition 
*/
#define RFLM_WCDMA_MAX_NUM_RX_CARRIERS                                        3 

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of TX Tx MPR(max power ratio) bins
    Need to be in sync with FW definition 
*/
#define RFLM_WCDMA_MAX_NUM_MPR_BINS                                           8

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines maximum number of RF protocol bus type (RFFE,SSBI,GRFC)
*/
#define RFLM_WCDMA_MAX_NUM_BUS_TYPE                                           3
  
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines maximum number of NB per WB
*/
#define RFLM_WCDMA_MAX_NB_PER_WB											  3

/*----------------------------------------------------------------------------*/

/*! 
    @brief
    This list of definitions for jammer detection
*/
#define RFLM_WCDMA_RXAGC_SAT_DET_THRESHOLD_SQUARED 0x2A1909
#define RFLM_WCDMA_RXAGC_NOISE_FLOOR_SQUARED       0x6A07
#define RFLM_WCDMA_RXAGC_SAT_DET_THRESHOLD 0x67D00
#define RFLM_WCDMA_RXAGC_SAT_DET_GAIN_HYST 70
#define RFLM_WCDMA_JAM_DET_EN 0


#define RFLM_WCDMA_TXAGC_BUFFER_COUNT  4

#define RFLM_Q6_MAX_CPU_SPEED_KHZ 614400

/*----------------------------------------------------------------------------*/                
/*! 
    @brief
    Enum definition of RF WCDMA DRx event control type
 
    @details
    DRx event also known as Rx LPM (low power mode) event, enable means Rx is stopped
    and put into low power mode. disable means Rx is resumed and bring out of low
    power mode
*/
typedef enum
{

  /*! Rx is stopped and put into low power mode */
  RFLM_WCDMA_DRX_ENABLE,

  /*! Rx is resumed and bring out of low power mode */
  RFLM_WCDMA_DRX_DISABLE,

  /*! Number of Rx DRx control */
  RFLM_WCDMA_DRX_CTL_NUM,

  /*! DRx control max  */
  RFLM_WCDMA_DRX_CTL_MAX = RFLM_WCDMA_DRX_CTL_NUM
} rflm_wcdma_drx_event_type;

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Enum definition of RF WCDMA DTx event control type
 
    @details
    Enable means Tx PLL is shut down and put in to low power mode. Disable means
    Tx PLL is retuned and bring out of low power mode.

*/
typedef enum
{
  /*! Tx PLL is stopped and put into low power mode */
  RFLM_WCDMA_DTX_ENABLE,

  /*! Tx PLL is retuned and bring out of low power mode */
  RFLM_WCDMA_DTX_DISABLE,

  /*! Number of Tx DTx control */
  RFLM_WCDMA_DTX_CTL_NUM,

  /*! DTx control max  */
  RFLM_WCDMA_DTX_CTL_MAX = RFLM_WCDMA_DTX_CTL_NUM
} rflm_wcdma_dtx_event_type;
/*----------------------------------------------------------------------------*/                
/*! 
    @brief
    Enum definition of RF WCDMA IRAT technology type
 
    @details
    This enum is primarily used to differentiate between various
    W2X and X2W scenarios - W2W, G2W and L2W. Based on this enum, we can
    perform unique action corresponding to each scenario
*/
typedef enum
{
  /*! WCDMA Standalone */
  RFLM_WCDMA_HOME_MEAS,

  /*! W2W Scenario */
  RFLM_WCDMA_W2W_NEIGHBOUR_MEAS,

  /*! G2W Scenario */
  RFLM_WCDMA_G2W_NEIGHBOUR_MEAS,

  /*! L2W Scenario */
  RFLM_WCDMA_L2W_NEIGHBOUR_MEAS,
 
  /*! Maximum scenarios possible  */
  RFLM_WCDMA_IRAT_MEAS_MAX,

  /*! Invalid scenario  */
  RFLM_WCDMA_INVALID_MEAS = RFLM_WCDMA_IRAT_MEAS_MAX
} rflm_wcdma_irat_meas_enum_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
    Enum definition of THERM read state
 
    @details
    This enum is used for indicating what state THERM read is in
*/
typedef enum
{
   /*! THERM_READ Inactive */
   RFLM_WCDMA_THERM_ENABLE_INACTIVE,

   /*! THERM Triggered from SW */
   RFLM_WCDMA_THERM_ENABLE_TRIGGERED_FROM_SW,

   /*! THERM enabled via RFLM */
   RFLM_WCDMA_THERM_ENABLE_COMPLETED_VIA_RFLM,

   /*! THERM ERROR state */
   RFLM_WCDMA_THERM_ENABLE_ERROR

} rflm_wcdma_therm_enable_state_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of CGAGC parameters
 
*/
typedef struct
{
  /*! CGAGC Initial RSSI value for all all LNA states */
  uint16    cm_init[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! This threshold is used to switch from LNA state 2 and
     state1. If the CGAGC accumulation is less than lna_fall_1
     then the LNAs switch over to a high gain state of 1from a
     state of 2. */
  uint32    lna_fall_1;

  /*! This threshold is used to switch from LNA state 1 and state
     0. If the CGAGC accumulation is less than lna_fall_2 then
     the LNAs will switch over to a high gain state of 0 from a
     state of 1. */
  uint32    lna_fall_2;

  /*! This threshold is used to switch from LNA state 2 to state 3.
     If the CGAGC accumulation is greater than lna_rise_1
     then the LNAs will switch over to a low gain state of 3 from a
     state of 2. */    
  uint32    lna_rise_1;

  /*! This threshold is used to switch from LNA state 3 to state 4.
     If the CGAGC accumulation is greater than lna_rise_2
     then the LNAs will switch over to a low gain state of 4 from a
     state of 3. */
  uint32    lna_rise_2;

  /*! This threshold is used to switch from LNA state 4 to state 5.
     If the CGAGC accumulation is greater than lna_rise_3
     then the LNAs will switch over to a low gain state of 4 from a
     state of 3. */
  uint32    lna_rise_3;
} rflm_wcdma_cgagc_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific FGAGC parameters
 
*/
typedef struct
{

  /*! LNA rise threshold in dB10 unit */
  int32    lnarise[RFLM_WDMA_MAX_NUM_LNA_STATES-1];

  /*! LNA fall threshold in dB10 unit */
  int32    lnafall[RFLM_WDMA_MAX_NUM_LNA_STATES-1];

  /*! TX to RX frequency ratio in Q15. Use center channel Tx/Rx frequency to represent the band */
  uint16   tx_rx_freq_ratio;

  /*! Flag to indicate LNA control override from SW */
  boolean  lna_oride_flag;

  /* LNA gain state to override from SW, applicable when lna_orige_flag is set */
  uint8    lna_gain_state_oride;

}rflm_wcdma_fgagc_band_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Rx IQ mismatch compensation paramters
 
*/
typedef struct
{
  /*! flag to indicate if the iqmc compensation coefficients are valid  */  
  boolean iqmc_valid[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! IQ mismatch compensation coefficients per LNA state  */
  uint32 iqmc_coeff[RFLM_WDMA_MAX_NUM_LNA_STATES];

}rflm_wcdma_rx_iqmc_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition for jammer detect api
 
*/
typedef struct
{
  /*! rxagc noise floor squared*/
  int32    rxagc_noise_floor_sq;

  /*! rxagc satutation detect threshold */
  int32    rxagc_sat_det_threshold;

  /*! rxagc satutation detect gain hyst */
  int32    rxagc_sat_det_gain_hyst;

 /*!indicates if jammer detection enabled */
  boolean         jam_det_en;                 

}rflm_wcdma_rxagc_jam_det_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of channel specific FGAGC parameters
 
*/
typedef struct
{

  /*! LNA offset for all LNA gain states in dB10 unit*/
  int16    lna_offset[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! DVGA gain offset in dB10 unit*/
  int16    dvga_gain_offset;

  /*! Minimum RxAGC value per LNA state in dB10 unit */
  int16    rxagcmin[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! Maximum RxAGC value per LNA state in dB10 unit */
  int16    rxagcmax[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! Static DVGA gain offset from RXLM in dB10 unit*/
  int32	   static_dvga_offset[RFLM_WCDMA_MAX_NB_PER_WB];

}rflm_wcdma_fgagc_chan_param_type;

/*----------------------------------------------------------------------------*/
/*! @brief XO parameters for the WCDMA frequency */
typedef struct
{
  /*! Downlink frequency in Q6 Hz */
  uint64 dl_freq;

  /*! inverse of the downlink frequency to calculate ppm, 2^33/Downlink_frequency, Q19 */
  uint32 inv_dl_freq;
}rflm_wcdma_xo_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of internal state used by Rx AGC fgagc
 
*/
typedef struct
{
  /*! LNA decision run by RFLM AGC state machine */
  int8     lna_decision;

  /* Shadow of current dynamic IQMC configuration */
  uint32   dynamic_iqmc_shadow;

  /*! flag indicating if lna state changed */
  boolean  lna_switch;

  /*! Preserve the last LNA state */
  int8     lna_state_shadow;

  /*! current up delay count */
  uint16   lna_up_delay_cnt;

  /*! current down delay count */
  uint16   lna_down_delay_cnt;

  /*! current stay delay count */
  uint16   lna_stay_delay_cnt;

  /*! flag indicating if jammer present */
  boolean  jammer_flag;

  /*! current rx agc min value */
  int32 rx_agc_min;

  /*! current rx agc max value */
  int32 rx_agc_max;

  /*! flag indicating if first time fgagc run after cgagc */
  boolean fgagc_start_flag;

 /*get the time when the lna update is done*/
   rflm_ustmr_t previous_run_time;

}rflm_wcdma_rx_fgagc_state_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of device configuration needed for FW operation
 
  @details
  Device configuration includes
  1. device transaction script executed in FW context
  2. Timing parameters related to the script transactions
  3. Number of transactions in the script
 
  Script transaction timing can be
  1. built into the script itself (delay per
  transaction) which is done by device driver based on standalone device spec. This
  timing is transparent to RF-MDSP operations.
  2. based on system timing requirement to shift the timing of the entire script,
  the input is usually taken from NV. This timing will be added in this structure
  per script
  3. Some timing parameters are out of CCS event timing realm and controlled in the
  FW timeline. That will be included in other funtional paramter structures.
  
*/
typedef struct
{
  /*! device transaction script*/
  void* script;

  /*! delay of script execution in cx8 unit to a reference time*/
  int16 delay;

  /*! Number of transactions */
  uint8 num_trans;

}rflm_wcdma_device_script_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Rx device configuration needed for FW RxAGC
 
*/
typedef struct
{
  /*! LPM config*/  
  rflm_wcdma_device_script_type drx[RFLM_WCDMA_DRX_CTL_NUM];
   
  /*! LNA config*/     
  rflm_wcdma_device_script_type lna[RFLM_WDMA_MAX_NUM_LNA_STATES];

  /*! Wakeup config*/
  rflm_wcdma_device_script_type wakeup[RFLM_WCDMA_MAX_NUM_BUS_TYPE];

  /*! Wakeup script number of tasks */
  uint8 num_tasks_wakeup_script;

  /* Rx PLL settling time in STMR unit*/
  uint32 pll_settling_time;

  /* RF Warmup time in STMR unit (RF transcation time + PLL settling time) */
  uint32 rf_warmup_time;

}rflm_wcdma_rx_device_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific Rx config parameters
 
*/
typedef struct
{
  
  /*! FGAGC band specific paramters */
  rflm_wcdma_fgagc_band_param_type fgagc_band_param;
  
  /*! CGAGC paramters */    
  rflm_wcdma_cgagc_param_type cgagc_param;

  /*! IQ mismatch compensation paramters */
  rflm_wcdma_rx_iqmc_param_type iqmc_param;

  /*! Rx device config */ 
  rflm_wcdma_rx_device_cfg_type device_cfg;

  /*! LNA device index used in RF-FW LNA interface*/ 
  uint8 lna_idx;

  /*! Antenna index used in RF-FW RxAGC interface */ 
  uint8 ant_idx;

  /*! Multi-Carrier index: this is a bitmask and a direct mapping from L1-RF intf to RxAGC interface*/ 
  uint8 multi_car_idx;

  /*! BTF Delay in Cx8 unit(nominally around 40 chips) */
  int16 btf_delay;

  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

  /*! IRAT Measurement scenario*/ 
  rflm_wcdma_irat_meas_enum_type meas_scenario;

  /*!structure definition for jammer detect api*/
  rflm_wcdma_rxagc_jam_det_type jam_det_param;

}rflm_wcdma_rx_cfg_bsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of channel specific Rx config parameters

*/
typedef struct
{
  /*! FGAGC channel specific paramters */
  rflm_wcdma_fgagc_chan_param_type fgagc_chan_param;

  /*! XO channel specific paramters */
  rflm_wcdma_xo_param_type xo_chan_param;

  /*! LNA device index used in RF-FW LNA interface*/ 
  uint8 lna_idx;

  /*! Antenna index used in RF-FW RxAGC interface */ 
  uint8 ant_idx;

 /*! Multi-Carrier index: this is a bitmask and a direct mapping from L1-RF intf to RxAGC interface*/ 
  uint8 multi_car_idx;
   
  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

  /*! IRAT Measurement scenario*/ 
  rflm_wcdma_irat_meas_enum_type meas_scenario;
       
}rflm_wcdma_rx_cfg_chsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of wakeup Rx config parameters
 
*/
typedef struct
{
  
  /*! Rx device config */ 
  rflm_wcdma_rx_device_cfg_type device_cfg;

  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

}rflm_wcdma_rx_wakeup_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of wakeup Rx config parameters
 
*/
typedef struct
{
  /*! Execution time of Preload script in USTMR units */ 
  uint32 preload_script_exec_time;

  /*! Execution time of Trigger (TRx) script in USTMR units */ 
  uint32 trigger_script_exec_time;

  /*! Execution time of Non TRx (ASM/Tuner) script in USTMR units */ 
  uint32 non_trx_script_exec_time;

}rflm_wcdma_meas_config_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific PA parameters

*/
typedef struct
{
  /*! Active PA State */
  boolean active_pa_state[RFLM_WCDMA_MAX_NUM_PA_STATES];
  
  /*! PA rise threshold for all PA gain states in dB10 unit */
  int32 rise[RFLM_WCDMA_MAX_NUM_PA_STATES-1];

  /*! PA fall threshold for all PA gain states in dB10 unit*/
  int32 fall[RFLM_WCDMA_MAX_NUM_PA_STATES-1];

  /*! PA timer hysteresis delay in BPG unit when switching down PA gain state */
  int16 timer_hysteresis[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! PA phase comp from rotator fall all gain state. 360/256 in 2's complement format */
  int16 rot_angle[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! Flag to disable FW running TxAGC. Used in FTM Cal mode */
  boolean txagc_disable;

    /*! Flag to disable running TxAGC for FTM Mode*/
  boolean ftm_disable_ldo_off;

}rflm_wcdma_pa_band_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx MPR(max power ratio) bin based backoff paramters
  for HS waveforms 

*/
typedef struct
{
  /*! Beta backoff for MPR bin in Q15, 1.0 is 32767 */
  uint16 beta_backoff[RFLM_WCDMA_MAX_NUM_MPR_BINS];

  /*! MPR values for MPR bin in dB10, i.e. 0. 0.5dB, 1dB */
  uint8 mpr_val[RFLM_WCDMA_MAX_NUM_MPR_BINS];

  /*! TX AGC Offset for MPR bin, adj in txPdm, in dB10
      (Range -128->127 -> -10.7dB->10.6dB) */
  uint8 txagc_offset[RFLM_WCDMA_MAX_NUM_MPR_BINS];

  /*! PA switch point reduction per MPR value, MPR=0,0.5,1.0,...,2.5, in dB12 */
  uint8 pa_thresh_backoff[RFLM_WCDMA_MAX_NUM_MPR_BINS-1];

}rflm_wcdma_mpr_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of XPT paramters

*/
typedef struct
{
  /*! XPT Tracking mode: 0 = APT; 1 = EPT; 2 = ET */
  uint8 xpt_mode[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! Pin values */
  int16 pin[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! Pout values */
  int16 pout[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! SMPS Mode: 0-disabled (default), 1-TX PDM, 2-GPIO->GRFC */
  int8 smps_mode;

  /*! PA state (0/1/2/3) of which EPT is enabled, this also corresponds to the linearizer chosen for EPT */
  int8 ept_pa_state;

  /*! TX AGC IQ gain unit set point for APT */
  int16 iq_setpoint_apt;

  /*! TX AGC IQ gain unit set point for EPT */
  int16 iq_setpoint_ept;

  /*! TX AGC IQ gain unit set point for ET */
  int16 iq_setpoint_et;

  /*! TX AGC ENV scale unit set point for EPT */
  int16 env_scale_setpoint_ept;

  /*! TX AGC ENV scale unit set point for ET */
  int16 env_scale_setpoint_et;

  /*! TX AGC IQ gain unit set point for APT 10MHz Tx BW (DC)*/
  int16 iq_setpoint_apt_2x;

  /*! TX AGC IQ gain unit set point for EPT 10MHz Tx BW (DC)*/
  int16 iq_setpoint_ept_2x;

  /*! TX AGC IQ gain unit set point for ET 10MHz Tx BW (DC)*/
  int16 iq_setpoint_et_2x;

  /*! TX AGC ENV scale unit set point for EPT 10MHz Tx BW (DC)*/
  int16 env_scale_setpoint_ept_2x;

  /*! TX AGC ENV scale unit set point for ET 10MHz Tx BW (DC)*/
  int16 env_scale_setpoint_et_2x;

}rflm_wcdma_xpt_param_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx device configuration needed for FW TxAGC
 
*/
typedef struct
{
  /*! PA ON config. Timing:
    PA ON rise delay in cx8 unit. PA on time = frame boundary - 2BPG + pa_on_rise_delay*/
  rflm_wcdma_device_script_type pa_on;
   
  /*! PA OFF config. Timing:
    PA OFF fall delay in cx8 unit. PA off time = frame boundary + 2BPG + pa_on_fall_delay*/     
  rflm_wcdma_device_script_type pa_off;

  /*! WTR ON config. Timing: 
    WTR ON rise delay in cx8 unit. tx on time = frame boundary - 2BPG + tx_on_rise_delay
    */
  rflm_wcdma_device_script_type tx_on;
   
  /*! WTR OFF config. Timing: 
    WTR OFF fall delay in cx8 unit. tx off time = frame boundary + 2BPG + tx_on_fall_delay*/
  rflm_wcdma_device_script_type tx_off;

  /*! PA state config*/
  rflm_wcdma_device_script_type pa_state[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! DTx(Tx PLL) config*/
  rflm_wcdma_device_script_type dtx[RFLM_WCDMA_DTX_CTL_NUM];

  /*! WTR gain (RGI) config, template only */
  rflm_wcdma_device_script_type wtr_gain;

  /*! PA quiescent current config, template only*/
  rflm_wcdma_device_script_type pa_current;

  /*! PA bias config, template only*/
  rflm_wcdma_device_script_type pa_bias;

  /*! Power Meter Trigger */
  rflm_wcdma_device_script_type power_meter_trig;

  /*! Tx Tune event */
  rflm_wcdma_device_script_type tx_tune;

  /*! Enable Autopin FBRX Gain 1 event */
  rflm_wcdma_device_script_type autopin_fbrx_gain1;

  /*! Enable Autopin FBRX Gain 1 event */
  rflm_wcdma_device_script_type autopin_fbrx_gain2;

  /* WTR Tx PLL settling time in STMR unit */
  uint32 pll_settling_time;

  /* Offset time to execute Tx On/Off before frame/slot boundary in BPG*/
  /* This the start time of script execution, script itself will has finer
  resolution delay for each task */
  uint32 tx_on_off_offset_time;

  /* Tx tune script execution time in STMR unit */
  uint32 tx_tune_time;

  /* Therm Bin */
  uint8 therm_bin;

  /* Therm Reminder */
  uint8 therm_rem;

  /* therm_enable script */
  rflm_wcdma_device_script_type therm_enable;

}rflm_wcdma_tx_device_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific Tx config parameters
 
*/
typedef struct
{
  /*! PA-related band specific paramters */
  rflm_wcdma_pa_band_param_type pa_band_param;
  
  /*! Tx MPR parameters*/
  rflm_wcdma_mpr_param_type mpr_param;
  
  /*! XPT parameters */    
  rflm_wcdma_xpt_param_type xpt_param;

  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

  /* NPA client for Q6 clock voting */
  boolean npa_handle_initialised;       ///<Flag indicating if q6clk NPA handle is initialized.
  #if (RFLM_FEATURE_BUILD_MODE==RFLM_FEATURE_BUILD_NORMAL)
  npa_client_handle npa_handle_q6clk;   ///<NPA client handle
  #endif
}rflm_wcdma_tx_cfg_bsp_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx multi-carrier configuration parameters
 
*/
typedef struct
{
  /*! Per carrier TxAGC frequency compensation in dB10 for DC to be applied 
      by baseband - needed since the linearizer is built for LO frequency */
  int16 freq_comp[RFLM_WCDMA_MAX_NUM_TX_CARRIERS][RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! base value of Phase Delta Accumulator -> nominal 0 */
  int32 tx_pda_ctrl_base[RFLM_WCDMA_MAX_NUM_TX_CARRIERS];

  /*! nominal values C0(-): 182955, C1(+): 341333 
      round(2^22*frequency_offset(=2.5e6)/sampling_rate(=30.72e6)) = 341333 for C1 */
  int32 tx_pda_ctrl_offset[RFLM_WCDMA_MAX_NUM_TX_CARRIERS];

  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

}rflm_wcdma_tx_mc_cfg_type;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to be used to pass callback data to CCS */
typedef struct
{
  /*! TxLM handle */
  rflm_handle_tx_t handle;

} rflm_wcdma_meter_ccs_cb_data;

/*------------------------------------------------------------------------------------------------*/
/*! Structure to be used to pass callback data to CCS for RFLM THERM read*/
typedef struct
{
  /*! TxLM handle */
  rflm_handle_tx_t handle;

} rflm_wcdma_therm_enable_ccs_cb_data;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold the data pertaining to Tx Power Meter
*/
typedef struct
{
  /*! Flag indicating if the power meter is supported or not */
  boolean is_power_meter_supported;
  
  /*! Varibale to keep track of the Tx power meter trigger periodicity. This number will vary 
  based on the Tx power error */
  uint32 meter_trigger_period_us;

  /*! Variable to store the last Power meter trigger time in USTMR count units */
  rflm_ustmr_t last_trigger_time;

  /*! Variable to store the last Power meter event action time in USTMR count units */
  rflm_ustmr_t last_event_action_time;

  /*! Variable to store the TxAGC at the time when Power Meter is triggered in dB10 units */
  uint16 buffered_txagc_db10;

  /*! Variable to indicate if the HDET trigger is completed or not */
  boolean hdet_trigger_completed;

  /*! Variable to keep track of concurrent atomic operation*/
  uint32 concurrent_count;

  /*! Data structure to hold CCS callback data */
  rflm_wcdma_meter_ccs_cb_data ccs_cb_data;

} rflm_wcdma_tx_meter_cfg_type;



/*----------------------------------------------------------------------------*/

/*! 
  @brief
  Structure definition to hold the data pertaining to Autopin enable
*/
typedef struct
{
  /*! Variable to store the last enable autopin trigger time in USTMR count units */
  rflm_ustmr_t last_trigger_time;

  /*! Variable to store the TxAGC at the time when enable autopin is triggered in dB10 units*/ 
  uint16 buffered_txagc_db10;

  /*! Variable to store gain_state at the time when enable autopin is triggered in dB10 units*/ 
  uint8 gain_state;
 
  /*! Variable to indicate if the autopin trigger was scheduled in the last txagc update*/
  boolean autopin_enable_trigger_done;

  /*! Data structure to hold CCS callback data */
  rflm_wcdma_meter_ccs_cb_data ccs_cb_data;

  /*Structure to hold extra capture time in multiple of txc rollover period*/
  uint32 extra_capture_time;

} rflm_wcdma_autopin_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold the data pertaining to THERM_READ
*/
typedef struct
{
   /*! Variable that holds the current therm_read state */
   rflm_wcdma_therm_enable_state_type therm_enable_state;

   /*! Variable to store the last THERM_READ trigger time in USTMR count units */
   rflm_ustmr_t last_trigger_time;

   /*! Variable to store the last THERM_READ event action time in USTMR count units */
   rflm_ustmr_t last_event_action_time;

   /*! Data structure to hold CCS callback data */
   rflm_wcdma_therm_enable_ccs_cb_data ccs_cb_data;

   /*! Status flag to see if MSG got sent properly or not */
   boolean msg_sent_successful;

} rflm_wcdma_therm_enable_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of channel specific Tx config parameters
 
*/
typedef struct
{
  /*! Specifies slot worth TxAGC compensation during PA state transition up 
    Used to tweak ILPC glitch around PA switchpoint */
  int8 pa_comp_up[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! Specifies slot worth TxAGC compensation during PA state transition down
     Used to tweak ILPC glitch around PA switchpoint */
  int8 pa_comp_down[RFLM_WCDMA_MAX_NUM_PA_STATES];

  /*! XPT delay value */
  int32 xpt_delay;

  /*! ET path delay register value */
  rflm_txagc_et_path_delay_t et_delay_reg[2];

  /*! address of current ET path delay register value */
  uint32 et_path_delay_addr;

  /*! Tx device scripts */ 
  rflm_wcdma_tx_device_cfg_type device_cfg;

  /*! RFLM handle */ 
  rflm_dm_handle_id_t rflm_handle;

}rflm_wcdma_tx_cfg_chsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RxAGC trace log
 
*/

#ifdef FEATURE_RF_WCDMA_DEBUG
typedef struct
{
  /*! CGAGC input parameters */ 
  rflm_wcdma_rx_cgagc_update_in_t  cgagc_input;
  /*! CGAGC output parameters */ 
  rflm_wcdma_rx_cgagc_update_out_t cgagc_output;
  /*! FGAGC input parameters */ 
  rflm_wcdma_rx_fgagc_update_in_t  fgagc_input;
  /*! FGAGC output parameters */ 
  rflm_wcdma_rx_fgagc_update_out_t fgagc_output;
  /*! CGAGC start time */ 
  int32                            cgagc_start_time;
  /*! CGAGC execution time */ 
  int32                            cgagc_exec_time;
  /*! FGAGC start time */ 
  int32                            fgagc_start_time;
  /*! FGAGC execution time */ 
  int32                            fgagc_exec_time;
} rflm_wcdma_rx_trace_log_type;
#endif


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx status variables for communication between RF MDSP
  and RFLM
 
*/
typedef struct
{
  uint8 waveform;                   ///< Waveform type
  
  /*! flag to indicate Tx On/PA status. Written only from RFLM */
  boolean tx_on;

  uint32  txagc_fixup_skip_count;   ///< rflm txagc fixup skip counter

  boolean last_txagc_fixup_skipped; ///< Flag indicating that Last TxAGC was skipped

  /*! flag to indicate temperature compensation is in progress. Write only from 
    RF MDSP */
  boolean temp_comp_in_progress;

   /*! flag to indicate enable autopin is in progress. */
  boolean enable_autopin_in_progress;

  /*! flag to indicate HDET is in progress. Write only from 
    RFLM */
  boolean power_meter_in_progress;

  /*! flag to indicate FBRx run in the last TxAGC. Write only from 
    RFLM */
  boolean run_fbrx;

  /*! flag to indicate ILPC correction algorithm is to be enabled or not */
  boolean fbrx_ilpc;
  
  /*! Variable to store the slot boundary for this TxAGC update */
  uint32 slot_boundary;

  /*! Variable to store stable tx start time */
  uint32 stable_tx_start_time;

  /*! Variable to store stable tx stop time */
  uint32 stable_tx_stop_time;

  /*! Status of tuner AOL. Written only from RFLM */
  rflm_cmn_tuner_event_status_type tuner_aol_status;

  /*! Status of tuner CL. Written only from RFLM */
  rflm_cmn_tuner_event_status_type tuner_cl_status;

  /* FBRx GPS active state */
  rflm_fbrx_gps_state_type fbrx_gps_state;

  /* FBRx on off state */
  boolean fbrx_ldo_on_off_state;

  /* DAC IREF value */
  uint8 dac_iref_value;

  /*  Phase comp value from LUT */
  int16 phase_comp;

  boolean log_mem_alloc;   ///< mem_alloc true or false
  uint32  log_context_id;   ///< Context ID

  uint8 log_set_idx;
  uint8 log_get_idx;

}rflm_wcdma_tx_status_type;

typedef struct
{
  uint16 calc2effect[RFLM_WCDMA_TXAGC_BUFFER_COUNT];  ///< Array to hold time from fed calc to ccs effect (slot boundary)
  int8   index;                        ///< index for calc start to effect time value array
  uint32 avg_calc2effect;              ///< Moving Average of calc start to effect time
  uint8   emergency_counter;            ///< Timeline emergency counter
  boolean higher_q6_clk;                ///< status variable indicating FED has voted for higher Q6 clock
  boolean callback_vote_request;        ///< flag to vote for higher Q6 clock
  boolean callback_vote_clear;          ///< flag to clear higher Q6 clock vote
  uint8   vote_clear_counter;           ///< clear vote hystersis counter
}rflm_wcdma_tx_time_profile_type;

typedef struct 
{
  rflm_err_t  txagc_update_err;     ///< rflm txagc err type

  uint8  waveform;     ///< Waveform type
  uint8  pa_state;     ///< pa_state
  uint8  pa_action;    ///< pa_action
  int16  out_pwr;      ///< final output power
  uint8  mpr_bin;      ///< txagc mpr bin
  uint8  xpt_mode;     ///< xpt_mode
  int8   txagc_offset; ///< txagc_offset for MPR bin
  uint8  pa_mpr;       ///< PA MPR shift factor for MPR bin
  int8   pa_comp;      ///< pa_comp value
  uint32 iq_gain;      ///< iq_gain
  uint32 env_scale;    ///< env_scale
  uint8  rgi_idx;      ///< RGI row index
  uint8  smps_idx;     ///< SMPS row index
  uint8  dpd_idx;      ///< DPD table index
  int8   pin;          ///< pin;
  int8   pout;         ///< pout;
  uint8  hys_cnt;      ///< pa_hystersis count
  rflm_err_t  err;     ///< rflm txagc err type
  boolean run_fbrx;    ///< run fbrx

  uint8  therm_bin;    ///< therm bin
  uint8  therm_rem;    ///< therm reminder
  
  rflm_ustmr_t ccs_action;     ///< CCS trigger time
  rflm_ustmr_t ccs_effect;     ///< CCS effect time (from FW)

  rflm_ustmr_t calc_start;     ///< fed_calc start
  rflm_ustmr_t calc_et_dac_1;  ///< fed_calc et_dac prog part1 end
  rflm_ustmr_t calc_et_dac_2;  ///< fed_calc et_dac prog part2 end 
  rflm_ustmr_t calc_fbrx_ldo;  ///< fed_calc fbrx ldo event end
  rflm_ustmr_t calc_end;       ///< fed_calc end

  rflm_ustmr_t update_start;           ///< fed_update start
  rflm_ustmr_t update_pa_stm;          ///< fed_update pa_stm end
  rflm_ustmr_t update_txagc_lookup;    ///< fed_update txagc lookup end
  rflm_ustmr_t update_fixup_deadline;  ///< fed_update fixup deadline end
  rflm_ustmr_t update_modem_cfg;       ///< fed_update modem cfg end
  rflm_ustmr_t update_dac_iref;        ///< fed_update dac iref end
  rflm_ustmr_t update_end;             ///< fed_update end

  boolean sos;     ///< Timeline emergency SOS (TRUE => adjust effect time and disable FBRx)
  uint16  deficit; ///< deficit time in USTRM units that is typically available from calc to effec min=>3us max=>10us
  uint32  txagc_fixup_skip_count;  ///< Txagc fixup skip count;

  uint32 reserved;
  
}rflm_wcdma_tx_logging_type;

typedef struct
{
  /*! Rx band config */ 
  rflm_wcdma_rx_cfg_bsp_type rx_bsp_cfg;

  /*! Rx channel config */ 
  rflm_wcdma_rx_cfg_chsp_type rx_chsp_cfg;

  /*! Rx wakeup config */
  rflm_wcdma_rx_wakeup_cfg_type rx_wakeup_cfg;

    /*! Rx state variables */
  rflm_wcdma_rx_fgagc_state_param_type rx_fgagc_state;

 #ifdef FEATURE_RF_WCDMA_DEBUG
  /*! Rx debug logging */
  rflm_wcdma_rx_trace_log_type rx_agc_trace_log[RFLM_WCDMA_TRACE_LOG_SIZE];
 #endif

  /*! current log buffer counter for CGAGC */
  uint8 cgagc_log_counter;

  /*! current log buffer counter for FGAGC */
  uint8 fgagc_log_counter;

  /*! Flag to indicate that FGAGC update is in progress*/
  boolean rxagc_fgagc_update_in_progress; 
  
  /*! Flag to indicate that AGC scipts are being pushed into DM*/
  boolean config_rxagc_in_progress;
 
  /*! IRAT (Measurement) paramaters that need to be configured*/
  rflm_wcdma_meas_config_type meas_config;

}rflm_wcdma_rx_dm_buf_type;

#define RFLM_W_TX_LOG_BUFFER_SIZE 3
typedef struct
{
  /*! Tx band config */ 
  rflm_wcdma_tx_cfg_bsp_type tx_bsp_cfg;

  /*! Tx band config */  
  rflm_wcdma_tx_cfg_chsp_type tx_chsp_cfg;

  /*! Tx multi-carrier config */ 
  rflm_wcdma_tx_mc_cfg_type tx_mc_cfg;

  /*! Tx Power meter configuration data */
  rflm_wcdma_tx_meter_cfg_type meter_cfg;

  /*! Autopin configuration data */
  rflm_wcdma_autopin_cfg_type autopin_cfg;

  /*! Tx status info */
  rflm_wcdma_tx_status_type tx_status;

  /* Tx Logging info */
  rflm_wcdma_tx_logging_type log[RFLM_W_TX_LOG_BUFFER_SIZE];

  /*! Tx timeline info */
  rflm_wcdma_tx_time_profile_type time;

  /*! Therm read config data */
  rflm_wcdma_therm_enable_cfg_type therm_enable_cfg;

}rflm_wcdma_tx_dm_buf_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  WCDMA FED DM interface structure
 
  @details
  This is the meta structure of RF MDSP-FW data structure. Since most of the
  pamrameters are moved to FED DM, the data structure will be copied to DM as
  is (excluding device scripts) based on LM handle
  
*/
typedef union
{
  rflm_wcdma_rx_dm_buf_type rx_data;

  rflm_wcdma_tx_dm_buf_type tx_data;

  /*! RFLM handle */ 
}rflm_wcdma_dm_buf_type;


/*==============================================================================
  ccs init related API prototypes to be called from sw
==============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @detail

  @param handle

  @param cell_id

  @return TRUE if succeeds, FALSE otherwise
*/
/*===========================================================================*/
extern boolean 
rflm_wcdma_create_rxagc_tasks
(
  rflm_handle_rx_t handle
);

/*----------------------------------------------------------------------------*/
/*@ brief rflm wcdma ccs init procedure

  @detail
  The API will initialize rflm wcdma ccs taskQ for both tx and rx, e.g.request
  tq handle and assign the starting tq address;

  This API should be called by MC layer

  @param void ( for now)
  
  @return void
*/
/*===========================================================================*/
boolean
rflm_wcdma_ccs_init
(uint8 device);

/*----------------------------------------------------------------------------*/
/*@ brief rflm wcdna de-init/release procedure

  @param void ( for now) unless we would like to 
  keep count of how mant tasq pars have been created
  
  @return void
*/
/*===========================================================================*/
void
rflm_wcdma_ccs_exit
(void);

/*----------------------------------------------------------------------------*/
/*@ brief  commit issue_seq sequnce to ccs

  @detail
  The API will be called ahead after the script is created for the event

  @param handle
  rflm_dm_handle_id_t

  @param rf_tq
  event id
  
  @return TRUE if succeeds, FALSE otherwise;
*/
/*===========================================================================*/
rflm_ccs_rf_seq_err_e rflm_wcdma_rf_ccs_mem_init
(
  rflm_dm_handle_id_t handle_id,
  rflm_wcdma_event_id_type event_id,
  uint16 event_max_size
);



/*----------------------------------------------------------------------------*/
/*@ brief return a tq pair for w to use

  @detail
  The API will call common rflm API to reserve device association with TQ pair

  @param void
  
  @return number of tq pair, either 0 or 1
*/
boolean rflm_wcdma_release_tq_pair_reserve_device
(void);


/*----------------------------------------------------------------------------*/
/*@ brief return a tq pair for w to use

  @detail
  The API will call common rflm API to reserve device association with TQ pair

  @param void
  
  @return number of tq pair, either 0 or 1
*/
boolean rflm_wcdma_tq_pair_disassociate_device
(void);


/*----------------------------------------------------------------------------*/
/*@ brief return a tq pair for w to use

  @detail
  The API will release and disassociate the TQ pairs

  @param void
  
  @return pass or failure
*/
boolean rflm_wcdma_release_tq_pair
(uint8 device);

/*------------------------------------------------------------------------------------------------*/
boolean
rflm_wcdma_try_lock_meter_trigger
(
  rflm_handle_tx_t handle
);

/*------------------------------------------------------------------------------------------------*/
boolean
rflm_wcdma_unlock_meter_trigger
(
  rflm_handle_tx_t handle
);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will attempt to abort Tuner AOL event

  @details
  This API will attempt abort Tuner AOL event.
 
  @param tech_id
  1x or HDR
 
  @param txlm_handle 
  The lm_handle for which event deallocation is required
*/
void
rflm_wcdma_abort_tuner_open_loop_update
(
  rflm_handle_tx_t txlm_handle
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will abort the pending AOL snums

  @details
*/
void
rflm_wcdma_abort_tuner_open_loop_pending_snums
(
  void
);

void
rflm_wcdma_ccs_tx_exit
(
  uint32 rflm_handle
);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_API_WCDMA_H */
