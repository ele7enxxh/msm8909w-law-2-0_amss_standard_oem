/*!
  @file
  dynamic_heap.c

  @brief
  Implementation of a heap wrapper for the dynamic memory heap.

*/

/*===========================================================================

  Copyright (c) 2009-2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/utils/src/dynamic_heap.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/29/14   abh     Added checking for initialization completion.
11/30/12   ps      Initial Version

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include "core_variation.h"
#include "memheap.h"
#include <stdio.h>
#include <string.h>
#include "err.h"
#include <stringl/stringl.h>
#include "msg.h"
#include "osal.h"
#include "dynamic_heap.h"
#include "mem_share_client.h"
#include <stdlib.h>
#include <stdint.h>
#include "qurt.h"
#include "msgcfg.h"
 #include "mba_ac.h"


 /*===========================================================================
 
                    INTERNAL DEFINITIONS AND TYPES
 
 ===========================================================================*/

#ifndef MEM_SHARE_REQUEST_SIZE 
#define MEM_SHARE_REQUEST_SIZE 0x200000
#endif

#ifndef MEM_SHARE_REQUEST_ALIGN 
#define MEM_SHARE_REQUEST_ALIGN 0x8
#endif

#define MSA_BIT_1 1

#define DYNAMIC_MEM_PHYS_POOL "DYN_MEM_PHYS_POOL"


/* Number of outstanding allocations from HLOS, depends of number of XPU locks*/
#define MAX_HLOS_ALLOCATIONS 2 

#define DYN_HEAP_NO_XPU_UNLOCK 0
#define DYN_HEAP_XPU_UNLOCK 1

#ifdef FEATURE_MEM_DEBUG_COMMON
extern void memdebug_updatecallerptr(mem_heap_type *heap_ptr,void *ptr,void *caller_ptr, size_t size);

#ifndef DYNAMIC_HEAP_CALLER_LEVEL
#define DYNAMIC_HEAP_CALLER_LEVEL     (0)
#endif

#if defined(FEATURE_QDSP6)
 #define  DYNAMIC_HEAP_CALLER_ADDRESS() ((void *)__builtin_return_address(DYNAMIC_HEAP_CALLER_LEVEL));
#else
 #define DYNAMIC_HEAP_CALLER_ADDRESS() ((void *) __return_address())
#endif
#endif

typedef struct _dynamic_heap_region_struct {
  qurt_mem_pool_t              mem_pool; 
  qurt_mem_region_attr_t       mem_attribs;
  qurt_mem_region_t            mem_region;
  uint32_t                     paddr;  /*<! physical address */
  uint32                       vaddr;  /* Virtual Address */
  uint32                       num_bytes;
  resource_id_type             xpu_id;
  boolean                      xpu_in_use;
}heap_mem_region_t;


mem_heap_type dynamic_mem_heap;
mem_share_client_info_type mem_share_client_info[DYNAMIC_MEM_CLIENT_ID_MAX];



static uint32 dyn_heap_critical_section_init=0;
static uint32 signalReleaseHeap = 0;
static osal_crit_sect_t dyn_heap_critical_section;
static int num_hlos_allocations = 0;

static int get_heap_memory(dynamic_mem_client_id client_id, dynamic_mem_attribute attr,uint32 numBytes, uint32 alignment,void **ptrMemBlock);
static int release_heap_memory(dynamic_mem_client_id client_id,boolean doXpuUnlock);
static int create_mem_region(dynamic_mem_client_id client_id, uint32 physAddr, uint32 numBytes, void **ptrMemBlock);


heap_mem_region_t memheap_region[DYNAMIC_MEM_CLIENT_ID_MAX];


extern mem_share_status_t mem_share_client_init(void);


unsigned int dynamic_heap_g_connect(void)
{
   if (dyn_heap_critical_section_init == 0)
   {
      dyn_heap_critical_section_init=1;
      if(OSAL_SUCCESS != osal_init_crit_sect(&dyn_heap_critical_section))
      {
         MSG_ERROR("dynamic_heap_init: osal_init_crit_sect returned failure\n",0,0,0);
         return DYNAMIC_MEM_FAIL;
      }
      //Also initialize qmi client here
      if(MEM_SHARE_SUCCESS != mem_share_client_init())
      {
         MSG_ERROR("dynamic_heap_init: mem_share_client_init returned failure\n",0,0,0);
         return DYNAMIC_MEM_FAIL;
      }
   }

   return DYNAMIC_MEM_SUCCESS;
}
unsigned int dynamic_heap_init(void)
{
	return  dynamic_heap_g_init(DYNAMIC_MEM_CLIENT_ID_GPS,DYNAMIC_MEM_ATTRIBUTE_CMA,MEM_SHARE_REQUEST_SIZE,&dynamic_mem_heap,NULL);
}
/*===========================================================================

  FUNCTION:  dynamic_heap_g_init

===========================================================================*/
/*!
    @brief
    Initializes the dynamic heap memory 

    @detail
    Initializes a heap by allocating memory on HLOS and mapping the memory in modem memory space..

    @return
    None

*/
/*=========================================================================*/

unsigned int dynamic_heap_g_init(dynamic_mem_client_id client_id, dynamic_mem_attribute attr, size_t size, mem_heap_type *heap_ptr,void **alloc_ptr)
{
   void *ptr_mem = NULL;
   if(size == 0)
   {
      return DYNAMIC_MEM_FAIL_INVALID_SIZE;
   }
   	// Do some validation about client id and attr
   if((client_id <= DYNAMIC_MEM_CLIENT_ID_MIN) && (client_id >= DYNAMIC_MEM_CLIENT_ID_MAX))
   {
      return DYNAMIC_MEM_FAIL_INVALID_CLIENT_ID;
   }
   if((attr <= DYNAMIC_MEM_ATTRIBUTE_MIN) && (attr >= DYNAMIC_MEM_ATTRIBUTE_MAX))
   {
		return DYNAMIC_MEM_FAIL_INVALID_ATTRIBUTE;
   }

   if(dynamic_heap_g_connect() != DYNAMIC_MEM_SUCCESS)
   {
      return DYNAMIC_MEM_FAIL;
   }
      
   osal_enter_crit_sect(&dyn_heap_critical_section);
   
   
   if(mem_share_client_info[client_id].is_init == 1)
   {
      osal_exit_crit_sect(&dyn_heap_critical_section);
      return DYNAMIC_MEM_INIT_ALREADY;
   }
   mem_share_client_info[client_id].sequence_id=0;
  
   if (0 != get_heap_memory(client_id, attr,size,MEM_SHARE_REQUEST_ALIGN, &ptr_mem))
   {
     osal_exit_crit_sect(&dyn_heap_critical_section);
     return DYNAMIC_MEM_FAIL;
   }
   if(heap_ptr != NULL)
   {
      mem_init_heap(heap_ptr,ptr_mem, size, NULL);
   }

   if(alloc_ptr != NULL)
   {
      *alloc_ptr=ptr_mem;
   }
   mem_share_client_info[client_id].is_init=1;
   
   osal_exit_crit_sect(&dyn_heap_critical_section);

   return DYNAMIC_MEM_SUCCESS;
} /* dynamic_heap_init() */

void* dynamic_alloc(size_t size)
{
   void *ptr = NULL;
   ptr =  dynamic_g_alloc(&dynamic_mem_heap,size);
   #ifdef FEATURE_MEM_DEBUG_COMMON
   {
      void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&dynamic_mem_heap,ptr, caller_ptr,size);
   }
   #endif
   return ptr;
}
/*===========================================================================

  FUNCTION:  dynamic_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or 
    NULL if the block could not be allocated.
*/
/*=========================================================================*/
void* dynamic_g_alloc(mem_heap_type *heap_ptr,size_t size)
{
   void *ptr = NULL;
   ptr=mem_malloc(heap_ptr,size);
   #ifdef FEATURE_MEM_DEBUG_COMMON
   {
      void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(heap_ptr,ptr, caller_ptr,size);
   }
   #endif
  return ptr;
} /* dynamic_alloc() */

void* dynamic_calloc(size_t elt_count, size_t elt_size)
{
   void *ptr = NULL;
   ptr =  dynamic_g_calloc(&dynamic_mem_heap,elt_count, elt_size);
   #ifdef FEATURE_MEM_DEBUG_COMMON
   {
      void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
      memdebug_updatecallerptr(&dynamic_mem_heap,ptr, caller_ptr,elt_count*elt_size);
   }
   #endif
   return ptr;
}


/*===========================================================================

  FUNCTION:  dynamic_g_calloc

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

void* dynamic_g_calloc(mem_heap_type *heap_ptr,size_t elt_count, size_t elt_size)
{
  void *ptr=NULL;
  ptr=mem_calloc(heap_ptr,elt_count, elt_size);
  
  #ifdef FEATURE_MEM_DEBUG_COMMON
  {
    void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(heap_ptr,ptr, caller_ptr,elt_count*elt_size);
  }
  #endif

  return ptr;
} /* dynamic_calloc() */

void *dynamic_realloc(void *ptr, size_t size)
{
  ptr=dynamic_g_realloc(&dynamic_mem_heap, ptr,size);
  
  #ifdef FEATURE_MEM_DEBUG_COMMON
  {
    void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(&dynamic_mem_heap,ptr, caller_ptr,size);
  }
  #endif

  return ptr;
}
/*===========================================================================

  FUNCTION:  dynamic_g_realloc

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
    If ptr is NULL, the function behaves exactly like memheap_extra_alloc 
    (see above). If ptr is not NULL and size is 0, the function behaves
    exactly like memheap_extra_free (see below).  
    If the block cannot be resized, ptr is not NULL and size is not 0, then 
    NULL is returned and the original block is left untouched.  If the ptr 
    block IS successfully resized and the returned value is different from 
    the ptr value passed in, the old block passed in must be considered 
    deallocated and no longer useable (i.e. do NOT try to mem_free it!) 
    This function will call memheap_extra_alloc if it can't grow the block in 
    place.

    @return
    Returns a pointer to the beginning of the resized block of memory (which
    may be different than ptr) or NULL if the block cannot be resized.
*/
/*=========================================================================*/

void *dynamic_g_realloc(mem_heap_type *heap_ptr,void *ptr, size_t size)
{
  void *ptr_ret=NULL;
  
  ptr_ret=mem_realloc(heap_ptr, ptr,size);
  
  #ifdef FEATURE_MEM_DEBUG_COMMON
  {
    void *caller_ptr=DYNAMIC_HEAP_CALLER_ADDRESS();
    memdebug_updatecallerptr(heap_ptr,ptr_ret, caller_ptr,size);
  }
  #endif

  return ptr_ret;
  
  
} /* dynamic_realloc() */


/*===========================================================================

  FUNCTION:  dynamic_g_free

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
void dynamic_g_free(mem_heap_type *heap_ptr,void *ptr)
{
  mem_free(heap_ptr,ptr);
} /* dynamic_free() */

void dynamic_free(void *ptr)
{
  dynamic_g_free(&dynamic_mem_heap,ptr);
  if((1 == signalReleaseHeap)  && (0 == dynamic_mem_heap.used_bytes))
  {
     dynamic_heap_deinit();
     signalReleaseHeap = 0;
  }

} /* dynamic_free() */

void dynamic_heap_deinit(void)
{
	int ret;
	ret=dynamic_heap_g_deinit(DYNAMIC_MEM_CLIENT_ID_GPS,&dynamic_mem_heap);
	if(ret == DYNAMIC_MEM_DEINIT_FAIL_HEAP_BLOCK_USED)
	{
		signalReleaseHeap = 1;
	}
		
}
/*===========================================================================
FUNCTION MEM_DEINIT_HEAP

DESCRIPTION
  De-Initializes the heap_ptr object only if the heap is in reset state.
  User is responsible for freeing all the allocated pointers before  calling
  into this function.
  If all the memory is freed, this function will also release all the memory back to the HLOS.
===========================================================================*/
unsigned int dynamic_heap_g_deinit(dynamic_mem_client_id client_id,mem_heap_type *heap_ptr)
{
   if((client_id <= DYNAMIC_MEM_CLIENT_ID_MIN) && (client_id >= DYNAMIC_MEM_CLIENT_ID_MAX))
   {
      return DYNAMIC_MEM_FAIL_INVALID_CLIENT_ID;
   }
  
   if(heap_ptr != NULL)
   {
      if(0 == heap_ptr->used_bytes)
      {
         mem_deinit_heap(heap_ptr);
      }
      else
      {
         return DYNAMIC_MEM_DEINIT_FAIL_HEAP_BLOCK_USED;
      }
   }
   osal_enter_crit_sect(&dyn_heap_critical_section);
   mem_share_client_info[client_id].is_init=0;
   osal_exit_crit_sect(&dyn_heap_critical_section);
   release_heap_memory(client_id,DYN_HEAP_XPU_UNLOCK);
   return DYNAMIC_MEM_SUCCESS;
}

unsigned int dynamic_heap_g_querysize( dynamic_mem_client_id client_id, uint32_t* size)
{
   if((client_id <= DYNAMIC_MEM_CLIENT_ID_MIN) && (client_id >= DYNAMIC_MEM_CLIENT_ID_MAX))
   {
      return DYNAMIC_MEM_FAIL_INVALID_CLIENT_ID;
   }
   if(size == NULL)
   {
      return DYNAMIC_MEM_FAIL;
   }
   if(dynamic_heap_g_connect() != DYNAMIC_MEM_SUCCESS)
   {
      return DYNAMIC_MEM_FAIL;
   }

   return mem_share_query_size(client_id,size);
}

static int dyn_get_xpu_partition(dynamic_mem_client_id client_id, resource_id_type *xpu_partition)
{
   switch(client_id)
   {
      case DYNAMIC_MEM_CLIENT_ID_GPS: 
        *xpu_partition = MBA_XPU_PARTITION_MODEM_UNCACHED_HEAP;
        break;

      case DYNAMIC_MEM_CLIENT_ID_DIAG:
        if ( memheap_region[DYNAMIC_MEM_CLIENT_ID_FTM].xpu_in_use == 0){
           *xpu_partition = MBA_XPU_PARTITION_FTM;
        }else{
        //No XPU lock for DIAG use case.
          MSG_HIGH("Diag memory, no XPU lock",0,0,0);
          return -1;
        }
        break;

      case DYNAMIC_MEM_CLIENT_ID_FTM:
        *xpu_partition = MBA_XPU_PARTITION_FTM;
        break;

      default:
        return -1;
   }
   return 0;
}
/*===========================================================================
FUNCTION GET_HEAP_MEMORY

DESCRIPTION
  Get memory pool for creating a heap. 
  Currently , this will invoke a QMI call to the server on HLOS to allocate
  memory on the HLOS and send the physical address and size information. 
  The physical address will then be mapped in modem memory space. 
  The memory will also be XPU locked to provide exclusive access to modem.

===========================================================================*/
static int get_heap_memory(dynamic_mem_client_id client_id, 
                           dynamic_mem_attribute attr,
                           uint32 numBytes, 
                           uint32 alignment,
                           void **ptrMemBlock)
{
   uint32 num_bytes_ret = 0;
   mem_share_alloc_rsp_type *mem_share_alloc_rsp = NULL;
   uint32_t phys_addr = 0;
   uint32 ret;
   resource_id_type xpu_partition;

   if(NULL == ptrMemBlock)
   {   
      MSG_ERROR("Invalid arguments, ptr_mem_block is NULL \n",0,0,0);
      return -1; 
   }
   if(num_hlos_allocations >= MAX_HLOS_ALLOCATIONS)
   {
      MSG_ERROR("FAILED: Num allocations allowed reached limit\n",0,0,0);
      return -1;
   }
   if(0 != dyn_get_xpu_partition(client_id, &xpu_partition))
   {
      MSG_HIGH("No XPU lock for this client ID: %d",client_id,0,0);
      return -1;
   }
   //Call QMI service to allocate the block from HLOS

   ret = mem_share_request_pool(client_id, attr,numBytes,alignment,&mem_share_alloc_rsp);

   
   if((MEM_SHARE_SUCCESS == ret) && (mem_share_alloc_rsp != NULL))
   {
      //Handle only the first address right now TODO add handling for more addresses
      phys_addr = mem_share_alloc_rsp[0].phy_addr;
      num_bytes_ret=mem_share_alloc_rsp[0].phy_addr_length;
      // Free the rsp
      free((void *)mem_share_alloc_rsp);
      memheap_region[client_id].paddr = phys_addr;
      memheap_region[client_id].num_bytes = num_bytes_ret;

      MSG_HIGH("HLOS returned SUCCESS - Add pool p_addr: %x num_bytes: %x, xpu_partition = %d",phys_addr, num_bytes_ret,xpu_partition);

      //Check if we can get a lock on the region first 

      if(E_SUCCESS != mba_xpu_lock_region(phys_addr, (phys_addr+num_bytes_ret), xpu_partition))
      {
         MSG_ERROR("mba_xpu_lock_region returned failure\n",0,0,0);
         release_heap_memory(client_id,DYN_HEAP_NO_XPU_UNLOCK);
         return -1;
      }
      memheap_region[client_id].xpu_id = xpu_partition;
      memheap_region[client_id].xpu_in_use = TRUE;
      //Map the memory into modem memory space.
      if(0 != create_mem_region(client_id, phys_addr, num_bytes_ret, ptrMemBlock))
      {
         MSG_ERROR("create_mem_region returned failure\n",0,0,0);
         release_heap_memory(client_id,DYN_HEAP_XPU_UNLOCK);
      }

   }
   else
   {
      MSG_ERROR("get_heap_memory: HLOS returned error \n",0,0,0);
      return -1;
   }

   return 0;
}


/*===========================================================================
FUNCTION RELEASE_HEAP_MEMORY

DESCRIPTION
  Release the memory pool allocated earlier 
  This function will remove the memory region from the pool, and will remove the XPU locks 
   on the memory. It will then invoke a QMI call to the server on HLOS to release the allocated memory.

===========================================================================*/
static int release_heap_memory(dynamic_mem_client_id client_id,boolean doXpuUnlock)
{
   int ret;
   resource_id_type xpu_partition;
   MSG_HIGH("releasing memory to HLOS! trying to delete pheap_region_cur %p v_addr %x p_addr %x \n",&memheap_region[client_id],memheap_region[client_id].vaddr,(memheap_region[client_id].mem_attribs.ppn << 12));


   if((memheap_region[client_id].vaddr == 0) || (memheap_region[client_id].num_bytes == 0))
   {
      return DYNAMIC_MEM_DEINIT_FAIL_INVALID_ARG ;
   }
   // assuming we are deleting region by region TODO handle multiple region deletion
   /* invalidate the cache */
   ret = qurt_mem_cache_clean((qurt_addr_t)memheap_region[client_id].vaddr, 
                             (qurt_size_t)memheap_region[client_id].num_bytes,
                             QURT_MEM_CACHE_INVALIDATE, QURT_MEM_DCACHE);
   if(QURT_EOK != ret)
   {
      MSG_ERROR("qurt_mem_cache_clean returned error %d!!!\n",ret,0,0);
      return DYNAMIC_MEM_DEINIT_FAIL_KERNEL_ERROR;
   }

   ret = qurt_mem_region_delete(memheap_region[client_id].mem_region);
   if(QURT_EOK != ret)
   {
      MSG_ERROR("qurt_mem_region_delete returned error %d!!!\n",ret,0,0);
      return DYNAMIC_MEM_DEINIT_FAIL_KERNEL_ERROR;
   }

   if(doXpuUnlock)
   {
      if(0 == dyn_get_xpu_partition(client_id, &xpu_partition))
      {
        if(E_SUCCESS != mba_xpu_scribble_unlock_region(xpu_partition))
        {
           MSG_ERROR("mba_xpu_lock_region returned failure\n",0,0,0);
           return DYNAMIC_MEM_DEINIT_FAIL_KERNEL_ERROR;
        }
         memheap_region[client_id].xpu_id = 0;
         memheap_region[client_id].xpu_in_use = 0;
      }
      else
      {
         MSG_ERROR("No XPU lock for this client ID: %d",client_id,0,0);
      }
   }

   MSG_HIGH("qurt_mem_region_delete completed !!!\n",0,0,0);

   if(MEM_SHARE_SUCCESS == mem_share_release_pool(client_id,memheap_region[client_id].paddr,memheap_region[client_id].num_bytes))
   {
      MSG_ERROR("mem_share_release_pool completed !!!\n",0,0,0);
   }
   else
   {
      MSG_ERROR("mem_share_release_pool returned failure !!!\n",0,0,0);
   }
   num_hlos_allocations--;
   return DYNAMIC_MEM_SUCCESS;
}

/*===========================================================================
FUNCTION CREATE_MEM_REGION

DESCRIPTION
   Using QURT apis, create a memory pool and a region and get a virtual address mapping 
   for the physical contiguous memory allocated by HLOS.

===========================================================================*/
static int create_mem_region(dynamic_mem_client_id client_id, uint32 physAddr, uint32 numBytes, void **ptrMemBlock)
{

   if(QURT_EOK == qurt_mem_pool_create(DYNAMIC_MEM_PHYS_POOL, (physAddr >> 12),(numBytes>>12), &memheap_region[client_id].mem_pool))
   {
      MSG_HIGH("qurt_mem_pool_create success \n",0,0,0);
   }
   else
   {
      MSG_ERROR("qurt_mem_pool_create failed\n",0,0,0);
      return -1;
   }

   qurt_mem_region_attr_init (&memheap_region[client_id].mem_attribs);
   qurt_mem_region_attr_set_cache_mode(&memheap_region[client_id].mem_attribs,QURT_MEM_CACHE_WRITETHROUGH);
   qurt_mem_region_attr_set_mapping (&memheap_region[client_id].mem_attribs, QURT_MEM_MAPPING_PHYS_CONTIGUOUS);
   qurt_mem_region_attr_set_type (&memheap_region[client_id].mem_attribs, QURT_MEM_REGION_SHARED);
   qurt_mem_region_attr_set_bus_attr(&memheap_region[client_id].mem_attribs, MSA_BIT_1);

   memheap_region[client_id].mem_attribs.ppn = (unsigned int) (physAddr >> 12);

   // Create the region for the physical memory
   if(QURT_EOK == qurt_mem_region_create(&memheap_region[client_id].mem_region,
                                numBytes, memheap_region[client_id].mem_pool, 
                                &memheap_region[client_id].mem_attribs))
   {
     qurt_mem_region_attr_get(memheap_region[client_id].mem_region,
                            &memheap_region[client_id].mem_attribs );

     qurt_mem_region_attr_get_virtaddr(&memheap_region[client_id].mem_attribs,
                                     (unsigned int *)&memheap_region[client_id].vaddr);

     if(memheap_region[client_id].vaddr != 0)
     {
        *ptrMemBlock = (void *)memheap_region[client_id].vaddr;

        MSG_HIGH("mem_heap_add_section completed num_hlos_allocations = %d v_addr: 0x%x p_addr: 0x%x\n",
                        num_hlos_allocations, memheap_region[client_id].vaddr, physAddr);
        num_hlos_allocations ++;
     }
     else
     {
        return -1;
     }
   }
   else
   {
      MSG_ERROR("qurt_mem_region_create failed\n",0,0,0);
      return -1;
   }
   return 0;
}

