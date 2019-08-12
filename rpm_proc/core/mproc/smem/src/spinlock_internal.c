/*===========================================================================

                    S P I N L O C K  I N T E R N A L

===========================================================================*/
/**
  @file spinlock_internal.c
*/
/* Abstracts the hardware specific implementation of Spin Locks used 
   in SMD and SMEM.
*/


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the SMEM_mainpage file. To 
         change any of the the file/group text for the PDF, edit the 
         SMEM_mainpage file, or contact Tech Pubs.

         The above description for this file is part of the "smem" group 
         description in the SMEM_mainpage file. 
*/

/*===========================================================================
Copyright (c) 2011-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/src/spinlock_internal.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/13   pa      Enable nested spinlocks.
01/14/13   pa      Switch to unsigned int's in function prototypes (uint32).
03/13/12   bt      Unfeaturize spin_lock_get (remove IMAGE_APPS_PROC).
01/25/12   bt      Updated HWIO macro and base names as of Virtio v2.0.0. 
12/19/11   bt      Initial revision for 8974, construction based on 8960.
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smem_target.h"
#include "spinlock.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/
/** Counter which indicates how many times spinlock has been locked. 
 ** Needed to enable nested locks. */
static uint32 spinlock_count[SPINLOCK_NUM_LOCKS] = {0};

/*===========================================================================
  FUNCTION  spin_lock_internal
===========================================================================*/
/**
  Acquires spinlock by writing a non-zero value to the hardware mutex.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  Locking the Mutex:
   - If Mutex is unlocked and PID is written, then the mutex is locked
   - If Mutex is locked and PID is written, then mutex register is not updated 
     and the mutex remains locked
 
  A more detailed description can be found in System Fast Peripheral Bus
  (80-VR139-1A Rev. A) documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  HW mutexes must be supported.
  Must lock a local OS mutex before calling this function.

  @sideeffects
  Locks the hardware mutex.
*/
/*==========================================================================*/
void spin_lock_internal( uint32 lock )
{
  /* Increment count.  This allows us to track how many unlocks to wait for. */
  spinlock_count[lock]++;

  /* First check if we already own the lock.  If so, just return.
   * This allows for nested locking. */
  if (SMEM_SPINLOCK_PID == spin_lock_get(lock))
  {
    return;
  }

  do
  {
    /* Attempt to write PID */
    HWIO_OUTI( TCSR_MUTEX_REG_n, lock, SMEM_SPINLOCK_PID );

    /* Check if PID was written (i.e. the hardware mutex is locked) */
  } while( HWIO_INI( TCSR_MUTEX_REG_n, lock ) != SMEM_SPINLOCK_PID );
}

/*===========================================================================
  FUNCTION  spin_unlock_internal
===========================================================================*/
/**
  Releases spinlock by writing zero to the hardware mutex.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  Unlocking the mutex:
   - If '0' is written, then mutex register is updated and the mutex is
     unlocked
 
  A more detailed description can be found in System Fast Peripheral Bus
  (80-VR139-1A Rev. A) documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  HW mutexes must be supported.
  Unlock local OS mutex after calling this function.

  @sideeffects
  Unlocks the hardware mutex.
*/
/*==========================================================================*/
void spin_unlock_internal( uint32 lock )
{
  /* If not locked, just return */
  if (0 == spinlock_count[lock])
  {
    return;
  }

  /* Decrement lock count */
  spinlock_count[lock]--;

  /* If there are additional outstanding locks, then return. */
  if (spinlock_count[lock])
{
    return;
  }

  /* Write '0' to the hardware mutex to unlock */
  HWIO_OUTI( TCSR_MUTEX_REG_n, lock, 0 );
}

/*===========================================================================
  FUNCTION  spin_lock_get
===========================================================================*/
/**
  This function retrieves the HW mutex value of the specified lock.
 
  Mutex characteristics:
   - Each Mutex has a separate read/write Mutex-register
   - Each Mutex-register can hold an integer value
   - '0' value = Mutex is free/unlocked
   - Non-zero value = Mutex is locked
 
  A more detailed description can be found in System FPB (80-VR139-1A Rev. A)
  documentation.

  @param[in] lock      Spinlock id.

  @return
  None

  @dependencies 
  None.

  @sideeffects
  None.
*/
/*==========================================================================*/
uint32 spin_lock_get( uint32 lock )
{
  return HWIO_INI( TCSR_MUTEX_REG_n, lock );
}

