
#ifndef QURT_ALLSIGNAL_H
#define QURT_ALLSIGNAL_H

/*======================================================================
                        qurt_allsignal.h

GENERAL DESCRIPTION
  Prototypes of Kernel signal  API functions      

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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_allsignal.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
#include <qurt_futex.h>

/*=====================================================================
 Typedefs
 ======================================================================*/

/**
 * qurt_allsignal type                                                      
 *
 * The caller of signal_wait can only be woken up when all bits in mask are set 
 *
 */
typedef union {
	unsigned long long int raw;
	struct {
		unsigned int waiting;
		unsigned int signals_in;
		unsigned int queue;
		void * dbg_cookie; /**< pointer to futex debug data structure */
	}X;
} qurt_allsignal_t;


/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**
  FUNCTION        qurt_allsignal_init

  DESCRIPTION     Initialize an "allsignal" object
  
	@param *signal [IN] Pointer to the qurt_allsignal_t

  DEPENDENCIES    Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

  @return         None
     
  SIDE EFFECTS    None
 */
/* ======================================================================*/
void qurt_allsignal_init(qurt_allsignal_t *signal);

/*======================================================================*/
/**
  FUNCTION        qurt_allsignal_destroy

  DESCRIPTION     Destroy an "allsignal" object
  
	@param *signal [IN] Pointer to the qurt_allsignal_t

  DEPENDENCIES    Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

  @return         None
 
  SIDE EFFECTS    None
 */
/* ======================================================================*/
void qurt_allsignal_destroy(qurt_allsignal_t *signal);

/*======================================================================*/
/**
  FUNCTION        qurt_allsignal_get

  DESCRIPTION     Get signal from an "allsignal" object.

	@param *signal [IN] Pointer to the qurt_allsignal_t

  DEPENDENCIES    None

  @return         return A 32 bit word with current signals
    
  SIDE EFFECTS    None
*/
/* ======================================================================*/
static inline unsigned int qurt_allsignal_get(qurt_allsignal_t *signal)
{ return signal->X.signals_in; };
    
/*======================================================================*/
/**
	FUNCTION				qurt_allsignal_wait

	DESCRIPTION 		Wait on an "allsignal" object.
	                The caller of signal_wait can only be woken up when all 
	                bits in mask are set 

	@param signal [IN] pointer to the qurt_allsignal_t 
	@param mask [IN]	 mask to receive on
	
	DEPENDENCIES		None

	@return      		None
 
	SIDE EFFECTS	  None
*/
/* ======================================================================*/
void qurt_allsignal_wait(qurt_allsignal_t *signal, unsigned int mask);

/*======================================================================*/
/**
	FUNCTION				qurt_allsignal_set

	DESCRIPTION 		Signal an "allsignal" object.  
									Set the signal bits with the value in "mask" 
	
	 @param	signal [IN] pointer to the qurt_allsignal_t 
	 @param	mask [IN]	bits to set 

	DEPENDENCIES		None

	@return     		None
 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_allsignal_set(qurt_allsignal_t *signal, unsigned int mask);

#endif /* QURT_ALLSIGNAL_H */

