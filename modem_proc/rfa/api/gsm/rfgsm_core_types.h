#ifndef RFGSM_CORE_TYPES_H
#define RFGSM_CORE_TYPES_H

/*!
   @file
   rfgsm_core_types.h

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 2008 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/gsm/rfgsm_core_types.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends
04/23/15   par     Fix incorrect AFC being applied in GBTA cases
01/16/14   ec      Add new flags for debug NV mechanism 
12/19/14   hoh     LIF data implementation
11/18/14   sw/sc   Deprecate legacy state machines and move cmd buff out of rfa
11/12/14   sw      Rename RX/TX script types to reflect actual devices
10/21/14   sc      Reduce code execution limiting use of get qsym API
10/01/14   tws     Remove unused define
09/24/14   hoh     Add multislot APT control feature
09/19/14   sb      Add compatibility for IP2 cal moving averaging + pass/fail status
09/12/14   sml     Adding start API Pcycle to know about pre-emption done to RF task
09/09/14   ch      Mainline FEATURE_RF_HAS_QTUNER 
08/30/14   sw      Use correct conversion macro for irat x2g preload offset
08/20/14   ck      Fix GSM FTM loggoing together with FW support
08/14/14   zbz     Add SMPS/ICQ/temp_rem to FTM log packet
08/04/14   cj      Added tx sharing under common feature flag definition
07/30/14   cj      Added changes for Tx sharing feature
07/30/14   hoh     Updates for enhanced Rx mode
07/28/14   tsr     DR-DSDS: Add support for device driver to handle worst case rx alpha for IDLE mode RX
07/28/14   ec      Add jammer detector scripts to core handle 
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/12/14   sw      Replace RFGC_T_ERR macro with a function (to reduce memory footprint)
07/09/14   ck      revert max slot number to avoid mismatch in the fw and sw interface
07/09/14   ck      Fix GSM FTM loggoing due to inconsistent tx slot number between embedded SW and tool sides
07/09/14   sw      Introduce new macro to either error fatal or print message depending on nv bit
07/08/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/08/14   ec      Add types for new metrics processing interface 
05/29/14   sb      IP2 cal bug fixes, updates
05/19/14   tsr     Added emums for RX PLL writes and RX PLL settling time
05/15/14   sw      Software profiling code
04/24/14   ch      Added new script type TX_FE_CONFIG_SCRIPT and RX_FE_CONFIG_SCRIPT
04/24/14   ggs     Port CR581886: Add logging parameter to track coexistence 
                   backoff passed from GL1 
04/16/14   sb      IP2 cal Interface and functionality changes
04/10/14   dps     Added new variables to rfgsmlib_sbi_burst_struct to store 
                   autoA and autoB script execution time 
03/26/14   tsr     Add support to query rfm device for TX or RX capability 
03/26/14   sb      Add support for IP2 cal for Dime PM 
03/19/14   hoh     Added support for 8 PA gain states; removed unused vbatt code
03/11/14   tsr     Added RXD support for RX burst 
03/10/14   tsr     Mainline v2 device scripts for dynamic CCS programming 
03/10/14   tsr     Add support to program multiple device script through a CCS event 
03/10/14   sc      Temp fix Rx alpha to 190 derived from Rx start delta + margin
03/06/14   sc      Remove dependency on rfmeas_sbi_type
02/28/14   sc      Pass core data through mc to core layers
02/25/14   tsr     Update RX Alpha based on wtr writes and pll settling time from device driver 
02/24/14   tsr     Deprecate Feature D3925 
02/24/14   tsr     Moved Macro for Number of GBs to core layer (from rfgsm msp layer)
02/18/14   sc      Add table definitions removing dependency on FW 
02/17/14   tsr     Added PLF setting time in QS to rfgsmlib_sbi_burst_struct
02/10/14   cj      rfgsm_buffer_type flag added to decide whether to use home/away buffer
01/27/14   sc      Remove GFW dep on maximum number of Rx writes
01/21/14   tsr     WTR TXAGC support for Multislot TX
12/26/13   hm      Added #define for tech scaling factor for accurate transaction time calculations
11/19/13   cj      Added non trx script ptr in rfgsm_mdsp_scripts_ptrs
11/13/13   cj      Added trigger script ptr in rfgsm_mdsp_scripts_ptrs
11/11/13   cj      Added defintion for max preload offset time
10/29/13   tsr     TX Transition buffer script v2 for Bolt 
10/23/13   tsr     Mainline D3295 changes for BOLT 
09/28/13   tsr     Added support to retrieve individual device scripts 
08/28/13   ec      Creating RF GSM Core state machine with functions to init and
                   drive 
08/27/13   sb      Add changes to update RX timing for GSM RXD 
08/13/13   sb      GSM RXD changes 
07/29/13   sml/svi ASD support for GSM tech
07/29/13   tws     Initial support for RX diversity commands
07/26/13   tws     Move therm read log macro to rftech_gsm
07/22/13   ec	   Create RF GSM Core F3 macros to print device and task info
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.  
07/12/13   av      GTA EXIT mode 
07/08/13   svi     Closed Loop Mode Fix.
07/08/13   svi     Adding Support of Closed Loop Mode for Antenna Tuner.
06/28/13   ec      Add the rf task context to the command logging, add macros to encompass common NULL 
				   pointer checking operations.
06/24/13   ec      Log the rf_task context commands are executing in.
06/20/13   ec      Add support for GSM Tune Away enter mode
05/30/13   tws     Move the therm_read struct to rfgsm_core_handle.h
04/24/13   cj      Added build_reset_scripts and pre_config to API enums
04/18/13   sb      Add FTM hook for overriding PA scaling,smps bias 
04/09/13   sr      removed legacy antenna tuner code which is not used.
04/09/13   tsr     Added RF APPS->RF Task MSGR framework for temp comp to make temp comp data thread safe
03/28/13   aka     GSM frame sweep feature changes.
03/27/13   tsr     Use device driver api to extract therm read from retrieved ssbi buffer
03/27/13   sb      Add support for Enhanced APT  
03/18/13   sc      Add definitions for core handle defaults
03/14/13   ec      Add rf warmup time to rf gsm timing info struct  
03/12/13   tsr     Added to tag to each therm read event handle to identify the successful retrieval of that handle 
03/05/13   tsr     Fix the bug which was corrupting rfc_gsm_core_ptr as a part of rfgsm_core_handle
02/25/13   sc      Added cm enter/exit API enums
02/22/13   sb      Adding Vbatt adc client status flag 
02/12/13   tsr     Ensure that Temp comp calculation is thread safe  
02/11/13   sc      Added enums for logging MSGR confirmations
02/11/13   sc      Added GSM mode type to keep track of Standalone/iRAT modes
01/22/12   sb      Changes to enable QPOET transactions to go out every gsm frame 
01/14/13   tsr     Added changes for Therm read implementation  
01/11/13   aka     Added support for simultaneous Tx SAR
01/09/13   sr      Removed un-used vbatt flags.  
01/08/13   ggs     RF_ON needs to be added to a new dedicated transition event. 
                   Remove MDSP TX overhead. This is now acoounted for in GFW 
01/03/13   sc      Added therm read full call flow framework  
12/20/12   sc      PLL is provided in QS so no need to convert from US to QS
12/20/12   sc      Use the RFGSM max SBI value to define the max num SBIs
12/14/12   sc      Added support for Exit Mode
12/14/12   sc      Removed support for RF Task iRAT startup and cleanup 
12/06/12   tws     Fix Klocwork errors.
11/26/12   aka     Changes to use a path specific gsm handle
11/23/12   tsr     Tx Alpha and TX SBI start delta update for DIME based on wtr1605 
11/19/12   sc      RF-Only mDSP update shared variables cleanup 
11/07/12   sc      Add Rx Alpha calc macro for G-standalone and iRAT
11/07/12   sc      Cleanup unrequired definition
11/06/12   sc      Cleanup unrequired features/definitions
10/12/12   sc      Added iRAT Startup/Cleanup enum for execution time logging
10/05/12   tsr     First cut of RF time profile.
10/04/12   sr      Added IRAT related API enums for logging. 
09/25/12   tc      Mainline RF_HAS_RFA_GSM_TASK. 
09/06/12   ggs     Removed ftm_tx_cal_set_band. Set Tx band should always 
                   be called from GL1 and not from ftm directly 
08/16/12   ec      Cleanup of FTM set pa range implementation
08/09/12   ggs     Added ftm_tx_cal_set_band
08/08/12   ec      Modified rfgsm_core_ftm_slot_override_type and added path delay 
                   member variable to reflect changes in RF-side FTM processing
08/07/12   sc      Seperate Rx and Tx scripts for CCS buffer tracability
08/07/12   sc      Remove unused script pointers
07/30/12   sc      Added temporary gain parameter struct to enable FW SU release
07/18/12   ec      Added rfgsm_core_ftm_slot_override_type and added array of same 
                   to rfgsm_card, to be used for holding FTM override values. 
07/12/12   shb     Include rf_buffer_intf.h to fix compiler error
07/11/12   shb     Updated script_buffer to be of rf_buffer_intf* type 
                   instead of void ptr
07/06/12   sc      Added Tx transition pointer for RFC GRFC script
07/06/12   sc      Added variables for GSM RF CCS framework
07/03/12   sc      Added type definitions for RF files using rfmeas_sbi_type
06/28/12   sr      Added start/stop rf_buffer ptrs for GRFCs
06/27/12   sc      Added definition to accommodate coordinated merge FW/SW/RF
06/11/12   sc      Enable RF Task shared memory support
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling 
02/28/12   dej     Hooked up PA start/stop delta adjustments in FTM mode. 
                   ( Added rfgsm_card.tx_timing_adjust_flag ) 
02/17/12   sn      Added support for SMPS PDM control in FTM mode
02/10/12   sb      Changes to support FTM logging for GSM 
01/10/12   sr      changes to enable vbatt comp.
01/05/12   sr      geran api changes for non-signaling.
10/27/11    sr     Added SAR state logging for tx F3 msgs.
10/17/11    sr     made changes add F3 msgs via DPC task for isr overrun issue.
10/13/11    sr     made changes add F3 msgs via DPC task.
10/11/11    sr     Added to log RFGSM_MC functions time profile,
09/15/11    sr     added antenna tuner support code.
08/15/11    sr     klockwork fixes: moved the consts to this file as it is 
                   referenced from more files.
07/11/11    sr     removed un-used code.
03/23/11    sr     made the c++ compatible header file.
03/21/11    sr     Added timing info access interface.
01/17/11   dej     Mainline RF_DEVICE_MERGE_INTERIM_FEATURE
06/24/10   rsr    support for mdm9k/scmm merge
06/16/10   rsr    Removing KV_vs_temp code.
03/19/10   lcl    Pull in changes from QSC6295
02/04/10   sr     Lint & kw fixes 
01/20/10   sr     Removed un-used code.
01/20/09   tws     Move sigma_delta_max_db value to RFC card file.
15/01/10   tws    Add high/low band PA switch GRFC support.
12/08/09   sr     removed un-used variables.
12/05/09   sr     changed the name from RFGSM_MAX_SLOTS_PER_FRAME to 
                  RFGSM_MAX_TX_SLOTS_PER_FRAME
12/02/09   dyc    Featurize for C2K only build.
12/02/09   sr     removed un-used variables to fix lint warnings.
10/26/09   sr     Compiler warnings fix
07/0/09    sr     Added the RF_SIGMA_DELTA_MAX_dB 
07/06/09   sr     changes to make the operating band to rfcom_gsm_band type
03/27/009  sr     moved the GSM CTL related defines to rfc_card_gsmctl
03/16/09   sr     removed commented out code.
03/13/09   sr     code cleanup for SCMM build
11/11/08   sr     Initial version to separate gsm specific type definitions.

============================================================================*/


/*! @todo satheesha: THIS FILE NEED TO BE CLEANED-UP: need Sean's help with this
*/

#include "rfcom.h"
#include "mdsp_intf.h"

#ifdef FEATURE_MODEM_TX_SHARING
#define FEATURE_TX_SHARING
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define RFGSM_MAX_TX_SLOTS_PER_FRAME   MDSP_MAX_UL_TS+1
#define RFGSM_DEVICE_MAX_TX_SLOTS RFGSM_MAX_TX_SLOTS_PER_FRAME
#define RFGSM_DEVICE_MAX_IP2_CAL_STEPS 81
#define RFGSM_DEVICE_MAX_JDET_READS_PER_RX 8

/* AM/AM and AM/PM tables entries for the polar RF path */
#define RFGSM_AM_AM_TABLE_ENTRIES  256
#define RFGSM_AM_PM_TABLE_ENTRIES  256

#define RFGSM_MAX_DPLL_GUARD_BITS  70

/*Minimum RX PLL writes*/
#define RFGSM_MIN_RX_PLL_AUTOB_WRITES 0

/*Minimum RX PLL settling time in QS */
#define RFGSM_MIN_RX_PLL_SETTLING_TIME  141

/* LNA gain switch point values when enhanced Rx is enabled */
#define RF_GAIN_ENH_RX_ENABLED_LNA_SWPT  -1680  /* -105 dBm */
#define RF_LIF_FREQ_OFFSET_DEFAULT_HZ   135000

#define RFGSM_MAX_UNIQUE_RFCOM_BANDS (RFCOM_NUM_GSM_BANDS - 1)

/* AM/AM and AM/PM tables for the polar RF path */
typedef struct
{
  uint16 am_am_table[4][RFGSM_AM_AM_TABLE_ENTRIES];
  uint16 am_pm_table[RFGSM_AM_PM_TABLE_ENTRIES];
}rfgsm_amam_ampm_tables_type;

typedef enum
{
  RX_WTR_SCRIPT,
  RX_ASM_SCRIPT,  
  RX_XSW_SCRIPT,
  RX_GRFC_SCRIPT,
  RX_ATUNER_SCRIPT,
  RX_MAX_SCRIPT
}rfgsm_rx_script_type;

typedef enum
{
  TX_WTR_SCRIPT,
  TX_ASM_SCRIPT,
  TX_PA_SCRIPT,  
  TX_APT_SCRIPT,
  TX_GRFC_SCRIPT,
  TX_ATUNER_SCRIPT,
  TX_MAX_SCRIPT
}rfgsm_tx_script_type;

typedef enum
{
  TX_TRANS_WTR_SCRIPT,
  TX_TRANS_FE_SCRIPT,
  TX_TRANS_MAX_SCRIPT
}rfgsm_tx_transition_script_type;


typedef struct
{
  uint16 txCfg2GainSetupEnvGain;
  uint16 txPaSlotGrfcState[4];
  uint16 txCfg2GainSetupPredist;
  uint16 txCfg2GainSetupPaScale;
  uint16 txCfg2GainSetupAmamTblIdx;
  /*QPA Support*/
  uint8 txSlotGainVal1;
  uint8 txSlotGainVal2;
  /*Qtuner Closed Loop Support*/
  int32  txPower;  
}rfgsm_GfwTxRfGainParam;

typedef struct
{
  /* enter mode script ptr or IRAT preload script ptr */
  void *enter_mode_script;
  void *tx_band_script;
  void *therm_read_script;
  /*script to retrieve the therm read ssbis*/
  void *process_read_therm_script;

  /*Tx CCS Script Pointers*/
  void *apt_script[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  void *tx_agc_trigger_gmsk_script[RFGSM_DEVICE_MAX_TX_SLOTS];
  void *tx_agc_trigger_8psk_script[RFGSM_DEVICE_MAX_TX_SLOTS];

  void *tx_trans_script[TX_TRANS_MAX_SCRIPT][RFGSM_MAX_TX_SLOTS_PER_FRAME];

  /*Rx CCS Script Pointers*/
  void *rx_start_script[RX_MAX_SCRIPT];
  void *rx_stop_script[RX_MAX_SCRIPT];

  /*Tx CCS Script Pointers*/
  void *tx_start_script[TX_MAX_SCRIPT];
  void *tx_stop_script[TX_MAX_SCRIPT];

  /* IRAT trigger script ptr */
  void *meas_trigger_script;
  void *meas_nontrx_script;

  /* IP2  cal related script ptr*/
  void *ip2_cal_setup_script;
  void *ip2_cal_step_script[RFGSM_DEVICE_MAX_IP2_CAL_STEPS];
  void *ip2_cal_cleanup_script;

  /* JDET read script ptr array */
  void *jdet_read_script[RFGSM_DEVICE_MAX_JDET_READS_PER_RX];

#ifdef FEATURE_TX_SHARING
  /* Tx sharing changes */
  /*===================*/
  /* rx enter mode script ptr */
  void *rx_enter_mode_script;
  
  /* tx enter mode preload script ptr */
  void *tx_enter_mode_script;
  
  /* tx enable script ptr */
  void *tx_enable_script[TX_MAX_SCRIPT];
    
  /* tx disable script ptr */
  void *tx_disable_script[TX_MAX_SCRIPT];
#endif
}rfgsm_mdsp_scripts_ptrs;

typedef enum
{
  BYPASS_MODE,
  REGULAR_APT_MODE,
  FORCED_APT_MODE,
  INVALID_MODE = 0xFF
} apt_mode_type;



/* US_TO_QS_SCALED = 1/0.923 << 16                                  */
/* US_TO_QS_SCALED_SBI_TIME = US_TO_QS_SCALED*2.4                   */
/* SCALED_ROUNDING_FACTOR = 0.923 << 16 (ensuring always round up)  */
#define US_TO_QS_SCALED                   71003
#define US_TO_QS_SCALED_SBI_TIME          170407
#define SCALED_ROUNDING_FACTOR            60489

// NUM_QS_PER_100_USEC = 1/0.923 * 100
#define NUM_QS_PER_100_USEC	108

/*Multibyte RFFE Write Time in QS*/
#define MB_RFFE_TRANSACTION_TIME	4.44

/* DC Cal delay during RX burst*/
#define RX_DC_CAL_DELAY_QS 54

/* RFFE Bus delay due to preemption by higher priority rffe transactions. 
   The number taken is WTR GSM TX burst RFFE execution time in QS */
#define RFGSM_WORST_CASE_RFFE_BUS_DELAY 137

#define RFGSM_MB_RFFE_TIME_CALC_QS(num_rffes) \
              (uint16)(num_rffes * 4.44) \

#define RFGSM_SBI_TIME_CALC_QS(num_sbis) \
              (uint16)(((num_sbis * US_TO_QS_SCALED_SBI_TIME) + SCALED_ROUNDING_FACTOR) >> 16) \

#define RFGSM_PLL_TIME_CALC_QS(pll_tune_time) \
              (uint16)(((pll_tune_time * US_TO_QS_SCALED) + SCALED_ROUNDING_FACTOR) >> 16) \

#define RFGSM_RF_RX_SETUP_TIME_QS(num_rffes, pll_tune_time) \
              RFGSM_MB_RFFE_TIME_CALC_QS(num_rffes) + pll_tune_time

#define RFGSM_KV_LENGTH_CALC_QS(num_gb) \
               (uint16)(num_gb*4)     \

/* TX SBI start time = num of kv length symbols + Time taken by sbis that go before KV training sequence starts.
  The number of sbis is zero for wtr1605. added for future targets
*/
#define RFGSM_TX_SBI_START_DELTA_QS(num_gb, num_pll_sbis) \
                  -(RFGSM_KV_LENGTH_CALC_QS(num_gb) + RFGSM_SBI_TIME_CALC_QS(num_pll_sbis)) \

#define RFGSM_RF_TX_SETUP_TIME_QS(num_gb,num_pll_sbis) \
                (-(RFGSM_TX_SBI_START_DELTA_QS(num_gb, num_pll_sbis)))\

#define RFGSM_MAX_RX_START_SBI_TRANSACTIONS 38

/*@TODO device driver to investigate whether this is needed*/
#define MDSP_MAX_SBI_TRANSACTIONS 60

#define RFGSM_PRELOAD_OFFSET_TIME_CALC_QS(preload_time) \
              (uint16)(((preload_time * US_TO_QS_SCALED) + SCALED_ROUNDING_FACTOR) >> 16) \



/*----------------------------------------------------------------------------*/
/*
  @brief
  This enum is used by GSM RF to differentiate between the modes of operation

  @details
  RFGSM_DEFAULT_MODE:     Default init value used when not actively operating for GSM
  RFGSM_STANDALONE_MODE:  GSM-only stand alone operation where GSM is the only active RAT
  RFGSM_IRAT_MODE:        When doing GSM measurement activity in X2G gap for iRAT
                          Can also be used to prevent therm read during iRAT
*/
typedef enum
{
  RFGSM_DEFAULT_MODE,
  RFGSM_STANDALONE_MODE,
  RFGSM_IRAT_MODE,
  RFGSM_GTA_MODE,
  RFGSM_MAX_MODE
}rfgsm_mode_type;

typedef enum
{
  RFGSM_DEFAULT_BUFFER,
  RFGSM_HOME_BUFFER,
  RFGSM_AWAY_BUFFER,
  RFGSM_MAX_BUFFER
}rfgsm_buffer_type;

typedef enum
{
  RFGSM_FREQ_ERR_RX_ONLY,
  RFGSM_FREQ_ERR_TX_ONLY,
  RFGSM_FREQ_ERR_MAX
}rfgsm_freq_err_type;

typedef struct 
{
  uint8 tx_supported_band_mask;
  uint8 rx_supported_band_mask;
} rfgsm_core_device_capability_type;


/* These macros are to enable automatic printing of device and task information in F3 messages from RF. These macros require rfm_dev and rf_task to
   be defined and valid in the calling function */
#define RFGC_MSG(lvl, txt)                                                            MSG_1( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev)
#define RFGC_MSG_1(lvl, txt, arg1)                                                    MSG_2( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1)
#define RFGC_MSG_2(lvl, txt, arg1, arg2)                                              MSG_3( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2)
#define RFGC_MSG_3(lvl, txt, arg1, arg2, arg3)                                        MSG_4( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2, arg3)
#define RFGC_MSG_4(lvl, txt, arg1, arg2, arg3, arg4)                                  MSG_5( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2, arg3, arg4)
#define RFGC_MSG_5(lvl, txt, arg1, arg2, arg3, arg4, arg5)                            MSG_6( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2, arg3, arg4, arg5)
#define RFGC_MSG_6(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6)                      MSG_7( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2, arg3, arg4, arg5, arg6)
#define RFGC_MSG_7(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6, arg7)                MSG_8( MSG_SSID_RF, lvl, "|dev:%d|" txt, rfm_dev, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define RFGC_T_MSG(lvl, txt)                                                          MSG_1( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task)
#define RFGC_T_MSG_1(lvl, txt, arg1)                                                  MSG_2( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1)
#define RFGC_T_MSG_2(lvl, txt, arg1, arg2)                                            MSG_3( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2)
#define RFGC_T_MSG_3(lvl, txt, arg1, arg2, arg3)                                      MSG_4( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2, arg3)
#define RFGC_T_MSG_4(lvl, txt, arg1, arg2, arg3, arg4)                                MSG_5( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2, arg3, arg4)
#define RFGC_T_MSG_5(lvl, txt, arg1, arg2, arg3, arg4, arg5)                          MSG_6( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2, arg3, arg4, arg5)
#define RFGC_T_MSG_6(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6)                    MSG_7( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2, arg3, arg4, arg5, arg6)
#define RFGC_T_MSG_7(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6, arg7)              MSG_8( MSG_SSID_RF, lvl, "|tsk:%d|" txt, rf_task, arg1, arg2, arg3, arg4, arg5, arg6, arg7)

#define RFGC_L_MSG(lvl, txt)                                                          MSG_2( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task)
#define RFGC_L_MSG_1(lvl, txt, arg1)                                                  MSG_3( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1)
#define RFGC_L_MSG_2(lvl, txt, arg1, arg2)                                            MSG_4( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2)
#define RFGC_L_MSG_3(lvl, txt, arg1, arg2, arg3)                                      MSG_5( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2, arg3)
#define RFGC_L_MSG_4(lvl, txt, arg1, arg2, arg3, arg4)                                MSG_6( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2, arg3, arg4)
#define RFGC_L_MSG_5(lvl, txt, arg1, arg2, arg3, arg4, arg5)                          MSG_7( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2, arg3, arg4, arg5)
#define RFGC_L_MSG_6(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6)                    MSG_8( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2, arg3, arg4, arg5, arg6)
#define RFGC_L_MSG_7(lvl, txt, arg1, arg2, arg3, arg4, arg5, arg6, arg7)              MSG_9( MSG_SSID_RF, lvl, "|dev:%d|tsk:%d|" txt, rfm_dev, rf_task, arg1, arg2, arg3, arg4, arg5, arg6, arg7)


/* The following macros are intended to encompass code used repeatedly to check for NULL pointers and then return from the calling function. */
/* This macro is to be used in cases where a specific value is to be returned (calling function returns non-void) */
#define RF_NULL_CHECK_RTN(p, txt, rtn)              \
{                                                   \
    if (p == NULL)                                  \
    {                                               \
        RF_MSG(MSG_LEGACY_ERROR, txt);              \
        return rtn;                                 \
    }                                               \
}

/* This macro is to be used where the calling function returns void */
#define RF_NULL_CHECK_RTN_V(p, txt)                 \
{                                                   \
    if (p == NULL)                                  \
    {                                               \
        RF_MSG(MSG_LEGACY_ERROR, txt);              \
        return;                                     \
    }                                               \
}

/* This macro is to be used in the cmd proccessing layer where rfm_dev is not defined */
#define RF_CMD_NULL_CHECK_RTN(p, txt, rtn)          \
{                                                   \
    if (p == NULL)                                  \
    {                                               \
        RFGC_T_MSG( MSG_LEGACY_ERROR, txt);         \
        return rtn;                                 \
    }                                               \
}

#define RF_INVALID_VALUE_32BIT  0xFFFFFFFF
#define RF_INVALID_VALUE_16BIT  0xFFFF
#define RF_INVALID_VALUE_8BIT   0xFF

#define RFGSM_RXLM_BUF_IDX_INVALID 0xFFFFFFFF

#define RFGSM_CORE_TEMP_COMP_8_BIN_SIZE     8
#define RFGSM_CORE_TEMP_COMP_16_BIN_SIZE        16

/* This is the value returned by rfgsm_compute_rx_pwr when RSSI is 0 .
** It is equal to the noise floor. 
*/
#define RFGSM_MIN_RX_POWER_REPORTED           (-120*16) /* dbm x 16    */

/* NOTE: Set the lower limit to -130dBm instead of infinity.  The RF
 * driver will return RF_IN_RANGE if the signal doesn't drop below
 * -130dBm.  
 */
#define BSP_RX_POWER_LOWER_LIMIT_dBm_x16      -2080  /* -130 dBm */
#define BSP_RX_POWER_UPPER_LIMIT_dBm_x16      -240   /*  -15 dBm       */

/* Gain range 1 may have 2 dB error, while gain range 2 could have 4 dB of
** error.  For validation purposes the high limit on gain range 1 may be
** 1_2_switch_point+2dB and the low limit for gain range 2 would be
** 1_2_switch_point-5dB.  So if the input power happens to be right in
** between this range, we will always declare this an error power and
** effectively filter out a perfectly valid channel for power scan.
*/
#define RF_GAIN_RANGE_SPEC_MAX_ERROR_dBx16    (4*16) /* 4dB max error */



#define RFGSM_MAX_TX_GAIN_RANGE                        8 

#define RFGSM_MDSP_SBI_WRITE_TIME_10XQS 25 /* 1 SBI write = 2.5qs */

#define SET_AT_RUNTIME 0

typedef struct
{
  uint16  kv_code;
  uint8   kv_code_r1;
} rfgsmlib_kv_code_type;

/* Gain ranges supported by the RTR*/
typedef enum
{
  RFGSMLIB_GAIN_RANGE_1=1,
  RFGSMLIB_GAIN_RANGE_2,
  RFGSMLIB_GAIN_RANGE_3,
  RFGSMLIB_GAIN_RANGE_4,
  RFGSMLIB_GAIN_RANGE_5
} rfgsmlib_rx_gain_range_type;

typedef enum
{
  RFGSM_PA_STATE_0=0,
  RFGSM_PA_STATE_1,
  RFGSM_PA_STATE_2,
  RFGSM_PA_STATE_3,
  RFGSM_PA_STATE_4,
  RFGSM_PA_STATE_5,
  RFGSM_PA_STATE_6,
  RFGSM_PA_STATE_7,
  RFGSM_PA_STATE_MAX
}rfgsm_pa_gain_type;

typedef enum
{
  RF_NULL_BUF=0,
  RF_RX_BUF=1,
  RF_TX_BUF=2,
  RF_MON_BUF=3
} rf_static_buffer_type;

typedef enum
{
  RF_MOD_GMSK,
  RF_MOD_8PSK,
  RF_MOD_UNKNOWN,
  RF_MAX_MOD_TYPE
} rfgsm_modulation_type;

typedef enum
{
   RFGSM_COEX_VBATT_BACKOFF,  
   RFGSM_COEX_DESENSE_BACKOFF,  
   RFGSM_MAX_BACKOFF
}rfgsm_coex_backoff_type;

typedef struct
{
  uint16 power_level;
  rfgsm_modulation_type modulation;
  int16 backoff[RFGSM_MAX_BACKOFF];
} rfgsm_power_profile_type;

typedef struct
{
  int16 power_level;
  rfgsm_modulation_type modulation;
} rfgsm_power_mod_type;

typedef struct
{
  rf_burst_type     burst;
  void              **scripts;
  uint16            burst_alpha;
  uint16            pll_settling_time_qs;
  uint16            num_ssbis_after_pll;
  uint16            num_ssbis_before_pll;
  boolean           uses_pll_flag;
  uint8             last_sbi_bus;
  uint16            num_bus_changes; 
  boolean           override_alpha;
  boolean           new_time_ref;
  uint16            burst_timing_offset_adjust_qs;
  uint32            autoA_execution_time_qs;
  uint32            autoB_execution_time_qs;
} rfgsmlib_sbi_burst_struct;

/* Structure that contains info only for Ip2 cal device driver algorithm*/
typedef struct
{
  int16 start_i_code;
  int16 start_q_code;
  boolean override_params;
  uint8 num_cal_points;  // Total number of IP2 cal nodes
  uint8 unit_step_size; // distance between adjacent cal points
  uint8 width;  // Number of cal points around the start I Q code
  int32 min_im2_val;
  uint8 num_iter;
}rfgsm_ip2_cal_alg_params_struct;

typedef struct
{
  rfgsm_ip2_cal_alg_params_struct alg_params;
  uint16 arfcn; //[RFCOM_NUM_GSM_BANDS];
  rfcom_gsm_band_type rfcom_band;
  uint8 tx_rgi;
  uint8 lna_range;
  boolean ip2_cal_supported;
  uint16 dc_settle_time_qs;
  uint16 step_duration_qs;
  
  /*pass/fail status of ip2 cal*/
  boolean status;
}rfgsm_ip2_cal_params_type;

/* Contains the i,q values of the IP2 cal grid corners*/
typedef struct
{
  /* Array containing cal step indices of overlapping points 
     in previous iteration grid*/
  uint8 prev_iter_step_idx[RFGSM_DEVICE_MAX_IP2_CAL_STEPS];

  /* Array containing cal step indices of overlapping points
     in current iteration grid */
  uint8 curr_iter_step_idx[RFGSM_DEVICE_MAX_IP2_CAL_STEPS];

  /* number of cal codes overlapping between previous
     and current iteration*/
  uint8 num_overlap_points;

  /* Flag to enable averaging between overlapping
   * points of current and previous iteration
   */
  boolean do_average;
}rfgsm_ip2_cal_grid_info_type;



typedef struct
{
  int32 tx_1slot_max_tune_time;
  uint16 rf_mdsp_tx_beta_qs;
}rf_tx_timings_struct; 


typedef struct
{
  /* RF device specific variables. */
  rf_tx_timings_struct rtr_timings;
  
}rfgsmlib_rf_device_type;

typedef struct
{
  boolean log_flag;
  rfcom_gsm_band_type rx_band;
  uint32 rx_chan;
  uint32 lna_gain;
} rfgsm_core_rx_log_type;

typedef PACK(struct)
{
  uint16 power_index;
  int32 pwr_in_dbm;
  int32 tx_pwr_offset;
  boolean pre_dist_flag;
  uint16 pa_scale;
  uint16 env_gain;
  uint8 pa_range;
  uint16 rgi;
  rfgsm_modulation_type mod_type;
  int16 coex_backoff;
  uint32 icq_bias;
  uint16 smps_pdm;
  apt_mode_type apt_mode;
} rfgsm_core_tx_slot_log_type;

typedef PACK(struct)
{
  uint8 ver_num; 
  uint32 fn;
  rfcom_gsm_band_type tx_band;
  uint16 tx_chan;
  boolean log_flag;
  int16 delay_val;  
  int32 freq_error; 
  uint8 num_slots;
  int32 sar_state;  
  uint16 vbatt_mv;
  uint16 therm_read;
  uint8 temp_comp_index; 
  int32 temp_comp_backoff;
  int16 temp_remainder;
  rfgsm_core_tx_slot_log_type tx_slot_log[RFGSM_MAX_TX_SLOTS_PER_FRAME];
} rfgsm_core_tx_log_type;

typedef struct
{
  boolean vbatt_comp_started_flag;
  uint16 vbatt_value_mv;
  int16 vbatt_comp_dBx100;
  boolean vbatt_adc_client_init_status;
  boolean enable_logging;
} rfgsm_core_vbatt_comp_info_type;

typedef struct
{
   /*! @ec - Placeholder member */
  boolean               slot_override_on;
 
  uint16                linear_rgi;

  uint16                tx_pow_dbm;

  uint16                pa_range;
 
  rfgsm_modulation_type modulation;

  uint16                env_gain;

  uint16                pa_scaling;

  uint16                smps_bias;


} rfgsm_core_ftm_slot_override_type;


/************************* RFGSM CARD STRUCTURE *******************************/
typedef struct
{
  /**** General status information of RF card ******/
  /* when in compressed mode = TRUE ; in standalone GSM = FALSE*/
  boolean rfgsm_cm;
  /* Current band */
  rfcom_gsm_band_type band;

  /* current tx band, this get updated in rfgsm_core_tune_tx()  */
  rfcom_gsm_band_type tx_band;

  /* GSM rf band before we go to sleep. This needs to be remembered so that 
  ** RF driver can wakeup in same band.
  */
  rfcom_gsm_band_type band_before_sleep;

  /* Transmit channel number */
  uint16 rf_tx_arfcn;
  /* Receive channel number */
  uint16 rf_rx_arfcn;
  /* Current PA range. Only used for Linear PA targets. */
  uint16 parange_override_val;
  /* Current RGI. Only used for Linear PA targets. */
  uint16 rgi_override_val;
  /* Current LNA range */
  uint8 lna_range;

  rfgsm_core_ftm_slot_override_type ftm_slot_override[RFGSM_MAX_TX_SLOTS_PER_FRAME];
  uint16 ftm_path_delay_override_val;

  /* PA start/stop times */
  int16 rf_pa_start_time;
  int16 rf_pa_stop_time;

  boolean tx_timing_adjust_flag;

  boolean hmsc_active;

  rfgsm_core_tx_log_type tx_log;
  rfgsm_core_rx_log_type rx_log;

  /* The device structures for RF programming. */
  rfgsmlib_rf_device_type rf_device;

  rfgsm_core_vbatt_comp_info_type vbatt_comp_info;


  rfgsm_mdsp_scripts_ptrs scripts;

  /* Compensated power in dbx16*/
  int16 valx16;

  uint8 rxlm_buf_index;
} rfgsm_card_type;




#define RFGSM_MAX_HANDLES 255

typedef struct
{ 
  boolean temp_comp_updated;     /* TRUE=if temp comp calculations performed  */
  uint8   temp_bin_8;           /* temperature bin based on an 8 bin structure  */
  uint8   temp_bin_16;          /* temperature bin based on an 16 bin structure */
  uint8   prev_temp_bin_8;      /* previous temp bin based on 8 bin structure */
  uint8   prev_temp_bin_16;     /* previous temp bin based on 16 bin structure */
  uint8   current_temp_bin_16;  /* RF task copy of current temperature bin value which gets updated via MSG router*/
} rfgsm_core_temp_comp_type;

typedef struct
{
  int32  mon_rf_setup_time_qs;  /* time required to setup RF for monitor burst */ 
  int32  rx_rf_setup_time_qs;  /* time required to setup RF for Rx burst */ 
  int32  rx_idle_rf_setup_time_qs;  /* time required to setup RF for Rx burst. This will only be used in DR-DSDS when GSM is in Recieve Idle mode only */ 
  int32  tx_rf_setup_time_qs;  /* time required to setup RF for Tx burst */ 
  uint16 rf_warmup_time_qs;    /* time RF requires to be ready after sleep */
  int32  ip2_cal_setup_time_qs;
} rfgsm_timing_info_type;


/*==============================================================================

                EXTERNALIZED DATA

==============================================================================*/
extern rfgsm_card_type rfgsm_card;


#ifdef __cplusplus
}
#endif


#endif