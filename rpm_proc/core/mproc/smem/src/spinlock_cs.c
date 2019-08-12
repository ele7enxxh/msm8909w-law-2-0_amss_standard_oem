
/*===========================================================================

               S P I N L O C K   C R I T I C A L  S E C T I O N S

===========================================================================*/
/**
  @file spinlock_cs.c
*/
/* Abstracts the OS specific implementation of Critical Section used by 
   spinlocks.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMEM_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMEM_mainpage file, or contact 
         Tech Pubs.

         The above description for this file is part of the "smem" group description 
         in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/spinlock_cs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/11   hwu     Moved assert.h and err.h to os file.
01/25/11   rs      Initial revision of DALSYS critical sections used by 
                   spinlocks.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "DALSys.h"
#include "smem_os.h"
#include "spinlock_cs.h"

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/*===========================================================================

                        LOCAL DATA DEFINITIONS

===========================================================================*/

/** The critical section structure for the spinlock */
static DALSYSSyncHandle spinlocks_cs = NULL;

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

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
void smem_spin_lock_cs_init( void )
{
  /* Initialize the critical section */
  if( DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
                                       &spinlocks_cs, 
                                       NULL) )
  {
    ERR_FATAL( "smem_spin_lock_cs_init: Critical section creation failed",
               0, 0, 0);
  }
}

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
void smem_spin_lock_cs_lock( void ) 
{
  /* Lock the non-preemtable critical section */
  DALSYS_SyncEnter( spinlocks_cs );
}

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
void smem_spin_lock_cs_unlock( void )
{
  /* Unlock the non-preemptable critical section */
  DALSYS_SyncLeave( spinlocks_cs );
}
