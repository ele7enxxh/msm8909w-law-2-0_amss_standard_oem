#ifndef HAL_MPM_PLATFORM_H
#define HAL_MPM_PLATFORM_H
/*
===========================================================================

FILE:         HALmpmPlatform.h

DESCRIPTION:  
  This is the platform hardware abstraction layer implementation for the
  MPM interrupt controller block.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/mpm/hal/source/MPM_V4/HALmpmPlatform.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
04/09/12   mda     MPMv2 for 8974

===========================================================================
             Copyright © 2009 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

void HAL_mpm_PlatformInitEarly (HAL_mpm_PlatformType *pmPlatform);

void HAL_mpm_true_sleep_xtal_set ( void );

/*void HAL_mpm_CfgVoltageRailSleep (...);

void HAL_mpm_CfgCXOSleep (...);

void HAL_mpm_ClearSleepModes (...);

void HAL_mpm_DisableCXO (...);

void HAL_mpm_EnableCXO (...);*/

/*
===============================================================================
  INTERFACES DOCUMENTATION
==============================================================================*/

/*==============================================================================
Function: HAL_mpm_PlatformInitEarly()
   FIXME: This procedure should have its description filled in here.

Parameters:
   pmPlatform: FIXME: This is for something.

Return value: None

Dependencies: FIXME: anything?

Side effects: FIXME: anything?

See Also: FIXME: anything?
==============================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* HAL_MPM_INTERNAL_H */

