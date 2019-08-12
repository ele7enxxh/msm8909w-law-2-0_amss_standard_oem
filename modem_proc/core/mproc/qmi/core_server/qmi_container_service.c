#include "qmi_container_service.h"
#include "core_server_platform.h"
/*---------------------------------------------------------------------------
   QMI_CONTAINER_SERVICE.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_container_service.c

  @brief
    This file contains the implementation of the container service
    APIs.

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
  LOCAL  FUNCTIONS Specific to list implementation
===========================================================================*/


/*===========================================================================
   FUNCTION qmi_initialize_list
===========================================================================*/
qmi_container_error_type
qmi_initialize_list(qmi_list_node_type **HEAD,
                    qmi_list_node_type **END)
{
    *HEAD = NULL;
    *END = NULL;

    return QMI_LIST_NO_ERR;
}

/*===========================================================================
   FUNCTION qmi_insert_list
===========================================================================*/
qmi_container_error_type
qmi_insert_list(qmi_list_node_type **HEAD,
                qmi_list_node_type **TAIL,
                key_type           search_key,
                void               *data)
{

   qmi_list_node_type *new_node;

   qmi_list_node_type *list_node = *HEAD;

   while (list_node != NULL ) {
       if (list_node->key == search_key )
           break;
       list_node = list_node->link.next;
   }

   if (list_node == NULL ) {
       new_node = ALLOCATOR(sizeof(qmi_list_node_type));

       if (new_node == NULL )
           return QMI_LIST_NO_MEM;
       
       new_node->key = search_key;
       new_node->data = data;
       new_node->link.prev = NULL;
       new_node->link.next = NULL;

       if(!(*TAIL)) {
       /* first node on the list */ 
       *HEAD = *TAIL = new_node;
       }
       else 
       {
           new_node->link.prev = *TAIL; 
           (*TAIL)->link.next = new_node;
           (*TAIL) = new_node;
       }
   }
   else {
       /* Overwriting data on the existing node */
       list_node->data = data;
   }

   return QMI_LIST_NO_ERR;
}

/*===========================================================================
   FUNCTION qmi_delete_list
===========================================================================*/
qmi_container_error_type
qmi_delete_list(qmi_list_node_type **HEAD,
                qmi_list_node_type **TAIL,
                key_type           search_key)
{
    qmi_container_error_type list_err;
    qmi_list_node_type  *list_node;

    list_node = *HEAD;

    while (list_node != NULL ) {
       if (list_node->key == search_key )
           break;
       list_node = list_node->link.next;
   }

   if (list_node != NULL ) {
       if(list_node->link.prev)
           list_node->link.prev->link.next = list_node->link.next;
         else /* node at the front of the list */ 
           *HEAD = list_node->link.next;
         if(list_node->link.next)
           list_node->link.next->link.prev = list_node->link.prev;
         else /* node at the end of the list */
           *TAIL = list_node->link.prev;
         
         DEALLOCATE(list_node);
         list_err = QMI_LIST_NO_ERR;
   }
   else
   {
       DEBUG("NODE NOT FOUND.......%d\n",search_key,0,0);
       list_err = QMI_LIST_NODE_NOT_FOUND;
   }
   return list_err;
}

/*===========================================================================
   FUNCTION qmi_search_list
===========================================================================*/
qmi_container_error_type
qmi_search_list(qmi_list_node_type *HEAD,
                key_type           search_key,
                void               **data)
{
    qmi_container_error_type list_err;
    qmi_list_node_type *list_node;

    list_node = HEAD;

    while (list_node != NULL ) {
       if (list_node->key == search_key )
           break;

       list_node = list_node->link.next;
   }

    if (list_node != NULL ) {
        *data = list_node->data;
        list_err =  QMI_LIST_NO_ERR; 
    }
    else
    {
        list_err = QMI_LIST_NODE_NOT_FOUND;
    }

    return list_err;
}

/* Public functions */
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
                  void                     *callback_func)
{
    qmi_container_service_type *list_service;

    list_service = ALLOCATOR(sizeof(qmi_container_service_type));

    if (list_service == NULL ) {
		DEBUG("MEMORY ALLOCATION ERROR...:%s\n","list_service",0,0);
        return  NULL;
    }

    /* Initializing the list interface */
    list_service->list_if.initialize_list = qmi_initialize_list;
    list_service->list_if.insert_list = qmi_insert_list;
    list_service->list_if.search_list = qmi_search_list;
    list_service->list_if.delete_list = qmi_delete_list;
    list_service->list_if.free_list = NULL;
   
    /* Initialize the lock */
	LOCK_INIT_DATA(list_service->list_lock,NULL);
    
    /* Initialize the counter and the variable used to assign node id */
    list_service->qmi_list.count = 0;
    list_service->node_id = 0;
    list_service->callback_func = (callback_fn_node_data)callback_func;

    /* Initialize the head and tail of the list */
    list_service->list_if.initialize_list(&(LIST_HEAD(list_service->qmi_list)),
                                          &(LIST_TAIL(list_service->qmi_list)));

    return list_service;
}

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
qmi_container_delete(qmi_container_service_type *list)
{
    qmi_container_error_type list_err;

    if (list == NULL ) {

        list_err =  QMI_INVALID_LIST;

    }else if (list->qmi_list.count != 0) {
        /* Do not delete the list if there are items in it */
        list_err = QMI_NON_EMPTY_LIST;

    }else {

         DEALLOCATE(list);
         list_err = QMI_LIST_NO_ERR;
    }
    return list_err;
}

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
qmi_container_add(qmi_container_service_type *list,
                  void                       *data,
                  qmi_container_error_type *err)
{


    qmi_container_error_type list_err;
  

    if (list == NULL ) {
        *err = QMI_INVALID_LIST;
        return 0;/* Zero is not a valid node id */
    }

    /* Lock the list before adding */
    LOCK_DATA(list->list_lock);

    /* Increment the node_id and count */
    list->node_id++;
    list->qmi_list.count++;
    list_err = list->list_if.insert_list(&(LIST_HEAD(list->qmi_list)),
                                         &(LIST_TAIL(list->qmi_list)),
                                         list->node_id,
                                         data);

    if (list_err != QMI_LIST_NO_ERR ) {
        *err = list_err;
        UNLOCK_DATA(list->list_lock);
        return 0;
    }
    
    
    UNLOCK_DATA(list->list_lock);
    return (key_type)list->node_id;
}
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
qmi_container_remove(qmi_container_service_type *list,
                     key_type                   key)
                
{
    qmi_container_error_type list_err;

    if (list == NULL ) {
        return QMI_INVALID_LIST;
    }

    /*Lock the list before  removing */
    LOCK_DATA(list->list_lock);

    list_err = list->list_if.delete_list(&(LIST_HEAD(list->qmi_list)),
                                         &(LIST_TAIL(list->qmi_list)),
                                         key);
    if (list_err == QMI_LIST_NO_ERR ) {
        list->qmi_list.count--;
    }

   UNLOCK_DATA(list->list_lock);

   return list_err;
}
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
qmi_container_search(qmi_container_service_type *list,
                     key_type                   key,
                     void                       **data)
{
    qmi_container_error_type list_err;

    if (list == NULL ) {
        return QMI_INVALID_LIST;
    }

    LOCK_DATA(list->list_lock);

    list_err = list->list_if.search_list(LIST_HEAD(list->qmi_list),
                                         key,
                                         data);

    UNLOCK_DATA(list->list_lock);

    return list_err;
}

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
qmi_container_traverse(qmi_container_service_type *list,
                       int32_t                    msg_id,
                       void                       *data,
                       int32_t                    len)
{
    qmi_list_node_type *list_node;
	
	if (list == NULL )
        return QMI_INVALID_LIST;
   

    LOCK_DATA(list->list_lock);

    list_node = LIST_HEAD(list->qmi_list);

    while (list_node != NULL ) {
        /* Call the callback function with the data provided */
        list->callback_func(list_node->data,
                            msg_id,
                            data,
                            len);

        list_node = list_node->link.next;
    }

    UNLOCK_DATA(list->list_lock);
    return QMI_LIST_NO_ERR;
}




