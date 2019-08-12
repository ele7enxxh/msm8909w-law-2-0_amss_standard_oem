#ifndef QMI_SAP_TARGET_H
#define QMI_SAP_TARGET_H
/******************************************************************************
  @file    qmi_sap_target.h
  @brief   The QMI Service Access Proxy (SAP) Target specific  Header file.

  DESCRIPTION
  QMI SAP Rex OS specific defines.
  
  ---------------------------------------------------------------------------
  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "msg.h"

#include "rex.h"
#include "err.h"
#include "smem_log.h"
#include "qmi_common.h"

typedef rex_crit_sect_type qmi_sap_lock_type;
#define QSAP_LOCK(ptr)        rex_enter_crit_sect(ptr)
#define QSAP_UNLOCK(ptr)      rex_leave_crit_sect(ptr)
#define QSAP_LOCK_INIT(ptr)   rex_init_crit_sect(ptr)
#define QSAP_LOCK_DEINIT(ptr)

#endif /* QMI_SAP_TARGET_H */
