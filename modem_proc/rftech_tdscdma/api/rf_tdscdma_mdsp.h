#ifndef RF_TDSCDMA_MDSP_H
#define RF_TDSCDMA_MDSP_H

/*! 
  @file
  rf_tdscdma_mdsp.h
 
  @brief
  This module defines the function prototypes and defintions used by CDMA 
  RF-MDSP Internal(RF) interface.
*/

/*==============================================================================

  Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/api/rf_tdscdma_mdsp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------- 
03/17/15   whz     Add support for TempComp for Jolokia
11/12/14   jyu     MDSP/SMEM cleanup
01/30/14   jyu     Added support for X2T meas
10/02/13   jyu     Removed obsoleted antenna tuner
05/13/13   kb      [XPT] Added support for XPT IQ capture API
04/24/13   qzh     Porting FTM RF code from NIKEL to DIME
04/30/13   ms      Tuner temp comp support
02/08/13   jyu     Removed hard-coded device info
12/04/12   ms      APT support
08/01/12   jyu     Added support to allow create/destroy RF buffers   
07/16/12   jps     Initial FTM burst TX/RX framework
07/11/12   jps     TD-SCDMA Antenna Tuner Feature 
05/03/12   zg      Added new function rf_tdscdma_mdsp_ptr_init() for iRAT 
02/08/12   jyu     Added HDET support 
12/20/11   ycl     Fix function prototype. 
08/15/11   ycl     Rework RF-MDSP interface. 
06/28/11   ycl     Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rf_tdscdma_mdsp_types.h"
#include "rf_tdscdma_mdsp_rf_event.h"
#include "rf_tdscdma_mc.h"
#include "ftm_tdscdma_log.h"
#include "rf_tdscdma_core_temp_comp.h"
#include "rf_tdscdma_core_tuner_temp_comp.h"
#include "rf_tdscdma_core_txplim.h"
#include "rf_tdscdma_msm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void rf_tdscdma_mdsp_init
(
  rfm_device_enum_type tx_device,
  void* init_vars,
  boolean is_irat
);

void rf_tdscdma_mdsp_exit
(
   boolean is_irat,
   boolean wait_for_fw_release
);


void rf_tdscdma_mdsp_update_override_val
(
  int16 rf_chain,
  int16 override_type,
  boolean override_flag,
  int16 override_val
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send HDET Config Message

  @details
  This function Send MDSP HDET config message
 
*/

void rf_tdscdma_mdsp_config_hdet
(
  int16 hdet_threshold_db,
  boolean enable_hdet,
  uint16 hdet_period
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send Therm Config Message

  @details
  This function Send MDSP Therm config message
 
*/

void rf_tdscdma_mdsp_config_therm
(
  boolean enable_therm,
  uint16 therm_period
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send iqmc update Message

  @details
  This function Send MDSP iqmc update message
 
*/
void rf_tdscdma_mdsp_update_iqmc
(
  uint32 iqmc_cfg1_0,
  uint32 iqmc_cfg1_1,
  boolean div_enable
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Triggers IQ Capture on the FW

  @details
  Triggers IQ Capture for EPT in FTM Calibration mode.

  @param first_trig
  Flag to do initialization.This must be set in 1st trigger

  @param last_trig
  Flag to do de-initialization.This must be set in last trigger 
 
  @param capt_buf_id
  Buffer index where to place captured data
 
  @param res_buf_id
  Buffer index where to place processed data
 
  @return
  Flag indicating the function execution success
*/
void rf_tdscdma_mdsp_xpt_capture
(
  boolean  first_trig,
  boolean  last_trig,
  uint8 capt_buf_id,
  uint8 res_buf_id
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns RF TDSCDMA FTM RxAGC val along with index

  @details
  This function returns RF TDSCDMA FTM RxAGC value in dB10
 
*/
rf_tdscdma_mdsp_slow_cali_results_type rf_tdscdma_mdsp_get_rxagc
(
  void
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send MDSP command

  @details
  This function Send MDSP MSG for FTM.
  Should not call this function for Mission mode
 
*/

void rf_tdscdma_mdsp_send_mdsp_msg
(
  rf_tdscdma_mdsp_msg_t msg_type,
  uint32 param
);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Convert mdsp txagc 16q9 format to rf db10 value

  @details
 
*/
uint16 rf_tdscdma_mdsp_txagc_mdsp_to_db10(int16 val);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Initialize TDSCDMA MDSP for iRAT 

  @details
  This function obtains MDSP Shared Memory addresses
*/
void rf_tdscdma_mdsp_ptr_init( void );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return raw RX AGC in FTM RF Mode 

  @details
  This function returns the raw RX AGC in FTM RF Mode
*/
boolean rf_tdscdma_mdsp_get_raw_rx_agc(rfcom_device_enum_type device, int16 *rxlvldBm, int16* rxlvlagc);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns RF RX AGC log by parsing the raw log from FW

  @details

*/
void rf_tdscdma_mdsp_get_rf_rx_agc_log( ftm_log_tdscdma_rx_agc_data_type *ftm_log );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Get the tx agc log data

  @details
  This function gets the tx agc log data from FW sharemem.
*/
void rf_tdscdma_mdsp_get_rf_tx_agc_log( ftm_log_tdscdma_tx_agc_data_type *ftm_log );

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send FTM Burst TX Message

  @details
  This function sends the Burst TX ON/OFF message
 
*/
void rf_tdscdma_mdsp_burst_tx(boolean on_off, rf_tdscdma_ftm_burst_tx_params_type tx_params, uint32 txlm_bufidx, uint32 tx_freq, rf_tdscdma_rfmode_cont_tx_msg_subid_type subID);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Send FTM Burst RX Message

  @details
  This function sends the Burst RX ON/OFF message
 
*/
void rf_tdscdma_mdsp_burst_rx
(
  boolean on_off, 
  rf_tdscdma_ftm_burst_rx_params_type rx_params, 
  uint32 rxlm_bufidx,
  uint32 rxlm_bufidx_div,
  uint32 prim_freq,
  uint32 work_freq
);

tfw_agc_result_buf_t* rf_tdscdma_mdsp_get_rf_agc_log_ptr( void );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RF_CDMA_MDSP_H */

