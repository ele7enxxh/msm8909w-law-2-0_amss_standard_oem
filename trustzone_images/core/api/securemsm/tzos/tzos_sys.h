#ifndef TZOS_SYS_H
#define TZOS_SYS_H

/**
@file tzos_sys.h
@brief Header file for TZOS system/CPU level functions.

@section S1 SYS Functions

The TZOS SYS API is a collection of system level configuration and
maintenance functions, including cache invalidation and cleaning,
interrupt disabling/enabling and secure/non-secure memory area checks.

@defgroup tzos_sys_group TZOS SYS Functions
@ingroup tzos_sys_group
@{
*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/securemsm/tzos/tzos_sys.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/24/10   tk       Initial version.

===========================================================================*/

/*===========================================================================

          INCLUDE FILES

============================================================================*/
#include "comdef.h"

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

/*===========================================================================

                              DEFINES

===========================================================================*/

/**
 * Macro \c ALWAYS is an alias for assert. By default it is compiled
 * as empty. Use of \c ALWAYS can be enabled by defining \c TZOS_STRICT
 * compile time.
 */
#ifdef TZOS_STRICT
#define ALWAYS(x) assert(x)
#else
#define ALWAYS(x)
#endif

/**
 * @name Interrupt mask bits
 *
 * Bit patterns for interrupt enabling/disabling. The bit locations
 * match the actual CPSR bits.
 *
 * @see \c tzos_int_enable and \c tzos_int_disable.
 */
/*@{*/
#define TZOS_EA_BIT     (1<<8) /**< External Abort mask bit */
#define TZOS_IRQ_BIT    (1<<7) /**< IRQ mask bit */
#define TZOS_FIQ_BIT    (1<<6) /**< FIQ mask bit */
/*@}*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/**
 * Enables data caching. This function does not invalidate the data
 * cache.
 */
void tzos_dcache_enable(void);

/**
 * Disables data caching. This function does not clean the data cache.
 */
void tzos_dcache_disable(void);

/**
 * Invalidates the entire data cache hierarchy.
 */
void tzos_dcache_inv(void);

/**
 * Invalidates the entire data cache hierarchy for the memory range
 * [\c start, \c end]. The range can be of any byte alignment. If \c
 * end < \c start, no error is raised, this function will just swap
 * the end points. \c NULL is a valid value for \c start and \c end,
 * because physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address, included
 * in the range.
 */
void tzos_dcache_inv_area(void* start, void* end);

/**
 * Invalidates the entire data cache hierarchy for the memory range
 * [\c start, \c start + \c len]. This is a convenience function to
 * access \c tzos_dcache_inv_area. \c NULL is a valid value for start,
 * because physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 */
inline void tzos_dcache_inv_range(void* start, uint32 len)
{
    tzos_dcache_inv_area(start, (void*)((uint32)start + len));
}

/**
 * Cleans the entire data cache hierarchy.
 */
void tzos_dcache_clean(void);

/**
 * Cleans the entire data cache hierarchy for the memory range [\c
 * start, \c end]. The range can be of any byte alignment. If \c end <
 * \c start, no error is raised, this function will just swap the end
 * points. \c NULL is a valid value for \c start and \c end, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end The end of the memory range, physical address,
 * included in the range.
 */
void tzos_dcache_clean_area(void* start, void* end);

/**
 * Cleans the entire data cache hierarchy for the memory range [\c
 * start, \c start + \c len]. This is a convenience function to access
 * \c tzos_dcache_clean_area. \c NULL is a valid value for \c start,
 * because physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 */
inline void tzos_dcache_clean_range(void* start, uint32 len)
{
    tzos_dcache_clean_area(start, (void*)((uint32)start + len));
}

/**
 * Enables L1 instruction cache. This function does not invalidate the
 * instruction cache.
 */
void tzos_icache_enable(void);

/**
 * Disables L1 instruction cache.
 */
void tzos_icache_disable(void);

/**
 * Invalidates the entire L1 instruction cache.
 */
void tzos_icache_inv(void);

/**
 * Invalidates the L1 instruction cache for the memory range [\c
 * start, \c end]. The range can be of any byte alignment. If \c end <
 * \c start, no error is raised, this function will just swap the end
 * points. \c NULL is a valid value for \c start and \c end, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end The end of the memory range, physical address,
 * included in the range.
 */
void tzos_icache_inv_area(void* start, void* end);

/**
 * Invalidates the L1 instruction cache for the memory range [\c
 * start, \c start + \c len]. This is a convenience function to access
 * \c tzos_icache_inv_area. \c NULL is a valid value for \c start,
 * because physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 */
inline void tzos_icache_inv_range(void* start, uint32 len)
{
    tzos_icache_inv_area(start, (void*)((uint32)start + len));
}

/**
 * Enables the selected interrupts.
 *
 * @param [in] flags Bit mask for enabling interrupts. Possible values
 * are \c TZOS_EA_BIT, \c TZOS_IRQ_BIT and \c TZOS_FIQ_BIT which can
 * be combined with bitwise OR, other bits are ignored.
 */
void tzos_int_enable(uint32 flags);

/**
 * Disables the selected interrupts.
 *
 * @param [in] flags Bit mask for disabling interrupts. Possible
 * values are \c TZOS_EA_BIT, \c TZOS_IRQ_BIT and \c TZOS_FIQ_BIT
 * which can be combined with bitwise OR, other bits are ignored.
 */
void tzos_int_disable(uint32 flags);

/**
 * Tests whether an address is in non-secure memory. \c NULL is a
 * valid value for \c ptr, because physical addressing is used.
 *
 * @param [in] ptr The address to be tested.
 *
 * @return \c TRUE if the address is in non-secure memory, \c FALSE
 * otherwise.
 */
boolean tzos_is_ns_addr(void* ptr);

/**
 * Tests whether all of the range [\c start, \c end] is in non-secure
 * memory. <b>Note that negating the result of this function does NOT
 * indicate secure area as a memory area might contain both non-secure
 * and secure memory</b>. If \c end < \c start, no error is raised,
 * this function will just swap the end points. \c NULL is a valid
 * value for \c start and \c end, because physical addressing is used.
 *
 * @param [in] start Start of the memory range, included in the range.
 *
 * @param [in] end End of the memory range, included in the range.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
boolean tzos_is_ns_area(void* start, void* end);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * non-secure memory. This is a convenience function to access \c
 * tzos_is_ns_area. \c NULL is a valid value for \c start, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
inline boolean tzos_is_ns_range(void* start, uint32 len)
{
    return tzos_is_ns_area(start, (void*)((uint32)start + len));
}

/**
 * Tests whether an address is in secure memory. \c NULL is a valid
 * value for \c ptr, because physical addressing is used.
 * 
 * @param [in] ptr The physical address to be tested.
 *
 * @return \c TRUE if the address is in secure memory, \c FALSE
 * otherwise.
 */
inline boolean tzos_is_s_addr(void* ptr)
{
    return !tzos_is_ns_addr(ptr);
}

/**
 * Tests whether all of the range [\c start, \c end] is in secure
 * memory. <b>Note that negating the result of this function does not
 * indicate non-secure area as the memory area might contain both
 * non-secure and secure memory</b>. If \c end < \c start, no error is
 * raised, this function will just swap the end points. \c NULL is a
 * valid value for \c start and \c end, because physical addressing is
 * used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address, included
 * in the range.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE if
 * the area contains non-secure memory.
 */
boolean tzos_is_s_area(void* start, void* end);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * secure memory. This is a convenience function to access \c
 * tzos_is_s_area. \c NULL is a valid value for start, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical addrss,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE if
 * the area contains non-secure memory.
 */
inline boolean tzos_is_s_range(void* start, uint32 len)
{
    return tzos_is_s_area(start, (void*)((uint32)start + len));
}

/**
 * Allocates memory from TZOS heap.
 *
 * @param [in] The amount of memory bytes to allocate.
 *
 * @return A valid pointer to the allocated memory if allocation
 * succeeded, \c NULL if allocation failed. The returned address is
 * physical.
 */
void* tzos_malloc(uint32 size);

/**
 * Frees memory allocated from TZOS heap.
 * 
 * @param [in] The memory buffer to be deallocated, physical
 * address. If \c NULL, then freeing is ignored.
 */
void tzos_free(void* ptr);

/**
 * Executes the Data Memory Barrier instruction (dmb).
 */
void tzos_dmb(void);

/**
 * Executes the Data Synchronization Barrier instruction (dsb).
 */
void tzos_dsb(void);

/**
 * Returns the CPU number of the current execution context.
 *
 * @return 0 for CPU0, 1 for CPU1, ..., n for CPUn.
 */
uint32 tzos_cur_cpu_num(void);

/** @} End of tzos_sys_group */
#endif /* TZOS_SYS_H */
