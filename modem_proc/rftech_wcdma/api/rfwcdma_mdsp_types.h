#ifndef RFWCDMA_MDSP_TYPES_H
#define RFWCDMA_MDSP_TYPES_H

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-mDSP AGC interface.
*/

/*===========================================================================


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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp_types.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
03/12/15   gh      Added support to set and get RFLM THERM_ENABLE state
12/12/14   rmb     Fix compilation error.
11/4/14    ac      ftm flag to denote the txagc state 
09/09/14   ak      Added support for TQ IDLE check before TQ release
10/28/14   zhh     define rflm tx and rx carriers with WFW macro
10/01/14   rmb     Increase CCS tasks from 10 to 14 for WCDMA Events.
07/23/14   dw      Added LNA dependent RSB programming support
07/21/14   vbh     Add a mdsp set data command to indicate RxAGC config is in progress
06/25/14   bn      Therm bin in RFLM logging
06/06/14   ac      NBR support
05/15/14   aro     Added commands to set and get the params related to
                   power meter
05/13/14   vbh     WFW RF interface cleanup
04/28/14   dw      Add support for set frequency adjust 
04/18/14   rmb     Use typedef to re define rflm ENUM as mdsp ENUM.
04/14/14   rmb     Add L2W FTM IRAT sync commands.
04/11/14   vbh     Defined set data ID for RFWCDMA_MDSP_DISABLE_TxAGC
04/09/14   dw      Add Rx Cal mode flag in AGC command for NBEE
04/04/14    gh     Augment max allowable CCS tasks per CCS event
04/03/14   dw      Add dynamic RxLM update support
03/03/14    dw     Added #define for RF_WCDMA_DEFAULT_LNA_STATE
02/25/14   vbh     Lower the RF_WCDMA_AGC_POLLING_INTERVAL to 20us
02/04/14   kai     Add xpt logging support
01/08/14    dw     Add mdsp/FED support for wakeup-tune config
01/02/14    dw     Added run time LNA switchpoint update support
12/13/13    dw     Merge type definition between RFLM and RFWCDMA_MDSP
12/14/13    dw     Add get RxAGC support
10/28/13    dw     Add LNA state logging support
10/30/13    ka     Added rx agc debug logging
10/15/13    dw     [1]Move LM handle ID to set/get data header
10/11/13   kai     Add Macro to convert TxAGC into dBm10
10/10/13    dw     Add invalid PA switchpoint definition
10/09/13    ka     Update tech buffer data types
09/24/13    dw     [1] Added lna_gain_state_oride variable
                   [2] Added type definitions for set_data and get_data
09/19/13    dw     Export event ID definition to LM API
09/18/13    dw     Added support for LM handle based event script generation
09/16/13    dw     Remove rflm_tech_id_t redefinition
09/16/13    dw     [1] Added support for FED DM tech buffer
                   [2] Added FTM Cal mode support
09/09/13    dw     More updates for Bolt
08/29/13    dw     Update with FED DM support
08/23/13    dw     Initial Verision
==============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "npa.h"
#include "comdef.h"
#include "wfw_sw_intf.h"
#include "wfw_mod_intf.h"
#include "wfw_drx_intf.h"
#include "wfw_rxagc_intf.h"
#include "wfw_misc_intf.h"
#include "rf_buffer_intf.h"
#include "rfcommon_mdsp_event_intf.h"
#include "rflm_dm_api.h"
#include "rxlm_intf.h"
#include "txlm_intf.h"
#include "rflm_api_wcdma.h"
#include "rflm_wcdma_sw_api.h"
#include "rflm_rxagc_api.h"
#include "rfcom.h"

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of LNA states.
    Need to be in sync with FED definition
*/
#define RF_WDMA_MAX_NUM_LNA_STATES                    RFLM_WDMA_MAX_NUM_LNA_STATES

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of PA states.
    Need to be in sync with FED definition
*/
#define RF_WCDMA_MAX_NUM_PA_STATES                  RFLM_WCDMA_MAX_NUM_PA_STATES
  
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of TX uplink carriers
    Need to be in sync with FED definition 
*/
#define RF_WCDMA_MAX_NUM_TX_CARRIERS              WFW_NUM_MAX_UPLINK_CARRIERS 


/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of RX downlink carriers
    Need to be in sync with FED definition 
*/
#define RF_WCDMA_MAX_NUM_RX_CARRIERS              WFW_RXAGC_MAX_CARRIERS


/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of TX Tx MPR(max power ratio) bins
    Need to be in sync with FED definition 
*/
#define RF_WCDMA_MAX_NUM_MPR_BINS                    RFLM_WCDMA_MAX_NUM_MPR_BINS

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of antennas/Rx chains
    Need to be in sync with FW definition 
*/
#define RF_WCDMA_MAX_NUM_ANTENNAS                        WFW_RXAGC_MAX_ANTENNAS

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of physical chains allowed
    for WCDMA. 
*/
#define RF_WCDMA_MAX_NUM_CHAINS                                               4

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of LNA device per antenna
    Need to be in sync with FW definition  
*/
#define RF_WCDMA_MAX_NUM_LNA_DEVICES_PER_ANT      WFW_RXAGC_MAX_LNAS_PER_ANTENNA
 
/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of CCS events
    Need to be in sync with FW definition 
*/
//#define RF_WCDMA_MAX_NUM_CCS_EVENTS                            WFW_RF_EVENT_NUM

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum number of tasks for the given CCS event
*/
#define RF_WCDMA_MAX_NUM_CCS_TASKS               RFLM_CCS_RF_MAX_NUM_CCS_TASKS

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines the maximum 32 bit event ID after applying teck and LM
    mask
*/
#define RF_WCDMA_MAX_EVENT_ID                                        0xFFFFFFFF

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines Polling interval after AGC command in us 
*/
#define RF_WCDMA_AGC_POLLING_INTERVAL                                       20

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines Polling timeout after AGC command in us
*/
#define RF_WCDMA_AGC_POLLING_TIMEOUT                                      10000

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines maximum invalid PA switchpoints
*/
#define RF_WCDMA_INVALID_PA_THRESH                                        32767

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines maximum number of RF protocol bus type (RFFE,SSBI,GRFC)
*/
#define RF_WCDMA_MAX_NUM_BUS_TYPE                   RFLM_WCDMA_MAX_NUM_BUS_TYPE  

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines default WTR LNA state to be initialized before CGAGC start
*/
#define RF_WCDMA_DEFAULT_LNA_STATE           RFLM_WCDMA_CGAGC_DEFAULT_LNA_STATE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to valid-range check if RxlM handle is valid
*/
#define IS_VALID_RXLM_HANDLE(h)  ((h >= 0)&&(h< RXLM_MAX_BUFFERS))? TRUE: FALSE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to valid-range check if TxlM handle is valid 
*/
#define IS_VALID_TXLM_HANDLE(h)  ((h >= 0)&&(h< TXLM_MAX_BUFFERS))? TRUE: FALSE

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Macro to convert TxAGC into dBm10 
*/
#define TXAGC_TO_DBM10(v) ((v) + 512 - 700)

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines Polling interval for TQ IDLE in us
*/
#define RF_WCDMA_TQ_POLLING_INTERVAL                                       25

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    This number defines maximum polling time for TQ IDLE in us
*/
#define RF_WCDMA_TQ_POLLING_TIMEOUT                                       150

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
  RF_WCDMA_DRX_ENABLE,

  /*! Rx is resumed and bring out of low power mode */
  RF_WCDMA_DRX_DISABLE,

  /*! Number of Rx DRx control */
  RF_WCDMA_DRX_CTL_NUM,

  /*! DRx control max  */
  RF_WCDMA_DRX_CTL_MAX = RF_WCDMA_DRX_CTL_NUM
} rfwcdma_mdsp_drx_event_type;


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
  RF_WCDMA_DTX_ENABLE,

  /*! Tx PLL is retuned and bring out of low power mode */
  RF_WCDMA_DTX_DISABLE,

  /*! Number of Tx DTx control */
  RF_WCDMA_DTX_CTL_NUM,

  /*! DTx control max  */
  RF_WCDMA_DTX_CTL_MAX = RF_WCDMA_DTX_CTL_NUM
} rfwcdma_mdsp_dtx_event_type;

/*----------------------------------------------------------------------------*/
/*! 
    @brief
    Enum definition of RF WCDMA MDSP SYNC CMD ID type
 
    @details
    This is internal RF MDSP sync command ID definition. In mdsp sync module
    it will be map to the actual FW sync command ID

*/
typedef enum
{
  /*! Command Id to send RxAGC command */
  RFWCDMA_MDSP_RXAGC_CMD_ID,

  /*! Command Id to init G2W mdsp interface in G2W scenario*/
  RFWCDMA_MDSP_G2W_INIT_CMD_ID,

  /*! Command Id to start WFW in G2W mode*/
  RFWCDMA_MDSP_G2W_START_CMD_ID,

   /*! Command Id to config and tune to neighbour W cell in G2W */
  RFWCDMA_MDSP_G2W_CONFIG_TUNE_CMD_ID,

  /*! Command Id to suspend and resume WFW in W2X */
  RFWCDMA_MDSP_W2X_SUSPEND_RESUME_CMD_ID,

  /*! Command Id to send Sleep command to WFW in G2W*/
  RFWCDMA_MDSP_G2W_SLEEP_CMD_ID,

  /*! Command Id to send RxLM dynamic update command */
  RFWCDMA_MDSP_RXLM_DYN_UPDATE_CMD_ID,

  /*! Command Id to init L2W mdsp interface in L2W scenario */
  RFWCDMA_MDSP_L2W_INIT_CMD_ID,

  /*! Command Id to start WFW in L2W Dedicated mode 
    and send Tune command neighbour W cell in L2W*/
  RFWCDMA_MDSP_L2W_START_AND_TUNE_CMD_ID,

  /*! Command Id to send Stop command to WFW in L2W*/
  RFWCDMA_MDSP_L2W_STOP_CMD_ID,

  /*! Command Id to deinitialize L2W. This should be the final step in L2W*/
  RFWCDMA_MDSP_L2W_DEINIT_CMD_ID,

  /*! Maximum Command IDs */
  RFWCDMA_MDSP_CMD_ID_MAX
}rfwcdma_mdsp_sync_send_cmd_id;
/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum definition of AGC Command type
 
*/
typedef enum {
  RFWCDMA_MDSP_AGC_CMD_NOOP,
  RFWCDMA_MDSP_AGC_CMD_DISABLE,
  RFWCDMA_MDSP_AGC_CMD_ENABLE,
  RFWCDMA_MDSP_AGC_CMD_INVALID
}rfwcdma_mdsp_agc_action_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum definition of RF WCDMA IRAT technology type
 
*/
typedef rflm_wcdma_irat_meas_enum_type rfwcdma_mdsp_rx_irat_meas_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of channel specific Rx config parameters
 
*/
typedef rflm_wcdma_rx_cfg_chsp_type rfwcdma_mdsp_rx_cfg_chsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific Rx config parameters
 
*/
typedef rflm_wcdma_rx_cfg_bsp_type rfwcdma_mdsp_rx_cfg_bsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Rx wakeup config parameters
 
*/
typedef rflm_wcdma_rx_wakeup_cfg_type rfwcdma_mdsp_rx_wakeup_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Rx IQ mismatch compensation paramters
*/
typedef rflm_wcdma_rx_iqmc_param_type rfwcdma_mdsp_rx_iqmc_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx MPR(max power ratio) bin based backoff paramters
  for HS waveforms 

*/
typedef rflm_wcdma_mpr_param_type rfwcdma_mdsp_mpr_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of XPT paramters

*/
typedef rflm_wcdma_xpt_param_type rfwcdma_mdsp_xpt_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific Tx config parameters
 
*/
typedef rflm_wcdma_tx_cfg_bsp_type rfwcdma_mdsp_tx_cfg_bsp_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx multi-carrier configuration parameters
 
*/
typedef rflm_wcdma_tx_mc_cfg_type rfwcdma_mdsp_tx_mc_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of channel specific Tx config parameters
 
*/
typedef rflm_wcdma_tx_cfg_chsp_type rfwcdma_mdsp_tx_cfg_chsp_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of Tx device configuration needed for FW TxAGC
 
*/
typedef rflm_wcdma_tx_device_cfg_type rfwcdma_mdsp_tx_device_cfg_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of band specific PA parameters

*/
typedef rflm_wcdma_pa_band_param_type rfwcdma_mdsp_pa_band_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of device configuration needed for FW operation
 
  @details
  Device configuration includes
  1. device transaction script executed in FW context
  2. Timing parameters related to the script transactions
 
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
typedef rflm_wcdma_device_script_type rfwcdma_mdsp_device_script_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  WCDMA FED DM interface structure
 
  @details
  This is the meta structure of RF MDSP-FW data structure. Since most of the
  pamrameters are moved to FED DM, the data structure will be copied to DM as
  is (excluding device scripts) based on LM handle
  
*/
typedef rflm_wcdma_dm_buf_type rfwcdma_mdsp_dm_buf_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RxAGC sync command paramters
 
*/
typedef struct
{
  /*! Specifies RxAGC command action per antenna/carrier*/
  rfwcdma_mdsp_agc_action_type agc_action[RF_WCDMA_MAX_NUM_ANTENNAS][RF_WCDMA_MAX_NUM_RX_CARRIERS];

  /*! RxLM buffer indices per AGC. One to one mapping to the AGC action array */
  uint32 rxlm_buf_idx[RF_WCDMA_MAX_NUM_ANTENNAS][RF_WCDMA_MAX_NUM_RX_CARRIERS];

  /*! < Total RF warmup time needed for the completion of SSBI execution + PLL tune time
     SSBI includes two parts
     1 - AutoB group of SSBIs (happen before PLL settling)
     2 - AutoA group of SSBIs (happen in parallel with PLL settling)
     This time is execTime(AutoB) + max(execTime(AutoA), PLL_settling_time)
     This time is ONLY used if rf_warmup_en flag is set */
  uint16 rf_warmup_time;

  /*! < Flag to indicate the AGC start during wake-up. FW will send the SSBI/RFFE transactions instead of RF*/
  boolean rf_warmup_en;

  /*! < Flag to indicate AGC cmd in calibration mode i.e. AGC will not be running, only RxFE will be configured */
  boolean cal_mode;

   /*! @ brief NBR feature enabling
       0: disabled
       1: enabled - nbr should be running as part of rxagc start cmd processing */
  uint8 nbr_en_flag;                 

  /*! @ brief rxlmBufIndex for modified G chain */
  uint8 nbr_rxlm_buf_idx;

}rfwcdma_mdsp_agc_cmd_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RxLM dynamic update command pamameters
 
*/
typedef struct
{
  /*! Specifies RxLM buffer indices */
  uint8 rxlm_buf_idx[RF_WCDMA_MAX_NUM_CHAINS];

  /*! Specifies DTR group mask */ 
  uint32 dtr_group_mask;

  /* Specified RxLM chain mask */
  uint32 chain_mask;

}rfwcdma_mdsp_dyn_rxlm_cmd_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RF-MDSP CCS event type
 
*/
typedef struct
{
  /*! Specifies event handle*/
  rfcommon_mdsp_event_handle* event_handle;

  /*! Specified interface pointer to push the event to in FED DM/shared memory per LM handle*/
  void* event_intf_ptr[RXLM_MAX_BUFFERS];

  /*! Specifies event settings */
  rfcommon_mdsp_event_settings_type event_param;

  /*! Specifies number of tasks in the event */
  uint8 num_tasks;

  /*! Specifies task transaction settings for all the tasks in the event */
  rfcommon_mdsp_transaction_settings_type tran_param[RF_WCDMA_MAX_NUM_CCS_TASKS];

  /*! Specifies max size of the event */
  uint16 event_max_size;

  /*! Specifies the number of counts the event have been built */
  uint16 event_count;

  /*! script type associated with RFLM DM */
  rflm_dm_script_type_e dm_script_type;

  /*! Flag to indicate whether the event is Rx or Tx */
  boolean rx_event;

}rfwcdma_mdsp_ccs_event_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RF-MDSP CCS event unique setting type
 
  @details
  most of settings in common mdsp ccs event setting type are generic for all
  WCDMA CCS events. Initialized template used for all events. This structure
  catpures the unique ccs event settings for WCDMA, the settings should be
  a subset of rfwcdma_mdsp_ccs_event_type
 
*/
typedef struct
{
  /*! If the event is atomic (issue_seq)*/
  boolean atomic;

  /*! event priority */
  uint8 event_priority;

  /*! maximum size of the event */
  uint16 event_max_size;

  /*! script type associated with RFLM DM */
  rflm_dm_script_type_e dm_script_type;

  /*! Flag to indicate whether the event is Rx or Tx */
  boolean rx_event;
    
}rfwcdma_mdsp_ccs_event_unique_setting_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum definition for generic set data command ID .
 
*/

typedef enum
{
  /*! Set dvga gain offset  */
  RFWCDMA_MDSP_SET_VGA_GAIN_OFFSET,

  /*! Set lna gain offset */
  RFWCDMA_MDSP_SET_LNA_GAIN_OFFSET,

  /* Override RxAGC LNA state in FED */
  RFWCDMA_MDSP_SET_LNA_GAIN_STATE,

  /* Override LNA switchpoint */
  RFWCDMA_MDSP_SET_LNA_SWITHPOINT,

  /*! Set frequency error compensation */
  RFWCDMA_MDSP_SET_FREQ_ADJUST,

  /*! Set TxAGC */
  RFWCDMA_MDSP_DISABLE_TxAGC,

  /*! Update ET delay */
  RFWCDMA_MDSP_UPDATE_ET_DELAY,

  /*! Set Power meter Enable flag */
  RFWCDMA_MDSP_SET_POWER_METER_ENABLE_FLAG,

  /*! Set Power meter periodicity */
  RFWCDMA_MDSP_SET_POWER_METER_PERIODICITY,

  /*! Set Power meter state */
  RFWCDMA_MDSP_SET_POWER_METER_STATE,

  /*! Set Pin Pout values */
  RFWCDMA_MDSP_SET_PIN_POUT,

  /*! Set MPR values */
  RFWCDMA_MDSP_SET_MPR,

  /*! Set Therm values */
  RFWCDMA_MDSP_SET_THERM,
  
  /*! Set AGC config flag*/
  RFWCDMA_MDSP_CONFIG_RxAGC_IN_PROGRESS,
  
  /*! Set IQ mismatch parameters  */
  RFWCDMA_MDSP_SET_RX_IQMC,

  /*! Set RFLM based Therm enable state */
  RFWCDMA_MDSP_SET_RFLM_THERM_ENABLE_STATE,

  /*! RF FLAG FROM FTM MODE TO FED*/
  RFWCDMA_MDSP_FTM_DISABLE_LDO_OFF,

  /*! Invalid command ID */
  RFWCDMA_MDSP_SET_DATA_INVALID

}rfwcdma_mdsp_set_data_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Enum definition for generic get data command ID .
 
*/

typedef enum
{
  /*! Get RxAGC value  */
  RFWCDMA_MDSP_GET_RXAGC_VAL,

  /* Get current LNA state */
  RFWCDMA_MDSP_GET_CUR_LNA_STATE,

  /*! Get TxAGC value  */
  RFWCDMA_MDSP_GET_TXAGC_VAL,

  /*! Get TxAGC parameters: current RGI,PA bias, PA state, PA current, IQ gain, envolope scale, DPD index  */
  RFWCDMA_MDSP_GET_TXAGC_PARAM,

  /*! Get FW Tx/PA ON status  */
  RFWCDMA_MDSP_GET_TX_ON_STATUS,

  /*! Get buffered TxAGC for pwoer meter */
  RFWCDMA_MDSP_GET_METER_BUFFERED_TXAGC,

/*! Get ROT_FREQ_NBR status  */
  RFWCDMA_MDSP_GET_ROT_FREQ_NBR,

  /*! Check to see if RXLM THERM has been enabled or not */
  RFWCDMA_MDSP_GET_RFLM_THERM_ENABLE_STATE,

  /*! Invalid command ID */
  RFWCDMA_MDSP_GET_DATA_INVALID

}rfwcdma_mdsp_get_data_cmd_id_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to set LNA offset
 
*/
typedef struct
{

  /*! LNA gain state */
  uint8 lna_gain_state;

  /*! LNA offset value to set*/
  int16 offset;

}rfwcdma_mdsp_set_lna_offset_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to set VGA gain offset
 
*/
typedef struct
{

  /*! LNA offset value to set*/
  int16 offset;

}rfwcdma_mdsp_set_vga_gain_offset_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to override RxAGC LNA state in FED
 
*/
typedef struct
{

  /*! LNA gain state to set */
  uint8 state;

}rfwcdma_mdsp_set_lna_gain_state_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to override LNA switchpoint values in FED
 
*/
typedef struct
{
    /*! LNA rise threshold in dB10 unit */
  int32    lnarise[RF_WDMA_MAX_NUM_LNA_STATES-1];

  /*! LNA fall threshold in dB10 unit */
  int32    lnafall[RF_WDMA_MAX_NUM_LNA_STATES-1];

}rfwcdma_mdsp_set_lna_switchpoint_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to send Pin/Pout values to FED
 
*/
typedef struct
{
  /*! Pin values in dB10 unit */
  int16    pin[RF_WCDMA_MAX_NUM_PA_STATES];

  /*! Pout values in dB10 unit */
  int16    pout[RF_WCDMA_MAX_NUM_PA_STATES];

}rfwcdma_mdsp_set_tx_lin_temp_vs_freq_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to send MPR values to FED
 
*/

typedef struct
{
  uint8 mpr[RF_WCDMA_MAX_NUM_MPR_BINS];
}rfwcdma_mdsp_set_mpr_bin_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to send therm values to FED
 
*/

typedef struct
{
  uint8 therm_bin;
  uint8 therm_rem;
}rfwcdma_mdsp_set_therm_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to set Tx frequency error compensation to WFW
 
*/
typedef struct
{
  /*! the Frequency compensation to apply in Hz */
  int32 tx_freq_error; 

  /*! Current WCDMA band */
  rfcom_wcdma_band_type band;

  /*! Rx channel */
  uint16 rx_chan;  

}rfwcdma_mdsp_set_freq_adjust_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to get RxAGC value (and enable/on status)
 
*/
typedef struct
{
  /*-----------Get RxAGC value input ---------------*/
  /*! Antenna Index */
  uint8 ant_idx;

  /*! Carrier ID index */
  uint8 car_idx;

  /*-----------Get RxAGC value output ---------------*/
  /*! Flag to indicate if RxAGC is enabled (RxAGC cmd enable/disable status) */
  uint8 rxagc_enable;

  /*! Flag to indicate if RxAGC is running. Difference between rxagc_on and rxagc_enable 
    is that rxagc can be enabled by agc command but agc can be prozen (due to CPC DRx,
    FET etc.) */
  uint8 rxagc_on;

  /*! RxAGC value, valid only when both rxagc_enable and rxagc_on are TRUE */
  int16 rxagc_val;

}rfwcdma_mdsp_get_rxagc_val_type;


/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to get current TxAGC parameters for logging purpose
 
*/
typedef struct
{
  /*! txagc look up index for const table */
  uint16 row_idx_const;

  /*! txagc look up index for smps table */
  uint16 row_idx_smps;

  /*! PA gain state */
  uint8 pa_state;

  /*! PA SMPS bias */
  uint16 pa_bias;

  /*! PA quiescent current */
  uint16 pa_q_current;

  /*! DPD index */
  uint8 dpd_idx;

  /*! IQ gain */
  int16 iq_gain;

  /*! Env Scale */
  int16 env_scale;

  /*! XPT mode */
  uint8 xpt_mode;

  /*! ET delay */
  int32 xpt_delay;

}rfwcdma_mdsp_get_txagc_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RF-MDSP generic set async data parameter
 
  @details
  This structure will be used to to generic set data operation from RF-MDSP to
  FED/WFW module.
 
*/
typedef struct
{
  /*! Set data command ID */
  rfwcdma_mdsp_set_data_cmd_id_type set_data_cmd;

  /*! RFLM handle ID */
  rflm_dm_handle_id_t rflm_handle;

  /*! Payload: data value to set */
  union
  {
    /*! Generic Byte */
    uint8  gen_b;

    /*! Generic Word */
    uint16 gen_w;     
    
    /*! Generic Double Word */                 
    uint32 gen_dw;     
    
    /*! Set LNA offset data structure */ 
    rfwcdma_mdsp_set_lna_offset_type set_lna_offset;   

    /*! Set LNA state oride data structure */ 
    rfwcdma_mdsp_set_lna_gain_state_type set_lna_state;
    
    /*! Set VGA gain offset data structure */ 
    rfwcdma_mdsp_set_vga_gain_offset_type set_vga_gain_offset;    

    /*! Set LNA switchpoint data structure */ 
    rfwcdma_mdsp_set_lna_switchpoint_type set_lna_switchpoint;

    /*! Set Pin Pout values structure */
    rfwcdma_mdsp_set_tx_lin_temp_vs_freq_type set_tx_lin_temp_vs_freq;

    /*! Set MPR values structure */
    rfwcdma_mdsp_set_mpr_bin_type set_mpr;

    /*! Set Therm values structure */
    rfwcdma_mdsp_set_therm_type set_therm;
    
    /* Set frequency adjust data structure */
    rfwcdma_mdsp_set_freq_adjust_type set_freq_adjust;
         
    /* Set Rx IQ mismatch parameters */
    rfwcdma_mdsp_rx_iqmc_param_type iqmc_param;
         
  }data;
}rfwcdma_mdsp_set_data_param_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition of RF-MDSP generic get async data parameter
 
  @details
  This structure will be used to to generic get data operation from RF-MDSP from
  FED/WFW module.
 
*/
typedef struct
{
  /*! Get data command ID */
  rfwcdma_mdsp_get_data_cmd_id_type get_data_cmd;

  /*! RFLM handle ID */
  rflm_dm_handle_id_t rflm_handle;

  /*! Payload; data value to get */
  union
  {
    /*! Generic Byte */
    uint8  gen_b;

    /*! Generic Word */
    uint16 gen_w;     
    
    /*! Generic Double Word */                 
    uint32 gen_dw;    
    
    /*! Get RxAGC value data structure */
    rfwcdma_mdsp_get_rxagc_val_type get_rxagc_val;

    /* Get TxAGC parameter data structure */
    rfwcdma_mdsp_get_txagc_param_type get_txagc_param;    
     
  }data;
}rfwcdma_mdsp_get_data_param_type;


#ifdef __cplusplus
}
#endif
#endif /* RFWCDMA_MDSP_TYPES_H */
