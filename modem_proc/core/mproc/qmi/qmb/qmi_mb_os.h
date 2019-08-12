#ifndef _QMI_MB_OS_H_
#define _QMI_MB_OS_H_
/******************************************************************************
  @file    qmi_mb_os.h
  @brief   OS Specific routines internal to QMB

  DESCRIPTION
  This header provides an OS abstraction to QMB
  This is a private header

  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "stdlib.h"
#include "stdint.h"
#include "rex.h"

#define MALLOC malloc
#define CALLOC calloc
#define FREE free

typedef rex_crit_sect_type qmi_mb_lock_type;
#define LOCK(ptr)        rex_enter_crit_sect(ptr)
#define UNLOCK(ptr)      rex_leave_crit_sect(ptr)
#define LOCK_INIT(ptr)   rex_init_crit_sect(ptr)
#define LOCK_DEINIT(ptr) rex_del_crit_sect(ptr)

#endif
