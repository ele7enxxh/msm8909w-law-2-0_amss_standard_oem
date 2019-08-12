#ifndef QURT_FASTINT_H
#define QURT_FASTINT_H
/*======================================================================
                        qurt_cond.h

GENERAL DESCRIPTION
  Prototypes of Kernel fast interrupt API functions      

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
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_fastint.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
/*=====================================================================
 Functions
======================================================================*/

/*======================================================================*/
/**
	FUNCTION				qurt_fastint_register

	DESCRIPTION 		Register a callback function to process fast interrupt
	                Fast interrupt handlers are small, short, simple pieces of 
	                code to handle an interrupt.  Fast interrupt handlers have
	                much lower overhead than the normal interrupt handling 
	                mechanisms, but the resources available to a fast interrupt
	                handler are also greatly reduced.
                  
                  The fast interrupt handler is a function taking an integer
                  argument (the number of the interrupt that was observed), 
                  and with no return value. While a fast interrupt is being 
                  serviced, it is prevented from being serviced on another
                  hardware thread. Once the fast interrupt handler completes,
                  the interrupt can be handled again. Up to one edge-triggered
                  interrupt will pend while the interrupt handler executes,
                  subsequent interrupts may be lost.
                  It is recommended that fast interrupt handlers manage the 
                  hardware as required, and then do some operation to signal a
                  normal thread, such as adding to a semaphore or sending a message.

    @param intno [IN] the number of the interrupt that was observed  
	  @param fn [IN] fast interrupt handler. is a function taking an integer
                  argument (the number of the interrupt that was observed), 
                  and with no return value. 
    
                  
	DEPENDENCIES 	Fast interrupt handlers have the following limitations:
                 - Fast interrupt handlers can only be registered for first level interrupt
                 - Fast interrupt handlers are considered to have better than the best possible thread priority.
                 - Fast interrupt handlers always execute in Address Space 0..
                 - Fast interrupt handlers execute in supervisor mode on a kernel stack in kernel thread context. 
                 Kernel calls that require a real thread context will result in an error.  This includes kernel calls 
                 to block, so any library call that may block should be avoided in a fast interrupt handler.
                 Nested fast interrupt handling is not supported. It is recommended that fast interrupt handlers manage 
                 the hardware as required, and then do some operation to signal a normal thread, such as adding to a 
                 semaphore or sending a message.

	@return        EOK      Successfully deregistered
	  						 EINT     Invalid int_num
	  						 EINVALID The interrupt was already deregistered
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned int qurt_fastint_register(int intno, void (*fn)(int));

/*======================================================================*/
/**
	FUNCTION				qurt_fastint_deregister 

	DESCRIPTION 		Disable the interrupt and deregister fast interrupt callback function.
	
    @param intno [IN] the number of the interrupt that was observed  
	
	DEPENDENCIES 	None

	@return 				EOK       Successfully deregistered
                  EINT      Invalid int_num
                  EINVALID  The interrupt was already deregistered
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned int qurt_fastint_deregister(int intno);



/*======================================================================*/
/**
	FUNCTION				qurt_isr_register

	SDESCRIPTION    Register ISR (Interrupt Service Routine) for a certain
                    interrupt number. 

                  This API is only supported for Hexagon V4 or later.

	DEPENDENCIES  ISR interrupt handler has the same limitation as fast interrupt handler, 

      @param intno [IN] the number of the interrupt that was observed  
	  @param fn [IN] ISR (interrupt service routine) is a function taking an integer
                  argument (the number of the interrupt that was observed), 
                  and with no return value. 
    
	@return        EOK      Successfully deregistered
	  						 EINT     Invalid int_num
	  						 EINVALID The interrupt was already deregistered
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned int qurt_isr_register(int intno, void (*fn)(int));


/*======================================================================*/
/**
	FUNCTION				qurt_isr_deregister 

	DESCRIPTION 		Disable the interrupt and deregister interrupt service routine.
	
    @param intno [IN] the number of the interrupt that was observed  
	
	DEPENDENCIES 	None

	@return 				EOK       Successfully deregistered
                  EINT      Invalid int_num
                  EINVALID  The interrupt was already deregistered
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned int qurt_isr_deregister(int intno);


#endif /* QURT_FASTINT_H */
