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


/**@file jslave_init.h

Holds the Device Stack Initalizations API.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/jslave_init.h#1 $
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

#ifndef _JSLAVE_INIT_H_
#define _JSLAVE_INIT_H_

#include <jos.h>


/** @ingroup hsusb_jungo_api 
@{ */

/** Initializes the USB stack. */
jresult_t jslave_init(uw_args_t *args);

/** Enables the stack. */
jresult_t jslave_enable(juint8_t core_number);

/** Disables the stack. */
void jslave_disable(juint8_t core_number);

/** Re-enables the stack with a different function driver configuration.
*/
typedef void (*reconfig_complete_cb_t)(void *arg, jresult_t status);
jresult_t jslave_reconfig(juint8_t core_number, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg);

/** Uninitializes the stack.\ The stack will require initialization before it 
  can be used again.
*/
void jslave_uninit(void);

/** Allows for remote wake up. */
jresult_t jslave_wakeup_host(juint8_t core_number);

/** Returns the configuration number of the device. */
juint8_t jcore_get_curr_configuration(juint8_t core_number);

/** @} */ /* end_group hsusb_jungo_api */

#endif
