#ifndef FTM_COMMON_SELFTEST_H
#define FTM_COMMON_SELFTEST_H

/*! 
  @file
  ftm_common_selftest.h
 
  @brief
  This file defines function prototypes, structures and global variables for 
  self-test for any technology
  
  @brief
  This file defines function prototypes, structures and global variables for 
  self-test for any technology
  @{
*/

/*==============================================================================

  Copyright (c) 2013 - 2014 QUALCOMM Technologies Inc. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_selftest.h#1 $


when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/04/14   dbz     [selftest] Add support for VSWR V2 processing
10/16/14   jmf     [selftest] Implement multi-gain stage switch for ACLR improvement
10/14/14   jmf     [selftest] Implement FBRx Noise Floor Cal
09/19/14   jmf     Define VSWR v2 Measurement
08/18/14   jmf     [SelfTest] ACLR meas changes
07/31/14   jmf     Correct Enum type for EVM capture
                   Add API to obtain current self test flag
07/24/14   jmf     [SelfTest] Move self test mode global flag to .c
07/24/14   dbz     Fix VSWR measurement mask
07/17/14   jmf     Add api to set self test flag / Define flag for self test mode
06/30/14   daa     Created.

==============================================================================*/

#include "ftm.h"
#include "DALStdDef.h"

#define ENABLE_TX_PWR_MEAS    0x01
#define ENABLE_ACLR_MEAS      0x04
#define ENABLE_EVM_MEAS       0x02
#define ENABLE_VSWR_MEAS      0x20
#define ENABLE_VSWR_MEAS_V2   0x40
// #define ENABLE_RX_RMS_MEAS 0x01
// #define ENABLE_EVM_MEAS    0x02

typedef struct
{
  rfcom_band_type_u com_band;
  uint8 selftest_fbrx_gs;
  uint8 wtr_lte_bw;
  boolean override_enable;
} ftm_common_selftest_fbrxgs_info_type;

boolean ftm_common_fbrx_iq_capture_processing
(
  ftm_rf_technology_type ftm_rf_tech,
  uint8 fbrx_chain,
  uint32 ftm_selftest_capture_type,
  uint16 num_samples,
  int32 **iq_buf_rx,
  int32 **iq_buf_tx,
  uint16 *evm_value_ptr,
  uint32 *rx_rms_mag_ptr,
  uint32 *samp_rate,
  ftm_common_selftest_fbrxgs_info_type * fbrx_gs_info
);

typedef struct
{
  int16 vswr_v2_return_loss;
  /*!< value of VSWR Return Loss in  dB100 */

  int16 vswr_v2_mag;
  /*!< value of VSWR Magnitude in dB100*/

  int16 vswr_v2_phase;
  /*!< value of VSWR phase in degree100*/

  int16 vswr_v2_fwd_pwr;
  /*!< Value of forward power in dB100 */

  int16 vswr_v2_rev_pwr;
  /*!< Value of reverse power in dB100 */

}ftm_common_selftest_vswr_v2_results_type;

void ftm_common_selftest_set_flag( uint8 * flag_value);
uint8 ftm_common_get_selftest_capture_flag(void);

boolean ftm_common_selftest_fbrx_noise_floor_nvwrite
(
  ftm_rf_technology_type tech, 
  uint8 nv_band, 
  rfcommon_nv_fbrx_noise_data_type * fbrx_noise_nv_struct
);

boolean ftm_common_selftest_get_fbrx_noise_nv_item(  ftm_rf_technology_type tech, 
                                                     uint8 nv_band, 
                                                     rfnv_item_id_enum_type * nv_item_to_write);

#endif