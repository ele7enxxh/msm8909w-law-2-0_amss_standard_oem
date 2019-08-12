/*
==============================================================================

FILE:         HALldoQ6.c

DESCRIPTION:
   This file contains the LDO HAL code for the QDSP6 core clock.


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/ldo/hw/msm8909/src/mss/HALldoQ6.c#1 $

when          who     what, where, why
--------      ---     ----------------------------------------------------------- 


==============================================================================
            Copyright (c) 2014 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>
#include "HALldoInternal.h"
#include "HALldoHWIO.h"


/* ============================================================================
**    Prototypes
** ==========================================================================*/

static void HAL_ldo_Q6LDOConfig              ( void );
static void HAL_ldo_Q6LDOEnable              ( void );
static void HAL_ldo_Q6LDODisable             ( void );
static void HAL_ldo_Q6LDOSetVoltage          ( uint32 nLDOVoltageUV );
static void HAL_ldo_Q6LDOSetRetentionVoltage ( uint32 nLDOVoltageUV );


/*=========================================================================
      Global Variables
==========================================================================*/


/*=========================================================================
      Macros
==========================================================================*/

/*
 * Mask for setting a bit in LDO_CTL1 when switching from BHS to LDO mode.
 */
#define HAL_LDO_CTL1_ENABLE_CONFIG  0x40

/*
 * Min/Max voltage values for the LDO operation range.
 * These voltages need to be converted to LDO reg format when programmed.
 */
#define HAL_LDO_VREF_MIN_UV   465 * 1000
#define HAL_LDO_VREF_MAX_UV  1100 * 1000

/*
 * LDO voltage delta.
 */
#define HAL_LDO_DELTA(val1, val2)                                            \
  ((val1)>(val2)?(val1)-(val2):(val2)-(val1))

/*
 * LDO range checker.
 */
#define HAL_LDO_IN_RANGE(x)                                                  \
  ( ((x) >=HAL_LDO_VREF_MIN_UV) && ((x) <= HAL_LDO_VREF_MAX_UV) )

/*
 * LDO uV to LDO_REF register format.
 */
#define HAL_LDO_UV_MAP_TO_HW(x)                                              \
  ( ( ( (x) - HAL_LDO_VREF_MIN_UV ) / 1000 ) / 5 ) + 1

/*
 * Set wait time to the equivalent of 5 us per 10 mV.
 * This value is expected to change when data becomes available.
 */
#define HAL_LDO_SETTLE_DELAY(start, end)                                     \
  HAL_ldo_BusyWait(5 * ( ( ( HAL_LDO_DELTA(start, end) + (10000 - 1) ) / 10000) ) )


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_ldo_mQ6LDOControl
 *
 * Functions for controlling the Q6 LDO
 */
HAL_ldo_LDOControlType HAL_ldo_mQ6LDOControl =
{
  /* .Config              = */ HAL_ldo_Q6LDOConfig,
  /* .Enable              = */ HAL_ldo_Q6LDOEnable,
  /* .Disable             = */ HAL_ldo_Q6LDODisable,
  /* .SetVoltage          = */ HAL_ldo_Q6LDOSetVoltage,
  /* .SetRetentionVoltage = */ HAL_ldo_Q6LDOSetRetentionVoltage
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* =========================================================================
**  HAL_ldo_Q6LDOConfig
** =========================================================================*/
/**
  Program the Q6 LDO config registers.

  @return
  None.

  @dependencies
  None.
*/
static void HAL_ldo_Q6LDOConfig
(
  void
)
{
  /*-----------------------------------------------------------------------*/
  /* Program the LDO_CFG registers per recommendation from HW.             */
  /*-----------------------------------------------------------------------*/

  HWIO_OUT(MSS_QDSP6SS_LDO_CFG0, 0x1004000);
  HWIO_OUT(MSS_QDSP6SS_LDO_CFG1, 0x50);
  HWIO_OUT(MSS_QDSP6SS_LDO_CFG2, 0xB068);

}


/* ===========================================================================
**  HAL_ldo_Q6LDOEnable
**
** ======================================================================== */
/**
  Enable the MSS Q6 LDO.

  This function is used to switch to LDO mode.  If already in LDO mode,
  it will simply return.

  @return
  None.

  @dependencies
  None.
*/

static void HAL_ldo_Q6LDOEnable
(
  void
)
{
  uint32 nQ6PowerCtl = HWIO_IN(MSS_QDSP6SS_PWR_CTL);
  uint32 nLDOCTL1Val;

  /*-----------------------------------------------------------------------*/
  /* Only switch to LDO if in BHS mode.                                    */
  /*-----------------------------------------------------------------------*/

  if(nQ6PowerCtl & HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, BHS_ON))
  {
    /*
     * The mode bit inside LDO_CTL1 needs to be set during the switch.
     */
    nLDOCTL1Val = HWIO_INF(MSS_QDSP6SS_LDO_CFG0, LDO_CTL1);
    HWIO_OUTF(
      MSS_QDSP6SS_LDO_CFG0,
      LDO_CTL1,
      nLDOCTL1Val | HAL_LDO_CTL1_ENABLE_CONFIG);

    /*
     * Turn on LDO by setting LDO_PWR_UP.
     */
    nQ6PowerCtl |= HWIO_MSS_QDSP6SS_PWR_CTL_LDO_PWR_UP_BMSK; 

    HWIO_OUT(MSS_QDSP6SS_PWR_CTL, nQ6PowerCtl);

    /*
     * Wait for 10 us.
     */
    HAL_ldo_BusyWait(10);

    /*
     * Turn off BHS and LDO Bypass.
     */
    nQ6PowerCtl &= ~HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, BHS_ON);
    nQ6PowerCtl &= ~HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, LDO_BYP);

    HWIO_OUT(MSS_QDSP6SS_PWR_CTL, nQ6PowerCtl);

    /*
     * Clear the mode bit after switching to LDO mode from BHS.
     */
    HWIO_OUTF(MSS_QDSP6SS_LDO_CFG0, LDO_CTL1, nLDOCTL1Val);
  }

}


/* =========================================================================
**  HAL_ldo_Q6LDODisable
** =========================================================================*/
/**
  Turn on off the MSS Q6 LDO.

  This function is used to switch to BHS mode.  If already in BHS mode,
  it will simply return.

  @return
  None.

  @dependencies
  None.
*/
static void HAL_ldo_Q6LDODisable
(
  void
)
{
  uint32 nQ6PowerCtl = HWIO_IN(MSS_QDSP6SS_PWR_CTL);

  /*-----------------------------------------------------------------------*/
  /* Only switch to BHS if we aren't already in BHS mode.                  */
  /*-----------------------------------------------------------------------*/

  if ((nQ6PowerCtl & HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, BHS_ON)) == 0)
  {
    /*
     * Turn on BHS from LDO mode.
     */
    nQ6PowerCtl |= HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, BHS_ON);
    HWIO_OUT(MSS_QDSP6SS_PWR_CTL, nQ6PowerCtl);

    /*
     * Wait 1 uS for the BHS to turn on.
     */
    HAL_ldo_BusyWait(1);

    /*
     * Put LDO in bypass.
     */
    nQ6PowerCtl |= HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, LDO_BYP);
    HWIO_OUT(MSS_QDSP6SS_PWR_CTL, nQ6PowerCtl);

    /*
     * Turn off LDO.
     */
    nQ6PowerCtl &= ~HWIO_FMSK(MSS_QDSP6SS_PWR_CTL, LDO_PWR_UP);
    HWIO_OUT(MSS_QDSP6SS_PWR_CTL, nQ6PowerCtl);
  }

} /* HAL_ldo_LDODisable */


/* =========================================================================
**  HAL_ldo_Q6LDODSetVoltage
** =========================================================================*/
/**
  Set the MSS Q6 LDO to required voltage.

  This function does not enable or disable the LDO. It simply updates the
  voltage.

  @return
  None.

  @dependencies
  None.
*/
static void HAL_ldo_Q6LDOSetVoltage
(
  uint32 nLDOVoltageUV
)
{
  uint32 nLDOVoltageData;

  /*
   * Program the new operating voltage data to the LDO VREF register.
   */
  nLDOVoltageData = HAL_LDO_UV_MAP_TO_HW(nLDOVoltageUV);
  HWIO_OUTF(MSS_QDSP6SS_LDO_VREF_SET, VREF_LDO, nLDOVoltageData);

}


/* =========================================================================
**  HAL_ldo_Q6LDOSetRetentionVoltage
** =========================================================================*/
/**
  Set the MSS Q6 LDO retention voltage.

  @return
  None.

  @dependencies
  None.
*/
static void HAL_ldo_Q6LDOSetRetentionVoltage
(
  uint32 nLDOVoltageUV
)
{
  uint32 nLDOVoltageData;

  /*
   * Program the new operating voltage data to the LDO VREF register.
   */
  nLDOVoltageData = HAL_LDO_UV_MAP_TO_HW(nLDOVoltageUV);
  HWIO_OUTF(MSS_QDSP6SS_LDO_VREF_SET, VREF_RET, nLDOVoltageData);

}


/* ===========================================================================
**  HAL_ldo_PlatformInitQ6
**
** ======================================================================== */

void HAL_ldo_PlatformInitQ6
(
  void
)
{

  /*
   * Install all MSS LDOs.
   */
  HAL_ldo_InstallLDO(HAL_LDO_MSS_Q6, &HAL_ldo_mQ6LDOControl);

} /* END HAL_ldo_PlatformInitMSSMain */

