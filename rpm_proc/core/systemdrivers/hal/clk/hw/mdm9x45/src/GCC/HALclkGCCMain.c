/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/hal/clk/hw/mdm9x45/src/GCC/HALclkGCCMain.c#1 $

when         who     what, where, why
----------   ---     ----------------------------------------------------------- 
06/05/2014           Auto-generated.


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


/*
 * Clock domains
 */
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSDAPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACECLKINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRDIV4ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV8ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTSCTRPREDIV16ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRPMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQPICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCIPAClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRMXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMBIMCCY4XClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMDEBUGCY4XClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMFFClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMPNOCCYClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMRPMCYClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMSNOCCY2XClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPDMULTAUDIOAHBFABRICCYClkDomain;


/*
 * Power domains
 */
extern HAL_clk_PowerDomainDescType HAL_clk_mGCCBIMCPowerDomain;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aGCCSourceMap
 *
 * GCC HW source mapping
 * 
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 * 
 */
HAL_clk_SourceMapType aGCCSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,    0},
  {HAL_CLK_SOURCE_GPLL0, 1},
  {HAL_CLK_SOURCE_GPLL2, 2},
  {HAL_CLK_SOURCE_GPLL3, 3},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/*
 * HAL_clk_mGCCClockDomainControl
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * HAL_clk_aGCCClockDomainDesc
 *
 * List of GCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aGCCClockDomainDesc [] =
{
  &HAL_clk_mGCCBIMCDDRClkDomain,
  &HAL_clk_mGCCPDM2ClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  &HAL_clk_mGCCQDSSATClkDomain,
  &HAL_clk_mGCCQDSSDAPClkDomain,
  &HAL_clk_mGCCQDSSSTMClkDomain,
  &HAL_clk_mGCCQDSSTRACECLKINClkDomain,
  &HAL_clk_mGCCQDSSTSCTRClkDomain,
  &HAL_clk_mGCCQDSSTSCTRPREDIV3ClkDomain,
  &HAL_clk_mGCCQDSSTSCTRDIV4ClkDomain,
  &HAL_clk_mGCCQDSSTSCTRPREDIV8ClkDomain,
  &HAL_clk_mGCCQDSSTSCTRPREDIV16ClkDomain,
  &HAL_clk_mGCCRBCPRClkDomain,
  &HAL_clk_mGCCRPMClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  &HAL_clk_mGCCQPICClkDomain,
  &HAL_clk_mGCCIPAClkDomain,
  &HAL_clk_mGCCRBCPRMXClkDomain,
  &HAL_clk_mGCCSPDMBIMCCY4XClkDomain,
  &HAL_clk_mGCCSPDMDEBUGCY4XClkDomain,
  &HAL_clk_mGCCSPDMFFClkDomain,
  &HAL_clk_mGCCSPDMPNOCCYClkDomain,
  &HAL_clk_mGCCSPDMRPMCYClkDomain,
  &HAL_clk_mGCCSPDMSNOCCY2XClkDomain,
  &HAL_clk_mGCCSPDMULTAUDIOAHBFABRICCYClkDomain,
  NULL
};


/*
 * HAL_clk_aGCCPowerDomainDesc
 *
 * List of GCC power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aGCCPowerDomainDesc [] =
{
  &HAL_clk_mGCCBIMCPowerDomain,
  NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitGCCMain
**
** ======================================================================== */

void HAL_clk_PlatformInitGCCMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aGCCClockDomainDesc, CLK_CTL_BASE);

  /*
   * Install all power domains
   */
  HAL_clk_InstallPowerDomains(HAL_clk_aGCCPowerDomainDesc, CLK_CTL_BASE);

} /* END HAL_clk_PlatformInitGCCMain */

