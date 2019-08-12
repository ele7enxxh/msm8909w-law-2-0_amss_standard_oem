/*
==============================================================================

FILE:         HALclkGenericPLL.c

DESCRIPTION:
  This file contains the generic clock HAL code for the PLL control.
  These PLLs are of the SR, SR2 and HF PLL variety.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/hal/clk/hw/v1/HALclkGenericPLL.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
09/04/12   lil     Modified enabling sequence for SR and SR2 PLLs
02/24/11   gfr     Created

==============================================================================
            Copyright (c) 2015 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/

#include "HALhwio.h"
#include "HALclkGenericPLL.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Definitions
** ==========================================================================*/


/* ============================================================================
**    Data Declarations
** ==========================================================================*/


/* ============================================================================
**    External Definitions
** ==========================================================================*/


extern HAL_clk_SourceControlType HAL_clk_SRPLLControl;
extern HAL_clk_SourceControlType HAL_clk_StromerPLLControl;


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_InstallPLL
**
** ======================================================================== */

void HAL_clk_InstallPLL
(
  HAL_clk_SourceType      eSource,
  HAL_clk_PLLContextType *pCtxt,
  uint32                  nBaseAddress
)
{
  /*
   * Add base address
   */
  pCtxt->nAddr += nBaseAddress;

  if (pCtxt->VoterRegister.nAddr != 0)
  {
    pCtxt->VoterRegister.nAddr += nBaseAddress;
  }

  /*
   * Install PLL handler
   */
  if ((pCtxt->ePLLType == HAL_CLK_PLL_SR) ||
      (pCtxt->ePLLType == HAL_CLK_PLL_SR2))
  {
    HAL_clk_InstallSource(eSource, &HAL_clk_SRPLLControl, pCtxt);
  }
  else if (pCtxt->ePLLType == HAL_CLK_PLL_STROMER)
  {
    HAL_clk_InstallSource(eSource, &HAL_clk_StromerPLLControl, pCtxt);
  }
  else
  {
    // error but no return parameter to provide!
  }

} /* END HAL_clk_InstallPLL */
