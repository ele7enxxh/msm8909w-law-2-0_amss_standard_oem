/*------------------------------------------------------------------------------
   SMEM_CONFIG.H  
------------------------------------------------------------------------------*/
/*!
  @file
    smem_config.h

  @brief
    This file contains the definitions for the SMEM config structures.  The 
    information in this header must be shippable to customers.
*/

/*------------------------------------------------------------------------------
     Copyright  2013 Qualcomm Technologies Incorporated.
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/src/smem_config.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/24/13   bt      Initial version
===========================================================================*/
#ifndef SMEM_CONFIG_H_
#define SMEM_CONFIG_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "smem_type.h"

/*===========================================================================
                        DEFINITIONS
===========================================================================*/
/** Max number of exclusions per ToC entry */
#define SMEM_TOC_MAX_EXCLUSIONS  4

/** Flag definitions for the entries */
#define SMEM_TOC_ENTRY_FLAGS_ENABLE_PROTECTION 1

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

/** The structure of the entries in DevConfig XML to configure the TOC. */
typedef struct
{
  uint32 size;
  uint32 flags;
  smem_host_type host0;
  smem_host_type host1;
  uint32 size_cacheline;
  uint32 exclusion_sizes[SMEM_TOC_MAX_EXCLUSIONS];
} smem_toc_config_type;

#endif /* SMEM_CONFIG_H_ */
