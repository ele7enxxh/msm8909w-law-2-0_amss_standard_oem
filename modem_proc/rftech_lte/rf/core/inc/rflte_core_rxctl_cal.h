#ifndef RFLTE_CORE_RXCTL_CAL_H
#define RFLTE_CORE_RXCTL_CAL_H
/*!
  @file
  rflte_core_rxctl_cal.h

  @brief
  This is the header file for LTE Rx control functionality supporting RX 
  calibration. 

  @details

*/

/*============================================================================== 
   
  Copyright (c) 2009, 2010 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
 
$DateTime: 2016/12/13 07:59:25 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/rf/core/inc/rflte_core_rxctl_cal.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
09/07/12   sbm     Remove rflte_core_im2_update_dac_code()
03/17/11   tnt     Replace INO with mempool IQ on Nikel
10/27/10   tnt     Add IM2 support for band 17/4/2/7/11/3/20 
06/03/10   qma     Warning fixes
05/25/10   tnt     Adding im2 support functions
04/01/10   qma     Initial version and added INO capture and FFT support for IM2.
==============================================================================*/ 

#include "rfcom.h"
#include "rfdevice_lte_type_defs.h"
#include "rflte_nv.h"

// For IM2 Cal
#define FTM_LTE_IM2_DFT_SIZE 2048
#define FTM_LTE_IM2_DFT_COEF_SCALE 32768
#define FTM_LTE_IM2_NUM_BIN 3
#define FTM_LTE_IM2_START_BIN 2  /* 2 for BW 10MHz;  4 for BW 5MHz*/
#define LTE_IM2_STEP_SIZE   4
#define LTE_IM2_MATRIX_SIZE 128
#define LTE_IM2_SEARCH_MAX  25
#define LTE_IM2_B13_RX_LO   720000
#define LTE_IM2_B1_RX_LO    2235000
#define LTE_IM2_B17_RX_LO   770000
#define LTE_IM2_B4_RX_LO    2235000
#define LTE_IM2_B2_RX_LO    2040000
#define LTE_IM2_B7_RX_LO    2735000
#define LTE_IM2_B11_RX_LO   1541000
#define LTE_IM2_B3_RX_LO    1937000
#define LTE_IM2_B20_RX_LO   765000

/*--------------------------------------------------------------------------*/
void rflte_core_capture_iq(rf_path_enum_type path);

/*--------------------------------------------------------------------------*/
void rflte_core_multi_dft(void);

/*--------------------------------------------------------------------------*/
double rflte_core_get_im2_power(rf_path_enum_type path);

/*--------------------------------------------------------------------------*/
void rflte_core_get_dac_code_es(rfdevice_lte_im2_dac_codes_type *center_dac_codes, rf_path_enum_type path);

/*--------------------------------------------------------------------------*/
double rflte_core_get_dac_code_ss(rfdevice_lte_im2_dac_codes_type *center_dac_codes, rf_path_enum_type path);

/*--------------------------------------------------------------------------*/
double rflte_core_get_dac_code_ss_test(rfdevice_lte_im2_dac_codes_type *center_dac_codes, rf_path_enum_type path);

/*--------------------------------------------------------------------------*/
void rflte_core_im2_cal_prx(rfdevice_lte_im2_dac_codes_type *center_dac_codes, rfcom_lte_band_type band);

/*--------------------------------------------------------------------------*/
void rflte_core_im2_cal_drx(rfdevice_lte_im2_dac_codes_type *center_dac_codes, rfcom_lte_band_type band);

/*--------------------------------------------------------------------------*/
void rflte_core_im2_config(rfcom_device_enum_type device,rfcom_lte_band_type band, rfcom_lte_bw_type bw,uint32 rx_freq);

/*--------------------------------------------------------------------------*/
void rflte_core_im2_build_contour(rf_path_enum_type path);

#endif 
