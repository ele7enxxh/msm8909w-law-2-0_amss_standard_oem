#ifndef HSU_COMMON_H
#define HSU_COMMON_H
/*=============================================================================

            hsu_common   ---   H E A D E R   F I L E

=============================================================================*/
/**
@file hsu_common.h

Contains common definitions,Constants and Macros for High-Speed USB.

Externalized function:
- hsu_os_is_giant_locked_by_curr_task()
*/

/*=============================================================================
Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/hsu_common.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/24/10  lo  (Tech Pubs) Edited/added Doxygen comments and markup.
05/17/06  sk  Creation

=============================================================================*/

#include "hsu_os_abstract_bus_ext.h"

/** @ingroup hsusb_qc_api
    @{
*/

/*=============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================
  Constants and Macros
=============================================================================*/
/** For use with the Jungo jresult_t type.
*/
#define HSU_JSUCCESS 0

/** For use with HSU_MSG_HIGH, HSU_MSG_MED, etc.
*/
#define HSU_MAX_MSG_LEN 80 

/** To overcome lint error 715: "Symbol 'Symbol' (Location) not referenced".
*/
#define HSU_USE_PARAM(x) (void)x

/** To overcome lint error 818: "Pointer parameter 'Symbol' (Location) could be
declared ptr to const".
*/
#define HSU_PARAM_COULD_BE_CONST(x) (void)x

/** Change a pointer to be 32 bytes aligned.
*/
#define HSU_ALIGN_PTR_TO_32_BYTE(ptr) ((byte *)((((uint32)(ptr)) + 31) & ~31))

/** Get the closest multiple of 32 (round up).
*/
#define HSU_ROUND_UP_TO_32(size) (((size) + 31) & ~31)

/** When the code is commercial, map this to (void)().
*/
#define HSU_ASSUME HSU_ASSERT

/** Macro for adding debug capabilities to os_safe_enter.
*/
#define os_safe_enter() hsu_os_safe_enter_d(__FILE__, __LINE__)

/** Macro for adding debug capabilities to os_safe_leave.
*/
#define os_safe_leave() hsu_os_safe_leave_d()

/** Internal implementation for os_safe_enter.
*/
void hsu_os_safe_enter_d(char *file, uint32 line);

/** Internal implementation for os_safe_leave.
*/
void hsu_os_safe_leave_d(void);

/*=============================================================================
FUNCTION    hsu_os_is_giant_locked_by_curr_task
=============================================================================*/
/**
Returns TRUE if the Giant mutex is currently locked by the running task.

@dependencies
None.

@sideeffects
None.

@return
TRUE  -- The Giant mutex is currently locked by the running task.\n
FALSE -- The Giant mutex is not currently locked by the running task.
*/
boolean hsu_os_is_giant_locked_by_curr_task(void);

/** @} */ /* endgroup hsusb_qc_api */

#endif /* HSU_COMMON_H */
