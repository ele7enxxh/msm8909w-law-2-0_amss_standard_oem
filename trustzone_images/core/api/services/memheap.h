#ifndef MEMHEAP_H
#define MEMHEAP_H
/*===========================================================================

DESCRIPTION
  A simple sub-allocator to manage memory allocations and deallocations
  using a Next Fit strategy.  

  Note that these routines are FULLY re-entrant.  Furthermore, while
  performing memory allocation/deallocation calls on one heap, the routines
  may be interrupted to perform memory allocation/deallocation calls on
  different heaps without blocking or corruption.  However, should an
  interrupting task attempt to perform memory allocation/deallocation
  on the same heap that had a critical section interrupted, it will block
  allowing the first call to finish.  All this is accomplished by giving
  each heap its own semaphore.

  FEATURE_MEMHEAP_MT
    Activates multi threading support for the heap

  Usage Notes - Avoiding Fragmentation  
 
  1. If all bytes in the heap are freed, the heap is guaranteed to be returned
     to the same state as after a call to mem_init_heap, except that the
     values of maxUsed & maxRequest are not reset.  The order in which blocks
     are freed is irrelevant so long as they ALL are freed.
 
  2. After n consecutive malloc/calloc calls, in a heap with non-fragmented
     free space (and no intervening realloc calls that return a new pointer),
     calling free in the reverse order on the n blocks guarantees that the heap
     will be returned to the exact same state as before the n alloc calls.
 
  3. FEATURE_HEAP_SMALLER_OVERHEAD == 0
     (2) still applies, except the calls to free may be made in any order and
     after all n blocks are freed, the heap is guaranteed to be returned to the
     exact same state as prior to the n alloc calls.
 
  4. FEATURE_HEAP_SMALLER_OVERHEAD == 1
     The overhead per heap block is 8 bytes.  However it's not possible to backup
     the next-block-to-start-searching-for-free-space pointer, so the heap MAY
     become more fragmented, but it MAY NOT -- it depends on the alloc/free
     calling patterns.
 
  5. FEATURE_HEAP_SMALLER_OVERHEAD == 0
     The overhead per heap block is 12 bytes.  Backing up is possible, so some
     fragmention that otherwise MIGHT occur can be prevented.  There is very little
     performance cost for this, the big expense is the additional 4 bytes of overhead
     per heap block.

Copyright (c) 1997,1998 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 1999,2000 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 2003      by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                                 Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.h_v   1.1   07 Mar 2002 18:48:56   rajeevg  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/memheap.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     --------------------------------------------------------
03/31/009  sri     amssassert.h is further featurised based on APPS/QDSP6
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/17/03    gr     Mainlined FEATURE_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
10/01/01    gr     Added support for some basic heap statistics.
11/17/00   crr     Change stdlib.h to stddef.h for size_t def. Add forward
                     decl for struct mem_heap_struct.
11/09/00   jct     Match to coding convention, remove of semaphore use, replace
                   with REX critical sections, remove stddef.h inclusion
02/17/99   kjm     Added realloc function.  Minor additions to
                     mem_heap_struct.  Added giant function descriptions.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.h file.
08/22/97   kjm     Original SubAllocator.h file created.
===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#if defined CUST_H
   #include "customer.h"
#endif
#include "comdef.h"
#ifndef FEATURE_WINCE
#if ((defined (IMAGE_MODEM_PROC)) || (! defined FEATURE_MULTIPROCESSOR) || \
     (defined (FEATURE_BMPOS_QNK)))
#include "amssassert.h"
#endif
#else
#include "assert.h"
#endif
#include <stddef.h>

/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define MEMHEAP_VERSION 0x0120    /* 1.2 */

#ifndef FEATURE_HEAP_SMALLER_OVERHEAD
   #define FEATURE_HEAP_SMALLER_OVERHEAD 1
#endif

/* Character to which we set the pad field in the block header. Useful to
** catch heap corruption.
*/
#define MEMHEAP_PAD_CHAR 0xaa

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */

/* Allocation Failed Procedure
**   This function is called when there is not enough room in ioHeap to satisfy
**   a request of inRequest bytes.  This routine should free up any memory it
**   can before returning.  The request will then be tried again.  There are
**   no restrictions on what this function may do including memory allocation/
**   deallocation calls.  Note that before calling this function, calls to
**   the allocator failed proc on ioHeap are disabled.  After this function
**   returns, calls are re-enabled.  This does NOT affect other heaps, so if
**   multiple heaps share the same allocator failed proc, it should be
**   re-entrant.
*/
struct mem_heap_struct;

typedef void (*mem_allocator_failed_proc_type)(
   struct mem_heap_struct *heap_ptr,
      /* Heap on which allocation is attempted
      */
   size_t                  request_size
      /* Size of failed request
      */
);

/* Each heap can have its own function that is used to lock the heap and
** free the heap.  mem_lock_fnc_type and mem_free_fnc_type are the types
** of these functions.
*/
typedef void (*mem_lock_fnc_type)( void );
typedef void (*mem_free_fnc_type)( void );

/* Memory Block Header
**    Header for each memory block allocated
**    NOTE: The size of this structure MUST be a multiple of the
            largest alignment size required by the host CPU
*/
typedef struct mem_block_header_struct {
#if !FEATURE_HEAP_SMALLER_OVERHEAD
  unsigned long backOffset; 
#endif
  unsigned long forw_offset;
  char          free_flag;
  char          last_flag;
  unsigned char extra;
  unsigned char pad1;
#ifdef FEATURE_MEM_DEBUG
  /* These fields allow us to match blocks in a heap with the filename and
  ** line number corresponding to the call that allocated or freed that
  ** block. The hash value is just a crc16 calculation over the filename,
  ** so it is possible that more than one filename in a build hashes to a
  ** particular value. In this case, the line number should be enough to
  ** identify the file uniquely in most cases.
  */
  uint16        filenamehash;
  uint16        linenum;
#endif
#ifdef FEATURE_MEM_HEADER_8BYTE
  /* If FEATURE_MEM_HEADER_8BYTE is defined, we need to pad the header to a
  ** multiple of 8-bytes.
  */
#if( ( defined(FEATURE_MEM_DEBUG) && FEATURE_HEAP_SMALLER_OVERHEAD ) || \
     ( !defined(FEATURE_MEM_DEBUG) && !FEATURE_HEAP_SMALLER_OVERHEAD ) )
  uint32        pad2;
#endif /* (MEM_DEBUG && SMALLER) || (!MEM_DEBUG && !SMALLER) */
#endif /* FEATURE_MEM_HEADER_8BYTE */
} mem_block_header_type;

/* Memory Heap
**    State of a heap
*/
typedef struct mem_heap_struct {
  mem_block_header_type         *first_block;
  mem_block_header_type         *next_block;
  unsigned long                  total_blocks;
  unsigned long                  total_bytes;
  unsigned long                  used_bytes;
  unsigned long                  max_used;
  unsigned long                  max_request;
  mem_allocator_failed_proc_type fail_fnc_ptr;  
#ifdef FEATURE_MEMHEAP_MT
  mem_lock_fnc_type              lock_fnc_ptr;
  mem_free_fnc_type              free_fnc_ptr;
#endif
} mem_heap_type;

#ifdef FEATURE_MEM_DEBUG
  typedef struct {
    void  *ptr;
    uint32 size;
    uint16 filenamehash;
    uint16 linenum;
  } mem_heap_alloc_info_type;
#endif


/* ------------------------------------------------------------------------
** Functions
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
   extern "C"
   {
#endif

/*===========================================================================
FUNCTION MEM_HEAP_INIT

DESCRIPTION
  Initializes the ioHeap object and sets up inMemoryChunk for use with the
  ioHeap object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  inFailProc may be NULL in which case no function will
  be called if mem_malloc or mem_calloc is about to fail.  If inFailProc
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
void mem_init_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
   void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                  heap_mem_size,
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */
   mem_allocator_failed_proc_type fail_fnc_ptr
      /* Function to call when allocation fails, can be NULL
      */
);

/*===========================================================================
FUNCTION MEM_CALLOC

DESCRIPTION
  Allocates enough space for elt_count elements each of elt_size bytes
  from the heap and initializes the space to nul bytes.  If
  heap_ptr is NULL or elt_count or elt_size is 0, the NULL pointer will
  be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_calloc,1p,2n>=0&&3n>=0&&(@p==0||@P==2n*3n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_calloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         elt_count,
         /* Number of elements to allocate
         */
      size_t         elt_size,
         /* Size of each element
         */
      char          *file_name,
         /* Name of file from which mem_calloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_calloc call
         */
   );
#else
   void* mem_calloc(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         elt_count,
         /* Number of elements to allocate
         */
      size_t         elt_size
         /* Size of each element
         */
   );
#endif

#ifdef FEATURE_MEM_DEBUG
/*lint -save -e683 */
   #define  mem_calloc(heap_ptr, elt_count, elt_size)\
      mem_calloc_debug(heap_ptr,\
                       elt_count,\
                       elt_size,\
                       (char *) __FILE__,\
                       (unsigned int) __LINE__)
                       
/*lint -restore */
#endif

/*===========================================================================
FUNCTION MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_malloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         size,
         /* Number of bytes to allocate
         */
      char          *file_name,
         /* Name of file from which mem_malloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_malloc call
         */
   );
#else
   void* mem_malloc(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      size_t         size
         /* Number of bytes to allocate
         */
   );
#endif

#ifdef FEATURE_MEM_DEBUG
/*lint -save -e683 */
   #define  mem_malloc(heap_ptr, size)\
      mem_malloc_debug(heap_ptr,\
                       size,\
                       (char *) __FILE__,\
                       (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION MEM_REALLOC

DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===========================================================================*/
/*lint -sem(mem_realloc,1p,3n>=0&&(@p==0||@P==3n)) */
#ifdef FEATURE_MEM_DEBUG
   void* mem_realloc_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      void          *ptr,
         /* A block previously allocated from heap_ptr 
         */
      size_t         size,
         /* Number of bytes to allocate
         */
      char          *file_name,
         /* Name of file from which mem_realloc was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_realloc call
         */
   );
#else
   void *mem_realloc(
      mem_heap_type *heap_ptr,
         /* Heap in which to re-allocate
         */
      void          *ptr,
         /* A block previously allocated from heap_ptr 
         */
      size_t         size
         /* New size (in bytes) of the ptr block of memory
         */
   );
#endif

#ifdef FEATURE_MEM_DEBUG
/*lint -save -e683 */
   #define  mem_realloc(heap_ptr, ptr, size)\
      mem_realloc_debug(heap_ptr,\
                       ptr,\
                       size,\
                       (char *) __FILE__,\
                       (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION MEM_FREE

DESCRIPTION
  Deallocates the ptr block of memory.  If ptr is NULL, heap_ptr is NULL or
  ptr is outside the range of memory managed by heap_ptr, then this function
  call does nothing (and is guaranteed to be harmless).  This function will
  ASSERT if it can detect an attempt to free an already freed block.  (This
  is not always reliable though, so it might not catch it.)
===========================================================================*/
/*lint -sem(mem_free,1p) */
#ifdef FEATURE_MEM_DEBUG
   void mem_free_debug(
      mem_heap_type *heap_ptr,
         /* Heap from which to allocate
         */
      void          *ptr,
         /* Memory to free
         */
      char          *file_name,
         /* Name of file from which mem_free was called
         */
      unsigned int   line_number
         /* Line number corresponding to mem_free call
         */
   );
#else
   void mem_free(
      mem_heap_type *heap_ptr,
         /* Heap in which to free memory
         */
      void          *ptr
         /* Memory to free
         */
   );
#endif

#ifdef FEATURE_MEM_DEBUG
/*lint -save -e683 */
   #define  mem_free(heap_ptr, ptr)\
      mem_free_debug(heap_ptr,\
                       ptr,\
                       (char *) __FILE__,\
                       (unsigned int) __LINE__)
/*lint -restore */
#endif

/*===========================================================================
FUNCTION MEM_HEAP_USED

DESCRIPTION
  Returns the number of allocated bytes, excluding overhead.
===========================================================================*/
size_t mem_heap_used (mem_heap_type *heap_ptr);

/*===========================================================================
FUNCTION MEM_HEAP_OVERHEAD

DESCRIPTION
  Returns the number of bytes of overhead, per allocation, for the heap.
===========================================================================*/
size_t mem_heap_overhead (mem_heap_type *heap_ptr);

/*===========================================================================
FUNCTION MEM_HEAP_CONSUMED

DESCRIPTION
  Returns the number of bytes consumed by allocation and overhead.
===========================================================================*/
size_t mem_heap_consumed (mem_heap_type *heap_ptr);

/* ------------------------------------------------------------------------
** 
** Memory Heap Debug and Test Routines
**
** ------------------------------------------------------------------------ */

/* defaults to NULL.  If not NULL, function will be called
   whenever an allocation has failed due to an out of heap
   space condition and is about to return NULL to the caller */
extern mem_allocator_failed_proc_type mem_allocator_failed_hook;

enum {
  kBlockFree = -1,
  kBlockUsed = 0,
  kLastBlock = -1,
  kMinChunkSize = 16
};

/*===========================================================================
FUNCTION MEM_GET_BLOCK_LOGICAL_SIZE

DESCRIPTION
  Returns the logical block size of a memory block in the heap

  Returns the size (in bytes) of the block excluding block header overhead and
  unused bytes at the end of the block.
===========================================================================*/
/*lint -sem(mem_get_block_logical_size,1p) */
unsigned long mem_get_block_logical_size(
   const mem_block_header_type *block_hdr_ptr
     /*  A memory block to get the logical size of
     */
);

typedef struct mem_heap_block_iterator_struct {
  const mem_heap_type   *mAllocator;
  mem_block_header_type *mBlock;
} mem_heap_block_iterator_type;

typedef struct mem_heap_totals_struct {
  unsigned long total_physical_bytes;
  unsigned long current_block_count;
  unsigned long used_blocks;
  unsigned long header_bytes;
  unsigned long used_bytes;
  unsigned long free_bytes;
  unsigned long wasted_bytes;
  unsigned long largest_free_block;
  unsigned long max_logical_used;
  unsigned long max_logical_request;
} mem_heap_totals_type;

/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_INIT

DESCRIPTION
  Initializes an iterator structure to be used for walking the blocks
  in the specified heap.
===========================================================================*/
void mem_heap_block_iterator_init(
   mem_heap_block_iterator_type *block_iter_ptr,
     /*  The Iterator structure to be initialized
     */
   const mem_heap_type          *heap_ptr
     /*  The heap this iterator should iterate over
     */
);

/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_RESET

DESCRIPTION
  Reset a heap block iterator to begin the iteration of heap blocks from
  the beginning.
===========================================================================*/
void mem_heap_block_iterator_reset(
   mem_heap_block_iterator_type *block_iter_ptr
     /* The iterator structure to be reset
     */
);

/*===========================================================================
FUNCTION MEM_HEAP_BLOCK_ITERATOR_NEXT

DESCRIPTION
  Return a memory block header for the next block (or next virtual block)
  in the heap being iterated.  Description is only valid if the function
  returns a non-zero value.
  The heap being iterated should not be changed during the iteration as the
  iterator may get lost or return bogus values.  There is no protection
  against this however.

  Returns:
      zero      There are no more heap blocks to return
      non-zero  The next heap block was returned in outBlock successfully
===========================================================================*/
int mem_heap_block_iterator_next(
   mem_heap_block_iterator_type *block_iter_ptr,
     /* Iterator to return next iterated block from
     */
   mem_block_header_type        *out_block_ptr,
     /* Pointer to a location where a copy of a memory block header
        is placed describing the returned memory block.  Only
        valid if the function returns a non-zero value.
     */
   int                           fake_free_coalesced
     /* Input flag.  zero => return the next block header from the heap
        exactly as it is.  non-zero => return the next virtual block
        header from the heap.  Virtual block headers are created by
        collapsing all adjacent free blocks into one.
     */
);

/*===========================================================================
FUNCTION MEM_HEAP_GET_TOTALS

DESCRIPTION
  Returns heap totals for the heap.  This function IS thread safe.
  Heap must already have been initialized.
  Return totals for the heap.
===========================================================================*/
void mem_heap_get_totals(
   mem_heap_type  *heap_ptr,
      /* The heap to return totals for
      */
   mem_heap_totals_type *totals
      /* A pointer to a totals structure to receive the various
         heap information
      */
);

/*===========================================================================
FUNCTION MEM_HEAP_SET_INT_LOCK

DESCRIPTION
  This function sets up the specified heap to use interrupt locking and
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_int_lock( mem_heap_type *heap_ptr );

/*===========================================================================
FUNCTION MEM_HEAP_SET_TASK_LOCK

DESCRIPTION
  This function sets up the specified heap to use task locking and
  freeing at its locking mechanism.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_task_lock( mem_heap_type *heap_ptr );

/*===========================================================================
FUNCTION MEM_HEAP_SET_NO_LOCK

DESCRIPTION
  This function sets up the specified heap to use no locking. This implies
  that this heap should only be used from the context of one task.
  Returns 1 on success and 0 on failure.
===========================================================================*/
int
mem_heap_set_no_lock( mem_heap_type *heap_ptr );

/*===========================================================================
FUNCTION MEM_HEAP_GET_FREEBLOCK_INFO

DESCRIPTION
  Returns information about the sizes of the largest n free blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_freeblock_info(
  mem_heap_type *heap_ptr,
  unsigned int   num_blocks,
  unsigned int  *buf
);
 
#ifdef FEATURE_MEM_DEBUG

/*===========================================================================
FUNCTION MEM_HEAP_GET_ALLOC_INFO

DESCRIPTION
  Returns information about the sizes of the first n allocated blocks in the
  heap.
  The heap must already have been initialized. The buffer passed in must
  have enough space to hold the requested information.
===========================================================================*/
void
mem_heap_get_alloc_info(
  mem_heap_type            *heap_ptr,
  unsigned int              num_blocks,
  mem_heap_alloc_info_type *buf
);

#endif /* FEATURE_MEM_DEBUG */

/*===========================================================================
FUNCTION MEM_HEAP_GET_CHUNK_SIZE

DESCRIPTION
  Returns the minimum chunk size used for allocations from a heap. All
  allocations from a specific heap are multiples of this size.
===========================================================================*/
unsigned int
mem_heap_get_chunk_size( mem_heap_type *heap_ptr );

#ifdef __cplusplus
   }
#endif

#endif /* MEMHEAP_H */
