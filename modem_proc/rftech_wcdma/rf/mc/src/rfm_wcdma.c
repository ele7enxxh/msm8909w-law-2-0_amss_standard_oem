/*!
   @file
   rfm_wcdma.c

   @brief
   This file contains all declarations and definitions necessary to use the RF
   WCDMA drivers.

   @details

*/

/*===========================================================================
Copyright (c) 2008 - 2014 by Qualcomm Technologies, Inc. All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/mc/src/rfm_wcdma.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/17/14   vbh     Add an API to provide Tx max power value by accepting band, device as a param
11/12/14   sd      Clean up of deprecated sleep/wakeup routines
10/29/14   ag      Fixed compiler warning 
09/26/14   vbh     Remove critical section lock from tx_retune_bw
09/15/14   joz     replace FEATURE_DIMEPM_MODEM by FEATURE_JOLOKIA_MODEM
09/15/14   vbh     Added device as a parameter to support Tx sharing
08/25/14   dw      Fix warnings
08/21/14   rmb     Remove unused APIs.
08/20/14   vbh     Updates to Exec Tune to allow tune to chan if prep tune skipped
08/14/14   vbh     Added support for TX APIs for init/deinit, enable/disable
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
08/06/14   aro     Support to override TempComp mode of operation
08/05/14   vbh     Added a parameter in rfwcdma sleep api to pass the skip agc flag
08/06/14   dw      Added API support to query different FTM states for WL1
08/01/14   vbh     [1] Updated state checks for prep/exec tune 
                       as script based tune for DC not supported 
                   [2] Allow sleep calls from RxTX state as an interim solution for DBDC
07/28/14   nv      We are making sure to access TX parameters only when we are
                   in TX state during Temp. Comp.
07/21/14   JJ      Use therm bins from each tech to get temp comp value
06/26/14   vbh     Use script based tune only if Tx is not ON in online mode
06/19/14   aro/ak  Added Support for Prep/Exec Temp Comp Split
06/16/12   dw      Fix retune_bw state machine checking
06/06/12   ac      nbr support
05/15/14   aro     Deprecated old command to perform MTPL update
05/15/14   aro     Added Prep/Exec Temp Comp stubs
05/15/14   ak      Add parameter to sleep_rx API check for early shutdown
05/01/14   rmb     Add support for new version of AGC log packet 0x4176 v2.
04/25/14   vs      Added stub for rfm_wcdma_prep_init_tx
04/11/14   vbh     [FR17754] Added support for script based tuning
04/04/14   zhh     KW error fix
03/28/14   vbh     FR2001 sleep API support for concurrent scenarios
03/25/14   rmb     Add a new get rxagc API during X2W measurements. 
03/13/14   vbh     Update the FR2001 routines to support concurrent wakeup routines
03/11/14   dw      Add Bolt DCHSUPA support
03/03/14   aa      Compiler warning fix
02/28/14   aa      DimePM linker error fix
02/24/14   vbh     Add sleep_Rx api to keep consistent naming convention
02/04/14   vbh     FR2001 - Prep/Exex wakeup algorithm changes
08/29/13   vs      Add RFM API to return RxAGC for all carriers
08/23/13   vs      Added support for 3 carrier
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build 
08/05/13   vs      Modified RF interface to query DBDC bands supported
08/01/13   dw      Add support for L1 to get the W2W neighbor physical device index 
07/31/13   aa      RF API to convert RF WCDMA band type to TRM band type
07/19/13   ac      DBDC bringup related changes
07/26/13   vs      Add RF interface to query DBDC bands supported
07/11/13   aro     Compiler Warning fix
07/10/13   ry      Included header to Fix Compiler Warning
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/24/13  ac       tx freezeaccum api change for both car and txlm state for dc cfg 
07/27/13   ry      Updated the API rfm_get_ul_freq_from_dl_uarfcn()
05/22/13  ka/ac    dc hsupa changes
05/09/13   jj      Update RF API rfm_get_ul_freq_from_dl_uarfc()
05/09/13   jj      RF API to get UL frequency from DL UARFCN
05/03/13   vb      Removed reduntant message during 200ms antenna tuner tick
04/29/13   jj      Add API (rfm_wcdma_get_default_band_chan) for WL1 to get default band and chan
04/04/13   ka      WCDMA state machine updates
03/28/13   kcj     Added rfm_wcdma_wakeup_tx function.
03/07/13   kai     Added rfm_wcdma_decfg_tx function
02/13/13   vb      Cleanup the old antenna tuner implementation
02/06/13   rmb    Added rfm_set_wakeup_optimization function.
02/04/13   ac      add api for hs->r99/r99->hs transition for ici coeff update
12/18/12   aa      Commented out the antenna tuner measurement API
10/10/12   kai/vb  Changed Qtuner interface APIs 
9/27/12    nv      Added new API to flush data.
09/03/12   zc      Fixing compiler warning
08/29/12    vb     Qtuner interface APIs 
06/13/12   swb     Add rxlm buffer index to rfm_pretune_to_chan function 
05/08/12   kai     Add API for CPC register setting  
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
02/27/12   swb     Return indication of missing cal data within init_tx function
02/22/12   swb     Inter-freq tune optimization: add rfm_pretune_to_chan function
02/16/12   kai     Added API for return PA state in func rfwcdma_mc_cmd_dispatch 
01/03/12   sty     fixed calls to RFM_DEBUG_EN_Q
12/01/11   dw      Add support for WCDMA low power mode (DSR)
10/13/11   swb     Added API for freezing Rx AGC accumulator 
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE. 
08/18/11   vb      Added support for splitting rfm_enable_tx &
                   added rfm_init_wcdma_tx
07/28/11   ckl     Remove obsolete rfm_get_cgagc_settling_time_for_tune() function
05/05/11   dw      Fix warning
05/05/11   dw      Add rfm_get_freq_from_dl_arfcn() 
04/05/11   sar     Included rfm_helper_incs.h for debug code. 
03/25/11   dw      add rfm_update_rxlm_buffer()
03/02/11   dw      Obsolete rfm_set_tx_general_ctls()
02/09/11   dw      Add RxLM interface support for W2W
01/05/10   dw      Initial support for RxLM
09/14/10   ap      Added GP_CLK enable/disable API  
07/23/10   ka      Change dual carrier interface to accept channel 
07/20/10   ka      Added API to determine if dual carrier is supported 
03/24/10   ka      Added support for DC wtow.
03/24/10   kguo    Removed duplicated line from rfm_enable_data_mover_clk_for_cm 
03/19/10   ad      Fix rxagc reads of car 0 
03/16/10   kguo    Merged from scmm_final_merge branch
02/16/10   vb      Klockwork error fixes
01/20/10   tws     Fix bad merge.
01/18/10   ckl     Removed med level lint error.
01/06/10   kguo    Merge from mdm9k dev branch to main\latest
11/30/09   ckl     Added rfm_get_freq_from_dl_arfcn( ) for W2W measurement
11/11/09   ka      Updates for dual carrier.
10/23/09   kguo    Merged changes for 9k bringup
10/11/09   ckl     Removed all warnings
10/03/09   ckl     Fixed rfm_get_freq_from_uarfcn( ) function
09/17/09   ckl     Removed direct function calls.
08/11/09   bn      Get HDET threshold API
07/21/09   bn      Fix rfm_get_hdet_tx_agc_value API
07/07/09   bn      TxAGC dB10 changes
06/05/09   bn      Bug Fix - To stop a pointer of pointer creation
06/04/09   ckl     Added rfm_get_freq_from_uarfcn, rfm_convert_chan_to_band,
                   rfm_get_hdet_tx_agc_value.
03/09/09   ckl     Code cleanup
10/14/08   sr      Initial version to separate wcdma specific external interface.

============================================================================*/

#include "rfcom.h"
#include "rfm.h"
#include "msg.h"
#include "rfm_types.h"
#include "rfm_helper_incs.h"
#include "rfm_async_types.h"
#include "rfcommon_msg.h"
#include "rfcommon_time_profile.h"
#include "rfwcdma_data.h"
#include "rfwcdma_core.h"
#include "rfc_common.h"
#include "rfc_wcdma_data.h"
#include "rfwcdma_mc_meas.h"
#include "rfwcdma_core_log.h"
#include "rfm_wcdma_umid.h"
#include "rf_apps_task.h"
#include "rfm_wcdma.h"
#include "rfcommon_core.h"
#include "rfwcdma_core_rxctl.h"
#include "rfwcdma_core_xpt.h"
#include "rfwcdma_mdsp_types.h"
#include "rfwcdma_mdsp_async.h"
#include "rfcommon_fbrx_types.h"
#include "rfcommon_fbrx_api.h"
#include "rfwcdma_core_txplim.h"
#include "rfm_wcdma.h"
#include "ftm.h"
#include "rfcommon_math.h"
#include "rfwcdma_core_util.h"

#ifdef FEATURE_RF_HAS_QTUNER
#include "rfmeas_mc.h"
#include "rfwcdma_core_antenna_tuner.h"
#endif
/*! @cond vbhide */

extern boolean enable_wakeup_optimization_flag;

/* WCDMA RF FTM state */
rfm_wcdma_ftm_state_type rfm_wcdma_ftm_state = RFM_WCDMA_FTM_NONE;
#define Tx_device 1


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This is used to set an RF physical path (antenna -> demod -> RxFE, etc.) to
  the RFCOM_TRANSCEIVER_0 logical device.  Typically, the RFCOM_TRANSCEIVER_0 is
  mapped with the physical RF path 0 (intuitive!). However, for certain test
  cases, we might want to exercise the secondary (diversity) physical RF path,
  and therefore map it to the logical device RFCOM_TRANSCEIVER_0.

  This also has the effect of mapping the "other" RF physical path to the
  "other" logical device.  For example calling rfm_set_primary_path( RF_PATH_0)
  maps RFCOM_TRANSCEIVER_0 logical device to RF_PATH_0 physical device AND maps
  RFCOM_RECEIVER_DIV logical device to RF_PATH_1 physical device.

*/
void rfm_set_primary_path( rf_path_enum_type primary_path )
{
   RFWCDMA_MC_V(set_primary_path)(primary_path);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This API returns the main RF physical path back to L1

*/
rf_path_enum_type rfm_get_primary_path(void)
{
  int32 ret_val = 0;

  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0,(int32)RFWCDMA_MC_GET_PRIMARY_PATH,0);

  return ((rf_path_enum_type)ret_val);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Adjusts the Tx power.  Reads HDET value and adjusts Tx power based
  on HDET value, temperature, and frequency.

*/
int32 rfm_do_tx_pwr_limit( rfcom_device_enum_type device )
{
  RF_MSG( RF_FATAL, "rfm_do_tx_pwr_limit: Deprecated" );
  return -1;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Performs temperature compensation.  Adjusts AGC linearizer tables
  based on temperature.  Must be called at 10 second intervals to perform
  temperature compensation.

  @pre
  rfm_init() must have already been called.
*/
void rfm_do_temp_comp( rfcom_device_enum_type device )
{
  RFM_DEBUG_EN_Q(&rfm_do_temp_comp, 0, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(device, (int32)RFWCDMA_DO_TEMP_COMP, 0);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Sets the Tx power limit to value given in units of dBm.

  @pre
  rfm_init() must have already been called.

*/
void rfm_set_tx_pwr_limit ( rfcom_device_enum_type device,
                            int16 tx_limit_dbm,
                            rfcom_txplim_channel_type chan_type )
{
  rfwcdma_mc_tx_pwr_limit_type pwr_limit;
  pwr_limit.tx_limit_dbm = tx_limit_dbm;
  pwr_limit.chan_type = chan_type;

  RFM_DEBUG_EN_Q(&rfm_set_tx_pwr_limit, 0, tx_limit_dbm, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_SET_TX_PWR_LIMIT, &pwr_limit);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value in units of dB10.

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value in units of dB10.
*/
int16 rfm_get_rx_agc_val ( rfcom_device_enum_type device )
{
  int32 rx_agc = 0;
  rfcom_multi_carrier_id_type carrier_id = RFCOM_MULTI_CARRIER_ID__0;

  RFM_DEBUG_EN_Q(&rfm_get_rx_agc_val, 0, 0, 0, 0);

  RFWCDMA_MC(cmd_dispatch, rx_agc)(device, (int32)RFWCDMA_MC_GET_RX_AGC, &carrier_id);

  return (int16)rx_agc;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value in db10 units

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value in units of dB10.
*/
int16 rfm_get_rx_raw_agc_val ( rfcom_device_enum_type device )
{
  int32 rx_agc = 0;
  rfcom_multi_carrier_id_type carrier_id = RFCOM_MULTI_CARRIER_ID__0;

  RFM_DEBUG_EN_Q(&rfm_get_rx_raw_agc_val, 0, 0, 0, 0);

  RFWCDMA_MC(cmd_dispatch, rx_agc)(device, RFWCDMA_MC_GET_RAW_RX_AGC, &carrier_id);

  return (int16)rx_agc;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value in db10 units for W neighbour during X2W 

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value in units of dB10.
*/
int16 rfm_get_rx_raw_agc_val_xtow_meas ( rfcom_device_enum_type device, rfcom_wcdma_band_type neigh_band, boolean bypass_rxagcon )
{
  int32 rx_agc = 0;

  rfwcdma_mc_meas_rxagc_xtow get_rxagc_params = {neigh_band, bypass_rxagcon};

  RFM_DEBUG_EN_Q(&rfm_get_rx_raw_agc_val_xtow_meas, 0, 0, 0, 0);

  RFWCDMA_MC(cmd_dispatch, rx_agc)(device, RFWCDMA_MC_GET_RAW_RX_AGC_X2W_MEAS, &get_rxagc_params);

  return (int16)rx_agc;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value in dbm units for W neighbour during X2W 

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value in units of dBm.
*/
int16 rfm_get_rx_agc_val_xtow_meas ( rfcom_device_enum_type device, rfcom_wcdma_band_type neigh_band, boolean bypass_rxagcon )
{
  int32 rx_agc = 0;

  rfwcdma_mc_meas_rxagc_xtow get_rxagc_params = {neigh_band, bypass_rxagcon};

  RFM_DEBUG_EN_Q(&rfm_get_rx_raw_agc_val_xtow_meas, 0, 0, 0, 0);

  RFWCDMA_MC(cmd_dispatch, rx_agc)(device, RFWCDMA_MC_GET_RX_AGC_X2W_MEAS, &get_rxagc_params);

  return (int16)rx_agc;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Tx AGC value in units of dBm.

  @pre
  rfm_init() must have already been called.

  @retval
  Tx AGC value in units of dBm.
*/
int16 rfm_get_tx_agc_val ( rfcom_device_enum_type device )
{
  int32 tx_agc = 0;
  
  RFM_DEBUG_EN_Q(&rfm_get_tx_agc_val, 0, 0, 0, 0);

  RFWCDMA_MC(cmd_dispatch, tx_agc)(device, (int32)RFWCDMA_MC_GET_TX_AGC_VAL, 0);
  return (int16)tx_agc;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value for RSCP.

  @retval
  Rx AGC value in dbm.
*/
int16 rfm_get_receive_agc_val_for_rscp( rfcom_device_enum_type device,
                                        rfcom_rxagc_type rxagc_type )
{
  int32 agc_val = 0;
  RFWCDMA_MC(cmd_dispatch, agc_val)(device, (int32)RFWCDMA_MC_GET_RX_AGC_VAL_FOR_RSCP, &rxagc_type );
  return (int16)agc_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Minimum Tx Power value in units of dB10.

  @retval
  Minimum Tx Power value in units of dB10.
*/
int16 rfm_get_min_tx_power_val( void )
{
  int32 min_tx_pwr_val = 0;
  RFWCDMA_MC(cmd_dispatch, min_tx_pwr_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MIN_TX_PWR_VAL, 0);
  return (int16)min_tx_pwr_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the hdet threshould value in units of dB10.

  @retval
  Hdet threshold value in units of dB10.
*/

int16 rfm_get_hdet_apply_threshold_agc( void )
{
  int32 hdet_threshold = 0;
  RFWCDMA_MC(cmd_dispatch,hdet_threshold)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_HDET_THRESHOLD_AGC, 0);
  return (int16)hdet_threshold;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rfm_get_wtow_rf_ctl_buf ( uint16 arfcn, void *buf_ptr )
{
  rfwcdma_mc_wtow_ctl_type wtow_ctl;
  wtow_ctl.arfcn = arfcn;
  wtow_ctl.buf_ptr = (uint8 *)buf_ptr;

  RFM_DEBUG_EN_Q(&rfm_get_wtow_rf_ctl_buf, arfcn, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_WTOW_RF_CTL_BUF, &wtow_ctl);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
void rfm_get_multi_carrier_wtow_rf_ctl_buf ( uint16 arfcn[], 
                                             rfcom_multi_carrier_hspa_id_type carrier_type, 
                                             void *buf_ptr 
                                            )
{
  rfwcdma_mc_multi_carrier_wtow_ctl_type wtow_ctl;
  uint8 i;

  for (i=0;i<RFCOM_MAX_CARRIERS;i++)
  {
    wtow_ctl.arfcn[i] = arfcn[i];
  }

  wtow_ctl.carrier_type = carrier_type;

  wtow_ctl.buf_ptr = (uint8 *)buf_ptr;

  RFM_DEBUG_EN_Q(&rfm_get_wtow_rf_ctl_buf, arfcn, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, RFWCDMA_MC_GET_MULTI_CARRIER_WTOW_RF_CTL_BUF, &wtow_ctl);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function freeze/unfreezes the TX accumulator.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/

void rfm_set_txfreezeaccum( uint8 val,rfcom_multi_carrier_id_type car_idx)
{
  rfwcdma_mc_tx_freezeaccum_type tx_freezeaccum;
  tx_freezeaccum.val= val;
  tx_freezeaccum.car_idx = car_idx;

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_SET_TX_FREEZE_ACUUM, &tx_freezeaccum);
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function turns on PA.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
void rfm_turn_on_pa( void )
{
  RFM_DEBUG_EN_Q(&rfm_turn_on_pa, 0, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_TURN_ON_PA, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function turns off PA.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
void rfm_turn_off_pa( void )
{
  RFM_DEBUG_EN_Q(&rfm_turn_off_pa, 0, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_TURN_OFF_PA, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables UL closed loop power control by removing the TX AGC
  gain value override.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
void rfm_enable_pwr_ctrl( void )
{
  RFM_DEBUG_EN_Q(&rfm_enable_pwr_ctrl, 0, 0, 0, 0);

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_ENABLE_PWR_CTRL, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the contribution to the transmit power that reflects
  the accumulation of UL TPC commands.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/

int16 rfm_get_txagctablestore(rfcom_multi_carrier_id_type car_idx)
{
  int32 tx_agc_tables = 0;
  RFWCDMA_MC(cmd_dispatch, tx_agc_tables)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_TX_AGC_TABLES, &car_idx);
  return (int16)tx_agc_tables;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function converts and returns TX power value in dbm unit based on
  the passed in txagcval value in db10 unit.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.

*/
int16 rfm_convert_tx_agc_to_dbm_unit( int16 txagcval )
{
  int32 txdbmval = 0;

  RFWCDMA_MC(cmd_dispatch, txdbmval)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_TX_AGC_TO_DBM, &txagcval);
  return (int16)txdbmval;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function convert and return TX AGC value in db10 unit based on the
  passed in dbm unit value.

  @retval
  The TX AGC value in db10 unit.
*/
int16 rfm_convert_tx_dbm_to_agc_unit( int16 dbmval )
{
  int32 txagcval = 0;

  RFWCDMA_MC(cmd_dispatch, txagcval)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_TX_DBM_TO_AGC, &dbmval);
  return (int16)txagcval;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the maximum TX power that UE is capable to transmit
  in dbm unit.

  @retval
  The maximum TX power in dbm unit.
*/
int16 rfm_get_ue_max_tx_power( rfcom_txplim_channel_type chan_type )
{
  int32 max_tx_pwr = 0;

  RFWCDMA_MC(cmd_dispatch, max_tx_pwr)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_UE_MAX_TX_PWR, &chan_type );
  return (int16) max_tx_pwr;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function provides the Tx rotator angle values from RF NV.

*/
void rfm_get_tx_rotator_angle (uint16 *tx_rot_ang_pa_00_p,
                               uint16 *tx_rot_ang_pa_01_p,
                               uint16 *tx_rot_ang_pa_10_p,
                               uint16 *tx_rot_ang_pa_11_p)
{
  rfwcdma_mc_rot_angle_type tx_rot_angle;
  tx_rot_angle.tx_rot_ang_pa_00_p = tx_rot_ang_pa_00_p;
  tx_rot_angle.tx_rot_ang_pa_01_p = tx_rot_ang_pa_01_p;
  tx_rot_angle.tx_rot_ang_pa_10_p = tx_rot_ang_pa_10_p;
  tx_rot_angle.tx_rot_ang_pa_11_p = tx_rot_ang_pa_11_p;

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_TX_ROTATOR_ANGLE, &tx_rot_angle);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function checks if the PA is on.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
uint8 rfm_pa_is_on( void )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch,ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_PA_IS_ON, 0 );
  return (uint8)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function logs the AGC data.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
void rfm_log_single_agc_data (rfcom_device_enum_type device)
{
  /* This is new version (Ver 2) of log packet 0x4176*/
  rfwcdma_core_log_agc_data_once();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the encoder BTF (propagation delay from modulator to antenna)
  value in chip unit.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
uint16 rfm_get_enc_btf_val( void )
{
  int32 btf_val = 0;
  RFWCDMA_MC(cmd_dispatch, btf_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_ENC_BTF_VAL, 0 );
  return (uint16)btf_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  The function updates the passed rf_max_tx_pwr_limit_type structure with
  MTPL (Max Tx Power Limit) values for difference BCS case in Tx AGC counts.
  L1 should call this function on every frame boundary and update
  the L1-DSP table with MTPL values.

  @pre
  rfm_init() must have already been called.
  MDSP must be downloaded and initialized for mode of operation specified
  by mode before calling this function.
*/
void rfm_get_max_tx_pwr_limit( rf_max_tx_pwr_limit_type *rf_mtpl )
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MAX_TX_PWR_LIMIT, rf_mtpl );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables the carrier frequency error PDM output.

  @pre
  rfm_init() must have already been called.
*/
void rfm_enable_trk_lo_adj_pdm( boolean enable )
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_ENABLE_TRK_LO_ADJ_PDM, &enable );

} /* rfm_enable_trk_lo_adj_pdm */

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function turns on RF regulators.

  @pre
  rfm_init() must have already been called.
*/
void rfm_turn_on_rf_ldo( uint16 delay ) /* time in us to wait after turning on LDO */
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_TURN_ON_RF_LDO, &delay );

} /* rfm_turn_on_rf_ldo */

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
rfi_band_type rfm_get_curr_band( void )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_CURR_BAND, 0);
  return (rfi_band_type)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details

*/
uint8 rfm_srchcm_interf_rfsetup_mdsp_buf_sz_w16_get(void)
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MDSP_BUF_SIZE_W16, 0);
  return (uint8)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the maximum TX power indicated by NV item depending
  on the channel type

  @retval
  The max TX power value.

*/
int16 rfm_get_max_tx_power_nv_item(rfcom_txplim_channel_type chan_type)
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MAX_TX_PWR_CONFIG_ITEM, &chan_type);
  return (int16)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function sets the TX_GENERAL_CTL register.

  @pre
  rf_init is called.
*/
void rfm_set_tx_general_ctl( rfcom_txplim_channel_type chan_type )
{
  
  MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Deprecated Function. TX_GENERAL_CTL not set by RF!", 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the rf warmup time to L1.

  @pre
  Rf warmup time in uS.

*/
uint16 rfm_get_rf_warmup_time( void )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_RF_WARMUP_TIME, 0);
  return (uint16)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables/disables the Data Mover clock for compressed mode
  depending on RF target.

  @pre
  rf_init is called.
*/
void rfm_enable_data_mover_clk_for_cm ( boolean enable )
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_ENABLE_DATA_MOVER_CLK_FOR_CM, &enable);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function enables/disables the Data Mover clock for HDET
  depending on RF target.

  @pre
  rf_init is called.

  @retval
  rfcom_dm_for_hdet_type:
  HDET_DM_NOT_REQUIRED if RF target doesn't require DM for HDEt
  HDET_DM_CLK_ENABLED if DM clock is enabled at return of function
  HDET_DM_CLK_DISABLED if DM clock is disabled at return of function
*/
rfcom_dm_for_hdet_type rfm_enable_data_mover_for_hdet (boolean enable)
{
  int32 hdet_type = 0;
  RFWCDMA_MC(cmd_dispatch, hdet_type)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_ENABLE_DATA_MOVER_FOR_HDET, &enable);
  return (rfcom_dm_for_hdet_type)hdet_type;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns whether a given feature is required by
  the current RF target

  @pre
  rf_init is called.

  @retval
  TRUE if feature is needed or enabled. FALSE otherwise.
*/
boolean rfm_get_rf_capability ( rfcom_rf_cap_type feature )
{
  int32 rf_cap = 0;
  RFWCDMA_MC(cmd_dispatch, rf_cap)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_RF_CAPABILITY, &feature);
  return (boolean)rf_cap;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
   This function returns the current TX agc value in mdsp agc units.
   To convert to dBm use the formula dBm = (mdsp units + 512)/12 - 57.

  @pre
   WCDMA mdsp fw image must be loaded.

  @retval
  tx agc value. range is -512..511 with 1/12 dB resolution
*/
int16 rfm_get_mdsp_tx_agc ( void )
{
  int32 tx_agc = 0;
  RFWCDMA_MC(cmd_dispatch, tx_agc)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MDSP_TX_AGC, 0);
  return (int16)tx_agc;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
   This function returns the maximum TX power limit value in mdsp agc units.
   To convert to dBm use the formula dBm = (mdsp units + 512)/12 - 57.

  @pre
   WCDMA mdsp fw image must be loaded.

  @retval
  tx power limit value. range is -512..511 with 1/12 dB resolution
*/
int16 rfm_get_mdsp_tx_power_limit ( void )
{
  int32 tx_pwr_limit = 0;
  RFWCDMA_MC(cmd_dispatch, tx_pwr_limit)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_MDSP_TX_PWR_LIMIT, 0);
  return (int16)tx_pwr_limit;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Query function to check if a particular band has Rx Diversity capabilities

  @retval
  TRUE if platform supports RxD in desired band
*/
boolean rfm_band_supports_rxd( rfi_band_type band )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_BAND_SUPPORTS_RXD, &band);
  return (boolean)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Query function to check if a particular band has dual carrier capability

  @retval
  TRUE if platform supports dual carrier in desired band
*/
boolean rfm_band_supports_dual_carrier( uint16 chan )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_BAND_SUPPORTS_DC, &chan);
  return (boolean)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function writes tech dependent lna nv items to mdsp

*/
#ifndef FEATURE_WCDMA_DB_DC_HSDPA 
void rfm_write_tech_dependent_nv_items_to_mdsp(rfcom_tech_type tech)
{
  
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_WRITE_TECH_DEPENDENT_CONFIG_TO_MDSP, &tech);
}
#else
void rfm_write_tech_dependent_nv_items_to_mdsp(rfcom_wcdma_band_type band,rfcom_tech_type tech)
{

  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_WRITE_TECH_DEPENDENT_CONFIG_TO_MDSP, &tech);
}
#endif
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Logs AGC data values for QXDM.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

*/
void rfm_log_agc(rfcom_device_enum_type device)
{
  /* This is new version (Ver 2) of log packet 0x4176*/
  rfwcdma_core_log_agc_data();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Flushes the AGC data collected till now 

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.

*/

void rfm_flush_agc_data(rfcom_device_enum_type device)
{
  rfwcdma_core_log_agc_data_flush();
}


/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function gets the current value of Tx power limit in units of
  Tx AGC counts. (-512 to +511).

  @pre
  rfm_init() must have already been called.

*/
int16 rfm_get_tx_pwr_limit( void )
{
  int32 ret_val = 0;
  RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_TX_PWR_LIMIT, 0 );
  return (int16) ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value.

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value.
*/
int16 rfm_get_receive_agc_val (rfcom_device_enum_type device,
                               rfcom_rxagc_type rxagc_type,
                               rfcom_rxagc_unit_type rxagc_unit)
{
  int32 ret_val = 0;
  rfwcdma_mc_rxagc_type rxagc;
  rxagc.type = rxagc_type;
  rxagc.unit = rxagc_unit;
  rxagc.carr_id = RFCOM_MULTI_CARRIER_ID__0 ;

  RFWCDMA_MC(cmd_dispatch, ret_val)( device, RFWCDMA_MC_GET_RX_AGC_C0_VAL, &rxagc );

  return (int16)ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Returns the Rx AGC value for Secondary carrier.

  @pre
  rfm_init() must have already been called.

  @retval
  Rx AGC value.
*/

int16 rfm_get_receive_agc_car1_val (rfcom_device_enum_type device,
                               rfcom_rxagc_type rxagc_type,
                               rfcom_rxagc_unit_type rxagc_unit)
{
  int32 ret_val = 0;
  rfwcdma_mc_rxagc_type rxagc;
  rxagc.type = rxagc_type;
  rxagc.unit = rxagc_unit;
  rxagc.carr_id = RFCOM_MULTI_CARRIER_ID__1 ;

  RFWCDMA_MC(cmd_dispatch, ret_val)( device, RFWCDMA_MC_GET_RX_AGC_C1_VAL, &rxagc );
  return (int16)ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
    RFM API to return RxAGC for specific carrier

  @details
  Returns the Rx AGC value for any given carrier in specified units.

  @PreCondition
    rfm_init() must have already been called.

  @param device
    RFM device
  
  @param rxagc_type
    Raw RxAGC/RxAGC with noise floor correction/In gap RxAGC(CM)
   
  @param rxagc_unit
    RxAGC in dBm/dB10/dB12
   
  @param multi_carrier_idx
    Corresponding Carrier RxAGC (1/2/3)
  
  @retval
  Rx AGC value.
*/

int16 rfm_wcdma_get_rxagc
(
  rfcom_device_enum_type device,
  rfcom_rxagc_type rxagc_type,
  rfcom_rxagc_unit_type rxagc_unit,
  rfcom_multi_carrier_id_type multi_carrier_idx
)
{
  int16 ret_val = 0;
  rfwcdma_mc_rxagc_type rxagc;
  
  rxagc.type = rxagc_type;
  rxagc.unit = rxagc_unit;
  rxagc.carr_id = multi_carrier_idx ;
    
  ret_val = rfwcdma_mc_get_rxagc( device, &rxagc);
  
  return(ret_val);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculate WCDMA frequency from UARFCN.

  @retval
  uint32 Tx channel frequency in Khz.
*/
uint32 rfm_get_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan,                            /* input channel number */
  rfcommon_channel_ul_dl_type ul_dl
)
{
  uint32 ret_val = 0;
  rfwcdma_mc_channel_type channel;
  channel.band = band;
  channel.chan[0] = chan;
  channel.ul_dl = (rfwcdma_core_uarfcn_ul_dl_type)ul_dl;

  RFM_DEBUG_EN_Q(&rfm_get_freq_from_uarfcn, band, chan, ul_dl, 0);

  /*lint -e732 */
  RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_FREQ_FROM_UARFCN, &channel );
  return (uint32)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculate WCDMA UL frequency from DL UARFCN.

  @retval
  uint32 Tx channel frequency in Khz.
*/
uint32 rfm_get_ul_freq_from_dl_uarfcn
(
 sys_band_class_e_type sys_band,
 word ul_chan
)
{ 
  uint32 ret_val = 0;
  rfwcdma_mc_channel_type channel;
  rf_card_band_type card_band;

  card_band = rfm_convert_sys_band_to_rf_card_band(sys_band);
  /* Sanity Check */
  if(RF_MAX_BAND == card_band)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid sys_band %d", sys_band);
    return 0;
  }
  channel.band = rfwcdma_core_convert_band_rf_card_to_rfcom(card_band);
  channel.chan[0] = ul_chan;
  channel.ul_dl = RFWCDMA_CORE_UARFCN_DL;

  RFM_DEBUG_EN_Q(&rfm_get_ul_freq_from_dl_uarfcn, card_band, ul_chan, RFWCDMA_CORE_UARFCN_DL, 0);

  /*lint -e732 */
  RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_UL_FREQ_FROM_DL_UARFCN, &channel );
  return (uint32)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculate WCDMA LO frequency from UARFCN and Carrier Mode

  @retval
  uint32 LO frequency in Khz.
*/
uint32 rfm_get_lo_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan[],                            /* input channel number */
  rfcommon_channel_ul_dl_type ul_dl,
  rfcom_multi_carrier_hspa_id_type num_chan
)
{
  uint32 ret_val = 0;
  rfwcdma_mc_channel_type channel;
  channel.band = band;
  channel.chan[0] = chan[0];
  channel.chan[1] = chan[1];
  channel.ul_dl = (rfwcdma_core_uarfcn_ul_dl_type)ul_dl;

  RFM_DEBUG_EN_Q(&rfm_get_lo_freq_from_uarfcn, band, &chan, ul_dl, num_chan);
  if ((num_chan == RFCOM_SINGLE_CARRIER) || (ul_dl == RFCOMMON_CHANNEL_UL))
  {
    return (rfm_get_freq_from_uarfcn(band, chan[0],ul_dl));
  }
  else
  {
    RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_LO_FREQ_FROM_UARFCN, &channel );
    return (uint32)ret_val;
  }
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Calculate WCDMA frequency from UARFCN.

  @retval
  uint32 Tx channel frequency in Khz.
*/
uint32 rfm_get_freq_from_dl_arfcn( uint16 channel)
{

  RFM_DEBUG_EN_Q(&rfm_get_freq_from_uarfcn, channel, 0, 0, 0);

  /* legacy API, hardcode band here but will be converted from chan in MC */
  return (rfm_get_freq_from_uarfcn(RFCOM_BAND_IMT, channel,RFCOMMON_CHANNEL_DL));
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Convert WCDMA channel number to band.

  @retval
  rfcom_wcdma_band_type WCDMA band
*/
rfcom_wcdma_band_type rfm_convert_chan_to_band( uint16 channel )
{
  uint32 ret_val = 0;

  RFM_DEBUG_EN_Q(&rfm_convert_chan_to_band, channel, 0, 0, 0);

  /*lint -e732 */
  RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_CONVERT_CHAN_TO_BAND, &channel );
  return (rfcom_wcdma_band_type)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  Get HDET buffered TxAGC value in dB10 AGC units

  @retval
  HDET buffered TxAGC value db10 AGC units

*/
int16 rfm_get_hdet_tx_agc_value( void )
{
  uint32 ret_val = 0;

  /*lint -e732 */
  RFWCDMA_MC(cmd_dispatch, ret_val)( RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GET_HDET_TX_AGC_VALUE, 0 );
  return (int16)ret_val;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Tunes the UHF Rx/Tx synthesizers to the channel specified.

  @details
  Tunes the UHF Rx/Tx synthesizers to the channel specified.  The channel
  is interpreted based on operating mode.  The input parameter synths
  specifies whether transmit, receive, or both synthesizers should be
  tuned to the specified channel.

  @param device : transceiver to be tuned.
  @param channel : Mode dependent channel
  @param synths : ynthesizor(s) to tune
  @param user_data_ptr : Pointer to user data passed into callback
  @param cb_handler : Call back handler
  @param num_chan : Carrier type
  @param rxlm_buf_idx: RxLM buffer index 

  @retval Returns total delay in microseconds this function will wait for
  synthesizers to lock.

  @pre
  rfm_init() must have already been called.
  rfm_enter_mode() must have already been called.
  Get HDET buffered TxAGC value in dB10 AGC units

===========================================================================*/

uint32 rfm_tune_to_multi_chan ( rfcom_device_enum_type device, uint16 channel[],
                          rfcom_multi_carrier_hspa_id_type num_chan,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler
                          ,uint32 rxlm_buf_idx
                          )
{
  uint32 ret_val = 0;

  //RFM_DEBUG_EN_Q(&rfm_tune_to_multi_chan, 0, channel, synths, &user_data_ptr);

  if (num_chan == RFCOM_SINGLE_CARRIER)
  {
  RFWCDMA_MC(tune_to_multi_chan, ret_val)(device, 
                                          channel, 
                                          RFWCDMA_MC_SC, 
                                          user_data_ptr, 
                                          (rfm_cb_handler_type) cb_handler
                                          ,rxlm_buf_idx
                                          );
  }
  else if (num_chan == RFCOM_DUAL_CARRIER)
  {
    RFWCDMA_MC(tune_to_multi_chan, ret_val)(device, 
                                            channel, 
                                            RFWCDMA_MC_DC, 
                                            user_data_ptr, 
                                            (rfm_cb_handler_type) cb_handler
                                            ,rxlm_buf_idx
                                            );
  }
  else if (num_chan == RFCOM_3_CARRIER)
  {
    RFWCDMA_MC(tune_to_multi_chan, ret_val)(device, 
                                            channel, 
                                            RFWCDMA_MC_3C, 
                                            user_data_ptr, 
                                            (rfm_cb_handler_type) cb_handler
                                            ,rxlm_buf_idx
                                            );
  }

  return ret_val;
}

uint32 rfm_tune_to_multi_chan_v2(rfm_device_enum_type device, uint16 channel[],
                          uint32 multi_carrier_idx,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler
                          ,uint32 rxlm_buf_idx
                          )
{
  uint32 ret_val = 0;

 
  RFWCDMA_MC(tune_to_multi_chan, ret_val)(device, 
                                          channel, 
                                          multi_carrier_idx, 
                                          user_data_ptr, 
                                          (rfm_cb_handler_type) cb_handler
                                          ,rxlm_buf_idx
                                          );

  return ret_val;
}

/*===========================================================================*/
boolean rfm_write_tech_dependent_dynamic_update(rfcom_tech_type curr_tech,uint8 rxlm_buf_idx[],rfcom_device_enum_type device)
{
   boolean ret_val = TRUE;

  if (curr_tech ==  RFCOM_HS_MODE)
  {
    RFWCDMA_MC(write_tech_dependent_dynamic_update, ret_val)(curr_tech, 
                                          rxlm_buf_idx, 
                                          device 
                                          );
  }
  else
  {
    RFWCDMA_MC(write_tech_dependent_dynamic_update, ret_val)(curr_tech, 
                                            rxlm_buf_idx, 
                                            device 
                                            );
                                         
  }

  return ret_val;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  Enable gp_clk.

  @details

  @param
*/
void rfm_gp_clock_on()
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GP_CLOCK_ON, 0);
}
  
/*----------------------------------------------------------------------------*/
/*
  @brief
  Disable GP clock.

  @details
 
  @param
*/
void rfm_gp_clock_off()
{
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, (int32)RFWCDMA_MC_GP_CLOCK_OFF, 0);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update rxlm buffer

  @details
  Shall be used in IRAT/IREQ where L1 calls RF to update RxLM buffer for neighbor cells.

  @param device : transceiver to be tuned.
  @param channel : Mode dependent channel
  @param num_chan : Carrier type
  @param rxlm_buf_idx: RxLM buffer index

  @retval True if update is succesful. False if update failed.

===========================================================================*/
boolean rfm_update_rxlm_buffer( rfcom_device_enum_type device, 
                                uint16 channel[],
                                rfcom_multi_carrier_hspa_id_type num_chan,
                                uint32 rxlm_buf_idx)
{
  int32 ret_val = 0;
  rfwcdma_mc_rxlm_param_type rxlm_param;
  rxlm_param.channel[0] = channel[0];
  rxlm_param.channel[1] = channel[1];
  rxlm_param.num_chan = num_chan;
  rxlm_param.rxlm_buf_idx = rxlm_buf_idx;
 
  RFWCDMA_MC(cmd_dispatch, ret_val)( 
                                    device, 
                                    RFWCDMA_MC_UPDATE_RXLM_BUFFER,
                                    &rxlm_param                   
                                   );

  return (boolean)ret_val; 
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx before rfm_enable_tx( ) is called.

  @details
  It initializes Tx parameter table, mDSP variables, and MODEM Tx registers.
  This function should be called before rfm_enable_tx( ) is called.

===========================================================================*/

 boolean rfm_init_wcdma_tx(uint32 txlm_buf_idx,uint32 multi_carrier_idx,rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfg)
{
  boolean ret_val;
  RFWCDMA_MC(init_tx,ret_val)(RFM_DEVICE_0,txlm_buf_idx,multi_carrier_idx,dc_hsupa_cfg);
  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepares for Tx after rfm_wakeup( ) is called.

  @details
  It initializes MODEM Tx registers.
  This function should be called before rfm_enable_tx( ) is called.

===========================================================================*/

void rfm_wcdma_wakeup_tx(void)
{
  RFWCDMA_MC_V(wakeup_tx)();
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function freezes/unfreezes the RX accumulator.
 
  @param device : transceiver to freeze/unfreeze
  @param freeze : flag to specify freeze/unfreeze
===========================================================================*/
void rfm_set_rx_freeze_accum( rfcom_device_enum_type device, boolean freeze )
{
  RFWCDMA_MC_V(cmd_dispatch)( device, 
                              RFWCDMA_MC_SET_RX_FREEZE_ACCUM,
                              &freeze );
}
/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This API enables/disables RF lower power mode. I will update RxLM/RxFE to low
  power/high power mode settings, re-start AGC and put RFIC in lower/high power mode
  if applicable.
 
  @param en : flag to specify lower/high power mode
  @param c0_rxlm_buf_idx: chain 0 RxLM buffer index
  @param c1_rxlm_buf_idx: chain 1 rxlm buffer index
===========================================================================*/
boolean rfm_set_low_power_mode(rfcom_lpm_config_enum_type lpm_config,
                               uint32 c0_rxlm_buf_idx,
                               uint32 c1_rxlm_buf_idx)
{
  rfwcdma_mc_lpm_param_type lpm_param;
  int32 ret_val = 0;
  lpm_param.lpm_config = lpm_config;
  lpm_param.c0_rxlm_buf_idx = c0_rxlm_buf_idx;
  lpm_param.c1_rxlm_buf_idx = c1_rxlm_buf_idx;

  RFWCDMA_MC(cmd_dispatch,ret_val)(RFCOM_TRANSCEIVER_0,
                                   RFWCDMA_MC_SET_LOWER_POWER_MODE,
                                   &lpm_param);
  return (boolean)ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This APT returns the PA state
 
  @details
  This APT returns the PA state

===========================================================================*/
uint8 rfm_get_pa_state( void )
{
  int32 pa_state = 0;
  RFWCDMA_MC(cmd_dispatch,pa_state)(RFCOM_TRANSCEIVER_0, RFWCDMA_MC_GET_PA_STATE, 0);
  return (uint8)pa_state;
}
/*!
  @brief
  Called before disabling Div chain through call to rfm_sleep(div):Enable/Disable CPC register.

  @details
     L1 can register CPC-DRX gap status with RF before disabling Div chain through call to rfm_sleep(div)                                                                                   .
     RF can then issue Stop-AGC to FW if even if AGC is already stopped due to CPC-DRX. 
     RF will not send Stop-AGC if AGC is already disabled for any other reason. 
 
*/
void rfm_register_cpc_drx_state(boolean enable){
  RFWCDMA_MC_V(cmd_dispatch)(RFCOM_TRANSCEIVER_0, RFWCDMA_MC_SET_CPC_STATE, &enable);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Regular ticks from WL1 to RF and this is a indication message. 
  
  @detail
  Using this function WL1 would provide a 200ms regular pulses/ticks to RFSW.
  When ever these indication message is received from L1, RFSW would appropriately
  apply dynamic antenna tuner updates through FW.
  RFSW would keep track of all pending scnearios buffer and if none, it is a
  no-op for RFSW during this function call.
 
  Note : With this function call, RFSW will NULL out any call back function registered 
  by WL1 to inform any scneario updates.

  @param 
  
  @return
  void
*/
void rfm_wcdma_antenna_tuner_tick(void)
{
 #ifdef FEATURE_RF_HAS_QTUNER
  // RF_MSG( RF_LOW, "rfm_wcdma_antenna_tuner_tick : tick received");
  (void)rfwcdma_core_antenna_tuner_incremental_update();
 #endif
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function sets the wakeup optimization flag.  If true, an SSBI script
  is created and sent to FW to program device wakeup through HW TQ. If false,
  RFSW directly programs device for wakeup.
 
  @param device : transeiver
  @param enable_optimization : flag to enable/disable wakeup optimization
===========================================================================*/
void rfm_set_wakeup_optimization( boolean enable_optimization )
{
  RFWCDMA_MC_V(cmd_dispatch)( RFM_DEVICE_0, 
                              RFWCDMA_MC_SET_WAKEUP_OPTIMIZATION,
                              &enable_optimization );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deconfig setting in rfm_init_wcdma_tx( ).

  @details
  It deconfigs Tx setting made in rfm_init_wcdma_tx().
  This function should be called before WL1 send txlm_cfg(disable) to FW.

===========================================================================*/

boolean rfm_wcdma_decfg_tx(void)
{
  boolean ret_val;
  RFWCDMA_MC(decfg_tx,ret_val)(RFM_DEVICE_0);
  return ret_val;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get default band and DL chan
  This will have to return the default band and DL chan that both RF and WL1 can stick to. 

  @details
  It returns the default band and DL chan that both RF and WL1 can stick to.
  default band is not necessarily band 1.

===========================================================================*/

void rfm_wcdma_get_default_band_chan(rfi_band_type *band, uint16 *chan)
{
   RFWCDMA_MC_V(get_default_rfi_band_and_chan)(band,chan);
}


rfi_band_type rfm_wcdma_convert_band_rfcom_to_rfi
(
  rfcom_wcdma_band_type band
)
{
  return rfwcdma_core_convert_band_rfcom_to_rfi(band);
}

/*===========================================================================*/
/*!
   @name Dual Band Dual carrier

    @brief
     This block will contain the functions related to the implementation of Dual band dual carrier feature.
 */ 

/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get DBDC band combinations supported

  @details
  This will have to return the number of DBDC combinations supported on the RFC and those band combos.
  For RFCs which does not support any DBDC bands , by deafult it will be zero.

  @param num_band_comb_supported
  This will return the total number of DBDC band combinations supported by the RFC

  @param band_comb_list
  This would give the DB-DC band combinations as to which band is supported on which carrier

  @return
   Flag indicating the validity of the DBDC band combinations
  
*/

boolean rfm_wcdma_get_dbdc_band_support( uint16 *num_band_comb_supported, const rfm_wcdma_dual_band_type** band_comb_list)
{
  boolean ret_val;
  
  RFWCDMA_MC(get_dbdc_band_support,ret_val)( num_band_comb_supported, band_comb_list );

  return ret_val;

} /* rfm_get_dbdc_band_support */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the W2W neighbor physical device index to support DBDC

  @details
  This function will return the physical device index based on the neighbor ARFCN and rfm device
  that can be mapped to the device index in the LNA/LPM FW interface used for neighbor tuning
 
  Example
  DBDC Primary band: B1 -> LNA/LPM device 0
  DBDC Secondary band: B8 -> LNA/LPM device 1
  Neighbor Band: B5 -> B5 will be tune on the same physical device as B8, return index 1
  Neighbor Band: B2 -> B2 will be tune on the same physical device as B1, return index 0
 

  @param chan
  neighbor cell ARFCN

  @param device
  neighbor cell RFM device
 
  @return
   physical device index for the home/neighbor pair
   return 0xFF if no valid device index can be found
*/

uint8 rfm_wcdma_get_nbr_phy_dev_ind(uint16 chan, rfm_device_enum_type device)
{
  uint8 ret_val;
  
  RFWCDMA_MC(get_nbr_phy_dev_ind,ret_val)(chan, device);

  return ret_val;
} /* rfm_wcdma_get_nbr_phy_dev_ind */

/*
  @brief
This API will return TRUE if atleast one of the WCDMA bands are supported 
 
  @details

  @param 

*/


boolean rfm_is_wcdma_supported(void)
{
  boolean ret_val = FALSE;
	
  RFWCDMA_MC(is_wcdma_supported,ret_val)();
  
  return ret_val;
}


/*! @} */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API is deprecated. Use rfm_wcdma_rx_prep_wakeup.
*/
rfm_api_status_t
rfm_wcdma_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  RF_MSG_2( RF_ERROR, "rfm_wcdma_prep_wakeup_rx: Deprecated API called, Dev: %d, " 
  	                  "RxLM: %d", device, rxlm_handle);
  return 0;

} /* rfm_wcdma_prep_wakeup_rx */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to perfrom Tx initialization

  @details
  Prepares the software to move the WCDMA reciever from the Rx state to the 
  RxTx state. This function will perform Tx linearizer loading to FW
 
  @param device
  The transmit path on which Tx initialization is to be prep'ed.

  @param txlm_handle
  The Tx Link Manager buffer that shall be used to configure the
  modem hardware.
 
  @param carrier_mask
  Mask indicating which Tx carriers are to be enabled.
 
  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_WCDMA_RX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
  
  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_prep_init_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const uint32 carrier_mask,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val =   RFM_WCDMA_HEALTHY_STATE;

#if 0
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_prep_init_tx, device, txlm_handle, &callback, 0 );

  /* Throw F3 Message */
  RF_MSG_3( RF_HIGH, "rfm_wcdma_prep_init_tx : START - Dev %d, "
            "TxLM Handle %d, MultiCarrIndex 0x%x", 
            device, 
            txlm_handle,
            carrier_mask );

  rfwcdma_core_txplim_load_mpr_backoff(carrier_mask);
  
  ret_val = rfwcdma_mc_prep_init_tx(device, txlm_handle, carrier_mask);

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
  
  /* Debug Message based on return value */
  if (ret_val != RFM_CDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_5( RF_ERROR, "rfm_wcdma_prep_init_tx: [FAILURE %d] - Dev %d, "
              "TxLM %d, CarrierMask 0x%x, PrepTxTime %d uS ", 
              ret_val, 
              device,
              txlm_handle,
              carrier_mask,
              profile_time );
  } /* if (ret_val != RFM_CDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_5( RF_HIGH, "rfm_wcdma_prep_init_tx: [Status %d] - Dev %d, "
              "TxLM %d, CarrierMask 0x%x, PrepTxTime %d uS", 
              ret_val, 
              device,
              txlm_handle,
              carrier_mask,
              profile_time );
  } /* if ! (ret_val != RFM_CDMA_HEALTHY_STATE) */

  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

#endif
  /* Return the API Wait time */
  return ret_val;


} /* rfm_wcdma_prep_init_tx */
/*!
  @brief
  Return RF Tx bandwidth for uplink multi-carrier/single-carrier fast switching

  @details
  This function will reprogram Tx devices for Tx bandwith change, Tx has to
  be already enabled (rfm_init_wcdma_tx and rfm_enable_tx called). It can be
  called for fast bandwidth switching.
 
  Scenarios                                RF API Call flows
  Bringup Tx in SC mode                    rfm_init_wcdma_tx(SC)+rfm_enable_tx()
  Configure/Activate DC from SC            rfm_init_wcdma_tx(DC)+rfm_wcdma_tx_retune_bw(DC)
  Deactivate DC with DC configured         rfm_wcdma_rx_retune_bw(SC)
  Re-activate DC with DC configured        rfm_wcdma_rx_retun_bw(DC)
  Deconfigure DC after deactivation        rfm_init_wcdma_tx(SC)       
 
  @param tx_retune_bw_params
  Input parameters for rfm_wcdma_tx_retune_bw API

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_TX_RETUNE_BW_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_tx_retune_bw
(
  const rfm_wcdma_tx_retune_bw_param* const tx_retune_bw_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */

   /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_tx_retune_bw, 
                  tx_retune_bw_params->device, 
                  tx_retune_bw_params->multi_carrier_idx, 
                  &callback,
                  0 );

  /* Throw F3 Message */
  RF_MSG_2( RF_HIGH, "rfm_wcdma_tx_retune_bw : START - Dev %d, "
            "Multi Carrier Idx %d", tx_retune_bw_params->device, tx_retune_bw_params->multi_carrier_idx );


  /* Proceed only if State is valid, i.e. ret_val is non-negative  */
  if (( rfwcdma_mc_validate_state( (1<<tx_retune_bw_params->device), RFWCDMA_MC_STATE_RXTX ) == RFM_WCDMA_HEALTHY_STATE )||
     ( rfwcdma_mc_validate_state( (1<<tx_retune_bw_params->device), RFWCDMA_MC_STATE_RX ) == RFM_WCDMA_HEALTHY_STATE ))
  {
    ret_val = rfwcdma_mc_tx_retune_bw(tx_retune_bw_params->device, tx_retune_bw_params->multi_carrier_idx);
  } /* if ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_BAD_STATE;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_4( RF_ERROR, "rfm_wcdma_tx_retune_bw: [FAILURE %d] - Dev %d, "
              "RxLM %d, [OPT %d], API Time %d uS ", 
              ret_val, 
              tx_retune_bw_params->device,
              tx_retune_bw_params->multi_carrier_idx,
              profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_4( RF_HIGH, "rfm_wcdma_tx_retune_bw: [STATUS %d] - Dev %d, "
              "RxLM %d, [OPT %d], API Time %d uS ", 
              ret_val, 
              tx_retune_bw_params->device,
              tx_retune_bw_params->multi_carrier_idx,
              profile_time );

  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */

 /* Return the API Wait time */
  return ret_val;
}/* rfm_wcdma_tx_retune_bw */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare to turn on the Rx radio on a aparticular device and tune to the active band and channel.

  @details
  Prepares the software to move the WCDMA reciever from the Sleep state to the 
  Rx state on a particular device. This function will prepare scripts to turn on the receiver.
  The scripts will be created for the band and channel which was
  tuned to earlier during tuning operation or pre-tuning operation. The
  function will not perform any operation related to direct access to the
  hardware, as it is not guaranteed that all the necessary clock are enabled
  while calling this function.
 
  @param rx_wakeup_params
  Pointer to the structure that holds the parameters passed by L1 to wakeup WCDMA Rx.
  
  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_PREP_WAKEUP_RX_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_rx_prep_wakeup
(
  const rfm_wcdma_rx_wakeup_params *const rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rfm_api_status_t device_state; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  int32 device =0, device_mask =0;
  uint32 rx_handle_pool[RFM_WCDMA_MAX_DEVICE] = {0};
  boolean device_valid = TRUE; 
  

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_rx_prep_wakeup : START" );

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_rx_prep_wakeup,
                    &callback,0,0,0 );

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
   {
      if (rx_wakeup_params -> api_params[device].is_valid == TRUE)
      {
        /* Prepare device mask and rx_handle_pool for saving overhead in mc calls  */
        device_mask |= 1<< device;
        rx_handle_pool[device] = rx_wakeup_params -> api_params[device].rxlm_handle;
       }
   }

   /* Validate the Current State */
   device_state = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_SLEEP );
  
   if (device_state != RFM_WCDMA_HEALTHY_STATE)
   {
      RF_MSG_1( RF_HIGH, "rfm_wcdma_rx_prep_wakeup : Dev %d, not all SLEEP state",device_mask);
   }
   
   /* Proceed only if State is valid for all the passed devices  */
   if (device_state == RFM_WCDMA_HEALTHY_STATE)
   {
       /* Prepare script only if FR2001 is enabled */
       if ( enable_wakeup_optimization_flag == TRUE )
       {
         /* Call WCDMA Main Control to Prepare Rx for Wakeup */
          ret_val = rfwcdma_mc_rx_prep_wakeup( device_mask, rx_handle_pool );
       } /* if ( enable_wakeup_optimization_flag == TRUE ) */

       else
       {
         RF_MSG( RF_ERROR, "rfm_wcdma_rx_prep_wakeup: FR2001 is not enabled, thus skipping");
       } /* if ! ( enable_wakeup_optimization_flag == TRUE ) */
      
      
       /* Based on Status of MC execution, set CB Event Flag */
       if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
       {
         /* Flag Callback event as successful */
         cb_event = RFM_WCDMA_PREP_WAKEUP_RX_COMPLETE;   
       } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */
      
   } /* if ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
   else
   {
     /* assign the current device state to return value of API */
     ret_val = device_valid;
   } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
   
   /* Execute Callback */
   if ( callback != NULL )
   {
     callback( cb_event, userdata );
   } /* if ( callback != NULL ) */
   
   /* get time spent in API */
   profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
   /* Debug Message based on return value */
   if (ret_val != RFM_WCDMA_HEALTHY_STATE)
   {
     /* Debug message */
     RF_MSG_4( RF_ERROR, "rfm_wcdma_rx_prep_wakeup: [FAILURE %d] -[OPT %d]," 
               "DevMask 0x%x PrepWakeupTime %d uS", 
               ret_val, 
               enable_wakeup_optimization_flag,
               device_mask,
               profile_time );
   } /* if (ret_val != RFM_CDMA_HEALTHY_STATE) */
   else
   {
     /* Debug message */
     RF_MSG_4( RF_HIGH, "rfm_wcdma_rx_prep_wakeup: [Status %d] - [OPT %d], "
               "DevMask 0x%x, PrepWakeupTime %d uS", 
               ret_val, 
               enable_wakeup_optimization_flag,
               device_mask,
               profile_time );
   } /* if ! (ret_val != RFM_CDMA_HEALTHY_STATE) */
   
   /* Acquire Critical Section Lock */
   rf_common_leave_critical_section(rfwcdma_get_lock_data());
   
   
   /* Return the API Wait time */
   return ret_val;
  
}/* rfm_wcdma_rx_prep_wakeup*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Wakeup Rx sequence to turn on the radio on a particular device 
  and tune to the given band and channel.

  @details
  This function will send a command to firmware to indicate that Rx needs to be
  started . The command will indicate FW to activate the Script.
 
  @param rx_wakeup_params
  Pointer to the structure that holds the parameters passed by L1 to wakeup WCDMA Rx.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_rx_exec_wakeup
( const rfm_wcdma_rx_wakeup_params *const rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rfm_api_status_t device_state, device_state_2; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  uint8 device_mask = 0, device =0; 
  uint32 rx_handle_pool[RFM_WCDMA_MAX_DEVICE] = {0};
 

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_rx_exec_wakeup,
                    &callback,0,0,0 );

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_rx_exec_wakeup : START");

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
  {
     if (rx_wakeup_params->api_params[device].is_valid == TRUE)
      {
        device_mask |= 1<< device;
        rx_handle_pool[device] = rx_wakeup_params->api_params[device].rxlm_handle; 
      }
  }
  
  /* If the current state is Rx, then just enable RxAGC based on RxAGC state */
  device_state = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX );

  if ( device_state == RFM_WCDMA_HEALTHY_STATE )
  {
    /* 
    RF is not in sleep state, only send start AGC if needed. Following 
    conditions need to be met to send start AGC command to avoid sending 
    back to back AGC start command
    1. FW RxAGC ON status flag need to be OFF
    2. RF internal RxAGC enable flag need to be FALSE 
    */
    ret_val = rfwcdma_mc_enable_rxagc( device_mask, rx_handle_pool );
    RF_MSG_1( RF_HIGH, "rfm_wcdma_exec_wakeup_rx: Dev %d, "
              "Enable Rx from nonSLEEP state", 
              device);
              
  } /* if ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
  else
  {   
  
    /* When FR2001 is disabled, this function can only be called from
      SLEEP state. Thus for FR2001 disabled case, validate the RF State with
      RFWCDMA_MC_STATE_SLEEP */
    
    if ( enable_wakeup_optimization_flag == FALSE )
    { 
       /* Validate the Current State */
       device_state = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_SLEEP );
        
       if (device_state == RFM_WCDMA_HEALTHY_STATE )
       {  
          
          for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
          {
            if (rx_wakeup_params -> api_params[device].is_valid == TRUE)
            {
              ret_val = rfwcdma_mc_exec_wakeup_rx_imm( device, rx_handle_pool[device]);
            }/* if (rx_wakeup_params -> api_params[device].is_valid == TRUE) */
          } 
       
          if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
          {
            /* Flag Callback event as successful */
            cb_event = RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE;   
          } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */
      
       }
        
     }/* if(enable_wakeup_optimization_flag == FALSE)*/
    
     else
     {
         /* When FR2001 is enabled, this function can only be called from
         PREP state. Thus for FR2001 enabled case, validate the RF State with
         RFWCDMA_MC_STATE_RX_PREP */
        
         /* Validate the Current State */
    
         device_state = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX_PREP );
         device_state_2 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX_PREP_TUNE );
            
         /* Proceed only if State is valid, i.e. ret_val is non-negative  */
         if ( device_state == RFM_WCDMA_HEALTHY_STATE || device_state_2 == RFM_WCDMA_HEALTHY_STATE)
         {
        
           /* Call WCDMA Main Control to Prepare Rx for Wakeup */
           ret_val = rfwcdma_mc_rx_exec_wakeup( device_mask, rx_handle_pool);
    
           /* Based on Status of MC execution, set CB Event Flag */
           if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
           {
             /* Flag Callback event as successful */
             cb_event = RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE;   
           } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */
        
         } /* if ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
         else
         {
           /* assign the current device state to return value of API */
           ret_val = device_state;
         } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
        
     } /* if ! ( enable_wakeup_optimization_flag == FALSE ) */

  }/* if !( device_state == RFM_WCDMA_HEALTHY_STATE ) */

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_4( RF_ERROR, "rfm_wcdma_rx_exec_wakeup: [FAILURE %d] - DevMask 0x%x, "
              "[OPT %d], ExecWakeupTime %d uS ", 
              ret_val, 
              device_mask,
              enable_wakeup_optimization_flag,
              profile_time );
  } /* if (ret_val != RFM_CDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_4( RF_HIGH, "rfm_wcdma_rx_exec_wakeup: [Status %d] - DevMask 0x%x, "
              "RxLM %d, [OPT %d], ExecWakeupTime %d uS", 
              ret_val, 
              device_mask,
              enable_wakeup_optimization_flag,
              profile_time );
  } /* if ! (ret_val != RFM_CDMA_HEALTHY_STATE) */

  /* Return the API Wait time */
  return ret_val;

}/* rfm_wcdma_rx_exec_wakeup*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Turn off the receiver.

  @details
  Moves the WCDMA radio from the Rx state to the Sleep state.  Any circuits
  that are no longer needed are powered down to conserve current.

  This API is asynchronous, and the radio cannot be considered stable
  until the user callback function has been called with the
  RFM_WCDMA_RX_SLEEP_COMPLETE event.  

  @param rx_sleep_params
  Pointer to the structure that holds the parameters passed by L1 to wakeup WCDMA Rx.

  @param callback
  A function pointer that will be called when the entire sleep sequence
  is complete.  This function will be passed the RFM_WCDMA_RX_SLEEP_COMPLETE
  event when all actions associated with this API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_rx_sleep
(
  const rfm_wcdma_rx_sleep_params *const rx_sleep_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rfm_api_status_t device_state_1; /* return value of device_state check */
  rfm_api_status_t device_state_2; /* return value of device_state check */
  rfm_api_status_t device_state_3; /* return value of device_state check */
  rfm_api_status_t device_state_4; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  uint8 device_mask = 0; 
  rfm_device_enum_type device =0;
  uint32 rx_handle_pool[RFM_WCDMA_MAX_DEVICE] = {0};
  boolean skip_agc_disable[RFM_WCDMA_MAX_DEVICE] = {FALSE};
  

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
   {
      if (rx_sleep_params -> api_params[device].is_valid == TRUE)
      {
        /* Prepare device mask and rx_handle_pool for saving overhead in mc calls  */
        device_mask |= 1<< device;
        rx_handle_pool[device] = rx_sleep_params -> api_params[device].rxlm_handle;
        skip_agc_disable[device] = rx_sleep_params -> api_params[device].skip_agc_disable; 
       }
   }
  
  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_rx_sleep, device_mask, &callback,0,0);

  /* Throw F3 Message */
  RF_MSG_1( RF_HIGH, "rfm_wcdma_rx_sleep : START - Device mask %d ",device_mask );

  /* Validate the Current State */
  device_state_1 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX_PREP );
  device_state_2 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX );
  device_state_3 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_ENTER_MODE );

  // Interim solution for DBDC case. Cannot be mainlined
  device_state_4 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RXTX );

  /* Proceed only if State is valid, i.e. ret_val is non-negative  */
  if ( ( device_state_1 == RFM_WCDMA_HEALTHY_STATE ) ||
       ( device_state_2 == RFM_WCDMA_HEALTHY_STATE ) ||
       ( device_state_3 == RFM_WCDMA_HEALTHY_STATE ) ||
       ( device_state_4 == RFM_WCDMA_HEALTHY_STATE ))

  {
    /* Call WCDMA Main Control to Prepare Rx for Sleep */
    ret_val = rfwcdma_mc_rx_sleep( device_mask, skip_agc_disable, rx_handle_pool );

    /* Based on Status of MC execution, set CB Event Flag */
    if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
    {
      /* Flag Callback event as successful */
      cb_event = RFM_WCDMA_SLEEP_RX_COMPLETE;   
    } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */

  } /* if state is allowed*/
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_FAILURE;

  } /*state check fails*/ 

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_rx_sleep: [FAILURE %d] - Dev %d, "
              "SleepTime %d uS ", 
              ret_val, 
              device_mask,
              profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_rx_sleep: [Status %d] - Dev %d, "
              "SleepTime %d uS", 
              ret_val, 
              device_mask,
              profile_time );
  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */

  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API Wait time */
  return ret_val;

} /* rfm_wcdma_rx_sleep */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare Rx scripts to tune the radio to requested channel(s).

  @details
  Prepares scripts for Rx Tune on the provided set of devices.The scripts will be created for 
  the band and channel which was requested. The
  function will not perform any operation related to direct access to the
  hardware, as it is not guaranteed that all the necessary clock are enabled
  while calling this function.
 
  @param rx_tune_params
  Pointer to the structure that holds the tune parameters passed by L1 to wakeup WCDMA Rx.
  
  @param callback
  A function pointer that will be called when the entire tune sequence
  is complete.  This function will be passed with
  RFM_WCDMA_RX_PREP_TUNE_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_rx_prep_tune
(
  rfm_wcdma_rx_tune_params * rx_tune_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t device_state = RFM_WCDMA_ERROR_BAD_STATE; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  int32 device =0, device_mask =0;
  boolean device_valid = TRUE; 
  rfwcdma_data_debug_flags_type *dbg_flags;

  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */

  /* return value of device_state check */
  rfm_api_status_t device_state_1; 
  rfm_api_status_t device_state_2; 
  rfm_api_status_t device_state_3; 
  rfm_api_status_t device_state_4; 
  
  dbg_flags = rfwcdma_get_debug_flags();

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_rx_prep_tune : START" );

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_rx_prep_tune,
                    &callback,0,0,0 );

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
  {
     if (rx_tune_params -> api_params[device].is_valid == TRUE)
     {
       /* Prepare device mask to save loops in mc routines  */
       device_mask |= 1<< device;
      }
  }

  /* Validate the Current State */

  /*Prep tune (script based tune) cannot be called if Tx is ON. In that case we 
    noop prep and call legacy tune_to_multi_chan from exec_tune */

  device_state_1 = rfwcdma_mc_validate_state( Tx_device, RFWCDMA_MC_STATE_RXTX );

  if ((dbg_flags->enable_script_based_tune == FALSE) && (device_state_1 == RFM_WCDMA_HEALTHY_STATE))
  {
    RF_MSG_1( RF_HIGH, "rfm_wcdma_rx_prep_tune : Dev %d, Skipping as Tx is ON",device_mask);
    rf_common_leave_critical_section(rfwcdma_get_lock_data());
    
    return RFM_WCDMA_ERROR_INVALID_MODE; 
  }

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
  {
    if (rx_tune_params -> api_params[device].is_valid == TRUE)
    {
      if ( (rf_count_one_bits(rx_tune_params -> api_params[device].carrier_mask))!= 1)
      {
        RF_MSG_1( RF_HIGH, "rfm_wcdma_rx_prep_tune : Dev %d, Prep Tune not supported for DC",device);
        rf_common_leave_critical_section(rfwcdma_get_lock_data());
        return RFM_WCDMA_ERROR_INVALID_MODE; 
      }
     }
  }

  /* Prep_tune can however be called from sleep (idle tune), enter mode (first time tune)
     Rx (tune to different channel in same band or different band) */

  device_state_2 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_SLEEP );
  device_state_3 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_ENTER_MODE );
  device_state_4 = rfwcdma_mc_validate_state( device_mask, RFWCDMA_MC_STATE_RX );
 

  if ((device_state_1 == RFM_WCDMA_HEALTHY_STATE) || 
      (device_state_2 == RFM_WCDMA_HEALTHY_STATE) || 
      (device_state_3 == RFM_WCDMA_HEALTHY_STATE) ||
      (device_state_4 == RFM_WCDMA_HEALTHY_STATE) )
  {
     device_state = RFM_WCDMA_HEALTHY_STATE; 
  }
  
  if (device_state != RFM_WCDMA_HEALTHY_STATE)
  {
     RF_MSG_1( RF_HIGH, "rfm_wcdma_rx_prep_tune : Dev %d, not in allowed state",device_mask);
  }

  /* Proceed only if State is valid for all the passed devices  */
  if (device_state == RFM_WCDMA_HEALTHY_STATE)
  {
      /* Call WCDMA Main Control to Prepare Rx for Wakeup */
      ret_val = rfwcdma_mc_rx_prep_tune( device_mask, rx_tune_params );
  }
      
    /* Based on Status of MC execution, set CB Event Flag */
    if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
    {
      /* Flag Callback event as successful */
      cb_event = RFM_WCDMA_PREP_WAKEUP_RX_COMPLETE;   
    } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */
      
  else
  {
    /* assign the current device state to return value of API */
    ret_val = device_valid;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

   /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
   
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_rx_prep_tune: [FAILURE %d] -," 
              "DevMask 0x%x PrepTuneTime %d uS", 
              ret_val, 
              device_mask,
              profile_time );
  } /* if (ret_val != RFM_CDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_rx_prep_tune: [Status %d] -, "
              "DevMask 0x%x, PrepTuneTime %d uS", 
               ret_val, 
               device_mask,
               profile_time );
  } /* if ! (ret_val != RFM_CDMA_HEALTHY_STATE) */
   
  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API Wait time */
  return ret_val;

}/* rfm_wcdma_rx_prep_tune*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Rx Tune sequence to to stop and restart AGC on a particular device 
  and tune to the given band and channel.

  @details
  This function will send a command to firmware to indicate that Rx needs to be
  started . The command will indicate FW to activate the Script.
 
  @param rx_tune_params
  Pointer to the structure that holds the parameters passed by L1 to wakeup WCDMA Rx.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_RX_EXEC_TUNE_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_rx_exec_tune
( rfm_wcdma_rx_tune_params *rx_tune_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t ret_val = RFM_WCDMA_ERROR_FAILURE; /* Return value */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  uint8 device_mask = 0, device =0, num_carriers =0; 
  uint32 rx_handle_pool[RFM_WCDMA_MAX_DEVICE] = {0};
  uint8 carrier_mask[RFM_WCDMA_MAX_DEVICE] = {0}; 
  
  rfwcdma_data_debug_flags_type *dbg_flags;
  boolean device_valid_state_1 = FALSE, device_valid_state_2 = FALSE, sc_enabled = TRUE, script_based_tune= TRUE; 
  uint8 path =0, carrier0 =0, carrier_index=0; 


  /* return value of device_state check */
  rfm_api_status_t device_state_1; 
 
  dbg_flags = rfwcdma_get_debug_flags();
  
  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_rx_exec_tune,&callback,0,0,0 );

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_rx_exec_tune : START");

  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
  {
     if (rx_tune_params->api_params[device].is_valid == TRUE)
     {
       device_mask |= 1<< device;
       rx_handle_pool[device] = rx_tune_params->api_params[device].rxlm_handle; 
       carrier_mask[device] = rx_tune_params -> api_params[device].carrier_mask; 
       num_carriers = rf_count_one_bits(rx_tune_params -> api_params[device].carrier_mask);

       if (num_carriers  >  1)
       {
         sc_enabled = FALSE; 
       }
       else 
       {
         sc_enabled &= TRUE; 
       }
     }
  }
  
  /* Tune to multi_chan is called if 
     1. Dual carrier mode is enabled
     2. Tx is ON 
     3. Prep_tune was skipped  */
    
  device_state_1 = rfwcdma_mc_validate_state( Tx_device, RFWCDMA_MC_STATE_RXTX );

  if (((dbg_flags->enable_script_based_tune == FALSE) && (device_state_1 == RFM_WCDMA_HEALTHY_STATE))
      || (sc_enabled != TRUE))
  {  
    script_based_tune = FALSE; 
  }

  /* Validate the Current State */
  for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
  {
    if(rx_tune_params->api_params[device].is_valid == TRUE)
    {
      /* perform device to Path mapping */
      path = rfcommon_core_device_to_path(device);
      carrier0 = rf_extract_first_one_bit_mask(rx_tune_params -> api_params[device].carrier_mask);
      carrier_index = carrier0 - 1; 
      RF_MSG_3(RF_HIGH, "rfm_wcdma_rx_exec_tune [Pre_validation]: device %d "
                        "first carrier :%d, carrier_index: %d",
                         device, carrier0, carrier_index);

      if ( rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state != RFWCDMA_MC_STATE_RX_PREP_TUNE )
      {
        device_valid_state_1 |= FALSE;
      }
      else 
      { 
        device_valid_state_1 = TRUE;
      }

      if ( rfwcdma_mc_state.car_path_state[path][carrier_index].rf_state != RFWCDMA_MC_STATE_RXTX_PREP_TUNE )
      {
        device_valid_state_2 |= FALSE;
      }
      else 
      { 
        device_valid_state_2 = TRUE;
      }
    }
  }
  RF_MSG_2(RF_HIGH, "rfm_wcdma_exec_tune: State validate - RX_PREP valid: %d, RXTX_PREP valid: %d", 
                  device_valid_state_1, device_valid_state_2); 
   
  /* Proceed only if state is valid, i.e. ret_val is non-negative  */
  if ( (device_valid_state_1 == TRUE ) ||(device_valid_state_2 == TRUE ) )
  {
    script_based_tune = TRUE; 
  }
  else 
  { 
    script_based_tune = FALSE; 
  }
  if (script_based_tune)
  {
    /* Call WCDMA Main Control to Prepare Rx for Wakeup */
    ret_val = rfwcdma_mc_rx_exec_tune( device_mask, carrier_mask, rx_handle_pool);
   
    /* Based on Status of MC execution, set CB Event Flag */
    if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
    {
      /* Flag Callback event as successful */
      cb_event = RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE;   
    } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */
  }
  else
  {
    RF_MSG_4( RF_HIGH, "rfm_wcdma_rx_exec_tune : Dev %d, Using non-script based approach"
                "Script enable flag %d, Tx ON status %d, Single carrier mode : %d",device_mask, 
                dbg_flags->enable_script_based_tune,device_state_1, sc_enabled );

    for (device=0; device< RFM_WCDMA_MAX_DEVICE; device++)
    {
      if (rx_tune_params -> api_params[device].is_valid == TRUE)
      {
        ret_val = rfwcdma_mc_tune_to_multi_chan(
                           device,
                           rx_tune_params -> api_params[device].channel,
                           rx_tune_params -> api_params[device].carrier_mask,
                           NULL,
                           NULL,
                           rx_tune_params -> api_params[device].rxlm_handle
                           );
     }/* if (rx_wakeup_params -> api_params[device].is_valid == TRUE) */
    }
    if ( ret_val >= RFM_WCDMA_HEALTHY_STATE )
    {
      /* Flag Callback event as successful */
      cb_event = RFM_WCDMA_EXEC_WAKEUP_RX_COMPLETE;   
    } /* if ( ret_val >= RFM_WCDMA_HEALTHY_STATE ) */

  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */
        
  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_4( RF_ERROR, "rfm_wcdma_rx_exec_tune: [FAILURE %d] - DevMask 0x%x, "
              "[OPT %d], ExecTuneTime %d uS ", 
              ret_val, 
              device_mask,
              dbg_flags->enable_script_based_tune,
              profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_4( RF_HIGH, "rfm_wcdma_rx_exec_tune: [Status %d] - DevMask 0x%x, "
              "[OPT %d], ExecTuneTime %d uS", 
              ret_val, 
              device_mask,
              dbg_flags->enable_script_based_tune,
              profile_time );
  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */

  /* Return the API Wait time */
  return ret_val;

}/* rfm_wcdma_rx_exec_tune*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prepare Temperature Compensation Update

  @details
  This function will prepare RF for temperature compensation by performing 
  static update. This update includes the reading of current 
  temparature and applying the temperature specific compensation to Tx 
  linearizer tables.
 
  The static update will comparatively take longer time, thus the execution
  will be done under RF_APPS_TASK. At the end of the update, RF will execute the
  callback function sent by L1, which will indicate to L1 that the first part 
  temp comp is completed.
 
  The completion of this function does not ensure that temp comp has completed.
  L1 software is expected to call execute temp comp to complete the 
  update process.
 
  @param intf_param
  Structure containing the parameter to be used during temperature compensation
  update
 
  @param callback
  A function pointer that will be called when the temp comp sequence
  is complete.  This function will be passed the 
  RFM_WCDMA_PREP_TEMP_COMP_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
 
  @exectime
  -# 10 uS in "Calling" Task context (@ Q6 576 MHz)
  -# 5000 uS in RF Apps Task context (@ Q6 576 MHz)
  
  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_prep_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rf_apps_cmd_type *temp_comp_cmd;
  rfm_device_enum_type device;
  rf_path_enum_type path;
  rf_time_tick_type profile_tick; /* Profile Tick */
  rf_time_type profile_time; /* Profile Time */
  rfwcdma_temp_comp_data_type *tc_data;
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE;

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  if ( intf_param == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_wcdma_prep_temp_comp: NULL param" );
    return RFM_WCDMA_ERROR_BAD_PARAM;
  } /* if ( intf_param == NULL ) */

  /* Get Device */
  device = intf_param->device;
  path = rfcommon_core_device_to_path(device);

  /* Get tempcomp data */
  tc_data = rfwcdma_data_get_temp_comp(device);

  if ( tc_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_wcdma_prep_temp_comp: NULL Temp Comp Data - "
                        "Device %d", device );
    return RFM_WCDMA_ERROR_NULL_STATE_DATA;
  } /* if ( tc_data == NULL ) */

  /* Return if Temp comp overriden to disable mode */
  if ( tc_data->override_disable_mode == TRUE )
  {
    RF_MSG_1( RF_HIGH, "rfm_wcdma_prep_temp_comp: - TempComp Overridden to Disable mode"
                        "Device %d", device );

    /* Execute Callback */
    if ( callback != NULL )
    {
      callback(RFM_WCDMA_TEMP_COMP_COMPLETE, userdata);
    } /* if ( callback != NULL ) */

    return RFM_WCDMA_HEALTHY_STATE;
  } /* if ( tc_data->override_disable_mode == TRUE ) */

  if ( tc_data->tc_state == TEMP_COMP_READY )
  {
    if ((rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_RX) ||
        (rfwcdma_mc_state.car_path_state[path][0].rf_state == RFWCDMA_MC_STATE_RXTX))
    {
      /* Get the pointer to the free buffer of RF command queue. */
      if ( ( temp_comp_cmd = rf_apps_get_buf() ) == NULL )
      {
        RF_MSG( RF_ERROR, "rfm_wcdma_prep_temp_comp: Cmd Q is full" );
        return RFM_WCDMA_ERROR_BAD_PARAM;
      } /* if ( ( temp_comp_cmd = rf_apps_get_buf() ) == NULL ) */

      temp_comp_cmd->msgr_hdr.id = RFA_RF_WCDMA_TEMP_COMP_UPDATE_CMD;
      temp_comp_cmd->rex_hdr.cmd_hdr.tech_id = RF_APPS_WCDMA_ID;

      /* Update the state */
      tc_data->tc_state = TEMP_COMP_PENDING_EXEC;
      tc_data->callback = callback;
      tc_data->userdata = userdata;

      /* Then put command to RF Apps Task */

      /* place RFA_RF_WCDMA_TEMP_COMP_UPDATE_CMD to RF command queue */
      rf_apps_cmd_put( temp_comp_cmd );

    } /* if ((rfwcdma_get_rf_state(RFM_DEVICE_0) == RFWCDMA_MC_STATE_RX) ||
        (rfwcdma_get_rf_state(RFM_DEVICE_0) == RFWCDMA_MC_STATE_RXTX)) */
    else
    {
      ret_val = RFM_WCDMA_ERROR_BAD_STATE;
      RF_MSG_2( RF_ERROR, "rfm_wcdma_prep_temp_comp: [Invalid State] Device %d, "
                "State %d", device, rfwcdma_mc_state.car_path_state[path][0].rf_state );
    } /* if ! ((rfwcdma_get_rf_state(RFM_DEVICE_0) == RFWCDMA_MC_STATE_RX) ||
        (rfwcdma_get_rf_state(RFM_DEVICE_0) == RFWCDMA_MC_STATE_RXTX)) */
  } /* if ( tc_data->tc_state == TEMP_COMP_READY ) */
  else
  {
    ret_val = RFM_WCDMA_ERROR_INVALID_PARAMS;
    RF_MSG_2( RF_ERROR, "rfm_wcdma_prep_temp_comp: [Invalid State Data] "
              "Device %d, State %d", device, tc_data->tc_state );

  } /* if ! ( tc_data->tc_state == TEMP_COMP_READY ) */

  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  RF_MSG_2( RF_HIGH, "rfm_wcdma_prep_temp_comp: [Prep Cmd Q'ed] Device %d, "
            "TimeTaken %d us", device, profile_time );

  return ret_val;
} /* rfm_wcdma_prep_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Execute Temperature Compensation Update

  @details
  This function will perform dynamic update which includes performing rxlm 
  dynamic update to FW to perform temperature based IQMC.
 
  For successful completion of this API, FW should be in active state. Thus
  it is the responsibility of L1 software to ensure that this function is called
  only when FW is active and will remain active until the completion if this
  function.
 
  Moreover this function is supposed to be called by L1 software only for the
  case when prep temp comp is successfully completed.
 
  @param intf_param
  Structure containing the parameter to be used during temperature compensation
  execution
 
  @param callback
  A function pointer that will be called when the temp comp sequence
  is complete.  This function will be passed the 
  RFM_WCDMA_TEMP_COMP_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
  
  @exectime
  50 uS (@ Q6 576 MHz)
 
  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_exec_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  int32 xpt_delay_value = 0;
  rfm_device_enum_type device;
  rf_path_enum_type path;
  rf_time_tick_type profile_tick; /* Profile Tick */
  rf_time_type profile_time; /* Profile Time */
  rf_time_type time_since_prep_comp; /* Profile Time */
  rfwcdma_temp_comp_data_type *tc_data;
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE;
  rfwcdma_mdsp_set_data_param_type update_et_delay_param = {0};
  rfcommon_fbrx_error_type fbrx_temp_comp_result;
  uint16 *tx_therm_bins = NULL;

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();
  if ( intf_param == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_wcdma_exec_temp_comp: NULL param" );
    return RFM_WCDMA_ERROR_BAD_PARAM;
  } /* if ( intf_param == NULL ) */

  /* Get Device */
  device = intf_param->device;
  path = rfcommon_core_device_to_path(device);

  /* Get Temp Comp Data */
  tc_data = rfwcdma_data_get_temp_comp(device);

  if ( tc_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_wcdma_exec_temp_comp: NULL Temp Comp Data - "
                        "Device %d", device );
    return RFM_WCDMA_ERROR_NULL_STATE_DATA;
  } /* if ( tc_data == NULL ) */

  /* Return if Temp comp overriden to disable mode */
  if ( tc_data->override_disable_mode == TRUE )
  {
    RF_MSG_1( RF_HIGH, "rfm_wcdma_exec_temp_comp: - TempComp Overridden to Disable mode"
                        "Device %d", device );

    /* Execute Callback */
    if ( callback != NULL )
    {
      callback(RFM_WCDMA_TEMP_COMP_COMPLETE, userdata);
    } /* if ( callback != NULL ) */

    return RFM_WCDMA_HEALTHY_STATE;
  } /* if ( tc_data->override_disable_mode == TRUE ) */
  
  /* Compute the time elapsed between Prep complete and execute start */
  time_since_prep_comp = rf_time_get_elapsed( tc_data->prep_complete_time,
                                              RF_USEC );
  
  /* Proceed only if the prep has completed successfully */
  if ( tc_data->tc_state == TEMP_COMP_PREP_COMPLETED )
  {
    if ( rfwcdma_mc_state.car_path_state[path][0].rf_state  == RFWCDMA_MC_STATE_RXTX )
    {
      
      rfwcdma_core_rxctl_update_rsb_coeff(tc_data->last_temperature);
      
      if (rfwcdma_core_xpt_is_enabled(rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx))
      {
        /* Update the XPT path delay value */
        xpt_delay_value = rfwcdma_core_xpt_path_delay_comp( device,
                                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx,
                                                            rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_chan_tx,
                                                            FALSE,
                                                            0,
                                                            rfwcdma_mc_state.tx_carriers);
      
        /* Update ET path delay to MDSP/DM*/
        update_et_delay_param.rflm_handle = rfwcdma_mc_state.txlm_buf_idx;
        update_et_delay_param.set_data_cmd = RFWCDMA_MDSP_UPDATE_ET_DELAY;
        update_et_delay_param.data.gen_dw = (uint32)xpt_delay_value;
      
        rfwcdma_mdsp_async_set_data(&update_et_delay_param);
      
      }
      
      /* Query therm bins from tx temp comp NV */
      tx_therm_bins = rfnv_wcdma_tbl_ptr[rfwcdma_mc_state.car_path_state[RF_PATH_0][0].curr_band_tx]->lin_temp_comp_data.tx_therm_bins;
      if (tx_therm_bins != NULL)
      {
        /* Update fbrx temp comp with queried therm bins */ 
         fbrx_temp_comp_result = rfcommon_fbrx_mc_update_temp_comp(rfwcdma_mc_state.txlm_buf_idx,
                                                                  tc_data->last_temperature,
                                                                  tx_therm_bins);
        if (fbrx_temp_comp_result != RFCOMMON_FBRX_SUCCESS)
        {
          MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR,"FBRx Temp Comp failed:%d", fbrx_temp_comp_result);
        }
      }
      else
      {
         MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,"FBRx Temp Comp failed: Therm bins is NULL!");
      }
      
      
      /* Do VBatt based Max Power Backoff */
      rfwcdma_core_txplim_do_vbatt_based_backoff();
    }
    else if(rfwcdma_mc_state.car_path_state[path][0].rf_state  == RFWCDMA_MC_STATE_RX)
    {
      rfwcdma_core_rxctl_update_rsb_coeff(tc_data->last_temperature);
    }
    else
    {
      RF_MSG_1( RF_HIGH, "rfm_wcdma_exec_temp_comp: [State not in Rx or RxTx] "
                "Device %d", device );    
    }    
  } /* if ( tc_data->tc_state == TEMP_COMP_PREP_COMPLETED ) */
  else
  {

    ret_val = RFM_WCDMA_ERROR_INVALID_PARAMS;
    RF_MSG_2( RF_ERROR, "rfm_wcdma_exec_temp_comp: [Invalid State Data] "
              "Device %d, State %d", device, tc_data->tc_state );

  } /* if ! ( tc_data->tc_state == TEMP_COMP_PREP_COMPLETED ) */

  /* Reset the State */
  tc_data->tc_state = TEMP_COMP_READY;

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( RFM_WCDMA_TEMP_COMP_COMPLETE, userdata );
  } /* if ( callback != NULL ) */
  
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  RF_MSG_3( RF_HIGH, "rfm_wcdma_exec_temp_comp: [Completed] Device %d, "
            "Since Prep Done %d, TimeTaken %d us", 
            device, 
            time_since_prep_comp,
            profile_time );
  
  return ret_val;

} /* rfm_wcdma_exec_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Abort Temperature Compensation Update

  @details
  This function aborts the pending the temp comp operation. L1 software is
  expected to call this API in case the prep temp comp is completed and L1
  software cannot proceed with execute part of temp comp.
 
  @param intf_param
  Structure containing the parameter to be used during temperature compensation
  abort
 
  @param callback
  A function pointer that will be called when the temp comp sequence
  is complete.  This function will be passed the 
  RFM_WCDMA_TEMP_COMP_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.
 
  @exectime
  10 uS (@ Q6 576 MHz)
 
  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.
*/
rfm_api_status_t
rfm_wcdma_abort_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{

  rfwcdma_temp_comp_data_type *tc_data;
  rf_time_tick_type profile_tick; /* Profile Tick */
  rf_time_type profile_time; /* Profile Time */
  rfm_device_enum_type device;

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  if ( intf_param == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_wcdma_abort_temp_comp: NULL param" );
    return RFM_WCDMA_ERROR_BAD_PARAM;
  } /* if ( intf_param == NULL ) */

  /* Get Device */
  device = intf_param->device;

  /* Get Temp Comp Data */
  tc_data = rfwcdma_data_get_temp_comp(device);

  if ( tc_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_wcdma_abort_temp_comp: NULL Temp Comp Data - "
                        "Device %d", device );
    return RFM_WCDMA_ERROR_NULL_STATE_DATA;
  } /* if ( tc_data == NULL ) */

  /* Return if Temp comp overriden to disable mode */
  if ( tc_data->override_disable_mode == TRUE )
  {
    RF_MSG_1( RF_HIGH, "rfm_wcdma_abort_temp_comp: - TempComp Overridden to Disable mode"
                        "Device %d", device );

    /* Execute Callback */
    if ( callback != NULL )
    {
      callback(RFM_WCDMA_TEMP_COMP_ABORT_COMPLETE, userdata);
    } /* if ( callback != NULL ) */

    return RFM_WCDMA_HEALTHY_STATE;
  } /* if ( tc_data->override_disable_mode == TRUE ) */
  
  /* Reset the State */
  tc_data->tc_state = TEMP_COMP_READY;

  /* Execute Callback */
  if ( callback != NULL )
  {
    callback( RFM_WCDMA_TEMP_COMP_ABORT_COMPLETE, userdata );
  } /* if ( callback != NULL ) */
  
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );

  RF_MSG_2( RF_HIGH, "rfm_wcdma_abort_temp_comp: [Completed] Device %d, "
            "TimeTaken %d us", device, profile_time );

  return RFM_WCDMA_HEALTHY_STATE;
} /* rfm_wcdma_abort_temp_comp */

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function configures the NBR enable flag along with NBR RxLM index.
  If it is true then NBR is enabled so modG RxLM will be loaded along with
  tune. Otherwise modG RxLM will not be loaded.
 
  @param nbr_enable : fkag to enable/disable NBR operations
  @param rxlm_buffer_index: RxLM buffer index to be used for loading modG
===========================================================================*/
void rfm_set_nbr_enable( boolean nbr_enable, uint32 rxlm_buffer_index )
{
  rfwcdma_mc_nbr_config_param_type nbr_config_data;
  nbr_config_data.enable_nbr = nbr_enable;
  nbr_config_data.nbr_rxlm_idx = rxlm_buffer_index;

  RFWCDMA_MC_V(cmd_dispatch)( RFM_DEVICE_0, 
                              RFWCDMA_MC_CONFIG_NBR,
                              &nbr_config_data );
} /* rfm_set_nbr_enable */

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function sets the RFM FTM state variable e.g. FTM RF, FTM Cal, FTM NS
 
  @param state: RFM FTM state variable

===========================================================================*/
void rfm_wcdma_set_ftm_state(rfm_wcdma_ftm_state_type state)
{
  rfm_wcdma_ftm_state_type temp_state = RFM_WCDMA_FTM_NONE;

  switch (state)
  {
  /* Cal flag is set externally. When enter WCDMA FTM RF mode from FTM, check if
  Cal state flag has been set */
  case RFM_WCDMA_FTM_RF:
    if (rfm_get_calibration_state() == TRUE)
    {
      temp_state = RFM_WCDMA_FTM_CAL;
    }
    else
    {
      temp_state = RFM_WCDMA_FTM_RF;
    }
    break;

  case RFM_WCDMA_FTM_CAL:
  case RFM_WCDMA_FTM_NONSIG:
  case RFM_WCDMA_FTM_NONE:
  case RFM_WCDMA_FTM_INVALID:
    temp_state = state;
    break;
  }

  if ((state != RFM_WCDMA_FTM_NONE )&&(ftm_get_mode() != FTM_MODE))
  {
     RF_MSG_1( RF_ERROR, "rfm_wcdma_set_ftm_state: setting FTM RF state:%d while not in FTM mode!",state);
     temp_state = RFM_WCDMA_FTM_NONE;
  }
  rfm_wcdma_ftm_state = temp_state;

} /* rfm_wcdma_set_ftm_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function gets the RFM FTM state variable e.g. FTM RF, FTM Cal, FTM NS.
  L1 will use the FTM state to determine if certain initialization sequence/memory
  allocation can be skipped for timeline/memory optimizaions.
 
  @param state: RFM FTM state variable

===========================================================================*/
rfm_wcdma_ftm_state_type rfm_wcdma_get_ftm_state(void)
{
  return rfm_wcdma_ftm_state;
} /* rfm_wcdma_get_ftm_state */

/*! @endcond */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize Tx before rfm_enable_tx( ) is called.

  @details
  It initializes Tx parameter table, mDSP variables, and MODEM Tx registers.
  This function should be called before rfm_enable_tx( ) is called.

  @param tx_params
  Structure variable holding the parameters for needed to initialize Tx such as
  device id, multicarrier index, txlm handle. 

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_TX_INIT_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.

 */
 
rfm_api_status_t 
rfm_wcdma_tx_init
( 
  rfm_wcdma_tx_init_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t device_state = RFM_WCDMA_ERROR_BAD_STATE; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  boolean status = TRUE; 

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_tx_init : START" );

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_tx_init,
                    &callback,0,0,0 );

  status = rfwcdma_mc_init_tx(tx_params->device, 
                              tx_params->txlm_handle, 
                              tx_params->carrier_mask,0);

  /* Based on Status of MC execution, set CB Event Flag */
  if ( status == TRUE )
  {
    /* Flag Callback event as successful */
    cb_event = RFM_WCDMA_TX_INIT_COMPLETE;   
  } /* if ( status == TRUE ) */
      
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_FAILURE;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

   /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
   
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_tx_int: [FAILURE %d] -," 
              "Dev 0x%x Init_Tx Time %d uS", 
              ret_val, 
              tx_params->device,
              profile_time );
  } /* if (ret_val != RFM_CDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_tx_init: [Status %d] -, "
              "Dev 0x%x, Init_Tx Time %d uS", 
               ret_val, 
               tx_params->device,
               profile_time );
  } /* if ! (ret_val != RFM_CDMA_HEALTHY_STATE) */
   
  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API execution status */
  return ret_val;

}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Deconfig setting in rfm_init_wcdma_tx().

  @details
  It deconfigs Tx setting made in rfm_wcdma_tx_init().
  This function should be called before WL1 send txlm_cfg(disable) to FW.

  @param tx_params
  Structure holding parameters for tx_enable

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_TX_DEINIT_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.

  */

rfm_api_status_t 
rfm_wcdma_tx_deinit
( 
  rfm_wcdma_tx_deinit_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t device_state = RFM_WCDMA_ERROR_BAD_STATE; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  boolean status = TRUE; 

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_tx_deinit : START" );

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_tx_deinit,&callback,0,0,0 );

  status = rfwcdma_mc_decfg_tx(tx_params->device); 

  /* Based on Status of MC execution, set CB Event Flag */
  if ( status == TRUE )
  {
    /* Flag Callback event as successful */
    cb_event = RFM_WCDMA_TX_DEINIT_COMPLETE;   
  } /* if ( status == TRUE ) */
      
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_FAILURE;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

   /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
   
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_tx_deint: [FAILURE %d] -,Dev 0x%x Init_Tx Time %d uS", 
              ret_val, tx_params->device,profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_tx_deinit: [Status %d] -, "
              "Dev 0x%x, Init_Tx Time %d uS", 
               ret_val, 
               tx_params->device,
               profile_time );
  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */
   
  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API execution status */
  return ret_val;
}
/*----------------------------------------------------------------------------*/

/*!
  @brief
  Enables transmitter for the WCDMA mode of operation 

  @details
  Enables transmitter for WCDMA mode of operation, typically preparing transmitter for access attempts.
  Turns on LDOs for transmit hardware.  Turns on PA.  Places RF driver in
  Rx/Tx state of operation.

  @param tx_params
  Structure holding parameters for tx_enable
  
  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_TX_INIT_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.

 */

rfm_api_status_t 
rfm_wcdma_tx_enable
( 
  rfm_wcdma_tx_enable_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t device_state = RFM_WCDMA_ERROR_BAD_STATE; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  uint8 status = 1; 

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

    /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_tx_enable,&callback,0,0,0 );

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_tx_enable : START" );

  status = rfwcdma_mc_enable_tx(tx_params->device,NULL, NULL);

  /* Based on Status of MC execution, set CB Event Flag */

  /* Based on Status of MC execution, set CB Event Flag */
  if ( status == 1 )
  {
    /* Flag Callback event as successful */
    cb_event = RFM_WCDMA_TX_ENABLE_COMPLETE;   
  } /* if ( status == 1 ) */
      
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_FAILURE;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

   /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
   
  
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_tx_enable: [FAILURE %d] -,Dev 0x%x Init_Tx Time %d uS", 
              ret_val, tx_params->device,profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_tx_enable: [Status %d] -, "
              "Dev 0x%x, Init_Tx Time %d uS", 
               ret_val, 
               tx_params->device,
               profile_time );
  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */
   
  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API execution status */
  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables transmitter for the WCDMA mode of operation 

  @details
  Disables transmitter for WCDMA mode Turns off LDOs for transmit hardware.  Turns off PA.
  Places RF driver in Rx state of operation.

  @param tx_params
  Structure holding the transmitter to be turned off and the corresponding txlm handle.

  @param callback
  A function pointer that will be called when the entire wakeup sequence
  is complete.  This function will be passed with
  RFM_WCDMA_TX_ENABLE_COMPLETE event when all actions associated with this 
  API are finished.

  @param userdata
  An opaque data handle that may be used by the calling module to pass
  additional information to the callback function.

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.

*/
rfm_api_status_t 
rfm_wcdma_tx_disable
( 
  rfm_wcdma_tx_disable_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
)
{
  rfm_api_status_t device_state = RFM_WCDMA_ERROR_BAD_STATE; /* return value of device_state check */
  rf_time_type profile_time; /* Profile Time */
  rf_time_tick_type profile_tick; /* Profile Tick */
  rfm_cb_event_enum_type cb_event = RFM_EXECUTION_FAILURE; /* CB event type */
  rfm_api_status_t ret_val = RFM_WCDMA_HEALTHY_STATE; /* Return value */
  boolean status = TRUE; 

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rfwcdma_get_lock_data());

  /* Start Profiling of API */
  profile_tick = rf_time_get_tick();

  /* Throw F3 Message */
  RF_MSG( RF_HIGH, "rfm_wcdma_tx_disable : START" );

  /* Log the API */
  RFM_DEBUG_EN_Q( &rfm_wcdma_tx_disable,&callback,0,0,0 );

  status = rfwcdma_mc_disable_tx(tx_params->device);
  
  /* Based on Status of MC execution, set CB Event Flag */
  if ( status == TRUE )
  {
    /* Flag Callback event as successful */
    cb_event = RFM_WCDMA_TX_DISABLE_COMPLETE;   
  } /* if ( status == TRUE ) */
      
  else
  {
    /* assign the current device state to return value of API */
    ret_val = RFM_WCDMA_ERROR_FAILURE;
  } /* if ! ( device_state == RFM_WCDMA_HEALTHY_STATE ) */

   /* Execute Callback */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */
   
  /* get time spent in API */
  profile_time = rf_time_get_elapsed( profile_tick, RF_USEC );
   
  /* Debug Message based on return value */
  if (ret_val != RFM_WCDMA_HEALTHY_STATE)
  {
    /* Debug message */
    RF_MSG_3( RF_ERROR, "rfm_wcdma_tx_disable: [FAILURE %d] -,Dev 0x%x"
                        "Init_Tx Time %d uS", ret_val, 
                        tx_params->device,profile_time );
  } /* if (ret_val != RFM_WCDMA_HEALTHY_STATE) */
  else
  {
    /* Debug message */
    RF_MSG_3( RF_HIGH, "rfm_wcdma_tx_disable: [Status %d] -, "
              "Dev 0x%x, Init_Tx Time %d uS", 
               ret_val, 
               tx_params->device,
               profile_time );
  } /* if ! (ret_val != RFM_WCDMA_HEALTHY_STATE) */
   
  /* Acquire Critical Section Lock */
  rf_common_leave_critical_section(rfwcdma_get_lock_data());

  /* Return the API execution status */

  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query TxAGC logging parameters 

  @details
  The API provides information about the requested TxAGC parameters.

  @param tx_log
  Structure holding the TxAGC logging information

  @return
  Variable indicating the status of API execution. The error codes are
  defined in #rfm_wcdma_error_code_type enumeration.

*/
rfm_api_status_t 
rfm_wcdma_get_txagc_params
(
  rfm_wcdma_txagc_log_param_type *tx_log
)
{
  rfwcdma_core_util_txagc_logging_type txagc_data;

  rfwcdma_core_util_get_rflm_txagc_logging(&txagc_data);
  
  /* Populate the xpt mode and PA range values if uninitialized*/
  if (txagc_data.xpt_mode!= 0xFF)
  {
    tx_log->xpt_mode = txagc_data.xpt_mode;
  }
  else 
  {
    tx_log->xpt_mode = 0xFF;
  }

  if (txagc_data.pa_range!= 0xFF)
  {
    tx_log->pa_range = txagc_data.pa_range;
  }
  else 
  {
    tx_log->pa_range = 0xFF;
  }

  RF_MSG_2(RF_HIGH, "rfm_wcdma_get_txagc_params: XPT mode:%d, PA range:%d",tx_log->xpt_mode,tx_log->pa_range);

  return RFM_WCDMA_HEALTHY_STATE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the maximum TX power indicated by NV item depending
  on the channel type
 
  @param chan_type
  Specify the channel type (RACH or DCH)
 
  @param band
  Specifies the band of operation

  @retval
  The max TX power value.

*/
int16 rfm_wcdma_get_tx_max_power_nv(rfm_device_enum_type device,
                                         rfcom_txplim_channel_type chan_type,
                                         rfcom_wcdma_band_type band )
{
  int32 ret_val = 0;
  ret_val= rfwcdma_core_txplim_get_mtpl_nv_item(chan_type,band);
  RF_MSG_4(RF_HIGH, "rfm_wcdma_get_tx_max_power_nv: Device - %d, chan_type %d, band %d - Max Tx %d",
                                                         device,chan_type, band,ret_val);
  return (int16)ret_val;
}

/*----------------------------------------------------------------------------*/
