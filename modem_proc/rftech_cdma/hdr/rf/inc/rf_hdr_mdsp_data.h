#ifndef RF_HDR_MDSP_DATA_H
#define RF_HDR_MDSP_DATA_H
/*! 
  @file
  rf_hdr_mdsp_data.h
 
  @brief
  This file manages all the Data used by HDR RF-MDSP. This module is completely
  internal to this layer.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/inc/rf_hdr_mdsp_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
03/24/14   spa     Featurize force sleep mssg
03/20/14   spa     Support to force microsleep via FW
07/19/13   Saul    XPT. ET delay vs freq vs temp support.
07/02/13   JJ      Added an instance of hdrfw_indication_cfg_msg_t
06/10/13   sty     use new FW interface var for enabling/disabing JD
05/31/13   zhw     Added thread-safe locks for msgr logging data structure
05/31/13   APU     Optimized the check and retrieval of smem address.    
04/15/13   aro     Added logging of command sent time
03/27/13   ry      Added new mdsp message types for WBAGC JD algorithm 
02/21/13   spa     Added structure for pilot meas stop stream mssg (FTM IRAT)
02/19/12   spa     Support to send fll_xo_cfg_req (FTM IRAT) 
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
08/22/12   cri     EPT support for CDMA FW interfaces
07/26/12   APU     Added support for HDR PA enhanced PA backoff
07/06/12   sty     deleted unused var and APIs
02/29/12   cd      Added message to update PA params for HDR
01/20/12   sty     Added rxlm_cfg_mask_msg to rf_hdr_mdsp_messages_type
12/07/11   aro     Moved MDSP types to Data file
09/29/11   aro     Added interface to initialize MDSP Data
07/19/11   Saul    Combined FW enable/disable APIs into one FW config state API.
06/10/11   aro     Added header file protection
06/10/11   aro     Renamed FW inteface file
06/10/11   aro     Deleted SMEM header file
06/09/11   aro     Renamed FW interface header file
06/09/11   aro     Deleted SMEM module to merge to one file
06/09/11   aro     Added accessor fun to get SMEM Data
06/08/11   aro     Added accessor func to get and set FW State data
06/08/11   aro     Added accessor functions for MSGR Messages and Log
06/08/11   aro     Added data structure to keep track of HDR MDSP Data
06/08/11   aro     Initial Revision
 
==============================================================================*/

#include "comdef.h"
#include "hdrfw_msg.h"
#include "cfw_rf_intf.h"
#include "rfm_cdma_fw_types.h"
#include "rfcommon_time_profile.h"
#include "rfcommon_locks.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
#define RF_HDR_MDSP_MSG_LIST_MAX 50 /*!< Max num of Cmds that can be logged */

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

} rf_hdr_mdsp_msg_log_param_type;

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

  rf_hdr_mdsp_msg_log_param_type sent_msg[RF_HDR_MDSP_MSG_LIST_MAX];
  /*!< Array to hold list Messages sent to MDSP in sequential order */

} rf_hdr_mdsp_msg_log_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all Messages to be sent to HDR MDSP from RF.

  @details
  This structure shall be used to hold all the Messages to be sent to HDR MDSP
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
  appropriate Interface provided by HDR FW. FW Shall provide this interface in 
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

  cfw_tx_limit_config_msg_t tx_limit;
  /*!< Str to hold Tx Power Limits */

  hdrfw_tx_closed_loop_config_msg_t tx_cl_override;
  /*!< Str to hold Tx Closed Loop Overrride */

  hdrfw_tx_open_loop_config_msg_t tx_ol_override;
  /*!< Str to hold Tx Open Loop Overrride */

  cfw_tx_agc_cfg_msg_t tx_agc_cfg;
  /*!< Struct to hold all Tx AGC related config */

  hdrfw_tx_pa_params_config_msg_t tx_pa_params_cfg;
  /*!< Message to update PA params for HDR */

  hdrfw_tx_freq_offset_config_msg_t tx_freq_offset_cfg;
  /*!< Struct to hold all Tx frequency offset related config */

  hdrfw_state_cfg_msg_t fw_state;
  /*!< Message to update the FW state. */

  cfw_dynamic_txc_update_msg_t txc_gain_update;
  /*!< Structure to hold the command and paylaod to update the Tx Conditioner 
  block Data path IQ gain */

  hdrfw_pilot_meas_cfg_req_msg_t pilot_meas_cfg_req;
  /*!< Str to hold Pilot Measurement Config Request data */

  hdrfw_fll_xo_cfg_msg_t fll_xo_cfg_req;
  /*!< Str to hold FLL XO config Request data */

  hdrfw_pilot_meas_stop_stream_msg_t pilot_meas_stop_req;
  /*!< Str to hold Pilot Measurement stop Request data */

  cfw_tx_hdet_request_msg_t hdet_read;
  /*!< Message to request MDSP to trigger HDET read */

  cfw_tx_hdet_config_msg_t hdet_cfg;
  /*!< Message to configure HDET operating parameters to MDSP */

  hdrfw_rxlm_config_masked_msg_t rxlm_cfg_mask_msg;
  /*!< Message to dynamically configure parts of RXLM for HDR */

  hdrfw_jd_threshold_update_msg_t jd_thresh_cfg;
  /*!< Message to configure Jammer detect thresholds */

  hdrfw_jd_mode_msg_t jd_algo_enable_disable;
  /*!< Message to enable/unfreeze the WB AGC algo in firmware */

  hdrfw_indication_cfg_msg_t rxagc_logging_enable;
  /*!< Firmware message to enable rxagc logging */

  #ifdef RF_1X_EPT_CAL_TEMP
  hdrfw_tx_lut_params_config_msg_t tx_lut_params_cfg_msg;
  /*!< Message to update LUT params */
  #endif

  cfw_xpt_config_msg_t xpt_configure;
  /*!< Message to configure XPT params to FW */

  cfw_xpt_delay_update_msg_t xpt_delay_update;
  /*!< Message to update XPT delay in FW */

  #ifdef FEATURE_BOLT_MODEM
  hdrfw_ftm_force_sleep_msg_t force_dlna_microsleep;
  /*!< FTM only mssg sent to FW for DLNA debuggings */
  #endif 

} rf_hdr_mdsp_messages_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold all SMEM Messages to be sent to HDR MDSP from RF.

  @details
  This structure shall be used to hold all the info to be sent to HDR MDSP
  from RF using Shared Memory. Once RF Upper Layer calls any RF-MDSP function, 
  then the parameters will be populated on one (or some) of the fields in SMEM; 

  @todo
  Right now all fields are set to have types as uint32. This shall be replaced
  appropriate Interface provided by HDR FW. FW Shall provide this interface in 
  MSGR Format.
*/
typedef struct
{

  hdrfw_shared_memory_interface_t *smem_ptr;
  /*!< Address to SMEM */

  boolean smem_initialized;
  /*!< Flag to indicate whether RF HDR SMEM is initialized */

} rf_hdr_mdsp_smem_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure definition to hold data pertaining to RF-MDSP for HDR.
 
  @details
  This defines the structure to hold data pertaining to HDR RF-MDSP. All data
  in this structure can be accessed only through the accessor function.
*/
typedef struct
{
  rf_hdr_mdsp_messages_type msgr_data;
  /*!< This structure shall be used to hold all the Messages to be sent to HDR 
  MDSP from RF. Once RF Upper Layer calls any RF-MDSP function, then the 
  parameters will be populated on one (or some) of these fields; and the 
  corresponding message(s)/parameter(s) will be then passed to MDSP using MSGR 
  interface. */

  rf_hdr_mdsp_smem_data_type smem_data;
  /*!< This structure shall be used to hold all the info to be sent to HDR MDSP
  from RF using Shared Memory */

  rf_hdr_mdsp_msg_log_type msg_log;
  /*!< Structure to hold Message Sent Logs. This is used for debugging purpose 
  only. */

} rf_hdr_mdsp_data_type;

/*----------------------------------------------------------------------------*/
boolean
rf_hdr_mdsp_init_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_hdr_mdsp_messages_type*
rf_hdr_mdsp_get_msgr_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_hdr_mdsp_msg_log_type*
rf_hdr_mdsp_get_msgr_log_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_hdr_mdsp_smem_data_type*
rf_hdr_mdsp_get_smem_data
(
  void
);
/*----------------------------------------------------------------------------*/
rf_hdr_mdsp_smem_data_type*
rf_hdr_mdsp_get_validated_smem_data 
(
   boolean*
) ;

/*! @} */

#ifdef __cplusplus
}
#endif

#endif /* RF_HDR_MDSP_DATA_H */

