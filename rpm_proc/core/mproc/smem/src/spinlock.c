/*===========================================================================

                       Spinlock Source File


 Copyright (c) 2008, 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/spinlock.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/13   bt      RPM hack: Do not use spinlock fn_ptrs, call internal HW 
                   Mutex locks directly.
01/08/13   pa      Reinstate spinlock_ptr for use in spin_lock_internal_asm.
                   Use function pointers when calling internal 
                   spin lock/unlock/get
                   Check that this proc is lock owner when unlocking.
09/21/12   bt      Reduce RPM code size ~28 Bytes.
09/21/12   bt      Remove unused variable.
01/25/12   hwu     Remove the IMAGE_APPS_PROC.
01/18/12   bt      Remove software spinlocks on all images.
09/23/11   tl      Disable memory spinlocks on RIVA; use only hw mutexes
07/25/11   rs      Added function to force clear spinlocks owned by the 
                   specified host, required to support SSR Phase 2 
04/22/11   bt      Use new SMEM PROC defines, remove usage of IMAGE_QDSP6_PROC.
02/18/11   bt      Made smem_num_spinlocks nonstatic for Q6 software spinlocks.
02/15/11   rs      Removed spin_lock_internal_init (only requird for Q6) 
02/04/11   tl      Replaced external includes with smem_os.h
01/26/11   esh     Added sofware spinlock init.
01/25/11   rs      Moved critical section to OS specific code 
11/05/10   rs      Hold both spinlock implementations, assembly and HW,
                   to allow processors switch to HW mutexes at their own pace
09/28/10   rs      Implement spinlock using hardware mutexes 
08/23/10   rs      Switch to DALSYS critical sections 
05/22/08   sa      Initial version

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "smem_os.h"
#include "smem_target.h"
#include "spinlock.h"
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
/** The maximum number of spinlocks allocated to SMEM */
static uint32 smem_num_spinlocks  = 0;

/** This table contains the Spinlock identifiers for each
 *  of the processors that can exist in the target (indexed
 *  by the SMEM host identifiers) */
static const uint32 spinlock_ids[ SMEM_NUM_HOSTS ] =
{
  1,            /**< Apps spinlock ID */
  2,            /**< Modem spinlock ID */
  3,            /**< Q6 spinlock ID */
  4,            /**< DSPS spinlock ID */
  5,            /**< RIVA spinlock ID */
  6             /**< Q6FW spinlock ID */
};

/*===========================================================================

                    EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_spin_lock_init
===========================================================================*/
/**
  Initializes spinlocks.
 
  Spinlocks are used to acquire exclusive access to resources shared by
  all processors in the system.
 
  @param[in] spin_lock_array    Pointer to spinlocks array in SMEM.
  @param[in] int_lock_array     Pointer to intlocks array in SMEM (unused).
  @param[in] num_int_locks      Number of spinlocks.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Initializes OS specific mutual exclusion mechanisms.
*/
/*=========================================================================*/
void smem_spin_lock_init( 
    volatile uint32 *spin_lock_array,
    uint32 *int_lock_array,
    uint32 num_int_locks )
{
  /* limit the number of spinlocks used by SMEM */
  smem_num_spinlocks    = num_int_locks;

#ifndef SMEM_RPM_PROC
  /* Initialize the critical section */
  smem_spin_lock_cs_init();
#endif
}

/*===========================================================================
FUNCTION      smem_spin_lock
===========================================================================*/
/**
  Acquires a spinlock as indicated by input integer, protected by non-
  preemtable critical section (effectively locking interrupts).
 
  @param[in] lock    Spinlock identifier.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Prevents context switch (via critical section) and causes remote
  processors to spin if they try to use the same resource (via spinlock).
*/
/*=========================================================================*/
void smem_spin_lock( uint32 lock ) 
{
  /* Validate spinlock id */
  if( lock >= smem_num_spinlocks )
  {
    ERR_FATAL( "Invalid argument to spin_lock", 0, 0, 0 );
  }

  /* Lock the non-preemtable critical section */
  smem_spin_lock_cs_lock();

  /* Acquire spinlock */
  spin_lock_internal( lock );
}

/*===========================================================================
FUNCTION      smem_spin_unlock
===========================================================================*/
/**
  Releases a spin lock as indicated by input integer, protected by non-
  preemtable critical section (effectively unlocking interrupts).
 
  @param[in] lock    Spinlock identifier.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Allows other threads (via critical section) and remote processors (via
  spinlock) to acquire the shared resource.
*/
/*=========================================================================*/
void smem_spin_unlock( uint32 lock )
{
  uint32 spinlock_value;

  /* Validate spinlock id */
  if( lock >= smem_num_spinlocks )
  {
    ERR_FATAL( "Invalid argument to spin_unlock", 0, 0, 0 );
  }

  /* Read current value from spinlock and error if not equal to local proc ID */
  spinlock_value = spin_lock_get( lock );
  if( SMEM_SPINLOCK_PID != spinlock_value )
  {
    ERR_FATAL( "Unexpected mutex value %d. Expected %d.", spinlock_value, SMEM_SPINLOCK_PID, 0 );
  }

  /* Release spinlock */
  spin_unlock_internal( lock );

  /* Unlock the non-preemptable critical section */
  smem_spin_lock_cs_unlock();
}

/*===========================================================================
  FUNCTION  smem_clear_spin_locks
===========================================================================*/
/**
  This function unlocks all spinlocks held by the specified host.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] host      Host identifier.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
void smem_spin_locks_clear( smem_host_type host )
{
  uint32 i;

  /* Lock the critical section */
  smem_spin_lock_cs_lock();

  /* Clear the spinlocks hold by the specified processor */
  for( i = 0; i < smem_num_spinlocks; i++ )
  {
    /* Check if spinlock is held by the host  */
    if( spinlock_ids[ host ] == spin_lock_get( i ) )
    {
      /* Release the spinlock */
      spin_unlock_internal( i );
    }
  }

  /* Unlock the critical section */
  smem_spin_lock_cs_unlock();
}
