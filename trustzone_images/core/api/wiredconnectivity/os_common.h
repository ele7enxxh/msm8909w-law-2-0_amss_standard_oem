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


/**@file os_common.h

Contains common functions for the USBWare stack and the porting layer.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/os_common.h#1 $
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

#ifndef _OS_COMMON_H_
#define _OS_COMMON_H_


/** @ingroup hsusb_jungo_api 
@{ */

/* 
 * Functions and prototypes common for the USBWare stack and the porting layer 
 */

#include <compiler.h>
#include <jtypes.h>

# define jsafe_enter            os_safe_enter
# define jsafe_leave            os_safe_leave
# define LOCK_CHECK             do { } while (0)
# define jmalloc               jmemp_alloc
# define jfree                 jmemp_free
void *jmemp_alloc(juint32_t, juint16_t);
void jmemp_free(void *);

/** @} */ /* end_group hsusb_jungo_api */

#endif
