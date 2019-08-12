/*===========================================================================
   Copyright (c) 2013-2015 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/monitor/common/src/qsee_spmi_bus_disable.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/10/15   vg      Updated the with SpmiHalt_DisableAllBuses ()API 
07/31/13   tk      Initial version.
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <SpmiHalt.h>

#include "tzbsp.h"
#include "tzbsp_config.h"
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Monitor call from non-secure side to disable SPMI bus. Disabling SPMI bus
 * prevents PMIC lock-ups caused by SPMI transactions in the middle of PS_HOLD
 * deassertion.
 *
 * @param [in] spare  Not used currently.
 */
int qsee_spmi_bus_disable_ns(uint32 spare)
{
  int err = E_SUCCESS;

  /* Concurrency isn't an issue. Call to disable bus is re-entrant. It will
   * write a bit to halt the bus and then query the status register until the
   * operation is done. */

  if(SPMI_SUCCESS != SpmiHalt_DisableAllBuses())
  {
    err = -E_NOT_AVAILABLE;
  }

  return err;
}
