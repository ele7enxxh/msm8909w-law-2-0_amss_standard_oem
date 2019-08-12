/**
  @file mcfg_dynrec.c

  @brief  
*/
/*
  Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_dynrec/src/mcfg_dynrec.c#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2015/12/08   hzhi    CR 948934: Fix non-initialized variable in function mcfg_dynrec_checksum().
 2015/10/19   hzhi    CR 917530: Remove qurt_mapping_remove() function call.
 2015/08/28   hzhi    CR 898369: Added checksum calculation for loaded RF card.
 2015/07/21   hzhi    CR 876240: Support RF EFS card in dynamic reclaiming section.  
 2015/02/27   hzhi    CR 798480: enable dynamic reclaiming feature on JO1.0.
 2015/02/03   hzhi    Removed veneer issue workaround, for CR790598.
 2014/12/05   hzhi    Created to support Dynamic Reclaiming v2.
===========================================================================*/
#include <stdlib.h>
#include <stringl/stringl.h>

#include "mcfg_dynrec.h"
#include "modem_mem.h"        
#include "memheap.h"
#include "qurt.h"
//for mcfg_dynrec_reclaim(). 
#include "atomic_ops.h"

#define DYNREC_MAX_LINKER_ENTRIES 80
#define DYNREC_LOAD_ERROR 1

/* variables that are declared here and updated 
 * during post-linking process.
 */
unsigned int __attribute__((section (".data"))) dynrec_base_address;
unsigned int __attribute__((section (".data"))) old_dynrec_base_address;

/* leave for TBD data reclaiming API.
unsigned int data_reclaim_address_start;
unsigned int data_reclaim_address_end;
*/

/* number of entries that were populated in the 
 * dynrec_linker_var_arr during post linking.
 */
int __attribute__((section (".data"))) dynrec_numentries;

//updated at post link, size that will be reclaimed.
unsigned int __attribute__((section (".data"))) dynrec_section_size; 

/* keeps track of how many modules were loaded before 
 * mcfg_dynrec_reclaim is called.
 */
int dynrec_rfc_modules_loaded = 0;

extern mem_heap_type modem_mem_heap;
extern mem_heap_type amss_mem_heap;

/* linker symbols defined in dynrec_rfc.lst file: 
 * __dynrec_rfc_section_start__ and __dynrec_rfc_section_end__
 * indicate 'dummy' module boundaries;
 * __dynrec_rfc_section_start2__ and __dynrec_rfc_section_end2__
 * indicate if RFC module is really extracted.
 */
extern char __dynrec_rfc_section_start__;
extern char __dynrec_rfc_section_start2__;
extern char __dynrec_rfc_section_end2__;
extern char __dynrec_rfc_section_end__;

extern void 
  mem_heap_add_section
  (
    mem_heap_type *heap_ptr, 
    void *section_start, 
    unsigned long section_size
  );

//data type of dynrec_linker_var_arr[] entries.
typedef struct __attribute__((__packed__)) dynrec_linker_var_arr_type{
  uint8_t        client_id;
  uint8_t        is_module_loaded;
  char           str_id[50];
  atomic_word_t  is_client_reclaimed;
  uint32_t       text_address_begin;
  uint32_t       text_address_end;
  uint32_t       rodata_data_address_begin;
  uint32_t       rodata_data_address_end;
} dynrec_linker_var_arr_t;

//enum type for mcfg_dynrec_status
typedef enum
{
  MCFG_DYNREC_STATE_SUCCESS                          = 0,
  MCFG_DYNREC_STATE_UNINITIALIZED                    = 1,
  MCFG_DYNREC_STATE_INITIALIZED                      = 2,
  MCFG_DYNREC_STATE_LOAD_FAIL                        = 3,
  MCFG_DYNREC_STATE_RECLAIM_FAIL                     = 4,
  MCFG_DYNREC_STATE_LINKER_VAR_ARR_OVERFLOW          = 5,
  MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY     = 6,
  MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL           = 7,
  MCFG_DYNREC_STATE_MEM_REGION_QUERY_FAIL            = 8,
  MCFG_DYNREC_STATE_MEMSCPY_FAIL                     = 9,
  MCFG_DYNREC_STATE_LOAD_INVALID_CLIENT_REJECTED     = 10,
  MCFG_DYNREC_STATE_RECLAIM_INVALID_CLIENT_REJECTED  = 11,
  MCFG_DYNREC_STATE_LOAD_INVALID_REQUEST_REJECTED    = 12,
  MCFG_DYNREC_STATE_RECLAIM_INVALID_REQUEST_REJECTED = 13,
  MCFG_DYNREC_STATE_ON_RFC_DISABLED                  = 14,
  MCFG_DYNREC_STATE_LOAD_REQUEST_IN_PROCESS          = 15,
  MCFG_DYNREC_STATE_LOAD_FAILED_ON_RECLAIMED_MODULE  = 16,
  MCFG_DYNREC_STATE_LOAD_FAILED_ON_LOADEDED_MODULE   = 17,
  MCFG_DYNREC_STATE_QURT_MAPPING_REMOVE_FAIL         = 18
}mcfg_dynrec_err_t;

//global variable for recording debug info only.
mcfg_dynrec_err_t mcfg_dynrec_last_error =  MCFG_DYNREC_STATE_UNINITIALIZED;

/*=======================================================================
data structure to store information of each dynamic reclaiming patterned 
modules, and information will be written in during post-linking process.
=========================================================================*/
dynrec_linker_var_arr_t __attribute__((section (".data")))
  dynrec_linker_var_arr[DYNREC_MAX_LINKER_ENTRIES] = {{0, 0, "X", {0}, 3, 4, 0, 0}};

/* global data to check if reclaim request on RFC is valid 
   if feature is turned off.
 */
atomic_word_t dynrec_rfc_reclaim_flag = {0};


/*==============================================================================
 * FUNCTION mcfg_dynrec_checksum() to calculate xor checksum of loaded RF card 
 * at runtime. This is for debugging QSTABILITY-904233 in the future. 
 *
 * Params[in]:
 *   va_start - starting virtual address to calculate.
 *   va_size - size of virtual memory region that calculation covers.
 *
 * Returns: 
 *   crc - checksum calculated. 
 *
 *============================================================================*/

//global data checksum.
unsigned int mcfg_dynrec_crc_rw = 0;
unsigned int mcfg_dynrec_crc_rx = 0;

unsigned int mcfg_dynrec_checksum(unsigned int va_start, unsigned int va_size)
{
  unsigned int sa, sb, crc = 0;
  unsigned int va_end = va_start + va_size - 1;
  //set 4-byte size for checksum.
  unsigned int crc_size = 4;
  //set basic page size to be 4KB.
  unsigned int pg_size = 0x1000;
  unsigned int ptr;
  //validate address and size: must be multiple of 4KB.
  if( va_start > 0 && !(va_start & (pg_size - 1)) && 
      va_size > 0 && !(va_size & (pg_size - 1)))
  {
    //do xor for checksum of each 4K page.
    for(; va_start < va_end; va_start += pg_size){
      sa = 0; 
      sb = 0;
      for(ptr = va_start; ptr < va_start + pg_size; ptr += crc_size){
        sa += *((unsigned int *)ptr);
        sb += sa;
      }
      crc += sa ^ sb;
    }
  }
  return crc;
}


/*=========================================================================
Internal API for mcfg_dynrec_reclaim_ext() to call and load each
specified dynrec-pattern module. 

Params[in]:
  table_index - Index of which element to load dynamically from the 
                dynrec_linker_var_arr array

Returns: 
  0 on Successfully copying and mapping the folder, and 
  a non zero value on Failure

Description: 
  Pass an array index of the element that will be copied to a different 
  region and mapped
*==========================================================================*/
static int mcfg_dynrec_load_module(int table_index, mcfg_dynrec_client_t client_id)
{
  int ret;
  unsigned int region_size_dynRec;
  unsigned int newAddr,i;
  unsigned int v_address_dynRec, p_address_unmapped_dynRec;
  qurt_mem_region_attr_t attr;
  qurt_mem_region_t region;
  unsigned int copy_size_dynrec;//to replace memcpy() with memscpy().
  
  qurt_mem_pool_t default_pool;
  
  qurt_mem_pool_attach ("DEFAULT_PHYSPOOL", &default_pool); 
  
  //special case: invalid index.
  if(table_index < 0)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY;
    return DYNREC_LOAD_ERROR;
  }

  /* special case: loading request on non-valid including 
   * empty modules.
   */
  if(dynrec_linker_var_arr[table_index].rodata_data_address_end < dynrec_linker_var_arr[table_index].rodata_data_address_begin)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_INVALID_LINKER_VAR_ARR_ENTRY;
    return DYNREC_LOAD_ERROR;
  }
  region_size_dynRec = dynrec_linker_var_arr[table_index].rodata_data_address_end - 
                       dynrec_linker_var_arr[table_index].rodata_data_address_begin ;
  
  /* for new dynrec clients, simply mark related entry in 
   * dynrec_linker_var_arr[] to be "is_module_loaded" and done.
   */
  if(client_id != MCFG_DYNREC_CLIENT_RFC)
  {
    dynrec_linker_var_arr[table_index].is_module_loaded = 1;
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;
    return 0;
  }

  /* for only RFC client, copy out modules to be loaded and 
   * create TLB mapping.
   */
  //part I: loading rodata + data sections of RFC module.
  /**
   * EFS RF card does not have .data or .rodata section, so this entry will be 
   * empty, but now we need to support it, by simply skip this copy step and 
   * moving forward to copying .text section.
   **/
  if(region_size_dynRec > 0){
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
    
    /* calculate new VA by finding delta between the old VAs 
     * and adding to the new base offset
     */
    newAddr = dynrec_base_address + 
              (dynrec_linker_var_arr[table_index].rodata_data_address_begin - 
               old_dynrec_base_address);
    copy_size_dynrec = region_size_dynRec;
    if(memscpy((void*)v_address_dynRec, 
               region_size_dynRec, 
               (void*)newAddr, 
               copy_size_dynrec)
       != copy_size_dynrec)
    {
      mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEMSCPY_FAIL;
      return DYNREC_LOAD_ERROR;
    }

    //do check sum for data section of loaded RF card.
    mcfg_dynrec_crc_rw = mcfg_dynrec_checksum( v_address_dynRec, 
                                                region_size_dynRec);

    ret = qurt_mapping_create
          (
            dynrec_linker_var_arr[table_index].rodata_data_address_begin,
            p_address_unmapped_dynRec, 
            region_size_dynRec, 
            QURT_MEM_CACHE_WRITEBACK, 
            QURT_PERM_READ | QURT_PERM_WRITE
          );
    if (ret != QURT_EOK)
    {
      mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;    
      return DYNREC_LOAD_ERROR;
    }
  
  }

  //part II: loading text sections of RFC module.
  newAddr = dynrec_base_address + 
            (dynrec_linker_var_arr[table_index].text_address_begin - 
            old_dynrec_base_address); 
  region_size_dynRec = dynrec_linker_var_arr[table_index].text_address_end - 
                       dynrec_linker_var_arr[table_index].text_address_begin;
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
  if(memscpy((void*)v_address_dynRec, 
             region_size_dynRec, 
             (void*)newAddr, 
             copy_size_dynrec) 
     != copy_size_dynrec)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEMSCPY_FAIL;
    return DYNREC_LOAD_ERROR;
  }
  
  //do check sum for code section of loaded RF card.
  mcfg_dynrec_crc_rx = mcfg_dynrec_checksum( v_address_dynRec, 
                                              region_size_dynRec);

  ret = qurt_mapping_create
        (
          dynrec_linker_var_arr[table_index].text_address_begin, 
          p_address_unmapped_dynRec, 
          region_size_dynRec, 
          QURT_MEM_CACHE_WRITEBACK, 
          QURT_PERM_READ | QURT_PERM_EXECUTE
        );
  if (ret != QURT_EOK)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_MEM_REGION_CREATE_FAIL;
    return DYNREC_LOAD_ERROR;
  }

  /* adding cache clean on RX mapped memory region which was 
   * created and initially mapped as RW, for CR743592.
   */
  qurt_mem_cache_clean
  (
    (qurt_addr_t)v_address_dynRec, 
    region_size_dynRec,
    QURT_MEM_CACHE_FLUSH_INVALIDATE, 
    QURT_MEM_DCACHE
  );


  dynrec_linker_var_arr[table_index].is_module_loaded = 1;
  mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;
  return 0;
}

/*=========================================================================
FUNCTION: MCFG_DYNREC_LOAD_EXT()

Params[in]:
  list - Pointer to the list of folder names that need to be loaded
  client_id - indicate current request comes from which dynrec client.

Returns: 
  0 on Successfully copying and mapping the folder(s), and 
  a non zero value on Failure.

Description: 
  Pass a list of folder names that will be copied to a different 
  region and mapped. This function can be called multiple times to add 
  more folders.
*=========================================================================*/
int mcfg_dynrec_load_ext (char* module_to_load, mcfg_dynrec_client_t client_id)
{
  int i;
  int loaded_num = 0;

  //do not accept loading request when feature is disabled.
  if(client_id == MCFG_DYNREC_CLIENT_RFC && 
     mcfg_dynrec_last_error == MCFG_DYNREC_STATE_ON_RFC_DISABLED)
  {
    return 1;
  }
  mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_REQUEST_IN_PROCESS;
  /* current implementation requires each loading process to 
   * handle exactly one module.
   */
  if(!module_to_load)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_INVALID_REQUEST_REJECTED;
    return 1;
  }

  /* scan through data structure to find the "exactly single" 
   * module that match request.
   */
  for(i = 0; i < dynrec_numentries; i++)
  {
    //when find such module, pass its index to next step.
    if((strcmp(module_to_load, dynrec_linker_var_arr[i].str_id) == 0 && 
       client_id == dynrec_linker_var_arr[i].client_id))
    {
      break;
    }
  }

  if(i >= dynrec_numentries)
  {
    /* if nothing is found when for loop terminates, 
     * reject current reuqest as from non-supported client.
     */
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_INVALID_CLIENT_REJECTED;
    return 1;
  }

  /* error case: either target module is already loaded 
   * or it's been reclaimed.
   */
  if(dynrec_linker_var_arr[i].is_module_loaded == 1 || 
     dynrec_linker_var_arr[i].is_client_reclaimed.value == 1)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_INVALID_REQUEST_REJECTED;
    return 1;
  }

  // calling internal API to load specified module.
  if(mcfg_dynrec_load_module(i, client_id) == 0)
  {
    loaded_num++;
  }

  //increment num of loaded rfc modules after successful loading.
  if(client_id == MCFG_DYNREC_CLIENT_RFC)
  {
    dynrec_rfc_modules_loaded += loaded_num;
  }
  return (loaded_num - 1);
}

/*===========================================================================
FUNCTION: MCFG_DYNREC_LOAD()

Params[in]:
  list - Pointer to the list of folder names that need to be loaded.
  num_module_to_load - number of items in the specified list.

Returns: 
  0 on Successfully copying and mapping the folder(s), and 
  a non zero value on Failure.

Description: 
  Wrapper to invoke new API, for RFC to use only.
  Pass a list of folder names that will be copied to a different region 
  and mapped. This function can be called multiple times to add more folders
=============================================================================*/
int mcfg_dynrec_load (char* list[], int num_modules_to_load)
{
  int loaded_num = 0;
  int i;

  if(mcfg_dynrec_last_error == MCFG_DYNREC_STATE_ON_RFC_DISABLED)
  {
    return 1;
  }

  for(i = 0; i < num_modules_to_load; i++)
  {
    //special case: RFC loading request cause index out of boundary.
    if(i > DYNREC_MAX_LINKER_ENTRIES ||
       i > dynrec_numentries)
    {
      mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LINKER_VAR_ARR_OVERFLOW;
      break;
    }
    //load one RF card in each loop, so increment loaded_num by either 0 or 1.
    loaded_num += (1 - mcfg_dynrec_load_ext(list[i], MCFG_DYNREC_CLIENT_RFC));
  }
  return (num_modules_to_load - loaded_num);
}

/*===========================================================================
 * internal API to handle RFC module reclaiming.
 *
 * return value: 
 *   0 on success;
 *   1 on failure.
 *===========================================================================*/
int mcfg_dynrec_reclaim_RFC(void)
{
  unsigned int reclaim_start_va = 0, reclaim_end_va = 0;
  int i;
  
  if(!atomic_compare_and_set(&dynrec_rfc_reclaim_flag, 0, 1))
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_INVALID_REQUEST_REJECTED;
    return 1;
  }
  //zero-initialize dynamic reclaimed module.
  reclaim_start_va = dynrec_base_address;
  reclaim_end_va   = dynrec_base_address + (&__dynrec_rfc_section_end__ - &__dynrec_rfc_section_start__);
  memset((void *)reclaim_start_va, 0, (&__dynrec_rfc_section_end__ - &__dynrec_rfc_section_start__));
  mem_heap_add_section
  (
    &modem_mem_heap, 
    (void *)reclaim_start_va, 
    (unsigned long)(&__dynrec_rfc_section_end__ - &__dynrec_rfc_section_start__)
  );
  
  /* if dynamic reclaiming feature on RFC module is disabled, no 
   * dynrec_linker_var_arr[] entry will associate with RFC client.
   * thus only need to update is_client_reclaimed field when it's
   * enabled.
   */
  if(mcfg_dynrec_last_error != MCFG_DYNREC_STATE_ON_RFC_DISABLED)
  {
    for(i = 0; i < dynrec_numentries; i++)
    {
      if(MCFG_DYNREC_CLIENT_RFC == dynrec_linker_var_arr[i].client_id)
      {
        dynrec_linker_var_arr[i].is_client_reclaimed.value = 1;
      }
    }
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;
  }
  return 0;
}


/*=============================================================================
FUNCTION: MCFG_DYNREC_RECLAIM_EXT()

Param[in]:
  num_used = number of RFC modules that were successfully loaded: this
             value is updated at the end of function call for caller to 
             check for error checking on caller side.
  client_id = indicate current request comes from which dynrec client.

Returns:
  0 on Successful reclaiming, and 
  a non zero value on Failure.

Description: 
  Reclaims memory of all dynrec-pattern modules specified in the linker 
  script. 
  Loaded dynrec-pattern module(s) through MCFG_DYNREC_LOAD_EXT() API 
  will be kept in new memory chunk created and mapped in TLB, won't be 
  affected by this reclaim API.
  If called multiple times by same client, only the very first request
  will get processed and all other requests will get rejected with 
  non-zero return value.
*=============================================================================*/
int mcfg_dynrec_reclaim_ext(int* num_module_loaded, mcfg_dynrec_client_t client_id)
{
  unsigned int reclaim_start_va = 0, reclaim_end_va = 0;
  int i = 0;
  mem_heap_type *target_heap = &amss_mem_heap;

  //call internal API to handle RFC reclaiming to simplify implementation.
  if(client_id == MCFG_DYNREC_CLIENT_RFC)
  {
    *num_module_loaded = dynrec_rfc_modules_loaded;
    return mcfg_dynrec_reclaim_RFC();
  }
  
  mcfg_dynrec_last_error = MCFG_DYNREC_STATE_RECLAIM_FAIL;
  for(i = 0; i < dynrec_numentries; i++)
  {
    if(client_id == dynrec_linker_var_arr[i].client_id)
    {
      if(!atomic_compare_and_set(&dynrec_linker_var_arr[i].is_client_reclaimed, 0, 1))
      {
        mcfg_dynrec_last_error = MCFG_DYNREC_STATE_LOAD_INVALID_REQUEST_REJECTED;
        /* to allow multiple reclaiming tasks 
         * running by same client.
         */
        //return 1;
        continue;
      }
      /* for all new dynrec clients, continue to reclaim
       * other modules belonging to same client.
       */
      if(dynrec_linker_var_arr[i].is_module_loaded)
      {
        (*num_module_loaded)++;
        continue;
      }

      reclaim_start_va = (unsigned int)(dynrec_linker_var_arr[i].rodata_data_address_begin);
      reclaim_end_va = (unsigned int)(dynrec_linker_var_arr[i].rodata_data_address_end);
      memset((void *)reclaim_start_va, 0, (reclaim_end_va - reclaim_start_va));
      mem_heap_add_section
      (
        target_heap, 
        (void *)reclaim_start_va, 
        (unsigned long)(reclaim_end_va - reclaim_start_va)
      );
      //update debug info only when reclaiming succeeds.
      mcfg_dynrec_last_error = MCFG_DYNREC_STATE_SUCCESS;
    }
  }
  if(mcfg_dynrec_last_error == MCFG_DYNREC_STATE_SUCCESS)
  {
    return 0;
  }
  /*if current request comes from non-supported client or client has no
    modules listed in linker script files, reject such request.*/
  return 1;  
}


/*============================================================================
FUNCTION: MCFG_DYNREC_RECLAIM()

Param[in]:
  num_used = number of RFC modules that were successfully loaded: this
             value is updated at the end of function call for caller to 
             check for error checking on caller side.

Returns:
  0 on Successful reclaiming, and 
  a non zero value on Failure.

Description: 
  Reclaims memory of all RFC modules specified in the linker script. 
  Loaded RFC module(s) through MCFG_DYNREC_LOAD() API will be kept in
  new memory chunk created and mapped in TLB, won't be affected by this
  reclaim API.
  If called multiple times by RFC client, only the very first request
  will get processed and all other requests will get rejected with 
  non-zero return value.
*=============================================================================*/
int mcfg_dynrec_reclaim(int * num_module_loaded)
{
  return mcfg_dynrec_reclaim_ext(num_module_loaded, MCFG_DYNREC_CLIENT_RFC);
}

/*=======================================================================
FUNCTION: MCFG_DYNREC_INIT()

Param[in]:
  None.

Description: 
  Init function to check if RFC dynamic reclaiming is turned on or off:
    --> if turned on, will load first all-tech RFC module before any other
        as workaround for RFC dynrec veneer issue. 
    --> if turned off, will return non-zero value to indicate this.
  Should not be invoked by any caller other than modem_cfg_task().
*========================================================================*/
int mcfg_dynrec_init(void)
{
  if(&__dynrec_rfc_section_start2__ == &__dynrec_rfc_section_end2__)
  {
    mcfg_dynrec_last_error = MCFG_DYNREC_STATE_ON_RFC_DISABLED;
    return 1;
  }
  return 0;
}


/*

return 0 on Successful reclaiming and a non zero value on Failure
Description: Reclaims memory of all folders specified in the linker script. Can call this function only once


int mcfg_dynrec_data_reclaim()
{

 mem_heap_add_section(&modem_mem_heap, (void *)dynrec_base_address, (unsigned long)(data_reclaim_address_end - data_reclaim_address_start));

 return 0;
}
*/
