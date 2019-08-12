/*------------------------------------------------------------------------------
   SMEM_TOC.H  
------------------------------------------------------------------------------*/
/*!
  @file
    smem_toc.h

  @brief
    This file contains the definitions for the SMEM table of contents
*/

/*------------------------------------------------------------------------------
     Copyright  2013 Qualcomm Technologies Incorporated.
     All rights reserved.
------------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/smem_toc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/13   bt      Clarify SMEM_TOC_SIZE define.
09/24/13   bt      Split SMEM config structure and defines into smem_config.h.
04/24/13   pa      Initial version
===========================================================================*/
#ifndef SMEM_TOC_H_
#define SMEM_TOC_H_

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "smem_config.h"

/*===========================================================================
                        DEFINITIONS
===========================================================================*/

/** Value placed in ToC identifier field */
#define SMEM_TOC_IDENTIFIER      0x434F5424 /* $TOC */

/** Define the size of the Table of Contents partition.  Like all partitions,
 * it must be a 4kB multiple. */
#define SMEM_TOC_SIZE            (4 * 1024)

/*===========================================================================
                        TYPE DEFINITIONS
===========================================================================*/

/** Defines an entry in the SMEM Table of Contents */
typedef struct 
{
  /* Offset in bytes from SMEM base of the region */
  uint32 offset;

  /* Size in bytes of the region */
  uint32 size;

  /* Flags for this region */
  uint32 flags;

  /** IDs for the 2 subsystems which have access to this partition.
   ** Order does not matter. 
   ** For the entry which describes the TOC itself, these are both set to 
   ** SMEM_INVALID_HOST.
   ** Use uint16, rather than enum type, to ensure size. */
  uint16 host0;
  uint16 host1;

  /* Lowest common multiple of cacheline sizes for both endpoints. For example, 
   * if Proc0 has cacheline size of 32 and Proc1 has cacheline size of 64, 
   * this value is set to 64 */
  uint32 size_cacheline;
  
  /* Reserved fields */
  uint32 reserved[3];

  /* Sizes of sub ranges that are part of the region, but are excluded from
   * the SMEM heap. These are allocated from the end of the region starting 
   * with sizes[0]. Set to 0 when not used. */
  uint32 exclusion_sizes[SMEM_TOC_MAX_EXCLUSIONS];

} smem_toc_entry_type;

/** Defines the header of the SMEM Table of Contents */
typedef struct 
{
  /* Identifier is a constant for use in debugging and identifying this struct
   * in a binary capture. Set to SMEM_TOC_IDENTIFIER. */
  uint32 identifier;

  /* Version number */
  uint32 version;

  /* Number of entries in the table */
  uint32 num_entries;

  /* Reserved fields */
  uint32 reserved[5];

  /* Zero or more entries follow */
  smem_toc_entry_type entry[];

} smem_toc_type;

#endif /* SMEM_TOC_H_ */
