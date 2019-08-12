#include "qmi_ping_server.h"

/* XXX This is an internal header to QMI-FW. If using this source 
 * as an example, please do not include this in your 
 * implementation. You should provide your own macro
 * for QMI_MEM_COPY */
#include "qmi_common.h"
/*---------------------------------------------------------------------------
   QMI_PING_SERVER.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_ping_server.c

  @brief
    This file contains the implementation of the ping service.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/core_server/qmi_ping_server.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
#define NUM_OF_MSGS 4


static uint32_t ref_count;
/*===========================================================================
 Externed Functions.
===========================================================================*/
extern void 
ping_server_event_loop(ping_server *me);

/*===========================================================================
 Local Functions.
===========================================================================*/
ping_server_error
ping_server_init(ping_server                 *me,
                 char                        *object_name,
         uint32_t                    instance_id);

void ping_send_inds(ping_indication_data_type *ind_data);

void
ping_server_handle_event(ping_server *me,
                         void        *event);

uint32_t
ping_server_get_client_index(ping_server *me);

ping_server_error
ping_server_initialize_indication_data(ping_indication_data_type *ind);

ping_server_error
ping_server_send_data_ind(void      *ind_obj,
              void      *ind_c_struct,
              uint32_t  ind_c_struct_len);

/*===========================================================================
 Response Functions corresponding to each request message.These
 functions should be filled with appropriate response behaviour.
===========================================================================*/

ping_server_error
qmi_ping_req_v01_handler(void               *server_data,
                void               *conn_obj,
                          qmi_req_handle     req_handle,
                          uint32_t           msg_id,
                          void               *req_c_struct,
                          uint32_t           req_c_struct_len)
{


    test_ping_req_msg_v01  *recv_msg;
    test_ping_resp_msg_v01 resp_msg;
    
    recv_msg = (test_ping_req_msg_v01 *)req_c_struct;

    DEBUG("RECEIVED MSG QMI_PING_REQ_V01:%d WITH CONTENT:%s\n",
          QMI_TEST_REQ_V01,  
          recv_msg->ping,0);

    memset( &resp_msg, 0, sizeof(test_ping_resp_msg_v01) );
    QMI_MEM_COPY( resp_msg.pong, sizeof(resp_msg.pong), "pong", 4);
    resp_msg.pong_valid = 1;

    resp_msg.resp.error = QMI_ERR_NONE_V01;
    resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;

    DEBUG("SENDING REPONSE MSG QMI_TEST_RESP_V01:%d WITH CONTENT:%s\n",
           QMI_TEST_RESP_V01,
           resp_msg.pong,0);

    return (ping_server_error)qmi_core_server_send_resp(req_handle,
                                     QMI_TEST_RESP_V01,
                                     &resp_msg,
                                     sizeof(test_ping_resp_msg_v01));

}

ping_server_error
qmi_ping_data_req_v01_handler(void               *server_data,
              void               *conn_obj,
                          qmi_req_handle     req_handle,
                          uint32_t           msg_id,
                          void               *req_c_struct,
                          uint32_t           req_c_struct_len)
{

   test_data_req_msg_v01 *recv_msg;
   test_data_resp_msg_v01 *resp_msg;
   ping_server_error rc;

   resp_msg = (test_data_resp_msg_v01 *)calloc(1, sizeof(test_data_resp_msg_v01));
   if(!resp_msg)
   {
     return PING_SERVER_INTERNAL_ERR;
   }

   recv_msg = (test_data_req_msg_v01 *)req_c_struct;
   resp_msg->data_len = recv_msg->data_len;

   if(resp_msg->data_len)
   {
     QMI_MEM_COPY(resp_msg->data, 
                  TEST_MED_DATA_SIZE_V01, 
                  recv_msg->data, 
                  resp_msg->data_len);
     resp_msg->data_valid = 1;
   }

   resp_msg->resp.error = QMI_ERR_NONE_V01;
   resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;

   /* Send a response back to client */
   rc =  (ping_server_error)qmi_core_server_send_resp(req_handle,
                                    QMI_TEST_DATA_RESP_V01,
                                    resp_msg,
                                    sizeof(test_data_resp_msg_v01));

   free(resp_msg);
   return rc;
}

ping_server_error
qmi_ping_large_data_req_v01_handler(void               *server_data,
              void               *conn_obj,
                          qmi_req_handle     req_handle,
                          uint32_t           msg_id,
                          void               *req_c_struct,
                          uint32_t           req_c_struct_len)
{

   test_large_data_req_msg_v01 *recv_msg;
   test_large_data_resp_msg_v01 *resp_msg;
   ping_server_error rc;

   resp_msg = (test_large_data_resp_msg_v01 *)calloc(1, sizeof(test_large_data_resp_msg_v01));
   if(!resp_msg)
   {
     return PING_SERVER_INTERNAL_ERR;
   }

   recv_msg = (test_large_data_req_msg_v01 *)req_c_struct;
   resp_msg->data_len = recv_msg->data_len;

   if(resp_msg->data_len)
   {
     QMI_MEM_COPY(resp_msg->data, 
                  TEST_LARGE_MAX_DATA_SIZE_V01, 
                  recv_msg->data, 
                  resp_msg->data_len);
     resp_msg->data_valid = 1;
   }

   resp_msg->resp.error = QMI_ERR_NONE_V01;
   resp_msg->resp.result = QMI_RESULT_SUCCESS_V01;

   /* Send a response back to client */
   rc =  (ping_server_error)qmi_core_server_send_resp(req_handle,
                                    QMI_TEST_LARGE_DATA_RESP_V01,
                                    resp_msg,
                                    sizeof(test_large_data_resp_msg_v01));

   free(resp_msg);

   return rc;
}


ping_server_error
qmi_ping_data_ind_reg_req_v01_handler(void               *server_data,
                  void               *conn_obj,
                                  qmi_req_handle     req_handle,
                                  uint32_t           msg_id,
                                  void               *req_c_struct,
                                  uint32_t           req_c_struct_len)

{

   test_data_ind_reg_req_msg_v01  *recv_msg;
   test_data_ind_reg_resp_msg_v01 resp_msg;
   test_data_ind_msg_v01          *ind = NULL;
   ping_server_client_data_type   *client_data;
   ping_server_error              rc;

   memset(&resp_msg, 0, sizeof(resp_msg));

   recv_msg = (test_data_ind_reg_req_msg_v01*)req_c_struct;
   DEBUG("RECEVIED MSG QMI_TEST_DATA_IND_REG_REQ_V01:%d",QMI_TEST_DATA_IND_REG_REQ_V01,0,0);
   /* This pointer gives access to the client data */
   client_data = qmi_core_server_get_client_data((qmi_core_conn_obj_type *)conn_obj);
   if(client_data)
   {
     if(recv_msg->num_inds_valid)
       client_data->ping_ind.num_inds =  recv_msg->num_inds;
   
     if(recv_msg->ind_size_valid)
       client_data->ping_ind.ind_size = recv_msg->ind_size;

     if(recv_msg->ms_delay_valid)
       client_data->ping_ind.ind_delay = recv_msg->ms_delay;

     if(recv_msg->num_reqs_delay_valid)
       client_data->ping_ind.num_reqs = recv_msg->num_reqs_delay;

     ind = calloc(1, sizeof(test_data_ind_msg_v01));
     if(!ind)
     {
       resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
       resp_msg.resp.error =  QMI_ERR_NO_MEMORY_V01;
     }
     else
     {
       resp_msg.resp.result = QMI_RESULT_SUCCESS_V01;
       resp_msg.resp.error =  QMI_ERR_NONE_V01;
     }
   }
   else
   {
     resp_msg.resp.result = QMI_RESULT_FAILURE_V01;
     resp_msg.resp.error =  QMI_ERR_NO_MEMORY_V01;
   }

   rc  =  (ping_server_error)qmi_core_server_send_resp(req_handle,
                                    QMI_TEST_DATA_IND_REG_RESP_V01,
                                    &resp_msg,
                                    sizeof(test_data_ind_reg_resp_msg_v01));

   if(rc)
   {
     if(ind)
       free(ind);
     return rc;
   }

   /* Typical use case for services is to add the client to the list of "would like indications".
    * And whenever the service would like to notify the client, it would go through this list,
    * and send an indication using xxx_send_data_ind().
    * 
    * But in this case, all we would like is to test if indications work or not. 
    * So, the ping client would register with the service that it needs x number of indications, 
    * and the service should respond by sending x indications.
    */

   if(ind)
   {
     rc = ping_server_send_data_ind(&client_data->ping_ind, ind, sizeof(test_data_ind_msg_v01));
   }

   return rc;
}


/*===========================================================================
 Dispatch table for ping server.
===========================================================================*/
qmi_msg_handler_type ping_server_dispatcher[NUM_OF_MSGS] = {
     {QMI_TEST_REQ_V01,(qmi_dispatch_fn_type)qmi_ping_req_v01_handler},
     {QMI_TEST_DATA_REQ_V01,(qmi_dispatch_fn_type)qmi_ping_data_req_v01_handler},
     {QMI_TEST_LARGE_DATA_REQ_V01,(qmi_dispatch_fn_type)qmi_ping_large_data_req_v01_handler},
     {QMI_TEST_DATA_IND_REG_REQ_V01,(qmi_dispatch_fn_type)qmi_ping_data_ind_reg_req_v01_handler}
};


/*===========================================================================
 Callback Implementation of QCSI interface.
===========================================================================*/
qmi_csi_cb_error
ping_server_connect(qmi_client_handle client_handle,
                    ping_server       *me,
                    void              **connection_handle)
{
    qmi_core_server_error_type core_server_err;
    uint32_t client_index;

    DEBUG("Connect callback called.............. \n",0,0,0);
    
    if ( qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR )
        return QMI_CSI_CB_INTERNAL_ERR;


    if((client_index = ping_server_get_client_index(me)) == MAX_NUM_CLIENTS )
        return QMI_CSI_CB_CONN_REFUSED;


    /* Register the client */
    core_server_err = qmi_core_server_register_client(&(me->client_data[client_index].conn_obj),
                                                      &(me->core_object),
                                                      client_handle,
                                                      1,/* Number of indication */
                                                      &(me->client_data[client_index]));

    
    if (core_server_err != QMI_CORE_SERVER_NO_ERR ) 
        return QMI_CSI_CB_INTERNAL_ERR;
    else 
        /* Set this client as active client */
        me->client_data[client_index].active_flag = 1;

    /* Assign connection handle to the address of the connection object */
    *connection_handle = &(me->client_data[client_index].conn_obj);

    DEBUG("Returning from the connect callback.........\n",0,0,0);
    return QMI_CSI_CB_NO_ERR;
}


qmi_csi_cb_error
ping_server_process_req(void                 *connection_handle,
                        qmi_req_handle       req_handle,
                        int                  msg_id,
                        void                 *req_c_struct,
                        int                  req_c_struct_len,
                        ping_server          *me)
{
    
    qmi_core_server_error_type core_server_err;
    qmi_core_conn_obj_type    *conn_obj;

    DEBUG("PROCESS REQUEST CALLBACK CALLED..........\n",0,0,0);

    if  (qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR ||
         connection_handle == NULL  )
        return QMI_CSI_CB_INTERNAL_ERR;
    

    conn_obj = (qmi_core_conn_obj_type *)connection_handle;

    /* Response/Request handler functions will eventually get called based on msg_id
       received */

    core_server_err = qmi_core_server_dispatch_msg(conn_obj,
                                                   me,
                                                   req_handle,
                                                   msg_id,
                                                   req_c_struct,
                                                   req_c_struct_len);

    if (core_server_err != QMI_CORE_SERVER_NO_ERR ) 
        return QMI_CSI_CB_INTERNAL_ERR;
    else
        return QMI_CSI_CB_NO_ERR;
}

qmi_csi_cb_error
ping_server_disconnect( void          *connection_handle,
                        ping_server   *me )
{
    
    qmi_core_server_error_type core_server_err;
    qmi_core_conn_obj_type     *conn_obj; 
  ping_server_client_data_type  *client_data;

    DEBUG("Disconnect callback called.............\n",0,0,0);

    if (qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR  || 
        connection_handle == NULL ) 
        return QMI_CSI_CB_INTERNAL_ERR;
    
   
   conn_obj = (qmi_core_conn_obj_type *)connection_handle;

   core_server_err = qmi_core_server_unregister_client(conn_obj);

   if (core_server_err != QMI_CORE_SERVER_NO_ERR ) 
   {
       return QMI_CSI_CB_INTERNAL_ERR;
   }
    else
   {
     client_data = qmi_core_server_get_client_data((qmi_core_conn_obj_type *)conn_obj);
     if(client_data)
     {
       client_data->active_flag = 0;
       client_data->transaction_counter = 0;
     }
   }
       
   return QMI_CSI_CB_NO_ERR;
}




void ping_server_handle_event(ping_server *me,
                              void        *event)
{
    /* Call the process event function. */
    /* Server context at this point knows that there is an event. */
    /* The event is a rex signal(in case of rex implementation ) that can be
       used by the server.*/
    /* qmi_core_server_handle_event will call  qmi_csi_handle_event function
       and that  will eventually call the  ping_server_process_req callback */
    qmi_core_server_handle_event(me);
}


/*===========================================================================
  FUNCTION  ping_server_class_init
===========================================================================*/
/*!
@brief
  This function initializes the reference count of ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error 
ping_server_class_init()
{
    ref_count = 0;
    /*Initialize other static class data*/
    return PING_SERVER_NO_ERROR;
}
/*===========================================================================
  FUNCTION  ping_server_class_destruct
===========================================================================*/
/*!
@brief
  This function cleans up any global data structure visible to 
  all ping server objects.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_class_destruct()
{
    return PING_SERVER_NO_ERROR;
}

/*===========================================================================
  FUNCTION  ping_server_init
===========================================================================*/
/*!
@brief
  This function creates a ping server object and initializes the client
  and server data.
@param[out]    me            Valid ping server object.
@param[in]     object_name   C string specifying the name of the object.
@param[in]     instance_id   Instance id assocaited with the object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_init(ping_server                 *me,
                 char                        *object_name,
         uint32_t                    instance_id)
                 
{
    qmi_core_server_error_type rc;
    uint32_t index;
    uint32_t priority = 14;
    unsigned long sig = QMI_PING_SVC_WAIT_SIG;


    /*Construct and Initialize the core server object */
    rc = qmi_core_server_new(&(me->core_object),
                             object_name,
               instance_id,/* Instance id */
                             1,/* Task flag */
                             (void*)ping_server_event_loop,
                             (void*)&priority,
               NULL,
               (void *)&sig,
               NULL,
                             ping_server_dispatcher,
                             sizeof(ping_server_dispatcher)/sizeof(qmi_msg_handler_type));

    if (rc == QMI_CORE_SERVER_NO_ERR ) 
    {
        /* Initialize the client data */
        for (index = 0; index < MAX_NUM_CLIENTS; index++ ) {
            memset(&(me->client_data[index].conn_obj),0,sizeof (qmi_core_conn_obj_type));
            /*Initialize each indication object declared in the client structure */
            ping_server_initialize_indication_data(&(me->client_data[index].ping_ind));
                                                   
            me->client_data[index].transaction_counter = 0;
            me->client_data[index].active_flag = 0;
        }
        me->client_counter = 0;
        me->client_index = 0;
        /* Initialize rest of the server data here */
    }
    return (ping_server_error)rc;
}

/*===========================================================================
  FUNCTION  ping_server_new
===========================================================================*/
/*!
@brief
  This function creates a new ping server object.

@param[in]  object_name     C string specifying the name.
@param[in]  instance_id     Id that gets associated with the server.
@param[out] err             Error values as specified in ping_server_error.

@retval    Returns a ping_server object on Success, NULL with appropriate
           error code set in err on failure.
*/
/*=========================================================================*/
ping_server*
ping_server_new(char                    *object_name,
        uint32_t                instance_id,
                ping_server_error       *err)
{
    ping_server*           ping_server_object;
    qmi_core_server_error_type      rc;
    /*The default priority of one greater than DIAG's task priority is 
      considered if NULL is passed to the API */
    uint32_t priority = 14;
    uint32_t index;
    unsigned long sig = QMI_PING_SVC_WAIT_SIG;
    
    
    ping_server_object =   ALLOCATOR(sizeof(ping_server_class_type));

    if (ping_server_object  == NULL ) {
        *err = (ping_server_error)QMI_CORE_SERVER_MEMORY_ERR;
        return NULL;
    }

    /*Construct and Initialize the core server object */
    rc = qmi_core_server_new(&(ping_server_object->core_object),
                             object_name,
                             instance_id,/* Instance id */
                             1,/* Task flag */
                             (void*)ping_server_event_loop,
                             (void*)&priority,
                             NULL,
                             (void *)&sig,
                             NULL,
                             ping_server_dispatcher,
                             sizeof(ping_server_dispatcher)/sizeof(qmi_msg_handler_type));

    if (rc == QMI_CORE_SERVER_NO_ERR ) 
    {
        /* Initialize the client data */
        for (index = 0; index < MAX_NUM_CLIENTS; index++ ) {
            memset(&(ping_server_object->client_data[index].conn_obj),0,sizeof (qmi_core_conn_obj_type));
            /*Initialize each indication object declared in the client structure */
            ping_server_initialize_indication_data(&(ping_server_object->client_data[index].ping_ind));
                                                   
            ping_server_object->client_data[index].transaction_counter = 0;
            ping_server_object->client_data[index].active_flag = 0;
        }
        ping_server_object->client_counter = 0;
        ping_server_object->client_index = 0;
        /* Initialize rest of the server data here */
        ref_count++;
        return ping_server_object;
    }
    else
    {
        DEALLOCATE(ping_server_object);
        *err = (ping_server_error)rc;
        return NULL;
    }
}
/*===========================================================================
  FUNCTION  ping_server_delete
===========================================================================*/
/*!
@brief
  This function destroys a ping server object.

@param[in] me     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_delete(ping_server  *me)
{
    qmi_core_server_error_type core_server_rc;

    if ( qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR )
        return PING_SERVER_INVALID_OBJECT;
    
    /* Destruct the core object */
    core_server_rc = qmi_core_server_delete(&(me->core_object));
    if (core_server_rc != QMI_CORE_SERVER_NO_ERR )
        return (ping_server_error)core_server_rc;
    
    /* Free  the server object */
    DEALLOCATE(me);
    ref_count--;
    return PING_SERVER_NO_ERROR;
}

static void ping_server_resume(qmi_core_conn_obj_type *conn_obj)
{
  ping_server_client_data_type *ping_client = 
    (ping_server_client_data_type *)conn_obj;
  if(!ping_client)
    return;
  if(ping_client->ping_ind.num_inds > 0)
    ping_send_inds(&ping_client->ping_ind);
}

/*===========================================================================
  FUNCTION  ping_server_register
===========================================================================*/
/*!
@brief
  This function register a ping server object with the framework.

@param[in] me      ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error 
ping_server_register(ping_server *me )
{
  
    qmi_core_server_error_type rc;
    
    if ( qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR )
            return PING_SERVER_INVALID_OBJECT;
    
    rc = qmi_core_server_set_resume_cb(&me->core_object, ping_server_resume);

    /* Registering the service object and callbacks with QCSI framework
       using the core server object  */


    rc = qmi_core_server_register(me,
                                  test_get_service_object_v01( ),
                                  (qmi_csi_connect)ping_server_connect,
                                  (qmi_csi_disconnect)ping_server_disconnect,
                                  (qmi_csi_process_req)ping_server_process_req);
                        
   return (ping_server_error)rc;

}

/*===========================================================================
  FUNCTION  ping_server_unregister
===========================================================================*/
/*!
@brief
  This function de-registers a ping server object with the framework.

@param[in] me     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_unregister(ping_server *me)
{
    qmi_core_server_error_type rc;

    if ( qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR )
            return PING_SERVER_INVALID_OBJECT;
    
    rc = qmi_core_server_unregister(me);

    return (ping_server_error)rc;
}

/*===========================================================================
  FUNCTION  ping_server_start_server
===========================================================================*/
/*!
@brief
  This function starts the ping server.

@param[in]  me     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error
@note
           This function should not be defined in case of taskless server.
*/
/*=========================================================================*/
ping_server_error
ping_server_start_server(ping_server *me)
{

    qmi_core_server_error_type rc;

    if ( qmi_core_server_check_valid_object(me) != QMI_CORE_SERVER_NO_ERR )
            return PING_SERVER_INVALID_OBJECT;

    rc = qmi_core_server_start_server(me);

    return (ping_server_error)rc;
}
/*===========================================================================
  FUNCTION  ping_server_get_client_index
===========================================================================*/
/*!
@brief
  Retreives the index of the client data array corresponding to client data
  information that is not active and can be used to store new client data.

@param[in] me              ping server object.

@retval    Returns the index.
*/
/*=========================================================================*/
uint32_t
ping_server_get_client_index(ping_server *me)
{
    
    uint32_t client_index;

    for (client_index = 0; client_index < MAX_NUM_CLIENTS; client_index++ ) {
        if (me->client_data[client_index].active_flag == 0) 
        break;
    }

    return  client_index;
}

void ping_send_inds(ping_indication_data_type *ind_data)
{
  if(!ind_data || !ind_data->ind_buf)
    return;

  for(;ind_data->inds_sent < ind_data->num_inds; ind_data->inds_sent++)
  {
    qmi_csi_error rc = (qmi_csi_error)qmi_indication_send(&ind_data->base_ind,
                                                          ind_data->ind_buf,
                                                          ind_data->ind_buf_len);
    if(rc == QMI_CSI_CONN_BUSY)
      break;
    if(rc != QMI_CSI_NO_ERR)
    {
      ind_data->inds_sent = ind_data->num_inds;
      break;
    }
  }
  if(ind_data->inds_sent == ind_data->num_inds)
  {
    free(ind_data->ind_buf);
    ind_data->ind_buf = NULL;
    ind_data->ind_buf_len = 0;
    ind_data->inds_sent = ind_data->num_inds = 0;
  }
}

/*===========================================================================
  FUNCTION  ping_server_send_data_ind
===========================================================================*/
/*!
@brief
  This callback function is called to handle indications corresponding to
  message ID QMI_PING_DATA_IND_V01.

@param[in] ind_obj            Extended indication type.
@param[in] ind_c_struct       Indication c structure.
@param[in] ind_c_struct_len   Length of indication c structure.
  
@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error 
*/
/*=========================================================================*/
ping_server_error
ping_server_send_data_ind(void      *ind_obj,
              void      *ind_c_struct,
              uint32_t  ind_c_struct_len)
{
  ping_indication_data_type *ind_data;
  test_data_ind_msg_v01     *ind_data_to_send;
  uint32_t j;

  if (!ind_obj || !ind_c_struct)
    return PING_SERVER_INVALID_OBJECT;

   ind_data =  (ping_indication_data_type*)ind_obj;
   ind_data_to_send =  (test_data_ind_msg_v01*)ind_c_struct;
   ind_data_to_send->data_len = ind_data->ind_size;

   for(j = 0;j < ind_data->ind_size; j++)
   {
     ind_data_to_send->data[j] = 100;
   }
   ind_data->inds_sent = 0;
   ind_data->ind_buf = ind_c_struct;
   ind_data->ind_buf_len = ind_c_struct_len;

   ping_send_inds(ind_data);

   return PING_SERVER_NO_ERROR;
}
              

/*===========================================================================
  FUNCTION  ping_server_initlialize_indication_data
===========================================================================*/
/*!
@brief
  This function initializes the base indication and associates a msg id,
  type and callback function with the indication type defined in the
  client data.The registered callback  will be called when the indication 
  is sent thru qmi_core_server_send_ind function.

@param[in] ind    Extended indication type.
  
@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error 
*/
/*=========================================================================*/
ping_server_error
ping_server_initialize_indication_data(ping_indication_data_type *ind)
{
  ping_server_error rc;

    if (ind == NULL ) 
        return PING_SERVER_INVALID_OBJECT;

  memset(ind,0,sizeof(ping_indication_data_type));
    
  rc = (ping_server_error)qmi_indication_initialize((qmi_indication_type *)ind,
                                   QMI_UNICAST_IND,
                                   QMI_TEST_DATA_IND_V01,
                                   sizeof(ping_indication_data_type),
                                  (qmi_indication_send_fn_type)ping_server_send_data_ind );

    /* Initialize any other extended indication data here */

   return rc;
}

/*===========================================================================
  FUNCTION  ping_server_send_ind
===========================================================================*/
/*!
@brief
  This function sends an indication identified by the message Id
  to  the registered clients.
  
@param[in]  me              ping server object.
@param[in]  msg_id          Message Id corresponding to the indication message.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_send_ind( ping_server  *me,
                      int32_t      msg_id)
                      
{
  test_data_ind_msg_v01  ind_msg;
  ping_server_error      rc;

  switch(msg_id)
  {
    case  QMI_TEST_DATA_IND_V01:
    rc = (ping_server_error)qmi_core_server_send_ind(&(me->core_object),
                                msg_id,
                                &ind_msg,
                                sizeof(test_data_ind_msg_v01));
    break;

    default:
    rc = PING_SERVER_UNKNOWN_MESSAGE;
      break;
  }
  return rc;                                  
}

/*===========================================================================
  FUNCTION  ping_server_get_os_params
===========================================================================*/
/*!
@brief
  This function returns a pointer to the OS dependent signaling information.
  
@param[in]  me     ping server object.

@retval    Returns pointer to qmi_csi_os_params. 
*/
/*=========================================================================*/
qmi_csi_os_params*
ping_server_get_os_params(ping_server  *me)
{
  return(qmi_core_server_get_os_params(&(me->core_object)));
}
