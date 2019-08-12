/* =========================================================================

                              A M S S   H E A P

DESCRIPTION
  Wrapper for malloc, realloc, calloc and free calls for AMSS_HEAP.

Copyright (c) 2006-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/amssheap.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/05/07   jc      Added periodic defragmentation feature.
08/15/07   jc      Added dynamic heap feature.
08/29/06   jc      Added heap debug support.
07/03/06   jc      Added heap init checks and malloc failure logging.
03/27/06   jhs     File created
============================================================================ */


/*============================================================================
                             INCLUDE FILES
============================================================================*/
#include "customer.h"
#include "msm.h"
#include "memheap.h"
#include "msg.h"
#include <stdlib.h>
#ifdef FEATURE_AMSSHEAP_DYNAMIC
#include "KxMutex.h"
#ifdef FEATURE_QUARTZ_135
#include "iguana/memsection.h"
#else
#include "memsection.h"
#endif
#endif  /* FEATURE_AMSSHEAP_DYNAMIC */

/*============================================================================
                             CONSTANT DEFINITIONS
============================================================================*/

/* Heap allocation failure constants */
#ifndef AMSSHEAP_FAIL_ARRAY_SIZE
#define AMSSHEAP_FAIL_ARRAY_SIZE  2
#endif
#ifndef AMSSHEAP_INFO_FREEBLOCKS
#define AMSSHEAP_INFO_FREEBLOCKS  64
#endif

/* Heap dynamic increace constants */
#ifdef FEATURE_AMSSHEAP_DYNAMIC
#ifndef AMSSHEAP_DYNAMIC_INC_SIZE
#define AMSSHEAP_DYNAMIC_INC_SIZE SCL_APPS_AMSS_HEAP_SIZE
#endif
#endif  /* FEATURE_AMSSHEAP_DYNAMIC */

/* Periodic defragmentation constants */
#ifdef FEATURE_AMSSHEAP_DEFRAG

#if defined(FEATURE_HEAP_SMALLER_OVERHEAD) && !FEATURE_HEAP_SMALLER_OVERHEAD
#error FEATURE_AMSSHEAP_DEFRAG is not necessary when FEATURE_HEAP_SMALLER_OVERHEAD=0
#endif

/* Number of free() calls between defragmentation operations */
#ifndef AMSSHEAP_DEFRAG_COUNT
#define AMSSHEAP_DEFRAG_COUNT     32
#endif

/* Copied from memheap.c */
#ifdef FEATURE_MEMHEAP_MT
   #define BEGIN_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->lock_fnc_ptr) \
         (heap)->lock_fnc_ptr(); \
     } while (0)
   #define END_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->free_fnc_ptr) \
         (heap)->free_fnc_ptr(); \
     } while (0)
#else
   #define BEGIN_CRITICAL_SECTION(heap) (void) 0
   #define END_CRITICAL_SECTION(heap) (void) 0
#endif

#endif  /* FEATURE_AMSSHEAP_DEFRAG */

/*============================================================================
                             DATA DECLARATIONS
=============================================================================*/

/* Structure used to log heap allocation failures */
typedef struct amssheap_info_s
{
  struct mem_heap_struct *heap_ptr;
  size_t size;
  mem_heap_totals_type totals;
  unsigned int freeblocks[AMSSHEAP_INFO_FREEBLOCKS];
} amssheap_info_s_type;

#ifdef FEATURE_AMSSHEAP_DYNAMIC
/* Control block for dynamically allocated heaps */
typedef struct amssheap_control_s
{
  /* memsection allocation data */
  memsection_ref_t memsection;
  uintptr_t memptr;

  /* memheap control structure */
  mem_heap_type heap;

} amssheap_control_s_type;
#endif

/* Structure used to compare address ranges */
typedef struct amssheap_addr_cmp_s
{
  void *start;
  void *end;
} amssheap_addr_cmp_s_type;

/*============================================================================
                             GLOBAL VARIABLES
============================================================================*/

/*============================================================================
                             LOCAL VARIABLES
============================================================================*/

mem_heap_type amss_mem_heap;

/* Current index into the array used to log heap allocation failures.
 * This index will continue to be incremented even when the array is full,
 * to serve as a total failure counter.
 */
int amssheap_fail_index = 0;

/* Array used to log heap allocation failures */
amssheap_info_s_type amssheap_fail_info[AMSSHEAP_FAIL_ARRAY_SIZE];

#ifdef FEATURE_AMSSHEAP_DYNAMIC

/* Used for address range comparisons */
amssheap_addr_cmp_s_type amss_mem_heap_range;

/* An additional heap will be dynamically allocated from L4/iguana if
 * the initial heap is exhausted
 */
amssheap_control_s_type amssheap_dynamic;

/* Critical section used to control access to the dynamically allocated heap */
KxMutex amssheap_crit_sect;

/* Statistics counters for the dynamic heap allocated from L4/iguana */
uint32 amssheap_dynamic_allocs;   /* successful allocations */
uint32 amssheap_dynamic_frees;    /* deallocations */
uint32 amssheap_dynamic_alloc_failures;   /* allocation failures */

#endif  /* FEATURE_AMSSHEAP_DYNAMIC */

#ifdef FEATURE_AMSSHEAP_DEFRAG
/* Count number of free() calls between defragmentation operations */
int amssheap_defrag_counter;
#endif


/*============================================================================
                             MACRO DEFINITIONS
============================================================================*/


/*============================================================================
                             FORWARD DECLARATIONS
============================================================================*/

void amssheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
);

#ifdef FEATURE_AMSSHEAP_DEBUG
/* AMSS memory debug functions */
extern void memdebug_init(void);
extern void memdebug_malloc
(
  size_t size,      /* Size of allocation request (bytes) */
  void *caller_ptr, /* Pointer to caller */
  void *ptr         /* Pointer to allocated memory block */
);
extern void memdebug_free
(
  void *ptr         /* Pointer to allocated memory block */
);
extern void memdebug_failure_handler(void);
#endif  /* FEATURE_AMSSHEAP_DEBUG */

#ifdef FEATURE_AMSSHEAP_DYNAMIC
/*===========================================================================

FUNCTION amssheap_dyn_heap_enabled

DESCRIPTION
  Verifies dynamic heap is allocated and ready for use.

DEPENDENCIES
  Caller must lock dynamic heap critical section.

RETURN VALUE
  int - TRUE if dynamic heap can be used for allocation, FALSE otherwise

SIDE EFFECTS
  Will attempt to allocate and initialize the dynamic heap.

===========================================================================*/
int amssheap_dyn_heap_enabled(void)
{
  /* Allocate and init the dynamic heap if necessary */
  if (amssheap_dynamic.memsection == (memsection_ref_t)0)
  {
    amssheap_dynamic.memsection = memsection_create(
      AMSSHEAP_DYNAMIC_INC_SIZE, &amssheap_dynamic.memptr);
    if (amssheap_dynamic.memsection != (memsection_ref_t)0 &&
        amssheap_dynamic.memptr != (uintptr_t)0)
    {
      mem_init_heap(&amssheap_dynamic.heap, (void *)amssheap_dynamic.memptr,
        AMSSHEAP_DYNAMIC_INC_SIZE, amssheap_malloc_failed);
      amssheap_dynamic_allocs++;
    }
    else
    {
      memset(&amssheap_dynamic, 0, sizeof(amssheap_dynamic));
      amssheap_dynamic_alloc_failures++;
    }
  }

  /* Indicate whether the dynamic heap is ready for use */
  if (amssheap_dynamic.memsection != (memsection_ref_t)0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}
#endif  /* FEATURE_AMSSHEAP_DYNAMIC */

/*===========================================================================

FUNCTION malloc

DESCRIPTION
  Allocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
#ifdef FEATURE_AMSSHEAP_DEBUG
void *amssheap_malloc
(
  size_t size,
  void *caller_ptr
)
#elif defined(AMSSHEAP_UNIT_TEST)
void *malloc_ut
(
  size_t size
)
#else
void *malloc
(
  size_t size
)
#endif
{
  void *ptr;
  ptr = mem_malloc( &amss_mem_heap, size );
#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_malloc(size, caller_ptr, ptr);
#endif
#ifdef FEATURE_AMSSHEAP_DYNAMIC
  if (ptr == NULL)
  {
    /* Lock access around dynamic heap */
    KxMutex_Lock(&amssheap_crit_sect);

    /* Allocate from the dynamic heap if possible */
    if (amssheap_dyn_heap_enabled())
    {
      ptr = mem_malloc(&amssheap_dynamic.heap, size);
    }

    /* Unlock access around dynamic heap */
    KxMutex_Unlock(&amssheap_crit_sect);
  }
#endif
  return ptr;
}


/*===========================================================================

FUNCTION realloc

DESCRIPTION
  Reallocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  None

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
#ifdef FEATURE_AMSSHEAP_DEBUG
void *amssheap_realloc
(
  void   *ptr,
  size_t size,
  void *caller_ptr
)
#elif defined(AMSSHEAP_UNIT_TEST)
void *realloc_ut
(
  void   *ptr,
  size_t size
)
#else
void *realloc
(
  void   *ptr,
  size_t size
)
#endif
{
#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_free(ptr);
#endif
  ptr = mem_realloc( &amss_mem_heap, ptr, size );
#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_malloc(size, caller_ptr, ptr);
#endif
  return ptr;
}


/*===========================================================================

FUNCTION calloc

DESCRIPTION
  Callocates from the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  A a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  size  - Size of allocation request.

===========================================================================*/
#ifdef FEATURE_AMSSHEAP_DEBUG
void *amssheap_calloc
(
  size_t count,
  size_t size,
  void *caller_ptr
)
#elif defined(AMSSHEAP_UNIT_TEST)
void *calloc_ut
(
  size_t count,
  size_t size
)
#else
void *calloc
(
  size_t count,
  size_t size
)
#endif
{
  void *ptr;
  ptr = mem_calloc( &amss_mem_heap, count, size );
#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_malloc(count * size, caller_ptr, ptr);
#endif
#ifdef FEATURE_AMSSHEAP_DYNAMIC
  if (ptr == NULL)
  {
    /* Lock access around dynamic heap */
    KxMutex_Lock(&amssheap_crit_sect);

    /* Allocate from the dynamic heap if possible */
    if (amssheap_dyn_heap_enabled())
    {
      ptr = mem_calloc(&amssheap_dynamic.heap, count, size);
    }

    /* Unlock access around dynamic heap */
    KxMutex_Unlock(&amssheap_crit_sect);
  }
#endif
  return ptr;
}


#ifdef FEATURE_AMSSHEAP_DEFRAG
/*===========================================================================

FUNCTION amssheap_defrag

DESCRIPTION
  This function defragments the AMSS heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void amssheap_defrag
(
  struct mem_heap_struct *heap_ptr   /* Heap to defragment */
)
{
  mem_block_header_type *block_ptr;
  mem_block_header_type *next_ptr;
  mem_block_header_type *end_ptr;
  mem_block_header_type *first_free_ptr;
  unsigned long total_bytes;
  long total_blocks;

  BEGIN_CRITICAL_SECTION(heap_ptr);

  block_ptr = heap_ptr->first_block;
  end_ptr = (void *)((char *)block_ptr + heap_ptr->total_bytes);
  total_bytes = block_ptr->forw_offset;
  first_free_ptr = NULL;
  total_blocks = 1;
  while (!block_ptr->last_flag)
  {
    next_ptr =
      (mem_block_header_type *)((char *)block_ptr + block_ptr->forw_offset);
    total_bytes += next_ptr->forw_offset;

    /* Check block header integrity */
    ASSERT(next_ptr < end_ptr);
    ASSERT(next_ptr->pad1 == MEMHEAP_PAD_CHAR);
    ASSERT(next_ptr->forw_offset > sizeof(mem_block_header_type));

    if (!block_ptr->free_flag || !next_ptr->free_flag)
    {
      block_ptr = next_ptr;
      total_blocks++;
      continue;
    }

    /* Record first free block found */
    if (first_free_ptr == NULL)
    {
      first_free_ptr = block_ptr;
    }

    /* This block and the next are free, so concatenate them */
    block_ptr->forw_offset += next_ptr->forw_offset;
    block_ptr->last_flag = next_ptr->last_flag;
    next_ptr->forw_offset = 0;
  }
  ASSERT(total_bytes == heap_ptr->total_bytes);
  heap_ptr->total_blocks = total_blocks;

  /* Set next block pointer to first free block */
  if (first_free_ptr != NULL)
  {
    heap_ptr->next_block = first_free_ptr;
  }
  else
  {
    heap_ptr->next_block = heap_ptr->first_block;
  }

  END_CRITICAL_SECTION(heap_ptr);
}
#endif  /* FEATURE_AMSSHEAP_DEFRAG */


/*===========================================================================

FUNCTION free

DESCRIPTION
  Deallocates a block of memory and returns it to the heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Replaces C Std Library implementation.

ARGUMENTS
  ptr - A pointer to the memory block that needs to be deallocated.

===========================================================================*/
#ifdef FEATURE_AMSSHEAP_DEBUG
void amssheap_free
(
  void *ptr,
  void *caller_ptr
)
#elif defined(AMSSHEAP_UNIT_TEST)
void free_ut
(
  void *ptr
)
#else
void free
(
  void *ptr
)
#endif
{
#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_free(ptr);
#endif
#if !defined(FEATURE_AMSSHEAP_DYNAMIC)
  mem_free( &amss_mem_heap, ptr );
#else
  if (amssheap_dynamic.memsection == (memsection_ref_t)0)
  {
    /* Dynamic heap is not in use, so free from default heap */
    mem_free(&amss_mem_heap, ptr);
  }
  else
  {
    /* Dynamic heap is in use, so determine which heap to use */
    if (ptr >= amss_mem_heap_range.start && ptr < amss_mem_heap_range.end)
    {
      mem_free(&amss_mem_heap, ptr);
    }
    else
    {
      /* Lock access around dynamic heap */
      KxMutex_Lock(&amssheap_crit_sect);

      /* Free block, and free dynamic heap if possible */
      mem_free(&amssheap_dynamic.heap, ptr);
      if (amssheap_dynamic.heap.used_bytes == 0)
      {
        memsection_delete(amssheap_dynamic.memsection);
        memset(&amssheap_dynamic, 0, sizeof(amssheap_dynamic));
        amssheap_dynamic_frees++;
      }

      /* Unlock access around dynamic heap */
      KxMutex_Unlock(&amssheap_crit_sect);
    }
  }
#endif  /* !FEATURE_AMSSHEAP_DYNAMIC */

#ifdef FEATURE_AMSSHEAP_DEFRAG
  /* Periodically defragment the heap */
  if (amssheap_defrag_counter-- <= 0)
  {
    amssheap_defrag(&amss_mem_heap);
    amssheap_defrag_counter = AMSSHEAP_DEFRAG_COUNT;
  }
#endif
}


/*===========================================================================

FUNCTION amssheap_malloc_failed

DESCRIPTION
  This function logs heap statistics, and is called when memheap detects
  an allocation failure from the AMSS heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void amssheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  amssheap_info_s_type *info;

#ifdef FEATURE_AMSSHEAP_DEBUG
  memdebug_failure_handler();
#endif

  /* Record the current heap state */
  if (amssheap_fail_index < ARR_SIZE(amssheap_fail_info))
  {
    info = &amssheap_fail_info[amssheap_fail_index];
    info->heap_ptr = heap_ptr;
    info->size = size;
    mem_heap_get_totals(heap_ptr, &info->totals);
    mem_heap_get_freeblock_info(heap_ptr, AMSSHEAP_INFO_FREEBLOCKS,
      info->freeblocks);
  }
  amssheap_fail_index++;

  MSG_ERROR("AMSS heap failed to allocate %d bytes (failure %d)",
    size, amssheap_fail_index, 0);
}


#ifdef FEATURE_AMSSHEAP_DEBUG
/*===========================================================================

FUNCTION amssheap_global_malloc_failed

DESCRIPTION
  This function is called when memheap detects an allocation failure
  from any heap managed by memheap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void amssheap_global_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  MSG_ERROR("mem_malloc heap 0x%x failed to allocate %d bytes",
    (uint32)heap_ptr, size, 0);
}
#endif  /* FEATURE_AMSSHEAP_DEBUG */


/*===========================================================================

FUNCTION amss_malloc_init

DESCRIPTION
  Initializes the AMSS heap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

ARGUMENTS
  start_heap - Pointer to the beginning of the heap.
  end_heap   - Pointer to the end of the heap.

===========================================================================*/
void amss_malloc_init
(
  void *start_heap,
  void *end_heap
)
{
  uint32 size;

  /* Clear the heap control struct in case of failure */
  memset(&amss_mem_heap, 0, sizeof(amss_mem_heap));

  size = (uint32)end_heap - (uint32)start_heap;

  if (start_heap == NULL || size == 0)
  {
    for ( ; ; )   /* Cannot fatal at this point in boot */
    {
    }
  }

  mem_init_heap( &amss_mem_heap, start_heap, size,
#ifdef FEATURE_AMSSHEAP_DYNAMIC
    NULL);
#else
    amssheap_malloc_failed);
#endif

  mem_heap_set_int_lock( &amss_mem_heap );

#ifdef FEATURE_AMSSHEAP_DEBUG
  /* Install a handler for all memheap allocation errors */
  if ( mem_allocator_failed_hook == NULL )
  {
    mem_allocator_failed_hook = amssheap_global_malloc_failed;
  }

  /* Init AMSS memory debug module */
  memdebug_init();
#endif

#ifdef FEATURE_AMSSHEAP_DYNAMIC
  /* Initialize dynamic heap allocation data */
  if (!KxMutex_Init(&amssheap_crit_sect, "AMSSHEAP"))
  {
    for ( ; ; )   /* Cannot fatal at this point in boot */
    {
    }
  }
  memset(&amssheap_dynamic, 0, sizeof(amssheap_dynamic));
  amssheap_dynamic_allocs = 0;
  amssheap_dynamic_frees = 0;
  amssheap_dynamic_alloc_failures = 0;
  amss_mem_heap_range.start = start_heap;
  amss_mem_heap_range.end = end_heap;
#endif

#ifdef FEATURE_AMSSHEAP_DEFRAG
  /* Initialize the defragment down-counter */
  amssheap_defrag_counter = AMSSHEAP_DEFRAG_COUNT;
#endif
}
