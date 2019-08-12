#ifndef QURT_COND_H
#define QURT_COND_H 
/*======================================================================
                        qurt_cond.h

GENERAL DESCRIPTION
  Prototypes of Kernel condition variable object  API functions      

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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_cond.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
#include <qurt_mutex.h>

/*=====================================================================
 Typedefs
 ======================================================================*/

/**
 * qurt condtion variable type                                                      
 */
typedef union {
	unsigned long long raw;
	struct {
		unsigned int count;
		unsigned int n_waiting;
        unsigned int queue;
        void * dbg_cookie; /** <pointer to futex debug data structure */
	}X;
} qurt_cond_t;

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**
	FUNCTION				qurt_cond_init

	DESCRIPTION 		Init a condtion variable object.
	
	  @param cond [IN] pointer to the qurt_cond_t 

  DEPENDENCIES   Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

	@return 				None
		 
	SIDE EFFECTS		None
 */
/* ======================================================================*/
void qurt_cond_init(qurt_cond_t *cond);

/*======================================================================*/
/**
	FUNCTION				qurt_cond_destroy

	DESCRIPTION 		Destroy a condtion variable object. 
	
	  @param cond [IN] pointer to the qurt_cond_t 

  DEPENDENCIES   Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

	@return 				None
		 
	SIDE EFFECTS		None
 */
/* ======================================================================*/
void qurt_cond_destroy(qurt_cond_t *cond);

/*======================================================================*/
/**
	FUNCTION				qurt_cond_signal

	DESCRIPTION 		Signal a condtion variable object.
	
	  @param cond [IN] pointer to the qurt_cond_t 

	DEPENDENCIES 	None

	@return 				None
		 
	SIDE EFFECTS		None
 */
/* ======================================================================*/
void qurt_cond_signal(qurt_cond_t *cond);

/*======================================================================*/
/**
	FUNCTION				qurt_cond_broadcast

	DESCRIPTION 		Broadcast a condtion variable object.
	
	  @param cond [IN] pointer to the qurt_cond_t 

	DEPENDENCIES 	None

	@return 				None
		 
	SIDE EFFECTS		None
 */
/* ======================================================================*/
void qurt_cond_broadcast(qurt_cond_t *cond);

/*======================================================================*/
/**
	FUNCTION				qurt_cond_wait

	DESCRIPTION 		Wait on a condtion variable object.
	
	  @param cond [IN] pointer to the qurt_cond_t 
	  @param	mutex [IN] pointer to the qurt_mutex_t. The mutex to be unlocked
	                if condition is not set  

	DEPENDENCIES 	None

	@return 				None
		 
	SIDE EFFECTS		None
 */
/* ======================================================================*/
void qurt_cond_wait(qurt_cond_t *cond, qurt_mutex_t *mutex);

#endif /* QURT_COND_H */

