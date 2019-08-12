/*! 
  @file
  rf_cdma_power_limiting.c
 
  @brief
  This file contains the Power Limiting module of CDMA driver.

  @details
  The power limiting module is responsible for keeping track of the current
  Transmit power limits accounting for all factors like HDET feedback, 
  temperature, SAR, SVDO configuration etc..

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_power_limiting.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/10/13   zhw     Remove dev_status_w in mc layer call flow
05/30/13   JJ      Fixed rf_cdma_get_tx_min_max_pwr input band type
01/31/13   APU     Fixed offtarget compiler error.                  
01/30/13   APU     Brought over Enhanced PA backoff code from Nikel.
09/17/12   APU     Take neq power in deciding the HDET power mode. 
09/25/12   APU     Fixed a KW error.
09/25/12   APU     Removed debug messages and flags.
09/20/12   APU     In Funnel mode consider the funneled max power for HDET PM
07/24/12   hdz     Changed interface of rf_1x_plim_get_lowest_max_power_limit()
02/08/12   cd      Fixed KW error in get current max power limit API
02/02/12   cd      Added API to get current minimum of max power limits from 
                   1x HDR PLIM modules 
10/13/11   aro     Added interface to enable and disable various debug mode
09/27/11   aro     Disabled PLIM debug messages
09/13/11   shb     Added PLIM state machine image to doxy documentation
08/29/11   shb     Added mode and override_enabled members in plim data struct
08/24/11   shb     Appended db* to plim data names for clarity of units since 
                   some members are dbm10 while others are db640 (signed tx agc)
08/23/11   shb     Added plim_supported flag in plim_data
08/17/11   shb     Added critical section support
08/15/11   shb     Initial version

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_power_limiting.h"
#include "rfcommon_msg.h"
#include "rf_cdma_data.h"
#include "rf_cdma_utils.h"
#include "rf_1x_power_limiting.h"
#include "rf_hdr_power_limiting.h"
#include "rf_hdr_mdsp.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Initializes the Power Limiting module for a given device
    
  @details  
  This API only sets up internal data structures and state for power limiting
  module operation on the selected device. It does NOT do any communication 
  with actual hardware. 

  Power Limiting algorithms rely on a well-defined initial state of its 
  data. Thus this function should be the first to be called on bootup via
  rfm_init() before any power limiting is started.

  If this API is called again in the middle of TX operation, it will reset
  the state variables.

  Note that this function is common to 1x/hdr and hence cannot initialize
  any variables with init values different for 1x and hdr. To achieve this,
  seperate 1x/hdr init functions should be defined in 
  rf_<1x,hdr>_power_limiting.c which should be called during enter_mode
    
  @param device  
  RF device on which the TX power limiting is to be performed

  @param *logical_dev
  Pointer to logical device param of device whose power limiting module is to
  be initialized
    
  @param *plim_data  
  Pointer to the data structure that contains power limiting data for a given 
  device
 
  @return
  TRUE if Power Limiting module initialization was successful on this device, 
  FALSE otherwise
*/
boolean
rf_cdma_plim_init
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type *logical_dev,
  rf_cdma_plim_data_type* plim_data
)
{
  boolean init_status = TRUE;
  if ( (plim_data == NULL) || (logical_dev == NULL) )
  {
    RF_MSG_1( RF_ERROR, "rf_cdma_plim_init: Power Limiting module cannot be "
                        "initialized for device %d - invalid data",device);
    return FALSE;
  }

  if ( rfc_cdma_is_tx_device(device) == TRUE )
  {
    /* As long as a device supports TX, Power limiting is supported on it */
    plim_data->plim_supported = TRUE;

    /* Reset PLIM mode to parked mode */
    plim_data->mode = RFM_PARKED_MODE;

    /* Initialize state variables */
    plim_data->override_enabled = FALSE;
    plim_data->state = RF_CDMA_PLIM_DISABLED;
    plim_data->tx_err_from_hdet_db640 = 0;
    
    /* Work in debug mode for bringup. Set to false when mainlined */
    plim_data->debugging_enabled = FALSE;
  
    /* Initialize Mutex lock for Plim critical section - 1 per device */
    init_status &= rf_common_init_critical_section( &plim_data->crit_section );
  }
  else
  {
    /* TX Device is NULL, so Power Limiting obviously cannot be supported on 
    device */
    RF_MSG_1( RF_MED, "rf_cdma_plim_init: Power Limiting not supported on "
                      "device %d. Not a TX device. ",
              device );
  }

  /* Initialization complete - set init_done flag. This should be the last
  parameter to be initialized */
  plim_data->init_done = TRUE;

  RF_MSG_1( RF_MED, "rf_cdma_plim_init: Power Limiting module initialized for "
                    "device %d", device );

  return init_status;
}

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Configure PLIM debug Mode
 
  @details
  This function will set the PLIM variables to debug mode,
 
  @param ic_data
  Pointer to PLIM data for which debug mode is to be se
 
  @param debug_en
  Flag indicating, if the debug mode is to be enabled or not.
 
  @return
  Status indicating, if the debug mode is enabled or not.
*/
boolean
rf_cdma_plim_configure_debug_mode
(
  rf_cdma_plim_data_type* plim_data,
  boolean debug_en
)
{
  boolean ret_val = FALSE;

  if ( plim_data != NULL )
  {
    plim_data->debugging_enabled = debug_en;
    RF_MSG_1( RF_LOW, "rf_cdma_plim_configure_debug_mode: Debug "
              "Mode %d", debug_en );
    ret_val = TRUE;
  }
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_plim_configure_debug_mode: Failed to "
              "Configure debug mode" );
    ret_val = FALSE;
  }

  return ret_val;

} /* rf_cdma_plim_configure_debug_mode */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Obtain current lowest max power limit from the PLIM module
 
  @details
  Provide a wrapper to the 1x and HDR get max power limit functionality. This 
  function returns the current lowest max power limit as determined by the 
  PLIM module. This excludes the HDET error and linearizer limit inputs 
  to power limiting.
 
  @return
  Return the current lowest max power limit in dBm10
 
*/
int16
rf_cdma_plim_get_curr_max_power_limit
(
  rfm_device_enum_type device,
  rfm_mode_enum_type mode
)
{
  int16 curr_max_power_limit = -600; /* Initialize to some min value */
  const rf_cdma_data_status_type* dev_status_r; /* Device Status Read ptr */
  rf_cdma_plim_data_type* plim_data;

  int32 band_min_pwr, band_max_pwr;

  /* Get Device Status */
  dev_status_r = rf_cdma_get_device_status( device );

  /* Extract plim_data from device data */
  plim_data = rf_cdma_get_mutable_plim( device );

  if ( (dev_status_r == NULL) || (plim_data == NULL) )
  {
    RF_MSG_1( RF_ERROR, "CDMA PLIM: get_lowest_max_power_limit(): "
                        "Invalid device: %d", device );
    return curr_max_power_limit;
  }

  /* Assign the min power for the band as the invalid max power limit */
  rf_cdma_get_tx_min_max_pwr( dev_status_r->curr_band,
                              &band_min_pwr,
                              &band_max_pwr
                            );
  curr_max_power_limit = (int16)band_min_pwr;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "CDMA PLIM: get_lowest_max_power_limit(): "
                        "Plim data not initialized for device: %d", device );
    return curr_max_power_limit;
  }

  /* Based on the requested mode, obtain the current max power limit */
  if ( mode == RFM_1X_MODE )
  {
    curr_max_power_limit = 
      rf_1x_plim_get_lowest_max_power_limit( &plim_data->max_power_limits, 
        device );
  }
  else if ( mode == RFM_1XEVDO_MODE )
  {
    boolean is_funnel_mode_enabled = FALSE ;
    int16 funnel_max_pwr_dBm640 = -600 ;
    const rfc_cdma_logical_device_params_type* logical_device = 
			rfc_cdma_get_logical_device_params ( device ) ;
		if ( NULL != logical_device )
		{
      boolean funnel_api_success = FALSE ;
      rf_cdma_mdsp_chain_enum_type mdsp_path = logical_device->tx_mdsp_path ;
      funnel_api_success = 
			rf_hdr_mdsp_get_funneling_metrics ( mdsp_path , &funnel_max_pwr_dBm640 , 
																				 &is_funnel_mode_enabled ) ;
      curr_max_power_limit = 
        rf_hdr_plim_get_lowest_max_power_limit ( &plim_data->max_power_limits ) ;

      if ( TRUE == funnel_api_success ) 
      {
        if ( TRUE == is_funnel_mode_enabled )
        {
          int16 funnel_max_pwr_dBm10 = 
          rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                         rf_cdma_get_turnaround_constant(plim_data->band) , 
                         funnel_max_pwr_dBm640 ) ;
          RF_MSG_3 ( RF_LOW , 
            "is_funneling_enabled:-> %d | FunnelPwr-> %d | "
             "CurrMaxPwr-> %d | " , is_funnel_mode_enabled , 
              funnel_max_pwr_dBm10 , curr_max_power_limit ) ;

          curr_max_power_limit = MAX ( curr_max_power_limit , funnel_max_pwr_dBm10 ) ;
          RF_MSG_1 ( RF_LOW , "WinnerMax for ascertaining  HDET PM-> %d " , 
             curr_max_power_limit ) ;
        }
      }
      else 
      {
        RF_MSG ( RF_ERROR , "rf_cdma_get_tx_min_max_pwr(): "
                            "rf_hdr_mdsp_get_funneling_metrics() returned failure " ) ;
      }
    }
    else
    {
      RF_MSG ( RF_ERROR , "rf_cdma_get_tx_min_max_pwr(): "
                          "Logical Device is NULL " ) ;
    }
  }
  else
  {
    RF_MSG_1( RF_ERROR, "CDMA PLIM: get_lowest_max_power_limit():"
                        " Invalid mode requested %d", mode );
  }

  return curr_max_power_limit;
} /* rf_cdma_plim_get_curr_max_power_limit */
#endif /* FEATURE_CDMA1X */
/*! @} */
