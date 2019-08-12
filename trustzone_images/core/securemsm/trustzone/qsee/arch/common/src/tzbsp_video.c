
/**
@file tzbsp_video.c
@brief Trustzone VIDEO Subsystem related functions.

*/
/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/src/tzbsp_video.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
05/21/12   sg        Add support for restoring security config
03/08/12   amen      Initial version
=============================================================================*/

/*--------------------------------------------------------------------------
  Responsible for video firmware segment address and size passing to the
  SMMU, which sets up the video subsystem
--------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <HALhwio.h>
#include <stdio.h>
#include <string.h>
#include <busywait.h>

#include "tzbsp.h"
#include "tzbsp_video.h"
#include "tzbsp_config.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_log.h"
#include "tzbsp_chipset.h"
#include "tzbsp_pil.h"
#include "tz_syscall_pub.h"
#include "tzbsp_smmu.h"
#include "tzbsp_hwio.h"
#include "tzbsp_mem.h"
#include "tzbsp_vmidmt.h"
#include "tzbsp_xpu.h"
#include "SMMUClock.h"

/*----------------------------------------------------------------------------
 * Macros, Defines, and type definitions
 *--------------------------------------------------------------------------*/

#define ROUND_DOWN_4KB(size)  (size & ~(0xFFF))
#define ROUND_UP_4KB(size)    (size + (0xFFF) & ~(0xFFF))

/*----------------------------------------------------------------------------
 * Variable definitions
 *--------------------------------------------------------------------------*/

/* Structure pointing to elf, prog headers... */
static const tzbsp_elf_info_t* s_elf_info = NULL;

static boolean tzbsp_video_va_ranges_set = FALSE;
static boolean tzbsp_video_fw_loaded = FALSE;

/* Structure to store the VA ranges */
typedef struct {
  uint32 start; /* Inclusive */
  uint32 end;   /* Exclusive */
} tzbsp_video_va_range_t;

static tzbsp_video_va_range_t tzbsp_video_vars[3];
int tzbsp_video_init_and_start(void);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Helper function to program the VARs from the stored values
 */
static void tzbsp_video_program_vars(void)
{
  /* Determine if the Venus HW supports non-pixel VAR */
  boolean tzbsp_video_has_nonpxl = tzbsp_video_get_hw_ver() > TZBSP_VIDEO_HW_VER_1_2_LT;

  /* Program the VARs */
  HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_FW_START_ADDR, tzbsp_video_vars[0].start);
  HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_FW_END_ADDR, tzbsp_video_vars[0].end);
  HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_CPA_START_ADDR, tzbsp_video_vars[1].start);
  HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_CPA_END_ADDR, tzbsp_video_vars[1].end);
  if(tzbsp_video_has_nonpxl)
  {
    HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_NONPIX_START_ADDR, tzbsp_video_vars[2].start);
    HWIO_OUT(VENUS0_VENUS_WRAPPER_VBIF_SS_SEC_NONPIX_END_ADDR, tzbsp_video_vars[2].end);
  }
}


/* Helper function to enable the internal Venus clocks */
static void tzbsp_video_enable_clks(void)
{
  /* Unhalt the Venus sub-system clocks */
  HWIO_OUT(VENUS0_VENUS_WRAPPER_CLOCK_CONFIG, 0x0);
  HWIO_OUT(VENUS0_VENUS_WRAPPER_CPU_CLOCK_CONFIG, 0x0);
  HWIO_OUT(VENUS0_VENUS_WRAPPER_CPU_CGC_DIS, 0x0);

  /* Wait at least 10 cycles of vcodec_clk; according to the clock plan, the
   * frequency is in the > 100 MHz range, so 1 us is plenty amount of wait. 
   */
  busywait(1);  
}

/* Helper function to halt the Venus core */
inline static void tzbsp_video_halt_core(void)
{
  HWIO_OUTF(VENUS0_VENUS_WRAPPER_SW_RESET, A9SS_SW_RESET, 0x1);
}

/**
 * Function to configure the Venus XPUs, handling any internal clocks
 *
 * @return \c E_SUCCESS on success, negative error code otherwise
 */
int tzbsp_video_configure_xpus(void)
{
  tzbsp_video_enable_clks();
  return tzbsp_xpu_restore_config(TZ_DEVICE_VIDEO);
}
/**
 * Clears the virtual address ranges and puts Venus into reset
 *
 * @param [in] scope   Not required, function made such to keep
 *                     with existing function calls in subsys.c
 *
 * @return \c E_SUCCESS if successful, negative return code on failure
 */
int tzbsp_video_tear_down(tzbsp_tear_down_scope_et scope)
{
  /* Reset the VARs */
  tzbsp_video_fw_loaded = FALSE;
  tzbsp_video_va_ranges_set = FALSE;

  /* Place Venus into reset */
  tzbsp_video_halt_core();

  /* Disable venus secure CB 19,20,21,22 and invalidate TLB*/
  SMMU_ClockEnable(SMMU_CLK_APPS_TCU);
  HWIO_OUT(APPS_SMMU_CB19_SCTLR_APPS, 0x0);
  HWIO_OUT(APPS_SMMU_CB20_SCTLR_APPS, 0x0);
  HWIO_OUT(APPS_SMMU_CB21_SCTLR_APPS, 0x0);
  HWIO_OUT(APPS_SMMU_CB22_SCTLR_APPS, 0x0);
  SMMU_ClockDisable(SMMU_CLK_APPS_TCU);

  tzbsp_smmu_invalidate_tlb(TZ_DEVICE_APPS, TZBSP_SMMU_VENUS_BITSTREAM_CB);
  tzbsp_smmu_invalidate_tlb(TZ_DEVICE_APPS, TZBSP_SMMU_VENUS_FIRMWARE_CB);
  tzbsp_smmu_invalidate_tlb(TZ_DEVICE_APPS, TZBSP_SMMU_VENUS_PIXEL_CB);
  tzbsp_smmu_invalidate_tlb(TZ_DEVICE_APPS, TZBSP_SMMU_VENUS_NON_PIXEL_CB);

  /* Forget about the ELF */
  s_elf_info = NULL;

  /* Clear the VARs; these will be reset on next load */
  memset(tzbsp_video_vars, 0, sizeof(tzbsp_video_vars));

  return E_SUCCESS;
}

/**
 * Configures the Venus subsystem (SMMU and VARs) and releases it from reset.
 *
 * @return \c E_SUCCESS if successful, negative return code on failure
 */
static int tzbsp_video_init_and_start(void)
{
  int err = E_SUCCESS;

  TZBSP_LOG(TZBSP_MSG_DEBUG, "Venus:%s",__func__);
  do
  {
  if(!tzbsp_video_fw_loaded)
  {
    /* Venus FW hasn't be loaded, can't release from reset */
    TZBSP_ERROR_CODE(VIDEO_FW_NOT_LOADED);
      err = -E_NOT_ALLOWED;
      break;
  }
  
    /* Ensure the internal clocks are enabled before touching registers */
    tzbsp_video_enable_clks();

  /* There is a chance that the subsystem is already configured
   * and running.  Can't store this in a variable, since Venus
   * supports power collapse
   */
  if(0 == HWIO_INF(VENUS0_VENUS_WRAPPER_SW_RESET, A9SS_SW_RESET))
  {
      TZBSP_ERROR_CODE(VIDEO_ALREADY_OUT_OF_RESET);
      err = -E_ALREADY_DONE;
      break;
  }

    err = tzbsp_xpu_restore_config(TZ_DEVICE_VIDEO);
    if(err)
    {
      TZBSP_ERROR_CODE(VIDEO_XPU_CONFIG_FAILED);
      break;
    }

    err = tzbsp_smmu_init(TZ_DEVICE_VIDEO);
    if(err)
    {
       TZBSP_ERROR_CODE(VIDEO_SMMU_INIT_FAILED);
       break;
    }

    /* Invalidate the firmware CB TLB */
    err = tzbsp_smmu_invalidate_tlb(TZ_DEVICE_APPS, TZBSP_SMMU_VENUS_FIRMWARE_CB);
    if(err)
    {
      TZBSP_ERROR_CODE(VIDEO_FW_CB_TLB_INVALIDATE_FAILED);
      break;
    }
  } while(0);

  /* Proceed with releasing from reset if all the configurations were successful */
  if(!err)
  {
    tzbsp_video_program_vars();

    /* Configuration done, release Venus from reset */
    HWIO_OUTF(VENUS0_VENUS_WRAPPER_SW_RESET, A9SS_SW_RESET, 0x0);
  }

  return err;
}


/**
 * Sets up the address to point to the segments(text, data, page
 * table) and passes the address and size to the SMMU
 *
 * @param [in] addr   Not required, function made such to keep
 *                     with existing function calls in subsys.c
 *
 * @return \c E_SUCCESS if the SMMU calls were successfully
 * done, error code otherwise.
 */
int tzbsp_video_bring_up(uint32 addr)
{
  Elf32_Phdr *phdr = NULL;
  uint32 i=0, fw_start =0, fw_end =0;
  int err=-E_BAD_ADDRESS;


  TZBSP_LOG(TZBSP_MSG_DEBUG, "Venus:%s",__func__ );
  if(s_elf_info == NULL)
  {
    return -E_NOT_ALLOWED;
  }

  phdr = (Elf32_Phdr*)(s_elf_info->prog_hdr);

  /* Firmware bounds are determined during the load */
  fw_start  = (uint32)(-1);
  fw_end    = 0;

  do
  {
    err = tzbsp_smmu_clear_pt(TZBSP_SMMU_SS_PT_VENUS_FW);
    if(err)
    {
      TZBSP_ERROR_CODE(VIDEO_SMMU_PT_CLEAR_FAILED);
      break;
    }

    /* Go through each program header, mapping as necessary */
    for(i = 0; i < s_elf_info->prog_hdr_num; i++)
    {
      if(tzbsp_is_valid_segment(phdr) && phdr->p_memsz != 0)
      {
        /* Update the firmware execution ranges */
        if(fw_start > phdr->p_vaddr)
        {
          fw_start = phdr->p_vaddr;
        }
        if(fw_end < ROUND_UP_4KB(phdr->p_vaddr + phdr->p_memsz))
        {
          fw_end = ROUND_UP_4KB(phdr->p_vaddr + phdr->p_memsz);
        }

        /* Attempt to map this program section */
        err = tzbsp_smmu_map(TZBSP_SMMU_SS_PT_VENUS_FW, phdr->p_vaddr, phdr->p_paddr,
                             ROUND_UP_4KB(phdr->p_memsz), phdr->p_flags,
                             REGION_INNER_OUTER_NON_CACHE, FALSE);
        if(err)
        {
          TZBSP_ERROR_CODE(VIDEO_SMMU_MAP_SEGMENT_FAILED);
          break;
        }
      }
      phdr++;
    }
    /* Don't continue if the SMMU mapping loop failed */
    if(err) break;

    if(fw_start != 0 || fw_end < fw_start)
    {
      err = -E_BAD_ADDRESS;
      TZBSP_ERROR_CODE(VIDEO_INVALID_FW_ADDR_RANGE);
      break;
    }
  
    /* Code loaded and pagetables set; mark FW loaded */
    tzbsp_video_vars[0].start = tzbsp_video_vars[1].start = fw_start;
    tzbsp_video_vars[0].end = tzbsp_video_vars[1].end = fw_end;
    tzbsp_video_vars[2].start = tzbsp_video_vars[2].end = 0;
    tzbsp_video_fw_loaded = TRUE;
  } while(0);

  /* Configure and start the subsystem */
  err = tzbsp_video_init_and_start();
  if(err)
  {
    tzbsp_video_tear_down(TZBSP_TEAR_DOWN_FULL);
    TZBSP_ERROR_CODE(VIDEO_BRING_UP_FAILED);
  }

  return err;
}


/**
 * Passes the elf_info struct to video bring up, the struct has
 * addresses to the segments and other information
 *
 * @param [in] elf_info   elf_info from the PIL
 *
 * @return \c E_SUCCESS if the SMMU calls were successfully
 * done, error code otherwise.
 */
int tzbsp_video_config(const tzbsp_elf_info_t* elf_info)
{
  s_elf_info = elf_info; /* passing elf_info struct to video code */
  return E_SUCCESS;
}

/**
 * Sets the virtual address (VA) ranges in the device's address map
 * NOTE: Non-pixel ranges only considered on Venus 1.2 and up
 *
 * @param [in] cpa_start    CP VA start
 * @param [in] cpa_size     Size of CP VAR
 * @param [in] image_start  Venus image VA start
 * @param [in] image_size   Size of Venus VAR
 * @param [in] cp_nonpixel_start  CP non-pixel VA start
 * @param [in] cp_nonpixel_size   Size of CP non-pixel VAR
 * @param [out] rsp         Result of this syscall
 * @param [in] rsplen       Size, in bytes, of the response buffer
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_video_set_va_ranges(uint32 cpa_start, uint32 cpa_size,
                              uint32 cp_nonpixel_start, uint32 cp_nonpixel_size,
                              int32* rsp, uint32 rsplen)
{
  uint32 cpa_end, nonpxl_end, image_start, image_end;
  boolean tzbsp_video_has_nonpxl;

  /* Valid response buffer? */
  if(rsplen < sizeof(int32))
  {
    return -E_INVALID_ARG;
  }

  /* Check that the response buffer is within non-secure memory */
  if(!tzbsp_is_ns_range(rsp, sizeof(int32)))
  {
    return -E_BAD_ADDRESS;
  }

  /* Ranges can't be changed once set and the subsystem must be started */
  if(!tzbsp_video_fw_loaded || tzbsp_video_va_ranges_set)
  {
    *rsp = -E_NOT_ALLOWED;
    return -E_NOT_ALLOWED;
  }

  /* Default response */
  *rsp = -E_INVALID_ARG;

  tzbsp_video_has_nonpxl = tzbsp_video_get_hw_ver() > TZBSP_VIDEO_HW_VER_1_2_LT;

  /* Validate the sizes of the regions */
  if(cpa_size == 0 || (tzbsp_video_has_nonpxl && cp_nonpixel_size == 0))
  {
    return -E_INVALID_ARG;
  }

  /* The CPA range must start at virtual address 0 */
  if(cpa_start != 0)
  {
    return -E_INVALID_ARG;
  }

  /* The end address is non-inclusive, hence why size is specified.  However,
   * for the checks, we want to deal with an inclusive end address, so we
   * don't need to worry about the zero address case. */
  cpa_end = cpa_start + cpa_size - 1;
  nonpxl_end = cp_nonpixel_start + cp_nonpixel_size - 1;
  image_start = tzbsp_video_vars[0].start;
  image_end = tzbsp_video_vars[0].end;

  /* Current implementation doesn't handle the wrap around cases */
  if(cpa_start > cpa_end ||
      (tzbsp_video_has_nonpxl && cp_nonpixel_start > nonpxl_end ))
  {
    return -E_INVALID_ARG;
  }

  /* The CPA VAR must contain the image VAR */
  if(image_start < cpa_start || image_end > cpa_end)
  {
    return -E_INVALID_ARG;
  }

  /* The non-pixel VAR needs to fall with CPA VAR, but can't overlap with
   * the image VAR */
  if(tzbsp_video_has_nonpxl &&
     ((cp_nonpixel_start < cpa_start || nonpxl_end > cpa_end) ||
      (image_start <= cp_nonpixel_start && cp_nonpixel_start <= image_end) ||
      (image_start <= nonpxl_end && nonpxl_end <= image_end) ||
      (cp_nonpixel_start <= image_start && image_end <= nonpxl_end)))
  {
    return -E_INVALID_ARG;
  }

  /* Valid VARs; program them into the registers */
  tzbsp_video_vars[1].start = cpa_start;
  tzbsp_video_vars[1].end = cpa_start + cpa_size;
  tzbsp_video_vars[2].start = cp_nonpixel_start;
  tzbsp_video_vars[2].end = cp_nonpixel_start + cp_nonpixel_size;

  tzbsp_video_program_vars();
  tzbsp_video_va_ranges_set = TRUE;

  *rsp = E_SUCCESS;
  return E_SUCCESS;
}

/**
 * @copydoc tzbsp_smmu_map
 */
int tzbsp_video_cpa_contains_memory(uint32 start, uint32 size)
{
  uint32 cpa_start, cpa_end, fw_start, fw_end, end;

  /* No point in checking if the VARs haven't been set */
  if(!tzbsp_video_va_ranges_set)
  {
    return -E_NOT_ALLOWED;
  }

  end = start + size;
  if(size == 0 || end < start)
  {
    return -E_INVALID_ARG;
  }

  /* Get the CPA and image address ranges */
  cpa_start = tzbsp_video_vars[1].start;
  cpa_end   = tzbsp_video_vars[1].end;
  fw_start  = tzbsp_video_vars[0].start;
  fw_end    = tzbsp_video_vars[0].end;

  /* Check if the range is within CPA and doesn't overlap with FW.
   *
   * Because of the checks above, we know that start < end. A range that
   * doesn't overlap Venus firmware would satisfy the following condition:
   *    end <= fw_start or start >= fw_end
   *
   * The complement of that is enough for checking for [start, end) range
   * overlap with [fw_start, fw_end):
   *    end > fw_start and start < fw_end
   */
  if(start < cpa_start || end > cpa_end || (end > fw_start && start < fw_end))
  {
    return -E_OUT_OF_RANGE;
  }

  return E_SUCCESS;
}

/**
 * Returns the FW address (VA) ranges in the device's address map
 *
 * @param [out] rsp     Buffer containing start and end address
 * @param [in] rsplen   Size, in bytes, of the response buffer
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_video_get_fw_range(uint32 *rsp, uint32 rsplen)
{
  /* Ensure we have enough space to return the end address */
  if(rsplen < 2*sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  /* Check that the response buffer is within non-secure memory */
  if(!tzbsp_is_ns_range(rsp, 2*sizeof(uint32)))
  {
    return -E_BAD_ADDRESS;
  }

  rsp[0] = tzbsp_video_vars[0].start;
  rsp[1] = tzbsp_video_vars[0].end;

  return E_SUCCESS;
}

/**
 * @copydoc tzbsp_video_get_hw_ver
 */
uint32 tzbsp_video_get_hw_ver(void)
{
  static uint32 version = 0;
  if(!version)
  {
    version = (HWIO_IN(VENUS0_VENUS_WRAPPER_HW_VERSION)&(0xFFFF0000)); //ignore step version
  }
  return version;
}

/**
 * Starts the Venus core running if the firmware was previously loaded
 * and the core isn't already running.
 *
 * @param [in]  state   State to place processor into; 0 for suspend,
 *                      1 for resume
 * @param [in]  spare   Reserved; SBZ
 * @param [out] rsp     Result of this syscall
 * @param [in]  rsplen  Size, in bytes, of the response buffer
 *
 * @returns E_SUCCESS on success, negative values on failure
 */
int tzbsp_video_set_state(uint32 state, uint32 spare, int32 *rsp, uint32 rsplen)
{
  TZBSP_LOG(TZBSP_MSG_DEBUG, "Venus:%s:state %d",__func__, state);
  /* Valid response buffer? */
  if(rsplen < sizeof(int32))
  {
    return -E_INVALID_ARG;
  }


  /* Check that the response buffer is within non-secure memory */
  if(!tzbsp_is_ns_range(rsp, sizeof(int32)))
  {
    return -E_BAD_ADDRESS;
  }

  if(TZ_VIDEO_STATE_SUSPEND == state)
  {
    tzbsp_video_halt_core();
    *rsp = E_SUCCESS;
  }
  else if(TZ_VIDEO_STATE_RESUME == state)
  {
    *rsp = tzbsp_video_init_and_start();
    if(-E_NOT_ALLOWED == *rsp || -E_ALREADY_DONE == *rsp)
    {
      /* FW hasn't been loaded, or subsytem is already running.
       * Return success, to make this function multiple-call
       * friendly */
      *rsp = E_SUCCESS;
    }
  }
  else
  {
    *rsp = -E_INVALID_ARG;
  }

  return *rsp;
}

