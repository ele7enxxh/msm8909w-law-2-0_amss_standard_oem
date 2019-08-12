/**
@file hyp_mm.c
@brief Hypervisor Memory Management
*/
/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/hypervisor/src/hyp_mm.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------

===========================================================================*/
#include <comdef.h>
#include <string.h> // for memset
#include "hyp_target.h"
#include "hyp_hvcapi.h"
#include "mmu_lpae.h"
#include "vm_mm.h"

// constants
#define L1PT_SIZE          32    // 4 entries of 8 bytes
#define HYP_L1PT_ADDR      VIRT_L1PT_MEMORY_ADDR
#define VM0_L1PT_ADDR      (VIRT_L1PT_MEMORY_ADDR + L1PT_SIZE)
#define PT_MEM_PHYS_OFFSET 0

// Hyperviser backup information data structure
typedef struct
{
   uint32_t magic;
   uint32_t src;
   uint32_t dest;
   uint32_t size;
}
hyp_backup_info_t;

extern void HYP_SetupMMU(uint64_t l1pt_addr);

VMMM_context_t vm0_context;

static int initHypPT(void)
{
   LPAE_map_attr_t map_attr;

   map_attr.input_addr = HYP_MEMORY_ADDR;
   map_attr.output_addr = HYP_MEMORY_ADDR;
   map_attr.size = HYP_MEMORY_SIZE;
   map_attr.table_attr.raw = 0;
   map_attr.lower_block_attr.raw = 0;
   map_attr.upper_block_attr.raw = 0;

   map_attr.lower_block_attr.S1.AttrIndx = 1; // map to normal DDR memory
   map_attr.lower_block_attr.S1.AP = 1; // read/write at any privilege level
   map_attr.lower_block_attr.S1.SH = 3; // inner shareable
   map_attr.lower_block_attr.S1.AF = 1; // access flag

   // cache clean is not needed as MMU is not yet enabled
   // TLB invalidate is not needed as this is the first and only mapping
   return LPAE_Map( (void *)HYP_L1PT_ADDR, NULL, NULL, &map_attr );
} 

void hyp_mm_warm_init(void)
{
   // setup MMU for hypervisor
   // MMU is enabled at the end of reset handler
   HYP_SetupMMU(HYP_L1PT_ADDR);

   // enable MMU for VM0
   VMMM_Enable(&vm0_context);
}

void hyp_mm_cold_init(void)
{
   int ret;

   // initialize the LPAE manager
   ret = LPAE_Init((void *)VIRT_PT_POOL_MEMORY_ADDR, 
      VIRT_PT_POOL_MEMORY_SIZE, PT_MEM_PHYS_OFFSET);
   while (LPAE_OK != ret); // loop forever

   // zero-init level 1 PT memory
   memset((void *)VIRT_L1PT_MEMORY_ADDR, 0, VIRT_L1PT_MEMORY_SIZE);

   // setup page table for hypervisor
   ret = initHypPT();
   while (LPAE_OK != ret); // loop forever

   // initialize page table for VM0
   ret = VMMM_Init(0, (void *)VM0_L1PT_ADDR, &vm0_context);
   while (LPAE_OK != ret); // loop forever
}


void hyp_config_download_mode_backup(uint32_t src_start, 
      uint32_t src_end)
{
   hyp_backup_info_t *p_info = (hyp_backup_info_t *)IMEM_BACKUP_ADDR;
   p_info->magic = IMEM_BACKUP_MAGIC;
   p_info->dest = HYP_COPY_ADDR;
   p_info->src = src_start;
   p_info->size = src_end - src_start;
}

int32 hyp32_grant_access(uint64_t ipa, uint64_t size, uint64_t perm,
                         uint64_t cache, uint64_t share)
{
   int          result;
   VMMM_attr_t  vmm_attr;

   if (NULL == vm0_context.l1pt)
   {
      // return success because we are in S2 MMU bypass mode
      return HVC_CALL_SUCCESS;
   }

   memset(&vmm_attr, 0, sizeof(VMMM_attr_t));

   vmm_attr.ipa            = ipa;
   vmm_attr.pa             = ipa;
   vmm_attr.size           = size;
   vmm_attr.perm           = perm;
   vmm_attr.cache_attr     = (VMMM_cache_t)cache;
   vmm_attr.share_attr     = (VMMM_share_t)share;

   result = VMMM_Map(&vm0_context, &vmm_attr);
   
   if (LPAE_OK == result) 
   {
     return HVC_CALL_SUCCESS;
   }

   return HVC_CALL_ERROR;
}

int32 hyp32_remove_access(uint64_t ipa, uint64_t size)
{
	int result;

   if (NULL == vm0_context.l1pt)
   {
      // return success because we are in S2 MMU bypass mode
      return HVC_CALL_SUCCESS;
   }

	result = VMMM_Unmap(&vm0_context, ipa, size);

	if (VMMM_OK == result) 
	{
	  return HVC_CALL_SUCCESS;
	}

	return HVC_CALL_ERROR;
}
