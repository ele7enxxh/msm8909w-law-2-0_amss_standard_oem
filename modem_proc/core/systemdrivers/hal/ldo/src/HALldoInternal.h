#ifndef __HALLDOINTERNAL_H__
#define __HALLDOINTERNAL_H__
/*
==============================================================================

FILE:         HALldoInternal.h

DESCRIPTION:
  Internal definitions for the ldo HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/ldo/src/HALldoInternal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
01/22/14   lil     Created.

==============================================================================
            Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALldo.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_ldo_InitFuncType
 *
 * Prototype for a generic module initialization function.
 */
typedef void (*HAL_ldo_InitFuncType)(void);


/*
 * HAL_ldo_PlatformType
 *
 * Platform information structure.  Each image defines one of these and
 * the core code uses it during initialization.
 *
 * afInitFuncs - Array of initialization functions.
 * aHWIOBases  - Array of HWIO bases required for this platform.
 */
typedef struct
{
  HAL_ldo_InitFuncType *afInitFuncs;
  HAL_ldo_HWIOBaseType *aHWIOBases;
} HAL_ldo_PlatformType;


/*
 * HAL_ldo_LDOControlType
 *
 * Set of function pointers used to control an LDO.
 *
 * Config              - Program the LDO's config registers.
 * Enable              - Enable the given LDO.
 * Disable             - Disable the given LDO.
 * SetVoltage          - Set LDO voltage.
 * SetRetentionVoltage - Set LDO retention voltage.
 */
typedef struct
{
  void (*Config)              (void);
  void (*Enable)              (void);
  void (*Disable)             (void);
  void (*SetVoltage)          (uint32 nLDOVoltageUV);
  void (*SetRetentionVoltage) (uint32 nLDOVoltageUV);
} HAL_ldo_LDOControlType;


/*
 * HAL_ldo_LDODescType
 *
 * Description of an LDO, including control functions.
 *
 * eLDO      - CPU enum identifier.
 * pmControl - Pointer to the control functions for this source.  If NULL
 *             then the LDO is not supported.
 */
typedef struct HAL_ldo_LDODescType
{
  HAL_ldo_LDOType         eLDO;
  HAL_ldo_LDOControlType *pmControl;
} HAL_ldo_LDODescType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


void HAL_ldo_InstallLDO
(
  HAL_ldo_LDOType         eLDO,
  HAL_ldo_LDOControlType *pControl
);


void HAL_ldo_BusyWait
(
  uint32 nTimeUS
);


#endif /* !HALLDOINTERNAL_H */

