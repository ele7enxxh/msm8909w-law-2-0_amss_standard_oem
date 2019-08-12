#ifndef RFWCDMA_MC_H
#define RFWCDMA_MC_H

/*
   @file
   rfwcdma_mc.h

   @brief

   @details

*/

/*===========================================================================
Copyright (c) 1999 - 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2008-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/wcdma/rfwcdma_mc.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/20/15   vs      FR 25072 - Dynamic enable/disable of one of two WTRs on 9x45 based on band needed
03/09/14   ag      Skip Hdet reading and temp comp in W2G gap
11/17/14   rmb     Add a new api to get source band for a RFM device.
11/12/14   sd      Cleanup of deprecated sleep/wakeup routines
11/05/14   sd      Main line feature FEATURE_WCDMA_DC_HSUPA_FRAMEWORK
10/30/14   ag      Fixed Compiler Error
09/17/14   dr      Adding Jolokia Featurization
09/15/14   vbh     Added device as a parameter to support Tx sharing
08/21/14   rmb     Remove unused APIs and add alternate path in msm.
08/05/14   vbh     Added a parameter in rfwcdma sleep api to pass the skip agc flag
08/06/14   dw      Cleanup unused state variable
08/01/14   vbh     Added carrier index as a parameter for exec tune
06/30/14   vbh     [DR-DS] [1] Clean-up device 0 hardcode 
                   [2]Add MC state variable to save Primary Rx device
06/23/14   zhh     implement ASM sleep during retune or any ifreq/irat 
06/06/14   ac      nbr support
05/15/14   aro     Deprecated old command to perform MTPL update
05/15/14   ak      Add parameter to sleep_rx API check for early shutdown
05/01/14   rmb     Remove old AGC log interface 0x4176 version 1.
04/11/14   vbh     [FR17754] Added APIs to support script based tune
03/28/14   vbh     Added rx_sleep API for concurrent FR2001
03/25/14   rmb     Add a new get rxagc API during X2W measurements.
03/13/14   vbh     Added API to support concurrent wakeup 
03/11/14   dw      Added DCHSUPA support on Bolt
02/24/14   vbh     Add sleep_Rx api to keep consistent naming convention
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
01/09/14   vbh     IQ capture - add an API to make MC state available
01/02/14   dw      Bolt cleanup
11/21/13   tks     Added function prototype for get 3-carrier status
10/28/13   dw      Move rxlm_buf_idx out of car_path_state in sm
09/13/13   kai     Add mdsp_init_once_flag
09/11/13   kr      Fix compiler warnings
09/06/13   aa      Increased RFFE script size to accommodate RF tuner requirement
08/30/13   vs      Add RF API rfwcdma_mc_get_rxagc
08/29/13   vs      Add Carrier ID to RxAGC type
08/28/13   vs      Changed the prototype of API rfwcdma_mc_get_carr_state
08/27/13   dj      Added state variable use_fb_path_flag and the corresponding 
                   get and set functions
08/23/13   vs      Added support for 3 carrier
08/14/13   ak      phone enter to "offline" with QRD RP4 RF card on TR1.1 CRM build 
08/05/13   vs      Modified RF interface to query DBDC bands supported
08/01/13   dw      Add support for L1 to get the W2W neighbor physical device index 
07/26/13   vs      Add RF interface to query DBDC bands supported
07/03/13   aa      Increased RFFE buffer size to support tuner scripts
06/27/13   ac      Unified RF API to support query of RxAGC for SC and DC case
06/24/13   ac      tx freezeaccum api change for both car and txlm state for dc cfg 
06/28/13   vs      Add WCDMA support for FTM_PROGRAM_QFE_SCENARIO
05/21/13   rmb     Added API to get RF path state and synth state
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/09/13   jl      Port IQ capture code
05/09/13   jj      RF API to get UL frequency from DL UARFCN
05/01/13   rmb     Added rfwcdma_mc_get_car_state_info_from_device() API to get state information for a particular device.
04/29/13   jj      Add support to get default band
04/23/13   ka      Initial DBDC support
04/04/13   ka      State machine updates
03/28/13   kcj     Added rfwcdma_mc_wakeup_tx function. 
03/13/13   jj      declare function rfwcdma_msgr_register, rfwcdma_msgr_deregister and rfwcdma_mc_set_multi_tx_sar_limit
03/07/13  tks/shb  Increased WCDMA SSBI script size for WTR1625 support
03/06/13   kai     Added rfwcdma_mc_decfg_tx
03/01/13   kai     Removed rfwcdma_mc_enable_et 
02/15/13   vb      Removed support for old antenna tuner scenario programming
02/07/13   ndb/vb  Added rfwcdma_mc_set_tuner_tune_code_override() implementation support 
02/06/13   rmb     Added rfwcdma_mc_set_wakeup_optimization enum. 
02/04/13   ac      HS state rxlm support
01/28/13   kai     Removed rfwcdma_mc_set_sapt_state
11/15/12   kai     Added disable ET
11/15/12   ka      Initial support for DCHSUPA
11/09/12   kai     Added enable ET
11/02/12   dw      [1] Add support for Dual Carrier on Dime
                   [2] Remove RxAGC enable flag
09/27/12   nv      Adding new command AGC flush data
10/08/12   ac      drx crash fix during rf cal
08/28/12   tks     Added support for scripting and immediate hw writes
08/27/12   tks     Increased the max size of sbi, rffe and grfc
08/17/12   tks     Added rxagc_enable flag as a part of mc_state variable 
07/27/12    ka     Fix merge error 
06/13/12   swb     Add rxlm buffer index to rfm_pretune_to_chan function 
05/08/12   kai     Add API for CPC register setting 
04/17/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
04/13/12   kai     Revert the following update_txlm_buffer change since gsm layer1 dependence not ready
04/13/12   kai     Move update_txlm_buffer from enable_tx to init_tx, remove txlm_buf_idx from disable_tx
03/07/12   cri     Added initial DPD support
02/27/12   swb     Return indication of missing cal data within init_tx function
02/22/12   swb     Inter-freq tune optimization: add rfwcdma_mc_pretune_to_chan function 
02/16/12   kai     Added PA state API RFWCDMA_MC_GET_PA_STATE in enum rfwcdma_mc_cmd_type  
12/01/11   dw      Add support for WCDMA low power mode (DSR)
11/17/11   aak/dw  Changes to enable Quiet mode  
10/13/11   swb     Added API for freezing Rx AGC accumulator
09/28/11   sb      Mainline FEATURE_RF_COMMON_LM_RFM_INTERFACE and FEATURE_RF_WCDMA_LM_RFM_INTERFACE.
09/08/11   ac      SAR support
08/18/11   vb      Added support for splitting rfwcdma_mc_enable_tx &
                   added rfwcdma_mc_init_tx
08/08/11   dw      RxD only mode support on Nikel
07/28/11   ckl     Remove RFWCDMA_MC_GET_CGAGC_SETTLING_TIME
05/05/11   dw      Add RFWCDMA_MC_GET_LO_FREQ_FROM_UARFCN  
04/20/11   dw      Temporary add back lm buffer indexes
04/20/11   dw      Added back function protype that was removed by mistake
04/08/11   sar     Removed condition #def's for CMI-4.
03/25/11   dw      Add RFWCDMA_MC_UPDATE_RXLM_BUFFER
03/02/11   dw      Nikel updates
02/25/11   ka/dw   Merge support for thermal mitigation  
01/28/11   dw      Initial TxLM support for WCDMA
01/05/10   dw      Initial support for RxLM
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
09/14/10   ap      Added GP_CLK enable/disable API  
08/19/10   ka      Port over exit mode fixes 
07/30/10   dw      Change the function prototype of rfwcdma_mc_force_tune_to_multi_chan
07/30/10   dw      Added rfwcdma_mc_tune_to_multi_chan_noband_check()
07/20/10   ka      Added API to determine if dual carrier is supported 
06/04/10   kguo    Added ltow support
05/21/10   dw      Sleep support for WCDMA Dual Carrier
05/20/10   tws     Add API to read/set gtow flag.
05/10/10   dw      Support for DC->SC transition
03/24/10   ka      Added support for DC wtow.
03/19/10   lcl     Changed rfwcdma_mc_init function prototype
03/16/10   kguo    Merged from scmm_final_merge branch
02/18/10   ad      Support for SC-DC transition for both antennas
02/03/10   kma     Fixed compilation issue for CDMA only build
29/01/10   sr      Changed the <tech>_mc_init() signature to fix warnings in 
                   <tech> specific builds
01/28/10   kma     Added rfwcdma_mc_power_clock_on/off()
01/18/10   kguo    Added prototype for tune_to_multi_chan
01/06/10   kguo    Merged from mdm9k dev branch to main\latest
11/30/09   bmg     Added common_init_successful parameter to initialization
                   Changed init function return to boolean
10/23/09   kguo    Merged changes for 9k bringup
10/07/09   ckl     Add rfwcdma_mc_set_primary_path( )
09/17/09   ckl     Moved Tx functions to core_txctl file.
06/04/09   ckl     Support rfm_get_freq_from_uarfcn, rfm_convert_chan_to_band,
                   rfm_get_hdet_tx_agc_value.
03/22/09   ckl     Added new rfm functions, rfwcdma_enable_rx, rfwcdma_disable_rx,
                   rfwcdma_mc_ioctl, and rfwcdma_mc_init
03/18/09   ckl     Fix duplicate enum names between rfwcdma_mc_state_enum_type
                   and rf_state_enum_type
03/17/09   ckl     Change rf_state_enum_type to rfwcdma_mc_state_enum_type for
                   SCMM build.
10/14/08   sr      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfwcdma_core.h"

/* additional header file*/
#include "msgr_types.h"
//#include "rfm_wcdma.h"
#include "rfwcdma_msg.h"
//#include "rfmeas_types.h"
#include "rfm_async_types.h"
#include "rfm_wcdma_param_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFWCDMA_MAX_SSBI_SCRIPT_SIZE 384
#define RFWCDMA_MAX_RFFE_SCRIPT_SIZE 200
#define RFWCDMA_MAX_GRFC_SCRIPT_SIZE 16

#define RFWCDMA_MAX_RX_DEVICES RFM_DEVICE_4

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_set_multi_tx_sar_limit(rfcom_device_enum_type device, int16 sar_limit);

/* ----------------------------------------------------------------------- */
/*!
   @brief
   Enumeration for states for WCDMA mode.

*/
typedef enum
{

  /*! @brief WCDMA Init State. Default state is Init. */
  RFWCDMA_MC_STATE_POWERUP_INIT,
  RFWCDMA_MC_STATE_ENTER_MODE,
  
/*! @brief WCDMA State indicating completion of building of scripts for Rx wakeup */
  RFWCDMA_MC_STATE_RX_PREP,

/*! @brief WCDMA State indicating completion of building of scripts for Rx Tune */
  RFWCDMA_MC_STATE_RX_PREP_TUNE,

  /*! @brief WCDMA Rx state.  This state is entered by rfwcdma_mc_tune_to_chan() or
  rfwcdma_mc_disable_wcdma_tx() and is exited by either rfwcdma_mc_rx_sleep() or
  rfwcdma_mc_enable_wcdma_tx()*/
  RFWCDMA_MC_STATE_RX,

  /*! @brief WCDMA RxTx state.  This state is entered by rfwcdma_enable_wcdma_tx()
  and is exited by rf_disable_wcdma_tx().*/
  RFWCDMA_MC_STATE_RXTX,

/*! @brief WCDMA State indicating completion of building of scripts for Rx Tune. 
    MC will enter this stae when tune is called with Tx ON */
  RFWCDMA_MC_STATE_RXTX_PREP_TUNE,

  /*! @brief WCDMA Sleep state.  This state is entered by rfwcdma_mc_rx_sleep()
  and is exited by rfwcdma_mc_wakeup().*/
  RFWCDMA_MC_STATE_SLEEP,


  /*! @brief GPS State. Entered by a call to rf_digital_band_select() with GPS as the
  passed rf band argument.*/
  RFWCDMA_MC_STATE_GPS,
} rfwcdma_mc_state_enum_type;

typedef enum
{ 
  RFWCDMA_MC_DISABLED = 0,
  RFWCDMA_MC_SC = 1,
  RFWCDMA_MC_DC = 3,
  RFWCDMA_MC_3C = 7,
  RFWCDMA_MC_CARRIER_IDX_MAX
} rfwcdma_mc_carrier_idx_enum_type;

typedef enum
{ 
  RFWCDMA_MC_ASM_TUNER_SCRIPTS = 0,
  RFWCDMA_MC_GRFC_SCRIPTS = 1,
  RFWCDMA_MC_WTR_SCRIPTS= 2,
  RFWCDMA_MC_WAKEUP_SCRIPTS_MAX
} rfwcdma_mc_wakeup_scripts_enum_type;

typedef struct
{
  /*! @brief Maintains state of each path */
  rfwcdma_mc_state_enum_type  rf_state;

  /*! @brief Current band for each path. Default is invalid_band
  Updated during tune to channel for RX*/
  rfcom_wcdma_band_type curr_band_rx;

   /*! @brief Current band for each path. Default is invalid_band
  Updated during tune to channel for TX*/
  rfcom_wcdma_band_type curr_band_tx;

  /*! @brief Current RX and TX channel. The channels are usually mapped 1:1
  but for flexibility we maintain rx and tx channel information */
  uint16 curr_chan_rx;
  uint16 curr_chan_tx;

  rfcom_device_enum_type rx_rfm_device;
  rfcom_device_enum_type tx_rfm_device;

}rfwcdma_mc_car_path_state_type;

/* !@todo: anupamav probably this structure should be shared by technologies, so that
different paths can be used by different technologies */
/* ----------------------------------------------------------------------- */
/*!
   @brief
   The structure maintains state variables for WCDMA.

   @details
   The object of the structure gets updated when State or band changes.
   Rx channel, Tx channel are also maintained here. Tx channel is
   maintained so that it provides clear distinction for RX and TX functionality
   when band changes.

   Default for state = Init_state, curr_band = INVALID_BAND, primary_path = RF_PATH_0.

*/
typedef struct
{

  /*! @brief Maintains state of each path and carrier */
  rfwcdma_mc_car_path_state_type car_path_state[RF_PATH_MAX][RFCOM_MAX_CARRIERS];

   /*! @brief Maintains state to be used for inverse mapping*/
  rf_path_enum_type primary_path;

   /*! @brief Maintains state of one time initialization */
  boolean init_once_flag;

  /* WCDMA CPC register state = TRUE/FALSE*/
  boolean cpc_state_flag;

  /*! RXLM buffer index used for a RFM device */
  uint32 rxlm_buf_idx[RFM_DEVICE_4]; 

  /* TXLM buffer index */
  uint32 txlm_buf_idx;

  uint32 rx_multi_carrier_idx[RFM_DEVICE_4];
  
  /* Varibale to hold the last multi_carrier index on a device.
   Used to stop AGC on all applicable carriers during tune */

  uint32 rx_multi_carrier_idx_shadow[RFM_DEVICE_4];

  uint32 tx_multi_carrier_idx;
  
  rfcom_multi_carrier_hspa_id_type rx_carriers[RFM_DEVICE_4];

  rfcom_multi_carrier_hspa_id_type tx_carriers;

  boolean mdsp_init_once_flag;

  uint16 warmup_time_cx8;

  /*! ASM status bit map for a RFM device */
  uint16 asm_status_bit_map[RFM_DEVICE_4]; 
  
  /*! Tuner status bit map for a RFM device */
  uint16 tuner_status_bit_map[RFM_DEVICE_4]; 
  
/*! Remember primary and diversity Rx devices for DRDs scenario*/
 
  rfm_device_enum_type primary_rx_device;
  
  rfm_device_enum_type secondary_rx_device;

  boolean irat_script;

  uint16 pwr_vote_status_bit_map[RFM_DEVICE_4]; 

}rfwcdma_mc_state_type;


extern rfwcdma_mc_state_type rfwcdma_mc_state;
	
/*switch between exec_immediate or prepare script*/
extern boolean rfwcdma_exec_type; 
extern rfcom_band_type_u tx_band;


/*----------------------------------------------------------------------------*/
typedef enum
{
  RFWCDMA_MC_IS_SLEEPING,
  RFWCDMA_MC_GET_BAND_MASK,
  RFWCDMA_MC_SET_PRIMARY_PATH,
  RFWCDMA_DO_TEMP_COMP,
  RFWCDMA_MC_SET_TX_PWR_LIMIT,
  RFWCDMA_MC_GET_RX_AGC,
  RFWCDMA_MC_GET_TX_AGC_VAL,
  RFWCDMA_MC_GET_RX_AGC_VAL_FOR_RSCP,
  RFWCDMA_MC_GET_RX_AGC_C0_VAL_FOR_RSCP,
  RFWCDMA_MC_GET_RX_AGC_C1_VAL_FOR_RSCP,
  RFWCDMA_MC_GET_MIN_TX_PWR_VAL,
  RFWCDMA_MC_GET_HDET_THRESHOLD_AGC,
  RFWCDMA_MC_GET_WTOW_RF_CTL_BUF,
  RFWCDMA_SET_TX_FREEZE_ACUUM,
  RFWCDMA_MC_TURN_ON_PA,
  RFWCDMA_MC_TURN_OFF_PA,
  RFWCDMA_MC_ENABLE_PWR_CTRL,
  RFWCDMA_MC_GET_TX_AGC_TABLES,
  RFWCDMA_MC_TX_AGC_TO_DBM,
  RFWCDMA_MC_TX_DBM_TO_AGC,
  RFWCDMA_MC_GET_UE_MAX_TX_PWR,
  RFWCDMA_MC_GET_TX_ROTATOR_ANGLE,
  RFWCDMA_MC_PA_IS_ON,
  RFWCDMA_MC_GET_ENC_BTF_VAL,
  RFWCDMA_MC_GET_MAX_TX_PWR_LIMIT,
  RFWCDMA_MC_ENABLE_TRK_LO_ADJ_PDM,
  RFWCDMA_MC_TURN_ON_RF_LDO,
  RFWCDMA_MC_GET_CURR_BAND,
  RFWCDMA_MC_GET_MDSP_BUF_SIZE_W16,
  RFWCDMA_MC_GET_MAX_TX_PWR_CONFIG_ITEM,
  RFWCDMA_MC_GET_RF_WARMUP_TIME,
  RFWCDMA_MC_ENABLE_DATA_MOVER_CLK_FOR_CM,
  RFWCDMA_MC_ENABLE_DATA_MOVER_FOR_HDET,
  RFWCDMA_MC_GET_RF_CAPABILITY,
  RFWCDMA_MC_GET_MDSP_TX_AGC,
  RFWCDMA_MC_GET_MDSP_TX_PWR_LIMIT,
  RFWCDMA_MC_BAND_SUPPORTS_RXD,
  RFWCDMA_MC_WRITE_TECH_DEPENDENT_CONFIG_TO_MDSP,
  RFWCDMA_MC_GET_TX_PWR_LIMIT,
  RFWCDMA_MC_GET_RX_AGC_VAL,
  RFWCDMA_MC_GET_RX_AGC_C0_VAL,
  RFWCDMA_MC_GET_RX_AGC_C1_VAL,
  RFWCDMA_MC_GET_RAW_RX_AGC,
  RFWCDMA_MC_GET_RAW_RX_AGC_X2W_MEAS,
  RFWCDMA_MC_GET_RX_AGC_X2W_MEAS,
  RFWCDMA_MC_GET_FREQ_FROM_UARFCN,
  RFWCDMA_MC_GET_UL_FREQ_FROM_DL_UARFCN,
  RFWCDMA_MC_GET_LO_FREQ_FROM_UARFCN,
  RFWCDMA_MC_CONVERT_CHAN_TO_BAND,
  RFWCDMA_MC_GET_HDET_TX_AGC_VALUE,
  RFWCDMA_MC_GET_MULTI_CARRIER_WTOW_RF_CTL_BUF,
  RFWCDMA_MC_BAND_SUPPORTS_DC,
  RFWCDMA_MC_GP_CLOCK_ON,
  RFWCDMA_MC_GP_CLOCK_OFF,
  RFWCDMA_REDUCE_MTPL_FOR_THERM,
  RFWCDMA_MC_UPDATE_RXLM_BUFFER,
  RFWCDMA_MC_GET_PRIMARY_PATH,
  RFWCDMA_MC_TRIGGER_SAR_UPDATE,
  RFWCDMA_MC_SET_RX_FREEZE_ACCUM,
  RFWCDMA_MC_SET_QUIET_MODE,
  RFWCDMA_MC_SET_LOWER_POWER_MODE,
  RFWCDMA_MC_GET_PA_STATE,
  RFWCDMA_MC_SET_CPC_STATE,
  RFWCDMA_MC_FLUSH_AGC_DATA,
  RFWCDMA_MC_SET_WAKEUP_OPTIMIZATION,
  RFWCDMA_MC_CONFIG_NBR
} rfwcdma_mc_cmd_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  int16 tx_limit_dbm;
  rfcom_txplim_channel_type chan_type;
} rfwcdma_mc_tx_pwr_limit_type;
/*----------------------------------------------------------------------------*/
typedef struct
{
  uint8 val;
  rfcom_multi_carrier_id_type car_idx;
} rfwcdma_mc_tx_freezeaccum_type;
/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 arfcn;
  void *buf_ptr;
} rfwcdma_mc_wtow_ctl_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 arfcn[RFCOM_MAX_CARRIERS];
  rfcom_multi_carrier_hspa_id_type carrier_type;
  void *buf_ptr;
} rfwcdma_mc_multi_carrier_wtow_ctl_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint16 *tx_rot_ang_pa_00_p;  /* Pointer to tx rotator angle pa state 00 */
  uint16 *tx_rot_ang_pa_01_p;  /* Pointer to tx rotator angle pa state 01 */
  uint16 *tx_rot_ang_pa_10_p;  /* Pointer to tx rotator angle pa state 10 */
  uint16 *tx_rot_ang_pa_11_p;   /* Pointer to tx rotator angle pa state 11 */
} rfwcdma_mc_rot_angle_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_rxagc_type type;
  rfcom_rxagc_unit_type unit;
  rfcom_multi_carrier_id_type carr_id;
} rfwcdma_mc_rxagc_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_wcdma_band_type band;
  word chan[2];
  rfwcdma_core_uarfcn_ul_dl_type ul_dl;
} rfwcdma_mc_channel_type;
/*----------------------------------------------------------------------------*/
typedef struct 
{
 rf_path_enum_type path;                
 rfcom_multi_carrier_hspa_id_type car;
 uint16 rf_multi_chan_rx[RFCOM_MAX_CARRIERS];  
} rfwcdma_mc_path_car_state_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  uint32 buf_idx;
  boolean is_buf_allocated;
} rfwcdma_mc_rxlm_buf_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_multi_carrier_hspa_id_type num_chan;
  uint32 rxlm_buf_idx;
  uint16 channel[2];
} rfwcdma_mc_rxlm_param_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  rfcom_lpm_config_enum_type lpm_config;
  uint32 c0_rxlm_buf_idx;
  uint32 c1_rxlm_buf_idx;
} rfwcdma_mc_lpm_param_type;

/*----------------------------------------------------------------------------*/
/*!
   @brief
   Data type for device masks passed to mc to support concurrent wakeup/ tune scenarios

*/
typedef uint8 rfwcdma_device_mask_type;

/*----------------------------------------------------------------------------*/
extern uint32 rfwcdma_mc_enter_mode(rfcom_device_enum_type device,
                             const void *user_data_ptr,
                             rfm_cb_handler_type cb_handler
                             ,uint32 rxlm_buf_idx
                             );

/*----------------------------------------------------------------------------*/
extern int32 rfwcdma_mc_exit_mode(rfcom_device_enum_type device,
                                  rfcom_mode_enum_type to_mode);

/*----------------------------------------------------------------------------*/
extern uint32 rfwcdma_mc_enable_tx( rfcom_device_enum_type device,
                             const void *user_data_ptr,
                             rfm_cb_handler_type cb_handler
                             );

/*----------------------------------------------------------------------------*/
extern int32 rfwcdma_mc_disable_tx(rfcom_device_enum_type device
                                   );

/*----------------------------------------------------------------------------*/
extern uint32 rfwcdma_mc_enable_rx(rfcom_device_enum_type device,
                                   const void *user_data_ptr,
                                   rfm_cb_handler_type cb_handler);

/*----------------------------------------------------------------------------*/
uint32 rfwcdma_mc_enable_rx
(
  rfcom_device_enum_type device,
  const void *user_data_ptr,
  rfm_cb_handler_type cb_handler
);

/*----------------------------------------------------------------------------*/
int32 rfwcdma_mc_disable_rx( rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
rfcom_ioctl_return_type rfwcdma_mc_ioctl
(
  rfcom_device_enum_type device,
  rfcom_ioctl_param_type request,
  void *pbuf, int32 length
);
/*----------------------------------------------------------------------------*/
int16 rfwcdma_mc_receive_agc_val_rscp( rfcom_device_enum_type device,
                                        rfcom_rxagc_type rxagc_type,uint32 multi_carrier_idx);
/*----------------------------------------------------------------------------*/
extern uint32 rfwcdma_mc_select_band_rx(rfcom_device_enum_type device, 
                                        rfcom_wcdma_band_type band,
                                        uint32 multi_carrier_idx,
                                        uint32 rxlm_buf_idx,
                                        rfcom_wcdma_band_type source_band);

/*----------------------------------------------------------------------------*/
extern uint32 rfwcdma_mc_select_band_tx(rfm_device_enum_type device, 
                                        rfcom_wcdma_band_type band,
                                        uint32 txlm_handle);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_init
(
  rfcom_device_enum_type device, 
  boolean common_init_successful,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
/*

*/
extern void rfwcdma_mc_init_once(void);
/*----------------------------------------------------------------------------*/
extern boolean rfwcdma_mc_init_tx(rfcom_device_enum_type device, uint32 txlm_buf_idx, uint32 multi_carrier_idx,rfcom_tx_multi_carrier_cfg_type dc_hsupa_cfg);
/*----------------------------------------------------------------------------*/
extern void rfwcdma_mc_wakeup_tx(void);

/*----------------------------------------------------------------------------*/
extern void rfwcdma_mc_set_primary_path(rf_path_enum_type primary_path);

/*----------------------------------------------------------------------------*/
extern rfcom_ioctl_return_type rfwcdma_mc_ioctl( rfcom_device_enum_type device,
                                                 rfcom_ioctl_param_type request,
                                                 void *pbuf, int32 length );

/*----------------------------------------------------------------------------*/

extern int32 rfwcdma_mc_cmd_dispatch
(
  rfcom_device_enum_type device, 
  int32 cmd,
  void *data
);

/*----------------------------------------------------------------------------*/
extern void rfwcdma_mc_power_clock_on( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
extern void rfwcdma_mc_power_clock_off( rfcom_device_enum_type device );

/*----------------------------------------------------------------------------*/
int32 rfwcdma_mc_tune_to_multi_chan( rfcom_device_enum_type device,                                
                                uint16 channel[],
                                uint32 multi_carrier_idx,
                                const void *user_data_ptr,
                                rfm_cb_handler_type cb_handler
                                ,uint32 rxlm_buf_idx
                                );
/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_write_tech_dependent_dynamic_update(rfcom_tech_type curr_tech,uint8 rxlm_buf_idx[],rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_set_gtow_flag( boolean );

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_get_gtow_flag( void );

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_set_ltow_flag( boolean );

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_get_ltow_flag( void );

/*----------------------------------------------------------------------------*/

uint32 rfwcdma_mc_force_tune_to_multi_chan( rfcom_device_enum_type device,                                
                                uint16 channel[],
                                rfcom_multi_carrier_hspa_id_type num_chan,
                                rfm_synth_tune_enum_type synths,
                                const void *user_data_ptr,
                                rfm_cb_handler_type cb_handler);
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reading NV for WCDMA. 

  @details
  Retrieves all the NV from WCDMA mode. Reads common and mode specific RF 
  configuration parameters from NV. This is called as part of the 
  LOAD_RF_NV function case where we need to read the calibration NV and write 
  them without having to reset. 

  @todo Changlee Need to return nv_init_success status
  Returning TRUE is a temporary solution to avoid going to offline for other
  technologies which don't activate WCDMA critical NV items.
*/
boolean rfwcdma_mc_reload_nv
(
  rfcom_device_enum_type device,
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);
/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_set_pmic_quiet_mode(boolean en);
boolean rfwcdma_mc_set_lower_power_mode(rfcom_lpm_config_enum_type lpm_config,
                                        uint32 c0_rxlm_buf_idx,
                                        uint32 c1_rxlm_buf_idx,
                                        uint32 alt_path);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_decfg_tx(rfcom_device_enum_type device);

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------*/
rfcom_wcdma_band_type rfwcdma_mc_get_default_rfcom_band(void);
/*----------------------------------------------------------------------------*/
void rfwcdma_mc_get_default_rfi_band_and_chan(rfi_band_type *band, uint16 *chan);
/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_get_dbdc_band_support(uint16 *num_band_comb_supported, const rfm_wcdma_dual_band_type** band_comb_list);

/*----------------------------------------------------------------------------*/
uint8 rfwcdma_mc_get_car_state_info_from_device(rfcom_device_enum_type device, rfwcdma_mc_car_path_state_type car_path_state[]);

/*----------------------------------------------------------------------------*/
rfwcdma_mc_state_enum_type rfwcdma_mc_get_path_state(rf_path_enum_type path);

rfcom_multi_carrier_hspa_id_type rfwcdma_mc_get_carr_state(rfcom_device_enum_type device);

uint32 rfwcdma_mc_get_samp_rate( uint8 rx_lm_buf_idx);

/*----------------------------------------------------------------------------*/
rfwcdma_mc_state_enum_type rfwcdma_mc_get_path_state_of_device(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_get_synth_lock_status
(
  rfcom_device_enum_type device,
  rfcom_mode_enum_type rf_mode,
  rfcom_synth_type synth_type,
  void *is_synth_locked
);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_is_wcdma_supported(void);

/*----------------------------------------------------------------------------*/
uint8 rfwcdma_mc_get_nbr_phy_dev_ind(uint16 chan, rfm_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_set_use_fb_path_flag(boolean val);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_get_use_fb_path_flag(void);

/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_HAS_QTUNER
boolean rfwcdma_mc_program_qfe_scenario(void);
#endif

/*----------------------------------------------------------------------------*/
int16 rfwcdma_mc_get_rxagc
(
  rfcom_device_enum_type device,
  rfwcdma_mc_rxagc_type *data
);

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_do_temp_comp( rfm_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rfwcdma_mc_reset_state_machine(void);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_get_tri_carrier_status
(
  rfcom_device_enum_type device
); 
/*----------------------------------------------------------------------------*/
rfwcdma_mc_state_type *rfwcdma_mc_get_state(void);

/*----------------------------------------------------------------------------*/
boolean rfwcdma_mc_map_rx_device(rfm_device_enum_type rx_device);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_validate_state
(
  rfwcdma_device_mask_type device_mask,
  rfwcdma_mc_state_enum_type exp_state
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_exec_wakeup_rx_imm
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_enable_rxagc
(
  rfwcdma_device_mask_type device_mask,
  lm_handle_type rxlm_handle[RFWCDMA_MAX_RX_DEVICES]
);

/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_rx_prep_wakeup
(
 uint32 device_mask,
 uint32 rxlm_buf_idx[RFM_DEVICE_4]
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_rx_exec_wakeup
(
 uint32 device_mask,
 uint32 rxlm_buf_idx[RFM_DEVICE_4]
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_tx_retune_bw
(
  const rfm_device_enum_type device,
  const uint32 multi_carrier_idx
);
/*----------------------------------------------------------------------------*/

rfm_api_status_t
rfwcdma_mc_rx_sleep
(
 uint32 device_mask,
 boolean skip_agc_disable[RFM_DEVICE_4], 
 uint32 rxlm_buf_idx[RFM_DEVICE_4]
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_rx_prep_tune
(
  uint32 device_mask,
  rfm_wcdma_rx_tune_params *rx_tune_params
);
/*----------------------------------------------------------------------------*/
rfm_api_status_t
rfwcdma_mc_rx_exec_tune
(
 uint32 device_mask,
 uint8 carrier_mask[RFM_DEVICE_4],
 uint32 rxlm_buf_idx[RFM_DEVICE_4]
);
/*----------------------------------------------------------------------------*/
typedef struct
{
  boolean enable_nbr;
  uint32 nbr_rxlm_idx;
} rfwcdma_mc_nbr_config_param_type;
/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_config_asm_status_bit_map
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
);
/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_get_asm_status_bit
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
);
/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_config_tuner_status_bit_map
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
);
/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_get_tuner_status_bit
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
);
/*----------------------------------------------------------------------------*/
rfcom_wcdma_band_type
rfwcdma_mc_get_source_band
(
  rfcom_device_enum_type device
);
/*----------------------------------------------------------------------------*/
void
rfwcdma_mc_pwr_vote_on_off
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off,
  rfm_wcdma_rx_tx_device_type rx_tx,
  boolean irat_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_set_pwr_vote_status
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band,
  boolean on_off
);
/*----------------------------------------------------------------------------*/
boolean
rfwcdma_mc_get_pwr_vote_status
(
  rfcom_device_enum_type device,
  rfcom_wcdma_band_type band
);
/*----------------------------------------------------------------------------*/
uint16
rfwcdma_mc_get_pwr_vote_bit_map_info
(
  rfcom_device_enum_type device
);

#endif /* RFWCDMA_MC_H */
