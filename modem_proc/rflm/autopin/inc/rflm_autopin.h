#ifndef RFLM_AUTOPIN_H
#define RFLM_AUTOPIN_H

/*
  @file
  rflm_autopin.h

  @brief
  This file contains prototypes and definitions used by RFLM AutoPin.
*/

/*=============================================================================
 
  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.
 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/autopin/inc/rflm_autopin.h#1 $

when       who    what, where, why
--------   ---    ------------------------------------------------------------- 
11/10/15   kma     Removed TA featurization 
10/30/15   kma     Added autopin FW msg 
06/04/15   whz    Initial version

===========================================================================*/

#include "rflm_api_autopin.h"
#include "fw_rf_common_intf.h"

/* APIs for RFLM DM handle */

typedef enum {
  RFLM_AUTOPIN_UPDATE_CTL_DATA,
  RFLM_AUTOPIN_UPDATE_PROC_RESULTS,
  RFLM_AUTOPIN_GET_CTL_DATA,
  RFLM_AUTOPIN_GET_PROC_RESULTS,
} rflm_autopin_cmd_e;


#define RFLM_AUTOPIN_MAX_BUF 4


#ifdef __cplusplus
extern "C" {
#endif


rflm_err_t rflm_autopin_set_data(void *autopin_data,
                                 rflm_autopin_cmd_e cmd,
                                 const void *payload);


rflm_err_t rflm_autopin_get_data(void *autopin_data,
                                 rflm_autopin_cmd_e cmd,
                                 void *payload);


rflm_err_t rflm_autopin_dup_data(void *new_autopin_data,
                                 void *old_autopin_data);


rflm_autopin_dm_template_t *rflm_autopin_assign_buf();


boolean rflm_autopin_release_buf(rflm_autopin_dm_template_t *autopin_template);

  
void rflm_autopin_init_buf(rflm_autopin_dm_template_t *template_ptr);

uint32 rflm_autopin_get_env_scale_backoff(rflm_handle_tx_t tx_handle);

void rflm_autopin_update_fw_par_proc_result(fw_autopin_output_params_t *pout);

#ifdef __cplusplus
}
#endif


#endif
