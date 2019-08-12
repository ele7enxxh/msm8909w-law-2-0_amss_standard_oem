/*=============================================================================

                       S M M U  C L O C K 

GENERAL DESCRIPTION
  This file contains the configuration for the SMMU config clock.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2014
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/kernel/smmu/smmu500/8909/tz/SMMUClock.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/06/14   sc      Added dsb before disabling clock
02/25/14   sc      Initial Version
=============================================================================*/
#include "SMMUTypes.h"
#include "SMMUClock.h"

#define SMMU_8909_BRINGUP

#ifdef SMMU_8909_BRINGUP
#include "tzbsp_sys.h"
#include <HALhwio.h>
#include "tzbsp_hwio.h"
#else
#include "DALSys.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#endif

extern void memory_barrier(void);

#ifdef SMMU_8909_BRINGUP
#define SMMU_CFG_CLK_ENA_SHFT \
  (HWIO_SHFT(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, SMMU_CFG_CLK_ENA))
#define SMMU_APPS_TCU_CLK_ENA_SHFT \
  (HWIO_SHFT(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, APSS_TCU_CLK_ENA))
#define SMMU_GPU_TCU_CLK_ENA_SHFT \
  (HWIO_SHFT(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, GFX_TCU_CLK_ENA))

// reference count APPS and GPU clks
// in order to disable SMMU_CFG_CLK, both reference counts for
// APPS_TCU and GPU_TCU clks have to be 0
extern tzbsp_mutex_t g_tzbsp_clk_mutex;
static uint32 gRefCount[SMMU_CLK_MAX_TCU - 1] = {0};
#else
DalDeviceHandle *pSMMClock = NULL;
ClockIdType SMMUClockID;
#endif

SMMUResult_t SMMU_ClockEnable( SMMU_ClkTCUVoteType eTCU )
{
#ifdef SMMU_8909_BRINGUP
  uint32 int_restore = 0;
  uint32 clkVal = 0;
  uint32 uVal = 0;

  if (eTCU >= SMMU_CLK_MAX_TCU)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  memory_barrier();
  int_restore = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_clk_mutex);

  clkVal = HWIO_IN(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE);

  switch (eTCU)
  {
  case SMMU_CLK_APPS_TCU:
    gRefCount[SMMU_CLK_APPS_TCU]++;
    uVal = (1 << SMMU_CFG_CLK_ENA_SHFT) | (1 << SMMU_APPS_TCU_CLK_ENA_SHFT);
    break;
  case SMMU_CLK_GPU_TCU:
    gRefCount[SMMU_CLK_GPU_TCU]++;
    uVal = (1 << SMMU_CFG_CLK_ENA_SHFT) | (1 << SMMU_GPU_TCU_CLK_ENA_SHFT);
    break;
  default:
    gRefCount[SMMU_CLK_APPS_TCU]++;
    gRefCount[SMMU_CLK_GPU_TCU]++;
    uVal = (1 << SMMU_CFG_CLK_ENA_SHFT) | (1 << SMMU_GPU_TCU_CLK_ENA_SHFT) | (1 << SMMU_APPS_TCU_CLK_ENA_SHFT);
    break;
  }

  clkVal |= uVal;
  HWIO_OUT(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, clkVal);
  memory_barrier();

  tzbsp_mutex_unlock(&g_tzbsp_clk_mutex);
  tzbsp_int_restore(int_restore);
#else
  DALSYS_InitMod(NULL);

  if (pSMMClock == NULL)
  {
    if (DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_CLOCK, &pSMMClock))
    {
      return SMMU_ERR_GENERIC_ERROR;
    }
    if (DAL_SUCCESS != DalClock_GetClockId(pSMMClock, "gcc_smmu_cfg_clk", &SMMUClockID))
    {
      return SMMU_ERR_GENERIC_ERROR;
    }
  }

  if (DAL_SUCCESS != DalClock_EnableClock(pSMMClock, SMMUClockID))
  {
    return SMMU_ERR_GENERIC_ERROR;
  }
#endif // SMMU_8909_BRINGUP

  return SMMU_ERR_OK;
} // SMMU_ClockEnable

SMMUResult_t SMMU_ClockDisable( SMMU_ClkTCUVoteType eTCU )
{
#ifdef SMMU_8909_BRINGUP
  uint32 int_restore = 0;

  if (eTCU >= SMMU_CLK_MAX_TCU)
  {
    return SMMU_ERR_GENERIC_ERROR;
  }

  memory_barrier();
  int_restore = tzbsp_int_disable_all();
  tzbsp_mutex_lock(&g_tzbsp_clk_mutex);

  switch (eTCU)
  {
  case SMMU_CLK_APPS_TCU:
    --gRefCount[SMMU_CLK_APPS_TCU];
    break;
  case SMMU_CLK_GPU_TCU:
    --gRefCount[SMMU_CLK_GPU_TCU];
    break;
  default:
    --gRefCount[SMMU_CLK_APPS_TCU];
    --gRefCount[SMMU_CLK_GPU_TCU];
    break;
  }

  if (gRefCount[SMMU_CLK_APPS_TCU] == 0)
  {
    // done with APPS TCU
    HWIO_OUTF(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, APSS_TCU_CLK_ENA, 0);
  }
  if (gRefCount[SMMU_CLK_GPU_TCU] == 0)
  {
    // done with GPU TCU
    HWIO_OUTF(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, GFX_TCU_CLK_ENA, 0);
  }
  if (gRefCount[SMMU_CLK_APPS_TCU] == 0 && gRefCount[SMMU_CLK_GPU_TCU] == 0)
  {
    // safe to disable SMMU_CFG_CLK
    HWIO_OUTF(GCC_APCS_TZ_SMMU_CLOCK_BRANCH_ENA_VOTE, SMMU_CFG_CLK_ENA, 0);
  }

  memory_barrier();
  tzbsp_mutex_unlock(&g_tzbsp_clk_mutex);
  tzbsp_int_restore(int_restore);
#else
  if (DAL_SUCCESS != DalClock_DisableClock(pSMMClock, SMMUClockID))
  {
    return SMMU_ERR_GENERIC_ERROR;
  }
#endif // SMMU_8909_BRINGUP

  return SMMU_ERR_OK;
} // SMMU_ClockDisable
