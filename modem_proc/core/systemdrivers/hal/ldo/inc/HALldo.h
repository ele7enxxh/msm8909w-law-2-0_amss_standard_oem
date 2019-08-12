#ifndef __HALLDO_H__
#define __HALLDO_H__
/*
==============================================================================

FILE:         HALldo.h

DESCRIPTION:
  Common includes for the LDO HAL module.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/ldo/inc/HALldo.h#1 $

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


#include <HALcomdef.h>


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/* ============================================================================
**    Typedefs
** ==========================================================================*/


/*
 * HAL_ldo_LDOType
 */
typedef enum
{
  HAL_LDO_MSS_Q6,

  HAL_LDO_NUM_OF_LDOS,
  HAL_ENUM_32BITS(LDO_LDO)
} HAL_ldo_LDOType;


/*
 * HAL_ldo_HWIOBaseType
 *
 * Container for a HWIO base address.  This is used by the driver to
 * map the required physical bases to the provided base pointer.
 *
 *  nPhysAddr  - Physical base address of the base.
 *  nSize      - Size of the memory region in bytes.
 *  pnBaseAddr - Pointer to location for the driver to fill in with
 *               the virtual address.
 */
typedef struct
{
  uint32  nPhysAddr;
  uint32  nSize;
  uint32 *pnBaseAddr;
} HAL_ldo_HWIOBaseType;


/*
 * LDO handle type
 * NOTE: this can also be typedef uint32 * HAL_ldo_LDOHandleType
 */
typedef void *HAL_ldo_LDOHandleType;


/*
 * HAL_ldo_Context
 *
 * HAL ldo driver context data.
 *
 *  BusyWait     - Pointer to busy wait function.
 *                 Assumption: Will be implemented or passed down by the
 *                 top level driver. If this function pointer is NULL then
 *                 HAL modules that require a wait/pause may fail - this
 *                 will be functional failure and not compile time failure.
 *
 *  nChipVersion - Version of the chip. Compare against 
 *                 HAL_LDO_CHIP_VERSION(major, minor).
 *  nChipId      - ID of the chip.
 *  nChipFamily  - Family of the chip.
 */
typedef struct
{
  void (*BusyWait) (uint32 uSeconds);
  uint32 nChipVersion;
  uint32 nChipId;
  uint32 nChipFamily;
} HAL_ldo_ContextType;


/*============================================================================

               FUNCTION DECLARATIONS FOR MODULE

============================================================================*/


/* ============================================================================
**    Mandatory Functions
** ==========================================================================*/


/*=========================================================================
**  Function : HAL_ldo_Init
** =========================================================================*/
/**
  Initialize the LDO HAL. 

  @param pHALldoCtxt [in] HAL LDO context.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_Init
(
  HAL_ldo_ContextType *pHALldoCtxt
);


/*=========================================================================
**  Function : HAL_ldo_GetHWIOBases
** =========================================================================*/
/**
  Populate the HWIO base addresses. 

  @param paHWIOBases [out] Pointer to array HWIO bases.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_GetHWIOBases
(
  HAL_ldo_HWIOBaseType **paHWIOBases
);


/*=========================================================================
**  Function : HAL_ldo_ConfigLDO
** =========================================================================*/
/**
  Program a LDO's config registers.

  @param eLDO [in] LDO enum identifier.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_ConfigLDO
(
  HAL_ldo_LDOType eLDO
);


/*=========================================================================
**  Function : HAL_ldo_EnableLDO
** =========================================================================*/
/**
  Turn on a LDO. 

  @param eLDO [in] LDO enum identifier.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_EnableLDO
(
  HAL_ldo_LDOType eLDO
);


/*=========================================================================
**  Function : HAL_ldo_DisableLDO
** =========================================================================*/
/**
  Turn off a LDO. 

  @param eLDO [in] LDO enum identifier.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_DisableLDO
(
  HAL_ldo_LDOType eLDO
);


/*=========================================================================
**  Function : HAL_ldo_SetLDOVoltage
** =========================================================================*/
/**
  Configure an LDO to requested voltage.

  @param eLDO       [in] LDO enum identifier.
  @param nVoltageUV [in] Voltage in uV.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_SetLDOVoltage
(
  HAL_ldo_LDOType eLDO,
  uint32          nVoltageUV
);


/*=========================================================================
**  Function : HAL_ldo_SetLDORetentionVoltage
** =========================================================================*/
/**
  Program a LDO's retention voltage.

  @param eLDO       [in] LDO enum identifier.
  @param nVoltageUV [in] Voltage in uV.

  @return 
  Nothing.

  @dependencies
  None.
*/

void HAL_ldo_SetLDORetentionVoltage
(
  HAL_ldo_LDOType eLDO,
  uint32          nVoltageUV
);


/*=========================================================================
**  Function : HAL_ldo_GetChipVersion
** =========================================================================*/
/**
  Return the chip version.

  @return 
  Chip version.

  @dependencies
  None.
*/

uint32 HAL_ldo_GetChipVersion
(
  void
);


/*=========================================================================
**  Function : HAL_ldo_GetChipId
** =========================================================================*/
/**
  Return the chip ID.

  @return 
  Chip ID.

  @dependencies
  None.
*/

uint32 HAL_ldo_GetChipId
(
  void
);


/*=========================================================================
**  Function : HAL_ldo_GetChipFamily
** =========================================================================*/
/**
  Return the chip family.

  @return 
  Chip family.

  @dependencies
  None.
*/

uint32 HAL_ldo_GetChipFamily
(
  void
);


#endif  /* __HALLDO_H__ */ 

