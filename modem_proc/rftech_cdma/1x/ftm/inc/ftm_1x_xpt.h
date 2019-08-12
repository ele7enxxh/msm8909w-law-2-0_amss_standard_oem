#ifndef FTM_1X_XPT_H
#define FTM_1X_XPT_H

/*!
  @file
  ftm_1x_xpt.h

  @brief
  This module contains defintions and prototypes for FTM 1x XPT Control
  APIs which are used to control the 1x Radio for XPT functionality.
*/

/*==============================================================================

  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/1x/ftm/inc/ftm_1x_xpt.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/05/13   hdz     Added ftm_1x_xpt_cal_config()
01/22/13   aro     Code to use unified IQ capture param
12/13/12   Saul    Common. Changed XPT capture, dpd, tx agc override intf.
11/16/12   Saul    Sample Capture. Use common response.
10/25/12   cd/Saul Initial Release. Generalized load DPD table API
                   - Changed request packet type
                   - Added response packet type
                   - Made XPT IQ capture common.

==============================================================================*/

#include "comdef.h"
#include "rfm_device_types.h"
#include "rfm_cdma.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_core_xpt.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_xpt_iq_capture
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb,
  uint8 xpt_capture_type,
  uint16 num_samples,
  boolean first_trig,
  boolean last_trig,
  rfcommon_xpt_processing_type do_processing,
  rf_common_xpt_buffer_id_type buffer_id,
  uint32 * samp_rate
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_xpt_cal_config
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
);

/*----------------------------------------------------------------------------*/
boolean
ftm_1x_xpt_cal_deconfig
(
  rfm_device_enum_type device,
  rfm_device_enum_type device_fb
);

#ifdef __cplusplus
}
#endif

#endif /* FTM_1X_XPT_H */
