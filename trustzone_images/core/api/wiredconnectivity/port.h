/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file port.h

Contains USB stack APIs to be used by the porting layer.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/25/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _PORT_H_
#define _PORT_H_

#include <compiler.h>
#include <controllers.h>
#include <os_common.h>



/** @ingroup hsusb_jungo_api 
@{ */

void os_safe_enter(void);
void os_safe_leave(void);

typedef void *os_bus_resource_h;
typedef void *os_bus_h;
typedef void *os_mutex_h;
typedef void *os_spinlock_h;
typedef void *os_timer_h;
typedef void *os_msleep_h;

typedef void (*jthread_func)(void *);

#define ISR_DONT_CALL_SOFT_INTR 0
#define ISR_CALL_SOFT_INTR 1
#define ISR_NOT_RECOGNIZED -1

typedef jint_t (*interrupt_handler)(void *);

/** Time information structure.
*/
typedef struct os_time {
    jint32_t tv_sec;     /**< Time in seconds. */
    jint32_t tv_usec;    /**< Time in microseconds. */
} os_time_t;

/** Enumeration of the controller type.
*/
typedef enum {
    CONTROLLER_HOST,     /**< &nbsp; */
    CONTROLLER_DEVICE,   /**< &nbsp; */
    CONTROLLER_OTG       /**< &nbsp; */
} controller_t;

#include <port_sync.h>
#include <port_mem.h>
#include <port_generic.h>
#include <port_bus.h>
#include "uw_args.h"

/* Usb stack APIs to be used by the porting layer */

/** Uninitializes and unloads the stack.
*/
void jstop_stack(void);

/** Registers a generic controller with the host stack.
*/
jresult_t add_generic_controller(os_bus_h bus, jint_t id, void **handle);

/** Unregister a generic controller from the host stack.
*/
jresult_t del_generic_controller(void *handle);

/** Suspends a generic controller.
*/
jresult_t generic_controller_suspend(void *handle);

/** Resumes a generic controller.
*/
jresult_t generic_controller_resume(void *handle);

/** @} */ /* end_group hsusb_jungo_api */

#endif
