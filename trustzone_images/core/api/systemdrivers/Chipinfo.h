#ifndef CHIPINFO_H
#define CHIPINFO_H
/*===========================================================================

                         Chipinfo Header File

DESCRIPTION
  This header file contains the public API for the chipinfo driver.

      Copyright (c) 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/Chipinfo.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/12   pbitra  Initial version.
01/24/13   pbitra  Added support for Chipinfo_GetVersion

===========================================================================*/

#include "comdef.h"     /* Common Definitions and Types */


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif
/**
Chip Family type.  The family type of the chip we are running on.
Each family may include multiple chip ids.
*/
typedef enum
{ 
  CHIPINFO_FAMILY_UNKNOWN     = 0,
  CHIPINFO_FAMILY_MSM8974     = 32,
  CHIPINFO_FAMILY_MDM9x25     = 35,
  CHIPINFO_FAMILY_MSM8x10     = 38,
  CHIPINFO_FAMILY_MPQ8092     = 39,
  CHIPINFO_FAMILY_MSM8x26     = 40,
  CHIPINFO_FAMILY_APQ8084     = 43,
  CHIPINFO_FAMILY_APQ8x94     = CHIPINFO_FAMILY_APQ8084,
  CHIPINFO_FAMILY_MSM8x32     = 44,
  CHIPINFO_FAMILY_MDM9x35     = 45,
  CHIPINFO_FAMILY_MSM8974_PRO = 46,
  CHIPINFO_FAMILY_FSM9900     = 47,
  CHIPINFO_FAMILY_MSM8x62     = 48,
  CHIPINFO_FAMILY_MSM8926     = 49,
  CHIPINFO_FAMILY_MSM8994     = 50,
  CHIPINFO_FAMILY_IPQ8064     = 51,
  CHIPINFO_FAMILY_MSM8916     = 52,
  CHIPINFO_FAMILY_MSM8936     = 53,
  CHIPINFO_FAMILY_MDM9x45     = 54,
  CHIPINFO_FAMILY_APQ8096     = 55,
  CHIPINFO_FAMILY_MSM8996     = 56,
  CHIPINFO_FAMILY_MSM8992     = 57,
  CHIPINFO_FAMILY_MSM8909     = 58,
  CHIPINFO_FAMILY_FSM90xx     = 59,
  CHIPINFO_FAMILY_MDM9x07     = 60,
  CHIPINFO_FAMILY_MSM8956     = 61,
  CHIPINFO_FAMILY_IPQ401x     = 62,
  CHIPINFO_FAMILY_MSM8929     = 63,
  CHIPINFO_FAMILY_MSM8952     = 64,
  CHIPINFO_FAMILY_MDM9x55     = 65,

  CHIPINFO_NUM_FAMILIES       = 66,
  CHIPINFO_FAMILY_32BITS      = 0x7FFFFFF
} ChipInfoFamilyType;

/**
 * Chip version type with major number in the upper 16 bits and minor
 * number in the lower 16 bits.  For example:
 *   1.0 -> 0x00010000
 *   2.3 -> 0x00020003
 * The CHIPINFO_VERSION macro should be used to generate the 
 * appropriate comparison value as the format is not guaranteed 
 * to remain unchanged. 
 */
typedef uint32 ChipInfoVersionType;

/*===========================================================================

                      MACROS

===========================================================================*/
/**
 * Macro to generate the hardware version number from a given major
 * and minor number.  For example to check if this is at least version 1.2:
 *   ChipInfo_GetVersion(h, &version);
 *   if (version >= CHIPINFO_VERSION(1,2)) ...
 */
#define CHIPINFO_VERSION(major, minor)  (((major) << 16) | (minor))

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*==========================================================================

  FUNCTION      CHIPINFO_GETFAMILY

  DESCRIPTION   This function retrieves the Chip Family
                from the SOC_HW_VERSION register.

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Chip Family

  SIDE EFFECTS  None.

==========================================================================*/

ChipInfoFamilyType Chipinfo_GetFamily (void);


/*==========================================================================

  FUNCTION      CHIPINFO_GETVERSION

  DESCRIPTION   This function retrieves the Chip Version
                from the SOC_HW_VERSION register.

  PARAMETERS    None

  DEPENDENCIES  None.

  RETURN VALUE  Chip Version

  SIDE EFFECTS  None.

==========================================================================*/
ChipInfoVersionType Chipinfo_GetVersion (void);

#ifdef __cplusplus
}
#endif

#endif /* CHIPINFO_H */

