/*! 
 @file rfwcdma_core_xpt.c 
 
 @brief 
  This file has a collection of WCDMA XPT utility functions.
 
*/ 
  
/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/core/src/rfwcdma_core_xpt.c#1 $
  
When       who     what, where, why 
------------------------------------------------------------------------------- 
09/24/14   rmb     Fix CW.
09/15/14   vbh     [1] Added device as a parameter to support Tx sharing
                   [2] Removed hardcoding on device 0
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh    Update XPT Delay structure to support DC-UPA
01/08/14   kai     Remove ET chain txlm buffer allocation
07/30/13   kai     Added forcing Delay vs Temp comp flag.
07/24/13   kai     Added XPT path delay offset vs temp vs freq
06/26/13   kai     Added rc process variation delay compensation
06/23/13   kai     Added WCDMA 16channels xpt delay support
05/10/13   bn      Support EPT_ET_CFG configuration
04/09/13   kai     Added getting et chain info from rfc
02/27/13   kai     initial version
  
============================================================================*/

#include "ftm.h"
#include "rfm_internal.h"
#include "rfwcdma_core_xpt.h"
#include "txlm_intf.h"
#include "rfcommon_msg.h"
#include "rfnv_wcdma.h"
#include "rfcommon_core_xpt.h"
#include "rfdevice_wcdma_intf.h"
#include "rfwcdma_core.h"
#include "rfwcdma_core_temp_comp.h"
#include "rfcommon_core_txlin_types.h"

extern rfnv_wcdma_nv_tbl_type *rfnv_wcdma_get_nv_tbl_ptr( rfnv_wcdma_supported_mode_type band );
extern rfwcdma_core_temp_comp_value_type rfwcdma_core_temp_comp_value;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get XPT Config Param from NV

  @details
  This function will query the XPT Config Param from NV

  @param band

  @param xpt_cfg_index
 
  @return
  Value of the requested XPT Config param
*/
uint32 rfwcdma_core_xpt_get_cfg_param
(
  rfcom_wcdma_band_type band,
  uint8 xpt_cfg_index
)
{
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  uint32 xpt_ctg_val = 0;
  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(rfnv_wcdma_convert_band_rfcom_to_rfnv(band));
  
  if ( nv_tbl != NULL )
  {
    /* Getting the XPT config param value corresponding to index */
    xpt_ctg_val = nv_tbl->tx_ept_dpd_config_params[xpt_cfg_index];
  }
  else
  {
    RF_MSG_1 ( RF_ERROR, "rfwcdma_core_xpt_get_cfg_param: Failed to get valid static "
               "NV pointer for band %d", band);
    xpt_ctg_val = 0;
  }
 
  return xpt_ctg_val;
} /* rfwcdma_core_xpt_get_cfg_param */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Query if XPT is enabled
 
  @details
  This function will check the XPT NV to determine if XPT is enabled or not
 
  @param band
  WCDMA band for which XPT enable is to be checked
 
  @return
  Flag indicating if XPT is enabled or not.
*/
boolean rfwcdma_core_xpt_is_enabled
(
  rfcom_wcdma_band_type band
)
{
  boolean is_xpt_enabled = FALSE;
  xpt_cfg_type nv_xpt_mode = 0;

  nv_xpt_mode = (xpt_cfg_type) rfwcdma_core_xpt_get_cfg_param(band, XPT_CFG);

  if ( ( nv_xpt_mode == EPT_CFG ) ||
       ( nv_xpt_mode == EPT_ET_CFG ) ||
       ( nv_xpt_mode == ET_CFG ) )
  {
    is_xpt_enabled = TRUE;
  }
  else
  {
    is_xpt_enabled = FALSE;
  }

  return is_xpt_enabled;

} /* rfwcdma_core_xpt_is_enabled */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Compute the XPT path delay value.
 
  @details
  This function will return the xpt path delay value.
 
  @param band
  WCDMA operational band.

  @param rf_chan_tx
  WCDMA Tx RF channel.
 
  @param force_delay_temp_comp
  Flag to force apply delay temp comp. 
 
  @param therm_scaled_value
  Current scaled therm value. 
 
  @return
  XPT path delay value.
*/
int32 rfwcdma_core_xpt_path_delay_comp
(
   rfm_device_enum_type device,
   rfcom_wcdma_band_type band,
   uint16 rf_chan_tx,
   boolean force_delay_temp_comp,
   uint16 therm_scaled_value,
   rfcom_multi_carrier_hspa_id_type tx_multi_carriers
)
{
  int8 i;
  int32 xpt_path_delay_val = 0;
  int16 delay_comp_x10ns = 0;
  rfwcdma_nv_xpt_path_delay_type *xpt_delay_tbl = NULL;
  rfwcdma_nv_xpt_dc_delay_type *xpt_dc_delay_tbl = NULL;
  rfnv_tx_delay_offsets_type *xpt_delay_offset_tbl = NULL;
  rfnv_wcdma_nv_tbl_type *nv_tbl = NULL;
  boolean delay_calibrated = FALSE;
  uint16 delay_calibration_temp_adc = 0;
  uint16 delay_calibration_temp_scaled = 0;
  int16 delay_offset_computed = 0;
  uint8 num_of_channels_enabled_for_delay = 0;
  uint16 *therm_bin_array = NULL;
  tx_band_cal_type* txlin_data = NULL;
  boolean delay_vs_temp_comp_status = FALSE;
  uint32 upper_bound_chan;

  nv_tbl = rfnv_wcdma_get_nv_tbl_ptr(rfnv_wcdma_convert_band_rfcom_to_rfnv(band));

  if(nv_tbl == NULL)
  {
    RF_MSG_1 ( RF_ERROR, "rfwcdma_core_xpt_path_delay_comp: Failed to get valid static "
               "NV pointer for band %d", band);
    return 0;
  }

  rfdevice_wcdma_tx_get_et_delay_compensation(device,
                                              band,
                                              &delay_comp_x10ns);

  /* Getting the XPT path 16channels delay info */
  xpt_delay_tbl = &(nv_tbl->xpt_path_delay);
  xpt_dc_delay_tbl = &(nv_tbl->xpt_dc_delay);

  if (( tx_multi_carriers == RFCOM_DUAL_CARRIER ) &&
      (nv_tbl->tx_dc_delay_offsets.channels_enabled > 0) )
  {
    xpt_delay_offset_tbl = &(nv_tbl->tx_dc_delay_offsets);
    num_of_channels_enabled_for_delay = xpt_dc_delay_tbl->num_of_active_entries;
  }
  else
  {
    xpt_delay_offset_tbl = &(nv_tbl->tx_delay_offsets);
    num_of_channels_enabled_for_delay = xpt_delay_tbl->num_of_active_entries;
  }

  /* Return the ET path delay value if the 16channels delay info not available */
  if (num_of_channels_enabled_for_delay == 0)
  {
    xpt_path_delay_val = nv_tbl->et_delay_value + delay_comp_x10ns;
  }
  else
  {
    /* Find the delay value according to channel info */
    for(i = 0; i < num_of_channels_enabled_for_delay; i++)
    {
       if((RFCOM_DUAL_CARRIER == tx_multi_carriers) && 
         (xpt_dc_delay_tbl->num_of_active_entries > 0))
      {
        upper_bound_chan = xpt_dc_delay_tbl->upper_bound_tx_chan[i];
      }
      else
      {
        upper_bound_chan = xpt_delay_tbl->upper_bound_tx_chan[i];
      }

      if(rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx) <= 
         rfwcdma_core_get_freq_from_ul_arfcn(upper_bound_chan))
      {
        if((RFCOM_DUAL_CARRIER == tx_multi_carriers) && 
           (xpt_dc_delay_tbl->num_of_active_entries > 0))
        {
          xpt_path_delay_val = (int32)(xpt_dc_delay_tbl->dc_delay[i] + 
                                      + delay_comp_x10ns);
        }
        else
        {
          xpt_path_delay_val = (int32)(xpt_delay_tbl->delay_value[i] + 
                                      + delay_comp_x10ns);
        }
        break;
      }
    }
  }

  /*Delay Temp compensation should not happen during RF Calibration(FTM state)*/
  if((force_delay_temp_comp == TRUE) ||
     ((xpt_delay_offset_tbl->channels_enabled != 0) && 
      ((!IS_FTM_IN_TEST_MODE())||(rfm_get_calibration_state() == FALSE))))
  {
    /* Find out if Delay NV was chared or calibrated 
       MSB bit will be turned ON if delay NV is calibrated, else NOT */
    if(num_of_channels_enabled_for_delay & 0x80)
    {
      txlin_data = rfwcdma_core_txlin_get_band_data(band);
      if (txlin_data != NULL)
      {
        delay_calibrated = TRUE;
        delay_calibration_temp_adc = txlin_data->ref_lin_temp_adc;
        delay_calibration_temp_scaled = rfwcdma_core_temp_comp_get_scaled_therm_read(delay_calibration_temp_adc);
        
        /* Obtain the valid channels for delay  by masking out the MSB bit */
        num_of_channels_enabled_for_delay = (num_of_channels_enabled_for_delay & 0x7F);
      }
      else
      {
        delay_calibrated = FALSE;
        RF_MSG( RF_ERROR,"Null pointer access!");
      }
    }
    else
    {
      delay_calibrated = FALSE;
    }

    /* Find the delay value offset according to delay vs temp vs freq */
    therm_bin_array = rfwcdma_core_temp_comp_get_therm_bin_array();

    if (force_delay_temp_comp == TRUE)
    {
      delay_vs_temp_comp_status = 
        rf_common_xpt_delay_offset_computation(xpt_delay_offset_tbl,
                                               (const uint16*) therm_bin_array,
                                               therm_scaled_value,
                                               delay_calibration_temp_scaled,
                                               rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx),
                                               delay_calibrated,
                                               &delay_offset_computed,
                                               (rfcommon_core_convert_freq_handler_type)(&rfwcdma_core_get_freq_from_ul_arfcn));
    }
    else
    {
      delay_vs_temp_comp_status = 
        rf_common_xpt_delay_offset_computation(xpt_delay_offset_tbl,
                                               (const uint16*) therm_bin_array,
                                               rfwcdma_core_temp_comp_value.last_scaled_temp_value,
                                               delay_calibration_temp_scaled,
                                               rfwcdma_core_get_freq_from_ul_arfcn(rf_chan_tx),
                                               delay_calibrated,
                                               &delay_offset_computed,
                                               (rfcommon_core_convert_freq_handler_type)(&rfwcdma_core_get_freq_from_ul_arfcn));
    }

    if(delay_vs_temp_comp_status == FALSE)
    {
      delay_offset_computed = 0;
    }

    xpt_path_delay_val += delay_offset_computed;

    RF_MSG_5(RF_HIGH, "XPT delay temp comp: %d cal temp scaled: %d current temp scaled:" 
                      "%d delay_calibrated: %d forced temp comp: %d",
             delay_offset_computed,
             delay_calibration_temp_scaled,
             rfwcdma_core_temp_comp_value.last_scaled_temp_value,
             delay_calibrated,
             force_delay_temp_comp);
  }

  RF_MSG_4(RF_HIGH, "XPT delay value:%d band:%d channel:%d delay comp: %d",
           xpt_path_delay_val,
           band,
           rf_chan_tx,
           delay_comp_x10ns);

  return xpt_path_delay_val;
} /* rfwcdma_core_xpt_path_delay_comp */
