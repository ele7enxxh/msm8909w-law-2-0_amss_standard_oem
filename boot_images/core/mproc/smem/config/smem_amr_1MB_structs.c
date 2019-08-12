/*===========================================================================

            Shared Memory A+M+R Device Configuration Structures

=============================================================================

  @file
    smem_amr_1MB_structs.c

    Contains structures to be used in SMEM Device Configuration.

  Copyright (c) 2014 Qualcomm Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/config/smem_amr_1MB_structs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/14   rv      Initial revision for A+M+R(APPS, MODEM and RPM).
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_config.h"

/*===========================================================================
                        DATA DECLARATIONS
===========================================================================*/
const smem_toc_config_type smem_partition_toc_entries[] =
{
  /* Apps<->Modem partition. */
  {
    480 * 1024,   /* Size of the partition  (must be 4kB multiple). */
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION, /* flags */
    SMEM_APPS,    /* proc0 with RW access to the partition. */
    SMEM_MODEM,   /* proc1 with RW access to the partition. */
    32,           /* The cacheline common multiple, in Bytes. */
    0, 0, 0, 0    /* Exclusion regions' sizes within this partition. */
  },

  /* Last entry uses invalid hosts to signify the end. */
  {
    0,
    0,
    SMEM_INVALID_HOST,
    SMEM_INVALID_HOST,
    0,
    0, 0, 0, 0
  }
};
