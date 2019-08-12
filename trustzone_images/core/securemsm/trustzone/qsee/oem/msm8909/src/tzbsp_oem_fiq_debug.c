/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/oem/msm8909/src/tzbsp_oem_fiq_debug.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
24/06/12   tk      Initial version.
=============================================================================*/

#include <comdef.h>
#include "tzbsp_config.h"
#include "tzbsp_target.h"
#include "tzbsp_mem.h"

static const uint16 g_tzbsp_allowed_debug_fiqs[] =
{
  TZBSP_INT_NON_SEC_WDOG_BITE, /* Non-Secure WDT */
};

/**
 * Controls which function can be used for non-secure FIQ debugger. Non-secure
 * FIQ debug allows handing control of an FIQ interrupt to non-secure side for
 * further processing.
 *
 * @param [in] intnum   QGIC interrupt number the non-secure side is trying to
 *                      install as the debug FIQ.
 *
 * @return \c TRUE if the interrupt is allowed to be used for non-secure FIQ
 *         debugging, \c FALSE otherwise.
 */
boolean tzbsp_is_allowed_debug_int(uint32 intnum)
{
  int i;
  for(i=0; i < TZBSP_ARRAY_SIZE(g_tzbsp_allowed_debug_fiqs); i++)
  {
    if(intnum == g_tzbsp_allowed_debug_fiqs[i])
    {
      return TRUE;
    }
  }

  return FALSE;
}
