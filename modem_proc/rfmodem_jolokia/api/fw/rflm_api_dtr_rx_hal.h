#ifndef RFLM_API_DTR_RX_HAL_H
#define RFLM_API_DTR_RX_HAL_H

/*!
   @file
   rflm_api_dtr_rx_hal.h

   @brief
   File to expose MSM specific Rx APIs to RFLM layer
 
   @details

*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/api/fw/rflm_api_dtr_rx_hal.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/11/14    vb     Initial version

=============================================================================*/

#include "comdef.h" 
#include "lm_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
extern lm_status_type rfcommon_msm_write_iqmc_to_hw_with_action_t( lm_handle_type rxlm_handle, 
                                                                   uint32 iqmc_coeff,
                                                                   boolean update_iqmc_value_imm,
                                                                   uint32 vsrc_counter_val );

#ifdef __cplusplus
}
#endif


#endif /* RFLM_API_DTR_RX_HAL_H */


