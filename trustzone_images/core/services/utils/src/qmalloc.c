/*===========================================================================
                              <qmalloc.c>
DESCRIPTION
  Provides memory allocation functionality to AMSS before the Brew
  application is up and running. This is done using a small statically
  allocated array.

  Once AMSS is up and running, the Brew interface is used for the
  allocation of memory from the Brew heap.

  This functionality should be used by AMSS ONLY!!!!!

  Compiled only when the ADS compiler is envoked.

  Borrows heavily from memheap.c

EXTERNALIZED FUNCTIONS
  q_malloc - Version of malloc that either allocates memory from the
             local heap (q_heap) if AMSS is not running, or uses the
             Brew library once AMSS has reached the end of tmc_init().

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No initialisation of this module is required.

  Copyright (c) 2005-2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/qmalloc.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/09   sri     Introduced Mutex for protection inside q_mem_malloc()
09/09/05   TJH     Created
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#include "customer.h"
#include "comdef.h"
#include "tmc.h"

#if (((defined FEATURE_Q_WIN32_LOCKS) || (defined FEATURE_WINCE)) && !defined(FEATURE_WINCE_BOOTLOADER))
#include <windows.h>
#define USE_MUTEX
#endif

#ifdef FEATURE_QUBE
#include "qube.h"
#include "err.h"
#endif /* FEATURE_QUBE */

#ifndef FEATURE_THIN_UI
#include "OEMFeatures.h"
#endif /* FEATURE_THIN_UI */

#if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
  #include "OEMHeap.h"
  #if defined (FEATURE_BREW_3_0)
    #include "AEE_OEMHeap.h"
  #endif
#endif

#ifdef FEATURE_DISP_TASK
  #include "disp_task.h"
#endif

#include "qmalloc.h"

#define FEATURE_REX_CS
#ifdef FEATURE_REX_CS
#include "k1_priv.h"
#endif /* FEATURE_REX_CS */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================
  Constant / Define Declarations
===========================================================================*/
#ifdef FEATURE_DISP_TASK
  #ifdef FEATURE_MDP2
    #define Q_DISP_TASK_HEAP_SIZE (DISP_TASK_BUF_WIDTH  *         \
                                DISP_TASK_BUF_HEIGHT * 2 *    \
                                DISP_NUM_PACKETS)
  #else
    #define Q_DISP_TASK_HEAP_SIZE DISP_TASK_MIN_MEMORY_REQUIREMENT
  #endif
#else
  #define Q_DISP_TASK_HEAP_SIZE (2 * 65536)
#endif

#ifdef FEATURE_QVPHONE
  #define Q_QVP_TASK_HEAP_SIZE 65536
#else
  #define Q_QVP_TASK_HEAP_SIZE 0
#endif

#ifdef FEATURE_REX_CS
  #define Q_HEAP_SIZE 1
#else
  #ifndef Q_HEAP_SIZE
    #define Q_HEAP_SIZE (3 * 65536 + Q_DISP_TASK_HEAP_SIZE + \
                             Q_QVP_TASK_HEAP_SIZE)
  #endif
#endif /* FEATURE_REX_CS */
/* Code to enter and exit critical sections.
 *
 */
#if defined (FEATURE_Q_WIN32_LOCKS)
static void        *q_malloc_mutex;
#elif defined (FEATURE_QUBE)
static qmutex_t     q_malloc_mutex;
#else /* if REX */
static rex_crit_sect_type q_malloc_mutex;
#endif /* FEATURE_WINCE */

#define INIT_HEAP_CRITICAL_SECTION() init_mutex()
#define ENTER_HEAP_CRITICAL_SECTION() lock_mutex()  
#define EXIT_HEAP_CRITICAL_SECTION() unlock_mutex()
/* #define DEINIT_CRITICAL_SECTION() - not reqd/implemented*/

/*===========================================================================
  Type Declarations
===========================================================================*/
enum {
    q_kBlockFree = -1,
    q_kBlockUsed = 0,
    q_kLastBlock = -1,
    q_kMinChunkSize = 16

};

/****************************************************************************
 * Memory Block Header
 *    Header for each memory block allocated
 *    NOTE: The size of this structure MUST be a multiple of the
 *          largest alignment size required by the host CPU
 ****************************************************************************/
typedef struct q_mem_block_header_struct {
    uint32 forw_offset;
    int8   free_flag;
    int8   last_flag;
    uint8  extra;
    uint8  pad1;

} q_mem_block_header_type;

/****************************************************************************
 * Memory Heap
 *    State of heap
 ****************************************************************************/
typedef struct q_mem_heap_struct {
    q_mem_block_header_type *first_block;
    q_mem_block_header_type *next_block;
    uint32                   total_blocks;
    uint32                   total_bytes;
    uint32                   used_bytes;
    uint32                   max_used;
    uint32                   max_request;

} q_mem_heap_type;

/*===========================================================================
  Local Object Definitions
===========================================================================*/
static q_mem_heap_type q_heap;
static int8            q_heap_buffer[Q_HEAP_SIZE];
static uint32          q_malloc_max = 0;

/*===========================================================================
  Forward Declarations
===========================================================================*/

/* ==========================================================================
FUNCTION INIT_MUTEX
DESCRIPTION
   Initializes the mutex
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
static void init_mutex( void )
{
#if defined (USE_MUTEX)
    if(NULL == q_malloc_mutex)
    q_malloc_mutex = CreateMutex( NULL, FALSE, NULL );
#elif defined (FEATURE_QUBE)
   int status;
   qmutex_attr_t  qmutexattr;
   qmutexattr.type = QMUTEX_LOCAL;
   
   status = qmutex_create(&q_malloc_mutex, &qmutexattr);
   ASSERT( status == EOK );
#else
  rex_init_crit_sect(&q_malloc_mutex);
#endif //USE_MUTEX
  return;
} /* END init_mutex */

/* ==========================================================================
FUNCTION LOCK_MUTEX
DESCRIPTION
   Locks the mutex 
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
static void lock_mutex( void )
{
#if defined(USE_MUTEX)
   WaitForSingleObject( q_malloc_mutex, INFINITE );
#elif defined(FEATURE_QUBE)
   int status;
   status = qmutex_lock((qmutex_t)q_malloc_mutex);
   ASSERT( status == EOK );
#else
   rex_enter_crit_sect( &q_malloc_mutex );
#endif //USE_MUTEX
    return;
} /* END lock_mutex */

/* ==========================================================================
FUNCTION UNLOCK_MUTEX
DESCRIPTION
   releases the mutex 
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
============================================================================= */
static void unlock_mutex( void )
{
#if defined (USE_MUTEX)
   ReleaseMutex( q_malloc_mutex );
#elif defined (FEATURE_QUBE)
   int status;
   status = qmutex_unlock((qmutex_t)q_malloc_mutex);
   ASSERT( status == EOK );
#else
   rex_leave_crit_sect(&q_malloc_mutex);
#endif //USE_MUTEX
  return;
} /* END unlock_mutex */

/*===========================================================================
FUNCTION Q_MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.

DEPENDENCIES
  To be called only from within q_mem_init_heap()

RETURN VALUE
  None

SIDE EFFECTS
  Initialises the local heap block header

===========================================================================*/
static void q_mem_init_block_header(q_mem_block_header_type *block_ptr,
                                    uint32                   size)
{
    block_ptr->free_flag   = (int8) q_kBlockFree;
    block_ptr->last_flag   = 0;
    block_ptr->pad1        = 0;
    block_ptr->extra       = 0;
    block_ptr->forw_offset = size;
    return;
}

/*===========================================================================
FUNCTION Q_MEM_GET_NEXT_BLOCK

DESCRIPTION
  Return the next block header in the heap for the block following the
  given one.  If the given block is the last block in the heap, return
  the first block in the heap.  Never returns NULL.

  Returns a pointer the the memory block header of the block following the given
  block (or the first block of the heap if the given block was the last
  block of the heap).

DEPENDENCIES
  Heap must be initialised

RETURN VALUE
  Pointer to next block!

SIDE EFFECTS
  None

===========================================================================*/
static q_mem_block_header_type
    *q_mem_get_next_block (const q_mem_heap_type         *heap_ptr,
                           const q_mem_block_header_type *block_ptr)
{
    return block_ptr->last_flag ? heap_ptr->first_block
        : (q_mem_block_header_type *)
        ((int8 *) block_ptr + block_ptr->forw_offset);
}

/*===========================================================================
FUNCTION Q_MEM_FIND_FREE_BLOCK

DESCRIPTION
  Find a free block of at least inSizeNeeded total bytes.  Collapse
  adjacent free blocks along the way.

DEPENDENCIES
  Heap must be initialised

RETURN VALUE
  Returns a pointer to a memory block header describing a free block
  of at least inSizeNeeded total bytes.  Returns NULL if no such free
  block exists or can be created by collapsing adjacent free blocks.

SIDE EFFECTS
  None

===========================================================================*/
static q_mem_block_header_type
    *q_mem_find_free_block(q_mem_heap_type *heap_ptr,
                           uint32           size_needed)
{
    uint32 searchBlocks;

    /* The maximum number of blocks to search.  After searching this
       many, we've been through the heap once and the allocation fails
       if we couldn't find/create a satisfactory free block */

    q_mem_block_header_type *followingBlock;

    /* a loop variable used to walk through the blocks of the heap */
    searchBlocks = heap_ptr->total_blocks;
    followingBlock = q_mem_get_next_block(heap_ptr, heap_ptr->next_block);

    for (; searchBlocks > 0; --searchBlocks, heap_ptr->next_block=followingBlock,
             followingBlock=q_mem_get_next_block(heap_ptr, heap_ptr->next_block))
    {
        if (heap_ptr->next_block->free_flag)
        {
            while (heap_ptr->next_block->forw_offset < size_needed
                   && followingBlock->free_flag && followingBlock > heap_ptr->next_block)
            {
                /* collapse adjacent free blocks into one if it
                 * will allow us to satisfy a request
                 */
                heap_ptr->next_block->forw_offset += followingBlock->forw_offset;
                heap_ptr->next_block->last_flag = followingBlock->last_flag;
                --heap_ptr->total_blocks;
                --searchBlocks; /* don't search more than we need */
                followingBlock = q_mem_get_next_block(heap_ptr, heap_ptr->next_block);
            }
            if (heap_ptr->next_block->forw_offset >= size_needed) {
                return heap_ptr->next_block;
            }
        }
    }
    return 0;
}

/*===========================================================================
FUNCTION Q_MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

DEPENDENCIES
  Heap must be initialised

RETURN VALUE
  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.

SIDE EFFECTS
  Reduces the available heap

===========================================================================*/
static void* q_mem_malloc(q_mem_heap_type *heap_ptr,
                          uint32     size)
{
    uint32 chunks;
    /* the computed minimum size of the memory block in chunks needed
       to satisfy the request */

    uint32 actualSize;
    /* the computed minimum size of the memory block in bytes needed
       to satisfy the request */

    uint8 bonusBytes;
    /* the computed number of unused bytes at the end of the allocated
       memory block.  Will always be < q_kMinChunkSize */

    q_mem_block_header_type *freeBlock;
    /* the free block found of size >= actualSize */

    void *answer = NULL;
    /* the address of memory to be returned to the caller */
    if (!heap_ptr || !size) return NULL;

    ENTER_HEAP_CRITICAL_SECTION();

    chunks = ((uint32) size + sizeof(q_mem_block_header_type)
              + q_kMinChunkSize - 1) / q_kMinChunkSize;
    actualSize = chunks * q_kMinChunkSize;
    bonusBytes = (uint8)
        (actualSize - size - sizeof(q_mem_block_header_type));

    freeBlock = q_mem_find_free_block(heap_ptr, actualSize);
    if (freeBlock)
    {
        /* split the block (if necessary) and return the new block */
        if (freeBlock->forw_offset >
            (actualSize + sizeof(q_mem_block_header_type)))
        {
            /* must split into two free blocks */
            q_mem_block_header_type *newBlock = (q_mem_block_header_type *)
                ((int8 *) freeBlock + actualSize);
            q_mem_init_block_header(newBlock,
                                    freeBlock->forw_offset - actualSize);
            newBlock->last_flag = freeBlock->last_flag;
            freeBlock->forw_offset = actualSize;
            freeBlock->last_flag = 0;
            ++heap_ptr->total_blocks;
        }

        /*  set up next block to search for
            next allocation request */
        heap_ptr->next_block = q_mem_get_next_block(heap_ptr, freeBlock);

        /* mark the block as used and return it */
        freeBlock->free_flag = q_kBlockUsed;
        freeBlock->extra = bonusBytes;
        heap_ptr->used_bytes += size;
        if (heap_ptr->used_bytes > heap_ptr->max_used) {
            heap_ptr->max_used = heap_ptr->used_bytes;
        }
        if (size > heap_ptr->max_request) {
            heap_ptr->max_request = size;
        }
        answer = (int8 *) freeBlock + sizeof(q_mem_block_header_type);
    }

    EXIT_HEAP_CRITICAL_SECTION();

    return answer;
}

/*===========================================================================
FUNCTION Q_MEM_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.

  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Performs initial heap setup

===========================================================================*/
static void q_mem_init_heap(q_mem_heap_type                 *heap_ptr,
                            void                            *heap_mem_ptr,
                            uint32                    heap_mem_size)
{
    int8 *memory_end_ptr;
    /* 1 beyond computed end of memory passed in to use as heap.
     */
    int8 *memory_start_ptr;
    /* The computed beginning of the memory passed in to use as heap.  This
       computed value guarantees the heap actually starts on a paragraph
       boundary.
    */
    uint32 chunks;
    /* How many whole blocks of size q_kMinChunkSize fit in the area of
       memory starting at memory_start_ptr and ending at (memory_end_ptr-1)
    */
    memory_start_ptr = (int8 *)heap_mem_ptr;
    memory_end_ptr   = memory_start_ptr + heap_mem_size;

    /* Advance to the nearest paragraph boundary. This while loop should work
    ** regardless of how many bits are required for address pointers near or
    ** far, etc. */
    while( (((uint32)memory_start_ptr) & 0x000FUL) )
    {
        ++memory_start_ptr;
    }

    chunks = (uint32) ((memory_end_ptr - memory_start_ptr) / q_kMinChunkSize);

    heap_ptr->first_block            = (q_mem_block_header_type *) memory_start_ptr;
    heap_ptr->next_block             = heap_ptr->first_block;
    q_mem_init_block_header(heap_ptr->first_block, chunks * q_kMinChunkSize);
    heap_ptr->first_block->last_flag = (int8) q_kLastBlock;
    heap_ptr->total_blocks           = 1;
    heap_ptr->max_used               = 0;
    heap_ptr->max_request            = 0;
    heap_ptr->used_bytes             = 0;
    heap_ptr->total_bytes            = chunks * q_kMinChunkSize;
    /* initializing the critical section for further use */
    INIT_HEAP_CRITICAL_SECTION();
    return;
}

/*===========================================================================
FUNCTION Q_MALLOC

DESCRIPTION
  Allocates memory from the local heap if AMSS is not running, or the BREW
  heap once AMSS is started.

DEPENDENCIES
  None

RETURN VALUE
  Either a pointer to the area of memory allocated or NULL is no memory
  area is available.

SIDE EFFECTS
  None

===========================================================================*/
void *q_malloc (uint32 size)
{
#ifdef FEATURE_REX_CS
	    /* K1 Routines */
   void *p;
   k1_ErrMalloc(size,&p);
   return p;
    
#else
    static boolean initialised = FALSE;

    if (FALSE == initialised) {
        q_mem_init_heap (&q_heap,
                          q_heap_buffer,
                          Q_HEAP_SIZE);
        initialised = TRUE;
    }

    if (tmc_is_init_done()) {
#ifdef FEATURE_UI_CORE_REMOVED
        return tmc_malloc (size);
#else
  #if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
        return OEM_Malloc (size);
  #else
        ERR_FATAL ("malloc not implemented", 0, 0, 0);
  #endif
#endif
    }
    else {
        q_malloc_max += size;

        /* Silence Lint */
        q_malloc_max = q_malloc_max;

        return (q_mem_malloc (&q_heap, size));
    }
#endif /* FEATURE_REX_CS */
}

/*===========================================================================
FUNCTION Q_FREE

DESCRIPTION
  Free memory from the Brew heap. Any memory allocated from the q_heap will
  be left allocated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void q_free (void *ptr)
{
#ifdef FEATURE_REX_CS
	    /* K1 Routines */
	k1_Free(ptr);
#else
    if ((ptr >= (void *) q_heap_buffer) &&
        (ptr <  (void *)(q_heap_buffer + Q_HEAP_SIZE))) {
        /* Memory was allocated on the q_heap. This memory is currently
         * permanently allocated, therefore do nothing */
    }
    else {
#ifdef FEATURE_UI_CORE_REMOVED
        /* Memory taken from the tmc heap, therefore free it using the
         * tmc heap functions */
        tmc_free (ptr);
#else
  #if defined(FEATURE_BREW_LITE) || defined(FEATURE_BREW)
        /* Memory taken from the Brew heap, therefore free it using the
         * Brew heap functions */
        OEM_Free (ptr);
  #else
        ERR_FATAL ("free not implemented", 0, 0, 0);
  #endif
#endif
    }
#endif /* FEATURE_REX_CS */
}
