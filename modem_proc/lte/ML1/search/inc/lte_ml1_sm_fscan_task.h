/*!
  @file
  lte_ml1_sm_fscan_task.h

  @brief
  This file is the interface to start and stop the ML1 SM FSCAN task

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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



$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/ML1/search/inc/lte_ml1_sm_fscan_task.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/14   lec     Initial version

===========================================================================*/

#ifndef LTE_ML1_SM_FSCAN_TASK_H
#define LTE_ML1_SM_FSCAN_TASK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "lte_variation.h"
#include "lte_ml1_comdef.h"
#include <customer.h>
#include "msgr.h"
#include "msgr_lte.h"


/*===========================================================================

                    EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* ML1 SM FSCAN stack size is 8K to match MGR task
   @TODO: 4096 may be a reasonable stack size */
#define LTE_ML1_SM_FSCAN_TASK_STACK_SIZE  8192

typedef struct
{
  /* pthread fields */
  boolean        task_is_initialized;    /*!< Flag set when the task gets initialized */
  pthread_t      thread_id;              /*!< task thread id */
  pthread_attr_t attr;                   /*!< task attr */

  /* msgr fields */
  msgr_client_t  msgr_client_id;
  msgr_id_t      msgr_queue_id;

} lte_ml1_sm_fscan_task_s;

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION:  lte_ml1_sm_fscan_task_init

===========================================================================*/
/*!
  @brief
  Initializes ML1 SM FSCAN task

  @return
  True if the ML1 SM FSCAN initialized successfully
*/
/*=========================================================================*/
pthread_t lte_ml1_sm_fscan_task_init
( 
  int priority /*!< thread priority from app mgr */ 
);

#endif /* LTE_ML1_SM_FSCAN_H */
