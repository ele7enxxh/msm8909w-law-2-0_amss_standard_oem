/* =========================================================================

DESCRIPTION
  Implementation of a simple sub-allocator to manage memory allocations
  and deallocations using a Next Fit strategy.  

Copyright (c) 1997-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/memheap.c_v   1.2   22 Mar 2002 16:54:42   rajeevg  $
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/services/utils/src/memheap_lite.c#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/10/12   rks      Added XOR based guard byte for integrity checks
17/05/12   rks     fix for CR362308
013/04/12  rks     memheap2 
09/03/12   rks     Use OSAL for OS abstraction and remove OS specific code/featurization
08/04/010  sri     Added checks for size variable overflow in mem_malloc()
08/04/009  sri     Added NULL checks where ever applicable
06/01/009  sri     Avoid compiler warnings based on MEMHEAP_ASSERT_NONE
04/14/009  sri     Replaced sprintf, strcat with snprintf, std_strlcat 
                   respectively, memheap_write_data_to_file() is changed to 
                   return void.
03/31/009  sri     qmutex attribute updated for qmutex_create()
09/01/004   gr     Deleted the context argument to the debug versions of
                   the memheap functions; the context is no longer used.
04/06/04    gr     Fixed a bug in the definition of the BEGIN_CRITICAL_SECTION
                   and END_CRITICAL_SECTION macros.
11/25/03    gr     Fixes for compiler warnings when FEATURE_MEM_DEBUG is
                   turned on.
11/18/03    rj     Added check for mem_malloc success before saving header 
                   info.
11/07/03    gr     More debug support.
09/23/03    gr     Added functions to retrieve information about a heap.
09/19/03    gr     Added functionality to store information in flash when
                   an ASSERT fails.
09/17/03    gr     Mainlined FEATURE_HEAP_DEBUG. Added support for storing a
                   filename hash and a line number in the block header.
07/08/03    gr     Simplified mem_heap_task_lock and mem_heap_task_free.
06/26/03    gr     Renamed ASSERT to MEMHEAP_ASSERT. By default, MEMHEAP_ASSERT
                   throws a fatal error and prints out the task id.
06/25/03    gr     Added more ASSERTS. Changed the behavior of
                   FEATURE_MEM_DEBUG to keep just the last n stores and
                   frees. The id of the allocating task is now stored in
                   the info array. Set the block header pad to a non-zero
                   value to try and catch heap corruption.
06/21/03    gr     Added ASSERTS in mem_realloc and mem_free to catch bad
                   pointers passed in.
06/03/03   kjm     Fixed problem in realloc when block is actually grown.
                   If nextBlock is pointing to the free block merged onto
                   the end of the grown block, nextBlock was failing to
                   get updated to point to the following block.
03/11/03    gr     Added support for a heap-specific locking mechanism. This
                   allows some heaps to be restricted to task-context use,
                   and others to be usable from ISRs.
01/20/03   lad     Changed critical section macros to use INTLOCK() instead
                   of TASKLOCK().  Featurized ASSERT calls to optimize.
03/22/02    gr     Corrected a bug in mem_malloc.
10/10/01    gr     BEGIN_CRITICAL_SECTION and END_CRITICAL_SECTION now map
                   to TASKLOCK and TASKFREE, instead of to rex_enter_crit_sect
                   and rex_leave_crit_sect.
10/01/01    gr     Added support for some basic heap statistics.
11/09/00   jct     Revised for coding standard, integrate with REX critical
                   sections
09/09/99   kjm     Added even more ASSERTs to detect corrupt heaps.
04/22/99   kjm     mem_realloc now correctly updates used_bytes, max_used and
                     max_request.
02/17/99   kjm     Added realloc function.  Added support for two
                     variations of heap block headers.  Support backing up
                     and auto-reset when last block is freed to reduce
                     fragmentation.
11/24/98   kjm     Ported to C-only code for use in phone software from
                     original SubAllocator.cp file.
08/22/97   kjm     Original SubAllocator.cp file created.
============================================================================ */


/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */
#include "memheap_lite.h"
#include <stdio.h>
#include <string.h>
//#include "lkmutex.h"
//#ifdef FEATURE_MEMHEAP2_USE_PRNG
//#include "tzbsp_prng.h"
//#endif
//#include "err.h"

//#include "msg.h"
//#include "osal.h"




/* ------------------------------------------------------------------------
** Defines
** ------------------------------------------------------------------------ */
#define OVERFLOW_CHECK(elt_count, elt_size) (!(elt_count >= (1U<<10) || elt_size >= (1U<<22)) || ((((uint64)elt_count * (uint64)elt_size) >> 32) == 0))

#define BOUNDARY_CHECK(theBlock, heap_ptr) ((theBlock >= ((mem_heap_type*)heap_ptr)->first_block) && (theBlock < (((mem_heap_type*)heap_ptr)->first_block + heap_ptr->total_bytes)))

#define FRD_OFFSET_CHECK(block, heap_ptr) ((((mem_block_header_type *)block)->forw_offset + (char *)block) <= ((((char*)((mem_heap_type*)heap_ptr)->first_block) + ((mem_heap_type*)heap_ptr)->total_bytes)))

//#define MEMHEAP_VERIFY_HEADER(block, heap_ptr) (((mem_block_header_type *)block)->header_guard == ((mem_heap_type*)heap_ptr)->block_header_guard ? TRUE : FALSE)

/* XOR based Guard byte calculations and restore */
#define INTEGRITY_CHECK_ON_USED_HEADER(magic_num_used, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_used)


#define INTEGRITY_CHECK_ON_FREE_HEADER(magic_num_free, block) \
            (block[0]^block[1]^block[2]^block[3]^magic_num_free)


#define ADD_GUARD_BYTES_TO_USED_HEADER(magic_num_used, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_used)


#define ADD_GUARD_BYTES_TO_FREE_HEADER(magic_num_free, block) \
            (block[0] = block[1]^block[2]^block[3]^magic_num_free)




#define HEAP_ALIGN	3

/*#define MAX_HEAP_INIT 5
uint32 magic_num[MAX_HEAP_INIT] = {(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1,(uint32)-1};
uint16 magic_num_index_array[MAX_HEAP_INIT] = {0,1,2,3,4};
uint16 magic_num_index = 0; */

#define osal_crit_sect_t int
static int osal_enter_crit_sect(osal_crit_sect_t* critical_section){return 0;};
static int osal_exit_crit_sect(osal_crit_sect_t* critical_section){return 0;};
//static int osal_init_crit_sect(osal_crit_sect_t* critical_section){return 0;};
static int osal_delete_crit_sect(osal_crit_sect_t* critical_section){return 0;};

static mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
);


/* NOTUSED */
#define NOTUSED(i) if(i){}

#define MEMHEAP_MIN_BLOCK_SIZE 16 

/* Code to enter and exit critical sections.
*/
   #define BEGIN_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->lock_fnc_ptr) \
         (heap)->lock_fnc_ptr(heap); \
     } while (0)
   #define END_CRITICAL_SECTION(heap) \
     do { \
       if ((heap)->free_fnc_ptr) \
         (heap)->free_fnc_ptr(heap); \
     } while (0)




static int mem_init_block_header(mem_block_header_type *, unsigned long, mem_heap_type *hep_ptr);

#ifdef FEATURE_MEMHEAP2_USE_PRNG
extern uint32 qsee_prng_getdata(uint8 *out, uint32 out_len);

#endif

#ifdef FEATURE_MEM_DEBUG
#ifndef MEM_HEAP_CALLER_ADDRESS_LEVEL
#define MEM_HEAP_CALLER_ADDRESS_LEVEL 0
#endif
#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS(level) ((void *)__builtin_return_address(level));
#else
 #define MEM_HEAP_CALLER_ADDRESS(level) ((void *) __return_address())
#endif
#endif

 /*static void MEMHEAP_ASSERT_FUNC(const char* format) 
 { 
        TCB_TYPE  self = TCB_SELF();
		ERR_FATAL( "In task 0x%x, Assertion " + format + " failed",, (unsigned long) self, 0, 0 );                 
 }*/

//#define MEMHEAP_ASSERT( xx_exp )
extern void qsee_err_fatal(void);
static void mem_heap_handle_failure()
{
  /* Never return. It is no longer safe for normal execution */
    //while (1);
    qsee_err_fatal();
}
#define OSAL_SUCCESS 0
#define MEMHEAP_ASSERT( xx_exp ) \
    if( !(xx_exp) ) \
    { \
       mem_heap_handle_failure(); \
       return MEMHEAP_FAILURE; \
    } \
    
/* Lock function for Memheap.
*/
static int
mem_heap_enter_crit_sect( void * ptr)
{
  int ret_value;
  ret_value = osal_enter_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
  MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS ); 
  return MEMHEAP_SUCCESS;
} /* END mem_heap_enter_crit_sect */

/* Matching free function for mem_heap_lock_mutex().
*/
static int
mem_heap_leave_crit_sect( void * ptr)
{
  int ret_value;
  ret_value = osal_exit_crit_sect((osal_crit_sect_t*) (((mem_heap_type *)ptr)->memheap_crit_sect));
  MEMHEAP_ASSERT( ret_value == OSAL_SUCCESS ); 
  return MEMHEAP_SUCCESS;
} /* END mem_heap_leave_crit_sect */



/*===========================================================================
FUNCTION mem_heap_get_random_num
DESCRIPTION

===========================================================================*/
int mem_heap_get_random_num(void*  random_ptr, int random_len)
{

#ifdef FEATURE_MEMHEAP2_USE_PRNG
  if(qsee_prng_getdata(((uint8*)random_ptr), random_len)!= random_len)
  {
    MEMHEAP_ASSERT(0);
  }
#else
 if(random_len == 4)
  {
    uint32 *ran_num = (uint32 *)random_ptr; 
    *ran_num = 0xabcddcba;
  } 
  else if(random_len == 2)
  {
    uint16 *ran_num = (uint16 *)random_ptr; 
    *ran_num = 0xabcd;
  }
#endif
  return MEMHEAP_SUCCESS;

}

/*===========================================================================
FUNCTION MEM_GET_NEXT_BLOCK

DESCRIPTION
  Return the next block header in the heap for the block following the
  given one.  If the given block is the last block in the heap, return
  the first block in the heap.  Never returns NULL.

  Returns a pointer the the memory block header of the block following the given
  block (or the first block of the heap if the given block was the last
  block of the heap).

===========================================================================*/
static mem_block_header_type *mem_get_next_block
(
   const mem_heap_type         *heap_ptr,
     /*  The heap the given block belongs to -- may NOT be NULL!
     */
   const mem_block_header_type *block_ptr
     /*  The block in the heap for which to return the following heap
         block
     */
)
{
   uint16 *pblk = NULL;
   mem_block_header_type *nextBlkPtr = NULL;
   MEMHEAP_ASSERT(block_ptr != NULL);
   MEMHEAP_ASSERT(heap_ptr != NULL); //this may be redundent since , it a static function re-visit this for possible optimisation
   MEMHEAP_ASSERT(block_ptr->forw_offset != 0);
   MEMHEAP_ASSERT(FRD_OFFSET_CHECK(block_ptr, heap_ptr));
   MEMHEAP_ASSERT(!((block_ptr->forw_offset)%kMinChunkSize));   
  
   nextBlkPtr =  block_ptr->last_flag  ? heap_ptr->first_block
           : (mem_block_header_type *) ((char *) block_ptr + block_ptr->forw_offset);
   pblk = (uint16*)nextBlkPtr;
   if(nextBlkPtr->free_flag == kBlockFree){
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
   return nextBlkPtr;
} /* END mem_get_next_block */




/*===========================================================================
FUNCTION MEM_INIT_HEAP

DESCRIPTION
  Initializes the heap_ptr object and sets up inMemoryChunk for use with the
  heap_ptr object.  inMemoryChunk may be aligned on any boundary.  Beginning
  bytes will be skipped until a paragraph boundary is reached.  Do NOT pass
  in NULL pointers.  infail_fnc_ptr may be NULL in which case no function will
  be called if mem_malloc or mem_calloc is about to fail.  If infail_fnc_ptr
  is provided, it will be called once and then the allocation will be
  attempted again.  See description of my_allocator_failed_proc for details.
  There is no protection for initializing a heap more than once.  If a heap
  is re-initialized, all pointers previously allocated from the heap are
  immediately invalidated and their contents possibly destroyed.  If that's
  the desired behavior, a heap may be initialized more than once.
===========================================================================*/
/*lint -sem(mem_init_heap,1p,2p,2P>=3n) */
int mem_init_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
   mem_magic_number_struct       *mem_magic_number,
     /* Base pointer for magic number array,index*/
   void                          *heap_mem_ptr,
      /* Pointer to contiguous block of memory used for this heap
      */
   unsigned long                  heap_mem_size
      /* The size in bytes of the memory pointed to by heap_mem_ptr
      */   
)
{
  char *memory_end_ptr;
    /* 1 beyond computed end of memory passed in to use as heap. 
    */
  char *memory_start_ptr;
    /* The computed beginning of the memory passed in to use as heap.  This
       computed value guarantees the heap actually starts on a paragraph
       boundary.
    */
  unsigned long chunks;
    /* How many whole blocks of size kMinChunkSize fit in the area of
       memory starting at memory_start_ptr and ending at (memory_end_ptr-1)
    */
  uint16 * pblk = NULL;

  MEMHEAP_ASSERT(heap_ptr);

  MEMHEAP_ASSERT(mem_magic_number);

  MEMHEAP_ASSERT(mem_magic_number->magic_num);

  MEMHEAP_ASSERT(mem_magic_number->magic_num_index_array);
  
  MEMHEAP_ASSERT(mem_magic_number->magic_num_index < MAX_HEAP_INIT); /* support at the most 30 heaps*/
  
  if( (heap_ptr->magic_num) && 
      (heap_ptr->magic_num == mem_magic_number->magic_num[heap_ptr->magic_num_index])){
  	/* heap is already initialized so just return */
  	return MEMHEAP_SUCCESS;
  }

  memset(heap_ptr, 0, sizeof(mem_heap_type));

  MEMHEAP_ASSERT(heap_mem_ptr);
  MEMHEAP_ASSERT(heap_mem_size);
  MEMHEAP_ASSERT(heap_mem_size >= (2*kMinChunkSize-1));

  memory_start_ptr = (char *)heap_mem_ptr;
  memory_end_ptr   = memory_start_ptr + heap_mem_size;

  //use the memory required for memheap_crit_sect from the heap getting initialized */
 while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;         
  }

  
  

  /*heap_ptr->memheap_crit_sect = memory_start_ptr;

  ret_value = osal_init_crit_sect((osal_crit_sect_t*)heap_ptr->memheap_crit_sect);
  MEMHEAP_ASSERT(ret_value == OSAL_SUCCESS ); */

  /* by default it is critical section */
  heap_ptr->lock_fnc_ptr = mem_heap_enter_crit_sect;
  heap_ptr->free_fnc_ptr = mem_heap_leave_crit_sect;

  /*move the memory start pointer by sizeof(osal_crit_sect_t) as we have used that much out of the heap;*/
  //memory_start_ptr = (char *)memory_start_ptr + sizeof(osal_crit_sect_t);

  /* Advance to the nearest paragraph boundary. This while loop should work
  ** regardless of how many bits are required for address pointers near or
  ** far, etc.
  **
  ** Turn off lint "size incompatibility" warning because cast from pointer
  ** to unsigned long will lose bits, but we don't care because we're only
  ** interested in the least significant bits and we never cast back into a
  ** pointer, so the warning can be safely ignored
  */
  /*lint --e(507)*/
  /*while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;
  }*/
  
  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  heap_ptr->first_block            = (mem_block_header_type *) memory_start_ptr;
  heap_ptr->next_block             = heap_ptr->first_block;

 
  mem_heap_get_random_num((&(mem_magic_number->magic_num[mem_magic_number->magic_num_index_array[mem_magic_number->magic_num_index]])), 4);
  heap_ptr->magic_num = mem_magic_number->magic_num[mem_magic_number->magic_num_index_array[mem_magic_number->magic_num_index]];
  heap_ptr->magic_num_index = mem_magic_number->magic_num_index_array[mem_magic_number->magic_num_index];
  mem_heap_get_random_num(&(heap_ptr->magic_num_free), 2);
  mem_heap_get_random_num(&(heap_ptr->magic_num_used), 2); 
  mem_magic_number->magic_num_index++;  
  mem_init_block_header(heap_ptr->first_block, chunks * kMinChunkSize, heap_ptr);
  heap_ptr->first_block->last_flag = (char) kLastBlock;
  heap_ptr->total_blocks           = 1;
  heap_ptr->max_used               = 0;
  heap_ptr->max_request            = 0;
  heap_ptr->used_bytes             = 0;
  heap_ptr->total_bytes            = chunks * kMinChunkSize;  
  pblk = (uint16*)(heap_ptr->first_block);
  ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
  return MEMHEAP_SUCCESS;
} /* END mem_init_heap */


/*===========================================================================
FUNCTION MEM_DEINIT_HEAP

DESCRIPTION
  De-Initializes the heap_ptr object only if the heap is in reset state.
  User is responsible for freeing all the allocated pointers before  calling 
  into this function.
===========================================================================*/
int mem_deinit_heap(
   mem_heap_type                 *heap_ptr,
      /* Statically allocated heap structure
      */
   mem_magic_number_struct       *mem_magic_number
)
{
  int ret_value;

  MEMHEAP_ASSERT(mem_magic_number);

  MEMHEAP_ASSERT(mem_magic_number->magic_num);

  MEMHEAP_ASSERT(mem_magic_number->magic_num_index_array);
  // return the magic number
  mem_magic_number->magic_num_index--;
  if(mem_magic_number->magic_num_index < MAX_HEAP_INIT)
  {
	  mem_magic_number->magic_num_index_array[mem_magic_number->magic_num_index]=heap_ptr->magic_num_index;
  }
 /* De-initialize heap only if all the allocated blocks are freed */
 if(heap_ptr->used_bytes == 0)
 {
   ret_value = osal_delete_crit_sect((osal_crit_sect_t*)heap_ptr->memheap_crit_sect);
   MEMHEAP_ASSERT(ret_value == OSAL_SUCCESS ); 
   memset(heap_ptr, 0, sizeof(mem_heap_type));
 }
 else
 {
	MEMHEAP_ASSERT(heap_ptr->used_bytes == 0);
 }
 return MEMHEAP_SUCCESS;
}
/*===========================================================================
FUNCTION MEM_INIT_BLOCK_HEADER

DESCRIPTION
  Initializes a memory block header to control a block of memory in the
  heap.  The header may still need to some of its fields adjusted after
  this call if it will be a used block or the last block in the heap.
===========================================================================*/
int mem_init_block_header(
   mem_block_header_type *block_ptr,
     /* Memory header block to be initialized 
     */
   unsigned long          size,
     /* The size of the block of memory controlled by this
        memory header block INCLUDING the size of the
        header block itself
     */
     mem_heap_type *heap_ptr
)
{
  char *p_temp;
  MEMHEAP_ASSERT(block_ptr);
  p_temp = ((char*)block_ptr);
  memset(p_temp, 0 , sizeof(mem_block_header_type));
  block_ptr->free_flag   = (char) kBlockFree;
  block_ptr->forw_offset = size;

  return MEMHEAP_SUCCESS;
} /* END mem_init_block_header */


/*===========================================================================
FUNCTION MEM_MALLOC

DESCRIPTION
  Allocates a block of size bytes from the heap.  If heap_ptr is NULL
  or size is 0, the NULL pointer will be silently returned.

  Returns a pointer to the newly allocated block, or NULL if the block
  could not be allocated.
===========================================================================*/
/*lint -sem(mem_malloc,1p,2n>=0&&(@p==0||@P==2n)) */
void* mem_malloc(
  mem_heap_type *heap_ptr,
     /* Heap from which to allocate
     */
  mem_magic_number_struct       *mem_magic_number,
  unsigned int         size
     /* Number of bytes to allocate
     */
)
{  

  unsigned long chunks;
    /* the computed minimum size of the memory block in chunks needed
       to satisfy the request */

  unsigned long actualSize;
    /* the computed minimum size of the memory block in bytes needed
       to satisfy the request */

  unsigned char bonusBytes;
    /* the computed number of unused bytes at the end of the allocated
       memory block.  Will always be < kMinChunkSize */

  mem_block_header_type *freeBlock = NULL;
    /* the free block found of size >= actualSize */

  void *answer = NULL;
    /* the address of memory to be returned to the caller */

  uint16 * pblk = NULL;
  uint32 blockHeaderSize=sizeof(mem_block_header_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  MEMHEAP_ASSERT(heap_ptr != NULL); 

  MEMHEAP_ASSERT(mem_magic_number);

  MEMHEAP_ASSERT(mem_magic_number->magic_num);

  MEMHEAP_ASSERT(mem_magic_number->magic_num_index_array);

  if (!size) return NULL;

  
  
  /* quick check if requested size of memory is available */
  if( (unsigned long) size > heap_ptr->total_bytes ) return NULL;

  /* chunks overflow check : check max memory that can be malloc'd at a time */
  if( (0xFFFFFFFF - ( kMinChunkSize + sizeof(mem_block_header_type)) ) 
  	    < ((unsigned long) size)) return NULL;

#if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
	if(size <= 4)
	{
		blockHeaderSize=blockHeaderSize-4;
	}
	
#endif


  chunks = ((unsigned long) size + blockHeaderSize
            + kMinChunkSize - 1) / kMinChunkSize;
  actualSize = chunks * kMinChunkSize;
  bonusBytes = (unsigned char)
                (actualSize - size - blockHeaderSize);  
    
    BEGIN_CRITICAL_SECTION(heap_ptr);


    MEMHEAP_ASSERT(heap_ptr->magic_num == mem_magic_number->magic_num[heap_ptr->magic_num_index]);   
    freeBlock = mem_find_free_block(heap_ptr, actualSize);
    

    if (freeBlock)
    {
      /* split the block (if necessary) and return the new block */
 
      MEMHEAP_ASSERT(freeBlock->forw_offset > 0);
      // frd offset and actual size are chunk aligned 
      if (freeBlock->forw_offset > actualSize)
      {
        /* must split into two free blocks */

 
        mem_block_header_type *newBlock = (mem_block_header_type *)
                                          ((char *) freeBlock + actualSize);
        mem_init_block_header(newBlock, freeBlock->forw_offset - actualSize, heap_ptr);
        newBlock->last_flag = freeBlock->last_flag;
        freeBlock->forw_offset = actualSize;
        freeBlock->last_flag = 0;


        ++heap_ptr->total_blocks;        
         pblk = (uint16*)newBlock;
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);
      }
      

      /* mark the block as used and return it */
  
      freeBlock->free_flag = kBlockUsed;
      freeBlock->extra = bonusBytes;      
      /*  set up next block to search for
          next allocation request */     
      heap_ptr->next_block = mem_get_next_block(heap_ptr, freeBlock);
       
       
      heap_ptr->used_bytes += size;
      MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
      
      if (heap_ptr->used_bytes > heap_ptr->max_used) {
        heap_ptr->max_used = heap_ptr->used_bytes;
      }
      if (size > heap_ptr->max_request) {
        heap_ptr->max_request = size;
      }
    
	  pblk = (uint16*)freeBlock;
      ADD_GUARD_BYTES_TO_USED_HEADER(heap_ptr->magic_num_used, pblk);
      answer = (char *) freeBlock + blockHeaderSize;
    }

#ifdef FEATURE_MEM_DEBUG
    if(answer != NULL)
    {
      MEMHEAP_ASSERT( freeBlock != NULL );
      freeBlock->caller_ptr=MEM_HEAP_CALLER_ADDRESS(MEM_HEAP_CALLER_ADDRESS_LEVEL);            
    }
#endif

    END_CRITICAL_SECTION(heap_ptr);

  return answer;
} /* END mem_malloc */


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
int mem_free(
  mem_heap_type *heap_ptr,
     /* Heap in which to free memory
     */
  mem_magic_number_struct       *mem_magic_number,
  void          *ptr
     /* Memory to free
     */
)
{
  mem_block_header_type *theBlock;
    /* The computed address of the memory header block in the heap that
       controls the memory referenced by ptr */ 

  uint32          sizeBlockHeader=sizeof(mem_block_header_type);

  uint16 *pblk = NULL;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  MEMHEAP_ASSERT(heap_ptr != NULL);

  MEMHEAP_ASSERT(mem_magic_number);

  MEMHEAP_ASSERT(mem_magic_number->magic_num);

  MEMHEAP_ASSERT(mem_magic_number->magic_num_index_array);
  /*commented out the below assert since NULL free occurances are found and once 
   corrsponding fixes are in its need to be uncomment*/
  MEMHEAP_ASSERT(heap_ptr->magic_num == mem_magic_number->magic_num[heap_ptr->magic_num_index]);
 
 

  /*commented out the below assert since NULL free occurances are found and once 
   corrsponding fixes are in its need to be uncomment*/
  if(ptr == NULL)
  {    
     //MEMHEAP_ERROR(" NULL ptr occurenaces in mem_free()",0,0,0);
    //MEMHEAP_ASSERT(0);

    return MEMHEAP_SUCCESS;
  }

  
  /* free the passed in block */

  MEMHEAP_ASSERT(heap_ptr->first_block);
  BEGIN_CRITICAL_SECTION(heap_ptr);
  
 
  theBlock = (mem_block_header_type *)
                          ((char *) ptr - sizeBlockHeader);
  // now we have to make a guess that size of (mem_block_header_type) is 12 or 16
#if defined FEATURE_MEM_DEBUG && defined FEATURE_QDSP6
	if ( theBlock->header_guard != heap_ptr->block_header_guard )
	{
		theBlock=(mem_block_header_type *)((char *)theBlock+4);	
    sizeBlockHeader=sizeBlockHeader-4;
	}
#endif

  //check for block alignment to 16
  MEMHEAP_ASSERT((((uint32)theBlock)%kMinChunkSize) == 0);
  
  
  /* boundry check for the ptr passed to free */
  MEMHEAP_ASSERT(BOUNDARY_CHECK(theBlock, heap_ptr));


 

  /* Try to detect corruption. */

 
  MEMHEAP_ASSERT(!theBlock->free_flag);  /* Attempt to detect multiple
                                            frees of same block */
  /* Make sure forw_offset is reasonable */
  MEMHEAP_ASSERT(theBlock->forw_offset >= sizeBlockHeader);
  /* Make sure extra is reasonable */
  MEMHEAP_ASSERT(theBlock->extra < kMinChunkSize);
 
  /* Make sure forw_offset is not spiling over the heap boundry */
  MEMHEAP_ASSERT(FRD_OFFSET_CHECK(theBlock, heap_ptr));
  
 //check for heap canary
  pblk = (uint16*)(theBlock);
  MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));

  

  if (!theBlock->free_flag) /* Be intelligent about not munging the heap if
                              a multiple free of the same block is detected */
  {

   
    MEMHEAP_ASSERT((theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra) <= heap_ptr->used_bytes);
    heap_ptr->used_bytes -= theBlock->forw_offset - sizeBlockHeader
                                  - theBlock->extra;
    MEMHEAP_ASSERT(heap_ptr->total_bytes >= heap_ptr->used_bytes);
    
    theBlock->free_flag = (char) kBlockFree;

   

  
    /* now backup the next pointer if applicable */
    //next_block = mem_get_next_block(heap_ptr, theBlock);

    if (theBlock < heap_ptr->next_block) {
      /* Backup now to lessen possible fragmentation */
      heap_ptr->next_block = theBlock;
    }   
	  pblk = (uint16*)theBlock;
    ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);

    /* reset heap to initial state if everything is now freed */
    if (!heap_ptr->used_bytes) {

      /* reset heap now, but retain statistics */
      heap_ptr->next_block = heap_ptr->first_block;
      mem_init_block_header(heap_ptr->first_block, heap_ptr->total_bytes, heap_ptr);
      heap_ptr->first_block->last_flag = (char) kLastBlock;
      heap_ptr->total_blocks = 1;
	   pblk = (uint16*)(heap_ptr->first_block);
      ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);          
    }
	}
  
     END_CRITICAL_SECTION(heap_ptr);
     return MEMHEAP_SUCCESS;
} /* END mem_free */

/*===========================================================================
FUNCTION MEM_FIND_FREE_BLOCK

DESCRIPTION
  Find a free block of at least inSizeNeeded total bytes.  Collapse
  adjacent free blocks along the way.

  Returns a pointer to a memory block header describing a free block
  of at least inSizeNeeded total bytes.  Returns NULL if no such free
  block exists or can be created by collapsing adjacent free blocks.
===========================================================================*/
mem_block_header_type *mem_find_free_block(
   mem_heap_type *heap_ptr,
     /*  The heap to search for a free block
     */
   unsigned long  size_needed
     /*  The minimum size in bytes of the block needed (this size
         INCLUDES the size of the memory block header itself)
     */
)
{
  long searchBlocks;
    /* The maximum number of blocks to search.  After searching this
       many, we've been through the heap once and the allocation fails
       if we couldn't find/create a satisfactory free block */

  mem_block_header_type *followingBlock;
    /* a loop variable used to walk through the blocks of the heap */
  uint16 *pblk = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  MEMHEAP_ASSERT(heap_ptr);
  MEMHEAP_ASSERT(heap_ptr->first_block);
  /* sanity check for the  heap_ptr->next_block  */
  if((heap_ptr->next_block)->free_flag == kBlockFree){
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk));
   }
   else{
      pblk = (uint16*)(heap_ptr->next_block);
      MEMHEAP_ASSERT(!INTEGRITY_CHECK_ON_USED_HEADER(heap_ptr->magic_num_used, pblk));
   }
        
  searchBlocks = (long) heap_ptr->total_blocks;

  followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);
  
  for (; searchBlocks > 0; --searchBlocks, heap_ptr->next_block=followingBlock,
                  followingBlock=mem_get_next_block(heap_ptr, heap_ptr->next_block))
  {
    MEMHEAP_ASSERT(heap_ptr->next_block->forw_offset > 0);
    MEMHEAP_ASSERT(followingBlock->forw_offset > 0);

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
        pblk = (uint16*) (heap_ptr->next_block);
        ADD_GUARD_BYTES_TO_FREE_HEADER(heap_ptr->magic_num_free, pblk);

        --heap_ptr->total_blocks;
        --searchBlocks; /* don't search more than we need */
		//memset the following block header information to 0 for safe unlinking
		{
             uint32 *temp = (uint32*)followingBlock; /*did it like this for optimization purpose*/
             temp[0] = NULL;
             temp[1] = NULL;
        }
        followingBlock = mem_get_next_block(heap_ptr, heap_ptr->next_block);

      }
      if (heap_ptr->next_block->forw_offset >= size_needed) {
        return heap_ptr->next_block;
      }
    }
  }
  return MEMHEAP_FAILURE;       /* didn't find anything */
} /* END mem_find_free_block */
