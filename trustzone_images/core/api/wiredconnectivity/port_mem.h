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


/**@file port_mem.h

Contains the memory access and management functions for use by the USBWare 
stack.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/port_mem.h#1 $
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

#ifndef _PORT_MEM_H_
#define _PORT_MEM_H_


/** @ingroup hsusb_jungo_api 
@{ */

/** Sets the allocated memory to zero.
*/
#define M_ZERO          0x0001

/** Requests a cachable buffer.
*/
#define M_CACHABLE      0x0002

/** Specifies that the allocation must not cross page boundaries.
*/
#define M_PAGE_ALIGN    0x0004

/* HSU addition */
#define M_IGNORE_MAX_SIZE_LIMITATION 0x0010
/* End HSU addition */


void *jos_malloc(juint32_t size, juint16_t flags);
void jos_free(void *addr);

jresult_t os_dma_alloc(juint32_t size, void **vaddr, void **paddr, juint16_t flags,
    void **cookie);
void os_dma_free(void *cookie);

void hsu_os_cache_flush(void *addr, juint32_t size);
void hsu_os_cache_invalidate(void *addr, juint32_t size);

/** @} */ /* end_group hsusb_jungo_api */

#endif
