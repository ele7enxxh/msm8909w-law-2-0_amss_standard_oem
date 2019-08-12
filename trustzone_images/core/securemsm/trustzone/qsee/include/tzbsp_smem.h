#ifndef TZBSP_SMEM_H
#define TZBSP_SMEM_H

/*===========================================================================
Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_smem.h#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/12   sg      Add APIs to add/remove VMIDs permissions from an xPU
09/14/10   tk      First version.
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

typedef enum
{
  TZBSP_MSS_AP_SMEM = 0,
  TZBSP_MSS_ADSP_SMEM,
  TZBSP_MSS_WCNSS_SMEM,
  TZBSP_AP_ADSP_SMEM,
  TZBSP_WCNSS_ADSP_SMEM,
  TZBSP_AP_WCNSS_SMEM,
  TZBSP_SMEM_TOC,
} tzbsp_smem_protect_id_t;

int tzbsp_protect_smem_partition(tzbsp_smem_protect_id_t id, uint32 start, uint32 end,
                                 uint32 rvmids, uint32 wvmids);
#endif
