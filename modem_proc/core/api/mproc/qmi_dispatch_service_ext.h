#ifndef _QMI_DISPATCH_SERVICE_EXT_H_
#define _QMI_DISPATCH_SERVICE_EXT_H_
/*---------------------------------------------------------------------------
   QMI_DISPATCH_SERVICE_EXT.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_dispatch_service_ext.h

  @brief
    This file contains the public data structures to be used in conjuntion with
    core server framework.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/mproc/qmi_dispatch_service_ext.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
#include "qmi_core_error.h"
#include "qmi_csi.h"

typedef enum qmi_dispatch_service_error {
 QMI_DISPATCH_SERVICE_NO_ERR = 0,
 QMI_DISPATCH_SERVICE_INVALID_ARGS = QMI_CORE_DISPATCH_ERROR,
 QMI_DISPATCH_SERVICE_MEM_ERR,
 QMI_DISPATCH_SERVICE_INVALID_MSG
}qmi_dispatch_service_error_type;


typedef  qmi_dispatch_service_error_type 
(*qmi_dispatch_fn_type)(void               *server_data,
                        void               *conn_obj,
                        qmi_req_handle     req_handle,
                        uint32_t           msg_id,
                        void               *req_c_struct,
                        uint32_t           req_c_struct_len);


typedef struct msg_handler {
    uint32_t             msg_id;
    qmi_dispatch_fn_type handler;
}qmi_msg_handler_type;

#endif
