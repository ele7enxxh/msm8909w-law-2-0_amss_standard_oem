/*
@file tzbsp_sys.c
@brief Trustzone System Services

Functions for system/processor level functionality.

*/
/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/src/tzbsp_mem.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
06/01/12   sg      Add tzbsp_mem_is_tagged_area stub
05/19/10   tk      Initial version.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <comdef.h>
#include <DALSys.h>
#include <IxErrno.h>

#include "tzbsp.h"
#include "tzbsp_cp.h"
#include "tzbsp_mem.h"
#include "tzbsp_log.h"
#include "tzbsp_target.h"
#include "tzbsp_uio.h"
#include "tt_tz.h"
#include "tzbsp_peripheral_info.h"
#include "tzbsp_xpu_config.h"
#include "tzbsp_memory_map.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

#define TZBSP_RETCHK(xx) if ((xx) != E_SUCCESS) {return (xx);}

extern void tzbsp_reconfig_black_list(uint32 , uint32);
extern int tzbsp_read_ddr_size(uint32*             num_ddr_regions,
                               tzbsp_ddr_region_t* ddr_regions,
                               uint32              ddr_regions_sz);

/**
   The DDR regions are defined during the cold init of each chipset
 */
tzbsp_ddr_region_t tzbsp_ddr_regions[TZBSP_NUM_DDR_REGIONS];
uint32             tzbsp_num_ddr_regions = 0;
uint32             tzbsp_ddr_max_addr = TZBSP_DDR_MAX_ADDR;


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/

/* Tests whether memory range [\c p1, \c p2] is fully contained in a
 * secure memory area. The order of \c p1 and \c p2 doesn't matter.
 *
 * @param [in] area A secure memory area.
 *
 * @param [in] p1 The start of the tested memory range.
 *
 * @param [in] p2 The end of the tested memory range.
 *
 * @return \c TRUE iff \c p1 and \c p2 both are contained in the \c
 * area, \c FALSE otherwise.
 */
static inline boolean contains(const tzbsp_blacklist_area_t* area,
                               const void* p1, const void* p2)
{
  if(0 == area->end)
  {
    if(area->start <= (uint32)p1 && area->start <= (uint32)p2)
      return TRUE;
  }
  else if(area->start <= (uint32)p1 && (uint32)p1 < area->end &&
          area->start <= (uint32)p2 && (uint32)p2 < area->end)
  {
    return TRUE;
  }

  return FALSE;
}

/* Tests whether memory range [\c p1, \c p2] overlaps secure memory area.
 *
 * @param [in] area A secure memory area.
 *
 * @param [in] p1 The start of the tested memory range.
 *
 * @param [in] p2 The end of the tested memory range.
 *
 * @return TRUE iff \c p1 is contained in \c area, or \c p2 is
 * contained in \c area, or \c area is contained in [\c p1, \c p2].
 */
static inline boolean overlaps(const tzbsp_blacklist_area_t* area,
                               const void* p1, const void* p2)
{
  if(0 == area->end)
  {
    if(area->start <= (uint32)p1 || area->start <= (uint32)p2)
      return TRUE;
  }
  else if((area->start <= (uint32)p1 && (uint32)p1 < area->end) ||
     (area->start <= (uint32)p2 && (uint32)p2 < area->end) ||
     ((uint32)p1 < area->start && area->end <= (uint32)p2))
  {
    return TRUE;
  }

  return FALSE;
}

/* @copydoc tzbsp_abort */
void tzbsp_abort(const char* fmt, ...)
{
  volatile int wait = 1;
  va_list ap;

  va_start(ap, fmt);
  tzbsp_log_with_ap(TZBSP_MSG_FATAL, fmt, ap);
  va_end(ap);

  /* Start waiting in an infinite loop. */
  while(wait);
}

/* @copydoc tzbsp_blist_find_area */
tzbsp_blacklist_area_t* tzbsp_blist_find_area(const tzbsp_blacklist_area_t* blist,
                                              uint32 id, uint32 status)
{
  uint32 i = 0;

  TZBSP_ALWAYS(NULL != blist)

  for(; TZBSP_BLACKLIST_ID_NONE != blist[i].id; ++i)
  {
    if ((blist[i].id == id) && (status & blist[i].status))
      return (tzbsp_blacklist_area_t*)(&blist[i]);
  }
  return NULL;
}

/* @copydoc tzbsp_blist_is_ns_addr */
boolean tzbsp_blist_is_ns_addr(const tzbsp_blacklist_area_t* blist,
                               const void* ptr)
{
  uint32 i;
  TZBSP_ALWAYS(NULL != blist);

  /* If the ptr location is contained in any of the secure areas, it
   * is not on non-secure area. */
  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if(!(TZBSP_BLACKLIST_ITEM_ENABLED & blist[i].status))
      continue;
    else if(blist[i].start <= (uint32)ptr)
    {
      if(0 == blist[i].end) /* The area is open ended. */
        return FALSE;
      else if((uint32)ptr < blist[i].end)
        return FALSE;
    }
  }

  return TRUE;
}

/* @copydoc tzbsp_blist_is_s_area
 *
 * Testing must be done against each individual secure memory
 * area. Testing just the end points is not enough as it is possible
 * to define a memory area that has end points in secure memory but
 * contains non-secure areas in between.
 */
boolean tzbsp_blist_is_s_area(const tzbsp_blacklist_area_t* blist,
                              const void* start, const void* end)
{
  uint32 i;
  TZBSP_ALWAYS(NULL != blist);

  if(end < start)
  {
    /* Either the specified addresses are incorrect, or there has been a
       wrap-around of the end address. Return False either way.
    */
    return FALSE;
  }

  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if(!(TZBSP_BLACKLIST_ITEM_ENABLED & blist[i].status))
      continue;
    else if(contains(&blist[i], start, end))
      return TRUE;
  }

  /* Either [start, end] is fully non-secure area or it only partially
   * overlapped some secure area. */
  return FALSE;
}

/* @copydoc tzbsp_blist_is_ns_area
 *
 * Testing must be done against each individual secure memory
 * area. Testing just the end points is not enough as it is possible
 * to define a memory area that has end points in non-secure memory
 * but contains secure areas in between.
 *
 * NOTE: This function returns TRUE value if the area is completely in
 * non-secure area. For all other cases it returns FALSE (which could mean
 * the area could be partly in non-secure and partly in secure or
 * because the end address is smaller than the start address
 * (incase of integer overflow of start + length).
 */
boolean tzbsp_blist_is_ns_area(const tzbsp_blacklist_area_t* blist,
                               const void* start, const void* end)
{
  uint32 i;
  TZBSP_ALWAYS(NULL != blist);

  if(end < start)
  {
    /* Either the specified addresses are incorrect, or there has been a
       wrap-around of the end address.
    */
    return FALSE;
  }

  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if(!(TZBSP_BLACKLIST_ITEM_ENABLED & blist[i].status))
      continue;
    else if(overlaps(&blist[i], start, end))
      return FALSE;
  }

  /* Area [start, end] didn't overlap with any secure memory area, and
   * is fully non-secure area. */
  return TRUE;
}

/* @copydoc tzbsp_blist_config_s_area */
int tzbsp_blist_config_s_area(tzbsp_blacklist_area_t* blist, uint32 id,
                              uint32 start, uint32 end)
{
  uint32 i = 0;
  TZBSP_ALWAYS(NULL != blist);

  if(end < start)
    return -E_FAILURE;

  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if((id == blist[i].id) &&
       (TZBSP_BLACKLIST_ITEM_DYNAMIC & blist[i].status))
    {
      blist[i].start = start;
      blist[i].end = end;
      return E_SUCCESS;
    }
  }

  return -E_FAILURE;
}

/* @copydoc tzbsp_blist_enable_s_area */
int tzbsp_blist_enable_s_area(tzbsp_blacklist_area_t* blist, uint32 id)
{
  uint32 i = 0;
  TZBSP_ALWAYS(NULL != blist);

  /* Enabling a static area not considered an error; those are always enabled
   * anyway. */
  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if(id == blist[i].id)
    {
      blist[i].status |= TZBSP_BLACKLIST_ITEM_ENABLED;
      return E_SUCCESS;
    }
  }
  return -E_FAILURE;
}

/* @copydoc tzbsp_blist_disable_s_area */
int tzbsp_blist_disable_s_area(tzbsp_blacklist_area_t* blist, uint32 id)
{
  uint32 i = 0;
  TZBSP_ALWAYS(NULL != blist);

  /* Disabling a static area is considered and error; those should always be
   * enabled. */
  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if((id == blist[i].id) &&
       (TZBSP_BLACKLIST_ITEM_DYNAMIC & blist[i].status))
    {
      blist[i].status &= ~TZBSP_BLACKLIST_ITEM_ENABLED;
      return E_SUCCESS;
    }
  }
  return -E_FAILURE;
}

/* @copydoc tzbsp_heap_init */
void tzbsp_heap_init(void)
{
  DALSYS_InitMod(NULL);
}

/* @copydoc tzbsp_malloc */
void* tzbsp_malloc(uint32 size)
{
  void* mem;
  if(DAL_SUCCESS == DALSYS_Malloc(size, &mem))
    return mem;
  else
    return NULL;
}

/* @copydoc tzbsp_free */
void tzbsp_free(void* ptr)
{
  (void) DALSYS_Free(ptr); /* Return value ignored */
}

/* @copydoc tzbsp_blist_is_s_area_for_id
 *
 * Testing must be done against each individual secure memory
 * area. Testing just the end points is not enough as it is possible
 * to define a memory area that has end points in secure memory but
 * contains non-secure areas in between.
 */
boolean tzbsp_blist_is_s_area_for_id(const tzbsp_blacklist_area_t* blist, uint32 id,
                                     const void* start, const void* end)
{
  uint32 i = 0;
  TZBSP_ALWAYS(NULL != blist);

  if(end < start)
  {
    /* Either the specified addresses are incorrect, or there has been a
       wrap-around of the end address. Return False either way.
    */
    return FALSE;
  }

  for(i=0; TZBSP_BLACKLIST_ID_NONE != blist[i].id; i++)
  {
    if(id == blist[i].id)
    {
      if(!(TZBSP_BLACKLIST_ITEM_ENABLED & blist[i].status))
      {
        return FALSE;
      }
      else if(contains(&blist[i], start, end))
      {
        return TRUE;
      }
    }
  }

  /* Either [start, end] is fully non-secure area or it only partially
   * overlapped some secure area. */
  return FALSE;
}

boolean tzbsp_is_s_range_for_id(uint32 id, const void* start, uint32 len)
{
  const void* end;
  end = (const void*)((uint32)start + len);
  return  tzbsp_blist_is_s_area_for_id(g_tzbsp_blist, id, start, end);
}

/**
   Checks to see if the given memory range is completely within the
   range of DDR

   @param[in] start_ptr Start of the address range to check
   @param[in] len       Length of the address range to check

   @return \c TRUE iff the given memory region is contained in the area.
           \c FALSE otherwise.

*/
boolean tzbsp_ddr_contains_area(const void* start_ptr, uint32 len)
{
  uint32 i;
  const uint64 start     = (uint64) start_ptr;
  const uint64 end       = (uint64) start_ptr + len;

  for( i = 0; i < tzbsp_num_ddr_regions; i++ )
  {
    uint64 ddr_start = tzbsp_ddr_regions[i].start;
    uint64 ddr_end   = (ddr_start + tzbsp_ddr_regions[i].size);

    if( (ddr_start <= start && start <  ddr_end) &&
        (ddr_start <  end   && end   <= ddr_end) )
    {
      return TRUE;
    }
  }

  return FALSE;
}

/* @copydoc tzbsp_vtop_iovec */
int tzbsp_vtop_iovec(uint32 v_addr, uint32 length, const void *pIoVecPhys)
{
#if defined (TZBSP_WITH_MMU)
  return tzbsp_uio(v_addr, length, pIoVecPhys);
#else
  if(pIoVecPhys)
  {
    tzbsp_uio_t *uio   = (tzbsp_uio_t*)(pIoVecPhys);
    uio->iov->iov_base = (void*)(v_addr);
    uio->iov->iov_len  = length;
    return E_SUCCESS;
  }
  return -E_INVALID_ARG;
#endif
}

/* @copydoc tzbsp_coalesce_ddr_regions */
uint32 tzbsp_coalesce_ddr_regions( tzbsp_ddr_region_t *regions,
                                   uint32 num_regions )
{
  uint32 i, j;
  uint32 total_regions = 0;

  if( num_regions < 2 )
  {
    /* Nothing to do */
    return num_regions;
  }

  /* Compare each DDR region against the others, attempting to coalesce */
  for( j = 0; j < num_regions; /*incremented below*/ )
  {
    /* 64-bit cast to prevent overflow errors */
    uint64 cstart = (uint64)regions[j].start;
    uint64 csize  = (uint64)regions[j].size;

    for( i = (j + 1); i < num_regions; i++ )
    {
      uint64 istart = (uint64)regions[i].start;
      uint64 isize  = (uint64)regions[i].size;
      if( isize == 0 )
      {
        continue;
      }
      else if( csize == 0 )
      {
        /* Since the regions are modified in-place, the seed
           for the next coalesce needs to be done dynamically */
        regions[j].start = (uint32)istart;

      }
      if( ((istart + isize) >= cstart) && (istart <= (cstart + csize)) )
      {
        /* Iterator region is contained within, or adjacent to,
           current region; coalesce this region */
        regions[j].start = ( istart < cstart ) ? (uint32)istart : (uint32)cstart;
        regions[j].size += (uint32)isize;
        regions[i].start = 0;
        regions[i].size = 0;
        break;
      }
    }

    /* Since each merge operation could allow other previous
     * unmergeable regions to be coalesced, the outer loop is only
     * iterated once the entire array is scanned in a single pass
     */
    if( i == num_regions )
    {
      if( regions[j].size != 0 )
      {
        total_regions++;
      }
      j++;
    }
  }

  return total_regions;
}

/**
   @copydoc tzbsp_mem_map_device
 */
int tzbsp_mem_map_device(uint32 mmp)
{
#if defined(TZBSP_WITH_MMU)
  mem_block_type mmp_block;
  int retval = tzbsp_get_peripheral_mem_block((tzbsp_mmp_type)mmp, &mmp_block);

  if (retval == E_SUCCESS)
  {
    retval = tzbsp_mmu_map(mmp_block.p_base,
                           mmp_block.v_base,
                           mmp_block.size_in_kbytes,
                           (TZ_MEM_DEVICE | TZ_READ_WRITE_PL1 |
                            TZ_NON_EXECUTABLE));

    if (retval == E_SUCCESS)
    {
      tzbsp_dtlb_inval_all();
    }
  }

  return retval;
#else
  return E_SUCCESS;
#endif
}


/**
   @copydoc tzbsp_mem_unmap_device
 */
int tzbsp_mem_unmap_device(uint32 mmp)
{
#if defined(TZBSP_WITH_MMU)
  mem_block_type mmp_block;
  int retval = tzbsp_get_peripheral_mem_block((tzbsp_mmp_type)mmp, &mmp_block);

  if (retval == E_SUCCESS)
  {
    retval = tzbsp_mmu_map(mmp_block.p_base,
                           mmp_block.v_base,
                           mmp_block.size_in_kbytes,
                           TZ_TTE_INVALID);

    if (retval == E_SUCCESS)
    {
      tzbsp_dtlb_inval_all();
    }
  }

  return retval;
#else
  return E_SUCCESS;
#endif
}

/**
   @copydoc tzbsp_mem_unmap_all_devices
 */
int tzbsp_mem_unmap_all_devices(void)
{
#if defined(TZBSP_WITH_MMU)
  tzbsp_mmp_type mmp_ii;
  int retval;

  for (mmp_ii = (tzbsp_mmp_type) 0; mmp_ii != TZBSP_NUM_MMP; mmp_ii++)
  {
    retval = tzbsp_mem_unmap_device(mmp_ii);

    if (retval == E_SUCCESS)
    {
      tzbsp_dtlb_inval_all();
    }
  }

  return retval;
#else
  return E_SUCCESS;
#endif
}

#ifdef TZBSP_WITH_TEST_SVC   /* Cuurently not needed in TZ Image */

/**
   @copydoc tzbsp_mem_unmap_all_devices
 */
int tzbsp_mem_map_all_devices(void)
{
#if defined(TZBSP_WITH_MMU)
  tzbsp_mmp_type mmp_ii;
  int retval;

  for (mmp_ii = (tzbsp_mmp_type) 0; mmp_ii != TZBSP_NUM_MMP; mmp_ii++)
  {
    retval = tzbsp_mem_map_device(mmp_ii);

    if (retval == E_SUCCESS)
    {
      tzbsp_dtlb_inval_all();
    }
  }

  return retval;
#else
  return E_SUCCESS;
#endif
}
#endif /* TZBSP_WITH_TEST_SVC */

/**
   @copydoc tzbsp_ddr_init
 */
int tzbsp_ddr_init(void)
{
  int retval = E_SUCCESS;
  int ii;
  uint32 ddr_end = 0;
  uint32 ddr_start = 0;
  uint32 mmu_map_len = 0;

  /* Determine the size of DDR */
  retval = tzbsp_read_ddr_size(&tzbsp_num_ddr_regions,
                               tzbsp_ddr_regions,
                               TZBSP_ARRAY_SIZE(tzbsp_ddr_regions));
  TZBSP_RETCHK(retval);

  for (ii = 0; ii < tzbsp_num_ddr_regions; ii++)
  {

    if(ddr_end < (tzbsp_ddr_regions[ii].start + tzbsp_ddr_regions[ii].size))
    {
      ddr_end = tzbsp_ddr_regions[ii].start + tzbsp_ddr_regions[ii].size;
    }
    if((0 == ddr_start) || (tzbsp_ddr_regions[ii].start < ddr_start))
    {
      ddr_start = tzbsp_ddr_regions[ii].start;
    }
  }
  tzbsp_ddr_max_addr = ddr_end;
  /* Special case for 4 GB limit */
  if (tzbsp_ddr_max_addr == 0xFFFFFFFF)
  {
    mmu_map_len = ((TZBSP_4GB - TZBSP_DDR_START_ADDR) >> 10);
  }
  else
  {
    mmu_map_len = ((tzbsp_ddr_max_addr - TZBSP_DDR_START_ADDR)>> 10);
  }

  /* Make DDR region initial setting invalid */
  retval = tzbsp_mmu_add_default_region(TZBSP_DDR_START_ADDR,
                                        TZBSP_DDR_START_ADDR,
                                        mmu_map_len,
                                        TZ_TTE_INVALID);
  TZBSP_RETCHK(retval);

  /* Map each region of DDR dynamically */
  for (ii = 0; ii < tzbsp_num_ddr_regions; ii++)
  {
    if( 0xFFFFFFFF == (tzbsp_ddr_regions[ii].start + tzbsp_ddr_regions[ii].size ))
    {
      mmu_map_len = ((TZBSP_4GB - tzbsp_ddr_regions[ii].start) >> 10);
    }
    else
    {
      mmu_map_len = (tzbsp_ddr_regions[ii].size >> 10);
    }
    retval =
      tzbsp_mmu_add_default_region(tzbsp_ddr_regions[ii].start,
                                   tzbsp_ddr_regions[ii].start,
                                   mmu_map_len,
                                   (TZ_READ_WRITE_PL1 |
                                    TZ_MEM_NON_CACHEABLE_S |
                                    TZ_NON_EXECUTABLE));
    TZBSP_RETCHK(retval);
  }

  /* Reconfigure memory outside of DDR to be blacklisted (don't want it
   * marked as dynamic, so can't go through the APIs) */
  tzbsp_reconfig_black_list(ddr_start, ddr_end);

  return E_SUCCESS;
}

/**
 * Tests whether Shared Buffer from non-secure domain or not.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end Length of the memory range in bytes.
 *
 * @return \c TRUE if shared buffer from non-secure domain. \c
 * FALSE if shared buffer from secure domain.
 */
boolean tzbsp_validate_security_for_shared_buffer(const void* start, uint32 len)
{
  uint64 end;
  if((uint64)start > (1ULL<<36) - (uint64)len)
  {
    //Integer overflow
    return FALSE;
  }
  end = (uint64)start + (uint64)len;

  if(!tzbsp_is_ns_range(start, len))
  {
    /* SB is in secure memory.  The only secure memory we allow is CP memory.
       If SB is not in CP memory, then return security check fail. */
    if( !tzbsp_memprot_is_s_tag_area(TZBSP_MEM_TAG_USECASE_CP, (uint64)start, end) &&
        !tzbsp_memprot_is_s_tag_area(TZBSP_MEM_TAG_USECASE_AUDIO_CP, (uint64)start, end) &&
        !tzbsp_memprot_is_s_tag_area(TZBSP_MEM_TAG_USECASE_VIDEO_PIXEL_CP, (uint64)start, end))
    {
      return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
  else /* SB from non-secure memory always allowed */
  {
    return TRUE;
  }
}



