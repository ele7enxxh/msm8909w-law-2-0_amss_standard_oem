/*=========================================================================

                         CHIPINFO

GENERAL DESCRIPTION
  This file contains the implementation of the Chipinfo APIs

EXTERNALIZED FUNCTIONS
  Chipinfo_GetId
  Chipinfo_GetFamily
  Chipinfo_GetVersion

INITIALIZATION AND SEQUENCING REQUIREMENTS
 None

      Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
==========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdrivers/chipinfo/src/Chipinfo.c#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $
  $Change: 10727476 $

when       who     what, where, why 
--------   ---     --------------------------------------------------------- 
08/17/12   pbitra     Initial version. 
11/07/12   pbitra     Removed Chip Id and Version detection. 
                      Corrected ChipFamily detection 
01/24/13   pbitra     Added Chip Version detection support.

==========================================================================*/ 

/*=========================================================================
      Include Files
==========================================================================*/
#include "ChipinfoHWIO.h"
#include "msmhwiobase.h"
#include "HALhwio.h"
#include "Chipinfo.h"


/*=========================================================================
      Data Definitions
==========================================================================*/
/*
 * ChipFamilyArray0
 *
 * Array of ChipFamilies with Family Number 0.
 */
ChipInfoFamilyType ChipFamilyArray0[] =
{
  CHIPINFO_FAMILY_MSM8974,
  CHIPINFO_FAMILY_MDM9x25,
  CHIPINFO_FAMILY_MSM8x26,
  CHIPINFO_FAMILY_APQ8084,
  CHIPINFO_FAMILY_MSM8926,
  CHIPINFO_FAMILY_MSM8x62,
  CHIPINFO_FAMILY_MPQ8092,
  CHIPINFO_FAMILY_MDM9x35,
  CHIPINFO_FAMILY_MSM8974_PRO,
  CHIPINFO_FAMILY_FSM9900,
  CHIPINFO_FAMILY_UNKNOWN,
  CHIPINFO_FAMILY_MSM8994,
  CHIPINFO_FAMILY_FSM90xx,
  CHIPINFO_FAMILY_UNKNOWN,
  CHIPINFO_FAMILY_MSM8992,
};

/*
 * ChipFamilyArray1
 *
 * Array of ChipFamilies with Family Number 1.
 */
ChipInfoFamilyType ChipFamilyArray1[] =
{
  CHIPINFO_FAMILY_MSM8x10,
  
};

/*
 * ChipFamilyArray2
 *
 * Array of ChipFamilies with Family Number 2.
 */
ChipInfoFamilyType ChipFamilyArray2[] =
{
  CHIPINFO_FAMILY_MSM8916,
  CHIPINFO_FAMILY_MSM8936,
  CHIPINFO_FAMILY_MDM9x45,
  CHIPINFO_FAMILY_MSM8909,
  CHIPINFO_FAMILY_MSM8956,
  CHIPINFO_FAMILY_IPQ401x,
  CHIPINFO_FAMILY_MSM8929,
  CHIPINFO_FAMILY_MSM8952,
  CHIPINFO_FAMILY_MDM9x55,
  CHIPINFO_FAMILY_MDM9x07,
};

/*
 * ChipFamilyArray3
 *
 * Array of ChipFamilies with Family Number 3.
 */
ChipInfoFamilyType ChipFamilyArray3[] =
{
  CHIPINFO_FAMILY_MSM8996,
};

/*=========================================================================
      Macros
==========================================================================*/
#define CHIPINFO_FAMILY_ARRAY0_SIZE (sizeof(ChipFamilyArray0) / sizeof(ChipFamilyArray0[0]))
#define CHIPINFO_FAMILY_ARRAY1_SIZE (sizeof(ChipFamilyArray1) / sizeof(ChipFamilyArray1[0]))
#define CHIPINFO_FAMILY_ARRAY2_SIZE (sizeof(ChipFamilyArray2) / sizeof(ChipFamilyArray2[0]))
#define CHIPINFO_FAMILY_ARRAY3_SIZE (sizeof(ChipFamilyArray3) / sizeof(ChipFamilyArray3[0]))

/* =========================================================================
**  Function : Chipinfo_GetFamily
** =========================================================================*/

ChipInfoFamilyType Chipinfo_GetFamily (void)
{
  ChipInfoFamilyType ChipFamily = CHIPINFO_FAMILY_UNKNOWN;
  uint32 ChipFamilyNum = 0;
  uint32 ChipDeviceNum = 0;

  ChipFamilyNum = (HWIO_TCSR_SOC_HW_VERSION_INM(
            HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_BMSK))
           >>HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_SHFT;

  ChipDeviceNum = (HWIO_TCSR_SOC_HW_VERSION_INM(
            HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_BMSK))
           >>HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_SHFT;


  switch (ChipFamilyNum)
  {
    case 0:  
    {
      if (ChipDeviceNum >= CHIPINFO_FAMILY_ARRAY0_SIZE)
      {
        ChipFamily =  CHIPINFO_FAMILY_UNKNOWN;
      }
      else
      {
        ChipFamily =  ChipFamilyArray0[ChipDeviceNum]; 
      }
      break;
    }
    case 1:
    {
      if (ChipDeviceNum >= CHIPINFO_FAMILY_ARRAY1_SIZE)
      {
        ChipFamily =  CHIPINFO_FAMILY_UNKNOWN;
      }
      else
      {
        ChipFamily =  ChipFamilyArray1[ChipDeviceNum]; 
      }
      break;
    }
    case 2:
    {
      if (ChipDeviceNum >= CHIPINFO_FAMILY_ARRAY2_SIZE)
      {
        ChipFamily =  CHIPINFO_FAMILY_UNKNOWN;
      }
      else
      {
        ChipFamily =  ChipFamilyArray2[ChipDeviceNum]; 
      }
      break;
    }
	case 3:
    {
      if (ChipDeviceNum >= CHIPINFO_FAMILY_ARRAY3_SIZE)
      {
        ChipFamily =  CHIPINFO_FAMILY_UNKNOWN;
      }
      else
      {
        ChipFamily =  ChipFamilyArray3[ChipDeviceNum]; 
      }
      break;
    }
    default: 
    {
      ChipFamily =  CHIPINFO_FAMILY_UNKNOWN; 
      break;
    }
  }

  return ChipFamily;

} /* END Chipinfo_GetFamily */


ChipInfoVersionType Chipinfo_GetVersion (void)
{
  ChipInfoVersionType ChipVersion;

  ChipVersion = CHIPINFO_VERSION(
     ((HWIO_TCSR_SOC_HW_VERSION_INM(
            HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_BMSK))
           >>HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_SHFT), 
      ((HWIO_TCSR_SOC_HW_VERSION_INM(
            HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_BMSK))
           >>HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_SHFT));
  return ChipVersion;

} /* END Chipinfo_GetVersion */
