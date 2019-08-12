/*!
  @file
  rfm_cdma.c

  @brief
  This file defines the Common CDMA RFM interfaces to be used by external
  module.

  @addtogroup RF_CDMA_COMMON_RFM
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rfm_cdma.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/16/16   rs      F3 reduction to avoid excessive F3 logging
11/06/14   spa     Change rx_nv_path to be per band per logical device
04/21/14   JJ      Moved rfm_cdma_convert_sys_band_to_sys_sband to rf_cdma_util.c
03/09/14   pk      Added interface to query DLNA support
10/14/13   zhw     Fix compiler warning
10/08/13   zhw     Expose turn-around constant to RFM layer
05/30/13   JJ      Remove RF_CDMA_INVALID_BAND, Convert rf_cdma_band type to
                   sys_band type
04/01/13   Saul    Corrected debug message.
08/22/12   aro     Renamed interface names and type definitions (time prof)
08/22/12   aro     Changes to migrate to new precide time profiler
05/29/12   APU     rfm_cdma_is_tunable_on_device() return is now also based on
                   rf_cdma_nv_device_data_is_valid()
05/18/12   aro     Fixed the BC Config checking for CDMA RFM Devices
05/14/12   zw      rfm_cdma_is_band_chan_supported() now checks both NV and HW
                   by calling rfm_cdma_is_tunable_on_device()
05/14/12   zw      Fixed bug in rfm_cdma_is_tunable_on_device() and
                   rfm_cdma_get_band_chan()
05/14/12   APU     rfm_cdma_is_tunable_on_device() return is now based on 2
                   factors now:
                   1> RFC [Check already existed]
                   2> NV [Introduced in this version]
05/13/12   jr      Reverted externed rfm_get_calibration_state() and
                   included rfm_internal.h
05/11/12   spa     rf_cdma_mc_is_band_chan_supported() used to query for valid
                   band/chan by rfm_cdma_is_band_chan_supported()
05/02/12   sa      Removed Compiler Warning. Externed rfm_get_calibration_state()
04/27/12   zhaow   Fixed rfm_cdma_is_band_chan_supported not checking whether
                   the device is in calibration mode or not.
03/26/12   hdz     Moved most implementation of rfm_cdma_update_agc()  to
                   rf_cdma_update_agc() to support calling from IST
03/15/12   spa     Added max device check for rf_cdma_get_device_status()
02/08/12   hdz     Looped all devices for rfm_cdma_update_agc()
02/07/12   hdz     Fixed compiling warnings
02/06/12   hdz     Added rfm_cdma_update_agc() for re-loading tx/rx agc config
                   in a call
12/07/11   sty     Reverted previous change
12/02/11   sty     Return channel that LO is tuned to in rfm_cdma_get_band_chan
09/12/11   sty     Added rfm_cdma_is_tunable_on_device()
08/29/11   sty     Temp implementation for rfm_cdma_get_sub_class()
06/16/11   sar     Included internal common rf cdma interface.
06/07/11   Saul    Updates for multi-carrier driver.
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
03/25/11   sty     Added initial implementation for
                   rfm_cdma_is_band_chan_supported()
01/15/11   aro     Updated rf_cdma_data_get_device_status() interface
01/15/11   aro     [1] Added Get Current Band Chan Function
                   [2] Doxygen Update
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/20/10   aro     Removed including of rfcom.h
12/13/10   aro     Filled-in RFM functionality
12/10/10   aro     Initial Revision.

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rfcommon_msg.h"
#include "rf_cdma_data.h"
#include "rf_cdma_nv.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_time_profile.h"
#include "rf_cdma_rx_agc.h"
#include "rf_cdma_tx_agc.h"
#include "rf_cdma_mc.h"
#include "rfm_cdma_band_types.h"
#include "rfm_internal.h"
#include "rfcommon_nv_mm.h"
#include "rf_cdma_utils.h"

/*============================================================================*/
/*!
  @name CDMA Configuration Query Functions

  @brief
  Functions to report the static properties of the current RF
  configuration.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Current Band and Channel

  @details
  This function queries the CDMA Data structure for the current band and
  channel on a given device and returns the value.

  @param device The radio device for which band and channel is resquested

  @return
  Current Band and Channel
*/
sys_channel_type
rfm_cdma_get_band_chan
(
  rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  sys_channel_type curr_band_chan; /* Variable to store current band / chan */

  curr_band_chan.band = SYS_BAND_BC_MAX;
  curr_band_chan.chan_num = RF_CDMA_INVALID_CHAN;

  /* Get Device Status from CDMA Data Structure */
  dev_status_r = rf_cdma_get_device_status( device );

  /*Perform NULL pointer check*/
  if( dev_status_r  == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_get_band_chan: NULL Device %d data",
              device );
  }
  else
  {
  /* Update return variable with band and channel */
  curr_band_chan.band = rf_cdma_rf_band_type_to_sys_band_type(dev_status_r->curr_band);

  /* In multi-carrier cases, the curr_chans[] holds the channels to
     which L1 needs RF ro tune to. However, the Rx LO is actually tuned to
     lo_tuned_to_chan*/
  curr_band_chan.chan_num = dev_status_r->lo_tuned_to_chan;
  }

  /* Return */
  return curr_band_chan;

} /* rfm_cdma_get_band_chan */

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Return Subclass Bitmask

  @details
  This function takes band parameter and and returns an associated subclass
  bitmask.

  @param device
  The radio path for which subclass is required.

  @param band
  Band whose associated subclass bitmask is erquested

  @return
  Subclass bitmask for given band and device.
*/
uint32
rfm_cdma_get_sub_class
(
  rfm_device_enum_type device,
  sys_band_class_e_type band
)
{

  uint32 result; /* final result of API */

  /* convert sys_band to rfm_cdma_band */
  rfm_cdma_band_class_type rf_band = rf_cdma_sys_band_type_to_rf_band_type(band);

  if ( rf_band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1( RF_ERROR,
              "rfm_cdma_get_sub_class: Invalid band %d", band );
    result = 0;
  }
  else
  {
    result = rfc_cdma_get_sub_class_for_band(device, rf_band);
  }

  return result;

} /* rfm_cdma_get_sub_class */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Rx Carrier Frequency

  @details
  This function returns the Rx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported
  then 0 is returned.

  @param band_chan
  Band and Channel for which carrier frequency is requested

  @return
  Rx Carrier Frequency in KHz
*/
uint32
rfm_cdma_get_rx_carrier_freq
(
  sys_channel_type band_chan
)
{
  rfm_cdma_band_class_type rf_band =
    rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);

  if ( rf_band != RFM_CDMA_MAX_BAND )
  {
    return rf_cdma_get_rx_carrier_freq( rf_band, band_chan.chan_num );
  }
  else
  {
    return 0;
  }
} /* rfm_cdma_get_rx_carrier_freq */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Tx Carrier Frequency

  @details
  This function returns the Tx carrier frequency in KHz for the band and channel
  specified in the parameters.  If the band or channel is not supported
  then 0 is returned.

  @param band_chan
  Band and Channel for which carrier frequency is requested

  @return
  Tx Carrier Frequency in KHz
*/
uint32
rfm_cdma_get_tx_carrier_freq
(
  sys_channel_type band_chan
)
{
  rfm_cdma_band_class_type rf_band =
    rf_cdma_sys_band_type_to_rf_band_type(band_chan.band);

  if ( rf_band != RFM_CDMA_MAX_BAND )
  {
    return rf_cdma_get_tx_carrier_freq( rf_band, band_chan.chan_num );
  }
  else
  {
    return 0;
  }
} /* rfm_cdma_get_tx_carrier_freq */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns turn-around constant (TC) in dBm for given band

  @param band for which TC is queried

  @return TC as defined by the standard. Unit is in dBm.
          Returns 0 for invalid/unsupported bands


*/
int
rfm_cdma_get_turnaround_constant
(
  const sys_band_class_e_type band
)
{
  uint32 result; /* final result of API */

  /* convert sys_band to rfm_cdma_band */
  rfm_cdma_band_class_type rf_band = rf_cdma_sys_band_type_to_rf_band_type(band);

  if ( rf_band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1( RF_ERROR,
              "rfm_cdma_get_turnaround_constant: Invalid band %d", band );
    result = 0;
  }
  else
  {
    result = rf_cdma_get_turnaround_constant(rf_band);
  }

  return result;
}/* rfm_cdma_get_turnaround_constant */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Returns whether the specified device is tunable or not for a given band

  @param device :  trasceiver device

  @param band : band to be checked for tuning.

  @retval TRUE if the device can be tuned to the given band depending on if
  the hardware and the NV are supported, FALSE otherwise

  @pre
  rfm_init() must have already been called, to ensure that RF card has been
  configured
*/
boolean
rfm_cdma_is_tunable_on_device
(
  const rfm_device_enum_type device,
  const sys_band_class_e_type band
)
{
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  boolean result = FALSE ; /* final result of API */
  /* convert sys_band to rfm_cdma_band */
  rfm_cdma_band_class_type rf_band =
                                   rf_cdma_sys_band_type_to_rf_band_type( band);

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params ( device ) ;

  /* Perform NULL Pointer check */
  if ( logical_dev == NULL )
  {
    RF_MSG_DEBUG_1 ( RF_ERROR, "rfm_cdma_is_tunable_on_device: NULL logical data for "
                        "device %d", device );
  }
  else if ( rf_band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_2 ( RF_ERROR,
      "rfm_cdma_is_tunable_on_device: ON device %d: Invalid band %d", device , band ) ;
  }
  else
  {
    /* Check if the band is supported on given device*/
    result = rf_cdma_is_band_supported( device,rf_band );

    if ( TRUE == result )
    {
      result = rfc_cdma_is_tunable_on_device(device, rf_band);
      if ( TRUE == result   )
      {
        if ( FALSE == rfm_get_calibration_state() )
        {
          result =
            rf_cdma_nv_device_data_is_valid( logical_dev->rx_nv_path[band],
                                             band) ;
          if ( FALSE == result )
          {
            /* If NV isn't valid for this band, then it isn't supported. */
            RF_MSG_1 ( RF_MED, "rfm_cdma_is_tunable_on_device: "
              "On device: %d rf_cdma_nv_device_data_is_valid() returned false",
              device ) ;
          }
        }
        else
        {
          RF_MSG_1 ( RF_MED, "rfm_cdma_is_tunable_on_device: On device:%d "
                             "rfm_get_calibration_state() returned TRUE "
                             "Calibration mode, skipping NV check", device ) ;
        }
      }
      else
      {
        RF_MSG_DEBUG_1 ( RF_MED, "rfm_cdma_is_tunable_on_device: Failed on device:%d "
                   , device ) ;
      }
    }
    else
    {
      RF_MSG_DEBUG_2 ( RF_HIGH, "rfm_cdma_is_tunable_on_device:On device:%d, Band %d "
                          "is not supported" , device , rf_band) ;
    }
  }
    return result ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Checks if band and channel are valid and supported

  @details
  This function checks if the (band, chan) is valid and if NV has been loaded
  for the band. If either of these two conditions are not satisfied, the API
  returns a FALSE

  @param device
  Device for which Band and Chan is to be validated.

  @param band_chan
  Band and Channel which is to be validated

  @return
  TRUE if band and channel are supported and FALSE otherwise
*/
boolean
rfm_cdma_is_band_chan_supported
(
  const rfm_device_enum_type device,
  const sys_channel_type band_chan
)
{
  /* return value */
  boolean ret_val;

  /* CDMA specific band and channel */
  rfm_cdma_band_class_type band;
  rfm_cdma_chan_type chan;

  /* Convert sys_channel_type to CDMA specific */
  band = rf_cdma_sys_band_type_to_rf_band_type( band_chan.band );
  chan = ( rfm_cdma_chan_type )band_chan.chan_num;

  /* Check for valid band*/
  if ( band >= RFM_CDMA_MAX_BAND )
  {
    RF_MSG_1( RF_ERROR,
              "rfm_cdma_is_band_chan_supported: Invalid band %d", band );
    ret_val = FALSE;
  }
  else
  {
    /* Call mc function to check if band and channel are supported
       Also performs NV check*/
    ret_val = rf_cdma_mc_is_band_chan_supported( device, band, chan );
  }

  /* Final Check*/
  if ( ret_val == FALSE )
  {
    RF_MSG_2( RF_ERROR, "rfm_cdma_is_band_chan_supported: "
                        "Check failed. Device: %d Band: %d ",
                         device, band );
  }
  return ret_val;
} /* rfm_cdma_is_band_chan_supported */

/*! @} */

/*============================================================================*/
/*!
  @name CDMA RxAGC

  @brief
  Functions to configure CDMA RxAGC
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*
  @brief
  Update tx/rx agc in a call

  @detail
  This API will update
    - all tx agc related parameters for all devices in TXRX
      mode.
    - all rx agc related parameters for all devices either in RX mode or in
      TXRX mode.

  @param void

  @retval TRUE if all devices are eligible for updating and updating are all
   successful; FALSE otherwise.
*/
boolean
rfm_cdma_update_agc
(
  void
)
{
  boolean result = TRUE; /* final result of API */
  rf_time_tick_type prof_agc_reload_cfg_t; /* AGC Reloading Config Time Profile */
  rf_time_type agc_reload_time = 0; /* AGC Reloading Config Time */

  prof_agc_reload_cfg_t = rf_time_get_tick();

  result &= rf_cdma_update_agc();

  agc_reload_time = rf_time_get_elapsed( prof_agc_reload_cfg_t,
                                                RF_USEC );

  /* report time taken */
  RF_MSG_1( RF_MED, "rfm_cdma_update_agc: time profile for reloading agc %d ",
            agc_reload_time );

  return result;
}

/*----------------------------------------------------------------------------*/
/*
  @brief
  API to check that DLNA is supported on the particular rx path

  @detail
   API to check that DLNA is supported on the particular rx path

  @param device on which DLNA support should be checked

  @retval TRUE if the rx path supports DLNA; FALSE otherwise.
*/

boolean
rfm_cdma_is_dlna_supported
(
  const rfm_device_enum_type device
)
{
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  boolean is_dlna_supported = FALSE;

  /* Get Device Status from CDMA Data Structure */
  dev_status_r = rf_cdma_get_device_status( device );

  /*Perform NULL pointer check*/
  if( dev_status_r == NULL )
  {
    RF_MSG_1( RF_ERROR, "rfm_cdma_get_band_chan: NULL Device %d data",
    device );
  }
  else
  {
    is_dlna_supported = dev_status_r->is_dlna_supported;
  }

  return is_dlna_supported;
}

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
