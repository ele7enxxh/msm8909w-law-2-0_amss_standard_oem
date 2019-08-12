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


/**@file port_generic.h

Contains generic functions for use by the USBWare stack.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port_generic.h#1 $
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

#ifndef _PORT_GENERIC_H_
#define _PORT_GENERIC_H_

#include "jtypes.h"


/** @ingroup hsusb_jungo_api 
@{ */

void hsu_os_printf(const char *fmt, ...);
void hsu_os_panic(const char *fmt, ...);
void hsu_os_dprintf(const char *fmt, ...);

jint_t jos_memcmp(const void *s1, const void *s2, juint32_t n);
juint32_t jos_strlcpy(char *dst, const char *src, juint32_t n);
jint32_t jos_snprintf(char *buf, juint32_t size, const char *fmt, ...);
jint_t jos_strcmp(const char *s1, const char *s2);
jint_t jos_strncmp(const char *s1, const char *s2, const juint32_t n);
void *jos_memset(void *b, jint_t c, juint32_t len);
void *jos_memcpy(void *out, const void *in, juint32_t n);
juint32_t jos_strlen(const char *str);

jresult_t hsu_os_port_init(void);
void hsu_os_port_uninit(void);

/** @} */ /* end_group hsusb_jungo_api */

#endif
