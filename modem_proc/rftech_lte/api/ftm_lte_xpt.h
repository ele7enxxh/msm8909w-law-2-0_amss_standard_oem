#ifndef FTM_LTE_XPT_H
#define FTM_LTE_XPT_H

/*!
  @file
  ftm_lte_xpt.h

  @brief
  This module contains defintions and prototypes. This file is used to
  declare all Factory Test Mode functionality related to XPT.
*/

/*==============================================================================

  Copyright (c) 2012 QUALCOMM Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/ftm_lte_xpt.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/06/14   jmf     Update for ftm_lte_xpt_cal_deconfigure to take in xpt mode
05/20/13   jmf     xPT IQ Capture ET Path Delay (+API change to return samp_rate)
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
03/26/13   cri     XPT cal deconfig support for LTE
03/25/13   cri     XPT cal config support for LTE
03/07/13   cri     Initial support for XPT cal config
12/13/12   Saul    Common. Changed XPT capture, dpd, tx agc override intf.
12/11/12   cri     Move to common DPD buffer allocation
08/30/12   cri     Initial check in

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "rflte_msm.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
boolean ftm_lte_xpt_cal_configure(uint8 device,
                                  uint8 device_fb,
                                  uint8 xpt_mode);

/*----------------------------------------------------------------------------*/
boolean ftm_lte_xpt_cal_deconfigure(uint8 device, 
                                    uint8 device_fb,
                                    uint8 xpt_mode);

/*----------------------------------------------------------------------------*/
boolean ftm_lte_xpt_iq_capture(uint8  device,
                               uint8  device_fb,
                               uint32 num_samples,
                               boolean first_trig,
                               boolean last_trig,
                               rfcommon_xpt_processing_type do_processing,
                               uint8 capture_type,
                               uint8 capture_buffer_id,
                               uint8 results_buffer_id,
                               uint32 * samp_rate);

void ftm_lte_xpt_process_iq_capture_rsp(void);

#ifdef __cplusplus
}
#endif

#endif /*FEATURE_FACTORY_TESTMODE */

#endif /* FTM_LTE_XPT_H */
