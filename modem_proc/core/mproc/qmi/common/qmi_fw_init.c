/******************************************************************************
  @file    qmi_fw_init.c
  @brief   QMI framework init

  DESCRIPTION
  QMI framework init module
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_csi_target.h"
#include "qmi_csi_common.h"
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_idl_lib_internal.h"
#include "qmi_si.h"
#include "common_v01.h"
#include "qmi_common.h"

extern qmi_csi_xport_ops_type qcsi_ipc_ops;
extern qmi_cci_xport_ops_type qcci_ipc_ops;

extern void qmi_ping_service_start(void);
extern void qmi_ping_client_start(void);
extern void qmi_sap_service_start(void);
extern void qmi_sap_pre_init(void);
extern void qmi_si_init(void);
extern void qmi_mb_fw_init(void);

#ifdef FEATURE_OPTIMIZED_FTM_CALIBRATION
extern uint32 mcs_get_ftm_mode(void);
#endif

static int qmi_fw_inited = 0;

#if defined(FEATURE_QCCI_QMUX_XPORT) || defined(FEATURE_QCCI_QBI_QMUX)
static int qmi_qmux_fw_inited = 0;
extern qmi_cci_xport_ops_type qmuxd_ops;
extern qmi_client_error_type qmi_cci_xport_qmuxd_init(void);
#endif 

#define QMI_CSI_GET_MSGS_ID 0x1E
#define QMI_CSI_GET_FIELDS_ID 0x1F
static qmi_csi_error handle_qmi_si_get_msgs
(
  qmi_idl_service_object_type service_obj,
  uint32_t instance_id, 
  uint16_t msg_id,
  qmi_req_handle req_handle,
  void *req_c_struct,
  unsigned int req_c_struct_len
);

static qmi_csi_error handle_qmi_si_get_fields
(
  qmi_idl_service_object_type service_obj,
  uint32_t instance_id, 
  uint16_t msg_id,
  qmi_req_handle req_handle,
  void *req_c_struct,
  unsigned int req_c_struct_len
);

void qmi_fw_init(void)
{
  if(qmi_fw_inited)
    return;
  qmi_fw_inited = 1;

  qmi_si_init();

  qmi_mb_fw_init();

  qmi_cci_common_init();

  /* IPC Router transport with QCSI & QCCI */
  qmi_csi_xport_start(&qcsi_ipc_ops, NULL);
  qmi_cci_xport_start(&qcci_ipc_ops, NULL);

#ifdef QMI_GET_SUPPORTED_MESSAGES_ARRAY_MAX_V01
  /* Register the Automated msg handlers */
  qmi_csi_add_msg_handler(QMI_CSI_GET_MSGS_ID,&handle_qmi_si_get_msgs);
  qmi_csi_add_msg_handler(QMI_CSI_GET_FIELDS_ID,&handle_qmi_si_get_fields);
#endif

  #ifdef FEATURE_QMI_SAP
      qmi_sap_pre_init();
  #endif
}

void qmi_fw_service_init(void)
{
  #ifdef FEATURE_OPTIMIZED_FTM_CALIBRATION
  if( mcs_get_ftm_mode() != 1 )
  #endif
  {
    /* Start ping service */
    qmi_ping_service_start();    
    #if defined(FEATURE_QMI_SAP) && defined(FEATURE_QSAP_REMOTE_REG_SERVICE)
      qmi_sap_service_start();
    #endif
    #ifdef FEATURE_QMI_PING_CLIENT
      qmi_ping_client_start();
    #endif
  }

}

#ifdef FEATURE_QCCI_QMUX_XPORT
void qmi_qmux_fw_init(void)
{
   qmi_client_error_type rc;
  
  if(qmi_qmux_fw_inited)
    return;
  qmi_qmux_fw_inited = 1;

  /* Open port QMI_CONN_ID_RMNET_0, which is port 0 */
  rc = qmi_cci_xport_qmuxd_init();
  if (rc == QMI_NO_ERR)
	  qmi_cci_xport_start(&qmuxd_ops, (void *)(uintptr_t)0);

}
#elif defined(FEATURE_QCCI_QBI_QMUX)
qmi_client_error_type qmi_cci_qmux_xport_unregister(unsigned int instance)
{
  /* Do nothing */
  return QMI_NO_ERR;
}

qmi_client_error_type qmi_cci_qmux_xport_register(unsigned int instance)
{
  unsigned int conn_id = INSTANCE_TO_QMUX_CONN(instance);
  qmi_client_error_type rc = QMI_NO_ERR;

  if(qmi_qmux_fw_inited == 0)
  {
    rc = qmi_cci_xport_qmuxd_init();
    if(rc == QMI_NO_ERR)
    {
      qmi_qmux_fw_inited = 1;
    }
  }

  if(rc == QMI_NO_ERR)
  {
    qmi_cci_xport_start(&qmuxd_ops, (void *)(uintptr_t)conn_id);
  }
  return rc;
}
#endif

#ifdef QMI_GET_SUPPORTED_MESSAGES_ARRAY_MAX_V01
static qmi_csi_error handle_qmi_si_get_msgs
(
  qmi_idl_service_object_type service_obj,
  uint32_t instance_id, 
  uint16_t msg_id,
  qmi_req_handle req_handle,
  void *req_c_struct,
  unsigned int req_c_struct_len
)
{
  qmi_get_supported_msgs_resp_v01 *qmi_si_msg_resp;
  qmi_si_list_entry_type *qmi_si_list = qmi_si_get_service_obj_impl(service_obj, instance_id);
  if (!qmi_si_list)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  qmi_si_msg_resp = MALLOC(sizeof(qmi_get_supported_msgs_resp_v01));
  if (!qmi_si_msg_resp)
  {
    qmi_si_free_list(qmi_si_list);
    return QMI_CSI_NO_MEM;
  }
  if (qmi_si_get_supported_msgs_resp(qmi_si_list->si_tbl,qmi_si_msg_resp) != QMI_SI_SUCCESS)
  {
    memset(qmi_si_msg_resp,0,sizeof(qmi_get_supported_msgs_resp_v01));
    qmi_si_msg_resp->resp.error = QMI_ERR_INTERNAL_V01;
    qmi_si_msg_resp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  qmi_csi_send_resp(req_handle,msg_id,(void*)qmi_si_msg_resp,sizeof(qmi_get_supported_msgs_resp_v01));
  FREE(qmi_si_msg_resp);
  qmi_si_free_list(qmi_si_list);
  return QMI_CSI_NO_ERR;
}

static qmi_csi_error handle_qmi_si_get_fields
(
  qmi_idl_service_object_type service_obj,
  uint32_t instance_id, 
  uint16_t msg_id,
  qmi_req_handle req_handle,
  void *req_c_struct,
  unsigned int req_c_struct_len
  )
{
  qmi_get_supported_fields_resp_v01 *qmi_si_field_resp;
  qmi_si_list_entry_type *qmi_si_list; 
  if (!req_c_struct)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  qmi_si_list = qmi_si_get_service_obj_impl(service_obj, instance_id);
  if (!qmi_si_list)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  qmi_si_field_resp = MALLOC(sizeof(qmi_get_supported_fields_resp_v01));
  if (!qmi_si_field_resp)
  {
    qmi_si_free_list(qmi_si_list);
    return QMI_CSI_NO_MEM;
  }
  if (qmi_si_get_supported_fields_resp(qmi_si_list->si_tbl,
                                       ((qmi_get_supported_fields_req_v01*)req_c_struct)->msg_id,
                                       qmi_si_field_resp) != QMI_SI_SUCCESS)
  {
    memset(qmi_si_field_resp,0,sizeof(qmi_get_supported_fields_resp_v01));
    qmi_si_field_resp->resp.error = QMI_ERR_INTERNAL_V01;
    qmi_si_field_resp->resp.result = QMI_RESULT_FAILURE_V01;
  }
  qmi_csi_send_resp(req_handle,msg_id,(void*)qmi_si_field_resp,sizeof(qmi_get_supported_fields_resp_v01));
  FREE(qmi_si_field_resp);
  qmi_si_free_list(qmi_si_list);
  return QMI_CSI_NO_ERR;
}
#endif

