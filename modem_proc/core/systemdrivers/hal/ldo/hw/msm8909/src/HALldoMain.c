/*
==============================================================================

FILE:         HALldoMain.c

DESCRIPTION:
  This file contains the main platform initialization code for the VCS
  HAL.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/ldo/hw/msm8909/src/HALldoMain.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
01/22/14   lil     Created.

==============================================================================
            Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include "HALldoInternal.h"
#include "HALhwio.h"
#include "HALldoHWIO.h"


/* ============================================================================
**    Externs
** ==========================================================================*/


extern void HAL_ldo_PlatformInitQ6(void);


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_ldo_aInitFuncs
 *
 * Declare array of module initialization functions.
 */
static HAL_ldo_InitFuncType HAL_ldo_afInitFuncs[] =
{
  /*
   * MSS
   */
  HAL_ldo_PlatformInitQ6,
  
  NULL
};


/*
 * Declare the base pointers for HWIO access.
 */
uint32 HAL_ldo_nHWIOBaseMSS;


/*
 * HAL_ldo_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_ldo_HWIOBaseType HAL_ldo_aHWIOBases[] =
{
  { MODEM_TOP_BASE_PHYS, MODEM_TOP_BASE_SIZE, &HAL_ldo_nHWIOBaseMSS },
  { 0, 0, NULL }
};


/*
 * HAL_ldo_Platform;
 * Platform data.
 */
HAL_ldo_PlatformType HAL_ldo_Platform =
{
  HAL_ldo_afInitFuncs,
  HAL_ldo_aHWIOBases
};

