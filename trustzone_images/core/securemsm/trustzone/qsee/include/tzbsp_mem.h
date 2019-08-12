#ifndef TZBSP_MEM_H
#define TZBSP_MEM_H

/**
@file tzbsp_mem.h
@brief Provide memory bound checking and managment
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_mem.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
06/01/12   sg       Added tzbsp_mem_is_tagged_area API
10/18/10   cap      Renamed file from tzbsp_sys.h to tzbsp_mem.h
05/24/10   tk       Initial version.

===========================================================================*/

/*------------------------------------------------------------------------
                            PUBLIC TYPES
------------------------------------------------------------------------*/

/*===========================================================================

                              DEFINES

===========================================================================*/
#include <comdef.h>
#include "tz_syscall_pub.h"

/**
 * Macro \c TZBSP_ALWAYS is an alias for assert. By default it is
 * compiled as empty. Use of \c TZBSP_ALWAYS can be enabled by
 * defining \c TZBSP_STRICT compile time.
 */
#ifdef TZBSP_STRICT
#define TZBSP_ALWAYS(x) assert(x)
#else
#define TZBSP_ALWAYS(x)
#endif

/*@}*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#define TZBSP_ROUNDUP(type, val, multiple) (((val) + (multiple)-1) & \
               ~(type)((multiple)-1))

#define TZBSP_ROUNDDOWN(type, val, multiple) ((val) & \
               ~(type)((multiple)-1))

#define TZBSP_MIN(a,b) ((a)>(b)?(b):(a))
#define TZBSP_MAX(a,b) ((a)>(b)?(a):(b))
#define TZBSP_DIFF(a,b) ((a)>(b)?(a)-(b):(b)-(a))

#define TZBSP_ARRAY_SIZE(xx) (sizeof(xx) / sizeof((xx)[0]))

#define TZBSP_PAGE_4K           4096
/* Clears the lowest 12 bits on xx. */
#define TZBSP_FLOOR_4K(xx)      (((uint32) (xx)) & ~(TZBSP_PAGE_4K-1))

/* Clears all apart from the lowest 12 bits on xx. */
#define TZBSP_PAGE_4K_OFFSET(xx) (((uint32) (xx)) & (TZBSP_PAGE_4K-1))

#define TZBSP_SECTION_1M        0x100000
#define TZBSP_FLOOR_1M(xx)      ((uint32) (xx) & ~(TZBSP_SECTION_1M-1))

/* Clears all apart from the lowest 20 bits on xx. */
#define TZBSP_SECTION_1M_OFFSET(xx) ((uint32) (xx) & (TZBSP_SECTION_1M-1))

#define TZBSP_PAGE_1K           1024
/* Clears the lowest 10 bits on xx. */
#define TZBSP_FLOOR_1K(xx)      ((xx) & ~(TZBSP_PAGE_1K-1))

#define TZBSP_ALIGN_4BYTE(xx)   ((xx) & ~0x3)

/** Helper macros for secure memory checking */
#define TZBSP_ERR_ADDR(func, ...) \
  do \
    if (func(__VA_ARGS__)) return -E_BAD_ADDRESS; \
  while (0)

/** @deprecated Only use the area/range functions. */
#define TZBSP_ERR_IS_SECURE(x) \
  TZBSP_ERR_ADDR(tzbsp_is_s_addr, x)

/** @deprecated Only use the area/range functions. */
#define TZBSP_ERR_IS_SECURE_RANGE(x, len) \
  TZBSP_ERR_ADDR(tzbsp_is_s_range, x, len)

#define TZBSP_BLACKLIST_ITEM_STATIC     0
#define TZBSP_BLACKLIST_ITEM_DYNAMIC    1
#define TZBSP_BLACKLIST_ITEM_ENABLED    2
#define TZBSP_BLACKLIST_ITEM_DISABLED   0 /* Dummy */

#define TZBSP_BLACKLIST_ID_NONE         0xffffffff

/**
 * Structure for mapping the DDR regions, which will be used for DDR pagetable
 * entries, and verifying pointers/structures reside in DDR.
 */
typedef struct tzbsp_ddr_region_s
{
  /* Start address of the DDR region */
  uint32 start;
  /* Size of the memory region, in bytes */
  uint32 size;
} tzbsp_ddr_region_t;

/**
 * Structure for defining secure memory areas. Area match is calculated as
 * start <= addr < end. In other words, the area end address is not included
 * in the area. This reflects the way area match is calculated in xPU
 * hardware.
 */
typedef struct tzbsp_blacklist_area_s
{
  /* Each blacklisted entry has a unique identifer */
  uint32 id;
  /* Flags */
  uint32 status;
  /* Start address of the memory, included in the area. */
  uint32 start;
  /* End address of the memory, excluded from the area. Value zero for the end
   * address means that the area extends to infinity. This is needed,
   * otherwise it wouldn't be possible to mark address 0xffff_ffff as secure
   * area (because the end address isn't included in the area). */
  uint32 end;
} tzbsp_blacklist_area_t;

/**
 * The default blacklist is defined by the target specific configuration.
 */
extern tzbsp_blacklist_area_t* g_tzbsp_blist;

/**
 * Check if any portion of given area is being serviced through AMT.
 */ 
extern boolean tzbsp_mpu_amt_is_overlap_amt(uint32 start, uint32 len);


/**
 * Check if any portion of given area is not being serviced 
 * through AMT. 
 */ 
extern boolean tzbsp_mpu_amt_is_not_overlap_amt(uint32 start, uint32 len);

/**
 * Find and return a blacklist area
 *
 * @param[in] id The id of blacklised memory area
 */
tzbsp_blacklist_area_t* tzbsp_blist_find_area(const tzbsp_blacklist_area_t* blist,
                                              uint32 id, uint32 status);

/**
 * @deprecated Only use the area/range based macros and functions.
 *
 * Tests whether an address is in non-secure memory. \c NULL is a
 * valid value for \c ptr, because physical addressing is used.
 *
 * @param [in] blist The blacklist containing secure memory areas.
 *
 * @param [in] ptr The address to be tested.
 *
 * @return \c TRUE if the address is in non-secure memory, \c FALSE
 * otherwise.
 */
boolean tzbsp_blist_is_ns_addr(const tzbsp_blacklist_area_t* blist,
                               const void* ptr);

/**
 * Tests whether all of the range [\c start, \c end] is in secure
 * memory. <b>Note that negating the result of this function does not
 * indicate non-secure area as the memory area might contain both
 * non-secure and secure memory</b>. If \c end < \c start, no error is
 * raised, this function will just swap the end points. \c NULL is a
 * valid value for \c start and \c end, because physical addressing is
 * used.
 *
 * @param [in] blist The blacklist containing secure memory areas.
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
boolean tzbsp_blist_is_s_area(const tzbsp_blacklist_area_t* blist,
                              const void* start, const void* end);

/**
 * Tests whether all of the range [\c start, \c end] is in non-secure
 * memory. <b>Note that negating the result of this function does NOT
 * indicate secure area as a memory area might contain both non-secure
 * and secure memory</b>. If \c end < \c start, no error is raised,
 * this function will just swap the end points. \c NULL is a valid
 * value for \c start and \c end, because physical addressing is used.
 *
 * @param [in] blist The blacklist containing secure memory areas.
 *
 * @param [in] start Start of the memory range, included in the range.
 *
 * @param [in] end End of the memory range, included in the range.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
boolean tzbsp_blist_is_ns_area(const tzbsp_blacklist_area_t* blist,
                               const void* start, const void* end);

/**
 * Configures a secure memory area start and end addresses. The enabled status
 * of the area is not modified by this function.
 *
 * @param [in] blist The blacklist containing secure memory areas.
 *
 * @param [in] id Index of the memory area to be modified.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address, included
 * in the range.
 *
 * @return Zero on success, error code otherwise.
 */
int tzbsp_blist_config_s_area(tzbsp_blacklist_area_t* blist, uint32 id,
                              uint32 start, uint32 end);

/**
 * Enables a secure memory area. Dynamically configured memory areas are
 * disabled by default. Statically configured memory areas are always enabled.
 *
 * @param [in] blist The blacklist containing secure memory areas.
 *
 * @param [in] id Index of the memory area.
 *
 * @return Zero on success, error code otherwise.
 */
int tzbsp_blist_enable_s_area(tzbsp_blacklist_area_t* blist, uint32 id);

/**
 * Disables a secure memory area. Only affects dynamically configured memory
 * areas, statically configured memory areas cannot be disabled.
 *
 * @param [in] blist The blacklist containing secure memory areas.
 *
 * @param [in] id Index of the memory area.
 *
 * @return Zero on success, error code otherwise.
 */
int tzbsp_blist_disable_s_area(tzbsp_blacklist_area_t* blist, uint32 id);

/* @copydoc tzbsp_blist_is_s_area_for_id
 *
 * Testing must be done against each individual secure memory
 * area. Testing just the end points is not enough as it is possible
 * to define a memory area that has end points in secure memory but
 * contains non-secure areas in between.
 */
boolean tzbsp_blist_is_s_area_for_id(const tzbsp_blacklist_area_t* blist, uint32 id,
                                     const void* start, const void* end);
									 

/**
 * Prints the arguments as \c tzbsp_log would and starts waiting in an infinite
 * loop.
 */
void tzbsp_abort(const char* fmt, ...);

/**
 * @deprecated Only use the area/range based macros and functions.
 *
 * Does the NS address check against the default blacklist.
 *
 * @see tzbsp_blist_is_ns_addr
 */
static inline boolean tzbsp_is_ns_addr(const void* ptr)
{
  return tzbsp_blist_is_ns_addr(g_tzbsp_blist, ptr);
}

/**
 * Does the NS area check against the default blacklist.
 *
 * @see tzbsp_blist_is_ns_area
 */
static inline boolean tzbsp_is_ns_area(const void* start, const void* end)
{
  return tzbsp_blist_is_ns_area(g_tzbsp_blist, start, end);
}

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * non-secure memory. This is a convenience function to access \c
 * tzbsp_is_ns_area. \c NULL is a valid value for \c start, because
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
static inline boolean tzbsp_is_ns_range(const void* start, uint32 len)
{
  /*
   * Return true if the area is non-secure and not locked through AMT
   */
  return (!tzbsp_mpu_amt_is_overlap_amt((uint32) start, len) && 
          tzbsp_is_ns_area(start, (const void*)((uint32)start + len - 1)));
}

/**
 * @deprecated Only use the area/range based macros and functions.
 *
 * Tests whether an address is in secure memory. \c NULL is a valid
 * value for \c ptr, because physical addressing is used.
 *
 * @param [in] ptr The physical address to be tested.
 *
 * @return \c TRUE if the address is in secure memory, \c FALSE
 * otherwise.
 */
static inline boolean tzbsp_is_s_addr(const void* ptr)
{
  return !tzbsp_is_ns_addr(ptr);
}

/**
 * Does the secure area check against the default blacklist.
 *
 * @see tzbsp_blist_is_s_area
 */
static inline boolean tzbsp_is_s_area(const void* start, const void* end)
{
  return tzbsp_blist_is_s_area(g_tzbsp_blist, start, end);
}

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * secure memory. This is a convenience function to access \c
 * tzbsp_is_s_area. \c NULL is a valid value for start, because
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
static inline boolean tzbsp_is_s_range(const void* start, uint32 len)
{
  return (!tzbsp_mpu_amt_is_not_overlap_amt((uint32) start, len) || 
          tzbsp_is_s_area(start, (const void*)((uint32)start + len-1)));
}

/**
 * Does the secure area config to the default blacklist.
 *
 * @see tzbsp_blist_config_s_area
 */
static inline int tzbsp_config_s_area(uint32 id, uint32 start, uint32 end)
{
  return tzbsp_blist_config_s_area(g_tzbsp_blist, id, start, end);
}

/**
 * Enables a secure area on the default blacklist.
 *
 * @see tzbsp_blist_enable_s_area
 */
static inline int tzbsp_enable_s_area(uint32 id)
{
  return tzbsp_blist_enable_s_area(g_tzbsp_blist, id);
}

/**
 * Enables a secure area on the default blacklist.
 *
 * @see tzbsp_blist_enable_s_area
 */
static inline int tzbsp_disable_s_area(uint32 id)
{
  return tzbsp_blist_disable_s_area(g_tzbsp_blist, id);
}

/**
 * Initialize TZBSP heap.
 */
void tzbsp_heap_init(void);

/**
 * Allocates memory from TZBSP heap.
 *
 * @param [in] The amount of memory bytes to allocate.
 *
 * @return A valid pointer to the allocated memory if allocation
 * succeeded, \c NULL if allocation failed. The returned address is
 * physical.
 */
void* tzbsp_malloc(uint32 size);

/**
 * Frees memory allocated from TZBSP heap.
 *
 * @param [in] The memory buffer to be deallocated, physical
 * address. If \c NULL, then freeing is ignored.
 */
void tzbsp_free(void* ptr);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * secure memory. This is a convenience function to access \c
 * tzbsp_is_s_area. \c NULL is a valid value for start, because
 * physical addressing is used.
 *
 * @param [in] id Index of the blacklist memory

 * @param [in] start Start of the memory range, physical addrss,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE if
 * the area contains non-secure memory.
 */
boolean tzbsp_is_s_range_for_id(uint32 id, const void* start, uint32 len);

/**
 * Takes a virtual memory buffer and converts it in to an iovec list of
 * physical memory pointers
 *
 * @param [in]  a virtual address
 *
 * @param [in]  the length of the virtual buffer to examine
 *
 * @param [out] Returns the virtual address as an array of type ioVec. The
 *              calling function needs to have allocated the ioVecPhys for
 *              population by this function.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if ioVecPhys is invalid
 *         E_FAILURE otherwise
 */
int tzbsp_vtop_iovec(uint32 v_addr, uint32 length, const void *pIoVecPhys);

/**
   Checks to see if the given memory range is completely within the
   range of DDR

   @param[in] start_ptr Start of the address range to check
   @param[in] len       Length of the address range to check

   @return \c TRUE iff the given memory region is contained in the area.
           \c FALSE otherwise.

*/
boolean tzbsp_ddr_contains_area(const void* start_ptr, uint32 len);

/**
 * Takes an array of DDR regions and coalesces them into the smallest set of 
 * continuous regions
 *
 * @param [in/out]  array of DDR regions, which will be merged in-place
 *
 * @param [in]      number of elements in the DDR regions array
 *
 * @return Number of remaining DDR regions
 */
uint32 tzbsp_coalesce_ddr_regions( tzbsp_ddr_region_t *regions, uint32 num_regions );

/**
   Takes a given memory memory mapped peripheral type and maps it into
   the virtual address space

   @param  mmp Enumerated value of memory mapped peripheral to map

   @return \c E_SUCCESS if mapping was successful
           \c E_FAILURE if mapping was not successful
 */
int tzbsp_mem_map_device(uint32 mmp);

/**
   Takes a given memory memory mapped peripheral type and unmaps it
   from the virtual address space

   @param  mmp Enumerated value of memory mapped peripheral to unmap

   @return \c E_SUCCESS if mapping was successful
           \c E_FAILURE if mapping was not successful
 */
int tzbsp_mem_unmap_device(uint32 mmp);

/**
   This unnmaps all peripherals from from the virtual address space

   @return \c E_SUCCESS if mapping was successful
           \c E_FAILURE if mapping was not successful
 */
int tzbsp_mem_unmap_all_devices(void);

/**
   This maps all peripherals from from the virtual address space

   @return \c E_SUCCESS if mapping was successful
           \c E_FAILURE if mapping was not successful
 */
int tzbsp_mem_map_all_devices(void);

/**
   This initializes the DDR translation tables and reads the DDR
   region configuration information
 */
int tzbsp_ddr_init(void);

boolean tzbsp_validate_security_for_shared_buffer(const void* start, uint32 len);


#endif /* TZBSP_MEM_H */
