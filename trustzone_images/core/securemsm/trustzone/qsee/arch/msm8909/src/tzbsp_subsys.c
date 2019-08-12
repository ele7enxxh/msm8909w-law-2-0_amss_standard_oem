/*===========================================================================
   Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/msm8909/src/tzbsp_subsys.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/08/12   amen    Video PIL
07/15/11   tk      Initial version.
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <string.h>
#include <HALhwio.h>

#include "tzbsp.h"
#include "tzbsp_config.h"
#include "tzbsp_syscall.h"
#include "tz_syscall_pub.h"
#include "tzbsp_errno.h"
#include "tzbsp_chipset.h"
#include "tzbsp_mem.h"
#include "tzbsp_xpu.h"
#include "tzbsp_xpu_config.h"
#include "tzbsp_vmid_config.h"
#include "tzbsp_memory_map.h"
#include "tzbsp_mmu_cache.h"
#include "tzbsp_hwio.h"
#include "tzbsp_secboot.h"
#include "tzbsp_amt.h"
#include "tzbsp_shared_imem.h"
#include "crypto/tzbsp_hash.h"
#include "sha_shared.h"


/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/* Area in DDR which subsystem images can be loaded */
#define TZBSP_PIL_LOAD_AREA_BASE    0x80000000
#define TZBSP_PIL_LOAD_AREA_END     0xA0000000

#define TZBSP_DYNAMIC_BLIST_ITEM    0xFFFF

#define TZBSP_SUBSYS_INFO_ADDRESS   (TZBSP_CODE_BASE + 0x0014C000)

/**
 * Structure for subsystem memory map areas.
 */
typedef struct tzbsp_image_area_s
{
  uint16 blist; /**< Blacklist ID for this area. */
  uint16 erase; /**< \c TRUE if the area should be erased when unlocked. */
  uint32 start; /**< Start address of the memory area (inclusive of range). */
  uint32 end;   /**< End address of the memory area (exclusive of range). */
  uint32 rvmid; /**< Read access permitted VMIDs */
  uint32 wvmid; /**< Write access permitted VMIDs */
} tzbsp_image_area_t;

static tzbsp_image_area_t g_tzbsp_wlan_area[] =
{
  {TZBSP_BLIST_WLAN, TRUE, 0, 0,
   TZBSP_VMID_TZ_BIT | TZBSP_VMID_WLAN_BIT,
   TZBSP_VMID_TZ_BIT | TZBSP_VMID_WLAN_BIT},
};

static tzbsp_image_area_t g_tzbsp_sec_app_area[] =
{
  {TZBSP_DYNAMIC_BLIST_ITEM, FALSE, 0, 0,
   TZBSP_VMID_TZ_BIT, TZBSP_VMID_TZ_BIT},
};

static tzbsp_image_area_t g_tzbsp_video_area[] =
{
  {TZBSP_BLIST_VIDEO, TRUE, 0, 0, 
   TZBSP_VMID_TZ_BIT | TZBSP_VMID_VIDEO_BIT,
   TZBSP_VMID_TZ_BIT | TZBSP_VMID_VIDEO_BIT}
};

typedef struct subsys_region_s
{
  uint32 start;
  uint32 size;
}subsys_region_t;

typedef struct tzbsp_subsys_info_s
{
  subsys_region_t subsys[TZ_PIL_AUTH_UNSUPPORTED_PROC];
  uint8 hash[TZBSP_SHA256_HASH_SZ];
}tzbsp_subsys_info_t;

tzbsp_subsys_info_t *g_subsys_info;

/*----------------------------------------------------------------------------
 * External Function Declarations
 * -------------------------------------------------------------------------*/

int tzbsp_mss_bring_up(uint32 start_addr);
int tzbsp_wlan_bring_up(uint32 start_addr);
int tzbsp_video_bring_up(uint32 start_addr);

int tzbsp_mss_tear_down(tzbsp_tear_down_scope_et scope);
int tzbsp_wlan_tear_down(tzbsp_tear_down_scope_et scope);
int tzbsp_video_tear_down(tzbsp_tear_down_scope_et scope);
void tzbsp_memzi(uint32* addr, uint32 size);

void tzbsp_reinit_subsystem_info(tz_pil_proc_e_type proc, uint32 start, uint32 size);

extern void tzbsp_enable_ce1_clocks(boolean enable);
uint8 sha_digest[TZBSP_SHA256_HASH_SZ];
/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/**
 * Returns the memory map area array for a subsystem.
 *
 * @param[in] proc    Subsystem identifier.
 * @param[out] count  Pointer to store the array size.
 *
 * @return A valid pointer to memory map area array, \c NULL otherwise.
 */
static tzbsp_image_area_t* tzbsp_get_mmap_areas(
    tz_pil_proc_e_type proc, uint32* count)
{
  switch (proc)
  {
  case TZ_PIL_AUTH_WLAN_PROC:
    *count = TZBSP_ARRAY_SIZE(g_tzbsp_wlan_area);
    return g_tzbsp_wlan_area;
  case TZ_PIL_AUTH_SEC_APP:
    *count = TZBSP_ARRAY_SIZE(g_tzbsp_sec_app_area);
    return g_tzbsp_sec_app_area;
  case TZ_PIL_AUTH_VIDEO_PROC:
    *count = TZBSP_ARRAY_SIZE(g_tzbsp_video_area);
    return g_tzbsp_video_area;
  default:
    TZBSP_ERROR_CODE(GET_MMAP_AREAS_INVALID_SUBSYS);
    *count = 0;
    return NULL;
  }
}

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

int tzbsp_subsys_image_init_ok(tz_pil_proc_e_type proc,
                               const tzbsp_elf_info_t* elf)
{
  int err = E_SUCCESS;

  switch(proc)
  {
    case TZ_PIL_AUTH_VIDEO_PROC:
      /* Parse the elf header struct to the video bringup code. */
      err = tzbsp_video_config(elf);
      break;

    default:
      break;
  }

  return err;
}

/* @copydoc tzbsp_subsys_bring_up */
int tzbsp_subsys_bring_up(tz_pil_proc_e_type proc, uint32 start_addr)
{
  int err = E_SUCCESS;

  /* Updating subsystem info */
  if (proc != TZ_PIL_AUTH_SEC_APP) 
  {
    uint32 start, end;

    /* Get the image address range, according to the ELF provided */
    tzbsp_subsys_get_memory_range(proc, &start, &end);
    tzbsp_reinit_subsystem_info(proc, start, TZBSP_ROUNDUP(uint32, (end - start), TZBSP_PAGE_4K));
  }

  switch (proc)
  {
    case TZ_PIL_AUTH_WLAN_PROC:
      err = tzbsp_wlan_bring_up(start_addr);
      break;
    case TZ_PIL_AUTH_SEC_APP:
      /* Do nothing */
      break;
    case TZ_PIL_AUTH_VIDEO_PROC:
      err = tzbsp_video_bring_up(start_addr);
      break;
    default:
      TZBSP_ERROR_CODE(SUBSYS_BRING_UP_INVALID_SUBSYS);
      err = -E_NOT_AVAILABLE;
  }

  TZBSP_ERROR_CODE_E(err, SUBSYS_BRING_UP_FAILED);
  return err;
}

/* @copydoc tzbsp_subsys_tear_down */
int tzbsp_subsys_tear_down(tz_pil_proc_e_type proc,
                           tzbsp_tear_down_scope_et scope)
{
  int err = E_SUCCESS;

  /* Tearing down CPU is not needed for 8960, but the API is chipset
     independent, so just return */
  if (scope == TZBSP_TEAR_DOWN_CPU)
  {
    return err;
  }

  /* Updating subsystem info */
  if (proc != TZ_PIL_AUTH_SEC_APP) 
  {
    tzbsp_reinit_subsystem_info(proc, 0, 0);
  }

  switch (proc)
  {
    case TZ_PIL_AUTH_WLAN_PROC:
      err = tzbsp_wlan_tear_down(scope);
      break;
    case TZ_PIL_AUTH_SEC_APP: /* Nothing is done for SEC Apps */
      break;
    case TZ_PIL_AUTH_VIDEO_PROC:
      err = tzbsp_video_tear_down(scope);
      break;
    default:
      TZBSP_ERROR_CODE(SUBSYS_TEAR_DOWN_INVALID_SUBSYS);
      err = -E_NOT_AVAILABLE;
  }

  TZBSP_ERROR_CODE_E(err, SUBSYS_TEAR_DOWN_FAILED);
  return err;
}

/** @copydoc tzbsp_subsys_is_supported */
boolean tzbsp_subsys_is_supported(uint32 proc)
{
  boolean ret = FALSE;

  switch (proc)
  {
    case TZ_PIL_AUTH_VIDEO_PROC:
    case TZ_PIL_AUTH_WLAN_PROC:
    case TZ_PIL_AUTH_SEC_APP:
      ret = TRUE;
      break;
    default:
      TZBSP_ERROR_CODE(SUBSYS_IS_SUPPORTED_INVALID_SUBSYS);
      break;
  }

  return ret;
}

/** @copydoc tzbsp_subsys_is_mandated */
boolean tzbsp_subsys_is_mandated(uint32 proc)
{
  boolean ret = FALSE;

  switch (proc)
  {
    case TZ_PIL_AUTH_WLAN_PROC:
    case TZ_PIL_AUTH_VIDEO_PROC:
      ret = TRUE;
      break;
    default:
      TZBSP_ERROR_CODE(SUBSYS_IS_MANDATED_INVALID_SUBSYS);
      break;
  }

  return ret;
}

/**
 * Returns the SW type hash table index used by the secboot
 * authentication routine
 *
 * @param[in]   proc    Subsystem identifier.
 * @param[out]  sw_type Populated with the SW type hash table
 *                      index on sucess.  Memory is alocated by
 *                      caller
 *
 * @return  E_SUCESS on sucess, negative error otherwise
 */
int tzbsp_subsys_sw_type(tz_pil_proc_e_type proc, uint32 *sw_type)
{
  if(sw_type == NULL)
  {
    return -E_INVALID_ARG;
  }

  switch (proc)
  {
  case TZ_PIL_AUTH_WLAN_PROC:
    *sw_type = SECBOOT_WCNSS_SW_TYPE;
    break;
  case TZ_PIL_AUTH_SEC_APP:
    *sw_type = SECBOOT_TZ_EXEC_SW_TYPE;
    break;
  case TZ_PIL_AUTH_VIDEO_PROC:
    *sw_type = SECBOOT_VIDEO_SW_TYPE;
    break;
  default:
    TZBSP_ERROR_CODE(SUBSYS_INVALID_PROC_SW_TYPE);
    *sw_type = (uint32)SECBOOT_INVALID_SW_TYPE;
    return -E_NOT_SUPPORTED;
  }

  return E_SUCCESS;
}

/**
 * Checks if a memory range is valid for a subsystem by comparing it to memory
 * map and current blacklist configuration.
 *
 * @param[in] proc    Subsystem identifier.
 * @param[in] start   Start of the memory area (inclusive).
 * @param[in] end     End of the memory area (exclusive).
 *
 * @param \c TRUE if the memory range is valid, \c FALSE otherwise.
 */
boolean tzbsp_subsys_is_valid_area(tz_pil_proc_e_type proc, uint32 start,
                                   uint32 end)
{
  int i;
  uint32 count;
  const tzbsp_image_area_t* area;

  /* Just ignore empty segments. */
  if(0 == start && 0 == end)
  {
    return TRUE;
  }

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || 0 == count)
  {
    TZBSP_ERROR_CODE(SUBSYS_IS_VALID_AREA_INVALID_SUBSYS);
    return FALSE;
  }

   if(end == 0)
  {
   TZBSP_ERROR_CODE(SUBSYS_IS_VALID_AREA_INVALID_SEGMENT);
   return FALSE;
  }
  
  for(i=0; i < count; i++)
  {
    if (area[i].start <= start && end <= area[i].end)
    {
      /* A valid memory area must not overlap any existing blacklisted
       * areas and must fall under a valid subsystem memory map
       * range. The blacklist checks include the end point (hence the
       * -1).
       */

      /* tzbsp_is_ns_area will throw an error if end < start, so check for that */
      if ((void*)start == (void*)end)
      {
        return TRUE;
      } 

      if (tzbsp_is_ns_area((void*)start, (void*)(end-1)))
      {
        return TRUE;
      }
      /* Secure application regions are locked using dynamic resource
       * groups prior to copying the image over.
       */
      else if ((TZ_PIL_AUTH_SEC_APP == proc) &&
               (area[i].blist == TZBSP_DYNAMIC_BLIST_ITEM))
      {
        return TRUE;
      }
    }
  }

  TZBSP_ERROR_CODE(SUBSYS_IS_VALID_AREA_INVALID_SEGMENT);
  return FALSE;
}

/**
 * XPU protects a subsystem.
 *
 * @param[in] proc    Subsystem identifier.
 *
 * @return \c E_SUCCESS on success, negative error code otherwise.
 */
int tzbsp_subsys_lock(tz_pil_proc_e_type proc)
{
  int i;
  uint32 count;
  const tzbsp_image_area_t* area;

  /* Secure apps are loaded into secure memory before being locked
     down. */
  if (TZ_PIL_AUTH_SEC_APP == proc)
  {
    return E_SUCCESS;
  }

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || count == 0)
  {
    TZBSP_ERROR_CODE(SUBSYS_LOCK_INVALID_SUBSYS);
    return -E_FAILURE;
  }

  for(i=0; i < count; i++)
  {
    /* The image area must not overlap any existing blacklisted
    * areas. The area checks include the end point (hence the -1). */
    if(!tzbsp_is_ns_area((void*)area[i].start, (void*)(area[i].end-1)))
    {
      TZBSP_ERROR_CODE(SUBSYS_LOCK_INVALID_BLIST);
      return -E_NOT_ALLOWED;
    }

    if(tzbsp_xpu_lock_area(area[i].blist, area[i].start, area[i].end,
                           area[i].rvmid, area[i].wvmid) < 0)
    {
      TZBSP_ERROR_CODE(SUBSYS_LOCK_XPU_LOCK_AREA_FAILED);
      return -E_FAILURE;
    }

    if(tzbsp_config_s_area(area[i].blist, area[i].start, area[i].end) < 0)
    {
      TZBSP_ERROR_CODE(SUBSYS_LOCK_CONFIG_S_AREA_FAILED);
      return -E_FAILURE;
    }

    if(tzbsp_enable_s_area(area[i].blist) < 0)
    {
      TZBSP_ERROR_CODE(SUBSYS_LOCK_ENABLE_S_AREA_FAILED);
      return -E_FAILURE;
    }
  }

  return E_SUCCESS;
}

/**
 * Removes XPU protetion from a subsystem.
 *
 * @param[in] proc    Subsystem identifier.
 * @param[in] erase   Erases the subsystem memory areas.
 *
 * @return \c E_SUCCESS on success, negative error code otherwise.
 */
int tzbsp_subsys_unlock(tz_pil_proc_e_type proc, boolean erase)
{
  int i;
  uint32 count;
  const tzbsp_image_area_t* area;

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || 0 == count)
  {
    TZBSP_ERROR_CODE(SUBSYS_UNLOCK_INVALID_SUBSYS);
    return -E_FAILURE;
  }

  for(i=0; i < count; i++)
  {
    /* There could be sensitive contents (e.g. keys) in mem */
    if(area[i].erase && erase)
    {
      memset((void*)area[i].start, 0, area[i].end - area[i].start);
      tzbsp_dcache_flush_region((void*)area[i].start,
                                area[i].end - area[i].start);
    }

    if(tzbsp_xpu_unlock_area(area[i].blist) < 0)
    {
      TZBSP_ERROR_CODE(SUBSYS_UNLOCK_XPU_UNLOCK_FAILED);
      return -E_FAILURE;
    }

    if(tzbsp_disable_s_area(area[i].blist) < 0)
    {
      TZBSP_ERROR_CODE(SUBSYS_UNLOCK_DISABLE_S_AREA_FAILED);
      return -E_FAILURE;
    }
  }

  return E_SUCCESS;
}

/* @copydoc tzbsp_subsys_is_memory_range_set */
boolean tzbsp_subsys_is_memory_range_set(tz_pil_proc_e_type proc)
{
  uint32 start, end;
  return (E_SUCCESS == tzbsp_subsys_get_memory_range(proc, &start, &end));
}

/* @copydoc tzbsp_subsys_set_memory_range */
int tzbsp_subsys_set_memory_range(tz_pil_proc_e_type proc, uint32 start,
                                   uint32 end)
{
  uint32 i;
  uint32 count;
  tzbsp_image_area_t* area;
  const tzbsp_image_area_t* compare_area;
  int32 err = E_SUCCESS;

  /* Memory range must be contained within the allowable subsys load region */
  if(TZBSP_PIL_LOAD_AREA_END < end
#if (TZBSP_PIL_LOAD_AREA_BASE != 0) /* Suppress compiler warning */
     || start < TZBSP_PIL_LOAD_AREA_BASE
#endif
    )
  {
    TZBSP_ERROR_CODE(SUBSYS_SET_MEM_INVALID_IMAGE_AREA);
    return -E_INVALID_ARG;
  }

  /* For XPU protection, start and end must be aligned to page sizes
   * TODO: Following up with HLOS team to determine best way to relay
   *       the aligned address back to the caller */
  if(start >= end ||
     TZBSP_ROUNDDOWN(uint32, start, TZBSP_PAGE_4K) != start ||
     TZBSP_ROUNDUP(uint32, end, TZBSP_PAGE_4K) != end)
  {
    TZBSP_ERROR_CODE(SUBSYS_SET_MEM_INVALID_ADDR);
    return -E_INVALID_ARG;
  }

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || 0 == count)
  {
    TZBSP_ERROR_CODE(SUBSYS_SET_MEM_INVALID_SUBSYS);
    return -E_NOT_SUPPORTED;
  }

  /* TODO: Images should only contain one area, in DDR, but LPASS requires
   *       a change to the image. */
  for(i = 0; i < TZ_PIL_AUTH_UNSUPPORTED_PROC; i++)
  {
    /* Get the next area to check */
    compare_area = tzbsp_get_mmap_areas((tz_pil_proc_e_type)i, &count);

    /* Not all subsytems are necessarily supported */
    if(!compare_area || area == compare_area || 0 == count)
    {
      continue;
    }

    /* Check for overlap */
    if((compare_area->start <= start && start < compare_area->end) ||
       (compare_area->start < end && end <= compare_area->end) ||
       (start < compare_area->start && compare_area->end <= end))
    {
      err = -E_ADDR_IN_USE;
      TZBSP_ERROR_CODE(SUBSYS_SET_MEM_ADDR_IN_USE);
      break;
    }
  }

  /* Update the start and end address for this area if no overlap exists */
  if(err == E_SUCCESS)
  {
    area->start = start;
    area->end = end;
  }

  return err;
}

/* @copydoc tzbsp_subsys_get_memory_range */
int tzbsp_subsys_get_memory_range(tz_pil_proc_e_type proc, uint32 *start,
                                  uint32 *end)
{
  uint32 count;
  const tzbsp_image_area_t* area;

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || 0 == count)
  {
    return -E_NOT_SUPPORTED;
  }

  if(0 == area->start && 0 == area->end)
  {
    return -E_NOT_AVAILABLE;
  }

  *start = area->start;
  *end = area->end;

  return E_SUCCESS;
}

/* @copydoc tzbsp_subsys_get_reloc_pil_range */
int tzbsp_subsys_get_reloc_pil_range(uint32 *start, uint32 *end)
{
  *start  = TZBSP_PIL_LOAD_AREA_BASE;
  *end    = TZBSP_PIL_LOAD_AREA_END;

  return E_SUCCESS;
}

/* @copydoc tzbsp_subsys_info_init */
void tzbsp_subsys_info_init(void)
{
  g_subsys_info = (tzbsp_subsys_info_t *)TZBSP_SUBSYS_INFO_ADDRESS;
  memset(g_subsys_info, 0, sizeof(tzbsp_subsys_info_t));
}

int tzbsp_sw_hash(SW_Auth_Alg_Type pAlgo,
                  const uint8 *msg,
                  uint32 msg_len,
                  uint8 *digest,
                  uint32 digest_len)
{
  sw_crypto_errno_enum_type status;   
  tzbsp_hash_ctx *cntx = NULL;
  IovecListType  ioVecListIn;
  IovecListType  ioVecListOut;
  IovecType      IovecIn;
  IovecType      IovecOut;

  memset((void*)digest, 0x0, digest_len);
  
  /* Input IOVEC */
  ioVecListIn.size = 1;
  ioVecListIn.iov = &IovecIn;
  ioVecListIn.iov[0].dwLen = msg_len; 
  ioVecListIn.iov[0].pvBase = (void *)msg;
 
  /* Output IOVEC */
  ioVecListOut.size = 1;
  ioVecListOut.iov = &IovecOut;
  ioVecListOut.iov[0].dwLen = digest_len; 
  ioVecListOut.iov[0].pvBase = digest;    

  status = SW_Hash_Init(&cntx, pAlgo);
  if( UC_E_SUCCESS != status )
  {
    return -E_FAILURE;
  }

  status = SW_Hash_Update (cntx, ioVecListIn); 
  if (status != UC_E_SUCCESS)
  {
    return -E_FAILURE;
  }
 
  status = SW_Hash_Final (cntx, &ioVecListOut);
  if (status != UC_E_SUCCESS)
  {
    return -E_FAILURE;
  }
  
  return E_SUCCESS;
}				  

/* @copydoc tzbsp_reinit_subsystem_info */
void tzbsp_reinit_subsystem_info(tz_pil_proc_e_type proc, uint32 start, uint32 size)
{
  int ret;

  if ((!g_subsys_info) || (TZ_PIL_AUTH_UNSUPPORTED_PROC <= proc))
  {
    TZBSP_ERROR_CODE(SUBSYS_INFO_INV_PARAMS);
    return;
  }

  g_subsys_info->subsys[proc].start = start;
  g_subsys_info->subsys[proc].size = size;
  if (size) 
  {
    tzbsp_enable_ce1_clocks(TRUE);
    ret = tzbsp_sw_hash(SW_AUTH_ALG_SHA256,
                     (uint8*)g_subsys_info->subsys,
                     (sizeof(subsys_region_t) * TZ_PIL_AUTH_UNSUPPORTED_PROC),
                     g_subsys_info->hash,
                     sizeof(g_subsys_info->hash));
    if (0 > ret)
    {
      TZBSP_ERROR_CODE(SUBSYS_INFO_HASH_FAILED);
    }
    tzbsp_enable_ce1_clocks(FALSE);
  }
}


/**
 * Write the NMI register of each Q6 based subsystem. These writes are done
 * in WDT bark context. These writes are not guaranteed to go through in all
 * WDT bark scenarios; bus hangs are possible.
 */
void tzbsp_write_q6_nmis(void)
{
  /* These are write only registers, cannot use OUTF. */
  HWIO_OUT(MSS_QDSP6SS_NMI, 1);
}

/*----------------------------------------------------------------------------
 * Subsystem Load Time Logger --- Debug Feature
 * -------------------------------------------------------------------------*/
#ifdef TZBSP_WITH_PIL_TIMING

#include "tzbsp_timer.h"
#include "tzbsp_log.h"
#warning Building with PIL timing

typedef struct tzbsp_pil_timing_s
{
  uint64 lpass_start;
  uint64 riva_start;
  uint64 video_start;
} tzbsp_pil_timing_t;

static tzbsp_pil_timing_t g_tzbsp_timings[2];

/* @copydoc tzbsp_pil_timing_start */
void tzbsp_pil_timing_start(uint32 command, uint32 subsys)
{
  int i;
  uint64 start;

  if(TZ_PIL_INIT_ID == command)
  {
    i = 0;
  }
  else if(TZ_PIL_AUTH_RESET_ID == command)
  {
    i = 1;
  }
  else
  {
    TZBSP_LOG_ERR("No such command: %u", command);
    return;
  }

  start = tzbsp_get_uptime_count_raw();

  switch(subsys)
  {
    case TZ_PIL_AUTH_WLAN_PROC:
      g_tzbsp_timings[i].riva_start = start;
      break;
    case TZ_PIL_AUTH_VIDEO_PROC:
      g_tzbsp_timings[i].video_start = start;
      break;
    default:
      break;
  }
}

/* @copydoc tzbsp_pil_timing_end */
void tzbsp_pil_timing_end(uint32 command, uint32 subsys)
{
  int i;
  uint64 end;

  if(TZ_PIL_INIT_ID == command)
  {
    i = 0;
  }
  else if(TZ_PIL_AUTH_RESET_ID == command)
  {
    i = 1;
  }
  else
  {
    TZBSP_LOG_ERR("No such command: %u", command);
    return;
  }

  end = tzbsp_get_uptime_count_raw();

  switch(subsys)
  {
    case TZ_PIL_AUTH_WLAN_PROC:
      end -= g_tzbsp_timings[i].riva_start;
      break;
    case TZ_PIL_AUTH_VIDEO_PROC:
      end -= g_tzbsp_timings[i].video_start;
      break;
    default:
      end = 0;
      break;
  }

  tzbsp_log(TZBSP_MSG_DEBUG, "PIL, i=%d, subsys=%u, time=%u", i, subsys,
            (uint32) end);
}

#endif /* TZBSP_WITH_PIL_TIMING */

/*----------------------------------------------------------------------------
 * PIL Unittest Debug Features
 * -------------------------------------------------------------------------*/
#ifdef TZBSP_WITH_TEST_SVC
int tzbsp_subsys_reset_image_mem_area(tz_pil_proc_e_type proc)
{
  uint32 count;
  tzbsp_image_area_t* area;

  area = tzbsp_get_mmap_areas(proc, &count);

  if(!area || 0 == count)
  {
    return -E_NOT_SUPPORTED;
  }

  area->start = 0;
  area->end = 0;

  return E_SUCCESS;
}
#endif /* TZBSP_WITH_TEST_SVC */
