#ifndef __VCSBSP_H__
#define __VCSBSP_H__
/*
===========================================================================
*/
/**
  @file VCSBSP.h

  BSP interface definitions for the DAL VCS device driver.
*/
/*
  ====================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ====================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/vcs/src/VCSBSP.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  01/22/14   lil     Created.
 
  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "comdef.h"
#include "VCSDefs.h"
#include "ClockDefs.h"
#include "DDIChipInfo.h"
#include "HALldo.h"
#include "npa_resource.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * VCSLogType
 *
 * Default configuration for the VCS log.
 *
 *  nLogSize        - Log size in bytes.  Must be a power of 2.
 */
typedef struct
{
  uint32 nLogSize;
} VCSLogType;


/*
 * VCSPropertyValueType
 *
 * Generic VCS property data.  Actual data type will depend on the property
 * and will not even necessarily be a pointer (i.e. could be straight uint32).
 */
typedef const void *VCSPropertyValueType;


/*
 * VCSHWVersionType;
 *
 * HW version information.
 *
 *  Min             - Minimum supported HW Major/Minor revision
 *  Max             - Maximum supported HW Major/Minor revision
 *                    '0xFF' indicates all versions.
 *  eChipInfoFamily - Chip this configuration is for. '0' indicates all chips.
 *  aeChipInfoId    - Array of chip id's that the configuration supports.
 *                    NULL indicates all chips.
 */
typedef struct
{
  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Min;

  struct
  {
    uint8 nMajor;
    uint8 nMinor;
  } Max;

  DalChipInfoFamilyType    eChipInfoFamily;
  const DalChipInfoIdType *aeChipInfoId;

} VCSHWVersionType;

/*
 * VCSCornerVoltageType
 *
 * BSP data structure for describing the voltage for each corner.
 *
 *  eCorner     - The voltage corner
 *  nVoltageUV  - The voltage in uV for this corner
 */
typedef struct
{
  VCSCornerType eCorner;
  uint32        nVoltageUV;
} VCSCornerVoltageType;


/*
 * VCSCornerVoltageRangeType
 *
 * BSP data structure for describing the voltage range for each corner.
 *
 *  eCorner - The voltage corner
 *  nMinUV  - The minimum rail voltage in uV for this corner
 *  nMaxUV  - The maximum rail voltage in uV for this corner
 */
typedef struct
{
  VCSCornerType eCorner;
  uint32        nMinUV;
  uint32        nMaxUV;
} VCSCornerVoltageRangeType;


/*
 * VCSRailCornerConfigType
 *
 * BSP Data structure for describing the rail configuration data.
 *
 *  eCornerMin        - The minimum corner at which the rail is allowed to operate
 *  eCornerMax        - The max corner at which the rail is allowed to operate
 *  eCornerInit       - The rail's initial corner at boot-up
 *  pVoltageRange     - Voltage corner table
 *  nNumVoltageRanges - Total number of corners used
 *  HWVersion         - Supported hardware version info
 */
typedef struct
{
  VCSCornerType              eCornerMin;
  VCSCornerType              eCornerMax;
  VCSCornerType              eCornerInit;
  VCSCornerVoltageRangeType *pVoltageRange;
  uint32                     nNumVoltageRanges;
  VCSHWVersionType           HWVersion;
} VCSRailCornerConfigType;


/*
 * VCSLDOConfigType
 *
 * BSP data structure for describing the LDO configuration.
 *
 *  eLDO               - LDO enum
 *  szName             - Name of NPA resource for this LDO
 *  nRetVoltageUV      - LDO's retention voltage in uV
 *  nHeadroom          - Amount of headroom between LDO and rail in corner
 *  pCornerVoltage     - LDO voltage corner table
 *  nNumCornerVoltages - Number of corners
 *  bEnable            - Enable switch for this LDO.
 */
typedef struct
{ 
  HAL_ldo_LDOType       eLDO;
  const char           *szName;
  uint32                nRetVoltageUV;
  uint32                nHeadroom;
  VCSCornerVoltageType *pCornerVoltage;
  uint32                nNumCornerVoltages;
  boolean               bEnable;
} VCSLDOConfigType;


/*
 * VCSCPUConfigType
 *
 * BSP data structure for describing the CPU configuration.
 *
 *  eCPU        - CPU enum
 *  szName      - Name of the NPA client handle for this CPU to vote on the rail
 *  eRail       - Rail enum for this CPU's power source.
 *  pLDODConfig - Pointer to LDO configuration (if applicable, otherwise NULL).
 *
 */
typedef struct
{
  ClockCPUType      eCPU;
  const char       *szName;
  VCSRailType       eRail;
  VCSLDOConfigType *pLDOConfig;
} VCSCPUConfigType;


/*
 * VCSRailConfigType
 *
 * BSP Data structure for describing the rail configuration.
 *
 *  eRail             - Rail enum
 *  szName            - Name of rail resource
 *  pCornerConfig     - The rail's configuration data
 *  nNumCornerConfigs - Total number of configurations
 *  szNameDependency  - Name of NPA resource dependency
 *  bEnableCPR        - Enable switch for invoking CPR
 *  bEnableDVS        - Enable switch for dynamic voltage scaling
 */
typedef struct
{
  VCSRailType              eRail;
  const char              *szName;
  VCSRailCornerConfigType *pCornerConfig;
  uint32                   nNumCornerConfigs;
  const char              *szNameDependency;
  boolean                  bEnableCPR;
  boolean                  bEnableDVS;
} VCSRailConfigType;


/*
 * VCSBSPConfigType
 *
 * BSP Data structure for list of the rail and CPU configurations.
 *
 *  pRailConfig     - Pointer to array of rail configurations
 *  nNumRailConfigs - Number of rails
 *  pCPUConfig      - Pointer to array of CPU configurations
 *  nNumCPUConfigs  - Number of CPUs
 *  pnCornerPMICMap - Pointer to array of integer PMIC corner enum values.
 */
typedef struct
{
  VCSRailConfigType *pRailConfig;
  uint32             nNumRailConfigs;
  VCSCPUConfigType  *pCPUConfig;
  uint32             nNumCPUConfigs;
  uint32            *pnCornerPMICMap;
} VCSBSPConfigType;


#endif  /* __VCSBSP_H__ */ 

