#ifndef QURT_PIMUTEX_H
#define QURT_PIMUTEX_H 1
/*=============================================================================

                 qurt_mutex.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qurt_pimutex API  

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_pimutex.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include <qurt_futex.h>
#include <qurt_mutex.h>

/*=============================================================================
                        FUNCTIONS
=============================================================================*/
/**
 * Init a "mutex" object for recursive way.  
 *
 * Actually this function will work exactly same as qurt_mutex_init.
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_pimutex_init(qurt_mutex_t *lock);

/**
 * Destroy a "mutex" object for recursive way.  
 *
 * Actually this function will work exactly same as qurt_mutex_destroy.
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_pimutex_destroy(qurt_mutex_t *lock);

/**
 * Lock a "mutex" object in recursive priority inherited way.  
 *
 * The locker thread will block until the lock is available. 
 *
 * If holder is current thread, it will increase the internal counter and
 * return 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_pimutex_lock(qurt_mutex_t *lock);
/**
 * Unlock a "mutex" object in recursive way.  
 *
 * If curent thread is the holder and internal counter is 1, it will free the
 * mutex, and if someone is blocking on the mutex, it will wake up the highest
 * waiting thread. 
 *
 * If current thread is the holder and internal counter is greater than 1, then
 * it will decrease the internal counter 
 *
 * If current thread is not the holder, then it will crash
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return void
 *
 */
void qurt_pimutex_unlock(qurt_mutex_t *lock);

/**
 * Try to lock a "mutex" object in recursive way.  
 *
 * If the mutex is available, it will grab the mutex and return 0
 *
 * If the mutex is hold by current thread, it will increase the internal
 * counter and  return 0
 *
 * If not, it will return non-zero 
 *
 * @param  lock  pointer to the qurt_mutex_t 
 * @return 0 if success 
 * @return non-zero if fail 
 *
 */
int qurt_pimutex_try_lock(qurt_mutex_t *lock);


#endif /* QURT_PIMUTEX_H */
