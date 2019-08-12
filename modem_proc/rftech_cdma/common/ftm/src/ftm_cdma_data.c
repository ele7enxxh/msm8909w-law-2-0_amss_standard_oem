/*! 
  @file
  ftm_cdma_data.c
 
  @brief
  This file contains Centralized Global Data used by Common CDMA FTM.
 
  @addtogroup RF_FTM_COMMON_DATA
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/ftm/src/ftm_cdma_data.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/10/14   JJ      Changed ftm_cdma_data_1x_get_fw_smem_addr() return type
04/08/14   JJ      Add timed sleep rx status in ftm_cdma_data
03/18/14   spa     Invalidate LM buffers for all devices in 
                   ftm_cdma_data_invalidate_lm_buffers API
12/26/13   sar/hdz Remove et_txlm_hanlde from Bolt c2k code, and use one single 
                   TxLm handle allocation.
11/11/13   JJ      Removed funcs to set/get fw smem addr
10/21/13   JJ      Added funcs to set/get fw smem addr
08/27/13   aka     Added support for 1x self-test commands 
06/19/13   JJ      Removed two functions to check/update fw msg registration status
06/18/13   JJ      Added two functions to check/update fw msg registration status
05/28/13   vws     Featurize to exclude all functions from G+W only builds
05/28/13   vr      Featurize to exclude from G+W only builds
05/14/13   cc      Added data structure to hold acquire CDMA IQ samples and RSP packet 
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
01/21/13   nrk     Added aggregated multi synth lock status check and updates functions
12/20/12   sty     Added lower bound check for device
12/19/12   cd      Add ET TxLM configuration support in FTM
12/04/12   zhw     APT PA Q Current support (interface cleanup)
11/09/12   zhw     Klockwork error fix
10/30/12   spa     Use RFM_DEVICE_3 as diversity device on SV path
08/17/12   spa     Fixed incorrect usage of RFM_MAX_DEVICES 
08/06/12   hdz     Removed tx_pwr_limit_data.tx_pwr_override 
07/24/12   hdz     Init tx_pwr_limit_data.tx_pwr_override to FALSE (non-override)
07/02/12   cvd     Dynamically allocate memory to rf_cdma_data.rf_device_status
05/07/12   aa      Changes for Code Maintainability and data access
03/22/12   aro     Removed unsed functions and types
03/22/12   aro     Migrated handling of HDR FW responses to RF_FWRSP task
03/09/12   spa     Changed ftm_cdma_data_update_band_chan arguments to cdma band     
                   and chan type
12/07/11   aro     Fixed the sequencing issue where SET PDM overrides the PA
                   Bias to 0x7FFF clearing the last set PA Bias 
12/06/11   aro     Added infrastructure in CDMA Cal v3 to perform one PM cal
11/17/11   aro     Added new MCPM configure function with voting mechanism
11/16/11   aro     Added function to vote and unvote FW disabe
11/16/11   aro     Added centralized infrastruture to manage FW state change
11/15/11   aro     Updated cal v3 data structure with LNA offset param
11/15/11   aro     Renamed CDMA cal v3 data structure
10/31/11   aro     Added data structure for RF task handlers
10/31/11   adk     Updated function ftm_cdma_resolve_master_device().
09/20/11   aro     Disabled CompositeCal debug mode
09/01/11   cd      Initialize PA state data and PA range overriden flag when in 
                   FTM mode
08/24/11   aro     Added accessor function to query CDMA log data
06/13/11   aro     Set the default CalSweep debug mode to TRUE
06/09/11   aro     Added Calibration Sweep V3 Data structure and Accessor func
05/16/11   aro     Added MCPM state and accessor function
05/06/11   aro     Deprecated linearizer sweep functions
04/27/11   sty     [1] Added init for chain_config in 
                       ftm_cdma_data_init_device_status()
                   [2] Added new APIs - ftm_cdma_data_update_chain_config() and
                       ftm_cdma_resolve_master_device()
04/07/11   aro     Updated TxLinearizer Data Structure and Accessor function
04/06/11   aro     Added Tx Linearizer Cal Data structure/ accessor func
03/29/11   aro     Disabled CompositeCal Debugging by default
03/29/11   sty     Added APIs - ftm_cdma_data_update_assoc_device and 
                   ftm_cdma_is_device_state_valid
03/24/11   aro     Renamed the structure for Q6 clock Speed voting
03/23/11   aro     Moved CompCal Data Init to CompCal func
03/22/11   aro     Added Composite Calibration Data Initialization
03/18/11   aro     Added CompositeCalibration debug flag
03/07/11   sty     Initialize timer-related data in 
                   ftm_cdma_data_init_device_status()
03/03/11   aro     Added Composite Calibration Data Structure and Accessor func
03/03/11   aro     Removed device parameter for Tx Cal Data to reuse same
                   structure for multiple devices
02/22/11   sty     Updated ftm_cdma_data_init_device_status to initialize new 
                   member
                   Added ftm_cdma_get_ftm_timer_ptr()
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   sty     Initialize pa_r1_r0 with valid value
02/17/11   sty     Renamed ftm_cdma_data_get_tx_sweep_adr to 
                   ftm_cdma_data_get_tx_sweep_ptr
02/16/11   sty     Initialized override variable in 
                   ftm_cdma_data_init_device_status
02/16/11   aro     Moved function to appropriate doxygen group
02/16/11   aro     Renamed function to ftm_cdma_data_invalidate_lm_buffers()
02/14/11   sty     Invoke ftm_cdma_invalidate_lm_buffers() on error condition in 
                   ftm_cdma_data_init_device_status()
                   Renamed ftm_rxlm_handle and ftm_txlm_handle to rxlm_handle 
                   and txlm_handle 
02/11/11   sty     Added return type to ftm_cdma_invalidate_lm_buffers
02/09/11   sty     Initialized RxLM and TxLM pointers in 
                   ftm_cdma_data_init_device_status()
                   Added ftm_cdma_data_get_device_ptr()
02/09/11   aro     Added per device field for RFCal Data
02/09/11   aro     Added Accessor function for RF Cal Data structure
02/07/11   aro     Removed Dispatch Command Logging
02/02/11   aro     Added Device Status Init function
02/02/11   aro     Added FTM Command History Logging
02/01/11   aro     Promoted Power Mode types to RFM
01/31/11   aro     Added PowerMode accessor functions
01/31/11   aro     Added Accessor Functions 
01/15/11   aro     [1] Made FTM Data structrue Static Variable
                   [2] Created Data Accessor function
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/13/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "ftm_cdma_data.h"
#include "ftm_msg.h"
#include "modem_mem.h"

#ifdef FEATURE_CDMA1X
#include "rfc_cdma.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Variable to store global Data to be used by 1xHDR FTM
*/
static ftm_cdma_data_type ftm_cdma_data;

/*============================================================================*/
/*!
  @name Update Data

  @brief
  Functions to update the data strucuture
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialization RF Device Status

  @details
  This function will initialize all fields of RF Device Status to a known
  default state. This function should be called only when CDMA Driver
  is initialized.
 
  @return
  Status of Device Status Initialization
*/
boolean
ftm_cdma_data_init_device_status
(
  void
)
{
  boolean ret_val = TRUE;

  rfm_device_enum_type dev_idx; /* Variable for Device Loop */

  /* Set all global CDMA RF Driver state to a known configuration */
  
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    if( rfc_cdma_get_logical_device_params(dev_idx) != NULL)
    {
      ftm_cdma_data.rf_device_status[dev_idx] = (ftm_cdma_data_status_type*)
        modem_mem_alloc(sizeof(ftm_cdma_data_status_type), MODEM_MEM_CLIENT_RFA);

      if(ftm_cdma_data.rf_device_status[dev_idx] == NULL)
      {
        FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_init_device_status: Could not "
          "allocate memory to ftm_cdma_data.rf_device_status for device id %d", 
              dev_idx );
        ret_val = FALSE;
        break;
      }
      /* Default RF state is Sleep State */
      ftm_cdma_data.rf_device_status[dev_idx]->rf_state = FTM_CDMA_STATE_SLEEP;
    
      /* Default RF CDMA band is Invalid Max Band */
      ftm_cdma_data.rf_device_status[dev_idx]->curr_band = 
                                                   FTM_CDMA_INVALID_BAND;
    
      /* Default RF CDMA chan is RF_CDMA_INVALID_CHAN (65536 in UINT16) */
      ftm_cdma_data.rf_device_status[dev_idx]->curr_chan = 
                                                  FTM_CDMA_INVALID_CHAN;
    
      /* Default RF CDMA band is Invalid Device */
      ftm_cdma_data.rf_device_status[dev_idx]->assoc_dev = RFM_INVALID_DEVICE;

      /* Init the chain config to default state */
      ftm_cdma_data.rf_device_status[dev_idx]->chain_config = 
                                              DISABLE_SECONDARY_CHAIN;

      /* Initialize PA Range to lowest value */
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_range 
                                                        = RFM_PA_R1_R0_0_0;
      /* Set PA Range overriden flag to FALSE */
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_range_overriden
                                                        = FALSE;

      /* Initialize PA State to lowest value */
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_state
                                                        = RFM_PA_GAIN_STATE_0;

      /* Initialize Tx AVGA to lowest value */
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.tx_avga_idx = 0;

      /* Start off with invalid Rx and Tx LM buffers */
      ftm_cdma_data_invalidate_lm_buffers(dev_idx);

      /* Reset Cal v3 data */
      ftm_cdma_data.cal_data.cal_v3[dev_idx].curr_power_mode = 
                                                RFM_CDMA_POWER_MODE_INVALID;
 
      /* Initialize the Override values to default */
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_range = 0;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_state = 0;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.tx_avga_idx = 0;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.apt_smps_bias_val = 
                                                                           0x7FFF;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.apt_pa_q_current = 
                                                                             0;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.apt_pa_q_current_enable 
                                                                        = FALSE;
      ftm_cdma_data.rf_device_status[dev_idx]->override_data.pa_range_overriden = 
                                                                            FALSE;
      /* Initialize the aggregated multisynth lock status to default */
      ftm_cdma_data.rf_device_status[dev_idx]->aggregated_multi_synth_lock_status.rx_synth_status = 
                                                 FTM_SYNTH_STATUS_UNDETERMINED;
      ftm_cdma_data.rf_device_status[dev_idx]->aggregated_multi_synth_lock_status.tx_synth_status = 
                                                 FTM_SYNTH_STATUS_UNDETERMINED;
    }
  }
  /* Initialize the pointer to NULL */
  ftm_cdma_data.iq_capture_data.iq_buf = NULL;

  /* Unset the samples available flag */
  ftm_cdma_data.iq_capture_data.data_available = FALSE;

  return ret_val;

} /* ftm_cdma_data_init_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update RF State

  @details
  This function will update the RF Start for given device

  @param device Device for which the RF State is to be updated
  @param rf_state new RF State for the given device
*/
void
ftm_cdma_data_update_rf_state
(
  rfm_device_enum_type device,
  ftm_cdma_state_type rf_state
)
{
  if ( device >= RFM_MAX_DEVICES ||
    ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d",
              device );
    return;
  }

  /* Device Device Status for the requested device */
  ftm_cdma_data.rf_device_status[device]->rf_state = rf_state;

} /* ftm_cdma_data_update_rf_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the mode (OFS/diversity) in which the device is to be tuned

  @details
  This function updates the state variable "chain_config", which is used in
  ftm_1x_set_channel() 

  @param device 
  for which the chain_config is to be updated

  @param chain_config 
  Tells if the Rx chain is to be tuned in OFS/diversity mode

*/
void
ftm_cdma_data_update_chain_config
(
  rfm_device_enum_type device,
  ftm_chain_config_type chain_config
)
{
  if ( device >= RFM_MAX_DEVICES || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return;
  }

  /* pair the device and assoc device */
  ftm_cdma_data.rf_device_status[device]->chain_config = chain_config;

} /* ftm_cdma_data_update_chain_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the master device to which the current device is associated with

  @details
  The algorithm in this API should take the given device, and go thru the list 
  of ALL devices and their current states (from ftm_cdma_data). Based on the info
  in ftm_cdma_data, the master device is to be returned. 
  
  Since this API is used only by calibration routines, the API returns device_0, 
  as this is the only valid master device. 
  
  @param device 
  For which the associated master is to be found

  @return 
  Associated master device

*/
rfm_device_enum_type
ftm_cdma_resolve_master_device
(
  rfm_device_enum_type device
)
{
  rfm_device_enum_type m_dev = RFM_DEVICE_0;

  if( (device == RFM_DEVICE_0) || (device == RFM_DEVICE_1))
  {
    m_dev = RFM_DEVICE_0;
  }
  else if( (device == RFM_DEVICE_2) || (device == RFM_DEVICE_3))
  {
    m_dev = RFM_DEVICE_2;
  }

  FTM_MSG_3( FTM_HIGH, "ftm_cdma_resolve_master_device(): "
             "device = %d; m_dev = %d; rf_state = %d",
             device, m_dev, ftm_cdma_data.rf_device_status[device]->rf_state);

  return m_dev;

} /* ftm_cdma_resolve_master_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the device <=> assoc_device pairing

  @details
  This function is called when a RF device needs to be "paired" with another RF 
  device - this signifies that one device is the master and the other one is the
  slave (i.e, it is in diersity mode, tuned to the same band and chan as the master 
  device). 

  @param device 
  This device will be associated with "assoc_dev"

  @param assoc_dev 
  This device will be associated with "device"

*/
void
ftm_cdma_data_update_assoc_device
(
  rfm_device_enum_type device,
  rfm_device_enum_type assoc_dev
)
{
  if ( device >= RFM_MAX_DEVICES || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return;
  }

  /* pair the device and assoc device */
  ftm_cdma_data.rf_device_status[device]->assoc_dev = assoc_dev;

} /* ftm_cdma_data_update_assoc_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update current band and chan

  @details
  This function will update the current band and channel for given device

  @param device 
  Device for which the band chan is to be updated

  @param band
  New band

  @param chan
  New channel
*/
void
ftm_cdma_data_update_band_chan
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  if ( device >= RFM_MAX_DEVICES || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return;
  }

 /* Update Device Status for the requested device */
  ftm_cdma_data.rf_device_status[device]->curr_band = band;
  ftm_cdma_data.rf_device_status[device]->curr_chan = chan;

} /* ftm_cdma_data_update_band_chan */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update the latest multi synth lock status

  @details
  This function will update the aggregated multi synth lock status for the 
  given device.

  @param device
  Device for which multi synth lock status is to be updated
  
  @param rx_synth_lock 
  latest Rx synth lock status
  
  @param rx_synth_lock 
  latest Tx synth lock status

  @return
  None
*/
void
ftm_1x_update_latest_aggregated_multisynth_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type tx_synth_lock
)
{
  ftm_multi_synth_lock_status_enum_type last_rx_synth_lock;
  ftm_multi_synth_lock_status_enum_type last_tx_synth_lock;
  boolean result = FALSE;

  if ( device >= RFM_INVALID_DEVICE || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_cdma_data_update_rf_state: Invalid Device %d",device);
    return ;
  }

  last_rx_synth_lock = FTM_SYNTH_STATUS_INVALID;
  last_tx_synth_lock = FTM_SYNTH_STATUS_INVALID;

  /* Get the last multi synth lock staus */
  result = ftm_1x_get_aggregated_multi_synth_lock_status( device,
                                                          &last_rx_synth_lock, 
                                                          &last_tx_synth_lock );

  if(result)
  {
    /* latch on to UNLOCKED status if unlock it detected */
    if( last_rx_synth_lock != FTM_SYNTH_STATUS_UNLOCKED)
    {
	  ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.rx_synth_status = \
                                                                   rx_synth_lock;
    }

    if( last_tx_synth_lock != FTM_SYNTH_STATUS_UNLOCKED)
    {
	  ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.tx_synth_status = \
                                                                   tx_synth_lock;
    }  
  }

}/* ftm_1x_update_latest_aggregated_multisynth_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Reset aggregated multi synth lock status

  @details
  This function will rest the aggregated multi synth lock status for the 
  given device.

  @param device Device for which multi synth lock status is to be reset

  @return
  Flag indicating the device multi synth lock status reset success/failed
*/
boolean
ftm_1x_reset_aggregated_multisynth_lock_status
(
  rfm_device_enum_type device
)
{
  if ( device >= RFM_INVALID_DEVICE || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_cdma_data_update_rf_state: Invalid Device %d",device);
    return FALSE;
  }

  ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.rx_synth_status = \
                                                 FTM_SYNTH_STATUS_UNDETERMINED;
  ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.tx_synth_status = \
                                                 FTM_SYNTH_STATUS_UNDETERMINED;

  FTM_MSG_3(FTM_LOW, "Multi synth lock status on device %d is reset to rx_synth_lock=%d tx_synth_lock=%d",
           device,FTM_SYNTH_STATUS_UNDETERMINED,FTM_SYNTH_STATUS_UNDETERMINED);
  
  return TRUE;
}/* ftm_1x_reset_aggregated_multi_synth_lock_status */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the Current Power Mode

  @details
  This function is used by the FTM handlers to set Current RF power mode.

  @param device
  RF Device whose power mode is queried
 
  @param power_mode
  Power Mode Value to be pushed to FTM Data structure
*/
void
ftm_cdma_data_set_power_mode
(
  rfm_device_enum_type device,
  rfm_cdma_power_mode_type power_mode
)
{
  if ( device >= RFM_MAX_DEVICES || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return;
  }
  ftm_cdma_data.rf_device_status[device]->power_mode = power_mode;
} /* ftm_cdma_data_set_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Revert Rx and TxLm buffers to invalid state

  @param device Device which the buffers need to be invalidated

*/
void
ftm_cdma_data_invalidate_lm_buffers
(
  rfm_device_enum_type device
)
{
  if ( device >= RFM_MAX_DEVICES || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return;
  }

  /* Initialize the RxLM buffer pointer to an invalid value */
  ftm_cdma_data.rf_device_status[device]->rxlm_handle = 
                                          FTM_INVALID_LM_BUFFER;
  
  /* Initialize the TxLM buffer pointer to an invalid value */
  ftm_cdma_data.rf_device_status[device]->txlm_handle = 
                                          FTM_INVALID_LM_BUFFER;

} /* ftm_cdma_data_invalidate_lm_buffers */

/*! @} */

/*============================================================================*/
/*!
  @name Query Data

  @brief
  Functions to query the data from FTM data strucuture
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get RF Device Status

  @details
  This function will return the device status for the given device

  @param device Device for which the status is queried
  @param dev_status Container for Device Status
 
  @return
  Flag indicating, if the the device Status is valid or not
*/
boolean
ftm_cdma_data_get_device_status
(
  rfm_device_enum_type device,
  ftm_cdma_data_status_type *dev_status
)
{
  if ( (device < RFM_DEVICE_0) || (device >= RFM_MAX_DEVICES) 
       || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    dev_status = NULL;
    return FALSE;
  }

   /* Device Device Status for the requested device */
  *dev_status = *ftm_cdma_data.rf_device_status[device];

  /* Return Valid Data Flag */
  return TRUE;

} /* ftm_cdma_data_get_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns a pointer to the "ftm_cdma_data" data structure

  @param device Device for pointer is to be returned
 
  @return
  Pointer to ftm_cdma_data on success and 0 on failure
*/
ftm_cdma_data_status_type *
ftm_cdma_data_get_device_ptr
(
  rfm_device_enum_type device
)
{

  if ( ( device >= RFM_DEVICE_0 ) && ( device < RFM_MAX_DEVICES ) )
  {
  /* Device Device Status for the requested device */
  return (ftm_cdma_data.rf_device_status[device]);
  }
  else
  {    
    FTM_MSG_1( FTM_ERROR,"ftm_cdma_data_get_device_ptr: Invalid device %d",
               device );
    return NULL;
  }

} /* ftm_cdma_data_get_device_status */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for Calibration Sweep V3

  @details
  This function is used by the FTM handlers to get/set calibration sweep v3
  data structure.
 
  @param device
  RFM device whose cal v3 data pointer is to be returned

  @return
  Address of calibration sweep v3 Data Container.
*/
ftm_cdma_cal_v3_data_type*
ftm_cdma_data_get_cal_v3_adr
(
  rfm_device_enum_type device
)
{
  if ( ( device >= RFM_DEVICE_0 ) && ( device < RFM_MAX_DEVICES ) )
  {
  return &( ftm_cdma_data.cal_data.cal_v3[device] );
  }
  else
  {
    return NULL;
  }
} /* ftm_cdma_data_get_cal_v3_adr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API that checks if device is in intended state

  @details
  This is a helper function designed to check if a device is in expetced state,
  and that all relevant state variables are correctly populated. For ex: if the
  intended state is FTM_CDMA_STATE_RX, the API would check to make sure that the
  band, chan, LM indices, etc are valid.
  As can be inferred, the checks done are based on the intended state (for ex: 
  there is no need to check for band and chan, if the intended state is 
  FTM_CDMA_STATE_SLEEP)
 
  @param device
  RF device whose state is to be validated
 
  @param exp_state
  The expected RF state
 
  @return
  TRUE if the state is valid and FALSE otherwise
*/

boolean 
ftm_cdma_is_device_state_valid
(
  rfm_device_enum_type device,
  ftm_cdma_state_type exp_state
)
{
  boolean result = TRUE; /* holds final result of API */
  ftm_cdma_data_status_type dev_state;

  if ( (device >= RFM_MAX_DEVICES) || 
       (ftm_cdma_data.rf_device_status[device] == NULL) )
  {
    result = FALSE;
    FTM_MSG_1( FTM_ERROR,"ftm_cdma_is_device_in_valid_state: Invalid device %d",
               device );
  }
  else
  {
    dev_state = *ftm_cdma_data.rf_device_status[device];
    switch (exp_state)
    {
    case FTM_CDMA_STATE_RX:
    case FTM_CDMA_STATE_RXTX:
      if ( (dev_state.rf_state    != exp_state)       || 
           (dev_state.curr_band   == FTM_CDMA_INVALID_BAND)   ||
           (dev_state.curr_chan   == FTM_CDMA_INVALID_CHAN)   )
      {
        /* one or more of the above conditions not satisfied */
        result = FALSE;
      }
      break;

    case FTM_CDMA_STATE_SLEEP:
      if ( (dev_state.rf_state    != FTM_CDMA_STATE_SLEEP)    || 
           (dev_state.curr_band   != FTM_CDMA_INVALID_BAND)   ||
           (dev_state.curr_chan   != FTM_CDMA_INVALID_CHAN)   ||
           (dev_state.assoc_dev   != RFM_INVALID_DEVICE) )
      {
        /* one or more of the above conditions not satisfied */
        result = FALSE;
      }
      break;

    default:
      result = TRUE;
      break;

    } /* switch (exp_state) */

  } /* if (device >= RFM_MAX_DEVICES) */ 

  return result;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the Current Power Mode

  @details
  This function is used by the FTM handlers to get Current RF power mode.

  @param device
  RF Device whose power mode is queried

  @return
  Current Power Mode
*/
rfm_cdma_power_mode_type
ftm_cdma_data_get_power_mode
(
  rfm_device_enum_type device
)
{
  if ( ( device < RFM_DEVICE_0 ) || ( device >= RFM_MAX_DEVICES ) ||
       ( ftm_cdma_data.rf_device_status[device] == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_update_rf_state: Invalid Device %d", 
              device );
    return RFM_CDMA_POWER_MODE_INVALID;
  }

  return ftm_cdma_data.rf_device_status[device]->power_mode;
} /* ftm_cdma_data_get_power_mode */

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Get the Current Power Mode

  @details
  This function is used by the FTM handlers to get Current RF power mode.

  @param device
  RF Device whose power mode is queried

  @return
  Current Power Mode
*/
rfm_cdma_band_class_type
ftm_cdma_data_get_band_class
(
  rfm_device_enum_type device
)
{
  if ( ( device < RFM_DEVICE_0 ) || ( device >= RFM_MAX_DEVICES ) ||
       ( ftm_cdma_data.rf_device_status[device] == NULL ) )
  {
    FTM_MSG_1( FTM_ERROR, "ftm_cdma_data_get_band_class: Invalid Device %d", 
              device );
    return RFM_CDMA_MAX_BAND;
  }

  return ftm_cdma_data.rf_device_status[device]->curr_band;
} /* ftm_cdma_data_get_power_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for MCPM State

  @details
  This function is used by the FTM handlers to get/set the MCPM States. 

  @return
  Address of MCPM State Data structure.
*/
ftm_cdma_mcpm_config_data_type*
ftm_cdma_get_mcpm_state_ptr
(
  void
)
{
  return &( ftm_cdma_data.mcpm_state );
} /* ftm_cdma_get_mcpm_state_ptr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for CDMA Log

  @details
  This function is used by the FTM handlers to get/set the CDMA Log data. 

  @return
  Address of CDMA log Data structure.
*/
ftm_cdma_log_data_type*
ftm_cdma_get_log_ptr
(
  void
)
{
  return &( ftm_cdma_data.cdma_log );
} /* ftm_cdma_get_log_ptr */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container for structure which stores acquired IQ samples information.

  @details
  This function is used by the FTM IQ capture handlers to get/set the information regarding IQ samples captured.

  @return
  Address of CDMA IQ capture Data structure which points to information of acquired samples.
*/
ftm_cdma_iq_capture_data_type*
ftm_cdma_get_iq_capture_data_ptr
(
  void
)
{
  return &( ftm_cdma_data.iq_capture_data );
} /* ftm_cdma_get_iq_capture_data_ptr */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container to FBRx IQ data.

  @details
  This function is used by the FTM handlers to get the FBRx IQ data. 

  @return
  Address of FBRx IQ Data structure.
*/
ftm_rf_cdma_fbrx_iq_acquired_samples_data_type*
ftm_rf_fbrx_get_iq_acquired_samples_data_ptr
(
  void
)
{
  return &( ftm_cdma_data.ftm_rf_fbrx_iq_acquired_samples_data );
} /* ftm_rf_fbrx_get_iq_acquired_samples_data_ptr */

/*---------------------------------------------------------------------------*/
/*!
  @brief
  Get the address of the data container of union which has all the RSP packet structures. .

  @details
  This function is used by the FTM CDMA/HDR handler to store RSP packet structures that will be send to diag.

  @return
  Address of data container of union which has all the RSP packet structures.
*/
ftm_cdma_rsp_pkt_type*
ftm_cdma_get_rsp_pkt_data_ptr
(
  void
)
{
  return &( ftm_cdma_data.rsp_pkt );
}


/*! @} */

/*============================================================================*/
/*!
  @name FW State

  @brief
  This section includes interfaces pertaining to FW State configuration
  Data.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Mutable FW State Config Data

  @details
  This function will return the FW State Config Data. The
  pointer returned from this function can be used to update the
  data as well.

  @return
  Pointer to FW State Config Data Structure used for updates
*/
ftm_cdma_fw_state_config_data_type *
ftm_cdma_get_fw_state_config_data
(
  void
)
{

  /* Return RF Task Data Pointer */
  return &(ftm_cdma_data.fw_state);

} /* ftm_cdma_get_fw_state_config_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to allocate memory for IQ samples.

  @details
  This function will be used to allocate memory

  @return
   None.
*/
void
ftm_cdma_allocate_memory_for_iq_sample_buffer
(
  void
)
{
  if(ftm_cdma_data.iq_capture_data.iq_buf == NULL)
  {
   /* Allocate max memory */
   ftm_cdma_data.iq_capture_data.iq_buf = (uint16*)\
   modem_mem_alloc(sizeof(uint16)*FTM_CDMA_IQ_MAX_SAMPLES, MODEM_MEM_CLIENT_RFA);
  }
  
  /* Invalidate the CDMA IQ capture buffer */
  if(ftm_cdma_data.iq_capture_data.iq_buf != NULL)
  {
    memset(ftm_cdma_data.iq_capture_data.iq_buf,0,sizeof(uint16)*FTM_CDMA_IQ_MAX_SAMPLES);  
  }
} /* ftm_cdma_allocate_memory_for_iq_sample_buffer */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  API to free up any dynamically allocated memory.

  @details
  This function will be used to free up any dynamically allocated memory that was used during 1x operation.
  This will be called during exiting the mode.

  @return
   None.
*/
void
ftm_cdma_free_memory
(
  void
)
{
  /* Deallocate memory if we allcoated for IQ capture */
  if(ftm_cdma_data.iq_capture_data.iq_buf != NULL)
  {
    modem_mem_free(ftm_cdma_data.iq_capture_data.iq_buf,MODEM_MEM_CLIENT_RFA);
    ftm_cdma_data.iq_capture_data.iq_buf = NULL;
  }

} /* ftm_cdma_free_memory */

/*----------------------------------------------------------------------------*/

void*
ftm_cdma_data_1x_get_fw_smem_addr
(
  void
)
{
   return ftm_cdma_data.fw_state.onex_fw.smem_addr;
} /* ftm_cdma_data_1x_get_fw_smem_addr */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the latest multi synth lock status

  @details
  This function will return the aggregated multi synth lock status for the 
  given device.

  @param device
  Device for which multi synth lock status is to be returned
  
  @param rx_synth_lock
  Rx synth lock status
  
  @param rx_synth_lock
  Tx synth lock status
  
  @return
  Flag indicating the device multi synth lock status read success/failed  
*/
boolean
ftm_1x_get_aggregated_multi_synth_lock_status
(
  rfm_device_enum_type device,
  ftm_multi_synth_lock_status_enum_type *rx_synth_lock,
  ftm_multi_synth_lock_status_enum_type *tx_synth_lock 
)
{
  if ( device >= RFM_INVALID_DEVICE || ftm_cdma_data.rf_device_status[device] == NULL )
  {
    FTM_MSG_1(FTM_ERROR,"ftm_cdma_data_update_rf_state: Invalid Device %d",device);
    return FALSE;
  }

  *rx_synth_lock = \
    ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.rx_synth_status;
  *tx_synth_lock = \
  	ftm_cdma_data.rf_device_status[device]->aggregated_multi_synth_lock_status.tx_synth_status;

  return TRUE;
} /*ftm_1x_get_aggregated_multi_synth_lock_status*/

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set timed sleep state.

  @details
  This function will set timed sleep state by the callback func passed to
  rfm_1x_timed_sleep_rx()

  @param state
  1x timed sleep state
 
  @return
  void
*/
void
ftm_cdma_data_set_timed_sleep_state
(
  boolean state
)
{
  ftm_cdma_data.timed_sleep_success = state;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get timed sleep state.

  @details
  This function will Get timed sleep state by the callback func passed to
  rfm_1x_timed_sleep_rx()
 
  @param void
 
  @return state
  1x timed sleep state
*/
boolean
ftm_cdma_data_get_timed_sleep_state
(
  void
)
{
  return(ftm_cdma_data.timed_sleep_success);
}

#endif

/*! @} */

/*! @} */
