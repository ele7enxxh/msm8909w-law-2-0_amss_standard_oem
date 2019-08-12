/**
@file tzbsp_dload_mode.c
@brief Emergency download mode functions.
*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/src/tzbsp_dload_mode.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/26/11   tk      Initial version
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

#include "tzbsp_errno.h"
#include "tzbsp.h"
#include "tzbsp_sys.h"
#include "tzbsp_config.h"
#include "tzbsp_xpu.h"
#include "tzbsp_log.h"
#include "tzbsp_secboot.h"
#include "tzbsp_mem.h"
#include "tzbsp_chipset.h"
#include "tzbsp_hwio.h"
#include "tzbsp_ext_os.h"
#include "HALhwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* Magic numbers to indicate a boot into download mode. */
#define TZBSP_DLOAD_MAGIC_NUM_1     0xE47B337D
#define TZBSP_DLOAD_MAGIC_NUM_2     0xCE14091A

#define SBL_DLOAD_MODE_BIT_MASK 0x00000010


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} tzbsp_dload_id_type;

/* TODO: Remove once 8974v1 support is dropped */
tzbsp_dload_id_type *dload_magic_ptr = (tzbsp_dload_id_type*) DLOAD_ID_ADDR;

/**
 * @return TRUE if the download mode magic is set, FALSE otherwise.
 */
boolean tzbsp_is_dload_mode_set( void )
{
#if 0
  static boolean dload_mode_init_done   = FALSE;
  static boolean dload_mode_cookies_set = FALSE;

  if(!dload_mode_init_done)
  {
    dload_mode_init_done = TRUE;

    /* TODO: Make local once 8974v1 support is dropped */
    //tzbsp_dload_id_type *ptr = (tzbsp_dload_id_type*) DLOAD_ID_ADDR;
    /* Check to see if download ID is present. */
    if ( dload_magic_ptr->magic_1 == TZBSP_DLOAD_MAGIC_NUM_1 &&
        dload_magic_ptr->magic_2 == TZBSP_DLOAD_MAGIC_NUM_2 )
    {
      dload_mode_cookies_set = TRUE;
    }
  }

  return dload_mode_cookies_set;
#endif
  return HWIO_TCSR_BOOT_MISC_DETECT_INM(SBL_DLOAD_MODE_BIT_MASK);
}

#ifdef TZBSP_WITH_TBASE
/**
 * @return TRUE if the download mode magic is set, FALSE otherwise.
 */
boolean tzbsp_is_dload_mode_set_flat_mapped( void )
{
  return in_dword_masked(HWIO_TCSR_BOOT_MISC_DETECT_PHYS, SBL_DLOAD_MODE_BIT_MASK);
}
#endif

/**
 * Sets the download mode magic.
 */
void tzbsp_set_dload_mode(void)
{
  /* TODO: Make local once 8974v1 support is dropped */
  //tzbsp_dload_id_type *ptr = (tzbsp_dload_id_type*) DLOAD_ID_ADDR;
  dload_magic_ptr->magic_1 = TZBSP_DLOAD_MAGIC_NUM_1;
  dload_magic_ptr->magic_2 = TZBSP_DLOAD_MAGIC_NUM_2;
}

/**
 * Emergency download mode supports memory dumping feature. TZ skips some
 * of the XPU lock down for the emergency download. The following
 * conditions must be met to allow memory dumping:
 *    - Authentication is not enabled OR debug is enabled for the signed image
 *    - FEATURE_DLOAD_MEM_DEBUG is defined.
 *    - Download mode magic is set.
 *
 * @return TRUE if lock down should be removed to allow download mode
 *         memory dumping, FALSE otherwise.
 */
boolean tzbsp_allow_memory_dump()
{
#if defined FEATURE_DLOAD_MEM_DEBUG
#ifdef TZBSP_WITH_TBASE
  boolean is_dload_mode_set;

  if(tzbsp_sysdbg_flat_mapped()) 
  {
    is_dload_mode_set = tzbsp_is_dload_mode_set_flat_mapped();
  }
  else
  {
    is_dload_mode_set = tzbsp_is_dload_mode_set();
  }
#endif

#ifdef TZBSP_WITH_TBASE
  if((tzbsp_secboot_hw_is_auth_enabled(1) == 0 || tzbsp_is_debug_enabled()) 
      && is_dload_mode_set)
#else
  if((tzbsp_secboot_hw_is_auth_enabled(1) == 0 || tzbsp_is_debug_enabled()) 
	  && tzbsp_is_dload_mode_set())
#endif
  {
    return TRUE;
  }
#endif

  /* By default memory dumping is denied. */
  return FALSE;
}

boolean tzbsp_is_nonsecure_device(void)
{
  return (tzbsp_secboot_hw_is_auth_enabled(1) == 0 ||
          tzbsp_is_debug_enabled());
}

/**
 * System call for SBL to check if memory dumping should be allowed
 *
 * @param [out] rsp   Pointer to buffer to store the boolean result;
 *                      \c TRUE if dumping allowed, \c FALSE otherwise
 * @param [in] size   Size of the buffer provided
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_security_allows_mem_dump(uint32 *rsp, uint32 rsplen)
{
  /* Ensure the response buffer is in non-secure memory and is large
   * enough to handle the response */
  if(rsplen < sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(rsp, rsplen))
  {
    return -E_BAD_ADDRESS;
  }

  /* Populate the response and return success */
  *rsp = (uint32)(tzbsp_secboot_hw_is_auth_enabled(1) == 0 || tzbsp_is_debug_enabled() || tzbsp_is_retail_crash_dump_enable());

  return E_SUCCESS;
}

/**
 * System call for UEFI to check if memory dumping should be allowed.
 * Identical to tzbsp_security_allows_mem_dump().
 *
 * @param [out] rsp   Pointer to buffer to store the boolean result;
 *                      \c TRUE if dumping allowed, \c FALSE otherwise
 * @param [in] size   Size of the buffer provided
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_security_allows_mem_dump_2(uint32 *rsp, uint32 rsplen)
{
  return tzbsp_security_allows_mem_dump(rsp, rsplen);
}

/**
 * System call for SBL to check if retail unlock is enable
 *
 * @param [out] rsp   Pointer to buffer to store the boolean result;
 *                      \c TRUE if retail unloc enable, \c FALSE otherwise
 * @param [in] size   Size of the buffer provided
 *
 * @return E_SUCCESS if the call succeeded, error code otherwise.
 */
int tzbsp_is_retail_unlock_enable(uint32 *rsp, uint32 rsplen)
{
  /* Ensure the response buffer is in non-secure memory and is large
   * enough to handle the response */
  if(rsplen < sizeof(uint32))
  {
    return -E_INVALID_ARG;
  }

  if(!tzbsp_is_ns_range(rsp, rsplen))
  {
    return -E_BAD_ADDRESS;
  }

  /* Populate the response and return success */
  *rsp = (uint32)tzbsp_is_retail_crash_dump_enable();

  return E_SUCCESS;
}

/**
 * System call for non-secure side to start spinning in TZ with both IRQ and
 * FIQ interrupts disabled. The purpose of this call is to allow HLOS to
 * simulate situations where watchdog happens while one or more of the CPUs is
 * in a state where it cannot handle interrupts. This function will return
 * without any side effects if secure watchdog debug isn't enabled.
 *
 * @note This call is placed in this file as it is a way to test CPU context
 *       dumps in download mode.
 *
 * @param [in] spare  Currently not used. Reserved for future use.
 *
 * @return \c E_SUCCESS on devices that don't allow secure watchdog debug.
 */
int tzbsp_spin_irqs_fiqs_masked_ns(uint32 spare)
{
  if(tzbsp_allow_sec_wdog_debug())
  {
    (void) tzbsp_int_disable_all();
    while(1);
  }

  return E_SUCCESS;
}
