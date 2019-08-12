#ifndef _QMI_CONTAINER_SERVICE_H
#define _QMI_CONTAINER_SERVICE_H
/*---------------------------------------------------------------------------
   QMI_CONTAINER_SERVICE.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_container_service.h

  @brief
    This file contains the APIs for the container service of core server
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
#include "stdint.h"
#include "qmi_common.h"
#include "core_server_platform.h"
#include "qmi_core_error.h"

typedef uint32_t key_type;

typedef struct qmi_list_node {
    LINK(struct qmi_list_node,link);
    void *data;
    key_type key;
}qmi_list_node_type;



typedef enum qmi_container_error {
    QMI_LIST_NO_ERR = 0,
    QMI_LIST_NODE_NOT_FOUND = QMI_CORE_LIST_ERROR,
    QMI_LIST_NO_MEM,
    QMI_INVALID_LIST,
    QMI_NON_EMPTY_LIST
}qmi_container_error_type;


/* Interface for list implementation */
typedef qmi_container_error_type (*initialize_list_fn_type)(qmi_list_node_type **HEAD,
                                                            qmi_list_node_type **END);


typedef qmi_container_error_type (*insert_list_fn_type)(qmi_list_node_type **HEAD,
                                                        qmi_list_node_type **TAIL,
                                                        key_type           search_key,
                                                        void               *data);
                                                   

typedef qmi_container_error_type (*search_list_fn_type)(qmi_list_node_type  *HEAD,
                                                        key_type            search_key,
                                                        void                **data);

typedef qmi_container_error_type (*delete_list_fn_type)(qmi_list_node_type **HEAD,
                                                        qmi_list_node_type **TAIL,
                                                        key_type           search_key);
                                                  


typedef qmi_container_error_type (*free_list_fn_type)(qmi_list_node_type *HEAD,
                                                      qmi_list_node_type *END);


/* Callback function that can work on each node data */
typedef qmi_container_error_type (*callback_fn_node_data)(void                  *node_data,
                                                          int32_t               msg_id,
                                                          void                  *data,
                                                          int32_t               len);

typedef struct  qmi_list_interface
{
    /* List Implementation */
    initialize_list_fn_type    initialize_list;
    insert_list_fn_type        insert_list;
    search_list_fn_type        search_list;
    delete_list_fn_type        delete_list;
    free_list_fn_type          free_list;
}qmi_list_interface_type;



typedef struct qmi_container_service {
    LIST(qmi_list_node_type,qmi_list);
    lock_impl                list_lock;
    qmi_list_interface_type  list_if;
    uint32_t                 node_id;
    /* Function that can work on each node data */
    callback_fn_node_data    callback_func;
}qmi_container_service_type;

/*===========================================================================
  FUNCTION  qmi_container_new
===========================================================================*/
/*!
@brief
  This function creats a new container object.

@param[out] err                Pointer to qmi_container_error_type. 
@parma[in]  *callback_func     Callback function that get called on each node.

@retval    NULL on Error, New container object otherwise.
*/
/*=========================================================================*/
qmi_container_service_type*
qmi_container_new(qmi_container_error_type *err,
                  void                *callback_func);

/*===========================================================================
  FUNCTION  qmi_container_delete
===========================================================================*/
/*!
@brief
  This function destroys the container object.

@param[in]  container       Pointer to container object.

@retval     QMI_LIST_NO_ERR on Success.
@retval     QMI_LIST_...    Error, see error codes defined
                                  in qmi_container_error_type
*/
/*=========================================================================*/
qmi_container_error_type
qmi_container_delete(qmi_container_service_type *container);

/*===========================================================================
  FUNCTION  qmi_container_add
===========================================================================*/
/*!
@brief
  This function adds a data to the container object.

@param[in]  container       Pointer to container object.
@param[in]  data            User data.
@param[out] err             Pointer to qmi_container_error_type.

@retval     Returns unique key on Success.
@retval     Return zero on failure. Zero is not a valid key.
*/
/*=========================================================================*/
key_type
qmi_container_add(qmi_container_service_type *container,
                  void                       *data,
                  qmi_container_error_type   *err);

/*===========================================================================
  FUNCTION  qmi_container_remove
===========================================================================*/
/*!
@brief
  This function removes a node from the container object.

@param[in]  container       Pointer to container object.
@param[in]  key             Unique key to identify the node.

@retval     QMI_LIST_NO_ERR on Success.
@retval     QMI_LIST_...    Error, see error codes defined
                                  in qmi_container_error_type
*/
/*=========================================================================*/                                  
qmi_container_error_type 
qmi_container_remove(qmi_container_service_type *container,
                     key_type                   key);

/*===========================================================================
  FUNCTION  qmi_container_search
===========================================================================*/
/*!
@brief
  This function searches the container object for a node specified by the key.

@param[in]  container       Pointer to container object.
@param[in]  key             Unique Key to identify the node.
@param[out] data            The  data corresponding to the key supplied.

@retval     QMI_LIST_NO_ERR on Success.
@retval     QMI_LIST_...    Error, see error codes defined
                                  in qmi_container_error_type
*/
/*=========================================================================*/

qmi_container_error_type
qmi_container_search(qmi_container_service_type *container,
                     key_type                   key,
                     void                       **data);

/*===========================================================================
  FUNCTION  qmi_container_traverse
===========================================================================*/
/*!
@brief
  This function traverse and execute the callback function(registered in
  qmi_list_new) on the data of each node.

@param[in]  list       Pointer to list object.
@param[in]  msg_id     Client specific data passed to the callback. 
@param[in]  data       Client specific data passed to the callback.
@param[in]  len        Client specific data passed to the callback.

@retval     QMI_LIST_NO_ERR on Success.
@retval     QMI_LIST_...    Error, see error codes defined
                                  in qmi_container_error_type
*/
/*=========================================================================*/
qmi_container_error_type
qmi_container_traverse(qmi_container_service_type *container,
                       int32_t                    msg_id,
                       void                       *data,
                       int32_t                    len);


#endif
