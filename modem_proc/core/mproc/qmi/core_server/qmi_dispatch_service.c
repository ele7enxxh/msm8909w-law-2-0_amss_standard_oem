#include "qmi_dispatch_service.h"
/*---------------------------------------------------------------------------
   QMI_DISPATCH_SERVICE.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_dispatch_service.c

  @brief
    This file contains the implementation of the APIs for the dispatch
    service.

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
qmi_dispatch_service_new(qmi_msg_handler_type                 *dispatch_table,
                         uint32_t                             dispatch_table_size,
                         qmi_dispatch_service_error_type  *err)
{

    qmi_dispatch_service_type *new_msg_handler;

    if (dispatch_table == NULL || dispatch_table_size  <= 0  ) {
        *err = QMI_DISPATCH_SERVICE_INVALID_ARGS;
        return  NULL;
    }

    new_msg_handler =  ALLOCATOR(sizeof(qmi_dispatch_service_type));

    if (new_msg_handler == NULL ) {
        *err = QMI_DISPATCH_SERVICE_MEM_ERR;
        return NULL;
    }

    new_msg_handler->dispatch_handler = dispatch_table;
    new_msg_handler->dispatch_table_size = dispatch_table_size;

    return new_msg_handler;
}
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
qmi_dispatch_service_delete(qmi_dispatch_service_type *dispatch_service)
{
    if (dispatch_service == NULL ) {
        return QMI_DISPATCH_SERVICE_INVALID_ARGS;
    }

    DEALLOCATE(dispatch_service);
    return QMI_DISPATCH_SERVICE_NO_ERR;
}
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
                                  uint32_t                  req_c_struct_len)
{
    uint32_t  index = 0;
    qmi_msg_handler_type  *msg_handler;
    qmi_dispatch_service_error_type dispatch_err;

    if (dispatch_service == NULL ) {
        return QMI_DISPATCH_SERVICE_INVALID_ARGS;
    }
    /* Linear search for the correct msg id */
     msg_handler =  dispatch_service->dispatch_handler;
     for (index = 0; index < dispatch_service->dispatch_table_size; index ++) {
         if (msg_handler->msg_id == msg_id)
         break;
         msg_handler++;
     }

     if (index ==  dispatch_service->dispatch_table_size ) {
         return QMI_DISPATCH_SERVICE_INVALID_MSG;
     }

     /* Call the corresponding handler */
     dispatch_err = msg_handler->handler(server_data,
                                         connection_obj,
                                         req_handle,
                                         msg_id,
                                         req_c_struct,
                                         req_c_struct_len);

     return dispatch_err;
}



