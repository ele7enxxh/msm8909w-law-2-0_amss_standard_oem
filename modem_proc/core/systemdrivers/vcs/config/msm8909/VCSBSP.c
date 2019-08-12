/*
==============================================================================

FILE:         VCSBSP.c

DESCRIPTION:
  This file contains VCS bsp data for DAL based driver.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/config/msm8909/VCSBSP.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
01/22/14   lil     Created.

==============================================================================
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "VCSBSP.h"
#include "pmapp_npa.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Data Declarations
==========================================================================*/

/*
 *  VCS_RailCornerConfigsCX
 *
 *  Set of rail corner configurations.
 */
static VCSRailCornerConfigType VCS_CornerConfigCX[] =
{
  {
    .eCornerMin        = VCS_CORNER_LOW,
    .eCornerMax        = VCS_CORNER_TURBO,
    .eCornerInit       = VCS_CORNER_NOMINAL,
    .pVoltageRange     = NULL, /* We do not know about CX's voltage table */
    .nNumVoltageRanges = 0,    /* We do not directly manage CX voltages */
    .HWVersion         = { {0x0, 0x0}, {0xFF, 0xFF} },
  }
};


/*
 *  VCS_RailCornerConfigsMX
 *
 *  Set of rail corner configurations.
 */
static VCSRailCornerConfigType VCS_CornerConfigMX[] =
{
  {
    .eCornerMin        = VCS_CORNER_LOW,
    .eCornerMax        = VCS_CORNER_TURBO,
    .eCornerInit       = VCS_CORNER_NOMINAL,
    .pVoltageRange     = NULL, /* We do not know about MX's voltage table */
    .nNumVoltageRanges = 0,    /* We do not directly manage MX voltages */
    .HWVersion         = { {0x0, 0x0}, {0xFF, 0xFF} },
  }
};


/*
 * VCS_RailConfigs
 *
 * Set of rail configurations.
 */
static VCSRailConfigType VCS_RailConfigs[] =
{
  {
    .eRail             = VCS_RAIL_CX,
    .szName            = VCS_NPA_RESOURCE_VDD_CX,
    .pCornerConfig     = VCS_CornerConfigCX,
    .nNumCornerConfigs = ARR_SIZE(VCS_CornerConfigCX),
    .szNameDependency  = PMIC_NPA_GROUP_ID_RAIL_CX,
    .bEnableCPR        = FALSE,
    .bEnableDVS        = TRUE
  },
  {
    .eRail             = VCS_RAIL_MX,
    .szName            = VCS_NPA_RESOURCE_VDD_MX,
    .pCornerConfig     = VCS_CornerConfigMX,
    .nNumCornerConfigs = ARR_SIZE(VCS_CornerConfigMX),
    .szNameDependency  = PMIC_NPA_GROUP_ID_RAIL_MX,
    .bEnableCPR        = FALSE,
    .bEnableDVS        = TRUE
  }
};

/*
 * Corner mapping from VCS enum to PMIC enum
 */
uint32 VCS_CornerPMICMap[VCS_CORNER_NUM_OF_CORNERS] =
{
  PMIC_NPA_MODE_ID_CORE_RAIL_OFF,          // VCS_CORNER_OFF
  PMIC_NPA_MODE_ID_CORE_RAIL_RETENTION,    // VCS_CORNER_RETENTION
  PMIC_NPA_MODE_ID_CORE_RAIL_LOW,          // VCS_CORNER_RETENTION_PLUS
  PMIC_NPA_MODE_ID_CORE_RAIL_LOW,          // VCS_CORNER_LOW_MINUS
  PMIC_NPA_MODE_ID_CORE_RAIL_LOW,          // VCS_CORNER_LOW
  PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL,      // VCS_CORNER_LOW_PLUS
  PMIC_NPA_MODE_ID_CORE_RAIL_NOMINAL,      // VCS_CORNER_NOMINAL
  PMIC_NPA_MODE_ID_CORE_RAIL_TURBO,        // VCS_CORNER_NOMINAL_PLUS
  PMIC_NPA_MODE_ID_CORE_RAIL_TURBO         // VCS_CORNER_TURBO
};
  
/*
 * List of CPUs.
 */
static VCSCPUConfigType VCS_CPUConfigs[] =
{
  {
    .eCPU       = CLOCK_CPU_MSS_Q6,
    .szName     = "/clk/cpu",
    .eRail      = VCS_RAIL_CX,
    .pLDOConfig = NULL,
  },
};


/*
 *  VCS_BSPConfig
 *
 *  List and length of Rail and CPU configurations.
 */
const VCSBSPConfigType VCS_BSPConfig =
{
  .pRailConfig     = VCS_RailConfigs,
  .nNumRailConfigs = ARR_SIZE(VCS_RailConfigs),
  .pCPUConfig      = VCS_CPUConfigs,
  .nNumCPUConfigs  = ARR_SIZE(VCS_CPUConfigs),
  .pnCornerPMICMap = VCS_CornerPMICMap
};

