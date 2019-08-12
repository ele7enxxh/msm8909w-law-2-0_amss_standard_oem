#ifdef SNS_MEMHEAP_H
#define SNS_MEMHEAP_H
/*============================================================================
  @file sns_memheap.h

  @brief
  Defines Sensors heap interfaces

  <br><br>

  DEPENDENCIES:

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  when       who    what, where, why
  ---------- --- -----------------------------------------------------------
  2015-01-12 ps  Merge updates for Memmgr
  2014-08-21 ps  Initial version
  ============================================================================*/

/*=====================================================================
                       INCLUDES
=======================================================================*/

/*=====================================================================
                          FUNCTIONS
=======================================================================*/

sns_err_code_e sns_heap_create(void);

void sns_heap_destroy(void);

void* sns_heap_malloc(uint32_t size);

void sns_heap_free(void *ptr);

#endif /* SNS_MEMHEAP_H */
