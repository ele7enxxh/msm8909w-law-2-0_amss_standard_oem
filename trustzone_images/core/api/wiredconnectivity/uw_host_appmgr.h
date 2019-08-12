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


/**@file uw_host_appmgr.h

Contains the Host Application Manager declarations.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_appmgr.h#1 $
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

#ifndef _UW_HOST_APPMGR_H_
#define _UW_HOST_APPMGR_H_

#include <jtypes.h>
#include <dev_monitor.h>


/** @ingroup hsusb_jungo_api 
@{ */

/** Application registration handle. */
typedef void *uwh_app_h;

/** Application probe method. */
typedef juint_t (*uwh_app_probe_cb_t)(devmon_info_t *desc);

/** @} */ /* end_group hsusb_jungo_api */

#endif
