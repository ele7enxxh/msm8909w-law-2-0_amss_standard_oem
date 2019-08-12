#ifndef __QMI_SI_OS_H_
#define __QMI_SI_OS_H_
/******************************************************************************
  @file    qmi_si_os.h
  @brief   OS Specific routines internal to QMI SI

  DESCRIPTION
  This header provides an OS abstraction to QMI SI

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "comdef.h"
#include "rex.h"
#include "stdlib.h"
#include "qmi_common_target.h"

#ifdef FEATURE_QMI_NATIVE_REX
/* stub macro on native rex */
#define rex_del_crit_sect(ptr)
#endif

typedef rex_crit_sect_type qmi_si_os_sem_type;

#define qmi_si_os_sem_init(sem) rex_init_crit_sect((sem))
#define qmi_si_os_sem_deinit(sem) rex_del_crit_sect((sem))
#define qmi_si_os_sem_lock(sem) rex_enter_crit_sect((sem))
#define qmi_si_os_sem_unlock(sem) rex_leave_crit_sect((sem))

#define qmi_si_os_malloc(ptr) malloc(ptr)
#define qmi_si_os_free(ptr) free(ptr)

#endif
