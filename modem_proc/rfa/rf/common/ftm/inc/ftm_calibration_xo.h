#ifndef FTM_COMMON_XO_CALIBRATION_H
#define FTM_COMMON_XO_CALIBRATION_H

/*!
  @file
  ftm_calibration_xo.h

  @brief
  This module contains defintions and prototypes for FTM common XO Calibration
  code which is used to dispatch xo calibration commands to mcs tcxo manager.
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_calibration_xo.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/15/12    dj     Fix for some compiler warnings
11/15/12    dj     Initial Release

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#include "ftmdiag.h"
#include "ftm.h"

#include "tcxomgr_cal.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  XO Cal parameter structure
*/
typedef PACK(struct){
  uint8  technology;
  /*!<  Technology
        tcxomgr_cal_xo_cal_tech enum:
          TECHNOLOGY_CDMA     = 0,
          TECHNOLOGY_WCDMA    = 1,
          TECHNOLOGY_GSM      = 2,
          TECHNOLOGY_LTE      = 3,
          TECHNOLOGY_TDSCDMA  = 4,
          TECHNOLOGY_CDMA_SEC = 5,
          TECHNOLOGY_MAX      = 6 */

  uint32 rx_freq_khz;
  /*!<  Carrier frequency in kHz*/

  int32  offset;
  /*!<  Single tone freq offset relative to the carrier freq in Hz */

  int32  temp_span;
  /*!<  Temperature span for factory calibration */

  uint16 min_ft_samples;
  /*!<  Min number of FT samples */

  uint16 max_ft_samples;
  /*!<  Max number of FT Samples */

  uint16 substage_time;
  /*!< Time to wait between samp substages (ms)*/

  uint16 timeout_time;
  /*!<  Maximum time to run each step before declaring failure (ms) */

  uint8 update_nv;
  /*!<  Decides if TCXO manager writes XO cal Nv */

  uint16 wb_id;
  /*!<  WB device ID used for IQ capture; Currently unused  */

  uint16 temp_grad_timeout;
  /*!<  Timeout value to reach req temp */

  uint8  xtal;
  /*!<  XTAL package used in the device 
        tcxomgr_cal_xtal_enum_type:
          TCXOMGR_XTAL_DEFAULT,
          TCXOMGR_XTAL_2016,
          TCXOMGR_XTAL_2520,
          TCXOMGR_XTAL_MAX */
} ftm_xo_cal_parameter_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  XO Cal Generic FTM Request Packet Structure
*/
typedef PACK(struct) {
  uint8   cmd;
  /*!< DIAG command code */

  uint8   sub_sys;
  /*!< Sub-system ID */

  uint16  mode;
  /*!< Sub-system command */

  uint16  ftm_cmd;
  /*!< FTM command ID */

  uint16  req_len;
  /*!< Length of the request packet */

  uint16  rsp_len;
  /*!< Length of the response packet */

  uint16  xo_cal_command;
  /*!< XO Cal action to perform */

  uint8   version;
  /*!< XO Cal request version */

  ftm_xo_cal_parameter_type ftm_xo_cal_parameter;
  /*!< XO Cal parameter structure */
} ftm_xo_cal_generic_request_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  XO Cal FTM Response Packet Structure
*/
typedef PACK(struct) {
  uint8   cmd;
  /*!< DIAG command code */

  uint8   sub_sys;
  /*!< Sub-system ID */

  uint16  mode;
  /*!< Sub-system command */

  uint16  ftm_cmd;
  /*!< FTM command ID */

  uint16  req_len;
  /*!< Length of the request packet */

  uint16  rsp_len;
  /*!< Length of the response packet */

  tcxomgr_cal_state_enum_type state;
  /*!< State of the coefficients. */

  tcxomgr_cal_fac_data_type   nom_coef;
  /*!< nominal coefficients     */

  tcxomgr_cal_fac_data_type   cal_coef;
  /*!< Calibrated coefficients  */

  tcxomgr_cal_info_type       tcxomgr_cal_info_pkt;
  /*!< Calibration information */
} ftm_xo_cal_generic_response_type;

/*----------------------------------------------------------------------------*/

ftm_rsp_pkt_type
ftm_xo_calibration
(
  void *ftm_req_data
);

/*----------------------------------------------------------------------------*/

void ftm_xo_cal_dc_coarse_cal
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
);

/*----------------------------------------------------------------------------*/

void ftm_xo_cal_collect_sample
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
);

/*----------------------------------------------------------------------------*/

void ftm_xo_cal_ft_curve_cal
(
   ftm_xo_cal_generic_request_type* req,
   ftm_xo_cal_generic_response_type* rsp_pkt
);

/*----------------------------------------------------------------------------*/

boolean
ftm_xo_cal_enter_mode
(
  ftm_rf_technology_type ftm_rf_tech,
  ftm_receive_chain_handle_type chain
);

/*----------------------------------------------------------------------------*/

void
ftm_xo_cal_exit_mode
(
  ftm_rf_technology_type ftm_rf_tech,
  ftm_receive_chain_handle_type chain
);

/*----------------------------------------------------------------------------*/

boolean
ftm_xo_cal_allocate_rxlm
(
  ftm_rf_technology_type ftm_rf_tech,
  ftm_receive_chain_handle_type chain
);

/*----------------------------------------------------------------------------*/

void
ftm_xo_cal_deallocate_rxlm
(
  ftm_rf_technology_type ftm_rf_tech
);

/*----------------------------------------------------------------------------*/

void
ftm_xo_populate_cal_params
(
  ftm_xo_cal_parameter_type* p_req,
  tcxomgr_cal_fac_param_type* p_cal_params,
  ftm_receive_chain_handle_type chain
);

/*----------------------------------------------------------------------------*/

void
ftm_xo_populate_response
(
  tcxomgr_cal_fac_param_type* p_cal_params,
  tcxomgr_cal_info_type* p_cal_info,
  ftm_xo_cal_generic_response_type *p_rsp
);

#endif
