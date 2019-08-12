/*!
  @file
  rfnv_wcdma_nv_copy_funcs.c

  @brief

  @details
  This module retrieves all NV items that will be used by the WCDMA RF unit.

*/

/*===========================================================================

Copyright (c) 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/rf/nv/src/rfnv_wcdma_nv_copy_funcs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
11/03/14   kai     Added support for WCDMA Rx V3 static NVs
10/31/14   ac      tuner NV support for DBDC
09/26/14   kai     Added support for WCDMA Rx V2 NVs
05/01/14   aro     NV item to hold char data for DC ET Delay vs freq vs temp
02/26/14   zhh     Update XPT Delay structure to support DC-UPA
07/25/14   zhh     KW warning with rfnv_wcdma_copy_hdet_mod_range
04/14/14   zhh     PinPout NV for DCUPA
03/20/14   zhh     update new MPR backoff NV
04/20/14   bn      WCDMA 8 PA state support and related cleanup
02/25/14   zhh     fix KW warning
01/15/14   rmb     Add NV support for Noise Floor Correction.
11/25/13   rmb     Add Pin Cal versus freq related NVs for all PA states and bands.  
08/23/13   vs      Added Rx static and Cal NVs
08/12/13   kai     Added DPD scaling support 
08/01/13   kr      replace memcpy with memscpy
07/24/13   kai     Added XPT path delay offset vs temp vs freq
07/16/13   vs      Added CL tuner NVs
06/05/13   kcj     Initial Revision. 

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfnv_wcdma_nv_copy_funcs.h"
#include "rfnv_wcdma_types.h"
#include "rfwcdma_core.h"
/* memscpy */
#include "stringl.h"
#include "modem_mem.h"

/*===========================================================================
                        
                        FUNCTION DEFINITIONS

 ===========================================================================*/

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_RISE, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_rise, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Rise 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_RISE, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_rise_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Rise 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_RISE, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_rise_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Rise 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{ 
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_RISE, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_rise_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Rise 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0,LNA_HS_RISE, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_rise_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_FALL, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_fall, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Fall 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_FALL, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_fall_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Fall 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_FALL, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_fall_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Fall 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_FALL, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_fall_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA HS Range Fall 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_hs_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_HS_FALL, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_hs_range_fall_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM HS Level NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_hs_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_hs_level = ((wcdma_nv_item_type*)item_ptr)->im_hs_level;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM HS Level 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_hs_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_hs_level_2 = ((wcdma_nv_item_type*)item_ptr)->im_hs_level_2;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM HS Level 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_hs_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_hs_level_3 = ((wcdma_nv_item_type*)item_ptr)->im_hs_level_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM HS Level 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_hs_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_hs_level_4 = ((wcdma_nv_item_type*)item_ptr)->im_hs_level_4;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the VGA Gain Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_vga_gain_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, VGA_OFFSET, 0 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->vga_gain_offset, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the VGA Gain Offset vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_vga_gain_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, VGA_OFFSET_VS_FREQ, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->vga_gain_offset_vs_freq[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 VGA Gain Offset vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_vga_gain_offset_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, VGA_OFFSET_VS_FREQ, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_vga_gain_offset_vs_freq[loop_ctr], rfnv_tbl_ptr);
  }
}
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the VGA Gain Offset vs. Temp NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_vga_gain_offset_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_TEMP_TABLE_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->vga_gain_offset_vs_temp[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->vga_gain_offset_vs_temp[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM2 I NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im2_i_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im2_i_value = ((wcdma_nv_item_type*)item_ptr)->im2_i_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM2 Q Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im2_q_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im2_q_value = ((wcdma_nv_item_type*)item_ptr)->im2_q_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM2 Transconductor Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im2_transconductor_value
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im2_transconductor_value = ((wcdma_nv_item_type*)item_ptr)->im2_transconductor_value;
}

/* R99 LNA Range Functions */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 4 ,
											 ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 4,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM Level NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_level = ((wcdma_nv_item_type*)item_ptr)->im_level;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM Level 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_level_2 = ((wcdma_nv_item_type*)item_ptr)->im_level_2;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM Level 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_level_3 = ((wcdma_nv_item_type*)item_ptr)->im_level_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the IM Level 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_im_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->im_level_4 = ((wcdma_nv_item_type*)item_ptr)->im_level_4;
}

/* Timer Copy Functions */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Non-Bypass Timer NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_nonbypass_timer
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->nonbypass_timer = ((wcdma_nv_item_type*)item_ptr)->nonbypass_timer;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Bypass Timer NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_bypass_timer
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->bypass_timer = ((wcdma_nv_item_type*)item_ptr)->bypass_timer;
}

/* LNA Range Offset Copy Functions */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 0 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_4
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_5
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Offset vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_1, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_offset_vs_freq[loop_ctr], rfnv_tbl_ptr);
  }
  
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Offset vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_1, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_offset_vs_freq[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Offset vs. Freq 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_offset_vs_freq_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_2, loop_ctr ,
                                              ((wcdma_nv_item_type*)item_ptr)->lna_offset_vs_freq_2[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Offset vs. Freq 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_2, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_offset_vs_freq_2[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Offset vs. Freq 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_offset_vs_freq_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_3, loop_ctr ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_offset_vs_freq_3[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Offset vs. Freq 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
   rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_3, loop_ctr ,
                                              ((wcdma_nv_item_type*)item_ptr)->c1_lna_offset_vs_freq_3[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Offset vs. Freq 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_offset_vs_freq_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
	 rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_4, loop_ctr ,
												 ((wcdma_nv_item_type*)item_ptr)->lna_offset_vs_freq_4[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Offset vs. Freq 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_offset_vs_freq_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
	 rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__CUMULATIVE, LNA_OFFSET_VS_FREQ_4, loop_ctr ,
												 ((wcdma_nv_item_type*)item_ptr)->c1_lna_offset_vs_freq_4[loop_ctr], rfnv_tbl_ptr);
  }
}

/* RX AGC Min/Max Copy Functions*/
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RX AGC Min NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_agc_min
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->rx_agc_min = ((wcdma_nv_item_type*)item_ptr)->rx_agc_min;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RX AGC Min 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_agc_min_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->rx_agc_min_2 = ((wcdma_nv_item_type*)item_ptr)->rx_agc_min_2;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RX AGC Min 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_agc_min_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->rx_agc_min_3 = ((wcdma_nv_item_type*)item_ptr)->rx_agc_min_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RX AGC Max NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_agc_max
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->rx_agc_max = ((wcdma_nv_item_type*)item_ptr)->rx_agc_max;
}

/* Miscellaneous */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC Phase Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_phase_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_phase_offset = ((wcdma_nv_item_type*)item_ptr)->agc_phase_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R1 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r1_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_rise_sw[1] = ((wcdma_nv_item_type*)item_ptr)->r1_rise;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R1 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r1_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_fall_sw[1] = ((wcdma_nv_item_type*)item_ptr)->r1_fall;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx Lim vs. Temp NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lim_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_TEMP_TABLE_SIZ; loop_ctr++)
  {
    /* read and scale back to 10-bits number */
    rfnv_tbl_ptr->tx_lim_vs_temp[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->tx_lim_vs_temp[loop_ctr] + RFNV_WCDMA_TX_LIM_OFFSET;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the UTRAN Tx Lim vs. Temp Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_utran_tx_lim_vs_temp_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_TEMP_TABLE_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->utran_tx_lim_vs_temp_offset[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->utran_tx_lim_vs_temp_offset[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lim vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lim_vs_freq
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->tx_lim_vs_freq[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->tx_lim_vs_freq[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lim Adj Gain NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lim_adj_gain
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->tx_lim_adj_gain = ((wcdma_nv_item_type*)item_ptr)->tx_lim_adj_gain;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Exp HDET vs. AGC NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_exp_hdet_vs_agc
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  /* read the first 16 elements from this NV item */
  for (loop_ctr=0; loop_ctr<NV_WCDMA_EXP_HDET_VS_AGC_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->exp_hdet_vs_agc[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->exp_hdet_vs_agc[loop_ctr];
  }

  /* set 17th element equal to 16th element */
  rfnv_tbl_ptr->exp_hdet_vs_agc[NV_WCDMA_EXP_HDET_VS_AGC_SIZ] =
  rfnv_tbl_ptr->exp_hdet_vs_agc[NV_WCDMA_EXP_HDET_VS_AGC_SIZ-1];
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HDET Off NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hdet_off
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hdet_off = ((wcdma_nv_item_type*)item_ptr)->hdet_off;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HDET SPN NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hdet_spn
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hdet_spn = ((wcdma_nv_item_type*)item_ptr)->hdet_spn;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Max Tx Power NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_max_tx_power
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->max_tx_power = ((wcdma_nv_item_type*)item_ptr)->max_tx_power;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Out of Service Threshold NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_out_of_service_thresh
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->out_of_service_thresh = ((wcdma_nv_item_type*)item_ptr)->out_of_service_thresh;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the ENC BTF Dly NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_enc_btf_dly
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->enc_btf_dly = ((wcdma_nv_item_type*)item_ptr)->enc_btf_dly;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Rx Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->rx_delay = ((wcdma_nv_item_type*)item_ptr)->rx_delay;
}

/* TX AGC Copy Functions */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC PA On Rise Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_pa_on_rise_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_pa_on_rise_delay = ((wcdma_nv_item_type*)item_ptr)->agc_pa_on_rise_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC PA On Fall Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_pa_on_fall_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_pa_on_fall_delay = ((wcdma_nv_item_type*)item_ptr)->agc_pa_on_fall_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC TX On Rise Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_tx_on_rise_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_tx_on_rise_delay = ((wcdma_nv_item_type*)item_ptr)->agc_tx_on_rise_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC TX On Fall Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_tx_on_fall_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_tx_on_fall_delay = ((wcdma_nv_item_type*)item_ptr)->agc_tx_on_fall_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC Update TX AGC Time NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_update_tx_agc_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_update_tx_agc_time = ((wcdma_nv_item_type*)item_ptr)->agc_update_tx_agc_time;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Gain Up Time NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_gain_up_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->pa_gain_up_time = ((wcdma_nv_item_type*)item_ptr)->pa_gain_up_time;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Gain Down Time NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_gain_down_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->pa_gain_down_time = ((wcdma_nv_item_type*)item_ptr)->pa_gain_down_time;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Rot Ang PA 00 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_rot_ang_pa_00
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.tx_rot_angle[0] = ((wcdma_nv_item_type*)item_ptr)->tx_rot_ang_pa_00;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Rot Ang PA 01 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_rot_ang_pa_01
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.tx_rot_angle[1] = ((wcdma_nv_item_type*)item_ptr)->tx_rot_ang_pa_01;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Rot Ang PA 10 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_rot_ang_pa_10
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.tx_rot_angle[2] = ((wcdma_nv_item_type*)item_ptr)->tx_rot_ang_pa_10;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Rot Ang PA 11 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_rot_ang_pa_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.tx_rot_angle[3] = ((wcdma_nv_item_type*)item_ptr)->tx_rot_ang_pa_11;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RTR VCO Coarse Tuning NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rtr_vco_coarse_tuning
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<16; loop_ctr++)
  {
    rfnv_tbl_ptr->rtr_vco_coarse_tuning[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->rtr_vco_coarse_tuning[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Up NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_up
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 1 indicates R1 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_up[1] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_up;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Down NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_down
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 1 indicates R1 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_dn[1] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_down;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R1 Rise Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r1_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r1_rise_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r1_rise_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R1 Fall Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r1_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r1_fall_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r1_fall_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx Cal Chan NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->tx_cal_chan[0][loop_ctr] = (uint16)((wcdma_nv_item_type*)item_ptr)->tx_cal_chan[loop_ctr];

    rfnv_tbl_ptr->tx_cal_chan_freq[0][loop_ctr] =
    rfwcdma_core_get_freq_from_uarfcn( band,
                                       (uint16)((wcdma_nv_item_type*)item_ptr)->tx_cal_chan[loop_ctr],
                                       RFWCDMA_CORE_UARFCN_UL );
  }

  /* Load the Tx cal channel list into common Tx lin temp comp 
  data for use with characterized lin vs temp vs freq NVs */
  rfnv_wcdma_load_lin_vs_temp_tx_char_chan( rfnv_tbl_ptr,
                                            item_ptr,
                                            band );

  //if ( rfnv_tbl_ptr->tx_cal_chan[0][0] != 0 &&
  //     rfnv_tbl_ptr->rx_cal_chan[0][0] != 0 )
  //{
  //  rfnv_tbl_ptr->interpolation_enable = TRUE;
  //}
  //else
  //{
  //  rfnv_tbl_ptr->interpolation_enable = FALSE;
  //}
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RX Cal Chan NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
	rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__0, RX_CAL_CHAN_LIST, loop_ctr ,
												 (uint16)((wcdma_nv_item_type*)item_ptr)->rx_cal_chan[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 RX Cal Chan NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_rx_cal_chan
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
  {
	rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, RX_CAL_CHAN_LIST, loop_ctr ,
													 (uint16)((wcdma_nv_item_type*)item_ptr)->rx_cal_chan[loop_ctr], rfnv_tbl_ptr);
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R2 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r2_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_rise_sw[2] = ((wcdma_nv_item_type*)item_ptr)->r2_rise;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R2 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r2_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_fall_sw[2] = ((wcdma_nv_item_type*)item_ptr)->r2_fall;
  }

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R3 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r3_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_rise_sw[3] = ((wcdma_nv_item_type*)item_ptr)->r3_rise;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the R3 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_r3_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    rfnv_tbl_ptr->pa_static.pa_fall_sw[3] = ((wcdma_nv_item_type*)item_ptr)->r3_fall;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Up R2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_up_r2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 2 indicates R2 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_up[2] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_up_r2;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Down R2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_down_r2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 2 indicates R2 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_dn[2] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_down_r2;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Up R3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_up_r3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 3 indicates R3 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_up[3] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_up_r3;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Compensate Down R3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_compensate_down_r3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /* Index 3 indicates R3 switchpoint */
    rfnv_tbl_ptr->pa_static.pa_comp_dn[3] = ((wcdma_nv_item_type*)item_ptr)->pa_compensate_down_r3;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R2 Rise Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r2_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r2_rise_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r2_rise_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R2 Fall Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r2_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r2_fall_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r2_fall_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R3 Rise Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r3_rise_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r3_rise_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r3_rise_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PRACH R3 Fall Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_prach_r3_fall_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->prach_r3_fall_offset = ((wcdma_nv_item_type*)item_ptr)->prach_r3_fall_offset;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Range Map NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_range_map
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<RFLIN_NUM_TX_LIN_LEGACY_4PA; loop_ctr++)
    {
      rfnv_tbl_ptr->pa_static.pa_range_map[loop_ctr] = ((wcdma_nv_item_type*)item_ptr)->pa_range_map[loop_ctr];
    }
    for (loop_ctr=RFLIN_NUM_TX_LIN_LEGACY_4PA; loop_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; loop_ctr++)
    {
      rfnv_tbl_ptr->pa_static.pa_range_map[loop_ctr] = 255;
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the DCH AGC Update TX AGC Time NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_dch_agc_update_tx_agc_time
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->dch_agc_update_tx_agc_time = ((wcdma_nv_item_type*)item_ptr)->dch_agc_update_tx_agc_time;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the AGC TX ADJ PDM Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_agc_tx_adj_pdm_delay
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->agc_tx_adj_pdm_delay = ((wcdma_nv_item_type*)item_ptr)->agc_tx_adj_pdm_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R1 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r1_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r1_rise = ((wcdma_nv_item_type*)item_ptr)->hs_r1_rise;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R1 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r1_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r1_fall = ((wcdma_nv_item_type*)item_ptr)->hs_r1_fall;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R2 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r2_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r2_rise = ((wcdma_nv_item_type*)item_ptr)->hs_r2_rise;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R2 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r2_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r2_fall = ((wcdma_nv_item_type*)item_ptr)->hs_r2_fall;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R3 Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r3_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r3_rise = ((wcdma_nv_item_type*)item_ptr)->hs_r3_rise;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HS R3 Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hs_r3_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->hs_r3_fall = ((wcdma_nv_item_type*)item_ptr)->hs_r3_fall;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Max Pwr Backoff Voltages NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_max_pwr_backoff_voltages
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<3; loop_ctr++)
  {
    rfnv_tbl_ptr->max_pwr_backoff_voltages[loop_ctr] =
      ((wcdma_nv_item_type*)item_ptr)->max_pwr_backoff_voltages[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Max Pwr Backoff Volt 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_max_pwr_backoff_volt1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<4; loop_ctr++)
  {
    rfnv_tbl_ptr->max_pwr_backoff_volt1[loop_ctr] =
      ((wcdma_nv_item_type*)item_ptr)->max_pwr_backoff_volt1[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Max Pwr Backoff Volt 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_max_pwr_backoff_volt2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<4; loop_ctr++)
  {
    rfnv_tbl_ptr->max_pwr_backoff_volt2[loop_ctr] =
      ((wcdma_nv_item_type*)item_ptr)->max_pwr_backoff_volt2[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Max Pwr Backoff Volt 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_max_pwr_backoff_volt3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<4; loop_ctr++)
  {
    rfnv_tbl_ptr->max_pwr_backoff_volt3[loop_ctr] =
      ((wcdma_nv_item_type*)item_ptr)->max_pwr_backoff_volt3[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF Mis Comp A Coeff NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_mis_comp_a_coeff
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_mis_comp_a_coeff = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_mis_comp_a_coeff;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF Mis Comp B Coeff NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_mis_comp_b_coeff
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_mis_comp_b_coeff = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_mis_comp_b_coeff;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef0_1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef0_1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef0_1 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef0_1;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef2_3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef2_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef2_3 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef2_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef4_5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef4_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef4_5 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef4_5;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef6_7 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef6_7
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef6_7 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef6_7;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef8_9 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef8_9
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef8_9 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef8_9;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef10_11 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef10_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef10_11 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef10_11;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef12_13 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef12_13
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef12_13 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef12_13;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI I Coef14_15 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_i_coef14_15
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_i_coef14_15 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_i_coef14_15;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef0_1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef0_1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef0_1 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef0_1;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef2_3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef2_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef2_3 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef2_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef4_5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef4_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef4_5 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef4_5;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef6_7 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef6_7
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef6_7 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef6_7;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef8_9 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef8_9
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef8_9 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef8_9;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef10_11 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef10_11
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef10_11 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef10_11;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef12_13 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef12_13
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef12_13 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef12_13;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Q Coef14_15 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_q_coef14_15
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_q_coef14_15 = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_q_coef14_15;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI IQ Trunc NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_iq_trunc
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_rxf_ici_iq_trunc = ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_iq_trunc;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 VGA Gain Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_vga_gain_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band, RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, VGA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_vga_gain_offset, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 VGA Gain Offset vs. Temp NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_vga_gain_offset_vs_temp
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<NV_TEMP_TABLE_SIZ; loop_ctr++)
  {
    rfnv_tbl_ptr->c1_vga_gain_offset_vs_temp[loop_ctr] =
      ((wcdma_nv_item_type*)item_ptr)->c1_vga_gain_offset_vs_temp[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 3 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_3, rfnv_tbl_ptr);

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 4 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_RISE, 5 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 1 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 2 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 3 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 4 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_FALL, 5 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 0 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_2, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_3, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_4, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_5
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__0, LNA_OFFSET, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_5, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM Level NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im_level
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im_level = ((wcdma_nv_item_type*)item_ptr)->c1_im_level;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM Level 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im_level_2
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im_level_2 = ((wcdma_nv_item_type*)item_ptr)->c1_im_level_2;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM Level 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im_level_3
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im_level_3 = ((wcdma_nv_item_type*)item_ptr)->c1_im_level_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM Level 4 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im_level_4
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im_level_4 = ((wcdma_nv_item_type*)item_ptr)->c1_im_level_4;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM2 I Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im2_i_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im2_i_value = ((wcdma_nv_item_type*)item_ptr)->c1_im2_i_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM2 Q Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im2_q_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im2_q_value = ((wcdma_nv_item_type*)item_ptr)->c1_im2_q_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 IM2 Transconductor Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_im2_transconductor_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_im2_transconductor_value = ((wcdma_nv_item_type*)item_ptr)->c1_im2_transconductor_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 Non-Bypass Timer NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_nonbypass_timer
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_nonbypass_timer = ((wcdma_nv_item_type*)item_ptr)->c1_nonbypass_timer;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 Bypass Timer NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_bypass_timer
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_bypass_timer = ((wcdma_nv_item_type*)item_ptr)->c1_bypass_timer;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RFR VCO Coarse Tune 2 1900 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rfr_vco_coarse_tune_2_1900
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<12; loop_ctr++)
  {
    rfnv_tbl_ptr->rfr_vco_coarse_tune_2_1900[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->rfr_vco_coarse_tune_2_1900[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the RF DRx Mode Sel NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rf_drx_mode_sel
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->rf_drx_mode_sel[0], sizeof(uint8)*3,
         (void *)&((wcdma_nv_item_type*)item_ptr)->rf_drx_mode_sel[0],
         sizeof(uint8)*3);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA Iaccum NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_iaccum
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  // Unused
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA Qaccum NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_qaccum
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  // Unused
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Timer Hysterisis NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_timer_hysterisis
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    /*! pa_fall_swpt array 0th index holds timer hysterisis value */
    rfnv_tbl_ptr->pa_static.pa_fall_sw[PA_STATIC_TIMER_HYS_INDEX] = ((wcdma_nv_item_type*)item_ptr)->timer_hysterisis;
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the EXP HDET vs. AGC v2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_exp_hdet_vs_agc_v2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  /* read the first 16 elements from this NV item */
  for (loop_ctr=0; loop_ctr < NV_WCDMA_EXP_HDET_VS_AGC_SIZ; loop_ctr++)
  {
   rfnv_tbl_ptr->exp_hdet_vs_agc_v2[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->exp_hdet_vs_agc_v2[loop_ctr];
  }

  /* set 17th element equal to 16th element */
  rfnv_tbl_ptr->exp_hdet_vs_agc_v2[NV_WCDMA_EXP_HDET_VS_AGC_SIZ] = 
  rfnv_tbl_ptr->exp_hdet_vs_agc_v2[NV_WCDMA_EXP_HDET_VS_AGC_SIZ - 1];
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA C0 C1 Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_c0_c1_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->wcdma_c0_c1_delay = ((wcdma_nv_item_type*)item_ptr)->wcdma_c0_c1_delay;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA LNA Phase Ctl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_lna_phase_ctl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<RF_MAX_LNA_GAIN_STATES; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_lna_phase_ctl[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_lna_phase_ctl[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA LNA Phase Ctl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_lna_phase_ctl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<RF_MAX_LNA_GAIN_STATES; loop_ctr++)
  {
    rfnv_tbl_ptr->c1_wcdma_lna_phase_ctl[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_lna_phase_ctl[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA Rel6 TX Beta Scaling Comp NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rel6_tx_beta_scaling_comp
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<7; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_rel6_tx_beta_scaling_comp[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_rel6_tx_beta_scaling_comp[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA Rel6 TX AGC Offset NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rel6_tx_agc_offset
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<7; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_rel6_tx_agc_offset[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_rel6_tx_agc_offset[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA Rel6 TX MPR Backoff NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rel6_tx_mpr_backoff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<7; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_rel6_tx_mpr_backoff[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_rel6_tx_mpr_backoff[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF Mis Comp A Coeff NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_a_coeff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_mis_comp_a_coeff = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_mis_comp_a_coeff;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF Mis Comp B Coeff NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_mis_comp_b_coeff
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_mis_comp_b_coeff = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_mis_comp_b_coeff;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef0_1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef0_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef0_1 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef0_1;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef2_3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef2_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef2_3 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef2_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef4_5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef4_5
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef4_5 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef4_5;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef6_7 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef6_7
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef6_7 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef6_7;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef8_9 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef8_9
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef8_9 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef8_9;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef10_11 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef10_11
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef10_11 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef10_11;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef12_13 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef12_13
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef12_13 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef12_13;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI I Coef14_15 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_i_coef14_15
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_i_coef14_15 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_i_coef14_15;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef0_1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef0_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef0_1 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef0_1;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef2_3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef2_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef2_3 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef2_3;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef4_5 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef4_5
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef4_5 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef4_5;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef6_7 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef6_7
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef6_7 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef6_7;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef8_9 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef8_9
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef8_9 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef8_9;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef10_11 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef10_11
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef10_11 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef10_11;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef12_13 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef12_13
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef12_13 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef12_13;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Q Coef14_15 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_q_coef14_15
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_q_coef14_15 = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_q_coef14_15;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI IQ Trunc NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_iq_trunc
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_wcdma_rxf_ici_iq_trunc = ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_iq_trunc;
}

/* R99 Carrier 1 LNA Range Functions */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 2 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_2_car1, rfnv_tbl_ptr);

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_4_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Rise 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_rise_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_rise_5_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall Car 1NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_car1, rfnv_tbl_ptr);

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 2 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 2,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_2_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_4_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Fall 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_fall_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_fall_5_car1, rfnv_tbl_ptr);
}

/* Carrier 1 LNA Range Offset Copy Functions */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset ar 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 0 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 2 Car 1NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_2_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_4_car1, rfnv_tbl_ptr);

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the LNA Range Offset 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_lna_range_offset_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->lna_range_offset_5_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the VGA Gain Offset Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_vga_gain_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_0, RFCOM_MULTI_CARRIER_ID__1 , VGA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->vga_gain_offset_car1, rfnv_tbl_ptr);
}

/* R99 Path 1 Carrier 1 LNA Range Functions */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 2 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_2_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 4 ,
											  ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_4_car1, rfnv_tbl_ptr);

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Rise 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_rise_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_RISE, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_rise_5_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall Car 1NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 2 Car 1NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 2 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_2_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 3 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 4 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_4_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Fall 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_fall_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_static_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_FALL, 5 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_fall_5_car1, rfnv_tbl_ptr);
}

/* Carrier 1 LNA Range Offset Copy Functions */
/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1 ,RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 0 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 2 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_2_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 1,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_2_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 3 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_3_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 2,
												 ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_3_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 4 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_4_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 3,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_4_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 LNA Range Offset 5 Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_lna_range_offset_5_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1, LNA_OFFSET, 4,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_lna_range_offset_5_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 VGA Gain Offset Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_vga_gain_offset_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_copy_rx_dynamic_data_from_legacy_nv(band ,RFM_DEVICE_1, RFCOM_MULTI_CARRIER_ID__1 , VGA_OFFSET, 1 ,
  	                                           ((wcdma_nv_item_type*)item_ptr)->c1_vga_gain_offset_car1, rfnv_tbl_ptr);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Cal Data Car 0 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<17; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_rxf_ici_cal_data_car0[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_cal_data_car0[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA RXF ICI Cal Data Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_rxf_ici_cal_data_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<17; loop_ctr++)
  {
    rfnv_tbl_ptr->wcdma_rxf_ici_cal_data_car1[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->wcdma_rxf_ici_cal_data_car1[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Cal Data Car 0 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<17; loop_ctr++)
  {
    rfnv_tbl_ptr->c1_wcdma_rxf_ici_cal_data_car0[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_cal_data_car0[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 WCDMA RXF ICI Cal Data Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_wcdma_rxf_ici_cal_data_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;

  for (loop_ctr=0; loop_ctr<17; loop_ctr++)
  {
    rfnv_tbl_ptr->c1_wcdma_rxf_ici_cal_data_car1[loop_ctr] =
    ((wcdma_nv_item_type*)item_ptr)->c1_wcdma_rxf_ici_cal_data_car1[loop_ctr];
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the WCDMA db10 SAR Back Off Limit NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_wcdma_db10_sar_back_off_limit
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->wcdma_db10_sar_back_off_limit, (sizeof(int16)*RFWCDMA_SAR_BACK_OFF_NV_SIZE),
         item_ptr,(sizeof(int16)*RFWCDMA_SAR_BACK_OFF_NV_SIZE));  
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Ant Tuner NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_ant_tuner
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->ant_tuner,sizeof(rfcommon_nv_ant_tuner_type),
         item_ptr,sizeof(rfcommon_nv_ant_tuner_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Ant Tuner NV for car1 for DBDC.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_ant_tuner_car1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->ant_tuner_car1,sizeof(rfcommon_nv_ant_tuner_type),
         item_ptr,sizeof(rfcommon_nv_ant_tuner_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp UP R1 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_up_r1_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 1 denotes R1 switchpoint */
      rfnv_tbl_ptr->pa_static.pa_comp_up_vs_freq[loop_ctr][1] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_up_r1_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp UP R2 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_up_r2_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 2 denotes R2 switchpoint */
      rfnv_tbl_ptr->pa_static.pa_comp_up_vs_freq[loop_ctr][2] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_up_r2_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp UP R3 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_up_r3_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 3 denotes R3 switchpoint */
      rfnv_tbl_ptr->pa_static.pa_comp_up_vs_freq[loop_ctr][3] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_up_r3_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp DN R1 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_dn_r1_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 1 denotes R1 switchpoint */  
      rfnv_tbl_ptr->pa_static.pa_comp_dn_vs_freq[loop_ctr][1] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_dn_r1_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp DN R2 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_dn_r2_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 2 denotes R2 switchpoint */  
      rfnv_tbl_ptr->pa_static.pa_comp_dn_vs_freq[loop_ctr][2] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_dn_r2_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA Comp DN R3 vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_comp_dn_r3_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 loop_ctr;
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (loop_ctr=0; loop_ctr<NV_FREQ_TABLE_SIZ; loop_ctr++)
    {
      /* Index 3 denotes R3 switchpoint */  
      rfnv_tbl_ptr->pa_static.pa_comp_dn_vs_freq[loop_ctr][3] =
        ((wcdma_nv_item_type*)item_ptr)->pa_comp_dn_r3_vs_freq[loop_ctr];
    }
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX GTR Threshold NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_gtr_thresh
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->tx_gtr_thresh, sizeof(rfwcdma_nv_tx_gtr_thresh_type),(rfwcdma_nv_tx_gtr_thresh_type*)item_ptr, sizeof(rfwcdma_nv_tx_gtr_thresh_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX EPT DPD Config Params NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_ept_dpd_config_params
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->tx_ept_dpd_config_params[0], sizeof(uint32)*MAX_EPT_NUM_CONFIG_PARAMS,
         (uint32*)item_ptr,
         sizeof(uint32)*MAX_EPT_NUM_CONFIG_PARAMS);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the ET Delay Value NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_et_delay_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->et_delay_value = ((wcdma_nv_item_type*)item_ptr)->et_delay_value;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the MTPL vs. Freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_mtpl_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->mtpl_vs_freq[0], (sizeof(int8)*NV_FREQ_TABLE_SIZ), (int8*)item_ptr,(sizeof(int8)*NV_FREQ_TABLE_SIZ));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HDET vs. Temp NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hdet_vs_temp
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->hdet_vs_temp[0], (sizeof(int16)*NV_FREQ_TABLE_SIZ), (int8*)item_ptr,(sizeof(int8)*NV_FREQ_TABLE_SIZ));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the HDET Mod Range NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_hdet_mod_range
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->hdet_mod_range[0], sizeof(int16)*NV_TEMP_TABLE_SIZ, 
          (int8*)item_ptr,(sizeof(int16)*NV_TEMP_TABLE_SIZ));
  memscpy(&rfnv_tbl_ptr->hdet_mod_range[1], sizeof(int16)*NV_TEMP_TABLE_SIZ, 
           (int8*)item_ptr + sizeof(int16)*NV_TEMP_TABLE_SIZ,(sizeof(int16)*NV_TEMP_TABLE_SIZ));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 0 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_0
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
    {
      for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][0] = 
          (int16)(((wcdma_nv_item_type*)item_ptr)->tx_lin_vs_temp_0[temp_index]);
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_offset( rfnv_tbl_ptr,
                                               item_ptr,
                                               0 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_1
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
    {
      for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][1] = 
          (int16)((wcdma_nv_item_type*)item_ptr)->tx_lin_vs_temp_1[temp_index];
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_offset( rfnv_tbl_ptr,
                                               item_ptr,
                                               1 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 2 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
    {
      for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][2] = 
          (int16)((wcdma_nv_item_type*)item_ptr)->tx_lin_vs_temp_2[temp_index];
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_offset( rfnv_tbl_ptr,
                                               item_ptr,
                                               2 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 3 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_3
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
    {
      for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][3] = 
          (int16)((wcdma_nv_item_type*)item_ptr)->tx_lin_vs_temp_3[temp_index];
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_offset( rfnv_tbl_ptr,
                                               item_ptr,
                                               3 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 0 Addl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_0_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;
  int8 *interim_comp_vs_temp_vs_freq;
  /*  int8 interim_comp_vs_temp_vs_freq[NV_FREQ_TABLE_SIZ][NV_TEMP_TABLE_SIZ];*/

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {
    interim_comp_vs_temp_vs_freq = (int8 *) item_ptr;
    
    for (freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
    {
      for (temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][0] += 
          (int16) (*(interim_comp_vs_temp_vs_freq + ((NV_TEMP_TABLE_SIZ * freq_index) + temp_index)));
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta( rfnv_tbl_ptr,
                                                      item_ptr,
                                                      0 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 1 Addl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_1_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;
  int8 *interim_comp_vs_temp_vs_freq;
  /*  int8 interim_comp_vs_temp_vs_freq[NV_FREQ_TABLE_SIZ][NV_TEMP_TABLE_SIZ];*/

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {  
    interim_comp_vs_temp_vs_freq = (int8 *) item_ptr;

    for (freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
    {
      for (temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][1] += 
          (int16) (*(interim_comp_vs_temp_vs_freq + ((NV_TEMP_TABLE_SIZ * freq_index) + temp_index)));
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta( rfnv_tbl_ptr,
                                                      item_ptr,
                                                      1 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 2 Addl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_2_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;
  int8 *interim_comp_vs_temp_vs_freq;
  /*  int8 interim_comp_vs_temp_vs_freq[NV_FREQ_TABLE_SIZ][NV_TEMP_TABLE_SIZ];*/

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {  
    interim_comp_vs_temp_vs_freq = (int8 *) item_ptr;

    for (freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
    {
      for (temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][2] += 
          (int16) (*(interim_comp_vs_temp_vs_freq + ((NV_TEMP_TABLE_SIZ * freq_index) + temp_index)));
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta( rfnv_tbl_ptr,
                                                      item_ptr,
                                                      2 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the TX Lin vs. Temp 3 Addl NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_3_addl
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 temp_index, freq_index;
  int8 *interim_comp_vs_temp_vs_freq;
  /*  int8 interim_comp_vs_temp_vs_freq[NV_FREQ_TABLE_SIZ][NV_TEMP_TABLE_SIZ];*/

  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {    
    interim_comp_vs_temp_vs_freq = (int8 *) item_ptr;

    for (freq_index = 0; freq_index < NV_FREQ_TABLE_SIZ; freq_index++ )
    {
      for (temp_index = 0; temp_index < NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][3] += 
          (int16) (*(interim_comp_vs_temp_vs_freq + ((NV_TEMP_TABLE_SIZ * freq_index) + temp_index)));
      }
    }

    /* Load legacy Lin vs temp data into the common Tx Lin data 
    structure type */
    rfnv_wcdma_load_legacy_lin_vs_temp_vs_freq_delta( rfnv_tbl_ptr,
                                                      item_ptr,
                                                      3 );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx Lin vs. Temp vs. Freq P Out NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_in
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 pa_index_ctr;
  uint16 temp_index, freq_index;

  /* 4PA State Pin-Pout NV copied to PA_Static structure */
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {    
    for (pa_index_ctr=0; pa_index_ctr<RFLIN_NUM_TX_LIN_LEGACY_4PA; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          rfnv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][pa_index_ctr] = 
            ((rfwcdma_nv_tx_lin_vs_temp_vs_freq_type *)item_ptr)->lin_vs_temp_vs_freq[pa_index_ctr][temp_index][freq_index];
        }
      }
    }
  }

  /* Load the Lin vs Temp vs Freq Pin data into the common Tx Lin NV
  data structure */
  rfnv_wcdma_load_lin_vs_temp_vs_freq( rfnv_tbl_ptr, TRUE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx Lin vs. Temp vs. Freq P Out NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_lin_vs_temp_vs_freq_p_out
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 pa_index_ctr;
  uint16 temp_index, freq_index;

  /* 4PA State Pin-Pout NV copied to PA_Static structure */
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {    
    for (pa_index_ctr=0; pa_index_ctr<RFLIN_NUM_TX_LIN_LEGACY_4PA; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          rfnv_tbl_ptr->pa_static.tx_lin_pout[temp_index][freq_index][pa_index_ctr] = 
            ((rfwcdma_nv_tx_lin_vs_temp_vs_freq_type *)item_ptr)->lin_vs_temp_vs_freq[pa_index_ctr][temp_index][freq_index];
        }
      }
    }
  }

  /* Load the Lin vs Temp vs Freq Pin data into the common Tx Lin NV
  data structure */
  rfnv_wcdma_load_lin_vs_temp_vs_freq( rfnv_tbl_ptr, FALSE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the XPT Path Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_xpt_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->xpt_path_delay,sizeof(rfwcdma_nv_xpt_path_delay_type),
         (uint32*)item_ptr,sizeof(rfwcdma_nv_xpt_path_delay_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the XPT path delay offset vs temp vs freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_delay_offsets
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->tx_delay_offsets, sizeof(rfnv_tx_delay_offsets_type),
         (uint32*)item_ptr,sizeof(rfnv_tx_delay_offsets_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Antenna tuner Closed Cloop control info.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_ant_tuner_cl_ctrl_info
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{

  /* Load the Atuner CL Ctrl info */
  if(rfcommon_load_ant_tuner_cl_ctrl_info_from_nv(item_ptr,&rfnv_tbl_ptr->ant_tuner_cl_ctrl_info) == FALSE)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_copy_ant_tuner_cl_ctrl_info() failed !", 0);
  }

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the DPD scaling value.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_dpd_scaling_value
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_item_id_enum_type item_code;

  switch (band)
  {
  case RFCOM_BAND_IMT:
      item_code = RFNV_WCDMA_2100_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_1900:
      item_code = RFNV_WCDMA_1900_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC3:
      item_code = RFNV_WCDMA_1800_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC4:
      item_code = RFNV_WCDMA_BC4_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_800:
      item_code = RFNV_WCDMA_800_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC8:
      item_code = RFNV_WCDMA_900_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC9:
      item_code = RFNV_WCDMA_1800_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC11:
      item_code = RFNV_WCDMA_1500_TX_XPT_DPD_SCALING_I;
      break;
  case RFCOM_BAND_BC19:
      item_code = RFNV_WCDMA_800_TX_XPT_DPD_SCALING_I;
      break;
  default:
      item_code = RFNV_WCDMA_2100_TX_XPT_DPD_SCALING_I;
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfnv_wcdma_copy_dpd_scaling_value"
                                          " can not find band %d", band );
      break;
  }

  rfcommon_xpt_load_dpd_scaling_nv(item_code,
                                   &rfnv_tbl_ptr->dpd_scaling_data,
                                   NULL, 0, NULL);
} /* rfnv_wcdma_copy_dpd_scaling_value */

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Rx multi-carrier static NVs.

  @details
  LNA rise and fall points are being loaded.
  
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_static_data_nv(rfnv_tbl_ptr,item_ptr,band,0);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Rx multi-carrier Calibrated NVs.
  
  @details
   LNA offsets and DVGA offsets are loaded from the NVs.
 
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_cal_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_dynamic_data_nv(rfnv_tbl_ptr,item_ptr,band);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Rx multi-carrier Calibrated NVs V2.
  
  @details
   LNA offsets and DVGA offsets are loaded from the V2 NVs.
 
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_rx_cal_data_v2
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_rx_cal_data_type rx_band_cal_data = { 0 } ;

  /* Load the rx_band_cal_data with NV content.
     Also initialize load_success  */
  rfcommon_populate_rx_gain_vs_freq_v2_nv(&rx_band_cal_data,
                                          item_ptr,
                                          RFM_LNA_GAIN_STATE_NUM);
  rfnv_wcdma_load_rx_dynamic_data_v2_nv(rfnv_tbl_ptr,
                                        &rx_band_cal_data,
                                        band);

  /* Now that the copying is done destroy the allocated memory. */
  if ( rx_band_cal_data.cal_118_data != NULL )
  {
    modem_mem_free ( rx_band_cal_data.cal_118_data, 
                         MODEM_MEM_CLIENT_RFA);
  }

  return;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Noise Floor NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_noise_floor
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->noise_floor = ((wcdma_nv_item_type*)item_ptr)->noise_floor;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Noise Floor Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_noise_floor_car1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->noise_floor_car1 = ((wcdma_nv_item_type*)item_ptr)->noise_floor_car1;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 Noise Floor Car 0 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_noise_floor_car0
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_noise_floor_car0 = ((wcdma_nv_item_type*)item_ptr)->c1_noise_floor_car0;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the C1 Noise Floor Car 1 NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c1_noise_floor_car1
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{
  rfnv_tbl_ptr->c1_noise_floor_car1 = ((wcdma_nv_item_type*)item_ptr)->c1_noise_floor_car1;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA static NV.

  @details
  New common format PA static NV which contains
  [1] PA Range Map
  [2] Rise/Fall switchpoints
  [3] Timer Hystersis
  [4] PA Comp UP/Down
  [5] Tx Rot Angle
  [6] PA Comp Up/Down vs Freq
  [7] Pin/Pout NVs
  [8] Other number of PA range related NVs.
  
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pa_static
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{

  int16 version = (*(rfcommon_nv_cmn_pa_static_type*)item_ptr).header[PA_STATIC_VERSION];

  /* Copy the PA static structure only for PA_STATIC_NV_VER_1 onwards */
  /* Check for valid new PA Static version */
  if( (version < RFWCDMA_NV_PA_STATIC_VER_MAX) && (version > RFWCDMA_NV_LEGACY_FOUR_STATE_PA) )
  {
    memscpy(&rfnv_tbl_ptr->pa_static, sizeof(rfcommon_nv_cmn_pa_static_type),
       (rfcommon_nv_cmn_pa_static_type*)item_ptr, sizeof(rfcommon_nv_cmn_pa_static_type));
  }

  /* Set PA_STATIC NV version */
  rfnv_wcdma_set_pa_static_nv_version(band,rfnv_tbl_ptr->pa_static.header[PA_STATIC_VERSION]);

  /* Update Pin data into the common Tx Lin NV data structure */
  rfnv_wcdma_load_lin_vs_temp_vs_freq( rfnv_tbl_ptr, TRUE );

  /* Update Pout data into the common Tx Lin NV data structure */
  rfnv_wcdma_load_lin_vs_temp_vs_freq( rfnv_tbl_ptr, FALSE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Pin Calibrated vs Freq NV for all PA states.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_pin_cal_vs_freq
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint8 pa_index;
  rfnv_wcdma_pin_cal_vs_freq_type *pin_cal_freq_ptr =
      (rfnv_wcdma_pin_cal_vs_freq_type *)item_ptr;

  /* Initialize the Pin Cal vs freq data structure all PA states*/
  for (pa_index = 0; pa_index < RFCOM_TXLIN_NUM_PA_STATES; pa_index++)
  {
    /* If any one of the pin calibrated value is a default max value,
       then Pin is not calibrated for that PA state */
    if( pin_cal_freq_ptr->pin_cal_vs_freq[pa_index][0] == RFNV_WCDMA_DEFAULT_PIN_CAL_VS_FREQ )
    {
      rfnv_tbl_ptr->lin_temp_comp_data.pin_cal_vs_freq.has_pin_freq_comp_cal_nv[pa_index] = FALSE;
    }

    else
    {
      rfnv_tbl_ptr->lin_temp_comp_data.pin_cal_vs_freq.has_pin_freq_comp_cal_nv[pa_index] = TRUE;
    }

    /* Load the Pin Calibrated vs Freq NV data into the common Tx Lin NV data structure */
    memscpy( 
        &rfnv_tbl_ptr->lin_temp_comp_data.pin_cal_vs_freq.pin[pa_index], (sizeof(int16)*NV_FREQ_TABLE_SIZ), 
        &pin_cal_freq_ptr->pin_cal_vs_freq[pa_index], 
        sizeof(int16)*NV_FREQ_TABLE_SIZ );
  }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the XPT Path Delay NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_xpt_dc_delay
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->xpt_dc_delay,sizeof(rfwcdma_nv_xpt_dc_delay_type),
         (uint32*)item_ptr,sizeof(rfwcdma_nv_xpt_dc_delay_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the MPR BACKOFF NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_mpr_backoff
(
    rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
    rfnv_item_type *item_ptr,
    rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->mpr_backoff,sizeof(rfwcdma_nv_mpr_backoff_type),
         (uint32*)item_ptr,sizeof(rfwcdma_nv_mpr_backoff_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the DC Tx Lin vs. Temp vs. Freq P Out NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_in
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 pa_index_ctr;
  uint16 temp_index;
  uint16 freq_index;

  /* 4PA State Pin-Pout NV copied to PA_Static structure */
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {    
    for (pa_index_ctr=0; pa_index_ctr<RFLIN_NUM_TX_LIN_LEGACY_4PA; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          rfnv_tbl_ptr->dc_pa_static.tx_lin_pin[temp_index][freq_index][pa_index_ctr] = 
            ((rfwcdma_nv_tx_lin_vs_temp_vs_freq_type *)item_ptr)->lin_vs_temp_vs_freq[pa_index_ctr][temp_index][freq_index];
        }
      }
    }
  }

  /* Load the Lin vs Temp vs Freq Pin data into the common Tx Lin NV
  data structure */
  rfnv_wcdma_load_dc_lin_vs_temp_vs_freq( rfnv_tbl_ptr, TRUE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx Lin vs. Temp vs. Freq P Out NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_dc_tx_lin_vs_temp_vs_freq_p_out
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  uint16 pa_index_ctr;
  uint16 temp_index;
  uint16 freq_index;

  /* 4PA State Pin-Pout NV copied to PA_Static structure */
  if(rfnv_wcdma_get_pa_static_nv_version(band) == RFWCDMA_NV_LEGACY_FOUR_STATE_PA)
  {    
    for (pa_index_ctr=0; pa_index_ctr<RFLIN_NUM_TX_LIN_LEGACY_4PA; pa_index_ctr++)
    {
      for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
      {
        for (freq_index=0; freq_index<NV_FREQ_TABLE_SIZ; freq_index++ )
        {
          rfnv_tbl_ptr->dc_pa_static.tx_lin_pout[temp_index][freq_index][pa_index_ctr] = 
            ((rfwcdma_nv_tx_lin_vs_temp_vs_freq_type *)item_ptr)->lin_vs_temp_vs_freq[pa_index_ctr][temp_index][freq_index];
        }
      }
    }
  }

  /* Load the Lin vs Temp vs Freq Pin data into the common Tx Lin NV
  data structure */
  rfnv_wcdma_load_dc_lin_vs_temp_vs_freq( rfnv_tbl_ptr, FALSE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the XPT path delay offset vs temp vs freq NV.

  @details

  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_tx_dc_delay_offsets
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  memscpy(&rfnv_tbl_ptr->tx_dc_delay_offsets, sizeof(rfnv_tx_delay_offsets_type),
         (uint32*)item_ptr,sizeof(rfnv_tx_delay_offsets_type));
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the PA static NV.

  @details
  New common format PA static NV which contains
  [1] PA Range Map
  [2] Rise/Fall switchpoints
  [3] Timer Hystersis
  [4] PA Comp UP/Down
  [5] Tx Rot Angle
  [6] PA Comp Up/Down vs Freq
  [7] Pin/Pout NVs
  [8] Other number of PA range related NVs.
  
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/

void rfnv_wcdma_copy_dc_pa_static
(
   rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
   rfnv_item_type *item_ptr,
   rfcom_wcdma_band_type band
)
{

  int16 version = (*(rfcommon_nv_cmn_pa_static_type*)item_ptr).header[PA_STATIC_VERSION];

  /* Copy the PA static structure only for PA_STATIC_NV_VER_1 onwards */
  /* Check for valid new PA Static version */
  if( (version < RFWCDMA_NV_PA_STATIC_VER_MAX) && (version > RFWCDMA_NV_LEGACY_FOUR_STATE_PA) )
  {
    memscpy(&rfnv_tbl_ptr->dc_pa_static, sizeof(rfcommon_nv_cmn_pa_static_type),
       (rfcommon_nv_cmn_pa_static_type*)item_ptr, sizeof(rfcommon_nv_cmn_pa_static_type));
  }

  /* Set PA_STATIC NV version */
  rfnv_wcdma_set_dc_pa_static_nv_version(band,rfnv_tbl_ptr->dc_pa_static.header[PA_STATIC_VERSION]);

  /* Update Pin data into the common Tx Lin NV data structure */
  rfnv_wcdma_load_dc_lin_vs_temp_vs_freq( rfnv_tbl_ptr, TRUE );

  /* Update Pout data into the common Tx Lin NV data structure */
  rfnv_wcdma_load_dc_lin_vs_temp_vs_freq( rfnv_tbl_ptr, FALSE );

}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Rx multi-carrier static NVs for Rx chain 0.

  @details
  LNA rise and fall points are being loaded.
  
  @param rfnv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param item_ptr
  Pointer to the NV item to be stored
 
  @param band RFCOM WCDMA band that the NV item is for
 
*/
void rfnv_wcdma_copy_c0_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_static_data_nv(rfnv_tbl_ptr,item_ptr,band,RFM_DEVICE_0);
}

void rfnv_wcdma_copy_c1_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_static_data_nv(rfnv_tbl_ptr,item_ptr,band,RFM_DEVICE_1);
}
void rfnv_wcdma_copy_c2_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_static_data_nv(rfnv_tbl_ptr,item_ptr,band,RFM_DEVICE_2);
}
void rfnv_wcdma_copy_c3_rx_static_data
(
  rfnv_wcdma_nv_tbl_type *rfnv_tbl_ptr,
  rfnv_item_type *item_ptr,
  rfcom_wcdma_band_type band
)
{
  rfnv_wcdma_load_rx_static_data_nv(rfnv_tbl_ptr,item_ptr,band,RFM_DEVICE_3);
}
