#include <string.h> // for memset
#include "vm_mm.h"
#include "mmu_lpae.h"

// constants
#define HAP_R  1
#define HAP_W  2

#define IPA_RANGE_BIT_NUM     32
#define PT_MEM_PHYS_OFFSET    0

#define VTTBR_VMID_MASK       0xff000000000000LL
#define VTTBR_VMID_SHIFT      48

#define PT_CACHE_CLEAN_FUNC   NULL

// macros
#define PA2VA(paddr) (uint32_t)((int64_t)(paddr) - PT_MEM_PHYS_OFFSET)
#define VA2PA(vaddr) ((int64_t)(vaddr) + PT_MEM_PHYS_OFFSET)
#define MASK_RSHIFT(value, mask, shift) \
   (((uint64_t)(value) & (mask)) >> (shift))
#define MASK_LSHIFT(value, mask, shift) \
   (((uint64_t)(value) << (shift)) & (mask))

// types
typedef union HYP_VTCR_Type {
   struct {
      unsigned S_T0SZ:5;   // includes both signed ext and T0SZ
      unsigned reserved1:1;    
      unsigned SL0:2;
      unsigned IRGN0:2;
      unsigned ORGN0:2;
      unsigned SH0:2;
      unsigned reserved5:17;
      unsigned SBO:1;
   } r;
   uint32_t raw;
}
HYP_VTCR_Type;

extern void VMMM_Invalidate_BP_ALL(void);
extern void VMMM_Invalidate_TLB_BY_IPA(uint64_t addr);
extern void VMMM_SetupMMU(uint64_t vttbr, uint32_t vtcr);

// statically defined memory regions
extern const VMMM_attr_t *vm_map_table[];
extern const unsigned int vm_map_table_length[];

static const VMMM_attr_t * VMMM_FindMapInList(unsigned int vmid, 
      uint64_t ipa, uint64_t size)
{
   unsigned int idx;
   uint64_t map_ipa_end;

   if (!vm_map_table[vmid]) return NULL;

   map_ipa_end = ipa + size - 1;

   for (idx = 0; idx < vm_map_table_length[vmid]; idx++)
   {
      uint64_t curr_ipa_end;
      const VMMM_attr_t * p_curr_attr = &vm_map_table[vmid][idx];

      curr_ipa_end = p_curr_attr->ipa + p_curr_attr->size - 1;

      if (
            (ipa >= p_curr_attr->ipa) &&
            (map_ipa_end <= curr_ipa_end)
         )
      {
         return p_curr_attr;
      }
   }
   return NULL;
}

int VMMM_Init(uint32_t vmid, void * l1pt, VMMM_context_t * p_context)
{
   const int ipa_bit_num = 32 - IPA_RANGE_BIT_NUM;
   HYP_VTCR_Type vtcr;
   
   if (VMMM_MAX_CONTEXT_NUM <= vmid)
   {
      return VMMM_ERR_GENERIC;
   }

   vtcr.raw = 0x80000000;
   vtcr.r.SH0 = 3;   /* inner shareable */
   // vtcr.r.SH0 = 0;   /* not shareable */
   vtcr.r.SL0 = 1;   /* Start at L1*/
   // page table walk does not use cache to prevent cache incoherency 
   // with using caches on both secure and non-secure domains, as
   // the page table management is in secure domain, and page table walk
   // is in non-secure domain
   vtcr.r.IRGN0 = 1; /* WBWA */
   // vtcr.r.IRGN0 = 0; /* non-cacheable */
   //vtcr.r.IRGN0 = 2; /* WT */
   vtcr.r.ORGN0 = 1; /* WBWA */
   // vtcr.r.ORGN0 = 0; /* non-cacheable */
   // vtcr.r.ORGN0 = 2; /* WT */
   vtcr.r.S_T0SZ = (unsigned int)ipa_bit_num;
   p_context->vtcr = vtcr.raw;

   p_context->vttbr = (uint64_t)VA2PA(l1pt) |
      MASK_LSHIFT( vmid, VTTBR_VMID_MASK, VTTBR_VMID_SHIFT );

   p_context->l1pt = l1pt;

   if (vm_map_table[vmid])
   {
      unsigned int i;

      for (i = 0; i < vm_map_table_length[vmid]; i++) 
      {
         int ret;
         ret = VMMM_Map(p_context, &(vm_map_table[vmid][i]));
         if (VMMM_OK != ret) return ret;

      }
   }

   return VMMM_OK;
}

void VMMM_Enable(const VMMM_context_t * p_context)
{
   VMMM_SetupMMU(p_context->vttbr, p_context->vtcr);
}

int VMMM_MapFromList(const VMMM_context_t * p_context, unsigned int vmid, 
      uint64_t ipa, uint64_t size)
{
   const VMMM_attr_t * p_map_attr;
   p_map_attr = VMMM_FindMapInList(vmid, ipa, size);

   if (p_map_attr)
   {
      return VMMM_Map(p_context, p_map_attr);
   }
   else
   {
      return VMMM_ERR_GENERIC;
   }
}

int VMMM_Map(const VMMM_context_t * p_context, const VMMM_attr_t * p_map_attr)
{
   int ret;
   LPAE_map_attr_t map_attr;

   if (NULL == p_context->l1pt)
   {
      // context is uninitialized
      return VMMM_ERR_INVALID_HANDLE;
   }
   
   map_attr.input_addr = p_map_attr->ipa;
   map_attr.output_addr = p_map_attr->pa;
   map_attr.size = p_map_attr->size;
   map_attr.table_attr.raw = 0;
   map_attr.lower_block_attr.raw = 0;
   map_attr.upper_block_attr.raw = 0;

   if (p_map_attr->perm & VMMM_PERM_X)
   {
      // TODO: DEP is currently disabled here as stage 2 currently maps 
      // all memory accessed by VMs with a small number of memory regions
      // without knowledge of which area is used for executable code and 
      // which area is used for writable data
      // if (p_map_attr->perm & VMMM_PERM_W) return VMMM_ERR_GENERIC;
   }
   else
   {
      map_attr.upper_block_attr.S2.XN = 1;
   }

   if (p_map_attr->perm & VMMM_PERM_W) 
   {
      map_attr.lower_block_attr.S2.HAP |= HAP_W;
   }

   if (p_map_attr->perm & VMMM_PERM_R) 
   {
      map_attr.lower_block_attr.S2.HAP |= HAP_R;
   }

   map_attr.lower_block_attr.S2.MemAttr = p_map_attr->cache_attr;
   map_attr.lower_block_attr.S2.SH = p_map_attr->share_attr;
   map_attr.lower_block_attr.S2.AF = 1; // access is always allowed

   ret = LPAE_Map( p_context->l1pt, VMMM_Invalidate_TLB_BY_IPA, 
         PT_CACHE_CLEAN_FUNC, &map_attr );
   VMMM_Invalidate_BP_ALL();

   if (LPAE_OK == ret)
   {
      return VMMM_OK;
   }
   else
   {
      return (ret + VMMM_ERR_LPAE_OFFSET);
   }
}

int VMMM_Unmap(const VMMM_context_t * p_context, uint64_t ipa, 
      uint64_t size )
{
   int ret;
   LPAE_map_attr_t map_attr;

   if (NULL == p_context->l1pt)
   {
      // context is uninitialized
      return VMMM_ERR_INVALID_HANDLE;
   }

   map_attr.input_addr = ipa;
   map_attr.size = size;
   map_attr.output_addr = 0;
   map_attr.table_attr.raw = 0;
   map_attr.lower_block_attr.raw = 0;
   map_attr.upper_block_attr.raw = 0;

   ret = LPAE_Unmap( p_context->l1pt, VMMM_Invalidate_TLB_BY_IPA, 
         PT_CACHE_CLEAN_FUNC, &map_attr );
   VMMM_Invalidate_BP_ALL();

   if (LPAE_OK == ret)
   {
      return VMMM_OK;
   }
   else
   {
      return (VMMM_ERR_LPAE_OFFSET + ret);
   }
}
