#ifndef QCRIL_UIM_LPA_H
#define QCRIL_UIM_LPA_H
/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/16   vr      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "user_identity_module_v01.h"
#include "qcril_uim.h"

/*===========================================================================

                           TYPES

===========================================================================*/

/*===========================================================================

                           FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cleanup_cached_iccids

===========================================================================*/
void qcril_uim_lpa_cleanup_cached_iccids
(
);

/*===========================================================================
  FUNCTION  qcril_qmi_uim_lpa_add_profile_ind_hdlr
===========================================================================*/
int qcril_qmi_uim_lpa_add_profile_ind_hdlr
(
  uim_add_profile_ind_msg_v01               * qcci_data_ptr,
  qmi_uim_indication_data_type              * qmi_data_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile_ind

===========================================================================*/
void qcril_uim_lpa_add_profile_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_lpa_process_qmi_callback

===========================================================================*/
void qcril_uim_lpa_process_qmi_callback
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_request

===========================================================================*/
void qcril_uim_lpa_user_request
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

#endif /* QCRIL_UIM_LPA_H */
