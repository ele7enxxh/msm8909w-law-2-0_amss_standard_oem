#ifndef QURT_ANYSIGNAL_H
#define QURT_ANYSIGNAL_H 
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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_anysignal.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
#include <qurt_signal.h>

/*=====================================================================
Typedefs
======================================================================*/

/**
 * qurt_anysignal type                                                      
 *
 * The caller of signal_wait can be woken up if any bits in mask is set 
 *
 */
typedef qurt_signal_t qurt_anysignal_t;

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**
  FUNCTION        qurt_anysignal_init

  DESCRIPTION     Initialize an "allsignal" object
  
	@param	signal	pointer to the qurt_anysignal_t 

  DEPENDENCIES   Because each futex based object will have an associated kernel
                  resource, users should make sure to call destroy function when this object
                  is not used anymore 

  @return         None
     
  SIDE EFFECTS    None
 */
/* ======================================================================*/
static inline void qurt_anysignal_init(qurt_anysignal_t *signal)
{
  qurt_signal_init(signal);
}

/*======================================================================*/
/**
  FUNCTION        qurt_anysignal_destroy

  DESCRIPTION     Destroy an "anysignal" object. 
  
	@param signal [IN] Pointer to the qurt_anysignal_t 

  DEPENDENCIES   Because each futex based object will have an associated kernel
                  resource, users should make sure to call destroy function when this object
                  is not used anymore 

  @return         None
 */
/* ======================================================================*/
static inline void qurt_anysignal_destroy(qurt_anysignal_t *signal)
{
  qurt_signal_destroy(signal);
}

/*======================================================================*/
/**
	FUNCTION				qurt_anysignal_wait

	DESCRIPTION 		Wait on an "anysignal" object.
                  Caller can only be woken up if any bit in "mask" is set
	
	@param signal [IN] Pointer to the qurt_allsignal_t 
	@param mask	[IN] Mask to receive on

	@return 				A 32 bit word with old signals 
 */
/* ======================================================================*/
static inline unsigned int qurt_anysignal_wait(qurt_anysignal_t *signal, unsigned int mask)
{
  return qurt_signal_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ANY);
}

/*======================================================================*/
/**
	FUNCTION				qurt_anysignal_set

	DESCRIPTION 		Set signal on an "anysignal" object
	
	@param signal [IN] Pointer to the qurt_allsignal_t 
	@param mask [IN] bits to set 

	@return 				A 32 bit word with old signals
 */
/* ======================================================================*/
unsigned int qurt_anysignal_set(qurt_anysignal_t *signal, unsigned int mask);

/*======================================================================*/
/**
	FUNCTION				qurt_anysignal_get

	DESCRIPTION 		Get signal from an "anysignal" object. 
	
	@param signal [IN] Pointer to the qurt_allsignal_t 

	@return 				A 32 bit word with current signals 
 */
/* ======================================================================*/
static inline unsigned int qurt_anysignal_get(qurt_anysignal_t *signal)
{
  return qurt_signal_get(signal);
}

/*======================================================================*/
/**
	FUNCTION				qurt_anysignal_clear

	DESCRIPTION 		Clear signals on an "anysignal" object.
	
	@param signal [IN] Pointer to the qurt_allsignal_t 
	@param mask [IN] bits to clear 

	@return 				A 32 bit word with old signals
 */
/* ======================================================================*/
unsigned int qurt_anysignal_clear(qurt_anysignal_t *signal, unsigned int mask);

#endif /* QURT_ANYSIGNAL_H */
