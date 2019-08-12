/**
* @file tzbsp_pil_xpu.c
* @brief Provide MPU protection for memory areas defined by PIL
*
* This file provides MPU locking and unlocking for ELF loaded images.
* It also adds MPU locked regions to the blaklist, which are considered
* secure memory areas from the perspecive of the HLOS.
*
*/


/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/img_auth/src/tzbsp_pil_xpu.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---      ------------------------------------
22/10/10   tk       Created
===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include <string.h>

#include "tzbsp.h"
#include "tzbsp_chipset.h"
#include "tzbsp_target.h"
#include "tzbsp_errno.h"
#include "tzbsp_mem.h"
#include "tzbsp_pil.h"
#include "tzbsp_xpu.h"
#include "tzbsp_xpu_config.h"
#include "tzbsp_sys.h"
#include "secboot_hw.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
boolean tzbsp_subsys_is_valid_area(tz_pil_proc_e_type proc, uint32 start,
                                   uint32 end);
int tzbsp_subsys_lock(tz_pil_proc_e_type proc);
int tzbsp_subsys_unlock(tz_pil_proc_e_type proc, boolean erase);

/**
 * Checks whether the image entry and segments are in the memory areas
 * allocated for the subsystem.
 *
 * @param[in] proc    Subsystem ID.
 * @param[in] hdr     Pointer to the image ELF header.
 * @param[in] count   Number of segments in the image.
 * @param[in] entry   The entry address to the image.
 *
 * @return \c E_SUCCESS if the image entry and segments are in valid memory
 *         areas, error code otherwise.
 */
int tzbsp_validate_image_area(tz_pil_proc_e_type proc,
                              const Elf32_Phdr* hdr,
                              uint32 count, uint32 entry)
{
  int i;
  uint32 start, end;

  /* Also entry must be within the image area. */
  if(!tzbsp_subsys_is_valid_area(proc, entry, entry + sizeof(entry)))
  {
    TZBSP_ERROR_CODE(VALIDATE_IMAGE_AREA_INVALID_ENTRY);
    return -E_FAILURE;
  }

  /* All image segments must be within the image area (either EBI or internal
   * memory). */
  for(i = 0; i < count; i++, hdr++)
  {
    if(tzbsp_is_valid_segment(hdr))
    {
      start = hdr->p_paddr;
      end = hdr->p_paddr + hdr->p_memsz;

      if(!tzbsp_subsys_is_valid_area(proc, start, end))
      {
        TZBSP_ERROR_CODE(VALIDATE_IMAGE_AREA_INVALID_SEGMENT);
        return -E_FAILURE;
      }
    }
  }

  return E_SUCCESS;
}

/* @copydoc tzbsp_pil_lock_xpu */
int32 tzbsp_pil_lock_xpu(tz_pil_proc_e_type proc, const Elf32_Phdr* hdr,
                         uint32 count, uint32 entry)
{
  int ret = E_SUCCESS;

  do
  {
    if(tzbsp_validate_image_area(proc, hdr, count, entry) < 0)
    {
      TZBSP_ERROR_CODE(PIL_LOCK_XPU_IMAGE_AREA_INVALID);
      ret = -E_FAILURE;
      break;
    }

    if (tzbsp_subsys_lock(proc) < 0)
    {
      TZBSP_ERROR_CODE(PIL_LOCK_XPU_SUBSYS_LOCK_FAILED);
      ret = -E_FAILURE;
      break;
    }
  } while(0);

  return ret;
}

int32 tzbsp_pil_unlock_xpu(tz_pil_proc_e_type proc)
{
  if (tzbsp_subsys_tear_down(proc, TZBSP_TEAR_DOWN_CPU) < 0)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_XPU_TEAR_DOWN_FAILED);
    return -E_FAILURE;
  }

  if (tzbsp_subsys_unlock(proc, FALSE) < 0)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_XPU_UNLOCK_FAILED);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}

int32 tzbsp_pil_unlock_xpu_zero(tz_pil_proc_e_type proc)
{
  if (tzbsp_subsys_tear_down(proc, TZBSP_TEAR_DOWN_CPU) < 0)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_ZERO_XPU_TEAR_DOWN_FAILED);
    return -E_FAILURE;
  }

  if (tzbsp_subsys_unlock(proc, TRUE) < 0)
  {
    TZBSP_ERROR_CODE(PIL_UNLOCK_ZERO_XPU_UNLOCK_FAILED);
    return -E_FAILURE;
  }

  return E_SUCCESS;
}
