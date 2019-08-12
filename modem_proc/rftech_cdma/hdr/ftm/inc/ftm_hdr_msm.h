#ifndef FTM_HDR_MSM_H
#define FTM_HDR_MSM_H

/*!
  @file
  ftm_hdr_msm.h
 
  @brief
  RF Driver's Common CDMA Modem HAL.

  @details
  This file includes definitions and prototypes used by the CDMA Modem HAL.
 
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/hdr/ftm/inc/ftm_hdr_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/04/13   APU     Brought over FTM HDR radio Tx control from Dime.
08/20/13   zhw/sty Update FTM HDR radio Tx control interface 
01/25/12   Saul    MCDO/FTM. Added DVGA offset dynamic configuration.
01/19/12   Saul    MCDO/FTM. Added ICIFIR configuration override.
08/18/11   aro     Moved MCPM config function to HAL
06/17/11   Saul    Now configuring Tx carrier freq offsets and mask for MCDO
06/07/11   sty     Added ftm_hdr_disable_modulator() 
04/12/11   aro     Deleted Enable Firmware interface to replace with RFM
03/22/11   sty     Deleted APIs that enabled and disabled clks - this is taken 
                   care of by MCPM
03/22/11   sty     Deleted ftm_hdr_enable_tones(), changed return type of
                   ftm_hdr_enable_modulator()
                   Deleted tramp.h
03/21/11   sty     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_enable_modulator
(
  uint8 carrier_mask,
  boolean skip_dac_toggle
);

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_disable_modulator
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_enable_mcpm
(
  void
);

/*----------------------------------------------------------------------------*/
void
ftm_hdr_disable_mcpm
(
  void
);

/*----------------------------------------------------------------------------*/
boolean
ftm_hdr_msm_update_icifir_items
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
ftm_hdr_msm_update_dvga_items
(
  rfm_device_enum_type device,
  uint8 narrow_band_index,
  int32 freq_offset_enum,
  int32 dvga_val
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_HDR_MSM_H */

