#ifndef TZBSP_VIDEO_H
#define TZBSP_VIDEO_H

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_video.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/12/12   jl      Initial version.
============================================================================*/
#include <comdef.h>

#define TZBSP_VIDEO_HW_VER_1_1_HT   ((0x1 << 28) | (0x0 << 16))
#define TZBSP_VIDEO_HW_VER_1_1_LT   ((0x1 << 28) | (0x1 << 16))
#define TZBSP_VIDEO_HW_VER_1_2_HT   ((0x1 << 28) | (0x2 << 16))
#define TZBSP_VIDEO_HW_VER_1_2_LT   ((0x1 << 28) | (0x3 << 16))

/**
 * Checks if a memory range is within the CPA range, but NOT
 * within the FW range.
 *
 * @param [in] start    Start of address range to check
 * @param [in] size     Size of address range to check
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_video_cpa_contains_memory(uint32 start, uint32 size);

/**
 * @return  Value of the HW revision register
 */
uint32 tzbsp_video_get_hw_ver(void);

#endif /* TZBSP_VIDEO_H */

