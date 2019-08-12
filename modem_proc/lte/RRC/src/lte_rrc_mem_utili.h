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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_mem_utili.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   aramani Initial version.
===========================================================================*/

#ifndef LTE_RRC_MEM_UTILI_H
#define LTE_RRC_MEM_UTILI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <stdio.h>
#include "memheap.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief size of RRC heap => 60K
*/
#define LTE_RRC_HEAP_MEM_BUFFER_SIZE 61440

/*! @brief size of RRC small heap => 30K
*/
#define LTE_RRC_SMALL_HEAP_MEM_BUFFER_SIZE 30720

/*! @brief size of RRC sib heap => 50K
*/
#define LTE_RRC_SIB_MEM_BUFFER_SIZE 51200

/*! @brief Threshold used to determine whether to allocate from small or big RRC heap.
*/
#define LTE_RRC_SMALL_HEAP_THRESHOLD 64

/*! @brief This structure holds the Static part of private data
*/
typedef struct
{
  uint8 placeholder;
  void * caller_ptr;
} lte_rrc_mem_util_static_data_s;

/*! @brief This structure holds the Dynamic part of private data
*/
typedef struct
{
  uint8 placeholder;

  #ifdef TEST_FRAMEWORK
  #error code not present
#endif

} lte_rrc_mem_util_dynamic_data_s;

/*! @brief Typedef variables internal to module lte_rrc_mem_util.c
*/
typedef struct
{
  /*!< Static part of private data */
  lte_rrc_mem_util_static_data_s *sd_ptr;
  /*!< Dynamic part of private data */
  lte_rrc_mem_util_dynamic_data_s *dd_ptr;
} lte_rrc_mem_util_s;

#endif /* LTE_RRC_MEM_UTILI_H */
