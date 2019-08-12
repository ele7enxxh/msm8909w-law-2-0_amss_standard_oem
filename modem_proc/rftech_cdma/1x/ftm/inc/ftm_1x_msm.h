#ifndef FTM_1X_MSM_H
#define FTM_1X_MSM_H

/*!
  @file
  ftm_1x_msm.h
 
  @brief
  RF Driver's Common CDMA Modem HAL.

  @details
  This file includes definitions and prototypes used by the CDMA Modem HAL.
 
*/

/*==============================================================================

  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/inc/ftm_1x_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
04/10/14   JJ      Pass fw_smem_ptr to enable modulator api and Moved get current
                   rtc time api to rf_cdma_msm.c
04/09/14   JJ      Added an api to get current rtc time  
04/24/13   sty     Added ftm_1x_enable_baseband_tx
09/04/12   adk     Added functionality to override NB and WB GDA parameters.
01/19/12   Saul    MCDO/FTM. Added ICIFIR configuration override.
08/18/11   aro     Moved MCPM config function to HAL
06/08/11   aro     Changed disable 1x modulator func signature to have device
06/07/11   aro     Changed 1x modulator function signature to have device
05/13/11   aro     Added interface to configure combiner interrupts
03/22/11   sty     Deleted APIs that enabled and disabled clks - this is taken 
                   care of by MCPM
03/21/11   sty     Added prototypes for ftm_1x_disable_modulator and 
                   ftm_1x_disable_tx_clocks
03/04/11   aro     Moved Install/Install Timer ISR to CDMA FTM Hal
02/22/11   sty     Added prototypes for ftm_1x_install_rfcal_timer() and 
                   ftm_1x_uninstall_rfcal_timer()
02/18/11   sty     Moved ftm_1x_set_cdma_cw and ftm_1x_set_freq_offset
02/14/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
void 
ftm_1x_enable_modulator
(
  const rfm_device_enum_type device,
  void* fw_smem_addr
);

/*----------------------------------------------------------------------------*/
void 
ftm_1x_disable_modulator
(
  const rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_set_cdma_cw
(
  boolean set_waveform_to_cw
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_set_freq_offset
(
  boolean non_zero_offset
);

/*----------------------------------------------------------------------------*/
void 
ftm_1x_enable_baseband_tx
(
  boolean enable_tx
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_configure_combiner_interrupts
(
  boolean enable_flag
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_enable_mcpm
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_1x_disable_mcpm
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_msm_update_icifir_items
(
  rfm_device_enum_type device,
  uint8 narrow_band_index,
  int32 freq_offset_enum,
  uint8 bypass_val,
  uint8 update_iq_coeffs,
  int16 *i_coeff,
  int16 *q_coeff
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_msm_update_gda_items
(
  rfm_device_enum_type device,
  uint8 narrow_band_index,
  uint8 update_wbw_gda_coeffs,
  uint8 update_nbn_gda_coeffs,
  int16 *wbw_gda_coeff,
  int16 *nbn_gda_coeff
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_1X_MSM_H */

