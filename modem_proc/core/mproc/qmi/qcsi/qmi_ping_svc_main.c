/******************************************************************************
  @file    qmi_ping_svc_main.c
  @brief   Rex OS implementation of the QMI Test service

  DESCRIPTION
  Rex OS implementation of the QMI test service.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "task.h"
#include "rcinit.h"
#include "rex.h"
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "test_service_v01.h"
#ifdef QMI_PING_SERVICE_SAP
#include "qmi_sap.h"
#endif

#ifdef FEATURE_QMI_NATIVE_REX
typedef unsigned char *rex_stack_pointer_type;
typedef unsigned long rex_task_func_param_type;
#endif

#define QMI_SVC_WAIT_SIG 0x00010000

#define QMI_PING_SERVICE_STACK_SIZE  (2048)
static rex_tcb_type        qmi_ping_service_tcb;
static char qmi_ping_service_stack[QMI_PING_SERVICE_STACK_SIZE];
static rex_task_attributes_type qmi_ping_service_task_attributes;

extern void *qmi_ping_register_service(qmi_csi_os_params *os_params);
static void qmi_ping_service_thread(uint32 handle)
{
  qmi_csi_os_params os_params;
  void *sp;

  os_params.tcb = rex_self();
  os_params.sig = QMI_SVC_WAIT_SIG;

  sp = qmi_ping_register_service(&os_params);

  #ifdef QMI_PING_SERVICE_SAP
  {
    qmi_sap_client_handle qsap_handle;
    qmi_sap_register(test_get_service_object_v01(), NULL, &qsap_handle);
  }
  #endif

  while(1)
  {
    rex_wait(QMI_SVC_WAIT_SIG);
    rex_clr_sigs(os_params.tcb, QMI_SVC_WAIT_SIG);
    if(sp)
    {
      qmi_csi_handle_event(sp, &os_params);
    }
  }
}

void qmi_ping_service_start(void)
{
  unsigned int prio = 10;
  char task_name[] = "QMI_PING_SVC";
  #ifdef QMI_RCINIT_CENTRALIZED_THREAD_PRIORITY
    RCINIT_INFO info_handle = NULL;

    info_handle = rcinit_lookup("QMI_PING_SVC");
    if (!info_handle) 
    {
      ERR_FATAL("QMI_PING_SVC task info not found",0 , 0, 0);
    }
    else
    {
      prio = rcinit_lookup_prio_info(info_handle);
      if (prio > 255) 
      {
        ERR_FATAL("Invalid Priority:%d",prio, 0, 0);
      }
    }
  #endif 
  rex_task_attr_init(& qmi_ping_service_task_attributes);
  rex_task_attr_set_stack_addr(&qmi_ping_service_task_attributes,
                               (rex_stack_pointer_type)qmi_ping_service_stack);
  rex_task_attr_set_stack_size(&qmi_ping_service_task_attributes,
                                QMI_PING_SERVICE_STACK_SIZE);
  rex_task_attr_set_prio(&qmi_ping_service_task_attributes, prio);
  rex_task_attr_set_entry_func(& qmi_ping_service_task_attributes,
                               qmi_ping_service_thread,
                               NULL);
  rex_task_attr_set_initially_suspended(&qmi_ping_service_task_attributes,
                                        FALSE);
  rex_task_attr_set_task_name(&qmi_ping_service_task_attributes,
                              task_name);
  rex_common_def_task(&qmi_ping_service_tcb, & qmi_ping_service_task_attributes);
}
