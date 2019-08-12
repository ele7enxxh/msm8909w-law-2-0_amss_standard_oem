/*!
  @file rf_tdscdma_mc.h

  @brief
  This file contains all declarations and definitions necessary to use
  the TDSCDMA Main Control.

  @details

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/tdscdma/rf_tdscdma_mc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/15   whz     Fix TempComp on Jolokia
12/16/14   jyu     Fixed T2X/X2T/T2T tuner issue
12/05/14   jyu     Separate RF WAKEUP_STEP0 into 0p1 and 0p2 for wakeup timeline optimization
11/10/14   jyu     Add message field to indicate if use_sw_tune_in_acq and to support 
                   CMAPI type 4 (for RxD override)
09/23/14   jyu     Added support for DR-DSDS X2T IRAT to change RxLM mode inside the gap
09/16/14   jyu     Support to build home restore script (for AsDiv and Tuner)
09/16/14   jyu     Support to record WB ADC for primary and secondary logic devices
09/01/14   jyu     Need to re-build Rx LNA script when logic_device changes
08/13/14   nsh     rf_tdscdma_mc_switch_ant need to return certain value for TL1
07/17/14   ms      Sawless RX support
07/16/14   kb      Support for RFLM Rf task interface and HDET trigger API
06/20/14   jyu     Added support for DR-DSDS alt_path
05/05/14   jyu     Added support for DSDS sleep/wakeup
05/02/14   jps     TDS port switching fixes
04/28/14   jps     Support for TDS port switching
04/24/14   ych     Added dynamic tuner script for ifreq
04/12/14   jyu     Added single_valid_band into mc_state variable
03/10/14   jyu     Added support for therm read
03/05/14   jyu     Removed obsoleted FW dependency
02/23/14   jyu     Added support for better handling RF exit_mode
                   in coordination of FW resource release
02/22/14   jyu     Added support to allow RF wakeup in parallel with MCPM 
01/16/14   jyu     Added support for RF wakeup timeline optimization
12/20/13   jyu     Enabled RF_TDSCDMA_INTEGRATE_COMMON_RXAGC
12/20/13   jyu     Exposed the data strcutre for LM handles
12/04/13   whz     Expose status_update and status_check APIs
11/07/13   nsh     FED RxAGC Feature support
10/23/13   jyu     Enabled RF_TDSCDMA_INTEGRATE_DM for Bolt
10/18/13   jyu     Added feature RF_TDSCDMA_INTEGRATE_DM and 
                   RF_TDSCDMA_INTEGRATE_COMMON_TXAGC, 
                   RF_TDSCDMA_INTEGRATE_COMMON_RXAGC,
                   RF_TDSCDMA_USE_SW_TUNE
10/04/13   jps     Fixes for IQ capture in FTM calibration mode
07/26/13   ms      AsDiv support
07/17/13   ms      Support for update tune code in mission mode
07/08/13   ms      Support for program_qfe_scenario
06/10/13   ms      API for channel to band conversion for L1
06/04/13   nsh	   Support Get Multi Synth State for Dime
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
03/21/13   ms      Tuner TDET support
03/20/13   jyu     Added support to query HDET info
03/15/13   jyu     Update state variable to include device info
03/11/13   jyu     Added support to allow L1 update RxLM for L2T test case
09/19/12   ms      Diversity support for wakeup and BHO
09/10/12   ms      Diversity support for FTM
08/13/12   jyu     Updated interface to take rfm_device_enum_type and for both 
                   primary and secondary device  
07/16/12   jps     Initial FTM burst TX/RX framework
06/22/11   jhe     update
05/03/11   xw      Initial version of TDSCDMA Main Control.
===========================================================================*/

#ifndef RF_TDSCDMA_MC_H
#define RF_TDSCDMA_MC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rfcom.h"
#include "lm_types.h"
#include "rf_tdscdma_msg.h"
#include "rfmeas_types.h"
#include "rfcom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RF_TDSCDMA_RX_DIVERSITY_ENABLE

/* To integrate DM for RF script population */
#ifndef FEATURE_RFA_D3925
#define RF_TDSCDMA_INTEGRATE_DM 
#endif

/* To integrate common TxAGC (in MDSP) for TxLIN and TxAGC script update */
#define RF_TDSCDMA_INTEGRATE_COMMON_TXAGC

/* To integrate common RxAGC (in MDSP) */
#define RF_TDSCDMA_INTEGRATE_COMMON_RXAGC


/* To enable/disable SW tune option (i.e., RFSW tune both Rx and Tx PLL and keep them always ON)
   The support includes in rf_tdscdma_mc.c: 1) not to turn off Tx/Rx PLL; 2) set default TxAGC
                        in rf_tdscdma_mdsp_rf_event.c: not to turn off Tx/Rx PLL;
                        but only switch antenna ports so called, partial scripts                  */
//#define RF_TDSCDMA_USE_SW_TUNE

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*!
   @brief
   Enumeration for states for TDSCDMA mode.

*/
typedef enum
{
  /*! Power on default state */
  RF_TDSCDMA_STATE_DEFAULT,
  /*! Init state */
  RF_TDSCDMA_STATE_INIT,
  /*! Sleep state */
  RF_TDSCDMA_STATE_SLEEP,
  /*! Rx state */
  RF_TDSCDMA_STATE_RX,
  /*! RxTx state */
  RF_TDSCDMA_STATE_RXTX,
  /*! Number of states */
  RF_TDSCDMA_STATE_NUM
} rf_tdscdma_state_enum_type;

typedef enum
{
  RF_TDSCDMA_RX_LIN_STATE_LOW,
  RF_TDSCDMA_RX_LIN_STATE_HIGH,
  RF_TDSCDMA_RX_LIN_STATE_NUM
} rf_tdscdma_rx_lin_state_enum_type;

typedef enum
{
  RF_TDSCDMA_PORT_STATE_A,
  RF_TDSCDMA_PORT_STATE_B,
  RF_TDSCDMA_PORT_STATE_NUM
} rf_tdscdma_port_state_enum_type;

/*!
   @brief
   The structure maintains state variables for TD-SCDMA.

   @details
   The object of the structure gets updated when state or band changes.
   Rx channel, Tx channel are also maintained here. Tx channel is 
   maintained so that it provides clear distinction for RX and TX functionality 
   when band changes.

   Default for state = Idle state, curr_band = INVALID_BAND, primary_path = RF_PATH_0.
 
*/
typedef struct
{
  /*! @brief Maintains state of each path */
  rf_tdscdma_state_enum_type rf_state[RF_PATH_MAX];

  /*! @brief Current rx band. Default is BAND_INVALID */     
  rfcom_tdscdma_band_type curr_band_rx;

  /*! @brief Current tx band. Default is BAND_INVALID */
  rfcom_tdscdma_band_type curr_band_tx;

  /*! @brief Current RX channel */
  uint16 curr_chan_rx;

  /*! @brief Current TX channel */
  uint16 curr_chan_tx;

  /*! @brief Maintains state to be used for inverse mapping*/
  //TODO - may not need this
  rf_path_enum_type primary_path;

  /*! @brief Flag maintaining state of one time initialization */
  boolean init_once_flag;

  /*! @brief Flag indicating whether TD-SCDMA is in FTM mode*/
  //TODO - may not need this
  boolean ftm_mode;

  /*! @brief Flag indicating whether on-line pdet DC cal has been done */
  boolean pdet_dc_cal_done;

  /*! @brief Flag indicating whether secondary path is on */
  boolean div_enable;

  /*! @brief Flag indicating HDET is on */
  boolean hdet_enable;

  /*! @brief Current primary chain device info. Default is RFM_INVALID_DEVICE */
  rfm_device_enum_type    curr_pri_device;

  /*! @brief Current secondary chain device info. Default is RFM_INVALID_DEVICE */
  rfm_device_enum_type    curr_sec_device;

  /*! @brief Current rx path info. Default is RFA_TDSCDMA_RX_PATH_PRIM */
  rfa_tdscdma_rx_path_t   curr_rx_path;

  /*! @brief Last rx channel before sleep */
  uint16 last_rx_chan_before_sleep;

  /*! @brief HDET info */
  uint8 hdet_info;

  /*! @brief Device wakeup script built flag */
  uint8 device_wakeup_script_built_flag;

  /*! @brief Device wakeup script exec flag */
  uint8 device_wakeup_script_exec_flag;

  /*! @brief One single valid band derived from BC_CONFIG NV */
  rfcom_tdscdma_band_type single_valid_band;

  /*! @brief port state */
  rf_tdscdma_port_state_enum_type port_state;

  /*! @brief alt_path information per path */
  uint8 alt_path[RF_PATH_MAX];

  /*! @brief alt_path information per path for those that have been used for IFREQ */
  uint8 alt_path_used_to_build_ifreq[RF_PATH_MAX];

  rf_tdscdma_rx_lin_state_enum_type rx_lin_state;

  /*! @brief the last rxLM handle used to build AOL update script */
  uint8 last_rxlm_handle_for_AOL_update;

  /*! @brief the last logic primary device used to build Rx LNA script */
  rfm_device_enum_type    last_pri_device_used_to_build_rx_lna_scpt;

  /*! @brief the flag whether Rx LNA script has been built on RFCOM TDSCDMA bands */
  boolean rx_lna_script_built_on_bands[RFCOM_NUM_TDSCDMA_BANDS];

  /*! @brief serving cell WB ADC index */
  uint8 scell_wb_adc_index_on_devices[RF_PATH_MAX];

  /*! @brief neighbor cell WB ADC index */
  uint8 ncell_wb_adc_index_on_devices[RF_PATH_MAX];

  /*! @brief current antenna switch position */
  uint32 curr_ant_sw_pos;

  /*! @brief next antenna switch position */
  uint32 next_ant_sw_pos;
} rf_tdscdma_mc_state_type;

typedef struct
{
  boolean is_buf_allocated;
  lm_handle_type buf_idx;
  rfa_tdscdma_rxlm_mode_t mode;
} rf_tdscdma_mc_rxlm_buf_type;

typedef struct
{
  boolean is_buf_allocated;
  lm_handle_type buf_idx;
} rf_tdscdma_mc_txlm_buf_type;

typedef struct
{
  rf_tdscdma_mc_rxlm_buf_type rx_buf_wrk[2];
  rf_tdscdma_mc_rxlm_buf_type rx_buf_tgt[2];
  rf_tdscdma_mc_txlm_buf_type tx_buf_wrk;
  rf_tdscdma_mc_txlm_buf_type tx_buf_tgt;
} rf_tdscdma_mc_lm_buf_type;

extern rf_tdscdma_mc_state_type rf_tdscdma_mc_state;
extern boolean is_wtr_hdet;
extern boolean rf_tdscdma_use_on_rumi;

extern rf_tdscdma_mc_lm_buf_type rf_tdscdma_mc_lm_bufs;
#if 0
extern rf_tdscdma_port_state_enum_type rf_tdscdma_mc_port_state;
#endif

typedef enum
{
  RF_TDSCDMA_MC_SUCCESS,
  RF_TDSCDMA_MC_SM_ERROR,
  RF_TDSCDMA_MC_INVALID_PARAM,
  RF_TDSCDMA_MC_STATUS_MAX,
} rf_tdscdma_mc_status_type;

typedef struct
{
  uint8 rx_wrk_buf_idx;
  uint8 rx_tgt_buf_idx;
  uint8 tx_wrk_buf_idx;
  uint8 tx_tgt_buf_idx;
} rf_tdscdma_mc_dbl_buf_type;

extern rf_tdscdma_mc_dbl_buf_type rf_tdscdma_mc_dbl_bufs;

typedef enum
{
  TDSCDMA_RFMODE_CMD_RSP_IDLE = 0,
  TDSCDMA_RFMODE_CMD_RSP_PENDING,
  TDSCDMA_RFMODE_CMD_RSP_SUCCESS,
  TDSCDMA_RFMODE_CMD_RSP_FAILURE,
  TDSCDMA_RFMODE_CMD_RSP_MAX
} rf_tdscdma_rfmode_cmd_rsp_status_type;


typedef enum
{

  TDSCDMA_RFMODE_TX_CMD_RSP_IDLE = 0,
  TDSCDMA_RFMODE_TX_CMD_RSP_PENDING,
  TDSCDMA_RFMODE_TX_CMD_RSP_SUCCESS,
  TDSCDMA_RFMODE_TX_CMD_RSP_FAILURE,
  TDSCDMA_RFMODE_TX_CMD_RSP_MAX
} rf_tdscdma_rfmode_tx_cmd_rsp_status_type;

typedef enum
{
  TDSCDMA_RFMODE_RX_CMD_RSP_IDLE = 0,
  TDSCDMA_RFMODE_RX_CMD_RSP_PENDING,
  TDSCDMA_RFMODE_RX_CMD_RSP_SUCCESS,
  TDSCDMA_RFMODE_RX_CMD_RSP_FAILURE,
  TDSCDMA_RFMODE_RX_CMD_RSP_MAX
} rf_tdscdma_rfmode_rx_cmd_rsp_status_type;

typedef enum
{
  TDSCDMA_RFMODE_CONT_TX_DEFAULT_SUBID = 0,
  TDSCDMA_RFMODE_CONT_TX_HDET_RECONFIG_SUBID,
  TDSCDMA_RFMODE_CONT_TX_THERM_RECONFIG_SUBID,
  TDSCDMA_RFMODE_CONT_TX_RES1_SUBID,
  TDSCDMA_RFMODE_CONT_TX_RES2_SUBID,
  TDSCDMA_RFMODE_CONT_TX_RES3_SUBID,
  TDSCDMA_RFMODE_CONT_TX_RES4_SUBID,
  TDSCDMA_RFMODE_CONT_TX_RES5_SUBID,
  TDSCDMA_RFMODE_CONT_TX_MAX_SUBID
} rf_tdscdma_rfmode_cont_tx_msg_subid_type;

typedef struct
{
  uint8 num_slots;
  uint16 mod_type;
  int16 tspwrlvl[5]; /* in dBmx10 units */
  int32 freqOffset;
  uint16 sf;
  int16 pdet_thresh; /* in dBmx10 units */
  uint16 pdet_trig_interval; /* in ms units */
  uint16 ovsf_code; 
  boolean conTX_tx_reconfig_block; /* Flag to block TX reconfig MSGS in cont TX */

} rf_tdscdma_ftm_burst_tx_params_type;

typedef struct
{
  uint8 num_slots;
  boolean rx1_active;
  boolean tracking_enable;
  int16 exp_rx_lvl;
  uint16 computed_lna_state;
  uint16 computed_lna_state_rx1;
} rf_tdscdma_ftm_burst_rx_params_type;



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
boolean rf_tdscdma_l1_msgr_register( msgr_client_t *client_id, msgr_id_t id );

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_l1_msgr_deregister( msgr_client_t *client_id );

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_msgr_rflm_register(void);

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_msgr_rflm_deregister(void);

/*----------------------------------------------------------------------------*/
 #ifdef FEATURE_RF_ASDIV

rf_tdscdma_mc_status_type rf_tdscdma_mc_switch_ant(rfm_device_enum_type device,
                         uint8 to_pos,
                         void *cb_func,
                         void *cb_data,
                         boolean is_immediate);
/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_switch_ant_abort
(
   rfm_device_enum_type device,
   uint8 to_pos,
   void *cb_func,
   void *cb_data
);


#endif
/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enter_mode
( 
   rfm_device_enum_type pri_device,
   rfm_device_enum_type sec_device,
   lm_handle_type rxlm_buffer_idx,
   rfa_tdscdma_rxlm_mode_t rxlm_mode,
   boolean is_dsds
);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_exit_mode
( 
   rfm_device_enum_type pri_device,
   rfm_device_enum_type sec_device,
   boolean wait_for_fw_release
);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enable_rx
( 
   rfm_device_enum_type pri_device,
   rfm_device_enum_type sec_device,
   uint16 rx_channel,
   lm_handle_type rxlm_buffer_idx,
   rfa_tdscdma_rxlm_mode_t rxlm_mode,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
   rfa_tdscdma_rx_path_t  rx_path,
   lm_handle_type          rxlm_buffer_idx_div,
   rfa_tdscdma_rxlm_mode_t rxlm_mode_div,
#endif
   uint32 *carrier_freq,
   uint8 *rx_wrk_buffer_idx,
   boolean is_dsds,
   uint8 rx_lin_state,
   boolean use_sw_tune_in_acq
);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_init_tx(void);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_enable_tx( rfm_device_enum_type device,                                                   
                                                   uint16 tx_channel,
                                                   lm_handle_type txlm_buffer_idx,
                                                   uint8 *tx_wrk_buffer_idx );

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_disable_tx( rfm_device_enum_type device );

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_sleep( rfm_device_enum_type pri_device,
                                               rfm_device_enum_type sec_device);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step0p1
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type          rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div  
#endif 
  ,boolean* rebuild_ifreq_script
);

rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step0p2
(
  rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type          rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div  
#endif 
  ,boolean* rebuild_ifreq_script
);

rf_tdscdma_mc_status_type rf_tdscdma_mc_wakeup_step1(rfm_device_enum_type pri_device,
  rfm_device_enum_type sec_device,
  uint16 rx_channel,
  lm_handle_type rxlm_buffer_idx,
  rfa_tdscdma_rxlm_mode_t rxlm_mode
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
  ,rfa_tdscdma_rx_path_t   rx_path,
  lm_handle_type          rxlm_buffer_idx_div,
  rfa_tdscdma_rxlm_mode_t rxlm_mode_div

#endif 
);

/*----------------------------------------------------------------------------*/
uint16 rf_tdscdma_mc_get_rf_warmup_time(void);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_tx_pwr_limit( int16 tx_pwr_limit );

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_update_bho( rfm_device_enum_type pri_device,
                                                    rfm_device_enum_type sec_device,
                                                    uint16 tgt_prim_channel,
                                                    uint16 tgt_channel,
                                                    lm_handle_type rxlm0_tgt_buffer_idx,
                                                    rfa_tdscdma_rxlm_mode_t rxlm_mode,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                                   rfa_tdscdma_rx_path_t  rx_path,
                                                   lm_handle_type rxlm_tgt_buffer_idx_div,
                                                   rfa_tdscdma_rxlm_mode_t rxlm_mode_div,
#endif
                                                    lm_handle_type txlm_tgt_buffer_idx,
                                                    uint32 *prim_carrier_freq,
                                                    uint32 *carrier_freq,
                                                    uint8 *rx_tgt_buffer_idx,
                                                    uint8 *tx_tgt_buffer_idx );

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_switch_buffer( rfa_tdscdma_buf_swap_t buf_swap,
                                                       lm_handle_type rxlm0_tgt_buffer_idx,
#ifdef RF_TDSCDMA_RX_DIVERSITY_ENABLE
                                                       lm_handle_type rxlm_tgt_buffer_idx_div,
#endif

                                                       lm_handle_type txlm_tgt_buffer_idx );

/*----------------------------------------------------------------------------*/
int16 rf_tdscdma_mc_get_max_tx_pwr(void);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_therm_backoff(uint8 backoff);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_coex_tx_pwr_limit(int16 sar_tx_pwr_limit,
                                                              int16 coex_tx_pwr_limit);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_proc_hdet_reading(uint16 num_ssbi, int16 mdsp_txagc, uint8 raw_reading[]);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_proc_therm_reading(uint16 num_ssbi, uint8 raw_reading[], boolean skip_calc, uint16 therm_read);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_trigger_hdet(int16);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_trigger_therm();

#if 0
/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_meas_update_tds_nbr( uint8 num_tds_nbr,
                                                             rfa_tdscdma_nbr_entry_t tds_nbr_list[],
                                                             uint32 carrier_freq_list[],
                                                             lm_handle_type ifreq_rxlm_buf_idx,
                                                             rfa_tdscdma_rxlm_mode_t ifreq_rxlm_mode );

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_meas_update_gsm_nbr( uint8 num_gsm_nbr,
                                                             rfa_tdscdma_nbr_entry_t gsm_nbr_list[],
                                                             lm_handle_type t2g_rxlm_buf_idx );
#endif


/*----------------------------------------------------------------------------*/
void rf_tdscdma_msg_dispatch_register(void);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_msg_dispatch_deregister(void);

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_mc_init(boolean common_init_success, 
                           rex_tcb_type *caller_tcb_ptr,
                           rex_sigs_type task_nv_wait_sig,
                           void (*task_wait_func_ptr)( rex_sigs_type ));

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_validate_msg_state(uint32 umid);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_update_msg_state(uint32 umid);

/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_HAS_QTUNER
void rf_tdscdma_mc_proc_tuner_therm_reading
(
   uint16 numSsbi,
   uint8 address[],
   uint8 measurement[]
);
#endif

/*---------------------------------------------------------------------------- 
  The following functions are designed for FTM (Factory Test Mode) 
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
boolean rf_tdscdma_mc_reload_nv
(
  rex_tcb_type *caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (*task_wait_func_ptr)( rex_sigs_type )
);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_set_bursty_mode_flag(boolean set);

void rf_tdscdma_mc_update_rxlm_mode(uint16 rx_channel, lm_handle_type rxlm_buffer_idx, rfa_tdscdma_rxlm_mode_t rxlm_mode);

void rf_tdscdma_mc_update_irat_rxlm_mode
(
  rfm_device_enum_type device,
  uint16 rx_channel, 
  lm_handle_type rxlm_buffer_idx, 
  rfa_tdscdma_rxlm_mode_t rxlm_mode
);

/*----------------------------------------------------------------------------*/
rf_tdscdma_mc_status_type rf_tdscdma_mc_set_sm_rfmode_tx(rfcom_device_enum_type device);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_burst_tx
(
  boolean on_off, 
  rf_tdscdma_ftm_burst_tx_params_type tx_params, 
  uint32 txlm_bufidx,
  rf_tdscdma_rfmode_cont_tx_msg_subid_type subID
);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_burst_rx
(
  boolean on_off, 
  rf_tdscdma_ftm_burst_rx_params_type rx_params, 
  uint32 rxlm_bufidx,
  uint32 rxlm_bufidx_div
);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_proc_burst_tx_rsp
(
  boolean status,
  boolean reset_config, 
  uint32 rsp_subID
);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_proc_iq_capture_rsp
(
  boolean status
);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_proc_burst_rx_rsp
(
  boolean status
);

/*----------------------------------------------------------------------------*/
rf_tdscdma_rfmode_tx_cmd_rsp_status_type rf_tdscdma_mc_get_burst_tx_cmd_status(void);

/*----------------------------------------------------------------------------*/
rf_tdscdma_rfmode_rx_cmd_rsp_status_type rf_tdscdma_mc_get_burst_rx_cmd_status(void);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_set_burst_tx_cmd_status(rf_tdscdma_rfmode_tx_cmd_rsp_status_type status);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_set_burst_rx_cmd_status(rf_tdscdma_rfmode_rx_cmd_rsp_status_type status);
void rf_tdscdma_mc_get_multi_synth_rsp(uint8 cmd_status, uint8 rx_pll_status, 
								       uint8 tx_pll_status, uint8 is_cont_tx_valid);
/*----------------------------------------------------------------------------*/
rf_tdscdma_rfmode_cmd_rsp_status_type  rf_tdscdma_mc_get_multi_synth_rsp_status(void);
void rf_tdscdma_mc_set_multi_synth_rsp_status(rf_tdscdma_rfmode_cmd_rsp_status_type status);
void rf_tdscdma_mc_get_multi_synth_rsp(boolean cmd_status, uint8 rx_pll_status, uint8 tx_pll_status, boolean is_cont_tx_valid);
uint8 rf_tdscdma_mc_get_rx_pll_status(void);
uint8 rf_tdscdma_mc_get_tx_pll_status(void);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_rf_mode_temp_comp_handler(void);

/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_rf_mode_hdet_comp_handler(void);

/*----------------------------------------------------------------------------*/
uint32 rf_l1_tdscdma_get_band_from_uarfcn(uint32 chan);

/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_HAS_QTUNER
boolean rf_tdscdma_mc_program_qfe_scenario(void);
#endif
/*----------------------------------------------------------------------------*/
#ifdef FEATURE_RF_HAS_QTUNER
void rf_tdscdma_mc_update_tuner_code(boolean is_immediate);
#endif

 #ifdef FEATURE_RF_ASDIV
void rf_tdscdma_mc_send_switch_ant_msgr
(
   rfm_device_enum_type device,
   uint8 to_pos,
   void *cb_func,
   void *cb_data,
   boolean action_immediate
);
/*----------------------------------------------------------------------------*/
void rf_tdscdma_mc_send_switch_ant_abort_msgr
(
   rfm_device_enum_type device,
   uint8 to_pos,
   void *cb_func,
   void *cb_data
);

#endif

void rf_tdscdma_mc_asdiv_script(uint32 ant_position,rfm_device_enum_type pri_device,rfcom_tdscdma_band_type band);

#ifdef RF_TDSCDMA_INTEGRATE_COMMON_RXAGC

void rf_tdscdma_mc_update_rflm_rxagc
(
   rfm_device_enum_type device,
   rf_path_enum_type path, 
   rfcom_tdscdma_band_type band,
   uint16 rx_channel,
   lm_handle_type rxlm_buffer_idx,
   rfa_tdscdma_rxlm_mode_t rxlm_mode,
   boolean is_ifreq,
   uint8 ifreq_index,
   uint8 buf_id,
   boolean is_irat
);

void rf_tdscdma_mc_rflm_init_rxagc(int32 rxagc_handle);



#endif


boolean rf_tdscdma_mc_is_coex_backoff_req (rfm_device_enum_type rfm_dev);

rf_tdscdma_mc_status_type rf_tdscdma_mc_dynamic_script(uint32 ifreq_meas_freq, lm_handle_type ifreq_buf_idx);

boolean rf_tdscdma_mc_dynamic_port_switch(uint8 port);

boolean rf_tdscdma_mc_is_rx_sawless(void);

rf_tdscdma_mc_status_type rf_tdscdma_mc_rxd_override
( 
  boolean rxd_override
);


#ifdef __cplusplus
}
#endif

#endif /* RF_TDSCDMA_MC_H */


