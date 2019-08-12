#ifndef FTM_COMMON_MSM_H
#define FTM_COMMON_MSM_H

/*!
  @file
  ftm_common_msm.h
 
  @brief
  Common FTM Modem HAL.

  @details
  This file includes definitions and prototypes used by the FTM Modem HAL.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_common_msm.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/09/14   jmf     Add API for performing RSB coeff calc with DC removed
05/01/13   zg      More update on rfcommon_msm_compute_rsb_coeff_from_iq().
04/03/13   zg      Updated rfcommon_msm_compute_rsb_coeff_from_iq().
04/02/13   shv/zg  Added rfcommon_msm_compute_rsb_coeff_from_iq().
04/24/12   aki     Added function ftm_common_msm_get_hw_timer_periodicty
03/02/12   aro     Updated Timer interfaces to return enumeration as timer
                   action status
08/25/11   aro     Renamed functions to better match the functionality
08/18/11   aro     Deleted unused function
08/17/11   aro     Added prototype for ftm_common_update_irq_periodicity()
08/17/11   aro     Added interface to enable irq for FTM calibration timer
08/17/11   aro     Added interface to disable irq for FTM calibration timer
08/12/11   aro     Added interface to calculate the irq strobe count for given
                   timer interval
04/14/11   aro     Added Timer Insall, Uninstall, and update functions
04/14/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "ftm_common_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*! IQ capture information struct for RSB calc with DC compensation */
typedef struct
{
  /* Pointer to IQ capture sample location */
  uint32 *access_ptr;
  /* number of samples in IQ capture */
  uint32 sample_size;
  /* DC values to compensate, uint32 with MSW = DC_I, LSW = DC_Q */
  uint32 dcCompVals;
} ftm_common_msm_iq_cap_info;

/*----------------------------------------------------------------------------*/
void 
ftm_common_msm_install_timer
( 
  uint32 interval,
  void (*cb_func) (void) 
);

/*----------------------------------------------------------------------------*/
void 
ftm_common_msm_uninstall_timer
(
  void
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type
ftm_common_msm_configure_hw_timer
(
  uint32 interval_us,
  ftm_common_isr_cb cb_func 
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type
ftm_common_msm_update_hw_timer
(
  uint32 interval_us
);

/*----------------------------------------------------------------------------*/
ftm_common_timer_status_type
ftm_common_msm_deconfigure_hw_timer
(
  void
);

/*----------------------------------------------------------------------------*/
uint32
ftm_common_msm_get_hw_timer_periodicty
(
  void
);

/*------------------------------------------------------------------------------*/
void rfcommon_msm_compute_rsb_coeff_from_iq 
(
  uint32 *access_ptr, 
  uint32 sample_size,
  int16 *rsb_coeff_a,
  int16 *rsb_coeff_b,
  uint64 *i_sq,
  uint64 *q_sq,
  int64  *i_q
);

/*------------------------------------------------------------------------------*/
void rfcommon_msm_compute_rsb_coeff_from_iq_dc_comped 
(
  ftm_common_msm_iq_cap_info iq_cap_struct,
  int16  *rsb_coeff_a,
  int16  *rsb_coeff_b,
  uint64 *i_sq_final,
  uint64 *q_sq_final,
  int64  *i_q_final
);
#ifdef __cplusplus
}
#endif

#endif /* FTM_COMMON_MSM_H */

