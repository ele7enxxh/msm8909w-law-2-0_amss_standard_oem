#ifndef RFM_INTERNAL_H
#define RFM_INTERNAL_H

/*!
   @file
   rfm_internal.h

   @brief
   This file contains all RFM interfaces to be used internally within RF only.
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/mc/inc/rfm_internal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/15/15   rs      Reduce DIAG logging volume from C2K
07/13/14   ec      Add debug flag to enable/disable sawless features
04/04/14   spa     Remove "use_default_btf" flag
02/21/14   sty     Added flag to provide hard-coded BTF value for 1x - till 
                   RxLM and TxLM hooks are in place
01/20/14   spa     Added flag imm_lna_write_cal for immediate LNA writes in cal 
01/15/14   spa     Remove append_tx_on flag , hack is no longer required
12/06/13   JJ      use script write for wakeup tx and remove immedate-write flag
12/02/13   sty     Added retune_delay to rf_cdma_bup_flags_t
11/21/13   JJ      Added use_imm_writes_for_tx debug flag
11/20/13   sty     Removed redundant debug flag 
11/19/13   JJ      Deleted skip_direct_writes flag 
11/18/13   sty     deleted skip_mcpm_stop flag 
10/28/13   APU     Additional debug flags to skip DVGA\ICIFIR during BU. 
10/24/13   spa     Add use_fake_nv_init debug flag for CDMA 
10/17/13   ggs     Add support to override amam and ampm tables 
                   with default values and disable
                   pre-distortion and disable Vbatt/temp comp
                   for BOLT using debug flags
08/28/13   JJ      Add bolt_rumi in rf_cdma_bup_flags_t
07/10/13   JJ      Rename lagacy flag rumi_bup_cdma  
07/10/13   JJ      remove enable_hdr_rxagc_logging flag
06/02/13   JJ      HDR sleep time profiling 
05/31/13   sty     deleted unused ant_tuner_script flag 
05/30/13   spa     Mainlined "use_new_fwrsp", use new RF dispatch implementation 
05/10/13   spa/sty Added debug flag enable_hdr_rxagc_logging
04/25/13   spa     Added send_pilot_meas_config debug flag for FTM IRAT 
04/25/13   cd      Temp Comp - support for multi-chan linearizer compensation
                   - Multi-algorithm support based on NV versioning
04/24/13   sty     Removed ftm_1x_retune_framevalid_delay
04/22/13   fh      Added ftm_1x_retune_framevalid_delay
03/20/13   cd      XPT Temp comp support
02/27/13   aro     Mainlined debug flag for Tx resource manager
02/21/13   aro     Added debug flag to enable tx resource manager for CDMA
02/21/13   Saul    CDMA. Added xpt_temp_comp_enabled debug flag.
02/20/13   zhw     Remove unused API 
01/28/13   spa     Remove refs to device_scripting flag
01/08/13   shb     Mainlined ssbi_on_off_supported
12/19/12   zhw     HDET debug flag
11/15/12   aro     Mainlined variable to enable new cal v3 opcode
11/15/12   aro     Variable to enable new cal v3 opcode
11/07/12   zhw     Add a CDMA debug flag for RF device scripting
11/05/12   sty     Added use_icvr to enable/disable I-ceiver
10/31/12   php     Remove TxAGC enable flag
10/26/12   spa     Added flag ssbi_on_off_supported
10/25/12   php     Add flag to disable TxAGC
10/19/12   Saul    CDMA. Removed code to profile writing dpd tables.
10/16/12   tsr     Cleanup GSM RF bringup flags 
09/27/12   zhw     Remove do_rffe_write debug flag
08/28/12   sb/tsr  Added disable temp comp flag for bringup  
08/25/12   spa     Added CDMA debug flag for direct RFFE write 
08/21/12   sc      Cleanup the GSM RF bringup flags
08/10/12   tsr     RF GSM CCS debug and Time profile flags added
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
08/02/12   zhw     Added a CDMA debug flag for CCS events
07/29/12   ac      added wcdma debug flags for rumi bringup
07/18/12   jyu     Added TDSCDMA debug flags for RUMI Bring Up 
07/12/012  tsr     Modified GSM debug Flags for RUMI Bring up  
07/12/012  tsr     Added GSM debug Flag for RUMI Bring up 
07/06/12   php     Add LTE debug flags for RUMI Bring Up
07/05/12   zhw     Added RUMI debug flag.
08/16/12   aro     Temp variable to enable/disable new FW-SW Sync
05/29/12   zhw     Refactored API naming. Removed externed debug APIs.  
05/29/12   zhw     Moved debug flag do_grfc_writes_in_rf_sw to rf_cdma_data
05/20/12   zhw     Removed cdma debug flag: rf_hdr_wait_time_after_tx_start
05/19/12   zhw     Moved debug flag do_sbi_writes_in_rf_sw to rf_cdma_data
04/17/12   sty     Deleted references to enable_sbi_scripting
03/29/12   sty     Added new debug flag - do_grfc_writes_in_rf_sw
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/13/12   aro     Added debug flag to enable/disable new RF_TASK dispatcher
03/02/12   aro     Mainlined Enhanced Rx Calibration
02/06/12   cd      Removed calv3 multi-read HDET debug flag 
02/02/12   sty     Deleted is_voice and pa_backoff flags from rf_cdma_bup_flags_t
01/11/12   aro     Enabled application of LNA Phase offset
01/23/12   sty     Deleted btf_from_nv and btf from rf_cdma_bup_flags_t 
01/09/12   aro     Debug flag to enable/disable usage of new LNA phase offset
12/06/11   aro     Added debug flag to enable/disable enhanced Tx Calibration
12/05/11   aro     Added debug flag to enable/disable Enhanced Rx calibration
12/02/11   cd      Added debug flag to use default IQ gain  
11/21/11   aro     Mainlined debug variable to use new RF task dispatch
11/14/11   aro     Mainlined iC task debug flag
11/14/11   aro     Mainlined the debug flags
11/10/11   aro     Added debug variable to enable/disable new RF TASK
                   CDMA dispatch control
11/09/11   cd      Added debug flag to disable temp comp
10/28/11   aro     Deleted enable_ic_timer debug flag
10/26/11   aro     Added debug flag to choose between old and new iC polling
                   implementation
10/18/11   cd      Removed multi_lin_cal debug flag
10/12/11   bmg     Added default max power limit for lim vs temp integration.
09/29/11   cd      - Remove debug flag use_hdr_to_enable_fw
                   - Remove debug flag wait_for_hdr_fw_enable
09/27/11   adk     Moved SSBI scripting-related flags to rf_cdma_debug_flags.
09/27/11   aro     Added debug flags to switch between old and new RC Tuner
09/26/11   adk     Added debug logs in SSBI scripts.
09/23/11   aro     Removed debug flag to switch between new and old RC Tuner
09/21/11   aro     Added DAL based antenna tuner support
09/21/11   aro     Added debug flag to enable/disable new RC Tuner interface
08/30/11   shb     Deleted maxpwr_lim_for_1x/hdr from bu debug flags - handled 
                   by power limiting module
08/29/11   aro     Added temporary PA Switchpoint backoff
08/29/11   aro     Added support to fetch BTF value from NV
08/16/11   cd      Added debug flag to turn on/off usage of multi-linearizer 
                   data 
08/11/11   aro     Removed intelliceiver enable flag
08/04/11   aro     Removed debug flag to override PM for no iC case
07/13/11   sty     Deleted use_rex_sigs_for_msgr flag
07/13/11   sty     Fixed merge error
07/13/11   sty     Deleted use_rex_sigs_for_msgr flag
07/13/11   aro     Added debug code to disable intelliceiver timer
07/06/11   aro     Added RFM Lock Data and accessor function
06/29/11   sty     Added maxpower limit for 1x and HDR 
06/27/11   aro     Removed Cal V3 debug flag
06/15/11   aro     Added debug variable for intelliceiver debug
06/15/11   aro     Removed RxAGC Sampling time debug variable
06/10/11   sty     Deleted debug flag rf_rx_msg_wait_time
06/09/11   aro     Added debug flag for Calibration Sweep V3
06/09/11   sty     Added use_rex_sigs_for_msgr flag 
06/02/11   aro     Removed debug flags
06/02/11   sty     Deleted use_hardcoded_tx_lut_vals
06/02/11   aro     Added debug flag to overrride PM for NV query
06/02/11   sty     Deleted rf_rx_msg_wait_time
06/01/11   aro     [1] Added debug flag for wait tiem for RxAGC sampling
                   [2] Added debug flag to use QRCT GUI for SV Chain
05/31/11   aro     Removed read_cdma_nv debug flag
05/25/11   sty     Added debug flag - rf_rx_msg_wait_time
05/25/11   sty     Deleted send_1x_rx_start and send_1x_rx_stop
05/24/11   sty     Deleted rf_rx_msg_wait_time
05/24/11   sty     Added debug flag to wait when HDR FW app is enabled 
05/24/11   aro     Added debug code to enabvle SV
05/23/11   sty     Added test_wait_after_tx_start
05/19/11   sty     Pruned rf_cdma_debug_flags - added hook for BTF
05/17/11   sty     Updated rf_cdma_debug_flags 
05/16/11   aro     Added debug flags to wait time and bypass nv
05/13/11   aro     Added Flag to bypass MCPM in FTM
05/11/11   aro     Added flags to skip Stop Rx and skip enable FW App
05/11/11   sty     Added flags to skip NV read during Rx AGC config
05/10/11   sty     Added bringup flags to skip HDR AGC config from NV
05/10/11   sty     Added bringup flags for CDMA
04/04/11   aro     Removed redundant function to get current RFM Mode
03/31/11   sty     Added prototype for rfm_get_rfm_mode_for_device()
03/21/11   sty     Changed API name to rfm_is_rfm_init_success
03/21/11   sty     Added prototype for rfm_get_rfm_initialized_state
03/02/11   aro     Moved Set/Get Calibration state interface to RF
01/15/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_mode_types.h"
#include "rfm_device_types.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
typedef struct 
{
  int32   default_max_power_limit;
  boolean disable_temp_comp;
  boolean use_default_iq_gain;
  boolean temp_workaround;
  boolean use_icvr;
  boolean disable_hdet;
  boolean send_pilot_meas_config;
  boolean bolt_rumi;
  int32   retune_delay; /* in us */
  boolean imm_lna_write_cal;
  boolean enable_f3_logging;
} rf_cdma_bup_flags_t;

/*----------------------------------------------------------------------------*/
extern rf_cdma_bup_flags_t rf_cdma_debug_flags;

/*----------------------------------------------------------------------------*/
typedef struct 
{
  boolean rumi_bup_lte;
} rf_lte_bup_flags_t;
/*----------------------------------------------------------------------------*/
extern rf_lte_bup_flags_t rf_lte_debug_flags;

/*----------------------------------------------------------------------------*/
typedef struct 
{
  boolean rumi_bup_tdscdma;
} rf_tdscdma_bup_flags_t;
/*----------------------------------------------------------------------------*/
extern rf_tdscdma_bup_flags_t rf_tdscdma_debug_flags;

typedef struct 
{
  boolean rumi_bup_wcdma;
} rf_wcdma_bup_flags_t;
/*----------------------------------------------------------------------------*/
extern rf_wcdma_bup_flags_t rf_wcdma_debug_flags;
/*----------------------------------------------------------------------------*/
typedef struct 
{
  boolean disable_ant_tuner;          /* Disable GSM Antenna Tuner Flag */
  boolean time_profile;               /* Time Profiling Flag */
  boolean disable_temp_comp;          /* Disable Temp comp */
  boolean disable_vbatt_comp;         /* Disable Vbatt comp*/
  boolean rfgsm_init_only;            /* rfgsm_mc_init only flag */
  boolean override_amam_ampm_tables;  /* Override amam ampm tables with default values */
  boolean predist_override;           /* Disable Pre-distortion */
  boolean sawless_override;           /* Disable SAWless features */
} rfgsm_bup_flags_t;
/*----------------------------------------------------------------------------*/
extern rfgsm_bup_flags_t rfgsm_debug_flags;

extern boolean rfwcdma_mc_presilicon_bypass_flag;
/*----------------------------------------------------------------------------*/
rfm_mode_enum_type
rfm_get_current_mode
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
rfm_set_calibration_state
(
  boolean cal_enable
);

/*----------------------------------------------------------------------------*/
boolean
rfm_get_calibration_state
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rfm_is_rfm_init_success
(
  void
);

rf_lock_data_type*
rfm_get_lock_data
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RFM_INTERNAL_H */
