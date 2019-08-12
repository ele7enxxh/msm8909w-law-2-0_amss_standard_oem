/*! 
  @file
  rf_hdr_power_limiting.c
 
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

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/rf/src/rf_hdr_power_limiting.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/12/14   spa     Change tx_nv_path to be per band per logical device
11/06/14   spa     Change rx_nv_path to be per band per logical device
10/11/13   JJ      fix a bug in rf_hdr_plim_update_lim_vs_temp_vs_freq 
09/12/13   sar     Fixed compiler warnings.
08/29/13   APU     Use CCP Max power for calculating temperature for Funnel-Tbl.
05/31/13   APU     Optimized the check and retrieval of smem address.    
05/30/13   JJ      Convert rf_band type to sys_band type, remove un-necessary  
                   conversion in func rf_hdr_get_min_carrier_separation_kHz
05/17/13   APU     Added more documentation to the code.            
05/15/13   APU     Added changes for Enhanced MCDO coonection setup.
05/09/13   APU     Resolved KW error.
05/02/13   APU     1> CCP is now a seperate module that contributes to the HDR 
                      MAX power module.
                   2> Placeholder for clamping HDR MAX power tables sent to FW 
                      when CCP values are greater than SAR values.
03/13/13   APU     Fixed Klockwork error.                             
11/03/13   vr      Fix to correct the LUT indicator bits
01/16/12   APU     Fixed HDR max power where HDR NV was being ignored.
12/03/12   APU     Fixed compiler warning.
12/03/12   APU     Added a one time check for therm cal monotonicity. 
09/17/12   APU     Take neq power as max power in funneling case when deciding 
                   the HDET power mode. 
09/11/12   APU     Unconditionally populate SMEM with Enhanced HDR MAX-PWR NVs. 
08/24/12   cd      CDMA EPT Temp and HDET feature support
07/26/12   APU     Added Enhanced PA backoff support.
05/03/12   zhaow   Fixed off-target compiling error
04/04/12   aro     Added callback functions to HDR Tx interfaces
03/21/12   spa     Added max device check in rf_cdma_get_mutable_device_status()
03/21/12   APU     Added rf_hdr_get_max_tx_power_for_band() API 
02/07/12   APU     Added F3 messages.
02/07/12   APU     Temp Klockwork fix.
02/07/12   cd      Changed the get lowest max power limit function to be 
                   a public API 
02/07/12   APU     Added NV support for MC-HDR power limiting.
02/03/12   hdz     Updated lin_limit unconditionally  
01/27/12   hdz     changed rf_hdr_plim_update_temp and rf_hdr_plim_update_ 
                   temp_mdsp to static type function 
01/27/12   hdz     Remove fcomp from max power limit module 
01/19/12   cd      Move PLIM linearizer limit configuration to PLIM module and 
                   store the limit in dBm10
12/08/11   hdz     Added rf_hdr_plim_update_temp()
11/14/11   Saul    SV Limiting. Made implementation more modular.
11/14/11   Saul    Thermal Backoff. Made implementation more modular.
11/11/11   Saul    Thermal Backoff Support.
11/10/11   Saul    SV Limiting. Initial support.
11/03/11   cd      Provide max linearizer entry info to power-limiting 
10/12/11   bmg     Added DSI SAR control to target max power limit
09/29/11   shb     Saturate max power within dynamic range before sending to FW
09/13/11   shb     Added PLIM state machine image to doxy documentation
09/06/11   shb     Added rf_hdr_plim_get_err_adj_max_limit()
08/31/11   shb     Added debug message
08/30/11   shb     Initial version - based on rf_1x_power_limiting.c

==============================================================================*/
#include "comdef.h"
#ifdef FEATURE_CDMA1X
#include "rf_hdr_power_limiting.h"
#include "rfcommon_msg.h"
#include "rf_cdma_utils.h" /* rf_cdma_get_turnaround_constant() */
#include "rf_cdma_data.h" /* rf_cdma_get_mutable_device_status etc.. */
#include "rfm_internal.h" /* rfm_get_calibration_state() */
#include "rf_hdr_mdsp.h"   /* rf_hdr_mdsp_configure_tx_power_limit() */
#include "rf_cdma_mdsp.h" /* rf_cdma_mdsp_dbm10_to_txagc() */
#include "rfcommon_math.h" /* rf_limit_s32 */
#include "rfcommon_core_sar.h" /* rfcommon_core_sar_get_state() */
#include "rf_cdma_utils_freq.h"

// Wildcard for HDR Carrier Configuration Pattern (CCP) bitmask
#define RFC_HDR_CCP_ANY     ((uint16)~0)
// Wildcard for HDR Reverse Link Bandwidth
#define RFC_HDR_RL_BW_ANY   ((uint8)~0)
// Wildcard for HDR Lowest Freq (LF) reverse link Channel
#define RFC_HDR_LF_CHAN_ANY ((uint16)~0)

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


#define INT_SWAP(a, b)   ((a)^=(b),(b)^=(a),(a)^=(b))

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to carrier info required by the HDR max power module.

  @details
  This structure is only used by the max power module to store carrier
  charecteristics that are required to come up with carrier config pattern[CCP]
  L1 and FW use index to notify RF of the carriers in MCDO. The max power NV is
  stored based on ascending frequencies. This structure stores frequencies in
  ascending order and corresponsing indices that L1 refers to while calling the
  RFA APIs

*/
typedef struct
{

  /*! This IS NOT THE index that L1 used to call RFA API BUT 
      the index when the carriers per SORTED by frequency */
  uint16 index ;

  /*! The channel paired with the index decribed above */
  uint16 channel ;

  /*! The frequency in KHz of the channel decribed above */
  uint32 freqKHz ;

} rf_hdr_carrier_characteristic_type ;

/*!
  @brief
  Helper function to get the pointer to HDR max power NV items.
 
  @param rf_hdr_config_ptr [Input]
 
  @param dev_status_w [Input]
 
  @return rf_cdma_tx_pwr_limit_data_type [Output]
  The address of the first HDR max power type data structure for
  the given band
*/
const rf_cdma_tx_pwr_limit_data_type* 
rf_hdr_get_max_pwr_nv_addr (
  const rf_hdr_config_type* rf_hdr_config_ptr ,
  rf_cdma_data_status_type* dev_status_w )
{
  const rf_cdma_tx_pwr_limit_data_type* tx_pwr_limit_data  = 
      &(rf_hdr_config_ptr->tx_static.tx_pwr_limit_data[0]) ;  /* Pointer to 1x static NV data */
  return tx_pwr_limit_data ;
}

/*!
  @brief
  Used to convert the carrier array in bit pattern.
  
  @details
  Used to convert the carrier array in bit pattern. 
  Resulting CCP is what we use to look up in the NV max pwr LUTs
 
  @param rfm_cdma_carrier_en_mask_type
  Active carriers.

  @return carrier_pattern
  A bit pattern i.e. CCP.  

*/
uint16
rf_hdr_convert_carrier_arr_into_bit_pattern ( rfm_cdma_carrier_en_mask_type carrier_mask ,
                                                rf_cdma_data_status_type* dev_status_w  )
{
  uint16 count , carrier_pattern = 0 ;
  for ( count = 0 ; count < RFM_CDMA_CARRIER_NUM ; count++ )
  {
    uint16 bit_position = ( carrier_mask.enable[count] == TRUE ) ? 1 : 0 ;
    carrier_pattern  <<= 1 ;
    carrier_pattern |= bit_position ;
  }
  return carrier_pattern ;
}

/*!
 * @brief 
 * In an MCDO call the 2 carriers must have a mimimum seperation 
 * between 2 adjacent carriers. This seperation is band 
 * dependent. The function returns the minimum seperation in KHz 
 * based on the band 
 * 
 * @param dev_status_w 
 * 
 * @return uint32 
 */
uint32 
rf_hdr_get_min_carrier_separation_kHz( rf_cdma_data_status_type* dev_status_w )
{
   uint32 min_separation_kHz = 0;
   rfm_cdma_band_class_type band =  dev_status_w->curr_band ;
   
   switch (band)
   {
       case RFM_CDMA_BC0:
          min_separation_kHz = 1230;            // 41ch * 30
          break;

       case RFM_CDMA_BC1:
       case RFM_CDMA_BC3:
       case RFM_CDMA_BC4:   // set to fall through intentionally
       case RFM_CDMA_BC5:   // set to fall through intentionally
       case RFM_CDMA_BC6: 
       case RFM_CDMA_BC10:  // set to fall through intentionally
       case RFM_CDMA_BC14:  // set to fall through intentionally
       case RFM_CDMA_BC15:  // set to fall through intentionally
          min_separation_kHz = 1250;            // 25ch * 50
          break;
   
       default: 
          min_separation_kHz = 0 ;
          break;
   } //switch (band)

   if (0 == min_separation_kHz)
   {
       RF_MSG_1 ( RF_ERROR, "rf_hdr_get_min_carrier_separation_kHz(band = %d) failed!",  band ) ;
   }

   return min_separation_kHz;
} //rf_hdr_get_min_carrier_separation_kHz()

/*!
  @brief
  The CCPs are looked up based on ascending frequency. L1 however can provide the channels in
  any order it desires. This function helps arranges the channels in the order of frequency to 
  aid the CCP lookup.
 
  @param device_status
 
  @param carrier_info
  This function fills in the carrier_info array [channels and
  frequency] for all the carriers in call after doing an
  ascending frequency based sort on all the carriers.
*/
void
rf_hdr_freq_based_sort (rf_cdma_data_status_type* dev_status_w, 
                        rf_hdr_carrier_characteristic_type* carrier_info)
{
  uint8 count;
  /* variable time 3 number sort */
  if ( carrier_info[1].freqKHz < carrier_info[0].freqKHz ) 
  {    
    INT_SWAP(carrier_info[1].freqKHz , carrier_info[0].freqKHz) ;
    INT_SWAP(carrier_info[1].channel , carrier_info[0].channel) ;
    INT_SWAP(carrier_info[1].index , carrier_info[0].index) ;
  }

 
  if (dev_status_w->num_carriers > 2)
  {
    if ( carrier_info[2].freqKHz < carrier_info[1].freqKHz ) 
    {
      INT_SWAP(carrier_info[2].freqKHz , carrier_info[1].freqKHz);
      INT_SWAP(carrier_info[2].channel , carrier_info[1].channel) ;
      INT_SWAP(carrier_info[2].index , carrier_info[1].index) ;
    }
    if ( carrier_info[1].freqKHz < carrier_info[0].freqKHz ) 
    {
      INT_SWAP(carrier_info[1].freqKHz , carrier_info[0].freqKHz) ;
      INT_SWAP(carrier_info[1].channel , carrier_info[0].channel) ;
      INT_SWAP(carrier_info[1].index , carrier_info[0].index) ;
    }
  }

  for ( count = 0 ; count < dev_status_w->num_carriers; count++ )
  {
    RF_MSG_3( RF_LOW , "rf_hdr_freq_based_sort(): "
    "|carr[%d].index=%d chan=%d|" , count , 
     carrier_info[count].index , carrier_info[count].channel ) ;
  }
}              

/*!
  @brief
  For MCDO setup enhancement feature, L1 needs flexibility to flip between
  SCDO or MCDO maximum power. When L1 wants to choose a max power limit
  that is not same as the max power for the TCA-CCP that the radio originally
  tuned to, we need a way to set a new max power in override scenarios.
  To avoid temp comp or HDET loops from trampling the new override, we need
  an override mechanism in HDR PLIM.
  The clear override tells PLIM to resume normal operation, meaning use the
  CCP which the radio was tuned to during wakeup tx.
 
  @param plim_data
*/ 
void
rf_hdr_plim_clear_ccp_override ( rf_cdma_plim_data_type* plim_data )
{
  plim_data->max_power_limits.
    max_power_info.algo_used.override_max_power = FALSE ;
}

/*!
  @brief
  For MCDO setup enhancement feature, L1 needs flexibility to flip between
  SCDO or MCDO maximum power. When L1 wants to choose a max power limit
  that is not same as the max power for the TCA-CCP that the radio originally
  tuned to, we need a way to set a new max power in override scenarios.
  To avoid temp comp or HDET loops from trampling the new override, we need
  an override mechanism in HDR PLIM.
  The set override tells PLIM to go into override max power mode, meaning 
  use the override the new CCP which L1 provides AFTER the CCP when the radio 
  was tuned to during wakeup tx.
 
  @param plim_data
*/ 
void
rf_hdr_plim_set_ccp_override ( rf_cdma_plim_data_type* plim_data )
{
  plim_data->max_power_limits.
    max_power_info.algo_used.override_max_power = TRUE ;
}              

/*!
  @brief
  For MCDO setup enhancement feature, L1 needs flexibility to flip between
  SCDO or MCDO maximum power. When L1 wants to choose a max power limit
  that is not same as the max power for the TCA-CCP that the radio originally
  tuned to, we need a way to set a new max power in override scenarios.
  To avoid temp comp or HDET loops from trampling the new override, we need
  an override mechanism in HDR PLIM.
  The geet override tells PLIM if we are override max power mode, meaning 
  use the override the new CCP which L1 provides AFTER the CCP when the radio 
  was tuned to during wakeup tx. OR in non-override mode, where CCP provided
  CCP would be the CCP used for max power.
 
  @param plim_data
*/ 
boolean
rf_hdr_plim_get_ccp_override ( rf_cdma_plim_data_type* plim_data )
{
  return ( plim_data->max_power_limits.
    max_power_info.algo_used.override_max_power ) ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Used to set the max power algorithm being used.
  
  @details
  The HDR max power can be set using one of the following: 
  RF_HDR_USE_ENHANCED_HDR_MAX_POWER  OR
  RF_HDR_USE_LEGACY_HDR_MAX_POWER    OR
  RF_HDR_USE_1X_MAX_POWER             OR [Default]
  depending the state of the hdr max power NVs. This setter function sets one
  of the algorithms to be used. 

  @param plim_data
  Relavent PLIM data structure for a given device.

  @return rf_hdr_max_power_algo_used_type
  Simply returns whatever was passed to be set.
*/
rf_hdr_max_power_algos
rf_hdr_plim_set_algo_used ( rf_cdma_plim_data_type* plim_data ,
                            rf_hdr_max_power_algos max_pwr_algo_used )
{
  plim_data->max_power_limits.max_power_info.algo_used.max_power_algo = 
    max_pwr_algo_used ;
  return plim_data->max_power_limits.max_power_info.algo_used.max_power_algo ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns to the max power algorithm currently being used.
  
  @details
  The HDR max power can be set using one of the following: 
  RF_HDR_USE_ENHANCED_HDR_MAX_POWER  OR
  RF_HDR_USE_LEGACY_HDR_MAX_POWER    OR
  RF_HDR_USE_1X_MAX_POWER             OR [Default]
  depending the state of the hdr max power NVs. This getter function gets the
  algorithm currently being used. 

  @param plim_data
  Relavent PLIM data structure for a given device.

  @return rf_hdr_max_power_algo_used_type
  Returns algo being used.
*/
rf_hdr_max_power_algos
rf_hdr_plim_get_algo_used ( const rf_cdma_plim_data_type* plim_data )
{
  return plim_data->max_power_limits.max_power_info.algo_used.max_power_algo ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Provides L1 with funneling Bias read from NV and converted to Q21.10 units
  
  @details
  Provides L1 with funneling Bias read from NV and converted to Q21.10 units.
  If all the carriers have a 0 funnel bias, it is an indicative of an error.


  @param funnel_bias_array_ptr
  Pointer to an array of size 3 of type rfm_hdr_per_carrier_funneling_bias_type.
  The array will be populated with carrier_id and funnel_bias for each carrier.
  In the event of a hole in carrier assignment, the carrier_id for the entry
  corresponding to the hole would be set to RFM_HDR_INVALID_CARRIER_ID.

  @param device
  Device whose funnel bias is requested

  @return rf_cdma_config_data_ptr
  TRUE: If any of the carrier had a valid value for funnel bias.
  Also indicates to L1 that "Enhanced PA backoff" is enabled 
  FALSE: Invalid [0 value] for all the funnel biases.
  Also indicates to L1 that "Enhanced PA backoff" is disabled
*/
boolean 
rf_hdr_plim_get_funneling_metrics
(
  rfm_device_enum_type device ,
  rfm_hdr_per_carrier_funneling_bias_type* funnel_bias_array_ptr
) 
{
  boolean success = FALSE ;
  const rf_cdma_data_status_type* p_device = 
      rf_cdma_get_device_status ( device ) ;

  if ( NULL != p_device )
  {
    const rfm_hdr_per_carrier_funneling_bias_type* funnel_bias_array = 
        &(p_device->plim.max_power_limits.max_power_info.funnel_info[0]) ;
    if ( NULL != funnel_bias_array )
    {
      uint8 count = 0 ;
      for ( ; count < RFM_CDMA_CARRIER_NUM ; count++ )
      {
        funnel_bias_array_ptr[count] = funnel_bias_array[count] ;
      }

      if ( RF_HDR_USE_ENHANCED_HDR_MAX_POWER != 
         p_device->plim.max_power_limits.max_power_info.algo_used.max_power_algo )
      {
        RF_MSG ( RF_LOW , 
                 "rfm_hdr_get_funneling_metrics(): " 
                 "RF_HDR_USE_ENHANCED_HDR_MAX_POWER "
                 "is returned as disabled by algo_used" ) ;
      }
      else
      {
        success = TRUE ;
      } /* else if ( RF_HDR_USE_ENHANCED_HDR_MAX_POWER != algo_used ) */
    }
  }
  return success ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Updates the funnel bias in an event of Traffic Channel Assignment [TCA]
  
  @details
  Updates the funnel bias in an event of TCA or in other words when carrier
  add//drop or a retune happens.

  @param dev_status_w
  
  @param carrier_info[]
  For every device carrier information is organized in an ascending frequency
  order. 

  @param hdr_pwr_data
  Pointer to the structure that contains the information about HDR MAX power
  as read from the NV.

  @param plim_data
  plim_data pointer for a given device.

  @return void
*/
void
rf_hdr_update_funnel_info ( 
    rf_cdma_data_status_type* dev_status_w , 
    rf_hdr_carrier_characteristic_type carrier_info[] , 
    const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
    rf_cdma_plim_data_type* plim_data ) 
{
  int count = 0 ;
  rfm_hdr_per_carrier_funneling_bias_type* p_funnel_info = 
        &(plim_data->max_power_limits.max_power_info.funnel_info[0]) ;
  for ( count = 0 ; count < RFM_CDMA_CARRIER_NUM ; count++ )
  {
    if ( dev_status_w->curr_chans[count] != ((uint16)(~0)) )
    {
      uint8 funnel_bias_dBm10 = hdr_pwr_data->funnel_bias ;
     
      uint32 funnel_bias_Q21_10 = 0 ;
      if ( 0 != funnel_bias_dBm10 )
      { 
         /* n x 1024 = n << 10 so n x 1024/10 will be as follows: */
         funnel_bias_Q21_10  =
         (uint32)( ( funnel_bias_dBm10 << 9 ) / 5 ) ;
      }
      RF_MSG_2 ( RF_LOW , "CarrierID: %d, FunnelBias (Q21.10): %d" , 
                     count , funnel_bias_Q21_10 ) ;

      p_funnel_info[count].carrier_id = (uint8)carrier_info[count].index ;
      p_funnel_info[count].funnel_bias = funnel_bias_Q21_10 ;   
    }
    else
    {
      p_funnel_info[count].carrier_id = RFM_HDR_INVALID_CARRIER_ID ;
      p_funnel_info[count].funnel_bias = 0 ;    
    }
  }    
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get ascending freq sorted channels.
  
  @details
  Helper function to get ascending freq sorted channels. Channels provided
  by L1 may not be in order but HDR MAX power LUTs are in ascending freq order.	
  This function helps while calculating max power when CCPs are being looked up.
 
  @param device_status
  Relavent device_status which contains the list of carriers.

  @param carrier_info*
  This will contain the sorted list of channels.  
 
  @return void
*/
void    
rf_hdr_get_highest_lowest_channels ( 
    rf_cdma_data_status_type* dev_status_w , 
    rf_hdr_carrier_characteristic_type* carrier_info ) 
{
  int count = 0 ;
  for ( count = 0 ; count < dev_status_w->num_carriers; count++ )
  {
    if ( dev_status_w->curr_chans[count] != ((uint16)(~0)) )
    {
      carrier_info[count].channel = dev_status_w->curr_chans[count] ;
      carrier_info[count].freqKHz = rf_cdma_get_rx_carrier_freq ( 
                                                          dev_status_w->curr_band , 
                                                          carrier_info[count].channel) ; 
	  carrier_info[count].index = dev_status_w->carrier_index[count];
	  	
    }
  }    
  rf_hdr_freq_based_sort (dev_status_w, carrier_info) ;
}

/*! 
  @brief
  Construct CCP based on channel-freq imput.
 
  @details
  For MCDO the carriers can be placed in different bandwidths,
  like 1x, 2x, 3x etc. There can be single or multiple carriers
  1, 2 or 3. These carriers can be placed at certain places in
  the allowed MCDO BW. This function based on the numner of
  carriers and where are carriers are located in the spectrum
  constructs a binary pattern. This pattern is called the
  Carrier Configuration Pattern or CCP. CCP Examples: 1> 00101
  [CCP of 0x5 in 3X BW] 2> 00011 [CCP of 0x3 in 2x BW] 3> 10101
  [CCP of 0x15 in 5x BW]
 
  @param min_carrier_separation_kHz
  A minimum seperation for a given band between 2 carriers in a
  MCDO call.
 
  @param carrier_config_pattern_bmask
  One of the Outputs of this function.
  The CCP generated based on where the carriers are in the
  band. This binary pattern can tell how many carriers are in
  call, in what BW are they spread.
 
  @param lowest_freq_chan
  One of the Outputs of this function.
  The channel with the lowest frequency. The farthest carrier
  can be situated 5 x Min seperation from this channel's
  frequency.
 
  @param max_reverse_link_bw
  One of the Outputs of this function.
  The BW is what the 2 outermost carriers in terms of minimum
  seperation
 
*/
boolean
rf_hdr_construct_ccp ( 
  rf_hdr_carrier_characteristic_type* carrier_info ,
  uint32 min_carrier_separation_kHz ,
  uint16* carrier_config_pattern_bmask ,
  uint16* lowest_freq_chan ,
  uint8* max_reverse_link_bw ) 
{

  uint16 carrier_config_index = 0; /* Iterator through channels */
  uint32 carrier_tx_freq_kHz = 0; /* Freq of the channel being inspected */
  
  /* Indicator of the first valid carrier freq found */
  uint32 first_indexed_carrier_tx_freq_kHz = 0; 
  uint16 first_indexed_carrier_bitpos = 0;
  
  uint16 inter_carrier_separation_bits = 0;
  uint16 delta_bmask_growth = 0;
  boolean error = FALSE ;
  
  /* measure of how far the LEFT most and RIGHT most carriers are separated from each other */      
  *max_reverse_link_bw = 0;  
  *carrier_config_pattern_bmask = 0;
  *lowest_freq_chan = (uint16) (~0) ;

  for (
       carrier_config_index = 0;
       carrier_config_index < RFM_CDMA_CARRIER_NUM ;
       carrier_config_index ++ )
  {
    /* Carrier frequency offset */
    carrier_tx_freq_kHz = carrier_info[carrier_config_index].freqKHz ;
    if ( (min_carrier_separation_kHz > 0) &&
          (carrier_tx_freq_kHz > 0) )
    {
      /* Initialize the variables on first carrier match */     
      if (0 == first_indexed_carrier_tx_freq_kHz)
      {
          // determine the first indexed carrier in the CSR[]
          *carrier_config_pattern_bmask = 1;
          first_indexed_carrier_bitpos = 0;
          *max_reverse_link_bw = 1;            // currently just one carrier
          first_indexed_carrier_tx_freq_kHz = carrier_tx_freq_kHz;
          *lowest_freq_chan = carrier_info[carrier_config_index].channel ;
      }  
      else
      {
        /* Since carrier's freq is higher than that (i.e. to the RIGHT)
         * of the 1st indexed carrier, left shift the bitmask to accomodate
         *  the new carrier at the right most bit position, and record the 
         *  new bit position of the 1st assigned carrier.  */
         inter_carrier_separation_bits = 
             (uint16)( ( carrier_tx_freq_kHz
                         - first_indexed_carrier_tx_freq_kHz
                         + (min_carrier_separation_kHz/2) )
                                           / min_carrier_separation_kHz ) ;
         if (0 == inter_carrier_separation_bits)
         {
            ERR_FATAL("Invalid carrier config(Fc1 = %d, Fc2 = %d, MinSeparation = %d) KHz",
                      first_indexed_carrier_tx_freq_kHz, 
                      carrier_tx_freq_kHz, 
                      min_carrier_separation_kHz) ;
            error = TRUE ;
         }
         if (inter_carrier_separation_bits > first_indexed_carrier_bitpos)
         {
            delta_bmask_growth = (inter_carrier_separation_bits - first_indexed_carrier_bitpos);
            *carrier_config_pattern_bmask <<= delta_bmask_growth;
            first_indexed_carrier_bitpos += delta_bmask_growth;
            *carrier_config_pattern_bmask |= 1;
            /* grown by these many 1x BW to the right of the original */
            *max_reverse_link_bw += delta_bmask_growth;  
         }
         else
         {
            *carrier_config_pattern_bmask |= 
              (1 << (first_indexed_carrier_bitpos - inter_carrier_separation_bits));
            /* first_indexed_carrier_bitpos remains unchanged as we didn't
            have to shift bmask to the LEFT, and so is max_reverse_link_bw */
         }
      } /* else if (carrier_tx_freq_kHz > first_indexed_carrier_tx_freq_kHz) */
    } /* if (0 == first_indexed_carrier_tx_freq_kHz) */
  }/* for*/
  return error ;
}

/*!
  @brief constant that siginifies invalid channel
*/
const rfm_cdma_chan_type INVALID_CHANNEL = (rfm_cdma_chan_type)(~0) ;

/*!
  @brief
  When the radio is first tuned for transmit, it wakes up with a given 
  CCP. If and L1 needs to override the max power for the newer CCP,
  this function can modify the CCP. The modified CCP will then be used for
  deciding the new max power.
*/
boolean                
rf_hdr_override_channels ( 
rf_hdr_carrier_characteristic_type* carrier_info ,
rfm_cdma_chan_type override_channels[] )
{
  size_t count = 0 , num_channels = 0 ;
  boolean valid_index[RFM_CDMA_CARRIER_NUM] = { FALSE , FALSE , FALSE } ;

  for ( count = 0 ; count < RFM_CDMA_CARRIER_NUM ; count++ )
  { 
    size_t match_chan_count = 0 ; 
    for ( ; match_chan_count < RFM_CDMA_CARRIER_NUM && 
          override_channels[match_chan_count] != INVALID_CHANNEL ; match_chan_count++ )
    {
      /* If match found, leave carrier_info alone otherwise mark for invalidation */
      if ( carrier_info[count].channel == override_channels[match_chan_count] )
      {
        valid_index[count] = TRUE ;
        num_channels ++ ;
        break ;
      }
    } /* For inner loop */
  } /* for ( count < RFM_CDMA_CARRIER_NUM ) */

  if ( num_channels > 0 )
  {
    for ( count = 0 ; count < RFM_CDMA_CARRIER_NUM ; count++ )
    {
      if ( valid_index[count] == FALSE )
      {  
        memset ( &(carrier_info[count]) , 0 , 
                     sizeof(rf_hdr_carrier_characteristic_type) ) ;
      }
    }
  }
  else
  {
    RF_MSG_6 ( RF_ERROR , "rf_hdr_override_channels(): " 
              "No override match found. Bad channels provided." 
              "Override Channels [%d][%d][%d] | "
              "Premissible Channels [%d][%d][%d] " ,
              override_channels[0] , override_channels[1] , override_channels[2] ,  
              carrier_info[0].channel , carrier_info[1].channel , 
              carrier_info[2].channel ) ;
  }

  /* Atleast one channel should match or there is an error */
  return (num_channels >= 1) ;
}

boolean
rf_hdr_construct_carrier_pattern (  rf_cdma_data_status_type* dev_status_w , 
                                    const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
                                    rf_hdr_carrier_characteristic_type* carrier_info ,
                                    uint16* carrier_config_pattern_bmask ,
                                    uint16* lowest_freq_chan ,
                                    uint8* max_reverse_link_bw ,
                                    rfm_cdma_chan_type override_channels[] ) 
{
  boolean success = FALSE ;
  uint32 min_carrier_separation_kHz = 0; /* valid seperation between any 2 hdr channels in a given band */
  /* Arrange channels in ascending order of their frequencies */
  rf_hdr_get_highest_lowest_channels ( dev_status_w , carrier_info ) ;
  min_carrier_separation_kHz = rf_hdr_get_min_carrier_separation_kHz( dev_status_w ) ;

  /* Override channels if L1 has provided the override array */
  if ( NULL != override_channels )
  {
    RF_MSG ( RF_HIGH , "rf_hdr_construct_carrier_pattern(): "
             "Overriding CCP" ) ;
    success =
    rf_hdr_override_channels ( carrier_info , override_channels ) ;
  }

  if ( rf_hdr_construct_ccp ( carrier_info , min_carrier_separation_kHz ,
                                 carrier_config_pattern_bmask , lowest_freq_chan , 
                                 max_reverse_link_bw )  == FALSE )
  {
    success = TRUE ;
  }
  else
  {
    RF_MSG_3 ( RF_ERROR , "rf_hdr_construct_carrier_pattern() "
              "CCP creation failed. Perhaps bad channels or "
              "incorrect seperations provided Channels[%d][%d][%d] " ,
              carrier_info[0].channel , carrier_info[1].channel ,
              carrier_info[2].channel ) ;
  }                     
  return success ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns monotonocity of NV HDR MAX power table. 
  
  @details
  Returns monotonocity of NV HDR MAX power table. Mononotonocity was determined by
  both X and Y cooredinates of both MAX power tables per CCP.


  @param hdr_max_power_limits
  Pointer to the structure that contains the information about HDR MAX power
  as read from the NV.  
  
  @return
  Returns 0 if LUT_MONOTONIC 
  Bit 0 is set to indicate HI_LUT_X_VALUES_NOT_ASCENDING
  Bit 1 is set to indicate HI_LUT_Y_VALUES_NOT_DESCENDING  
  Bit 2 is set to indicate LO_LUT_X_VALUES_NOT_ASCENDING 
  Bit 3 is set to indicate LO_LUT_Y_VALUES_NOT_DESCENDING 
*/
lut_monotonicity_t
rf_hdr_check_max_pwr_table_monotonocity (  
    const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits ) 
{
  return hdr_max_power_limits->table_monotonicity ;
}
/*----------------------------------------------------------------------------*/

boolean
rf_hdr_check_max_neq_pwr_gt_max_eq_pwr (  
    const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits ) 
{
  return hdr_max_power_limits->is_max_eq_pwr_greater_than_max_neq_value ;
}

/*!
  @brief
  Checks if enhanced PA backoff should be used. 
  
  @details
  Checks if enhanced PA backoff should be used. Non-monotonic NV LUTS, bad
  funnel bias and wild card carrier config patterns will cause the legacy
  HDR max power to be used and the enhanced PA back off will be disabled.

  @param hdr_max_power_limits
  Pointer to the structure that contains the information about HDR MAX power
  as read from the NV.  
  
  @param plim_data
  plim_data pointer for a given device.
 
  @return
  Returns 1 if enhanced PA backoff is enabled
  0 otherwise
*/
boolean
rf_hdr_check_for_enhanced_pa_backoff ( 
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
  rf_cdma_plim_data_type* plim_data ) 
{
  boolean enhanced_pa_backoff = FALSE ;
  if ( ( RFC_HDR_RL_BW_ANY == hdr_pwr_data->bw ) &&
        ( RFC_HDR_CCP_ANY == hdr_pwr_data->carrier_config_pattern ) )
  {
    rf_hdr_plim_set_algo_used ( plim_data , RF_HDR_USE_LEGACY_HDR_MAX_POWER ) ;
    enhanced_pa_backoff = FALSE ; 
    RF_MSG ( RF_LOW , 
             "rf_hdr_check_for_enhanced_pa_backoff(): " 
             "RF_HDR_USE_LEGACY_HDR_MAX_POWER, rl bw and CCP are wildcards " ) ;
  }
  else if ( LUT_MONOTONIC != 
            rf_hdr_check_max_pwr_table_monotonocity ( hdr_pwr_data ) )
  {
    rf_hdr_plim_set_algo_used ( plim_data , RF_HDR_USE_LEGACY_HDR_MAX_POWER ) ;
    enhanced_pa_backoff = FALSE ; 
    RF_MSG_1 ( RF_LOW , 
               "rf_hdr_check_for_enhanced_pa_backoff(): " 
               "MonoValue = 0x%X "
               "using RF_HDR_USE_LEGACY_HDR_MAX_POWER, LUT is non monotonic " , 
               (rf_hdr_check_max_pwr_table_monotonocity( hdr_pwr_data )) ) ;
  }
  else if ( FALSE == 
            rf_hdr_check_max_neq_pwr_gt_max_eq_pwr ( hdr_pwr_data ) )
  {
    rf_hdr_plim_set_algo_used ( plim_data , RF_HDR_USE_LEGACY_HDR_MAX_POWER ) ;
    enhanced_pa_backoff = FALSE ; 
    RF_MSG ( RF_LOW , 
                 "rf_hdr_check_for_enhanced_pa_backoff(): " 
                 "Max neq for hi or lo LUT is less than = max eq power " 
                 "using RF_HDR_USE_LEGACY_HDR_MAX_POWER " ) ; 
  }
  else
  {
    rf_hdr_plim_set_algo_used ( plim_data , RF_HDR_USE_ENHANCED_HDR_MAX_POWER ) ;
    enhanced_pa_backoff = TRUE ; 

    RF_MSG ( RF_LOW , 
             "rf_hdr_check_for_enhanced_pa_backoff():" 
             "RF_HDR_USE_ENHANCED_HDR_MAX_POWER " ) ;
  }
  return enhanced_pa_backoff ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Sorts the LUT indicator in order of ascending frequency.
  
  @details
  Sorts the LUT indicator in order of ascending frequency. L1 talks to RF and
  FW in terms of carrier-indices, for example carriers [0, 1, 2] in a 3
  carrier case may have channels [400 , 375 , 425]. The frequencies of the
  channels are not in any particular order. The NV lookups for the MAX power
  are arragned in order of ascending frequencies. Since return the information
  to L1 and FW in terms of indcies. We need to rearrange the NV items to
  match the indices. This funtion pairs the NV item to the correct index.


  @param carrier_info[]
  For every device carrier information is organized in an ascending frequency
  order. 

  @param hdr_pwr_data
  Pointer to the structure that contains the information about HDR MAX power
  as read from the NV.

  @param plim_data
  plim_data pointer for a given device.

  @return void
*/
void
sort_lut_indicator_in_carrier_freq_order ( 
	rf_cdma_data_status_type* dev_status_w ,
    rf_hdr_carrier_characteristic_type carrier_info[] , 
    const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
    rf_cdma_plim_data_type* plim_data )
{
  uint8 num_carriers = dev_status_w->num_carriers;
  uint8 count;
  boolean unsorted_LUT_indicator_bits[] = 
      { FALSE , FALSE , FALSE } ;
  boolean* sorted_LUT_indicator = 
  &(plim_data->max_power_limits.max_power_info.freq_sorted_lut_indicator[0]) ;

  sorted_LUT_indicator[0]=0;
  sorted_LUT_indicator[1]=0;
  sorted_LUT_indicator[2]=0;

  unsorted_LUT_indicator_bits[0] = 
    (hdr_pwr_data->LUT_indicator & 0x1) ? TRUE : FALSE ;
  unsorted_LUT_indicator_bits[1] = 
    (hdr_pwr_data->LUT_indicator & 0x2) ? TRUE : FALSE ;
  unsorted_LUT_indicator_bits[2] = 
    (hdr_pwr_data->LUT_indicator & 0x4) ? TRUE : FALSE ;


  if ( num_carriers < RFM_CDMA_CARRIER_NUM )
  {
    for ( count = 0 ; count < num_carriers ; count++ )
    {
      sorted_LUT_indicator[carrier_info[count].index] = 
       unsorted_LUT_indicator_bits[--num_carriers] ;

      RF_MSG_4( RF_LOW , "sort_lut_indicator_in_carrier_freq_order(): "
      "LutIndicator[%d]= %d |chan=%d| unsort_lut_index:%d ",
      carrier_info[count].index,sorted_LUT_indicator[carrier_info[count].index],
      carrier_info[count].channel,num_carriers);
    }
  }
}

/*!
  @brief
  Compensates the power for temperature.
  
  @details
  The max power for the phone is charecterised over 8 temperature ranges. When
  we find the correct CCP. We then need to ascertain the correct bin. Once the
  correct bin is known we have to correct the max power relative to the
  reference temperature at which all the readings were taken.
  
  @param plim_data
  CDMA power liminting structure that holds power limiting values for a given device.

  @return difference_in_power_dB10
  difference between the reference power and power at the current temperature
  in_power_dB10
*/
int16
get_temperature_adjusted_power_difference ( 
  const rf_cdma_plim_data_type* plim_data ) 
{
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data = 
      plim_data->max_power_limits.max_power_info.hdr_max_power ;
  int16 difference_in_power_dB10 = 0 , ref_temp_max_pwr_dBm10 = 0 , 
  curr_temp_max_pwr_dBm10 = plim_data->max_power_limits.max_power_info.max_hdr_ccp_pwr_dB10 ;
  const int16* max_power_array = &(hdr_pwr_data->tx_pwr_limit_dBm10[0]) ;
  uint16 ref_mdsp_temp = hdr_pwr_data->mdsp_reading_at_reference_temperature ;

  const rf_cdma_therm_properties_type* therm_cal_info =
            plim_data->max_power_limits.max_power_info.therm_cal_info ;

  if ( TRUE == therm_cal_info->is_therm_cal_nv_data_valid )
  {
    const uint16* therm_cal = &(therm_cal_info->tx_therm_cal[0]);
    ref_temp_max_pwr_dBm10 = 
      rf_lininterp_u16_s16 ( 
              therm_cal , max_power_array  , 
              RF_CDMA_NV_TEMP_LIMIT_SIZE , ref_mdsp_temp ) ;   
  
    /* Add the result to hdet correction to get the right power at a given temp..
       Negative err implies, actual tx power is greater than desired power.
       If tx err is positive, actual tx power is lower than desired power.
       So:
       difference_in_power_dB10:     +ve            -ve
       Final Power:            :   Increase (+)    Decrease (-)
       Operation with hdet     :   Add to hdet the result of this operation
    **/
    difference_in_power_dB10 = curr_temp_max_pwr_dBm10 - ref_temp_max_pwr_dBm10 ;
  }
  else
  {
    RF_MSG ( RF_LOW , "is_therm_cal_valid is FALSE" ) ;
  }
  RF_MSG_3 ( RF_LOW , 
             "In dB10 CurrTemp tx Max-> %d | RefTemp tx Max-> %d | "
             "Diff [Pc - Pr]-> %d | : " ,
              curr_temp_max_pwr_dBm10 ,
              ref_temp_max_pwr_dBm10 , 
              difference_in_power_dB10 ) ;

  return difference_in_power_dB10 ;
}

void 
rf_hdr_print_max_power_values ( 
  const rf_cdma_tx_pwr_limit_data_type* p_hdr_data ) ;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Prints the HDR NV MAX power for a given CCP.
 
  @note
  plim_data->max_power_limits.max_power_info.hdr_max_power gets
  a NULL value or pointer to NV items and is usable only and ony
  after this call
 */
const rf_cdma_tx_pwr_limit_data_type* 
rf_hdr_match_bit_pattern_get_data 
( 
  rf_cdma_data_status_type* dev_status_w , 
  uint16 bit_pattern , 
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
  rf_cdma_plim_data_type* plim_data , 
  rfm_cdma_chan_type override_channels[] 
) 
{
  uint8 rl_bw_in_1_25M_quanta ;
  uint16 channel_count = 0 ; 
  const rf_cdma_tx_pwr_limit_data_type* matched_element_with_hdr_pwr_limits = NULL ;
  uint16 lowest_freq_chan = 0 ;
  rf_hdr_carrier_characteristic_type 
      carrier_info[RFM_CDMA_CARRIER_NUM] = { {0} , {0} , {0} } ;
  
  uint16 bit_pattern_of_carrier_seperation ;
  plim_data->max_power_limits.max_power_info.hdr_max_power = NULL ;

  if ( dev_status_w->num_carriers > RFM_CDMA_CARRIER_NUM )
  {
    RF_MSG ( RF_ERROR , "rf_hdr_match_bit_pattern_get_data() returned FALSE " ); 
  }
  else if ( rf_hdr_construct_carrier_pattern ( dev_status_w , hdr_pwr_data , 
                             carrier_info , &bit_pattern_of_carrier_seperation ,
                             &lowest_freq_chan , &rl_bw_in_1_25M_quanta ,
                             override_channels ) == FALSE )
  {
    RF_MSG_2 ( RF_ERROR , "rf_hdr_match_bit_pattern_get_data():"
                          " num_carriers[%d] > RFM_CDMA_CARRIER_NUM[%d] " ,
                          dev_status_w->num_carriers , RFM_CDMA_CARRIER_NUM ) ; 
  }
  else
  {
    for ( channel_count = 0 ; 
        channel_count < hdr_pwr_data[channel_count].number_of_elements ; channel_count++ )
    {   
      /* BW == 0, means invalid data */
      if ( hdr_pwr_data[channel_count].bw != 0 )
      {        
          uint16 carrier_config_pattern = hdr_pwr_data[channel_count].carrier_config_pattern ;
          uint8 bw = hdr_pwr_data[channel_count].bw ; 
          uint16 hdr_lowest_freq_chan  = hdr_pwr_data[channel_count].lowest_freq_chan ;
          uint16 hdr_highest_freq_chan = hdr_pwr_data[channel_count].highest_freq_chan ;
          
        /* Pattern match */  
        if ( (RFC_HDR_CCP_ANY == carrier_config_pattern ) || 
             (bit_pattern_of_carrier_seperation == carrier_config_pattern) )  
        {
           /* AND RL BW match */
           if ( (RFC_HDR_RL_BW_ANY == bw ) || 
                (rl_bw_in_1_25M_quanta == bw ) ) 
           {
             /* AND channel match */   
             if (   ( (RFC_HDR_LF_CHAN_ANY == hdr_lowest_freq_chan) &&  
                     (RFC_HDR_LF_CHAN_ANY == hdr_highest_freq_chan) )
                || ( (lowest_freq_chan >= hdr_lowest_freq_chan) &&
                       (lowest_freq_chan <= hdr_highest_freq_chan) )  )
             {
              rf_hdr_print_max_power_values ( &(hdr_pwr_data[channel_count]) ) ;
              matched_element_with_hdr_pwr_limits = &(hdr_pwr_data[channel_count]) ;
    
              /* Check for the enhanced PA Backoff algo */
              /* Unconditionally populate this structure 
                 if HDR max power NV items exist for a given CCP */
              plim_data->max_power_limits.max_power_info.hdr_max_power = 
                matched_element_with_hdr_pwr_limits ;
              if ( TRUE == 
                   rf_hdr_check_for_enhanced_pa_backoff ( 
                     &(hdr_pwr_data[channel_count]) , plim_data ) )
              {
                sort_lut_indicator_in_carrier_freq_order ( 
             dev_status_w ,
                   carrier_info , 
                   &(hdr_pwr_data[channel_count]) , 
                   plim_data ) ;
    
                rf_hdr_update_funnel_info ( 
                   dev_status_w ,  
                   carrier_info ,
                   &(hdr_pwr_data[channel_count]) ,
                   plim_data ) ;  
              }
              else
              {
                RF_MSG ( RF_LOW , "rf_hdr_match_bit_pattern_get_data:" 
                           "rf_hdr_check_for_enhanced_pa_backoff() returned false " ) ;    
              }
              break ;
            } /* channel match */  
          } /* RL BW match */ 
        } /* Bit patter match*/
      } /* if ( hdr_pwr_data[channel_count].bw != 0 ) */
    } /* FOR */
  } /* num_carriers <= NUM_CUMULATIVE_CARRIERS */
  return matched_element_with_hdr_pwr_limits ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get the current CCP that was earlier retrieved from NV.
 
  @note
  This is the only and only function that must update
  max_hdr_ccp_pwr_dB10. Do not update this variable outside this
  function.
*/ 
void
rf_hdr_update_ccp_max_power ( rf_cdma_data_status_type* dev_status_w , 
                              rf_cdma_plim_data_type* plim_data ) 
{
  rf_cdma_enhanced_power_limiting_type* max_pwr_info = 
               &(plim_data->max_power_limits.max_power_info) ;
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data = max_pwr_info->hdr_max_power ;
  const rf_cdma_therm_properties_type* therm_cal_info = max_pwr_info->therm_cal_info ;
  const int16* max_power_array = &(hdr_pwr_data->tx_pwr_limit_dBm10[0]) ;

  /* Default to SAR state in case there is an error */
  max_pwr_info->max_hdr_ccp_pwr_dB10 = plim_data->max_power_limits.max_power_limit_sar_dbm10 ;
  if ( NULL != therm_cal_info )
  {
    if ( TRUE == therm_cal_info->is_therm_cal_nv_data_valid )
    {
      const uint16* therm_cal = &(therm_cal_info->tx_therm_cal[0]);
      uint16 current_mdsp_temp_reading =
         dev_status_w->temp_comp.last_temp_sensor_read ;

      if ( max_pwr_info->debug_hdr_max_power ) 
      {
        int count = 0 ;
        RF_MSG_1 ( RF_LOW , "Temp MDSP Reading: %d ", current_mdsp_temp_reading ) ; 
        for ( ; count < RF_CDMA_NV_TEMP_LIMIT_SIZE ; count ++ ) {
          RF_MSG_3  ( RF_LOW , "[C%d:] ccp_max_pwr -> %d dB10 | ThermCal -> %d | " , 
                      count , max_power_array[count] ,  therm_cal[count] ) ;
       }
      }
     max_pwr_info->max_hdr_ccp_pwr_dB10 = 
        rf_lininterp_u16_s16 ( therm_cal , max_power_array  , 
                RF_CDMA_NV_TEMP_LIMIT_SIZE , current_mdsp_temp_reading ) ;  
    }
    else
    {
      RF_MSG ( RF_ERROR , "rf_hdr_update_ccp_max_power(): "
                 "is_therm_cal_nv_data_valid returned FALSE, "
                 "setting CCP power to SAR power. " ) ;
    }
    RF_MSG_1 ( RF_LOW , "rf_hdr_update_ccp_max_power(): " 
               "Calculated CCP Power is: %d dBm10" ,
               max_pwr_info->max_hdr_ccp_pwr_dB10 ) ;
  }
  else
  {
    RF_MSG ( RF_ERROR , "rf_hdr_update_ccp_max_power(): " 
             "therm_cal_info is NULL" ) ;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get the check if CCP can be found based on
  earlier retrieved HDR max power NV. If a matching CCP is not
  returned, then set the HDR plim algorithm to use the 1X MAX
  power
*/ 
boolean
rf_hdr_get_max_power_records ( const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
                              rf_cdma_data_status_type* dev_status_w ,
                              rfm_device_enum_type device ,
                              const rf_hdr_config_type* rf_hdr_config_ptr ,
                              rf_cdma_plim_data_type* plim_data ,
                              rfm_cdma_chan_type override_channels[] )
{
  uint16 carrier_bit_position = 0 ;
  boolean success = FALSE ;

  /* Individual record after the pattern is found */
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data_record =
      rf_hdr_match_bit_pattern_get_data ( dev_status_w , carrier_bit_position , 
                                          hdr_pwr_data , plim_data , 
                                          override_channels ) ; 

  if ( NULL != hdr_pwr_data_record ) 
  {
    rf_hdr_update_ccp_max_power ( dev_status_w , plim_data ) ;
    success = TRUE ;
  }
  else /*  if ( hdr_pwr_data_record == NULL ) */
  {
    /* No pattern match */ 
    RF_MSG ( RF_HIGH , " rf_hdr_get_max_power_records(): No pattern found " ) ;

    rf_hdr_plim_set_algo_used ( plim_data , RF_HDR_USE_1X_MAX_POWER ) ;
    RF_MSG ( RF_HIGH , 
             "rf_hdr_get_max_power_records(): HDR MAX PWR algo is: RF_HDR_USE_1X_MAX_POWER" ) ;
  }
  return success ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get the common 1x and HDR NV items when HDR-CCP NVs are 
  not populated for the band the phone is in.
*/ 
boolean
rf_hdr_get_common_pwr_values ( const rfc_cdma_logical_device_params_type* logical_dev ,
                                rf_cdma_plim_data_type* plim_data ,
                                int16 max_power[] )
{
  boolean success = FALSE ;
  const rf_1x_config_type* rf_1x_config_data_ptr = 
      rf_cdma_nv_get_1x_data( logical_dev->rx_nv_path[plim_data->band] , 
                              plim_data->band);
  if ( NULL != rf_1x_config_data_ptr )
  {
    int count = 0 ;
    size_t sar_state = rfcommon_core_sar_get_state() ;
    const rf_cdma_config_type* rf_cdma_config_data_ptr = 
                        rf_1x_config_data_ptr->cdma_common_ptr ;
    success = TRUE ;

    if ( sar_state >= RF_SAR_STATE_MAX )
    {
      /* Invalid SAR state, revert to a default 1 */
      RF_MSG_1 ( RF_HIGH ,  "rf_hdr_get_common_pwr_values(): "
                 "Invalid SAR state: %d. Revrting to default SAR state " ,
                  sar_state ) ; 
      sar_state = RF_SAR_STATE_DEFAULT ;
    }

    for ( count = 0 ; count < RF_CDMA_NV_TEMP_LIMIT_SIZE ; count ++ )    
    {
      max_power[count] =
      rf_cdma_config_data_ptr->tx_static.lim_vs_temp[sar_state][count] ;
    }
  }
  else
  {
    RF_MSG ( RF_ERROR , "rf_hdr_get_common_pwr_values(): " 
             "rf_1x_config_data_ptr is NULL, SAR will NOT work" ) ;
  }
  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get the HDR max channel records in the NV
  if present and if the NV is not populated, use the common 1x
  and HDR power for the band the phone is in.

*/
boolean
rf_hdr_calculate_max_tx_power_limit 
( 
  rfm_device_enum_type device , 
  rf_cdma_plim_data_type* plim_data,
  const rfc_cdma_logical_device_params_type* logical_dev ,
  int16 sar_max_power[] ,
  rfm_cdma_chan_type override_channels[] 
) 
{
  boolean success = FALSE ;
  /* Get device */
  rf_cdma_data_status_type* dev_status_w = 
    rf_cdma_get_mutable_device_status ( device ) ;

  /* NULL pointer check*/
  if(  dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR,"rf_hdr_calculate_max_tx_power_limit: NULL data for" 
                "device %d" ,device );
  }/* if(  dev_status_w == NULL ) */
  else
  { /* If CCP is overriden by L1 dont bother getting a 
      new CCP until L1 override is removed */
    if ( FALSE == rf_hdr_plim_get_ccp_override(plim_data) )
    {
      const rf_hdr_config_type* rf_hdr_config_ptr =
          rf_cdma_nv_get_hdr_data ( 
                             logical_dev->tx_nv_path[dev_status_w->curr_band] , 
                             dev_status_w->curr_band ) ;
    
      if ( NULL != rf_hdr_config_ptr )
      {
        /* Get power levels populated from NV during rfm_init() */
        const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data = 
              rf_hdr_get_max_pwr_nv_addr ( rf_hdr_config_ptr , dev_status_w ) ;

        /* If active NV exists and pattern matching criterion are met, apply HDR limits */
        if ( NULL != hdr_pwr_data )
        {
          success = 
            rf_hdr_get_max_power_records ( hdr_pwr_data , dev_status_w , device , 
                                             rf_hdr_config_ptr , plim_data , override_channels ) ; 
          if ( FALSE == success )
          {
            RF_MSG ( RF_ERROR , "rf_hdr_calculate_max_tx_power_limit(): " 
                     "rf_hdr_get_max_power_records() failed to find a matching CCP" ) ; 
          }
        }
      
        /* if Inactive NV, use common\1x power limits otherwise just 
            populate the SAR limits */
        if ( rf_hdr_get_common_pwr_values ( logical_dev , 
                                         plim_data , sar_max_power )== TRUE  )
        {
          success = TRUE ;
        }
        else
        {
          success = FALSE ;
          RF_MSG ( RF_ERROR ,  "rf_hdr_calculate_max_tx_power_limit(): "
                   "rf_hdr_get_common_pwr_values() returned FAILURE" ) ;
        }
      }
    }  /* If CCP max power is overridden */
  }/* if!(  dev_status_w == NULL ) */
  return success ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to get the highest interpolated max power value where: 
  max_power = f(temp, NV max power values) for a given HDR max channel record in
  the NV for the band the phone is in.
  
*/
int16
rf_hdr_get_interpolated_max_value_for_band ( 
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data ,
  const rf_cdma_config_type* rf_cdma_config_data_ptr ,
  uint32 hkadc_temperature_reading )
{
  uint32 tx_therm_cal[RF_CDMA_NV_TEMP_LIMIT_SIZE] = { 0 } , count = 0  ;
  int16 max_power_readings[RF_CDMA_NV_TEMP_LIMIT_SIZE] = { 0 }  , 
        max_power_in_band = 234 ;


  for ( count = 0 ; count < RF_CDMA_NV_TEMP_LIMIT_SIZE ; count++ )
  {
    /*copy NV to local variable from unit16 to uint32 */
    max_power_readings[count] = 
        hdr_pwr_data->tx_pwr_limit_dBm10[count] ;
    max_power_in_band = MAX ( max_power_readings[count] , max_power_in_band ) ; 
  }
  
  if ( NULL != rf_cdma_config_data_ptr )
  {
    for ( count = 0 ; count < RF_CDMA_NV_TEMP_LIMIT_SIZE ; count++ )
    {
      /*copy NV to local variable from unit16 to uint32 */
      tx_therm_cal[count] = 
          (uint32) rf_cdma_config_data_ptr->tx_cal.therm_cal_data.tx_therm_cal[count] ;
    }
    
    max_power_in_band = rf_lininterp_u32_s16( tx_therm_cal,
                                              max_power_readings ,
                                              RF_CDMA_NV_TEMP_LIMIT_SIZE,
                                              hkadc_temperature_reading );
  }
  else
  {
    RF_MSG ( RF_ERROR , 
        "rf_hdr_get_interpolated_max_value_for_band() has failed because rf_cdma_config_data_ptr is NULL " ) ;
  }
  return max_power_in_band ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Function to calculate the highest max power limit from from all the HDR NV 
  MAX power items for the band in which the phone currently is in
  
*/
boolean
rf_hdr_get_max_tx_power_for_band ( 
  rfm_device_enum_type device , 
  int16* max_power_limit_in_band_dB10 ) 
{
  boolean success = FALSE ;
  rf_cdma_data_status_type* dev_status_w;
  const rfc_cdma_logical_device_params_type*  logical_dev;
  const rf_1x_config_type* rf_1x_config_data_ptr; 
  const rf_hdr_config_type* rf_hdr_config_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;
  const rf_cdma_tx_pwr_limit_data_type* hdr_pwr_data;
  uint32 temperature_reading;
  int16 max_power_in_band_dBm10;
  int16 previous_max_power_in_dBm10;
  int channel_count;
  int max_channel_records; 

  /* Get device */
  dev_status_w = rf_cdma_get_mutable_device_status ( device ) ;
  /* KW fix */
  if ( dev_status_w == NULL)
  {

    RF_MSG_1( RF_ERROR, "rf_hdr_get_max_tx_power_for_band: "
                        "Invalid Device %d",device );
        return success;
  }

  logical_dev = rfc_cdma_get_logical_device_params( device );
  /* KW fix   */
  if (logical_dev == NULL)
  {
      RF_MSG_1( RF_ERROR, "rf_hdr_get_max_tx_power_for_band: "
                          "Invalid Logical Device %d",device );
        return success;
  }
  rf_hdr_config_ptr = rf_cdma_nv_get_hdr_data ( 
                              logical_dev->tx_nv_path[dev_status_w->curr_band] , 
                              dev_status_w->curr_band ) ;

  rf_1x_config_data_ptr = rf_cdma_nv_get_1x_data(
                              logical_dev->rx_nv_path[dev_status_w->curr_band], 
                              dev_status_w->curr_band ) ;
  
  /* KW fix   */
  if (rf_1x_config_data_ptr == NULL)
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_get_max_tx_power_for_band: "
                       "NULL ptr returned for 1x NV data %d",device );
    return success;
  }
  rf_cdma_config_data_ptr = rf_1x_config_data_ptr->cdma_common_ptr ;
    
  temperature_reading = 0 ;
  /* If last temp comp read failed, skip temp comp by setting temp_comp_offset
  to zero; otherwise, do interpolation */
  if ( dev_status_w->temp_comp.is_last_reading_valid == TRUE )
  {
    temperature_reading = 
      (uint32)dev_status_w->temp_comp.last_temp_sensor_read ;
  }
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "rf_hdr_get_max_tx_power_for_band(): "
            "rf_cdma_config_data_ptr is NULL " ); 
  }
    

  if ( NULL != rf_hdr_config_ptr ) 
  {
    /* Get power levels populated from NV during rfm_init() */
    hdr_pwr_data = rf_hdr_get_max_pwr_nv_addr ( rf_hdr_config_ptr , 
                                           dev_status_w ) ;
    max_power_in_band_dBm10 = 0;
    previous_max_power_in_dBm10 = 0;

    /* If active NV exists and pattern matching criterion are met, apply HDR limits */
    if ( hdr_pwr_data != NULL )
    {
      channel_count = 0 ;
      max_channel_records = hdr_pwr_data[channel_count].number_of_elements ; 
      for ( ; channel_count < max_channel_records ; channel_count++ )
      { 
        max_power_in_band_dBm10 = 
           rf_hdr_get_interpolated_max_value_for_band ( &(hdr_pwr_data[channel_count]) , 
                                                           rf_cdma_config_data_ptr , 
                                                           temperature_reading ) ;   
        previous_max_power_in_dBm10 = MAX ( previous_max_power_in_dBm10 ,
                                              max_power_in_band_dBm10 ) ;
      }
      *max_power_limit_in_band_dB10 = previous_max_power_in_dBm10 ;
      success = TRUE ;
    }
    else
    {
      RF_MSG ( RF_ERROR , 
          "rf_hdr_get_max_tx_power_for_band() failed "
          "because HDR MAX power NV items were not found" ) ; 
    }
  }
  else
  {
    RF_MSG ( RF_ERROR ,
        "rf_hdr_get_max_tx_power_for_band() failed "
        "because CDMA NV pointer is NULL" ) ; 
  }
  return success ;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to calculate the lowest max power limit from all the
  contributing modules
*/
int16
rf_hdr_plim_get_lowest_max_power_limit(rf_cdma_max_power_limits_type* max_power_limits)
{
  int16 lowest_max_power;

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

  /* CCP Power */
  lowest_max_power = MIN ( lowest_max_power , 
                           max_power_limits->max_power_info.max_hdr_ccp_pwr_dB10 ) ;

  if ( max_power_limits->max_power_info.debug_hdr_max_power == TRUE )
  {
   
    RF_MSG_5 ( RF_HIGH , "rf_hdr_plim_get_lowest_max_power_limit(): " 
               "In dBm10: || t_v_f: %d | sar: %d | sv: %d | thermal: %d | CCP: %d || " ,
               max_power_limits->max_power_limit_vs_temp_vs_freq_dbm10,
               max_power_limits->max_power_limit_sar_dbm10 , 
               max_power_limits->max_power_limit_sv_dbm10 , 
               max_power_limits->max_power_limit_thermal_backoff_dbm10 , 
               max_power_limits->max_power_info.max_hdr_ccp_pwr_dB10
               ) ;
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
rf_hdr_plim_update_temp
(
  rfm_device_enum_type device
)
{

  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  
  const rf_1x_config_type* rf_1x_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;
  boolean plim_update_status = FALSE;

  uint32 tx_therm_cal[RF_CDMA_NV_TEMP_LIMIT_SIZE];
  int16  sar_max_pwr_limit[RF_CDMA_NV_TEMP_LIMIT_SIZE];

  uint8 i;
  boolean hdr_nv_read_success = FALSE ;
  const rf_cdma_therm_properties_type* therm_cal_infomation ;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );
  
  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );
  
  if ( (dev_status_w == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_plim_update_temp: Invalid device: "
                        "%d", device );
    return FALSE;
  }
  
  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;
  
  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_plim_update_temp: Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }
  
  rf_1x_config_data_ptr = rf_cdma_nv_get_1x_data(
                                      logical_dev->rx_nv_path[plim_data->band],
                                      plim_data->band);

  if ( rf_1x_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "rf_hdr_plim_update_temp" );
    return FALSE;
  }
  
  rf_cdma_config_data_ptr = rf_1x_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "rf_hdr_plim_update_temp" );
    return FALSE;
  }

  therm_cal_infomation = &(rf_cdma_config_data_ptr->tx_cal.therm_cal_data) ;
  plim_data->max_power_limits.max_power_info.therm_cal_info =
        therm_cal_infomation ;

  hdr_nv_read_success = 
      rf_hdr_calculate_max_tx_power_limit ( device , plim_data ,logical_dev , 
                                            sar_max_pwr_limit , NULL ) ;

  if ( FALSE == hdr_nv_read_success )
  {
    RF_MSG ( RF_ERROR, 
            "rf_hdr_plim_update_temp(): rf_hdr_calculate_max_tx_power_limit() has failed" ) ;
  }

  if ( therm_cal_infomation->is_therm_cal_nv_data_valid == TRUE )
  {

  /*copy NV to local variable from unit16 to uint32 */
  for ( i = 0; i < RF_CDMA_NV_TEMP_LIMIT_SIZE; i++ )
  {
      tx_therm_cal[i] = 
      (uint32) rf_cdma_config_data_ptr->tx_cal.therm_cal_data.tx_therm_cal[i];
  }

   /* If last temp comp read failed, skip temp comp by setting temp_comp_offset
  to zero; otherwise, do interpolation */
  if ( dev_status_w->temp_comp.is_last_reading_valid == TRUE )
  {
    plim_data->max_power_limits.max_power_limit_sar_dbm10
      = rf_lininterp_u32_s16( tx_therm_cal,
                                  sar_max_pwr_limit ,
                                  RF_CDMA_NV_TEMP_LIMIT_SIZE ,
                                  (uint32)dev_status_w->temp_comp.last_temp_sensor_read ) ;
    plim_update_status = TRUE;
  }
  else
  {
    plim_update_status = FALSE;
  }
  }
  else
  {
    RF_MSG ( RF_ERROR , "rf_hdr_plim_update_temp(): "
             "is_therm_cal_nv_data_valid returned FALSE ") ;
    plim_update_status = FALSE ;
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
boolean
rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr
(
  rf_cdma_mdsp_chain_enum_type mdsp_chain ,
  const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits ,
  int16 k10 ,
  const rf_cdma_mdsp_cb_handler_type callback ,
  void* const userdata
)
{
  uint16 table_idx = 0; /* Table Index to SMEM */
  hdrfw_pa_backoff_lut_tables_t* max_pwr_data ; /* Max Pwr Data */
  rf_hdr_mdsp_smem_data_type* smem_data; /* Ptr to SMEM data */
  boolean api_status = TRUE; /* used to see if msgr call was successful*/
  rf_cdma_mdsp_cb_event_type cb_event = RF_CDMA_MDSP_EXECUTION_FAILURE;
    
  /* check for valid chain */
  if ( mdsp_chain >= HDRFW_NUM_ANTENNAS )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr(): Invalid chain %d", 
              mdsp_chain);
    api_status &= FALSE;
  } /* if ( mdsp_chain >= HDRFW_NUM_ANTENNAS ) */

  /* Get SMEM Data */
  smem_data = rf_hdr_mdsp_get_validated_smem_data(&api_status);
  /* Continue only if all checks passed */
  if ( api_status == TRUE )
  {
    /* Get Pointer to Tx Linearizer Data */
    boolean debug = TRUE ;
    max_pwr_data = &(smem_data->smem_ptr->write.paBackoffLUTTables);
  
    /* Loop through all Entries for PA State 0 */
    for ( table_idx = 0; table_idx < MAX_HDR_PWR_LIMIT_COORDINATES; table_idx++ )
    {
      int16 pwr_dbm10 = 0 , entries , 
        *addr = NULL ; /* SMEM addr that holds the TxAGC in mdsp units */
      for ( entries = 0 ; entries < 4 ; entries++ )
      {
        switch ( entries )
        {
        /* x LO */
        case 0 :
          pwr_dbm10 = 
              hdr_max_power_limits->lo_tx_imbalanced_carrier_pwr_lut[table_idx].x_axis_val ;
          addr = &max_pwr_data->LoBackoffLutXY[0][table_idx];
          break ;
          
        /* y LO */
        case 1 :
          pwr_dbm10 = 
              hdr_max_power_limits->lo_tx_imbalanced_carrier_pwr_lut[table_idx].y_axis_val ;
          addr = &max_pwr_data->LoBackoffLutXY[1][table_idx];
          break ;

        /* x HI */
        case 2 :
          pwr_dbm10 = 
              hdr_max_power_limits->hi_tx_imbalanced_carrier_pwr_lut[table_idx].x_axis_val ;
          addr = &max_pwr_data->HiBackoffLutXY[0][table_idx];
          break ;

        /* y HI */
        case 3 :
          pwr_dbm10 = 
              hdr_max_power_limits->hi_tx_imbalanced_carrier_pwr_lut[table_idx].y_axis_val ;
          addr = &max_pwr_data->HiBackoffLutXY[1][table_idx];
          break ;
        } /* switch */
        /* Convert Tx Power to MDSP units and write Max Power to SMEM */
        *addr = 
            rf_cdma_mdsp_dbm10_to_txagc ( pwr_dbm10 , k10 ) ;
      } /* for ( entries < 4 ) */
    } /* for ( table_idx < MAX_HDR_PWR_LIMIT_COORDINATES ) */

    if ( TRUE == debug )
    {
      RF_MSG ( RF_LOW , "SMEM LO LUT ==============================================================" ) ;
      RF_MSG_8 ( RF_LOW , "txAGC-640 X: %d , %d , %d , %d , %d , %d , %d , %d " ,
          max_pwr_data->LoBackoffLutXY[0][0] , max_pwr_data->LoBackoffLutXY[0][1] ,
          max_pwr_data->LoBackoffLutXY[0][2] , max_pwr_data->LoBackoffLutXY[0][3] ,
          max_pwr_data->LoBackoffLutXY[0][4] , max_pwr_data->LoBackoffLutXY[0][5] ,
          max_pwr_data->LoBackoffLutXY[0][6] , max_pwr_data->LoBackoffLutXY[0][7] ) ;
      RF_MSG_8 ( RF_LOW , "txAGC-640 Y: %d , %d , %d , %d , %d , %d , %d , %d " ,
          max_pwr_data->LoBackoffLutXY[1][0] , max_pwr_data->LoBackoffLutXY[1][1] ,
          max_pwr_data->LoBackoffLutXY[1][2] , max_pwr_data->LoBackoffLutXY[1][3] ,
          max_pwr_data->LoBackoffLutXY[1][4] , max_pwr_data->LoBackoffLutXY[1][5] ,
          max_pwr_data->LoBackoffLutXY[1][6] , max_pwr_data->LoBackoffLutXY[1][7] ) ;

      RF_MSG ( RF_LOW , "SMEM HI LUT ===============================================================" ) ;
      RF_MSG_8 ( RF_LOW , "txAGC-640 X: %d , %d , %d , %d , %d , %d , %d , %d " ,
          max_pwr_data->HiBackoffLutXY[0][0] , max_pwr_data->HiBackoffLutXY[0][1] ,
          max_pwr_data->HiBackoffLutXY[0][2] , max_pwr_data->HiBackoffLutXY[0][3] ,
          max_pwr_data->HiBackoffLutXY[0][4] , max_pwr_data->HiBackoffLutXY[0][5] ,
          max_pwr_data->HiBackoffLutXY[0][6] , max_pwr_data->HiBackoffLutXY[0][7] ) ;
      RF_MSG_8 ( RF_LOW , "txAGC-640 Y: %d , %d , %d , %d , %d , %d , %d , %d " ,
          max_pwr_data->HiBackoffLutXY[1][0] , max_pwr_data->HiBackoffLutXY[1][1] ,
          max_pwr_data->HiBackoffLutXY[1][2] , max_pwr_data->HiBackoffLutXY[1][3] ,
          max_pwr_data->HiBackoffLutXY[1][4] , max_pwr_data->HiBackoffLutXY[1][5] ,
          max_pwr_data->HiBackoffLutXY[1][6] , max_pwr_data->HiBackoffLutXY[1][7] ) ;
    } /* debug */
    RF_MSG ( RF_LOW , "Populated Imbalance table in FW SMEM " ) ;  
    /* Populate cb_event based on the API status */
    cb_event = RF_CDMA_MDSP_EVENT_SUCCESS;
  } /* if ( api_status == TRUE ) */
  else
  {
    RF_MSG( RF_ERROR,"rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr: Validation Failed");
  } /* if ! ( api_status == TRUE ) */

  /* Execute Callback function */
  if ( callback != NULL )
  {
    callback( cb_event, userdata );
  } /* if ( callback != NULL ) */

  /* Send Debug Message */
  RF_MSG_2( RF_LOW, "rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr(): [Status %d] C%d", 
            api_status, mdsp_chain );

  /* return */
  return api_status;

} /* rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr */

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Helper function to clamp funneling max power limits when one
  of the PLIM inputs is lower than the CCP before sending it to
  SMEM
  
  @details
  Every time an update is provided to power limiting module and
  when one of PLIM inputs is lower than the CCP we need to clamp
  funneling max power limits and the table needs to be
  recalculated for funneling update the SMEM with Funneling
  MAX power for firmware.

  @param tx_mdsp_path MDSP path to which final min and max power
  limits should be applied

  @param hdr_max_power_limits
  A structure containing the current CCP HDR power limit
  information

  @param clamped_hdr_max_power_limits Output: A structure
  containing the clamped CCP HDR power limit information

  @param clamping_value_dBm10
  In dBm10 the amount of clamping to be done
 
  @return boolean
  Pass indicated by: TRUE
  Failure indicated by: FALSE
 
*/
boolean
rf_hdr_plim_clamp_funneling_tables ( 
   const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits ,
   rf_cdma_tx_pwr_limit_data_type* clamped_hdr_max_power_limits , 
   int16 clamping_value_dBm10 )
{
  boolean success = FALSE ;
  if ( clamping_value_dBm10 > 0 )
  {
    /* CLAMP and update the return variable */
  }
  return success ;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Helper function to calculate funneling max power limits to
  populate SMEM
  
  @details
  Every time an update is provided to power limiting module, the max power limit
  should be recalculated and for funneling update the SMEM with
  Funneling MAX power to firmware. 

  @param tx_mdsp_path MDSP path to which final min and max power
  limits should be applied

  @param plim_data
  A structure containing the current power limit information

  @param k10
  Turnaround constant x 10. Used to convert powers in dbm10 to tx agc in db640

  @param *max_plim_err_adj_db640
  Output: Max power limit in db640 computed by this function. This is the limit
  sent to firmware
 
  @return rf_hdr_mdsp_max_power_capsule_t Capsule that conatins
  all the required parameters for MAX power update. This is the
  limit sent to firmware
*/
rf_hdr_mdsp_max_power_capsule_t 
rf_hdr_populate_max_power_capsule 
(
  rf_cdma_mdsp_chain_enum_type tx_mdsp_path,
  rf_cdma_plim_data_type* plim_data,
  int16 max_plim_err_adj_db640 ,
  int16 k10 ,
  int16 resultant_max_power_dBm10 
)
{
  rf_hdr_mdsp_max_power_capsule_t max_pwr_capsule = { 0 } ;
  int16 temp_comp_pwr_db640 = 0 , temp_comp_pwr_dB10 = 0 , 
    hdet_comp_pwr_dB10 = 0 , hdet_comp_pwr_db640 = 0 ;

  /* Unconditionally populate the legacy PA backoff params */
  max_pwr_capsule.mdsp_chain = tx_mdsp_path ;
  max_pwr_capsule.min_pwr_db640 = 
      rf_cdma_mdsp_dbm10_to_txagc ( plim_data->min_power_limit_dbm10 , k10 ) ;
  max_pwr_capsule.max_pwr_db640 = max_plim_err_adj_db640 ;
  max_pwr_capsule.callback = NULL ;
  max_pwr_capsule.userdata = NULL ;  
  max_pwr_capsule.is_enhanced_power_limiting_enabled = FALSE ;

  RF_MSG_2 ( RF_MED , "rf_hdr_populate_max_power_capsule(): " 
            "In dbm10: ||Min Pwr: %d|Max Pwr: %d||"  ,
             plim_data->min_power_limit_dbm10 ,
             rf_cdma_convert_txagc ( RFM_CDMA_AGC_FORMAT__DBM10,
                                     rf_cdma_get_turnaround_constant(plim_data->band),
                                     max_pwr_capsule.max_pwr_db640)) ;
  /* Use enhanced PA backoff if NVs are correct and MCDO */
  if ( RF_HDR_USE_ENHANCED_HDR_MAX_POWER ==
       rf_hdr_plim_get_algo_used (plim_data) )
  {
    boolean success ;
    const rf_cdma_tx_pwr_limit_data_type* hdr_max_power_limits = 
        plim_data->max_power_limits.max_power_info.hdr_max_power ;
    const int16 ccp_pwr_dBm10 =
      plim_data->max_power_limits.max_power_info.max_hdr_ccp_pwr_dB10 ;
    rf_cdma_tx_pwr_limit_data_type clamped_hdr_max_power ;

    temp_comp_pwr_dB10 =
      plim_data->max_power_limits.max_power_info.
      temp_adjusted_power_difference_dB10 =
        get_temperature_adjusted_power_difference ( plim_data ) ;

    /* Uncomment this for BOLT 1.0 */
    if ( ( ccp_pwr_dBm10 > resultant_max_power_dBm10 ) && 0 )
    {
      int16 clamping_value_dBm10 = ccp_pwr_dBm10 - resultant_max_power_dBm10 ;
      success = 
        rf_hdr_plim_clamp_funneling_tables ( hdr_max_power_limits , 
                                             &clamped_hdr_max_power  ,
                                             clamping_value_dBm10 ) ;
      /* Replace the tables being send to FW */
      if ( TRUE == success )
      {
        hdr_max_power_limits = &clamped_hdr_max_power ;
      }
      RF_MSG ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
                        "Funneling tables will have to be clamped.") ;
      RF_MSG_2 ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
                 "| In dBm10: CCP Power -> %d | Lowest Max Power -> %d | " ,
              ccp_pwr_dBm10 , 
              resultant_max_power_dBm10 ) ;
    }
    else
    {
      RF_MSG ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
                        "Clamping of Funneling tables disabled " ) ; 
    }

    if ( hdr_max_power_limits != NULL )
    {
      /* Update the tbl pointed and fill smem with new table */
      success =
        rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr ( tx_mdsp_path ,
                                                      hdr_max_power_limits ,
                                                      k10 , NULL , NULL ) ;
      /* If we are not successful, invalidate the hdr pwr limit ptr */
      if ( FALSE == success ) 
      {
        hdr_max_power_limits = NULL ;
        RF_MSG ( RF_ERROR , "rf_hdr_plim_update_limits(): " 
             "rf_hdr_mdsp_fill_smem_with_enhanced_max_pwr() failed" ) ;
      }
      else /* Populate the flags and values*/
      {
        /* Mark this flag TRUE only if 
          A> HDR max power Table is valid and monotonic
          B> SMEM is correctly filled with HDR Max power table */
        max_pwr_capsule.is_enhanced_power_limiting_enabled = TRUE ;
        max_pwr_capsule.freq_sorted_lut_indicator[0]
            = plim_data->max_power_limits.max_power_info.freq_sorted_lut_indicator[0] ;
        max_pwr_capsule.freq_sorted_lut_indicator[1]
            = plim_data->max_power_limits.max_power_info.freq_sorted_lut_indicator[1] ;
        max_pwr_capsule.freq_sorted_lut_indicator[2]
            = plim_data->max_power_limits.max_power_info.freq_sorted_lut_indicator[2] ;
      } /* SMEM filled correctly */
    } /* hdr_max_power_limits != NULL */
    else
    {
      RF_MSG ( RF_ERROR , "rf_hdr_populate_max_power_capsule(): " 
               "HDR Max power table empty " ) ;
    } /*  hdr_max_power_limits != NULL */

    /* Start updating the other Enhanced PA backoff entities */
    max_pwr_capsule.hdr_max_power_limits = hdr_max_power_limits ;

    /* HDET and Temp Calculation Section   */
    hdet_comp_pwr_db640 = plim_data->tx_err_from_hdet_db640 ;
    temp_comp_pwr_db640 = -64 * temp_comp_pwr_dB10 ;
    hdet_comp_pwr_dB10 = hdet_comp_pwr_db640 / -64 ; 

    /*  HDET and Temperature Compensation begin */
    max_pwr_capsule.hdet_temp_correction_db640 = 
            hdet_comp_pwr_db640 + temp_comp_pwr_db640 ;
    /*  HDET and Temperature Compensation end */

    RF_MSG ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
                      "Temp Comp for Enhanced PA Backoff: " 
               "in dB10 terms +ve means increase power, -ve means decrease it  " ) ;
    RF_MSG ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
             "HDR MAX PWR temperature & HDET correction components are: " ) ;
    RF_MSG_3 ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
               "In dB10 PwrDiff: HDET-> %d | TempComp-> %d | Total [A + B]: %d " ,
              hdet_comp_pwr_dB10 , temp_comp_pwr_dB10 , 
              (temp_comp_pwr_dB10 + hdet_comp_pwr_dB10)  ) ;
    RF_MSG_3 ( RF_LOW , "rf_hdr_populate_max_power_capsule(): "
               "In -dBm640 PwrDiff: HDET-> %d | TempComp-> %d | Total [A + B]: %d " ,
              hdet_comp_pwr_db640 , temp_comp_pwr_db640 ,
              max_pwr_capsule.hdet_temp_correction_db640 ) ;

  } /* Is algo RF_HDR_USE_ENHANCED_HDR_MAX_POWER ? */
  else
  {
    RF_MSG ( RF_LOW , 
             "rf_hdr_plim_update_limits(): Enhanced PA back not used" ) ;
  } /* Nope algo is not RF_HDR_USE_ENHANCED_HDR_MAX_POWER. */
  return max_pwr_capsule ;   
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
rf_hdr_plim_update_limits
(
  rf_cdma_mdsp_chain_enum_type tx_mdsp_path,
  rf_cdma_plim_data_type* plim_data,
  int16 k10,
  int16* max_plim_err_adj_db640
)
{
  int16 max_plim_dbm10;
  int32 max_plim_err_adj_db640_unlimited;
  int16 max_lin_limit_db640;
  rf_hdr_mdsp_max_power_capsule_t max_pwr_capsule = { 0 } ;

  /* Obtain the max power limit from all contributing modules */
  max_plim_dbm10 = 
      rf_hdr_plim_get_lowest_max_power_limit( &plim_data->max_power_limits );

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
    RF_MSG_2 (RF_HIGH, "HDR PLIM: Using linearizer based limit %d(dBm10), "
                       "which is lower than desired power limit %d(dBm10)",
              plim_data->max_power_linearizer_limit_dbm10,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(
                                      plim_data->band),
                                    *max_plim_err_adj_db640) );

    *max_plim_err_adj_db640 = max_lin_limit_db640;
  }

  /* Set MAX power LUTs */
  max_pwr_capsule =
    rf_hdr_populate_max_power_capsule ( tx_mdsp_path , plim_data ,
                                        *max_plim_err_adj_db640 , k10 , 
                                        max_plim_dbm10 ) ;
  
  /* Apply new limit to firmware */
  rf_hdr_mdsp_configure_tx_power_limit_db640 ( max_pwr_capsule ) ;
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
rf_hdr_plim_update_temp_mdsp
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
)
{

  int16 k10;

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) )
  {
    /* Only update the limit if the power limiting module is running,
       otherwise it will be updated by default the next time the module
       is started. */

    k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

    /*Update plim_max_pwr according to temp reading*/ 
    if ( rf_hdr_plim_update_temp( device ) == FALSE ) 
    { 
       /* No active temp measurement updates yet, so always use index 0
             directly from the temp comp table. */
       plim_data->max_power_limits.max_power_limit_sar_dbm10
         = rf_cdma_config_data_ptr->tx_static.lim_vs_temp[rfcommon_core_sar_get_state()][0];
    }

    /* Recalculate max power limit taking new TX err from HDET in account.
      Send new limits to MDSP */
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );


  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */

}
/*! @} */

/*============================================================================*/
/*!
  @name HDR Power Limiting APIs

  @brief
  Interface functions from HDR power limiting module to start/stop/update or
  override power limiting.
*/
/*! @{ */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Starts HDR TX power limiting on a given device
  
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
rf_hdr_plim_start
(
  rfm_device_enum_type device,
  const rfc_cdma_logical_device_params_type* logical_dev,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band
)
{
  boolean status = TRUE;
  int16 k10 = 10*rf_cdma_get_turnaround_constant(band);
  const rf_hdr_config_type* rf_hdr_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;

  if ( (plim_data == NULL) || 
       (plim_data->init_done == FALSE) ||
       (plim_data->plim_supported == FALSE) ||
       (logical_dev == NULL) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: Cannot start Power Limiting on device %d, "
              ,device );
    return FALSE;
  }
  if ( rfm_get_calibration_state() == TRUE )
  {
    RF_MSG( RF_LOW, "HDR PLIM: Not starting Power Limiting loop in calibration "
                    "mode" );
    return FALSE;
  }

  rf_hdr_config_data_ptr = rf_cdma_nv_get_hdr_data(
                                          logical_dev->rx_nv_path[band], band);

  if ( rf_hdr_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "HDR PLIM: Invalid HDR NV config structure" );
    return FALSE;
  }

  rf_cdma_config_data_ptr = rf_hdr_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "HDR PLIM: Invalid common CDMA NV config structure" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( plim_data->state == RF_CDMA_PLIM_DISABLED )
  {
    plim_data->mode = RFM_1XEVDO_MODE;
    plim_data->band = band;

    /* Set default max/min power limits if not in override state */
    if ( plim_data->override_enabled == FALSE )
    {
      /* Min power in dBm is always -(-12.7dBm) + K:
      -60.3 for low bands, -63.3 for high bands. */
      plim_data->min_power_limit_dbm10 = -(-127) + 
        10 * rf_cdma_get_turnaround_constant(band);
      
      /*Update plim_max_pwr according to temp reading*/ 
      if ( rf_hdr_plim_update_temp( device ) == FALSE ) 
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
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );
 
    /* Set state to "Enabled" so PLIM now accepts updates from modules */
    plim_data->state = RF_CDMA_PLIM_ENABLED;

    rf_hdr_plim_clear_ccp_override ( plim_data ) ;

    /* At the begining of every TCA all override must cease to exist because TCA is a retune.
     Clear the override so fresh CCP to whatever the retune corresponds is prepared and the
     max power is applied  */
    if ( plim_data->debugging_enabled == TRUE )
    {
      RF_MSG_4( RF_LOW, "HDR PLIM: Started. Max plim(dbm10) %d, min plim (dbm10)"
                        " %d, tx err from hdet(db640) %d, "
                        "max plim err adj(db640) %d",
                plim_data->max_power_limits.max_power_limit_sar_dbm10,
                plim_data->min_power_limit_dbm10,
                plim_data->tx_err_from_hdet_db640,
                plim_data->max_power_limit_err_adj_db640 );
    }
  }
  else
  {
    /* Plim not in disable state. Not safe to re-enable. */
    RF_MSG_2( RF_ERROR, "HDR PLIM: Cannot start plim unless in disabled state "
                        "on device %d, curr state: %d",
              device, plim_data->state );
    status = FALSE;
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disables HDR power limiting on a device
  
  @details
  Puts HDR PLIM module on this device in "disabled" state. It will no longer
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
rf_hdr_plim_stop
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data
)
{
  if ( plim_data == NULL )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: Cannot stop Power Limiting on device %d, "
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
  RF CDMA HDET module should use this API to update tx error in the HDR power 
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
rf_hdr_plim_update_tx_err_from_hdet
(
  rfm_device_enum_type device,
  int16 tx_err_db640
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  int16 k10;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (dev_status_w == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_tx_err_from_hdet(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_tx_err_from_hdet(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) )
  {
    /* Valid state - can accept this update */
    plim_data->tx_err_from_hdet_db640 = tx_err_db640;

    /* Recalculate max power limit taking new TX err from HDET in account.
    Send new limits to MDSP */
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );

    status = TRUE;
  } /* if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) ) */
  else
  {
    RF_MSG_3( RF_HIGH, "HDR PLIM: Cannot update power limit for device: %d,"
                       "Mode %d, state: %d", 
              device, plim_data->mode, plim_data->state );
    status = FALSE;
  }

  if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_6( RF_LOW, "HDR PLIM: Update err from HDET. Status: %d, device: %d, "
                      "TX err from HDET(dB640): %d, actual max pwr lim(dBm10): %d,"
                      " Err adjusted limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->tx_err_from_hdet_db640, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
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
rf_hdr_plim_update_sar_state
(
  rfm_device_enum_type device
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  const rf_hdr_config_type* rf_hdr_config_data_ptr;
  const rf_cdma_config_type* rf_cdma_config_data_ptr;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if (    (dev_status_w == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_sar_state(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_sar_state(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  rf_hdr_config_data_ptr = rf_cdma_nv_get_hdr_data(
                                       logical_dev->rx_nv_path[plim_data->band],
                                       plim_data->band);

  if ( rf_hdr_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "HDR PLIM: Invalid HDR NV config structure" );
    return FALSE;
  }

  rf_cdma_config_data_ptr = rf_hdr_config_data_ptr->cdma_common_ptr;
  if ( rf_cdma_config_data_ptr == NULL )
  {
    RF_MSG( RF_ERROR,
            "HDR PLIM: Invalid common CDMA NV config structure" );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  status = TRUE;

  rf_hdr_plim_update_temp_mdsp( plim_data,
                                device,
                                rf_cdma_config_data_ptr,
                                logical_dev );

  if ( (plim_data->debugging_enabled == TRUE) &&
       (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE))
  {
    RF_MSG_6( RF_LOW, "HDR PLIM: Update target power from SAR. "
                      "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                      "actual max pwr lim(dBm10): %d, Err adjusted "
                      "limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->tx_err_from_hdet_db640, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  } /* if ( plim_data->debugging_enabled == TRUE ) */

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
} /* rf_hdr_plim_update_sar_state() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface adjust the power limit from the SV module.

  @details
  This function is called from SV module to adjust the HDR max power limit
  only when 1x Tx is on and at certain levels.
 
  @param device
  The device to configure SV limit
 
  @param max_power_limit_dbm10
  The max power limit to use for SV.
*/
boolean
rf_hdr_plim_update_sv_limit
(
  rfm_device_enum_type device,
  int16 max_power_limit_dbm10
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  int16 k10;
  boolean api_status;
  
  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if (    (dev_status_w == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_sv_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_sv_limit(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  api_status = TRUE;

  /* Update limit var and FW only for those devices in HDR mode */
  if (plim_data->mode == RFM_1XEVDO_MODE)
  {
    /* Update the interanl variable */
    plim_data->max_power_limits.max_power_limit_sv_dbm10 = max_power_limit_dbm10;
  
    /* Update max power if it is actually enabled */
    if (plim_data->state == RF_CDMA_PLIM_ENABLED)
    {
      /* Only update the limit if the power limiting module is running,
         otherwise it will be updated by default the next time the module
         is started. */
  
      k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);
  
      /* Send new limits to MDSP */
      rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                                 plim_data,
                                 k10,
                                 &plim_data->max_power_limit_err_adj_db640 );
  
      if ( plim_data->debugging_enabled == TRUE )
      {
        RF_MSG_6( RF_LOW, "HDR PLIM: Update SV power limit. "
                          "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                          "actual max pwr lim(dBm10): %d, Err adjusted "
                          "limit(dB640,dBm10): (%d,%d)",
                  api_status, 
                  device, 
                  plim_data->tx_err_from_hdet_db640, 
                  plim_data->max_power_limits.max_power_limit_sv_dbm10,
                  plim_data->max_power_limit_err_adj_db640,
                  rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                        rf_cdma_get_turnaround_constant(plim_data->band),
                                        plim_data->max_power_limit_err_adj_db640) );
      } /* if ( plim_data->debugging_enabled == TRUE ) */
    } /* if (plim_data->state == RF_CDMA_PLIM_ENABLED) */
  }/* if (plim_data->mode == RFM_1XEVDO_MODE) */

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return api_status;
} /* rf_hdr_plim_update_sv_limit() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to adjust the power limit from Thermal Backoff.
  
  @details
  Updates the Thermal Backoff limit variable and triggers an update
  to max power limiting.
 
  @param device
  The device to configure thermal backoff
 
  @param thermal_backoff_state
  The thermal backoff state to configure
*/
boolean
rf_hdr_plim_update_thermal_backoff_limit
( 
  rfm_device_enum_type device,
  rfm_hdr_thermal_state_type thermal_backoff_state
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  int16 k10;
  boolean api_status;
  int16 max_power_limit_dbm10;
  
  /* Get common NV pointer */
  rf_cdma_nv_common_items_type* cdma_nv_common_items = 
    rf_cdma_nv_get_common_table();

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if (    (dev_status_w == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_thermal_backoff_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_update_thermal_backoff_limit(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  api_status = TRUE;

  switch( thermal_backoff_state )
  {
  case RFM_HDR_THERMAL_STATE_ENABLE:
    {
      /* Get max limit from NV */
      if( cdma_nv_common_items == NULL )
      {
        /* If for some reason NV ptr is NULL at this point */
        api_status = FALSE;
        max_power_limit_dbm10 = 420;
        RF_MSG_1( RF_ERROR, "rf_hdr_plim_update_thermal_backoff_limit(): NV is NULL. "
                  "Using %d dBm10 for default backoff.",
                  max_power_limit_dbm10 );
      }
      else
      {
        /* Use the value from NV */
        max_power_limit_dbm10 = cdma_nv_common_items->rfnv_cdma_thermal_max_limit[0];
      }
    }
    break;

  case RFM_HDR_THERMAL_STATE_DISABLE:
      /* Set to highest max limit */
      max_power_limit_dbm10 = 420;
    break;

  default:
      /* Set to highest max limit */
      api_status = FALSE;
      max_power_limit_dbm10 = 420;
      RF_MSG_2( RF_ERROR, "rf_hdr_plim_update_thermal_backoff_limit(): Need to add support for "
                "thermal backoff state %d. Using %d dBm10 for default backoff.",
                thermal_backoff_state, max_power_limit_dbm10 );
  }

  plim_data->max_power_limits.
      max_power_limit_thermal_backoff_dbm10 = max_power_limit_dbm10;

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) )
  {
    /* Only update the limit if the power limiting module is running,
       otherwise it will be updated by default the next time the module
       is started. */

    k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

    /* Send new limits to MDSP */
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );

    if ( plim_data->debugging_enabled == TRUE )
    {
      RF_MSG_6( RF_LOW, "HDR PLIM: Update Thermal Backoff power limit. "
                        "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                        "actual max pwr lim(dBm10): %d, Err adjusted "
                        "limit(dB640,dBm10): (%d,%d)",
                api_status, 
                device, 
                plim_data->tx_err_from_hdet_db640, 
                plim_data->max_power_limits.max_power_limit_thermal_backoff_dbm10,
                plim_data->max_power_limit_err_adj_db640,
                rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                      rf_cdma_get_turnaround_constant(plim_data->band),
                                      plim_data->max_power_limit_err_adj_db640) );
    } /* if ( plim_data->debugging_enabled == TRUE ) */
  } /* if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) && (plim_data->mode == RFM_1XEVDO_MODE) ) */

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return api_status;
} /* rf_hdr_plim_update_thermal_backoff_limit() */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Override max and min power limits
  
  @details
  Override functionality provided for debug purposes to experiment with max and
  min power limits.

  This function will put the HDR plim module in override mode for a given device
  with max and min power limits provide.

  If power limiting module is already enabled and running, the new limits will 
  be immediately applied to firmware else it will just store the overriden 
  limits in its data and apply it when rf_hdr_plim_start() is called. This 
  function will not be able to override plim if it is enabled in HDR mode for
  this device. The corresponding HDR function should be called.

  Once in override mode, this function can be recalled to update the limits 
  again.

  To disable override mode rf_hdr_plim_disable_override() must be called.
  
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
rf_hdr_plim_enable_override
(
  rfm_device_enum_type device,
  int16 min_plim_dbm10,
  int16 max_plim_dbm10
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  int16 k10;
  boolean status = TRUE;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (dev_status_w == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_override_limits(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_override_limits(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( plim_data->plim_supported == TRUE )
  {
    if ( (plim_data->state == RF_CDMA_PLIM_DISABLED) ||
         (plim_data->mode == RFM_1XEVDO_MODE) )
    {
      /* Update power limits in plim data structure only if 
       1) PLIM is currently disabled. Assume it will be enabled in HDR mode 
       2) PLIM is enabled and current mode is HDR. This function cannot 
       override power limits if PLIM is enabled and operating in 1x mode */
      plim_data->max_power_limits.max_power_limit_sar_dbm10 = max_plim_dbm10;
      plim_data->min_power_limit_dbm10 = min_plim_dbm10;

      RF_MSG_3( RF_LOW, "HDR PLIM: Overriding [min, max] pwrs to: [%d, %d]dbm10"
                        "on device %d",
                plim_data->min_power_limit_dbm10,
                plim_data->max_power_limits.max_power_limit_sar_dbm10,
                device );

      /* If plim is enabled, update and send new limits to MDSP else
      these limits will be sent to MDSP when plim is started by calling
      rf_hdr_plim_start() */
      if ( plim_data->state == RF_CDMA_PLIM_ENABLED )
      {
        k10 = 10 * rf_cdma_get_turnaround_constant( plim_data->band );
        rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
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
      RF_MSG_3( RF_HIGH, "HDR PLIM: Cannot override power limits for device %d,"
                         "mode: %d, plim state: %d",
                device,
                plim_data->mode,
                plim_data->state );
      status = FALSE;
    }
  } /* if ( plim_data->plim_supported == TRUE ) */
  else
  {
    RF_MSG_1( RF_HIGH, "HDR PLIM: Cannot override power limits for device %d,"
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
rf_hdr_plim_disable_override
(
  rfm_device_enum_type device
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  rf_cdma_plim_data_type* plim_data;
  
  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  if ( dev_status_w == NULL )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_disable_override(): Invalid "
                        "device: %d",
              device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_disable_override(): Plim data not"
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

  Max power limiting module should be enabled and running in 1xEVDO mode for 
  this API to work.
 
  @param device
  Device whose max power limit is queried

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
   - PLIM module not operating in EVDO(HDR) mode
*/
boolean
rf_hdr_plim_get_err_adj_max_limit
(
  rfm_device_enum_type device,
  int16* max_plim_err_adj_dbm10
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status = TRUE;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (dev_status_w == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_get_err_adj_max_limit(): "
                        "Invalid device: %d",
              device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: rf_hdr_plim_get_err_adj_max_limit(): Plim data "
                        "not initialized for device: %d",
              device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) )
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
rf_hdr_plim_update_temp_common
(
  rf_cdma_plim_data_type* plim_data,
  rfm_device_enum_type device,
  const rf_cdma_config_type* rf_cdma_config_data_ptr,
  const rfc_cdma_logical_device_params_type* logical_dev
)
{
   boolean status = TRUE;

   rf_hdr_plim_update_temp_mdsp( plim_data,
                                 device,
                                 rf_cdma_config_data_ptr,
                                 logical_dev );
   
   if ( (plim_data->debugging_enabled == TRUE) &&
        (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
        (plim_data->mode == RFM_1XEVDO_MODE))
   {
     RF_MSG_6( RF_LOW, "HDR PLIM: Update target power from temperature. "
                       "Status: %d, device: %d, TX err from HDET(dB640): %d, "
                       "actual max pwr lim(dBm10): %d, Err adjusted "
                       "limit(dB640,dBm10): (%d,%d)",
               status, 
               device, 
               plim_data->tx_err_from_hdet_db640, 
               plim_data->max_power_limits.max_power_limit_sar_dbm10,
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
  TX linearizer limit in dBm10 units. It is the max linearizer entry for
  the highest PA state LUT.
  
  @return
  TRUE if the update was accepted and applied to firmware. FALSE if power
  limiting loop was unable to do so.
*/
boolean
rf_hdr_plim_update_lin_limit
(
  rfm_device_enum_type device,
  int16 tx_lin_limit_dBm10
)
{
  rf_cdma_data_status_type* dev_status_w; /* Device Status Write Pointer */
  const rfc_cdma_logical_device_params_type* logical_dev;
  rf_cdma_plim_data_type* plim_data;
  boolean status;
  int16 k10;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status( device );

  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params( device );

  if ( (dev_status_w == NULL) || (logical_dev == NULL) ||
       ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_tx_lin_limit(): Invalid device: "
                        "%d", device );
    return FALSE;
  }

  /* Extract plim_data from device data */
  plim_data = &dev_status_w->plim;

  if ( plim_data->init_done == FALSE )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_tx_lin_limit(): Plim data not"
                        "initialized for device: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  /* Valid state - can accept this update */
  plim_data->max_power_linearizer_limit_dbm10 = tx_lin_limit_dBm10;
 
  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) )
  {
    /* Recalculate max power limit taking new linearizer limit into account.
    Send new limits to MDSP */
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );

    status = TRUE;
  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */
  else
  {
    RF_MSG_3( RF_HIGH, "HDR PLIM: Cannot update power limit for device: %d,"
                       "Mode %d, state: %d", 
              device, plim_data->mode, plim_data->state );
    status = FALSE;
  }

  if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_6( RF_LOW, "HDR PLIM: update_lin_limit. Status: %d, device: %d, "
                      "TX lin limit(dBm10): %d, actual max pwr lim(dBm10): %d,"
                      " Err adjusted limit(dB640,dBm10): (%d,%d)",
              status, 
              device, 
              plim_data->max_power_linearizer_limit_dbm10, 
              plim_data->max_power_limits.max_power_limit_sar_dbm10,
              plim_data->max_power_limit_err_adj_db640,
              rf_cdma_convert_txagc(RFM_CDMA_AGC_FORMAT__DBM10,
                                    rf_cdma_get_turnaround_constant(plim_data->band),
                                    plim_data->max_power_limit_err_adj_db640) );
  }

  /* Leave critical Section */
  rf_common_leave_critical_section( &plim_data->crit_section );

  return status;
} /* rf_hdr_plim_update_lin_limit */

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
rf_hdr_plim_update_lim_vs_temp_vs_freq
(
  rfm_device_enum_type device,
  rf_cdma_plim_data_type* plim_data,
  rfm_cdma_band_class_type band,
  rfm_cdma_chan_type chan,
  uint16 last_valid_therm_read
)
{
  const rfc_cdma_logical_device_params_type* logical_dev;
  boolean status;
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
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_lim_vs_temp_vs_freq(): "
                        "Invalid device: %d", device );
    return FALSE;
  }

  tx_static = rf_cdma_nv_get_tx_static( logical_dev->tx_nv_path[band], band );
  if ( tx_static == NULL )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_lim_vs_temp_vs_freq(): "
                        "NULL pointer: %d", device );
    return FALSE;
  }

  tx_cal = rf_cdma_nv_get_tx_cal( logical_dev->tx_nv_path[band], band );
  if ( tx_cal == NULL )
  {
    RF_MSG_1( RF_ERROR, "HDR PLIM: update_lim_vs_temp_vs_freq(): "
                        "NULL pointer: %d", device );
    return FALSE;
  }

  /* enter critical Section */
  rf_common_enter_critical_section( &plim_data->crit_section );

  k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);

  if ( tx_cal->tx_multi_lin_cal.tx_cal_chan_size > 0 &&
       tx_cal->tx_multi_lin_cal.tx_cal_chan_size <= RF_CDMA_NV_FREQ_LIST_SIZE &&
       tx_cal->therm_cal_data.is_therm_cal_nv_data_valid ==1  )
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
      rf_lininterp_u16_s16( tx_cal->therm_cal_data.tx_therm_cal ,
                            freq_interpolated_temp_array,
                            RF_CDMA_NV_TEMP_LIMIT_SIZE,
                            last_valid_therm_read );

    status = TRUE;
  }
  else
  {
    RF_MSG_2( RF_ERROR, "HDR PLIM: update_lim_vs_temp_vs_freq: "
                        "Invalid Tx cal channel list size %d or "
                        "Invalid therm cal nv data %d ",
                        tx_cal->tx_multi_lin_cal.tx_cal_chan_size, 
                        tx_cal->therm_cal_data.is_therm_cal_nv_data_valid);
    status = FALSE;
  }

  if ( (plim_data->state == RF_CDMA_PLIM_ENABLED) &&
       (plim_data->mode == RFM_1XEVDO_MODE) &&
       status == TRUE )
  {
    /* Recalculate max power limit taking new limit into account.
    Send new limits to MDSP */
    rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                               plim_data,
                               k10,
                               &plim_data->max_power_limit_err_adj_db640 );
  } /* if ( plim_data->state == RF_CDMA_PLIM_ENABLED ) */
  else
  {
    RF_MSG_3( RF_HIGH, "HDR PLIM: Cannot update power limit for device: %d,"
                       "Mode %d, state: %d", 
              device, plim_data->mode, plim_data->state );
    status = FALSE;
  }

  if ( plim_data->debugging_enabled == TRUE )
  {
    RF_MSG_7( RF_LOW, "HDR PLIM: update_lim_vs_temp_vs_freq. "
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
} /* rf_hdr_plim_update_lim_vs_temp_vs_freq */

/*! 
  @brief
  Get a new max power with modified CCP based on L1's override
  channels.
 
  @details
  For MCDO setup enhancement feature, L1 needs flexibility to
  flip between SCDO or MCDO maximum power. When L1 wants to
  choose a max power limit that is not same as the max power for
  the TCA-CCP that the radio originally tuned to, we need a way
  to set a new max power in override scenarios. This function
  gets the new CCP and overrides the power based on the new CCP
  that L1 desires

  @note: The band chan array will always have only [num_carriers
  - 1] elements populated. The rest should NOT be used or looked
    at.
 
  @param device [Input]
  The RF Tx device.
 
  @param band_chan_array [Input]
  The list of override channels
 
  @param num_carriers
  The number of override channels L1 wants to override. This can
  be 1, 2 or 3.
 
  @param *max_power_limit_selected_dB10 [Output]
  The overriden max CCP power. This is a temperature compensated
  entity.
 
  @return success
  [True = successful]
  [False = unsuccessful]
 
*/ 
boolean
rf_hdr_plim_set_max_pwr_per_modified_ccp ( 
   rfm_device_enum_type device , 
   const rfm_cdma_chan_type* const band_chan_array,
   const uint8 num_carriers  ,
   int16* max_power_limit_selected_dB10 ) 
{
  boolean success = FALSE ;
  rf_cdma_data_status_type* dev_status_w ;
  const rfc_cdma_logical_device_params_type* logical_dev ;
  rf_cdma_plim_data_type* plim_data ;
  int16 max_pwr[RF_CDMA_NV_TEMP_LIMIT_SIZE] = { 0 } ;
  rfm_cdma_chan_type override_channels[RFM_CDMA_CARRIER_NUM] = 
  { INVALID_CHANNEL , INVALID_CHANNEL , INVALID_CHANNEL } ;

  /* Get Writable Device Status */
  dev_status_w = rf_cdma_get_mutable_device_status ( device ) ;
  
  /* Get Logical Device Parameter */
  logical_dev = rfc_cdma_get_logical_device_params ( device ) ;
  
  if ( (dev_status_w == NULL)
       || (logical_dev == NULL)
       || ( rfc_cdma_is_tx_device(device) != TRUE ) )
  {
    RF_MSG_1( RF_ERROR, "rf_hdr_plim_update_temp: Invalid device: "
                        "%d", device ) ;
  }
  else
  {
    /* Extract plim_data from device data */
    plim_data = &dev_status_w->plim ;
    if ( plim_data->init_done == FALSE )
    {
      RF_MSG_1( RF_ERROR, "rf_hdr_plim_update_temp: Plim data not"
                          "initialized for device: %d", device );
    }
    else
    {
      {
        size_t count = 0 ;
        for ( ; count < num_carriers ; count++ )
        {
          override_channels[count] = band_chan_array[count] ; 
        }
      }
      RF_MSG_3 ( RF_HIGH , "rf_hdr_plim_set_max_pwr_per_modified_ccp() " 
                 "||Override channels: [1][2][3] => %d | %d | %d ||" ,
                  override_channels[0], override_channels[1] , override_channels[2] ) ;
      
      /* Clear override so you can calculate the new CCP. This should be atomic */
      rf_hdr_plim_clear_ccp_override ( plim_data ) ;
      
      /* Calculate the new CCP based on override channels and set the CCP max power
         accordingly... */ 
      success = 
       rf_hdr_calculate_max_tx_power_limit ( device , plim_data , 
                                             logical_dev , max_pwr , 
                                             override_channels ) ;
      /* Push the overriden max power to FW immediately */
      if ( TRUE == success )
      {
        int16 k10 = 0 ;
        /* Now set the override. This override wil be cleared on a next retune. */
        rf_hdr_plim_set_ccp_override ( plim_data ) ;
        /* Recalculate max power limit taking new limit into account.
        Send new limits to MDSP */
        k10 = 10 * rf_cdma_get_turnaround_constant(plim_data->band);
        rf_hdr_plim_update_limits( logical_dev->tx_mdsp_path,
                                   plim_data,
                                   k10,
                                   &plim_data->max_power_limit_err_adj_db640 );
      }
      else
      {
        RF_MSG ( RF_ERROR , "rf_hdr_set_max_pwr_per_modified_ccp(): " 
                 "Error in calculating new CCP. The max power for the "
                 "overridden channels will NOT be applied") ;
      }
    }
  }
  return success ;
}

#endif /* FEATURE_CDMA1X */
/*! @} */

/*! @} */
