/*!
  @file
  rfnv_tdscdma.c

  @brief

  @details
  This module retrieves all NV items that will be used by the TDSCDMA RF unit.

*/

/*===========================================================================

Copyright (c) 2008, 2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/nv/src/rfnv_tdscdma.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/14   jps     Updates for get PA switchpoints API
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
09/11/14   ych     Unregistered rfnv_tdscdma_b40_b related API pointers
09/10/14   ych     Disabled rfnv_tdscdma_b40.c and made corresponding compilation/cw/kw changes
06/30/14   jhe     Init the pinpout flag in the new pa static api
06/19/14   ych     Added temp bins sorting
05/29/14   ych     Updated for 8 PA gain states   
05/04/14   jhe     KW fix
05/20/13   al      Add rfnv support for split band
04/19/11   hy      Initial Revision.

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfcommon_nv.h"
#include "rfnv_tdscdma.h"
#include "rfnv_tdscdma_b34.h"
#include "rfnv_tdscdma_b39.h"
#include "rfnv_tdscdma_b40.h"
#include "rfnv_tdscdma_b40_b.h"
#include "msg.h" 
#include "err.h"
#include "rfcommon_nv_mm.h"
#include "rf_tdscdma_core_util.h"
/* memscpy */
#include "stringl.h"

/*===========================================================================
                        
                           CONSTANT DEFINITIONS

===========================================================================*/


/*===========================================================================
                        
                           DATA DEFINITIONS

===========================================================================*/

/*===========================================================================
                        
                        FUNCTION DEFINITIONS

===========================================================================*/

rfnv_tdscdma_rx_tbl_type   *rfnv_tdscdma_rx_tbl_ptr;
rfnv_tdscdma_tx_tbl_type   *rfnv_tdscdma_tx_tbl_ptr;

rfcommon_rfnv_info_type rfnv_tdscdma_supported_band_info_table[] =
{
  { 
    rfnv_tdscdma_b34_rx_items_list,
	  rfnv_tdscdma_b34_rx_item_table_get,
    rfnv_tdscdma_b34_rx_list_size_get,
    TRUE,
    rfnv_tdscdma_b34_copy_rx_item
  },

  { 
    rfnv_tdscdma_b34_tx_items_list,
    rfnv_tdscdma_b34_tx_item_table_get,
    rfnv_tdscdma_b34_tx_list_size_get,
    TRUE,
    rfnv_tdscdma_b34_copy_tx_item
  },

  { 
    rfnv_tdscdma_b39_rx_items_list,
    rfnv_tdscdma_b39_rx_item_table_get,
    rfnv_tdscdma_b39_rx_list_size_get,
    TRUE,
    rfnv_tdscdma_b39_copy_rx_item
  },
       
  { 
    rfnv_tdscdma_b39_tx_items_list,
    rfnv_tdscdma_b39_tx_item_table_get,
    rfnv_tdscdma_b39_tx_list_size_get,
    TRUE,
    rfnv_tdscdma_b39_copy_tx_item
  },

//  { 
//    rfnv_tdscdma_b40_rx_items_list,
//    rfnv_tdscdma_b40_rx_item_table_get,
//    rfnv_tdscdma_b40_rx_list_size_get,
//    TRUE,
//    rfnv_tdscdma_b40_copy_rx_item
//  },
//                 
//  { 
//    rfnv_tdscdma_b40_tx_items_list,
//    rfnv_tdscdma_b40_tx_item_table_get,
//    rfnv_tdscdma_b40_tx_list_size_get,
//    TRUE,
//    rfnv_tdscdma_b40_copy_tx_item
//  },
  
//  { 
//    rfnv_tdscdma_b40_b_tx_items_list,
//    rfnv_tdscdma_b40_b_tx_item_table_get,
//    rfnv_tdscdma_b40_b_tx_list_size_get,
//    TRUE,
//    rfnv_tdscdma_b40_b_copy_tx_item
//  },

};

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initializes MSM configuration based on RF HW.

  @details
  Returns pointer to TDSCDMA RF NV table for Rx items.

  @param band Supported band

  @return rfnv_tdscdma_rx_tbl_type TDSCDMA NV calibration table pointer
  
*/

rfnv_tdscdma_rx_tbl_type *rfnv_tdscdma_get_rx_tbl_ptr( rfnv_tdscdma_supported_band_type band )
{
  rfnv_tdscdma_rx_tbl_type *nv_tbl_ptr = NULL;

  /*lint -e787 not all enum's used in switch */
  switch (band)
  {
    case RFNV_TDSCDMA_B34:
      nv_tbl_ptr = &rfnv_tdscdma_b34_rx_tbl;
      break;
    
    case RFNV_TDSCDMA_B39:
	  nv_tbl_ptr = &rfnv_tdscdma_b39_rx_tbl;
	  break;

    case RFNV_TDSCDMA_B40:
	  //nv_tbl_ptr = &rfnv_tdscdma_b40_rx_tbl;
	  break;

    default:
        ERR_FATAL("Invalid band=%d. Cannot initialize TDSCDMA Rx NV table", (int)band, 0, 0);
  }
  /*lint +e787 */

  return nv_tbl_ptr;
} 

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Initializes MSM configuration based on RF HW.

  @details
  Returns pointer to TDSCDMA RF NV table for Tx items.

  @param band Supported band

  @return rfnv_tdscdma_tx_tbl_type TDSCDMA NV calibration table pointer
  
*/

rfnv_tdscdma_tx_tbl_type *rfnv_tdscdma_get_tx_tbl_ptr( rfnv_tdscdma_supported_band_type band )
{
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr = NULL;

  /*lint -e787 not all enum's used in switch */
  switch (band)
  {
    case RFNV_TDSCDMA_B34:
      nv_tbl_ptr = &rfnv_tdscdma_b34_tx_tbl;
      break;
    
    case RFNV_TDSCDMA_B39:
  	  nv_tbl_ptr = &rfnv_tdscdma_b39_tx_tbl;
	    break;

    case RFNV_TDSCDMA_B40:
	    //nv_tbl_ptr = &rfnv_tdscdma_b40_tx_tbl;
  	  break;
	  
	case RFNV_TDSCDMA_B40_B:
	    //nv_tbl_ptr = &rfnv_tdscdma_b40_b_tx_tbl;
  	  break;

    default:
        ERR_FATAL("Invalid band=%d. Cannot initialize TDSCDMA Tx NV table", (int)band, 0, 0);
  }
  /*lint +e787 */

  return nv_tbl_ptr;
} 

/*----------------------------------------------------------------------------*/
/*!
  @brief
  retrieve all nv items for all supported bands

  @details
  This function will retrieve all nv items for all supported band thru NV
  interface, and place them in different table for each band

  @param band supported band

  @return rfnv_tdscdma_tbl_type TDSCDMA NV calibration table pointer
  
*/
rfcommon_nv_status_type rfnv_tdscdma_retrieve_items
(
rex_tcb_type *task_ptr,                     //!< pointer to tcb of the calling task
rex_sigs_type task_wait_sig,                //!< signal that calling task waiting for
void (*task_wait_handler)( rex_sigs_type )  //!< function ptr to callback function
)
{
    rfcommon_nv_status_type status = RFCOMMON_NV_READ_SUCCESS;
    int16 rfnv_tdscdma_nv_tbl_size = sizeof(rfnv_tdscdma_supported_band_info_table) / sizeof(rfcommon_rfnv_info_type);

    status = rfcommon_rfnv_retrieve_items((rfcommon_rfnv_info_type *)&rfnv_tdscdma_supported_band_info_table[0], 
             rfnv_tdscdma_nv_tbl_size, 
             task_ptr, 
             task_wait_sig, 
             task_wait_handler);

    return status;
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function converts rfcom_tdscdma_band_type to rfnv_tdscdma_supported_band_type.

  @param band rfcom_tdscdma_band_type TDSCDMA band

  @return rfnv_tdscdma_supported_band_type
*/
rfnv_tdscdma_supported_band_type rfnv_tdscdma_convert_band_rfcom_to_rfnv (rfcom_tdscdma_band_type band)
{
  rfnv_tdscdma_supported_band_type tds_band = RFNV_TDSCDMA_SUPPORTED_BAND_NUM;
  
  switch (band)
  {
    case RFCOM_BAND_TDSCDMA_B34:
      tds_band = RFNV_TDSCDMA_B34;
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      tds_band = RFNV_TDSCDMA_B39;
      break;    
      
    case RFCOM_BAND_TDSCDMA_B40:
      tds_band = RFNV_TDSCDMA_B40;
      break;
	  
	case RFCOM_BAND_TDSCDMA_B40_B:
      tds_band = RFNV_TDSCDMA_B40_B;
      break;	  

    default:
      MSG_ERROR( "Invalid band in rfnv_tdscdma_convert_band_rfcom_to_rfnv, %d", band,0,0 );
      break;
  }

  return (tds_band);
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the NV valid flag to the correct value based on BC config

  @details
  This function is used to set the NV valid flag for each band which is enabled
  in NV_BC_Config to TRUE so that when NV is read part of Reload RF NV. This way
  we can avoid missing any NV for the active bands if this flag has been set to
  FALSE by someone else.

  @retval

*/

void rfnv_tdscdma_reset_valid_flags()
{
  //   
  uint16 idx, num_elements;
  num_elements = sizeof(rfnv_tdscdma_supported_band_info_table)/sizeof(rfnv_tdscdma_supported_band_info_table[0]);

  
  for (idx = 0; idx < num_elements; ++idx)
  {
    //hyang todo: check band mask
    rfnv_tdscdma_supported_band_info_table[idx].rfnv_data_valid_flag = TRUE;
  }  
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx characterization therm bins, for use with characterized
  Lin vs Temp vs Freq data

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
*/
void rfnv_tdscdma_load_equidistant_therm_bins_data
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr
)
{
  uint8 therm_bin;

  /* Initialize the therm bins to use equally spaced bins */
  for ( therm_bin = 0; therm_bin < NV_TEMP_TABLE_SIZ; therm_bin++ )
  {
    nv_tbl_ptr->lin_temp_comp_data.tx_therm_bins[therm_bin] = 
                                 therm_bin * RF_TDSCDMA_NV_SINGLE_TEMP_BIN_SIZE;
  }

  return;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load Lin vs Temp vs Freq Pin and Pout NV data offset data into common Tx
  Lin data structure type for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
  @param rfnv_item_ptr
  Pointer to the NV buffer where data loaded from NV currently is stored
 
  @param is_pin
  Flag to indicate if Pin data or not. If not, the data is copied to Pout
*/
void rfnv_tdscdma_load_lin_vs_temp_vs_freq
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr,
  rfnv_item_type           *rfnv_item_ptr,
  boolean                  is_pin
)
{
  uint8 pa_index_ctr = 0;
  rf_tdscdma_nv_tx_lin_vs_temp_vs_freq_type *lin_temp_freq_ptr =
    (rf_tdscdma_nv_tx_lin_vs_temp_vs_freq_type *)rfnv_item_ptr;

  /* Initialize the temp comp data structure */
  nv_tbl_ptr->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;
  nv_tbl_ptr->lin_temp_comp_data.is_legacy_temp_comp_nv = FALSE; 

  for ( pa_index_ctr = 0; 
        pa_index_ctr < RFCOM_TXLIN_NUM_LINEARIZERS; 
        pa_index_ctr++ )
  {
    if ( is_pin )
    {
      memscpy( 
          &nv_tbl_ptr->lin_temp_comp_data.lin_vs_temp_vs_freq.pin[pa_index_ctr], (sizeof(int16)*NV_TEMP_TABLE_SIZ*RF_TDSCDMA_MAX_CAL_CHAN),
          &lin_temp_freq_ptr->lin_vs_temp_vs_freq[pa_index_ctr], 
          sizeof(int16)*NV_TEMP_TABLE_SIZ*RF_TDSCDMA_MAX_CAL_CHAN );
    }
    else
    {
      memscpy( 
          &nv_tbl_ptr->lin_temp_comp_data.lin_vs_temp_vs_freq.pout[pa_index_ctr], (sizeof(int16)*NV_TEMP_TABLE_SIZ*RF_TDSCDMA_MAX_CAL_CHAN),
          &lin_temp_freq_ptr->lin_vs_temp_vs_freq[pa_index_ctr], 
          sizeof(int16)*NV_TEMP_TABLE_SIZ*RF_TDSCDMA_MAX_CAL_CHAN );
    }
  }

  /* Also initialize the equidistant therm bins data in the NV data structure */
  rfnv_tdscdma_load_equidistant_therm_bins_data( nv_tbl_ptr );

  return;
}/* rfnv_tdscdma_load_lin_vs_temp_vs_freq */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load the Tx characterization channel list, for use with characterized
  Lin vs Temp vs Freq data

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
 
*/
void rfnv_tdscdma_load_lin_vs_temp_tx_char_chan
(
  rfnv_tdscdma_tx_tbl_type *nv_tbl_ptr
)
{
  uint8 freq_index;

  if ( nv_tbl_ptr == NULL )
  {
    MSG( MSG_SSID_RF, MSG_LEGACY_ERROR,
         "rfnv_tdscdma_load_lin_vs_temp_tx_char_chan: "
         "Null NV table pointer");
    return;
  }

  for ( freq_index = 0; freq_index < RF_TDSCDMA_MAX_CAL_CHAN; freq_index++ )
  {
    /* Populate the Lin vs Temp static NV data structure for 
       use with characterization data */
    if ( nv_tbl_ptr->tx_cal_chan[freq_index] != 0 )
    {
      nv_tbl_ptr->lin_temp_comp_data.tx_char_chan[freq_index] = 
                      (uint16)nv_tbl_ptr->tx_cal_chan[freq_index];
      //in Khz
      nv_tbl_ptr->lin_temp_comp_data.tx_char_freq[freq_index] =
                      rf_tdscdma_core_util_get_freq_from_chan(nv_tbl_ptr->tx_cal_chan[freq_index]);
    }
    else
    {
      /* At the first channel entry with '0', assume that the
         channel list is terminated and break out of the loop */
      break;
    }
  }
  nv_tbl_ptr->lin_temp_comp_data.tx_char_chan_size = freq_index;

}/* rfnv_tdscdma_load_lin_vs_temp_tx_char_chan */


/*--------------------------------------------------------------------------*/
/*!
  @brief
  Load Lin vs Temp vs Freq Pin and Pout NV data offset data from Pa_static into
  common Tx Lin data structure type for all bands

  @details

  @param nv_tbl_ptr
  Pointer to the band-specific NV table where data must be stored
*/
void rfnv_tdscdma_load_lin_vs_temp_vs_freq_from_pa_static
(
  rfnv_tdscdma_tx_tbl_type* nv_tbl_ptr
)
{
  uint8 pa_index_ctr;
  uint8 temp_index;
  uint8 freq_index;
    
  rfcom_txlin_temp_comp_nv_data_type* rfnv_item_ptr = (rfcom_txlin_temp_comp_nv_data_type*)&nv_tbl_ptr->lin_temp_comp_data; 
  
  /* Initialize the temp comp data structure */
  nv_tbl_ptr->lin_temp_comp_data.has_pin_pout_temp_comp_nv = TRUE;
  nv_tbl_ptr->lin_temp_comp_data.is_legacy_temp_comp_nv = FALSE; 
      
  for (pa_index_ctr=0; pa_index_ctr<RFCOM_TXLIN_NUM_LINEARIZERS; pa_index_ctr++)
  {
    for (temp_index=0; temp_index<NV_TEMP_TABLE_SIZ; temp_index++ )
    {
      for (freq_index=0; freq_index<RF_TDSCDMA_MAX_CAL_CHAN; freq_index++ )
      {
        rfnv_item_ptr->lin_vs_temp_vs_freq.pin[pa_index_ctr][temp_index][freq_index] = 
          nv_tbl_ptr->pa_static.tx_lin_pin[temp_index][freq_index][pa_index_ctr];
        rfnv_item_ptr->lin_vs_temp_vs_freq.pout[pa_index_ctr][temp_index][freq_index] = 
          nv_tbl_ptr->pa_static.tx_lin_pout[temp_index][freq_index][pa_index_ctr];
      }
    }
  }

  rfnv_tdscdma_load_equidistant_therm_bins_data( nv_tbl_ptr );

}/* rfnv_tdscdma_load_lin_vs_temp_tx_char_chan */

boolean rfnv_tdscdma_get_new_nv_flag(rfcom_tdscdma_band_type band)
{
  boolean New_NV_flag = FALSE; 
  switch (band)
  {
    case RFCOM_BAND_TDSCDMA_B34:
      New_NV_flag = rfnv_tdscdma_b34_tx_get_new_nv_flag();
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      New_NV_flag = rfnv_tdscdma_b39_tx_get_new_nv_flag();
      break;

    case RFCOM_BAND_TDSCDMA_B40:
      //New_NV_flag = rfnv_tdscdma_b40_tx_get_new_nv_flag();
      break;

    default:
      MSG_ERROR( "Unsupported TDS band for NV identifier!", 0, 0, 0);
   }
   return New_NV_flag;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  Extract the PA switchpoints for a given band

  @details

  @param band
  Band

  @param rise_swpts
  Pointer to an array of rise switchpoints

  @param fall_swpts
  Pointer to an array of fall switchpoints
  
*/

boolean rfnv_tdscdma_get_pa_swpts(rfcom_tdscdma_band_type band, int16* rise_swpts, int16* fall_swpts, int16* active_pa_states)
{
  boolean status = FALSE; 
  switch (band)
  {
    case RFCOM_BAND_TDSCDMA_B34:
      status = rfnv_tdscdma_b34_tx_get_pa_swpt(rise_swpts, fall_swpts, active_pa_states);
      break;

    case RFCOM_BAND_TDSCDMA_B39:
      status = rfnv_tdscdma_b39_tx_get_pa_swpt(rise_swpts, fall_swpts, active_pa_states);
      break;

    //case RFCOM_BAND_TDSCDMA_B40:
      //New_NV_flag = rfnv_tdscdma_b40_tx_get_new_nv_flag();
    //  break;

    default:
      MSG_ERROR( "Unsupported TDS band for NV identifier!", 0, 0, 0);
   }
   return status;
}/* rfnv_tdscdma_get_pa_swpts */
