#ifndef QMI_SAP_COMMON_H
#define QMI_SAP_COMMON_H
/******************************************************************************
  @file    qmi_sap_common.h
  @brief   The QMI Service Access Proxy (SAP) Common Header file.

  DESCRIPTION
  QMI SAP internal data structures
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_sap_target.h"
#include "qmi_sap_target_ext.h"
#include "qmi_idl_lib.h"
#include "qmi_common.h"
#include "qmi_client.h"
#include "qmi_sap.h"


struct qmi_sap_xport_s;
struct qmi_sap_xport_ops_s;
struct qmi_sap_service_s;
struct qmi_sap_client_s;

typedef struct qmi_sap_xport_s
{
  /* xport ops table */
  struct qmi_sap_xport_ops_s *ops;

  /* address length of xport */
  uint32_t addr_len;

  /* opaque handle returned by xport open */
  void *handle;

  /* pointer back to service */
  struct qmi_sap_service_s *service;
} qmi_sap_xport_type;

typedef struct qmi_sap_client_s
{
  /* links to prev and next client in list */
  LINK(struct qmi_sap_client_s, link);

  /* Client Handle for communicating with QCCI */
  qmi_client_type       user_handle;

  struct
  {
    /* pointer to xport */
    struct qmi_sap_xport_s *xport;
    /* Address of client - opaque storage*/
    uint8_t addr[MAX_ADDR_LEN];
  }xport;

  /* Service object, needed for indications */
  qmi_idl_service_object_type service_obj;

  /* pointer back to service */
  struct qmi_sap_service_s *service;

  /* TXN ID counter for indications */
  uint16_t next_ind_txn_id;
}qmi_sap_client_type;

typedef struct qmi_sap_service_s
{
  /* links to prev and next in service list */
  LINK(struct qmi_sap_service_s, link);

  /* unique service handle */
  qmi_sap_client_handle *handle;

  /* service registration data */
  qmi_idl_service_object_type service_obj;

  /* handle returned by qmux_reg_service */
  void *service_handle;

  /* connect callback */
  qmi_sap_client_connect_cb  conn_cb;

  /* disconnect callback */
  qmi_sap_client_disconnect_cb disconn_cb;

  /* number of successful xport connections */
  unsigned int num_xports;

  LIST(struct qmi_sap_client_s,clients);

}qmi_sap_service_type;

qmi_sap_error qmi_sap_register_internal
(
  qmi_idl_service_object_type service_obj,
  qmi_client_os_params        *os_params,
  qmi_sap_client_handle       *user_handle
);

#endif /* QMI_SAP_COMMON_H */
