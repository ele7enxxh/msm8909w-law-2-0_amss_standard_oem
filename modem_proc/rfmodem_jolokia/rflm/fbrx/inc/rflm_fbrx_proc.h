/*!
  @file
  rflm_fbrx_proc.h

  @brief
  Header for RFLM FBRX Post-Processing Module
  
  @detail

  
  @todo


*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/rflm/fbrx/inc/rflm_fbrx_proc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/14   oei     Initial version
==============================================================================*/
#ifndef RFLM_FBRX_PROC_H
#define RFLM_FBRX_PROC_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm.h"
#include "rflm_api_fbrx.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFLM_FBRX_READBACK_PWR_SCRIPT_NUM_TRANSACTIONS 4



/*==============================================================================
  Function Prototypes
==============================================================================*/

/*----------------------------------------------------------------------------*/
extern int32 rflm_fbrx_ilpc_gain_estimator(
   rflm_fbrx_ilpc_est_data_t *in);

extern rflm_fbrx_ilpc_decision_e rflm_fbrx_ilpc_txfe_gain_conversion(
   rflm_fbrx_ilpc_est_data_t* in);

extern void rflm_fbrx_ilpc_read_txc_power_measurements(
   rflm_fbrx_ilpc_est_data_t* params);

extern void rflm_fbrx_ilpc_read_wtr_power_measurements(
   rflm_fbrx_ilpc_est_data_t* params);

extern void rflm_fbrx_ilpc_log_decision(
   rflm_fbrx_dm_template_t* fbrx_data_ptr);
#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_FBRX_PROC_H */

