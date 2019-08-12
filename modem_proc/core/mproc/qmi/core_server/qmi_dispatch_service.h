#ifndef _QMI_DISPATCH_SERVICE_H_
#define _QMI_DISPATCH_SERVICE_H_
/*---------------------------------------------------------------------------
   QMI_DISPATCH_SERVICE.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_dispatch_service.h

  @brief
    This file contains the APIs for the dispatch service of core server
    framework.

  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
#include "core_server_platform.h"
#include "qmi_dispatch_service_ext.h"
#include "qmi_csi.h"
#include "qmi_core_error.h"
#include "stdint.h"



typedef struct qmi_dispatch_service {
    qmi_msg_handler_type    *dispatch_handler;
    uint32_t            dispatch_table_size;
}qmi_dispatch_service_type;

/*===========================================================================
  FUNCTION  qmi_dispatch_service_new
===========================================================================*/
/*!
@brief
  This function creates a dispatch object.

@param[in]  dispatch_table        Dispatch table consisting of the handler
                                  corresponding to each message ID.
@param[in]  dispatch_table_size   Size of the dispatch table. 
@param[out] err                   Pointer to the various error values defined
                                  in qmi_dispatch_service_error_type. 

@retval    Returns a dispatch object on Success, NULL with appropriate
           error value set in err output argument otherwise.
*/
/*=========================================================================*/
qmi_dispatch_service_type*
qmi_dispatch_service_new(qmi_msg_handler_type             *dispatch_table,
                         uint32_t                         dispatch_table_size,
                         qmi_dispatch_service_error_type  *err);
/*===========================================================================
  FUNCTION  qmi_dispatch_service_delete
===========================================================================*/
/*!
@brief
  This function destroys a dispatch object.

@param[in]  dispatch_service   Dispatch object.

@retval    Returns a QMI_DISPATCH_SERVICE_NO_ERR on Success.
@retval    QMI_DISPATCH_...    Error, see error codes defined
                                  in qmi_dispatch_service_error_type
*/
/*=========================================================================*/

qmi_dispatch_service_error_type
qmi_dispatch_service_delete(qmi_dispatch_service_type *dispatch_service);
/*===========================================================================
  FUNCTION  qmi_dispatch_service_dispatch_msg
===========================================================================*/
/*!
@brief
  This function dispatches the message to its appropriate handler.

@param[in]  dispatch_service      Dispatch object.
@param[in]  server_data           Server specific data. 
@param[in]  connection_obj        Connection object pertaining to the client
                                  connection.
@param[in]  req_handle            Handle provided by the infrastructure
                                  to specify this particular transaction.
@param[in]  msg_id                Message Id pertaining to this particular 
                                  message.
@param[in]  req_c_struct          C struct with the decoded data.
@param[in]  req_c_struct_len      Length of the c struct. 

@retval    Returns a QMI_DISPATCH_SERVICE_NO_ERR on Success.
@retval    QMI_DISPATCH_...    Error, see error codes defined
                                  in qmi_dispatch_service_error_type
*/
/*=========================================================================*/
qmi_dispatch_service_error_type
qmi_dispatch_service_dispatch_msg(qmi_dispatch_service_type *dispatch_service,
                                  void                      *server_data,
                                  void                      *connection_obj,
                                  qmi_req_handle            req_handle,
                                  uint32_t                  msg_id,
                                  void                      *req_c_struct,
                                  uint32_t                  req_c_struct_len);



#endif





