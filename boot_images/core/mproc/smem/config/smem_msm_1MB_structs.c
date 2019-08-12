/*===========================================================================

          Shared Memory MSM (1MB SMEM) Device Configuration Structures

=============================================================================

  @file
    smem_msm_1MB_structs.c

    Contains structures to be used in SMEM Device Configuration.

  Copyright (c) 2013 Qualcomm Technologies Incorporated. 
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/config/smem_msm_1MB_structs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/13   bt      Enable partitions on Apps edges.
09/24/13   bt      Enabling this to be part of packed builds.
04/04/13   bt      Initial revision.  All partitions disabled (size = 0).
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
    340 * 1024,   /* Size of the partition. */
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION, /* flags */
    SMEM_APPS,    /* proc0 with RW access to the partition. */
    SMEM_MODEM,   /* proc1 with RW access to the partition. */
    32,           /* The cacheline common multiple, in Bytes. */
    0, 0, 0, 0    /* Exclusion regions' sizes within this partition. */
  },
  
  /* Apps<->ADSP partition. */
  {
    144 * 1024,
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION,
    SMEM_APPS,
    SMEM_ADSP,
    32,
    0, 0, 0, 0
  },
  
  /* Apps<->WCNSS partition. */
  {
    176 * 1024,
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION,
    SMEM_APPS,
    SMEM_WCN,
    32,
    0, 0, 0, 0
  },
  
  /* Modem<->ADSP partition. */
  {
    96 * 1024,
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION,
    SMEM_MODEM,
    SMEM_ADSP,
    32,
    0, 0, 0, 0
  },
  
  /* Modem<->WCNSS partition. */
  {
    48 * 1024,
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION,
    SMEM_MODEM,
    SMEM_WCN,
    32,
    0, 0, 0, 0
  },
  
  /* ADSP<->WCNSS partition. */
  {
    32 * 1024,
    SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION,
    SMEM_ADSP,
    SMEM_WCN,
    32,
    0, 0, 0, 0
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
