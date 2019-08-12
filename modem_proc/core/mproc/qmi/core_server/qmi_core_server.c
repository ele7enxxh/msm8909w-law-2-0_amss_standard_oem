#include  "qmi_core_server.h"
#include  "qmi_container_service.h"
#include  "qmi_dispatch_service.h"
#include  "qmi_task_service.h"
#include  "qmi_indication_service.h"


/*---------------------------------------------------------------------------
   QMI_CORE_SERVER.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_core_server.c

  @brief
    This file contains the imlementation for the core server framework.
    It enables the clients to extended the core object into there respective
    server objects.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$$

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

/* Code used to validate core server object */
#define CORE_OBJECT_ID 0XC0DE

/*===========================================================================
  FUNCTION  qmi_handle_indication
===========================================================================*/
/*!
@brief
This callback function gets invoked on data stored in each node 
of the list object. The data is basically the client connection object   

@param[in]  data                  Pointer to core connection object.
@param[in]  msg_id                Message id corresponding to the indication.
@param[in]  ind_c_struct          C data structure for this indication.
@param[in]  ind_c_struct_len      Size of the indication c struct.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/
/*=========================================================================*/
qmi_core_server_error_type
qmi_handle_indication(void      *data,
                      int32_t   msg_id,
                      void      *ind_c_struct,
                      int32_t   ind_c_struct_len)
{
    uint8_t  *struct_ptr;
    uint32_t ind_index;
    qmi_core_conn_obj_type *core_conn;
    qmi_indication_type    *ind_obj;

    if (data == NULL)
        return QMI_CORE_SERVER_INVALID_OBJECT;
    
    
    core_conn = (qmi_core_conn_obj_type *)data;
    struct_ptr =  core_conn->client_obj;
    DEBUG("TRYING TO SEND INDICATION WITH MESSAGE ID.....%d\n",msg_id,0,0);
    /* Jump to the first indication object in the client structure */
    /* In client structure the first structure will be core connection object
       followed by the indication objects */
    struct_ptr = (struct_ptr +  sizeof(qmi_core_conn_obj_type));
    if (struct_ptr !=  NULL ) {
        /* Go through all the indication objects declared in the client data */
        for (ind_index = 0; ind_index < core_conn->num_ind; ind_index++ )
        {
            ind_obj = (qmi_indication_type *)(struct_ptr);
            /* TODO: Check for error handling
            might need to log the error in sending indication */
            if (msg_id == ind_obj->msg_id)
            {
                 ind_obj->send_ind(struct_ptr,
                                   ind_c_struct,
                                   ind_c_struct_len);

                 break;
            }
               
            /* Jump to other indication objects defined */
            struct_ptr = struct_ptr +  ind_obj->size_ind_obj;
        }
    }
    else
        return QMI_CORE_SERVER_INVALID_OBJECT;
    return QMI_CORE_SERVER_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_core_server_new
===========================================================================*/
/*!
@brief
  This function constructs a new core server object.

@param[out] core_object   Pointer to core object.The object is constructed 
                          and returned as out parameter. 
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
@param[in]  sig           This signal is used by the task created for the
                          server to wait on server events, should be NULL
                          if task_flag is not set.
@param[in]  os_params     Pointer to the signaling information for the 
                          taskless server. This parameter should be set to
                          NULL if task_flag is set, i.e. it should be provided by
                          user in case of taskless server.
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
                     uint32_t                     table_size)
{
    
    qmi_core_server_data_type        *core_server_data = NULL;
    qmi_container_service_type       *list_service = NULL;
    qmi_task_service_type            *task_service = NULL;
    qmi_dispatch_service_type        *dispatch_service;
    qmi_container_error_type         list_err = QMI_LIST_NO_ERR;
    qmi_task_service_error_type      task_err = QMI_TASK_SERVICE_NO_ERR;
    qmi_dispatch_service_error_type  dispatch_err = QMI_DISPATCH_SERVICE_NO_ERR;
    

    if (core_object == NULL )
        return  QMI_CORE_SERVER_INVALID_OBJECT;

    /* OS params makes sense only when we have a taskless server */
    if (task_flag == 0 && os_params == NULL )
        return QMI_CORE_SERVER_INVALID_OBJECT;
   

    /* Initializing and construct core server data */
    if ((core_server_data = ALLOCATOR(sizeof(qmi_core_server_data_type))) == NULL)
        return QMI_CORE_SERVER_MEMORY_ERR;


    if ((strlen(name) + 1) > CLASS_NAME_MAX )
    {
        /* Preventing buffer overflow */
        DEALLOCATE(core_server_data);
        return QMI_CORE_SERVER_INVALID_OBJECT;
    }
        

    core_object->core_object_id = CORE_OBJECT_ID;

    /*Initialize the core server data*/
    strlcpy(core_server_data->class_name, name, CLASS_NAME_MAX);
    memset(&(core_server_data->service_handle),0,sizeof(qmi_csi_service_handle));
    memset(&(core_server_data->os_params),0,sizeof(qmi_csi_os_params));
    if (instance_id)
        core_server_data->instance_id = instance_id;
    else
        core_server_data->instance_id = 0;

    core_object->core_server_data = core_server_data;
    

    /* Initialize and construct the list object */
    if ((list_service = qmi_container_new(&list_err,
                                          (void *)qmi_handle_indication)) == NULL)
    {
        DEALLOCATE(core_server_data);
        return (qmi_core_server_error_type)list_err;
    }
        

    core_object->list_service = list_service;
   

   /* Initialize and construct the task object */
    if (task_flag != 0 )
	{
        if ((task_service = qmi_task_service_new(&task_err,
                                                 name,
                                                 entry_func,
                                                 priority,
                                                 stk_size,
                                                 sig)) == NULL)
        {
            DEALLOCATE(core_server_data);
            return (qmi_core_server_error_type)task_err;
        }
            
		
		/*Initialize the os_params of core server object with the tcb
		  and signal in case of rex implementation*/
		task_err = qmi_task_set_os_params(task_service,
			                              &(core_server_data->os_params));

		if (task_err != QMI_TASK_SERVICE_NO_ERR)
        {
             DEALLOCATE(core_server_data);
             return (qmi_core_server_error_type)task_err;
            
        }
	}
	else if (os_params != NULL )
    {
        QMI_MEM_COPY(&(core_server_data->os_params),
                     sizeof(qmi_csi_os_params),
                     os_params,
                     sizeof(qmi_csi_os_params));
	}

    core_object->task_service = task_service;
    /* Initialize and construct the dispatch object */
    if ((dispatch_service = qmi_dispatch_service_new(disp_table,
                                                     table_size,
                                                     &dispatch_err)) == NULL)
    {
        DEALLOCATE(core_server_data);
        return (qmi_core_server_error_type)dispatch_err;
    }

    core_object->dispatch_service = dispatch_service;
    return QMI_CORE_SERVER_NO_ERR;
}

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
qmi_core_server_delete(qmi_core_server_object_type *core_object)
{

    qmi_container_error_type          list_err;
    qmi_task_service_error_type       task_err;
    qmi_dispatch_service_error_type   dispatch_err;

    if (core_object == NULL ) {
        return QMI_CORE_SERVER_INVALID_OBJECT;
    }

    if (core_object->core_server_data)
        DEALLOCATE(core_object->core_server_data);
    else 
        return QMI_CORE_SERVER_INVALID_OBJECT;
    

    /* Deallocate the container object */
    if((list_err = qmi_container_delete(core_object->list_service)) != QMI_LIST_NO_ERR)
            return (qmi_core_server_error_type)list_err;
    
    /*Deallocate the task object */
    if ((task_err = qmi_task_service_delete(core_object->task_service)) !=  QMI_TASK_SERVICE_NO_ERR)
            return (qmi_core_server_error_type)task_err;
     
    /*Deallocate the dispatch table */
    if ((dispatch_err = qmi_dispatch_service_delete(core_object->dispatch_service))!=  QMI_DISPATCH_SERVICE_NO_ERR)
            return (qmi_core_server_error_type)dispatch_err;
        
   
    return QMI_CORE_SERVER_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_core_server_internal_resume_cb
===========================================================================*/
/*!
@brief
  This function is called by the QCSI framework when the client
  is no longer in a blocked state.

@param[in] clnt_handle   QCSI client descriptor
@param[in] connection_handle   Cookie associated with this connection
@param[in] service_cookie   Cookie associated with this service

@retval    None
*/
/*=========================================================================*/
static void qmi_core_server_internal_resume_cb
(
  qmi_client_handle clnt_handle,
  void *connection_handle,
  void *service_cookie
)
{
  if(connection_handle)
  {
    qmi_core_conn_obj_type *conn_obj = (qmi_core_conn_obj_type *)connection_handle;
    if(conn_obj->core_object && conn_obj->core_object->core_server_data &&
        conn_obj->core_object->core_server_data->resume_cb)
    {
      conn_obj->core_object->core_server_data->resume_cb(conn_obj);
    }
  }
}

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
                         qmi_csi_process_req           service_process_req)
{

    qmi_csi_error csi_rc;
    qmi_core_server_error_type rc;
    qmi_csi_os_params *os_params;
    qmi_core_server_object_type *core_object = NULL;
	  qmi_csi_service_handle service_handle;
    qmi_csi_options core_server_options;

    if (server_obj == NULL ) {
        return QMI_CORE_SERVER_INVALID_OBJECT;
    }

    core_object = (qmi_core_server_object_type*)server_obj;

    /* If we already have a valid CSI handle, then
     * avoid registering a second time */
    if(core_object->core_server_data->service_handle)
    {
      return QMI_CORE_SERVER_INTERNAL_ERR;
    }

    /* Check if service task has started. If not start it */
    if(!qmi_task_service_has_started(core_object->task_service))
    {
      rc = qmi_core_server_start_server(server_obj);
      if(rc != QMI_CORE_SERVER_NO_ERR)
      {
        return rc;
      }

      /* If the service thread is still not up error */
      if(!qmi_task_service_has_started(core_object->task_service))
      {
        return QMI_CORE_SERVER_INTERNAL_ERR;
      }
    }

    os_params =  &(core_object->core_server_data->os_params);

    /* Set the instance id */
    QMI_CSI_OPTIONS_INIT(core_server_options);
    QMI_CSI_OPTIONS_SET_INSTANCE_ID(core_server_options,
                                    core_object->core_server_data->instance_id);
    QMI_CSI_OPTIONS_SET_RESUME_IND_CB(core_server_options, 
                                    qmi_core_server_internal_resume_cb);

    csi_rc = qmi_csi_register_with_options(service_obj,
                                       service_connect,
                                       service_disconnect,
                                       service_process_req,
                                       server_obj,
                                       os_params,
                                       &core_server_options,
                                       &service_handle);

    rc = (qmi_core_server_error_type)csi_rc;
    if (csi_rc == QMI_CSI_NO_ERR )
        core_object->core_server_data->service_handle = service_handle;
    
    return rc;
}
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
qmi_core_server_unregister(void  *server_obj)
{
      qmi_csi_error rc;
      qmi_core_server_object_type  *core_object;
      if (server_obj == NULL ) {
          return QMI_CORE_SERVER_INVALID_OBJECT;
      }

      core_object = (qmi_core_server_object_type*)server_obj;

      if (core_object == NULL) 
          return QMI_CORE_SERVER_INVALID_OBJECT;

      rc = qmi_csi_unregister(core_object->core_server_data->service_handle);

      return (qmi_core_server_error_type)rc;

}
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
@param[in]   num_ind          Number of Indications implemented by the server.
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
                                void                         *client_data)
{
    qmi_container_error_type list_err;
    uint32_t                 client_id;
    uint8_t                  *struct_ptr;
    uint32_t                 ind_index;
    qmi_indication_type      *ind_obj;


    if (client_conn == NULL ||  core_object == NULL || client_data == NULL )
        return QMI_CORE_SERVER_INVALID_OBJECT;

    /* Storing the address of client connection object in the list */
    if((client_id = qmi_container_add(core_object->list_service,
                                      client_conn,
                                      &list_err)) != 0)
    {
        /* Constructing a client connection object */
        client_conn->client_handle = client_handle;
        client_conn->client_id = client_id;  
        client_conn->client_obj = client_data;
        client_conn->num_ind =  num_ind;
        client_conn->core_object = core_object;

        /* Set the client handle for the indication objects for
           this particular client */

        struct_ptr = (uint8_t *)client_data;

        struct_ptr = (struct_ptr +  sizeof(qmi_core_conn_obj_type));

        if (struct_ptr !=  NULL ) {
            /* Go through all the indication objects declared in the client data */
            for (ind_index = 0; ind_index < num_ind; ind_index++ )
            {
                ind_obj = (qmi_indication_type *)(struct_ptr);
                ind_obj->client_handle = client_handle;
                /* Skip to next indication object, if the indication object
                   is extended then its size will be used to skip to next
                   one. */
                struct_ptr = (struct_ptr +   ind_obj->size_ind_obj);
            }
        }
        else
            return QMI_CORE_SERVER_INTERNAL_ERR;
    }
    else
        return (qmi_core_server_error_type)list_err;

    return QMI_CORE_SERVER_NO_ERR;
}
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
qmi_core_server_unregister_client(qmi_core_conn_obj_type     *client_conn)
{
    qmi_container_error_type list_err;

    if (client_conn == NULL )
        return QMI_CORE_SERVER_INVALID_OBJECT;
    

    list_err = qmi_container_remove(client_conn->core_object->list_service,
                                    client_conn->client_id);

    return (qmi_core_server_error_type)list_err; 
}
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
                             int32_t                  req_c_struct_len)
{

    qmi_dispatch_service_error_type dispatch_err;

    if (client_conn ==  NULL )
        return QMI_CORE_SERVER_INVALID_OBJECT;

    dispatch_err = qmi_dispatch_service_dispatch_msg(client_conn->core_object->dispatch_service,
                                                     server_obj,
                                                     client_conn,
                                                     req_handle,
                                                     msg_id,
                                                     req_c_struct,
                                                     req_c_struct_len);

    return (qmi_core_server_error_type)dispatch_err;
  
}

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
                         int32_t                       res_c_struct_len)
{

    /* Walk thru the list of connection object and send indications
    based on the clients indication flags */

    return (qmi_core_server_error_type)qmi_container_traverse(core_object->list_service,
                                  msg_id,
                                  res_c_struct,
                                  res_c_struct_len);
                             

}

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
qmi_core_server_start_server(void   *server_obj)
{
    qmi_core_server_object_type *core_object;

    if (server_obj == NULL) 
        return QMI_CORE_SERVER_INVALID_OBJECT;

    core_object =  (qmi_core_server_object_type*)server_obj;
    /* Put the server object as argument that gets passed to the
     entry function */

    ((qmi_task_service_type*)core_object->task_service)->args = server_obj;

    if(qmi_task_service_has_started(core_object->task_service))
    {
      return QMI_CORE_SERVER_NO_ERR;
    }

    return (qmi_core_server_error_type)qmi_task_service_create_task(core_object->task_service);
}

/*===========================================================================
  FUNCTION  qmi_core_server_handle_event
===========================================================================*/
/*!
@brief
  Handle event after the server thread receives an event notification.
  Callbacks from qmi_core_server_register may be invoked in
  the server's context.

@param[in]  core_object           Pointer to server object.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_handle_event(void *server_obj)
{
    
    qmi_core_server_object_type *core_object;
    core_object =  (qmi_core_server_object_type *)server_obj;
    return (qmi_core_server_error_type)qmi_csi_handle_event(core_object->core_server_data->service_handle,
                                &(core_object->core_server_data->os_params));
}

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
                          int32_t            c_struct_len)
{
    return (qmi_core_server_error_type)qmi_csi_send_resp(req_handle,
                             msg_id,
                             c_struct,
                             c_struct_len);
}
/*===========================================================================
  FUNCTION  qmi_core_server_check_valid_object
===========================================================================*/
/*!
@brief
  Checks if the core server object passed in all of the above APIs
  was created by the qmi_core_server_new API.

@param[in]  server_obj            Pointer to server object.
                              
@retval    QMI_CORE_SERVER_NO_ERR on Success
@retval    QMI_CORE_SERVER_...    Error, see error codes defined
                                  in qmi_core_server.h
*/                                                                      
/*=========================================================================*/
qmi_core_server_error_type
qmi_core_server_check_valid_object(void *server_obj)
{
    qmi_core_server_object_type *core_object;


    if (server_obj == NULL )
        return QMI_CORE_SERVER_INVALID_OBJECT;

    core_object = (qmi_core_server_object_type *)server_obj;

    if (core_object->core_object_id != CORE_OBJECT_ID ) 
        return QMI_CORE_SERVER_INVALID_OBJECT;
    
    return QMI_CORE_SERVER_NO_ERR;
}
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
qmi_core_server_get_client_data(qmi_core_conn_obj_type *conn_obj)
{
    if (conn_obj == NULL )
        return NULL;

    return conn_obj->client_obj;
    
}
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
qmi_core_server_get_os_params( qmi_core_server_object_type  *core_object)
{
    return(&(core_object->core_server_data->os_params));
}

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
                              qmi_core_server_resume_cb_type resume_cb)
{
  if(!core_object || !resume_cb)
    return QMI_CORE_SERVER_INVALID_OBJECT;
  core_object->core_server_data->resume_cb = resume_cb;
  return QMI_CORE_SERVER_NO_ERR;
}

