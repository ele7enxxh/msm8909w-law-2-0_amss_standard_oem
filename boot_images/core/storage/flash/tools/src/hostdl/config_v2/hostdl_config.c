/*==================================================================
 *
 * FILE:        hostdl_config.c
 *
 * SERVICES:    Target Specific Initialization
 *
 * DESCRIPTION:
 *    This module contains startup/memory initialization required to
 *    execute flashprg standalone.  Some of these functions are NOT
 *    required for standard operation.
 *
 * Copyright (c) 2009,2011-2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/tools/src/hostdl/config_v2/hostdl_config.c#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 02/02/15     sb      Limit spmiCfg_Init() call to ehostdl
 * 09/30/14     sb      Configure GPIOs for NAND interface
 * 04/09/14     eo      Call clock_DebugInit() before clock_Init()
 * 06/14/13     eo      Call clock API to enable QPIC clocks 
 * 05/03/13     eo      Clear boot partition select reg 
 * 04/08/13     eo      Call pmic support after zi init
 * 04/01/13     eo      Add pmic support for warm reset
 * 10/19/12     sv      Update hw_init for 9x25
 * 09/17/12     eo      Ehostdl/hostdl support for 9x25
 * 09/16/11     sv      Add clock init/watchdog for - 9x15, cleanups
 * 11/24/09     jz      Fixed compiler warning (added a newline at eof)
 * 05/05/09     sv      Remove memory_init, add init_hw
 * 02/27/09     jz      Initial version
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "HALhwio.h"

#include "hostdl_startup.h"
#include "hostdl.h"
#include "hostdl_config.h"
#include "hostdl_flash.h"
#include "hostdl_hwio.h"

#include "ClockBoot.h"

#include "SpmiCfg.h" 
#include "SpmiBus.h"
#include "pm_pon.h"

#ifdef FLASH_EHOSTDL_INCLUDE_CLK_INIT
  #include "clkregim.h"
#endif
//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Extern Definitions
//--------------------------------------------------------------------------

// These symbols are created by the ARM linker and are referenced by using
// these special characters ($$).
/*lint -e27 -e10 -e19 -save*/
extern byte *Image$$APP_RAM$$ZI$$Base;
extern byte *Image$$APP_RAM$$ZI$$Limit;
/*lint -restore*/
//extern boolean clk_regime_apps_init_boot (void);
//--------------------------------------------------------------------------
// Global Data Declarations
//--------------------------------------------------------------------------

__global_reg (1)
     dword *dst32;              /*lint !e102 !e745 */
__global_reg (2)
     dword *src32;

//--------------------------------------------------------------------------
// Function Declarations
//--------------------------------------------------------------------------

/*===========================================================================

  This function sets PS HOLD to warm reset

===========================================================================*/
void hostdl_pmic_init (void)
{
  /* Init PS HOLD to warm reset */
#ifdef BUILD_EHOSTDL
  if ((SPMI_CFG_SUCCESS == SpmiCfg_Init(TRUE)) &&
    (SPMI_BUS_SUCCESS == SpmiBus_Init())) 
  {
    (void) pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_WARM_RESET);
  }
#else 
  /* Call to spmicfg_init() not required in hostdl. TZ will init spmicfg 
   * and lock registers. 
   */ 
  if (SPMI_BUS_SUCCESS == SpmiBus_Init()) 
  {
    (void) pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_WARM_RESET);
  }
#endif
}

/*===========================================================================

  This function sets TLMM for NAND GPIOs 

===========================================================================*/
void hostdl_gpio_init (void)
{

#ifdef TARGET_ID_9K
  HWIO_OUT(TLMM_EBI2_EMMC_GPIO_CFG, 0x2);
#endif

#ifdef TARGET_ID_8K
  HWIO_OUT(TLMM_SDC1_HDRV_PULL_CTL, 0x00090404);
  HWIO_OUT(TLMM_EBI2_EMMC_GPIO_CFG, 0x1C210841);
#endif
}
/*===========================================================================

  This function causes the MSM to reset

===========================================================================*/
void target_reset (void)
{
  /* clear boot partition select reg before pulling ps_hold low. */
  HWIO_OUT( BOOT_PARTITION_SELECT, 0x0 );
  
  /* pull down pmic PS_HOLD to force a reset*/
  HWIO_OUTF( MPM2_MPM_PS_HOLD, PSHOLD, 0x0 ); 

  for (;;)
  {
    // let the watch dog timer expire here...
  }

  /* does not return */
}

/*===========================================================================

  This function does the hardware initializations
  This will be used in Emergency Hostdl
===========================================================================*/
void hw_init()
{
#ifdef BUILD_EHOSTDL  

  Clock_Init(CLOCK_BOOT_PERF_MAX, CLOCK_BOOT_PERF_MAX);

  /* Enable QPIC clocks.*/
  Clock_EnableQPICForDownloadMode();
#endif
  
  ENABLE_WATCHDOG();

  return;
}
