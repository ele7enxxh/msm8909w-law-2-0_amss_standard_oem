#ifndef FTM_1X_CONRTOL_H
#define FTM_1X_CONRTOL_H

/*!
  @file
  ftm_1x_control.h

  @brief
  This module contains prototypes and definitions to be used by common control 
  code for FTM.
*/

/*==============================================================================

  Copyright (c) 2011, 2012, 2013, 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/inc/ftm_1x_control.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/20/14   ndb    Add Tuner_ID as part of the response in "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
04/10/14   JJ      Changed smem_addr type to void*
10/21/13   JJ      Added fw smem addr to ftm_cdma_data 
08/27/13   aka     Added support for 1x self-test commands 
06/28/13   JJ      Add API for antenna tuner to update tune code in ftm mode
06/20/13   JJ      Changed func ftm_1x_deregister_fw_msgs()
06/19/13   JJ      Add functions for register/de-register fw msgs and change
                   structure ftm_1x_fw_state_config_data_type
06/18/13   JJ      Add members to structure ftm_1x_fw_state_config_data_type
05/14/13   cc      Added support FTM 1x IQ capture 
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
03/19/13   aro     Deleted ET config path
02/04/13   ndb/bmg Added ftm_1x_set_tuner_tune_code_override() 
01/21/13   nrk     Changed input param type path to device for multi synth functions
11/28/12   nrk     Added prototype of APIs to get MULTI SYNTH STATE.
11/08/12   cd      - Added xPT Tx AGC override
                   - Added functionality to configure ET path
09/28/12   aro     Added device to 1x FWS config interface
08/16/12  ars/hdz  Modifed FTM frequency adjust logic to apply correction using 
                   XO manager API
07/24/12   hdz     Added ftm_1x_set_tx_pwr_limit()
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
03/22/12   aro     Deleted unused types/functions
03/08/12   hdz     Added ftm_1x_set_freq_adjust
12/29/11   adk     Query RF card to find out which DAC cals are needed
12/28/11   adk     Implemented DAC_1 calibration in the 1x mode
12/22/11   adk     Implemented DAC_0 calibration in the 1x mode
11/21/11   aro     Added fixes to enable DRx Composite Calibration
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/17/11   aro     Added RFM callback function for FTM
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
10/31/11   aro     Added handler function to handle FW state config response
09/20/11   aro     Added new function to perform clear PA Range
                   override action
09/01/11   cd      Add APT support
                   - Added interface to program APT bias in FTM mode
08/11/11   aro     Added support to second chain test call
07/27/11   aro     Added generic FTM send command function for CDMA
06/13/11   aro     Added Internal command to set Lowest Tx Power
06/02/11   sty     Deleted prototype for ftm_1x_resolve_bandclass()
05/27/11   aro     Added SV Chain Calibration temp support
05/19/11   aro     Added function to configure XO Cal Mode for 1x
04/18/11   sty     Deleted unused ftm_1x_get_all_hdet_tx_sweep()
04/14/11   sty     Renamed ftm_1x_set_diversity to ftm_1x_set_sec_chain_mode()
04/11/11   sty     Renamed ftm_secondary_chain_control_enum_type to 
                   ftm_sec_chain_ctl_enum_type 
03/29/11   sty     Added prototype for ftm_1x_disable_tx() 
03/29/11   sty     Added prototype for ftm_1x_set_diversity
03/14/11   aro     Removed unused prototype
03/14/11   aro     Deleted FW enable and disable function to move to RF
03/07/11   aro     Updated ftm_1x_set_pa_range() interface
03/01/11   aro     Moved  FTM 1x enter and exit to system control file
02/17/11   sty     Added ftm_1x_set_pa_state and changed return type of 
                   ftm_1x_set_tx_avga
02/16/11   sty     Added support for Tx operations from FTM
02/16/11   aro     Moved around Doxygen group
02/16/11   aro     Renamed 1x Tear down interface
02/16/11   aro     Added FW disable function
02/15/11   aro     [1] Added 1x enter and exit
                   [2] Removed FTM init env function
02/14/11   sty     Renamed ftm_1x_set_tx to ftm_1x_enable_tx
                   Changed return type to ftm_1x_init_ftm_environment
02/10/11   sty     Added C++ protection
02/09/11   aro     Added HDET query functions
02/09/11   aro     [1] Added Set Tx Interface
                   [2] Added Configure Tx Waveform interface
01/31/11   aro     Updated RF Tear Down function
01/15/11   aro     [1] Added FTM Set Mode function
                   [2] Added FTM 1x utility functions
01/12/11   aro     Initial Revision

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_cdma_control.h"
#include "ftm.h"
#include "sys.h"
#include "rfm_device_types.h"
#include "rfm_gain_state_types.h"
#include "rfm_cdma_fw_types.h"
#include "rf_cmd_dispatcher.h"
#include "rf_cmd_interface.h"
#include "rfm_async_types.h"
#include "txlm_intf.h"
#include "ftm_common_control.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! Structure to store the 1x FW app state configuration data */
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
  /*!< Variable to indicate the current FW state for 1x */

  void* smem_addr;
  /*! shared memory from FW after RF config fw_state*/

} ftm_1x_fw_state_config_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to store the 1x MCPM configuration data */
typedef struct
{
  uint32 mcpm_vote_mask;
  /*!< Mask to store the current votes requested for MCPM. Voting 
  can be done per device basis. Thus each bit corresponds to a vote for a 
  device. For instance, Bit 0 corresponds to RFM_DEVICE_0 votes, 
  Bit 1 corresponds to RFM_DEVICE_1 votes. Until this vote mask is set to 
  zero, FW cannot be disabled. This is required to avoid unnecessary disabling 
  of MCPM while doing FTM mode exit on a device while 1x is still active 
  on other device. */ 

  boolean mcpm_configured;
  /*!< Variable to indicate the current FW state for 1x */

} ftm_1x_mcpm_config_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to hold the Data used to pass as paylaod to the callback to 
RFM funcitons */
typedef struct
{

  rfm_device_enum_type device;
  /*!< Current RFM Device */

} ftm_1x_rfm_callback_data_type;

/*----------------------------------------------------------------------------*/
sys_band_class_e_type
ftm_1x_resolve_bandclass
(
  ftm_mode_id_type mode
);

/*----------------------------------------------------------------------------*/
rfm_device_enum_type
ftm_1x_get_valid_tx_device
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_vote_fw_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_unvote_fw_state
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_configure_fw_state
(
  rfm_device_enum_type device,
  rfm_cdma_fw_state_type fw_state
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_vote_mcpm
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_unvote_mcpm
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_configure_mcpm_state
(
  boolean enable_mcpm
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_rfm_callback
(
  rfm_cb_event_enum_type rfm_event,
  void *data
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_mode
(
  rfm_device_enum_type device,
  ftm_mode_id_type mode
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_channel
(
  rfm_device_enum_type device,
  sys_channel_num_type rf_channel
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_second_chain_mode
(
  rfm_device_enum_type device,
  ftm_sec_chain_ctl_enum_type sec_chain_mode
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_second_chain_test_call
(
  rfm_device_enum_type device,
  ftm_second_chain_test_call_type sec_chain_cmd
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_enable_tx
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_disable_tx
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_tx_waveform
(
  rfm_device_enum_type device,
  ftm_tx_waveform_type ftm_tx_waveform,
  ftm_tx_waveform_attrib_type ftm_tx_attrib
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_pa_range
(
  const rfm_device_enum_type device,
  uint8 pa_range
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_clear_pa_range_override
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type 
ftm_1x_set_pa_state
(
  const rfm_device_enum_type device,
  rfm_pa_gain_state_type state
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_tx_avga
(
  rfm_device_enum_type device,
  uint16 avga_idx
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_tx_lowest_power
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_tear_down
(
  rfm_device_enum_type  device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_get_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock, 
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_update_aggreated_multisynth_status
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_get_latest_multisynth_status
( 
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock 
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_reset_multisynth_status
(
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type 
ftm_1x_set_tx_apt_smps_bias_val
(
  const rfm_device_enum_type device,
  uint16 apt_smps_bias_val
);

/*----------------------------------------------------------------------------*/
ftm_common_dac_cal_rsp_type 
ftm_1x_do_tx_dac_cal
(
  rfm_device_enum_type device,
  txlm_chain_type txlm_chain,
  uint16 dac_cal_ant0,
  uint16 dac_cal_ant1
);

/*----------------------------------------------------------------------------*/

int32 ftm_1x_set_freq_adjust
(
  rfm_device_enum_type device,
  int32 user_freq_adjust_ppb, 
  uint32 center_frequency_hz, 
  uint8 enable_xo_freq_adjust,
  ftm_cdma_rsp_type *status
);

/*----------------------------------------------------------------------------*/
ftm_cdma_rsp_type
ftm_1x_set_tx_pwr_limit
( 
  rfm_device_enum_type device, 
  boolean tx_pwr_override,
  int16 tx_pwr_limit_dbm
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_set_xpt_txagc_override
(
  ftm_xpt_txagc_override_type *ovr_params
);

/*----------------------------------------------------------------------------*/
ftm_tuner_override_status_type 
ftm_1x_set_tuner_tune_code_override
( 
  rfcom_device_enum_type device,
  uint8 override_flag,
  void *data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
);
/*----------------------------------------------------------------------------*/

ftm_iq_capture_error_code_type
ftm_1x_iq_capture_get_config
(
  ftm_iq_capture_get_config_type *data
);

/*----------------------------------------------------------------------------*/

ftm_iq_capture_error_code_type 
ftm_1x_iq_capture_acquire_samples
(
  rfcom_device_enum_type device,
  ftm_iq_capture_acquire_samples_type *data
);

/*----------------------------------------------------------------------------*/

void 
ftm_1x_iq_capture_get_samples
(
  rfcom_device_enum_type device,
  ftm_iq_capture_get_samples_type *data,
  ftm_rsp_pkt_type* rsp_pkt
);

/*----------------------------------------------------------------------------*/

void 
ftm_1x_create_iq_capture_get_samples_rsp_diag_pkt
(
  rfm_device_enum_type device,
  ftm_rsp_pkt_type *rsp_pkt
);

/*----------------------------------------------------------------------------*/

void
ftm_1x_register_fw_msgs
(
  void
);

/*----------------------------------------------------------------------------*/

void
ftm_1x_deregister_fw_msgs
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_antenna_tuner_update_tune_code
(
  void
);

/*============================================================================

FUNCTION FTM_RF_MEAS_TX_PARAMS

DESCRIPTION
  This API is used to capture and process Tx IQ samples using FBRx and to calculate
  various Tx parameters (i.e. Tx Power, EVM and ACLR)

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_1x_fbrx_meas_tx_params
(
   rfcom_device_enum_type device,
   uint16 num_averages,
   uint32 capture_offset,
   uint32 tx_measurement_config,
   ftm_rsp_pkt_type *rsp_pkt
);

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_TRIGGER

DESCRIPTION
  This command acquires one set of IQ samples and returns key parameters used for
  collecting Rx and TX IQ samples. The samples collected in on acquisition will be
  contiguous. Sampels from different acquisitions are not time coherent.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/
void ftm_1x_fbrx_iq_capture_trigger
(
   rfcom_device_enum_type device,
   uint8 sample_size,
   uint8 fbrx_chain,
   ftm_rsp_pkt_type *rsp_pkt_ptr
);

/*============================================================================

FUNCTION FTM_FBRX_IQ_CAPTURE_GET_SAMPLES

DESCRIPTION
  This API is used to get Tx and/or Rx IQ samples for CDMA from buffer that has been used to capture samples from ACQUIRE SAMPLES. 

DEPENDENCIES
 FTM_FBRX_IQ_CAPTURE_TRIGGER command should have been issued before executing get sample command.

RETURN VALUE
  None

SIDE EFFECTS
 None
=============================================================================*/

void ftm_1x_fbrx_iq_capture_get_samples
(
   rfcom_device_enum_type device,
   uint8 sample_source,
   uint32 sample_offset,
   uint32 sample_size,
   ftm_rsp_pkt_type *rsp_pkt
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_1X_CONRTOL_H */


