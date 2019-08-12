#ifndef QURT_SIGNAL_H
#define QURT_SIGNAL_H

/**
  @file qurt_signal.h
  @brief  Prototypes of kernel signal API functions.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/


#define QURT_SIGNAL_ATTR_WAIT_ANY 0x00000000
#define QURT_SIGNAL_ATTR_WAIT_ALL 0x00000001

/*=====================================================================
 Typedefs
 ======================================================================*/

/** @addtogroup signals_types
@{ */
/** qurt_signal type                                           
 */
typedef union {
    /** @cond */
	unsigned long long int raw;
	struct {
		unsigned int signals;
		unsigned int waiting;
		unsigned int queue;
		unsigned int attribute;
	}X;
    /** @endcond */
} qurt_signal_t;
/** @} */ /* end_addtogroup signals_types */

/*=====================================================================
 Functions
======================================================================*/
 
/*======================================================================*/
/**@ingroup func_qurt_signal_init
  Initializes a signal object.
  Signal returns the initialized object.
  The signal object is initially cleared.

  @datatypes
  #qurt_signal_t

  @param[in] *signal Pointer to the initialized object.

  @return         
  None.
     
  @dependencies
  Each mutex-based object has an associated 
       kernel resource(s), therefore users should make sure to call qurt_signal_destroy()
       when this object is not used anymore 
 */
/* ======================================================================*/
void qurt_signal_init(qurt_signal_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal_destroy
  Destroys the specified signal object.
  
  @note1hang Signal objects must be destroyed when they are no longer in use. Failure 
  to do this causes resource leaks in the QuRT kernel.\n
  @note1cont Signal objects must not be destroyed while they are still in use. If this
  happens the behavior of QuRT is undefined.
  
  @datatypes
  #qurt_signal_t

  @param[in] *signal  Pointer to the signal object to destroy.

  @return
  None.

  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_signal_destroy(qurt_signal_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal_wait    
  Suspends the current thread until the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified set of signals to be set, 
  and one or more of those signals is set in the signal object, then the thread is awakened.

  If a thread is waiting on a signal object for all of the specified set of signals to be set, 
  and all of those signals are set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @datatypes
  #qurt_signal_t

  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value which identifies the individual signals in the signal object to be 
                         waited on.
  @param[in] attribute   Indicates whether the thread waits for any of the signals to be set, or for all of 
                         them to be set. Values:\n
                         - QURT_SIGNAL_ATTR_WAIT_ANY \n
                         - QURT_SIGNAL_ATTR_WAIT_ALL @tablebulletend
  @return     	
  A 32-bit word with current signals.
 
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_signal_wait(qurt_signal_t *signal, unsigned int mask, 
                unsigned int attribute);

/*======================================================================*/
/**@ingroup func_qurt_signal_wait_any
  Suspends the current thread until any of the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified set of signals to be set, 
  and one or more of those signals is set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @datatypes
  #qurt_signal_t

  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value which identifies the individual signals in the signal object to be 
                         waited on.
	
  @return     	
  A 32-bit word with current signals.
 
  @dependencies
  None.
*/
/* ======================================================================*/
static inline unsigned int qurt_signal_wait_any(qurt_signal_t *signal, unsigned int mask)
{
  return qurt_signal_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ANY);
}

/*======================================================================*/
/**@ingroup func_qurt_signal_wait_all
  Suspends the current thread until all of the specified signals are set.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for all of the specified set of signals to be set, 
  and all of those signals are set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @datatypes
  #qurt_signal_t

  @param[in] signal      Pointer to the signal object to wait on. 
  @param[in] mask        Mask value which identifies the individual signals in the signal object to be 
                         waited on.
	
  @return      	
  A 32-bit word with current signals.
 
  @dependencies
  None.
*/
/* ======================================================================*/
static inline unsigned int qurt_signal_wait_all(qurt_signal_t *signal, unsigned int mask)
{
  return qurt_signal_wait(signal, mask, QURT_SIGNAL_ATTR_WAIT_ALL);
}

/*======================================================================*/
/**@ingroup func_qurt_signal_set
  Sets signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates 
  that a signal is to be set, and 0 that it is not to be set.
  	
  @datatypes
  #qurt_signal_t

  @param[in]	signal  Pointer to the signal object to be modified.
  @param[in]	mask 	Mask value which identifies the individual signals to be set in the signal 
                        object.

  @return 
  None.
  
  @dependencies
  None.
*/
/* ======================================================================*/
void qurt_signal_set(qurt_signal_t *signal, unsigned int mask);

/*======================================================================*/
/**@ingroup func_qurt_signal_get
   Gets a signal from a signal object.
   
   Returns the current signal values of the specified signal object.

  @datatypes
  #qurt_signal_t

  @param[in] *signal Pointer to the signal object to access.

  @return         
  A 32-bit word with current signals
    
  @dependencies
  None.
*/
/* ======================================================================*/
unsigned int qurt_signal_get(qurt_signal_t *signal);

/*======================================================================*/
/**@ingroup func_qurt_signal_clear
  Clear signals in the specified signal object.

  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 
  indicates that a signal is to be cleared, and 0 that it is not to be cleared.

  @note1hang Signals must be explicitly cleared by a thread when it is awakened -- the wait 
           operations do not automatically clear them.

  @datatypes
  #qurt_signal_t

  @param[in] signal   Pointer to the signal object to modify.
  @param[in] mask     Mask value which identifies the individual signals to be cleared in the signal object.

  @return 		  
  None.

  @dependencies
  None.
 */
/* ======================================================================*/
void qurt_signal_clear(qurt_signal_t *signal, unsigned int mask);

/**@ingroup func_qurt_signal_wait_cancellable    
  Suspends the current thread until either the specified signals are set or the wait operation is cancelled.
  The operation is cancelled if the user process of the calling thread is killed, or if the calling thread must finish its current QDI invocation and return to user space. 


  Signals are represented as bits 0-31 in the 32-bit mask value. A mask bit value of 1 indicates that a signal is to be waited on, and 0 that it is not to be waited on.

  If a thread is waiting on a signal object for any of the specified set of signals to be set, and one or more of those signals is set in the signal object, then the thread is awakened.

  If a thread is waiting on a signal object for all of the specified set of signals to be set, and all of those signals are set in the signal object, then the thread is awakened.

  @note1hang At most one thread can wait on a signal object at any given time.

  @note1hang  When the operation is cancelled, the caller should assume that the signal is never going to be set.

  @datatypes
  #qurt_signal_t

  @param[in] signal      Pointer to the signal object to wait on.
  @param[in] mask        Mask value which identifies the individual signals in the signal object to be 
                         waited on.
  @param[in] attribute   Indicates whether the thread waits for any of the signals to be set, or for all of 
                         them to be set. Values:\n
                         - QURT_SIGNAL_ATTR_WAIT_ANY \n
                         - QURT_SIGNAL_ATTR_WAIT_ALL @tablebulletend
  @param[out] return_mask Pointer to the 32-bit mask value that was originally passed to the function.


  @return     	
  QURT_EOK -- Wait completed. \n
  QURT_ECANCEL -- Wait cancelled.

 
  @dependencies
  None.
*/
/* ======================================================================*/
int qurt_signal_wait_cancellable(qurt_signal_t *signal, unsigned int mask, 
                                 unsigned int attribute,
                                 unsigned int *return_mask);

#endif /* QURT_SIGNAL_H */
