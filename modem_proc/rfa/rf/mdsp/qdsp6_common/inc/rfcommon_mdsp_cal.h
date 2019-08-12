#ifndef RFCOMMON_MDSP_CAL_H 
#define RFCOMMON_MDSP_CAL_H 

/*! 
  @file
  rfcommon_mdsp_cal.h
 
  @brief
  Thie module defines the MDSP interfaces related to rf calibration
*/

/*==============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/mdsp/qdsp6_common/inc/rfcommon_mdsp_cal.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/16/15   vv      Initial Revision.

==============================================================================*/


/*=============================================================================
                       ADC DC Calibration  Definitions
=============================================================================*/

#define RFCOMMON_REG_INVALID 0x00

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rf_dispatch.h"
#include "rfcommon_semaphore.h"
#include "rf_dispatch_snum.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                       ADC DC Calibration  Structure Definitions
=============================================================================*/
typedef struct
{
  /*To store the semaphore which will be used when communicating with firmware during VCM ADC DC calibration*/
  rfcommon_semaphore_token_type* sem_token; 

  /*WBDC_EST2_CURR_I value */
  int32 wbdc_est2_curr_i;

  /*WBDC_EST2_CURR_Q value */
  int32 wbdc_est2_curr_q;

}rfcommon_vcm_adc_cal_data_type;

/*=============================================================================
                       ADC DC Calibration Function Declarations
=============================================================================*/

boolean rfcommon_mdsp_fw_rfcmd_adc_vcm_cal(uint32 rflm_rx_handle);

void rfcommon_mdsp_adc_vcm_cal_fwrsp_handler
( 
  void* cmd_ptr,
  rf_dispatch_cid_info_type *cid_info,
  void *cb_data 
);

boolean rfcommon_mdsp_snum_adc_vcm_cal_fwrsp_handler
( 
  void *cmd_ptr,
  const struct rf_dispatch_snum_info_type_t *snum_data
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_MDSP_H */


