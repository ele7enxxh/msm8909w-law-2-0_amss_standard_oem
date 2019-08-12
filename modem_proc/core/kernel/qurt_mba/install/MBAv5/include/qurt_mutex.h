#ifndef QURT_MUTEX_H
#define QURT_MUTEX_H
/*=============================================================================

                 qurt_mutex.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qtypes API  
   This is a mostly-userspace mutex, but will call the 
   kernel to block if the mutex is taken 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_mutex.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <qurt_futex.h>

/*=============================================================================
                        TYPEDEFS
=============================================================================*/

/**
 * qurt mutex type                                                      
 *
 * Note that user can apply both non-recursive mutex lock/unlock and recursive
 * mutex lock/unlock on this type
 */
typedef struct qurt_mutex_aligned8{
    unsigned int holder; 
    unsigned int count;
    unsigned int queue;
    unsigned int wait_count;
} __attribute__ ((aligned(8))) qurt_mutex_t;

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

#define MUTEX_MAGIC 0xfe

/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**
 * Init a "mutex" object.  
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_mutex_init(qurt_mutex_t *lock);

/**
 * Destroy a "mutex" object.  
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_mutex_destroy(qurt_mutex_t *lock); 

/**
 * Lock a "mutex" object.  
 *
 * The locker thread will block until the lock is available. 
 *
 * Even if holder is current thread, it will still block, which will cause
 * deadlock.
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_mutex_lock(qurt_mutex_t *lock);		/* blocking */
/**
 * Unlock a "mutex" object.  
 *
 * It will free the mutex, and if someone is blocking on the mutex, it will
 * wake up the highest waiting thread. 
 *
 * If current thread is not the holder, then it will crash
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_mutex_unlock(qurt_mutex_t *lock);	/* unlock */

/**
 * Try to lock a "mutex" object.  
 *
 * If the mutex is available, it will grab the mutex and return 0
 * If not, it will return non-zero 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return 0 if success 
 * @return non-zero if fail 
 *
 */
int qurt_mutex_try_lock(qurt_mutex_t *lock);	

#endif /* QURT_MUTEX_H */

