/*!
  @file
  rflm_cmn.h

  @brief
  RFLM CMN driver

  @detail
  common RFLM API for tech RFLM module to reference
  - Enqueue dummy CCS task
 
  @addtogroup RFLM_CMN
  @{
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/common/inc/rflm_cmn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/13   zhw     Documentation Update
12/02/13   zhw     Initial Revision

===========================================================================*/

#ifndef RFLM_CMN_H
#define RFLM_CMN_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "rflm_cmn_intf.h"

/*----------------------------------------------------------------------------*/
uint32 
rflm_cmn_enqueue_dummy_task
(
  rflm_ccs_rf_tq_handle_t *tq,
  void* callback_api,
  void* callback_params
);

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_CMN_H */
