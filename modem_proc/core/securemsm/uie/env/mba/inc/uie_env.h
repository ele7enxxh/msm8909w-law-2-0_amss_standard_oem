/*===========================================================================
              Unified Image Encryption (UIE) Environment Interface

GENERAL DESCRIPTION
  Definitions and types for UIE (document 80-NP914-1 Rev A) on the MBA.

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
#ifndef _UIE_ENV_H
#define _UIE_ENV_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mba_logger.h" // MBA_LOG_MSG
#include "qurt_alloc.h" // qurt_malloc, qurt_free
#include <stringl.h>    // memscpy
#include "secboot_hw.h"
#include "mba_utils.h"  // map_memory_range

/*===========================================================================
                 DEFINITIONS AND TYPE DECLARATIONS
===========================================================================*/
#define UIE_MALLOC  qurt_malloc
#define UIE_FREE    qurt_free
#define UIE_MEMSCPY memscpy

#define UIE_LOG_MSG(fmt, ...)           MBA_LOG_MSG(fmt, ##__VA_ARGS__)
#define UIE_LOG_HEX(label, buf, buflen) MBA_LOG_HEX(label, buf, buflen)
/**
 * @brief Map the device memory into the MMU for access
 *
 * @return - 0 on success; otherwise error
 */
static inline int uie_map_fuse_area(uint32 addr, uint32 len)
{
  int retval = 0;
  unsigned int entry;
  uint32 vadd_out_param_uie = 0x0;

  retval = mba_map_memory_range(addr, len, QURT_MEM_CACHE_NONE, &entry, FALSE, &vadd_out_param_uie);

  return retval;
}

/**
 * @brief Unmap the device memory into the MMU for access
 *
 * @return - 0 on success; otherwise error
 */
static inline void uie_unmap_fuse_area(uint32 addr, uint32 len)
{
  // Not needed for MBA (only used during boot)
  return;
}

/**
 * @brief return code segment
 *
 * @return - code segment
 */
static inline uint32 uie_get_secboot_hw_code_segment(void)
{
  return SECBOOT_HW_MSS_CODE_SEGMENT;
}
#endif
