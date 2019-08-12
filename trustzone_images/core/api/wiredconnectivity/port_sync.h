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


/**@file port_sync.h

Contains the Porting layer's synchronization and thread management 
functions.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port_sync.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/26/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _PORT_SYNC_H_
#define _PORT_SYNC_H_

/* HSU addition (begin) */
#include <port.h>
#include <jtypes.h>
/* HSU addition (end) */


/** @ingroup hsusb_jungo_api 
@{ */

/** Thread priorities enumeration.
The first three values must correspond to jtask_type and are for use by the 
stack's task threads. THREAD_PRIORITY_OTHER is used by all additional threads 
required for specific driver purposes.\n
The priorities are ordered from highest to lowest (0 - highest). */
typedef enum {
    THREAD_PRIORITY_CONTROLLER = 0, /**< &nbsp; */
    THREAD_PRIORITY_CORE,           /**< &nbsp; */
    THREAD_PRIORITY_DRIVER,         /**< &nbsp; */
    THREAD_PRIORITY_OTHER           /**< &nbsp; */
} os_thread_priority;

jresult_t hsu_os_thread_create(jthread_func func, void *arg, 
    os_thread_priority priority, void **handle);

juint32_t hsu_os_get_context(void);

jresult_t hsu_os_mutex_init(os_mutex_h *mtx);
void hsu_os_mutex_uninit(os_mutex_h mtx);
void hsu_os_mutex_lock(os_mutex_h mtx);
void hsu_os_mutex_unlock(os_mutex_h mtx);

jresult_t hsu_os_spinlock_init(os_spinlock_h *lock);
void hsu_os_spinlock_uninit(os_spinlock_h lock);
void hsu_os_spinlock_lock(os_spinlock_h lock);
void hsu_os_spinlock_unlock(os_spinlock_h lock);

void hsu_os_delay_us(juint32_t usec);
void hsu_os_delay_ms(juint32_t msec);

/* HSU addition : add new api, os_locked_delay_ms which makes the caller task
 * sleep for msec mili-second without releasing giant mutex. It should not be
 * called in interrupt context. Note that the function halts caller task.
 */
void hsu_os_locked_delay_ms(juint32_t msec);
/* End of HSU addition */

jresult_t hsu_os_msleep_init(os_msleep_h *msleep_h);
void  hsu_os_msleep_uninit(os_msleep_h msleep_h);
jresult_t hsu_os_msleep(os_msleep_h msleep_h, juint32_t timeout);
jresult_t hsu_os_wakeup(os_msleep_h msleep_h);

void hsu_os_get_time(os_time_t *time);
juint32_t hsu_os_get_hz(void);

/** @} */ /* end_group hsusb_jungo_api */

#endif
