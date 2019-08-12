/*!
  @file
  lte_rrc_mem_util.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_mem_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/10   amit    Added lte_rrc_mem_deinit_heap()
10/31/08   da      Removed prototype of lte_rrc_sib_mem_calloc().
10/09/08   da      Added API for returning the used bytes of RRC heaps.
09/05/08   da      Initial version.
===========================================================================*/

#ifndef LTE_RRC_MEM_UTIL_H
#define LTE_RRC_MEM_UTIL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
# include <stddef.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define lte_rrc_mem_priv_data_free(ptr) lte_rrc_mem_priv_data_free_func(ptr); ptr = NULL;
#define lte_rrc_mem_free(ptr) lte_rrc_mem_free_func(ptr); ptr = NULL;


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_mem_init_heap
(
  void
);

extern void lte_rrc_mem_deinit_heap
(
  void
);

extern void* lte_rrc_mem_priv_data_calloc
(
  size_t  size  /*!< size of memory to be allocated */
);

extern void lte_rrc_mem_priv_data_free_func
(
  void *ptr /*!< ptr to block to be freed */
);

extern void* lte_rrc_mem_calloc
(
  size_t  size  /*!< size of memory to be allocated */
);
extern void*  lte_rrc_mem_realloc
(
  void *ptr,  /*!< ptr block to be resized */
  size_t  size  /*!< size of memory to be allocated */
);

extern void lte_rrc_mem_free_func
(
  void *ptr /*!< ptr to block to be freed */
);

extern void* lte_rrc_sib_mem_calloc
(
  size_t size  /*!< size of memory to be allocated */
);

unsigned long lte_rrc_mem_util_get_rrc_heap_used_bytes
(
  void 
);

extern unsigned long lte_rrc_mem_util_get_rrc_small_heap_used_bytes
(
  void  
); 
 
extern unsigned long lte_rrc_mem_util_get_rrc_sib_heap_used_bytes
(
  void 
);  

extern void lte_rrc_mem_utils_set_alloc_caller_level_1
(
  void
);

extern void lte_rrc_mem_utils_unset_alloc_caller
(
  void
);
#endif /* LTE_RRC_MEM_UTIL_H */
