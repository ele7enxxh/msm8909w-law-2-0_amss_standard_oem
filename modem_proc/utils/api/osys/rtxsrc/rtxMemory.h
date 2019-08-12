/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/**
 * @file rtxMemory.h
 * Memory management function and macro definitions.
 */
#ifndef __RTXMEMORY_H__
#define __RTXMEMORY_H__

#include "rtxsrc/rtxContext.h"

/*
 * Uncomment this definition before building the C or C++ run-time
 * libraries to enable compact memory management.  This will have a
 * smaller code footprint than the standard memory management, however,
 * the performance may not be as good.
 */
/*#define _MEMCOMPACT*/

#define RT_MH_DONTKEEPFREE 0x1
#define RT_MH_VALIDATEPTR  0x2
#define RT_MH_CHECKHEAP    0x4
#define RT_MH_TRACE        0x8
#define RT_MH_DIAG         0x10
#define RT_MH_DIAG_DEBUG   0x20
#define RT_MH_ZEROONFREE   0x40

#define OSRTMH_PROPID_DEFBLKSIZE   1
#define OSRTMH_PROPID_SETFLAGS     2
#define OSRTMH_PROPID_CLEARFLAGS   3
#define OSRTMH_PROPID_KEEPFREEUNITS 4

#define OSRTMH_PROPID_USER         10

#define OSRTXM_K_MEMBLKSIZ         (4*1024)

/**
 * @defgroup rtmem Memory Allocation Macros and Functions
 *
 * Memory allocation functions and macros handle memory management for the
 * XBinder C run-time. Special algorithms are used for allocation and
 * deallocation of memory to improve the run-time performance. @{
 */
/**
 * This macro allocates a single element of the given type.
 *
 * @param pctxt        - Pointer to a context block
 * @param type         - Data type of record to allocate
 */
#define OSRTALLOCTYPE(pctxt,type) \
(type*) rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type))

/**
 * This macro allocates and zeros a single element of the given type.
 *
 * @param pctxt        - Pointer to a context block
 * @param type         - Data type of record to allocate
 */
#define OSRTALLOCTYPEZ(pctxt,type) \
(type*) rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type))

/**
 * Reallocate an array. This macro reallocates an array (either expands or
 * contracts) to hold the given number of elements. The number of elements is
 * specified in the <i>n</i> member variable of the pseqof argument.
 *
 * @param pctxt        - Pointer to a context block
 * @param pseqof       - Pointer to a generated SEQUENCE OF array structure.
 *                       The <i>n</i> member variable must be set to the number
 *                       of records to allocate.
 * @param type         - Data type of an array record
 */
#define OSRTREALLOCARRAY(pctxt,pseqof,type) do {\
if (sizeof(type)*(pseqof)->n < (pseqof)->n) return RTERR_NOMEM; \
if (((pseqof)->elem = (type*) rtxMemHeapRealloc \
(&(pctxt)->pMemHeap, (pseqof)->elem, sizeof(type)*(pseqof)->n)) == 0) \
return RTERR_NOMEM; \
} while (0)

#ifndef _NO_MALLOC
#define OSCRTMALLOC0(nbytes)       malloc(nbytes)
#define OSCRTFREE0(ptr)            free(ptr)
#else

#ifdef _NO_THREADS
extern EXTERNRT OSCTXT g_ctxt;

#define OSCRTMALLOC0(nbytes)       rtxMemAlloc(&g_ctxt,(nbytes))
#define OSCRTFREE0(ptr)            rtxMemFreePtr(&g_ctxt,(ptr))
#else
#define OSCRTMALLOC0(nbytes)       (void*)0
#define OSCRTFREE0(ptr)            (void*)0

#endif /* _NO_THREADS */
#endif /* _NO_MALLOC */

#define OSCRTMALLOC rtxMemAlloc
#define OSCRTFREE   rtxMemFreePtr

struct OSRTDList;

#ifdef __cplusplus
extern "C" {
#endif

/* Alias for __cdecl modifier; if __cdecl keyword is not supported,
 * redefine it as empty macro. */

#if !defined(OSCDECL)
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define OSCDECL __cdecl
#else
#define OSCDECL
#endif
#endif /* OSCDECL */

EXTERNRT void  rtxMemHeapAddRef (void** ppvMemHeap);
EXTERNRT void* rtxMemHeapAlloc (void** ppvMemHeap, size_t nbytes);
EXTERNRT void* rtxMemHeapAllocZ (void** ppvMemHeap, size_t nbytes);
EXTERNRT void* rtxMemHeapSysAlloc (void** ppvMemHeap, size_t nbytes);
EXTERNRT void* rtxMemHeapSysAllocZ (void** ppvMemHeap, size_t nbytes);
EXTERNRT int   rtxMemHeapCheckPtr (void** ppvMemHeap, const void* mem_p);
EXTERNRT int   rtxMemHeapCreate (void** ppvMemHeap);

EXTERNRT int   rtxMemHeapCreateExt (void** ppvMemHeap,
                                     OSMallocFunc malloc_func,
                                     OSReallocFunc realloc_func,
                                     OSFreeFunc free_func);

EXTERNRT void  rtxMemHeapFreeAll (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapFreePtr (void** ppvMemHeap, void* mem_p);
EXTERNRT void  rtxMemHeapSysFreePtr (void** ppvMemHeap, void* mem_p);

EXTERNRT void* rtxMemHeapRealloc
(void** ppvMemHeap, void* mem_p, size_t nbytes_);

EXTERNRT void* rtxMemHeapSysRealloc
(void** ppvMemHeap, void* mem_p, size_t nbytes_);

EXTERNRT void  rtxMemHeapRelease (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapReset (void** ppvMemHeap);
EXTERNRT void  rtxMemHeapSetProperty (void** ppvMemHeap,
                                       OSUINT32 propId, void* pProp);

EXTERNRT void* rtxMemNewArray (size_t nbytes);
EXTERNRT void* rtxMemNewArrayZ (size_t nbytes);
EXTERNRT void  rtxMemDeleteArray (void* mem_p);

EXTERNRT void* rtxMemHeapAutoPtrRef (void** ppvMemHeap, void* ptr);
EXTERNRT int rtxMemHeapAutoPtrUnref (void** ppvMemHeap, void* ptr);
EXTERNRT int rtxMemHeapAutoPtrGetRefCount (void** ppvMemHeap, void* mem_p);

EXTERNRT void rtxMemHeapInvalidPtrHook (void** ppvMemHeap, const void* mem_p);

EXTERNRT void rtxMemHeapCheck (void **ppvMemHeap, const char* file, int line);
EXTERNRT void rtxMemHeapPrint (void **ppvMemHeap);

#if !defined(_ARMTCC) && !defined(__SYMBIAN32__)
/**
 * This function sets the pointers to standard allocation functions. These
 * functions are used to allocate/reallocate/free memory blocks. By
 * default, standard C functions - 'malloc', 'realloc' and 'free' - are used.
 * But if some platforms do not support these functions (or some other reasons
 * exist) they can be overloaded. The functions being overloaded should have
 * the same prototypes as the standard functions.
 *
 * @param malloc_func Pointer to the memory allocation function ('malloc' by
 *    default).
 * @param realloc_func Pointer to the memory reallocation function ('realloc'
 *    by default).
 * @param free_func Pointer to the memory deallocation function ('free' by
 *    default).
 */
EXTERNRT void rtxMemSetAllocFuncs (OSMallocFunc malloc_func,
                                   OSReallocFunc realloc_func,
                                   OSFreeFunc free_func);

#endif /* __SYMBIAN32__ */

EXTERNRT void rtxMemFreeOpenSeqExt
(OSCTXT* pctxt, struct OSRTDList *pElemList);

/**
 * This function returns the actual granularity of memory blocks in the
 * context.
 *
 * @param pctxt        Pointer to a context block.
 */
EXTERNRT OSUINT32 rtxMemHeapGetDefBlkSize (OSCTXT* pctxt);

#ifndef __SYMBIAN32__
/**
 * This function sets the minimum size and the granularity of memory blocks
 * for newly created memory heaps.
 *
 * @param blkSize      The minimum size and the granularity of
 *                       memory blocks.
 */
EXTERNRT void rtxMemSetDefBlkSize (OSUINT32 blkSize);
#endif

/**
 * This function returns the actual granularity of memory blocks.
 *
 * @return             The currently used minimum size and the granularity of
 *                       memory blocks.
 */
EXTERNRT OSUINT32 rtxMemGetDefBlkSize (void);

/**
 * This function determines if the memory heap defined in the give context
 * is empty (i.e. contains no outstanding memory allocations).
 *
 * @param pctxt        Pointer to a context block.
 * @return             Boolean true value if heap is empty.
 */
EXTERNRT OSBOOL rtxMemHeapIsEmpty (OSCTXT* pctxt);

/**
 * This helper function determines if an arbitrarily sized block of
 * memory is set to zero.
 *
 * @param pmem          Pointer to memory block to check
 * @param memsiz        Size of the memory block
 * @return              Boolean result: true if memory is all zero
 */
EXTERNRT OSBOOL rtxMemIsZero (const void* pmem, size_t memsiz);

#ifdef _STATIC_HEAP
EXTERNRT void rtxMemSetStaticBuf (void* memHeapBuf, OSUINT32 blkSize);
#endif

/**
 * Allocate memory.  This macro allocates the given number of bytes.  It is
 * similar to the C \c malloc run-time function.
 *
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemAlloc(pctxt,nbytes) \
rtxMemHeapAlloc(&(pctxt)->pMemHeap,nbytes)

/**
 * This macro makes a direct call to the configured system memory
 * allocation function.  By default, this is the C malloc function,
 * but it is possible to configure to use a custom allocation function.
 *
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemSysAlloc(pctxt,nbytes) \
rtxMemHeapSysAlloc(&(pctxt)->pMemHeap,nbytes)

/**
 * Allocate and zero memory.  This macro allocates the given number of bytes
 * and then initializes the memory block to zero.
 *
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemAllocZ(pctxt,nbytes) \
rtxMemHeapAllocZ(&(pctxt)->pMemHeap,nbytes)

/**
 * Allocate and zero memory.  This macro allocates the given number of bytes
 * and then initializes the memory block to zero.
 *
 * This macro makes a direct call to the configured system memory
 * allocation function.  By default, this is the C malloc function,
 * but it is possible to configure to use a custom allocation function.
 *
 * @param pctxt - Pointer to a context block
 * @param nbytes - Number of bytes of memory to allocate
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemSysAllocZ(pctxt,nbytes) \
rtxMemHeapSysAllocZ(&(pctxt)->pMemHeap,nbytes)


/**
 * Reallocate memory.  This macro reallocates a memory block (either
 * expands or contracts) to the given number of bytes.  It is
 * similar to the C \c realloc run-time function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to reallocate.  This must have been
 *   allocated using the rtxMemAlloc macro or the rtxMemHeapAlloc function.
 * @param nbytes - Number of bytes of memory to which the block is to be
 *   resized.
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.  This may be the same as the mem_p
 *   pointer that was passed in if the block did not need to be relocated.
 */
#define rtxMemRealloc(pctxt,mem_p,nbytes) \
rtxMemHeapRealloc(&(pctxt)->pMemHeap, (void*)mem_p, nbytes)

/**
 * This macro makes a direct call to the configured system memory
 * reallocation function to do the reallocation..  By default,
 * this is the C realloc function, but it is possible to configure to use
 * a custom reallocation function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to reallocate.  This must have been
 *   allocated using the rtxMemSysAlloc macro or the rtxMemHeapSysAlloc
 *   function.
 * @param nbytes - Number of bytes of memory to which the block is to be
 *   resized.
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.  This may be the same as the mem_p
 *   pointer that was passed in if the block did not need to be relocated.
 */
#define rtxMemSysRealloc(pctxt,mem_p,nbytes) \
   rtxMemHeapSysRealloc(&(pctxt)->pMemHeap,(void*)mem_p,nbytes)

/**
 * Free memory pointer.  This macro frees memory at the given pointer.
 * The memory must have been allocated using the rtxMemAlloc (or similar)
 * macros or the rtxMem memory allocation macros.  This macro is
 * similar to the C \c free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemAlloc macro or the
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreePtr(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * This macro makes a direct call to the configured system memory
 * free function.  By default, this is the C free function, but it is
 * possible to configure to use a custom free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemSysAlloc macro or the
 *   rtxMemHeapSysAlloc function.
 */
#define rtxMemSysFreePtr(pctxt,mem_p) \
rtxMemHeapSysFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Free memory associated with a context.  This macro frees all memory
 * held within a context.  This is all memory allocated using the
 * rtxMemAlloc (and similar macros) and the rtxMem memory allocation
 * functions using the given context variable.
 *
 * @param pctxt - Pointer to a context block
 */
EXTERNRT void rtxMemFree (OSCTXT* pctxt);

/**
 * Reset memory associated with a context.  This macro resets all memory
 * held within a context.  This is all memory allocated using the rtxMemAlloc
 * (and similar macros) and the rtxMem memory allocation functions using the
 * given context variable.
 *
 * <p>The difference between this and the OSMEMFREE macro is that the
 * memory blocks held within the context are not actually freed.  Internal
 * pointers are reset so the existing blocks can be reused.  This can
 * provide a performace improvement for repetitive tasks such as decoding
 * messages in a loop.
 *
 * @param pctxt - Pointer to a context block
 */
EXTERNRT void rtxMemReset (OSCTXT* pctxt);

/**
 * Allocate type.  This macro allocates memory to hold a variable of the
 * given type.
 *
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef
 * @return - Pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemAllocType(pctxt,ctype) \
(ctype*)rtxMemHeapAlloc(&(pctxt)->pMemHeap,sizeof(ctype))

/**
 * Allocate type.  This macro allocates memory to hold a variable of the
 * given type.
 *
 * This macro makes a direct call to the configured system memory
 * allocation function.  By default, this is the C malloc function,
 * but it is possible to configure to use a custom allocation function.
 *
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef
 * @return - Pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemSysAllocType(pctxt,ctype) \
(ctype*)rtxMemHeapSysAlloc(&(pctxt)->pMemHeap,sizeof(ctype))

/**
 * Allocate type and zero memory.  This macro allocates memory to hold a
 * variable of the given type and initializes the allocated memory to zero.
 *
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef
 * @return - Pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemAllocTypeZ(pctxt,ctype) \
(ctype*)rtxMemHeapAllocZ(&(pctxt)->pMemHeap,sizeof(ctype))

/**
 * Allocate type and zero memory.  This macro allocates memory to hold a
 * variable of the given type and initializes the allocated memory to zero.
 *
 * This macro makes a direct call to the configured system memory
 * allocation function.  By default, this is the C malloc function,
 * but it is possible to configure to use a custom allocation function.
 *
 * @param pctxt - Pointer to a context block
 * @param ctype - Name of C typedef
 * @return - Pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.
 */
#define rtxMemSysAllocTypeZ(pctxt,ctype) \
(ctype*)rtxMemHeapSysAllocZ(&(pctxt)->pMemHeap,sizeof(ctype))

/**
 * Free memory pointer.  This macro frees memory at the given pointer.
 * The memory must have been allocated using the rtxMemAlloc (or similar)
 * macros or the rtxMem memory allocation macros.  This macro is
 * similar to the C \c free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemAlloc or rtxMemAlloc macro or the
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreeType(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Free memory pointer.  This macro frees memory at the given pointer.
 * The memory must have been allocated using the rtxMemSysAlloc (or similar)
 * macros or the rtxMemSys memory allocation macros.  This macro is
 * similar to the C \c free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemSysAlloc or rtxMemSysAlloc macro or the
 *   rtxMemSysHeapAlloc function.
 */
#define rtxMemSysFreeType(pctxt,mem_p) \
rtxMemHeapSysFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Allocate a dynamic array. This macro allocates a dynamic array of
 * records of the given type. The pointer to the allocated array is
 * returned to the caller.
 *
 * @param pctxt        - Pointer to a context block
 * @param n            - Number of records to allocate
 * @param type         - Data type of an array record
 */
#define rtxMemAllocArray(pctxt,n,type) \
(type*)rtxMemHeapAlloc (&(pctxt)->pMemHeap, sizeof(type)*n)

/**
 * Allocate a dynamic array. This macro allocates a dynamic array of
 * records of the given type. The pointer to the allocated array is
 * returned to the caller.
 *
 * This macro makes a direct call to the configured system memory
 * allocation function.  By default, this is the C malloc function,
 * but it is possible to configure to use a custom allocation function.
 *
 * @param pctxt        - Pointer to a context block
 * @param n            - Number of records to allocate
 * @param type         - Data type of an array record
 */
#define rtxMemSysAllocArray(pctxt,n,type) \
(type*)rtxMemHeapSysAlloc (&(pctxt)->pMemHeap, sizeof(type)*n)

/**
 * Allocate a dynamic array and zero memory. This macro allocates a dynamic
 * array of records of the given type and writes zeros over the allocated
 * memory. The pointer to the allocated array is returned to the caller.
 *
 * @param pctxt        - Pointer to a context block
 * @param n            - Number of records to allocate
 * @param type         - Data type of an array record
 */
#define rtxMemAllocArrayZ(pctxt,n,type) \
(type*)rtxMemHeapAllocZ (&(pctxt)->pMemHeap, sizeof(type)*n)

/**
 * Free memory pointer.  This macro frees memory at the given pointer.
 * The memory must have been allocated using the rtxMemAlloc (or similar)
 * macros or the rtxMem memory allocation macros.  This macro is
 * similar to the C \c free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemAlloc or rtxMemAlloc macro or the
 *   rtxMemHeapAlloc function.
 */
#define rtxMemFreeArray(pctxt,mem_p) \
rtxMemHeapFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Free memory pointer.  This macro frees memory at the given pointer.
 * The memory must have been allocated using the rtxMemSysAlloc (or similar)
 * macros or the rtxMemSys memory allocation macros.  This macro is
 * similar to the C \c free function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to free.  This must have
 *   been allocated using the rtxMemSysAlloc or rtxMemSysAlloc macro or the
 *   rtxMemSysHeapAlloc function.
 */
#define rtxMemSysFreeArray(pctxt,mem_p) \
rtxMemHeapSysFreePtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Reallocate memory.  This macro reallocates a memory block (either
 * expands or contracts) to the given number of bytes.  It is
 * similar to the C \c realloc run-time function.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block to reallocate.  This must have been
 *   allocated using the rtxMemAlloc macro or the rtxMemHeapAlloc function.
 * @param n - Number of items of the given type to be allocated.
 * @param type - Array element data type (for example, int).
 * @return - Void pointer to allocated memory or NULL if insufficient memory
 *   was available to fulfill the request.  This may be the same as the pmem
 *   pointer that was passed in if the block did not need to be relocated.
 */
#define rtxMemReallocArray(pctxt,mem_p,n,type) \
(type*)rtxMemHeapRealloc(&(pctxt)->pMemHeap, (void*)mem_p, sizeof(type)*n)

/* Auto-pointer functions */
/**
 * This function allocates a new block of memory and creates an auto-pointer
 * with reference count set to one.  The \c rtxMemAutoPtrRef and
 * \c rtxMemAutoPtrUnref functions can be used to increment and decrement
 * the reference count.  When the count goes to zero, the memory held by
 * the pointer is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param nbytes        Number of bytes to allocate.
 * @return              Pointer to allocated memory or NULL if not enough
 *                        memory is available.
 */
#define rtxMemNewAutoPtr(pctxt,nbytes) \
rtxMemHeapAlloc(&(pctxt)->pMemHeap, nbytes)

/**
 * This function increments the auto-pointer reference count.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be incremented.
 * @return              Referenced pointer value (ptr argument) or NULL
 *                        if reference count could not be incremented.
 */
#define rtxMemAutoPtrRef(pctxt,ptr) \
rtxMemHeapAutoPtrRef(&(pctxt)->pMemHeap, (void*)(ptr))

/**
 * This function decrements the auto-pointer reference count.  If the count
 * goes to zero, the memory is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be decremented.
 * @return              Positive reference count or a negative error
 *                        code.  If zero, memory held by pointer will
 *                        have been freed.
 */
#define rtxMemAutoPtrUnref(pctxt,ptr) \
rtxMemHeapAutoPtrUnref(&(pctxt)->pMemHeap, (void*)(ptr))

/**
 * This function returns the reference count of the given pointer.
 * goes to zero, the memory is freed.
 *
 * @param pctxt         Pointer to a context structure.
 * @param ptr           Pointer on which reference count is to be fetched.
 * @return              Pointer reference count.
 */
#define rtxMemAutoPtrGetRefCount(pctxt,ptr) \
rtxMemHeapAutoPtrGetRefCount(&(pctxt)->pMemHeap, (void*)(ptr))

/**
 * Check memory pointer.  This macro check pointer on presence in heap.
 *
 * @param pctxt - Pointer to a context block
 * @param mem_p - Pointer to memory block.
 * @return        1 - pointer refer to memory block in heap;
 *                0 - poiter refer not memory heap block.
 */
#define rtxMemCheckPtr(pctxt,mem_p) \
rtxMemHeapCheckPtr(&(pctxt)->pMemHeap, (void*)mem_p)

/**
 * Check memory heap.
 *
 * @param pctxt - Pointer to a context block
 */
#define rtxMemCheck(pctxt) \
rtxMemHeapCheck(&(pctxt)->pMemHeap, __FILE__, __LINE__)

/**
 * Print memory heap structure to stderr.
 *
 * @param pctxt - Pointer to a context block
 */
#define rtxMemPrint(pctxt) \
rtxMemHeapPrint(&(pctxt)->pMemHeap)

/**
 * Set memory heap property.
 *
 * @param pctxt -  Pointer to a context block
 * @param propId - Property Id.
 * @param pProp -  Pointer to property value.
 */
#define rtxMemSetProperty(pctxt,propId,pProp) \
rtxMemHeapSetProperty (&(pctxt)->pMemHeap, propId, pProp)


#ifdef __cplusplus
}
#endif
/**
 * @}
 */
#endif /*__RTXMEMORY_H__*/
