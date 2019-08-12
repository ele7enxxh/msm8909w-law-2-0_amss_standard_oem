/*!
 @file rf_tdscdma_core_util.c

 @brief
  This file has a collection of utility functions for TDSCDMA technology.

 @details

*/

/*===========================================================================

Copyright (c) 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_util.c#1 $

When       who     what, where, why
-------------------------------------------------------------------------------
05/08/16   jr      Reduce F3 messages
10/03/14   jyu     Better handling for split band
05/29/14   ych     Updated for 8 PA gain states
04/08/14   jhe     Add is_band_supported api and file cleanup
10/23/13   qzh     Fix KW warnings
09/18/13   ms      Add api for getting filtered band
08/22/13   jr      Removed API rf_tdscdma_core_util_calc_tx_cal_chan_freq()
08/14/13   al      Added split band support for get_mask
05/20/13   al      Added support for split band
04/05/13   kg      Added new API to get the band no. from the rfcom band type
02/07/13   kg      Adding new util functions for band mask and max/min freq
12/05/12   jyu     Provided a work-around to fix freq comp bug when calculate max
                   power limit
09/24/12   jyu     Added support to get PA_STATE from PA_RANGE
05/02/12   ycl     NV max power change to RF AGC unit (930).
05/27/11   jhe     initial version.

============================================================================*/

#include "rf_tdscdma_core_util.h"
#include "amssassert.h"
#include "rfc_bsp.h"
#include "DDITlmm.h"
#include "rfd_grfc.h"
#include "rfc_card_tdscdma.h"
#include "rf_tdscdma.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_core_txplim.h"
#include "rfc_common.h"
#include "rfcommon_core.h"

static boolean rf_tdscdma_core_util_f3_reduction = TRUE;

/*=============================================================================
                             DATA DEFINITIONS
=============================================================================*/
/*TDSCDMA channel raster in kHz.*/
#define RF_TDSCDMA_CHAN_RASTER_IN_KHZ 200

/*MINIMUM CHANNEL NUMBER FOR EACH BAND*/
#define RF_TDSCDMA_B34_MIN_CHAN    	10054
#define RF_TDSCDMA_B39_MIN_CHAN    	9404
#define RF_TDSCDMA_B40_MIN_CHAN    	11504
#define RF_TDSCDMA_B40_B_MIN_CHAN   11801

/*MAXIMUM CHANNEL NUMBER FOR EACH BAND*/
#define RF_TDSCDMA_B34_MAX_CHAN    	10121
#define RF_TDSCDMA_B39_MAX_CHAN    	9596
#define RF_TDSCDMA_B40_MAX_CHAN    	11996

/*LOWEST FREQUENCY FOR EACH BAND IN kHz*/
#define RF_TDSCDMA_B34_LOW_FREQ 	2010000
#define RF_TDSCDMA_B39_LOW_FREQ 	1880000
#define RF_TDSCDMA_B40_LOW_FREQ 	2300000

/*HIGHEST FREQUENCY FOR EACH BAND IN kHz*/
#define RF_TDSCDMA_B34_HIGH_FREQ 	2025000
#define RF_TDSCDMA_B39_HIGH_FREQ 	1920000
#define RF_TDSCDMA_B40_HIGH_FREQ 	2400000

//TDSCDMA Band mask definitions
#define RF_TDSCDMA_B34_MASK_IDX  0
#define RF_TDSCDMA_B39_MASK_IDX  1
#define RF_TDSCDMA_B40_MASK_IDX  2

#define RF_TDSCDMA_B34_MASK  (uint64)(1<<(uint64)(RF_TDSCDMA_B34_MASK_IDX))
#define RF_TDSCDMA_B39_MASK  (uint64)(1<<(uint64)(RF_TDSCDMA_B39_MASK_IDX))
#define RF_TDSCDMA_B40_MASK  (uint64)(1<<(uint64)(RF_TDSCDMA_B40_MASK_IDX))

uint32 rf_tdscdma_core_tx_cal_chan_freq[RF_TDSCDMA_MAX_CAL_CHAN];

boolean rf_tdscdma_tx_freq_comp_debug_flag = FALSE;

extern rf_tdscdma_core_txlin_data_type rf_tdscdma_core_txlin_data;


/*----------------------------------------------------------------------------*/
/*!

 @brief
  This function returns the DL/UL frequency in Khz based on the
  downlink/uplink channel number passed in as an argument for TDSCDMA.

 @details
  The downlink/uplink channel numbers do not overlap; hence the band
  information is not needed but it is already passed by L1(??).
  Please note that the channel raster is 200kHz, which means the
  carrier frequency must be a multiple of 200kHz

  Desired LO frequency in Khz unit for Downlink in uint32.
  (eg: for 2110Mhz, it returns 2110000).

 @param
  chan: Downlink/Uplink channel number.
  band: Band that the chan in argument 1 belongs to.

*/
uint32 rf_tdscdma_core_util_get_freq_from_uarfcn(uint16 chan, rfcom_tdscdma_band_type band)
{
  uint32 freq = 0;

  switch (band)
  {
    case RFCOM_BAND_TDSCDMA_B34:
     case RFCOM_BAND_TDSCDMA_B34_B:
	  if( (chan >= RF_TDSCDMA_B34_MIN_CHAN) && (chan <= RF_TDSCDMA_B34_MAX_CHAN) )
	  {
		freq = chan * RF_TDSCDMA_CHAN_RASTER_IN_KHZ;
	  }
      else
      {
        MSG_ERROR("Invalid TDSCDMA chan %d for Band %d", chan, band, 0);
      }
	  break;

    case RFCOM_BAND_TDSCDMA_B39:
     case RFCOM_BAND_TDSCDMA_B39_B:
	  if( (chan >= RF_TDSCDMA_B39_MIN_CHAN) && (chan <= RF_TDSCDMA_B39_MAX_CHAN) )
	  {
		freq = chan * RF_TDSCDMA_CHAN_RASTER_IN_KHZ;
	  }
      else
      {
        MSG_ERROR("Invalid TDSCDMA chan %d for Band %d", chan, band, 0);
      }
	  break;

    case RFCOM_BAND_TDSCDMA_B40:
	case RFCOM_BAND_TDSCDMA_B40_B:
	  if( (chan >= RF_TDSCDMA_B40_MIN_CHAN) && (chan <= RF_TDSCDMA_B40_MAX_CHAN) )
	  {
		freq = chan * RF_TDSCDMA_CHAN_RASTER_IN_KHZ;
	  }
      else
      {
        MSG_ERROR("Invalid TDSCDMA chan %d for Band %d", chan, band, 0);
      }
	  break;

    default :
      MSG_ERROR("Invalid RF Band for TDSCDMA = %d", band, 0, 0);
      break;
  }

  return (freq);

} /*rf_tdscdma_core_get_freq_from_uarfcn*/


/*----------------------------------------------------------------------------*/
/*!

 @brief
  This function returns the DL/UL channel based on the
  frequency(KHz) passed in as an argument for TDSCDMA.

 @details
  The downlink/uplink channel numbers do not overlap; hence the band
  information is not needed but it is already passed by L1(??).
  Please note that the channel raster is 200kHz, which means the
  carrier frequency must be a multiple of 200kHz

 @param
  freq: Frequency (KHz)
  band: Band that the frequency in argument 1 belongs to.

*/
uint16 rf_tdscdma_core_util_get_uarfcn_from_freq(uint32 freq, rfcom_tdscdma_band_type band)
{
  uint16 chan = 0xFFFF;

  if( (freq % RF_TDSCDMA_CHAN_RASTER_IN_KHZ) != 0 )
  {
    MSG_ERROR("Invalid carrier freq for TDSCDMA = %d", freq, 0, 0);
  }

  chan = freq/RF_TDSCDMA_CHAN_RASTER_IN_KHZ;

  return (chan);

} /*rf_tdscdma_core_get_uarfcn_from_freq*/


/*----------------------------------------------------------------------------*/
/*!

 @brief
  This function returns the band of a DL/UL channel number.

 @details
  Since in TDSCDMA the channel numbers of different bands
  do not coincide, we can return the band of a given channel.

 @param
  chan: channel number.

*/
rfcom_tdscdma_band_type rf_tdscdma_core_util_get_band_from_uarfcn(uint16 chan)
{
  rfcom_tdscdma_band_type band = RFCOM_BAND_TDSCDMA_INVALID;
  rfcom_tdscdma_band_type rfc_band = RFCOM_BAND_TDSCDMA_INVALID;
  boolean split_band = FALSE;

  if ( (RF_TDSCDMA_B34_MIN_CHAN <= chan) && (chan <=  RF_TDSCDMA_B34_MAX_CHAN) )
  {
    band = RFCOM_BAND_TDSCDMA_B34;
  }
  else if ( (RF_TDSCDMA_B39_MIN_CHAN <= chan) && (chan <=  RF_TDSCDMA_B39_MAX_CHAN) )
  {
    band = RFCOM_BAND_TDSCDMA_B39;
  }
  else if ( (RF_TDSCDMA_B40_MIN_CHAN <= chan) && (chan <=  RF_TDSCDMA_B40_MAX_CHAN) )
  {
    band = RFCOM_BAND_TDSCDMA_B40;
  }
  else
  {
    band = RFCOM_BAND_TDSCDMA_INVALID;
	MSG_ERROR("Invalid Channel Number for TDSCDMA = %d", chan, 0, 0);
  }

  /* Check if it is a split band */
  rfc_band = band;
  rfc_tdscdma_get_tx_band(&split_band, chan, &rfc_band);
  if((split_band == TRUE) && (band > RFCOM_BAND_TDSCDMA_B40))
  //if(split_band == TRUE)
  {
    band = rfc_band;
  }

  return band;
}

/*----------------------------------------------------------------------------*/
/*!

 @brief
  This function returns the filtererd/unfiltered band

 @details
 
 
 @param
  band: band number.

*/
rfcom_tdscdma_band_type rf_tdscdma_core_util_get_filtered_band(rfcom_tdscdma_band_type band)
{
  rfcom_tdscdma_band_type rfc_band = RFCOM_BAND_TDSCDMA_INVALID;
  
  rfc_coex_filter_status_type filter_status = RFC_FILTERED_PATH_FALSE;

  filter_status= rfcommon_core_get_coex_filter_status(RFM_EGSM_MODE);

  if (FALSE == rf_tdscdma_core_util_f3_reduction)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_MED," DEBUG: rf_tdscdma_core_util_get_filtered_band: initial TDSCDMA band %d", band);
  }

  if (band == RFCOM_BAND_TDSCDMA_ANY_BAND)
  {
    return (band);
  }

  if (filter_status == RFC_FILTERED_PATH_TRUE)
  {
    rfc_tdscdma_get_filtered_band(band, filter_status, &rfc_band);
    if (rfc_band == RFCOM_BAND_TDSCDMA_INVALID)
    {
      MSG_3(MSG_SSID_RF, MSG_LVL_MED,"rf_tdscdma_core_util_get_filtered_band: TDSCDMA rfc_band %d, band=%d, filter_status= %d", rfc_band, band, filter_status);
      return (band);
    }
    else
    {  
      MSG_2(MSG_SSID_RF, MSG_LVL_MED,"rf_tdscdma_core_util_get_filtered_band: TDSCDMA rfc_band %d and filter_status= %d", rfc_band, filter_status);
      return (rfc_band);
    }      
  }
  else
  { 
    //MSG_1(MSG_SSID_RF, MSG_LVL_MED,"rf_tdscdma_core_util_get_filtered_band:  TDSCDMA band %d", band);
    return (band);
  }

}

/*----------------------------------------------------------------------------*/
/*!

 @brief
  This function returns the DL/UL frequency in Khz based on the
  downlink/uplink channel number passed in as an argument for TDSCDMA.

*/
uint32 rf_tdscdma_core_util_get_freq_from_chan(uint16 chan)
{
  return ( rf_tdscdma_core_util_get_freq_from_uarfcn(chan,
                                                     rf_tdscdma_core_util_get_band_from_uarfcn(chan)) );
}

/*!

 @brief
  To surpress CW warning

*/
uint32 rf_tdscdma_core_util_get_freq_from_chan_common_api(uint32 chan)
{
  return ( rf_tdscdma_core_util_get_freq_from_chan((uint16) chan) );
}



/*----------------------------------------------------------------------------*/
/*!
  @details
  This function returns Rx band specific NV table poniter

  @param band TDSCDMA operational band

  @return rfnv_tdscdma_rx_tbl_type pointer
*/
rfnv_tdscdma_rx_tbl_type *rf_tdscdma_core_util_init_rx_nv_table(rfcom_tdscdma_band_type band)
{
  rfnv_tdscdma_rx_tbl_type *rfnv_ptr=NULL;

  switch ( band )
  {
     case RFCOM_BAND_TDSCDMA_B34:
     case RFCOM_BAND_TDSCDMA_B34_B:
      rfnv_ptr = rfnv_tdscdma_get_rx_tbl_ptr(RFNV_TDSCDMA_B34);
      break;

    case RFCOM_BAND_TDSCDMA_B39:
    case RFCOM_BAND_TDSCDMA_B39_B:
      rfnv_ptr = rfnv_tdscdma_get_rx_tbl_ptr(RFNV_TDSCDMA_B39);
      break;

    case RFCOM_BAND_TDSCDMA_B40:
	  case RFCOM_BAND_TDSCDMA_B40_B:
      rfnv_ptr = rfnv_tdscdma_get_rx_tbl_ptr(RFNV_TDSCDMA_B40);
      break;

    default:
      MSG_ERROR( "Invalid band=%d. TDSCDMA Rx NV table initialization failed.", band, 0, 0 );
  }

  return(rfnv_ptr);
}


/*----------------------------------------------------------------------------*/
/*!
  @details
  This function returns Tx band specific NV table poniter

  @param band TDSCDMA operational band

  @return rfnv_tdscdma_tx_tbl_type pointer
*/
rfnv_tdscdma_tx_tbl_type *rf_tdscdma_core_util_init_tx_nv_table(rfcom_tdscdma_band_type band)
{
  rfnv_tdscdma_tx_tbl_type *rfnv_ptr=NULL;

  switch ( band )
  {
  case RFCOM_BAND_TDSCDMA_B34:
    case RFCOM_BAND_TDSCDMA_B34_B:
      rfnv_ptr = rfnv_tdscdma_get_tx_tbl_ptr(RFNV_TDSCDMA_B34);
      break;

  case RFCOM_BAND_TDSCDMA_B39:
    case RFCOM_BAND_TDSCDMA_B39_B:
      rfnv_ptr = rfnv_tdscdma_get_tx_tbl_ptr(RFNV_TDSCDMA_B39);
      break;

    case RFCOM_BAND_TDSCDMA_B40:
      rfnv_ptr = rfnv_tdscdma_get_tx_tbl_ptr(RFNV_TDSCDMA_B40);
      break;

	case RFCOM_BAND_TDSCDMA_B40_B:
      rfnv_ptr = rfnv_tdscdma_get_tx_tbl_ptr(RFNV_TDSCDMA_B40_B);
      break;

    default:
      MSG_ERROR( "Invalid band=%d. TDSCDMA Tx NV table initialization failed.", band, 0, 0 );
  }

  return(rfnv_ptr);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the PA range map from RF NV.

  @details

  @param PA index value

*/
uint8 rf_tdscdma_core_util_get_pa_range_from_nv(uint8 pa_index)
{
  return(rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_range_map[pa_index]);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the NV index for a given PA range form the PA range map RF NV.

  @details

  @param PA range value.

*/
uint8 rf_tdscdma_core_util_get_pa_range_map_nv_index(rfcom_tdscdma_band_type band, uint8 pa_range)
{
  uint8 pa_index;
  rfnv_tdscdma_tx_tbl_type* rfnv_tdscdma_tx_tbl_ptr_prev = rfnv_tdscdma_tx_tbl_ptr;

  if(band < RFCOM_NUM_TDSCDMA_BANDS)
  {
    rfnv_tdscdma_tx_tbl_ptr = rf_tdscdma_core_util_init_tx_nv_table(band);

    if(rfnv_tdscdma_tx_tbl_ptr != NULL)
    {
      for(pa_index=0; pa_index<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index++)
      {
        if(pa_range == rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_range_map[pa_index])
        {
          return pa_index;
        }
      }
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "rfnv_tdscdma_tx_tbl_ptr is NULL for Band %d! Skipped get_pa_range_map_nv_index", band);
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "Wrong band %d passed into query for PA_STATE", band);
  }

  rfnv_tdscdma_tx_tbl_ptr = rfnv_tdscdma_tx_tbl_ptr_prev;

  /* PA range not found */
  MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "PA range not found in the pa_range_map NV", 0);
  return (0xFF);
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the PA compensation UP NV value.

  @details

  @param PA index value

*/
int16 rf_tdscdma_core_util_get_pa_comp_up_nv(uint8 pa_index)
{
  if ( pa_index<RFCOMMON_NV_MAX_PA_STATE ) // KW...
  {
    return(rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_comp_up[pa_index]);
  }
  else
  {
    return 0;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns the PA compensation Down NV value.

  @details

  @param PA index value

*/
int16 rf_tdscdma_core_util_get_pa_comp_down_nv(uint8 pa_index)
{
  if ( pa_index<RFCOMMON_NV_MAX_PA_STATE )
  {
    return(rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_comp_dn[pa_index]);
  }
  else
  {
    return 0;
  }
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return TX AGC value in db10 unit based on the passed in dbm unit value.

  @details

  @param dbmval

  @return int16 TX AGC value in db10 unit.
*/
int16 rf_tdscdma_core_util_convert_tx_dbm_to_db10_unit( int16 dbmval )
{
  return (int16) ( (dbmval-RF_TDSCDMA_MIN_TX_POWER_DBM)*RF_TDSCDMA_TXAGC_RESOLUTION );
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Returns TX power value in dbm unit based on the passed in txagcval value in
  db10 unit.

  @details

  @param txagcval Tx AGC value

  @return int16 TX AGC value in db10 unit.

*/
int16 rf_tdscdma_core_util_convert_tx_agc_to_dbm_unit( int16 txagcval )
{
  return ( (txagcval / RF_TDSCDMA_TXAGC_RESOLUTION) + RF_TDSCDMA_MIN_TX_POWER_DBM);
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function is a helper function to convert from rfcom_tdscdma_band_type
  to rf_card_band_type.

  @details

*/
rf_card_band_type rf_tdscdma_core_util_convert_band_rfcom_to_rfc(rfcom_tdscdma_band_type rfcom_band)
{
  /*initialize the rfc_band variable.*/
  rf_card_band_type rfc_band = RF_MAX_BAND;

  switch ( rfcom_band )
  {
    case RFCOM_BAND_TDSCDMA_B34:
      rfc_band = RF_TDSCDMA_B34_BAND;
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      rfc_band = RF_TDSCDMA_B39_BAND;
      break;

      case RFCOM_BAND_TDSCDMA_B34_B:
      rfc_band = RF_TDSCDMA_B34_B_BAND;
      break;

    case RFCOM_BAND_TDSCDMA_B39_B:
      rfc_band = RF_TDSCDMA_B39_B_BAND;
      break;

    case RFCOM_BAND_TDSCDMA_B40:
      rfc_band = RF_TDSCDMA_B40_BAND;
      break;

	case RFCOM_BAND_TDSCDMA_B40_B:
      rfc_band = RF_TDSCDMA_B40_B_BAND;
      break;

    default:
      MSG_ERROR( "Invalid rfcom band=%d in rf_tdscdma_core_tdscdma_band_helper().", rfcom_band, 0, 0 );
      break;
  }

  return rfc_band;
}


/* ----------------------------------------------------------------------- */
/*!
  @brief
  This function is a helper function to convert from rf_card_band_type to
  rfcom_tdscdma_band_type.

  @details

*/
rfcom_tdscdma_band_type rf_tdscdma_core_util_convert_band_rfc_to_rfcom(rf_card_band_type rfc_band)
{
  rfcom_tdscdma_band_type rfcom_band = RFCOM_BAND_TDSCDMA_INVALID;

  switch ( rfc_band )
  {
    case RF_TDSCDMA_B34_BAND:
      rfcom_band = RFCOM_BAND_TDSCDMA_B34;
      break;

    case RF_TDSCDMA_B39_BAND:
      rfcom_band = RFCOM_BAND_TDSCDMA_B39;
      break;

  case RF_TDSCDMA_B34_B_BAND:
    rfcom_band = RFCOM_BAND_TDSCDMA_B34;
    break;

  case RF_TDSCDMA_B39_B_BAND:
    rfcom_band = RFCOM_BAND_TDSCDMA_B39;
    break;


    case RF_TDSCDMA_B40_BAND:
      rfcom_band = RFCOM_BAND_TDSCDMA_B40;
      break;

	case RF_TDSCDMA_B40_B_BAND:
      rfcom_band = RFCOM_BAND_TDSCDMA_B40_B;
      break;

    default:
      MSG_ERROR( "Invalid rfc band=%d in rf_tdscdma_core_convert_rfc_band_to_rfcom_band().", rfc_band, 0, 0 );
      break;
  }

  return rfcom_band;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the min and max Rx freq (in KHz) of a TDSCDMA band

  @details

  @return
  boolean TRUE if band is supported, FALSE otherwise
*/
boolean rf_tdscdma_core_util_get_min_max_rx_freqs(rfcom_tdscdma_band_type band, uint32* min, uint32* max)
{
  boolean ret_val = TRUE;
  uint16 low_earfcn = 0, high_earfcn = 0;

  switch(band)
  {
  case RFCOM_BAND_TDSCDMA_B34:
    *min = RF_TDSCDMA_B34_LOW_FREQ;
    *max = RF_TDSCDMA_B34_HIGH_FREQ;
    break;

  case RFCOM_BAND_TDSCDMA_B39:
    *min = RF_TDSCDMA_B39_LOW_FREQ;
    *max = RF_TDSCDMA_B39_HIGH_FREQ;
    break;

  case RFCOM_BAND_TDSCDMA_B40:
    *min = RF_TDSCDMA_B40_LOW_FREQ;
    *max = RF_TDSCDMA_B40_HIGH_FREQ;
    break;

  case RFCOM_BAND_TDSCDMA_B40_B:
    rfc_tdscdma_get_split_band_earfcns(band, &low_earfcn, &high_earfcn);
    *min = rf_tdscdma_core_util_get_freq_from_chan(low_earfcn);
    *max = rf_tdscdma_core_util_get_freq_from_chan(high_earfcn);
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LVL_MED,"rf_tdscdma_core_util_get_min_max_rx_freqs: TDSCDMA band %d NOT SUPPORTED!", band);
    ret_val = FALSE;
  } // end switch
  return ret_val;
} // cmn_utils_get_min_max_rx_freqs for TDSCDMA

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the TDSCDMA mask

  @return
  uint8 TDSCDMA mask
*/
uint64 rf_tdscdma_core_util_get_mask(rfcom_tdscdma_band_type band)
{
  uint64 ret_val = 0;

  switch(band)
  {
  case  RFCOM_BAND_TDSCDMA_B34:
   case  RFCOM_BAND_TDSCDMA_B34_B:
    ret_val = RF_TDSCDMA_B34_MASK;
    break;

  case  RFCOM_BAND_TDSCDMA_B39:
  case  RFCOM_BAND_TDSCDMA_B39_B:
    ret_val = RF_TDSCDMA_B39_MASK;
    break;

  case  RFCOM_BAND_TDSCDMA_B40:
  case  RFCOM_BAND_TDSCDMA_B40_B:
    ret_val = RF_TDSCDMA_B40_MASK;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LVL_MED,"get_tdscdma_band_mask: Invalid TDSCDMA band %d!", band);
    break;
  }

  return ret_val;
} // cmn_utils_get_sys_band_mask - TDSCDMA

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function gets the TDSCDMA band num from rfcom band type

  @return
  boolean
*/
boolean rf_tdscdma_core_util_get_band_num(rfcom_tdscdma_band_type band,uint16* num)
{
  uint64 ret_val = 0;

  switch(band)
  {
  case  RFCOM_BAND_TDSCDMA_B34:
    *num = 34;
    break;

  case  RFCOM_BAND_TDSCDMA_B39:
    *num = 39;
    break;

  case  RFCOM_BAND_TDSCDMA_B40:
    *num = 40;
    break;

  default:
    MSG_1(MSG_SSID_RF, MSG_LVL_MED,"rf_tdscdma_core_util_get_band_num: Invalid TDSCDMA band %d!", band);
    break;
  }

  return ret_val;
} // rf_tdscdma_core_util_get_band_num - TDSCDMA


/*!
 @brief
  This function returns the full band if input is split band else just returns same band.

 @details
  In case of split bands, NS is not aware of split band so we need to pass just the full band

  @param
   tdscdma rfcom band
*/

void rf_tdscdma_core_get_fullband_from_subband(rfcom_tdscdma_band_type *tdscdma_rfcom_band)
{
	if (*tdscdma_rfcom_band == RFCOM_BAND_TDSCDMA_B40_B)
		*tdscdma_rfcom_band = RFCOM_BAND_TDSCDMA_B40;
	return;
}


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Check if the band is supported or not

  @details

*/
boolean rf_tdscdma_core_util_is_band_supported(rfcom_tdscdma_band_type band, rf_path_enum_type path)
{
  //to be fully implemented
  if (band < RFCOM_NUM_TDSCDMA_BANDS)
  {
    return TRUE;
  }

  return FALSE;
}

