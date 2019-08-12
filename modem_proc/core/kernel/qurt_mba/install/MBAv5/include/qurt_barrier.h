#ifndef QURT_BARRIER_H
#define QURT_BARRIER_H

/*======================================================================
                        qurt_barrier.h

GENERAL DESCRIPTION
  Prototypes of Kernel barrier  API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_barrier.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/

/*=====================================================================
 Constants and macros
======================================================================*/
#define QURT_BARRIER_SERIAL_THREAD 1
#define QURT_BARRIER_OTHER 0

#ifndef ASM
#include <qurt_mutex.h>

/*=====================================================================
Typedefs
======================================================================*/

/**
 * qurt_barrier type                                                      
 *
 * All threads will be woken up only when all threads have reached the
 * qurt_barrier_wait call
 *
 */
typedef union {
	struct {
		unsigned short threads_left;
		unsigned short count;
		unsigned int threads_total;
        unsigned int queue;
        void * dbg_cookie; /**< pointer to futex debug structure */
	};
	unsigned long long int raw;
} qurt_barrier_t;

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**
	FUNCTION				qurt_barrier_init

	DESCRIPTION 		Init a "barrier" object.  
	
	 @param barrier [IN] pointer to the qurt_barrier_t 
	 @param threads_total [IN] Total number of threads that will be calling
	                barrier_wait

	DEPENDENCIES	  Because each futex based object will have an associated
	                kernel resource, users should make sure to call destroy
	                function when this object is not used anymore	

	@return     		None
*/
/* ======================================================================*/
int qurt_barrier_init(qurt_barrier_t *barrier, unsigned int threads_total);

/*======================================================================*/
/**
	FUNCTION				qurt_barrier_destroy

	DESCRIPTION 		Destroy a "barrier" object.  
	
	 @param barrier [IN] pointer to the qurt_barrier_t 

	DEPENDENCIES	  Because each futex based object will have an associated
	                kernel resource, users should make sure to call destroy
	                function when this object is not used anymore	

	@return     		None
*/
/* ======================================================================*/
int qurt_barrier_destroy(qurt_barrier_t *barrier);

/*======================================================================*/
/**
  FUNCTION				qurt_barrier_wait

  DESCRIPTION 		Wait on a "barrier" object.  
	                All threads will be woken up only when all threads have
	                reached the qurt_barrier_wait call
	                
	 @param barrier [IN] pointer to the qurt_barrier_t 

	DEPENDENCIES 	Because each futex based object will have an associated
									kernel resource, users should make sure to call destroy
									function when this object is not used anymore 

	@return 				
 	                @return QURT_BARRIER_OTHER: if it is woken up by last caller 
	                @return QURT_BARRIER_SERIAL_THREAD: if it is the last caller 
*/
/* ======================================================================*/
int qurt_barrier_wait(qurt_barrier_t *barrier);
#endif

#endif /* QURT_BARRIER_H */

