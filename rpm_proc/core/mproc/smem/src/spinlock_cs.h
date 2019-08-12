
/*---------------------------------------------------------------------------
   SPINLOCK_CS.H  - Spinlock Critical Sections
---------------------------------------------------------------------------*/
/**
  @file spinlock_cs.h

*/
/* This file contains OS independent declarations for the critical sections
   used by spinlocks. */

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMD_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMD_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smd" group description 
         in the SMD_mainpage file. 
*/

/*--------------------------------------------------------------------------
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/spinlock_cs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/11   rs      Initial revision of DALSYS critical sections used by 
                   spinlocks.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef SPINLOCK_CS_H
#define SPINLOCK_CS_H

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC VARIABLE DECLARATIONS

===========================================================================*/


/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_init
===========================================================================*/
/**
  Creates the non-preemtable critical section used to acquire the spinlock
  from different tasks.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void smem_spin_lock_cs_init( void );

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_lock
===========================================================================*/
/**
  Locks the critical section used by the spinlock.
 
  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to acquire the spinlock can be scheduled, elevating
  priority to prevent priority-inversion deadlocks.
 
  In order to avoid context switches while the spinlock is held, the task's
  priority is elevated on multi-core processors, or preemption is disabled
  entirely on single-core processors. That way the amount of time the remote
  processor spins is reduced to the smallest amount of time possible
  while the spinlock is held.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  Prevents context switch.
*/
/*==========================================================================*/
void smem_spin_lock_cs_lock( void );

/*===========================================================================
  FUNCTION  smem_spin_lock_cs_unlock
===========================================================================*/
/**
  Releases the critical section used by the spinlock.
 
  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to release the spinlock can be scheduled, allowing
  context switch once the spinlock is unlocked.
 
  @return
  None

  @dependencies 
  None.

  @sideeffects
  Allows context switch.
*/
/*==========================================================================*/
void smem_spin_lock_cs_unlock( void );

#endif /* SPINLOCK_CS_H */
