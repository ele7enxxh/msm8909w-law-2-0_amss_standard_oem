/**
@file tzbsp_wlan.c
@brief Trustzone WLAN Subsystem related functions.

*/
/*===========================================================================
   Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/src/tzbsp_wlan.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------ 
08/12/11   sg      Initial version
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>
#include <HALhwio.h>
#include "ClockPIL.h"

#include "tzbsp.h"
#include "tzbsp_config.h"
#include "tzbsp_errno.h"
#include "tzbsp_sys.h"
#include "tzbsp_log.h"
#include "tzbsp_chipset.h"
#include "tzbsp_hwio.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/* The CMEM dump area needs to be at least 5 bytes to allow writing error
 * condition related magic numbers. */
#define CMEM_DUMP_MIN_SIZE            5

/* The magic is "TZCD", Trust Zone Cmem Dump. */
#define CMEM_WRITE_MAGIC(base, val)   \
  ((uint8*)base)[0] = 0x54;           \
  ((uint8*)base)[1] = 0x5A;           \
  ((uint8*)base)[2] = 0x43;           \
  ((uint8*)base)[3] = 0x44;           \
  ((uint8*)base)[4] = val

typedef enum
{
  CMEM_DUMP_NOT_ALLOWED = 1,
} tzbsp_cmem_dump_deny_et;

/*----------------------------------------------------------------------------
 * Global variables and structs
 * -------------------------------------------------------------------------*/

//static void* g_cmem_dump_base;
//static size_t g_cmem_dump_size;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

void tzbsp_program_addr_wlan(uint32 addr)
{
  uint32 reg;

  /* Set bits 0 (HIGH_IVT) and 2 (REMAP_EN) of WLAN_PMU_CCPU_CTL */
  reg = HWIO_IN(WCSS_A_PMU_CCPU_CTL);
  HWIO_OUT(WCSS_A_PMU_CCPU_CTL, reg | 1 | (1<<2));


  /* Write start address >> 16 into WLAN_PMU_CCPU_BOOT_REMAP_ADDR */
  HWIO_OUT(WCSS_A_PMU_CCPU_BOOT_REMAP_ADDR, addr >> 16);

  /* Clear bit 0 (WARM_BOOT) of WLAN_PMU_CFG */
  reg = HWIO_IN(WCSS_A_PMU_CFG);
  HWIO_OUT(WCSS_A_PMU_CFG, (reg & ~(1)));
  tzbsp_dsb();
}

int tzbsp_wlan_tear_down(tzbsp_tear_down_scope_et scope)
{
  if(FALSE == Clock_ResetProcessor(CLOCK_PROCESSOR_WCN))
  {
    return -E_FAILURE;
  }

  return -E_SUCCESS;
}

int tzbsp_wlan_bring_up(uint32 addr)
{
  static boolean cold_boot = TRUE;
  
  if(cold_boot == TRUE)
  {
  if(FALSE == Clock_SetupProcessor(CLOCK_PROCESSOR_WCN))
  {
    return -E_FAILURE;
  }
	cold_boot = FALSE;
  }

  tzbsp_program_addr_wlan(addr);

  if(FALSE == Clock_EnableProcessor(CLOCK_PROCESSOR_WCN))
  {
    return -E_FAILURE;
  }

  return E_SUCCESS;
}
