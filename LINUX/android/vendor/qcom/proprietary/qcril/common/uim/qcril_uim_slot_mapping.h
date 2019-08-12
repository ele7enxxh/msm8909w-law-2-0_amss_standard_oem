#ifndef QCRIL_UIM_SLOT_MAPPING_H
#define QCRIL_UIM_SLOT_MAPPING_H
/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "qcril_uim.h"

/*===========================================================================

                               FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_get_physical_slot_mapping_resp

===========================================================================*/
/*!
    @brief
    Processes the response for physical slot mapping command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_physical_slot_mapping_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*=========================================================================

  FUNCTION:  qcril_uim_switch_slot_resp

===========================================================================*/
/*!
    @brief
    Processes the response for switch physical slot command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_switch_slot_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

#endif /* QCRIL_UIM_SLOT_MAPPING_H */

