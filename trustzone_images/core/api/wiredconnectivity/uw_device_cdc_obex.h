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


/**@file uw_device_cdc_obex.h

Contains the declarations of the Communication Device Class/Object Exchange 
Communication (CDC/OBEX) function.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_device_cdc_obex.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/21/10  sw    (Tech Pubs) - edited/added Doxygen comments and markup.

==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com) 
**************************************************************************/ 


#ifndef _UW_DEVICE_CDC_OBEX_H_
#define _UW_DEVICE_CDC_OBEX_H_

#include <uw_device_cdc.h>


/** @ingroup hsusb_jungo_api 
@{ */

/* HSU addition */
typedef jresult_t (*obex_request_handler_cb_t)(cdc_appctx_t ctx, juint8_t bRequest, 
                             juint16_t wValue, juint16_t wIndex, juint16_t wLength);
/* End of HSU addition*/

/** OBEX initilalization information.
*/
typedef struct
{
    cdc_init_info_t basic;  /**< CDC initialization information.
*/

} obex_init_info_t;

/** OBEX callbacks.
*/
typedef struct
{
    cdc_callbacks_t basic;         /**< List of callbacks to the application. */
    /* HSU addition */
    obex_request_handler_cb_t request_handler_cb; /**< List of callbacks to 
                                                       handle requests. */
    /* End of HSU addition*/
} obex_callbacks_t;

jresult_t obex_init(cdc_handle_t handle, obex_callbacks_t *cbs,
    obex_init_info_t *info, cdc_appctx_t *ctx);

/** @} */ /* end_group hsusb_jungo_api */
    
#endif 
