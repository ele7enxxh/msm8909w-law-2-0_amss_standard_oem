#ifndef  _QMI_CORE_SERVER_H_
#define  _QMI_CORE_SERVER_H_
/*---------------------------------------------------------------------------
   QMI_CORE_SERVER.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_core_server.h

  @brief
    This file contains the APIs for the core server framework.It enables
    the clients to extended the core object  into there respective server
    objects.

  ---------------------------------------------------------------------------
  Copyright (c) 2011, 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

#include  "core_server_platform.h"
#include  "qmi_dispatch_service_ext.h"
#include  "qmi_indication_service.h"
#include  "qmi_csi.h"
#include  "qmi_core_error.h"

/* Core server error codes */
typedef enum qmi_core_server_error
{
    QMI_CORE_SERVER_NO_ERR = 0,
    QMI_CORE_SERVER_INVALID_OBJECT = QMI_CORE_SERVER_ERROR,
    QMI_CORE_SERVER_MEMORY_ERR,
    QMI_CORE_SERVER_INTERNAL_ERR
}qmi_core_server_error_type;

struct qmi_core_conn_obj;
typedef struct qmi_core_conn_obj qmi_core_conn_obj_type;

typedef void (*qmi_core_server_resume_cb_type)(qmi_core_conn_obj_type *conn_obj);

typedef struct qmi_core_server_data
{
    char                             class_name[CLASS_NAME_MAX];
    uint32_t                         instance_id;
    qmi_csi_service_handle           service_handle;
    qmi_core_server_resume_cb_type   resume_cb;
    qmi_csi_os_params                os_params;
}qmi_core_server_data_type;

/* Core server object */
typedef struct qmi_core_server_object
{
    uint32_t                       core_object_id;/*Used to verify a valid object */
    qmi_core_server_data_type      *core_server_data;
    void                           *list_service;
    void                           *task_service;
    void                           *dispatch_service;
}qmi_core_server_object_type;


/* Connection object  representing each client that connects to the server */
struct qmi_core_conn_obj {
    qmi_core_server_object_type *core_object;
    qmi_client_handle           client_handle;
    uint32_t                    client_id;
    uint32_t                    num_ind;
    void                        *client_obj;
};

/*===========================================================================
  FUNCTION  qmi_core_server_new
===========================================================================*/
/*!
@brief
  This function constructs a new core server object.

@param[out] core_object   Pointer to a newly constructed core server object. 
@param[in]  name          Name of the core object.
@param[in]  instance_id   Instance identifier associated with a server.
@param[in]  task_flag     Task is created for the server if this flag is set,
                          otherwise the server is assumed to be taskless.
@param[in]  entry_func    Entry point for the task created by the framework.
                          This argument should be NULL if task_flag is not
                          set.
@param[in]  priority      Priority used to create the task.This argument
                          should be NULL if task_flag is not set.
@param[in]  stk_size      The stack size used to create the task.This argument
                          should be NULL if task_flag is not set.
@param[in]  sig           Signal  used by the task created to wait on server
                          events, should be NULL if task_flag is not set.
@param[in]  os_params     Pointer to the signaling information for the 
                          taskless server. This parameter should be set to
                          NULL if task_flag is set.
@param[in]  disp_table    Pointer to dispatch function table, used to handle
                          messages that server understands.
@param[in]  table_size    Size of the dispatch table.

@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_new( qmi_core_server_object_type  *core_object,
                     char                         *name,
                     uint32_t                     instance_id,
                     uint8_t                      task_flag,
                     void                         *entry_func,
                     void                         *priority,
                     void                         *stk_size,
                     void                         *sig,
                     qmi_csi_os_params            *os_params,
                     qmi_msg_handler_type         *disp_table,
                     uint32_t                     table_size);

/*===========================================================================
  FUNCTION  qmi_core_server_delete
===========================================================================*/
/*!
@brief
  This function destructs the core server object.

@param[in] core_object   Pointer to core object that has to be destroyed. 

@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_delete(qmi_core_server_object_type  *core_object);

/*===========================================================================
  FUNCTION  qmi_core_server_register
===========================================================================*/
/*!
@brief
  This function registers a server object with the QCSI infrastructure.

@param[in]  server_obj           Pointer to the server object.
@param[in]  service_obj          Object containing meta information to 
                                 encode/decode the messages.
@param[in]  service_connect      Callback to register each client with the 
                                 service.
@param[in]  service_disconnect   Callback to unregister each client from
                                 service.
@param[in]  service_process_req  Callback that handles the incoming requests. 

@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_register(void                          *server_obj,
                         qmi_idl_service_object_type   service_obj,
                         qmi_csi_connect               service_connect,
                         qmi_csi_disconnect            service_disconnect,
                         qmi_csi_process_req           service_process_req);

/*===========================================================================
  FUNCTION  qmi_core_server_unregister
===========================================================================*/
/*!
@brief
  This function deregisters a server object with the QCSI infrastructure.

@param[in]  server_obj           Pointer to the server object.

@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_unregister(void  *server_obj);

/*===========================================================================
  FUNCTION  qmi_core_server_register_client
===========================================================================*/
/*!
@brief
  This function registers a client with the framework and provides
  a connection object to represent the client.This function
  should be used from inside the service_connect callback.

@param[out]  client_conn      Handle to represent a client connection.
@param[in]   core_object      Pointer to core server object.
@param[in]   client_handle    Handle used by QCSI infrastructure
                              to identify the destination client.
@param[in]   num_ind          Number of Indications the server wants the
                              client to be aware of.
@param[in]  client_data       Client specific data. 
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_register_client(qmi_core_conn_obj_type       *client_conn,
                                qmi_core_server_object_type  *core_object,
                                qmi_client_handle            client_handle,
                                uint32_t                     num_ind,
                                void                         *client_data);
                                
/*===========================================================================
  FUNCTION  qmi_core_server_unregister_client
===========================================================================*/
/*!
@brief
  This function unregisters a client with the framework.This function
  should be used from inside the service_disconnect callback.
  This funciton should be called before client deallocates the client data.

@param[in]  client_conn      Handle to represent a client connection.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_unregister_client(qmi_core_conn_obj_type     *client_conn);

/*===========================================================================
  FUNCTION  qmi_core_server_dispatch_msg
===========================================================================*/
/*!
@brief
  This function calls the appropriate handler of the incoming message.

@param[in]  client_conn       Handle to represent a client connection.
@param[in]  server_obj        Pointer to server object that extends core
                              server object.
@param[in]  req_handle        Handle provided by the infrastructure
                              to specify this particular transaction and
                              message.
@param[in]  req_c_struct      C struct with the decoded data.
@param[in]  req_c_struct_len  Length of c struct.
 
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_dispatch_msg(qmi_core_conn_obj_type   *client_conn,
                             void                     *server_obj,
                             qmi_req_handle           req_handle,
                             int32_t                  msg_id,
                             void                     *req_c_struct,
                             int32_t                  req_c_struct_len);

/*===========================================================================
  FUNCTION  qmi_core_server_send_ind
===========================================================================*/
/*!
@brief
  This function sends indication corresponding to a message id.

@param[in]  core_object           Pointer to core server object.
@param[in]  msg_id                Message id corresponding to the indication.
@param[in]  resp_c_struct         C data structure for this indication.
@param[in]  resp_c_struct_len     Size of the response c struct.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_send_ind(qmi_core_server_object_type   *core_object,
                         int32_t                       msg_id,
                         void                          *res_c_struct,
                         int32_t                       res_c_struct_len);

/*===========================================================================
  FUNCTION  qmi_core_server_handle_event
===========================================================================*/
/*!
@brief
  Handle event after the server thread receives an event notification.
  Callbacks from qmi_core_server_register may be invoked in
  the server's context.

@param[in]  core_object           Pointer to core server object.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/

qmi_core_server_error_type
qmi_core_server_handle_event(void *server_obj);


/*===========================================================================
  FUNCTION  qmi_core_server_start_server
===========================================================================*/
/*!
@brief
  Starts the server thread. This function shouldn't be used if the server
  is taskless.

@param[in]  server_obj            Pointer to server object.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_start_server(void *server_obj);

/*===========================================================================
  FUNCTION  qmi_core_server_send_resp
===========================================================================*/
/*!
@brief
  Sends a response to the client.

@param[in]  req_handle            Handle used by the infrastructure to
                                  identify the transaction and the destination
                                  client.
@param[in]  msg_id                Message ID for this particular message.
@param[in]  resp_c_struct         C data structure for this response.
@param[in]  resp_c_struct_len     Size of the response c struct.                               


@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_send_resp(qmi_req_handle     req_handle,
                          int32_t            msg_id,
                          void               *c_struct,
                          int32_t            c_struct_len);


/*===========================================================================
  FUNCTION  qmi_core_server_check_valid_object
===========================================================================*/
/*!
@brief
  This function checks if the core server object passed in the
  core server framework APIs were created by the constructor
  qmi_core_server_new.

@param[in]  server_obj            Pointer to server object.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/

qmi_core_server_error_type
qmi_core_server_check_valid_object(void *server_obj);

/*===========================================================================
  FUNCTION  qmi_core_server_get_client_data
===========================================================================*/
/*!
@brief
  Retrieves the client specific data that was passed  while registering
  a client(qmi_client_register_client).

@param[in]  conn_obj              Pointer to connection object.
                              
@retval    NULL  on Failure, Client specific data otherwise
*/                                                                      
/*=========================================================================*/
void *
qmi_core_server_get_client_data(qmi_core_conn_obj_type *conn_obj);

/*===========================================================================
  FUNCTION  qmi_core_server_get_os_params
===========================================================================*/
/*!
@brief
  Retrieves the signaling information in case the server is created with a
  task(task_flag is set in the qmi_core_server_new).

@param[in]  core_object  Pointer to the core object.
                              
@retval    NULL  on Failure, os params otherwise
*/                                                                      
/*=========================================================================*/
qmi_csi_os_params*
qmi_core_server_get_os_params( qmi_core_server_object_type  *core_object);

/*===========================================================================
  FUNCTION  qmi_core_server_set_resume_cb
===========================================================================*/
/*!
@brief
  Sets the resume callback to be used by the framework to notify the
  service when a connection is unblocked.

@param[in]  core_object  Pointer to the core object.
@param[in]  resume_cb    Callback to the resume cb
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_set_resume_cb(qmi_core_server_object_type *core_object,
                              qmi_core_server_resume_cb_type resume_cb);

#endif
