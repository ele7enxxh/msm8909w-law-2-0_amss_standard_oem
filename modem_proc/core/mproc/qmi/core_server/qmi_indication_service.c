#include "qmi_indication_service.h"
/*---------------------------------------------------------------------------
   QMI_INDICATION_SERVICE.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_indication_service.h

  @brief
    This file contains the implementation of indicaition service APIs.

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


/*===========================================================================
  FUNCTION  qmi_indication_set_flag
===========================================================================*/
/*!
@brief
  This function activates the indication.

@param[in]  ind    Indication object.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_set_flag(qmi_indication_type *ind)
{
    if (ind == NULL ) 
        return QMI_INDICATION_INVALID_OBJ;
    
    ind->flag = 1;

    return  QMI_INDICATION_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_indication_get_flag
===========================================================================*/
/*!
@brief
  This function determines if a given indication is active or inactive.

@param[in]  ind    Indication object.
@param[out] flag   Status of indication if success
                   active = 1, inactive = 0

@retval    QMI_INDICATION_NO_ERR on Success
@retval    QMI_INDICATION_... Error,. see error codes defined in
                                      qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_get_flag(qmi_indication_type       *ind,
                        uint32_t                  *flag)
{
    if (ind == NULL || flag == NULL)
    {
        return QMI_INDICATION_INVALID_OBJ;
    }
    *flag = ind->flag;
    return QMI_INDICATION_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_indication_clear_flag
===========================================================================*/
/*!
@brief
  This function de-activates the indication.

@param[in]  ind    Indication object.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_clear_flag(qmi_indication_type *ind)
{
    if (ind == NULL ) 
        return QMI_INDICATION_INVALID_OBJ;
    
    ind->flag = 0;
    return QMI_INDICATION_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_indication_set_counter
===========================================================================*/
/*!
@brief
  This function sets the counter for the indication.

@param[in]  ind       Indication object.
@param[in]  counter   Value of the counter.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_set_counter(qmi_indication_type *ind,
                           uint32_t            counter)
{
    if (ind == NULL )
        return QMI_INDICATION_INVALID_OBJ;

    ind->counter = counter;

    return QMI_INDICATION_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_indication_clear_counter
===========================================================================*/
/*!
@brief
  This function sets the counter to zero for the indication.

@param[in]  ind       Indication object.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_clear_counter(qmi_indication_type *ind)
{
    if (ind == NULL ) 
        return QMI_INDICATION_INVALID_OBJ;

    ind->counter  = 0;
    return QMI_INDICATION_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_indication_send
===========================================================================*/
/*!
@brief
  This function sends the indication to the clients if it is active.

@param[in]  ind                   Indication object.
@param[in]  ind_c_struct          C struct with the indication data.
@param[in]  ind_c_struct_len      Length of the c struct.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_send(void                *ind,
                    void                *ind_c_struct,
                    uint32_t             ind_c_struct_len)
{

    qmi_indication_error_type rc = QMI_INDICATION_NO_ERR;
    qmi_indication_type *ind_obj;


    ind_obj = (qmi_indication_type *)ind;
               

    if (ind_obj == NULL )
        return QMI_INDICATION_INVALID_OBJ;
    

    switch (ind_obj->ind_type ) {
    case QMI_ONE_SHOT_IND:
        if (ind_obj->flag == 1 ) {
			DEBUG("SENDING ONE SHOT INDICATION WITH ..........MSG_ID:%d\n",ind_obj->msg_id,0,0);
            rc = (qmi_indication_error_type)qmi_csi_send_ind(ind_obj->client_handle,
                              ind_obj->msg_id,
                              ind_c_struct,
                              ind_c_struct_len);
            /* Reset the flag, since its ONE SHOT indication */
            ind_obj->flag = 0;
        }
        
        break;

    case QMI_UNICAST_IND:
        rc = (qmi_indication_error_type)qmi_csi_send_ind(ind_obj->client_handle,
                              ind_obj->msg_id,
                              ind_c_struct,
                              ind_c_struct_len);
        break;

    default:
        //LOG unknown  indication type
        rc = QMI_INDICATION_INVALID_TYPE;
        break;

    }

    return rc;
}
/*===========================================================================
  FUNCTION  qmi_indication_initialize
===========================================================================*/
/*!
@brief
  This function initializes the supplied indication object.

@param[in]  ind        Indication object.
@param[in]  ind_class  Type of indication as defined in 
                       qmi_indication_class_type.
@param[in]  msg_id     Message Id corresponding to the indication object.
@param[in]  size       Size of the indication object.If the indication
                       object is extended the size of the extended structure
                       should be provided otherwise size of qmi_indication_type
                       should be provided.
@param[in]  send_ind   Callback function that will be used to send this
                       indicaition.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_initialize(qmi_indication_type          *ind,
                          qmi_indication_class_type    ind_class,
                          uint32_t                      msg_id,
                          uint32_t                     size,
                          qmi_indication_send_fn_type  send_ind)
{
    if (ind == NULL ) 
        return QMI_INDICATION_INVALID_OBJ;
    

    ind->counter = 0;
    ind->msg_id = msg_id;
    ind->ind_type = ind_class;
    ind->size_ind_obj = size;

    ind->flag =  (ind_class == QMI_ONE_SHOT_IND)?  0 : 1;
    /*If the indication object is extended then use the send indication
    function assigned by the user, otherwise the default is qmi_indication_send */
    if (send_ind != NULL )
        ind->send_ind = send_ind;
    else
    ind->send_ind = qmi_indication_send;

    return QMI_INDICATION_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_indication_set_client_handle
===========================================================================*/
/*!
@brief
  This function sets the client handle.

@param[in]  ind             Indication object.
@param[in]  client_handle   Handle used by the infrastructure
                            to identify the destination client.
                       

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/

qmi_indication_error_type
qmi_indication_set_client_handle(qmi_indication_type *ind,
                                 qmi_client_handle   client_handle)
{
    if (ind == NULL )
        return QMI_INDICATION_INVALID_OBJ;

    ind->client_handle = client_handle;
    return QMI_INDICATION_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_indication_get_type
===========================================================================*/
/*!
@brief
  This function retrieves the type of indication.

@param[in]  ind        Indication object.
@param[out] ind_class  Type of indication as defined in 
                       qmi_indication_class_type.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_get_type(qmi_indication_type *ind,
                        qmi_indication_class_type *ind_class)
{
    if (ind == NULL ) 
        return QMI_INDICATION_INVALID_OBJ;
    

    *ind_class = ind->ind_type;

    return QMI_INDICATION_NO_ERR;
}
/*===========================================================================
  FUNCTION  qmi_indication_send_broadcast
===========================================================================*/
/*!
@brief
  This function sends a broadcast indication to all clients of the 
  server identified by the service_provider.

@param[in]  service_provider      Handle used by the infrastructure
                                  to identify the service that intends to 
                                  send a broadcast message.
@param[in]  msg_id                Message Id corresponding to the 
                                  indication object.
@param[in]  ind_c_struct          C struct with the indication data.
@param[in]  ind_c_struct_len      Length of the c struct.

@retval    Returns a QMI_INDICATION_NO_ERR on Success.
@retval    QMI_INDICATION_...    Error, see error codes defined
                                  in qmi_indication_error_type
*/
/*=========================================================================*/
qmi_indication_error_type
qmi_indication_send_broadcast(qmi_csi_service_handle service_provider,
                              uint32_t                msg_id,
                              void                   *ind_c_struct,
                              uint32_t                ind_c_struct_len)

{
    return (qmi_indication_error_type)qmi_csi_send_broadcast_ind(service_provider,
                                      msg_id,
                                      ind_c_struct,
                                      ind_c_struct_len);
}




                                 

