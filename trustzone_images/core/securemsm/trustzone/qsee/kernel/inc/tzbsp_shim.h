#ifndef __QSEE_SHIM_H
#define __QSEE_SHIM_H

#include "lktypes.h"
#include "tzos_exec.h"

extern tzexec_func_group_crypto_ptr  crypto_funcs;

extern tzexec_func_group_boot_ptr  boot_funcs;

extern qsee_func_group_core_ptr  core_funcs;

extern qsee_func_group_securechannel_ptr  securechannel_funcs;

extern tzexec_func_group_timer_ptr  timer_funcs;

/**
 * Invalidate a memory region in the cache. Please note that the data
 * in the cache will not be written back to the main memory. However,
 * any further accesses to data in this region would result in a
 * cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void qsee_dcache_inval_region(void *addr, unsigned int length);

/**
 * Cleans and invalidates a memory region in the cache. Please note
 * that the data in the cache would be written back to main memory if
 * dirty and the region invalidated. So any further access to the data
 * would result in a cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void qsee_dcache_flush_region(void *addr, uint32_t  len);

/**
 * Cleans a memory region in the cache. Please note that this will
 * write back any data that is dirty but it will not invalidate the
 * cache region.  So any further accesses to data in this region would
 * result in a cache-hit.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
void qsee_dcache_clean_region(void *addr, unsigned int length);

void qsee_inval_all_tlb(void);

void qsee_inval_inst_tlb(void);

void qsee_inval_data_tlb(void);

#endif
