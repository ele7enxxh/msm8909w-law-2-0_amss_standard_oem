#ifndef QURT_INT_H
#define QURT_INT_H
/*======================================================================
                        qurt_int.h

GENERAL DESCRIPTION
  Prototypes of Kernel interrupt API functions      

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

	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_int.h#1 $ 
	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $


when 				who 		what, where, why
---------- 	--- 		------------------------------------------------
2011-02-25 	op			Add Header file
======================================================================*/

/*=====================================================================
 Constants and macros
======================================================================*/

#define SIG_INT_ABORT 0x80000000

/*=====================================================================
 Functions
======================================================================*/

/**
	FUNCTION				qurt_interrupt_register

	DESCRIPTION 		The function enables an interrupt. The calling thread will be registered as 
								  IST (Interrupt Service Thread) of the interrupt. An IST thread can register 
								  up to 31 interrupts as limited by the number of bits (leftmost bit 31 is 
								  reserved by Qurt) in a signal variable.
								 
								  Whenever an interrupt occurs, the Qurt kernel generates the signal that is 
								  specified in the signal mask for this interrupt and delivers the signal to 
								  IST via "int_signal" variable.
								 
								  Qurt reserves some interrupts for internal use, the remainder are available 
								  for use by applications, and thus are valid interrupt numbers. If the specified 
								  interrupt number is outside the valid range, the register operation returns EINT.
								 
  NOTE 		        signal number 31 (SIG_INT_ABORT) is reserved for notifying the IST 
 					        that no interrupt is registered. This is useful when other threads 
 					        de-register all the interrupts previously registered by the IST. 
 					        Qurt kernel sends the signal to wake up the IST if it is still waiting 
 					        to receive interrupts.

	 @param int_num [IN] Interrupt number
	 @param int_signal [IN] Signal set (holds up to 31 user-defined signals: 0~30)
	 @param signal_mask	[IN] Contains the signal to be generated when registered interrupt occurs

	DEPENDENCIES 	None		

	@return         EOK Successfully registered
	@return         EINT    Invalid int_num, e.g.
                     -   >31 in case of L1 interrupt
                     -   Non-existed L2 interrupt
                     -   OS reserved
	@return         ESIG    Invalid signal mask (Can only set one bit at a time in signal_mask)
	@return         EVAL    Interrupt has been registered
*/
 unsigned int qurt_interrupt_register(int int_num, qurt_anysignal_t *int_signal, int signal_mask);

/*
 * Wait for registered interrupt signal
 *
 * Description
 *
 * Suspend the current thread until one of its registered interrupts occurs. The second input, "mask", 
 * contains the interrupt signals the IST expects to receive. The interrupt signals are registered 
 * with interrupts via qurt_register_interrupt API.
 *
 * The signals returned in "signal" variable indicate which interrupts occurred. Use function 
 * qurt_anysignal_get to read the signals. IST shall locally maintain a table that maps a signal to 
 * a specific interrupt. IST shall also check if signal SIG_INT_ABORT is received. If so, the IST 
 * shall quit from interrupt receiving loop.
 *
 * For detail information on this API, see Qurt User Manual Section 4.2.5
 *
 * Prototype
 *
 * unsigned int qurt_anysignal_wait(qurt_anysignal_t *int_signal, unsigned int mask)
 */

/**
	FUNCTION				qurt_interrupt_acknowledge

	DESCRIPTION 		Re-enable an interrupt after process it.
	                After an interrupt is taken, it will be disabled by Qurt.
	                To re-enable it, IST need acknowledge the interrupt.

	 NOTE 			    IST shall clear the interrupt signal using 
	                qurt_anysignal_clear() function before acknowledge 
	 						    the interrupt. Otherwise, re-occurring interrupt may be lost.
	 
	 						    IST shall instruct the device to clear the interrupt before
	 						    acknowledge it. Otherwise, the same interrupt could be 
	 						    re-processed.

	 @param int_num [IN] Interrupt number

	DEPENDENCIES 	None		

	@return 				EOK Successfully acknowledge the interrupt
	@return 				E_INT_DEREGISTER interrupt is already de-registered
	
*/
int qurt_interrupt_acknowledge(int int_num);

/**
	FUNCTION				qurt_interrupt_deregister

	DESCRIPTION 		Disable the IST from receiving the interrupt.
									Disable the interrupt and deregister IST as the handler
									thread of the interrupt.
									Application threads including IST call this function to
									deregister an interrupt. If deregistration happens during
									IST is waiting to receive interrupt, the IST will probably
									be stuck in waiting for the interrupts that will never occur.
									To avoid the problem, the Qurt kernel sends signal 
									SIG_INT_ABORT to wake up the IST after figuring out the IST
									has no interrupts registered.
									If the specified interrupt was not registered before, the 
									deregister function returns EINT.

	 @param int_num [IN] Interrupt number

	DEPENDENCIES 	None		

	@return 				EOK Successfully registered
  @return         EINT Invalid int_num
*/
unsigned int qurt_interrupt_deregister(int int_num);



/**
	FUNCTION				qurt_interrupt_enable

	DESCRIPTION 		The function enables an interrupt. The interrupt for the int_num needs 
                        to be registered beforehand. After calling qurt_interrupt_disable(), 
                        the corresponding interrupt cannot come to q6core anymore until qurt_interrupt_enable() 
                        is called with the same int_num.

                @param int_num [IN] Interrupt number
                @return         EOK Successfully enabled
                @return         EINT    Invalid int_num
                @return         EVAL    Interrupt has not been registered

*/
 unsigned int qurt_interrupt_enable(int int_num);


/**
	FUNCTION				qurt_interrupt_disable

	DESCRIPTION 		The function disables an interrupt. The interrupt for the int_num needs 
                        to be registered beforehand. After calling qurt_interrupt_disable(), 
                        the corresponding interrupt cannot come to q6core anymore until qurt_interrupt_enable() 
                        is called with the same int_num.

                @param int_num [IN] Interrupt number
                @return         EOK Successfully disabled
                @return         EINT    Invalid int_num
                @return         EVAL    Interrupt has not been registered

*/
 unsigned int qurt_interrupt_disable(int int_num);



/**
 * Read the pending status of the given interrupt.
 *
 * Description
 *
 * If the interrupt is pending, the returned status is 1. If the interrupt is not pending, the returned status is 0.
 *
 * If the specified interrupt was not registered before, the status reading function returns EINT.
 *
 * @param int_num [IN]  Interrupt number
 * @param status [OUT]  Interrupt status. 1: pending; 0: not pending
 *
 * @return unsigned int EOK Successfully returned
 * @return unsigned int EINT Invalid int_num
 */
unsigned int qurt_interrupt_status(int int_num, int *status);

/**
 * Clear the pending status of the given interrupt.
 *
 * Description
 *
 * If the interrupt is pending, the function will clear its pending status.
 *
 * If the specified interrupt was not registered before, the status reading function returns EINT.
 *
 * NOTE: Please don't call this function in IST (Interrupt Service Thread). Use qurt_ack_interrupt() to acknowledge & clear
 *       an interrupt after processing it. This API is created for Power Management thread to clear pending interrupt. It 
 *       could be used in other scenarios, where application doesn't use any QURT or DAL APIs to control interrupt.
 *
 * @param int_num [IN] Interrupt number
 *
 * @return unsigned int EOK Successfully returned
 * @return unsigned int EINT Invalid int_num
 */
unsigned int qurt_interrupt_clear(int int_num);

/**
 * Get registered interrupts.
 *
 * Description
 *
 * Return a bitmask indicating the registered L1 interrupts
 *
 * If an interrupt is registered, the bit is set to 1
 *
 * NOTE: Do not call this function in an IST (Interrupt Service Thread). Use qurt_ack_interrupt() to acknowledge & clear
 *       an interrupt after processing it. This API is created for a Power Management thread to get the L1 registration interrupt
 *       bitmask. It may be useful in other cases, where an application doesn't use any QURT or DAL APIs to control interrupts.
 *
 * @return unsigned int a bitmask of registered L1 interrupts
 *                      
 */
unsigned int qurt_interrupt_get_registered(void);

/**
 * Get interrupt configuration. 
 *
 * Description
 *
 * Get interrupt type and polarity of a l2vic interrupt
 *
 * NOTE: This function is only supported for v4 l2vic interrupts. The interrupt number is l2vic interrupt number 
 *       and interrupt type and polarity being configured for l2vic are returned.
 *
 * @param int_num [IN]         Interrupt number
 * @param *int_type [OUT]      Interrupt Type. 1: edge triggered; 0: level triggered
 * @param *int_polarity [OUT]  Interrupt Polarity. 1: active low; 0: active high
 *
 * @return unsigned int EOK    Successfully returned
 * @return unsigned int EINT   Invalid int_num
 */
unsigned int qurt_interrupt_get_config(unsigned int int_num, unsigned int *int_type, unsigned int *int_polarity);

/**
 * Set interrupt configuration. 
 *
 * Description
 *
 * Set interrupt type and polarity of a l2vic interrupt
 *
 * NOTE: This function is only supported for v4 l2vic interrupts. The interrupt number is l2vic interrupt number. 
 *       Race condition should be prevented at the application level.  Before calling this function to change interrupt type and polarity,
 *       the corresponding l2vic interrupt needs to be de-registered with qurt_deregister_interrupt() or qurt_deregister_fastint(),
 *
 * @param int_num        Interrupt number
 * @param int_type       Interrupt Type. 1: edge triggered; 0: level triggered
 * @param int_polarity   Interrupt Polarity. 1: active low; 0: active high
 *
 * @return unsigned int  EOK           Successfully returned
 * @return unsigned int  ENOTALLOWED   Not allow - the interrupt is being registered.
 * @return unsigned int  EINT          Invalid int_num
 */
unsigned int qurt_interrupt_set_config(unsigned int int_num, unsigned int int_type, unsigned int int_polarity);

/**
 * Raise interrupt 
 *
 * Description
 *
 * Raise interrupt with a interrupt number 
 *
 * NOTE: For QDSP6 V2/V3, this function is to raise a L1 interrupt with a interupt number ranged from 0 to 31.
 *       For QDSP6 V4, this function is to raise a level triggered L2VIC interrupt with a interupt number ranged from 0 to 1023.
 *
 * @param int_num        Interrupt number
 *
 * @return N/A
 */
void qurt_interrupt_raise(unsigned int interrupt_num);


#endif /* QURT_INT_H */

