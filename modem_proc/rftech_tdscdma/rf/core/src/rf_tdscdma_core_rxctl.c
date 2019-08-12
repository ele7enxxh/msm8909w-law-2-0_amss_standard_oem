/*!
  @file rf_tdscdma_core_rxctl.c

  @brief
  Provides TDSCDMA Rx control functionality

  @details


*/

/*============================================================================== 
   
  Copyright (c) 2011, 2013 Qualcomm Technologies Incorporated. All Rights Reserved 
   
  Qualcomm Technologies Proprietary 
   
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 07:59:50 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/src/rf_tdscdma_core_rxctl.c#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
11/12/14   jyu     MDSP/SMEM cleanup
11/07/14   jhe     Update for the new Rx NV
10/02/14   jyu     Fixed bug when applying new NVs
09/26/14   ms      Fixed bug for populating new RX NVs
07/17/14   ms      Sawless RX support
11/07/13   nsh     FED RxAGC Feature, move the struct to rf_tdscdma_core_rxctl.h
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
04/07/13   jyu     KW fix
03/15/13   jyu     Merged "Dynamic frequency update"
02/27/13   ms      Diversity support
07/28/11   jyu     Added null pointer checking before access NV ptr 
05/27/11   jhe     Initial version

==============================================================================*/ 

#include "rfa_variation.h"
#include "msg.h"
#include "rfcom.h"
#include "rf_tdscdma_core_rxctl.h"
#include "rf_tdscdma_core_util.h"
#include "rfnv_tdscdma.h"
#include "rf_tdscdma_mdsp.h"
#include "rfcommon_core_error.h"
#include "rfc_tdscdma_data.h"



//debug
boolean use_new_rx_common_nv = FALSE;


rf_tdscdma_core_rxctl_data_type rf_tdscdma_rxfreq_comp;

/* Store the freq of the channel list in rx_cal_chan and c1_rx_cal_chan,
   for all bands */
static uint32 rf_tdscdma_rx_cal_chan_freq[RF_PATHS_TABLE_SIZ][RFCOM_NUM_TDSCDMA_BANDS][RF_TDSCDMA_MAX_CAL_CHAN];


/*- -------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates Cal chan frequency and stores in 
  rf_tdscdma_rx_cal_chan_freq

  @detail
  It needs to be called before calculating Rx freq comp
  If n(<16) channels are used for a band, it is assumed the first n consecutive NV are filled 
  with the channel list, followed by 0s

 */
void rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq( rf_path_enum_type rx_chain, rfcom_tdscdma_band_type band )
{
  uint16 chan_index =0;
  rfnv_tdscdma_rx_tbl_type *rx_nv_tbl_ptr;

  rfnv_rx_cal_data_typeid_118_type *rx_cal_data = NULL;
  //uint16 number_of_rx_freq_comp_elements  = 0;

  rx_nv_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(band);

  if (rx_nv_tbl_ptr == NULL)
  {
    MSG_ERROR("rx_nv_tbl_ptr is NULL",0,0,0);
    return;
  }

  /* Obtain new Rx Freq comp structures from NV Table pointer. */
  //number_of_rx_freq_comp_elements =  rx_nv_tbl_ptr->rx_band_cal_data.variant_marker.num_elements;
  rx_cal_data                     =  rx_nv_tbl_ptr->rx_band_cal_data.cal_118_data;

  /* New NV */
  /* assuming all path has same rx cal channel list, using the first to fill both path */
  if(rx_cal_data != NULL)
  {
    if (rx_chain == RF_PATH_0)
    {
      for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
      {
        rf_tdscdma_rx_cal_chan_freq[0][band][chan_index] = 0;
      }

      if ((rx_cal_data->channel_list[0] == 0) &&
          (rx_cal_data->channel_list[0] == rx_cal_data->channel_list[1]))
      {
        /* cal chan are empty */     
        MSG_ERROR("rx_cal_data->channel_list[] is empty",0,0,0);
      }
      else
      {
        for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
        {      
          if (rx_cal_data->channel_list[chan_index] != 0)
          {
            rf_tdscdma_rx_cal_chan_freq[0][band][chan_index] = 
               rf_tdscdma_core_util_get_freq_from_uarfcn(rx_cal_data->channel_list[chan_index], band);
          }
          else
          {
            break;
          }             
        }
      }
    }
    else if (rx_chain == RF_PATH_1)
    {
      for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
      {
        rf_tdscdma_rx_cal_chan_freq[1][band][chan_index] = 0;
      }

      if ((rx_cal_data->channel_list[0] == 0) &&
          (rx_cal_data->channel_list[0] == rx_cal_data->channel_list[1]))
      {     
        /* cal chan are empty */
        MSG_ERROR("rx_cal_data->channel_list[] is empty",0,0,0);
      }
      else
      {
        for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
        {      
          if (rx_cal_data->channel_list[chan_index] !=0 )
          {
            rf_tdscdma_rx_cal_chan_freq[1][band][chan_index] = 
               rf_tdscdma_core_util_get_freq_from_uarfcn(rx_cal_data->channel_list[chan_index], band);
          }
          else
          {
            break;
          }             
        }
      }
    }  
    else
    {
      MSG_ERROR("RF_PATH is incorrect: %d\n", rx_chain, 0, 0);
    }

    return;
  }

  /* Old NV */
  if (rx_chain == RF_PATH_0)
  {
    for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
    {
      rf_tdscdma_rx_cal_chan_freq[0][band][chan_index] = 0;
    }
    
    if ((rx_nv_tbl_ptr->rx_cal_chan[0] == 0) &&
        (rx_nv_tbl_ptr->rx_cal_chan[0] == rx_nv_tbl_ptr->rx_cal_chan[1]))
    {
      /* cal chan are empty */     
      MSG_ERROR("rx_nv_tbl_ptr->rx_cal_chan[] is empty",0,0,0);
    }
    else
    {
      for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
      {      
        if (rx_nv_tbl_ptr->rx_cal_chan[chan_index] != 0)
        {
          rf_tdscdma_rx_cal_chan_freq[0][band][chan_index] = 
             rf_tdscdma_core_util_get_freq_from_uarfcn(rx_nv_tbl_ptr->rx_cal_chan[chan_index], band);
        }
        else
        {
          break;
        }             
      }
    }
  }
  else if (rx_chain == RF_PATH_1)
  {
    for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
    {
      rf_tdscdma_rx_cal_chan_freq[1][band][chan_index] = 0;
    }

    if ((rx_nv_tbl_ptr->rx_div_cal_chan[0] == 0) &&
        (rx_nv_tbl_ptr->rx_div_cal_chan[0] == rx_nv_tbl_ptr->rx_div_cal_chan[1]))
    {     
      /* cal chan are empty */
      MSG_ERROR("rx_nv_tbl_ptr->rx_div_cal_chan[] is empty",0,0,0);
    }
    else
    {
      for(chan_index=0; chan_index<RF_TDSCDMA_MAX_CAL_CHAN; chan_index++)
      {      
        if (rx_nv_tbl_ptr->rx_div_cal_chan[chan_index] !=0 )
        {
          rf_tdscdma_rx_cal_chan_freq[1][band][chan_index] = 
             rf_tdscdma_core_util_get_freq_from_uarfcn(rx_nv_tbl_ptr->rx_div_cal_chan[chan_index], band);
        }
        else
        {
          break;
        }             
      }
    }
  }  
  else
  {
    MSG_ERROR("RF_PATH is incorrect: %d\n", rx_chain, 0, 0);
  }

  return;
}


/*- -------------------------------------------------------------------------*/
/*!
  @brief
  This function calculates TDSCDMA Rx frequency comp data and updates to mdsp.

  @detail
  This function interpolates frequency compensation items from the channel
  number, and applies the interpolation results to all affected frequency
  compensation items.

  @param rx_chain RF primary or secondary path
  @param band Operational TDSCDMA band
  @param channel Operational TDSCDMA channel
 */
void rf_tdscdma_core_rxctl_update_freq_comp
(
  rfm_device_enum_type device,
  rf_path_enum_type rx_chain,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
  int16 rx_buf,
  uint8 buf_idx
)
{
  int16 rx_start_bin = 0;   /* starting RX cal channel for bin containing */
  int32 rx_bin_size;        /* number of channels in the bin (not constant) */
  int32 rx_delta_x;         /* scale of channel number in bin */
  int16 delta_y;            /* used to store delta_y between adjacent nv items */
  boolean rx_channel_under_flag = 0; /* flag if lowest RX cal channel > tune value */
  uint32 rx_freq;
  boolean out_of_range_flag = FALSE;
  uint8 i =0;
  uint8 j =0 ;
  uint8 loop = 0;
  rfnv_tdscdma_rx_tbl_type *rx_nv_tbl_ptr;
  rfc_cfg_params_type cfg;
  rfc_device_info_type *device_info_ptr = NULL;
  uint32 nv_container_idx = 0;  

  rfnv_rx_cal_data_typeid_118_type* rx_cal_data = NULL;
  uint16 number_of_rx_freq_comp_elements  = 0;
  
  /* Get NV container from RFC */
  cfg.band = band;
  cfg.req = RFC_REQ_DEFAULT_GET_DATA;
  cfg.logical_device = device;
  cfg.path = rx_chain;
  cfg.rx_tx = RFC_CONFIG_RX;
  cfg.alternate_path = rf_tdscdma_mc_state.alt_path[rx_chain];
  cfg.port_state = (rfc_port_state_type)rf_tdscdma_mc_state.port_state;

  if (rfc_tdscdma_get_devices_info_data(&cfg, &device_info_ptr) == FALSE)
  {
    MSG_4(MSG_SSID_RF, MSG_LVL_ERROR, 
          "ERR: cannot get Data from RFC for device %d, band %d, alt_path %d, port_state %d",
          device, 
          band,
          cfg.alternate_path,
          cfg.port_state );
    return;
  }

  if (device_info_ptr == NULL)
  {
    MSG_4(MSG_SSID_RF, MSG_LVL_ERROR, 
          "ERR: NULL device info returned for device %d, band %d, alt_path %d, port_state %d",
          device, 
          band,
          cfg.alternate_path,
          cfg.port_state );
    return;
  }

  rx_nv_tbl_ptr = rf_tdscdma_core_util_init_rx_nv_table(band);

  nv_container_idx = device_info_ptr->nv_container;

  if (rx_nv_tbl_ptr == NULL)
  {
    MSG_ERROR("rx_nv_tbl_ptr is NULL for rx_chain %d, band %d and channel %d", rx_chain, band, rx_channel);
    return;
  }

  if (rx_chain > RF_PATH_1) 
  {
    MSG_ERROR("RF_PATH %d is wrong", rx_chain, 0, 0);
    return;
  }

  if (band >= RFCOM_NUM_TDSCDMA_BANDS)
  {
    MSG_ERROR("Band %d is wrong", band, 0, 0);
    return;
  }


  /* Obtain new Rx Freq comp structures from NV Table pointer. */
  number_of_rx_freq_comp_elements =  rx_nv_tbl_ptr->rx_band_cal_data.variant_marker.num_elements;
  rx_cal_data                     =  rx_nv_tbl_ptr->rx_band_cal_data.cal_118_data;

  if(rx_cal_data != NULL)
  {
    use_new_rx_common_nv = TRUE;
    MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, 
          "rf_tdscdma_core_rxctl_update_freq_comp: using NEW RX NV for device %d, band %d",
          device, band);

  }
  else
  {
    use_new_rx_common_nv = FALSE;
    MSG_2(MSG_SSID_RF, MSG_LVL_HIGH, 
          "rf_tdscdma_core_rxctl_update_freq_comp: using OLD RX NV for device %d, band %d",
          device, band);
  }

  /* update the last channel and band checked with the channel passed in */
  rf_tdscdma_rxfreq_comp.last_chan_checked[rx_chain] = rx_channel;
  rf_tdscdma_rxfreq_comp.last_band_checked[rx_chain] = band;
  rf_tdscdma_rxfreq_comp.last_chain_checked = rx_chain;

  rx_freq = rf_tdscdma_core_util_get_freq_from_uarfcn(rx_channel, band); 

  /* Checking against the 1st element of the RX cal channel freq list */
  if (rx_freq <= rf_tdscdma_rx_cal_chan_freq[rx_chain][band][0])
  {
    rx_start_bin = 0;
    rx_channel_under_flag ++;   /* set flag if we are under the cal table range */
  }
  /* search for frequency bin that contains the channel that is being compensated
     or if the bin contains zero we went off the table, use the last value */
  else
  {
    out_of_range_flag = FALSE;
    while ( rx_start_bin < (RF_TDSCDMA_MAX_CAL_CHAN-1) )
    {
      /* note: freq tables with less than 16 elements in them must be padded with 0's */
      if ( rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin+1] == 0 )
      {
        out_of_range_flag = TRUE;
        break;
      }
      else if (rx_freq <= rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin+1])
      {
        break;            /* exit*/
      }
      else
      {
        rx_start_bin ++;     /* check next bin*/
      }
    }
  }

  if (rx_start_bin == (RF_TDSCDMA_MAX_CAL_CHAN-1) || out_of_range_flag)
  {
    rx_bin_size = 0;
  }
  else
  {
    rx_bin_size=(int32)(rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin+1]-
                rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin]);
  }  

  if (use_new_rx_common_nv == TRUE)
  {
    /* Outside the array case */
    if ((rx_bin_size <= 0) || (rx_channel_under_flag == 1))
    {
      /* update rx freq comp items */   
      for(loop=0; loop<number_of_rx_freq_comp_elements; loop++)
      {    
        #if 0
        MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
              "RxFreqComp Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
              loop,
              rx_cal_data->path_idx, 
              nv_container_idx,
              rx_cal_data->cal_type);
        #endif
            
        if((RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_LL == rx_cal_data->cal_type) && (nv_container_idx == rx_cal_data->path_idx))
        {
          j = 0;

          for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++) 
          {
            rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
             rx_cal_data->freq_comp_offset[i][rx_start_bin];
          }       

        #if 0
        MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
              "RxFreqComp(LL) Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
              loop,
              rx_cal_data->path_idx, 
              nv_container_idx,
              rx_cal_data->cal_type);
        #endif
  
        }
        else if ((RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_HL == rx_cal_data->cal_type) && (nv_container_idx == rx_cal_data->path_idx) )
        {
          j = 1;

          for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++) 
          {
            rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
             rx_cal_data->freq_comp_offset[i][rx_start_bin];
          }           

        #if 0
        MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
              "RxFreqComp(HL) Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
              loop,
              rx_cal_data->path_idx, 
              nv_container_idx,
              rx_cal_data->cal_type);
        #endif
        }
        rx_cal_data++;
      }//end for(loop=0; loop<number_of_rx_freq_comp_elements; loop++)
    }
    else  /* interpolate the item if it's delta_y is not zero */
    {
      if (rx_start_bin < (RF_TDSCDMA_MAX_CAL_CHAN -1))
      {
        /* ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
        /* delta_x is the distance from your local channel to the beginning of therm_bin_array bin, -1 */
        rx_delta_x = (int32)((rx_freq-rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin])-1);

        for(loop=0; loop < number_of_rx_freq_comp_elements; loop++)
        {
          #if 0
          MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
                "RxFreqInterpolate Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
                loop,
                rx_cal_data->path_idx, 
                nv_container_idx,
                rx_cal_data->cal_type);
          #endif
        
          if((RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_LL == rx_cal_data->cal_type) && (nv_container_idx == rx_cal_data->path_idx) )
          {
            j=0;

            /* for each gain state */
            for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++)
            {
              delta_y = (rx_cal_data->freq_comp_offset[i][rx_start_bin+1]-
                              rx_cal_data->freq_comp_offset[i][rx_start_bin]);
            
              (delta_y<0)?delta_y--:delta_y++;
            
              /* take the ratio of the delta_x to the entire bin size, and apply
                 it to the delta_y.  This is the value you need to add to the
                 comp_vs_freq value of your left bin edge to get the comp_vs_freq
                 value of your rx frequency */
              rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
                 (int16)(delta_y*rx_delta_x/rx_bin_size) +
                 rx_cal_data->freq_comp_offset[i][rx_start_bin];

            }      
            #if 0
            MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
                  "RxFreqInterpolate(LL) Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
                  loop,
                  rx_cal_data->path_idx, 
                  nv_container_idx,
                  rx_cal_data->cal_type);
            #endif 
            //break;
          }
          else if((RFNV_RX_CAL_DATA_NV_TYPE_TDSCDMA_HL == rx_cal_data->cal_type) && (nv_container_idx == rx_cal_data->path_idx) )
          {
            j=1;
            
            /* for each gain state */
            for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++)
            {
              delta_y = (rx_cal_data->freq_comp_offset[i][rx_start_bin+1]-
                            rx_cal_data->freq_comp_offset[i][rx_start_bin]);
          
              (delta_y<0)?delta_y--:delta_y++;
          
            /* take the ratio of the delta_x to the entire bin size, and apply
               it to the delta_y.  This is the value you need to add to the
               comp_vs_freq value of your left bin edge to get the comp_vs_freq
               value of your rx frequency */
              rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
               (int16)(delta_y*rx_delta_x/rx_bin_size) +
               rx_cal_data->freq_comp_offset[i][rx_start_bin];

            }
            #if 0
            MSG_4(MSG_SSID_RF, MSG_LVL_HIGH, 
                  "RxFreqInterpolate(HL) Debug. loop: %d, rx_cal_data->path_idx: %d, nv_container_idx: %d, rx_cal_data->cal_type: %d",
                  loop,
                  rx_cal_data->path_idx, 
                  nv_container_idx,
                  rx_cal_data->cal_type);
            #endif 
            //break;
          }

          rx_cal_data++;
        
        }//end of for(loop=0; loop < number_of_rx_freq_comp_elements; loop++)    
        
      }
      else
      {
        MSG_ERROR("rf_tdscdma_core_rxctl_update_freq_offsets(): rx_start_bin = 15!!",0,0,0);
      }    
    }
  }//end use_new_rx_common_nv
  else // Use old NV
  {
    /* Outside the array case */
    if ( (rx_bin_size <= 0) || (rx_channel_under_flag == 1) )
    {
      /* update rx freq comp items */
      if (rx_chain == RF_PATH_0)
      {
        for(j=0; j<RF_TDSCDMA_NUM_RX_LIN_STATES; j++)
        {     
          for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++) 
          {
            rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
              rx_nv_tbl_ptr->rx_gain_vs_freq[i][rx_start_bin];
          } 
        }           
      }
      else if (rx_chain == RF_PATH_1)
      {
        for(j=0; j<RF_TDSCDMA_NUM_RX_LIN_STATES; j++)
        {
          for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++) 
          {
            rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
              rx_nv_tbl_ptr->rx_div_gain_vs_freq[i][rx_start_bin];
          }
        }
      }
    }
    else  /* interpolate the item if it's delta_y is not zero */
    {
      if (rx_start_bin < (RF_TDSCDMA_MAX_CAL_CHAN -1))
      {
        if (rx_chain == RF_PATH_0)
        {
          /* ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
          /* delta_x is the distance from your local channel to the beginning of the
             bin, -1 */
          rx_delta_x = (int32)((rx_freq-rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin])-1);

          for(j=0; j<RF_TDSCDMA_NUM_RX_LIN_STATES; j++)
          {
            /* for each gain state */
            for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++)
            {
              delta_y = (rx_nv_tbl_ptr->rx_gain_vs_freq[i][rx_start_bin+1]-
                            rx_nv_tbl_ptr->rx_gain_vs_freq[i][rx_start_bin]);
         
              (delta_y<0)?delta_y--:delta_y++;
         
            /* take the ratio of the delta_x to the entire bin size, and apply
               it to the delta_y.  This is the value you need to add to the
               comp_vs_freq value of your left bin edge to get the comp_vs_freq
               value of your rx frequency */
              rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
               (int16)(delta_y*rx_delta_x/rx_bin_size) +
               rx_nv_tbl_ptr->rx_gain_vs_freq[i][rx_start_bin];
            }
          }
        }
        else if (rx_chain == RF_PATH_1)
        {
          /* ensures that dx/binsize <1 at bin boundary, when delta_y = 1. */
          /* delta_x is the distance from your local channel to the beginning of the
             bin, -1 */
          rx_delta_x = (int32)((rx_freq-rf_tdscdma_rx_cal_chan_freq[rx_chain][band][rx_start_bin])-1);

          for(j=0; j<RF_TDSCDMA_NUM_RX_LIN_STATES; j++)
          {
            /* for each gain state */
            for(i=0; i<RF_TDSCDMA_MAX_LNA_GAIN_STATES; i++)
            {
              delta_y = (rx_nv_tbl_ptr->rx_div_gain_vs_freq[i][rx_start_bin+1]-
                                rx_nv_tbl_ptr->rx_div_gain_vs_freq[i][rx_start_bin]);
             
              (delta_y<0)?delta_y--:delta_y++;
             
                /* take the ratio of the delta_x to the entire bin size, and apply
                   it to the delta_y.  This is the value you need to add to the
                   comp_vs_freq value of your left bin edge to get the comp_vs_freq
                   value of your rx frequency */
              rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][i][j] =
                   (int16)(delta_y*rx_delta_x/rx_bin_size) +
                   rx_nv_tbl_ptr->rx_div_gain_vs_freq[i][rx_start_bin];
            }
          }
        }
      }
      else
      {
        MSG_ERROR("rf_tdscdma_core_rxctl_update_freq_offsets(): rx_start_bin = 15!!",0,0,0);
      }    
    }
  } //end else of use_new_rx_common_nv

  if (rx_chain > RF_PATH_1)
  {
    MSG_ERROR("rf_tdscdma_core_rxctl_update_freq_offsets(): rx path= %d is not valid!!",rx_chain,0,0);
  }

  MSG_5(MSG_SSID_RF, MSG_LVL_HIGH, 
        "Updated Rx freq comp for device %d, band %d, alt_path %d, port_state %d, NV_container %d",
        device, 
        band,
        cfg.alternate_path,
        cfg.port_state,
        nv_container_idx);

  return;
}


uint16 rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp(rf_path_enum_type rx_chain, uint8 index, uint8 linearity)
{
  if((index > (RF_TDSCDMA_MAX_LNA_GAIN_STATES-1)) || (rx_chain > RF_PATH_1))
  {
    MSG_ERROR("rf_tdscdma_core_rxctl_get_rfmode_rx_fcomp: Bad index! rx_chain: %d, index: %d",rx_chain,index,0);
    return 0;
  }
  else
  {
    return rf_tdscdma_rxfreq_comp.rx_gain_vs_freq[rx_chain][index][linearity];
  }
}

