/**
  @file mcfg_dynrec.c

  @brief  
*/
/*
  Copyright (c) 2014 QUALCOMM Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_dynrec/src/mcfg_dynrec_dpm.c#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/12/19   ao      Back to v1 with fix for RFC reclaiming disabled
 2014/12/15   hzhi    Updated to support Dynamic Reclaiming v2, for CR759155.
 2014/10/27   hzhi    Fix for CR731992.
 2014/10/22   hzhi    Fix for CR743592.
 2014/08/19   hzhi    Created for Dynamic Reclaiming.
===========================================================================*/
#ifdef FEATURE_DYNREC_RECLAIMING
#include <stdlib.h>

#include "mcfg_dynrec.h"
#include "modem_mem.h"        
#include "memheap.h"
#include "qurt.h"
//for mcfg_dynrec_reclaim(). 
#include "atomic_ops.h"

#define DYNREC_MAX_LINKER_ENTRIES 80
#define DYNREC_LOAD_ERROR 1
#define DYNREC_VENEER_ERROR
/*const int data_to_be_reclaimed_dummy[250000] = {1,2,34,6576565,890};*/

/* global data to check if reclaim request is valid. */
atomic_word_t dynrec_reclaimed_flag = {FALSE};

unsigned int dynrec_base_address;
unsigned int old_dynrec_base_address;

/*unsigned int data_reclaim_address_start;
unsigned int data_reclaim_address_end;*/

int dynrec_numentries;/*number of entries that were populated in the dynrec_linker_var_arr during post link*/

unsigned int dynrec_section_size; /*updated at post link, size that will be reclaimed*/

extern mem_heap_type modem_mem_heap;

extern char __dynrec_section_start__;
extern char __dynrec_section_start2__;
extern char __dynrec_section_end2__;
extern char __dynrec_section_end__;

extern void 
  mem_heap_add_section
  (
    mem_heap_type *heap_ptr, 
    void *section_start, 
    unsigned long section_size
  );

typedef struct dynrec_linker_var_arr_type{
  uint8_t valid;
  char str_id[50];
  uint32_t ro_text_address_begin;
  uint32_t ro_text_address_end;
    
  uint32_t data_address_begin;
  uint32_t data_address_end;
} dynrec_linker_var_arr_t;

//enum type for mcfg_dynrec_status
typedef enum
{
  MCFG_DYNREC_STATE_SUCCESS                       = 0,
  MCFG_DYNREC_STATE_LOAD_FAIL                     = 1,
  MCFG_DYNREC_STATE_RECLAIM_FAIL                  = 2,
  MCFG_DYNREC_STATE_LINKER_VAR_ARR_OVERFLOW       = 3,
  MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY  = 4,
  MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL        = 5,
  MCFG_DYNREC_STATE_MEM_REGION_QUERY_FAIL         = 6,
  MCFG_DYNREC_STATE_MEMSCPY_FAIL                  = 7,
  MCFG_DYNREC_STATE_FEATURE_DISABLED              = 8
}mcfg_dynrec_err_t;

//global variable for recording debug info only.
mcfg_dynrec_err_t mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;

dynrec_linker_var_arr_t dynrec_linker_var_arr[DYNREC_MAX_LINKER_ENTRIES] = {{0,"X",3,4, 0, 0}};

/*keeps track of how many modules were loaded before mcfg_dynrec_reclaim is called*/
int dynrec_regions_loaded = 0;

/*
Params[in]:
table_index - Index of which element to load dynamically from the dynrec_linker_var_arr array


Returns: 0 on Successfully copying and mapping the folder and a non zero value on Failure

Description: Pass an array index of the element that will be copied to a different region and mapped


*/

static int mcfg_dynrec_load_module(int table_index){
  int ret;
  unsigned int region_size_dynRec;
  unsigned int newAddr,i;
  unsigned int v_address_dynRec, p_address_unmapped_dynRec;
  qurt_mem_region_attr_t attr;
  qurt_mem_region_t region;
  unsigned int copy_size_dynrec;//to replace memcpy() with memscpy().
  
  qurt_mem_pool_t default_pool;
  
  qurt_mem_pool_attach ("DEFAULT_PHYSPOOL", &default_pool); 
  
  //aborting request to load invalid-indexed RF card.
  if(table_index < 0 || table_index >= DYNREC_MAX_LINKER_ENTRIES)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY;
    return DYNREC_LOAD_ERROR;
  }
  
  region_size_dynRec = dynrec_linker_var_arr[table_index].data_address_end - dynrec_linker_var_arr[table_index].data_address_begin ;
  if(region_size_dynRec <= 0)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY;
    return DYNREC_LOAD_ERROR;
  }
  
  qurt_mem_region_attr_init (&attr);
  
  qurt_mem_region_attr_set_mapping (&attr, QURT_MEM_MAPPING_VIRTUAL_RANDOM);
  
  ret = qurt_mem_region_create (&region, region_size_dynRec, default_pool, &attr);
  if(ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  ret = qurt_mem_region_attr_get (region, &attr);
  if(ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_QUERY_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  qurt_mem_region_attr_get_physaddr (&attr, &p_address_unmapped_dynRec);
  qurt_mem_region_attr_get_virtaddr (&attr, &v_address_dynRec);
  
  /*calculate new VA by finding delta between the old VAs and adding to the new base offset*/
  newAddr = dynrec_base_address + (dynrec_linker_var_arr[table_index].data_address_begin - old_dynrec_base_address);
  
  copy_size_dynrec = region_size_dynRec;
  if(memscpy((void*)v_address_dynRec, region_size_dynRec, (void*)newAddr, copy_size_dynrec) != copy_size_dynrec)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEMSCPY_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  ret = qurt_mapping_create ( dynrec_linker_var_arr[table_index].data_address_begin, p_address_unmapped_dynRec, region_size_dynRec, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_WRITE);
  if (ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;    
    return DYNREC_LOAD_ERROR;
  }

  newAddr = dynrec_base_address + (dynrec_linker_var_arr[table_index].ro_text_address_begin - old_dynrec_base_address); 
  region_size_dynRec = dynrec_linker_var_arr[table_index].ro_text_address_end - dynrec_linker_var_arr[table_index].ro_text_address_begin;
  if(region_size_dynRec <= 0)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY;
    return DYNREC_LOAD_ERROR;
  }
  
  qurt_mem_region_attr_init (&attr);
  
  qurt_mem_region_attr_set_mapping (&attr, QURT_MEM_MAPPING_VIRTUAL_RANDOM);
  
  ret = qurt_mem_region_create (&region, region_size_dynRec, default_pool, &attr);
  if (ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  ret = qurt_mem_region_attr_get (region, &attr);
  if (ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_QUERY_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  qurt_mem_region_attr_get_physaddr (&attr, &p_address_unmapped_dynRec);
  qurt_mem_region_attr_get_virtaddr (&attr, &v_address_dynRec);
  
  copy_size_dynrec = region_size_dynRec;
  if(memscpy((void*)v_address_dynRec, region_size_dynRec, (void*)newAddr, copy_size_dynrec) != copy_size_dynrec)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEMSCPY_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  ret = qurt_mapping_create ( dynrec_linker_var_arr[table_index].ro_text_address_begin, p_address_unmapped_dynRec, region_size_dynRec, QURT_MEM_CACHE_WRITEBACK, QURT_PERM_READ | QURT_PERM_EXECUTE);
  if (ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  //adding cache clean on RX mapped memory region which was 
  // initially mapped as RW, for CR743592.
  qurt_mem_cache_clean((qurt_addr_t)v_address_dynRec, region_size_dynRec,
		       QURT_MEM_CACHE_FLUSH_INVALIDATE, QURT_MEM_DCACHE);

  dynrec_linker_var_arr[table_index].valid = 0;
  return 0;
}

/*
Params[in]:
list - Pointer to the list of folder names that need to be loaded
numItems - number of items in the specified list

Returns: 0 on Successfully copying and mapping the folder(s) and a non zero value on Failure

Description: Pass a list of folder names that will be copied to a different region and mapped. This function can be called multiple times to add more folders


*/
int mcfg_dynrec_load (char* list[], int numItems){
  int i;
  int loaded_num = 0;
  int k;
  
  //special case: no dynrec.lst file included for linking, i.e., dynamic reclaiming feature disabled.
  if(&__dynrec_section_start2__ == &__dynrec_section_end2__)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_FEATURE_DISABLED;
    return 0;
  }
  
  if(dynrec_numentries > DYNREC_MAX_LINKER_ENTRIES)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LINKER_VAR_ARR_OVERFLOW;
    dynrec_numentries = DYNREC_MAX_LINKER_ENTRIES;
  }
  
  for(i =0; i < numItems; i++)
  {
    for (k = 0; k < dynrec_numentries; k++)
	{
      if((strcmp(list[i],dynrec_linker_var_arr[k].str_id) == 0))
	  {
#ifdef DYNREC_VENEER_ERROR
        if(k == 0)
		{
          loaded_num++;
          continue;
        }
#endif /* DYNREC_VENEER_ERROR */
        if(dynrec_linker_var_arr[k].valid == 1)/*if valid bit is not 1, then the section has already been loaded*/
		{ 
          if(mcfg_dynrec_load_module(k) == 0)
		  {
            loaded_num++;
          }
        }
        break;
      }
    }   
  }
  dynrec_regions_loaded += loaded_num;
  return (numItems - loaded_num);
}
/*
Param[out]:
num_used = number of folders that were successfully loaded

return 0 on Successful reclaiming and a non zero value on Failure

Description: Reclaims memory of all folders specified in the linker script. Can call this function only once


*/

/*=========================================================================================================
 * mark global flag dynrec_reclaimed_flag as TRUE if and only if this is the first time module is reclaimed.
 * otherwise directly return reclaiming failed.
 *=========================================================================================================*/
int mcfg_dynrec_reclaim(int * numused){

  //special case: no dynrec.lst file included for linking, i.e., dynamic reclaiming feature disabled.
  if(&__dynrec_section_start2__ == &__dynrec_section_end2__)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_FEATURE_DISABLED;
    return 0;
  }

  if(atomic_compare_and_set(&dynrec_reclaimed_flag, FALSE, TRUE))
  {
    //zero-initialize dynamic reclaimed module.
    memset((void *)dynrec_base_address, 0, (&__dynrec_section_end__ - &__dynrec_section_start__));
    mem_heap_add_section(&modem_mem_heap, (void *)dynrec_base_address, (unsigned long)(&__dynrec_section_end__ - &__dynrec_section_start__));
    //mem_heap_add_section(&modem_mem_heap, (void *)dynrec_base_address, (unsigned long)dynrec_section_size);
    *numused = dynrec_regions_loaded;
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;
    return 0;
  }
  mcfg_dynrec_last_error = MCFG_DYNREC_STATE_RECLAIM_FAIL;
  return 1;
}

/*

return 0 on Successful reclaiming and a non zero value on Failure

Description: Reclaims memory of first RF card in the dynamic reclaim section due to a presence of veneer in the section


*/
int mcfg_dynrec_init(void){
#ifdef DYNREC_VENEER_ERROR
  if (mcfg_dynrec_load_module(0) != 0)
  {
    return DYNREC_LOAD_ERROR;
  }
  else
  {
    dynrec_regions_loaded++;
  }
#endif /*DYNREC_VENEER_ERROR*/
  return 0;
}


/*

return 0 on Successful reclaiming and a non zero value on Failure
Description: Reclaims memory of all folders specified in the linker script. Can call this function only once


*/
/*
int data_reclaim(){
 mem_heap_add_section(&modem_mem_heap, (void *)dynrec_base_address, (unsigned long)(data_reclaim_address_end - data_reclaim_address_start));
 return 0;
}
*/
#endif /*FEATURE_DYNREC_RECLAIMING*/