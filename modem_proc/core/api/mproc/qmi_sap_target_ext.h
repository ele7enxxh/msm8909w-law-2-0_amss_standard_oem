#ifndef QMI_SAP_TARGET_EXT_H
#define QMI_SAP_TARGET_EXT_H
/******************************************************************************
  @file    qmi_sap_target_ext.h
  @brief   The QMI Service Access Proxy (SAP) target header file

  DESCRIPTION
  QMI Service Access Proxy OS Specific routines.  

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rex.h"

typedef rex_tcb_type  qsap_os_task_type;
typedef rex_sigs_type qsap_os_sig_type;

#define QSAP_MALLOC(size)      malloc(size)
#define QSAP_CALLOC(num, size) calloc(num, size)
#define QSAP_FREE(ptr)         free(ptr)
#define QSAP_REALLOC(ptr,size) realloc(ptr, size)

#endif /* QMI_SAP_TARGET_EXT_H */
