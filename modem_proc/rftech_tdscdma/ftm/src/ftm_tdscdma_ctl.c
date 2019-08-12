/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            F T M  T D S C D M A  C T L

GENERAL DESCRIPTION
  This is the FTM RF file which contains RF specific commands

Copyright (c) 2009 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2013 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/src/ftm_tdscdma_ctl.c#1 $
  $DateTime: 2016/12/13 07:59:50 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/15   jz      Fix set secondary chain logic
03/04/15   jz      Enable XO cal on TDS node
02/25/15   jr      Fix compiler warning messages
02/19/15   jak     Offtarget fix
01/31/15   whz     Explicitly invalid cache for IQ capture
01/14/15   jz      Fix IQ capture functionality issue
01/06/15   jak     update ftm_tdscdma_div_status in ftm_tdscdma_set_secondary_chain()
12/22/14   whz     Cleanup code
12/17/14   ms      lin state override support
12/02/14   jyu     Shut off RX when switch to TX cal
11/25/14   jmf     [selftest] VSWRv2 measurement support for TDSCDMA
11/24/14   jyu     Skip sending TX_CONFIG MSG to FW when on Dev2/3
11/20/14   whz     Set Rx mode before reading RxAGC
11/17/14   jps     Vote for max clk speed for TD-SCDMA FTM mode
11/14/14   jhe     Disable fbrx in xpt_cal_deconfig
11/14/14   jps     [sefltest] Update to search center & width for EVM capture
11/03/14   jps     Updates for get PA switchpoints API
11/03/14   jyu     Rx calibration time reduction
10/17/14   jps     [sefltest] Add suppport to specifiy search center for EVM capture
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
10/13/14   al      Added ftm_tdscdma_set_devices to support dynamic device settings
10/01/14   jyu     Added support to allow override alt_path 
09/26/14   jyu     Added support to calibrate different alt_path
09/25/14   jyu     Fixed bug for DR-DSDS
09/25/14   kb      [JOLOKIA] Support new FW IQ format for FTM IQ capture
09/21/14   jyu     Added RFcal support for DR-DSDS
09/20/14   ndb     Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
09/18/14   ych     Suppress kw warnings
09/17/14   kb      Clean up FEATURE_DIMEPM_MODEM from mainline
09/10/14   ych     Fix Tuner callflow issues
09/03/14   ych     Coupler configuration during XPT cal
08/28/14   xsh     Remove "Local" for ftm_tdscdma_config_fw_state API definition
08/18/14   jmf     [SelfTest] ACLR Meas Changes
08/18/14   kb      RUMI hacks for FTM mode operation.
08/09/14   ych     Provided accomodation to EPT Char call flows in FTM
08/07/14   jyu     Need to disable Tx burst before re-tuning (for FTM RF mode)
07/26/14   lcl     Fix OFT compilation error
07/18/14   aa      Fix FBRx DC Cal failures due to incorrect condition buffer ID check
07/17/14   ms      Sawless RX support
07/08/14   jps     FTM RF Mode Rx1 IQ capture fix
06/25/14   jyu     Work-around intermittent RxAGC failure
06/04/14   jyu     Added support to separate non-trigger and trigger writes
06/03/14   jz      Add tuner tune code override command
05/15/14   whz     Fix ftm_tdscdma_config_fw_state for DPM
05/05/14   jyu     Added support for DSDS sleep/wakeup
04/28/14   jps     Support for TDS port switching
04/21/14   jyu     Added support to use FED set LNA gain state
04/20/14   jyu     Added support to use FED execuate RxOFF and TxOFF scripts
03/29/14   kb      [Merge][XPT] Add porper shutdown of WTR during XPT deconfig.
03/21/14   zg      OFT fix.
03/21/14   zg      Removed rfc_common_get_cmn_device_object() from TDSCDMA client.
03/21/14   ms      Remove rf_tdscdma_msm_stmr_init() from ftm enter mode
03/17/14   ych     Increased TX waveform switching delay
03/13/14   jps     Update FW state transition ordering
02/07/14   zg      Added LNA latch support.
01/31/14   jmf     Added support for FBRx Cal IQ Captures
01/28/13   ych     Interface adjustments for DPD sample capture
12/23/13   ych     Set two valid RXLM numbers for DRx mode to FW
12/18/13   jps     Add new TFW state
12/15/13   jps     Enable RFLM based TXAGC override
12/13/13   nsh     Fix compiler warnings
12/11/13   ych     Featurized TxAGC Override
12/10/13   jyu     Update set_lna_range()
12/04/13   kb      [XPT] Add support for pin capture.
11/31/13   ych     Added an API for fast calibration for TxAGC override 
11/20/13   jps     Do not enable_tx in FTM RF mode during tune_to_chan()
11/06/13   cd      Populate RFLM handle for RFLM overrides
10/26/13   jyu     Changed the FW message call flow per FW design
                   TX_CONFIG (with TX MODEM CHAIN info) 
                   should be sent only once when set to FTM mode
10/24/13   zg      Fixed wrong featurization from previous checkin
10/23/13   qzh     Fix KW warnings
10/22/13   jps/cd  Added functionality for baseband IREF override in XPT Tx ovr
10/18/13   ych     Added Tx FTM call interface
10/12/13   ych     Added an API to enable data conversion in ACQ mode
10/08/13   zg      Added TxAGC override support.
10/04/13   jps     Fixes for IQ capture in FTM calibration mode
09/10/13   aka     Added FTM APIs for self-test cmds in TDS mode 
09/06/13   ars     Added FTM interface for FTM_GET_MULTIPLE_RX_LEVEL_DBM command
08/29/13   jyu     Changed the set_rx_gain_state interface
08/19/13   kb      [XPT] Update delay_cap_type and samp_rate in IQ capture parameters.
08/08/13   zhzheng  Re-intialize the FTM_DB_Status when entering TDSCDMA, in case it's cleared
                   when exiting previous mode
08/07/13   kb      Update Current Tx waveform after Tx_ON
07/24/13   gvn     Change name of stg parameter for RSB cal
07/23/13   ych     Opcodes calv4 support
07/22/13   kb      [XPT] 1. Updated XPT IQ capture API to return sampling rate.
                         2. Added using modem_chain to deconfig WTR after sample capture
                         3. Updateed device config API to accept RFM_FB_ATTENUATION_STATE_LOW during deconfig.
07/15/13   cri     Immediate MSM register programming for tx override
07/08/13   ms      Support for program_qfe_scenario
07/06/13   qzh     Replace memcpy() with safer memscpy().
07/03/12   zg      Cleanup.
07/03/13   zg      Added initial Self-cal support.
07/03/13   aliang  Add support of switching from a B40b channel to a B40a channel in ftm mode
06/26/13   jyu     Added support to remove the requirement to specify band info
                   when it is not ready
06/19/13   kb      [XPT] Added support for Tx waveform switching
06/18/13   ms      Change default band to B34
06/18/13   whz     Shorten the extra delay for LNA gain3 RF Cal
06/06/13   jps     Clean up FTM RF data structures on exit mode
06/05/13   kb      [XPT] Support to update rfm_mode for TDS in FTM mode
06/03/13   nsh     Support Get Multi Synth State in Dime
05/29/13   kb      1. Changed mcpm clk voting in xpt_cal_config and deconfig
                   2. Added turn off WTR ET path in sample capture deconfig
05/29/13   whz     Added extra delay for LNA gain3 RF Cal
05/25/13   qzh     Porting TDSCDMA IQ capture from NIKEL to DIME
05/20/13   al      Added support of split band
05/13/13   kb      Fix compiler warnings.
05/13/13   kb      [XPT] Added xpt_iq_capture() API
05/06/13   kb      [XPT] Added xpt_cal_config() API
05/01/13   zg      Updates on RSB cal and temp comp.
04/25/13   qzh     Porting FTM RF code from NIKEL to DIME
04/24/13   jyu     Added support to allow pass multiple script buffers for script
                   building
04/22/13   zg      Added therm read in RSB calibration.
04/17/13   zg      RSB calibration updates.
04/05/13   jyu     Re-organize the PA interface to combine GRFC type PA and RFFE
                   type PA into one common PA interface
04/02/13   sn      Cleanup PA interface
04/03/13   zg      Added support for RSB calibration.
04/02/13   sn      Cleanup ASM interface
03/21/13   ms      Tuner TDET support
03/07/13   zg      Added support for IQ capture via TFW.
03/07/13   sr      changed the "pwr_tracker" name to "papm".
03/04/13   jyu     Fixed a bug for diversity RFcal
02/08/13   jyu     Removed Hard-coded device info
02/08/13   jyu     Set RX_ON (SSBI) to enable RF cal for WTR1625/2605
01/31/13   ms      Made variables global for ftm_tdscdma_meas support
01/15/13   ms      Removed hack to enable diversity in set mode
01/15/13   ms      Removed diversity disable in cal state config
12/30/12   jyu     Added support for run-time select RX_ON control by SSBI or GRFC
12/21/12   jyu     Integrated with rfdevice TDSCDMA papm APIs
12/21/12   jyu     Integrated with rfdevice TDSCDMA ASM APIs
12/21/12   jyu     Integrated with rfdevice TDSCDMA PA APIs
12/17/12   ms      KW fix
12/04/12   aka     Removed GSM set mode for device 2 from toggle_mode API
11/04/12   ms      APT support
11/27/12   tws     Add rfm device to FTM GSM.
09/10/12   ms      Diversity support
09/07/12   kb      Set correct MCPM sequence: Start -> Idle -> Voice
08/20/12   jyu     Added support to set/clean specific GRFC (immediate execution)
08/20/12   jyu     Added support to cleanup GRFCs
08/13/12   jyu     Updated interface to take rfm_device_enum_type and for both
                   primary and secondary device
08/08/12   jyu     Integrate with ASM and PA interface
08/08/12   jyu     Added rfm_device_enum to FTM code
08/02/12   nsh     interface change from "PATH" to "DEVICE"
07/31/12   qzh     Rfdevice interface change to use rfm_device instead of path
07/18/12   jps     Add FTM error handling for RF mode commands
07/18/12   jps     Compiler warning fix
07/17/12   hy      Remove obsolete APT codes
07/16/12   jps     Initial FTM burst TX/RX framework
07/16/12   gary    Fix KW warnings.
07/11/12   hy      Enable TD-SCDMA APT RFCal.
04/12/12   hy      Use Idle mode MCPM to get higher clock rate during calibration.
04/10/12   hy      Remove reference to rf_tdscdma_msm_set_rx_on
04/07/12   ycl     Added DAC0_UPDATE GRFC.
03/26/12   hy      TD-SCDMA Cal time optimization
04/24/12   jfc     Remove tramp.h include
03/06/12   jyu     Updated rfdevice cmd ID
03/02/12   ycl     Update MCPM transitions to match MCPM state transition.
02/15/12   hy      remove log_ftm_gen_type
02/14/12   jhe     Temp fix for the first time tx issue
11/18/11   hy      Ensure all cached state variables are invalidated at mode exits,
                          to fix a failure of Cal V3 if after XO Cal
11/16/11   hy      Added TDSCDMA V2/V3 Cal, DAC Cal prototype
09-23-11   jhe     Match the updated MC Api
09-07-11   zg      Replaced HAL hack functions with RFC APIs for Rx/Tx band config
08-30-11   hy      persist PA range after turn Tx off/on.
                          changed lna state settling time to 200us (from 500us)
08-25-11   hy      Fixed lna range 0 offset was always zero
                          check lna range index validity.
                          Moved mc_tx_enable from tune to chan to set tx on.
                          Multiple rxagc read to filter out unstable data
                          Changed TFW default state from inactive to standby.
                          Send Rxlm config msg in rx cal msg.
08-24-11   hy      Rx Cal support.
05-19-11   hy      Created.
===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"
#include <stdio.h>
#include <stringl/stringl.h>

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftmicap.h"
#include "rfcommon_fbrx_api.h"
#ifdef FTM_HAS_TDSCDMA

#include "time_svc.h"   /* Replaces clk.h*/
#include "time_tod.h"   /* Replaces clk.h*/
#include "DALSys.h"
#include "mcpm_api.h" /*used by ftm_tdscdma_mode_enter*/
#include "rfcom.h"
#include "ftm.h"
#include "ftm_v.h"
#include "rf_test_task_v.h"
#include "rfnv.h"
#include "ftm_log.h"
#include "rfdevice_tdscdma_intf.h"
#include "rfdevice_hdet_tdscdma_intf.h"
#include "rfdevice_type_defs.h"
#include "rfdevice_tdscdma_type_defs.h"

#include "bsp.h"
#include "task.h"
#include "fws.h"//for firmware load service

#include "rf_tdscdma.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_mc.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_msm.h"
#include "rfc_tdscdma_data.h"

#include "ftm_tdscdma_ctl.h"
#include "ftm_tdscdma_dispatch.h"
#include "ftm_tdscdma_selftest.h"
#include "ftm_calibration_data_module.h"

#include "rfdevice_tdscdma_pa_intf.h"
#include "rfdevice_tdscdma_asm_intf.h"
#include "rfdevice_tdscdma_papm_intf.h"
#include "rfdevice_tdscdma_coupler_intf.h"
#include "rfcommon_nv.h"

#ifdef FTM_HAS_TDSCDMA_BER
#include "ftm_tdscdma_ber.h"
#endif /* FTM_HAS_TDSCDMA_BER */

//#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
//#include "tdsrxdiv.h"
//#endif /* FTM_HAS_TDSCDMA_RX_DIVERSITY */

#include "ftmdiag.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "lm_types.h"
#include "ftm_common_control.h"
#include "ftm_common_msm.h"
#include "ftm_common_msm.h"
#include "rfm.h"
#include "ftm_common_xpt.h"
#include "rfcommon_mc.h"
#include "rfcommon_core_utils.h"
#include "ftm_msg.h"
#include "ftm_common_iq_processing.h"
#include "ftm_common_dispatch.h" /* trigger IQ capture */
//#include "rfcommon_mdsp.h"   //Needed to use rfcommon_iq_efs_save
#include "ftm_common_selftest.h"
#include "rflm_tds_mc.h"
#include "qurt_memory.h"

#ifdef FTM_HAS_GSM
#include "ftm_gsm_ctl.h"
#include "gl1_hw.h"
#endif

#include "rfc_card_tdscdma.h"

#include "DALSys.h"
#include "tcxomgr.h"

#ifdef FEATURE_RF_HAS_QTUNER
#include "rf_tdscdma_core_antenna_tuner.h"
#endif

#include "rfcommon_data.h"

#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
#define FTM_HAS_TDSCDMA_RX_DIVERSITY
#endif

#ifndef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE
#include "rflm_tds_mc.h"
#endif


/*AGC read averages. TDS is using CW, thus a small value works well*/
#define FTM_TDSCDMA_NUM_OF_AVG_RX_AGC_READ    2

/* Lower limit for tx sweep interval in units of sleep clock periods */
#define FTM_TDSCDMA_TX_SWEEP_INTERVAL_LOWER_LIMIT 0  /*  no limit for now*/

#define FTM_TDSCDMA_NUMBER_OF_INTERBAND_GAIN_STATE_USED   2

#define FTM_TDSCDMA_SWEEP_INTERM_LEVEL 90

#define FTM_TDSCDMA_DEFAULT_SMPS_PDM_VAL 786 /*Equivalent of 3.4v at PA*/

#define FTM_TDSCDMA_FREQ_CAL_SWEEP_NUM_HDET_AVG 1

#define FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS 0
#define FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE 1
#define FTM_TDSCDMA_RF_MODE_TX_CMD_TIMEOUT 20 /* Time out for FTM_TX command response from FW (in ms)*/
#define FTM_TDSCDMA_RF_MODE_RX_CMD_TIMEOUT 20 /* Time out for FTM_RX command response from FW (in ms)*/

#define FTM_TDSCDMA_RF_MODE_GET_MULTI_SYNTH_TIMEOUT 20
#define FTM_TDSCDMA_RF_MODE_IQ_CAPTURE_TIMEOUT 80


#define FTM_TDSCDMA_IQ_CAPTURE_GET_SAMPLE 8192


/* *************************************************************/
/* ***************FTM IQ capture macros*****************/
/* *************************************************************/
/*! Maximum TDSCDMA IQ Samples that can be captured  */
#define FTM_TDSCDMA_IQ_MAX_SAMPLES 8192

/*! TDSCDMA Sample server IQ format */
#define FTM_TDSCDMA_IQ_SS_DATA_FORMAT FTM_IQ_16_BIT_FORMAT

/*! CHIPX2 TDSCDMA frequency */
#define FTM_TDSCDMA_IQ_SAMPLE_FREQ_CHIPX2 (1.28e6*2)

/*! This is the maximum number of TDSCDMA IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
/*! Through experimentation it is found to be 512 bytes: 128 samples  I (2 byte) + Q(2 byte)  */
#define FTM_TDSCDMA_IQ_DIAG_MAX_SAMPLES (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/4)

/*! These two constants are used by XPT capture configuration for the purpose of self-test */
/*! Whenever capture type of EVM or VSWR is requested these values take effect instead of  */
/*! coming from DPD CONFIG NV */
#define FTM_TDSCDMA_SELFTEST_SEARCH_CENTER 47
#define FTM_TDSCDMA_SELFTEST_SEARCH_WIDTH 15

int2 ftm_tdscdma_tx_cal_sweep_hdet_val[FTM_UPLINK_SWEEP_CAL_SIZE];

hdet_tracking_params_type *hdet_tdscdma_tracking_params_ptr = NULL;

/* Period to call HDET tracking callback function */
int4 ftm_tdscdma_hdet_cb_interval = 1;

//LOCAL rf_tdscdma_mdsp_tfw_state_t ftm_tdscdma_current_fw_state = RF_TDSCDMA_MDSP_TFW_STATE_STANDBY;
//LOCAL ftm_tdscdma_cal_state_type ftm_tdscdma_current_cal_state = FTM_TDSCDMA_CAL_STATE_INACTIVE;
rf_tdscdma_mdsp_tfw_state_t ftm_tdscdma_current_fw_state = RF_TDSCDMA_MDSP_TFW_STATE_STANDBY;
ftm_tdscdma_cal_state_type ftm_tdscdma_current_cal_state = FTM_TDSCDMA_CAL_STATE_INACTIVE;

ftm_tdscdma_tx_cal_type ftm_tdscdma_current_tx_cal_mode = FTM_TDSCDMA_TX_CAL_LEGACY;
boolean ftm_tdscdma_self_cal_drx_enabled = FALSE;

/* Clock Callback structure */
LOCAL timer_type  *ftm_tdscdma_hdet_cb_struct = NULL;


LOCAL int2 ftm_tdscdma_num_of_hdet_reads = 0;
LOCAL int4 ftm_tdscdma_local_hdet_val = 0;

/* Tx AGC Adjust PDM for Tx Sweep Cal */
LOCAL int2 ftm_tdscdma_tx_agc_adj_pdm = 90;

/* Maximum Tx AGC Adjust PDM for Tx Sweep Cal */
LOCAL int2 ftm_tdscdma_tx_agc_adj_pdm_max = 90;

/* HDET read index */
LOCAL int2 ftm_tdscdma_hdet_index = 0;

/* Internal counter for uplink Cal sweep to skip odd number of each 10ms interrupt */
LOCAL int2 ftm_tdscdma_uplink_sweep_int_cnt = 0;

/* Current Tx AGC Adj PDM value */
LOCAL int2 ftm_tdscdma_cur_tx_agc_adj_pdm = 90;

/* Current PA Range value */
LOCAL uint8 ftm_tdscdma_cur_pa_range = 0;

/* The number of Tx sweep */
LOCAL int2 ftm_tdscdma_hdet_outloop_index = 0;

LOCAL boolean ftm_tdscdma_uplink_sweep_cal_enable = FALSE;

/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;

boolean ftm_tdscdma_div_status = FALSE;

int16 ftm_tdscdma_power_dbm;

/* default Tx status*/
boolean ftm_tdscdma_tx_on_status = FALSE;

/* Length of each step of the tx cal sweep, units are sleep clock periods */
uint16 ftm_tdscdma_tx_cal_sweep_interval = 20; /* 20 ms */

/* Number of HDET readings to take during each step of the tx cal sweep */
byte ftm_tdscdma_num_of_avg_hdet_read = 5;

static uint8  ftm_tdscdma_uplink_sweep_cal_step = 1;

static uint16 ftm_tdscdma_tx_rx_freq_cal_cur_seg = 0;

static uint16 ftm_tdscdma_tx_rx_freq_cal_channels[FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tdscdma_tx_rx_freq_cal_pa_ranges[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint16 ftm_tdscdma_tx_rx_freq_cal_pdm_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tdscdma_tx_rx_freq_cal_hdet_read_list[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static uint8  ftm_tdscdma_tx_rx_freq_cal_rx_actions[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tdscdma_tx_rx_freq_cal_exp_agc[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ];

static int16  ftm_tdscdma_tx_rx_freq_cal_rx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];
static uint16 ftm_tdscdma_tx_rx_freq_cal_tx_result[FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ*FTM_TX_RX_FREQ_CAL_MAX_FREQS];

static uint8  ftm_tdscdma_tx_rx_steps_per_freq=0;
static uint8  ftm_tdscdma_tx_rx_num_freqs=0;
static uint16 ftm_tdscdma_tx_rx_chain=0;

uint8 ftm_tdscdma_rx_lin_state=0;

extern void rf_tdscdma_mc_set_blocking_cmd_rsp_status(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg, rf_tdscdma_rfmode_cmd_rsp_status_type status);
extern rf_tdscdma_rfmode_cmd_rsp_status_type  rf_tdscdma_mc_get_blocking_rsp_status(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg);

rftx_pdm_limit_type ftm_tdscdma_pdm_limits = {0,127};

uint8 ftm_tdscdma_eul_mpr_idx = 0;

LOCAL uint16 ftm_tdscdma_lna_state_settling_delay_us = 100;

rfcom_tdscdma_band_type ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_INVALID;
uint16 ftm_tdscdma_current_chan = 0;

rfm_device_enum_type ftm_tdscdma_current_pri_device   = RFM_INVALID_DEVICE;
rfm_device_enum_type ftm_tdscdma_current_sec_device   = RFM_INVALID_DEVICE;
rfm_device_enum_type ftm_tdscdma_current_tx_device    = RFM_INVALID_DEVICE;
uint8                ftm_tdscdma_current_pri_alt_path = 0;
uint8                ftm_tdscdma_current_sec_alt_path = 0;
boolean              ftm_tdscdma_override_alt_path    = FALSE;

ftm_mode_id_type ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_MAX;

ftm_multi_synth_lock_status_enum_type tx_pll_multi_state = FTM_SYNTH_STATUS_INVALID;
ftm_multi_synth_lock_status_enum_type rx_pll_multi_state = FTM_SYNTH_STATUS_INVALID;

rfcom_tdscdma_band_type ftm_curr_tdscdma_mode;
pout_tbl_type ftm_tdscdma_pout_tbl =
{
  1,
  {0},
  {0},
  POUT_DATA_TYPE_MAX
};

boolean ftm_tdscdma_smps_pa_bias_override = FALSE;

uint16 ftm_tdscdma_smps_pa_bias_override_val = 390;

ftm_lm_buf_type ftm_tdscdma_rxlm_buffer[2]= {{0, FALSE},{0,FALSE},};
ftm_lm_buf_type ftm_tdscdma_txlm_buffer = {0,FALSE};

boolean ftm_tdscdma_rx_div_enable = FALSE;

static ftm_tdscdma_burst_tx_state_type ftm_tdscdma_burst_tx_state = FTM_TDSCDMA_BURST_TX_OFF;
static ftm_tdscdma_burst_rx_state_type ftm_tdscdma_burst_rx_state = FTM_TDSCDMA_BURST_RX_OFF;

/* Shadow structure to track FTM TX parameters */
static rf_tdscdma_ftm_burst_tx_params_type ftm_tdscdma_burst_tx_params_shadow =
{
  1, /* Number of slots */
  0, /* Modulation Type: 0 = QPSK default*/
  {-450,0,0,0,0}, /* Power Levels: Only TS1 is used, rest are currently ignored */
  0, /* Freq Offset in Hz */
  8, /* Spreading Factor: Default to 8 */
  150, /* PDET threshold, dBmx10 units */
  2, /* PDET Trigger Interval in no. of sub-frames */
  1, /* OVSF Code */
  TRUE /* Flag to block HDET/THERM TX reconfig messages to FW */
};

/* Shadow structure to track FTM RX parameters */
static rf_tdscdma_ftm_burst_rx_params_type ftm_tdscdma_burst_rx_params_shadow =
{
  1, /* Number of slots */
  FALSE, /* rx1 chain inactive by default */
  FALSE, /* Enable/Disable LNA state machine: FALSE = Disabled by default */
  -1600, /* Expected RX Level */
  0, /* Computed Rx0 LNA state */
  0 /* Computed Rx1 LNA state */
};

static ftm_tdscdma_tx_waveform_type ftm_tdscdma_curr_tx_waveform = FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST;  /* init value of special burst */
static uint16 ftm_tdscdma_tx_wf_switching_delay_us = 3000;

static uint16 rf_tdscdma_ftm_wrk_chan = 0xFFFF;

/*static address for raw data IQ capture*/
static uint32 *raw_data_start_address = NULL;
static uint32 *raw_data_start_address_div = NULL;


// This varaible is used to maintain information of acquired IQ samples.
static ftm_tdscdma_iq_acquired_samples_data_type ftm_tdscdma_iq_acquired_samples_data = {RFM_DEVICE_0, RFM_DEVICE_0, 0, NULL, FALSE, FALSE};
// Variable that is used as Response packet for GET SAMPLES Command
static ftm_iq_capture_get_samples_rsp_pkt_type ftm_tdscdma_iq_capture_get_samples_rsp_pkt;
// Variable that is used as Response packet for FTM_GET_MULTIPLE_RX_LEVEL_DBM
static ftm_get_multiple_rx_level_dbm_rsp_pkt_type ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt;

// This varaible is used to maintain information of acquired IQ samples.
static ftm_rf_tdscdma_fbrx_iq_acquired_samples_data_type ftm_rf_tdscdma_fbrx_iq_acquired_samples_data;
// Variable used as Response packet for FTM_RF_MEAS_TX_PARAMS command
static ftm_rf_fbrx_meas_tx_params_rsp_pkt_type ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt;
// Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_TRIGGER command
static ftm_rf_fbrx_iq_capture_trigger_rsp_pkt_type ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt;
// Variable used as Response packet for FTM_FBRX_IQ_CAPTURE_GET_SAMPLES command
static ftm_rf_fbrx_iq_capture_get_samples_rsp_pkt_type ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt;

/*----------------------------------------------------------------------------*/
boolean ftm_tdscdma_ctl_dbg_msg_flag = TRUE; /*!< Flag for enabling
TDSCDMA FTM debug messages. CalV3 may override this flag */

/* parameter for iq capture & RSB cal */


ftm_tdscdma_iq_capture_data_type ftm_tds_iq_capture_data =
{
  0,      //startAddrPrx
  0,      //startAddrDrx
  FALSE,  //OverrideStg
  FALSE,  //Print Sample data
  40000,  //IqCaptureOutputDelay
  0,      //IqCaptureStatus
  2000,    //MaxWaitCnt //todo: should be back to 20 after remving IQ printing
  16384   //SampleSize
};

extern boolean rfcommon_fbrx_mc_enable_rf_dev(rfdevice_rxtx_common_class *dev_ptr, uint8 gain_state);
extern boolean rfcommon_fbrx_mc_disable_rf_dev(rfdevice_rxtx_common_class *dev_ptr);

extern void rf_tdscdma_mdsp_tx_hw_block_config(boolean en_dis_flag);
extern uint16 rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(rf_path_enum_type rx_chain, uint8 index, uint8 linearity);

ftm_lm_buf_type ftm_tdscdma_xpt_fb_path_buffer = {0,FALSE};

ftm_mode_id_type ftm_tdscdma_util_convert_band_rfcom_to_ftm(rfcom_tdscdma_band_type rfcom_band)
{
  ftm_mode_id_type ftm_band = FTM_PHONE_MODE_MAX;
  switch ( rfcom_band )
  {
    case RFCOM_BAND_TDSCDMA_B34:
      ftm_band = FTM_PHONE_MODE_TDSCDMA_B34;
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      ftm_band = FTM_PHONE_MODE_TDSCDMA_B39;
      break;

    case RFCOM_BAND_TDSCDMA_B40:
      ftm_band = FTM_PHONE_MODE_TDSCDMA_B40;
      break;

	case RFCOM_BAND_TDSCDMA_B40_B:
      ftm_band = FTM_PHONE_MODE_TDSCDMA_B40_B;
      break;

    default:
      FTM_MSG_ERROR( "Invalid rfcom band=%d in ftm_tdscdma_util_convert_band_rfcom_to_ftm().", rfcom_band, 0, 0 );
      break;
  }

  return ftm_band;
}

rfcom_tdscdma_band_type ftm_tdscdma_util_convert_band_ftm_to_rfcom(ftm_mode_id_type ftm_band)
{
  rfcom_tdscdma_band_type rfcom_band = RFCOM_BAND_TDSCDMA_INVALID;
  switch ( ftm_band )
  {
    case FTM_PHONE_MODE_TDSCDMA_B34:
      rfcom_band = RFCOM_BAND_TDSCDMA_B34;
      break;

    case FTM_PHONE_MODE_TDSCDMA_B39:
      rfcom_band = RFCOM_BAND_TDSCDMA_B39;
      break;

    case FTM_PHONE_MODE_TDSCDMA_B40:
      rfcom_band = RFCOM_BAND_TDSCDMA_B40;
      break;

    default:
      FTM_MSG_ERROR( "Invalid ftm_band =%d in ftm_tdscdma_util_convert_band_ftm_to_rfcom().", ftm_band, 0, 0 );
      break;
  }

  return rfcom_band;
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_UPLINK_WAVEFORM

DESCRIPTION
   This function will program the encoder to generate a TDSCDMA uplink waveform.
   It is used only for RF testing purposes.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_tdscdma_set_uplink_waveform(void);

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_tdscdma_set_pa(boolean state);


/*===========================================================================

FUNCTION   FTM_UPLINK_SWEEP_CAL_ISR

DESCRIPTION
   FTM Tx Sweep Calibration service routine.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_uplink_sweep_cal_isr(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure TDS firmware state

  @details
  This function will enable FW App while running under FTM task context by
  calling RF-MDSP Interface.

  @param fw_state
  The state in which to configure firmware
*/
void
ftm_tdscdma_config_fw_state
(
  rf_tdscdma_mdsp_tfw_state_t fw_state
);

/*============================================================================*/
/*!
  @name TDSCDMA calibration state

  @brief
  This section contains functions to control TDSCDMA calibration state,
  because cal can't run Rx/Tx at the same time.
  this function will send calibration msg to FW and tune the corresponding RF resources.

  @param cal_state
  The calibration state in which to configure.
*/
LOCAL void
ftm_tdscdma_config_cal_state
(
  ftm_tdscdma_cal_state_type cal_state
);


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_PDM

DESCRIPTION
    This function takes a pdm value and writes it to the specified PDM.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Will automatically OPEN the PDM loop even if it was previously closed.
   This allows applications a more intuitive approach to using this function.
===========================================================================*/
void ftm_tdscdma_set_pdm(ftm_pdm_id_type pdm_id, int2 val)
{
  rfdevice_tdscdma_lut_idx_pa_range_type lut_pa_data;

  /* Find the PDM which we want to adjust and set it to the value. */
  switch (pdm_id)
  {
  case FTM_PDM_TX_AGC_ADJ:
    val = RF_SATURATE( val, (int2)ftm_tdscdma_pdm_limits.min_pdm, (int2)ftm_tdscdma_pdm_limits.max_pdm );
    lut_pa_data.lut_idx = (uint8)val;
    lut_pa_data.pa_range = ftm_tdscdma_cur_pa_range;
    lut_pa_data.band = ftm_tdscdma_current_mode;
    lut_pa_data.slot_index = 0;

    /* Set Tx AGC */
    rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                     RFCOM_TDSCDMA_MODE,
                                     ftm_tdscdma_current_mode,
                                     RFDEVICE_SET_LUT_INDEX,
                                     (uint8 *)&lut_pa_data);


    ftm_tdscdma_cur_tx_agc_adj_pdm = val;
    break;

  default:
    FTM_MSG_ERROR("PDM id is not supported %d", pdm_id, 0, 0);
    break;
  } /* switch */

} /* end ftm_tdscdma_set_pdm */

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SMPS_PA_BIAS_VAL

DESCRIPTION
    This function sets the SMPS PA Bias Value in FTM mode only.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_tdscdma_set_smps_pa_bias_val(uint16 val)
{

  boolean status;

  /*Return if it is not FTM mode*/
  if(!IS_FTM_IN_TEST_MODE())
  {
    FTM_MSG_ERROR("exiting from ftm_tdscdma_set_smps_pa_bias_val val=%d ",val,0,0);
    return;
  }

  /*Keep track of the overriden value for future use*/
  ftm_tdscdma_smps_pa_bias_override_val = val;
  FTM_MSG_HIGH("start ftm_tdscdma_set_smps_pa_bias_val val=%val", val,0,0);

  if (rfdevice_tdscdma_use_papm(ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode))
  {
    status = rfdevice_tdscdma_papm_set_mode_bias(ftm_tdscdma_current_tx_device,
                                                 ftm_tdscdma_current_mode,
                                                 RFDEVICE_APT_MODE,
                                                 val,
                                                 RFDEVICE_EXECUTE_IMMEDIATE,
                                                 NULL,
                                                 0);

    if (!status)
    {
      MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "Error when set mode bias for papm on Device %d, Band %d", ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode);
    }
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LVL_MED, "papm device is not present for Device %d and Band %d", ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode);
  }

} /*ftm_tdscdma_set_smps_pa_bias_val*/

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_SMPS_PA_BIAS_OVERRIDE

DESCRIPTION
    This function overrides the SMPS PA Bias tables and whenever PDM
    is set (in FTM mode only), the overriden value will take effect.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None
===========================================================================*/
void ftm_tdscdma_set_smps_pa_bias_override(boolean val)
{
  /*Return if it is not FTM mode*/
  if(!IS_FTM_IN_TEST_MODE())
  {
    return;
  }

  switch (val)
  {
    case 0:
      ftm_tdscdma_smps_pa_bias_override = FALSE;
      rf_tdscdma_msm_set_smps_pa_bias_override(FALSE);
      break;

    case 1:
      ftm_tdscdma_smps_pa_bias_override = TRUE;
      rf_tdscdma_msm_set_smps_pa_bias_override(TRUE);
      break;

    default:
      FTM_MSG_ERROR("SMPS PA Bias Override flag invalid",0,0,0);
	  break;
  }

} /*ftm_tdscdma_set_smps_pa_bias_override*/

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TXAGC
DESCRIPTION
    This function sets LUT index, paRange and IQ gain for a given TxAGC

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_tdscdma_set_txagc(int16 txagc, boolean pa_override, uint8 parange )
{
#if 0
  uint16 lut_idx;
  uint16 dig_gain;
  uint8  nv_pa_idx = 0;
  float gain_adj = 0;
  uint16 iqgain = 0;//RFWCDMA_MDSP_TX_DIG_IQ_GAIN; //hyang todo: need TDSCDMA version

  /* PA Range and PA NV index lookup */
  rf_tdscdma_core_util_pa_range_lookup(txagc, ftm_tdscdma_eul_mpr_idx,
                                           pa_override, &parange, &nv_pa_idx);

  /* Add TxAGC offset to compensate for beta scaling */
//hyang todo: ask jiali, why this func is gone  txagc = txagc + (int16)rf_tdscdma_core_util_get_txagc_offset(ftm_tdscdma_eul_mpr_idx);

  /* Tx linearizer lookup */
  rf_tdscdma_core_util_tx_linearizer_lookup(txagc, nv_pa_idx, &lut_idx, &dig_gain);

  /* Compute digital gain adjustment */
  gain_adj = (float)(iqgain*pow( 10,(float)((-1.0*dig_gain)/(RF_TXAGC_RESOLUTION*20))));
  iqgain = (uint16)gain_adj;

  /* Set PDM digital gain and PA Range */
  ftm_tdscdma_set_pdm_iqgain_parange(lut_idx,iqgain,parange);
  #endif

  //hyang todo:
  //rf_tdscdma_mdsp_update_override_val(RF_TDSCDMA_MDSP_CHAIN_0,
  //RF_TDSCDMA_MDSP_TXAGC_OVERRIDE, TRUE, txagc);//unlinke wcdma, tds has override in fw, and the calculation is done in fw.


}



/*===========================================================================

FUNCTION FTM_TDSCDMA_TXAGC_SWEEP

DESCRIPTION
    This function does a power sweep based on the configured txagc values.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
===========================================================================*/
void ftm_tdscdma_txagc_sweep(int16 start, int16 stop, int16 step, uint16 delay,
                                           boolean parange_override, uint8 parange)
{
  int16 txagc = 0;

  /* Trigger for the CallBox */
  ftm_tdscdma_set_txagc(100,0,0);
  DALSYS_BusyWait(delay);
  ftm_tdscdma_set_txagc(930,0,0);
  DALSYS_BusyWait(delay);
  ftm_tdscdma_set_txagc(100,0,0);
  DALSYS_BusyWait(delay);

  if(step>0 && start<stop)
  {
    for(txagc=start; txagc<=stop; txagc+=step)
    {
      ftm_tdscdma_set_txagc(txagc,parange_override,parange);
      DALSYS_BusyWait(delay);
    }
  }
  else if(step<0 && start>stop)
  {
    for(txagc=start; txagc>=stop; txagc+=step)
    {
      ftm_tdscdma_set_txagc(txagc,parange_override,parange);
      DALSYS_BusyWait(delay);
    }
  }
  else
  {
    MSG_ERROR("Wrong user input",0,0,0);
  }
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_LNA_RANGE

DESCRIPTION
  This function sets the LNA range 0 (highest gain) to 3 (lowest gain).


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_lna_range(ftm_receive_chain_handle_type rx_chain, byte ftm_lna_range, byte ftm_rx_lin_state )
{
#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE
  rfdevice_tdscdma_set_lna_range_type set_lna_range_data;
  rfm_device_enum_type device = RFM_DEVICE_0; // Set default to device 0 for fixing KW warnings.
#else
  rflm_err_t exec_result;
  rflm_tds_schedule_rf_internal_event_in_t internal_event_data;
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */


#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE // This feature by default is NOT defined
  /* This API will program one chain (prx or drx) at a time */
  if (((rx_chain == FTM_RECEIVE_CHAIN_0) || (rx_chain == FTM_RECEIVE_CHAIN_2)) || !ftm_tdscdma_rx_div_enable)
  {
    device = ftm_tdscdma_current_pri_device;
    set_lna_range_data.program_pri_chain = TRUE;
    set_lna_range_data.program_sec_chain = FALSE;
    set_lna_range_data.pri_chain_action = RFDEVICE_EXECUTE_IMMEDIATE;
  }
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
  else if ( ((rx_chain == FTM_RECEIVE_CHAIN_1) || (rx_chain == FTM_RECEIVE_CHAIN_3)) || ftm_tdscdma_rx_div_enable)
  {
    device = ftm_tdscdma_current_sec_device;
    set_lna_range_data.program_pri_chain = FALSE;
    set_lna_range_data.program_sec_chain = TRUE;
    set_lna_range_data.sec_chain_action = RFDEVICE_EXECUTE_IMMEDIATE;
  }
  #endif /* FTM_HAS_TDSCDMA_RX_DIVERSITY */
  else
  {
    FTM_MSG_ERROR("rx_chain=%d invalid",rx_chain, 0, 0);
    return;
  }

  if (ftm_lna_range < RF_TDSCDMA_MAX_LNA_GAIN_STATES)
  {
    set_lna_range_data.band = ftm_tdscdma_current_mode; 
    set_lna_range_data.lna_range = ftm_lna_range;
    set_lna_range_data.lin_state = ftm_rx_lin_state;

    
    rfdevice_tdscdma_rx_cmd_dispatch(device,
                                     RFCOM_TDSCDMA_MODE,
                                     ftm_tdscdma_current_mode,
                                     RFDEVICE_TDSCDMA_SET_RX_GAIN_STATE,
                                     (uint8 *)&set_lna_range_data);

    rfdevice_tdscdma_rx_cmd_dispatch(device,
                                     RFCOM_TDSCDMA_MODE,
                                     ftm_tdscdma_current_mode,
                                     RFDEVICE_TDSCDMA_SET_LNA_LATCH,
                                     (uint8 *)&set_lna_range_data);

    /* Wait for LNA state transition to settle */
    DALSYS_BusyWait(ftm_tdscdma_lna_state_settling_delay_us);
  }
  else
  {
    FTM_MSG_ERROR("lna range=%d invalid",ftm_lna_range, 0, 0);
  }
  
#else /* Use FED to execute LNA script to set LNA */

  /* Error if the LNA Gx is not valid */
  if (ftm_lna_range >= RF_TDSCDMA_MAX_LNA_GAIN_STATES)
  { 
    FTM_MSG_ERROR("lna range=%d invalid", ftm_lna_range, 0, 0);
    return;
  }
  if (rf_tdscdma_use_on_rumi == FALSE)
  {
    /* This will program PRx and DRx chain separately */
    if (((rx_chain == FTM_RECEIVE_CHAIN_0) || (rx_chain == FTM_RECEIVE_CHAIN_2)) || (!ftm_tdscdma_rx_div_enable))
    {
      /* Program PRx */ 
      internal_event_data.actionCmd = RFLM_TDS_EXECUTE_SCRIPT; 
      internal_event_data.effectTime = 0xFFFFFFFF;
      internal_event_data.scriptType = RFLM_TDS_RX_LNA_SCRIPT;
      internal_event_data.scriptInfo.rxLnaScript.band = ftm_tdscdma_current_mode;
      internal_event_data.scriptInfo.rxLnaScript.gainState = ftm_lna_range;
      internal_event_data.scriptInfo.rxLnaScript.rxChain = (rflm_tds_rx_chain_e)FTM_RECEIVE_CHAIN_0;
      internal_event_data.scriptInfo.rxLnaScript.callInCalMode = TRUE;
      internal_event_data.scriptInfo.rxLnaScript.linState = ftm_rx_lin_state;
    }
    else if (((rx_chain == FTM_RECEIVE_CHAIN_1) || (rx_chain == FTM_RECEIVE_CHAIN_3)) && ftm_tdscdma_rx_div_enable)
    {
      /* Program DRx */ 
      internal_event_data.actionCmd = RFLM_TDS_EXECUTE_SCRIPT; 
      internal_event_data.effectTime = 0xFFFFFFFF;
      internal_event_data.scriptType = RFLM_TDS_RX_LNA_SCRIPT;
      internal_event_data.scriptInfo.rxLnaScript.band = ftm_tdscdma_current_mode;
      internal_event_data.scriptInfo.rxLnaScript.gainState = ftm_lna_range;
      internal_event_data.scriptInfo.rxLnaScript.rxChain = (rflm_tds_rx_chain_e)FTM_RECEIVE_CHAIN_1;
      internal_event_data.scriptInfo.rxLnaScript.callInCalMode = TRUE;
      internal_event_data.scriptInfo.rxLnaScript.linState = ftm_rx_lin_state;
    }   
    else
    {
      FTM_MSG_ERROR("Invalid configuration. rx_chain: %d, divEn: %d", rx_chain, ftm_tdscdma_rx_div_enable, 0);
      return;   
    }

    exec_result = rflm_tds_schedule_rf_internal_event(&internal_event_data);

    if(exec_result == RFLM_ERR_NONE)
    {
      /* Wait for LNA state transition to settle */
      //DALSYS_BusyWait(2*ftm_tdscdma_lna_state_settling_delay_us);
    }
    else
    {
      FTM_MSG_ERROR("Error when executing LNA scripts", rx_chain, ftm_tdscdma_rx_div_enable, 0);
    }
  }

#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */

  FTM_MSG_HIGH("Debug. rx_chain: %d, ftm_lna_range: %d, rxD: %d", rx_chain, ftm_lna_range, ftm_tdscdma_rx_div_enable);

} /* ftm_tdscdma_set_lna_range() */

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_LNA_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_lna_offset(ftm_receive_chain_handle_type rx_chain, byte index, int2 setting)
{

  if (( rx_chain == FTM_RECEIVE_CHAIN_0) || ( rx_chain == FTM_RECEIVE_CHAIN_2))
  {
    if (index < RF_TDSCDMA_MAX_LNA_GAIN_STATES)//valid
    {
      //hyang todo: call stanley's mdsp intf.
    }
    else
    {
      FTM_MSG_ERROR("lna range=%d invalid",index, 0,0);
    }
  }
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
  else if (( rx_chain == FTM_RECEIVE_CHAIN_1) || ( rx_chain == FTM_RECEIVE_CHAIN_3))
  {
   if (index < RF_TDSCDMA_MAX_LNA_GAIN_STATES)//valid
   {
     //hyang todo: call stanley's mdsp intf.
   }
   else
   {
     FTM_MSG_ERROR("lna range=%d invalid",index, 0,0);
   }
  }
  #endif /* FTM_HAS_TDSCDMA_RX_DIVERSITY */
  else
  {
    FTM_MSG_ERROR("rx_chain=%d invalid",rx_chain, 0,0);
  }
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_GET_LNA_OFFSET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_tdscdma_get_lna_offset(ftm_receive_chain_handle_type rx_chain, byte index, word expected_agc_val, byte rx_lin_state)
{
  int2 lna_offset=0;
  int2 current_agc_val=0;

  /*Note: for TDS, index 0 is valid. byte index is unsigned, thus only need to check upper bond.*/
  /* If the index is less than the max number of LNA range, set the LNA Range to the current index */
  if (index < RF_TDSCDMA_MAX_LNA_GAIN_STATES)
  {
    /* Reset the indexed LNA Offset to zero */
    //ftm_tdscdma_set_lna_offset( rx_chain, index , 0);

    /* Set the LNA range to the current index*/
    ftm_tdscdma_set_lna_range( rx_chain, index , rx_lin_state );

    current_agc_val = ftm_tdscdma_get_rx_agc(rx_chain);

    lna_offset = (int2)expected_agc_val - current_agc_val;

    //ftm_tdscdma_set_lna_offset( rx_chain, index , lna_offset);
  }
  else
  {
    FTM_MSG_ERROR("lna index=%d is invalid, max supported num=%d",index,RF_TDSCDMA_MAX_LNA_GAIN_STATES,0);

    lna_offset = 999;//return invalid value
  }

  return (lna_offset);
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_GET_RX_AGC

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int2 ftm_tdscdma_get_rx_agc(ftm_receive_chain_handle_type rx_chain)
{
  int2 agc_read_val=0;
  int2 num_of_agc_reads=0;
  int2 current_agc_val=0;
  rf_tdscdma_mdsp_slow_cali_results_type agc_result = {0};
  rf_tdscdma_mdsp_slow_cali_results_type agc_result2 = {0};

  int2 num_of_agc_reads_validity_counter = 0;
  /*v3 timeline is fine since we config earlier. xo cal needs this at least 4*/
  const uint8 num_of_agc_reads_validity_counter_max = 50;
  const int2 rx_agc_converage_threshold = 10;//1db
  ftm_tdscdma_set_rx(TRUE);

  for (num_of_agc_reads = 0;
      num_of_agc_reads < FTM_TDSCDMA_NUM_OF_AVG_RX_AGC_READ;
      num_of_agc_reads +=2)//two reads per iteration
  {
    if ((rx_chain == FTM_RECEIVE_CHAIN_0) || (rx_chain == FTM_RECEIVE_CHAIN_2))
    {
       do
       {
         /* call box may be relatively slow to change to desired power */
         DALSYS_BusyWait(120);
         agc_result = rf_tdscdma_mdsp_get_rxagc();
         /*100us = 128chips, with 20us margin*/
         DALSYS_BusyWait(120);
         agc_result2 = rf_tdscdma_mdsp_get_rxagc();
       }while (
          (++num_of_agc_reads_validity_counter <= num_of_agc_reads_validity_counter_max)
          &&(
            (agc_result.dlPowerLevel < -600)//invalid
            ||(agc_result2.dlPowerLevel < -600)//invalid
            ||(abs(agc_result.dlPowerLevel - agc_result2.dlPowerLevel) > rx_agc_converage_threshold)
            ||(agc_result.index == agc_result2.index)//index stuck, mdsp not updating shared mem.
          )
        );

       agc_read_val += agc_result.dlPowerLevel;
       agc_read_val += agc_result2.dlPowerLevel;


    }
 #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
    else if ((rx_chain == FTM_RECEIVE_CHAIN_1) || (rx_chain == FTM_RECEIVE_CHAIN_3))
    {
       do
       {
          agc_result = rf_tdscdma_mdsp_get_rxagc();
         /*100us = 128chips, with 20us margin*/
         DALSYS_BusyWait(120);
          agc_result2 = rf_tdscdma_mdsp_get_rxagc();
       }while (
          (++num_of_agc_reads_validity_counter <= num_of_agc_reads_validity_counter_max)
          &&(
            (agc_result.dlPowerLevelDiv < -600)//invalid
            ||(agc_result2.dlPowerLevelDiv < -600)//invalid
            ||(abs(agc_result.dlPowerLevelDiv - agc_result2.dlPowerLevelDiv) > rx_agc_converage_threshold)
            ||(agc_result.index == agc_result2.index)//index stuck, mdsp not updating shared mem.
          )
        );

       agc_read_val += agc_result.dlPowerLevelDiv;
       agc_read_val += agc_result2.dlPowerLevelDiv;
     }
#endif
       if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
       {
        FTM_MSG_MED("agc read valid counter=%d, max=%d",\
        num_of_agc_reads_validity_counter,
        num_of_agc_reads_validity_counter_max,0);

        /*print agc result if debug mode is enabled.
        The first valid rate usually takes place in the 4th attempt*/
       FTM_MSG_MED("num_of_agc_reads=%d,index=%d,dlPowerLevel=%d",\
        num_of_agc_reads,agc_result.index,agc_result.dlPowerLevel);
       FTM_MSG_MED("num_of_agc_reads=%d,index=%d,dlPowerLevel=%d",\
        num_of_agc_reads +1 ,agc_result2.index,agc_result2.dlPowerLevel);

#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY

      FTM_MSG_MED("num_of_agc_reads=%d,index=%d,dlPowerLevelDiv=%d",\
         num_of_agc_reads,agc_result.index,agc_result.dlPowerLevelDiv);
      FTM_MSG_MED("num_of_agc_reads=%d,index=%d,dlPowerLevelDiv=%d",\
         num_of_agc_reads +1 ,agc_result2.index,agc_result2.dlPowerLevelDiv);
#endif

       if(agc_result.index == agc_result2.index)//index stuck, mdsp not change.
       {
         FTM_MSG_ERROR("agc_result.index=%d,agc_result2.index=%d seems stuck.",\
          agc_result.index,agc_result2.index,0);
       }
      }

  }

  current_agc_val = agc_read_val/FTM_TDSCDMA_NUM_OF_AVG_RX_AGC_READ;

  return current_agc_val;
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_MODE

DESCRIPTION
    This function will switch modes to TDSCDMA.

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS
   When a mode is changed WHILE transmitting, this function will
   automatically shut down the transmit for the old mode and enter the
   transmit state for the new mode.

===========================================================================*/
void ftm_tdscdma_set_mode(ftm_mode_id_type mode)
{
   boolean change_mode = FALSE;
    /* default chan is removed to meet V3 1ms timeline */
   //uint16 default_channel = 10088;

   if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
   {
   FTM_MSG_HIGH("ftm_tdscdma_set_mode:enter mode %d",mode,0,0);
   }

   /* Set the current RF mode to TDSCDMA */
   if (mode == FTM_PHONE_MODE_TDSCDMA_B34)
   {
      ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B34;
      ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B34;
      ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B34;
      change_mode = TRUE;
      //default_channel = 10081;
      if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
      {
        ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
        ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
        ftm_tdscdma_current_tx_device = RFM_DEVICE_0;

        FTM_MSG_HIGH("ftm_tdscdma_set_mode updated pri_device=%d, sec_device=%d, tx_device=%d",
                     ftm_tdscdma_current_pri_device,
                     ftm_tdscdma_current_sec_device,
                     ftm_tdscdma_current_tx_device);
      }
   }
   else if (mode == FTM_PHONE_MODE_TDSCDMA_B39)
   {
      ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B39;
      ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B39;
      ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B39;
      change_mode = TRUE;
      //default_channel = 9500;
      if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
      {
        ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
        ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
        ftm_tdscdma_current_tx_device = RFM_DEVICE_0;

        FTM_MSG_HIGH("ftm_tdscdma_set_mode updated pri_device=%d, sec_device=%d, tx_device=%d",
                     ftm_tdscdma_current_pri_device,
                     ftm_tdscdma_current_sec_device,
                     ftm_tdscdma_current_tx_device);
      }
   }
   else if (mode == FTM_PHONE_MODE_TDSCDMA_B40)
   {
      ftm_current_rf_mode = FTM_DB_RF_TDSCDMA_B40;
      ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_B40;
      ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_TDSCDMA_B40;
      change_mode = TRUE;
      //default_channel = 11750;
      if (ftm_tdscdma_current_pri_device == RFM_INVALID_DEVICE)
      {
        ftm_tdscdma_current_pri_device = RFM_DEVICE_0;
        ftm_tdscdma_current_sec_device = RFM_DEVICE_1;
        ftm_tdscdma_current_tx_device = RFM_DEVICE_0;

        FTM_MSG_HIGH("ftm_tdscdma_set_mode updated pri_device=%d, sec_device=%d, tx_device=%d",
                     ftm_tdscdma_current_pri_device,
                     ftm_tdscdma_current_sec_device,
                     ftm_tdscdma_current_tx_device);
      }
   }
   else
   {
      FTM_MSG_ERROR("mode %d is invalid.",mode,0,0);
   }

   /* If it's already in DB_RF_TDSCDMA mode, skip setting the same mode again */
   if (ftm_get_current_state(ftm_tdscdma_current_pri_device) != FTM_STATE_TDSCDMA)
   {
      //load mdsp image, and config basic clks
      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
        FTM_MSG_HIGH("calling ftm_rfmode_enter(%d, FTM_STATE_TDSCDMA)", ftm_tdscdma_current_pri_device,0,0);
      }

      ftm_rfmode_enter(ftm_tdscdma_current_pri_device, FTM_STATE_TDSCDMA);

      /*Called to update rfm_mode in FTM mode.*/
      rfm_enter_mode(ftm_tdscdma_current_pri_device, RFCOM_TDSCDMA_MODE, NULL, NULL, 0);
   }

   /*Initialize NV ptr. */
   rfnv_tdscdma_rx_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(ftm_tdscdma_current_mode);
   rfnv_tdscdma_tx_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);

   if(change_mode)
   {
      /*default chan is removed to meet V3 1ms timeline */
      //ftm_tdscdma_tune_to_chan(FTM_RECEIVE_CHAIN_0,default_channel);
   }


   if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
   {
     FTM_MSG_HIGH("ftm_tdscdma_set_mode:done mode %d",mode,0,0);
   }

   // ftm_tdscdma_set_secondary_chain(TRUE);

}        /* end ftm_tdscdma_set_mode */

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_RX

DESCRIPTION
   This function will set the RX state for TDSCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Tx will be turned off.

===========================================================================*/
void ftm_tdscdma_set_rx(boolean state)
{
  /*the check for state variable here is removed,
  since Tx action can directly turn off Rx.
  all actions go through ftm_tdscdma_config_cal_state tp insure consistant result*/
  if ( state == TRUE )
  {
    ftm_tdscdma_config_cal_state(FTM_TDSCDMA_CAL_STATE_RX);

	if (FTM_SYNTH_STATUS_DISABLED == rx_pll_multi_state)
	{
		rx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
	    FTM_MSG_HIGH("Set Multi Synth Mode for Rx in Calibration Mode: Disabled -> Locked",0,0,0);
	}

    if (FTM_SYNTH_STATUS_DISABLED == tx_pll_multi_state)
    {
	    tx_pll_multi_state = FTM_SYNTH_STATUS_NOT_APPLICABLE;
	    FTM_MSG_HIGH("Set Multi Synth Mode for Tx in Calibration Mode: Disabled -> Not Applicable",0,0,0);
    }


  }
  else //turn off
  {
    /*only issue turn off command if current state is rx
    so that we wont touch tfw if we are in tx cal state*/
    if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_RX)
    {
      ftm_tdscdma_config_cal_state(FTM_TDSCDMA_CAL_STATE_INACTIVE);

	  /*Only modify tx pll multi state when it's under Rx Calibration state*/
	if (FTM_SYNTH_STATUS_NOT_APPLICABLE == tx_pll_multi_state)
	{
	    tx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
		  FTM_MSG_HIGH("Set Multi Synth Mode for Tx in Calibration Mode: Not Applicable -> Disabled",0,0,0);
	  }
    }
  }
} /* end ftm_tdscdma_set_rx */


/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TX

DESCRIPTION
   This function will set the TX state for TDSCDMA.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   Rx will be turned off.

===========================================================================*/
void ftm_tdscdma_set_tx(boolean state)
{
  /*the check for state variable here is removed,
  so that we wont touch tfw if we are in tx cal state*/
  if ( state == TRUE )
  {
    ftm_tdscdma_config_cal_state(FTM_TDSCDMA_CAL_STATE_TX);

	if (FTM_SYNTH_STATUS_DISABLED == tx_pll_multi_state)
	{
           tx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
	   FTM_MSG_HIGH("Set Multi Synth Mode for Tx: Disabled -> Locked",0,0,0);
	}

    if (FTM_SYNTH_STATUS_DISABLED == rx_pll_multi_state)
    {
	    rx_pll_multi_state = FTM_SYNTH_STATUS_NOT_APPLICABLE;
	    FTM_MSG_HIGH("Set Multi Synth Mode for Rx: Disabled -> Not Applicable",0,0,0);
    }
    //accomodate call seq. from toolside: i. Switch to DPD -> ii. Set Tx_On
    //ftm_tdscdma_curr_tx_waveform = FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST;
  }
  else //turn off
  {
    /*only issue turn off command if current state is tx*/
    if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_TX)
    {
      ftm_tdscdma_config_cal_state(FTM_TDSCDMA_CAL_STATE_INACTIVE);

	  /*Only modify rx pll multi state when it's under Tx Calibration state*/
	if (FTM_SYNTH_STATUS_NOT_APPLICABLE == rx_pll_multi_state)
	{
	    rx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
		FTM_MSG_HIGH("Set Multi Synth Mode for Rx: Not Applicable -> Disabled",0,0,0);
	}
    }
  }
} /* end ftm_tdscdma_set_tx */

void ftm_tdscdma_set_tx_continuous(boolean state)
{
  FTM_MSG_ERROR("ftm_tdscdma_set_tx_continuous: command not used",0,0,0);
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_PA

DESCRIPTION
   This function will set the pa on/off.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_tdscdma_set_pa(boolean state)
{
  (void)rfdevice_tdscdma_pa_on_off(ftm_tdscdma_current_tx_device,
                                   ftm_tdscdma_current_mode,
                                   state,
                                   NULL,
                                   RFDEVICE_EXECUTE_IMMEDIATE,
                                   RFCOM_INVALID_TIMING_OFFSET);
} /* end tdscdma_set_pa */

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_UPLINK_WAVEFORM

DESCRIPTION
   This function will program the encoder to generate a TDSCDMA uplink waveform.
   It is used only for RF testing purposes.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
LOCAL void ftm_tdscdma_set_uplink_waveform(void)
{
  ftm_tdscdma_send_txlm_cfg_cmd(ftm_tdscdma_txlm_buffer.buf_idx, TRUE);

  /* Configurate modulator timing in mDSP */
  ftm_tdscdma_modulator_timing();

  /* Configurate DPCCH in mDSP */
  ftm_tdscdma_uplink_dpcch_config(TRUE);

  /* Configurate Uplink modulator */
  ftm_tdscdma_config_modulator();

  ftm_tdscdma_eul_mpr_idx = 0;
}

/*===========================================================================
FUNCTION  FTM_TDSCDMA_SET_PA_RANGE

DESCRIPTION
  This function sets PA range 0 (low gain) or 1 (high gain).

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ftm_tdscdma_set_pa_range(byte paRange)
{
   if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
   {
       FTM_MSG_HIGH("ftm_tdscdma_set_pa_range: paRange %d",paRange,0,0);
   }

   rfdevice_tdscdma_pa_set_gain_range(ftm_tdscdma_current_tx_device,
                                      ftm_tdscdma_current_mode,
                                      paRange,
                                      NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      RFCOM_INVALID_TIMING_OFFSET);


   ftm_tdscdma_cur_pa_range = paRange;
}

/*===========================================================================

FUNCTION ftm_uplink_sweep_cal

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_uplink_sweep_cal (boolean enable )
{
  /* Lock interrupt to prevent ftm_uplink_sweep_cal_isr() kicking in and changing ftm_tdscdma_cur_tx_agc_adj_pdm */


  ftm_tdscdma_uplink_sweep_cal_enable = enable;

  if (ftm_tdscdma_uplink_sweep_cal_enable)
  {
    if (ftm_tdscdma_cur_tx_agc_adj_pdm > FTM_UPLINK_SWEEP_CAL_MAX)
    {
      /* Preventing user set the maximum Tx power exceeding FTM_UPLINK_SWEEP_CAL_MAX */
      ftm_tdscdma_cur_tx_agc_adj_pdm = FTM_UPLINK_SWEEP_CAL_MAX;
    }

    /* Now store the maximum Tx AGC adjust PDM specified by the user */
    ftm_tdscdma_tx_agc_adj_pdm_max = ftm_tdscdma_cur_tx_agc_adj_pdm;

    ftm_tdscdma_hdet_index = 0;

    ftm_tdscdma_hdet_outloop_index = 0;

    /* Reset tx_agc_adj_pdm back to the maximum */
    ftm_tdscdma_tx_agc_adj_pdm = ftm_tdscdma_tx_agc_adj_pdm_max;

    ftm_common_msm_install_timer ((uint32)ftm_tdscdma_tx_cal_sweep_interval, ftm_tdscdma_uplink_sweep_cal_isr);

  }
  else
  {
    ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, ftm_tdscdma_tx_agc_adj_pdm_max);
    ftm_common_msm_uninstall_timer();
  }

  /* Reset uplink Cal sweep internal counter */
  ftm_tdscdma_uplink_sweep_int_cnt = 0;

} /* ftm_uplink_sweep_cal() */

/*===========================================================================

FUNCTION   FTM_UPLINK_SWEEP_CAL_ISR

DESCRIPTION
   FTM Uplink Sweep Calibration service routine.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_uplink_sweep_cal_isr(void)
{
  int ftm_tdscdma_adc_read_cnt=0;
  uint16 ftm_tdscdma_local_temp_hdet_val=0;

  /* Reset local_hdet_val */
  ftm_tdscdma_local_hdet_val = 0;
  if (is_wtr_hdet)
  {
  for (ftm_tdscdma_num_of_hdet_reads = 0; ftm_tdscdma_num_of_hdet_reads < ftm_tdscdma_num_of_avg_hdet_read; ftm_tdscdma_num_of_hdet_reads++)
  {
    ftm_tdscdma_adc_read_cnt = 0;
    do
    {
      rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                       RFCOM_TDSCDMA_MODE,
                                       ftm_tdscdma_current_mode,
                                       RFDEVICE_GET_HDET_VALUE,
                                       &ftm_tdscdma_local_temp_hdet_val);

      ftm_tdscdma_adc_read_cnt++;
    } while ( ((ftm_tdscdma_local_temp_hdet_val <= 0) || ftm_tdscdma_local_temp_hdet_val == 0xFFFF)
              && (ftm_tdscdma_adc_read_cnt < 3));

    ftm_tdscdma_local_hdet_val += ftm_tdscdma_local_temp_hdet_val;
  }
  }
else
  {
    for (ftm_tdscdma_num_of_hdet_reads = 0; ftm_tdscdma_num_of_hdet_reads < ftm_tdscdma_num_of_avg_hdet_read; ftm_tdscdma_num_of_hdet_reads++)
  {
    ftm_tdscdma_adc_read_cnt = 0;
    do
    {
         (void)rfdevice_hdet_tdscdma_tx_hdet_read(ftm_tdscdma_current_tx_device,
                                                  RFCOM_BAND_TDSCDMA_ANY_BAND,
                                                  FALSE,
                                                  &ftm_tdscdma_local_temp_hdet_val,
                                                  NULL,
                                                  RFDEVICE_EXECUTE_IMMEDIATE);

      ftm_tdscdma_adc_read_cnt++;
    } while ( ((ftm_tdscdma_local_temp_hdet_val <= 0) || ftm_tdscdma_local_temp_hdet_val == 0xFFFF)
              && (ftm_tdscdma_adc_read_cnt < 3));

    ftm_tdscdma_local_hdet_val += ftm_tdscdma_local_temp_hdet_val;
  }
  }

  if (ftm_tdscdma_uplink_sweep_cal_enable && (ftm_tdscdma_uplink_sweep_int_cnt == 0))
  {
    /* Force a low to high transition and serve as a trigger for spectrum analyzer */
    /* If the pdm lower limit is not zero then drop the pdm in two steps to avoid
     * a glitch in the output waveform. */
    if (ftm_tdscdma_pdm_limits.min_pdm > 0 && ftm_tdscdma_tx_agc_adj_pdm > FTM_TDSCDMA_SWEEP_INTERM_LEVEL)
    {
      ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, FTM_TDSCDMA_SWEEP_INTERM_LEVEL);
      DALSYS_BusyWait(20);
    }
    /* Do not use the absolute lowest pdm since it may cause a glitch in the output */
    ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, (int2)ftm_tdscdma_pdm_limits.min_pdm+1);
  }
  else if (ftm_tdscdma_uplink_sweep_cal_enable)
  {
    if (ftm_tdscdma_uplink_sweep_int_cnt > 1)
    {
      if (ftm_tdscdma_num_of_avg_hdet_read > 0)//not zero.
      {
        ftm_tdscdma_tx_cal_sweep_hdet_val[ftm_tdscdma_hdet_index-1] = (int2)ftm_tdscdma_local_hdet_val/ftm_tdscdma_num_of_avg_hdet_read;
      }
      else
      {
        ftm_tdscdma_tx_cal_sweep_hdet_val[ftm_tdscdma_hdet_index-1] = 0;
      }
    }

    /* Do not use the absolute lowest pdm since it may cause a glitch in the output */
    if (ftm_tdscdma_tx_agc_adj_pdm <= ftm_tdscdma_pdm_limits.min_pdm)
    {
      ftm_tdscdma_tx_agc_adj_pdm =(int2)ftm_tdscdma_pdm_limits.min_pdm + 1;
    }
    ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, ftm_tdscdma_tx_agc_adj_pdm);

    /* Decrease Tx power level by the amount of ftm_uplink_sweep_cal_step until it reaches
       the minimum pdm plus one. */
    if (ftm_tdscdma_tx_agc_adj_pdm > ftm_tdscdma_pdm_limits.min_pdm + 1)
    {
      ftm_tdscdma_tx_agc_adj_pdm -= ftm_tdscdma_uplink_sweep_cal_step;
      ftm_tdscdma_hdet_index++;
    }
    else
    {
      /* Fill the rest of unused ftm_tx_cal_sweep_hdet_val[] with zeros */
      do
      {
        ftm_tdscdma_tx_cal_sweep_hdet_val[ftm_tdscdma_hdet_index] = 0;
      } while (ftm_tdscdma_hdet_index++ < FTM_UPLINK_SWEEP_CAL_SIZE-1 );

      /* Reset tx_agc_adj_pdm back to the maximum */
      ftm_tdscdma_tx_agc_adj_pdm = ftm_tdscdma_tx_agc_adj_pdm_max;

      /* Reset hdet_index */
      ftm_tdscdma_hdet_index = 0;

      /* It does the Tx sweep again in case the spectrum analyzer trig missed the 1st sweep */
      ftm_tdscdma_hdet_outloop_index++;

      /* turn off sweep after one full iterations */
      if (ftm_tdscdma_hdet_outloop_index == 1)
      {
        ftm_tdscdma_uplink_sweep_cal(OFF);
      }
    }
  }
  ftm_tdscdma_uplink_sweep_int_cnt++;
}


/*===========================================================================

FUNCTION       FTM_TDSCDMA_HDET_TRACKING_CB

DESCRIPTION
  1.25ms callback function for closed-loop tracking of HDET.
  Adjusts TX_AGC_ADJ PDM to control HDET to the desired set point.
===========================================================================*/
void ftm_tdscdma_hdet_tracking_cb( int4 interval )
{
#ifdef FEATURE_FTM_HWTC
  ftm_hdet_track_debug_log_type *hdet_log = NULL;
  static unsigned int log_count = 4;
#endif /* FEATURE_FTM_HWTC */

  unsigned int hdet_accum=0;             // kept at unsigned 16Q8
  unsigned int new_pdm_val=0;
  byte rf_hdet_data_trk=0;
  uint16 temp_hdet=0;
  uint8 hdet_count=0;

  const unsigned short hdet_accum_scale = 63701; // 0.972 in unsigned 16Q16
  const unsigned short hdet_update_scale = 1835; // 0.028 in unsigned 16Q16

  const short pdm_proportional_scale = 152;    // -0.00463 in signed 16Q15

  do
  {
    if (is_wtr_hdet)
    {
      rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                       RFCOM_TDSCDMA_MODE,
                                       ftm_tdscdma_current_mode,
                                        RFDEVICE_GET_HDET_VALUE,
                                       &temp_hdet);
    }

   else
   {
     (void)rfdevice_hdet_tdscdma_tx_hdet_read(ftm_tdscdma_current_tx_device,
                                              ftm_tdscdma_current_mode,
                                              FALSE,
                                              &temp_hdet,
                                              NULL,
                                              RFDEVICE_EXECUTE_IMMEDIATE);
   }


    hdet_count++;
  } while (temp_hdet == 0xFFFF && hdet_count <= 5);
  rf_hdet_data_trk = (byte)temp_hdet;


  hdet_accum = hdet_tdscdma_tracking_params_ptr->hdet_accum;
  hdet_accum = hdet_update_scale*(rf_hdet_data_trk<<8)
         + hdet_accum_scale*hdet_accum;
  hdet_accum = hdet_accum >> 16; // Convert from 32Q24 back to 16Q8
  hdet_tdscdma_tracking_params_ptr->hdet_accum = hdet_accum;

  /* Compute the new PDM value based on the HDET error */
  new_pdm_val = hdet_tdscdma_tracking_params_ptr->hdet_tracking_pdm;
  new_pdm_val = new_pdm_val + pdm_proportional_scale*(((int)hdet_tdscdma_tracking_params_ptr->hdet_set_point<<8) - (int)hdet_accum);
  /*   32Q23    =    32Q23    +     16Q15           *(           8Q0   to  16Q8                          -          16Q8   */

  hdet_tdscdma_tracking_params_ptr->hdet_tracking_pdm = new_pdm_val;

  /* Round and write to PDM */
  ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ,((new_pdm_val + (1<<22)) >> 23));

#ifdef FEATURE_FTM_HWTC
  log_count--;
  if (log_count == 0)
  {
    log_count = 4;
    hdet_log = (ftm_hdet_track_debug_log_type *)ftm_log_malloc(0xF000, sizeof(ftm_hdet_track_debug_log_type));
    if(hdet_log != NULL)
      {
        hdet_log->hdet = rf_hdet_data_trk;
        hdet_log->filt_hdet = (int16)hdet_accum;
        hdet_log->tx_agc_pdm = (new_pdm_val + (1<<22)) >> 23;
        log_commit(hdet_log);
      }

  }
#endif /* FEATURE_FTM_HWTC */


} /* ftm_HDET_tracking */

/*===========================================================================

FUNCTION       FTM_TDSCDMA_SET_HDET_TRACKING

DESCRIPTION
  If state is true, HDET tracking function is called, which does HDET tracking till the state becomes FALSE
===========================================================================*/
void ftm_tdscdma_set_hdet_tracking( boolean state, unsigned short set_point )
{
  if (state) {
    /* Allocate space */
    if (hdet_tdscdma_tracking_params_ptr == NULL) {
      hdet_tdscdma_tracking_params_ptr = ftm_malloc(sizeof(hdet_tracking_params_type));

      if (hdet_tdscdma_tracking_params_ptr == NULL) {
        FTM_MSG_ERROR("Failed to allocate memory for ISR parameter structure", 0, 0, 0);
        return;
      }
    }

    /* Setup the parameter block */
    hdet_tdscdma_tracking_params_ptr->hdet_accum = set_point << 8;
    hdet_tdscdma_tracking_params_ptr->hdet_set_point = set_point;
    hdet_tdscdma_tracking_params_ptr->hdet_tracking_pdm = 450U << 23;

    /* Initialize the PDM to a close value */
    ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ,(hdet_tdscdma_tracking_params_ptr->hdet_tracking_pdm >> 23));

    /* Give the HDET circuit time to settle */
    ftm_clk_rex_wait(50);

    /* Start the FTM HDET Callback to do closed loop control */

    /* Allocate space for callback structure */
    ftm_tdscdma_hdet_cb_struct = (timer_type*)ftm_malloc(sizeof(timer_type));
    if (ftm_tdscdma_hdet_cb_struct == NULL) {
      FTM_MSG_ERROR("Failed to allocate memory for callback structure", 0, 0, 0);
      return;
    }

    /* Initialize and register a callback structure */
    timer_def2( ftm_tdscdma_hdet_cb_struct, NULL );
    timer_reg( ftm_tdscdma_hdet_cb_struct,
               (timer_t2_cb_type)ftm_tdscdma_hdet_tracking_cb,
               (timer_cb_data_type)NULL,
               (uint32)ftm_tdscdma_hdet_cb_interval,
               (uint32)ftm_tdscdma_hdet_cb_interval);
  } else {
    /* Diable the FTM HDET Callback function */
    /* Deregister a callback structure */
    (void)timer_clr(ftm_tdscdma_hdet_cb_struct, T_NONE);
  }
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_CONFIGURE_TX_SWEEP_CAL

DESCRIPTION
  This function allows the user to set the length of each step in the tx cal
  sweep and the number of HDET readings averaged per step.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_configure_tx_sweep_cal(uint16 interval, byte num_avg)
{
  if (interval >= FTM_TDSCDMA_TX_SWEEP_INTERVAL_LOWER_LIMIT)
  {
    ftm_tdscdma_tx_cal_sweep_interval = interval;
  }
  else
  {
    FTM_MSG_ERROR("Tx sweep interval %d is too low.",interval,0,0);
  }

  ftm_tdscdma_num_of_avg_hdet_read = num_avg;

}

/*===========================================================================

FUNCTION FTM_TDSCDMA_SET_TX_SWEEP_STEP_SIZE

DESCRIPTION
  This function allows the user to set the PDM step size for the TX cal sweep.
  The PDM will be decreased by the given number of counts for each step in
  the sweep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_tx_sweep_step_size(uint8 step)
{
  if (step >= FTM_UPLINK_SWEEP_CAL_MIN_STEP)
  {
    ftm_tdscdma_uplink_sweep_cal_step = step;
  }
  else
  {
    ftm_tdscdma_uplink_sweep_cal_step = FTM_UPLINK_SWEEP_CAL_MIN_STEP;
    FTM_MSG_ERROR("FTM TX sweep step size too low, using %d.", ftm_tdscdma_uplink_sweep_cal_step, 0, 0);
  }

}

/*===========================================================================

FUNCTION FTM_TDSCDMA_TUNE_TO_CHAN

DESCRIPTION
  This function tunes the radio to a given uplink channel number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_tune_to_chan(ftm_receive_chain_handle_type rx_chain, uint16 chan)
{
   const ftm_tdscdma_cal_state_type ftm_tdscdma_cal_state_backup = ftm_tdscdma_current_cal_state;

   const rfcom_tdscdma_band_type ftm_tdscdma_target_mode = rf_tdscdma_core_util_get_band_from_uarfcn(chan);

   ftm_tdscdma_tx_waveform_type waveform_b4_set_chan_flag = FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST;

   FTM_MSG_LOW("ftm_tdscdma_tune_to_chan: ftm_tdscdma_current_cal_state %d",ftm_tdscdma_current_cal_state,0, 0);

   if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
   {
   FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: enter rx_chain %d, chan %d",rx_chain,chan, 0);
   }


   if (ftm_tdscdma_target_mode == RFCOM_BAND_TDSCDMA_INVALID)
   {
     FTM_MSG_ERROR("ftm_tdscdma_tune_to_chan: not a valid channel", chan, 0, 0);
     return;
   }
   else if(ftm_tdscdma_current_mode != ftm_tdscdma_target_mode)//chan is in different band
   {
     if (((ftm_tdscdma_target_mode == RFCOM_BAND_TDSCDMA_B40_B) && (ftm_tdscdma_current_mode == RFCOM_BAND_TDSCDMA_B40))
	    || ((ftm_tdscdma_target_mode == RFCOM_BAND_TDSCDMA_B40) && (ftm_tdscdma_current_mode == RFCOM_BAND_TDSCDMA_B40_B)))
	 {
        ftm_tdscdma_current_mode = ftm_tdscdma_target_mode; // Set current mode to target mode
	 }
	 else
	 {
	   FTM_MSG_ERROR("ftm_tdscdma_tune_to_chan: chan is in a different band %d from current band %d",
                     ftm_tdscdma_target_mode, ftm_tdscdma_current_mode, 0);
       FTM_MSG_ERROR("must call ftm_tdscdma_set_mode() first!", 0 ,0, 0);
	   return;
	 }
   }

   //need to toggle rx on signal
   //when tune to a new chan, invalidate cal state.
   if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_TX)
   {
      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
      FTM_MSG_HIGH("tx is on before tune to chan, turn tx off", 0, 0, 0);

      }
      //align RF/FW OFF status for DPD waveform for BOLT
      if (ftm_tdscdma_curr_tx_waveform == FTM_TDSCDMA_TX_WAVEFORM_DPD)
      {
        ftm_tdscdma_switch_tx_waveform((byte)0);
        waveform_b4_set_chan_flag = FTM_TDSCDMA_TX_WAVEFORM_DPD;
      }      
      ftm_tdscdma_set_tx(FALSE);
   }
   else if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_RX)
   {
      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
      FTM_MSG_HIGH("rx is on before tune to chan, turn rx off", 0, 0, 0);

      }
      ftm_tdscdma_set_rx(FALSE);
   }


   /*disable tx. config_cal_state no longer calls mc_tx_disable */
   if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
   {
      if(IS_FTM_IN_TEST_MODE() && !rfm_get_calibration_state())
      {
         // Set Tx burst OFF in FTM RF mode
         ftm_tdscdma_burst_tx(FALSE);
      }
      else
      {
        rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
      }
   }

   if(!ftm_tdscdma_rx_div_enable)
   {
     /* Set RXLM to Cx2 if we are in RF Mode or set RXLM to Cx1 in calibration mode */
     if (rfm_get_calibration_state())
     {
       // Primary chain only
       rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device,
                               ftm_tdscdma_current_sec_device,
                               chan,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_ACQ,
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                               RFA_TDSCDMA_RX_PATH_PRIM,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_ACQ,
  #endif
                               NULL,
                               NULL,
                               FALSE, // is_dsds
                               ftm_tdscdma_rx_lin_state,
                               TRUE   // always use SW tune in ACQ
                               );
     }
     else
     {
       rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device,
                               ftm_tdscdma_current_sec_device,
                               chan,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_TRK,
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                               RFA_TDSCDMA_RX_PATH_PRIM,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_TRK,
  #endif
                               NULL,
                               NULL,
                               FALSE, // is_dsds
                               ftm_tdscdma_rx_lin_state,
                               TRUE   // always use SW tune in ACQ
                               );

     }

      //RXLM
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXLM_CFG_MSG,
                                    ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);
   }
   else
   {
     /* Set RXLM to Cx2 if we are in RF Mode or set RXLM to Cx1 in calibration mode */
     if (rfm_get_calibration_state())
     {
       // Primary + diversity chain
       rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device,
                               ftm_tdscdma_current_sec_device,
                               chan,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_ACQ,
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                               RFA_TDSCDMA_RX_PATH_BOTH,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_ACQ,
  #endif
                               NULL,
                               NULL,
                               FALSE, // is_dsds
                               ftm_tdscdma_rx_lin_state,
                               TRUE   // always use SW tune in ACQ
                               );
     }
     else
     {
       // Primary + diversity chain
       rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device,
                               ftm_tdscdma_current_sec_device,
                               chan,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_TRK,
  #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                               RFA_TDSCDMA_RX_PATH_BOTH,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                               RFA_TDSCDMA_RXLM_MODE_TRK,
  #endif
                               NULL,
                               NULL,
                               FALSE, // is_dsds
                               ftm_tdscdma_rx_lin_state,
                               TRUE   // always use SW tune in ACQ
                               );
     }

      //RXLM
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXLM_CFG_MSG,
                                    ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);

#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG, (ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);
#endif
   }
#if 0
   {
      FTM_MSG_ERROR("FTM invalid rx_chain=%d", rx_chain, 0, 0);
   }
#endif

   if (rfm_get_calibration_state())
   {
   /*enable tx*/
   rf_tdscdma_mc_enable_tx(ftm_tdscdma_current_tx_device, 
                           chan, 
                           ftm_tdscdma_txlm_buffer.buf_idx, 
                           NULL);

   /*notify mdsp */
   rf_tdscdma_mdsp_tx_hw_block_config(TRUE);
   rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_TXLM_CFG_MSG, ftm_tdscdma_txlm_buffer.buf_idx);
   }

   ftm_tdscdma_current_chan = chan;

   if (ftm_tdscdma_cal_state_backup == FTM_TDSCDMA_CAL_STATE_TX)
   {

      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
       FTM_MSG_HIGH("turn tx back on", 0, 0, 0);
      }

      ftm_tdscdma_set_tx(TRUE);
       
      if (waveform_b4_set_chan_flag == FTM_TDSCDMA_TX_WAVEFORM_DPD)
      {
        ftm_tdscdma_switch_tx_waveform((byte)1);        
      }       
   }
   else if(ftm_tdscdma_cal_state_backup == FTM_TDSCDMA_CAL_STATE_RX)
   {
      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
       FTM_MSG_HIGH("turn rx back on", 0, 0, 0);
      }

       ftm_tdscdma_set_rx(TRUE);
   }

   rf_tdscdma_ftm_wrk_chan = chan;

   if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
   {
   FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: done rx_chain %d, chan %d",rx_chain,chan, 0);
}

   FTM_MSG_LOW("ftm_tdscdma_tune_to_chan: ftm_tdscdma_current_cal_state %d",ftm_tdscdma_current_cal_state,0, 0);
}

/*===========================================================================

FUNCTION FTM_TX_RX_FREQ_CAL_LOG_RESULTS

DESCRIPTION
  This function sends a log packet with the results of the TX/RX frequency
  calibration sweep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_tx_rx_freq_cal_log_results( void )
{
  ftm_log_gen_type *log_ptr=NULL;
  uint8 *data_ptr=NULL;
  uint8 i=0;

  FTM_MSG_HIGH("ftm_tdscdma_tx_rx_freq_cal_log_results:enter",0,0,0);

  /* create log packet to return results */
  /* ftm_log_gen_type contains headers only */
  /* log_ftm_gen_type contains headers and a data placeholder */
  log_ptr = (ftm_log_gen_type*)ftm_log_malloc((uint16)FTM_LOG_TX_RX_SWEEP,
                                              sizeof(ftm_log_gen_type) +
                                              2*sizeof(uint16)*ftm_tdscdma_tx_rx_steps_per_freq *
                                                ftm_tdscdma_tx_rx_num_freqs +
                                              sizeof(uint16)*ftm_tdscdma_tx_rx_num_freqs +
                                              8*ftm_tdscdma_tx_rx_steps_per_freq +
                                              4);

  if (log_ptr != NULL)
  {
    /* Copy the request parameters and the results into the log packet */
    data_ptr = (uint8*) (log_ptr + sizeof(ftm_log_gen_type));//to get rid of log_ftm_gen_type

    if (data_ptr != NULL)
    {
      if( (ftm_tdscdma_tx_rx_num_freqs <= FTM_TX_RX_FREQ_CAL_MAX_FREQS)
          && (ftm_tdscdma_tx_rx_steps_per_freq <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ))
      {

      /* rx_chain */
      if (ftm_tdscdma_tx_rx_chain > 255)
       {
          FTM_MSG_ERROR("ftm_tdscdma_tx_rx_chain is greater than 255", 0, 0, 0);
          return;
       }
     // else
       {
       *data_ptr = (uint8)ftm_tdscdma_tx_rx_chain;
       /*lint -save -e415 -e416 typecast data from uin8 to uint16 data[] pointer will not be out of bounce */
       data_ptr+=sizeof(uint16);
       }

      /* num_freqs */
      *data_ptr=ftm_tdscdma_tx_rx_num_freqs;
      data_ptr+=sizeof(uint8);

      /* num_steps */
      *data_ptr=ftm_tdscdma_tx_rx_steps_per_freq;
      data_ptr+=sizeof(uint8);
      /*lint -restore */

      /* channel_list */
      memscpy((void*)data_ptr,
             sizeof(uint16) * ftm_tdscdma_tx_rx_num_freqs,
             &ftm_tdscdma_tx_rx_freq_cal_channels[0],
             sizeof(uint16) * ftm_tdscdma_tx_rx_num_freqs);
      data_ptr+=sizeof(uint16) * ftm_tdscdma_tx_rx_num_freqs;

      /* tx_pdm_list */
      memscpy((void*)data_ptr,
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq,
             &ftm_tdscdma_tx_rx_freq_cal_pdm_list[0],
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq);
      data_ptr+=sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq;

      /* pri_expected_agc_val_list */
      memscpy((void*)data_ptr,
             sizeof(int16) * ftm_tdscdma_tx_rx_steps_per_freq,
             &ftm_tdscdma_tx_rx_freq_cal_exp_agc[0],
             sizeof(int16) * ftm_tdscdma_tx_rx_steps_per_freq);
      data_ptr+=sizeof(int16) * ftm_tdscdma_tx_rx_steps_per_freq;

      /* primary_rx_results */
      memscpy((void*)data_ptr,
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq,
             &ftm_tdscdma_tx_rx_freq_cal_rx_result[0],
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs);
      data_ptr+=sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs;

      /* pa_range_list */
      memscpy((void*)data_ptr,
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq,
               &ftm_tdscdma_tx_rx_freq_cal_pa_ranges[0],
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq);
        data_ptr+= sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq;

      /* read_hdet_list */
      memscpy((void*)data_ptr,
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq,
               &ftm_tdscdma_tx_rx_freq_cal_hdet_read_list[0],
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq);
        data_ptr+= sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq;

      /* hdet_results */
      memscpy((void*)data_ptr,
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs,
             &ftm_tdscdma_tx_rx_freq_cal_tx_result[0],
             sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs);
      data_ptr+=sizeof(uint16) * ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs;

      /* rx_action_list */
      memscpy((void*)data_ptr,
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq,
               &ftm_tdscdma_tx_rx_freq_cal_rx_actions[0],
               sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq);
        data_ptr+= sizeof(uint8) * ftm_tdscdma_tx_rx_steps_per_freq;

      /* lpm list - zero it out since not supported on umts targets */
      for (i=0;i<ftm_tdscdma_tx_rx_steps_per_freq;i++)
      {
        *data_ptr++ = 0;
      }

      }
      else
      {
        FTM_MSG_ERROR("FTM num of Rx-Tx freq/Steps are greater than allowed", 0, 0, 0);
      }
    }
    else
    {
      FTM_MSG_ERROR("Could not access data memory - tx-rx vs freq. log packet will be incorrect!", 0, 0, 0);
    }

    /* send log */
    log_commit( (ftm_log_type*)log_ptr );
  }
  else
  {
    FTM_MSG_ERROR("Could not allocate memory for freq. cal log packet", 0, 0, 0);
  }

  FTM_MSG_HIGH("ftm_tdscdma_tx_rx_freq_cal_log_results:done",0,0,0);
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_TX_RX_FREQ_CAL_ISR

DESCRIPTION
  This ISR is called for each step of the TX/RX frequency calibration sweep.
  It performs LNA offset calibration, reads HDET, and changes
  channels as required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_tx_rx_freq_cal_isr( void )
{
  uint8 num_hdet_reads, hdet_count = 0;
  uint16 temp_hdet = 0;
  uint8 cur_seg_within_freq = ftm_tdscdma_tx_rx_freq_cal_cur_seg % ftm_tdscdma_tx_rx_steps_per_freq;

  FTM_MSG_HIGH("ftm_tdscdma_tx_rx_freq_cal_isr:enter seq =%d",ftm_tdscdma_tx_rx_freq_cal_cur_seg,0,0);

  /* check if we have not yet reached the end of the sweep */
  if (ftm_tdscdma_tx_rx_freq_cal_cur_seg < ftm_tdscdma_tx_rx_steps_per_freq * ftm_tdscdma_tx_rx_num_freqs)
  {
    /* sweep not finished, check if end of current frequency */
    if (cur_seg_within_freq == ftm_tdscdma_tx_rx_steps_per_freq-1)
    {
      /* Turn off Tx */
      ftm_tdscdma_set_tx(FALSE);
      /* Turn off Rx */
      ftm_tdscdma_set_rx(FALSE);
      /* last step of this frequency, tune to new channel */
      ftm_tdscdma_tune_to_chan(FTM_RECEIVE_CHAIN_0, ftm_tdscdma_tx_rx_freq_cal_channels[(ftm_tdscdma_tx_rx_freq_cal_cur_seg /
                                                    ftm_tdscdma_tx_rx_steps_per_freq + 1) %
                                                    ftm_tdscdma_tx_rx_num_freqs]);
      /*no need to set min pdm because tds tear down tx after tune to chan and it requires explicitly turn on*/
    }
    else
    {
      /* still within current frequency, perform tx calibration */
      if (ftm_tdscdma_tx_rx_freq_cal_pa_ranges[cur_seg_within_freq] !=
          FTM_TX_RX_SWEEP_NO_ACTION)
      {
        /*Turn on Tx*/
        ftm_tdscdma_set_tx(TRUE);

        /* set tx pa range and pdm */
        ftm_tdscdma_set_pa_range(ftm_tdscdma_tx_rx_freq_cal_pa_ranges[cur_seg_within_freq]);

        ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ,
                          ftm_tdscdma_tx_rx_freq_cal_pdm_list[cur_seg_within_freq]);

        /* read hdet if desired */
        if (ftm_tdscdma_tx_rx_freq_cal_hdet_read_list[cur_seg_within_freq] != 0)
        {
          for ( num_hdet_reads = 0;
                num_hdet_reads < FTM_TDSCDMA_FREQ_CAL_SWEEP_NUM_HDET_AVG;
                num_hdet_reads++)
          {
            hdet_count = 0;
            do
            {
              if (is_wtr_hdet)
              {
                rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                                 RFCOM_TDSCDMA_MODE,
                                                 ftm_tdscdma_current_mode,
                                                 RFDEVICE_GET_HDET_VALUE,
                                                 &temp_hdet);
              }

              else
              {
                (void)rfdevice_hdet_tdscdma_tx_hdet_read(ftm_tdscdma_current_tx_device,
                                                         ftm_tdscdma_current_mode,
                                                         FALSE,
                                                         &temp_hdet,
                                                         NULL,
                                                         RFDEVICE_EXECUTE_IMMEDIATE);
              }

              hdet_count++;
            } while (temp_hdet == 0xFFFF && hdet_count <= 3);
            ftm_tdscdma_tx_rx_freq_cal_tx_result[ftm_tdscdma_tx_rx_freq_cal_cur_seg] += temp_hdet;
          }
          ftm_tdscdma_tx_rx_freq_cal_tx_result[ftm_tdscdma_tx_rx_freq_cal_cur_seg] /=
            FTM_TDSCDMA_FREQ_CAL_SWEEP_NUM_HDET_AVG;
        } /* end hdet read */
        else
        {
          ftm_tdscdma_tx_rx_freq_cal_tx_result[ftm_tdscdma_tx_rx_freq_cal_cur_seg]=0;
        }
      } /* end tx section */
      /*Unlike other technologies, TDS can't do Rx/Tx at the same time, thus this is an "else if" instead of "if"*/
      /* perform rx action */
      else if (ftm_tdscdma_tx_rx_freq_cal_rx_actions[cur_seg_within_freq] != FTM_RX_NO_ACTION)
      {
        /*Turn on Rx*/
        ftm_tdscdma_set_rx(TRUE);

        if(ftm_tdscdma_tx_rx_chain == 0)
        {
          ftm_tdscdma_tx_rx_freq_cal_rx_result[ftm_tdscdma_tx_rx_freq_cal_cur_seg] =
            ftm_tdscdma_get_lna_offset(FTM_RECEIVE_CHAIN_0,
                                     ftm_tdscdma_tx_rx_freq_cal_rx_actions[cur_seg_within_freq],
                                     (uint16)ftm_tdscdma_tx_rx_freq_cal_exp_agc[cur_seg_within_freq],ftm_tdscdma_rx_lin_state);
        }
#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
        else if(ftm_tdscdma_tx_rx_chain == 1)
        {
          ftm_tdscdma_tx_rx_freq_cal_rx_result[ftm_tdscdma_tx_rx_freq_cal_cur_seg] =
            ftm_tdscdma_get_lna_offset(FTM_RECEIVE_CHAIN_1,
                                     ftm_tdscdma_tx_rx_freq_cal_rx_actions[cur_seg_within_freq],
                                     (uint16)ftm_tdscdma_tx_rx_freq_cal_exp_agc[cur_seg_within_freq],ftm_tdscdma_rx_lin_state);
        }
#endif /*FTM_HAS_TDSCDMA_RX_DIVERSITY*/
        else
        {
          FTM_MSG_ERROR("ftm_tdscdma_tx_rx_freq_cal_isr:Unsupported Rx Chain %d",ftm_tdscdma_tx_rx_chain,0,0);
        }
      }    /*end if not ftm rx no action*/
    } /* end if not last step of this freq. */
  } /* end if not at end of sweep */
  else
  {
    /* full frequency sweep is done */
    /* turn off timer interrupts */
    ftm_common_msm_uninstall_timer();

    /* send log packet with results */
    ftm_tdscdma_tx_rx_freq_cal_log_results();

    /* Turn off Tx */
    ftm_tdscdma_set_tx(FALSE);

    /* Turn off Rx */
    ftm_tdscdma_set_rx(FALSE);

  }

  FTM_MSG_HIGH("ftm_tdscdma_tx_rx_freq_cal_isr:done seq =%d",ftm_tdscdma_tx_rx_freq_cal_cur_seg,0,0);

  /* increment segment counter for next isr iteration */
  ftm_tdscdma_tx_rx_freq_cal_cur_seg++;
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_DO_TX_RX_FREQ_CAL_SWEEP

DESCRIPTION
  This function configures and starts the TX/RX frequency calibration sweep.

DEPENDENCIES
  None

RETURN VALUE
  Error code.

SIDE EFFECTS
  None

===========================================================================*/
uint16 ftm_tdscdma_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs,
                                   uint8  num_steps_per_freq,
                                   uint16 step_length,
                                   uint16 *freq_list_ptr,
                                   uint16 *pdm_list_ptr,
                                   int16  *pri_exp_agc_val_ptr,
                                   uint8  *pa_range_list_ptr,
                                   uint8  *read_hdet_ptr,
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr,
                                   uint8   rx_lin_state)

{
  uint8 i, j=0;
  ftm_tx_rx_error_code_type result = FTM_TX_RX_SUCCESS;

  FTM_MSG_HIGH("ftm_tdscdma_do_tx_rx_freq_cal_sweep:enter",0,0,0);

  ftm_tdscdma_tx_rx_freq_cal_cur_seg = 0;


  /* lock interrupts as timer may be running */


  if((rx_chain == FTM_RECEIVE_CHAIN_0) || (rx_chain == FTM_RECEIVE_CHAIN_1))
  {
    ftm_tdscdma_tx_rx_chain = rx_chain;
  }

  if((rx_chain == FTM_RX_LIN_STATE_HIGH) || (rx_chain == FTM_RX_LIN_STATE_LOW))
  {
    ftm_tdscdma_rx_lin_state = rx_lin_state;
  }

  /* limit num_steps_per_freq to <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ */
  if (num_steps_per_freq <= FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ)
  {
    ftm_tdscdma_tx_rx_steps_per_freq = num_steps_per_freq;
  }
  else
  {
    ftm_tdscdma_tx_rx_steps_per_freq = FTM_TX_RX_SWEEP_MAX_STEPS_PER_FREQ;
    result |= FTM_TX_RX_STEPS_INVALID; /*lint !e655*/
  }

  /* limit num_freqs to <= FTM_TX_RX_FREQ_CAL_MAX_FREQS */
  if (num_freqs <= FTM_TX_RX_FREQ_CAL_MAX_FREQS)
  {
    ftm_tdscdma_tx_rx_num_freqs = num_freqs;
  }
  else
  {
    ftm_tdscdma_tx_rx_num_freqs = FTM_TX_RX_FREQ_CAL_MAX_FREQS;
    result |= FTM_TX_RX_FREQS_INVALID;  /*lint !e655*/
  }

  /* limit step_length to >= FTM_TX_RX_MIN_STEP_LENGTH (20ms) */
  if (step_length < FTM_TX_RX_MIN_STEP_LENGTH)
  {
    step_length = FTM_TX_RX_MIN_STEP_LENGTH;
    result |= FTM_TX_RX_STEP_LEN_INVALID; /*lint !e655*/
  }

  /* copy input data to local arrays, as packet memory may be reassigned */
  /* before sweep completion */
  for (i=0; i<num_freqs; i++)
  {
    ftm_tdscdma_tx_rx_freq_cal_channels[i] = freq_list_ptr[i];
    for(j=0; j<ftm_tdscdma_tx_rx_steps_per_freq; j++)
    {
      ftm_tdscdma_tx_rx_freq_cal_tx_result[ i*ftm_tdscdma_tx_rx_steps_per_freq+j] = 0;
      ftm_tdscdma_tx_rx_freq_cal_rx_result[ i*ftm_tdscdma_tx_rx_steps_per_freq+j] = 0;
    }
  }

  for (i=0; i<ftm_tdscdma_tx_rx_steps_per_freq; i++)
  {
    ftm_tdscdma_tx_rx_freq_cal_pa_ranges[i] = pa_range_list_ptr[i];
    ftm_tdscdma_tx_rx_freq_cal_pdm_list[i] = (uint16)(pdm_list_ptr[i]);
    ftm_tdscdma_tx_rx_freq_cal_hdet_read_list[i] = read_hdet_ptr[i];
    ftm_tdscdma_tx_rx_freq_cal_rx_actions[i] = rx_action_ptr[i];
    ftm_tdscdma_tx_rx_freq_cal_exp_agc[i] = (int16)(pri_exp_agc_val_ptr[i]);
  }

  /* create trigger */
  ftm_tdscdma_tune_to_chan(FTM_RECEIVE_CHAIN_0, ftm_tdscdma_tx_rx_freq_cal_channels[0]);

  /* Move the set PDM to the start to avoid any false trigger */
  ftm_tdscdma_set_pdm(FTM_PDM_TX_AGC_ADJ, FTM_UPLINK_SWEEP_CAL_MIN);
  ftm_tdscdma_set_tx(TRUE);
  ftm_tdscdma_set_pa_range(ftm_tdscdma_tx_rx_freq_cal_pa_ranges[0]);

  DALSYS_BusyWait(100);

  ftm_common_msm_install_timer ((uint32)(step_length/32), ftm_tdscdma_tx_rx_freq_cal_isr);

  FTM_MSG_HIGH("ftm_tdscdma_do_tx_rx_freq_cal_sweep:done result=%d",result,0,0);

  return (uint16)result;
}

/*===========================================================================

FUNCTION FTM_SET_SECONDARY_CHAIN

DESCRIPTION
  This function is used during calibration to activate the secondary chain.
  Enable and disable secondary chain

DEPENDENCIES
  Should be called after ftm_tdscdma_set_mode function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_set_secondary_chain( boolean on_off )
{
//#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY

//  uint8 rxd_nv = 0;
//  boolean agc_status = FALSE;
  #ifdef FEATURE_MDSP_IOCTL
  static uint16 ant1_samp_clk_src_div;
  #endif

  //hyang todo: tds version once available rxd_overide_nv_settings( rxd_nv );
//  FTM_MSG_HIGH("RxD nv value set to 0",0,0,0);

if ((on_off == TRUE) && (ftm_tdscdma_rx_div_enable == FALSE))
  {
    #ifdef HW_HAS_SET_SYSTEM_MODE_CHAIN_PARAMETER
    /* Ensure HW configured correctly for TDSCDMA on secondary antenna. */
    //hyang todo: hw_set_system_mode() doesn't have tdscdma client. need to revisit once diversity is available
    (void)hw_set_system_mode(HW_SYSTEM_MODE_TDSCDMA, HW_MODEM_CHAIN_1);
    #endif /*HW_HAS_SET_SYSTEM_MODE_CHAIN_PARAMETER*/

 //   ftm_tdscdma_allocate_rxlm(FTM_RECEIVE_CHAIN_1);

    /* Set RXLM to Cx2 if we are in RF Mode or set RXLM to Cx1 in calibration mode */
    if (rfm_get_calibration_state())
    {
      rf_tdscdma_mc_enter_mode(ftm_tdscdma_current_pri_device, 
                               ftm_tdscdma_current_sec_device,
                               ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx, 
                               RFA_TDSCDMA_RXLM_MODE_ACQ,
                               FALSE // is_dsds
                               );
      ftm_tdscdma_rx_div_enable = TRUE;
      ftm_tdscdma_div_status = TRUE;
    }
    else
    {
      if (ftm_tdscdma_burst_rx_state == FTM_TDSCDMA_BURST_RX_ON)
      {
        /* Switch RF mode RX to Rx0+Rx1 */
        FTM_MSG_HIGH("Switching RF mode from Rx0 to Rx0+Rx1!!!",0,0,0);
        ftm_tdscdma_rx_div_enable = TRUE;
        ftm_tdscdma_div_status = FALSE;
        (void)ftm_tdscdma_burst_rx(ftm_tdscdma_burst_rx_params_shadow.exp_rx_lvl, TRUE);
        ftm_tdscdma_div_status = TRUE;
      }
      else
      {
        rf_tdscdma_mc_enter_mode(ftm_tdscdma_current_pri_device, 
                                 ftm_tdscdma_current_sec_device,
                                 ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx, 
                                 RFA_TDSCDMA_RXLM_MODE_TRK,
                                 FALSE // is_dsds
                                 );
        ftm_tdscdma_rx_div_enable = TRUE;
        ftm_tdscdma_div_status = TRUE;
      }
    }

    //ftm_tdscdma_rx_div_enable = TRUE;
    //ftm_tdscdma_div_status = TRUE;
/*    if (agc_status)
    {
      FTM_MSG_HIGH("Primary and secondary chain active",0,0,0);
      on_off = TRUE;
    }
    else
    {
      FTM_MSG_ERROR("MDSP AGC activate command failed",0,0,0);
      on_off = FALSE;
    }
*/
//hyang todo    rf_tdscdma_mdsp_send_rxagc_cmd(RFTDSCDMA_CORE_AGC_CMD_NOOP,
//                                RFTDSCDMA_CORE_AGC_CMD_NOOP,
//                                RFTDSCDMA_CORE_AGC_CMD_ENABLE,
//                                RFTDSCDMA_CORE_AGC_CMD_NOOP
//                                #ifdef FEATURE_RXLM
//                                ,ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx
//                                #endif
//                                );

  }
  else if((on_off == FALSE) && (ftm_tdscdma_rx_div_enable == TRUE))
  {
   // rf_tdscdma_mc_sleep(RFM_DEVICE_0, RFM_DEVICE_1);

    if (ftm_tdscdma_burst_rx_state == FTM_TDSCDMA_BURST_RX_ON)
    {
      /* Switch RF mode RX to Rx0 only */
      FTM_MSG_HIGH("Switching RF mode from Rx0+Rx1 to Rx0!!!",0,0,0);
      ftm_tdscdma_rx_div_enable = FALSE;
      ftm_tdscdma_div_status = TRUE;
      (void)ftm_tdscdma_burst_rx(ftm_tdscdma_burst_rx_params_shadow.exp_rx_lvl, TRUE);
      ftm_tdscdma_div_status = FALSE;
    }
    else
    {
      ftm_tdscdma_rx_div_enable = FALSE;
      ftm_tdscdma_div_status = FALSE;
    }
/*
    if (agc_status)
    {
      FTM_MSG_HIGH("Only primary chain is active",0,0,0);
      on_off = TRUE;
    }
    else
    {
      FTM_MSG_ERROR("MDSP AGC deactivate command failed",0,0,0);
      on_off = FALSE;
    }
   */
  }
//#else
  //FTM_MSG_ERROR("ftm_tdscdma_set_secondary_chain(): not supported",0,0,0);
//#endif /* FTM_HAS_TDSCDMA_RX_DIVERSITY */

}

/*===========================================================================

FUNCTION FTM_SECOND_CHAIN_TEST_CALL

DESCRIPTION
  This function is used in non-signalling calls to test a particular antenna.
  Enable will set the RxD controller NV to 6 to use ANT1 for primary reception
  Disable will set the RxD controller NV to 0 to use ANT0 for primary reception

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_second_chain_test_call( uint8 second_chain )
{
#if 0
  switch (second_chain)
  {
  case FTM_PRIMARY_CHAIN_ONLY:
    tdsrxdiv_overide_nv_settings( 0 );
    FTM_MSG_HIGH("RxD nv value set to 0 for primary only",0,0,0);
    break;

  case FTM_SECONDARY_CHAIN_ONLY:
    tdsrxdiv_overide_nv_settings( 6 );
    FTM_MSG_HIGH("RxD nv value set to 6 to activate secondary chain",0,0,0);
    break;

  case FTM_PRIMARY_AND_SECONDARY:
    tdsrxdiv_overide_nv_settings( 2 );
    FTM_MSG_HIGH("RxD nv value set to 2 to activate primary and secondary chain",0,0,0);
    break;

  default:
    FTM_MSG_ERROR("Wrong parameter",0,0,0);
    break;
  }
#endif
//#else
//    FTM_MSG_ERROR("ftm_tdscdma_second_chain_test_call(): not supported",0,0,0);
//#endif /* FTM_HAS_TDSCDMA_RX_DIVERSITY */
}


/*===========================================================================

FUNCTION FTM_DO_DC_CAL

DESCRIPTION
  This function performs tdscdma DC calibration.  The calibrated value will be
  used during TDSCDMA inter-freq/inter-band.

DEPENDENCIES
  Made sure TDSCDMA mode is set and the channel is tuned.
  NV task must be running.

RETURN VALUE
  None

SIDE EFFECTS
  Writes calibration results to NV.

===========================================================================*/
void ftm_tdscdma_do_dc_cal(void)
{
  FTM_MSG_ERROR("ftm_tdscdma_do_dc_cal() not supported yet", 0, 0, 0);
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_DO_TX_DAC_CAL

DESCRIPTION
    This function will complete the TX DAC cal on TDSCDMA.

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
boolean ftm_tdscdma_do_tx_dac_cal(void)
{
  boolean result = FALSE;

  return result;
}


/*===========================================================================

FUNCTION FTM_TDSCDMA_TOGGLE_MODES

DESCRIPTION
  This function will switch from TDSCDMA to GSM mode and back, or vice-versa.
  This triggers the updating of rf driver internal variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_toggle_modes( void )
{
  ftm_mode_id_type current_mode=0;

  current_mode = ftm_db_to_rf_mode( ftm_current_rf_mode );

  if ((ftm_current_rf_mode == FTM_DB_RF_GSM_850)  ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_900)  ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_1800) ||
      (ftm_current_rf_mode == FTM_DB_RF_GSM_1900) )
  {
    /* we are in GSM, so set to TDSCDMA and then back */
    ftm_tdscdma_set_mode(FTM_PHONE_MODE_TDSCDMA_B34);
    #ifdef FTM_HAS_GSM
    ftm_gsm_set_mode(ftm_tdscdma_current_pri_device, current_mode);
    #endif
  }
  else
  {
    /* we are in TDSCDMA, so set to GSM and then back */
    #ifdef FTM_HAS_GSM
    ftm_gsm_set_mode(ftm_tdscdma_current_pri_device, FTM_PHONE_MODE_GSM_900);
    #endif
    ftm_tdscdma_set_mode(current_mode);
  }
}

void ftm_tdscdma_set_smps_pout_table_type(tx_pout_data_type set_tbl_type)
{
   FTM_MSG_ERROR("ftm_tdscdma_set_smps_pout_table_type: command not used",0,0,0);
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_ALLOCATE_RXLM

DESCRIPTION
    This function will request rxlm buffers FTM TDSCDMA for the given chain

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_tdscdma_allocate_rxlm(ftm_receive_chain_handle_type chain)
{
  lm_tech_type tech = LM_TDSCDMA;
  lm_status_type status;

  if (!ftm_tdscdma_rxlm_buffer[chain].is_buf_allocated)
  {
    status = rxlm_allocate_buffer((rxlm_chain_type)chain, tech, &(ftm_tdscdma_rxlm_buffer[chain].buf_idx));
    if (status != LM_SUCCESS )
    {
       MSG_ERROR("RxLM allocate buffer failed, status:%d",status,0,0);
       return;
    }
    else
    {
       MSG_HIGH("RXLM buffer allocated for chain:%d, buffer index:%d",chain,ftm_tdscdma_rxlm_buffer[chain].buf_idx,0);
    }
    ftm_tdscdma_rxlm_buffer[chain].is_buf_allocated = TRUE;
  }
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_DEALLOCATE_RXLM

DESCRIPTION
    This function will deallocate rxlm buffers FTM TDSCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_tdscdma_deallocate_rxlm(void)
{
  uint8 i;
  lm_status_type status;
  for (i=0; i<2; i++)
  {
    if (ftm_tdscdma_rxlm_buffer[i].is_buf_allocated)
    {
      status = rxlm_deallocate_buffer(ftm_tdscdma_rxlm_buffer[i].buf_idx);
      if (status != LM_SUCCESS )
      {
        MSG_ERROR("RxLM allocate buffer failed, status:%d",status,0,0);
      }
      else
      {
        MSG_HIGH("RXLM buffer deallocated for chain:%d",i,0,0);
        ftm_tdscdma_rxlm_buffer[i].is_buf_allocated = FALSE;
      }
    }
  }
}
/*===========================================================================

FUNCTION FTM_TDSCDMA_ALLOCATE_TXLM

DESCRIPTION
    This function will request txlm buffers FTM TDSCDMA

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_tdscdma_allocate_txlm(void)
{
  lm_tech_type tech = LM_TDSCDMA;
  lm_status_type status;

  if (!ftm_tdscdma_txlm_buffer.is_buf_allocated)
  {
    status = txlm_allocate_buffer(TXLM_CHAIN_0, tech, &(ftm_tdscdma_txlm_buffer.buf_idx));
    if (status != LM_SUCCESS )
    {
       MSG_ERROR("TxLM allocate buffer failed, status:%d",status,0,0);
       return;
    }
    else
    {
       MSG_HIGH("TXLM buffer allocated buffer index:%d",ftm_tdscdma_txlm_buffer.buf_idx,0,0);
    }
    ftm_tdscdma_txlm_buffer.is_buf_allocated = TRUE;
  }
}

/*===========================================================================

FUNCTION FTM_TDSCDMA_DEALLOCATE_TXLM

DESCRIPTION
    This function will deallocate txlm buffers FTM TDSCDMA has requested

DEPENDENCIES

RETURN VALUE
   None.

SIDE EFFECTS


===========================================================================*/
void ftm_tdscdma_deallocate_txlm(void)
{
  lm_status_type status;
  if (ftm_tdscdma_txlm_buffer.is_buf_allocated)
  {
    status = txlm_deallocate_buffer(ftm_tdscdma_txlm_buffer.buf_idx);
    if (status != LM_SUCCESS )
    {
      MSG_ERROR("TxLM allocate buffer failed, status:%d",status,0,0);
    }
    else
    {
      MSG_HIGH("TXLM buffer deallocated for buffer index %d",ftm_tdscdma_txlm_buffer.buf_idx,0,0);
      ftm_tdscdma_txlm_buffer.is_buf_allocated = FALSE;
    }
  }
}

/*============================================================================*/
/*!
  @name Internal Mode Transition Interface - TDS-CDMA

  @brief
  This section contains interfaces to control the FTM mode transition for
  TDS-CDMA.
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter TDSCDMA Mode

  @details
  Configure the RF to TDSCDMA mode. RF must be in PARK mode before calling
  this function.

  @param device RF Device to which FTM is to be entered

  @return
  TRUE on success and FALSE on failure
*/
boolean
ftm_tdscdma_mode_enter
(
  rfm_device_enum_type device
)
{
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */

  ftm_rfmode = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is same as current mode in the system*/
  if ( ftm_rfmode == FTM_STATE_TDSCDMA)
  {
    /* do nothing */
    return TRUE;
  }

  /*if we do ns, we must exit. this function will check ns state.*/
#ifdef FTM_HAS_TDSCDMA_BER
   ftm_tdscdma_ber_cleanup();
#endif

  /* MCPM Call to properly prepare the modem clock. These calls should be
  called in proper sequence to follow the MCPM STM.
  Note: Idle mode has higher clock rate thus is preferred over Voice*/
  MCPM_Config_Modem( MCPM_TDSCDMA_START_REQ, NULL ); /* Page and Decode State */
  //MCPM_Config_Modem( MCPM_TDSCDMA_ACQ_REQ, NULL ); /* ACQ State */
  MCPM_Config_Modem( MCPM_TDSCDMA_IDLE_REQ, NULL ); /* Idle State */
  MCPM_Config_Modem( MCPM_TDSCDMA_VOICE_START_REQ, NULL );


  /* Config STMR for TFW*/
  //rf_tdscdma_msm_stmr_init();//comment out as stmr should not be changed

  /* allocate rxlm buffer for chain 0*/
  ftm_tdscdma_allocate_rxlm(FTM_RECEIVE_CHAIN_0);

  /* allocate rxlm buffer for rx diversity chain*/
  ftm_tdscdma_allocate_rxlm(FTM_RECEIVE_CHAIN_1);
  /* allocate txlm buffer */
  ftm_tdscdma_allocate_txlm();

  /* Set RXLM to Cx2 if we are in RF Mode or set RXLM to Cx1 in calibration mode */
  if (rfm_get_calibration_state())
  {
    rf_tdscdma_mc_enter_mode(ftm_tdscdma_current_pri_device, 
                             ftm_tdscdma_current_sec_device, 
                             ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                             RFA_TDSCDMA_RXLM_MODE_ACQ,
                             FALSE // is_dsds
                             );//PRx.
  }
  else
  {
    rf_tdscdma_mc_enter_mode(ftm_tdscdma_current_pri_device, 
                             ftm_tdscdma_current_sec_device, 
                             ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                             RFA_TDSCDMA_RXLM_MODE_TRK,
                             FALSE // is_dsds
                             );//PRx.
  }

  /*Config TFW to FTM mode*/
  if (rfm_get_calibration_state())
  {
    ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_FTM_CAL);
  }
  else
  {
    ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_FTM);
  }

  /* Get RF device Tx AGC limitation value, so that we don't need to get it every time set pdm is called */
  rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                   RFCOM_TDSCDMA_MODE,
                                   ftm_tdscdma_current_mode,
                                   RFDEVICE_GET_TX_AGC_LIMITS,
                                   &ftm_tdscdma_pdm_limits);

  /* Note: the flag "ftm_current_rf_mode" could be cleared to IDLE when exiting previous mode.
   * It's safe to set it back to TDSCDMA here again .
   * This flag must not be FTM_DB_RF_IDLE, otherwise the IS_FTM_IN_TEST_MODE() may get incorrect mode in FTM
   */
  switch(ftm_tdscdma_current_mode)
  {
    case RFCOM_BAND_TDSCDMA_B34:
      ftm_set_rf_db_state(FTM_DB_RF_TDSCDMA_B34);
      break;
    case RFCOM_BAND_TDSCDMA_B39:
      ftm_set_rf_db_state(FTM_DB_RF_TDSCDMA_B39);
      break;
    case RFCOM_BAND_TDSCDMA_B40:
    case RFCOM_BAND_TDSCDMA_B40_B:
      ftm_set_rf_db_state(FTM_DB_RF_TDSCDMA_B40);
      break;
    default:
      /* set to FTM_DB_RF_TDSCDMA_B34 by default */
      ftm_set_rf_db_state(FTM_DB_RF_TDSCDMA_B34);
      break;
  }

  /*Reset Multi Synth State to Disable*/
  ftm_tdscdma_set_multi_synth_state(TRUE);

  if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
  {
     FTM_MSG_HIGH("ftm_tdscdma_mode_enter:enter mode %d",ftm_tdscdma_current_mode,0,0);
  }

  return TRUE;

} /* ftm_tdscdma_enter */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Exit TDSCDMA Mode for FTM

  @details
  This function is used to clean-up procedure for TDSCDMA.

  @param device RF Device for which FTM will exit TDSCDMA

  @return
  TRUE on success and FALSE on failure
*/
boolean
ftm_tdscdma_mode_exit
(
  rfm_device_enum_type device
)
{
  ftm_rfstate_enum_type ftm_rfmode; /* Current FTM RF Mode */

  ftm_rfmode = ftm_get_current_state(device);

  /* Do nothing, if the requested Mode is not TDSCDMA in the system*/
  if ( ftm_rfmode != FTM_STATE_TDSCDMA)
  {
    /* do nothing */
    return TRUE;
  }

  /*make sure Tx is turned off */
  ftm_tdscdma_set_tx(FALSE);

  /*make sure Rx is turned off*/
  ftm_tdscdma_set_rx(FALSE);

  /*disable tx. */
  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
  {
     rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
  }

  /*Set Multi Synth State to INVALID*/
  ftm_tdscdma_set_multi_synth_state(FALSE);

  /* Clean up FTM RF data structures */
  ftm_tdscdma_rfmode_cleanup();

  /*unload fw*/
  ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_STANDBY);

  /*exit mode. */
  rf_tdscdma_mc_exit_mode(ftm_tdscdma_current_pri_device, 
                          ftm_tdscdma_current_sec_device, 
                          FALSE // wait_fw_release_resource 
                          );

  /* deallocate all txlm buffers */
  ftm_tdscdma_deallocate_txlm();

  /* deallocate all rxlm buffers */
  ftm_tdscdma_deallocate_rxlm();

  MCPM_Config_Modem( MCPM_TDSCDMA_IDLE_REQ, NULL ); /* Idle State */
  MCPM_Config_Modem( MCPM_TDSCDMA_STOP_REQ, NULL ); /* Power Down State */

  ftm_current_rf_mode = FTM_DB_RF_IDLE;
  ftm_tdscdma_current_mode = RFCOM_BAND_TDSCDMA_INVALID;
  ftm_tdscdma_current_mode_id = FTM_PHONE_MODE_MAX;
  
  ftm_tdscdma_current_pri_device   = RFM_INVALID_DEVICE;
  ftm_tdscdma_current_sec_device   = RFM_INVALID_DEVICE;
  ftm_tdscdma_current_tx_device    = RFM_INVALID_DEVICE;
  ftm_tdscdma_current_pri_alt_path = 0;
  ftm_tdscdma_current_sec_alt_path = 0;

  // Free up the allocated memory.
  ftm_tdscdma_free_memory();

  return TRUE;

} /* ftm_tdscdma_exit */

/*============================================================================*/
/*!
  @name Firmware App Control

  @brief
  This section contains functions to control FW App.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure TDS firmware state

  @details
  This function will enable FW App while running under FTM task context by
  calling RF-MDSP Interface.

  @param fw_state
  The state in which to configure firmware
*/
void
ftm_tdscdma_config_fw_state
(
  rf_tdscdma_mdsp_tfw_state_t fw_state
)
{
  /*check if already in right state.*/
  if(ftm_tdscdma_current_fw_state == fw_state)
  {
    return;
  }

  //since we are changing fw state, invalidate fw cal state.
  ftm_tdscdma_config_cal_state(FTM_TDSCDMA_CAL_STATE_INACTIVE);

  if (fw_state != RF_TDSCDMA_MDSP_TFW_STATE_STANDBY)
  {
    /* Enable FW only if comming from disabled state */
    if( ftm_tdscdma_current_fw_state == RF_TDSCDMA_MDSP_TFW_STATE_STANDBY)
    {
      /* Enable FW Application */
       fws_app_enable(FW_APP_RFCMD);
       fws_app_enable(FW_APP_TDSCDMA);
       FTM_MSG_HIGH("ftm_tdscdma_config_fw_state: FW enabled.",0,0,0);
    }

    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_STATE_CFG_MSG,fw_state);
#ifndef FEATURE_JOLOKIA_MODEM   
    // Send Tx modem chain information to FW
    if(ftm_tdscdma_current_tx_device == RFM_DEVICE_0)
    {
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_TXLM_CFG_MSG, 0xf0);
    }
    else
    {
      FTM_MSG_HIGH("ftm_tdscdma_config_fw_state: Skipped sending TXLM CFG MSG to FW when Tx on %d", ftm_tdscdma_current_tx_device, 0, 0);
    }
#endif
    
  }
  else
  {
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_STATE_CFG_MSG,RF_TDSCDMA_MDSP_TFW_STATE_STANDBY);

    /*Disable FW App*/
    fws_app_disable(FW_APP_RFCMD);
    fws_app_disable(FW_APP_TDSCDMA);


    FTM_MSG_HIGH("ftm_tdscdma_config_fw_state: FW disabled.",0,0,0);
  }

  //cache state
  ftm_tdscdma_current_fw_state = fw_state;


  FTM_MSG_HIGH("ftm_tdscdma_config_fw_state: FW state=%d.",fw_state,0,0);

} /* ftm_tdscdma_config_fw_state */

/*============================================================================*/
/*!
  @name Firmware calibration state

  @brief
  This section contains functions to control FW App state.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Configure TDS firmware calibration state

  @details
  This function will enable FW App while running under FTM task context by
  calling RF-MDSP Interface.

  @param fw_state
  The calibration state in which to configure firmware
*/
LOCAL void
ftm_tdscdma_config_cal_state
(
  ftm_tdscdma_cal_state_type cal_state
)
{
#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE // This feature is by default NOT defined
  boolean asm_status = TRUE, pa_status = TRUE;
  rfc_tdscdma_grfc_notime_control_type tds_grfc_notime_control_data;
#else
  rflm_tds_schedule_rf_internal_event_in_t in_data;
  rflm_err_t exec_result;
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE  */


  /*check if already in right state. must have this check in case cal_state is inactive.*/
  if (ftm_tdscdma_current_cal_state == cal_state) 
  {
    //already in right state. Nothing to do but directly return
    return;
  }

  /*ensure fw is in cal state*/
  if( (ftm_tdscdma_current_fw_state != RF_TDSCDMA_MDSP_TFW_STATE_FTM_CAL) &&
      (ftm_tdscdma_current_fw_state != RF_TDSCDMA_MDSP_TFW_STATE_FTM)     &&
      (ftm_tdscdma_current_fw_state != RF_TDSCDMA_MDSP_TFW_STATE_FTM_TRK)  )
  {
    FTM_MSG_ERROR("FW state %d is not FTM.", ftm_tdscdma_current_fw_state, 0, 0);
    return;
  }

  if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_RX)//Rx
  {
    //send mdsp disable msg.
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_RX_MSG, FALSE);

    //disable rx diversity
    //ftm_tdscdma_rx_div_enable = FALSE; //no need to disable flag

    //Shut off Rx
    if (rf_tdscdma_use_on_rumi == FALSE)
    {
      in_data.effectTime = 0xFFFFFFFF; // Immediate execution
      in_data.scriptType = RFLM_TDS_RX_OFF_SCRIPT;
      in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
      in_data.scriptInfo.rxOffScript.callInCalMode = TRUE;    
      in_data.scriptInfo.rxOffScript.bufId = 0; // Hard-coded bufID=0 for CAL mode
      if (!ftm_tdscdma_div_status)
      {
      	in_data.scriptInfo.rxOffScript.divEn = FALSE;
      }
      else
      {
        in_data.scriptInfo.rxOffScript.divEn = TRUE;
      }
      
      exec_result = rflm_tds_schedule_rf_internal_event(&in_data);
       
      if (exec_result == RFLM_ERR_NONE)
      {   
        MSG_5(MSG_SSID_RF, MSG_LVL_MED, 
              "RxOff script executed for device: %d, Band: %d, Chan: %d, isCalMode: %d, rxdEn: %d", 
              ftm_tdscdma_current_pri_device,           
              ftm_tdscdma_current_mode,
              ftm_tdscdma_current_chan,
              in_data.scriptInfo.rxOffScript.callInCalMode,
              in_data.scriptInfo.rxOffScript.divEn);
      }
      else
      {
        MSG_6(MSG_SSID_RF, MSG_LVL_ERROR, 
              "RxOff script executed ERROR (code %d) for device: %d, Band: %d, Chan: %d, isCalMode: %d, rxdEn: %d", 
              exec_result,
              ftm_tdscdma_current_pri_device,           
              ftm_tdscdma_current_mode,
              ftm_tdscdma_current_chan,
              in_data.scriptInfo.rxOffScript.callInCalMode,
              in_data.scriptInfo.rxOffScript.divEn);
      }
    }

    if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
    {
       FTM_MSG_HIGH("Rx Disable to mdsp. cal_state=%d",ftm_tdscdma_current_cal_state,0,0);
    }

  }
  else if (ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_TX)//Tx
  {
    /* Do Rx disable before enabling DRx path during the Tx self Cal   */
    if ( ftm_tdscdma_current_tx_cal_mode == FTM_TDSCDMA_TX_CAL_SELF
          && cal_state == FTM_TDSCDMA_CAL_STATE_RX )
    {
      if ( ftm_tdscdma_self_cal_drx_enabled )
      {
        //send mdsp rx disable msg.
        rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_RX_MSG, FALSE);
        ftm_tdscdma_self_cal_drx_enabled = FALSE;

        if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
        {
          FTM_MSG_HIGH("Rx Disable to mdsp. cal_state = %d, drx_enabled = %d",
                       ftm_tdscdma_current_cal_state,
                       ftm_tdscdma_self_cal_drx_enabled,
                       0);
        }
      } /* end of if( ftm_tdscdma_self_cal_drx_enabled )*/
    }/* end of Self Cal Check if (( ftm_tdscdma_current_tx_cal_mode == FTM_TDSCDMA_TX_CAL_SELF ...) */
    else
    {
      //send mdsp tx disable msg.
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_TX_MSG, FALSE);

#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE // This feature is by default NOT defined
      //turn off WTR Tx
      rf_tdscdma_msm_set_tx_on(ftm_tdscdma_current_tx_device,
                               ftm_tdscdma_current_mode,
                               FALSE);

      (void)rfdevice_tdscdma_pa_on_off(ftm_tdscdma_current_tx_device,
                                       ftm_tdscdma_current_mode,
                                       FALSE,
                                       NULL,
                                       RFDEVICE_EXECUTE_IMMEDIATE,
                                       RFCOM_INVALID_TIMING_OFFSET);
#else /* Below is by default enabled to use FED execute TxOFF script */ 
      /* Call FED to schedule TxOff script */
      if (rf_tdscdma_use_on_rumi == FALSE)
      {
        in_data.effectTime = 0xFFFFFFFF; // Immediate execution
        in_data.scriptType = RFLM_TDS_TX_OFF_SCRIPT;
        in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
        in_data.scriptInfo.txOffScript.callInCalMode = TRUE;    
        in_data.scriptInfo.txOffScript.bufId = 0; // Hard-coded bufID=0 for CAL mode
         
        exec_result = rflm_tds_schedule_rf_internal_event(&in_data);
         
        if (exec_result == RFLM_ERR_NONE)
        {   
          MSG_4(MSG_SSID_RF, MSG_LVL_MED, 
                "TxOff script executed for device: %d, Band: %d, Chan: %d, isCalMode: %d", 
                ftm_tdscdma_current_tx_device,           
                ftm_tdscdma_current_mode,
                ftm_tdscdma_current_chan,
                in_data.scriptInfo.txOffScript.callInCalMode);
        }
        else
        {
          MSG_5(MSG_SSID_RF, MSG_LVL_ERROR, 
                "TxOff script executed ERROR (code %d) for device: %d, Band: %d, Chan: %d, isCalMode: %d", 
                exec_result,
                ftm_tdscdma_current_tx_device,           
                ftm_tdscdma_current_mode,
                ftm_tdscdma_current_chan,
                in_data.scriptInfo.txOffScript.callInCalMode);
        }
      }
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */

      if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
      {
          FTM_MSG_HIGH("Tx Disable: cal_state=%d, tx_cal=%d.",
                       cal_state,ftm_tdscdma_current_tx_cal_mode,0);
      }
    }/* end of else part of Self Cal Check */
  }

  if (cal_state == FTM_TDSCDMA_CAL_STATE_RX)//Rx
  {
    /* ********************************
    **         Configure Rx
    ** *******************************/

    if ((ftm_tdscdma_current_cal_state == FTM_TDSCDMA_CAL_STATE_TX) && 
        (ftm_tdscdma_current_tx_cal_mode == FTM_TDSCDMA_TX_CAL_SELF)  ) /* self-cal scenario */
    {
      // only cnfig DRx for the Tx Self cal purpose
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG,
                                    (ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);

      ftm_tdscdma_self_cal_drx_enabled = TRUE;

      FTM_MSG_HIGH("DRX RXLM enabled: cal_state=%d, tx_cal=%d.",
                    cal_state,ftm_tdscdma_current_tx_cal_mode,0);
    }
    else // non self-cal condition ( normal Rx Cal)
    {
#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE // This feature by default is NOT defined      
      /* Primary Chain ASM config */
      if (rfdevice_tdscdma_use_asm(ftm_tdscdma_current_pri_device, ftm_tdscdma_current_mode))
      {
        // reset status
        asm_status = TRUE;

        asm_status &= rfdevice_tdscdma_asm_enable_rx(ftm_tdscdma_current_pri_device,
                                                     ftm_tdscdma_current_mode,
                                                     NULL,
                                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                                     RFCOM_INVALID_TIMING_OFFSET,
                                                     TRUE,
                                                     NULL,
                                                     TDS_ASM_INCLUDE_ALL);
        if (!asm_status)
        {
          FTM_MSG_ERROR( "ASM configuration ERROR for Device %d on Band %d for PRx", ftm_tdscdma_current_pri_device, ftm_tdscdma_current_mode, 0 );
        }
      }
      else
      {
        // Use GRFC control Rx
        FTM_MSG_MED( "NO ASM presented for Device %d on Band %d for PRx", ftm_tdscdma_current_pri_device, ftm_tdscdma_current_mode, 0 ); //temp add for debug
      }

      /* Secondary Chain ASM config */
      if (rfdevice_tdscdma_use_asm(ftm_tdscdma_current_sec_device, ftm_tdscdma_current_mode))
      {
        // reset status
        asm_status = TRUE;

        asm_status &= rfdevice_tdscdma_asm_enable_rx(ftm_tdscdma_current_sec_device,
                                                     ftm_tdscdma_current_mode,
                                                     NULL,
                                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                                     RFCOM_INVALID_TIMING_OFFSET,
                                                     TRUE,
                                                     NULL,
                                                     TDS_ASM_INCLUDE_ALL);

        if (!asm_status)
        {
          FTM_MSG_ERROR( "ASM configuration ERROR for Device %d on Band %d for DRx", ftm_tdscdma_current_sec_device, ftm_tdscdma_current_mode, 0 );
        }
      }
      else
      {
        // Use GRFC control Rx
        FTM_MSG_MED( "NO ASM presented for Device %d on Band %d for DRx", ftm_tdscdma_current_sec_device, ftm_tdscdma_current_mode, 0 ); //temp add for debug
      }

      // This will configure all Rx GRFCs (for those are configured by ASM they are NOT included below)
      tds_grfc_notime_control_data.band = ftm_tdscdma_current_mode;
      tds_grfc_notime_control_data.sig_type = RFC_SIG_TYPE_INVALID; // To configure all
      tds_grfc_notime_control_data.override_logic = RFC_LOGIC_INVALID; // No override logic, use RFC AG logic
      (void)rfc_tdscdma_command_dispatch(RF_PATH_0, RFC_TDSCDMA_CLEAN_TX_GRFC_NO_TIME, (void*)(&tds_grfc_notime_control_data));
      (void)rfc_tdscdma_command_dispatch(RF_PATH_0, RFC_TDSCDMA_SET_RX_GRFC_NO_TIME, (void*)(&tds_grfc_notime_control_data));
      (void)rfc_tdscdma_command_dispatch(RF_PATH_1, RFC_TDSCDMA_SET_RX_GRFC_NO_TIME, (void*)(&tds_grfc_notime_control_data));

      rf_tdscdma_msm_set_rx_on(ftm_tdscdma_current_pri_device, ftm_tdscdma_current_mode, TRUE);

      //turn On WTR Rx(note: rx on signal is part of rx_grfcs)
      
      
#else /* Below is by default enabled to use FED execute Rx Burst script */ 
      /* Call FED to schedule Rx Burst script */
      if (rf_tdscdma_use_on_rumi == FALSE)
      {
        in_data.effectTime = 0xFFFFFFFF; // Immediate execution
        in_data.scriptType = RFLM_TDS_RX_BURST_SCRIPT;
        in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;
    
        in_data.scriptInfo.rxBurstScript.callInCalMode = TRUE;    
        in_data.scriptInfo.rxBurstScript.divEn = ftm_tdscdma_rx_div_enable;    
        in_data.scriptInfo.rxBurstScript.gainState[0] = 0; // Hard-coded G0 
        in_data.scriptInfo.rxBurstScript.gainState[1] = 0; // Hard-coded G0
        in_data.scriptInfo.rxBurstScript.linState = 0; //Hard code to default

        if(!ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].is_buf_allocated)
        {
          MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
                "ftm_tdscdma_rxlm_buffer[%d] is NOT allocated!! Handle=%d", 
                FTM_RECEIVE_CHAIN_0,
                ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);
        }

        if(ftm_tdscdma_rx_div_enable)
        {
          if(!ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].is_buf_allocated)
          {
            MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, 
                  "ftm_tdscdma_rxlm_buffer[%d] is NOT allocated!! Handle=%d", 
                  FTM_RECEIVE_CHAIN_1,
                  ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);
          }
        }
        
        in_data.scriptInfo.rxBurstScript.rxLmHandle[0] = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx; 
        in_data.scriptInfo.rxBurstScript.rxLmHandle[1] = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx;
        in_data.scriptInfo.rxBurstScript.bufId = 0; // Hard-coded bufID=0 for CAL mode
         
        exec_result = rflm_tds_schedule_rf_internal_event(&in_data);
         
        if (exec_result == RFLM_ERR_NONE)
        {   
          MSG_4(MSG_SSID_RF, MSG_LVL_MED, 
                "RxBurst script executed for device: %d, Band: %d, Chan: %d, rxdEn: %d", 
                ftm_tdscdma_current_pri_device,           
                ftm_tdscdma_current_mode,
                ftm_tdscdma_current_chan,
                ftm_tdscdma_rx_div_enable);
        }
        else
        {
          MSG_5(MSG_SSID_RF, MSG_LVL_ERROR, 
                "RxBurst script executed ERROR (code %d) for device: %d, Band: %d, Chan: %d, rxdEn: %d", 
                exec_result,
                ftm_tdscdma_current_pri_device,           
                ftm_tdscdma_current_mode,
                ftm_tdscdma_current_chan,
                ftm_tdscdma_rx_div_enable);
        }
      }
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */

      /*RXLM: ideally, only need to send rxlm msg after a freq change, however, TFW requires this msg to start rx calibration cmd */
      if(ftm_tdscdma_rx_div_enable)
      {
        rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG,(ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);
      }
      else
      {
        rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXLM_CFG_MSG,ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);
      }
    }/* end of else part (non self-cal condition) */

    //send mdsp Rx enable.
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_RX_MSG, TRUE);

    if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
    {
      FTM_MSG_HIGH("RX Enabled sent to MDSP: cal_state=%d, tx_cal=%d.",
                    cal_state,ftm_tdscdma_current_tx_cal_mode,0);
    }
  }
  else if (cal_state == FTM_TDSCDMA_CAL_STATE_TX)//Tx
  {
    /* ********************************
    **          Configure Tx
    ** *******************************/

    //send Tx enable. Set it early to let DAC settle while we set other stuff
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_TX_MSG, TRUE);

#ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE // This feature by default is NOT enabled
    /* Tx Chain PA config */

    // reset status
    pa_status = TRUE;

    pa_status &= rfdevice_tdscdma_pa_config(ftm_tdscdma_current_tx_device,
                                            ftm_tdscdma_current_mode,
                                            NULL,
                                            RFDEVICE_EXECUTE_IMMEDIATE,
                                            RFCOM_INVALID_TIMING_OFFSET);

    if (!pa_status)
    {
      FTM_MSG_ERROR( "RFFE-PA configuration ERROR for Device %d on Band %d", ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode, 0 );
    }

    /* Tx Chain ASM config */
    if (rfdevice_tdscdma_use_asm(ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode))
    {
      // reset status
      asm_status = TRUE;

      asm_status &= rfdevice_tdscdma_asm_enable_tx(ftm_tdscdma_current_tx_device,
                                                   ftm_tdscdma_current_mode,
                                                   NULL,
                                                   RFDEVICE_EXECUTE_IMMEDIATE,
                                                   RFCOM_INVALID_TIMING_OFFSET,
                                                   TRUE,
                                                   NULL,
                                                   TDS_ASM_INCLUDE_ALL);

      if (!asm_status)
      {
        FTM_MSG_ERROR( "ASM configuration ERROR for Device %d on Band %d for TX", ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode, 0 );
      }
    }
    else
    {
      FTM_MSG_MED( "NO ASM presented for Device %d on Band %d for TX", ftm_tdscdma_current_tx_device, ftm_tdscdma_current_mode, 0 ); //temp add for debug
    }

    // This will configure all Tx GRFCs (for those are configured by ASM they are included below)
    tds_grfc_notime_control_data.band = ftm_tdscdma_current_mode;
    tds_grfc_notime_control_data.sig_type = RFC_SIG_TYPE_INVALID; // To configure all
    tds_grfc_notime_control_data.override_logic = RFC_LOGIC_INVALID; // No override logic, use RFC AG logic
    (void)rfc_tdscdma_command_dispatch(ftm_tdscdma_current_pri_device,
                                       RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME,
                                       (void*)(&tds_grfc_notime_control_data));

    (void)rfc_tdscdma_command_dispatch(ftm_tdscdma_current_sec_device,
                                       RFC_TDSCDMA_CLEAN_RX_GRFC_NO_TIME,
                                       (void*)(&tds_grfc_notime_control_data));

    (void)rfc_tdscdma_command_dispatch(ftm_tdscdma_current_tx_device,
                                       RFC_TDSCDMA_SET_TX_GRFC_NO_TIME,
                                       (void*)(&tds_grfc_notime_control_data));

    #if 0 // don't need to turn off RX_ON here
    // Set RX_ON to LOW by SSBI if applicable
    rf_tdscdma_msm_set_rx_on(ftm_tdscdma_current_pri_device,
                             ftm_tdscdma_current_mode,
                             FALSE);
    #endif

    //turn on Tx. (todo: revisit: tx_grfcs has tx on but still requries explicitly set tx on signal here)
    rf_tdscdma_msm_set_tx_on(ftm_tdscdma_current_tx_device,
                             ftm_tdscdma_current_mode,
                             TRUE);

    //turn on PA
    (void)rfdevice_tdscdma_pa_on_off(ftm_tdscdma_current_tx_device,
                                     ftm_tdscdma_current_mode,
                                     TRUE,
                                     NULL,
                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                     RFCOM_INVALID_TIMING_OFFSET);

#else /* Below is by default enabled to use FED execute Tx Burst script */ 
    /* Call FED to schedule Tx Burst script */
    if (rf_tdscdma_use_on_rumi == FALSE)
    {
      rf_tdscdma_msm_set_rx_on(RFM_DEVICE_0,
                               ftm_tdscdma_current_mode,
                               FALSE);

      in_data.effectTime = 0xFFFFFFFF; // Immediate execution
      in_data.scriptType = RFLM_TDS_TX_BURST_SCRIPT;
      in_data.actionCmd  = RFLM_TDS_EXECUTE_SCRIPT;

      in_data.scriptInfo.txBurstScript.bufId = 0; // Hard-coded bufID = 0 for CAL mode
      in_data.scriptInfo.txBurstScript.txPwrdBmQ9 = 0x7FFF; // min power
      in_data.scriptInfo.txBurstScript.txLmHandle =  ftm_tdscdma_txlm_buffer.buf_idx;
      in_data.scriptInfo.txBurstScript.callInCalMode = TRUE;    
      
      exec_result = rflm_tds_schedule_rf_internal_event(&in_data);
       
      if (exec_result == RFLM_ERR_NONE)
      {   
        MSG_4(MSG_SSID_RF, MSG_LVL_MED, 
              "TxBurst script executed for device: %d, Band: %d, Chan: %d, txLMHandle: %d", 
              ftm_tdscdma_current_tx_device,           
              ftm_tdscdma_current_mode,
              ftm_tdscdma_current_chan,
              ftm_tdscdma_txlm_buffer.buf_idx);
      }
      else
      {
        MSG_5(MSG_SSID_RF, MSG_LVL_ERROR, 
              "TxBurst script executed ERROR (code %d) for device: %d, Band: %d, Chan: %d, txLMHandle: %d", 
              exec_result,
              ftm_tdscdma_current_tx_device,           
              ftm_tdscdma_current_mode,
              ftm_tdscdma_current_chan,
              ftm_tdscdma_txlm_buffer.buf_idx);
      }
    }
#endif /* #ifdef RF_TDSCDMA_USE_DEVICE_IMMEDIATE_WRITE */


    // set DAC Update signal to latch Tx IQ EN
    // Question: Do we need this for DIME?
    rf_tdscdma_msm_set_dac_update();

    if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
    {
      FTM_MSG_HIGH("Tx Enable. cal_state=%d, tx_cal=%d.", cal_state, ftm_tdscdma_current_tx_cal_mode, 0);
    }

  }

  /* Don't update the cal state when enabling Rx path during the Tx self Cal
  */
  if (!( ftm_tdscdma_current_tx_cal_mode == FTM_TDSCDMA_TX_CAL_SELF
         && cal_state == FTM_TDSCDMA_CAL_STATE_RX ))
  {
    //cache the current state
    ftm_tdscdma_current_cal_state = cal_state;

    if (ftm_tdscdma_ctl_dbg_msg_flag == TRUE)
    {
      FTM_MSG_HIGH("FTM cal state was updated: cal_state=%d",cal_state,0,0);
    }
  }
} /* ftm_tdscdma_config_fw_cal_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RX powerl level in RX AGC and dBm units

  @details
  Function to get Rx Power Level. Returns the Rx level, for a specified receiver, in
  both Rx AGC and dBm10 units

  @param device
  RF Device to which FTM is to be entered

  @param rx_level_agc
  RX level in AGC units

  @param rx_level_dbm10
  RX level in dBmx10 units

  @return
  TRUE on success and FALSE on failure
*/
boolean ftm_tdscdma_get_rx_level_dbm
(
  rfcom_device_enum_type device,
  int16 *rx_level_agc,
  int16 *rx_level_dbm10
)
{
  boolean ret_val = FALSE;
  int16 mdsp_rxlvldBm10 = 0xFFFF;
  int16 mdsp_rxlvlagc = 0xFFFF;

  if((device == ftm_tdscdma_current_sec_device) && (ftm_tdscdma_rx_div_enable == FALSE))
  {
    FTM_MSG_ERROR("RFMODE: RxD RX Level dBm. Set secondary chain command not called!",0,0,0);
    ret_val = FALSE;
    return ret_val;
  }

  /* Query SMEM for RX AGC information */
  ret_val = rf_tdscdma_mdsp_get_raw_rx_agc(device, &mdsp_rxlvldBm10, &mdsp_rxlvlagc);

  if(ret_val == FALSE)
  {
    FTM_MSG_ERROR("RFMODE: RX Level dBm. RX AGC value is bad!",0,0,0);
  }
  *rx_level_dbm10 = mdsp_rxlvldBm10;
  *rx_level_agc = mdsp_rxlvlagc;

  FTM_MSG_HIGH("RFMODE: RX Level dBm. Raw: %d, dBm10: %d",*rx_level_agc,*rx_level_dbm10,0);

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API returns the Rx level, for all the requested receiver chains associated
  with one carrier, in both Rx AGC and dBm units. This command is only valid after
  the mode (Technology/Band) is set using FTM_SET_MODE and FTM_SET_CALIBRATION_STATE
  has been used to deactivate calibration state or calibration state has not been 
  activated while the device was powered up. Also, its required to enable all 
  receiver chains and program them to correct LNA state  before requesting RxAGC 
  for a Rx chain (e.g. secondary chain should be enabled using FTM_SET_SECODNARY_CHAIN 
  before using this command).

  @details
  Function to get Rx Power Level. Returns the Rx level, for a specified receiver, in
  both Rx AGC and dBm10 units

  @param device
  RF Device to which FTM is to be entered

  @param select_rx_chain
  Selects the receiver chain for which Rx level needs to be measured

  @param rsp_pkt
  req_pkt_ptr pointer to the DIAG request packet.

  @return
  TRUE on success and FALSE on failure
*/
boolean ftm_tdscdma_get_multiple_rx_level_dbm
(
  rfcom_device_enum_type device,
  uint16 select_rx_chain,
  ftm_rsp_pkt_type *rsp_pkt
)
{
  boolean ret_val = FALSE;
  uint8 i;
  uint8 number_selected_rx_chains = 0;
  uint16 status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
  /* Get Rx Level DBM data */
  int16 ftm_tdscdma_rx_level_agc[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] ={0};
  int16 ftm_tdscdma_rx_level_dbm10[FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER] = {0};   
  uint16 payload_size = 0;

  for (i=0;i<FTM_RX_LEVEL_DBM_MAX_SELECTED_RECEIVER;i++)
  {
    if ((select_rx_chain >> i) & (uint16)0x1)
    {
      if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
      {
        switch(i)
        {
           case 0:
             /* Query SMEM for RxAGC Data on Primary Chain */
             ret_val = rf_tdscdma_mdsp_get_raw_rx_agc(device, 
                                                      &ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains], 
                                                      &ftm_tdscdma_rx_level_agc[number_selected_rx_chains]);
             if (ret_val == FALSE )
             {
               status = FTM_RX_LEVEL_DBM_TEST_GENERAL_FAILURE;
               ftm_tdscdma_rx_level_agc[number_selected_rx_chains] = ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains] = 0;
               FTM_MSG_ERROR("RFMODE: FTM_GET_MULTIPLE_RX_LEVEL_DBM - Primary Chain RX AGC value is bad!",0,0,0);
             }
             else
             {  
               status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
               FTM_MSG_MED("FTM GET_MULTIPLE_RX_LEVEL_DBM - Primary Chain Rx AGC = %d : Rx Power dbm10 = %d",ftm_tdscdma_rx_level_agc[number_selected_rx_chains],
                                                                                                             ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains],0);               
             }
             break;
           case 1:
             /* Query SMEM for RxAGC Data on Secondary Chain if Secondary chain is  enabled */
             if (ftm_tdscdma_rx_div_enable == TRUE)
             {                             
               ret_val = rf_tdscdma_mdsp_get_raw_rx_agc(ftm_tdscdma_current_sec_device, 
                                                        &ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains], 
                                                        &ftm_tdscdma_rx_level_agc[number_selected_rx_chains]);               
               if (ret_val == FALSE )
               {
                 status = FTM_RX_LEVEL_DBM_TEST_GENERAL_FAILURE;
                 ftm_tdscdma_rx_level_agc[number_selected_rx_chains] = ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains] = 0;
                 FTM_MSG_ERROR("RFMODE: FTM_GET_MULTIPLE_RX_LEVEL_DBM - Secondary Chain RX AGC value is bad!",0,0,0);
               }
               else
               {  
                 status = FTM_RX_LEVEL_DBM_TEST_SUCCESS;
                 FTM_MSG_MED("FTM GET_MULTIPLE_RX_LEVEL_DBM - Secondary Chain Rx AGC = %d : Rx Power dbm10 = %d", ftm_tdscdma_rx_level_agc[number_selected_rx_chains],
                                                                                                                  ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains],0);               
               }               
             }
             else
             {            
               status = FTM_RX_LEVEL_DBM_TEST_ATLEAST_ONE_RECEIVER_BAD_STATE;
               ftm_tdscdma_rx_level_agc[number_selected_rx_chains] = ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains] = 0;
               FTM_MSG_ERROR("RFMODE: FTM_GET_MULTIPLE_RX_LEVEL_DBM - Secondary chain not enabled!",0,0,0);                              
             }
             break;
           default:
             status = FTM_RX_LEVEL_DBM_TEST_INVALID_RX_CHAIN;
             FTM_MSG_ERROR("FTM_GET_MULTIPLE_RX_LEVEL_DBM - Invalid receiver type selected (select_rx_chain - %4X)", select_rx_chain,0,0);   
             ftm_tdscdma_rx_level_agc[number_selected_rx_chains] = ftm_tdscdma_rx_level_dbm10[number_selected_rx_chains] = 0;
             break;
        }
      }
      number_selected_rx_chains++;
    }
  }
  if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
  {
    ret_val = TRUE;
  }
  else
  {
    ret_val = FALSE;
  }

  FTM_MSG_MED("Preparing for the response packet - FTM_GET_MULTIPLE_RX_LEVEL_DBM",0,0,0);  
    
  /***************** Fill up RSP packet with standard data *****************/    
  // Fill in the constant fields of the response packet. Sample buffer will be filled after sanity check.
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.cmd_code = 75;
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.subsys_id = FTM;
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.subsys_cmd_code = ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) ? FTM_TDSCDMA_C : FTM_TDSCDMA_RX_2_C ;
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.ftm_rf_cmd = FTM_GET_MULTIPLE_RX_LEVEL_DBM;  

  // Start with sucess flag and change it if we fail any check conditions.
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.select_rx_chain = select_rx_chain;
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.ftm_error_code = status;
    
  payload_size = number_selected_rx_chains*(sizeof(ftm_tdscdma_rx_level_agc[0]) + sizeof(ftm_tdscdma_rx_level_dbm10[0]));
  ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.payload_size = payload_size;
     
  for (i=0; i<number_selected_rx_chains; i++)
  {
    if (status == FTM_RX_LEVEL_DBM_TEST_SUCCESS)
    {
      ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i] = ftm_tdscdma_rx_level_agc[i];
      ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i + 1] = ftm_tdscdma_rx_level_dbm10[i];
    }
    else
    {
      ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i] = 0;
      ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt.results[2*i + 1] = 0;
    }
  }
     
  // Fill in the constant fields of the response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_tdscdma_get_multiple_rx_level_dbm_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len =  6 + 2*sizeof(uint16) + sizeof(int16) + payload_size;
   
  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Adjust the frequency of TX

  @details
  This API is used for applying a Tx frequency error compensation value.
  Frequency error is in Hz. The amount of frequency correction applied is either
  the user_freq_adjust_ppb or the combination of user_freq_adjust_ppb + the
  frequency error estimate from XO manager. This selection is based on the input
  value of enable_xo_freq_adjust

  @param user_freq_adjust_ppb
  User defined frequency adjustment in ppb units
  ppb = Parts Per Billion
  ppb = Frequency Adjust (HZ)/ Center Frequency (MHz) * 1000

  @param center_frequency_hz
  Carrier frequency for the current cellular band/channel, in Hz's units

  @param enable_xo_freq_adjust
  0 - Total Frequency Adjust = User defined frequency adjustment (i.e. user_freq_adjust_ppb)
  1 - Total Frequency Adjust = User defined frequency adjustment (i.e. user_freq_adjust_ppb)
      + Frequency error reported by XO manager.

  @return
  TRUE on success and FALSE on failure
*/
int32 ftm_tdscdma_set_freq_adjust
(
  int32 user_freq_adjust_ppb,
  uint32 center_frequency_hz,
  uint8 enable_xo_freq_adjust
)
{
  int32 tx_freq_error_composite_hz = 0;
  int32 freq_error_xo_ppb =0;
  int32 freq_error_xo = 0;

  /* Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = TRUE;

  FTM_MSG_HIGH("ftm_tdscdma_set_freq_adjust: tx_freq_error_ppb : %d : centre_freq_hz : %d : Enable = %d",
            user_freq_adjust_ppb,
            center_frequency_hz ,
            enable_xo_freq_adjust);

  if(enable_xo_freq_adjust)
  {
    freq_error_xo = (int32)tcxomgr_ft_curr_blocking();

    //Converts the frequency error estimated by XO Manager API to ppb units.
    freq_error_xo_ppb = (int32)((int64)((int64)((int64)(freq_error_xo <<16) * 1000)/ 1024)>> 16);

    FTM_MSG_HIGH("ftm_tdscdma_set_freq_adjust: freq_error_xo_ppb : %d", freq_error_xo_ppb,0,0);

    if(!user_freq_adjust_ppb)
    {
      // The formula below does:
      // 1) Converts the frequency error estimated by XO Manager API to Hz from ppb as expected by
      // rf_tdscdma_mdsp_set_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* (freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1)* ((int32)((int64)((
                                         (int64)((freq_error_xo_ppb << 16))*
                                         (int64)((center_frequency_hz))
                                        )/1000000000) >>16 ));


      MSG_HIGH("ftm_tdscdma_set_freq_adjust: tx_freq_error_composite_hz : %d",tx_freq_error_composite_hz,0,0);
    }
    else
    {
      // The formula below does:
      // 1) Adds the user input frequency error with frequency error estimated by XO Manager.
      // 2) Converts the result in 1) from ppb units to Hz as expected by rf_tdscdma_mdsp_set_freq_adjust() API
      // tx_freq_error_composite_hz = (-1)* ((user_freq_adjust_ppb  + freq_error_xo_ppb)*2^16 * center_frequency_hz) / 10^6 / 2^16

      tx_freq_error_composite_hz = (-1)*(int32)((int64)((
                                          (int64)(((int64)((user_freq_adjust_ppb  + freq_error_xo_ppb) << 16))* center_frequency_hz )
                                          )/1000000000) >> 16) ;

      FTM_MSG_HIGH("ftm_tdscdma_set_freq_adjust: tx_freq_error_composite_hz : %d",tx_freq_error_composite_hz,0,0);
    }
  }
  else
  {
     tx_freq_error_composite_hz = user_freq_adjust_ppb;
     FTM_MSG_HIGH("ftm_tdscdma_set_freq_adjust: tx_freq_error_composite_hz : %d",tx_freq_error_composite_hz,0,0);
  }

  /* Update TX params shadow with frequency error */
  ftm_tdscdma_burst_tx_params_shadow.freqOffset = tx_freq_error_composite_hz;

  /* Call API/MSGR command to adjust the frequency error */
  rf_tdscdma_mc_burst_tx(TRUE,ftm_tdscdma_burst_tx_params_shadow, ftm_tdscdma_txlm_buffer.buf_idx, TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID);
  (void)ftm_tdscdma_rf_mode_tx_cmd_handler();

  /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = FALSE;

  return tx_freq_error_composite_hz;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup the TX parameters for RF Mode TX

*/
boolean ftm_tdscdma_set_burst_tx_params
(
  uint8 num_slots,
  uint8 mod_type,
  int16 ts1_pwrlvl,
  int16 ts2_pwrlvl,
  int16 ts3_pwrlvl,
  int16 ts4_pwrlvl,
  int16 ts5_pwrlvl
)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */

  ftm_tdscdma_power_dbm = ts1_pwrlvl;

  /* Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = TRUE;

  /* Check and turn OFF if RX is ON */
  if(ftm_tdscdma_burst_rx_state == FTM_TDSCDMA_BURST_RX_ON)
  {
    FTM_MSG_ERROR("RX is ON! Bailing out!",0,0,0);
    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
    /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
    ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = FALSE;
    return error_code;
  }

  /* Update entries in the shadow */
  ftm_tdscdma_burst_tx_params_shadow.num_slots = num_slots;
  ftm_tdscdma_burst_tx_params_shadow.mod_type = mod_type;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[0] = ts1_pwrlvl;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[1] = ts2_pwrlvl;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[2] = ts3_pwrlvl;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[3] = ts4_pwrlvl;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[4] = ts5_pwrlvl;

  switch (ftm_tdscdma_burst_tx_state)
  {
    case FTM_TDSCDMA_BURST_TX_OFF:
      /* Reset freq offset */
      ftm_tdscdma_burst_tx_params_shadow.freqOffset = 0;
      error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
      break;

    case FTM_TDSCDMA_BURST_TX_ON:
      FTM_MSG_MED("TX parameters changed while RF Mode TX is ON: %d",ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[0],0,0);
      /* Call MSGR command to update TX power Level */
      rf_tdscdma_mc_burst_tx(TRUE,ftm_tdscdma_burst_tx_params_shadow, ftm_tdscdma_txlm_buffer.buf_idx, TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID);
      error_code = ftm_tdscdma_rf_mode_tx_cmd_handler();
      break;

    default:
      FTM_MSG_ERROR("Unknown burst TX state: %d. Changes will not take effect. Bailing out!",ftm_tdscdma_burst_tx_state,0,0);
      error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
      break;
  }

  /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = FALSE;

  return error_code; /* 0: Success, 1: Error */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable/disable the transmitter in RF mode

*/
boolean ftm_tdscdma_burst_tx
(
  boolean on_off
)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */

  /* Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = TRUE;

  /* Turn OFF FTM cont RX and TX */
  ftm_tdscdma_set_rx(FALSE);
  //ftm_clk_rex_wait(5);
  ftm_tdscdma_set_tx(FALSE);
  //ftm_clk_rex_wait(5);

  /* Check and return if Cal State Flag is TRUE */
  if(rfm_get_calibration_state())
  {
    FTM_MSG_ERROR("Calibration State Flag is TRUE. Bailing out!",0,0,0);
    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
    /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
    ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = FALSE;
    return error_code;
  }

  /* Check and disable if RX if it is ON */
  if(ftm_tdscdma_burst_rx_state == FTM_TDSCDMA_BURST_RX_ON)
  {
    FTM_MSG_ERROR("RX is ON! Bailing out!",0,0,0);

    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
    /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
    ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = FALSE;
    return error_code;
  }

  if(on_off)
  {
    switch(ftm_tdscdma_burst_tx_state)
    {
       case FTM_TDSCDMA_BURST_TX_OFF:
         FTM_MSG_HIGH("Turning ON TD-SCDMA burst TX",0,0,0);

         /* Call MC layer API to put the state machine for TX RF mode operation */
         (void)rf_tdscdma_mc_set_sm_rfmode_tx(ftm_tdscdma_current_tx_device);

         /* Emulate the L1 API call to set network power limit
              Setting the network power limit to 24dBm. TX power limiting uses MIN(network limit, NV max power)
              hence it will pick the max power from NV.
              */
         (void)rf_tdscdma_mc_set_tx_pwr_limit(24);

         /* Enable TX HW block in FTM RF Mode */
         rf_tdscdma_mdsp_tx_hw_block_config(TRUE);

         /* Call MC layer API to initiate MTPL/HDET/THERM loops */
         (void)rf_tdscdma_mc_enable_tx(ftm_tdscdma_current_tx_device, rf_tdscdma_ftm_wrk_chan, ftm_tdscdma_txlm_buffer.buf_idx, NULL);

         /* Send MSGR command to start TX burst and pass the current shadow TX params struct */
         rf_tdscdma_mc_burst_tx(TRUE,ftm_tdscdma_burst_tx_params_shadow, ftm_tdscdma_txlm_buffer.buf_idx, TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID);
         error_code = ftm_tdscdma_rf_mode_tx_cmd_handler();

         /* Set the burst TX state machine to TX_ON */
         ftm_tdscdma_burst_tx_state = FTM_TDSCDMA_BURST_TX_ON;

      	 /*numslot == 0 means it's actually FTM RF Cont Tx*/
         if (0 == ftm_tdscdma_burst_tx_params_shadow.num_slots)
         {
           if (FTM_SYNTH_STATUS_DISABLED == tx_pll_multi_state)
           {
             tx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
             FTM_MSG_HIGH("Set Multi Synth Mode for Tx in FTM RF Burst Mode: Disabled -> Locked",0,0,0);
           }
         }

         if (FTM_SYNTH_STATUS_DISABLED == rx_pll_multi_state)
         {
           rx_pll_multi_state = FTM_SYNTH_STATUS_NOT_APPLICABLE;
           FTM_MSG_HIGH("Set Multi Synth Mode for Rx in FTM RF Burst Mode: Disabled -> Not Applicable",0,0,0);
         }
         break;

       case FTM_TDSCDMA_BURST_TX_ON:
         FTM_MSG_LOW("TX is already ON.",0,0,0);
         error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
         break;

       default:
         FTM_MSG_ERROR("Unknown burst TX state: %d. Changes will not take effect. Bailing out!",ftm_tdscdma_burst_tx_state,0,0);
         error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
         break;
    }
  }
  else
  {
    switch(ftm_tdscdma_burst_tx_state)
    {
       case FTM_TDSCDMA_BURST_TX_OFF:
         FTM_MSG_LOW("TX is already OFF.",0,0,0);
         error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
         break;

       case FTM_TDSCDMA_BURST_TX_ON:
         FTM_MSG_HIGH("Turning OFF TD-SCDMA burst TX",0,0,0);

         // Changed the call sequence: Need to disable FW/FED before disable RFSW
         
         /* Send MSGR command to stop burst TX */
         rf_tdscdma_mc_burst_tx(FALSE,
                                ftm_tdscdma_burst_tx_params_shadow, 
                                ftm_tdscdma_txlm_buffer.buf_idx, 
                                TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID);
         error_code = ftm_tdscdma_rf_mode_tx_cmd_handler();

         /* Disable TX HW block in FTM RF Mode */
         rf_tdscdma_mdsp_tx_hw_block_config(FALSE);

         /* Call MC layer API to disable TX */
         if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
         {
           rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
         }

         /* Set the burst TX state machine to TX_OFF */
         ftm_tdscdma_burst_tx_state = FTM_TDSCDMA_BURST_TX_OFF;

         rf_tdscdma_mc_set_burst_tx_cmd_status(TDSCDMA_RFMODE_TX_CMD_RSP_IDLE);

         /*numslot == 0 means it's actually FTM RF Cont Tx*/
         if (0 == ftm_tdscdma_burst_tx_params_shadow.num_slots)
         {
           if (FTM_SYNTH_STATUS_NOT_APPLICABLE == rx_pll_multi_state)
        	 {
        		 rx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
        		 FTM_MSG_HIGH("Set Multi Synth Mode for Rx in FTM RF Mode: Not Applicable -> Disabled",0,0,0);
        	 }
         }
         break;

       default:
         FTM_MSG_ERROR("Unknown burst TX state: %d. Changes will not take effect. Bailing out!",ftm_tdscdma_burst_tx_state,0,0);
         error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
         break;
    }
  }

  /* Un-Block HDET/THERM TX reconfig messages (only applicable to continuous TX mode */
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = TRUE;

  return error_code; /* 0: Success, 1: Error */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the RF Mode TX parameters data structure

*/
rf_tdscdma_ftm_burst_tx_params_type ftm_tdscdma_get_rf_mode_tx_params(void)
{
  return ftm_tdscdma_burst_tx_params_shadow;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return the FTM TX Link Manager buffer index

*/
uint32 ftm_tdscdma_get_lm_idx(void)
{
  return ftm_tdscdma_txlm_buffer.buf_idx;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable/disable receiver in RF mode

*/
boolean ftm_tdscdma_burst_rx
(
  int16 exp_rx_lvl,
  boolean on_off
)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */
  uint32 rxlm_bufidx = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx;
  uint32 rxlm_bufidx_div = ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx;

  /* Turn OFF FTM cont RX and TX */
  ftm_tdscdma_set_rx(FALSE);
  //ftm_clk_rex_wait(5);
  ftm_tdscdma_set_tx(FALSE);
  //ftm_clk_rex_wait(5);

  /* Check and return if Cal State Flag is TRUE */
  if(rfm_get_calibration_state())
  {
    FTM_MSG_ERROR("Calibration State Flag is TRUE. Bailing out!",0,0,0);
    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
    return error_code;
  }

  /* Check and disable if TX if it is ON */
  if(ftm_tdscdma_burst_tx_state == FTM_TDSCDMA_BURST_TX_ON)
  {
    FTM_MSG_ERROR("TX is ON! Bailing out!",0,0,0);
    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
    return error_code;
  }

  /* Update shadow */
  if (!ftm_tdscdma_rx_div_enable)
  {
    ftm_tdscdma_burst_rx_params_shadow.rx1_active = FALSE;
  }
  else
  {
    ftm_tdscdma_burst_rx_params_shadow.rx1_active = TRUE;
  }

  ftm_tdscdma_burst_rx_params_shadow.exp_rx_lvl = exp_rx_lvl;
  /* Identify LNA state from 'exp_rx_lvl' */
  ftm_tdscdma_get_lna_range(exp_rx_lvl);

  if(on_off)
  {
    switch(ftm_tdscdma_burst_rx_state)
    {
      case FTM_TDSCDMA_BURST_RX_OFF:
        FTM_MSG_HIGH("Turning ON TD-SCDMA burst RX",0,0,0);

        /* Send MSGR command to start RX burst and pass the current shadow RX params struct */
        rf_tdscdma_mc_burst_rx(TRUE,ftm_tdscdma_burst_rx_params_shadow, rxlm_bufidx, rxlm_bufidx_div);
        error_code = ftm_tdscdma_rf_mode_rx_cmd_handler();

        /* Set the burst RX state machine to RX_ON */
        ftm_tdscdma_burst_rx_state = FTM_TDSCDMA_BURST_RX_ON;

   		  if (FTM_SYNTH_STATUS_DISABLED == rx_pll_multi_state)
    		{
    			rx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
    	    	FTM_MSG_HIGH("Set Multi Synth Mode for Rx in FTM RF Burst Mode: Disabled -> Locked",0,0,0);
    		}

      	if (FTM_SYNTH_STATUS_DISABLED == tx_pll_multi_state)
      	{
  	    	tx_pll_multi_state = FTM_SYNTH_STATUS_NOT_APPLICABLE;
  	    	FTM_MSG_HIGH("Set Multi Synth Mode for Tx in FTM RF Burst Mode: Disabled -> Not Applicable",0,0,0);
      	}
        break;

      case FTM_TDSCDMA_BURST_RX_ON:
        /* Send MSGR command to update LNA state based on shadow rx params */
        rf_tdscdma_mc_burst_rx(TRUE,ftm_tdscdma_burst_rx_params_shadow, rxlm_bufidx, rxlm_bufidx_div);
        error_code = ftm_tdscdma_rf_mode_rx_cmd_handler();

        /* Set the burst RX state machine to RX_ON */
        ftm_tdscdma_burst_rx_state = FTM_TDSCDMA_BURST_RX_ON;
        break;

      default:
        FTM_MSG_ERROR("Unknown burst RX state: %d. Changes will not take effect. Bailing out!",ftm_tdscdma_burst_rx_state,0,0);
        error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
        break;
    }
  }
  else
  {
    switch(ftm_tdscdma_burst_rx_state)
    {
       case FTM_TDSCDMA_BURST_RX_OFF:
         FTM_MSG_HIGH("RX is already OFF. Ignoring this command!",0,0,0);
        error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
        break;

      case FTM_TDSCDMA_BURST_RX_ON:
         FTM_MSG_HIGH("Turning OFF TD-SCDMA burst RX",0,0,0);
         /* Send MSGR command to stop burst RX */
         rf_tdscdma_mc_burst_rx(FALSE,ftm_tdscdma_burst_rx_params_shadow, rxlm_bufidx, rxlm_bufidx_div);
         error_code = ftm_tdscdma_rf_mode_rx_cmd_handler();

         /* Set the burst RX state machine to RX_OFF */
         ftm_tdscdma_burst_rx_state = FTM_TDSCDMA_BURST_RX_OFF;

         rf_tdscdma_mc_set_burst_rx_cmd_status(TDSCDMA_RFMODE_RX_CMD_RSP_IDLE);

    		 if (FTM_SYNTH_STATUS_NOT_APPLICABLE == tx_pll_multi_state)
    		 {
    			 tx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
    			 FTM_MSG_HIGH("Set Multi Synth Mode for Tx in FTM RF Mode: Not Applicable -> Disabled",0,0,0);
    		 }

        break;

      default:
         FTM_MSG_ERROR("Unknown burst RX state: %d. Changes will not take effect. Bailing out!",ftm_tdscdma_burst_rx_state,0,0);
        error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
        break;
    }
  }

  return error_code; /* 0: Success, 1: Error */
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the LNA state from RX Level

  @details
  Function to update the LNA state from RX level. This API will derive the swith-points
  in dBmx10 value from the LNA TRK switch-point NV items (which are in dBFS scale).
  The conversion formula to convert switchpoint in dBm from dBFS scale is as follows
  (Documented in 80-N9269-1)

  Pin(dBm) = (Switch Point in dBFS)/2 + (LNA_offset) / 10 + 48
  For example, from Gain Range 0 --> Gain Range 1:
  Pin(dBm) = -65/2 -843/10 + 48 = -68.8 dBm

  @param exp_rx_lvl
  DL power level in dBmx10 units

  @return
  none
*/
void ftm_tdscdma_get_lna_range
(
  int16 exp_rx_lvl
)
{
  uint16 computed_lna_state = RF_TDSCDMA_MAX_LNA_GAIN_STATES-1; /* Default to lowest LNA state */
  uint16 rx_fcomp = 0;
  /* LNA switchpoints.
  index 0: G0->G1, index 1: G1->G2, index 2: G2->G3
  */
  int16 lna_swpts_rx0_dbx10[RF_TDSCDMA_MAX_LNA_GAIN_STATES-1] = {0,0,0};
  int16 lna_swpts_rx1_dbx10[RF_TDSCDMA_MAX_LNA_GAIN_STATES-1] = {0,0,0};

//TO-DO add correct RX linearity. Currently hard coded to 0

  if(rfnv_tdscdma_rx_tbl_ptr != NULL)
  {
    /* Rx0 */
    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_0,0,0);
    lna_swpts_rx0_dbx10[0] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[0]*10)>>1) +
      rx_fcomp + 480;

    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_0,1,0);
    lna_swpts_rx0_dbx10[1] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[4]*10)>>1) +
      rx_fcomp + 480;

    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_0,2,0);
    lna_swpts_rx0_dbx10[2] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[8]*10)>>1) +
      rx_fcomp + 480;

    if(exp_rx_lvl <= lna_swpts_rx0_dbx10[0])
    {
      computed_lna_state = 0;
    }
    else if(exp_rx_lvl > lna_swpts_rx0_dbx10[0] && exp_rx_lvl <= lna_swpts_rx0_dbx10[1])
    {
      computed_lna_state = 1;
    }
    else if(exp_rx_lvl > lna_swpts_rx0_dbx10[1] && exp_rx_lvl <= lna_swpts_rx0_dbx10[2])
    {
      computed_lna_state = 2;
    }
    else
    {
      computed_lna_state = 3;
    }

    FTM_MSG_MED("Computed RX0 LNA State: %d",computed_lna_state,0,0);
    ftm_tdscdma_burst_rx_params_shadow.computed_lna_state = computed_lna_state;

    /* Rx1 */
    computed_lna_state = RF_TDSCDMA_MAX_LNA_GAIN_STATES-1;

    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_1,0,0);
    lna_swpts_rx1_dbx10[0] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[0]*10)>>1) +
      rx_fcomp + 480;

    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_1,1,0);
    lna_swpts_rx1_dbx10[1] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[4]*10)>>1) +
      rx_fcomp + 480;

    rx_fcomp = rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(RF_PATH_1,2,0);
    lna_swpts_rx1_dbx10[2] =
      -((rfnv_tdscdma_rx_tbl_ptr->lna_range_rise_fall_wb_mode_tracking[8]*10)>>1) +
      rx_fcomp + 480;

    if(exp_rx_lvl <= lna_swpts_rx1_dbx10[0])
    {
      computed_lna_state = 0;
    }
    else if(exp_rx_lvl > lna_swpts_rx1_dbx10[0] && exp_rx_lvl <= lna_swpts_rx1_dbx10[1])
    {
      computed_lna_state = 1;
    }
    else if(exp_rx_lvl > lna_swpts_rx1_dbx10[1] && exp_rx_lvl <= lna_swpts_rx1_dbx10[2])
    {
      computed_lna_state = 2;
    }
    else
    {
      computed_lna_state = 3;
    }

    FTM_MSG_MED("Computed RX1 LNA State: %d",computed_lna_state,0,0);
    ftm_tdscdma_burst_rx_params_shadow.computed_lna_state_rx1 = computed_lna_state;
  }
  else
  {
    FTM_MSG_MED("Skipped ftm_tdscdma_get_lna_range() because rfnv_tdscdma_rx_tbl_ptr = NULL",0,0,0);     
  }

}

void ftm_tdscdma_do_iq_capture ( uint16 ftm_channel )
{
    /* disable tx in order to turn rx */
    if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
    {
      rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
    }

    // Preparing Shard Memory for RXLM config
    rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device, 
                            ftm_tdscdma_current_sec_device,
                            ftm_channel,
                            ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                            RFA_TDSCDMA_RXLM_MODE_TRK, // Note: trk mode is used for IQ capture
                    #ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                            RFA_TDSCDMA_RX_PATH_BOTH, // RFA_TDSCDMA_RX_PATH_PRIM,
                            ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                            RFA_TDSCDMA_RXLM_MODE_TRK,
                    #endif
                            NULL, 
                            NULL,
                            FALSE, // is_dsds
                            ftm_tdscdma_rx_lin_state,
                            TRUE   // always use SW tune in ACQ
                            );

    /* It is required to change fw state for starting iq capture */
    ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_STANDBY);
    FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW to STANDBY", 0, 0, 0);

    ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_FTM_TRK);
    FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW to FTM_TRK", 0, 0, 0);

    // RXLM Config
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXLM_CFG_MSG,ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);
    FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW RXLM CFG. buf_idx = %d",
                  ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx, 0, 0);
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG,(ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);
    FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW RXLM_RXD CFG. buf_idx = %d",
                  (ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx, 0, 0);

    // Sending iq capture command
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG,((ftm_tds_iq_capture_data.SampleSize << 16) & 0xFFFF0000));
    FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send IQ capture cmd to TFW: sample = %d",
                  ftm_tds_iq_capture_data.SampleSize, 0, 0);
}

/*===========================================================================
FUNCTION ftm_tdscdma_cmd_get_iq_data_from_fw

DESCRIPTION
   This function used to send cmd to FW to get the IQ data

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_cmd_get_iq_data_from_fw(rfm_device_enum_type device)
{
    boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;

    /*Store the ant_port of IQ capture for future check*/
    ftm_tdscdma_iq_acquired_samples_data.device_with_IQ = device;
    /*Set the data flag to false before getting IQ data*/
    ftm_tdscdma_iq_acquired_samples_data.data_avail_from_FW = FALSE;

    rf_tdscdma_mc_set_blocking_cmd_rsp_status(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG, TDSCDMA_RFMODE_CMD_RSP_PENDING);

    //Send cmd to FW to get IQ data
    rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG, ((((uint32)FTM_TDSCDMA_IQ_MAX_SAMPLES << 16) & 0xFFFF0000) | device ));

    error_code = ftm_tdscdma_rf_mode_blocking_cmd_handler(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG);

    FTM_MSG_HIGH("TDSCDMA IQ Capture: send cmd to FW to get data for device:%d and error_code: %d", device, error_code, 0);
}


/*===========================================================================
FUNCTION ftm_tdscdma_util_convert_20pcfl_iq_data_format

DESCRIPTION
   This function used to convert iq data format from FW to the format that RFSW used
   I and Q buffer is combined into a 32-bit 1/q datatype in FW. Here they're separated for easy analysis

   The IQ data from FW is PCFL format, which is 20bit, the highest 4 bit is exponent, the middle 8 bit is i-path data, the lowest 8 bit is q-path data.
   As below
   ________________________________________________________________
   |                    |                                        |                                         |
   |exponet(4bit) |     i-path data(8bit)            |     q-path data(8bit)            |
   |_______________________________________________________________|

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_util_convert_20pcfl_iq_data_format
(
  uint32 *input_buf,
  word *output_buf,
  uint32 num_sample
)
{
  uint8 bit_offset = 0;
  uint64 sample;
  uint32 sample_pcfl;
  uint16 exponent = 0;
  uint16 cnt = 0;
  int16 i_data = 0;
  int16 q_data = 0;

  for(cnt=0; cnt<num_sample; cnt++)
  {
    sample = (((uint64)input_buf[0]>>0) + ((uint64)input_buf[1] << 32))>> bit_offset;

    if((bit_offset += 20) >= 32)
    {
      input_buf++;
    }
    bit_offset = bit_offset % 32;

    sample_pcfl = sample & 0xFFFFF;

    exponent = (sample_pcfl >> 16) & 0x0f; ///< 4 bit exponent
    i_data = (sample_pcfl) & 0xff00;
    q_data = (sample_pcfl<<8) & 0xff00;
    i_data = i_data >> (exponent); ///< Q.15 format
    q_data = q_data >> (exponent); ///< Q.15 format

    output_buf[cnt * 2] = (word)i_data;
    output_buf[cnt * 2 + 1] = (word)q_data;
  }
}

/*===========================================================================
FUNCTION ftm_tdscdma_util_convert_10pcfl_iq_data_format

DESCRIPTION
   This function used to convert iq data format from FW to the format that RFSW used
   I and Q buffer is combined into a 32-bit 1/q datatype in FW. Here they're separated for easy analysis

   The IQ data from FW is 10PCFL format, which is 10bit, the highest 4 bit is exponent, the middle 3 bit
   is i-path data, the lowest 3 bit is q-path data.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_util_convert_10pcfl_iq_data_format
(
  uint32 *input_buf,
  word *output_buf,
  uint32 num_sample
)
{
  uint8 bit_offset = 0;
  uint64 sample;
  uint32 sample_pcfl;
  uint16 exponent = 0;
  uint16 cnt = 0;
  int16 i_data = 0; 
  int16 q_data = 0;

  for(cnt=0; cnt<num_sample; cnt++)
  {
    sample = (((uint64)input_buf[0]>>0) + ((uint64)input_buf[1] << 32))>> bit_offset;

    if((bit_offset += 10) >= 32)
    {
      input_buf++;
    }
    bit_offset = bit_offset % 32;

    sample_pcfl = sample & 0x3FF;

    exponent = (sample_pcfl >> 6) & 0x0f; ///< 4 bit exponent
    i_data = (sample_pcfl<<10) & 0xE000;  ///shift to MSB and extract
    q_data = (sample_pcfl<<13) & 0xE000;
    i_data >>= exponent; ///< Q.15 format
    q_data >>= exponent; ///< Q.15 format

    output_buf[cnt * 2] = (word)i_data;
    output_buf[cnt * 2 + 1] = (word)q_data;
  }
}

/*===========================================================================
FUNCTION ftm_tdscdma_store_iq_data

DESCRIPTION
   This function used to store the IQ data from FW

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_store_iq_data(uint32 start_address, uint32 start_address_div, uint32 num_samples)
{
  if(num_samples != FTM_TDSCDMA_IQ_CAPTURE_GET_SAMPLE)
  {
    FTM_MSG_ERROR("TDSCDMA IQ Capture: sample num captured doesn't match configured, error!", 0, 0, 0);
  }

  /*Set the data flag to true*/
  ftm_tdscdma_iq_acquired_samples_data.data_avail_from_FW = TRUE;

  /*Store the start address and num_samples of the IQ data that captured*/
  raw_data_start_address = (uint32 *)start_address;
  raw_data_start_address_div = (uint32 *)start_address_div;

  FTM_MSG_HIGH("TDSCDMA IQ Capture: start address is stored.", 0, 0, 0);
}

/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_GET_CONFIG

DESCRIPTION
  This API is used to get the IQ capture configuration for TDSCDMA.

DEPENDENCIES
  None

RETURN VALUE
  Always TRUE

SIDE EFFECTS
 None
=============================================================================*/
ftm_iq_capture_error_code_type ftm_tdscdma_iq_capture_get_config(ftm_iq_capture_get_config_type *data)
{
  // Since this command is just to query configuration, return SUCCESS always.
  ftm_iq_capture_error_code_type ret_value=FTM_IQ_SUCCESS;

  data->max_sample_size = FTM_TDSCDMA_IQ_MAX_SAMPLES;
  data->sample_data_format = FTM_TDSCDMA_IQ_SS_DATA_FORMAT;
  data->sampling_freq = FTM_TDSCDMA_IQ_SAMPLE_FREQ_CHIPX2;
  data->max_diag_sample_size = FTM_TDSCDMA_IQ_DIAG_MAX_SAMPLES;

  MSG_4(MSG_SSID_FTM,MSG_LVL_HIGH,"TDSCDMA IQ capture, configuration: Max sample size %d, Sample data format width %d, Sampling freq %lu Hz, Max diag sample size %d",
    data->max_sample_size, data->sample_data_format, data->sampling_freq, data->max_diag_sample_size);

  return ret_value;
}

/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_ACQUIRE_SAMPLES

DESCRIPTION
  This API is used to Acquire samples for TDSCDMA and store the information related to them in a structure. The information include the
  device, carrier and number of samples acquired.

DEPENDENCIES
  RF chain should be configured before acquring. Device and carrier id should be correctly passed.

RETURN VALUE
  FTM_IQ_SUCCESS, - Everything is fine.
  FTM_IQ_GENERAL_FAILURE,  - RF chain has not been configured

SIDE EFFECTS
 None
=============================================================================*/

ftm_iq_capture_error_code_type ftm_tdscdma_iq_capture_acquire_samples(rfcom_device_enum_type device, ftm_iq_capture_acquire_samples_type *data)
{
  ftm_iq_capture_error_code_type ret_value = FTM_IQ_SUCCESS;
  uint32 *raw_data_address = NULL;

  FTM_MSG_HIGH("TDSCDMA IQ Capture, Acquire samples command sent with parameters Device %d, No. of Req. samples %d",
      device, data->sample_size, 0);

  /*Check if IQ data has been captured from FW*/
  if (ftm_tdscdma_iq_acquired_samples_data.data_avail_from_FW == FALSE)
  {
    FTM_MSG_ERROR( "Data hasn't been captured from FW", 0, 0, 0);
    ret_value = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  }

  /*Check if the data_ptr points to IQ data address is available*/
  if (raw_data_start_address == NULL)
  {
    FTM_MSG_ERROR( "data_ptr points to IQ data address isn't available", 0, 0, 0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }

  /*Check if the device to acquire matches the device that has IQ data captured from FW*/
  if (ftm_tdscdma_iq_acquired_samples_data.device_with_IQ != device)
  {
    FTM_MSG_ERROR( "Device to acquire doesn't match the device that has IQ data captured from FW, device to acquire=%d, device with IQ samples=%d",
      device, ftm_tdscdma_iq_acquired_samples_data.device_with_IQ, 0);
    ret_value = FTM_IQ_GENERAL_FAILURE;
  }

  /***********************Do one time initialization if needed***********************************/
  // Allocate memory of Maximum sample size 2 * 2* 8192 bytes.
  if(ftm_tdscdma_iq_acquired_samples_data.iq_buf == NULL)
 {
   ftm_tdscdma_iq_acquired_samples_data.iq_buf = (uint16*)ftm_malloc(2*2*FTM_TDSCDMA_IQ_MAX_SAMPLES);
    // If pointer is NULL even after allocating memory, then declare it as failure
    if(ftm_tdscdma_iq_acquired_samples_data.iq_buf == NULL)
    {
      ret_value = FTM_IQ_GENERAL_FAILURE;
      FTM_MSG_ERROR( "Unable to allocate memory for IQ buffer",0,0,0);
    }
 }


  /**************** Update the variables with requested information  ******************************/
  // Update the strucutre with requested parameters. Start with data_available to be FALSE and change it to TRUE only when are able
  // acquire it sucessfully.
  ftm_tdscdma_iq_acquired_samples_data.data_available = FALSE;
  ftm_tdscdma_iq_acquired_samples_data.device_to_aquire = device;
  ftm_tdscdma_iq_acquired_samples_data.num_req_samples = data->sample_size;

  if ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2))
  {
    raw_data_address = raw_data_start_address;
  }
  else if ((device == RFM_DEVICE_1) || (device == RFM_DEVICE_3))
  {
    raw_data_address = raw_data_start_address_div;
  }

  // Now check if number of IQ samplese requested is > Max it can acquire
  if(ftm_tdscdma_iq_acquired_samples_data.num_req_samples > FTM_TDSCDMA_IQ_MAX_SAMPLES)
  {
	 FTM_MSG_ERROR( "Number of samples requested %d > Max. value %d",
    ftm_tdscdma_iq_acquired_samples_data.num_req_samples, FTM_TDSCDMA_IQ_MAX_SAMPLES, 0);
     ret_value = FTM_IQ_GENERAL_FAILURE;
  }

  /********************* Get samples and copy to acquisition buffer ****************************/
  // Get samples only if we pass all check conditions. Each sample is of 4 bytes I(2 byte) + Q (2 byte)
  if(ret_value == FTM_IQ_SUCCESS)
  {
    // Invalidate previous samples, 2 * 8192 bytes
    memset(ftm_tdscdma_iq_acquired_samples_data.iq_buf, 0, 2*2*FTM_TDSCDMA_IQ_MAX_SAMPLES);
    
    /*Get the samples from share mem with FW to the IQ data buffer allocated - No format conversion required for Jolokia*/
    // Invalidate cache in case FW uses DMA copy
#if !(defined(TEST_FRAMEWORK) && defined(FEATURE_JOLOKIA_MODEM))    
    qurt_mem_cache_clean((qurt_addr_t)raw_data_address, 2*2*FTM_TDSCDMA_IQ_MAX_SAMPLES, QURT_MEM_CACHE_INVALIDATE, 1);    
#endif

#ifdef FEATURE_JOLOKIA_MODEM
    if(NULL != raw_data_address)
    {
      memscpy(ftm_tdscdma_iq_acquired_samples_data.iq_buf,
              2*2*FTM_TDSCDMA_IQ_MAX_SAMPLES,
              raw_data_address,
              2*2*FTM_TDSCDMA_IQ_MAX_SAMPLES);
    }
    #else 
    /*Get the samples from share mem with FW to the IQ data buffer allocated after converting the buffer format*/
    if (rfm_get_calibration_state())
    {
      ftm_tdscdma_util_convert_10pcfl_iq_data_format(raw_data_address,
        ftm_tdscdma_iq_acquired_samples_data.iq_buf, (uint32)(FTM_TDSCDMA_IQ_MAX_SAMPLES));
    }
    else
    {
      ftm_tdscdma_util_convert_20pcfl_iq_data_format(raw_data_address,
        ftm_tdscdma_iq_acquired_samples_data.iq_buf, (uint32)(FTM_TDSCDMA_IQ_MAX_SAMPLES));
    }
    #endif           
  	ftm_tdscdma_iq_acquired_samples_data.data_available = TRUE;
    FTM_MSG_HIGH("%d TDSCDMA IQ samples sucessfully acquired for path %d",
      data->sample_size, device, 0);
  }
  else
  {
    FTM_MSG_ERROR("TDSCDMA IQ samples could not be acquired for path %d, Sample size %d", device, 0, 0);
  }

  return ret_value;

}


/*============================================================================

FUNCTION FTM_TDSCDMA_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get samples for TDSCDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES.

DEPENDENCIES
 The CAPTURE command should have been issued.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_tdscdma_iq_capture_get_samples(rfcom_device_enum_type device, ftm_iq_capture_get_samples_type *data,ftm_rsp_pkt_type *rsp_pkt)
{

  FTM_MSG_HIGH("TDSCDMA IQ Capture: Get samples command sent with parameters Device %d, Sample offset %d, No. of Req. samples %d",
    device, data->sample_offset, data->sample_size);

  /***************** Fill up RSP packet with standard data *****************/
  // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) ? FTM_TDSCDMA_C : FTM_TDSCDMA_RX_2_C ;
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_IQ_CAPTURE_GET_SAMPLES;
  // Start with sucess flag and change it if we fail any check conditions.
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_SUCCESS;
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.sample_offset = data->sample_offset;
  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.sample_size = data->sample_size;


  // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_tdscdma_iq_capture_get_samples_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;


  /***************** Do sanity checks *******************************/

  // First check if data is available
  if (ftm_tdscdma_iq_acquired_samples_data.data_available == FALSE )
  {
    FTM_MSG_ERROR( "IQ samples are not acquired, please acquire them first",0,0,0);
    ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  }

  // Check if IQ has been acquired for the same device.
  if (ftm_tdscdma_iq_acquired_samples_data.device_to_aquire != device)
  {
    FTM_MSG_ERROR( "IQ samples are not acquired for this device, please acquire them first",0,0,0);
	  ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code = FTM_IQ_IQ_SAMPLES_UNAVAILABLE;
  }


  // Check if number of samples requested is greater than acquired.
  if( (data->sample_offset + data->sample_size) > ftm_tdscdma_iq_acquired_samples_data.num_req_samples )
  {
     FTM_MSG_ERROR( "Sample offset or Sample size is wrong %d %d",data->sample_offset,data->sample_size,0);
     ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code =  FTM_IQ_ILLEGAL_COMBIMATION;
  }

  // check if requested number of samples > Max. value that can be sent through Diag
  if( data->sample_size > FTM_TDSCDMA_IQ_DIAG_MAX_SAMPLES )
  {
    FTM_MSG_ERROR( "Requested Sample size %d is greater than %d", data->sample_size, FTM_TDSCDMA_IQ_DIAG_MAX_SAMPLES,0);
    ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code =  FTM_IQ_GENERAL_FAILURE;
  }


   /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/
  // If all the sanity checks are passed, we will be sending IQ samples in Response packet.
  if(ftm_tdscdma_iq_capture_get_samples_rsp_pkt.ftm_error_code == FTM_IQ_SUCCESS)
  {
    // Since we met all the check conditions, copy the data.
    // Since extracted data is of format:  First sample: I(16 bits) and then second sample Q (16 bits)
    //    a) Sample offset x corresponds to 2*x element in the array
    //    b) Sample size y corresponds to 2*y elements (I + Q) => 2*2*y bytes
    memscpy(ftm_tdscdma_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_16_bit,
    sizeof(ftm_tdscdma_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_16_bit),
    &(ftm_tdscdma_iq_acquired_samples_data.iq_buf[(2*(data->sample_offset))]),
    2*2*data->sample_size);

    rsp_pkt->pkt_len = FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE + 2* sizeof(word) * data->sample_size; // Header size +   (I + Q , each 1 word) * total number

    FTM_MSG_HIGH("%d TDSCDMA IQ samples sent to Diag with sample offset %d", data->sample_size, data->sample_offset,0);

  }
  // If we fail checks, dont fill IQ samples. RSP packet will only have header.
  else
  {
    rsp_pkt->pkt_len =  FTM_IQ_CAPTURE_GET_SAMPLE_HDR_SIZE;
    FTM_MSG_ERROR("TDSCDMA IQ samples Capture failed for SAMPLE SIZE %d and SAMPLE OFFSET %d",data->sample_size,data->sample_offset,0);
  }
}

/*============================================================================

FUNCTION FTM_TDSCDMA_FREE_MEMORY

DESCRIPTION
  This API is used to free up the memory allocated in TDSCDMA FTM mode. This API will be generally called while exiting the mode

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_tdscdma_free_memory(void)
{

  // Check if memory has been allocated to acquire IQ samples.
  if(ftm_tdscdma_iq_acquired_samples_data.iq_buf != NULL)
  {
    ftm_free(ftm_tdscdma_iq_acquired_samples_data.iq_buf);
	ftm_tdscdma_iq_acquired_samples_data.iq_buf = NULL;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handle the response from FW for FTM_TX command in RF mode

  @details
  This command waits for MSGR response from FW to indicate completion of FTM_TX
  command processing by FW. This API should be clubbed with rf_tdscdma_mc_burst_tx()
  API to ensure handling of FW response.

*/
boolean ftm_tdscdma_rf_mode_tx_cmd_handler(void)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */
  uint8 timeout_counter = 0;
  rf_tdscdma_rfmode_tx_cmd_rsp_status_type cmd_rsp = TDSCDMA_RFMODE_TX_CMD_RSP_MAX;

  while (timeout_counter <= FTM_TDSCDMA_RF_MODE_TX_CMD_TIMEOUT)
  {
    /* Get the MSGR command status */
    cmd_rsp = rf_tdscdma_mc_get_burst_tx_cmd_status();
    if (cmd_rsp == TDSCDMA_RFMODE_TX_CMD_RSP_PENDING)
    {
      /* If a response is pending from FW, continue waiting till timeout */
      ftm_clk_rex_wait(1);
    }
    else if(cmd_rsp == TDSCDMA_RFMODE_TX_CMD_RSP_SUCCESS)
    {
      error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
      FTM_MSG_LOW("FTM TX CMD RSP Success. Timeout counter: %d", timeout_counter, 0, 0);
      break;
    }
    else
    {
      FTM_MSG_LOW("Timeout counter: %d", timeout_counter, 0, 0);
      break;
    }
    timeout_counter++;
  }

  if (timeout_counter > FTM_TDSCDMA_RF_MODE_TX_CMD_TIMEOUT)
  {
    FTM_MSG_ERROR("No response from mDSP FW for RF Mode TX MSGR command (timed out)!", 0, 0, 0);
  }

  rf_tdscdma_mc_set_burst_tx_cmd_status(TDSCDMA_RFMODE_TX_CMD_RSP_IDLE);
  return error_code;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Handle the response from FW for FTM_RX command in RF mode

  @details
  This command waits for MSGR response from FW to indicate completion of FTM_RX
  command processing by FW. This API should be clubbed with rf_tdscdma_mc_burst_rx()
  API to ensure handling of FW response.

*/
boolean ftm_tdscdma_rf_mode_rx_cmd_handler(void)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */
  uint8 timeout_counter = 0;
  rf_tdscdma_rfmode_rx_cmd_rsp_status_type cmd_rsp = TDSCDMA_RFMODE_RX_CMD_RSP_MAX;

  while (timeout_counter <= FTM_TDSCDMA_RF_MODE_RX_CMD_TIMEOUT)
  {
    /* Get the MSGR command status */
    cmd_rsp = rf_tdscdma_mc_get_burst_rx_cmd_status();
    if (cmd_rsp == TDSCDMA_RFMODE_RX_CMD_RSP_PENDING)
    {
      /* If a response is pending from FW, continue waiting till timeout */
      ftm_clk_rex_wait(1);
    }
    else if(cmd_rsp == TDSCDMA_RFMODE_RX_CMD_RSP_SUCCESS)
    {
      error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
      FTM_MSG_LOW("FTM RX CMD RSP Success. Timeout counter: %d", timeout_counter, 0, 0);
      break;
    }
    else
    {
      FTM_MSG_LOW("Timeout counter: %d", timeout_counter, 0, 0);
      break;

    }
    timeout_counter++;
  }

  if (timeout_counter > FTM_TDSCDMA_RF_MODE_RX_CMD_TIMEOUT)
  {
    FTM_MSG_ERROR("No response from mDSP FW for RF Mode RX MSGR command (timed out)!", 0, 0, 0);
  }

  rf_tdscdma_mc_set_burst_rx_cmd_status(TDSCDMA_RFMODE_RX_CMD_RSP_IDLE);
  return error_code;
}

/*===========================================================================
FUNCTION ftm_tdscdma_rf_mode_blocking_cmd_handler

DESCRIPTION
   This function used to handle all the blocking cmd under FTM-RF

DEPENDENCIES
   None.

RETURN VALUE
   Boolean, FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS if cmd return successful,
   FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE if the cmd returns failure, or even
   not return until the timeout threshold


SIDE EFFECTS
   None.

===========================================================================*/
boolean ftm_tdscdma_rf_mode_blocking_cmd_handler(rf_tdscdma_mdsp_msg_t ftm_rf_cmd_msg)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
  uint8 timeout_counter = 0;
  uint8 timeout_threshold = 0;
  rf_tdscdma_rfmode_cmd_rsp_status_type cmd_rsp = TDSCDMA_RFMODE_CMD_RSP_MAX;

  switch(ftm_rf_cmd_msg)
  {
        /*Get PLL Status*/
	    case RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG:
			timeout_threshold = FTM_TDSCDMA_RF_MODE_GET_MULTI_SYNTH_TIMEOUT;
			break;


	    /*IQ capture*/
    case RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG:
	  timeout_threshold = FTM_TDSCDMA_RF_MODE_IQ_CAPTURE_TIMEOUT;
	  break;

    default :
	  FTM_MSG_ERROR("This command: %d has not been supported as a blocking one", ftm_rf_cmd_msg,0,0);
	  return FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE;
  }

  while (timeout_counter <= timeout_threshold)
  {
    /* Get the MSGR command status */
    cmd_rsp = rf_tdscdma_mc_get_blocking_rsp_status(ftm_rf_cmd_msg);
    if (TDSCDMA_RFMODE_CMD_RSP_PENDING  == cmd_rsp)
    {
      /*If a response is pending from FW, continue waiting till time out*/
      ftm_clk_rex_wait(1);
    }
    else if(TDSCDMA_RFMODE_CMD_RSP_SUCCESS== cmd_rsp)
    {
      error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
      FTM_MSG_LOW("FTM RF Blocking Cmd %d Success, Timeout Counter, %d", ftm_rf_cmd_msg, timeout_counter, 0);
      break;
    }
    else
    {
      FTM_MSG_LOW("Timeout counter: %d", timeout_counter,0,0);
      break;
    }
    timeout_counter++;
  }

  if (timeout_counter > timeout_threshold)
  {
    FTM_MSG_ERROR("No response from mDSP FW for RF Mode Cmd %d (time out)", ftm_rf_cmd_msg,0,0);
  }

  rf_tdscdma_mc_set_blocking_cmd_rsp_status(ftm_rf_cmd_msg,TDSCDMA_RFMODE_CMD_RSP_IDLE);
  return error_code;

}

/*============================================================================

FUNCTION ftm_tdscdma_do_rsb_cal

DESCRIPTION
  This fuction performs rsb calibration. The single tone generator channel is
  configured to provide a tone input to the lna of the transceiver. IQ samples
  are captured for this tone signal with the iq mismatch coefficients zeroed
  out. IQ samples are processed using rsb algorithm to obtain the rsb a and
  rsb b coefficients. These values are stored in nv items for future access.

DEPENDENCIES
  None

RETURN VALUE
  boolean value indicating the status as TRUE or FALSE for success or failure

SIDE EFFECTS

=============================================================================*/
boolean ftm_tdscdma_do_rsb_cal
(
  ftm_rsb_txlo_rsp_type *rsp_pkt,
  boolean override_stg
)
{
#ifdef FEATURE_TRITON_MODEM
  FTM_MSG_ERROR( "TRITON does not support ftm_tdscdma_do_rsb_cal!!!", 0,0,0);
  return FALSE;

#else /* if not FEATURE_TRITON_MODEM*/

  uint16                        stg_channel=ftm_tdscdma_current_chan;
  rfcom_tdscdma_band_type       rf_band, rf_band_stg;
  rf_time_tick_type             prof_rsb_cal;
  rf_time_type                  prof_rsb;
  uint16 therm_data = 0;
  int16  rsb_a = 0, rsb_b = 0;
  uint64 i_sq = 0, q_sq = 0;
  int64  i_q  = 0;
  int16  sin_theta_final=0, cos_theta_final=0, gain_inv_final=0;

  /* override_stg = True means using external signal */
  ftm_tds_iq_capture_data.OverrideStg = override_stg;

  /* Check for invalid band input */
  rf_band = (rfcom_tdscdma_band_type)(rsp_pkt->curr_band - FTM_PHONE_MODE_TDSCDMA_B34);
  if ( rf_band >= RFCOM_NUM_TDSCDMA_BANDS )
  {
    rsp_pkt->cal_status = FTM_UNSUPPORTED_BAND;
    return FALSE;
  }

  /* Enter/set TDSCDMA mode for primary chain */
  ftm_tdscdma_set_mode( rsp_pkt->curr_band );
  /* Enter/set TDSCDMA mode for secondary chain */
  ftm_tdscdma_set_secondary_chain( TRUE );

  /* --------------------------------------------------------------------
   Query STG channel
  ---------------------------------------------------------------------- */
  /* Time profiling */
  prof_rsb_cal = rf_time_get_tick();

  /* Get the STG channel information from device */
  rfdevice_tdscdma_rx_get_stg_channel(ftm_tdscdma_current_pri_device,
                                      rf_band,
                                      &stg_channel,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);
  FTM_MSG_LOW("got STG Channel=%d from Band=%d, Time taken=%d",
              stg_channel, rf_band, prof_rsb);

  /* If STG Channel is incorrect then report Unsupported Band */
  rf_band_stg = rf_tdscdma_core_util_get_band_from_uarfcn(stg_channel);

  /* doing further check on the band-channel matching */
  if( rf_band_stg == RFCOM_BAND_TDSCDMA_INVALID ||
      rf_band_stg != rf_band
    )
  {
    FTM_MSG_ERROR( "ftm_tdscdma_do_rsb_cal: wrong stg channel %d from band %d, returned band= %d",
                   stg_channel,
                   rf_band,
                   rf_band_stg);
    return FALSE;
  }
  /*------------------------------------------------------------------
  The following section is to do IQ capture with the STG channel
  --------------------------------------------------------------------*/
  prof_rsb_cal = rf_time_get_tick();

  /* It is required to change fw state for starting iq capture */
//  ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_STANDBY);
//  FTM_MSG_HIGH("ftm_tdscdma_do_rsb_cal: send TFW to STANDBY", 0, 0, 0);
//  ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_FTM_TRK);
//  FTM_MSG_HIGH("ftm_tdscdma_do_rsb_cal: send TFW to FTM_TRK", 0, 0, 0);

  /* disable tx in order to turn rx */
  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
  {
    rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
  }

  /* Tune Radio & Preparing Shard Memory for RXLM config */
  rf_tdscdma_mc_enable_rx(ftm_tdscdma_current_pri_device, 
                          ftm_tdscdma_current_sec_device,
                          stg_channel,
                          ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                          RFA_TDSCDMA_RXLM_MODE_TRK,
#ifdef FTM_HAS_TDSCDMA_RX_DIVERSITY
                          RFA_TDSCDMA_RX_PATH_BOTH, // RFA_TDSCDMA_RX_PATH_PRIM,
                          ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx,
                          RFA_TDSCDMA_RXLM_MODE_TRK,
#endif
                          NULL, 
                          NULL,
                          FALSE, // is_dsds
                          ftm_tdscdma_rx_lin_state,
                          TRUE   // always use SW tune in ACQ
                          );

   /* Clear IQMC before the RSB calibration */
   rf_tdscdma_msm_clear_iqmc_rxlm(ftm_tdscdma_current_pri_device,
                                  stg_channel,
                                  ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);

   rf_tdscdma_msm_clear_iqmc_rxlm(ftm_tdscdma_current_sec_device,
                                  stg_channel,
                                  ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);

  /* Set the LNA range to G0 */
  //ftm_tdscdma_set_lna_range(ftm_tdscdma_get_rx_chain_handle(RFM_DEVICE_0), 0 );
  //ftm_tdscdma_set_lna_range(ftm_tdscdma_get_rx_chain_handle(RFM_DEVICE_1), 0 );
  //FTM_MSG_HIGH("ftm_tdscdma_do_rsb_cal: set_lna_range to G0", 0, 0, 0);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);
  FTM_MSG_LOW("rx enable time =%d,", prof_rsb,0,0);

  /* Configure the single tone generator */
  prof_rsb_cal = rf_time_get_tick();
  if ( ftm_tds_iq_capture_data.OverrideStg == FALSE )
  {
    rfdevice_tdscdma_rx_configure_stg(ftm_tdscdma_current_pri_device,  //PRX
                                      rf_band,
                                      NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL);
    rfdevice_tdscdma_rx_configure_stg(ftm_tdscdma_current_sec_device,  //DRX
                                      rf_band,
                                      NULL,
                                      RFDEVICE_EXECUTE_IMMEDIATE,
                                      NULL);
  }
  DALSYS_BusyWait(4000);
  DALSYS_BusyWait(5000);

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);
  FTM_MSG_LOW("STG Configure time= %d ", prof_rsb,0,0 );

  /* Set IQMC coeff's to zero */
  //prx_status &= rf_tdscdma_msm_reset_iqmc_rxlm((rfm_device_enum_type)FTM_RECEIVE_CHAIN_0,
  //                                           ,
  //                                           RFCOM_SINGLE_CARRIER,
  //                                           ftm_tdscdma_rxlm_buffer[RFM_DEVICE_0].buf_idx);

  prof_rsb_cal = rf_time_get_tick();

  /* It is required to reconfig fw state for starting iq capture */
  ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_STANDBY);
  FTM_MSG_HIGH("ftm_tdscdma_do_rsb_cal: send TFW to STANDBY", 0, 0, 0);
  ftm_tdscdma_config_fw_state(RF_TDSCDMA_MDSP_TFW_STATE_FTM_TRK);
  FTM_MSG_HIGH("ftm_tdscdma_do_rsb_cal: send TFW to FTM_TRK", 0, 0, 0);

  /* Asking FW to do RXLM Config on both chains*/
  rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXLM_CFG_MSG,ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx);
  FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW RXLM CFG. buf_idx = %d",
                ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx, 0, 0);
  rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_RXDIVLM_CFG_MSG,(ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx);
  FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send TFW RXLM_RXD CFG. buf_idx = %d",
                (ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx<<4) + ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_1].buf_idx, 0, 0);

  /* Sending iq capture command */
  ftm_tds_iq_capture_data.IqCaptureStatus = FTM_TDSCDMA_IQ_CAPTURE_NO_RSP; //0=clear the flag at first
  ftm_tds_iq_capture_data.MaxWaitCnt =  2000;   //todo: change back to 20
  rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_IQ_CAPTURE_CFG_MSG,((ftm_tds_iq_capture_data.SampleSize << 16) & 0xFFFF0000));
  FTM_MSG_HIGH("ftm_tdscdma_tune_to_chan: send IQ capture cmd to TFW: sample = %d",
                ftm_tds_iq_capture_data.SampleSize, 0, 0);

  /* Waiting for the result from TFW return message */
  while ( (ftm_tds_iq_capture_data.IqCaptureStatus == FTM_TDSCDMA_IQ_CAPTURE_NO_RSP) &&
          (ftm_tds_iq_capture_data.MaxWaitCnt != 0) )
  {
    DALSYS_BusyWait(1000); // 1ms resolution
    ftm_tds_iq_capture_data.MaxWaitCnt -- ;
  }

  if ( ftm_tds_iq_capture_data.IqCaptureStatus == FTM_TDSCDMA_IQ_CAPTURE_SUCCEED )
  {
    /* Read one therm value */
    rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
                                     RFCOM_TDSCDMA_MODE,
                                     rf_band,
                                     RFDEVICE_GET_THERMISTER_VALUE,
                                     &therm_data);
    /* PRX RSB processing */
    rfcommon_msm_compute_rsb_coeff_from_iq ( ftm_tds_iq_capture_data.startAddrPrx,
                                             ftm_tds_iq_capture_data.SampleSize,
                                             &rsb_a,
                                             &rsb_b,
                                             &i_sq,
                                             &q_sq,
                                             &i_q
                                            );

     rfdevice_tdscdma_rx_get_rsb_nv_params(ftm_tdscdma_current_pri_device,
                                            rf_band,
                                            i_sq,
                                            q_sq,
                                            i_q,
                                            therm_data,
                                            &sin_theta_final,
                                            &cos_theta_final,
                                            &gain_inv_final
                                            );

    /* DRX RSB processing */
    rfcommon_msm_compute_rsb_coeff_from_iq(ftm_tds_iq_capture_data.startAddrDrx,
                                           ftm_tds_iq_capture_data.SampleSize,
                                           &rsb_a,
                                           &rsb_b,
                                           &i_sq,
                                           &q_sq,
                                           &i_q
                                           );

    rfdevice_tdscdma_rx_get_rsb_nv_params(ftm_tdscdma_current_sec_device,
                                          rf_band,
                                          i_sq,
                                          q_sq,
                                          i_q,
                                          therm_data,
                                          &sin_theta_final,
                                          &cos_theta_final,
                                          &gain_inv_final
                                          );
  }

  prof_rsb = rf_time_get_elapsed(prof_rsb_cal, RF_USEC);
  FTM_MSG_LOW("IQ capture + RSB processing time = %d us, iq_capture_status = %d",
              prof_rsb, ftm_tds_iq_capture_data.IqCaptureStatus, 0);

  /* disable STG */
  if ( ftm_tds_iq_capture_data.OverrideStg == FALSE )
  {
    rfdevice_tdscdma_rx_disable_stg(ftm_tdscdma_current_pri_device,
                                    rf_band,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);
    rfdevice_tdscdma_rx_disable_stg(ftm_tdscdma_current_sec_device,
                                    rf_band,
                                    RFDEVICE_EXECUTE_IMMEDIATE,
                                    NULL);
  }
  FTM_MSG_LOW("STG was disabled on both PRX and DRX",0,0,0);


  //FTM_MSG_LOW("RSB calTherm read = %d", therm_data,0,0);

  return TRUE;

#endif /*FEATURE_TRITON_MODEM*/
} /* ftm_tdscdma_do_rsb_cal() */

boolean ftm_tdscdma_xpt_cal_config
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_mode,
  boolean config
)
{
  boolean ret_val = TRUE;
  lm_tech_type tech = LM_TDSCDMA;
  lm_status_type status;
  mcpm_mcvsrequest_parms_type params;

  rfc_device_info_type *device_info_ptr = NULL;
  rfc_cfg_params_type cfg;
  rfdevice_coupler_tech_cfg_type coupler_cfg;
  rfdevice_rxtx_common_class *device_ptr = NULL;/*rfc_common_get_cmn_device_object(0);*/
  rfdevice_trx_tdscdma_tx** tds_device_ptr = NULL;

  /* Find out common device pointer from the 1st Tx device */
  tds_device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(
                              device,
                              RFC_CONFIG_TX, 
                              ftm_tdscdma_current_mode, 
                              RFDEVICE_TRANSCEIVER);
  if (tds_device_ptr != NULL)
  {
    device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device_ptr[0]);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. Tx band: %d", 
          ftm_tdscdma_current_mode);
    return FALSE;
  }
  
  cfg.alternate_path = 0;
  cfg.band = ftm_tdscdma_current_mode;
  cfg.req = config ? RFC_REQ_ENABLE_XPT_CAPTURE : RFC_REQ_DISABLE_XPT_CAPTURE;
  cfg.logical_device = device_fb;
  cfg.rx_tx= RFC_CONFIG_TX;

  rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr);

  if (device_info_ptr == NULL)
  {
    MSG_ERROR("RFC TDSCDMA device_info_ptr is Null",0,0,0);
    return(FALSE);
  }

  /* xPT Cal config */
  if (config)
  {
    switch ( (ftm_xpt_config_type)xpt_mode )
    {
    case FTM_XPT_CONFIG_FBRX: // start config FBRX_MODE
	 
      if (!ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated)
      {
	      ftm_tdscdma_xpt_fb_path_buffer.buf_idx = rfcommon_fbrx_mc_get_tech_handle( RFM_TDSCDMA_MODE );
        if( ftm_tdscdma_xpt_fb_path_buffer.buf_idx != -1 )
        {
          ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated = TRUE;
        }
        else
        {
          MSG_ERROR("xPT feedback path buffer allocate failed, status:%d", 0, 0, 0);
         return FALSE;
        }
      }
      break;
    default:
      /* if secondary chain is enabled, turn it off first */
      if (ftm_tdscdma_rxlm_buffer[RF_PATH_1].is_buf_allocated &&
          ((rf_path_enum_type)device_fb ==RF_PATH_1))
      {
        ftm_tdscdma_set_secondary_chain(FALSE);
      }

      if (!ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated)
      {
        /* if the xpt fb path not config, go ahead allocate RxLM buffer
           and config the feedback path */
        status = rxlm_allocate_buffer((rxlm_chain_type)device_fb, tech,
                                      &(ftm_tdscdma_xpt_fb_path_buffer.buf_idx));

        if (status != LM_SUCCESS )
        {
           MSG_ERROR("xPT feedback path buffer allocate failed, status:%d",status,0,0);
           return FALSE;
        }
        else
        {
          MSG_HIGH("xPT feedback path buffer allocated for chain:%d, buffer index:%d",
                   device_fb, ftm_tdscdma_xpt_fb_path_buffer.buf_idx, 0);
        }
        ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated = TRUE;

        if(NULL != rfc_tdscdma_get_rf_device_obj(device, 
                                                 RFC_CONFIG_TX,
                                                 ftm_tdscdma_current_mode, 
                                                 RFDEVICE_COUPLER))
        {
          coupler_cfg.mode = RFCOM_TDSCDMA_MODE;
          coupler_cfg.band.tdscdma_band = ftm_tdscdma_current_mode;
          coupler_cfg.direction = RFDEVICE_COUPLER_DIRECTION_FWD;
          coupler_cfg.port = 0;
          ret_val &= rfdevice_tdscdma_coupler_config(device,
                                                     coupler_cfg,
                                                     NULL,
                                                     RFDEVICE_EXECUTE_IMMEDIATE,
                                                     0);
        }

        /* Acquire handle lock */
        ret_val &= ( rxlm_acquire_lock(ftm_tdscdma_xpt_fb_path_buffer.buf_idx) == LM_SUCCESS );

        /* Step1: Configure RxLm feedback path */
        ret_val &= rf_tdscdma_msm_set_xpt_state_rxlm(ftm_tdscdma_xpt_fb_path_buffer.buf_idx);

        /* modem chain update necessary for DIME RxLM settings */
        ret_val &= rf_tdscdma_msm_update_dynamic_rxlm_buffer_xpt(ftm_tdscdma_xpt_fb_path_buffer.buf_idx,(rfm_device_enum_type)device_fb,
                                                                 ftm_tdscdma_current_mode);

        /* Release handle lock */
        ret_val &= ( rxlm_release_lock(ftm_tdscdma_xpt_fb_path_buffer.buf_idx) == LM_SUCCESS );

        /* Step2: Enable WTR Tx PLL for feedback */

        /* Step3: Set WTR ET Path */
        ret_val &= rfc_tdscdma_setup_fb_path(device_fb,
                                             ftm_tdscdma_current_mode,
                                             TRUE);

        ret_val &= rfcommon_fbrx_mc_enable_rf_dev(device_ptr, 2);

        ret_val &= rfc_tdscdma_setup_fb_device(device,
                                               device_fb,
                                               ftm_tdscdma_current_mode,
                                               ftm_tdscdma_current_chan, 
                                               TRUE);


        /* Step4: Vote modem bus clk to 144 MHz */
        /* Setup offline clock to 144MHz for EPT Sample capture */
        params.mcvs_req_type = MCVS_FULL_REQUEST;
        params.modem_clk_bus_KHz = 144000;
        params.mcvs_update_info = MCPM_MCVS_CLKBUS_UPDATE;
        MCPM_MCVSConfig_Modem(MCPM_TDSCDMA_TECH, &params);
        MSG_1(MSG_SSID_RF, MSG_LVL_HIGH, "Offline speed returned by MCPM:%d", MCPMDRV_Get_Modem_Offline_Clock(MCPM_TDSCDMA_TECH));

        FTM_MSG_MED("xPT Cal config status %d.",ret_val,0,0);

      }/* if (!ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated) */
    }/* switch */
  } /* if (config) */
  else /* xPT Cal Deconfig */
  {
    switch ( (ftm_xpt_config_type)xpt_mode )
    {
    case FTM_XPT_CONFIG_FBRX: // start deconfig FTM_XPT_CONFIG_FBRX
	    ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated = FALSE;	  
      break; // end deconfig for FTM_XPT_CONFIG_FBRX
    default:
    /* Unvote modem bus clk to 144 MHz */
    MCPM_MCVSConfig_Release(MCPM_TDSCDMA_TECH);

    if (ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated)
    {
      status = rxlm_deallocate_buffer(ftm_tdscdma_xpt_fb_path_buffer.buf_idx);
      if (status != LM_SUCCESS )
      {
        MSG_ERROR("xPT feedback path buffer deallocate failed, status:%d",status,0,0);
      }
      else
      {
        MSG_HIGH("xPT feedback path buffer deallocated for chain:%d",device,0,0);
        ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated = FALSE;
      }
    } /* if (ftm_tdscdma_xpt_fb_path_buffer.is_buf_allocated) */

    /* Turn off WTR ET Path */
    ret_val &= rfc_tdscdma_setup_fb_path(device_fb,
                                         ftm_tdscdma_current_mode,
                                         FALSE);

    /* Turn off FBRx during XPT deconfig */
    ret_val &= rfcommon_fbrx_mc_disable_rf_dev(device_ptr);
    
    FTM_MSG_MED("Turn OFF FBRx during XPT deconfig", 0,0,0);

    ret_val &= rfc_tdscdma_setup_fb_device(device,
                                           device_fb,
                                           ftm_tdscdma_current_mode,
                                           ftm_tdscdma_current_chan, 
                                           FALSE);

    FTM_MSG_MED("xPT Cal Deconfig status %d.",ret_val,0,0);
    }

  }/* xPT Cal Deconfig */
  return ret_val;

}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override TXAGC parameters

  @details
  This function will override TXAGC params.

  @param device
  The device to perform override on.

  @param txagc_oride_parms
  The parameter values to override.

  @return
  Return if override was successful or not.
*/
boolean ftm_tdscdma_xpt_override_txagc
(
  rfm_device_enum_type device,
  uint8  xpt_mode,
  uint8  amam_bank_idx,
  uint16 amam_start_offset,
  uint8  amam_tbl_size,
  uint8  ampm_bank_idx,
  uint16 ampm_start_offset,
  uint8  ampm_tbl_size,
  uint8  epdt_bank_idx,
  uint16 epdt_start_offset,
  uint8  epdt_tbl_size,
  uint8  iq_gain_action_type,
  uint32 iq_gain,
  uint8  env_scale_action_type,
  uint32 env_scale,
  uint16 rgi,
  uint16 smps_bias,
  uint8  pa_state,
  uint16 pa_curr,
  uint8  delay_action_type,
  int32  delay
)
{
  boolean status = TRUE;
  /* common override parameters */
  rfcommon_mdsp_txagc_oride_params_type mdsp_ovr_params;

  /* QPOET variables */
  rfdevice_papm_mode_type qpoet_mode;
  uint8 qpoet_xpt_mode;
  uint8 pa_range;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type rfc_cfg;
  rfnv_tdscdma_tx_tbl_type *tx_nv_tbl_ptr = NULL;
  /* for common RGI interface */
  rfdevice_tx_lut_data_type lut_data;
  rfcom_band_type_u tech_band;

  /*KW fix */
  memset (&mdsp_ovr_params, 0, sizeof( rfcommon_mdsp_txagc_oride_params_type));

  tx_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);

  /* Check for valid PA state */
  if ( pa_state != FTM_COMMON_XPT_TX_OVR_PA_STATE_INVALID )
  {
    pa_range = rf_tdscdma_core_util_get_pa_range_from_nv(pa_state);
    /* Set PA Range */
    rfdevice_tdscdma_pa_set_gain_range(device,
                                       ftm_tdscdma_current_mode,
                                       pa_range,
                                       NULL,
                                       RFDEVICE_EXECUTE_IMMEDIATE,
                                       RFCOM_INVALID_TIMING_OFFSET);

    /* Turn on the PA */
    rfdevice_tdscdma_pa_on_off(device,
                               ftm_tdscdma_current_mode,
                               TRUE,
                               NULL,
                               RFDEVICE_EXECUTE_IMMEDIATE,
                               RFCOM_INVALID_TIMING_OFFSET);

    ftm_tdscdma_cur_pa_range = pa_range;
  }

  /* RGI override */
  /* Check for RGI validity */
  if ( rgi != FTM_COMMON_XPT_TX_OVR_RGI_INVALID )
  {
    /* Find out common device pointer from the 1st Tx device */
    rfdevice_rxtx_common_class *device_ptr = NULL;/*rfc_common_get_cmn_device_object(0);*/
    rfdevice_trx_tdscdma_tx** tds_device_ptr = NULL;
    
    tech_band.tdscdma_band = ftm_tdscdma_current_mode;

    tds_device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(
                                 device,
                                 RFC_CONFIG_TX, 
                                 ftm_tdscdma_current_mode, 
                                 RFDEVICE_TRANSCEIVER);

    if (tds_device_ptr != NULL)
    {
      device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device_ptr[0]);
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. Tx band: %d", 
            ftm_tdscdma_current_mode);
      return FALSE;
    }

    if ( NULL != device_ptr )
    {
      if ( rfdevice_cmn_is_cmn_tx_intf_supported( device_ptr ) )
      {
        /* common TxAGC API */
        status &=
             rfdevice_cmn_get_tx_lut_scripts(device_ptr, 
                                             RFM_TDSCDMA_MODE,
                                             tech_band,
                                             pa_state, 
                                             rgi,
                                             RFDEVICE_EXECUTE_IMMEDIATE,
                                             &lut_data ) ;
        /* Also update IREF data for baseband override */
        mdsp_ovr_params.iref_uamp_x10 = lut_data.iref_uamp_x10;
        //RF_MSG ( RF_HIGH , "Common TxAGC to configure TxAFC! " );
        MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, 
              "Common API to configure TxAGC rgi=%d, paState=%d",
              rgi, pa_state);
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
              "error: rfdevice_cmn_tx_intf is not supported!, band=%d",
              ftm_tdscdma_current_mode);
      }
    }
    else
    {
       MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
             "ftm_tdscdma_xpt_override_txagc(): device_ptr is NULL. band=%d",
             ftm_tdscdma_current_mode) ;
    }

    ftm_tdscdma_cur_tx_agc_adj_pdm = rgi;
  }

  /* Check for PA current validity */
  if ( pa_curr != FTM_COMMON_XPT_TX_OVR_PA_CURR_INVALID )
  {
    /* TODO - do we need to set PA current on QFE2340? */
#if 0
    /* Program PA current_bias*/
    status &= rfdevice_tdscdma_pa_set_current_bias
                            (device,
                             ftm_tdscdma_current_mode,
                             lut_info.pa_range,
                             pa_curr, //in pdm - value ranges from 0 to 256
                             FALSE,
                             NULL,
                             RFDEVICE_EXECUTE_IMMEDIATE,
                             RFCOM_INVALID_TIMING_OFFSET);
#endif
  }

  /* get modem chain */
  rfc_cfg.alternate_path = 0;
  rfc_cfg.band = ftm_tdscdma_current_mode;
  rfc_cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_cfg.logical_device = device;
  rfc_cfg.rx_tx = RFC_CONFIG_TX;
  if (rfc_tdscdma_get_devices_info_data(&rfc_cfg, &device_info_ptr) == TRUE)
  {
    mdsp_ovr_params.modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot get device config %d, band %d",
                                           device, ftm_tdscdma_current_mode);
    return FALSE;
  }

  /* Populate the MSM override data */
  mdsp_ovr_params.amam_addr.mem_bank_id = amam_bank_idx;
  mdsp_ovr_params.amam_addr.start_offset = amam_start_offset;
  mdsp_ovr_params.amam_addr.tbl_size = amam_tbl_size;
  mdsp_ovr_params.ampm_addr.mem_bank_id = ampm_bank_idx;
  mdsp_ovr_params.ampm_addr.start_offset = ampm_start_offset;
  mdsp_ovr_params.ampm_addr.tbl_size = ampm_tbl_size;
  mdsp_ovr_params.detrough_addr.mem_bank_id = epdt_bank_idx;
  mdsp_ovr_params.detrough_addr.start_offset = epdt_start_offset;
  mdsp_ovr_params.detrough_addr.tbl_size = epdt_tbl_size;
  mdsp_ovr_params.xpt_mode = xpt_mode;
  mdsp_ovr_params.latch_immediate = FALSE;
  mdsp_ovr_params.max_sample_count = 0;
  mdsp_ovr_params.rflm_handle = ftm_tdscdma_txlm_buffer.buf_idx;

  /* Decide on IQ gain override type */
  switch ( iq_gain_action_type )
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.iq_gain = iq_gain;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
    mdsp_ovr_params.iq_gain = rf_tdscdma_msm_get_default_txc_gain(
                                                 ftm_tdscdma_txlm_buffer.buf_idx);
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
  {
    if (tx_nv_tbl_ptr != NULL)
    {
      mdsp_ovr_params.iq_gain = (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_IQ_GAIN_UNITY];
    }
    else
    {
      mdsp_ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    }
    break;
  }

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    break;
  }

  /* Decide on Env Scale override type */
  switch ( env_scale_action_type )
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.env_scale = env_scale;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
  {
    if (tx_nv_tbl_ptr != NULL)
    {
      if( xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT )
      {
        mdsp_ovr_params.env_scale =
          (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
      }
      else if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
      {
        mdsp_ovr_params.env_scale =
          (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
      }
    }
    else
    {
      mdsp_ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
    }
    break;
  }

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
    break;
  }

  /* Decide on delay value override type */
  if (delay_action_type != FTM_COMMON_XPT_TX_OVERRIDE_SKIP)
  {
    FTM_MSG_HIGH("TDS delay_action_type is not SKIP %d!", delay_action_type, 0, 0);
  }
  switch ( delay_action_type )
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    mdsp_ovr_params.delay = delay;
    mdsp_ovr_params.delay_txlm_handle = ftm_tdscdma_txlm_buffer.buf_idx;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
  {
    if (tx_nv_tbl_ptr != NULL)
    {
      // tds doesn't have the et_delay NV
      //mdsp_ovr_params.delay = tx_nv_tbl_ptr->et_delay_value;
      mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
      mdsp_ovr_params.delay_txlm_handle = ftm_tdscdma_txlm_buffer.buf_idx;
    }
    else
    {
      mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
      mdsp_ovr_params.delay_txlm_handle = 0;
    }
    break;
  }

  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
    mdsp_ovr_params.delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;
    mdsp_ovr_params.delay_txlm_handle = 0;
    break;
  }

  /* Program QPOET */
  /* Get Power tracker object from RFC */
  qpoet_xpt_mode = (uint8)xpt_mode;
  switch ( qpoet_xpt_mode )
  {
    case RFCOMMON_MDSP_XPT_MODE_APT:
    case RFCOMMON_MDSP_XPT_MODE_EPT:
      qpoet_mode = RFDEVICE_APT_MODE;
      break;
    case RFCOMMON_MDSP_XPT_MODE_ET:
      qpoet_mode = RFDEVICE_ET_MODE;
      break;
    default:
      qpoet_mode = RFDEVICE_NO_PT_MODE;
      break;
  }

  if ( qpoet_mode == RFDEVICE_ET_MODE )
  {
    FTM_MSG_ERROR("ftm_tdscdma_xpt_override_txagc: TDS is asked to do ET!", 0, 0, 0);

#if 0
    /* Call the common MC interface to override MSM settings */
    status &= rfcommon_mc_xpt_txagc_override( device, &mdsp_ovr_params );

    DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */

    rfdevice_tdscdma_papm_set_mode_bias( device,
                                         ftm_tdscdma_current_mode,
                                         qpoet_mode,
                                         0, /* bias in mV*/
                                         RFDEVICE_EXECUTE_IMMEDIATE,
                                         NULL);
#endif
  }
  else
  {
    if ( qpoet_mode == RFDEVICE_APT_MODE &&
         smps_bias != FTM_COMMON_XPT_TX_OVR_SMPS_BIAS_INVALID )
    {
      ftm_tdscdma_set_smps_pa_bias_override(TRUE);
      /*Keep track of the overriden value for future use*/
      ftm_tdscdma_smps_pa_bias_override_val = smps_bias;

      rfdevice_tdscdma_papm_set_mode_bias( device,
                                           ftm_tdscdma_current_mode,
                                           qpoet_mode,
                                           smps_bias, /* bias in mV*/
                                           RFDEVICE_EXECUTE_IMMEDIATE,
                                           NULL,
                                           0);
    }

    DALSYS_BusyWait(10); /* wait for 10 us before configuring QPOET */
    /* Call the common MC interface to override MSM settings */
    status &= rfcommon_mc_xpt_txagc_override( device, &mdsp_ovr_params );
  }

  // Debug msg
  MSG_7( MSG_SSID_RF, MSG_LEGACY_HIGH,
         "ftm_tdscdma_xpt_override_txagc: XPT Mode %d, IQ Gain Action %d, "
         "IQ Gain %d, Env Scale Action %d, Env Scale %d, Delay Action %d, Delay %d",
         xpt_mode,
         iq_gain_action_type,
         mdsp_ovr_params.iq_gain,
         env_scale_action_type,
         mdsp_ovr_params.env_scale,
         delay_action_type,
         mdsp_ovr_params.delay );

  MSG_4( MSG_SSID_RF, MSG_LEGACY_HIGH,
         "ftm_tdscdma_xpt_override_txagc: RGI %d, SMPS %d, PA State %d, PA Curr %d",
         rgi,
         smps_bias,
         pa_state,
         pa_curr);

  MSG_1( MSG_SSID_RF, MSG_LEGACY_HIGH,
         "set commom xpt txagc override, status = %d",
         status);

  return status;
}/* ftm_tdscdma_xpt_override_txagc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override TXAGC parameters 

  @details
  This function will override TXAGC params.

  @param device
  The device to perform override on. 
 
  @param txagc_oride_parms
  The parameter values to override.
 
  @return
  Return if override was successful or not. 
*/
boolean
ftm_tdscdma_xpt_process_override_txagc
( 
  rfm_device_enum_type device,
  uint8  xpt_mode,
  uint8  amam_bank_idx,
  uint16 amam_start_offset,
  uint8  amam_tbl_size,
  uint8  ampm_bank_idx,
  uint16 ampm_start_offset,
  uint8  ampm_tbl_size,
  uint8  epdt_bank_idx,
  uint16 epdt_start_offset,
  uint8  epdt_tbl_size,
  uint8  iq_gain_action_type,
  uint32 iq_gain,
  uint8  env_scale_action_type,
  uint32 env_scale,
  uint16 rgi,
  uint16 smps_bias,
  uint8  pa_state,
  uint16 pa_curr,
  uint8  delay_action_type,
  int32  delay
)
{
	boolean status = TRUE;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type rfc_cfg;
  rfnv_tdscdma_tx_tbl_type *tx_nv_tbl_ptr = NULL;

  uint32 final_iq_gain;
  uint32 final_env_scale = FTM_COMMON_XPT_TX_OVR_ENV_SCALE_INVALID;
  int32 final_delay = FTM_COMMON_XPT_TX_OVR_DELAY_INVALID;

  rfcommon_core_xpt_tx_override_type curr_tx_ovr;
  rfdevice_rxtx_common_class* device_ptr;
  rfdevice_trx_tdscdma_tx** tds_device_ptr = NULL;


  tx_nv_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
  if (NULL == tx_nv_tbl_ptr)
  {
  	MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "error: Failed to retrieve NV for band=%d",
  	                                   ftm_tdscdma_current_mode);
  	return FALSE;  		
  }

  /* Find out common device pointer from the 1st Tx device */
  tds_device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(
                              device,
                              RFC_CONFIG_TX, 
                              ftm_tdscdma_current_mode, 
                              RFDEVICE_TRANSCEIVER);
  if (tds_device_ptr != NULL)
  {
    device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device_ptr[0]);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. Tx band: %d", 
          ftm_tdscdma_current_mode);
    return FALSE;
  }
  
  if ( NULL != device_ptr )
  {
    if ( rfdevice_cmn_is_cmn_tx_intf_supported( device_ptr ) )
    {
      MSG_2( MSG_SSID_RF, MSG_LEGACY_HIGH, "Common API to configure TxAGC rgi=%d, paState=%d",
                                         rgi, pa_state);
    }
    else
    {
      MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "error: rfdevice_cmn_tx_intf is not supported!, band=%d",
                                         ftm_tdscdma_current_mode);
      return FALSE;                                   
    }
  }
  else
  {
     MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_tdscdma_xpt_override_txagc(): device_ptr is NULL. band=%d",
                                         ftm_tdscdma_current_mode ) ;
     return FALSE;                                    
  }

  /* get modem chain */
  rfc_cfg.alternate_path = 0;
  rfc_cfg.band = ftm_tdscdma_current_mode;
  rfc_cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  rfc_cfg.logical_device = device;
  rfc_cfg.rx_tx = RFC_CONFIG_TX;
  if (rfc_tdscdma_get_devices_info_data(&rfc_cfg, &device_info_ptr) == TRUE)
  {
    curr_tx_ovr.rflm_handle = ftm_tdscdma_txlm_buffer.buf_idx;
    curr_tx_ovr.modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    MSG_2( MSG_SSID_RF, MSG_LEGACY_ERROR, "Cannot get device config %d, band %d",
                                           device, ftm_tdscdma_current_mode);
    return FALSE;
  }

  /* Decide on IQ gain override type */
  switch ( iq_gain_action_type )
  {
    case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
      final_iq_gain = iq_gain;
      break;
    
    case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
      final_iq_gain = rf_tdscdma_msm_get_default_txc_gain(                
                                         ftm_tdscdma_txlm_buffer.buf_idx);
      break;
    
    case FTM_COMMON_XPT_TX_OVERRIDE_NV:
      final_iq_gain = (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_IQ_GAIN_UNITY];
      break;
    
    case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
    default:
      final_iq_gain = FTM_COMMON_XPT_TX_OVR_IQ_GAIN_INVALID;
    break;
  }

  /* Decide on Env Scale override type */
  switch ( env_scale_action_type )
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
    final_env_scale = env_scale;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
  {
    if( xpt_mode == RFCOMMON_MDSP_XPT_MODE_EPT )
    {
	    final_env_scale = (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[EPT_CFG_CMN_EVN_SCALE_UNITY];
    }
    else if ( xpt_mode == RFCOMMON_MDSP_XPT_MODE_ET )
    {
      final_env_scale = (int32)tx_nv_tbl_ptr->ept_dpd_cfg_params[ET_CFG_CMN_ENV_SCALE_UNITY];
    }
    break;
  }   

  case FTM_COMMON_XPT_TX_OVERRIDE_DEFAULT:
  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:    
    break;
  }

  /* Decide on delay value override type */
  if (delay_action_type != FTM_COMMON_XPT_TX_OVERRIDE_SKIP)
  {
    FTM_MSG_ERROR("TDS delay_action_type is not SKIP %d!", delay_action_type, 0, 0);
  }
  switch ( delay_action_type )
  {
  case FTM_COMMON_XPT_TX_OVERRIDE_VAL:
  	final_delay = delay;
    break;

  case FTM_COMMON_XPT_TX_OVERRIDE_NV:
  case FTM_COMMON_XPT_TX_OVERRIDE_SKIP:
  default:
  	break;
  }

  /* Update remainings of the current set of overrides to FTM common data */
  curr_tx_ovr.xpt_mode = xpt_mode;
  curr_tx_ovr.iq_gain = final_iq_gain;
  curr_tx_ovr.iq_gain_action_type = iq_gain_action_type;
  curr_tx_ovr.env_scale = final_env_scale;
  curr_tx_ovr.env_scale_action_type = env_scale_action_type;
  curr_tx_ovr.et_delay = final_delay;
  curr_tx_ovr.delay_action_type = delay_action_type;
  curr_tx_ovr.amam_addr.mem_bank_id = amam_bank_idx;
  curr_tx_ovr.amam_addr.start_offset = amam_start_offset;
  curr_tx_ovr.amam_addr.tbl_size = amam_tbl_size;
  curr_tx_ovr.ampm_addr.mem_bank_id = ampm_bank_idx;
  curr_tx_ovr.ampm_addr.start_offset = ampm_start_offset;
  curr_tx_ovr.ampm_addr.tbl_size = ampm_tbl_size;
  curr_tx_ovr.detrough_addr.mem_bank_id = epdt_bank_idx;
  curr_tx_ovr.detrough_addr.start_offset = epdt_start_offset;
  curr_tx_ovr.detrough_addr.tbl_size = epdt_tbl_size;
  curr_tx_ovr.latch_immediate = FALSE;
  curr_tx_ovr.max_sample_count = 0;
  curr_tx_ovr.rgi = rgi;
  curr_tx_ovr.papm_bias = smps_bias;
  curr_tx_ovr.pa_state = pa_state;
  curr_tx_ovr.pa_curr = pa_curr;
  curr_tx_ovr.pa_range = rf_tdscdma_core_util_get_pa_range_from_nv(pa_state);
  
  status &= ftm_common_xpt_set_curr_tx_override( device, 
                                                 &curr_tx_ovr );
	
	if ( !status )
		MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_tdscdma_xpt_process_override_txagc, status = %d", status ) ;
  
  return status;
} /* ftm_tdscdma_xpt_process_override_txagc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override XPT txagc in RF

  @details
  This function calls the RFM FTM interface to perform a Tx override from RF

  @param ovr_params
  Override parameters obtained from FTM dispatch

  @return
  TRUE is set for successful execution; FALSE otherwise
*/
boolean ftm_tdscdma_xpt_override_rf_settings
(
  ftm_xpt_txagc_override_type* params
)
{
  boolean ret_val = TRUE;

  /* FTM xPT Tx AGC override does not override the DPD HW */
  uint8  amam_bank_idx = -1;
  uint16 amam_start_offset = -1;
  uint8  amam_tbl_size = 0;
  uint8  ampm_bank_idx = -1;
  uint16 ampm_start_offset = -1;
  uint8  ampm_tbl_size = 0;
  uint8  epdt_bank_idx = -1;
  uint16 epdt_start_offset = -1;
  uint8  epdt_tbl_size = 0;

  rfcom_band_type_u tech_band;
  rfdevice_rxtx_common_class* device_ptr;
  rfdevice_trx_tdscdma_tx** tds_device_ptr = NULL;

  
  if(params == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_tdscdma_xpt_override_rf_settings: Null argument");
    return FALSE;
  }

  /* Find out common device pointer from the 1st Tx device */
  //device_ptr = rfc_common_get_cmn_device_object ( 0 );

  tds_device_ptr = (rfdevice_trx_tdscdma_tx**) rfc_tdscdma_get_rf_device_obj(
                              params->device,
                              RFC_CONFIG_TX, 
                              ftm_tdscdma_current_mode, 
                              RFDEVICE_TRANSCEIVER);

  if (tds_device_ptr != NULL)
  {
    device_ptr = rfdevice_tdscdma_tx_get_common_device_pointer(tds_device_ptr[0]);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfc_tdscdma_get_rf_device_obj failure. Null pointer detected. Tx band: %d", 
          ftm_tdscdma_current_mode);
    return FALSE;
  }

  /* check for valid device ptr */
  if ( NULL == device_ptr)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "ftm_tdscdma_xpt_override_rf_settings: Failed to get a valid pointer, device = %d", params->device);  	
  	return FALSE;
  }

  if ( rfcommon_debug_flags.use_rflm_txagc_override )
  {
  	    /* Process override params */
    ret_val &= ftm_tdscdma_xpt_process_override_txagc( params->device,           
                                                       params->xpt_mode,         
                                                       amam_bank_idx,    
                                                       amam_start_offset,
                                                       amam_tbl_size,    
                                                       ampm_bank_idx,    
                                                       ampm_start_offset,
                                                       ampm_tbl_size,    
                                                       epdt_bank_idx,    
                                                       epdt_start_offset,
                                                       epdt_tbl_size,
                                                       params->iq_gain_action_type,    
                                                       params->iq_gain,
                                                       params->env_scale_action_type,        
                                                       params->env_scale,        
                                                       params->rgi,              
                                                       params->smps_bias,        
                                                       params->pa_state,         
                                                       params->pa_curr,
                                                       params->delay_action_type,
                                                       params->delay );

    /* Apply the Tx override through RFLM */
    tech_band.tdscdma_band = ftm_tdscdma_current_mode;
    ret_val &= rfm_ftm_apply_xpt_tx_agc_override( RFM_TDSCDMA_MODE, params->device,
                                                  tech_band );  	
  }
  else
  {	
  	/* Send override to RF*/
    ret_val &= ftm_tdscdma_xpt_override_txagc( params->device,
                                               params->xpt_mode,
                                               amam_bank_idx,
                                               amam_start_offset,
                                               amam_tbl_size,
                                               ampm_bank_idx,
                                               ampm_start_offset,
                                               ampm_tbl_size,
                                               epdt_bank_idx,
                                               epdt_start_offset,
                                               epdt_tbl_size,
                                               params->iq_gain_action_type,
                                               params->iq_gain,
                                               params->env_scale_action_type,
                                               params->env_scale,
                                               params->rgi,
                                               params->smps_bias,
                                               params->pa_state,
                                               params->pa_curr,
                                               params->delay_action_type,
                                               params->delay );
  }
  return ret_val;
}/* ftm_tdscdma_xpt_override_rf_settings */

/*!
  @brief
  Trigger Single instance of IQ Capture

  @details
  This function will call RF-MDSP function to trigger the IQ Capture
  for XPT for TDS.

  @param device
  Primary device for the Transmit path for IQ capture

  @param device_fb
  Feedback device for the IQ capture

  @param xpt_capture_type
  Indicate what mode the capture is for (Currently TDS supports only EPT mode)

  @param num_samples
  Number of TXC/RX samples to capture.Must be a multiple of 1024(1K)

  @param first_trig
  Flag to do initialization.This must be set in 1st trigger

  @param last_trig
  Flag to do de-initialization.This must be set in last trigger

  @param do_processing
  Flag to indicate if firmware should do DPD estimation on the captured samples.

  @param buffer_id
  Buffer index to store captured data and processed data

  @param samp_rate
  Pointer return to hold sampling rate of Rx IQ capture

  @return
  TRUE is set successfully; FALSE otherwise

*/
boolean ftm_tdscdma_xpt_iq_capture(uint8 device,
                                   uint8 device_fb,
                                   uint8 xpt_capture_type,
                                   uint16 num_samples,
                                   boolean first_trig,
                                   boolean last_trig,
                                   rfcommon_xpt_processing_type do_processing,
                                   rf_common_xpt_buffer_id_type buffer_id,
                                   uint32 * samp_rate)
{
  boolean ret_val = TRUE;
  uint8 tx_modem_chain, rx_modem_chain;
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;
  rfc_device_info_type *device_info_ptr;
  rfc_cfg_params_type cfg;
  rfnv_tdscdma_tx_tbl_type *nv_tbl = NULL;
  uint16 split_samp = 0; // No split sample capture
  
  nv_tbl = rf_tdscdma_core_util_init_tx_nv_table(ftm_tdscdma_current_mode);
  
  /* Validate NV table */
  if (nv_tbl == NULL)
  {
    FTM_MSG_ERROR("ftm_tdscdma_xpt_iq_capture: Failed to get valid static NV pointer for band %d",
                  ftm_tdscdma_current_mode, 0, 0);
    return FALSE;
  }

  /*Initialize Pin IQ capture type to INVALID*/
  iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_INVALID;

  /*Initialize Delay IQ capture type to INVALID*/
  iq_cap_data.delay_capture_type = RF_COMMON_XPT_DELAY_CAP_INVALID;


  cfg.alternate_path = 0;
  cfg.band = ftm_tdscdma_current_mode;
  cfg.req = RFC_REQ_ENABLE_XPT_CAPTURE;
  cfg.logical_device = device_fb;
  cfg.rx_tx= RFC_CONFIG_TX;

  /* Get Rx Modem chain */
  /* TODO -- (kb) to be verified for Bryce card after its bring up. */
  if (rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr) == TRUE)
  {
    rx_modem_chain = device_info_ptr->modem_chain;
  }
  else
  {
    FTM_MSG_ERROR("Cannot get device config %d, band %d", device, ftm_tdscdma_current_mode, 0);
    return FALSE;
  }

  /* Get Tx Modem chain */
  tx_modem_chain = rfcommon_core_get_tx_chain_from_device(device);

  FTM_MSG_MED("ftm_tdscdma_xpt_iq_capture rx_modem_chain %d tx_modem_chain %d.",
              rx_modem_chain, tx_modem_chain, 0);

  if (first_trig==TRUE)
  {
    /* Set Capture params in FW SMEM */
    ret_val &= rfcommon_mdsp_setup_capture_params(tx_modem_chain,                              /* tx modem chain */
                                                  rx_modem_chain,                              /* rx modem chain */
                                                  (uint8)ftm_tdscdma_xpt_fb_path_buffer.buf_idx, /* xPT feedback path id */
                                                  xpt_capture_type,                            /* FW capture type */
                                                  (boolean)(0x1 & (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_RX_COMPACT]),
                                                  num_samples,                                 /* number of samples */
                                                  split_samp,
                                                  do_processing);
    /* Send DPD processing Params to FW*/
    if (do_processing>PROC_NONE)
    {
      if ( ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_SELF_TEST_EVM) || ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_SELF_TEST_VSWR))
      {
          /* set up common DPD config params in SMEM for new EVM search window, only if first trig */
          rfcommon_mdsp_setup_dpd_proc_params((int16)FTM_TDSCDMA_SELFTEST_SEARCH_CENTER,
                                              (uint8)FTM_TDSCDMA_SELFTEST_SEARCH_WIDTH,
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_FIRST_BIN],
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_EXTRAP_BIN],
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_BIN_COUNT],
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_FIRST_BIN_COUNT],
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_LAST_BIN_COUNT],
                                                       (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_SPEC_INV],
                                                  do_processing);
          FTM_MSG_2(RF_HIGH, "ftm_tdscdma_xpt_iq_capture: For Selftest EVM/VSWR type, set search center = %d, search width = %d",
                    FTM_TDSCDMA_SELFTEST_SEARCH_CENTER, FTM_TDSCDMA_SELFTEST_SEARCH_WIDTH);
      }
      else
      {
      ret_val &= rfcommon_mdsp_setup_dpd_proc_params((int16)nv_tbl->ept_dpd_cfg_params[EPT_CFG_CMN_SEARCH_CENTER],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[EPT_CFG_CMN_SEARCH_WIDTH],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_FIRST_BIN],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_EXTRAP_BIN],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_BIN_COUNT],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_FIRST_BIN_COUNT],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_MIN_LAST_BIN_COUNT],
                                                     (uint8)nv_tbl->ept_dpd_cfg_params[DPD_CFG_SPEC_INV],
                                                     do_processing);
    }
  }
  }

  if (samp_rate !=NULL)
  {
    *samp_rate = rf_tdscdma_msm_get_samp_rate((lm_handle_type)ftm_tdscdma_xpt_fb_path_buffer.buf_idx);
    iq_cap_data.samp_rate = *samp_rate; 
  }

  else
  {
    iq_cap_data.samp_rate = 10000000;
    //FTM_MSG_MED("ftm_tdscdma_xpt_iq_capture: Samp Rate not queried, ET delay value returned in 1/1000th of samples");
  }

  if ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_DELAY)
  {
     FTM_MSG_ERROR( "ftm_tdscdma_xpt_iq_capture: TDSCDMA does not support delay capture", 0, 0, 0 );
  }

  if ((rfcommon_xpt_capture_type)xpt_capture_type == XPT_CAPTURE_PIN)
  {
    /*Set Pin IQ capture type*/
    iq_cap_data.pin_capture_type = RF_COMMON_XPT_PIN_CAP_ALL_TECHS;

    FTM_MSG_HIGH( "ftm_tdscdma_xpt_iq_capture: Pushing pin data to pin map with pin capture type: %d", iq_cap_data.pin_capture_type, 0, 0);

    ret_val &= ftm_cal_data_push_pin_data_to_pin_map(ftm_tdscdma_current_chan,
                                                     buffer_id.capt_buf_id,
                                                     buffer_id.res_buf_id);
    FTM_MSG_MED("ftm_tdscdma_xpt_iq_capture: Push to pin map - Chan: %d, CaptBuf: %d, ResBuf: %d",
                 ftm_tdscdma_current_chan,
                 buffer_id.capt_buf_id,
                 buffer_id.res_buf_id);
    
  }


  /* Store the IQ Caprure Data */
  iq_cap_data.device = device;
  iq_cap_data.device_fb = device_fb;
  iq_cap_data.channel = ftm_tdscdma_current_chan;
  iq_cap_data.is_last_trig = last_trig;
  iq_cap_data.buffer_id = buffer_id;
  iq_cap_data.capture_type = xpt_capture_type;
  ret_val &= rfcommon_xpt_store_iq_capture_data( &iq_cap_data );

  /* Trigger FW IQ capture */
  rf_tdscdma_mdsp_xpt_capture(first_trig, last_trig, buffer_id.capt_buf_id, buffer_id.res_buf_id);

  FTM_MSG_MED("ftm_tdscdma_xpt_iq_capture status %d.",ret_val,0,0);

  return ret_val;
}

/*===========================================================================
FUNCTION ftm_tdscdma_get_multi_synth_handler

DESCRIPTION
   This function used to get the multi synth status for Tx and Rx Pll

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_tdscdma_get_multi_synth_handler(void)
{
   boolean tx_pll_state = FALSE;
   boolean rx_pll_state = FALSE;
   boolean is_cmd_sent = FALSE;

   rftdscdma_mdsp_common_event_data_type rf_tds_build_event_data;

   if(rfm_get_calibration_state())  /*Calibration Mode*/
   {
      FTM_MSG_HIGH("Command Received in Cal Mode ",0,0,0);

      switch (rx_pll_multi_state)
      {
         case FTM_SYNTH_STATUS_LOCKED:
		 case FTM_SYNTH_STATUS_UNLOCKED:
		     /*read rx pll*/
			 rfdevice_tdscdma_rx_cmd_dispatch(ftm_tdscdma_current_pri_device,
			                                  RFM_TDSCDMA_MODE,
			                                  RFCOM_BAND_TDSCDMA_ANY_BAND,
			                                  RFDEVICE_GET_RX_SYNC_LOCK,
			                                  &rx_pll_state);

			 if ((FTM_SYNTH_STATUS_LOCKED == rx_pll_multi_state ) &&
			 	(FALSE == rx_pll_state))
			 {
			 	rx_pll_multi_state = FTM_SYNTH_STATUS_UNLOCKED;
			    FTM_MSG_HIGH("Rx turned to UNLOCKED ",0,0,0);
			 }
			 break;
		 case FTM_SYNTH_STATUS_UNDETERMINED:
		 	 /*read rx pll*/
			 rfdevice_tdscdma_rx_cmd_dispatch(ftm_tdscdma_current_pri_device,
			                                  RFM_TDSCDMA_MODE,
			                                  RFCOM_BAND_TDSCDMA_ANY_BAND,
			                                  RFDEVICE_GET_RX_SYNC_LOCK,
			                                  &rx_pll_state);


		     if ((FALSE == rx_pll_state)
			 	&& (FTM_TDSCDMA_CAL_STATE_INACTIVE== ftm_tdscdma_current_cal_state))
			 	rx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
			 else if ((TRUE == rx_pll_state)
			 	&& (FTM_TDSCDMA_CAL_STATE_RX == ftm_tdscdma_current_cal_state))
			 	rx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
			 else
			 	/*print some logs here*/
			 	FTM_MSG_HIGH("Invalid Status, please turn Rx/Tx off, Rx Pll Status:%d, CalState: %d ",rx_pll_state,ftm_tdscdma_current_cal_state,0);
			 break;
		  default :
		 	 /*print some logs here*/
			 FTM_MSG_HIGH("Rx PLL State will return the current state without doing anything",0,0,0);
			 break;

      }

      switch (tx_pll_multi_state)
      {

     case FTM_SYNTH_STATUS_LOCKED:
		 case FTM_SYNTH_STATUS_UNLOCKED:
		     /*read rx pll*/
			 rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
			                                  RFM_TDSCDMA_MODE,
			                                  RFCOM_BAND_TDSCDMA_ANY_BAND,
			                                  RFDEVICE_GET_TX_SYNC_LOCK,
			                                  &tx_pll_state);

			 if ((FTM_SYNTH_STATUS_LOCKED == tx_pll_multi_state ) &&
			 	(FALSE == tx_pll_state))
			 {
			 	tx_pll_multi_state = FTM_SYNTH_STATUS_UNLOCKED;
			    FTM_MSG_HIGH("Tx turned to UNLOCKED ",0,0,0);
			 }
			 break;
       
		 case FTM_SYNTH_STATUS_UNDETERMINED:
		 	 /*read tx pll*/
		 	 rfdevice_tdscdma_tx_cmd_dispatch(ftm_tdscdma_current_tx_device,
		 	                                  RFM_TDSCDMA_MODE,
		 	                                  RFCOM_BAND_TDSCDMA_ANY_BAND,
		 	                                  RFDEVICE_GET_TX_SYNC_LOCK,
		 	                                  &tx_pll_state);

		     if ((FALSE == tx_pll_state)
			 	&& (FTM_TDSCDMA_CAL_STATE_INACTIVE== ftm_tdscdma_current_cal_state))
			 	tx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
			 else if ((TRUE == tx_pll_state)
			 	&& (FTM_TDSCDMA_CAL_STATE_TX == ftm_tdscdma_current_cal_state))
			 	tx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
			 else
			 	/*print some logs here*/
			    FTM_MSG_HIGH("Invalid Status, please turn Rx/Tx off, Tx Pll Status:%d, CalState: %d ",tx_pll_state,ftm_tdscdma_current_cal_state,0);
			 break;
		  default :
		 	 /*print some logs here*/
			 FTM_MSG_HIGH("Tx PLL State will return the current state without doing anything",0,0,0);
			 break;

      }

   }
   else /*FTM-RF Mode*/
   {
	   FTM_MSG_HIGH("Command Received in FTM RF Mode ",0,0,0);
	   switch (rx_pll_multi_state)
	   {
		  case FTM_SYNTH_STATUS_LOCKED:
		  case FTM_SYNTH_STATUS_UNLOCKED:
          rf_tds_build_event_data.pri_device = ftm_tdscdma_current_pri_device;
          rf_tds_build_event_data.sec_device = ftm_tdscdma_current_sec_device;
			    rf_tds_build_event_data.tx_device = ftm_tdscdma_current_tx_device;
   			  rf_tds_build_event_data.band = RFCOM_BAND_TDSCDMA_ANY_BAND;
    		  rf_tds_build_event_data.channel = 10055;
    		  rf_tds_build_event_data.drx_enable = FALSE;
    		  rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
			  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_PLL, &rf_tds_build_event_data);

			  /*send cmd to fw to read ssbi*/
			  ftm_tdscdma_rf_mode_get_multi_synth_state( &rx_pll_state, &tx_pll_state);

			  is_cmd_sent = TRUE;

			  if ((FTM_SYNTH_STATUS_LOCKED == rx_pll_multi_state ) &&
				 (FALSE == rx_pll_state))
			  {
				 rx_pll_multi_state = FTM_SYNTH_STATUS_UNLOCKED;
			  }

			  break;

		  case FTM_SYNTH_STATUS_UNDETERMINED :
          rf_tds_build_event_data.pri_device = ftm_tdscdma_current_pri_device;
          rf_tds_build_event_data.sec_device = ftm_tdscdma_current_sec_device;
			    rf_tds_build_event_data.tx_device = ftm_tdscdma_current_tx_device;
   			  rf_tds_build_event_data.band = RFCOM_BAND_TDSCDMA_ANY_BAND;
    		  rf_tds_build_event_data.channel = 10055;
    		  rf_tds_build_event_data.drx_enable = FALSE;
    		  rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
			  rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_PLL, &rf_tds_build_event_data);

			  /*send cmd to fw to read ssbi*/
		  	  ftm_tdscdma_rf_mode_get_multi_synth_state( &rx_pll_state, &tx_pll_state);

			  /*this flag means the cmd has been sent for once*/
			  is_cmd_sent = TRUE;

			  if ((FALSE == rx_pll_state)
				 && (FTM_TDSCDMA_BURST_RX_OFF== ftm_tdscdma_burst_rx_state))
			  {
				 rx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
			  }
			  else if ((TRUE == rx_pll_state)
			  	 && (FTM_TDSCDMA_BURST_RX_ON == ftm_tdscdma_burst_rx_state))
			  {
				 rx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
			  }
			  else
			  {
				 /*print some logs here*/
			  	 FTM_MSG_ERROR("Invalid Status, please turn Rx Burst Off, Rx Pll Status:%d, Rx Burst State: %d ",rx_pll_state,ftm_tdscdma_burst_rx_state,0);
			  }

			  break;

		   default :
			  /*print some logs here*/
			  FTM_MSG_HIGH("Rx PLL State will return the current state without doing anything",0,0,0);
			  break;

	   }

	   if((ftm_tdscdma_burst_tx_state == FTM_TDSCDMA_BURST_TX_ON) &&
	   	  (ftm_tdscdma_burst_tx_params_shadow.num_slots > 0))
	   {
		   FTM_MSG_ERROR("Not supported for Tx Burst,  Tx Pll Status:%d will stay the same, TxBurstState: %d ",tx_pll_state,ftm_tdscdma_burst_tx_state,0);
		   return;
	   }

	   switch (tx_pll_multi_state)
	   {
		  case FTM_SYNTH_STATUS_LOCKED:
		  case FTM_SYNTH_STATUS_UNLOCKED:

        if (!is_cmd_sent)  /*Won't have to read if the cmd has been sent once*/
        {
                
          rf_tds_build_event_data.pri_device = ftm_tdscdma_current_pri_device;
          rf_tds_build_event_data.sec_device = ftm_tdscdma_current_sec_device;
          rf_tds_build_event_data.tx_device = ftm_tdscdma_current_tx_device;

   				rf_tds_build_event_data.band = RFCOM_BAND_TDSCDMA_ANY_BAND;
    			rf_tds_build_event_data.channel = 10055;
    			rf_tds_build_event_data.drx_enable = FALSE;
    			rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
          rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_PLL, &rf_tds_build_event_data);

          ftm_tdscdma_rf_mode_get_multi_synth_state( &rx_pll_state, &tx_pll_state);
        }

			  if ((FTM_SYNTH_STATUS_LOCKED == tx_pll_multi_state ) &&
				    (FALSE == tx_pll_state))
			  {
          tx_pll_multi_state = FTM_SYNTH_STATUS_UNLOCKED;
			  }
			  break;

		  case FTM_SYNTH_STATUS_UNDETERMINED :

              if (!is_cmd_sent)  /*Won't have to read if the cmd has been sent once*/
              {

        rf_tds_build_event_data.pri_device = ftm_tdscdma_current_pri_device;
        rf_tds_build_event_data.sec_device = ftm_tdscdma_current_sec_device;
        rf_tds_build_event_data.tx_device = ftm_tdscdma_current_tx_device;

   				rf_tds_build_event_data.band = RFCOM_BAND_TDSCDMA_ANY_BAND;
    			rf_tds_build_event_data.channel = 10055;
    			rf_tds_build_event_data.drx_enable = FALSE;
    			rf_tds_build_event_data.buf_id = rf_tdscdma_mc_dbl_bufs.tx_wrk_buf_idx;
				rf_tdscdma_mdsp_rf_event_cmd_dispatch(RF_TDS_EVT_DO_PLL, &rf_tds_build_event_data);


		  	    ftm_tdscdma_rf_mode_get_multi_synth_state( &rx_pll_state, &tx_pll_state);
              }

			  if ((FALSE == tx_pll_state)
				 && (FTM_TDSCDMA_BURST_TX_OFF== ftm_tdscdma_burst_tx_state))
			  {
				 tx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
			  }
			  else if ((TRUE == tx_pll_state)
				 && (FTM_TDSCDMA_BURST_TX_ON == ftm_tdscdma_burst_tx_state))
			  {
				 tx_pll_multi_state = FTM_SYNTH_STATUS_LOCKED;
			  }
			  else
			  {
				 /*print some logs here*/
			  	FTM_MSG_ERROR("Invalid Status, please turn Rx/Tx off, Tx Pll Status:%d, Continuous Tx State : %d ",tx_pll_state,ftm_tdscdma_burst_tx_state,0);
			  }
			  break;

		   default :
			  /*print some logs here*/
			  FTM_MSG_HIGH("Tx PLL State will return the current state without doing anything",0,0,0);
			  break;

	   }

   }
}


/*===========================================================================
FUNCTION ftm_tdscdma_reset_multi_synth_state

DESCRIPTION
   This function used to reset the multi synth status for Tx and Rx Pll

DEPENDENCIES
   None.

RETURN VALUE
   Return TRUE if both Tx and Rx pll been set to UNDERTERMINED

SIDE EFFECTS
   None.

===========================================================================*/

boolean ftm_tdscdma_reset_multi_synth_state(void)
{
    uint8 failure_cnt = 0;

    switch (rx_pll_multi_state)
    {
		case FTM_SYNTH_STATUS_LOCKED:
		case FTM_SYNTH_STATUS_UNLOCKED:
			rx_pll_multi_state = FTM_SYNTH_STATUS_UNDETERMINED;
			break;
		default:
			failure_cnt++;
			break;
    }

	switch (tx_pll_multi_state)
	{
		case FTM_SYNTH_STATUS_LOCKED:
		case FTM_SYNTH_STATUS_UNLOCKED:
			tx_pll_multi_state = FTM_SYNTH_STATUS_UNDETERMINED;
			break;
		default:
			failure_cnt++;
			break;
	}

	if (0 < failure_cnt)
		return FALSE;

	return TRUE;
}


/*===========================================================================
FUNCTION ftm_tdscdma_init_multi_synth_state

DESCRIPTION
   This function is used to initialize the multi synth status for Tx and Rx Pll to DISABLED or
   turn it back to INVALID

DEPENDENCIES
   None.

RETURN VALUE
   Return TRUE if both Tx and Rx pll been set to UNDERTERMINED

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_set_multi_synth_state(boolean on_off)
{

   if (TRUE == on_off)
   {
      tx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
      rx_pll_multi_state = FTM_SYNTH_STATUS_DISABLED;
   }
   else
   {
      tx_pll_multi_state = FTM_SYNTH_STATUS_INVALID;
      rx_pll_multi_state = FTM_SYNTH_STATUS_INVALID;
   }
}
/*===========================================================================
FUNCTION ftm_tdscdma_rf_mode_get_multi_synth_state

DESCRIPTION
   This function is used to send speficif cmd to get both Tx and Rx PLL status from FW

DEPENDENCIES
   None.

RETURN VALUE
   None

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_tdscdma_rf_mode_get_multi_synth_state(uint8 * rx_pll_status, uint8 * tx_pll_status)
{
    boolean error_code;
    /*Initiate the cmd status to PENDING*/
	rf_tdscdma_mc_set_multi_synth_rsp_status(TDSCDMA_RFMODE_CMD_RSP_PENDING);
	/*Send the MSG*/
	rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG,0);
	/*Blocking until receive the response*/
	error_code = ftm_tdscdma_rf_mode_blocking_cmd_handler(RF_TDSCDMA_MDSP_GET_MULTI_SYNTH_MSG);

	*rx_pll_status = rf_tdscdma_mc_get_rx_pll_status();
	*tx_pll_status = rf_tdscdma_mc_get_tx_pll_status();

	if (FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE== error_code)
	{
	  FTM_MSG_ERROR("Command Failed, both tx and rx pll status remain the same", 0,0,0);
	}
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clean up FTM RF Mode data structures

  @details
  This API will clean up the FTM RF Mode data structures and will re-initialize
  them to the default boot up values. Further, this API will check the TX and
  RX state machines and issues TX OFF and/or RX OFF if either of them are active.

*/
void ftm_tdscdma_rfmode_cleanup(void)
{

  FTM_MSG_MED("TD-SCDMA exit mode: Cleaning up FTM RF Mode params",0,0,0);

  /* Clean up FTM TX data structures */
  if (ftm_tdscdma_burst_tx_state == FTM_TDSCDMA_BURST_TX_ON)
  {
    /* Turn OFF TX if it is ON*/
    FTM_MSG_HIGH("TD-SCDMA exit mode: FTM TX is ON! Issuing RF Mode TX OFF command.",0,0,0);
    if(!(ftm_tdscdma_burst_tx(FALSE)))
    {
      FTM_MSG_ERROR("TD-SCDMA exit mode: Error processing RF Mode TX OFF command!",0,0,0);
    }
  }

  ftm_tdscdma_burst_tx_params_shadow.num_slots = 1;
  ftm_tdscdma_burst_tx_params_shadow.mod_type = 0;
  ftm_tdscdma_burst_tx_params_shadow.tspwrlvl[0]=-450;
  ftm_tdscdma_burst_tx_params_shadow.freqOffset = 0;
  ftm_tdscdma_burst_tx_params_shadow.sf = 8;
  ftm_tdscdma_burst_tx_params_shadow.pdet_thresh = 150;
  ftm_tdscdma_burst_tx_params_shadow.pdet_trig_interval = 2;
  ftm_tdscdma_burst_tx_params_shadow.ovsf_code = 1;
  ftm_tdscdma_burst_tx_params_shadow.conTX_tx_reconfig_block = TRUE;

  /* Clean up FTM RX data structures */
  if (ftm_tdscdma_burst_rx_state == FTM_TDSCDMA_BURST_RX_ON)
  {
    /* Turn OFF RX if it is ON*/
    FTM_MSG_HIGH("TD-SCDMA exit mode: FTM RX is ON! Issuing RF Mode RX OFF command.",0,0,0);
    if(!(ftm_tdscdma_burst_rx(-1600,FALSE)))
    {
      FTM_MSG_ERROR("TD-SCDMA exit mode: Error processing RF Mode RX OFF command!",0,0,0);
    }
  }

  ftm_tdscdma_burst_rx_params_shadow.num_slots = 1;
  ftm_tdscdma_burst_rx_params_shadow.rx1_active = FALSE;
  ftm_tdscdma_burst_rx_params_shadow.tracking_enable = FALSE;
  ftm_tdscdma_burst_rx_params_shadow.exp_rx_lvl = -1600;
  ftm_tdscdma_burst_rx_params_shadow.computed_lna_state = 0;
  ftm_tdscdma_burst_rx_params_shadow.computed_lna_state_rx1 = 0;

}

/*===========================================================================
FUNCTION FTM_TDSCDMA_SWITCH_TX_WAVEFORM

DESCRIPTION
  This function switches the waveform between Special burst and DPD waveform.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_tdscdma_switch_tx_waveform( byte tx_waveform )
{

  /* value of 0 means Special burst; 1 means DPD waveform */

  if (tx_waveform == FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST)  /* switch to SB */
  {
    if (ftm_tdscdma_curr_tx_waveform == FTM_TDSCDMA_TX_WAVEFORM_DPD)
    {
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_XPT_TX_MSG, RF_TDSCDMA_MDSP_TFW_XPT_MODE_OFF);
      DALSYS_BusyWait(ftm_tdscdma_tx_wf_switching_delay_us);
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_TX_MSG, TRUE);
      DALSYS_BusyWait(100);
      ftm_tdscdma_curr_tx_waveform = FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST;
    }
    else
    {
      FTM_MSG_HIGH("already in SB. no action. tx waveform = %d ",tx_waveform, 0,0);
    }
  }
  else if (tx_waveform == FTM_TDSCDMA_TX_WAVEFORM_DPD)   /* switch to DPD */
  {
    if (ftm_tdscdma_curr_tx_waveform == FTM_TDSCDMA_TX_WAVEFORM_SPECIAL_BURST)
    {
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_CONT_TX_MSG, FALSE);
      DALSYS_BusyWait(ftm_tdscdma_tx_wf_switching_delay_us);
      rf_tdscdma_mdsp_send_mdsp_msg(RF_TDSCDMA_MDSP_XPT_TX_MSG, RF_TDSCDMA_MDSP_TFW_XPT_MODE_DPD_BURST);
      DALSYS_BusyWait(100);
      ftm_tdscdma_curr_tx_waveform = FTM_TDSCDMA_TX_WAVEFORM_DPD;
    }
    else
    {
      FTM_MSG_HIGH("already DPD waveform. no action. tx waveform = %d ",tx_waveform, 0,0);
    }
  }
  else
  {
    FTM_MSG_ERROR("tx waveform=%d invalid",tx_waveform, 0,0);
  }

} /* ftm_tdscdma_switch_tx_waveform() */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Config Tx Self Calibration Mode

  @details
  This function will enable/disable Tx Self Cal mode.

  @param enable
  TRUE = enable and FALSE = disable

  @return
  None
*/

void ftm_tdscdma_config_tx_selfcal(boolean enable)
{
  /* If Self Cal is enalbed */
  if (enable)
  {
    ftm_tdscdma_current_tx_cal_mode = FTM_TDSCDMA_TX_CAL_SELF;
  }
  /* If Legacy Cal is enalbed */
  else
  {
    ftm_tdscdma_current_tx_cal_mode = FTM_TDSCDMA_TX_CAL_LEGACY;
  }

  FTM_MSG_MED("ftm_tdscdma_config_tx_selfcal: enable = %d.",enable,0,0);
}


#ifdef FEATURE_RF_HAS_QTUNER
/*============================================================================

FUNCTION
ftm_tdscdma_program_qfe_scenario

DESCRIPTION
This API will be called to program the tuner immediately and no script or FW
 action is required. Inside this API check all the active paths then give a call to
 tuner manager associated with appropriate tuners

DEPENDENCIES
  None

RETURN VALUE
  Flag indicating if the api operation was successful (TRUE) or failed (FALSE)

SIDE EFFECTS
  None
=============================================================================*/

boolean ftm_tdscdma_program_qfe_scenario(void)
{
  boolean status = FALSE;

  status = rf_tdscdma_mc_program_qfe_scenario();

  if(!status)
  {
   MSG_ERROR("FTM_TDSCDMA_PROGRAM_QFE_SCENARIO: Failed ",0,0,0);
  }

  return status;

}
#endif /* FEATURE_RF_HAS_QTUNER */


/*============================================================================

FUNCTION FTM_RF_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_tdscdma_fbrx_meas_tx_params
(
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint32 capture_offset,
   uint32 tx_measurement_config,
   ftm_rsp_pkt_type *rsp_pkt
)
{

  rf_time_tick_type prof_tick;
  rf_time_type prof_time = -1;
  uint8 selftest_error;

  ftm_tdscdma_selftest_meas_params_struct params;
  params.device = device;
  params.num_averages = num_averages;
  params.capture_offset = capture_offset;
  params.tx_measurement_config = tx_measurement_config;

  prof_tick = rf_time_get_tick();

  selftest_error = ftm_tdscdma_selftest(
                                         params,
                                         rsp_pkt,
                                         &ftm_rf_tdscdma_fbrx_meas_tx_params_rsp_pkt,
                                         &ftm_rf_tdscdma_fbrx_iq_acquired_samples_data,
                                         NULL,// &ftm_tdscdma_power_dbm,
                                         NULL);// &ftm_selftest_pwr_only);

  if (!selftest_error)
  {
    prof_time = rf_time_get_elapsed( prof_tick, RF_MSEC );
    FTM_MSG_1( FTM_HIGH, "Self_test Time profiling TDSCDMA  "
                           "ExecTime %d ms", prof_time );
  }
  else
  {
    FTM_MSG( FTM_HIGH, "Self_test failed");
  }

}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in one acquisition will be
  contiguous. Samples from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_tdscdma_fbrx_iq_capture_trigger
(
   rfcom_device_enum_type device,
   uint8 sample_size,
   uint8 fbrx_chain,
   ftm_rsp_pkt_type *rsp_pkt
)
{
  ftm_fbrx_test_error_code_type error_code = FTM_FBRX_TEST_SUCCESS;

   boolean capture_done;

   uint16 sample_capture_size = 9216;
   uint32 samp_rate = -1;
   ftm_common_selftest_fbrxgs_info_type fbrx_gs_info;

  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.data_available = FALSE;
  ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.device=device;  

  fbrx_gs_info.override_enable = FALSE;
  switch (sample_size)
  {
   case FTM_FBRX_SAMPLE_SIZE_1024:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 1024; break;
   case FTM_FBRX_SAMPLE_SIZE_2048:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 2048; break;
   case FTM_FBRX_SAMPLE_SIZE_3072:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 3072; break;
   case FTM_FBRX_SAMPLE_SIZE_4096:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 4096; break;
   case FTM_FBRX_SAMPLE_SIZE_5120:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 5120; break;
   case FTM_FBRX_SAMPLE_SIZE_6144:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 6144; break;
   case FTM_FBRX_SAMPLE_SIZE_7168:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 7168; break;
   case FTM_FBRX_SAMPLE_SIZE_8192:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 8192; break;
   case FTM_FBRX_SAMPLE_SIZE_9216:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 9216; break;
    default:
      ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples = 9216;
      FTM_MSG_ERROR( "Number of samples requested %d > Max. value %d",ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples,FTM_TDSCDMA_FBRX_IQ_SAMPLES_MAX,0);	  
      error_code = FTM_FBRX_TEST_IQ_SAMPLES_EXCEEDS_CAPTURE_LIMIT;
      FTM_MSG_1(FTM_ERROR, "Number of samples requested not supported", sample_size);
    break;
  }

   if( ftm_get_current_state(device) != FTM_STATE_TDSCDMA)
   {
	 capture_done = FALSE;
   }
   else
   {
     /* Trigger I/Q capture*/
     capture_done = ftm_common_fbrx_iq_capture_processing(
                                       FTM_RF_TECH_TDSCDMA,
                                       fbrx_chain,
                                       FALSE,                     /* proc_flag */
                                       sample_capture_size,
                                       &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx),
                                       &(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx),
                                       NULL,
                                       NULL,
                                       &samp_rate,
                                       &fbrx_gs_info);

   }
   
   if (capture_done == FALSE)
   {
     error_code = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
   }
   else
   {
     ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.data_available = TRUE;
      /*dump acquired samples in EFS*/
      /*rfcommon_iq_efs_save(
	 	ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx, 
	 	ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx, 
	 	9216);*/
   }

  /***************** Fill up RSP packet with standard data *****************/

  // Fill in the constant fields of the response packet. Sample buffer will be filled after sanity check.
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.cmd_code = 75;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.subsys_id = FTM;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.subsys_cmd_code = ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) ? FTM_TDSCDMA_C : FTM_TDSCDMA_RX_2_C ;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_TRIGGER;  
   
  // Update the final error code in the response packet.
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.ftm_error_code = error_code;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_tx = FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_TX;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.sample_data_format_rx = FTM_TDSCDMA_FBRX_IQ_SAMPLE_DATA_FORMAT_RX;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.sample_size = sample_size;
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.fbrx_chain = fbrx_chain; 
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.sampling_frequency = FTM_TDSCDMA_FBRX_IQ_SAMPLE_FREQ; 
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_tx = FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX; 
  ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt.max_diag_sample_size_rx = FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX; 

  // Fill in the constant fields of the response packet.Size of the packet will be based on the requested sample size.
  rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
  rsp_pkt->pkt_payload = &(ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt);
  rsp_pkt->delete_payload = FALSE;
  rsp_pkt->pkt_len =  sizeof(ftm_rf_tdscdma_fbrx_iq_capture_trigger_rsp_pkt);
  
  FTM_MSG_3(FTM_MED, "Inputs to IQ Capture trigger -> device: %d :sample_size : %d : fbrx_chain : %d",
            device, sample_size, fbrx_chain);
}

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for TDSCDMA from buffer that has
  been used to capture samples in ftm_tdscdma_fbrx_iq_capture_trigger().

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing get
 samples command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_tdscdma_fbrx_iq_capture_get_samples
(
   rfcom_device_enum_type device,
   uint8 sample_source,
   uint32 sample_offset,
   uint32 sample_size,
   ftm_rsp_pkt_type *rsp_pkt
)
{
   uint8 status = FTM_FBRX_TEST_SUCCESS;

   uint16 sample_buffer_size;
   uint16 data_byte_size;
   uint16 data_byte_offset;
  
   int32 *iq_buf_rx_ptr = NULL;
   int32 *iq_buf_tx_ptr = NULL;

  /***************** Do sanity checks *******************************/
   /*Check if IQ has been acquired for the same device*/
  if (ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.device != device)
  {
    FTM_MSG( FTM_ERROR, "IQ samples are not acquired for this device, please acquire them first");	  
	  status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;    
  }
  else
  {
    /*Check if data is available */
    if (ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.data_available == FALSE )
    {
      FTM_MSG( FTM_ERROR, "IQ samples are not acquired, please acquire them first");      
      status = FTM_FBRX_TEST_IQ_SAMPLES_UNAVAILABLE;
    }
    else
    {
      /* Check if number of samples requested is greater than acquired.*/
      if( (sample_offset + sample_size) > ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.req_samples )
      {
        FTM_MSG_2( FTM_ERROR, "Sample offset(%d) or Sample size(%d) is wrong", sample_offset, sample_size);      
        status =  FTM_FBRX_TEST_ILLEGAL_COMBIMATION;	         
      }
      else
      {
        /*Check if the requested sample source is correct*/
        if ((sample_source != FTM_FBRX_IQ_SAMPLE_TX) 
            &&(sample_source != FTM_FBRX_IQ_SAMPLE_RX))
        {
          FTM_MSG_1( FTM_ERROR, "Requested sample source is not valid",sample_source);	   
          status =  FTM_FBRX_TEST_GENERAL_FAILURE;            
        }
        else
        {
          /*check Tx request for max sample size and memory alignment requirements*/
          if(sample_source == FTM_FBRX_IQ_SAMPLE_TX)
          { 
            if(sample_size > FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX) 
          { 
            FTM_MSG_3(FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                           sample_size,
                           FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_TX,
                           sample_source);	   

            status =  FTM_FBRX_TEST_GENERAL_FAILURE;              
          } 
          if(sample_size & 0x01) 
          { 
              FTM_MSG_2(FTM_ERROR, "Requested Sample size %d has to be an even number for source %d",
                             sample_size,
                             sample_source);	   
              status =  FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
          }
          /*check Rx request for max sample size and memory alignment requirements*/
          else if(sample_source == FTM_FBRX_IQ_SAMPLE_RX)
          {
            if(sample_size > FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX) 
          { 
            FTM_MSG_3( FTM_ERROR, "Requested Sample size %d is greater than %d for sample source %d",
                       sample_size,
                       FTM_TDSCDMA_FBRX_IQ_DIAG_MAX_SAMPLES_RX,
                       sample_source);	   
              status = FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
            if(sample_size & 0x07) 
            { 
              FTM_MSG_2( FTM_ERROR, "Requested Sample size %d has to be multiple of 8 for source %d",
                         sample_size,
                         sample_source);	   
            status = FTM_FBRX_TEST_GENERAL_FAILURE;              
            }
          }
          else
          {
            FTM_MSG_1( FTM_ERROR, "Requested Sample type is not suuported %d", sample_source);
            status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;    
          }

          /*if all sanity check passed, go ahead witrh sample dispatch*/
          if (status == FTM_FBRX_TEST_SUCCESS)
          {
            if(sample_source == FTM_FBRX_IQ_SAMPLE_TX)
            {
              data_byte_offset = sample_offset*2;
              iq_buf_tx_ptr = ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx + (data_byte_offset/sizeof(int32));

              /* Buffer for samples and FFT - 8 bit I/8 bit Q*/
              sample_buffer_size = (sample_size * 2);

              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data == NULL)
              {
                ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data = (int32*)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }

              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;	  
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Tx - IQ buffer");
              }
              else
              {
                ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data, FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG, (void *)iq_buf_tx_ptr, sample_buffer_size);

                /* Parse the Tx data*/
                ftm_common_parse_tx_IQ(FTM_STATE_TDSCDMA, ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data, sample_size);
              }
            }
            else if(sample_source == FTM_FBRX_IQ_SAMPLE_RX)
            {
              data_byte_offset = (sample_offset*20)/8;
              data_byte_size   = (sample_size*20)/8;
              iq_buf_rx_ptr = ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx + (data_byte_offset/sizeof(int32));
			  
              /* Buffer for samples and FFT - 32 bit I/32 bit Q*/
              sample_buffer_size = (sample_size * 2 * 4);

              if(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data == NULL)
              {
                ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data = (int32*)ftm_malloc(FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG);
              }
			  
              /* If pointer is NULL even after allocating memory, then declare it as failure*/
              if(ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data == NULL)
              {
                status = FTM_FBRX_TEST_MEM_ALLOCATION_FAILURE;	  
                FTM_MSG(FTM_ERROR, "Unable to allocate memory for Rx - IQ buffer");
              }
              else
              {
                ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes = sample_buffer_size;

                /*get sample_size samples from the Rx buffer*/
                memscpy((void *)ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data, FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG, (void *)iq_buf_rx_ptr, data_byte_size);
			  
                /* Parse the data*/
              ftm_common_parse_PCFL20_to_IQ(FTM_STATE_TDSCDMA, ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data, sample_size);
              }
            }
            else
            {
              status = FTM_FBRX_TEST_ILLEGAL_COMBIMATION;
            }
          }
        }         
      }
    }
  }

   /**************** Fill the packet after check ************* */
  if (rsp_pkt != NULL)
  {    
     /***************** Fill up RSP packet with standard data *****************/

     // Fill in the constant fields of response packet. Sample buffer will be filled after sanity check.
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.cmd_code = 75;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.subsys_id = FTM;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.subsys_cmd_code = ((device == RFM_DEVICE_0) || (device == RFM_DEVICE_2)) ? FTM_TDSCDMA_C : FTM_TDSCDMA_RX_2_C ;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.ftm_rf_cmd = FTM_FBRX_IQ_CAPTURE_GET_SAMPLES;

     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.ftm_error_code = status;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.sample_source = sample_source;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.sample_offset = sample_offset;
     ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.sample_size = sample_size;

     // Fill in the constant fields of response packet.Size of the packet will be based on the requested sample size.
     rsp_pkt->cmd = FTM_RSP_DO_NEW_PKT;
     rsp_pkt->pkt_payload = &ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt;
     rsp_pkt->delete_payload = FALSE;
     /*******Fill up remaining part of RSP packet based on sanity checks and acquired data***********/
     if (status == FTM_FBRX_TEST_SUCCESS)
     {
      // Since all the error checks were a success, copy the IQ data into diag command buffer 
      // Extract the I and Q information from acquired IQ buffer [sample_offset,sample_offset+sample_number) and copy it to diag in the format IQIQIQ     
      if (sample_source == FTM_FBRX_IQ_SAMPLE_TX)
      {                
        memscpy(ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_8_bit,
                FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG,
               ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_tx_rsp_data,
               ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes); 

        // Header size +   (I + Q , each 1 byte) * total number of samples
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.tx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);
      }               
      if (sample_source == FTM_FBRX_IQ_SAMPLE_RX)
      {                 
        memscpy(ftm_rf_tdscdma_fbrx_iq_capture_get_samples_rsp_pkt.sample_buffer.sample_buffer_32_bit,
                FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG,
               ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.iq_buf_rx_rsp_data,
               ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes);

        // Header size +   (I + Q , each 2 byte) * total number of samples 
        rsp_pkt->pkt_len = FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE + ftm_rf_tdscdma_fbrx_iq_acquired_samples_data.rx_iq_data_siz_bytes;
        FTM_MSG_3(FTM_MED, "%d CDMA IQ samples (Source = %d ) sent to Diag with sample offset %d",
                  sample_size,
                  sample_source,
                  sample_offset);               
        }                          
     }
     // If we fail checks, dont fill IQ samples. RSP packet will only have header.
     else
     {               
       rsp_pkt->pkt_len =  FTM_RF_FBRX_IQ_CAPTURE_GET_SAMPLES_HDR_SIZE;
       FTM_MSG_2(FTM_ERROR, "CDMA FBRx get IQ samples failed for SAMPLE SIZE %d and SAMPLE OFFSET %d",
                sample_size,
                sample_offset);
    }
  } 
}

uint16 ftm_tdscdma_trigger_asm_update(ftm_tdscdma_port_state_type port)
{
  boolean error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_FAILURE; /* 0: Success, 1: Error */

  if (port >= TDS_PORT_MAX)
  {
    FTM_MSG_1(FTM_HIGH, "Error executing ftm_tdscdma_trigger_asm_update(): port: %d", port);
    return error_code;

  }
  FTM_MSG_1(FTM_HIGH, "Executing ftm_tdscdma_trigger_asm_update(): port: %d", port);

  if (rf_tdscdma_mc_dynamic_port_switch((uint8)port))
  {
    error_code = FTM_TDSCDMA_RF_MODE_ERROR_CODE_SUCCESS;
  }

  return (uint16)error_code;

}


/*============================================================================

FUNCTION ftm_tdscdma_set_tuner_tune_code_override

DESCRIPTION
  This API is used override the current Antenna tuner tune code settings 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/

ftm_tuner_override_status_type ftm_tdscdma_set_tuner_tune_code_override( rfcom_device_enum_type device, uint8 override_flag, void *data, uint8* tuner_nv_ptr,uint8* tuner_id_ptr)
{
  ftm_tuner_override_status_type status = FTM_TUNER_OVERRIDE_ERROR;

#ifdef FEATURE_RF_HAS_QTUNER 
  status = rf_tdscdma_core_antenna_tuner_tune_code_override( device,
                                                             rf_tdscdma_mc_state.curr_band_rx, 
                                                             override_flag, 
                                                             data, 
                                                             tuner_nv_ptr,
                                                             tuner_id_ptr);
#endif
  return status;
}

void ftm_tdscdma_set_rx_lin_state(uint8 rx_lin_state)
{
  ftm_tdscdma_rx_lin_state = rx_lin_state;
  tds_override_rx_lin_state_val= rx_lin_state;
  FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_set_rx_lin_state(): ftm_tdscdma_rx_lin_state: %d", ftm_tdscdma_rx_lin_state);
}


/*============================================================================
FUNCTION ftm_tdscdma_set_alt_path

DESCRIPTION 
  This API is used override the alt_path
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
void ftm_tdscdma_set_alt_path
( 
  ftm_tdscdma_alt_path_oride_params_type oride_param
)
{
  /* Exit the current device */

  /* Make sure Tx is turned off */
  ftm_tdscdma_set_tx(FALSE);

  /* Make sure Rx is turned off */
  ftm_tdscdma_set_rx(FALSE);
  
  /* Disable Tx */  
  if (rf_tdscdma_mc_state.rf_state[RF_PATH_0] == RF_TDSCDMA_STATE_RXTX)
  {
    if(ftm_tdscdma_current_tx_device != RFM_INVALID_DEVICE)
    {
      rf_tdscdma_mc_disable_tx(ftm_tdscdma_current_tx_device);
    }
  }

  if(ftm_tdscdma_current_pri_device != RFM_INVALID_DEVICE)
  {
    rf_tdscdma_mc_exit_mode(ftm_tdscdma_current_pri_device, 
                            ftm_tdscdma_current_sec_device, 
                            FALSE  //boolean wait_for_fw_release
                            );

    FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_set_alt_path(): Exit on Device %d and %d", 
              ftm_tdscdma_current_pri_device,
              ftm_tdscdma_current_sec_device);    
  }
  else
  {
    FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_set_alt_path(): Skipped calling exit_mode. pri_device %d, sec_device %d", 
              ftm_tdscdma_current_pri_device,
              ftm_tdscdma_current_sec_device);
  }
  
  ftm_tdscdma_current_pri_alt_path = oride_param.master_dev_alt_path_idx;
  ftm_tdscdma_current_sec_alt_path = oride_param.slave_dev_alt_path_idx;
  ftm_tdscdma_current_pri_device   = oride_param.master_dev_id;
  ftm_tdscdma_current_tx_device    = oride_param.master_dev_id;
  ftm_tdscdma_current_sec_device   = oride_param.slave_dev_id;
  ftm_tdscdma_override_alt_path    = TRUE;

  /* Enter mode on the new device*/
  if(oride_param.master_dev_id != RFM_INVALID_DEVICE)
  {
    rf_tdscdma_mc_enter_mode(oride_param.master_dev_id, 
                             oride_param.slave_dev_id, 
                             ftm_tdscdma_rxlm_buffer[FTM_RECEIVE_CHAIN_0].buf_idx,
                             RFA_TDSCDMA_RXLM_MODE_ACQ,
                             FALSE // is_dsds
                             );

    FTM_MSG_4(FTM_HIGH, "ftm_tdscdma_set_alt_path(): Updated Device %d with alt_path %d and Device %d with alt_path %d", 
              oride_param.master_dev_id,
              oride_param.master_dev_alt_path_idx,
              oride_param.slave_dev_id,
              oride_param.slave_dev_alt_path_idx);      
  }
  else
  {
    FTM_MSG_2(FTM_HIGH, "ftm_tdscdma_set_alt_path(): Skipped calling enter_mode. pri_device %d, sec_device %d", 
              ftm_tdscdma_current_pri_device,
              ftm_tdscdma_current_sec_device);
  }    
}


/*============================================================================
FUNCTION ftm_tdscdma_set_devices

DESCRIPTION 
  This API sets pri, sec and tx devices
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
=============================================================================*/
void ftm_tdscdma_set_devices
(
  void *ftm_req_data
)
{
  PACK(struct) ftm_tdscdma_set_devices_req_s /* Input parameter structure */
  {
    uint8   cmd;
    uint8   sub_sys;
    uint16  mode;
    uint16  tdscdma_cmd;
    uint8   pri_device;
	uint8   sec_device;
	uint8   tx_device;
  } *set_devices_req_ptr;

  ftm_tdscdma_set_devices_params_type devices_params =
  {
    RFM_DEVICE_0,  //pri_device
    RFM_DEVICE_1,  //sec_device
    RFM_DEVICE_0,  //tx_device
  };

  set_devices_req_ptr = (struct ftm_tdscdma_set_devices_req_s *)ftm_req_data;

  devices_params.pri_device = (rfm_device_enum_type)set_devices_req_ptr->pri_device;
  devices_params.sec_device = (rfm_device_enum_type)set_devices_req_ptr->sec_device;
  devices_params.tx_device =  (rfm_device_enum_type)set_devices_req_ptr->tx_device;


  if ((devices_params.pri_device == RFM_DEVICE_0) || (devices_params.pri_device == RFM_DEVICE_2))
  {
     ftm_tdscdma_current_pri_device = devices_params.pri_device;
  }
  else
  {
    FTM_MSG_ERROR("ftm_tdscdma_set_devices:Invalid primary device %d", devices_params.pri_device,0, 0);
	return;
  }

  if ((devices_params.sec_device == RFM_DEVICE_1) || (devices_params.pri_device == RFM_DEVICE_3))
  {
     ftm_tdscdma_current_sec_device = devices_params.sec_device;
  }
  else
  {
    FTM_MSG_ERROR("ftm_tdscdma_set_devices:Invalid secondary device %d", devices_params.sec_device,0, 0);
	return;
  }

  if ((devices_params.tx_device == RFM_DEVICE_0) || (devices_params.tx_device == RFM_DEVICE_2))
  {
     ftm_tdscdma_current_tx_device = devices_params.tx_device;
  }
  else
  {
    FTM_MSG_ERROR("ftm_tdscdma_set_devices:Invalid TX device %d", devices_params.tx_device,0, 0);
	return;
  }
  FTM_MSG_3(FTM_HIGH, "ftm_tdscdma_set_devices(): Set devices: pri_device %d, sec_device %d, tx_device %d", 
			ftm_tdscdma_current_pri_device,
            ftm_tdscdma_current_sec_device,
			ftm_tdscdma_current_tx_device);
}

/*============================================================================

FUNCTION ftm_tdscdma_get_pa_switchpoints

DESCRIPTION
  This API is used extract the PA switchpoints for a given band 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
boolean ftm_tdscdma_get_pa_switchpoints(rfm_device_enum_type tx_device, rfcom_tdscdma_band_type band, int16* pa_rise_swpts, int16* pa_fall_swpts, int16* active_pa_states)
{
  boolean status = FALSE;

  FTM_MSG_1(FTM_HIGH, "ftm_tdscdma_get_pa_switchpoints(): Input argument: band: %d", band);

  if ((pa_rise_swpts == NULL) || (pa_fall_swpts == NULL) || (active_pa_states == NULL))
  {
    FTM_MSG(FTM_ERROR, "ftm_tdscdma_get_pa_switchpoints(): Input argument is a NULL pointer! Bailing out.");
    return status;
  }

  if (tx_device != RFM_DEVICE_0)
  {
    FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_get_pa_switchpoints(): tx_device is incorrect(%d)! Bailing out.", tx_device);
    return status;
  }

  switch (band)
  {
     case RFCOM_BAND_TDSCDMA_B34:
       if (rfnv_tdscdma_get_pa_swpts(RFCOM_BAND_TDSCDMA_B34, pa_rise_swpts, pa_fall_swpts, active_pa_states))
       {
         status = TRUE;
       }
       else
       {
         FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_get_pa_switchpoints(): PA_STATIC Not populated for TD-SCDMA band id: %d!", band);
       }
       break;
     case RFCOM_BAND_TDSCDMA_B39:
       if (rfnv_tdscdma_get_pa_swpts(RFCOM_BAND_TDSCDMA_B39, pa_rise_swpts, pa_fall_swpts, active_pa_states))
       {
         status = TRUE;
       }
       else
       {
         FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_get_pa_switchpoints(): PA_STATIC Not populated for TD-SCDMA band id: %d!", band);
       }
       break;
     default:
       FTM_MSG_1(FTM_ERROR, "ftm_tdscdma_get_pa_switchpoints(): band id is incorrect for TD-SCDMA(%d)! Bailing out.", band);
       break;
  }

  return status;

}


/*! @} */

#endif /* FTM_HAS_TDSCDMA */
#endif /* FEATURE_FACTORY_TESTMODE */
