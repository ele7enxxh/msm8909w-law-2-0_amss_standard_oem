/*****************************************************************************
***
*** TITLE
***
***  GPRS DYNAMIC MEMORY MANAGEMENT WRAPPER INTERFACE
***
***
*** DESCRIPTION
***
***  Provides an abstracted interface to the dynamic memory management
***  system and the heap allocated for inter task messaging resource.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/gprs_mem.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 21-08-13   npt     CR531633 - Remove FEATURE_DIME_MODEM dependencies 
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 09-07-30   abe     Added GS_ALLOC wrapper for FEATURE_MODEM_HEAP
*** 09-05-08   sjw     Integrate support for FEATURE_MODEM_HEAP
*** 03-09-25   tjw     Suppress Lint warning in GPRS_MEM_MALLOC
***
*****************************************************************************/

#ifndef INC_GPRS_MEM_H
#define INC_GPRS_MEM_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif


#include "modem_mem.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         GPRS_MEM_MALLOC() and GPRS_MEM_CALLOC()
===
===  DESCRIPTION
===
===    Wrappers for memory allocation functions used by GERAN components
===
===  DEPENDENCIES
===
===    GPRS_MEM_CALLOC is only available when FEATURE_MODEM_HEAP is defined
===    and will perform a zero-init of the allocated memory before returning
===
===  RETURN VALUE
===
===    Pointer to block of dynamically allocated memory from the
===    appropriate heap.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/

#define GPRS_MEM_MALLOC(siz) modem_mem_alloc((siz), MODEM_MEM_CLIENT_GERAN_CRIT)
#define GPRS_MEM_CALLOC(siz) modem_mem_calloc(1, (siz), MODEM_MEM_CLIENT_GERAN_CRIT)

/*===========================================================================
===
===  MACRO         GPRS_MEM_FREE()
===
===  DESCRIPTION
===
===    Wrapper to de-allocate memory which was previously allocated for GPRS
===    intertask messaging.
===
===  DEPENDENCIES
===
===    Memory must have been allocated using macro GPRS_MEM_MALLOC().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/

#define GPRS_MEM_FREE(ptr) modem_mem_free((ptr), MODEM_MEM_CLIENT_GERAN_CRIT)

/*===========================================================================
===
===  MACRO         GS_ALLOC()
===
===  DESCRIPTION
===
===    Wrapper to allocate memory for GPRS intertask messaging
===    for LLC , RR , L2 for NAS related messages.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    Pointer to block of dynamically allocated memory from the
===    appropriate heap.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
#define GS_ALLOC(siz)  GPRS_MEM_MALLOC((siz))

/*===========================================================================
===
===  MACRO         GS_FREE()
===
===  DESCRIPTION
===
===    Wrapper to de-allocate memory which was previously allocated for GPRS
===    intertask messaging.
===
===  DEPENDENCIES
===
===    Memory must have been allocated using macro GS_ALLOC().
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None
===
===========================================================================*/
#define GS_FREE(ptr)  GPRS_MEM_FREE((ptr))

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


#endif /* INC_GPRS_MEM_H */

/*** EOF: don't remove! ***/
