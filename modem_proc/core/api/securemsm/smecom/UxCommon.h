#ifndef UXCOMMON_H
#define UXCOMMON_H

/**
   @file Uxcommon.h

   @brief
   Memory management module.

   The module defines the common memory management 
   functions. The module can enable or disable memory tracker.

   Externalized Functions:

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                               U X  C O M M O N 
                             H E A D E R  F I L E

Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/UxCommon.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $  

when         who     what, where, why
--------     ---     --------------------------------------------------------
04/20/11    nk       Removed QDSP6 3.0.04 compiler warnings
01/18/11     leo     (Tech Pubs) Edited Doxygen comments and markup.
12/13/10     nk      Doxygenated.
10/05/10     nk      Removed _UXMALLOC_DEBUG_ CMI compliance.

==========================================================================*/

#include "comdef.h"
#include "stdlib.h"
//#define __UXMALLOC_DEBUG__
#include "tracker.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup SMECOMUTIL_MEMORY 
    @{ 
*/

/**
  Type for a Ux allocator to allocate memory in the local or shared pool.
*/
typedef enum
{
UX_ALLOC_LOCAL,       /**< Local memory pool.  */ 
UX_ALLOC_SHARED,      /**< Shared memory pool. */ 
} UxAllocE;

/**
   Type that defines whether to enable or disable the debug flag.
*/
typedef enum
{
UX_MALLOC_DEBUG_DISABLE = 0,         /**< Debug memory tracker disable. */
UX_MALLOC_DEBUG_ENABLE,              /**< Debug memory tracker enable.  */
} UxMallocDebugE;

/**
   Function pointer to an IxContent acknowledgement callback.
*/
typedef void (*IxContentAckFuncType) (uint32, uint32);

/**
  Gets the status of the memory tracker (enabled or disabled).

   @return
   UX_MALLOC_DEBUG_DISABLE -- Memory tracker is disabled.\n
   UX_MALLOC_DEBUG_ENABLE  -- Memory tracker is enabled.

   @dependencies
   None.
*/
UxMallocDebugE getUxMallocDebugStat( void );

/**
   Allocates memory from a local heap.

   @param[in] size Size of the memory to be allocated.

   @return
   Returns the void pointer to the allocated memory.

   @dependencies
   None.
*/
void *UxMallocLocal (size_t size);


/**
   Reallocates memory from a heap. The choice of heap depends on the 
   Processor Domain (PD) from which this call is made and whether it is
   a shared allocation.

   @param[in,out] ptr Pointer to the reallocated memory.
   @param[in] size    Size of the memory to be allocated.

   @return
    Returns a pointer to the reallocated memory block.

   @dependencies
   None.
*/
void *UxReallocLocal (void *ptr, size_t size);

/**
   Allocates memory from a shared heap.

   @param[in] size Size of the memory to be allocated.

   @return
   Returns the void pointer to the allocated memory.

   @dependencies
   None.
*/
void *UxMallocShared (size_t size);

/**
   Debug version of UxMalloc. Outputs the file name and line number.
*/
#define UxMalloc(size, memType) \
UxMallocDebug ((size), (memType), __FILE__, __LINE__)

/**
   Debug version of UxRealloc. Outputs the file name and line number.
*/
#define UxRealloc(ptr, size) \
UxReallocDebug ((ptr), (size), __FILE__, __LINE__)

/**
   Allocates memory from a heap. The choice of heap depends on the PD from
   which this call is made from and whether it is a shared allocation. 

   @param[in] size       Size of the memory to be allocated.
   @param[in] memType    Type of memory to be allocated.
   @param[in] pFileName  Pointer to the file name.
   @param[in] lineNumber Line number information.

   @return
   Returns a pointer to the allocated memory block.

   @dependencies
   None.
*/
void *UxMallocDebug (size_t size, UxAllocE memType,
const char *pFileName, unsigned int lineNumber);

/**
   Reallocates memory from a heap. The choice of heap depends on the PD from
   which this call is made from and whether it is a shared allocation.

   @param[in] ptr        Pointer to the reallocated memory.
   @param[in] size       Size of the reallocated memory.
   @param[in] pFileName  Pointer to the file name.
   @param[in] lineNumber Line number information.

   @return
   Returns a pointer to the reallocated memory block.

   @dependencies
   None.
*/
void *UxReallocDebug (void *ptr, size_t size,
const char *pFileName, unsigned int lineNumber);

/**
   Frees memory allocated by UxMalloc or UxRealloc (or the debug versions
   of these functions).

   @param[in] ptr Pointer to the memory to be freed.

   @return
   None.

   @dependencies
   None.
*/
void UxFree (void *ptr);

/**
   Frees the allocated memory with no tracker enabled.

   @param[in] ptr Pointer to the memory to be freed.

   @return
   None.

   @dependencies
   None.
*/
void UxFreeNoTrack (void *ptr);

/**
   Safe delete.

   This macro is used for safe handling of malloced memory. The macro performs
   a safe delete operation.

   @param[in] p Pointer to the memory to be safely deleted.
*/
#define UX_SAFE_DELETE(p)     { if (NULL != p) { delete (p); (p) = NULL; } }

/**
   Safe free.

   This macro is used for safe handling of malloced memory. The macro performs
   a safe free operation.

   @param[in] p Pointer to memory to be safely freed.
*/
#define UX_SAFE_FREE(p)       { if (p) { UxFree((void*)p); (p) = 0; } }

/** @} */  /* end_addtogroup SMECOMUTIL_MEMORY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UXCOMMON_H */
