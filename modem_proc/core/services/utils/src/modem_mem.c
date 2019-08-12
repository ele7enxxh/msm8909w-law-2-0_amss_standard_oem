/*!
  @file
  modem_mem.c

  @brief
  Implementation of a heap manager to manage memory allocations
  and deallocations using a Best-Fit strategy.

*/

/*===========================================================================

  Copyright (c) 2009-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/modem_mem.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/20/15   ps      FR29424-CR911469- Memheap quota support
11/12/14   ps      Remove Debug Array
01/07/14   rks     CR491964:Add dynamic sections support in memheap
12/12/13   rks     CR586541:Add a symbol that stores the Modem mem heap size into the ELF,
                   for memory calculation purposes
02/05/13   ps      Add debug information to the block header
12/06/11   rs      Klocwork Errors removed 
12/01/11   rs      Minor change: Added ifdef FEATURE_QDSP6 :CR318841
10/11/11   ada     Fix size check, Add function modem_mem_FatalDump to dump headers in a dynamic buffer in case of crash
10/04/11   ch      Check if size is corrupted 
09/01/11   rs      Minor fix for compilation errors in MOBs.
08/10/11   rs      Added Safe Unlinking during unlinking of free block (CR298163) 
08/10/11   rs      Added fix for CR 276505 - Fix for detecting corrupt ClientID 
                   while accessing Client Memory Budget 
08/10/11   rs      Added fix for CR 276504 - Fix for potential INT overflow 
                   of extra field or old size of block during Realloc 
08/10/11   rs      Added fix for CR 276478 - Fix for potential INT overflow 
                   of block size 
08/10/11   rs      Added fix for CR 274236 - Check ClientID before updating 
                   Client Memory Usage	 
03/09/10   ic      Multi-section supports
06/07/09   ic      Added extra debugging supports - heap dumps and logging
04/24/09   ic      Added new fields - client & flags - in modem_mem_block_s
                   Removed two fields - last & free - in modem_mem_blocks_s,
                   and moved them to the new field
03/16/09   ic      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>           /* Definition for basic types and macros */
#include "modem_mem.h"        /* External interface to modem_mem.c */
#include "memheap.h"
#include "PrngML.h"
#include "memheap_v.h"

#ifdef FEATURE_MEM_DEBUG
#include "rcinit.h"
#endif
mem_heap_type modem_mem_heap;
static unsigned int is_heap_initialized=0;
static volatile unsigned int modem_mem_heap_size = MODEM_MEM_HEAP_SIZE; //CR586541
static void *random_alloc_ptr=NULL;
#define MODEM_MEM_RANDOM_ALLOC_MASK 0x1FFFF

#ifdef FEATURE_MEM_DEBUG
#ifndef MODEM_HEAP_CALLER_LEVEL
#define MODEM_HEAP_CALLER_LEVEL     (0)
#endif

#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(MODEM_HEAP_CALLER_LEVEL));
#else
 #define MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif

#endif

/* Heap allocation failure constants */
#ifndef MODEMHEAP_FAIL_ARRAY_SIZE
#define MODEMHEAP_FAIL_ARRAY_SIZE  2
#endif

int modemheap_fail_index = 0;

/* Array used to log heap allocation failures */
memheap_v_info_s_type modemheap_fail_info[MODEMHEAP_FAIL_ARRAY_SIZE];
#ifdef FEATURE_MEM_DEBUG
static uint32 modem_mem_get_task_quota(void);
#endif

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief
** Contains the information for a memory section input configuration
*/
typedef struct _modem_mem_section_cfg
{
   byte                 *start;       /*!< start address of section */
   uint32                size;        /*!< size of the section */
} modem_mem_section_cfg;


#ifdef FEATURE_MEM_DEBUG
/* AMSS memory debug functions */
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);
memheap_task_stats_type modem_heap_task_stats[MEMHEAP_MAX_THREADS];
extern uint32 memheap_global_error;
#endif  /* FEATURE_MEM_DEBUG */

#ifdef FEATURE_ENABLE_HEAP_PROFILING
extern memheap_profile_table_type memheap_profile_table[50];
#endif
extern void mem_heap_add_section(
     mem_heap_type *heap_ptr, 
     void *section_start, 
     unsigned long section_size);

extern void mem_init_heap_alloc(
   mem_heap_type                 *heap_ptr,
   unsigned long                  heap_mem_size,
   mem_allocator_failed_proc_type fail_fnc_ptr
);


void modemheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
);
/*===========================================================================

  FUNCTION:  modem_mem_validate_sections

===========================================================================*/
/*!
    @brief
    Validates and arranges the memory sections available for the Modem Heap

    @return
    None
*/
/*=========================================================================*/
void modem_mem_validate_sections(
  modem_mem_section_cfg mem_sections[]   /*!< array of available memory
                                              sections */
)
{
  /*! @brief temporary buffer for modem heap section entry
  */
  modem_mem_section_cfg t_section;
  /*! @brief for loop index
  */
  int i, j;

  /*-----------------------------------------------------------------------*/

  for (i = 0; mem_sections[i].start != 0x0; ++i) 
  {

    for (j = i + 1; mem_sections[j].start != 0x0; ++j) 
    {
      if (mem_sections[i].start > mem_sections[j].start) 
      {
        t_section = mem_sections[i];
        mem_sections[i] = mem_sections[j];
        mem_sections[j] = t_section;
      }
    }
  }
  return;

} /* modem_mem_validate_sections() */

/*===========================================================================

  FUNCTION:  modem_mem_init_heap

===========================================================================*/
/*!
    @brief
    Initializes a heap using given memory sections.

    @detail
    Initializes a heap using the memory sections given.

    @return
    None

    @note
    The modem heap clients should NOT call this function; this funcion is 
    called only once during the system initialization.
*/
/*=========================================================================*/

static void modem_mem_init_heap(void)
{
  /*! @brief 
  **  Modem Central heap buffer area
  */
  uint32 random_allocation_size=0;
  int j;

  /*! @brief array of free memory sections
  */
  modem_mem_section_cfg sections[] = 
  {
  /* It's important to keep T_WINNT checking here; on QTF builds the target
   * specific modem heap configuration should NOT be used 
   */
  #ifndef T_WINNT
    #ifdef MODEM_MEM_SECTION_BASE0
    { (byte *) MODEM_MEM_SECTION_BASE0, MODEM_MEM_SECTION_SIZE0 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE1
    { (byte *) MODEM_MEM_SECTION_BASE1, MODEM_MEM_SECTION_SIZE1 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE2
    { (byte *) MODEM_MEM_SECTION_BASE2, MODEM_MEM_SECTION_SIZE2 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE3
    { (byte *) MODEM_MEM_SECTION_BASE3, MODEM_MEM_SECTION_SIZE3 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE4
    { (byte *) MODEM_MEM_SECTION_BASE4, MODEM_MEM_SECTION_SIZE4 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE5
    { (byte *) MODEM_MEM_SECTION_BASE5, MODEM_MEM_SECTION_SIZE5 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE6
    { (byte *) MODEM_MEM_SECTION_BASE6, MODEM_MEM_SECTION_SIZE6 },
    #endif
    #ifdef MODEM_MEM_SECTION_BASE7
    { (byte *) MODEM_MEM_SECTION_BASE7, MODEM_MEM_SECTION_SIZE7 },
    #endif
  #endif /* T_WINNT */
    { NULL,                    0                       }
  };
  modem_mem_heap_size = MODEM_MEM_HEAP_SIZE;
  memset(&modem_mem_heap, 0, sizeof(mem_heap_type));
  mem_init_heap_alloc(&modem_mem_heap,MODEM_MEM_HEAP_SIZE,&modemheap_malloc_failed);
  modem_mem_validate_sections(sections);
  mem_init_heap(&modem_mem_heap,sections[0].start,sections[0].size,&modemheap_malloc_failed);
  for (j = 0; (j < (sizeof(sections)/sizeof(modem_mem_section_cfg))) && (sections[j].start != 0x0); ++j)   
  {
    mem_heap_add_section(&modem_mem_heap,sections[j].start, sections[j].size);
  }

   // Do a random allocation and add it back when heap gets exhaused
   //we can get uint32 random number which can randomize the LSB 32 bits
   //MAsk the random number so that we can get max 128k allocation
  if(PRNGML_ERROR_NONE != PrngML_getdata(((uint8*)&random_allocation_size), 4))
  {
     ASSERT(0);
  }
  random_allocation_size=random_allocation_size&MODEM_MEM_RANDOM_ALLOC_MASK; 

  if (random_allocation_size != 0)
  {
     random_alloc_ptr=modem_mem_alloc(random_allocation_size,0);
     if (random_alloc_ptr == NULL)
     {
        ASSERT(0);
     }
  }

#ifdef FEATURE_MEM_DEBUG
   if(MEM_TLS_CREATE_ERR != memheap_global_error)
   {
      modem_mem_heap.heapStatistics = &modem_heap_task_stats[0];
   }
   modem_mem_heap.mem_get_quota_fnc_ptr = modem_mem_get_task_quota;
#endif

  return;
} /* modem_mem_init_heap() */


/*===========================================================================

  FUNCTION:  modem_mem_check_heap_initialized

===========================================================================*/
/*!
    @brief
    Checks to see if the heap is initialized. If not, this function 
    initializes the heap.

    @return
    None
*/
/*=========================================================================*/
static void modem_mem_check_heap_initialized(void)
{
  /* Immediately return if the heap is initialied; except for the very 
  ** first function call, it will return from here
  */
  if (is_heap_initialized) 
  {
    return;
  }

  
  if (!(is_heap_initialized))
  {
	is_heap_initialized=1;
    modem_mem_init_heap();    
  }
  return;
} /* modem_mem_check_heap_initialized() */


/*===========================================================================

  FUNCTION:  modem_mem_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* modem_mem_alloc(
  size_t                size,       /*!< Number of bytes to allocate */
  modem_mem_client_e    client      /*!< Client ID for this mem block */
)
{
  void *ptr = NULL;

  /*-----------------------------------------------------------------------*/

  if (!size) 
  {
    return NULL;  /* Zero size request - return NULL immediately */
  }

  /*-----------------------------------------------------------------------*/

  modem_mem_check_heap_initialized();

  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 malloc_inTime=0, malloc_outTime=0, diff =0;
	malloc_inTime = timetick_get_safe();
    #endif


  ptr=mem_malloc(&modem_mem_heap,size);
  
	#ifdef FEATURE_ENABLE_HEAP_PROFILING
	malloc_outTime = timetick_get_safe();
	diff = malloc_outTime - malloc_inTime;
    memheap_profile_table[modem_mem_heap.debug_heap_index].malloc_avg+= diff;
    memheap_profile_table[modem_mem_heap.debug_heap_index].malloc_cnt++;
	#endif

#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);
  }
#endif
  return ptr;
 
} /* modem_mem_alloc() */

void* modem_mem_alloc_setcaller(
  size_t               size,       /*!< Number of bytes to allocate */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
  ptr=modem_mem_alloc(size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,size);
#endif
  return ptr;
}

/*===========================================================================

  FUNCTION:  modem_mem_calloc

===========================================================================*/
/*!
    @brief
    Allocates enough space for elt_count elements each of elt_size bytes
    from the heap and initializes the space to nul bytes.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated. If elt_count or elt_size is 0, the NULL pointer 
    will be silently returned.
*/
/*=========================================================================*/

void* modem_mem_calloc(
  size_t               elt_count,  /*!< Number of elements to allocate */
  size_t               elt_size,   /*!< Size of each element*/
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr=NULL;
  modem_mem_check_heap_initialized();
  ptr=mem_calloc(&modem_mem_heap,elt_count, elt_size);
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);
  }
  #endif
  return ptr;
} /* modem_mem_calloc() */

void* modem_mem_calloc_setcaller(
  size_t              elt_count,  /*!< Number of elements to allocate */
  size_t              elt_size,   /*!< Size of each element*/
  modem_mem_client_e  client,      /*!< Client ID for this mem block */
  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr = NULL;
  ptr=modem_mem_calloc(elt_count,elt_size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_mem_heap,ptr, caller_ptr,elt_count * elt_size);
#endif
  return ptr;
}
/*===========================================================================

  FUNCTION:  modem_mem_realloc

===========================================================================*/
/*!
    @brief
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.

    @detail
    Resizes the ptr block of memory to be size bytes while preserving the
    block's contents.  If the block is shortened, bytes are discarded off the
    end.  If the block is lengthened, the new bytes added are not initialized
    and will have garbage values.  
    If ptr is NULL, the function behaves exactly like modem_mem_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like modem_mem_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call modem_mem_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *modem_mem_realloc(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client      /*!< Client ID for this mem block */
)
{
  void *ptr_ret=NULL;
  modem_mem_check_heap_initialized();
  
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 realloc_inTime=0 , realloc_outTime=0;
	realloc_inTime = timetick_get_safe();
  #endif
  ptr_ret=mem_realloc(&modem_mem_heap, ptr,size);
   
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
    realloc_outTime = timetick_get_safe( );
    memheap_profile_table[modem_mem_heap.debug_heap_index].realloc_avg = realloc_outTime - realloc_inTime;
    memheap_profile_table[modem_mem_heap.debug_heap_index].realloc_cnt++;
  #endif
#ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);
    }
  #endif
  return ptr_ret;
} /* modem_mem_realloc() */

void* modem_mem_realloc_setcaller(
  void                *ptr,        /*!< A block previously allocated 
                                        from heap */
  size_t               size,       /*!< New size (in bytes) of the ptr block
                                        of memory */
  modem_mem_client_e   client,      /*!< Client ID for this mem block */

  void*                caller_ptr   /*!< Caller Ptr to be set in debug information */
)
{
  void *ptr_ret=NULL;
  ptr_ret=modem_mem_realloc(ptr,size,client);
#ifdef FEATURE_MEM_DEBUG
  memdebug_updatecallerptr(&modem_mem_heap,ptr_ret, caller_ptr,size);
#endif
  return ptr_ret;
}

/*===========================================================================

  FUNCTION:  modem_mem_free

===========================================================================*/
/*!
    @brief
    Deallocates the ptr block of memory. 

    @detail
    Deallocates the ptr block of memory.  If ptr is NULL, or ptr is outside 
    the range of memory managed by heap, then this function call does nothing
    (and is guaranteed to be harmless).

    @return
    None.
*/
/*=========================================================================*/
void modem_mem_free(
  void                *ptr,        /*!< Memory to free */
  modem_mem_client_e   client      /*!< User ID for this mem block */
)
{
#ifdef FEATURE_ENABLE_HEAP_PROFILING
    uint32 free_inTime=0, free_outTime=0;
	free_inTime = timetick_get_safe();
  #endif
  mem_free(&modem_mem_heap,ptr);
#ifdef FEATURE_ENABLE_HEAP_PROFILING
    free_outTime = timetick_get_safe();
    memheap_profile_table[modem_mem_heap.debug_heap_index].free_avg += (free_outTime - free_inTime);
    memheap_profile_table[modem_mem_heap.debug_heap_index].free_cnt++;
  #endif 
} /* modem_mem_free() */


/*===========================================================================

  FUNCTION:  modem_mem_add_region

===========================================================================*/
/*!
    @brief
    Add a memory region to the modem heap

    @detail
    This API can be used to add extra left over memory that can be used 
    in the modem heap. 

    @return
    None.
*/
/*=========================================================================*/

void modem_mem_add_region(void *section_start, unsigned long section_size)
{
   mem_heap_add_section(&modem_mem_heap,section_start,section_size);
}

/*===========================================================================

  FUNCTION:  modem_mem_get_section_info

===========================================================================*/
/*!
    @brief
    Get the section information about modem heap

    @detail
   

    @return
    None.
*/
/*=========================================================================*/

void modem_mem_get_section_info(void **section_info)
{
	*section_info=(void *)&modem_mem_heap.sections[0];
}

/*===========================================================================

FUNCTION modemheap_malloc_failed

DESCRIPTION
  This function logs heap statistics, and is called when memheap detects
  an allocation failure from the Modem heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void modemheap_malloc_failed
(
  struct mem_heap_struct *heap_ptr, /* Heap on which allocation is attempted */
  size_t size                       /* Size of failed request */
)
{
  memheap_v_info_s_type *info;

  /* Record the current heap state */
  if (modemheap_fail_index < ARR_SIZE(modemheap_fail_info))
  {
    int i, freeBlk_list_idx = 0;
    unsigned long  totalFreeBlocks = 0;
    unsigned long  totalSizeOfFreeBlocks=0;
    freeBlockList *temp;
    uint16* pblk;
    info = &modemheap_fail_info[modemheap_fail_index];
    (info->totals).largest_free_block=0;
    for (i= 0; i< MEMHEAP2_BIN_COUNT; i++)
    {
       totalFreeBlocks += heap_ptr->BIN[i].totalNodes;
     }

     for (i = MEMHEAP2_BIN_COUNT-1; i>=0; i--)
     {
         if((heap_ptr->BIN[i].firstNode)!=0){
           temp = heap_ptr->BIN[i].firstNode;

           pblk =  (uint16*)temp;
           ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));

           if((info->totals).largest_free_block == 0){
              if((heap_ptr->BIN[i].lastNode)!=0){
                   uint16* pblk2  = (uint16*)(heap_ptr->BIN[i].lastNode);
                   ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk2)));
                  (info->totals).largest_free_block = ((mem_block_header_type*)(heap_ptr->BIN[i].lastNode))->forw_offset;
              }
           }

           totalSizeOfFreeBlocks+=((mem_block_header_type*)temp)->forw_offset;
           if(MEMHEAP_V_INFO_FREEBLOCKS > freeBlk_list_idx)
           {
               info->freeblocks[freeBlk_list_idx] = ((mem_block_header_type*)temp)->forw_offset;
               freeBlk_list_idx++;
           }
           
           while(temp->nextPtr)
           {
               pblk =  (uint16*)(temp->nextPtr);
               ASSERT(!(INTEGRITY_CHECK_ON_FREE_HEADER(heap_ptr->magic_num_free, pblk)));
               if(MEMHEAP_V_INFO_FREEBLOCKS > freeBlk_list_idx)
               {
                   info->freeblocks[freeBlk_list_idx] = NEXT_BLOCK_SIZE(temp);
                   freeBlk_list_idx++;
               }
               totalSizeOfFreeBlocks+=NEXT_BLOCK_SIZE(temp);
               temp = temp->nextPtr;
           }
         }
     }

    
    info->heap_ptr = heap_ptr;
    info->size = size;

    (info->totals).total_physical_bytes = heap_ptr->total_bytes; 
    (info->totals).current_block_count = heap_ptr->total_blocks;
    (info->totals).used_blocks = heap_ptr->total_blocks - totalFreeBlocks;
  
    (info->totals).used_bytes = heap_ptr->used_bytes;
    (info->totals).free_bytes = totalSizeOfFreeBlocks;
    (info->totals).max_logical_used = heap_ptr->max_used;
    (info->totals).max_logical_request = heap_ptr->max_request;
  }
  modemheap_fail_index++;

  MSG_ERROR("mdoem heap failed to allocate %d bytes (failure %d)",
    size, modemheap_fail_index, 0);
}


#ifdef FEATURE_MEM_DEBUG
static uint32 modem_mem_get_task_quota(void)
{
   RCINIT_INFO rc_handle;
   RCINIT_HEAPQSZ rc_heap_quota=0;

   rc_handle = rcinit_lookup_self();
   if(NULL != rc_handle)
   {
       rc_heap_quota = rcinit_lookup_heapqsz_info(rc_handle,&modem_mem_heap);
   }

   return rc_heap_quota;
}
#endif
