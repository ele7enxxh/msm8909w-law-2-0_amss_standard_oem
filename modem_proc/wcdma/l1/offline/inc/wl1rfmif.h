#ifndef WL1RFMIF_H
#define WL1RFMIF_H

/*===========================================================================
                WL1 RF Driver interface header file

DESCRIPTION
  This file is WL1 RF Driver interface header file that supports multimode
  external interface while maintaning backward compatibility. This file
  contains all declarations and definitions necessary for WL1 to use the
  RF drivers. WL1 modules call inline function in this and this takes care of
  all RF API changes featurization.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None. All are passive inline function calling RF modules based on feature.
  All initialization requirment of respective RF modules should be followed

Copyright (c) 2006 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1rfmif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------------------------------------------------------------------------------------------------------- 
08/22/16   db      Call RF Enter/Exit for W2W measurements to support WTR LPM feature in RF
09/14/15   rn      Added Rumi framework enhancement changes for searcher module.
09/07/15   pv      Block all RF API calls in FTM mode.
08/11/15   kr      replace memcpy as its been deprecated
05/11/15   kr      set RF state to awake from tune not from enter mode
01/21/15   nd      Made changes for QTF with CRM
01/29/15   psr     Added wrapper for rfm_wcdma_get_tx_max_power_nv()
12/22/14   kr      do combined wakeup for dsds
10/07/14   sks     Compilation fix. 
09/17/14   mit     Skip enter mode calls from wl1 in FTM mode
09/05/14   pv      Removal of calls to unused RF APIs and enabling latest BO supported RF SLEEP API for Jolokia. 
08/28/14   vr      Mainlining code under FEATURE_WCDMA_DIME_SW, FEATURE_WCDMA_DIME_TX_STATE_REPORTING,
                   FEATURE_TRITON_MODEM for BO.2.0.
08/13/14   mit     FTM_RF/FTM_cal optimizations in WL1
08/13/14   sks     Skip sending AGC disable for early RF shutdown.
08/08/14   ymu     Fix Compilation warnings
08/04/14   sks     Set the correct status for the primary RF chain by checking with WRM rather than by hard coding it.
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
07/18/14   sad     RF api changes and inter freq tune changes
07/15/14   bj      WCDMA Narrow Band Rejection for System Search.
07/11/14   gp      Added support for RF Temp Comp Split architecture
07/10/14   pv/sks  Replaced all tune to chan calls by prep tune and exec tune. Replaced sleep_rx by rx_sleep.
07/03/14   sks     Changes for FCDC
07/02/14   sks     Temporarily revert the previous changes
06/26/14   pv/sks  Replaced all tune to chan calls by prep tune and exec tune. Replaced sleep_rx by rx_sleep.
06/24/14   mk      Support for changing EARFCN variable type from uint16 to uint32.
06/11/14   sks     Update the div chain awake status when doing a combined RF wakeup of both PRx and DRx
06/03/14   tsk     Compilation fixes for JOLOKIA 1.0
05/29/14   sks     Combined RF wakeup and tune optimizations on BO.2.0
05/27/14   mm      Removed featurization for DUAL_SIM
05/20/14   ar      Cleanup FEATURE_DIME_MODEM
05/19/14   vs      Removing RFM dynamic update API calls.
05/14/14   sks     IDLE Wakeup Optimizations and cleanup
04/18/14   ks      Pass correct rxlm index values for w2w build scripts
02/12/14   sad     DB-3C API check in
04/01/14   pj      DIME PM RUMI support changes
03/24/14   ac      Changed RFM_IMT_MODE to RFCOM_WCDMA_MODE
03/12/14   sks     Call rfm_Exit_mode when L1 stack deactivates.
03/10/14   ks      Change W2W tuning to have target param always use SC carrier type
03/10/14   ks      Bolt FED API checkin
02/14/14   km      rfm_tune_chan featurization consistent with rf api
01/30/14   ymu     Call pre_rf_init for DC-HSUPA call setup.
01/21/14   abs     Mainlining wl1_rfm_get_ul_freq_from_dl_arfcn()
12/13/13   sr      Fixed a merge issue for wl1_rf_carrier_enum_type.
12/11/13   mm      3C CM/IRAT support
12/10/13   pv      Support of RFM PREP and EXEC wakeup APIs.
12/03/13   vc      Added a function to get rf default band and channel info.
11/21/13   tsk     FR2573: SAR management with Integrated DSDA changes.
10/23/13   sm      Set RxD wakup flag when enter mode is called for correct behavior in PCH
11/05/13   vs      cleaned up the API wl1_rfm_get_curr_band() 
10/23/13   sad     Update to inverse DL freq calculations
10/17/13   bs      Preliminary 3C Support
10/15/13   vs      Modified the HS api for LNA config.
10/11/13   cc      Add a current Rx mode check before acquiring the 2nd device id 
10/03/13   pv      Back out previous check-in.
10/08/13   rsr     3C API Changes Checkin
09/30/13   cc      Fix the incorrect band issue for DBDC interF
09/29/13   as      Continuing port of Dual Sim from Triton to Dime.
09/23/13   as      Porting Dual Sim from Triton to Dime.
09/12/13   jd      Searcher redesign
09/03/13   km      kw fix
08/27/13   rsr     Changed API call for Div. Only mode from "Enter_mode" to "Rfm_wakeup"
08/12/13   cc      DBDC CM W2W feature check in
08/12/13   km      Featurize rfm_tune_chan until RF checks in DBDC changes
07/26/13   at      rfm_wcdma_wakeup_tx() should not be called back-to-back after rfm_init_wcdma_tx()
07/24/13   jd      Changes for Dual band Dual carrier
07/08/13   mm      Update the internal W band enum type to the one used by RF
07/02/13   yw      Extend RF API to Freeze TxAccum for Sec carrier.
                   Extend rfm_init_wcdma_tx to notify DC cfged mode.
05/22/13   oh      Adding DC-HSUPA framework support
04/23/13   pr      Use new rfm_wakeup_tx api and rfm_init_tx_needed flag to reduce RF wakeup times.
03/29/13   pkg     Tx state reporting to MCS for DIME.
03/12/13   at      RF API to decfg Txlm buffer for secondary Tx path
02/22/13   ms      Check div chain RxLM buffer allocation instead of RxD active state
02/05/13   ms      Added support for HS RxLM
01/28/13   dp      Added support for Antenna Tuner
02/05/13   pv      Support for FR 2001 - RF Wakeup Optimization.
01/27/13   geg     Removed FEATURE_WCDMA_RUMI_SUPPORT flag from RxAGC API's as RF support is now available.
01/15/12   ash     Removed FEATURE_WCDMA_RUMI_SUPPORT flag from rfm_do_temp_comp() as RF support is now available.
01/14/12   ash     Removed FEATURE_WCDMA_RUMI_SUPPORT flag from rfm_set_tx_pwr_limit() as RF support is now available.
12/07/12   scm     Pass trm_get_rf_device() return value into RFM APIs.
08/29/12   pv      Changed the RF awake API and moved flag update to end of API.
08/03/12   gsk     Moving rfm_pretune func inside FEATURE_WCDMA
08/03/12   gsk     Updating the RxAGC read with new RF api
08/01/12    pv     New AGC read APIs for Offline PICH.
11/20/12   gsk     Support for Idle mode search and reselections.
11/19/12   pr      Fixed Merge Error.
11/17/12   cc      W2W interF and CM feature for Dime
11/09/12   pr      Remove FEATURE_RUMI_SUPPORT for all supported api's. 
10/23/12   ash     Changes for Dime Bring up
10/01/12   ash     Fixed Dime Compiler warnings
09/20/12   ash     Dime Tx bring-up changes
08/23/12   gsk     Backing out changes for CR 380698
08/15/12   gsk     Moving rfm_pretune func inside FEATURE_WCDMA
08/15/12   gsk     Updating the RxAGC read with new RF api
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/31/12   at      Added new CM API support for PA gain state and correct TX power support
07/05/12    pv     Offline PICH changes - tracking div chain status.
07/05/12   gsk     Idle mode FRs 1782/1705/1947/2152/1950
06/04/12   gsk     Fix for Reacq failure on serving during interband tuning.
05/15/12   gsk     FR 1782: WCDMA Inter- Frequency Tuning Optimization.
06/29/12   zr      Added function to get uncompensated RxAGC
05/08/12   pr      Added API to inform RF state when CPC-DRX or E-DRX is active
05/03/12   vr      Fixed warning.
04/19/12   yh      Pass extra flag for rfm_meas_exit() for W2L
04/17/12   at      Fix in RF API for GSM dependency.
04/16/12   at      Remove RF Tx enable from UL PhyChan Add for NikeL
03/30/12   at      Added new CM API support for PA gain state and correct TX power support
03/21/12   ar      For invalid RF CAL data, WL1 will call CM API to make UE go to 
                   offline mode and send Cphy_setup_failure to RRC.03/19/12   vr      Added check to avoid RF TX disable twice
03/07/12   zr      Adding function that converts an Rx EARFCN to its corresponding
                   carrier frequency
02/01/12   gnk     Nikel Feature cleanup.
01/27/12   vs      Nikel feature cleanup.
01/07/12   stk     Updated the rfm_get_receive_agc_val_for_rscp with correct parameters for NIKE targets.
                   This API will now return the RAW agc values in dbm instead of noise compensated value.
11/30/11   rc      Added DSR support under FEATURE_WCDMA_DSR_SUPPORT   
11/29/11   gnk     Removed some unused features
10/20/11   gnk     Removed MTD related code for NikeL
09/21/11   sp      Added NPA Support for Thermal Mitigation under FEATURE_WCDMA_NPA_THERMAL
09/16/11   sj      Added support for RxD API to change Rx Chain Configuration for next DCH state
09/06/11   vsr     Update APIs to work in Div-Only mode
08/20/11   oh      Call rfm_init during ul phychan add.
08/11/11   mmr     Removed xo calls from rfm interface. Added an api to get
                   freq from a given ARFCN.
08/03/11   dp      Re-worked dynamic tune time code for NikeL bringup.
07/07/11   hk      Up-merge from offline/main/latest
06/30/11   mk      Warning fixes
06/17/11   mk      Removed depricated RF API includes rfagc.h
06/09/11   dp      Checked in fixes for dynamic tune time code
05/27/11   mmr     Added XO fixes
05/26/11   dp      Added changes to make irat tuning time value dynamic
05/26/11   dp      Added support for W2W XO Parameter passing to FW
04/28/11   mmr     Added support for Feature XO.
04/22/11   yh      Removed RxLM buf idx from CM RF functions
04/13/11   yh      Enabled RxLM code for RUMI builds
04/04/11   oh      Changes to support UL DCH on NikeL
04/04/11   yh      Added support for RxLM/TxLM
03/30/11   gnk     Mainlined several features
03/17/11   sv      Added RF wrapper function to get rxagc for diversity chain.
02/17/11   gnk     Populate the MTPL to FW for the correct antenna
02/11/11   sp      Added support for Thermal Mitigation under FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
12/01/10   dp      modified featurization around rfm_meas_common_xxxx functions 
                   (and #include dependancies) so rf script tuning works in 
                   idle w2l
10/27/10   mm      Corrected typo in RF API name
10/15/10   mr/yh   Added support for W2L Compressed mode
10/04/10   sv      Fix compiler warning.
08/30/10   yh      Make carrier_spacing uint16
07/09/10   vsr     Updated arguments in wl1_rfm_tune_to_chan_rx()
06/19/10   sv      Added function that returns RF AGC for secondary carrier.
06/18/10   pv      Fixed compiler warnings.
06/02/10   sa      Changed function "wl1_rfm_get_tx_agc_val_in_dB12" to
                   wl1_rfm_get_tx_agc_val_in_mdsp_units to support txagc in 
                   dB10 format. 
05/21/10   yh      Added support API to convert sys_band to rf_card_band
03/19/10   yh      Add DC+CM support.
03/17/10   rgn     Added code to switch between sc and dc mode
03/17/10   rgn     Added code to control dc carrier spacing 
02/26/10   rvs     Fixes for Mustang.
11/18/09   gv      Use db10 instead of db12 units for computation
10/24/09   ms      Featurization changes
05/29/09   m       Introduced wrapper function for wl1_rfm_get_tx_pwr_limit
04/29/09   mg      Fixed compiler warnings
02/18/09   prk     Changed the declaration of Inline functions to pass offline builds.
01/28/09   rmak    Added state variable for tracking WL1's knowledge of RF On/Sleep
12/01/08   saik    Enabling rfm_sleep in wl1_rfm_sleep by taking out #ifndef RUMI_BRING_UP
11/25/08   asm     Removed RUMI_BRING_UP featurization from freezeaccum and pa_off functions
11/13/08   yh      Add CM support
10/07/08   gnk     Added support for logging tx accum from SW side
10/03/08   hk      Bringing in 7k Mailine fixes
09/06/07   stk     Changed RFM_IMT_MODE to RFCOM_WCDMA_MODE, which is passed to rfm_enter_mode()
08/15/07   mg      Remove rf_pm_ldo_on_has_extra_delay declaration.
08/02/07   asm     New API to feed RF the beta values and HS status
05/31/07   nag     Featurized functions for the EDGE only build
05/02/07   ms      Created separate APIs for HS & R99 LNA switchpoint 
                   programming.
04/30/07   ms      Featurized LNA switchpoint programming implementation under
                   FEATURE_HSDPA_PROGRAM_LNA_SWITCH.
04/16/07   ms      Added API's to query RF whether LNA switchpoint update
                   is required or not and to program LNA NV items to mDSP
03/01/07   vrao    Added rfm_indicate_txagc_threshold_crossing() to process
                   FW interrupt based on WCDMA Tx AGC
02/09/07   asm     Changed RF API to set Tx General Ctl register
12/20/06   rmak    Changed wl1_rfm_get_rf_warmup_time to return uint32
12/11/06   mg      Add wl1_rfm_get_rf_warmup_time
12/08/06   rmak    Mainlining FEATURE MODEM SLEEP CONTROLLER INSTANCE 1
                   and FEATURE INTER SYSTEM TIMING MAINT THROUGH SLEEPCTLR
10/30/06   rmak    rf_pm_ldo_on_has_extra_delay featurized under
                   FEATURE INTER SYSTEM TIMING MAINT THROUGH SLEEP CTLR
08/29/06   rc      Added wl1_rfm_get_ue_max_tx_power_in_dB12() 
                   and wl1_rfm_get_tx_agc_val_in_dB12() functions
                   for Evt 6D evaluation.
08/16/06   rc      Changed RF API calls in wl1_rfm_get_ue_max_tx_power() 
                   and wl1_rfm_get_tx_agc_val() functions. 
08/15/06   rmak    Add extern rf_pm_ldo_on_has_extra_delay to determine RF LDO timing
07/19/06   yh      Fixed compiler error on Saber in wl1_rfm_do_tx_pwr_limit()
07/17/06   yh      Added new functions wl1_rfm_set_tx_pwr_limit, wl1_rfm_enable_tx,
                   wl1_rfm_do_tx_pwr_limit, wl1_rfm_do_temp_comp and
                   wl1_rfm_disable_tx to support new RFM API.
07/17/06   asm     Created function to determine if DM needed for HDET
05/26/06   bd      Fixed compiler error on Saber in wl1_rfm_get_rx_agc_val()
05/19/06   au      Unified interface to RF APIs through wl1rfmif header.
03/14/06   scm     Implement wl1_rfm_get_curr_band().
02/20/06   bbd     Removed rf_l1_cmn_defs.h per RF driver request
01/18/06   yiz     Fixed compile errors due to MSM specific features
01/17/06   gs/bd   Updated RFAPI calls
01/13/06   gs      Created this file
===========================================================================*/


#include "wcdma_variation.h"
#include "rfm.h"
#include "rex.h"

#ifdef FEATURE_WCDMA

/* ---------------------------- */
/* Check for necessary features */
/* ---------------------------- */

#include "l1utils.h"
#include "wl1xlm.h"
#include "wl1trm.h"
#include "rxdiv_v.h"
#include "rxlm_intf.h"
#include "rfm_wcdma.h"

#if defined(FEATURE_WCDMA_TO_LTE) && (defined(FEATURE_WCDMA_CM_LTE_SEARCH) || defined(FEATURE_W2L_IDLE_FW_RF_TUNE))
#include "rfmeas_lte.h"
#include "rfmeas_wcdma.h"
#include "rfmeas_types.h"
#include "rfm_meas.h"
#include "rfm_mode_types.h"
#endif

/* SAR feature set is a superset of DIME TX state reporting. */
#ifdef FEATURE_WCDMA_SAR_TX_PWR_FILTER
#include "wl1ulsarmgr.h"
#endif /* FEATURE_WCDMA_SAR_TX_PWR_FILTER */
#include "rfm_wcdma_param_types.h"

/* =======================================================================
**                        Externed Variables
** ======================================================================= */

extern rex_crit_sect_type wl1xlm_crit_sect;

#ifdef FEATURE_WCDMA_RUMI_SUPPORT
extern uint8 wl1_rumi_dc_enable;
#endif

/* Num of RF source items for Single-Band */
#define WL1_RF_HEADER_SB_NUM_SOURCE 1
/* Num of RF source items for Dual-Band */
#define WL1_RF_HEADER_DB_NUM_SOURCE 2

/* ENUM to keep track of RF TX state*/
typedef enum 
{
  WL1_RF_TX_ENABLED,
  WL1_RF_TX_DISABLED
} wl1_rf_tx_status_enum;

/* Carrier information */
typedef enum
{
  WL1_RFM_SINGLE_CARRIER,
  WL1_RFM_DUAL_CARRIER,
  WL1_RFM_3_CARRIER,
  WL1_RFM_MAX_NUM_CARRIERS
} wl1_rfm_carrier_mode_enum_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for RFM WCDMA FTM state. Use to distinguish different modes of 
  operation in FTM for RF */
typedef enum
{
  WL1_RFM_WCDMA_FTM_NONE = 0,
  /*!< RF in non-FTM state i.e. online mode */

  WL1_RFM_WCDMA_FTM_CAL = 1,
  /*!< RF in FTM Cal state */

  WL1_RFM_WCDMA_FTM_RF = 2,
  /*!< RF in FTM RF state */

  WL1_RFM_WCDMA_FTM_NONSIG = 3,
  /*!< RF in FTM non-signaling state */

  WL1_RFM_WCDMA_FTM_INVALID
  /*!< RF in FTM state max */

} wl1_rfm_wcdma_ftm_state_type;

extern rfm_wcdma_ftm_state_type rfm_wcdma_get_ftm_state(void);

extern wl1_rf_tx_status_enum wl1_rf_tx_state;

#define WL1_RF_TUNE_CARR_0 0x1
#define WL1_RF_TUNE_CARR_1 0x2
#define WL1_RF_TUNE_CARR_2 0x4

#ifdef FEATURE_WCDMA_3C_HSDPA
#define WL1_RF_MAX_NUM_CARR 3
#elif defined (FEATURE_WCDMA_DC_HSDPA)
#define WL1_RF_MAX_NUM_CARR 2
#else
#define WL1_RF_MAX_NUM_CARR 1
#endif

extern uint16 l1_rf_channel_tune_list[4];

#define NUM_CARR_0 0
#define NUM_CARR_1 1
#ifdef FEATURE_WCDMA_DC_HSDPA
#define NUM_CARR_2 2
#ifdef FEATURE_WCDMA_3C_HSDPA
#define NUM_CARR_3 3
#endif
#endif

#define WL1_RF_HEADER_SB_NUM_SOURCE 1
#define WL1_RF_HEADER_DB_NUM_SOURCE 2

/* This flag is used to inform RF to skip sending AGC command to FW. It will be set to TRUE in case of early RF shutdown
   since RF and FW sleep can happen in parallel with the latest optimizations */
static boolean wl1_skip_sending_agc_disable = FALSE;

/* Create alias to be used in WL1 instead of RF type */
typedef rfcom_wcdma_band_type wl1_rf_card_band_enum_type;

typedef rfcom_tx_multi_carrier_cfg_type wl1_rfcom_tx_multi_carrier_cfg_type;

rfm_wcdma_rx_wakeup_params wl1_wakeup_params_copy;
 
rfm_wcdma_rx_sleep_params wl1_sleep_params_copy; 

rfm_wcdma_rx_tune_params wl1_tune_params_copy;


rfm_wcdma_rx_wakeup_params wl1_wakeup_params;

rfm_wcdma_rx_sleep_params wl1_sleep_params;

rfm_wcdma_rx_tune_params wl1_tune_params;

/* enums to hold the chain type . DUAL is currently used in rfm_wakeup from sleep for RxD on.*/

typedef enum
{  
  WL1_RFM_WAKEUP_PRIMARY,
  WL1_RFM_WAKEUP_SINGLE = WL1_RFM_WAKEUP_PRIMARY,
  WL1_RFM_WAKEUP_DIVERSITY,
  WL1_RFM_WAKEUP_DUAL  
}wl1_rfm_wakeup_chain_type;

/* enums to hold the RF operation to be performed on a chain(s). */
typedef enum
{
  WL1_RFM_WAKEUP_PREP,
  WL1_RFM_WAKEUP_EXEC,
  WL1_RFM_SLEEP_EXEC,
  WL1_RFM_PREP_TUNE,
  WL1_RFM_EXEC_TUNE
}wl1_rfm_wakeup_type;

typedef enum
{
  RF_AWAKE,
  RF_SLEEP_IN_PROG,
  RF_ASLEEP
}wl1_rf_sleep_status_enum_type;

extern wl1_rf_sleep_status_enum_type wl1_rf_state[2];

#define PRIMARY_CHAIN 0
#define SECONDARY_CHAIN 1
#define WL1_RF_AWAKE()          (wl1_rf_state[PRIMARY_CHAIN] == RF_AWAKE)
#define WL1_RF_DIV_AWAKE()      (wl1_rf_state[SECONDARY_CHAIN] == RF_AWAKE)

typedef struct
{
  /* carrier tune bmsk */
  uint8 tune_bmsk;
  /*freq array */
  uint16 uarfcn[WL1_RF_MAX_NUM_CARR];
  /* current tuned center freq */
  int32 curr_tuned_chan_rx_freq_kHz;
}wl1_rf_config_struct_type;


/* Struct for RF source parameters */
typedef struct
{
  /* Source Device id */
  uint32 device;
  /* Indicate if RxD is enabled */
  boolean div_en; 
  /* Source Div Device id if RxD is enabled */
  uint32 div_device;
  /* Serving W band */
  wl1_rf_card_band_enum_type serving_band;
  /* Serving W DL chan */
  uint16 serving_chan[NUM_CARR];
  /* Number of carriers: 0 - Single carrier 1- Dual Carrier 2- 3-Carrier*/
  wl1_rfm_carrier_mode_enum_type carrier_type;
} wl1_rf_source_params_struct_type;


/* =======================================================================
**                        Function Declarations
** ======================================================================= */
extern wl1_rf_config_struct_type* wl1_dl_get_rf_db(uint8 );

extern int32 rfm_exit_mode (rfcom_device_enum_type device,
                     rfcom_mode_enum_type to_rfmode);

/*===========================================================================
FUNCTION wl1_rfm_set_txfreezeaccum

DESCRIPTION
  This function freeze or unfreeze Tx AGC accumulation
  Input values: 1 freeze, 0 unfreeze

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

// cfitoz
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif 
void wl1_rfm_set_txfreezeaccum(
  /* Indicate freeze or not freeze, freeze if TRUE */
  uint8 val, 
  wl1_ul_carr_id_enum_type carr_idx
)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  /* Disable or enable TxFreeze for both carrier when we send in max number of carrier */
  val = (val) ? 1 : 0; /* make sure trsnalation matches what mDSP needs */
  rfm_set_txfreezeaccum( val, (rfcom_multi_carrier_id_type)carr_idx);
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_get_txagctablestore

DESCRIPTION
  This function returns the contribution to the transmit power that reflects 
  the accumulation of UL TPC commands

DEPENDENCIES
  None

RETURN VALUE
  Tx AGC accumulation by TPC command

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif 
int16 wl1_rfm_get_txagctablestore(wl1_ul_carr_id_enum_type carr_idx)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_get_txagctablestore((rfcom_multi_carrier_id_type)carr_idx);
#else
  return (0);
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_convert_tx_agc_to_dbm_unit

DESCRIPTION
  This function converts Tx AGC from linear to dBm units

DEPENDENCIES
  None

RETURN VALUE
  dBm value of Tx AGC

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_convert_tx_agc_to_dbm_unit(
  /* Tx AGC value in linear unit */
  int16 txagcval)
{

#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_convert_tx_agc_to_dbm_unit(txagcval);
#else
 return (-60);
#endif

}

/*===========================================================================
FUNCTION wl1_rfm_convert_tx_dbm_to_agc_unit

DESCRIPTION
  This function converts Tx AGC dBm value to linear AGC units

DEPENDENCIES
  None

RETURN VALUE
  Tx AGC linear value

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_convert_tx_dbm_to_agc_unit(
  /* Tx AGC value in dBm units */
  int16 dbmval)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_convert_tx_dbm_to_agc_unit(dbmval);
#else
  return (0);
#endif
}

#ifdef FEATURE_WCDMA_MTPL_THERMAL_BACKOFF
/*===========================================================================
FUNCTION wl1_rfm_reduce_mptl_db_for_therm
         
DESCRIPTION
  This function calls RF API to reduce MTPL value in dB

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_reduce_mptl_db_for_therm(uint8 backoff_db)
{
  WL1_MSG1(HIGH, "MTPL_BACKOFF: Reducing MTPL by %d dB", backoff_db);
  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
    rfm_reduce_mptl_db_for_therm(backoff_db);
  #endif
}

#endif /* FEATURE_WCDMA_MTPL_THERMAL_BACKOFF */
/*===========================================================================
FUNCTION wl1_rfm_turn_off_pa
         wl1_rfm_turn_on_pa

DESCRIPTION
  This function turns PA off or on

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_turn_off_pa(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_turn_off_pa();
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_turn_on_pa
         wl1_rfm_turn_on_pa

DESCRIPTION
  This function turns on/off PA

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_turn_on_pa(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT  
  rfm_turn_on_pa();
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_pa_is_on

DESCRIPTION
  This function returns PA on status

DEPENDENCIES
  None

RETURN VALUE
  PA on/off status

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint8 wl1_rfm_pa_is_on(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_pa_is_on();
#else
  return 1;
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_enable_pwr_ctrl

DESCRIPTION
  This function enables power control

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_enable_pwr_ctrl(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_enable_pwr_ctrl();
#endif
}

/*===========================================================================
FUNCTION wl1_rfm_get_ue_max_tx_power

DESCRIPTION
  This function return UE max power limit UE is capable of transmitting

DEPENDENCIES
  None

RETURN VALUE
  UE max power limit in dBm units

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_ue_max_tx_power(
  /* RF channel type */
  rfcom_txplim_channel_type chan_type)
{
  return rfm_get_ue_max_tx_power(chan_type);
}

/*===========================================================================
FUNCTION WL1_RFM_GET_UE_MAX_TX_POWER_IN_AGC

DESCRIPTION
  This function returns UE max power limit UE is capable of transmitting in 
  TX AGC units

DEPENDENCIES
  None

RETURN VALUE
  UE max power limit in TX AGC units

SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_ue_max_tx_power_in_agc(void)

{
  return rfm_get_mdsp_tx_power_limit();
}


/*===========================================================================
FUNCTION wl1_rfm_get_tx_rotator_angle

DESCRIPTION
  This function return UE tx rotator angle

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Pointer argument passed are updated
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_get_tx_rotator_angle(
  uint16 *tx_rot_ang_pa_00_p,  /* Pointer to tx rotator angle pa state 00 */
  uint16 *tx_rot_ang_pa_01_p,  /* Pointer to tx rotator angle pa state 01 */
  uint16 *tx_rot_ang_pa_10_p,  /* Pointer to tx rotator angle pa state 10 */
  uint16 *tx_rot_ang_pa_11_p)   /* Pointer to tx rotator angle pa state 11 */
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_get_tx_rotator_angle(tx_rot_ang_pa_00_p, tx_rot_ang_pa_01_p,
                           tx_rot_ang_pa_10_p, tx_rot_ang_pa_11_p);
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_get_max_tx_pwr_limit

DESCRIPTION
  The function updates the passed rf_max_tx_pwr_limit_type structure with 
  MTPL (Max Tx Power Limit) values for difference BCS case in Tx AGC counts. 
  L1 should call this function on every frame boundary and update 
  the L1-DSP table with MTPL values.  
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  Update MTPL structure passed as argument
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif

void wl1_rfm_get_max_tx_pwr_limit(
  /* MTPL struture pointer to get values */
  rf_max_tx_pwr_limit_type *rf_mtpl)
{
  rfm_get_max_tx_pwr_limit(rf_mtpl);
}

/*===========================================================================
FUNCTION  wl1_rfm_get_cgagc_settling_time

DESCRIPTION
  The function gets CGAGC settling time
  
DEPENDENCIES
  None
  
RETURN VALUE
  CGAGC settling time
  
SIDE EFFECTS
  Nome
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_cgagc_settling_time(void)
{
  return 0;
}

/*===========================================================================
FUNCTION  wl1_rfm_get_rf_warmup_time

DESCRIPTION
  The function gets RF warmup time
  
DEPENDENCIES
  None
  
RETURN VALUE
  RF warmup time
  
SIDE EFFECTS
  Nome
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint16 wl1_rfm_get_rf_warmup_time(void)
{
  return rfm_get_rf_warmup_time();
}

/*===========================================================================
FUNCTION  wl1_rfm_turn_on_rf_ldo

DESCRIPTION
  The function turns on/off RF LDOs. elay is hardcodde to 0 us.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_turn_on_rf_ldo(uint16 delay)
{
  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
    rfm_turn_on_rf_ldo(0);
  #else
    WL1_MSG0(ERROR, "CODE_NOT_PRESENT, wl1_rfm_turn_on_rf_ldo");
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_enable_trk_lo_adj_pdm

DESCRIPTION
  The function enables/disables trk lo PDM adjust
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_enable_trk_lo_adj_pdm(
  /* indicate enable or disble TRK LO adj PDM */
  boolean enable)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_enable_trk_lo_adj_pdm(enable);
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_get_wtow_rf_ctl_buf

DESCRIPTION
  The function get inter-freq CM RF buffer for given freq
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  Update buffer pointer passed with RF setup prameters
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_get_wtow_rf_ctl_buf(
  /* inter-freq ARFCN */
  uint16 arfcn,
  /* RF setup buffer pointer */
  void *buf_ptr
  )
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_get_wtow_rf_ctl_buf(arfcn, buf_ptr);
#endif
   return;
}

#ifdef FEATURE_WCDMA_DC_HSDPA
/*===========================================================================
FUNCTION  wl1_rfm_get_multi_carrier_wtow_rf_ctl_buf

DESCRIPTION
  The function gets inter-freq CM RF buffer for given freqs. Supports DC
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  Update buffer pointer passed with RF setup prameters
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_get_multi_carrier_wtow_rf_ctl_buf(
  /* inter-freq ARFCN */
  uint16 *arfcn,
  /* Carrier type */
  rfcom_multi_carrier_hspa_id_type carrier_type,
  /* RF setup buffer pointer */
  void *buf_ptr
  )
{
  rfm_get_multi_carrier_wtow_rf_ctl_buf(arfcn, carrier_type, buf_ptr);

   return;
}
#endif /* FEATURE_WCDMA_DC_HSDPA */

/*===========================================================================
FUNCTION  wl1_rfm_get_enc_btf_val

DESCRIPTION
  The function get BTF delay value
  
DEPENDENCIES
  None
  
RETURN VALUE
  BTF delay value
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint16 wl1_rfm_get_enc_btf_val(void)
{
  return rfm_get_enc_btf_val();
}

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif

/*===========================================================================
FUNCTION  wl1_populate_rfm_slp_wkup_params

DESCRIPTION
  This function populates the parameters of the RF wakeup/sleep/tune structures for dispatching to RF to perform the respective operations.
  Takes as arguments the RxLM buffer index, the device_id, chains to be operated upon - PRx or PRx/DRx, and nature of the operation - wakeup, tune or sleep.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void wl1_populate_rfm_slp_wkup_params(uint32 buf_idx, uint16 device_id, wl1_rfm_wakeup_chain_type chain, wl1_rfm_wakeup_type wakeup_type)
{
   uint8 device_id_local;   
   MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"buf_idx %d, dev id %d,chain %d,wakeup type %d",buf_idx,device_id,chain,wakeup_type);
   memset(&wl1_wakeup_params, 0x0, sizeof(rfm_wcdma_rx_wakeup_params));
   memset(&wl1_sleep_params, 0x0, sizeof(rfm_wcdma_rx_sleep_params));
   memset(&wl1_tune_params, 0x0, sizeof(rfm_wcdma_rx_tune_params));

   switch (wakeup_type) 
   {
     case WL1_RFM_WAKEUP_PREP :
     case WL1_RFM_WAKEUP_EXEC:     
     {
         switch (chain) 
         {
           case WL1_RFM_WAKEUP_DUAL:
              device_id_local = wl1_trm_get_primary_rf_device();              
              wl1_wakeup_params.api_params[device_id_local].is_valid = TRUE;
              wl1_wakeup_params.api_params[device_id_local].rxlm_handle = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN0_HOME);
              device_id_local = wl1_trm_get_secondary_rf_device();              
              wl1_wakeup_params.api_params[device_id_local].is_valid = TRUE;
              wl1_wakeup_params.api_params[device_id_local].rxlm_handle = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);
              break;     
           default:
              wl1_wakeup_params.api_params[device_id].is_valid = TRUE;
              wl1_wakeup_params.api_params[device_id].rxlm_handle = buf_idx;
              break;
         }         
         break;
     }
     case WL1_RFM_PREP_TUNE:
     case WL1_RFM_EXEC_TUNE:
     {
       switch (chain) 
       {
         case WL1_RFM_WAKEUP_DUAL:
                device_id_local = wl1_trm_get_primary_rf_device();                
                wl1_tune_params.api_params[device_id_local].is_valid = TRUE;
                wl1_tune_params.api_params[device_id_local].rxlm_handle = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN0_HOME);
                device_id_local = wl1_trm_get_secondary_rf_device();                
                wl1_tune_params.api_params[device_id_local].is_valid = TRUE;
                wl1_tune_params.api_params[device_id_local].rxlm_handle = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);
                break;
         default:
                wl1_tune_params.api_params[device_id].is_valid = TRUE;
                wl1_tune_params.api_params[device_id].rxlm_handle= buf_idx;
                break;
       }

       break;

     }
     case WL1_RFM_SLEEP_EXEC:
      {
         wl1_sleep_params.api_params[device_id].is_valid = TRUE;
         wl1_sleep_params.api_params[device_id].rxlm_handle= buf_idx;
         wl1_sleep_params.api_params[device_id].skip_agc_disable = wl1_skip_sending_agc_disable;
         break;
      }

     default:
       ERR_FATAL("Incorrect wakeup type!",0,0,0);
   }
} 

/*===========================================================================
FUNCTION  wl1_rfm_prep_wakeup_combined

DESCRIPTION
  This calls the new prep wakeup function from RF.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/


#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_prep_wakeup_combined(wl1_rfm_wakeup_chain_type chain)
{
   uint8 i;   
   switch(chain)
   {
    case WL1_RFM_WAKEUP_DUAL:
         wl1_populate_rfm_slp_wkup_params(0,0,WL1_RFM_WAKEUP_DUAL,WL1_RFM_WAKEUP_PREP);
         break;
    case WL1_RFM_WAKEUP_PRIMARY:
         wl1_populate_rfm_slp_wkup_params(wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN0_HOME), wl1_trm_get_primary_rf_device(), WL1_RFM_WAKEUP_PRIMARY, WL1_RFM_WAKEUP_PREP);
         break;
    default:
         break;
   }
   rfm_wcdma_rx_prep_wakeup(&wl1_wakeup_params, NULL, NULL); 
   WCDMA_MEMCPY(&wl1_wakeup_params_copy,sizeof(rfm_wcdma_rx_wakeup_params),&wl1_wakeup_params, sizeof(rfm_wcdma_rx_wakeup_params));
   memset(&wl1_wakeup_params, 0x0, sizeof(rfm_wcdma_rx_wakeup_params));
   for (i = 0;i < RFM_WCDMA_MAX_DEVICE;i++) 
   {
      WL1_MSG2(HIGH, "prep wakeup : after copy device validity %d, rxlm handle %d",
                     wl1_wakeup_params_copy.api_params[i].is_valid,
                     wl1_wakeup_params_copy.api_params[i].rxlm_handle);
   }
 

    
}

/*===========================================================================
FUNCTION  wl1_rfm_exec_wakeup_combined

DESCRIPTION
  This calls the new exec wakeup function from RF.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_exec_wakeup_combined(rfm_cb_handler_type cb_handler)
{        
  uint8 i;
  rfm_wcdma_rx_exec_wakeup(&wl1_wakeup_params, cb_handler, NULL);
  WCDMA_MEMCPY(&wl1_wakeup_params_copy,sizeof(rfm_wcdma_rx_wakeup_params),&wl1_wakeup_params, sizeof(rfm_wcdma_rx_wakeup_params));
  memset(&wl1_wakeup_params, 0x0, sizeof(rfm_wcdma_rx_wakeup_params));
  

  for (i = 0;i < RFM_WCDMA_MAX_DEVICE;i++) 
  {
    WL1_MSG2(HIGH, "exec wakeup: after copy  device validity %d, rxlm handle %d",
                   wl1_wakeup_params_copy.api_params[i].is_valid,
                   wl1_wakeup_params_copy.api_params[i].rxlm_handle);
  }
       
}

/*===========================================================================
FUNCTION  wl1_rfm_sleep_combined

DESCRIPTION
  This calls the new sleep function from RF.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/



#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_sleep_combined(rfm_cb_handler_type cb_handler)
{
  uint8 i;
  rfm_wcdma_rx_sleep(&wl1_sleep_params, cb_handler, NULL);
  WCDMA_MEMCPY(&wl1_sleep_params_copy,sizeof(rfm_wcdma_rx_sleep_params),&wl1_sleep_params, sizeof(rfm_wcdma_rx_sleep_params));
  memset(&wl1_sleep_params, 0x0, sizeof(rfm_wcdma_rx_sleep_params));

  for (i = 0;i < RFM_WCDMA_MAX_DEVICE;i++) 
  {
    WL1_MSG2(HIGH, "exec sleep : after copy device validity %d, rxlm handle %d",
                   wl1_sleep_params_copy.api_params[i].is_valid,
                   wl1_sleep_params_copy.api_params[i].rxlm_handle);
  }

}

/*===========================================================================
FUNCTION  wl1_rfm_prep_tune

DESCRIPTION
  This calls the new prep tune functionality. Takes channel number as a parameter apart from buffer index and chain type.
  Called only in idle mode, so the carrier index is always 1 in prep tune.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/



#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_prep_tune(uint16 channel,uint32 buf_idx, wl1_rfm_wakeup_chain_type chain )
{
  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    WL1_MSG1(ERROR, "wl1_rfm_tune_to_chan: FTM mode %d return", wl1_is_mode_FTM());
  }
  uint8 i,device_id_local;
  wl1_populate_rfm_slp_wkup_params(buf_idx,wl1_trm_get_primary_rf_device(),chain,WL1_RFM_PREP_TUNE); 
  device_id_local = wl1_trm_get_primary_rf_device();
  wl1_tune_params.api_params[device_id_local].channel[0] = channel;
  wl1_tune_params.api_params[device_id_local].carrier_mask = 1;
  rfm_wcdma_rx_prep_tune(&wl1_tune_params,NULL, NULL);
  WCDMA_MEMCPY(&wl1_tune_params_copy,sizeof(rfm_wcdma_rx_tune_params),&wl1_tune_params, sizeof(rfm_wcdma_rx_tune_params));
  memset(&wl1_tune_params, 0x0, sizeof(rfm_wcdma_rx_tune_params));

  for (i = 0;i < RFM_WCDMA_MAX_DEVICE;i++) 
  {
  
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"prep tune: after copy  device validity %d, rxlm handle %d, channel %d, carrier %d",wl1_tune_params_copy.api_params[i].is_valid,wl1_tune_params_copy.api_params[i].rxlm_handle,
        wl1_tune_params_copy.api_params[i].channel[0],wl1_tune_params_copy.api_params[i].carrier_mask);
  }
  
}

/*===========================================================================
FUNCTION  wl1_rfm_exec_tune

DESCRIPTION
  This calls the new exec tune functionality. Takes channel number as a parameter apart from buffer index and chain type.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/


#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_exec_tune(uint8 device_id, uint16 *freq_tune_list, uint8 tune_bmsk, uint32 buf_idx,wl1_rfm_wakeup_chain_type chain)
{
   uint8 i,device_id_local;

  wl1_populate_rfm_slp_wkup_params(buf_idx,device_id,chain,WL1_RFM_EXEC_TUNE); 

  device_id_local = device_id;

  for (i=0; i< WL1_RF_MAX_NUM_CARR; i++)
  {
    wl1_tune_params.api_params[device_id_local].channel[i] = freq_tune_list[i];
  }

  wl1_tune_params.api_params[device_id_local].carrier_mask = tune_bmsk;
  if (WL1_RFM_WAKEUP_DUAL == chain)
  {
    device_id_local = wl1_trm_get_secondary_rf_device(); 
    wl1_tune_params.api_params[device_id_local].carrier_mask = tune_bmsk;
    for (i=0; i< WL1_RF_MAX_NUM_CARR; i++)
    {
      wl1_tune_params.api_params[device_id_local].channel[i] = freq_tune_list[i];
    }
  }

  rfm_wcdma_rx_prep_tune(&wl1_tune_params,NULL, NULL);

  rfm_wcdma_rx_exec_tune(&wl1_tune_params,NULL, NULL);
  WCDMA_MEMCPY(&wl1_tune_params_copy,sizeof(rfm_wcdma_rx_tune_params),&wl1_tune_params, sizeof(rfm_wcdma_rx_tune_params));
  memset(&wl1_tune_params, 0x0, sizeof(rfm_wcdma_rx_tune_params));


  for (i = 0;i < RFM_WCDMA_MAX_DEVICE;i++) 
  {
  
    MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"exec tune: after copy  device validity %d, rxlm handle %d, channel %d, carrier %d",wl1_tune_params_copy.api_params[i].is_valid,wl1_tune_params_copy.api_params[i].rxlm_handle,
        wl1_tune_params_copy.api_params[i].channel[0],wl1_tune_params_copy.api_params[i].carrier_mask);
  }  

}

/*===========================================================================
FUNCTION  wl1_rfm_sleep_div_chain

DESCRIPTION
  Put diversity RF to sleep
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_sleep_div_chain(rfm_cb_handler_type cb_handler,
                                 uint32 buf_idx, 
                                 uint16 device_id
                            )
{
  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    WL1_MSG1(ERROR, "wl1_rfm_tune_to_chan: FTM mode %d return", wl1_is_mode_FTM());    
  }

    if (device_id == wl1_trm_get_secondary_rf_device())
    {
      wl1_rf_state[SECONDARY_CHAIN] = RF_SLEEP_IN_PROG;
    }

    WL1_MSG2(HIGH, "DBDC_DBG: wl1_rfm_sleep_div_chain, device_id:%d , buf_idx:%d",
                   device_id, buf_idx);
    #ifdef FEATURE_MULTI_RF_CHAIN_API
    wl1_populate_rfm_slp_wkup_params(buf_idx,device_id,WL1_RFM_WAKEUP_PRIMARY,WL1_RFM_SLEEP_EXEC);
    wl1_rfm_sleep_combined(NULL);
    #endif

    if (device_id == wl1_trm_get_secondary_rf_device())
    {
      wl1_rf_state[SECONDARY_CHAIN] = RF_ASLEEP;
    }

   return;
}
/*===========================================================================
FUNCTION  wl1_rfm_wakeup_div_chain

DESCRIPTION
  Wakeup diversity RF
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else
INLINE 
#endif
void wl1_rfm_wakeup_div_chain(
    rfm_cb_handler_type cb_handler     /* Call back handler */
    , uint32 buf_idx, uint16 device_id
)
{

  if (device_id == wl1_trm_get_secondary_rf_device())
  {
    wl1_rf_state[SECONDARY_CHAIN] = RF_AWAKE;
  }

  #ifdef FEATURE_MULTI_RF_CHAIN_API
  wl1_populate_rfm_slp_wkup_params(buf_idx,device_id,WL1_RFM_WAKEUP_DIVERSITY,WL1_RFM_WAKEUP_EXEC);
  wl1_rfm_exec_wakeup_combined(cb_handler);
  #endif    
  
  return;
}


/*===========================================================================
FUNCTION  wl1_rfm_wakeup_opt_status

DESCRIPTION
  Wakeup primary RF with/without optimization
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_wakeup_opt_status(boolean enable_opt)
{
  WL1_MSG1(HIGH, "FR2001: Updating the RF opt status(%d) from L1", enable_opt);
  rfm_set_wakeup_optimization(enable_opt);
}

/*===========================================================================
FUNCTION  wl1_rfm_wakeup

DESCRIPTION
  Wakeup primary RF
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_wakeup(
  rfm_cb_handler_type cb_handler,
  uint32 buf_idx,
  wl1_rfm_wakeup_chain_type chain
  )
{
#ifdef FEATURE_MULTI_RF_CHAIN_API
  uint32 div_buf_idx;
#endif
  wl1_rf_state[PRIMARY_CHAIN] = RF_AWAKE;
  
  #ifdef FEATURE_MULTI_RF_CHAIN_API

  wl1_populate_rfm_slp_wkup_params(buf_idx,wl1_trm_get_primary_rf_device(),chain,WL1_RFM_WAKEUP_EXEC);
  wl1_rfm_exec_wakeup_combined(cb_handler);
  if(chain == WL1_RFM_WAKEUP_DUAL)
  {
    wl1_rf_state[SECONDARY_CHAIN] = RF_AWAKE;
  }       

  if(rfm_get_primary_path() == RF_PATH_1)
  {
    wl1_rf_state[SECONDARY_CHAIN] = RF_AWAKE;

    div_buf_idx = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);

    wl1_populate_rfm_slp_wkup_params(div_buf_idx,wl1_trm_get_secondary_rf_device(),chain,WL1_RFM_WAKEUP_EXEC);

    wl1_rfm_exec_wakeup_combined(cb_handler);    
  }

  #else
  rfm_wakeup(RFCOM_WCDMA_MODE, NULL, NULL);
  #endif

  return;
}

/*===========================================================================
FUNCTION  wl1_rfm_prep_wakeup_div_chain

DESCRIPTION
  prepares RF scripts prior to RF wakeup
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
  #else
INLINE 
  #endif
void wl1_rfm_prep_wakeup_div_chain(uint32 buf_idx)
{
  rfm_wcdma_prep_wakeup_rx(wl1_trm_get_secondary_rf_device(),buf_idx, NULL, NULL);
}

/*===========================================================================
FUNCTION  wl1_rfm_sleep

DESCRIPTION
  Put primary RF to sleep
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_sleep(
  uint32 buf_idx,
  wrm_resource_enum_type resource
  )
{
  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    WL1_MSG1(ERROR, "wl1_rfm_tune_to_chan: FTM mode %d return", wl1_is_mode_FTM());    
  }

  #ifdef FEATURE_MULTI_RF_CHAIN_API
uint32 div_buf_idx;
  if(resource == WCDMA_TRM_CA )
  {    
    wl1_populate_rfm_slp_wkup_params(buf_idx,wl1_trm_get_primary_rf_device_for_ca(),WL1_RFM_WAKEUP_PRIMARY,WL1_RFM_SLEEP_EXEC);
    wl1_rfm_sleep_combined(NULL);      
  }
  else
  {
    if(rfm_get_primary_path() == RF_PATH_1)
    {
      wl1_rf_state[SECONDARY_CHAIN] = RF_SLEEP_IN_PROG;
      
      div_buf_idx = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);
  
      wl1_populate_rfm_slp_wkup_params(div_buf_idx,wl1_trm_get_secondary_rf_device(),WL1_RFM_WAKEUP_PRIMARY,WL1_RFM_SLEEP_EXEC);

      wl1_rfm_sleep_combined(NULL);
      
      wl1_rf_state[SECONDARY_CHAIN] = RF_ASLEEP;
    }

    wl1_rf_state[PRIMARY_CHAIN] = RF_SLEEP_IN_PROG; 

    wl1_populate_rfm_slp_wkup_params(buf_idx,wl1_trm_get_primary_rf_device(),WL1_RFM_WAKEUP_PRIMARY,WL1_RFM_SLEEP_EXEC);
    wl1_rfm_sleep_combined(NULL);  
    
    wl1_rf_state[PRIMARY_CHAIN] = RF_ASLEEP;
  }

  #else
  rfm_sleep();
  #endif

  return;
}


/*===========================================================================
FUNCTION  wl1_rfm_log_agc

DESCRIPTION
  Log AGC value
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_log_agc(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    rfm_log_agc(wl1_trm_get_primary_rf_device());
  #else
    rfm_log_agc();
  #endif
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_exit_mode

DESCRIPTION 
  Exit IMT mode
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_exit_mode(
  uint16 device_id 
  )
{
  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    WL1_MSG1(ERROR, "wl1_rfm_tune_to_chan: FTM mode %d return", wl1_is_mode_FTM());    
  }
  /*BOLT_TODO : Need to check what to call for Diversity case */
  if(rfm_get_primary_path() == RF_PATH_1)
  {
    rfm_exit_mode( (rfcom_device_enum_type)wl1_trm_get_secondary_rf_device(), (rfcom_mode_enum_type)RFM_PARKED_MODE);
    wl1_rf_state[SECONDARY_CHAIN] = RF_ASLEEP;
  }

  rfm_exit_mode((rfcom_device_enum_type) device_id, (rfcom_mode_enum_type)RFM_PARKED_MODE);
  wl1_rf_state[PRIMARY_CHAIN] = RF_ASLEEP;
}

/*===========================================================================
FUNCTION  wl1_rfm_enter_mode

DESCRIPTION 
  Enter IMT mode
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_enter_mode(
  uint32 buf_idx,
  uint16 device_id 
  )
{

  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    return ;
  }

  #ifdef FEATURE_MULTI_RF_CHAIN_API
  (void) rfm_enter_mode((rfcom_device_enum_type) device_id, RFCOM_WCDMA_MODE, NULL, NULL
                         #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE
                          ,buf_idx
                         #endif  
                        );
  if(rfm_get_primary_path() == RF_PATH_1)
  {
    uint32 div_buf_idx = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);

    (void) rfm_enter_mode(wl1_trm_get_secondary_rf_device(), RFCOM_WCDMA_MODE, NULL, NULL
                #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE 
                ,div_buf_idx
                #endif
                );
  }

  #else
  (void) rfm_enter_mode( RFCOM_WCDMA_MODE, NULL, NULL
                         #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE
                          ,buf_idx
                         #endif  
                        );
  #endif
  return;
}
/*===========================================================================
FUNCTION  wl1_rfm_tune_to_chan

DESCRIPTION
  Tune TX and RX to freq passed as parameter
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_rfm_tune_to_chan(uint32 buf_idx,
                            rfm_device_enum_type rf_device_id,
                            rfm_synth_tune_enum_type rfm_tune_type,
                            wl1_rfm_wakeup_chain_type chain
                            )
{

  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    WL1_MSG1(ERROR, "wl1_rfm_tune_to_chan: FTM mode %d return", wl1_is_mode_FTM());    
  }

  wl1_rf_config_struct_type* rf_db;

  uint8 band, carr_idx;

  if (rf_device_id == wl1_trm_get_primary_rf_device())
  {
    wl1_rf_state[PRIMARY_CHAIN] = RF_AWAKE;
  }

  if (rf_device_id == wl1_trm_get_primary_rf_device() || rf_device_id == wl1_trm_get_secondary_rf_device())
  {
    band = WL1_DL_BAND_0;
    rf_db = wl1_dl_get_rf_db(band);
  }
  else
  {
    band = WL1_DL_BAND_1;
    rf_db = wl1_dl_get_rf_db(band);
  }

#ifdef FEATURE_WCDMA_RUMI_SUPPORT
  if(wl1_rumi_dc_enable)
  {
    rf_db->uarfcn[1] = 10725;
    /*enable secondary carrier*/
    rf_db->tune_bmsk |= WL1_RF_TUNE_CARR_1;

  }
#endif

  for (carr_idx = 0; carr_idx < WL1_RF_MAX_NUM_CARR; carr_idx++)
  {
    l1_rf_channel_tune_list[carr_idx] = rf_db->uarfcn[carr_idx];
  }

  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"TUNE_TO_CHAN Tune type: %d Tune bmsk : %d rf device id %d prim_freq %d sec_freq %d ter_freq %d ", 
                                                                             rfm_tune_type,
                                                                             rf_db->tune_bmsk,
                                                                             rf_device_id,
                                                                             l1_rf_channel_tune_list[WL1_PRIMARY_CARRIER_IDX],
                                                                             l1_rf_channel_tune_list[WL1_SECONDARY_CARRIER_IDX],
                                                                             l1_rf_channel_tune_list[WL1_TERTIARY_CARRIER_IDX]
                                                                             );


  wl1_rfm_exec_tune(rf_device_id,
                    l1_rf_channel_tune_list,
                    rf_db->tune_bmsk,
                    buf_idx,
                    chain
                   );

  if((rfm_get_primary_path() == RF_PATH_1) && (rfm_tune_type == RFM_TUNE_TX_RX))
  {
    uint32 div_buf_idx = wl1_xlm_get_rxlm_buf(WL1_XLM_RX_CHAIN1_DIVERSITY);

    wl1_rfm_exec_tune(wl1_trm_get_secondary_rf_device(),
                      l1_rf_channel_tune_list,
                      rf_db->tune_bmsk,
                      div_buf_idx,
                      chain
                     );
  }

   return 0;
}

/*===========================================================================
FUNCTION  wl1_rfm_enter_mode_div_chain

DESCRIPTION 
  Enter IMT mode on diversity branch
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_enter_mode_div_chain(
    rfm_cb_handler_type cb_handler,     /* Call back handler */
    uint32 buf_idx, uint16 device_id, uint16 freq,boolean call_tune_api
    )
{

  if (device_id == wl1_trm_get_secondary_rf_device())
  {
    wl1_rf_state[SECONDARY_CHAIN] = RF_AWAKE;
  }
  MSG_HIGH("DBDC_DBG: Div chain device id:%d, buf_idx: %d",device_id,buf_idx,0);

  if(wl1_is_mode_FTM())
  {
    /*Do not call rfm_enter_mode while in FTM mode*/
    return ;
  }

    #ifdef FEATURE_MULTI_RF_CHAIN_API
    rfm_enter_mode((rfcom_device_enum_type) device_id, RFCOM_WCDMA_MODE, NULL, cb_handler
                   #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE
                   ,buf_idx
                   #endif  
                   );
    if(call_tune_api)
    {
      wl1_rfm_tune_to_chan(buf_idx,
                         (rfm_device_enum_type)device_id,
                         RFM_TUNE_RX,
			 WL1_RFM_WAKEUP_PRIMARY
                          );
       MSG_HIGH("DBDC_DBG: Div chain Tuned : device id:%d, buf_idx: %d, freq : %d",device_id,buf_idx,freq);
    }
    #endif
   return;
}

/*===========================================================================
FUNCTION  wl1_rfm_get_rx_agc_val

DESCRIPTION
  Get rx AGV value
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_rx_agc_val(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_rx_agc_val(wl1_trm_get_primary_rf_device());
  #else
    return rfm_get_rx_agc_val();
  #endif
#else
  return 0;
#endif
}
/*===========================================================================
FUNCTION  wl1_rfm_get_tx_agc_val

DESCRIPTION
  Get Tx AGV value in dB units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_tx_agc_val(void)
{
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_tx_agc_val(wl1_trm_get_primary_rf_device());
  #else
    return rfm_get_tx_agc_val();
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_get_tx_agc_val_in_mdsp_units

DESCRIPTION
  Get the HDET corrected Tx AGV value in mdsp units 
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_tx_agc_val_in_mdsp_units(void)

{
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_mdsp_tx_agc(); 
  #else
    return rfagc_get_tx_agc_value(); 
  #endif
}

/*===========================================================================
FUNCTION  WL1_RFM_GET_CURRENT_PA_STATE

DESCRIPTION
  Get the PA state from RFA SW 
  
DEPENDENCIES
  None
  
RETURN VALUE
  uint8
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint8 wl1_rfm_get_current_pa_state(void)

{
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_current_pa_state(); 
  #else
    WL1_MSG0(ERROR, "Need RFM support for PA state query");
    return 0; 
  #endif
}


/*===========================================================================
FUNCTION  wl1_rfagc_get_rx_agc_pdm

DESCRIPTION
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint16 wl1_rfagc_get_rx_agc_pdm(void)
{
  return 0;
}

/*===========================================================================
FUNCTION  wl1_rfm_get_receive_agc_val_db12

DESCRIPTION
  The function get RF AGC sample logged for serving cell in db/12 units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_receive_agc_val_db10(rfcom_device_enum_type rfcom_transceiver,uint8 carr_idx)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return  rfm_wcdma_get_rxagc(rfcom_transceiver, RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DB10,(rfcom_multi_carrier_id_type)carr_idx);
#else
  return (-60);
#endif

  /* Old_Code 
  #ifdef FEATURE_MULTI_RF_CHAIN_API
  return rfm_get_receive_agc_val(rfcom_transceiver, RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DB10);
  #else
  return rfm_get_receive_agc_val(RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DB10);
  #endif
  */
}

/*===========================================================================
FUNCTION  wl1_rfm_force_read_agc_val_db12

DESCRIPTION
  The function get RF AGC sample logged for serving cell in db/12 units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_force_read_agc_val_db10(rfcom_device_enum_type rfcom_transceiver)
{
  return rfm_wcdma_get_rxagc(rfcom_transceiver, RFCOM_SERVINGCELL_FORCE_READ_RXAGC,RFCOM_RXAGC_IN_DB10,(rfcom_multi_carrier_id_type)CARR_0);
}


/*===========================================================================
FUNCTION  WL1_RFM_GET_RECEIVE_AGC_VAL_FOR_ROUNDED_RSCP

DESCRIPTION
  The function get RF AGC sample logged for serving cell in db/12 units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_receive_agc_val_for_rounded_rscp(rfcom_device_enum_type rfcom_transceiver, uint8 carr_idx)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_wcdma_get_rxagc(rfcom_transceiver, RFCOM_SERVINGCELL_RXAGC_RAW,RFCOM_RXAGC_IN_DB10,(rfcom_multi_carrier_id_type)carr_idx);
#else
  return(-60*12);
#endif
  /* Old_code 
  #ifdef FEATURE_MULTI_RF_CHAIN_API
  return rfm_get_receive_agc_val(rfcom_transceiver, RFCOM_SERVINGCELL_RXAGC_RAW,RFCOM_RXAGC_IN_DB10);
  #else
  return rfm_get_receive_agc_val(RFCOM_SERVINGCELL_RXAGC_RAW,RFCOM_RXAGC_IN_DB10);
  #endif
  */
}

/*===========================================================================
FUNCTION  WL1_RFM_FORCE_READ_AGC_VAL_FOR_ROUNDED_RSCP

DESCRIPTION
  The function get RF AGC sample logged for serving cell in db/12 units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_force_read_agc_val_for_rounded_rscp(rfcom_device_enum_type rfcom_transceiver)
{
  return rfm_wcdma_get_rxagc(rfcom_transceiver, RFCOM_SERVINGCELL_FORCE_READ_RXAGC_RAW,RFCOM_RXAGC_IN_DB10,(rfcom_multi_carrier_id_type)CARR_0);
}

/*===========================================================================
FUNCTION  wl1_rfm_get_receive_agc_val_dbm

DESCRIPTION
  The function get RF AGC sample logged in db units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_receive_agc_val_dbm(uint8 carr_idx)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_wcdma_get_rxagc(wl1_trm_get_primary_rf_device(), RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM,(rfcom_multi_carrier_id_type)carr_idx);
#else
  return (-60);
#endif
  /*
  #ifdef FEATURE_MULTI_RF_CHAIN_API
  return rfm_get_receive_agc_val(wl1_trm_get_primary_rf_device(), RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM);
  #else
  return rfm_get_receive_agc_val(RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM);
  #endif
  */
}

/*===========================================================================
FUNCTION  wl1_rfm_force_read_agc_val_dbm

DESCRIPTION
  The function get RF AGC sample logged in db units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_force_read_agc_val_dbm(void)
{
  return rfm_wcdma_get_rxagc(wl1_trm_get_primary_rf_device(), RFCOM_SERVINGCELL_FORCE_READ_RXAGC,RFCOM_RXAGC_IN_DBM,(rfcom_multi_carrier_id_type)CARR_0);
}

/*===========================================================================
FUNCTION  wl1_rfm_get_receive_div_agc_val_dbm

DESCRIPTION
  The function gets RF AGC sample logged in db units for diversity chain.
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_receive_div_agc_val_dbm(uint8 carr_idx)
{
  return rfm_wcdma_get_rxagc(wl1_trm_get_secondary_rf_device(), RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM,(rfcom_multi_carrier_id_type)carr_idx);

  /* Old_Code 
  #ifdef FEATURE_MULTI_RF_CHAIN_API
  return rfm_get_receive_agc_val(wl1_trm_get_secondary_rf_device(), RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM);
  #else
  return rfm_get_receive_agc_val(RFCOM_SERVINGCELL_RXAGC,RFCOM_RXAGC_IN_DBM);
  #endif
  */
}

/*===========================================================================
FUNCTION  wl1_rfm_force_read_div_agc_val_dbm

DESCRIPTION
  The function gets RF AGC sample logged in db units for diversity chain.
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_force_read_div_agc_val_dbm(void)
{
  return rfm_wcdma_get_rxagc(wl1_trm_get_secondary_rf_device(), RFCOM_SERVINGCELL_FORCE_READ_RXAGC,RFCOM_RXAGC_IN_DBM,(rfcom_multi_carrier_id_type)CARR_0);
}


/*===========================================================================
FUNCTION  wl1_rfm_get_receive_agc_val_for_rscp

DESCRIPTION
  The function get RF AGC sample logged for rscp, serving cell
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_receive_agc_val_for_rscp(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_receive_agc_val_for_rscp(wl1_trm_get_primary_rf_device(), RFCOM_SERVINGCELL_RXAGC_RAW);
  #else
  return rfm_get_receive_agc_val_for_rscp(RFCOM_SERVINGCELL_RXAGC);
  #endif
#else
  return(-60);
#endif

}

/*===========================================================================
FUNCTION  wl1_rfm_force_read_agc_val_for_rscp

DESCRIPTION
  The function get RF AGC sample logged for rscp, serving cell
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_force_read_agc_val_for_rscp(void)
{
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_receive_agc_val_for_rscp(wl1_trm_get_primary_rf_device(), RFCOM_SERVINGCELL_FORCE_READ_RXAGC_RAW);
  #else
    return rfm_get_receive_agc_val_for_rscp(RFCOM_SERVINGCELL_FORCE_READ_RXAGC);
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_get_receive_agc_val_for_rscp_ingap

DESCRIPTION
  The function get RF AGC sample logged for rscp measured in gap.
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else
INLINE 
#endif
int16 wl1_rfm_get_receive_agc_val_for_rscp_ingap(void)
{
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    return rfm_get_receive_agc_val_for_rscp(wl1_trm_get_primary_rf_device(), RFCOM_INGAP_RXAGC_RAW);
  #else
  return rfm_get_receive_agc_val_for_rscp(RFCOM_INGAP_RXAGC);
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_log_single_agc_data

DESCRIPTION
  The function get RF AGC sample logged.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_log_single_agc_data(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  #ifdef FEATURE_MULTI_RF_CHAIN_API
    rfm_log_single_agc_data(wl1_trm_get_primary_rf_device());
  #else
    rfm_log_single_agc_data();
  #endif
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_set_tx_general_ctl

DESCRIPTION
  The function sets TX_GEN_CONTFOL register in H/W
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_set_tx_general_ctl(rfcom_txplim_channel_type chan_type)
{
  rfm_set_tx_general_ctl(chan_type);
}

/*===========================================================================
FUNCTION  wl1_rfm_get_max_tx_power_nv_item

DESCRIPTION
  The function get maximum TX power indicated by NV item depending on the
  channel type
  
DEPENDENCIES
  None
  
RETURN VALUE
  The max TX power value.
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_max_tx_power_nv_item(
  /* UL channel type */
  rfcom_txplim_channel_type chan_type)
{

  return rfm_get_max_tx_power_nv_item(chan_type);

}

/*===========================================================================
FUNCTION  wl1_rfm_get_max_tx_power_in_rf_sleep

DESCRIPTION
  The function get maximum TX power indicated by NV item depending on the
  channel type if rf is asleep
  
DEPENDENCIES
  None
  
RETURN VALUE
  The max TX power value.
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_max_tx_power_in_rf_sleep(
                    rfm_device_enum_type device,
                    rfcom_txplim_channel_type chan_type,
                    rfcom_wcdma_band_type band)
{

  return rfm_wcdma_get_tx_max_power_nv(device,chan_type,band);

}


/*===========================================================================
FUNCTION  wl1_rfm_get_curr_band

DESCRIPTION
  The function returns the current active WCDMA carrier band.  Possibilities
  are:

  SYS_BAND_WCDMA_I_IMT_2000  - WCDMA EUROPE JAPAN & CHINA IMT 2100 band
  SYS_BAND_WCDMA_II_PCS_1900 - WCDMA US PCS 1900 band
  SYS_BAND_WCDMA_III_1700    - WCDMA EUROPE&CHINA DCS 1800 band
  SYS_BAND_WCDMA_IV_1700     - WCDMA US 1700 band
  SYS_BAND_WCDMA_V_850       - WCDMA US 850 band
  SYS_BAND_WCDMA_VI_800      - WCDMA JAPAN 800 band
  SYS_BAND_WCDMA_VII_2600    - WCDMA EUROPE 2600 band
  SYS_BAND_WCDMA_VIII_900    - WCDMA EUROPE & CHINA 900 band
  SYS_BAND_WCDMA_IX_1700     - WCDMA JAPAN 1700 band
  
DEPENDENCIES
  None
  
RETURN VALUE
  sys_band_class_e_type
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
sys_band_class_e_type wl1_rfm_get_curr_band(void)
{
  /* This cast looks a little dangerous, but rfi_band_type returned by
  ** rfm_get_curr_band() is almost identical to sys_band_class_e_type.  The
  ** RF enums are set equal to the SYS enums. */
  return (sys_band_class_e_type)rfm_get_curr_band();
}

/*===========================================================================
FUNCTION  wl1_rfm_convert_sys_band_to_rf_card_band

DESCRIPTION
  The function converts a sys_band_class_e_type into wl1_rf_card_band_enum_type.

DEPENDENCIES
  None
  
RETURN VALUE
  wl1_rf_card_band_enum_type
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
wl1_rf_card_band_enum_type wl1_rfm_convert_sys_band_to_rf_card_band(sys_band_class_e_type sys_band)
{
  return (wl1_rf_card_band_enum_type)rfm_convert_sys_band_to_rf_card_band(sys_band);
}

/*===========================================================================
FUNCTION  WL1_RFM_CONVERT_CHAN_TO_BAND

DESCRIPTION
  The function converts a WCDMA channel into rfcom_wcdma_band_type.

DEPENDENCIES
  None
  
RETURN VALUE
  rfcom_wcdma_band_type
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
rfcom_wcdma_band_type wl1_rfm_convert_chan_to_band(uint16 freq)
{
  return rfm_convert_chan_to_band(freq);
}


/*===========================================================================
FUNCTION  wl1_rfm_get_DM_required_for_HDET

DESCRIPTION
  The function queries RF to see if the Data Mover clock is required for HDET
  operation.
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if DM is required for HDET operation, FALSE otherwise
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_rfm_get_DM_required_for_HDET( void )
{
#ifdef FEATURE_WCDMA_RUMI_SUPPORT
  WL1_MSG0(ERROR, "CODE NOT PRESENT");
#else

 return rfm_get_rf_capability( (rfcom_rf_cap_type)RF_CAP_DM_FOR_HDET );

#endif
 return FALSE;
}

/*===========================================================================
FUNCTION  wl1_rfm_set_tx_pwr_limit

DESCRIPTION
  The function sets RF TX power limit as RACH for IDLE states
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_set_tx_pwr_limit(
  int16 tx_limit_dbm,                    /* Tx power limit in dBm */
  rfcom_txplim_channel_type chan_type
  )
{
  rfm_set_tx_pwr_limit(
                       #ifdef FEATURE_MULTI_RX_CHAIN
                       wl1_trm_get_primary_rf_device(),
                       #endif
                       tx_limit_dbm,
                       chan_type
                      );

   return;
}

/*===========================================================================

FUNCTION  wl1_rfm_wcdma_wakeup_tx

DESCRIPTION
  API used for WCDMA Wakeup.

DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_wcdma_wakeup_tx(void)
{
  rfm_wcdma_wakeup_tx();
}

/*===========================================================================

FUNCTION  wl1_rfm_init_wcdma_tx

DESCRIPTION
  Initialization functionality related to W Tx is called here.

DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_rfm_init_wcdma_tx(uint32 multi_carr_idx, wl1_rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfgd)
{
  boolean ret_val = TRUE;

  ret_val = rfm_init_wcdma_tx(
                           wl1_xlm_get_txlm_buf(WL1_XLM_TXLM_CHAIN_0),
                           multi_carr_idx,
                           dc_hsupa_cfgd
                          );
  return ret_val;
}

/*===========================================================================

FUNCTION  wl1_rfm_prep_init_wcdma_tx

DESCRIPTION
  Initialization functionality related to W Tx is called here.
  Specific to DC HSUPA 
DEPENDENCIES
  None
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_prep_init_wcdma_tx(uint32 multi_carr_idx)
{
   int16 ret_val = 0;
   
   #ifdef FEATURE_WCDMA_BOLT_2_0
   #else
   ret_val = (int16) rfm_wcdma_prep_init_tx(
                     RFM_DEVICE_0,
                     wl1_xlm_get_txlm_buf(WL1_XLM_TXLM_CHAIN_0),
                     multi_carr_idx,
                     NULL,
                     NULL
                     );
   #endif
   return ret_val;
}

/*===========================================================================
FUNCTION  wl1_rfm_set_dsr_mode

DESCRIPTION
  Set DSR mode to RF
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_rfm_set_dsr_mode(rfcom_lpm_config_enum_type lpm_mode, wl1_xlm_rx_client_enum_type rxlm_c0, 
                             wl1_xlm_rx_client_enum_type rxlm_c1)
{
  return (rfm_set_low_power_mode(lpm_mode,rxlm_c0, rxlm_c1));
}

/*===========================================================================

FUNCTION  wl1_rfm_enable_tx

DESCRIPTION
  Enables transmitter for mode of operation RF driver is currently in.
  The index into txlm buffer needs to passed as part of NikeL

DEPENDENCIES
  None
RETURN VALUE
  Returns total delay in microseconds this function will wait for RF
  hardware to settle.

SIDE EFFECTS
  None

===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_rfm_enable_tx(void)
{
    return rfm_enable_tx(
                         #ifdef FEATURE_MULTI_RX_CHAIN
                         wl1_trm_get_primary_rf_device(),
                         #endif
                         NULL,
                         NULL
                         #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE
                         ,0  /* Not used by RF. Required due to GSM dependency */
                         #endif
                        );
}

/*===========================================================================

FUNCTION  wl1_rfm_do_tx_pwr_limit

DESCRIPTION
  Adjusts the Tx power.

DEPENDENCIES
  rfm_init() must have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__ 
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_do_tx_pwr_limit(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  return rfm_do_tx_pwr_limit(
                             #ifdef FEATURE_MULTI_RX_CHAIN
                             wl1_trm_get_primary_rf_device()
                             #endif
                            );
#else
  return 0;
#endif
}

/*===========================================================================

FUNCTION  wl1_rfm_disable_tx

DESCRIPTION
  Disables transmitter for mode of operation RF driver is currently
  operating in.

DEPENDENCIES
  rfm_enable_tx() should have already been called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_disable_tx(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
    rfm_disable_tx(
                   #ifdef FEATURE_MULTI_RX_CHAIN
                   wl1_trm_get_primary_rf_device()
                   #endif
                   #ifdef FEATURE_RF_COMMON_LM_RFM_INTERFACE
                   ,0  /* Not used by RF. Required due to GSM dependency */
                   #endif
                  );
#endif
}

/*===========================================================================

FUNCTION  wl1_rfm_decfg_tx

DESCRIPTION
  Txlm decfg for Chain 1 since RF maintains Txlm buffer for secondary Tx path.
  
DEPENDENCIES
  This function should be called before sending mdsp_sync_send_txlm_cfg_cmd (disable)
  to FW.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_decfg_tx(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  /* Txlm decfg for Chain 1 since RF maintains Txlm buffer for Chain 1 */
  (void)rfm_wcdma_decfg_tx();
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_get_rf_capability_for_lna_switch() 

DESCRIPTION
  This function returns whether LNA switchpoint updates are 
  required by the current RF target.
    
DEPENDENCIES
  rf_init is called.
  
RETURN VALUE
  TRUE if feature is needed or enabled. FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_rfm_get_rf_capability_for_lna_switch(void)
{
 #if defined FEATURE_RFM_INTERFACE
  return rfm_get_rf_capability(RF_CAP_TECH_DEPENDENT_LNA_SWITCH);
 #else
  return FALSE;
 #endif
}


static void 
tech_write_dependent_nv_items_to_mdsp
(rfcom_tech_type tech,
 rfcom_wcdma_band_type band
)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT 
  #ifndef FEATURE_WCDMA_DB_DC_HSDPA
    rfm_write_tech_dependent_nv_items_to_mdsp(tech);
  #else
    rfm_write_tech_dependent_nv_items_to_mdsp(band, tech);
  #endif
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_write_hs_dependent_nv_items_to_mdsp() 

DESCRIPTION
  This function writes HS dependent lna nv items to mdsp

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_write_hs_dependent_nv_items_to_mdsp(rfcom_wcdma_band_type band)
{
 #if defined FEATURE_RFM_INTERFACE
  tech_write_dependent_nv_items_to_mdsp(RFCOM_HS_MODE, band);
 #else
  return;
 #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_write_r99_dependent_nv_items_to_mdsp() 

DESCRIPTION
  This function writes R99 dependent lna nv items to mdsp

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else
INLINE 
#endif
void wl1_rfm_write_r99_dependent_nv_items_to_mdsp(rfcom_wcdma_band_type band)
{
 #if defined FEATURE_RFM_INTERFACE
  tech_write_dependent_nv_items_to_mdsp(RFCOM_REL99_MODE, band);
 #else
  return;
 #endif

}

/*===========================================================================
FUNCTION  wl1_rfm_get_tx_pwr_limit

DESCRIPTION
  Get Tx AGV value in dB units
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/

#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_tx_pwr_limit(void)
{
    return rfm_get_tx_pwr_limit();
}


/*===========================================================================
FUNCTION  WL1_RFM_GET_HDET_APPLY_THRESHOLD_AGC

DESCRIPTION
  The function gets the threshold in TX AGC unit to apply HDET correction to
  Tx power
  
DEPENDENCIES
  None
  
RETURN VALUE
  the threshold in TX AGC unit
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int16 wl1_rfm_get_hdet_apply_threshold_agc(void)
{
  return rfm_get_hdet_apply_threshold_agc(); 
}

/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_ENTER

DESCRIPTION
  The function sends a meas_common_enter before a W2W gap search
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_enter(
   /* Number of source items */
   uint8 num_source_item,
   /* Source params */
   wl1_rf_source_params_struct_type* rf_source_params,
  /* Neighbor frequency */
  uint16 nbr_freq, 
  /* RXLM home handle */
  uint16 home_rxlm,
  /* RXLM neighbor handle */
  uint16 nbr_rxlm)
{
  rfm_meas_enter_param_type rf_meas_enter;
  rfm_meas_wcdma_enter_param_type source_param, target_param;
  rfm_meas_result_type rf_result;
  uint8 src_index, channel_index = 0, carrier_index;

  /* Populate the header params for source */
  rf_meas_enter.header.source_tech = RFCOM_WCDMA_MODE;
  rf_meas_enter.header.num_source_item = num_source_item;

  /* Populate the source header params */
  for(src_index = 0; src_index < MIN(num_source_item, WL1_RF_HEADER_DB_NUM_SOURCE); src_index++)
  {
    /* reset the carrier index for num source items 2*/
    carrier_index = 0;

    rf_meas_enter.header.source_param[src_index].device     = (rfm_device_enum_type)rf_source_params[src_index].device;
    rf_meas_enter.header.source_param[src_index].band       = rf_source_params[src_index].serving_band;
    rf_meas_enter.header.source_param[src_index].div_en     = rf_source_params[src_index].div_en;
    rf_meas_enter.header.source_param[src_index].div_device = (rfm_device_enum_type)rf_source_params[src_index].div_device;

    /* Populate the channels in source params */
    while((channel_index < WL1_RFM_MAX_NUM_CARRIERS) && (carrier_index <= rf_source_params[src_index].carrier_type))
    {
      source_param.channel[channel_index++] = rf_source_params[src_index].serving_chan[carrier_index++];
    }
  }

  /* Populate the header params for target */
  rf_meas_enter.header.target_tech = RFCOM_WCDMA_MODE;
  rf_meas_enter.header.target_param.device = wl1_trm_get_rf_device_for_target(nbr_freq);
  rf_meas_enter.header.target_param.band = wl1_rfm_convert_chan_to_band(nbr_freq);
  rf_meas_enter.header.target_param.div_en = FALSE;

  /* Populate the source enter params for WCDMA */
  source_param.device = wl1_trm_get_primary_rf_device();
  source_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;
  source_param.rxlm_buf_index = home_rxlm;

  /* Populate the target enter params */
  target_param.device = wl1_trm_get_rf_device_for_target(nbr_freq);
  target_param.channel[0] = nbr_freq;
  target_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;
  target_param.rxlm_buf_index = nbr_rxlm;

  rf_meas_enter.source_param = &source_param;
  rf_meas_enter.target_param = &target_param;

  rf_result = rfm_meas_common_script_enter(&rf_meas_enter, NULL);
  if (rf_result != RFM_MEAS_COMMON_SUCCESS)
  {
    ERR_FATAL("Failed rfm_meas_common_enter",0,0,0);
  }
}

/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_EXIT

DESCRIPTION
  The function sends a meas_common_exit after a W2W gap search
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_exit(
   /* Number of source items */
   uint8 num_source_item,
   /* Source params */
   wl1_rf_source_params_struct_type* rf_source_params,
   /* Neighbor frequency */
   uint16 nbr_freq)
{
  rfm_meas_exit_param_type rf_meas_exit;
  rfm_meas_wcdma_exit_param_type source_param, target_param;
  rfm_meas_result_type rf_result;

  uint8 src_index, channel_index = 0, carrier_index;

  rf_meas_exit.header.source_tech = RFCOM_WCDMA_MODE;
  rf_meas_exit.header.num_source_item = num_source_item;

  /* Populate the source header params */
  for(src_index = 0; src_index < MIN(num_source_item, WL1_RF_HEADER_DB_NUM_SOURCE); src_index++)
  {
    /* reset the carrier index for num source items 2*/
    carrier_index = 0;

    rf_meas_exit.header.source_param[src_index].device     = (rfm_device_enum_type)rf_source_params[src_index].device;
    rf_meas_exit.header.source_param[src_index].band       = rf_source_params[src_index].serving_band;
    rf_meas_exit.header.source_param[src_index].div_en     = rf_source_params[src_index].div_en;
    rf_meas_exit.header.source_param[src_index].div_device = (rfm_device_enum_type)rf_source_params[src_index].div_device;

    /* Populate the channels in source params */
    while((channel_index < WL1_RFM_MAX_NUM_CARRIERS) && (carrier_index <= rf_source_params[src_index].carrier_type))
    {
      source_param.channel[channel_index++] = rf_source_params[src_index].serving_chan[carrier_index++];
    }
  }

  /* Populate the header params for target */
  rf_meas_exit.header.target_tech = RFCOM_WCDMA_MODE;
  rf_meas_exit.header.target_param.device = wl1_trm_get_rf_device_for_target(nbr_freq);
  rf_meas_exit.header.target_param.band = wl1_rfm_convert_chan_to_band(nbr_freq);
  rf_meas_exit.header.target_param.div_en = FALSE;

  /* Populate the source exit params for WCDMA */
  source_param.device = wl1_trm_get_primary_rf_device();
  source_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;

  /* Populate the target exit params */
  target_param.device = wl1_trm_get_rf_device_for_target(nbr_freq);
  target_param.channel[0] = nbr_freq;
  target_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;;

  rf_meas_exit.source_param = &source_param;
  rf_meas_exit.target_param = &target_param;
  rf_meas_exit.is_measurement_gap_aborted = FALSE;

  rf_result = rfm_meas_common_script_exit(&rf_meas_exit, NULL);
  if (rf_result != RFM_MEAS_COMMON_SUCCESS)
  {
    ERR_FATAL("Failed rfm_meas_common_exit",0,0,0);
  }
}

#if defined(FEATURE_WCDMA_TO_LTE) && (defined(FEATURE_WCDMA_CM_LTE_SEARCH) || defined(FEATURE_W2L_IDLE_FW_RF_TUNE))
/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_EXIT_W2L

DESCRIPTION
  The function sends a meas_common_exit after a W2L gap search
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_exit_w2l(
   /* Number of source items */
   uint8 num_source_item,
   /* Source params */
   wl1_rf_source_params_struct_type* rf_source_params)
{
  rfm_meas_exit_param_type rf_meas_exit;
  rfm_meas_lte_exit_param_type target_param;
  rfm_meas_wcdma_exit_param_type source_param;
  rfm_meas_result_type rf_result;

  uint8 src_index, channel_index = 0, carrier_index;

  rf_meas_exit.header.source_tech = RFCOM_WCDMA_MODE;
  rf_meas_exit.header.target_tech = RFCOM_LTE_MODE;

  rf_meas_exit.header.num_source_item = num_source_item;

  /* Populate the source header params */
  for(src_index = 0; src_index < MIN(num_source_item, WL1_RF_HEADER_DB_NUM_SOURCE); src_index++)
  {
    /* reset the carrier index for num source items 2*/
    carrier_index = 0;

    rf_meas_exit.header.source_param[src_index].device     = (rfm_device_enum_type)rf_source_params[src_index].device;
    rf_meas_exit.header.source_param[src_index].band       = rf_source_params[src_index].serving_band;
    rf_meas_exit.header.source_param[src_index].div_en     = rf_source_params[src_index].div_en;
    rf_meas_exit.header.source_param[src_index].div_device = (rfm_device_enum_type)rf_source_params[src_index].div_device;

    /* Populate the channels in source params */
    while((channel_index < WL1_RFM_MAX_NUM_CARRIERS) && (carrier_index <= rf_source_params[src_index].carrier_type))
  {
      source_param.channel[channel_index++] = rf_source_params[src_index].serving_chan[carrier_index++];
  }
  }

  /* Populate the source enter params for WCDMA */
  source_param.device = wl1_trm_get_primary_rf_device();
  source_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;

  /* Populate the target param device */
  target_param.device = wl1_trm_get_primary_rf_device();

  rf_meas_exit.source_param = &source_param;
  rf_meas_exit.target_param = &target_param;
  rf_meas_exit.is_measurement_gap_aborted = FALSE;

  rf_result = rfm_meas_common_script_exit(&rf_meas_exit, NULL);
  if (rf_result != RFM_MEAS_COMMON_SUCCESS)
  {
    ERR_FATAL("Failed rfm_meas_common_exit",0,0,0);
  }
}

/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_ENTER_W2L

DESCRIPTION
  The function sends a meas_common_enter before a W2L gap search
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_enter_w2l(
   /* Number of source items */
   uint8 num_source_item,
   /* Source params */
   wl1_rf_source_params_struct_type* rf_source_params)
{
  rfm_meas_enter_param_type rf_meas_enter;
  rfm_meas_lte_enter_param_type target_param;
  rfm_meas_wcdma_enter_param_type source_param;
  rfm_meas_result_type rf_result;

  uint8 src_index, channel_index = 0, carrier_index;

  rf_meas_enter.header.source_tech = RFCOM_WCDMA_MODE;
  rf_meas_enter.header.target_tech = RFCOM_LTE_MODE;

  rf_meas_enter.header.num_source_item = num_source_item;

  /* Populate the source header params */
  for(src_index = 0; src_index < MIN(num_source_item, WL1_RF_HEADER_DB_NUM_SOURCE); src_index++)
  {
    /* reset the carrier index for num source items 2*/
    carrier_index = 0;

    rf_meas_enter.header.source_param[src_index].device     = (rfm_device_enum_type)rf_source_params[src_index].device;
    rf_meas_enter.header.source_param[src_index].band       = rf_source_params[src_index].serving_band;
    rf_meas_enter.header.source_param[src_index].div_en     = rf_source_params[src_index].div_en;
    rf_meas_enter.header.source_param[src_index].div_device = (rfm_device_enum_type)rf_source_params[src_index].div_device;

    /* Populate the channels in source params */
    while((channel_index < WL1_RFM_MAX_NUM_CARRIERS) && (carrier_index <= rf_source_params[src_index].carrier_type))
  {
      source_param.channel[channel_index++] = rf_source_params[src_index].serving_chan[carrier_index++];
    }
  }

  /* Populate the source enter params for WCDMA */
  source_param.device = wl1_trm_get_primary_rf_device();
  source_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;

  /* Populate the target param device */
  target_param.device = wl1_trm_get_primary_rf_device();

  rf_meas_enter.source_param = &source_param;
  rf_meas_enter.target_param = &target_param;

  rf_result = rfm_meas_common_script_enter(&rf_meas_enter, NULL);
  if (rf_result != RFM_MEAS_COMMON_SUCCESS)
  {
    ERR_FATAL("Failed rfm_meas_common_enter",0,0,0);
  }
}

/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_GET_IRAT_INFO

DESCRIPTION
  The function gets RF tune time (startup and cleanup).
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_get_irat_info(rfm_meas_irat_info_type* rf_meas_info)
{
  rfm_meas_result_type rf_meas_status;

  if(rf_meas_info == NULL)
  {
    ERR_FATAL("INVALID input param. rf_meas_info:%d ",0,0,0);
  }
  
  /* Initializing rf_meas_structure */
  memset(rf_meas_info, 0x0, sizeof(rfm_meas_irat_info_type));

  /* Populate the target and source tech for rf */
  rf_meas_info->header.source_tech = RFCOM_WCDMA_MODE;
  rf_meas_info->header.target_tech = RFCOM_LTE_MODE;
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rf_meas_status = rfm_meas_common_get_irat_info_param(rf_meas_info,NULL);
#else
  rf_meas_status = RFM_MEAS_COMMON_SUCCESS;
#endif  

  if(rf_meas_status == RFM_MEAS_COMMON_FAILURE)
  {
    ERR_FATAL("INVALID RF meas result", 0, 0, 0);
  }
}
#endif /* defined(FEATURE_WCDMA_TO_LTE) && (defined(FEATURE_WCDMA_CM_LTE_SEARCH) || defined(FEATURE_W2L_IDLE_FW_RF_TUNE)) */

/*===========================================================================
FUNCTION  WL1_RFM_DL_ARFCN_TO_CARRIER

DESCRIPTION
  This function gets translates ARFCN values into a carrier frequency, taking
  care of the dual carrier case when necessary.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Uint32 value representing the carrier frequency in kHz
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_rfm_dl_arfcn_to_carrier(
  /* the arfcn(s)  index 1 stores the dual carrier frequency if present */
  uint16 arfcn[], 
  /* the number of carriers */
  uint8 num_car
  )
{
  /* these parameters will be filled in and used to get the carrier frequency */
  rfcom_wcdma_band_type band;
  rfcom_multi_carrier_hspa_id_type num_chan = RFCOM_SINGLE_CARRIER;
  /* carrier in khz  (will be returned) */
  uint32 carrier_khz;

  num_car = WL1_BITCOUNT_U8(num_car);

  /* get frequency information */
  switch(num_car)
  {
    case 1:
    {
      num_chan = RFCOM_SINGLE_CARRIER;
      break;
    }
    case 2:
    {
      num_chan = RFCOM_DUAL_CARRIER;
      break;
    }
#ifdef FEATURE_WCDMA_3C_HSDPA
    case 3:
    {
      num_chan = RFCOM_3_CARRIER;
      break;
    }
#endif /* FEATURE_WCDMA_3C_HSDPA */
    default:
    {
      num_chan = RFCOM_SINGLE_CARRIER;
      break;
    }
  }

  band = wl1_rfm_convert_chan_to_band(arfcn[0]);

  /* get carrier frequency (in kHz) */
  carrier_khz = rfm_get_lo_freq_from_uarfcn(band, arfcn, RFCOMMON_CHANNEL_DL, num_chan);
  return carrier_khz;
}

/*===========================================================================
FUNCTION  wl1_rfm_get_irat_w2w_tune_times

DESCRIPTION
  This function returns the tune-away and tune-back times for a w2w search
  in microseconds.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None.  Addresses of input arguments are filled in with tune times.
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_get_irat_w2w_tune_times(
  /* address of the tune away time variable (in microseconds) */
  uint16 *ssbi_write_time, 
  /* address of the tune back time variable (in microseconds) */
  uint16 *pll_settling_time,
  /* address of a pll_settling_time variable (in microseconds)*/
  uint16 *agc_settling_time,
  /* address of a nbr_preload_offset_time variable (in microseconds)*/
  uint16 *nbr_preload_offset_time,
  /* address of a home_preload_offset_time variable (in microseconds)*/
  uint16 *home_preload_offset_time
  )
{
  /* address will be sent to RF and they will fill in the information we seek */
  rfm_meas_irat_info_type tune_info;
  /* tune_info.irat_info_param needs to point to an allocated irat_info 
     structure before getting tune_info back from RF */
  rfm_meas_wtow_irat_info_param_type irat_info;
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  /* used for error checking */
  rfm_meas_result_type rfm_get_info_result;
#endif

  /** function code starts here **/

  tune_info.header.source_tech = RFCOM_WCDMA_MODE;
  tune_info.header.target_tech = RFCOM_WCDMA_MODE;
  tune_info.irat_info_param = &irat_info;

#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  /* have RF fill in information */
  rfm_get_info_result = rfm_meas_common_get_irat_info_param(&tune_info, NULL);
  if (rfm_get_info_result == RFM_MEAS_COMMON_FAILURE)
  {
    ERR_FATAL("rfm_meas_common_get_irat_info_param() returned failure", 0, 0, 0);
  }

  *ssbi_write_time = (uint16)irat_info.ssbi_write_time;
  *pll_settling_time = (uint16)irat_info.pll_settling_time;
  *agc_settling_time = (uint16)irat_info.pll_settling_time;
  *nbr_preload_offset_time = (uint16)tune_info.nbr_pretune_offset_time;
  *home_preload_offset_time = (uint16)tune_info.home_pretune_offset_time;
#else
  *ssbi_write_time = (uint16)500;
  *pll_settling_time = (uint16)500;
  *agc_settling_time = (uint16)500;
  *nbr_preload_offset_time = (uint16)500;
  *home_preload_offset_time = (uint16)500;
#endif
}
/*===========================================================================
FUNCTION  wl1_rfm_get_freq_from_dl_arfcn

DESCRIPTION
  Get freq from arfcn
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
int32 wl1_rfm_get_freq_from_dl_arfcn(uint16 freq_param)
{
  return(rfm_get_freq_from_dl_arfcn(freq_param));
}

/*===========================================================================
FUNCTION  wl1_rfm_get_ul_freq_from_dl_arfcn

DESCRIPTION
  Get ul freq from dl uarfcn
  
DEPENDENCIES
  None
  
RETURN VALUE
  int
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_rfm_get_ul_freq_from_dl_arfcn(sys_band_class_e_type sys_band, uint16 freq_param)
{
  return(rfm_get_ul_freq_from_dl_uarfcn(sys_band, freq_param));
}

#ifdef FEATURE_WCDMA_TO_LTE
/*===========================================================================
 FUNCTION  wl1_rfm_convert_rx_earfcn_to_freq

 DESCRIPTION
   Returns the carrier frequency, in kHz for the given Rx EARFCN
  
 DEPENDENCIES
   None
  
 RETURN VALUE
   The carrier frequency, in kHz, as an unsigned 32-bit integer
  
 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_rfm_convert_rx_earfcn_to_freq(uint32 earfcn)
{
  /* This parameter is unused, but we have to pass in something. */
  rfcom_band_type_u band;
  band.lte_band = RFCOM_BAND_LTE_INVALID;

  return rfm_get_rx_freq_from_rx_earfcn(earfcn, band, RFM_LTE_MODE);
}
#endif /* FEATURE_WCDMA_TO_LTE */

/*===========================================================================
 FUNCTION  wl1_rfm_register_cpc_drx_state

 DESCRIPTION
   This API is used to inform RF state when CPC-DRX or E-DRX is active

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_register_cpc_drx_state(boolean rtr_ctrl_state)
{
  rfm_register_cpc_drx_state(rtr_ctrl_state);
}

/*===========================================================================
FUNCTION     WL1_RFM_WCDMA_GET_NBR_PHY_DEV_IND

DESCRIPTION
  This function calls RF API to get a proper nbr dev index, and it will be sent 
  to FW by cm_init cmd to know which dev will be used to tune to the nbr freq.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  uint8 - WTR dev index used for nbr freq

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint8 wl1_rfm_wcdma_get_nbr_phy_dev_ind(
  uint16 freq, 
  rfm_device_enum_type device
)
{
  return rfm_wcdma_get_nbr_phy_dev_ind(freq, device);
}

/*===========================================================================
FUNCTION     WL1_RFM_UPDATE_RXLM_BUFFER

DESCRIPTION
  This function calls rfm_update_rxlm_buffer to update the neighbor rxlm buffer contents.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
boolean wl1_rfm_update_rxlm_buffer(rfm_device_enum_type device,
                                   uint16 *interf_arfcn,
                                   uint32 rxlm_buf_idx)
{
  return rfm_update_rxlm_buffer(device, 
                                interf_arfcn, 
                                RFCOM_SINGLE_CARRIER, 
                                rxlm_buf_idx);
}

/*===========================================================================
FUNCTION  WL1_RFM_MEAS_COMMON_BUILD_SCRIPTS

DESCRIPTION
  The function sends a rfm_meas_common_script_build_scripts before a W2W gap search
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_meas_common_build_scrips(
  /* Number of header source items */
  uint8 num_source_item,
  /* RF header source params */
  wl1_rf_source_params_struct_type rf_source_params[],
  /* Neighbor frequency */
  uint16 nbr_freq, 
  /* RXLM home handle */
  uint16 home_rxlm,
  /* RXLM neighbor handle */
  uint16 nbr_rxlm)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT
  #ifdef FEATURE_CM_UNIT_TEST
  #error code not present
#else
  /* Setup parameters for build script */
  rfm_meas_setup_param_type rfm_setup_param;
  /* Tune parameters for W2W */
  rfm_meas_w2w_setup_and_build_scripts_param_type rfm_w2w_tune_param;
  /* Tune parameters for target in W2W */
  rfm_meas_w2w_setup_and_build_scripts_param_type rfm_w2w_tune_target_param;
  /* RFM result type */
  rfm_meas_result_type rfm_result;
  
  /* Set the setup parameters */
  rfm_setup_param.header.source_tech = RFCOM_WCDMA_MODE;
  rfm_setup_param.header.target_tech = RFCOM_WCDMA_MODE;

  rfm_setup_param.header.num_source_item = num_source_item;

  rfm_setup_param.header.source_param[0].device = wl1_trm_get_primary_rf_device();
  rfm_setup_param.header.source_param[0].band = rf_source_params[0].serving_band;

  if (rxd_is_active())
  {
    rfm_setup_param.header.source_param[0].div_en = TRUE;
    rfm_setup_param.header.source_param[0].div_device = wl1_trm_get_secondary_rf_device();
  }
  else
  {
    rfm_setup_param.header.source_param[0].div_en = FALSE;
    rfm_setup_param.header.source_param[0].div_device = RFM_INVALID_DEVICE;
  }

  if ((rfm_setup_param.header.num_source_item == WL1_RF_HEADER_DB_NUM_SOURCE) &&
      (wl1_trm_carrier_aggregation_on()))
  {
    rfm_setup_param.header.source_param[1].device = wl1_trm_get_primary_rf_device_for_ca();
    rfm_setup_param.header.source_param[1].band = rf_source_params[1].serving_band;

    if (rxd_is_active())
    {
      rfm_setup_param.header.source_param[1].div_en = TRUE;
      rfm_setup_param.header.source_param[1].div_device = wl1_trm_get_secondary_rf_device_for_ca();
    }
    else
    {
      rfm_setup_param.header.source_param[1].div_en = FALSE;
      rfm_setup_param.header.source_param[1].div_device = RFM_INVALID_DEVICE;
    }
  }
  else
  {
    rfm_setup_param.header.num_source_item = WL1_RF_HEADER_SB_NUM_SOURCE;
  }

  rfm_setup_param.header.target_param.device = wl1_trm_get_rf_device_for_target(nbr_freq);
  rfm_setup_param.header.target_param.band = wl1_rfm_convert_chan_to_band(nbr_freq);
  rfm_setup_param.header.target_param.div_en = FALSE;

  rfm_w2w_tune_param.home_arfcn[CARR_0] = rf_source_params[0].serving_chan[CARR_0];
  /* Set up the tune parameters */
  #ifdef FEATURE_WCDMA_DC_HSDPA
  if(rf_source_params[0].carrier_type == WL1_RFM_SINGLE_CARRIER)
  {
    if(rf_source_params[1].carrier_type == WL1_RFM_SINGLE_CARRIER)
    {
      rfm_w2w_tune_param.home_arfcn[CARR_1] = rf_source_params[1].serving_chan[CARR_0];
    }
    #ifdef FEATURE_WCDMA_3C_HSDPA
    if(rf_source_params[1].carrier_type == WL1_RFM_DUAL_CARRIER)
    {
      rfm_w2w_tune_param.home_arfcn[CARR_2] = rf_source_params[1].serving_chan[CARR_1];
    }
    #endif
  }
  else if(rf_source_params[0].carrier_type == WL1_RFM_DUAL_CARRIER)
  {
    rfm_w2w_tune_param.home_arfcn[CARR_1] = rf_source_params[0].serving_chan[CARR_1];
    #ifdef FEATURE_WCDMA_3C_HSDPA
    if(rf_source_params[1].carrier_type == WL1_RFM_SINGLE_CARRIER)
    {
      rfm_w2w_tune_param.home_arfcn[CARR_2] = rf_source_params[1].serving_chan[CARR_0];
    }
    #endif
  }
  #ifdef FEATURE_WCDMA_3C_HSDPA
  else if(rf_source_params[0].carrier_type == WL1_RFM_3_CARRIER)
  {
    rfm_w2w_tune_param.home_arfcn[CARR_1] = rf_source_params[0].serving_chan[CARR_1];
    rfm_w2w_tune_param.home_arfcn[CARR_2] = rf_source_params[0].serving_chan[CARR_2];
  }
  #endif /* FEATURE_WCDMA_3C_HSDPA */
  #endif /* FEATURE_WCDMA_DC_HSDPA */

  /* Target tune params */
  rfm_w2w_tune_param.nbr_arfcn =  nbr_freq;
  rfm_w2w_tune_param.carrier_type = (rfcom_multi_carrier_hspa_id_type)rf_source_params[0].carrier_type;
  rfm_w2w_tune_param.home_buffer_index = home_rxlm;
  rfm_w2w_tune_param.nbr_buffer_index = nbr_rxlm;

  MSG_6(MSG_SSID_DFLT, MSG_LEGACY_MED,
        "SRCHCM DBDC: source dev id %d band %d home[0] %d, source dev id %d band %d home[1] %d", 
        rfm_setup_param.header.source_param[0].device,
        rfm_setup_param.header.source_param[0].band,
        rfm_w2w_tune_param.home_arfcn[CARR_0],
        rfm_setup_param.header.source_param[1].device,
        rfm_setup_param.header.source_param[1].band,
        rfm_w2w_tune_param.home_arfcn[CARR_1]);

  WL1_MSG2(MED, "SRCHCM DBDC: target dev id %d band %d",
          rfm_setup_param.header.target_param.device,
                rfm_setup_param.header.target_param.band);

  /* For W2W, target carrier type should always be set to SC even if source is SC or DC */
  rfm_w2w_tune_target_param = rfm_w2w_tune_param;
  rfm_w2w_tune_target_param.carrier_type = RFCOM_SINGLE_CARRIER;

  rfm_setup_param.source_param = (void*)&rfm_w2w_tune_param;
  rfm_setup_param.target_param = (void*)&rfm_w2w_tune_target_param;


  /* Prepare the clean-up and start-up scripts and populate them at the RF buffer index */
  rfm_result = rfm_meas_common_script_build_scripts( &rfm_setup_param , NULL );

  if (rfm_result == RFM_MEAS_COMMON_FAILURE)
  {
    ERR_FATAL("Failure to build RF scripts for W2W CM",0,0,0);
  }
  #endif /* FEATURE_CM_UNIT_TEST */
#else
  WL1_MSG0(HIGH, "No code");
#endif
}

/*===========================================================================
 FUNCTION  wl1_rfm_wcdma_antenna_tuner_tick

 DESCRIPTION
   Tells RF its time for them to make a small tuner adjustment.  Should be 
   called periodically by the wl1 ATuner module.
  
 DEPENDENCIES
   None
  
 RETURN VALUE
   void
  
 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_wcdma_antenna_tuner_tick(void)
{
  rfm_wcdma_antenna_tuner_tick();
}

/*===========================================================================
 FUNCTION  wl1_rfm_get_default_band_chan

 DESCRIPTION
   get the default band and channel info.
 
 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_get_default_band_chan(rfi_band_type *band, uint16 *chan)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT  
  rfm_wcdma_get_default_band_chan(band, chan);
#else
  *band = RFI_IMT_BAND; 
  *chan = 10700;
#endif
}

/*===========================================================================
FUNCTION  wl1_rfm_perp_temp_comp

DESCRIPTION
  This function will prepare RF for temperature compensation by performing static update. 
  This update includes the reading of current temparature and applying the temperature 
  specific compensation to Tx linearizer tables. This function calls a CB function at the completion
  of the computations and L1 will trigger Part 2 temp comp based on this.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Will call CB function specified by L1, which in turn will trigger Part2 Temp Comp

===========================================================================*/
#ifdef __GNUC__ 
static inline 
#else
INLINE 
#endif
void wl1_rfm_prep_temp_comp(rfm_cb_handler_type cb_handler)
{
  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_wcdma_temp_comp_param intf_param;
  intf_param.device = wl1_trm_get_primary_rf_device();

  rfm_wcdma_prep_temp_comp(&intf_param,
                           cb_handler,
                           NULL
                          );
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_exec_temp_comp

DESCRIPTION
  This function will perform dynamic update to WFW for RF TX Temp Comp which
  includes performing rxlm dynamic update to FW to perform temperature based IQMC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Calls MPSP Sync function to update WFW for the RF Temp Comp Procedure

===========================================================================*/
#ifdef __GNUC__ 
static inline 
#else
INLINE 
#endif
void wl1_rfm_exec_temp_comp(void)
{
  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_wcdma_temp_comp_param intf_param;
  intf_param.device = wl1_trm_get_primary_rf_device();

  rfm_wcdma_exec_temp_comp(&intf_param, NULL, NULL);
  #endif
}

/*===========================================================================
FUNCTION  wl1_rfm_abort_temp_comp

DESCRIPTION
  Aborts the temp comp computation that is performed as part of rfm_wcdma_prep_temp_comp()

DEPENDENCIES
  rfm_wcdma_prep_temp_comp() must have been called before calling this API

RETURN VALUE
  None

SIDE EFFECTS
  Will call rfm_wcdma_abort_temp_comp which will abort the RF Temp Comp Part1 Computations
  performed in rfm_wcdma_prep_temp_comp()

===========================================================================*/
#ifdef __GNUC__ 
static inline 
#else
INLINE 
#endif
void wl1_rfm_abort_temp_comp(void)
{
  #ifndef FEATURE_WCDMA_RUMI_SUPPORT
  rfm_wcdma_temp_comp_param intf_param;
  intf_param.device = wl1_trm_get_primary_rf_device();

  rfm_wcdma_abort_temp_comp(&intf_param, NULL, NULL);
  #endif
}

/*===========================================================================
 FUNCTION  wl1_rfm_flush_agc_data

 DESCRIPTION
   Flushes AGC data collected till this point. Function will be called 
   after a long tune away in dsds mode.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_flush_agc_data(void)
{
#ifndef FEATURE_WCDMA_RUMI_SUPPORT  
  rfm_flush_agc_data(RFCOM_TRANSCEIVER_0);
#endif
}

/*===========================================================================
 FUNCTION  wl1_rfm_set_nbr_enable

 DESCRIPTION
   Enable NBR config to RF and send the RxLM buffer index.

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_rfm_set_nbr_enable(boolean nbr_enable, uint32 rxlm_buf_idx)
{
  rfm_set_nbr_enable(nbr_enable, rxlm_buf_idx);
}

/*===========================================================================
 FUNCTION  wl1_skip_agc_disable_cmd

 DESCRIPTION
   API sets the flag used by RF to skip sending AGC cmd to FW 
   in case of early RF shutdown

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_skip_agc_disable_cmd(boolean skip_agc)
{
  wl1_skip_sending_agc_disable = skip_agc;
}
#endif /* FEATURE_WCDMA */

/*===========================================================================
 FUNCTION  WL1_SET_RF_STATE

 DESCRIPTION
   API used to set the current RF state. Device ID specifies the RF chain -
   Primary or diversity

 DEPENDENCIES
   None

 RETURN VALUE
   None

 SIDE EFFECTS
   None
===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_set_rf_state(wl1_rf_sleep_status_enum_type rf_state, uint8 chain_id)
{
  if (chain_id > SECONDARY_CHAIN)
  {
    WL1_MSG1(ERROR," Unsupported device ID: %d", chain_id);
    return;
  }
  wl1_rf_state[chain_id] = rf_state;
}
#endif /* WL1RFMIF_H */

