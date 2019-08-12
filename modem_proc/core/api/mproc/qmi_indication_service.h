#ifndef _QMI_INDICATION_SERVICE_H_
#define _QMI_INDICATION_SERVICE_H_
/*---------------------------------------------------------------------------
   QMI_INDICATION_SERVICE.H
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_indication_service.h

  @brief
    This file contains the APIs for the indicaition service.

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
#include "stdint.h"
#include "qmi_csi.h"
#include "qmi_core_error.h"
#include "core_server_platform.h"


typedef enum qmi_indication_error {
    QMI_INDICATION_NO_ERR = 0,
    QMI_INDICATION_INVALID_OBJ = QMI_CORE_INDICATION_ERROR,
    QMI_INDICATION_INVALID_TYPE
}qmi_indication_error_type;


typedef enum qmi_indication_class {
    QMI_UNICAST_IND = 0,
    QMI_ONE_SHOT_IND
}qmi_indication_class_type;



typedef qmi_indication_error_type
(*qmi_indication_send_fn_type)(void     *ind,
                               void     *ind_c_struct,
                               uint32_t  ind_c_struct_len);

typedef struct qmi_indication {
    uint8_t                     flag;
    uint32_t                    counter;
    uint32_t                    msg_id;
    qmi_indication_class_type   ind_type;
    qmi_client_handle           client_handle;/*QCSI client handle */
    /*Size of the object that extends this indication object,
      By default it should contain its own size
      to facilitate the sending of indications defined by clients
      in the client data  */
    uint32_t                    size_ind_obj;
    qmi_indication_send_fn_type send_ind;

}qmi_indication_type;

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
qmi_indication_set_flag(qmi_indication_type *ind);
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
qmi_indication_clear_flag(qmi_indication_type *ind);
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
                        uint32_t                  *flag);
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
                           uint32_t            counter);
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
qmi_indication_clear_counter(qmi_indication_type *ind);

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
                          qmi_indication_send_fn_type  send_ind);

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
                    uint32_t             ind_c_struct_len);

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
                              uint32_t                ind_c_struct_len);
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
                        qmi_indication_class_type *ind_class);


#endif 


