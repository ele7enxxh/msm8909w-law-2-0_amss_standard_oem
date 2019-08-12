#ifndef RF_TDSCDMA_CORE_RXCTL_H
#define RF_TDSCDMA_CORE_RXCTL_H
/*!
  @file
  rf_tdscdma_core_rxctl.h

  @brief
  This is the header file for TDSCDMA Rx control functionality. 

  @details

*/

/*============================================================================== 
   
  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/core/inc/rf_tdscdma_core_rxctl.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
07/17/14   ms      Sawless RX support
11/07/13   nsh     FED RxAGC Feature, move struct to .h for easy including
05/27/11   jhe     Initial version
==============================================================================*/ 

#include "rf_tdscdma_mdsp.h"

/*!
  @brief
  TDSCDMA Rx Frequency compensation variables
*/
typedef struct
{
  int16 rx_gain_vs_freq[RF_PATHS_TABLE_SIZ][RF_TDSCDMA_MAX_LNA_GAIN_STATES][RF_TDSCDMA_NUM_RX_LIN_STATES];
  uint16 last_chan_checked[RF_PATHS_TABLE_SIZ];
  rf_path_enum_type last_chain_checked;
  rfcom_tdscdma_band_type last_band_checked[RF_PATHS_TABLE_SIZ];
} rf_tdscdma_core_rxctl_data_type;


void rf_tdscdma_core_rxctl_calc_rx_cal_chan_freq( rf_path_enum_type rx_chain, rfcom_tdscdma_band_type band );

void rf_tdscdma_core_rxctl_update_freq_comp
(
  rfm_device_enum_type device,
  rf_path_enum_type rx_chain,
  rfcom_tdscdma_band_type band,
  uint16 rx_channel,
  int16 rx_buf,
  uint8 buf_idx
);

#endif 
