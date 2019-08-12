/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             SBL1 copy ddr debug file

GENERAL DESCRIPTION
  This module copies the data/code from L2 Cache and stores it in CodeRAM , this code gets
  executed after a watch dog reset .

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1_copy_ddr_debug.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/14   yps    Fixed wrong include file name Boot_cache_mmu.h
08/29/13   sl      Updated scl section names.
07/17/13   sr      Initial Version

===========================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "boot_target.h"
#include "boot_cache_mmu.h"
#include "boot_page_table_armv7.h"
#include "boot_util.h"
/*==============================================================================
                                  MACROS
==============================================================================*/
#undef  WORDSIZE
#define WORDSIZE   sizeof(int)
#undef  WORDMASK
#define WORDMASK   (WORDSIZE - 1)
//#define DDI_T32_DEBUG
#ifdef DDI_T32_DEBUG
boolean DDI_T32_DEBUG_FLAG=1;
#endif
/*
 * Base address of Apps PBL L2 page table for RPM CODE RAM:
 * it is 4th L2 page table (=0x400 bytes) after L1 page table (=0x4000 bytes).
 */
#define APPS_L2_RPM_CODE_RAM_PAGE_TABLE_BASE  (SCL_IMEM_BASE + 0x4000 + 0xC00)


/*==============================================================================
                                  DATA
==============================================================================*/
extern uint32  Load$$DDR_DEBUG_WDOG$$Base;
extern uint32  Image$$DDR_DEBUG_WDOG$$Base;
extern uint32  Image$$DDR_DEBUG_WDOG$$Length;

extern uint32  Load$$DDR_DEBUG_CODE$$Base;
extern uint32  Image$$DDR_DEBUG_CODE$$Base;
extern uint32  Image$$DDR_DEBUG_CODE$$Length;

extern uint32  Load$$DDR_DEBUG_RO$$Base;
extern uint32  Image$$DDR_DEBUG_RO$$Base;
extern uint32  Image$$DDR_DEBUG_RO$$Length;

extern uint32  Load$$DDR_DEBUG_RW$$Base;
extern uint32  Image$$DDR_DEBUG_RW$$Base;
extern uint32  Image$$DDR_DEBUG_RW$$Length;

extern uint32  Load$$DDR_DEBUG_RW_COPY$$Base;
extern uint32  Image$$DDR_DEBUG_RW_COPY$$Base;
extern uint32  Image$$DDR_DEBUG_RW_COPY$$Length;

extern uint32  Load$$DDR_DEBUG_ZI$$Base;
extern uint32  Image$$DDR_DEBUG_ZI$$Base;
extern uint32  Image$$DDR_DEBUG_ZI$$ZI$$Length;

extern uint32  Image$$COPY_ROM$$Base;
extern uint32  Load$$COPY_ROM$$Base;

extern uint32  Image$$COPY_ROM$$Length;

extern uint32  Image$$TCM_SDI$$Base;
extern uint32  Image$$TCM_SDI$$Length;
extern uint32  Load$$TCM_SDI$$Base;




const struct mem_block ddi_initial_memory_map[] =
{
  /* SBL L2/TC Cacheable */
  {
    SCL_SBL1_CODE_BASE,
    SCL_SBL1_CODE_BASE,
    SCL_SBL1_TOTAL_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE,
    MMU_L1_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L1_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN, },

  /* IMEM
     Page table region must be mapped as non-cachable to be consistent with 
     the setting in TTBR0(we set page table walk as non-cachable)*/
  { SCL_IMEM_BASE,
    SCL_IMEM_BASE,
    SCL_IMEM_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_MEM_NON_CACHEABLE,
    MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* DDR region, must be marked as shared device */
  { DDR_MEM_BASE,
    DDR_MEM_BASE, 
    DDR_MEM_SIZE >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_DEVICE_SHARED,
    MMU_L1_PAGETABLE_NON_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },
       
  /* Debug image & DDR driver region, must be marked as executable */
  { SCL_RPM_CODE_RAM_BASE,
    SCL_RPM_CODE_RAM_BASE, 
    (SCL_RPM_CODE_RAM_SIZE+0x10000) >> 10,
    MMU_L1_SECTION_MAPPING,
    MMU_L1_PAGETABLE_MEM_READ_WRITE, 
    MMU_L1_PAGETABLE_DEVICE_SHARED,
    MMU_L1_PAGETABLE_EXECUTABLE_REGION,
    MMU_PAGETABLE_DEF_DOMAIN,  },

  /* DO NOT add any addtional memory region as L2 section, 
     there is no more L2 page table available in IMEM page table*/
    
  /* Last Region Marker */
  { MMU_LAST_ADDRESS, MMU_LAST_ADDRESS }

};

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
static uint32 bLoad8 (const uint8* p)
{
  uint32 aligned_p = (uint32) p & (~0x00000003);
  uint32 index = (uint32) p & 0x00000003;

  return (( *((uint32*) aligned_p) >> (index<<3)) &  0x000000FF ) ;

}

static void bStor8 (uint8* p, uint32 val)
{
  uint32 aligned_p = (uint32) p & (~0x00000003);
  uint32 index = (uint32) p & 0x00000003;
  uint32 current_value = *((uint32*) aligned_p);
  *((uint32*) aligned_p) =  ((current_value & ~(0xFF << (index<<3)))) |
                             (val<< (index <<3));
}

static void qmemcpy( void* destination_ptr, const void* source_ptr, uint32 len )
{
  uint8       *dst_ptr = (uint8*) destination_ptr;
  const uint8 *src_ptr = (const uint8*) source_ptr;
  uint32      remainder;
  uint32      *src32_ptr;
  uint32      *dst32_ptr;
  do
  {
    if (len == 0 || (uint32)dst_ptr == (uint32)src_ptr)
        break;

    if ((uint32)dst_ptr < (uint32)src_ptr)
    {
        remainder = (uint32)src_ptr;
        if ((remainder | (uint32)dst_ptr) & WORDMASK)
        {
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len < WORDSIZE)
                remainder = len;
            else
               remainder = WORDSIZE - (remainder & WORDMASK);

            len -= remainder;
            if (remainder)
                do
                   bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
                while (--remainder);
        }

        remainder = len >> 2;
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do
              *dst32_ptr++ = *src32_ptr++;
           while (--remainder);

           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }

        remainder = len & WORDMASK;
        if (remainder)
            do
               bStor8(dst_ptr++, bLoad8((uint8*)src_ptr++));
            while (--remainder);
    }
    else
    {
        src_ptr += len;
        dst_ptr += len;
        remainder = (uint32)src_ptr;
        if ((remainder | (uint32)dst_ptr) & WORDMASK)
        {
            if ((remainder ^ (uint32)dst_ptr) & WORDMASK || len <= WORDSIZE)
               remainder = len;
            else
               remainder &= WORDMASK;
            len -= remainder;
            if (remainder)
                do
                  bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
                while (--remainder);
        }

        remainder = len >> 2;
        if (remainder)
        {
           src32_ptr = (uint32*) src_ptr;
           dst32_ptr = (uint32*) dst_ptr;
           do
              *--dst32_ptr = *--src32_ptr;
           while (--remainder);

           src_ptr = (uint8*) src32_ptr;
           dst_ptr = (uint8*) dst32_ptr;
        }

        remainder = len & WORDMASK;
        if (remainder)
            do
               bStor8(--dst_ptr, bLoad8((uint8*)--src_ptr));
            while (--remainder);
    }
  } while (0);

}

void ddr_debug_init( void )
{
  extern void ddr_debug_invalidate_tlb( void );

  #ifdef DDI_T32_DEBUG
  while(DDI_T32_DEBUG_FLAG);
  #endif

  ddr_debug_invalidate_tlb();

  boot_mmu_page_table_map_mem_block_list((uint32 *)SCL_RPM_CODE_RAM_BASE,ddi_initial_memory_map);

  qmemcpy((void *)&Image$$TCM_SDI$$Base,(void *)&Load$$TCM_SDI$$Base,(uint32)&Image$$TCM_SDI$$Length);
  qmemcpy((void *)&Image$$DDR_DEBUG_RW$$Base,(void *)&Load$$DDR_DEBUG_RW$$Base,(uint32)&Image$$DDR_DEBUG_RW$$Length);

  qmemcpy( (void *)(SCL_RPM_CODE_RAM_BASE+0x5000),
		 (void *)&Load$$COPY_ROM$$Base,
		 (SCL_RPM_CODE_RAM_SIZE+SCL_SYSTEM_DEBUG_SIZE-0X5000) );
}
