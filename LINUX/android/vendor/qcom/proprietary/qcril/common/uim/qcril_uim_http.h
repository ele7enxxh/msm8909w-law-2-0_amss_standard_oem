#ifndef QCRIL_UIM_HTTP_H
#define QCRIL_UIM_HTTP_H
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
04/07/16   ks      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "user_identity_module_http_v01.h"
#include "qcril_uim.h"

/*===========================================================================

  FUNCTION:  qcril_uim_http_init

===========================================================================*/
void qcril_uim_http_init
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_http_release

===========================================================================*/
void qcril_uim_http_release
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_http_client_transaction_request

===========================================================================*/
void qcril_uim_http_client_transaction_request
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

#endif /* QCRIL_UIM_HTTP_H */

