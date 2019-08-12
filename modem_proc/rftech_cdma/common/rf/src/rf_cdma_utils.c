/*! 
  @file
  rf_cdma_utils.c
 
  @brief
  This file defines the common 1xHDR utilities to be used by CDMA Core.
 
  @addtogroup RF_CDMA_COMMON_UTILS
  @{
*/

/*==============================================================================

  Copyright (c) 2010 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/rf/src/rf_cdma_utils.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/31/16   rs      Removed excessive F3 msgs
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/06/14   spa     Change rx_nv_path to be per band per logical device
07/17/14   spa     Added API to convert RFM band to RFC band
05/02/14   cd      8-PA state functionality updates
04/23/14   JJ      Fixed a KW error
04/21/14   JJ      Added api to check is band is supported on given device
04/07/13   sty     KW fixes
01/31/14   pk      Compiler warning fix
12/06/13   zhw     Support for using FTM command FBRX gain state
05/30/13   JJ      Change func input param from sys_band type to rfm_cdma type
05/23/13   JJ      Change rf_cdma_get_turnaround_constant input param type
02/04/13   cd      Prevent computation of offset scale factor when offset is 
                   zero
01/10/12   zhw     Move away from using common PA device state
12/27/12   shb     Updates for different physical rx_device per band
12/19/12   cd      Remove unrequired API to allocate TxLM for ET
12/19/12   Saul    CDMA. TX AGC XPT Dev. Use xpt cfg msg in online.
10/07/12   zhw     API for converting Cx1 to USTMR units.
10/02/12   zhw     Comment correction
09/21/12   zhw     Added API to convert PA state into PA gain range
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/09/12   cd      Added function to convert IQ gain offset from dB10 to HW 
                   units
03/08/12   spa     Updated execute_wakeup_rx to use CDMA specific band and chan 
01/19/12   aro     Updated all HAL interfaces to use RxLM/TxLm handle rather
                   than logical device
01/10/12   Saul    MCDO. RTR now taking EVDO mode. 
                   Allow operation in multi-carrier mode and bandwidths.
12/20/11   aro     MC function to perform mapping between PA range and state
12/10/11   sty     Added base_freq_offset_hz as arg to 
                   rf_cdma_compute_carrier_info()
12/01/11   sty     [1] Added rf_cdma_band_to_rfm_card_band()
                   [2] moved freq-related APIs to rf_cdma_freq.c
10/11/11   aro     Added interface to check high band and low band
10/11/11   aro     Added interface to convert PA Switchpoint from NV units to
                   dBm10
08/25/11   shb     Corrected documentation
08/22/11   cd      Fixed Klocwork warnings
07/26/11   sty     Fixed floating-point error in rf_cdma_convert_txagc()
07/26/11   sty     Fixed floating-point error in rf_cdma_convert_rxagc()
07/25/11   sty     Minor changes in rf_cdma_convert_rxagc()
07/05/11   Saul    Added bandwidth computation.
06/27/11   sty     Fixed loss-of-resolution issue
06/21/11   Saul    Updates for multi-carrier driver.
06/14/11   sty     Documentation update
06/14/11   sty     Added APIs to convert AGC to requested format
06/14/11   Saul    Now configuring CSR freq offsets.
06/13/11   Saul    Fixed inconsistent indent.
06/10/11   Saul    Updated CDMA BW defines.
06/10/11   Saul    Now configuring carriers based on mask.
06/10/11   cd      Removed unused rf_cdma_utils_interpolate_chan_offset()
06/09/11   Saul    RxLM updates for multi-carrier driver.
06/07/11   shb     Updated RF CDMA device interface to accept RX/TX device
                   pointers (queried from RFC) instead of the logical path
06/02/11   aro     Update for Cal Compensation units
05/10/11   cd      Removed rf_cdma_utils_get_freq_bin()
04/21/11   aro     Updated CDMA Device Data to have read and mutable interfaces
04/21/11   aro     [1] Moved Get Calibration compensation function to Common
                   [2] Doxygen Update
03/17/11   bmg     Moved rf_cdma_get_freq_from_channel to rf_cdma_utils_freq.c
03/09/11   sty     Reverted previous change
03/09/11   sty     Changed arg type in rf_cdma_get_tx_min_max_pwr
03/08/11   sty     Changed rf_cdma_get_tx_min_max_pwr() to use correct band enum
03/04/11   sty     Added rf_cdma_get_turnaround_constant()
                   Used new macro for messages
03/04/11   sty     Doxygen warning fix
03/03/11   sty     Fixed warnings
03/03/11   sty     Added rf_cdma_get_tx_min_max_pwr()
02/01/11   aro     Added Channel Offset interpolation function
01/08/11   aro     Changed @ingroup to @addtogroup
01/05/11   aro     Moved @defgroup to rf_cdma_documentation.h
12/29/10   aro     Added function to return Frequency Bin for given band/chan
12/16/10   aro     Doxygen Update
12/13/10   aro     Added CDMA utility function
12/10/10   aro     Initial Revision.

==============================================================================*/

#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_cdma_utils.h"
#include "rf_cdma_msm.h"
#include "rf_cdma_data.h"
#include "rfdevice_cdma_interface.h"
#include "rfcommon_math.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils_freq.h"
#include "rfcommon_core_error.h"
#include "rfc_cdma.h"
#include "rf_cdma_nv.h"
#include "math.h"
#include "rfcommon_nv_mm.h"

/*----------------------------------------------------------------------------*/
/*! Channel Span for BC0 */
#define RF_CDMA_UTILS_BC0_CHAN_SPAN ((uint16)832)

/*----------------------------------------------------------------------------*/
/*! Channel Span for BC4 */
#define RF_CDMA_UTILS_BC4_CHAN_SPAN ((uint16)600)

/*----------------------------------------------------------------------------*/
/*! Channel Span for BC1 */
#define RF_CDMA_UTILS_PCS_CHAN_SPAN ((uint16)1201)

/*============================================================================*/
/*!
  @name Band and Frequency Conversion

  @brief
  Functions to perform Conversions related to Band and Frequency
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert rf_card_band_type to rfm_cdma_band_class_type
 
  @param rf_card_band
  Band Class to be mapped
 
  @return
  Band Class of type rfm_cdma_band_class_type
*/
rfm_cdma_band_class_type
rf_cdma_band_to_rfm_card_band
(
  rf_card_band_type rf_card_band
)
{
  rfm_cdma_band_class_type rfm_band_class;

  if ( rf_card_band <= RF_BC19_BAND)
  {
    rfm_band_class = (rfm_cdma_band_class_type)rf_card_band;
  }
  else
  {
    /* flag error */
    rfm_band_class = RFM_CDMA_MAX_BAND;
  }
  
  return rfm_band_class;

} /* rf_cdma_band_to_card_band */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert 1x Band to RF Card Band
 
  @details
  This function will map the given 1x band to generic RF Card Band.
 
  @param rf1x_band
  1x Band to be mapped
 
  @return
  Mapped RF Card Card Band
*/
rf_card_band_type
rf_cdma_band_to_card_band
(
  rfcom_1x_band_type rf1x_band
) 
{
  rf_card_band_type rfc_band = RF_MAX_BAND;

  switch( rf1x_band )
  {
  case RFCOM_BAND_1X_BC0:
    rfc_band = RF_BC0_BAND;
    break;
  case RFCOM_BAND_1X_BC1:
    rfc_band = RF_BC1_BAND;
    break;
  case RFCOM_BAND_1X_BC2:
    rfc_band = RF_BC2_BAND;
    break;
  case RFCOM_BAND_1X_BC3:
    rfc_band = RF_BC3_BAND;
    break;
  case RFCOM_BAND_1X_BC4:
    rfc_band = RF_BC4_BAND;
    break;
  case RFCOM_BAND_1X_BC5:
    rfc_band = RF_BC5_BAND;
    break;
  case RFCOM_BAND_1X_BC6:
    rfc_band = RF_BC6_BAND;
    break;
  case RFCOM_BAND_1X_BC7:
    rfc_band = RF_BC7_BAND;
    break;
  case RFCOM_BAND_1X_BC8:
    rfc_band = RF_BC8_BAND;
    break;
  case RFCOM_BAND_1X_BC9:
    rfc_band = RF_BC9_BAND;
    break;
  case RFCOM_BAND_1X_BC10:
    rfc_band = RF_BC10_BAND;
    break;
  case RFCOM_BAND_1X_BC11:
    rfc_band = RF_BC11_BAND;
    break;
  case RFCOM_BAND_1X_BC12:
    rfc_band = RF_BC12_BAND;
    break;
  case RFCOM_BAND_1X_BC13:
    rfc_band = RF_BC13_BAND;
    break;
  case RFCOM_BAND_1X_BC14:
    rfc_band = RF_BC14_BAND;
    break;
  case RFCOM_BAND_1X_BC15:
    rfc_band = RF_BC15_BAND;
    break;
  case RFCOM_BAND_1X_BC16:
    rfc_band = RF_BC16_BAND;
    break;
  case RFCOM_BAND_1X_BC18:
    rfc_band = RF_BC18_BAND;
    break;
  case RFCOM_BAND_1X_BC19:
    rfc_band = RF_BC18_BAND;
    break;
  default:
    break;
  }
  return( rfc_band );
} /* rf_cdma_band_to_card_band */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes span based on channel list provided

  @param num_band_chan
  The length of the band_chan parameter array.

  @param band
  An band to serve as a reference for computing the span.
 
  @param chan_array
  An array of channels  to serve as a reference for computing the span.
 
  @param min_freq_khz_ptr - Output. The lowest frequency of all the channels.
 
  @param max_freq_khz_ptr - Output. The highest frequency of all the channels.

  @return
  TRUE - if span computation is valid, FALSE otherwise
*/
boolean
rf_cdma_get_chan_span_khz
(
   const uint32 num_band_chan,
   const rfm_cdma_band_class_type band,
   const rfm_cdma_chan_type * const chan_array,
   uint32                 * const min_freq_khz_ptr,
   uint32                 * const max_freq_khz_ptr
)
{
   boolean status = FALSE;
   uint32  min_freq_khz = 0;
   uint32  max_freq_khz = 0;
   uint32  band_chan_index = 0;
   uint32  curr_freq_khz = 0;

   if ( (num_band_chan > 0)
        && (NULL != chan_array)
        && (NULL != min_freq_khz_ptr)
        && (NULL != max_freq_khz_ptr) )
   {
      // valid input channels present
      for ( band_chan_index = 0;
            band_chan_index < num_band_chan;
            band_chan_index++ )
      {
         curr_freq_khz = rf_cdma_get_rx_carrier_freq(
                            band,
                            chan_array[band_chan_index]);

         RF_ASSERT(curr_freq_khz > 0);

         if (curr_freq_khz > 0)
         {
            if (0 == min_freq_khz)  // means max_freq_khz is also 0
            {
               // this is the 1st valid band/chan that we have encountered
               min_freq_khz = curr_freq_khz;
               max_freq_khz = curr_freq_khz;
            }
            else
            {
               if (curr_freq_khz < min_freq_khz)
               {
                  min_freq_khz = curr_freq_khz;
               }
               else if (curr_freq_khz > max_freq_khz)
               {
                  max_freq_khz = curr_freq_khz;
               }
            } //else if (0 == min_freq_khz)
         } //if (curr_freq_khz > 0)
         else
         {
            // invalid band/chan entry passed in 'band_chan_array'
            break;
         } //else if (curr_freq_khz > 0)
      } //for ( band_chan_index = 0; ...

      *min_freq_khz_ptr = min_freq_khz;
      *max_freq_khz_ptr = max_freq_khz;
      status = (min_freq_khz > 0); // no need to AND it with (max_freq_khz > 0)
   } //if ( (num_band_chan > 0) && (NULL != band_chan_array) && ...

   return status;
} /* rf_cdma_get_chan_span_khz() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes the bandwidth to use based on channel list provided

  @param num_band_chan
  The length of the band_chan parameter array. It take higher precedence over
  band_chan_array. If num_band_chan > 1, single-carrier bandwidth is
  immediately returned.

  @param band
  The band to compute bandwidth for
 
  @param chan_array
  An array of channels for to serve as a reference
  for computing the bandwidth.

  @return
  The computed bandwidth.
*/
uint32 
rf_cdma_compute_bandwidth
( 
  const uint32 num_band_chan,
  const rfm_cdma_band_class_type band,
  const rfm_cdma_chan_type * const chan_array
)
{
  uint32 bw_khz = RF_CDMA_BW_INVALID;
  uint32 required_bw_khz = 0;/* Var for required bandwidth */
  uint32 min_freq_khz = 0;
  uint32 max_freq_khz = 0;
  uint32 delta_max_min_freq_khz = 0;
  boolean span_ok = FALSE;

  /* If only one channel, use 1x BW, no need to do calculations */
  if( num_band_chan == 1)
  {
    required_bw_khz = RF_CDMA_BW_1X;/* default bandwidth */
  }
  else /* If multi-channels, calculate bandwidth */
  {
    span_ok = rf_cdma_get_chan_span_khz( 
                                          num_band_chan, band, chan_array, 
                                          &min_freq_khz, &max_freq_khz
                                        );

    /* If span calculation is OK */
    if(span_ok == TRUE)
    {
      /* Compute corresponding bandwidth */
      delta_max_min_freq_khz = max_freq_khz - min_freq_khz;
      /* +1250 kHz to account for lo/hi spectrums that fall outside
         of the two carrier freqs of both channels */
      required_bw_khz = delta_max_min_freq_khz + 1250;
    }
    else/* If span calculation is not OK */
    {
      /* Invalidate bandwidth */
      required_bw_khz = RF_CDMA_BW_INVALID;
      RF_MSG( 
              RF_ERROR, 
              "rf_cdma_compute_bandwidth: Error in computing bandwidth. "\
              "RF settings may be invalid." );
    }
  }/* If multi-channels, calculate bandwidth */

  /* Assign the corresponding bandwidth setting */
  if( required_bw_khz <= 0 )
  {
    bw_khz = RF_CDMA_BW_INVALID;
  }
  else if( required_bw_khz <= 1480 )
  {
    bw_khz = RF_CDMA_BW_1X;
  }
  else if( required_bw_khz <= RF_CDMA_BW_3X )
  {
    bw_khz = RF_CDMA_BW_3X;
  }
  else if( required_bw_khz <= RF_CDMA_BW_5X )
  {
    bw_khz = RF_CDMA_BW_5X;
  }
  else if( required_bw_khz > RF_CDMA_BW_5X )
  {
    bw_khz = RF_CDMA_BW_7X;
  }
  
  return bw_khz;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Computes carrier info to use based on carrier items provided
 
  @param band
  CDMA Band to which the device is tuned to.
 
  @param chans
  An array containing the channel information.
 
  @param num_carriers
  The number of carriers to activate.

  @param carrier_index
  An array containing the carrier information.
 
  @param carrier_mask - Output
  The mask to configure based on carrier info.
 
  @param csr_freq_offsets_hz - Output
  The corresponding frequency offsets in Hz.
 
  @param base_freq_offset_hz
  This arg can be used in cases where an extra freq_offset is to be added to
  the offsets that are calculated in the API.
  Use case is SSMA, where, for 1x , this arg could be non-zero, based on
  SSMA recos
*/
void
rf_cdma_compute_carrier_info
( 
  rfm_cdma_band_class_type band,
  const uint16 * const chans,
  uint32 num_carriers, 
  const uint16 * const carrier_index,
  rfm_cdma_carrier_en_mask_type* carrier_mask,
  int32* csr_freq_offsets_hz,
  int32 base_freq_offset_hz 
)
{
  uint8 carrier_idx; /* Variable for carrier indexing */
  uint16 carrier_loc; /* Variable for carrier placement */
  uint32 num_carriers_in_mask;/* Variable for counting num of carriers in mask */
  uint32 freq_lo_chan_hz; /* Frequency for tunning channel */
  uint32 freq_chan_hz;/* Frequency for "current" channel */

  for(carrier_idx = 0; carrier_idx < RFM_CDMA_CARRIER_NUM; ++carrier_idx)
  {
    /* Turn all carriers off by default */
    carrier_mask->enable[carrier_idx] = FALSE;

    /* Zero all offsets */
    csr_freq_offsets_hz[carrier_idx] = 0;
  }

  if ( num_carriers > RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG_1( RF_ERROR, 
              "rf_cdma_compute_carrier_info: Invalid num of band_chan %d", 
              num_carriers );
    /* outputs - carrier_mask and csr_freq_offsets_hz have already been 
       set to their default values - so can return now */
    return;
  }

  /* Calculate tune channel frequency */
  freq_lo_chan_hz = 1000 * rf_cdma_get_rx_carrier_freq ( band,
            (int) rf_cdma_compute_center_tune_chan(num_carriers, band, chans) );

  /* For each carrier...*/
  for(carrier_idx = 0; carrier_idx < num_carriers; ++carrier_idx)
  {
    carrier_loc = carrier_index[carrier_idx];
    /* Ensure carrier number is actually valid/within bounds */
    if( carrier_loc < RFM_CDMA_CARRIER_NUM )
    {
      /* Carrier mask */
      carrier_mask->enable[carrier_loc] = TRUE;

      /* Carrier frequency offset */
      freq_chan_hz = base_freq_offset_hz + 
               (1000 * rf_cdma_get_rx_carrier_freq( band, chans[carrier_idx] ));
      
      /* Calculate freq offset relative to lo chan and update CSR variable */
      csr_freq_offsets_hz[ carrier_loc ] = freq_chan_hz - freq_lo_chan_hz;
    }
    else
    {
      RF_MSG_2( 
                RF_ERROR, 
                "rf_cdma_compute_carrier_info: Carrier index = %d" \
                " has invalid carrier value of %d",
                carrier_idx, carrier_loc );
    }
  }/* For each carrier...*/

  /* Ensure the requested number of carriers matches the mask */
  num_carriers_in_mask = 0;
  for(carrier_idx = 0; carrier_idx < RFM_CDMA_CARRIER_NUM; ++carrier_idx)
  {
    if(carrier_mask->enable[carrier_idx] == TRUE)
    {
      num_carriers_in_mask++;
    }
  }

  if( num_carriers != num_carriers_in_mask )
  {
    RF_MSG_2( RF_ERROR, 
              "rf_cdma_compute_carrier_mask: Requested " \
              "num of carriers = %d does not match carriers to be " \
              "configured = %d",
              num_carriers, num_carriers_in_mask );
  }

} /* rf_cdma_compute_carrier_info */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if the requested band is high band or low band.
 
  @details
  This function will check if the quested band is high band or low band. High
  band refers to band in 2 GHz range and low band refers to band in 1 GHz range.
 
  @param band
  RF band which is to be checked for high band and low band.
 
  @retval TRUE
  This return value indicates that the band under consideration is high band.
 
  @retval FALSE
  This return value indicates that the band under consideration is low band.
*/
boolean
rf_cdma_is_high_band
(
  rfm_cdma_band_class_type band
)
{
  boolean is_high_band = FALSE;

  switch( band )
  {
  case RFM_CDMA_BC0:
  case RFM_CDMA_BC2:
  case RFM_CDMA_BC3:
  case RFM_CDMA_BC5:
  case RFM_CDMA_BC7:
  case RFM_CDMA_BC9:
  case RFM_CDMA_BC10:
  case RFM_CDMA_BC11:
  case RFM_CDMA_BC12:
    is_high_band = FALSE;
    break;

  case RFM_CDMA_BC1:
  case RFM_CDMA_BC4:
  case RFM_CDMA_BC6:
  case RFM_CDMA_BC8:
  case RFM_CDMA_BC13:
  case RFM_CDMA_BC14:
  case RFM_CDMA_BC15:
  case RFM_CDMA_BC16:
    is_high_band = TRUE;
    break;

  default:
    is_high_band = FALSE;
    break;
 }

  return is_high_band;

} /* rf_cdma_is_high_band */

/*! @} */

/*============================================================================*/
/*!
  @name RF Power

  @brief
  Utility Functions related to RF Power
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns Max Tx power (1/10 dBm)
 
  @details
  This function returns minimum and maximum TX power (in 1/10 dBm units)
  for a given band.

  @param band Band max Tx power needs to be queried
 
  @param p_tx_min_pwr holds the min Tx power
 
  @param p_tx_max_pwr holds the max Tx power

*/
void 
rf_cdma_get_tx_min_max_pwr
(
  rfm_cdma_band_class_type const band,
  int32 *p_tx_min_pwr, 
  int32 *p_tx_max_pwr
)
{
  boolean is_high_band;

  is_high_band = rf_cdma_is_high_band(band);

  if ( is_high_band == TRUE )
  {
    *p_tx_min_pwr = -634;  /* -63.4 dBm: hi-band */
  }
  else
  {
    *p_tx_min_pwr = -604;  /* -60.4 dBm: low-band */
  }

  *p_tx_max_pwr = *p_tx_min_pwr + 1023; /* 102.4 dBm: TX dynamic range */

} /* rf_cdma_get_tx_min_max_pwr  */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns turn-around constant (TC) for given band
 
  @param band for which TC is queried
 
  @return TC as defined by the standard. Returns 0 for invalid/unsupported bands

*/
int
rf_cdma_get_turnaround_constant
(
  rfm_cdma_band_class_type const band
)
{
  int turnaround_constant=0;
  boolean is_high_band;

  is_high_band = rf_cdma_is_high_band(band);

  /* Return the TC as defined by the CDMA standard */
  if ( is_high_band == TRUE )
  {
    turnaround_constant = -76;
  }
  else
  {
    turnaround_constant = -73;
  }

  return turnaround_constant;

} /* rf_cdma_get_turnaround_constant  */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get Rx Calibration Compensation

  @details
  This function calculates all the Rx calibration compensation required for
  the target and returns the value. This final compensation value is applied
  to the Rx Calibration Data. This calibration value is <b>negated</b> from the
  RF Calibration Data

  @param device
  RF Device whose Rx Calibration Compensation is to be applied
 
  @param band
  Band for which Compensation is queried
 
  @param chan
  Channel for which Compensation is queried

  @return
  Calibration Compensation value to be negated on Rx Cal Data. This will be
  in 1/640th of dB.
*/
int16
rf_cdma_get_calibration_compensation
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan
)
{
  int16 total_comp = 0; /* Total Compensation */
  int16 nominal_bbf_gain = 0; /* Var to store BBF Gain */
  int16 rxf_comp = 0; /* Var to store RXF Compensation */
  const rf_cdma_data_status_type *dev_status_r; /* Device Status Read Pointer */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */

  dev_status_r = rf_cdma_get_device_status( device );

  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL || dev_status_r == NULL)
  {
    RF_MSG( RF_ERROR, "rf_cdma_get_calibration_compensation: "
                      "Null pointer returned to logical_dev ");
    return 0;
  }

  /* Get BBF Gain from Device */
  rfdevice_cdma_rx_cmd_dispatch( logical_dev->rx_device[band],
                                 RFDEVICE_GET_NOMINAL_BBF_GAIN_DB10,
                                 &nominal_bbf_gain );

  /* Convert the BBF Nominal Gain from RTR to 1/640th units */
  nominal_bbf_gain = nominal_bbf_gain * 64;

  /* Get RXF Compensation */
  rxf_comp = rf_cdma_msm_get_cal_compensation( dev_status_r->rxlm_handle );

  /* Compute Total Compensation */
  total_comp = nominal_bbf_gain + rxf_comp;

  /* Return the compensation */
  return total_comp;

} /* rf_cdma_get_calibration_compensation */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converts the raw Rx AGC into the requested dBm units (dBm10, dBm256, etc)

  @param format
  in which the raw AGC is to be returned
 
  @param raw_rxagc
  Converted dBm units, and the same value if format is not supported

  @return
  Converted dBm units 
*/
int16
rf_cdma_convert_rxagc
(
  const rf_cdma_agc_format_type format,
  const int16 raw_rxagc
)
{
  int16 rxagc_conv; /* RxAGC converted to requested units */
   
  /* RxAGC (dBm) = (raw_RxAGC * (102.4/65536) - Ref-level)) */
  /*             = (raw_RxAGC * (1/640) - 63.8            )  */

  /* Perform conversion based on format */
  switch (format)
  {
  case RFM_CDMA_AGC_FORMAT__RAW: /* RAW RxAGC */
    rxagc_conv = raw_rxagc;
    break;

  case RFM_CDMA_AGC_FORMAT__DBM256: /* RxAGC in dBm*256 units  */
    /* RxAGC (dBm256) = (256*raw_RxAGC * (102.4/65536) - 256*Ref-level)) */
    /*             = (raw_RxAGC * (256/640) - 63.8 *256)                 */
    /*             = (raw_RxAGC*4/10) - 16333)                            */ 
    rxagc_conv = (int16) ((rf_divide_with_rounding(raw_rxagc*4,10) - 16333));
    break;

  case RFM_CDMA_AGC_FORMAT__DBM10: /* RxAGC in dBm*10 units   */
    /* RxAGC (dBm10) = (10*raw_RxAGC * (102.4/65536) - 10*Ref-level)) */
    /*             = (raw_RxAGC * (10/640) - 63.8*10)                 */
    /*             = (raw_RxAGC/64) - 638)                            */ 
    rxagc_conv = (int16) (rf_divide_with_rounding(raw_rxagc,64) - 638);
    break;

  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_convert_rxagc(): Invalid Format %d",
              format );
    rxagc_conv = raw_rxagc;
    break;
  } /* switch (format) */

  return rxagc_conv;
} /* rf_cdma_convert_rxagc */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Converts the raw Tx AGC into the requested dBm units (dBm10, dBm256, etc)

  @param format
  in which the raw AGC is to be returned

  @param turnaround_const
  Turnaround constant for current band in dB (NOT dB10)
 
  @param raw_txagc
  AGC value that needs to be converted

  @return
  Converted dBm units, and the same value if format is not supported
*/
int16
rf_cdma_convert_txagc
(
  const rf_cdma_agc_format_type format,
  const int16 turnaround_const,
  const int16 raw_txagc
)
{
  int16 conv_txagc; /* holds the converted TxAGC value */

  /* TxAGC (dBm) = (K + Ref_level - raw_TxAGC * (102.4/65536)) */
  /*             = (K + 63.8 - raw_TxAGC/640                   */

  switch( format )
  {
  case RFM_CDMA_AGC_FORMAT__RAW:
    /* in raw dBm units - depends resolution used by FW */
    conv_txagc = raw_txagc;
    break;

  case RFM_CDMA_AGC_FORMAT__DBM256:
    conv_txagc = (int16) ( turnaround_const*256 + 16333 - 
                           rf_divide_with_rounding(raw_txagc*4,10));
    break;

  case RFM_CDMA_AGC_FORMAT__DBM10:
    conv_txagc = (int16) ( turnaround_const*10 + 638 - 
                           rf_divide_with_rounding(raw_txagc, 64));
    break;

  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_convert_txagc: Invalid format requested"
                        " = %d", format );
    conv_txagc = raw_txagc;
    break;

  } /* switch( format ) */

  return conv_txagc; 

} /* rf_cdma_convert_txagc */

/*! @} */

/*============================================================================*/
/*!
  @name Power Amplifier

  @brief
  Utility Functions related to Power Amplifier
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert the PA Switchpoint from NV to dBm10 units
 
  @details
  Based on the band, this function will convert the PA Switchpoint to dBm10
  units
 
  @param pa_sp_nv
  PA Switchpoint values from NV to be converted to dBm10
 
  @param band
  RF Band to which the PA switchpoint correspond to
 
  @return
  PA switchpoint converted to dBm10 units
*/
int16
rf_cdma_covert_pa_switchpoint_to_dbm
(
  int16 pa_sp_nv,
  rfm_cdma_band_class_type band
)
{
  boolean is_high_band;
  int16 pa_thres_conv_constant = 0;
  int16 pa_sp_dbm = 0;

  is_high_band = rf_cdma_is_high_band(band);

  if ( is_high_band == TRUE )
  {
    pa_thres_conv_constant= 630;
  }
  else
  {
    pa_thres_conv_constant= 600;
  }

  pa_sp_dbm = ( pa_sp_nv * 4 ) - pa_thres_conv_constant;

  return pa_sp_dbm;

} /* rf_cdma_covert_pa_switchpoint_to_dbm */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Map PA Range to PA Gain state

  @details
  This function will query NV to get the PA_R_MAP to convert PA Range to
  PA Gain State

  @param device
  RF Device for PA Range to PA State conversion is to be done

  @param band
  Band for which the PA range to state mapping should be done.
 
  @param pa_range
  PA Range which needs to be mapped to PA State
 
  @return
  PA State correspondint to the given PA Range
*/
rfm_pa_gain_state_type
rf_cdma_map_pa_range_to_pa_state
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_range_type pa_range
)
{
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rfm_pa_gain_state_type pa_state_mapped = RFM_PA_GAIN_STATE_NUM;
  int16 *pa_r_map;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_map_pa_range_to_pa_state: "
                      "Null pointer returned to logical_dev ");
    /* Return invalid gain state val */
    return pa_state_mapped;
  }

  /* Get 1x NV Data */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band], 
                                             band ); 
  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_1 (RF_ERROR, "rf_cdma_map_pa_range_to_pa_state: "
                        "Failed to retrieve valid Config" 
                        " data from NV for band %d", band);
    /* Return invalid gain state val */
    return pa_state_mapped;
  }

  pa_r_map = &(rf_1x_config_ptr->cdma_common_ptr->tx_static.pa_r_map[0]);

  for ( pa_state_mapped = RFM_PA_GAIN_STATE_0; 
        pa_state_mapped < RFM_PA_GAIN_STATE_NUM;
        pa_state_mapped++ )
  {
    if ( pa_range == (rfm_pa_range_type)(pa_r_map[pa_state_mapped]) )
    {
      /* Once the PA range matches, break out of the loop */
      break;
    }
  }

  /* Return Mapped PA State */
  return pa_state_mapped;

} /* rf_cdma_map_pa_range_to_pa_state */


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Map PA state to PA Gain Range

  @details
  This function will query NV to get the PA_R_MAP to convert PA State to
  PA Gain Range

  @param device
  RF Device for PA Range to PA State conversion is to be done

  @param band
  Band for which the PA range to state mapping should be done.
 
  @param pa_state
  PA State which needs to be mapped to PA Gain Range
 
  @return
  PA Gain Range correspondint to the given PA State
*/

rfm_pa_range_type
rf_cdma_map_pa_state_to_gain_range
(
  rfm_device_enum_type device,
  rfm_cdma_band_class_type band,
  rfm_pa_gain_state_type pa_state
)
{
  const rf_1x_config_type *rf_1x_config_ptr;   /* Pointer to 1x RF NV data */
  const rfc_cdma_logical_device_params_type *logical_dev; /* Logical Device */
  rfm_pa_range_type gain_range_mapped = RFM_PA_GAIN_RANGE_NUM;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  if ( logical_dev == NULL )
  {
    RF_MSG( RF_ERROR, "rf_cdma_map_pa_state_to_gain_range: "
                      "Null pointer returned to logical_dev ");
    /* Return invalid gain state val */
    return gain_range_mapped;
  }

  /* Get 1x NV Data */
  rf_1x_config_ptr = rf_cdma_nv_get_1x_data( logical_dev->tx_nv_path[band], 
                                             band ); 
  if ( rf_1x_config_ptr == NULL )
  {
    RF_MSG_1 (RF_ERROR, "rf_cdma_map_pa_state_to_gain_range: "
                        "Failed to retrieve valid Config" 
                        " data from NV for band %d", band);
    /* Return invalid gain state val */
    return gain_range_mapped;
  }

  /* Obtain PA range map for requested PA state */
  gain_range_mapped = 
     (rfm_pa_range_type)rf_1x_config_ptr->cdma_common_ptr->
                                            tx_static.pa_r_map[pa_state];

  /* Return Mapped PA Range */
  return gain_range_mapped;

} /* rf_cdma_map_pa_state_to_gain_range */

/*! @} */

/*============================================================================*/
/*!
  @name Miscellaneous

  @brief
  Miscellaneous utility functions
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function used to compute the IQ gain adjustment value in HW units from the
  dBx10 offset value provided in NV.

  @details
 
  @param dBx10_offset
  Power adjustment offset in dBx10
 
  @return
  Returns the IQ gain adjust value in Q16 format, after converting
  from NV dB offset value to HW units
*/
uint64
rf_cdma_utils_compute_tx_iq_gain_adj_val
(
  int8 dBx10_offset
)
{
  int32 exp10_input_q26, exp10_output_q15;
 
  if ( dBx10_offset != 0 )
  {
    //convert dBx10_offset to a format suitable for qdsp4_exp10
    //Input should be in Q26 => (dBx10_offset * 2^ 26 * 1/10 * 1/20) = 335544.
    exp10_input_q26 = (int32) dBx10_offset * (335544);
    
    //output is in Q15. 
    exp10_output_q15 =  rf_compute_exp10(exp10_input_q26);
  }
  else
  {
    /* Don't compute the scaling, just return 1 in Q15 format */
    exp10_output_q15 = (1 << 15);
  }
 
  return (exp10_output_q15);

} /* rf_cdma_utils_compute_tx_iq_gain_adj_val() */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Converts time passed in micro-secs to USTMR units
 
  @param time_us  
  time in us
    
  @return
  converted to USTMR units
*/
int32
rf_cdma_convert_cx1_to_ustmr_units
(
  int32 cx1_units
)
{
  int32 ustmr_units; 
  double time_us;

  time_us = (double) (cx1_units /1.2288) ; /* Chip Rate 1.2288 Mhz*/
  ustmr_units = (int32) ceil ( time_us * 19.2); /* USTMR runs at 19.2 Mhz */

  return ustmr_units;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Converts sys_band_class_e_type to sys_sband_e_type
 
  @param band  
  sys_band_class_e_type
    
  @return
  sys_sband_e_type
*/
sys_sband_e_type
rf_cdma_convert_sys_band_to_sys_sband
( 
  sys_band_class_e_type band
)
{
  sys_sband_e_type sys_sband;

  switch( band)
  {
  case SYS_BAND_BC0:
    sys_sband = SYS_SBAND_BC0_A;
    break;
  case SYS_BAND_BC1:
    sys_sband = SYS_SBAND_BC1;
    break;
  case SYS_BAND_BC3:
    sys_sband = SYS_SBAND_BC3;
    break;
  case SYS_BAND_BC4:
    sys_sband = SYS_SBAND_BC4;
    break;
  case SYS_BAND_BC5:
    sys_sband = SYS_SBAND_BC5;
    break;
  case SYS_BAND_BC6:
    sys_sband = SYS_SBAND_BC6;
    break;
  case SYS_BAND_BC10:
    sys_sband = SYS_SBAND_BC10;
    break;
  case SYS_BAND_BC11:
    sys_sband = SYS_SBAND_BC11;
    break;
  case SYS_BAND_BC14:
    sys_sband = SYS_SBAND_BC14;
    break;
  case SYS_BAND_BC15:
    sys_sband = SYS_SBAND_BC15;
    break;
  default:
    sys_sband = SYS_SBAND_MAX;
    /* caller of this API should handle the error return message */
    break;
  }
  return sys_sband;

} /* rf_cdma_convert_sys_band_to_sys_sband() */

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  Check if the band is supported on a given device,  by comparing it with
  band_mask from NV
 
  @param band
  Band which is to be validated
 
  @param path
  rf_path on which the band is

  @return
  TRUE if band is supported and FALSE otherwise
*/
boolean
rf_cdma_is_band_supported
(
  const rfm_device_enum_type device, 
  const rfm_cdma_band_class_type band
)
{
  /* return val */
  boolean ret_val;
  /* Logical Device */
  const rfc_cdma_logical_device_params_type *logical_dev;
  /* Band config NV */
  uint64 nv_mask;
  /* Bank mask */
  uint64 bc_mask;
  /* sys_band_class_e_type */
  sys_band_class_e_type sys_band;
  /* sys_sband_e_type */
  sys_sband_e_type sys_sband;
  
  /* Get Logical Device Param */
  logical_dev = rfc_cdma_get_logical_device_params ( device ) ;

  if ( logical_dev != NULL )
  {
    /* Get Band config NV */
    nv_mask = rfcommon_nv_get_rf_bc_config ( 
                          (rf_path_enum_type)(logical_dev->rx_nv_path[band]) ) ;   
    
    /* Convert rfm_cdma_band_class_type to sys_band */
    sys_band = rf_cdma_rf_band_type_to_sys_band_type ( band ) ;
    
    /* Convert sys_band_class_e_type to sys_sband_e_type */
    sys_sband = rf_cdma_convert_sys_band_to_sys_sband ( sys_band ) ; 
    
    /* Get sys band mask */
    bc_mask = ((sys_band_mask_type) SYS_BM_64BIT(sys_sband)) ;
    
    /*Check if the band is supported in Band config NV*/
    if ( ( nv_mask & bc_mask) != 0)
    {
      ret_val = TRUE;
    }
    else
    {
      ret_val = FALSE;
    }
  } /* if ( logical_dev != NULL ) */
  else
  {
    RF_MSG( RF_ERROR, "rf_cdma_is_band_supported: "
                      "Null pointer returned to logical_dev ");
    ret_val = FALSE;
  }

  return ret_val;
}
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert RFM CDMA Band to RF Card Band
 
  @details
  This function will map the given RFM CDMA band to generic RF Card Band.
 
  @param rfm_band
  RFM CDMA to be mapped
 
  @return
  Mapped RF Card Card Band
*/
rf_card_band_type
rf_cdma_convert_rfm_band_to_card_band
(
  rfm_cdma_band_class_type rfm_band
) 
{
  rf_card_band_type rfc_band = RF_MAX_BAND;

  switch( rfm_band )
  {
  case RFM_CDMA_BC0:
    rfc_band = RF_BC0_BAND;
    break;
  case RFM_CDMA_BC1:
    rfc_band = RF_BC1_BAND;
    break;
  case RFM_CDMA_BC2:
    rfc_band = RF_BC2_BAND;
    break;
  case RFM_CDMA_BC3:
    rfc_band = RF_BC3_BAND;
    break;
  case RFM_CDMA_BC4:
    rfc_band = RF_BC4_BAND;
    break;
  case RFM_CDMA_BC5:
    rfc_band = RF_BC5_BAND;
    break;
  case RFM_CDMA_BC6:
    rfc_band = RF_BC6_BAND;
    break;
  case RFM_CDMA_BC7:
    rfc_band = RF_BC7_BAND;
    break;
  case RFM_CDMA_BC8:
    rfc_band = RF_BC8_BAND;
    break;
  case RFM_CDMA_BC9:
    rfc_band = RF_BC9_BAND;
    break;
  case RFM_CDMA_BC10:
    rfc_band = RF_BC10_BAND;
    break;
  case RFM_CDMA_BC11:
    rfc_band = RF_BC11_BAND;
    break;
  case RFM_CDMA_BC12:
    rfc_band = RF_BC12_BAND;
    break;
  case RFM_CDMA_BC13:
    rfc_band = RF_BC13_BAND;
    break;
  case RFM_CDMA_BC14:
    rfc_band = RF_BC14_BAND;
    break;
  case RFM_CDMA_BC15:
    rfc_band = RF_BC15_BAND;
    break;
  case RFM_CDMA_BC16:
    rfc_band = RF_BC16_BAND;
    break;
  case RFM_CDMA_BC18:
    rfc_band = RF_BC18_BAND;
    break;
  case RFM_CDMA_BC19:
    rfc_band = RF_BC19_BAND;
    break;
  default:
    RF_MSG_1( RF_ERROR, "rf_cdma_utils_convert_rfm_band_to_card_band: "
                      "Invalid RFM band :%d" ,rfc_band );
    break;
  }/* switch( rfm_band ) */

  return( rfc_band );

} /* rf_cdma_utils_convert_rfm_band_to_card_band */

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */

