/*
==============================================================================

FILE:         HALldo.c

DESCRIPTION:
  Common code for the LDO HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/ldo/src/HALldo.c#1 $

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
#include "HALldoInternal.h"
#include <HALhwio.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Typedefs
** ==========================================================================*/

/*
 * HAL_ldo_Type
 *
 * Consolidated set of internal static data for the ldo HAL.
 *
 * aLDODesc    - Array of LDO descriptors.
 * mHALldoCtxt - HAL LDO driver context data
 */
typedef struct
{
  HAL_ldo_LDODescType aLDODesc[HAL_LDO_NUM_OF_LDOS];
  HAL_ldo_ContextType mHALldoCtxt;
} HAL_ldo_Type;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_ldo
 *
 * Consolidated static data for the LDO HAL.
 *
 * We fill in some non-zero data to ensure that this ends up in the RW
 * segment instead of ZI since the LDO HAL is used in the bootloader
 * before the ZI is cleared.
 */
static HAL_ldo_Type HAL_ldo;


/*
 * HAL_ldo_Platform
 *
 * External reference to the platform configuration data.
 */
extern HAL_ldo_PlatformType HAL_ldo_Platform;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_ldo_Init
**
** ======================================================================== */

void HAL_ldo_Init
(
  HAL_ldo_ContextType *pHALldoCtxt
)
{
  HAL_ldo_InitFuncType fInit;
  uint32 n = 0;

  /*
   * Sanity check
   */
  if (pHALldoCtxt != NULL)
  {
     /*
      * Save the busy wait function pointer
      */
     HAL_ldo.mHALldoCtxt.BusyWait = pHALldoCtxt->BusyWait;

     /*
      * Save the chip info
      */
     HAL_ldo.mHALldoCtxt.nChipVersion = pHALldoCtxt->nChipVersion;
     HAL_ldo.mHALldoCtxt.nChipId = pHALldoCtxt->nChipId;
     HAL_ldo.mHALldoCtxt.nChipFamily = pHALldoCtxt->nChipFamily;
  }

  /*
   * Init software state.
   */
  if (HAL_ldo_Platform.afInitFuncs != NULL)
  {
    while ((fInit = HAL_ldo_Platform.afInitFuncs[n++]) != NULL)
    {
      fInit();
    }
  }

} /* END HAL_ldo_Init */


/* ===========================================================================
**  HAL_ldo_GetHWIOBases
**
** ======================================================================== */

void HAL_ldo_GetHWIOBases
(
  HAL_ldo_HWIOBaseType **paHWIOBases
)
{
  *paHWIOBases = HAL_ldo_Platform.aHWIOBases;

} /* END HAL_ldo_GetHWIOBases */


/* ===========================================================================
**  HAL_ldo_GetChipVersion
**
** ======================================================================== */

uint32 HAL_ldo_GetChipVersion
(
  void
)
{
  return HAL_ldo.mHALldoCtxt.nChipVersion;

} /* END HAL_ldo_GetChipVersion */


/* ===========================================================================
**  HAL_ldo_GetChipId
**
** ======================================================================== */

uint32 HAL_ldo_GetChipId
(
  void
)
{
  return HAL_ldo.mHALldoCtxt.nChipId;

} /* END HAL_ldo_GetChipId */


/* ===========================================================================
**  HAL_ldo_GetChipFamily
**
** ======================================================================== */

uint32 HAL_ldo_GetChipFamily
(
  void
)
{
  return HAL_ldo.mHALldoCtxt.nChipFamily;

} /* END HAL_ldo_GetChipFamily */


/* ===========================================================================
**  HAL_ldo_InstallLDO
**
** ======================================================================== */

void HAL_ldo_InstallLDO
(
  HAL_ldo_LDOType         eLDO,
  HAL_ldo_LDOControlType *pControl
)
{
  /*
   * Sanity.
   */
  if ((eLDO >= HAL_LDO_NUM_OF_LDOS) || (pControl == NULL))
  {
    return;
  }

  /*
   * Fill in the pointers in the LDO descriptor.
   */
  HAL_ldo.aLDODesc[eLDO].eLDO = eLDO;
  HAL_ldo.aLDODesc[eLDO].pmControl = pControl;

} /* END HAL_ldo_InstallLDO */


/* ===========================================================================
**  HAL_ldo_ConfigLDO
**
** ======================================================================== */

void HAL_ldo_ConfigLDO
(
  HAL_ldo_LDOType eLDO
)
{
  if (eLDO < HAL_LDO_NUM_OF_LDOS &&
      HAL_ldo.aLDODesc[eLDO].pmControl != NULL &&
      HAL_ldo.aLDODesc[eLDO].pmControl->Config != NULL)
  {
    HAL_ldo.aLDODesc[eLDO].pmControl->Config();
  }

} /* END HAL_ldo_ConfigLDO */


/* ===========================================================================
**  HAL_ldo_EnableLDO
**
** ======================================================================== */

void HAL_ldo_EnableLDO
(
  HAL_ldo_LDOType eLDO
)
{
  if (eLDO < HAL_LDO_NUM_OF_LDOS &&
      HAL_ldo.aLDODesc[eLDO].pmControl != NULL &&
      HAL_ldo.aLDODesc[eLDO].pmControl->Enable != NULL)
  {
    HAL_ldo.aLDODesc[eLDO].pmControl->Enable();
  }

} /* END HAL_ldo_EnableLDO */


/* ===========================================================================
**  HAL_ldo_DisableLDO
**
** ======================================================================== */

void HAL_ldo_DisableLDO
(
  HAL_ldo_LDOType eLDO
)
{
  if (eLDO < HAL_LDO_NUM_OF_LDOS &&
      HAL_ldo.aLDODesc[eLDO].pmControl != NULL &&
      HAL_ldo.aLDODesc[eLDO].pmControl->Disable != NULL)
  {
    HAL_ldo.aLDODesc[eLDO].pmControl->Disable();
  }

} /* END HAL_ldo_DisableLDO */


/* ===========================================================================
**  HAL_ldo_SetLDOVoltage
**
** ======================================================================== */

void HAL_ldo_SetLDOVoltage
(
  HAL_ldo_LDOType eLDO,
  uint32          nVoltageUV
)
{
  if (eLDO < HAL_LDO_NUM_OF_LDOS &&
      HAL_ldo.aLDODesc[eLDO].pmControl != NULL &&
      HAL_ldo.aLDODesc[eLDO].pmControl->SetVoltage != NULL)
  {
    HAL_ldo.aLDODesc[eLDO].pmControl->SetVoltage(nVoltageUV);
  }

} /* END HAL_ldo_SetLDOVoltage */


/* ===========================================================================
**  HAL_ldo_SetLDORetentionVoltage
**
** ======================================================================== */

void HAL_ldo_SetLDORetentionVoltage
(
  HAL_ldo_LDOType eLDO,
  uint32          nVoltageUV
)
{
  if (eLDO < HAL_LDO_NUM_OF_LDOS &&
      HAL_ldo.aLDODesc[eLDO].pmControl != NULL &&
      HAL_ldo.aLDODesc[eLDO].pmControl->SetRetentionVoltage != NULL)
  {
    HAL_ldo.aLDODesc[eLDO].pmControl->SetRetentionVoltage(nVoltageUV);
  }

} /* END HAL_ldo_SetLDORetentionVoltage */


/* ===========================================================================
**  HAL_ldo_BusyWait
**
** ======================================================================== */

void HAL_ldo_BusyWait
(
  uint32 nTimeUS
)
{
  HAL_ldo.mHALldoCtxt.BusyWait(nTimeUS);

}

