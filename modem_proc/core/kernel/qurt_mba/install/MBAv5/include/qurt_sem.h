#ifndef QURT_SEM_H
#define QURT_SEM_H 
/*=============================================================================

                 qurt_sem.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of Semapore API  

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


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_sem.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file 
=============================================================================*/
#include <qurt_futex.h>

/*=============================================================================
												TYPEDEFS
=============================================================================*/

/**
 * qurt semaphore type                                                      
 */
typedef union {
	unsigned int raw[2];
	struct {
		unsigned short val;
		unsigned short n_waiting;
        unsigned int reserved;
        unsigned int queue;  
        void * dbg_cookie; //pointer to futex debug data structure
	}X;
} qurt_sem_t;

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**
 * Add to a semaphore.  
 *
 * If amt is 1 and someone is waiting, then wake up one waiting thread.
 *
 * if amt is greater than 1 and someone is waiting, then wake up all waiting
 * threads.
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @param  amt  amount to be added to the value of semaphore 
 * @return even though it's a int number, the value is meaningless 
 *
 */
int qurt_sem_add(qurt_sem_t *sem, unsigned int amt);
/**
 * "up" a semaphore.  
 *
 * add semaphore by 1
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return even though it's a int number, the value is meaningless 
 *
 */
static inline int qurt_sem_up(qurt_sem_t *sem) { return qurt_sem_add(sem,1); };
/**
 * "down" a semaphore.  
 *
 * if semaphore is greater than 0, then if will reduce the value by 1 and
 * return.  otherwise, it will go block itself. 
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return even though it's a int number, the value is meaningless 
 *
 */
int qurt_sem_down(qurt_sem_t *sem);

/**
 * try to "down" a semaphore.  
 *
 * if semaphore is greater than 0, then if will reduce the value by 1 and
 * return 0.  otherwise, it will return -1
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return 0 if success
 * @return -1 if fail 
 *
 */
int qurt_sem_try_down(qurt_sem_t *sem);
/**
 * Init a "semaphore" object.  
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return void
 *
 */
void qurt_sem_init(qurt_sem_t *sem);
/**
 * Destroy a "semaphore" object.  
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return void
 *
 */
void qurt_sem_destroy(qurt_sem_t *sem);

/**
 * Init a "semaphore" object with a value.  
 *
 * Note: Because each futex based object will have an associated kernel
 * resource, users should make sure to call destroy function when this object
 * is not used anymore 
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @param  val  16 bits value 
 * @return void
 *
 */
void qurt_sem_init_val(qurt_sem_t *sem, unsigned short val);

/**
 * Get current value of a "semaphore" object.  
 *
 * @param  sem  pointer to the qurt_sem_t 
 * @return  val  16 bits value 
 */
static inline unsigned short qurt_sem_get_val(qurt_sem_t *sem ){return sem->X.val;}

#endif /* QURT_SEM_H  */

