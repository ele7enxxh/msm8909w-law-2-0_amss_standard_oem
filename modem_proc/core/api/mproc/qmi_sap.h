#ifndef QMI_SAP_H
#define QMI_SAP_H
/******************************************************************************
  @file    qmi_sap.h
  @brief   The QMI Service Access Proxy (SAP) Header file.

  DESCRIPTION
  QMI service access routines.  Services wanting to export their
  functionality off chip via QMUX will use these routines.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "qmi_sap_target_ext.h"
#include "qmi_idl_lib.h"
#include "qmi_client.h"

typedef void *qmi_sap_client_handle;

typedef enum
{
  QMI_SAP_NO_ERR = 0,
  QMI_SAP_INTERNAL_ERR,
  QMI_SAP_SVC_REGISTERED
}qmi_sap_error;

typedef enum
{
  QMI_SAP_CB_NO_ERR = 0,
  QMI_SAP_CB_INTERNAL_ERR
}qmi_sap_cb_error;

/*=============================================================================
  CALLBACK FUNCTION qmi_sap_client_connect_cb
=============================================================================*/
typedef qmi_sap_cb_error (*qmi_sap_client_connect_cb)
(
  qmi_client_type user_handle
);

typedef qmi_sap_cb_error (*qmi_sap_client_disconnect_cb)
(
  qmi_client_type user_handle
);

typedef struct
{
  unsigned int options_set;
  qmi_sap_client_connect_cb conn_cb;
  qmi_sap_client_disconnect_cb disconn_cb;
}qmi_sap_options;

#define QMI_SAP_OPTIONS_CONNECT_CB_VALID (1)
#define QMI_SAP_OPTIONS_DISCONNECT_CB_VALID (2)
#define QMI_SAP_OPTIONS_INIT(opt) (opt).options_set = 0
#define QMI_SAP_OPTIONS_SET_CONNECT_CB(opt,cb) \
  do { \
    (opt).conn_cb = cb; \
    (opt).options_set |= QMI_SAP_OPTIONS_CONNECT_CB_VALID; \
  } while (0)
#define QMI_SAP_OPTIONS_SET_DISCONNECT_CB(opt,cb) \
  do { \
    (opt).disconn_cb = cb; \
    (opt).options_set |= QMI_SAP_OPTIONS_DISCONNECT_CB_VALID; \
  } while (0)

/*=============================================================================
  FUNCTION qmi_sap_register
=============================================================================*/
/*!
@brief
  This function allows a client to register a particular service with QSAP

@param[in]   service_obj        Pointer to the service object of the service being 
                                registered with QSAP 
@param[out]  user_handle        Pointer to a handle the client can use to deregister 
                                from QSAP 

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
qmi_sap_error qmi_sap_register
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_client_handle       *user_handle
);

/*=============================================================================
  FUNCTION qmi_sap_deregister
=============================================================================*/
/*!
@brief
  Deregisters a service from QSAP

@param[in]   user_handle        Pointer to the user_handle provided by the 
                                qmi_sap_register function

@retval      QMI_SAP_NO_ERR     Success
*/
/*=========================================================================*/
qmi_sap_error qmi_sap_deregister
(
  qmi_sap_client_handle user_handle
);

/*=============================================================================
  FUNCTION qmi_sap_register_with_options
=============================================================================*/
qmi_sap_error qmi_sap_register_with_options
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_options             qsap_options,
  qmi_sap_client_handle       *user_handle
);

#endif /* QMI_SAP_H */
