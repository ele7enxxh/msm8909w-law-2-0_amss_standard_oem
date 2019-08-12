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


/**@file os_port.h

Contains mapping between functions for use by the USBWare stack only and 
the Real-time EXecutive (REX) OS abstraction layer.  

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/os_port.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/ 

#ifndef _OS_PORT_H_
#define _OS_PORT_H_


/** @ingroup hsusb_jungo_api 
@{ */

/* 
 * Functions and prototypes for use by the USBWare stack only 
 */

#include <os_common.h>

#define jbus_resource_h         os_bus_resource_h
#define jbus_h                  os_bus_h
#define jspinlock_h             os_spinlock_h
#define jtime_t                 os_time_t
#define jmsleep_h               os_msleep_h

#define jget_context            hsu_os_get_context

#define jspinlock_init          hsu_os_spinlock_init
#define jspinlock_uninit        hsu_os_spinlock_uninit
#define jspinlock_lock          hsu_os_spinlock_lock
#define jspinlock_unlock        hsu_os_spinlock_unlock


#define jget_time               hsu_os_get_time
#define jget_hz                 hsu_os_get_hz

#define jdprintf                hsu_os_dprintf

#define j_memcmp                jos_memcmp
#define j_strlcpy               jos_strlcpy
#define j_snprintf              jos_snprintf
#define j_strcmp                jos_strcmp
#define j_strncmp               jos_strncmp
#define j_memset                jos_memset
#define j_memcpy                jos_memcpy
#define j_strlen                jos_strlen

#define jhw_init                hsu_os_hw_init
#define jhw_uninit              hsu_os_hw_uninit

#define j_bus_alloc_resource    hsu_os_bus_alloc_resource
#define jbus_release_resource   hsu_os_bus_release_resource

#define j_bus_space_read_1      hsu_os_bus_space_read_1
#define j_bus_space_read_2      hsu_os_bus_space_read_2
#define j_bus_space_read_4      hsu_os_bus_space_read_4
#define j_bus_space_write_1     hsu_os_bus_space_write_1
#define j_bus_space_write_2     hsu_os_bus_space_write_2
#define j_bus_space_write_4     hsu_os_bus_space_write_4

#define j_power_request         hsu_os_power_request
#define j_get_port_info         hsu_os_get_port_info

#define j_bus_get_transceiver   hsu_os_bus_get_transceiver
#define j_bus_get_controller_id hsu_os_bus_get_controller_id

#define j_cache_invalidate      hsu_os_cache_invalidate
#define j_cache_flush           hsu_os_cache_flush

#define jnotify_os              hsu_os_notify
#include <port.h>

/** @} */ /* end_group hsusb_jungo_api */

#endif
