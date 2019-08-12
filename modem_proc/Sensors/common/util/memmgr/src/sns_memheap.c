/*============================================================================
  @file sns_memheap.c

  @brief
  Implements Sensors heap interfaces using memheap2.

  <br><br>

  DEPENDENCIES:  Uses memheap2 APIs.

Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential
  ============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  when       who    what, where, why 
  ---------- --- -----------------------------------------------------------
  2015-08-13 bd  Decrease default heap size to 1.0MB, and increase sensors heap by 512kB
  2015-07-10 hw  Increase default heap size to 1.5MB
  2015-01-12 ps  Merge updates for Memmgr
  2014-08-21 ps  Moved Sensor heap functions from sns_memmgr.c

  ============================================================================*/

/*=====================================================================
  INCLUDES
=======================================================================*/

#include "sns_common.h"

#include "memheap.h"
#include "sns_memheap.h"

#define SNS_DEFAULT_HEAP_SIZE   ((256+768)*1024)

/*============================================================================
  Global Data Definitions
  ===========================================================================*/

/* Default heap memory pointer */
mem_heap_type sns_def_heap_state;
mem_heap_totals_type sns_def_heap_total;

/* Sensors heap of SNS_DEFAULT_HEAP_SIZE bytes */
static uint8_t sns_def_heap_mem[SNS_DEFAULT_HEAP_SIZE] __attribute__((__section__(".bss.sns_def_heap_mem")));

#define  SNS_OS_IS_HEAP_PTR(ptr)  (((uint8_t *)ptr) > ((uint8_t *)sns_def_heap_mem)  && ((uint8_t *)ptr) < (((uint8_t *)sns_def_heap_mem)  + sizeof(sns_def_heap_mem)))

/*===========================================================================
  FUNCTION:   sns_heap_malloc()
  size        Size of memory to be allocated
  Returns the memory allocated
  ===========================================================================*/
void* sns_heap_malloc(uint32_t size)
{
      return mem_malloc(&sns_def_heap_state, size);
}

/*===========================================================================
  FUNCTION:   sns_heap_free()
  ptr         Pointer which is freed
  ===========================================================================*/
void sns_heap_free(void *ptr)
{
   if(SNS_OS_IS_HEAP_PTR(ptr))
   {
      mem_free(&sns_def_heap_state, ptr);
   }
   else
   {
      MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, "trying to free from non-heap pointer!!");
   }
}

/*===========================================================================
  FUNCTION:   sns_heap_create()
  Creates a dedicated heap for Sensors using memheap2
  ===========================================================================*/
sns_err_code_e sns_heap_create(void)
{
   //Initialize the heap using memheap
   //Memheap can handle any alignment of heap_start_ptr
   //heap_size need not be a multiple of any power of 2.
   mem_init_heap(&sns_def_heap_state, sns_def_heap_mem, sizeof(sns_def_heap_mem), NULL);

   return SNS_SUCCESS;
}

/*===========================================================================
  FUNCTION:   sns_heap_destroy()
  Removes the Sensors heap created using memheap2
  ===========================================================================*/
sns_err_code_e sns_heap_destroy(void)
{
   mem_deinit_heap(&sns_def_heap_state);

   return SNS_SUCCESS;
}

