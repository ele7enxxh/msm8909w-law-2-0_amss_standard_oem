/* =========================================================================

                              A M S S   H E A P

DESCRIPTION
  Wrapper for malloc, realloc, calloc and free calls for AMSS_HEAP.

Copyright (c) 2006-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================ */

/* =========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/amssheap.c#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
09/20/15   ps      FR29424-CR911469- Memheap quota support
11/12/14   ps      Remove Debug Array
01/15/14   rks     CR578502:dynamic heap design(heap sharing)
01/07/14   rks     CR491964:Add dynamic sections support in memheap
12/03/13   rks     CR451394:random virtual address mapping for heaps
09/07/2012 mg      modem heap using memheap
08/30/12  rks      robust XOR logic and enable PRNG uses
07/19/12  rks      Guard customer.h   under #ifdef CUST_H
05/10/12  rks      memheap2 
12/05/07   jc      Added periodic defragmentation feature.
08/15/07   jc      Added dynamic heap feature.
08/29/06   jc      Added heap debug support.
07/03/06   jc      Added heap init checks and malloc failure logging.
03/27/06   jhs     File created
============================================================================ */


/*============================================================================
                             INCLUDE FILES
============================================================================*/
#include "core_variation.h"
#include "memheap.h"
#include "memheap_v.h"
#include "msg.h"
#include <stdlib.h>

#ifdef FEATURE_QDSP6
#ifdef FEATURE_QURT
#include <qurt.h>
#include <qurt_types.h>

#else
#ifdef FEATURE_BLAST
#include <blast.h>
#endif
#endif
#endif

#include "osal.h"
#include "timetick.h"

#ifdef FEATURE_ENABLE_HEAP_PROFILING
#include "timer.h"
#ifndef FEATURE_QDSP6
#include "DDIInterruptController.h"
#include "DALStdErr.h"
#include "DALDeviceID.h"
#include "DALDevice.h"
#else /*FEATURE_QDSP6*/   
#include "qurt.h"
#endif /*FEATURE_QDSP6*/
#endif

#ifdef FEATURE_MEMHEAP2_USE_PRNG
#include "PrngML.h"
#endif

#ifdef FEATURE_MEM_DEBUG
#include "rcinit.h"
#endif


/*============================================================================
                             CONSTANT DEFINITIONS
============================================================================*/

/* Heap allocation failure constants */
#ifndef AMSSHEAP_FAIL_ARRAY_SIZE
#define AMSSHEAP_FAIL_ARRAY_SIZE  2
#endif

#ifndef MEMHEAP_CRIT_SEC_SIZE 
#define MEMHEAP_CRIT_SEC_SIZE 64
#endif

#ifdef FEATURE_MEM_DEBUG
#ifndef AMSS_HEAP_CALLER_LEVEL
#define AMSS_HEAP_CALLER_LEVEL     (0)
#endif

#if defined(FEATURE_QDSP6)
 #define  MEM_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(AMSS_HEAP_CALLER_LEVEL));
#else
 #define MEM_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif
#endif

/*============================================================================
                             DATA DECLARATIONS
=============================================================================*/
#ifdef FEATURE_ENABLE_HEAP_PROFILING
int start_profiling = 0;
boolean debug_timer_init = FALSE;
#define PROFILE_START_POINT 100
memheap_profile_table_type memheap_profile_table[50];
uint32 memheap_profile_table_index = 0;

static uint32 malloc_in_out_biggest_diff = 0;
static uint32 malloc_in_out_smallest_diff = 100;
static DalDeviceHandle *hTimerHandle = NULL;
#endif /*FEATURE_ENABLE_HEAP_PROFILING */

extern uint32 memheap_global_error;


#define MALLOC_NOT_READY 0
#define MALLOC_INITIALIZING 1
#define MALLOC_READY 2
static uint32 mem_require_for_qmutex = 0;
static uint32 amssheap_init_flag = MALLOC_NOT_READY;


extern dword magic_num[];
extern uint16 magic_num_index;
extern uint16 magic_num_index_array[];
extern uint16 magic_num_index;

#ifdef FEATURE_MEM_DEBUG
memheap_task_stats_type amss_heap_task_stats[MEMHEAP_MAX_THREADS];
extern int memheap_tls_key;
#endif


/*============================================================================
                             GLOBAL VARIABLES
============================================================================*/

#define ADDR_MASK_256K  0x3FFFF
/*============================================================================
                             LOCAL VARIABLES
============================================================================*/

#if !defined(FEATURE_BLAST) && !defined(FEATURE_QURT) && !defined(FEATURE_L4) && !defined(FEATURE_UTILS_MEMHEAP_NOSTATIC_INIT)

/* really should define in SConscript so it can be updated in configuration */
#define MEMHEAP_HEAP_SIZE 0x300000

static uint8 amss_malloc_buffer[MEMHEAP_HEAP_SIZE];

#endif

static uint16 malloc_count = 0; /*stores the mallocs done till qmutex was read*/

mem_heap_type amss_mem_heap;

#if defined(FEATURE_QURT)
extern void *heap_region_start;
extern unsigned long heap_region_used;
extern unsigned long heap_region_size;
extern qurt_mem_pool_t qurt_mem_default_pool;
#endif

/* Current index into the array used to log heap allocation failures.
 * This index will continue to be incremented even when the array is full,
 * to serve as a total failure counter.
 */
int amssheap_fail_index = 0;

/* Array used to log heap allocation failures */
memheap_v_info_s_type amssheap_fail_info[AMSSHEAP_FAIL_ARRAY_SIZE];


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

#ifdef FEATURE_MEM_DEBUG
/* AMSS memory debug functions */
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);
static uint32 amssheap_get_task_quota(void);
#endif  /* FEATURE_MEM_DEBUG */

extern void mem_heap_init_lock_free_fns(mem_heap_type *heap_ptr);


extern void memheap_addNodeToBin(mem_heap_type *heap_ptr, void *freeBlock);

extern void mem_heap_add_section(
     mem_heap_type *heap_ptr, 
     void *section_start, 
     unsigned long section_size);

extern void init_binToUSeForMalloc(
  mem_heap_type             *heap_ptr, 
  mem_block_header_type     *mem_block
);

#ifdef FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT
void amss_malloc_init
#else
static void amss_malloc_init
#endif
(
  void *start_heap,
  void *end_heap
);
#ifdef FEATURE_QURT
extern void * sys_sbrk(int more);
typedef long long Align;
union header {                  /* block header */
    struct {
        unsigned size;          /* size of this block */
        union header *ptr;      /* next block if on free list */
    } s;
    Align x;                    /* force alignment of blocks */
};
typedef union header Header;
#endif
static void amss_heap_initialized(void);
#ifdef FEATURE_ENABLE_HEAP_PROFILING
/*===========================================================================
FUNCTION print_memheap_profile_table

DESCRIPTION
   This function will print the profiling numbers for all of the memheap users
   
   QXDM CMD : send_data 0x4B 0x49 0x00 0x00 0x15 0x00

===========================================================================*/
void print_memheap_profile_table(void){

   MSG_HIGH("memheap2-Profiling numbers  malloc_in_out_smallest_diff=%d malloc_in_out_biggest_diff:=%d",
               malloc_in_out_smallest_diff, malloc_in_out_biggest_diff, 0);

  {
   int i;
   for(i = 0; (i < memheap_profile_table_index) && (memheap_profile_table[i].heap_ptr) ; i++){
    
      mem_heap_type * temp = (mem_heap_type *)(memheap_profile_table[i].heap_ptr);

      MSG_HIGH("----------------------Next Heap-----------------------",0,0,0);
            
      {
         MSG_HIGH("heapNo.=%d -heap_ptr:=0x%x  ..heap_size:=0x%x",
            i, memheap_profile_table[i].heap_ptr, 
            ((mem_heap_type *)(memheap_profile_table[i].heap_ptr))->total_bytes);

         MSG_HIGH("malloc_avg = %d     malloc_cnt = %d ",
            memheap_profile_table[i].malloc_avg, memheap_profile_table[i].malloc_cnt,0);

         MSG_HIGH("free_avg: = %d   free_cnt = %d   realloc_avg = %d",
            memheap_profile_table[i].free_avg, memheap_profile_table[i].free_cnt, 
            memheap_profile_table[i].realloc_avg);

         MSG_HIGH("used_bytes = 0x%x  total_blocks = %d",temp->used_bytes,
              temp->total_blocks, 0 );

         if(temp->legacy_check & kUseBINsImpl)
         {
           unsigned long binNo = MEMHEAP2_BIN_COUNT - 1;

           while(!(temp->BIN[binNo]).lastNode && binNo >= 0)
              binNo--;

           MSG_HIGH("Biggest Availabe FreeBlock size = 0x%x ",
              (((temp->BIN[binNo]).lastNode)->freeBlock).forw_offset,0,0);

         }
      }
   }
  }

}


void init_heap_debug(mem_heap_type *heap_ptr){
  heap_ptr->debug_heap_index = memheap_profile_table_index;
  memheap_profile_table[heap_ptr->debug_heap_index].heap_ptr = (void*)heap_ptr;
  memheap_profile_table_index++;
}

void init_heap_debug_timers(void)
{
  DALSYS_InitMod(NULL);
  DalTimetick_Attach("SystemTimer", &hTimerHandle);
  ASSERT(hTimerHandle != NULL);
  /* Enable system timer */
  DalTimetick_Enable(hTimerHandle,1);
  DalTimetick_InitTimetick64(hTimerHandle);
  debug_timer_init = TRUE;
}
#endif /*FEATURE_ENABLE_HEAP_PROFILING*/

void amss_heap_init(void *start_heap, uint32 size)
{
  uint16 * pblk = NULL;
  char *memory_end_ptr;
  unsigned long chunks;
  mem_block_header_type *blk_ptr;
  char *memory_start_ptr = (char *)start_heap;

  memory_end_ptr = memory_start_ptr + size;
  while( (((unsigned long)memory_start_ptr) & 0x000FUL) )
  { 
     ++memory_start_ptr;
  }

  chunks = (unsigned long) ((memory_end_ptr - memory_start_ptr) / kMinChunkSize);

  amss_mem_heap.first_block           = (mem_block_header_type *) memory_start_ptr;
  amss_mem_heap.next_block            = amss_mem_heap.first_block;

  blk_ptr = (mem_block_header_type *)memory_start_ptr;  
  amss_mem_heap.total_blocks      = 1;
  amss_mem_heap.max_used          = 0;
  amss_mem_heap.max_request       = 0;
  amss_mem_heap.used_bytes        = 0;
  amss_mem_heap.total_bytes       = chunks * kMinChunkSize;
  amss_mem_heap.fail_fnc_ptr      = amssheap_malloc_failed;
  amss_mem_heap.legacy_check = kUseBINsImpl;
  amss_mem_heap.sections[amss_mem_heap.number_of_sections].size= chunks * kMinChunkSize;
  amss_mem_heap.sections[amss_mem_heap.number_of_sections].start_addr=(void *)memory_start_ptr;

  blk_ptr->free_flag = (char)0x1;
  blk_ptr->last_flag = (char)0x1;
  blk_ptr->start_flag = (char)0x1;
  blk_ptr->forw_offset = chunks * kMinChunkSize;
  blk_ptr->section_index = amss_mem_heap.number_of_sections;
  amss_mem_heap.number_of_sections++;

  #ifdef FEATURE_MEMHEAP2_USE_PRNG
  if(PRNGML_ERROR_NONE != PrngML_getdata_lite(((uint8*)(&(magic_num[magic_num_index_array[magic_num_index]]))), 4))
  {
	 ASSERT(0);
  }
  if(PRNGML_ERROR_NONE != PrngML_getdata_lite((uint8*)(&(amss_mem_heap.magic_num_free)), 2))
  {
	 ASSERT(0);
  }
  if(PRNGML_ERROR_NONE != PrngML_getdata_lite((uint8*)(&(amss_mem_heap.magic_num_used)), 2))
  {
	 ASSERT(0);
  }
  #else
  
  magic_num[magic_num_index_array[magic_num_index]] = 0xabcddcba;
  amss_mem_heap.magic_num_used = 0xaaaa;
  amss_mem_heap.magic_num_free = 0xcccc;
  #endif  

  amss_mem_heap.magic_num = magic_num[magic_num_index_array[magic_num_index]];
  amss_mem_heap.magic_num_index = magic_num_index_array[magic_num_index];
  magic_num_index++;

  pblk = (uint16*)blk_ptr;
  ADD_GUARD_BYTES_TO_FREE_HEADER(amss_mem_heap.magic_num_free, pblk);

  init_binToUSeForMalloc(&amss_mem_heap, amss_mem_heap.first_block);
  memheap_addNodeToBin(&amss_mem_heap, amss_mem_heap.first_block);

#ifdef FEATURE_MEM_DEBUG
   if(QURT_EOK != qurt_tls_create_key(&memheap_tls_key, NULL))
   {
      memheap_global_error = memheap_global_error | MEM_TLS_CREATE_ERR ;
   }
   else
   {
      amss_mem_heap.heapStatistics = &amss_heap_task_stats[0];
   }
   amss_mem_heap.mem_get_quota_fnc_ptr = amssheap_get_task_quota;

#endif

#ifdef FEATURE_ENABLE_HEAP_PROFILING
  init_heap_debug(&amss_mem_heap);
#endif
}

static void amss_heap_initialized(void)
{

#if defined(FEATURE_BLAST) || defined(FEATURE_QURT) || defined(FEATURE_L4)
    void *heap_start;
    void *heap_end=0;
#ifdef FEATURE_QDSP6
#ifdef FEATURE_QURT

#ifdef FEATURE_MBA_MEMHEAP_IMAGE

   Header *heap_header_ptr=(Header *)sys_sbrk(1);
   ASSERT(heap_header_ptr != (void *)-1);
   heap_region_start = (void *)heap_header_ptr;
   heap_region_size = heap_header_ptr->s.size;
   heap_start = heap_region_start;
   heap_end = (void *)((char *)heap_region_start+heap_region_size);

#else
   /* Create a region to allocate space for amss and modem heaps */
   int ret;
   unsigned int amssheapsize;
   unsigned int heap_base_vaddr;
   /* Query the AMSS heap size */
   qurt_sysenv_app_heap_t heap_obj;
   ret = qurt_sysenv_get_app_heap(&heap_obj);

   ASSERT(ret == QURT_EOK);
   amssheapsize = heap_obj.heap_limit - heap_obj.heap_base;

   qurt_mem_region_attr_t heap_region_attr;
   qurt_mem_region_t heap_region;
   unsigned int    heap_region_pa;
   
   // Get a big size add 3 more MB into it.
   heap_region_size = amssheapsize + MODEM_MEM_HEAP_SIZE + QZIP_MEMHEAP_SIZE;

   while(1) 
   {
       qurt_mem_region_attr_init( &heap_region_attr );
       

       qurt_mem_region_attr_set_mapping( &heap_region_attr, QURT_MEM_MAPPING_VIRTUAL);
       ret = qurt_mem_region_create(&heap_region, heap_region_size, qurt_mem_default_pool, &heap_region_attr);

       ASSERT(ret == QURT_EOK);
       qurt_mem_region_attr_get(heap_region, &heap_region_attr);
       qurt_mem_region_attr_get_physaddr(&heap_region_attr,&heap_region_pa);

       //check if physical address is 256k aligned or not, if not align into next 256k and use that
       // for physical address.

       if((heap_region_pa & ADDR_MASK_256K) != 0 ) {
           heap_region_pa=((heap_region_pa + ADDR_MASK_256K)& ~ADDR_MASK_256K);
       }
     
       //delete the region
       ret= qurt_mem_region_delete(heap_region);
       ASSERT(ret == QURT_EOK);

       heap_region_size=heap_region_size-QZIP_MEMHEAP_SIZE;

       // qurt_mem_region_attr_init (&heap_region_attr);
       // qurt_mem_region_attr_set_mapping (&heap_region_attr, QURT_MEM_MAPPING_VIRTUAL_RANDOM);
       // qurt_mem_region_attr_set_physaddr(&heap_region_attr,(qurt_addr_t)heap_region_pa);
      
       // ret = qurt_mem_region_create(&heap_region, heap_region_size, qurt_mem_default_pool, &heap_region_attr);
       // if (ret != QURT_EOK)
       // {
           qurt_mem_region_attr_init (&heap_region_attr);
           qurt_mem_region_attr_set_mapping (&heap_region_attr, QURT_MEM_MAPPING_PHYS_CONTIGUOUS);
           qurt_mem_region_attr_set_physaddr(&heap_region_attr,(qurt_addr_t)heap_region_pa);
           ret = qurt_mem_region_create(&heap_region, heap_region_size, qurt_mem_default_pool, &heap_region_attr);
           if(ret != QURT_EOK) 
           {
               continue;
           }
           break;
       // }

   }
   qurt_mem_region_attr_get(heap_region, &heap_region_attr);
   qurt_mem_region_attr_get_virtaddr(&heap_region_attr, &heap_base_vaddr);

   heap_region_start = (void *)heap_base_vaddr;
   heap_start = heap_region_start;

   /* Use only the size for the size amss heap 
     remaining memory will be used for other heaps   */

   heap_end = (void *)((char *)heap_region_start+amssheapsize);
   heap_region_used = amssheapsize;

#endif
	memset(heap_start, 0, (uint32)heap_end-(uint32)heap_start);

#else
#ifdef FEATURE_BLAST
    int ret;
    blast_app_heap_obj_t heap_obj;
    ret = blast_get_env_object(BLAST_ENV_OBJ_APP_HEAP, (unsigned int *)&heap_obj);
    ASSERT(ret == BLAST_EOK);

    heap_start = (void *)heap_obj.heap_base;
    heap_end = (void *)heap_obj.heap_limit;
#endif
#endif
#endif

#ifdef FEATURE_L4
    L4_Word_t heap_base;
    L4_Word_t heap_size;

    heap_base = okl4_virtmem_item_getbase(myheap);
    heap_size = okl4_virtmem_item_getsize(myheap);
    heap_start = (void*)heap_base;
    heap_end = (void*)(heap_base + heap_size - 1);
#endif
    amss_malloc_init(heap_start, heap_end);

#else 
  // lets make this static implementation optional as it is not needed for RIVA, riva directly calls amss_malloc_init
#ifndef FEATURE_UTILS_MEMHEAP_NOSTATIC_INIT  
  amss_malloc_init((&amss_malloc_buffer[0]), (void *)(&amss_malloc_buffer[0] +sizeof(amss_malloc_buffer)-1));
#endif
#endif /* defined(FEATURE_BLAST) || defined(FEATURE_QURT) || defined(FEATURE_L4) */

	if((amssheap_init_flag != MALLOC_READY) && (!mem_require_for_qmutex))
	{
		int ret_value;
		mem_require_for_qmutex = TRUE;
		magic_num[magic_num_index] = 0; /*need to set it to zero otherwise it will assert in the first malloc*/
		ret_value = osal_init_crit_sect((osal_crit_sect_t*)&amss_mem_heap.memheap_crit_sect);

		if(ret_value == OSAL_SUCCESS)
		{
			mem_heap_init_lock_free_fns(&amss_mem_heap);
			amssheap_init_flag = MALLOC_READY;
		}
		else{
			mem_require_for_qmutex = FALSE;
			malloc_count=malloc_count+1;
		}
	 
	}
}

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

#ifdef FEATURE_L4
OKL4_ENV_GET_STATIC(okl4_virtmem_item_t, myheap, "MAIN_HEAP");
#endif

#ifdef AMSSHEAP_UNIT_TEST
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
  void *ptr = NULL;

  if(amssheap_init_flag == MALLOC_NOT_READY) 
  {    
	amss_heap_initialized();    
  }

  if((amssheap_init_flag == MALLOC_READY) || !ptr){
    #ifdef FEATURE_ENABLE_HEAP_PROFILING
    time_timetick_type malloc_inTime=0, malloc_outTime=0, diff =0;
    start_profiling++;
    if(start_profiling > PROFILE_START_POINT){
       if (!debug_timer_init) {
          init_heap_debug_timers();
       }
       DalTimetick_GetTimetick64(hTimerHandle, &malloc_inTime);
    }
    #endif

    ptr = mem_malloc( &amss_mem_heap, size );

	#ifdef FEATURE_ENABLE_HEAP_PROFILING
  if(start_profiling > PROFILE_START_POINT){
     if (!debug_timer_init) {
        init_heap_debug_timers();
     }
    DalTimetick_GetTimetick64(hTimerHandle, &malloc_outTime);
    diff = malloc_outTime - malloc_inTime;
      memheap_profile_table[amss_mem_heap.debug_heap_index].malloc_avg+= diff;
      memheap_profile_table[amss_mem_heap.debug_heap_index].malloc_cnt++;
  
      if (diff > malloc_in_out_biggest_diff)
        malloc_in_out_biggest_diff = diff;
  
      if (diff < malloc_in_out_smallest_diff)
        malloc_in_out_smallest_diff = diff;
  }
	#endif
  }
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&amss_mem_heap,ptr, caller_ptr,size);
  }
#endif
  return ptr;
}

void *amssheap_malloc
(
  size_t size,
  void *caller_ptr
)
{
   void *ptr=malloc(size);
#ifdef FEATURE_MEM_DEBUG
   memdebug_updatecallerptr(&amss_mem_heap,ptr,caller_ptr,size);
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
#ifdef AMSSHEAP_UNIT_TEST
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

  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  time_timetick_type realloc_inTime=0 , realloc_outTime=0;
  if(start_profiling > PROFILE_START_POINT){
     if (!debug_timer_init) {
        init_heap_debug_timers();
     }
    DalTimetick_GetTimetick64(hTimerHandle, &realloc_inTime);
  }
  #endif
  
  ptr = mem_realloc( &amss_mem_heap, ptr, size );
  
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
   if(start_profiling > PROFILE_START_POINT){
      if (!debug_timer_init) {
         init_heap_debug_timers();
      }
    DalTimetick_GetTimetick64(hTimerHandle, &realloc_outTime);
    memheap_profile_table[amss_mem_heap.debug_heap_index].realloc_avg = realloc_outTime - realloc_inTime;
    memheap_profile_table[amss_mem_heap.debug_heap_index].realloc_cnt++;
	}
  #endif

#ifdef FEATURE_MEM_DEBUG
    {
      void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&amss_mem_heap,ptr, caller_ptr,size);
    }
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
#ifdef AMSSHEAP_UNIT_TEST
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
  if(amssheap_init_flag == MALLOC_NOT_READY) 
  {    
	amss_heap_initialized();    
  }
  ptr = mem_calloc( &amss_mem_heap, count, size );
#ifdef FEATURE_MEM_DEBUG
  {
    void *caller_ptr=MEM_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&amss_mem_heap,ptr, caller_ptr,count * size);
  }
#endif
  return ptr;
}

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
#ifdef AMSSHEAP_UNIT_TEST
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
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  time_timetick_type free_inTime=0, free_outTime=0;
  if(start_profiling > PROFILE_START_POINT){
     if (!debug_timer_init) {
        init_heap_debug_timers();
     }
    DalTimetick_GetTimetick64(hTimerHandle, &free_inTime);
  }
  #endif
  
  mem_free( &amss_mem_heap, ptr );
  
  #ifdef FEATURE_ENABLE_HEAP_PROFILING
  if(start_profiling > PROFILE_START_POINT){
     if (!debug_timer_init) {
        init_heap_debug_timers();
     }
      DalTimetick_GetTimetick64(hTimerHandle, &free_outTime);
      memheap_profile_table[amss_mem_heap.debug_heap_index].free_avg += (free_outTime - free_inTime);
      memheap_profile_table[amss_mem_heap.debug_heap_index].free_cnt++;
    }
  #endif /*FEATURE_ENABLE_HEAP_PROFILING*/
}

/*===========================================================================

FUNCTION memalign

DESCRIPTION
  This function allocate a memory block from amss_mem_heap of size(size)
  having the alocated memory block address aligned with block_size.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

ARGUMENTS
  block_size - need to aligned with
  size       - size requested memory size

===========================================================================*/
void* memalign(
  size_t           block_size, /*need to aligned with*/
  size_t           size /*requested memory size*/
)
{
  return mem_memalign(&amss_mem_heap, block_size, size);
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
  memheap_v_info_s_type *info;

  /* Record the current heap state */
  if (amssheap_fail_index < ARR_SIZE(amssheap_fail_info))
  {
    int i, freeBlk_list_idx = 0;
    unsigned long  totalFreeBlocks = 0;
    unsigned long  totalSizeOfFreeBlocks=0;
    freeBlockList *temp;
    uint16* pblk;
    info = &amssheap_fail_info[amssheap_fail_index];
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
  amssheap_fail_index++;

  MSG_ERROR("AMSS heap failed to allocate %d bytes (failure %d)",
    size, amssheap_fail_index, 0);
}



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
#ifdef FEATURE_UTILS_ENABLE_AMSS_MALLOC_INIT
void amss_malloc_init
#else
static void amss_malloc_init
#endif
(
  void *start_heap,
  void *end_heap
)
{
  uint32 size;
  int ret_value;
  /* Clear the heap control struct in case of failure */
  memset(&amss_mem_heap, 0, sizeof(amss_mem_heap));

  size = (uint32)end_heap - (uint32)start_heap;

  if (start_heap == NULL || size == 0)
  {
    for ( ; ; )   /* Cannot fatal at this point in boot */
    {
    }
  } 
  amssheap_init_flag = MALLOC_INITIALIZING;
  amss_heap_init(start_heap, size);
  ret_value = osal_init_crit_sect((osal_crit_sect_t*)&amss_mem_heap.memheap_crit_sect);
  if(ret_value != OSAL_SUCCESS)
  {
    MSG_HIGH(" Failed:osal_init_crit_sect ret_value = %d ",ret_value,0,0);
  }else
  {
	amssheap_init_flag = MALLOC_READY;
	mem_require_for_qmutex =1;
  }

  mem_heap_init_lock_free_fns(&amss_mem_heap);

}

/*===========================================================================

FUNCTION amss_heap_add_region

DESCRIPTION
  Add a memory region to the amss heap

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

ARGUMENTS
  section_start - Pointer to the beginning of the memory region.
  size   - size of the memory region to be added.

===========================================================================*/

void amss_heap_add_region(void *section_start, unsigned long section_size)
{
   mem_heap_add_section(&amss_mem_heap,section_start,section_size);
}

/*===========================================================================

FUNCTION amssheap_get_current_heap_usages

DESCRIPTION
  returns the current amss_mem_heap usage(i.e. in use memory)

DEPENDENCIES
  None

SIDE EFFECTS
  None
ARGUMENTS
  None

===========================================================================*/
unsigned long amssheap_get_current_heap_usages(void)
{
  return amss_mem_heap.used_bytes;
}

/*===========================================================================

FUNCTION amssheap_get_current_in_use_blocks_count

DESCRIPTION
  returns the total current in use blocks in amss_mem_heap

DEPENDENCIES
  None

SIDE EFFECTS
  None
ARGUMENTS
  None
===========================================================================*/
unsigned long amssheap_get_current_in_use_blocks_count(void)
{
  int i;
  unsigned long  totalFreeBlocks = 0;
  for (i= 0; i< MEMHEAP2_BIN_COUNT; i++)
  {
    totalFreeBlocks += amss_mem_heap.BIN[i].totalNodes;
  }
  return (amss_mem_heap.total_blocks - totalFreeBlocks);
}

/*===========================================================================

FUNCTION amssheap_mem_get_block_size

DESCRIPTION
  Returns the block size from a assigned ptr in the amss_mem_heap.

  @param[in] void Pointer to the memory block to be queried.

  @return
  Returns the size (in bytes) of the block, excluding the block header 
  overhead and the unused bytes at the end of the block.

===========================================================================*/
unsigned long amssheap_mem_get_block_size (void *ptr)
{
  return mem_get_block_size(&amss_mem_heap, ptr);
}

/*===========================================================================

FUNCTION amssheap_add_section

DESCRIPTION
  Add a memory section to amss_mem_heap.

===========================================================================*/
void amssheap_add_section(
   /* Pointer to contiguous block of memory used for this section  */
   void                          *section_start,
   /* Size of the section  */
   unsigned long                  section_size
)
{
  mem_heap_add_section( &amss_mem_heap, section_start, section_size);
}

void amss_mem_get_section_info(void **section_info)
{
	*section_info=(void *)&amss_mem_heap.sections[0];
}

#ifdef FEATURE_MEM_DEBUG
static uint32 amssheap_get_task_quota(void)
{
   RCINIT_INFO rc_handle;
   RCINIT_HEAPQSZ rc_heap_quota=0;

   rc_handle = rcinit_lookup_self();
   if(NULL != rc_handle)
   {
       rc_heap_quota = rcinit_lookup_heapqsz_info(rc_handle,&amss_mem_heap);
   }

   return rc_heap_quota;
}
#endif