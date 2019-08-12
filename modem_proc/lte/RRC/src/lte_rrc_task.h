/*!
  @file lte_rrc_task.h

  @brief
  Externalizes the RRC thread init/de-init routines.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/10   sureshs Added mem init and deinit functions
10/31/08   ask     Ported RRC to POSIX APIs, renamed to lte_rrc_task.h
07/01/08   ask     Initial Revision

===========================================================================*/

#ifndef LTE_RRC_TASK_H
#define LTE_RRC_TASK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <msgr.h>
#include <msgr_types.h>
#include <msgr_lte.h>
#include <pthread.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#ifdef __cplusplus
extern "C"
#endif /* #ifdef __cplusplus */


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

pthread_t lte_rrc_task_init
(
  int priority  /*!< thread priority passed in by app mgr */
);

void* lte_rrc_task_main
(
  void *arg             /*!< argument passed in during thread creation */  
);

void lte_rrc_task_deinit
(
  void
);

boolean lte_rrc_task_is_initialized
(
  void
);

extern boolean lte_rrc_task_tdscdma_segment_is_loaded
(
  void
);

extern boolean lte_rrc_task_wcdma_segment_is_loaded
(
  void
);

extern void lte_rrc_task_mem_init(void);

extern void lte_rrc_task_mem_deinit(void);

#endif /* LTE_RRC_TASK_H */
