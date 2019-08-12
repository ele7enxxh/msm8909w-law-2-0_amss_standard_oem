/**
  @file mcfg_mmu.c

  @brief  
*/
/*
  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_segment/src/mcfg_mmu.c#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
  04/28/15    hzhi    CR 827861; Added support for client to insert new entry into mcfg_mmu_table[] for fast VA<-->PA look up.
  07/25/14    skotalwa Updated for CR 699655: optimize MMU_table lookup API to fix MPPS performance hit.
  06/10/14    hzhi    Created for CR 670990: removing unnecessary dependency of mmu update on segment loading feature.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "sys_type.h"
#include "sys.h"
#include "dsm_init.h"

#include "mcfg_int.h"
#include "mcfg_mmu.h"
#include "modem_mem.h"
#include "memheap.h"
#ifdef FEATURE_MBA_RECLAIM
#include "mba_ac.h"
#endif 

/*------------------------------------------------------------------*
 * data type for mba reclaiming                                     *
 * 
 *------------------------------------------------------------------*/
#ifdef FEATURE_MBA_RECLAIM

#define  MBA_PHY_POOL	"MBA_PHY_POOL"
uint32   mcfg_mmu_MBA_VA = 0;
#ifndef  MBA_RECLAIM_SIZE
#define  MBA_RECLAIM_SIZE  0x80000
#endif
extern mem_heap_type modem_mem_heap;
void     mcfg_reclaim_MBA();
uint32   MBA_START_ADDRESS = MBA_PHYS_ADDR;
#endif /*FEATURE_MBA_RECLAIM*/
/*------------------------------------------------------------------*
 * local data types to manage mcfg MMU table to translate        *
 * heap section VA to PA upon query.                                *
 *------------------------------------------------------------------*/
mcfg_MMU_entry_t mcfg_MMU_table[MCFG_MMU_ENTRY_MAX_NUMBER];

/*------------------------------------------------------------------*
 * mcfg_mmu_table_last_entry points to last non-zero entry in MMU.  *
 *------------------------------------------------------------------*/
mcfg_MMU_entry_t *mcfg_mmu_table_last_entry = &mcfg_MMU_table[0];

/*------------------------------------------------------------------*
 * Global data recording last error of mmu update for debug         *
 * purpose.                                                         *
 *------------------------------------------------------------------*/
mcfg_mmu_update_status_t mcfg_mmu_update_last_error = MCFG_MMU_UPDATE_SUCCESS;

/*===========================================================================
variable used in segload_update_MMU_table() to make sure target translation
is loaded in TLB. 
===========================================================================*/
int tmp_mem_read = 0;

/*------------------------------------------------------------------*
 * Global data recording heap information returned from enumeration,*
 * Kept global for debugging purpose.                               *
 *------------------------------------------------------------------*/
mem_heap_section *section_info;

/*===========================================================================

FUNCTION mcfg_mmu_update

DESCRIPTION
  This function update mcfg_MMU_table[] after segment loading is done. 
  Every heap section is supposed to be added into this MMU table as 
  a separate entry, and for mcfg_VA_to_PA() and mcfg_PA_to_VA() api to
  query on. 

DEPENDENCIES
  corebsp supports modem mem heap enumeration api modem_mem_get_section_info(). 

RETURN VALUE
  MCFG_MMU_UPDATE_SUCCESS             --> mcfg_MMU_table[] updated.
  MCFG_MMU_UPDATE_TRANSLATION_FAIL    --> given VA cannot be translated to
                                          leagal PA by qurt_lookup_physaddr().
  MCFG_MMU_UPDATE_ENTRY_OVERFLOW_FAIL --> too many heap sections according to
                                          enumeration result. 
  MCFG_MMU_UPDATE_MALLOC_FAIL         --> cannot allocate enough memory for
                                          section_info[] ADT.

SIDE EFFECTS
  None.

===========================================================================*/
mcfg_mmu_update_status_t mcfg_mmu_update(void)
{
  int i = 0, j = 0;
  qurt_addr_t tmp_va, tmp_va_end;
  qurt_paddr_t tmp_pa;
  unsigned int tmp_offset, pre_offset = image_pstart - image_vstart;

  //first mmu table entry set for entire modem image except for dynamic heap.
  mcfg_MMU_table[0].va_start = 0;
  mcfg_MMU_table[0].va_end = 0xFFFFFFFF;
  mcfg_MMU_table[0].pa_start = 0;
  mcfg_MMU_table[0].pa_end = 0xFFFFFFFF;
  mcfg_MMU_table[0].offset = pre_offset;

  
  section_info = (mem_heap_section*)modem_mem_alloc(MEM_HEAP_SECTIONS_MAX*sizeof(mem_heap_section),44); 
  if(!section_info)
  {
    mcfg_mmu_update_last_error = MCFG_MMU_UPDATE_MALLOC_FAIL;
    return mcfg_mmu_update_last_error;
  }
  

  modem_mem_get_section_info((void **)&section_info);

  while(i<MEM_HEAP_SECTIONS_MAX)
  {
    if((section_info[i].start_addr != NULL)&&(section_info[i].size != 0))
    {
      tmp_va = (qurt_addr_t)(section_info[i].start_addr);
      tmp_va_end = tmp_va + (qurt_size_t)(section_info[i].size) - 1;
      
      tmp_mem_read += *((int *)tmp_va);
       
      if((tmp_pa = qurt_lookup_physaddr(tmp_va)) == 0)
      {
        mcfg_mmu_update_last_error = MCFG_MMU_UPDATE_TRANSLATION_FAIL;
        break;
      }

      tmp_offset = (unsigned int)tmp_pa - (unsigned int)tmp_va;

      j++;
      if(j >= MCFG_MMU_ENTRY_MAX_NUMBER)
      {
        mcfg_mmu_update_last_error = MCFG_MMU_UPDATE_ENTRY_OVERFLOW_FAIL;
        break;
      }
      mcfg_MMU_table[j].va_start = tmp_va;
      mcfg_MMU_table[j].va_end = tmp_va_end;
      mcfg_MMU_table[j].offset = tmp_offset;
    }
    i++;
  }
  //going back and update mmu table for sections from heap. 
  j = 1;

  while(mcfg_MMU_table[j].va_start != 0)
  {
    mcfg_MMU_table[j].pa_start = mcfg_MMU_table[j].va_start + mcfg_MMU_table[j].offset;
    mcfg_MMU_table[j].pa_end = mcfg_MMU_table[j].va_end + mcfg_MMU_table[j].offset;
    //mem_heap_add_section(&modem_mem_heap, (void *)mcfg_MMU_table[i].va_start, (unsigned long)(mcfg_MMU_table[i].va_end + 1 - mcfg_MMU_table[i].va_start));
    j++;
  }

  //point mcfg_mmu_table_last_entry to updated last non-zero entry of MMU table.
  mcfg_mmu_table_last_entry = &mcfg_MMU_table[j-1];
  return mcfg_mmu_update_last_error;
}


/*===========================================================================

FUNCTION mcfg_mmu_add_entry

DESCRIPTION
  This function allows user to request adding extra entry into mcfg_mmu_table.
  If requested entry is already covered by current table, no change will be 
  made; otherwise new table entry will be created.

DEPENDENCIES
  corebsp supports modem mem heap enumeration api modem_mem_get_section_info(). 

LIMITATIONS
  current version does not support adding mixed various-offset address ranges or
  address range smaller than one page, please make sure the v_start pssed in
  belong to the same contiguous memory chunk. 

RETURN VALUE
  0 - on success.
  1 - on error.

SIDE EFFECTS
  This API is NOT SAFE in multi-threading environment. This is intentional due 
  to performance concern. 
  Please use it with carefullness and do not add plenty small memory chunks 
  with this API. 

===========================================================================*/
int mcfg_mmu_add_entry(qurt_addr_t v_start, qurt_size_t v_size)
{
  //iterator.
  int i;
  int dummy_read;
  qurt_addr_t tmp_va_end;
  qurt_paddr_t tmp_pa, tmp_pa_end;
  unsigned int tmp_offset;
  mcfg_MMU_entry_t *p = mcfg_mmu_table_last_entry;
  mcfg_MMU_entry_t *q = mcfg_mmu_table_last_entry;

  //special case handler.
  if(v_start == 0 || v_size == 0 || v_size & 0x0fff){
    return 1;
  }
  
  tmp_va_end = v_start + v_size - 1;
  while(p > mcfg_MMU_table){//skip very first entry for this check on purpose.
    //coverage verified, no need to add new entry.
    if(p->va_start <= v_start && p->va_end >= tmp_va_end){
      return 0;
    }
    p--;
  }

  //if not yet covered, do update.
  dummy_read = *((int *)v_start);
  if((tmp_pa = qurt_lookup_physaddr(v_start)) == 0){
    return 1;
  }
  tmp_offset = (unsigned int)tmp_pa - (unsigned int)v_start;
  tmp_pa_end = tmp_pa + v_size - 1;
  
  //check chance to collapse new entry with existing one, iff same offset.
  p = mcfg_mmu_table_last_entry;
  while(p >= mcfg_MMU_table){//including very first entry for collapse purpose.
    if(p->offset == tmp_offset){
      //find proper existing entry to collapse with new one.
      if(p->va_start > tmp_va_end || p->va_end < v_start){
        continue;
      }
      if(p->va_start > v_start){
        p->va_start = v_start;
        p->pa_start = tmp_pa;
      }
      if(p->va_end < tmp_va_end){
        p->va_end = tmp_va_end;
        p->pa_end = tmp_pa_end;
      }
      return 0;
    }
    p--;
  }

  //if reaching here, it's impossible to collapse with existing entries. 
  //special case: overflow
  if(mcfg_mmu_table_last_entry == mcfg_MMU_table + MCFG_MMU_ENTRY_MAX_NUMBER - 1){
    return 1;
  }
  p = mcfg_mmu_table_last_entry + 1;
  p->va_start = v_start;
  p->va_end = tmp_va_end;
  p->pa_start = tmp_pa;
  p->pa_end = tmp_pa_end;
  p->offset = tmp_offset;
  mcfg_mmu_table_last_entry++;
  return 0;
}


/*===========================================================================

FUNCTION mcfg_mmu_update_init

DESCRIPTION
  This function is entry point to update mcfg_MMU_table[] after segment 
  loading is done. 

DEPENDENCIES
  modem_cfg_task(). 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcfg_mmu_update_init(void)
{
  #ifdef FEATURE_MBA_RECLAIM
   mcfg_reclaim_MBA();/*reclaim MBA buffer*/
  #endif

  if(mcfg_mmu_update() != MCFG_MMU_UPDATE_SUCCESS)
  {
    MCFG_MSG_HIGH("Modem Config MMU Table Update Failed.");
  }
  return;
}
#ifdef FEATURE_MBA_RECLAIM
/*===========================================================================

FUNCTION mcfg_reclaim_MBA

DESCRIPTION
  This function reclaims MBA buffer during boot up and adds this memory into heap. 

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcfg_reclaim_MBA(void)
{
  //uint32_t                     phys_addr = 0x80F80000;
  
  qurt_mem_pool_t              mem_pool; 
  qurt_mem_region_attr_t       mem_attribs;
  qurt_mem_region_t            mem_region;
  uint32_t                     *ptr;

  if(E_SUCCESS != mba_xpu_lock_region(MBA_PHYS_ADDR, (MBA_PHYS_ADDR+MBA_RECLAIM_SIZE), MBA_XPU_PARTITION_RECLAIM_MBA))
  {
     mcfg_mmu_MBA_VA = 1;
     MCFG_MSG_HIGH("mba_xpu_lock_region Failed.");
     return;
  }

  if(QURT_EOK != qurt_mem_pool_create(MBA_PHY_POOL, (MBA_PHYS_ADDR >> 12),(MBA_RECLAIM_SIZE>>12), &mem_pool))
  {
     mcfg_mmu_MBA_VA = 2;
     MCFG_MSG_HIGH("qurt_mem_pool_create Failed.");
     return;
  }

  qurt_mem_region_attr_init (&mem_attribs);
  qurt_mem_region_attr_set_cache_mode(&mem_attribs,QURT_MEM_CACHE_WRITEBACK);
  qurt_mem_region_attr_set_mapping (&mem_attribs, QURT_MEM_MAPPING_PHYS_CONTIGUOUS);
  qurt_mem_region_attr_set_type (&mem_attribs, QURT_MEM_REGION_SHARED);
  qurt_mem_region_attr_set_bus_attr(&mem_attribs, 1);
  mem_attribs.ppn = (unsigned int) (MBA_PHYS_ADDR >> 12);

  if(QURT_EOK == qurt_mem_region_create(&mem_region,
                                MBA_RECLAIM_SIZE, mem_pool, 
                                &mem_attribs))
  {
     qurt_mem_region_attr_get(mem_region,
                            &mem_attribs );

     qurt_mem_region_attr_get_virtaddr(&mem_attribs,
                                     (unsigned int *)&mcfg_mmu_MBA_VA);
     
     mem_heap_add_section(&modem_mem_heap,(void *)mcfg_mmu_MBA_VA,(unsigned long)MBA_RECLAIM_SIZE);
  }
  else
  {
     mcfg_mmu_MBA_VA = 3;
  }
}
#endif /*FEATURE_MBA_RECLAIM*/