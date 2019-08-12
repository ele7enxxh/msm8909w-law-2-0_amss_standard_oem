#ifndef RFCOMMON_AUTOPIN_MSM_H
#define RFCOMMON_AUTOPIN_MSM_H

/*!
  @file rfcommon_autopin_msm.h

  @brief
  This file contains Common HAL for the target.
*/

/*============================================================================== 
   
  Copyright (c) 2010 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/common/inc/rfcommon_autopin_msm.h#1 $ 
   
when       who     what, where, why 
--------   ---     -------------------------------------------------------------
12/08/15   vkm     added new api for params capture for autopin 
12/07/15   kma     Updated time offset for CDMA and LTE
12/04/15   kma     Updated time offset for W and TDS
12/01/15   ag      Modify WCDMA timing offset to more suitable value
11/18/15   whz     Move window size macros to header file
11/13/15   kma     Updated WTR capture size API interface
11/03/15   kma     Initial version
==============================================================================*/ 

#include "comdef.h"
#include "rfcom.h"
#include "lm_types.h"
#include "rflm_api_cmn.h"

/*PAR capture window size in us unit*/
#define AUTOPIN_PAR_CAPTURE_WINDOW_WCDMA    126
#define AUTOPIN_PAR_CAPTURE_WINDOW_CDMA     123
#define AUTOPIN_PAR_CAPTURE_WINDOW_TDSCDMA  128
#define AUTOPIN_PAR_CAPTURE_WINDOW_LTE      118

/*Time alignment offsets*/
#define AUTOPIN_PAR_CDMA_CAPTURE_TIME_OFFSET        ((uint32)(19200000/1000000.0*24))
#define AUTOPIN_PAR_WCDMA_CAPTURE_TIME_OFFSET       ((uint32)(19200000/1000000.0*24))
#define AUTOPIN_PAR_LTE_CAPTURE_TIME_OFFSET         ((uint32)(19200000/1000000.0*30))
#define AUTOPIN_PAR_TDSCDMA_CAPTURE_TIME_OFFSET     ((uint32)(19200000/1000000.0*21))

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/

/*!
  @brief
  API to get TXC rollover period

  @details

*/
boolean rfcommon_msm_get_txc_rollover_period(lm_handle_type txlm_handle, 
                                             uint32 *txc_input_freq, 
                                             uint32 *txc_rollover_period,
                                             uint32 *txrot_clk);

/*----------------------------------------------------------------------------*/

/*!
  @brief
  API to get PAR capture params

  @details

*/
void rfcommon_msm_get_par_capture_params(rflm_tech_id_t rflm_tech,
                                         uint32 *reflog_capture_length, 
                                         uint32 *downsample_rate,
                                         uint32 *capture_delay,
                                         uint16 *wtr_capture_length);

/*----------------------------------------------------------------------------*/


/*!
  @brief
  API to get PAR capture params for LTE

  @details

*/
void rfcommon_msm_get_par_capture_params_lte(uint32 *reflog_capture_length,
                                         uint32 *downsample_rate,
                                         uint32 *capture_delay,
                                         uint16 *wtr_capture_length,
                                         rfcom_lte_bw_type lte_bandwidth);

/*----------------------------------------------------------------------------*/

/*!
  @brief
  Stores autopin capture results in EFS.

  @details
  This function writes the autopin capture results to EFS



===========================================================================*/

boolean rfcommon_autopin_msm_store_data(uint32 *reflog_capture_data,
                                        uint8  *wtr_capture_data);


#ifdef __cplusplus
}
#endif

#endif /*RFCOMMON_AUTOPIN_MSM_H*/
