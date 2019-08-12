#ifndef FTM_HDR_CONRTOL_H
#define FTM_HDR_CONRTOL_H

/*!
  @file
  ftm_hdr_control.h

  @brief
  This module contains prototypes and definitions to be used by HDR control 
  code for FTM.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_control.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/15/14   pk      Compiler warning fix
07/08/14   pk      Implementation of HDR FTM module
11/11/13   JJ      Removed fw smem addr from ftm_cdma_data 
10/21/13   JJ      Added fw smem addr to ftm_cdma_data 
06/20/13   JJ      Changed func ftm_hdr_deregister_fw_msgs()
06/19/13   JJ      Add functions for register/de-register fw msgs and change
                   structure ftm_hdr_fw_state_config_data_type
06/18/13   JJ      Add members to structure ftm_hdr_fw_state_config_data_type
12/04/12   aro     Added HDR support for RF-FW synchronization
03/22/12   aro     Removed unsed functions and types
11/17/11   aro     Enabled voting for CDMA FW State config and MCPM config
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/17/11   aro     Added RFM callback function for FTM
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
11/10/11   aro     Added RF Task handler to handle response message for HDR
                   FW state change
04/13/11   aro     Removed HDR Enable FW interface to replace with RFM
03/01/11   aro     Moved  FTM HDR enter and exit to system control file
02/16/11   aro     Added HDR Tear down interface
01/20/11   aro     Initial Revision

==============================================================================*/

#include "comdef.h"
#include "ftm_cdma_control.h"
#include "rfm_device_types.h"
#include "rf_cmd_dispatcher.h"
#include "rf_cmd_interface.h"
#include "rfm_cdma_fw_types.h"
#include "rfm_async_types.h"
#include "rfm_hdr_agc_types.h"

/*----------------------------------------------------------------------------*/
/*! Structure to store the HDR FW app state configuration data */
typedef struct
{
  uint32 fw_state_vote_mask;
  /*!< Mask to store the current votes requested for FW state. Voting 
  can be done per device basis. Thus each bit corresponds to a vote for a 
  device. For instance, Bit 0 corresponds to RFM_DEVICE_0 votes, 
  Bit 1 corresponds to RFM_DEVICE_1 votes. Until this vote mask is set to 
  zero, FW cannot be disabled. This is required to avoid unnecessary disabling 
  of FW app while doing FTM mode exit on a device while FW is still enabled 
  on other device. */ 

  rfm_cdma_fw_state_type fw_state;
  /*!< Variable to indicate the current FW state for HDR */

} ftm_hdr_fw_state_config_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store the HDR MCPM configuration data */
typedef struct
{
  uint32 mcpm_vote_mask;
  /*!< Mask to store the current votes requested for MCPM. Voting 
  can be done per device basis. Thus each bit corresponds to a vote for a 
  device. For instance, Bit 0 corresponds to RFM_DEVICE_0 votes, 
  Bit 1 corresponds to RFM_DEVICE_1 votes. Until this vote mask is set to 
  zero, FW cannot be disabled. This is required to avoid unnecessary disabling 
  of MCPM while doing FTM mode exit on a device while HDR is still active 
  on other device. */ 

  boolean mcpm_configured;
  /*!< Variable to indicate the current FW state for HDR */

} ftm_hdr_mcpm_config_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the Data used to pass as paylaod to the callback to 
RFM funcitons */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Current RFM Device */

} ftm_hdr_rfm_callback_data_type;

/*----------------------------------------------------------------------------*/
void
ftm_hdr_vote_fw_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_unvote_fw_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_configure_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_vote_mcpm
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_unvote_mcpm
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_configure_mcpm_state
(
  boolean enable_mcpm
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_rfm_callback
(
  rfm_cb_event_enum_type rfm_event,
  void *cb_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_tear_down
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_register_fw_msgs
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_deregister_fw_msgs
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_set_mode
(	
  rfm_device_enum_type device,
  ftm_mode_id_type mode
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_set_channel
(
  const rfm_device_enum_type device,
  const uint8 num_band_chan,
  const uint16 *band_chan_array
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_set_tx_on

(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_set_tx_off

(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_set_second_chain_mode
(
  rfm_device_enum_type device,
  ftm_sec_chain_ctl_enum_type sec_chain_mode
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_enter_mode
(
  const rfcom_device_enum_type device,
  const rfcom_mode_enum_type rf_mode,
  void * const user_data_ptr,
  rfm_cb_handler_type cb_handler
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_wakeup_rx
(
  const rfm_device_enum_type device,
  const lm_handle_type rxlm_handle,
  const uint32 num_band_chan,
  const sys_channel_type *band_chan_array,
  const int32 *carrier_index,
  const rfm_hdr_agc_acq_type* agc_mode,
  const rfm_cb_handler_type callback,
  void* const userdata   
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_wakeup_tx
(
  const rfm_device_enum_type device,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_sleep_tx
(
  const rfm_device_enum_type device,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_enable_diversity
(
  const rfm_device_enum_type master_device,
  const rfm_device_enum_type slave_device,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
int32
ftm_hdr_disable_diversity
(
  const rfm_device_enum_type device_s,
  const rfm_cb_handler_type callback,
  void* const userdata
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_hdr_get_rx_level_dbm
(
  rfm_device_enum_type device,
  int16 *rx_level_agc,
  int16 *rx_level_dbm
);

#endif /* FTM_HDR_CONRTOL_H */

