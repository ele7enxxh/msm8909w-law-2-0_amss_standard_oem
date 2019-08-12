/*!
  @file
  rf_tdscdma_core_txlin.c

  @brief
  Provides TDSCDMA TX Linearizer Functionality

  @details

*/

/*===========================================================================

Copyright (c) 2011 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_txlin.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/18/15   whz     Check if pa_index out of range for AutoPin
12/16/15   jr      Fix compiler warning messages
12/15/15   whz     Print out real Pin value for AutoPin
12/09/15   whz     Apply txlin API changes for skipping Pout
09/10/15   whz     Use room temperature Pin NV as initial value for AutoPin
06/04/15   whz     Added AutoPin support
02/25/15   jr      Fix compiler warning messages
02/18/15   whz     Avoid DwPTS cut on HQ device
02/13/15   cd      Tx Lin max power cap/boost updates
01/30/15   kb      Fix EPT failure due to OLPC workaround
01/16/15   jz      Add assert while txlin NV isn't populated correctly
01/02/15   whz     Keep RX_ON in TxAGC
12/30/14   jz      UpPCH issue fix
12/09/14   whz     Remove JO feature for the change of RX_ON
11/18/14   nsh     Get the TxAGC group timing shift from RFC
11/12/14   jyu     MDSP/SMEM cleanup
11/07/14   whz     Removed JO feature for ASM integration into common TxAGC
10/15/14   jyu     Fixed bug to support Tx on Device2
10/10/14   qzh     Add support for 3rd party GRFC PA and no PAPM configuration in TDS
10/07/14   jhe     Added ref chan xpt pin offset in txlin_update
09/17/14   kb      Clean up FEATURE_DIMEPM_MODEM from mainline
07/21/14   aro/kai Added expansion ratio param in DPD expansion function
06/04/14   jyu     Added support to separate non-trigger and trigger writes
05/29/14   ych     Updated for 8 PA gain states
05/23/14   whz     Featurize the ASM change for non-DPM
05/08/14   jyu     Include ASM control as part of TxAGC script
05/04/14   jhe     KW fix
04/15/14   jhe     Assert in tds code if no txlin NV present
04/02/14   jhe     KW fix
03/20/14   jhe     txlin temp comp update
03/11/14   jhe     DPD code update
01/07/14   cd      Fixed xPT feature compiler error
01/06/14   cd      Linearizer and Tx AGC updates for loading xPT information to 
                   RFLM
12/15/13   jps     Fixes for RFLM based TXAGC override
11/25/13   jps     Populate first sweep lin table
11/19/13   jps     Support for RFLM-based Tx AGC override (TD-SCDMA)
11/08/13   jps     Fix for common TXAGC lin update if FEATURE_RF_HAS_XPT_SUPPORT
                   is not defined
11/06/13   jps     PA state pointer interface change for TD-SCDMA
11/06/13   jps     Updates for common TXAGC hookup
10/31/13   jps     Update PA parameters to common TXAGC
10/30/13   jps     Updates and fixes for common TXAGC hookup
10/15/13   jyu     Integrated with common TxAGC for TxLIN population
09/18/13   ms      Support for B34_B,B39_B
08/21/13   jhe     Update to support DPD
06/07/13   jf      Fix LTE freq comp
05/20/13   al      Addied support for split band
03/20/13   Saul    MultiLin. Each RGI now has info for which freqcomp to use.  
02/22/13   jyu     Merged from NikeL
12/04/12   ms      Removed unwanted APT functions
11/12/12   Saul    MultiLin. Removed dpd_data from lin table type
08/21/12   Saul    MultiLin. Renamed API to reflect its freq-comp work
08/15/12   rsr     Adding Default smps value flag to apt_supported structure and 
                   renamic structures from apt to xPT for Dime  
06/13/12   jhe     Multi-lin NV update
05/27/11   jhe     Initial version

============================================================================*/

#include "rfa_variation.h"
#include "modem_mem.h"
#include "comdef.h"
#include "rfcom.h"
#include "rf_tdscdma.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_mdsp.h"
#include "rf_tdscdma_core_txlin.h"
#include "rf_tdscdma_mc.h"
#include "rf_tdscdma_core_util.h"
#include "rf_tdscdma_core_xpt.h"
#include "rflm_txagc_api.h"
#include "rflm_dm_api.h"
#include "rfm.h"
/* memscpy */
#include "stringl.h"
#include "assert.h"

#include "rfc_common_tdd.h"
#include "rfdevice_tdscdma_asm_intf.h" 
#include "rfdevice_tdscdma_pa_intf.h"
#include "rfc_card.h"
#include "rfcommon_autopin_api.h"
#include "rflm_api_autopin.h"



rf_tdscdma_core_txlin_data_type rf_tdscdma_core_txlin_data;


/*!
  @brief   
   This structure holds the linearizer data copied from NVs for all supported TDSCDMA bands */
/* support multi-lin Rev1 and Rev3 */
//TODO - bolt only supports Rev3 multi-lin NV, to consider get rid of Rev1 related code
rf_tdscdma_core_tx_multi_lin_data_type rf_tdscdma_core_tx_multi_lin_data[2][RFCOM_NUM_TDSCDMA_BANDS] =
{
  {
    {NULL,RFNV_TDSCDMA_B34_TX_MULTI_LIN_DATA_I},  /* B34 */
    {NULL,RFNV_TDSCDMA_B39_TX_MULTI_LIN_DATA_I},  /* B39 */
    {NULL,RFNV_TDSCDMA_B40_TX_MULTI_LIN_DATA_I},  /* B40 */  
    {NULL,RFNV_TDSCDMA_B40_B_TX_MULTI_LIN_DATA_I}, /* B40_B */
    {NULL,RFNV_TDSCDMA_B34_TX_MULTI_LIN_DATA_I}, /* B34_B */
    {NULL,RFNV_TDSCDMA_B39_TX_MULTI_LIN_DATA_I}  /* B39_B */
  },
  {
    {NULL,RFNV_TDSCDMA_B34_TX_MULTI_LIN_V3_DATA_I},  /* B34 */
    {NULL,RFNV_TDSCDMA_B39_TX_MULTI_LIN_V3_DATA_I},  /* B39 */
    {NULL,RFNV_TDSCDMA_B40_TX_MULTI_LIN_V3_DATA_I},  /* B40 */  
    {NULL,RFNV_TDSCDMA_B40_B_TX_MULTI_LIN_V3_DATA_I}, /* B40_B */
    {NULL,RFNV_TDSCDMA_B34_TX_MULTI_LIN_V3_DATA_I},  /* B34_B */
    {NULL,RFNV_TDSCDMA_B39_TX_MULTI_LIN_V3_DATA_I}  /* B39_B */
  }
};

rf_tdscdma_core_txlin_record_type rf_tdscdma_core_current_txlin_data;

int16 rf_tdscdma_core_autopin_pin_comp_init[RFCOM_TXLIN_NUM_LINEARIZERS] = { 0 };


/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details

  @param

*/
void rf_tdscdma_core_txlin_calc_total_txlin_comp(void)
{
  uint8 i;
  int16 offset_val;

  for (i=0; i<RFCOM_TXLIN_NUM_LINEARIZERS; i++)
  {
    /* add the temperature compensation value */
    /* temp comp factor needs a sign change before passing to MDSP */
    offset_val = (-1)*rf_tdscdma_core_txlin_data.tx_lin_vs_temp_val[i];

    // No longer needed as freq comp is now processed in multi-lin
    /* add the frequency compensation value */
    /* freq comp factor needs a sign change before passing to MDSP */
    //offset_val -= (int16) rf_tdscdma_core_txlin_data.tx_comp_vs_freq_val[i];

    /* Limit the offset value to within the AGC range */
    /* Saturate offset -/+10dB */
    offset_val = RF_SATURATE( offset_val, -10*RF_TDSCDMA_TXAGC_RESOLUTION, 10*RF_TDSCDMA_TXAGC_RESOLUTION );

    /* store the total linearizer compensation value for all 4 gain ranges */
    rf_tdscdma_core_txlin_data.tx_lin_comp[i] = offset_val;
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This API updates tx_lin_comp[ept_pa_index] to have value from Pout_vs_temp_vs_freq

  @details

  @param

*/
void rf_tdscdma_core_txlin_update_xpt_txlin_comp(void)
{
  uint8 ept_pa_index=0;
  
  ept_pa_index = rf_tdscdma_core_current_txlin_data.ept_pa_state;

  /* Note:
     Here it is assumed APT temp comp will use Lin_vs_temp NV, while the Pout for ept is using 
     Pout_vs_temp_vs_freq NV. If it is going to be Pout_vs_temp_vs_freq for APT too, need update here.
     Also for Lin_vs_temp NV, the sign is flipped before passing to TFW
  */

  /* implies xpt is enabled */
  if (ept_pa_index>0)
  {
    /* similar to TX_LIN_VS_TEMP NV,   
       temp comp factor needs a sign change before passing to MDSP */
    rf_tdscdma_core_txlin_data.tx_lin_comp[ept_pa_index] = (-1)*rf_tdscdma_core_txlin_data.pout_vs_temp_vs_freq[ept_pa_index];
  }

}


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function will return the max valid PA state based on NV setting of PA 
  switchpoints. 
   
*/
uint8 rf_tdscdma_core_txlin_get_num_valid_pa_state(rfcom_tdscdma_band_type band)
{
  rfnv_tdscdma_tx_tbl_type* tx_nv_tbl = rf_tdscdma_core_util_init_tx_nv_table(band);

  /* Null pointer check */
  if (tx_nv_tbl == NULL)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_core_txlin_get_max_valid_pa_stateL:Null pointer access");
    return 0;
  }

  if(rfnv_tdscdma_get_new_nv_flag(band))  //New NV
  {
    return tx_nv_tbl->pa_static.header[PA_STATIC_NUM_ACTIVE_PA];
  }
  else   //Old NV
  {
    /* Check if switchpoints are railed */
    if (tx_nv_tbl->pa_static.pa_rise_sw[3] < 1023)
    {
      return 4;
    }
    else if (tx_nv_tbl->pa_static.pa_rise_sw[2] < 1023)
    {
      return 3;
    }
    else if (tx_nv_tbl->pa_static.pa_rise_sw[1] < 1023)
    {
      return 2;
    }
    else
    {
      return 1;   
    }
  }
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function copies the linearizer data from NVs for all supported bands 


  @details

*/
void rf_tdscdma_core_txlin_init(void)
{
  rfcom_tdscdma_band_type band;
  //tx_multi_lin_rev_type lin_type;
  int16 lin_type; 
  boolean success;
  uint8 lin_index;

  for (band=RFCOM_BAND_TDSCDMA_B34; band<RFCOM_NUM_TDSCDMA_BANDS; band++)
  {
    /* Start from the newest NV revision */
    #ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC
    for(lin_type=TX_MULTI_LIN_REV_3; lin_type>0; lin_type-=2)
    #else
    for(lin_type=TX_MULTI_LIN_REV_3; lin_type>=0; lin_type-=2)
    #endif
    {
      if (lin_type==TX_MULTI_LIN_REV_3)
      {
        lin_index = 1;
      }
      else if (lin_type==TX_MULTI_LIN_REV_1)
      {
        lin_index = 0;
      }
      else
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_FATAL,"wrong mutilin rev used %d", lin_type);
        return;
      }

      /* Allocate memory to load linearizer */
      if (rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data == NULL)
      {
        rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data = 
        modem_mem_calloc(1,sizeof(tx_band_cal_type),MODEM_MEM_CLIENT_RFA);
      }

      if (rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data != NULL)
      {
        success = rfcommon_core_load_linearizer_from_nv( lin_type,
                                                         rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_nv,
                                                         rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data,
                                                         rf_tdscdma_core_util_get_freq_from_chan_common_api );

        if(success)
        {
          /* Do not continue if NV load was successful for this band. Only newest revision should be loaded. 
             Go to the next band */
          break;
        }

        /* If fails even on multi-lin rev 1 */
        else if( (!success) && (lin_type==TX_MULTI_LIN_REV_1) )
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_core_txlin_init: "
                                               "Failed to load multilin nv for band %d. ", band);
          /* free up the memeory*/
          modem_mem_free((void*)rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data, MODEM_MEM_CLIENT_RFA);
          rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data = NULL;
        }

        else
        {
          modem_mem_free((void*)rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data, MODEM_MEM_CLIENT_RFA);
          rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data = NULL;
        }
      }
      else
      {
        /* malloc failed */
        MSG_2(MSG_SSID_RF, MSG_LEGACY_FATAL,"memory allocation failed for Txlin V%d data on band:%d", lin_type, band);
      }
    } /* for NUM LINEARIZERS */
  } /* for NUM BANDS */
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the multi linearizer data (copied from NVs) for
  the band requested

  @details

  @param band TDSCDMA band

  @note: before this function is called, multi_lin_rev for the current_txlin_data
  must have been set correctly
  
*/
tx_band_cal_type *rf_tdscdma_core_txlin_get_band_data( rfcom_tdscdma_band_type band )
{
  tx_band_cal_type * multi_lin_ptr = NULL;
  uint8 lin_index=0;

  if (band < RFCOM_NUM_TDSCDMA_BANDS)
  {
    if ( rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_3 )
    {
      lin_index = 1;
    }
    else if (rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_1)
    {
      lin_index = 0;
    }
    else
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rf_tdscdma_core_txlin_get_band_data, invalid multi lin rev %d", 
            rf_tdscdma_core_current_txlin_data.multi_lin_rev);
      return NULL;
    }

    if(rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data != NULL)
    {
      multi_lin_ptr = rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data;
    }
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid TDSCDMA band passed", 0);
    multi_lin_ptr = NULL;
  }

  return multi_lin_ptr;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This fucntion initializes the XPT parameters for the given band 
    
  @details 
  Checks the multi-lin NV to determine the multi-lin version for the given band 
  
*/
void rf_tdscdma_core_txlin_xpt_band_init(rfcom_tdscdma_band_type band)
{
  int8 lin_index;

  if (band < RFCOM_NUM_TDSCDMA_BANDS)
  {
    /* Set the correct Multi-lin revision */
    for (lin_index = 1; lin_index >= 0; lin_index--)
    {
      if (rf_tdscdma_core_tx_multi_lin_data[lin_index][band].txlin_data != NULL)
      {
        if (lin_index == 1)   /* index 1 stores multi-lin rev 3 */
        {
          rf_tdscdma_core_current_txlin_data.multi_lin_rev = TX_MULTI_LIN_REV_3;
        }
        else if (lin_index == 0)  /* index 0 stores multi-lin rev 1 */
        {
          rf_tdscdma_core_current_txlin_data.multi_lin_rev = TX_MULTI_LIN_REV_1;
        }
        break;
      }
    }

    rf_tdscdma_core_current_txlin_data.num_valid_pa_state = rf_tdscdma_core_txlin_get_num_valid_pa_state(band);

  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid TD-SCDMA band passed", 0);
    rf_tdscdma_core_current_txlin_data.multi_lin_rev = TX_MULTI_LIN_NUM_REVS;
    return;
  }

}/* rf_tdscdma_core_txlin_xpt_band_init */


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function will search the Tx band Cal data NV to get the Linearizer data   
  for the given frequency, pa state, and data type. it will apply freq compensation 
  to the linearizer data lut and update the linearizer table to be populated to FW 

  @details

*/
static boolean rf_tdscdma_core_txlin_get_final_lut(tx_linearizer_search_param_type *search_param,
                                                   tx_band_cal_type *tx_band_cal_data_ptr,
                                                   tx_linearizer_table_data_type *current_tx_lin_data,
                                                   rfcommon_core_txlin_cap_boost_type *cap_boost_data
                                                  )
{
  boolean status_flag = FALSE;
  uint8 pa_index = search_param->pa_range;

  /*************************************************************************************************
      Populate Tx Linearizer LUT STEP 1: use common function to search for the linearizer table from
      Tx band Cal data, and apply frequency compensation for the given channel and PA range
   ***************************************************************************************/

  status_flag = rfcommon_core_freq_comp_linearizer( search_param,
                                                    tx_band_cal_data_ptr,
                                                    current_tx_lin_data,
                                                    (void*)cap_boost_data,
                                                    TRUE );
  if (!status_flag)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," TDSCDMA TxLin update: No result after populating linearizer from NV for PA index:%d ", pa_index);
    /* If fail here, stop the procedure and go to the next PA index */
    return status_flag;
  }

  /* Sort TX Table in increasing order
   *
   * QSORT uses the function given (rfcommon_core_qsort_compare_increasing_pwr)
   * to compare elements, and sorts the whole linearizer row based on comparing
   * tx power level.
   */
  if(rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_3)
  {
    qsort( &(current_tx_lin_data->lin_data->table),  
           RFCOM_TXLIN_NUM_SEGMENTS,
           sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
  }         

  return TRUE;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function populate the Tx linearizer for the given frequency

  @details
   
  @return
  TRUE if all RFcal data is present
  FALSE if missing RFcal data   
*/
boolean rf_tdscdma_core_txlin_update( rfcom_tdscdma_band_type band,
                                      uint16 rf_chan_tx,
                                      uint8 buf_idx, 
                                      lm_handle_type txlm_buffer_idx )
{
  rfcommon_core_xpt_tx_agc_info_type xpt_info;
  uint16 num_rgi_with_dpd_info;
  xpt_dpd_scaling_value_type dpd_scale_factor;

  boolean status_flag = TRUE;
  uint8 pa_index;  
  uint32 chan_freq = 0;
  tx_band_cal_type *tx_band_cal_data_ptr = NULL;
  tx_linearizer_search_param_type search_param;
  tx_linearizer_table_data_type txlin_final_table;
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_curr_table;
  int16 pin_comp_val[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 pout_comp_val[RFCOM_TXLIN_NUM_LINEARIZERS];
  int16 txlin_vs_temp[RF_TDSCDMA_NUM_TX_LINEARIZERS];
  int32 ref_xpt_pin_offset = 0;
  int16 i;

  #ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC
  rfcommon_core_txlin_agc_device_info device_info;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcom_band_type_u rfm_band;

  rflm_txagc_dynamic_t *txagc_dynamic_data;
  uint8 pa_active[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8 pa_state = 0;
  uint8 num_valid_pa_states = 0;
  /* Power cap/boost algorithm */
  rfcommon_core_txlin_cap_boost_type cap_boost_data;
  #endif

  

  /********************************************************************* 
    Initialization: get Tx band Cal data handler,etc
   *********************************************************************/

  /* Clear the Txlin shadow first */
  memset(&rf_tdscdma_core_current_txlin_data,0,sizeof(rf_tdscdma_core_current_txlin_data));

  /* Init variables for XPT, eg multi-lin rev */
  rf_tdscdma_core_txlin_xpt_band_init(band);

  /* Get current temp comp algorithm */
  rf_tdscdma_core_current_txlin_data.temp_comp_algo = RFCOM_TXLIN_TEMP_COMP_ALGO_MULTILIN_V3;

  /*Convert current tx_chan to equivalent frequency */
  chan_freq = rf_tdscdma_core_util_get_freq_from_chan(rf_chan_tx);

  /*For the current band, get the pointer to complete linearizer band cal data  */
  tx_band_cal_data_ptr = rf_tdscdma_core_txlin_get_band_data(band);

  /*Check for NULL pointers*/
  if ( tx_band_cal_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"TDSCDMA TxLin update: Cal data Null pointer, no txlin NV present");
    ASSERT(tx_band_cal_data_ptr!=NULL);
    return FALSE;
  }

  if(tx_band_cal_data_ptr->tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid TDSCDMA tx cal chan size: %d",tx_band_cal_data_ptr->tx_cal_chan_size); 
    return FALSE;
  }

  /* memory allocations */
  txlin_first_sweep_tbl.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  txlin_final_table.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if((txlin_first_sweep_tbl.lin_data == NULL) ||
     (txlin_final_table.lin_data == NULL))
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Fail to allocate memory for processing linearizer");
    return FALSE;
  }

  /* get the pin/pout temp comp data */
  rf_tdscdma_core_temp_comp_get_pin_pout(rf_chan_tx, pin_comp_val, pout_comp_val);

  /* calc txlin vs temp from NV */
  rf_tdscdma_core_temp_comp_calc_txlin_vs_temp(txlin_vs_temp);

  RF_MSG_4( RF_MED, "txlin_vs_temp values:%d,%d,%d,%d", 
            txlin_vs_temp[0],txlin_vs_temp[1],txlin_vs_temp[2],txlin_vs_temp[3]);

  /* add txlin_vs_temp to pout, to provide another knob for adjusting pout 
     note: txlin_vs_temp NV only has 4 pa states */
  for (i=0; i<RF_TDSCDMA_NUM_TX_LINEARIZERS; i++)
  {
    pout_comp_val[i] += txlin_vs_temp[i];
  }

  if( rf_tdscdma_core_xpt_is_enabled(band) && 
      (rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1) )
  {
    rf_common_xpt_init_dpd(rf_tdscdma_mc_state.curr_pri_device);
  }

  #ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC

  rfm_band.tdscdma_band = band;

  /* Retrieve device info */
  rf_tdscdma_core_txlin_get_tx_device_info(rf_tdscdma_mc_state.curr_pri_device, band, &device_info);

  /* Allocate Memory for Dynamic data (this will be used to populate and publish PA params to common TXAGC) */
  txagc_dynamic_data = modem_mem_alloc( sizeof(rflm_txagc_dynamic_t),
                                        MODEM_MEM_CLIENT_RFA );

  if ( txagc_dynamic_data == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Fail to allocate memory for txagc_dynamic_data");
    return FALSE;
  }

  cap_boost_data.max_pwr_ovr_algo = 
     rf_tdscdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val =
     rf_tdscdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_MAX_PWR_OVR_VAL );

  #endif /* ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC*/


  /*********************************************************************
     Start populating linearizer data for all PA ranges for voice/R4 waveforem
  **********************************************************************/

  /* Populate search parameter: frequency and waverform type */
  search_param.rfm_device = rf_tdscdma_mc_state.curr_pri_device;
  search_param.tx_freq = chan_freq;

  for (pa_index = 0; pa_index < rf_tdscdma_core_current_txlin_data.num_valid_pa_state; pa_index++)
  {
    /* this should never happen but so that KW won't complain... */
    if (pa_index >= RFCOM_TXLIN_NUM_LINEARIZERS)
    {
      continue;
    }

    /**************************************                      
       get the linearizer
     **************************************/

    /* Populate Voice linearizer LUT for the PA state  */
    search_param.tx_waveform = TX_LINEARIZER_VOICE; 
    /* Populate search parameter*/
    search_param.pa_range = pa_index;

    /* Clear final table for population */
    memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
    txlin_final_table.xpt_data = NULL;  

    MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH,"TDSCDMA TxLin update: Populate TxLin from Tx Band Cal data for PA index:%d ", pa_index);

    /* Utility Function to get lin data from NV and process the data */
    status_flag = rf_tdscdma_core_txlin_get_final_lut(&search_param,
                                                      tx_band_cal_data_ptr,
                                                      &txlin_final_table,
                                                      &cap_boost_data );

    if (!status_flag)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," TDSCDMA Tx Lin update: Populate final lin table failed with pa_index =%d something wrong with txlin NV", pa_index);
      /* Free the temporary table */
      modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
      txlin_final_table.lin_data = NULL;
	  ASSERT(0);
      return FALSE;
    }

    /* save the non-temp-comped lin data */
    memscpy(&rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index], sizeof(tx_linearizer_table_type), 
            txlin_final_table.lin_data, sizeof(tx_linearizer_table_type));
    rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index] = txlin_final_table.aux_lin_data;
    rf_tdscdma_core_current_txlin_data.txlin_valid_entries[pa_index] = RFCOM_TXLIN_NUM_SEGMENTS;
    
    MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH,"txlin_update: pa_index =%d aux_data=%d", pa_index, rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index]);

    /********************************                      
       get the first sweep linearizer 
     ********************************/

    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH,"TDSCDMA TxLin update: updating first sweep table ");

    if(rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1)
    {
      search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP; 
      search_param.pa_range = pa_index;

      /* Clear first sweep table for population */
      memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
      txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
      txlin_first_sweep_tbl.xpt_data = NULL;

      /*Obtain the first sweep data*/
      status_flag = rfcommon_core_freq_comp_linearizer( &search_param,
                                                        tx_band_cal_data_ptr,
                                                        &txlin_first_sweep_tbl,
                                                        (void*)&cap_boost_data,
                                                        TRUE );

      if (!status_flag)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"TDSCDMA TxLin update: No result populating first sweep linearizer from NV for PA index:%d something wrong with txlin NV", 
			  search_param.pa_range);
        /* Free the temporary table */
        modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
        txlin_first_sweep_tbl.lin_data = NULL;
		ASSERT(0);
        return status_flag;
      }

      /* Need to sort TX Table in increasing order
       *
       * QSORT uses the function given (rfcommon_core_qsort_compare_increasing_pwr)
       * to compare elements, and sorts the whole linearizer row based on comparing
       * tx power level.
       */
      if(rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_3)
      {
        qsort( &(txlin_first_sweep_tbl.lin_data->table),  
                RFCOM_TXLIN_NUM_SEGMENTS,
                sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
      }  
    }


    /************************************************
      Apply temp comp to the current linearizer table
     ************************************************/

    txlin_curr_table.lin_data = &rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index];
    txlin_curr_table.aux_lin_data = rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index];
    txlin_curr_table.xpt_data = txlin_final_table.xpt_data;

    /* ref_xpt_pin_offset is usually calculated in rf_tdscdma_core_temp_comp_vs_freq_get_comp(),  however at channel setup
       it may not have all the info (aux_lin_data) to get it correct at the time it is called */
    ref_xpt_pin_offset = 0;
    if(txlin_curr_table.aux_lin_data == TX_LINEARIZER_EPT_DPD_V2_DATA)
    {
      ref_xpt_pin_offset = (int32)rf_tdscdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_REF_EPT_PIN_OFFSET );
    }
    else if(txlin_curr_table.aux_lin_data == TX_LINEARIZER_ET_DPD_DATA)
    {
      ref_xpt_pin_offset = (int32)rf_tdscdma_core_xpt_get_cfg_param( band, XPT_CFG_CMN_REF_ET_PIN_OFFSET );
    }
    pin_comp_val[pa_index] += ref_xpt_pin_offset;
    MSG_3(MSG_SSID_RF, MSG_LEGACY_MED,"txlin_update: pa_index=%d aux_data=%d ref_xpt_pin_offset=%d", 
          pa_index, txlin_curr_table.aux_lin_data, ref_xpt_pin_offset);

    status_flag = rfcommon_core_txlin_temp_vs_freq_comp_update_v2(
                                                rf_tdscdma_core_current_txlin_data.temp_comp_algo,
                                                &txlin_curr_table,
                                                &txlin_final_table,
                                                &txlin_first_sweep_tbl,
                                                pin_comp_val[pa_index],
                                                pout_comp_val[pa_index],
                                                rf_tdscdma_core_current_txlin_data.txlin_valid_entries[pa_index],
                                                FALSE);

    if (!status_flag)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"temp comp populate failure for pa_index =%d ", pa_index);
    }

    #if 1
    /* Copy the temp-comped linearizer to shadow for logging */
    memscpy(&rf_tdscdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
            txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
    #endif
    rf_tdscdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val[pa_index];
    rf_tdscdma_core_current_txlin_data.txlin_pout_comp[pa_index] = pout_comp_val[pa_index];


    /********************************                      
      load DPD for EPT state(s) 
     ********************************/

    if( rf_tdscdma_core_xpt_is_enabled(band) && 
            (rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1) )
    {
      /* Initialize and load DPD variable for EPT/ET PA ranges */
      if( (rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index] == TX_LINEARIZER_EPT_DPD_V2_DATA) || 
          (rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index] == TX_LINEARIZER_ET_DPD_DATA) )
      {
        /* Note: TDS currently only supports EPT */
        /* save the ept pa state */
        rf_tdscdma_core_current_txlin_data.ept_pa_state = pa_index;

        /* Init DPD to invalid vals */
        memset( xpt_info.amam_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.ampm_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );
        memset( xpt_info.epdt_bank_info, 0xFF, 64*sizeof(rfcommon_xpt_dpd_bank_info_type) );

        /* interpolate the scale value */
        rf_common_xpt_dpd_scale_computation ( &rfnv_tdscdma_tx_tbl_ptr->dpd_scaling_data,
                                              &rfnv_tdscdma_tx_tbl_ptr->lin_temp_comp_data.tx_char_freq[0],
                                              rfnv_tdscdma_tx_tbl_ptr->lin_temp_comp_data.tx_char_chan_size,
                                              chan_freq,
                                              &dpd_scale_factor);

        // make sure dpd_scale is disabled
        dpd_scale_factor.valid = FALSE;

        /* load dpd */
        status_flag = rfcommon_core_populate_tx_dpd(rf_tdscdma_mc_state.curr_pri_device,
                                                    &txlin_final_table,
                                                    &(xpt_info.amam_bank_info[0]),
                                                    &(xpt_info.ampm_bank_info[0]),
                                                    &(xpt_info.epdt_bank_info[0]),
                                                    &num_rgi_with_dpd_info,
                                                    &dpd_scale_factor,
                                                    NULL);

        if(!status_flag)
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"DPD populate failure for pa_index =%d ", pa_index);
          /* Free the temporary table */
          modem_mem_free((void*)txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
          txlin_final_table.lin_data = NULL;
          modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
          txlin_first_sweep_tbl.lin_data = NULL;
          return FALSE;
        }

      }
    }                         


    /* Initialize PA STM info */
    pa_stm_info.pa_state = pa_state;  //pa_index, same thing
    pa_stm_info.pa_range = rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_range_map[pa_index]; // pa_range

    /* Configure the linearizer to RFLM */ 
    status_flag = rfcommon_core_txlin_config_init_linearizer(RFM_TDSCDMA_MODE, 
                                                             rf_tdscdma_mc_state.curr_pri_device,
                                                             rfm_band,
                                                             (int32)txlm_buffer_idx, // TxLM handle
                                                             &txlin_final_table, // *final_tx_lut
                                                             &txlin_first_sweep_tbl, 
                                                             &device_info,
                                                             &pa_stm_info,
                                                             (void*)&xpt_info); 
    if (!status_flag)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"txlin_config_init_linearizer failed for pa_index =%d!", pa_index);
    }

    pa_state++;

  } /* end for(pa_index = 0; pa_index < RFCOM_TXLIN_NUM_LINEARIZERS; pa_index++) */


  /* config_txagc must be called after config_init_linearizer */
  status_flag = rf_tdscdma_core_txlin_config_txagc(rf_tdscdma_mc_state.curr_pri_device,
                                                   band,
                                                   txlm_buffer_idx);
  if (!status_flag)
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"txlin_config_txagc failed !");
  }


  #ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC
  /* First get TxAGC data from RFLM DM buffer using RFLM/TxLM handle */
  status_flag = rflm_dm_get_txagc_data( txlm_buffer_idx, 
                                        (void *)txagc_dynamic_data, 
                                        TXAGC_GET_DYNAMIC_DATA );

  if (!status_flag)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," TDS Tx Lin update: Error when querying common TXAGC dynamic data! ", 0);
    /* Free the allocated memory */
    modem_mem_free(txagc_dynamic_data, MODEM_MEM_CLIENT_RFA);
    return FALSE;
  }
  else
  {
    // just fill it - tds doesn't use timer hysteresis
    txagc_dynamic_data->pa_sp.hysteresis_fall = rfnv_tdscdma_tx_tbl_ptr->timer_hysterisis;

    num_valid_pa_states = rf_tdscdma_core_txlin_get_num_valid_pa_state(band);
    for (pa_state = 0; pa_state < num_valid_pa_states; pa_state++)
    {
      if (pa_state == 0)
      {
        /* For lowest PA state, rise and fall are min power -70 dBm */
        txagc_dynamic_data->pa_sp.sp[pa_state].pa_fall_sp = -700;
        txagc_dynamic_data->pa_sp.sp[pa_state].pa_rise_sp = -700;
      }
      else
      {
        if(rfnv_tdscdma_get_new_nv_flag(band))  //New NV
        {
          /* Update PA switchpoint */
          txagc_dynamic_data->pa_sp.sp[pa_state].pa_fall_sp = 
                                               (rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_fall_sw[pa_state]);
          txagc_dynamic_data->pa_sp.sp[pa_state].pa_rise_sp = 
                                               (rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_rise_sw[pa_state]);
        }
        else
        {
          /* Update PA switchpoint */
          txagc_dynamic_data->pa_sp.sp[pa_state].pa_fall_sp = 
                                               (rfnv_tdscdma_tx_tbl_ptr->pa_rise_fall[pa_state-1].fall_threshold - 700);
          txagc_dynamic_data->pa_sp.sp[pa_state].pa_rise_sp = 
                                               (rfnv_tdscdma_tx_tbl_ptr->pa_rise_fall[pa_state-1].rise_threshold - 700);
        }
      }

      pa_active[pa_state] = 1;

      /* Update PA state number */
      txagc_dynamic_data->pa_sp.sp[pa_state].pa_state = (rflm_txagc_pa_state_data_t)pa_state;

      if(rf_tdscdma_core_xpt_is_enabled(band))
      {
        switch (rf_tdscdma_core_current_txlin_data.current_aux_data[pa_state])
        {
        case TX_LINEARIZER_NO_AUX_DATA:
          /* APT mode */
          txagc_dynamic_data->pa_sp.sp[pa_state].xpt_mode = RFLM_TXAGC_APT;
          break;
        case TX_LINEARIZER_EPT_DPD_V2_DATA:
          /* EPT mode */
          txagc_dynamic_data->pa_sp.sp[pa_state].xpt_mode = RFLM_TXAGC_EPT;
          break;
        case TX_LINEARIZER_ET_DPD_DATA:
          /* ET mode */
          txagc_dynamic_data->pa_sp.sp[pa_state].xpt_mode = RFLM_TXAGC_ET;
          break;
        default:
          RF_MSG( RF_ERROR, "rf_tdscdma_core_txlin_update: Invalid dpd_type" );
          status_flag = FALSE;
          /* APT mode */
          txagc_dynamic_data->pa_sp.sp[pa_state].xpt_mode = RFLM_TXAGC_APT;
          break;
        }
      }
      else
      {
        RF_MSG( RF_HIGH, "rf_tdscdma_core_txlin_update: APT only mode!" );
        /* APT only mode */
        txagc_dynamic_data->pa_sp.sp[pa_state].xpt_mode = RFLM_TXAGC_APT;
      }
    } /* End for loop */

    /* Update the active PA state (4 states for now) */
    rflm_txagc_set_pa_state_ptr(txagc_dynamic_data, pa_active,(uint32)txlm_buffer_idx);

    /* Push back the TxAGC data to FED DM */
    status_flag = rflm_dm_set_txagc_data( txlm_buffer_idx, 
                                      TXAGC_SET_DYNAMIC_DATA, 
                                      (void *)txagc_dynamic_data );

    if ( txagc_dynamic_data != NULL )
    {
      modem_mem_free( txagc_dynamic_data, MODEM_MEM_CLIENT_RFA );
    }
  }
  #endif /* #ifdef RF_TDSCDMA_INTEGRATE_COMMON_TXAGC */
  

  if(rf_tdscdma_core_xpt_is_enabled(band) && 
          (rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1))
  {
    // on Bolt this api has no effect ...
    rf_common_xpt_commit_dpd_data(rf_tdscdma_mc_state.curr_pri_device);
  }

  /* Deallocate memory */
  if (txlin_first_sweep_tbl.lin_data != NULL)
  {
    modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
  }
  if (txlin_final_table.lin_data != NULL)
  {
    modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_final_table.lin_data = NULL;
  }

  return TRUE;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This func is called to combine freq comp and temp comp to update the total 
  compensation value to mdsp shared memory

  @details

*/
void rf_tdscdma_core_txlin_comp_update(uint16 tx_chan,
                                       boolean init_flag,
                                       uint8 buf_idx)
{
  //#ifdef FEATURE_RF_HAS_XPT_SUPPORT
  uint8 ept_pa_index=0;
  int16 pin_delta=0;
  //#endif

  /* calculate TX linearizer compensation value (freq + temp)*/
  rf_tdscdma_core_txlin_calc_total_txlin_comp();

  /* update the pout to EPT pa range */
  //#ifdef FEATURE_RF_HAS_XPT_SUPPORT
  if( rf_tdscdma_core_xpt_is_enabled(rf_tdscdma_core_util_get_band_from_uarfcn(tx_chan)) )
  {
    rf_tdscdma_core_txlin_update_xpt_txlin_comp();
  }
  //#endif


  /* Load Pin comp value to MDSP shared memory  */
  //#ifdef FEATURE_RF_HAS_XPT_SUPPORT
  if( rf_tdscdma_core_xpt_is_enabled(rf_tdscdma_core_util_get_band_from_uarfcn(tx_chan)) )
  {
    ept_pa_index = rf_tdscdma_core_current_txlin_data.ept_pa_state;
    /* If Pin is -x db, means online should have x db more Pin. According to FW implementation,
       flip the sign before passing to FW */
    pin_delta = (-1)*rf_tdscdma_core_txlin_data.pin_vs_temp_vs_freq[ept_pa_index];
  }
  //#endif
}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  After the lin temp comp value is interpolated for the given temperature, this 
  func is called to update the value (to be used to calc total comp temp+freq)

  @details

*/
void rf_tdscdma_core_txlin_temp_val_update
(
  int16 tx_lin_vs_temp_val[],
  int16 pin_offset[],
  int16 pout_offset[]
)
{
  uint8 i;
  for (i=0; i < RFCOM_TXLIN_NUM_LINEARIZERS; i++)
  {
    rf_tdscdma_core_txlin_data.tx_lin_vs_temp_val[i] = tx_lin_vs_temp_val[i];
    rf_tdscdma_core_txlin_data.pin_vs_temp_vs_freq[i] = pin_offset[i];
    rf_tdscdma_core_txlin_data.pout_vs_temp_vs_freq[i] = pout_offset[i];
  }

}


/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the pointer to the current Tx linearizer data

  @details

  @return pointer to the current Tx linearizer data

*/
rf_tdscdma_core_txlin_record_type* rf_tdscdma_core_get_current_txlin_data(void)
{
  return (&rf_tdscdma_core_current_txlin_data);
}

/*--------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the tx device info

*/
boolean rf_tdscdma_core_txlin_get_tx_device_info
(
  rfm_device_enum_type device,                                            
  rfcom_tdscdma_band_type band,
  rfcommon_core_txlin_agc_device_info *device_info)
{
  boolean api_status = TRUE;
  rfdevice_type_enum_type device_type[3] = {RFDEVICE_TRANSMITTER, RFDEVICE_PA, RFDEVICE_PAPM};
  uint8 device_loop = 0;
  uint8 device_idx = 0;
  void** tds_device = NULL;
  void* device_ptr[3] = {NULL, NULL, NULL};

  if ( device_info == NULL )
  {
    RF_MSG( RF_ERROR, "rf_tdscdma_core_txlin_get_tx_device: Null device info pointer!");
    api_status &= FALSE;
    return api_status;
  }

  if (band >= RFCOM_NUM_TDSCDMA_BANDS)
  {
    RF_MSG_2( RF_ERROR, "rf_tdscdma_core_txlin_get_tx_device: Bad input arguments! device: %d, band: %d", device, band);
    api_status &= FALSE;
    return api_status;
  }

  device_info->wtr_device = NULL;
  device_info->pa_device = NULL;
  device_info->wtr_device = NULL;

  for (device_loop = 0; device_loop < 3; device_loop++)
  {
    tds_device = rfc_tdscdma_get_rf_device_obj(device, 
                                               RFC_CONFIG_TX, 
                                               band, 
                                               device_type[device_loop]);

    if((tds_device == NULL) || (tds_device[device_idx] == NULL))
    {
      MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "RFDEVICE-TDS: NULL device_ptr for band %d, device %d and device type %d", 
            band, 
            device, 
            device_type[device_loop]); 
      device_ptr[device_loop] = NULL;
      //api_status &= FALSE;
      //return api_status;
    }
    else
    {
      device_ptr[device_loop] = tds_device[device_idx];
    }
  } /* for (device_loop = 0; device_loop < 3; device_loop++) */

  device_info->wtr_device = device_ptr[0];
  device_info->pa_device = device_ptr[1];
  device_info->papm_device = device_ptr[2];

  return api_status;

}

boolean rf_tdscdma_core_txlin_config_txagc
(
  rfm_device_enum_type tx_device,
  rfcom_tdscdma_band_type band,
  uint32 txlm_buffer_index
)
{
  rfcommon_core_txlin_agc_device_info device_info;
  rfcom_band_type_u rfm_band;
  rfcommon_core_txlin_agc_timing_params timing_params;
  boolean api_status = TRUE;
  rf_buffer_intf* rftdscdma_core_txagc_script_buffer_ptr;    
  rf_buffer_intf* asm_scpt_ptr = NULL;
  rfdevice_tdscdma_set_rx_on_type set_rx_on_data;
  rf_buffer_intf* pa_scpt_ptr = NULL;
  void *tx_band_config_ptr = NULL;

  api_status &= rf_tdscdma_core_txlin_get_tx_device_info(tx_device, band, &device_info);

  if (!api_status)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rf_tdscdma_core_txlin_get_tx_device_info() failed for band %d and device type %d", band, tx_device); 
    return api_status;
  }

  /* 
      Param	              Bolt (in usec x10)	D3925 (in usec x10)
      wtr_early_trig_offset	            -80	                    -80
      wtr_late_trig_offset	            -50	                    -50
      pa_trig_offset	               -160                    -160
      qpoet_early_trig_offset	       -280                    -200
      qpoet_late_trig_offset	        -20	                    -20
                                                                */


  timing_params.pa_trig_offset = -160;
  timing_params.papm_early_trig_offset = -280;
  timing_params.papm_late_trig_offset = -20;
  timing_params.wtr_early_trig_offset = -80;
  timing_params.wtr_late_trig_offset = -50;

  rfm_band.tdscdma_band = band;

  /* Tx ASM (RFFE): Set to Tx port */
  rftdscdma_core_txagc_script_buffer_ptr = rf_buffer_create(0, 
                                                            RFTDSCDMA_STANDALONE_MAX_RFFE_SCRIPT_SIZE,
                                                            RFTDSCDMA_STANDALONE_MAX_GRFC_SCRIPT_SIZE);
  
  tx_band_config_ptr = rfc_tdscdma_tx_band_config_get(tx_device, band, RFC_GRFC_DATA);
  
  if(rftdscdma_core_txagc_script_buffer_ptr == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LVL_ERROR, "Initialized rftdscdma_core_txagc_script_buffer_ptr failed!!", 0);   
  }
  else
  {    
    (void)rf_buffer_clear(rftdscdma_core_txagc_script_buffer_ptr);

    if (rfdevice_tdscdma_use_asm(tx_device,  band))
    {
      rfc_grfc_script_entry_type asm_timing_info;
      boolean asm_status;

      if(tx_band_config_ptr)
      {
        rfc_common_tdd_get_grfc_info(tx_band_config_ptr, RFC_TIMING_ASM_CTL, &asm_timing_info);
        
        //asm_timing_info.start_delta = 0;
        //if (rf_tdscdma_mdsp_rfc_timing_debug_flag)
        //{
            MSG_1(MSG_SSID_RF, MSG_LVL_MED, "TxAGC ASM timing: asm_timing_info = %d", asm_timing_info.start_delta);
        //}  		

        asm_status = rfdevice_tdscdma_asm_enable_tx(tx_device,
                                                    band,
                                                    rftdscdma_core_txagc_script_buffer_ptr,
                                                    RFDEVICE_CREATE_SCRIPT, 
                                                    asm_timing_info.start_delta,
                                                    TRUE,
                                                    NULL,
                                                    TDS_ASM_TRIGGER_ONLY); 
        if (!asm_status)
        {
          MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "ASM configuration ERROR on Device %d and Band %d", tx_device, band);     
        }
        else
        {
          /* Use this script buf only when ASM presented and configured correctly */

          const rf_hal_bus_resource_script_type *resource_script = NULL;
          uint16 loop;
          
          asm_scpt_ptr = rftdscdma_core_txagc_script_buffer_ptr;

          resource_script = rf_buffer_get_script_ptr(asm_scpt_ptr, RF_HAL_BUS_RESOURCE_RFFE);            

          if (resource_script != NULL)
          {
            for (loop = 0; loop < resource_script->num_trans; loop++)
            {
              MSG_4(MSG_SSID_RF, MSG_LVL_MED, 
                    "txAGC ASM Debug: loop %d, Addr 0x5x, Data 0x%x, Start_delta = %d", 
                    loop, 
                    resource_script->script_ptr.rffe[loop].addr,
                    resource_script->script_ptr.rffe[loop].data[0],
                    resource_script->script_ptr.rffe[loop].start_delta);
            }        
          }

          MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "TxAGC debug for band %d and device type %d, txlm_buffer_index %d", band, tx_device, txlm_buffer_index); 
        }

        rfc_common_tdd_get_grfc_buffer(tx_band_config_ptr, 
                                       RFC_ANT_SEL, 
                                       RFC_START_SCRIPT, 
                                       rftdscdma_core_txagc_script_buffer_ptr,
                                       0, 
                                       NULL, 
                                       FALSE, 
                                       FALSE); 
      }    
      else
      {
        MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "tx_band_config_ptr is NULL for Device %d and Band %d", tx_device, band);
      }
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, "ASM not presented on Device %d and Band %d", tx_device, band);
    }
	
    if (rfc_tdscdma_grfc_pa_existed())
    {
   
      boolean pa_status;

      pa_status = rfdevice_tdscdma_pa_on_off(tx_device,
                                             band,
                                             TRUE,
                                             rftdscdma_core_txagc_script_buffer_ptr,
                                             RFDEVICE_CREATE_SCRIPT,
                                             0xFFF);

	  if (!pa_status)
      {
        MSG_2(MSG_SSID_RF, MSG_LVL_ERROR, "NSHEN: PA configuration ERROR on Device %d and Band %d", tx_device, band);     
  }  
      else
      {
        // Use this script buf only when ASM presented and configured correctly 
        const rf_hal_bus_resource_script_type *resource_script = NULL;
        uint16 loop;

        pa_scpt_ptr = rftdscdma_core_txagc_script_buffer_ptr;

        resource_script = rf_buffer_get_script_ptr(pa_scpt_ptr, RF_HAL_BUS_RESOURCE_GRFC);            
    
        if (resource_script != NULL)
        {
          for (loop = 0; loop < resource_script->num_trans; loop++)
          {
            MSG_4(MSG_SSID_RF, MSG_LVL_MED, 
                  "txAGC PA Debug: loop %d, GRFC ID %d, Start_delta = %d, Start Logic %d", 
                   loop, 
                   resource_script->script_ptr.grfc->grfc_id,
                   resource_script->script_ptr.grfc->start_delta,
                   resource_script->script_ptr.grfc->start_logic);
          }        
        }
      } 
    }
  }  
  if (!rfm_get_calibration_state())
  {
  set_rx_on_data.band = band;
  set_rx_on_data.rx_on = FALSE;
  set_rx_on_data.execution_type = RFDEVICE_CREATE_SCRIPT; 
  set_rx_on_data.script_buffer = rftdscdma_core_txagc_script_buffer_ptr;
  set_rx_on_data.is_rx_on_ssbi = TRUE;
  set_rx_on_data.is_rf_on_ssbi = TRUE;
  set_rx_on_data.transition_type = RFDEVICE_TDSCDMA_RX_TO_TX_TRANSITION;
  set_rx_on_data.w_time = TRUE; 
  
  rfdevice_tdscdma_rx_cmd_dispatch(tx_device, 
                                   RFCOM_TDSCDMA_MODE, 
                                   band,
                                   RFDEVICE_SET_RX_ON,
                                   &set_rx_on_data
                                   );
  }
  api_status &= rfcommon_core_txlin_config_agc(RFM_TDSCDMA_MODE,
                                               tx_device,
                                               rfm_band,
                                               txlm_buffer_index,       //int32 tx_handle_id, 
                                               0,                       //uint16 script_buffer_id, use default buffer 
                                               device_info.wtr_device,  // void* wtr_device,
                                               device_info.pa_device,   // void* pa_device,
                                               device_info.papm_device, // void* papm_device,
                                               asm_scpt_ptr,            // rf_buffer_intf* tech_append_tx_agc,
                                               &timing_params);																														

  (void)rf_buffer_destroy(rftdscdma_core_txagc_script_buffer_ptr);
  
  if (!api_status)
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfcommon_core_txlin_config_agc() failed for band %d and device type %d", band, tx_device); 
    return api_status;
  }
  else
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_MED, "rfcommon_core_txlin_config_agc() success for band %d and device type %d", band, tx_device); 
  }

  return api_status;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function update the Tx linearizer with temp comp only

  @details
  Initial linearizer on the current band/channel needs to be populated first 

*/
boolean rf_tdscdma_core_txlin_temp_comp_update
(
  int16 pin_comp_val[],
  int16 pout_comp_val[]
)
{
  boolean status_flag = TRUE;
  uint8 pa_index = 0;
  tx_band_cal_type *tx_band_cal_data_ptr = NULL;
  tx_linearizer_search_param_type search_param;
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_final_table;
  tx_linearizer_table_data_type txlin_curr_table;
  rfcom_band_type_u rfcom_band;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcom_tdscdma_band_type curr_band_tx;
  /* Power cap/boost algorithm */
  rfcommon_core_txlin_cap_boost_type cap_boost_data;
  boolean skip_pout_temp_comp = FALSE;


  /*No need to populate linearizer data in FTM mode*/
  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE))||
       (rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX))
  {
    return FALSE;  
  }

  /*memory allocations*/
  txlin_first_sweep_tbl.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  txlin_final_table.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if((txlin_first_sweep_tbl.lin_data == NULL)||
     (txlin_final_table.lin_data == NULL))
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Fail to allocate memory for processing linearizer");
    return FALSE;
  }

  if (rfcommon_autopin_is_enabled(RFM_TDSCDMA_MODE))
  {
    skip_pout_temp_comp = TRUE;
  }
  
  curr_band_tx = rf_tdscdma_mc_state.curr_band_tx;

  /*For the current band, get the pointer to complete linearizer band cal data  */
  tx_band_cal_data_ptr = rf_tdscdma_core_txlin_get_band_data(curr_band_tx);

  /*Check for NULL pointers*/
  if ( tx_band_cal_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"TDSCDMA TxLin update: Null pointer access to cal data");
    return FALSE;
  }

  if(tx_band_cal_data_ptr->tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid TDSCDMA tx cal chan size: %d",tx_band_cal_data_ptr->tx_cal_chan_size); 
    return FALSE;
  }
  
  /* Populate cap-boost data for common TxAGC */  
  cap_boost_data.max_pwr_ovr_algo = 
     rf_tdscdma_core_xpt_get_cfg_param( curr_band_tx, XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val =
     rf_tdscdma_core_xpt_get_cfg_param( curr_band_tx, XPT_CFG_CMN_MAX_PWR_OVR_VAL );
    
  for ( pa_index = 0; pa_index < rf_tdscdma_core_current_txlin_data.num_valid_pa_state; pa_index++)
  {
    /* this should never happen but so that KW won't complain... */
    if (pa_index >= RFCOM_TXLIN_NUM_LINEARIZERS)
    {
      continue;
    }

    /* Check if the current linearizer table is valid */
    if (rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index].chan_freq!=\
        rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx))
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Current Lin data not valid for pa_index:%d",pa_index);
      continue;
    }
    
    /* Clear final table for population */
    memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
    txlin_final_table.xpt_data = NULL;  

    /* Clear current first sweep table for population */
    memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
    txlin_first_sweep_tbl.xpt_data = NULL;

    /* Populate search parameter*/
    search_param.rfm_device = rf_tdscdma_mc_state.curr_pri_device;
    search_param.tx_freq = 
      rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP; 
    search_param.pa_range = pa_index;

    /* Search first sweep linearizer data */
    if(rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1)
    {
      /*Obtain the first sweep data*/
      status_flag = rfcommon_core_freq_comp_linearizer( &search_param,
                                                        tx_band_cal_data_ptr,
                                                        &txlin_first_sweep_tbl,
                                                        (void*)&cap_boost_data,
                                                        TRUE );

      if (!status_flag)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," No result populating first sweep linearizer from NV for PA index:%d ", search_param.pa_range);
        /* Free the temporary table */
        modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
        txlin_first_sweep_tbl.lin_data = NULL;
        return status_flag;
      }

      /* Need to sort TX Table in increasing order
       *
       * QSORT uses the function given (rfcommon_core_qsort_compare_increasing_pwr)
       * to compare elements, and sorts the whole linearizer row based on comparing
       * tx power level.
       */
      if(rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_3)
      {
        qsort( &(txlin_first_sweep_tbl.lin_data->table),  
                RFCOM_TXLIN_NUM_SEGMENTS,
                sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
      }  
    }

    /* point to the saved freq_comped lin */
    txlin_curr_table.lin_data = &rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index];
    txlin_curr_table.aux_lin_data = rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index];
    //txlin_curr_table.xpt_data doesn't matter here

    status_flag = rfcommon_core_txlin_temp_vs_freq_comp_update_v2(
                                                   rf_tdscdma_core_current_txlin_data.temp_comp_algo,
                                                   &txlin_curr_table,
                                                   &txlin_final_table,
                                                   &txlin_first_sweep_tbl,
                                                   pin_comp_val[pa_index],
                                                   pout_comp_val[pa_index],
                                                                  rf_tdscdma_core_current_txlin_data.txlin_valid_entries[pa_index],
                                                                  skip_pout_temp_comp);

    if (!status_flag)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"temp comp populate failure for pa_index =%d ", pa_index);
    }

    #if 1
    /* Copy the temp-comped linearizer to shadow for logging */
    memscpy(&rf_tdscdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
            txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
    #endif
    rf_tdscdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val[pa_index];
    rf_tdscdma_core_current_txlin_data.txlin_pout_comp[pa_index] = pout_comp_val[pa_index];

    /* Load the comped linearizer table */

    /* Update common band information */
    rfcom_band.tdscdma_band = curr_band_tx;

    /* Initialize PA STM info */
    pa_stm_info.pa_state = pa_index;
    pa_stm_info.pa_range = rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_range_map[pa_index]; // pa_range

    /* check the flag once more in case mc_disable_tx happens during temp comp */
    /* this check will reduce the chance to use tx handle after it is cleared by mc_disable_tx */
    if( rf_tdscdma_core_temp_comp_is_enabled() && rf_tdscdma_mc_lm_bufs.tx_buf_wrk.is_buf_allocated )
    {
      /* Configure the linearizer to RFLM through common Tx AGC */ 
      status_flag = rfcommon_core_txlin_config_update_linearizer(RFM_TDSCDMA_MODE, 
                                                                 rf_tdscdma_mc_state.curr_pri_device,
                                                                 rfcom_band, 
                                                                 rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,   //working txlm buffer id
                                                                 &txlin_final_table, 
                                                                 &txlin_first_sweep_tbl,
                                                                 &pa_stm_info,
                                                                 skip_pout_temp_comp);

      if (!status_flag)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"txlin_config_update_linearizer failed for pa_index =%d!", pa_index);
      }
    }

  }/* for ( pa_index = 0; pa_index < rf_tdscdma_core_current_txlin_data.num_valid_pa_state; pa_index++) */

  /*Deallocate the first sweep data */
  if (txlin_first_sweep_tbl.lin_data != NULL)
  {
    modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
  }
  /*Deallocate the final lin data */
  if (txlin_final_table.lin_data != NULL)
  {
    modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_final_table.lin_data = NULL;
  }

  RF_MSG_5( RF_HIGH, "TxLin Temp Comp finshed, Pin comp values:%d,%d,%d,%d,%d", 
            pin_comp_val[0],pin_comp_val[1],pin_comp_val[2],pin_comp_val[3],pin_comp_val[4]);
  RF_MSG_5( RF_HIGH, "TxLin Temp Comp finshed, Pout comp values:%d,%d,%d,%d,%d", 
            pout_comp_val[0],pout_comp_val[1],pout_comp_val[2],pout_comp_val[3],pout_comp_val[4]);

  return status_flag;
}


/*--------------------------------------------------------------------------*/
/*!
  @brief 
  This function update the Tx linearizer using AutoPin

  @details
  Initial linearizer on the current band/channel needs to be populated first 

*/
boolean rf_tdscdma_core_txlin_autopin_update
(
  int16 pin_comp_val,
  uint8 pa_index
)
{
  boolean status_flag = TRUE;
  tx_band_cal_type *tx_band_cal_data_ptr = NULL;
  tx_linearizer_search_param_type search_param;
  tx_linearizer_table_data_type txlin_first_sweep_tbl;
  tx_linearizer_table_data_type txlin_final_table;
  tx_linearizer_table_data_type txlin_curr_table;
  rfcom_band_type_u rfcom_band;
  rfcommon_core_txlin_agc_pa_stm_info pa_stm_info;
  rfcom_tdscdma_band_type curr_band_tx;
  /* Power cap/boost algorithm */
  rfcommon_core_txlin_cap_boost_type cap_boost_data;
  rflm_autopin_dm_template_t *autopin_template = NULL;

  /*No need to populate linearizer data in FTM calibration and non-Tx modes*/
  if ( (IS_FTM_IN_TEST_MODE()&&(rfm_get_calibration_state() == TRUE))||
       (rf_tdscdma_mc_state.rf_state[RF_PATH_0] != RF_TDSCDMA_STATE_RXTX))
  {
    return FALSE;
  }

  /*memory allocations*/
  txlin_first_sweep_tbl.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  txlin_final_table.lin_data = 
    (tx_linearizer_table_type *)modem_mem_alloc(sizeof(tx_linearizer_table_type),MODEM_MEM_CLIENT_RFA ); 
  if((txlin_first_sweep_tbl.lin_data == NULL)||
     (txlin_final_table.lin_data == NULL))
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"Fail to allocate memory for processing linearizer");
    return FALSE;
  }
  
  curr_band_tx = rf_tdscdma_mc_state.curr_band_tx;

  /*For the current band, get the pointer to complete linearizer band cal data  */
  tx_band_cal_data_ptr = rf_tdscdma_core_txlin_get_band_data(curr_band_tx);

  /*Check for NULL pointers*/
  if ( tx_band_cal_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"TDSCDMA TxLin update: Null pointer access to cal data");
    return FALSE;
  }

  if(tx_band_cal_data_ptr->tx_cal_chan_size > RFCOM_TXLIN_NUM_CHANNELS)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Invalid TDSCDMA tx cal chan size: %d",tx_band_cal_data_ptr->tx_cal_chan_size); 
    return FALSE;
  }

  if (pa_index >= MIN(rf_tdscdma_core_current_txlin_data.num_valid_pa_state, 8))
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid PA index %d", pa_index); 
    return FALSE;
  }

  /* Get the Pin NV value and use it as the init value for AutoPin convergence
   * rf_tdscdma_core_autopin_pin_comp_init[] was populated in rf_tdscdma_core_temp_comp_vs_freq_get_comp*/
      autopin_template = (rflm_autopin_dm_template_t *)rflm_dm_get_autopin_buf_ptr(rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx);
    if (autopin_template == NULL)
    {
      RF_MSG_2(RF_ERROR,"rf_tdscdma_core_autopin_update_pin: Failed to get AutoPin buffer from DM for Tx handle %d, can't log ref pin %d", 
               rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx, rf_tdscdma_core_autopin_pin_comp_init[pa_index]);
    }
    else
    {
      autopin_template->autopin_ctl.pin_init = rf_tdscdma_core_autopin_pin_comp_init[pa_index];
    }
  pin_comp_val += rf_tdscdma_core_autopin_pin_comp_init[pa_index];

  /* Populate cap-boost data for common TxAGC */  
  cap_boost_data.max_pwr_ovr_algo = 
     rf_tdscdma_core_xpt_get_cfg_param( curr_band_tx, XPT_CFG_CMN_MAX_PWR_OVR_ALGO );
  cap_boost_data.lin_cap_boost_nv_val =
     rf_tdscdma_core_xpt_get_cfg_param( curr_band_tx, XPT_CFG_CMN_MAX_PWR_OVR_VAL );
    
    /* Check if the current linearizer table is valid */
    if (rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index].chan_freq!=\
        rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx))
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"Current Lin data not valid for pa_index:%d",pa_index);
    }
    
    /* Clear final table for population */
    memset(txlin_final_table.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_final_table.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA;
    txlin_final_table.xpt_data = NULL;  

    /* Clear current first sweep table for population */
    memset(txlin_first_sweep_tbl.lin_data,0,sizeof(tx_linearizer_table_type));
    txlin_first_sweep_tbl.aux_lin_data = TX_LINEARIZER_NO_AUX_DATA ;
    txlin_first_sweep_tbl.xpt_data = NULL;

    /* Populate search parameter*/
    search_param.rfm_device = rf_tdscdma_mc_state.curr_pri_device;
    search_param.tx_freq = 
      rf_tdscdma_core_util_get_freq_from_chan(rf_tdscdma_mc_state.curr_chan_tx);
    search_param.tx_waveform = TX_LINEARIZER_FIRST_SWEEP; 
    search_param.pa_range = pa_index;

    /* Search first sweep linearizer data */
    if(rf_tdscdma_core_current_txlin_data.multi_lin_rev != TX_MULTI_LIN_REV_1)
    {
      /*Obtain the first sweep data*/
      status_flag = rfcommon_core_freq_comp_linearizer(&search_param,
                                                       tx_band_cal_data_ptr,
                                                       &txlin_first_sweep_tbl,
                                                       (void*)&cap_boost_data,
                                                       TRUE );

      if (!status_flag)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR," No result populating first sweep linearizer from NV for PA index:%d ", search_param.pa_range);
        /* Free the temporary table */
        modem_mem_free(txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
        txlin_first_sweep_tbl.lin_data = NULL;
        return status_flag;
      }

      /* Need to sort TX Table in increasing order
       *
       * QSORT uses the function given (rfcommon_core_qsort_compare_increasing_pwr)
       * to compare elements, and sorts the whole linearizer row based on comparing
       * tx power level.
       */
      if(rf_tdscdma_core_current_txlin_data.multi_lin_rev == TX_MULTI_LIN_REV_3)
      {
        qsort( &(txlin_first_sweep_tbl.lin_data->table),  
                RFCOM_TXLIN_NUM_SEGMENTS,
                sizeof(tx_linearizer_row_type), rfcommon_core_qsort_compare_increasing_pwr );
      }  
    }

    /* point to the saved freq_comped lin */
    txlin_curr_table.lin_data = &rf_tdscdma_core_current_txlin_data.current_lin_tbl[pa_index];
    txlin_curr_table.aux_lin_data = rf_tdscdma_core_current_txlin_data.current_aux_data[pa_index];
    //txlin_curr_table.xpt_data doesn't matter here

    status_flag = rfcommon_core_txlin_temp_vs_freq_comp_update_v2(rf_tdscdma_core_current_txlin_data.temp_comp_algo,
                                                                  &txlin_curr_table,
                                                                  &txlin_final_table,
                                                                  &txlin_first_sweep_tbl,
                                                                  pin_comp_val,
                                                                  0,
                                                                  rf_tdscdma_core_current_txlin_data.txlin_valid_entries[pa_index],
                                                                  TRUE);

    if (!status_flag)
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"temp comp populate failure for pa_index =%d ", pa_index);
    }

    #if 1
    /* Copy the temp-comped linearizer to shadow for logging */
    memscpy(&rf_tdscdma_core_current_txlin_data.current_lin_tbl_comp[pa_index], sizeof(tx_linearizer_table_type),
            txlin_final_table.lin_data,sizeof(tx_linearizer_table_type));
    #endif
    rf_tdscdma_core_current_txlin_data.txlin_pin_comp[pa_index] = pin_comp_val;

    /* Load the comped linearizer table */

    /* Update common band information */
    rfcom_band.tdscdma_band = curr_band_tx;

    /* Initialize PA STM info */
    pa_stm_info.pa_state = pa_index;
    pa_stm_info.pa_range = rfnv_tdscdma_tx_tbl_ptr->pa_static.pa_range_map[pa_index]; // pa_range

    /* check the flag once more in case mc_disable_tx happens during temp comp */
    /* this check will reduce the chance to use tx handle after it is cleared by mc_disable_tx */
    if( rf_tdscdma_core_temp_comp_is_enabled() && rf_tdscdma_mc_lm_bufs.tx_buf_wrk.is_buf_allocated )
    {
      /* Configure the linearizer to RFLM through common Tx AGC */ 
      status_flag = rfcommon_core_txlin_config_update_linearizer(RFM_TDSCDMA_MODE, 
                                                                 rf_tdscdma_mc_state.curr_pri_device,
                                                                 rfcom_band, 
                                                                 rf_tdscdma_mc_lm_bufs.tx_buf_wrk.buf_idx,   //working txlm buffer id
                                                                 &txlin_final_table, 
                                                                 &txlin_first_sweep_tbl,
                                                                 &pa_stm_info,
                                                                 TRUE) ;

      if (!status_flag)
      {
        MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"txlin_config_update_linearizer failed for pa_index =%d!", pa_index);
      }
    }

  /*Deallocate the first sweep data */
  if (txlin_first_sweep_tbl.lin_data != NULL)
  {
    modem_mem_free((void*)txlin_first_sweep_tbl.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_first_sweep_tbl.lin_data = NULL;
  }
  /*Deallocate the final lin data */
  if (txlin_final_table.lin_data != NULL)
  {
    modem_mem_free(txlin_final_table.lin_data, MODEM_MEM_CLIENT_RFA);
    txlin_final_table.lin_data = NULL;
  } 

  RF_MSG_3( RF_HIGH, "rf_tdscdma_core_txlin_autopin_update, initial Pin %d, Pin %d, PA state %d", 
            rf_tdscdma_core_autopin_pin_comp_init[pa_index], pin_comp_val, pa_index);
  
  return status_flag;
}

