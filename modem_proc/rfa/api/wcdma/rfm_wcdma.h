#ifndef RFM_WCDMA_H
#define RFM_WCDMA_H

/*!
  @file
  rfm_wcdma.h

  @brief
  This file contains all declarations and definitions necessary to use the RF
  WCDMA drivers.

  @details
  The following function must be called before calling any other
  functions provided by this driver:

    1. rfm_init()

  This places the RF driver in the PARKED/SLEEP state.  Then a specific
  driver mode (i.e. CDMA 1x, HDR, WCDMA, GSM, GPS, PARKED, etc.) can be
  entered by calling the following functions in the sequence given:

    2. rfm_enter_mode()
    3. rfm_tune_to_chan()
    4. rfm_start_loops()

  The RF driver is now in the Idle/Rx state in the mode entered.

*/

/*===========================================================================
Copyright (c) 2008-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/rfm_wcdma.h#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/17/14   vbh     Add an API to provide Tx max power value by accepting band, device as a param
11/12/14   sd      Cleanup of deprecated sleep/wakeup routines
11/05/14   sd      Main line FEATURE_WCDMA_DC_HSUPA_FRAMEWORK
10/30/14   ag      Fixed Comipler Error
09/17/14   dr      Adding Jolokia Featurization
08/21/14   rmb     Remove unused APIs.
08/14/14   vbh     Added API definitions for Tx routines - init/deinit, enable/disable
08/05/14   dw      Added API to query different FTM states for WL1
06/19/14   aro/ak  Added Support for Prep/Exec Temp Comp Split
06/06/14   ac      nbr support
05/15/14   aro     Added Prep/Exec Temp Comp stubs
05/15/14   ak      Add parameter to sleep_rx API check for early shutdown
04/25/14   vs      Added stub for rfm_wcdma_prep_init_tx
04/11/14   vbh     [FR17754] Add prep_tune, exec_tune apis 
03/25/14   rmb     Add a new get rxagc API during X2W measurements.
03/11/13   dw      Add Bolt DCHSUPA support
03/05/14   vbh     Add new APIs to support dual wakeup/sleep
02/28/14   aa      DimePM linker error fix
02/24/14   vbh     Add sleep_Rx api to keep consistent naming convention
09/19/13   vs      Modification to RF AGC query interface
09/11/13   aro     Support to migrate WCDMA Temp Comp to RF Apps task
08/29/13   vs      Add RFM API to return RxAGC for all carriers
08/23/13   vs      Deprecated ununsed RF API
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build 
08/05/13   vs      Modified RF interface to query DBDC bands supported
08/01/13   dw      Add support for L1 to get the W2W neighbor physical device index 
07/31/13   aa      RF API to convert RF WCDMA band type to TRM band type 
07/19/13   ac      added another version of tune to take car idx for DBDC
07/26/13   vs      Add RF interface to query DBDC bands supported
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/25/13   ac      tx freezeaccum api change for both car and txlm state for dc cfg
07/27/13   ry      Updated the API rfm_get_ul_freq_from_dl_uarfcn()
05/22/13  ka/ac    dc hsupa changes
05/09/13   jj      Update RF API rfm_get_ul_freq_from_dl_uarfc()
05/09/13   jj      RF API to get UL frequency from DL UARFCN
04/29/13   jj      Add API (rfm_wcdma_get_default_band_chan) for WL1 to get default band and chan 
03/28/13   kcj     Added rfm_wcdma_wakeup_tx function.
03/06/13   kai     Added rfm_wcdma_decfg_tx function  
02/13/13   vb      Cleanup old antenna tuner implementation
02/06/13   rmb     Added rfm_set_wakeup_optimization function.
02/04/13   ac      rxlm HS support 
10/10/12   kai/vb  Changed Qtuner interface APIs 
09/27/12   nv      Added new API to flush data.
08/29/12   vb      Qtuner interface APIs 
06/13/12   swb     Add rxlm buffer index to rfm_pretune_to_chan function 
05/08/12   kai     Add API for CPC register setting 
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx 
02/27/12   swb     Return indication of missing cal data within init_tx function 
02/22/12   swb     Inter-freq tune optimization: add rfm_pretune_to_chan function 
02/16/12   kai     Add API for return PA state   
12/01/11   dw      Add support for WCDMA low power mode (DSR)
10/18/11   dw      Add prototype for rfm_power_clock_on and rfm_power_clock_off 
10/13/11   swb     Added API for freezing Rx AGC accumulator
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE. 
08/18/11   vb      Added support for splitting rfm_enable_tx &
                   added rfm_init_wcdma_tx
07/28/11   ckl     Remove obsolete rfm_get_cgagc_settling_time_for_tune() function
05/16/11   ad      Remove access to internal MDSP prototypes for rxagc cmd status 
                   and hdet buffered tx gain value
05/13/11   sar     Added prototypes needed by L1.
05/05/11   dw      Add rfm_get_freq_from_dl_arfcn()  
04/21/11   dw      Remove LM buffer index from wtow get rf ctl buffer API.
04/19/11   bmg     extern "C" for C++ safety
04/08/11   sar     Removed condition #def's for CMI-4.
03/25/11   dw      Add rfm_update_rxlm_buffer()
02/09/11   dw      Add RxLM interface support for W2W
01/05/10   dw      Initial support for RxLM
09/14/10   ap      Added GP_CLK enable/disable API 
07/23/10   ka      Change dual carrier interface to accept channel 
07/20/10   ka      Added API to determine if dual carrier is supported 
03/22/10   ka      Add interface for DC CM.
01/06/10   kguo    Merged from mdm9k dev branch to main\latest 
11/30/09   ckl     Added rfm_get_freq_from_dl_arfcn( ) for W2W measurement
11/11/09   ka      Updates for dual carrier.
10/23/09   kguo    Merged changes for 9k bringup 
06/04/09   ckl     Added rfm_get_freq_from_uarfcn, rfm_convert_chan_to_band,
                   rfm_get_hdet_tx_agc_value.
03/09/09   clk     Code cleanup
10/14/08   sr     Initial version to separate wcdma specific external interface.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfcom.h"
#include "rfm_types.h"
#include "lm_types.h"
#include "rfm_wcdma_param_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*----------------------------------------------------------------------------*/
/*! Enumeration for RFM WCDMA error codes */
typedef enum
{
  RFM_WCDMA_ERROR_NULL_STATE_DATA = -8,
  /*!< Error code indicating State Data is NULL */

  RFM_WCDMA_ERROR_BAD_PARAM = -7,
  /*!< Error code indicating Parameter passed to the function is bad */

  RFM_WCDMA_ERROR_INVALID_PARAMS = -6,
  /*!< Error code indicating that there is invalid parameters passed to the API*/

  RFM_WCDMA_ERROR_SCRIPT_WRITE = -5,
  /*!< Error code indicating that script writing failed */

  RFM_WCDMA_ERROR_BAD_PATH_MAPPING = -4,
  /*!< Error code indicating that there is bad path mapping */

  RFM_WCDMA_ERROR_BAD_STATE = -3,
  /*!< Error code indicating that there is bad state transition */

  RFM_WCDMA_ERROR_INVALID_MODE = -2,
  /*!< Error code when trying to tune the radio when RF Mode is not correct.*/

  RFM_WCDMA_ERROR_FAILURE = -1,
  /*!< Error code when RF func Fails inspite of good parameter and state. */

  RFM_WCDMA_HEALTHY_STATE = 0,
  /*!< Code when RF func has healthy RF state. This will be used during RF State
  validation. For RFM Status, any positive number refers to successful
  execution. */

} rfm_wcdma_error_code_type;

/*----------------------------------------------------------------------------*/
/*! Enumeration for RFM WCDMA FTM state. Use to distinguish different modes of 
  operation in FTM for RF */
typedef enum
{
  RFM_WCDMA_FTM_NONE = 0,
  /*!< RF in non-FTM state i.e. online mode */

  RFM_WCDMA_FTM_CAL = 1,
  /*!< RF in FTM Cal state */

  RFM_WCDMA_FTM_RF = 2,
  /*!< RF in FTM RF state */

  RFM_WCDMA_FTM_NONSIG = 3,
  /*!< RF in FTM non-signaling state */

  RFM_WCDMA_FTM_INVALID
  /*!< RF in FTM state max */

} rfm_wcdma_ftm_state_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the parameter which is to be sent to RF during 
temperature compensation update. */
typedef struct
{

  rfm_device_enum_type device;
  /*!< RFM device on which RF is requested to perform temperature 
  compensation update */

} rfm_wcdma_temp_comp_param;

/*----------------------------------------------------------------------------*/
/*! Structure to hold rfm_wcdma_tx_retune_bw param  */
typedef struct
{
  rfm_device_enum_type device;
  /*!< RFM device to tune*/

  uint32 multi_carrier_idx;
  /*!< Multi Carrier Index Bitmask */
}rfm_wcdma_tx_retune_bw_param;

/*--------------------------------------------------------------------------*/
void rfm_set_primary_path( rf_path_enum_type primary_path );

/*--------------------------------------------------------------------------*/
int32 rfm_do_tx_pwr_limit( rfcom_device_enum_type device );

/*--------------------------------------------------------------------------*/
void 
rfm_do_temp_comp
(
  rfm_device_enum_type device
);

/*--------------------------------------------------------------------------*/
void rfm_set_tx_pwr_limit ( rfcom_device_enum_type device,
                            int16 tx_limit_dbm,  /* Tx power limit in dBm */
                            rfcom_txplim_channel_type chan_type );

/*--------------------------------------------------------------------------*/
int16 rfm_get_rx_agc_val ( rfcom_device_enum_type device );

/*--------------------------------------------------------------------------*/
int16 rfm_get_tx_agc_val ( rfcom_device_enum_type device );

/*--------------------------------------------------------------------------*/
int16 rfm_get_receive_agc_val_for_rscp( rfcom_device_enum_type device,
                                        rfcom_rxagc_type rxagc_type );
/*--------------------------------------------------------------------------*/
int16 rfm_get_min_tx_power_val( void );

/*--------------------------------------------------------------------------*/
int16 rfm_get_hdet_apply_threshold_agc( void );

/*--------------------------------------------------------------------------*/
void rfm_get_wtow_rf_ctl_buf ( uint16 arfcn, void *buf_ptr);

/*--------------------------------------------------------------------------*/
void rfm_set_txfreezeaccum( uint8 val,rfcom_multi_carrier_id_type car_idx);
/*--------------------------------------------------------------------------*/
void rfm_turn_on_pa( void );

/*--------------------------------------------------------------------------*/
void rfm_turn_off_pa( void );

/*--------------------------------------------------------------------------*/
void rfm_enable_pwr_ctrl( void );

/*--------------------------------------------------------------------------*/
int16 rfm_get_txagctablestore(rfcom_multi_carrier_id_type car_idx);
/*--------------------------------------------------------------------------*/
int16 rfm_convert_tx_agc_to_dbm_unit( int16 txagcval );

/*--------------------------------------------------------------------------*/
int16 rfm_convert_tx_dbm_to_agc_unit( int16 dbmval );

/*--------------------------------------------------------------------------*/
int16 rfm_get_ue_max_tx_power( rfcom_txplim_channel_type chan_type );

/*--------------------------------------------------------------------------*/
void rfm_get_tx_rotator_angle ( uint16 *tx_rot_ang_pa_00_p,  /* Pointer to tx rotator angle pa state 00 */
                                uint16 *tx_rot_ang_pa_01_p,  /* Pointer to tx rotator angle pa state 01 */
                                uint16 *tx_rot_ang_pa_10_p,  /* Pointer to tx rotator angle pa state 10 */
                                uint16 *tx_rot_ang_pa_11_p);   /* Pointer to tx rotator angle pa state 11 */

/*--------------------------------------------------------------------------*/
uint8 rfm_pa_is_on( void );

/*--------------------------------------------------------------------------*/
void rfm_log_single_agc_data ( rfcom_device_enum_type device );

/*--------------------------------------------------------------------------*/
uint16 rfm_get_enc_btf_val( void );

/*--------------------------------------------------------------------------*/
void rfm_get_max_tx_pwr_limit( rf_max_tx_pwr_limit_type *rf_mtpl );

/*--------------------------------------------------------------------------*/
void rfm_enable_trk_lo_adj_pdm( boolean enable );

/*--------------------------------------------------------------------------*/
void rfm_turn_on_rf_ldo( uint16 delay ); /* time in us to wait after turning on LDO */

/*--------------------------------------------------------------------------*/
rfi_band_type rfm_get_curr_band( void );

/*--------------------------------------------------------------------------*/
uint8 rfm_srchcm_interf_rfsetup_mdsp_buf_sz_w16_get(void);

/*--------------------------------------------------------------------------*/
int16 rfm_get_max_tx_power_nv_item(rfcom_txplim_channel_type chan_type);

/*--------------------------------------------------------------------------*/
void rfm_set_tx_general_ctl( rfcom_txplim_channel_type chan_type );

/*--------------------------------------------------------------------------*/
uint16 rfm_get_rf_warmup_time( void );

/*--------------------------------------------------------------------------*/
void rfm_enable_data_mover_clk_for_cm ( boolean enable );

/*--------------------------------------------------------------------------*/
rfcom_dm_for_hdet_type rfm_enable_data_mover_for_hdet (boolean enable);

/*--------------------------------------------------------------------------*/
boolean rfm_get_rf_capability ( rfcom_rf_cap_type feature );

/*--------------------------------------------------------------------------*/
int16 rfm_get_mdsp_tx_agc ( void );

/*--------------------------------------------------------------------------*/
int16 rfm_get_mdsp_tx_power_limit ( void );

/*--------------------------------------------------------------------------*/
boolean rfm_band_supports_rxd( rfi_band_type band );

/*--------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_DB_DC_HSDPA 
void rfm_write_tech_dependent_nv_items_to_mdsp(rfcom_wcdma_band_type band,rfcom_tech_type tech);
#else
void rfm_write_tech_dependent_nv_items_to_mdsp(rfcom_tech_type tech);
#endif
/*--------------------------------------------------------------------------*/
boolean rfm_write_tech_dependent_dynamic_update(rfcom_tech_type curr_tech,uint8 rxlm_buf_idx[],rfcom_device_enum_type device);

/*--------------------------------------------------------------------------*/
void rfm_log_agc(rfcom_device_enum_type device);

void rfm_flush_agc_data(rfcom_device_enum_type device);

/*--------------------------------------------------------------------------*/
int16 rfm_get_tx_pwr_limit( void );

/*--------------------------------------------------------------------------*/
int16 rfm_get_receive_agc_val ( rfcom_device_enum_type device,
                                rfcom_rxagc_type rxagc_type,
                                rfcom_rxagc_unit_type rxagc_unit);

/*--------------------------------------------------------------------------*/
int16 rfm_get_receive_agc_car1_val ( rfcom_device_enum_type device,
                                rfcom_rxagc_type rxagc_type,
                                rfcom_rxagc_unit_type rxagc_unit);
/*----------------------------------------------------------------------------*/
int16 rfm_wcdma_get_rxagc
(
  rfcom_device_enum_type device,
  rfcom_rxagc_type rxagc_type,
  rfcom_rxagc_unit_type rxagc_unit,
  rfcom_multi_carrier_id_type carrier_idx
);

/*--------------------------------------------------------------------------*/
uint32 rfm_get_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan,
  rfcommon_channel_ul_dl_type ul_dl
);
/*--------------------------------------------------------------------------*/
uint32 rfm_get_ul_freq_from_dl_uarfcn
(
  sys_band_class_e_type sys_band,
  word ul_chan
);
/*--------------------------------------------------------------------------*/
uint32 rfm_get_lo_freq_from_uarfcn
(
  rfcom_wcdma_band_type band,
  word chan[],                            /* input channel number */
  rfcommon_channel_ul_dl_type ul_dl,
  rfcom_multi_carrier_hspa_id_type num_chan
);
/*--------------------------------------------------------------------------*/
uint32 rfm_get_freq_from_dl_arfcn( uint16 channel );

/*--------------------------------------------------------------------------*/
rfcom_wcdma_band_type rfm_convert_chan_to_band( uint16 channel );

/*--------------------------------------------------------------------------*/
int16 rfm_get_hdet_tx_agc_value( void );

/*--------------------------------------------------------------------------*/
#ifdef FEATURE_WCDMA_DB_DC_HSDPA 
 uint32 rfm_tune_to_multi_chan_v2(rfm_device_enum_type device, uint16 channel[],
                          uint32 multi_carrier_idx,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler
                          ,uint32 rxlm_buf_idx
                          );

#endif
uint32 rfm_tune_to_multi_chan( rfcom_device_enum_type device, uint16 channel[],
                          rfcom_multi_carrier_hspa_id_type num_chan,
                          rfm_synth_tune_enum_type synths,
                          const void *user_data_ptr,
                          rfm_cb_handler_type cb_handler
                          ,uint32 rxlm_buf_idx
                          );

/*--------------------------------------------------------------------------*/
int16 rfm_get_rx_raw_agc_val ( rfcom_device_enum_type );
/*--------------------------------------------------------------------------*/
int16 rfm_get_rx_raw_agc_val_xtow_meas ( rfcom_device_enum_type device, rfcom_wcdma_band_type neigh_band, boolean bypass_rxagcon );
/*--------------------------------------------------------------------------*/
int16 rfm_get_rx_agc_val_xtow_meas ( rfcom_device_enum_type device, rfcom_wcdma_band_type neigh_band, boolean bypass_rxagcon );

/*--------------------------------------------------------------------------*/
void rfm_get_multi_carrier_wtow_rf_ctl_buf ( uint16 arfcn[], 
                                             rfcom_multi_carrier_hspa_id_type carrier_type, 
                                             void *buf_ptr 
                                            );

/*--------------------------------------------------------------------------*/
boolean rfm_band_supports_dual_carrier( uint16 chan );

/*----------------------------------------------------------------------------*/
void rfm_gp_clock_on (void);

/*----------------------------------------------------------------------------*/
void rfm_gp_clock_off (void );

/*----------------------------------------------------------------------------*/
boolean rfm_update_rxlm_buffer( rfcom_device_enum_type device, 
                                uint16 channel[],
                                rfcom_multi_carrier_hspa_id_type num_chan,
                                uint32 rxlm_buf_idx);

/*--------------------------------------------------------------------------*/ 
 boolean rfm_init_wcdma_tx(uint32 txlm_buf_idx, uint32 multi_carrier_idx,rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfg);
 
/*----------------------------------------------------------------------------*/
void rfm_wcdma_wakeup_tx(void);
/*----------------------------------------------------------------------------*/
rf_path_enum_type rfm_get_primary_path(void);
/*----------------------------------------------------------------------------*/
void rfm_power_clock_on(void);
/*----------------------------------------------------------------------------*/
void rfm_set_rx_freeze_accum( rfcom_device_enum_type device, boolean freeze );
/*----------------------------------------------------------------------------*/
void rfm_power_clock_off(void);
/*----------------------------------------------------------------------------*/
boolean rfm_set_low_power_mode(rfcom_lpm_config_enum_type lpm_config,
                               uint32 c0_rxlm_buf_idx,
                               uint32 c1_rxlm_buf_idx);
/*----------------------------------------------------------------------------*/
uint8 rfm_get_pa_state( void );
/*----------------------------------------------------------------------------*/
void rfm_register_cpc_drx_state(boolean enable);

/*----------------------------------------------------------------------------*/
void rfm_wcdma_antenna_tuner_tick(void);

/*----------------------------------------------------------------------------*/
void rfm_set_wakeup_optimization( boolean enable_optimization );

/*----------------------------------------------------------------------------*/
boolean rfm_wcdma_decfg_tx(void);

/*----------------------------------------------------------------------------*/
void rfm_wcdma_get_default_band_chan(rfi_band_type *band, uint16 *chan);

/*----------------------------------------------------------------------------*/
rfi_band_type rfm_wcdma_convert_band_rfcom_to_rfi
(
  rfcom_wcdma_band_type band
);

/*----------------------------------------------------------------------------*/

boolean rfm_wcdma_get_dbdc_band_support( uint16 *num_band_comb_supported, const rfm_wcdma_dual_band_type** band_comb_list );

/*----------------------------------------------------------------------------*/
uint8 rfm_wcdma_get_nbr_phy_dev_ind(uint16 chan, rfm_device_enum_type device);

boolean rfm_is_wcdma_supported(void);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_prep_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/

rfm_api_status_t
rfm_wcdma_prep_init_tx
(
  const rfm_device_enum_type device,
  const lm_handle_type txlm_handle,
  const uint32 carrier_mask,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*! @endcond */
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_rx_prep_wakeup
(
  const rfm_wcdma_rx_wakeup_params *const rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_rx_exec_wakeup
(
  const rfm_wcdma_rx_wakeup_params *const rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_rx_sleep
(
  const rfm_wcdma_rx_sleep_params *const rx_sleep_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_tx_retune_bw
(
  const rfm_wcdma_tx_retune_bw_param* const tx_retune_bw_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_rx_prep_tune
(
  rfm_wcdma_rx_tune_params *rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_rx_exec_tune
(
  rfm_wcdma_rx_tune_params *rx_wakeup_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_prep_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_exec_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfm_wcdma_abort_temp_comp
(
  const rfm_wcdma_temp_comp_param *intf_param,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/
void rfm_set_nbr_enable( boolean nbr_enable, uint32 rxlm_buffer_index );
/*----------------------------------------------------------------------------*/
void rfm_wcdma_set_ftm_state(rfm_wcdma_ftm_state_type state);
/*----------------------------------------------------------------------------*/
rfm_wcdma_ftm_state_type rfm_wcdma_get_ftm_state(void);
/*----------------------------------------------------------------------------*/

rfm_api_status_t 
rfm_wcdma_tx_init
( 
  rfm_wcdma_tx_init_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/

rfm_api_status_t 
rfm_wcdma_tx_deinit
( 
  rfm_wcdma_tx_deinit_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/

rfm_api_status_t 
rfm_wcdma_tx_enable
( 
  rfm_wcdma_tx_enable_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/

rfm_api_status_t 
rfm_wcdma_tx_disable
( 
  rfm_wcdma_tx_disable_params *tx_params,
  const rfm_cb_handler_type callback,
  void* const userdata
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t 
rfm_wcdma_get_txagc_params
(
  rfm_wcdma_txagc_log_param_type *tx_log
);
/*----------------------------------------------------------------------------*/

int16 rfm_wcdma_get_tx_max_power_nv(rfm_device_enum_type device,
                                    rfcom_txplim_channel_type chan_type,
                                    rfcom_wcdma_band_type band );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RFM_WCDMA_H */
