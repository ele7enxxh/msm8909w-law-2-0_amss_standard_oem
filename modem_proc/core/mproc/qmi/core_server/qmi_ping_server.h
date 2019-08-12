#ifndef _QMI_PING_H_
#define _QMI_PING_H_
/*---------------------------------------------------------------------------
   QMI_PING_SERVER.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_ping_server.h

  @brief
    This file contains the APIs for the ping service.

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/core_server/qmi_ping_server.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/
#include "qmi_core_server.h"
#include "test_service_v01.h"
#include <stdint.h>



#define MAX_NUM_CLIENTS 20
#define QMI_PING_SVC_WAIT_SIG 0x00030000
/* This structure keeps track of the data we receive from the clients
   via ping_data_ind_reg_req_msg_v01 msg. This data is then used to deliver
   indications to clients via ping_data_ind_msg_v01*/
typedef struct ping_indication_data {
  qmi_indication_type base_ind;
  void                *ind_buf;
  unsigned int        ind_buf_len;
  uint16_t            inds_sent;
  uint16_t            num_inds;
  uint16_t            ind_size;
  uint16_t            ind_delay;
  uint16_t            num_reqs;
}ping_indication_data_type;


/*This struct keeps track of the clients that connects through connect callback*/
typedef  struct ping_server_client_data  {
    /* First element of structure should be connection object */
    qmi_core_conn_obj_type         conn_obj;
    /* Indications should be defined here,probably with descriptive names */
    ping_indication_data_type      ping_ind;
    /* Rest of the client data goes here */
    uint32_t                       transaction_counter;/* Counts number of transaction served */
    uint8_t                        active_flag;         
}ping_server_client_data_type;


/* This is the server object */
typedef struct ping_server_class {
    /* Core object should be first element in this struct */
    qmi_core_server_object_type        core_object;
    /* Client data */
    ping_server_client_data_type       client_data[MAX_NUM_CLIENTS];
    uint32_t                           client_index;/* Index into the client data array */
    uint32_t                           client_counter;/*Counts number of active clients */
    /* Rest of the server data goes here */
}ping_server_class_type;



/* Public APIs */

/* Ping server object */
typedef ping_server_class_type ping_server;


typedef  enum {
    //Inherits from error in the core server
    PING_SERVER_NO_ERROR = QMI_CORE_SERVER_NO_ERR,
    PING_SERVER_INTERNAL_ERR = QMI_CORE_SERVER_INTERNAL_ERR + 1,
    PING_SERVER_NOT_INITIALISED,
    PING_SERVER_INVALID_OBJECT,
    PING_SERVER_UNKNOWN_MESSAGE
}ping_server_error;


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
ping_server_class_init(void);


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
ping_server_class_destruct(void);


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
                ping_server_error       *err);

/*===========================================================================
  FUNCTION  ping_server_delete
===========================================================================*/
/*!
@brief
  This function destroys a ping server object.

@param[in]  ping_object     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_delete(ping_server *ping_object);

/*===========================================================================
  FUNCTION  ping_server_register
===========================================================================*/
/*!
@brief
  This function register a ping server object with the framework.

@param[in]  ping_object     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_register(ping_server *ping_object );

/*===========================================================================
  FUNCTION  ping_server_unregister
===========================================================================*/
/*!
@brief
  This function de-registers a ping server object with the framework.

@param[in]  ping_object     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_unregister(ping_server *ping_object);

/*===========================================================================
  FUNCTION  ping_server_start_server
===========================================================================*/
/*!
@brief
  This function starts the ping server.

@param[in]  ping_object     ping server object.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error
@note
           This function should not be defined in case of taskless server.
*/
/*=========================================================================*/
ping_server_error
ping_server_start_server(ping_server *ping_object);

/*===========================================================================
  FUNCTION  ping_server_send_ind
===========================================================================*/
/*!
@brief
  This function sends an indication identified by the message Id
  to  the registered clients. 
  
@param[in]  ping_object     ping server object.
@param[in]  msg_id          Message Id corresponding to the indication message.

@retval    PING_SERVER_..... Error, see Error codes defined in
           ping_server_error   
*/
/*=========================================================================*/
ping_server_error
ping_server_send_ind( ping_server *ping_object,
                      int32_t     msg_id);

/*===========================================================================
  FUNCTION  ping_server_get_os_params
===========================================================================*/
/*!
@brief
  This function returns a pointer to the OS dependent signaling information.
  
@param[in]  ping_object     ping server object.

@retval    Returns pointer to qmi_csi_os_params. 
*/
/*=========================================================================*/
qmi_csi_os_params*
ping_server_get_os_params(ping_server  *ping_object);

#endif
