#ifndef FTM_1X_CALIBRATION_H
#define FTM_1X_CALIBRATION_H

/*!
  @file
  ftm_1x_calibration.h

  @brief
  This module defines the function prototypes and defintions used by RF
  calibration.
*/

/*==============================================================================

  Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/inc/ftm_1x_calibration.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
06/18/14   JJ      Added support to reduce Cal time
08/21/13   dj      Moved feedback_setup to be run from the cdma command dispatch
06/07/13   zhw     Remove deprecated constants
04/24/13    dj     Added support for Feedback Setup
04/15/13   aro     Mainline calv3 optimization code/delete obsolete code
03/14/13   aro     Deprecated Composite Calibration and TxSweep Commands
01/22/13   aro     Renamed IQ capture and config interfaces
01/18/13   aro     Deleted nikel-XPT specific code
12/03/12   aro     Added 1x handler function for Tx measure opcode
11/30/12   aro     1x handler function for rx measure opcode
11/15/12   aro     Tx Override 1x handler function
11/14/12   cri     Move radio setup request to common 
11/07/12   aro     Migrated Radio Setup operation to FTM Task
11/06/12   aro     Added 1x handler function to perform cal v3 radio setup
08/07/12   vs      Modifications to Cal V3 Tx config data packet
08/04/12   vs      Add IQ gain setpoint in Cal V3 Tx config data packet
08/01/12   aro     Added EPT override Test function
07/31/12   aro     Updated single IQ capture interface
07/20/12   aro     Added support to do polling for EPT expansion
07/18/12   aro     Renamed the XPT type definitions
07/18/12   aro     Moved the XPT result pushing to common XPT framework
07/18/12   aro     Added Error Code and SubCopde fields in XPT results
07/17/12   aro     Updated XPT Opcode payload
07/17/12   aro     Enhanced support of configure feedback path
06/13/12   aro     Fixed single instance IQ Capture
06/13/12   aro     Update EPT configure, iq capture, and override functions
                   to have devices
06/13/12   aro     Removed unrelevant fields from EPT payloads
06/08/12   aro     Deleted FTM wrapper function for EPT IQ capture
06/05/12   anr     Initial EPT Support
12/05/11   aro     Added enums for RxAGC settling time
11/29/11   aro     Deleted unused Cal v3 Rx Config and Rx Measure functions
11/16/11   aro     Renamed CDMA Rx config and measure functions
11/15/11   aro     Added new optimized Cal v3 Config Rx function with
                   reduced execution time
07/27/11   aro     [1] Updated Radio Config and radio retune function to have
                   return type
                   [2] Added Tear down RF function for cal v3
07/26/11   aro     Added HDET settling time query to RF Device
07/22/11   aro     Moved HDET settling time enum
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/19/11   aro     Updated Config Tx and Measure Tx functions to handle multiple
                   Tx Data
07/19/11   aro     Updated Config Rx and Measure Rx functions to handle multiple
                   Device for a given segment
07/08/11   aro     Updated Rx measurement interface to pass pointer to result
                   container
07/08/11   aro     Added support to return multiple Rx measurement data
07/01/11   aro     Added interface to perform Intelliceiver Calibration
06/28/11   aro     Renamed header file to ftm_common_calibration_v3
06/24/11   aro     Fixed Rx and Tx measurement to return Cal result
06/21/11   aro     Deleted tech specific ISR func to use Common ISR
05/09/11   aro     Added support for 1x Calibration Sweep
05/06/11   aro     Deprecated linearizer sweep functions
04/07/11   aro     Renamed HDET Read function
04/07/11   aro     [1] Re-sequenced the TxLinearizer argument
                   [2] Deleted unnecessary function
                   [3] Renamed Trigger functions for Sweep
04/07/11   aro     Updated argument type for TxLinearizer
04/07/11   aro     [1] Added C++ Protection
                   [2] Added Tx Linearizer Prototype
03/23/11   aro     Made TxSweep and CompCal trigger functions static
03/22/11   aro     Compiler Warning Fix
03/03/11   aro     Changed the return type of Composite Calibration func
03/04/11   aro     Added trigger interface for Composite calibration
02/23/11   sty     Added prototype for ftm_1x_get_all_hdet_readings
02/09/11   sty     Added header files
02/09/11   aro     Added TxSweep functions
02/09/11   aro     Added Configure TxSweep Interfaces
02/01/11   aro     Added support for NV Commit
02/01/11   aro     Added LNA Calibration Support
02/01/11   aro     Renamed function name 
02/01/11   aro     Added DVGA Calibrate and Store
12/16/10   aro     Initial Revision

==============================================================================*/

#include "comdef.h"

#include "ftm_cdma_control.h"
#include "ftmdiag.h"
#include "ftm_common.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_calibration_v3_radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calibrate_dvga
(
  void  * ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calibrate_lna
(
  void  * ftm_req_data
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_calibrate_intelliceiver
(
  void  * ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_configure_radio
(
   ftm_cal_instr_payload_config_radio_type const *rf_cfg
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_retune_radio
(
   ftm_cal_instr_payload_retune_radio_type const *rf_retune
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_tear_down_radio
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_configure_rx
(
  ftm_cal_instr_payload_config_rx_type const *rx_config
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_configure_tx
(
   ftm_cal_instr_payload_config_tx_type const *tx_meas
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_measure_tx
(
  ftm_cal_instr_payload_meas_hdet_type const *hdet_meas,
  ftm_calibration_tx_result_type *tx_result
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_sweep_measure_rx
(
  ftm_cal_instr_payload_meas_rx_type const *rx_meas,
  ftm_calibration_rx_result_type *rx_result
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_v3_radio_setup
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_radio_setup
(
  ftm_cal_radio_setup_req_type *rs_req
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_v3_feedback_setup
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_feedback_setup
(
  ftm_cal_feedback_setup_req_type *fb_req
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_v3_tx_override
(
  ftm_cal_tx_override_parameter_type const *tx_override
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_v3_rx_measure
(
  ftm_cal_rx_meas_parameter_type const *rx_measure,
  ftm_cal_rx_meas_result_data_type *cal_result
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calibration_v3_tx_measure
(
  ftm_cal_tx_meas_parameter_type const *tx_measure,
  ftm_cal_tx_meas_result_data_type *cal_result
);

/*----------------------------------------------------------------------------*/
ftm_rsp_pkt_type
ftm_1x_commit_cal_nv
(
  void  * ftm_req_data
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_cal_xpt_configure
(
  ftm_cal_xpt_instr_payload_ept_config_type const *payload
);

boolean
ftm_1x_cal_xpt_iq_capture
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_polling_event_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_calv3_xpt_iq_capture
(
  ftm_cal_xpt_instr_req_type *ftm_req_data
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_1X_CALIBRATION_H */
