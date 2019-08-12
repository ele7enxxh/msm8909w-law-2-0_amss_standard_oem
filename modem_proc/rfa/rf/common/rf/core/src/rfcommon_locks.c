/*!
  @file
  rf_cdma_locks.c
 
  @details
  This file defines interfaces used to make RFA Driver Thread Safe.
 
  @addtogroup RF_COMMON_RF_LOCKS
  @{
*/

/*==============================================================================

  Copyright (c) 2011 - 2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_locks.c#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/30/12   bmg     Added non-blocking try function
10/16/12   tsr     Cleanup bringup flags  
08/13/12   tsr     Replaced Bringup feature with rumi_bup_gsm flag to facilitate Bringup
07/11/12   tsr     Changes for Dime Rumi Bring-up
05/02/12   aro     Interface to de-init critical section
07/06/11   aro     Updated Lock interfaces to pass the Lock Data
04/21/11   aro     Branched file to common location
04/21/11   aro     Changed to move LOCK implementation to common
04/21/11   aro     Renamed LOCKs function to move to Common area
03/08/11   aro     Doxygen Warning Fix
03/08/11   aro     Initial version

==============================================================================*/

#include "comdef.h"
#include "rfcommon_locks.h"

/*============================================================================*/
/*!
  @name Critical Section

  @brief
  This section holds all Interfaces define critical section code.
*/
/*! @{ */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Critical Section Initialization

  @details
  Mutexes are used to allow only one thread to enter the critical section
  of code that is accessing shared data. Thus, a given data structure which
  needs mutual exclusion will need to create a mutex to go with it.
 
  @param crit_section
  Variable storing the critical section lock
 
  @return
  Flag indicating whether Initialization is successful
*/
boolean
rf_common_init_critical_section
(
  rf_lock_data_type *crit_section
)
{
  /* Init Critical Section */
  rex_init_crit_sect( crit_section );

  /* Return */
  return TRUE;
} /* rf_common_init_critical_section */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Critical Section de-Initialization

  @details
  Mutexes are used to allow only one thread to enter the critical section
  of code that is accessing shared data. Thus, a given data structure which
  needs mutual exclusion will need to create a mutex to go with it.
 
  @param crit_section
  Variable storing the critical section lock
 
  @return
  Flag indicating whether de-Initialization is successful
*/
boolean
rf_common_deinit_critical_section
(
  rf_lock_data_type *crit_section
)
{
  /* Init Critical Section */
  rex_del_crit_sect( crit_section );

  /* Return */
  return TRUE;
} /* rf_common_deinit_critical_section */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enter Critical Section

  @details
  This function will acquire lock to assign critical section for the successding 
  code. The code will be set to critical section  until the lock is released.
  Thus there should not be an attempt to re-acquire the same lock without 
  releasing it. This will result in Deadlock scenario.
 
  @param crit_section
  Variable storing the critical section lock
*/
void
rf_common_enter_critical_section
(
  rf_lock_data_type *crit_section
)
{
  /* Enter Critical Section */
  rex_enter_crit_sect( crit_section );

} /* rf_common_enter_critical_section */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Try to enter a critical section, and do not block if the critical section is
  not available.

  @details
  Non-blocking call to acquire a critical section, returning a boolean that
  indicates if the critical sectino was acquired or not.  If the critical
  section is free, it will be locked and this task has exclusive access
  to it.  If the critical section is alrady owned by another task, the
  function returns immediately without changing the status of the
  critical section.  The caller must avoid the code protected by the
  critical section if the lock was not obtained.
 
  @param crit_section
  Variable storing the critical section lock
 
  @retval TRUE
  The critical section is now locked and owned by the calling task.
 
  @retval FALSE
  The critical section was locked by another task and is not owned by the
  calling task.
*/
boolean
rf_common_try_enter_critical_section
(
  rf_lock_data_type *crit_section
)
{
  /* Enter Critical Section */
  return rex_try_enter_crit_sect( crit_section );

} /* rf_common_try_enter_critical_section */

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Disable Critical Section

  @details
  This function will release lock which was acquired for the preceecing code.
 
  @param crit_section
  Variable storing the critical section lock
*/
void
rf_common_leave_critical_section
(
  rf_lock_data_type *crit_section
)
{
  /* Enter Critical Section */
  rex_leave_crit_sect( crit_section );

} /* rf_common_leave_critical_section */

/*! @} */

/*! @} */
