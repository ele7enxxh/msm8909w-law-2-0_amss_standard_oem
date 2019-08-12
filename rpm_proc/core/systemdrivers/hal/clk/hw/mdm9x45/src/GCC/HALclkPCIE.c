/*
==============================================================================

FILE:         HALclkPCIE.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   PCIE clocks.

   List of clock domains:


   List of power domains:
     - HAL_clk_mGCCPCIEPowerDomain



==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkPCIE.c#1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
07/15/2014           Auto-generated.


==============================================================================
            Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_mGCCPCIEPowerDomain
 *
 * PCIE power domain.
 */
HAL_clk_PowerDomainDescType HAL_clk_mGCCPCIEPowerDomain =
{
  /* .szPowerDomainName       = */ "VDD_PCIE",
  /* .nGDSCRAddr              = */ HWIO_OFFS(GCC_PCIE_GDSCR),
  /* .pmControl               = */ &HAL_clk_mGenericPowerDomainControl,
  /* .pmNextPowerDomain       = */ NULL
};

