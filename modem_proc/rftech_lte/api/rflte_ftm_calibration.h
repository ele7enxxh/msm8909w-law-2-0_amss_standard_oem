#ifndef RFLTE_FTM_CALIBRATION_H
#define RFLTE_FTM_CALIBRATION_H

/*!
  @file
  rflte_ftm_calibration.h

  @brief
  This module defines the function prototypes and defintions used by RF
  calibration.
*/

/*==============================================================================

  Copyright (c) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rflte_ftm_calibration.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------ 
01/12/15   bsh     Reduce settling time for NBEE 
08/21/13   dj      Moved feedback_setup to be run from the lte command dispatch
06/26/13   gvn     Write RSB Cal params to NV and other RSB cal updates
05/01/13   gvn     LTE RSB Cal 
04/24/13    dj     Added support for Feedback Setup 
04/07/13   php     Add NBEE settling time of 1000us
03/26/13   cri     XPT opcode support for LTE 
12/06/12   aki     Added rflte_ftm_calibration_v3_polling_event_init
12/04/12   cri     Tx meas opcode for LTE
11/26/12   cri     Added Tx Override and Sync opcodes
11/14/12   cri     LTE radio setup opcode support  
05/16/12   vb      Do DAC cal for a device path only if a tech is enabled for it 
02/07/12   aca     LTE Dac calibration support
10/26/11   pl      Adding a function to get estimated RSSI from LNA state and BW
08/09/11   aro     Updated calv3 config Tx waveform function to have return
07/25/11   tnt     Update to new common cal v3 intf
07/20/11   aro     Added Rx Measurement OpCode
07/20/11   aro     Renamed Cal v3 Opcode/payload type names for consistency
07/08/11   aro     Updated Rx measurement interface to pass pointer to result
                   container
06/28/11   tnt     Initial Revision

==============================================================================*/

#include "comdef.h"
#include "ftm_common_calibration_v3.h"
#include "ftm_common_control.h"
#include "ftm_calibration_v3_radio.h"
#include "ftm_calibration_v3_tx.h"
#include "ftm_calibration_v3_rx.h"
#include "ftm_calibration_v3_sync.h"
#include "rflte_mc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FTM_LTE_CALV3_NBEE_SETTLING_TIME 1000

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_configure_radio
(
   ftm_cal_instr_payload_config_radio_type const *rf_cfg
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_cfg_tx_wf
(
   ftm_cal_instr_payload_config_tx_wf_type const *tx_wf
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_retune_radio
(
   ftm_cal_instr_payload_retune_radio_type const *rf_retune
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_configure_rx
(
   ftm_cal_instr_payload_config_rx_type const *cfg_rx
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_configure_tx
(
   ftm_cal_instr_payload_config_tx_type const *cfg_tx
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_measure_tx
(
   ftm_cal_instr_payload_meas_hdet_type const *hdet_meas,
   ftm_calibration_tx_result_type *tx_result
);
/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_measure_rx
(
  ftm_cal_instr_payload_meas_rx_type const *rx_meas,
  ftm_calibration_rx_result_type *rxacg_result
);

/*----------------------------------------------------------------------------*/
boolean	
rflte_ftm_calibration_tear_down_radio(void);

/*----------------------------------------------------------------------------*/
int16
rflte_ftm_calibration_get_lna_expected_rssi(uint8 lna_state, rfcom_lte_bw_type bw);

/*----------------------------------------------------------------------------*/
ftm_common_dac_cal_rsp_type 
rflte_ftm_calibration_do_dac_cal 
( 
  rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_v3_radio_setup
(
  ftm_cal_radio_setup_parameter_type const *radio_setup
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_v3_feedback_setup
(
  ftm_cal_feedback_setup_parameter_type const *feedback_setup
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_v3_tx_override
(
  ftm_cal_tx_override_parameter_type const *tx_override
);

/*----------------------------------------------------------------------------*/
int32
rflte_ftm_calibration_v3_sync
(
  ftm_cal_sync_parameter_type const *sync
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_v3_tx_measure
(
  ftm_cal_tx_meas_parameter_type const *tx_measure,
  ftm_cal_tx_meas_result_data_type *cal_result
);

/*----------------------------------------------------------------------------*/

boolean
rflte_ftm_calibration_v3_rx_measure
(
  ftm_cal_rx_meas_parameter_type const *rx_measure,
  ftm_cal_rx_meas_result_data_type *cal_result
);

/*----------------------------------------------------------------------------*/
ftm_mode_id_type ftm_lte_get_ftm_band_enum_from_band(rfcom_lte_band_type lte_band);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_v3_polling_event_init
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_xpt_configure
(
  ftm_cal_xpt_instr_payload_ept_config_type const *payload
);

/*----------------------------------------------------------------------------*/
boolean
rflte_ftm_calibration_xpt_iq_capture
(
  ftm_cal_xpt_instr_payload_iq_capture_type const *payload
);

/*----------------------------------------------------------------------------*/
boolean rflte_ftm_calibration_rsb_calibration(ftm_mode_id_type mode, boolean enable_stg, uint8 device, uint8 div_device, boolean exit_required);

/*----------------------------------------------------------------------------*/
boolean rflte_ftm_calibration_txlo_calibration
(
   ftm_mode_id_type mode,
   rfm_device_enum_type rfm_device, 
   rfm_device_enum_type rfm_device_fb
);

#ifdef __cplusplus
}
#endif

#endif /* RFLTE_FTM_CALIBRATION_H */
