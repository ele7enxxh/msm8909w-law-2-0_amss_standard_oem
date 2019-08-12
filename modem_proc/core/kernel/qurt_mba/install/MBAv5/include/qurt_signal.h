#ifndef QURT_SIGNAL_H
#define QURT_SIGNAL_H

/*======================================================================
                        qurt_signal.h

GENERAL DESCRIPTION
  Prototypes of kernel signal API functions

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2012
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_signal.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when         who     what, where, why
  ----------   ---     ------------------------------------------------
  2012-03-29   sho     Creation
 ======================================================================*/

#define QURT_SIGNAL_ATTR_WAIT_ANY 0x00000000
#define QURT_SIGNAL_ATTR_WAIT_ALL 0x00000001

/*=====================================================================
 Typedefs
 ======================================================================*/

/**
 * qurt_signal type                                                      
 */
typedef union {
	unsigned long long int raw;
	struct {
		unsigned int signals;
		unsigned int waiting;
		unsigned int queue;
		unsigned int attribute;
		void * dbg_cookie; /**< pointer to futex debug data structure */
	}X;
} qurt_signal_t;


/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**
  FUNCTION        qurt_signal_init

  DESCRIPTION     Initialize an "signal" object
  
	@param *signal [IN] Pointer to the qurt_signal_t

  DEPENDENCIES    Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

  @return         None
     
  SIDE EFFECTS    None
 */
/* ======================================================================*/
void qurt_signal_init(qurt_signal_t *signal);

/*======================================================================*/
/**
  FUNCTION        qurt_signal_destroy

  DESCRIPTION     Destroy an "signal" object
  
	@param *signal [IN] Pointer to the qurt_signal_t

  DEPENDENCIES    Because each futex based object will have an associated 
                  kernel resource, users should make sure to call destroy
                  function when this object is not used anymore 

  @return         None
 
  SIDE EFFECTS    None
 */
/* ======================================================================*/
void qurt_signal_destroy(qurt_signal_t *signal);

/*======================================================================*/
/**
	FUNCTION		qurt_signal_wait

	DESCRIPTION 	Wait on an "signal" object.
                For all-wait, waiter is woken up when all bits in mask are set
                For any-wait, waiter is woken up when any bits in mask are set

	@param signal [IN]     pointer to the qurt_signal_t 
    @param mask [IN]       mask to receive on
    @param attribute [IN]  wait attribute (ex. QURT_SIGNAL_ATTR_WAIT_ANY, QURT_SIGNAL_ATTR_WAIT_ALL)
	
	DEPENDENCIES	None

	@return     	A 32 bit word with current signals
 
	SIDE EFFECTS	None
*/
/* ======================================================================*/
unsigned int qurt_signal_wait(qurt_signal_t *signal, unsigned int mask, 
                unsigned int attribute);

/*======================================================================*/
/**
	FUNCTION		qurt_signal_wait_any

	DESCRIPTION 	Wait on an "signal" object.
                    Waiter is woken up when any bits in mask are set

	@param signal [IN]     pointer to the qurt_signal_t 
    @param mask [IN]       mask to receive on
	
	DEPENDENCIES	None

	@return     	A 32 bit word with current signals
 
	SIDE EFFECTS	None
*/
/* ======================================================================*/
static inline unsigned int qurt_signal_wait_any(qurt_signal_t *signal, unsigned int mask)
{
  return qurt_signal_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ANY);
}

/*======================================================================*/
/**
	FUNCTION		qurt_signal_wait

	DESCRIPTION 	Wait on an "signal" object.
                    For all-wait, waiter is woken up when all bits in mask are set
                    For any-wait, waiter is woken up when any bits in mask are set

	@param signal [IN]     pointer to the qurt_signal_t 
    @param mask [IN]       mask to receive on
    @param attribute [IN]  wait attribute (ex. QURT_SIGNAL_ATTR_WAIT_ANY, QURT_SIGNAL_ATTR_WAIT_ALL)
	
	DEPENDENCIES	None

	@return      	A 32 bit word with current signals
 
	SIDE EFFECTS	None
*/
/* ======================================================================*/
static inline unsigned int qurt_signal_wait_all(qurt_signal_t *signal, unsigned int mask)
{
  return qurt_signal_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ALL);
}

/*======================================================================*/
/**
	FUNCTION				qurt_signal_set

	DESCRIPTION 		Signal an "signal" object.  
						Set the signal bits with the value in "mask" 
	
	 @param	signal [IN] pointer to the qurt_signal_t 
	 @param	mask [IN]	bits to set 

	DEPENDENCIES		None

	@return     		None
 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_signal_set(qurt_signal_t *signal, unsigned int mask);

/*======================================================================*/
/**
  FUNCTION        qurt_signal_get

  DESCRIPTION     Get signal from an "signal" object.

	@param *signal [IN] Pointer to the qurt_signal_t

  DEPENDENCIES    None

  @return         A 32 bit word with current signals
    
  SIDE EFFECTS    None
*/
/* ======================================================================*/
unsigned int qurt_signal_get(qurt_signal_t *signal);

/*======================================================================*/
/**
   FUNCTION	      qurt_anysignal_clear

   DESCRIPTION 	  Clear signals
	
   @param signal  [IN] Pointer to the qurt_signal_t 
   @param mask    [IN] bits to clear 

   @return 		  None
 */
/* ======================================================================*/
void qurt_signal_clear(qurt_signal_t *signal, unsigned int mask);

#endif /* QURT_SIGNAL_H */
