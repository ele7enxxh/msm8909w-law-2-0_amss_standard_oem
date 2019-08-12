#ifndef RF_1X_MDSP_DATA_H
#define RF_1X_MDSP_DATA_H
/*! 
  @file
  rf_1x_mdsp_data.h
 
  @brief
  This file manages all the Data used by 1X RF-MDSP. This module is completely
  internal to this layer.
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/inc/rf_1x_mdsp_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/08/14   JJ      Added oqpch_search_done msg 
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
06/07/13   zhw     Mainline feature TEMP_RXAGC_ONVERGENCE
05/31/13   zhw     Added thread-safe locks for msgr logging data structure
05/31/13   APU     Optimized the check and retrieval of smem address.    
04/15/13   aro     Added logging of command sent time
04/10/13   aro     Unified LNA calibration setup interface
02/21/13   spa     Added structure for pilot meas stop stream mssg (FTM IRAT)
01/18/13   aro     Deleted nikel-XPT specific code
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
12/11/12   zhw     Enable FW stop Tx AGC message for non ET builds
11/28/12   Saul    Sample Capture. Temp features to FEATURE_RF_HAS_XPT_SUPPORT
11/16/12   zhw     Cleanup deprecated FW interface
11/08/12   cd      Added API to configure xPT
09/19/12   Saul    CDMA Sample Capture. Stop tx agc update fw command
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   cri     EPT support for CDMA FW interfaces
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
07/06/12   sty     deleted unused var and APIs
07/06/12   aro     Uncommented EPT to use temporary feature
06/05/12   anr     Initial EPT Support
03/08/12   hdz     Added tx_freq_offset_static_config_msg to set static config
                   Added tx_freq_offset_dynamic_config_msg to set dynamic config
                   Added tx_freq_offset_afc_mode_msg to set afc mode
02/29/12   cd      Added 1x mdsp message to update Tx PA Params
01/24/12   shb     Added 1x mdsp message to reset HDET
01/17/11   sty     Added cdma_rxlm_cfg_mask_msg to rf_1x_mdsp_messages_type
12/22/11   adk     Implemented DAC_0 calibration in the 1x mode
12/07/11   aro     Moved MDSP types to Data file
09/29/11   aro     Added interface to initialize MDSP Data
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
07/05/11   bmg     extern "C" linkage for C++ compatability.
06/10/11   aro     Added header file protection
06/09/11   aro     Renamed FW interface header file
06/09/11   aro     Deleted SMEM module to merge to one file
06/09/11   aro     Added accessor fun to get SMEM Data
06/08/11   aro     Added accessor func to get and set FW State data
06/08/11   aro     Added accessor functions for MSGR Messages and Log
06/08/11   aro     Added data structure to keep track of 1x MDSP Data
06/08/11   aro     Initial Revision
 
==============================================================================*/

#include "comdef.h"
#include "cdmafw_msg.h"
#include "cfw_rf_intf.h"
#include "rfm_cdma_fw_types.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all Messages to be sent to 1x MDSP from RF.

  @details
  This structure shall be used to hold all the Messages to be sent to 1x MDSP
  from RF. Once RF Upper Layer calls any RF-MDSP function, then the parameters
  will be populated on one (or some) of these fields; and the corresponding
  message(s)/parameter(s) will be then passed to MDSP using MSGR interface.

  The values maintained in this structured are "NOT" meant to be used as Shadow.
  This shall be viewed as "Temporary BUFFER" before pushing data to MDSP. The
  only reason to have this structure stored in Data Segment (as global variable)
  rather than in Stack (as local variable) is to maintain "debug" knowlege of
  the state of current MDSP messages sent.

  @todo
  Right now all fields are set to have types as uint32. This shall be replaced
  appropriate Interface provided by 1X FW. FW Shall provide this interface in 
  MSGR Format.
*/
typedef struct
{
  cfw_rx_agc_cfg_msg_t rx_agc_cfg;
  /*!< Structure to hold RxAGC Configuration. This message only sets up 
  parameters for RxAGC. This does not imply the start or stop of RxAGC. RxAGC 
  start is mostly implied by other configuration messages (such as RX_START) 
  where RxAGC information is deposited earlier than the required start action.*/

  cfw_rx_start_cfg_msg_t rx_start_cfg;
  /*!< Structure to hold Rx Start Configuration. CFW Rx start config message
  definition. Configuration only, actual action gated by RX_START's trigger 
  mode. */

  cfw_rx_start_msg_t rx_start;
  /*!< Rx start message This message is used to initiate RF tune and
  U-RxFE configuration through supplied RF tune and RxLM buffers. Message 
  implies start of RxAGC after specified delay from start time.*/

  cfw_rx_stop_msg_t rx_stop;
  /*!< Rx Stop message. This message is used to deactivate RXLM, program
  RTR sleep script, and stop the RxAGC. */

  cfw_rx_agc_stop_msg_t rxagc_stop;
  /*!< Firmware message to stop RxAGC operation */

  cfw_intelliceiver_update_msg_t ic_update;
  /*!< Message to update Rx with new intelliciever state */

  cfw_tx_start_msg_t tx_start;
  /*!< Tx Start Message. This message will initialize Tx RF Tune and TXLM
  confiuration through RF Tune and TxLM buffers */

  cfw_tx_stop_msg_t tx_stop;
  /*!< Tx Stop Message. This message will deactivate TXLM configuration, and
  program RTR Tx to Sleep. */

  cfw_lna_override_msg_t lna_override;
  /*!< Message to override the LNA to a given LNA gain state */

  cfw_lna_gain_offset_msg_t lna_gain_offset;
  /*!< LNA Gain offset to be used for a given LNA state */

  cdmafw_rxagc_acq_mode_config_msg_t rxagc_acq_mode;
  /*!< Message to indicate 1xFW to move the RxAGC to ACQ and TRK mode while 
  RxAGC is running */

  cdmafw_state_cfg_msg_t fw_state;
  /*!< Message to update the FW state. */

  cfw_tx_agc_cfg_msg_t tx_agc_cfg;
  /*!< TxAGC configuration Message. This will be used to configure the TxAGC.*/

  cdmafw_tx_pa_params_config_msg_t tx_pa_params_cfg;
  /*!< Message to update PA params for 1x */

  cdmafw_tx_limit_config_msg_t tx_limit;
  /*!< Str to hold PA Parameter */

  cdmafw_tx_override_msg_t tx_override;
  /*!< Str to hold Tx Override Data */

  cdmafw_tx_closed_loop_override_msg_t tx_cl_override;
  /*!< Str to hold Tx Closed Loop Override Data */

  cdmafw_tx_open_loop_override_msg_t tx_ol_override;
  /*!< Str to hold Tx Open Loop Override Data */

  cdmafw_tx_pa_state_override_msg_t pa_state_overrride;
  /*!< Str to hold PA State Override Data */

  cfw_dynamic_txc_update_msg_t txc_gain_update;
  /*!< Structure to hold the command and paylaod to update the Tx Conditioner 
  block Data path IQ gain */

  cdmafw_pilot_meas_cfg_req_msg_t pilot_meas_cfg_req;
  /*!< Str to hold Pilot Measurement Config Request data */

  cdmafw_pilot_meas_stop_stream_msg_t pilot_meas_stop_req;
  /*!< Str to hold Pilot Measurement stop Request data */

  cfw_tx_hdet_request_msg_t hdet_read;
  /*!< Message to request MDSP to trigger HDET read */

  cfw_tx_hdet_config_msg_t hdet_cfg;
  /*!< Message to configure HDET operating parameters to MDSP */

  cfw_tx_hdet_reset_msg_t hdet_reset;
  /*!< Message to reset MDSP HDET state machine */

  cfw_tx_dac_cal_config_msg_t dac_cal_config;
  /*!< Message to send DAC calibration parameters to MDSP */

  cdmafw_rxlm_config_masked_msg_t cdma_rxlm_cfg_mask_msg;
  /*!< Message to dynamically configure parts of RXLM for 1x */

  cdmafw_rx_set_freq_offset_msg_t cdma_freq_offset_msg;
  /*!< Message to update the freq offset */

  cdmafw_xo_afc_static_config_msg_t tx_freq_offset_static_config_msg;
  /*!< Message to set tx freq offset_static*/

  cdmafw_xo_afc_dynamic_config_msg_t tx_freq_offset_dynamic_config_msg;
  /*!< Message to set tx freq offset_dynamic*/

  cdmafw_afc_mode_msg_t tx_freq_offset_afc_mode_msg;
  /*!< Message to set afc mode*/

  #ifdef RF_1X_EPT_CAL_TEMP
  cdmafw_tx_lut_params_config_msg_t tx_lut_params_cfg_msg;
  /*!< Message to update LUT params */
  #endif

  cdmafw_xpt_capture_msg_t tx_ept_capture;
  /*!< Message to trigger IQ captures*/

  cdmafw_stop_txagc_update_msg_t stop_txagc_update;
  /*!< Message to stop Tx AGC update */
  
  cfw_xpt_config_msg_t xpt_configure;
  /*!< Message to configure XPT params to FW */

  cfw_xpt_delay_update_msg_t xpt_delay_update;
  /*!< Message to update XPT delay in FW */

  cdmafw_rxagc_lna_cal_override_msg_t cal_setup_lna;
  /*!< Message to configure LNA for calibration */

  cdmafw_oqpch_search_done_msg_t stop_oqpch_srch;
  /*!< Message to Stop OQPCH SEARCH */

} rf_1x_mdsp_messages_type;

/*----------------------------------------------------------------------------*/
#define RF_1X_MDSP_MSG_LIST_MAX 50 /*!< Max num of Cmds that can be logged */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Message Sent Logs

  @details
  This structure holds logs related to the messages sent
*/
typedef struct
{

  msgr_umid_type msg_id;
  /*!< Array to hold list Messages sent to MDSP in sequential order */

  rf_time_tick_type sent_time;
  /*!< Time when the command is sent to FW */
  
  rf_time_type time_delta;
  /*!< Time Delta in uS between previous msg sent and current msg sent */

  errno_enum_type msgr_ret_status;
  /*!< the result of msgr API that is called to send msg_id */

} rf_1x_mdsp_msg_log_param_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold Message Sent Logs

  @details
  This structure holds logs related to the messages sent
*/
typedef struct
{

  boolean log_en;
  /*!< Flag to enable/disable Message Router Log. Flag is defaulted to suppress 
  log */

  boolean f3_en;
  /*!< Flag to enable/disable Message Router F3 Success Message. Flag is 
  defaulted to suppress the F3 Message */

  uint16 idx;
  /*!< Value indicating which index of the array was populated last */

  rf_lock_data_type crit_section;
  /*!< Critical section for message logging data structure */

  rf_1x_mdsp_msg_log_param_type sent_msg[RF_1X_MDSP_MSG_LIST_MAX];
  /*!< Array to hold list Messages sent to MDSP in sequential order */


} rf_1x_mdsp_msg_log_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all SMEM Messages to be sent to 1x MDSP from RF.

  @details
  This structure shall be used to hold all the info to be sent to 1x MDSP
  from RF using Shared Memory. Once RF Upper Layer calls any RF-MDSP function, 
  then the parameters will be populated on one (or some) of the fields in SMEM; 

  @todo
  Right now all fields are set to have types as uint32. This shall be replaced
  appropriate Interface provided by 1X FW. FW Shall provide this interface in 
  MSGR Format.
*/
typedef struct
{

  cdmafw_shared_memory_interface_t *smem_ptr;
  /*!< Address to SMEM */

  boolean smem_initialized;
  /*!< Flag to indicate whether RF HDR SMEM is initialized */

} rf_1x_mdsp_smem_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold data pertaining to RF-MDSP for 1x.
 
  @details
  This defines the structure to hold data pertaining to 1x RF-MDSP. All data
  in this structure can be accessed only through the accessor function.
*/
typedef struct
{
  rf_1x_mdsp_messages_type msgr_data;
  /*!< This structure shall be used to hold all the Messages to be sent to 1x 
  MDSP from RF. Once RF Upper Layer calls any RF-MDSP function, then the 
  parameters will be populated on one (or some) of these fields; and the 
  corresponding message(s)/parameter(s) will be then passed to MDSP using MSGR 
  interface. */

  rf_1x_mdsp_smem_data_type smem_data;
  /*!< This structure shall be used to hold all the info to be sent to 1x MDSP
  from RF using Shared Memory */

  rf_1x_mdsp_msg_log_type msg_log;
  /*!< Structure to hold Message Sent Logs. This is used for debugging purpose 
  only. */

} rf_1x_mdsp_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_1x_mdsp_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_1x_mdsp_messages_type*
rf_1x_mdsp_get_msgr_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_1x_mdsp_msg_log_type*
rf_1x_mdsp_get_msgr_log_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_1x_mdsp_smem_data_type*
rf_1x_mdsp_get_smem_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_1x_mdsp_smem_data_type*
rf_1x_mdsp_get_validated_smem_data 
(
   boolean*
) ;

/*! @} */

#ifdef __cplusplus
}
#endif

#endif /* RF_1X_MDSP_DATA_H */
