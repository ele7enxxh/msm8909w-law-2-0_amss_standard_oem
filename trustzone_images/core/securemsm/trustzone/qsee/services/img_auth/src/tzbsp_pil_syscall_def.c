
/**
* @file tzbsp_syscall_def.c
* @brief All library SYSCALL need to defined in one place
*
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/src/tzbsp_pil_syscall_def.c#1 $
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
#define PIL_FLAGS  TZ_SC_FLAG_SHARED_SYSCALL | \
                   TZ_SC_FLAG_USE_CRYPTO | \
                   TZ_SC_FLAG_ENABLE_INT

TZBSP_DEFINE_SYSCALL(TZ_PIL_INIT_ID,
                     PIL_FLAGS,
                     tzbsp_pil_init_image_ns,
                     2, { sizeof(int), sizeof(int) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_AUTH_RESET_ID,
                     PIL_FLAGS,
                     tzbsp_pil_auth_reset_ns,
                     1, { sizeof(int) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_MEM_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL,
                     tzbsp_pil_mem_area,
                     3, { sizeof(int), sizeof(int), sizeof(int) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_UNLOCK_XPU_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL,
                     tzbsp_pil_unlock_area, 1, { sizeof(int) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_IS_SUBSYS_SUPPORTED_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL | TZ_SC_FLAG_RSP,
                     tzbsp_pil_is_subsystem_supported,
                     1, { sizeof(uint32) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_IS_SUBSYS_MANDATED_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL | TZ_SC_FLAG_RSP,
                     tzbsp_pil_is_subsystem_mandated,
                     1, { sizeof(uint32) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_GET_MEM_AREA_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL | TZ_SC_FLAG_RSP,
                     tzbsp_pil_get_mem_area,
                     1, { sizeof(uint32) } );

TZBSP_DEFINE_SYSCALL(TZ_PIL_MSS_RESTART_ID,
                     TZ_SC_FLAG_SHARED_SYSCALL,
                     tzbsp_pil_modem_reset,
                     2, { sizeof(uint32), sizeof(uint32) });

extern const void *tz_syscall_core_start;
extern const void *tz_syscall_core_end;
