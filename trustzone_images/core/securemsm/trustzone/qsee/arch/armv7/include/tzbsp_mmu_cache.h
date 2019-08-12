#ifndef TZBSP_MMU_CACHE_H
#define TZBSP_MMU_CACHE_H

/*=============================================================================

                    Boot Cache MMU Header File

GENERAL DESCRIPTION
  This header file contains the public interface for the cache
  and mmu control functions.  Originally from here:
  //source/qcom/qct/core/pkg/modem/mp/arm11/rel/1.0/modem_proc/core/boot/secboot3/common/cache_mmu.h#3

Copyright 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
=============================================================================*/


/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/armv7/include/tzbsp_mmu_cache.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/11   pre     Initial Version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <comdef.h>


typedef struct
{
  /** The physical base to start the mapping at */
  uint64                p_base;

  /** The virtual base to start the mapping at */
  uint64                v_base;

  /** How much memory to map */
  uint32                size_in_kbytes;

  /** The memory attributes for the virtual memory being mapped */
  uint32                tz_cfg;

} mem_block_type;

/*===========================================================================

                       FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
 *  Function :  tzbsp_mmu_cache_init
 * ==========================================================================
 */
/**
 * Initialize and enable the MMU, and enable the I-cache and D-cache.
 */
__weak int tzbsp_mmu_cache_init(void);

/*===========================================================================
 *  Function :  tzbsp_mmu_cache_disable
 * ==========================================================================
 */
/**
 * Disable the MMU and caches.
 */
void tzbsp_mmu_cache_disable(void);

/*===========================================================================
 *  Function :  tzbsp_mmu_cache_re_enable
 * ==========================================================================
 */
/**
 * Re-enable the MMU and caches.
 */
__weak void tzbsp_mmu_cache_re_enable(void);

/*===========================================================================
 *  Function :  tzbsp_tlb_inval_all
 * ==========================================================================
 */
/**
 * Invalidate the entire unified TLB.
 */
void __weak tzbsp_tlb_inval_all(void);

/*===========================================================================
 *  Function :  tzbsp_itlb_inval_all
 * ==========================================================================
 */
/**
 * Invalidate the entire instruction TLB.
 */
void __weak tzbsp_itlb_inval_all(void);

/*===========================================================================
 *  Function :  tzbsp_tlb_inval_all_is
 * ==========================================================================
 */
/**
 * Invalidate the unified TLB inner shared
 */
void __weak tzbsp_tlb_inval_all_is(void);

/*===========================================================================
 *  Function :  tzbsp_tlb_inval_by_asid_is
 * ==========================================================================
 */
/**
 * Invalidate the unified TLB by ASID inner shared
 */
void __weak tzbsp_tlb_inval_by_asid_is(uint32 asid);

/*===========================================================================
 *  Function :  tzbsp_tlb_inval_by_mva_is
 * ==========================================================================
 */
/**
 * Invalidate the unified TLB by MVA inner shared
 */
void __weak tzbsp_tlb_inval_by_mva_is(uint32 mva);

/*===========================================================================
 *  Function :  tzbsp_dtlb_inval_all
 * ==========================================================================
 */
/**
 * Invalidate the entire data TLB.
 */
void __weak tzbsp_dtlb_inval_all(void);

/*===========================================================================
 *  Function :  tzbsp_cache_flush
 * ==========================================================================
 */
/**
 * Flushes the entire cache system.  The data cache flush is achieved
 * using atomic clean / invalidates working outwards from L1
 * cache. This is done using Set/Way based cache maintainance
 * instructions.  The instruction cache can still be invalidated back
 * to the point of unification in a single instruction.  This also
 * flushes the branch target cache.
 */
void __weak tzbsp_cache_flush(void);

/*===========================================================================
 *  Function :  tzbsp_dcache_clean_all
 * ==========================================================================
 */
/**
 * Clean the data cache system.  The clean is achieved using clean
 * operations working outwards from L1 cache. This is done using
 * Set/Way based cache maintainance instructions.
 */
void tzbsp_dcache_clean_all(void);

/*===========================================================================
 *  Function :  tzbsp_dcache_inval_all
 * ==========================================================================
 */
/**
 * Inval the data cache system.  The invalidate is achieved using
 * invalidate operations working outwards from L1 cache. This is done
 * using Set/Way based cache maintainance instructions.
 */
void tzbsp_dcache_inval_all(void);

/*===========================================================================
 *  Function :  tzbsp_dcache_flush_all
 * ==========================================================================
 */
/**
 * Flush the data cache system.  The flush is achieved using atomic
 * clean / invalidates working outwards from L1 cache. This is done
 * using Set/Way based cache maintainance instructions.
 */
void tzbsp_dcache_flush_all(void);

/*===========================================================================
 *  Function :  tzbsp_dcache_clean_region
 * ==========================================================================
 */
/**
 * Cleans a memory region in the cache. Please note that this will
 * write back any data that is dirty but it will not invalidate the
 * cache region.  So any further accesses to data in this region would
 * result in a cache-hit.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void tzbsp_dcache_clean_region(void *addr, unsigned int length);

/*===========================================================================
 *  Function :  tzbsp_dcache_inval_region
 * ==========================================================================
 */
/**
 * Invalidate a memory region in the cache. Please note that the data
 * in the cache will not be written back to the main memory. However,
 * any further accesses to data in this region would result in a
 * cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void tzbsp_dcache_inval_region(void *addr, unsigned int length);

/*===========================================================================
 *  Function :  tzbsp_dcache_flush_region
 * ==========================================================================
 */
/**
 * Cleans and invalidates a memory region in the cache. Please note
 * that the data in the cache would be written back to main memory if
 * dirty and the region invalidated. So any further access to the data
 * would result in a cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void tzbsp_dcache_flush_region(void *addr, unsigned int length);

/*===========================================================================
 *  Function :  tzbsp_icache_inv
 * ==========================================================================
 */
/**
 * Instuction cache invalidate, inner shared
 */
void __weak tzbsp_icache_inv(void);

/*===========================================================================
 *  Function :  tzbsp_mmu_disable
 * ==========================================================================
 */
/**
 * Disable the ARM MMU. This function does not perform any cache maintenance
 */
void tzbsp_mmu_disable(void);

/*===========================================================================
 *  Function :  tzbsp_mmu_enable
 * ==========================================================================
 */
/**
 * Enable the ARM MMU. This function does not perform any cache maintenance
 */
void tzbsp_mmu_enable(void);

/*===========================================================================
 *  Function :  tzbsp_mmu_is_enabled
 * ==========================================================================
 */
/**
 * Return the current state of the MMU. TRUE if the MMU is currently enabled
 */
uint32 tzbsp_mmu_is_enabled(void);

/*===========================================================================
 *  Function :  tzbsp_vtop
 * ==========================================================================
 */
/**
 * Converts virtual memory pointer to physical address
 *
 * @param v_addr virtual address
 * @param p_addr physical address associated with the virtual address
 *
 * @returns  E_SUCCESS on success
 *          -E_FAILURE on failure
 */
uint32 tzbsp_vtop(uint32 v_addr, uint32 *p_addr);

/**
   Maps a given memory block in the TZ secure translation table(s)

   @param map Structure that contains information relevant for mapping
              memory in the MMU

   @returns  E_SUCCESS on success
            -E_FAILURE on failed
            -E_NO_MEMORY when memory couldn't be alloced for an L2 page

   @pre interrupt locks may be required depending on memory regions
        being mapped

   @post TLB maintenance may be required after calling this function
 */
int tzbsp_mmu_map_block(const mem_block_type  * map);

/**
   Maps a given memory block in the TZ secure translation table(s)

   @param p_addr   physical address start
   @param v_addr   virtual address start
   @param sz_in_kb size of region to map in KB
   @param msk      memory attributes

   @returns  E_SUCCESS on success
            -E_FAILURE on failed
            -E_NO_MEMORY when memory couldn't be alloced for an L2 page

   @pre interrupt locks may be required depending on memory regions
        being mapped

   @post TLB maintenance may be required after calling this function
 */
int tzbsp_mmu_map(uint32 p_addr, uint32 v_addr, uint32 sz_in_kb, uint32 msk);

/**
   Maps a given memory block in the TZ secure translation table(s) and
   adds mapping information to default region map

   @param map Structure that contains information relevant for mapping
              memory in the MMU

   @returns  E_SUCCESS on success
            -E_FAILURE on failed
            -E_NO_MEMORY when memory couldn't be alloced for an L2 page

   @pre interrupt locks may be required depending on memory regions
        being mapped

   @post TLB maintenance may be required after calling this function
 */
int tzbsp_mmu_map_block_default_region(const mem_block_type  * map);

/**
   Adds mapping information to default region map

   @param map Structure that contains information relevant for mapping
              memory in the MMU

   @returns  E_SUCCESS on success
            -E_FAILURE on failed

   @pre this function is useful only if it is called before enabling MMU
        during Cold Boot.
 */
int tzbsp_mmu_add_block_default_region(const mem_block_type  * map);

/**
   Maps a given memory block in the TZ secure translation table(s) and
   adds mapping information to default region map

   @param p_addr   physical address start
   @param v_addr   virtual address start
   @param sz_in_kb size of region to map in KB
   @param msk      memory attributes

   @returns  E_SUCCESS on success
            -E_FAILURE on failed
            -E_NO_MEMORY when memory couldn't be alloced for an L2 page

   @pre interrupt locks may be required depending on memory regions
        being mapped

   @post TLB maintenance may be required after calling this function
 */
int tzbsp_mmu_map_default_region(uint32 p_addr, uint32 v_addr,
                                 uint32 sz_in_kb, uint32 msk);

/**
   Adds mapping information to default region map

   @param p_addr   physical address start
   @param v_addr   virtual address start
   @param sz_in_kb size of region to map in KB
   @param msk      memory attributes

   @returns  E_SUCCESS on success
            -E_FAILURE on failed
 */
int tzbsp_mmu_add_default_region(uint32 p_addr, uint32 v_addr,
                                 uint32 sz_in_kb, uint32 msk);

/**
   Retrieves the TZ configuration for a given virtual address and size
   for the default TZBSP mapping of this region.

   @param v_addr   virtual address start
   @param sz_in_kb size of region in KB
   @param tzcfg    memory attributes

   @returns  E_SUCCESS       On success.
            -E_FAILURE       tzcfg pointer NULL
            -E_NOT_FOUND     If there is no default mapping for the
                             given memory range.
            -E_NOT_SUPPORTED The given is described by multiple mappings.

   @pre  interrupt locks may be required
 */
int tzbsp_get_default_region_tz_cfg(uint32 v_addr, uint32 sz_in_kb,
                                    uint32* tzcfg);

/*===========================================================================
 *  Function :  tzbsp_mmu_disable_and_configure_ns
 * ==========================================================================
 */
/**
 * Invalidates L1 and TLB before disabling the MMU.  Configures the NS
 * translation table registers, and enables the caches and MMU for NS-world.
 */
void tzbsp_mmu_disable_and_configure_ns(void);

#endif /* TZBSP_MMU_CACHE_H */
