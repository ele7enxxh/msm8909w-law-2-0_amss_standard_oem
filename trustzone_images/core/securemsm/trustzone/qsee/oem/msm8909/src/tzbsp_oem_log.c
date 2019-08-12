/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/oem/msm8909/src/tzbsp_oem_log.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/05/10   tk      Initial version.
=============================================================================*/

#include <comdef.h>
#include <secboot_hw.h>
#include "tzbsp_config.h"
#include "tzbsp_log.h"
#include "tzbsp_secboot.h"

extern uint8 in_error_fatal; 
/**
 * Allows defining the output target for TZ log messages. Currently \c
 * TZBSP_LOG_TARGET_JTAG and \c TZBSP_LOG_TARGET_RING_BUFFER are supported.
 *
 * Zero value effectively disables logging.
 */
uint32 g_tzbsp_log_target = TZBSP_LOG_TARGET_RING_BUFFER_BIT |
                            TZBSP_LOG_TARGET_JTAG_BIT;

/**
 * Allows overriding XPU syndrome dumping. If 1, then XPU violation error
 * syndrome registers are dumped. If 0, then XPU violations are still
 * acknowledged by the XPU ISR, but the syndrome registers are not dumped.
 */
uint32 g_tzbsp_dump_xpu_syndrome = TZBSP_XPU_SYNDROME_DUMPING;

/**
 * All calls to \c tzbsp_log will be filtered by this function. Allowing TZ
 * logging allows attackers to extract information from TZ. For example, error
 * codes in the log allow tracing function execution path.
 *
 * @return \c FALSE if logging isn't allowed (\c tzbsp_log will skip logging),
 *         \c TRUE if logging is allowed.
 */
boolean tzbsp_oem_allow_logging(void)
{
#ifdef VIRTIO_8962
#warning VIRTIO_8962: forcing logging in tzbsp_oem_allow_logging 
	return TRUE;
#endif
#ifndef TZBSP_RUMI
  /* By default, logging is disabled if secure boot is enabled. */
  if(tzbsp_secboot_hw_is_auth_enabled(1))
  {
    /* Enable fatal error string logging for secure enabled device. */
    if(tzbsp_is_retail_crash_dump_enable() || tzbsp_is_debug_enabled() || (in_error_fatal == 1))
      return TRUE;
    else
    return FALSE;
  }
  else
  {
#endif
    return TRUE;
#ifndef TZBSP_RUMI
  }
#endif
}
