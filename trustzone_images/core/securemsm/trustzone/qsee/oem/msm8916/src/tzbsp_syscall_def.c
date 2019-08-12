/**
* @file tzbsp_syscall_def.c
* @brief All OEM syscalls must be defined (by macro) in this one file
* The function definitions can go elsewhere
*
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/oem/msm8916/src/tzbsp_syscall_def.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
12/16/2010 cap      Initial
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "tzbsp.h"
#include "tzbsp_syscall.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
/* An example system call. Uncomment to add it to the list of registered system
 * calls. */
/*
TZBSP_DEFINE_SYSCALL(TZ_OEM_TEST_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL,
                     tzbsp_oem_do_something,
                     4, { sizeof(int), sizeof(void*), sizeof(void*), sizeof(int) });
*/
/** Reference syscall section */
extern const void * tz_syscall_oem_start = &(_tz_syscall$$Base);
extern const void * tz_syscall_oem_end = &(_tz_syscall$$Limit);
