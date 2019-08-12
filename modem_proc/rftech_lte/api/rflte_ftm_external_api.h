/*!
  @file rflte_ftm_external_api.h

  @brief
  This file contains all declarations and definitions for
  the LTE FTM Main Control.

  @details

*/

/*===========================================================================

  Copyright (c) 2009 - 2012 QUALCOMM Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/02/13   cri     Support for RFLM Tx override for LTE
01/17/13   cri     Update usage for Tx override opcode
08/27/12   pl/sbm  Update to new device-based MDSP interfaces
08/24/11   whc     Initial Check-in	- merge from 9K PLF 119 CL 1685074
===========================================================================*/

#ifndef RFLTE_FTM_EXTERNAL_API_H
#define RFLTE_FTM_EXTERNAL_API_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
int16 rflte_ftm_mc_get_rxagc_val(rfm_device_enum_type device);
/*----------------------------------------------------------------------------*/
boolean rflte_ftm_xpt_override_rf_settings(ftm_xpt_txagc_override_type* params);
/*----------------------------------------------------------------------------*/
boolean rflte_ftm_xpt_override_txagc( rfm_device_enum_type device,
                                      uint8  xpt_mode,
                                      uint8  amam_bank_idx,
                                      uint16 amam_start_offset,
                                      uint8  amam_tbl_size,
                                      uint8  ampm_bank_idx,
                                      uint16 ampm_start_offset,
                                      uint8  ampm_tbl_size,
                                      uint8  epdt_bank_idx,
                                      uint16 epdt_start_offset,
                                      uint8  epdt_tbl_size,
                                      uint8  iq_gain_action_type,
                                      uint32 iq_gain,
                                      uint8  env_scale_action_type,
                                      uint32 env_scale,
                                      uint16 rgi,
                                      uint16 smps_bias,
                                      uint8  pa_state,
                                      uint16 pa_curr,
                                      uint8  delay_action_type,
                                      int32  delay);
/*----------------------------------------------------------------------------*/
boolean rflte_ftm_xpt_process_tx_override_params ( rfm_device_enum_type device,
                                                   uint8  xpt_mode,
                                                   uint8  amam_bank_idx,
                                                   uint16 amam_start_offset,
                                                   uint8  amam_tbl_size,
                                                   uint8  ampm_bank_idx,
                                                   uint16 ampm_start_offset,
                                                   uint8  ampm_tbl_size,
                                                   uint8  epdt_bank_idx,
                                                   uint16 epdt_start_offset,
                                                   uint8  epdt_tbl_size,
                                                   uint8  iq_gain_action_type,
                                                   uint32 iq_gain,
                                                   uint8  env_scale_action_type,
                                                   uint32 env_scale,
                                                   uint16 rgi,
                                                   uint16 smps_bias,
                                                   uint8  pa_state,
                                                   uint16 pa_curr,
                                                   uint8  delay_action_type,
                                                   int32  env_delay_val);

#endif /* RFLTE_FTM_EXTERNAL_API_H */


