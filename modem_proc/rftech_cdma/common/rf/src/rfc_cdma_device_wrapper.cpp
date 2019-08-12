/*!
  @file
  rfc_cdma_device_wrapper.cpp

  @brief
  Contains implementation of CDMA wrapper object used to communicate with
  multiple CDMA RX transceiver devices on the same receive path
  
  @addtogroup RF_CDMA_COMMON_DEVICE_WRAPPER
  @{
*/

/*==============================================================================

  Copyright (c) 2012-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rfc_cdma_device_wrapper.cpp#1 $

when       who     what, where, why
--------------------------------------------------------------------------------
09/16/15   sd      Interface changes for Rx AGC reload
07/29/15   zhw     Added support for using AWAY WXE buffer during DRx QTA
07/08/15   vr      Dev intf changes for MSM8909 + WTR2955
08/01/14   lmiao   Added new multi_carrier_droop_compensation() API to support cdma mcdo
06/19/14   spa     Update interface for notch callback API and data
03/09/14   pk      Added interface to query DLNA support
11/13/13   shb     Corrected arg in CDMA meas script API
11/11/13   shb     Added new CDMA RX create_meas_script API
08/30/13   shb     Fixed KW errors
08/26/13   shb     Converted to cpp for new rfdevice_trx_cdma_rx class 
                   receiver device interface
06/01/13   spa     Add support for Rx on/off script retrieval 
03/14/13   shb     Updated critical section ptr to lock first RX device
01/08/13   shb     Implemented RFDEVICE_GET_COMM_INFO cmd dispatch
01/06/13   shb     Added function to return LNA gain words per gain state in 
                   rf_buffer_intf format
12/27/12   shb     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfc_cdma_device_wrapper.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_locks.h"
#include "rfcommon_msg.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to enter critical section on rx_device object.

  @details
  Device operations need to be thread safe. This function ensures the device is
  locked at the appropriate layer. This function will lock all rx_devices in
  this wrapper object
*/
boolean 
rfc_cdma_device_wrapper::enter_critical_section
(
  void
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_rx_enter_critical_section(rx_devices[device_index]) 
         == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::enter_critical_section"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;
} /* rfc_cdma_device_wrapper::enter_critical_section */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to leave critical section on a rx_device wrapper

  @details
  Device operations need to be thread safe. This function releases the device
  lock on all rx_devices in the wrapper for waiting threads to use.
 
  @param rx_device
  pointer to rx_device that needs to be locked
*/
boolean 
rfc_cdma_device_wrapper::leave_critical_section
(
  void
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_rx_leave_critical_section(rx_devices[device_index]) 
         == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::leave_critical_section"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;
} /* rfc_cdma_device_wrapper::leave_critical_section */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the rx tune function
  on each of them

  @details
  Wrapper function that goes thru all Receive devices in the wrapper object,
  creates their rx tune script in the same buf obj ptr
 
  @param mode
  RF Mode for which RF Rx Device Tune script is to be createds
 
  @param band
  Rx Band for which the script is to be generated
 
  @param chan
  Rx Channel for which the script is to be generated
 
  @param rx_bw_khz
  Rx Band-width of operation in KHz.

  @param tx_bw_khz
  Tx Band-width of operation in KHz.
 
  @param power_mode
  Power Mode for which the script is to be generated
  
  @param *buf_obj_ptr
  Data structure in which the all transactions are to be populated
 
  @param script_mode
  Input flag indicating whether the script being generated is for a
  temporary tune away for measurement (IRAT) or for a standalone RX tune.
  This is used to determine if device should set its internal state variables
  to a new state based on this function execution

 @rf_cdma_program_wtr_notch_cb
 Callback function pointer from rf cdma tech layer that would program WTR
 notches to the modem

 @cb_data
 Pointer to a structure from rf cdma tech layer where all the WTR notch
 data would be populated before invoking the callback function

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
*/

boolean 
rfc_cdma_device_wrapper::create_tune_script
(
  rfm_mode_enum_type mode, 
  rfm_cdma_band_class_type band, 
  rfm_cdma_chan_type chan, 
  uint32 rx_bw_khz, 
  uint32 tx_bw_khz, 
  rfm_cdma_power_mode_type power_mode, 
  rf_buffer_intf *buf_obj_ptr, 
  rfdevice_cdma_script_operation_type script_mode,
  rf_cdma_spur_cb_type rf_cdma_program_wtr_notch_cb = NULL,
  rfdevice_cdma_ssma_callback_data_type* cb_data = NULL,
  rfdevice_wtr_config_bitmask_type *wtr_config_bitmask, /* = NULL */
  boolean use_alt_gain /* = FALSE */
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_create_rx_tune_script( rx_devices[device_index],
                                              mode,
                                              band,
                                              chan,
                                              rx_bw_khz,
                                              tx_bw_khz,
                                              power_mode,
                                              buf_obj_ptr,
                                              script_mode,
                                              rf_cdma_program_wtr_notch_cb,
                                              cb_data ) == FALSE ) 
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::create_tune_script"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::create_tune_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Create measurement (IRAT) script for CDMA
  
  @details
  Generates preload and trigger scripts for CDMA measurement - tune away or
  tune back.
  
  @param band
  Rx Band for which the script is to be generated
 
  @param chan
  Rx Channel for which the script is to be generated
 
  @param rx_bw_khz
  Rx Band-width of operation in KHz.

  @param tx_bw_khz
  Tx Band-width of operation in KHz.
 
  @param power_mode
  Power Mode for which the script is to be generated
 
  @param meas_type
  Indicates whether this is a startup (tune away) script or
  cleanup (tune back) script

  @param *script_data_ptr
  Structure with input/output parameters needed for IRAT scripts.
  
  script_data_ptr->preload_action
  is an input param indicating whether preload_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (preload was already done)
  
  script_data_ptr->preload_buf
  Buffer ptr where preload script should be populated IF
  script_data_ptr->preload_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Preload script can be executed before the measurement gap without
  disrupting the source RAT.
  
  script_data_ptr->meas_action
  is an input param indicating whether trigger_script should be executed
  internally by device, returned in buffer provided or skipped
  altogether (meas script will be generated later)
  
  script_data_ptr->meas_buf
  Buffer ptr where meas script should be populated if
  script_data_ptr->meas_action == RFDEVICE_MEAS_CREATE_SCRIPT.
  Meas script should be executed within the IRAT measurment gap as its
  execution will immediately disrupt source RAT and tune for away measurement.

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
 
  @return
  Success/Failure flag
*/
boolean 
rfc_cdma_device_wrapper::create_meas_script
(
  rfm_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rfdevice_meas_scenario_type meas_type,
  rfdevice_meas_script_data_type *script_data_ptr,
  boolean use_alt_gain /* = FALSE */
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_create_rx_meas_script( rx_devices[device_index],
                                              mode,
                                              band,
                                              chan,
                                              rx_bw_khz,
                                              tx_bw_khz,
                                              power_mode,
                                              meas_type,
                                              script_data_ptr) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::create_meas_script"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::create_meas_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the set linearity
  function on each of them
 
  @param mode
  RF Mode for which RF Rx Device Tune script is to be createds
 
  @param band
  Rx Band for which the script is to be generated
 
  @param chan
  Rx Channel for which the script is to be generated
 
  @param rx_bw_khz
  Rx Band-width of operation in KHz.

  @param tx_bw_khz
  Tx Band-width of operation in KHz.
 
  @param power_mode
  Power Mode for which the script is to be generated
  
  @param *buf_obj_ptr
  Data structure in which the all transactions are to be populated  

  @param use_alt_gain
  Parameter specifying whether to use default gain settings or
  the gain settings due to antenna switch
*/
boolean 
rfc_cdma_device_wrapper::create_enable_diversity_script
(
  rfcom_mode_enum_type mode,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz,
  rfm_cdma_power_mode_type power_mode,
  rf_buffer_intf *buf_obj_ptr,
  rfdevice_trx_cdma_rx* paired_device,
  rfdevice_rx_mode_type rx_mode,
  rfdevice_operation_mode operation_mode,
  rfdevice_wtr_config_bitmask_type *wtr_config_bitmask, /* = NULL */
  boolean use_alt_gain /* = FALSE */
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_create_rx_enable_diversity_script( 
           rx_devices[device_index],
           mode,
           band,
           chan,
           rx_bw_khz,
           tx_bw_khz,
           power_mode,
           buf_obj_ptr) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::create_enable_diversity_script"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::create_enable_diversity_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the rx
  set linearity function on each of them
 
  @param power_mode
  rfm_cdma_power_mode_type power mode of operation
 
  @param retune_pll
  Indicates if PLL must be relocked after changing linearity settings
*/
boolean
rfc_cdma_device_wrapper::ic_set_linearity
(
  rfm_cdma_power_mode_type power_mode
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_ic_set_linearity( rx_devices[device_index],
                                         power_mode ) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::set_linearity"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::set_linearity */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the rx
  set linearity function on each of them

  @details
  Wrapper function that goes thru all Receive devices in the wrapper object,
  creates their rx linearity script in the same buf obj ptr
*/
boolean
rfc_cdma_device_wrapper::post_pll_lock_processing
(
  void
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_rx_post_pll_lock_processing(rx_devices[device_index]) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::post_pll_lock_processing"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::post_pll_lock_processing */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the sleep
  function on each of them
 
  @param *buf_obj_ptr
  Data structure in which the all transactions are to be populated
 
  @param rx_bw_khz
  Rx Band-width of operation in KHz.

  @param tx_bw_khz
  Tx Band-width of operation in KHz.
*/
boolean
rfc_cdma_device_wrapper::create_sleep_script
(
  rf_buffer_intf *buf_obj_ptr,
  rfdevice_trx_cdma_rx* paired_device,
  rfdevice_rx_mode_type rx_mode,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_create_rx_sleep_script( 
           rx_devices[device_index],
           buf_obj_ptr,
           rx_bw_khz,
           tx_bw_khz) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_sleep failed for "
                          "device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::create_sleep_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and executes the disable div
  function on each of them
 
  @param *buf_obj_ptr
  Data structure in which the all transactions are to be populated
 
  @param rx_bw_khz
  Rx Band-width of operation in KHz.

  @param tx_bw_khz
  Tx Band-width of operation in KHz.
*/
boolean
rfc_cdma_device_wrapper::create_disable_diversity_script
(
  rf_buffer_intf *buf_obj_ptr,
  rfdevice_trx_cdma_rx* paired_device,
  rfdevice_rx_mode_type rx_mode,
  uint32 rx_bw_khz,
  uint32 tx_bw_khz
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_create_rx_disable_diversity_script( 
           rx_devices[device_index],
           buf_obj_ptr,
           rx_bw_khz,
           tx_bw_khz) == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::create_disable_diversity_script"
                          " failed for device# %d",
                device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::create_disable_diversity_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and checks for IntelliCeiver
  support
 
  @retval TRUE
  IntelliCeiver is supported by atleast one receive device in the wrapper object
 
  @retval FALSE
  IntelliCeiver is not supported by any receive device
*/
boolean 
rfc_cdma_device_wrapper::is_ic_supported
(
  void
)
{
  uint8 device_index;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    /* If any device supports intelliceiver, it is supported by the system */
    if ( rfdevice_cdma_is_ic_supported(rx_devices[device_index])
         == TRUE )
    {
      return TRUE;
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return FALSE;

} /* rfc_cdma_device_wrapper::is_ic_supported */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and checks for DLNA
  support
 
  @retval TRUE
  DLNA is supported by atleast one receive device in the wrapper object
 
  @retval FALSE
  DLNA is not supported by any receive device
*/

boolean 
rfc_cdma_device_wrapper::is_dlna_supported
(
   void
)
{
	uint8 device_index;
	
	for ( device_index = 0; 
		  device_index < num_rx_devices;
		  device_index++ )
	{
	  /* If any device supports DLNA, it is supported by the system */
	  if ( rfdevice_cdma_is_dlna_supported(rx_devices[device_index])
		   == TRUE )
	  {
		return TRUE;
	  }
	  
	} /* for ( device_index = 0; device_index < num_rx_devices;...*/
	
	return FALSE;


} /* is_dlna_supported */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and gets power mode
 
  @return
  Recommended power mode of operation
*/
rfm_cdma_power_mode_type 
rfc_cdma_device_wrapper::get_ic_state
(
  void
)
{
  uint8 device_index;
  rfm_cdma_power_mode_type power_mode = RFM_CDMA_POWER_MODE_MAX;
  rfm_cdma_power_mode_type device_power_mode;

  if ( is_ic_supported() == TRUE )
  {
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      if ( rfdevice_cdma_is_ic_supported(rx_devices[device_index]) 
           == TRUE)
      {
        device_power_mode = 
          rfdevice_cdma_get_ic_state(rx_devices[device_index]);

        if ( device_power_mode < power_mode )
        {
          power_mode = device_power_mode;
        }
      }
    } /* for ( device_index = 0; device_index < num_rx_devices;...*/
  } /* if ( rfc_cdma_rxdev_wrapper_is_intelliceiver_supported(rx_device) == TRUE ) */
  else
  {
    power_mode = RFM_CDMA_POWER_MODE_NORMAL;
  }

  return power_mode;

} /* rfc_cdma_device_wrapper::get_ic_state */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and sets intelliceiver cal
  data
 
  @param cal_data
  Pointer to the intelliceiver calibration data to be pushed into the device

  @param band
  CDMA band for which calibration data is being set in device

  @param power_mode
  CDMA Power mode for which calibration data should be applied to device
*/
boolean
rfc_cdma_device_wrapper::set_ic_cal_data
(
  rfdevice_cdma_ic_thres_param_type* cal_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode
)
{
  uint8 device_index;
  boolean status = FALSE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_set_ic_cal_data( rx_devices[device_index],
                                   cal_data,
                                   band,
                                   power_mode ) == TRUE )
    {
      status = TRUE;
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::set_ic_cal_data */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and performs intelliceiver cal
 
  @details
  Current interface can perform calibration only on max 1 device. The first
  device that supports IntelliCeiver will be calibrated in the current
  implementation
 
  @param power
  Power at which intelliceiver is calibrated
 
  @return
  Structure containing the intellicever calibration data which contains the
  threshold gain at which the intelliceiver tripped.
*/
rfdevice_cdma_ic_cal_data_type 
rfc_cdma_device_wrapper::ic_calibration
(
  int16 power
)
{
  uint8 device_index;
  rfdevice_cdma_ic_cal_data_type cal_data = {0, 0};
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_is_ic_supported(rx_devices[device_index])
         == TRUE)
    {
      return rfdevice_cdma_ic_calibration( rx_devices[device_index],
                                           power );
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  RF_MSG( RF_ERROR, "rfc_cdma_rxdev_wrapper_do_intelliceiver_cal. No device"
                    "supports intelliceiver" );
  
  return cal_data;

} /* rfc_cdma_device_wrapper::ic_calibration */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and gets power mode
 
  @param *rx_device
  Pointer to wrapper CDMA RX device object whose devices need to be queried
  for power mode
 
  @return
  Recommended power mode of operation
*/
rfdevice_cdma_ic_power_mode_config_type 
rfc_cdma_device_wrapper::get_ic_pm_config
(
  rfm_cdma_band_class_type band,
  uint32 rx_bw_khz,
  rfdevice_trx_cdma_rx* paired_device,
  rfdevice_rx_mode_type rx_mode
)
{
  uint8 device_index;
  rfdevice_cdma_ic_power_mode_config_type power_mode_cfg = 
    RFDEVICE_CDMA_IC_PM_CONFIG_INVALID;
  rfdevice_cdma_ic_power_mode_config_type device_power_cfg;

  if ( is_ic_supported() == TRUE )
  {
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      if ( rfdevice_cdma_is_ic_supported(rx_devices[device_index]) 
           == TRUE)
      {
        device_power_cfg = 
          rfdevice_cdma_get_ic_pm_config( rx_devices[device_index],
                                          band,
                                          rx_bw_khz );

        if ( device_power_cfg == RFDEVICE_CDMA_IC_ADAPTIVE_PM )
        {
          return device_power_cfg;
        }
        else if ( device_power_cfg < RFDEVICE_CDMA_IC_PM_CONFIG_INVALID )
        {
          if ( device_power_cfg < power_mode_cfg )
          {
            power_mode_cfg = device_power_cfg;
          }
        }
        else
        {
          RF_MSG_2( RF_ERROR, "rfc_cdma_rxdev_wrapper_get_power_mode_config:"
                              "Invalid result %d from device %d",
                    device_power_cfg, device_index );
        }
      } /* is_ic_supported */
    } /* for ( device_index = 0; device_index < num_rx_devices;...*/
  } /* if ( rfc_cdma_rxdev_wrapper_is_intelliceiver_supported(rx_device) == TRUE ) */
  else
  {
    power_mode_cfg = RFDEVICE_CDMA_IC_FIXED_PM_0;
  }

  return power_mode_cfg;

} /* rfc_cdma_device_wrapper::get_ic_pm_config */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and resets Jammer detector
 
*/
boolean
rfc_cdma_device_wrapper::ic_reset_jammer_detector
(
  void
)
{
  uint8 device_index;
  boolean status = FALSE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_ic_reset_jammer_detector( rx_devices[device_index] )
         == TRUE)
    {
      status = TRUE;
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;

} /* rfc_cdma_device_wrapper::ic_reset_jammer_detector */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Loops thru all rx_devices in the wrapper object and gets LNA phase offset
 
  @details
  The final LNA phase offsets are the sum of offsets for each gain state from
  each receive device. A modulus 360 operation is done to wrap around phase
  offset if it exceeds 360 degrees
 
  @param band
  RF CDMA band for which the LNA phase offset is queried
 
  @param chan
  Channel for which LNA phase offset is queried.
 
  @param power_mode
  Current power mode for which the LNA phase offsets are required.
 
  @param state
  RF CDMA state of receive device. Settings may be different for Diversity vs
  SHDR mode
 
  @param rx_bw_khz
  RX carrier Bandwidth in kHz. Used to select bandwidth specific phase offset
  for MCDO
 
  @param lna_phase
  Container to populate the LNA phase offsets for given configuration
 
  @retval TRUE
  LNA offset was retrieved successfully for all device
 
  @retval FALSE
  Atleast one RX device did not provide valid LNA offset
*/
boolean 
rfc_cdma_device_wrapper::get_lna_phase_offsets
(
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  rfm_cdma_power_mode_type power_mode,
  rf_cdma_state_type state,
  uint32 rx_bw_khz,
  rfdevice_cdma_lna_phase_offset_type *lna_phase
)
{
  uint8 device_index, gain_index;
  boolean overall_status = TRUE;
  boolean dev_status;
  rfdevice_cdma_lna_phase_offset_type lna_phase_for_device;

  for (gain_index = 0; gain_index < RFM_LNA_GAIN_STATE_NUM; gain_index++)
  {
    lna_phase->offset_in_deg[gain_index] = 0;
  }
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    /* If any device supports intelliceiver, it is supported by the system */
    dev_status = 
      rfdevice_cdma_get_lna_phase_offsets( rx_devices[device_index],
                                           band,
                                           chan,
                                           power_mode,
                                           state,
                                           rx_bw_khz,
                                           &lna_phase_for_device );

    if (dev_status == TRUE)
    {
      for (gain_index = 0; gain_index < RFM_LNA_GAIN_STATE_NUM; gain_index++)
      {
        lna_phase->offset_in_deg[gain_index] += 
          lna_phase_for_device.offset_in_deg[gain_index];

        /* Wrap around if offset exceeds 360 degrees */
        if ( lna_phase->offset_in_deg[gain_index] >= 360 )
        {
          lna_phase->offset_in_deg[gain_index] -= 360;
        }
      }
    }
    else
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_get_lna_phase_offsets. "
                          "Device %d!",
                device_index );
      overall_status = FALSE;
    }
    
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return overall_status;

} /* rfc_cdma_device_wrapper::get_lna_phase_offsets */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Set band data not supported by wrapper object
 
  @details
  set_band_data_fp must be called to push band port info for a particular
  rx device for a band. This cannot be supported via wrapper object as the data
  to be pushed in is unique for each physical rx device. Hence the
  set_band_data_fp must be called independently
 
  @param band
  Unused
 
  @param data_array
  Unused
 
  @param array_size
  Unused
 
  @return
  Always returns FALSE as this API is not supported
*/
boolean
rfc_cdma_device_wrapper::set_band_data
(
  rfm_cdma_band_class_type band,
  int32* data_array,
  uint8 array_size
)
{
  RF_MSG( RF_ERROR, "rfc_cdma_rxdev_wrapper_set_band_data - not supported! Call"
                    " individual devices for this function" );
  return FALSE;

} /* rfc_cdma_device_wrapper::set_band_data */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Gets script to set LNA to a particular gain state
 
  @details
  This function should be used to get LNA gain word script for one LNA state
  at a time. Also it populates the script in rf_buffer_intf* provided
 
  The wrapper will loop thru all RX devices and append their LNA gain scripts
  in the same buffer
 
  @param band
  CDMA band whose LNA gain word is required.
 
  @param power_mode
  Linearity of operation
 
  @param rx_bw_khz
  BW in khz of operation. LNA gain word may be different for single carrier
  vs multi carrier
 
  @param gain_state
  LNA gain state whose script is needed
 
  @param buf
  Pointer to rf script buffer object where LNA gain script will be populated
 
  @retval TRUE
  LNA gain word script was successfully populated for the arguments provided
 
  @retval FALSE
  LNA gain word script population failed! buf ptr was not updated
*/
boolean
rfc_cdma_device_wrapper::create_lna_gain_script
(
  rfm_cdma_band_class_type band,
  rfm_cdma_power_mode_type power_mode,
  uint32 rx_bw_khz,
  rfm_lna_gain_state_type gain_state,
  rf_buffer_intf* buf,
  rfdevice_trx_cdma_rx* paired_device,
  rfdevice_rx_mode_type rx_mode
)
{
  uint8 device_index;
  boolean overall_status = TRUE;
  boolean curr_dev_status;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    curr_dev_status = rfdevice_cdma_create_rx_lna_gain_script( 
                        rx_devices[device_index],
                        band,
                        power_mode,
                        rx_bw_khz,
                        gain_state,
                        buf );

    if ( curr_dev_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfdevice_cdma_create_rx_lna_gain_script failed for "
                          "device# %d",
                device_index );
      overall_status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return overall_status;

} /* rfc_cdma_device_wrapper::create_lna_gain_script */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Gets scripts for Rx on/off from the WTR/WFR
 
  @details
  This function should be used to get Rx on/off scripts
  Also it populates the script in rf_buffer_intf* provided
 
  The wrapper will loop thru all RX devices and append their rx on/off  
  scripts in the same buffer
 
  @param rx_on_off
  on or off scripts
 
  @param on_off_setting_buf
  Pointer to rf script buffer object where script will be populated
 
  @param time_delay_us
  time delay to be added to script
 
  @retval TRUE
  Script was successfully populated for the arguments provided
 
  @retval FALSE
  Script population failed! buf ptr was not updated
*/
boolean
rfc_cdma_device_wrapper::get_rx_on_off_script
(
  boolean rx_on_off,
  rf_buffer_intf* on_off_setting_buf,
  int16 time_delays_us
)
{
  uint8 device_index;
  boolean overall_status = TRUE;
  boolean curr_dev_status;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    curr_dev_status = rfdevice_cdma_rx_get_rx_on_off_script( 
                                         rx_devices[device_index], 
                                         rx_on_off, 
                                         on_off_setting_buf,
                                         time_delays_us);

    if ( curr_dev_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfdevice_cdma_rx_get_rx_on_off_script failed for "
                          "device# %d",
                device_index );
      overall_status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return overall_status;

} /* rfc_cdma_device_wrapper::get_rx_on_off_script */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Accesses the command dispatch interface for rx devices in wrapper object
 
  @details
  Each command is handled uniquely based on its requirements.
 
  @param cmd
  Specifies the command to performed by the device. Should be from enum
  rfdevice_cmd_enum_type

  @param *data
  Void data pointer to handle input and/or output parameter for the command.
 
  @return
  Final status of command dispatch after executing on all devices in the
  wrapper. FALSE indicates failures, TRUE indicates overall success
*/
boolean 
rfc_cdma_device_wrapper::cmd_dispatch
(
  rfdevice_cmd_enum_type cmd,
  void* data
)
{
  uint8 device_index;
  boolean status = TRUE; /* Overall status of this function */
  boolean current_status = FALSE; /* Used to track status within commands */
  int16 nominal_bbf_gain = 0;
  int16* final_bbf_gain = (int16*)data;
  

  switch (cmd)
  {
  case RFDEVICE_GET_NOMINAL_BBF_GAIN_DB10:

    /* Initial final result to 0 */
    *final_bbf_gain = 0;

    /* Loop thru each device and add their bbf gains */
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      nominal_bbf_gain = 0;

      if ( rfdevice_cdma_rx_cmd_dispatch(rx_devices[device_index],
                                         RFDEVICE_GET_NOMINAL_BBF_GAIN_DB10,
                                         &nominal_bbf_gain) == TRUE )
      {
        *final_bbf_gain  += nominal_bbf_gain;
      }
      else
      {
        RF_MSG_2( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch cmd %d failed"
                            "for device %d ",
                  cmd, device_index );
        status = FALSE;
      }
    } /* for loop on device_index */
    break;

  case RFDEVICE_GET_RX_RECOMMENDATION:
  case RFDEVICE_GET_RX_SYNC_LOCK:
  case RFDEVICE_GET_IQMC_COEFS:
    /* Loop thru each device and execute command dispatch.
    Only 1 of the devices should provide valid result */
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      if ( rfdevice_cdma_rx_cmd_dispatch(rx_devices[device_index],
                                         cmd,
                                         data) == TRUE )
      {
        if ( current_status != TRUE )
        {
          current_status = TRUE;
        }
        else
        {
          RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch failed for "
                              "cmd %d: More than 1 device returned valid data ",
                    cmd );
          status = FALSE;
        }
      } /* if rfdevice_cdma_rx_cmd_dispatch == TRUE */
    } /* for loop on device_index */

    if ( current_status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch. All devices "
                          "failed for cmd %d!", cmd );
      status = FALSE;
    }
    break;

  case RFDEVICE_GET_LNA_STATE:
    RF_MSG( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch. "
                      "RFDEVICE_GET_LNA_STATE not yet implemented " ) ;
    break;

  case RFDEVICE_ADJUST_RF_CAL_FOR_HDR:
  case RFDEVICE_MULTI_CARRIER_DROOP_COMPENSATION:

    /* Default status to FALSE and change to TRUE if any device gives valid
    result */
    status = FALSE;

    /* Loop thru each device and to get HDR RF cal adjustment. 
    For now, use the value from first device that returns valid data. */
    /*! @todo Should expand to handle different gain adjustments for multiple 
      devices if needed */
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      
      if ( rfdevice_cdma_rx_cmd_dispatch(rx_devices[device_index],
                                         cmd,
                                         data) == TRUE )
      {
        status = TRUE;
        break;
      }
    } /* for loop on device_index */

    if ( status == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch. All devices "
                          "failed for cmd %d!", cmd );
    }
    break;

  case RFDEVICE_GET_COMM_INFO:
    for ( device_index = 0; 
          device_index < num_rx_devices;
          device_index++ )
    {
      rfdevice_cdma_rx_cmd_dispatch(rx_devices[device_index],
                                    cmd,
                                    data);
      RF_MSG_1( RF_HIGH, "rfc_cdma_rxdev_wrapper_cmd_dispatch. Get COMM,"
                         "device_index %d",
                device_index );
    }
    
    break;

  default:
    RF_MSG_1( RF_ERROR, "rfc_cdma_rxdev_wrapper_cmd_dispatch. Unsupported"
                        " command %d ", cmd ) ;
    status = FALSE;
    break;

  } /* switch (cmd) */
  
  return status;

} /* rfc_cdma_device_wrapper::cmd_dispatch */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This API will load cdma mcdo data into the input paramter based on 
  selected bandwidth. This API may be overrided by different sub classes.  

  @param data
  data buffer where cdma mcdo data will be stored, should be allocated 
  prior to this function call

  @param bwHz
  Bandwidth selected for specific mcdo data

  @return
  Success/Failure flag

*/
boolean 
rfc_cdma_device_wrapper::multi_carrier_droop_compensation
(
rfdevice_cdma_multi_carrier_droop_compensations_type_v2 *data,
uint32 bwHz
)
{
  uint8 device_index;
  boolean status = TRUE;
  
  for ( device_index = 0; 
        device_index < num_rx_devices;
        device_index++ )
  {
    if ( rfdevice_cdma_rx_multi_carrier_droop_compensation( rx_devices[device_index],
																 data,
																 bwHz)==FALSE)
    {
      RF_MSG_1( RF_ERROR, "rfc_cdma_device_wrapper::cdma_multi_carrier_droop_compensation"
                          " failed for device# %d",
							device_index );
      status = FALSE;
    }
  } /* for ( device_index = 0; device_index < num_rx_devices;...*/

  return status;
} /*multi_carrier_droop_compensation*/


/*----------------------------------------------------------------------------*/
/*!
  @brief
  "Constructor" of a CDMA RX device wrapper object

  @details
  Accepts up to 2 CDMA RX devices and returns a wrapper object that contains
  these devices as its data. This wrapper object is implemented to execute
  functions on each of the wrapped devices.
 
  Wrapper objects have been statically declared. Next available object is
  provided.
 
  If the requested wrapper object has already been created, a pointer to the
  same object is provided to save memory
 
  @param rx_device_0
  Pointer to first CDMA RX device
 
  @param rx_device_1
  Pointer to second CDMA RX device in wrapper
 
  @return
  Returns a CDMA RX device that wraps the 2 objects provided as inputs. Will
  return in case of failure to construct.
*/
rfc_cdma_device_wrapper::rfc_cdma_device_wrapper
(
  rfdevice_trx_cdma_rx* rx_device_0, 
  rfdevice_trx_cdma_rx* rx_device_1
)
{
  if ( (rx_device_0 == NULL) && (rx_device_1 == NULL) )
  {
    RF_MSG( RF_ERROR, "rfc_cdma_device_wrapper constructor: RX ptrs are NULL" );
    return;
  }

  num_rx_devices = 0;

  if ( rx_device_0 != NULL )
  {
    rx_devices[num_rx_devices] = rx_device_0;
    num_rx_devices++;

    /* Always returning rx_device_0 id and type for now - need to revisit for
    wrapper object if needed */
    rf_device_id = rx_device_0->rfdevice_id_get();
    rf_device_type = rx_device_0->rfdevice_type_get();
  }

  if ( rx_device_1 != NULL )
  {
    rx_devices[num_rx_devices] = rx_device_1;
    num_rx_devices++;

    if ( rx_device_0 == NULL )
    {
      /* Need to set rf_device_id incase rx_device_0 is NULL */
      rf_device_id = rx_device_1->rfdevice_id_get();
      rf_device_type = rx_device_1->rfdevice_type_get();
    }
  }

  

} /* rfc_cdma_device_wrapper::rfc_cdma_device_wrapper */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Empty destructor
*/
//rfc_cdma_device_wrapper::~rfc_cdma_device_wrapper()
//{
//}
#endif /* FEATURE_CDMA1X */

/*! @} */
