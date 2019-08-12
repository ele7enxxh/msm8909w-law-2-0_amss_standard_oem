/*! 
  @file
  rf_1x_power_limiting.c
 
  @brief
  This file contains the Power Limiting module of 1x driver.

  @details
  The power limiting module is responsible for keeping track of the current
  Transmit power limits accounting for all factors like HDET feedback, 
  temperature, SAR, SVDO configuration etc. And updating the TX power limit
  to firmware whenever applicable.

  @image html rf_cdma_plim_state_machine.jpg
 
  @addtogroup RF_CDMA_COMMON_PLIM
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/rf/src/rf_1x_power_limiting.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/06/14   spa     Change rx_nv_path to be per band per logical device
09/06/13   cc     Update TxPwr limit when updating external limit.
12/03/12   APU     Fixed compiler warning.
12/03/12   APU     Added a one time check for therm cal monotonicity. 
09/05/12   aro     Added RF-FW Sync callback parameter in MDSP Interface
08/24/12   cd      CDMA EPT Temp and HDET feature support
08/06/12   hdz     Changed rf_1x_plim_get_lowest_max_power_limit() to access
                   rf_cdma_data instead of ftm_cdma_data.
07/24/12   hdz     Changed rf_1x_plim_get_lowest_max_power_limit() to support 
                   tx_pwr_limit override;
04/02/12   aro     Added CB as argument in Rx / Tx MDSP interfaces
02/06/12   cd      Changed the get lowest max power limit function to be 
                   a public API
02/03/12   hdz     Updated lin_limit unconditionally 
01/27/12   hdz     changed rf_1x_plim_update_temp and rf_1x_plim_update_ 
                   temp_mdsp to static type function 
01/27/12   hdz     Remove fcomp from max power limit module 
01/19/12   cd      Move PLIM linearizer limit configuration to PLIM module and 
                   store the limit in dBm10
12/08/11   hdz     Added support for rf_1x_plim_update_temp()
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
11/03/11   cd      Provide max linearizer entry info to power-limiting 
10/12/11   bmg     Added DSI SAR control to target max power limit
09/29/11   shb     Saturate max power within dynamic range before sending to FW
09/13/11   shb     Added PLIM state machine image to doxy documentation
09/06/11   shb     Corrected documentation
09/06/11   shb     Added rf_1x_plim_get_err_adj_max_limit
08/31/11   shb     Added debug message
08/30/11   shb     Renamed rf_1x_mdsp_configure_tx_power_limit_db640
08/29/11   shb     Added rf_1x_plim_enable/disable_override()
08/29/11   shb     Created helper function: rf_1x_plim_update_limits()
08/24/11   shb     Accept TX err from HDET in raw signed db640 tx agc units 
                   without any rounding to avoid losing precision
08/24/11   shb     Appended db* to plim data names for clarity of units since 
                   some members are dbm10 while others are db640 (signed tx agc)
08/23/11   shb     Added plim_supported flag in plim_data
08/19/11   shb     Fixes to update power limit based on HDET error correctly
08/18/11   shb     Initial version - implemented start/stop and hdet err update
                   APIs

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_1x_power_limiting.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h" /* rf_cdma_get_turnaround_constant() */
#include "rf_cdma_data.h" /* rf_cdma_get_mutable_device_status etc.. */
#include "rfm_internal.h" /* rfm_get_calibration_state() */
#include "rf_1x_mdsp.h"   /* rf_1x_mdsp_configure_tx_power_limit() */
#include "rf_cdma_mdsp.h" /* rf_cdma_mdsp_dbm10_to_txagc() */
#include "rfcommon_math.h" /* rf_limit_s32 */
#include "rfcommon_core_sar.h" /* rfcommon_core_sar_get_state() */
#include "ftm.h"
#include "rf_cdma_utils_freq.h"

/*============================================================================*/
/*!
  @name Helper Functions

  @brief
  Helper functions used for miscellaneous functionality needed by CDMA
  power limiting algorithm
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to calculate the lowest max power limit from all the
  contributing modules
*/
int16
rf_1x_plim_get_lowest_max_power_limit
(
  rf_cdma_max_power_limits_type* max_power_limits,
  rfm_device_enum_type device  
)
{
  int16 lowest_max_power;
  const rf_cdma_data_status_type* dev_status_r; /* Device Status Read Pointer */

  if ( rfcommon_core_sar_get_state() == RF_SAR_STATE_DEFAULT )
  {
    /* Always use lim_vs_temp_vs_freq for SAR State 0 */
    lowest_max_power = max_power_limits->max_power_limit_vs_temp_vs_freq_dbm10;
  }
  else
  {
    /* For SAR states other than 0, obtain min of Lim vs Temp and
    Lim vs Temp vs Freq */
    lowest_max_power = MIN(max_power_limits->max_power_limit_vs_temp_vs_freq_dbm10,
                           max_power_limits->max_power_limit_sar_dbm10 );
  }

  /* Obtain min of current lowest and SV */
  lowest_max_power = MIN(lowest_max_power, 
                         max_power_limits->max_power_limit_sv_dbm10 );
 
  /* Obtain min of current lowest and Thermal Backoff */
  lowest_max_power = MIN(lowest_max_power, 
                         max_power_limits->max_power_limit_thermal_backoff_dbm10 );


  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device );
  
  if ( dev_status_r != NULL )
  {
    lowest_max_power = MIN(lowest_max_power, 
                       dev_status_r->plim.tx_override_pwr_limit_dbm10 );
  }

  return lowest_max_power;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update local copy of max_pwr_limit according to therm reading
  
  @details
  Obtain the max power limit for the current therm reading using interpolation 
  algorithm.

  @param device
  Device whose plim override needs to be disabled

  @return
  Flag indicating whether updating is successful or not.
*/
static boolean 
rf_1x_plim_update_temp
(
  rfm_device_enum_type device
)
{

  const rf_cdma_data_status_type* dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  
  const rf_1x_config_type* rf_1x_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;
  boolean plim_update_status = FALSE;

  uint32 tx_therm_cal[RF_CDMA_NV_TEMP_LIMIT_SIZE];
  int16  max_pwr_limit[RF_CDMA_NV_TEMP_LIMIT_SIZE];
  uint8 i;
  
  /* Get Writable Device Status */
  dev_status_r = rf_cdma_get_device_status( device );
  
  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );
    
  /* Extract plim_data from device data */
  plim_data = rf_cdma_get_mutable_plim( device );

  if ( (dev_status_r == NULL)
       || (plim_data == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_plim_update_temp: Invalid device: "
                        "%d", device );
    return FALSE;
  }
  
  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_1x_plim_update_temp: Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }
  
  rf_1x_config_data_ptr = rf_cdma_nv_get_1x_data(
                                       logical_dev->rx_nv_path[plim_data->band],
                                       plim_data->band );
  if ( rf_1x_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "rf_1x_plim_update_temp" );
    return FALSE;
  }
  
  rf_cdma_config_data_ptr = rf_1x_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "rf_1x_plim_update_temp" );
    return FALSE;
  }

  /*copy NV to local variable from unit16 to uint32 */
  if ( rf_cdma_config_data_ptr->tx_cal.therm_cal_data.is_therm_cal_nv_data_valid ) 
  {
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      tx_therm_cal[i] = 
        (uint32) rf_cdma_config_data_ptr->tx_cal.therm_cal_data.tx_therm_cal[i];

      max_pwr_limit[i] = 
        rf_cdma_config_data_ptr->tx_static.lim_vs_temp[rfcommon_core_sar_get_state()][i];
    }
  

    /* If last temp comp read failed, skip temp comp by setting temp_comp_offset
    to zero; otherwise, do interpolation */
    if ( dev_status_r->temp_comp.is_last_reading_valid == TRUE )
    {
      plim_data->max_power_limits.max_power_limit_sar_dbm10
        = rf_lininterp_u32_s16( tx_therm_cal,
                                max_pwr_limit,
                                RF_CDMA_NV_TEMP_LIMIT_SIZE,
                                (uint32)dev_status_r->temp_comp.last_temp_sensor_read);
      plim_update_status = TRUE;
    }
    else
    {
      plim_update_status = FALSE;
    }
  } /* is_therm_cal_nv_data_valid */
  else 
  {
    plim_update_status = FALSE;
  }

  return plim_update_status;

}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to calculate max power limit in db640 and update to firmware
  
  @details
  Every time an update is provided to power limiting module, the max power limit
  should be recalculated and updated in firmware. Hence this static helper 
  function has been created.

  It computes the max power limit in db640 (signed firmware tx agc units) from
  max power limit in dbm10 and tx error from hdet in db640.

  This limit is then applied to firmware
  
  @param tx_mdsp_path
  MDSP path to which final min and max power limits should be applied

  @param plim_data
  A structure containing the current power limit information
 
  @param k10
  Turnaround constant x 10. Used to convert powers in dbm10 to tx agc in db640

  @param *max_plim_err_adj_db640
  Output: Max power limit in db640 computed by this function. This is the limit
  sent to firmware
*/
static void
rf_1x_plim_update_limits
(
  rfm_device_enum_type device, 
  rf_cdma_mdsp_chain_enum_type tx_mdsp_path,
  rf_cdma_plim_data_type* plim_data,
  int16 k10,
  int16* max_plim_err_adj_db640
)
{
  int16 max_plim_dbm10;
  int32 max_plim_err_adj_db640_unlimited;
  int16 max_lin_limit_db640;

  /* Obtain the max power limit from all contributing modules */
  max_plim_dbm10 = rf_1x_plim_get_lowest_max_power_limit( 
    &plim_data->max_power_limits, device );

  /* Subtract err reported by HDET in dB640. Negative tx_err_from_hdet implies, 
  actual tx power is greater than desired power. Hence subtract this -ve err 
  to current power limit to reduce it. If tx err is positive, actual tx power 
  is lower than desired power hence we subtract this positive number to 
  increase the max power limit */
  max_plim_err_adj_db640_unlimited = 
    rf_cdma_mdsp_dbm10_to_txagc( max_plim_dbm10, k10 ) - plim_data->tx_err_from_hdet_db640;

  /* Saturate max power limit within TX AGC min & max values before sending
  to FW */
  *max_plim_err_adj_db640 = 
    (int16)rf_limit_s32( max_plim_err_adj_db640_unlimited, 
                         RF_CDMA_MDSP_MIN_TX_PWR,
                         RF_CDMA_MDSP_MAX_TX_PWR );

  /* Take the minimum of the linearizer-provided power limit and the
  HDET suggested power limit to provide to FW. This is to prevent FW
  from trying to meet a power limit that the linearizer itself cannot
  support. In TxAGC units, lower power corresponds to higher TxAGC
  value */
  max_lin_limit_db640 = rf_cdma_mdsp_dbm10_to_txagc(
                            plim_data->max_power_linearizer_limit_dbm10, k10 );

  if ( max_lin_limit_db640 > *max_plim_err_adj_db640 )
  {
    RF_MSG_2 (RF_HIGH, "1x PLIM: Using linearizer based limit %d(dBm10), "
                       "which is lower than desired power limit %d(dBm10)",
              plim_data->max_power_linearizer_limit_dbm10,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(
                                      plim_data->band),
                                    *max_plim_err_adj_db640) );

    *max_plim_err_adj_db640 = max_lin_limit_db640;
  }

  /* Apply new limit to firmware */
  rf_1x_mdsp_configure_tx_power_limit_db640
    ( tx_mdsp_path,
      rf_cdma_mdsp_dbm10_to_txagc(plim_data->min_power_limit_dbm10, k10),
      *max_plim_err_adj_db640, NULL, NULL, NULL );
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update local copy of max_pwr_limit to mdsp
  
  @details
  Update the latest max_pwr_limit that is calculated in 
  rf_hdr_plim_update_temp() to mdsp

  @param plim_data
  Pointer to plim_data

  @param device
  Device whose plim override needs to be disabled

  @param rf_cdma_config_data_ptr
  Pointer to cdma common NV data

  @param logical_dev
  Pointer to logical device
*/
static void
rf_1x_plim_update_temp_mdsp
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
)
{
  int16 k10;

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1X_MODE) )
  {
    /* Only update the limit if the power limiting module is running,
       otherwise it will be updated by default the next time the module
       is started. */

    k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

    /*Update plim_max_pwr according to temp reading*/ 
    if ( rf_1x_plim_update_temp( device ) == FALSE ) 
    { 
       /* No active temp measurement updates yet, so always use index 0
             directly from the temp comp table. */
       plim_data->max_power_limits.max_power_limit_sar_dbm10
         = rf_cdma_config_data_ptr->tx_static.lim_vs_temp[rfcommon_core_sar_get_state()][0];
    }

    /* Recalculate max power limit taking new TX err from HDET in account.
    Send new limits to MDSP */
    rf_1x_plim_update_limits( device,
                              logical_dev->tx_mdsp_path,
                              plim_data,
                              k10,
                              &plim_data->max_power_limit_err_adj_db640 );

  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */


}
/*! @} */

/*============================================================================*/
/*!
  @name 1x Power Limiting APIs

  @brief
  Interface functions from 1x power limiting module to start/stop/update or
  override power limiting.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts 1x TX power limiting on a given device
  
  @details
  Calculates initial max and min power limits by querying information from 
  all applicable modules. Updates initial limits to firmware.

  Plim module for the given device is "enabled" and accepts updates from 
  clients. Eg: TX AGC error updates from HDET

  rf_cdma_plim_init() should be called once to initialize internal PLIM data 
  on a device before attempting to start power limiting. If plim has been 
  enabled previously for a technology - 1x/hdr, it should be disabled by 
  calling rf_<1x,hdr>_plim_stop() before starting it again.
  
  @param device  
  RF device on which the TX power limiting is to be performed

  @param *plim_data  
  Pointer to the data structure that contains power limiting data for a given 
  device
 
  @return
  TRUE if Power Limiting module has been successfully "enabled" on this device. 
  FALSE otherwise

  @todo
  Initial max power limit is currently hardcoded. Should come from LimvsTemp NV
*/
boolean
rf_1x_plim_start
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band
)
{
  boolean status = TRUE;
  int16 k10 = 10*rf_cdma_get_turnaround_constant(band);
  const rf_1x_config_type* rf_1x_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;

  if ( (plim_data == NULL) || 
       (plim_data->init_done == FALSE) ||
       (plim_data->plim_supported == FALSE) ||
       (logical_dev == NULL) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: Cannot start Power Limiting on device %d, "
              ,device );
    return FALSE;
  }
  if ( rfm_get_calibration_state() == TRUE )
  {
    RF_MSG( RF_LOW, "1x PLIM: Not starting Power Limiting loop in calibration "
                    "mode" );
    return FALSE;
  }

  rf_1x_config_data_ptr = rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[band]
                                                 , band );
  if ( rf_1x_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "1x PLIM: Invalid 1x NV config structure" );
    return FALSE;
  }

  rf_cdma_config_data_ptr = rf_1x_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "1x PLIM: Invalid common CDMA NV config structure" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( plim_data->state == RF_CDMA_PLIM_DISABLED )
  {
    plim_data->mode = RFM_1X_MODE;
    plim_data->band = band;

    /* Set default max/min power limits if not in override state */
    if ( plim_data->override_enabled == FALSE )
    {
      /* Min power in dBm is always -(-12.7dBm) + K:
      -60.3 for low bands, -63.3 for high bands. */
      plim_data->min_power_limit_dbm10 = -(-127) + 
        10 * rf_cdma_get_turnaround_constant(band);

      /*Update plim_max_pwr according to temp reading*/ 
      if ( rf_1x_plim_update_temp( device ) == FALSE ) 
      { 
         /* No active temp measurement updates yet, so always use index 0
               directly from the temp comp table. */
         plim_data->max_power_limits.max_power_limit_sar_dbm10
           = rf_cdma_config_data_ptr->tx_static.lim_vs_temp[rfcommon_core_sar_get_state()][0];
      }
    }

    /* Zero TX error from HDET to start */
    plim_data->tx_err_from_hdet_db640 = 0;

    /* Calculate error adjusted power limits in FW units and send
    final limits to MDSP */
    rf_1x_plim_update_limits( device,
                              logical_dev->tx_mdsp_path,
                              plim_data,
                              k10,
                              &plim_data->max_power_limit_err_adj_db640 );
 
    /* Set state to "Enabled" so PLIM now accepts updates from modules */
    plim_data->state = RF_CDMA_PLIM_ENABLED;

    if ( plim_data->debugging_enabled == TRUE )
    {
      RF_MSG_5( RF_LOW, "1x PLIM: Started. Max plim SAR(dbm10) %d, "
                        "Max plim vs temp vs freq %d (dbm10) min plim (dbm10)"
                        " %d, tx err from hdet(db640) %d, "
                        "max plim err adj(db640) %d",
                plim_data->max_power_limits.max_power_limit_sar_dbm10,
                plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
                plim_data->min_power_limit_dbm10,
                plim_data->tx_err_from_hdet_db640,
                plim_data->max_power_limit_err_adj_db640 );
    }
  }
  else
  {
    /* Plim not in disable state. Not safe to re-enable. */
    RF_MSG_2( RF_ERROR, "1x PLIM: Cannot start plim unless in disabled state on "
                        "device %d, curr state: %d", device, plim_data->state );
    status = FALSE;
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables 1x power limiting on a device
  
  @details
  Puts 1x PLIM module on this device in "disabled" state. It will no longer
  accept updates from clients or update them to device.
  
  @param device  
  RF device on which the TX power limiting is to be disabled

  @param *plim_data  
  Pointer to the data structure that contains power limiting data for a given 
  device
 
  @return
  TRUE if Power Limiting module was successfully disabled. FALSE otherwise
*/
boolean
rf_1x_plim_stop
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data
)
{
  if ( plim_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: Cannot stop Power Limiting on device %d, "
                        "NULL data", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  plim_data->state = RF_CDMA_PLIM_DISABLED;
  plim_data->mode = RFM_PARKED_MODE;

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates max tx power limit based on hdet feedback
  
  @details
  RF CDMA HDET module should use this API to update tx error in the 1x power 
  limiting module for a device.
  
  @param device
  Logical device whose error is being updated. This function queries PLIM
  data and other info like logical_dev based on this param
  
  @param tx_err_db640
  TX power error in dB640 units to be consistent with FW AGC units. Is the 
  difference between desired tx power and actual tx power as determined by 
  hdet. A negative number implies actual tx power is greater than desired 
  power and vice versa.
  
  @return
  TRUE if the update was accepted and applied to firmware. FALSE if power
  limiting loop was unable to do so.
*/
boolean
rf_1x_plim_update_tx_err_from_hdet
(
  rfm_device_enum_type device,
  int16 tx_err_db640
)
{
  const rf_cdma_data_status_type* dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  int16 k10;

  /* Get Writable Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Extract plim_data from device data */
  plim_data = rf_cdma_get_mutable_plim( device );

  if ( (dev_status_r == NULL) || (logical_dev == NULL) || ( plim_data == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_tx_err_from_hdet(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_tx_err_from_hdet(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1X_MODE) )
  {
    /* Valid state - can accept this update */
    plim_data->tx_err_from_hdet_db640 = tx_err_db640;

    /* Recalculate max power limit taking new TX err from HDET in account.
    Send new limits to MDSP */
    rf_1x_plim_update_limits( device,
                              logical_dev->tx_mdsp_path,
                              plim_data,
                              k10,
                              &plim_data->max_power_limit_err_adj_db640 );

    status = TRUE;
  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */
  else
  {
    RF_MSG_3( RF_HIGH, "1x PLIM: Cannot update power limit for device: %d,"
                       "Mode %d, state: %d", 
              device, plim_data->mode, plim_data->state );
    status = FALSE;
  }

 // if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_7( RF_LOW, "1x PLIM: Update err from HDET. Status: %d, device: %d, "
                      "TX err from HDET(dB640): %d, actual SAR "
                      "max pwr lim(dBm10): %d, lim_vs_temp_vs_freq %d(dBm10)"
                      " Err adjusted limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->tx_err_from_hdet_db640, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to let the power limit module know the SAR state has changed.

  @details
  This function is called from 1x MC when it is informed that the SAR state
  has changed, so that the max power limit module may re-compute the target
  max power with the new SAR state.

  SAR state is queried externally from the rfcommon_core_sar_get_state() API.
*/
boolean
rf_1x_plim_update_sar_state
(
  rfm_device_enum_type device
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  const rf_1x_config_type* rf_1x_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;

  /* Get Writable Device Status */
  plim_data = rf_cdma_get_mutable_plim( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (plim_data == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_update_sar_state(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_update_sar_state(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  rf_1x_config_data_ptr = rf_cdma_nv_get_1x_data(
                                       logical_dev->rx_nv_path[plim_data->band],
                                       plim_data->band);
  if ( rf_1x_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "1x PLIM: Invalid 1x NV config structure" );
    return FALSE;
  }

  rf_cdma_config_data_ptr = rf_1x_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "1x PLIM: Invalid common CDMA NV config structure" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  status = TRUE;

  rf_1x_plim_update_temp_mdsp( plim_data,
                               device,
                               rf_cdma_config_data_ptr,
                               logical_dev );


  if ( (plim_data->debugging_enabled == TRUE) &&
       (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1X_MODE) )

  {
    RF_MSG_7( RF_LOW, "1x PLIM: Update target power from SAR. "
                      "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                      "actual SAR max pwr lim(dBm10): %d, "
                      "lim_vs_temp_vs_freq %d(dBm10), Err adjusted "
                      "limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->tx_err_from_hdet_db640, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  } /* if ( plim_data->debugging_enabled == TRUE ) */

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
} /* rf_1x_plim_update_sar_state() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override max and min power limits
  
  @details
  Override functionality provided for debug purposes to experiment with max and
  min power limits.

  This function will put the 1x plim module in override mode for a given device
  with max and min power limits provide.

  If power limiting module is already enabled and running, the new limits will 
  be immediately applied to firmware else it will just store the overriden 
  limits in its data and apply it when rf_1x_plim_start() is called. This 
  function will not be able to override plim if it is enabled in HDR mode for
  this device. The corresponding HDR function should be called.

  Once in override mode, this function can be recalled to update the limits 
  again.

  To disable override mode rf_1x_plim_disable_override() must be called.
  
  @param device
  Device whose plim module is to be override. Used to obtain plim_data,
  logical_dev for this device

  @param max_plim_dbm10
  New overriden max power limit in dbm10. Eg 21.5dbm = 215

  @param min_plim_dbm10
  New overriden min power limit in dbm10. Eg -35.6 = -356

  @return
  TRUE if override was successfully done, FALSE otherwise
*/
boolean
rf_1x_plim_enable_override
(
  rfm_device_enum_type device,
  int16 min_plim_dbm10,
  int16 max_plim_dbm10
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  int16 k10;
  boolean status = TRUE;

  /* Get Writable Plim Pointer */
  plim_data = rf_cdma_get_mutable_plim( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (plim_data == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_override_limits(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_override_limits(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( plim_data->plim_supported == TRUE )
  {
    if ( (plim_data->state == RF_CDMA_PLIM_DISABLED) ||
         (plim_data->mode == RFM_1X_MODE) )
    {
      /* Update power limits in plim data structure only if 
       1) PLIM is currently disabled. Assume it will be enabled in 1x mode 
       2) PLIM is enabled and current mode is 1x. This function cannot 
       override power limits if PLIM is enabled and operating in HDR mode */
      plim_data->max_power_limits.max_power_limit_sar_dbm10 = max_plim_dbm10;
      plim_data->min_power_limit_dbm10 = min_plim_dbm10;

      RF_MSG_3( RF_LOW, "1x PLIM: Overriding [min, max] pwrs to: [%d, %d]dbm10"
                        "on device %d",
                plim_data->min_power_limit_dbm10,
                plim_data->max_power_limits.max_power_limit_sar_dbm10,
                device );

      /* If plim is enabled, update and send new limits to MDSP else
      these limits will be sent to MDSP when plim is started by calling
      rf_1x_plim_start() */
      if ( plim_data->state == RF_CDMA_PLIM_ENABLED )
      {
        k10 = 10 * rf_cdma_get_turnaround_constant( plim_data->band );
        rf_1x_plim_update_limits( device,
                                  logical_dev->tx_mdsp_path,
                                  plim_data,
                                  k10,
                                  &plim_data->max_power_limit_err_adj_db640 );
      }

      /* Set override_enabled flag so that updates from other modules are ignored
      and max/min limits are now fixed to ones provided to this function */
      plim_data->override_enabled = TRUE;
    }
    else
    {
      RF_MSG_3( RF_HIGH, "1x PLIM: Cannot override power limits for device %d,"
                         "mode: %d, plim state: %d",
                device,
                plim_data->mode,
                plim_data->state );
      status = FALSE;
    }
  } /* if ( plim_data->plim_supported == TRUE ) */
  else
  {
    RF_MSG_1( RF_HIGH, "1x PLIM: Cannot override power limits for device %d,"
                        " PLIM not supported!",
              device );
      status = FALSE;
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables PLIM override mode
  
  @details
  Resets override_enabled flag in plim data structure corresponding to given
  device
  
  @param device
  Device whose plim override needs to be disabled

  @return
  Flag indicating whether override was disabled successfully or not
*/
boolean
rf_1x_plim_disable_override
(
  rfm_device_enum_type device
)
{
  rf_cdma_plim_data_type* plim_data;
  
  /* Get Writable Device Status */
  plim_data = rf_cdma_get_mutable_plim( device );

  if ( plim_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_disable_override(): Invalid "
                        "device: %d",
              device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_disable_override(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  plim_data->override_enabled = FALSE;

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns current error (from HDET) adjusted max power limit
  
  @details
  Converts the internal max power limit (calculated in db640) to dbm10 and 
  returns to caller.

  Max power limiting module should be enabled and running in 1x mode for this
  API to work.
 
  @param device
  Device whose max plim is queried

  @param *max_plim_err_adj_dbm10
  Output argument - will contain current max power limit applied to firmware in
  dbm10. If API fails for any reason (FALSE return), this variable will not be
  updated.

  @retval TRUE
  Current error adjusted max power limit was successfully retrieved to the 
  address of max_plim_err_adj_dbm10

  @retval FALSE
  A valid error adjusted max power limit could not be provided because of one 
  of the following reasons. max_plim_err_adj_dbm10 is not touched
   - Invalid device (NULL logical_dev, tx_device etc..)
   - PLIM data not initialized for device (rf_cdma_plim_init() should be 
   called for every device before any other APIs)
   - PLIM module in disabled state on this device
   - PLIM module not operating in 1x mode
*/
boolean
rf_1x_plim_get_err_adj_max_limit
(
  rfm_device_enum_type device,
  int16* max_plim_err_adj_dbm10
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status = TRUE;

  /* Get Writable Plim Data */
  plim_data = rf_cdma_get_mutable_plim( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (plim_data == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_get_err_adj_max_limit(): "
                        "Invalid device: %d",
              device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_get_err_adj_max_limit(): Plim data "
                        "not initialized for device: %d",
              device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1X_MODE) )
  {
    *max_plim_err_adj_dbm10 = 
      rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                            rf_cdma_get_turnaround_constant(plim_data->band),
                            plim_data->max_power_limit_err_adj_db640);
  }
  else
  {
    status = FALSE;
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Update local copy of max_pwr_limit to mdsp
  
  @details
  Interface API to temp comp read command handler to update max_power_limit 
  using the latest therm reading

  @param plim_data
  Pointer to plim_data

  @param device
  Device whose plim override needs to be disabled

  @param rf_cdma_config_data_ptr
  Pointer to cdma common NV data

  @param logical_dev
  Pointer to logical device
*/
void
rf_1x_plim_update_temp_common
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
)
{
 
  boolean status = TRUE;

  rf_1x_plim_update_temp_mdsp( plim_data,
                               device,
                               rf_cdma_config_data_ptr,
                               logical_dev );

  
  if ( (plim_data->debugging_enabled == TRUE) &&
        (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
        (plim_data->mode == RFM_1X_MODE) )
  
   {
     RF_MSG_7( RF_LOW, "1x PLIM: Update target power from temperature. "
                       "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                       "actual SAR max pwr lim(dBm10): %d, "
                       "lim_vs_temp_vs_freq %d(dBm10), Err adjusted "
                       "limit(dB640,dBm10): (%d,%d)",
               status, 
               device, 
               plim_data->tx_err_from_hdet_db640, 
               plim_data->max_power_limits.max_power_limit_sar_dbm10,
               plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
               plim_data->max_power_limit_err_adj_db640,
               rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                     rf_cdma_get_turnaround_constant(plim_data->band),
                                     plim_data->max_power_limit_err_adj_db640) );
   } /* if ( plim_data->debugging_enabled == TRUE ) */

  
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates max tx power limit based on the maximum power linearizer entry
  
  @details
  The max power limit provided to FW must not exceed the max power level
  available in the highest PA state LUT at any time. The configure Tx
  AGC functionality will use this API to provide the linearizer limit to
  the power limiting module.
  
  @param device
  Logical device for which lin limit is being updated. This function
  queries PLIM data and other info like logical_dev based on this param
  
  @param tx_lin_limit_dBm10
  TX linearizer limit in dBm10. It is the max linearizer entry for
  the highest PA state LUT.
  
  @return
  TRUE if the update was accepted and applied to firmware. FALSE if power
  limiting loop was unable to do so.
*/
boolean
rf_1x_plim_update_lin_limit
(
  rfm_device_enum_type device,
  int16 tx_lin_limit_dBm10
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  int16 k10;

  /* Get Writable Device Status */
  plim_data = rf_cdma_get_mutable_plim( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (plim_data == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_tx_lin_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_tx_lin_limit(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  /* Valid state - can accept this update */
  plim_data->max_power_linearizer_limit_dbm10 = tx_lin_limit_dBm10;

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1X_MODE) )
  {
    /* Recalculate max power limit taking new linearizer limit into account.
    Send new limits to MDSP */
    rf_1x_plim_update_limits( device,
                              logical_dev->tx_mdsp_path,
                              plim_data,
                              k10,
                              &plim_data->max_power_limit_err_adj_db640 );

    status = TRUE;
  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */
  else
  {
    RF_MSG_3( RF_HIGH, "1x PLIM: Cannot update power limit for device: %d,"
                       "Mode %d, state: %d", 
              device, plim_data->mode, plim_data->state );
    status = FALSE;
  }

  if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_7( RF_LOW, "1x PLIM: update_lin_limit. Status: %d, device: %d, "
                      "TX lin limit(dBm10): %d, actual SAR "
                      "max pwr lim(dBm10): %d, lim_vs_temp_vs_freq %d(dBm10),"
                      " Err adjusted limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->max_power_linearizer_limit_dbm10, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
} /* rf_1x_plim_update_lin_limit */

/*----------------------------------------------------------------------------*/
/*!
  @brief rf_1x_plim_update_external_limit
  Override  tx power limit 
  
  @details
  The API will set the max tx pwr limit and flag in override mode, which will be used to 
  override  max tx power later when updating tx power limit
  
  @param device
  Logical device for which lin limit is being updated. This function
  queries PLIM data and other info like logical_dev based on this param
  
  @param tx_pwr_override
  TRUE will do tx pwr limit override;
  FLASE will not do tx pwr limit override;

  @param tx_pwr_limit_dbm
  The new tx power limit in dbm10;
  
  @return
  TRUE if the update was accepted and applied to firmware. FALSE if power
  limiting loop was unable to do so.
*/
boolean
rf_1x_plim_update_external_limit
(
  rfm_device_enum_type device,
  boolean tx_pwr_override, 
  int16 tx_pwr_limit_dbm
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev;
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */

  logical_dev = rfc_cdma_get_logical_device_params( device );

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( dev_status_w == NULL || logical_dev == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_update_external_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( 
                               logical_dev->tx_nv_path[dev_status_w->curr_band],
                               dev_status_w->curr_band );

  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: rf_1x_plim_update_external_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  if ( tx_pwr_override == TRUE )
  {
    dev_status_w->plim.tx_override_pwr_limit_dbm10 = tx_pwr_limit_dbm;
  }
  else
  {
    dev_status_w->plim.tx_override_pwr_limit_dbm10 = 420;
  }

  rf_1x_plim_update_temp_mdsp( &dev_status_w->plim,device,
                               rf_1x_config_ptr->cdma_common_ptr,logical_dev);

  return TRUE;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates max tx power limit based on frequency and temperature NV
  
  @details
  The max power limiting module will use power limiting information from 
  NV to determine the max power for a given frequency and temperature 
  combination. This max power limit will always override the temp limit for 
  SAR State 0 and feeds into the "min" function for the remaining SAR 
  states.
  
  @param device
  Logical device for which lin limit is being updated. This function
  queries PLIM data and other info like logical_dev based on this param

  @param plim_data
  Pointer to plim_data

  @param band
  RF Band for which limit must be computed

  @param chan
  RF channel for which limit must be computed and stored

  @param last_valid_therm_read
  Last valid temperature reading to obtain the limit vs temperature
  
  @return
  TRUE if the update was accepted and applied to firmware. FALSE if power
  limiting loop was unable to do so.
*/
boolean
rf_1x_plim_update_lim_vs_temp_vs_freq
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint16 last_valid_therm_read
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  boolean status = FALSE ;
  int16 k10;
  const rf_cdma_tx_cal_type* tx_cal;
  const rf_cdma_tx_static_type* tx_static;
  int16 freq_interpolated_temp_array[RF_CDMA_NV_TEMP_LIMIT_SIZE];
  uint8 i;

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_lim_vs_temp_vs_freq(): "
                        "Invalid device: %d", device );
    return FALSE;
  }

  tx_static = rf_cdma_nv_get_tx_static( logical_dev->tx_nv_path[band], band );
  if ( tx_static == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_lim_vs_temp_vs_freq(): "
                        "NULL pointer: %d", device );
    return FALSE;
  }

  tx_cal = rf_cdma_nv_get_tx_cal( logical_dev->tx_nv_path[band], band );
  if ( tx_cal == NULL )
  {
    RF_MSG_1( RF_ERROR, "1x PLIM: update_lim_vs_temp_vs_freq(): "
                        "NULL pointer: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  if ( tx_cal->therm_cal_data.is_therm_cal_nv_data_valid )
  {
    /* Valid state - can accept this update */
    for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
    {
      freq_interpolated_temp_array[i] = 
        rf_lininterp_u32_s16(
          tx_cal->tx_multi_lin_cal.tx_cal_freq,
          tx_static->lim_vs_temp_vs_freq[i],
          tx_cal->tx_multi_lin_cal.tx_cal_chan_size,
          rf_cdma_get_tx_carrier_freq(band, chan) );
    }

    plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10 = 
      rf_lininterp_u16_s16( tx_cal->therm_cal_data.tx_therm_cal,
                            freq_interpolated_temp_array,
                            RF_CDMA_NV_TEMP_LIMIT_SIZE,
                            last_valid_therm_read );

    if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
         (plim_data->mode == RFM_1X_MODE) )
    {
      /* Recalculate max power limit taking new limit into account.
      Send new limits to MDSP */
      rf_1x_plim_update_limits( device,
                                logical_dev->tx_mdsp_path,
                                plim_data,
                                k10,
                                &plim_data->max_power_limit_err_adj_db640 );

      status = TRUE;
    } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */
    else
    {
      RF_MSG_3( RF_HIGH, "1x PLIM: Cannot update power limit for device: %d,"
                         "Mode %d, state: %d", 
                device, plim_data->mode, plim_data->state );
    }
  }
  else
  {
    RF_MSG ( RF_HIGH , "rf_1x_plim_update_lim_vs_temp_freq() :"
             "Therm Cal items marked Invalid.Skipping max power update " ) ;
  }
  if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_7( RF_LOW, "1x PLIM: update_lim_vs_temp_vs_freq. "
                      "Status: %d, device: %d, "
                      "TX lin limit(dBm10): %d, actual SAR max "
                      "pwr lim(dBm10): %d, lim_vs_temp_vs_freq %d(dBm10),"
                      " Err adjusted limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->max_power_linearizer_limit_dbm10, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limits.max_power_limit_vs_temp_vs_freq_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
} /* rf_1x_plim_update_lim_vs_temp_vs_freq */
#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
