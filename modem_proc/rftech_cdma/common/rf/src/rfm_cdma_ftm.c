/*! 
  @file
  rfm_cdma_ftm.c
 
  @brief
  This file defines the RFM-CDMA interfaces to be used by <b>FTM module 
  only</b>.
 
  @addtogroup RF_CDMA_COMMON_RFM_FTM
  @{
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rfm_cdma_ftm.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/10/15   vr      set et path for primary device in config fb transceiver 
11/12/14   sbo     FTM logging should read from the available HDET value. 
11/12/14   spa     Change tx_nv_path to be per band per logical device
09/30/14   zhw     Compiler warning fix
09/23/14   pk      compiler warning fix
08/20/14   ndb     Add Tuner_ID as part of the response in 
                   "FTM_SET_TUNER_TUNE_CODE_OVERRIDE"
08/06/14   fh      Fixed c2k thermistor read tq pair req/release and api
05/19/14   yb      Coupler configuration during XPT cal
05/22/14   sty     Added rfm_cdma_ftm_set_antenna()
05/14/14   APU     Suppy CL tuner with valid TxLM handle.
02/21/14   jmf     C2k FBRx Caps for Droop/DC/RSB Cal
02/06/14   cd      Remove hard-coding to retrieve common device instance
02/04/14   jmf     rfm_cdma_ftm_config_fb_transceiver updated with support for 
                   configuring fb gain state
01/09/14   zhw     CDMA NV loading memory leak fix
12/06/13   cd      Update FBRx gain state to 2, as recommended for all techs
11/27/13   yb      Removing tune_code_override from rfdevice_antenna_tuner_intf layer.
                   Replacing with rf_cdma_atuner_tune_code_override API.
11/25/13   cd      Add feedback Rx configuration as part of XPT config/deconfig
10/10/13   APU     Check for HDET objects() before enabling disabling objects 
                   because some targets may have no HDET at all.
08/26/13   shb     Updates for new C++ transceiver device interface
08/15/13   Saul    SB. Made rf_state 1x/hdr specific.
07/23/13   cd      Fixed compile errors due to multiple tuner support changes
07/23/13   cd      Configure tuner for feedback Rx when setting up the fb 
                   transceiver and back to TDET after feedback Rx is complete
07/22/13   APU     Change the agc_convergence API to have finer polling 
                   granularity so that it can be called from cal
05/23/13   JJ      Clean un-necessary cdma sys_band to rf_band_type conversion
05/20/13   kai     Get the feedback path attenuation state from rfc
05/02/13   vb      Tuner CA changes 
04/24/13   cd      Add support for Therm read as part of Tx Meas Opcode
04/17/31   zhw     Move hdet_init() to cdma_enable_hdet()
03/19/13   aro     Modularized FB config into transceiver, asm, and rxlm
03/13/13   sn      Added XPT functions
03/09/13   hdz     Added rfm_cdma_ftm_convert_hdet_to_txagc_dbm10()
02/20/13   sty     Added rfm_cdma_ftm_log_search_diversity_device() and 
                   rfm_cdma_ftm_log_search_primary_device()
02/08/13   spa     Added call to initialize HDET during setup
02/04/13   ndb/bmg Added tune_code_override() implementation support 
02/01/13   bmg     Moved all HDET calls to CDMA HDET object interface
01/07/13   aro/shb Return TRUE from IC power override if IC is disabled
01/07/13   sty     KW fix to use correct band type
01/04/13   sty     KW fix to use correct band type
12/27/12   shb     Updates for different physical rx_device per band
11/05/12   sty     Use the flag use_icvr to enable/disable I-ceiver
11/05/12   sty     warning fix
11/05/12   sty     Enabled I-ceiver
09/27/12   zhw     Remove write_sbi/grfc_via_rfsw flag
08/07/12   hdz     Added rfm_cdma_ftm_get_therm_index_remainder()
07/31/12   aro     F3 message update
05/29/12   zhw     Support for write GRFC via RFSW debug flag 
05/19/12   zhw     Support for write SBI via RFSW debug flag
03/28/12   aro     Added Current RF State as the parameter for ic calibration
                   to be used for PM override function to check if the state
                   has changed between the time when PM Ocerride command is
                   posted and handled
03/28/12   aro     Updated PM override sequence to have PAUSE and RESUME iC
                   before and after PM override
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   cd      Provide functionality to enable/disable Rx AGC logging
03/15/12   spa     Added max device check for rf_cdma_get_device_status() 
03/01/12   sty     Added code to enable/disable debug messages in 
                   rfm_cdma_ftm_configure_debug_mode() for SSMA
02/06/12   cd      Added interface to setup HDET 
01/23/12   hdz     Added rfm_cdma_ftm_get_therm_reading()
01/16/12   aro     PA state as an argument to enable ET path function
01/16/12   aro     Added interface to query THERM read
01/13/12   aro     Added support for ET path enable for self calibration
01/05/12   sty     Added ability to enable/disable SSMA dynamically
12/20/11   aro     MC function to perform mapping between PA range and state
12/06/11   aro     Replaced RTR8600 device call with Device interface
12/06/11   aro     Added interface to sample and average HDET reads
11/22/11   aro     Added interface to disable iC for debug purpose
11/15/11   aro     Changed F3 message level for PM override
10/13/11   aro     Added interface to enable and disable various debug mode for
                   various debug modules
09/21/11   aro     Updated "Get Power Mode" interface documentation for
                   accuracy
09/21/11   aro     Added interface to report Jammer Status
09/20/11   aro     Fixed the bug where HDET settling time was already returned
                   1 or 0 due to bad data type
08/31/11   aro     Updated HDET Read to use new API
08/24/11   aro     Updated get PM function to return current PM state of
                   intelliceiver driver (not jammer status)
08/22/11   cd      Fixed Klocwork warnings
08/11/11   aro     Added support to query synth status
08/05/11   shb     Added arg in rfdevice_cdma_read_hdet() to indicate if HKADC
                   conversion has already been triggered by MDSP
08/03/11   aro     Removed extra F3 message
08/03/11   sty     Return correct result in rfm_cdma_ftm_get_min_max_avga_idx()
08/03/11   shb     Editted hdet data to hold all device params in 1 struct
07/26/11   aro     Added interface to query HDET settling time
07/25/11   aro     [1] Updated HDET enable/disable/read interfaces to use new
                   device interface functions
                   [2] Pushed PDET LPF bandwidth to device driver
07/21/11   aro     Added interfaces to enable and disable the HDET circuit
07/18/11   aro     Added intelliceiver HDR support
07/13/11   aro     Updated intelliceiver interfaces not to have device param
07/12/11   aro     Added support to query intelliceiver power mode
07/06/11   aro     Updated Override PM interface to return status
07/05/11   aro     Moved the check of iC support to MC
07/01/11   aro     Added Clear Power Mode override interface for FTM
06/29/11   aro     [1] Changed Set Power Mode interface to pass logical dev
                   and cal data from upper layer
                   [2] Renamed intelliceiver function name
06/14/11   aro     Added interface to perform PA Range to PA State Mapping
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/01/11   aro     Fixed the return value
05/31/11   aro     Removed RFM_DEVICE_0 hardcoding to enable SV Tx Calibration
05/21/11   aro     Added interface to query Logical device parameter
04/07/11   aro     Added Read HDET implementation
03/07/11   aro     Changed return type for HDET Read
03/04/11   aro     [1] Added AVGA index Query function
                   [2] Added Turnaround Query function
02/17/11   aro     Removed RFM Device Check to replace with RFM Mode check
02/17/11   aro     Renamed intelliceiver function
02/09/11   aro     Added HDET Read Interface
02/02/11   aro     Added Set Power Mode Interface
02/01/11   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfm_cdma_ftm.h"
#include "rf_cdma_mc.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rf_cdma_intelliceiver.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_msg.h"
#include "rf_cdma_ssma.h"
#include "rfm_internal.h"
#include "rf_cdma_utils_freq.h"
#include "rfdevice_hdet_cdma_intf.h"
#include "rfcommon_atuner_intf.h"
#include "rf_cdma_atuner.h"
#include "rf_cdma_nv.h"
#include "rfcommon_fbrx_api.h"
#include "rfdevice_coupler_intf.h"

/* For AGC convergence function */
#include "rf_1x_mc.h"

/*============================================================================*/
/*!
  @name Rx Intelliceiver Configuration

  @brief
  Functions used to configure RF parameter in FTM Mode
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override Power Mode

  @details
  This function will Override the current RF power mode selection. The power
  mode parameter is RF target dependent.

  @param device
  RF Device whose power mode is to be set

  @param power_mode
  Power Mode value to be set

  @return
  Flag indicating whether the API execution is successful or not.
*/
boolean
rfm_cdma_ftm_power_mode_override
(
  rfm_device_enum_type device,
  rfm_cdma_power_mode_type power_mode
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE; /* Return Value */

  if ( rf_cdma_debug_flags.use_icvr == FALSE )
  {
    return ret_val ;
  }

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_r = rf_cdma_get_device_status( device );
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if(  (dev_status_r == NULL ) || ( dev_status_w ==NULL) )
  {   
      RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_power_mode_override: "
                          "NULL Device %d data ", device );
      return FALSE;
  }

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_cdma_ftm_power_mode_override: "
                      "Null pointer returned to logical_dev ");
    return FALSE;
  }

  /* Set Power Mode */
  if ( dev_status_w->intelliceiver.ic_supported )
  {
    /* Pause intelliceiver before doing Power Mode override */
    ret_val &= rf_cdma_ic_pause( &(dev_status_w->intelliceiver) );

    /* Power mode override */
    ret_val &= rf_cdma_ic_power_mode_override( &(dev_status_w->intelliceiver),
                        logical_dev, 1, dev_status_r->curr_band,
                        &(dev_status_r->lo_tuned_to_chan),
                        dev_status_r->data_1x.rf_state, dev_status_r->data_hdr.rf_state, 
                        power_mode );

    /* Resume intelliceiver */
    ret_val &= rf_cdma_ic_resume(&(dev_status_w->intelliceiver));


    /* FW needs CAL_MODE_ACQ time to settle before we send the next command so FW gets 
    time to transition from track to ACQ. If this is not done FW will crash. */
    ret_val &= rf_1x_mc_rx_agc_convergence ( device ) ;
  }
  else
  {
    RF_MSG_1( RF_HIGH, "rfm_cdma_ftm_power_mode_override: iC not supported "
                       "for Device %d", device );

    ret_val = TRUE;
  }

  /* Return Success */
  return ret_val;

} /* rfm_cdma_ftm_power_mode_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Clear Power Mode override

  @details
  This function will clear the power mode override.

  @param device
  RF Device whose power mode override is to be cleared
 
  @return
  Flag indicating whether the API execution is successful or not.
*/
boolean
rfm_cdma_ftm_clear_power_mode_override
(
  rfm_device_enum_type device
)
{
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  boolean ret_val = TRUE; /* Return Value */

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_w = rf_cdma_get_mutable_device_status( device );
   
  /*NULL pointer check*/
  if(  dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR,"rfm_cdma_ftm_clear_power_mode_override: NULL data for" 
              "device %d" ,device );
    ret_val = FALSE;
  }/* if(  dev_status_w == NULL ) */
  else
  {
      /* Clear Power Mode override */
      if ( dev_status_w->intelliceiver.ic_supported )
      {
        ret_val =
         rf_cdma_ic_clear_power_mode_override( &(dev_status_w->intelliceiver) );
      }
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_power_mode_override: iC not supported "
                            "for Device %d", device );
        ret_val = FALSE;
      }
  }/* if!(  dev_status_w == NULL ) */

  /* Return Success */
  return ret_val;

} /* rfm_cdma_ftm_clear_power_mode_override */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable/Disable Intelliciver operation for debug purpose

  @details
  This function will enable or disable the iC operation on a given device

  @param device
  RF Device whose iC operation is to be changed
 
  @param disable_ic
  Flag indicating whether the iC operation is to be enabled or disabled.
 
  @return
  Flag indicating whether the API execution is successful or not.
*/
boolean
rfm_cdma_ftm_configure_ic_operation
(
  rfm_device_enum_type device,
  boolean disable_ic
)
{
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  boolean ret_val = FALSE; /* Return Value */

  /* Get Current Device Status for the given Device and check whether the data 
  returned is valid. */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /*NULL pointer check*/
  if(  dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR,"rfm_cdma_ftm_configure_ic_operation: NULL data for" 
              "device %d" ,device );
  }/* if(  dev_status_w == NULL ) */
  else
  {
      /* Clear Power Mode override */
      if ( dev_status_w->intelliceiver.ic_supported )
      {
        ret_val = rf_cdma_ic_debug_configure_ic( &(dev_status_w->intelliceiver), 
                                                 disable_ic );
      }
      else
      {
        RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_configure_ic_operation: iC not supported "
                            "for Device %d", device );
        ret_val = FALSE;
      }
  }/* if!(  dev_status_w == NULL ) */
  /* Return Success */
  return ret_val;

} /* rfm_cdma_ftm_configure_ic_operation */

/*! @} */

/*============================================================================*/
/*!
  @name Rx Query Functions

  @brief
  Functions to query Rx Data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the min and max index of the Tx linearizer table

  @details
  The RF device is queried to get the info needed
 
  @param device The device for which linearizer size is to be determined
 
  @param min_lut_idx holds the minimum valid Tx linearizer table index

  @param max_lut_idx holds the maximum valid Tx linearizer table index
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rfm_cdma_ftm_get_min_max_avga_idx 
(
  const rfm_device_enum_type device,
  uint8 *min_lut_idx,
  uint8 *max_lut_idx
)
{
  boolean result;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rfm_cdma_band_class_type band = rf_cdma_data_get_curr_band(device) ;

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_cdma_ftm_get_min_max_avga_idx: "
                      "Null pointer returned to logical_dev ");
    return FALSE;
  }

  if ( logical_dev->tx_device[band] != NULL )
  {
    rfdevice_cdma_tx_cmd_dispatch(logical_dev->tx_device[band],
                                  RFDEVICE_GET_TX_LIN_MIN_OFFSET, min_lut_idx);
   
    rfdevice_cdma_tx_cmd_dispatch(logical_dev->tx_device[band],
                                  RFDEVICE_GET_TX_LIN_MAX_OFFSET, max_lut_idx);
    result = TRUE;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_get_min_max_avga_idx: NULL Logical Device"
                        " for device %d", device );
    result = FALSE;
  }

  return result;

} /* rfm_cdma_ftm_get_min_max_avga_idx */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Query current linearity power mode
 
  @details
  This function will query intelliceiver driver for the current power mode at 
  which it it operating. This will read the software variable for the power mode 
  state.
 
  @param device
  RF Device on which intelliceiver driver is to be queried for current power 
  mode
 
  @return
  Current power mode maintained by intelliciever driver on the given device
*/
rfm_cdma_power_mode_type
rfm_cdma_ftm_get_power_mode
(
  rfm_device_enum_type device
)
{
  rfm_cdma_power_mode_type power_mode; /* Current Power Mode */
  rf_cdma_data_status_type *dev_status_w; /* Device Status */

  /* Get writeable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Perform NULL pointer check */
  if ( dev_status_w != NULL )
  {
    /* Query device driver for intelliceiver state */
    power_mode = rf_cdma_ic_get_current_power_mode(
                                               &(dev_status_w->intelliceiver));
  }
  else
  {
    /* Populate invalid Power Mode */
    power_mode = RFM_CDMA_POWER_MODE_MAX;
  }

  /* Return */
  return power_mode;
} /* rfm_cdma_ftm_get_power_mode */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Query current Jammer State of RTR
 
  @details
  This function will query RTR driver for the current jammer state. Jammer
  State does not necessarily indicate the linearity of the RTR.
 
  @param device
  RF Device on which RTR is to be queried for current jammer state
 
  @return
  Current jamme state of the RTR on the given device
*/
rfm_cdma_power_mode_type
rfm_cdma_ftm_get_jammer_state
(
  rfm_device_enum_type device
)
{
  rfm_cdma_power_mode_type power_mode; /* Current Power Mode */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Device Status for band info */
  const rf_cdma_data_status_type* dev_status_r = 
                                            rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rfm_cdma_ftm_get_jammer_state: "
                      "Null pointer returned to logical_dev ");
    return RFM_CDMA_POWER_MODE_MAX;
  }

  if ( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_get_jammer_state: "
            "Null pointer returned for device status - device %d", device );
    return RFM_CDMA_POWER_MODE_MAX;
  }

  /* Query device driver for intelliceiver state */
  power_mode = 
  rfdevice_cdma_get_ic_state( logical_dev->rx_device[dev_status_r->curr_band]);   
  
  /* Return */
  return power_mode;

} /* rfm_cdma_ftm_get_jammer_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query the Synthesizer lock status
 
  @details
  This function will query the Device driver to check if the synth on a given
  device is locked or not.
 
  @param device
  RF device whose synth lock status is queried
 
  @return
  Flag indicating, if the synth is locked or not.
*/
boolean
rfm_cdma_ftm_get_synth_lock_state
(
  rfm_device_enum_type device
)
{
  boolean synth_state;
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  /* Get Device Status for band info */
  const rf_cdma_data_status_type* dev_status_r = 
                                            rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Perform NULL pointer check */
  if ( logical_dev == NULL )
  {    
    synth_state = 0; /* Populate invalid Power Mode */
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_is_synth_locked: Bad Logical Data "
                        "for dev %d", device );
  }
  else if ( dev_status_r == NULL )
  {
    synth_state = 0; /* Populate invalid Power Mode */
    RF_MSG_1( RF_ERROR , "rfm_cdma_ftm_is_synth_locked: "
              "Null pointer returned for device status - device %d", device );
  }
  else
  {
    /* Query device driver for synth state */
    rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[dev_status_r->curr_band], 
                                   RFDEVICE_GET_RX_SYNC_LOCK, &synth_state );
   }

  /* Return */
  return synth_state;

} /* rfm_cdma_ftm_get_synth_lock_state */

/*! @} */

/*============================================================================*/
/*!
  @name Tx HDET Functions

  @brief
  Functions to handle HDET
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Setup HDET

  @details
  This function is responsible for setting up the HDET circuit in device.
 
  @param device
  RF Device on which HDET is to be enabled
 
  @param power_mode
  HDET power mode for which device must be set-up
 
  @return
  Flag indicating if HDET was setup successfully
*/
boolean
rfm_cdma_ftm_setup_hdet
(
  rfm_device_enum_type device,
  rfdevice_cdma_hdet_power_mode_type power_mode
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = FALSE; /* return value */
  const rf_cdma_data_status_type *dev_status_r;

  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Setup HDET circuit */
  if ( dev_status_r!= NULL && logical_dev != NULL )
  {
    /* get the current band */
    rfm_cdma_band_class_type band = dev_status_r->curr_band;

    ret_val = rfdevice_hdet_cdma_setup(logical_dev->hdet_obj[band],
                                       NULL,
                                       RFDEVICE_EXECUTE_IMMEDIATE,
                              power_mode );
    RF_MSG_2( RF_MED, "rfm_cdma_ftm_setup_hdet: Setup HDET on Dev %d "
                      "for Power Mode %d",
              device, power_mode );
  }
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_setup_hdet: NULL pointers for device %d",
              device );
  }

  return ret_val;

} /* rfm_cdma_ftm_setup_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enable HDET

  @details
  This function is responsible for enabling the HDET circuit in RTR device.
 
  @param device
  RF Device on which HDET is to be enabled
 
  @return
  Flag indicating if HDET was enabled
*/
boolean
rfm_cdma_ftm_enable_hdet
(
  rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE ; /* return value */
  const rf_cdma_data_status_type *dev_status_r;

  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Enable HDET circuit */
  if ( dev_status_r != NULL && logical_dev != NULL )
  {
    rfm_cdma_band_class_type band = dev_status_r->curr_band;
    if ( logical_dev->hdet_obj[band] != NULL )
    {
    /* Get the current channel, use lo_tuned_to_chan in case its MCDO scenario*/
    uint16 chan = dev_status_r->lo_tuned_to_chan;
    uint32 tx_freq = rf_cdma_get_tx_carrier_freq( band, (int)chan );

    ret_val = rfdevice_hdet_cdma_init ( logical_dev->hdet_obj[band] , 
                                        NULL, RFDEVICE_EXECUTE_IMMEDIATE, 
                                        tx_freq );

    ret_val = rfdevice_hdet_cdma_enable( logical_dev->hdet_obj[band],
                                         NULL, RFDEVICE_EXECUTE_IMMEDIATE );
    RF_MSG_1( RF_MED, "rfm_cdma_ftm_enable_hdet: Enabled HDET on Dev %d",
              device );
  }
  else
  {
      RF_MSG_1( RF_MED, "rfm_cdma_ftm_enable_hdet: HDET Obj NOT defined on Dev %d",
                device );
    }
  }
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_enable_hdet: NULL pointers for device %d",
              device );
  }

  return ret_val;

} /* rfm_cdma_ftm_enable_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Read last logged HDET reading for a given device
 
  @param device
  RF Device on which HDET is logged

  @return
  Logged HDET reading
*/
uint16
rfm_cdma_ftm_read_hdet
(
  rfm_device_enum_type device
)
{
  rf_cdma_hdet_data_type* hdet_data;
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  uint16 hdet_reading;
  boolean ret_val = FALSE;

  /* Get Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( dev_status_w != NULL )
  {
    hdet_data = &(dev_status_w->hdet);
  
    if ( hdet_data != NULL )
    {
      hdet_reading = hdet_data->last_hdet_read; 
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  }
  /* Return good value only Read is value */
  if ( ret_val == FALSE )
  {
    hdet_reading = 0;
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_read_hdet: Bad HDET Read on Dev %d",
              device );
  }

  return hdet_reading;
} /* rfm_cdma_ftm_read_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sample N number of HDET reads

  @details
  This function is responsible for reading the target-dependent HDET
  channel.  Targets without integrated HDET ADCs will use the HKADC
  channel specified in the RF card file.  Targets with integrated HDET
  ADCs will use the dedicated converter in the RFT/RTR.
 
  This function will sample N number of HDET reads and return the average HDET
  read.
 
  @param device
  RF Device on which HDET read is to be done
 
  @param num_of_reads
  Number of HDET reads to be done and averaged.

  @return
  Average HDET reading taken from N number of samples
*/
uint16
rfm_cdma_ftm_multi_read_hdet
(
  rfm_device_enum_type device,
  uint16 num_of_reads
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  uint16 hdet_read = 0xFFFF; /* HDET read */
  const rf_cdma_data_status_type *dev_status_r;

  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( dev_status_r != NULL && logical_dev != NULL )
  {  
    rfm_cdma_band_class_type band = dev_status_r->curr_band;
    {
      rf_time_tick_type prof_cal_func;     /* Function Profiling Variable */
      rf_time_type prof_cal_func_val = 0; /* Function Profiling Value Variable */

      prof_cal_func = rf_time_get_tick(); /* Get Time tick for Function Profile */

    hdet_read = rfdevice_hdet_cdma_multi_read_hdet(logical_dev->hdet_obj[band],
                                                num_of_reads );

      prof_cal_func_val = rf_time_get_elapsed( prof_cal_func, RF_USEC );

      RF_MSG_1( RF_MED, "rfdevice_hdet_cdma_multi_read_hdet: "
                          "Profile %d us", prof_cal_func_val );
    }
  }

  return hdet_read;

} /* rfm_cdma_ftm_multi_read_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable HDET

  @details
  This function is responsible for disabling the HDET circuit in RTR device.
 
  @param device
  RF Device on which HDET is to be disabled
 
  @return
  Flag indicating if HDET was disabled
*/
boolean
rfm_cdma_ftm_disable_hdet
(
  rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = TRUE ; /* return value */
  const rf_cdma_data_status_type *dev_status_r;

  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Disable HDET circuit */
  if ( dev_status_r != NULL && logical_dev != NULL )
  {
    rfm_cdma_band_class_type band = dev_status_r->curr_band;

    if ( NULL != logical_dev->hdet_obj[band] )
    {
    ret_val = rfdevice_hdet_cdma_disable(logical_dev->hdet_obj[band],
                                         NULL, RFDEVICE_EXECUTE_IMMEDIATE);
    RF_MSG_1( RF_MED, "rfm_cdma_ftm_disable_hdet: Disabled HDET on Dev %d",
              device );
  }
  else
  {
      RF_MSG_1( RF_MED, "rfm_cdma_ftm_disable_hdet: HDET Obj not defined on Dev %d",
                device );
    }
  }
  else
  {
    ret_val = FALSE;
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_disable_hdet: NULL pointers for device %d",
              device );
  }

  return ret_val;

} /* rfm_cdma_ftm_disable_hdet */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query HDET settling time
 
  @details
  This function is used to query the HDET settling time for the requested
  device. The settling time would be initialized in CDMA data structure
  during bootup by quering the device driver.
 
  @param device
  Device for which HDET settling time is queried
 
  @return
  HDET settling time for the requested device
*/
uint32
rfm_cdma_ftm_get_hdet_settling_time
(
  rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type* dev_status;
  uint32 ret_val;

  /* Get device status */
  dev_status = rf_cdma_get_device_status(device);

  /* NULL pointer check */
  if ( dev_status != NULL )
  {
    ret_val = dev_status->hdet.device_param.pdet_lpf_settling_t;
  }
  else
  {
    ret_val = 0;
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_get_hdet_settling_time: NULL Device %d "
              "Data Pointer ", device );
  }

  return ret_val;

} /* rfm_cdma_ftm_get_hdet_settling_time */

/*! @} */

/*============================================================================*/
/*!
  @name Thermistor

  @brief
  Functions pertaining to thermistor HDET
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query thermistor read
 
  @details
  This function is used to query the thermistor read for the requested
  device.
 
  @param device
  Device for which Therm read is queried
 
  @param therm_read
  Pointer to variable to store therm read

  @param band
  Band for which Therm read is queried
 
  @return
  Flag indicating the validity of the THERM read
*/
boolean
rfm_cdma_ftm_get_therm_read
(
  rfm_device_enum_type device,
  uint32 *therm_read,
  const rfm_cdma_band_class_type band
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rfdevice_therm_read_type rtr_therm_read;
  boolean ret_val = FALSE;

  RF_MSG_1( RF_LOW, "rfm_cdma_ftm_get_therm_read: band %d", band);

  /* Get Logical Device parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* Get Therm Read */
    rfdevice_cdma_tx_cmd_dispatch( 
                                   logical_dev->tx_device[band],
                                   RFDEVICE_GET_THERMISTER_VALUE,
                                   &rtr_therm_read );

    *therm_read = rtr_therm_read.temperature;
    ret_val = rtr_therm_read.valid_temp_read;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_get_therm_read: Bad dev %d logical data",
              device );
    *therm_read = 0;
    ret_val = FALSE;
  }

  /* Return */
  return ret_val;

} /* rfm_cdma_ftm_get_therm_read */ 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query thermistor read during RF calibration
 
  @details
  This function is used to query the thermistor read for the requested
  device. The therm read is returned to the caller
 
  @param device
  Device for which Therm read is queried
 
  @param therm_read
  Pointer to variable to store therm read
 
  @return
  Flag indicating the validity of the THERM read
*/
boolean
rfm_cdma_ftm_get_therm_read_in_cal
(
  rfm_device_enum_type device,
  uint16 *therm_read
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean ret_val = FALSE;
  rfdevice_cdma_therm_read_type therm_read_data;

  /* Get Logical Device parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev != NULL )
  {
    /* Get Therm Read */
    rfdevice_cdma_read_therm( 
       logical_dev->tx_device[rf_cdma_data_get_curr_band(device)],
                              FALSE,
                              &therm_read_data );

    *therm_read = therm_read_data.therm_reading;
    ret_val = therm_read_data.therm_quality;
  }
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_get_therm_read_in_cal: "
                        "Bad dev %d logical data",
              device );
    *therm_read = 0;
    ret_val = FALSE;
  }

  /* Return */
  return ret_val;

} /* rfm_cdma_ftm_get_therm_read_in_cal */ 

/*! @} */


/*============================================================================*/
/*!
  @name Common Query Functions

  @brief
  Functions to query Common CDMA Data
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns turn-around constant (TC) for given band
 
  @param band for which TC is queried
 
  @return TC as defined by the standard. Returns 0 for invalid/unsupported bands

*/
int
rfm_cdma_ftm_get_tc
(
  rfm_cdma_band_class_type const band
)
{
  return (rf_cdma_get_turnaround_constant( band ));

} /* rfm_cdma_ftm_get_tc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Logical Device Parameter

  @details
  This function will return all of the platform-dependent parameters to
  configure the proper hardware devices.

  @param device
  RF Device for which the logical parameter are queried

  @return
  Pointer to the Logical Device parameter structure
*/
const rfc_cdma_logical_device_params_type*
rfm_cdma_ftm_get_logical_device_params
(
  rfm_device_enum_type device
)
{
  return ( rfc_cdma_get_logical_device_params(device) );

} /* rfm_cdma_ftm_get_logical_device_params */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Map PA Range to PA Gain state

  @details
  This function will query NV to get the PA_R_MAP to convert PA Range to
  PA Gain State

  @param device
  RF Device for PA Range to PA State conversion is to be done
 
  @param pa_range
  PA Range which needs to be mapped to PA State
 
  @return
  PA State correspondint to the given PA Range
*/
rfm_pa_gain_state_type
rfm_cdma_map_pa_range_to_pa_state
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_range_type pa_range
)
{
  rfm_pa_gain_state_type pa_state_mapped = RFM_PA_GAIN_STATE_NUM;
  
  pa_state_mapped = rf_cdma_map_pa_range_to_pa_state(device, band, pa_range);

  /* Return Mapped PA State */
  return pa_state_mapped;

} /* rfm_cdma_map_pa_range_to_pa_state */

/*---------------------------------------?-------------------------------------*/
/*!
  @brief
  Mimics the FW calls to RFLM, to set the antenna switch to a particular
  position. The API assumes that the device is in RXTX mode
 
  @param device
  RF device for which antenna switch is to be performed
 
  @return
  TRUE on success and FALSE on failure
*/
boolean
rfm_cdma_ftm_set_antenna
(
  rfm_device_enum_type device
)
{
  boolean api_res; /* API result */
 
  /* get the Rx and TxLM handles */
  const lm_handle_type rx_handle = rf_cdma_data_get_rxlm_handle(device) ;
  const lm_handle_type tx_handle = rf_cdma_data_get_txlm_handle(device) ;
 
  /* Proceed only if State is valid */    
  if ( ( rx_handle == RFM_INVALID_LM_BUFFER ) ||
       ( tx_handle == RFM_INVALID_LM_BUFFER ) )
  {
    api_res = FALSE;

    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_set_antenna: Dev %d not in RXTX state", 
                        device );
  }
  else
  {
    api_res = rf_cdma_mdsp_set_antenna(rx_handle, tx_handle);
  }

  return api_res;

} /* rfm_cdma_ftm_set_antenna */

/*! @} */

/*! @} */

/*============================================================================*/
/*!
  @name Debug

  @brief
  Functions used for CDMA debugging
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure CDMA debug Mode
 
  @details
  Depending on the mask, this function will set various CDMA modules to debug
  mode. This API is equivalent to the following in command in QXDM command
  line:
 
  SendRawRequest 75 11 39 0 04 48 0 0 0 0 [device] [debug_mask] [debug_en]
  [device] is of length 1 byte.
  [debug_mask] is 4 byte long. Should be converted to raw bytes from uint32.
  [debug_en] is 4 byte long. Should be converted to raw bytes from uint32.
  To convert from uint32 to the raw request bytes, convert from decimal to hex
  first, then reverse all the bytes.
  e.g.
  debug_mask = 64 (dec) will result in raw bytes: 64 0 0 0 
  e.g.
  [device] = 0, [debug_mask] = 64, [debug_en] = 1 will require a QXDM command:
  SendRawRequest 75 11 39 0 04 48 0 0 0 0 0 64 0 0 0 1 0 0 0
 
  @param debug_mask
  Mask indicating the CDMA module whose debug mode is to be configured.
  The definition of mask is found in #rfm_cdma_ftm_config_debug_mode_mask_type
 
  @param debug_en
  Flag indicating, if the debug mode is to be enabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
rfm_cdma_ftm_configure_debug_mode
(
  rfm_device_enum_type device,
  uint32 debug_mask,
  boolean debug_en
)
{
  boolean ret_val = TRUE;
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */

  /* Handle flags that are not device specific */
  if ( (debug_mask & RFM_CDMA_CONFIG_ENABLE_SSMA) > 0 )
  {
    ret_val &= rf_cdma_update_ssma_flag ( debug_en );
  }

  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( dev_status_w !=NULL )
  {
    if ( (debug_mask & RFM_CDMA_CONFIG_IC_DEBUG) > 0 )
    {
      ret_val &= rf_cdma_ic_debug_configure_f3( &(dev_status_w->intelliceiver), 
                                             debug_en );
    }
  
    if ( (debug_mask & RFM_CDMA_CONFIG_HDET_DEBUG) > 0 )
    {
      ret_val &= rf_cdma_hdet_configure_debug_mode( &(dev_status_w->hdet),
                                                    debug_en );
    }
  
    if ( (debug_mask & RFM_CDMA_CONFIG_PLIM_DEBUG) > 0 )
    {
      ret_val &= rf_cdma_plim_configure_debug_mode( &(dev_status_w->plim),
                                                    debug_en );
    }

    if ( (debug_mask & RFM_CDMA_CONFIG_ENABLE_SSMA_DEBUG_MSGS) > 0 )
    {
      ret_val &= rf_cdma_enable_ssma_debug_msgs( &(dev_status_w->rf_rx_reco), 
                                                 debug_en );
    }
    if ( (debug_mask & RFM_CDMA_CONFIG_RXAGC_LOG_DEBUG) > 0 )
    {
      ret_val &= rf_cdma_data_set_rxagc_log_mode( device, debug_en );
    }
  }
  else
  {
    ret_val = FALSE;
  }

  return ret_val;

} /* rfm_cdma_ftm_configure_debug_mode */

/*! @} */

/*============================================================================*/
/*!
  @name Tx C2k Temp Comp Functions

  @brief
  Functions to handle C2k Temp Comp
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return C2K thermistor reading

  @details
  This API will return C2K thermistor reading. If last reading is invalid, zero 
  will be returned; otherwise, a valid reading will be returned;
 
  @param device
  RF Device on which temp comp is to be enabled
 
  @return
  uint16 type of thermistor reading
*/
  uint16
  rfm_cdma_ftm_get_therm_reading
  (
    rfm_device_enum_type device
  )
  {

    return rf_cdma_get_therm_reading( device );
  
  } /* rfm_cdma_get_therm_reading */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return  index and remainder of C2K thermistor reading

  @details
  
 
  @param device
  RF Device on which temp comp is to be enabled

  @param *remainder
  pointer to the remainder

  @param *therm_raw
  pointer to the raw therm reading
 
  @return
  uint8 therm_index
*/
uint8
rfm_cdma_ftm_get_therm_index_remainder
(
  rfm_device_enum_type device,
  uint8 *remainder,
  uint16 *therm_raw
)
{
  uint8 therm_index;

  therm_index = rf_cdma_temp_comp_get_therm_index_remainder( device, 
    remainder, therm_raw);
 
  return therm_index;
  
} /* rfm_cdma_ftm_get_therm_index_remainder */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return  hdet reading in txagc unit

  @details
  
 
  @param hdet_read
  HDET reading

  @param device
  Device that phone is operating on
 
  @return
  int16 hdet reading in txagc unit
*/
int16
rfm_cdma_ftm_convert_hdet_to_txagc
(
   uint16 hdet_read,
   rfm_device_enum_type device
)
{
  int16 txagc_raw = 0;
  rf_cdma_data_status_type *dev_status_w; /* Device Status Write Pointer */
  rf_cdma_hdet_data_type hdet_data;

  /* Get Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );
 
  if ( dev_status_w == NULL )
  {
    return txagc_raw;
  }

  hdet_data = dev_status_w->hdet;

  if ( hdet_data.init_done == TRUE && hdet_data.state !=RF_CDMA_HDET_INACTIVE )
  {
    txagc_raw = rf_cdma_convert_hdet_read_to_txagc(
       &hdet_data,hdet_read,hdet_data.power_mode);
  
  }
  
  return txagc_raw;

}

/*! @} */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set the tuner tune code setttings
 
  @details
  This function is used to override the tuner tune code based on the configuration passefd to the device
  
 
  @param device
  RF device whose settings need to be overriden
 
  @param override_flag
  Flag for enabling and disabling the override 

  @param data
  Pointer to the tuner tunecode config structure 

  @param tuner_nv_ptr   
  Pointer to the tuner NV data structure
 
  @param tuner_id_ptr   
  Pointer to the tuner_id
  
  @return
  Return status of the override function, which must map directly to
  the ftm_tuner_override_status_type.
*/
uint32
rfm_cdma_ftm_set_tuner_tune_code_override
( 
  rfcom_device_enum_type device,
  uint8 override_flag,
  void *data,
  uint8* tuner_nv_ptr,
  uint8* tuner_id_ptr
)
{
#ifdef FEATURE_RF_HAS_QTUNER
  uint32 status = RFCMN_ATUNER_ERROR;
  const rf_cdma_data_status_type* dev_status = 
                  rf_cdma_get_device_status(device);
  const rfc_cdma_logical_device_params_type* logical_dev = 
                  rfc_cdma_get_logical_device_params(device);
  rfm_cdma_band_class_type band = RFM_CDMA_MAX_BAND;                  

  if(dev_status == NULL || logical_dev == NULL)
  {
    RF_MSG(RF_HIGH, "FTM_TUNER_OVERRIDE: Unable to get dev_status or logical_dev");
    return RFCMN_ATUNER_TUNE_CODE_FAILURE;
  }

  band = dev_status->curr_band;
  if (logical_dev->ant_tuner[band] != NULL)  
  {
    RF_MSG_2(RF_HIGH, "FTM_TUNER_OVERRIDE: Rx CDMA device %d, override flag %d",device, override_flag);
    status = rf_cdma_atuner_tune_code_override(logical_dev->ant_tuner[band],
                                                       override_flag,
                                                       data,
                                                       tuner_nv_ptr,
                                                       tuner_id_ptr);
  }
  else
  {
    status = RFCMN_ATUNER_NOT_PRESENT;
    RF_MSG(RF_HIGH, "FTM_TUNER_OVERRIDE: Unable to find tuner");
  }
  return status;

#else /* FEATURE_RF_HAS_QTUNER */
  return RFCMN_ATUNER_NOT_PRESENT;
#endif /* FEATURE_RF_HAS_QTUNER */
} /* rfm_cdma_ftm_set_tuner_tune_code_override() */

/*============================================================================*/
/*!
  @name Helper functions

  @brief
  This section contains the helper functions used for AGC Loggings
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for active Primary device

  @details
  This function will query the CDMA RF data structure and look for the device
  which is in given mode and the radio is in either Rx or RxTX state.
 
  @return
  Primary device which is in either Rx or RxTx state.
*/
rfm_device_enum_type
rfm_cdma_ftm_log_search_primary_device
(
  rfm_mode_enum_type mode
)
{
  const rf_cdma_data_status_type* dev_status; /* Device Status */
  rfm_device_enum_type dev_idx; /* Var for device looping */
  rfm_device_enum_type device = RFM_INVALID_DEVICE; /* FOund device */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Loop through all the devices to find the device which is entered for
  given mode and is in Rx or RxTx radio state */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get device status */
    dev_status = rf_cdma_get_device_status(dev_idx);

    /* Continue if the device status is not NULL */
    if ( dev_status != NULL )
    {
      /* Device match is found, if the device is in Rx or RxTx and mode  */
      if ( ( dev_status->rf_mode == mode ) && 
           ( ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RX ) 
             || 
             ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXTX )
             ||
             ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RX ) 
             || 
             ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXTX ) )
         )
      {
        device = dev_idx;
        break;
      }
    }
    else
    {
      RF_MSG_1( RF_HIGH, "rfm_cdma_ftm_log_search_primary_device: Dev %d NULL" 
                 "Data", dev_idx );
      device = RFM_INVALID_DEVICE;
    }
  } /* for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* Return matched device */
  return device;

} /* rfm_cdma_ftm_log_search_primary_device */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Search for active Diversity device

  @details
  This function will query the CDMA RF data structure and look for the device
  which is in given mode and the radio is in RxDIV state.
 
  @return
  Diversity device which is in RxDIV state.
*/
rfm_device_enum_type
rfm_cdma_ftm_log_search_diversity_device
(
  rfm_mode_enum_type mode
)
{
  const rf_cdma_data_status_type* dev_status; /* Device Status */
  rfm_device_enum_type dev_idx; /* Var for device looping */
  rfm_device_enum_type device = RFM_INVALID_DEVICE; /* FOund device */

  /* Acquire Critical Section Lock */
  rf_common_enter_critical_section(rf_cdma_get_lock_data());

  /* Loop through all the devices to find the device which is entered for
  given mode and is in Rx or RxTx radio state */
  for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ )
  {
    /* Get device status */
    dev_status = rf_cdma_get_device_status(dev_idx);

    /* Continue if the device status is not NULL */
    if ( dev_status != NULL )
    {
      /* Device match is found, if the device is in Rx or RxTx and mode  */
      if ( ( dev_status->rf_mode == mode ) && 
           ( ( dev_status->data_1x.rf_state == RF_CDMA_STATE_RXDIV )
             ||
             ( dev_status->data_hdr.rf_state == RF_CDMA_STATE_RXDIV ) )
         )
      {
        device = dev_idx;
        break;
      }
    }
    else
    {
      RF_MSG_1( RF_HIGH, "rfm_cdma_ftm_log_search_diversity_device: Dev %d NULL" 
                 "Data", dev_idx );
      device = RFM_INVALID_DEVICE;
    }
  } /* for ( dev_idx = RFM_DEVICE_0; dev_idx < RFM_MAX_DEVICES; dev_idx++ ) */

  /* Release Critical Section Lock */
  rf_common_leave_critical_section(rf_cdma_get_lock_data());

  /* Return matched device */
  return device;

} /* rfm_cdma_ftm_log_search_diversity_device */

/*! \} */

/*============================================================================*/
/*!
  @name XPT configure feedback for IQ capture

  @brief
  This section XPT calibration related functions that sets up feedback
  for IQ capture
*/
/*! \{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call RFC function to setup feedback path

  @details
  call RFC function to setup feedback path for IQ capture during XPT cal

  @param device_fb
  feedback RF device to which the path is set up

  @param band
  Enum to indicate the cdma band

  @param et_enable
  boolean enable or disable ET
  
  @return
  boolean status
*/
/*----------------------------------------------------------------------------*/
boolean
rfm_cdma_ftm_config_fb_asm 
(
  rfm_device_enum_type device_fb, 
  rfm_cdma_band_class_type band, 
  boolean et_enable
)
{
  boolean ret_val = TRUE ;

  ret_val &= rfc_cdma_setup_fb_path (device_fb, band, et_enable);  

  return(ret_val);
} /* rfm_1x_ftm_config_fb_rxlm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call RFC function to setup feedback device

  @details
  call RFC function to setup feedback device for IQ capture during XPT cal

  @param device
  Transmitting RF device that sends IQ samples
  
  @param device_fb
  feedback RF device to which the path is set up

  @param band
  Enum to indicate the cdma band

  @param tx_chan
  ARFCN channel number
  
  @param fb_state
  feedback path attuenuation states
 
  @return
  boolean status
*/
boolean 
rfm_cdma_ftm_config_fb_transceiver 
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  uint32 tx_chan,
  uint8 fb_state
)
{
  boolean ret_val = TRUE ;
  rfdevice_trx_cdma_tx *tx_device = NULL;  
  rfdevice_rxtx_common_class* trx_cmn_device = NULL;
  const rfc_cdma_logical_device_params_type* logical_dev = NULL;
  const rfc_common_logical_device_params_type* logical_dev_fb = NULL;
  rfc_cmn_properties_type *rfc_cmn_properties_ptr = NULL;
  const rf_cdma_tx_static_type *tx_static_nv_ptr;
  rfdevice_rxtx_common_class* device_ptr;
  rfdevice_coupler_tech_cfg_type coupler_cfg;

  /* Get pointer to the common logical device */
  logical_dev_fb = rfc_common_get_logical_device_params ( device );
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev == NULL || logical_dev_fb == NULL )
  {
    RF_MSG_2( RF_ERROR, "rfm_cdma_ftm_config_fb_transceiver: NULL logical"
                        "data for Tx device %d or FB device %d", device, 
              device_fb );
    return FALSE;
  } /* if ( logical_dev_fb == NULL ) */

  /* Get Tx static NV data */
  tx_static_nv_ptr = rf_cdma_nv_get_tx_static( logical_dev->tx_nv_path[band], 
                                               band );
  if ( tx_static_nv_ptr == NULL )
  {
    RF_MSG_1 ( RF_ERROR, "rfm_cdma_ftm_config_fb_transceiver: "
                     "Failed to get valid static NV pointer "
                     "for device %d", device );
    return FALSE;
  }

  if ( NULL != logical_dev->coupler_obj[band] )
  {    /* Get Tx coupler scripts */ 
    coupler_cfg.mode = mode;
    coupler_cfg.band.cdma_band = band;
    coupler_cfg.direction = RFDEVICE_COUPLER_DIRECTION_FWD;
    coupler_cfg.port = 0;
    ret_val &= rfdevice_coupler_config( logical_dev->coupler_obj[band],
                                       coupler_cfg,
                                       NULL,
                                       RFDEVICE_EXECUTE_IMMEDIATE,
                                       0);
  }/*if( NULL != logical_dev->coupler_obj[band] )*/
  #ifdef FEATURE_RF_HAS_QTUNER
  /* Configure Tuner to Tx mode, which sets up the coupler in the tuner to 
  route correctly to feedback Rx. This can be considered a hack until 
  tuner functionality is available to configure the coupler separately */
  else if ( logical_dev->ant_tuner[band] != NULL )
  {
    rfcom_band_type_u band_in_common_format;
    ant_tuner_device_tokens_type ant_tuner_script_token_data;

    band_in_common_format.cdma_band = band;

    ret_val &=
      rf_cdma_atuner_configure_for_tx( rf_cdma_data_get_txlm_handle (device) , 
                                       logical_dev->ant_tuner[band],
                                       RFM_1X_MODE,
                                       band_in_common_format,
                                       tx_chan,
                                       RFDEVICE_EXECUTE_IMMEDIATE,
                                       NULL,
                                       0, /* No timing offset */
                                       &ant_tuner_script_token_data,
                                     (void*)&(tx_static_nv_ptr->ant_tuner_cl) );
  }
  #endif
  else
  {
    RF_MSG( RF_ERROR, "rfm_cdma_ftm_config_fb_transceiver: "
                        "coupler and tuner device is NULL");
  }

  ret_val &= rfc_cdma_setup_fb_device( device, device_fb, band, tx_chan, 
                                       &trx_cmn_device );

  /* Get the common device pointer for the Tx device */
  device_ptr = 
    rfdevice_cdma_tx_get_common_device_pointer ( 
       logical_dev->tx_device[rf_cdma_data_get_curr_band(device)] );

  /* FB Rx functionality */
  ret_val &= rfcommon_fbrx_mc_enable_rf_dev( device_ptr, fb_state );

  if (ret_val == TRUE)
  {
    if (trx_cmn_device!=NULL)
    {
  
      tx_device = (rfdevice_trx_cdma_tx*)rfdevice_cmn_get_tech_device(
                                             trx_cmn_device,
                                             RFM_1X_MODE,
                                             RFDEVICE_TX,
                                             RFDEVICE_TRX_PATH_0);

      if (tx_device!=NULL)
      {  
        /* Acquire Critical Section Lock */
        rfdevice_cdma_tx_enter_critical_section(tx_device);
      
        ret_val &= rfdevice_cdma_tx_tune_default_port(tx_device,RFM_1X_MODE, 
                (rfm_cdma_band_class_type)band, (uint32)tx_chan, 1250, 
                RFDEVICE_DPD_DISABLED, RFDEVICE_EXECUTE_IMMEDIATE, NULL);
    
        /* Release Critical Section Lock */
        rfdevice_cdma_tx_leave_critical_section(tx_device);

        ret_val &= TRUE;
      }
      else
      {
    
        RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_config_fb_transceiver: "
                            "trx_cmn_device is NULL", 0 );  
        ret_val &= FALSE;
      }
    }
    else
    {
  
      RF_MSG_1( RF_LOW, "rfm_cdma_ftm_config_fb_transceiver: "
                        "trx_cmn_device is NULL", 0 );      
      ret_val &= TRUE;
    }
  }
  else
  {
    ret_val &=  FALSE;
  }

  if ( ret_val == TRUE )
  {
    /* get the feedback path attenuation state from rfc	*/
    ret_val &= rfc_get_cmn_device_properties(&rfc_cmn_properties_ptr);

    if(rfc_cmn_properties_ptr != NULL)
    {
      /* Call device interface to configure the ET Path */
      /*ret_val &=*/ rfdevice_cmn_set_et_path
                           ( logical_dev_fb->cmn_device,
                                         mode, 
                             (rfm_fb_attenuation_state_type)
                             rfc_cmn_properties_ptr->rfc_attenuation_state,
                                         TRUE );
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rfc_cmn_properties pointer is NULL", 0 ); 
      ret_val &=  FALSE;
    }
  } /* if ( ret_val == TRUE ) */

  RF_MSG_2( RF_HIGH, "rfm_cdma_ftm_config_fb_transceiver: Dev %d [Status %d]", 
            device, ret_val );

  return ret_val;

} /* rfm_cdma_ftm_config_fb_transceiver */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  call RFC function to setup feedback device

  @details
  call RFC function to setup feedback device for IQ capture during XPT cal

  @param device
  Transmitting RF device that sends IQ samples
  
  @param device_fb
  feedback RF device to which the path is set up

  @param band
  Enum to indicate the cdma band

  @param tx_chan
  ARFCN channel number
  
  @return
  boolean status
*/
boolean 
rfm_cdma_ftm_deconfig_fb_transceiver 
(
  rfm_device_enum_type device, 
  rfm_device_enum_type device_fb,
  rfm_mode_enum_type mode
)
{
  boolean ret_val = TRUE ;
  const rfc_cdma_logical_device_params_type* logical_dev = NULL;
  const rfc_common_logical_device_params_type* logical_dev_fb = NULL;
  rfdevice_rxtx_common_class* device_ptr;

  /* Get pointer to the common logical device */
  logical_dev_fb = rfc_common_get_logical_device_params ( device_fb );
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* NULL Pointer check */
  if ( logical_dev_fb == NULL || logical_dev == NULL )
  {
    RF_MSG_2( RF_ERROR, "rfm_cdma_ftm_config_fb_transceiver: NULL logical"
                        "data for Tx device %d or FB device %d", 
              device, device_fb );
    return FALSE;
  } /* if ( logical_dev_fb == NULL ) */

  /* Get the common device pointer for the Tx device */
  device_ptr = 
    rfdevice_cdma_tx_get_common_device_pointer( 
       logical_dev->tx_device[rf_cdma_data_get_curr_band(device)] );

  ret_val &= rfcommon_fbrx_mc_disable_rf_dev( device_ptr );

  /* Call device interface to configure the ET Path */
  /*ret_val &= */ rfdevice_cmn_set_et_path( logical_dev_fb->cmn_device,
                                       mode, 
                                       RFM_FB_ATTENUATION_STATE_LOW, FALSE );

  /* Re-enable HDET. This API is specifically required when using tuner-HDET,
  because the coupler in the tuner is configured to route to feedback Rx 
  during cal config. And must be restored to TDET during cal deconfig */
  ret_val &= rfm_cdma_ftm_enable_hdet( device );

  RF_MSG_2( RF_HIGH, "rfm_cdma_ftm_deconfig_fb_transceiver: Dev %d [Status %d]", 
            device, ret_val );

  return ret_val;

} /* rfm_cdma_ftm_deconfig_fb_transceiver */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain/Release the lock for HDET resource.
  Once the lock is acquired, HDET, Tuner CL and Temp comp will be halted. 

  @details
  Obtain/Release the lock for HDET resource. Once the lock is acquired, HDET,
  Tuner CL and Temp comp will be halted.
  The lock is a mutex that are shared between HDET , Tuner CL and Temp comp
  operations.
  Once the lock is obtained, it can not be obtained again.
  Once the lock is released, it can not be released again.

  @param device
  RFM device to obtain HDET lock on. Usually this is the Tx device

  @param lock_hdet_resource
  If TRUE, obtain lock for HDET resource. If FALSE, release lock for
  HDET resource
  
  @return api_status
  If TRUE, lock is obtained/released successfully. If FALSE, lock is not
  obtained/released and would remain as it is.
*/
boolean
rfm_cdma_ftm_obtain_hdet_lock
(
  rfm_device_enum_type device,
  boolean lock_hdet_resource
)
{
  boolean api_status = FALSE;
  rf_cdma_hdet_data_type *hdet_data; /* HDET write pointer*/

  hdet_data = rf_cdma_get_mutable_hdet( device );

  if ( hdet_data != NULL ) 
  {
    if( hdet_data->state != RF_CDMA_HDET_INACTIVE )
    {
      /* enter critical Section */
      rf_common_enter_critical_section( &(hdet_data->crit_section) );

      if (lock_hdet_resource == TRUE)
      {
        api_status = rf_cdma_obtain_hkadc_lock(device);
        if ( api_status == FALSE )
        {
          RF_MSG_1( RF_HIGH, "rfm_cdma_ftm_obtain_hdet_lock: Dev %d "
                             "Failed to obtain HKADC Lock", 
                             device );
        }
      }
      else
      {         
        api_status = rf_cdma_release_hkadc_lock(device);
        if ( api_status == FALSE )
        {
          RF_MSG_1( RF_HIGH, "rfm_cdma_ftm_obtain_hdet_lock: Dev %d "
                             "Failed to release HKADC Lock", 
                             device );
        }
      }

      /* Leave critical Section */
      rf_common_leave_critical_section( &(hdet_data->crit_section) );
    }
  }  
  else
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_ftm_obtain_hdet_lock: Dev %d NULL HDET data", 
                        device );
  }

  RF_MSG_3( RF_MED, "rfm_cdma_ftm_obtain_hdet_lock: Dev %d, Obtain Lock %d,"
                    " Success %d", device, lock_hdet_resource, api_status);

  return api_status;
}/* rfm_cdma_ftm_obtain_hdet_lock */
  
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Translates rfm_cdma_band_class_type values to sys_band_class_e_type 

  @param band
  A band value in rfm_cdma_band_class_type format that must correspond to a
  valid sys band.

  @return
  The equivalent value of the band parameter in the sys_band_class_e_type
  domain.
*/
sys_band_class_e_type 
rfm_cdma_rf_band_type_to_sys_band_type
(
  rfm_cdma_band_class_type band
)
{

  return rf_cdma_rf_band_type_to_sys_band_type(band);

} /* rfm_cdma_sys_band_type_to_rf_band_type */


/*!
  @brief
  Return the PA switchpoints for given tx device and band
 
  @param rfm_device_enum_type: tx_device.
  Tx device for which PA SWPTS are queried.
 
  @param rfcom_cdma_band_type: band
 
  @param pa_rise_swpts: pointer to an array for PA rise
  switchpoints
 
  @param pa_fall_swpts: pointer to an array for PA fall
  switchpoints
 
  @param active_pa_states: number of active PA states
 
  @retval TRUE if PA switchpoints are populated in NV, FALSE if
  PA switchpoints are not populated in NV
*/

boolean 
rfm_cdma_ftm_get_pa_switchpoints
( 
  rfm_device_enum_type      tx_device , 
  rfm_cdma_band_class_type  band , 
  int16                     pa_rise_swpts[] , 
  int16                     pa_fall_swpts[] , 
  int16*                    active_pa_states
) 
{
  return ( 
     rf_cdma_mc_get_pa_switchpoints ( tx_device , band , pa_rise_swpts , 
                                    pa_fall_swpts , active_pa_states ) ) ;
}

  
#endif /* FEATURE_CDMA1X */
/*! @} */
