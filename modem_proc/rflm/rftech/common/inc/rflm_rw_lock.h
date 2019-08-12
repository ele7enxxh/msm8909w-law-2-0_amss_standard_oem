/*!
  @file
  rflm_rw_lock.h

  @brief
  RFLM Read/Write Locks
  
  @detail
  Provides RFLM common API's for other RFLM modules to call:
  - Read/Write locks
    * Allow multiple threads to read but only a single thread to write
 
  @addtogroup RFLM_CMN
  @{
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/common/inc/rflm_rw_lock.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/15   ak      Implementation of new DTR lock mechanism
09/24/14   cvd     Initial Revision

===========================================================================*/

#ifndef RFLM_RW_LOCK_H
#define RFLM_RW_LOCK_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "qurt.h"

typedef struct
{
  qurt_mutex_t wr_lock;
  qurt_sem_t read_sem;
  uint32 max_num_readers;
  int32 write_tid;
  uint32 write_counter;
} rflm_rw_lock_t;

/*----------------------------------------------------------------------------*/
void rflm_rw_lock_init(rflm_rw_lock_t *rw_lock, uint32 max_num_readers);

void rflm_rw_lock_read_lock(rflm_rw_lock_t *rw_lock);

void rflm_rw_lock_read_unlock(rflm_rw_lock_t *rw_lock);

void rflm_rw_lock_write_lock(rflm_rw_lock_t *rw_lock);

void rflm_rw_lock_write_unlock(rflm_rw_lock_t *rw_lock);

void rflm_rw_lock_destroy(rflm_rw_lock_t *rw_lock);

#ifdef __cplusplus
}  // extern "C"
#endif 

#endif /* RFLM_RW_LOCK_H */
